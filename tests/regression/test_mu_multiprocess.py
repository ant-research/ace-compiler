#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""Multi-user smoke — multi-process (mode C, one OS process per user).

The process-isolation baseline. Each user gets its OWN FheContext in its own
fresh OS process — no rebuild residue, no shared context. This is the
verified-correct reference path; the serial/parallel cells must agree with it.

Key bundles are generated one-per-fresh-process (mu_gen_keys), then each user
loads + infers in its own process. Needs MU_NUM_USERS free GPUs (one FheContext
per process). Falls back to skip if not enough free cards.

  pytest tests/regression/test_mu_multiprocess.py -v
"""

import multiprocessing as mp
import os
import shutil

import pytest
import torch
from conftest import (
    MU_AGREE_ATOL,
    MU_INPUT_SEED,
    MU_NUM_USERS,
    mu_agree,
    mu_compile_once,
    mu_free_gpus,
    mu_gen_keys,
    mu_package,
    mu_requires_phantom,
)


pytestmark = [
    pytest.mark.usefixtures("staged_io_workaround"),
    mu_requires_phantom(),
]


def _mp_infer_worker(uid, key_dir, package, gpu_id, input_arrays, out_q):
    """Spawned: load one user's key in a fresh process and infer on shared inputs."""
    os.environ["CUDA_VISIBLE_DEVICES"] = str(gpu_id)
    os.environ.setdefault("ACE_GDS_ENABLED", "0")
    os.environ.setdefault("FHERT_GALOIS_LOAD_THREADS", "1")
    import importlib

    importlib.invalidate_caches()
    from ace import fhe
    from ace.fhe.config import IoConfig, IoPolicy, IoPolicySettings

    IoPolicySettings(policy=IoPolicy.STAGED).apply()
    IoConfig(galois_merged_format=False).apply()
    try:
        pkg = {**package, "key_path": key_dir, "key_mode": "preload_full"}
        runner = fhe.FHERuntime(pkg)
        runner.init()
        tensors = [torch.tensor(a, dtype=torch.float32) for a in input_arrays]
        out = runner.run(*tensors).float().reshape(-1).tolist()
        runner.finalize()
        out_q.put({"uid": uid, "ok": True, "out": out, "err": ""})
    except Exception as e:
        import traceback

        out_q.put({"uid": uid, "ok": False, "out": [], "err": f"{type(e).__name__}: {e}\n{traceback.format_exc()}"})


def test_multi_user_multiprocess_agree(tmp_path):
    """K users, each its own process: same input -> outputs agree across processes."""
    gpus = mu_free_gpus(MU_NUM_USERS)
    if gpus is None:
        pytest.skip(f"need {MU_NUM_USERS} free GPUs for multi-proc cell; not enough now")

    prog, inputs = mu_compile_once()
    key_dirs = [str(tmp_path / f"keys_{i}") for i in range(MU_NUM_USERS)]
    for kd in key_dirs:
        mu_gen_keys(prog, inputs, kd)  # multi-process gen (avoids rebuild residue)
    package = mu_package(prog)

    torch.manual_seed(MU_INPUT_SEED)
    shared_inputs = [t.cpu().tolist() for t in inputs]

    try:
        ctx = mp.get_context("spawn")
        q = ctx.Queue()
        procs = []
        for i, kd in enumerate(key_dirs):
            p = ctx.Process(target=_mp_infer_worker, args=(i, kd, package, gpus[i], shared_inputs, q))
            p.start()
            procs.append(p)
        inf = {}
        for _ in procs:
            r = q.get(timeout=600)
            inf[r["uid"]] = r
        for p in procs:
            p.join(timeout=60)
            if p.is_alive():
                p.terminate()

        for i in range(MU_NUM_USERS):
            r = inf.get(i)
            assert r is not None and r["ok"], f"user_{i} multi-proc failed: {r['err'] if r else 'no result'}"

        ref = torch.tensor(inf[0]["out"])
        for i in range(1, MU_NUM_USERS):
            mu_agree(ref, torch.tensor(inf[i]["out"]), f"multiprocess: user_{i} vs user_0")
    finally:
        for kd in key_dirs:
            shutil.rmtree(kd, ignore_errors=True)
