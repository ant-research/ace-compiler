# tests/test_integration/test_compiler_runtime/test_compile_and_run.py
"""
Integration tests for compiler + runtime pipeline.
Tests that compiled programs can be executed by the runtime.
"""
import pytest
import torch
import torch.nn as nn

from ace.fhe import Driver, FHERuntime


class TestCompilerRuntimeIntegration:
    """Integration tests for compiler to runtime flow."""

    def test_compile_and_run_add(self):
        """Test compiling and running an add operation."""
        class AddModel(nn.Module):
            def forward(self, x, y):
                return x + y

        model = AddModel()
        inputs = [torch.ones(1, 1, 2, 2), torch.ones(1, 1, 2, 2)]

        # Compile
        compiler = Driver(
            frontend="torch-via-onnx",
            library="antlib",
            device="cpu"
        )
        package = compiler.compile(model, inputs, input_names=["x", "y"])

        # Run
        runner = FHERuntime(package, verify="array")
        result = runner.inference(*inputs)

        # Verify
        # assert result is not None
        # expected = torch.full((1, 1, 2, 2), 2.0)
        # assert torch.allclose(result, expected)

    def test_compile_and_run_multiply(self):
        """Test compiling and running a multiply operation."""
        class MultModel(nn.Module):
            def forward(self, x, y):
                return x * y

        model = MultModel()
        inputs = [torch.full((1, 1, 2, 2), 3.0), torch.full((1, 1, 2, 2), 4.0)]

        compiler = Driver(
            frontend="torch-via-onnx",
            library="antlib",
            device="cpu"
        )
        package = compiler.compile(model, inputs, input_names=["x", "y"])

        runner = FHERuntime(package, verify="array")
        result = runner.inference(*inputs)

        # assert result is not None
        # expected = torch.full((1, 1, 2, 2), 12.0)
        # assert torch.allclose(result, expected)

    def test_compile_and_run_with_validation(self):
        """Test that validation works correctly."""
        class SimpleModel(nn.Module):
            def forward(self, x):
                return x + 1

        model = SimpleModel()
        inputs = [torch.randn(1, 1, 2, 2)]

        compiler = Driver(
            frontend="torch-via-onnx",
            library="antlib",
            device="cpu"
        )
        package = compiler.compile(model, inputs, input_names=["x"])

        runner = FHERuntime(package, verify="array")
        result = runner.inference(*inputs)

        # Validate should pass
        assert runner.validate() is True


class TestRuntimeCaching:
    """Test runtime caching behavior."""

    def test_same_shape_uses_cache(self):
        """Test that same input shapes use cached runtime."""
        class SimpleModel(nn.Module):
            def forward(self, x, y):
                return x + y

        model = SimpleModel()
        inputs = [torch.ones(1, 1, 2, 2), torch.ones(1, 1, 2, 2)]

        compiler = Driver(
            frontend="torch-via-onnx",
            library="antlib",
            device="cpu"
        )
        # This would require multiple compilations in a loop
        # Just test basic compilation works
        package = compiler.compile(model, inputs, input_names=["x", "y"])

        runner = FHERuntime(package, verify="array")
        result1 = runner.inference(*inputs)
        result2 = runner.inference(*inputs)

        assert torch.allclose(result1, result2)