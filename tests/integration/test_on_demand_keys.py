# tests/integration/test_on_demand_keys.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# Verify KeyMode::ON_DEMAND_FULL produces output matching PRELOAD_FULL
# (key_offline), exercising the lazy galois-key loading path (Ensure_galois_key
# on every rotate). ON_DEMAND_FULL is the full-pipeline variant (SK/PK preloaded
# for encrypt/decrypt) of the otherwise eval-only ON_DEMAND, used here so the
# comparison can run end-to-end through FHERuntime.run().
#
# Uses an op with rotations (GEMM_49X3_OP) so the on-demand path actually pulls
# galois keys on cache miss; an op with no rotate would only exercise the
# empty-shell init and trivially match.
#
# Mirrors the autouse staged-IO workaround in test_kms_key_save_load (this host
# has false-available cuFile -> galois load dead-waits without staged IO).

import os

import pytest

from ace import fhe
from ace import runtime as _C
from ace.fhe.backend import get_library_impl
from ace.fhe.config import IoConfig, IoPolicy, IoPolicySettings
from ace.sample.ops.specs import GEMM_49X3_OP


def _provider_available(name, device):
    try:
        return get_library_impl(name, device=device).check_available()
    except Exception:
        return False


_requires_phantom = pytest.mark.skipif(not _provider_available("phantom", "cuda"), reason="phantom-cuda not available")


@pytest.fixture(autouse=True)
def _staged_io_workaround():
    """Same cuFile-false-host workaround as test_kms_key_save_load."""
    old = {k: os.environ.get(k) for k in ("ACE_GDS_ENABLED", "FHERT_GALOIS_LOAD_THREADS")}
    os.environ["ACE_GDS_ENABLED"] = "0"
    os.environ["FHERT_GALOIS_LOAD_THREADS"] = "1"
    IoPolicySettings(policy=IoPolicy.STAGED).apply()
    IoConfig(galois_merged_format=False).apply()
    try:
        yield
    finally:
        for k, v in old.items():
            if v is None:
                os.environ.pop(k, None)
            else:
                os.environ[k] = v


@_requires_phantom
def test_on_demand_full_matches_preload_full(tmp_path):
    """ON_DEMAND_FULL inference must match PRELOAD_FULL (key_offline).

    GEMM_49X3_OP uses rotations, so ON_DEMAND_FULL's FheContext::Rotate ->
    Ensure_galois_key lazily pulls each galois key on cache miss. The output
    must match the fully-preloaded baseline within 1e-3.
    """
    model = GEMM_49X3_OP.create_model()
    inputs = tuple(t.clone() for t in GEMM_49X3_OP.example_inputs)

    # 1. Compile online; save the key set once.
    _C.set_key_mode(0)  # ONLINE for the initial generation run
    _C.set_key_path("")
    compiled = fhe.compile(
        frontend="torch",
        library="phantom",
        device="cuda",
        encrypt_inputs=GEMM_49X3_OP.encrypt_inputs,
    )(model)
    prog = compiled.fhe_compile(inputs)

    runner = fhe.FHERuntime(prog.package)
    runner.run(*[t.clone() for t in inputs])  # warm up / generate
    key_dir = str(tmp_path / "keys_ond")
    rc = runner.save_keys(key_dir)
    assert rc == 0, f"save_keys failed rc={rc}"
    runner.finalize()

    pkg = {
        "model": GEMM_49X3_OP.name,
        "kernel": prog.package["kernel"],
        "config_path": prog.package.get("config_path", ""),
        "key_path": key_dir,
    }

    # 2. Baseline: PRELOAD_FULL (key_offline) — full galois resident at init.
    _C.set_key_path(key_dir)
    pkg_baseline = dict(pkg, key_mode="key_offline")
    rb = fhe.FHERuntime(pkg_baseline)
    rb.init()
    baseline = rb.run(*[t.clone() for t in inputs]).detach().cpu().flatten().float()
    rb.finalize()

    # 3. ON_DEMAND_FULL — galois keys lazy (Ensure_galois_key on each rotate).
    _C.set_key_path(key_dir)
    pkg_ond = dict(pkg, key_mode="on_demand_full")
    ro = fhe.FHERuntime(pkg_ond)
    ro.init()
    ondemand = ro.run(*[t.clone() for t in inputs]).detach().cpu().flatten().float()
    ro.finalize()
    _C.clear_key_path()

    # 4. Outputs must match.
    max_diff = (baseline - ondemand).abs().max().item()
    assert max_diff < 1e-3, f"ON_DEMAND_FULL output differs from PRELOAD_FULL (max_diff={max_diff})"
