# benchmark/alexnet/conftest.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# AlexNet-specific fixtures for FHE benchmark tests.
#
# Mirrors the ResNet fixture pattern:
#   - conftest defines compiled_alexnet (indirect fixture factory)
#   - test re-parameterizes with ALEXNET_TARGET_PARAMS (indirect=True)
#
# AlexNet uses ``frontend="torch-via-onnx"`` instead of the default "torch"
# because AveragePool2d segfaults on phantom/ace GPU executors and the direct
# torch→AIR vectorizer fails on deep conv graphs (vector_gen.cxx:196: roll num
# attribute should not be empty). torch-via-onnx bypasses AIR, so compilation
# succeeds but GPU runtime still fails (too deep for phantom CKKS noise budget).
# Only ant-cpu produces meaningful results.
#
# ALEXNET_TARGET_PARAMS lives in _fixtures.py alongside TARGET_PARAMS so test
# files can import from there without requiring relative conftest imports.

import pytest

from _fixtures import (
    ALEXNET_TARGET_PARAMS,
    _compile_spec,
)
from ace.model.spec_alexnet import (
    ALEXNET_CIFAR10,
)


# ============================================================================
# Compiled model fixtures — mirror of ResNet's _make_compiled_fixture
#
# The fixture factory accepts request.param = (library, device), just like
# ResNet. The only difference is passing frontend="torch-via-onnx" to
# _compile_spec (required because the torch→AIR vectorizer fails on AlexNet's
# depth; the ONNX route produces valid AIR IR with proper Roll attributes).
# ============================================================================


def _make_compiled_fixture(spec):
    """Factory for compiled model fixtures. Returns (program, library, device)."""

    def fixture(request):
        library, device = request.param
        vr_file = spec.get_vr_profile()
        program = _compile_spec(
            spec,
            library=library,
            device=device,
            frontend="torch-via-onnx",  # AlexNet requires torch-via-onnx
            relu_vr_file=vr_file,
        )
        return program, library, device

    return fixture


compiled_alexnet = pytest.fixture(scope="session")(_make_compiled_fixture(ALEXNET_CIFAR10))


# ============================================================================
# Plaintext model fixture — session-scoped, same as ResNet pattern.
# ============================================================================


@pytest.fixture(scope="session")
def alexnet_model():
    """Session-scoped AlexNet model for plaintext inference."""
    model = ALEXNET_CIFAR10.create_model()
    model.eval()
    return model


# ============================================================================
# Pytest configuration (AlexNet-specific markers).
# ============================================================================


def pytest_configure(config):
    config.addinivalue_line("markers", "alexnet: AlexNet benchmark tests (ant-cpu, torch-via-onnx)")