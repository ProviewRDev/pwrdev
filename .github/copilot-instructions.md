# ProviewR AI Coding Instructions

## Project Overview

ProviewR is an open-source industrial process control system with a modular architecture designed for automation and SCADA applications. The system follows a component-based design with clear separation between runtime (rt), GUI components (xtt, wb), and industrial protocol modules.

## Core Architecture

### Module Structure
The codebase is organized into specialized modules, each with a consistent directory layout:
- `lib/` - Core libraries and shared functionality  
- `exe/` - Executable programs and applications
- `mmi/` - Man-Machine Interface components
- `wbl/` - Workbench Load files (object definitions)
- `exp/` - Export/interface definitions
- `doc/` - Module documentation

**Key Modules:**
- **rt** - Runtime system and core engine
- **xtt** - Process graphics and HMI (Human Machine Interface) 
- **wb** - Workbench/configurator tools
- **nmps** - Network Message Passing System
- **dataq** - Data queue management for historical data
- **profibus/otherio** - Industrial protocol implementations
- **java** - Java web interface components

### Build System Architecture

ProviewR uses a Perl-based build system (`pwre.pl`) that manages module dependencies and cross-compilation. See `src/doc/man/man_pwre.fodt` for full reference.

**Quick reference:**
```bash
pwre init latest                  # Initialize build environment
pwre module <name>                # Switch to module (rt, xtt, wb, profibus, …)
pwre build module                 # Build current module
pwre method_build <module> gtk    # Build GUI-dependent programs
pwre build_all_modules gtk        # Full project build
pwre configure                    # Check installed dependencies
```

**Build variables** are defined in `src/tools/bld/src/variables.mk`. Key variables: `pwre_btype` (dbg/rls), `pwre_os`, `pwre_hw`, `pwre_conf_gtk`/`pwre_conf_qt`.

**Generic makefiles** in `src/tools/bld/src/` provide reusable rules: `exe_generic.mk`, `lib_generic.mk`, `wbl_generic.mk`, `jpwr_generic.mk`, `jsw_generic.mk`, `msg_generic.mk`.

### Component Integration Patterns

**XTT Navigation System:** The XTT (eXecution Tool and Trends) module uses a hierarchical navigation pattern where `XttGtk` provides GTK-specific UI while core logic resides in platform-agnostic classes. Authorization checks are consistently applied: `if (!xtt->xnav->is_authorized()) return;`

**Data Flow Architecture:** 
- **DataQ** classes handle time-series data with specialized queue implementations (DataQ1, DataQ5, DataQ30, DataQ120 for different time intervals)
- **NMPS** provides distributed system communication with cell-based data structures
- **GDH** (Global Data Handler) manages real-time data access across the system

## Development Conventions

### Debug vs Release Builds

ProviewR supports two build types controlled by `pwre_btype` in `src/tools/bld/src/variables.mk`:

| | Debug (`dbg`) | Release (`rls`) |
|--|--|--|
| Compiler flags | `-g -D_GNU_SOURCE` | `-O3 -D_GNU_SOURCE -DPWR_NDEBUG` |
| `pwr_Assert(a)` | Checks condition, `exit()` on failure | **No-op**: `((void)0)` |
| Optimization | None | Full (`-O3`) |

**Critical rule: Never put side effects inside `pwr_Assert()`.** The expression is completely eliminated in release builds. Use proper error handling instead.

### Key Macros (from `src/exp/inc/src/pwr.h`)

```c
pwr_dStatus(sts, status, ists)     // Declare local status var, init to ists
pwr_Return(value, sts, error)      // Set *sts = error, return value
pwr_ReturnVoid(sts, error)         // Set *sts = error, return (void)
pwr_StatusBreak(var, error)        // Set var = error, break
ODD(sts)                           // Success check (lowest bit set)
EVEN(sts)                          // Failure check (lowest bit clear)
pwr_Assert(condition)              // Debug-only assertion — NO-OP in rls!
```

Always check `EVEN(sts)` after status-returning function calls.

### Shared Memory and Locking

The runtime uses shared memory segments (`/tmp/pwr_rtdb_*`, `/tmp/pwr_pool_*`, `/tmp/pwr_qdb_*`). Locking conventions:

- Use scoped lock macros: `gdb_ScopeLock { ... } gdb_ScopeUnlock` and `qdb_ScopeLock { ... } qdb_ScopeUnlock`
- Use `break` (not `return`) to exit scoped lock blocks — `return` would skip the unlock
- Recursive locks are not allowed — `qdb_LockOwned` check catches double-lock in debug
- Shared memory struct changes require version bump (`gdb_cVersion`, `qdb_cVersion`) or runtime bugcheck

### File Organization Patterns
- GTK implementations use `*_gtk.cpp` suffix pattern
- Platform-neutral logic in base classes (e.g., `Xtt` base, `XttGtk` implementation)
- Configuration objects use `.wb_load` extension for workbench definitions

### Authorization & Security  
All GUI operations require authorization checks using the pattern:

```cpp
if (!xtt->xnav->is_authorized())
  return;
```

System operations may require elevated privileges (`pwr_mPrv_SevAdmin`).

### Error Handling
- Use ProviewR-specific error codes and `pwr_tStatus` return values
- Error logging follows `errh_CErrLog()` pattern with structured error arguments
- Status checking uses `ODD(sts)` macro for success validation

## Critical Developer Knowledge

### Environment Setup
The system requires proper PWR environment initialization before building or working with modules:

```bash
# List build environments
pwre list

# Init for instance the "latest" ProviewR environment
pwre init latest

# Example to change to module profibus
pwre module profibus

# Build current module
pwre build module   

# Build method dependent programs, i.e. wb (gtk)
pwre method_build wb gtk

# To make sure a binary is relinked a touch on a source file in the respective module is needed, e.g.
touch /data0/pwr/profibus/exe/profinet_viewer2/gtk/pn_dcp_tool_gtk.h && pwre build module # Relink the gtk binary

# To build the entire project
pwre build_all_modules gtk
```

The system uses `/usr/local/adm` or local `adm/` directories for configuration. Always initialize the environment before using pwre commands.

### GUI Framework Support  
The system supports both GTK and Qt through compile-time selection. When working on UI components, maintain separation between framework-specific code and business logic.

### Graphics Framework
Layered graphics in `xtt/lib/`: Glow (primitives) → Grow (HMI objects) → specialized contexts. Flow handles PLC flow charts, Ge is the graphics editor, Cow provides common dialogs. Each graphics area has a GTK widget (`*_gtk.cpp`) wrapping a platform-agnostic context.

See [GRAPHICS_FRAMEWORK.md](.github/GRAPHICS_FRAMEWORK.md) for component details, context hierarchy, and widget patterns.

### Database Architecture

**Data flow:** WBL → LDH → BerkeleyDB → (build/export) → DBS → GDB → GDH → RTDB

Key APIs: `gdh_*` for runtime data access, `ldh_*` for workbench configuration. See [DATABASE_ARCHITECTURE.md](.github/DATABASE_ARCHITECTURE.md) for full component reference.

### QCom (Queue Communication)

Core IPC system using shared-memory queues. API: `qcom_Init()`, `qcom_CreateQ()`, `qcom_Put()`, `qcom_Get()`. UDP transport between nodes via `rt_qmon`. See [QCOM.md](.github/QCOM.md) for queue types, network protocol, and patterns.

### Module Dependencies
Understand the build dependency chain when making changes:
- Core system changes require rebuilding dependent modules  
- Protocol modules are largely independent but may depend on core runtime

### Real-time Considerations
This is a real-time industrial control system. Changes to rt module components, data handling, or timing-sensitive code require careful consideration of system performance and deterministic behavior.

### Mixed-Version Runtime
When modifying class definitions or network protocol structs, see [mixed-version-runtime-compatibility.md](.github/mixed-version-runtime-compatibility.md) for rules on safe class changes and when to increment `netver`.

### Code Formatting
- **C/C++**: `.clang-format` at project root (LLVM-based, Allman braces, 110-column limit). Use `clang-format` to format.
- **JavaScript/HTML/CSS**: Prettier via `pnpm format` (targets `java/jsw/**/*.{css,html,js,jsi}`).

### Testing
Tests are standalone C executables in `src/tst/`, `src/lib/rt/tst/`, and `src/lib/co/tst/`. They are built as part of the normal module build — no separate test runner or framework. Run them directly after building.

## Getting Started
1. Examine existing module structure in `src/` for patterns
2. Use `pwre.pl` commands to understand build dependencies  
3. Study `xtt` module for GUI development patterns
4. Review [DATABASE_ARCHITECTURE.md](.github/DATABASE_ARCHITECTURE.md) for data layer understanding
5. Review [QCOM.md](.github/QCOM.md) for inter-process communication
6. Review [UPGRADE.md](.github/UPGRADE.md) for the project upgrade infrastructure
7. Review [mixed-version-runtime-compatibility.md](.github/mixed-version-runtime-compatibility.md) for class change safety rules

The modular architecture allows focused development while maintaining system integrity through well-defined interfaces and consistent build patterns.