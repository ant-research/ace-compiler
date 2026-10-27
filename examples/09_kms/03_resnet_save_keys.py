#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
KMS Step 1 (ResNet): Generate FHE keys and save to disk

Compiles a ResNet model with the phantom GPU backend, runs inference to trigger
key generation, then saves the keys to disk. Key generation for these models
takes significant time — KMS lets you pay that cost once and reuse the keys.

Galois keys are saved in per-key format: one .bin file per relin_key +
a galois_key.index.json.

Supported models (via --model):
  resnet20   — ResNet-20 on CIFAR-10  (default, fastest)
  resnet32   — ResNet-32 on CIFAR-10
  resnet44   — ResNet-44 on CIFAR-10
  resnet56   — ResNet-56 on CIFAR-10
  resnet110  — ResNet-110 on CIFAR-10

I/O transport is configurable via IoConfig:
  --io auto        Auto-detect GDS (default): use cuFile if available
  --io gds         Force GDS (fail if cuFile unavailable)
  --io staged      Force host-staged I/O (no GDS)

Usage:
    python3 03_resnet_save_keys.py [KEY_DIR] [--model MODEL] [--io {auto,gds,staged}]
                                    [--disk-paths DISK1 DISK2 ...]

Examples:
    # Default: ResNet-20, auto-detect GDS
    python3 03_resnet_save_keys.py

    # ResNet-110 keys
    python3 03_resnet_save_keys.py --model resnet110

    # Force GDS (direct GPU<->SSD)
    python3 03_resnet_save_keys.py --io gds

    # Multi-disk for galois key distribution
    python3 03_resnet_save_keys.py --disk-paths /mnt/nvme0/keys /mnt/nvme1/keys

Next step:
    python3 04_resnet_load_keys.py KEY_DIR

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
from ace.fhe.cache import configure_cache
from ace.fhe.config import IoConfig, IoPolicySettings
from ace.fhe.util import gpu_available
from ace.model.spec_resnet import (
    RESNET20_CIFAR10,
    RESNET32_CIFAR10,
    RESNET44_CIFAR10,
    RESNET56_CIFAR10,
    RESNET110_CIFAR10,
)


# Model name → ModelSpec mapping
MODEL_SPECS = {
    "resnet20": RESNET20_CIFAR10,
    "resnet32": RESNET32_CIFAR10,
    "resnet44": RESNET44_CIFAR10,
    "resnet56": RESNET56_CIFAR10,
    "resnet110": RESNET110_CIFAR10,
}


def _find_phantom_lib():
    """Find the phantom shared library for the key save C API."""
    import ace

    candidates = [
        os.path.join(os.path.dirname(ace.__file__), "lib", "libFHErt_phantom.so"),
        os.environ.get("ACE_PHANTOM_LIB", ""),
    ]
    for candidate in candidates:
        if candidate and os.path.exists(candidate):
            return candidate
    raise FileNotFoundError("Cannot find libFHErt_phantom.so. Set ACE_PHANTOM_LIB to its path.")


def _apply_io_config(io_mode: str, disk_paths: list[str] | None = None):
    """Apply I/O transport configuration based on --io argument."""
    disk_paths = disk_paths or []
    policy_map = {
        "auto": "auto",
        "gds": "gds",
        "staged": "staged",
        "mapped": "mapped",
    }
    IoPolicySettings(policy=policy_map[io_mode], disk_paths=disk_paths).apply()

    gds_status = "available" if _C.gds_available() else "not available"
    print(f"  I/O config:  {io_mode} (GDS {gds_status})")
    if disk_paths:
        print(f"  Disk paths:  {disk_paths}")


def main():
    parser = argparse.ArgumentParser(description="KMS ResNet: Save FHE keys")
    parser.add_argument(
        "key_dir", nargs="?", default=None, help="Output directory for saved keys (default: ./kms_demo_keys_resnet{N})"
    )
    parser.add_argument(
        "--model", choices=list(MODEL_SPECS.keys()), default="resnet20", help="ResNet model to use (default: resnet20)"
    )
    parser.add_argument(
        "--io",
        choices=["auto", "gds", "staged", "host_mapped"],
        default="auto",
        help="I/O transport mode: auto (default), gds (force cuFile), staged (force host-staged, no GDS)",
    )
    parser.add_argument(
        "--disk-paths",
        nargs="+",
        default=[],
        help="Multi-disk paths for per-key galois key distribution. "
        "Key i is placed on disk_paths[i % len(disk_paths)]. "
        "If not specified, all keys go to key_dir/model/.",
    )
    parser.add_argument("--cache-dir", default=None, help="Compile cache directory (default: /tmp/ace-compile-cache)")
    args = parser.parse_args()

    spec = MODEL_SPECS[args.model]
    model_label = spec.name if hasattr(spec, "name") else args.model

    if args.key_dir is None:
        args.key_dir = f"./kms_demo_keys_{args.model}"
    key_dir = os.path.abspath(args.key_dir)

    if not gpu_available():
        print("ERROR: GPU not available. This example requires CUDA.")
        sys.exit(1)

    print("=" * 60)
    print(f"KMS {model_label}: Generate and Save FHE Keys")
    print("=" * 60)
    print(f"  Model:      {model_label}")
    print(f"  Key dir:    {key_dir}")

    # Apply I/O configuration
    _apply_io_config(args.io, args.disk_paths)
    print()

    # Configure compile cache directory
    if args.cache_dir:
        cache_dir = os.path.abspath(args.cache_dir)
        os.makedirs(cache_dir, exist_ok=True)
        configure_cache(cache_dir=cache_dir)
        print(f"  Cache dir:  {cache_dir}")

    # -- Compile model --------------------------------------------
    print(f"[1/4] Compiling {model_label} ...")
    t0 = time.time()

    model = spec.create_model()
    example_inputs = tuple(torch.randn(inp.shape, dtype=inp.dtype) for inp in spec.example_inputs)

    # Build compile options for phantom
    compile_options = dict(spec.compile_options.get("phantom", {}))
    compile_options.setdefault("p2c", {})["lib"] = "phantom"

    # Load ReLU VR profile for optimal scale management
    vr_file = spec.get_vr_profile()

    compiled = fhe.compile(
        frontend="torch",
        library="phantom",
        device="cuda",
        encrypt_inputs=spec.encrypt_inputs,
        relu_vr_file=vr_file,
        **compile_options,
    )(model)
    prog = compiled.fhe_compile(example_inputs)

    t_compile = time.time() - t0
    print(f"      Done ({t_compile:.1f}s)")

    # -- Run inference (triggers key generation) ------------------
    print("[2/4] Running inference (keys will be generated) ...")
    t0 = time.time()
    _C.set_key_path("")  # ensure no pre-set key_path

    runner = fhe.FHERuntime(prog.package)
    x = torch.randn(*spec.example_inputs[0].shape, dtype=torch.float32)
    result = runner.run(x)

    t_infer = time.time() - t0
    print(f"      Result shape: {result.shape}")
    print(f"      Inference time: {t_infer:.1f}s (includes key generation)")

    # -- Save keys to disk ----------------------------------------
    print(f"[3/4] Saving keys to {key_dir} ...")
    t0 = time.time()

    rc = runner.save_keys(key_dir)
    if rc != 0:
        print(f"  ERROR: save_keys failed with rc={rc}")
        sys.exit(1)

    runner.finalize()

    t_save = time.time() - t0
    print(f"      Done ({t_save:.1f}s)")

    # -- Verify ---------------------------------------------------
    print("[4/4] Verifying saved key files ...")

    # Check single-file keys
    single_key_files = [
        ("global/secret_key.bin", "Secret key"),
        ("global/public_key.bin", "Public key"),
        ("model/relin_key.bin", "Relin key"),
    ]
    total = 0
    for rel_path, label in single_key_files:
        fpath = os.path.join(key_dir, rel_path)
        if not os.path.exists(fpath):
            print(f"  ERROR: Missing {fpath}")
            sys.exit(1)
        size = os.path.getsize(fpath)
        total += size
        print(f"      {label:12s}  {size:>10,d} bytes  ({size / 1024 / 1024:.1f} MB)")

    # Check per-key galois files
    index_path = os.path.join(key_dir, "model", "galois_key.index.json")
    if not os.path.exists(index_path):
        print(f"  ERROR: Missing galois_key.index.json at {index_path}")
        sys.exit(1)

    with open(index_path) as f:
        index_data = json.load(f)

    num_relin_keys = len(index_data.get("relin_keys", []))
    galois_total = 0
    missing_keys = 0
    for entry in index_data["relin_keys"]:
        # Check on the correct disk path
        disk_idx = entry.get("disk", 0)
        disk_paths_in_index = index_data.get("disk_paths", [])
        if disk_paths_in_index and disk_idx < len(disk_paths_in_index):
            key_path = os.path.join(disk_paths_in_index[disk_idx], "model", entry["file"])
        else:
            key_path = os.path.join(key_dir, "model", entry["file"])

        if not os.path.exists(key_path):
            missing_keys += 1
            print(f"  WARNING: Missing {key_path}")
        else:
            galois_size = os.path.getsize(key_path)
            galois_total += galois_size

    if missing_keys > 0:
        print(f"  ERROR: {missing_keys} galois key files missing")
        sys.exit(1)

    total += galois_total
    print(
        f"      {'Galois keys':12s}  {galois_total:>10,d} bytes  "
        f"({galois_total / 1024 / 1024:.1f} MB, {num_relin_keys} files)"
    )
    print(f"      {'Total':12s}  {total:>10,d} bytes  ({total / 1024 / 1024:.1f} MB)")

    # Write metadata for the load step
    meta = {
        "model": model_label,
        "model_key": args.model,
        "key_dir": key_dir,
        "kernel": prog.package["kernel"],
        "config_path": prog.package.get("config_path", ""),
        "example_input_shape": list(spec.example_inputs[0].shape),
        "io_mode": args.io,
        "disk_paths": args.disk_paths,
    }
    meta_path = os.path.join(key_dir, "meta.json")
    with open(meta_path, "w") as f:
        json.dump(meta, f, indent=2)
    print(f"      Metadata:  {meta_path}")

    print()
    print("=" * 60)
    print("Keys saved successfully!")
    print(f"  Compile:    {t_compile:.1f}s")
    print(f"  Inference:  {t_infer:.1f}s (includes key generation)")
    print(f"  Key save:   {t_save:.1f}s")
    print(f"  Total keys: {total / 1024 / 1024:.1f} MB")
    print()
    print("To load these keys in a new process, run:")
    print(f"  python3 04_resnet_load_keys.py {key_dir}")
    if args.disk_paths:
        print(f"  python3 04_resnet_load_keys.py {key_dir} --disk-paths {' '.join(args.disk_paths)}")
    print("=" * 60)


if __name__ == "__main__":
    main()
