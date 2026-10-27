#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Multi-User Step 1: Generate N independent FHE key bundles

For multi-user inference, each user must have its OWN secret/public/eval keys
(SK/PK/RLK/GK). The whole point of multi-user switching is that each user
encrypts/decrypts with their own keys while sharing the compiled model and the
crypto context.

This script compiles ONE model, then loops N times: each iteration builds a
fresh FHERuntime in ONLINE mode (which triggers a fresh KeyGenerator with new
randomness), runs one inference to materialise the keys on the GPU, saves them
to a per-user directory, and finalises. Because every FHERuntime is finalised
(and the global key_path/key_mode reset) before the next is created, each user
gets an independent key set.

Why not just call save_keys() N times on the same runtime?  The C++ FheContext
generates its keys ONCE at init; save_keys() only serialises whatever is in
memory.  Calling it twice writes the SAME key material to two directories —
useless for multi-user.  The rebuild-per-user loop below is the only way to get
independent keys from the Python layer today.

  ⚠️ The standalone `ace.kms.generator` module and `ace_tool kms gen-*-keys`
     are STUBS — they write PLACEHOLDER text files, not real keys.  Do NOT use
     them.  The live-runtime path here is the only correct one.

Usage:
    python3 01_gen_multi_keys.py --num-users 3
    python3 01_gen_multi_keys.py --num-users 3 --key-root /mnt/nvme0/multi_user_keys
    python3 01_gen_multi_keys.py --num-users 3 --spec gemm_49x3

Output (per user i):
    <root>/user_0/   global/{secret_key.bin, public_key.bin}
                    model/{relin_key.bin, galois_key.*, galois_key.index.json}
    <root>/meta.json        # shared model/kernel/config info + per-user key dirs

Spec choices (all verified to run end-to-end on phantom/cuda):
    gemm_49x3    Linear(49->3) on flattened 7x7  -- DEFAULT, has rotation keys
    linear_bias  Linear(4->4) with bias
    add_op       x + y                          -- tiny, no rotation keys

Next step:
    python3 examples/10_multi_user/small/01_batch.py --key-root <root>

Requires: CUDA GPU, phantom backend
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
from ace import runtime as _C
from ace.fhe.config import IoConfig, IoPolicy, IoPolicySettings
from ace.fhe.util import gpu_available

# Sample specs that are verified to compile + infer correctly on phantom/cuda.
# gemm_49x3 is preferred: a GEMM exercises relinearization + rotation keys,
# so multi-user switching actually moves meaningful key material (unlike add_op
# which has no galois keys to switch).
from ace.sample.ops.specs import ADD_OP, GEMM_49X3_OP, LINEAR_BIAS_OP


SPECS = {
    "gemm_49x3": GEMM_49X3_OP,
    "linear_bias": LINEAR_BIAS_OP,
    "add_op": ADD_OP,
}


def _apply_io_config(io_mode: str, galois_merged: bool = True):
    """Apply I/O transport policy and galois-key file format.

    Default is ``staged`` (host-staged: fread + cudaMemcpy) which works on any
    filesystem. Use ``gds``/``auto`` only when the key directory lives on a
    real NVMe mount — on tmpfs/regular-fs paths the GDS path falls back AND
    the merged-v3 bulk reader can stall on the fallback.

    ``galois_merged`` selects the on-disk galois-key layout:
      True  → merged multi-file v3 (optimized for GDS queue depth on NVMe)
      False → per-key (one galois_key.NN.bin per relin key; simpler load path,
              no multi-file bulk reader — avoids the merged-read stall on
              non-NVMe filesystems).
    """
    policy_map = {"auto": IoPolicy.AUTO, "gds": IoPolicy.GDS, "staged": IoPolicy.STAGED, "mapped": IoPolicy.MAPPED}
    if io_mode not in policy_map:
        raise ValueError(f"Unknown IO mode: {io_mode}")
    IoPolicySettings(policy=policy_map[io_mode]).apply()
    # Expert-level override: the file format is an IoConfig concern, not policy.
    IoConfig(galois_merged_format=galois_merged).apply()
    gds_status = "available" if _C.gds_available() else "not available"
    fmt = "merged-v3" if galois_merged else "per-key"
    print(f"  IO config: {io_mode} (GDS {gds_status}), galois format: {fmt}")


def _compile_spec(spec, backend="phantom"):
    """Compile a sample ModelSpec with the chosen GPU backend.

    backend: "phantom" (default), "ace", or "ant" — passed to fhe.compile(library=...).
    Uses built-in ReLU profiling (harmless no-op when the spec has no ReLU).
    The compiled kernel (.so) + config (.conf) are reused across all users —
    only the key material differs per user.
    """
    model = spec.create_model()
    compiled = fhe.compile(
        frontend="torch",
        library=backend,
        device="cuda",
        encrypt_inputs=spec.encrypt_inputs,
        profile_relu=True,
    )(model)
    return compiled.fhe_compile(spec.example_inputs)


def _run_once_to_generate_keys(spec, package):
    """Build a fresh ONLINE runtime, run once to generate keys, return runner.

    The caller saves keys from this runner and then MUST call runner.finalize().
    """
    _C.set_key_path("")  # ONLINE: generate, don't load
    runner = fhe.FHERuntime(package)
    runner.run(*spec.example_inputs)
    return runner


def _gen_worker(uid, user_dir, spec_name, package, io_mode, galois_merged, gpu_id, out_q):
    """Spawned child: generate ONE user's key bundle in a fresh process.

    Module-level so it is picklable for multiprocessing.spawn. Running each
    user's generation in its OWN process is the key correctness fix: phantom's
    FheContext rebuild leaves flaky residual state that can corrupt key
    generation in the 2nd+ iteration of a single-process loop (contaminated keys
    get saved, then produce garbage at inference). One fresh process per user →
    each key bundle is generated under a clean, single FheContext → clean keys.
    """
    import importlib

    if gpu_id is not None:
        os.environ["CUDA_VISIBLE_DEVICES"] = str(gpu_id)
    os.environ.setdefault("ACE_GDS_ENABLED", "0")
    os.environ.setdefault("FHERT_GALOIS_LOAD_THREADS", "1")
    importlib.invalidate_caches()
    # Fresh imports so CUDA init happens in THIS process.
    from ace import fhe
    from ace import runtime as _C
    from ace.fhe.config import IoConfig, IoPolicy, IoPolicySettings
    from ace.sample.ops.specs import ADD_OP, GEMM_49X3_OP, LINEAR_BIAS_OP

    _SPECS = {"gemm_49x3": GEMM_49X3_OP, "linear_bias": LINEAR_BIAS_OP, "add_op": ADD_OP}
    spec = _SPECS[spec_name]

    policy_map = {"auto": IoPolicy.AUTO, "gds": IoPolicy.GDS, "staged": IoPolicy.STAGED, "mapped": IoPolicy.MAPPED}
    IoPolicySettings(policy=policy_map[io_mode]).apply()
    IoConfig(galois_merged_format=galois_merged).apply()

    os.makedirs(user_dir, exist_ok=True)
    rc = -1
    galois_files = 0
    galois_bytes = 0
    err = ""
    try:
        _C.set_key_path("")
        runner = fhe.FHERuntime(package)
        runner.run(*spec.example_inputs)
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
    parser = argparse.ArgumentParser(description="Multi-User: generate N independent FHE key bundles")
    parser.add_argument("--num-users", type=int, default=3, help="Number of independent key bundles (default: 3)")
    parser.add_argument("--key-root", default=None, help="Key-root output dir (default: ./multi_user_keys)")
    parser.add_argument("--spec", choices=list(SPECS.keys()), default="gemm_49x3", help="Sample model to compile")
    parser.add_argument(
        "--io", choices=["auto", "gds", "staged", "mapped"], default="staged", help="IO policy (default: staged)"
    )
    parser.add_argument(
        "--galois-format",
        choices=["merged", "per-key"],
        default="per-key",
        help="Galois key file format (default: per-key — avoids the merged-v3 bulk-read stall on non-NVMe filesystems)",
    )
    parser.add_argument(
        "--gpus",
        nargs="+",
        type=int,
        default=None,
        help="One free GPU id per user (e.g. --gpus 4 5 7) for parallel multi-process generation. "
        "Each user's keys are generated in its own fresh process (mandatory for correctness — "
        "see _gen_worker docstring) on the assigned GPU. If fewer GPUs than users are given, "
        "they are cycled; if omitted, all users run on the default GPU sequentially (still "
        "separate processes, just sharing the device).",
    )
    parser.add_argument(
        "--backend",
        choices=["phantom", "ace", "ant"],
        default="phantom",
        help="Compile/runtime backend (default: phantom). ace links libace_cuda and "
        "reuses the same generate/infer scripts. ace caveats: (1) key gen needs a card "
        "with >30G free (gemm_49x3 peaks ~26 GiB/proc via rmm); (2) ace Encode/Prepare_input/"
        "Bootstrap still route to the primary session, so Server-Serial/Parallel multi-session "
        "is not yet symmetrized — prefer 01_batch/05_on_demand_keys for ace; (3) ace has no "
        "ON_DEMAND support (Ensure_galois_key is a no-op stub) — only ONLINE/PRELOAD(_FULL).",
    )
    args = parser.parse_args()

    spec = SPECS[args.spec]
    root = os.path.abspath(args.key_root or f"./multi_user_keys_{args.spec}")

    if not gpu_available():
        print("ERROR: GPU not available. This example requires CUDA.")
        sys.exit(1)

    print("=" * 60)
    print(f"Multi-User: Generate {args.num_users} Independent Key Bundles")
    print("=" * 60)
    print(f"  Model spec: {spec.name}  ({spec.model_class.__name__})")
    print(f"  Input shape: {tuple(spec.example_inputs[0].shape)}")
    print(f"  Encrypt inputs: {spec.encrypt_inputs}")
    print(f"  Root key dir: {root}")

    # ── IO policy (must be set before any key save/load) ──
    _apply_io_config(args.io, galois_merged=(args.galois_format == "merged"))
    print()

    # ── Compile the model ONCE (all users share the same .so + .conf) ──
    print(f"[1/3] Compiling {spec.name} ...")
    t0 = time.time()
    prog = _compile_spec(spec, args.backend)
    package = prog.package
    t_compile = time.time() - t0
    print(f"      kernel: {package['kernel']}")
    print(f"      config: {package.get('config_path', '')}")
    print(f"      Done ({t_compile:.1f}s)")
    print()

    # ── Shared metadata ──
    os.makedirs(root, exist_ok=True)
    shared_meta = {
        "spec": args.spec,
        "backend": args.backend,
        "model_name": spec.name,
        "model_class": spec.model_class.__name__,
        "input_shape": list(spec.example_inputs[0].shape),
        "encrypt_inputs": spec.encrypt_inputs,
        "kernel": package["kernel"],
        "config_path": package.get("config_path", ""),
        "num_users": args.num_users,
        "user_key_dirs": [],
    }

    # ── Generate one independent key bundle per user (multiprocess) ──
    # Each user's keys MUST be generated in its own fresh process — a single
    # process that loops online→save→finalize produces contaminated 2nd+ key
    # bundles (phantom FheContext rebuild residue). See _gen_worker docstring.
    user_dirs = [os.path.join(root, f"user_{i}") for i in range(args.num_users)]
    gpus = args.gpus or []

    ctx = mp.get_context("spawn")  # fork is not safe with CUDA + pybind
    out_q = ctx.Queue()
    procs = []
    t_gen0 = time.time()
    print(f"[2/3] Spawning {args.num_users} key-generation processes ...")
    for i, user_dir in enumerate(user_dirs):
        gpu_id = gpus[i] if gpus else None
        p = ctx.Process(
            target=_gen_worker,
            args=(i, user_dir, args.spec, package, args.io, args.galois_format == "merged", gpu_id, out_q),
        )
        p.start()
        procs.append((i, p))

    # Collect results in completion order; map back to user index.
    gen_results = {}
    for _ in procs:
        r = out_q.get(timeout=600)
        gen_results[r["uid"]] = r
    for _uid, p in procs:
        p.join(timeout=30)
        if p.is_alive():
            p.terminate()

    for i, user_dir in enumerate(user_dirs):
        r = gen_results.get(i)
        if r is None or r["rc"] != 0:
            print(f"  ERROR generating user_{i}: {r['err'] if r else 'no result'}")
            sys.exit(1)
        shared_meta["user_key_dirs"].append(
            {
                "user": i,
                "key_dir": user_dir,
                "galois_key_files": r["galois_files"],
                "galois_key_bytes": r["galois_bytes"],
            }
        )
        print(f"  user_{i}: galois {r['galois_files']} files, {r['galois_bytes'] / 1024 / 1024:.1f} MB -> {user_dir}")
    print(f"      generation total: {time.time() - t_gen0:.1f}s")

    # ── Write shared metadata ──
    meta_path = os.path.join(root, "meta.json")
    with open(meta_path, "w") as f:
        json.dump(shared_meta, f, indent=2)

    print()
    print(f"[3/3] Wrote shared metadata: {meta_path}")
    total_galois = sum(u["galois_key_bytes"] for u in shared_meta["user_key_dirs"])
    print()
    print("=" * 60)
    print("All key bundles generated successfully!")
    print(f"  Users:              {args.num_users}")
    print(f"  Galois keys/user:   {shared_meta['user_key_dirs'][0]['galois_key_files']} files")
    print(f"  Galois keys/user:   {shared_meta['user_key_dirs'][0]['galois_key_bytes'] / 1024 / 1024:.1f} MB")
    print(f"  Total galois keys:  {total_galois / 1024 / 1024:.1f} MB")
    print()
    print("To run multi-user inference with these keys:")
    print(f"  python3 examples/10_multi_user/small/01_batch.py --key-root {root}")
    print("=" * 60)


if __name__ == "__main__":
    main()
