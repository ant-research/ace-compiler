#!/usr/bin/env python3
"""IO Performance Baseline Runner

Runs the full 3x3x3x2x2 = 108 combination matrix:
  DataPath: staged, mapped, gds
  Threads:  1, 2, 4
  Disks:    1, 2, 4
  FileMode: single, multi
  DataSize: 10 GiB, 20 GiB

Results are saved as JSON + markdown report.
"""

import json
import os
import sys
import time


sys.path.insert(0, "/work/ace-fhe/ace-compiler")

from ace import runtime as _C
from ace.fhe.config import IoConfig, IoPolicySettings


GiB = 1024**3

DATA_PATHS = ["host_staged", "host_mapped", "gds"]
THREADS = [1, 2, 4]
DISKS = [1, 2, 4]
FILE_MODES = ["single", "multi"]
DATA_SIZES = [1]  # GiB **per disk** — total = data_size x n_disks

DISK_PATHS = ["/mnt/nvme0", "/mnt/nvme1", "/mnt/nvme2", "/mnt/nvme3"]

# Map DataPath → IoPolicy for internal resolution
DP_TO_POLICY = {
    "host_staged": "staged",
    "host_mapped": "mapped",
    "gds": "gds",
}


def available_disks(n):
    """Return first n accessible disk paths."""
    result = []
    for dp in DISK_PATHS:
        if os.path.isdir(dp) and os.access(dp, os.W_OK | os.R_OK):
            result.append(dp)
        if len(result) >= n:
            break
    return result


def run_bench(data_path, threads, n_disks, file_mode, data_size_gib):
    """Run a single benchmark configuration."""
    disk_list = available_disks(n_disks)
    if len(disk_list) < n_disks:
        return None  # skip

    # Apply config: IoPolicySettings for transport path, IoConfig for threads
    IoPolicySettings(
        policy=DP_TO_POLICY[data_path],
        disk_paths=disk_list,
    ).apply()
    IoConfig(
        threads=threads,
        disk_paths=disk_list,
    ).apply()

    # Benchmark parameters
    bench_dir = os.path.join(disk_list[0], "io_bench")
    os.makedirs(bench_dir, exist_ok=True)
    bench_path = os.path.join(bench_dir, f"{data_path}_T{threads}_D{n_disks}_{file_mode}_{data_size_gib}G")
    num_files = 1 if file_mode == "single" else max(4, threads)
    data_size = int(data_size_gib * GiB)

    try:
        result = _C.bench_io_roundtrip(bench_path, data_size, num_files, disk_list)
    except Exception as e:
        return {"error": str(e), "config": f"{data_path}/T{threads}/D{n_disks}/{file_mode}/{data_size_gib}G"}

    return {
        "data_path": data_path,
        "threads": threads,
        "disks": n_disks,
        "file_mode": file_mode,
        "data_size_gib": data_size_gib,
        "write_ms": result["write_ms"],
        "read_ms": result["read_ms"],
        "write_gib_s": result["write_gib_s"],
        "read_gib_s": result["read_gib_s"],
        "transport_name": result["transport_name"],
        "verified": result["verified"],
        "gds_available": result["gds_available"],
    }


def generate_report(results, output_dir):
    """Generate JSON + markdown report from results."""
    os.makedirs(output_dir, exist_ok=True)

    # JSON
    json_path = os.path.join(output_dir, "io_baseline_results.json")
    with open(json_path, "w") as f:
        json.dump(results, f, indent=2)
    print(f"JSON results: {json_path}")

    # Markdown report
    md_path = os.path.join(output_dir, "io_baseline_report.md")
    lines = [
        "# IO Performance Baseline Report\n",
        "- **GPU**: A100 80GB PCIe",
        "- **NVMe**: 4x local NVMe",
        f"- **GDS**: {'available' if results and results[0].get('gds_available') else 'unavailable'}",
        f"- **Total configs**: {len(results)}",
        f"- **Date**: {time.strftime('%Y-%m-%d %H:%M')}",
        "",
    ]

    # Group by DataPath
    for dp in DATA_PATHS:
        dp_results = [r for r in results if r.get("data_path") == dp]
        if not dp_results:
            continue

        lines.append(f"\n## {dp}\n")

        for fm in FILE_MODES:
            fm_results = [r for r in dp_results if r.get("file_mode") == fm]
            if not fm_results:
                continue

            lines.append(f"\n### {fm} file mode\n")
            lines.append("| Threads | Disks | Size(GiB) | Write(GiB/s) | Read(GiB/s) | Transport | Verified |")
            lines.append("|---------|-------|-----------|-------------|------------|-----------|----------|")

            for r in sorted(
                fm_results, key=lambda x: (x.get("threads", 0), x.get("disks", 1), x.get("data_size_gib", 0))
            ):
                lines.append(
                    f"| {r['threads']} "
                    f"| {r['disks']} "
                    f"| {r['data_size_gib']} "
                    f"| {r['write_gib_s']:.2f} "
                    f"| {r['read_gib_s']:.2f} "
                    f"| {r['transport_name']} "
                    f"| {'✓' if r['verified'] else '✗'} |"
                )

    # Summary table: best throughput per DataPath
    lines.append("\n\n## Summary: Best Read Throughput per DataPath\n")
    lines.append("| DataPath | Threads | Disks | Mode | Size | Read(GiB/s) |")
    lines.append("|----------|---------|-------|------|------|------------|")
    for dp in DATA_PATHS:
        dp_results = [r for r in results if r.get("data_path") == dp and r.get("verified")]
        if dp_results:
            best = max(dp_results, key=lambda r: r["read_gib_s"])
            lines.append(
                f"| {dp} | {best['threads']} | {best['disks']} | {best['file_mode']} | "
                f"{best['data_size_gib']}GiB | {best['read_gib_s']:.2f} |"
            )

    with open(md_path, "w") as f:
        f.write("\n".join(lines) + "\n")
    print(f"Markdown report: {md_path}")


def main():
    # Compute total
    total = len(DATA_PATHS) * len(THREADS) * len(DISKS) * len(FILE_MODES) * len(DATA_SIZES)
    print(f"IO Baseline: {total} configurations to run")
    print(f"  DataPaths: {DATA_PATHS}")
    print(f"  Threads: {THREADS}")
    print(f"  Disks: {DISKS}")
    print(f"  FileModes: {FILE_MODES}")
    print(f"  DataSizes: {DATA_SIZES} GiB\n")

    results = []
    skipped = []
    idx = 0

    for dp in DATA_PATHS:
        for t in THREADS:
            for d in DISKS:
                for fm in FILE_MODES:
                    for sz in DATA_SIZES:
                        idx += 1
                        label = f"{dp}/T{t}/D{d}/{fm}/{sz}GiB"
                        print(f"[{idx}/{total}] {label} ... ", end="", flush=True)

                        result = run_bench(dp, t, d, fm, sz)

                        if result is None:
                            print("SKIP (disk unavailable)")
                            skipped.append(label)
                        elif "error" in result:
                            print(f"ERROR: {result['error']}")
                            results.append(result)
                        else:
                            print(
                                f"write={result['write_gib_s']:.2f} read={result['read_gib_s']:.2f} [{result['transport_name']}] {'✓' if result['verified'] else '✗'}"
                            )
                            results.append(result)

    # Generate report
    output_dir = "/mnt/nvme0/io_bench_results"
    generate_report(results, output_dir)

    print(f"\nDone: {len(results)} results, {len(skipped)} skipped")


if __name__ == "__main__":
    main()
