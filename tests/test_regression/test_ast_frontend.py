# tests/test_regression/test_ast_frontend.py
"""
Regression tests for AST frontend path.

Tests FUNCTION_TEST_CASES through:
  Python function -> AST analysis -> compile -> execution

Uses @fhe.compile for compilation tests and @fhe.compute for end-to-end tests.

Note: AST frontend only supports functions, not model classes.
"""
import pytest
import torch
from pathlib import Path

from ace import fhe
from test_cases import FUNCTION_TEST_CASES
from test_cases.input_utils import generate_inputs_by_mode, InputMode


# ============================================================================
# Fixtures
# ============================================================================

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
# Function Tests with @fhe.compile
# ============================================================================

@pytest.mark.parametrize("backend,device", BACKEND_PARAMS)
class TestExportFunctions:
    """Test FUNCTION_TEST_CASES compilation with @fhe.compile."""

    def test_compile_function(self, func_case, func_inputs, backend, device):
        """Test function compilation through AST path."""
        func = func_case.func

        @fhe.compile(frontend="ast", library=backend, device=device)
        def compiled_func(*args):
            return func(*args)

        prog = compiled_func.compile(func_inputs)
        assert prog is not None

        runner = fhe.FHERuntime(prog)
        runner.inference(*func_inputs)
        runner.validate()


# ============================================================================
# Function Tests with @fhe.export (frontend IR export)
# ============================================================================

@pytest.mark.parametrize("backend,device", BACKEND_PARAMS)
class TestExportFunctions:
    """Test FUNCTION_TEST_CASES export with @fhe.export."""

    def test_export_function(self, func_case, func_inputs, backend, device, temp_root):
        """Test function frontend IR export."""
        func = func_case.func

        # Directly decorate the original function
        export_func = fhe.export(frontend="ast", library=backend, device=device)(func)

        # Export to .B file - use temp_root for unified temp directory management
        output_path = str(temp_root / f"func_{func_case.name}.B")
        result = export_func.export(func_inputs, output_path=output_path, format="air")

        assert result is not None
        assert isinstance(result, str)
        # Verify the file was created
        import os
        assert os.path.exists(result), f"Exported file not found: {result}"


# ============================================================================
# Function Tests with @fhe.compute
# ============================================================================

@pytest.mark.parametrize("backend,device", BACKEND_PARAMS)
class TestExportFunctions:
    """Test FUNCTION_TEST_CASES end-to-end with @fhe.compute."""

    def test_compute_function(self, func_case, func_inputs, backend, device):
        """Test function compilation and execution through AST path."""
        func = func_case.func

        @fhe.compute(frontend="ast", library=backend, device=device)
        def compute_func(*args):
            return func(*args)

        result = compute_func(*func_inputs)
        assert result is not None
        assert isinstance(result, torch.Tensor)