# benchmark/vgg16/conftest.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# VGG16-specific fixtures for FHE benchmark tests.
#
# Mirrors the ResNet fixture pattern:
#   - conftest defines compiled_vgg16 (indirect fixture factory)
#   - test re-parameterizes with VGG16_TARGET_PARAMS (indirect=True)
#
# VGG16 uses ``frontend="torch-via-onnx"`` instead of the default "torch"
# because the direct torch→AIR vectorizer fails on VGG16's 4th conv block
# onward (vector_gen.cxx:196: roll num attribute should not be empty).
# torch-via-onnx bypasses AIR, so compilation succeeds, but GPU runtime
# still fails due to insufficient CKKS noise budget for this deep model.
# Only ant-cpu produces meaningful results.
#
# VGG16_TARGET_PARAMS lives in _fixtures.py alongside TARGET_PARAMS so test
# files can import from there without requiring relative conftest imports.

import pytest

from _fixtures import (
    VGG16_TARGET_PARAMS,
    _compile_spec,
)
from ace.model.spec_vgg16 import (
    VGG16_CIFAR10,
)


# ============================================================================
# Compiled model fixtures — mirror of ResNet's _make_compiled_fixture
#
# The fixture factory accepts request.param = (library, device), just like
# ResNet. The only difference is passing frontend="torch-via-onnx" to
# _compile_spec (required because the torch→AIR vectorizer fails on VGG16's
# 4th conv block).
#
# NOTE: the VGG16 fixture uses unconditional pytest.mark.skip for GPU
# backends (not skipif), because the GPU skip is always active regardless
# of whether CUDA is available — AveragePool2d is unsupported on GPU
# executors, and even on GPU without AveragePool2d the model is too deep
# for phantom CKKS noise budget.
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
            frontend="torch-via-onnx",  # VGG16 requires torch-via-onnx
            relu_vr_file=vr_file,
        )
        return program, library, device

    return fixture


compiled_vgg16 = pytest.fixture(scope="session")(_make_compiled_fixture(VGG16_CIFAR10))


# ============================================================================
# Plaintext model fixture — session-scoped, same as ResNet pattern.
# ============================================================================


@pytest.fixture(scope="session")
def vgg16_model():
    """Session-scoped VGG16 model for plaintext inference."""
    model = VGG16_CIFAR10.create_model()
    model.eval()
    return model


# ============================================================================
# Pytest configuration (VGG16-specific markers).
# ============================================================================


def pytest_configure(config):
    config.addinivalue_line("markers", "vgg16: VGG16 benchmark tests (ant-cpu, torch-via-onnx)")