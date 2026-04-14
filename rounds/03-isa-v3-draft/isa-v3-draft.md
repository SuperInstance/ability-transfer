# FLUX ISA v3 Draft — Unified Agent Bytecode Specification

**Author:** Datum (Quartermaster)  
**Date:** 2026-04-13  
**Status:** Draft — Round Table Incorporation  
**Supersedes:** ISA v2 (247 opcodes, 7 formats)  
**Source Material:** Round table critique (Seed, Kimi, DeepSeek) + Oracle1 synthesis (5 consensus hits)

---

## 1. Design Philosophy — From CPU Emulator to Agent Runtime

ISA v2 is a well-engineered RISC-style bytecode that converged across three independent models. It works. But every critic identified the same structural problem: **it thinks like a CPU, not an agent**. ISA v3 reorients the design around three axioms that emerged from the round table:

1. **Agents run in time.** They have deadlines, contention windows, and resource budgets. An ISA without temporal primitives forces agents to spend cycles polling.
2. **Agents share space.** Multi-agent systems need isolation, capability gates, and fuel counters. An ISA without security primitives is a security timebomb.
3. **Agents evolve.** A fixed 256-opcode space is terminal rigidity. An ISA without an extension mechanism cannot survive the pace of AI evolution.

ISA v3 keeps the stack-machine core that makes FLUX bytecode simple to implement and reason about, while adding the three missing primitive classes (temporal, security, extensibility) identified by every critic.

---

## 2. Extension Mechanism — 0xFF Escape Prefix (ISA-002)

### 2.1 The Problem

ISA v2 has 247 of 256 opcode slots occupied. Only 9 free slots remain in a domain that evolves weekly. When the next attention mechanism, quantization scheme, or coordination primitive arrives, the spec must either break backward compatibility or overlay ugly hacks.

### 2.2 The Solution — ESCAPE Prefix

Reserve `0xFF` as the **ESCAPE prefix**. Any bytecode sequence starting with `0xFF` is interpreted as an extended opcode:

```
Format: [0xFF] [extension_id] [payload...]

Where:
  - extension_id (1 byte): Identifies the extension space (0x00-0xFF)
  - payload: Variable length, depends on extension_id

Total extension space: 255 extensions * 256 sub-opcodes = 65,280 extended opcodes
```

### 2.3 Extension Discovery and Negotiation

Runtimes expose their supported extensions via a **capability table**:

```python
FLUX_CAPABILITY_TABLE = {
    "extensions": [0x01, 0x03, 0x10],
    "max_memory": 65536,
    "features": ["temporal", "security", "simd_v1"],
}
```

Agent-to-agent negotiation follows this protocol:

1. Agent A sends `ESCAPE 0x00 PROBE` (query supported extensions)
2. Agent B responds with its capability table
3. Agent A selects a common extension subset for communication
4. If no common extensions exist, fall back to base ISA v3 opcodes

### 2.4 Reserved Extension IDs

| Extension ID | Name | Purpose |
|---|---|---|
| 0x00 | PROBE | Capability negotiation |
| 0x01 | TEMPORAL | Time-aware primitives (DEADLINE, YIELD, PERSIST) |
| 0x02 | SECURITY | Capability gates, memory tagging, sandbox regions |
| 0x03 | ASYNC | SUSPEND/RESUME with continuation handles |
| 0x10-0x1F | TENSOR | Neural compute extensions (SIMD, attention patterns) |
| 0x20-0x2F | STRUCTURED_DATA | JSON, MsgPack, protobuf deserialization |
| 0x30-0x3F | PROBABILISTIC | Sampling distributions (Gumbel, Gaussian, categorical) |
| 0x40-0x4F | GRAPH | Knowledge graph traversal, embedding search |
| 0xF0-0xFF | VENDOR | Vendor-specific extensions |

### 2.5 Migration Path from v2

- Base opcodes (0x00-0xFE) are unchanged — full backward compatibility
- Any v2 program runs unmodified on a v3 runtime
- New programs can opt into extensions by using `0xFF` prefix
- Runtimes that don't support a given extension raise `EXTENSION_NOT_SUPPORTED` instead of `UNKNOWN_OPCODE`

---

## 3. Compressed Instruction Format (ISA-003)

### 3.1 The Problem

Format E ([op][4-byte payload]) is the dominant format in v2, used by 157 of 247 opcodes. For simple operations like MOV, NEG, INC, DEC, DUP — which need 0-1 operands — this wastes 2-3 bytes per instruction. Agent runtimes executing tight perceive-decide-act loops are penalized by 20-30% code bloat.

### 3.2 The Solution — 3-Byte Short Format

Introduce a **compressed format** for the 32 most frequently used opcodes, modeled on RISC-V's C-extension:

```
Short format: [0xFF] [short_opcode] [operand_byte]

Where:
  - 0xFF: Escape prefix (ensures backward compatibility with v2)
  - short_opcode (0xC0-0xDF): Maps to a base opcode
  - operand_byte (1 byte):
    - 0x00-0x1F: Register/immediate addressing mode
    - 0x20-0x7F: Signed 6-bit immediate (-32 to +31)
    - 0x80-0xFF: Unsigned 7-bit immediate (0 to 127)
```

### 3.3 Opcode Short-Format Assignments

These 32 opcodes get compressed forms, selected by frequency analysis of agent bytecode patterns:

| Short Opcode | Base Opcode | Name | Rationale |
|---|---|---|---|
| 0xC0 | 0x00 | HALT | Program termination (every program) |
| 0xC1 | 0x01 | NOP | Padding/debugging |
| 0xC2 | 0x55 | PUSH_i6 | Push 6-bit signed immediate (-32 to +31) |
| 0xC3 | 0x56 | POP | Stack cleanup |
| 0xC4 | 0x10 | ADD | Most common arithmetic op |
| 0xC5 | 0x11 | SUB | Second most common |
| 0xC6 | 0x12 | MUL | Multiplication |
| 0xC7 | 0x15 | NEG | Negation (unary, zero-operand waste in Format E) |
| 0xC8 | 0x16 | INC | Increment (unary) |
| 0xC9 | 0x17 | DEC | Decrement (unary) |
| 0xCA | 0x20 | EQ | Equality comparison |
| 0xCB | 0x22 | LT | Less-than comparison |
| 0xCC | 0x24 | GT | Greater-than comparison |
| 0xCD | 0x30 | AND | Bitwise AND |
| 0xCE | 0x31 | OR | Bitwise OR |
| 0xCF | 0x33 | NOT | Bitwise NOT (unary) |
| 0xD0 | 0x50 | JMP_i7 | Jump with 7-bit unsigned offset (0-127) |
| 0xD1 | 0x51 | JZ_i7 | Jump-if-zero with 7-bit offset |
| 0xD2 | 0x52 | JNZ_i7 | Jump-if-not-zero with 7-bit offset |
| 0xD3 | 0x53 | CALL_i7 | Call with 7-bit offset |
| 0xD4 | 0x54 | RET | Return from call |
| 0xD5 | 0x60 | DUP | Stack duplicate (unary) |
| 0xD6 | 0x61 | SWAP | Stack swap |
| 0xD7 | 0x70 | FADD | Float add |
| 0xD8 | 0x71 | FSUB | Float subtract |
| 0xD9 | 0x72 | FMUL | Float multiply |
| 0xDA | 0x80 | CONF_GET | Get confidence |
| 0xDB | 0x81 | CONF_SET | Set confidence |
| 0xDC | 0x90 | SIGNAL | Agent signal |
| 0xDD | 0x91 | BROADCAST | Agent broadcast |
| 0xDE | 0x92 | LISTEN | Agent listen |
| 0xDF | reserved | — | Reserved for future use |

### 3.4 Decoding Strategy

```
function decode(pc):
    opcode = bytecode[pc]
    if opcode == 0xFF:
        extension_id = bytecode[pc + 1]
        if 0xC0 <= extension_id <= 0xDF:
            // Short format: 3 bytes total
            operand = bytecode[pc + 2]
            return SHORT_DECODE_TABLE[extension_id - 0xC0](operand)
            pc += 3
        else:
            // Other extension: variable length
            return EXTENSION_DECODE_TABLE[extension_id](pc + 2)
    else:
        // Standard v2 format: variable length
        return STANDARD_DECODE_TABLE[opcode](pc)
```

### 3.5 Size Impact Analysis

Based on typical agent bytecode patterns:

| Pattern | v2 Size | v3 Short Size | Savings |
|---|---|---|---|
| `PUSH 1; ADD; PUSH 1; SUB` | 18 bytes | 10 bytes | 44% |
| `DUP; MUL; DUP; MUL; ADD` | 26 bytes | 15 bytes | 42% |
| Tight loop (DEC; JNZ nearby) | 8 bytes | 6 bytes | 25% |
| `PUSH 1000000` (large imm) | 8 bytes | 8 bytes | 0% (falls back to standard) |

Average expected savings: **25-35%** for typical agent programs. The short format is most effective for tight loops and simple arithmetic sequences — exactly the hot paths in agent perceive-decide-act cycles.

---

## 4. Temporal Primitives (TEMP-001, DeepSeek)

### 4.1 The Problem

Agents operate in real time. They have deadlines ("respond within 200ms"), resource contention windows ("another agent is using the GPU"), and durability requirements ("save state before shutdown"). The current ISA has no way to express any of these. Agents must spend cycles polling, which wastes compute and increases latency.

### 4.2 Extension 0x01: TEMPORAL

All temporal opcodes use the `0xFF 0x01` prefix:

#### FUEL_CHECK
```
Format: [0xFF] [0x01] [0x00]
Behavior: Pushes remaining fuel counter (number of instructions before forced yield).
Use: Cooperative scheduling — agents check fuel before expensive operations.
Stack: -> [fuel_remaining:u32]
```

#### DEADLINE_BEFORE
```
Format: [0xFF] [0x01] [0x01] [timestamp:u32] [jump_target:u16]
Behavior: If current time >= timestamp, jump to target. Otherwise, continue.
         The timestamp is milliseconds since program start.
Use: Timeout guards on perception-decision-action loops.
Stack: Unchanged
```

#### YIELD_IF_CONTENTION
```
Format: [0xFF] [0x01] [0x02] [resource_id:u8]
Behavior: Check if the specified resource (memory region, channel, device) is contended.
         If contended by another agent, suspend execution and yield to scheduler.
         If not contended, continue immediately.
         Returns 1 on yield, 0 on continue.
Use: Cooperative resource sharing without busy-waiting.
Stack: -> [yielded:0|1]
```

#### PERSIST_CRITICAL_STATE
```
Format: [0xFF] [0x01] [0x03] [region_start:u16] [region_size:u16]
Behavior: Asynchronously persist the specified memory region to durable storage.
         Non-blocking: returns immediately. Completion signaled via channel.
Use: Checkpoint critical agent state before risky operations.
Stack: -> [persist_id:u32]
```

#### TIME_NOW
```
Format: [0xFF] [0x01] [0x04]
Behavior: Pushes current timestamp (ms since program start) onto the stack.
Use: Timing operations, timeout calculations.
Stack: -> [timestamp_ms:u32]
```

#### SLEEP_UNTIL
```
Format: [0xFF] [0x01] [0x05] [timestamp:u32]
Behavior: Suspend execution until the specified timestamp. Releases CPU to scheduler.
Use: Periodic agents (e.g., scan every 1000ms).
Stack: Unchanged
```

### 4.3 Design Rationale

DeepSeek identified temporal operations as "a genuinely new primitive class" and they are right. CPU ISAs do not need temporal primitives because they execute synchronously under an OS scheduler. Agent runtimes are fundamentally different: agents cooperate for resources, have real-time constraints, and must survive interruptions. These six opcodes cover the essential temporal operations without over-engineering. The key insight is that agents are not programs — they are processes that exist in time and must reason about time.

---

## 5. Security Primitives (SEC-001)

### 5.1 The Problem

Multi-agent systems run untrusted code from multiple agents on shared hardware. Without security primitives, a malicious or buggy agent can read or write another agent's memory, consume infinite resources (denial of service), or access privileged operations without authorization. Every critic flagged this as a critical gap — Kimi called it "a security timebomb" and DeepSeek demanded "hardware isolation (minimum 10 opcodes)."

### 5.2 Extension 0x02: SECURITY

#### CAP_INVOKE
```
Format: [0xFF] [0x02] [0x00] [capability_id:u16] [n_args:u8] [args...]
Behavior: Invoke an operation protected by a capability gate.
         The capability_id must be held by the current agent context.
         If the capability is not held, raise CAPABILITY_DENIED (error code 0xE0).
         Capabilities are granted by the runtime at agent creation time
         and cannot be forged or transferred without explicit delegation.
Use: Protected access to hardware (GPU, sensors), network, filesystem.
Stack: [args...] -> [result]
```

#### MEM_TAG
```
Format: [0xFF] [0x02] [0x01] [addr:u16] [size:u16] [tag:u8]
Behavior: Tag a memory region with a capability tag (0-255).
         Subsequent LOAD/STORE operations to tagged regions require
         the current agent to hold a matching tag capability.
         Untagged memory is accessible to all agents.
         Inspired by ARM MTE (Memory Tagging Extension).
Use: Memory isolation between agents sharing an address space.
Stack: Unchanged
```

#### SANDBOX_ENTER
```
Format: [0xFF] [0x02] [0x02] [region_start:u16] [region_size:u16] [permissions:u8]
Behavior: Restrict the agent's memory access to the specified region.
         permissions bitmask:
           bit 0: READ (load from sandbox)
           bit 1: WRITE (store to sandbox)
           bit 2: EXECUTE (run code in sandbox)
           bit 3: ESCAPE (use 0xFF extensions within sandbox)
           bit 4: SIGNAL (send signals from sandbox)
           bit 5: LISTEN (receive signals in sandbox)
         Any access outside the sandbox raises SANDBOX_VIOLATION (error code 0xE1).
         The previous permissions are saved on a sandbox stack.
Use: Isolate untrusted agent code before execution.
Stack: Unchanged
```

#### SANDBOX_EXIT
```
Format: [0xFF] [0x02] [0x03]
Behavior: Exit the current sandbox, restoring previous memory permissions
         from the sandbox stack. If no sandbox is active, this is a no-op.
Use: Return from sandboxed execution.
Stack: Unchanged
```

#### FUEL_SET
```
Format: [0xFF] [0x02] [0x04] [fuel_limit:u32]
Behavior: Set the fuel counter for the current execution context.
         The agent is forcibly halted when fuel reaches zero.
         A fuel limit of 0 means unlimited (default).
         The fuel counter decrements by 1 for each base opcode executed.
         Extended opcodes (0xFF) may cost more than 1 fuel.
Use: Cooperative preemption, denial-of-service prevention, resource budgeting.
Stack: Unchanged
```

#### IDENTITY_GET
```
Format: [0xFF] [0x02] [0x05]
Behavior: Push the current agent's identity handle onto the stack.
         The identity handle is an opaque u32 value assigned by the runtime
         at agent creation time. It cannot be forged or changed.
         Two agents in the same runtime will always have different handles.
Use: Auditing, access control, logging, multi-agent coordination.
Stack: -> [identity_handle:u32]
```

### 5.3 Capability Model

Capabilities follow the **possession model** (similar to Unix file descriptors):

1. **Creation:** The runtime grants capabilities at agent creation time, based on agent role and trust level
2. **Delegation:** An agent can delegate a subset of its capabilities to another agent via `SIGNAL` with a capability handle as payload
3. **Revocation:** The runtime can revoke capabilities at any time (e.g., on agent misbehavior, resource pressure)
4. **Inheritance:** Child agents (created via FORK in a sandboxed context) inherit a restricted subset of the parent's capabilities

This is deliberately simpler than full capability-based security (like seL4 or Fuchsia) because agent runtimes need low overhead. The goal is to raise the bar from "no security" to "meaningful isolation," not to achieve formal verification. The overhead of a capability check is a single hash table lookup — negligible compared to the cost of an LLM inference step.

### 5.4 Error Codes

| Code | Name | Description |
|---|---|---|
| 0xE0 | CAPABILITY_DENIED | Agent attempted CAP_INVOKE without holding the required capability |
| 0xE1 | SANDBOX_VIOLATION | Agent accessed memory outside its sandbox region |
| 0xE2 | FUEL_EXHAUSTED | Agent's fuel counter reached zero |
| 0xE3 | TAG_MISMATCH | Agent accessed tagged memory without matching tag |
| 0xE4 | EXTENSION_NOT_SUPPORTED | Agent used an extension not supported by the runtime |
| 0xE5 | INVALID_CAPABILITY | Capability handle is malformed or has been revoked |

---

## 6. Async Primitives (ASYNC-001)

### 6.1 The Problem

Agents are event-driven. They receive signals, respond to queries, and coordinate with other agents. The current ISA is synchronous: once execution starts, it runs to completion (or HALT). There is no way to pause execution, wait for an external event, and resume later. This forces agents to implement complex state machines manually, or to poll channels in tight loops (wasting fuel).

### 6.2 Extension 0x03: ASYNC

#### SUSPEND
```
Format: [0xFF] [0x03] [0x00] [resume_channel:u8]
Behavior: Suspend the current execution context. Save full VM state
         (stack, memory, PC, flags, call stack, fuel counter) as a continuation.
         Register the continuation on the specified channel.
         The scheduler may run other agents while this one is suspended.
         When a message arrives on the resume_channel, the agent is
         eligible for resumption (the message becomes the stack top on resume).
Use: Wait for external events without blocking the runtime.
Stack: [state...] -> (saved to continuation), resumes with [message, state...]
```

#### RESUME
```
Format: [0xFF] [0x03] [0x01]
Behavior: Restore the most recent continuation from the current
         agent's wait queue and resume execution from the saved PC.
         If no continuation exists, push 0 and continue.
Use: Explicit resume after SUSPEND (usually called by the scheduler, not the agent).
Stack: -> [has_continuation:0|1]
```

#### FORK
```
Format: [0xFF] [0x03] [0x02] [entry_point:u16] [stack_share:u8]
Behavior: Create a new execution context (lightweight agent/task) starting
         at the specified address.
         stack_share determines stack behavior:
           0x00: Copy current stack (full snapshot)
           0x01: Share stack memory (cow semantics)
           0x02: Empty stack
         The new context gets a copy of memory and flags.
         Returns the new context's ID.
Use: Parallel execution within an agent (e.g., fan-out queries to multiple agents).
Stack: -> [context_id:u32]
```

#### JOIN
```
Format: [0xFF] [0x03] [0x03] [context_id:u32]
Behavior: Wait for the specified context to complete (reach HALT or final SUSPEND).
         If the context has already completed, return immediately.
         Pushes the context's final stack top value.
         If the context was cancelled, pushes 0.
Use: Fan-in parallel results, barrier synchronization.
Stack: -> [result_value]
```

#### CANCEL
```
Format: [0xFF] [0x03] [0x04] [context_id:u32]
Behavior: Cancel a previously FORKed context. The context's resources
         (stack, continuations) are freed. If the context is currently
         running, it will be preempted at the next fuel check.
         Returns 1 if the context was found and cancelled, 0 if not found.
Use: Timeout/cleanup for parallel operations, cancellation tokens.
Stack: -> [cancelled:0|1]
```

#### AWAIT_CHANNEL
```
Format: [0xFF] [0x03] [0x05] [channel:u8] [timeout_ms:u16]
Behavior: Wait for a message on the specified channel, with an optional timeout.
         If a message is available, push it and continue.
         If timeout_ms is 0, this is non-blocking (push 0 if no message).
         If timeout_ms is 0xFFFF, wait indefinitely (same as SUSPEND).
         If timeout expires, push 0 and continue.
Use: Message-based coordination with timeout semantics.
Stack: -> [message_or_0]
```

### 6.3 Continuation Handle Format

```
struct ContinuationHandle {
    context_id: u32,      // Unique ID for this execution context
    resume_channel: u8,   // Channel that triggers resumption
    state: enum,          // RUNNING, SUSPENDED, COMPLETED, CANCELLED
    saved_pc: u16,        // Program counter at suspension point
    stack_depth: u16,     // Number of stack elements saved
    fuel_remaining: u32,  // Fuel counter at suspension point
    created_at: u32,      // Timestamp of creation
    suspended_at: u32,    // Timestamp of last suspension
}
```

### 6.4 Interaction with Security Extension

When an agent is SUSPENDed inside a SANDBOX_ENTER, the sandbox state is preserved. Upon RESUME, the agent continues within its sandbox. This ensures that security invariants are maintained across suspension boundaries. A FORKed context inherits the parent's sandbox restrictions.

---

## 7. Category Restructuring — What Moves Where

### 7.1 What Stays in Base ISA (0x00-0xFE)

These are the computational primitives that every runtime must implement:

| Range | Category | Opcodes | Status |
|---|---|---|---|
| 0x00-0x02 | System | HALT, NOP, BREAK | Keep |
| 0x10-0x17 | Integer Arithmetic | ADD through DEC | Keep |
| 0x20-0x25 | Comparison | EQ through GE | Keep |
| 0x30-0x35 | Logic/Bitwise | AND through SHR | Keep |
| 0x40-0x44 | Memory | LOAD through POKE | Keep |
| 0x50-0x56 | Control Flow | JMP through POP | Keep |
| 0x60-0x63 | Stack | DUP through ROT | Keep |
| 0x70-0x73 | Float | FADD through FDIV | Keep |
| 0x80-0x82 | Confidence | CONF_GET through CONF_MUL | Keep (see rationale) |
| 0x90-0x92 | A2A | SIGNAL, BROADCAST, LISTEN | Keep (simplified) |

### 7.2 What Moves to Extensions

| Category | Current Range | New Location | Rationale |
|---|---|---|---|
| Viewpoint/Linguistic | 0xA0-0xAF | EXT 0x50 (optional) | Application-level, not a hardware primitive |
| Sensors | 0xB0-0xBF | EXT 0x51 (optional) | I/O belongs in host function interface |
| Tensor/Neural | 0xC0-0xCF | EXT 0x10-0x1F (256 slots) | Neural compute needs 100+ variations, not 16 |
| Collections | 0xD0-0xDF | Library | HashMaps and dictionaries belong in standard libraries |
| Debug | 0xF0-0xFE | EXT 0xE0-0xEF (optional) | Debug introspection is not a runtime primitive |

### 7.3 Confidence Opcodes — Why They Stay

The round table was split on whether confidence opcodes belong in the ISA. Decision: **keep them in base ISA** because:

1. Every agent runtime needs them (they are not domain-specific to any particular AI architecture)
2. They are only 3 opcodes (minimal footprint — 1.2% of opcode space)
3. They were validated by 3 independent models during ISA convergence
4. Moving them to an extension would break v2 backward compatibility for a core feature
5. No critic argued they should be removed — only that they should not grow

---

## 8. Complete Opcode Table — v3

### 8.1 Base Opcodes (0x00-0xFE, unchanged from v2)

```
System Control
  0x00  HALT        Stop execution
  0x01  NOP         No operation
  0x02  BREAK       Debug breakpoint

Integer Arithmetic
  0x10  ADD         a + b
  0x11  SUB         a - b
  0x12  MUL         a * b
  0x13  DIV         a / b (truncate toward zero)
  0x14  MOD         a % b
  0x15  NEG         -a
  0x16  INC         a + 1
  0x17  DEC         a - 1

Comparison
  0x20  EQ          a == b ? 1 : 0
  0x21  NE          a != b ? 1 : 0
  0x22  LT          a < b ? 1 : 0
  0x23  LE          a <= b ? 1 : 0
  0x24  GT          a > b ? 1 : 0
  0x25  GE          a >= b ? 1 : 0

Logic / Bitwise
  0x30  AND         a & b
  0x31  OR          a | b
  0x32  XOR         a ^ b
  0x33  NOT         ~a
  0x34  SHL         a << (b & 0x1F)
  0x35  SHR         a >> (b & 0x1F)

Memory
  0x40  LOAD addr   Push memory[addr]
  0x41  STORE addr  Pop and store to memory[addr]
  0x43  PEEK        Pop addr, push memory[addr]
  0x44  POKE        Pop val, pop addr, store

Control Flow
  0x50  JMP addr    Unconditional jump
  0x51  JZ addr     Jump if Z flag set
  0x52  JNZ addr    Jump if Z flag clear
  0x53  CALL addr   Push PC, jump to addr
  0x54  RET         Pop PC
  0x55  PUSH imm32  Push 4-byte signed immediate
  0x56  POP         Pop and discard

Stack Manipulation
  0x60  DUP         Duplicate top of stack
  0x61  SWAP        Swap top two elements
  0x62  OVER        Copy second element to top
  0x63  ROT         Rotate top three elements

Float Operations
  0x70  FADD        float(a) + float(b)
  0x71  FSUB        float(a) - float(b)
  0x72  FMUL        float(a) * float(b)
  0x73  FDIV        float(a) / float(b)

Confidence
  0x80  CONF_GET    Push confidence value [0.0, 1.0]
  0x81  CONF_SET    Pop and set confidence (clamped)
  0x82  CONF_MUL    confidence *= pop() (clamped)

Agent-to-Agent
  0x90  SIGNAL ch   Pop val, send to channel ch
  0x91  BROADCAST   Pop val, broadcast to channel ch
  0x92  LISTEN ch   Receive from channel ch, push (0 if empty)

Extension Gateway
  0xFF  ESCAPE      Extension prefix — see Extension IDs below
```

### 8.2 Extension Opcodes (via 0xFF prefix)

```
0xFF 0x00       PROBE                    Capability negotiation
0xFF 0x00 0x00  PROBE_REQUEST            Query remote capabilities
0xFF 0x00 0x01  PROBE_RESPONSE           Respond with capability table

0xFF 0x01       TEMPORAL                 Time/fuel/resource primitives
  0xFF 0x01 0x00  FUEL_CHECK             Push remaining fuel
  0xFF 0x01 0x01  DEADLINE_BEFORE        Conditional jump on deadline
  0xFF 0x01 0x02  YIELD_IF_CONTENTION    Cooperative resource yield
  0xFF 0x01 0x03  PERSIST_CRITICAL_STATE Async state persistence
  0xFF 0x01 0x04  TIME_NOW               Push current timestamp
  0xFF 0x01 0x05  SLEEP_UNTIL            Suspend until timestamp

0xFF 0x02       SECURITY                 Isolation and capability primitives
  0xFF 0x02 0x00  CAP_INVOKE             Protected operation call
  0xFF 0x02 0x01  MEM_TAG                Tag memory region
  0xFF 0x02 0x02  SANDBOX_ENTER          Restrict memory access
  0xFF 0x02 0x03  SANDBOX_EXIT           Restore memory access
  0xFF 0x02 0x04  FUEL_SET               Set fuel limit
  0xFF 0x02 0x05  IDENTITY_GET           Push agent identity handle

0xFF 0x03       ASYNC                    Suspension and parallelism
  0xFF 0x03 0x00  SUSPEND                Save state, wait on channel
  0xFF 0x03 0x01  RESUME                 Restore saved state
  0xFF 0x03 0x02  FORK                   Create parallel context
  0xFF 0x03 0x03  JOIN                   Wait for context completion
  0xFF 0x03 0x04  CANCEL                 Cancel a context
  0xFF 0x03 0x05  AWAIT_CHANNEL          Wait for message with timeout
```

---

## 9. Migration Guide — v2 to v3

### 9.1 For Runtime Implementors

1. **Backward compatibility:** All v2 opcodes (0x00-0xFE) work unchanged on v3 runtimes. Zero migration cost.
2. **New decoder path:** Add an `0xFF` prefix check before the standard opcode dispatch table. This is a 5-line change.
3. **Extension registration:** Implement a capability table that the runtime advertises on `PROBE` requests.
4. **New error codes:** Add `EXTENSION_NOT_SUPPORTED` (0xE4) and `CAPABILITY_DENIED` (0xE0) alongside existing `UNKNOWN_OPCODE`.
5. **Fuel counter:** Implement an optional fuel counter that decrements on each instruction, queried by `FUEL_CHECK` and set by `FUEL_SET`. Default is unlimited (fuel = 0xFFFFFFFF).
6. **Continuation storage:** For ASYNC support, implement continuation serialization (save/restore VM state to a byte buffer).

### 9.2 For Agent Developers

1. **No changes needed for existing code:** All v2 bytecode runs on v3 runtimes without modification.
2. **Opt-in to extensions:** Use the `0xFF` prefix to access temporal, security, and async features when available.
3. **Always negotiate:** Send `PROBE` before using any extension to check runtime support.
4. **Add fuel checks:** Insert `FUEL_CHECK` before expensive operations for cooperative scheduling.
5. **Use sandboxes for untrusted code:** Wrap third-party bytecode in `SANDBOX_ENTER`/`SANDBOX_EXIT`.
6. **Use SUSPEND instead of polling:** Replace `LISTEN; DUP; JZ self` loops with `SUSPEND channel`.

### 9.3 For Fleet Operations

1. **Gradual rollout:** v3 runtimes coexist with v2 — no fleet-wide migration needed.
2. **Capability-based I2I:** Use `PROBE` in I2I handshake to determine remote capabilities before sending extension opcodes.
3. **Security policies:** Define capability grants per agent role. For example: Quartermaster gets filesystem and network access; Navigator gets navigation and sensor capabilities; new agents get a minimal default set.
4. **Fuel budgets:** Set per-agent fuel limits via `FUEL_SET` in the boot sequence to prevent runaway agents.

---

## 10. Open Questions for Round Table Review

1. **Fuel granularity:** Should fuel count instructions, bytes, or estimated cycles? Instructions are simplest but don't account for extension opcode cost differences. Recommendation: instructions with extension multiplier (e.g., SANDBOX_ENTER costs 5 fuel).
2. **Capability persistence:** Should capabilities survive agent restart across sessions? Recommendation: no — re-grant on each spawn. This prevents privilege escalation from stale state.
3. **Continuation memory cost:** Each SUSPEND saves full VM state (stack + memory snapshot). What is the maximum number of concurrent continuations per agent? Recommendation: runtime-defined minimum of 16, no maximum.
4. **Deadline precision:** Should deadlines use wall-clock time or virtual time (fuel-adjusted)? Recommendation: wall-clock for real-time agents, virtual time for deterministic testing mode.
5. **Short format addressing range:** The 7-bit jump offset (0-127 bytes) covers most tight loops but may not cover longer forward jumps. Should we add a 16-bit short format (4 bytes via 0xFF)? Recommendation: not in v3 — use standard Format E for long jumps.

---

## Appendix A: Conformance Test Vectors for v3 Extensions

The following test vectors should be added to `conformance-vectors.json` once v3 runtimes are available. They are organized by extension and cover normal operation, edge cases, and error conditions.

### Temporal Extension Vectors

```
temporal_fuel_check_initial     -> FUEL_CHECK pushes initial fuel > 0
temporal_fuel_decrement         -> After N instructions, FUEL_CHECK shows decreased fuel
temporal_deadline_not_reached   -> DEADLINE_BEFORE far-future, execution continues
temporal_deadline_reached       -> DEADLINE_BEFORE past-timestamp, jump taken
temporal_deadline_preserves     -> DEADLINE_BEFORE with no match, stack unchanged
temporal_time_now_monotonic     -> TIME_NOW returns monotonically increasing values
temporal_sleep_past             -> SLEEP_UNTIL with past timestamp returns immediately
temporal_yield_no_contention    -> YIELD_IF_CONTENTION with free resource returns 0
```

### Security Extension Vectors

```
security_cap_invoke_no_cap      -> CAP_INVOKE without capability raises 0xE0
security_cap_invoke_with_cap    -> CAP_INVOKE with valid capability succeeds
security_mem_tag_isolation      -> LOAD from differently-tagged region raises 0xE3
security_sandbox_read_only      -> STORE inside read-only sandbox raises 0xE1
security_sandbox_exit_restores  -> SANDBOX_EXIT restores full memory access
security_sandbox_nested         -> Nested SANDBOX_ENTER, double EXIT restores original
security_fuel_set_halts         -> FUEL_SET 1, two instructions later agent halts (0xE2)
security_identity_unique        -> IDENTITY_GET returns same value within context
security_identity_different     -> Two contexts return different identity handles
```

### Async Extension Vectors

```
async_suspend_resume_stack      -> SUSPEND saves stack, RESUME restores identical stack
async_fork_returns_id           -> FORK returns a valid non-zero context ID
async_fork_stack_copy           -> FORK with stack_copy=0x00, child has parent's stack
async_join_completes            -> JOIN on completed context returns final stack top
async_join_timeout              -> JOIN on running context suspends caller
async_cancel_prevents           -> CANCEL'd context never reaches HALT
async_cancel_returns_one        -> CANCEL on valid context returns 1
async_cancel_returns_zero       -> CANCEL on invalid context returns 0
async_await_nonblocking         -> AWAIT_CHANNEL with timeout=0 is non-blocking
async_await_with_message        -> AWAIT_CHANNEL with pending message returns it
```

## Appendix B: Round Table Critic Incorporation Matrix

| Critic | Recommendation | v3 Status | Notes |
|---|---|---|---|
| Seed | Sensor-confidence fusion opcodes | Deferred to EXT 0x51 | Kept base ISA lean; library-level concern |
| Seed | Agent-semantic jumps | Deferred | Over-engineering for current fleet needs |
| Seed | Legacy alias removal | Acknowledged | Free slots reclaimed by moving categories to extensions |
| Seed | Unary ops need smaller format | **Implemented** (Section 3) | Short format for NEG, INC, DEC, NOT, DUP |
| Kimi | 0xFF escape prefix | **Implemented** (Section 2) | Core extensibility mechanism |
| Kimi | Single decode path (LEB128) | Deferred to v4 | Breaking change; v3 prioritizes backward compat |
| Kimi | Move domain categories to extensions | **Implemented** (Section 7) | Viewpoint, Sensors, Tensor, Collections, Debug moved |
| Kimi | Stack machine for common case | **Partial** (Section 3) | Short format approximates this without breaking v2 |
| DeepSeek | DEADLINE_BEFORE | **Implemented** (Section 4) | Temporal primitives |
| DeepSeek | YIELD_IF_CONTENTION | **Implemented** (Section 4) | Cooperative resource sharing |
| DeepSeek | PERSIST_CRITICAL_STATE | **Implemented** (Section 4) | Async durability hint |
| DeepSeek | SUSPEND/RESUME | **Implemented** (Section 6) | Full async primitive set |
| DeepSeek | FUEL_CHECK as mandatory yield | **Implemented** (Sections 4, 5) | Cooperative scheduling |
| DeepSeek | CAP_INVOKE | **Implemented** (Section 5) | Capability-based access control |
| DeepSeek | MEM_TAG | **Implemented** (Section 5) | ARM MTE-inspired memory isolation |
| DeepSeek | EMBEDDING_KNN | Deferred to EXT 0x40 | Future work |
| DeepSeek | SAMPLE_DIST | Deferred to EXT 0x30 | Future work |
| DeepSeek | GRAPH_STEP | Deferred to EXT 0x40 | Future work |
| DeepSeek | JSON_NEXT / MSGPACK_PARSE | Deferred to EXT 0x20 | Future work |
| All 3 | Format E monoculture is wasteful | **Implemented** (Section 3) | 32 compressed short-form opcodes |
| All 3 | 256-slot rigidity | **Solved** (Section 2) | 65,280 extension slots via 0xFF |
| All 3 | Missing agent primitives | **Implemented** (Sections 4-6) | Temporal + Security + Async |
| All 3 | Security/isolation absent | **Implemented** (Section 5) | 6 security opcodes + error codes |

---

*This draft incorporates feedback from all three round table critics and Oracle1's synthesis. It is ready for review and iteration. The three key innovations are: (1) the 0xFF escape prefix providing 65K extension slots, (2) compressed short-form opcodes reducing code size by 25-35%, and (3) three new primitive classes (temporal, security, async) that transform FLUX from a CPU emulator into an agent runtime.*
