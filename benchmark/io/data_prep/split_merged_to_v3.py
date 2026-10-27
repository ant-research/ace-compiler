#!/usr/bin/env python3
"""Split existing v2 merged galois key files into v3 multi-file format.

v2: 1 merged file per disk → poor GDS throughput (single NVMe queue per file)
v3: N sub-files per disk → N independent NVMe submission queues → ~Nx bandwidth

This script:
  1. Reads the v2 galois_key.index.json
  2. Splits each disk's merged file into N (default 4) sub-files
  3. Rewrites the index as version 3 with per-entry file_slot

Usage (in docker):
  # D1 keys
  python3 benchmark/io/split_merged_to_v3.py /mnt/nvme0/kms_demo_keys_resnet20_gds

  # D4 keys
  python3 benchmark/io/split_merged_to_v3.py /mnt/nvme0/kms_demo_keys_resnet20_4disk

  # Custom N
  python3 benchmark/io/split_merged_to_v3.py /mnt/nvme0/kms_demo_keys_resnet20_4disk --slots 4
"""

import argparse
import json
import os
import shutil
import sys


GiB = 1024**3

SLOT_KEY = "slot"  # JSON key name must match C++ read code: ej.value("slot", 0)


def main():
    parser = argparse.ArgumentParser(description="Split v2 merged → v3 multi-file")
    parser.add_argument("key_dir", help="Key directory with model/galois_key.index.json")
    parser.add_argument("--slots", type=int, default=4, help="Number of sub-files per disk (default: 4)")
    args = parser.parse_args()

    key_dir = os.path.abspath(args.key_dir)
    model_dir = os.path.join(key_dir, "model")
    idx_path = os.path.join(model_dir, "galois_key.index.json")

    if not os.path.exists(idx_path):
        print(f"ERROR: {idx_path} not found")
        sys.exit(1)

    with open(idx_path) as f:
        idx = json.load(f)

    version = idx.get("version", 1)
    if version >= 3:
        print(f"Already v3 format (version={version}), nothing to do")
        sys.exit(0)

    if version < 2:
        print(f"ERROR: Expected v2 merged format, got version={version}")
        sys.exit(1)

    disk_paths = idx.get("disk_paths", [])
    relin_keys = idx.get("relin_keys", [])
    num_slots = args.slots

    if not disk_paths:
        print("ERROR: no disk_paths in index.json")
        sys.exit(1)

    # ── Step 1: Group entries by disk, compute sub-file boundaries ──

    # For each disk, collect entries in order, then split into num_slots groups
    disk_entries = {}  # disk_index -> list of (entry_idx, entry)
    for i, entry in enumerate(relin_keys):
        d = entry.get("disk", entry.get("disk_index", 0))
        disk_entries.setdefault(d, []).append((i, entry))

    print(f"Key dir: {key_dir}")
    print(f"Version: {version} → 3")
    print(f"Slots per disk: {num_slots}")
    print(f"Disks: {len(disk_paths)}")
    print(f"Relin keys: {len(relin_keys)}")

    total_key_size = sum(e.get("size", 0) for e in relin_keys)
    print(f"Total galois key size: {total_key_size / GiB:.2f} GiB")
    print()

    # Backup original index
    bak_path = idx_path + ".v2.bak"
    if not os.path.exists(bak_path):
        shutil.copy2(idx_path, bak_path)
        print(f"Backed up v2 index → {bak_path}")

    # ── Step 2: For each disk, split the merged file into num_slots sub-files ──

    new_entries = [None] * len(relin_keys)  # will be populated in order
    split_files = set()

    for d, entries in sorted(disk_entries.items()):
        disk_dir = disk_paths[d]
        src_file = entries[0][1].get("file", "")
        src_path = os.path.join(disk_dir, src_file)

        if not os.path.exists(src_path):
            print(f"  ERROR: {src_path} not found")
            sys.exit(1)

        disk_total = sum(e.get("size", 0) for _, e in entries)
        n_entries = len(entries)
        print(f"\nDisk {d}: {src_file} ({disk_total / GiB:.2f} GiB, {n_entries} keys)")

        # Split entries into num_slots groups (roughly equal count)
        per_slot = (n_entries + num_slots - 1) // num_slots

        for slot in range(num_slots):
            start = slot * per_slot
            end = min(start + per_slot, n_entries)
            slot_entries = entries[start:end]

            if not slot_entries:
                continue

            # Compute byte range for this slot
            first_offset = slot_entries[0][1].get("offset", 0)
            last_entry = slot_entries[-1][1]
            read_start = first_offset
            read_end = last_entry.get("offset", 0) + last_entry.get("size", 0)
            read_size = read_end - read_start

            # New sub-file name: galois_key.merged.D.S.bin
            new_name = f"galois_key.merged.{d}.{slot}.bin"
            new_path = os.path.join(disk_dir, new_name)

            print(f"  Slot {slot}: {len(slot_entries)} keys, {read_size / GiB:.2f} GiB → {new_name}")

            # Split the file: read [read_start, read_end) from src, write to dst
            with open(src_path, "rb") as fin:
                fin.seek(read_start)
                remaining = read_size
                with open(new_path, "wb") as fout:
                    while remaining > 0:
                        chunk = fin.read(min(256 * 1024 * 1024, remaining))
                        if not chunk:
                            break
                        fout.write(chunk)
                        remaining -= len(chunk)

            # Update entries: set file=new_name, offset=relative, slot=slot
            cursor = 0
            for entry_idx, entry in slot_entries:
                entry_size = entry.get("size", 0)
                entry_nk = entry.get("nk", 2)
                # offset relative to sub-file start
                rel_offset = entry.get("offset", 0) - read_start

                new_entries[entry_idx] = {
                    "file": new_name,
                    "disk": d,
                    SLOT_KEY: slot,
                    "offset": rel_offset,
                    "size": entry_size,
                    "nk": entry_nk,
                    "crc32": "0x0",
                }
                cursor += entry_size

            split_files.add(new_path)

    # ── Step 3: Write new index.json ──

    new_idx = {
        "version": 3,
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

    print(f"\nUpdated index.json → version=3, {len(new_entries)} entries, {len(split_files)} sub-files")
    print("Old v2 merged files kept on disk (not deleted)")
    print("Split done ✓")


if __name__ == "__main__":
    main()
