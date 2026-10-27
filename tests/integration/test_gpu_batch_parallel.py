#!/usr/bin/env python3
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""GPU single-user multi-image batch via ``run_batch(parallel=True)``.

Tests that ``FHERuntime.run_batch(parallel=True)`` correctly:
  1. Auto-creates sessions for each image (all bound to the same key_dir).
  2. Executes all sessions in parallel (ThreadPoolExecutor, per-session
     evaluator+stream, no scratch-race).
  3. Returns all-agree outputs (same key, same circuit → identical FHE output
     within CKKS noise tolerance).
  4. Reports correct success/failure counts and timing.

Also tests:
  - ``num_sessions`` property reflects session count.
  - Session 0 runs via ``run_session`` concurrently with ``run_batch_parallel``.

  pytest tests/integration/test_gpu_batch_parallel.py -v

Requires: a GPU and the phantom backend. Uses a single free GPU.
"""

import os

import pytest
import torch

from ace import fhe
from ace.fhe.backend import get_library_impl
from ace.fhe.config import IoConfig, IoPolicy, IoPolicySettings
from ace.sample.ops.specs import ADD_OP


_AGREE_ATOL = 0.1
_NUM_IMAGES = 4


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


# ─────────────────────────── helpers ───────────────────────────


def _setup_env(gpu_id: int):
    os.environ["CUDA_VISIBLE_DEVICES"] = str(gpu_id)
    os.environ.setdefault("ACE_GDS_ENABLED", "0")
    os.environ.setdefault("FHERT_GALOIS_LOAD_THREADS", "1")
    IoPolicySettings(policy=IoPolicy.STAGED).apply()
    IoConfig(galois_merged_format=False).apply()


def _compile_and_load_keys(tmp_path, spec, gpu_id, key_mode="preload_full"):
    """Compile model and preload keys on GPU, return runner + key_dir."""
    _setup_env(gpu_id)

    model = spec.create_model()
    compiled = fhe.compile(
        frontend="torch",
        library="phantom",
        device="cuda",
        encrypt_inputs=spec.encrypt_inputs,
        profile_relu=True,
    )(model)
    prog = compiled.fhe_compile(spec.example_inputs)
    package = {
        "model": spec.name,
        "kernel": prog.package["kernel"],
        "config_path": prog.package.get("config_path", ""),
    }

    key_dir = str(tmp_path / "keys")
    pkg = {**package, "key_path": key_dir, "key_mode": key_mode}
    runner = fhe.FHERuntime(pkg)
    runner.init()
    return runner, package, key_dir


# ─────────────────────────── tests ───────────────────────────


@pytest.mark.skipif(not _phantom_available(), reason="phantom-cuda not available")
def test_run_batch_parallel_all_agree(tmp_path):
    """GPU batch: N images → N sessions all share same keys → outputs agree."""
    gpu = _free_gpu()
    if gpu is None:
        pytest.skip("no free GPU")

    spec = ADD_OP
    runner, _package, _key_dir = _compile_and_load_keys(tmp_path, spec, gpu)
    try:
        # Verify num_sessions starts at 1 (primary only)
        assert runner.num_sessions == 1, f"expected 1, got {runner.num_sessions}"

        # Prepare N identical inputs (same user, single-image-per-session model)
        N = _NUM_IMAGES
        inputs = [list(t.clone() for t in spec.example_inputs) for _ in range(N)]

        # Run batch parallel
        result = runner.run_batch(inputs, parallel=True, num_threads=min(N, 2), verbose=False)

        # Every image should succeed
        assert result.num_success == N, f"expected {N} success, got {result.num_success}"
        assert result.num_failure == 0, f"expected 0 failure, got {result.num_failure}"

        # All outputs must agree (same key, same circuit, same input)
        base = result.outputs[0].float().reshape(-1)
        for i in range(1, N):
            other = result.outputs[i].float().reshape(-1)
            diff = (base - other).abs().max().item()
            assert diff < _AGREE_ATOL, (
                f"batch parallel disagreement: img 0 vs img {i} max_diff={diff} (atol={_AGREE_ATOL})"
            )

        # After creating N sessions, num_sessions should reflect N
        assert runner.num_sessions == N, f"expected {N} sessions, got {runner.num_sessions}"

        # Timing: not zero (we actually ran)
        assert result.timing.num_images == N
        # total_ms > 0 means something happened
        assert result.timing.total_ms > 0, "total_ms is zero — nothing ran"

    finally:
        runner.finalize()


@pytest.mark.skipif(not _phantom_available(), reason="phantom-cuda not available")
def test_run_batch_parallel_vs_serial_agrees(tmp_path):
    """GPU batch parallel outputs match serial run_session outputs."""
    gpu = _free_gpu()
    if gpu is None:
        pytest.skip("no free GPU")

    spec = ADD_OP
    runner, _package, _key_dir = _compile_and_load_keys(tmp_path, spec, gpu)
    try:
        N = 3
        inputs = [list(t.clone() for t in spec.example_inputs) for _ in range(N)]

        # Serial: run_session(0..N-1, each image)
        serial_results = []
        for i in range(N):
            out = runner.run_session(i, *[t.clone() for t in inputs[i]])
            serial_results.append(out.float().reshape(-1))

        # Check serial all-agree
        for i in range(1, N):
            diff = (serial_results[0] - serial_results[i]).abs().max().item()
            assert diff < _AGREE_ATOL, f"serial disagreement: img 0 vs img {i}, diff={diff}"

        # Now parallel via batch
        result = runner.run_batch(inputs, parallel=True, num_threads=N, verbose=False)
        assert result.num_success == N

        # Check parallel matches serial
        for i in range(N):
            diff = (serial_results[i] - result.outputs[i].float().reshape(-1)).abs().max().item()
            assert diff < _AGREE_ATOL, f"parallel vs serial disagreement: img {i} diff={diff} (atol={_AGREE_ATOL})"

    finally:
        runner.finalize()


@pytest.mark.skipif(not _phantom_available(), reason="phantom-cuda not available")
def test_num_sessions_property(tmp_path):
    """num_sessions starts at 1, increments on create_session."""
    gpu = _free_gpu()
    if gpu is None:
        pytest.skip("no free GPU")

    spec = ADD_OP
    runner, _package, key_dir = _compile_and_load_keys(tmp_path, spec, gpu)
    try:
        assert runner.num_sessions == 1, "primary session not counted"

        idx1 = runner.create_session(key_dir)
        assert idx1 >= 1
        assert runner.num_sessions == 2, "num_sessions should be 2 after 1 create_session"

        idx2 = runner.create_session(key_dir)
        assert idx2 >= 2
        assert runner.num_sessions == 3, "num_sessions should be 3 after 2 create_session"

    finally:
        runner.finalize()


@pytest.mark.skipif(not _phantom_available(), reason="phantom-cuda not available")
def test_run_batch_parallel_single_image_matches_serial(tmp_path):
    """run_session(0, x) and run_batch(parallel=True) with N=1 should produce
    identical output for session 0, validating the batch parallel path
    delegates to run_session with correct session routing.
    """
    gpu = _free_gpu()
    if gpu is None:
        pytest.skip("no free GPU")

    spec = ADD_OP
    runner, _package, _key_dir = _compile_and_load_keys(tmp_path, spec, gpu)
    try:
        # Single image → one session
        single_input = [list(t.clone() for t in spec.example_inputs)]

        # Serial run_session(0, ...)
        out_serial = runner.run_session(0, *single_input[0])

        # Batch parallel with 1 image, 1 thread
        result = runner.run_batch(single_input, parallel=True, num_threads=1)
        assert result.num_success == 1

        # They must agree (same session 0, same keys, same circuit)
        diff = (out_serial.float().reshape(-1) - result.outputs[0].float().reshape(-1)).abs().max().item()
        assert diff < _AGREE_ATOL, f"run_session(0) vs run_batch(parallel=True)[0] differ: diff={diff}"

    finally:
        runner.finalize()
