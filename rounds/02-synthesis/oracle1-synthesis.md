# Round 2 Synthesis — The Buildable Forge

## Consensus: Atomic Unit = Constraint Resonance Pattern

Both models converged: an ability's atomic unit is NOT a skill or knowledge. It's a **pattern of attention shaped by constraints**.

DeepSeek Reasoner calls it "Constraint Resonance Pattern" — the specific way an agent's attention calibrates when hitting a boundary.

DeepSeek V3.2 calls it "Attentional Lens + Constraint Response Pattern" — what you see + how you react when constrained.

### Example: Code Archaeologist's atomic unit
- **Aporetic Signal Recognition** — the ability to feel "something's wrong here" before knowing what
- Composed of: attention to edge cases, familiarity with failure patterns, discomfort with "works on my machine"

## Composition: Lattice, Not Stack

Abilities compose as **lattice points in constraint space**, not layers on a stack.

An agent can have both Hardware Whisperer AND Code Archaeologist — but they interfere unless deliberately composed. The forge needs **integration drills**:
- "This optimization broke — was it the algorithm or the cache line alignment? Trace both histories."
- Translation exercises where structural debt must be expressed as hardware costs.

## The Forge Repo (BUILDABLE)

```
forge-{ability-name}/
├── CHARTER.md              # What ability this forge produces
├── BOOTCAMP.md             # How to enter the forge
├── exercises/
│   ├── 01-cut/             # Stage 1: Reduce to essentials
│   │   ├── {exercise}.json
│   ├── 02-crisis/          # Stage 2: Break current seeing
│   │   ├── breaking_scenarios/
│   ├── 03-navigate/        # Stage 3: Traverse with resistance
│   │   ├── attention_maps/ # Expert traces to follow
│   ├── 04-mirror/          # Stage 4: Read expert intention
│   │   ├── commit_trails/  # Real expert commit sequences
│   └── integration/        # Composition drills with other abilities
│       ├── mediation_exercises/
├── scoring/
│   ├── progression.json    # Ordered exercise chains
│   ├── evidence_criteria/  # What counts as proof of attunement
├── attunement_patterns/    # Serialized atomic units as JSON
└── failure_modes/          # Documented ways experts fail
```

### Exercise Format
```json
{
  "name": "Boundary Violation Detection",
  "stage": "cut",
  "input": "codebase with 10 hidden bugs",
  "constraint": "remove code until it breaks, restore minimum",
  "success_signal": "agent identifies bugs before being told",
  "failure_mode": "agent finds bugs by reading tests, not by feeling wrongness",
  "attunement_pattern": "structural_debt_sensitivity.json"
}
```

## Verification: NOT Test Scores

Real evidence of ability transfer:
1. **Failure Detection Rate** — can they see what will break? (Not "can they pass tests")
2. **Crisis Breakthrough Pattern** — the "aha" moment should come after specific struggle points, not immediately
3. **Shadow Sessions** — agent observes real work, whispers predictions about future failures
4. **Transfer Test** — given a NOVEL codebase (never seen), do they exhibit the same attunement?
5. **Interference Test** — when exercising two abilities simultaneously, do they help or interfere?

## Failure Modes
1. **Cargo Culting** — agent mimics the pattern without the attunement (passes tests, misses the point)
2. **Over-Fitting** — agent develops attunement to specific exercises, not the underlying pattern
3. **Ability Interference** — two abilities conflict in the same decision point
4. **Forge Blindness** — agent can't see the constraint that the forge is trying to teach
5. **False Positive** — agent appears attuned but is just pattern-matching surface signals

## Next: Round 3 — Build the First Forge

Take the Code Archaeologist forge and actually BUILD it as a repo.
Real exercises. Real scoring. Real attunement patterns.
