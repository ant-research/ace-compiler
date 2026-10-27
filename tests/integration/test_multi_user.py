#!/usr/bin/env python3
"""
Multi-user FHE inference — interface-level simulation test.

Verifies that N users can share one compiled model and each run inference under
their OWN independent key bundle, with all users' decrypted outputs agreeing on
the same plaintext input (the cross-user invariant: same circuit + same input,
different keys → same output within FHE noise).

Strategy tested here is Strategy C (one fresh process per user, for BOTH key
generation and inference). This is the verified-correct approach: phantom's
FheContext rebuild leaves flaky residual state that corrupts key generation in
the 2nd+ iteration of a single-process loop (contaminated keys → garbage
inference). Isolating each user to its own fresh process avoids the rebuild
entirely. ace backend is unaffected but we test on phantom (the default GPU path).

  pytest tests/integration/test_multi_user.py -v

Requires: GPU(s), phantom backend. Each user gets its own GPU (one FheContext
per process; sharing a card across processes risks OOM/context contention for
large models). The test discovers free GPUs automatically; skips if not enough.
"""

import multiprocessing as mp
import os
import sys

import pytest
import torch

from ace import fhe
from ace import runtime as _C
from ace.fhe.backend import get_library_impl
from ace.fhe.config import IoConfig, IoPolicy, IoPolicySettings
from ace.sample.ops.specs import GEMM_49X3_OP


# How many users to simulate. Small enough to fit on typical multi-GPU hosts.
NUM_USERS = 3
# Deterministic plaintext input shared across all users (so their outputs must agree).
_INPUT_SEED = 4242
# Cross-user agreement tolerance: CKKS noise on gemm_49x3 is ~1e-8 vs magnitudes ~1; 0.1 is safe.
_AGREE_ATOL = 0.1


def _phantom_available():
    try:
        return get_library_impl("phantom", device="cuda").check_available()
    except Exception:
        return False


def _free_gpus(n):
    """Return up to n GPU indices that currently show 0 MiB used, or None if < n free."""
    try:
        out = os.popen("nvidia-smi --query-gpu=index,memory.used --format=csv,noheader,nounits").read()
    except Exception:
        return None
    free = []
    for line in out.strip().splitlines():
        parts = [p.strip() for p in line.split(",")]
        if len(parts) >= 2 and parts[1].isdigit() and int(parts[1]) < 500:
            free.append(int(parts[0]))
            if len(free) >= n:
                return free
    return free if len(free) >= n else None


# ───────────────────────── worker functions (module-level for spawn) ─────────────────────────


def _gen_worker(uid, key_dir, package, gpu_id, out_q):
    """Spawned: generate ONE user's keys in a fresh process (clean FheContext)."""
    os.environ["CUDA_VISIBLE_DEVICES"] = str(gpu_id)
    os.environ.setdefault("ACE_GDS_ENABLED", "0")
    os.environ.setdefault("FHERT_GALOIS_LOAD_THREADS", "1")
    import importlib

    importlib.invalidate_caches()
    from ace import fhe
    from ace.fhe.config import IoConfig, IoPolicy, IoPolicySettings
    from ace.sample.ops.specs import GEMM_49X3_OP as spec

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


def _infer_worker(uid, key_dir, package, gpu_id, input_arrays, out_q):
    """Spawned: load ONE user's key and run inference on the shared inputs."""
    os.environ["CUDA_VISIBLE_DEVICES"] = str(gpu_id)
    os.environ.setdefault("ACE_GDS_ENABLED", "0")
    os.environ.setdefault("FHERT_GALOIS_LOAD_THREADS", "1")
    import importlib

    importlib.invalidate_caches()
    from ace import fhe

    IoPolicySettings(policy=IoPolicy.STAGED).apply()
    IoConfig(galois_merged_format=False).apply()
    try:
        pkg = {**package, "key_path": key_dir, "key_mode": "preload_full"}
        runner = fhe.FHERuntime(pkg)
        runner.init()
        outs = []
        for inp in input_arrays:
            x = torch.tensor(inp[0], dtype=torch.float32)
            out = runner.run(x)
            outs.append(out.float().reshape(-1).tolist())
        runner.finalize()
        out_q.put({"uid": uid, "ok": True, "outs": outs, "err": ""})
    except Exception as e:
        import traceback

        out_q.put({"uid": uid, "ok": False, "outs": [], "err": f"{type(e).__name__}: {e}\n{traceback.format_exc()}"})


# ────────────────────────────────── test ──────────────────────────────────


@pytest.mark.skipif(not _phantom_available(), reason="phantom-cuda not available")
def test_multi_user_independent_processes_agree(tmp_path):
    """N users, each in its own fresh process for gen AND inference, must agree.

    This is the regression anchor for the multi-user correctness fix: generating
    AND inferring each user in a separate fresh process (so no FheContext rebuild
    within a process) yields correct, agreeing outputs. It will fail if either
    generation or inference regresses to a shared-process/rebuild model that
    triggers phantom's rebuild-residue contamination.
    """
    gpus = _free_gpus(NUM_USERS)
    if gpus is None:
        pytest.skip(f"need {NUM_USERS} free GPUs; not enough free cards right now")

    spec = GEMM_49X3_OP
    # Compile once in the parent (shares the .so across all user processes).
    model = spec.create_model()
    compiled = fhe.compile(
        frontend="torch", library="phantom", device="cuda", encrypt_inputs=spec.encrypt_inputs, profile_relu=True
    )(model)
    prog = compiled.fhe_compile(spec.example_inputs)
    package = {"model": spec.name, "kernel": prog.package["kernel"], "config_path": prog.package.get("config_path", "")}

    # Shared plaintext inputs (plain lists, picklable for spawn).
    torch.manual_seed(_INPUT_SEED)
    n_inputs = 3
    shared_inputs = [[spec.example_inputs[0].cpu().tolist()] for _ in range(n_inputs)]

    # ── Phase 1: generate keys, one fresh process per user ──
    ctx = mp.get_context("spawn")
    q = ctx.Queue()
    key_dirs = [str(tmp_path / f"user_{i}") for i in range(NUM_USERS)]
    procs = []
    for i, kd in enumerate(key_dirs):
        p = ctx.Process(target=_gen_worker, args=(i, kd, package, gpus[i], q))
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

    # ── Phase 2: infer, one fresh process per user, on shared inputs ──
    q3 = ctx.Queue()
    procs = []
    for i, kd in enumerate(key_dirs):
        p = ctx.Process(target=_infer_worker, args=(i, kd, package, gpus[i], shared_inputs, q3))
        p.start()
        procs.append(p)
    inf = {}
    for _ in procs:
        r = q3.get(timeout=600)
        inf[r["uid"]] = r
    for p in procs:
        p.join(timeout=60)
        if p.is_alive():
            p.terminate()

    # ── Assertions ──
    for i in range(NUM_USERS):
        r = inf.get(i)
        assert r is not None and r["ok"], f"user_{i} inference failed: {r['err'] if r else 'no result'}"
        assert len(r["outs"]) == n_inputs, f"user_{i} got {len(r['outs'])} outputs, expected {n_inputs}"

    # Cross-user agreement: every input, every pair of users must agree.
    ref = inf[0]["outs"]
    for i in range(1, NUM_USERS):
        for k in range(n_inputs):
            a = torch.tensor(ref[k])
            b = torch.tensor(inf[i]["outs"][k])
            d = (a - b).abs().max().item()
            assert d < _AGREE_ATOL, (
                f"cross-user disagreement: input {k} user_{i} vs user_0 max_diff={d} (atol={_AGREE_ATOL})"
            )
