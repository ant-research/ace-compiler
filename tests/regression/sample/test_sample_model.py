# tests/regression/sample/test_sample_model.py
"""
Sample PyTorch validation tests.

Tests cover model/function creation, forward pass, and determinism.
No FHE compilation involved — pure PyTorch validation.

Run with: pytest -m model tests/regression/sample/
"""
import pytest
import torch

from ace.sample.ops.specs import ALL_OPS_SPECS
from ace.sample.funcs.specs import ALL_FUNCS_SPECS
from utils import requires_torch


@requires_torch
class TestOpModel:
    """Op model validation: creation, forward pass, determinism."""

    @pytest.mark.parametrize("spec", ALL_OPS_SPECS, ids=lambda s: s.name)
    def test_model_creation(self, spec):
        """Test that model can be instantiated."""
        model = spec.create_model()
        assert model is not None, f"Failed to create model {spec.name}"

    @pytest.mark.parametrize("spec", ALL_OPS_SPECS, ids=lambda s: s.name)
    def test_model_forward_pass(self, spec):
        """Test that forward pass produces valid output (no NaN/Inf)."""
        model = spec.create_model()
        model.eval()

        with torch.no_grad():
            output = model(*spec.example_inputs)

        assert output is not None, f"Model {spec.name} returned None"
        assert not torch.isnan(output).any(), f"Model {spec.name} output contains NaN"
        assert torch.isfinite(output).all(), f"Model {spec.name} output contains Inf"

    @pytest.mark.parametrize("spec", ALL_OPS_SPECS, ids=lambda s: s.name)
    def test_model_has_expected_ops(self, spec):
        """Test that spec declares expected operations."""
        assert len(spec.expected_ops) > 0, f"Spec {spec.name} has no expected_ops"

    @pytest.mark.parametrize("spec", ALL_OPS_SPECS, ids=lambda s: s.name)
    def test_model_deterministic_output(self, spec):
        """Test that same input produces same output."""
        model = spec.create_model()
        model.eval()

        with torch.no_grad():
            output1 = model(*spec.example_inputs)
            output2 = model(*spec.example_inputs)

        assert torch.allclose(output1, output2, atol=1e-6), \
            f"Model {spec.name} output is not deterministic"


@requires_torch
class TestFuncModel:
    """Function validation: execution, determinism, shape."""

    @pytest.mark.parametrize("spec", ALL_FUNCS_SPECS, ids=lambda s: s.name)
    def test_function_execution(self, spec):
        """Test that function execution completes normally."""
        output = spec.func(*spec.example_inputs)

        assert output is not None, f"Function {spec.name} returned None"
        assert isinstance(output, torch.Tensor), f"Function {spec.name} output is not a tensor"
        assert not torch.isnan(output).any(), f"Function {spec.name} output contains NaN"
        assert not torch.isinf(output).any(), f"Function {spec.name} output contains Inf"

    @pytest.mark.parametrize("spec", ALL_FUNCS_SPECS, ids=lambda s: s.name)
    def test_function_deterministic_output(self, spec):
        """Test that same input produces same output."""
        output1 = spec.func(*spec.example_inputs)
        output2 = spec.func(*spec.example_inputs)

        assert torch.allclose(output1, output2, atol=1e-6), \
            f"Function {spec.name} output is not deterministic"

    @pytest.mark.parametrize("spec", ALL_FUNCS_SPECS, ids=lambda s: s.name)
    def test_function_output_shape(self, spec):
        """Test that output has valid shape."""
        output = spec.func(*spec.example_inputs)

        assert output.shape is not None, f"Function {spec.name} output has no shape"
        assert output.numel() > 0, f"Function {spec.name} output is empty"

    @pytest.mark.parametrize("spec", ALL_FUNCS_SPECS, ids=lambda s: s.name)
    def test_function_has_expected_ops(self, spec):
        """Test that spec declares expected ops (if any)."""
        if spec.expected_ops:
            assert len(spec.expected_ops) > 0, \
                f"Spec {spec.name} has empty expected_ops"