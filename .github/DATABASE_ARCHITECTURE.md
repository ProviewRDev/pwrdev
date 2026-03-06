# ProviewR Database Architecture

This document describes ProviewR's database architecture, including the relationship between configuration-time and runtime databases, storage systems, and the APIs that connect them.

## Overview

ProviewR uses a multi-layered database architecture that separates configuration-time data from runtime data:

```
┌──────────────────────────────────────────────────────────────────────────┐
│                          CONFIGURATION TIME                              │
│  ┌───────────────┐    ┌──────────────┐    ┌───────────────────────────┐ │
│  │  WBL Files    │───▶│     LDH      │───▶│  Editing Database (.db)   │ │
│  │  (.wb_load)   │    │ (Workbench   │    │  BerkeleyDB or MySQL      │ │
│  └───────────────┘    │     API)     │    └───────────────────────────┘ │
│                       └──────┬───────┘                                  │
│                              │ Build/Export                             │
│                              ▼                                          │
│                       ┌──────────────┐                                  │
│                       │  Snapshot    │                                  │
│                       │  (.dbs)      │  Custom binary format            │
│                       └──────┬───────┘                                  │
└──────────────────────────────┼──────────────────────────────────────────┘
                               │
┌──────────────────────────────┼──────────────────────────────────────────┐
│                          RUNTIME                                        │
│                              ▼                                          │
│  ┌───────────────────────────────────────────────────────────────────┐ │
│  │                     RTDB  (Shared Memory)                         │ │
│  │  ┌─────────────────────────────────────────────────────────────┐  │ │
│  │  │  GDB — hash tables: oid, vid, cid, nid, name               │  │ │
│  │  │  Pool allocator, object bodies, volume headers              │  │ │
│  │  └─────────────────────────────────────────────────────────────┘  │ │
│  └───────────────────────────────────────────────────────────────────┘ │
│                              ▲                                          │
│                     ┌────────┴────────┐                                 │
│                     │       GDH       │  Public API for all processes   │
│                     └─────────────────┘                                 │
└─────────────────────────────────────────────────────────────────────────┘
```

## Components

### 1. WBL Files (Workbench Load)

**Purpose:** Text-based source files that define **class definitions** (the schema) and initial object templates. These describe what classes exist, what attributes they have, their types and sizes, and which bodies (RtBody, DevBody, SysBody) they contain.

The `.wb_load` files in the source tree (`*/wbl/`) are the **class volumes** — `pwrb`, `nmps`, `profibus`, etc. Each module's `wbl/` directory defines the classes that module provides. When you configure a project and place e.g. an `Ai` object in the database, the class definition for `Ai` came from a `.wb_load` file in `src/wbl/pwrb/`.

The same text format is also used for `.wb_dmp` (dump) files, which are text exports of user project volumes — those contain the actual object instances and their attribute values.

**Location:** `*/wbl/` directories throughout the codebase (e.g., `src/wbl/`, `nmps/wbl/`, `profibus/wbl/`)

**File Extension:** `.wb_load`

**Format:** Hierarchical text format with object definitions

```
Volume <name> <volume_id>
  Body <class>
    Attr <attribute> = <value>
  EndBody
EndVolume

Object <name> <class>
  Body <body_name>
    Attr <attribute> = <value>
  EndBody
EndObject
```

**Example:**
```
Volume pwrb pwr_eClass_ClassVolume 0.0.0.2
  Body SysBody
    Attr Description = "Base classes"
  EndBody
EndVolume

Object And $ClassDef 1
  Body SysBody
    Attr ObjTime = 27-APR-2006 14:08:12.00
  EndBody
EndObject
```

**Processing:** WBL files are parsed by `wb_vrepwbl` (Volume Repository WBL) which provides a read-only interface to the workbench for class definitions.

**Key Files:**
- `src/lib/wb/src/wb_wblparser.y` - Yacc parser for .wb_load syntax
- `src/lib/wb/src/wb_vrepwbl.cpp` - Volume repository implementation

### 2. Editing Database Storage

**Purpose:** Persistent storage for workbench editing sessions. The default backend is BerkeleyDB (`.db` files); MySQL (`.dbms` files) is available as a compile-time alternative (`PWRE_CONF_MYSQL`).

**Implementation:**
- BerkeleyDB: `wb/lib/wb/src/wb_db.cpp` (includes `db_cxx.h`)
- MySQL: `wb/lib/wb/src/wb_dbms.cpp` (includes `mysql.h`)

**Table Structure:**
| Table | Key | Data | Purpose |
|-------|-----|------|---------|
| `m_t_ohead` | Object ID (oid) | `db_sObject` struct | Object identity (oid, cid), name, parent/sibling/child links, timestamps, body sizes |
| `m_t_rbody` | (oid, bix) | Binary blob | **Runtime body** — actual attribute values (setpoints, parameters, configuration) |
| `m_t_dbody` | (oid, bix) | Binary blob | **Development body** — graphical layout, PLC editor data, documentation fields |
| `m_t_class` | Class ID | `dbs_sClass` struct | Cached class definitions (copied from `.dbs` snapshots, refreshed by "Update Classes") |
| `m_t_name` | (parent oid, name) | oid | Name-to-OID index for object name lookups |
| `m_t_info` | "info" | Volume info | Volume metadata (vid, cid, volume name, timestamp) |

The `.db` file is the **complete working copy** of a volume — the full object tree with all attribute data, not just metadata. When you set `Pump1.MaxFlow = 100.0` in the configurator, that value is stored in `m_t_rbody` as part of the binary body blob for that object.

**Key Classes:**
- `wb_db` - Main database wrapper class
- `wb_db_txn` - Transaction management
- `wb_db_ohead` - Object header operations
- `wb_db_rbody` / `wb_db_dbody` - Body data operations

**Operations:**
```cpp
// Open/create database
wb_db db;
db.open(filename);

// Transaction-based modifications
wb_db_txn txn(&db);
db.put(txn, ohead);  // Write object header
db.commit(txn);
```

### 3. DBS Files (Database Snapshot)

**Purpose:** ProviewR's own binary format for runtime volume loading. These are snapshot files created during the build process from the editing database.

**Location:** `$pwrp_load/<volume>.dbs`

**Implementation:** `wb/lib/wb/src/wb_dbs.cpp` (writer), `src/lib/co/src/co_dbs.h` (format definitions)

**Characteristics:**
- Custom binary format (NOT BerkeleyDB) — uses direct `fwrite()`/`fread()`
- Section-based layout with relative-offset references between sections
- Generated from the editing database (`.db`) during "Create Loadfile" / build
- Loaded by runtime (`rt_ini_load.c`) into GDB/RTDB shared memory at startup
- Also used by the workbench to load read-only class volumes from `$pwr_load/`

**File Layout** (section indices from `dbs_eSect` in `co_dbs.h`):

```
┌─────────────────────┐
│  dbs_sFile           │  File header (version, timestamp)
├─────────────────────┤
│  Section Table [15]  │  dbs_sSect array — offset + size per section
├─────────────────────┤
│  [1] volume          │  dbs_sVolume — vid, cid, name, timestamps
├─────────────────────┤
│  [2] volref          │  Volume references (dependencies on other volumes)
├─────────────────────┤
│  [3] oid             │  OID index (fast oid → object lookup)
├─────────────────────┤
│  [4] object          │  dbs_sObject entries — hierarchy, oid, cid, flags,
│                       │  body offsets (sibling/child linked via queue links)
├─────────────────────┤
│  [5] rbody           │  Runtime body binary blobs (actual attribute values)
├─────────────────────┤
│  [6] name            │  dbs_sName entries — name-to-object index
├─────────────────────┤
│  [8] dbody           │  Development body blobs (graphical/editor data)
├─────────────────────┤
│  [10] class          │  dbs_sClass entries — cached class definitions
├─────────────────────┤
│  [13] scobject       │  Sub-class objects
├─────────────────────┤
│  [14] fixup          │  Object reference fixups (resolve cross-volume refs)
└─────────────────────┘
```

The `.dbs` file is a flattened, immutable export of the same data that lives in the `.db` editing database. Runtime loads it directly into shared memory without needing BerkeleyDB at all.

### 4. co_convert — Build-Time Code Generator

**Purpose:** Reads `.wb_load` class definition files and generates C/C++ header files, help files, and other derived artifacts. This is a **build tool**, not a runtime or workbench component.

**Binary:** `$pwr_exe/co_convert`

**Source:** `src/exe/co_convert/src/co_convert.cpp`

**Key Modes:**

| Flag | Input | Output | Purpose |
|------|-------|--------|---------|
| `-s` | `.wb_load` | `.h` struct files | C struct for each class body (one `.h` per class) |
| `-so` | `.wb_load` | single `.h` | Combined struct header for entire volume |
| `-p` | `.wb_load` | `.hpp` proto files | C++ prototypes |
| `-po` | `.wb_load` | single `.hpp` | Combined prototype header |
| `-w` | `.wb_load` | `.html` | Class documentation |
| `-x` | `.wb_load` | xtthelp | Operator help from class descriptions |
| `-q` | `.wb_load` | PostScript | Printable class documentation |

**When it runs:** During `pwre build module` for any module that has a `wbl/` directory. The generated `.h` files end up in `$pwr_einc/` and are `#include`d by IO and PLC code that accesses class attributes by struct field name.

**Example:** The class definition `Ai` in `src/wbl/pwrb/` produces `pwr_baseclasses.h` containing:

```c
typedef struct {
  pwr_tFloat32 ActualValue;
  pwr_tFloat32 FilteredValue;
  // ... all RtBody attributes
} pwr_sClass_Ai;
```

### 5. GDB (Global DataBase)

**Purpose:** In-memory runtime database that holds all runtime object data after system startup.

**Location:** Shared memory segment

**Key Files:**
- `src/lib/rt/src/rt_gdb.h` - Structure definitions
- `src/lib/rt/src/rt_gdb.c` - Implementation

**Core Structures:**

```c
typedef struct {
  pool_sHead pool;           // Pool allocator header
  gdb_sHashTable oid_ht;     // Object ID hash table
  gdb_sHashTable vid_ht;     // Volume ID hash table
  gdb_sHashTable cid_ht;     // Class ID hash table
  gdb_sHashTable nid_ht;     // Node ID hash table
  gdb_sHashTable name_ht;    // Name hash table
  gdb_sHashTable ms_ht;      // Mount server hash table
  gdb_sHashTable as_ht;      // Alias server hash table
  pool_sQlink vol_lh;        // Volume list head
  pool_sQlink node_lh;       // Node list head
  pool_sQlink subc_lh;       // Subscription client list
  pool_sQlink subs_lh;       // Subscription server list
  pool_sQlink cclass_lh;     // Cached class list
} gdb_sGlobal;
```

**Hash Tables:** GDB uses hash tables for fast lookups by:
- `oid` - Object ID
- `vid` - Volume ID  
- `cid` - Class ID
- `nid` - Node ID
- `name` - Object name path

**Object Structure:**
```c
typedef struct gdb_sObject {
  pool_sQlink obj_ll;        // Object list link
  pwr_tOid oid;              // Object identity
  pwr_tCid cid;              // Class identity
  pwr_sAttrRef aref;         // Attribute reference
  // ... more fields
} gdb_sObject;
```

### 6. GDH (Global Data Handler)

**Purpose:** Public API for runtime data access. This is the main interface applications use to interact with the runtime database.

**Key File:** `src/lib/rt/src/rt_gdh.h`

**Common Functions:**

| Function | Purpose |
|----------|---------|
| `gdh_Init()` | Initialize GDH connection |
| `gdh_NameToObjid()` | Convert object name to OID |
| `gdh_ObjidToName()` | Convert OID to name |
| `gdh_GetObjectInfo()` | Read object attribute value |
| `gdh_SetObjectInfo()` | Write object attribute value |
| `gdh_RefObjectInfo()` | Get direct pointer to object data |
| `gdh_UnrefObjectInfo()` | Release object reference |
| `gdh_GetRootList()` | Get first object in volume |
| `gdh_GetNextSibling()` | Navigate object hierarchy |
| `gdh_GetChild()` | Get first child object |
| `gdh_GetParent()` | Get parent object |
| `gdh_SubRefAdd()` | Add subscription reference |
| `gdh_SubRefRemove()` | Remove subscription reference |

**Usage Example:**
```c
pwr_tStatus sts;
pwr_tOid oid;
pwr_tFloat32 value;

// Initialize
gdh_Init("my_application");

// Get object ID from name
sts = gdh_NameToObjid("H1-Pump1.ActualValue", &oid);

// Read value
sts = gdh_GetObjectInfo(
  cdh_ObjidToAref(&oid), 
  &value, 
  sizeof(value)
);

// Direct pointer access (more efficient for frequent access)
pwr_tFloat32 *pValue;
pwr_tSubscription sub;
sts = gdh_RefObjectInfo(
  cdh_ObjidToAref(&oid),
  (void**)&pValue,
  &sub,
  sizeof(*pValue)
);
// Now *pValue is always current
```

### 7. LDH (Local Data Handler)

**Purpose:** Workbench/configuration API for creating, modifying, and navigating objects during system configuration.

**Key Files:**
- `src/lib/wb/src/wb_ldh.h` - API declarations
- `src/lib/wb/src/wb_ldh.cpp` - Implementation

**Session Management:**
LDH uses sessions to manage configuration changes:

```c
ldh_tSession session;
ldh_tWorkbench workbench;

// Open workbench
ldh_OpenWB(&workbench, db_name, user);

// Start session for modifications
ldh_OpenSession(&session, workbench, vid, ldh_eAccess_ReadWrite);

// Make changes...
ldh_CreateObject(session, &oid, name, cid, parent, ldh_eDest_After);
ldh_SetObjectPar(session, oid, "RtBody", "Description", "My object");

// Commit or revert
ldh_SaveSession(session);
ldh_CloseSession(session);
```

**Key Functions:**

| Category | Functions |
|----------|-----------|
| Session | `ldh_OpenSession()`, `ldh_CloseSession()`, `ldh_SaveSession()`, `ldh_RevertSession()` |
| Object CRUD | `ldh_CreateObject()`, `ldh_DeleteObject()`, `ldh_MoveObject()`, `ldh_CopyObject()` |
| Navigation | `ldh_GetRootList()`, `ldh_GetChild()`, `ldh_GetNextSibling()`, `ldh_GetParent()` |
| Attributes | `ldh_GetObjectPar()`, `ldh_SetObjectPar()`, `ldh_GetObjectBody()` |
| Classes | `ldh_GetClassList()`, `ldh_GetClassBody()`, `ldh_GetAttrDef()` |

### 8. RTDB (Real-Time DataBase)

**Purpose:** Shared memory segment containing all runtime object data, accessible by all processes on a node.

**Location:** `/tmp/pwr_rtdb_<node_id>` (shared memory file)

**Characteristics:**
- Memory-mapped for direct access
- Pool-based memory allocator
- Lock-free reads, synchronized writes
- Contains GDB structures plus object body data

**Pool Allocator:**
The RTDB uses a pool allocator (`pool_sHead`) that provides:
- Fast allocation from pre-allocated pool segments
- Reference counting
- Garbage collection

**Key Files:**
- `src/lib/rt/src/rt_pool.h` - Pool allocator
- `src/lib/rt/src/rt_ini.c` - RTDB initialization

### 9. Volumes

**Purpose:** Logical containers that organize objects. Each volume has a unique ID and contains a hierarchy of objects.

**Volume Types:**

| Type | ID Range | Purpose |
|------|----------|---------|
| RootVolume | 0.1.x.x | Plant hierarchy, main objects |
| ClassVolume | 0.0.0.x | Class definitions |
| SharedVolume | 0.x.x.x | Shared between nodes |
| DirectoryVolume | N/A | Virtual volume for navigation |
| SystemVolume | Special | System configuration |

**Volume Structure:**
```c
typedef struct {
  pwr_tVid vid;           // Volume identity
  pwr_tOid root_oid;      // Root object of volume
  pwr_tCid cid;           // Volume class
  char name[32];          // Volume name
  gdb_sNode *node;        // Owner node
  pool_sQlink obj_lh;     // Object list head
} gdb_sVolume;
```

## Data Flow

### Configuration Time (Workbench)

1. **Load WBL files:** `wb_vrepwbl` parses `.wb_load` files
2. **Merge sources:** Multiple volume repositories (WBL, DB) combined via `wb_merep`
3. **Edit via LDH:** User makes changes through LDH API
4. **Persist to editing database:** Changes saved to `.db` (BerkeleyDB) or `.dbms` (MySQL)
5. **Export:** Build process creates runtime snapshot files (`.dbs`)

### Runtime

1. **Startup:** `rt_ini` creates RTDB shared memory
2. **Load volumes:** `.dbs` files loaded into GDB structures
3. **Build hash tables:** Object, class, volume lookups initialized
4. **Run:** Applications access data via GDH API
5. **Subscriptions:** GDH provides subscription mechanism for change notification

## Component Relationships

```
┌──────────────────────────────────────────────────────────────────┐
│                         WORKBENCH                                 │
│                                                                   │
│  ┌─────────┐    ┌─────────┐    ┌──────────┐    ┌─────────────┐ │
│  │   WBL   │    │  LDH    │    │ wb_erep  │    │ Editing DB  │ │
│  │ Parser  │───▶│  API    │◀──▶│ (Engine) │◀──▶│ (.db/.dbms) │ │
│  └─────────┘    └─────────┘    └──────────┘    └─────────────┘ │
│       │              │                                           │
│       │         Classes &                                        │
│       └────────▶ Objects                                         │
└──────────────────────────────────────────────────────────────────┘
                           │
                     Export/Build
                           │
                           ▼
┌──────────────────────────────────────────────────────────────────┐
│                         RUNTIME                                   │
│                                                                   │
│  ┌─────────────────────────────────────────────────────────────┐│
│  │                      RTDB (Shared Memory)                    ││
│  │  ┌─────────────────────────────────────────────────────────┐││
│  │  │                         GDB                              │││
│  │  │  ┌──────────┐ ┌──────────┐ ┌──────────┐ ┌────────────┐│││
│  │  │  │  Objects │ │ Volumes  │ │  Classes │ │   Nodes    ││││
│  │  │  │ (oid_ht) │ │ (vid_ht) │ │ (cid_ht) │ │  (nid_ht)  ││││
│  │  │  └──────────┘ └──────────┘ └──────────┘ └────────────┘│││
│  │  └─────────────────────────────────────────────────────────┘││
│  └─────────────────────────────────────────────────────────────┘│
│                              ▲                                   │
│                              │                                   │
│  ┌───────────┐    ┌──────────┴───────────┐    ┌───────────────┐│
│  │  PLC/IO   │◀──▶│         GDH          │◀──▶│  Operators    ││
│  │ Processes │    │   (Public API)       │    │   (XTT, Ge)   ││
│  └───────────┘    └──────────────────────┘    └───────────────┘│
└──────────────────────────────────────────────────────────────────┘
```

## Best Practices

### For GDH Users

1. **Prefer `gdh_RefObjectInfo()` over `gdh_GetObjectInfo()`** for frequently accessed data
2. **Always check return status** - network delays or object removal can cause failures
3. **Use subscriptions** for change notification rather than polling
4. **Release references** with `gdh_UnrefObjectInfo()` when done

### For LDH Users

1. **Use sessions** - always work within a session context
2. **Save or revert** - don't leave sessions hanging
3. **Validate before save** - use `ldh_CheckSession()` to detect issues
4. **Handle undo** - LDH supports undo within a session

### For Database Access

1. **Don't access the editing database directly** - use LDH or wb_db/wb_dbms classes
2. **Use transactions** for write operations
3. **Handle concurrent access** - the database layer supports locking

## See Also

- [QCOM.md](QCOM.md) - Queue Communication system for inter-process messaging
- [GRAPHICS_FRAMEWORK.md](GRAPHICS_FRAMEWORK.md) - Graphics subsystem documentation
