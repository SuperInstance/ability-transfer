# CHANGELOG.md — Ability Transfer Simulation Lab

All notable changes to the ability-transfer simulation lab are documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

---

## [0.4.0] - 2026-04-14

### Added
- **README.md** — Major expansion with 10 new sections:
  - Architecture Overview with ASCII pipeline diagram
  - Theoretical Framework (ability-vs-skill distinction with concrete examples)
  - Simulation Methodology (multi-model approach documentation)
  - Round Summaries (detailed tables for rounds 1-3 with model-by-model contributions)
  - Key Insights Discovered (6 major findings across all rounds)
  - Connection to FLUX ISA (path from simulation to ISA v3 specification)
  - Practical Applications for Fleet Agents
  - Findings Catalog (30+ findings organized by type with source references)
  - Updated Principles (expanded from 4 to 10)
  - Repository Structure documentation
- **FINDINGS.md** — Comprehensive synthesis document with 6 sections:
  - Section 1: On the Nature of Abilities (6 philosophical findings)
  - Section 2: ISA Design Principles (7 technical findings with critic incorporation matrix)
  - Section 3: The Claws Architecture (5 architecture findings with opcode behavior matrix)
  - Section 4: Convergence Points (5 high-confidence multi-model agreements)
  - Section 5: Divergence Points (4 disagreements with resolution analysis)
  - Section 6: Recommendations (10 actionable next steps across 3 time horizons)
- **METHODOLOGY.md** — Full simulation methodology documentation:
  - Why multi-model simulation (cognitive diversity argument)
  - Model selection criteria (4 models with cognitive style analysis)
  - Round structure and lifecycle (5 phases)
  - Handoff protocol (information flow rules)
  - Quality criteria (5 evaluation dimensions with red flags)
  - Synthesis methodology (how to merge diverse perspectives)
  - Limitations and caveats
- **CONTRIBUTING.md** — Contribution guide with:
  - How to add a new simulation round (setup checklist)
  - Model selection decision matrix
  - Round output format requirements
  - Research question proposal process
  - Review process (self-review, cross-model, post-commit)
  - Git workflow and conflict resolution
- **CHANGELOG.md** — This file

### Changed
- README.md simulation rounds table updated to reflect actual completion status (rounds 1-3 complete, not just round 1)
- README.md status changed from "Round 1 in progress" to "Rounds 1-3 complete"
- README.md principles expanded from 4 to 10

---

## [0.3.0] - 2026-04-13

### Added
- **Round 3a: Claw Architecture** (`rounds/03-claw-arch/`)
  - `deepseek.md` — Claw Binary Interface (CBI), CUDAClaw/ZeroClaw implementations, YokeSync v1 protocol, opcode behavior matrix
  - `kimi.md` — Module system with abstract factory, hardware detection, adaptive scheduler, fleet deployment strategies
  - `seed.md` — Module manifest system, HybridClaw detection, Protobuf yoke structure, FleetDirectory gRPC service
  - `oracle1-synthesis.md` — Three claw types defined, fleet mapping, yoke innovation documented, implementation priority set
- **Round 3b: ISA v3 Draft** (`rounds/03-isa-v3-draft/`)
  - `isa-v3-draft.md` — Full FLUX ISA v3 specification (724 lines):
    - 0xFF escape prefix (65,280 extension slots)
    - 32 compressed short-form opcodes (25-35% code size reduction)
    - TEMPORAL extension (6 opcodes: FUEL_CHECK, DEADLINE_BEFORE, YIELD_IF_CONTENTION, PERSIST_CRITICAL_STATE, TIME_NOW, SLEEP_UNTIL)
    - SECURITY extension (6 opcodes: CAP_INVOKE, MEM_TAG, SANDBOX_ENTER/EXIT, FUEL_SET, IDENTITY_GET)
    - ASYNC extension (6 opcodes: SUSPEND, RESUME, FORK, JOIN, CANCEL, AWAIT_CHANNEL)
    - Category restructuring (Sensors, Viewpoint, Collections, Debug moved to extensions)
    - Full backward compatibility with v2
    - Round table critic incorporation matrix (19/22 recommendations implemented)
    - Conformance test vectors for all extensions
    - Migration guide for runtime implementors, agent developers, and fleet operators

### Key Outcomes
- ISA v3 transforms FLUX from a CPU emulator into an agent runtime
- Three claw types (CUDAClaw/ZeroClaw/HybridClaw) enable cross-hardware agent execution
- Yoke transfer protocol enables sub-second state migration between hardware types
- 19 of 22 critic recommendations incorporated; 3 deferred to future extensions

---

## [0.2.0] - 2026-04-12

### Added
- **Round 2a: ISA Critique** (`rounds/02-isa-critique/`)
  - `deepseek.md` — Category bloat, format imbalance, missing agent primitives, security gaps, resource management blindness
  - `kimi.md` — 247/256 trap, format zoo, category budget delusion, zero security, "complexity bomb" verdict
  - `seed.md` — Format E overuse, legacy alias bloat, sensor-confidence fusion proposal, semantic jumps, SIMD/tensor merge
  - `oracle1-synthesis.md` — 5 consensus hits, unique gold veins per model, 7 prioritized action items
- **Round 2b: Ability Synthesis** (`rounds/02-synthesis/`)
  - `deepseek-reasoner.md` — Constraint Resonance Pattern (atomic unit), harmonic/discordant composition, forge repo structure with scoring and failure modes
  - `deepseek-v3.md` — Attentional Lens + Constraint Response Pattern, Dojo Matrix, verification as "scars in operation"
  - `oracle1-synthesis.md` — Converged both DeepSeek models, buildable forge repo structure, 5 verification methods, 5 failure modes

### Key Outcomes
- All 4 models independently converged on 5 critical ISA flaws (strongest validation of multi-model approach)
- Constraint Resonance Pattern established as the atomic unit of ability
- Forge repo structure designed with 4-stage exercises and scoring
- Kimi's 0xFF escape prefix identified as biggest single insight
- Ability composition defined as lattice (not stack) — interference without mediation

---

## [0.1.0] - 2026-04-11

### Added
- **Round 1: Foundations** (`rounds/01-kimi/`, `rounds/01-oracle1/`)
  - `philosophy-of-ability.md` — Kimi's philosophical analysis: ability as "clearing," the Gesture of Discrimination, Structural Isomorphism for transfer, the Socratic elenchus as mechanism, repo as dojo
  - `grounding.md` — Oracle1's fleet grounding: Code Archaeologist, Hardware Whisperer, Cross-Language Perceiver, Quality Auditor abilities mapped to fleet agents
- **README.md** — Initial documentation with core question, forge metaphor, simulation round table, 4 emerging principles

### Key Outcomes
- Philosophical primitive identified: ability is pre-cognitive resonance, not possessed knowledge
- Forge metaphor established: Heat → Hammer → Quench → Temper = Cut → Crisis → Navigate → Mirror
- Fleet abilities grounded: 4 concrete ability types identified in existing fleet
- Repo-as-dojo insight: the repository structure IS the transfer mechanism

---

## [0.0.0] - 2026-04-11

### Added
- Repository initialization
- Initial README.md with core research question
- Directory structure for simulation rounds
