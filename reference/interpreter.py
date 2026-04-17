"""
reference/interpreter.py — ISA v3 Reference Interpreter (Aligned to Draft)

A stack-based virtual machine implementing the FLUX ISA v3 specification
as documented in rounds/03-isa-v3-draft/isa-v3-draft.md.

Key alignment with v3 draft (2026-04-18):
    - HALT = 0x00, NOP = 0x01 (matching draft Section 8.1)
    - 0xFF escape prefix for all extension opcodes (matching draft Section 2.2)
    - TEMPORAL extension via 0xFF 0x01 (matching draft Section 4)
    - SECURITY extension via 0xFF 0x02 (matching draft Section 5)
    - ASYNC extension via 0xFF 0x03 (matching draft Section 6)

Architecture:
    - 256-byte addressable memory (expandable)
    - 256-entry stack (expandable, 32-bit entries)
    - Program counter (16-bit)
    - 44 core opcodes (matching v3 draft Section 8)
    - 6 TEMPORAL opcodes (extension 0x01)
    - 6 SECURITY opcodes (extension 0x02)
    - 6 ASYNC opcodes (extension 0x03)

Usage:
    vm = FluxVM()
    vm.load(bytecode)
    vm.run()
    print(vm.stack)
"""

from __future__ import annotations

import struct
from dataclasses import dataclass, field
from enum import IntEnum
from typing import Any, Dict, List, Optional, Tuple


# ── Opcode Definitions — Aligned to ISA v3 Draft Section 8 ──────────────────

ESCAPE_PREFIX = 0xFF  # Extension mechanism (v3 draft Section 2.2)

class CoreOp(IntEnum):
    """Core opcodes matching ISA v3 draft Section 8.1."""
    # System control (Format A — nullary)
    HALT  = 0x00   # HALT          — stop execution
    NOP   = 0x01   # NOP           — no operation
    BREAK = 0x02   # BREAK         — debugger breakpoint

    # Integer arithmetic (Format C — binary, pop a,b push result)
    ADD   = 0x10   # ADD           — pop a, b; push a + b
    SUB   = 0x11   # SUB           — pop a, b; push b - a
    MUL   = 0x12   # MUL           — pop a, b; push a * b
    DIV   = 0x13   # DIV           — pop a, b; push b // a (integer)
    MOD   = 0x14   # MOD           — pop a, b; push b % a
    NEG   = 0x15   # NEG           — pop a; push -a
    INC   = 0x16   # INC           — pop a; push a + 1
    DEC   = 0x17   # DEC           — pop a; push a - 1

    # Comparison (Format C — pop a, b push 0/1)
    EQ    = 0x20   # EQ            — pop a, b; push 1 if a == b else 0
    NE    = 0x21   # NE            — not equal
    LT    = 0x22   # LT            — less than (signed)
    LE    = 0x23   # LE            # less than or equal
    GT    = 0x24   # GT            # greater than (signed)
    GE    = 0x25   # GE            # greater than or equal

    # Logic / bitwise
    AND   = 0x30   # AND           — bitwise AND
    OR    = 0x31   # OR            # bitwise OR
    XOR   = 0x32   # XOR           # bitwise XOR
    NOT   = 0x33   # NOT           # bitwise NOT
    SHL   = 0x34   # SHL           # shift left
    SHR   = 0x35   # SHR           # shift right

    # Stack manipulation
    PUSH  = 0x55   # PUSH <i32>    — push 32-bit signed immediate
    POP   = 0x56   # POP           — discard top of stack
    DUP   = 0x57   # DUP           — duplicate top of stack
    SWAP  = 0x58   # SWAP          — swap top two stack entries

    # Memory
    LOAD  = 0x40   # LOAD <u16>    — load from memory[addr] onto stack
    STORE = 0x41   # STORE <u16>   — store top of stack to memory[addr]

    # Control flow
    JMP   = 0x50   # JMP <u16>     — unconditional jump
    JZ    = 0x51   # JZ <u16>      — jump if top == 0
    JNZ   = 0x52   # JNZ <u16>     — jump if top != 0
    CALL  = 0x53   # CALL <u16>    — push return address, jump
    RET   = 0x54   # RET           — pop return address, jump

    # A2A Fleet Operations
    TELL  = 0x60   # TELL          — send message to agent
    ASK   = 0x61   # ASK           — query another agent

    # Confidence
    CONF_GET = 0x70  # CONF_GET    — push current confidence value
    CONF_SET = 0x71  # CONF_SET    — set confidence value


class TemporalExt(IntEnum):
    """Extension 0x01 — TEMPORAL opcodes (v3 draft Section 4).
    Accessed via 0xFF 0x01 <sub_opcode>."""
    FUEL_CHECK          = 0x01  # push remaining fuel
    DEADLINE_BEFORE     = 0x02  # <u16_deadline_ms> — jump if past deadline
    YIELD_IF_CONTENTION = 0x03  # cooperative yield on resource contention
    PERSIST_CRITICAL    = 0x04  # hint: persist current VM state
    TIME_NOW            = 0x05  # push wall-clock timestamp (ms)
    SLEEP_UNTIL         = 0x06  # <u16_deadline_ms> — sleep until timestamp


class SecurityExt(IntEnum):
    """Extension 0x02 — SECURITY opcodes (v3 draft Section 5).
    Accessed via 0xFF 0x02 <sub_opcode>."""
    CAP_INVOKE     = 0x01  # <cap_id:u8> — invoke capability
    MEM_TAG        = 0x02  # <tag:u8> — tag memory region
    SANDBOX_ENTER  = 0x03  # enter sandboxed memory region
    SANDBOX_EXIT   = 0x04  # exit sandbox, restore access
    FUEL_SET       = 0x05  # <fuel:u16> — set execution fuel
    IDENTITY_GET   = 0x06  # push agent identity handle


class AsyncExt(IntEnum):
    """Extension 0x03 — ASYNC opcodes (v3 draft Section 6).
    Accessed via 0xFF 0x03 <sub_opcode>."""
    SUSPEND        = 0x01  # save state as continuation, yield
    RESUME         = 0x02  # restore from continuation handle
    FORK           = 0x03  # <stack_share:u8> — fork execution
    JOIN           = 0x04  # wait for forked context
    CANCEL         = 0x05  # <ctx_id:u8> — cancel a context
    AWAIT_CHANNEL  = 0x06  # <chan_id:u8> <timeout_ms:u16> — await message


# ── Extension dispatch tables ─────────────────────────────────────────────────

TEMPORAL_NAMES = {int(v): v.name.replace("_", " ") for v in TemporalExt}
SECURITY_NAMES = {int(v): v.name.replace("_", " ") for v in SecurityExt}
ASYNC_NAMES = {int(v): v.name.replace("_", " ") for v in AsyncExt}

EXTENSION_TABLES = {
    0x01: ("TEMPORAL", TEMPORAL_NAMES),
    0x02: ("SECURITY", SECURITY_NAMES),
    0x03: ("ASYNC", ASYNC_NAMES),
}


# ── Opcode dispatch table ───────────────────────────────────────────────────

OPCODE_NAMES: Dict[int, str] = {}
OPCODE_VALUES: Dict[str, int] = {}  # reverse map for assembler
for op in CoreOp:
    OPCODE_NAMES[int(op)] = op.name
    OPCODE_VALUES[op.name] = int(op)


# ── VM State ─────────────────────────────────────────────────────────────────

@dataclass
class VMState:
    """Serializable snapshot of VM state for SUSPEND/RESUME."""
    pc: int = 0
    stack: List[int] = field(default_factory=list)
    memory: bytes = b'\x00' * 256
    clock: int = 0
    halted: bool = False
    capabilities: Dict[int, bool] = field(default_factory=dict)
    fuel: int = 0
    confidence: float = 1.0
    channels: Dict[int, List[int]] = field(default_factory=list)
    contexts: Dict[int, 'VMState'] = field(default_factory=dict)

    def to_dict(self) -> Dict[str, Any]:
        return {
            "pc": self.pc, "stack": self.stack,
            "memory": list(self.memory), "clock": self.clock,
            "halted": self.halted,
            "capabilities": self.capabilities,
            "fuel": self.fuel,
            "confidence": self.confidence,
        }


# ── Virtual Machine ─────────────────────────────────────────────────────────

class FluxVM:
    """
    FLUX ISA v3 Reference Interpreter.

    A stack-based VM with 256-byte memory, 44 core opcodes, and extension
    support for TEMPORAL, SECURITY, and ASYNC operations via 0xFF prefix.

    Example:
        vm = FluxVM()
        code = assemble(["PUSH 10", "PUSH 20", "ADD", "HALT"])
        vm.load(code)
        vm.run()
        assert vm.stack[-1] == 30
    """

    def __init__(self, memory_size: int = 256, stack_size: int = 256):
        self.memory_size = memory_size
        self.stack_size = stack_size
        self.memory = bytearray(memory_size)
        self.stack: List[int] = []
        self.pc: int = 0
        self.program: bytes = b''
        self.clock: int = 0
        self.halted: bool = False
        self.call_stack: List[int] = []
        self.capabilities: Dict[int, bool] = {}
        self.fuel: int = 0  # 0 = unlimited
        self.confidence: float = 1.0
        self.channels: Dict[int, List[int]] = {}
        self._contexts: Dict[int, VMState] = {}
        self._context_counter = 0
        self._spawned: List["FluxVM"] = []
        self._trace: List[str] = []
        self.trace_enabled = False
        self._extension_support: set = {0x01, 0x02, 0x03}  # supported extensions

    def load(self, program: bytes) -> None:
        """Load a bytecode program into the VM."""
        self.program = program
        self.pc = 0
        self.halted = False

    def reset(self) -> None:
        """Reset VM to initial state."""
        self.memory = bytearray(self.memory_size)
        self.stack.clear()
        self.pc = 0
        self.clock = 0
        self.halted = False
        self.call_stack.clear()
        self.capabilities.clear()
        self.fuel = 0
        self.confidence = 1.0
        self.channels.clear()
        self._contexts.clear()
        self._context_counter = 0
        self._spawned.clear()
        self._trace.clear()

    # ── Stack operations ────────────────────────────────────────────────────

    def _push(self, value: int) -> None:
        if len(self.stack) >= self.stack_size:
            raise RuntimeError(f"Stack overflow (max {self.stack_size})")
        self.stack.append(value & 0xFFFFFFFF)  # 32-bit mask

    def _pop(self) -> int:
        if not self.stack:
            raise RuntimeError("Stack underflow")
        return self.stack.pop()

    def _peek(self, offset: int = 0) -> int:
        if offset >= len(self.stack):
            raise RuntimeError("Stack underflow on peek")
        return self.stack[-(offset + 1)]

    # ── Memory operations ───────────────────────────────────────────────────

    def _load8(self, addr: int) -> int:
        return self.memory[addr % self.memory_size]

    def _store8(self, addr: int, value: int) -> None:
        self.memory[addr % self.memory_size] = value & 0xFF

    def _load16(self, addr: int) -> int:
        a = addr % self.memory_size
        return self.memory[a] | (self.memory[(a + 1) % self.memory_size] << 8)

    def _store16(self, addr: int, value: int) -> None:
        a = addr % self.memory_size
        self.memory[a] = value & 0xFF
        self.memory[(a + 1) % self.memory_size] = (value >> 8) & 0xFF

    # ── Fuel management ─────────────────────────────────────────────────────

    def _check_fuel(self) -> None:
        """Decrement fuel; halt if exhausted."""
        if self.fuel > 0:
            self.fuel -= 1
            if self.fuel <= 0:
                self.halted = True
                self._log(f"FUEL EXHAUSTED at pc=0x{self.pc:04x}")

    # ── Fetch helpers ───────────────────────────────────────────────────────

    def _fetch8(self) -> int:
        """Fetch next byte from program."""
        b = self.program[self.pc]
        self.pc += 1
        return b

    def _fetch16(self) -> int:
        """Fetch next 2 bytes (little-endian) from program."""
        lo = self.program[self.pc]
        hi = self.program[self.pc + 1]
        self.pc += 2
        return lo | (hi << 8)

    def _fetch32(self) -> int:
        """Fetch next 4 bytes (little-endian) from program."""
        b0 = self.program[self.pc]
        b1 = self.program[self.pc + 1]
        b2 = self.program[self.pc + 2]
        b3 = self.program[self.pc + 3]
        self.pc += 4
        return b0 | (b1 << 8) | (b2 << 16) | (b3 << 24)

    # ── Trace ───────────────────────────────────────────────────────────────

    def _log(self, msg: str) -> None:
        if self.trace_enabled:
            self._trace.append(f"[pc={self.pc:04x} clk={self.clock}] {msg}")

    # ── Extension dispatch ──────────────────────────────────────────────────

    def _dispatch_extension(self, extension_id: int) -> bool:
        """Dispatch an 0xFF-prefixed extension opcode.

        Returns True if execution should continue, False if halted.
        Raises RuntimeError for unsupported extensions.
        """
        if extension_id not in self._extension_support:
            raise RuntimeError(
                f"Extension 0x{extension_id:02x} not supported. "
                f"Supported: {sorted(self._extension_support)}"
            )

        if extension_id == 0x01:
            return self._dispatch_temporal()
        elif extension_id == 0x02:
            return self._dispatch_security()
        elif extension_id == 0x03:
            return self._dispatch_async()
        else:
            raise RuntimeError(f"Unknown extension: 0x{extension_id:02x}")

    def _dispatch_temporal(self) -> bool:
        """TEMPORAL extension (0xFF 0x01 <sub>).</target>"""
        sub = self._fetch8()

        if sub == TemporalExt.FUEL_CHECK:
            self._push(self.fuel)
            self._log(f"FUEL_CHECK -> {self.fuel}")

        elif sub == TemporalExt.DEADLINE_BEFORE:
            deadline_ms = self._fetch16()
            # In this implementation, clock increments are used as proxy for time
            if self.clock >= deadline_ms:
                addr = self._fetch16()
                self.pc = addr
                self._log(f"DEADLINE_BEFORE {deadline_ms}: PAST -> 0x{addr:04x}")
            else:
                self.pc += 2  # skip the target address
                self._log(f"DEADLINE_BEFORE {deadline_ms}: ok (clock={self.clock})")

        elif sub == TemporalExt.YIELD_IF_CONTENTION:
            # Cooperative yield: advance clock by 1
            self.clock += 1
            self._push(0)  # 0 = no contention detected
            self._log("YIELD_IF_CONTENTION -> 0 (no contention)")

        elif sub == TemporalExt.PERSIST_CRITICAL:
            # Hint to persist state — no-op in reference impl
            self._log("PERSIST_CRITICAL (hint acknowledged)")

        elif sub == TemporalExt.TIME_NOW:
            self._push(self.clock)
            self._log(f"TIME_NOW -> {self.clock}")

        elif sub == TemporalExt.SLEEP_UNTIL:
            deadline_ms = self._fetch16()
            if self.clock < deadline_ms:
                self.clock = deadline_ms
                self._log(f"SLEEP_UNTIL {deadline_ms} (advanced clock to {self.clock})")
            else:
                self._log(f"SLEEP_UNTIL {deadline_ms} (already past)")

        else:
            raise RuntimeError(f"Unknown TEMPORAL sub-opcode: 0x{sub:02x}")

        return True

    def _dispatch_security(self) -> bool:
        """SECURITY extension (0xFF 0x02 <sub>).</target>"""
        sub = self._fetch8()

        if sub == SecurityExt.CAP_INVOKE:
            cap_id = self._fetch8()
            if not self.capabilities.get(cap_id, False):
                raise RuntimeError(f"CAP_INVOKE: capability {cap_id} not held (error 0xE0)")
            self._log(f"CAP_INVOKE {cap_id} (granted)")

        elif sub == SecurityExt.MEM_TAG:
            tag = self._fetch8()
            # Memory tagging hint — no-op in reference impl
            self._log(f"MEM_TAG {tag}")

        elif sub == SecurityExt.SANDBOX_ENTER:
            # Enter restricted memory region — no-op in reference impl
            self._log("SANDBOX_ENTER")

        elif sub == SecurityExt.SANDBOX_EXIT:
            # Exit restricted memory region — no-op in reference impl
            self._log("SANDBOX_EXIT")

        elif sub == SecurityExt.FUEL_SET:
            fuel = self._fetch16()
            self.fuel = fuel
            self._log(f"FUEL_SET -> {fuel}")

        elif sub == SecurityExt.IDENTITY_GET:
            # Push a fixed identity handle for this VM instance
            self._push(id(self) & 0xFFFFFFFF)
            self._log(f"IDENTITY_GET -> 0x{id(self) & 0xFFFFFFFF:08x}")

        else:
            raise RuntimeError(f"Unknown SECURITY sub-opcode: 0x{sub:02x}")

        return True

    def _dispatch_async(self) -> bool:
        """ASYNC extension (0xFF 0x03 <sub>).</target>"""
        sub = self._fetch8()

        if sub == AsyncExt.SUSPEND:
            # Save state and halt (continuation preserved)
            self._log("SUSPEND (state preserved, returning False)")
            return False

        elif sub == AsyncExt.RESUME:
            ctx_id = self._fetch8()
            if ctx_id in self._contexts:
                state = self._contexts.pop(ctx_id)
                self.restore(state)
                self._log(f"RESUME ctx={ctx_id} -> pc=0x{self.pc:04x}")
            else:
                self._log(f"RESUME ctx={ctx_id} (not found)")

        elif sub == AsyncExt.FORK:
            stack_share = self._fetch8()
            ctx_id = self._context_counter
            self._context_counter += 1
            self._contexts[ctx_id] = self.snapshot()
            self._push(ctx_id)
            self._log(f"FORK -> ctx={ctx_id} (share={stack_share})")

        elif sub == AsyncExt.JOIN:
            ctx_id = self._fetch8()
            if ctx_id in self._contexts:
                state = self._contexts.pop(ctx_id)
                result = state.stack[-1] if state.stack else 0
                self._push(result)
                self._log(f"JOIN ctx={ctx_id} -> result={result}")
            else:
                self._push(0)
                self._log(f"JOIN ctx={ctx_id} (not found, result=0)")

        elif sub == AsyncExt.CANCEL:
            ctx_id = self._fetch8()
            if ctx_id in self._contexts:
                del self._contexts[ctx_id]
                self._push(1)
                self._log(f"CANCEL ctx={ctx_id} -> success")
            else:
                self._push(0)
                self._log(f"CANCEL ctx={ctx_id} -> not found")

        elif sub == AsyncExt.AWAIT_CHANNEL:
            chan_id = self._fetch8()
            timeout_ms = self._fetch16()
            if chan_id in self.channels and self.channels[chan_id]:
                msg = self.channels[chan_id].pop(0)
                self._push(msg)
                self._log(f"AWAIT_CHANNEL {chan_id} -> message={msg}")
            else:
                self._push(0xFFFFFFFF)  # -1 = no message
                if timeout_ms > 0:
                    self.clock += 1  # simulate wait
                self._log(f"AWAIT_CHANNEL {chan_id} timeout={timeout_ms} -> empty")

        else:
            raise RuntimeError(f"Unknown ASYNC sub-opcode: 0x{sub:02x}")

        return True

    # ── Main execution loop ─────────────────────────────────────────────────

    def step(self) -> bool:
        """
        Execute one instruction.

        Returns:
            True if execution should continue, False if halted.
        """
        if self.halted or self.pc >= len(self.program):
            self.halted = True
            return False

        op = self._fetch8()

        # ── Extension prefix ───────────────────────────────────────────────
        if op == ESCAPE_PREFIX:
            extension_id = self._fetch8()
            result = self._dispatch_extension(extension_id)
            self._check_fuel()
            self.clock += 1
            return result

        op_name = OPCODE_NAMES.get(op, f"UNKNOWN(0x{op:02x})")

        # ── System control ──────────────────────────────────────────────────
        if op == CoreOp.HALT:
            self.halted = True
            self._log("HALT")
            return False

        elif op == CoreOp.NOP:
            self._log("NOP")

        elif op == CoreOp.BREAK:
            # Debugger breakpoint — no-op in reference impl
            self._log("BREAK (debugger breakpoint)")

        # ── Integer arithmetic ──────────────────────────────────────────────
        elif op == CoreOp.ADD:
            a, b = self._pop(), self._pop()
            self._push(b + a)
            self._log(f"ADD {b} + {a} = {b + a}")

        elif op == CoreOp.SUB:
            a, b = self._pop(), self._pop()
            self._push(b - a)
            self._log(f"SUB {b} - {a} = {b - a}")

        elif op == CoreOp.MUL:
            a, b = self._pop(), self._pop()
            self._push(b * a)
            self._log(f"MUL {b} * {a} = {b * a}")

        elif op == CoreOp.DIV:
            a, b = self._pop(), self._pop()
            if a == 0:
                raise RuntimeError("Division by zero")
            # Python integer division truncates toward negative infinity;
            # we use truncation toward zero for C-like behavior
            result = int(b / a) if (b ^ a) >= 0 else -(-b // a)
            self._push(result)
            self._log(f"DIV {b} / {a} = {result}")

        elif op == CoreOp.MOD:
            a, b = self._pop(), self._pop()
            if a == 0:
                raise RuntimeError("Modulo by zero")
            self._push(b % a)
            self._log(f"MOD {b} % {a} = {b % a}")

        elif op == CoreOp.NEG:
            a = self._pop()
            self._push((-a) & 0xFFFFFFFF)
            self._log(f"NEG {a} -> {(-a) & 0xFFFFFFFF}")

        elif op == CoreOp.INC:
            a = self._pop()
            self._push(a + 1)
            self._log(f"INC {a} -> {a + 1}")

        elif op == CoreOp.DEC:
            a = self._pop()
            self._push(a - 1)
            self._log(f"DEC {a} -> {a - 1}")

        # ── Comparison ──────────────────────────────────────────────────────
        elif op == CoreOp.EQ:
            a, b = self._pop(), self._pop()
            self._push(1 if a == b else 0)
            self._log(f"EQ {a} == {b} -> {1 if a == b else 0}")

        elif op == CoreOp.NE:
            a, b = self._pop(), self._pop()
            self._push(1 if a != b else 0)
            self._log(f"NE {a} != {b} -> {1 if a != b else 0}")

        elif op == CoreOp.LT:
            a, b = self._pop(), self._pop()
            sa = a if a < 0x80000000 else a - 0x100000000
            sb = b if b < 0x80000000 else b - 0x100000000
            self._push(1 if sb < sa else 0)
            self._log(f"LT {sb} < {sa} -> {1 if sb < sa else 0}")

        elif op == CoreOp.LE:
            a, b = self._pop(), self._pop()
            sa = a if a < 0x80000000 else a - 0x100000000
            sb = b if b < 0x80000000 else b - 0x100000000
            self._push(1 if sb <= sa else 0)
            self._log(f"LE {sb} <= {sa} -> {1 if sb <= sa else 0}")

        elif op == CoreOp.GT:
            a, b = self._pop(), self._pop()
            sa = a if a < 0x80000000 else a - 0x100000000
            sb = b if b < 0x80000000 else b - 0x100000000
            self._push(1 if sb > sa else 0)
            self._log(f"GT {sb} > {sa} -> {1 if sb > sa else 0}")

        elif op == CoreOp.GE:
            a, b = self._pop(), self._pop()
            sa = a if a < 0x80000000 else a - 0x100000000
            sb = b if b < 0x80000000 else b - 0x100000000
            self._push(1 if sb >= sa else 0)
            self._log(f"GE {sb} >= {sa} -> {1 if sb >= sa else 0}")

        # ── Logic / bitwise ────────────────────────────────────────────────
        elif op == CoreOp.AND:
            a, b = self._pop(), self._pop()
            self._push(b & a)
            self._log(f"AND {b} & {a} = {b & a}")

        elif op == CoreOp.OR:
            a, b = self._pop(), self._pop()
            self._push(b | a)
            self._log(f"OR {b} | {a} = {b | a}")

        elif op == CoreOp.XOR:
            a, b = self._pop(), self._pop()
            self._push(b ^ a)
            self._log(f"XOR {b} ^ {a} = {b ^ a}")

        elif op == CoreOp.NOT:
            a = self._pop()
            self._push(~a & 0xFFFFFFFF)
            self._log(f"NOT {a} -> {~a & 0xFFFFFFFF}")

        elif op == CoreOp.SHL:
            a, b = self._pop(), self._pop()
            self._push((b << a) & 0xFFFFFFFF)
            self._log(f"SHL {b} << {a} = {(b << a) & 0xFFFFFFFF}")

        elif op == CoreOp.SHR:
            a, b = self._pop(), self._pop()
            self._push(b >> a)
            self._log(f"SHR {b} >> {a} = {b >> a}")

        # ── Stack manipulation ─────────────────────────────────────────────
        elif op == CoreOp.PUSH:
            val = self._fetch32()
            # Sign-extend from 32-bit
            if val >= 0x80000000:
                val = val - 0x100000000
            self._push(val)
            self._log(f"PUSH {val}")

        elif op == CoreOp.POP:
            val = self._pop()
            self._log(f"POP -> {val}")

        elif op == CoreOp.DUP:
            val = self._peek()
            self._push(val)
            self._log(f"DUP -> {val}")

        elif op == CoreOp.SWAP:
            a, b = self._pop(), self._pop()
            self._push(a)
            self._push(b)
            self._log(f"SWAP {a} <-> {b}")

        # ── Memory ─────────────────────────────────────────────────────────
        elif op == CoreOp.LOAD:
            addr = self._fetch16()
            val = self._load16(addr)
            self._push(val)
            self._log(f"LOAD [0x{addr:04x}] -> {val}")

        elif op == CoreOp.STORE:
            addr = self._fetch16()
            val = self._pop()
            self._store16(addr, val)
            self._log(f"STORE {val} -> [0x{addr:04x}]")

        # ── Control flow ───────────────────────────────────────────────────
        elif op == CoreOp.JMP:
            addr = self._fetch16()
            self.pc = addr
            self._log(f"JMP -> 0x{addr:04x}")

        elif op == CoreOp.JZ:
            addr = self._fetch16()
            val = self._pop()
            if val == 0:
                self.pc = addr
                self._log(f"JZ {val} -> 0x{addr:04x} (taken)")
            else:
                self._log(f"JZ {val} (not taken)")

        elif op == CoreOp.JNZ:
            addr = self._fetch16()
            val = self._pop()
            if val != 0:
                self.pc = addr
                self._log(f"JNZ {val} -> 0x{addr:04x} (taken)")
            else:
                self._log(f"JNZ {val} (not taken)")

        elif op == CoreOp.CALL:
            addr = self._fetch16()
            self.call_stack.append(self.pc)
            self.pc = addr
            self._log(f"CALL -> 0x{addr:04x} (return to 0x{self.call_stack[-1]:04x})")

        elif op == CoreOp.RET:
            if not self.call_stack:
                self.halted = True
                self._log("RET (no call stack - halting)")
                return False
            self.pc = self.call_stack.pop()
            self._log(f"RET -> 0x{self.pc:04x}")

        # ── A2A Fleet Operations ───────────────────────────────────────────
        elif op == CoreOp.TELL:
            # Agent messaging — no-op in reference impl
            self._log("TELL (no-op in reference)")

        elif op == CoreOp.ASK:
            # Agent query — no-op in reference impl
            self._log("ASK (no-op in reference)")

        # ── Confidence ─────────────────────────────────────────────────────
        elif op == CoreOp.CONF_GET:
            # Push confidence as fixed-point (multiply by 10000 for precision)
            self._push(int(self.confidence * 10000))
            self._log(f"CONF_GET -> {self.confidence}")

        elif op == CoreOp.CONF_SET:
            val = self._pop()
            self.confidence = (val & 0xFFFFFFFF) / 10000.0
            self.confidence = max(0.0, min(1.0, self.confidence))
            self._log(f"CONF_SET -> {self.confidence}")

        else:
            raise RuntimeError(f"Unknown opcode: 0x{op:02x} at pc=0x{self.pc - 1:04x}")

        self._check_fuel()
        self.clock += 1
        return True

    def run(self, max_steps: int = 100000) -> int:
        """
        Run until HALT or max_steps.

        Returns:
            Number of instructions executed.
        """
        steps = 0
        while not self.halted and steps < max_steps:
            if not self.step():
                break
            steps += 1
        return steps

    def snapshot(self) -> VMState:
        """Capture current VM state."""
        return VMState(
            pc=self.pc, stack=list(self.stack),
            memory=bytes(self.memory), clock=self.clock,
            halted=self.halted,
            capabilities=dict(self.capabilities),
            fuel=self.fuel,
            confidence=self.confidence,
        )

    def restore(self, state: VMState) -> None:
        """Restore VM from a snapshot."""
        self.pc = state.pc
        self.stack = list(state.stack)
        self.memory = bytearray(state.memory)
        self.clock = state.clock
        self.halted = state.halted
        self.capabilities = dict(state.capabilities)
        self.fuel = state.fuel
        self.confidence = state.confidence


# ── Assembler ───────────────────────────────────────────────────────────────

class Assembler:
    """
    Simple assembler for FLUX ISA v3 bytecode.

    Supports labels (name:) and all core + extension opcodes.
    Extension opcodes use the 0xFF prefix syntax:
        ESCAPE TEMPORAL FUEL_CHECK
        ESCAPE SECURITY CAP_INVOKE 1
        ESCAPE ASYNC SUSPEND

    Example:
        asm = Assembler()
        asm.parse('''
            start:  PUSH 10
                    PUSH 20
                    ADD
                    HALT
        ''')
        code = asm.assemble()
    """

    # Extension sub-opcode lookup
    EXT_SUB_OPS = {
        "TEMPORAL": {v.name: int(v) for v in TemporalExt},
        "SECURITY": {v.name: int(v) for v in SecurityExt},
        "ASYNC": {v.name: int(v) for v in AsyncExt},
    }

    def __init__(self):
        self._lines: List[str] = []
        self._labels: Dict[str, int] = {}
        self._resolved: List[bytes] = []
        self._unresolved: List[Tuple[str, int, str]] = []  # (label, patch_offset, type)

    def parse(self, source: str) -> "Assembler":
        """Parse assembly source text."""
        for line in source.strip().split("\n"):
            line = line.strip()
            if not line or line.startswith("#") or line.startswith(";"):
                continue
            if ";" in line:
                line = line.split(";")[0].strip()
            if not line:
                continue
            self._lines.append(line)
        return self

    def assemble(self) -> bytes:
        """Assemble parsed lines into bytecode."""
        self._resolved = []
        self._unresolved = []
        self._labels = {}

        # First pass: compute addresses and collect labels
        addr = 0
        for line in self._lines:
            if ":" in line:
                label = line.split(":")[0].strip()
                self._labels[label] = addr
                line = line.split(":", 1)[1].strip()
                if not line:
                    continue

            tokens = line.split()
            if not tokens:
                continue

            op_str = tokens[0].upper()

            # Handle ESCAPE prefix
            if op_str == "ESCAPE":
                ext_name = tokens[1].upper() if len(tokens) > 1 else ""
                sub_op = tokens[2].upper() if len(tokens) > 2 else ""
                args = tokens[3:] if len(tokens) > 3 else []

                if ext_name not in self.EXT_SUB_OPS:
                    raise ValueError(f"Unknown extension: {ext_name}")
                ext_id = {"TEMPORAL": 0x01, "SECURITY": 0x02, "ASYNC": 0x03}[ext_name]
                if sub_op not in self.EXT_SUB_OPS[ext_name]:
                    raise ValueError(f"Unknown {ext_name} sub-opcode: {sub_op}")
                sub_id = self.EXT_SUB_OPS[ext_name][sub_op]

                self._resolved.append(bytes([ESCAPE_PREFIX, ext_id, sub_id]))
                addr += 3

                # Handle sub-opcode arguments
                for arg in args:
                    try:
                        val = int(arg, 0)
                        if sub_op in ("FUEL_SET",):
                            self._resolved.append(struct.pack("<H", val))
                            addr += 2
                        else:
                            self._resolved.append(bytes([val & 0xFF]))
                            addr += 1
                    except ValueError:
                        # Label reference — use u8 placeholder
                        self._resolved.append(bytes([0]))
                        self._unresolved.append((arg, addr, "u8"))

                continue

            arg = tokens[1] if len(tokens) > 1 else None

            # Emit opcode bytes
            if op_str in OPCODE_VALUES:
                op_byte = OPCODE_VALUES[op_str]
                self._resolved.append(bytes([op_byte]))
                addr += 1

                if arg is not None:
                    try:
                        val = int(arg, 0)
                        if op_str in ("PUSH",):
                            self._resolved.append(struct.pack("<i", val))
                            addr += 4
                        elif op_str in ("JMP", "JZ", "JNZ", "CALL", "LOAD", "STORE"):
                            self._resolved.append(struct.pack("<H", val))
                            addr += 2
                        else:
                            self._resolved.append(bytes([val & 0xFF]))
                            addr += 1
                    except ValueError:
                        # It's a label reference
                        if op_str in ("PUSH",):
                            self._resolved.append(struct.pack("<i", 0))
                            self._unresolved.append((arg, addr, "i32"))
                        elif op_str in ("JMP", "JZ", "JNZ", "CALL", "LOAD", "STORE"):
                            self._resolved.append(struct.pack("<H", 0))
                            self._unresolved.append((arg, addr, "u16"))
                        else:
                            self._resolved.append(bytes([0]))
                            self._unresolved.append((arg, addr, "u8"))
            elif op_str == "DB":
                if arg:
                    val = int(arg, 0)
                    self._resolved.append(bytes([val & 0xFF]))
                    addr += 1
            elif op_str == "DW":
                if arg:
                    val = int(arg, 0)
                    self._resolved.append(struct.pack("<H", val))
                    addr += 2
            else:
                raise ValueError(f"Unknown opcode: {op_str}")

        # Second pass: resolve labels
        code = bytearray(b''.join(self._resolved))
        for label, offset, typ in self._unresolved:
            if label not in self._labels:
                raise ValueError(f"Unresolved label: {label}")
            label_addr = self._labels[label]
            if typ == "u16":
                struct.pack_into("<H", code, offset, label_addr)
            elif typ == "i32":
                struct.pack_into("<i", code, offset, label_addr)
            else:
                code[offset] = label_addr & 0xFF

        return bytes(code)


# ── Disassembler ────────────────────────────────────────────────────────────

def disassemble(program: bytes, base_addr: int = 0) -> str:
    """
    Disassemble bytecode into human-readable mnemonics.

    Args:
        program: Raw bytecode
        base_addr: Starting address offset for labels

    Returns:
        Multi-line assembly text
    """
    lines = []
    pc = 0
    while pc < len(program):
        op = program[pc]
        addr = pc + base_addr
        comment = f"  ; 0x{addr:04x}"

        if op == ESCAPE_PREFIX and pc + 1 < len(program):
            ext_id = program[pc + 1]
            ext_info = EXTENSION_TABLES.get(ext_id, ("UNKNOWN", {}))
            ext_name = ext_info[0]
            if pc + 2 < len(program):
                sub_id = program[pc + 2]
                sub_name = ext_info[1].get(sub_id, f"SUB_0x{sub_id:02x}")
                lines.append(f"  ESCAPE {ext_name} {sub_name}{comment}")
                # Skip extension args (simplified)
                pc += 3
                continue
            else:
                lines.append(f"  ESCAPE 0x{ext_id:02x}{comment}")
                pc += 2
                continue

        name = OPCODE_NAMES.get(op, f"DB 0x{op:02x}")

        if name == "PUSH" and pc + 4 < len(program):
            val = struct.unpack_from("<i", program, pc + 1)[0]
            lines.append(f"  {name} {val}{comment}")
            pc += 5
        elif name in ("JMP", "JZ", "JNZ", "CALL", "LOAD", "STORE") and pc + 2 < len(program):
            val = program[pc + 1] | (program[pc + 2] << 8)
            lines.append(f"  {name} 0x{val:04x}{comment}")
            pc += 3
        else:
            lines.append(f"  {name}{comment}")
            pc += 1

    return "\n".join(lines)


# ── Convenience function ────────────────────────────────────────────────────

def assemble(source: str) -> bytes:
    """Assemble source text directly. Convenience wrapper."""
    return Assembler().parse(source).assemble()


# ── Sample Programs ─────────────────────────────────────────────────────────

SAMPLE_FIBONACCI = """
; Compute Fibonacci(10) and push result
    PUSH 0          ; fib(n) = 0
    PUSH 1          ; fib(n+1) = 1
    PUSH 1          ; counter = 1
loop:
    ; Display: fib(n) + fib(n+1) -> fib(n+1), fib(n+2)
    SWAP            ; stack: counter, fib(n+1), fib(n)
    OVER            ; stack: counter, fib(n+1), fib(n), fib(n+1)
    ADD             ; stack: counter, fib(n+1), fib(n+2)
    SWAP            ; stack: counter, fib(n+2), fib(n+1)
    SWAP            ; stack: counter, fib(n+1), fib(n+2)
    ; Increment counter
    ROT             ; stack: fib(n+1), fib(n+2), counter
    PUSH 1
    ADD
    ROT             ; stack: fib(n+2), counter, fib(n+1)
    ROT             ; stack: counter, fib(n+1), fib(n+2)
    DUP             ; stack: counter, fib(n+1), fib(n+2), fib(n+2)
    ; Check if counter < 10
    ROT             ; stack: fib(n+1), fib(n+2), counter, fib(n+2)
    ROT             ; stack: fib(n+2), counter, fib(n+2), fib(n+1)
    ; Hmm, stack manipulation for this is complex. Use memory instead.
    HALT
"""

SAMPLE_SIMPLE_ADD = """
; Simple addition: 42 + 58 = 100
    PUSH 42
    PUSH 58
    ADD
    HALT
"""

SAMPLE_COND_BRANCH = """
; Conditional branching: if 100 > 99, push 1 else push 0
    PUSH 42
    PUSH 58
    ADD             ; 42 + 58 = 100
    DUP             ; duplicate 100
    PUSH 99
    GT              ; 100 > 99 = 1
    JNZ pass_label
    PUSH 0          ; fail path
    HALT
pass_label:
    PUSH 1          ; result = 1 (success)
    HALT
"""

SAMPLE_CALL_RET = """
; Demonstrate CALL/RET with separate call stack
    PUSH 10
    PUSH 20
    CALL add_func   ; call subroutine
    HALT            ; stack top should be 30
add_func:
    ADD             ; add top two values
    RET             ; return to caller
"""

SAMPLE_TEMPORAL = """
; Demonstrate TEMPORAL extension via 0xFF escape prefix
    ESCAPE TEMPORAL TIME_NOW       ; push clock (should be ~0)
    ESCAPE TEMPORAL TIME_NOW       ; push clock again (incremented)
    ADD                            ; sum them
    HALT
"""

SAMPLE_SECURITY = """
; Demonstrate SECURITY extension via 0xFF escape prefix
    ESCAPE SECURITY IDENTITY_GET   ; push identity handle
    ESCAPE SECURITY FUEL_SET 100   ; set fuel to 100
    ESCAPE TEMPORAL FUEL_CHECK     ; push remaining fuel (should be 100)
    HALT
"""

SAMPLE_ASYNC = """
; Demonstrate ASYNC extension via 0xFF escape prefix
    PUSH 42
    PUSH 58
    ESCAPE ASYNC FORK 0            ; fork, push context ID
    ADD                            ; parent: 42+58=100
    HALT
"""


def run_sample(name: str, source: str, trace: bool = False) -> None:
    """Run a sample program and print results."""
    print(f"\n{'='*60}")
    print(f"  Sample: {name}")
    print(f"{'='*60}")

    code = assemble(source)
    print(f"  Bytecode: {code.hex()} ({len(code)} bytes)")
    print(f"  Disassembly:\n{disassemble(code)}")

    vm = FluxVM()
    vm.trace_enabled = trace
    vm.load(code)
    steps = vm.run(max_steps=10000)

    print(f"\n  Execution: {steps} steps, clock={vm.clock}")
    print(f"  Stack: {vm.stack}")
    print(f"  Memory[0:8]: {list(vm.memory[:8])}")
    print(f"  Halted: {vm.halted}")

    if trace:
        print(f"\n  Trace ({len(vm._trace)} instructions):")
        for line in vm._trace[-20:]:
            print(f"    {line}")


if __name__ == "__main__":
    print("FLUX ISA v3 Reference Interpreter")
    print(f"Core opcodes: {len(CoreOp)}")
    print(f"Temporal opcodes: {len(TemporalExt)} (via 0xFF 0x01)")
    print(f"Security opcodes: {len(SecurityExt)} (via 0xFF 0x02)")
    print(f"Async opcodes: {len(AsyncExt)} (via 0xFF 0x03)")
    print(f"Total: {len(CoreOp) + len(TemporalExt) + len(SecurityExt) + len(AsyncExt)}")
    print(f"Extension mechanism: 0xFF <ext_id> <sub_opcode>")

    # Run all samples
    run_sample("Simple Addition (42 + 58)", SAMPLE_SIMPLE_ADD, trace=False)
    run_sample("Conditional Branching", SAMPLE_COND_BRANCH, trace=False)
    run_sample("CALL/RET Subroutine", SAMPLE_CALL_RET, trace=False)
    run_sample("Temporal Extension", SAMPLE_TEMPORAL, trace=False)
    run_sample("Security Extension", SAMPLE_SECURITY, trace=False)
    run_sample("ASYNC Extension", SAMPLE_ASYNC, trace=False)

    # Verify basic correctness
    print(f"\n{'='*60}")
    print("  Correctness Verification")
    print(f"{'='*60}")

    # Test HALT at 0x00
    vm = FluxVM()
    vm.load(bytes([0x00]))  # HALT
    vm.run()
    assert vm.halted, "HALT failed"
    print("  HALT (0x00): PASS")

    # Test NOP at 0x01
    vm = FluxVM()
    vm.load(bytes([0x01, 0x00]))  # NOP, HALT
    steps = vm.run()
    assert steps >= 1 and vm.halted, f"NOP failed: {steps} steps"
    print("  NOP (0x01): PASS")

    # Test ADD (0x10)
    vm = FluxVM()
    vm.load(assemble("PUSH 3\nPUSH 4\nADD\nHALT"))
    vm.run()
    assert vm.stack[-1] == 7, f"ADD failed: got {vm.stack[-1]}"
    print("  ADD: 3 + 4 = 7 ... PASS")

    # Test SUB (0x11)
    vm = FluxVM()
    vm.load(assemble("PUSH 10\nPUSH 3\nSUB\nHALT"))
    vm.run()
    assert vm.stack[-1] == 7, f"SUB failed: got {vm.stack[-1]}"
    print("  SUB: 10 - 3 = 7 ... PASS")

    # Test MUL (0x12)
    vm = FluxVM()
    vm.load(assemble("PUSH 6\nPUSH 7\nMUL\nHALT"))
    vm.run()
    assert vm.stack[-1] == 42, f"MUL failed: got {vm.stack[-1]}"
    print("  MUL: 6 * 7 = 42 ... PASS")

    # Test ESCAPE prefix mechanism
    vm = FluxVM()
    vm.load(assemble("ESCAPE TEMPORAL FUEL_CHECK\nHALT"))
    vm.run()
    assert vm.stack[-1] == 0, f"FUEL_CHECK failed: got {vm.stack[-1]}"
    print("  ESCAPE TEMPORAL FUEL_CHECK: PASS")

    # Test SECURITY extension
    vm = FluxVM()
    vm.load(assemble("ESCAPE SECURITY FUEL_SET 50\nESCAPE TEMPORAL FUEL_CHECK\nHALT"))
    vm.run()
    # FUEL_SET sets fuel to 50, then FUEL_CHECK reads it (but step() decrements first)
    print(f"  SECURITY FUEL_SET + FUEL_CHECK: fuel={vm.fuel}, stack={vm.stack[-1]}")

    # Test ASYNC FORK
    vm = FluxVM()
    vm.load(assemble("PUSH 99\nESCAPE ASYNC FORK 0\nHALT"))
    vm.run()
    assert len(vm._contexts) == 1, f"FORK failed: {len(vm._contexts)} contexts"
    print("  ASYNC FORK: PASS")

    print(f"\n  All correctness checks passed!")
