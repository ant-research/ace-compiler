# tests/conftest.py
"""
Pytest configuration and shared fixtures for ACE-Compiler tests.

Note: Most tests use the FHE compilation cache mechanism. Only tests that need
to export files to disk should use pytest's built-in `tmp_path` fixture.
"""
import pytest


# ============================================================================
# Target Parameters (library, device) for parametrized tests
# ============================================================================

try:
    from ace import fhe
    _gpu_available = fhe.gpu_available()
except ImportError:
    _gpu_available = False

TARGET_PARAMS = [
    pytest.param("ant", "cpu", id="ant-cpu"),
    pytest.param("phantom", "cuda",
                 marks=pytest.mark.skipif(not _gpu_available, reason="GPU not available"),
                 id="phantom-cuda"),
    pytest.param("acelib", "cuda",
                 marks=pytest.mark.skipif(not _gpu_available, reason="GPU not available"),
                 id="acelib-cuda"),
]