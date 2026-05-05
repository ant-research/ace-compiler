# tests/test_regression/test_torch_via_onnx_frontend.py
"""
Regression tests for torch-via-onnx frontend path.

Tests MODEL_TEST_CASES and FUNCTION_TEST_CASES through:
  torch model/function -> ONNX export -> compile

Uses fhe.compile and fhe.compute interfaces (not decorator syntax).
"""
import pytest
import torch

from ace import fhe
from test_cases import MODEL_TEST_CASES, FUNCTION_TEST_CASES
from test_cases.input_utils import generate_inputs_by_mode, InputMode


# ============================================================================
# Fixtures
# ============================================================================

@pytest.fixture(params=MODEL_TEST_CASES, ids=lambda tc: tc.name)
def model_case(request):
    """Parametrized fixture for model test cases."""
    return request.param


@pytest.fixture(params=FUNCTION_TEST_CASES, ids=lambda tc: tc.name)
def func_case(request):
    """Parametrized fixture for function test cases."""
    return request.param


@pytest.fixture(params=[InputMode.ONES, InputMode.NEG_ONES, InputMode.ARANGE],
                ids=lambda m: m.value)
def input_mode(request):
    """Parametrized fixture for input modes."""
    return request.param


@pytest.fixture
def model_inputs(model_case, input_mode):
    """Generate inputs for model test case."""
    return generate_inputs_by_mode(model_case.example_inputs, input_mode)


@pytest.fixture
def func_inputs(func_case, input_mode):
    """Generate inputs for function test case."""
    return generate_inputs_by_mode(func_case.example_inputs, input_mode)


# Backend parameters
BACKEND_PARAMS = [
    pytest.param("antlib", "cpu", id="antlib-cpu"),
    pytest.param("phantom", "cuda",
                 marks=pytest.mark.skipif(not fhe.gpu_available(), reason="GPU not available"),
                 id="phantom-cuda"),
    pytest.param("hyperfhe", "cuda",
                 marks=pytest.mark.skipif(not fhe.gpu_available(), reason="GPU not available"),
                 id="hyperfhe-cuda"),
]


# ============================================================================
# Model Tests with fhe.compile
# ============================================================================

@pytest.mark.parametrize("backend,device", BACKEND_PARAMS)
class TestCompileModels:
    """Test MODEL_TEST_CASES compilation with fhe.compile."""

    def test_compile_success(self, model_case, model_inputs, backend, device):
        """Test model compilation through torch-via-onnx path."""
        model = model_case.create_model()
        encrypt_inputs = model_case.encrypt_inputs

        # Use fhe.compile interface (not decorator)
        compiled_model = fhe.compile(
            frontend="torch-via-onnx",
            library=backend,
            device=device,
            encrypt_inputs=encrypt_inputs
        )(model)

        prog = compiled_model.compile(model_inputs)

        assert prog is not None
        assert "kernel" in prog
        assert "model" in prog


# ============================================================================
# Function Tests with fhe.compile (decorator syntax)
# ============================================================================

@pytest.mark.parametrize("backend,device", BACKEND_PARAMS)
class TestCompileFunctions:
    """Test FUNCTION_TEST_CASES compilation with @fhe.compile decorator."""

    def test_compile_success(self, func_case, func_inputs, backend, device):
        """Test function compilation through torch-via-onnx path."""
        func = func_case.func

        @fhe.compile(frontend="torch-via-onnx", library=backend, device=device)
        def compiled_func(*args):
            return func(*args)

        prog = compiled_func.compile(func_inputs)

        assert prog is not None
        assert "kernel" in prog
        assert "model" in prog


# ============================================================================
# Model Tests with fhe.compute
# ============================================================================

@pytest.mark.parametrize("backend,device", BACKEND_PARAMS)
class TestComputeModels:
    """Test MODEL_TEST_CASES with fhe.compute."""

    def test_compute_success(self, model_case, model_inputs, backend, device):
        """Test model compilation and execution through torch-via-onnx path."""
        model = model_case.create_model()
        encrypt_inputs = model_case.encrypt_inputs

        # Use fhe.compute interface (not decorator)
        compute_model = fhe.compute(
            frontend="torch-via-onnx",
            library=backend,
            device=device,
            encrypt_inputs=encrypt_inputs
        )(model)

        result = compute_model(*model_inputs)

        assert result is not None
        assert isinstance(result, torch.Tensor)


# ============================================================================
# Function Tests with fhe.compute (decorator syntax)
# ============================================================================

@pytest.mark.parametrize("backend,device", BACKEND_PARAMS)
class TestComputeFunctions:
    """Test FUNCTION_TEST_CASES with @fhe.compute decorator."""

    def test_compute_success(self, func_case, func_inputs, backend, device):
        """Test function compilation and execution through torch-via-onnx path."""
        func = func_case.func

        @fhe.compute(frontend="torch-via-onnx", library=backend, device=device)
        def compute_func(*args):
            return func(*args)

        result = compute_func(*func_inputs)

        assert result is not None
        assert isinstance(result, torch.Tensor)