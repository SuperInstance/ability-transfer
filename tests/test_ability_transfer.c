/*
 * Ability Transfer System — Test Suite
 * 20 tests covering all major subsystems
 *
 * Copyright (c) 2026 SuperInstance, MIT License
 */

#include "ability_transfer.h"
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <assert.h>

static int tests_passed = 0;
static int tests_failed = 0;

#define TEST(name) void name(void)
#define RUN(name) do { printf("  %-50s", #name); name(); } while(0)
#define PASS do { printf("PASS\n"); tests_passed++; return; } while(0)
#define FAIL(msg) do { printf("FAIL: %s\n", msg); tests_failed++; return; } while(0)
#define ASSERT_EQ(a, b, msg) do { if ((a) != (b)) { printf("FAIL: %s (got %d, expected %d)\n", msg, (int)(a), (int)(b)); tests_failed++; return; } } while(0)
#define ASSERT_GT(a, b, msg) do { if (!((a) > (b))) { printf("FAIL: %s\n", msg); tests_failed++; return; } } while(0)
#define ASSERT_FLT_EQ(a, b, msg) do { if (fabsf((a) - (b)) > 0.01f) { printf("FAIL: %s (got %.2f, expected %.2f)\n", msg, (double)(a), (double)(b)); tests_failed++; return; } } while(0)

/* ── Skill Representation Tests ─────────────────────────────── */

TEST(test_skill_init) {
    Skill s;
    skill_init(&s);
    ASSERT_EQ(strlen(s.name), 0, "name should be empty");
    ASSERT_EQ(s.category, AT_SKILL_UNKNOWN, "category should be unknown");
    ASSERT_EQ(s.mastery, AT_MASTERY_NOVICE, "mastery should be novice");
    ASSERT_EQ(s.forge_stage, AT_FORGE_NONE, "forge_stage should be none");
    ASSERT_EQ(s.tag_count, 0, "no tags");
    ASSERT_EQ(s.dep_count, 0, "no deps");
    ASSERT_EQ(s.exercise_count, 0, "no exercises");
    ASSERT_EQ(s.context_count, 0, "no contexts");
    PASS;
}

TEST(test_skill_basic_properties) {
    Skill s;
    skill_init(&s);
    skill_set_name(&s, "debugging");
    skill_set_description(&s, "The ability to systematically find and fix bugs");
    skill_set_category(&s, AT_SKILL_TECHNICAL);
    skill_set_mastery(&s, AT_MASTERY_ADVANCED);
    skill_set_forge_stage(&s, AT_FORGE_QUENCHED);

    ASSERT_EQ(strcmp(s.name, "debugging"), 0, "name should be 'debugging'");
    ASSERT_EQ(s.category, AT_SKILL_TECHNICAL, "category should be technical");
    ASSERT_EQ(s.mastery, AT_MASTERY_ADVANCED, "mastery should be advanced");
    ASSERT_EQ(s.forge_stage, AT_FORGE_QUENCHED, "forge_stage should be quenched");
    PASS;
}

TEST(test_skill_tags) {
    Skill s;
    skill_init(&s);
    ASSERT_EQ(skill_add_tag(&s, "coding"), 0, "add tag coding");
    ASSERT_EQ(skill_add_tag(&s, "debugging"), 0, "add tag debugging");
    ASSERT_EQ(skill_add_tag(&s, "coding"), 1, "duplicate tag should return 1");
    ASSERT_EQ(s.tag_count, 2, "should have 2 tags");
    ASSERT_EQ(skill_has_tag(&s, "coding"), 1, "should have coding tag");
    ASSERT_EQ(skill_has_tag(&s, "debugging"), 1, "should have debugging tag");
    ASSERT_EQ(skill_has_tag(&s, "cooking"), 0, "should not have cooking tag");
    PASS;
}

TEST(test_skill_dependencies) {
    Skill s;
    skill_init(&s);
    skill_set_name(&s, "system-design");

    ASSERT_EQ(skill_add_dependency(&s, "coding", AT_MASTERY_INTERMEDIATE), 0, "add dep");
    ASSERT_EQ(skill_add_dependency(&s, "debugging", AT_MASTERY_BEGINNER), 0, "add dep");
    ASSERT_EQ(s.dep_count, 2, "should have 2 deps");
    ASSERT_EQ(strcmp(s.deps[0].skill_name, "coding"), 0, "first dep is coding");
    ASSERT_EQ(s.deps[1].min_mastery, AT_MASTERY_BEGINNER, "second dep mastery");
    PASS;
}

TEST(test_skill_exercises) {
    Skill s;
    skill_init(&s);
    skill_set_name(&s, "code-review");

    skill_add_exercise(&s, "find-bugs", "Review code to find 5 bugs", AT_FORGE_HEATED, 3, 10, 0.7f);
    skill_add_exercise(&s, "pattern-match", "Identify common bug patterns", AT_FORGE_HAMMERED, 5, 20, 0.6f);
    skill_add_exercise(&s, "edge-case-hunt", "Find edge cases in production code", AT_FORGE_QUENCHED, 7, 5, 0.5f);

    ASSERT_EQ(s.exercise_count, 3, "should have 3 exercises");
    ASSERT_EQ(s.exercises[0].difficulty, 3, "first exercise difficulty");
    ASSERT_EQ(s.exercises[1].stage, AT_FORGE_HAMMERED, "second exercise stage");
    ASSERT_FLT_EQ(s.exercises[2].success_rate, 0.5f, "third exercise success rate");
    PASS;
}

TEST(test_skill_contexts) {
    Skill s;
    skill_init(&s);
    skill_set_name(&s, "communication");

    skill_add_context(&s, "meetings", "Team meetings and standups", 0.9f);
    skill_add_context(&s, "written", "Emails and documentation", 0.7f);
    skill_add_context(&s, "conflict", "Conflict resolution", 0.5f);

    ASSERT_EQ(s.context_count, 3, "should have 3 contexts");
    ASSERT_FLT_EQ(s.contexts[0].transfer_score, 0.9f, "meetings transfer score");
    ASSERT_FLT_EQ(s.contexts[2].transfer_score, 0.5f, "conflict transfer score");
    PASS;
}

TEST(test_skill_clone) {
    Skill s1, s2;
    skill_init(&s1);
    skill_set_name(&s1, "original");
    skill_set_category(&s1, AT_SKILL_COGNITIVE);
    skill_set_mastery(&s1, AT_MASTERY_EXPERT);
    skill_add_tag(&s1, "thinking");

    ASSERT_EQ(skill_clone(&s1, &s2), 0, "clone should succeed");
    ASSERT_EQ(strcmp(s2.name, "original"), 0, "cloned name");
    ASSERT_EQ(s2.category, AT_SKILL_COGNITIVE, "cloned category");
    ASSERT_EQ(s2.mastery, AT_MASTERY_EXPERT, "cloned mastery");
    ASSERT_EQ(s2.tag_count, 1, "cloned tag count");

    /* Verify independence */
    skill_set_name(&s2, "modified");
    ASSERT_EQ(strcmp(s1.name, "original"), 0, "original unchanged");
    PASS;
}

TEST(test_skill_capabilities) {
    Skill s;
    skill_init(&s);
    skill_set_name(&s, "web-dev");

    skill_add_capability(&s, "http", 3);
    skill_add_capability(&s, "javascript", 4);
    skill_add_capability(&s, "css", 2);

    ASSERT_EQ(s.cap_count, 3, "should have 3 capabilities");
    ASSERT_EQ(strcmp(s.caps[0].name, "http"), 0, "first cap is http");
    ASSERT_EQ(s.caps[1].min_level, 4, "second cap level");
    PASS;
}

/* ── Transfer Protocol Tests ────────────────────────────────── */

TEST(test_serialize_deserialize_roundtrip) {
    Skill s1, s2;
    skill_init(&s1);
    skill_set_name(&s1, "negotiation");
    skill_set_category(&s1, AT_SKILL_BEHAVIORAL);
    skill_set_mastery(&s1, AT_MASTERY_ADVANCED);
    skill_set_forge_stage(&s1, AT_FORGE_TEMPERED);
    skill_add_tag(&s1, "soft-skills");
    skill_add_tag(&s1, "business");
    skill_add_dependency(&s1, "communication", AT_MASTERY_INTERMEDIATE);
    skill_add_exercise(&s1, "roleplay", "Practice negotiation scenarios", AT_FORGE_HAMMERED, 5, 10, 0.7f);
    skill_add_context(&s1, "sales", "Sales negotiations", 0.8f);
    s1.quality_score = 0.85f;
    s1.verified = 1;

    uint8_t buf[AT_MAX_PROTOCOL_SIZE];
    uint32_t len = 0;
    ASSERT_EQ(transfer_serialize(&s1, buf, &len), 0, "serialize");
    ASSERT_GT(len, 0, "serialized length > 0");

    ASSERT_EQ(transfer_deserialize(buf, len, &s2), 0, "deserialize");
    ASSERT_EQ(strcmp(s2.name, "negotiation"), 0, "deserialized name");
    ASSERT_EQ(s2.category, AT_SKILL_BEHAVIORAL, "deserialized category");
    ASSERT_EQ(s2.mastery, AT_MASTERY_ADVANCED, "deserialized mastery");
    ASSERT_EQ(s2.forge_stage, AT_FORGE_TEMPERED, "deserialized forge_stage");
    ASSERT_EQ(s2.tag_count, 2, "deserialized tag count");
    ASSERT_EQ(s2.dep_count, 1, "deserialized dep count");
    ASSERT_EQ(s2.exercise_count, 1, "deserialized exercise count");
    ASSERT_EQ(s2.context_count, 1, "deserialized context count");
    ASSERT_FLT_EQ(s2.quality_score, 0.85f, "deserialized quality score");
    ASSERT_EQ(s2.verified, 1, "deserialized verified flag");
    PASS;
}

TEST(test_transfer_message_creation) {
    TransferMessage msg;
    Skill s;
    skill_init(&s);
    skill_set_name(&s, "pair-programming");

    ASSERT_EQ(transfer_create_offer(&msg, "agent-A", "agent-B", &s), 0, "create offer");
    ASSERT_EQ(msg.type, TP_MSG_OFFER, "message type");
    ASSERT_EQ(strcmp(msg.sender, "agent-A"), 0, "sender");
    ASSERT_EQ(strcmp(msg.receiver, "agent-B"), 0, "receiver");
    ASSERT_EQ(strcmp(msg.skill_name, "pair-programming"), 0, "skill name");
    ASSERT_GT(msg.payload_len, 0, "payload not empty");

    ASSERT_EQ(transfer_create_request(&msg, "agent-B", "agent-A", "code-review"), 0, "create request");
    ASSERT_EQ(msg.type, TP_MSG_REQUEST, "message type");
    ASSERT_EQ(strcmp(msg.skill_name, "code-review"), 0, "skill name");

    ASSERT_EQ(transfer_create_accept(&msg, "agent-B", "agent-A", "code-review"), 0, "create accept");
    ASSERT_EQ(msg.type, TP_MSG_ACCEPT, "message type");

    ASSERT_EQ(transfer_create_reject(&msg, "agent-B", "agent-A", "code-review", "Not ready"), 0, "create reject");
    ASSERT_EQ(msg.type, TP_MSG_REJECT, "message type");
    ASSERT_EQ(strcmp(msg.verify_detail, "Not ready"), 0, "reject reason");

    ASSERT_EQ(transfer_create_compat_query(&msg, "agent-A", "agent-B", &s), 0, "create compat query");
    ASSERT_EQ(msg.type, TP_MSG_QUERY_COMPAT, "message type");

    ASSERT_EQ(transfer_create_verify(&msg, "agent-A", "agent-B", &s), 0, "create verify");
    ASSERT_EQ(msg.type, TP_MSG_VERIFY, "message type");
    PASS;
}

TEST(test_message_type_names) {
    ASSERT_EQ(strcmp(transfer_message_type_name(TP_MSG_OFFER), "OFFER"), 0, "OFFER");
    ASSERT_EQ(strcmp(transfer_message_type_name(TP_MSG_REQUEST), "REQUEST"), 0, "REQUEST");
    ASSERT_EQ(strcmp(transfer_message_type_name(TP_MSG_ACCEPT), "ACCEPT"), 0, "ACCEPT");
    ASSERT_EQ(strcmp(transfer_message_type_name(TP_MSG_REJECT), "REJECT"), 0, "REJECT");
    ASSERT_EQ(strcmp(transfer_message_type_name(TP_MSG_ACKNOWLEDGE), "ACKNOWLEDGE"), 0, "ACKNOWLEDGE");
    PASS;
}

/* ── Compatibility Tests ────────────────────────────────────── */

TEST(test_compatibility_identical_skills) {
    Skill s1, s2;
    skill_init(&s1);
    skill_set_name(&s1, "debugging");
    skill_set_category(&s1, AT_SKILL_TECHNICAL);
    skill_set_mastery(&s1, AT_MASTERY_INTERMEDIATE);
    skill_add_capability(&s1, "logic", 3);

    skill_init(&s2);
    skill_set_name(&s2, "debugging");
    skill_set_category(&s2, AT_SKILL_TECHNICAL);
    skill_set_mastery(&s2, AT_MASTERY_INTERMEDIATE);
    skill_add_capability(&s2, "logic", 3);

    CompatibilityResult result;
    compatibility_check(&s1, &s2, &result);

    ASSERT_FLT_EQ(result.category_match, 1.0f, "same category = 1.0");
    ASSERT_FLT_EQ(result.capability_match, 1.0f, "same caps = 1.0");
    ASSERT_GT(result.overall_score, 0.8f, "identical skills should score high");
    PASS;
}

TEST(test_compatibility_different_categories) {
    Skill s1, s2;
    skill_init(&s1);
    skill_set_name(&s1, "math");
    skill_set_category(&s1, AT_SKILL_COGNITIVE);

    skill_init(&s2);
    skill_set_name(&s2, "coding");
    skill_set_category(&s2, AT_SKILL_TECHNICAL);

    CompatibilityResult result;
    compatibility_check(&s1, &s2, &result);

    ASSERT_GT(result.category_match, 0.0f, "different categories still have some match");
    ASSERT_GT(result.overall_score, 0.1f, "overall score > 0");
    PASS;
}

TEST(test_compatibility_category_score) {
    ASSERT_FLT_EQ(compatibility_category_score(AT_SKILL_COGNITIVE, AT_SKILL_COGNITIVE), 1.0f, "same");
    ASSERT_FLT_EQ(compatibility_category_score(AT_SKILL_JUDGMENT, AT_SKILL_COGNITIVE), 0.7f, "judgment-cognitive");
    ASSERT_FLT_EQ(compatibility_category_score(AT_SKILL_ADAPTATION, AT_SKILL_METALEARNING), 0.6f, "adaptation-metalearning");
    ASSERT_GT(compatibility_category_score(AT_SKILL_TECHNICAL, AT_SKILL_CREATIVITY), 0.2f, "technical-creativity");
    PASS;
}

/* ── Adaptation Tests ───────────────────────────────────────── */

TEST(test_adapt_skill_new_context) {
    Skill source;
    skill_init(&source);
    skill_set_name(&source, "code-review");
    skill_set_category(&source, AT_SKILL_TECHNICAL);
    skill_set_mastery(&source, AT_MASTERY_ADVANCED);
    skill_add_exercise(&source, "review-code", "Review a simple function", AT_FORGE_HEATED, 3, 5, 0.8f);
    skill_add_exercise(&source, "review-patterns", "Find patterns in codebase", AT_FORGE_HAMMERED, 5, 10, 0.7f);
    skill_add_context(&source, "python", "Python codebases", 0.9f);

    Skill agent_skills[1];
    skill_init(&agent_skills[0]);
    skill_set_name(&agent_skills[0], "python-basics");
    skill_set_mastery(&agent_skills[0], AT_MASTERY_INTERMEDIATE);

    AdaptationResult result;
    adapt_skill(&source, "rust", agent_skills, 1, &result);

    ASSERT_EQ(strcmp(result.adapted.name, "code-review"), 0, "name preserved");
    ASSERT_GT(result.exercises_added, 0, "should add bridge exercises");
    /* Should have original + 3 bridge exercises */
    ASSERT_EQ(result.adapted.exercise_count, 2 + 3, "total exercises after adaptation");
    ASSERT_GT(result.adaptation_score, 0.5f, "adaptation score > 0.5");
    PASS;
}

TEST(test_adapt_adjust_mastery) {
    Skill source;
    skill_init(&source);
    skill_set_name(&source, "testing");
    skill_set_mastery(&source, AT_MASTERY_ADVANCED);

    AdaptationResult result;
    adapt_skill(&source, NULL, NULL, 0, &result);
    adapt_adjust_mastery(&result, AT_MASTERY_BEGINNER);

    ASSERT_EQ(result.adapted.mastery, AT_MASTERY_BEGINNER, "mastery adjusted");
    PASS;
}

TEST(test_adapt_bridge_exercises) {
    Skill source;
    skill_init(&source);
    skill_set_name(&source, "architect");
    skill_set_mastery(&source, AT_MASTERY_EXPERT);

    Skill gaps[2];
    skill_init(&gaps[0]);
    skill_set_name(&gaps[0], "design-patterns");
    skill_set_mastery(&gaps[0], AT_MASTERY_INTERMEDIATE);
    skill_init(&gaps[1]);
    skill_set_name(&gaps[1], "system-thinking");
    skill_set_mastery(&gaps[1], AT_MASTERY_BEGINNER);

    AdaptationResult result;
    adapt_skill(&source, NULL, NULL, 0, &result);
    adapt_add_bridge_exercises(&result, gaps, 2);

    ASSERT_EQ(result.exercises_added, 2, "2 bridge exercises added");
    PASS;
}

/* ── Knowledge Graph Tests ──────────────────────────────────── */

TEST(test_graph_basic) {
    KnowledgeGraph g;
    graph_init(&g);

    int a = graph_add_node(&g, "coding", AT_SKILL_TECHNICAL, AT_MASTERY_INTERMEDIATE, AT_FORGE_QUENCHED);
    int b = graph_add_node(&g, "debugging", AT_SKILL_TECHNICAL, AT_MASTERY_BEGINNER, AT_FORGE_HEATED);
    int c = graph_add_node(&g, "system-design", AT_SKILL_COGNITIVE, AT_MASTERY_NOVICE, AT_FORGE_NONE);

    ASSERT_EQ(a, 0, "first node id");
    ASSERT_EQ(b, 1, "second node id");
    ASSERT_EQ(c, 2, "third node id");
    ASSERT_EQ(g.node_count, 3, "3 nodes");
    PASS;
}

TEST(test_graph_edges_and_find) {
    KnowledgeGraph g;
    graph_init(&g);
    int a = graph_add_node(&g, "coding", AT_SKILL_TECHNICAL, 0, 0);
    int b = graph_add_node(&g, "debugging", AT_SKILL_TECHNICAL, 0, 0);
    int c = graph_add_node(&g, "system-design", AT_SKILL_COGNITIVE, 0, 0);

    ASSERT_EQ(graph_add_edge(&g, b, a, "depends_on", 0.8f), 0, "debugging depends on coding");
    ASSERT_EQ(graph_add_edge(&g, c, b, "depends_on", 0.9f), 0, "system-design depends on debugging");
    ASSERT_EQ(graph_add_edge(&g, a, b, "strengthens", 0.7f), 0, "coding strengthens debugging");
    ASSERT_EQ(g.edge_count, 3, "3 edges");

    ASSERT_EQ(graph_find_node(&g, "coding"), 0, "find coding");
    ASSERT_EQ(graph_find_node(&g, "debugging"), 1, "find debugging");
    ASSERT_EQ(graph_find_node(&g, "nonexistent"), -1, "find nonexistent");
    PASS;
}

TEST(test_graph_dependencies) {
    KnowledgeGraph g;
    graph_init(&g);
    int a = graph_add_node(&g, "basic-math", AT_SKILL_COGNITIVE, 0, 0);
    int b = graph_add_node(&g, "algebra", AT_SKILL_COGNITIVE, 0, 0);
    int c = graph_add_node(&g, "calculus", AT_SKILL_COGNITIVE, 0, 0);
    graph_add_edge(&g, b, a, "depends_on", 1.0f);
    graph_add_edge(&g, c, b, "depends_on", 1.0f);

    int deps[16];
    int count = graph_get_dependencies(&g, c, deps, 16);
    ASSERT_EQ(count, 1, "calculus has 1 direct dep");
    ASSERT_EQ(deps[0], b, "depends on algebra");

    int dependents[16];
    count = graph_get_dependents(&g, a, dependents, 16);
    ASSERT_EQ(count, 1, "basic-math has 1 dependent");
    ASSERT_EQ(dependents[0], b, "algebra depends on basic-math");
    PASS;
}

TEST(test_graph_topological_sort) {
    KnowledgeGraph g;
    graph_init(&g);
    int a = graph_add_node(&g, "reading", AT_SKILL_COGNITIVE, 0, 0);
    int b = graph_add_node(&g, "writing", AT_SKILL_BEHAVIORAL, 0, 0);
    int c = graph_add_node(&g, "analysis", AT_SKILL_COGNITIVE, 0, 0);
    int d = graph_add_node(&g, "critique", AT_SKILL_JUDGMENT, 0, 0);
    graph_add_edge(&g, b, a, "depends_on", 1.0f);  /* writing depends on reading */
    graph_add_edge(&g, c, a, "depends_on", 1.0f);  /* analysis depends on reading */
    graph_add_edge(&g, d, b, "depends_on", 1.0f);  /* critique depends on writing */
    graph_add_edge(&g, d, c, "depends_on", 1.0f);  /* critique depends on analysis */

    int order[16];
    int count = graph_topological_sort(&g, order, 16);
    ASSERT_EQ(count, 4, "all 4 nodes sorted");

    /* reading should come before writing, analysis */
    int reading_pos = -1, writing_pos = -1;
    for (int i = 0; i < count; i++) {
        if (order[i] == a) reading_pos = i;
        if (order[i] == b) writing_pos = i;
    }
    ASSERT_GT(writing_pos, reading_pos, "writing after reading in topo sort");
    PASS;
}

TEST(test_graph_path_strength) {
    KnowledgeGraph g;
    graph_init(&g);
    int a = graph_add_node(&g, "a", AT_SKILL_TECHNICAL, 0, 0);
    int b = graph_add_node(&g, "b", AT_SKILL_TECHNICAL, 0, 0);
    int c = graph_add_node(&g, "c", AT_SKILL_TECHNICAL, 0, 0);
    graph_add_edge(&g, a, b, "enables", 0.8f);
    graph_add_edge(&g, b, c, "enables", 0.5f);

    float strength = graph_path_strength(&g, a, c);
    ASSERT_GT(strength, 0.0f, "path exists");
    /* Expected: 1.0 * 0.8 * 0.5 = 0.4 (approximately, BFS finds first path) */
    ASSERT_GT(strength, 0.3f, "path strength reasonable");
    PASS;
}

TEST(test_graph_find_learning_path) {
    KnowledgeGraph g;
    graph_init(&g);
    int a = graph_add_node(&g, "basics", AT_SKILL_TECHNICAL, 0, 0);
    int b = graph_add_node(&g, "intermediate", AT_SKILL_TECHNICAL, 0, 0);
    int c = graph_add_node(&g, "advanced", AT_SKILL_TECHNICAL, 0, 0);
    graph_add_edge(&g, a, b, "enables", 1.0f);
    graph_add_edge(&g, b, c, "enables", 1.0f);

    int path[16];
    int count = graph_find_learning_path(&g, a, c, path, 16);
    ASSERT_EQ(count, 3, "path length 3");
    ASSERT_EQ(path[0], a, "path starts at basics");
    ASSERT_EQ(path[1], b, "path through intermediate");
    ASSERT_EQ(path[2], c, "path ends at advanced");
    PASS;
}

/* ── Verification Tests ─────────────────────────────────────── */

TEST(test_verify_complete_skill) {
    Skill s;
    skill_init(&s);
    skill_set_name(&s, "thorough-testing");
    skill_set_category(&s, AT_SKILL_TECHNICAL);
    skill_set_description(&s, "Comprehensive testing methodology covering unit, integration, and E2E");
    skill_add_tag(&s, "testing");
    skill_add_tag(&s, "quality");
    skill_add_exercise(&s, "unit-test", "Write unit tests for a function", AT_FORGE_HEATED, 3, 10, 0.8f);
    skill_add_exercise(&s, "integration", "Test module interactions", AT_FORGE_HAMMERED, 5, 5, 0.6f);
    skill_add_exercise(&s, "e2e", "End-to-end test scenarios", AT_FORGE_QUENCHED, 7, 3, 0.5f);
    skill_add_context(&s, "web", "Web application testing", 0.9f);
    skill_add_context(&s, "api", "API testing", 0.8f);
    s.quality_score = 0.9f;

    VerificationResult result;
    verify_skill(&s, &result);

    ASSERT_EQ(result.passed, 1, "skill should pass verification");
    ASSERT_GT(result.pass_rate, 0.7f, "pass rate > 70%%");
    PASS;
}

TEST(test_verify_incomplete_skill) {
    Skill s;
    skill_init(&s);
    /* No name, no category, no exercises, no contexts */

    VerificationResult result;
    verify_skill(&s, &result);

    ASSERT_EQ(result.passed, 0, "empty skill should fail");
    ASSERT_GT(0.7f, result.pass_rate, "pass rate < 70%%");
    PASS;
}

TEST(test_verify_helpers) {
    Skill s;
    skill_init(&s);
    skill_set_name(&s, "test");

    ASSERT_EQ(verify_has_min_exercises(&s, 3), 0, "no exercises");
    skill_add_exercise(&s, "e1", "Exercise 1", AT_FORGE_HEATED, 3, 5, 0.7f);
    skill_add_exercise(&s, "e2", "Exercise 2", AT_FORGE_HAMMERED, 5, 5, 0.6f);
    skill_add_exercise(&s, "e3", "Exercise 3", AT_FORGE_QUENCHED, 7, 3, 0.5f);
    ASSERT_EQ(verify_has_min_exercises(&s, 3), 1, "has 3 exercises");

    ASSERT_EQ(verify_has_forge_progression(&s), 1, "has heated+hammered progression");

    ASSERT_EQ(verify_has_context_coverage(&s, 2), 0, "no contexts");
    skill_add_context(&s, "ctx1", "Context 1", 0.5f);
    skill_add_context(&s, "ctx2", "Context 2", 0.6f);
    ASSERT_EQ(verify_has_context_coverage(&s, 2), 1, "has 2 contexts");
    PASS;
}

TEST(test_verify_reliability) {
    Skill s;
    skill_init(&s);
    skill_set_name(&s, "test");

    float r1 = verify_estimated_reliability(&s);
    ASSERT_GT(r1 + 0.01f, 0.009f, "reliability >= 0 for initialized skill");

    s.mastery = AT_MASTERY_MASTER;
    s.forge_stage = AT_FORGE_TEMPERED;
    for (int i = 0; i < 10; i++) {
        char name[32]; snprintf(name, 32, "ex%d", i);
        skill_add_exercise(&s, name, "exercise", AT_FORGE_TEMPERED, 7, 5, 0.7f);
    }
    for (int i = 0; i < 5; i++) {
        char dom[32]; snprintf(dom, 32, "ctx%d", i);
        skill_add_context(&s, dom, "context", 0.8f);
    }
    float r2 = verify_estimated_reliability(&s);
    ASSERT_GT(r2, r1, "enhanced skill more reliable");
    ASSERT_GT(r2, 0.8f, "maxed skill reliability > 0.8");
    PASS;
}

/* ── Utility Tests ───────────────────────────────────────────── */

TEST(test_enum_names) {
    ASSERT_EQ(strcmp(skill_category_name(AT_SKILL_TECHNICAL), "technical"), 0, "technical");
    ASSERT_EQ(strcmp(skill_category_name(AT_SKILL_JUDGMENT), "judgment"), 0, "judgment");
    ASSERT_EQ(strcmp(mastery_level_name(AT_MASTERY_EXPERT), "expert"), 0, "expert");
    ASSERT_EQ(strcmp(mastery_level_name(AT_MASTERY_NOVICE), "novice"), 0, "novice");
    ASSERT_EQ(strcmp(forge_stage_name(AT_FORGE_QUENCHED), "quenched"), 0, "quenched");
    ASSERT_EQ(strcmp(forge_stage_name(AT_FORGE_TEMPERED), "tempered"), 0, "tempered");
    PASS;
}

/* ── Main ───────────────────────────────────────────────────── */

int main(void) {
    printf("Ability Transfer System — Test Suite\n\n");

    printf("Skill Representation:\n");
    RUN(test_skill_init);
    RUN(test_skill_basic_properties);
    RUN(test_skill_tags);
    RUN(test_skill_dependencies);
    RUN(test_skill_exercises);
    RUN(test_skill_contexts);
    RUN(test_skill_clone);
    RUN(test_skill_capabilities);

    printf("\nTransfer Protocol:\n");
    RUN(test_serialize_deserialize_roundtrip);
    RUN(test_transfer_message_creation);
    RUN(test_message_type_names);

    printf("\nCompatibility Checking:\n");
    RUN(test_compatibility_identical_skills);
    RUN(test_compatibility_different_categories);
    RUN(test_compatibility_category_score);

    printf("\nSkill Adaptation:\n");
    RUN(test_adapt_skill_new_context);
    RUN(test_adapt_adjust_mastery);
    RUN(test_adapt_bridge_exercises);

    printf("\nKnowledge Graph:\n");
    RUN(test_graph_basic);
    RUN(test_graph_edges_and_find);
    RUN(test_graph_dependencies);
    RUN(test_graph_topological_sort);
    RUN(test_graph_path_strength);
    RUN(test_graph_find_learning_path);

    printf("\nVerification:\n");
    RUN(test_verify_complete_skill);
    RUN(test_verify_incomplete_skill);
    RUN(test_verify_helpers);
    RUN(test_verify_reliability);

    printf("\nUtilities:\n");
    RUN(test_enum_names);

    printf("\n%d/%d tests passed\n", tests_passed, tests_passed + tests_failed);
    return tests_failed > 0 ? 1 : 0;
}
