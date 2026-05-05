# tests/test_regression/conftest.py
"""
Fixtures for regression tests.

Provides:
- Fixed input generation (ones, neg_ones, arange)
- Random input generation for coverage tests
- Model/function test cases from test_cases
- ResNet-specific fixtures
- Backend configuration
"""
import pytest
from pathlib import Path

# Import centralized dependency checks
from test_utils import (
    TORCH_AVAILABLE,
    TORCH_FX_AVAILABLE,
    HAS_FRONTEND,
    ONNX_AVAILABLE,
    FHE_AVAILABLE,
    MODEL_TEST_CASES,
    FUNCTION_TEST_CASES,
    skip_if_no_torch,
    skip_if_no_torch_fx,
    skip_if_no_frontend,
    skip_if_no_onnx,
    skip_if_no_fhe,
)

# Import input utilities
from test_cases.input_utils import (
    InputMode,
    generate_inputs_by_mode,
    REGRESSION_MODES,
)

# Import ResNet model test cases
try:
    from test_regression.resnet.data import RESNET_MODEL_TEST_CASES
except ImportError:
    RESNET_MODEL_TEST_CASES = []


# ============================================================================
# Input Mode Fixtures
# ============================================================================

@pytest.fixture(params=REGRESSION_MODES, ids=lambda m: m.value)
def input_mode(request):
    """Parametrized fixture for input modes: ones, neg_ones, arange."""
    return request.param


# ============================================================================
# Model Test Case Fixtures
# ============================================================================

@pytest.fixture
def model_cases():
    """Return all model test cases."""
    if not TORCH_AVAILABLE:
        pytest.skip("torch not available")
    return MODEL_TEST_CASES


@pytest.fixture
def model_case_inputs(model_case, input_mode):
    """Generate fixed inputs for model_case."""
    return generate_inputs_by_mode(model_case.example_inputs, input_mode)


@pytest.fixture
def regression_inputs(model_case, input_mode):
    """Generate fixed inputs for model_case (alias for model_case_inputs)."""
    return generate_inputs_by_mode(model_case.example_inputs, input_mode)


# ============================================================================
# Function Test Case Fixtures
# ============================================================================

@pytest.fixture
def func_cases():
    """Return all function test cases."""
    if not TORCH_AVAILABLE:
        pytest.skip("torch not available")
    return FUNCTION_TEST_CASES


@pytest.fixture
def func_case_inputs(func_case, input_mode):
    """Generate fixed inputs for func_case."""
    return generate_inputs_by_mode(func_case.example_inputs, input_mode)


@pytest.fixture
def regression_func_inputs(func_case, input_mode):
    """Generate fixed inputs for func_case (alias for func_case_inputs)."""
    return generate_inputs_by_mode(func_case.example_inputs, input_mode)


# ============================================================================
# Coverage Test Fixtures (Random Inputs)
# ============================================================================

@pytest.fixture
def coverage_inputs(model_case):
    """
    Generate random inputs for model_case.
    Used for coverage tests to explore more input scenarios.
    """
    if not TORCH_AVAILABLE:
        pytest.skip("torch not available")
    return generate_inputs_by_mode(model_case.example_inputs, InputMode.RANDOM)


@pytest.fixture
def multiple_random_inputs(model_case, num_samples=3):
    """
    Generate multiple sets of random inputs for model_case.
    Used for more comprehensive coverage tests.
    """
    if not TORCH_AVAILABLE:
        pytest.skip("torch not available")

    inputs_list = []
    for i in range(num_samples):
        inputs = generate_inputs_by_mode(
            model_case.example_inputs,
            InputMode.RANDOM,
            seed=42 + i
        )
        inputs_list.append(inputs)
    return inputs_list


@pytest.fixture
def coverage_func_inputs(func_case):
    """
    Generate random inputs for func_case.
    Used for coverage tests to explore more input scenarios.
    """
    if not TORCH_AVAILABLE:
        pytest.skip("torch not available")
    return generate_inputs_by_mode(func_case.example_inputs, InputMode.RANDOM)


@pytest.fixture
def multiple_random_func_inputs(func_case, num_samples=3):
    """
    Generate multiple sets of random inputs for func_case.
    Used for more comprehensive coverage tests.
    """
    if not TORCH_AVAILABLE:
        pytest.skip("torch not available")

    inputs_list = []
    for i in range(num_samples):
        inputs = generate_inputs_by_mode(
            func_case.example_inputs,
            InputMode.RANDOM,
            seed=42 + i
        )
        inputs_list.append(inputs)
    return inputs_list


# ============================================================================
# ResNet Model Case Fixture
# ============================================================================

@pytest.fixture(params=RESNET_MODEL_TEST_CASES, ids=lambda tc: tc.name)
def resnet_case(request):
    """Parametrized fixture for ResNet model test cases."""
    return request.param


# ============================================================================
# ResNet-Specific Fixtures
# ============================================================================

@pytest.fixture(params=REGRESSION_MODES, ids=lambda m: m.value)
def resnet_regression_inputs(resnet_case, request):
    """Generate deterministic inputs for ResNet regression tests."""
    return generate_inputs_by_mode(resnet_case.example_inputs, request.param)


@pytest.fixture
def resnet_coverage_inputs(resnet_case):
    """Generate random inputs for ResNet coverage tests."""
    return generate_inputs_by_mode(resnet_case.example_inputs, InputMode.RANDOM)


@pytest.fixture
def resnet_coverage_inputs_multi(resnet_case):
    """Generate multiple random input sets for ResNet coverage testing."""
    inputs_list = []
    for seed in [42, 123, 456]:
        inputs = generate_inputs_by_mode(resnet_case.example_inputs, InputMode.RANDOM, seed=seed)
        inputs_list.append(inputs)
    return inputs_list


# ============================================================================
# Library Configuration Fixtures
# ============================================================================

@pytest.fixture
def library():
    """Default library name for regression tests."""
    return "antlib"


@pytest.fixture
def device():
    """Default device for regression tests."""
    return "cpu"


# ============================================================================
# Expected Output Fixtures
# ============================================================================

@pytest.fixture
def expected_model_output(model_case, model_case_inputs):
    """Compute expected output from original model."""
    import torch
    model = model_case.create_model()
    model.eval()
    with torch.no_grad():
        output = model(*model_case_inputs)
    return output


@pytest.fixture
def expected_func_output(func_case, func_case_inputs):
    """Compute expected output from original function."""
    return func_case.run(*func_case_inputs)


# ============================================================================
# Pytest Configuration
# ============================================================================

def pytest_configure(config):
    """Register custom markers."""
    config.addinivalue_line(
        "markers", "deterministic: mark test as using deterministic inputs"
    )
    config.addinivalue_line(
        "markers", "random: mark test as using random inputs"
    )
    config.addinivalue_line(
        "markers", "function: mark test as function-level test"
    )
    config.addinivalue_line(
        "markers", "model: mark test as model-level test"
    )
    config.addinivalue_line(
        "markers", "resnet: mark test as ResNet-specific test"
    )
    config.addinivalue_line(
        "markers", "regression: mark test as regression test"
    )
    config.addinivalue_line(
        "markers", "slow: Slow tests (use -m 'not slow' to skip)"
    )
    config.addinivalue_line(
        "markers", "requires_torch_fx: requires torch.fx and frontend"
    )
    config.addinivalue_line(
        "markers", "requires_onnx: requires onnx"
    )