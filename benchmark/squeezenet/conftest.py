# benchmark/squeezenet/conftest.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# SqueezeNet-specific fixtures for FHE benchmark tests.
#
# Mirrors the AlexNet/VGG16 fixture pattern:
#   - conftest defines compiled_squeezenet (indirect fixture factory)
#   - test re-parameterizes with SQUEEZENET_TARGET_PARAMS (indirect=True)
#
# SqueezeNet uses ``frontend="torch-via-onnx"`` for the same reason as
# AlexNet/VGG16: the direct torch→AIR vectorizer fails on deeper conv graphs.
# Only ant-cpu produces meaningful results — AveragePool2d and Concat are
# unsupported on the phantom/ace GPU executors.
#
# SQUEEZENET_TARGET_PARAMS lives in _fixtures.py alongside TARGET_PARAMS so
# test files can import from there without requiring relative conftest imports.

import pytest
from _fixtures import (
    SQUEEZENET_TARGET_PARAMS,
    _compile_spec,
)

from ace.model.spec_squeezenet import (
    SQUEEZENET_CIFAR10,
)


# ============================================================================
# Compiled model fixtures — mirror of AlexNet/VGG16's
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
            frontend="torch-via-onnx",  # SqueezeNet requires torch-via-onnx
            relu_vr_file=vr_file,
        )
        return program, library, device

    return fixture


compiled_squeezenet = pytest.fixture(scope="session")(_make_compiled_fixture(SQUEEZENET_CIFAR10))


# ============================================================================
# Plaintext model fixture — session-scoped, same as AlexNet/VGG16 pattern.
# ============================================================================


@pytest.fixture(scope="session")
def squeezenet_model():
    """Session-scoped SqueezeNet model for plaintext inference."""
    model = SQUEEZENET_CIFAR10.create_model()
    model.eval()
    return model


# ============================================================================
# Pytest configuration (SqueezeNet-specific markers).
# ============================================================================


def pytest_configure(config):
    config.addinivalue_line("markers", "squeezenet: SqueezeNet benchmark tests (ant-cpu, torch-via-onnx)")
