#!/usr/bin/env python3
"""IO Read-Performance Benchmark — 3 DataPath modes.

Measures READ throughput across host_staged / host_mapped / gds.
Write is done once per config as data preparation only (always staged).

Matrix (T4, 10 GiB/disk default):
  DataPath:  host_staged, host_mapped, gds
  Disk:      D1 (single), D4 (4 disks)
  FileMode:  single (1 file/disk), multi (4 files/disk)
  → 3 x 2 x 2 = 12 configs

Each config: multiple read repeats → report median.

Usage (in docker):
  python3 benchmark/io/bench_read.py
  python3 benchmark/io/bench_read.py --data-size 10 --repeat 3
  python3 benchmark/io/bench_read.py --quick   # 1 GiB, D1 only, 1 repeat
"""

import argparse
import json
import os
import sys
import time


sys.path.insert(0, os.path.join(os.path.dirname(__file__), "..", ".."))

from ace import runtime as _C
from ace.fhe.config import IoConfig, IoPolicySettings


GiB = 1024**3
DISK_PATHS = ["/mnt/nvme0", "/mnt/nvme1", "/mnt/nvme2", "/mnt/nvme3"]

DP_CONFIG = {
    "host_staged": {"policy": "staged"},
    "host_mapped": {"policy": "mapped"},
    "gds": {"policy": "gds"},
}


def apply_config(data_path: str, threads: int, disk_list: list, chunk_size: int = 0):
    """Apply IO config for the given DataPath."""
    cfg = DP_CONFIG[data_path]
    IoPolicySettings(policy=cfg["policy"], disk_paths=disk_list).apply()
    io_cfg = IoConfig(threads=threads, disk_paths=disk_list)
    if chunk_size > 0:
        io_cfg.chunk_size = chunk_size
    io_cfg.apply()


def run_read_bench(
    data_path: str, n_disks: int, file_mode: str, data_size: int, repeat: int, threads: int = 4, chunk_size: int = 0
):
    """Run one config: write-prep (staged) → read N times → cleanup."""
    disk_list = DISK_PATHS[:n_disks]
    num_files = 1 if file_mode == "single" else max(4, threads)
    bench_path = f"readbench_{data_path}_T{threads}_D{n_disks}_{file_mode}"

    # Step 1: Write data using staged (always) — data preparation only
    w = _C.bench_io_write_prep(bench_path, data_size, num_files, disk_list)

    # Step 2: Switch to target DataPath, read N times
    apply_config(data_path, threads, disk_list, chunk_size)

    reads = []
    transport = None
    verified = None

    for i in range(repeat):
        r = _C.bench_io_read(bench_path, data_size, num_files, disk_list)
        reads.append(r["read_gib_s"])
        if i == 0:
            transport = r["transport_name"]
            verified = r["verified"]

    # Step 3: Cleanup
    _C.bench_io_cleanup(bench_path, num_files, disk_list)

    reads_sorted = sorted(reads)
    return {
        "data_path": data_path,
        "threads": threads,
        "disks": n_disks,
        "file_mode": file_mode,
        "data_size_gib": data_size // GiB,
        "num_files": num_files,
        "write_gib_s": round(w["write_gib_s"], 2),
        "read_median_gib_s": round(reads_sorted[len(reads_sorted) // 2], 2),
        "read_min_gib_s": round(reads_sorted[0], 2),
        "read_max_gib_s": round(reads_sorted[-1], 2),
        "read_all_gib_s": [round(r, 2) for r in reads],
        "transport_name": transport,
        "verified": verified,
    }


def main():
    parser = argparse.ArgumentParser(description="IO read-performance benchmark")
    parser.add_argument("--data-size", type=int, default=10, help="Data size per disk in GiB (default: 10)")
    parser.add_argument("--repeat", type=int, default=3, help="Number of read repetitions per config (default: 3)")
    parser.add_argument("--quick", action="store_true", help="Quick mode: 1 GiB, 1 repeat, D1 only")
    parser.add_argument("--only-gds", action="store_true", help="Only run GDS configs")
    parser.add_argument("--chunk-size", type=int, default=0, help="Chunk size in bytes (0=policy default 512KB)")
    args = parser.parse_args()

    data_size = args.data_size * GiB
    repeat = 1 if args.quick else args.repeat

    dp_list = ["gds"] if args.only_gds else ["host_staged", "host_mapped", "gds"]

    if args.quick:
        configs = [(dp, 4, 1, fm) for dp in dp_list for fm in ["single", "multi"]]
    else:
        configs = [(dp, 4, n_disks, fm) for dp in dp_list for n_disks in [1, 4] for fm in ["single", "multi"]]

    total = len(configs)
    print(f"Read Benchmark: {total} configs x {repeat} reads, {args.data_size} GiB/disk")
    print(
        f"{'Config':<35} {'Write':>8} {'Read(med)':>10} {'Read(min)':>10} {'Read(max)':>10} {'Transport':<16} {'OK':>3}"
    )
    print("-" * 100)

    results = []
    t0 = time.perf_counter()

    for idx, (dp, t, nd, fm) in enumerate(configs, 1):
        label = f"{dp}/T{t}/D{nd}/{fm}/{args.data_size}G"
        print(f"[{idx}/{total}] {label:<35}", end="  ", flush=True)
        try:
            r = run_read_bench(dp, nd, fm, data_size, repeat, t, args.chunk_size)
            ok = "✓" if r["verified"] else "✗"
            print(
                f"{r['write_gib_s']:>7.2f}  {r['read_median_gib_s']:>9.2f}  "
                f"{r['read_min_gib_s']:>9.2f}  {r['read_max_gib_s']:>9.2f}  "
                f"{r['transport_name']:<16} {ok}"
            )
            results.append(r)
        except Exception as e:
            print(f"ERROR: {e}")

    elapsed = time.perf_counter() - t0

    # Summary table
    print("\n" + "=" * 100)
    print("READ Performance Summary (GiB/s)")
    print("=" * 100)
    print(
        f"{'DataPath':<15} {'T':>3} {'D':>3} {'Mode':<8} {'Size':>5} "
        f"{'Read(med)':>10} {'Read(min)':>10} {'Read(max)':>10} {'Transport':<16}"
    )
    print("-" * 100)
    for r in results:
        print(
            f"{r['data_path']:<15} {r['threads']:>3} {r['disks']:>3} {r['file_mode']:<8} "
            f"{r['data_size_gib']:>5}  {r['read_median_gib_s']:>9.2f}  "
            f"{r['read_min_gib_s']:>9.2f}  {r['read_max_gib_s']:>9.2f}  "
            f"{r['transport_name']:<16}"
        )

    # Save JSON
    out_dir = DISK_PATHS[0] + "/io_bench_results"
    os.makedirs(out_dir, exist_ok=True)
    out_file = os.path.join(out_dir, f"read_bench_{args.data_size}g_{time.strftime('%Y%m%d_%H%M')}.json")
    with open(out_file, "w") as f:
        json.dump(
            {
                "meta": {
                    "data_size_per_disk_gib": args.data_size,
                    "repeat": repeat,
                    "gpu": "A100 80GB",
                    "nvme": "4x local NVMe",
                    "timestamp": time.strftime("%Y-%m-%d %H:%M"),
                    "elapsed_sec": round(elapsed, 1),
                },
                "results": results,
            },
            f,
            indent=2,
        )
    print(f"\nResults saved: {out_file}")
    print(f"Total time: {elapsed:.0f}s")

    return results


if __name__ == "__main__":
    main()
