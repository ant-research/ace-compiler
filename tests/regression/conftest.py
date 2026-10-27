# tests/regression/conftest.py
"""
Fixtures for regression tests.

Provides:
- ResNet-specific fixtures
- Custom pytest markers
- Test ordering: CPU smoke tests run before CUDA smoke tests to avoid
  CUDA runtime state corrupting the CPU FHE runtime (segfault).
"""

import pytest


# Import ResNet model test cases
try:
    from ace.model.spec_resnet import ALL_RESNET_SPECS as RESNET_MODEL_TEST_CASES
except ImportError:
    RESNET_MODEL_TEST_CASES = []


# ============================================================================
# Multi-user smoke matrix helpers (test_mu_*.py)
#
# Shared by the three process-model files (multiprocess / serial / parallel).
# GEMM_49X3_OP rotates, so ON_DEMAND's lazy Ensure_galois_key path is actually
# exercised — an op without rotate would only touch the empty-shell init and
# pass for the wrong reason.
# ============================================================================

import os
import shutil

import torch

from ace import fhe
from ace import runtime as _C
from ace.fhe.backend import get_library_impl
from ace.fhe.config import IoConfig, IoPolicy, IoPolicySettings
from ace.sample.ops.specs import GEMM_49X3_OP as _MU_SPEC


# ============================================================================
# ResNet Model Case Fixture
# ============================================================================


@pytest.fixture(params=RESNET_MODEL_TEST_CASES, ids=lambda tc: tc.name)
def resnet_case(request):
    """Parametrized fixture for ResNet model test cases."""
    return request.param


# ============================================================================
# Pytest Configuration
# ============================================================================


def pytest_configure(config):
    """Register custom markers (supplements pytest.ini declarations)."""
    pass


def pytest_collection_modifyitems(session, config, items):
    """Reorder smoke tests so CPU (ant) tests run before CUDA tests.

    Running CUDA smoke tests (ace, phantom) before CPU tests (ant) can
    leave the CUDA runtime in a state that causes the CPU FHE runtime to
    segfault.  Moving ant smoke tests first avoids this.
    """
    ant_items = []
    other_items = []
    for item in items:
        if "test_sample_smoke_antlib" in item.nodeid:
            ant_items.append(item)
        else:
            other_items.append(item)
    items[:] = ant_items + other_items


# ============================================================================
# Multi-user smoke matrix helpers (test_mu_*.py) — imports + tolerances
#


# Cross-user / cross-process agreement tolerance. CKKS noise on gemm_49x3 is
# ~1e-8 vs magnitudes ~1; 0.1 is safe and matches the multi-user integration anchor.
MU_AGREE_ATOL = 0.1
MU_NUM_USERS = 2
MU_INPUT_SEED = 4242


def mu_provider_available(name, device="cuda"):
    try:
        return get_library_impl(name, device=device).check_available()
    except Exception:
        return False


def mu_free_gpus(n):
    """Return up to n GPU indices showing < 500 MiB used, or None if < n free."""
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


@pytest.fixture
def staged_io_workaround():
    """This host has false-available cuFile (Bar1 unavailable) -> galois-key
    batched read dead-waits. Force staged IO + single-thread galois load +
    per-key format. NOT autouse: each test_mu_* module opts in via
    ``pytestmark = pytest.mark.usefixtures("staged_io_workaround")`` so ResNet/
    sample tests in this dir are unaffected. Restores env + clears global key
    state on exit so cells don't leak into each other.
    """
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
        _C.clear_key_path()
        _C.set_key_mode(0)  # ONLINE


def mu_compile_once():
    """Compile GEMM once; reuse the .so across cells. Returns (prog, inputs)."""
    model = _MU_SPEC.create_model()
    model.eval()
    compiled = fhe.compile(
        frontend="torch",
        library="phantom",
        device="cuda",
        encrypt_inputs=_MU_SPEC.encrypt_inputs,
    )(model)
    inputs = tuple(t.clone() for t in _MU_SPEC.example_inputs)
    prog = compiled.fhe_compile(inputs)
    return prog, inputs


def mu_package(prog, key_dir=None, key_mode=None):
    pkg = {
        "model": _MU_SPEC.name,
        "kernel": prog.package["kernel"],
        "config_path": prog.package.get("config_path", ""),
    }
    if key_dir is not None:
        pkg["key_path"] = key_dir
    if key_mode is not None:
        pkg["key_mode"] = key_mode
    return pkg


def _mu_gen_worker(key_dir, package, input_arrays, out_q):
    """Spawned: generate ONE key bundle in a FRESH process.

    Must be multi-process: generating 2+ key bundles in the SAME process hits
    phantom's FheContext rebuild-residue bug → the 2nd+ bundle is contaminated
    → inference with it produces garbage (e.g. magnitude ~2000 vs ~0.2). Each
    bundle in its own fresh process avoids the rebuild entirely. This mirrors
    examples/.../generate_keys.py (the verified-correct gen path).
    """
    os.environ["CUDA_VISIBLE_DEVICES"] = os.environ.get("CUDA_VISIBLE_DEVICES", "0")
    os.environ.setdefault("ACE_GDS_ENABLED", "0")
    os.environ.setdefault("FHERT_GALOIS_LOAD_THREADS", "1")
    import importlib

    importlib.invalidate_caches()
    from ace import fhe
    from ace.fhe.config import IoConfig, IoPolicy, IoPolicySettings

    IoPolicySettings(policy=IoPolicy.STAGED).apply()
    IoConfig(galois_merged_format=False).apply()
    try:
        from ace import runtime as _C

        _C.set_key_mode(0)  # ONLINE
        _C.set_key_path("")
        runner = fhe.FHERuntime(package)
        tensors = [torch.tensor(a, dtype=torch.float32) for a in input_arrays]
        runner.run(*tensors)
        rc = runner.save_keys(key_dir)
        out_q.put({"rc": rc, "err": ""})
    except Exception as e:
        import traceback

        out_q.put({"rc": -1, "err": f"{type(e).__name__}: {e}\n{traceback.format_exc()}"})


def mu_gen_keys(prog, inputs, key_dir):
    """Generate ONE key bundle in a fresh child process; ONLINE gen + save.

    Multi-process on purpose (see _mu_gen_worker). Caller owns cleanup.
    """
    import multiprocessing as mp

    # package must be picklable for spawn: kernel is a .so path string, fine.
    package = mu_package(prog)
    input_arrays = [t.detach().cpu().tolist() for t in inputs]
    ctx = mp.get_context("spawn")
    q = ctx.Queue()
    p = ctx.Process(target=_mu_gen_worker, args=(str(key_dir), package, input_arrays, q))
    p.start()
    r = q.get(timeout=300)
    p.join(timeout=30)
    if p.is_alive():
        p.terminate()
    assert r["rc"] == 0, f"mu_gen_keys failed for {key_dir}: {r['err']}"


def mu_run(prog, inputs, key_dir, key_mode):
    """Single-user inference under a key mode; returns flattened float output."""
    _C.set_key_path(key_dir)
    runner = fhe.FHERuntime(mu_package(prog, key_dir=key_dir, key_mode=key_mode))
    runner.init()
    out = runner.run(*[t.clone() for t in inputs]).detach().cpu().flatten().float()
    runner.finalize()
    _C.clear_key_path()
    return out


def mu_agree(a, b, what):
    # Mirror examples/.../03_server_serial.py: relative+absolute tolerance, since
    # gemm outputs are large-magnitude and a pure atol is too strict. CKKS noise
    # scales with magnitude, so rtol=0.05 is the meaningful bound.
    d = (a - b).abs().max().item()
    mag = max(a.abs().max().item(), b.abs().max().item(), 1.0)
    assert torch.allclose(a, b, atol=MU_AGREE_ATOL, rtol=0.05), (
        f"{what}: max_diff={d} (rtol=0.05 atol={MU_AGREE_ATOL}); |out|max~{mag:.1f}"
    )


def mu_requires_phantom():
    return pytest.mark.skipif(not mu_provider_available("phantom", "cuda"), reason="phantom-cuda not available")
