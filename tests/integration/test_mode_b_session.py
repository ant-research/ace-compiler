#!/usr/bin/env python3
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""Mode B session API — serial correctness regression test.

Anchors the per-session evaluator + thread-local routing + ``run_session``
serial path (ace-compiler ``feat/mode-b-parallel``). This is the path that
the cross-DSO TLS refactor (commit 78a6109 — confining FHE thread-local state
to the backend .so) must NOT break: two resident sessions, serialized
``run_session`` calls on the main thread, each bound to its own keys.

Strategy tested here:
  - Two INDEPENDENT key bundles are generated, each in its own fresh process
    (spawn) — this avoids phantom's FheContext rebuild-residue contaminating
    the 2nd+ key bundle (the same discipline as test_multi_user.py). The keys
    are DIFFERENT across users, so a wrong-session routing bug (e.g. an
    evaluator key rebind that silently keeps the primary) would surface as a
    cross-user mismatch rather than being masked by identical keys.
  - In the main process: build one FHERuntime on user_0's keys (primary,
    idx=0), ``create_session`` user_1's keys (idx=1), then run the SAME
    plaintext input through ``run_session(0, x)`` and ``run_session(1, x)``
    SERIALLY.

Invariant: same circuit + same input, different keys → same decrypted output
within CKKS noise (cross-user agreement). A regression in the session routing
or the thread-local current-context binding breaks this.

  pytest tests/integration/test_mode_b_session.py -v

Requires: a GPU and the phantom backend. Uses a single free GPU (both sessions
share one FheContext — that is the whole point of Mode B).
"""

import multiprocessing as mp
import os

import pytest
import torch

from ace import fhe
from ace.fhe.backend import get_library_impl
from ace.fhe.config import IoConfig, IoPolicy, IoPolicySettings
from ace.sample.ops.specs import ADD_OP


NUM_USERS = 2
# Cross-user agreement tolerance: ADD_OP output ~O(1), CKKS noise well below 0.1.
_AGREE_ATOL = 0.1


def _phantom_available():
    try:
        return get_library_impl("phantom", device="cuda").check_available()
    except Exception:
        return False


def _free_gpu():
    """Return one GPU index showing ~0 MiB used, or None."""
    try:
        out = os.popen("nvidia-smi --query-gpu=index,memory.used --format=csv,noheader,nounits").read()
    except Exception:
        return None
    for line in out.strip().splitlines():
        parts = [p.strip() for p in line.split(",")]
        if len(parts) >= 2 and parts[1].isdigit() and int(parts[1]) < 500:
            return int(parts[0])
    return None


# ─────────────────────────── worker (module-level for spawn) ───────────────────────────


def _gen_worker(uid, key_dir, package, gpu_id, out_q):
    """Spawned: generate ONE user's keys in a fresh process (clean FheContext)."""
    os.environ["CUDA_VISIBLE_DEVICES"] = str(gpu_id)
    os.environ.setdefault("ACE_GDS_ENABLED", "0")
    os.environ.setdefault("FHERT_GALOIS_LOAD_THREADS", "1")
    import importlib

    importlib.invalidate_caches()
    from ace import fhe
    from ace import runtime as _C
    from ace.fhe.config import IoConfig, IoPolicy, IoPolicySettings
    from ace.sample.ops.specs import ADD_OP as spec

    IoPolicySettings(policy=IoPolicy.STAGED).apply()
    IoConfig(galois_merged_format=False).apply()
    os.makedirs(key_dir, exist_ok=True)
    try:
        _C.set_key_path("")
        runner = fhe.FHERuntime(package)
        runner.run(*spec.example_inputs)  # online: generate keys
        rc = runner.save_keys(key_dir)
        runner.finalize()
        out_q.put({"uid": uid, "rc": rc, "err": ""})
    except Exception as e:
        import traceback

        out_q.put({"uid": uid, "rc": -1, "err": f"{type(e).__name__}: {e}\n{traceback.format_exc()}"})


# ─────────────────────────────────── test ───────────────────────────────────


@pytest.mark.skipif(not _phantom_available(), reason="phantom-cuda not available")
def test_mode_b_serial_run_session_agrees(tmp_path):
    """Two resident sessions, serial ``run_session`` on the main thread, must agree.

    Regression anchor for the Mode B-P serial path (per-session evaluator +
    cross-DSO TLS routing + ``run_session``). Fails if the thread-local
    current-context binding (confined to the backend .so in 78a6109) regresses,
    or if ``run_session`` routes a session to the wrong evaluator/keys.
    """
    gpu = _free_gpu()
    if gpu is None:
        pytest.skip("no free GPU right now (need one card <500 MiB used)")

    spec = ADD_OP
    # Compile once in the parent; the .so is shared by the gen workers.
    model = spec.create_model()
    compiled = fhe.compile(
        frontend="torch", library="phantom", device="cuda", encrypt_inputs=spec.encrypt_inputs, profile_relu=True
    )(model)
    prog = compiled.fhe_compile(spec.example_inputs)
    package = {"model": spec.name, "kernel": prog.package["kernel"], "config_path": prog.package.get("config_path", "")}

    # ── Phase 1: generate 2 independent key bundles, one fresh process each ──
    ctx = mp.get_context("spawn")
    q = ctx.Queue()
    key_dirs = [str(tmp_path / f"user_{i}") for i in range(NUM_USERS)]
    procs = []
    for i, kd in enumerate(key_dirs):
        p = ctx.Process(target=_gen_worker, args=(i, kd, package, gpu, q))
        p.start()
        procs.append(p)
    gen = {}
    for _ in procs:
        r = q.get(timeout=600)
        gen[r["uid"]] = r
    for p in procs:
        p.join(timeout=60)
        if p.is_alive():
            p.terminate()
    for i in range(NUM_USERS):
        r = gen.get(i)
        assert r is not None and r["rc"] == 0, f"user_{i} key gen failed: {r['err'] if r else 'no result'}"

    # ── Phase 2: main process — shared context, 2 resident sessions, serial run ──
    os.environ["CUDA_VISIBLE_DEVICES"] = str(gpu)
    os.environ.setdefault("ACE_GDS_ENABLED", "0")
    os.environ.setdefault("FHERT_GALOIS_LOAD_THREADS", "1")
    IoPolicySettings(policy=IoPolicy.STAGED).apply()
    IoConfig(galois_merged_format=False).apply()

    pkg0 = {**package, "key_path": key_dirs[0], "key_mode": "preload_full"}
    runner = fhe.FHERuntime(pkg0)
    try:
        runner.init()
        idx1 = runner.create_session(key_dirs[1])
        assert idx1 >= 1, f"create_session(user_1) returned {idx1}"

        # Same plaintext input through both sessions, SERIALLY on the main thread.
        x = tuple(t.clone() for t in spec.example_inputs)
        out0 = runner.run_session(0, *x)
        out1 = runner.run_session(1, *x)

        a = out0.float().reshape(-1)
        b = out1.float().reshape(-1)
        diff = (a - b).abs().max().item()
        assert diff < _AGREE_ATOL, (
            f"run_session serial disagreement: user_1 vs user_0 max_diff={diff} (atol={_AGREE_ATOL})"
        )
    finally:
        runner.finalize()
