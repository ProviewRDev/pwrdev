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
Template version-step package.

This package is intentionally named `_template_transition` so it will never
be discovered by `load_version_steps()`, which only imports exact names like
`v61_to_v70`.

Copy this directory and rename it to `v<from>_to_<to>` when creating a real
transition package.
"""

from pwr_upgrade.steps import register_step, StepRunner, StepScope


VERSION_STEPS = [
    "template_python_step",
    "template_wb_cmd_step",
    "template_shell_step",
    "template_binary_step",
]


def register_steps():
    """Register example steps for all supported runner types."""
    register_step(
        name="template_python_step",
        description="Template: version-local Python helper",
        help_text="""\
Example of a Python runner that keeps version-specific file logic beside
the transition package.

Artifact points to a Python file in this package and the callable to invoke.
""",
        category="version-specific",
        depends_on=["updateclasses"],
        skippable=True,
        runner=StepRunner.PYTHON,
        artifact="python_step.py:run",
        scope=StepScope.ONCE,
    )

    register_step(
        name="template_wb_cmd_step",
        description="Template: wb_cmd .pwr_com script",
        help_text="""\
Example of a wb_cmd runner for object and attribute fixups inside each volume.

Artifact points to a package-local .pwr_com file.
""",
        category="version-specific",
        depends_on=["template_python_step"],
        skippable=True,
        runner=StepRunner.WB_CMD_SCRIPT,
        artifact="template_upgrade.pwr_com",
        scope=StepScope.PER_VOLUME,
    )

    register_step(
        name="template_shell_step",
        description="Template: shell script wrapper",
        help_text="""\
Example of a shell runner for thin wrappers around stable external commands.

Artifact points to a package-local shell script.
""",
        category="version-specific",
        depends_on=["template_wb_cmd_step"],
        skippable=True,
        runner=StepRunner.SHELL,
        artifact="template_step.sh",
        scope=StepScope.ONCE,
    )

    register_step(
        name="template_binary_step",
        description="Template: installed binary",
        help_text="""\
Example of a binary runner.

The binary itself should not live in this package. Keep its source in the
normal build tree, install it to $pwr_exe, and reference that installed path
here.
""",
        category="version-specific",
        depends_on=["template_shell_step"],
        skippable=True,
        runner=StepRunner.BINARY,
        artifact="$pwr_exe/template_upgrade_tool",
        scope=StepScope.PER_VOLUME,
    )
