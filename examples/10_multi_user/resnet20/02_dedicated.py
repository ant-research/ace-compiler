#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
ResNet-20 Multi-User Inference — Dedicated mode (formerly Strategy C; multiprocess).

Each user = a fresh OS process loading its own key bundle → no FheContext
rebuild within a process → no phantom rebuild-residue → correct, cross-user-
agreeing outputs.

--gpus selects the card(s); all workers start in parallel:
  - one GPU id  → all workers share that card (single-card parallel; ≤3 fit in 96 GiB)
  - N GPU ids   → workers distributed across cards (uid % len(gpus))

Per-user load / init / infer are timed in the worker; total wall = spawn +
init + infer.

Usage:
    ACE_GDS_ENABLED=0 FHERT_GALOIS_LOAD_THREADS=1 \
    python3 examples/10_multi_user/resnet20/02_dedicated.py --key-root /mnt/nvme0/mu_keys_resnet20 --gpus 7 --inputs 1
"""

from __future__ import annotations

import argparse
import json
import multiprocessing as mp
import os
import sys
import time

import torch


def _worker(user_idx, key_dir, kernel, config_path, input_arrays, gpu_id, out_q):
    """Spawned child (one per user): load resnet20 keys, run inferences."""
    os.environ["CUDA_VISIBLE_DEVICES"] = str(gpu_id)
    os.environ.setdefault("ACE_GDS_ENABLED", "0")
    os.environ.setdefault("FHERT_GALOIS_LOAD_THREADS", "1")
    import importlib

    importlib.invalidate_caches()
    from ace import fhe
    from ace.fhe.config import IoConfig, IoPolicy, IoPolicySettings
    from ace.model.spec_resnet import RESNET20_CIFAR10 as spec

    IoPolicySettings(policy=IoPolicy.STAGED).apply()
    IoConfig(galois_merged_format=False).apply()

    try:
        package = {
            "model": spec.name,
            "kernel": kernel,
            "config_path": config_path,
            "key_path": key_dir,
            "key_mode": "preload_full",
        }
        runner = fhe.FHERuntime(package)
        # Three-way timing: init_total (context build + all keys), galois_load
        # (22 GiB read, C++ counter), infer (run loop, excludes init/load).
        t0 = time.perf_counter()
        runner.init()
        init_total = time.perf_counter() - t0
        galois_load = runner.get_galois_load_time_seconds()

        outs = []
        t1 = time.perf_counter()
        for inp in input_arrays:
            x = (torch.tensor(inp, dtype=torch.float32),)
            out = runner.run(*x)
            outs.append(out.float().reshape(out.shape).clone())
        infer = time.perf_counter() - t1
        loaded = runner.keys_loaded_from_kms()
        runner.finalize()
        out_q.put(
            {
                "user": user_idx,
                "ok": True,
                "outs": [o.tolist() for o in outs],
                "load": float(galois_load),
                "init_total": float(init_total),
                "infer": float(infer),
                "kms": int(loaded),
                "err": "",
            }
        )
    except Exception as e:
        import traceback

        out_q.put(
            {
                "user": user_idx,
                "ok": False,
                "outs": [],
                "load": -1.0,
                "init_total": -1.0,
                "infer": 0.0,
                "kms": 0,
                "err": f"{type(e).__name__}: {e}\n{traceback.format_exc()}",
            }
        )


def _launch(ctx, out_q, meta, shared_inputs, gpus):
    """Spawn one worker per user, all started in parallel. Each worker lands on
    gpus[uid % len(gpus)] — single card → all share it (parallel on one card,
    ≤3 fit in 96 GiB); multiple cards → distributed across them."""
    procs = []
    for u in meta["user_key_dirs"]:
        uid = u["user"]
        p = ctx.Process(
            target=_worker,
            args=(
                uid,
                u["key_dir"],
                meta["kernel"],
                meta.get("config_path", ""),
                shared_inputs,
                gpus[uid % len(gpus)],
                out_q,
            ),
        )
        p.start()
        procs.append(p)
    print(f"[launch] {len(procs)} worker processes started (parallel on {len(gpus)} card(s): {gpus}) ...")
    results = {}
    for _ in procs:
        r = out_q.get(timeout=1200)
        results[r["user"]] = r
    for p in procs:
        p.join(timeout=60)
        if p.is_alive():
            p.terminate()
    return results


def main():
    parser = argparse.ArgumentParser(description="ResNet-20 Multi-User Inference — Dedicated (multiprocess)")
    parser.add_argument(
        "--key-root", required=True, help="Key-root dir from resnet20/generate_keys.py (contains meta.json)"
    )
    parser.add_argument(
        "--gpus",
        nargs="+",
        type=int,
        default=None,
        help="GPU id(s). Default: reuse CUDA_VISIBLE_DEVICES card (or GPU 0) — "
        "all workers share that card (single-card parallel, ≤3 fit in 96 GiB). "
        "Multiple → distributed across cards. e.g. --gpus 7 or --gpus 4 5 7",
    )
    parser.add_argument("--inputs", type=int, default=1, help="Inferences per user (default: 1)")
    args = parser.parse_args()

    if args.gpus is None:
        cvd = os.environ.get("CUDA_VISIBLE_DEVICES", "")
        args.gpus = [int(cvd.split(",")[0])] if cvd else [0]

    with open(os.path.join(os.path.abspath(args.key_root), "meta.json")) as f:
        meta = json.load(f)
    if meta.get("spec") != "resnet20":
        print(f"ERROR: meta.json spec='{meta.get('spec')}', expected 'resnet20'.")
        sys.exit(1)
    num_users = meta["num_users"]

    # Reject non-idle GPUs — resnet20 needs ~24 GiB/process; a busy card risks OOM.
    # (Single-card parallel with N users needs N×24 GiB headroom on that one card.)
    free = os.popen("nvidia-smi --query-gpu=index,memory.used --format=csv,noheader,nounits").read()
    used = {int(ln.split(",")[0]): int(ln.split(",")[1]) for ln in free.strip().splitlines() if "," in ln}
    busy = [g for g in args.gpus if used.get(g, 0) > 2000]
    if busy:
        print(f"ERROR: GPUs {busy} are not free (>2 GiB used). ResNet-20 needs ~24 GiB per process.")
        sys.exit(1)
    if len(args.gpus) == 1 and num_users > 3:
        print(
            f"ERROR: single-card parallel with {num_users} users needs {num_users}×24 GiB > 96 GiB. "
            f"Use more GPUs (--gpus ...) or ≤3 users on one card."
        )
        sys.exit(1)

    # Inputs MUST be normalized CIFAR-10 images (not torch.randn): resnet20's
    # ReLU VR profile is fit on normalized-CIFAR ranges; randn makes ReLU explode.
    from ace.model.dataset import load_cifar10_images

    images, _ = load_cifar10_images(args.inputs)
    shared_inputs = [images[i : i + 1].cpu().tolist() for i in range(args.inputs)]

    print("=" * 60)
    print(f"ResNet-20 Multi-User (Dedicated) — {num_users} users, parallel on {args.gpus}")
    print("=" * 60)

    ctx = mp.get_context("spawn")
    t_start = time.perf_counter()
    results = _launch(ctx, ctx.Queue(), meta, shared_inputs, args.gpus)
    wall = time.perf_counter() - t_start

    # Per-user + cross-user agreement (same input, different keys → same output).
    all_ok = True
    ref_uid = sorted(results)[0]
    print("[result]")
    for uid in sorted(results):
        r = results[uid]
        if not r["ok"]:
            all_ok = False
            print(f"  user_{uid}: FAIL {r['err']}")
            continue
        init = r["init_total"] - r["load"]
        print(
            f"  user_{uid}: load={r['load']:.2f}s init={init:.2f}s infer={r['infer']:.2f}s"
            + ("" if r["kms"] else "  !! keys NOT from KMS (regenerated)")
        )
        if r["kms"] != 1:
            all_ok = False
        if uid != ref_uid and not torch.allclose(
            torch.tensor(results[uid]["outs"]), torch.tensor(results[ref_uid]["outs"]), atol=1.0, rtol=0.1
        ):
            all_ok = False
            print(f"    !! user_{uid} mismatch vs user_{ref_uid}")
    if all_ok:
        print("  all users agree ✓")

    # Summary
    print()
    print("=" * 60)
    print("Summary")
    print("=" * 60)
    print(f"  users / inputs:  {num_users} / {args.inputs}")
    print(f"  wall:            {wall:.1f}s  (parallel on {args.gpus}; includes spawn/init/infer)")
    ok = [results[u] for u in sorted(results) if results[u]["ok"]]
    if ok:
        n = len(ok)
        loads = [r["load"] for r in ok]
        inits = [r["init_total"] - r["load"] for r in ok]
        infers = [r["infer"] for r in ok]
        print(f"  {'user':>8s} {'load':>7s} {'init':>7s} {'infer':>7s}")
        for r in ok:
            print(f"  user_{r['user']:<3d} {r['load']:7.2f} {r['init_total'] - r['load']:7.2f} {r['infer']:7.2f}")
        print(f"  {'mean':>8s} {sum(loads) / n:7.2f} {sum(inits) / n:7.2f} {sum(infers) / n:7.2f}")
    print(f"  all correct:     {all_ok}")
    print("=" * 60)
    if not all_ok:
        sys.exit(1)


if __name__ == "__main__":
    main()
