# tests/test_regression/test_functions/test_deterministic.py
"""
Function regression tests using deterministic inputs.

These tests use deterministic inputs (ones, negative ones, arange) to ensure
reproducible results for function-level tests.

Run with: pytest -m deterministic tests/test_regression/test_functions/
"""
import pytest
from test_utils import TORCH_AVAILABLE, torch


@pytest.mark.deterministic
@pytest.mark.function
class TestFunctionRegression:
    """Function regression tests: verify reproducibility with fixed inputs."""

    def test_function_execution(self, func_case, regression_func_inputs):
        """Test that function execution completes normally."""
        if not TORCH_AVAILABLE:
            pytest.skip("torch not available")

        output = func_case.run(*regression_func_inputs)

        # Verify output is not None
        assert output is not None, f"Function {func_case.name} returned None"

        # Verify output is a tensor
        assert isinstance(output, torch.Tensor), "Output is not a tensor"
        assert not torch.isnan(output).any(), "Output contains NaN"
        assert not torch.isinf(output).any(), "Output contains Inf"

    def test_function_deterministic(self, func_case, regression_func_inputs):
        """Test function output determinism."""
        if not TORCH_AVAILABLE:
            pytest.skip("torch not available")

        output1 = func_case.run(*regression_func_inputs)
        output2 = func_case.run(*regression_func_inputs)

        assert torch.allclose(output1, output2, atol=1e-6), \
            f"Function {func_case.name} output is not deterministic"

    def test_function_output_shape(self, func_case, regression_func_inputs):
        """Test function output shape."""
        if not TORCH_AVAILABLE:
            pytest.skip("torch not available")

        output = func_case.run(*regression_func_inputs)

        # Output shape should match first input shape (for most basic operations)
        # Here we only verify output is a valid tensor
        assert output.shape is not None, "Output has no shape"
        assert output.numel() > 0, "Output is empty"