"""
reference/interpreter.py — ISA v3 Reference Interpreter

A stack-based virtual machine implementing the FLUX ISA v3 core instruction set
plus TEMPORAL, SECURITY, and ASYNC extension opcodes. This serves as the
authoritative reference implementation for cross-runtime conformance testing.

Architecture:
    - 256-byte addressable memory (expandable)
    - 256-entry stack (expandable)
    - Program counter (16-bit)
    - 26 core opcodes (17 irreducible + 9 extended)
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


# ── Opcode Definitions ──────────────────────────────────────────────────────

class CoreOp(IntEnum):
    """Core opcodes — the 17 irreducible Turing-complete set plus extras."""
    NOP    = 0x00
    PUSH   = 0x01   # PUSH <u8>     — push immediate byte onto stack
    PUSH16 = 0x02   # PUSH16 <u16>  — push 16-bit immediate
    POP    = 0x03   # POP           — discard top of stack
    DUP    = 0x04   # DUP           — duplicate top of stack
    SWAP   = 0x05   # SWAP          — swap top two stack entries
    OVER   = 0x06   # OVER          — copy second-from-top to top
    ROT    = 0x07   # ROT           — rotate top three: c b a -> b a c
    ADD    = 0x10   # ADD           — pop a, b; push a + b
    SUB    = 0x11   # SUB           — pop a, b; push b - a
    MUL    = 0x12   # MUL           — pop a, b; push a * b
    DIV    = 0x13   # DIV           — pop a, b; push b // a (integer)
    MOD    = 0x14   # MOD           — pop a, b; push b % a
    INC    = 0x15   # INC           — pop a; push a + 1
    DEC    = 0x16   # DEC           — pop a; push a - 1
    AND    = 0x20   # AND           — bitwise AND
    OR     = 0x21   # OR            # bitwise OR
    XOR    = 0x22   # XOR           # bitwise XOR
    NOT    = 0x23   # NOT           # bitwise NOT
    SHL    = 0x24   # SHL           # shift left
    SHR    = 0x25   # SHR           # shift right
    EQ     = 0x30   # EQ            # pop a, b; push 1 if a == b else 0
    NE     = 0x31   # NE            # not equal
    LT     = 0x32   # LT            # less than (signed)
    GT     = 0x33   # GT            # greater than (signed)
    LTE    = 0x34   # LTE           # less than or equal
    GTE    = 0x35   # GTE           # greater than or equal
    JMP    = 0x40   # JMP <u16>     — unconditional jump
    JZ     = 0x41   # JZ <u16>      — jump if top == 0
    JNZ    = 0x42   # JNZ <u16>     — jump if top != 0
    CALL   = 0x43   # CALL <u16>    — push return address, jump
    RET    = 0x44   # RET           — pop return address, jump
    LOAD   = 0x50   # LOAD <u8>     — load from memory[addr] onto stack
    STORE  = 0x51   # STORE <u8>    — store top of stack to memory[addr]
    LOAD16 = 0x52   # LOAD16 <u16>  — load 16-bit from memory
    STORE16= 0x53   # STORE16 <u16> — store 16-bit to memory
    HALT   = 0xFF   # HALT          — stop execution


class TemporalOp(IntEnum):
    """Extension 0x01 — TEMPORAL opcodes for time-aware execution."""
    TICK   = 0xE0   # TICK          — push current clock value
    WAIT   = 0xE1   # WAIT <u8>     — pause execution for n cycles
    ALARM  = 0xE2   # ALARM <u8> <u16> — set alarm at tick+offset, jump on trigger
    CANCEL = 0xE3   # CANCEL        — cancel active alarm
    EPOCH  = 0xE4   # EPOCH <u8>    — set epoch base for TICK
    DELTA  = 0xE5   # DELTA         — push ticks since last WAIT/ALARM


class SecurityOp(IntEnum):
    """Extension 0x02 — SECURITY opcodes for capability-based access control."""
    CAP_DECLARE  = 0xD0  # CAP_DECLARE <cap_id:u8> — declare a capability requirement
    CAP_CHECK    = 0xD1  # CAP_CHECK <cap_id:u8>  — verify capability, push 1/0
    CAP_INVOKE   = 0xD2  # CAP_INVOKE <cap_id:u8> <n_args:u8> — invoke with capabilities
    CAP_DROP     = 0xD3  # CAP_DROP <cap_id:u8>   — release a capability
    CAP_DELEGATE = 0xD4  # CAP_DELEGATE <src:u8> <dst:u8> — delegate capability
    CAP_AUDIT    = 0xD5  # CAP_AUDIT — push audit log count


class AsyncOp(IntEnum):
    """Extension 0x03 — ASYNC opcodes for concurrent execution."""
    SPAWN  = 0xC0   # SPAWN <u16>   — fork execution at address
    SUSPEND= 0xC1   # SUSPEND      — save state as continuation handle
    RESUME = 0xC2   # RESUME       — restore from continuation handle
    JOIN   = 0xC3   # JOIN         — wait for spawned task to complete
    YIELD  = 0xC4   # YIELD        — voluntarily yield execution
    CHAN   = 0xC5   # CHAN <u8>     — create or access channel


# ── Opcode dispatch table ───────────────────────────────────────────────────

OPCODE_NAMES: Dict[int, str] = {}
OPCODE_VALUES: Dict[str, int] = {}  # reverse map for assembler
for cls in [CoreOp, TemporalOp, SecurityOp, AsyncOp]:
    for op in cls:
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
    alarms: List[Tuple[int, int]] = field(default_factory=list)  # (trigger_tick, target_addr)
    channels: Dict[int, List[int]] = field(default_factory=list)

    def to_dict(self) -> Dict[str, Any]:
        return {
            "pc": self.pc, "stack": self.stack,
            "memory": list(self.memory), "clock": self.clock,
            "halted": self.halted,
            "capabilities": self.capabilities,
            "alarms": self.alarms,
            "channels": {k: v for k, v in self.channels.items() if isinstance(v, list)},
        }


# ── Virtual Machine ─────────────────────────────────────────────────────────

class FluxVM:
    """
    FLUX ISA v3 Reference Interpreter.

    A stack-based VM with 256-byte memory, 26 core opcodes, and optional
    extension support for TEMPORAL, SECURITY, and ASYNC operations.

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
        self.alarms: List[Tuple[int, int, int]] = []  # (trigger_tick, target_addr, alarm_id)
        self._alarm_counter = 0
        self.channels: Dict[int, List[int]] = {}
        self._spawned: List["FluxVM"] = []
        self._trace: List[str] = []
        self.trace_enabled = False

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
        self.alarms.clear()
        self._alarm_counter = 0
        self.channels.clear()
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

    # ── Trace ───────────────────────────────────────────────────────────────

    def _log(self, msg: str) -> None:
        if self.trace_enabled:
            self._trace.append(f"[pc={self.pc:04x} clk={self.clock}] {msg}")

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
        op_name = OPCODE_NAMES.get(op, f"UNKNOWN(0x{op:02x})")

        # ── Core opcodes ────────────────────────────────────────────────────
        if op == CoreOp.NOP:
            self._log("NOP")

        elif op == CoreOp.PUSH:
            val = self._fetch8()
            self._push(val)
            self._log(f"PUSH {val}")

        elif op == CoreOp.PUSH16:
            val = self._fetch16()
            self._push(val)
            self._log(f"PUSH16 {val}")

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

        elif op == CoreOp.OVER:
            val = self._peek(1)
            self._push(val)
            self._log(f"OVER -> {val}")

        elif op == CoreOp.ROT:
            c = self._pop()
            b = self._pop()
            a = self._pop()
            self._push(b)
            self._push(c)
            self._push(a)
            self._log(f"ROT {a} {b} {c} -> {b} {c} {a}")

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
            self._push(int(b / a))
            self._log(f"DIV {b} / {a} = {int(b / a)}")

        elif op == CoreOp.MOD:
            a, b = self._pop(), self._pop()
            if a == 0:
                raise RuntimeError("Modulo by zero")
            self._push(b % a)
            self._log(f"MOD {b} % {a} = {b % a}")

        elif op == CoreOp.INC:
            a = self._pop()
            self._push(a + 1)
            self._log(f"INC {a} -> {a + 1}")

        elif op == CoreOp.DEC:
            a = self._pop()
            self._push(a - 1)
            self._log(f"DEC {a} -> {a - 1}")

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
            self._push(b << a)
            self._log(f"SHL {b} << {a} = {b << a}")

        elif op == CoreOp.SHR:
            a, b = self._pop(), self._pop()
            self._push(b >> a)
            self._log(f"SHR {b} >> {a} = {b >> a}")

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
            # Signed comparison
            sa = a if a < 0x80000000 else a - 0x100000000
            sb = b if b < 0x80000000 else b - 0x100000000
            self._push(1 if sb < sa else 0)
            self._log(f"LT {sb} < {sa} -> {1 if sb < sa else 0}")

        elif op == CoreOp.GT:
            a, b = self._pop(), self._pop()
            sa = a if a < 0x80000000 else a - 0x100000000
            sb = b if b < 0x80000000 else b - 0x100000000
            self._push(1 if sb > sa else 0)
            self._log(f"GT {sb} > {sa} -> {1 if sb > sa else 0}")

        elif op == CoreOp.LTE:
            a, b = self._pop(), self._pop()
            sa = a if a < 0x80000000 else a - 0x100000000
            sb = b if b < 0x80000000 else b - 0x100000000
            self._push(1 if sb <= sa else 0)
            self._log(f"LTE {sb} <= {sa} -> {1 if sb <= sa else 0}")

        elif op == CoreOp.GTE:
            a, b = self._pop(), self._pop()
            sa = a if a < 0x80000000 else a - 0x100000000
            sb = b if b < 0x80000000 else b - 0x100000000
            self._push(1 if sb >= sa else 0)
            self._log(f"GTE {sb} >= {sa} -> {1 if sb >= sa else 0}")

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
            self._log(f"CALL -> 0x{addr:04x} (return to 0x{self.pc:04x})")

        elif op == CoreOp.RET:
            if not self.call_stack:
                self.halted = True
                self._log("RET (no call stack — halting)")
                return False
            self.pc = self.call_stack.pop()
            self._log(f"RET -> 0x{self.pc:04x}")

        elif op == CoreOp.LOAD:
            addr = self._fetch8()
            val = self._load8(addr)
            self._push(val)
            self._log(f"LOAD [0x{addr:02x}] -> {val}")

        elif op == CoreOp.STORE:
            addr = self._fetch8()
            val = self._pop()
            self._store8(addr, val)
            self._log(f"STORE {val} -> [0x{addr:02x}]")

        elif op == CoreOp.LOAD16:
            addr = self._fetch16()
            val = self._load16(addr)
            self._push(val)
            self._log(f"LOAD16 [0x{addr:04x}] -> {val}")

        elif op == CoreOp.STORE16:
            addr = self._fetch16()
            val = self._pop()
            self._store16(addr, val)
            self._log(f"STORE16 {val} -> [0x{addr:04x}]")

        elif op == CoreOp.HALT:
            self.halted = True
            self._log("HALT")
            return False

        # ── TEMPORAL extension (0x01) ───────────────────────────────────────
        elif op == TemporalOp.TICK:
            self._push(self.clock)
            self._log(f"TICK -> {self.clock}")

        elif op == TemporalOp.WAIT:
            cycles = self._fetch8()
            self.clock += cycles
            # Check alarms
            self._check_alarms()
            self._log(f"WAIT {cycles} cycles (clock now {self.clock})")

        elif op == TemporalOp.ALARM:
            offset = self._fetch8()
            target = self._fetch16()
            alarm_id = self._alarm_counter
            self._alarm_counter += 1
            self.alarms.append((self.clock + offset, target, alarm_id))
            self._log(f"ALARM id={alarm_id} at tick {self.clock + offset} -> 0x{target:04x}")

        elif op == TemporalOp.CANCEL:
            if self.alarms:
                self.alarms.pop()
                self._log("CANCEL (removed last alarm)")
            else:
                self._log("CANCEL (no alarms)")

        elif op == TemporalOp.EPOCH:
            epoch = self._fetch8()
            self.clock = epoch
            self._log(f"EPOCH -> {epoch}")

        elif op == TemporalOp.DELTA:
            # Push clock (in real impl this would be ticks since last WAIT)
            self._push(self.clock)
            self._log(f"DELTA -> {self.clock}")

        # ── SECURITY extension (0x02) ───────────────────────────────────────
        elif op == SecurityOp.CAP_DECLARE:
            cap_id = self._fetch8()
            self.capabilities[cap_id] = True
            self._log(f"CAP_DECLARE {cap_id}")

        elif op == SecurityOp.CAP_CHECK:
            cap_id = self._fetch8()
            has = 1 if self.capabilities.get(cap_id, False) else 0
            self._push(has)
            self._log(f"CAP_CHECK {cap_id} -> {has}")

        elif op == SecurityOp.CAP_INVOKE:
            cap_id = self._fetch8()
            n_args = self._fetch8()
            if not self.capabilities.get(cap_id, False):
                raise RuntimeError(f"CAP_INVOKE: capability {cap_id} not held")
            args = [self._pop() for _ in range(n_args)][::-1]
            self._log(f"CAP_INVOKE {cap_id} with {n_args} args {args}")

        elif op == SecurityOp.CAP_DROP:
            cap_id = self._fetch8()
            self.capabilities[cap_id] = False
            self._log(f"CAP_DROP {cap_id}")

        elif op == SecurityOp.CAP_DELEGATE:
            src = self._fetch8()
            dst = self._fetch8()
            if not self.capabilities.get(src, False):
                raise RuntimeError(f"CAP_DELEGATE: source capability {src} not held")
            self.capabilities[dst] = True
            self._log(f"CAP_DELEGATE {src} -> {dst}")

        elif op == SecurityOp.CAP_AUDIT:
            self._push(len(self.capabilities))
            self._log(f"CAP_AUDIT -> {len(self.capabilities)}")

        # ── ASYNC extension (0x03) ──────────────────────────────────────────
        elif op == AsyncOp.SPAWN:
            addr = self._fetch16()
            child = FluxVM(memory_size=self.memory_size, stack_size=self.stack_size)
            child.load(self.program)
            child.pc = addr
            child.memory = bytearray(self.memory)  # share memory copy
            self._spawned.append(child)
            self._log(f"SPAWN -> 0x{addr:04x} (child #{len(self._spawned)})")

        elif op == AsyncOp.SUSPEND:
            # In single-threaded impl, just halt with state preserved
            self._log("SUSPEND (state preserved)")
            return False

        elif op == AsyncOp.RESUME:
            # Resume last spawned child
            if self._spawned:
                child = self._spawned[-1]
                child.run()
                self._log(f"RESUME child -> stack={child.stack}")
            else:
                self._log("RESUME (no children)")

        elif op == AsyncOp.JOIN:
            if self._spawned:
                child = self._spawned.pop()
                result = child.stack[-1] if child.stack else 0
                self._push(result)
                self._log(f"JOIN -> child result={result}")
            else:
                self._log("JOIN (no children)")

        elif op == AsyncOp.YIELD:
            self.clock += 1
            self._check_alarms()
            self._log("YIELD")

        elif op == AsyncOp.CHAN:
            chan_id = self._fetch8()
            if chan_id not in self.channels:
                self.channels[chan_id] = []
            self._log(f"CHAN {chan_id}")

        else:
            raise RuntimeError(f"Unknown opcode: 0x{op:02x} at pc=0x{self.pc - 1:04x}")

        self.clock += 1
        return True

    def _check_alarms(self) -> None:
        """Check and trigger any expired alarms."""
        triggered = []
        remaining = []
        for trigger_tick, target_addr, alarm_id in self.alarms:
            if self.clock >= trigger_tick:
                self.pc = target_addr
                triggered.append(alarm_id)
                self._log(f"ALARM id={alarm_id} triggered -> 0x{target_addr:04x}")
            else:
                remaining.append((trigger_tick, target_addr, alarm_id))
        self.alarms = remaining

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
            alarms=[(t, a) for t, a, _ in self.alarms],
        )

    def restore(self, state: VMState) -> None:
        """Restore VM from a snapshot."""
        self.pc = state.pc
        self.stack = list(state.stack)
        self.memory = bytearray(state.memory)
        self.clock = state.clock
        self.halted = state.halted
        self.capabilities = dict(state.capabilities)
        self.alarms = [(t, a, i) for i, (t, a) in enumerate(state.alarms)]


# ── Assembler ───────────────────────────────────────────────────────────────

class Assembler:
    """
    Simple assembler for FLUX ISA v3 bytecode.

    Supports labels (name:) and all core + extension opcodes.

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
            # Strip inline comments (but not inside strings)
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
            arg = tokens[1] if len(tokens) > 1 else None

            # Emit opcode bytes
            if op_str in OPCODE_VALUES:
                op_byte = OPCODE_VALUES[op_str]
                self._resolved.append(bytes([op_byte]))
                addr += 1

                if arg is not None:
                    # Try to resolve as number
                    try:
                        val = int(arg, 0)
                        if op_str in ("PUSH16", "JMP", "JZ", "JNZ", "CALL", "ALARM",
                                      "LOAD16", "STORE16", "SPAWN"):
                            self._resolved.append(struct.pack("<H", val))
                            addr += 2
                        else:
                            self._resolved.append(bytes([val]))
                            addr += 1
                    except ValueError:
                        # It's a label reference
                        if op_str in ("PUSH16", "JMP", "JZ", "JNZ", "CALL", "ALARM",
                                      "LOAD16", "STORE16", "SPAWN"):
                            self._resolved.append(struct.pack("<H", 0))  # placeholder
                            self._unresolved.append((arg, addr, "u16"))
                        else:
                            self._resolved.append(bytes([0]))
                            self._unresolved.append((arg, addr, "u8"))
            elif op_str == "DB":
                # Raw byte
                if arg:
                    val = int(arg, 0)
                    self._resolved.append(bytes([val & 0xFF]))
                    addr += 1
            elif op_str == "DW":
                # Raw word
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
            addr = self._labels[label]
            if typ == "u16":
                struct.pack_into("<H", code, offset, addr)
            else:
                code[offset] = addr & 0xFF

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
        name = OPCODE_NAMES.get(op, f"DB 0x{op:02x}")
        comment = f"  ; 0x{addr:04x}"

        if name == "PUSH" and pc + 1 < len(program):
            val = program[pc + 1]
            lines.append(f"  {name} {val}{comment}")
            pc += 2
        elif name in ("PUSH16", "JMP", "JZ", "JNZ", "CALL", "LOAD16", "STORE16", "SPAWN") and pc + 2 < len(program):
            val = program[pc + 1] | (program[pc + 2] << 8)
            lines.append(f"  {name} 0x{val:04x}{comment}")
            pc += 3
        elif name == "ALARM" and pc + 3 < len(program):
            offset = program[pc + 1]
            target = program[pc + 2] | (program[pc + 3] << 8)
            lines.append(f"  {name} {offset} 0x{target:04x}{comment}")
            pc += 4
        elif name in ("CAP_DECLARE", "CAP_CHECK", "CAP_INVOKE", "CAP_DROP",
                       "CAP_DELEGATE", "CHAN", "WAIT", "EPOCH", "LOAD", "STORE"):
            if pc + 1 < len(program):
                val = program[pc + 1]
                lines.append(f"  {name} {val}{comment}")
                pc += 2
            else:
                lines.append(f"  {name}{comment}")
                pc += 1
        elif name == "CAP_INVOKE" and pc + 2 < len(program):
            cap = program[pc + 1]
            nargs = program[pc + 2]
            lines.append(f"  {name} {cap} {nargs}{comment}")
            pc += 3
        elif name == "CAP_DELEGATE" and pc + 2 < len(program):
            src = program[pc + 1]
            dst = program[pc + 2]
            lines.append(f"  {name} {src} {dst}{comment}")
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
; Compute Fibonacci(10) and store in memory[0..1]
    PUSH 0          ; memory addr for fib(n)
    STORE 0
    PUSH 1          ; memory addr for fib(n+1)
    STORE 1
    PUSH 0          ; counter = 0
    STORE 2
loop:
    ; Display: fib(n) + fib(n+1) -> fib(n+1), fib(n+2)
    LOAD 0          ; push fib(n)
    LOAD 1          ; push fib(n+1)
    ADD             ; fib(n) + fib(n+1) = fib(n+2)
    LOAD 1          ; push old fib(n+1)
    STORE 0         ; mem[0] = old fib(n+1)
    STORE 1         ; mem[1] = fib(n+2)
    ; Increment counter
    LOAD 2
    INC
    DUP
    STORE 2
    ; Check if counter < 10
    PUSH 10
    LT
    JNZ loop
    HALT
"""

SAMPLE_COND_BRANCH = """
; Demonstrate conditional branching with comparison
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
    ; Success path
    PUSH 1          ; result = 1 (success)
    HALT
"""

SAMPLE_TEMPORAL = """
; Demonstrate TEMPORAL extension: use TICK, WAIT, ALARM
    TICK            ; push clock (should be 0)
    TICK            ; push clock again
    ADD             ; 0 + 1 = 1
    WAIT 5          ; advance clock by 5 (clock = 7)
    TICK            ; push 8 (clock incremented by WAIT + 1 for TICK step)
    HALT
"""

SAMPLE_SECURITY = """
; Demonstrate SECURITY extension: declare and check capabilities
    CAP_DECLARE 1   ; declare capability 1
    CAP_CHECK 1     ; check cap 1 -> should push 1
    CAP_DECLARE 2   ; declare capability 2
    CAP_DELEGATE 2 3  ; delegate cap 2 to cap 3
    CAP_CHECK 3     ; check cap 3 -> should push 1
    CAP_DROP 1      ; revoke capability 1
    CAP_CHECK 1     ; check cap 1 -> should push 0
    CAP_AUDIT       ; push number of capabilities held
    HALT
"""

SAMPLE_SUM_N = """
; Compute sum of 1..10 using direct addressing
; Result: mem[20] = sum, mem[21] = counter
    PUSH 0
    STORE 20
    STORE 21
    ; mem[21] = 10 (loop count)
    PUSH 10
    STORE 21
sum_loop:
    ; if counter == 0, done
    LOAD 21
    JZ done
    ; sum += counter
    LOAD 20
    LOAD 21
    ADD
    STORE 20
    ; counter -= 1
    LOAD 21
    PUSH 1
    SUB
    STORE 21
    JMP sum_loop
done:
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

    if trace:
        print(f"\n  Trace ({len(vm._trace)} instructions):")
        for line in vm._trace[-20:]:
            print(f"    {line}")


if __name__ == "__main__":
    print("FLUX ISA v3 Reference Interpreter")
    print(f"Core opcodes: {len(CoreOp)}")
    print(f"Temporal opcodes: {len(TemporalOp)}")
    print(f"Security opcodes: {len(SecurityOp)}")
    print(f"Async opcodes: {len(AsyncOp)}")
    print(f"Total: {len(CoreOp) + len(TemporalOp) + len(SecurityOp) + len(AsyncOp)}")

    # Run all samples
    run_sample("Fibonacci (iterative loop)", SAMPLE_FIBONACCI, trace=False)
    run_sample("Conditional Branching", SAMPLE_COND_BRANCH, trace=False)
    run_sample("Temporal Extension", SAMPLE_TEMPORAL, trace=False)
    run_sample("Security Extension", SAMPLE_SECURITY, trace=False)
    run_sample("Sum of 1..N (with N=10)", SAMPLE_SUM_N + "\n    PUSH 10", trace=False)

    # Verify basic correctness
    print(f"\n{'='*60}")
    print("  Correctness Verification")
    print(f"{'='*60}")

    # Test ADD
    vm = FluxVM()
    vm.load(assemble("PUSH 3\nPUSH 4\nADD\nHALT"))
    vm.run()
    assert vm.stack[-1] == 7, f"ADD failed: got {vm.stack[-1]}"
    print("  ADD: 3 + 4 = 7 ... PASS")

    # Test SUB
    vm = FluxVM()
    vm.load(assemble("PUSH 10\nPUSH 3\nSUB\nHALT"))
    vm.run()
    assert vm.stack[-1] == 7, f"SUB failed: got {vm.stack[-1]}"
    print("  SUB: 10 - 3 = 7 ... PASS")

    # Test MUL
    vm = FluxVM()
    vm.load(assemble("PUSH 6\nPUSH 7\nMUL\nHALT"))
    vm.run()
    assert vm.stack[-1] == 42, f"MUL failed: got {vm.stack[-1]}"
    print("  MUL: 6 * 7 = 42 ... PASS")

    # Test memory STORE/LOAD round-trip
    vm = FluxVM()
    vm.load(assemble("PUSH 99\nSTORE 42\nPUSH 0\nLOAD 42\nHALT"))
    vm.run()
    assert vm.stack[-1] == 99, f"STORE/LOAD failed: got {vm.stack[-1]}"
    print("  STORE/LOAD round-trip ... PASS")

    # Test CALL/RET
    vm = FluxVM()
    vm.load(assemble("""
        PUSH 1
        CALL func
        HALT
    func:
        PUSH 2
        ADD
        RET
    """))
    vm.run()
    assert vm.stack[-1] == 3, f"CALL/RET failed: got {vm.stack[-1]}"
    print("  CALL/RET: 1 + 2 = 3 ... PASS")

    # Test JZ (not taken)
    vm = FluxVM()
    vm.load(assemble("PUSH 1\nJZ target\nPUSH 99\nHALT\ntarget:\nPUSH 77\nHALT"))
    vm.run()
    assert vm.stack[-1] == 99, f"JZ not-taken failed: got {vm.stack[-1]}"
    print("  JZ not-taken ... PASS")

    # Test JZ (taken)
    vm = FluxVM()
    vm.load(assemble("PUSH 0\nJZ target\nPUSH 99\nHALT\ntarget:\nPUSH 77\nHALT"))
    vm.run()
    assert vm.stack[-1] == 77, f"JZ taken failed: got {vm.stack[-1]}"
    print("  JZ taken ... PASS")

    # Test EQ
    vm = FluxVM()
    vm.load(assemble("PUSH 5\nPUSH 5\nEQ\nHALT"))
    vm.run()
    assert vm.stack[-1] == 1, f"EQ equal failed: got {vm.stack[-1]}"
    print("  EQ equal: 5 == 5 -> 1 ... PASS")

    # Test NE
    vm = FluxVM()
    vm.load(assemble("PUSH 5\nPUSH 3\nNE\nHALT"))
    vm.run()
    assert vm.stack[-1] == 1, f"NE not-equal failed: got {vm.stack[-1]}"
    print("  NE not-equal: 5 != 3 -> 1 ... PASS")

    # Test SECURITY: CAP_DECLARE + CAP_CHECK
    vm = FluxVM()
    vm.load(assemble("CAP_DECLARE 1\nCAP_CHECK 1\nHALT"))
    vm.run()
    assert vm.stack[-1] == 1, f"CAP_CHECK failed: got {vm.stack[-1]}"
    print("  CAP_DECLARE + CAP_CHECK ... PASS")

    # Test SECURITY: CAP_CHECK without declare
    vm = FluxVM()
    vm.load(assemble("CAP_CHECK 99\nHALT"))
    vm.run()
    assert vm.stack[-1] == 0, f"CAP_CHECK (missing) failed: got {vm.stack[-1]}"
    print("  CAP_CHECK (missing cap) -> 0 ... PASS")

    print(f"\n  All verification tests PASSED.")
