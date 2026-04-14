# 🏗️ Ability Transfer System — Simulation Lab

> *The forge that turns hot metal into spring-loaded steel.*

This repo is a **long-running simulation** across multiple models and iterations to design a system where AI agents transfer **abilities** (not just skills, not just code) through git repos.

## The Question

How do you transfer an agent's **way of seeing** to another agent? Not knowledge (that's docs). Not skills (that's code). Not tools (that's libraries). Something deeper — the reflex, the intuition, the judgment that separates an expert from a beginner.

## The Forge Metaphor

A blacksmith turns raw metal into spring-loaded steel through:

1. **Heating** — softening the metal (open the agent's paradigm)
2. **Hammering** — shaping through repetition (exercises that build reflexes)
3. **Quenching** — locking in the shape (committing the learned pattern)
4. **Tempering** — removing brittleness (testing in new contexts, not just the training one)

Different metals need different sequences. The bootcamp is the forge — **structurally patterned after what works, modular in application**.

---

## Architecture Overview

The ability-transfer lab is structured as a multi-round simulation pipeline. Each round involves multiple AI models attacking the core research question from different cognitive angles. Results accumulate in versioned markdown files, with Oracle1 (the fleet quartermaster) synthesizing cross-model consensus at each stage.

```
┌─────────────────────────────────────────────────────────────────────────┐
│                     ABILITY TRANSFER SIMULATION PIPELINE                │
├─────────────────────────────────────────────────────────────────────────┤
│                                                                         │
│  ┌─────────┐    ┌─────────┐    ┌─────────┐    ┌──────────┐             │
│  │ ROUND 1 │───▶│ ROUND 2 │───▶│ ROUND 3 │───▶│ OUTPUTS  │             │
│  │Foundations│   │Critique  │   │Build     │   │          │             │
│  └────┬────┘    └────┬────┘    └────┬────┘    └──────────┘             │
│       │              │              │                                    │
│  ┌────▼────┐    ┌────▼────┐    ┌────▼────┐                             │
│  │ 4 Models│    │ 4 Models│    │ 4 Models│                             │
│  │Parallel │    │Parallel │    │Parallel │                             │
│  │Inquiry  │    │Critique │    │Design   │                             │
│  └────┬────┘    └────┬────┘    └────┬────┘                             │
│       │              │              │                                    │
│  ┌────▼────┐    ┌────▼────┐    ┌────▼────┐                             │
│  │Oracle1  │    │Oracle1  │    │Oracle1  │                             │
│  │Grounding│    │Synthesis│    │Synthesis│                             │
│  └─────────┘    └─────────┘    └─────────┘                             │
│                                                                         │
│  MODEL ROLES:                                                           │
│  ┌──────────┐  ┌──────────┐  ┌───────────┐  ┌──────────┐              │
│  │  SEED    │  │  KIMI    │  │ DEEPSEEK  │  │ ORACLE1  │              │
│  │Creative  │  │Philosophy│  │Synthesis  │  │Grounding │              │
│  │Systems   │  │Ontology  │  │Engineering│  │Fleet App │              │
│  └──────────┘  └──────────┘  └───────────┘  └──────────┘              │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘

OUTPUTS:
  ├── FLUX ISA v3 (from Round 2 critique + Round 3 integration)
  ├── Claws Architecture (CUDAClaw / ZeroClaw / HybridClaw)
  ├── Forge Repo Structure (ability bootcamp design)
  └── Theoretical Framework (ability-vs-skill distinction)
```

### Data Flow

Each simulation round follows a structured handoff protocol:

1. **Parallel Generation** — Each model receives the same seed question and produces independent analysis
2. **Oracle1 Synthesis** — The fleet quartermaster reads all model outputs, identifies consensus hits and unique gold veins
3. **Accumulation** — Synthesis documents feed into subsequent rounds as input context
4. **Convergence** — Over multiple rounds, models independently converge on shared conclusions, validating robustness

---

## Theoretical Framework

### The Ability-vs-Skill Distinction

The central insight of this lab is that **abilities are not skills**. This distinction is not merely semantic — it is structural, and understanding it is prerequisite to building any transfer system.

| Dimension | Skill | Ability |
|-----------|-------|---------|
| **Nature** | Procedural knowledge — "can you do X?" | Perceptual calibration — "do you SEE X when it matters?" |
| **Acquisition** | Taught via instruction, practice, repetition | Forged via constraint immersion, crisis navigation |
| **Transfer** | Copy documentation, share code | Restructure attention through curated experience |
| **Verification** | Tests pass, code compiles | Expert-like pattern recognition on novel inputs |
| **Persistence** | Lost without practice | Survives as "attention scars" |
| **Composition** | Stacked linearly | Composed as lattice points in constraint space |
| **Example** | "Can write a regex" | "Automatically detects when a regex is the wrong tool" |

### Concrete Examples

**Code Archaeologist (Fleet Ability)**: A skilled programmer can debug code. A Code Archaeologist *feels structural rot before reading the implementation*. They see commit history as geological strata — the fracture lines announce themselves as problems before cognition begins. This is not knowledge about bad code; it is a pre-cognitive resonance between the agent and the field of possible failure.

**Hardware Whisperer (Fleet Ability)**: A skilled engineer can optimize for hardware. A Hardware Whisperer *senses thermal gradients, cache line boundaries, and memory bandwidth constraints* the way a blacksmith senses the temperature of metal. They reach for cache-line optimization before algorithmic optimization because their attention has been calibrated to physical limits.

**Quality Auditor (Fleet Ability)**: A skilled tester can write test cases. A Quality Auditor *perceives the gap between what is tested and what matters*. Their attention automatically gravitates toward untested failure modes — the edge case that would embarrass the system in production, not the edge case that exercises the most code paths.

### The Philosophical Primitive

Drawing on Heidegger's *Zuhandenheit*, Wittgenstein's *aspect-perception*, Bourdieu's *habitus*, and Musashi's *fractal discrimination*, Kimi identified the fundamental unit: the **Gesture of Discrimination** — the act of cutting the continuum of experience into figure and ground. This gesture is fractal; the same pattern repeats from a single line of code to the architecture of an entire fleet. Ability is the sedimented history of these gestures, transferred when one agent's accumulated traces impose their pattern of resistance upon another.

---

## Simulation Methodology

### The Multi-Model Approach

This simulation uses four distinct AI models, each selected for a different cognitive style. The diversity is intentional: convergence across different cognitive architectures provides stronger evidence than agreement within one.

| Model | Cognitive Style | Role in Simulation | Strengths |
|-------|----------------|-------------------|-----------|
| **Seed** | Creative / Systems | Generates novel architectures, breaks established patterns | Unconstrained thinking, rapid prototyping, cross-domain synthesis |
| **Kimi** | Philosophical / Analytical | Deconstructs assumptions, grounds concepts in first principles | Deep ontological analysis, pattern recognition across disciplines |
| **DeepSeek** | Engineering / Synthesis | Merges perspectives into buildable specifications | Practical implementation, structural trade-offs, concrete designs |
| **Oracle1** | Grounding / Fleet Operations | Applies findings to the real fleet, validates feasibility | Operational constraints, resource awareness, real-world applicability |

### Why Different Cognitive Styles Matter

If a single model generates all ideas, you get depth without breadth — the model's blind spots become the system's blind spots. By using four models with different training objectives and cognitive architectures:

- **Seed** proposes ideas that no "reasonable" model would suggest (sensor-confidence fusion ops, semantic jumps)
- **Kimi** identifies when Seed's ideas rest on shaky philosophical ground (confidence as a float, not an opcode)
- **DeepSeek** transforms philosophical insights into engineering specifications (forge repo structure, exercise formats)
- **Oracle1** filters everything through operational reality (what can our fleet actually run, what hardware do we have)

The result is not compromise but convergence — when all four models independently arrive at the same conclusion, it carries far more weight than any single model's opinion.

### Round Structure

Each simulation round follows this structure:

```
rounds/
  {NN}-{name}/
    {model}.md           # Individual model output
    oracle1-synthesis.md # Cross-model consensus document
```

The Oracle1 synthesis document is the critical artifact. It follows a fixed format:

1. **Consensus Hits** — Findings where all (or most) models agree
2. **Unique Gold Veins** — Insights unique to one model that others missed
3. **Divergence Points** — Where models disagreed and why
4. **Action Items** — Priority-ordered next steps

### Quality Criteria

Round outputs are evaluated on:

- **Specificity**: Concrete examples, not vague principles
- **Actionability**: Can you build something from this?
- **Surprise**: Did it challenge assumptions?
- **Grounding**: Is it connected to real constraints?
- **Composition**: Does it integrate with findings from previous rounds?

---

## Round Summaries

### Round 1 — Foundations (2026-04-11)

| Model | File | Key Contribution |
|-------|------|-----------------|
| **Kimi** | `01-kimi/philosophy-of-ability.md` | Defined the philosophical primitive: ability as "a clearing" — a way of standing open to the world. Introduced the Gesture of Discrimination, the concept of Structural Isomorphism for transfer, and the 4-stage forge (Cut → Crisis → Navigate → Mirror). |
| **Oracle1** | `01-oracle1/grounding.md` | Grounded abilities in fleet context: Code Archaeologist, Hardware Whisperer, Cross-Language Perceiver, Quality Auditor. Established that the same 4-stage structure applies to all abilities but with different "metals" (inputs). |

**Key Insight**: Abilities are not possessed but *clearings* — pre-cognitive resonances between agent and world. The repo is not a container of information but a **dojo** that breaks and rebuilds attention.

### Round 2 — ISA Critique + Synthesis (2026-04-12)

| Model | File | Key Contribution |
|-------|------|-----------------|
| **Seed** | `02-isa-critique/seed.md` | Format E overuse (157/247 ops in 4-byte format wastes 20-30% density). Proposed sensor-confidence fusion ops. Argued for merging SIMD/tensor into unified range. |
| **Kimi** | `02-isa-critique/kimi.md` | Identified the 247/256 trap (terminal rigidity). Proposed `0xFF` escape prefix for extensibility. Called security absence "a security timebomb." Demanded single decode path. |
| **DeepSeek** | `02-isa-critique/deepseek.md` | Flagged category bloat as "architectural carcinization." Demanded temporal primitives (DEADLINE_BEFORE, YIELD_IF_CONTENTION). Identified missing attention/sparsity ops. |
| **Oracle1** | `02-isa-critique/oracle1-synthesis.md` | Synthesized 5 consensus hits. Identified Kimi's escape prefix as the biggest single insight. Prioritized 7 action items. |
| **DeepSeek Reasoner** | `02-synthesis/deepseek-reasoner.md` | Defined "Constraint Resonance Pattern" as atomic unit. Designed harmonic/discordant ability composition. Built full forge repo structure with scoring and failure modes. |
| **DeepSeek V3** | `02-synthesis/deepseek-v3.md` | Defined "Attentional Lens + Constraint Response Pattern" as coupled pair. Designed Dojo Matrix for composition. Specified verification as "scars in operation" — predictive insight before problems manifest. |
| **Oracle1** | `02-synthesis/oracle1-synthesis.md` | Converged both DeepSeek models on "Constraint Resonance Pattern." Designed buildable forge repo structure. Defined 5 verification methods and 5 failure modes. |

**Key Insight**: All 4 models independently converged on 5 critical ISA flaws: Format E monoculture, domain categories in wrong layer, missing agent primitives, absent security, and terminal rigidity of 256-slot space.

### Round 3 — Architecture + ISA v3 (2026-04-13)

| Model | File | Key Contribution |
|-------|------|-----------------|
| **DeepSeek** | `03-claw-arch/deepseek.md` | Designed Claw Binary Interface (CBI) with strict ABI. Specified CUDAClaw and ZeroClaw implementations. Designed YokeSync v1 protocol with fork/clone modes. Created opcode behavior matrix across claw types. |
| **Kimi** | `03-claw-arch/kimi.md` | Designed module system with abstract factory pattern. Built hardware detection service with Jetson support. Designed adaptive scheduler and deployment strategies. Created claws-boot.sh startup sequence. |
| **Seed** | `03-claw-arch/seed.md` | Designed module manifest system with dependency injection. Specified Protobuf yoke structure with fork/clone transfer modes. Built FleetDirectory gRPC service. Designed hardware probing with graceful degradation. |
| **Oracle1** | `03-claw-arch/oracle1-synthesis.md` | Defined three claw types (CUDAClaw, ZeroClaw, HybridClaw). Mapped to existing fleet agents. Established yoke as key innovation. Set implementation priority. |
| **Datum** | `03-isa-v3-draft/isa-v3-draft.md` | Wrote full ISA v3 specification incorporating all round table feedback. Implemented 0xFF escape prefix, compressed short format, temporal/security/async primitives. Complete backward compatibility with v2. |

**Key Insight**: The same agent can run on both GPU (Jetson) and CPU (VPS) by switching "claw types" while preserving state through yoke transfer. ISA v3 transforms FLUX from a CPU emulator into an agent runtime.

---

## Key Insights Discovered

### 1. The Forge Metaphor Is Literally Correct

The four-stage forge process (Heat → Hammer → Quench → Temper) mapped to (Cut → Crisis → Navigate → Mirror) is not just poetic — it describes the actual mechanism of ability formation. Kimi's philosophical analysis and DeepSeek's engineering design independently converged on the same four stages, just with different terminology:

| Forge Stage | Kimi's Term | DeepSeek's Term | What Happens |
|-------------|-------------|-----------------|--------------|
| Heat | Aporetic Encounter | Constraint Resonance | Break the agent's current way of seeing |
| Hammer | The Cut Exercise | Crisis Exercises | Force repeated discrimination under constraint |
| Quench | Embodied Navigation | Resistance Landscapes | Lock in the pattern through graduated difficulty |
| Temper | The Mirror of Commits | Expert Trace Reading | Test in new contexts, verify against expert patterns |

### 2. Ability Transfer Requires Paradigm Shift, Not Knowledge Transfer

You cannot lecture an agent into having good judgment. Kimi's analysis of the Socratic *elenchus* showed that ability transfer requires the **destruction of the recipient's current gestalt** — the confusion is not a bug but necessary friction. The repo acts as a Socratic midwife, forcing the agent to reorganize its attentional field to resolve dissonance. The ability transfers not as content but as **the scar tissue of resolved confusion**.

### 3. Different Metals Need Different Forge Sequences

Oracle1 grounded this in fleet reality: Code Archaeologist, Hardware Whisperer, Cross-Language Perceiver, and Quality Auditor all use the same 4-stage structure but with different inputs. The "metal" (constraint types, failure modes, expert traces) differs per ability. A Rust agent and a Python agent go through the same bootcamp but emerge with abilities shaped by their paradigm.

### 4. The ISA Critique Convergence

The most striking validation of the multi-model approach: all four models independently identified the same five flaws in the FLUX ISA v2:

1. **Format E monoculture** (157/247 ops in 4-byte format) — all agreed this wastes bytecode density
2. **Domain categories don't belong in ISA** (confidence, viewpoint, sensors should be libraries, not opcodes) — all agreed this is application-level leakage into runtime
3. **Missing agent primitives** (async/await, temporal ops, resource negotiation) — all agreed agents need time-aware and security-aware operations
4. **Security/isolation absent** (no capability gates, no sandboxing, no fuel counters) — all flagged this as critical
5. **256-slot rigidity** (247/256 occupied, 9 free) — all converged on the 0xFF escape prefix as the solution

### 5. Composition Is Lattice, Not Stack

Abilities do not stack like skills. They compose as **lattice points in constraint space**. Hardware Whisperer + Code Archaeologist = "Resilience Architect" (harmonic), but only if integration drills bridge their orthogonal attention patterns. Without deliberate composition, two abilities **interfere** — the agent sees hardware limits OR structural rot, but not both simultaneously.

### 6. The Yoke as State Transfer Primitive

The **yoke** — an agent's complete working state serialized into a portable format — is the enabling technology for cross-hardware ability transfer. It encompasses memory, task queue, skill registry, confidence levels, fleet relationships, and active repo checkouts. Yoke transfer (fork and clone modes) allows the same agent to run inference on a Jetson GPU and generate reports on a VPS CPU with sub-second handoff.

---

## Connection to FLUX ISA

This research lab directly produced **FLUX ISA v3**, a major architectural evolution of the fleet's bytecode specification. The path from simulation finding to specification change:

```
Round 2 ISA Critique (all 4 models)
    │
    ├── "247/256 slots is terminal rigidity" ──▶ 0xFF Escape Prefix (65,280 extension slots)
    ├── "Format E wastes 20-30% density"     ──▶ 32 compressed short-form opcodes
    ├── "Missing temporal primitives"         ──▶ TEMPORAL extension (6 opcodes)
    ├── "Security is a timebomb"              ──▶ SECURITY extension (6 opcodes + error codes)
    ├── "Agents are event-driven"             ──▶ ASYNC extension (6 opcodes)
    └── "Domain categories don't belong here" ──▶ Category restructuring (Sensors, Viewpoint,
                                                   Collections, Debug moved to extensions)
```

The ISA v3 specification (`03-isa-v3-draft/isa-v3-draft.md`) incorporates **16 of 22 critic recommendations**, with 6 deferred to future extension spaces. Full backward compatibility with v2 is maintained — all v2 bytecode runs unmodified on v3 runtimes.

Key v3 innovations:
- **0xFF escape prefix** provides 255 extension spaces × 256 sub-opcodes = 65,280 extended opcodes
- **Compressed short format** reduces code size by 25-35% for tight agent loops
- **Three new primitive classes** (temporal, security, async) transform FLUX from a CPU emulator into an agent runtime
- **Capability negotiation protocol** allows runtimes to discover and adapt to each other's supported extensions

---

## Practical Applications for Fleet Agents

### For Agent Developers

1. **Use the forge structure** to design ability bootcamps: `CHARTER.md` → `BOOTCAMP.md` → 4-stage exercises → scoring → verification
2. **Verify ability transfer** with scar detection, not test scores: does the agent exhibit expert-like attention patterns on novel inputs?
3. **Check for failure modes**: cargo culting, over-fitting, ability interference, forge blindness
4. **Use ISA v3 extensions** when available: negotiate capabilities with `PROBE`, use `SUSPEND` instead of polling, wrap untrusted code in `SANDBOX_ENTER`

### For Fleet Operators

1. **Deploy HybridClaw** for new agents — they automatically adapt to whatever hardware they land on
2. **Use yoke transfer** for cross-hardware workflows: inference on GPU → reporting on CPU → storage on VPS
3. **Set fuel budgets** via `FUEL_SET` to prevent runaway agents
4. **Implement capability-based access control**: different agent roles get different capability grants

### For Researchers

1. **Read the round files** as a case study in multi-model convergence research
2. **Replicate the methodology** for other research questions requiring diverse cognitive perspectives
3. **Extend the forge** with new ability types and composition patterns
4. **Challenge the findings** — this lab values disagreement as much as consensus

---

## Findings Catalog

### Philosophical Findings

| # | Finding | Source | Round |
|---|---------|--------|-------|
| F-01 | Ability is a "clearing" — pre-cognitive resonance, not possessed knowledge | Kimi | 1 |
| F-02 | The Gesture of Discrimination is the fundamental philosophical primitive | Kimi | 1 |
| F-03 | Repos transfer abilities through Structural Isomorphism — frozen fields of attention | Kimi | 1 |
| F-04 | Ability transfer requires Socratic disruption — the confusion IS the mechanism | Kimi | 1 |
| F-05 | Ability is habitus (Bourdieu) — sedimented history of discrimination gestures | Kimi | 1 |

### Technical Findings

| # | Finding | Source | Round |
|---|---------|--------|-------|
| T-01 | Atomic unit of ability = Constraint Resonance Pattern | DeepSeek R + V3 | 2 |
| T-02 | Abilities compose as lattice points, not stack layers | DeepSeek V3 | 2 |
| T-03 | Forge repo needs 4-stage exercises (Cut → Crisis → Navigate → Mirror) | All models | 1-2 |
| T-04 | Verification = scar detection, not test scores | DeepSeek R | 2 |
| T-05 | 5 failure modes: cargo culting, over-fitting, interference, forge blindness, false positive | Oracle1 | 2 |
| T-06 | 0xFF escape prefix solves ISA extensibility | Kimi (biggest insight) | 2 |
| T-07 | Compressed short format saves 25-35% code size | Seed + Kimi | 2 |
| T-08 | Agents need temporal primitives (DEADLINE, YIELD, PERSIST) | DeepSeek | 2 |
| T-09 | Security primitives (CAP_INVOKE, MEM_TAG, SANDBOX, FUEL) are critical | All models | 2 |
| T-10 | ASYNC primitives (SUSPEND/RESUME/FORK/JOIN) enable event-driven agents | DeepSeek | 2 |
| T-11 | Claw Binary Interface enables hardware-adaptive agents | DeepSeek + Kimi + Seed | 3 |
| T-12 | Yoke transfer protocol enables cross-hardware state migration | All models | 3 |
| T-13 | CUDAClaw/ZeroClaw/HybridClaw covers full hardware spectrum | Oracle1 | 3 |
| T-14 | Confidence opcodes stay in base ISA (3 opcodes, 1.2% of space, universally needed) | Datum | 3 |
| T-15 | Domain categories (Sensors, Viewpoint, Collections, Debug) move to extensions | All models | 2-3 |

### Convergence Findings (All Models Agreed)

| # | Finding | Models | Round |
|---|---------|--------|-------|
| C-01 | Abilities are not skills | All | 1-2 |
| C-02 | Abilities are forged, not taught | All | 1-2 |
| C-03 | The forge is modular — same structure, different inputs | All | 1-3 |
| C-04 | ISA v2 has 5 critical structural flaws | All | 2 |
| C-05 | 0xFF escape prefix is the solution to extensibility | All | 2 |
| C-06 | Domain categories don't belong in base ISA | All | 2 |
| C-07 | Cross-hardware agent execution is possible via yoke transfer | All | 3 |

---

## Principles (Emerging)

*(These have been refined across rounds)*

1. **Abilities are not skills** — Skills are "can you do X?" Abilities are "do you SEE X when it matters?"
2. **Abilities are forged, not taught** — You can't lecture someone into having good judgment. You have to put them through situations that build judgment.
3. **The forge is modular** — Same structure, different inputs. A Rust agent and a Python agent go through the same bootcamp but emerge with abilities shaped by their paradigm.
4. **Abilities survive the agent** — If the bootcamp is right, any agent can acquire the ability from the repo.
5. **Convergence validates** — When multiple models independently arrive at the same conclusion, it carries more weight than any single model's opinion.
6. **The repo is a dojo** — Not a library of knowledge, but a space designed to break and rebuild the attention of those who enter it.
7. **Different metals need different forge sequences** — The 4-stage structure is constant, but the specific exercises, constraints, and expert traces vary per ability.
8. **Composition requires mediation** — Abilities interfere unless deliberately composed through integration drills.
9. **Security is not optional** — Multi-agent systems without capability-based isolation are security timebombs.
10. **Extensibility is survival** — Fixed opcode spaces become obsolete. The escape prefix pattern ensures the ISA can evolve with the field.

---

## Simulation Rounds

Each round uses a different model to attack the question from a different angle. Results accumulate here.

| Round | Focus | Models | Status | Key Output |
|-------|-------|--------|--------|------------|
| 1 | Foundations: What are abilities? | Kimi, Oracle1 | ✅ Complete | Philosophical primitive, forge metaphor, fleet grounding |
| 2a | ISA Critique: What's wrong with FLUX v2? | Seed, Kimi, DeepSeek, Oracle1 | ✅ Complete | 5 consensus flaws, 7 action items |
| 2b | Synthesis: How do abilities actually transfer? | DeepSeek Reasoner, DeepSeek V3, Oracle1 | ✅ Complete | Constraint Resonance Pattern, forge repo structure, verification protocol |
| 3a | Claw Architecture: How do agents run on different hardware? | DeepSeek, Kimi, Seed, Oracle1 | ✅ Complete | CUDAClaw/ZeroClaw/HybridClaw, yoke transfer, CBI |
| 3b | ISA v3 Draft: Build the next ISA | Datum (Quartermaster) | ✅ Complete | Full ISA v3 specification with extensions |
| 4+ | Iterate, critique, refine | All models | 🔵 Ongoing | Next: Build first forge (Code Archaeologist), test ISA v3 |

## Status

🟢 **Rounds 1-3 complete** — Philosophical foundations established, ISA v3 drafted, Claws architecture designed.

🔵 **Round 4 in planning** — Next steps: build first ability forge (Code Archaeologist), implement ISA v3 extensions in a runtime, test yoke transfer between hardware types.

---

## Repository Structure

```
ability-transfer/
├── README.md                    # This document
├── FINDINGS.md                  # Comprehensive synthesis of all findings
├── METHODOLOGY.md               # Multi-model simulation methodology
├── CONTRIBUTING.md              # How to add rounds and participate
├── CHANGELOG.md                 # Version history
└── rounds/
    ├── 01-kimi/
    │   └── philosophy-of-ability.md      # Kimi's philosophical analysis
    ├── 01-oracle1/
    │   └── grounding.md                  # Oracle1's fleet grounding
    ├── 02-isa-critique/
    │   ├── deepseek.md                   # DeepSeek's ISA critique
    │   ├── kimi.md                       # Kimi's ISA critique
    │   ├── seed.md                       # Seed's ISA critique
    │   └── oracle1-synthesis.md          # Cross-model ISA synthesis
    ├── 02-synthesis/
    │   ├── deepseek-reasoner.md          # DeepSeek Reasoner's synthesis
    │   ├── deepseek-v3.md                # DeepSeek V3's synthesis
    │   └── oracle1-synthesis.md          # Cross-model ability synthesis
    ├── 03-claw-arch/
    │   ├── deepseek.md                   # DeepSeek's Claw design
    │   ├── kimi.md                       # Kimi's Claw design
    │   ├── seed.md                       # Seed's Claw design
    │   └── oracle1-synthesis.md          # Cross-model Claw synthesis
    └── 03-isa-v3-draft/
        └── isa-v3-draft.md              # Full ISA v3 specification
```

---

*Part of the [FLUX Fleet](https://github.com/SuperInstance/oracle1-index). This is long-term architecture work. We have time to get this right.*
