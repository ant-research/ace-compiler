#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Profiling Example 1: Quick FHE Profiling with fhe.profiler API

A minimal example showing all profiling usage patterns on a small linear model.
Compiles in seconds (unlike ResNet-20), making it ideal for quick iteration.

Usage patterns demonstrated:
  1. Context manager:  with fhe.profiler(...) as prof: ...
  2. Convenience method: program.profile(images, labels)
  3. Manual trace export with memory counter tracks

Usage:
    python 01_quick_profile.py
    python 01_quick_profile.py --library phantom --device cuda --trace-dir ./trace
"""

import argparse
import torch
import torch.nn as nn

from ace import fhe


# ---------------------------------------------------------------------------
# Model: tiny linear model, compiles in seconds
# ---------------------------------------------------------------------------
@fhe.compile(frontend="torch", library="antlib", device="cpu")
class LinearModel(nn.Module):
    """Simple linear model: y = Wx + b."""

    def __init__(self, input_size=4, output_size=4):
        super().__init__()
        self.linear = nn.Linear(input_size, output_size)

    def forward(self, x):
        return self.linear(x)


def main():
    parser = argparse.ArgumentParser(
        description="Quick FHE profiling with fhe.profiler API")
    parser.add_argument("--library", default="antlib", choices=["antlib", "phantom"],
                        help="FHE backend library (default: antlib)")
    parser.add_argument("--device", default="cpu", choices=["cpu", "cuda"],
                        help="Device (default: cpu)")
    parser.add_argument("--trace-dir", default=None,
                        help="Export Chrome trace to this directory")
    args = parser.parse_args()

    if args.library == "phantom" and not fhe.gpu_available():
        print("GPU not available, falling back to antlib/cpu")
        args.library = "antlib"
        args.device = "cpu"

    # Compile
    print(f"Compiling LinearModel ({args.library}/{args.device})...")
    x = torch.randn(1, 4)
    program = LinearModel.compile([x])
    print("Compilation done.\n")

    # =======================================================================
    # Pattern 1: Context manager (recommended for full control)
    #
    # Use when you need to profile multiple operations, access raw
    # torch.profiler, or customize trace export.
    # =======================================================================
    print("=" * 60)
    print("Pattern 1: Context manager (fhe.profiler)")
    print("=" * 60)

    with fhe.profiler(device=args.device, trace_dir=args.trace_dir) as prof:
        result = program(x)

    print(f"\nFHE result: {result.tolist()}")
    print(prof.summary())

    # Access the underlying torch.profiler for advanced queries
    if prof.torch_profiler is not None:
        print("\n--- Top 5 events by CPU time ---")
        print(prof.torch_profiler.key_averages().table(
            sort_by="cpu_time_total", row_limit=5))

    # =======================================================================
    # Pattern 2: CompiledProgram.profile() (simplest one-liner)
    #
    # Use when you just want a quick profile of a single inference.
    # =======================================================================
    print("\n" + "=" * 60)
    print("Pattern 2: Convenience method (program.profile)")
    print("=" * 60)

    profile_result = program.profile([x], device=args.device)
    print(f"\n{profile_result}")

    # =======================================================================
    # Pattern 3: Manual trace export with memory counters
    #
    # export_trace() automatically adds GPU memory counter tracks
    # (from fhe::mem::* events) so Perfetto renders them as charts.
    # =======================================================================
    if args.trace_dir:
        print("\n" + "=" * 60)
        print("Pattern 3: Manual trace export")
        print("=" * 60)

        with fhe.profiler(device=args.device) as prof:
            _ = program(x)

        print(prof.summary())

        import os
        os.makedirs(args.trace_dir, exist_ok=True)
        trace_path = os.path.join(args.trace_dir, "linear_fhe_trace.json")
        prof.export_trace(trace_path)
        print(f"\nChrome trace exported to: {trace_path}")
        print("Open in chrome://tracing or https://ui.perfetto.dev")
        print("Search 'fhe::' to see FHE phases, 'GPU Memory' for memory chart")


if __name__ == "__main__":
    main()