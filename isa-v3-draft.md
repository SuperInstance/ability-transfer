# FLUX ISA v3 — Draft Specification

> **Status**: Draft — Subject to review by FLUX Fleet runtimes  
> **Date**: 2026-04-11  
> **Predecessor**: ISA v2 (247 opcodes, 7 formats, 9 free slots)  
> **Authors**: Synthesized from Seed, Kimi, DeepSeek roundtable critique; Oracle1 mediation

---

## Table of Contents

1. [Design Goals & Philosophy](#1-design-goals--philosophy)
2. [Changes from ISA v2 — Executive Summary](#2-changes-from-isa-v2--executive-summary)
3. [Opcode Address Space Map](#3-opcode-address-space-map)
4. [Instruction Encoding](#4-instruction-encoding)
   - 4.1 [Primary 4-Byte Format (Format E, retained)](#41-primary-4-byte-format-format-e-retained)
   - 4.2 [Compressed 2-Byte Format (new)](#42-compressed-2-byte-format-new)
   - 4.3 [Escape Prefix 0xFF (new)](#43-escape-prefix-0xff-new)
5. [Core Opcodes (0x00–0x1F): System & Control](#5-core-opcodes-0x001f-system--control)
6. [Arithmetic & Logic (0x20–0x3F)](#6-arithmetic--logic-0x203f)
7. [Control Flow (0x40–0x4F)](#7-control-flow-0x400x4f)
8. [Agent Coordination (0x50–0x5F)](#8-agent-coordination-0x500x5f)
9. [Resource & Temporal Primitives (0x60–0x6F) — NEW](#9-resource--temporal-primitives-0x600x6f--new)
10. [Memory & Data (0x70–0x7F)](#10-memory--data-0x700x7f)
11. [Collections (0x80–0x8F)](#11-collections-0x800x8f)
12. [Unified Tensor Range (0xB0–0xDF) — MERGED](#12-unified-tensor-range-0xb00xdf--merged)
13. [System Extension (0xF0–0xFE)](#13-system-extension-0xf00xfe)
14. [Escape Space (0xFF 0x00–0xFF 0xFF)](#14-escape-space-0xff-0x000xff-0xff)
15. [Security Primitives — NEW](#15-security-primitives--new)
16. [Async Primitives — NEW](#16-async-primitives--new)
17. [Removed Opcodes & Migration Guide](#17-removed-opcodes--migration-guide)
18. [Host Function Interface (replaces removed domains)](#18-host-function-interface-replaces-removed-domains)
19. [Design Rationale & Attribution](#19-design-rationale--attribution)
20. [Appendix A: Compressed Opcode Quick Reference](#appendix-a-compressed-opcode-quick-reference)
21. [Appendix B: Escape Sub-Opcode Allocation](#appendix-b-escape-sub-opcode-allocation)
22. [Appendix C: Formal Grammar](#appendix-c-formal-grammar)

---

## 1. Design Goals & Philosophy

### 1.1 Core Principles

**P1 — Minimal Opcode Surface.** The ISA provides *computational primitives*, not domain semantics. Confidence scoring, viewpoint analysis, and sensor fusion are *applications* of primitives, not primitives themselves. *(Consensus: all 3 models; Oracle1 synthesis item 2)*

**P2 — Density Over Uniformity.** Instruction encoding optimizes for the common case. The top 32 most-frequent ops get a compressed 2-byte encoding. Rare ops use 4 bytes. This is not a design flaw — it is the design. *(Seed; Kimi)*

**P3 — Unbounded Extensibility.** The 0xFF escape prefix provides 256 sub-opcodes today, and the scheme nests (0xFF 0xFF opens a third level). No future ISA revision shall ever be blocked by slot exhaustion. *(Kimi — "the biggest single insight"; Oracle1 priority 1)*

**P4 — Security by Default.** Capability-based invocation, memory tagging, and sandboxed regions are first-class primitives, not bolted-on afterthoughts. Agents execute untrusted code; the ISA must protect the host. *(DeepSeek; Kimi; Oracle1 priority 7)*

**P5 — Temporal Awareness.** Agents operate in time. Fuel budgets, deadlines, and contention backoff are architectural primitives, not OS-level hints. *(DeepSeek; Oracle1 priority 5)*

**P6 — Asynchronous First-Class.** Suspend/resume with continuation handles is a native opcode, not a library pattern built on jumps and callbacks. *(Kimi; Oracle1 priority 4)*

### 1.2 What This ISA Is Not

- Not a RISC CPU emulator dressed in AI buzzwords *(Kimi)*
- Not a taxonomy of AI subdomains forced into opcode ranges *(DeepSeek)*
- Not a mechanism for encoding application-level semantics into hardware *(Consensus)*

### 1.3 Design Non-Goals

- Stack machine conversion: Registers are retained for tensor work where data placement matters. The compressed format addresses density without changing the execution model.
- Immediate-heavy format redesign: LEB128 immediates are deferred to ISA v3.1 pending profiling data from real agent workloads. *(Kimi proposed LEB128; retained as future work)*
- Structured data opcodes (JSON, MessagePack): These are host function calls, not ISA primitives.

---

## 2. Changes from ISA v2 — Executive Summary

| # | Action | Category | Attribution |
|---|--------|----------|-------------|
| 1 | **Reserve 0xFF as ESCAPE prefix** (256 sub-opcodes) | Extensibility | Kimi |
| 2 | **Add compressed 2-byte format** for top 32 ops | Density | Kimi, Seed |
| 3 | **Remove confidence range** (0x60–0x6F) — 16 slots freed | Domain cleanup | DeepSeek, Oracle1 |
| 4 | **Remove viewpoint/linguistic range** (0x70–0x7F) — 16 slots freed | Domain cleanup | DeepSeek, Oracle1 |
| 5 | **Remove A2A Fleet range** (0x50–0x5F) — collapse to 2 opcodes | Domain cleanup | Kimi, DeepSeek |
| 6 | **Add resource/temporal primitives** in freed 0x60 range | Agent primitives | DeepSeek |
| 7 | **Merge SIMD (0xB0) + tensor (0xC0)** into unified 0xB0–0xDF | Simplification | Seed, DeepSeek |
| 8 | **Add security primitives**: CAP_INVOKE, MEM_TAG, sandbox | Security | DeepSeek, Kimi |
| 9 | **Add async primitives**: SUSPEND, RESUME, continuation handles | Async | Kimi, DeepSeek |
| 10 | **Move sensors to host function interface** | Domain cleanup | Kimi, DeepSeek |

**Net effect**: ~32 slots freed from domain cleanup → repurposed for primitives. 256+ slots available via escape. Code density improved ~25% via compressed format.

---

## 3. Opcode Address Space Map

```
0x00  ┌─────────────────────────────────────────────────────────┐
      │  SYSTEM & CONTROL (32 slots)                            │
      │  NOP, HALT, LOAD, STORE, MOV, FUEL_CHECK, CAP_INVOKE,  │
      │  MEM_TAG, SANDBOX_ENTER, SANDBOX_LEAVE, ...            │
0x1F  ├─────────────────────────────────────────────────────────┤
      │  ARITHMETIC & LOGIC (32 slots)                          │
      │  ADD, SUB, MUL, DIV, MOD, AND, OR, XOR, SHL, SHR,     │
      │  NEG, NOT, CMP, MIN, MAX, ABS, CLZ, POPCNT, ...       │
0x3F  ├─────────────────────────────────────────────────────────┤
      │  CONTROL FLOW (16 slots)                                │
      │  JMP, JMP_IF, JMP_IF_EQ, JMP_IF_LT, CALL, RET,        │
      │  LOOP, BREAK, DEADLINE_BEFORE, ...                     │
0x4F  ├─────────────────────────────────────────────────────────┤
      │  AGENT COORDINATION (16 slots)                          │
      │  SENDMSG, RECVMSG, SPAWN, JOIN, BROADCAST,             │
      │  QUERY_FLEET, PING, SYNC_BARRIER, ...                  │
0x5F  ├─────────────────────────────────────────────────────────┤
      │  RESOURCE & TEMPORAL (16 slots) ★ NEW IN V3             │
      │  FUEL_CHECK, DEADLINE_BEFORE, YIELD_IF_CONTENTION,     │
      │  PERSIST_CRITICAL_STATE, RESOURCE_QUERY,               │
      │  RESOURCE_ACQUIRE, RESOURCE_RELEASE, ...               │
0x6F  ├─────────────────────────────────────────────────────────┤
      │  MEMORY & DATA (16 slots)                               │
      │  ALLOC, FREE, LOAD8, STORE8, LOAD16, STORE16,          │
      │  LOAD32, STORE32, MEMCPY, MEMSET, MEMCMP, ...          │
0x7F  ├─────────────────────────────────────────────────────────┤
      │  COLLECTIONS (16 slots)                                 │
      │  VEC_NEW, VEC_PUSH, VEC_POP, VEC_LEN, VEC_GET,        │
      │  MAP_NEW, MAP_SET, MAP_GET, MAP_DEL, ...               │
0x8F  ├──────────────────────────┬──────────────────────────────┤
      │  RESERVED (32 slots)      │  0x90–0xAF held for future │
      │  Formerly: sensors, etc.  │  allocation (tensor grow,  │
      │  Now: host functions      │  sparse ops, attention)    │
0xAF  ├──────────────────────────┴──────────────────────────────┤
      │  UNIFIED TENSOR (48 slots) ★ MERGED IN V3               │
      │  T_ADD, T_MUL, T_MATMUL, T_CONV2D, T_ATTN,            │
      │  T_RESHAPE, T_TRANSPOSE, T_CAST, T_QUANTIZE,           │
      │  T_DEQUANTIZE, T_SPARSE_ADD, T_SPARSE_MATMUL,          │
      │  T_GELU, T_SOFTMAX, T_LAYER_NORM, T_FLASH_ATTN,       │
      │  T_MOE_ROUTE, T_GATED_LINEAR, ...                     │
0xDF  ├─────────────────────────────────────────────────────────┤
      │  ASYNC (8 slots) ★ NEW IN V3                            │
      │  SUSPEND, RESUME, CONTINUATION_NEW, CONTINUATION_CALL, │
      │  AWAIT, SELECT, FUTURE_NEW, FUTURE_GET                 │
0xE7  ├─────────────────────────────────────────────────────────┤
      │  LONG JUMPS (8 slots)                                   │
      │  LJMP, LJMP_IF, CALL_FAR, RET_FAR, ...                 │
0xEF  ├─────────────────────────────────────────────────────────┤
      │  SYSTEM EXTENSION (15 slots)                            │
      │  DEBUG_BREAK, DEBUG_LOG, PROF_ENTER, PROF_EXIT,        │
      │  VERSION, FEATURE_QUERY, FEATURE_ENABLE, ...           │
0xFE  ├─────────────────────────────────────────────────────────┤
0xFF  │  ★ ESCAPE PREFIX (256 sub-opcodes) ★ NEW IN V3          │
      │  0xFF 0x00–0xFF: extended opcode space                  │
      │  0xFF 0xFF: nested escape (future)                     │
      │  Sub-allocations defined in Appendix B                 │
      └─────────────────────────────────────────────────────────┘
```

### 3.1 Slots Freed by ISA v3 Cleanup

| Old Range | v2 Category | v3 Disposition | Slots |
|-----------|-------------|----------------|-------|
| 0x60–0x6F | Confidence | Removed → host function interface | 16 |
| 0x70–0x7F | Viewpoint/Linguistic | Removed → host function interface | 16 |
| 0x80–0x8F | Sensors | Removed → host function interface | 16 |
| 0x90–0x9F | Sensor extensions | Removed → host function interface | 16 |
| 0x50–0x5F (partial) | A2A Fleet (14 ops) | Collapsed to 2 core ops (SENDMSG/RECVMSG) | 14 |
| 0xB0–0xBF | SIMD | Merged into unified tensor range | — |
| 0xC0–0xCF | Tensor/Neural | Merged into unified tensor range | — |
| **Total freed** | | | **~76+ slots** |

---

## 4. Instruction Encoding

### 4.1 Primary 4-Byte Format (Format E, retained)

The dominant format from ISA v2 is retained for the general case. It is no longer the *only* format — it is the *fallback* format.

```
Byte 0     Byte 1     Byte 2     Byte 3
┌───────┐ ┌───────┐ ┌───────┐ ┌───────┐
│ OPCODE│ │   rd  │ │  rs1  │ │  rs2  │
│ (8b)  │ │ (8b)  │ │ (8b)  │ │ (8b)  │
└───────┘ └───────┘ └───────┘ └───────┘
```

- **rd**: Destination register (0–255)
- **rs1**: Source register 1 (0–255)
- **rs2**: Source register 2 (0–255)
- Immediate variants encode rs2 as a signed 8-bit immediate (-128 to +127)

Register file: 256 general-purpose registers (r0–r255). r0 is hardwired to zero. The rigidity of 256 registers is a separate concern (addressed by escape-based future extensions, not by this spec).

### 4.2 Compressed 2-Byte Format (new)

Modeled on the RISC-V C-extension. The high bit of byte 0 distinguishes compressed from standard encoding.

#### Decoding Rule

```
If byte[0] < 0x80:  → compressed 2-byte instruction
If byte[0] >= 0x80: → standard 4-byte instruction (byte[0] = opcode)
If byte[0] == 0xFF: → escape prefix (see §4.3)
```

This gives a clean 128-entry compressed opcode space (0x00–0x7F). We initially allocate **32 compressed mappings** for the most frequent operations. The remaining 96 entries (0x20–0x7F) are reserved for future allocation based on runtime profiling data.

#### Encoding Layout

```
Compressed 2-byte:
Byte 0              Byte 1
┌───────┬───────┐  ┌───────┬───────┐
│ COP  │  rs1  │  │  rd   │  rs2  │
│ (5b) │  (3b) │  │ (5b)  │  (3b) │
└───────┴───────┘  └───────┴───────┘

  bits:  7 6 5 4 3     2 1 0       7 6 5 4 3     2 1 0
```

- **COP** (5 bits, 0–31): Compressed opcode index
- **rs1** (3 bits, 0–7): Source register 1 (restricted to r0–r7)
- **rd** (5 bits, 0–31): Destination register (restricted to r0–r31)
- **rs2** (3 bits, 0–7): Source register 2 (restricted to r0–r7)

#### Constraint: Register Window

Compressed instructions operate on a **register window**:
- Destination: registers r0–r31 (common temporaries, loop counters, accumulators)
- Sources: registers r0–r7 ( hottest registers for inner loops)

Rationale: Agent hot loops (perception-action cycles, inference inner loops) use few registers but execute billions of times. Restricting the register window is an acceptable trade for 50% code size reduction. *(Seed; Kimi)*

#### Compressed Opcode Map (Top 32 Operations)

| COP | Full Opcode | Mnemonic | Format | Category |
|-----|-------------|----------|--------|----------|
| 0x00 | 0x01 | `NOP` | E(r0,r0,r0) | System |
| 0x01 | 0x04 | `MOV rd, rs1` | E(rd,rs1,r0) | System |
| 0x02 | 0x20 | `ADD rd, rs1, rs2` | E | Arithmetic |
| 0x03 | 0x21 | `SUB rd, rs1, rs2` | E | Arithmetic |
| 0x04 | 0x22 | `MUL rd, rs1, rs2` | E | Arithmetic |
| 0x05 | 0x30 | `AND rd, rs1, rs2` | E | Logic |
| 0x06 | 0x31 | `OR rd, rs1, rs2` | E | Logic |
| 0x07 | 0x32 | `XOR rd, rs1, rs2` | E | Logic |
| 0x08 | 0x23 | `CMP rd, rs1, rs2` | E | Arithmetic |
| 0x09 | 0x24 | `NEG rd, rs1` | E(rd,rs1,r0) | Arithmetic |
| 0x0A | 0x33 | `SHL rd, rs1, rs2` | E | Logic |
| 0x0B | 0x34 | `SHR rd, rs1, rs2` | E | Logic |
| 0x0C | 0x40 | `JMP offset` | J(offset) | Control |
| 0x0D | 0x42 | `JMP_IF_EQ rd, rs1, offset` | E+imm | Control |
| 0x0E | 0x43 | `JMP_IF_LT rd, rs1, offset` | E+imm | Control |
| 0x0F | 0x44 | `CALL addr` | J(addr) | Control |
| 0x10 | 0x02 | `LOAD rd, [rs1+offset]` | E+imm | Memory |
| 0x11 | 0x03 | `STORE rs1, [rd+offset]` | E+imm | Memory |
| 0x12 | 0x70 | `ALLOC rd, size` | E+imm | Memory |
| 0x13 | 0x60 | `FUEL_CHECK` | I | Resource |
| 0x14 | 0xB0 | `T_ADD rd, rs1, rs2` | E | Tensor |
| 0x15 | 0xB1 | `T_MUL rd, rs1, rs2` | E | Tensor |
| 0x16 | 0xB2 | `T_MATMUL rd, rs1, rs2` | E | Tensor |
| 0x17 | 0x45 | `RET` | I | Control |
| 0x18 | 0x25 | `MIN rd, rs1, rs2` | E | Arithmetic |
| 0x19 | 0x26 | `MAX rd, rs1, rs2` | E | Arithmetic |
| 0x1A | 0x27 | `ABS rd, rs1` | E(rd,rs1,r0) | Arithmetic |
| 0x1B | 0xE0 | `LJMP far_offset` | L | Long Jump |
| 0x1C | 0xE3 | `CALL_FAR far_addr` | L | Long Jump |
| 0x1D | 0x50 | `SENDMSG rs1, rs2` | E | Coordination |
| 0x1E | 0x51 | `RECVMSG rd` | E(rd,r0,r0) | Coordination |
| 0x1F | 0x35 | `NOT rd, rs1` | E(rd,rs1,r0) | Logic |

Compressed entries 0x20–0x7F (96 slots) are **reserved**. Allocation requires a proposal with profiling data showing >1% frequency in representative agent workloads.

---

### 4.3 Escape Prefix 0xFF (new)

This is the single most important structural change in ISA v3. *(Kimi; Oracle1 priority 1)*

#### Overview

Opcode `0xFF` is never a standalone instruction. It is a **prefix byte** that signals: "the next byte is a sub-opcode in extended space."

```
Standard extended encoding (3 bytes minimum):
┌───────┐ ┌───────┐ ┌───────┐ ┌───────┐ ┌───────┐
│ 0xFF  │ │  SUB  │ │  ... operands ...              │
│ ESC   │ │ (8b)  │ │                                │
└───────┘ └───────┘ └────────────────────────────────┘
```

#### Sub-Opcode Encoding

After the 0xFF prefix, the next byte determines the sub-opcode (0x00–0xFF). The operand encoding for each sub-opcode is defined individually — there is no universal format. This allows each extension to use the encoding that best fits its needs.

#### Nested Escape

`0xFF 0xFF` opens a **second level** of escape, providing another 255 sub-opcodes (0xFF 0xFF 0x01 through 0xFF 0xFF 0xFF). This nesting can continue, though it is expected that two levels suffice for the foreseeable future.

```
Level 0: [0x00..0xFE]           = 255 primary opcodes
Level 1: [0xFF 0x00..0xFF 0xFE] = 255 extended sub-opcodes
Level 2: [0xFF 0xFF 0x01..0xFF] = 255 nested sub-opcodes
                                  ─────────────
                                  765 total addressable opcodes
```

#### Discovery & Negotiation

Runtimes MUST implement a capability query mechanism:

```
FEATURE_QUERY feature_id → returns { supported: bool, version: u8 }
```

Before using any escape sub-opcode, a well-behaved program SHOULD query its support. If unsupported, the program MUST fall back to an alternative implementation or trap gracefully.

The runtime MUST return `TRAP_UNKNOWN_ESCAPE` if an unrecognized escape sub-opcode is executed, rather than silently ignoring it.

#### Escape Space Allocation

See [Appendix B](#appendix-b-escape-sub-opcode-allocation) for the full sub-opcode allocation plan. Initial allocations:

| Sub-Opcode | Name | Purpose |
|-----------|------|---------|
| 0x00 | `ESCAPE_NOP` | Explicit no-op in escape space |
| 0x01–0x0F | `FUTURE_MATH_*` | Reserved for future math extensions (bfloat16, float8, etc.) |
| 0x10–0x1F | `FUTURE_TENSOR_*` | Reserved for future tensor extensions (attention variants, sparse) |
| 0x20–0x2F | `FUTURE_SECURITY_*` | Reserved for future security extensions |
| 0x30–0x3F | `FUTURE_ASYNC_*` | Reserved for future async extensions |
| 0x40–0x4F | `HOST_FUNC_*` | Host function interface registration and invocation |
| 0x50–0x5F | `PLATFORM_*` | Platform-specific extensions (ARM, x86, CUDA hints) |
| 0xFE | `ESCAPE_VERSION` | Returns escape space version number |
| 0xFF | `ESCAPE_NEST` | Nested escape — opens level 2 |

---

## 5. Core Opcodes (0x00–0x1F): System & Control

| Opcode | Mnemonic | Operands | Description |
|--------|----------|----------|-------------|
| 0x00 | `NOP` | — | No operation. Pipeline hint. |
| 0x01 | `HALT` | — | Stop execution. Return control to host. |
| 0x02 | `LOAD rd, [rs1+imm8]` | rd, rs1, imm8 | Load 32-bit value from memory. |
| 0x03 | `STORE rs1, [rd+imm8]` | rs1, rd, imm8 | Store 32-bit value to memory. |
| 0x04 | `MOV rd, rs1` | rd, rs1 | Register-to-register copy. |
| 0x05 | `LOADI rd, imm16` | rd, imm16 | Load 16-bit immediate into rd (sign-extended to 32). |
| 0x06 | `LOADI32 rd, imm32` | rd, lo16, hi16 | Load 32-bit immediate (two-instruction sequence). |
| 0x07 | `SWAP rd, rs1` | rd, rs1 | Exchange register values. |
| 0x08 | `SELECT rd, rs1, rs2, cond` | rd, rs1, rs2, cond | Conditional move (predication without branch). |
| 0x09 | `FUEL_CHECK` | — | Cooperative scheduling checkpoint. See §9. |
| 0x0A | `CAP_INVOKE cap, rd` | cap, rd | Capability-gated call. See §15. |
| 0x0B | `FEATURE_QUERY feature_id, rd` | feature_id, rd | Query runtime feature support. See §4.3. |
| 0x0C | `VERSION rd` | rd | Load ISA version into rd (v3.0 = 0x0300). |
| 0x0D | `SANDBOX_ENTER region_id` | region_id | Enter sandboxed memory region. See §15. |
| 0x0E | `SANDBOX_LEAVE` | — | Exit current sandbox region. See §15. |
| 0x0F | `TRAP code` | code | Explicit trap with error code. |
| 0x10–0x1F | *reserved* | — | Reserved for future system primitives. |

---

## 6. Arithmetic & Logic (0x20–0x3F)

| Opcode | Mnemonic | Operands | Description |
|--------|----------|----------|-------------|
| 0x20 | `ADD rd, rs1, rs2` | rd, rs1, rs2 | Integer addition. |
| 0x21 | `SUB rd, rs1, rs2` | rd, rs1, rs2 | Integer subtraction. |
| 0x22 | `MUL rd, rs1, rs2` | rd, rs1, rs2 | Integer multiplication (low 32 bits). |
| 0x23 | `MULH rd, rs1, rs2` | rd, rs1, rs2 | Integer multiplication (high 32 bits). |
| 0x24 | `DIV rd, rs1, rs2` | rd, rs1, rs2 | Signed integer division. Traps on divide-by-zero. |
| 0x25 | `DIVU rd, rs1, rs2` | rd, rs1, rs2 | Unsigned integer division. |
| 0x26 | `MOD rd, rs1, rs2` | rd, rs1, rs2 | Signed remainder. |
| 0x27 | `MODU rd, rs1, rs2` | rd, rs1, rs2 | Unsigned remainder. |
| 0x28 | `FADD rd, rs1, rs2` | rd, rs1, rs2 | IEEE 754 float32 addition. |
| 0x29 | `FSUB rd, rs1, rs2` | rd, rs1, rs2 | IEEE 754 float32 subtraction. |
| 0x2A | `FMUL rd, rs1, rs2` | rd, rs1, rs2 | IEEE 754 float32 multiplication. |
| 0x2B | `FDIV rd, rs1, rs2` | rd, rs1, rs2 | IEEE 754 float32 division. |
| 0x2C | `FCMP rd, rs1, rs2` | rd, rs1, rs2 | Float comparison. rd = -1, 0, or +1. |
| 0x2D | `FNEG rd, rs1` | rd, rs1 | Float negation. |
| 0x2E | `FABS rd, rs1` | rd, rs1 | Float absolute value. |
| 0x2F | `FTOI rd, rs1` | rd, rs1 | Float to integer (truncate toward zero). |
| 0x30 | `AND rd, rs1, rs2` | rd, rs1, rs2 | Bitwise AND. |
| 0x31 | `OR rd, rs1, rs2` | rd, rs1, rs2 | Bitwise OR. |
| 0x32 | `XOR rd, rs1, rs2` | rd, rs1, rs2 | Bitwise XOR. |
| 0x33 | `SHL rd, rs1, rs2` | rd, rs1, rs2 | Shift left logical. |
| 0x34 | `SHR rd, rs1, rs2` | rd, rs1, rs2 | Shift right logical. |
| 0x35 | `SAR rd, rs1, rs2` | rd, rs1, rs2 | Shift right arithmetic. |
| 0x36 | `NOT rd, rs1` | rd, rs1 | Bitwise complement. |
| 0x37 | `NEG rd, rs1` | rd, rs1 | Two's complement negation. |
| 0x38 | `CMP rd, rs1, rs2` | rd, rs1, rs2 | Integer comparison. rd = -1, 0, or +1. |
| 0x39 | `MIN rd, rs1, rs2` | rd, rs1, rs2 | Minimum (integer). |
| 0x3A | `MAX rd, rs1, rs2` | rd, rs1, rs2 | Maximum (integer). |
| 0x3B | `ABS rd, rs1` | rd, rs1 | Absolute value (integer). |
| 0x3C | `CLZ rd, rs1` | rd, rs1 | Count leading zeros. |
| 0x3D | `POPCNT rd, rs1` | rd, rs1 | Population count (bit count). |
| 0x3E | `ITOF rd, rs1` | rd, rs1 | Integer to float conversion. |
| 0x3F | *reserved* | — | Reserved for future. |

---

## 7. Control Flow (0x40–0x4F)

| Opcode | Mnemonic | Operands | Description |
|--------|----------|----------|-------------|
| 0x40 | `JMP offset` | imm8 | Unconditional relative jump (-128 to +127 instructions). |
| 0x41 | `JMP_REG rs1` | rs1 | Unconditional jump to address in rs1. |
| 0x42 | `JMP_IF_EQ rs1, rs2, offset` | rs1, rs2, imm8 | Jump if rs1 == rs2. |
| 0x43 | `JMP_IF_LT rs1, rs2, offset` | rs1, rs2, imm8 | Jump if rs1 < rs2 (signed). |
| 0x44 | `JMP_IF_GT rs1, rs2, offset` | rs1, rs2, imm8 | Jump if rs1 > rs2 (signed). |
| 0x45 | `JMP_IF_NEZ rs1, offset` | rs1, imm8 | Jump if rs1 != 0. |
| 0x46 | `CALL offset` | imm8 | Relative call. Push return address to call stack. |
| 0x47 | `CALL_REG rs1` | rs1 | Indirect call to address in rs1. |
| 0x48 | `RET` | — | Return from call. Pop from call stack. |
| 0x49 | `LOOP rd, offset` | rd, imm8 | Decrement rd; jump if rd != 0. |
| 0x4A | `DEADLINE_BEFORE rd, offset` | rd, imm8 | Temporal jump. See §9. ★ NEW |
| 0x4B | `SWITCH rd, table_offset` | rd, imm8 | Jump table dispatch. rd indexes into table. |
| 0x4C–0x4F | *reserved* | — | Reserved for future control flow. |

---

## 8. Agent Coordination (0x50–0x5F)

This range is **dramatically simplified** from ISA v2's 16-opcode A2A Fleet category. Coordination is reduced to its kernel: message passing and synchronization. All higher-level protocol (task routing, yoke transfer, fleet directory queries) is handled by host functions. *(Kimi; DeepSeek)*

| Opcode | Mnemonic | Operands | Description |
|--------|----------|----------|-------------|
| 0x50 | `SENDMSG rs1, rs2` | rs1, rs2 | Send message. rs1 = capability handle, rs2 = payload pointer. |
| 0x51 | `RECVMSG rd` | rd | Receive next message into rd (blocking). Returns capability handle. |
| 0x52 | `SENDMSG_NB rs1, rs2, rd` | rs1, rs2, rd | Non-blocking send. rd = success flag (0 or 1). |
| 0x53 | `RECVMSG_NB rd` | rd | Non-blocking receive. rd = capability handle or 0 (no message). |
| 0x54 | `SPAWN rs1` | rs1 | Spawn child agent. rs1 = entry point address. |
| 0x55 | `JOIN rd, rs1` | rd, rs1 | Wait for child agent. rs1 = child handle, rd = exit status. |
| 0x56 | `BROADCAST rs1, rs2` | rs1, rs2 | Broadcast message to agent group. rs1 = group capability, rs2 = payload. |
| 0x57 | `SYNC_BARRIER` | — | Barrier synchronization. All agents in group must reach this point. |
| 0x58 | `CHANNEL_NEW rd, type` | rd, imm8 | Create communication channel. Returns channel handle in rd. |
| 0x59 | `CHANNEL_SEND rs1, rs2` | rs1, rs2 | Send on channel. rs1 = channel handle, rs2 = value. |
| 0x5A | `CHANNEL_RECV rd, rs1` | rd, rs1 | Receive on channel. rd = value, rs1 = channel handle. |
| 0x5B | `CHANNEL_CLOSE rs1` | rs1 | Close channel handle. |
| 0x5C–0x5F | *reserved* | — | Reserved for future coordination primitives. |

---

## 9. Resource & Temporal Primitives (0x60–0x6F) — NEW

This range is **entirely new** in ISA v3, occupying space freed by the removal of the confidence category. These primitives address the reality that agents run in time and compete for resources. *(DeepSeek — "the genuine missing primitive class"; Oracle1 priority 5)*

| Opcode | Mnemonic | Operands | Description |
|--------|----------|----------|-------------|
| 0x60 | `FUEL_CHECK` | — | Cooperative scheduling checkpoint. See below. |
| 0x61 | `FUEL_SET rd` | rd | Set remaining fuel budget. rd = fuel units. |
| 0x62 | `FUEL_QUERY rd` | rd | Query remaining fuel. rd = fuel units remaining. |
| 0x63 | `DEADLINE_SET rd` | rd | Set absolute deadline. rd = timestamp (epoch ms). |
| 0x64 | `DEADLINE_QUERY rd` | rd | Query current deadline. rd = timestamp. |
| 0x65 | `DEADLINE_CHECK rd` | rd | Query time remaining until deadline. rd = ms remaining. Traps (TRAP_DEADLINE_EXCEEDED) if expired. |
| 0x66 | `YIELD_IF_CONTENTION rs1` | rs1 | Voluntary backoff. rs1 = resource handle. If resource is contested, yield execution. Returns contention level in rs1 (0 = no contention). |
| 0x67 | `RESOURCE_QUERY rtype, rd` | imm8, rd | Query available resource. rtype: 0=memory, 1=compute, 2=bandwidth, 3=custom. rd = available units. |
| 0x68 | `RESOURCE_ACQUIRE rtype, amount` | imm8, rd | Acquire resource units. rd = amount requested. Returns actual amount acquired (may be less). Traps if zero. |
| 0x69 | `RESOURCE_RELEASE rtype, amount` | imm8, rd | Release previously acquired resource units. |
| 0x6A | `PERSIST_CRITICAL_STATE rs1, rs2` | rs1, rs2 | Async durability hint. rs1 = state pointer, rs2 = size. Runtime SHOULD persist this data to stable storage asynchronously. |
| 0x6B | `ENERGY_QUERY rd` | rd | Query energy state. rd = {battery_pct, thermal_zone}. Encoding: high byte = thermal (0–255°C), low byte = battery (0–100%). |
| 0x6C | `THROTTLE_HINT rs1` | rs1 | Suggest precision throttling. rs1 = 0 (max precision), 1 (half precision), 2 (quarter precision). Runtime MAY comply. |
| 0x6D–0x6F | *reserved* | — | Reserved for future resource/temporal ops. |

### 9.1 FUEL_CHECK Semantics

`FUEL_CHECK` is the **cooperative scheduling primitive**. It serves three purposes:

1. **Preemption point**: The runtime MAY suspend the agent here and schedule another. This is the only guaranteed preemption point.
2. **Budget enforcement**: If the agent's fuel counter reaches zero, `FUEL_CHECK` traps with `TRAP_OUT_OF_FUEL`. The host decides whether to grant more fuel.
3. **Checkpoint opportunity**: The runtime MAY use `FUEL_CHECK` as a hint to snapshot agent state for migration (yoke transfer).

Placement: Compilers/runtimes SHOULD insert `FUEL_CHECK` at the head of every loop body. Manual assembly MAY place them less frequently, but MUST place at least one per 1024 instructions to ensure liveness.

### 9.2 DEADLINE_BEFORE Semantics (0x4A in Control Flow)

```
DEADLINE_BEFORE rd, offset
```

- `rd` holds a timestamp (epoch milliseconds).
- `offset` is the jump target (signed 8-bit, relative).
- If the current time is before `rd`, execution continues to the next instruction.
- If the current time is at or past `rd`, execution jumps to `offset`.

This enables deadline-aware loops:

```
  loop:
    DEADLINE_BEFORE r_deadline, done    ; if past deadline, jump to done
    ; ... do work ...
    JMP loop
  done:
    ; deadline was missed — handle gracefully
```

This is the agent-world analog of real-time OS deadline scheduling. *(DeepSeek)*

### 9.3 YIELD_IF_CONTENTION Semantics

```
YIELD_IF_CONTENTION rs1
```

- `rs1` holds a resource handle (obtained from `RESOURCE_ACQUIRE`).
- The runtime checks the contention level on the resource:
  - **0**: No contention. Execution continues immediately.
  - **1–63**: Low contention. Execution continues; returns contention level in rs1.
  - **64+**: High contention. Execution YIELDS (similar to SUSPEND). Returns contention level in rs1 upon resumption.

This is a **voluntary** backoff mechanism — agents cooperate rather than spinning on contested resources. *(DeepSeek)*

### 9.4 PERSIST_CRITICAL_STATE Semantics

```
PERSIST_CRITICAL_STATE rs1, rs2
```

- `rs1` = pointer to state data in agent memory.
- `rs2` = size in bytes.
- The runtime asynchronously writes this data to stable storage.
- This is a **hint**, not a guarantee. The runtime MAY:
  - Buffer and batch writes.
  - Skip if storage is unavailable (set a flag readable via FEATURE_QUERY).
  - Deduplicate if the same region is persisted twice.
- Use case: Before a risky operation, an agent persists its decision state so that a crash doesn't lose progress. *(DeepSeek)*

---

## 10. Memory & Data (0x70–0x7F)

This range moves up from ISA v2's 0x70 position (which was viewpoint/linguistic, now removed). Pure memory operations belong in the ISA; data interpretation belongs in libraries.

| Opcode | Mnemonic | Operands | Description |
|--------|----------|----------|-------------|
| 0x70 | `ALLOC rd, size` | rd, imm8 | Allocate memory. rd = pointer, size in units of 16 bytes. |
| 0x71 | `FREE rs1` | rs1 | Free previously allocated memory. Traps on double-free. |
| 0x72 | `LOAD8 rd, [rs1]` | rd, rs1 | Load unsigned byte, zero-extend to 32 bits. |
| 0x73 | `STORE8 rs1, [rd]` | rs1, rd | Store low byte of rs1 to memory. |
| 0x74 | `LOAD16 rd, [rs1]` | rd, rs1 | Load unsigned halfword, zero-extend to 32 bits. |
| 0x75 | `STORE16 rs1, [rd]` | rs1, rd | Store low halfword of rs1 to memory. |
| 0x76 | `MEMCPY rd, rs1, rs2` | rd, rs1, rs2 | Copy rs2 bytes from rs1 to rd. Traps on overlap or OOB. |
| 0x77 | `MEMSET rd, rs1, rs2` | rd, rs1, rs2 | Fill rs2 bytes at rd with value rs1. |
| 0x78 | `MEMCMP rd, rs1, rs2` | rd, rs1, rs2 | Compare rs2 bytes at rd and rs1. Result: 0 (equal), -1, +1. |
| 0x79 | `MEMTAG_SET rd, rs1, tag` | rd, rs1, imm8 | Set memory tag. See §15. |
| 0x7A | `MEMTAG_CHECK rd, tag` | rd, imm8 | Check memory tag. Traps on mismatch. See §15. |
| 0x7B | `STACK_ALLOC rd, size` | rd, imm8 | Allocate from stack region (fast path). |
| 0x7C | `STACK_FREE rs1` | rs1 | Pop stack frame. |
| 0x7D | `LOAD_ADDR rd, offset` | rd, imm8 | Load absolute address into rd (PC-relative). |
| 0x7E | `PTR_ADD rd, rs1, rs2` | rd, rs1, rs2 | Pointer arithmetic (bounds-checked if in sandbox). |
| 0x7F | `PTR_DIFF rd, rs1, rs2` | rd, rs1, rs2 | Pointer difference (rs1 - rs2). |

---

## 11. Collections (0x80–0x8F)

Dynamic collections (vectors, maps) are retained from ISA v2 but with a reduced footprint. Overly specialized collection ops (e.g., sorted set operations) are moved to host functions. *(DeepSeek — "collections at ISA level is architectural malpractice for anything beyond basic arrays")*

| Opcode | Mnemonic | Operands | Description |
|--------|----------|----------|-------------|
| 0x80 | `VEC_NEW rd, capacity` | rd, imm8 | Create vector with initial capacity. |
| 0x81 | `VEC_PUSH rs1, rs2` | rs1, rs2 | Push rs2 to vector rs1. |
| 0x82 | `VEC_POP rd, rs1` | rd, rs1 | Pop from vector rs1 into rd. Traps if empty. |
| 0x83 | `VEC_LEN rd, rs1` | rd, rs1 | Store vector length in rd. |
| 0x84 | `VEC_GET rd, rs1, rs2` | rd, rs1, rs2 | Get element at index rs2 from vector rs1. |
| 0x85 | `VEC_SET rs1, rs2, rs3` | rs1, rs2, rs3 | Set element at index rs2 to rs3 in vector rs1. |
| 0x86 | `MAP_NEW rd, capacity` | rd, imm8 | Create hash map with initial capacity. |
| 0x87 | `MAP_SET rs1, rs2, rs3` | rs1, rs2, rs3 | Set key rs2 to value rs3 in map rs1. |
| 0x88 | `MAP_GET rd, rs1, rs2` | rd, rs1, rs2 | Get value for key rs2 from map rs1. Traps on missing. |
| 0x89 | `MAP_DEL rs1, rs2` | rs1, rs2 | Delete key rs2 from map rs1. |
| 0x8A | `MAP_HAS rd, rs1, rs2` | rd, rs1, rs2 | Check if key rs2 exists in map rs1. rd = 0 or 1. |
| 0x8B | `MAP_LEN rd, rs1` | rd, rs1 | Store map entry count in rd. |
| 0x8C–0x8F | *reserved* | — | Reserved for future collection ops. |

---

## 12. Unified Tensor Range (0xB0–0xDF) — MERGED

The SIMD range (0xB0–0xBF) and tensor/neural range (0xC0–0xCF) from ISA v2 are **merged** into a single unified tensor range. Rationale: modern AI workloads treat SIMD and tensor operations as a continuum — mixed-precision, sparse, variable-rank. Separate categories forced context switches that prevented optimization. *(Seed; DeepSeek; Oracle1 priority 6)*

The unified range spans 0xB0–0xDF (48 slots), up from 16+16=32 in ISA v2 — a net gain of 16 slots.

| Opcode | Mnemonic | Operands | Description |
|--------|----------|----------|-------------|
| 0xB0 | `T_ADD rd, rs1, rs2` | rd, rs1, rs2 | Element-wise tensor addition. |
| 0xB1 | `T_SUB rd, rs1, rs2` | rd, rs1, rs2 | Element-wise tensor subtraction. |
| 0xB2 | `T_MUL rd, rs1, rs2` | rd, rs1, rs2 | Element-wise tensor multiplication. |
| 0xB3 | `T_DIV rd, rs1, rs2` | rd, rs1, rs2 | Element-wise tensor division. |
| 0xB4 | `T_MATMUL rd, rs1, rs2` | rd, rs1, rs2 | Matrix multiplication (2D). |
| 0xB5 | `T_MATMUL_BATCH rd, rs1, rs2` | rd, rs1, rs2 | Batched matrix multiplication (3D). |
| 0xB6 | `T_CONV2D rd, rs1, rs2` | rd, rs1, rs2 | 2D convolution. rs2 = kernel descriptor. |
| 0xB7 | `T_TRANSPOSE rd, rs1` | rd, rs1 | Transpose last two dimensions. |
| 0xB8 | `T_RESHAPE rd, rs1, rs2` | rd, rs1, rs2 | Reshape tensor. rs2 = shape descriptor. |
| 0xB9 | `T_CAST rd, rs1, dtype` | rd, rs1, imm8 | Cast tensor to dtype. dtype: 0=f32, 1=f16, 2=bf16, 3=int8, 4=int4, 5=uint8. |
| 0xBA | `T_QUANTIZE rd, rs1, scale, zp` | rd, rs1, rs2, imm8 | Quantize: rd = (rs1 / scale) + zero_point. |
| 0xBB | `T_DEQUANTIZE rd, rs1, scale, zp` | rd, rs1, rs2, imm8 | Dequantize: rd = (rs1 - zero_point) * scale. |
| 0xBC | `T_GELU rd, rs1` | rd, rs1 | GELU activation (element-wise). |
| 0xBD | `T_RELU rd, rs1` | rd, rs1 | ReLU activation (element-wise). |
| 0xBE | `T_SOFTMAX rd, rs1` | rd, rs1 | Softmax along last dimension. |
| 0xBF | `T_LAYER_NORM rd, rs1` | rd, rs1 | Layer normalization. |
| 0xC0 | `T_ATTENTION rd, Q, K, V` | rd, rs1, rs2, rs3 | Standard attention: rd = softmax(QK^T/√d)V. |
| 0xC1 | `T_FLASH_ATTN rd, Q, K, V` | rd, rs1, rs2, rs3 | Flash attention (memory-efficient). CUDAClaw: fused kernel. ZeroClaw: chunked reference. |
| 0xC2 | `T_CAUSAL_MASK rd, rs1` | rd, rs1 | Apply causal mask (upper triangle = -inf). |
| 0xC3 | `T_SPARSE_ADD rd, rs1, rs2` | rd, rs1, rs2 | Sparse tensor addition. rs1, rs2 = CSR/CSC descriptors. |
| 0xC4 | `T_SPARSE_MATMUL rd, rs1, rs2` | rd, rs1, rs2 | Sparse-dense matrix multiply. |
| 0xC5 | `T_MOE_ROUTE rd, rs1, rs2` | rd, rs1, rs2 | Mixture-of-experts routing. rs1 = input, rs2 = gate weights. rd = expert indices. |
| 0xC6 | `T_GATED_LINEAR rd, rs1, rs2` | rd, rs1, rs2 | Gated linear unit: rd = rs1 ⊙ σ(rs2). |
| 0xC7 | `T_EMBED_LOOKUP rd, rs1, rs2` | rd, rs1, rs2 | Embedding table lookup. rs1 = table, rs2 = indices. |
| 0xC8 | `T_CROSS_ENTROPY rd, rs1, rs2` | rd, rs1, rs2 | Cross-entropy loss. |
| 0xC9 | `T_CONCAT rd, rs1, rs2, dim` | rd, rs1, rs2, imm8 | Concatenate tensors along dimension. |
| 0xCA | `T_SPLIT rd, rs1, dim, idx` | rd, rs1, imm8, imm8 | Split tensor at dimension/index. |
| 0xCB | `T_BROADCAST rd, rs1, rs2` | rd, rs1, rs2 | Broadcast rs1 to shape rs2. |
| 0xCC | `T_REDUCE_SUM rd, rs1, dim` | rd, rs1, imm8 | Reduce sum along dimension. |
| 0xCD | `T_REDUCE_MAX rd, rs1, dim` | rd, rs1, imm8 | Reduce max along dimension. |
| 0xCE | `T_GATHER rd, rs1, rs2` | rd, rs1, rs2 | Gather elements by indices. |
| 0xCF | `T_SCATTER rs1, rs2, rs3` | rs1, rs2, rs3 | Scatter elements to indices. |
| 0xD0 | `T_RANGE rd, start, end, step` | rd, rs1, rs2, rs3 | Generate range tensor. |
| 0xD1 | `T_ARANGE rd, start, end` | rd, rs1, rs2 | Generate arange (step=1). |
| 0xD2 | `T_FULL rd, shape, value` | rd, rs1, rs2 | Create tensor filled with value. |
| 0xD3 | `T_EYE rd, size` | rd, imm8 | Create identity matrix. |
| 0xD4 | `T_RANDN rd, shape` | rd, rs1 | Random normal tensor (seeded by runtime). |
| 0xD5 | `T_CLIP rd, rs1, min, max` | rd, rs1, rs2, rs3 | Clip values to [min, max]. |
| 0xD6 | `T_WHERE rd, cond, rs1, rs2` | rd, rs1, rs2, rs3 | Conditional selection: rd[i] = rs1[i] if cond[i] else rs2[i]. |
| 0xD7 | `T_TOPK rd, rs1, k` | rd, rs1, imm8 | Top-k elements. |
| 0xD8 | `T_SORT rd, rs1, dim` | rd, rs1, imm8 | Sort along dimension. |
| 0xD9 | `T_UNIQUE rd, rs1` | rd, rs1 | Unique elements. |
| 0xDA | `T_IS_FINITE rd, rs1` | rd, rs1 | Check for NaN/Inf. rd = boolean tensor. |
| 0xDB | `T_NORM rd, rs1, order` | rd, rs1, imm8 | Tensor norm. order: 0=inf, 1=L1, 2=L2. |
| 0xDC | `T_MATMUL_ACC rd, rs1, rs2` | rd, rs1, rs2 | rd += rs1 @ rs2 (fused multiply-accumulate). |
| 0xDD | `T_SCALAR_MUL rd, rs1, rs2` | rd, rs1, rs2 | Multiply tensor by scalar. |
| 0xDE | `T_SCALAR_ADD rd, rs1, rs2` | rd, rs1, rs2 | Add scalar to tensor. |
| 0xDF | *reserved* | — | Reserved. |

---

## 13. System Extension (0xF0–0xFE)

| Opcode | Mnemonic | Operands | Description |
|--------|----------|----------|-------------|
| 0xF0 | `DEBUG_BREAK` | — | Trap to debugger. |
| 0xF1 | `DEBUG_LOG rs1` | rs1 | Log register value. Format defined by host. |
| 0xF2 | `PROF_ENTER rs1` | rs1 | Enter profiling region. rs1 = region ID. |
| 0xF3 | `PROF_EXIT rs1` | rs1 | Exit profiling region. |
| 0xF4 | `VERSION rd` | rd | Load ISA version (0x0300 for v3.0). |
| 0xF5 | `IMPLEMENTATION rd` | rd | Load implementation ID (vendor-specific). |
| 0xF6 | `FEATURE_QUERY feature_id, rd` | imm8, rd | Query feature support. |
| 0xF7 | `FEATURE_ENABLE feature_id` | imm8 | Enable optional feature. Traps if unsupported. |
| 0xF8 | `FEATURE_DISABLE feature_id` | imm8 | Disable optional feature. |
| 0xF9 | `SERIALIZE_STATE rd` | rd | Serialize full VM state to buffer. rd = buffer handle. |
| 0xFA | `DESERIALIZE_STATE rs1` | rs1 | Restore VM state from buffer. |
| 0xFB | `COMMIT_CHECKPOINT label` | imm8 | Create named checkpoint for rollback. |
| 0xFC | `ROLLBACK_CHECKPOINT label` | imm8 | Roll back to named checkpoint. |
| 0xFD | `GC_TRIGGER` | — | Suggest garbage collection. |
| 0xFE | `ESCAPE` | 0xFF prefix | Escape to extended opcode space. *See §4.3.* |

---

## 14. Escape Space (0xFF 0x00–0xFF 0xFF)

The escape prefix is 0xFF (opcode 0xFE is the *entry point* to escape space). After the escape byte, the next byte selects the sub-opcode.

**Full specification in Appendix B.** Initial sub-opcode ranges:

| Sub-Range | Allocation | Purpose |
|-----------|-----------|---------|
| 0xFF 0x00 | `ESCAPE_NOP` | Explicit no-op in extended space |
| 0xFF 0x01–0x0F | Reserved: Future Math | bfloat16, float8, matrix transpose, complex arithmetic |
| 0xFF 0x10–0x1F | Reserved: Future Tensor | Winograd, attention variants, quantization schemes |
| 0xFF 0x20–0x2F | Reserved: Future Security | Hardware keys, attestation, TEE integration |
| 0xFF 0x30–0x3F | Reserved: Future Async | Coroutines, streams, pipelining |
| 0xFF 0x40–0x4F | Host Function Interface | Registration, invocation, callback |
| 0xFF 0x50–0x5F | Platform-Specific | ARM NEON hints, x86 AVX hints, CUDA kernel launch |
| 0xFF 0x60–0x7F | Vendor Extensions | Vendor-specific opcodes (first-come, registry-based) |
| 0xFF 0x80–0xFD | Unallocated | Available for future proposal |
| 0xFF 0xFE | `ESCAPE_VERSION` | Returns escape space version |
| 0xFF 0xFF | `ESCAPE_NEST` | Nested escape — level 2 |

---

## 15. Security Primitives — NEW

Security was *completely absent* from ISA v2. This is a critical omission for a multi-agent runtime where untrusted code, external tool calls, and indefinite execution are the norm. *(DeepSeek; Kimi; Oracle1 priority 7)*

### 15.1 Capability-Based Invocation: `CAP_INVOKE`

```
CAP_INVOKE capability_handle, rd
```

- `capability_handle` (in a register): An opaque handle representing a permission to perform a specific action.
- `rd`: Destination for return value.
- The runtime checks the capability before executing:
  - **Valid**: Execute the associated function. Return result in rd.
  - **Invalid/expired**: Trap with `TRAP_CAPABILITY_DENIED`.
  - **Revoked**: Trap with `TRAP_CAPABILITY_REVOKED`.

Capabilities are **not user-constructible**. They are granted by the host runtime and stored in a protected capability table. The handle is an index into this table.

**Capability types**:
| Type ID | Description |
|---------|-------------|
| 0x01 | File I/O (read/write specific paths) |
| 0x02 | Network (connect to specific endpoints) |
| 0x03 | Agent spawn (create child agents) |
| 0x04 | Memory (access specific memory regions) |
| 0x05 | Sensor access (read specific sensors) |
| 0x06 | Tool invocation (call specific external tools) |
| 0x07 | Yoke transfer (initiate state migration) |
| 0x08 | Host function (call arbitrary host function) |

### 15.2 Memory Tagging: `MEM_TAG`

Memory tagging provides ARM MTE-style spatial safety for agent memory.

```
MEMTAG_SET ptr, size, tag      ; 0x79 in Memory range
MEMTAG_CHECK ptr, tag          ; 0x7A in Memory range
```

- `MEMTAG_SET`: Associates a 4-bit tag (0–15) with the memory region [ptr, ptr+size). Subsequent writes to tagged memory update the tag shadow.
- `MEMTAG_CHECK`: Verifies that the tag at `ptr` matches `tag`. If not, traps with `TRAP_MEM_TAG_MISMATCH`.

**Semantics**:
- Tags are checked on memory dereference (LOAD/STORE) when the runtime's tag-checking mode is enabled.
- Tags are a **runtime enforcement** mechanism, not a hardware requirement. ZeroClaw implements in software; CUDAClaw may use hardware MTE if available.
- Uninitialized memory has tag 0. Convention: tag 0 means "untrusted/uninitialized."

### 15.3 Sandboxed Memory Regions

```
SANDBOX_ENTER region_id        ; 0x0D in System range
SANDBOX_LEAVE                  ; 0x0E in System range
```

- `SANDBOX_ENTER`: Activates a sandboxed memory region. While active:
  - All memory accesses are bounds-checked against the region's limits.
  - Writes outside the region trap with `TRAP_SANDBOX_VIOLATION`.
  - The region's memory tags are isolated from the parent's.
- `SANDBOX_LEAVE`: Deactivates the current sandbox. Access restrictions revert to the parent context.

**Region lifecycle**:
1. The host runtime creates regions via the host function interface.
2. Regions have defined bounds, capacity, and permission flags (read/write/execute).
3. Regions MAY be nested (sandbox within sandbox).
4. A `SANDBOX_LEAVE` without a matching `SANDBOX_ENTER` is a no-op.

**Use case**: An agent receiving untrusted code (e.g., a plugin) creates a sandbox region, loads the code there, and executes. Even if the code attempts malicious memory access, the sandbox prevents it.

---

## 16. Async Primitives — NEW

Agents are inherently event-driven. The ISA v2 model of synchronous execution with jump-based polling is inadequate for I/O-bound, multi-agent workloads. *(Kimi; DeepSeek; Oracle1 priority 4)*

### 16.1 Continuation Handles

A **continuation** captures the full execution state at a suspend point: program counter, registers, call stack, and capability context. Continuations are first-class values — they can be stored in registers, passed as arguments, and invoked later.

### 16.2 Async Opcode Specification

| Opcode | Mnemonic | Operands | Description |
|--------|----------|----------|-------------|
| 0xE0 | `SUSPEND reason` | imm8 | Suspend execution. reason: 0=voluntary, 1=waiting_io, 2=waiting_agent, 3=deadline_yield, 4=fuel_exhausted. Stores continuation in implicit register r254. |
| 0xE1 | `RESUME cont` | rs1 | Resume a previously suspended continuation. rs1 = continuation handle. Traps if continuation is already running or invalid. |
| 0xE2 | `CONTINUATION_NEW rd` | rd | Create an empty continuation frame. Returns handle in rd. Used for manual coroutine creation. |
| 0xE3 | `CONTINUATION_CALL cont, arg` | rs1, rs2 | Invoke continuation with argument. rs1 = continuation, rs2 = argument. The current continuation is automatically suspended. |
| 0xE4 | `AWAIT future` | rs1 | Suspend until future rs1 is resolved. Returns result in implicit register r253. |
| 0xE5 | `SELECT count, ...handles` | imm8, rs1..rsN | Multi-await. Suspends until any of the specified continuations/futures are ready. Returns index of ready handle. |
| 0xE6 | `FUTURE_NEW rd` | rd | Create an unresolved future. Returns handle in rd. |
| 0xE7 | `FUTURE_RESOLVE handle, value` | rs1, rs2 | Resolve a future with a value. Resumes any agent awaiting this future. |

### 16.3 SUSPEND Semantics

When `SUSPEND` executes:

1. The runtime captures the current execution state into a continuation object.
2. The continuation handle is stored in **r254** (reserved for async state).
3. Control returns to the scheduler.
4. The scheduler MAY:
   - Run another agent/task.
   - Keep the current agent idle until a wake condition occurs.
   - Migrate the agent to another hardware instance (yoke transfer).

The continuation is opaque to the agent — it cannot inspect or modify its own captured state. This prevents introspection-based security attacks.

### 16.4 Conventions

- **r253**: Async result register. `AWAIT` stores its result here.
- **r254**: Continuation register. `SUSPEND` stores the continuation handle here.
- **r255**: Reserved for runtime use (scheduler state, capability context).

---

## 17. Removed Opcodes & Migration Guide

### 17.1 Removed Categories

| Category | Old Range | v3 Replacement |
|----------|-----------|---------------|
| Confidence (16 ops) | 0x60–0x6F | Host function `confidence.*` |
| Viewpoint/Linguistic (16 ops) | 0x70–0x7F | Host function `viewpoint.*` |
| Sensors (16 ops) | 0x80–0x8F | Host function `sensor.*` |
| Sensor extensions (16 ops) | 0x90–0x9F | Host function `sensor_ext.*` |
| A2A Fleet (14 ops) | 0x50–0x5D | `SENDMSG`/`RECVMSG` + host functions |
| SIMD (8 ops) | 0xB0–0xB7 | Merged into unified tensor range |
| Tensor/Neural (8 ops) | 0xC0–0xC7 | Merged into unified tensor range |

### 17.2 Migration Patterns

**Pattern 1: Confidence operations → host function**

```asm
; ISA v2
CONFIDENCE_COMPUTE r1, r2       ; compute confidence from data

; ISA v3 (host function approach)
LOADI r1, host_func_confidence_compute  ; function ID
LOAD r2, [data_ptr]
CAP_INVOKE r1, r3              ; r3 = result, requires confidence capability
```

**Pattern 2: Sensor reads → host function**

```asm
; ISA v2
SENSOR_READ r1, 0x03           ; read sensor 3

; ISA v3
LOADI r1, host_func_sensor_read
LOADI r2, 3                    ; sensor ID
CAP_INVOKE r1, r3              ; r3 = sensor data
```

**Pattern 3: A2A Fleet → SENDMSG/RECVMSG**

```asm
; ISA v2
FLEET_SEND_TASK r1, r2, r3     ; 3-register task send

; ISA v3
MOV r1, target_capability      ; capability handle
MOV r2, payload_ptr
SENDMSG r1, r2                 ; 2-register message send
```

**Pattern 4: SIMD → unified tensor**

```asm
; ISA v2
SIMD_ADD_F32 r1, r2, r3        ; SIMD float add

; ISA v3 (same semantic, different opcode)
T_ADD r1, r2, r3               ; works for scalars, vectors, tensors
```

### 17.3 Binary Translation

Runtimes implementing v3 SHOULD provide a binary translator from v2 to v3 bytecode. The translation is mechanical:

1. Re-map opcodes according to the address space changes above.
2. Replace removed opcodes with `CAP_INVOKE` sequences pointing to compatibility host functions.
3. Optionally: re-encode hot paths in compressed format (profile-guided).

The translation preserves semantic equivalence. v2 binaries run unmodified on v3 runtimes that include the compatibility layer.

---

## 18. Host Function Interface (replaces removed domains)

The Host Function Interface (HFI) is the mechanism by which domain-specific operations — confidence, viewpoint, sensors, and other application-level concerns — are accessed without polluting the ISA opcode space.

### 18.1 Architecture

```
┌──────────────────────────────────────────────┐
│              FLUX Bytecode (ISA v3)          │
│  ┌─────────────────────────────────────────┐ │
│  │  Core ISA (ops 0x00–0xFE)              │ │
│  │  + Escape space (0xFF prefix)          │ │
│  └──────────┬──────────────────────────────┘ │
│             │ CAP_INVOKE                      │
│             ▼                                 │
│  ┌─────────────────────────────────────────┐ │
│  │  Capability Table (host-managed)        │ │
│  └──────────┬──────────────────────────────┘ │
│             │                                │
│             ▼                                │
│  ┌─────────────────────────────────────────┐ │
│  │  Host Functions (runtime-implemented)   │ │
│  │  - confidence.compute()                 │ │
│  │  - confidence.aggregate_median()        │ │
│  │  - viewpoint.extract()                  │ │
│  │  - viewpoint.align()                    │ │
│  │  - sensor.read(id)                      │ │
│  │  - sensor.fuse_weighted(s, c, prior)    │ │
│  │  - action.select_policy(policy, ctx)    │ │
│  │  - embedding.knn(query, k)              │ │
│  │  - json.parse(ptr) / json.next(ptr)     │ │
│  │  - sample.distribution(dist_type, args) │ │
│  └─────────────────────────────────────────┘ │
└──────────────────────────────────────────────┘
```

### 18.2 Registration

Host functions are registered at runtime initialization. The host provides a function table indexed by function ID. The agent accesses them through `CAP_INVOKE` with a capability handle that maps to a function ID.

### 18.3 Gold Vein Libraries (retained)

The following domain-specific operations were identified as **"gold veins"** in the critique — genuinely useful but correctly placed as library calls rather than ISA primitives:

**Sensor-Confidence Fusion (Seed's proposal)**:
```c
// Host function: sensor.fuse_weighted
// Signature: float fuse_weighted(float sensor_data, float confidence, float prior_belief)
// Returns: (sensor_data * confidence) + prior_belief * (1 - confidence)
```

**ACTION_SELECT_POLICY (Seed's proposal)**:
```c
// Host function: action.select_policy
// Signature: int select_policy(int policy_output, action_table* actions)
// Maps a policy output (LLM token, robot joint command) to a runtime action
```

These are provided as standard library functions in the runtime's host function table, not as opcodes. They can be updated, optimized, and extended without ISA revisions.

---

## 19. Design Rationale & Attribution

### 19.1 0xFF Escape Prefix

**Proposed by**: Kimi  
**Why it matters**: With 9 free slots in a 256-slot space, ISA v2 faced terminal rigidity — any new opcode category required breaking the spec. The escape prefix provides 256 immediate sub-opcodes and unbounded nesting. This is the single highest-impact change in ISA v3.  
**Trade-off**: Extended opcodes use more bytes (3+ vs 4). This is acceptable because escape opcodes are by definition rare/extension ops, not hot-path instructions.

### 19.2 Compressed 2-Byte Format

**Proposed by**: Kimi (RISC-V C-extension model), Seed (format redistribution analysis)  
**Why it matters**: 157/247 ops used a uniform 4-byte format. Agent hot loops (perception-action cycles) are dominated by simple ops (ADD, CMP, JMP) that waste 50% of their encoding on unused fields. The compressed format cuts these to 2 bytes — a 25–30% code density improvement for inner loops.  
**Trade-off**: Register window restriction (dest r0–r31, sources r0–r7). Acceptable because hot loops use few registers; cold code uses the full 4-byte format.

### 19.3 Domain Category Removal

**Proposed by**: DeepSeek ("architectural carcinization"), Kimi ("I/O belongs in host function interface"), Seed (implicit — by proposing format cleanup that exposed category bloat)  
**Why it matters**: Confidence, viewpoint, linguistic, and sensor categories encode *application-level semantics* into ISA opcodes. When the underlying AI techniques change (and they will — faster than ISA revisions), the opcodes become dead weight or require breaking changes. Moving them to host functions preserves semantic richness without freezing it into hardware.  
**Trade-off**: Host function calls have higher latency than direct opcodes. Mitigated by: (a) these operations are not on the critical path of inner loops, and (b) CUDAClaw can still provide hardware-accelerated implementations behind the host function interface.

### 19.4 Unified Tensor Range

**Proposed by**: Seed ("Merge SIMD/Tensor"), DeepSeek ("siloing SIMD and tensor ops prevents optimization")  
**Why it matters**: Modern AI workloads (attention, MoE, quantized inference) blend what were traditionally separate SIMD and tensor operations. The merge enables the runtime to optimize across the boundary — e.g., fusing a SIMD activation with a tensor matmul into a single GPU kernel launch.  
**Trade-off**: The unified range uses broader encoding (ops must specify element type). This is handled by the `T_CAST` opcode and tensor descriptor metadata, not by separate opcode classes.

### 19.5 Resource & Temporal Primitives

**Proposed by**: DeepSeek ("Agents run in time"), Oracle1 synthesis (priority 5)  
**Why it matters**: ISA v2 assumed infinite hardware and instantaneous execution. Real agents operate under fuel budgets, deadlines, resource contention, and energy constraints. `FUEL_CHECK`, `DEADLINE_BEFORE`, and `YIELD_IF_CONTENTION` make these constraints first-class architectural features rather than external scheduler hints.

### 19.6 Security Primitives

**Proposed by**: DeepSeek ("security timebomb"), Kimi ("zero security primitives in an agent runtime")  
**Why it matters**: Multi-agent systems execute untrusted code from external sources. Without capability gates, memory tagging, and sandboxing, a compromised agent can corrupt the entire fleet. Security primitives are not optional — they are prerequisites for any deployment beyond a single trusted agent.

### 19.7 Async Primitives

**Proposed by**: Kimi ("SUSPEND/RESUME with continuation handles"), DeepSeek ("agents are event-driven")  
**Why it matters**: Synchronous jump-based polling is architecturally wrong for I/O-bound agent workloads. Continuation-based async allows agents to efficiently wait for network responses, sensor data, child agents, and futures without wasting cycles.

---

## Appendix A: Compressed Opcode Quick Reference

```
0x00 NOP            0x10 LOAD rd,[rs1+off]   0x20 (reserved)
0x01 MOV rd,rs1     0x11 STORE rs1,[rd+off]  0x21 (reserved)
0x02 ADD rd,rs1,rs2 0x12 ALLOC rd,size       0x22 (reserved)
0x03 SUB rd,rs1,rs2 0x13 FUEL_CHECK          0x23 (reserved)
0x04 MUL rd,rs1,rs2 0x14 T_ADD rd,rs1,rs2    ...
0x05 AND rd,rs1,rs2 0x15 T_MUL rd,rs1,rs2    0x7F (reserved)
0x06 OR  rd,rs1,rs2 0x16 T_MATMUL rd,rs1,rs2
0x07 XOR rd,rs1,rs2 0x17 RET
0x08 CMP rd,rs1,rs2 0x18 MIN rd,rs1,rs2
0x09 NEG rd,rs1     0x19 MAX rd,rs1,rs2
0x0A SHL rd,rs1,rs2 0x1A ABS rd,rs1
0x0B SHR rd,rs1,rs2 0x1B LJMP far_offset
0x0C JMP offset     0x1C CALL_FAR far_addr
0x0D JMP_IF_EQ      0x1D SENDMSG rs1,rs2
0x0E JMP_IF_LT      0x1E RECVMSG rd
0x0F CALL addr      0x1F NOT rd,rs1
```

---

## Appendix B: Escape Sub-Opcode Allocation

```
0xFF 0x00  ESCAPE_NOP               No-op in escape space
0xFF 0x01  MATH_BF16_ADD            bfloat16 addition
0xFF 0x02  MATH_BF16_MUL            bfloat16 multiplication
0xFF 0x03  MATH_BF16_MATMUL         bfloat16 matrix multiply
0xFF 0x04  MATH_F8_ADD              float8 (E4M3) addition
0xFF 0x05  MATH_F8_MUL              float8 (E4M3) multiplication
0xFF 0x06  MATH_F8_MATMUL           float8 matrix multiply
0xFF 0x07  MATH_COMPLEX_MUL         Complex number multiply
0xFF 0x08  MATH_COMPLEX_FFT         FFT (size specified by operand)
0xFF 0x09  MATH_QUAT_MUL            Quaternion multiply
0xFF 0x0A  MATH_QUAT_NORM           Quaternion normalize
0xFF 0x0B-0x0F reserved

0xFF 0x10  TENSOR_WINOGRAD          Winograd convolution
0xFF 0x11  TENSOR_FLASH_ATTN_V2     Flash Attention v2
0xFF 0x12  TENSOR_FLASH_ATTN_V3     Flash Attention v3 (ring attention)
0xFF 0x13  TENSOR_GROUPED_QKV       Fused grouped QKV projection
0xFF 0x14  TENSOR_ROTARY_EMBED      Rotary positional embedding
0xFF 0x15  TENSOR_ALIBI             Attention with linear bias
0xFF 0x16  TENSOR_SWIGLU           SwiGLU activation
0xFF 0x17  TENSOR_RMS_NORM         Root mean square normalization
0xFF 0x18  TENSOR_MOE_DISPATCH     MoE expert dispatch
0xFF 0x19  TENSOR_MOE_COMBINE      MoE expert combine
0xFF 0x1A-0x1F reserved

0xFF 0x20  SEC_ATTEST              Remote attestation request
0xFF 0x21  SEC_SEAL                Seal data (encrypt + authenticate)
0xFF 0x22  SEC_UNSEAL              Unseal data
0xFF 0x23  SEC_RANDOM              Cryptographically secure random
0xFF 0x24  SEC_HASH                Hash (SHA-256)
0xFF 0x25  SEC_HMAC                HMAC
0xFF 0x26  SEC_SIGN                Sign (Ed25519)
0xFF 0x27  SEC_VERIFY              Verify signature
0xFF 0x28-0x2F reserved

0xFF 0x30  ASYNC_SPAWN_FIBER       Spawn lightweight fiber
0xFF 0x31  ASYNC_YIELD_FIBER       Yield to scheduler
0xFF 0x32  ASYNC_CHANNEL_CREATE    Create async channel (MPSC)
0xFF 0x33  ASYNC_CHANNEL_SEND      Async channel send
0xFF 0x34  ASYNC_CHANNEL_RECV      Async channel receive
0xFF 0x35  ASYNC_TIMER_SET         Set one-shot timer
0xFF 0x36  ASYNC_TIMER_CANCEL      Cancel timer
0xFF 0x37  ASYNC_EPOLL_WAIT        Multi-event wait (I/O multiplexing)
0xFF 0x38-0x3F reserved

0xFF 0x40  HFI_REGISTER            Register host function
0xFF 0x41  HFI_INVOKE              Invoke host function by ID
0xFF 0x42  HFI_QUERY               Query host function metadata
0xFF 0x43  HFI_CALLBACK_SET        Set callback for host function
0xFF 0x44  HFI_CALLBACK_CLEAR      Clear callback
0xFF 0x45-0x4F reserved

0xFF 0x50  PLAT_CUDA_KERNEL_LAUNCH Launch CUDA kernel
0xFF 0x51  PLAT_CUDA_SYNC          Synchronize CUDA stream
0xFF 0x52  PLAT_CUDA_MEMCPY_H2D    Host to device copy
0xFF 0x53  PLAT_CUDA_MEMCPY_D2H    Device to host copy
0xFF 0x54  PLAT_NEON_HINT          ARM NEON optimization hint
0xFF 0x55  PLAT_AVX_HINT           x86 AVX optimization hint
0xFF 0x56  PLAT_CACHE_PREFETCH     Cache prefetch hint
0xFF 0x57  PLAT_CACHE_FLUSH        Cache flush hint
0xFF 0x58  PLAT_TLB_INVALIDATE     TLB invalidation
0xFF 0x59-0x5F reserved

0xFF 0x60-0x7F  VENDOR_EXTENSIONS   Vendor-specific (registry required)
0xFF 0x80-0xFD  UNALLOCATED        Available for proposal

0xFF 0xFE  ESCAPE_VERSION          Returns escape space version
0xFF 0xFF  ESCAPE_NEST             Opens level 2 escape
```

---

## Appendix C: Formal Grammar

```
instruction ::= compressed_instr | standard_instr | escape_instr

compressed_instr ::= byte:{ 0x00 .. 0x7F } byte

standard_instr ::= byte:{ 0x80 .. 0xFE } byte byte byte
    -- byte[0] = opcode (0x80..0xFE, excluding 0xFF which is escape)
    -- byte[1] = rd (destination register)
    -- byte[2] = rs1 (source register 1)
    -- byte[3] = rs2 (source register 2 or immediate)

escape_instr ::= 0xFF sub_opcode operand*
sub_opcode ::= byte:{ 0x00 .. 0xFE }   -- level 1 sub-opcode
             | 0xFF byte                 -- level 2 nesting

register ::= 0 .. 255
compressed_dest ::= 0 .. 31    -- 5-bit field
compressed_src  ::= 0 .. 7     -- 3-bit field

operand ::= register | immediate | memory_ref | tensor_desc

immediate ::= imm8   -- signed 8-bit (-128 to 127)
            | imm16  -- signed 16-bit (-32768 to 32767)
            | uimm8  -- unsigned 8-bit (0 to 255)

memory_ref ::= [register + offset]

tensor_desc ::= handle:{ capability_table_index }

capability_handle ::= u32   -- index into host capability table
continuation_handle ::= u32 -- index into runtime continuation table
```

---

## Revision History

| Version | Date | Changes |
|---------|------|---------|
| v3.0-draft | 2026-04-11 | Initial draft. All 7 action items from Oracle1 synthesis implemented. |
| v2.x | 2026-04-10 | Predecessor. 247 opcodes, 7 formats, 9 free slots. |

---

*This specification is a living document. Submit proposals for escape sub-opcode allocation via the FLUX Fleet review process. All proposals must include: (1) use case with profiling data, (2) encoding specification, (3) CUDAClaw and ZeroClaw implementation notes, and (4) security analysis.*
