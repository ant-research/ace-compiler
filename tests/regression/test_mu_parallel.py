#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""Multi-user smoke — single-process, PARALLEL (mode B-P).

One FheContext holds K sessions. run_session(i, x) binds the CALLING THREAD to
session i (per-thread TLS active idx) with the GIL released, so K worker threads
truly run concurrently. This is the target serving path. A refactor that breaks
per-session isolation (shared evaluator mutation, Io store regression, K>=2
race) makes outputs diverge or crashes.

Parametrized over key loading mode (preload_full / on_demand_full). GEMM_49X3_OP
rotates so ON_DEMAND's lazy path is exercised.

  pytest tests/regression/test_mu_parallel.py -v
"""

import shutil
from concurrent.futures import ThreadPoolExecutor

import pytest
import torch
from conftest import (
    MU_INPUT_SEED,
    MU_NUM_USERS,
    mu_agree,
    mu_compile_once,
    mu_gen_keys,
    mu_package,
    mu_requires_phantom,
)


pytestmark = [
    pytest.mark.usefixtures("staged_io_workaround"),
    mu_requires_phantom(),
]


@pytest.mark.parametrize("key_mode", ["preload_full", "on_demand_full"])
def test_multi_user_parallel_run_session(tmp_path, key_mode):
    """K users, parallel run_session on K threads: same input -> outputs agree."""
    prog, inputs = mu_compile_once()
    key_dirs = [str(tmp_path / f"keys_{i}") for i in range(MU_NUM_USERS)]
    for kd in key_dirs:
        mu_gen_keys(prog, inputs, kd)
    try:
        from ace import runtime as _C

        _C.set_key_path(key_dirs[0])
        from ace import fhe

        runner = fhe.FHERuntime(mu_package(prog, key_dir=key_dirs[0], key_mode=key_mode))
        runner.init()
        for i in range(1, MU_NUM_USERS):
            idx = runner.create_session(key_dirs[i])
            assert idx > 0, f"create_session({key_dirs[i]}) failed under {key_mode}"

        # Shared plaintext inputs -> parallel users must agree.
        torch.manual_seed(MU_INPUT_SEED)
        shared = [tuple(t.clone() for t in inputs) for _ in range(MU_NUM_USERS)]
        outs = [None] * MU_NUM_USERS

        def _work(i):
            outs[i] = runner.run_session(i, *shared[i]).detach().cpu().flatten().float()

        with ThreadPoolExecutor(max_workers=MU_NUM_USERS) as pool:
            list(pool.map(_work, range(MU_NUM_USERS)))
        runner.finalize()
        _C.clear_key_path()

        for i in range(1, MU_NUM_USERS):
            mu_agree(outs[0], outs[i], f"parallel {key_mode}: user_{i} vs user_0")
    finally:
        for kd in key_dirs:
            shutil.rmtree(kd, ignore_errors=True)
