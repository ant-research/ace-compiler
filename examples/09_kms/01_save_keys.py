#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
KMS Step 1: Generate FHE keys and save to disk

FHE key generation is expensive (especially for large CKKS parameters).
This script compiles a model, runs inference to generate keys, then saves
them to disk so they can be reused later.

IO policy is configurable via --io:
  --io auto        Auto-detect GDS (default)
  --io gds         Force GDS (fail if cuFile unavailable)
  --io staged      Force host-staged I/O (no GDS)
  --io mapped      Force host-mapped I/O (mmap + cudaMemcpy)

Deprecated (still accepted, emit warning):
  --io auto_detect, --io galois_key_load, --io key_persistence
  --io checkpoint, --io high_throughput

Usage:
    python3 01_save_keys.py [KEY_DIR] [--io {auto,gds,staged,mapped,...}]

    KEY_DIR  -- output directory for saved keys (default: ./kms_demo_keys)

Output:
    KEY_DIR/global/secret_key.bin    -- secret key
    KEY_DIR/global/public_key.bin    -- public key
    KEY_DIR/model/relin_key.bin      -- relinearization key
    KEY_DIR/model/galois_key.*       -- Galois (rotation) keys (per-key format)
    KEY_DIR/model/galois_key.index.json -- galois key index
    KEY_DIR/meta.json                -- metadata for the load step

Next step:
    python3 02_load_keys.py KEY_DIR

Requires: CUDA GPU, phantom backend
"""

import argparse
import ctypes
import glob
import json
import os
import sys

import torch

from ace import fhe
from ace import runtime as _C
from ace.fhe.config import IoConfig, IoPolicy, IoPolicySettings
from ace.fhe.util import gpu_available


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


def _apply_io_config(io_mode: str):
    """Apply IO configuration based on --io argument.

    Uses IoPolicySettings (primary API) for policy-level options,
    and IoConfig (expert override) for direct transport control.
    """
    # Policy-level options (recommended)
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
    parser = argparse.ArgumentParser(description="KMS Step 1: Save FHE keys")
    parser.add_argument("key_dir", nargs="?", default="./kms_demo_keys", help="Output directory for saved keys")
    parser.add_argument("--io", choices=io_choices, default="auto", help="IO policy (default: auto)")
    args = parser.parse_args()

    key_dir = os.path.abspath(args.key_dir)

    if not gpu_available():
        print("ERROR: GPU not available. This example requires CUDA.")
        sys.exit(1)

    print("=" * 60)
    print("KMS Step 1: Generate and Save FHE Keys")
    print("=" * 60)
    print(f"  Key output dir: {key_dir}")

    # Apply IO configuration
    _apply_io_config(args.io)
    print()

    # -- Compile model --------------------------------------------
    print("[1/4] Compiling model ...")

    @fhe.compile(frontend="torch", library="phantom", device="cuda", encrypt_inputs=["x", "y"])
    def add(x, y):
        return x + y

    x = torch.randn(1, 4, dtype=torch.float32)
    y = torch.randn(1, 4, dtype=torch.float32)
    prog = add.compile([x, y])
    print("      Done.")

    # -- Run inference (triggers key generation) ------------------
    print("[2/4] Running inference (keys will be generated) ...")
    _C.set_key_path("")  # ensure no pre-set key_path

    runner = fhe.FHERuntime(prog.package)
    result = runner.run(x, y)
    expected = x + y

    print(f"      Result:   {result.flatten()[:4].tolist()}")
    print(f"      Expected: {expected.flatten()[:4].tolist()}")

    # -- Save keys to disk ----------------------------------------
    print(f"[3/4] Saving keys to {key_dir} ...")

    rc = runner.save_keys(key_dir)
    if rc != 0:
        print(f"  ERROR: save_keys failed with rc={rc}")
        sys.exit(1)

    runner.finalize()

    # -- Verify ---------------------------------------------------
    print("[4/4] Verifying saved key files ...")
    key_files = [
        ("global/secret_key.bin", "Secret key"),
        ("global/public_key.bin", "Public key"),
        ("model/relin_key.bin", "Relin key"),
    ]
    total = 0
    for rel_path, label in key_files:
        fpath = os.path.join(key_dir, rel_path)
        if not os.path.exists(fpath):
            print(f"  ERROR: Missing {fpath}")
            sys.exit(1)
        size = os.path.getsize(fpath)
        total += size
        print(f"      {label:12s}  {size:>10,d} bytes  ({size / 1024 / 1024:.1f} MB)")

    # Galois keys: per-key format (galois_key.NN.bin) or legacy merged format
    galois_bins = glob.glob(os.path.join(key_dir, "model/galois_key.*.bin"))
    galois_merged = os.path.join(key_dir, "model/galois_key.bin")
    if galois_bins:
        total_galois = sum(os.path.getsize(f) for f in galois_bins)
        total += total_galois
        print(
            f"      {'Galois keys':12s}  {total_galois:>10,d} bytes  "
            f"({total_galois / 1024 / 1024:.1f} MB, {len(galois_bins)} files)"
        )
    elif os.path.exists(galois_merged):
        size = os.path.getsize(galois_merged)
        total += size
        print(f"      {'Galois key':12s}  {size:>10,d} bytes  ({size / 1024 / 1024:.1f} MB)")
    else:
        print(f"  ERROR: No galois key files found in {key_dir}/model/")
        sys.exit(1)

    print(f"      {'Total':12s}  {total:>10,d} bytes  ({total / 1024 / 1024:.1f} MB)")

    # Write metadata for the load step
    meta = {
        "key_dir": key_dir,
        "kernel": prog.package["kernel"],
        "config_path": prog.package.get("config_path", ""),
        "io_mode": args.io,
    }
    meta_path = os.path.join(key_dir, "meta.json")
    with open(meta_path, "w") as f:
        json.dump(meta, f, indent=2)
    print(f"      Metadata:  {meta_path}")

    print()
    print("=" * 60)
    print("Keys saved successfully!")
    print()
    print("To load these keys in a new process, run:")
    print(f"  python3 02_load_keys.py {key_dir}")
    print("=" * 60)


if __name__ == "__main__":
    main()
