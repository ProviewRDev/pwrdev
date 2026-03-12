#!/bin/bash
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

# pwr_upgrade - ProviewR Unified Upgrade Tool
#
# A Python TUI application for upgrading ProviewR projects between versions.
# This wrapper script invokes the Python module with the correct environment.
#
# Usage:
#   pwr_upgrade [OPTIONS]
#
# Options:
#   --help, -h          Show help message
#   --version           Show version information
#   --dry-run           Preview actions without executing
#   --all               Run all steps without prompting
#   --from STEP         Start from a specific step
#   --skip STEPS        Skip skippable steps (comma-separated)
#   --reload-only       Run dumpdb, renamedb and loaddb only
#   --list              List all available steps
#
# Default behavior is interactive: select steps, inspect help, and
# confirm, skip, or stop at each step.
#
# This tool replaces and unifies reload.sh and upgrade.sh functionality.

# Ensure PYTHONPATH includes pwr_exe for the pwr_upgrade module
if [ -n "$pwr_exe" ]; then
    export PYTHONPATH="$pwr_exe:$PYTHONPATH"
fi

# Run the Python module
exec python3 -m pwr_upgrade "$@"
