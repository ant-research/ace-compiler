# benchmark/mobilenet/conftest.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# MobileNet-specific fixtures for FHE benchmark tests.
#
# Mirrors the AlexNet/VGG16/SqueezeNet fixture pattern:
#   - conftest defines compiled_mobilenet (indirect fixture factory)
#   - test re-parameterizes with MOBILENET_TARGET_PARAMS (indirect=True)
#
# MobileNet uses ``frontend="torch-via-onnx"`` for the same reason as
# AlexNet/VGG16/SqueezeNet: the direct torch→AIR vectorizer fails on deeper
# conv graphs. Only ant-cpu produces meaningful results — AveragePool2d is
# unsupported on the phantom/ace GPU executors.
#
# MOBILENET_TARGET_PARAMS lives in _fixtures.py alongside TARGET_PARAMS so
# test files can import from there without requiring relative conftest imports.

import pytest
from _fixtures import (
    MOBILENET_TARGET_PARAMS,
    _compile_spec,
)

from ace.model.spec_mobilenet import (
    MOBILENET_CIFAR10,
)


# ============================================================================
# Compiled model fixtures — mirror of AlexNet/VGG16/SqueezeNet's
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
            frontend="torch-via-onnx",  # MobileNet requires torch-via-onnx
            relu_vr_file=vr_file,
        )
        return program, library, device

    return fixture


compiled_mobilenet = pytest.fixture(scope="session")(_make_compiled_fixture(MOBILENET_CIFAR10))


# ============================================================================
# Plaintext model fixture — session-scoped, same as AlexNet/VGG16 pattern.
# ============================================================================


@pytest.fixture(scope="session")
def mobilenet_model():
    """Session-scoped MobileNet model for plaintext inference."""
    model = MOBILENET_CIFAR10.create_model()
    model.eval()
    return model


# ============================================================================
# Pytest configuration (MobileNet-specific markers).
# ============================================================================


def pytest_configure(config):
    config.addinivalue_line("markers", "mobilenet: MobileNet benchmark tests (ant-cpu, torch-via-onnx)")
