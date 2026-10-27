#!/usr/bin/env python3
# summarize.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# Parse a pytest --junitxml report and append one JSONL line summarizing the
# nightly benchmark run to the history file. Used by scripts/perf-guard/
# orchestrator.sh.
#
# The comparison against committed baselines (test_latency/*.csv etc.) is done
# inside pytest by pytest-regressions; this script only aggregates the verdict.
#
# Usage:
#   python3 scripts/perf-guard/summarize.py \
#       --junit /mnt/nvme0/perf_history/resnet/runs/<run_id>/junit.xml \
#       --history /mnt/nvme0/perf_history/resnet/history.jsonl \
#       --run-id <run_id> --git <sha> [--gpu-tag <tag>]
#
# gpu_tag defaults to benchmark/_fixtures.gpu_tag() if importable.
"""Aggregate a pytest junit report into a single JSONL history line."""

from __future__ import annotations

import argparse
import json
import sys
import xml.etree.ElementTree as ET
from pathlib import Path
from typing import Any


def _gpu_tag() -> str:
    """Reuse benchmark/_fixtures.gpu_tag() if importable, else 'unknown'."""
    try:
        sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "benchmark"))
        from _fixtures import gpu_tag  # type: ignore[import-not-found]

        return gpu_tag()
    except Exception:
        return "unknown"


def _text(node: ET.Element | None) -> str:
    """Return stripped text content of a node, or '' if None/empty."""
    if node is None or node.text is None:
        return ""
    return node.text.strip()


def _truncate(msg: str, limit: int = 600) -> str:
    """Cap a failure message length so one JSONL line stays scannable."""
    if len(msg) <= limit:
        return msg
    return msg[:limit] + " …[truncated]"


def parse_junit(junit_path: Path) -> dict[str, Any]:
    """Parse junit.xml into aggregate counts + per-failure detail."""
    tree = ET.parse(junit_path)
    root = tree.getroot()

    tests = failures = errors = skipped = 0
    failed: list[dict[str, str]] = []

    # junit-xml nests <testcase> under <testsuite>, sometimes under <testsuites>.
    for tc in root.iter("testcase"):
        tests += 1
        cls = tc.get("classname", "")
        name = tc.get("name", "")
        file = tc.get("file", "")
        # Prefer the recorded file attr; derive from classname as fallback.
        if not file:
            file = cls.replace(".", "/") + ".py"

        fail = tc.find("failure")
        err = tc.find("error")
        skip = tc.find("skipped")

        if skip is not None:
            skipped += 1
            continue
        if err is not None:
            errors += 1
            msg = _text(err) or err.get("message") or "error"
            failed.append({"name": name, "file": file, "type": "error", "msg": _truncate(msg)})
        elif fail is not None:
            failures += 1
            msg = _text(fail) or fail.get("message") or "failed"
            failed.append({"name": name, "file": file, "type": "failure", "msg": _truncate(msg)})

    passed = tests - failures - errors - skipped
    return {
        "tests": tests,
        "failures": failures,
        "errors": errors,
        "skipped": skipped,
        "passed": passed,
        "failed": failed,
    }


def main() -> int:
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument("--junit", required=True, type=Path, help="Path to junit.xml")
    p.add_argument("--history", required=True, type=Path, help="JSONL history file (appended)")
    p.add_argument("--run-id", default="unknown")
    p.add_argument("--git", default="unknown")
    p.add_argument("--gpu-tag", default=None, help="Override gpu_tag (default: auto-detect)")
    p.add_argument("--ts", default=None, help="ISO timestamp; default: from run-id YYYYMMDD_HHMM")
    args = p.parse_args()

    if not args.junit.is_file():
        print(f"summarize: junit not found: {args.junit}", file=sys.stderr)
        return 2

    agg = parse_junit(args.junit)
    gpu_tag = args.gpu_tag if args.gpu_tag is not None else _gpu_tag()

    # Derive an ISO timestamp from run_id (YYYYMMDD_HHMM) when not given.
    ts = args.ts
    if ts is None and len(args.run_id) == 13 and args.run_id[8] == "_":
        ts = f"{args.run_id[0:4]}-{args.run_id[4:6]}-{args.run_id[6:8]}T{args.run_id[9:11]}:{args.run_id[11:13]}:00"

    record = {
        "run_id": args.run_id,
        "ts": ts,
        "git": args.git,
        "gpu_tag": gpu_tag,
        **agg,
        "pass": agg["failures"] == 0 and agg["errors"] == 0,
    }

    args.history.parent.mkdir(parents=True, exist_ok=True)
    with args.history.open("a", encoding="utf-8") as f:
        f.write(json.dumps(record, ensure_ascii=False) + "\n")

    # Human-readable echo for the orchestrator log.
    verdict = "PASS" if record["pass"] else "FAIL"
    print(
        f"[summarize] {verdict}  run={args.run_id} gpu={gpu_tag} git={args.git}  "
        f"tests={agg['tests']} pass={agg['passed']} fail={agg['failures']} "
        f"err={agg['errors']} skip={agg['skipped']}"
    )
    for item in record["failed"]:
        print(f"  - {item['file']}::{item['name']} [{item['type']}]: {item['msg']}")

    return 0 if record["pass"] else 1


if __name__ == "__main__":
    sys.exit(main())
