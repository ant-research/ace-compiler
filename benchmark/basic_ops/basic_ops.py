#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Basic operator FHE compile-and-run benchmark.

Covers Add, Mul, MVM (Gemm) and Conv2d. The script is backend-agnostic:
simply pass ``--library`` and ``--device`` to run on CPU/GPU/FPGA targets.
Default is phantom/cuda (GPU).

Usage::

    # GPU (phantom)
    python3 benchmark/basic_ops/bench_basic_ops.py

    # CPU (ant)
    python3 benchmark/basic_ops/bench_basic_ops.py --library ant --device cpu

    # Write machine-readable JSON report
    python3 benchmark/basic_ops/bench_basic_ops.py --output report.json

The script clears the per-operator compile cache before each measurement, so the
reported "compile_time_s" is the cold-cache end-to-end cost from PyTorch
tracing, AIR export, ``fhe_cmplr`` ``.cu`` generation, and ``nvcc`` ``.so``
linking (or the equivalent for the selected backend).
"""

from __future__ import annotations

import argparse
import json
import os
import shutil
import sys
import time
from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Any

import torch

# Allow importing from repo root when run as ``python3 benchmark/basic_ops/...``
repo_root = Path(__file__).resolve().parents[2]
if str(repo_root) not in sys.path:
    sys.path.insert(0, str(repo_root))

from ace import fhe
from ace.sample.ops.specs import ADD_OP, CONV2D_OP, LINEAR_OP, MUL_OP


DEFAULT_CACHE_DIR = "/tmp/ace-compile-cache"


# ============================================================================
# Benchmark spec
# ============================================================================


@dataclass(frozen=True)
class OperatorResult:
    """One row of the benchmark report."""

    name: str
    input_shape: list[int] | None
    library: str
    device: str
    compile_time_s: float
    execution_time_ms: float
    result_ok: bool
    backend: str


# ============================================================================
# Core timing helpers
# ============================================================================


def clear_compile_cache(name_fragment: str, cache_dir: str = DEFAULT_CACHE_DIR) -> None:
    """Remove cache directories whose name contains ``name_fragment``."""
    if not os.path.isdir(cache_dir):
        return
    for entry in os.listdir(cache_dir):
        if name_fragment in entry:
            shutil.rmtree(os.path.join(cache_dir, entry), ignore_errors=True)


def input_shape_of(spec) -> list[int] | None:
    """Return the first example input's shape as a list, if available."""
    if not spec.example_inputs:
        return None
    first = spec.example_inputs[0]
    if hasattr(first, "shape"):
        return list(first.shape)
    return None


def compile_and_run(
    spec,
    library: str = "phantom",
    device: str = "cuda",
    cache_dir: str = DEFAULT_CACHE_DIR,
) -> OperatorResult:
    """Compile and run a single operator spec, returning timing data.

    Args:
        spec: ``ModelSpec`` to benchmark.
        library: Backend library (``phantom``, ``ace``, ``ant``).
        device: Target device (``cuda``, ``cpu``).
        cache_dir: ACE compile cache directory to clear beforehand.

    Returns:
        ``OperatorResult`` with compile/run timing and success flag.
    """
    clear_compile_cache(spec.name, cache_dir)

    model = spec.create_model()
    model.eval()

    t0 = time.perf_counter()
    compiled = fhe.compile(
        frontend="torch",
        library=library,
        device=device,
        encrypt_inputs=spec.encrypt_inputs,
    )(model)
    program = compiled.fhe_compile(spec.example_inputs)
    t1 = time.perf_counter()

    runner = fhe.runtime.FHERuntime(program)
    runner.init()
    t2 = time.perf_counter()

    try:
        result = runner.run(*spec.example_inputs)
        result_ok = result is not None
    except Exception as exc:  # noqa: BLE001
        print(f"  {spec.name}: execution failed: {exc}", file=sys.stderr)
        result_ok = False
    t3 = time.perf_counter()

    return OperatorResult(
        name=spec.name,
        input_shape=input_shape_of(spec),
        library=library,
        device=device,
        compile_time_s=t1 - t0,
        execution_time_ms=(t3 - t2) * 1000.0,
        result_ok=result_ok,
        backend=library,
    )


# ============================================================================
# CLI
# ============================================================================


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description="Benchmark basic FHE operators (Add/Mul/MVM/Conv) on a chosen backend."
    )
    parser.add_argument(
        "--library",
        default="phantom",
        help="FHE backend library to use (default: phantom).",
    )
    parser.add_argument(
        "--device",
        default="cuda",
        help="Target device (default: cuda).",
    )
    parser.add_argument(
        "--cache-dir",
        default=DEFAULT_CACHE_DIR,
        help=f"ACE compile cache directory to clear (default: {DEFAULT_CACHE_DIR}).",
    )
    parser.add_argument(
        "--output",
        default=None,
        help="Optional JSON file to write results to.",
    )
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv)

    specs = [ADD_OP, MUL_OP, LINEAR_OP, CONV2D_OP]
    shape_map = {
        "add_op": "[1, 4] + [1, 4]",
        "mul_op": "[1, 4] × [1, 4]",
        "linear_op": "[1, 4]",
        "conv2d_op": "[1, 3, 16, 16]",
    }

    print(f"Backend: {args.library}/{args.device}")
    print("-" * 80)
    print(f"{'Operator':<15} {'Input':<20} {'Compile(s)':>12} {'Exec(ms)':>12} {'Result':>8}")
    print("-" * 80)

    results: list[OperatorResult] = []
    for spec in specs:
        try:
            result = compile_and_run(
                spec,
                library=args.library,
                device=args.device,
                cache_dir=args.cache_dir,
            )
        except Exception as exc:  # noqa: BLE001
            print(f"{spec.name:<15} {shape_map[spec.name]:<20} {'FAILED':>12} {'—':>12} {'FAIL':>8}")
            print(f"  error: {exc}", file=sys.stderr)
            result = OperatorResult(
                name=spec.name,
                input_shape=input_shape_of(spec),
                library=args.library,
                device=args.device,
                compile_time_s=0.0,
                execution_time_ms=0.0,
                result_ok=False,
                backend=args.library,
            )
        results.append(result)
        status = "PASS" if result.result_ok else "FAIL"
        print(
            f"{result.name:<15} {shape_map[spec.name]:<20} "
            f"{result.compile_time_s:>12.2f} {result.execution_time_ms:>12.2f} {status:>8}"
        )

    print("-" * 80)

    if args.output:
        payload: dict[str, Any] = {
            "library": args.library,
            "device": args.device,
            "cache_dir": args.cache_dir,
            "results": [asdict(r) for r in results],
        }
        with open(args.output, "w") as f:
            json.dump(payload, f, indent=2, sort_keys=True)
        print(f"Wrote results to {args.output}")

    return 0 if all(r.result_ok for r in results) else 1


if __name__ == "__main__":
    raise SystemExit(main())
