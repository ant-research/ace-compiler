#!/usr/bin/env python3
"""Re-save existing keys in v3 multi-file merged format.

Load keys from existing key_dir, then re-save galois keys in v3 format
(N sub-files per disk for maximum GDS throughput).
"""

import argparse
import ctypes
import json
import os
import sys
import time


sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))


def main():
    parser = argparse.ArgumentParser(description="Re-save keys in v3 multi-file merged format")
    parser.add_argument("key_dir", help="Source key directory")
    parser.add_argument(
        "--disk-paths",
        nargs="+",
        default=["/mnt/nvme0", "/mnt/nvme1", "/mnt/nvme2", "/mnt/nvme3"],
        help="Disk paths for multi-disk save",
    )
    args = parser.parse_args()

    key_dir = os.path.abspath(args.key_dir)

    from ace import runtime as _C
    from ace.fhe.config import IoConfig, IoPolicySettings

    # Set gds + high_throughput for the save
    IoPolicySettings(policy="gds", disk_paths=args.disk_paths).apply()
    IoConfig(threads=4, disk_paths=args.disk_paths).apply()

    # Load keys from existing dir (any format)
    _C.set_key_path(key_dir)

    from ace import fhe

    with open(os.path.join(key_dir, "meta.json")) as f:
        meta = json.load(f)

    package = {
        "model": meta.get("model_key", meta.get("model", "resnet20_cifar10")),
        "kernel": meta["kernel"],
        "config_path": meta.get("config_path", ""),
        "key_path": key_dir,
    }

    print(f"Loading keys from {key_dir} ...")
    runner = fhe.FHERuntime(package)
    # Re-save via the runner KMS surface (load_keys first to bring up a
    # FheContext via the PRELOAD path, then re-save in the new v3 format).
    print(f"Re-saving keys to {key_dir} in v3 format ...")

    runner.load_keys(key_dir)
    rc = runner.save_keys(key_dir)
    if rc != 0:
        print(f"ERROR: save_keys failed with rc={rc}")
        sys.exit(1)

    runner.finalize()
    _C.clear_key_path()

    # Verify index.json is v3
    idx_path = os.path.join(key_dir, "model", "galois_key.index.json")
    with open(idx_path) as f:
        idx = json.load(f)
    print(f"Index version: {idx.get('version')}")
    print(f"Sub-files per disk: {len(set(e['file'] for e in idx['relin_keys']))}")

    total = sum(e.get("size", 0) for e in idx["relin_keys"])
    print(f"Total galois key size: {total / (1024**3):.2f} GiB")
    print("Re-save done ✓")


if __name__ == "__main__":
    main()
