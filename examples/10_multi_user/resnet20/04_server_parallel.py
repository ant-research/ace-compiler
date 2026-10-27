#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
ResNet-20 Multi-User Inference — Server-Parallel mode (formerly Mode B-P; per-session evaluator, concurrent serving).

One process, one GPU, one shared FheContext (built once, never rebuilt). Each user's
key bundle is loaded into its OWN resident FheSession in HBM via create_session, and
each session has its OWN evaluator + PhantomCKKSEncoder + CUDA stream + (for resnet20)
its OWN bootstrap manager. Serving N users CONCURRENTLY: N worker threads each call
run_session(idx) — the calling thread is bound to session idx (thread-local evaluator
routing + per-thread stream) before kernel execution, so two sessions run in parallel
on two streams with NO shared mutable workspace. Often faster than Server-Serial when
single-user inference doesn't saturate the GPU (~60% SM on resnet20).

This is the concurrency-safe path landed in commit bec4488 (route encode/decode +
bootstrap to per-session state). Contrast with 03_server_serial.py (Server-Serial): there a single
active evaluator is rebound via switch_session and inference is serial.

Inputs MUST be normalized CIFAR-10 images (ReLU VR profile is fit on CIFAR ranges;
raw randn makes the ReLU polynomial explode).

NOTE: extra sessions must use the same galois elements as the primary (true for
same-model keys). The shared GaloisTool / PhantomContext (NTT/sin-cos/RNS tables) is
not rebuilt — it is read-only under concurrency.

Usage:
    CUDA_VISIBLE_DEVICES=0 python3 examples/10_multi_user/resnet20/04_server_parallel.py \\
        --key-root /mnt/nvme0/mu_keys_resnet20 --num-users 2 --rounds 2
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
from ace.model.dataset import load_cifar10_images
from ace.model.spec_resnet import RESNET20_CIFAR10 as SPEC


# generate_keys._compile_once lives in the same dir — reuse for a fresh kernel/config.
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from generate_keys import _compile_once


# multi_user_runtime lives in the parent dir (10_multi_user/).
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))


def main():
    ap = argparse.ArgumentParser(description="ResNet-20 Multi-User — Server-Parallel (concurrent run_session)")
    ap.add_argument(
        "--key-root", required=True, help="Key-root dir from resnet20/generate_keys.py (contains meta.json)"
    )
    ap.add_argument(
        "--num-users", type=int, default=3, help="Number of users/sessions to serve concurrently (default: 3)"
    )
    ap.add_argument("--rounds", type=int, default=3, help="Concurrent rounds (default: 3)")
    ap.add_argument(
        "--io", choices=["auto", "gds", "staged", "mapped"], default="staged", help="IO policy (default: staged)"
    )
    ap.add_argument(
        "--release",
        action="store_true",
        help="After each parallel round, release_session all sessions (evict galois-key "
        "HBM; session structure stays). Next round's run_session re-loads keys "
        "on-demand via Ensure_galois_key. Tests the 'parallel release' strategy "
        "and tightens HBM so more concurrent users fit. Round wall-time then "
        "includes per-key reload cost vs. resident.",
    )
    args = ap.parse_args()

    os.environ.setdefault("ACE_GDS_ENABLED", "0")
    os.environ.setdefault("FHERT_GALOIS_LOAD_THREADS", "1")
    IoPolicySettings(
        policy={"auto": IoPolicy.AUTO, "gds": IoPolicy.GDS, "staged": IoPolicy.STAGED, "mapped": IoPolicy.MAPPED}[
            args.io
        ]
    ).apply()

    root = os.path.abspath(args.key_root)
    with open(os.path.join(root, "meta.json")) as f:
        meta = json.load(f)
    if meta.get("spec") != "resnet20":
        print(f"ERROR: meta.json spec='{meta.get('spec')}', expected 'resnet20'.")
        sys.exit(1)
    all_user_dirs = [u["key_dir"] for u in meta["user_key_dirs"]]
    n = min(args.num_users, len(all_user_dirs))
    user_dirs = all_user_dirs[:n]
    user_ids = [f"user_{u['user']}" for u in meta["user_key_dirs"][:n]]
    if n < 2:
        print("ERROR: Server-Parallel needs >= 2 users.")
        sys.exit(1)

    if not gpu_available():
        print("ERROR: GPU not available. This example requires CUDA.")
        sys.exit(1)

    print("=" * 70)
    print(f"ResNet-20 Multi-User (Server-Parallel) — {n} users, {args.rounds} rounds")
    print(f"  release: {'ON (evict galois between rounds)' if args.release else 'off (keys resident)'}")
    print("=" * 70)

    # --- 1. Compile resnet20 once (fresh kernel+config; ignores stale meta paths) ---
    # Backend follows meta (set by generate_keys --backend) so the recompiled
    # kernel matches the key bundle's backend.
    print("  compiling resnet20 ...", flush=True)
    t0 = time.perf_counter()
    package = _compile_once(meta.get("backend", "phantom"))
    print(f"  compiled ({time.perf_counter() - t0:.1f}s); kernel: {package['kernel']}")

    # --- 2. Primary FHERuntime on user_0 (session idx 0) ---
    pkg = {**package, "key_path": user_dirs[0], "key_mode": "preload_full"}
    t0 = time.perf_counter()
    runner = fhe.FHERuntime(pkg)
    runner.init()
    print(f"  primary {user_ids[0]} init={time.perf_counter() - t0:5.1f}s  (idx=0)", flush=True)

    session_idx = {user_ids[0]: 0}

    # --- 3. Resident extra sessions (own evaluator + stream + bootstrap mgr) ---
    for uid, kd in zip(user_ids[1:], user_dirs[1:]):
        t0 = time.perf_counter()
        idx = runner.create_session(kd)
        if idx < 0:
            print(f"  !! create_session({uid}) failed (returned {idx})")
            runner.finalize()
            sys.exit(1)
        session_idx[uid] = idx
        print(f"  create  {uid} idx={idx} total={time.perf_counter() - t0:5.1f}s", flush=True)

    # --- 4. Same CIFAR input through every session (so outputs must agree) ---
    cifar_imgs, _ = load_cifar10_images(n)
    x = (cifar_imgs[0:1].float(),)

    # --- 5. Warm-up (one run to absorb lazy kernel/NTT setup) ---
    # NOTE: a full serial reference (run_session per session, sequentially) used
    # to run here as the correctness baseline. It is omitted by default now —
    # 03_server_serial.py already covers shared-context serial serving, and
    # re-running it here just duplicates that cost. The serial block is kept
    # commented below for one-off comparison if you ever need par-vs-serial on
    # the exact same setup:
    #
    # print("\n--- serial reference (one thread, sequential) ---", flush=True)
    # serial_out = {}
    # for uid in user_ids:
    #     idx = session_idx[uid]
    #     t0 = time.perf_counter()
    #     out = runner.run_session(idx, *x)
    #     serial_out[uid] = out.float().reshape(-1)
    #     print(f"  serial {uid} (idx={idx}) {time.perf_counter() - t0:5.1f}s -> {serial_out[uid][:3].tolist()}", flush=True)
    _ = runner.run_session(0, *x)  # warm-up only

    # --- 6. Parallel: N worker threads, each run_session(idx) concurrently ---
    print(f"\n--- parallel ({n} threads, real overlap) ---", flush=True)
    all_ok = True
    for rnd in range(args.rounds):
        t0 = time.perf_counter()
        with ThreadPoolExecutor(max_workers=n) as ex:
            futs = {uid: ex.submit(runner.run_session, session_idx[uid], *x) for uid in user_ids}
            par_out = {uid: fut.result().float().reshape(-1) for uid, fut in futs.items()}
        wall = time.perf_counter() - t0

        round_ok = True
        anchor = user_ids[0]
        for uid in user_ids[1:]:
            # Parallel outputs must agree across sessions (same input, different
            # keys -> same output). Replaces the former par-vs-serial check now
            # that the serial reference is commented out (see section 5).
            ok = torch.allclose(par_out[uid], par_out[anchor], atol=0.1)
            if not ok:
                round_ok = all_ok = False
                diff = (par_out[uid] - par_out[anchor]).abs().max().item()
                print(f"    !! {uid} mismatch vs {anchor}: max_diff={diff:.4g}")
        status = "all agree ✓" if round_ok else "MISMATCH"

        # --release: evict every session's galois HBM now so it isn't resident
        # during the next round. run_session re-pulls each key on demand, so the
        # *next* round's wall-time reflects the reload cost. Measured HERE so the
        # release cost itself is visible (and the active session workspace is
        # reset before the next run_session burst).
        hbm_tag = ""
        if args.release:
            rel_t = time.perf_counter()
            free0, tot = torch.cuda.mem_get_info()
            for uid in user_ids:
                rc_id = runner.release_session(session_idx[uid])
                if rc_id < 0:
                    print(f"    !! release_session({uid} idx={session_idx[uid]}) failed rc={rc_id}")
            free1, _ = torch.cuda.mem_get_info()
            hbm_tag = f"  release={(time.perf_counter() - rel_t) * 1e3:.0f}ms hbm≈{(tot - free0) // (1024 * 1024)}→{(tot - free1) // (1024 * 1024)}MiB"

        print(
            f"  round {rnd}: wall={wall:5.1f}s  {status}{hbm_tag}  "
            f"{' / '.join(f'{uid}={par_out[uid][:1].tolist()}' for uid in user_ids)}",
            flush=True,
        )

    runner.finalize()

    # --- 7. Summary ---
    print()
    print("=" * 70)
    print("Summary")
    print("=" * 70)
    print(f"  users / rounds:  {n} / {args.rounds}")
    print(f"  all parallel outputs agree:  {all_ok}")
    print("  (per-image serial/parallel timings printed above)")
    print("=" * 70)
    if not all_ok:
        sys.exit(1)


if __name__ == "__main__":
    main()
