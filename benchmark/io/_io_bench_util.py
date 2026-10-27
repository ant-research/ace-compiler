# =============================================================================
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# =============================================================================

"""Shared utilities for IO performance benchmarks.

Provides result aggregation, report generation, and helper functions.
"""

import os
import sys
from dataclasses import dataclass, field
from typing import Optional


GiB = 1024**3

DISK_PATHS = ["/mnt/nvme0", "/mnt/nvme1", "/mnt/nvme2", "/mnt/nvme3"]


@dataclass
class BenchResult:
    """Single benchmark result row."""

    data_path: str = ""  # Kept for backward compat; new code uses 'policy'
    policy: str = ""
    threads: int = 0
    disks: int = 1
    file_mode: str = "single"  # "single" or "multi"
    data_size_gib: float = 0.0
    write_ms: float = 0.0
    write_gib_s: float = 0.0
    read_ms: float = 0.0
    read_gib_s: float = 0.0
    transport_name: str = ""
    verified: bool = False
    gds_available: bool = False


def policy_to_expected_transport(policy: str, gds_available: bool) -> str:
    """Map IoPolicy to expected transport name prefix.

    Supports both new names (auto, gds, staged, mapped) and
    deprecated names (checkpoint, galois_key_load, high_throughput, etc.)

    Returns a string that transport_name should start with:
    - "staged" → "staged" (always staged)
    - "gds" → "gds" (if cuFile available, else error)
    - "auto" / deprecated names → "gds" or "staged" (depends on GDS)
    """
    if policy == "staged":
        return "staged"
    if policy == "mapped":
        return "mapped"
    if policy == "gds":
        return "gds" if gds_available else "gds(fallback)"
    # auto + all deprecated names (checkpoint, galois_key_load, etc.)
    # Deprecated "checkpoint" already mapped to staged above in new code,
    # but handle old callers:
    if policy == "checkpoint":
        return "staged"
    if gds_available:
        return "gds"
    return "staged"


def datapath_to_expected_transport(data_path: str) -> str:
    """Map DataPath string to expected transport name.

    DEPRECATED: Use policy_to_expected_transport instead.
    Kept for backward compat with old benchmark code.
    """
    mapping = {
        "host_staged": "staged",
        "staged": "staged",
        "host_mapped": "mapped",
        "mapped": "mapped",
        "gds": "gds",
        "auto": "",  # depends on GDS availability
    }
    return mapping.get(data_path, "")


def collect_results(raw_results: list[dict]) -> list[BenchResult]:
    """Convert raw dict results to BenchResult list."""
    out = []
    for r in raw_results:
        out.append(
            BenchResult(
                data_path=r.get("data_path", "") or r.get("policy", ""),
                policy=r.get("policy", "") or r.get("data_path", ""),
                threads=r.get("threads", 0),
                disks=r.get("disks", 1),
                file_mode=r.get("file_mode", "single"),
                data_size_gib=r.get("data_size_gib", 0.0),
                write_ms=r.get("write_ms", 0.0),
                write_gib_s=r.get("write_gib_s", 0.0),
                read_ms=r.get("read_ms", 0.0),
                read_gib_s=r.get("read_gib_s", 0.0),
                transport_name=r.get("transport_name", ""),
                verified=r.get("verified", False),
                gds_available=r.get("gds_available", False),
            )
        )
    return out


def write_report(results: list[dict], output_path: str):
    """Write a markdown comparison report from benchmark results."""
    if not results:
        return

    grouped = {}
    for r in results:
        key = (r.get("data_path", "") or r.get("policy", ""), r.get("file_mode", "single"))
        grouped.setdefault(key, []).append(r)

    lines = ["# IO Performance Benchmark Report\n"]
    lines.append(f"Total configurations tested: {len(results)}\n")

    for (config_key, file_mode), rows in sorted(grouped.items()):
        lines.append(f"\n## {config_key} / {file_mode}\n")
        lines.append("| Threads | Disks | Size(GiB) | Write(GiB/s) | Read(GiB/s) | Transport | Verified |")
        lines.append("|---------|-------|-----------|-------------|------------|-----------|----------|")
        for r in sorted(rows, key=lambda x: (x.get("threads", 0), x.get("disks", 1))):
            lines.append(
                f"| {r.get('threads', 0)} "
                f"| {r.get('disks', 1)} "
                f"| {r.get('data_size_gib', 0):.0f} "
                f"| {r.get('write_gib_s', 0):.2f} "
                f"| {r.get('read_gib_s', 0):.2f} "
                f"| {r.get('transport_name', '')} "
                f"| {'✓' if r.get('verified') else '✗'} |"
            )

    with open(output_path, "w") as f:
        f.write("\n".join(lines) + "\n")
