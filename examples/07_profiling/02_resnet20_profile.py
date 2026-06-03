#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Profiling Example 2: ResNet-20 FHE Inference with fhe.profiler

Demonstrates how to use the FHE profiling API to trace inference.
FHE operations are instrumented with RECORD_USER_SCOPE/NVTX in C++,
so they appear automatically in the profiler trace alongside regular PyTorch ops.

Trace hierarchy (nested):
    fhe::inference               (Python FHERuntime)
      fhe::run_batch_sequential  (C++ BATCH_RUNNER)
        fhe::run                 (C++ KERNEL_RUNNER, per image)
          fhe::prepare_input
          fhe::execute
          fhe::get_output

Usage:
    python 02_resnet20_profile.py
    python 02_resnet20_profile.py --library phantom --device cuda
    python 02_resnet20_profile.py --num-images 5 --trace-dir ./trace
"""

import argparse
import torch

from ace import fhe
from ace.model.spec_resnet import RESNET20_CIFAR10
from ace.model.dataset import load_cifar10_images, CIFAR10_CLASSES


def compile_resnet20(library="antlib", device="cpu"):
    """Compile ResNet-20 with the given backend."""
    spec = RESNET20_CIFAR10
    model = spec.create_model()

    example_inputs = tuple(
        torch.randn(inp.shape, dtype=inp.dtype)
        for inp in spec.example_inputs
    )

    compile_options = dict(spec.compile_options) if spec.compile_options else {}
    compile_options.setdefault("p2c", {})["lib"] = library

    compiled = fhe.compile(
        frontend="torch",
        library=library,
        device=device,
        encrypt_inputs=spec.encrypt_inputs,
        **compile_options,
    )(model)

    return compiled.fhe_compile(example_inputs)


def main():
    parser = argparse.ArgumentParser(description="ResNet-20 FHE profiling with fhe.profiler")
    parser.add_argument("--library", default="antlib", choices=["antlib", "phantom"],
                        help="FHE backend library (default: antlib)")
    parser.add_argument("--device", default="cpu", choices=["cpu", "cuda"],
                        help="Device (default: cpu)")
    parser.add_argument("--num-images", type=int, default=3,
                        help="Number of images to infer (default: 3)")
    parser.add_argument("--parallel", action="store_true",
                        help="Use OpenMP parallel batch execution")
    parser.add_argument("--num-threads", type=int, default=0,
                        help="OpenMP threads (0=auto, only with --parallel)")
    parser.add_argument("--trace-dir", default=None,
                        help="Export Chrome trace to this directory")
    args = parser.parse_args()

    if args.library == "phantom" and not fhe.gpu_available():
        print("GPU not available, falling back to antlib/cpu")
        args.library = "antlib"
        args.device = "cpu"

    # Compile
    print(f"Compiling ResNet-20 ({args.library}/{args.device})...")
    program = compile_resnet20(library=args.library, device=args.device)
    print("Compilation done.")

    # Load dataset
    images, labels = load_cifar10_images(args.num_images)

    # Profile using fhe.profiler context manager
    with fhe.profiler(device=args.device, trace_dir=args.trace_dir) as prof:
        result = program.run_dataset(
            images[:args.num_images],
            labels[:args.num_images],
            top_k=1,
            parallel=args.parallel,
            num_threads=args.num_threads,
            verbose=True,
        )

    # Print results
    print(f"\n{'=' * 60}")
    print(f"Dataset result: {result}")
    print(f"{'=' * 60}")
    print(prof.summary())

    # Print top-level summary (all events)
    if prof.torch_profiler is not None:
        print("\n--- All Events (top 15 by CPU time) ---")
        print(prof.torch_profiler.key_averages().table(
            sort_by="cpu_time_total", row_limit=15))

    if args.trace_dir:
        print(f"\nChrome trace exported to: {args.trace_dir}")
        print("Open in chrome://tracing or ui.perfetto.dev")


if __name__ == "__main__":
    main()