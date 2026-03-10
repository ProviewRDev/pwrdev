"""
Version-specific upgrade steps.

Each module or package in this directory defines the extra steps needed
for a particular version transition, e.g. v60_to_v61 for V6.0 -> V6.1.

Use a package directory when the transition needs colocated assets such as
`.pwr_com`, helper Python modules, or small shell scripts.

See `_template_transition/` for a complete non-loadable example package.

A version step module must define:

    VERSION_STEPS: list[str]
        Ordered list of step names. These are inserted between
        'updateclasses' and 'compile' in the upgrade sequence.

    register_steps() -> None
        Function that calls steps.register_step() for each step
        defined in VERSION_STEPS.

Example skeleton:

    from pwr_upgrade.steps import register_step, StepRunner, StepScope

    VERSION_STEPS = ["my_conversion"]

    def register_steps():
        register_step(
            name="my_conversion",
            description="Do something version-specific",
            help_text="...",
            category="version-specific",
            skippable=True,
            runner=StepRunner.WB_CMD_SCRIPT,
            artifact="$pwr_exe/my_upgrade_script.pwr_com",
            scope=StepScope.PER_VOLUME,
        )

Recommended runner choices:

    StepRunner.BUILTIN
        Python code in executor.py. Use for core shared steps.

    StepRunner.PYTHON
        A version-local Python helper module. Use when logic should live
        beside a specific version transition instead of in executor.py.

    StepRunner.WB_CMD_SCRIPT
        A wb_cmd .pwr_com script. Use for object-tree and attribute fixups.
        The artifact can be a package-local file.

    StepRunner.BINARY
        An installed executable in $pwr_exe. Use for heavier API/schema work.

    StepRunner.SHELL
        An installed shell script in $pwr_exe. Use sparingly for thin wrappers
        around stable external commands.

Upgrade-specific binaries should stay in their normal build subsystem
(for example wb/exe/... or src/upgrade/exe/...). The version step should
reference the installed executable path, typically via $pwr_exe/<tool>.
"""
