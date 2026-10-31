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
from ace.fhe.config.spec import ModelSpec
from ace.sample.ops.arithmetic import AddOp, MulOp
from ace.sample.ops.conv import Conv2dOp
from ace.sample.ops.linear import LinearOp


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


def input_shape_str(shape: list[int] | None) -> str:
    """Format a list[int] shape as a display string, e.g. '[1, 64, 32, 16]'."""
    if shape is None:
        return "—"
    return "[" + ", ".join(str(s) for s in shape) + "]"


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


def _make_add_spec(name: str, shape: tuple) -> "ModelSpec":
    """Create a ModelSpec for element-wise add at a given shape."""
    return ModelSpec(
        name=name,
        model_class=AddOp,
        example_inputs=(torch.randn(*shape, dtype=torch.float32), torch.randn(*shape, dtype=torch.float32)),
        encrypt_inputs=["x", "y"],
        expected_ops=["Add"],
    )


def _make_mul_spec(name: str, shape: tuple) -> "ModelSpec":
    """Create a ModelSpec for element-wise mul at a given shape."""
    return ModelSpec(
        name=name,
        model_class=MulOp,
        example_inputs=(torch.randn(*shape, dtype=torch.float32), torch.randn(*shape, dtype=torch.float32)),
        encrypt_inputs=["x", "y"],
        expected_ops=["Mul"],
    )


def _make_linear_spec(name: str, input_size: int, num_classes: int, input_shape: tuple) -> "ModelSpec":
    """Create a ModelSpec for Linear (GEMM) at a given (input_size, num_classes)."""
    return ModelSpec(
        name=name,
        model_class=LinearOp,
        example_inputs=(torch.randn(*input_shape, dtype=torch.float32),),
        encrypt_inputs=["x"],
        model_init_args=(input_size, num_classes),
        expected_ops=["Gemm"],
    )


def _make_conv2d_spec(name: str, in_ch: int, out_ch: int, kernel_size: int, input_shape: tuple) -> "ModelSpec":
    """Create a ModelSpec for Conv2d at a given (in_ch, out_ch, kernel_size)."""
    return ModelSpec(
        name=name,
        model_class=Conv2dOp,
        example_inputs=(torch.randn(*input_shape, dtype=torch.float32),),
        encrypt_inputs=["x"],
        model_init_args=(in_ch, out_ch, kernel_size),
        expected_ops=["Conv"],
    )


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv)

    # Each entry: (display_name, ModelSpec, display_shape_string)
    specs: list[tuple[str, ModelSpec, str]] = [
        # ── Add ──────────────────────────────────────────────────────────
        ("add_op_32",   _make_add_spec("add_op_32",   (1, 32)),  "[1, 32] + [1, 32]"),
        ("add_op_64",   _make_add_spec("add_op_64",   (1, 64)),  "[1, 64] + [1, 64]"),
        ("add_op_128",  _make_add_spec("add_op_128",  (1, 128)), "[1, 128] + [1, 128]"),
        ("add_op_256",  _make_add_spec("add_op_256",  (1, 256)), "[1, 256] + [1, 256]"),
        ("add_op_512",  _make_add_spec("add_op_512",  (1, 512)), "[1, 512] + [1, 512]"),

        # ── Mul ──────────────────────────────────────────────────────────
        ("mul_op_32",   _make_mul_spec("mul_op_32",   (1, 32)),  "[1, 32] × [1, 32]"),
        ("mul_op_64",   _make_mul_spec("mul_op_64",   (1, 64)),  "[1, 64] × [1, 64]"),
        ("mul_op_128",  _make_mul_spec("mul_op_128",  (1, 128)), "[1, 128] × [1, 128]"),
        ("mul_op_256",  _make_mul_spec("mul_op_256",  (1, 256)), "[1, 256] × [1, 256]"),
        ("mul_op_512",  _make_mul_spec("mul_op_512",  (1, 512)), "[1, 512] × [1, 512]"),

        # ── Linear (GEMM) ────────────────────────────────────────────────
        ("linear_4x4",   _make_linear_spec("linear_4x4",   4,   4,   (1, 4)),   "[1, 4]"),
        ("linear_16x8",  _make_linear_spec("linear_16x8",  16,  8,   (1, 16)),  "[1, 16]"),
        ("linear_64x32", _make_linear_spec("linear_64x32", 64,  32,  (1, 64)),  "[1, 64]"),
        ("linear_128x64", _make_linear_spec("linear_128x64", 128, 64, (1, 128)), "[1, 128]"),

        # ── Conv2d ───────────────────────────────────────────────────────
        ("conv2d_3x16",   _make_conv2d_spec("conv2d_3x16",   3, 16, 3, (1, 3, 16, 16)),  "[1, 3, 16, 16]"),
        ("conv2d_16x32",  _make_conv2d_spec("conv2d_16x32",  16, 32, 3, (1, 16, 8, 8)),  "[1, 16, 8, 8]"),
        ("conv2d_32x64",  _make_conv2d_spec("conv2d_32x64",  32, 64, 3, (1, 32, 4, 4)),  "[1, 32, 4, 4]"),
    ]

    print(f"Backend: {args.library}/{args.device}")
    print(f"Configs : {len(specs)} (shape variants per operator)")
    print("-" * 100)
    print(f"{'Operator':<20} {'Input':<22} {'Compile(s)':>12} {'Exec(ms)':>12} {'Result':>8}")
    print("-" * 100)

    results: list[OperatorResult] = []
    for display_name, spec, display_shape in specs:
        try:
            result = compile_and_run(
                spec,
                library=args.library,
                device=args.device,
                cache_dir=args.cache_dir,
            )
        except Exception as exc:  # noqa: BLE001
            print(f"{display_name:<20} {display_shape:<22} {'FAILED':>12} {'—':>12} {'FAIL':>8}")
            print(f"  error: {exc}", file=sys.stderr)
            result = OperatorResult(
                name=display_name,
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
            f"{result.name:<20} "
            f"{(input_shape_str(result.input_shape)) if result.input_shape else display_shape:<22} "
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