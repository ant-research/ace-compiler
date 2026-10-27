#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Multi-User Inference — Server-Parallel SERIAL driver (formerly Mode B-P SERIAL; per-session evaluator, serial serving).

This is the serial serving path of the Server-Parallel mode (the parallel half
— ``run_session`` from N worker threads at once — lives in
resnet20/04_server_parallel.py; see report.md for its 2-way-stable / 3-way-race
status).

One process, one GPU, one shared FheContext (built once, never rebuilt). Each
user's key bundle is loaded into its OWN resident FheSession in HBM via
``FHERuntime.create_session(key_dir)`` — and each session has its OWN evaluator
+ CUDA stream (the per-session evaluator + cross-DSO thread-local routing from
commit 78a6109). Serving is therefore by ``run_session(idx, x)`` directly:
the calling thread is bound to session ``idx`` before kernel execution, so
**no ``switch_session`` is needed** between users — contrast 03_server_serial.py (Server-Serial),
which rebinds a single active evaluator via ``switch_session`` before each
``run()``.

Flow
----
1. Build one FHERuntime on user_0's key_dir (primary session, idx 0) → init()
   (timed: context build + primary key load).
2. For each extra user: create_session(key_dir) — full key load into HBM (timed).
   Resident thereafter. Each gets its own evaluator + stream.
3. Cycle users SERIALLY: run_session(idx, x) (timed). No switch.
4. Cross-user output agreement (same input, different keys → same output).

Usage:
    python3 examples/10_multi_user/small/generate_keys.py --num-users 3 --key-root /tmp/mu_small
    python3 examples/10_multi_user/small/04_server_parallel.py --key-root /tmp/mu_small
"""

import argparse
import json
import os
import statistics
import sys
import time

import torch

from ace import fhe
from ace.fhe.config import IoPolicy, IoPolicySettings
from ace.fhe.util import gpu_available
from ace.sample.ops.specs import ADD_OP, GEMM_49X3_OP, LINEAR_BIAS_OP


sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

SPECS = {"gemm_49x3": GEMM_49X3_OP, "linear_bias": LINEAR_BIAS_OP, "add_op": ADD_OP}


def main():
    parser = argparse.ArgumentParser(
        description="Multi-User FHE inference — Server-Parallel serial driver (per-session evaluator, run_session, no switch)"
    )
    parser.add_argument(
        "--key-root", required=True, help="Key-root dir from small/generate_keys.py (contains meta.json)"
    )
    parser.add_argument("--rounds", type=int, default=3, help="Times to cycle through all users (default: 3)")
    parser.add_argument(
        "--io", choices=["auto", "gds", "staged", "mapped"], default="staged", help="IO policy (default: staged)"
    )
    args = parser.parse_args()

    os.environ.setdefault("ACE_GDS_ENABLED", "0")
    os.environ.setdefault("FHERT_GALOIS_LOAD_THREADS", "1")
    IoPolicySettings(
        policy={"auto": IoPolicy.AUTO, "gds": IoPolicy.GDS, "staged": IoPolicy.STAGED, "mapped": IoPolicy.MAPPED}[
            args.io
        ]
    ).apply()

    with open(os.path.join(os.path.abspath(args.key_root), "meta.json")) as f:
        meta = json.load(f)
    spec = SPECS[meta["spec"]]
    num_users = meta["num_users"]
    user_dirs = [u["key_dir"] for u in meta["user_key_dirs"]]
    user_ids = [f"user_{u['user']}" for u in meta["user_key_dirs"]]

    if not gpu_available():
        print("ERROR: GPU not available. This example requires CUDA.")
        sys.exit(1)

    print("=" * 70)
    print(f"Multi-User (Server-Parallel serial) — {spec.name}, {num_users} users, {args.rounds} rounds")
    print("=" * 70)

    # --- 1. Primary FHERuntime on user_0 key dir (idx 0) ---
    package = {
        "model": spec.name,
        "kernel": meta["kernel"],
        "config_path": meta.get("config_path", ""),
        "key_path": user_dirs[0],
        "key_mode": "preload_full",
    }

    t0 = time.perf_counter()
    runner = fhe.FHERuntime(package)
    t0i = time.perf_counter()
    runner.init()
    primary_init_total = time.perf_counter() - t0i
    primary_build = time.perf_counter() - t0
    primary_load = runner.get_galois_load_time_seconds()
    primary_init = primary_init_total - primary_load
    print(
        f"  primary {user_ids[0]:8s} build(ctx+load)={primary_build * 1e3:6.0f}ms "
        f"load={primary_load * 1e3:6.0f}ms init={primary_init * 1e3:6.0f}ms  (idx=0)"
    )

    session_idx = {user_ids[0]: 0}

    # --- 2. Resident extra sessions (one per remaining user) — own evaluator+stream ---
    create_times: dict[str, float] = {}
    for uid, kd in zip(user_ids[1:], user_dirs[1:]):
        t0 = time.perf_counter()
        idx = runner.create_session(kd)
        dt = time.perf_counter() - t0
        if idx < 0:
            print(f"  !! create_session({uid}) failed (returned {idx}); C++ session path missing?")
            runner.finalize()
            sys.exit(1)
        session_idx[uid] = idx
        gk = runner.get_galois_load_time_seconds()
        create_times[uid] = dt
        print(f"  create  {uid:8s} idx={idx} total={dt * 1e3:6.0f}ms galois_load={gk * 1e3:6.0f}ms")

    # --- 3. Warm-up run under primary ---
    x = tuple(torch.randn(t.shape, dtype=t.dtype) for t in spec.example_inputs)
    _ = runner.run_session(0, *x)

    # --- 4. Cycle users SERIALLY: run_session(idx, x). NO switch_session. ---
    infer_times: dict[str, list[float]] = {uid: [] for uid in user_ids}
    all_ok = True
    t0_total = time.perf_counter()

    for r in range(args.rounds):
        x = tuple(torch.randn(t.shape, dtype=t.dtype) for t in spec.example_inputs)
        round_outputs: dict[str, torch.Tensor] = {}

        for uid in user_ids:
            idx = session_idx[uid]
            t1 = time.perf_counter()
            out = runner.run_session(idx, *x)
            inf = time.perf_counter() - t1
            infer_times[uid].append(inf)
            round_outputs[uid] = out.float().reshape(out.shape)
            print(f"  round {r} {uid:8s} (idx={idx}) infer={inf * 1e3:6.0f}ms")

        # Cross-user agreement (same input, different keys → same output).
        atol, rtol = 0.1, 0.05
        anchor = user_ids[0]
        for uid in user_ids[1:]:
            if not torch.allclose(round_outputs[uid], round_outputs[anchor], atol=atol, rtol=rtol):
                all_ok = False
                diff = (round_outputs[uid] - round_outputs[anchor]).abs().max().item()
                print(f"    !! {uid} mismatch vs {anchor}: max_abs_diff={diff:.4g}")
        if all_ok:
            print(f"  round {r}: all users agree ✓ (serial run_session, no switch)")

    total_s = time.perf_counter() - t0_total
    runner.finalize()

    # --- 5. Summary ---
    print()
    print("=" * 70)
    print("Summary")
    print("=" * 70)
    print(f"  users / rounds:  {num_users} / {args.rounds}")
    print(f"  serve wall:      {total_s:.3f}s  (run_session calls only; setup excluded)")
    print(f"  all correct:     {all_ok}")
    print()
    print(f"  {'user':8s} {'create_ms':>10s} {'infer_ms':>10s}")
    for uid in user_ids:
        c = create_times.get(uid, 0.0)
        inf = statistics.mean(infer_times[uid]) * 1e3 if infer_times[uid] else 0.0
        print(f"  {uid:8s} {c * 1e3:9.1f}ms {inf:9.1f}ms")
    print()
    print(f"  primary build (one-time):  {primary_build * 1e3:.0f}ms  (context + key load)")
    print(
        f"  per-user create (one-time): {statistics.mean(create_times.values()) * 1e3:.0f}ms avg "
        f"(galois load; resident thereafter, own evaluator+stream)"
    )
    print("  serving: serial run_session(idx) — no switch_session in the loop")
    print("=" * 70)
    if not all_ok:
        sys.exit(1)


if __name__ == "__main__":
    main()
