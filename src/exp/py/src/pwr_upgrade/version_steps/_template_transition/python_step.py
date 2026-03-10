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

"""Template version-local Python runner."""

import glob
import os


def run(executor, step) -> None:
    """Example Python step entry point."""
    del step

    dump_files = glob.glob(os.path.join(executor.env.pwrp_db, "*.wb_dmp"))
    if not dump_files:
        executor.log("Template python step: no dump files found")
        return

    for dump_file in dump_files:
        executor.log(f"Template python step would inspect: {dump_file}")

        if executor.config.dry_run:
            continue

        # Example pattern:
        # with open(dump_file, "r", encoding="utf-8", errors="replace") as f:
        #     content = f.read()
        # content = content.replace("OldName", "NewName")
        # with open(dump_file, "w", encoding="utf-8") as f:
        #     f.write(content)

    executor.log("Template python step completed")
