#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# LR-specific fixtures for FHE benchmark tests.
#
# LR is a single GEMM with no ReLU — no VR profiling needed. Compiled once
# per (library, device) with session scope because FHE compilation is slow.

import pytest

from ace import fhe
from ace.model.spec_lr import LR_1D

_gpu_available = fhe.gpu_available()

# Shared utilities from benchmark/_fixtures.py (via benchmark/conftest.py)
from _fixtures import _compile_spec, gpu_tag  # noqa: F401  (gpu_tag re-exported)

# LR/LeNet are covered on ant + phantom only. ace-CUDA is deliberately
# excluded: like other single-GEMM ops it hits the known "ace-library
# inference error" (see tests/regression/sample/test_sample_smoke_ace.py).
SMALL_TARGET_PARAMS = [
    pytest.param(("ant", "cpu"), id="ant-cpu"),
    pytest.param(
        ("phantom", "cuda"),
        marks=pytest.mark.skipif(not _gpu_available, reason="GPU not available"),
        id="phantom-cuda",
    ),
]


def _make_compiled_fixture(spec):
    """Factory for compiled LR fixture. Returns (program, library, device)."""

    def fixture(request):
        library, device = request.param
        program = _compile_spec(spec, library=library, device=device)
        return program, library, device

    return fixture


compiled_lr = pytest.fixture(scope="session")(_make_compiled_fixture(LR_1D))


# ============================================================================
# Batch of synthetic samples for latency/memory (LR input is [1, 16])
# ============================================================================


@pytest.fixture(scope="session")
def lr_sample_batch():
    """10 copies of the deterministic LR example input."""
    import torch

    x = LR_1D.example_inputs[0]  # [1, 16]
    images = x.repeat(10, 1)     # [10, 16]
    labels = [0] * 10            # synthetic labels (toy)
    return images, labels


def pytest_configure(config):
    config.addinivalue_line("markers", "lr: LR benchmark tests")
