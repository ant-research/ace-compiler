# tests/conftest.py
"""
Pytest configuration and shared fixtures for ANT-ACE tests.

For dependency checks and skip markers, import from test_utils:
    from test_utils import TORCH_AVAILABLE, HAS_FRONTEND, skip_if_no_torch

Note: Most tests use the FHE compilation cache mechanism. Only tests that need
to export files to disk should use pytest's built-in `tmp_path` fixture.
"""
import pytest
import os

# Set default log level for tests - DEBUG for development
# Can be overridden via environment variable
os.environ.setdefault("ACE_LOG_LEVEL", "DEBUG")

# Import only what's needed for fixtures
import torch
import torch.nn as nn

from test_utils.dependencies import (
    TORCH_AVAILABLE,
    MODEL_TEST_CASES,
    FUNCTION_TEST_CASES,
    ASTToIRConverter,
    TORCH_IR_AVAILABLE,
    TorchExportToIRPipeline,
)


# ============================================================================
# Test Case Utilities
# ============================================================================

def get_case_by_name(cases, name):
    """Get test case by name from a list of cases."""
    for tc in cases:
        if tc.name == name:
            return tc
    available = [tc.name for tc in cases]
    raise ValueError(f"Test case '{name}' not found. Available: {available}")


# ============================================================================
# Model Test Case Fixtures
# ============================================================================

@pytest.fixture(scope="session")
def all_model_cases():
    """Return all model test cases (list)."""
    return MODEL_TEST_CASES


@pytest.fixture(scope="session")
def get_model_case_by_name(all_model_cases):
    """Return a function to get model test case by name."""
    def _get(name):
        return get_case_by_name(all_model_cases, name)
    return _get


@pytest.fixture(params=MODEL_TEST_CASES, ids=lambda tc: tc.name)
def model_case(request):
    """Parametrized fixture - returns single model test case for each test."""
    return request.param


# ============================================================================
# Function Test Case Fixtures
# ============================================================================

@pytest.fixture(scope="session")
def all_func_cases():
    """Return all function test cases (list)."""
    return FUNCTION_TEST_CASES


@pytest.fixture(scope="session")
def get_func_case_by_name(all_func_cases):
    """Return a function to get function test case by name."""
    def _get(name):
        return get_case_by_name(all_func_cases, name)
    return _get


@pytest.fixture(params=FUNCTION_TEST_CASES, ids=lambda tc: tc.name)
def func_case(request):
    """Parametrized fixture - returns single function test case for each test."""
    return request.param


# ============================================================================
# Converter Fixtures
# ============================================================================

@pytest.fixture
def ir_converter():
    """Return an IR converter (AST to IR)."""
    if ASTToIRConverter is None:
        pytest.skip("ASTToIRConverter not available")
    return ASTToIRConverter()


@pytest.fixture
def torch_ir_converter():
    """Return a Torch to IR pipeline converter."""
    if not TORCH_IR_AVAILABLE:
        pytest.skip("TorchExportToIRPipeline not available")
    return TorchExportToIRPipeline()


# ============================================================================
# Model Fixtures
# ============================================================================

@pytest.fixture
def simple_model():
    """Simple linear model (4 -> 2) for testing."""
    if not TORCH_AVAILABLE:
        pytest.skip("torch not available")
    from ace.samples.ops import LinearOp
    return LinearOp(4, 2)


@pytest.fixture
def example_inputs():
    """Random input tensors for testing."""
    if not TORCH_AVAILABLE:
        pytest.skip("torch not available")
    return [torch.randn(1, 4)]


# ============================================================================
# ============================================================================
# Pytest Configuration
# ============================================================================

def pytest_configure(config):
    """Register custom markers."""
    config.addinivalue_line(
        "markers", "gpu: mark test as requiring GPU (skipped if no GPU)"
    )
    config.addinivalue_line(
        "markers", "skip_if_no_torch: skip if torch not available"
    )
    config.addinivalue_line(
        "markers", "skip_if_no_torch_fx: skip if torch.fx not available"
    )
    config.addinivalue_line(
        "markers", "skip_if_no_frontend: skip if ace extension not available"
    )
    config.addinivalue_line(
        "markers", "skip_if_no_fhe: skip if ace.fhe not available"
    )