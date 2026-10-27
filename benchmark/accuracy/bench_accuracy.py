#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Encrypted vs plaintext accuracy benchmark for FHE models.

Runs 100-image inference on LR, LeNet and ResNet-20, comparing FHE top-1
accuracy with the plaintext PyTorch model accuracy. The script is
backend-agnostic via ``--library`` / ``--device`` (default phantom/cuda).

Usage::

    # GPU (phantom)
    python3 benchmark/accuracy/bench_accuracy.py

    # CPU (ant)
    python3 benchmark/accuracy/bench_accuracy.py --library ant --device cpu

    # JSON report
    python3 benchmark/accuracy/bench_accuracy.py --output accuracy_report.json
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

repo_root = Path(__file__).resolve().parents[2]
if str(repo_root) not in sys.path:
    sys.path.insert(0, str(repo_root))

from ace import fhe
from ace.model.dataset import load_cifar10_images
from ace.model.spec_lenet import LENET_CIFAR10
from ace.model.spec_lr import LR_1D
from ace.model.spec_resnet import RESNET20_CIFAR10

DEFAULT_CACHE_DIR = "/tmp/ace-compile-cache"
NUM_IMAGES = 100


@dataclass(frozen=True)
class AccuracyResult:
    model: str
    library: str
    device: str
    num_images: int
    encrypted_accuracy_pct: float
    plaintext_accuracy_pct: float
    delta_pct: float
    compile_time_s: float
    inference_time_s: float


def clear_compile_cache(name_fragment: str, cache_dir: str = DEFAULT_CACHE_DIR) -> None:
    if not os.path.isdir(cache_dir):
        return
    for entry in os.listdir(cache_dir):
        if name_fragment in entry:
            shutil.rmtree(os.path.join(cache_dir, entry), ignore_errors=True)


def compile_spec(spec, library: str, device: str, **kwargs) -> tuple[Any, torch.nn.Module]:
    model = spec.create_model()
    model.eval()
    compile_opts = dict(spec.compile_options.get(library, {})) if spec.compile_options else {}
    compile_opts.update(kwargs)
    compiled = fhe.compile(
        frontend="torch",
        library=library,
        device=device,
        encrypt_inputs=spec.encrypt_inputs,
        **compile_opts,
    )(model)
    program = compiled.fhe_compile(spec.example_inputs)
    return program, model


def compute_plaintext_accuracy(model: torch.nn.Module, images: torch.Tensor, labels: list[int]) -> int:
    correct = 0
    with torch.no_grad():
        for i in range(len(labels)):
            pred = model(images[i : i + 1]).argmax(dim=1).item()
            if pred == labels[i]:
                correct += 1
    return correct


def benchmark_lr(library: str, device: str) -> AccuracyResult:
    clear_compile_cache("lr_1d")
    t0 = time.perf_counter()
    program, model = compile_spec(LR_1D, library, device)
    t1 = time.perf_counter()

    x = LR_1D.example_inputs[0]
    images = x.repeat(NUM_IMAGES, 1)
    labels = [0] * NUM_IMAGES

    t2 = time.perf_counter()
    result = program.run_dataset(images, labels, top_k=1, verbose=False)
    t3 = time.perf_counter()

    plain_correct = compute_plaintext_accuracy(model, images, labels)
    fhe_correct = result.num_correct_top1

    return AccuracyResult(
        model="LR",
        library=library,
        device=device,
        num_images=NUM_IMAGES,
        encrypted_accuracy_pct=round(fhe_correct / NUM_IMAGES * 100, 2),
        plaintext_accuracy_pct=round(plain_correct / NUM_IMAGES * 100, 2),
        delta_pct=round((plain_correct - fhe_correct) / NUM_IMAGES * 100, 2),
        compile_time_s=round(t1 - t0, 2),
        inference_time_s=round(t3 - t2, 2),
    )


def benchmark_cifar_model(name: str, spec, library: str, device: str, **compile_kwargs) -> AccuracyResult:
    clear_compile_cache(spec.name)
    t0 = time.perf_counter()
    program, model = compile_spec(spec, library, device, **compile_kwargs)
    t1 = time.perf_counter()

    images, labels = load_cifar10_images(NUM_IMAGES)

    t2 = time.perf_counter()
    result = program.run_dataset(images, labels, top_k=1, verbose=False)
    t3 = time.perf_counter()

    plain_correct = compute_plaintext_accuracy(model, images, labels)
    fhe_correct = result.num_correct_top1

    return AccuracyResult(
        model=name,
        library=library,
        device=device,
        num_images=NUM_IMAGES,
        encrypted_accuracy_pct=round(fhe_correct / NUM_IMAGES * 100, 2),
        plaintext_accuracy_pct=round(plain_correct / NUM_IMAGES * 100, 2),
        delta_pct=round((plain_correct - fhe_correct) / NUM_IMAGES * 100, 2),
        compile_time_s=round(t1 - t0, 2),
        inference_time_s=round(t3 - t2, 2),
    )


def parse_args(argv: list[str] | None = None) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Encrypted/plaintext accuracy benchmark on FHE models.")
    parser.add_argument("--library", default="phantom", help="Backend library (default: phantom).")
    parser.add_argument("--device", default="cuda", help="Target device (default: cuda).")
    parser.add_argument("--num-images", type=int, default=NUM_IMAGES, help="Number of images (default: 100).")
    parser.add_argument("--cache-dir", default=DEFAULT_CACHE_DIR, help="ACE compile cache dir.")
    parser.add_argument("--output", default=None, help="Optional JSON output file.")
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv)
    global NUM_IMAGES
    NUM_IMAGES = args.num_images

    resnet_vr_file = "/opt/ace-env/lib/python3.12/site-packages/ace/model/resnet/profiles/resnet20_cifar10.json"

    print(f"Backend: {args.library}/{args.device}, images={NUM_IMAGES}")
    print("-" * 90)
    print(f"{'Model':<12} {'Encrypted%':>12} {'Plain%':>12} {'Delta%':>12} {'Compile(s)':>14} {'Infer(s)':>12}")
    print("-" * 90)

    results = []
    lr = benchmark_lr(args.library, args.device)
    results.append(lr)
    print(
        f"{lr.model:<12} {lr.encrypted_accuracy_pct:>12.2f} {lr.plaintext_accuracy_pct:>12.2f} "
        f"{lr.delta_pct:>12.2f} {lr.compile_time_s:>14.2f} {lr.inference_time_s:>12.2f}"
    )

    # Process-per-model to avoid cross-model GPU context issues.
    print(flush=True)

    lenet = benchmark_cifar_model("LeNet", LENET_CIFAR10, args.library, args.device, profile_relu=True)
    results.append(lenet)
    print(
        f"{lenet.model:<12} {lenet.encrypted_accuracy_pct:>12.2f} {lenet.plaintext_accuracy_pct:>12.2f} "
        f"{lenet.delta_pct:>12.2f} {lenet.compile_time_s:>14.2f} {lenet.inference_time_s:>12.2f}",
        flush=True,
    )

    resnet20 = benchmark_cifar_model("ResNet-20", RESNET20_CIFAR10, args.library, args.device, relu_vr_file=resnet_vr_file)
    results.append(resnet20)
    print(
        f"{resnet20.model:<12} {resnet20.encrypted_accuracy_pct:>12.2f} {resnet20.plaintext_accuracy_pct:>12.2f} "
        f"{resnet20.delta_pct:>12.2f} {resnet20.compile_time_s:>14.2f} {resnet20.inference_time_s:>12.2f}"
    )

    print("-" * 90)

    if args.output:
        payload = {
            "library": args.library,
            "device": args.device,
            "num_images": NUM_IMAGES,
            "results": [asdict(r) for r in results],
        }
        with open(args.output, "w") as f:
            json.dump(payload, f, indent=2, sort_keys=True)
        print(f"Wrote results to {args.output}")

    return 0


if __name__ == "__main__":
    raise SystemExit(main())