#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Shared fixtures for driver tests.

Contains input tensors and test utilities for testing Driver driver flow.
"""

import pytest
import torch
import torch.nn as nn

# Import centralized dependency checks from test_utils
from test_utils import (
    TORCH_AVAILABLE,
    TORCH_FX_AVAILABLE,
    HAS_TORCH_FX,
    HAS_FRONTEND,
    skip_if_no_torch,
    skip_if_no_torch_fx,
    skip_if_no_frontend,
)

# Import test models and functions from ace.samples
from ace.samples.ops import AddOp as AddModel
from ace.samples.funcs import (
    add_func,
    mul_func,
)


def simple_function(x):
    """Simple function that adds 1."""
    return x + 1


# ============================================================================
# Skip Markers (imported from root conftest)
# ============================================================================

# Markers are registered in root conftest.py
# Input tensor fixtures (input_1d, input_4d, etc.) are defined in tests/conftest.py