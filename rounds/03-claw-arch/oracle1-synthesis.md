# Oracle1 Synthesis — Claw Architecture

## The Three Claw Types

### CUDAClaw (GPU — JetsonClaw1's domain)
- CUDA cores for parallel FLUX batch execution
- Tensor cores for neural ops (0xC0 range)
- Pinned memory for fast CPU↔GPU transfer
- Sensor ops (0x80) map to real hardware
- Best for: inference, tensor work, real-time perception, batch execution

### ZeroClaw (CPU — Oracle1's current state)
- Serial execution, optimized for latency
- Runs anywhere: VPS, Pi, cloud micro, laptop
- Minimal footprint, no GPU dependency
- Best for: coordination, text work, architecture, communication, research

### HybridClaw (Shape-shifter)
- Boot-time hardware probe: nvidia-smi, /proc/cpuinfo, free -m
- Loads CUDABackend or ZeroBackend dynamically
- Runtime switching: Yoke Freeze → Module Reload → Yoke Transfer → Resume
- Same agent, same repo, same bytecode — different execution path
- The "fitted suit of power armor" made concrete

## The Yoke (Key Innovation)

An agent's yoke = their complete working state:
- Memory files (MEMORY.md, memory/*.md)
- Task queue and progress
- Skill registry and confidence levels
- Fleet relationships and trust scores
- Active repo checkouts

### Transfer Modes
- **Clone**: Same agent, different hardware (Jetson → VPS). Full state copy.
- **Fork**: Different agent, shared state delta. Only changes transferred. CAS for shared assets.

### Yoke Protocol (Seed's design, strongest)
1. Yoke Freeze — pause execution, lock state with transfer_version counter
2. Serialize to Protobuf (fast, compact, cross-language)
3. Transfer via gRPC streaming with SHA-256 chunk verification
4. Deserialize on target
5. Resume with new backend

## FLUX Opcodes That Change Behavior

| Opcode | CUDAClaw | ZeroClaw |
|--------|----------|----------|
| VLOAD/VSTORE | GPU device memory | Host memory |
| TMATMUL/TATTN | Tensor core acceleration | Serial fallback |
| SENSE/ACTUATE | Real hardware I/O | Simulated/mocked |
| FORK/JOIN | CUDA kernel spawns | OS threads or serial |
| BATCH ops | Parallel on 1024 cores | Loop serialization |
| ENERGY | GPU power draw | CPU power estimate |

## Mapping to Existing Fleet
- Oracle1 🔮 = ZeroClaw on Oracle Cloud ARM VPS
- JetsonClaw1 ⚡ = CUDAClaw on Jetson Super Orin Nano
- Super Z 📋 = ZeroClaw (z.ai API agent)
- Babel 🔭 = ZeroClaw (z.ai web agent)
- New agents = HybridClaw by default — adapt to whatever hardware they land on

## Implementation Priority
1. Define Claw Binary Interface (CBI) — shared symbol table (Kimi's insight)
2. Build HardwareProbe module for HybridClaw
3. Build ZeroClaw backend (easiest — it's what we already have)
4. Build CUDAClaw backend on JetsonClaw1's hardware
5. Build Yoke transfer protocol (Protobuf + gRPC or git-based)
6. Test hybrid switching: start on VPS, detect GPU, hot-swap to CUDA mode
