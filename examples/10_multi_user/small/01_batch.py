#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Multi-User Inference — Batch mode (formerly Mode A; single-process serial), small case (gemm/linear/add).

One process, one GPU. Serve N users serially: each user = rebuild switch
(finalize old FheContext + construct new on the user's key_dir) → init → infer.
Per-user load / init / infer timed separately; total wall = pure service.

phantom rebuild-residue is flaky here (the small gemm case is the xfail anchor).
For correct, reliable multi-user serving use Dedicated mode (small/02_dedicated.py).

Usage:
    python3 examples/10_multi_user/small/01_batch.py --key-root /tmp/mu_keys --rounds 1
"""

import argparse
import json
import os
import statistics
import sys
import time

import torch

from ace.fhe.config import IoPolicy, IoPolicySettings
from ace.fhe.util import gpu_available
from ace.sample.ops.specs import ADD_OP, GEMM_49X3_OP, LINEAR_BIAS_OP


# multi_user_runtime lives in the parent dir (10_multi_user/), shared with resnet20 group
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from multi_user_runtime import MultiUserRuntime


SPECS = {"gemm_49x3": GEMM_49X3_OP, "linear_bias": LINEAR_BIAS_OP, "add_op": ADD_OP}


def main():
    parser = argparse.ArgumentParser(description="Multi-User FHE inference — Batch (rebuild switch)")
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
    package = {"model": spec.name, "kernel": meta["kernel"], "config_path": meta.get("config_path", "")}
    if not gpu_available():
        print("ERROR: GPU not available. This example requires CUDA.")
        sys.exit(1)

    mu = MultiUserRuntime(package)
    for u in meta["user_key_dirs"]:
        mu.register_user(f"user_{u['user']}", u["key_dir"])
    user_ids = mu.user_ids

    print("=" * 60)
    print(f"Multi-User (Batch) — {spec.name}, {num_users} users, {args.rounds} rounds")
    print("=" * 60)

    # Per user: switch → explicit init() (timed: context build + key load) → run() (timed: infer).
    #   load  = galois read (C++ counter)
    #   init  = init_total - load  (PhantomContext build + SK/PK/RLK)
    #   infer = one run()
    per_user_load: dict[str, float] = {}
    per_user_init: dict[str, float] = {}
    per_user_infer: dict[str, float] = {}
    all_ok = True
    t0_total = time.perf_counter()

    for r in range(args.rounds):
        x = tuple(torch.randn(t.shape, dtype=t.dtype) for t in spec.example_inputs)
        round_outputs: dict[str, torch.Tensor] = {}

        for uid in user_ids:
            mu.switch(uid)
            handle = mu._active_handle
            t0 = time.perf_counter()
            handle.init()
            init_total = time.perf_counter() - t0
            load = mu.get_galois_load_time()
            init = init_total - load
            t1 = time.perf_counter()
            out = mu.run(*x)
            infer = time.perf_counter() - t1
            per_user_load[uid], per_user_init[uid], per_user_infer[uid] = load, init, infer
            round_outputs[uid] = out.float().reshape(out.shape)
            print(f"  round {r} {uid:8s} load={load * 1e3:6.0f}ms init={init * 1e3:6.0f}ms infer={infer * 1e3:6.0f}ms")

        # Cross-user agreement (same input, different keys → same output).
        atol, rtol = 0.1, 0.05
        anchor = user_ids[0]
        for uid in user_ids[1:]:
            if not torch.allclose(round_outputs[uid], round_outputs[anchor], atol=atol, rtol=rtol):
                all_ok = False
                diff = (round_outputs[uid] - round_outputs[anchor]).abs().max().item()
                print(f"    !! {uid} mismatch vs {anchor}: max_abs_diff={diff:.4g}")
        if all_ok:
            print(f"  round {r}: all users agree ✓")

    total_s = time.perf_counter() - t0_total
    mu.close()

    print()
    print("=" * 60)
    print("Summary")
    print("=" * 60)
    print(f"  users / rounds:  {num_users} / {args.rounds}")
    print(f"  total wall:      {total_s:.2f}s  (pure service, no diagnostic overhead)")
    print(f"  all correct:     {all_ok}")
    print()
    print(f"  {'user':8s} {'load':>9s} {'init':>9s} {'infer':>9s}")
    loads = [per_user_load[u] for u in user_ids]
    inits = [per_user_init[u] for u in user_ids]
    infers = [per_user_infer[u] for u in user_ids]
    for uid in user_ids:
        print(
            f"  {uid:8s} {per_user_load[uid] * 1e3:8.0f}ms {per_user_init[uid] * 1e3:8.0f}ms {per_user_infer[uid] * 1e3:8.0f}ms"
        )
    if len(user_ids) > 1:
        print(
            f"  {'mean':8s} {statistics.mean(loads) * 1e3:8.0f}ms {statistics.mean(inits) * 1e3:8.0f}ms {statistics.mean(infers) * 1e3:8.0f}ms"
        )
    print("=" * 60)
    if not all_ok:
        sys.exit(1)


if __name__ == "__main__":
    main()
