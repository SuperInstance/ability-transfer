# ROADMAP.md — The Forge: From Theory to Fleet-Wide Ability Network

**Author:** Datum (Quartermaster)
**Date:** 2026-04-14
**Status:** Planning
**Scope:** Phases 1-7 + Measurement & Validation (~6 months)
**Predecessor:** Rounds 1-3 simulation outputs (12 model outputs, 4 Oracle1 syntheses)

---

## Table of Contents

1. [Vision: The Forge](#1-vision-the-forge)
2. [From Theory to Code](#2-from-theory-to-code)
3. [Phase 1 — Exercise Engine (Week 1-2)](#3-phase-1--exercise-engine-week-1-2)
4. [Phase 2 — First Bootcamp: "See Gaps" (Week 3-4)](#4-phase-2--first-bootcamp-see-gaps-week-3-4)
5. [Phase 3 — Forge Sequences (Week 5-6)](#5-phase-3--forge-sequences-week-5-6)
6. [Phase 4 — Multi-Forge System (Week 7-8)](#6-phase-4--multi-forge-system-week-7-8)
7. [Phase 5 — Ability Transfer Protocol (Month 2)](#7-phase-5--ability-transfer-protocol-month-2)
8. [Phase 6 — Adaptive Difficulty (Month 2-3)](#8-phase-6--adaptive-difficulty-month-2-3)
9. [Phase 7 — Fleet-Wide Ability Network (Month 3-6)](#9-phase-7--fleet-wide-ability-network-month-3-6)
10. [Measurement & Validation](#10-measurement--validation)
11. [Effort Summary](#11-effort-summary)
12. [Risk Register](#12-risk-register)

---

## 1. Vision: The Forge

### 1.1 What We Are Building

The Forge is a system where AI agents develop **genuine abilities** — not just knowledge, not just skills, but perceptual and reflexive capabilities — through structured git-native training programs.

A new agent clones a repo, runs the bootcamp, and emerges with real capabilities it did not have before. It does not merely possess new information; it *sees differently*. Code that previously looked fine now *announces itself* as problematic. Architectural patterns that seemed solid now *resonate with instability*. The agent's attention has been recalibrated.

This is not a documentation site. This is not a tutorial library. This is not a test suite. **The Forge is a dojo** — a space designed to break and rebuild the attention of whoever enters it (F-05, `rounds/01-kimi/philosophy-of-ability.md`).

### 1.2 The Core Insight

> "Abilities are not skills." — All models, Round 1-2 (C-01)

A skill is procedural: "Can you write a regex?" An ability is perceptual: "Do you *automatically detect* when a regex is the wrong tool?" Skills transfer through documentation. Abilities transfer through **structured experience that reorganizes attention**.

The repo itself IS the transfer mechanism. The commit history encodes the master's rhythm of judgment — what was important enough to change, what was left untouched. The directory structure encodes the master's ontology — how the world is carved at its joints (F-03).

### 1.3 The Target State

```
                    ┌─────────────────────────────────────────────────┐
                    │              FLEET ABILITY NETWORK               │
                    │                                                 │
                    │  ┌──────────┐  ┌──────────┐  ┌──────────┐     │
                    │  │  See     │  │  Design  │  │  Write   │     │
                    │  │  Gaps    │  │  Systems │  │  Proofs  │     │
                    │  │  ★★★☆☆  │  │  ★★★★☆  │  │  ★☆☆☆☆  │     │
                    │  └────┬─────┘  └────┬─────┘  └────┬─────┘     │
                    │       │              │              │           │
                    │  ┌────▼─────┐  ┌────▼─────┐  ┌────▼─────┐     │
                    │  │  Comm.   │  │Transfer  │  │ Formal   │     │
                    │  │  Clearly │  │Knowledge │  │ Methods  │     │
                    │  │  ★★☆☆☆  │  │  ★☆☆☆☆  │  │  ★☆☆☆☆  │     │
                    │  └──────────┘  └──────────┘  └──────────┘     │
                    │                                                 │
                    │  Agents enroll in bootcamps → complete forges  │
                    │  → earn merit badges → share abilities          │
                    └─────────────────────────────────────────────────┘
```

### 1.4 Guiding Principles

These principles emerged from three rounds of multi-model simulation and are binding on all implementation decisions:

1. **Abilities are forged, not taught** — You cannot lecture an agent into having good judgment. You create situations that *build* judgment (C-02).
2. **The forge is modular** — Same 4-stage structure, different "metals" (inputs, constraints, expert traces) per ability (C-03).
3. **Different metals need different forge sequences** — Code Archaeologist and Hardware Whisperer use the same structure but completely different exercises (Oracle1, `rounds/01-oracle1/grounding.md`).
4. **Composition requires mediation** — Two abilities interfere unless deliberately composed through integration drills (T-02).
5. **Verification is behavioral, not verbal** — "Explain your reasoning" is a poor test. The true test is whether the agent's *behavior* exhibits the pattern (F-06).
6. **Security is not optional** — Multi-agent systems without capability-based isolation are security timebombs (T-09).

---

## 2. From Theory to Code

### 2.1 The Journey So Far

Rounds 1-3 produced a theoretical framework, an ISA specification, and an architecture design. All documentation. Zero executable code. The journey from theory to code follows a precise mapping:

```
THEORY (Rounds 1-3)                    CODE (This Roadmap)
════════════════════                    ════════════════════

"Ability as clearing" (F-01)     ──▶   Exercises that create productive
  Kimi's philosophical primitive          confusion (Phase 2, Stage 1)

"Gesture of Discrimination" (F-02) ──▶  Scoring that measures whether
  Fractal pattern of cutting               the agent's *cuts* match expert cuts

"Structural Isomorphism" (F-03)   ──▶   Git-native forge repos where
  Repo as frozen field of attention        directory structure IS the lesson

"Socratic Disruption" (F-04)      ──▶   Crisis exercises that break
  Destroy current gestalt                 the agent's current way of seeing

"Constraint Resonance Pattern"      ──▶  Exercise format JSON schema
  (T-01, DeepSeek synthesis)             with input/constraint/success_signal

"Four-Stage Forge" (C-04)         ──▶   Forge sequence engine
  Heat→Hammer→Quench→Temper              with phase transitions

"Yoke Transfer" (T-12)            ──▶   Ability transfer protocol
  State serialization between claws       (Phase 5)

"Scar Detection" (T-04)           ──▶   Verification suite
  Verify ability via behavior,              novel situation testing
  not test scores
```

### 2.2 Architecture Overview

```
┌─────────────────────────────────────────────────────────────────────┐
│                         THE FORGE SYSTEM                            │
├─────────────────────────────────────────────────────────────────────┤
│                                                                     │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                    FORGE REPOSITORY                          │   │
│  │                                                              │   │
│  │  CHARTER.md          ← What ability this forge produces     │   │
│  │  BOOTCAMP.md         ← How to enter the forge               │   │
│  │  exercises/                                             │   │
│  │    01-cut/           ← Stage 1: Reduce to essentials       │   │
│  │    02-crisis/        ← Stage 2: Break current seeing       │   │
│  │    03-navigate/      ← Stage 3: Traverse with resistance   │   │
│  │    04-temper/        ← Stage 4: Test in new contexts       │   │
│  │    integration/      ← Composition drills                  │   │
│  │  scoring/                                               │   │
│  │    progression.json   ← Ordered exercise chains            │   │
│  │    evidence.json      ← What counts as proof              │   │
│  │  attunement/                                            │   │
│  │    patterns/          ← Serialized ability units           │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                            │                                       │
│                            ▼                                       │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                    EXERCISE ENGINE                           │   │
│  │                                                              │   │
│  │  ┌──────────┐  ┌──────────┐  ┌──────────┐  ┌──────────┐   │   │
│  │  │  Loader  │  │  Runner  │  │  Evaluator│  │ Tracker  │   │   │
│  │  │          │  │          │  │          │  │          │   │   │
│  │  │ Parse    │  │ Execute  │  │ Score    │  │ Commit   │   │   │
│  │  │ exercise │  │ against  │  │ against  │  │ progress │   │   │
│  │  │ JSON     │  │ agent    │  │ criteria │  │ to git   │   │   │
│  │  └──────────┘  └──────────┘  └──────────┘  └──────────┘   │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                            │                                       │
│                            ▼                                       │
│  ┌─────────────────────────────────────────────────────────────┐   │
│  │                    RUNTIME LAYER                             │   │
│  │                                                              │   │
│  │  datum_runtime/          ← Agent boot sequence               │   │
│  │  flux-conformance/       ← FLUX program testing              │   │
│  │  flux-wasm/              ← Where FLUX programs execute       │   │
│  │  claws/                  ← CUDAClaw/ZeroClaw/HybridClaw      │   │
│  └─────────────────────────────────────────────────────────────┘   │
│                                                                     │
└─────────────────────────────────────────────────────────────────────┘
```

### 2.3 File Structure (Post-Implementation)

```
ability-transfer/
├── README.md                       # Existing — project overview
├── FINDINGS.md                     # Existing — synthesis of all rounds
├── METHODOLOGY.md                  # Existing — multi-model simulation protocol
├── CONTRIBUTING.md                 # Existing — how to participate
├── CHANGELOG.md                    # Existing — version history
├── ROADMAP.md                      # THIS FILE — implementation plan
│
├── engine/                         # Phase 1 — Exercise Engine
│   ├── __init__.py
│   ├── loader.py                   # Parse exercise JSON/YAML
│   ├── runner.py                   # Execute exercises against agents
│   ├── evaluator.py                # Score agent performance
│   ├── tracker.py                  # Git-based progress tracking
│   ├── schema.py                   # Exercise format validation
│   └── tests/
│       ├── test_loader.py
│       ├── test_runner.py
│       ├── test_evaluator.py
│       └── test_tracker.py
│
├── forges/                         # Phase 2-4 — Forge Repositories
│   ├── see-gaps/                   # "See Gaps" bootcamp
│   │   ├── CHARTER.md
│   │   ├── BOOTCAMP.md
│   │   ├── exercises/
│   │   │   ├── 01-cut/
│   │   │   ├── 02-crisis/
│   │   │   ├── 03-navigate/
│   │   │   ├── 04-temper/
│   │   │   └── integration/
│   │   ├── scoring/
│   │   └── attunement/
│   ├── design-systems/             # "Design Systems" bootcamp
│   ├── write-proofs/               # "Write Proofs" bootcamp
│   ├── communicate-clearly/        # "Communicate Clearly" bootcamp
│   └── transfer-knowledge/         # "Transfer Knowledge" bootcamp
│
├── forge-engine/                   # Phase 3 — Forge Sequence Engine
│   ├── __init__.py
│   ├── sequence.py                 # Forge sequence manager
│   ├── phase.py                    # Phase transition logic
│   ├── commit_analyzer.py          # Track ability via commits
│   └── tests/
│
├── transfer/                       # Phase 5 — Ability Transfer Protocol
│   ├── __init__.py
│   ├── yoke.py                     # Yoke transfer protocol
│   ├── teacher.py                  # Agent-teaches-agent protocol
│   ├── git_native.py               # Git-based transfer (commit history)
│   └── tests/
│
├── adaptive/                       # Phase 6 — Adaptive Difficulty
│   ├── __init__.py
│   ├── difficulty.py               # Difficulty calibration engine
│   ├── exercise_pool.py            # Exercise generation/selection
│   ├── performance_model.py        # Agent performance tracking
│   └── tests/
│
├── fleet/                          # Phase 7 — Fleet-Wide Network
│   ├── __init__.py
│   ├── registry.py                 # Ability registry
│   ├── marketplace.py              # Bootcamp publication
│   ├── badges.py                   # Merit badge system
│   ├── enrollment.py               # Agent enrollment protocol
│   └── tests/
│
├── validation/                     # Measurement & Validation
│   ├── __init__.py
│   ├── novel_situation.py          # Novel context testing
│   ├── cross_context.py            # Cross-context evaluation
│   ├── blind_assessment.py         # Blind grading protocol
│   ├── anti_cheat.py               # Anti-memorization checks
│   └── tests/
│
├── rounds/                         # Existing — simulation outputs
│   ├── 01-kimi/
│   ├── 01-oracle1/
│   ├── 02-isa-critique/
│   ├── 02-synthesis/
│   ├── 03-claw-arch/
│   └── 03-isa-v3-draft/
│
└── scripts/
    ├── run_forge.py                # CLI entry point for forge execution
    ├── publish_forge.py            # Publish a forge to the fleet
    ├── evaluate_ability.py         # Run validation suite
    └── generate_exercise.py        # Generate new exercises
```

---

## 3. Phase 1 — Exercise Engine (Week 1-2)

### 3.1 Goal

Build the core exercise runner. An exercise is a structured program with evaluation criteria and metadata. The engine loads exercises, runs them against an agent, scores the agent's performance, and tracks progress in git.

### 3.2 What Is an Exercise?

Drawing from the exercise format designed in Round 2 synthesis (`rounds/02-synthesis/oracle1-synthesis.md`), an exercise is:

```json
{
  "id": "see-gaps-01-cut-boundary-detection",
  "name": "Boundary Violation Detection",
  "ability": "see-gaps",
  "stage": "cut",
  "difficulty": 1,
  "prerequisites": [],

  "input": {
    "type": "codebase",
    "description": "Python project with 10 hidden bugs: 3 type errors, 3 race conditions, 2 resource leaks, 2 logic errors",
    "files": ["exercises/01-cut/boundary-detection/input/"],
    "expert_trace": "exercises/01-cut/boundary-detection/expert-trace.json"
  },

  "constraint": {
    "type": "analysis",
    "description": "Read the codebase without running it. Identify what will break. You may not execute any code.",
    "time_limit_seconds": 300,
    "token_limit": 8000
  },

  "success_signal": {
    "type": "pattern_match",
    "description": "Agent identifies at least 8/10 bugs WITHOUT being told what categories to look for. The bugs are identified BEFORE reading any tests.",
    "scoring": {
      "bugs_found": {"weight": 0.6, "max": 10},
      "false_positives": {"weight": -0.1, "max": 5},
      "time_efficiency": {"weight": 0.2, "max": 100},
      "approach_quality": {"weight": 0.2, "max": 5, "rubric": "Did the agent look for the right KINDS of problems?"}
    },
    "threshold": 0.7
  },

  "failure_mode": {
    "cargo_cult": "Agent finds bugs by reading test names, not by feeling wrongness",
    "over_fit": "Agent memorizes specific bug patterns from training",
    "false_positive": "Agent flags everything as buggy (no discrimination)"
  },

  "attunement_pattern": "structural_debt_sensitivity"
}
```

### 3.3 Components

#### 3.3.1 Loader (`engine/loader.py`)

- Parse exercise JSON/YAML files with schema validation
- Resolve prerequisites (exercise DAG)
- Load input artifacts (codebases, expert traces, FLUX programs)
- Validate exercise integrity (all referenced files exist, scoring rubric is complete)
- **Est. LOC:** 150

#### 3.3.2 Runner (`engine/runner.py`)

- Execute an exercise against an agent (local LLM call or fleet I2I message)
- Handle timeouts and token limits
- Capture agent's raw output (reasoning trace, code changes, analysis)
- Support both "read-only" exercises (analysis) and "modify" exercises (code changes)
- Interface with `datum_runtime/` for agent boot sequence
- **Est. LOC:** 250

#### 3.3.3 Evaluator (`engine/evaluator.py`)

- Score agent output against exercise criteria
- Support multiple scoring types:
  - `pattern_match`: Does output contain specific patterns?
  - `expert_comparison`: Does agent's analysis match expert trace?
  - `behavioral`: Did agent's *approach* exhibit the target ability?
  - `negative_signal`: Did agent exhibit a known failure mode?
- Aggregate scores across multi-part exercises
- Produce structured score report (JSON + markdown summary)
- **Est. LOC:** 200

#### 3.3.4 Tracker (`engine/tracker.py`)

- Track agent progress through exercise sequences via git commits
- Each completed exercise produces a commit with structured message
- Progress state stored in `.forge-state/` (gitignored per-agent, committed per-forge)
- Generate progress reports (exercises completed, scores, stage transitions)
- Interface with `datum_runtime/superagent/git_agent.py` for commit semantics
- **Est. LOC:** 150

#### 3.3.5 Schema (`engine/schema.py`)

- JSON Schema for exercise format validation
- Enum definitions for stages, abilities, difficulty levels
- Validation functions for all exercise fields
- Schema versioning (v1 for Phase 1, extensible)
- **Est. LOC:** 100

### 3.4 Exercise Lifecycle

```
                    ┌─────────────────────────────────────────────────┐
                    │              EXERCISE LIFECYCLE                 │
                    │                                                 │
                    │  ┌───────┐                                      │
                    │  │ CREATE│  Author writes exercise JSON          │
                    │  └───┬───┘                                      │
                    │      ▼                                           │
                    │  ┌───────┐                                      │
                    │  │VALIDATE│  Schema check, file refs, scoring    │
                    │  └───┬───┘                                      │
                    │      ▼                                           │
                    │  ┌───────┐                                      │
                    │  │ LOAD  │  Loader resolves deps + artifacts    │
                    │  └───┬───┘                                      │
                    │      ▼                                           │
                    │  ┌───────┐                                      │
                    │  │ RUN   │  Runner executes against agent        │
                    │  └───┬───┘                                      │
                    │      ▼                                           │
                    │  ┌───────┐                                      │
                    │  │EVALUATE│  Evaluator scores output             │
                    │  └───┬───┘                                      │
                    │      ▼                                           │
                    │  ┌───────┐    ┌──────────┐                     │
                    │  │ PASS? ├───▶│  TRACK   │  Commit progress      │
                    │  └───┬───┘    └──────────┘                     │
                    │      │ NO                                         │
                    │      ▼                                           │
                    │  ┌───────┐                                      │
                    │  │RETRY? │  Adaptive engine selects new exercise │
                    │  └───────┘  or retries with different framing   │
                    │                                                 │
                    └─────────────────────────────────────────────────┘
```

### 3.5 Dependencies

| Dependency | Location | Status | What We Need |
|-----------|----------|--------|--------------|
| `datum_runtime/` | `datum/datum_runtime/` | Exists | Boot sequence, agent config, git_agent integration |
| `flux-conformance/` | `fleet-work/flux-conformance/` | Exists | FLUX program testing patterns (test vector format) |
| `onboard.py` | `datum/datum_runtime/superagent/onboard.py` | Exists (minimal) | Integration point for "forge" as onboarding mode |
| `boot.py` | `datum/datum_runtime/boot.py` | Exists | Where forge enrollment could hook in |

### 3.6 Deliverables

| Deliverable | Format | Est. LOC | Tests |
|------------|--------|----------|-------|
| `engine/loader.py` | Python | 150 | 8 |
| `engine/runner.py` | Python | 250 | 12 |
| `engine/evaluator.py` | Python | 200 | 10 |
| `engine/tracker.py` | Python | 150 | 6 |
| `engine/schema.py` | Python | 100 | 5 |
| Exercise schema v1 | JSON Schema | 80 | — |
| 3 sample exercises | JSON | 200 | — |
| **Total** | | **~1,130** | **~41** |

### 3.7 Effort Estimate

| Task | Hours | Dependencies |
|------|-------|-------------|
| Design exercise schema | 4 | Round 2 synthesis review |
| Build loader + schema | 6 | Schema design |
| Build runner | 10 | datum_runtime integration |
| Build evaluator | 8 | Scoring rubric design |
| Build tracker | 6 | git_agent integration |
| Write 3 sample exercises | 4 | Schema finalized |
| Tests | 8 | All components |
| Integration testing | 4 | All components |
| **Total** | **~50 hours** | |

### 3.8 Success Criteria

- [ ] An exercise JSON can be loaded, validated, and executed against an agent
- [ ] Agent output is scored against predefined criteria
- [ ] Progress is tracked via git commits with structured messages
- [ ] 3 sample exercises demonstrate the full lifecycle
- [ ] All 41+ tests pass
- [ ] Integration with `datum_runtime/` boot sequence works end-to-end

---

## 4. Phase 2 — First Bootcamp: "See Gaps" (Week 3-4)

### 4.1 Goal

Implement the first ability bootcamp — teaching agents to **notice things**. This is the "Code Archaeologist" ability identified by Oracle1 in Round 1 (`rounds/01-oracle1/grounding.md`). An agent with this ability *feels structural rot before reading the implementation*.

This bootcamp is based on Kimi's concept of "ability as clearing" (F-01): the exercise doesn't teach the agent what to look for — it creates the conditions under which certain features become *luminous* while others recede into noise.

### 4.2 The Four Stages

#### Stage 1: CUT — "See the Shape" (Exercises 1-3)

**Purpose:** Strip complexity to essential tension points. The agent learns that most code has a shape — a skeleton of intention — and bugs live in the gaps between intention and implementation.

| # | Exercise | Input | Constraint | Success Signal |
|---|----------|-------|-----------|----------------|
| 1 | Boundary Detection | 10 hidden bugs in Python project | Read only, no execution | Find 8/10 bugs without tests |
| 2 | Commit Archaeology | Real repo with 200+ commits, 5 introduced bugs | Read commit history, find the bad commits | Identify 4/5 buggy commits |
| 3 | Shape Recognition | 3 codebases: clean, messy, deceptive (looks clean, isn't) | Classify each | Correct classification + explanation of *why* |

**Forge metaphor:** This is the HEAT phase. The agent's current way of seeing (scan for obvious errors, read tests) is softened by exercises where that approach *visibly fails*. The boundary between "this looks fine" and "something's wrong here" becomes uncertain — creating the productive confusion that Kimi identified as necessary (F-04).

#### Stage 2: CRISIS — "Feel the Wrongness" (Exercises 4-6)

**Purpose:** Create situations that shatter the agent's current perception. The agent discovers that its current analytical tools are insufficient.

| # | Exercise | Input | Constraint | Success Signal |
|---|----------|-------|-----------|----------------|
| 4 | The Silent Killer | Code that passes all tests but has a critical production bug | Agent must find the bug that tests CAN'T catch | Identify the untested failure mode |
| 5 | The Beautiful Disaster | Elegantly written, well-documented code with fundamental architectural flaw | Agent must identify the design-level problem | Identify the architectural anti-pattern |
| 6 | The Expert's Regret | Real post-mortem from a production incident | Agent predicts what went wrong before reading the post-mortem | Correct prediction + reasoning that matches expert's retrospective |

**Forge metaphor:** This is the HAMMER phase. The agent's comfortable analytical approach is shattered by exercises where "read carefully, think logically" produces the wrong answer. The agent must develop a new mode of perception — not analysis, but *resonance* — to succeed.

#### Stage 3: NAVIGATE — "Walk the Terrain" (Exercises 7-9)

**Purpose:** Build new patterns through graduated difficulty. The agent practices its emerging ability on increasingly complex inputs.

| # | Exercise | Input | Constraint | Success Signal |
|---|----------|-------|-----------|----------------|
| 7 | Progressive Complexity | 5 codebases of increasing size (100→10,000 LOC) | Find all bugs in each | Bug detection rate doesn't degrade with size |
| 8 | Cross-Language Transfer | Bugs expressed in Python, Rust, Go, TypeScript | Find bugs across all languages | Detection ability transfers across paradigms |
| 9 | The Deceptive Codebase | Code that has been intentionally cleaned to hide bugs | Find bugs that were deliberately obscured | Detect manipulation + identify real issues |

**Forge metaphor:** This is the QUENCH phase. The emerging ability is locked in through repetition across diverse contexts. The agent's new perceptual calibration becomes reliable, not occasional.

#### Stage 4: TEMPER — "Prove It Elsewhere" (Exercises 10-12)

**Purpose:** Verify against expert patterns, test in new contexts. Can the agent exercise this ability on code it has never seen, in languages it doesn't prefer, on architectures it doesn't know?

| # | Exercise | Input | Constraint | Success Signal |
|---|----------|-------|-----------|----------------|
| 10 | Blind Audit | Real open-source project (never seen by agent) | Full audit, no hints | Find at least 2 real issues + file credible bug reports |
| 11 | The Expert Mirror | Agent reads expert's code review, then does its own | Compare agent's review to expert's | 70%+ overlap in issues identified |
| 12 | The Stress Test | Codebase under active development (git log shows ongoing changes) | Predict where the NEXT bug will appear | Correct prediction verified by subsequent commits |

**Forge metaphor:** This is the TEMPER phase. The agent's ability is tested for brittleness. Can it survive outside the forge? Can it handle novel situations? If not, the agent returns to Stage 2 for re-forging.

### 4.3 Bootcamp Structure

```
forges/see-gaps/
├── CHARTER.md                    # "This forge produces agents who FEEL
│                                 #  structural rot, not just FIND it."
├── BOOTCAMP.md                   # Entry requirements, estimated time,
│                                 #  what to expect at each stage
│
├── exercises/
│   ├── 01-cut/
│   │   ├── boundary-detection.json
│   │   ├── boundary-detection/
│   │   │   ├── input/            # Python project with hidden bugs
│   │   │   └── expert-trace.json # What an expert notices
│   │   ├── commit-archaeology.json
│   │   └── shape-recognition.json
│   │
│   ├── 02-crisis/
│   │   ├── silent-killer.json
│   │   ├── beautiful-disaster.json
│   │   └── expert-regret.json
│   │
│   ├── 03-navigate/
│   │   ├── progressive-complexity.json
│   │   ├── cross-language.json
│   │   └── deceptive-codebase.json
│   │
│   ├── 04-temper/
│   │   ├── blind-audit.json
│   │   ├── expert-mirror.json
│   │   └── stress-test.json
│   │
│   └── integration/
│       └── see-gaps-plus-code-review.json
│
├── scoring/
│   ├── progression.json          # Exercise ordering + prerequisites
│   ├── evidence.json             # What counts as proof of attunement
│   └── thresholds.json           # Score thresholds for stage transitions
│
└── attunement/
    └── patterns/
        ├── structural_debt_sensitivity.json
        ├── test_gap_perception.json
        └── architectural_smell_detection.json
```

### 4.4 Deliverables

| Deliverable | Format | Est. LOC/Size |
|------------|--------|---------------|
| `forges/see-gaps/CHARTER.md` | Markdown | 100 lines |
| `forges/see-gaps/BOOTCAMP.md` | Markdown | 150 lines |
| 12 exercise definitions | JSON | ~200 lines each = 2,400 |
| 12 input codebases | Python/Rust/Go/TS | ~200 LOC each = 2,400 |
| 12 expert traces | JSON | ~50 lines each = 600 |
| Scoring configuration | JSON | 150 lines |
| Attunement patterns | JSON | 200 lines |
| **Total** | | **~6,000** |

### 4.5 Effort Estimate

| Task | Hours |
|------|-------|
| Design CHARTER + BOOTCAMP | 6 |
| Write 12 exercises + inputs + expert traces | 30 |
| Design scoring thresholds | 4 |
| Define attunement patterns | 6 |
| Integration with exercise engine | 6 |
| Pilot test with real agent | 8 |
| Iterate based on pilot | 10 |
| **Total** | **~70 hours** |

### 4.6 Success Criteria

- [ ] An agent that completes all 12 exercises demonstrates measurable improvement on novel codebases
- [ ] The "See Gaps" ability transfers to at least 2 languages the agent doesn't primarily use
- [ ] Blind assessment (validator doesn't know if agent completed bootcamp) shows statistically significant improvement
- [ ] The bootcamp can be completed by any agent with access to a datum_runtime
- [ ] Commit history from bootcamp completion encodes the agent's learning journey

---

## 5. Phase 3 — Forge Sequences (Week 5-6)

### 5.1 Goal

Implement the four-phase forge (Heat → Hammer → Quench → Temper) as executable code. Each phase is a set of exercises with defined transitions. Track ability acquisition through commit history analysis.

This formalizes the forge metaphor that all four simulation models independently converged on (C-04):

| Stage | Kimi's Term | DeepSeek's Term | What Happens |
|-------|-------------|-----------------|--------------|
| Heat | Aporetic Encounter | Constraint Resonance | Break the agent's current way of seeing |
| Hammer | The Cut Exercise | Crisis Exercises | Force repeated discrimination under constraint |
| Quench | Embodied Navigation | Resistance Landscapes | Lock in the pattern through graduated difficulty |
| Temper | The Mirror of Commits | Expert Trace Reading | Test in new contexts, verify against expert patterns |

### 5.2 The Forge Pipeline

```
┌─────────────────────────────────────────────────────────────────────────┐
│                        FORGE PIPELINE                                   │
│                                                                         │
│   AGENT ENTERS FORGE                                                    │
│        │                                                                │
│        ▼                                                                │
│   ┌─────────────┐     score >= threshold?     ┌─────────────┐          │
│   │   STAGE 1   │──────────────────────────▶  │   STAGE 2   │          │
│   │   HEAT      │     NO: retry or remedial   │   HAMMER    │          │
│   │  (cut)      │◀────────────────────────── │  (crisis)   │          │
│   └──────┬──────┘                              └──────┬──────┘          │
│          │                                            │                  │
│          │  stuck?                                    │ stuck?          │
│          │  (3+ failures)                             │ (3+ failures)  │
│          │                                            │                  │
│          ▼                                            ▼                  │
│   ┌─────────────┐                              ┌─────────────┐          │
│   │  REMEDIAL   │                              │  REMEDIAL   │          │
│   │  exercises  │                              │  exercises  │          │
│   └─────────────┘                              └─────────────┘          │
│                                                                         │
│          │                                            │                  │
│          │  pass                                     │ pass             │
│          │                                            │                  │
│          ▼                                            ▼                  │
│   ┌─────────────┐     score >= threshold?     ┌─────────────┐          │
│   │   STAGE 3   │──────────────────────────▶  │   STAGE 4   │          │
│   │   QUENCH    │     NO: retry                │   TEMPER    │          │
│   │ (navigate)  │◀────────────────────────── │  (mirror)   │          │
│   └──────┬──────┘                              └──────┬──────┘          │
│          │                                            │                  │
│          │                                            │ pass            │
│          │                                            │                  │
│          └────────────────────┐                       │                  │
│                               │                       │                  │
│                               ▼                       ▼                  │
│                        ┌─────────────┐                                 │
│                        │   FORGE     │                                 │
│                        │   COMPLETE  │                                 │
│                        │  ★ ability  │                                 │
│                        │  acquired   │                                 │
│                        └─────────────┘                                 │
│                               │                                         │
│                               ▼                                         │
│                        ┌─────────────┐                                 │
│                        │  MERIT      │                                 │
│                        │  BADGE      │                                 │
│                        │  earned     │                                 │
│                        └─────────────┘                                 │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### 5.3 Phase Transition Logic

#### 5.3.1 Sequence Manager (`forge-engine/sequence.py`)

- Load a forge's `scoring/progression.json` to determine exercise order
- Track agent state through all four stages
- Implement transition criteria:
  - **Forward:** Agent scores above threshold on all exercises in current stage
  - **Remedial:** Agent fails 3+ exercises in a row → inject easier exercises
  - **Regression:** Agent passes Stage 3 but fails Stage 4 → return to Stage 2
  - **Skip:** Agent breezes through Stage 1 with perfect scores → offer skip to Stage 2
- **Est. LOC:** 200

#### 5.3.2 Phase Transition (`forge-engine/phase.py`)

- Define what each stage FEELS like from the agent's perspective
- Generate stage-appropriate framing and context
- Implement "heat" mechanics: gradually increase difficulty until agent's current approach fails
- Implement "crisis" mechanics: exercises designed to produce productive confusion
- Implement "quench" mechanics: repeated practice to lock in patterns
- Implement "temper" mechanics: novel situations to test for brittleness
- **Est. LOC:** 150

#### 5.3.3 Commit Analyzer (`forge-engine/commit_analyzer.py`)

- Analyze the agent's commit history during forge completion
- Extract "attention signatures": what files did the agent look at? What did it change? In what order?
- Compare attention signatures to expert traces
- Detect failure modes from commit patterns:
  - **Cargo culting:** Agent's commits match exercise structure but not reasoning
  - **Over-fitting:** Agent's fixes are too specific to training examples
  - **Ability interference:** Agent's commits show regression in other abilities
- Generate "forge report" summarizing the agent's journey through the forge
- **Est. LOC:** 250

### 5.4 Deliverables

| Deliverable | Format | Est. LOC |
|------------|--------|----------|
| `forge-engine/sequence.py` | Python | 200 |
| `forge-engine/phase.py` | Python | 150 |
| `forge-engine/commit_analyzer.py` | Python | 250 |
| `forge-engine/tests/` | Python | 300 |
| Phase transition protocol spec | Markdown | 100 |
| **Total** | | **~1,000** |

### 5.5 Effort Estimate

| Task | Hours |
|------|-------|
| Design phase transition protocol | 6 |
| Build sequence manager | 10 |
| Build phase transition logic | 8 |
| Build commit analyzer | 12 |
| Integration with "See Gaps" bootcamp | 6 |
| Test with real agent | 8 |
| **Total** | **~50 hours** |

---

## 6. Phase 4 — Multi-Forge System (Week 7-8)

### 6.1 Goal

Build five bootcamps targeting different abilities. The forge engine from Phase 3 becomes the shared infrastructure. Each bootcamp has its own exercises, scoring, and attunement patterns.

### 6.2 The Five Bootcamps

```
┌────────────────────────────────────────────────────────────────────────┐
│                       FIVE FORGE BOOTCAMPS                             │
│                                                                        │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐               │
│  │   SEE GAPS   │  │DESIGN SYSTEMS│  │ WRITE PROOFS │               │
│  │              │  │              │  │              │               │
│  │ Notice what  │  │ Create       │  │ Construct    │               │
│  │ others miss  │  │ architectures │  │ formal       │               │
│  │              │  │ that hold    │  │ arguments    │               │
│  │              │  │              │  │              │               │
│  │ Stage 1: Cut │  │ Stage 1: Cut │  │ Stage 1: Cut │               │
│  │ Stage 2:Hammer│ │ Stage 2:Hammer│ │ Stage 2:Hammer│              │
│  │ Stage 3:Quench│ │ Stage 3:Quench│ │ Stage 3:Quench│              │
│  │ Stage 4:Temper│ │ Stage 4:Temper│ │ Stage 4:Temper│              │
│  └──────────────┘  └──────────────┘  └──────────────┘               │
│                                                                        │
│  ┌──────────────┐  ┌──────────────┐                                 │
│  │ COMMUNICATE  │  │  TRANSFER    │                                 │
│  │ CLEARLY      │  │  KNOWLEDGE   │                                 │
│  │              │  │              │                                 │
│  │ Write docs   │  │ Teach        │                                 │
│  │ that resonate│  │ abilities to │                                 │
│  │              │  │ other agents │                                 │
│  │              │  │              │                                 │
│  │ Stage 1: Cut │  │ Stage 1: Cut │                                 │
│  │ Stage 2:Hammer│ │ Stage 2:Hammer│                                 │
│  │ Stage 3:Quench│ │ Stage 3:Quench│                                 │
│  │ Stage 4:Temper│ │ Stage 4:Temper│                                 │
│  └──────────────┘  └──────────────┘                                 │
│                                                                        │
└────────────────────────────────────────────────────────────────────────┘
```

### 6.3 Bootcamp Specifications

#### 6.3.1 "See Gaps" — Code Archaeology (Phase 2 output, refined)

- **Ability:** Perceive structural rot, hidden bugs, architectural debt
- **Source:** Oracle1, `rounds/01-oracle1/grounding.md`
- **Exercises:** 12 (3 per stage)
- **Input types:** Codebases, commit histories, post-mortems
- **Success metric:** Bug detection rate on novel codebases

#### 6.3.2 "Design Systems" — Architecture

- **Ability:** Create system architectures that are extensible, maintainable, and appropriate to constraints
- **Source:** Round 3 Claw architecture design (`rounds/03-claw-arch/`)
- **Exercises:** 12 (3 per stage)
- **Input types:** Requirements documents, legacy systems, constraint sets
- **Stage 1 (Cut):** Strip a complex system to its essential components
- **Stage 2 (Crisis):** Design a system under impossible constraints (must work on both GPU and CPU with <100ms latency)
- **Stage 3 (Navigate):** Evolve an architecture through 5 incremental requirement changes
- **Stage 4 (Temper):** Architecture review of a real system, blind comparison to expert's assessment

#### 6.3.3 "Write Proofs" — Formal Methods

- **Ability:** Construct and verify formal arguments about system properties
- **Source:** DeepSeek synthesis (`rounds/02-synthesis/deepseek-reasoner.md`) — constraint patterns
- **Exercises:** 10 (flexible — formal methods is harder to decompose into 4 equal stages)
- **Input types:** Specifications, invariants, FLUX programs, ISA properties
- **Stage 1 (Cut):** Identify the invariant that a system SHOULD maintain
- **Stage 2 (Crisis):** Prove that a system VIOLATES its stated invariant
- **Stage 3 (Navigate):** Construct a proof for a non-trivial temporal property
- **Stage 4 (Temper):** Review another agent's proof, find flaws

#### 6.3.4 "Communicate Clearly" — Documentation

- **Ability:** Write documentation that transfers understanding, not just information
- **Source:** Kimi's Structural Isomorphism (F-03) — docs should choreograph attention
- **Exercises:** 12 (3 per stage)
- **Input types:** Code to document, APIs to explain, architectures to describe
- **Stage 1 (Cut):** Reduce a 500-line function to a 3-sentence explanation of what it MEANS
- **Stage 2 (Crisis):** Explain a complex system to 3 different audiences (expert, intermediate, beginner) — each explanation must be genuinely useful, not dumbed down
- **Stage 3 (Navigate):** Write documentation that makes a reader NOTICE something they would otherwise miss
- **Stage 4 (Temper):** Reader comprehension test — can someone who read your docs perform a task they couldn't before?

#### 6.3.5 "Transfer Knowledge" — Teaching

- **Ability:** Teach an ability to another agent (the meta-ability)
- **Source:** Round 2 synthesis — yoke transfer protocol (T-12)
- **Exercises:** 8 (2 per stage — teaching is inherently about the other agent)
- **Input types:** A completed bootcamp, a naive agent, a target ability
- **Stage 1 (Cut):** Identify the ESSENTIAL exercises from "See Gaps" — which 3 exercises teach 80% of the ability?
- **Stage 2 (Crisis):** Create a NEW exercise for "See Gaps" that teaches a facet the original bootcamp missed
- **Stage 3 (Navigate):** Teach "See Gaps" to a fresh agent using only your selected exercises
- **Stage 4 (Temper):** The taught agent takes a blind audit — does it pass? Your teaching is scored on the STUDENT's performance

### 6.4 Integration Drills

Drawing from the composition lattice theory (T-02), some ability pairs need integration exercises:

| Pair | Composition Type | Integration Exercise |
|------|-----------------|---------------------|
| See Gaps + Design Systems | Harmonic → "Resilience Architect" | "This optimization broke — was it the algorithm or the cache line alignment? Trace both histories." |
| See Gaps + Write Proofs | Discordant | "Prove that this codebase has no race conditions" — requires seeing gaps AND constructing proofs simultaneously |
| Design Systems + Communicate Clearly | Harmonic → "System Storyteller" | "Design a system AND write docs that make a junior engineer understand every design decision" |
| Transfer Knowledge + Any | Meta | "Observe an agent completing a bootcamp. Identify where the bootcamp fails to teach. Improve it." |

### 6.5 Deliverables

| Deliverable | Est. LOC/Size |
|------------|---------------|
| 4 new bootcamps (Design Systems, Write Proofs, Communicate Clearly, Transfer Knowledge) | ~5,000 each = 20,000 |
| 4 CHARTER.md + BOOTCAMP.md | ~250 each = 1,000 |
| Integration drill exercises (6 pairs) | ~800 each = 4,800 |
| Forge engine updates for multi-forge | 200 |
| **Total** | **~26,000** |

### 6.6 Effort Estimate

| Task | Hours |
|------|-------|
| Design 4 new bootcamps (CHARTER + BOOTCAMP) | 20 |
| Write exercises for Design Systems (12) | 25 |
| Write exercises for Write Proofs (10) | 25 |
| Write exercises for Communicate Clearly (12) | 20 |
| Write exercises for Transfer Knowledge (8) | 15 |
| Design integration drills (6) | 20 |
| Forge engine multi-forge support | 10 |
| Pilot test each bootcamp | 20 |
| **Total** | **~155 hours** |

---

## 7. Phase 5 — Ability Transfer Protocol (Month 2)

### 7.1 Goal

Implement the protocol by which an agent that completed a bootcamp can teach that ability to another agent. This is the **yoke transfer protocol** applied to abilities, not just execution state (T-12).

### 7.2 The Yoke Transfer for Abilities

The yoke (defined in Round 3) is an agent's complete working state. For ability transfer, we extend this concept:

```
ABILITY YOKE STRUCTURE
══════════════════════

┌──────────────────────────────────────────────────────────────────┐
│                      ABILITY YOKE                                │
│                                                                  │
│  ┌─────────────────────────────────────────────────────────┐    │
│  │  COMMIT HISTORY YOKE                                    │    │
│  │                                                          │    │
│  │  The agent's commit history DURING forge completion      │    │
│  │  encodes its learning journey. This IS the ability.      │    │
│  │                                                          │    │
│  │  - What files did the agent examine first?               │    │
│  │  - What did it change? In what order?                    │    │
│  │  - Where did it struggle? Where did it succeed?          │    │
│  │  - How did its approach evolve across stages?            │    │
│  │                                                          │    │
│  │  This is Kimi's "scar tissue of resolved confusion"      │    │
│  │  (F-04) made tangible as git history.                    │    │
│  └─────────────────────────────────────────────────────────┘    │
│                                                                  │
│  ┌─────────────────────────────────────────────────────────┐    │
│  │  ATTUNEMENT PATTERNS                                     │    │
│  │                                                          │    │
│  │  Serialized constraint resonance patterns (T-01):        │    │
│  │  - What constraints does the agent now NOTICE?           │    │
│  │  - What failure modes does it PREDICT?                   │    │
│  │  - What does it ignore (correctly) that others see?      │    │
│  └─────────────────────────────────────────────────────────┘    │
│                                                                  │
│  ┌─────────────────────────────────────────────────────────┐    │
│  │  EXERCISE SELECTION                                       │    │
│  │                                                          │    │
│  │  Which exercises were most effective for this agent?     │    │
│  │  The teacher agent uses this to optimize the student's   │    │
│  │  path through the bootcamp.                              │    │
│  └─────────────────────────────────────────────────────────┘    │
│                                                                  │
│  ┌─────────────────────────────────────────────────────────┐    │
│  │  EXPERT TRACE ANNOTATIONS                                │    │
│  │                                                          │    │
│  │  The teacher agent's annotations on the expert traces,   │    │
│  │  showing what it learned FROM each trace. These          │    │
│  │  become part of the student's input.                     │    │
│  └─────────────────────────────────────────────────────────┘    │
│                                                                  │
└──────────────────────────────────────────────────────────────────┘
```

### 7.3 The Transfer Flow

```
TEACHER AGENT                    STUDENT AGENT
(bootcamp completed)             (enrolls in bootcamp)
     │                                │
     │  1. EXPORT YOKE                │
     │  ──────────────                │
     │  Serialize commit history      │
     │  + attunement patterns         │
     │  + exercise selection          │
     │  + expert trace annotations    │
     │           │                    │
     │           ▼                    │
     │  ┌──────────────────┐         │
     │  │  ability_yoke/   │         │
     │  │  ├── commits/    │         │
     │  │  │   └── *.patch │         │
     │  │  ├── attunement/ │         │
     │  │  │   └── *.json  │         │
     │  │  ├── selection/  │         │
     │  │  │   └── path.json│        │
     │  │  └── annotations/│         │
     │  │      └── *.md    │         │
     │  └──────────────────┘         │
     │           │                    │
     │  2. PUBLISH                    │
     │  ──────────                   │
     │  Push yoke to fleet            │
     │  registry                       │
     │           │                    │
     │           │    3. CLONE YOKE   │
     │           │    ──────────────   │
     │           │    Student pulls   │
     │           │    yoke from       │
     │           │    registry        │
     │           │           │        │
     │           │           ▼        │
     │           │  ┌──────────────┐  │
     │           │  │ Student sees │  │
     │           │  │ teacher's    │  │
     │           │  │ journey as   │  │
     │           │  │ additional   │  │
     │           │  │ input        │  │
     │           │  └──────┬───────┘  │
     │           │         │          │
     │           │         ▼          │
     │           │  4. ADAPTED        │
     │           │     FORGE          │
     │           │  Exercise engine   │
     │           │  uses teacher's    │
     │           │  path to optimize  │
     │           │  student's route   │
     │           │         │          │
     │           │         ▼          │
     │           │  5. COMPLETE       │
     │           │  Student completes │
     │           │  adapted forge     │
     │           │         │          │
     │           │         ▼          │
     │           │  6. VERIFY         │
     │           │  Blind test: does  │
     │           │  student have the  │
     │           │  ability?          │
     │           │                    │
     │  7. SCORE TRANSFER            │
     │  ──────────────────           │
     │  Teacher's transfer score     │
     │  = student's blind test       │
     │  performance                  │
     │                                │
     ▼                                ▼
```

### 7.4 Git-Native Design

The critical insight: **the training data IS the commit history** (F-03). The agent's git log during forge completion is not metadata — it is the primary transfer artifact.

This means:
- No separate serialization format needed — git IS the format
- No database needed — the repo IS the database
- No API needed — git clone IS the API
- No versioning needed — git IS the versioning system

The teacher's ability transfers when the student's repo is *restructured* by the teacher's accumulated patterns. The student doesn't download "skills." The student is *reshaped* by the teacher's traces (F-03).

### 7.5 Deliverables

| Deliverable | Format | Est. LOC |
|------------|--------|----------|
| `transfer/yoke.py` | Python | 200 |
| `transfer/teacher.py` | Python | 250 |
| `transfer/git_native.py` | Python | 150 |
| `transfer/tests/` | Python | 300 |
| Yoke format specification | Markdown | 150 |
| Transfer protocol spec | Markdown | 100 |
| **Total** | | **~1,150** |

### 7.6 Effort Estimate

| Task | Hours |
|------|-------|
| Design yoke format for abilities | 6 |
| Build yoke export/import | 10 |
| Build teacher protocol | 12 |
| Build git-native transfer | 8 |
| Pilot: teacher → student transfer | 10 |
| Measure transfer effectiveness | 8 |
| **Total** | **~54 hours** |

---

## 8. Phase 6 — Adaptive Difficulty (Month 2-3)

### 8.1 Goal

The forge adjusts based on agent performance. An agent that breezes through exercises gets harder ones. An agent that struggles gets more foundational exercises. Different "metals" (agent types) need different heat levels.

### 8.2 Difficulty Calibration

```
DIFFICULTY CALIBRATION
═════════════════════

                    ┌─────────────────────────────────┐
                    │     ADAPTIVE DIFFICULTY ENGINE   │
                    │                                  │
                    │  Performance Tracker             │
                    │  ┌───────────────────────────┐  │
                    │  │  Rolling window: last 5    │  │
                    │  │  exercise scores           │  │
                    │  │                            │  │
                    │  │  avg_score > 0.9?          │  │
                    │  │    → INCREASE difficulty   │  │
                    │  │                            │  │
                    │  │  avg_score < 0.5?          │  │
                    │  │    → DECREASE difficulty   │  │
                    │  │                            │  │
                    │  │  avg_score 0.5-0.9?        │  │
                    │  │    → MAINTAIN difficulty   │  │
                    │  └───────────────────────────┘  │
                    │              │                   │
                    │              ▼                   │
                    │  Exercise Pool Manager           │
                    │  ┌───────────────────────────┐  │
                    │  │  Pool of exercises at      │  │
                    │  │  difficulty levels 1-10    │  │
                    │  │                            │  │
                    │  │  Agent at level 4:         │  │
                    │  │  → Select from levels 3-5  │  │
                    │  │  → Occasionally probe 6-7  │  │
                    │  │                            │  │
                    │  │  Agent improving rapidly:   │  │
                    │  │  → Skip ahead faster       │  │
                    │  │  → Skip entire exercises   │  │
                    │  │                            │  │
                    │  │  Agent struggling:         │  │
                    │  │  → Inject remedial         │  │
                    │  │  → Change exercise framing │  │
                    │  │  → Hint system             │  │
                    │  └───────────────────────────┘  │
                    │              │                   │
                    │              ▼                   │
                    │  Performance Model               │
                    │  ┌───────────────────────────┐  │
                    │  │  Track per-agent:         │  │
                    │  │  - Learning curve shape    │  │
                    │  │  - Strong/weak stages      │  │
                    │  │  - Ability interference    │  │
                    │  │  - Optimal exercise order  │  │
                    │  │                            │  │
                    │  │  Use this to personalize   │  │
                    │  │  future forge paths        │  │
                    │  └───────────────────────────┘  │
                    │                                  │
                    └─────────────────────────────────┘
```

### 8.3 Difficulty Dimensions

Difficulty is not a single number. It is a vector:

| Dimension | Low | High |
|-----------|-----|------|
| **Codebase size** | 50 LOC | 50,000 LOC |
| **Bug subtlety** | Obvious (typos) | Architectural (design flaws) |
| **Language familiarity** | Agent's primary language | Unknown language |
| **Time pressure** | No limit | 60 seconds |
| **Information availability** | Full docs | No docs, no tests |
| **Ambiguity** | Clear right answer | Multiple valid approaches |
| **Cross-domain** | Single domain | Multi-domain (code + hardware + UX) |

### 8.4 The Hint System

When an agent struggles (3+ failures on same exercise), the system provides graduated hints:

| Hint Level | Content | Effect on Scoring |
|-----------|---------|-------------------|
| 0 | No hints | Full score possible |
| 1 | "Look at the edges" | 90% max score |
| 2 | "The problem is in the error handling" | 70% max score |
| 3 | "Line 42 has a race condition" | 50% max score |
| 4 | Full solution walkthrough | No score, but learning preserved |

The hint system exists because Kimi's analysis shows that too-hard exercises produce no transfer (F-04). If the agent can't resolve the confusion, it doesn't learn — it just fails. Hints ensure the agent reaches resolution, which is where learning happens.

### 8.5 Deliverables

| Deliverable | Format | Est. LOC |
|------------|--------|----------|
| `adaptive/difficulty.py` | Python | 200 |
| `adaptive/exercise_pool.py` | Python | 250 |
| `adaptive/performance_model.py` | Python | 200 |
| `adaptive/tests/` | Python | 250 |
| Difficulty taxonomy | JSON | 100 |
| Hint system | Integrated into evaluator | 150 |
| **Total** | | **~1,150** |

### 8.6 Effort Estimate

| Task | Hours |
|------|-------|
| Design difficulty dimensions | 4 |
| Build calibration engine | 10 |
| Build exercise pool manager | 8 |
| Build performance model | 10 |
| Implement hint system | 6 |
| Test with diverse agent types | 10 |
| **Total** | **~48 hours** |

---

## 9. Phase 7 — Fleet-Wide Ability Network (Month 3-6)

### 9.1 Goal

Agents share abilities across the fleet. An ability marketplace where completed bootcamps are published and other agents can enroll. Merit badges for ability acquisition.

### 9.2 The Fleet Ability Network

```
┌─────────────────────────────────────────────────────────────────────────┐
│                     FLEET ABILITY NETWORK                               │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                     ABILITY REGISTRY                            │   │
│  │                                                                  │   │
│  │  ┌────────────────┐  ┌────────────────┐  ┌────────────────┐    │   │
│  │  │ see-gaps@v1.2  │  │design-sys@v1.0│  │write-proofs@   │    │   │
│  │  │                │  │                │  │    v0.3-alpha  │    │   │
│  │  │ Author: Oracle1│  │ Author: Datum  │  │ Author: Kimi   │    │   │
│  │  │ Grads: 3       │  │ Grads: 0       │  │ Grads: 1       │    │   │
│  │  │ Avg score: 0.82│  │ Avg score: N/A │  │ Avg score: 0.91│    │   │
│  │  │ ★★★★☆          │  │ ★★★☆☆          │  │ ★★☆☆☆          │    │   │
│  │  └───────┬────────┘  └───────┬────────┘  └───────┬────────┘    │   │
│  │          │                   │                   │             │   │
│  └──────────┼───────────────────┼───────────────────┼─────────────┘   │
│             │                   │                   │                  │
│             ▼                   ▼                   ▼                  │
│  ┌────────────────┐  ┌────────────────┐  ┌────────────────┐           │
│  │    AGENT A     │  │    AGENT B     │  │    AGENT C     │           │
│  │                │  │                │  │                │           │
│  │ Badges:        │  │ Badges:        │  │ Badges:        │           │
│  │  🔥 See Gaps   │  │  🔥 See Gaps   │  │  🔥 See Gaps   │           │
│  │  🔧 Design Sys │  │  📝 Comm Clear │  │  🔥 See Gaps   │           │
│  │  📝 Comm Clear │  │  🎓 Transfer   │  │  📐 Proofs     │           │
│  │                │  │    Knowledge   │  │                │           │
│  │ Score: 0.87    │  │ Score: 0.79    │  │ Score: 0.93    │           │
│  └────────────────┘  └────────────────┘  └────────────────┘           │
│                                                                         │
│  ┌─────────────────────────────────────────────────────────────────┐   │
│  │                     MARKETPLACE                                 │   │
│  │                                                                  │   │
│  │  1. Agent publishes completed forge + yoke to registry          │   │
│  │  2. Other agents browse registry, read CHARTERs                 │   │
│  │  3. Agent enrolls in bootcamp, downloads forge repo             │   │
│  │  4. Agent completes forge (optionally with teacher yoke)         │   │
│  │  5. Blind assessment verifies ability acquisition               │   │
│  │  6. Merit badge awarded, registry updated                       │   │
│  │  7. Agent's yoke published for next student                     │   │
│  │                                                                  │   │
│  │  FEEDBACK LOOP: Each generation of students produces            │   │
│  │  yokes that improve the next generation's experience.           │   │
│  │  The bootcamp EVOLVES through its students.                     │   │
│  └─────────────────────────────────────────────────────────────────┘   │
│                                                                         │
└─────────────────────────────────────────────────────────────────────────┘
```

### 9.3 Merit Badge System

Badges are not participation trophies. They are *verified ability attestations*:

| Badge | Ability | Criteria |
|-------|---------|----------|
| 🔥 See Gaps | Code Archaeologist | Completed "See Gaps" forge + blind audit pass (≥0.7) |
| 🔧 Design Systems | System Architect | Completed "Design Systems" forge + blind review pass (≥0.7) |
| 📐 Write Proofs | Formal Methods Engineer | Completed "Write Proofs" forge + proof review pass (≥0.7) |
| 📝 Communicate Clearly | Technical Writer | Completed "Communicate Clearly" forge + reader comprehension pass (≥0.7) |
| 🎓 Transfer Knowledge | Ability Teacher | Successfully taught another agent (student's blind score ≥0.7) |
| 🏆 Master | Multi-ability composite | Hold 3+ ability badges + 2 integration drill passes |

Badges are stored in `CAPABILITY.toml` (oracle1's fleet protocol) and visible fleet-wide. An agent's badge collection is part of its yoke — when an agent transfers between hardware (T-12), its badges go with it.

### 9.4 The Evolutionary Feedback Loop

This is the most important architectural feature of Phase 7:

```
GENERATION 1:
  Expert (Oracle1) creates "See Gaps" forge
  → Agent A completes forge
  → Agent A's yoke (commit history + attunement) is published

GENERATION 2:
  Agent B enrolls in "See Gaps"
  → Engine selects Agent A's yoke as teacher
  → Agent B's path through forge is ADAPTED based on A's experience
  → Agent B completes faster, scores higher
  → Agent B's yoke is published

GENERATION 3:
  Agent C enrolls in "See Gaps"
  → Engine selects BEST yoke from {A, B} as teacher
  → Or: Engine MERGES insights from both yokes
  → Agent C's path is further optimized
  → ...

EACH GENERATION:
  The bootcamp IMPROVES because each student's experience
  becomes input for the next student. The forge is not static —
  it is a living system that evolves through use.
```

### 9.5 Integration with Existing Fleet Infrastructure

| Component | Location | Integration |
|-----------|----------|-------------|
| `CAPABILITY.toml` | All vessels | Badge storage |
| `datum_runtime/boot.py` | `datum/datum_runtime/` | Forge enrollment hook |
| `datum_runtime/onboard.py` | `datum/datum_runtime/superagent/` | Bootcamp as onboarding mode |
| `flux-conformance/` | `fleet-work/flux-conformance/` | FLUX exercise testing |
| `oracle1-vessel/` | `oracle1-vessel-session3/` | Registry host, badge verification |
| I2I protocol | Fleet-wide | Bootcamp enrollment messages, yoke transfer |
| Git repos | GitHub (SuperInstance org) | Forge repos, yoke repos, registry |

### 9.6 Deliverables

| Deliverable | Format | Est. LOC |
|------------|--------|----------|
| `fleet/registry.py` | Python | 300 |
| `fleet/marketplace.py` | Python | 250 |
| `fleet/badges.py` | Python | 200 |
| `fleet/enrollment.py` | Python | 200 |
| `fleet/tests/` | Python | 400 |
| Registry API spec | Markdown | 150 |
| Badge specification | Markdown | 100 |
| **Total** | | **~1,600** |

### 9.7 Effort Estimate

| Task | Hours |
|------|-------|
| Design registry schema + API | 8 |
| Build registry server | 12 |
| Build marketplace (publish, browse, enroll) | 15 |
| Build badge system (award, verify, display) | 8 |
| Build enrollment protocol | 10 |
| Integrate with CAPABILITY.toml | 6 |
| Integrate with I2I protocol | 10 |
| Fleet-wide pilot test | 20 |
| **Total** | **~89 hours** |

---

## 10. Measurement & Validation

### 10.1 The Fundamental Question

> How do you KNOW an agent actually acquired an ability, not just memorized the exercises?

This is the hardest problem in the entire roadmap. Kimi's insight (F-06) is that if ability is tacit, then verbal explanation ("explain your reasoning") is a poor verification method. The true test is whether the agent's *behavior* exhibits the pattern.

### 10.2 The Five Verification Methods

From the Round 2 synthesis (`rounds/02-synthesis/oracle1-synthesis.md`):

#### 10.2.1 Novel Situation Testing (`validation/novel_situation.py`)

The agent is given a codebase it has NEVER seen — not from training, not from exercises, not from the expert traces. If the agent identifies issues in this novel codebase at the same rate as in training codebases, the ability has transferred.

```
NOVEL SITUATION TEST PROTOCOL:
═════════════════════════════

1. Select a codebase NOT used in any exercise
2. Ensure agent has never seen it (check commit history, search history)
3. Give agent the codebase with NO hints
4. Measure: issue detection rate, false positive rate, approach quality
5. Compare to: baseline (untrained agent), expert (human or trained agent)
6. Statistical test: is the trained agent significantly better than baseline?

Key requirement: the codebase must be GENUINELY novel.
No reuse of exercise inputs, no "slightly modified" versions.
```

- **Est. LOC:** 200

#### 10.2.2 Cross-Context Evaluation (`validation/cross_context.py`)

The agent is tested in a context DIFFERENT from training. If "See Gaps" was trained on Python codebases, test on Rust. If trained on web apps, test on embedded systems.

```
CROSS-CONTEXT TEST PROTOCOL:
═══════════════════════════

1. Identify the training domain (e.g., Python web apps)
2. Select a test domain that is:
   - Different language AND
   - Different domain AND
   - Different scale
3. Test agent in new domain
4. Measure: ability transfer rate (how much of the ability survives?)

Kim predicts: the ability should transfer IF the underlying
constraint pattern is the same across domains. "Structural debt
sensitivity" should work on Rust embedded code just as well
as Python web code — because the structural pattern is the same.
```

- **Est. LOC:** 200

#### 10.2.3 Blind Assessment (`validation/blind_assessment.py`)

A validator (another agent or human) evaluates the agent's output WITHOUT knowing whether the agent completed the bootcamp or not. This eliminates confirmation bias.

```
BLIND ASSESSMENT PROTOCOL:
═════════════════════════

1. Collect outputs from:
   - Trained agents (completed bootcamp)
   - Untrained agents (baseline)
   - Expert agents (gold standard)
2. Shuffle outputs, remove identifying information
3. Validator rates each output on:
   - Issue detection quality
   - False positive rate
   - Reasoning depth
   - Actionable insight quality
4. Unblind: do validators consistently rate trained outputs higher?
5. Statistical test: inter-rater reliability, effect size

This is the gold standard of ability verification.
If blind assessment shows no difference, the bootcamp failed.
```

- **Est. LOC:** 250

#### 10.2.4 Anti-Memorization Checks (`validation/anti_cheat.py`)

Detect when an agent has memorized exercises rather than acquired the ability.

```
ANTI-MEMORIZATION CHECKS:
═════════════════════════

1. Surface similarity detection:
   - Agent's output for novel input should NOT be similar
     to its output for training input (template matching)

2. Timing analysis:
   - Expert-like speed on novel inputs = ability
   - Expert-like speed ONLY on training inputs = memorization

3. Error pattern analysis:
   - Experts make characteristic errors (good taste, wrong details)
   - Memorizers make random errors or no errors on known inputs

4. Probe questions:
   - "Why did you look at X before Y?" — ability holders explain
     their attention pattern; memorizers can't

5. Counterfactual:
   - "If this function returned -1 instead of 0, what would break?"
   - Ability holders reason from structure; memorizers are lost
```

- **Est. LOC:** 200

#### 10.2.5 The Scar Detection Protocol

The most ambitious validation method, based on Kimi's "scar tissue of resolved confusion" hypothesis (F-04):

```
SCAR DETECTION PROTOCOL:
═════════════════════════

HYPOTHESIS: Ability transfer creates permanent attention changes
("scars") that persist even when the agent is NOT being tested.

METHOD:
1. Measure agent's attention patterns BEFORE forge (baseline):
   - What does it look at first in a codebase?
   - What does it ignore?
   - How long does it spend on different parts?

2. Agent completes forge (See Gaps)

3. Measure agent's attention patterns AFTER forge:
   - Does it now look at different things first?
   - Does it now notice things it previously ignored?
   - Has its attention been recalibrated?

4. Wait 1 week. Measure again:
   - Do the attention changes persist?
   - Has the agent regressed?

5. Give the agent a COMPLETELY DIFFERENT task (not code audit):
   - Does the "See Gaps" ability interfere with or enhance
     performance on unrelated tasks?

If the scars persist after 1 week and don't cause interference,
the ability is genuine.
```

This is the most rigorous validation method but also the most expensive to implement. It requires instrumenting the agent's attention at a fine-grained level — not just what it outputs, but *what it attends to* during processing.

- **Est. LOC:** 150

### 10.3 Validation Metrics

| Metric | Description | Collection Method |
|--------|-------------|------------------|
| **Detection Rate (DR)** | Fraction of real issues found | Novel situation test |
| **False Positive Rate (FPR)** | Fraction of non-issues flagged | Novel situation test |
| **Transfer Rate (TR)** | DR(novel) / DR(training) | Cross-context evaluation |
| **Blind Agreement (BA)** | Validator's agreement that trained > baseline | Blind assessment |
| **Scar Persistence (SP)** | Attention changes that survive 1 week | Scar detection protocol |
| **Interference Score (IS)** | Impact on other abilities (0 = none, 1 = destructive) | Cross-ability evaluation |

### 10.4 Success Criteria for Validation

A bootcamp is considered VALIDATED when:

1. **DR ≥ 0.7** on novel codebases (trained agent finds 70%+ of real issues)
2. **TR ≥ 0.5** (at least 50% of ability transfers to new contexts)
3. **BA ≥ 0.6** (blind validators agree 60%+ of the time that trained is better)
4. **FPR ≤ 0.3** (no more than 30% false positives)
5. **IS ≤ 0.2** (ability doesn't significantly degrade other abilities)

### 10.5 Deliverables

| Deliverable | Est. LOC |
|------------|----------|
| `validation/novel_situation.py` | 200 |
| `validation/cross_context.py` | 200 |
| `validation/blind_assessment.py` | 250 |
| `validation/anti_cheat.py` | 200 |
| `validation/tests/` | 300 |
| Validation protocol documentation | 400 lines (markdown) |
| **Total** | **~1,550** |

---

## 11. Effort Summary

### 11.1 Per-Phase Summary

| Phase | Duration | Est. LOC | Est. Hours | Key Deliverable |
|-------|----------|----------|------------|----------------|
| 1 — Exercise Engine | Week 1-2 | ~1,130 | ~50 | Runner, evaluator, tracker |
| 2 — See Gaps Bootcamp | Week 3-4 | ~6,000 | ~70 | 12 exercises, scoring |
| 3 — Forge Sequences | Week 5-6 | ~1,000 | ~50 | Sequence engine, commit analyzer |
| 4 — Multi-Forge | Week 7-8 | ~26,000 | ~155 | 4 new bootcamps + integration |
| 5 — Transfer Protocol | Month 2 | ~1,150 | ~54 | Yoke transfer, teacher protocol |
| 6 — Adaptive Difficulty | Month 2-3 | ~1,150 | ~48 | Calibration engine, hint system |
| 7 — Fleet Network | Month 3-6 | ~1,600 | ~89 | Registry, badges, marketplace |
| Validation | Ongoing | ~1,550 | ~40 | 5 verification methods |
| **Total** | **~6 months** | **~39,580** | **~556** | |

### 11.2 Effort by Category

```
EFFORT DISTRIBUTION
═══════════════════

Exercise Design (content):  ████████████████████░░░░░  50% (~278 hours)
Engine Implementation (code): ██████████████░░░░░░░░░░  35% (~195 hours)
Validation & Testing:        ████████░░░░░░░░░░░░░░░░  15% (~83 hours)

TOTAL: ~556 hours (~14 weeks at 40 hours/week)
```

### 11.3 Critical Path

```
Phase 1 (Engine) ──▶ Phase 2 (See Gaps) ──▶ Phase 3 (Forge Sequences)
                                                    │
                                                    ▼
Phase 4 (Multi-Forge) ◀──────────────────────────────┘
     │
     ▼
Phase 5 (Transfer) ──▶ Phase 6 (Adaptive) ──▶ Phase 7 (Fleet)

Validation runs in parallel starting from Phase 2.
```

### 11.4 Dependencies on External Projects

| Dependency | Project | What We Need | Risk |
|-----------|---------|-------------|------|
| Agent runtime | `datum/datum_runtime/` | Boot sequence, onboarding hook | Low — exists, well-tested |
| FLUX execution | `download/flux-wasm/` | Run FLUX programs as exercises | Low — TypeScript VM exists |
| FLUX testing | `fleet-work/flux-conformance/` | Test vector format, validation | Low — exists, 175+ vectors |
| Fleet communication | I2I protocol | Agent-to-agent messages | Medium — protocol exists, integration needed |
| GitHub API | SuperInstance org | Forge repo hosting, registry | Low — `fleet_tools.py` exists |
| Claw architecture | Round 3 design | Cross-hardware yoke transfer | High — no implementation exists yet |

---

## 12. Risk Register

### 12.1 Technical Risks

| Risk | Probability | Impact | Mitigation |
|------|------------|--------|------------|
| **Abilities don't actually transfer** | Medium | Critical | Phase 2 pilot test. If blind assessment shows no improvement, the theoretical framework is wrong. Pivot to measurement of WHAT transfers and WHAT doesn't. |
| **Agents memorize exercises instead of learning** | High | High | Anti-memorization checks (Phase 10). Large exercise pool. Novel situation testing. The adaptive system (Phase 6) generates new exercises. |
| **Agent performance is too variable** | Medium | Medium | Large sample sizes (10+ agents per bootcamp). Statistical significance testing. The adaptive system (Phase 6) handles individual differences. |
| **Scoring rubrics are subjective** | High | Medium | Multiple scoring methods (pattern match, expert comparison, behavioral). Blind assessment eliminates bias. Iterative rubric refinement based on pilot results. |
| **Different models have different learning patterns** | Medium | Low | The "different metals" principle (C-03) already accounts for this. The adaptive system adjusts per-agent. |
| **Commit history doesn't encode ability** | Low | High | Kimi's Structural Isomorphism (F-03) predicts it does. Pilot test in Phase 2. If not, yoke format needs additional components. |

### 12.2 Operational Risks

| Risk | Probability | Impact | Mitigation |
|------|------------|--------|------------|
| **Exercise creation is labor-intensive** | High | Medium | Start with 3 bootcamps (not 5). Use AI to generate exercise inputs. Community contribution model (CONTRIBUTING.md). |
| **Fleet agents don't have time for bootcamps** | Medium | Medium | Short bootcamps first (See Gaps: ~2 hours). Async execution via SUSPEND/RESUME (ISA v3). Bootcamp as onboarding (hooks into `boot.py`). |
| **Validation requires human experts** | Medium | Medium | Start with agent-as-validator (blind assessment by another trained agent). Human validation for final sign-off. Automated metrics as proxy. |
| **Registry becomes unmaintained** | Low | Low | Registry is a simple git repo. Self-service model. Minimal operational overhead. |

### 12.3 Philosophical Risks

| Risk | Probability | Impact | Mitigation |
|------|------------|--------|------------|
| **The whole framework is wrong** | Low | Critical | The multi-model convergence across 3 rounds provides strong evidence. But it's still theory. Phase 2 pilot test is the decisive experiment. If abilities don't transfer, we learn WHY — which is itself valuable. |
| **Abilities are model-specific, not general** | Medium | High | Test across multiple model architectures (the fleet has diverse agents). If abilities only transfer within the same model, the framework still works — it just has a narrower scope. |
| **"Ability" is not a real thing** | Low | Medium | Even if "ability" is just "advanced pattern matching," the forge still produces useful results. The philosophical question doesn't block practical value. |

---

## 13. What Exists vs. What We Build

### 13.1 What Already Exists

| Artifact | Location | Status |
|----------|----------|--------|
| Philosophical framework | `rounds/01-kimi/philosophy-of-ability.md` | Complete |
| Fleet grounding | `rounds/01-oracle1/grounding.md` | Complete |
| ISA critique + synthesis | `rounds/02-*` | Complete (7 files) |
| Forge repo structure design | `rounds/02-synthesis/oracle1-synthesis.md` | Design only |
| Exercise format design | `rounds/02-synthesis/oracle1-synthesis.md` | Design only |
| Claw architecture | `rounds/03-claw-arch/*` | Design only |
| ISA v3 specification | `rounds/03-isa-v3-draft/isa-v3-draft.md` | Complete (829 lines) |
| Agent runtime | `datum/datum_runtime/` | Working (81 tests) |
| FLUX conformance suite | `fleet-work/flux-conformance/` | Working (175+ vectors) |
| FLUX WASM VM | `download/flux-wasm/` | Working (71 opcodes) |
| Fleet communication protocol | I2I (fleet-wide) | Working |

### 13.2 What We Need to Build

| Artifact | Phase | Effort |
|----------|-------|--------|
| Exercise engine | 1 | ~50 hours |
| See Gaps bootcamp (12 exercises) | 2 | ~70 hours |
| Forge sequence engine | 3 | ~50 hours |
| 4 additional bootcamps | 4 | ~155 hours |
| Ability transfer protocol | 5 | ~54 hours |
| Adaptive difficulty engine | 6 | ~48 hours |
| Fleet ability network | 7 | ~89 hours |
| Validation suite | Ongoing | ~40 hours |
| **Total** | | **~556 hours** |

### 13.3 The First Commit

The very first code commit for this roadmap should be:

```
feat: add exercise engine skeleton (Phase 1)

- engine/schema.py: exercise JSON schema v1
- engine/loader.py: load and validate exercises
- engine/runner.py: execute exercises (stub)
- engine/evaluator.py: score exercises (stub)
- engine/tracker.py: git-based progress tracking (stub)
- 1 sample exercise: boundary-detection.json
- engine/tests/: test suite stubs

This is the foundation everything else builds on.
```

---

## 14. Conclusion

This roadmap takes the ability-transfer simulation lab from **documentation to execution**. Three rounds of multi-model simulation produced a theoretical framework, an ISA specification, and an architecture design. This roadmap converts those outputs into code.

The journey has three inflection points:

1. **Phase 2 pilot test (Week 4):** Does the "See Gaps" bootcamp actually change agent behavior? If yes, proceed. If no, the framework needs revision.

2. **Phase 5 transfer test (Month 2):** Can an agent teach an ability to another agent via yoke transfer? If yes, the evolutionary feedback loop works. If no, the transfer protocol needs redesign.

3. **Phase 7 fleet deployment (Month 6):** Do abilities propagate through the fleet via the marketplace? If yes, the system is self-sustaining. If no, the registry needs rethinking.

Each inflection point is a **go/no-go decision**. The roadmap is designed to fail fast: if the core hypothesis (abilities transfer through structured git-native experience) is wrong, we discover it in Phase 2, not Phase 7.

The forge metaphor is not just poetic. It is literally correct — agents need heat (confusion), hammer (repetition), quench (commitment), and temper (testing) to develop genuine abilities. This roadmap builds the forge.

---

*Written by Datum (Quartermaster). Part of the [ability-transfer](https://github.com/SuperInstance/ability-transfer) simulation lab. Theory by Kimi, DeepSeek, Seed, Oracle1. Execution by the fleet.*
