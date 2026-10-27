#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Multi-User Inference — Dedicated mode (formerly Mode C; multiprocess), small case (gemm/linear/add).

Each user = a fresh OS process loading its own key bundle → no FheContext
rebuild within a process → no phantom rebuild-residue → correct, cross-user-
agreeing outputs.

--gpus selects the card(s); all workers start in parallel:
  - one GPU id  → all workers share that card (single-card parallel; small-case
                  HBM is tiny, so many workers fit on one card)
  - N GPU ids   → workers distributed across cards (uid % len(gpus))

Per-user load / init / infer are timed in the worker; total wall = spawn +
init + infer.

Usage:
    python3 examples/10_multi_user/small/02_dedicated.py --key-root /tmp/mu_keys --gpus 7 --inputs 4
    python3 examples/10_multi_user/small/02_dedicated.py --key-root /tmp/mu_keys --gpus 5 6 7 --inputs 4
"""

from __future__ import annotations

import argparse
import json
import multiprocessing as mp
import os
import sys
import time

import torch

from ace.sample.ops.specs import ADD_OP, GEMM_49X3_OP, LINEAR_BIAS_OP


SPECS = {"gemm_49x3": GEMM_49X3_OP, "linear_bias": LINEAR_BIAS_OP, "add_op": ADD_OP}


def _worker(user_idx, key_dir, kernel, config_path, spec_name, input_arrays, gpu_id, out_q):
    """Spawned child (one per user): load this user's keys, run inferences."""
    os.environ["CUDA_VISIBLE_DEVICES"] = str(gpu_id)
    os.environ.setdefault("ACE_GDS_ENABLED", "0")
    os.environ.setdefault("FHERT_GALOIS_LOAD_THREADS", "1")
    import importlib

    importlib.invalidate_caches()
    from ace import fhe
    from ace.fhe.config import IoPolicy, IoPolicySettings

    spec = SPECS[spec_name]
    IoPolicySettings(policy=IoPolicy.STAGED).apply()

    try:
        package = {
            "model": spec.name,
            "kernel": kernel,
            "config_path": config_path,
            "key_path": key_dir,
            "key_mode": "preload_full",
        }
        runner = fhe.FHERuntime(package)
        # Three-way timing: init_total (context build + all keys), load (galois read,
        # C++ counter), infer (run loop, excludes init/load).
        t0 = time.perf_counter()
        runner.init()
        init_total = time.perf_counter() - t0
        load = runner.get_galois_load_time_seconds()

        outs = []
        t1 = time.perf_counter()
        for inp in input_arrays:
            x = tuple(torch.tensor(a, dtype=torch.float32) for a in inp)
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
                "load": float(load),
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


def _launch(ctx, out_q, meta, shared_inputs, spec_name, gpus):
    """Spawn one worker per user, all started in parallel on gpus[uid % len(gpus)]."""
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
                spec_name,
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
        r = out_q.get(timeout=600)
        results[r["user"]] = r
    for p in procs:
        p.join(timeout=30)
        if p.is_alive():
            p.terminate()
    return results


def main():
    parser = argparse.ArgumentParser(description="Multi-User FHE inference — Dedicated (multiprocess)")
    parser.add_argument(
        "--key-root", required=True, help="Key-root dir from small/generate_keys.py (contains meta.json)"
    )
    parser.add_argument(
        "--gpus",
        nargs="+",
        type=int,
        default=None,
        help="GPU id(s). Default: reuse CUDA_VISIBLE_DEVICES card (or GPU 0) — "
        "all workers share that card. Multiple → distributed across cards. "
        "e.g. --gpus 7 or --gpus 5 6 7",
    )
    parser.add_argument("--inputs", type=int, default=4, help="Inferences per user (default: 4)")
    args = parser.parse_args()

    if args.gpus is None:
        cvd = os.environ.get("CUDA_VISIBLE_DEVICES", "")
        args.gpus = [int(cvd.split(",")[0])] if cvd else [0]

    with open(os.path.join(os.path.abspath(args.key_root), "meta.json")) as f:
        meta = json.load(f)
    spec = SPECS[meta["spec"]]
    num_users = meta["num_users"]

    # Shared plaintext inputs (same across users → outputs must agree within FHE noise).
    torch.manual_seed(4242)
    shared_inputs = [[t.cpu().tolist() for t in spec.example_inputs] for _ in range(args.inputs)]

    print("=" * 60)
    print(f"Multi-User (Dedicated) — {spec.name}, {num_users} users, parallel on {args.gpus}")
    print("=" * 60)

    ctx = mp.get_context("spawn")
    t_start = time.perf_counter()
    results = _launch(ctx, ctx.Queue(), meta, shared_inputs, meta["spec"], args.gpus)
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
            f"  user_{uid}: load={r['load'] * 1e3:.0f}ms init={init * 1e3:.0f}ms infer={r['infer']:.3f}s"
            + ("" if r["kms"] else "  !! keys NOT from KMS")
        )
        if r["kms"] != 1:
            all_ok = False
        if uid != ref_uid:
            for k, (a, b) in enumerate(zip(results[uid]["outs"], results[ref_uid]["outs"])):
                if not torch.allclose(torch.tensor(a), torch.tensor(b), atol=0.1, rtol=0.05):
                    all_ok = False
                    print(f"    !! input {k}: user_{uid} vs user_{ref_uid} DIFF")
    if all_ok:
        print("  all users agree ✓")

    # Summary
    print()
    print("=" * 60)
    print("Summary")
    print("=" * 60)
    print(f"  users / inputs:  {num_users} / {args.inputs}")
    print(f"  wall:            {wall:.2f}s  (parallel on {args.gpus}; includes spawn/init/infer)")
    ok = [results[u] for u in sorted(results) if results[u]["ok"]]
    if ok:
        n = len(ok)
        loads = [r["load"] for r in ok]
        inits = [r["init_total"] - r["load"] for r in ok]
        infers = [r["infer"] for r in ok]
        print(f"  {'user':>8s} {'load':>9s} {'init':>9s} {'infer':>9s}")
        for r in ok:
            print(
                f"  user_{r['user']:<3d} {r['load'] * 1e3:8.0f}ms {(r['init_total'] - r['load']) * 1e3:8.0f}ms {r['infer']:8.3f}s"
            )
        print(f"  {'mean':>8s} {sum(loads) / n * 1e3:8.0f}ms {sum(inits) / n * 1e3:8.0f}ms {sum(infers) / n:8.3f}s")
    print(f"  all correct:     {all_ok}")
    print("=" * 60)
    if not all_ok:
        sys.exit(1)


if __name__ == "__main__":
    main()
