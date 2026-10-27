#!/usr/bin/env python3
"""GDS 4T4D Galois Key Read Performance Benchmark.

Goal: Confirm galois key load via GDS 4T4D matches raw IO baseline (39.09 GiB/s).

Steps:
  1. Merge existing per-key galois files into merged format (1 file per disk).
  2. Load keys via GDS 4T4D in a subprocess, measure galois load time.
  3. Compute throughput = galois_key_size / load_time, compare to baseline.

Usage (in docker):
  # D1 keys (single disk) — merge + benchmark
  python3 benchmark/io/gds_key_read_bench.py /mnt/nvme0/kms_demo_keys_resnet20_gds

  # D4 keys (4 disks) — merge + benchmark
  python3 benchmark/io/gds_key_read_bench.py /mnt/nvme0/kms_demo_keys_resnet20_4disk

  # D4 + D1, repeat 3 times
  python3 benchmark/io/gds_key_read_bench.py /mnt/nvme0/kms_demo_keys_resnet20_gds \
      --key-dir-4d /mnt/nvme0/kms_demo_keys_resnet20_4disk --repeat 3

  # Skip merge step (already merged)
  python3 benchmark/io/gds_key_read_bench.py /mnt/nvme0/kms_demo_keys_resnet20_4disk --skip-merge
"""

import argparse
import json
import os
import subprocess
import sys
import time


sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))

GiB = 1024**3
BASELINE_GDS_T4D4 = 39.09  # GiB/s from bench_read.py
BASELINE_GDS_T4D1 = 2.66  # GiB/s from bench_read.py
DISK_PATHS = ["/mnt/nvme0", "/mnt/nvme1", "/mnt/nvme2", "/mnt/nvme3"]


def merge_galois_keys(key_dir: str, dry_run: bool = False):
    """Merge per-key galois files into merged format (1 file per disk).

    Reads galois_key.index.json, groups by disk_index, concatenates
    per-key .bin files into galois_key.merged.{disk}.bin, and rewrites
    index as version=2.
    """
    model_dir = os.path.join(key_dir, "model")
    idx_path = os.path.join(model_dir, "galois_key.index.json")

    if not os.path.exists(idx_path):
        print(f"ERROR: {idx_path} not found")
        return False

    with open(idx_path) as f:
        idx = json.load(f)

    if idx.get("version", 1) >= 2:
        print(f"Already merged format (version={idx['version']}), nothing to do")
        return True

    disk_paths = idx.get("disk_paths", [])
    relin_keys = idx.get("relin_keys", [])

    if not disk_paths:
        print("ERROR: no disk_paths in index.json")
        return False

    num_disks = len(disk_paths)
    total_key_size = sum(e.get("size", 0) for e in relin_keys)
    print(f"Merging: {len(relin_keys)} keys, {num_disks} disks, {total_key_size / GiB:.2f} GiB total")

    # Group entries by disk_index
    groups = {}
    for entry in relin_keys:
        d = entry.get("disk", entry.get("disk_index", 0))
        groups.setdefault(d, []).append(entry)

    for d, entries in sorted(groups.items()):
        total = sum(e.get("size", 0) for e in entries)
        print(f"  Disk {d}: {len(entries)} files, {total / GiB:.2f} GiB")

    if dry_run:
        print("[DRY RUN] No files written")
        return True

    # Backup original index
    bak_path = idx_path + ".v1.bak"
    if not os.path.exists(bak_path):
        import shutil

        shutil.copy2(idx_path, bak_path)
        print(f"Backed up original index → {bak_path}")

    # Merge: for each disk, cat all per-key .bin files into one merged file
    new_entries = []

    for d, entries in sorted(groups.items()):
        disk_dir = disk_paths[d]
        merged_name = f"galois_key.merged.{d}.bin"
        merged_path = os.path.join(disk_dir, merged_name)

        print(f"  Merging {len(entries)} files → {merged_path} ...")

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
                        chunk = in_f.read(256 * 1024 * 1024)
                        if not chunk:
                            break
                        out_f.write(chunk)

                offset += entry_size

        print(f"  Written: {offset / GiB:.2f} GiB")

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

    print(f"Updated index.json → version=2, {len(new_entries)} entries")
    print("Merge done ✓")
    return True


def compute_galois_key_size(key_dir: str) -> int:
    """Compute total size of galois key files from index.json."""
    idx_path = os.path.join(key_dir, "model", "galois_key.index.json")
    if os.path.exists(idx_path):
        try:
            with open(idx_path) as f:
                idx = json.load(f)
            return sum(e.get("size", 0) for e in idx.get("relin_keys", []))
        except Exception:
            pass
    # Fallback: scan local files
    total = 0
    for root, _, files in os.walk(key_dir):
        for f in files:
            if f.startswith("galois_key") and f.endswith(".bin"):
                total += os.path.getsize(os.path.join(root, f))
    return total


# ─── Subprocess worker: load keys + measure galois read time ────────────

_WORKER_SCRIPT = r"""
import ctypes
import json
import os
import sys
import time

_project_root = os.environ.get("_ACE_PROJECT_ROOT", "")
if _project_root:
    sys.path.insert(0, _project_root)

from ace import fhe
from ace import runtime as _C
from ace.fhe.config import IoConfig, IoPolicySettings
from ace.fhe.runtime import DatasetEvaluator

key_dir      = sys.argv[1]
data_path    = sys.argv[2]   # "gds"
n_disks      = int(sys.argv[3])
threads      = int(sys.argv[4])
policy_str   = sys.argv[5]   # "gds" or "staged"

disk_list = ["/mnt/nvme0", "/mnt/nvme1", "/mnt/nvme2", "/mnt/nvme3"][:n_disks]

# Apply IO config: IoPolicy for transport path + IoConfig for threads
POLICY_MAP = {
    "gds": "gds",
    "host_staged": "staged",
    "host_mapped": "mapped",
}
IoPolicySettings(policy=POLICY_MAP.get(data_path, policy_str), disk_paths=disk_list).apply()
IoConfig(threads=threads, disk_paths=disk_list).apply()

with open(os.path.join(key_dir, "meta.json")) as f:
    meta = json.load(f)

package = {
    "model": meta.get("model_key", meta.get("model", "resnet20_cifar10")),
    "kernel": meta["kernel"],
    "config_path": meta.get("config_path", ""),
    "key_path": key_dir,
}

_C.set_key_path(key_dir)

t0 = time.perf_counter()
runner = fhe.FHERuntime(package)

from ace.model.dataset import load_cifar10_images
images, labels = load_cifar10_images(1)
result = DatasetEvaluator(runner).evaluate(images[:1], labels[:1], top_k=1, verbose=False)
t1 = time.perf_counter()

# Get galois key load time + KMS-loaded flag via the runner dlsym surface
# (replaces the retired ctypes FHert_* / Phantom_* access path).
galois_s = runner.get_galois_load_time_seconds()
kms_loaded = runner.keys_loaded_from_kms() == 1

total_s = t1 - t0
infer_s = result.timing.total_ms / 1000.0 if result.timing.num_images > 0 else 0
init_s = total_s - infer_s
infer_per_img_ms = result.timing.avg_per_image_ms if result.timing.num_images > 0 else None

runner.finalize()
_C.clear_key_path()

output = {
    "galois_s": round(galois_s, 4) if galois_s >= 0 else None,
    "init_s": round(init_s, 2),
    "total_s": round(total_s, 2),
    "infer_s": round(infer_s, 2),
    "infer_per_img_ms": round(infer_per_img_ms, 1) if infer_per_img_ms else None,
    "kms_loaded": kms_loaded,
    "top1_correct": result.top1_accuracy > 0 if result.top1_accuracy is not None else None,
}
print("RESULT_JSON=" + json.dumps(output))
"""


def run_single_load(key_dir, data_path, n_disks, threads, policy_str):
    """Launch a subprocess for one key-load + inference measurement."""
    bench_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.abspath(os.path.join(bench_dir, "..", ".."))
    env = dict(os.environ)
    env["_ACE_PROJECT_ROOT"] = project_root
    result = subprocess.run(
        [sys.executable, "-c", _WORKER_SCRIPT, key_dir, data_path, str(n_disks), str(threads), policy_str],
        capture_output=True,
        text=True,
        cwd=bench_dir,
        timeout=300,
        env=env,
    )

    for line in result.stdout.splitlines():
        if line.startswith("RESULT_JSON="):
            return json.loads(line[len("RESULT_JSON=") :])

    err = result.stderr.strip().splitlines()[-5:] if result.stderr else ["no stderr"]
    return {"error": "\n".join(err)}


def main():
    parser = argparse.ArgumentParser(description="GDS key read benchmark (merge + load)")
    parser.add_argument("key_dir", help="Key directory for D1")
    parser.add_argument("--key-dir-4d", default=None, help="Key directory for D4 configs (across 4 disks)")
    parser.add_argument("--skip-merge", action="store_true", help="Skip merge step (already merged format)")
    parser.add_argument("--threads", type=int, default=4)
    parser.add_argument("--repeat", type=int, default=3)
    parser.add_argument(
        "--policy", default="gds", choices=["gds", "staged", "auto"], help="IoPolicy for key loading (default: gds)"
    )
    args = parser.parse_args()

    key_dir = os.path.abspath(args.key_dir)
    key_dir_4d = os.path.abspath(args.key_dir_4d) if args.key_dir_4d else key_dir

    # ─── Step 1: Merge galois keys ──────────────────────────────────
    if not args.skip_merge:
        print("=" * 60)
        print("Step 1: Merge per-key galois files → merged format")
        print("=" * 60)

        if not merge_galois_keys(key_dir):
            print(f"ERROR: Failed to merge keys in {key_dir}")
            sys.exit(1)
        print()

        if key_dir_4d != key_dir:
            if not merge_galois_keys(key_dir_4d):
                print(f"ERROR: Failed to merge keys in {key_dir_4d}")
                sys.exit(1)
            print()
    else:
        print("[Skip merge] Using existing merged format keys")

    # ─── Step 2: Compute key sizes ─────────────────────────────────
    galois_size_d1 = compute_galois_key_size(key_dir)
    galois_size_d4 = compute_galois_key_size(key_dir_4d)

    print(f"\nGalois key size (D1): {galois_size_d1 / GiB:.2f} GiB  ({key_dir})")
    if key_dir_4d != key_dir:
        print(f"Galois key size (D4): {galois_size_d4 / GiB:.2f} GiB  ({key_dir_4d})")
    print()

    # ─── Step 3: Run GDS key-load benchmark ────────────────────────
    print("=" * 60)
    print("Step 2: GDS key-load benchmark")
    print("=" * 60)

    configs = [
        # (label, key_dir, n_disks, galois_size)
        ("gds/T4/D1", key_dir, 1, galois_size_d1),
        ("gds/T4/D4", key_dir_4d, 4, galois_size_d4),
    ]

    print(f"{'Config':<20} {'Galois(s)':>12} {'Init(s)':>10} {'KMS':>5} {'OK':>4} {'GiB/s':>10} {'%baseline':>10}")
    print("-" * 85)

    results = []

    for label, kd, nd, gs in configs:
        galois_gib = gs / GiB
        galois_times = []
        last_r = None

        for rep in range(args.repeat):
            r = run_single_load(kd, "gds", nd, args.threads, args.policy)
            if "error" in r:
                print(f"  [{label}] rep {rep}: ERROR: {r['error']}")
                break
            if r.get("galois_s") is not None and r["galois_s"] >= 0:
                galois_times.append(r["galois_s"])
            last_r = r

        if not galois_times:
            print(f"  {label:<20}  FAILED")
            results.append({"label": label, "error": True})
            continue

        galois_med = sorted(galois_times)[len(galois_times) // 2]
        galois_min = sorted(galois_times)[0]
        kms = "✓" if last_r.get("kms_loaded") else "✗"
        ok = "✓" if last_r.get("top1_correct") else "?"

        if galois_med > 0:
            gib_s = galois_gib / galois_med
            baseline = BASELINE_GDS_T4D4 if nd == 4 else BASELINE_GDS_T4D1
            pct = gib_s / baseline * 100
            galois_str = f"{galois_med:>10.2f}"
            throughput_str = f"{gib_s:>10.2f}"
            pct_str = f"{pct:>9.0f}%"
        else:
            galois_str = "—"
            throughput_str = "—"
            pct_str = "—"

        print(
            f"  {label:<20} {galois_str}  {last_r.get('init_s', 0):>9.2f}  {kms:>5}  {ok:>4}  {throughput_str}  {pct_str}"
        )

        results.append(
            {
                "label": label,
                "n_disks": nd,
                "galois_median_s": round(galois_med, 4),
                "galois_min_s": round(galois_min, 4),
                "galois_all_s": [round(x, 4) for x in galois_times],
                "galois_gib": round(galois_gib, 2),
                "gib_s": round(gib_s, 2) if galois_med > 0 else None,
                "kms_loaded": last_r.get("kms_loaded"),
                "top1_correct": last_r.get("top1_correct"),
            }
        )

    # ─── Summary ───────────────────────────────────────────────────
    print()
    print("=" * 85)
    print("Summary: GDS Key-Load vs Raw IO Baseline")
    print("=" * 85)

    for r in results:
        if r.get("error"):
            print(f"  {r['label']}: FAILED")
            continue
        nd = r["n_disks"]
        baseline = BASELINE_GDS_T4D4 if nd == 4 else BASELINE_GDS_T4D1
        actual = r["gib_s"]
        if actual is not None:
            pct = actual / baseline * 100
            print(f"  {r['label']}: {actual:.2f} GiB/s  (baseline={baseline:.2f}, {pct:.0f}% of baseline)")
        else:
            print(f"  {r['label']}: no throughput data")

    # Save results
    out_dir = DISK_PATHS[0] + "/io_bench_results"
    os.makedirs(out_dir, exist_ok=True)
    out_file = os.path.join(out_dir, f"gds_key_read_bench_{time.strftime('%Y%m%d_%H%M')}.json")
    with open(out_file, "w") as f:
        json.dump(
            {
                "meta": {
                    "key_dir_d1": key_dir,
                    "key_dir_d4": key_dir_4d if key_dir_4d != key_dir else None,
                    "galois_size_d1_gib": round(galois_size_d1 / GiB, 2),
                    "galois_size_d4_gib": round(galois_size_d4 / GiB, 2),
                    "threads": args.threads,
                    "repeat": args.repeat,
                    "policy": args.policy,
                    "baseline_gds_t4d4": BASELINE_GDS_T4D4,
                    "baseline_gds_t4d1": BASELINE_GDS_T4D1,
                    "timestamp": time.strftime("%Y-%m-%d %H:%M"),
                },
                "results": results,
            },
            f,
            indent=2,
        )
    print(f"\nResults saved: {out_file}")


if __name__ == "__main__":
    main()
