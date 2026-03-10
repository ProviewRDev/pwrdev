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
Version-specific upgrade steps for V6.1 -> V7.0.

These steps run between 'updateclasses' and 'compile' during phase 2.
"""

from pwr_upgrade.steps import register_step, StepRunner, StepScope


VERSION_STEPS = [
    "convert_volume_objects",
    "convert_pn_xml",
    "remove_lucida_sans",
]


def register_steps():
    """Register all V6.1 -> V7.0 specific steps."""
    register_step(
        name="convert_volume_objects",
        description="Convert objects in volumes (V6.1->V7.0)",
        help_text="""\
Executes the V6.1->V7.0 object conversion script on each volume.
Updates object attributes and configurations that changed
between these versions.

Specific changes for V6.1->V7.0:
- PnDevice.AlarmBuffer.BufferSize set to 10
- PnDevice.StartupTime default set to 5 (if 0)
""",
        category="version-specific",
        depends_on=["updateclasses"],
        skippable=True,
        runner=StepRunner.WB_CMD_SCRIPT,
        artifact="upgrade.pwr_com",
        scope=StepScope.PER_VOLUME,
    )

    register_step(
        name="convert_pn_xml",
        description="Convert Profinet XML files (V6.1->V7.0)",
        help_text="""\
Converts Profinet runtime XML configuration files for
PnDevice objects to the V7.0 format.

Only relevant if the project uses Profinet I/O.
""",
        category="version-specific",
        depends_on=["convert_volume_objects"],
        skippable=True,
        runner=StepRunner.BINARY,
        artifact="$pwr_exe/wb_convert_pn_xml",
        scope=StepScope.PER_VOLUME,
    )

    register_step(
        name="remove_lucida_sans",
        description="Replace Lucida Sans font (V6.1->V7.0)",
        help_text="""\
Replaces the licensed Lucida Sans font with Helvetica
in all .pwg and .pwsg graphics files.

Lucida Sans is a licensed font that should not have been used.
This step replaces font code 4 (Lucida Sans) with 0 (Helvetica)
for the relevant text object types.

After this step, review your graphics to verify fonts look
acceptable. Helvetica is slightly narrower than Lucida Sans.
""",
        category="version-specific",
        depends_on=["convert_pn_xml"],
        skippable=True,
        runner=StepRunner.PYTHON,
        artifact="remove_lucida_sans.py:run",
        scope=StepScope.ONCE,
    )
