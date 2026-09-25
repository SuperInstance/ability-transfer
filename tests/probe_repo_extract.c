/*
 * probe_repo_extract.c — Round 4: the repo as frozen attention field, measured.
 *
 * Extracts a Skill from a real git repository's structure and commit rhythm,
 * following FINDINGS F-03/F-05: directory structure = the master's ontology,
 * commit history = the rhythm of judgment. The extracted Skill is emitted as
 * one SKILL_EXTRACT JSON line on stdout, loadable as an at.skill.json artifact
 * (gauge-checkable) and offerable via the transfer protocol.
 *
 * Self-hosting demo:  ./probe_repo_extract . ability-transfer
 *
 * MIT License (c) 2026 SuperInstance
 */
#define _POSIX_C_SOURCE 200809L  /* popen, pclose, strtok_r */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ability_transfer.h"

#define MAX_DIRS 64
#define MAX_DIR_LEN 64
#define MAX_TOUCHED 4096

static char* run_git(const char* repo, const char* args) {
    char cmd[1024];
    snprintf(cmd, sizeof cmd, "git -C \"%s\" %s 2>/dev/null", repo, args);
    FILE* p = popen(cmd, "r");
    if (!p) return NULL;
    char* buf = malloc(1 << 20);
    size_t n = fread(buf, 1, (1 << 20) - 1, p);
    buf[n] = 0;
    pclose(p);
    return buf;
}

static int count_lines(const char* s) {
    int n = 0;
    if (s && *s) for (const char* p = s; *p; p++) if (*p == '\n') n++;
    return n;
}

static void json_escape(char* dst, size_t cap, const char* src) {
    size_t j = 0;
    for (const char* p = src; *p && j + 2 < cap; p++) {
        if (*p == '"' || *p == '\\') { dst[j++] = '\\'; dst[j++] = *p; }
        else if (*p == '\n') { dst[j++] = '\\'; dst[j++] = 'n'; }
        else dst[j++] = *p;
    }
    dst[j] = 0;
}

int main(int argc, char** argv) {
    const char* repo = argc > 1 ? argv[1] : ".";
    const char* name = argc > 2 ? argv[2] : "repo-ability";

    /* ── metrics from the frozen attention field ── */
    char* log_out  = run_git(repo, "log --format=%ct --reverse");
    char* tree_out = run_git(repo, "ls-tree -r --name-only HEAD");
    char* churn_out= run_git(repo, "log --name-only --format= -- .");
    if (!log_out || !*log_out) { fprintf(stderr, "not a git repo with history: %s\n", repo); return 2; }

    int commits = count_lines(log_out);

    /* commit span in days */
    long first = 0, last = 0;
    if (sscanf(log_out, "%ld", &first) != 1) { fprintf(stderr, "no parseable timestamps\n"); return 2; }
    const char* lp = log_out;
    while (strstr(lp, "\n")) lp = strstr(lp, "\n") + 1;
    if (lp && *lp) last = atol(lp); else last = first;
    long span_days = commits > 1 ? (last - first) / 86400 + 1 : 1;
    double seniority = span_days / 365.0;
    double cadence = commits / ((double)span_days / 90.0 + 0.25); /* commits per 90 days */
    if (cadence > commits) cadence = commits;

    /* ontology: top-level dirs */
    char dirs[MAX_DIRS][MAX_DIR_LEN]; int dir_count = 0;
    int depth_total = 0, file_count = 0, test_files = 0, src_files = 0;
    {
        char* save = NULL;
        for (char* line = strtok_r(tree_out, "\n", &save); line && dir_count < MAX_DIRS;
             line = strtok_r(NULL, "\n", &save)) {
            file_count++;
            int depth = 0; char* f = line;
            while (*f) { if (*f == '/') depth++; f++; }
            depth_total += depth;
            const char* base = strrchr(line, '/'); base = base ? base + 1 : line;
            if (strstr(base, "test") || strstr(base, "spec")) test_files++;
            if (strstr(base, ".c") || strstr(base, ".py") || strstr(base, ".js") ||
                strstr(base, ".ts") || strstr(base, ".rs")) src_files++;
            if (strchr(line, '/')) {
                char top[MAX_DIR_LEN]; snprintf(top, sizeof top, "%.*s", MAX_DIR_LEN - 1, line);
                char* slash = strchr(top, '/'); if (slash) *slash = 0;
                int seen = 0;
                for (int i = 0; i < dir_count; i++) if (!strcmp(dirs[i], top)) { seen = 1; break; }
                if (!seen) snprintf(dirs[dir_count++], MAX_DIR_LEN, "%s", top);
            }
        }
    }
    double avg_depth = file_count ? (double)depth_total / file_count : 0;
    double discipline = file_count ? (double)test_files / file_count : 0;

    /* judgment rhythm: fraction of touched files revisited (hot spots) */
    int touched_total = 0, touched_unique = 0;
    {
        char seen[MAX_TOUCHED][256]; int seen_n = 0;
        char* save = NULL;
        for (char* line = strtok_r(churn_out, "\n", &save); line && seen_n < MAX_TOUCHED;
             line = strtok_r(NULL, "\n", &save)) {
            if (!*line) continue;
            touched_total++;
            int found = 0;
            for (int i = 0; i < seen_n; i++) if (!strcmp(seen[i], line)) { found = 1; break; }
            if (!found) { snprintf(seen[seen_n++], 256, "%s", line); touched_unique++; }
        }
    }
    double revision = touched_total ? 1.0 - (double)touched_unique / touched_total : 0;

    /* ── score → mastery (documented, deterministic) ──
     * seniority*0.5 + log1p(cadence)*0.3 + discipline*0.2, clamped 0..5  */
    double score = seniority * 0.5 + log1p(cadence) * 0.3 + discipline * 0.2;
    MasteryLevel mastery = (MasteryLevel)(score < 0 ? 0 : score > 5 ? 5 : (int)score);
    ForgeStage stage = (test_files > 0 && revision > 0.15) ? AT_FORGE_TEMPERED : AT_FORGE_QUENCHED;

    /* ── build the Skill ── */
    Skill s; skill_init(&s);
    skill_set_name(&s, name);
    skill_set_category(&s, AT_SKILL_JUDGMENT);
    skill_set_mastery(&s, mastery);
    skill_set_forge_stage(&s, stage);
    char desc[AT_MAX_DESC_LEN];
    snprintf(desc, sizeof desc,
        "Code archaeology over %s: %d commits in %ld days (%.2f/90d), %d files "
        "across %d top-level realms (avg depth %.2f), %d%% test files, %.0f%% of "
        "touches are re-visions of hot spots. The repo's frozen attention field "
        "as an extractable, offerable ability.",
        repo, commits, span_days, cadence, file_count, dir_count, avg_depth,
        (int)(discipline * 100), revision * 100);
    skill_set_description(&s, desc);
    skill_add_tag(&s, "code-archaeology");
    skill_add_tag(&s, "git");
    skill_add_tag(&s, "repo-dojo");
    for (int i = 0; i < dir_count && i < 8; i++) {
        char tag[AT_MAX_TAG_LEN]; snprintf(tag, sizeof tag, "realm:%.24s", dirs[i]);
        skill_add_tag(&s, tag);
    }
    skill_add_dependency(&s, "git", AT_MASTERY_ADVANCED);
    skill_add_dependency(&s, "pattern-salience", AT_MASTERY_INTERMEDIATE);
    skill_add_exercise(&s, "map-the-ontology",
        "Reproduce the top-level directory ontology from memory; compare against the repo; "
        "note every realm you misplaced (Socratic disruption).",
        AT_FORGE_HEATED, 4, 3, 0.7f);
    skill_add_exercise(&s, "read-the-rhythm",
        "Replay the commit log and predict which files the next commit touches; score hits.",
        AT_FORGE_HAMMERED, 6, 5, 0.55f);
    skill_add_exercise(&s, "hot-spot-verdict",
        "For each hot-spot file, write the reason it keeps being revisited before reading it.",
        AT_FORGE_QUENCHED, 7, 3, 0.6f);
    skill_add_exercise(&s, "cold-realm-audit",
        "Find a realm untouched in the last third of history; argue for delete vs keep.",
        AT_FORGE_TEMPERED, 8, 2, 0.5f);
    skill_add_context(&s, "code-review", "Judging unfamiliar codebases by structure first", 0.85f);
    skill_add_context(&s, "refactor-triage", "Deciding what to preserve during rewrites", 0.8f);
    skill_add_context(&s, "agent-onboarding", "Bred agents cloning shells via git (pollination)", 0.9f);

    /* ── knowledge graph: deps → learning path ──
     * Edges point from prerequisite → ability ("enables"/"strengthens"),
     * so the learning path FROM git TO the extracted skill is the order an
     * apprentice should walk. */
    KnowledgeGraph g; graph_init(&g);
    int id_skill = graph_add_node(&g, name, AT_SKILL_JUDGMENT, mastery, stage);
    int id_git   = graph_add_node(&g, "git", AT_SKILL_TECHNICAL, AT_MASTERY_EXPERT, AT_FORGE_TEMPERED);
    int id_pat   = graph_add_node(&g, "pattern-salience", AT_SKILL_COGNITIVE, AT_MASTERY_INTERMEDIATE, AT_FORGE_QUENCHED);
    graph_add_edge(&g, id_git, id_skill, "enables", 0.9f);
    graph_add_edge(&g, id_pat, id_skill, "strengthens", 0.7f);
    int path[8]; int path_len = graph_find_learning_path(&g, id_git, id_skill, path, 8);

    /* ── offer message (the transfer protocol, live) ── */
    TransferMessage offer; transfer_message_init(&offer);
    transfer_create_offer(&offer, "probe-repo-extract", "apprentice-agent", &s);

    /* ── serialize round trip (the artifact bytes that would travel a repo) ── */
    uint8_t wire[AT_MAX_PROTOCOL_SIZE]; uint32_t wire_len = 0;
    int rt_ok = transfer_serialize(&s, wire, &wire_len);
    Skill back; skill_init(&back);
    if (rt_ok == 0) rt_ok = transfer_deserialize(wire, wire_len, &back);

    /* ── emit SKILL_EXTRACT JSON line ── */
    char edesc[AT_MAX_DESC_LEN * 2]; json_escape(edesc, sizeof edesc, s.description);
    printf("SKILL_EXTRACT {");
    printf("\"schema\":\"at.skill/v1\",\"name\":\"%s\",\"category\":\"judgment\",\"mastery\":%d,",
           s.name, (int)s.mastery);
    printf("\"forge_stage\":%d,\"description\":\"%s\",", (int)s.forge_stage, edesc);
    printf("\"tags\":[");
    for (int i = 0; i < s.tag_count; i++) printf("%s\"%s\"", i ? "," : "", s.tags[i]);
    printf("],");
    printf("\"metrics\":{\"commits\":%d,\"span_days\":%ld,\"cadence_per_90d\":%.3f,"
           "\"files\":%d,\"realms\":%d,\"avg_depth\":%.3f,\"test_ratio\":%.3f,"
           "\"revision_ratio\":%.3f,\"mastery_score\":%.3f},",
           commits, span_days, cadence, file_count, dir_count, avg_depth,
           discipline, revision, score);
    printf("\"exercises\":[");
    for (int i = 0; i < s.exercise_count; i++)
        printf("%s{\"name\":\"%s\",\"stage\":%d,\"difficulty\":%d,\"reps\":%d}",
               i ? "," : "", s.exercises[i].name, (int)s.exercises[i].stage,
               s.exercises[i].difficulty, s.exercises[i].repetitions);
    printf("],");
    printf("\"learning_path\":[");
    for (int i = 0; i < path_len; i++) printf("%s\"%s\"", i ? "," : "", g.nodes[path[i]].name);
    printf("],");
    printf("\"offer\":{\"type\":\"%s\",\"from\":\"%s\",\"to\":\"%s\",\"seq\":%u,\"payload_len\":%u},",
           transfer_message_type_name(offer.type), offer.sender, offer.receiver,
           offer.sequence, offer.payload_len);
    printf("\"wire_roundtrip\":\"%s\",\"wire_bytes\":%u",
           rt_ok == 0 ? "PASS" : "FAIL", wire_len);
    printf("}\n");

    free(log_out); free(tree_out); free(churn_out);
    return 0;
}
