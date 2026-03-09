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
Runtime options for ProviewR upgrade tool.
"""

from dataclasses import dataclass, field
from typing import List, Optional


# Default version settings
DEFAULT_OLD_VERSION = "6.0"
DEFAULT_NEW_VERSION = "6.1"


@dataclass
class UpgradeConfig:
    """Runtime configuration for the upgrade process."""
    
    # Version info
    from_version: str = DEFAULT_OLD_VERSION
    to_version: str = DEFAULT_NEW_VERSION
    
    # Step control
    skip_steps: List[str] = field(default_factory=list)
    start_from: Optional[str] = None
    
    # Behavior options
    interactive: bool = True
    dry_run: bool = False
    backup_count: int = 9
    log_level: str = "INFO"
    
    # Phase control
    dump_only: bool = False  # Phase 1 only (--dump)
    reload_only: bool = False  # Refresh databases without full upgrade

    # Paths
    log_file: Optional[str] = None
