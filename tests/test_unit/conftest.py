# tests/test_unit/conftest.py
"""
Pytest configuration and shared fixtures for test_unit.

Provides deterministic input tensor fixtures for unit tests.
Regression/integration tests use generate_inputs_by_mode() instead.
"""

import pytest
import torch


# ============================================================================
# Input Tensor Fixtures (Deterministic, Shared Across test_unit)
# ============================================================================
# These provide deterministic, fixed-value tensors for reproducible unit tests.
# Tests that need random inputs should generate them locally.

@pytest.fixture
def input_1d():
    """1D input tensor with fixed values [1, 2, 3]."""
    return torch.tensor([1.0, 2.0, 3.0])


@pytest.fixture
def input_1d_another():
    """Another 1D input tensor with fixed values [4, 5, 6]."""
    return torch.tensor([4.0, 5.0, 6.0])


@pytest.fixture
def input_2d():
    """2D input tensor with fixed values [[1,2],[3,4]]."""
    return torch.tensor([[1.0, 2.0], [3.0, 4.0]])


@pytest.fixture
def input_2d_another():
    """Another 2D input tensor with fixed values [[5,6],[7,8]]."""
    return torch.tensor([[5.0, 6.0], [7.0, 8.0]])


@pytest.fixture
def input_4d():
    """4D input tensor (1,1,2,2) with fixed values."""
    return torch.tensor([[[[1.0, 2.0], [3.0, 4.0]]]])


@pytest.fixture
def conv_weight():
    """Convolution weight tensor (1,1,2,2) with fixed values."""
    return torch.tensor([[[[1.0, 2.0], [3.0, 4.0]]]])


@pytest.fixture
def conv_bias():
    """Convolution bias tensor."""
    return torch.tensor([0.5])


@pytest.fixture
def gemm_bias():
    """GEMM bias tensor (2,2)."""
    return torch.tensor([[0.1, 0.2], [0.3, 0.4]])