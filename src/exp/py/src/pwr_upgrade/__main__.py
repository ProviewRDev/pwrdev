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
ProviewR Upgrade Tool - Entry Point

Usage:
    pwr_upgrade [OPTIONS]
    python -m pwr_upgrade [OPTIONS]

A unified tool for upgrading ProviewR projects between versions.
Replaces and combines functionality from reload.sh and upgrade.sh.
"""

import sys
import getopt

from . import __version__
from .config import UpgradeConfig
from .cli import run_upgrade, show_help, list_steps


def print_version():
    """Print version information."""
    print(f"pwr_upgrade version {__version__}")
    print("ProviewR Unified Upgrade Tool")
    print("Copyright (C) 2005-2026 SSAB EMEA AB")


def build_config(argv):
    """Parse command line arguments into an UpgradeConfig."""
    # Parse command line arguments
    try:
        opts, _ = getopt.getopt(
            argv,
            "hva",
            [
                "help",
                "version",
                "dump",
                "reload-only",
                "dry-run",
                "all",
                "from=",
                "skip=",
                "list",
            ]
        )
    except getopt.GetoptError as e:
        raise ValueError(str(e)) from e

    config = UpgradeConfig()
    
    for opt, arg in opts:
        if opt in ("-h", "--help"):
            return config, "help"

        if opt in ("-v", "--version"):
            return config, "version"

        if opt == "--list":
            return config, "list"

        elif opt == "--dump":
            config.dump_only = True

        elif opt == "--reload-only":
            config.reload_only = True

        elif opt == "--dry-run":
            config.dry_run = True

        elif opt in ("-a", "--all"):
            config.interactive = False

        elif opt == "--from":
            config.start_from = arg

        elif opt == "--skip":
            config.skip_steps = [s.strip() for s in arg.split(',')]

    if config.dump_only and config.reload_only:
        raise ValueError("--dump and --reload-only cannot be used together")

    return config, "run"


def main(argv=None):
    """Main entry point."""
    if argv is None:
        argv = sys.argv[1:]

    try:
        config, command = build_config(argv)
    except ValueError as e:
        print(f"Error: {e}")
        print("Use --help for usage information.")
        return 1

    if command == "help":
        show_help()
        return 0

    if command == "version":
        print_version()
        return 0

    if command == "list":
        list_steps()
        return 0

    return run_upgrade(config)


if __name__ == "__main__":
    sys.exit(main())
