# benchmark/conftest.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#
# Shared fixtures for all FHE benchmark tests.
# Model-specific fixtures live in each model subdirectory's conftest.py
# (e.g. benchmark/resnet/conftest.py).
#
# Shared utilities (TARGET_PARAMS, _compile_spec, etc.) are defined in
# benchmark/_fixtures.py so that sub-conftests can import them without
# circular-import issues.

import pytest

# Re-export shared utilities for test files that import from conftest
from _fixtures import (
    TARGET_PARAMS,
    _compile_spec,
    _load_images,
)


# ============================================================================
# Session-scoped dataset fixtures
# ============================================================================


@pytest.fixture(scope="session")
def cifar10_images():
    """Load CIFAR-10 test images (up to 1000)."""
    from _fixtures import _MAX_IMAGES

    images, labels, class_names = _load_images("cifar10", _MAX_IMAGES)
    return images, labels, class_names


@pytest.fixture(scope="session")
def cifar100_images():
    """Load CIFAR-100 test images (up to 1000)."""
    from _fixtures import _MAX_IMAGES

    images, labels, class_names = _load_images("cifar100", _MAX_IMAGES)
    return images, labels, class_names


# ============================================================================
# Pytest configuration
# ============================================================================


def pytest_configure(config):
    config.addinivalue_line("markers", "slow: Slow tests (large-scale benchmarks)")
