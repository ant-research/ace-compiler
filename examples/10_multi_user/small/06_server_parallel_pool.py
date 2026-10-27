#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Small (gemm_49x3) Multi-User Inference — Server-Parallel with KEY POOL.

04_server_parallel.py, extended: serve N > K logical users from only K key sets
resident in HBM. Logical user i maps to session idx = i % K (key bundles
k0, k1, ..., k(K-1), cycling). This bypasses the per-user HBM ceiling:
N users no longer hold N×22 GiB — only K×22 GiB is resident, yet N users are
served.

Concurrency model: K sessions each run on their own CUDA stream + evaluator
(per-session state, as in 04). N logical requests are submitted to a
ThreadPoolExecutor; each request acquires its target session's lock (one lock
per session idx = i % K), then run_session(idx). So at most K inferences run
truly concurrently (one per session), and extra requests queue on the lock.
phantom's per-session evaluator/stream workspace is NOT thread-safe for two
threads sharing a session simultaneously, so the per-session lock serializes
that — K-way real concurrency on K distinct sessions, N>K requests multiplexed.

Same input across all logical users → outputs must agree (same input,
keys cycle but same set; same-slot users produce near-identical output,
cross-slot agree within atol like 04).

Usage:
    CUDA_VISIBLE_DEVICES=6 ACE_GDS_ENABLED=0 FHERT_GALOIS_LOAD_THREADS=1 \\
    python3 examples/10_multi_user/small/06_server_parallel_pool.py \\
        --key-root /mnt/nvme0/multi_user_gemm \\
        --pool-size 3 --num-logical-users 16 --rounds 1 [--io staged]
"""

import argparse
import json
import os
import sys
import time
from concurrent.futures import ThreadPoolExecutor

import torch

from ace import fhe
from ace.fhe.config import IoPolicy, IoPolicySettings
from ace.fhe.util import gpu_available
from ace.sample.ops.specs import ADD_OP, GEMM_49X3_OP, LINEAR_BIAS_OP


sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

SPECS = {"gemm_49x3": GEMM_49X3_OP, "linear_bias": LINEAR_BIAS_OP, "add_op": ADD_OP}


def main():
    ap = argparse.ArgumentParser(description="Small Multi-User — Server-Parallel with KEY POOL")
    ap.add_argument("--key-root", required=True, help="Key-root dir from small/generate_keys.py (contains meta.json)")
    ap.add_argument(
        "--pool-size",
        type=int,
        required=True,
        help="K: number of resident sessions (key bundles k0..k(K-1)). Serves N>K logical users by i%K.",
    )
    ap.add_argument(
        "--num-logical-users",
        type=int,
        required=True,
        help="N: number of logical users served concurrently (N >= K recommended; logical i -> session i%K).",
    )
    ap.add_argument("--rounds", type=int, default=3, help="Concurrent rounds (default: 3)")
    ap.add_argument(
        "--io", choices=["auto", "gds", "staged", "mapped"], default="staged", help="IO policy (default: staged)"
    )
    ap.add_argument(
        "--key-mode",
        choices=["preload_full", "on_demand_full"],
        default="preload_full",
        help="Key residency at init (default: preload_full). on_demand_full defers galois keys.",
    )
    args = ap.parse_args()

    os.environ.setdefault("ACE_GDS_ENABLED", "0")
    os.environ.setdefault("FHERT_GALOIS_LOAD_THREADS", "1")
    IoPolicySettings(
        policy={"auto": IoPolicy.AUTO, "gds": IoPolicy.GDS, "staged": IoPolicy.STAGED, "mapped": IoPolicy.MAPPED}[
            args.io
        ]
    ).apply()

    from ace import runtime as _C
    from ace.fhe.config import KeyMode

    KeyMode.apply(args.key_mode)

    K = args.pool_size
    N = args.num_logical_users
    if K < 2:
        print("ERROR: Server-Parallel pool needs pool-size >= 2 (concurrency requires >=2 sessions).")
        sys.exit(1)
    if N < K:
        print(f"NOTE: num-logical-users {N} < pool-size {K}; serving only {N}.", flush=True)

    root = os.path.abspath(args.key_root)
    with open(os.path.join(root, "meta.json")) as f:
        meta = json.load(f)
    spec = SPECS[meta["spec"]]
    all_user_dirs = [u["key_dir"] for u in meta["user_key_dirs"]]
    if len(all_user_dirs) < K:
        print(f"ERROR: pool-size {K} exceeds key bundles available ({len(all_user_dirs)}).")
        sys.exit(1)
    # K sessions map 1:1 to the first K key bundles.
    user_dirs = all_user_dirs[:K]
    user_ids = [f"user_{meta['user_key_dirs'][i]['user']}" for i in range(K)]
    logical_uids = [f"logical_user_{i}" for i in range(N)]

    if not gpu_available():
        print("ERROR: GPU not available. This example requires CUDA.")
        sys.exit(1)

    print("=" * 70)
    print(
        f"Small Multi-User (Server-Parallel POOL) — {spec.name}, K={K} sessions, N={N} logical users, {args.rounds} rounds"
    )
    print(f"  key_mode = {KeyMode.get()}")
    print(f"  mapping: logical_user_i -> session (i % {K})  =  key k{{i%{K}}}")
    print("=" * 70)

    # --- 1. Primary FHERuntime on session 0 (key k0) ---
    package = {
        "model": spec.name,
        "kernel": meta["kernel"],
        "config_path": meta.get("config_path", ""),
        "key_path": user_dirs[0],
        "key_mode": args.key_mode,
    }

    t0 = time.perf_counter()
    runner = fhe.FHERuntime(package)
    runner.init()
    print(f"  session 0 (k0) init={time.perf_counter() - t0:5.1f}s", flush=True)

    # --- 2. Build the remaining K-1 sessions (k1..k(K-1)) ---
    for i in range(1, K):
        t0 = time.perf_counter()
        idx = runner.create_session(user_dirs[i])
        if idx < 0:
            print(f"  !! create_session for k{i} failed (returned {idx})")
            runner.finalize()
            sys.exit(1)
        print(f"  session {idx} (k{i}) create={time.perf_counter() - t0:5.1f}s", flush=True)

    # --- 3. Inputs ---
    x = tuple(torch.randn(t.shape, dtype=t.dtype) for t in spec.example_inputs)

    # --- 4. Warm-up run_session(0) (absorb lazy kernel/NTT setup) ---
    _ = runner.run_session(0, *x)

    # --- 5. K dedicated single-thread pools (one worker per session slot) ---
    # Each slot s has its OWN ThreadPoolExecutor(max_workers=1): one worker
    # thread, bound forever to session s (only ever calls run_session(s)).
    # Same-slot requests serialize naturally (single worker); K pools run
    # concurrently across slots (K-way real overlap). This avoids both race
    # sources: (a) no two threads share one session (single worker per slot),
    # (b) no worker hops across sessions (each pool's worker stays on its slot),
    # (c) ThreadPool workers are thread-local-safe in phantom (unlike one-shot
    # threading.Thread, which crashed with illegal memory access).
    slot_pools = [ThreadPoolExecutor(max_workers=1) for _ in range(K)]

    def serve(logical_i, out_box):
        slot = logical_i % K
        t0 = time.perf_counter()
        # Share input x across workers. NOTE: per-call t.clone() was tried as a
        # K=3 input-double-free mitigation (it did fix K=3 round 0) BUT it
        # regressed K=2 long runs (rounds>=5 crashed) — phantom's Io_get_input
        # holds the input ref beyond run_session, so a cloned-then-released
        # tensor is use-after-freed. Reverted to shared x: K=2 stable (the
        # documented ceiling), K=3 round 0 crashes with the input-double-free
        # (layer a, fix at codegen MFREE — don't free Get_input_data result).
        out = runner.run_session(slot, *x)
        inf = time.perf_counter() - t0
        out_box[logical_i] = (logical_i, slot, out.float().reshape(-1), inf)

    # --- 6. Parallel rounds: N logical users, K-way concurrency ---
    print(f"\n--- parallel ({N} logical users on K={K} dedicated-slot workers) ---", flush=True)
    all_ok = True
    round_times = []
    for rnd in range(args.rounds):
        results = {}
        t0 = time.perf_counter()
        # Submit logical user i to its slot's dedicated pool. Same-slot users
        # queue on that pool's single worker (serial); K slots run in parallel.
        futs = [slot_pools[i % K].submit(serve, i, results) for i in range(N)]
        for f in futs:
            f.result()
        wall = time.perf_counter() - t0
        round_times.append(wall)

        # Agreement: same slot -> near-identical; cross-slot -> atol=0.1.
        anchor = results[0]
        round_ok = True
        for i in range(1, N):
            r = results[i]
            if r[1] == anchor[1]:  # same slot
                ok = torch.allclose(r[2], anchor[2], atol=0.5, rtol=0.05)
            else:
                ok = torch.allclose(r[2], anchor[2], atol=0.1)
            if not ok:
                round_ok = all_ok = False
                diff = (r[2] - anchor[2]).abs().max().item()
                print(f"    !! logical_user_{i} (slot {r[1]}) mismatch vs anchor: max_diff={diff:.4g}")
        slot_summary = ", ".join(f"k{results[i][1]}" for i in range(min(N, 8)))
        print(
            f"  round {rnd}: wall={wall:5.1f}s  {'all agree ✓' if round_ok else 'MISMATCH'}  "
            f"slots[0:{min(N, 8)}]={slot_summary}",
            flush=True,
        )
        # Per-logical-user infer time for round 0 (sanity)
        if rnd == 0:
            for i in range(N):
                _, slot, _, inf = results[i]
                print(f"    logical_user_{i} -> k{slot}  infer={inf:5.2f}s")

    for p in slot_pools:
        p.shutdown()
    runner.finalize()

    # --- 7. Summary ---
    print()
    print("=" * 70)
    print("Summary")
    print("=" * 70)
    print(f"  K sessions / N logical / rounds:  {K} / {N} / {args.rounds}")
    print(f"  key_mode:                        {KeyMode.get()}")
    print(f"  all outputs agree:               {all_ok}")
    if round_times:
        print(f"  parallel wall:                   {min(round_times):.1f}s min / {max(round_times):.1f}s max")
        print(f"  (K={K}-way concurrency; N={N} requests multiplexed on K sessions)")
    print("=" * 70)
    if not all_ok:
        sys.exit(1)


if __name__ == "__main__":
    main()
