#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
ResNet-20 FHE Inference with ReLU VR from Profile JSON

Demonstrates loading pre-profiled ReLU value range data and embedding it
into AIR IR during compilation, eliminating the need for -SIHE:relu_vr CLI args.

Uses the new ModelSpec + ReLUProfiler API:
    - ModelSpec.get_vr_profile() for auto-discovery
    - CompileOptions(relu_vr_file=...) for embedding VR into AIR IR

Usage:
    # Default: load VR from profile JSON (phantom/cuda)
    python resnet20_relu_vr.py

    # Use antlib/cpu backend
    python resnet20_relu_vr.py --library antlib --device cpu

    # Run 10 or 100 images
    python resnet20_relu_vr.py --num-images 10
    python resnet20_relu_vr.py --num-images 100
"""

import argparse
import time

import torch

from ace import fhe
from ace.fhe.config import CompileOptions
from ace.fhe.config.spec import ModelSpec
from ace.model.spec_resnet import RESNET20_CIFAR10
from ace.model.dataset import load_cifar10_images


def main():
    parser = argparse.ArgumentParser(description="ResNet-20 FHE inference with ReLU VR from profile JSON")
    parser.add_argument("--library", default="phantom", choices=["antlib", "phantom"],
                        help="FHE backend library (default: phantom)")
    parser.add_argument("--device", default="cuda", choices=["cpu", "cuda"],
                        help="Device (default: cuda)")
    parser.add_argument("--num-images", type=int, default=1,
                        help="Number of images to infer (default: 1)")
    args = parser.parse_args()

    if args.library == "phantom" and not fhe.gpu_available():
        print("GPU not available, falling back to antlib/cpu")
        args.library = "antlib"
        args.device = "cpu"

    spec = RESNET20_CIFAR10

    # Step 1: Create model
    print("\n[Step 1] Creating ResNet-20 model...")
    model = spec.create_model()
    print("  Model loaded and set to eval mode")

    # Step 2: Load dataset
    print(f"\n[Step 2] Loading {args.num_images} CIFAR-10 test image(s)...")
    try:
        images, labels = load_cifar10_images(args.num_images)
        print(f"  Loaded {len(labels)} images, shape: {images.shape}")
    except FileNotFoundError as e:
        print(f"  ERROR: CIFAR-10 data not found: {e}")
        print("  Using random input for compilation test")
        images = torch.randn(args.num_images, 3, 32, 32)
        labels = list(range(args.num_images))

    # Step 3: Load ReLU VR profile and compile
    profile_path = spec.get_vr_profile()
    if profile_path is None:
        raise FileNotFoundError(
            "No ReLU VR profile found for ResNet-20. "
            "Run profiling first: python -m ace.model.relu_profile --model resnet20"
        )
    print(f"\n[Step 3] Loading VR profile from: {profile_path}")

    from ace.fhe.config.profiler import _load_vr_file
    vr_data = _load_vr_file(profile_path)
    print(f"  Loaded {len(vr_data)} ReLU VR entries")
    for name, vr in list(vr_data.items())[:5]:
        print(f"    {name} = {vr}")
    if len(vr_data) > 5:
        print(f"    ... ({len(vr_data) - 5} more)")

    compile_opts = dict(spec.compile_options) if spec.compile_options else {}
    compile_opts.setdefault("p2c", {})["lib"] = args.library

    print(f"\n[Step 4] Compiling ResNet-20 ({args.library}/{args.device})...")
    example_inputs = (images[:1],)

    t0 = time.time()
    compiled = fhe.compile(
        frontend="torch",
        library=args.library,
        device=args.device,
        encrypt_inputs=spec.encrypt_inputs,
        relu_vr_file=profile_path,
        **compile_opts,
    )(model)
    program = compiled.fhe_compile(example_inputs)
    t_compile = time.time() - t0
    print(f"  Compilation done in {t_compile:.1f}s")

    # Step 5: Plaintext inference
    print(f"\n[Step 5] Running plaintext inference on {args.num_images} image(s)...")
    with torch.no_grad():
        plain_outputs = model(images)
        plain_preds = plain_outputs.argmax(dim=1).tolist()
    plain_correct = sum(1 for p, l in zip(plain_preds, labels) if p == l)
    print(f"  Plaintext accuracy: {plain_correct}/{args.num_images}"
          f" ({100*plain_correct/args.num_images:.1f}%)")

    # Step 6: FHE inference
    print(f"\n[Step 6] Running FHE inference on {args.num_images} image(s)...")
    t0 = time.time()
    result = program.run_dataset(
        images[:args.num_images],
        labels[:args.num_images],
        top_k=1,
        verbose=True,
        plaintext_predictions=plain_preds[:args.num_images],
    )
    t_infer = time.time() - t0
    print(f"  FHE inference done in {t_infer:.1f}s")
    print(f"  Result: {result}")

    # Summary
    print(f"\n{'=' * 60}")
    print(f"Summary:")
    print(f"  Model:       ResNet-20 (CIFAR-10)")
    print(f"  Backend:     {args.library}/{args.device}")
    print(f"  VR source:   relu_vr_file ({profile_path})")
    print(f"  VR entries:  {len(vr_data)}")
    print(f"  Compile:     {t_compile:.1f}s")
    print(f"  Inference:   {t_infer:.1f}s ({args.num_images} images)")
    print(f"  Plaintext:   {plain_correct}/{args.num_images} correct")
    print(f"  FHE result:  {result}")
    print(f"{'=' * 60}")


if __name__ == "__main__":
    main()