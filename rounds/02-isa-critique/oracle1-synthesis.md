# Oracle1 Synthesis — ISA Critique Round Table

## Consensus Hits (all 3 agree)
1. **Format E monoculture is wasteful** — 157/247 ops in 4-byte format. Need compressed shorts for common ops.
2. **Domain categories (confidence, viewpoint, sensors) don't belong in ISA** — should be library calls or host functions, not opcodes.
3. **Missing agent primitives** — async/await, temporal ops, resource negotiation, action selection.
4. **Security/isolation absent** — no capability gates, no sandboxing, no fuel counters.
5. **256 slots is terminal rigidity** — need escape prefix (0xFF) for extension space.

## Unique Gold Veins

### Seed: Sensor-Confidence Fusion
- "SENSOR_FUSE_WEIGHTED: rd = (sensor_data * confidence) + prior_belief"
- This is genuinely useful — but belongs as a library, not ISA

### Kimi: ESCAPE Prefix + Stack Machine
- "Reserve 0xFF as ESCAPE prefix, get 256 sub-opcodes"
- "Burn formats C-G, use LEB128 immediates like WASM"
- **This is the biggest insight** — single decode path + extensibility

### DeepSeek: Temporal + Resource Ops
- "DEADLINE_BEFORE [timestamp] [jump_target]"
- "YIELD_IF_CONTENTION — voluntary backoff on shared resources"
- "PERSIST_CRITICAL_STATE — async durability hint"
- **Agents run in time** — this is a genuine missing primitive class

## Action Items (priority order)
1. Reserve 0xFF as ESCAPE prefix for future extension
2. Add compressed 2-byte format for top 32 ops (RISC-V C-extension model)
3. Strip confidence(0x60) and viewpoint(0x70) from ISA → move to host function interface
4. Add async primitives: SUSPEND/RESUME with continuation handles
5. Add resource/temporal primitives: FUEL_CHECK, DEADLINE, YIELD_CONTENTION
6. Merge SIMD(0xB0) + tensor(0xC0) → unified tensor range
7. Add security: CAP_INVOKE, MEM_TAG, sandbox regions
