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
import re
import glob
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
)
from .steps import UpgradeStep, StepStatus
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
    
    def execute_convert_volume_objects(self, step: UpgradeStep) -> None:
        """Convert objects in volumes."""
        databases = self.env.databases
        
        upgrade_script = os.path.join(self.env.pwr_exe, "upgrade.pwr_com")
        if not os.path.exists(upgrade_script):
            self.log(f"Warning: Upgrade script not found: {upgrade_script}")
            return
        
        for db_name in databases:
            self.log(f"-- Converting volume {db_name}")
            self.wb_cmd(f'@"{upgrade_script}"', volume=db_name)
    
    def execute_convert_pn_xml(self, step: UpgradeStep) -> None:
        """Convert Profinet XML files."""
        databases = self.env.databases
        
        convert_tool = os.path.join(self.env.pwr_exe, "wb_convert_pn_xml")
        if not os.path.exists(convert_tool):
            self.log("Profinet XML conversion tool not available, skipping")
            return
        
        for db_name in databases:
            self.log(f"-- Processing volume {db_name}")
            self.run_command(f'"{convert_tool}" {db_name}', check=False)
    
    def execute_remove_lucida_sans(self, step: UpgradeStep) -> None:
        """Replace Lucida Sans font with Helvetica."""
        # Find all .pwg and .pwsg files
        graph_files = []
        for ext in ('*.pwg', '*.pwsg'):
            graph_files.extend(glob.glob(
                os.path.join(self.env.project_root, '**', ext),
                recursive=True
            ))
        
        if not graph_files:
            self.log("No graph files found")
            return
        
        # Font code pattern: specific codes followed by font type 4 (Lucida Sans)
        # Replace with font type 0 (default/Helvetica)
        pattern = re.compile(rb'^(2729|4223|3010|2245|1307) 4$', re.MULTILINE)
        
        count = 0
        for graph_file in graph_files:
            self.log(f"-- Processing {graph_file}")
            
            if self.config.dry_run:
                continue
            
            try:
                with open(graph_file, 'rb') as f:
                    content = f.read()
                
                new_content, replacements = pattern.subn(rb'\1 0', content)
                
                if replacements > 0:
                    with open(graph_file, 'wb') as f:
                        f.write(new_content)
                    count += replacements
            except (IOError, OSError) as e:
                self.log(f"Warning: Could not process {graph_file}: {e}")
        
        self.log(f"Replaced {count} font references")
    
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
            'convert_volume_objects': self.execute_convert_volume_objects,
            'convert_pn_xml': self.execute_convert_pn_xml,
            'remove_lucida_sans': self.execute_remove_lucida_sans,
            'compile': self.execute_compile,
            'createload': self.execute_createload,
            'createboot': self.execute_createboot,
            'createpackage': self.execute_createpackage,
        }
        
        executor = executors.get(step.name)
        if not executor:
            raise UpgradeError(f"No executor found for step: {step.name}", step=step.name)
        
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
