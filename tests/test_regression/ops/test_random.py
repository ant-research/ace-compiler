# tests/test_regression/test_models/test_random.py
"""
Model coverage tests using random inputs.

These tests use random inputs to explore more input scenarios and improve
test coverage. They help discover:
- Edge cases with unusual input values
- Numerical stability issues
- Shape-related bugs

Run with: pytest -m random tests/test_regression/test_models/
"""
import pytest
from test_utils import TORCH_AVAILABLE, torch


@pytest.mark.random
@pytest.mark.model
class TestModelCoverage:
    """Model coverage tests: explore more scenarios with random inputs."""

    def test_model_with_random_input(self, model_case, coverage_inputs):
        """Test model handling of random inputs."""
        if not TORCH_AVAILABLE:
            pytest.skip("torch not available")

        model = model_case.create_model()
        model.eval()

        with torch.no_grad():
            output = model(*coverage_inputs)

        # Verify output is valid
        assert output is not None, f"Model {model_case.name} returned None"

        if isinstance(output, (tuple, list)):
            for i, out in enumerate(output):
                assert isinstance(out, torch.Tensor), f"Output {i} is not a tensor"
                # Allow NaN and Inf but log warnings
                if torch.isnan(out).any():
                    pytest.warning(f"Model {model_case.name} output {i} contains NaN")
                if torch.isinf(out).any():
                    pytest.warning(f"Model {model_case.name} output {i} contains Inf")
        else:
            assert isinstance(output, torch.Tensor), "Output is not a tensor"

    def test_model_multiple_random_inputs(self, model_case, multiple_random_inputs):
        """Test model handling of multiple random inputs."""
        if not TORCH_AVAILABLE:
            pytest.skip("torch not available")

        model = model_case.create_model()
        model.eval()

        outputs = []
        with torch.no_grad():
            for inputs in multiple_random_inputs:
                output = model(*inputs)
                outputs.append(output)

        # Verify all outputs are valid
        for i, output in enumerate(outputs):
            assert output is not None, f"Model {model_case.name} returned None for input {i}"

    def test_model_output_consistency(self, model_case, coverage_inputs):
        """Test model consistency with same random inputs."""
        if not TORCH_AVAILABLE:
            pytest.skip("torch not available")

        model = model_case.create_model()
        model.eval()

        # Run twice with same random inputs
        # Note: coverage_inputs has fixed seed, so it's reproducible
        with torch.no_grad():
            output1 = model(*coverage_inputs)
            output2 = model(*coverage_inputs)

        if isinstance(output1, (tuple, list)):
            for i, (o1, o2) in enumerate(zip(output1, output2)):
                assert torch.allclose(o1, o2, atol=1e-6), \
                    f"Model {model_case.name} output {i} is inconsistent"
        else:
            assert torch.allclose(output1, output2, atol=1e-6), \
                f"Model {model_case.name} output is inconsistent"


@pytest.mark.random
@pytest.mark.model
class TestModelEdgeCases:
    """Model edge case tests."""

    def test_model_with_small_values(self, model_case):
        """Test model handling of small values."""
        if not TORCH_AVAILABLE:
            pytest.skip("torch not available")

        model = model_case.create_model()
        model.eval()

        # Generate small value inputs
        small_inputs = []
        for inp in model_case.example_inputs:
            small = torch.randn(inp.shape, dtype=inp.dtype) * 1e-6
            small_inputs.append(small)

        with torch.no_grad():
            output = model(*small_inputs)

        assert output is not None, f"Model {model_case.name} failed with small values"

    def test_model_with_large_values(self, model_case):
        """Test model handling of large values."""
        if not TORCH_AVAILABLE:
            pytest.skip("torch not available")

        model = model_case.create_model()
        model.eval()

        # Generate large value inputs
        large_inputs = []
        for inp in model_case.example_inputs:
            large = torch.randn(inp.shape, dtype=inp.dtype) * 1e3
            large_inputs.append(large)

        with torch.no_grad():
            output = model(*large_inputs)

        assert output is not None, f"Model {model_case.name} failed with large values"

    def test_model_with_negative_values(self, model_case):
        """Test model handling of negative values."""
        if not TORCH_AVAILABLE:
            pytest.skip("torch not available")

        model = model_case.create_model()
        model.eval()

        # Generate negative value inputs
        neg_inputs = []
        for inp in model_case.example_inputs:
            neg = -torch.abs(torch.randn(inp.shape, dtype=inp.dtype))
            neg_inputs.append(neg)

        with torch.no_grad():
            output = model(*neg_inputs)

        assert output is not None, f"Model {model_case.name} failed with negative values"