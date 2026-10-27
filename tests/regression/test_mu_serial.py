#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""Multi-user smoke — single-process, SERIAL (mode B hot-switch).

One FheContext holds K sessions (each its own key bundle). switch_session(idx)
rebinds the active session, then run() executes against that session. Users are
served one at a time on the calling thread. This is the correctness
prerequisite for the parallel (B-P) path: if per-session key isolation is
broken here, parallel will be too.

Parametrized over key loading mode:
  - preload_full:   all keys (incl. galois) resident at init
  - on_demand_full: galois keys lazy via Ensure_galois_key on each rotate

GEMM_49X3_OP rotates, so ON_DEMAND actually pulls galois keys on cache miss.

  pytest tests/regression/test_mu_serial.py -v
"""

import shutil

import pytest
import torch
from conftest import (
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
def test_multi_user_serial_switch_session(tmp_path, key_mode):
    """K users, serial switch_session: same input -> outputs agree across users."""
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

        outs = []
        for i in range(MU_NUM_USERS):
            assert runner.switch_session(i) == 0, f"switch_session({i}) failed"
            outs.append(runner.run(*[t.clone() for t in inputs]).detach().cpu().flatten().float())
        runner.finalize()
        _C.clear_key_path()

        for i in range(1, MU_NUM_USERS):
            mu_agree(outs[0], outs[i], f"serial {key_mode}: user_{i} vs user_0")
    finally:
        for kd in key_dirs:
            shutil.rmtree(kd, ignore_errors=True)
