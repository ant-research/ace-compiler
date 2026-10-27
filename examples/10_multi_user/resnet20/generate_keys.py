#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
ResNet-20 multi-user key generation (Strategy C, per-user independent process)

ResNet-20 is the "real model" second step after the small gemm_49x3 case. Each
user's galois key is ~22 GiB, so:

  - key generation MUST run one fresh process per user (same fix as the small
    case: phantom FheContext rebuild residue would otherwise contaminate the
    2nd+ user's keys). See examples/10_multi_user/01_gen_multi_keys.py docstring
    and .claude/plans/multi-user/README.md root-cause section.
  - each process needs ~25 GiB HBM → each user MUST get its own GPU (--gpus).
  - key output should live on a real filesystem with enough space (66 GiB for
    3 users) — default /mnt/nvme0; /tmp overlayfs works but is RAM-backed.

Compile: uses resnet20's declared compile_options (vec.conv_parl/ssf, ckks
N=65536 q0=60 sf=56 icl=17) and its ReLU VR profile (profiles/resnet20_cifar10.json).
profile_relu=True is NOT used — resnet20 needs the precomputed VR profile for
correct accuracy and the right memory footprint.

Usage:
    python3 04_resnet20_gen_keys.py --num-users 3 --gpus 4 5 7 --key-root /mnt/nvme0/mu_keys_resnet20
    python3 04_resnet20_gen_keys.py --num-users 2 --gpus 4 5 --key-root /mnt/nvme0/mu_keys_resnet20

Prereqs: CUDA GPUs (one free 80GiB+ card per user), phantom backend, resnet20
weights + VR profile (both shipped under fhe_dsl/python/model/resnet/).
"""

import argparse
import glob
import json
import multiprocessing as mp
import os
import sys
import time

import torch

from ace import fhe
from ace.fhe.util import gpu_available
from ace.model.spec_resnet import RESNET20_CIFAR10 as SPEC


# Default key output root: NVMe has the space (66 GiB for 3 users) and survives.
# Override with --key-root.
DEFAULT_ROOT = "/mnt/nvme0/mu_keys_resnet20"


def _compile_once(backend="phantom"):
    """Compile resnet20 once (parent process); returns the package dict shared
    across all user key-generation processes.

    backend: one of the resnet20 spec's compile_options keys — "phantom"
    (default), "ace", or "ant". fhe.compile(library=backend) accepts these."""
    model = SPEC.create_model()
    opts = dict(SPEC.compile_options.get(backend, {}))
    # phantom backend needs p2c.lib set (spec's phantom entry omits p2c); the
    # other backends carry their p2c in the spec entry.
    if backend == "phantom":
        opts.setdefault("p2c", {})["lib"] = "phantom"
    vr = SPEC.get_vr_profile()
    compiled = fhe.compile(
        frontend="torch",
        library=backend,
        device="cuda",
        encrypt_inputs=SPEC.encrypt_inputs,
        relu_vr_file=vr,
        **opts,
    )(model)
    return compiled.fhe_compile(SPEC.example_inputs).package


def _gen_worker(uid, user_dir, package, io_mode, galois_merged, gpu_id, out_q):
    """Spawned child: generate ONE user's resnet20 key bundle in a fresh process.

    Fresh process → single FheContext → no rebuild residue → clean keys.
    """
    import importlib

    if gpu_id is not None:
        os.environ["CUDA_VISIBLE_DEVICES"] = str(gpu_id)
    # Required on this cuFile-false host (galois load would dead-wait without).
    os.environ.setdefault("ACE_GDS_ENABLED", "0")
    os.environ.setdefault("FHERT_GALOIS_LOAD_THREADS", "1")
    importlib.invalidate_caches()
    from ace import fhe
    from ace import runtime as _C
    from ace.fhe.config import IoConfig, IoPolicy, IoPolicySettings
    from ace.model.spec_resnet import RESNET20_CIFAR10 as spec

    policy_map = {"auto": IoPolicy.AUTO, "gds": IoPolicy.GDS, "staged": IoPolicy.STAGED, "mapped": IoPolicy.MAPPED}
    IoPolicySettings(policy=policy_map[io_mode]).apply()
    IoConfig(galois_merged_format=galois_merged).apply()
    os.makedirs(user_dir, exist_ok=True)

    rc = -1
    galois_files = 0
    galois_bytes = 0
    err = ""
    try:
        _C.set_key_path("")  # ONLINE: generate, don't load
        runner = fhe.FHERuntime(package)
        runner.run(*spec.example_inputs)  # triggers KeyGenerator on GPU
        rc = runner.save_keys(user_dir)
        runner.finalize()
        bins = glob.glob(os.path.join(user_dir, "model", "galois_key.*.bin"))
        merged = os.path.join(user_dir, "model", "galois_key.bin")
        galois_bytes = sum(os.path.getsize(f) for f in bins)
        if not bins and os.path.exists(merged):
            galois_bytes = os.path.getsize(merged)
            bins = [merged]
        galois_files = len(bins)
    except Exception as e:
        import traceback

        err = f"{type(e).__name__}: {e}\n{traceback.format_exc()}"
        rc = -1
    out_q.put({"uid": uid, "rc": rc, "galois_files": galois_files, "galois_bytes": galois_bytes, "err": err})


def main():
    parser = argparse.ArgumentParser(description="ResNet-20 multi-user key generation (Strategy C)")
    parser.add_argument("--num-users", type=int, default=3, help="Number of independent key bundles (default: 3)")
    parser.add_argument(
        "--gpus",
        nargs="+",
        type=int,
        default=None,
        help="GPU id(s) for keygen. Default: reuse the card in CUDA_VISIBLE_DEVICES "
        "(or GPU 0 if unset) for ALL users — single-card parallel, needs "
        "~25 GiB×num_users. For multi-card, give one id per user, e.g. --gpus 4 5 7",
    )
    parser.add_argument("--key-root", default=DEFAULT_ROOT, help=f"Key-root output dir (default: {DEFAULT_ROOT})")
    parser.add_argument(
        "--io",
        choices=["auto", "gds", "staged", "mapped"],
        default="staged",
        help="IO policy (default: staged — host cuFile is false-available here)",
    )
    parser.add_argument(
        "--galois-format",
        choices=["merged", "per-key"],
        default="per-key",
        help="Galois key file format (default: per-key — avoids merged-v3 bulk-read stall on non-NVMe)",
    )
    parser.add_argument(
        "--backend",
        choices=["phantom", "ace", "ant"],
        default="phantom",
        help="Compile/runtime backend (default: phantom). ace backend lacks per-session "
        "routing (Server-Parallel/Serial will crash); Batch/Dedicated may work.",
    )
    args = parser.parse_args()

    # Default: reuse one card for all users (single-card parallel keygen).
    # Picks the card from CUDA_VISIBLE_DEVICES (first id) or GPU 0. Needs
    # ~25 GiB×num_users on that card; if it OOMs, give multi-card ids instead.
    if args.gpus is None:
        cvd = os.environ.get("CUDA_VISIBLE_DEVICES", "")
        single = int(cvd.split(",")[0]) if cvd else 0
        args.gpus = [single] * args.num_users
    elif len(args.gpus) != args.num_users:
        print(
            f"ERROR: --gpus gave {len(args.gpus)} GPUs but --num-users={args.num_users}. "
            f"Give one id per user (multi-card) or omit --gpus (single-card reuse)."
        )
        sys.exit(1)

    root = os.path.abspath(args.key_root)
    if not gpu_available():
        print("ERROR: GPU not available. This example requires CUDA.")
        sys.exit(1)

    free = os.popen("nvidia-smi --query-gpu=memory.used --format=csv,noheader,nounits").read()
    used = {int(ln.split(",")[0]): int(ln.split(",")[1]) for ln in free.strip().splitlines() if "," in ln}
    busy = [g for g in args.gpus if used.get(g, 0) > 2000]
    if busy:
        print(f"ERROR: GPUs {busy} are not free (>2 GiB used). ResNet-20 key gen needs ~25 GiB per card.")
        print("       Pick idle cards: nvidia-smi --query-gpu=index,memory.used --format=csv,noheader")
        sys.exit(1)

    # Space check: ~22 GiB galois + a few GiB SK/PK/RLK per user.
    needed_gb = args.num_users * 24
    try:
        st = os.statvfs(root if os.path.isdir(root) else os.path.dirname(root) or ".")
        free_gb = (st.f_bavail * st.f_frsize) / (1024**3)
        if free_gb < needed_gb:
            print(f"ERROR: {root} has only {free_gb:.0f} GiB free; need ~{needed_gb} GiB for {args.num_users} users.")
            sys.exit(1)
    except OSError:
        pass  # skip if path not yet created

    print("=" * 60)
    print(f"ResNet-20 Multi-User Key Generation — {args.num_users} users")
    print("=" * 60)
    print("  Model:      resnet20_cifar10  (N=65536, ~22 GiB galois/user)")
    print(f"  Key root:   {root}")
    print(f"  GPUs:       {args.gpus}  (one per user)")
    print(f"  IO/format:  {args.io} / {args.galois_format}")
    print()

    # ── Compile once (parent) ──
    print("[1/3] Compiling resnet20 ...")
    t0 = time.time()
    package = _compile_once(args.backend)
    print(f"      kernel: {package['kernel']}")
    print(f"      config: {package.get('config_path', '')}")
    print(f"      Done ({time.time() - t0:.1f}s)")
    print()

    # ── Spawn one generation process per user ──
    os.makedirs(root, exist_ok=True)
    print(f"[2/3] Spawning {args.num_users} key-generation processes (one GPU each) ...")
    ctx = mp.get_context("spawn")
    out_q = ctx.Queue()
    user_dirs = [os.path.join(root, f"user_{i}") for i in range(args.num_users)]
    procs = []
    t_gen0 = time.time()
    for i, ud in enumerate(user_dirs):
        p = ctx.Process(
            target=_gen_worker, args=(i, ud, package, args.io, args.galois_format == "merged", args.gpus[i], out_q)
        )
        p.start()
        procs.append(p)

    results = {}
    for _ in procs:
        r = out_q.get(timeout=900)  # resnet20 gen can take a few minutes
        results[r["uid"]] = r
    for p in procs:
        p.join(timeout=30)
        if p.is_alive():
            p.terminate()

    shared_meta = {
        "spec": "resnet20",
        "model_name": "resnet20_cifar10",
        "backend": args.backend,
        "input_shape": list(SPEC.example_inputs[0].shape),
        "encrypt_inputs": SPEC.encrypt_inputs,
        "kernel": package["kernel"],
        "config_path": package.get("config_path", ""),
        "num_users": args.num_users,
        "user_key_dirs": [],
    }
    all_ok = True
    for i, ud in enumerate(user_dirs):
        r = results.get(i)
        if r is None or r["rc"] != 0:
            all_ok = False
            print(f"  user_{i}: FAILED {r['err'] if r else 'no result'}")
            continue
        shared_meta["user_key_dirs"].append(
            {"user": i, "key_dir": ud, "galois_key_files": r["galois_files"], "galois_key_bytes": r["galois_bytes"]}
        )
        print(f"  user_{i}: galois {r['galois_files']} files, {r['galois_bytes'] / 1024**3:.2f} GiB -> {ud}")
    print(f"      generation total: {time.time() - t_gen0:.1f}s")

    meta_path = os.path.join(root, "meta.json")
    with open(meta_path, "w") as f:
        json.dump(shared_meta, f, indent=2)
    print(f"[3/3] Wrote shared metadata: {meta_path}")

    print()
    print("=" * 60)
    if all_ok:
        total = sum(u["galois_key_bytes"] for u in shared_meta["user_key_dirs"])
        print("All ResNet-20 key bundles generated successfully!")
        print(f"  Users:        {args.num_users}")
        print(
            f"  Galois/user:  {shared_meta['user_key_dirs'][0]['galois_key_files']} files, "
            f"{shared_meta['user_key_dirs'][0]['galois_key_bytes'] / 1024**3:.2f} GiB"
        )
        print(f"  Total galois: {total / 1024**3:.2f} GiB")
        print()
        print("Next: run multi-user inference — resnet20/02_dedicated.py (reads meta.json):")
        print("  ACE_GDS_ENABLED=0 FHERT_GALOIS_LOAD_THREADS=1 \\")
        print(
            f"  python3 examples/10_multi_user/resnet20/02_dedicated.py --key-root {root} --gpus {' '.join(map(str, args.gpus))} --inputs <N>"
        )
    else:
        print("Some users FAILED — see above.")
    print("=" * 60)
    if not all_ok:
        sys.exit(1)


if __name__ == "__main__":
    main()
