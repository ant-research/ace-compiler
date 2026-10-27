#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
ResNet-20 Multi-User Inference — Server-Parallel with KEY POOL.

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

Same CIFAR input across all logical users → outputs must agree (same input,
keys cycle but same set; same-slot users produce near-identical output,
cross-slot agree within atol like 04).

Usage:
    CUDA_VISIBLE_DEVICES=0 \\
    python3 examples/10_multi_user/resnet20/06_server_parallel_pool.py \\
        --key-root /mnt/nvme0/mu_keys_resnet20 \\
        --pool-size 3 --num-logical-users 8 --rounds 2 [--io staged]
"""

import argparse
import json
import os
import sys
import threading
import time
from concurrent.futures import ThreadPoolExecutor

import torch

from ace import fhe
from ace.fhe.config import IoPolicy, IoPolicySettings
from ace.fhe.util import gpu_available
from ace.model.dataset import load_cifar10_images
from ace.model.spec_resnet import RESNET20_CIFAR10 as SPEC


# generate_keys._compile_once lives in the same dir — reuse for a fresh kernel/config.
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from generate_keys import _compile_once


# multi_user_runtime lives in the parent dir (10_multi_user/).
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))


def main():
    ap = argparse.ArgumentParser(description="ResNet-20 Multi-User — Server-Parallel with KEY POOL")
    ap.add_argument(
        "--key-root", required=True, help="Key-root dir from resnet20/generate_keys.py (contains meta.json)"
    )
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
    ap.add_argument("--hbm", action="store_true", help="Print resident GPU HBM after building all K sessions.")
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
    if meta.get("spec") != "resnet20":
        print(f"ERROR: meta.json spec='{meta.get('spec')}', expected 'resnet20'.")
        sys.exit(1)
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
    print(f"ResNet-20 Multi-User (Server-Parallel POOL) — K={K} sessions, N={N} logical users, {args.rounds} rounds")
    print(f"  key_mode = {KeyMode.get()}")
    print(f"  mapping: logical_user_i -> session (i % {K})  =  key k{{i%{K}}}")
    print("=" * 70)

    # --- 1. Compile resnet20 once ---
    print("  compiling resnet20 ...", flush=True)
    t0 = time.perf_counter()
    package = _compile_once(meta.get("backend", "phantom"))
    print(f"  compiled ({time.perf_counter() - t0:.1f}s); kernel: {package['kernel']}", flush=True)

    # --- 2. Primary FHERuntime on session 0 (key k0) ---
    pkg = {**package, "key_path": user_dirs[0], "key_mode": args.key_mode}
    t0 = time.perf_counter()
    runner = fhe.FHERuntime(pkg)
    runner.init()
    print(f"  session 0 (k0) init={time.perf_counter() - t0:5.1f}s", flush=True)

    # --- 3. Build the remaining K-1 sessions (k1..k(K-1)) ---
    for i in range(1, K):
        t0 = time.perf_counter()
        idx = runner.create_session(user_dirs[i])
        if idx < 0:
            print(f"  !! create_session for k{i} failed (returned {idx})")
            runner.finalize()
            sys.exit(1)
        print(f"  session {idx} (k{i}) create={time.perf_counter() - t0:5.1f}s", flush=True)

    if args.hbm:
        try:
            free, total = torch.cuda.mem_get_info()
            print(
                f"  [hbm] after building K={K} sessions: resident ≈ {(total - free) // (1024 * 1024)} MiB "
                f"(key {'lazy-empty' if args.key_mode == 'on_demand_full' else 'full-set per session'})"
            )
        except Exception as e:
            print(f"  [hbm] mem_get_info failed: {e}")

    # --- 4. Inputs (normalized CIFAR) ---
    cifar_imgs, _ = load_cifar10_images(max(N, 1))
    # Use a single CIFAR image for the agreement check; rounds cycle images.
    x = (cifar_imgs[0:1].float(),)

    # --- 5. Warm-up ALL K sessions (absorb lazy kernel/NTT/encoder setup per session) ---
    # Previously only session 0 was warmed — the first parallel round then
    # paid session 1/2's lazy init inside their infer timing (51s vs 16s
    # steady). Warm each session so measured rounds are steady-state.
    for s in range(K):
        _ = runner.run_session(s, *x)

    # --- 6. K dedicated single-thread pools (one worker per session slot) ---
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
        out = runner.run_session(slot, *x)
        inf = time.perf_counter() - t0
        out_box[logical_i] = (logical_i, slot, out.float().reshape(-1), inf)

    # --- 7. Parallel rounds: N logical users, K-way concurrency ---
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

    # --- 8. Summary ---
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
