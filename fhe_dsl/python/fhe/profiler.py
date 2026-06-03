#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
FHE Profiling API.

Usage::

    # Context manager (recommended)
    with fhe.profiler(device="cuda", trace_dir="./trace") as prof:
        result = program.run_dataset(images, labels)
    print(prof.summary())

    # Convenience method on CompiledProgram
    profile_result = program.profile(images, labels, device="cuda")
    print(profile_result)
"""

import json
import os
import re
from dataclasses import dataclass, field
from typing import List, Optional

import torch
from torch.profiler import ProfilerActivity


# Regex for fhe::mem:: event names: fhe::mem::<phase>::<used>MB_gpu_used_<free>MB_gpu_free
_MEM_SNAPSHOT_PATTERN = re.compile(
    r"fhe::mem::(\w+)::(\d+)MB_gpu_used_(\d+)MB_gpu_free"
)


@dataclass
class FHEEvent:
    """A single FHE profiling event from torch.profiler."""

    name: str
    cpu_time_total_ms: float
    cpu_time_avg_ms: float
    count: int


@dataclass
class MemSnapshot:
    """A GPU memory snapshot from an fhe::mem::* profiling event."""

    phase: str
    gpu_used_mb: int
    gpu_free_mb: int


@dataclass
class ProfileResult:
    """Structured result from an FHE profiling session."""

    fhe_events: List[FHEEvent] = field(default_factory=list)
    memory_snapshots: List[MemSnapshot] = field(default_factory=list)
    dataset_result: object = None
    trace_path: Optional[str] = None

    def summary(self) -> str:
        """Return a formatted summary of FHE events and memory snapshots."""
        lines = []

        # FHE events table
        if self.fhe_events:
            lines.append("--- FHE Events (sorted by CPU time) ---")
            lines.append(
                f"{'Event':<55} {'CPU total (ms)':>15} {'Calls':>8} {'CPU avg (ms)':>15}"
            )
            lines.append("-" * 95)
            for e in self.fhe_events:
                lines.append(
                    f"{e.name:<55} {e.cpu_time_total_ms:>15.1f} "
                    f"{e.count:>8} {e.cpu_time_avg_ms:>15.1f}"
                )

        # Memory snapshots table
        if self.memory_snapshots:
            lines.append("")
            lines.append("--- Memory Snapshots ---")
            lines.append(f"{'Phase':<40} {'GPU Used (MB)':>15} {'GPU Free (MB)':>15}")
            lines.append("-" * 72)
            for m in self.memory_snapshots:
                lines.append(
                    f"{m.phase:<40} {m.gpu_used_mb:>15} {m.gpu_free_mb:>15}"
                )
        elif not self.fhe_events:
            lines.append("(No profiling data collected.)")

        return "\n".join(lines)

    def __str__(self) -> str:
        return self.summary()


class FHEProfiler:
    """Context manager for profiling FHE inference.

    Wraps torch.profiler with FHE-specific event filtering, memory snapshot
    parsing, and Chrome Trace export with memory counter tracks.

    Args:
        device: "cpu" or "cuda". Adds ProfilerActivity.CUDA when "cuda".
        trace_dir: If set, auto-exports Chrome Trace to this directory on exit.
        profile_memory: Enable PyTorch memory tracking.
        record_shapes: Record tensor shapes for each op.
        with_stack: Record call stacks for each op.

    Example::

        with fhe.profiler(device="cuda", trace_dir="./trace") as prof:
            result = program.run_dataset(images, labels)
        print(prof.summary())
    """

    def __init__(
        self,
        device: str = "cpu",
        trace_dir: Optional[str] = None,
        profile_memory: bool = True,
        record_shapes: bool = False,
        with_stack: bool = False,
    ):
        self._device = device
        self._trace_dir = trace_dir
        self._profile_memory = profile_memory
        self._record_shapes = record_shapes
        self._with_stack = with_stack
        self._prof: Optional[torch.profiler.profile] = None
        self._result: Optional[ProfileResult] = None

    def __enter__(self):
        activities = [ProfilerActivity.CPU]
        if self._device == "cuda" and torch.cuda.is_available():
            activities.append(ProfilerActivity.CUDA)

        self._prof = torch.profiler.profile(
            activities=activities,
            with_stack=self._with_stack,
            profile_memory=self._profile_memory,
            record_shapes=self._record_shapes,
        )
        self._prof.__enter__()
        return self

    def __exit__(self, *args):
        self._prof.__exit__(*args)
        self._result = self._build_result()

        if self._trace_dir is not None:
            self.export_trace_dir(self._trace_dir)

    def _build_result(self) -> ProfileResult:
        """Parse torch.profiler output into structured FHE events and memory snapshots."""
        result = ProfileResult()

        if self._prof is None:
            return result

        key_avg = self._prof.key_averages()

        # Collect FHE events (excluding fhe::mem::)
        for entry in key_avg:
            key = entry.key
            if not key.startswith("fhe::"):
                continue
            if key.startswith("fhe::mem::"):
                # Parse memory snapshots
                m = _MEM_SNAPSHOT_PATTERN.match(key)
                if m:
                    result.memory_snapshots.append(MemSnapshot(
                        phase=m.group(1),
                        gpu_used_mb=int(m.group(2)),
                        gpu_free_mb=int(m.group(3)),
                    ))
                continue

            count = entry.count
            result.fhe_events.append(FHEEvent(
                name=key,
                cpu_time_total_ms=entry.cpu_time_total / 1000,
                cpu_time_avg_ms=(entry.cpu_time_total / count / 1000) if count else 0,
                count=count,
            ))

        # Sort by CPU time descending
        result.fhe_events.sort(key=lambda e: e.cpu_time_total_ms, reverse=True)

        # Sort memory snapshots by phase order
        result.memory_snapshots.sort(key=lambda m: m.phase)

        return result

    @property
    def result(self) -> Optional[ProfileResult]:
        """Return the profiling result (available after exiting the context)."""
        return self._result

    @property
    def torch_profiler(self) -> Optional[torch.profiler.profile]:
        """Return the underlying torch.profiler for advanced use."""
        return self._prof

    def summary(self) -> str:
        """Return a formatted summary string."""
        if self._result is None:
            return "(Profiler has not been run yet. Use 'with fhe.profiler(...) as prof:')"
        return self._result.summary()

    def export_trace(self, path: str, add_memory_counters: bool = True):
        """Export Chrome Trace JSON file.

        Args:
            path: Output file path.
            add_memory_counters: If True, post-process the trace to add
                Perfetto-compatible memory counter tracks from fhe::mem:: events.
        """
        if self._prof is None:
            raise RuntimeError("No profiling data to export")

        self._prof.export_chrome_trace(path)

        if add_memory_counters and self._result and self._result.memory_snapshots:
            _add_memory_counters(path)

    def export_trace_dir(self, trace_dir: str, name: str = "fhe_trace.json"):
        """Export Chrome Trace to a directory.

        Args:
            trace_dir: Output directory (created if not exists).
            name: Trace file name within the directory.
        """
        os.makedirs(trace_dir, exist_ok=True)
        path = os.path.join(trace_dir, name)
        self.export_trace(path)
        return path


def _add_memory_counters(trace_path: str):
    """Post-process a Chrome Trace JSON to add GPU memory counter tracks.

    Parses fhe::mem::* events and adds counter events (ph="C") that
    Perfetto renders as memory charts.
    """
    with open(trace_path, "r") as f:
        trace = json.load(f)

    if isinstance(trace, dict):
        events = trace.get("traceEvents", [])
    else:
        events = trace

    counter_events = []
    for event in events:
        if not isinstance(event, dict):
            continue
        name = event.get("name", "")
        m = _MEM_SNAPSHOT_PATTERN.match(name)
        if not m:
            continue

        gpu_used_mb = int(m.group(2))
        gpu_free_mb = int(m.group(3))
        ts = event.get("ts", 0)
        pid = event.get("pid", 0)

        counter_events.append({
            "name": "GPU Memory (MB)",
            "ph": "C",
            "ts": ts,
            "pid": pid,
            "tid": 0,
            "cat": "fhe_mem",
            "args": {
                "gpu_used": gpu_used_mb,
                "gpu_free": gpu_free_mb,
            },
        })

    if counter_events:
        events.extend(counter_events)
        with open(trace_path, "w") as f:
            json.dump(trace, f)


def profiler(
    device: str = "cpu",
    trace_dir: Optional[str] = None,
    profile_memory: bool = True,
    record_shapes: bool = False,
    with_stack: bool = False,
) -> FHEProfiler:
    """Create an FHE profiling context manager.

    Args:
        device: "cpu" or "cuda". Adds ProfilerActivity.CUDA when "cuda".
        trace_dir: If set, auto-exports Chrome Trace on exit.
        profile_memory: Enable PyTorch memory tracking.
        record_shapes: Record tensor shapes for each op.
        with_stack: Record call stacks for each op.

    Returns:
        FHEProfiler instance to use as a context manager.

    Example::

        with fhe.profiler(device="cuda", trace_dir="./trace") as prof:
            result = program.run_dataset(images, labels)
        print(prof.summary())
    """
    return FHEProfiler(
        device=device,
        trace_dir=trace_dir,
        profile_memory=profile_memory,
        record_shapes=record_shapes,
        with_stack=with_stack,
    )