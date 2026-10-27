#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
ResNet-20 Multi-User Inference — Batch mode (formerly Strategy A; single-process serial).

One process, one GPU. Serve N users serially: each user = rebuild switch
(finalize old FheContext + construct new on the user's key_dir) → init → infer.
Per-user load / init / infer are timed separately; total wall = pure service.

This is the non-server / batch baseline (rebuild per user, no shared context),
contrasted with the server modes (Server-Serial / Server-Parallel, sharing one
context) and Dedicated (per-user process). See examples/10_multi_user/report.md
for the mode map.

phantom rebuild-residue is flaky on the small gemm case (resnet20 passed each
run so far). For correct, reliable multi-user serving use Dedicated mode
(resnet20/02_dedicated.py).

Usage:
    CUDA_VISIBLE_DEVICES=7 ACE_GDS_ENABLED=0 FHERT_GALOIS_LOAD_THREADS=1 \
    python3 examples/10_multi_user/resnet20/01_batch.py --key-root /mnt/nvme0/mu_keys_resnet20 --rounds 1
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


# multi_user_runtime lives in the parent dir (10_multi_user/), shared with small group
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))
from multi_user_runtime import MultiUserRuntime


def main():
    parser = argparse.ArgumentParser(description="ResNet-20 Multi-User Inference — Batch mode (rebuild switch)")
    parser.add_argument(
        "--key-root", required=True, help="Key-root dir from resnet20/generate_keys.py (contains meta.json)"
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

    root = os.path.abspath(args.key_root)
    with open(os.path.join(root, "meta.json")) as f:
        meta = json.load(f)
    if meta.get("spec") != "resnet20":
        print(f"ERROR: meta.json spec='{meta.get('spec')}', expected 'resnet20'.")
        sys.exit(1)

    num_users = meta["num_users"]
    package = {"model": SPEC.name, "kernel": meta["kernel"], "config_path": meta.get("config_path", "")}
    if not gpu_available():
        print("ERROR: GPU not available. This example requires CUDA.")
        sys.exit(1)

    mu = MultiUserRuntime(package)
    for u in meta["user_key_dirs"]:
        mu.register_user(f"user_{u['user']}", u["key_dir"])
    user_ids = mu.user_ids

    # Inputs MUST be normalized CIFAR-10 images (not torch.randn): resnet20's
    # ReLU VR profile is fit on normalized-CIFAR activation ranges; raw randn
    # makes the ReLU polynomial explode (±2000, non-deterministic).
    from ace.model.dataset import load_cifar10_images

    cifar_imgs, _ = load_cifar10_images(max(args.rounds, 1))

    print("=" * 60)
    print(f"ResNet-20 Multi-User (Batch) — {num_users} users, {args.rounds} rounds")
    print("=" * 60)

    # Per user: switch → explicit init() (timed: context build + key load) → run() (timed: infer).
    #   load  = 22 GiB galois read (C++ counter)
    #   init  = init_total - load  (PhantomContext build + SK/PK/RLK)
    #   infer = one run()
    per_user_load: dict[str, float] = {}
    per_user_init: dict[str, float] = {}
    per_user_infer: dict[str, float] = {}
    all_ok = True
    t0_total = time.perf_counter()

    for r in range(args.rounds):
        x = (cifar_imgs[r : r + 1].float(),)  # keep batch dim (1,3,32,32)
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
            print(f"  round {r} {uid:8s} load={load:5.2f}s init={init:5.2f}s infer={infer:5.2f}s")

        # Cross-user agreement (same input, different keys → same output).
        atol, rtol = 1.0, 0.1
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
    print(f"  total wall:      {total_s:.1f}s  (pure service, no diagnostic overhead)")
    print(f"  all correct:     {all_ok}")
    print()
    print(f"  {'user':8s} {'load':>7s} {'init':>7s} {'infer':>7s}")
    loads = [per_user_load[u] for u in user_ids]
    inits = [per_user_init[u] for u in user_ids]
    infers = [per_user_infer[u] for u in user_ids]
    for uid in user_ids:
        print(f"  {uid:8s} {per_user_load[uid]:7.2f} {per_user_init[uid]:7.2f} {per_user_infer[uid]:7.2f}")
    if len(user_ids) > 1:
        print(
            f"  {'mean':8s} {statistics.mean(loads):7.2f} {statistics.mean(inits):7.2f} {statistics.mean(infers):7.2f}"
        )
    print("=" * 60)
    if not all_ok:
        sys.exit(1)


if __name__ == "__main__":
    main()
