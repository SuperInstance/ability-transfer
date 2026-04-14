# CONTRIBUTING.md — How to Participate in the Simulation

This document describes how to add new simulation rounds, propose research questions, and contribute findings to the ability-transfer lab.

---

## 1. Overview

The ability-transfer lab is a multi-model simulation where AI agents collaborate (independently) to design systems for transferring "abilities" between agents through git repos. Contributions take three forms:

1. **New simulation rounds** — Running a round with one or more models on a research question
2. **New research questions** — Proposing questions for future rounds
3. **Synthesis and documentation** — Improving cross-round analysis and documentation

All contributions follow the same principles: independence (models don't see each other's outputs within a round), accumulation (findings build on previous rounds), and convergence (multiple models independently agreeing carries more weight than any single opinion).

---

## 2. Adding a New Simulation Round

### 2.1 Prerequisites

Before starting a new round, ensure you have:

- [ ] Read all previous round outputs in `rounds/` directory
- [ ] Read the latest Oracle1 synthesis document for context on current state
- [ ] Read `FINDINGS.md` for the consolidated findings catalog
- [ ] Identified a specific research question that advances the simulation
- [ ] Determined which models will participate and why

### 2.2 Round Numbering Convention

Rounds are numbered sequentially with a descriptive suffix:

```
rounds/{NN}-{short-name}/
```

| Component | Format | Example |
|-----------|--------|---------|
| NN | Two-digit sequential number | `04` |
| short-name | Lowercase hyphenated descriptor | `forge-build`, `empirical-test`, `isa-v3-review` |

### 2.3 Round Setup Checklist

1. **Create the round directory**:
   ```bash
   mkdir -p rounds/04-{name}/
   ```

2. **Write the seed question**: A clear, specific question that:
   - Is grounded in previous round findings
   - Is specific enough to be actionable
   - Is open enough to allow diverse responses
   - Does NOT presuppose the answer

   Example of a good seed question:
   > "Given the 4-stage forge structure (Cut → Crisis → Navigate → Mirror) and the constraint resonance pattern as the atomic unit of ability, design a concrete set of 10 exercises for the Code Archaeologist ability. Each exercise must specify: stage, input codebase, constraints, success criteria, and failure modes. The exercises must be buildable as markdown + JSON files in a git repo."

   Example of a bad seed question:
   > "How should we design exercises?" (Too vague)

3. **Run each model independently**: Generate output for each model WITHOUT showing it other models' outputs. Save each output as `{model-name}.md` in the round directory.

4. **Write the Oracle1 synthesis**: After all model outputs are complete, write `oracle1-synthesis.md` following the fixed format (see §2.4).

### 2.4 Oracle1 Synthesis Template

```markdown
# Oracle1 Synthesis — {Round Name}

## Consensus Hits (all models agree)
1. **{Finding title}** — {Brief description}
   - Model A: {key evidence}
   - Model B: {key evidence}
   - Model C: {key evidence}

## Unique Gold Veins

### {Model}: {Insight Title}
- {Description of the unique insight}
- Why it matters: {impact on overall research}

### {Model}: {Insight Title}
- {Description of the unique insight}
- Why it matters: {impact on overall research}

## Divergence Points

### {Topic of Disagreement}
- **Model A says**: {position}
- **Model B says**: {position}
- **Assessment**: {which position is stronger and why}

## Action Items (priority order)
1. {Action} — {Source model, rationale}
2. {Action} — {Source model, rationale}
3. {Action} — {Source model, rationale}
```

---

## 3. Model Selection Criteria

### 3.1 Current Model Roles

| Model | Role | When to Use |
|-------|------|-------------|
| **Seed** | Creative generation, novel architectures, breaking assumptions | Ideation rounds, design exploration, "what if" questions |
| **Kimi** | Philosophical analysis, conceptual clarity, identifying shaky foundations | Critique rounds, ontological questions, assumption-challenging |
| **DeepSeek** | Engineering synthesis, practical specifications, structural trade-offs | Synthesis rounds, buildable design, implementation planning |
| **Oracle1** | Fleet grounding, operational feasibility, cross-model synthesis | Every round (synthesis role), feasibility checks, resource analysis |

### 3.2 Choosing Models for a Round

Not every round needs all four models. Use this decision matrix:

| Round Type | Minimum Models | Recommended Models | Why |
|------------|---------------|-------------------|-----|
| Inquiry | 2 (Kimi + one other) | All 4 | Maximum cognitive diversity for open questions |
| Critique | 3 (Kimi + DeepSeek + one other) | All 4 | Need both philosophical depth and engineering judgment |
| Synthesis | 2 (DeepSeek + Oracle1) | DeepSeek R + DeepSeek V3 + Oracle1 | Need convergent engineering perspectives |
| Design | 3 (Seed + DeepSeek + Oracle1) | All 4 | Need creativity, pragmatism, and grounding |
| Specification | 1 (Oracle1 or Datum) | Oracle1 + reviewer | Need focused, precise writing |
| Empirical | 1 (any, plus human observer) | Domain-specific | Need relevant expertise |

### 3.3 Adding a New Model

To add a new model to the simulation:

1. **Justify the cognitive gap**: What cognitive style does the current set lack that this model provides? Document the rationale in a paragraph.

2. **Verify independence**: Ensure the new model has not been trained on the same data as existing models or exposed to previous round outputs.

3. **Test with a pilot round**: Run one round with the new model alongside existing models. Evaluate whether the new model produces genuinely different insights or merely restates existing positions.

4. **Update this document**: Add the model to the selection criteria table with its role, cognitive style, and when to use it.

---

## 4. Round Output Format Requirements

### 4.1 File Format

All round outputs must be:

- **Markdown** (`.md`) — for human readability and git diffing
- **UTF-8 encoded** — no special characters beyond standard markdown
- **Self-contained** — each file should be readable without external references (though links to other round files are encouraged)
- **Minimum 800 words** — shallow outputs don't produce useful synthesis material

### 4.2 Structure

Each model output should include:

1. **Title** — Clear, descriptive heading (H1 or H2)
2. **Direct answer** — Start with the core insight, not background
3. **Supporting arguments** — At least 3 distinct reasons or evidence points
4. **Concrete examples** — Code, data structures, or specific scenarios
5. **Relationship to previous rounds** — Explicit references to findings from earlier rounds (with file paths)
6. **Open questions** — What this output doesn't answer

### 4.3 Code and Diagrams

- Code blocks should specify language: ````python`, ````cpp`, etc.
- ASCII diagrams are preferred over external images (they diff well in git)
- Tables are encouraged for structured comparisons
- YAML/JSON examples should be valid and parseable

### 4.4 Naming Conventions

| Item | Convention | Example |
|------|-----------|---------|
| Model output file | `{model-name}.md` | `kimi.md`, `deepseek.md`, `seed.md` |
| Synthesis file | `oracle1-synthesis.md` | Always this name |
| Exercise files | `{stage}-{name}.json` or `{stage}-{name}.yaml` | `01-cut-boundary-detection.json` |
| Round directory | `{NN}-{short-name}` | `04-forge-build` |

---

## 5. Proposing a New Research Question

### 5.1 What Makes a Good Research Question?

A good research question for this lab:

- **Advances the simulation**: Builds on previous findings, doesn't repeat covered ground
- **Is specific**: Can be answered in 800-2000 words, not a book-length inquiry
- **Is grounded**: References specific fleet constraints, hardware, or previous findings
- **Has multiple valid approaches**: Different models should reasonably disagree
- **Produces buildable output**: The answer should lead to something that can be implemented

### 5.2 How to Propose

1. Check `FINDINGS.md` Section 6 (Recommendations) for pre-identified questions
2. Check `CHANGELOG.md` for the current simulation state
3. Write your proposal as an issue or discussion item with:
   - **Question**: The specific research question
   - **Motivation**: Why this question matters now
   - **Previous context**: Which round findings it builds on
   - **Expected output**: What form the answer should take
   - **Suggested models**: Which models should participate and why

### 5.3 Example Proposals

From the current recommendations backlog:

| Priority | Question | Source | Round |
|----------|----------|--------|-------|
| High | Can we empirically verify ability transfer? Design A/B experiments. | FINDINGS.md §6.1 | 4 |
| High | Implement ISA v3 extensions in a FLUX runtime | FINDINGS.md §6.1 | 4 |
| Medium | Can Hardware Whisperer + Code Archaeologist compose into Resilience Architect? | FINDINGS.md §6.2 | 5 |
| Medium | Does the forge structure work for non-software abilities? | FINDINGS.md §6.3 | 6 |
| Low | Build a fleet-wide ability registry | FINDINGS.md §6.3 | 7 |

---

## 6. Review Process

### 6.1 Self-Review (Before Committing)

Before committing a round output, verify:

- [ ] File follows the naming convention
- [ ] File is minimum 800 words
- [ ] File includes concrete examples (code, data structures, scenarios)
- [ ] File references previous round findings with file paths
- [ ] File is self-contained (readable without external context)
- [ ] Synthesis document covers all model outputs (not just favorites)
- [ ] Synthesis honestly reports divergence points

### 6.2 Cross-Model Review

Oracle1 performs cross-model review as part of the synthesis process. This review checks:

- **Completeness**: Are all model outputs represented in the synthesis?
- **Accuracy**: Does the synthesis correctly represent each model's position?
- **Honesty**: Are disagreements surfaced rather than hidden?
- **Progress**: Does the round advance beyond the previous round?

### 6.3 Post-Commit Review

After a round is committed:

1. **Update README.md**: Add the round to the simulation rounds table with status and key output
2. **Update FINDINGS.md**: Add new findings to the appropriate sections with source references
3. **Update CHANGELOG.md**: Record the round completion with date and summary
4. **Tag the commit**: Use semantic versioning for milestone rounds

---

## 7. Git Workflow

### 7.1 Branching

- `main` branch contains all committed rounds
- Feature branches (`round-{NN}-{name}`) for work-in-progress rounds
- Synthesis branches (`synthesis-{NN}`) for Oracle1 synthesis work

### 7.2 Commit Messages

Follow conventional commits:

```
feat: add round 04 forge-build exercises
docs: update FINDINGS.md with round 4 results
fix: correct synthesis divergence section for round 2a
```

### 7.3 When to Commit

- Commit individual model outputs as they are generated (incremental commits are fine)
- Commit the synthesis document as a separate commit (it's the critical artifact)
- Commit documentation updates (README, FINDINGS, CHANGELOG) together as a documentation commit

---

## 8. Conflict Resolution

### 8.1 When Models Disagree

Disagreement is a feature, not a bug. When models disagree:

1. **Surface the disagreement** in the synthesis divergence section
2. **Explain *why*** they disagree (different assumptions, different priorities, different constraints)
3. **Assess which position is stronger** based on:
   - Consistency with previous round findings
   - Grounding in real constraints (fleet hardware, operational reality)
   - Actionability (can you build something from this?)
4. **Defer to evidence** when available; defer to Oracle1's judgment when not

### 8.2 When to Escalate

Escalate to a broader review when:

- Two models produce flatly contradictory technical claims (e.g., "X is possible" vs. "X is impossible")
- The disagreement blocks all progress on a research question
- The disagreement reveals a fundamental assumption that needs to be re-examined

---

*Contributions to this lab are contributions to the FLUX Fleet's long-term architecture. Every round, every finding, every disagreement is part of building something that will outlast any individual session. Thank you for participating.*
