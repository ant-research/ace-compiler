# tests/test_regression/test_onnx_frontend.py
"""
Regression tests for ONNX frontend path.

Tests MODEL_TEST_CASES through:
  torch model -> ONNX export -> Driver -> execution

Note: ONNX frontend takes an ONNX file path directly, not a decorated model.
We use Driver directly instead of @fhe.compile decorator.
"""
import pytest
import torch
import torch.onnx
from pathlib import Path

from ace import fhe
from ace.fhe import Driver, FHERuntime
from test_cases import MODEL_TEST_CASES
from test_cases.input_utils import generate_inputs_by_mode, InputMode
from test_utils import ONNX_AVAILABLE


# Skip if onnx not available
requires_onnx = pytest.mark.skipif(
    not ONNX_AVAILABLE,
    reason="onnx not available"
)


# ============================================================================
# Fixtures
# ============================================================================

@pytest.fixture(params=MODEL_TEST_CASES, ids=lambda tc: tc.name)
def model_case(request):
    """Parametrized fixture for model test cases."""
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
def onnx_file(model_case, model_inputs, temp_root):
    """Export model to ONNX file."""
    model = model_case.create_model()
    model.eval()

    onnx_path = temp_root / f"{model_case.name}.onnx"

    torch.onnx.export(
        model,
        tuple(model_inputs),
        str(onnx_path),
        input_names=model_case.encrypt_inputs,
        output_names=["output"],
        opset_version=14,
    )

    return str(onnx_path)


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
# Model Tests with Driver (ONNX file path)
# ============================================================================

@requires_onnx
@pytest.mark.parametrize("backend,device", BACKEND_PARAMS)
class TestCompileModels:
    """Test MODEL_TEST_CASES compilation via ONNX file path."""

    def test_compile_model(self, model_case, onnx_file, model_inputs, backend, device):
        """Test model compilation through ONNX path using Driver."""
        encrypt_inputs = model_case.encrypt_inputs

        # Use Driver directly with ONNX file path
        compiler = Driver(
            frontend="onnx",
            library=backend,
            device=device
        )

        # For ONNX frontend, pass onnx file path as source
        prog = compiler.compile(onnx_file, model_inputs, input_names=encrypt_inputs)
        assert prog is not None
        assert "kernel" in prog
        assert "model" in prog
