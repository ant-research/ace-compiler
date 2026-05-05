# tests/test_regression/test_torch_frontend.py
"""
Regression tests for torch frontend path.

Tests MODEL_TEST_CASES and FUNCTION_TEST_CASES through:
  torch model/function -> torch.fx capture -> compile -> execution

Uses fhe.compile and fhe.compute interfaces.
"""
import os
import pytest
import torch
from pathlib import Path

from ace import fhe
from test_cases import MODEL_TEST_CASES, FUNCTION_TEST_CASES
from test_cases.input_utils import generate_inputs_by_mode, InputMode
from test_cases import get_compile_options
from test_utils import TORCH_FX_AVAILABLE


# Skip if torch.fx not available
requires_torch_fx = pytest.mark.skipif(
    not TORCH_FX_AVAILABLE,
    reason="torch.fx not available"
)


# Note: fhe_build_dir fixture is defined in tests/conftest.py
# It creates a unique directory under temp_root for each test case:
#   temp_root/test_subdir/test_case_name/
# And sets ACE_FHE_BUILD_DIR environment variable.

# temp_root is imported from tests/conftest.py


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

@requires_torch_fx
@pytest.mark.parametrize("backend,device", BACKEND_PARAMS)
class TestCompileModels:
    """Test MODEL_TEST_CASES compilation with fhe.compile."""

    def test_compile_success(self, model_case, model_inputs, backend, device):
        """Test model compilation through torch path."""
        model = model_case.create_model()
        encrypt_inputs = model_case.encrypt_inputs

        # Get compile options with priority: env > case > model_specific > default
        compile_opts = get_compile_options(model_case.name, model_case.compile_options)

        # Use fhe.compile interface (not decorator)
        compiled_model = fhe.compile(
            frontend="torch",
            library=backend,
            device=device,
            encrypt_inputs=encrypt_inputs,
            **compile_opts
        )(model)

        # Use fhe_compile - works for both model instances and classes
        prog = compiled_model.fhe_compile(model_inputs)

        # prog is a CompiledProgram object, access package via .package
        assert prog is not None
        assert "kernel" in prog.package
        assert "model" in prog.package

        print(f"AIR to Kernel: {prog.package['kernel']}") 


# ============================================================================
# Function Tests with fhe.compile (decorator syntax)
# ============================================================================

@requires_torch_fx
@pytest.mark.parametrize("backend,device", BACKEND_PARAMS)
class TestCompileFunctions:
    """Test FUNCTION_TEST_CASES compilation with @fhe.compile decorator."""

    def test_compile_success(self, func_case, func_inputs, backend, device):
        """Test function compilation through torch path."""
        func = func_case.func

        @fhe.compile(frontend="torch", library=backend, device=device)
        def compiled_func(*args):
            return func(*args)

        prog = compiled_func.compile(func_inputs)

        assert prog is not None
        assert "kernel" in prog
        assert "model" in prog


# ============================================================================
# Model Tests with fhe.compute
# ============================================================================

@requires_torch_fx
@pytest.mark.parametrize("backend,device", BACKEND_PARAMS)
class TestComputeModels:
    """Test MODEL_TEST_CASES with fhe.compute."""

    def test_compute_success(self, model_case, model_inputs, backend, device):
        """Test model compilation and execution through torch path."""
        model = model_case.create_model()
        encrypt_inputs = model_case.encrypt_inputs

        # Get compile options with priority: env > case > model_specific > default
        compile_opts = get_compile_options(model_case.name, model_case.compile_options)

        # Use fhe.compute interface (not decorator)
        compute_model = fhe.compute(
            frontend="torch",
            library=backend,
            device=device,
            encrypt_inputs=encrypt_inputs,
            **compile_opts
        )(model)

        result = compute_model(*model_inputs)

        assert result is not None
        assert isinstance(result, torch.Tensor)


# ============================================================================
# Model Tests with fhe.export (frontend IR export)
# ============================================================================

@pytest.mark.parametrize("backend,device", BACKEND_PARAMS)
class TestExportModels:
    """Test MODEL_TEST_CASES with fhe.export to verify frontend IR generation."""

    def test_export_success(self, model_case, model_inputs, backend, device, tmp_path):
        """Test model frontend IR export."""
        model = model_case.create_model()
        encrypt_inputs = model_case.encrypt_inputs

        # Use fhe.export interface to test frontend output
        export_model = fhe.export(
            frontend="torch",
            library=backend,
            device=device,
            encrypt_inputs=encrypt_inputs
        )(model)

        # Export to .B file in build directory
        output_path = str(tmp_path / "model.B")
        result = export_model.export(model_inputs, output_path=output_path, format="air")

        assert result is not None
        assert isinstance(result, str)
        # Verify the file was created
        import os
        assert os.path.exists(result), f"Exported file not found: {result}"


# ============================================================================
# Function Tests with fhe.compute (decorator syntax)
# ============================================================================

@requires_torch_fx
@pytest.mark.parametrize("backend,device", BACKEND_PARAMS)
class TestComputeFunctions:
    """Test FUNCTION_TEST_CASES with @fhe.compute decorator."""

    def test_compute_success(self, func_case, func_inputs, backend, device):
        """Test function compilation and execution through torch path."""
        func = func_case.func

        @fhe.compute(frontend="torch", library=backend, device=device)
        def compute_func(*args):
            return func(*args)

        result = compute_func(*func_inputs)

        assert result is not None
        assert isinstance(result, torch.Tensor)