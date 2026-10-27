#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Profiling Example: ResNet Model FHE Inference with fhe.profiler

Supports all ResNet models (20, 32, 44, 56, 110) on CIFAR-10/100.

Usage:
    python 02_resnet_profile.py --model resnet20
    python 02_resnet_profile.py --model resnet56 --library phantom --device cuda
    python 02_resnet_profile.py --model resnet110 --num-images 5 --trace-dir ./trace
    python 02_resnet_profile.py --model all --parallel  # Profile all models
"""

import argparse
import sys
import time

import torch

from ace import fhe
from ace.model.dataset import CIFAR10_CLASSES, CIFAR100_CLASSES, load_cifar10_images, load_cifar100_images
from ace.model.spec_resnet import (
    RESNET20_CIFAR10,
    RESNET32_CIFAR10,
    RESNET32_CIFAR100,
    RESNET44_CIFAR10,
    RESNET56_CIFAR10,
    RESNET110_CIFAR10,
    RESNET_CIFAR10_SPECS,
    RESNET_CIFAR100_SPECS,
)


# Model name to spec mapping
MODEL_SPECS = {
    "resnet20": (RESNET20_CIFAR10, "cifar10"),
    "resnet32": (RESNET32_CIFAR10, "cifar10"),
    "resnet32_c100": (RESNET32_CIFAR100, "cifar100"),
    "resnet44": (RESNET44_CIFAR10, "cifar10"),
    "resnet56": (RESNET56_CIFAR10, "cifar10"),
    "resnet110": (RESNET110_CIFAR10, "cifar10"),
}


def compile_resnet(spec, library="phantom", device="cuda"):
    """Compile ResNet model with the given backend."""
    model = spec.create_model()

    example_inputs = tuple(torch.randn(inp.shape, dtype=inp.dtype) for inp in spec.example_inputs)

    # Get compile options for the specific library
    compile_options = {}
    if spec.compile_options:
        # Get library-specific options (phantom, ant, ace, etc.)
        lib_options = spec.compile_options.get(library, {})
        compile_options = dict(lib_options)
    compile_options.setdefault("p2c", {})["lib"] = library

    # ReLU variable-precision profile — required for correct inference accuracy
    # and realistic memory footprint (higher-degree mod-reduction polynomials).
    relu_vr_file = spec.get_vr_profile()

    compiled = fhe.compile(
        frontend="torch",
        library=library,
        device=device,
        encrypt_inputs=spec.encrypt_inputs,
        relu_vr_file=relu_vr_file,
        **compile_options,
    )(model)

    return compiled.fhe_compile(example_inputs)


def profile_model(spec, dataset, library, device, num_images, parallel, num_threads, trace_dir, verbose):
    """Profile a single ResNet model."""
    model_name = spec.name
    print(f"\n{'=' * 60}")
    print(f"Profiling: {model_name} ({library}/{device})")
    print(f"{'=' * 60}")

    # Load dataset
    if dataset == "cifar100":
        images, labels = load_cifar100_images(num_images)
    else:
        images, labels = load_cifar10_images(num_images)

    # Compile
    print(f"Compiling {model_name}...")
    start_compile = time.perf_counter()
    program = compile_resnet(spec, library=library, device=device)
    compile_time = time.perf_counter() - start_compile
    print(f"Compilation done in {compile_time:.1f}s.")

    # Profile
    with fhe.profiler(device=device, trace_dir=trace_dir) as prof:
        result = program.run_dataset(
            images[:num_images],
            labels[:num_images],
            top_k=1,
            parallel=parallel,
            num_threads=num_threads,
            verbose=verbose,
        )

    # Print results
    print(f"\n--- {model_name} Results ---")
    print(f"Dataset result: {result}")
    print(prof.summary())

    # Extract key timing info
    summary = prof.summary()
    timing_info = {
        "model": model_name,
        "library": library,
        "device": device,
        "num_images": num_images,
        "compile_time_s": compile_time,
        "total_time_s": result.timing.total_ms / 1000.0,
        "avg_per_image_s": result.timing.avg_per_image_ms / 1000.0,
    }

    return timing_info, prof


def main():
    parser = argparse.ArgumentParser(
        description="ResNet FHE profiling - supports all ResNet models",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Profile ResNet-20 (default)
  python 02_resnet_profile.py

  # Profile ResNet-56 on GPU
  python 02_resnet_profile.py --model resnet56 --library phantom --device cuda

  # Profile all models
  python 02_resnet_profile.py --model all

  # Profile with trace export
  python 02_resnet_profile.py --model resnet20 --trace-dir ./trace

  # Parallel batch execution
  python 02_resnet_profile.py --model resnet20 --parallel --num-threads 8
        """,
    )
    parser.add_argument(
        "--model",
        type=str,
        default="resnet20",
        choices=[*list(MODEL_SPECS.keys()), "all"],
        help="ResNet model to profile (default: resnet20)",
    )
    parser.add_argument(
        "--library",
        default="phantom",
        choices=["ant", "phantom"],
        help="FHE backend library (default: phantom)",
    )
    parser.add_argument(
        "--device",
        default="cuda",
        choices=["cpu", "cuda"],
        help="Device (default: cuda)",
    )
    parser.add_argument(
        "--num-images",
        type=int,
        default=3,
        help="Number of images to infer (default: 3)",
    )
    parser.add_argument(
        "--parallel",
        action="store_true",
        help="Use OpenMP parallel batch execution",
    )
    parser.add_argument(
        "--num-threads",
        type=int,
        default=0,
        help="OpenMP threads (0=auto, only with --parallel)",
    )
    parser.add_argument(
        "--trace-dir",
        default=None,
        help="Export Chrome trace to this directory",
    )
    parser.add_argument(
        "--verbose",
        action="store_true",
        help="Print verbose output for each image",
    )
    args = parser.parse_args()

    # Check GPU availability
    if args.library == "phantom" and not fhe.gpu_available():
        print("GPU not available, falling back to ant/cpu")
        args.library = "ant"
        args.device = "cpu"

    # Determine models to profile
    models_to_profile = list(MODEL_SPECS.keys()) if args.model == "all" else [args.model]

    print(f"Models to profile: {models_to_profile}")
    print(f"Configuration: {args.library}/{args.device}, {args.num_images} images")

    # Profile each model
    all_results = []
    for model_key in models_to_profile:
        if model_key not in MODEL_SPECS:
            print(f"Warning: Unknown model '{model_key}', skipping...")
            continue

        spec, dataset = MODEL_SPECS[model_key]

        # Create trace dir for each model if needed
        trace_dir = None
        if args.trace_dir:
            trace_dir = f"{args.trace_dir}/{model_key}"

        try:
            timing_info, _prof = profile_model(
                spec=spec,
                dataset=dataset,
                library=args.library,
                device=args.device,
                num_images=args.num_images,
                parallel=args.parallel,
                num_threads=args.num_threads,
                trace_dir=trace_dir,
                verbose=args.verbose,
            )
            all_results.append(timing_info)
        except Exception as e:
            print(f"Error profiling {model_key}: {e}")
            import traceback

            traceback.print_exc()

    # Print summary
    if all_results:
        print(f"\n{'=' * 60}")
        print("SUMMARY - All Models")
        print(f"{'=' * 60}")
        print(f"{'Model':<25} {'Library':<10} {'Compile(s)':<12} {'Total(s)':<10} {'Avg/Img(s)':<12}")
        print("-" * 70)
        for r in all_results:
            print(
                f"{r['model']:<25} {r['library']:<10} "
                f"{r['compile_time_s']:<12.1f} {r['total_time_s']:<10.1f} "
                f"{r['avg_per_image_s']:<12.2f}"
            )

    if args.trace_dir:
        print(f"\nChrome traces exported to: {args.trace_dir}/")
        print("Open in chrome://tracing or ui.perfetto.dev")


if __name__ == "__main__":
    main()
