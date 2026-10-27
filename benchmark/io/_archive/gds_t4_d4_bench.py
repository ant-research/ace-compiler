#!/usr/bin/env python3
"""GDS T4/D4 focused benchmark — 10GiB per disk, single & multi file."""
import sys, os
sys.path.insert(0, "/work/ace-fhe/ace-compiler")

from ace import runtime as _C
from ace.fhe.config import IoPolicySettings, IoConfig

GiB = 1024 ** 3
DISKS = ["/mnt/nvme0", "/mnt/nvme1", "/mnt/nvme2", "/mnt/nvme3"]

# Setup
for d in DISKS:
    os.makedirs(f"{d}/io_bench", exist_ok=True)

IoPolicySettings(policy="gds", disk_paths=DISKS).apply()
IoConfig(threads=4, disk_paths=DISKS).apply()

data_size_per_disk = 10 * GiB  # 10 GiB per disk = 40 GiB total

# === Test 1: Single file per disk (4 files, 10GiB each) ===
print("=" * 60)
print(f"GDS T4/D4 — single file — 10GiB/disk (40GiB total)")
print("=" * 60)

# Write
write_buf = _C.bench_io_roundtrip("/mnt/nvme0/io_bench/_dummy", 1, 1, DISKS)  # warmup
import time

# Manual per-disk write
file_paths = [f"{d}/io_bench/gds_t4d4_single_disk{i}" for i, d in enumerate(DISKS)]
t0 = time.perf_counter()
for fp in file_paths:
    w = _C.bench_io_roundtrip(fp, 1, 1, [fp.split("/io_bench")[0]])
    # This writes 1 byte — we need the actual write below
# Actually use the proper per-disk write

# Direct approach: just call bench_io_roundtrip with the right data_size
t0 = time.perf_counter()
r = _C.bench_io_roundtrip("gds_t4d4_single", data_size_per_disk, 1, DISKS)
t1 = time.perf_counter()
print(f"  write: {r['write_gib_s']:.2f} GiB/s ({r['write_ms']:.0f}ms)")
print(f"  read:  {r['read_gib_s']:.2f} GiB/s ({r['read_ms']:.0f}ms)")
print(f"  transport: {r['transport_name']}  verified: {'✓' if r['verified'] else '✗'}")
print(f"  total_data: {r['data_size_gib']:.1f} GiB  disks: {r['num_disks']}")

# === Test 2: Multi file per disk (4 files/disk, 2.5GiB each = 10GiB/disk) ===
print()
print("=" * 60)
print(f"GDS T4/D4 — multi file (4/disk) — 10GiB/disk (40GiB total)")
print("=" * 60)

r = _C.bench_io_roundtrip("gds_t4d4_multi", data_size_per_disk, 4, DISKS)
print(f"  write: {r['write_gib_s']:.2f} GiB/s ({r['write_ms']:.0f}ms)")
print(f"  read:  {r['read_gib_s']:.2f} GiB/s ({r['read_ms']:.0f}ms)")
print(f"  transport: {r['transport_name']}  verified: {'✓' if r['verified'] else '✗'}")
print(f"  total_data: {r['data_size_gib']:.1f} GiB  disks: {r['num_disks']}")

# === Test 3: Single disk baseline (GDS T4/D1) ===
print()
print("=" * 60)
print(f"GDS T4/D1 — single file — 10GiB (single disk baseline)")
print("=" * 60)

IoPolicySettings(policy="gds", disk_paths=["/mnt/nvme0"]).apply()
IoConfig(threads=4, disk_paths=["/mnt/nvme0"]).apply()

r = _C.bench_io_roundtrip("gds_t4d1_single", 10 * GiB, 1, ["/mnt/nvme0"])
print(f"  write: {r['write_gib_s']:.2f} GiB/s ({r['write_ms']:.0f}ms)")
print(f"  read:  {r['read_gib_s']:.2f} GiB/s ({r['read_ms']:.0f}ms)")
print(f"  transport: {r['transport_name']}  verified: {'✓' if r['verified'] else '✗'}")
