#!/usr/bin/env python3
# examples/10_multi_user/small/05_on_demand_keys.py
#
# On-demand galois-key loading — standalone runnable demo.
#
# Compiles a small model with rotations (gemm_49x3), saves keys once, then
# runs the same inference two ways and compares:
#   1. PRELOAD_FULL  (key_offline): all galois keys resident at init (~HBM-full).
#   2. ON_DEMAND_FULL: SK/PK/RLK preload, galois keys LAZY — each rotate pulls
#      the one needed key into an HBM cache on first use (Ensure_galois_key).
#
# Asserts outputs match (lazy loading is correct) and prints peak-resident
# GPU memory to show the HBM saving at init (and that working-set residency,
# not full-set, holds).
#
# cuFile-false-host note: this host has false-available cuFile; force staged
# IO / per-key galois / single-thread (mirrors tests/integration/conftest).

import argparse
import os
import sys
import tempfile


# cuFile-false-host workaround (must be set before importing ace).
os.environ.setdefault("ACE_GDS_ENABLED", "0")
os.environ.setdefault("FHERT_GALOIS_LOAD_THREADS", "1")

import torch

from ace import fhe
from ace import runtime as _C
from ace.fhe.config import IoConfig, IoPolicy, IoPolicySettings
from ace.sample.ops.specs import GEMM_49X3_OP


SPECS = {"gemm_49x3": GEMM_49X3_OP}


def _peak_gpu_mb(label):
    try:
        free, total = torch.cuda.mem_get_info()
        return (total - free) // (1024 * 1024)
    except Exception:
        return -1


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument("--spec", choices=list(SPECS), default="gemm_49x3")
    ap.add_argument("--tol", type=float, default=1e-3, help="max output diff vs baseline")
    ap.add_argument("--warmup", type=int, default=2, help="untimed warm-up frames per mode")
    ap.add_argument("--runs", type=int, default=5, help="timed frames per mode (latency min/avg/max)")
    ap.add_argument(
        "--free-galois-key",
        action="store_true",
        help="emit Free_galois_key after variable-key rotates (codegen -P2C:fgk)",
    )
    args = ap.parse_args()

    spec = SPECS[args.spec]
    os.environ["ACE_GDS_ENABLED"] = "0"
    os.environ["FHERT_GALOIS_LOAD_THREADS"] = "1"
    IoPolicySettings(policy=IoPolicy.STAGED).apply()
    IoConfig(galois_merged_format=False).apply()

    model = spec.create_model()
    inputs = tuple(t.clone() for t in spec.example_inputs)

    key_dir = tempfile.mkdtemp(prefix=f"on_demand_demo_{spec.name}_")

    print("=" * 72)
    print(f"On-Demand Key Loading demo — spec={spec.name}")
    print("=" * 72)

    # 1. Compile online + save the key set once (ONLINE mode for key gen).
    _C.set_key_mode(0)  # ONLINE
    _C.set_key_path("")
    print("[1] compiling + generating keys online ...")
    p2c = {"free_galois_key": True} if args.free_galois_key else None
    compiled = fhe.compile(
        frontend="torch", library="phantom", device="cuda", encrypt_inputs=spec.encrypt_inputs, p2c=p2c
    )(model)
    prog = compiled.fhe_compile(inputs)
    runner = fhe.FHERuntime(prog.package)
    runner.run(*[t.clone() for t in inputs])
    rc = runner.save_keys(key_dir)
    assert rc == 0, f"save_keys failed rc={rc}"
    runner.finalize()
    print(f"    keys saved to {key_dir}")

    base_pkg = {
        "model": spec.name,
        "kernel": prog.package["kernel"],
        "config_path": prog.package.get("config_path", ""),
        "key_path": key_dir,
    }

    # 2. PRELOAD_FULL baseline — full galois set resident at init.
    _C.set_key_path(key_dir)
    rb = fhe.FHERuntime({**base_pkg, "key_mode": "key_offline"})
    rb.init()
    preload_init_hbm = _peak_gpu_mb("preload")
    # Warm-up + timed runs (per-frame latency; gemm is deterministic).
    import time

    for _ in range(args.warmup):
        rb.run(*[t.clone() for t in inputs])
    torch.cuda.synchronize() if torch.cuda.is_available() else None
    pre_times = []
    for _ in range(args.runs):
        t0 = time.perf_counter()
        out = rb.run(*[t.clone() for t in inputs])
        torch.cuda.synchronize() if torch.cuda.is_available() else None
        pre_times.append((time.perf_counter() - t0) * 1e3)
    baseline = out.detach().cpu().flatten().float()
    torch.cuda.synchronize() if torch.cuda.is_available() else None
    preload_post_hbm = _peak_gpu_mb("preload-post")  # resident after frames (full set)
    rb.finalize()
    print(f"[2] PRELOAD_FULL      : peak HBM @ init ≈ {preload_init_hbm} MiB (full galois set resident)")
    print(f"    HBM after frames   : ≈ {preload_post_hbm} MiB")
    print(
        f"    per-frame ms       : min={min(pre_times):.1f} "
        f"avg={sum(pre_times) / len(pre_times):.1f} max={max(pre_times):.1f} "
        f"(warmup={args.warmup}, runs={args.runs})"
    )

    # 3. ON_DEMAND_FULL — galois LAZY.
    _C.set_key_path(key_dir)
    ro = fhe.FHERuntime({**base_pkg, "key_mode": "on_demand_full"})
    ro.init()
    ondemand_init_hbm = _peak_gpu_mb("on_demand")
    # Warm-up: first frame(s) pay the lazy galois-key cache-miss load; timed
    # runs after warm-up measure steady-state (keys resident in HBM cache).
    for _ in range(args.warmup):
        ro.run(*[t.clone() for t in inputs])
    torch.cuda.synchronize() if torch.cuda.is_available() else None
    ond_times = []
    for _ in range(args.runs):
        t0 = time.perf_counter()
        out = ro.run(*[t.clone() for t in inputs])
        torch.cuda.synchronize() if torch.cuda.is_available() else None
        ond_times.append((time.perf_counter() - t0) * 1e3)
    ondemand = out.detach().cpu().flatten().float()
    torch.cuda.synchronize() if torch.cuda.is_available() else None
    ondemand_post_hbm = _peak_gpu_mb("on_demand-post")  # resident after frames, no free
    ro.finalize()
    print(f"[3] ON_DEMAND_FULL    : peak HBM @ init ≈ {ondemand_init_hbm} MiB (galois empty until first rotate)")
    print(f"    HBM after frames   : ≈ {ondemand_post_hbm} MiB (lazy keys not freed → should approach PRELOAD)")
    print(
        f"    per-frame ms       : min={min(ond_times):.1f} "
        f"avg={sum(ond_times) / len(ond_times):.1f} max={max(ond_times):.1f} "
        f"(warmup={args.warmup}, runs={args.runs})"
    )
    print(f"    HBM saved @ init   : ~{preload_init_hbm - ondemand_init_hbm} MiB")
    print(
        f"    HBM after vs PRELOAD after: {ondemand_post_hbm - preload_post_hbm:+d} MiB "
        f"(≈0 confirms steady-state ≈ full set → free needed to raise K)"
    )
    print(
        f"    latency delta(avg) : ON_DEMAND - PRELOAD = "
        f"{sum(ond_times) / len(ond_times) - sum(pre_times) / len(pre_times):+.1f} ms/frame"
    )

    # 4. Compare outputs.
    max_diff = (baseline - ondemand).abs().max().item()
    print("-" * 72)
    print(f"[4] max_diff ON_DEMAND_FULL vs PRELOAD_FULL = {max_diff:.3e}")
    _C.clear_key_path()
    if max_diff < args.tol:
        print(f"    PASS (within tol={args.tol}) — on-demand lazy loading produces identical output.")
        return 0
    print(f"    FAIL: exceeds tol={args.tol}")
    return 1


if __name__ == "__main__":
    sys.exit(main())
