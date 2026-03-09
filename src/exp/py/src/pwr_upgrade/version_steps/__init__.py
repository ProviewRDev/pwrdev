"""
Version-specific upgrade steps.

Each file in this directory defines the extra steps needed for a
particular version transition, e.g. v60_to_v61.py for V6.0 -> V6.1.

A version step module must define:

    VERSION_STEPS: list[str]
        Ordered list of step names. These are inserted between
        'updateclasses' and 'compile' in the upgrade sequence.

    register_steps() -> None
        Function that calls steps.register_step() for each step
        defined in VERSION_STEPS.

Example skeleton:

    from pwr_upgrade.steps import register_step

    VERSION_STEPS = ["my_conversion"]

    def register_steps():
        register_step(
            name="my_conversion",
            description="Do something version-specific",
            help_text="...",
            category="version-specific",
            skippable=True,
        )
"""
