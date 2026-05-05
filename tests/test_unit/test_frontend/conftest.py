# tests/test_unit/test_frontend/conftest.py
"""
Pytest configuration and shared fixtures for frontend tests.

Provides:
- Common fixtures for all frontend tests
- Shared test models and functions
- Parametrized fixtures for model/function testing
"""
import pytest
import torch
import torch.nn as nn

from ace.fhe.frontend import get_frontend
from ace.samples.funcs import (
    add_func, sub_func, mul_func, div_func,
    relu_func, sigmoid_func, tanh_func,
)

# Single-argument helper function for edge case tests
def single_arg_add(x):
    """Single argument add function."""
    return x + 1
from ace.samples.ops import (
    AddOp, SubOp, MulOp, DivOp,
    ReluOp, SigmoidOp, TanhOp,
    LinearOp, Conv2dOp, AvgPool2dOp,
)


# ============================================================================
# Pytest Configuration
# ============================================================================

def pytest_collection_modifyitems(items):
    """Mark all frontend tests with @pytest.mark.forked for subprocess isolation."""
    for item in items:
        item.add_marker(pytest.mark.forked)


# ============================================================================
# Frontend Fixtures
# ============================================================================

@pytest.fixture
def torch_frontend():
    """Fixture for torch frontend."""
    return get_frontend("torch")


@pytest.fixture
def torch_via_onnx_frontend():
    """Fixture for torch-via-onnx frontend."""
    return get_frontend("torch-via-onnx")


@pytest.fixture
def ast_frontend():
    """Fixture for ast frontend."""
    return get_frontend("ast")


@pytest.fixture
def ast_via_onnx_frontend():
    """Fixture for ast-via-onnx frontend."""
    return get_frontend("ast-via-onnx")


@pytest.fixture
def onnx_frontend():
    """Fixture for onnx frontend."""
    return get_frontend("onnx")


# ============================================================================
# Model Fixtures
# ============================================================================

@pytest.fixture
def add_model():
    """Add operator model."""
    return AddOp()


@pytest.fixture
def sub_model():
    """Sub operator model."""
    return SubOp()


@pytest.fixture
def mul_model():
    """Mul operator model."""
    return MulOp()


@pytest.fixture
def div_model():
    """Div operator model."""
    return DivOp()


@pytest.fixture
def relu_model():
    """ReLU operator model."""
    return ReluOp()


@pytest.fixture
def sigmoid_model():
    """Sigmoid operator model."""
    return SigmoidOp()


@pytest.fixture
def tanh_model():
    """Tanh operator model."""
    return TanhOp()


@pytest.fixture
def linear_model():
    """Linear operator model."""
    return LinearOp(4, 4)


@pytest.fixture
def conv_model():
    """Conv2d operator model."""
    return Conv2dOp(3, 16, 3)


@pytest.fixture
def pool_model():
    """AvgPool2d operator model."""
    return AvgPool2dOp(2, 2)


# ============================================================================
# Function Fixtures
# ============================================================================

@pytest.fixture
def add_function():
    """Add function."""
    return add_func


@pytest.fixture
def sub_function():
    """Sub function."""
    return sub_func


@pytest.fixture
def mul_function():
    """Mul function."""
    return mul_func


@pytest.fixture
def relu_function():
    """ReLU function."""
    return relu_func


@pytest.fixture
def single_arg_add_fixture():
    """Single argument add function for edge case tests."""
    return single_arg_add


# ============================================================================
# Input Tensor Fixtures
# ============================================================================

@pytest.fixture
def input_1d():
    """1D input tensor (1, 4)."""
    return torch.randn(1, 4)


@pytest.fixture
def input_2d():
    """2D input tensor (1, 1, 4, 4)."""
    return torch.randn(1, 1, 4, 4)


@pytest.fixture
def input_4d():
    """4D input tensor (1, 3, 8, 8)."""
    return torch.randn(1, 3, 8, 8)


@pytest.fixture
def input_pair():
    """Pair of 1D input tensors."""
    return torch.randn(1, 4), torch.randn(1, 4)


# ============================================================================
# Parametrized Fixtures
# ============================================================================

@pytest.fixture(params=["model", "function"])
def source_type(request):
    """Parametrized fixture for source type."""
    return request.param


@pytest.fixture(params=["torch", "torch-via-onnx", "ast", "ast-via-onnx", "onnx"])
def frontend_name(request):
    """Parametrized fixture for frontend name."""
    return request.param


@pytest.fixture
def frontend(frontend_name):
    """Get frontend by name."""
    return get_frontend(frontend_name)