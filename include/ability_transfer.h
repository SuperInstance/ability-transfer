#ifndef ABILITY_TRANSFER_H
#define ABILITY_TRANSFER_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Ability Transfer System — C Implementation
 *
 * A framework for representing, transferring, adapting, and verifying
 * agent abilities (deeper than skills — these capture judgment, reflex,
 * and contextual awareness).
 *
 * Copyright (c) 2026 SuperInstance, MIT License
 */

/* ── Limits ─────────────────────────────────────────────────── */
#define AT_MAX_NAME_LEN        128
#define AT_MAX_DESC_LEN        512
#define AT_MAX_TAGS            16
#define AT_MAX_TAG_LEN         32
#define AT_MAX_DEPS            32
#define AT_MAX_EXERCISES       64
#define AT_MAX_CONTEXTS        16
#define AT_MAX_SKILLS          1024
#define AT_MAX_NODES           2048
#define AT_MAX_EDGES           4096
#define AT_MAX_TRANSFER_STEPS  32
#define AT_MAX_PROTOCOL_SIZE   4096

/* ── Skill Representation ───────────────────────────────────── */

typedef enum {
    AT_SKILL_UNKNOWN = 0,
    AT_SKILL_COGNITIVE,    /* Analytical thinking, pattern recognition */
    AT_SKILL_BEHAVIORAL,   /* Communication, negotiation, collaboration */
    AT_SKILL_TECHNICAL,    /* Coding, debugging, system design */
    AT_SKILL_CREATIVITY,   /* Ideation, reframing, lateral thinking */
    AT_SKILL_JUDGMENT,     /* Decision-making under uncertainty */
    AT_SKILL_METALEARNING, /* Learning how to learn */
    AT_SKILL_ADAPTATION,   /* Adjusting approach to context */
} SkillCategory;

typedef enum {
    AT_MASTERY_NOVICE = 0,
    AT_MASTERY_BEGINNER,
    AT_MASTERY_INTERMEDIATE,
    AT_MASTERY_ADVANCED,
    AT_MASTERY_EXPERT,
    AT_MASTERY_MASTER
} MasteryLevel;

typedef enum {
    AT_FORGE_NONE = 0,
    AT_FORGE_HEATED,     /* Paradigm opened, ready for shaping */
    AT_FORGE_HAMMERED,   /* Exercises building reflexes */
    AT_FORGE_QUENCHED,   /* Pattern committed */
    AT_FORGE_TEMPERED    /* Tested in new contexts, reliable */
} ForgeStage;

/* Capability requirements for a skill */
typedef struct {
    char name[AT_MAX_TAG_LEN];
    int  min_level;       /* 0-5 mastery requirement */
} CapabilityReq;

/* A dependency: another skill required with minimum mastery */
typedef struct {
    char skill_name[AT_MAX_NAME_LEN];
    MasteryLevel min_mastery;
} SkillDependency;

/* An exercise that builds the ability (part of the forge) */
typedef struct {
    char name[AT_MAX_NAME_LEN];
    char description[AT_MAX_DESC_LEN];
    ForgeStage stage;     /* Which forge stage this exercise targets */
    int  difficulty;      /* 1-10 */
    int  repetitions;     /* How many times to repeat */
    float success_rate;   /* Expected success rate for verification */
} Exercise;

/* A context where the ability should apply */
typedef struct {
    char domain[AT_MAX_TAG_LEN];
    char description[AT_MAX_DESC_LEN];
    float transfer_score; /* 0.0-1.0, how well skill transfers here */
} Context;

/* The core skill/ability representation */
typedef struct {
    char name[AT_MAX_NAME_LEN];
    char description[AT_MAX_DESC_LEN];
    SkillCategory category;
    MasteryLevel mastery;
    ForgeStage forge_stage;
    char version[32];

    /* Tags for classification */
    int  tag_count;
    char tags[AT_MAX_TAGS][AT_MAX_TAG_LEN];

    /* Dependencies on other skills */
    int dep_count;
    SkillDependency deps[AT_MAX_DEPS];

    /* Capability requirements */
    int cap_count;
    CapabilityReq caps[AT_MAX_DEPS];

    /* Forge exercises */
    int exercise_count;
    Exercise exercises[AT_MAX_EXERCISES];

    /* Contexts where this ability applies */
    int context_count;
    Context contexts[AT_MAX_CONTEXTS];

    /* Metadata */
    uint64_t created_at;
    uint64_t updated_at;
    char author[AT_MAX_NAME_LEN];
    int  verified;         /* Has been verified in some context */
    float quality_score;   /* 0.0-1.0, overall quality assessment */
} Skill;

/* ── Transfer Protocol ──────────────────────────────────────── */

typedef enum {
    TP_MSG_OFFER = 0,      /* Agent offers a skill */
    TP_MSG_REQUEST,        /* Agent requests a skill */
    TP_MSG_ACCEPT,         /* Receiver accepts transfer */
    TP_MSG_REJECT,         /* Receiver rejects transfer */
    TP_MSG_QUERY_COMPAT,   /* Check compatibility before transfer */
    TP_MSG_COMPAT_RESULT,  /* Compatibility result */
    TP_MSG_ADAPT,          /* Request adaptation for context */
    TP_MSG_ADAPT_RESULT,   /* Adapted skill returned */
    TP_MSG_VERIFY,         /* Request verification */
    TP_MSG_VERIFY_RESULT,  /* Verification result */
    TP_MSG_ACKNOWLEDGE     /* Final acknowledgment */
} TransferMessageType;

typedef struct {
    TransferMessageType type;
    char sender[AT_MAX_NAME_LEN];
    char receiver[AT_MAX_NAME_LEN];
    char skill_name[AT_MAX_NAME_LEN];
    char target_context[AT_MAX_TAG_LEN];

    /* Payload: serialized skill data for OFFER/ADAPT_RESULT */
    uint8_t payload[AT_MAX_PROTOCOL_SIZE];
    uint32_t payload_len;

    /* Metadata */
    uint64_t timestamp;
    uint32_t sequence;
    char session_id[64];

    /* Compatibility result (for COMPAT_RESULT) */
    float compatibility_score;
    char compatibility_reason[AT_MAX_DESC_LEN];

    /* Verification result */
    int verify_passed;
    char verify_detail[AT_MAX_DESC_LEN];
} TransferMessage;

/* ── Compatibility Checking ─────────────────────────────────── */

typedef struct {
    float overall_score;          /* 0.0-1.0 */
    float category_match;         /* category compatibility */
    float dependency_match;       /* how many deps are satisfied */
    float capability_match;       /* how many caps are available */
    float context_overlap;        /* context similarity */
    float mastery_gap;            /* how much mastery differs */
    int   missing_deps_count;
    char  missing_deps[AT_MAX_DEPS][AT_MAX_NAME_LEN];
    char  recommendation[AT_MAX_DESC_LEN];
} CompatibilityResult;

/* ── Skill Adaptation ───────────────────────────────────────── */

typedef struct {
    Skill original;
    Skill adapted;
    char target_context[AT_MAX_TAG_LEN];
    float adaptation_score;       /* How well adapted (0.0-1.0) */
    int   exercises_added;
    int   exercises_modified;
    char  adaptation_notes[AT_MAX_DESC_LEN];
} AdaptationResult;

/* ── Knowledge Graph ────────────────────────────────────────── */

typedef struct {
    int  id;
    char name[AT_MAX_NAME_LEN];
    SkillCategory category;
    MasteryLevel mastery;
    ForgeStage forge_stage;
} GraphNode;

typedef struct {
    int from_id;
    int to_id;
    char relationship[AT_MAX_TAG_LEN]; /* "depends_on", "enables", "similar_to", "strengthens" */
    float weight;                      /* 0.0-1.0 strength */
} GraphEdge;

typedef struct {
    GraphNode nodes[AT_MAX_NODES];
    int node_count;
    GraphEdge edges[AT_MAX_EDGES];
    int edge_count;
} KnowledgeGraph;

/* ── Verification ───────────────────────────────────────────── */

typedef struct {
    int  passed;
    int  total_tests;
    int  passed_tests;
    float pass_rate;
    char results[AT_MAX_EXERCISES][AT_MAX_DESC_LEN];
    int  result_count;
    char summary[AT_MAX_DESC_LEN];
} VerificationResult;

/* ── API: Skill Management ──────────────────────────────────── */

void skill_init(Skill* s);
int  skill_set_name(Skill* s, const char* name);
int  skill_set_description(Skill* s, const char* desc);
int  skill_set_category(Skill* s, SkillCategory cat);
int  skill_set_mastery(Skill* s, MasteryLevel level);
int  skill_set_forge_stage(Skill* s, ForgeStage stage);
int  skill_add_tag(Skill* s, const char* tag);
int  skill_add_dependency(Skill* s, const char* name, MasteryLevel min_level);
int  skill_add_capability(Skill* s, const char* name, int min_level);
int  skill_add_exercise(Skill* s, const char* name, const char* desc,
                        ForgeStage stage, int difficulty, int reps, float success_rate);
int  skill_add_context(Skill* s, const char* domain, const char* desc, float score);
int  skill_has_tag(const Skill* s, const char* tag);
int  skill_clone(const Skill* src, Skill* dst);

/* ── API: Transfer Protocol ─────────────────────────────────── */

void transfer_message_init(TransferMessage* msg);
int  transfer_serialize(const Skill* s, uint8_t* buf, uint32_t* len);
int  transfer_deserialize(const uint8_t* buf, uint32_t len, Skill* s);
int  transfer_create_offer(TransferMessage* msg, const char* sender,
                           const char* receiver, const Skill* skill);
int  transfer_create_request(TransferMessage* msg, const char* sender,
                             const char* receiver, const char* skill_name);
int  transfer_create_accept(TransferMessage* msg, const char* sender,
                            const char* receiver, const char* skill_name);
int  transfer_create_reject(TransferMessage* msg, const char* sender,
                            const char* receiver, const char* skill_name,
                            const char* reason);
int  transfer_create_compat_query(TransferMessage* msg, const char* sender,
                                  const char* receiver, const Skill* skill);
int  transfer_create_verify(TransferMessage* msg, const char* sender,
                            const char* receiver, const Skill* skill);
const char* transfer_message_type_name(TransferMessageType type);

/* ── API: Compatibility Checking ────────────────────────────── */

void compatibility_check(const Skill* source, const Skill* target_agent,
                         CompatibilityResult* result);
int  compatibility_satisfies_deps(const Skill* skill, const Skill* agent_skills,
                                   int agent_skill_count);
float compatibility_category_score(SkillCategory a, SkillCategory b);

/* ── API: Skill Adaptation ──────────────────────────────────── */

void adapt_skill(const Skill* source, const char* target_context,
                 const Skill* agent_skills, int agent_skill_count,
                 AdaptationResult* result);
void adapt_adjust_mastery(AdaptationResult* result, MasteryLevel target);
void adapt_add_bridge_exercises(AdaptationResult* result,
                                const Skill* gap_skills, int gap_count);

/* ── API: Knowledge Graph ───────────────────────────────────── */

void graph_init(KnowledgeGraph* g);
int  graph_add_node(KnowledgeGraph* g, const char* name,
                    SkillCategory cat, MasteryLevel mastery, ForgeStage stage);
int  graph_add_edge(KnowledgeGraph* g, int from, int to,
                    const char* relationship, float weight);
int  graph_find_node(const KnowledgeGraph* g, const char* name);
int  graph_get_dependencies(const KnowledgeGraph* g, int node_id,
                            int* dep_ids, int max_deps);
int  graph_get_dependents(const KnowledgeGraph* g, int node_id,
                           int* dep_ids, int max_deps);
int  graph_topological_sort(const KnowledgeGraph* g, int* order, int max_order);
float graph_path_strength(const KnowledgeGraph* g, int from, int to);
int  graph_find_learning_path(const KnowledgeGraph* g, int from, int to,
                               int* path, int max_path);

/* ── API: Verification ──────────────────────────────────────── */

void verify_skill(const Skill* s, VerificationResult* result);
int  verify_has_min_exercises(const Skill* s, int min_count);
int  verify_has_forge_progression(const Skill* s);
int  verify_has_context_coverage(const Skill* s, int min_contexts);
float verify_estimated_reliability(const Skill* s);

/* ── Utility ────────────────────────────────────────────────── */

const char* skill_category_name(SkillCategory cat);
const char* mastery_level_name(MasteryLevel level);
const char* forge_stage_name(ForgeStage stage);

#ifdef __cplusplus
}
#endif
#endif
