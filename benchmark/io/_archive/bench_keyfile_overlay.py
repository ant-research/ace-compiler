#!/usr/bin/env python3
"""Bench bench_io_read on real key file data.

Steps:
  1. bench_io_write_prep: create file structure with correct naming
  2. Overwrite each bench file with key sub-file content (dd)
  3. bench_io_read: measure read throughput on key data
  4. Compare with key load throughput
"""
import os
import subprocess
import sys

sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), '..', '..'))

from ace import runtime as _C
from ace.fhe.config import IoConfig, IoPolicySettings

GiB = 1024 ** 3
DISKS = ["/mnt/nvme0", "/mnt/nvme1", "/mnt/nvme2", "/mnt/nvme3"]

IoPolicySettings(policy="gds", disk_paths=DISKS).apply()
IoConfig(threads=4, disk_paths=DISKS).apply()

# ── Step 1: Create bench file structure with bench_io_write_prep ──
# We want 4 files per disk. With n_disks=4 and round-robin,
# num_files=16 gives 4 files/disk.
# data_size per disk must match key file sizes. Use a size ≤ smallest
# key sub-file so bench doesn't read past the key data we'll overlay.

# Key sub-file sizes per disk:
key_dir = "/mnt/nvme0/kms_demo_keys_resnet20_4disk"
key_files_per_disk = {}
for d in range(4):
    disk_dir = f"/mnt/nvme{d}/kms_4d/model"
    files = sorted([
        f for f in os.listdir(disk_dir)
        if f.startswith(f"galois_key.merged.{d}.") and f.endswith(".bin")
        and len(f.split(".")) == 5
    ])
    key_files_per_disk[d] = [
        (os.path.join(disk_dir, f), os.path.getsize(os.path.join(disk_dir, f)))
        for f in files
    ]

# Smallest key sub-file across all disks
min_key_file = min(sz for flist in key_files_per_disk.values() for _, sz in flist)
# data_size per file = min_key_file (so each bench file fits within key overlay)
# per_disk = min_key_file * 4
data_size_per_disk = min_key_file * 4

print(f"Smallest key sub-file: {min_key_file / GiB:.3f} GiB")
print(f"Data size per disk: {data_size_per_disk / GiB:.3f} GiB")
print(f"Per-file read size: {min_key_file / GiB:.3f} GiB")
print()

# Write bench files (random data, just for path/buffer setup)
print("Step 1: bench_io_write_prep ...")
_ = _C.bench_io_write_prep("keydata", data_size_per_disk, 16, DISKS)
print("  done")

# ── Step 2: Overwrite bench files with key sub-file data ──
# bench naming: keydata_<i> on disk (i % 4)
# Files 0,4,8,12 → disk 0; 1,5,9,13 → disk 1; etc.
# We map: slot 0 on disk D = bench file (D + 0*4) = keydata_D
#         slot 1 on disk D = bench file (D + 1*4) = keydata_{D+4}
#         slot 2 on disk D = bench file (D + 2*4) = keydata_{D+8}
#         slot 3 on disk D = bench file (D + 3*4) = keydata_{D+12}

print("Step 2: Overwrite bench files with key data (dd) ...")
for d in range(4):
    for slot in range(4):
        bench_idx = d + slot * 4
        bench_path = f"/mnt/nvme{d}/io_bench/keydata_{bench_idx}"
        if slot < len(key_files_per_disk[d]):
            key_path, key_size = key_files_per_disk[d][slot]
            # dd: copy key_size bytes from key file to bench file
            result = subprocess.run(
                ["dd", f"if={key_path}", f"of={bench_path}",
                 "bs=1M", f"count={key_size // (1024*1024)}",
                 "conv=notrunc"],
                capture_output=True, text=True
            )
            actual = os.path.getsize(bench_path)
            print(f"  disk{d} slot{slot}: {key_path} → {bench_path} "
                  f"({actual / GiB:.3f} GiB)")
        else:
            print(f"  disk{d} slot{slot}: no key file, skip")

print("  done")
print()

# ── Step 3: bench_io_read on key data ──
print("Step 3: bench_io_read on key data ...")
for trial in range(3):
    r = _C.bench_io_read("keydata", data_size_per_disk, 16, DISKS)
    print(f"  trial {trial}: {r['read_gib_s']:.2f} GiB/s  "
          f"transport={r['transport_name']}  verified={r['verified']}")

# ── Step 4: Compare with key load ──
print()
print("Comparison:")
print(f"  bench_io_read (key data):  see above")
print(f"  key load (load_galois_key_merged): ~17 GiB/s")
print(f"  bench_io_read (synthetic): ~52 GiB/s")

# Cleanup
_C.bench_io_cleanup("keydata", 16, DISKS)
