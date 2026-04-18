/*
 * Ability Transfer System — Core Implementation
 *
 * Implements skill representation, transfer protocol, compatibility checking,
 * skill adaptation, verification, and knowledge graph management.
 *
 * Copyright (c) 2026 SuperInstance, MIT License
 */

#include "ability_transfer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ── Utility: Names ─────────────────────────────────────────── */

const char* skill_category_name(SkillCategory cat) {
    switch (cat) {
        case AT_SKILL_COGNITIVE:    return "cognitive";
        case AT_SKILL_BEHAVIORAL:   return "behavioral";
        case AT_SKILL_TECHNICAL:    return "technical";
        case AT_SKILL_CREATIVITY:   return "creativity";
        case AT_SKILL_JUDGMENT:     return "judgment";
        case AT_SKILL_METALEARNING: return "metalearning";
        case AT_SKILL_ADAPTATION:   return "adaptation";
        default:                    return "unknown";
    }
}

const char* mastery_level_name(MasteryLevel level) {
    switch (level) {
        case AT_MASTERY_NOVICE:       return "novice";
        case AT_MASTERY_BEGINNER:     return "beginner";
        case AT_MASTERY_INTERMEDIATE: return "intermediate";
        case AT_MASTERY_ADVANCED:     return "advanced";
        case AT_MASTERY_EXPERT:       return "expert";
        case AT_MASTERY_MASTER:       return "master";
        default:                      return "unknown";
    }
}

const char* forge_stage_name(ForgeStage stage) {
    switch (stage) {
        case AT_FORGE_NONE:     return "none";
        case AT_FORGE_HEATED:   return "heated";
        case AT_FORGE_HAMMERED: return "hammered";
        case AT_FORGE_QUENCHED: return "quenched";
        case AT_FORGE_TEMPERED: return "tempered";
        default:                return "unknown";
    }
}

const char* transfer_message_type_name(TransferMessageType type) {
    switch (type) {
        case TP_MSG_OFFER:         return "OFFER";
        case TP_MSG_REQUEST:       return "REQUEST";
        case TP_MSG_ACCEPT:        return "ACCEPT";
        case TP_MSG_REJECT:        return "REJECT";
        case TP_MSG_QUERY_COMPAT:  return "QUERY_COMPAT";
        case TP_MSG_COMPAT_RESULT: return "COMPAT_RESULT";
        case TP_MSG_ADAPT:         return "ADAPT";
        case TP_MSG_ADAPT_RESULT:  return "ADAPT_RESULT";
        case TP_MSG_VERIFY:        return "VERIFY";
        case TP_MSG_VERIFY_RESULT: return "VERIFY_RESULT";
        case TP_MSG_ACKNOWLEDGE:   return "ACKNOWLEDGE";
        default:                   return "UNKNOWN";
    }
}

/* ── Skill Management ───────────────────────────────────────── */

void skill_init(Skill* s) {
    memset(s, 0, sizeof(*s));
    s->category = AT_SKILL_UNKNOWN;
    s->mastery = AT_MASTERY_NOVICE;
    s->forge_stage = AT_FORGE_NONE;
    s->quality_score = 0.0f;
    s->verified = 0;
    s->created_at = (uint64_t)time(NULL);
    s->updated_at = s->created_at;
}

int skill_set_name(Skill* s, const char* name) {
    if (!name || !*name) return -1;
    snprintf(s->name, AT_MAX_NAME_LEN, "%s", name);
    s->updated_at = (uint64_t)time(NULL);
    return 0;
}

int skill_set_description(Skill* s, const char* desc) {
    if (!desc) return -1;
    snprintf(s->description, AT_MAX_DESC_LEN, "%s", desc);
    s->updated_at = (uint64_t)time(NULL);
    return 0;
}

int skill_set_category(Skill* s, SkillCategory cat) {
    s->category = cat;
    s->updated_at = (uint64_t)time(NULL);
    return 0;
}

int skill_set_mastery(Skill* s, MasteryLevel level) {
    s->mastery = level;
    s->updated_at = (uint64_t)time(NULL);
    return 0;
}

int skill_set_forge_stage(Skill* s, ForgeStage stage) {
    s->forge_stage = stage;
    s->updated_at = (uint64_t)time(NULL);
    return 0;
}

int skill_add_tag(Skill* s, const char* tag) {
    if (!tag || s->tag_count >= AT_MAX_TAGS) return -1;
    if (skill_has_tag(s, tag)) return 1; /* already present */
    snprintf(s->tags[s->tag_count], AT_MAX_TAG_LEN, "%s", tag);
    s->tag_count++;
    s->updated_at = (uint64_t)time(NULL);
    return 0;
}

int skill_add_dependency(Skill* s, const char* name, MasteryLevel min_level) {
    if (!name || s->dep_count >= AT_MAX_DEPS) return -1;
    snprintf(s->deps[s->dep_count].skill_name, AT_MAX_NAME_LEN, "%s", name);
    s->deps[s->dep_count].min_mastery = min_level;
    s->dep_count++;
    s->updated_at = (uint64_t)time(NULL);
    return 0;
}

int skill_add_capability(Skill* s, const char* name, int min_level) {
    if (!name || s->cap_count >= AT_MAX_DEPS) return -1;
    snprintf(s->caps[s->cap_count].name, AT_MAX_TAG_LEN, "%s", name);
    s->caps[s->cap_count].min_level = min_level;
    s->cap_count++;
    s->updated_at = (uint64_t)time(NULL);
    return 0;
}

int skill_add_exercise(Skill* s, const char* name, const char* desc,
                       ForgeStage stage, int difficulty, int reps, float success_rate) {
    if (!name || s->exercise_count >= AT_MAX_EXERCISES) return -1;
    Exercise* e = &s->exercises[s->exercise_count];
    snprintf(e->name, AT_MAX_NAME_LEN, "%s", name);
    if (desc) snprintf(e->description, AT_MAX_DESC_LEN, "%s", desc);
    e->stage = stage;
    e->difficulty = difficulty;
    e->repetitions = reps;
    e->success_rate = success_rate;
    s->exercise_count++;
    s->updated_at = (uint64_t)time(NULL);
    return 0;
}

int skill_add_context(Skill* s, const char* domain, const char* desc, float score) {
    if (!domain || s->context_count >= AT_MAX_CONTEXTS) return -1;
    Context* c = &s->contexts[s->context_count];
    snprintf(c->domain, AT_MAX_TAG_LEN, "%s", domain);
    if (desc) snprintf(c->description, AT_MAX_DESC_LEN, "%s", desc);
    c->transfer_score = score;
    s->context_count++;
    s->updated_at = (uint64_t)time(NULL);
    return 0;
}

int skill_has_tag(const Skill* s, const char* tag) {
    if (!tag) return 0;
    for (int i = 0; i < s->tag_count; i++) {
        if (strcmp(s->tags[i], tag) == 0) return 1;
    }
    return 0;
}

int skill_clone(const Skill* src, Skill* dst) {
    if (!src || !dst) return -1;
    memcpy(dst, src, sizeof(Skill));
    return 0;
}

/* ── Transfer Protocol ──────────────────────────────────────── */

void transfer_message_init(TransferMessage* msg) {
    memset(msg, 0, sizeof(*msg));
    msg->timestamp = (uint64_t)time(NULL);
}

/* Simple text-based serialization format for skills */
int transfer_serialize(const Skill* s, uint8_t* buf, uint32_t* len) {
    if (!s || !buf || !len) return -1;
    uint32_t pos = 0;

    /* Header: "ATSK" magic */
    if (pos + 4 > AT_MAX_PROTOCOL_SIZE) return -1;
    memcpy(buf + pos, "ATSK", 4); pos += 4;

    /* Name (length-prefixed string) */
    uint8_t name_len = (uint8_t)strlen(s->name);
    if (pos + 1 + name_len > AT_MAX_PROTOCOL_SIZE) return -1;
    buf[pos++] = name_len;
    memcpy(buf + pos, s->name, name_len); pos += name_len;

    /* Category, mastery, forge_stage */
    if (pos + 3 > AT_MAX_PROTOCOL_SIZE) return -1;
    buf[pos++] = (uint8_t)s->category;
    buf[pos++] = (uint8_t)s->mastery;
    buf[pos++] = (uint8_t)s->forge_stage;

    /* Tag count + tags */
    if (pos + 1 > AT_MAX_PROTOCOL_SIZE) return -1;
    buf[pos++] = (uint8_t)s->tag_count;
    for (int i = 0; i < s->tag_count; i++) {
        uint8_t tlen = (uint8_t)strlen(s->tags[i]);
        if (pos + 1 + tlen > AT_MAX_PROTOCOL_SIZE) return -1;
        buf[pos++] = tlen;
        memcpy(buf + pos, s->tags[i], tlen); pos += tlen;
    }

    /* Dep count + deps */
    if (pos + 1 > AT_MAX_PROTOCOL_SIZE) return -1;
    buf[pos++] = (uint8_t)s->dep_count;
    for (int i = 0; i < s->dep_count; i++) {
        uint8_t dlen = (uint8_t)strlen(s->deps[i].skill_name);
        if (pos + 2 + dlen > AT_MAX_PROTOCOL_SIZE) return -1;
        buf[pos++] = dlen;
        memcpy(buf + pos, s->deps[i].skill_name, dlen); pos += dlen;
        buf[pos++] = (uint8_t)s->deps[i].min_mastery;
    }

    /* Exercise count + exercises (simplified) */
    if (pos + 1 > AT_MAX_PROTOCOL_SIZE) return -1;
    buf[pos++] = (uint8_t)s->exercise_count;
    for (int i = 0; i < s->exercise_count; i++) {
        uint8_t nlen = (uint8_t)strlen(s->exercises[i].name);
        if (pos + 5 + nlen > AT_MAX_PROTOCOL_SIZE) return -1;
        buf[pos++] = nlen;
        memcpy(buf + pos, s->exercises[i].name, nlen); pos += nlen;
        buf[pos++] = (uint8_t)s->exercises[i].stage;
        buf[pos++] = (uint8_t)s->exercises[i].difficulty;
        buf[pos++] = (uint8_t)s->exercises[i].repetitions;
        /* success_rate as uint8 0-255 mapped to 0.0-1.0 */
        buf[pos++] = (uint8_t)(s->exercises[i].success_rate * 255);
    }

    /* Context count + contexts */
    if (pos + 1 > AT_MAX_PROTOCOL_SIZE) return -1;
    buf[pos++] = (uint8_t)s->context_count;
    for (int i = 0; i < s->context_count; i++) {
        uint8_t dlen = (uint8_t)strlen(s->contexts[i].domain);
        if (pos + 2 + dlen > AT_MAX_PROTOCOL_SIZE) return -1;
        buf[pos++] = dlen;
        memcpy(buf + pos, s->contexts[i].domain, dlen); pos += dlen;
        buf[pos++] = (uint8_t)(s->contexts[i].transfer_score * 255);
    }

    /* Quality score and verified flag */
    if (pos + 2 > AT_MAX_PROTOCOL_SIZE) return -1;
    buf[pos++] = (uint8_t)(s->quality_score * 255);
    buf[pos++] = s->verified ? 1 : 0;

    *len = pos;
    return 0;
}

int transfer_deserialize(const uint8_t* buf, uint32_t len, Skill* s) {
    if (!buf || !s || len < 4) return -1;
    skill_init(s);

    /* Check magic */
    if (memcmp(buf, "ATSK", 4) != 0) return -1;
    uint32_t pos = 4;

    /* Name */
    if (pos >= len) return -1;
    uint8_t name_len = buf[pos++];
    if (pos + name_len > len) return -1;
    memcpy(s->name, buf + pos, name_len); s->name[name_len] = '\0';
    pos += name_len;

    /* Category, mastery, forge_stage */
    if (pos + 3 > len) return -1;
    s->category = (SkillCategory)buf[pos++];
    s->mastery = (MasteryLevel)buf[pos++];
    s->forge_stage = (ForgeStage)buf[pos++];

    /* Tags */
    if (pos >= len) return -1;
    uint8_t tag_count = buf[pos++];
    for (int i = 0; i < tag_count && i < AT_MAX_TAGS; i++) {
        if (pos >= len) return -1;
        uint8_t tlen = buf[pos++];
        if (pos + tlen > len) return -1;
        memcpy(s->tags[i], buf + pos, tlen); s->tags[i][tlen] = '\0';
        pos += tlen;
    }
    s->tag_count = tag_count < AT_MAX_TAGS ? tag_count : AT_MAX_TAGS;

    /* Dependencies */
    if (pos >= len) return -1;
    uint8_t dep_count = buf[pos++];
    for (int i = 0; i < dep_count && i < AT_MAX_DEPS; i++) {
        if (pos >= len) return -1;
        uint8_t dlen = buf[pos++];
        if (pos + dlen > len) return -1;
        memcpy(s->deps[i].skill_name, buf + pos, dlen);
        s->deps[i].skill_name[dlen] = '\0';
        pos += dlen;
        if (pos >= len) return -1;
        s->deps[i].min_mastery = (MasteryLevel)buf[pos++];
    }
    s->dep_count = dep_count < AT_MAX_DEPS ? dep_count : AT_MAX_DEPS;

    /* Exercises */
    if (pos >= len) return -1;
    uint8_t ex_count = buf[pos++];
    for (int i = 0; i < ex_count && i < AT_MAX_EXERCISES; i++) {
        if (pos >= len) return -1;
        uint8_t nlen = buf[pos++];
        if (pos + nlen > len) return -1;
        memcpy(s->exercises[i].name, buf + pos, nlen);
        s->exercises[i].name[nlen] = '\0';
        pos += nlen;
        if (pos + 4 > len) return -1;
        s->exercises[i].stage = (ForgeStage)buf[pos++];
        s->exercises[i].difficulty = buf[pos++];
        s->exercises[i].repetitions = buf[pos++];
        s->exercises[i].success_rate = buf[pos++] / 255.0f;
    }
    s->exercise_count = ex_count < AT_MAX_EXERCISES ? ex_count : AT_MAX_EXERCISES;

    /* Contexts */
    if (pos >= len) return -1;
    uint8_t ctx_count = buf[pos++];
    for (int i = 0; i < ctx_count && i < AT_MAX_CONTEXTS; i++) {
        if (pos >= len) return -1;
        uint8_t dlen = buf[pos++];
        if (pos + dlen > len) return -1;
        memcpy(s->contexts[i].domain, buf + pos, dlen);
        s->contexts[i].domain[dlen] = '\0';
        pos += dlen;
        if (pos >= len) return -1;
        s->contexts[i].transfer_score = buf[pos++] / 255.0f;
    }
    s->context_count = ctx_count < AT_MAX_CONTEXTS ? ctx_count : AT_MAX_CONTEXTS;

    /* Quality and verified */
    if (pos + 2 > len) return -1;
    s->quality_score = buf[pos++] / 255.0f;
    s->verified = buf[pos++] ? 1 : 0;

    return 0;
}

int transfer_create_offer(TransferMessage* msg, const char* sender,
                          const char* receiver, const Skill* skill) {
    if (!msg || !sender || !receiver || !skill) return -1;
    transfer_message_init(msg);
    msg->type = TP_MSG_OFFER;
    snprintf(msg->sender, AT_MAX_NAME_LEN, "%s", sender);
    snprintf(msg->receiver, AT_MAX_NAME_LEN, "%s", receiver);
    snprintf(msg->skill_name, AT_MAX_NAME_LEN, "%s", skill->name);
    msg->payload_len = AT_MAX_PROTOCOL_SIZE;
    return transfer_serialize(skill, msg->payload, &msg->payload_len);
}

int transfer_create_request(TransferMessage* msg, const char* sender,
                            const char* receiver, const char* skill_name) {
    if (!msg || !sender || !receiver || !skill_name) return -1;
    transfer_message_init(msg);
    msg->type = TP_MSG_REQUEST;
    snprintf(msg->sender, AT_MAX_NAME_LEN, "%s", sender);
    snprintf(msg->receiver, AT_MAX_NAME_LEN, "%s", receiver);
    snprintf(msg->skill_name, AT_MAX_NAME_LEN, "%s", skill_name);
    return 0;
}

int transfer_create_accept(TransferMessage* msg, const char* sender,
                           const char* receiver, const char* skill_name) {
    if (!msg || !sender || !receiver || !skill_name) return -1;
    transfer_message_init(msg);
    msg->type = TP_MSG_ACCEPT;
    snprintf(msg->sender, AT_MAX_NAME_LEN, "%s", sender);
    snprintf(msg->receiver, AT_MAX_NAME_LEN, "%s", receiver);
    snprintf(msg->skill_name, AT_MAX_NAME_LEN, "%s", skill_name);
    return 0;
}

int transfer_create_reject(TransferMessage* msg, const char* sender,
                           const char* receiver, const char* skill_name,
                           const char* reason) {
    if (!msg || !sender || !receiver || !skill_name) return -1;
    transfer_message_init(msg);
    msg->type = TP_MSG_REJECT;
    snprintf(msg->sender, AT_MAX_NAME_LEN, "%s", sender);
    snprintf(msg->receiver, AT_MAX_NAME_LEN, "%s", receiver);
    snprintf(msg->skill_name, AT_MAX_NAME_LEN, "%s", skill_name);
    if (reason) snprintf(msg->verify_detail, AT_MAX_DESC_LEN, "%s", reason);
    return 0;
}

int transfer_create_compat_query(TransferMessage* msg, const char* sender,
                                 const char* receiver, const Skill* skill) {
    if (!msg || !sender || !receiver || !skill) return -1;
    transfer_message_init(msg);
    msg->type = TP_MSG_QUERY_COMPAT;
    snprintf(msg->sender, AT_MAX_NAME_LEN, "%s", sender);
    snprintf(msg->receiver, AT_MAX_NAME_LEN, "%s", receiver);
    snprintf(msg->skill_name, AT_MAX_NAME_LEN, "%s", skill->name);
    msg->payload_len = AT_MAX_PROTOCOL_SIZE;
    return transfer_serialize(skill, msg->payload, &msg->payload_len);
}

int transfer_create_verify(TransferMessage* msg, const char* sender,
                           const char* receiver, const Skill* skill) {
    if (!msg || !sender || !receiver || !skill) return -1;
    transfer_message_init(msg);
    msg->type = TP_MSG_VERIFY;
    snprintf(msg->sender, AT_MAX_NAME_LEN, "%s", sender);
    snprintf(msg->receiver, AT_MAX_NAME_LEN, "%s", receiver);
    snprintf(msg->skill_name, AT_MAX_NAME_LEN, "%s", skill->name);
    msg->payload_len = AT_MAX_PROTOCOL_SIZE;
    return transfer_serialize(skill, msg->payload, &msg->payload_len);
}

/* ── Compatibility Checking ──────────────────────────────────── */

float compatibility_category_score(SkillCategory a, SkillCategory b) {
    if (a == b) return 1.0f;
    /* Related categories get partial credit */
    if ((a == AT_SKILL_JUDGMENT && b == AT_SKILL_COGNITIVE) ||
        (a == AT_SKILL_COGNITIVE && b == AT_SKILL_JUDGMENT))
        return 0.7f;
    if ((a == AT_SKILL_ADAPTATION && b == AT_SKILL_METALEARNING) ||
        (a == AT_SKILL_METALEARNING && b == AT_SKILL_ADAPTATION))
        return 0.6f;
    /* All categories have some overlap */
    return 0.3f;
}

void compatibility_check(const Skill* source, const Skill* target_agent,
                         CompatibilityResult* result) {
    if (!source || !target_agent || !result) return;
    memset(result, 0, sizeof(*result));

    /* Category match */
    result->category_match = compatibility_category_score(source->category,
                                                          target_agent->category);

    /* Capability match (simplified: count how many source caps target agent has) */
    int cap_matched = 0;
    for (int i = 0; i < source->cap_count; i++) {
        for (int j = 0; j < target_agent->cap_count; j++) {
            if (strcmp(source->caps[i].name, target_agent->caps[j].name) == 0) {
                cap_matched++;
                break;
            }
        }
    }
    result->capability_match = source->cap_count > 0
        ? (float)cap_matched / source->cap_count : 1.0f;

    /* Mastery gap: smaller gap = better compatibility */
    int gap = (int)source->mastery - (int)target_agent->mastery;
    if (gap < 0) gap = -gap;
    result->mastery_gap = gap <= 0 ? 1.0f : 1.0f / (1.0f + gap * 0.3f);

    /* Context overlap */
    int ctx_overlap = 0;
    for (int i = 0; i < source->context_count; i++) {
        for (int j = 0; j < target_agent->context_count; j++) {
            if (strcmp(source->contexts[i].domain, target_agent->contexts[j].domain) == 0) {
                ctx_overlap++;
                break;
            }
        }
    }
    result->context_overlap = (source->context_count > 0 || target_agent->context_count > 0)
        ? (2.0f * ctx_overlap) / (source->context_count + target_agent->context_count) : 0.5f;

    /* Weighted overall score */
    result->overall_score = result->category_match * 0.25f +
                            result->capability_match * 0.25f +
                            result->mastery_gap * 0.20f +
                            result->context_overlap * 0.30f;

    /* Dependency check placeholder (needs knowledge graph for full check) */
    result->missing_deps_count = source->dep_count; /* worst case: assume all missing */
    result->dependency_match = source->dep_count > 0 ? 0.5f : 1.0f;

    /* Recalculate with dependency match */
    result->overall_score = result->category_match * 0.20f +
                            result->capability_match * 0.20f +
                            result->mastery_gap * 0.15f +
                            result->context_overlap * 0.25f +
                            result->dependency_match * 0.20f;

    /* Recommendation */
    if (result->overall_score >= 0.8f) {
        snprintf(result->recommendation, AT_MAX_DESC_LEN,
                 "Excellent fit. Proceed with direct transfer.");
    } else if (result->overall_score >= 0.6f) {
        snprintf(result->recommendation, AT_MAX_DESC_LEN,
                 "Good fit. Minor adaptation may be needed.");
    } else if (result->overall_score >= 0.4f) {
        snprintf(result->recommendation, AT_MAX_DESC_LEN,
                 "Moderate fit. Adaptation exercises recommended.");
    } else {
        snprintf(result->recommendation, AT_MAX_DESC_LEN,
                 "Poor fit. Significant prerequisites may be missing.");
    }
}

int compatibility_satisfies_deps(const Skill* skill, const Skill* agent_skills,
                                 int agent_skill_count) {
    if (!skill || !agent_skills) return 0;
    int satisfied = 0;
    for (int i = 0; i < skill->dep_count; i++) {
        for (int j = 0; j < agent_skill_count; j++) {
            if (strcmp(skill->deps[i].skill_name, agent_skills[j].name) == 0 &&
                agent_skills[j].mastery >= skill->deps[i].min_mastery) {
                satisfied++;
                break;
            }
        }
    }
    return skill->dep_count > 0 ? satisfied : 1;
}

/* ── Skill Adaptation ───────────────────────────────────────── */

void adapt_skill(const Skill* source, const char* target_context,
                 const Skill* agent_skills, int agent_skill_count,
                 AdaptationResult* result) {
    if (!source || !result) return;
    memset(result, 0, sizeof(*result));

    result->original = *source;
    skill_clone(source, &result->adapted);

    if (target_context) {
        snprintf(result->target_context, AT_MAX_TAG_LEN, "%s", target_context);
    }

    /* Check if the skill already covers this context */
    int context_exists = 0;
    for (int i = 0; i < result->adapted.context_count; i++) {
        if (target_context && strcmp(result->adapted.contexts[i].domain, target_context) == 0) {
            context_exists = 1;
            break;
        }
    }

    /* If context is new, add bridge exercises */
    if (!context_exists && target_context && agent_skill_count > 0) {
        /* Add the target context with a moderate transfer score */
        skill_add_context(&result->adapted, target_context,
                         "Adapted for target context", 0.6f);

        /* Add bridge exercises for the new context */
        char ex_name[AT_MAX_NAME_LEN];
        char ex_desc[AT_MAX_DESC_LEN];

        snprintf(ex_name, AT_MAX_NAME_LEN, "bridge_%s_familiarization", target_context);
        snprintf(ex_desc, AT_MAX_DESC_LEN,
                 "Familiarization exercise for %s context", target_context);
        skill_add_exercise(&result->adapted, ex_name, ex_desc,
                          AT_FORGE_HEATED, 3, 5, 0.7f);
        result->exercises_added++;

        snprintf(ex_name, AT_MAX_NAME_LEN, "bridge_%s_practice", target_context);
        snprintf(ex_desc, AT_MAX_DESC_LEN,
                 "Practice applying skill in %s context", target_context);
        skill_add_exercise(&result->adapted, ex_name, ex_desc,
                          AT_FORGE_HAMMERED, 5, 10, 0.6f);
        result->exercises_added++;

        snprintf(ex_name, AT_MAX_NAME_LEN, "bridge_%s_tempering", target_context);
        snprintf(ex_desc, AT_MAX_DESC_LEN,
                 "Tempering: verify skill in %s edge cases", target_context);
        skill_add_exercise(&result->adapted, ex_name, ex_desc,
                          AT_FORGE_TEMPERED, 7, 3, 0.5f);
        result->exercises_added++;
    }

    /* Adjust mastery based on agent's existing skill level */
    MasteryLevel max_agent_mastery = AT_MASTERY_NOVICE;
    for (int i = 0; i < agent_skill_count; i++) {
        if (strcmp(agent_skills[i].name, source->name) == 0) {
            max_agent_mastery = agent_skills[i].mastery;
            break;
        }
    }
    if (max_agent_mastery > AT_MASTERY_NOVICE) {
        result->adapted.mastery = max_agent_mastery;
    }

    /* Calculate adaptation score */
    result->adaptation_score = 0.7f + 0.1f * (result->exercises_added > 0 ? 1 : 0);
    if (context_exists) result->adaptation_score += 0.1f;

    snprintf(result->adaptation_notes, AT_MAX_DESC_LEN,
             "Adapted for %s with %d new exercises",
             target_context ? target_context : "general context",
             result->exercises_added);
}

void adapt_adjust_mastery(AdaptationResult* result, MasteryLevel target) {
    if (!result) return;
    result->adapted.mastery = target;
    result->adapted.updated_at = (uint64_t)time(NULL);
}

void adapt_add_bridge_exercises(AdaptationResult* result,
                                const Skill* gap_skills, int gap_count) {
    if (!result || !gap_skills) return;
    for (int i = 0; i < gap_count; i++) {
        char name[AT_MAX_NAME_LEN];
        char desc[AT_MAX_DESC_LEN];
        snprintf(name, AT_MAX_NAME_LEN, "prereq_%s_bridge", gap_skills[i].name);
        snprintf(desc, AT_MAX_DESC_LEN,
                 "Bridge exercise for prerequisite: %s", gap_skills[i].name);
        skill_add_exercise(&result->adapted, name, desc,
                          AT_FORGE_HEATED, gap_skills[i].mastery + 1, 5, 0.65f);
        result->exercises_added++;
    }
}

/* ── Knowledge Graph ────────────────────────────────────────── */

void graph_init(KnowledgeGraph* g) {
    memset(g, 0, sizeof(*g));
}

int graph_add_node(KnowledgeGraph* g, const char* name,
                   SkillCategory cat, MasteryLevel mastery, ForgeStage stage) {
    if (!name || g->node_count >= AT_MAX_NODES) return -1;
    GraphNode* n = &g->nodes[g->node_count];
    n->id = g->node_count;
    snprintf(n->name, AT_MAX_NAME_LEN, "%s", name);
    n->category = cat;
    n->mastery = mastery;
    n->forge_stage = stage;
    g->node_count++;
    return n->id;
}

int graph_add_edge(KnowledgeGraph* g, int from, int to,
                   const char* relationship, float weight) {
    if (!relationship || g->edge_count >= AT_MAX_EDGES) return -1;
    if (from < 0 || from >= g->node_count || to < 0 || to >= g->node_count) return -1;
    GraphEdge* e = &g->edges[g->edge_count];
    e->from_id = from;
    e->to_id = to;
    snprintf(e->relationship, AT_MAX_TAG_LEN, "%s", relationship);
    e->weight = weight;
    g->edge_count++;
    return 0;
}

int graph_find_node(const KnowledgeGraph* g, const char* name) {
    if (!name) return -1;
    for (int i = 0; i < g->node_count; i++) {
        if (strcmp(g->nodes[i].name, name) == 0) return i;
    }
    return -1;
}

int graph_get_dependencies(const KnowledgeGraph* g, int node_id,
                            int* dep_ids, int max_deps) {
    if (!g || node_id < 0 || node_id >= g->node_count) return 0;
    int count = 0;
    /* Dependencies: edges where this node FROM depends ON TO */
    for (int i = 0; i < g->edge_count && count < max_deps; i++) {
        if (g->edges[i].from_id == node_id &&
            strcmp(g->edges[i].relationship, "depends_on") == 0) {
            dep_ids[count++] = g->edges[i].to_id;
        }
    }
    return count;
}

int graph_get_dependents(const KnowledgeGraph* g, int node_id,
                           int* dep_ids, int max_deps) {
    if (!g || node_id < 0 || node_id >= g->node_count) return 0;
    int count = 0;
    /* Dependents: edges where TO is this node */
    for (int i = 0; i < g->edge_count && count < max_deps; i++) {
        if (g->edges[i].to_id == node_id &&
            strcmp(g->edges[i].relationship, "depends_on") == 0) {
            dep_ids[count++] = g->edges[i].from_id;
        }
    }
    return count;
}

/* Topological sort using Kahn's algorithm */
int graph_topological_sort(const KnowledgeGraph* g, int* order, int max_order) {
    if (!g || !order) return -1;
    int n = g->node_count;
    if (n == 0) return 0;
    if (n > max_order) return -1;

    int in_degree[AT_MAX_NODES] = {0};
    int queue[AT_MAX_NODES];
    int qfront = 0, qback = 0;

    /* Calculate in-degrees: for "depends_on" edge from->to,
     * from depends on to, so from has an in-degree (to must come first) */
    for (int i = 0; i < g->edge_count; i++) {
        if (strcmp(g->edges[i].relationship, "depends_on") == 0) {
            in_degree[g->edges[i].from_id]++;
        }
    }

    /* Start with nodes that have no dependencies */
    for (int i = 0; i < n; i++) {
        if (in_degree[i] == 0) {
            queue[qback++] = i;
        }
    }

    int count = 0;
    while (qfront < qback) {
        int node = queue[qfront++];
        order[count++] = node;

        /* When processing a node, reduce in-degree of nodes that depend on it.
         * For "depends_on" edge from=X, to=node: X depends on node.
         * So processing node should reduce in_degree[X]. */
        for (int i = 0; i < g->edge_count; i++) {
            if (g->edges[i].to_id == node &&
                strcmp(g->edges[i].relationship, "depends_on") == 0) {
                int dependent = g->edges[i].from_id;
                in_degree[dependent]--;
                if (in_degree[dependent] == 0) {
                    queue[qback++] = dependent;
                }
            }
        }
    }

    return count; /* returns sorted count; if < n, there's a cycle */
}

/* BFS to find path strength */
float graph_path_strength(const KnowledgeGraph* g, int from, int to) {
    if (!g || from < 0 || from >= g->node_count || to < 0 || to >= g->node_count)
        return 0.0f;
    if (from == to) return 1.0f;

    /* BFS */
    float dist[AT_MAX_NODES];
    int visited[AT_MAX_NODES] = {0};
    for (int i = 0; i < g->node_count; i++) dist[i] = 0.0f;

    int queue[AT_MAX_NODES];
    int qfront = 0, qback = 0;
    queue[qback++] = from;
    visited[from] = 1;
    dist[from] = 1.0f;

    while (qfront < qback) {
        int node = queue[qfront++];
        for (int i = 0; i < g->edge_count; i++) {
            int neighbor = -1;
            float weight = 0.0f;
            if (g->edges[i].from_id == node) {
                neighbor = g->edges[i].to_id;
            } else if (g->edges[i].to_id == node) {
                neighbor = g->edges[i].from_id;
            }
            if (neighbor >= 0 && !visited[neighbor]) {
                weight = g->edges[i].weight;
                visited[neighbor] = 1;
                dist[neighbor] = dist[node] * weight;
                if (neighbor == to) return dist[neighbor];
                queue[qback++] = neighbor;
            }
        }
    }
    return 0.0f; /* no path found */
}

/* BFS to find shortest path */
int graph_find_learning_path(const KnowledgeGraph* g, int from, int to,
                               int* path, int max_path) {
    if (!g || from < 0 || from >= g->node_count || to < 0 || to >= g->node_count)
        return 0;
    if (from == to) {
        if (max_path > 0) path[0] = from;
        return 1;
    }

    int parent[AT_MAX_NODES];
    int visited[AT_MAX_NODES] = {0};
    for (int i = 0; i < g->node_count; i++) parent[i] = -1;

    int queue[AT_MAX_NODES];
    int qfront = 0, qback = 0;
    queue[qback++] = from;
    visited[from] = 1;

    while (qfront < qback) {
        int node = queue[qfront++];
        for (int i = 0; i < g->edge_count; i++) {
            int neighbor = -1;
            if (g->edges[i].from_id == node) {
                neighbor = g->edges[i].to_id;
            } else if (g->edges[i].to_id == node) {
                neighbor = g->edges[i].from_id;
            }
            if (neighbor >= 0 && !visited[neighbor]) {
                visited[neighbor] = 1;
                parent[neighbor] = node;
                if (neighbor == to) {
                    /* Reconstruct path */
                    int count = 0;
                    for (int cur = to; cur != -1; cur = parent[cur]) {
                        if (count >= max_path) return count;
                        path[count++] = cur;
                    }
                    /* Reverse path */
                    for (int i = 0; i < count / 2; i++) {
                        int tmp = path[i];
                        path[i] = path[count - 1 - i];
                        path[count - 1 - i] = tmp;
                    }
                    return count;
                }
                queue[qback++] = neighbor;
            }
        }
    }
    return 0; /* no path */
}

/* ── Verification ───────────────────────────────────────────── */

void verify_skill(const Skill* s, VerificationResult* result) {
    if (!s || !result) return;
    memset(result, 0, sizeof(*result));

    int total = 0, passed = 0;

    /* Test 1: Has valid name */
    total++;
    if (strlen(s->name) > 0) {
        passed++;
        snprintf(result->results[result->result_count++], AT_MAX_DESC_LEN,
                 "PASS: Has valid name '%s'", s->name);
    } else {
        snprintf(result->results[result->result_count++], AT_MAX_DESC_LEN,
                 "FAIL: Missing skill name");
    }

    /* Test 2: Has category */
    total++;
    if (s->category != AT_SKILL_UNKNOWN) {
        passed++;
        snprintf(result->results[result->result_count++], AT_MAX_DESC_LEN,
                 "PASS: Has valid category '%s'", skill_category_name(s->category));
    } else {
        snprintf(result->results[result->result_count++], AT_MAX_DESC_LEN,
                 "FAIL: Category is unknown");
    }

    /* Test 3: Has description */
    total++;
    if (strlen(s->description) > 10) {
        passed++;
        snprintf(result->results[result->result_count++], AT_MAX_DESC_LEN,
                 "PASS: Has meaningful description (%zu chars)", strlen(s->description));
    } else {
        snprintf(result->results[result->result_count++], AT_MAX_DESC_LEN,
                 "FAIL: Description too short or missing");
    }

    /* Test 4: Has minimum exercises */
    total++;
    if (verify_has_min_exercises(s, 3)) {
        passed++;
        snprintf(result->results[result->result_count++], AT_MAX_DESC_LEN,
                 "PASS: Has %d exercises (>= 3 required)", s->exercise_count);
    } else {
        snprintf(result->results[result->result_count++], AT_MAX_DESC_LEN,
                 "FAIL: Has %d exercises, need at least 3", s->exercise_count);
    }

    /* Test 5: Has forge progression */
    total++;
    if (verify_has_forge_progression(s)) {
        passed++;
        snprintf(result->results[result->result_count++], AT_MAX_DESC_LEN,
                 "PASS: Has forge stage progression");
    } else {
        snprintf(result->results[result->result_count++], AT_MAX_DESC_LEN,
                 "FAIL: Missing forge stage progression");
    }

    /* Test 6: Has context coverage */
    total++;
    if (verify_has_context_coverage(s, 2)) {
        passed++;
        snprintf(result->results[result->result_count++], AT_MAX_DESC_LEN,
                 "PASS: Has %d contexts (>= 2 required)", s->context_count);
    } else {
        snprintf(result->results[result->result_count++], AT_MAX_DESC_LEN,
                 "FAIL: Has %d contexts, need at least 2", s->context_count);
    }

    /* Test 7: Has tags */
    total++;
    if (s->tag_count > 0) {
        passed++;
        snprintf(result->results[result->result_count++], AT_MAX_DESC_LEN,
                 "PASS: Has %d tags", s->tag_count);
    } else {
        snprintf(result->results[result->result_count++], AT_MAX_DESC_LEN,
                 "FAIL: No tags specified");
    }

    /* Test 8: Quality score */
    total++;
    if (s->quality_score > 0.0f) {
        passed++;
        snprintf(result->results[result->result_count++], AT_MAX_DESC_LEN,
                 "PASS: Quality score %.2f", s->quality_score);
    } else {
        snprintf(result->results[result->result_count++], AT_MAX_DESC_LEN,
                 "WARN: Quality score not set");
        passed++; /* Warning, not failure */
    }

    result->total_tests = total;
    result->passed_tests = passed;
    result->pass_rate = total > 0 ? (float)passed / total : 0.0f;
    result->passed = (result->pass_rate >= 0.7f);

    snprintf(result->summary, AT_MAX_DESC_LEN,
             "Verification: %d/%d tests passed (%.0f%%)%s",
             passed, total, result->pass_rate * 100,
             result->passed ? " - PASSED" : " - FAILED");
}

int verify_has_min_exercises(const Skill* s, int min_count) {
    return s && s->exercise_count >= min_count;
}

int verify_has_forge_progression(const Skill* s) {
    if (!s || s->exercise_count < 2) return 0;
    int has_heated = 0, has_hammered = 0;
    int has_quenched = 0, has_tempered = 0;
    (void)has_quenched; (void)has_tempered; /* reserved for future stricter checks */
    for (int i = 0; i < s->exercise_count; i++) {
        switch (s->exercises[i].stage) {
            case AT_FORGE_HEATED:   has_heated = 1; break;
            case AT_FORGE_HAMMERED: has_hammered = 1; break;
            case AT_FORGE_QUENCHED: has_quenched = 1; break;
            case AT_FORGE_TEMPERED: has_tempered = 1; break;
            default: break;
        }
    }
    return has_heated && has_hammered; /* At minimum: heated → hammered */
}

int verify_has_context_coverage(const Skill* s, int min_contexts) {
    return s && s->context_count >= min_contexts;
}

float verify_estimated_reliability(const Skill* s) {
    if (!s) return 0.0f;
    float score = 0.0f;

    /* Mastery contributes 25% */
    score += ((float)s->mastery / (float)AT_MASTERY_MASTER) * 0.25f;

    /* Forge stage contributes 25% */
    score += ((float)s->forge_stage / (float)AT_FORGE_TEMPERED) * 0.25f;

    /* Exercise count (diminishing returns) contributes 25% */
    float ex_score = (s->exercise_count >= 10) ? 1.0f : (float)s->exercise_count / 10.0f;
    score += ex_score * 0.25f;

    /* Context coverage contributes 25% */
    float ctx_score = (s->context_count >= 5) ? 1.0f : (float)s->context_count / 5.0f;
    score += ctx_score * 0.25f;

    return score;
}
