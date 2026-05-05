# tests/test_regression/test_functions/test_random.py
"""
Function coverage tests using random inputs.

These tests use random inputs to explore more input scenarios and improve
test coverage for function-level tests.

Run with: pytest -m random tests/test_regression/test_functions/
"""
import pytest
from test_utils import TORCH_AVAILABLE, torch


@pytest.mark.random
@pytest.mark.function
class TestFunctionCoverage:
    """Function coverage tests: explore more scenarios with random inputs."""

    def test_function_with_random_input(self, func_case, coverage_func_inputs):
        """Test function handling of random inputs."""
        if not TORCH_AVAILABLE:
            pytest.skip("torch not available")

        output = func_case.run(*coverage_func_inputs)

        # Verify output is valid
        assert output is not None, f"Function {func_case.name} returned None"
        assert isinstance(output, torch.Tensor), "Output is not a tensor"

    def test_function_multiple_random_inputs(self, func_case, multiple_random_func_inputs):
        """Test function handling of multiple random inputs."""
        if not TORCH_AVAILABLE:
            pytest.skip("torch not available")

        outputs = []
        for inputs in multiple_random_func_inputs:
            output = func_case.run(*inputs)
            outputs.append(output)

        # Verify all outputs are valid
        for i, output in enumerate(outputs):
            assert output is not None, f"Function {func_case.name} returned None for input {i}"

    def test_function_output_consistency(self, func_case, coverage_func_inputs):
        """Test function consistency with same random inputs."""
        if not TORCH_AVAILABLE:
            pytest.skip("torch not available")

        output1 = func_case.run(*coverage_func_inputs)
        output2 = func_case.run(*coverage_func_inputs)

        assert torch.allclose(output1, output2, atol=1e-6), \
            f"Function {func_case.name} output is inconsistent"


@pytest.mark.random
@pytest.mark.function
class TestFunctionEdgeCases:
    """Function edge case tests."""

    def test_function_with_small_values(self, func_case):
        """Test function handling of small values."""
        if not TORCH_AVAILABLE:
            pytest.skip("torch not available")

        small_inputs = []
        for inp in func_case.example_inputs:
            small = torch.randn(inp.shape, dtype=inp.dtype) * 1e-6
            small_inputs.append(small)

        output = func_case.run(*small_inputs)
        assert output is not None, f"Function {func_case.name} failed with small values"

    def test_function_with_large_values(self, func_case):
        """Test function handling of large values."""
        if not TORCH_AVAILABLE:
            pytest.skip("torch not available")

        large_inputs = []
        for inp in func_case.example_inputs:
            large = torch.randn(inp.shape, dtype=inp.dtype) * 1e3
            large_inputs.append(large)

        output = func_case.run(*large_inputs)
        assert output is not None, f"Function {func_case.name} failed with large values"

    def test_function_with_negative_values(self, func_case):
        """Test function handling of negative values."""
        if not TORCH_AVAILABLE:
            pytest.skip("torch not available")

        neg_inputs = []
        for inp in func_case.example_inputs:
            neg = -torch.abs(torch.randn(inp.shape, dtype=inp.dtype))
            neg_inputs.append(neg)

        output = func_case.run(*neg_inputs)
        assert output is not None, f"Function {func_case.name} failed with negative values"

    def test_function_with_mixed_values(self, func_case):
        """Test function handling of mixed positive/negative values."""
        if not TORCH_AVAILABLE:
            pytest.skip("torch not available")

        mixed_inputs = []
        for inp in func_case.example_inputs:
            mixed = torch.randn(inp.shape, dtype=inp.dtype)
            mixed_inputs.append(mixed)

        output = func_case.run(*mixed_inputs)
        assert output is not None, f"Function {func_case.name} failed with mixed values"