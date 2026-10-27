#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
KMS Step 2 (ResNet): Load pre-generated FHE keys and run inference

This script loads keys saved by 03_resnet_save_keys.py, skipping the
expensive key generation step. The model is read from meta.json in the
key directory.

Galois keys are loaded in per-key format with parallel I/O (controlled
by --num-io-threads).

Timing breakdown (uses C++ BatchTiming for precise inference timing):
  - Init:  key loading from disk + weight encoding  (wall-clock minus inference)
  - Infer: average pure inference time per image    (from C++ BatchTiming)
  - Total: end-to-end wall-clock time

I/O transport is configurable via IoConfig:
  --io auto        Auto-detect GDS (default): use cuFile if available
  --io gds         Force GDS (fail if cuFile unavailable)
  --io staged      Force host-staged I/O (no GDS)

Usage:
    python3 04_resnet_load_keys.py KEY_DIR [--io {auto,gds,staged}]
                                           [--num-io-threads N]
                                           [--disk-paths DISK1 DISK2 ...]
                                           [--num-images N]

    KEY_DIR -- directory containing saved keys (from 03_resnet_save_keys.py)

Examples:
    # Default (auto-detect GDS, 1 image, 1 I/O thread)
    python3 04_resnet_load_keys.py ./kms_demo_keys_resnet20

    # Force GDS with 4 parallel I/O threads
    python3 04_resnet_load_keys.py ./kms_demo_keys_resnet20 --io gds --num-io-threads 4

    # Multi-disk (must match the disk_paths used during save)
    python3 04_resnet_load_keys.py ./kms_demo_keys_resnet20 --disk-paths /mnt/nvme0/keys /mnt/nvme1/keys

    # Multiple images for stable averaging
    python3 04_resnet_load_keys.py ./kms_demo_keys_resnet20 --num-images 3

Requires: CUDA GPU, phantom backend
"""

import argparse
import ctypes
import json
import os
import sys
import time

import torch

from ace import fhe
from ace import runtime as _C
from ace.fhe.config import IoConfig, IoPolicySettings
from ace.fhe.runtime import DatasetEvaluator
from ace.fhe.util import gpu_available
from ace.model.dataset import load_cifar10_images


def _find_phantom_lib():
    """Find the phantom shared library for the key load C API."""
    import ace

    candidates = [
        os.path.join(os.path.dirname(ace.__file__), "lib", "libFHErt_phantom.so"),
        os.environ.get("ACE_PHANTOM_LIB", ""),
    ]
    for candidate in candidates:
        if candidate and os.path.exists(candidate):
            return candidate
    raise FileNotFoundError("Cannot find libFHErt_phantom.so. Set ACE_PHANTOM_LIB to its path.")


def _apply_io_config(io_mode: str, threads: int = 1, disk_paths: list[str] | None = None):
    """Apply I/O transport configuration based on --io argument."""
    disk_paths = disk_paths or []
    policy_map = {
        "auto": "auto",
        "gds": "gds",
        "staged": "staged",
        "mapped": "mapped",
    }
    IoPolicySettings(policy=policy_map[io_mode], disk_paths=disk_paths).apply()
    IoConfig(threads=threads, disk_paths=disk_paths).apply()

    gds_status = "available" if _C.gds_available() else "not available"
    print(f"  I/O config:     {io_mode} (GDS {gds_status})")
    print(f"  I/O threads:    {threads}")
    if disk_paths:
        print(f"  Disk paths:     {disk_paths}")


def main():
    parser = argparse.ArgumentParser(description="KMS ResNet: Load FHE keys and run inference")
    parser.add_argument("key_dir", help="Directory with saved keys (from 03_resnet_save_keys.py)")
    parser.add_argument(
        "--io",
        choices=["auto", "gds", "staged", "host_mapped"],
        default="auto",
        help="I/O data path: auto (default), gds (direct DMA), "
        "host_staged (fread+cudaMemcpy), host_mapped (mmap+cudaMemcpy)",
    )
    parser.add_argument(
        "--num-io-threads",
        type=int,
        default=1,
        help="Number of parallel I/O threads for galois key loading (default: 1, recommended: 4 for GDS)",
    )
    parser.add_argument(
        "--disk-paths",
        nargs="+",
        default=[],
        help="Multi-disk paths for per-key galois key loading. "
        "Must match the disk_paths used during save. "
        "If not specified, disk info from index.json is used.",
    )
    parser.add_argument(
        "--num-images", type=int, default=1, help="Number of CIFAR-10 images for inference (default: 1)"
    )
    args = parser.parse_args()

    key_dir = os.path.abspath(args.key_dir)
    meta_path = os.path.join(key_dir, "meta.json")

    if not gpu_available():
        print("ERROR: GPU not available. This example requires CUDA.")
        sys.exit(1)

    if not os.path.isdir(key_dir):
        print(f"ERROR: Key directory not found: {key_dir}")
        sys.exit(1)

    if not os.path.exists(meta_path):
        print(f"ERROR: meta.json not found in {key_dir}")
        print("  Did you run 03_resnet_save_keys.py first?")
        sys.exit(1)

    with open(meta_path) as f:
        meta = json.load(f)

    kernel_path = meta["kernel"]
    config_path = meta.get("config_path", "")
    model_name = meta.get("model", "ResNet")

    # If disk_paths not specified on CLI, try meta.json
    disk_paths = args.disk_paths
    if not disk_paths:
        disk_paths = meta.get("disk_paths", [])

    print("=" * 60)
    print(f"KMS {model_name}: Load Pre-Generated Keys and Run Inference")
    print("=" * 60)
    print(f"  Model:      {model_name}")
    print(f"  Key dir:    {key_dir}")
    print(f"  Kernel:     {kernel_path}")
    print(f"  Images:     {args.num_images}")

    # Apply I/O configuration
    _apply_io_config(args.io, args.num_io_threads, disk_paths)
    print()

    # Check galois_key.index.json exists (per-key format required)
    index_path = os.path.join(key_dir, "model", "galois_key.index.json")
    if not os.path.exists(index_path):
        # Also check disk_paths[0] if multi-disk
        if disk_paths:
            alt_index = os.path.join(disk_paths[0], "model", "galois_key.index.json")
            if os.path.exists(alt_index):
                index_path = alt_index
            else:
                print("ERROR: galois_key.index.json not found")
                print(f"  Tried: {os.path.join(key_dir, 'model', 'galois_key.index.json')}")
                if disk_paths:
                    print(f"  Tried: {alt_index}")
                sys.exit(1)
        else:
            print(f"ERROR: galois_key.index.json not found at {index_path}")
            print("  Per-key galois key format is required.")
            print("  Run 03_resnet_save_keys.py first to generate keys.")
            sys.exit(1)

    # -- Set key_path and create FHERuntime -----------------------
    print("[1/3] Setting key_path and creating FHERuntime ...")
    _C.set_key_path(key_dir)
    print(f"      key_path = {_C.get_key_path()}")

    package = {
        "model": meta.get("model_key", "resnet20_cifar10"),
        "kernel": kernel_path,
        "config_path": config_path,
        "key_path": key_dir,
    }
    runner = fhe.FHERuntime(package)
    print("      FHERuntime created (keys will be loaded from disk).")

    # -- Run inference using DatasetEvaluator for precise timing --------
    print(f"[2/3] Running inference on {args.num_images} image(s) ...")

    images, labels = load_cifar10_images(args.num_images)

    t_start = time.perf_counter()
    dataset_result = DatasetEvaluator(runner).evaluate(
        images[: args.num_images],
        labels[: args.num_images],
        top_k=1,
        verbose=False,
    )
    t_end = time.perf_counter()
    total_s = t_end - t_start

    # Timing breakdown
    if dataset_result.timing.num_images > 0:
        infer_total_s = dataset_result.timing.total_ms / 1000.0
        infer_s = dataset_result.timing.avg_per_image_ms / 1000.0
        init_s = total_s - infer_total_s
    else:
        init_s = 0.0
        infer_s = 0.0

    print(f"      init={init_s:.2f}s  infer={infer_s:.2f}s  total={total_s:.2f}s")
    print(f"      Top-1 accuracy: {dataset_result.top1_accuracy:.0%}")

    runner.finalize()

    # -- Verify keys were loaded (not regenerated) ---------------
    print("[3/3] Verifying keys were loaded from disk ...")

    is_loaded = runner.keys_loaded_from_kms()

    if is_loaded == 1:
        print("      Keys loaded from KMS: YES (per-key format)")
    else:
        print("      Keys loaded from KMS: NO (keys were regenerated!)")

    # -- Summary --------------------------------------------------
    print()
    print("=" * 60)
    if is_loaded == 1:
        print(f"Success! Pre-generated {model_name} keys loaded and")
        print("inference completed without key generation.")
        print()
        print(f"  Init (key load + encode):  {init_s:.2f}s")
        print(f"  Inference (avg/image):     {infer_s:.2f}s")
        print(f"  Total wall-clock:          {total_s:.2f}s")
        print(f"  I/O threads:               {args.num_io_threads}")
    else:
        print("WARNING: Keys were NOT loaded from disk -- they were")
        print("regenerated. Check that key files exist in the key dir.")
    print("=" * 60)

    if is_loaded != 1:
        sys.exit(1)


if __name__ == "__main__":
    main()
