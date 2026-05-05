# test_regression/resnet/test_model.py
"""
ResNet PyTorch model tests.

Tests cover model creation, forward pass, input robustness, and determinism.
No FHE compilation involved — pure PyTorch model validation.
"""
import pytest
import torch


@pytest.mark.resnet
@pytest.mark.model
class TestResNetModelCreation:
    """Tests for ResNet model creation and initialization."""

    def test_model_creation(self, resnet_case):
        """Test that ResNet model can be created."""
        model = resnet_case.create_model()
        assert model is not None

    def test_model_forward_pass(self, resnet_case):
        """Test that ResNet model can perform forward pass."""
        model = resnet_case.create_model()
        model.eval()

        with torch.no_grad():
            output = model(*resnet_case.example_inputs)

        assert output is not None
        assert output.shape[1] == 10

    def test_model_has_expected_ops(self, resnet_case):
        """Test that model has expected operations."""
        assert len(resnet_case.expected_ops) > 0


# ============================================================================
# Regression Tests (Deterministic Inputs)
# ============================================================================

@pytest.mark.resnet
@pytest.mark.deterministic
@pytest.mark.model
class TestResNetRegression:
    """Regression tests with deterministic inputs."""

    def test_forward_with_ones(self, resnet_case):
        """Test forward pass with all-ones input."""
        from test_cases.input_utils import InputMode, generate_inputs_by_mode

        model = resnet_case.create_model()
        model.eval()

        inputs = generate_inputs_by_mode(resnet_case.example_inputs, InputMode.ONES)
        with torch.no_grad():
            output = model(*inputs)

        assert output is not None
        assert not torch.isnan(output).any()

    def test_forward_with_neg_ones(self, resnet_case):
        """Test forward pass with all-negative-ones input."""
        from test_cases.input_utils import InputMode, generate_inputs_by_mode

        model = resnet_case.create_model()
        model.eval()

        inputs = generate_inputs_by_mode(resnet_case.example_inputs, InputMode.NEG_ONES)
        with torch.no_grad():
            output = model(*inputs)

        assert output is not None
        assert not torch.isnan(output).any()

    def test_forward_with_arange(self, resnet_case):
        """Test forward pass with incremental values."""
        from test_cases.input_utils import InputMode, generate_inputs_by_mode

        model = resnet_case.create_model()
        model.eval()

        inputs = generate_inputs_by_mode(resnet_case.example_inputs, InputMode.ARANGE)
        with torch.no_grad():
            output = model(*inputs)

        assert output is not None
        assert not torch.isnan(output).any()

    def test_deterministic_output(self, resnet_regression_inputs, resnet_case):
        """Test that same input produces same output."""
        model = resnet_case.create_model()
        model.eval()

        with torch.no_grad():
            output1 = model(*resnet_regression_inputs)
            output2 = model(*resnet_regression_inputs)

        assert torch.allclose(output1, output2)


# ============================================================================
# Coverage Tests (Random Inputs)
# ============================================================================

@pytest.mark.resnet
@pytest.mark.random
@pytest.mark.model
class TestResNetCoverage:
    """Coverage tests with random inputs."""

    def test_forward_with_random(self, resnet_case):
        """Test forward pass with random input."""
        from test_cases.input_utils import InputMode, generate_inputs_by_mode

        model = resnet_case.create_model()
        model.eval()

        inputs = generate_inputs_by_mode(resnet_case.example_inputs, InputMode.RANDOM)
        with torch.no_grad():
            output = model(*inputs)

        assert output is not None
        assert not torch.isnan(output).any()

    def test_multiple_random_inputs(self, resnet_coverage_inputs_multi, resnet_case):
        """Test forward pass with multiple random inputs."""
        model = resnet_case.create_model()
        model.eval()

        for inputs in resnet_coverage_inputs_multi:
            with torch.no_grad():
                output = model(*inputs)

            assert output is not None
            assert not torch.isnan(output).any()

    def test_output_range(self, resnet_coverage_inputs, resnet_case):
        """Test that output values are in reasonable range."""
        model = resnet_case.create_model()
        model.eval()

        with torch.no_grad():
            output = model(*resnet_coverage_inputs)

        assert torch.isfinite(output).all()
        assert output.shape[1] == 10


# ============================================================================
# Edge Case Tests
# ============================================================================

@pytest.mark.resnet
@pytest.mark.model
class TestResNetEdgeCases:
    """Edge case tests for ResNet models."""

    def test_batch_size_consistency(self, resnet_case):
        """Test that model handles different batch sizes."""
        model = resnet_case.create_model()
        model.eval()

        for batch_size in [1, 2, 4]:
            original_input = resnet_case.example_inputs[0]
            new_input = torch.randn(batch_size, *original_input.shape[1:])

            with torch.no_grad():
                output = model(new_input)

            assert output.shape[0] == batch_size
            assert output.shape[1] == 10