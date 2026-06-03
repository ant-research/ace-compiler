# tests/regression/resnet/test_resnet_model.py
"""
ResNet PyTorch model tests.

Tests cover model creation, forward pass, and determinism.
No FHE compilation involved — pure PyTorch model validation.
"""
import pytest
import torch

from utils import requires_torch


@requires_torch
class TestResNetModel:
    """ResNet model creation, forward pass, and determinism tests."""

    def test_model_creation(self, resnet_case):
        """Test that ResNet model can be created."""
        model = resnet_case.create_model()
        assert model is not None

    def test_model_forward_pass(self, resnet_case):
        """Test that ResNet forward pass produces valid output."""
        model = resnet_case.create_model()
        model.eval()

        with torch.no_grad():
            output = model(*resnet_case.example_inputs)

        assert output is not None
        assert not torch.isnan(output).any()
        assert torch.isfinite(output).all()

    def test_model_has_expected_ops(self, resnet_case):
        """Test that model spec declares expected operations."""
        assert len(resnet_case.expected_ops) > 0

    def test_deterministic_output(self, resnet_case):
        """Test that same input produces same output."""
        model = resnet_case.create_model()
        model.eval()

        x = resnet_case.example_inputs[0]
        with torch.no_grad():
            output1 = model(x)
            output2 = model(x)

        assert torch.allclose(output1, output2)

    def test_batch_size_consistency(self, resnet_case):
        """Test that model handles different batch sizes."""
        model = resnet_case.create_model()
        model.eval()

        original_input = resnet_case.example_inputs[0]
        num_classes = output_shape = None

        for batch_size in [1, 2, 4]:
            new_input = torch.randn(batch_size, *original_input.shape[1:])
            with torch.no_grad():
                output = model(new_input)

            assert output.shape[0] == batch_size
            # Infer num_classes from first run
            if num_classes is None:
                num_classes = output.shape[1]
            else:
                assert output.shape[1] == num_classes