#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Shared fixtures for backend tests.
"""

import pytest
import torch


# ============================================================================
# Input Tensor Fixtures
# ============================================================================

@pytest.fixture
def input_1d():
    """1D input tensor."""
    return torch.randn(1, 4)


@pytest.fixture
def input_2d():
    """2D input tensor (NCHW)."""
    return torch.randn(1, 1, 4, 4)


@pytest.fixture
def input_4d():
    """4D input tensor (NCHW)."""
    return torch.randn(1, 3, 8, 8)