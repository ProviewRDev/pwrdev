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
            # Check if it's a ClassVolume
            try:
                with open(load_path, 'r') as f:
                    first_line = f.readline()
                    if 'ClassVolume' in first_line or 'pwr_eClass_ClassVolume' in first_line:
                        volumes.append(load_path)
            except (IOError, OSError):
                pass
        return sorted(volumes)
    
    @property
    def dump_files(self) -> List[str]:
        """Get list of .wb_dmp dump files."""
        dmp_pattern = os.path.join(self.pwrp_db, "*.wb_dmp")
        return sorted(glob.glob(dmp_pattern))


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


def get_volume_name_from_load_file(filepath: str) -> Optional[str]:
    """Extract volume name from a .wb_load file."""
    try:
        with open(filepath, 'r') as f:
            for line in f:
                # Look for ClassVolume or pwr_eClass_ClassVolume
                match = re.search(r'(?:ClassVolume|pwr_eClass_ClassVolume)\s+(\w+)', line)
                if match:
                    return match.group(1)
    except (IOError, OSError):
        pass
    return None


def ensure_directory(path: str) -> None:
    """Ensure a directory exists, creating it if necessary."""
    os.makedirs(path, exist_ok=True)


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
