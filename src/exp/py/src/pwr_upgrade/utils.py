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
Utility functions for ProviewR environment and path handling.
"""

import os
import glob
import re
from pathlib import Path
from dataclasses import dataclass
from typing import Optional, List, Dict


class UpgradeError(Exception):
    """Base exception for upgrade errors."""
    def __init__(self, message: str, step: str = None):
        self.message = message
        self.step = step
        super().__init__(message)


class EnvironmentError(UpgradeError):
    """Raised when required environment is not set up."""
    pass


class CommandError(UpgradeError):
    """Raised when a command execution fails."""
    def __init__(self, message: str, command: str = None, returncode: int = None, step: str = None):
        self.command = command
        self.returncode = returncode
        super().__init__(message, step)


@dataclass
class ProjectEnvironment:
    """ProviewR project environment information."""
    project_name: str
    project_root: str
    pwrp_db: str
    pwrp_load: str
    pwrp_inc: str
    pwrp_tmp: str
    pwrp_log: str
    pwrp_cnf: str
    pwrp_pop: str
    pwr_exe: str
    pwr_load: str
    
    @property
    def databases(self) -> List[str]:
        """Get list of database names (excluding rt_eventlog and pwrp__ prefixed)."""
        db_pattern = os.path.join(self.pwrp_db, "*.db")
        dbs = []
        for db_path in glob.glob(db_pattern):
            db_name = Path(db_path).stem
            if db_name != "rt_eventlog" and not db_name.startswith("pwrp__"):
                dbs.append(db_name)
        return sorted(dbs)
    
    @property
    def classvolumes(self) -> List[str]:
        """Get list of class volume .wb_load files."""
        load_pattern = os.path.join(self.pwrp_db, "*.wb_load")
        volumes = []
        for load_path in glob.glob(load_pattern):
            if Path(load_path).name == "directory.wb_load":
                continue
            volume_info = get_volume_info_from_load_file(load_path)
            if volume_info and "ClassVolume" in volume_info["class"]:
                volumes.append(load_path)
        return sorted(volumes)
    
    @property
    def dump_files(self) -> List[str]:
        """Get list of .wb_dmp dump files."""
        dmp_pattern = os.path.join(self.pwrp_db, "*.wb_dmp")
        return sorted(glob.glob(dmp_pattern))


@dataclass(frozen=True)
class ProjectLock:
    """Information about a detected ProviewR project lock file."""
    resource: str
    target_path: str
    lock_path: str
    owner: Optional[str]


def translate_filename(name: str) -> str:
    """
    Translate ProviewR environment variables in filename.
    e.g., $pwrp_db/file.txt -> /actual/path/db/file.txt
    """
    if not name or name[0] != '$':
        return name
    
    idx = name.find('/')
    if idx == -1:
        # Just an env var, no path
        env_name = name[1:]
        return os.environ.get(env_name, name)
    
    env_name = name[1:idx]
    env_value = os.environ.get(env_name)
    if env_value is None:
        return name
    return env_value + name[idx:]


def get_env_or_raise(var_name: str) -> str:
    """Get environment variable or raise EnvironmentError."""
    value = os.environ.get(var_name)
    if not value:
        raise EnvironmentError(
            f"Environment variable ${var_name} is not set. "
            "Make sure you have run 'sdf <project>' to set up the project environment."
        )
    return value


def get_project_environment() -> ProjectEnvironment:
    """
    Get the current ProviewR project environment.
    Raises EnvironmentError if required variables are not set.
    """
    pwrp_root = get_env_or_raise('pwrp_root')
    project_name = os.path.basename(pwrp_root)
    
    # Try to get from env, fall back to standard paths
    def get_or_default(var: str, default: str) -> str:
        return os.environ.get(var, default)
    
    return ProjectEnvironment(
        project_name=project_name,
        project_root=pwrp_root,
        pwrp_db=get_or_default('pwrp_db', os.path.join(pwrp_root, 'src', 'db')),
        pwrp_load=get_or_default('pwrp_load', os.path.join(pwrp_root, 'bld', 'common', 'load')),
        pwrp_inc=get_or_default('pwrp_inc', os.path.join(pwrp_root, 'bld', 'common', 'inc')),
        pwrp_tmp=get_or_default('pwrp_tmp', os.path.join(pwrp_root, 'bld', 'common', 'tmp')),
        pwrp_log=get_or_default('pwrp_log', os.path.join(pwrp_root, 'bld', 'common', 'log')),
        pwrp_cnf=get_or_default('pwrp_cnf', os.path.join(pwrp_root, 'src', 'cnf')),
        pwrp_pop=get_or_default('pwrp_pop', os.path.join(pwrp_root, 'src', 'pop')),
        pwr_exe=get_env_or_raise('pwr_exe'),
        pwr_load=get_env_or_raise('pwr_load'),
    )


def save_file_with_backup(filepath: str, max_versions: int = 9) -> Optional[str]:
    """
    Save a file by renaming it with version suffix (.1, .2, ... .9).
    Rotates existing backups. Returns the backup path or None if file didn't exist.
    """
    if not os.path.exists(filepath):
        return None
    
    # Rotate existing backups
    for version in range(max_versions, 0, -1):
        old_backup = f"{filepath}.{version}"
        new_backup = f"{filepath}.{version + 1}"
        if os.path.exists(old_backup):
            if version == max_versions:
                # Remove oldest
                if os.path.isdir(old_backup):
                    import shutil
                    shutil.rmtree(old_backup)
                else:
                    os.remove(old_backup)
            else:
                os.rename(old_backup, new_backup)
    
    # Move current to .1
    backup_path = f"{filepath}.1"
    os.rename(filepath, backup_path)
    return backup_path


def get_volume_info_from_load_file(filepath: str) -> Optional[Dict[str, str]]:
    """
    Extract volume declaration information from a .wb_load file.

    Returns a dict with keys ``name`` and ``class`` for the first matching
    ``Volume <name> <class> ...`` declaration, or ``None`` if no volume line
    can be found.
    """
    try:
        with open(filepath, 'r') as f:
            for line in f:
                stripped = line.strip()
                if not stripped or stripped.startswith('!'):
                    continue

                match = re.match(r'^Volume\s+(\S+)\s+(\S+)', stripped)
                if match:
                    return {"name": match.group(1), "class": match.group(2)}
    except (IOError, OSError):
        pass
    return None


def get_volume_name_from_load_file(filepath: str) -> Optional[str]:
    """Extract volume name from a .wb_load file."""
    volume_info = get_volume_info_from_load_file(filepath)
    if volume_info:
        return volume_info["name"]
    return None


def ensure_directory(path: str) -> None:
    """Ensure a directory exists, creating it if necessary."""
    os.makedirs(path, exist_ok=True)


def get_lock_path(target_path: str) -> str:
    """Return the ProviewR lock-file path for a database or load file."""
    return f"{target_path}.lock"


def read_lock_owner(lock_path: str) -> Optional[str]:
    """Read the first line of a ProviewR lock file."""
    try:
        with open(lock_path, 'r') as f:
            owner = f.readline().strip()
            return owner or None
    except (IOError, OSError):
        return None


def detect_project_locks(env: ProjectEnvironment) -> List[ProjectLock]:
    """Detect project database and classvolume lock files."""
    candidates = []
    seen_targets = set()

    def add_candidate(resource: str, target_path: str) -> None:
        if not os.path.exists(target_path):
            return
        if target_path in seen_targets:
            return
        seen_targets.add(target_path)
        candidates.append((resource, target_path))

    add_candidate("Directory volume", os.path.join(env.pwrp_db, "directory.wb_load"))

    for db_name in env.databases:
        add_candidate(f"Database {db_name}", os.path.join(env.pwrp_db, f"{db_name}.db"))

    for load_path in env.classvolumes:
        volume_name = get_volume_name_from_load_file(load_path) or Path(load_path).stem
        add_candidate(f"Class volume {volume_name}", load_path)

    locks = []
    for resource, target_path in candidates:
        lock_path = get_lock_path(target_path)
        if os.path.exists(lock_path):
            locks.append(
                ProjectLock(
                    resource=resource,
                    target_path=target_path,
                    lock_path=lock_path,
                    owner=read_lock_owner(lock_path),
                )
            )

    return locks


def remove_project_lock(lock: ProjectLock) -> None:
    """Remove a ProviewR project lock file."""
    os.remove(lock.lock_path)


def detect_platform() -> Dict[str, str]:
    """Detect current platform (OS and hardware)."""
    import platform
    
    machine = platform.machine()
    system = platform.system().lower()
    
    # Map to ProviewR conventions
    if machine == 'x86_64':
        hw = 'x86_64'
    elif machine.startswith('arm'):
        hw = 'arm'
    elif machine == 'aarch64':
        hw = 'arm64'
    else:
        hw = 'x86'
    
    if system == 'linux':
        os_name = 'linux'
    elif system == 'darwin':
        os_name = 'macos'
    elif system == 'freebsd':
        os_name = 'freebsd'
    else:
        os_name = system
    
    return {
        'hardware': hw,
        'os': os_name,
        'platform': f"{hw}_{os_name}",
    }
