# tests/test_integration/test_frontend_runtime/test_full_pipeline.py
"""
Integration tests for full pipeline (frontend + compiler + runtime).
Tests the complete flow from model definition to encrypted execution.
"""
import pytest
import torch
import torch.nn as nn

from ace.fhe import Driver, FHERuntime


class TestFullPipeline:
    """Integration tests for the full pipeline."""

    def test_simple_add_pipeline(self):
        """Test full pipeline with simple addition."""
        class AddModel(nn.Module):
            def forward(self, x, y):
                return x + y

        model = AddModel()
        input0 = torch.ones(1, 1, 2, 2)
        input1 = torch.ones(1, 1, 2, 2)
        inputs = [input0, input1]

        # Frontend + Compiler
        compiler = Driver(
            frontend="torch-via-onnx",
            library="antlib",
            device="cpu"
        )
        package = compiler.compile(model, inputs, input_names=["x", "y"])

        # Runtime
        runner = FHERuntime(package, verify="array")
        result = runner.inference(*inputs)
        runner.validate()

        # Verify
        # expected = torch.full((1, 1, 2, 2), 2.0)
        # assert torch.allclose(result, expected)

    def test_matrix_multiplication_pipeline(self):
        """Test full pipeline with matrix multiplication."""
        class GemmModel(nn.Module):
            def __init__(self):
                super().__init__()
                self.linear = nn.Linear(4, 2)

            def forward(self, x):
                return self.linear(x)

        model = GemmModel()
        inputs = [torch.randn(1, 4)]

        compiler = Driver(
            frontend="torch-via-onnx",
            library="antlib",
            device="cpu"
        )
        package = compiler.compile(model, inputs, input_names=["x"])

        runner = FHERuntime(package, verify="array")
        result = runner.inference(*inputs)
        runner.validate()

        # Verify output shape
        # assert result.shape == (1, 2)

    def test_convolution_pipeline(self):
        """Test full pipeline with convolution."""
        class ConvModel(nn.Module):
            def __init__(self):
                super().__init__()
                self.conv = nn.Conv2d(1, 1, kernel_size=3, padding=1)

            def forward(self, x):
                return self.conv(x)

        model = ConvModel()
        inputs = [torch.randn(1, 1, 4, 4)]

        compiler = Driver(
            frontend="torch-via-onnx",
            library="antlib",
            device="cpu"
        )
        package = compiler.compile(model, inputs, input_names=["x"])

        runner = FHERuntime(package, verify="array")
        result = runner.inference(*inputs)
        runner.validate()

        # Verify output shape
        # assert result.shape == (1, 1, 4, 4)


class TestMultipleOperations:
    """Test pipeline with multiple operations."""

    def test_relu_activation(self):
        """Test pipeline with ReLU activation."""
        class ReluModel(nn.Module):
            def __init__(self):
                super().__init__()
                self.linear = nn.Linear(4, 4)
                self.relu = nn.ReLU()

            def forward(self, x):
                return self.relu(self.linear(x))

        model = ReluModel()
        inputs = [torch.randn(1, 4)]

        compiler = Driver(
            frontend="torch-via-onnx",
            library="antlib",
            device="cpu"
        )
        package = compiler.compile(model, inputs, input_names=["x"])

        runner = FHERuntime(package, verify="array")
        result = runner.inference(*inputs)
        runner.validate()

        # ReLU should produce non-negative values
        # assert (result >= 0).all()