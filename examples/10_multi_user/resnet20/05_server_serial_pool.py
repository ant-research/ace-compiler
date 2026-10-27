#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
ResNet-20 Multi-User Inference — Server-Serial mode (formerly Mode B; single-process native session hot-switch).

One process, one GPU, one shared FheContext (built once, never rebuilt). N key
bundles are each loaded into their own resident FheSession in HBM via
``FHERuntime.create_session(key_dir)``; switching users is a microsecond-scale
evaluator key rebind via ``FHERuntime.switch_session(idx)`` — no context teardown,
no galois-key re-read. This is the native C++ session path exposed by commit
0436af0; the architectural alternative to Batch mode's ~per-user context rebuild.

Contrast with Batch (resnet20/01_batch.py): there every switch finalizes the
FheContext and rebuilds it on the new key_dir, so each user pays context-build
(~11s) + galois-key load (~22 GiB read) on EVERY switch. Server-Serial pays those once
(context: 1×; key load: 1× per user at create_session), then switches are ~free.

Memory note: resnet20 galois keys are ~22 GiB/user. Server-Serial holds N users' keys
resident simultaneously in HBM (shared GaloisTool, but N galois-key sets):
  3 users ≈ 3×22 + context ≈ ~70 GiB  →  fits a 96/80 GiB card, but tight.
Pass ``--num-users`` / use a key root with the matching number of bundles.

Flow
----
1. Compile resnet20 once (fresh /tmp kernel+config; meta.json paths are ignored
   so stale /tmp paths after a container restart don't matter).
2. Build one FHERuntime on user_0's key_dir (primary session, idx 0) → init()
   (timed: context build + primary key load).
3. For each extra user: create_session(key_dir) — full key load into HBM (timed).
   Resident thereafter.
4. Cycle users: switch_session(idx) (timed, µs) → run() (timed, s).
5. Cross-user output agreement (same CIFAR input, different keys → same output).

Inputs MUST be normalized CIFAR-10 images (see memory resnet20-input-must-be-cifar):
the ReLU VR profile is fit on normalized-CIFAR ranges; raw randn makes the ReLU
polynomial explode.

NOTE: extra sessions must use the same galois elements as the primary (true for
same-model keys). The shared GaloisTool is not rebuilt.

Usage:
    CUDA_VISIBLE_DEVICES=4 python3 examples/10_multi_user/resnet20/03_server_serial.py \
        --key-root /mnt/nvme0/mu_keys_resnet20 --rounds 2
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
from ace.model.spec_resnet import RESNET20_CIFAR10 as SPEC


# generate_keys._compile_once lives in the same dir — reuse for a fresh kernel.
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from generate_keys import _compile_once


# multi_user_runtime lives in the parent dir (10_multi_user/), shared with small group
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))


def main():
    parser = argparse.ArgumentParser(
        description="ResNet-20 Multi-User Inference — Server-Serial (native session hot-switch)"
    )
    parser.add_argument(
        "--key-root", required=True, help="Key-root dir from resnet20/generate_keys.py (contains meta.json)"
    )
    parser.add_argument("--rounds", type=int, default=3, help="Times to cycle through all users (default: 3)")
    parser.add_argument(
        "--num-users",
        type=int,
        default=None,
        help="Use only the first N users' key bundles (default: all in meta.json)",
    )
    parser.add_argument(
        "--io", choices=["auto", "gds", "staged", "mapped"], default="staged", help="IO policy (default: staged)"
    )
    parser.add_argument(
        "--pool-size",
        type=int,
        default=None,
        help="Key-pool mode: build only K resident sessions (key bundles k0..k(K-1)) "
        "and serve N logical users by mapping logical user i -> session i % K. "
        "Lets N > K users be served from K key sets' HBM (e.g. 8 users on K=3, "
        "like k0 k1 k2 k0 k1 k2 k0 k1) — bypasses the per-user HBM ceiling. "
        "Default: None = one session per user (original behaviour).",
    )
    parser.add_argument(
        "--num-logical-users",
        type=int,
        default=None,
        help="Number of logical users served (in --pool-size mode). Default: same as "
        "the number sessions would be without pooling. Drives the serve loop "
        "(rounds cycle logical users 0..N-1, each switching to i % K).",
    )
    parser.add_argument(
        "--key-mode",
        choices=["preload_full", "on_demand_full"],
        default="preload_full",
        help="How each session's keys are made resident. preload_full loads the "
        "full galois set at init (~22 GiB/user). on_demand_full preloads "
        "SK/PK/RLK but defers galois keys — each rotate lazily pulls the "
        "one needed key into HBM via Ensure_galois_key (KeyMode::ON_DEMAND). "
        "Run the same key root under both modes to compare HBM and verify "
        "output parity (default: preload_full).",
    )
    parser.add_argument(
        "--hbm",
        action="store_true",
        help="Print peak-resident GPU HBM after primary init (for on-demand vs preload comparison).",
    )
    args = parser.parse_args()

    os.environ.setdefault("ACE_GDS_ENABLED", "0")
    os.environ.setdefault("FHERT_GALOIS_LOAD_THREADS", "1")
    IoPolicySettings(
        policy={"auto": IoPolicy.AUTO, "gds": IoPolicy.GDS, "staged": IoPolicy.STAGED, "mapped": IoPolicy.MAPPED}[
            args.io
        ]
    ).apply()

    # Apply key mode globally before any FheContext/KeyMgr::Init runs — both the
    # primary and every create_session pick it up via the process-global key_mode.
    from ace import runtime as _C
    from ace.fhe.config import KeyMode

    KeyMode.apply(args.key_mode)
    print(f"  key_mode = {KeyMode.get()}")

    root = os.path.abspath(args.key_root)
    with open(os.path.join(root, "meta.json")) as f:
        meta = json.load(f)
    if meta.get("spec") != "resnet20":
        print(f"ERROR: meta.json spec='{meta.get('spec')}', expected 'resnet20'.")
        sys.exit(1)

    all_user_dirs = [u["key_dir"] for u in meta["user_key_dirs"]]
    pool_mode = args.pool_size is not None
    if pool_mode:
        K = args.pool_size
        if K < 1 or len(all_user_dirs) < K:
            print(f"ERROR: --pool-size {K} must be in [1, {len(all_user_dirs)}] (key bundles available).")
            sys.exit(1)
        # Sessions map 1:1 to the first K key bundles.
        user_dirs = all_user_dirs[:K]
        user_ids = [f"user_{meta['user_key_dirs'][i]['user']}" for i in range(K)]
        num_logical = args.num_logical_users
        if num_logical is None:
            num_logical = K
        if num_logical < K:
            print(f"NOTE: --num-logical-users {num_logical} < pool-size {K}; serving only that many.")
        # Logical user i -> session i % K.
        logical_uids = [f"logical_user_{i}" for i in range(num_logical)]
        num_users = num_logical  # for the header / summary
    else:
        n = args.num_users or len(all_user_dirs)
        user_dirs = all_user_dirs[:n]
        user_ids = [f"user_{u['user']}" for u in meta["user_key_dirs"][:n]]
        num_users = len(user_dirs)
    if not gpu_available():
        print("ERROR: GPU not available. This example requires CUDA.")
        sys.exit(1)

    print("=" * 70)
    if pool_mode:
        print(
            f"ResNet-20 Multi-User (Server-Serial, KEY POOL) — {num_users} logical users on K={args.pool_size} key sets, {args.rounds} rounds"
        )
    else:
        print(f"ResNet-20 Multi-User (Server-Serial) — {num_users} users, {args.rounds} rounds")
    print("=" * 70)

    # --- 1. Compile resnet20 once (fresh kernel+config; ignores stale meta paths) ---
    # Backend follows meta (set by generate_keys --backend) so the recompiled
    # kernel matches the key bundle's backend.
    print("  compiling resnet20 ...")
    t0 = time.perf_counter()
    package = _compile_once(meta.get("backend", "phantom"))
    print(f"  compiled ({time.perf_counter() - t0:.1f}s)")
    print(f"  kernel: {package['kernel']}")

    # --- 2. Primary FHERuntime on user_0 key dir (idx 0) ---
    pkg = {**package, "key_path": user_dirs[0], "key_mode": args.key_mode}
    t0 = time.perf_counter()
    runner = fhe.FHERuntime(pkg)
    t0i = time.perf_counter()
    runner.init()
    primary_init_total = time.perf_counter() - t0i
    primary_build = time.perf_counter() - t0
    primary_load = runner.get_galois_load_time_seconds()
    primary_init = primary_init_total - primary_load
    print(
        f"  primary {user_ids[0]:8s} build(ctx+load)={primary_build:5.2f}s "
        f"load={primary_load:5.2f}s init={primary_init:5.2f}s  (idx=0)"
    )
    if args.hbm:
        try:
            free, total = torch.cuda.mem_get_info()
            print(
                f"  [hbm] after primary init: resident ≈ {(total - free) // (1024 * 1024)} MiB "
                f"(galois {'lazy-empty' if args.key_mode == 'on_demand_full' else 'full-set resident'})"
            )
        except Exception as e:
            print(f"  [hbm] mem_get_info failed: {e}")

    session_idx = {user_ids[0]: 0}

    # --- 3. Resident extra sessions (one per remaining user) ---
    create_times: dict[str, float] = {}
    create_loads: dict[str, float] = {}
    for uid, kd in zip(user_ids[1:], user_dirs[1:]):
        t0 = time.perf_counter()
        idx = runner.create_session(kd)
        dt = time.perf_counter() - t0
        if idx < 0:
            print(f"  !! create_session({uid}) failed (returned {idx}); C++ Server-Serial path missing?")
            runner.finalize()
            sys.exit(1)
        session_idx[uid] = idx
        gk = runner.get_galois_load_time_seconds()
        create_times[uid] = dt
        create_loads[uid] = gk
        print(f"  create  {uid:8s} idx={idx} total={dt:5.2f}s galois_load={gk:5.2f}s")

    # --- 4. Inputs (normalized CIFAR) ---
    from ace.model.dataset import load_cifar10_images

    n_imgs = max(args.rounds, 1)
    cifar_imgs, _ = load_cifar10_images(n_imgs)

    # --- 5. Warm-up run under primary (first run pays lazy setup) ---
    runner.switch_session(0)
    x0 = (cifar_imgs[0:1].float(),)
    _ = runner.run(*x0)

    # --- 6. Cycle users: switch (timed) → run (timed) ---
    switch_times: dict[str, list[float]] = {uid: [] for uid in user_ids}
    infer_times: dict[str, list[float]] = {uid: [] for uid in user_ids}
    all_ok = True
    t0_total = time.perf_counter()

    for r in range(args.rounds):
        x = (cifar_imgs[r : r + 1].float(),)
        round_outputs: dict[str, torch.Tensor] = {}

        # Logical users 0..N-1, each switches to session i % K (key pool).
        serve_uids = list(enumerate(logical_uids)) if pool_mode else [(session_idx[uid], uid) for uid in user_ids]

        for idx, uid in serve_uids:
            if pool_mode:
                idx = session_idx[user_ids[idx % args.pool_size]]
            t0 = time.perf_counter()
            rc = runner.switch_session(idx)
            sw = time.perf_counter() - t0
            if rc < 0:
                print(f"  !! switch_session({uid}, idx={idx}) failed")
                all_ok = False
                continue
            t1 = time.perf_counter()
            out = runner.run(*x)
            inf = time.perf_counter() - t1
            uid_key = uid
            switch_times.setdefault(uid_key, []).append(sw)
            infer_times.setdefault(uid_key, []).append(inf)
            round_outputs[uid_key] = out.float().reshape(out.shape)
            extra = f" (key=k{idx})" if pool_mode else ""
            print(f"  round {r} {uid:16s} switch={sw * 1e6:7.1f}µs infer={inf:5.2f}s{extra}")

        # Cross-user agreement (same input, different keys -> same output, key
        # bundle hash aside). In pool mode, logical users sharing a key pool slot
        # must produce IDENTICAL output (i % K equal); distinct slots still agree
        # same-input/different-keys like the original).
        atol, rtol = 1.0, 0.1
        if pool_mode:
            anchor = logical_uids[0]
            anchor_idx = 0 % args.pool_size
            for i, uid in enumerate(logical_uids):
                if uid not in round_outputs:
                    continue
                slot = i % args.pool_size
                if slot == anchor_idx:
                    # same key pool slot -> expect near-exact match
                    if not torch.allclose(round_outputs[uid], round_outputs[anchor], atol=0.5, rtol=0.05):
                        all_ok = False
                        diff = (round_outputs[uid] - round_outputs[anchor]).abs().max().item()
                        print(f"    !! {uid} (slot {slot}) mismatch vs {anchor}: max_abs_diff={diff:.4g}")
                else:
                    if not torch.allclose(round_outputs[uid], round_outputs[anchor], atol=atol, rtol=rtol):
                        all_ok = False
                        diff = (round_outputs[uid] - round_outputs[anchor]).abs().max().item()
                        print(f"    !! {uid} (slot {slot}) mismatch vs {anchor}: max_abs_diff={diff:.4g}")
        else:
            anchor = user_ids[0]
            for uid in user_ids[1:]:
                if uid not in round_outputs:
                    continue
                if not torch.allclose(round_outputs[uid], round_outputs[anchor], atol=atol, rtol=rtol):
                    all_ok = False
                    diff = (round_outputs[uid] - round_outputs[anchor]).abs().max().item()
                    print(f"    !! {uid} mismatch vs {anchor}: max_abs_diff={diff:.4g}")
        if all_ok:
            print(f"  round {r}: all users agree ✓")

    total_s = time.perf_counter() - t0_total
    runner.finalize()

    # --- 7. Summary ---
    print()
    print("=" * 70)
    print("Summary")
    print("=" * 70)
    print(f"  users / rounds:  {num_users} / {args.rounds}")
    print(f"  serve wall:      {total_s:.2f}s  (switches + infers only; setup excluded)")
    print(f"  all correct:     {all_ok}")
    print()
    print(f"  {'user':16s} {'create_s':>9s} {'switch_us':>10s} {'infer_s':>9s}")
    served_uids = logical_uids if pool_mode else user_ids
    for uid in served_uids:
        c = create_times.get(uid, 0.0)
        sw = statistics.mean(switch_times[uid]) * 1e6 if switch_times.get(uid) else 0.0
        inf = statistics.mean(infer_times[uid]) if infer_times.get(uid) else 0.0
        print(f"  {uid:16s} {c:8.2f}s {sw:9.1f}µs {inf:8.2f}s")
    all_sw = [v for vs in switch_times.values() for v in vs]
    if all_sw:
        print(
            f"  {'mean':8s} {'':>9s} {statistics.mean(all_sw) * 1e6:9.1f}µs "
            f"(min={min(all_sw) * 1e6:.1f}µs max={max(all_sw) * 1e6:.1f}µs)"
        )
    print()
    print(f"  primary build (one-time):     {primary_build:.2f}s  (context + key load)")
    if create_times:
        print(
            f"  per-user create (one-time):   {statistics.mean(list(create_times.values())):.2f}s avg "
            f"(22 GiB galois load; resident thereafter)"
        )
    if all_sw:
        print(f"  switch_session latency:       ~{statistics.mean(all_sw) * 1e6:.1f}µs  (key rebind, no IO)")
    print("=" * 70)
    if not all_ok:
        sys.exit(1)


if __name__ == "__main__":
    main()
