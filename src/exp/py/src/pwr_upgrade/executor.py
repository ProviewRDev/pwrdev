#!/usr/bin/python3
#
# ProviewR   Open Source Process Control.
# Copyright (C) 2005-2026 SSAB EMEA AB.
#
# This file is part of ProviewR.
#
# This program is free software; you can redistribute it and/or 
# modify it under the terms of the GNU General Public License as 
# published by the Free Software Foundation, either version 2 of 
# the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful 
# but WITHOUT ANY WARRANTY; without even the implied warranty of 
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License 
# along with ProviewR. If not, see <http://www.gnu.org/licenses/>
#

"""
Command execution engine for ProviewR upgrade tool.
"""

import os
import glob
import importlib.util
import shlex
import subprocess
import time
from pathlib import Path
from typing import Optional, List, Callable, Tuple

from .utils import (
    ProjectEnvironment,
    CommandError,
    UpgradeError,
    save_file_with_backup,
    get_volume_name_from_load_file,
    ensure_directory,
    translate_filename,
)
from .steps import UpgradeStep, StepStatus, StepRunner, StepScope
from .config import UpgradeConfig


class StepExecutor:
    """Executes upgrade steps."""
    
    def __init__(
        self,
        env: ProjectEnvironment,
        config: UpgradeConfig,
        output_callback: Callable[[str], None] = None,
    ):
        self.env = env
        self.config = config
        self.output_callback = output_callback or print
    
    def log(self, message: str) -> None:
        """Output a log message."""
        self.output_callback(message)
    
    def run_command(
        self,
        cmd: str,
        check: bool = True,
        capture: bool = True,
    ) -> Tuple[int, str, str]:
        """
        Run a shell command.
        
        Returns: (returncode, stdout, stderr)
        """
        self.log(f"$ {cmd}")
        
        if self.config.dry_run:
            self.log("[DRY RUN] Command not executed")
            return (0, "", "")
        
        try:
            result = subprocess.run(
                cmd,
                shell=True,
                capture_output=capture,
                text=True,
                errors="replace",
                env=os.environ,
            )
            
            if result.stdout:
                self.log(result.stdout)
            if result.stderr:
                self.log(result.stderr)
            
            if check and result.returncode != 0:
                raise CommandError(
                    f"Command failed with exit code {result.returncode}",
                    command=cmd,
                    returncode=result.returncode,
                )
            
            return (result.returncode, result.stdout or "", result.stderr or "")
            
        except subprocess.SubprocessError as e:
            raise CommandError(f"Failed to execute command: {e}", command=cmd)

    def run_command_args(
        self,
        args: List[str],
        check: bool = True,
        capture: bool = True,
    ) -> Tuple[int, str, str]:
        """Run a command without a shell, optionally streaming output."""
        cmd = shlex.join(args)
        self.log(f"$ {cmd}")

        if self.config.dry_run:
            self.log("[DRY RUN] Command not executed")
            return (0, "", "")

        try:
            result = subprocess.run(
                args,
                capture_output=capture,
                text=True,
                errors="replace",
                env=os.environ,
            )

            stdout = result.stdout or ""
            stderr = result.stderr or ""

            if stdout:
                self.log(stdout)
            if stderr:
                self.log(stderr)

            if check and result.returncode != 0:
                raise CommandError(
                    f"Command failed with exit code {result.returncode}",
                    command=cmd,
                    returncode=result.returncode,
                )

            return (result.returncode, stdout, stderr)

        except subprocess.SubprocessError as e:
            raise CommandError(f"Failed to execute command: {e}", command=cmd)
    
    def wb_cmd(self, args: str, volume: str = None, quiet: bool = True) -> Tuple[int, str, str]:
        """Run wb_cmd with given arguments."""
        cmd_parts = ["wb_cmd"]
        if quiet:
            cmd_parts.append("-q")
        if volume:
            cmd_parts.extend(["-v", volume])
        cmd_parts.append(args)

        return self.run_command_args(cmd_parts)

    def resolve_step_artifact(self, step: UpgradeStep) -> str:
        """Resolve a step artifact path from env vars or module-relative paths."""
        if not step.artifact:
            raise UpgradeError(f"Step {step.name} has no artifact configured", step=step.name)

        translated = translate_filename(step.artifact)
        if translated == step.artifact and step.artifact.startswith("$"):
            variable, sep, remainder = step.artifact[1:].partition("/")
            value = getattr(self.env, variable, None)
            if value:
                translated = os.path.join(value, remainder) if sep else value

        if translated != step.artifact or os.path.isabs(translated):
            return translated

        if step.source_file:
            return os.path.normpath(os.path.join(os.path.dirname(step.source_file), step.artifact))

        return translated

    def get_step_volumes(self, step: UpgradeStep) -> List[str]:
        """Get the target volume list for a step."""
        del step
        return self.env.databases

    def execute_wb_cmd_script_step(self, step: UpgradeStep) -> None:
        """Execute a wb_cmd script once or once per volume."""
        script_path = self.resolve_step_artifact(step)
        if not self.config.dry_run and not os.path.exists(script_path):
            raise UpgradeError(f"wb_cmd script not found: {script_path}", step=step.name)

        if step.scope == StepScope.PER_VOLUME:
            for db_name in self.get_step_volumes(step):
                self.log(f"-- Running {step.name} on volume {db_name}")
                self.wb_cmd(f'@"{script_path}"', volume=db_name)
            return

        self.log(f"-- Running {step.name}")
        self.wb_cmd(f'@"{script_path}"')

    def execute_binary_step(self, step: UpgradeStep) -> None:
        """Execute an installed binary once or once per volume."""
        tool_path = self.resolve_step_artifact(step)
        if not self.config.dry_run and not os.path.exists(tool_path):
            raise UpgradeError(f"Binary not found: {tool_path}", step=step.name)

        if step.scope == StepScope.PER_VOLUME:
            for db_name in self.get_step_volumes(step):
                self.log(f"-- Running {os.path.basename(tool_path)} on volume {db_name}")
                self.run_command_args([tool_path, db_name])
            return

        self.log(f"-- Running {os.path.basename(tool_path)}")
        self.run_command_args([tool_path])

    def execute_shell_step(self, step: UpgradeStep) -> None:
        """Execute an installed shell script once or once per volume."""
        script_path = self.resolve_step_artifact(step)
        if not self.config.dry_run and not os.path.exists(script_path):
            raise UpgradeError(f"Shell script not found: {script_path}", step=step.name)

        if step.scope == StepScope.PER_VOLUME:
            for db_name in self.get_step_volumes(step):
                self.log(f"-- Running {os.path.basename(script_path)} on volume {db_name}")
                self.run_command_args([script_path, db_name])
            return

        self.log(f"-- Running {os.path.basename(script_path)}")
        self.run_command_args([script_path])

    def execute_python_step(self, step: UpgradeStep) -> None:
        """Execute a Python helper module from the version-step package."""
        artifact = step.artifact or ""
        module_ref, _, callable_name = artifact.partition(":")
        callable_name = callable_name or "run"
        module_path = self.resolve_step_artifact(
            UpgradeStep(
                name=step.name,
                description=step.description,
                artifact=module_ref,
                source_file=step.source_file,
            )
        )

        if not self.config.dry_run and not os.path.exists(module_path):
            raise UpgradeError(f"Python step module not found: {module_path}", step=step.name)

        spec = importlib.util.spec_from_file_location(
            f"pwr_upgrade_step_{step.name}",
            module_path,
        )
        if spec is None or spec.loader is None:
            raise UpgradeError(f"Unable to load Python step module: {module_path}", step=step.name)

        module = importlib.util.module_from_spec(spec)
        spec.loader.exec_module(module)

        runner = getattr(module, callable_name, None)
        if runner is None:
            raise UpgradeError(
                f"Python step callable not found: {callable_name} in {module_path}",
                step=step.name,
            )

        runner(self, step)
    
    # =========================================================================
    # Step Implementations
    # =========================================================================
    
    def execute_dumpdb(self, step: UpgradeStep) -> None:
        """Dump all databases to text files."""
        databases = self.env.databases
        
        if not databases:
            self.log("No databases found to dump")
            return
        
        # Remove existing dump files
        existing_dumps = glob.glob(os.path.join(self.env.pwrp_db, "*.wb_dmp"))
        for dump_file in existing_dumps:
            if not self.config.dry_run:
                os.remove(dump_file)
            self.log(f"Removed: {dump_file}")
        
        # Dump each database
        for db_name in databases:
            dump_file = os.path.join(self.env.pwrp_db, f"{db_name}.wb_dmp")
            self.log(f"-- Dumping volume {db_name} to {dump_file}")
            
            self.wb_cmd(
                f'wb dump/nofocode/out="{dump_file}"',
                volume=db_name,
            )
    
    def execute_savedirectory(self, step: UpgradeStep) -> None:
        """Save directory volume."""
        self.log("-- Saving directory volume")
        self.wb_cmd("save", volume="directory")
    
    def execute_classvolumes(self, step: UpgradeStep) -> None:
        """Create loadfiles for class volumes."""
        class_volumes = self.env.classvolumes
        
        if not class_volumes:
            self.log("No class volumes found")
            return
        
        self.log("Found classvolumes:")
        for load_file in class_volumes:
            self.log(load_file)
        self.log("")
        self.log(f"Found {len(class_volumes)} class volume(s)")
        
        ensure_directory(self.env.pwrp_inc)
        ensure_directory(self.env.pwrp_load)
        
        for load_file in class_volumes:
            volume_name = get_volume_name_from_load_file(load_file)
            if not volume_name:
                self.log(f"Warning: Could not extract volume name from {load_file}")
                continue
            
            volume_low = volume_name.lower()
            self.log(f"-- Creating structfile and loadfile for {volume_name}")
            
            # Create struct file
            self.run_command(f'co_convert -so -d {self.env.pwrp_inc} "{load_file}"')
            
            # Create proto file
            self.run_command(f'co_convert -po -d {self.env.pwrp_inc} "{load_file}"')
            
            # Create snapshot
            dbs_file = os.path.join(self.env.pwrp_load, f"{volume_low}.dbs")
            self.wb_cmd(f'create snapshot/file="{load_file}"/out="{dbs_file}"')
    
    def execute_renamedb(self, step: UpgradeStep) -> None:
        """Backup existing databases."""
        databases = self.env.databases
        
        for db_name in databases:
            db_path = os.path.join(self.env.pwrp_db, f"{db_name}.db")
            
            if os.path.exists(db_path):
                backup_path = save_file_with_backup(db_path, self.config.backup_count)
                if backup_path:
                    self.log(f"-- Saved {db_path} → {backup_path}")
    
    def execute_loaddb(self, step: UpgradeStep) -> None:
        """Load dump files into new databases."""
        dump_files = self.env.dump_files
        
        if not dump_files:
            self.log("No dump files to load")
            return
        
        for dump_file in dump_files:
            db_name = Path(dump_file).stem
            if db_name == "directory":
                continue
            
            self.log(f"-- Loading volume {db_name}")
            
            list_file = os.path.join(self.env.pwrp_db, f"{db_name}.lis")
            self.wb_cmd(f'wb load/nofocode/load="{dump_file}"/out="{list_file}"')
    
    def execute_updateclasses(self, step: UpgradeStep) -> None:
        """Update classes in all volumes."""
        databases = self.env.databases
        
        for db_name in databases:
            self.log(f"-- Updating classes in volume {db_name}")
            self.wb_cmd("update classes", volume=db_name)
    
    def execute_compile(self, step: UpgradeStep) -> None:
        """Compile all PLC programs."""
        databases = self.env.databases
        
        for db_name in databases:
            self.log(f"-- Compiling PLC programs in {db_name}")
            self.wb_cmd("compile /all", volume=db_name)
    
    def execute_createload(self, step: UpgradeStep) -> None:
        """Create loadfiles for volumes."""
        databases = self.env.databases
        
        # Remove old loadfiles
        self.log("-- Removing old loadfiles")
        old_loads = glob.glob(os.path.join(self.env.pwrp_load, "ld_vol*.dat"))
        for load_file in old_loads:
            if not self.config.dry_run:
                os.remove(load_file)
        
        for db_name in databases:
            self.log(f"-- Creating loadfile for {db_name}")
            self.wb_cmd(f"create load/volume={db_name}", volume=db_name)
    
    def execute_createboot(self, step: UpgradeStep) -> None:
        """Create bootfiles for all nodes."""
        self.log("-- Creating bootfiles for all nodes")
        self.wb_cmd("create boot/all")
    
    def execute_createpackage(self, step: UpgradeStep) -> None:
        """Create distribution packages."""
        distribute_file = os.path.join(self.env.pwrp_db, "pwrp_cnf_distribute.dat")
        
        if not os.path.exists(distribute_file):
            self.log("No distribute configuration found, skipping")
            return
        
        # Read node names from distribute config
        nodes = []
        try:
            with open(distribute_file, 'r') as f:
                for line in f:
                    if line.startswith('node '):
                        parts = line.split()
                        if len(parts) >= 2:
                            nodes.append(parts[1])
        except (IOError, OSError):
            pass
        
        if not nodes:
            self.log("No nodes found in distribute configuration")
            return
        
        self.log(f"-- Creating packages for {len(nodes)} node(s)")
        for node in nodes:
            self.log(f"-- Creating package for node {node}")
            self.wb_cmd(f"distribute /node={node} /package")
    
    # =========================================================================
    # Step Dispatcher
    # =========================================================================
    
    def execute_step(self, step: UpgradeStep) -> None:
        """Execute a single step."""
        # Map step names to executor methods
        executors = {
            'dumpdb': self.execute_dumpdb,
            'savedirectory': self.execute_savedirectory,
            'classvolumes': self.execute_classvolumes,
            'renamedb': self.execute_renamedb,
            'loaddb': self.execute_loaddb,
            'updateclasses': self.execute_updateclasses,
            'compile': self.execute_compile,
            'createload': self.execute_createload,
            'createboot': self.execute_createboot,
            'createpackage': self.execute_createpackage,
        }

        runner_executors = {
            StepRunner.PYTHON: self.execute_python_step,
            StepRunner.WB_CMD_SCRIPT: self.execute_wb_cmd_script_step,
            StepRunner.BINARY: self.execute_binary_step,
            StepRunner.SHELL: self.execute_shell_step,
        }

        if step.runner == StepRunner.BUILTIN:
            executor = executors.get(step.name)
            if not executor:
                raise UpgradeError(f"No executor found for step: {step.name}", step=step.name)
        else:
            executor = runner_executors.get(step.runner)
            if not executor:
                raise UpgradeError(
                    f"No executor available for runner {step.runner.value}: {step.name}",
                    step=step.name,
                )
        
        step.status = StepStatus.RUNNING
        start_time = time.time()
        
        try:
            executor(step)
            step.status = StepStatus.SUCCESS
        except (CommandError, UpgradeError) as e:
            step.status = StepStatus.FAILED
            step.error = str(e)
            raise
        except Exception as e:
            step.status = StepStatus.FAILED
            step.error = f"Unexpected error: {e}"
            raise UpgradeError(str(e), step=step.name)
        finally:
            step.duration = time.time() - start_time
