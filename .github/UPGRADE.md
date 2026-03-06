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
| `steps.py`      | Step registry and ordering. Defines core steps (phase 1 + phase 2) and the `load_version_steps()` dynamic loader |
| `executor.py`   | `StepExecutor` — one `execute_<step>()` method per step. Calls `wb_cmd`, shell commands, file manipulations |
| `utils.py`      | `ProjectEnvironment` dataclass (reads `$pwrp_*` env vars), backup helpers, error types |

#### `version_steps/` — Version-Specific Step Modules

Located in `src/exp/py/src/pwr_upgrade/version_steps/`.

Each module covers one version pair and follows the naming convention `v<from>_to_<to>.py` (e.g. `v60_to_v61.py`). A module must define:

- `VERSION_STEPS` — ordered list of step name strings.
- `register_steps()` — function that calls `register_step()` for each entry.

These steps are inserted between `updateclasses` and `compile` during phase 2. They are loaded dynamically based on the `from_version` / `to_version` in the config.

To add a new version transition, create `version_steps/v61_to_v62.py` and follow the existing pattern.

### wb_cmd Scripts (`.pwr_com`)

These are scripts for the ProviewR workbench command interpreter (`wb_cmd`). They run **inside a loaded volume** and have access to the full object tree through wb_cmd built-in functions (`GetAttribute`, `SetAttribute`, `GetClassListAttrRef`, `CreateObject`, etc.).

| File | Versions | What it does |
|------|----------|--------------|
| `src/exp/com/src/upgrade.pwr_com` | V6.0 → V6.1 | Iterates `PnDevice` objects and sets attribute values that cannot be supplied via class templates — for example `AlarmBuffer.BufferSize = 10` (a `NoEdit` attribute) and `StartupTime` defaults. |
| `src/exp/com/src/upgrade_pb.pwr_com` | V5.4 → V5.5 | Creates `RootVolumeLoad` / `RootVolumeConfig` objects under `SevNodeConfig` and sets `Distribute.Components` bits. |
| `src/exp/com/src/reload_vol_versions.pwr_com` | Any | Saves and restores `ClassVolume.RtVersion` attributes across a reload. Called by `reload.sh`. |

**When to put logic in a `.pwr_com` file:**

- You need to read or write attribute values on objects already in the database.
- The attribute is `NoEdit` or has a compile-time constant that can't be set in a class template.
- You need to create or rearrange objects (e.g. adding a new child object under every instance of a class).
- The change is data-level, not schema-level — the class definition itself hasn't changed, but instances need attribute fixup.

The `.pwr_com` script is invoked by `executor.py` as:

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
| `wb/exe/wb_upgrade/src/wb_upgrade.cpp` | Historical V2.0→V2.1 object-attribute mapping. Dead code, kept for reference. |

### Legacy Scripts

| File | What it does |
|------|--------------|
| `src/exp/com/src/upgradeV451.sh` | Bash script for V4.4→V4.5.1 transition. Historical reference only. |

## Core Upgrade Steps

The following steps are defined in `steps.py` and are version-independent:

### Phase 1 (runs in OLD version, via `--dump`)

| Step | What it does |
|------|--------------|
| `dumpdb` | For every volume listed in `$pwrp_db/pwrp_cnf_volumelist.dat`, call `wb_cmd` to export each volume database to a `.wb_dmp` text file. This is the portable representation. |

### Phase 2 (runs in NEW version, default)

| Step | What it does |
|------|--------------|
| `savedirectory` | Export the directory volume via `wb_cmd list/directory`. |
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

### 1. Schema Changes (attribute rename / class move)

Write a dump-conversion step. Options:

- **AWK/sed script** — add a new file next to `upgrade_dmp.awk`, call it from the version-step executor.
- **Python in executor.py** — parse `.wb_dmp` lines directly in the `execute_*` method if the transformation is simple.

Register the step in `version_steps/v<X>_to_v<Y>.py` and add the executor method in `executor.py`.

### 2. Object-Level Data Fixup (set attribute values post-load)

Write a **`.pwr_com` script**. Place it in `src/exp/com/src/` and name it descriptively (e.g. `upgrade_v62.pwr_com`).

The script is run by `executor.py` as `wb_cmd -q -v <vol> @$pwr_exe/<script>.pwr_com` per volume.

Use `.pwr_com` when you need to:
- Set `NoEdit` attributes to hardcoded constants.
- Provide default values for newly-added attributes where `updateclasses` only zeroes them.
- Create new objects in the tree (e.g. adding a config child under every instance of a class).
- Iterate instances with `GetClassListAttrRef()` and conditionally update.

### 3. External File Conversions (XML, graphics, config files)

Write an executor method in `executor.py` that calls the appropriate tool or does the transformation in Python. Register the step in the version-steps module.

Examples:
- Profinet XML: `wb_convert_pn_xml` (C binary)
- PWG/PWSG graphics: regex replacement in Python (see `execute_remove_lucida_sans`)

### 4. Config / Defaults Update

Update `config.py`:
- Change `DEFAULT_OLD_VERSION` / `DEFAULT_NEW_VERSION`.
- Update the sample config.

### 5. Checklist

```
[ ] version_steps/v<X>_to_v<Y>.py    — register step names + descriptions
[ ] executor.py                       — add execute_<step>() methods
[ ] *.pwr_com (if needed)             — wb_cmd script for object fixup
[ ] upgrade_dmp_vXY.awk (if needed)   — dump text rewriting
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
