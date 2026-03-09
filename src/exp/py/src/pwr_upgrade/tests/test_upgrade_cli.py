import unittest

from pwr_upgrade.__main__ import build_config
from pwr_upgrade.steps import (
    filter_steps,
    get_phase1_steps,
    get_phase2_steps,
    get_steps_from,
    load_version_steps,
    reset_all_steps,
    select_steps,
)


class StepReferenceTests(unittest.TestCase):
    def setUp(self):
        reset_all_steps()
        load_version_steps("6.0", "6.1")

    def test_phase1_start_selection_stays_in_phase1(self):
        steps = get_phase1_steps()
        selected = get_steps_from("1", steps)

        self.assertEqual([step.name for step in selected], ["dumpdb"])

    def test_filter_steps_supports_numeric_ranges(self):
        steps = get_phase2_steps()
        filtered = filter_steps(steps, ["2-3"])

        self.assertEqual(
            [step.name for step in filtered[:3]],
            ["savedirectory", "loaddb", "updateclasses"],
        )

    def test_select_steps_supports_numeric_references(self):
        steps = get_phase2_steps()
        selected = select_steps(steps, ["3", "1"])

        self.assertEqual(
            [step.name for step in selected],
            ["renamedb", "savedirectory"],
        )


class BuildConfigTests(unittest.TestCase):
    def test_reload_only_mode_is_parsed(self):
        config, command = build_config(
            ["--reload-only", "--all", "--from", "2", "--skip", "1,3-4"]
        )

        self.assertEqual(command, "run")
        self.assertTrue(config.reload_only)
        self.assertFalse(config.interactive)
        self.assertEqual(config.start_from, "2")
        self.assertEqual(config.skip_steps, ["1", "3-4"])

    def test_conflicting_modes_are_rejected(self):
        with self.assertRaisesRegex(ValueError, "cannot be used together"):
            build_config(["--dump", "--reload-only"])


if __name__ == "__main__":
    unittest.main()
