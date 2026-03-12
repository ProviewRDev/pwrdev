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

"""V6.1 -> V7.0 font conversion helper."""

import glob
import os
import re


def run(executor, step) -> None:
    """Replace Lucida Sans font code with Helvetica in graph files."""
    del step

    graph_files = []
    for ext in ("*.pwg", "*.pwsg"):
        graph_files.extend(
            glob.glob(
                os.path.join(executor.env.project_root, "**", ext),
                recursive=True,
            )
        )

    if not graph_files:
        executor.log("No graph files found")
        return

    pattern = re.compile(rb"^(2729|4223|3010|2245|1307) 4$", re.MULTILINE)

    count = 0
    for graph_file in graph_files:
        executor.log(f"-- Processing {graph_file}")

        if executor.config.dry_run:
            continue

        try:
            with open(graph_file, "rb") as f:
                content = f.read()

            new_content, replacements = pattern.subn(rb"\1 0", content)

            if replacements > 0:
                with open(graph_file, "wb") as f:
                    f.write(new_content)
                count += replacements
        except (IOError, OSError) as e:
            executor.log(f"Warning: Could not process {graph_file}: {e}")

    executor.log(f"Replaced {count} font references")
