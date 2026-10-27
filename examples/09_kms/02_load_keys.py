#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
KMS Step 2: Load pre-generated FHE keys and run inference

This script loads keys that were saved by 01_save_keys.py, skipping
the expensive key generation step. Setting key_path before creating
FHERuntime causes the C++ runtime to *load* keys instead of *generating*
them.

IO policy is configurable via --io:
  --io auto        Auto-detect GDS (default)
  --io gds         Force GDS (fail if cuFile unavailable)
  --io staged      Force host-staged I/O (no GDS)
  --io mapped      Force host-mapped I/O (mmap + cudaMemcpy)

Deprecated (still accepted, emit warning):
  --io auto_detect, --io galois_key_load, --io key_persistence
  --io checkpoint, --io high_throughput

Usage:
    python3 02_load_keys.py KEY_DIR [--io {auto,gds,staged,mapped,...}]

    KEY_DIR -- directory containing saved keys (from 01_save_keys.py)

Requires: CUDA GPU, phantom backend
"""

import argparse
import ctypes
import json
import os
import sys

import torch

from ace import fhe
from ace import runtime as _C
from ace.fhe.config import IoConfig, IoPolicy, IoPolicySettings
from ace.fhe.util import gpu_available


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


def _apply_io_config(io_mode: str):
    """Apply IO configuration based on --io argument.

    Uses IoPolicySettings (primary API) for policy-level options,
    and IoConfig (expert override) for direct transport control.
    """
    policy_map = {
        "auto_detect": IoPolicy.AUTO,  # deprecated → auto
        "galois_key_load": IoPolicy.AUTO,  # deprecated → auto
        "key_persistence": IoPolicy.AUTO,  # deprecated → auto
        "checkpoint": IoPolicy.STAGED,  # deprecated → staged
        "high_throughput": IoPolicy.AUTO,  # deprecated → auto
        # New names (primary)
        "auto": IoPolicy.AUTO,
        "gds": IoPolicy.GDS,
        "staged": IoPolicy.STAGED,
        "mapped": IoPolicy.MAPPED,
    }

    if io_mode in policy_map:
        IoPolicySettings(policy=policy_map[io_mode]).apply()
    else:
        raise ValueError(f"Unknown IO mode: {io_mode}")

    gds_status = "available" if _C.gds_available() else "not available"
    print(f"  IO config:    {io_mode} (GDS {gds_status})")


def main():
    io_choices = [
        # Primary (recommended)
        "auto",
        "gds",
        "staged",
        "mapped",
        # Deprecated (still accepted)
        "auto_detect",
        "galois_key_load",
        "key_persistence",
        "checkpoint",
        "high_throughput",
    ]
    parser = argparse.ArgumentParser(description="KMS Step 2: Load FHE keys and run inference")
    parser.add_argument("key_dir", help="Directory with saved keys (from 01_save_keys.py)")
    parser.add_argument("--io", choices=io_choices, default="auto", help="IO policy (default: auto)")
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
        print("  Did you run 01_save_keys.py first?")
        sys.exit(1)

    with open(meta_path) as f:
        meta = json.load(f)

    kernel_path = meta["kernel"]
    config_path = meta.get("config_path", "")

    print("=" * 60)
    print("KMS Step 2: Load Pre-Generated Keys and Run Inference")
    print("=" * 60)
    print(f"  Key dir:   {key_dir}")
    print(f"  Kernel:    {kernel_path}")
    print(f"  Config:    {config_path}")

    # Apply IO configuration
    _apply_io_config(args.io)
    print()

    # -- Set key_path BEFORE creating FHERuntime ------------------
    print("[1/3] Setting key_path ...")
    _C.set_key_path(key_dir)
    print(f"      key_path = {_C.get_key_path()}")

    # -- Create runtime and run inference -------------------------
    print("[2/3] Creating FHERuntime (keys will be loaded from disk) ...")

    package = {
        "model": "Add",
        "kernel": kernel_path,
        "config_path": config_path,
        "key_path": key_dir,
    }
    runner = fhe.FHERuntime(package)

    x = torch.randn(1, 4, dtype=torch.float32)
    y = torch.randn(1, 4, dtype=torch.float32)
    result = runner.run(x, y)
    print(f"      Result: {result.flatten()[:4].tolist()}")

    runner.finalize()

    # -- Verify keys were loaded (not regenerated) ---------------
    print("[3/3] Verifying keys were loaded from disk ...")

    is_loaded = runner.keys_loaded_from_kms()

    if is_loaded == 1:
        print("      Keys loaded from KMS: YES")
    else:
        print("      Keys loaded from KMS: NO (keys were regenerated!)")

    print()
    print("=" * 60)
    if is_loaded == 1:
        print("Success! Pre-generated keys loaded and inference completed.")
    else:
        print("WARNING: Keys were NOT loaded from disk -- they were regenerated.")
        print("  Check that key files exist in the key directory.")
    print("=" * 60)

    if is_loaded != 1:
        sys.exit(1)


if __name__ == "__main__":
    main()
