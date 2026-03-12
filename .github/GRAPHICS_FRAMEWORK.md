# ProviewR Graphics Framework Documentation

This document describes the graphics framework implemented in ProviewR, explaining the various components, their abbreviations, purposes, and relationships.

## Overview

ProviewR's graphics framework is a layered system built on GTK (and historically Motif). The framework provides everything from low-level drawing primitives to high-level HMI (Human Machine Interface) components for industrial process control visualization.

## Component Hierarchy

```
Glow (Base Graphics Library)
├── GlowCtx (Base Context)
│   ├── GrowCtx (Rich Graphics Objects)
│   │   ├── CurveCtx (Curves/Trends)
│   │   ├── ColPalCtx (Color Palette)
│   │   └── KeyboardCtx (Virtual Keyboard)
│   └── BrowCtx (Browser/Tree View - Glow variant)
│
Flow (Flow Chart Library)
├── FlowCtx (Flow Context)
│   └── BrowCtx (Browser/Tree View - Flow variant)
```

---

## Core Components

### Glow (Graphics Library for Objects in Windows)

**Location:** `xtt/lib/glow/src/glow_*.cpp`

**Purpose:** Glow is the foundational, low-level graphics drawing library. It provides:
- Base drawing primitives (rectangles, lines, arcs, text, points)
- Drawing context management (`GlowCtx`)
- Window and viewport handling (`GlowWind`)
- Event handling infrastructure
- Coordinate transformation and zooming
- Color and drawing style management

**Key Classes:**
| Class | Purpose |
|-------|---------|
| `GlowCtx` | Base context class managing drawable elements |
| `GlowWind` | Main or navigation window data |
| `GlowArrayElem` | Base class for all drawable elements |
| `GlowRect`, `GlowArc`, `GlowLine`, `GlowText` | Basic drawing primitives |
| `GlowNode`, `GlowNodeClass` | Base node implementations |
| `GlowDraw` | Abstract drawing interface (platform-specific implementations) |

**Context Types** (defined in `glow.h`):
| Type | Purpose |
|------|---------|
| `glow_eCtxType_Glow` | Base Glow context |
| `glow_eCtxType_Grow` | Extended Grow context |
| `glow_eCtxType_Brow` | Browser/tree view context |
| `glow_eCtxType_ColPal` | Color palette context |
| `glow_eCtxType_Curve` | Curve/trend display context |
| `glow_eCtxType_Keyboard` | Virtual keyboard context |

---

### Grow (Graphics with Rich Objects)

**Location:** `xtt/lib/glow/src/glow_grow*.cpp`

**Purpose:** Grow extends Glow to provide rich, interactive graphical objects for process graphics and HMI applications. It adds:
- Complex graphical objects (bars, trends, sliders, menus, images)
- Background colors and images
- Advanced event handling (rubberbanding, selection rectangles)
- Object creation modes for editors
- Subgraph support for reusable components
- Trace/dynamic connection to process data

**Key Classes:**
| Class | Purpose |
|-------|---------|
| `GrowCtx` | Extended context with editing modes (extends `GlowCtx`) |
| `GrowNode` | Subgraph/component objects (extends `GlowNode`) |
| `GrowRect`, `GrowRectRounded` | Rectangle drawing objects |
| `GrowArc`, `GrowLine`, `GrowPolyline` | Shape objects |
| `GrowText`, `GrowAnnot` | Text and annotation objects |
| `GrowTrend`, `GrowXYCurve` | Real-time trend curve display |
| `GrowBar`, `GrowBarChart` | Bar graph objects |
| `GrowSlider` | Slider control |
| `GrowImage` | Image display |
| `GrowMenu`, `GrowToolbar` | UI elements |
| `GrowTable` | Table display |
| `GrowWindow`, `GrowFolder` | Container objects |
| `GrowGroup` | Object grouping |

**Edit Modes:**
- `grow_eMode_Edit` - Normal editing
- `grow_eMode_Rect`, `grow_eMode_RectRounded` - Create rectangles
- `grow_eMode_Line`, `grow_eMode_Polyline` - Create lines
- `grow_eMode_Text`, `grow_eMode_Annot` - Create text
- `grow_eMode_Circle`, `grow_eMode_Arc` - Create circles/arcs
- `grow_eMode_ConPoint` - Create connection points
- `grow_eMode_Scale` - Scale objects

---

### Flow (Flow Chart Editor)

**Location:** `xtt/lib/flow/src/flow_*.cpp`

**Purpose:** Flow is a flow chart editor library used primarily for:
- **PLC Editor** - Programming PLC code visually
- **PLC Trace** - Runtime debugging/monitoring of PLC code

Flow was developed by CS in 1999 to replace Digital's "Neted" editor which wasn't available on Linux.

> "Flow is a flow chart editor used in the plc editor and plc trace to program and troubleshoot plc code. The flow chart consists of nodes that are connected with other nodes by connections."

**Key Classes:**
| Class | Purpose |
|-------|---------|
| `FlowCtx` | Flow chart context |
| `FlowArray`, `FlowNodeArray` | Element storage |
| `FlowNode`, `FlowNodeClass` | Nodes and their graphical definitions |
| `FlowCon`, `FlowConClass` | Connections and their styles |
| `FlowConPoint` | Connection points on nodes |
| `FlowRect`, `FlowArc`, `FlowLine`, `FlowText` | Basic drawing elements |
| `FlowAnnot` | Annotations (text positions in nodes) |

---

### Brow (Browser/Tree View)

**Location:** 
- `xtt/lib/flow/src/flow_browctx.cpp` (extends FlowCtx)
- `xtt/lib/glow/src/glow_browctx.cpp` (extends GlowCtx)

**Purpose:** Brow provides tree-view/browser navigation functionality. It extends either FlowCtx or GlowCtx to provide:
- Hierarchical tree display with indentation
- Parent/child navigation
- Collapsing/expanding nodes
- Annotations with relative positioning

**Key Features:**
- `indentation` - Tree indentation amount
- `annotation_space` - Space between annotations
- `brow_InsertDest` - Insert with destination for tree positioning
- `brow_Collapse()` - Collapse node children
- `get_parent()`, `get_child()`, `get_next_sibling()` - Tree navigation

**Usage:** Used extensively for:
- Navigator trees (WNav, XNav)
- Attribute editors (WAttNav, WdaNav)
- Palette browsers
- Help navigation (XHelpNav)
- Event lists (EvList)

---

### Ge (Graphics Editor)

**Location:** `xtt/lib/ge/src/ge_*.cpp`

**Purpose:** Ge is the Graphics Editor application - the tool used to create and edit process graphics (`.pwg` files). It uses Grow as its graphics engine.

**Key Components:**
| Class | Purpose |
|-------|---------|
| `Graph` | Main class for the Ge drawing area, manages GrowCtx |
| `GraphGrow` | Wrapper class for handling Grow context |
| `GeDyn` | Dynamic behavior/animations for graphics objects |
| `SubGraphs` | Subgraph management (reusable components) |
| `SubPalette` | Object palette for editor |
| `AttrNav` | Attribute navigation/editing |
| `Ge` | The editor application window |

**File Types:**
- `.pwg` - ProviewR graph files (Grow format)
- Subgraphs - Reusable component templates stored in `$pwrp_pop/` directories

---

### Cow (Common Objects for Windows)

**Location:** `xtt/lib/cow/src/cow_*.cpp`

**Purpose:** Cow provides common UI components shared across ProviewR applications:
- Dialogs (message boxes, file selection, input dialogs)
- Navigation components (CoWow, CoXHelp)
- Tree widgets (CoTreeWidget)
- Login functionality (CoLogin)
- Popup menus (CoWowMenu)
- Status bar widgets

---

## Additional Graphics Components

### CurveCtx
**Location:** `xtt/lib/glow/src/glow_curvectx.cpp`

Extends GrowCtx for curve/trend displays. Used for historical data visualization with features like:
- Multiple curves in one view
- Time axis handling
- Zoom and scroll capabilities

### ColPalCtx
**Location:** `xtt/lib/glow/src/glow_colpalctx.cpp`

Color palette context with 300 colors for selection of fill, border, and text colors in the graphics editor.

### KeyboardCtx
**Location:** `xtt/lib/glow/src/glow_keyboardctx.cpp`

Virtual on-screen keyboard supporting:
- Standard keyboard layout
- Numeric keypad
- Alphabetic layouts

### XttGraph / GeCurve
**Location:** `xtt/lib/xtt/src/xtt_ge*.cpp`

Runtime graph display components used by the Xtt operator application to show process graphics.

---

## Widget Classes

### GrowWidget (GTK)
**Location:** `xtt/lib/glow/gtk/glow_growwidget_gtk.cpp`

GTK widget wrapper for Grow contexts. Provides:
- GTK integration for Grow drawing
- Event handling bridge between GTK and Grow
- Scrollbar support via `GtkScrollable` interface
- Accessibility support for on-screen keyboards

### FlowWidget (GTK)
**Location:** `xtt/lib/flow/gtk/flow_widget_gtk.cpp`

GTK widget wrapper for Flow contexts.

---

## Abbreviation Reference

| Abbreviation | Likely Meaning | Purpose |
|--------------|---------------|---------|
| **Glow** | Graphics Library for Objects in Windows | Base drawing primitives and context management |
| **Grow** | Graphics with Rich Objects | Extended objects for HMI/SCADA displays |
| **Flow** | Flow (chart) | PLC programming and debugging diagrams |
| **Brow** | Browser | Tree view navigation component |
| **Ge** | Graphics Editor | Application for creating process graphics |
| **Cow** | Common Objects for Windows | Shared UI components and dialogs |
| **Xtt** | eXecution Tool and Trends | Runtime operator interface |
| **Wb** | Workbench | Configuration/development environment |
| **Nav** | Navigator | Tree-based navigation components |
| **Wnav** | Workbench Navigator | Configuration tree navigation |
| **Xnav** | Xtt Navigator | Runtime object navigation |
| **Wda** | Workbench Data | Data/attribute editing |
| **GeDyn** | Ge Dynamic | Dynamic behavior definitions |

---

## File Naming Conventions

- `glow_*.cpp/h` - Core Glow library files
- `glow_grow*.cpp/h` - Grow extension files (within glow directory)
- `flow_*.cpp/h` - Flow library files
- `ge_*.cpp/h` - Graphics Editor files
- `cow_*.cpp/h` - Common widget files
- `xtt_*.cpp/h` - Xtt runtime files
- `*_gtk.cpp/h` - GTK-specific implementations

---

## Architecture Notes

1. **Platform Abstraction:** The `GlowDraw` base class provides an abstract interface that platform-specific implementations (GTK via `GlowDrawGtk`) implement.

2. **Context Hierarchy:** All graphics contexts inherit from base context classes, allowing code reuse while enabling specialized functionality.

3. **Event-Driven:** The framework uses callback-based event handling where contexts register event callbacks that are invoked by the widget layer.

4. **Object Model:** All drawable elements inherit from base element classes (`GlowArrayElem`, `FlowArrayElem`) enabling uniform handling in arrays and event processing.

5. **Dual Widget/Context:** Each graphics area has both a widget (GTK/platform-specific) and a context (platform-agnostic logic), cleanly separating platform concerns.
