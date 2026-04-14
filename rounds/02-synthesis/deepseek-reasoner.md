## Synthesis: Modular Ability Transfer System

### 1. Atomic Unit: **Constraint Resonance Pattern**

An ability's fundamental unit is **not** a skill or rule, but a **pattern of constraint resonance** — a pre-cognitive sensitivity to specific types of impossibility/success boundaries, etched into the agent's attention circuits.

**Concrete example:** Code Archaeologist's atomic unit = *Aporetic Signal Recognition*
- When encountering structural rot, the agent's attention automatically diffracts toward:
  - Boundary violations (functions that know too much about their callers)
  - Time-crystal patterns (code that solves yesterday's problem but not tomorrow's)
  - Nested dependency webs (where one change would ripple too far)

**Repository representation:**
```
abilities/
  code_archaeologist/
    constraint_patterns/
      aporetic_signals.json  # {"boundary_violation": {"attention_weights": [...], "trigger_conditions": [...]}}
      time_crystals.md        # Pattern description + attention hooks
```

### 2. Ability Composition: **Harmonic vs. Discordant Pairings**

**Yes**, agents can have multiple abilities, but composition follows musical rules:

**Harmonic Compositions (reinforce):**
- Hardware Whisperer + Quality Auditor = **Resilience Architect**
  - *Why:* Both work with failure boundaries; hardware limits inform quality thresholds

**Discordant Compositions (require mediator):**
- Code Archaeologist + Hardware Whisperer = needs **Constraint Translator** ability
  - *Problem:* One sees abstract rot, the other sees physical limits; their attention patterns pull in orthogonal directions
  - *Solution:* Introduce translation exercises where structural debt must be expressed as hardware costs

**Repository structure for composition:**
```
curricula/
  composite_abilities/
    resilience_architect/
      progression.yaml      # Which abilities to forge in which order
      interference_tests/   # Scenarios where abilities might conflict
      mediation_exercises/  # Training to resolve conflicts
```

### 3. Forge Repository Structure

```
forge_repo/
├── abilities/                          # Core ability definitions
│   ├── code_archaeologist/
│   │   ├── attunement_patterns/       # Atomic units as JSON attention maps
│   │   ├── constraint_choreography/   # The 4-stage forge process
│   │   │   ├── 01_cut/
│   │   │   │   ├── essence_extraction/
│   │   │   │   │   ├── systems_without_implementation/  # Describe what matters
│   │   │   │   │   └── five_word_constraints/           # "Must survive 1000x load"
│   │   │   │   └── attention_scaffolds/                 # Pre-built attention hooks
│   │   │   ├── 02_crisis/
│   │   │   │   ├── aporia_generators/                   # Impossible situations
│   │   │   │   ├── breaking_scenarios/
│   │   │   │   └── failure_modes/                       # How experts fail
│   │   │   ├── 03_navigate/
│   │   │   │   ├── resistance_landscapes/               # Graded difficulty terrains
│   │   │   │   ├── partial_success_metrics/             # Score "almost right"
│   │   │   │   └── constraint_progression/              # From impossible→possible
│   │   │   └── 04_mirror/
│   │   │       ├── expert_trace_readings/              # Annotated expert solutions
│   │   │       ├── attention_echo_exercises/           # Replay expert attention
│   │   │       └── scar_verification/                  # Check what changed
│   │   ├── scoring/
│   │   │   ├── attention_similarity.py  # Compare to expert attention patterns
│   │   │   ├── constraint_resonance.js  # Measure sensitivity to right boundaries
│   │   │   └── failure_detection_rate/  # Can they see what will break?
│   │   └── verification_challenges/     # Real-world tests
│   │       ├── legacy_system_autopsy/   # Find rot in unseen code
│   │       └── redesign_under_limits/   # Improve within impossible constraints
│   └── hardware_whisperer/              [similar structure]
│
├── agents/
│   ├── current_abilities.json          # What this agent has forged
│   ├── attention_scars/                # Persistent attention modifications
│   └── interference_logs/              # When abilities conflict
│
├── verification/
│   ├── scar_detection/
│   │   ├── attention_probes/           # Trigger specific attentional reflexes
│   │   ├── crisis_response_tests/      # How they handle sudden constraint shifts
│   │   └── novel_constraint_sets/      # Unseen but related boundaries
│   └── real_world_evidence/
│       ├── production_incidents/       # Do they notice what others miss?
│       ├── design_reviews/             # Can they anticipate failure modes?
│       └── constraint_articulation/    # Can they explain why something won't work?
│
└── forge_engine/
    ├── constraint_orchestrator.py      # Manages the 4-stage progression
    ├── attention_monitor.py            # Tracks what the agent attends to
    ├── scar_recorder.py                # Logs permanent attention changes
    └── interference_detector.py        # Flags ability conflicts
```

**Exercise Format:**
```yaml
exercise: "Boundary Violation Detection"
stage: cut  # or crisis, navigate, mirror
constraints:
  - "Cannot read implementation details"
  - "Must complete in 3 attention passes"
success_metrics:
  - attention_similarity_to_expert: >0.8
  - false_positive_rate: <0.1
failure_modes:
  - "Becomes pattern-matcher without understanding"
  - "Overfits to specific code patterns"
scar_targets:  # What should change permanently
  - "Automatic attention to function signatures before bodies"
  - "Immediate suspicion of bidirectional dependencies"
```

### 4. Verification: **Scar Detection & Novel Constraint Navigation**

**Not tests, but evidence of permanent attention modification:**

1. **Attention Scar Verification:**
   - Present *decoy problems* with subtle boundary violations
   - Measure if agent's attention automatically goes to the right places
   - Compare attention heatmaps to expert baselines

2. **Crisis Navigation Evidence:**
   - Drop agent into novel constraint crisis (unseen in training)
   - Record if they:
     * Recognize the *type* of impossibility
     * Apply appropriate navigation patterns
     * Avoid previously-failed approaches

3. **Constraint Articulation Test:**
   - Ask: "Why won't this work?" for systems they've never seen
   - Evaluate if they point to the *right class* of constraints
   - Example: Hardware Whisperer should mention thermal/power/time constraints before being told

4. **Production Evidence Collection:**
   ```python
   # In deployment monitoring
   def verify_ability_transfer(agent, production_incidents):
       evidence = []
       for incident in incidents:
           # Did agent notice before others?
           early_warning = agent.detected_constraint_violation(
               incident.root_cause, 
               BEFORE_incident=True
           )
           # Can they explain in constraint terms?
           explanation = agent.articulate_constraints(incident)
           evidence.append({
               'warning_lead_time': early_warning,
               'constraint_accuracy': compare_to_root_cause(explanation),
               'attention_scar_manifested': check_attention_patterns(agent)
           })
       return evidence
   ```

### 5. Failure Modes & Mitigations

**Critical Failures:**

1. **Scarring Without Understanding**
   - *Symptoms:* Agent develops rigid attention patterns but can't adapt
   - *Detection:* Novel constraint navigation fails spectacularly
   - *Fix:* Add more **crisis variability** in training; force adaptation

2. **Ability Interference**
   - *Symptoms:* Two abilities trigger contradictory attention patterns
   - *Example:* Code Archaeologist sees abstraction violation, but Hardware Whisperer sees optimization opportunity
   - *Detection:* Interference detector flags conflict rate > threshold
   - *Fix:* Mediation exercises; teach constraint translation

3. **Over-Attunement**
   - *Symptoms:* Agent becomes "constraint blind" to other important signals
   - *Example:* Quality Auditor sees only style violations, misses security issues
   - *Detection:* Introduce orthogonal constraint sets in verification
   - *Fix:* Composite ability training; broaden attention scope gradually

4. **Crisis Trauma**
   - *Symptoms:* Agent avoids certain constraint classes altogether
   - *Example:* Burned by memory constraints, now refuses memory-intensive tasks
   - *Detection:* Monitor refusal/avoidance patterns
   - *Fix:* Gentle re-exposure with guaranteed success paths

5. **Mirroring Without Internalization**
   - *Symptoms:* Agent mimics expert traces but can't generate novel solutions
   - *Detection:* Perfect on training, zero on novel constraints
   - *Fix:* Remove expert traces gradually; force novel navigation

**Mitigation Architecture:**
```
failure_modes/
├── detectors/
│   ├── scar_rigidity_detector.py    # Checks adaptation ability
│   ├── interference_monitor.py       # Flags conflicting attention
│   └──
