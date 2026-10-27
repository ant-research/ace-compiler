#!/usr/bin/env python3
"""Merge per-key galois key files into merged format (one file per disk).

Reads an existing galois_key.index.json (version=1, per-key format),
concatenates all .bin files belonging to each disk into a single
galois_key.merged.{disk}.bin, and rewrites the index as version=2.

This eliminates per-key cuFile handle overhead for GDS reads — instead
of 67 file open/close cycles, only 4 persistent handles are needed.

Usage:
  # Merge 1-disk keys (all on nvme0)
  python3 benchmark/io/merge_galois_keys.py /mnt/nvme0/kms_demo_keys_resnet20_gds

  # Merge 4-disk keys (distributed across 4 NVMe)
  python3 benchmark/io/merge_galois_keys.py /mnt/nvme0/kms_demo_keys_resnet20_4disk

  # Dry run (show what would happen, don't write)
  python3 benchmark/io/merge_galois_keys.py --dry-run /mnt/nvme0/kms_demo_keys_resnet20_4disk
"""

import argparse
import json
import os
import shutil
import sys


GiB = 1024**3


def main():
    parser = argparse.ArgumentParser(description="Merge per-key galois files into merged format")
    parser.add_argument("key_dir", help="Key directory containing model/galois_key.index.json")
    parser.add_argument("--dry-run", action="store_true", help="Show plan without writing")
    args = parser.parse_args()

    key_dir = os.path.abspath(args.key_dir)
    model_dir = os.path.join(key_dir, "model")
    idx_path = os.path.join(model_dir, "galois_key.index.json")

    if not os.path.exists(idx_path):
        print(f"ERROR: {idx_path} not found")
        sys.exit(1)

    with open(idx_path) as f:
        idx = json.load(f)

    if idx.get("version", 1) >= 2:
        print(f"Already merged format (version={idx['version']}), nothing to do")
        sys.exit(0)

    disk_paths = idx.get("disk_paths", [])
    relin_keys = idx.get("relin_keys", [])

    if not disk_paths:
        print("ERROR: no disk_paths in index.json")
        sys.exit(1)

    num_disks = len(disk_paths)
    print(f"Key dir: {key_dir}")
    print(f"Disk paths: {num_disks} disks")
    for i, dp in enumerate(disk_paths):
        print(f"  disk[{i}]: {dp}")
    print(f"Relin keys: {len(relin_keys)} entries")
    print(f"Version: {idx.get('version', 1)} → 2")
    print()

    # Group entries by disk_index
    groups = {}
    for entry in relin_keys:
        d = entry.get("disk", entry.get("disk_index", 0))
        groups.setdefault(d, []).append(entry)

    for d, entries in sorted(groups.items()):
        total = sum(e.get("size", 0) for e in entries)
        print(f"  Disk {d}: {len(entries)} files, {total / GiB:.2f} GiB → galois_key.merged.{d}.bin")

    if args.dry_run:
        print("\n[DRY RUN] No files written")
        return

    # Backup original index
    bak_path = idx_path + ".v1.bak"
    if not os.path.exists(bak_path):
        shutil.copy2(idx_path, bak_path)
        print(f"\nBacked up original index → {bak_path}")

    # Merge: for each disk, cat all per-key .bin files into one merged file
    merged_filenames = {}
    new_entries = []

    for d, entries in sorted(groups.items()):
        disk_dir = disk_paths[d]
        merged_name = f"galois_key.merged.{d}.bin"
        merged_path = os.path.join(disk_dir, merged_name)
        merged_filenames[d] = merged_name

        print(f"\nMerging {len(entries)} files → {merged_path} ...")

        offset = 0
        with open(merged_path, "wb") as out_f:
            for entry in entries:
                fname = entry.get("file", "")
                src = os.path.join(disk_dir, fname)
                if not os.path.exists(src):
                    print(f"  WARNING: {src} not found, skipping")
                    continue

                entry_size = entry.get("size", 0)
                entry_nk = entry.get("nk", 2)

                new_entries.append(
                    {
                        "file": merged_name,
                        "disk": d,
                        "offset": offset,
                        "size": entry_size,
                        "nk": entry_nk,
                        "crc32": "0x0",
                    }
                )

                # Copy data
                with open(src, "rb") as in_f:
                    while True:
                        chunk = in_f.read(256 * 1024 * 1024)  # 256 MB chunks
                        if not chunk:
                            break
                        out_f.write(chunk)

                offset += entry_size

        print(f"  Written: {offset / GiB:.2f} GiB ({len(entries)} source files, kept)")

    # Write new index.json
    new_idx = {
        "version": 2,
        "poly_modulus_degree": idx.get("poly_modulus_degree"),
        "coeff_modulus_size": idx.get("coeff_modulus_size"),
        "parms_id": idx.get("parms_id"),
        "galois_elts": idx.get("galois_elts", []),
        "key_component_bytes": idx.get("key_component_bytes"),
        "disk_paths": disk_paths,
        "relin_keys": new_entries,
    }

    with open(idx_path, "w") as f:
        json.dump(new_idx, f, indent=2)

    print(f"\nUpdated index.json → version=2, {len(new_entries)} entries")
    print("Done ✓")


if __name__ == "__main__":
    main()
