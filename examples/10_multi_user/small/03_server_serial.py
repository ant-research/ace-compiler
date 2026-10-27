#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Multi-User Inference — Server-Serial mode (formerly Mode B; single-process native session hot-switch), small case.

One process, one GPU, one shared FheContext (built once, never rebuilt). N key
bundles are each loaded into their own resident FheSession in HBM via
``FHERuntime.create_session(key_dir)``; switching users is a microsecond-scale
evaluator key rebind via ``FHERuntime.switch_session(idx)`` — no context teardown,
no galois-key re-read. This is the native C++ session path that the C++ commit
exposed to Python.

Contrast with Batch (small/01_batch.py): there every switch finalizes the
FheContext and rebuilds it on the new key_dir, so each switch pays
context-build + key-load again. Server-Serial pays key-load once per user (at
create_session) and context-build only once (primary init); switches are ~free.

Flow
----
1. Build one FHERuntime on user_0's key_dir (primary session, idx 0).
2. init() — PhantomContext build + primary key load (timed).
3. For each extra user: create_session(key_dir) — full key load into HBM
   (timed). Resident thereafter.
4. Cycle users: switch_session(idx) (timed, µs–ms) → run() (timed).
5. Cross-user output agreement (same input, different keys → same output).

NOTE: extra sessions must use the same galois elements as the primary (true for
same-model keys generated with the same spec). The shared GaloisTool is not
rebuilt.

Usage:
    python3 examples/10_multi_user/small/03_server_serial.py --key-root /mnt/nvme0/multi_user_gemm --rounds 3
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
from ace.sample.ops.specs import ADD_OP, GEMM_49X3_OP, LINEAR_BIAS_OP


# multi_user_runtime lives in the parent dir (10_multi_user/), shared with resnet20 group
sys.path.insert(0, os.path.dirname(os.path.dirname(os.path.abspath(__file__))))

SPECS = {"gemm_49x3": GEMM_49X3_OP, "linear_bias": LINEAR_BIAS_OP, "add_op": ADD_OP}


def main():
    parser = argparse.ArgumentParser(description="Multi-User FHE inference — Server-Serial (native session hot-switch)")
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
    user_dirs = [u["key_dir"] for u in meta["user_key_dirs"]]
    user_ids = [f"user_{u['user']}" for u in meta["user_key_dirs"]]

    if not gpu_available():
        print("ERROR: GPU not available. This example requires CUDA.")
        sys.exit(1)

    print("=" * 70)
    print(f"Multi-User (Server-Serial) — {spec.name}, {num_users} users, {args.rounds} rounds")
    print("=" * 70)

    # --- 1. Build one long-lived FHERuntime on the primary (user_0) key dir ---
    # key_mode=preload_full so init() loads SK/PK/RLK/GK for the primary session.
    package = {
        "model": spec.name,
        "kernel": meta["kernel"],
        "config_path": meta.get("config_path", ""),
        "key_path": user_dirs[0],
        "key_mode": "preload_full",
    }

    t0 = time.perf_counter()
    runner = fhe.FHERuntime(package)
    t0i = time.perf_counter()
    runner.init()
    primary_init_total = time.perf_counter() - t0i
    primary_build = time.perf_counter() - t0
    primary_load = runner.get_galois_load_time_seconds()
    primary_init = primary_init_total - primary_load
    print(
        f"  primary {user_ids[0]:8s} build(ctx+load)={primary_build * 1e3:6.0f}ms "
        f"load={primary_load * 1e3:6.0f}ms init={primary_init * 1e3:6.0f}ms  (idx=0)"
    )

    # session index 0 = primary
    session_idx = {user_ids[0]: 0}

    # --- 2. Create resident extra sessions (one per remaining user) ---
    # create_session loads a FULL key bundle from key_dir into HBM; this is the
    # one-time per-user cost that Batch would pay on EVERY switch.
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
        print(f"  create  {uid:8s} idx={idx} total={dt * 1e3:6.0f}ms galois_load={gk * 1e3:6.0f}ms")

    # --- 3. Warm-up run under primary (first run often pays lazy setup) ---
    x = tuple(torch.randn(t.shape, dtype=t.dtype) for t in spec.example_inputs)
    runner.switch_session(0)
    _ = runner.run(*x)

    # --- 4. Cycle users: switch (timed) → run (timed) ---
    switch_times: dict[str, list[float]] = {uid: [] for uid in user_ids}
    infer_times: dict[str, list[float]] = {uid: [] for uid in user_ids}
    all_ok = True
    t0_total = time.perf_counter()

    for r in range(args.rounds):
        x = tuple(torch.randn(t.shape, dtype=t.dtype) for t in spec.example_inputs)
        round_outputs: dict[str, torch.Tensor] = {}

        for uid in user_ids:
            idx = session_idx[uid]
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
            switch_times[uid].append(sw)
            infer_times[uid].append(inf)
            round_outputs[uid] = out.float().reshape(out.shape)
            print(f"  round {r} {uid:8s} switch={sw * 1e6:7.1f}µs infer={inf * 1e3:6.0f}ms")

        # Cross-user agreement (same input, different keys → same output).
        atol, rtol = 0.1, 0.05
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

    # --- 5. Summary ---
    print()
    print("=" * 70)
    print("Summary")
    print("=" * 70)
    print(f"  users / rounds:  {num_users} / {args.rounds}")
    print(f"  serve wall:      {total_s:.3f}s  (switches + infers only; setup excluded)")
    print(f"  all correct:     {all_ok}")
    print()
    print(f"  {'user':8s} {'create_ms':>10s} {'switch_us':>10s} {'infer_ms':>10s}")
    for uid in user_ids:
        c = create_times.get(uid, 0.0)
        sw = statistics.mean(switch_times[uid]) * 1e6 if switch_times[uid] else 0.0
        inf = statistics.mean(infer_times[uid]) * 1e3 if infer_times[uid] else 0.0
        print(f"  {uid:8s} {c * 1e3:9.1f}ms {sw:9.1f}µs {inf:9.1f}ms")
    all_sw = [v for vs in switch_times.values() for v in vs]
    if all_sw:
        print(
            f"  {'mean':8s} {'':>10s} {statistics.mean(all_sw) * 1e6:9.1f}µs "
            f"(min={min(all_sw) * 1e6:.1f}µs max={max(all_sw) * 1e6:.1f}µs)"
        )
    print()
    print(f"  primary build (one-time):  {primary_build * 1e3:.0f}ms  (context + key load)")
    print(
        f"  per-user create (one-time): {statistics.mean(list(create_times.values())) * 1e3:.0f}ms avg "
        f"(galois load; resident thereafter)"
    )
    print(f"  switch_session latency:     ~{statistics.mean(all_sw) * 1e6:.1f}µs  (key rebind, no IO)")
    print("=" * 70)
    if not all_ok:
        sys.exit(1)


if __name__ == "__main__":
    main()
