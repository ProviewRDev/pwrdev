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
Upgrade step definitions and registry.

Steps are divided into:
  - Core steps: needed for every upgrade (defined here)
  - Version-specific steps: only for certain version transitions
    (defined in version_steps/<from>_to_<to>.py)

The upgrade runs in two phases:
  Phase 1 (--dump): dumpdb — runs in OLD version environment
  Phase 2 (default): everything else — runs in NEW version environment
"""

from dataclasses import dataclass, field
from enum import Enum
from typing import List, Optional, Dict
import importlib


class StepStatus(Enum):
    """Status of an upgrade step."""
    PENDING = "pending"
    RUNNING = "running"
    SUCCESS = "success"
    FAILED = "failed"
    SKIPPED = "skipped"


@dataclass
class UpgradeStep:
    """Definition of a single upgrade step."""
    
    # Unique identifier (used in config and CLI)
    name: str
    
    # Human-readable description
    description: str
    
    # Detailed help text shown in TUI
    help_text: str = ""
    
    # Category for grouping in TUI
    category: str = "upgrade"
    
    # Steps that must complete before this one
    depends_on: List[str] = field(default_factory=list)
    
    # Can this step be skipped?
    skippable: bool = True
    
    # Current status
    status: StepStatus = StepStatus.PENDING
    
    # Error message if failed
    error: Optional[str] = None
    
    # Output/log from execution
    output: str = ""
    
    # Execution time in seconds
    duration: float = 0.0


# Registry of all upgrade steps
UPGRADE_STEPS: Dict[str, UpgradeStep] = {}


def register_step(
    name: str,
    description: str,
    help_text: str = "",
    category: str = "upgrade",
    depends_on: List[str] = None,
    skippable: bool = True,
) -> UpgradeStep:
    """Register an upgrade step."""
    step = UpgradeStep(
        name=name,
        description=description,
        help_text=help_text,
        category=category,
        depends_on=depends_on or [],
        skippable=skippable,
    )
    UPGRADE_STEPS[name] = step
    return step


def get_step(name: str) -> Optional[UpgradeStep]:
    """Get a step by name."""
    return UPGRADE_STEPS.get(name)


def get_all_steps() -> List[UpgradeStep]:
    """Get all steps in execution order."""
    return list(UPGRADE_STEPS.values())


def get_steps_by_category(category: str) -> List[UpgradeStep]:
    """Get all steps in a category."""
    return [s for s in UPGRADE_STEPS.values() if s.category == category]


def reset_all_steps() -> None:
    """Reset all steps to pending status."""
    for step in UPGRADE_STEPS.values():
        step.status = StepStatus.PENDING
        step.error = None
        step.output = ""
        step.duration = 0.0


# =============================================================================
# Core Step Definitions (always present in every upgrade)
# =============================================================================

# --- Phase 1: Pre-upgrade (runs in OLD version environment) ---

register_step(
    name="dumpdb",
    description="Dump all databases to text files",
    help_text="""\
Dumps each database volume to a .wb_dmp text file.
This creates a portable text representation that can be loaded
into a new version of the database.

Files created: $pwrp_db/<volume>.wb_dmp

Command: wb_cmd -v <volume> wb dump/nofocode/out="<file>"

NOTE: This step must run in the OLD version environment,
before you change the project version and do 'sdf'.
""",
    category="phase1",
    depends_on=[],
    skippable=False,
)

# --- Phase 2: Upgrade (runs in NEW version environment) ---

register_step(
    name="savedirectory",
    description="Save directory volume",
    help_text="""\
Saves the directory volume which contains the project structure
and volume definitions.

Command: wb_cmd -q -v directory save
""",
    category="core",
    depends_on=[],
    skippable=False,
)

register_step(
    name="classvolumes",
    description="Create loadfiles for class volumes",
    help_text="""\
Creates struct files and loadfiles (.dbs) for all class volumes
defined in .wb_load files.

For each ClassVolume:
1. co_convert -so -d $pwrp_inc <file>  (struct file)
2. co_convert -po -d $pwrp_inc <file>  (proto file)
3. wb_cmd create snapshot/file="<file>"/out="$pwrp_load/<vol>.dbs"
""",
    category="core",
    depends_on=["savedirectory"],
    skippable=True,
)

register_step(
    name="renamedb",
    description="Backup existing databases",
    help_text="""\
Renames existing .db files to .db.1 (with rotation up to .9).
This preserves the old database as a backup before loading
new data.

Files backed up: $pwrp_db/<volume>.db -> $pwrp_db/<volume>.db.1
""",
    category="core",
    depends_on=["classvolumes"],
    skippable=False,
)

register_step(
    name="loaddb",
    description="Load dump files into new databases",
    help_text="""\
Creates new databases and loads the .wb_dmp text files,
creating fresh databases in the new version format.

Command: wb_cmd wb load/nofocode/load="<dumpfile>"/out="<listfile>"
""",
    category="core",
    depends_on=["renamedb"],
    skippable=False,
)

register_step(
    name="updateclasses",
    description="Update classes in all volumes",
    help_text="""\
Updates class definitions in all volumes to match the new
ProviewR version's class library.

Command: wb_cmd -q -v <volume> update classes
""",
    category="core",
    depends_on=["loaddb"],
    skippable=True,
)

# Placeholder: version-specific steps are inserted here
# (between updateclasses and compile)

register_step(
    name="compile",
    description="Compile all PLC programs",
    help_text="""\
Compiles all PLC programs in all volumes.

Command: wb_cmd -q -v <volume> compile /all
""",
    category="build",
    depends_on=["updateclasses"],
    skippable=False,
)

register_step(
    name="createload",
    description="Create loadfiles for volumes",
    help_text="""\
Creates binary loadfiles (.dat) for all root volumes.
These are used by the runtime system.

Command: wb_cmd -q -v <volume> create load/volume=<volume>
""",
    category="build",
    depends_on=["compile"],
    skippable=False,
)

register_step(
    name="createboot",
    description="Create bootfiles for all nodes",
    help_text="""\
Creates bootfiles for all nodes defined in the project.
This also links the PLC programs for each node.

Command: wb_cmd -q create boot/all

Note: Before this step, you should compile any modules
included by ra_plc_user.
""",
    category="build",
    depends_on=["createload"],
    skippable=False,
)

register_step(
    name="createpackage",
    description="Create distribution packages",
    help_text="""\
Creates distribution packages for all nodes defined
in the distribute configuration.

Command: wb_cmd -q distribute /node=<node> /package
""",
    category="build",
    depends_on=["createboot"],
    skippable=True,
)


# =============================================================================
# Step ordering
# =============================================================================

# Phase 1 steps (run in old version environment with --dump)
PHASE1_STEPS = [
    "dumpdb",
]

# Database refresh flow used for development reloads.
RELOAD_ONLY_STEPS = [
    "dumpdb",
    "renamedb",
    "loaddb",
]

# Core phase 2 steps (always present)
CORE_PHASE2_STEPS = [
    "savedirectory",
    "classvolumes",
    "renamedb",
    "loaddb",
    "updateclasses",
    # --- version-specific steps inserted here ---
    "compile",
    "createload",
    "createboot",
    "createpackage",
]

# Version-specific steps are inserted between updateclasses and compile
VERSION_STEP_INSERT_AFTER = "updateclasses"

# Combined order (built dynamically by load_version_steps)
STEP_ORDER: List[str] = PHASE1_STEPS + CORE_PHASE2_STEPS


def load_version_steps(from_version: str, to_version: str) -> List[str]:
    """
    Load version-specific steps for a particular upgrade path.
    
    Looks for a module in version_steps/ named like v60_to_v61.py
    The module must define:
      - register_steps(): function that calls register_step() for each step
      - VERSION_STEPS: list of step names in execution order
    
    Returns the names of the version-specific steps that were loaded.
    """
    global STEP_ORDER
    
    # Normalize version strings: "6.0" -> "v60", "6.1" -> "v61"
    from_tag = "v" + from_version.replace(".", "")
    to_tag = "v" + to_version.replace(".", "")
    module_name = f".version_steps.{from_tag}_to_{to_tag}"
    
    version_step_names = []
    
    try:
        mod = importlib.import_module(module_name, package="pwr_upgrade")
        
        # Let the module register its steps
        if hasattr(mod, 'register_steps'):
            mod.register_steps()
        
        # Get the ordered list of version-specific step names
        if hasattr(mod, 'VERSION_STEPS'):
            version_step_names = list(mod.VERSION_STEPS)
    except ImportError:
        # No version-specific module — that's fine
        pass
    
    # Rebuild STEP_ORDER with version steps inserted
    phase2 = list(CORE_PHASE2_STEPS)
    if version_step_names:
        idx = phase2.index(VERSION_STEP_INSERT_AFTER) + 1
        for i, name in enumerate(version_step_names):
            phase2.insert(idx + i, name)
    
    STEP_ORDER = PHASE1_STEPS + phase2
    return version_step_names


def get_ordered_steps() -> List[UpgradeStep]:
    """Get all steps in canonical execution order."""
    return [UPGRADE_STEPS[name] for name in STEP_ORDER if name in UPGRADE_STEPS]


def get_phase1_steps() -> List[UpgradeStep]:
    """Get phase 1 (pre-upgrade / dump) steps only."""
    return [UPGRADE_STEPS[name] for name in PHASE1_STEPS if name in UPGRADE_STEPS]


def get_phase2_steps() -> List[UpgradeStep]:
    """Get phase 2 (upgrade proper) steps only."""
    phase2_names = [n for n in STEP_ORDER if n not in PHASE1_STEPS]
    return [UPGRADE_STEPS[name] for name in phase2_names if name in UPGRADE_STEPS]


def get_reload_only_steps() -> List[UpgradeStep]:
    """Get the minimal database refresh flow."""
    return [UPGRADE_STEPS[name] for name in RELOAD_ONLY_STEPS if name in UPGRADE_STEPS]


def resolve_step_reference(reference: str, steps: List[UpgradeStep]) -> str:
    """Resolve a step name or 1-based index against the displayed step list."""
    token = str(reference).strip()
    if not token:
        raise ValueError("step reference is empty")

    if token.isdigit():
        index = int(token)
        if 1 <= index <= len(steps):
            return steps[index - 1].name
        raise ValueError(f"step number out of range: {token}")

    for step in steps:
        if step.name == token:
            return step.name

    raise ValueError(f"unknown step reference: {token}")


def expand_step_references(references: List[str], steps: List[UpgradeStep]) -> List[str]:
    """Expand step references into concrete step names."""
    resolved: List[str] = []
    seen = set()

    for reference in references:
        token = str(reference).strip()
        if not token:
            continue

        if token.count("-") == 1:
            start_token, end_token = [part.strip() for part in token.split("-", 1)]
            if start_token.isdigit() and end_token.isdigit():
                start_index = int(start_token)
                end_index = int(end_token)
                if start_index < 1 or end_index < start_index or end_index > len(steps):
                    raise ValueError(f"invalid step range: {token}")
                for index in range(start_index, end_index + 1):
                    name = steps[index - 1].name
                    if name not in seen:
                        resolved.append(name)
                        seen.add(name)
                continue

        name = resolve_step_reference(token, steps)
        if name not in seen:
            resolved.append(name)
            seen.add(name)

    return resolved


def get_steps_from(start_step: str, steps: Optional[List[UpgradeStep]] = None) -> List[UpgradeStep]:
    """Get steps starting from a specific step within the provided step list."""
    ordered_steps = list(steps or get_ordered_steps())
    start_name = resolve_step_reference(start_step, ordered_steps)

    try:
        start_idx = next(i for i, step in enumerate(ordered_steps) if step.name == start_name)
    except StopIteration as exc:
        raise ValueError(f"unknown step reference: {start_step}") from exc

    return ordered_steps[start_idx:]


def select_steps(steps: List[UpgradeStep], references: List[str]) -> List[UpgradeStep]:
    """Select a subset of steps, preserving the order specified by the references."""
    step_names = expand_step_references(references, steps)
    selected = []
    by_name = {step.name: step for step in steps}
    for name in step_names:
        step = by_name.get(name)
        if step:
            selected.append(step)
    return selected


def filter_steps(steps: List[UpgradeStep], skip_refs: List[str]) -> List[UpgradeStep]:
    """Filter out skipped steps using step names, numbers, or numeric ranges."""
    skip_names = set(expand_step_references(skip_refs, steps))
    return [step for step in steps if step.name not in skip_names]
