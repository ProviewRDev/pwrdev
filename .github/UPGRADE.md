# ProviewR Upgrade Infrastructure

This document describes all files involved in upgrading a ProviewR project from one version to another, and what kind of change goes where when preparing a new version transition.

## Overview

Upgrading a project is a **two-phase process**:

1. **Phase 1 (old version environment):** Dump the project's workbench databases to portable text files (`.wb_dmp`).
2. **Phase 2 (new version environment):** Load the dumps into fresh databases using the new schema, update classes, apply version-specific conversions, recompile, and rebuild boot/load/package files.

Between the phases the developer changes the project version in the administrator (`pwra`) and re-enters the project with `sdf <project>` so that `$pwr_exe`, `$pwrp_db` etc. point to the new installation.

## File Map

### Entry Points

| File | Purpose |
|------|---------|
| `src/exp/com/src/pwr_upgrade.sh` | Shell wrapper — sets PYTHONPATH, invokes `python3 -m pwr_upgrade` |
| `src/exp/com/src/upgrade.sh` | Legacy all-in-one bash upgrade script (V6.0→V6.1). Kept for reference; the Python tool supersedes it. |
| `src/exp/com/src/reload.sh`  | Legacy database reload script. Reads `$pwrp_db/pwrp_cnf_volumelist.dat`. Kept for reference. |

### Python Package — `pwr_upgrade`

Located in `src/exp/py/src/pwr_upgrade/`.

| File | Role |
|------|------|
| `__init__.py`   | Package metadata (`__version__`) |
| `__main__.py`   | Entry point — CLI argument parsing (getopt) |
| `cli.py`        | Rich TUI — banners, step table, prompts, summary |
| `config.py`     | `UpgradeConfig` dataclass; YAML file loading; sample-config generator |
| `steps.py`      | Step registry and ordering. Defines core steps, version-step metadata, and the `load_version_steps()` dynamic loader |
| `executor.py`   | `StepExecutor` — executes built-in core steps and generic runner-backed steps (`python`, `wb_cmd_script`, `binary`, `shell`) |
| `utils.py`      | `ProjectEnvironment` dataclass (reads `$pwrp_*` env vars), backup helpers, error types |

### Interactive Mode

`pwr_upgrade` runs in interactive mode by default. In this mode the user can:

- select a subset of steps before execution,
- inspect detailed help for the next step,
- execute one step at a time with `y`,
- stop the run with `n`,
- switch to unattended execution for the remaining steps with `go`,
- skip the current step with `skip` if and only if that step is marked `skippable=True`.

The `--all` option disables the prompts and runs the selected step list without interaction.
The `--skip` option only accepts steps that are marked skippable.

#### `version_steps/` — Version-Specific Step Modules

Located in `src/exp/py/src/pwr_upgrade/version_steps/`.

Each module covers one version pair and follows the naming convention `v<from>_to_<to>` (for example `v60_to_v61`). It can be a single `.py` file or, preferably when assets are needed, a package directory with `__init__.py`. It must define:

- `VERSION_STEPS` — ordered list of step name strings.
- `register_steps()` — function that calls `register_step()` for each entry.

These steps are inserted between `updateclasses` and `compile` during phase 2. They are loaded dynamically based on the `from_version` / `to_version` in the config.

To add a new version transition, create `version_steps/v61_to_v62.py` for a simple transition or `version_steps/v61_to_v62/` for a transition that needs colocated assets.

A complete safe template is available in `version_steps/_template_transition/`.
It is intentionally ignored by the loader and can be copied when starting a new transition.

Each registered step can also declare:

- `runner` — how it executes (`builtin`, `python`, `wb_cmd_script`, `binary`, `shell`)
- `artifact` — installed tool/script path or module-relative file
- `scope` — `once` or `per_volume`

This is the intended upgrade framework going forward: the version module should mostly describe *what runs*, while `executor.py` provides reusable runner backends.

### Version-Step Runners

Use the smallest execution backend that matches the change:

| Runner | Use for | Typical artifact |
|--------|---------|------------------|
| `builtin` | Shared Python logic that belongs to the core framework | `executor.py` method |
| `python` | Version-local Python logic that should live beside one transition | `remove_lucida_sans.py:run` |
| `wb_cmd_script` | Object-tree traversal, attribute fixups, object creation in a loaded volume | `upgrade.pwr_com` or `$pwr_exe/<script>.pwr_com` |
| `binary` | Heavier schema/API logic, XML parsing, conversions that belong in C/C++ | `$pwr_exe/<tool>` |
| `shell` | Thin wrappers around stable external commands | `script.sh` or `$pwr_exe/<script>.sh` |

The runner choice matters more than the version number. Do not force every version transition to have one binary, one `.pwr_com`, and one shell script. Most transitions should only need one or two step types.

### Binary Placement

Upgrade-specific binaries should **stay in the normal build subsystem** and be referenced from the version-step metadata after installation.

Examples:

- workbench/API tools belong under `wb/exe/...`
- generic upgrade utilities belong under `src/upgrade/exe/...`

The version-step module should then reference the installed artifact, typically with `$pwr_exe/<tool>`.

Do **not** move compiled binaries into `src/exp/py/src/pwr_upgrade/version_steps/...` or another `src/exp` folder just to group them by version. That would fight the build system and blur ownership. Keep the source where the build already expects it, and let the version-step metadata point at the installed result.

### wb_cmd Scripts (`.pwr_com`)

These are scripts for the ProviewR workbench command interpreter (`wb_cmd`). They run **inside a loaded volume** and have access to the full object tree through wb_cmd built-in functions (`GetAttribute`, `SetAttribute`, `GetClassListAttrRef`, `CreateObject`, etc.).

| File | Versions | What it does |
|------|----------|--------------|
| `src/exp/com/src/upgrade.pwr_com` | V6.0 → V6.1 | Legacy/global copy of the V6.1→V7.0 `PnDevice` fixup script. The modern `pwr_upgrade` flow can instead keep version-local `.pwr_com` assets beside the transition package. |
| `src/exp/com/src/upgrade_pb.pwr_com` | V5.4 → V5.5 | Creates `RootVolumeLoad` / `RootVolumeConfig` objects under `SevNodeConfig` and sets `Distribute.Components` bits. |
| `src/exp/com/src/reload_vol_versions.pwr_com` | Any | Saves and restores `ClassVolume.RtVersion` attributes across a reload. Called by `reload.sh`. |

**When to put logic in a `.pwr_com` file:**

- You need to read or write attribute values on objects already in the database.
- The attribute is `NoEdit` or has a compile-time constant that can't be set in a class template.
- You need to create or rearrange objects (e.g. adding a new child object under every instance of a class).
- The change is data-level, not schema-level — the class definition itself hasn't changed, but instances need attribute fixup.

The `.pwr_com` script is invoked by `executor.py` through the `wb_cmd_script` runner as:

```
wb_cmd -q -v <volume> @$pwr_exe/upgrade.pwr_com
```

for every volume in the project. Use `verify(0)` at the top to suppress interactive prompts.

### Dump Conversion Scripts

These operate on the **text dump files** (`.wb_dmp`) before they are loaded into the new database. They are for schema-level changes: renamed attributes, renamed classes, restructured object hierarchies.

| File | What it does |
|------|--------------|
| `src/exp/com/src/upgrade_dmp.awk` | AWK script that renames attributes in `.wb_dmp` text. Historical example: `Sv`/`ATv`/`DTv` class `ActualValue` → `InitialValue`. |
| `src/exp/com/src/upgrade_cnvdmp.sh` | Shell/sed script that renames object-level attributes (`object_did` → `oid`, `window_did` → `woid`) and remaps class references (`pwrb:Class-*` → `SsabOx:Class-*`). |

**When to put logic here:**

- An attribute has been **renamed** in the class definition and existing dump files contain the old name.
- A class has been **moved** to a different volume (class reference strings need rewriting).
- Object-level structural fields (`oid`, `woid`, `poid`) have changed encoding.

These scripts are only needed when the dump text format itself is incompatible. If the class schema simply added a new attribute with a default, `updateclasses` handles it automatically — no dump rewriting needed.

Both scripts are currently historical artifacts from much older version transitions. For a new version pair, add a new AWK/sed file (or handle it in Python inside the version-step executor) rather than editing the old ones.

### Binary Utilities

| File | What it does |
|------|--------------|
| `src/upgrade/exe/upgrade_bckcnv/src/upgrade_bckcnv.c` | Converts backup (`.bck`) files from 32-bit to 64-bit layout. Only relevant for the 32→64-bit platform transition. |
| `wb/exe/wb_upgrade/src/wb_upgrade.cpp` | Historical V2.0→V2.1 object-attribute mapping. Legacy reference, not part of the modern `pwr_upgrade` flow. |

### Legacy Scripts

| File | What it does |
|------|--------------|
| `src/exp/com/src/upgradeV451.sh` | Bash script for V4.4→V4.5.1 transition. Historical reference only. |

## Core Upgrade Steps

The following steps are defined in `steps.py` and are version-independent:

### Phase 1 (runs in OLD version, via `--dump`)

| Step | What it does |
|------|--------------|
| `dumpdb` | For every discovered project database volume, call `wb_cmd` to export the database to a `.wb_dmp` text file. This is the portable representation. |

### Phase 2 (runs in NEW version, default)

| Step | What it does |
|------|--------------|
| `savedirectory` | Save the directory volume with `wb_cmd -q -v directory save`. |
| `classvolumes` | For each class volume, run `co_convert` to regenerate struct/proto header files and wb_load from the new installation's templates. |
| `renamedb` | Backup existing `.db` files to numbered suffixes (`.db.1`, `.db.2`, ...). |
| `loaddb` | Load every `.wb_dmp` back into a fresh database using `wb_cmd wb load`. |
| `updateclasses` | Run `wb_cmd update classes` on each volume so that objects pick up new/changed attributes from the new class definitions. |
| *(version-specific steps inserted here)* | |
| `compile` | `wb_cmd compile /all` — recompile all PLC programs. |
| `createload` | Generate load files for every volume. |
| `createboot` | Create boot files for all nodes. |
| `createpackage` | Build distribution packages (optional). |

## Adding a New Version Transition

When preparing a release that requires project-level migration, here is where each kind of change goes.

### 0. Start with a Version-Step Manifest

Create or update `version_steps/v<X>_to_v<Y>.py` or `version_steps/v<X>_to_v<Y>/` and register each required step with:

- `name`
- `description`
- `depends_on`
- `skippable`
- `runner`
- `artifact` if the runner is not `builtin`
- `scope`

Only put imperative logic in `executor.py` when the generic runners are not enough.

### 1. Schema Changes (attribute rename / class move)

Write a dump-conversion step. Options:

- **version-local Python step** — preferred for simple or moderate text rewriting that should disappear with this version pair.
- **AWK/sed script** — acceptable for historical compatibility or very small line-based rewrites.

Register the step in the version-step module or package. Only add a new executor method if the generic runner model is not enough.

### 2. Object-Level Data Fixup (set attribute values post-load)

Write a **`.pwr_com` script**. Prefer placing it in the version-step package if it is only needed for one transition. Keep it under `src/exp/com/src/` only when it is shared with legacy tooling or otherwise belongs in the global install set.

Register the step with:

- `runner=StepRunner.WB_CMD_SCRIPT`
- `artifact="<script>.pwr_com"` for a package-local asset, or `artifact="$pwr_exe/<script>.pwr_com"` for an installed shared script
- `scope=StepScope.PER_VOLUME`

Use `.pwr_com` when you need to:
- Set `NoEdit` attributes to hardcoded constants.
- Provide default values for newly-added attributes where `updateclasses` only zeroes them.
- Create new objects in the tree (e.g. adding a config child under every instance of a class).
- Iterate instances with `GetClassListAttrRef()` and conditionally update.

### 3. External File Conversions (XML, graphics, config files)

Use either:

- `runner=StepRunner.BINARY` for an installed executable, or
- `runner=StepRunner.PYTHON` for a version-local Python helper.

Examples:
- Profinet XML: `wb_convert_pn_xml` (C binary)
- PWG/PWSG graphics: regex replacement in Python (see `execute_remove_lucida_sans`)

If a new binary is needed, keep its source in the normal build tree (`wb/exe/...`, `src/upgrade/exe/...`, etc.) and reference the installed executable from the version-step metadata. Do not relocate it under `src/exp` just because the upgrade module uses it.

### 4. Config / Defaults Update

Update `config.py`:
- Change `DEFAULT_OLD_VERSION` / `DEFAULT_NEW_VERSION`.
- Update the sample config.

### 5. Checklist

```
[ ] version_steps/v<X>_to_v<Y>.py or /__init__.py — register steps with runner/artifact/scope metadata
[ ] executor.py                       — add code only if a generic runner is not enough
[ ] *.pwr_com (if needed)             — wb_cmd script for object fixup
[ ] upgrade_dmp_vXY.awk (if needed)   — dump text rewriting
[ ] new binary source (if needed)     — place in wb/exe/... or src/upgrade/exe/...
[ ] config.py                         — bump DEFAULT versions
[ ] Test: pwr_upgrade.sh --help       — verify steps appear
[ ] Test: pwr_upgrade.sh --dry-run    — verify step ordering
```

## Environment Variables

The upgrade tool relies on these `$pwrp_*` / `$pwr_*` variables being set (via `sdf <project>`):

| Variable | Content |
|----------|---------|
| `$pwrp_root` | Project root directory |
| `$pwrp_db` | Database directory (`.db`, `.wb_dmp`, volumelist) |
| `$pwrp_cnf` | Configuration directory (YAML config lives here) |
| `$pwrp_log` | Log directory |
| `$pwrp_load` | Load/boot file output directory |
| `$pwrp_exe` | Project executables |
| `$pwr_exe` | System executables (`wb_cmd`, `co_convert`, upgrade scripts) |

## Two-Phase Workflow

```
# Phase 1 — still on the OLD ProviewR version
sdf myproject
pwr_upgrade.sh --dump           # dumps databases

# Change version in administrator
pwra                            # set project to new version, save, exit

# Phase 2 — now on the NEW ProviewR version
sdf myproject                   # re-enter with new env
pwr_upgrade.sh                  # full upgrade
```
