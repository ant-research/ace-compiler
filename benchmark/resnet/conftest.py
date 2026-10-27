# benchmark/resnet/conftest.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# ResNet-specific fixtures for FHE benchmark tests.
# Shared utilities (TARGET_PARAMS, _compile_spec, etc.) come from
# benchmark/_fixtures.py via benchmark/conftest.py.

import pytest

# Shared utilities - available to test files via `from conftest import ...`
from _fixtures import (
    TARGET_PARAMS,
    _compile_spec,
)

# ============================================================================
# Model imports
# ============================================================================
from ace.model.spec_resnet import (
    RESNET20_CIFAR10,
    RESNET32_CIFAR10,
    RESNET32_CIFAR100,
    RESNET44_CIFAR10,
    RESNET56_CIFAR10,
    RESNET110_CIFAR10,
)


# ============================================================================
# Compiled model fixtures (parameterized by (library, device) tuple)
#
# Each fixture returns (program, library, device) so tests can use library/device
# for baseline keys without probing internal attributes.
#
# scope="session": FHE compilation is expensive (minutes for large models).
# The compiled program is read-only once created, so it's safe to reuse
# across all test modules in the same pytest session.
# ============================================================================


def _make_compiled_fixture(spec):
    """Factory for compiled model fixtures. Returns (program, library, device)."""

    def fixture(request):
        library, device = request.param
        vr_file = spec.get_vr_profile()
        program = _compile_spec(spec, library=library, device=device, relu_vr_file=vr_file)
        return program, library, device

    return fixture


compiled_resnet_20 = pytest.fixture(scope="session")(_make_compiled_fixture(RESNET20_CIFAR10))
compiled_resnet_32 = pytest.fixture(scope="session")(_make_compiled_fixture(RESNET32_CIFAR10))
compiled_resnet_44 = pytest.fixture(scope="session")(_make_compiled_fixture(RESNET44_CIFAR10))
compiled_resnet_56 = pytest.fixture(scope="session")(_make_compiled_fixture(RESNET56_CIFAR10))
compiled_resnet_110 = pytest.fixture(scope="session")(_make_compiled_fixture(RESNET110_CIFAR10))
compiled_resnet_32_cifar100 = pytest.fixture(scope="session")(_make_compiled_fixture(RESNET32_CIFAR100))


# ============================================================================
# Plaintext model fixtures - also session-scoped since models are read-only.
# ============================================================================


@pytest.fixture(scope="session")
def resnet20_model():
    """Session-scoped ResNet-20 model for plaintext inference."""
    model = RESNET20_CIFAR10.create_model()
    model.eval()
    return model


@pytest.fixture(scope="session")
def resnet32_model():
    model = RESNET32_CIFAR10.create_model()
    model.eval()
    return model


@pytest.fixture(scope="session")
def resnet44_model():
    model = RESNET44_CIFAR10.create_model()
    model.eval()
    return model


@pytest.fixture(scope="session")
def resnet56_model():
    model = RESNET56_CIFAR10.create_model()
    model.eval()
    return model


@pytest.fixture(scope="session")
def resnet110_model():
    model = RESNET110_CIFAR10.create_model()
    model.eval()
    return model


@pytest.fixture(scope="session")
def resnet32_cifar100_model():
    model = RESNET32_CIFAR100.create_model()
    model.eval()
    return model


# ============================================================================
# Pytest configuration (ResNet-specific markers)
# ============================================================================


def pytest_configure(config):
    config.addinivalue_line("markers", "resnet: ResNet benchmark tests")
