# tests/unit/frontend/conftest.py
"""
Pytest configuration and shared fixtures for frontend tests.
"""
import pytest

from ace.fhe.frontend import get_frontend


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
def onnx_frontend():
    """Fixture for onnx frontend."""
    return get_frontend("onnx")