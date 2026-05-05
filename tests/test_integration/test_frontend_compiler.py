# tests/test_integration/test_frontend_compiler/test_frontend_to_ir.py
"""
Integration tests for frontend + compiler pipeline.
Tests that frontends correctly produce IR that can be consumed by the compiler.
"""
import pytest
import torch
import torch.nn as nn

from ace.fhe import Driver


class TestFrontendCompilerIntegration:
    """Integration tests for frontend to compiler flow."""

    def test_torch_via_onnx_to_compiler(self):
        """Test torch-via-onnx frontend produces valid IR for compiler."""
        class SimpleModel(nn.Module):
            def forward(self, x, y):
                return x + y

        model = SimpleModel()
        inputs = [torch.randn(1, 1, 2, 2), torch.randn(1, 1, 2, 2)]

        compiler = Driver(
            frontend="torch-via-onnx",
            library="antlib",
            device="cpu"
        )

        # This should produce valid IR and compile to a package
        package = compiler.compile(model, inputs, input_names=["x", "y"])

        assert package is not None
        assert "model" in package
        assert "kernel" in package
        assert "input_info" in package


class TestMultipleInputShapes:
    """Test compiler handles different input shapes."""

    def test_different_input_shapes(self):
        """Test compilation with different input shapes."""
        class SimpleModel(nn.Module):
            def forward(self, x):
                return x + 1

        model = SimpleModel()

        compiler = Driver(
            frontend="torch-via-onnx",
            library="antlib",
            device="cpu"
        )

        # Compile with one shape
        inputs1 = [torch.randn(1, 1, 2, 2)]
        package1 = compiler.compile(model, inputs1, input_names=["x"])

        assert package1 is not None

        # Compile with different shape
        inputs2 = [torch.randn(1, 1, 4, 4)]
        package2 = compiler.compile(model, inputs2, input_names=["x"])

        assert package2 is not None