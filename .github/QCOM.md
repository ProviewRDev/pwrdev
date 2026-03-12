# ProviewR QCom (Queue Communication)

QCom is ProviewR's core inter-process and inter-node message-passing system. The name stands for **Queue Communication**, reflecting its queue-based architecture where messages are passed through queues that processes create, attach to, and monitor.

## Overview

QCom provides the foundational messaging infrastructure for ProviewR's runtime system:

- **Inter-Process Communication (IPC):** Processes on the same node exchange messages
- **Inter-Node Communication:** Distributed communication between ProviewR nodes across a network
- **Request/Reply Pattern:** Synchronous communication via `qcom_Request()`/`qcom_Reply()`
- **Publish/Subscribe Pattern:** Asynchronous messaging via broadcast and forward queues
- **Event Notification:** Broadcasts network events (node up/down, link connect/disconnect)

## Architecture

```
┌─────────────────────────────────────────────────────────────────┐
│                     Application Process                          │
│  ┌──────────────────────────────────────────────────────────┐  │
│  │ qcom_Init() → qcom_CreateQ() → qcom_Put()/qcom_Get()     │  │
│  └──────────────────────────────────────────────────────────┘  │
└────────────────────────────┬────────────────────────────────────┘
                             │
                             ▼
┌─────────────────────────────────────────────────────────────────┐
│                    QCom Library (rt_qcom.c)                      │
│  ┌─────────────┐  ┌─────────────┐  ┌──────────────────────┐   │
│  │ Local Queue │  │ Export Queue│  │   QDB (shared        │   │
│  │  (direct)   │  │  (remote)   │  │   memory pool)       │   │
│  └─────────────┘  └──────┬──────┘  └──────────────────────┘   │
└──────────────────────────┼──────────────────────────────────────┘
                           │
                           ▼
┌─────────────────────────────────────────────────────────────────┐
│                   rt_qmon (QCom Monitor)                         │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────────┐     │
│  │Export Thread │  │Import Thread │  │   Link Threads   │     │
│  │(segmentation)│  │(recv UDP)    │  │   (per node)     │     │
│  └──────┬───────┘  └──────┬───────┘  └──────┬───────────┘     │
│         │                 │                  │                  │
│         └─────────── UDP Socket (55000+bus) ─┘                  │
└─────────────────────────────┬───────────────────────────────────┘
                              │
                              ▼
┌─────────────────────────────────────────────────────────────────┐
│                      Network (UDP)                               │
│                    To/From Remote Nodes                          │
└─────────────────────────────────────────────────────────────────┘
```

## Key Data Structures

### Queue Identification

```c
typedef struct {
  qcom_tQix qix;    // Queue index
  pwr_tNodeId nid;  // Node identity
} qcom_sQid;
```

### Application Identification

```c
typedef struct {
  qcom_tAix aix;    // Application index
  pwr_tNodeId nid;  // Node identity
} qcom_sAid;
```

### Queue Types

| Type | Description |
|------|-------------|
| `qcom_eQtype_private` | Standard message queue owned by one process |
| `qcom_eQtype_forward` | Queue that forwards messages to bound queues |
| `qcom_eQtype_broadcast` | Broadcast queue for multi-node delivery |
| `qcom_eQtype_event` | Event notification queue |

### Message Types

**Base types (`qcom_eBtype`):**
- `system` - System messages
- `qcom` - QCom internal messages
- `event` - Event notifications

**Sub types (`qcom_eStype`):**
- `linkConnect` - Link connected
- `linkDisconnect` - Link disconnected
- `linkActive` - Link became active
- `linkStalled` - Link stalled
- `applConnect` - Application connected
- `applDisconnect` - Application disconnected

### Put/Get Structures

```c
typedef struct {
  qcom_sQid reply;        // Reply queue
  qcom_sType type;        // Message type (base + sub)
  unsigned int msg_id;    // Message ID for prioritization
  unsigned int prio;      // Priority
  unsigned int allocate;  // Whether to copy data to pool
  unsigned int size;      // Data size
  void* data;             // Data pointer
} qcom_sPut;

typedef struct {
  qcom_sAid sender;       // Sender application
  pid_t pid;              // Sender process ID
  qcom_sQid receiver;     // Receiver queue
  qcom_sQid reply;        // Reply queue
  qcom_sType type;        // Message type
  qcom_tRid rid;          // Request ID
  unsigned int maxSize;   // Maximum buffer size
  unsigned int size;      // Actual data size
  void* data;             // Data pointer
} qcom_sGet;
```

## API Reference

### Initialization & Exit

| Function | Purpose |
|----------|---------|
| `qcom_Init()` | Initialize QCom for an application |
| `qcom_Exit()` | Disconnect from QCom, release resources |

### Queue Management

| Function | Purpose |
|----------|---------|
| `qcom_CreateQ()` | Create a new queue |
| `qcom_DeleteQ()` | Delete a queue |
| `qcom_AttachQ()` | Attach to an existing queue |
| `qcom_Bind()` | Bind queues for forwarding |
| `qcom_Unbind()` | Unbind queues |

### Message Operations

| Function | Purpose |
|----------|---------|
| `qcom_Put()` | Send a message to a queue |
| `qcom_Get()` | Receive a message from a queue (with timeout) |
| `qcom_Request()` | Combined put+get for request/reply |
| `qcom_Reply()` | Reply to a request |

### Memory Management

| Function | Purpose |
|----------|---------|
| `qcom_Alloc()` | Allocate buffer in QCom pool |
| `qcom_Free()` | Free allocated buffer |

### Event/Signal Operations

| Function | Purpose |
|----------|---------|
| `qcom_WaitAnd()` | Wait for events with AND mask |
| `qcom_WaitOr()` | Wait for events with OR mask |
| `qcom_SignalAnd()` | Signal events with AND mask |
| `qcom_SignalOr()` | Signal events with OR mask |

### Node Information

| Function | Purpose |
|----------|---------|
| `qcom_MyNode()` | Get local node info |
| `qcom_Node()` | Get info for specific node |
| `qcom_NextNode()` | Iterate nodes |
| `qcom_MyBus()` | Get current bus number |

## Usage Examples

### Basic Message Sending

```c
pwr_tStatus sts;
qcom_sQid my_qid, target_qid;
qcom_sPut put;
char message[] = "Hello";

// Initialize QCom
sts = qcom_Init(&sts, 0, "MyApp");

// Create a queue
sts = qcom_CreateQ(&sts, &my_qid, NULL, "MyQueue");

// Send message to target
memset(&put, 0, sizeof(put));
put.type.b = 1;
put.type.s = 1;
put.size = sizeof(message);
put.data = message;

sts = qcom_Put(&sts, &target_qid, &put);
```

### Receiving Messages

```c
qcom_sGet get;
char buffer[1024];

memset(&get, 0, sizeof(get));
get.maxSize = sizeof(buffer);
get.data = buffer;

// Wait up to 1000ms for message
sts = qcom_Get(&sts, &my_qid, &get, 1000);

if (ODD(sts)) {
  // Process message in get.data
  printf("Received: %s\n", (char*)get.data);
}
```

### Request/Reply Pattern

```c
qcom_sPut put;
qcom_sGet get;
char request[] = "GetStatus";
char response[256];

// Setup request
put.reply = my_qid;  // Where to send reply
put.type.b = 100;
put.type.s = 1;
put.size = sizeof(request);
put.data = request;

// Setup receive buffer
get.maxSize = sizeof(response);
get.data = response;

// Send request and wait for reply
sts = qcom_Request(&sts, &target_qid, &put, &my_qid, &get, 5000);
```

## Network Communication

### Message Flow for Remote Communication

1. **Local process** calls `qcom_Put()` with remote destination
2. Message is placed in the **export queue** (`qdb_cQexport`)
3. **rt_qmon's export thread** reads from export queue
4. Message is **segmented** (default 8192 bytes per segment)
5. Segments are **sent via UDP** (port = 55000 + bus number)
6. **Import thread** on receiving node receives segments
7. Segments are **reassembled** into complete message
8. Message is placed in **target queue** on receiver

### Segment Flags

| Flag | Description |
|------|-------------|
| `first` | First segment of a message |
| `middle` | Middle segment |
| `last` | Last segment |
| `single` | Single-segment message (first \| middle \| last) |

### Acknowledgment System

- Each segment requires acknowledgment before next is sent
- Timeout-based resend with exponential backoff
- `MinResendTime` to `MaxResendTime` progression
- Link taken down if no acks received

## Related Components

### rt_qmon (QCom Monitor)

**File:** `src/exe/rt_qmon/src/rt_qmon.c`

The heart of inter-node QCom communication:

- Manages export/import of messages between nodes
- Creates UDP socket on port **55000 + bus number**
- Runs three main threads:
  - **Export thread:** Reads from export queue, segments messages, sends to links
  - **Import thread:** Receives UDP segments, inserts into link queues
  - **Link threads** (one per remote node): Handle send/receive, acknowledgments, reassembly
- Manages link state (connected, active, stalled)
- Broadcasts link events to processes

### rt_neth (Net Handler)

**File:** `src/exe/rt_neth/src/rt_neth.c`

Higher-level network services built on QCom:

- Provides realtime database information to other nodes
- Handles node discovery and volume exchange
- Services: subscriptions, cache operations, object queries
- Waits for rt_qmon before starting

**Relationship:**
```
rt_qmon → Low-level message transport (segments, acks, links)
rt_neth → Application-level protocol (objects, volumes, subscriptions)
```

### NET Library

**File:** `src/lib/rt/src/rt_net.c`

Wrapper functions for QCom calls with XDR encoding:

- `net_Put()` - Send network message with automatic XDR conversion
- `net_Get()` - Receive with XDR decoding
- Handles byte order and platform differences
- Used by both rt_neth and applications

## QDB (QCom Database)

The internal shared memory database for QCom:

**File:** `src/lib/rt/src/rt_qdb.h`

### Key Structures

```c
typedef struct {
  pool_sHead pool;          // Pool allocator header
  // Hash tables for lookups
  // Queue lists
  // Node information
  // Link state
} qdb_sGlobal;

typedef struct {
  // Job-local QCom data
  qcom_sQid eventQ;         // Event queue
  qcom_sAid aid;            // Application identity
} qdb_sLocal;

typedef struct {
  // Queue structure
  pool_sQlink in_lh;        // Input buffer list
  pool_sQlink out_lh;       // Output buffer list
  sem_t sem;                // Semaphore for waiting
} qdb_sQue;

typedef struct {
  // Link statistics and state
  float rtt;                // Round-trip time
  unsigned int ack_count;   // Acknowledgment count
  enum { ... } state;       // Link state
} qdb_sLink;
```

## Key Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `qdb_cVersion` | 1 | QDB structure version |
| `net_cVersion` | 8 | Network protocol version |
| Default segment size | 8192 | Bytes per UDP segment |
| UDP port base | 55000 | Port = 55000 + bus number |
| `net_cProcHandler` | 110 | Queue index for net handler |

## Key Files

| File | Purpose |
|------|---------|
| `src/lib/rt/src/rt_qcom.h` | Public API declarations |
| `src/lib/rt/src/rt_qcom.c` | API implementation |
| `src/lib/rt/src/rt_qdb.h` | Internal database structures |
| `src/exe/rt_qmon/src/rt_qmon.c` | QCom Monitor daemon |
| `src/exe/rt_neth/src/rt_neth.c` | Network handler |
| `src/lib/rt/src/rt_net.c` | Network library with XDR |

## See Also

- [DATABASE_ARCHITECTURE.md](DATABASE_ARCHITECTURE.md) - Database architecture overview
- [GRAPHICS_FRAMEWORK.md](GRAPHICS_FRAMEWORK.md) - Graphics subsystem documentation
