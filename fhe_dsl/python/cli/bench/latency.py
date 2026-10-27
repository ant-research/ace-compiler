# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
Benchmark latency subcommand — detailed FHE inference timing.

Separates compilation, data loading, initialization (fhe::init), and
inference (fhe::run_batch_sequential) phases for each model.

Usage:
    ace_tool bench latency --model resnet20 --num-images 10
    ace_tool bench latency --model all --num-images 10
    ace_tool bench latency --model resnet32 --num-images 5 --library phantom --device cuda
"""

import argparse
import time
from contextlib import suppress

import torch

from ace import fhe
from ace.model.dataset import load_cifar10_images, load_cifar100_images
from ace.model.spec_resnet import (
    RESNET20_CIFAR10,
    RESNET32_CIFAR10,
    RESNET32_CIFAR100,
    RESNET44_CIFAR10,
    RESNET56_CIFAR10,
    RESNET110_CIFAR10,
)


MODEL_SPECS = {
    "resnet20": (RESNET20_CIFAR10, "cifar10"),
    "resnet32": (RESNET32_CIFAR10, "cifar10"),
    "resnet32_c100": (RESNET32_CIFAR100, "cifar100"),
    "resnet44": (RESNET44_CIFAR10, "cifar10"),
    "resnet56": (RESNET56_CIFAR10, "cifar10"),
    "resnet110": (RESNET110_CIFAR10, "cifar10"),
}


def register(subparsers):
    """Register the latency subcommand parser."""
    p = subparsers.add_parser(
        "latency",
        help="Detailed latency benchmark with phase breakdown",
        description="Run FHE inference and report timing for each phase: "
        "compilation, data loading, initialization (fhe::init), "
        "and inference (fhe::run_batch_sequential).",
        formatter_class=argparse.RawDescriptionHelpFormatter,
    )
    p.add_argument(
        "--model",
        default="resnet20",
        choices=[*MODEL_SPECS.keys(), "all"],
        help="Model to benchmark (default: resnet20). Use 'all' for all models.",
    )
    p.add_argument(
        "--library",
        default="phantom",
        choices=["ant", "phantom", "ace"],
        help="Backend library (default: phantom)",
    )
    p.add_argument(
        "--device",
        default="cuda",
        choices=["cpu", "cuda"],
        help="Device (default: cuda)",
    )
    p.add_argument(
        "--num-images",
        type=int,
        default=10,
        help="Number of images for inference (default: 10)",
    )


def _compile_model(spec, library="phantom", device="cuda"):
    """Compile the model and return a CompiledProgram."""
    model = spec.create_model()
    example_inputs = tuple(torch.randn(inp.shape, dtype=inp.dtype) for inp in spec.example_inputs)

    compile_options = {}
    if spec.compile_options:
        lib_options = spec.compile_options.get(library, {})
        compile_options = dict(lib_options)
    compile_options.setdefault("p2c", {})["lib"] = library

    compiled = fhe.compile(
        frontend="torch",
        library=library,
        device=device,
        encrypt_inputs=spec.encrypt_inputs,
        **compile_options,
    )(model)

    return compiled.fhe_compile(example_inputs)


def _run_benchmark(spec, dataset, num_images, library="phantom", device="cuda"):
    """Run detailed latency benchmark for a single model.

    Returns a dict with per-phase timing data.
    """
    model_name = spec.name

    print(f"\n{'=' * 60}")
    print(f"Testing: {model_name} ({library}/{device})")
    print(f"{'=' * 60}")

    # Load data
    print(f"Loading {num_images} images...")
    load_start = time.perf_counter()
    if dataset == "cifar100":
        images, labels = load_cifar100_images(num_images)
    else:
        images, labels = load_cifar10_images(num_images)
    load_time = time.perf_counter() - load_start
    print(f"Data loading time: {load_time:.2f}s")

    # Compile model (not counted in test)
    print("Compiling model...")
    compile_start = time.perf_counter()
    program = _compile_model(spec, library, device)
    compile_time = time.perf_counter() - compile_start
    print(f"Compilation time: {compile_time:.1f}s")

    # Run with profiler
    print("\nRunning inference with profiling...")
    with fhe.profiler(device=device) as prof:
        result = program.run_dataset(
            images[:num_images],
            labels[:num_images],
            top_k=1,
            verbose=False,
        )

    # Get profiler results
    summary = prof.summary()

    # Parse each phase timing
    timing_data = {
        "model": model_name,
        "library": library,
        "device": device,
        "num_images": num_images,
        "compile_time_s": round(compile_time, 1),
        "data_load_time_s": round(load_time, 2),
        "total_batch_time_s": round(result.timing.total_ms / 1000, 2),
        "avg_per_image_s": round(result.timing.avg_per_image_ms / 1000, 2),
        "min_per_image_s": round(result.timing.min_image_ms / 1000, 2),
        "max_per_image_s": round(result.timing.max_image_ms / 1000, 2),
    }

    # Parse detailed time from profiler summary
    # Example format: "fhe::init [343MB gpu]     27327.1        1         27327.1"
    init_time_ms = 0
    run_time_ms = 0

    for line in summary.split("\n"):
        if "fhe::init" in line:
            parts = line.split()
            for i, p in enumerate(parts):
                if p == "fhe::init" and i + 1 < len(parts):
                    with suppress(ValueError):
                        init_time_ms = float(parts[i + 1])
        elif "fhe::run_batch_sequential" in line:
            parts = line.split()
            for i, p in enumerate(parts):
                if p == "fhe::run_batch_sequential" and i + 1 < len(parts):
                    with suppress(ValueError):
                        run_time_ms = float(parts[i + 1])

    timing_data["init_time_s"] = round(init_time_ms / 1000, 2) if init_time_ms else 0
    timing_data["inference_time_s"] = round(run_time_ms / 1000, 2) if run_time_ms else timing_data["total_batch_time_s"]

    # Print results
    print("\n--- Timing Results ---")
    print(f"Compilation time:  {timing_data['compile_time_s']:>8.1f}s  (key generation/context init)")
    print(f"Data loading time: {timing_data['data_load_time_s']:>8.2f}s  (image loading)")
    print(f"Init time:         {timing_data['init_time_s']:>8.2f}s  (fhe::init)")
    print(f"Inference time:    {timing_data['inference_time_s']:>8.2f}s  (fhe::run_batch_sequential)")
    print(f"Total batch time:  {timing_data['total_batch_time_s']:>8.2f}s")
    print(f"Avg per image:     {timing_data['avg_per_image_s']:>8.2f}s")
    print(f"Min per image:     {timing_data['min_per_image_s']:>8.2f}s")
    print(f"Max per image:     {timing_data['max_per_image_s']:>8.2f}s")

    return timing_data


def run(args):
    """Execute the latency benchmark."""
    models = list(MODEL_SPECS.keys()) if args.model == "all" else [args.model]

    all_results = []
    for model_key in models:
        spec, dataset = MODEL_SPECS[model_key]
        result = _run_benchmark(spec, dataset, args.num_images, args.library, args.device)
        all_results.append(result)

    # Print summary table
    print(f"\n{'=' * 80}")
    print("SUMMARY - All Models")
    print(f"{'=' * 80}")
    print(f"{'Model':<25} {'Compile':<8} {'Load':<7} {'Init':<7} {'Infer':<8} {'Avg/Img':<9} {'Min':<8} {'Max':<8}")
    print("-" * 80)
    for r in all_results:
        print(
            f"{r['model']:<25} "
            f"{r['compile_time_s']:<8.1f} "
            f"{r['data_load_time_s']:<7.2f} "
            f"{r['init_time_s']:<7.2f} "
            f"{r['inference_time_s']:<8.2f} "
            f"{r['avg_per_image_s']:<9.2f} "
            f"{r['min_per_image_s']:<8.2f} "
            f"{r['max_per_image_s']:<8.2f}"
        )
