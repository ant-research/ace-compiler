#!/usr/bin/env python3
"""ResNet Offline Key Loading IO Benchmark.

Measures key-load latency across IO DataPath modes for ResNet-20
FHE inference. Keys must be pre-generated (see --save-keys-first).

Focus: confirm key-load IO throughput matches raw IO baseline.
  - GaloisKey load time: pure IO for the dominant key type (~19 GiB)
  - Baseline reference: GDS 4T4D multi = 39.09 GiB/s (bench_read.py)

**Each config runs in a separate subprocess** because FHERuntime is a
process-wide singleton — keys cached after the first load, subsequent
loads hit memory, not disk. Subprocess isolation guarantees cold-start IO.

Matrix:
  DataPath:  host_staged, host_mapped, gds
  Disk:      D1 (single), D4 (4 disks)
  Threads:   4 (recommended for GDS)
  Repeat:    N separate process launches per config → median

Usage (in docker):
  # Step 0: Save keys first (one-time)
  python3 benchmark/io/bench_key_load.py --save-keys-first /mnt/nvme0/bench_keys_resnet20

  # Step 1: Run key-load benchmark
  python3 benchmark/io/bench_key_load.py /mnt/nvme0/kms_demo_keys_resnet20_gds
  python3 benchmark/io/bench_key_load.py /mnt/nvme0/kms_demo_keys_resnet20_gds --only-gds
  python3 benchmark/io/bench_key_load.py /mnt/nvme0/kms_demo_keys_resnet20_gds \
      --key-dir-4d /mnt/nvme0/kms_demo_keys_resnet20_4disk --only-gds --repeat 3
"""

import argparse
import json
import os
import subprocess
import sys
import time


sys.path.insert(0, os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", ".."))

GiB = 1024**3
DISK_PATHS = ["/mnt/nvme0", "/mnt/nvme1", "/mnt/nvme2", "/mnt/nvme3"]

# ─── Subprocess worker ───────────────────────────────────────────────

_WORKER_SCRIPT = r"""
import ctypes
import json
import os
import sys
import time

# Add project root to path (passed via env)
_project_root = os.environ.get("_ACE_PROJECT_ROOT", "")
if _project_root:
    sys.path.insert(0, _project_root)

from ace import fhe
from ace import runtime as _C
from ace.fhe.config import IoConfig, IoPolicySettings
from ace.fhe.runtime import DatasetEvaluator

DP_CONFIG = {
    "host_staged": {"policy": "staged"},
    "host_mapped": {"policy": "mapped"},
    "gds":         {"policy": "gds"},
}

def apply_config(data_path, threads, disk_list):
    cfg = DP_CONFIG[data_path]
    IoPolicySettings(policy=cfg["policy"], disk_paths=disk_list).apply()
    IoConfig(threads=threads, disk_paths=disk_list).apply()

key_dir      = sys.argv[1]
data_path    = sys.argv[2]
n_disks      = int(sys.argv[3])
threads      = int(sys.argv[4])

disk_list = ["/mnt/nvme0", "/mnt/nvme1", "/mnt/nvme2", "/mnt/nvme3"][:n_disks]
apply_config(data_path, threads, disk_list)

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

# Output as JSON line (parent process parses this)
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


def run_single_load(key_dir, data_path, n_disks, threads):
    """Launch a subprocess for one key-load + inference measurement."""
    bench_dir = os.path.dirname(os.path.abspath(__file__))
    project_root = os.path.abspath(os.path.join(bench_dir, "..", ".."))
    env = dict(os.environ)
    env["_ACE_PROJECT_ROOT"] = project_root
    result = subprocess.run(
        [sys.executable, "-c", _WORKER_SCRIPT, key_dir, data_path, str(n_disks), str(threads)],
        capture_output=True,
        text=True,
        cwd=bench_dir,
        timeout=300,
        env=env,
    )

    # Parse RESULT_JSON from subprocess stdout
    for line in result.stdout.splitlines():
        if line.startswith("RESULT_JSON="):
            return json.loads(line[len("RESULT_JSON=") :])

    # If no RESULT_JSON found, return error info
    err = result.stderr.strip().splitlines()[-3:] if result.stderr else ["no stderr"]
    return {"error": "\n".join(err)}


# ─── Save keys ───────────────────────────────────────────────────────


def save_keys(key_dir: str):
    """Compile ResNet-20, run inference to generate keys, save to key_dir."""
    import torch

    from ace import fhe
    from ace import runtime as _C
    from ace.model.spec_resnet import RESNET20_CIFAR10

    os.makedirs(key_dir, exist_ok=True)

    print("Compiling ResNet-20 ...")
    spec = RESNET20_CIFAR10
    vr_file = spec.get_vr_profile()
    model = spec.create_model()
    example_inputs = tuple(torch.randn(inp.shape, dtype=inp.dtype) for inp in spec.example_inputs)

    compiled = fhe.compile(
        frontend="torch",
        library="phantom",
        device="cuda",
        encrypt_inputs=spec.encrypt_inputs,
        relu_vr_file=vr_file,
    )(model)
    prog = compiled.fhe_compile(example_inputs)
    print(f"  Compiled: kernel={prog.package['kernel'][:60]}...")

    print("Running inference (key generation) ...")
    _C.set_key_path("")
    runner = fhe.FHERuntime(prog.package)
    x = torch.randn(*spec.example_inputs[0].shape, dtype=torch.float32)
    runner.run(x)

    print(f"Saving keys to {key_dir} ...")
    import ace

    rc = runner.save_keys(key_dir)
    assert rc == 0, f"save_keys failed with rc={rc}"

    runner.finalize()

    meta = {
        "model": "resnet20_cifar10",
        "kernel": prog.package["kernel"],
        "config_path": prog.package.get("config_path", ""),
    }
    with open(os.path.join(key_dir, "meta.json"), "w") as f:
        json.dump(meta, f, indent=2)

    total_size = 0
    for root, _, files in os.walk(key_dir):
        for fname in files:
            total_size += os.path.getsize(os.path.join(root, fname))
    print(f"  Keys saved: {total_size / GiB:.2f} GiB")


# ─── Helpers ─────────────────────────────────────────────────────────


def compute_key_size(key_dir: str) -> int:
    total = 0
    for root, _, files in os.walk(key_dir):
        for fname in files:
            fp = os.path.join(root, fname)
            if os.path.isfile(fp):
                total += os.path.getsize(fp)
    return total


def compute_galois_key_size(key_dir: str) -> int:
    """Compute total size of galois key files.
    First tries to sum from index.json (works for multi-disk layouts),
    falls back to scanning local files.
    """
    # Try index.json — has per-entry size, works even if files are on other disks
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


# ─── Main ────────────────────────────────────────────────────────────


def main():
    parser = argparse.ArgumentParser(description="ResNet key-load IO benchmark")
    parser.add_argument("key_dir", help="Key directory for D1 (single-disk) configs")
    parser.add_argument("--key-dir-4d", default=None, help="Key directory for D4 configs (keys across 4 disks)")
    parser.add_argument(
        "--save-keys-first", action="store_true", help="Compile ResNet-20, generate and save keys first"
    )
    parser.add_argument("--threads", type=int, default=4)
    parser.add_argument("--repeat", type=int, default=3, help="Number of separate process launches per config")
    parser.add_argument("--only-gds", action="store_true")
    parser.add_argument("--quick", action="store_true", help="Quick mode: D1 only, 1 repeat")
    args = parser.parse_args()

    key_dir = os.path.abspath(args.key_dir)

    # Save keys if requested
    if args.save_keys_first:
        if os.path.exists(os.path.join(key_dir, "meta.json")):
            print(f"Keys already exist at {key_dir}, skipping save")
        else:
            save_keys(key_dir)
        print()

    if not os.path.exists(os.path.join(key_dir, "meta.json")):
        print(f"ERROR: No meta.json in {key_dir}")
        print("  Run with --save-keys-first to generate keys")
        sys.exit(1)

    # Config matrix
    dp_list = ["gds"] if args.only_gds else ["host_staged", "host_mapped", "gds"]

    if args.quick:
        configs = [(dp, 4, 1) for dp in dp_list]
        repeat = 1
    else:
        configs = [(dp, 4, nd) for dp in dp_list for nd in [1, 4]]
        repeat = args.repeat

    key_dir_4d = os.path.abspath(args.key_dir_4d) if args.key_dir_4d else key_dir

    # Sanity check D4 key dir
    if any(nd > 1 for _, _, nd in configs) and not os.path.exists(os.path.join(key_dir_4d, "meta.json")):
        print(f"WARNING: --key-dir-4d ({key_dir_4d}) has no meta.json, D4 will use D1 keys")

    # Compute key sizes
    key_size = compute_key_size(key_dir)
    galois_key_size = compute_galois_key_size(key_dir)
    galois_key_size_4d = compute_galois_key_size(key_dir_4d)

    print("ResNet-20 Key-Load Benchmark (subprocess-per-config)")
    print(f"  Key dir (D1): {key_dir}  ({key_size / GiB:.2f} GiB, galois={galois_key_size / GiB:.2f} GiB)")
    if key_dir_4d != key_dir:
        print(f"  Key dir (D4): {key_dir_4d}  (galois={galois_key_size_4d / GiB:.2f} GiB)")
    print(f"  Configs: {len(configs)} x {repeat} process launches")
    print()
    print(f"{'Config':<25} {'Galois(s)':>12} {'Init(s)':>10} {'Infer(ms)':>10} {'KMS':>4} {'OK':>4} {'GDS GiB/s':>10}")
    print("-" * 90)

    results = []
    t_wall0 = time.perf_counter()

    for idx, (dp, t, nd) in enumerate(configs, 1):
        cur_key_dir = key_dir_4d if nd > 1 else key_dir
        cur_galois_size = galois_key_size_4d if (nd > 1 and key_dir_4d != key_dir) else galois_key_size
        label = f"{dp}/T{t}/D{nd}"
        print(f"[{idx}/{len(configs)}] {label:<25}", end="  ", flush=True)

        galois_times = []
        inits = []
        last_r = None

        for _ in range(repeat):
            r = run_single_load(cur_key_dir, dp, nd, t)
            if "error" in r:
                print(f"ERROR: {r['error']}")
                break
            if r.get("galois_s") is not None and r["galois_s"] >= 0:
                galois_times.append(r["galois_s"])
            inits.append(r.get("init_s", 0))
            last_r = r

        if not galois_times and not inits:
            results.append({"data_path": dp, "threads": t, "disks": nd, "error": True})
            continue

        galois_sorted = sorted(galois_times)
        galois_med = galois_sorted[len(galois_sorted) // 2] if galois_sorted else None
        galois_min = galois_sorted[0] if galois_sorted else None

        init_med = sorted(inits)[len(inits) // 2] if inits else 0
        kms = "✓" if last_r.get("kms_loaded") else "✗"
        ok = "✓" if last_r.get("top1_correct") else "?"
        infer_ms = f"{last_r['infer_per_img_ms']:.0f}" if last_r.get("infer_per_img_ms") else "—"

        # Galois throughput
        galois_gib = cur_galois_size / GiB
        if galois_med and galois_med > 0:
            gds_gib_s = galois_gib / galois_med
            galois_str = f"{galois_med:>10.2f}"
            throughput_str = f"{gds_gib_s:>10.2f}"
        else:
            galois_str = "—"
            throughput_str = "—"

        print(f"{galois_str}  {init_med:>9.2f}  {infer_ms:>10}  {kms:>4}  {ok:>4}  {throughput_str}")

        result_entry = {
            "data_path": dp,
            "threads": t,
            "disks": nd,
            "galois_median_s": round(galois_med, 4) if galois_med else None,
            "galois_min_s": round(galois_min, 4) if galois_min else None,
            "galois_all_s": [round(x, 4) for x in galois_times],
            "init_median_s": round(init_med, 2),
            "init_all_s": [round(x, 2) for x in inits],
            "kms_loaded": last_r.get("kms_loaded"),
            "top1_correct": last_r.get("top1_correct"),
            "galois_gib": round(galois_gib, 2),
        }
        results.append(result_entry)

    t_wall = time.perf_counter() - t_wall0

    # Summary
    print()
    print("=" * 90)
    print("Key-Load Performance Summary")
    print("=" * 90)
    print(f"{'DataPath':<15} {'T':>3} {'D':>3} {'Galois(med)':>12} {'Galois(min)':>12} {'GiB/s':>8} {'KMS':>4}")
    print("-" * 70)
    for r in results:
        if r.get("error"):
            print(f"{r['data_path']:<15} {r['threads']:>3} {r['disks']:>3}  ERROR")
            continue
        g_med = f"{r['galois_median_s']:>10.2f}s" if r.get("galois_median_s") else "—"
        g_min = f"{r['galois_min_s']:>10.2f}s" if r.get("galois_min_s") else "—"
        gibs = (
            f"{r['galois_gib'] / r['galois_median_s']:>7.2f}"
            if r.get("galois_median_s") and r["galois_median_s"] > 0
            else "—"
        )
        kms = "✓" if r.get("kms_loaded") else "✗"
        print(f"{r['data_path']:<15} {r['threads']:>3} {r['disks']:>3} {g_med}  {g_min}  {gibs}  {kms:>4}")

    # Baseline comparison
    print()
    print("Baseline Comparison (bench_read.py GDS 4T4D multi = 39.09 GiB/s)")
    print("-" * 70)
    for r in results:
        if r.get("error") or not r.get("galois_median_s"):
            continue
        actual = r["galois_gib"] / r["galois_median_s"] if r["galois_median_s"] > 0 else 0
        if r["data_path"] == "gds" and r["disks"] == 4:
            pct = actual / 39.09 * 100
            print(f"  gds/T4/D4: {actual:.2f} GiB/s = {pct:.0f}% of baseline")
        elif r["data_path"] == "gds" and r["disks"] == 1:
            pct = actual / 2.66 * 100
            print(f"  gds/T4/D1: {actual:.2f} GiB/s = {pct:.0f}% of baseline (D1=2.66)")

    # Save JSON
    out_dir = DISK_PATHS[0] + "/io_bench_results"
    os.makedirs(out_dir, exist_ok=True)
    out_file = os.path.join(out_dir, f"key_load_bench_{time.strftime('%Y%m%d_%H%M')}.json")
    with open(out_file, "w") as f:
        json.dump(
            {
                "meta": {
                    "key_dir_d1": key_dir,
                    "key_dir_d4": key_dir_4d if key_dir_4d != key_dir else None,
                    "key_size_gib": round(key_size / GiB, 2),
                    "galois_key_size_gib": round(galois_key_size / GiB, 2),
                    "repeat": repeat,
                    "gpu": "A100 80GB",
                    "nvme": "4x local NVMe",
                    "timestamp": time.strftime("%Y-%m-%d %H:%M"),
                    "wall_seconds": round(t_wall, 1),
                },
                "results": results,
            },
            f,
            indent=2,
        )
    print(f"\nResults saved: {out_file}")
    print(f"Total wall time: {t_wall:.0f}s")


if __name__ == "__main__":
    main()
