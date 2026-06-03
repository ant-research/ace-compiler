# tests/regression/conftest.py
"""
Fixtures for regression tests.

Provides:
- ResNet-specific fixtures
- Custom pytest markers
- Test ordering: CPU smoke tests run before CUDA smoke tests to avoid
  CUDA runtime state corrupting the CPU FHE runtime (segfault).
"""
import pytest

# Import ResNet model test cases
try:
    from ace.model.spec_resnet import ALL_RESNET_SPECS as RESNET_MODEL_TEST_CASES
except ImportError:
    RESNET_MODEL_TEST_CASES = []


# ============================================================================
# ResNet Model Case Fixture
# ============================================================================

@pytest.fixture(params=RESNET_MODEL_TEST_CASES, ids=lambda tc: tc.name)
def resnet_case(request):
    """Parametrized fixture for ResNet model test cases."""
    return request.param


# ============================================================================
# Pytest Configuration
# ============================================================================

def pytest_configure(config):
    """Register custom markers (supplements pytest.ini declarations)."""
    pass


def pytest_collection_modifyitems(session, config, items):
    """Reorder smoke tests so CPU (antlib) tests run before CUDA tests.

    Running CUDA smoke tests (acelib, phantom) before CPU tests (antlib) can
    leave the CUDA runtime in a state that causes the CPU FHE runtime to
    segfault.  Moving antlib smoke tests first avoids this.
    """
    antlib_items = []
    other_items = []
    for item in items:
        if "test_sample_smoke_antlib" in item.nodeid:
            antlib_items.append(item)
        else:
            other_items.append(item)
    items[:] = antlib_items + other_items