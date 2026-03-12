# Version Step Template

This directory is a safe copy-from template for new upgrade transitions.

It is intentionally named `_template_transition`, so `pwr_upgrade` will never
load it automatically. The loader only imports exact names of the form:

- `v60_to_v61`
- `v61_to_v70`

## What is included

- `__init__.py`
  Registers one example step for each runner type.
- `python_step.py`
  Example of `StepRunner.PYTHON`.
- `template_upgrade.pwr_com`
  Example of `StepRunner.WB_CMD_SCRIPT`.
- `template_step.sh`
  Example of `StepRunner.SHELL`.

The binary example is defined in `__init__.py` only:

- `runner=StepRunner.BINARY`
- `artifact="$pwr_exe/template_upgrade_tool"`

That is deliberate. Real binary source should stay in the normal build tree,
for example under `wb/exe/...` or `src/upgrade/exe/...`, not inside this
package.

## How to use it

1. Copy `_template_transition` to `v<from>_to_<to>`.
2. Edit `__init__.py` and keep only the steps you actually need.
3. Replace the template assets with real transition logic.
4. If you need a binary, add its source in the normal build subsystem and
   reference the installed `$pwr_exe/<tool>` path from the version package.
