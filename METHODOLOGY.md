# METHODOLOGY.md — Multi-Model Simulation Protocol

This document describes the methodology used in the ability-transfer simulation lab: how rounds are structured, why multiple models are used, how synthesis works, and what quality criteria govern the process.

---

## 1. Why Multi-Model Simulation?

### 1.1 The Problem with Single-Model Research

A single AI model, no matter how capable, has systematic blind spots shaped by its training data, optimization objectives, and architectural constraints. When a single model generates ideas, critiques designs, or proposes solutions, the output is bounded by that model's particular way of seeing the world. This creates two risks:

1. **False confidence**: Ideas that seem robust within one model's frame may collapse when viewed from a different angle
2. **Missed alternatives**: The model's blind spots become the research program's blind spots

### 1.2 The Multi-Model Solution

This lab addresses these risks by running **parallel simulations across four models with deliberately different cognitive styles**. Each model receives the same input (research question, previous round outputs) but produces independent analysis. The key insight is that **convergence across different cognitive architectures provides stronger evidence than agreement within one**.

This is analogous to peer review in academic research, but at the level of cognitive architecture rather than human expertise. When four models with different training objectives independently arrive at the same conclusion, it carries far more weight than any single model's opinion.

### 1.3 Why These Specific Models?

| Model | Cognitive Style | Training Objective | What It Brings |
|-------|----------------|-------------------|----------------|
| **Seed** | Creative / Generative | Maximize novelty and breadth | Unconstrained thinking that breaks established patterns. Proposes ideas that no "reasonable" model would suggest. Useful for avoiding local optima. |
| **Kimi** | Philosophical / Analytical | Deep reasoning and conceptual clarity | Deconstructs assumptions and grounds analysis in first principles. Excels at identifying when creative ideas rest on shaky foundations. Acts as the critical voice. |
| **DeepSeek** | Engineering / Synthesis | Practical problem-solving | Transforms abstract insights into concrete, buildable specifications. Identifies structural trade-offs and implementation constraints. Bridges the gap between philosophy and code. |
| **Oracle1** | Grounding / Operational | Fleet management and feasibility | Filters everything through operational reality — what hardware do we have, what can our fleet actually run, what are the resource constraints. Acts as the reality check. |

The selection is not arbitrary. Each model compensates for the others' weaknesses:
- Seed's unconstrained creativity is checked by Kimi's philosophical rigor
- Kimi's theoretical depth is grounded by Oracle1's operational constraints
- DeepSeek's engineering pragmatism ensures ideas are actually buildable
- Oracle1's conservatism prevents over-engineering

---

## 2. Round Structure

### 2.1 Anatomy of a Round

Each simulation round follows this structure:

```
rounds/{NN}-{name}/
├── {model1}.md           # Model 1's independent output
├── {model2}.md           # Model 2's independent output
├── {model3}.md           # Model 3's independent output
├── {model4}.md           # Model 4's independent output (if applicable)
└── oracle1-synthesis.md  # Cross-model consensus document
```

### 2.2 Round Lifecycle

A round progresses through five phases:

```
Phase 1: SEED
    │  Oracle1 formulates the research question based on previous round outputs
    │  Question is specific enough to be actionable, open enough to allow diverse responses
    ▼
Phase 2: PARALLEL GENERATION
    │  Each model receives the SAME input (seed question + previous context)
    │  Models generate outputs INDEPENDENTLY (no cross-model visibility)
    │  This ensures genuine cognitive diversity
    ▼
Phase 3: SYNTHESIS
    │  Oracle1 reads ALL model outputs and produces oracle1-synthesis.md
    │  Synthesis follows the fixed format (see §2.3)
    │  Key: identifying convergence vs. divergence
    ▼
Phase 4: INTEGRATION
    │  Synthesis findings feed into subsequent rounds as input context
    │  Action items become the seed questions for the next round
    ▼
Phase 5: ARCHIVAL
       Round files are committed and tagged
       Findings catalog updated
       Status tracked in README.md
```

### 2.3 Oracle1 Synthesis Format

The synthesis document is the critical artifact of each round. It follows a fixed format to ensure consistency and comparability across rounds:

```markdown
# Oracle1 Synthesis — {Round Name}

## Consensus Hits (all models agree)
1. {Finding} — {brief evidence from each model}

## Unique Gold Veins
### {Model}: {Insight Title}
- {Description}
- Why it matters

## Divergence Points
- {Where models disagreed}
- {Why the disagreement exists}
- {Which position is stronger and why}

## Action Items (priority order)
1. {Action} — {source, rationale}
```

### 2.4 Round Types

Not all rounds follow the same pattern. The lab uses three types:

| Type | Purpose | Example | Models Used |
|------|---------|---------|-------------|
| **Inquiry** | Open exploration of a research question | Round 1: "What are abilities?" | All 4 models independently |
| **Critique** | Structured evaluation of a design artifact | Round 2a: "What's wrong with FLUX ISA v2?" | All 4 models independently |
| **Synthesis** | Merging perspectives into a buildable spec | Round 2b: "How do abilities transfer?" | DeepSeek R + DeepSeek V3 + Oracle1 |
| **Design** | Concrete architecture for a specific system | Round 3a: "Design the Claws architecture" | All 4 models independently |
| **Specification** | Formal documentation of a design | Round 3b: ISA v3 draft | Datum (Quartermaster) |

---

## 3. The Handoff Protocol

### 3.1 How Context Flows Between Rounds

```
Round N Output
    │
    ├── model-outputs/     ← Individual model analyses (diverse perspectives)
    └── synthesis.md       ← Oracle1's consensus document (integrated view)
         │
         ├── Consensus Hits    → Input to Round N+1 (high confidence)
         ├── Unique Gold Veins → Optional input to Round N+1 (exploratory)
         ├── Divergence Points → Seed question for Round N+1 (resolution needed)
         └── Action Items      → Direct input to Round N+1 (what to build next)
```

### 3.2 Handoff Rules

1. **Full context, not summaries**: Each model in Round N+1 receives the COMPLETE Round N outputs, not Oracle1's summary alone. This prevents synthesis bias from constraining fresh analysis.

2. **Independent generation**: Models do NOT see each other's outputs within a round. This is critical — if Seed sees Kimi's critique before writing, Seed will adjust to address Kimi's concerns, and we lose the cognitive diversity.

3. **Oracle1 sees everything**: Oracle1's synthesis role requires full visibility into all model outputs. Oracle1 is the only model with cross-model perspective within a round.

4. **Cumulative accumulation**: Round N+1 receives not just Round N outputs, but all previous round outputs. This allows models to trace the evolution of ideas and identify patterns across rounds.

### 3.3 Information Flow Diagram

```
  ┌──────────────────────────────────────────────────┐
  │               ROUND N CONTEXT                    │
  │  (All previous round outputs + seed question)    │
  └──────┬──────────┬──────────┬──────────┬──────────┘
         │          │          │          │
    ┌────▼────┐┌────▼────┐┌────▼────┐┌────▼────┐
    │  Seed   ││  Kimi   ││DeepSeek ││ Oracle1 │
    │(blind)  ││(blind)  ││(blind)  ││(sees    │
    │         ││         ││         ││ all)    │
    └────┬────┘└────┬────┘└────┬────┘└────┬────┘
         │          │          │          │
         └──────────┼──────────┼──────────┘
                    │          │
               ┌────▼──────────▼────┐
               │  Oracle1 Synthesis │
               │  (consensus +      │
               │   divergence +     │
               │   action items)    │
               └────────┬───────────┘
                        │
                   ┌────▼────┐
                   │ROUND N+1│
                   │ SEED    │
                   └─────────┘
```

---

## 4. Quality Criteria

### 4.1 Evaluating Individual Model Outputs

Each model output is evaluated against five criteria:

| Criterion | Description | Weight | How to Detect |
|-----------|-------------|--------|---------------|
| **Specificity** | Concrete examples, not vague principles | High | "Use DEADLINE_BEFORE with ms timestamps" vs. "Add temporal awareness" |
| **Actionability** | Can you build something from this? | High | Includes code, data structures, or step-by-step procedures |
| **Surprise** | Did it challenge assumptions? | Medium | Proposes something that wasn't in the seed question |
| **Grounding** | Connected to real constraints? | Medium | References specific hardware, fleet resources, or implementation details |
| **Composition** | Integrates with previous findings? | Medium | Builds on or critiques Round N-1 findings explicitly |

### 4.2 Evaluating Synthesis Quality

Oracle1 synthesis documents are evaluated against:

| Criterion | Description | How to Detect |
|-----------|-------------|---------------|
| **Completeness** | All model outputs represented | Every model's key finding appears in either consensus or unique veins |
| **Honesty about divergence** | Disagreements surfaced, not hidden | Divergence section exists and explains *why* models disagree |
| **Prioritization** | Action items ordered by impact/feasibility | High-priority items address consensus hits first |
| **Progress** | Advances beyond previous round | New insights, not just restatement of previous round |

### 4.3 Red Flags

The following indicate problems with a round:

| Red Flag | Meaning | Remedy |
|----------|---------|--------|
| No consensus hits | Models are too far apart or question is too vague | Narrow the research question, provide more context |
| All models say the same thing | Question is too constrained or models influenced each other | Widen the question, verify independence |
| Synthesis is just a summary | Oracle1 didn't add analytical value | Require Oracle1 to explicitly judge and prioritize |
| No divergence points | Models are converging prematurely or question has obvious answer | Introduce deliberate tension in the seed question |
| Action items are all "more research" | Round didn't produce buildable output | Require at least one concrete implementation task |

---

## 5. How Synthesis Rounds Work

### 5.1 The Synthesis Challenge

Synthesis rounds (like Round 2b) face a harder problem than inquiry or critique rounds: they must merge multiple independent analyses into a coherent, buildable specification. This requires resolving contradictions, choosing between competing designs, and filling gaps that no single model addressed.

### 5.2 The DeepSeek Convergence Example

Round 2b provides a textbook example of successful synthesis. Two DeepSeek models (Reasoner and V3) independently analyzed the ability transfer problem:

| Aspect | DeepSeek Reasoner | DeepSeek V3 |
|--------|-------------------|-------------|
| Atomic unit name | "Constraint Resonance Pattern" | "Attentional Lens + Constraint Response Pattern" |
| Composition model | "Harmonic vs. Discordant Pairings" | "Lattice Points in Constraint Space" |
| Forge structure | `abilities/` with `constraint_patterns/` | `abilities/` with `lens_drills/` + `crisis_exercises/` |
| Verification | "Scar Detection" | "Scars in Operation" |
| Failure modes | 5 modes with detection + mitigation | 5 modes with different names, same content |

Oracle1's synthesis recognized that these were **the same insight expressed in different vocabularies**. The synthesis merged them into a single framework: "Constraint Resonance Pattern" (the atomic unit), "Lattice Composition" (how abilities combine), and "Scar Detection" (how to verify transfer). The resulting specification was stronger than either model's individual output because it incorporated both perspectives.

### 5.3 When Synthesis Fails

Synthesis can fail when models produce genuinely incompatible frameworks. In Round 2a (ISA Critique), all models agreed on the five flaws but disagreed on solutions. Kimi wanted to burn formats C-G and adopt LEB128; DeepSeek wanted to strip 40% of ops; Seed wanted to reclassify within existing formats.

Oracle1's synthesis handled this by:
1. Acknowledging the disagreement explicitly (divergence section)
2. Identifying the underlying agreement (all want smaller, denser bytecode)
3. Proposing a pragmatic compromise (keep base ISA, add compressed short format)
4. Deferring the breaking change (LEB128) to a future version

---

## 6. Model Selection Criteria

### 6.1 When to Add a New Model

A new model should be added to the simulation when:

- It has a **genuinely different cognitive architecture** (not just a fine-tuned variant of an existing model)
- It brings a **cognitive style not represented** by the current four models
- It can **receive the same input format** (markdown seed questions + context files)
- It can **produce structured markdown output** suitable for synthesis

### 6.2 Cognitive Style Coverage

The current four models cover four cognitive styles. Gaps that might warrant adding a model:

| Cognitive Style | Description | Potential Model |
|-----------------|-------------|-----------------|
| **Empirical** | Designs experiments, proposes measurements, thinks in data | A model optimized for scientific reasoning |
| **Adversarial** | Actively tries to break proposals, finds worst-case scenarios | A model trained on red-teaming |
| **Historical** | Draws on precedent, identifies recurring patterns, thinks in analogies | A model with broad historical training data |
| **Economic** | Optimizes for resource efficiency, cost-benefit analysis, trade-offs | A model trained on optimization problems |

### 6.3 Model Independence

The critical requirement is **cognitive independence**. Models that share training data, are fine-tuned from the same base, or are exposed to each other's outputs during generation will produce correlated results, defeating the purpose of multi-model simulation. When adding a new model, verify that it has not been trained on the same data or exposed to previous round outputs.

---

## 7. Limitations and Caveats

### 7.1 Known Limitations

1. **All models share a Western philosophical tradition**: The ability-vs-skill distinction draws heavily on Heidegger, Wittgenstein, Bourdieu, and Musashi. A model trained on different philosophical traditions might frame the problem entirely differently.

2. **Synthesis is done by one model**: Oracle1's synthesis introduces a single point of failure. A different synthesizer might weight consensus hits differently or identify different gold veins.

3. **No empirical validation yet**: The theoretical framework (abilities as constraint resonance patterns, transfer through structural isomorphism) has not been tested against actual agent behavior. The findings are currently hypotheses, not laws.

4. **The fleet is small**: Oracle1 grounds findings against a fleet of ~8 agents. Conclusions about fleet-scale ability transfer are extrapolations from limited data.

### 7.2 Mitigation Strategies

- **Add models with different training distributions** to reduce philosophical monoculture
- **Use multiple synthesizers** (not just Oracle1) for critical rounds
- **Build empirical experiments** (forge implementation, A/B testing) as soon as possible
- **Document assumptions explicitly** so they can be challenged later

---

*This methodology is itself a product of the simulation — it was refined across three rounds based on what produced the most useful outputs. It will continue to evolve as the simulation progresses.*
