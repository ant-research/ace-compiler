# benchmark/runtime/conftest.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# Runtime configuration benchmark fixtures.
#
# This directory benchmarks runtime-level configuration dimensions
# (key mode, IO policy, session switching) that are independent of
# the specific model. The model is a configurable carrier — default
# ResNet-20, overridable via ACE_BENCH_MODEL env var.
#
# Self-contained: no imports from benchmark/resnet/. When this
# directory is migrated to a standalone ace-benchmark repo, it
# can be moved as-is.

import os
import shutil
import tempfile

import pytest

# Re-use shared compile helper and GPU tag from benchmark root
from _fixtures import _compile_spec, gpu_tag

from ace import fhe
from ace import runtime as _C
from ace.fhe.config import IoPolicySettings
from ace.model.spec_resnet import (
    RESNET20_CIFAR10,
    RESNET32_CIFAR10,
    RESNET56_CIFAR10,
)


# ============================================================================
# Model selection — configurable via environment variable
# ============================================================================

_BENCH_MODEL = os.environ.get("ACE_BENCH_MODEL", "resnet20")

_MODEL_SPECS = {
    "resnet20": RESNET20_CIFAR10,
    "resnet32": RESNET32_CIFAR10,
    "resnet56": RESNET56_CIFAR10,
}

if _BENCH_MODEL not in _MODEL_SPECS:
    raise ValueError(f"Unknown ACE_BENCH_MODEL='{_BENCH_MODEL}'. Choose from: {', '.join(sorted(_MODEL_SPECS))}")

MODEL_SPEC = _MODEL_SPECS[_BENCH_MODEL]


# ============================================================================
# GPU / backend parameters
# ============================================================================

_gpu_available = fhe.gpu_available()

GPU_PARAMS = [
    pytest.param(
        ("phantom", "cuda"),
        marks=pytest.mark.skipif(not _gpu_available, reason="GPU not available"),
        id="phantom-cuda",
    ),
    pytest.param(
        ("ace", "cuda"),
        marks=pytest.mark.skipif(not _gpu_available, reason="GPU not available"),
        id="ace-cuda",
    ),
]

KEY_MODE_PARAMS = [
    pytest.param("online", id="online"),
    pytest.param(
        "key_offline",
        marks=pytest.mark.skipif(not _gpu_available, reason="GPU needed for key save"),
        id="key_offline",
    ),
    pytest.param(
        "server_only",
        marks=pytest.mark.skipif(not _gpu_available, reason="GPU needed for key save"),
        id="server_only",
    ),
]

IO_POLICY_PARAMS = [
    pytest.param("staged", id="staged"),
    pytest.param("mapped", id="mapped"),
    pytest.param("auto", id="auto"),
]


# ============================================================================
# Key mode package helper
# ============================================================================


def setup_key_mode_package(program, key_mode, tmp_dir, library="phantom", runner=None):
    """Set up a package dict for the given key_mode.

    For key_offline / server_only, keys are saved via the supplied live
    ``runner`` (its FheContext must be active — i.e. call BEFORE
    ``runner.finalize()``). This replaces the retired ctypes FHert_/Phantom_/Ace_*
    path, which required finalize-first and re-dlopen'd the backend.

    Args:
        program: CompiledProgram for the package/kernel/config fields.
        key_mode: One of "online", "key_offline", "server_only".
        tmp_dir: Temporary directory for key storage.
        library: Backend library name ("phantom" or "ace") — kept for API
            compatibility; no longer used to pick a ctypes prefix.
        runner: A live FHERuntime with an active context; REQUIRED for
            key_offline / server_only (used to call runner.save_keys(...)).

    Returns:
        (package_dict, key_dir_or_none)
    """
    if key_mode == "online":
        return program.package, None

    if key_mode == "key_offline":
        if runner is None:
            raise ValueError("setup_key_mode_package: key_offline requires a live runner to save keys")
        key_dir = tempfile.mkdtemp(prefix="bench_key_offline_", dir=tmp_dir)
        rc = runner.save_keys(key_dir)
        assert rc == 0, f"runner.save_keys failed rc={rc}"

        IoPolicySettings(policy="auto").apply()
        _C.set_key_path(key_dir)
        package = {
            **program.package,
            "key_path": key_dir,
            "key_mode": "key_offline",
        }
        return package, key_dir

    elif key_mode == "server_only":
        if runner is None:
            raise ValueError("setup_key_mode_package: server_only requires a live runner to save keys")
        eval_dir = tempfile.mkdtemp(prefix="bench_server_only_", dir=tmp_dir)
        rc = runner.save_keys(eval_dir, eval_only=True)
        assert rc == 0, f"runner.save_keys(eval_only) failed rc={rc}"

        IoPolicySettings(policy="auto").apply()
        _C.set_key_path(eval_dir)
        package = {
            **program.package,
            "key_path": eval_dir,
            "key_mode": "server_only",
        }
        return package, eval_dir

    else:
        raise ValueError(f"Unknown key_mode: {key_mode}")


# ============================================================================
# Fixtures
# ============================================================================


@pytest.fixture(scope="module")
def compiled_model(request):
    """Compile the benchmark model for the given backend.

    Returns (program, library, device).
    """
    library, device = request.param
    vr_file = MODEL_SPEC.get_vr_profile()
    program = _compile_spec(MODEL_SPEC, library=library, device=device, relu_vr_file=vr_file)
    return program, library, device


@pytest.fixture(scope="module")
def cifar10_images():
    """Load CIFAR-10 images for benchmarking."""
    from ace.model.dataset import load_cifar10_images

    images, labels = load_cifar10_images(3)
    return images, labels


# ============================================================================
# Pytest configuration
# ============================================================================


def pytest_configure(config):
    config.addinivalue_line("markers", "runtime: Runtime configuration benchmarks")
