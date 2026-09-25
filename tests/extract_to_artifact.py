#!/usr/bin/env python3
"""extract_to_artifact.py — turn a probe_repo_extract SKILL_EXTRACT line into a
validated at.skill.json artifact.

Usage:
  ./probe_repo_extract <repo> <name> | python3 tests/extract_to_artifact.py \
      --repo <repo> --out at.skill.json [--gauge /path/to/gauge-lab]

Adds provenance + evidence blocks (unknown to the strict schema; gauge checks
the required core), optionally runs SuperInstance/gauge against the artifact,
and exits nonzero if the schema check fails. Round 4 of the Forge.
"""
import argparse, json, subprocess, sys, datetime

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--gauge", default=None,
                    help="path to a gauge checkout (used for PYTHONPATH); the schema "
                         "is discovered from THIS repo's .gauge.json + schemas/")
    ap.add_argument("--repo", default=".",
                    help="repo owning the artifact (schema discovery root; default: .)")
    ap.add_argument("--source", default=None,
                    help="repo the probe read (provenance commit; default: --repo)")
    ap.add_argument("--out", default="at.skill.json", help="artifact output path")
    ap.add_argument("--in", dest="infile", default=None,
                    help="read SKILL_EXTRACT line from file (default: stdin)")
    args = ap.parse_args()

    line = (open(args.infile).readline() if args.infile else sys.stdin.readline()).strip()
    if not line.startswith("SKILL_EXTRACT "):
        print("error: expected SKILL_EXTRACT line", file=sys.stderr); return 2
    data = json.loads(line[len("SKILL_EXTRACT "):])

    src_repo = args.source or args.repo
    commit = subprocess.run(
        ["git", "-C", src_repo, "rev-parse", "--short=12", "HEAD"],
        capture_output=True, text=True).stdout.strip() or "unknown"

    data["provenance"] = {
        "extracted_by": "probe_repo_extract (round 4)",
        "source_repo": src_repo,
        "commit": commit,
        "extracted_at": datetime.datetime.now(datetime.timezone.utc).isoformat(),
        "doctrine": "FINDINGS F-03/F-05: repo as frozen attention field",
    }
    data["evidence"] = {
        "receipt": f"git:{commit}",
        "metrics": data.get("metrics", {}),
        "wire_roundtrip": data.get("wire_roundtrip"),
        "wire_bytes": data.get("wire_bytes"),
    }

    with open(args.out, "w") as f:
        json.dump(data, f, indent=2)
    print(f"wrote {args.out} ({len(data)} top-level keys), commit {commit}")

    if args.gauge:
        import os
        env = dict(os.environ, PYTHONPATH=args.gauge + os.pathsep
                   + os.environ.get("PYTHONPATH", ""))
        r = subprocess.run(
            [sys.executable, "-m", "gauge.cli", "check", args.out,
             "--schema", "at.skill", "--repo", args.repo],
            capture_output=True, text=True, env=env)
        print(r.stdout.strip() or r.stderr.strip())
        if r.returncode != 0:
            print("GAUGE: artifact FAILED schema check", file=sys.stderr)
            return 1
        print("GAUGE: artifact OK against at.skill")
    return 0

if __name__ == "__main__":
    sys.exit(main())
