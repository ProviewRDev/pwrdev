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
Rich-based Terminal User Interface for ProviewR upgrade tool.
"""

import os
from datetime import datetime
from typing import List, Optional

# Try to import Rich, provide fallback for basic terminal output
try:
    from rich.console import Console
    from rich.panel import Panel
    from rich.table import Table
    from rich.progress import Progress, SpinnerColumn, TextColumn, BarColumn, TaskProgressColumn
    from rich.prompt import Prompt, Confirm
    from rich.text import Text
    from rich.style import Style
    from rich.live import Live
    from rich.layout import Layout
    from rich.logging import RichHandler
    RICH_AVAILABLE = True
except ImportError:
    RICH_AVAILABLE = False

from .steps import (
    UpgradeStep, StepStatus, get_ordered_steps, get_steps_from,
    get_phase1_steps, get_phase2_steps, get_reload_only_steps,
    load_version_steps, filter_steps, reset_all_steps, select_steps,
)
from .config import UpgradeConfig
from .utils import ProjectEnvironment, get_project_environment, UpgradeError, EnvironmentError as EnvError
from .executor import StepExecutor


# Color scheme matching ProviewR style
COLORS = {
    'primary': 'blue',
    'success': 'green',
    'warning': 'yellow',
    'error': 'red',
    'info': 'cyan',
    'muted': 'dim',
}


class BasicConsole:
    """Fallback console when Rich is not available."""
    
    def print(self, *args, **kwargs):
        # Strip Rich-specific kwargs
        kwargs.pop('style', None)
        kwargs.pop('highlight', None)
        print(*args)
    
    def clear(self):
        os.system('clear' if os.name != 'nt' else 'cls')
    
    def rule(self, title: str = ""):
        width = 80
        if title:
            pad = (width - len(title) - 2) // 2
            print("-" * pad + " " + title + " " + "-" * pad)
        else:
            print("-" * width)


class UpgradeTUI:
    """Terminal User Interface for upgrade process."""
    
    def __init__(self, config: UpgradeConfig):
        self.config = config
        self.console = Console() if RICH_AVAILABLE else BasicConsole()
        self.env: Optional[ProjectEnvironment] = None
        self.executor: Optional[StepExecutor] = None
        self.log_file = None
        self.log_buffer: List[str] = []
    
    def setup_logging(self) -> None:
        """Set up logging to file and buffer."""
        if self.env and self.env.pwrp_log:
            timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
            log_path = os.path.join(self.env.pwrp_log, f"upgrade_{timestamp}.log")
            
            # Ensure log directory exists
            os.makedirs(self.env.pwrp_log, exist_ok=True)
            
            self.log_file = open(log_path, 'w')
            self.console.print(f"Logging to: {log_path}", style=COLORS['muted'])
    
    def log(self, message: str) -> None:
        """Log a message to buffer and file."""
        timestamp = datetime.now().strftime("%H:%M:%S")
        formatted = f"[{timestamp}] {message}"
        
        self.log_buffer.append(formatted)
        
        if self.log_file:
            self.log_file.write(formatted + "\n")
            self.log_file.flush()
        
        # Keep buffer size reasonable
        if len(self.log_buffer) > 1000:
            self.log_buffer = self.log_buffer[-500:]
    
    def output_callback(self, message: str) -> None:
        """Callback for executor output."""
        self.log(message)
        if RICH_AVAILABLE:
            self.console.print(message, markup=False, highlight=False)
        else:
            print(message)
    
    def show_header(self) -> None:
        """Display the header banner."""
        if self.config.reload_only:
            phase_text = "Reload-only: dump, backup, and load databases"
        elif self.config.dump_only:
            phase_text = "Phase 1: Dump databases (OLD version environment)"
        else:
            phase_text = "Phase 2: Upgrade (NEW version environment)"
        
        if RICH_AVAILABLE:
            header = Panel(
                f"[bold]ProviewR Upgrade Tool[/bold]\n"
                f"Upgrade from V{self.config.from_version} to V{self.config.to_version}\n"
                f"{phase_text}",
                style="blue",
                expand=False,
            )
            self.console.print(header)
        else:
            self.console.rule("ProviewR Upgrade Tool")
            print(f"Upgrade from V{self.config.from_version} to V{self.config.to_version}")
            print(phase_text)
            self.console.rule()
    
    def show_project_info(self) -> None:
        """Display current project information."""
        if not self.env:
            return
        
        if RICH_AVAILABLE:
            table = Table(title="Project Information", expand=False)
            table.add_column("Property", style="cyan")
            table.add_column("Value")
            
            table.add_row("Project", self.env.project_name)
            table.add_row("Root", self.env.project_root)
            table.add_row("Databases", ", ".join(self.env.databases) or "(none)")
            
            self.console.print(table)
        else:
            print(f"\nProject: {self.env.project_name}")
            print(f"Root: {self.env.project_root}")
            print(f"Databases: {', '.join(self.env.databases) or '(none)'}")
    
    def show_steps(self, steps: List[UpgradeStep]) -> None:
        """Display the list of upgrade steps."""
        if RICH_AVAILABLE:
            table = Table(title="Upgrade Steps", expand=False)
            table.add_column("#", justify="right", style="cyan", width=3)
            table.add_column("Step", style="bold")
            table.add_column("Description")
            table.add_column("Status", justify="center")
            
            for i, step in enumerate(steps, 1):
                status_style = {
                    StepStatus.PENDING: "dim",
                    StepStatus.RUNNING: "yellow",
                    StepStatus.SUCCESS: "green",
                    StepStatus.FAILED: "red",
                    StepStatus.SKIPPED: "blue",
                }.get(step.status, "")
                
                status_icon = {
                    StepStatus.PENDING: "○",
                    StepStatus.RUNNING: "◐",
                    StepStatus.SUCCESS: "●",
                    StepStatus.FAILED: "✗",
                    StepStatus.SKIPPED: "−",
                }.get(step.status, "?")
                
                table.add_row(
                    str(i),
                    step.name,
                    step.description,
                    Text(status_icon, style=status_style),
                )
            
            self.console.print(table)
        else:
            print("\nUpgrade Steps:")
            print("-" * 60)
            for i, step in enumerate(steps, 1):
                status = step.status.value
                print(f"  {i:2}. [{status:8}] {step.name}: {step.description}")
    
    def show_step_help(self, step: UpgradeStep) -> None:
        """Display detailed help for a step."""
        if RICH_AVAILABLE:
            panel = Panel(
                step.help_text or "No detailed help available.",
                title=f"Help: {step.name}",
                style="cyan",
            )
            self.console.print(panel)
        else:
            print(f"\n=== Help: {step.name} ===")
            print(step.help_text or "No detailed help available.")
            print()
    
    def prompt_step_selection(self, steps: List[UpgradeStep]) -> List[UpgradeStep]:
        """Allow user to select which steps to run."""
        prompt = (
            "\nSelect steps to run by number, range, or step name "
            "(for example: 2,4-6), or press Enter to run all:"
        )

        while True:
            if RICH_AVAILABLE:
                self.console.print(prompt)
                selection = Prompt.ask("Steps", default="")
            else:
                print(prompt)
                selection = input("Steps []: ").strip()

            if not selection:
                return steps

            try:
                selected = select_steps(
                    steps, [item.strip() for item in selection.split(",")]
                )
            except ValueError as e:
                if RICH_AVAILABLE:
                    self.console.print(f"[red]{e}[/red]")
                else:
                    print(f"Error: {e}")
                continue

            if selected:
                return selected

            if RICH_AVAILABLE:
                self.console.print("[red]No steps selected[/red]")
            else:
                print("Error: No steps selected")
    
    def prompt_start_step(self, steps: List[UpgradeStep]) -> List[UpgradeStep]:
        """Allow user to select starting step."""
        default = "1" if steps else ""

        while True:
            if RICH_AVAILABLE:
                start = Prompt.ask(
                    "Enter start step number or name",
                    default=default,
                )
            else:
                print(f"Enter start step number or name [{default}]: ", end="")
                start = input().strip() or default

            try:
                return get_steps_from(start, steps)
            except ValueError as e:
                if RICH_AVAILABLE:
                    self.console.print(f"[red]{e}[/red]")
                else:
                    print(f"Error: {e}")
    
    def prompt_continue(self, step: UpgradeStep) -> str:
        """Prompt user before executing a step. Returns 'y', 'n', or 'go'."""
        if RICH_AVAILABLE:
            self.console.print(
                Panel(
                    f"[bold]{step.description}[/bold]\n\n"
                    f"Step: {step.name}\n"
                    f"Category: {step.category}",
                    title="Next Step",
                    style="yellow",
                )
            )
            
            response = Prompt.ask(
                "Continue?",
                choices=["y", "n", "go", "help"],
                default="y",
            )
        else:
            print(f"\n{'=' * 60}")
            print(f"Next: {step.name} - {step.description}")
            print(f"{'=' * 60}")
            print("Continue? [y/n/go/help] (y): ", end="")
            response = input().strip().lower() or "y"
        
        if response == "help":
            self.show_step_help(step)
            return self.prompt_continue(step)
        
        return response
    
    def show_step_result(self, step: UpgradeStep) -> None:
        """Display the result of a step execution."""
        if step.status == StepStatus.SUCCESS:
            style = "green"
            icon = "✓"
            msg = f"Completed in {step.duration:.1f}s"
        elif step.status == StepStatus.FAILED:
            style = "red"
            icon = "✗"
            msg = step.error or "Failed"
        elif step.status == StepStatus.SKIPPED:
            style = "blue"
            icon = "−"
            msg = "Skipped"
        else:
            style = "yellow"
            icon = "?"
            msg = step.status.value
        
        if RICH_AVAILABLE:
            self.console.print(f"[{style}]{icon} {step.name}: {msg}[/{style}]")
        else:
            print(f"{icon} {step.name}: {msg}")
    
    def show_summary(self, steps: List[UpgradeStep]) -> None:
        """Display summary of upgrade process."""
        success = sum(1 for s in steps if s.status == StepStatus.SUCCESS)
        failed = sum(1 for s in steps if s.status == StepStatus.FAILED)
        skipped = sum(1 for s in steps if s.status == StepStatus.SKIPPED)
        total_time = sum(s.duration for s in steps)
        
        if RICH_AVAILABLE:
            table = Table(title="Upgrade Summary", expand=False)
            table.add_column("Metric", style="cyan")
            table.add_column("Value", justify="right")
            
            table.add_row("Successful", f"[green]{success}[/green]")
            table.add_row("Failed", f"[red]{failed}[/red]")
            table.add_row("Skipped", f"[blue]{skipped}[/blue]")
            table.add_row("Total Time", f"{total_time:.1f}s")
            
            self.console.print(table)
            
            if failed == 0:
                if self.config.dump_only:
                    done_message = "[bold green]Phase 1 completed.[/bold green]"
                elif self.config.reload_only:
                    done_message = "[bold green]Database reload completed.[/bold green]"
                else:
                    done_message = "[bold green]The upgrade procedure is now complete.[/bold green]"
                self.console.print(
                    Panel(
                        done_message,
                        style="green",
                    )
                )
            else:
                self.console.print(
                    Panel(
                        f"[bold red]Upgrade failed with {failed} error(s).[/bold red]\n"
                        "Review the log for details.",
                        style="red",
                    )
                )
        else:
            print("\n" + "=" * 40)
            print("UPGRADE SUMMARY")
            print("=" * 40)
            print(f"  Successful: {success}")
            print(f"  Failed:     {failed}")
            print(f"  Skipped:    {skipped}")
            print(f"  Total time: {total_time:.1f}s")
            print()
            
            if failed == 0:
                if self.config.dump_only:
                    print("Phase 1 completed.")
                elif self.config.reload_only:
                    print("Database reload completed.")
                else:
                    print("The upgrade procedure is now complete.")
            else:
                print(f"Upgrade failed with {failed} error(s).")
    
    def show_phase1_done(self) -> None:
        """Show instructions after phase 1 completes."""
        msg = (
            "Phase 1 complete. Dump files created.\n\n"
            "Next steps:\n"
            "  1. Open the administrator:  pwra\n"
            f"  2. Change the project version to V{self.config.to_version}\n"
            "  3. Save and close the administrator\n"
            f"  4. Switch to the new environment:  sdf {self.env.project_name}\n"
            "  5. Run phase 2:  pwr_upgrade.sh"
        )
        if RICH_AVAILABLE:
            self.console.print(Panel(msg, title="What to do next", style="cyan"))
        else:
            print("\n" + "=" * 60)
            print(msg)
            print("=" * 60)
    
    def run(self) -> int:
        """Main entry point for the TUI."""
        try:
            # Initialize environment
            self.env = get_project_environment()
            self.executor = StepExecutor(
                env=self.env,
                config=self.config,
                output_callback=self.output_callback,
            )
            self.setup_logging()
            
            # Load version-specific steps
            version_step_names = load_version_steps(
                self.config.from_version, self.config.to_version
            )
            
            # Show header and project info
            self.show_header()
            self.show_project_info()
            
            if version_step_names:
                vs_list = ", ".join(version_step_names)
                if RICH_AVAILABLE:
                    self.console.print(
                        f"Version-specific steps loaded: [cyan]{vs_list}[/cyan]"
                    )
                else:
                    print(f"Version-specific steps loaded: {vs_list}")
            
            # Get steps to execute based on phase
            reset_all_steps()
            
            if self.config.reload_only:
                steps = get_reload_only_steps()
            elif self.config.dump_only:
                steps = get_phase1_steps()
            else:
                steps = get_phase2_steps()
            
            # Apply config filters
            if self.config.skip_steps:
                try:
                    steps = filter_steps(steps, self.config.skip_steps)
                except ValueError as e:
                    raise UpgradeError(str(e))
            
            if self.config.start_from:
                try:
                    steps = get_steps_from(self.config.start_from, steps)
                except ValueError as e:
                    raise UpgradeError(str(e))
            
            # Show steps
            self.show_steps(steps)
            
            # Interactive step selection
            if self.config.interactive and not self.config.start_from and len(steps) > 1:
                steps = self.prompt_step_selection(steps)
            
            # Execute steps
            go_mode = not self.config.interactive
            
            for step in steps:
                if not go_mode and self.config.interactive:
                    response = self.prompt_continue(step)
                    if response == 'n':
                        break
                    elif response == 'go':
                        go_mode = True
                
                self.log(f"Starting step: {step.name}")
                
                try:
                    self.executor.execute_step(step)
                    self.show_step_result(step)
                except UpgradeError as e:
                    self.show_step_result(step)
                    if RICH_AVAILABLE:
                        self.console.print(f"[red]Error: {e.message}[/red]")
                    else:
                        print(f"Error: {e.message}")
                    
                    if self.config.interactive:
                        if RICH_AVAILABLE:
                            cont = Confirm.ask("Continue with next step?", default=False)
                        else:
                            print("Continue with next step? [y/N]: ", end="")
                            cont = input().strip().lower() == 'y'
                        
                        if not cont:
                            break
                    else:
                        break
            
            # Show summary
            self.show_summary(steps)
            
            # After phase 1, show next-steps instructions
            failed = sum(1 for s in steps if s.status == StepStatus.FAILED)
            if self.config.dump_only and failed == 0:
                self.show_phase1_done()
                return 0
            
            return 1 if failed > 0 else 0
            
        except EnvError as e:
            if RICH_AVAILABLE:
                self.console.print(f"[red]Environment Error: {e.message}[/red]")
            else:
                print(f"Environment Error: {e.message}")
            return 1
        
        except KeyboardInterrupt:
            if RICH_AVAILABLE:
                self.console.print("\n[yellow]Upgrade cancelled by user.[/yellow]")
            else:
                print("\nUpgrade cancelled by user.")
            return 130
        
        finally:
            if self.log_file:
                self.log_file.close()


def run_upgrade(config: UpgradeConfig) -> int:
    """Run the upgrade process with given configuration."""
    tui = UpgradeTUI(config)
    return tui.run()


def show_help() -> None:
    """Display help information."""
    console = Console() if RICH_AVAILABLE else BasicConsole()
    
    steps = get_ordered_steps()
    
    if RICH_AVAILABLE:
        console.print(Panel(
            "[bold]ProviewR Upgrade Tool[/bold]\n\n"
            "Usage: pwr_upgrade [OPTIONS]\n\n"
            "A unified tool for upgrading ProviewR projects between versions.",
            style="blue",
        ))
        
        console.print("\n[bold]Options:[/bold]")
        console.print("  --help, -h          Show this help message")
        console.print("  --version           Show version information")
        console.print("  --dump              Phase 1: dump databases (run in OLD version env)")
        console.print("                      Without --dump, runs Phase 2 (NEW version env)")
        console.print("  --reload-only       Run dumpdb, renamedb, and loaddb only")
        console.print("  --dry-run           Preview actions without executing")
        console.print("  --all               Run all steps without prompting")
        console.print("  --from STEP         Start from a step name or step number")
        console.print("  --skip STEPS        Skip step names/numbers (comma-separated, ranges allowed)")
        console.print("  --list              List all available steps")
        
        console.print("\n[bold]Workflow:[/bold]")
        console.print("  1. sdf <project>                  Switch to project (old version)")
        console.print("  2. pwr_upgrade.sh --dump           Dump databases")
        console.print("  3. pwra                            Change project version in admin")
        console.print("  4. sdf <project>                   Reload environment (new version)")
        console.print("  5. pwr_upgrade.sh                  Run upgrade")
        console.print("  Dev refresh: pwr_upgrade.sh --reload-only")
        
        console.print("\n[bold]Available Steps:[/bold]")
        
        table = Table(show_header=True, expand=False)
        table.add_column("Step", style="cyan")
        table.add_column("Description")
        table.add_column("Category", style="dim")
        table.add_column("Phase", style="dim")
        
        for step in steps:
            phase = "1 (dump)" if step.category == "phase1" else "2 (upgrade)"
            table.add_row(step.name, step.description, step.category, phase)
        
        console.print(table)
    else:
        console.rule("ProviewR Upgrade Tool")
        print("\nUsage: pwr_upgrade [OPTIONS]")
        print("\nA unified tool for upgrading ProviewR projects between versions.")
        print("\nOptions:")
        print("  --help, -h          Show this help message")
        print("  --version           Show version information")
        print("  --dump              Phase 1: dump databases (run in OLD version env)")
        print("                      Without --dump, runs Phase 2 (NEW version env)")
        print("  --reload-only       Run dumpdb, renamedb, and loaddb only")
        print("  --dry-run           Preview actions without executing")
        print("  --all               Run all steps without prompting")
        print("  --from STEP         Start from a step name or step number")
        print("  --skip STEPS        Skip step names/numbers (comma-separated, ranges allowed)")
        print("  --list              List all available steps")
        print("\nWorkflow:")
        print("  1. sdf <project>                  Switch to project (old version)")
        print("  2. pwr_upgrade.sh --dump           Dump databases")
        print("  3. pwra                            Change project version in admin")
        print("  4. sdf <project>                   Reload environment (new version)")
        print("  5. pwr_upgrade.sh                  Run upgrade")
        print("  Dev refresh: pwr_upgrade.sh --reload-only")
        print("\nAvailable Steps:")
        for step in steps:
            phase = "1" if step.category == "phase1" else "2"
            print(f"  {step.name:25} {step.description} [phase {phase}]")


def list_steps() -> None:
    """List all available steps."""
    steps = get_ordered_steps()
    
    if RICH_AVAILABLE:
        console = Console()
        table = Table(title="Upgrade Steps", expand=False)
        table.add_column("#", justify="right", width=3)
        table.add_column("Step", style="cyan")
        table.add_column("Description")
        table.add_column("Category", style="dim")
        table.add_column("Phase", style="dim")
        table.add_column("Skippable", justify="center")
        
        for i, step in enumerate(steps, 1):
            phase = "1 (dump)" if step.category == "phase1" else "2 (upgrade)"
            table.add_row(
                str(i),
                step.name,
                step.description,
                step.category,
                phase,
                "Yes" if step.skippable else "No",
            )
        
        console.print(table)
    else:
        print("Upgrade Steps:")
        print("-" * 70)
        for i, step in enumerate(steps, 1):
            phase = "1" if step.category == "phase1" else "2"
            skip = "Y" if step.skippable else "N"
            print(f"{i:2}. {step.name:25} {step.description} [phase {phase}] [{step.category}] Skip:{skip}")
