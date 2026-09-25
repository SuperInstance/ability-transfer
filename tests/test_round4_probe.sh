#!/bin/sh
# test_round4_probe.sh — regression test for the repo-extraction probe (Round 4).
#
# Self-hosting: extracts the ability OF THIS REPO from its own frozen attention
# field, then asserts the artifact invariants. Run from repo root:
#   sh tests/test_round4_probe.sh
# Exit 0 = all assertions pass.
set -e
cd "$(dirname "$0")/.."

PROG=./probe_repo_extract
if [ ! -x "$PROG" ]; then
    echo "building probe..."
    ${CC:-gcc} -std=c11 -Wall -Wextra -O2 -Iinclude -o probe_repo_extract \
        tests/probe_repo_extract.c src/ability_transfer.o -lm
fi

LINE=$($PROG . ability-transfer | head -1)
echo "$LINE" | grep -q '^SKILL_EXTRACT ' || { echo "FAIL: no SKILL_EXTRACT prefix"; exit 1; }

BODY=$(echo "$LINE" | sed 's/^SKILL_EXTRACT //')

check() { # $1 = python expr, $2 = label
    printf '%s' "$BODY" | python3 -c "import json,sys; d=json.load(sys.stdin); sys.exit(0 if ($1) else 1)" \
        && echo "ok - $2" || { echo "FAIL - $2"; exit 1; }
}

check 'd["schema"] == "at.skill/v1"'                                   "schema pinned"
check 'd["wire_roundtrip"] == "PASS"'                                  "offer bytes round-trip"
check 'd["wire_bytes"] > 0'                                            "wire payload nonempty"
check 'd["offer"]["type"] == "OFFER"'                                  "transfer protocol OFFER"
check 'len(d["learning_path"]) >= 2 and d["learning_path"][0] == "git"' "learning path walks deps"
check 'len(d["exercises"]) == 4'                                       "forge has 4 stages"
check 'all(e["stage"] == i + 1 for i, e in enumerate(d["exercises"]))' "exercise stages ordered HEATED..TEMPERED"
check 'd["metrics"]["commits"] > 0'                                    "real commits measured"
check 'd["metrics"]["realms"] > 0'                                     "ontology realms measured"
check 'any(t.startswith("realm:") for t in d["tags"])'                 "realms frozen into tags"
check '0 <= d["mastery"] <= 5'                                         "mastery in range"
echo "round-4 probe: all assertions passed"
