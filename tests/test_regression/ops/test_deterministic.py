# tests/test_regression/test_models/test_deterministic.py
"""
Model regression tests using deterministic inputs.

These tests use deterministic inputs (ones, negative ones, arange) to ensure
reproducible results. They are suitable for:
- Golden output comparison
- CI/CD regression detection
- Numerical stability verification

Run with: pytest -m deterministic tests/test_regression/test_models/
"""
import pytest
from test_utils import TORCH_AVAILABLE, torch


@pytest.mark.deterministic
@pytest.mark.model
class TestModelRegression:
    """Model regression tests: verify reproducibility with fixed inputs."""

    def test_model_forward_pass(self, model_case, regression_inputs):
        """Test that model forward pass completes normally."""
        if not TORCH_AVAILABLE:
            pytest.skip("torch not available")

        model = model_case.create_model()
        model.eval()

        with torch.no_grad():
            output = model(*regression_inputs)

        # Verify output is not None
        assert output is not None, f"Model {model_case.name} returned None"

        # Verify output is a tensor or tuple of tensors
        if isinstance(output, (tuple, list)):
            for i, out in enumerate(output):
                assert isinstance(out, torch.Tensor), f"Output {i} is not a tensor"
                assert not torch.isnan(out).any(), f"Output {i} contains NaN"
                assert not torch.isinf(out).any(), f"Output {i} contains Inf"
        else:
            assert isinstance(output, torch.Tensor), "Output is not a tensor"
            assert not torch.isnan(output).any(), "Output contains NaN"
            assert not torch.isinf(output).any(), "Output contains Inf"

    def test_model_output_shape(self, model_case, regression_inputs):
        """Test model output shape consistency."""
        if not TORCH_AVAILABLE:
            pytest.skip("torch not available")

        model = model_case.create_model()
        model.eval()

        # Run twice with same inputs to verify output shape consistency
        with torch.no_grad():
            output1 = model(*regression_inputs)
            output2 = model(*regression_inputs)

        if isinstance(output1, (tuple, list)):
            assert len(output1) == len(output2), "Output tuple length mismatch"
            for i, (o1, o2) in enumerate(zip(output1, output2)):
                assert o1.shape == o2.shape, f"Output {i} shape mismatch"
        else:
            assert output1.shape == output2.shape, "Output shape mismatch"

    def test_model_deterministic(self, model_case, regression_inputs):
        """Test model output determinism (same input should produce same output)."""
        if not TORCH_AVAILABLE:
            pytest.skip("torch not available")

        model = model_case.create_model()
        model.eval()

        with torch.no_grad():
            output1 = model(*regression_inputs)
            output2 = model(*regression_inputs)

        if isinstance(output1, (tuple, list)):
            for i, (o1, o2) in enumerate(zip(output1, output2)):
                assert torch.allclose(o1, o2, atol=1e-6), \
                    f"Output {i} is not deterministic"
        else:
            assert torch.allclose(output1, output2, atol=1e-6), \
                "Model output is not deterministic"