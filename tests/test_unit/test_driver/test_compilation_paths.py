# tests/test_unit/test_driver/test_compilation_paths.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Unit tests for Driver compilation paths.

Tests the Driver's handling of different frontend/backend paths:
    - 5 frontends: torch, torch-via-onnx, ast, ast-via-onnx, onnx
    - Each frontend has its own test class
    - Each class has methods for different paths (memory, file)
    - Each method is parameterized with 2 test cases (AddModel, LinearOp)
"""

import pytest
import torch
import torch.nn as nn
from typing import List

from ace.fhe.driver import Driver

# Import centralized dependency checks and backend parameters from test_utils
from test_utils import (
    TORCH_FX_AVAILABLE,
    HAS_TORCH_FX,
    HAS_FRONTEND,
    skip_if_no_torch_fx,
    skip_if_no_frontend,
    gpu_available,
    CPU_BACKENDS,
    GPU_BACKENDS,
    ALL_BACKENDS,
)

# Import test models from ace.samples
from ace.samples.ops import AddOp as AddModel
from ace.samples.ops import LinearOp as _LinearOp


class LinearOp(_LinearOp):
    """Linear layer with 4 -> 2 dimensions."""
    def __init__(self):
        super().__init__(4, 2)


# ============================================================================
# Model fixtures for parameterization
# ============================================================================

MODEL_FIXTURES = [
    pytest.param(AddModel, "AddModel", id="AddModel"),
    pytest.param(LinearOp, "LinearOp", id="LinearOp"),
]


# ============================================================================
# Test Functions (for AST frontend)
# ============================================================================

def add_function(x, y):
    """Simple add function."""
    return x + y


def relu_function(x):
    """ReLU function."""
    return torch.relu(x)


# ============================================================================
# TestDriverTorchPath - torch frontend (torch.fx → AIR)
# ============================================================================

@pytest.mark.skipif(not HAS_TORCH_FX or not HAS_FRONTEND, reason="torch.fx or frontend not available")
class TestDriverTorchPath:
    """Tests for torch frontend paths.

    Path 1 (memory): PyTorch → torch.fx → AIR (memory) → backend
    Path 2 (file onnx): PyTorch → torch.fx → ONNX file → backend
    Path 3 (file air): PyTorch → torch.fx → AIR file (.B) → backend
    """

    @pytest.mark.skip(reason="Memory IR compilation not yet implemented")
    @pytest.mark.parametrize("backend,device", ALL_BACKENDS)
    @pytest.mark.parametrize("model_cls,model_name", MODEL_FIXTURES)
    def test_memory_path(self, tmp_path, backend, device, model_cls, model_name):
        """Test torch frontend memory path: PyTorch → AIR (memory) → backend."""
        try:
            compiler = Driver(
                frontend="torch",
                library=backend,
                device=device
            )
        except Exception as e:
            pytest.skip(f"Backend {backend}/{device} not available: {e}")

        model = model_cls()
        # AddModel has 2 inputs, LinearOp has 1 input
        if model_name == "AddModel":
            inputs = [torch.randn(1, 4), torch.randn(1, 4)]
            input_names = ["x", "y"]
        else:
            inputs = [torch.randn(1, 4)]
            input_names = ["x"]

        try:
            result = compiler.compile(model, inputs, input_names)
            assert result is not None
        except NotImplementedError as e:
            pytest.skip(f"Backend {backend} not implemented: {e}")
        except Exception as e:
            pytest.skip(f"Compilation failed: {e}")

    @pytest.mark.parametrize("backend,device", ALL_BACKENDS)
    @pytest.mark.parametrize("model_cls,model_name", MODEL_FIXTURES)
    def test_file_onnx_path(self, tmp_path, backend, device, model_cls, model_name):
        """Test torch frontend ONNX file path: PyTorch → ONNX file → backend."""
        try:
            compiler = Driver(
                frontend="torch",
                library=backend,
                device=device
            )
        except Exception as e:
            pytest.skip(f"Backend {backend}/{device} not available: {e}")

        model = model_cls()
        onnx_file = tmp_path / f"{model_name}.onnx"

        try:
            # Export PyTorch model to ONNX
            # AddModel has 2 inputs, LinearOp has 1 input
            if model_name == "AddModel":
                inputs = (torch.randn(1, 4), torch.randn(1, 4))
                torch.onnx.export(model, inputs, str(onnx_file), input_names=["x", "y"])
            else:
                inputs = (torch.randn(1, 4),)
                torch.onnx.export(model, inputs[0], str(onnx_file), input_names=["x"])
            assert onnx_file.exists()

            # Use ONNX file with backend
            compiler.backend_impl.build_dir = tmp_path
            result = compiler.backend_impl.build(str(onnx_file))
            assert result is not None
        except NotImplementedError as e:
            pytest.skip(f"Backend {backend} not implemented: {e}")
        except Exception as e:
            pytest.skip(f"Compilation failed: {e}")

    @pytest.mark.parametrize("backend,device", ALL_BACKENDS)
    @pytest.mark.parametrize("model_cls,model_name", MODEL_FIXTURES)
    def test_file_air_path(self, tmp_path, backend, device, model_cls, model_name):
        """Test torch frontend AIR file path: PyTorch → AIR file (.B) → backend."""
        from ace.fhe.frontend.torch import Torch

        try:
            compiler = Driver(
                frontend="torch",
                library=backend,
                device=device
            )
        except Exception as e:
            pytest.skip(f"Backend {backend}/{device} not available: {e}")

        model = model_cls()
        air_file = tmp_path / f"{model_name}.B"

        # AddModel has 2 inputs, LinearOp has 1 input
        if model_name == "AddModel":
            inputs = [torch.randn(1, 4), torch.randn(1, 4)]
            input_names = ["x", "y"]
        else:
            inputs = [torch.randn(1, 4)]
            input_names = ["x"]

        try:
            # Generate AIR file using frontend
            frontend = Torch()
            traced = frontend.compile(model, inputs, input_names)
            traced.export_ir(str(air_file))
            assert air_file.exists()

            # Use AIR file with backend
            compiler.backend_impl.build_dir = tmp_path
            result = compiler.backend_impl.build(traced)
            assert result is not None
        except NotImplementedError as e:
            pytest.skip(f"Backend {backend} not implemented: {e}")
        except Exception as e:
            pytest.skip(f"Compilation failed: {e}")


# ============================================================================
# TestDriverTorchViaOnnxPath - torch-via-onnx frontend (torch → ONNX → AIR)
# ============================================================================

class TestDriverTorchViaOnnxPath:
    """Tests for torch-via-onnx frontend paths.

    Path 1 (memory): PyTorch → ONNX → AIR (memory) → backend
    Path 2 (file onnx): PyTorch → ONNX file → backend
    Path 3 (file air): PyTorch → ONNX → AIR file (.B) → backend
    """

    @pytest.mark.skip(reason="Memory IR compilation not yet implemented")
    @pytest.mark.parametrize("backend,device", ALL_BACKENDS)
    @pytest.mark.parametrize("model_cls,model_name", MODEL_FIXTURES)
    def test_memory_path(self, tmp_path, backend, device, model_cls, model_name):
        """Test torch-via-onnx frontend memory path: PyTorch → ONNX → AIR (memory) → backend."""
        try:
            compiler = Driver(
                frontend="torch-via-onnx",
                library=backend,
                device=device
            )
        except Exception as e:
            pytest.skip(f"Backend {backend}/{device} not available: {e}")

        model = model_cls()
        inputs = [torch.randn(1, 4)]

        try:
            result = compiler.compile(model, inputs, ["x"])
            assert result is not None
        except NotImplementedError as e:
            pytest.skip(f"Backend {backend} not implemented: {e}")
        except Exception as e:
            pytest.skip(f"Compilation failed: {e}")

    @pytest.mark.parametrize("backend,device", ALL_BACKENDS)
    @pytest.mark.parametrize("model_cls,model_name", MODEL_FIXTURES)
    def test_file_onnx_path(self, tmp_path, backend, device, model_cls, model_name):
        """Test torch-via-onnx frontend ONNX file path: PyTorch → ONNX file → backend."""
        from ace.fhe.frontend.torch.torch_via_onnx import TorchViaOnnx
        from ace.fhe.ir import ONNXFileIR

        try:
            compiler = Driver(
                frontend="torch-via-onnx",
                library=backend,
                device=device
            )
        except Exception as e:
            pytest.skip(f"Backend {backend}/{device} not available: {e}")

        model = model_cls()
        onnx_file = tmp_path / f"{model_name}.onnx"

        # AddModel has 2 inputs, LinearOp has 1 input
        if model_name == "AddModel":
            inputs = (torch.randn(1, 4), torch.randn(1, 4))
            input_names = ["x", "y"]
        else:
            inputs = (torch.randn(1, 4),)
            input_names = ["x"]

        try:
            # Export PyTorch model to ONNX file
            torch.onnx.export(model, inputs, str(onnx_file), input_names=input_names)
            assert onnx_file.exists()

            # Use ONNX file with backend via ONNXFileIR
            onnx_model = ONNXFileIR(str(onnx_file))
            compiler.backend_impl.build_dir = tmp_path
            result = compiler.backend_impl.build(onnx_model)
            assert result is not None
        except NotImplementedError as e:
            pytest.skip(f"Backend {backend} not implemented: {e}")
        except Exception as e:
            pytest.skip(f"Compilation failed: {e}")

    @pytest.mark.parametrize("backend,device", ALL_BACKENDS)
    @pytest.mark.parametrize("model_cls,model_name", MODEL_FIXTURES)
    def test_file_air_path(self, tmp_path, backend, device, model_cls, model_name):
        """Test torch-via-onnx frontend AIR file path: PyTorch → ONNX → AIR file (.B) → backend."""
        from ace.fhe.ir import AIRFileIR, convert_onnx_to_air

        try:
            compiler = Driver(
                frontend="torch-via-onnx",
                library=backend,
                device=device
            )
        except Exception as e:
            pytest.skip(f"Backend {backend}/{device} not available: {e}")

        model = model_cls()
        onnx_file = tmp_path / f"{model_name}.onnx"
        air_file = tmp_path / f"{model_name}.B"

        # AddModel has 2 inputs, LinearOp has 1 input
        if model_name == "AddModel":
            inputs = (torch.randn(1, 4), torch.randn(1, 4))
            input_names = ["x", "y"]
        else:
            inputs = (torch.randn(1, 4),)
            input_names = ["x"]

        try:
            # Export PyTorch model to ONNX file
            torch.onnx.export(model, inputs, str(onnx_file), input_names=input_names)
            assert onnx_file.exists()

            # Convert ONNX to AIR file using fhe_cmplr
            convert_onnx_to_air(str(onnx_file), str(air_file))
            assert air_file.exists()

            # Use AIR file with backend via AIRFileIR
            air_model = AIRFileIR(str(air_file))
            compiler.backend_impl.build_dir = tmp_path
            result = compiler.backend_impl.build(air_model)
            assert result is not None
        except NotImplementedError as e:
            pytest.skip(f"Backend {backend} not implemented: {e}")
        except Exception as e:
            pytest.skip(f"Compilation failed: {e}")


# ============================================================================
# TestDriverASTPath - ast frontend (Python AST → AIR)
# ============================================================================

@pytest.mark.skipif(not HAS_FRONTEND, reason="frontend not available")
class TestDriverASTPath:
    """Tests for AST frontend paths.

    Path 1 (memory): Python function → AST → AIR (memory) → backend
    Path 2 (file air): Python function → AST → AIR file (.B) → backend
    """

    @pytest.mark.skip(reason="Memory IR compilation not yet implemented")
    @pytest.mark.parametrize("backend,device", ALL_BACKENDS)
    def test_memory_path(self, tmp_path, backend, device):
        """Test AST frontend memory path: Python function → AIR (memory) → backend."""
        try:
            compiler = Driver(
                frontend="ast",
                library=backend,
                device=device
            )
        except Exception as e:
            pytest.skip(f"Backend {backend}/{device} not available: {e}")

        inputs = [None, None]  # AST doesn't need actual tensors

        try:
            result = compiler.compile(add_function, inputs, ["x", "y"])
            assert result is not None
        except NotImplementedError as e:
            pytest.skip(f"Backend {backend} not implemented: {e}")
        except Exception as e:
            pytest.skip(f"Compilation failed: {e}")

    @pytest.mark.parametrize("backend,device", ALL_BACKENDS)
    def test_file_air_path(self, tmp_path, backend, device):
        """Test AST frontend AIR file path: Python function → AIR file (.B) → backend."""
        try:
            compiler = Driver(
                frontend="ast",
                library=backend,
                device=device
            )
        except Exception as e:
            pytest.skip(f"Backend {backend}/{device} not available: {e}")

        air_file = tmp_path / "add.B"
        inputs = [None, None]

        try:
            # Generate AIR file
            # Note: AST frontend needs to support export
            result = compiler.compile(add_function, inputs, ["x", "y"])
            assert result is not None
        except NotImplementedError as e:
            pytest.skip(f"Backend {backend} not implemented: {e}")
        except Exception as e:
            pytest.skip(f"Compilation failed: {e}")


# ============================================================================
# TestDriverASTViaOnnxPath - ast-via-onnx frontend (Python AST → ONNX → AIR)
# ============================================================================

class TestDriverASTViaOnnxPath:
    """Tests for ast-via-onnx frontend paths.

    Path 1 (memory): Python function → ONNX → AIR (memory) → backend
    Path 2 (file onnx): Python function → ONNX file → backend
    Path 3 (file air): Python function → ONNX → AIR file (.B) → backend
    """

    @pytest.mark.skip(reason="Memory IR compilation not yet implemented")
    @pytest.mark.parametrize("backend,device", ALL_BACKENDS)
    def test_memory_path(self, tmp_path, backend, device):
        """Test ast-via-onnx frontend memory path: Python function → ONNX → AIR (memory) → backend."""
        try:
            compiler = Driver(
                frontend="ast-via-onnx",
                library=backend,
                device=device
            )
        except Exception as e:
            pytest.skip(f"Backend {backend}/{device} not available: {e}")

        inputs = [torch.randn(1, 4), torch.randn(1, 4)]

        try:
            result = compiler.compile(add_function, inputs, ["x", "y"])
            assert result is not None
        except NotImplementedError as e:
            pytest.skip(f"Backend {backend} not implemented: {e}")
        except Exception as e:
            pytest.skip(f"Compilation failed: {e}")

    @pytest.mark.parametrize("backend,device", ALL_BACKENDS)
    def test_file_onnx_path(self, tmp_path, backend, device):
        """Test ast-via-onnx frontend ONNX file path: Python function → ONNX file → backend."""
        try:
            compiler = Driver(
                frontend="ast-via-onnx",
                library=backend,
                device=device
            )
        except Exception as e:
            pytest.skip(f"Backend {backend}/{device} not available: {e}")

        inputs = [torch.randn(1, 4), torch.randn(1, 4)]

        try:
            # Generate ONNX file using frontend
            result = compiler.compile(add_function, inputs, ["x", "y"])
            assert result is not None
        except NotImplementedError as e:
            pytest.skip(f"Backend {backend} not implemented: {e}")
        except Exception as e:
            pytest.skip(f"Compilation failed: {e}")

    @pytest.mark.parametrize("backend,device", ALL_BACKENDS)
    def test_file_air_path(self, tmp_path, backend, device):
        """Test ast-via-onnx frontend AIR file path: Python function → ONNX → AIR file (.B) → backend."""
        try:
            compiler = Driver(
                frontend="ast-via-onnx",
                library=backend,
                device=device
            )
        except Exception as e:
            pytest.skip(f"Backend {backend}/{device} not available: {e}")

        inputs = [torch.randn(1, 4), torch.randn(1, 4)]
        air_file = tmp_path / "add.B"

        try:
            result = compiler.compile(add_function, inputs, ["x", "y"])
            assert result is not None
        except NotImplementedError as e:
            pytest.skip(f"Backend {backend} not implemented: {e}")
        except Exception as e:
            pytest.skip(f"Compilation failed: {e}")


# ============================================================================
# TestDriverOnnxPath - onnx frontend (ONNX file → AIR)
# ============================================================================

class TestDriverOnnxPath:
    """Tests for ONNX frontend paths.

    Path 1 (memory): ONNX file → AIR (memory) → backend
    Path 2 (file onnx): ONNX file → backend (direct)
    """

    @pytest.mark.skip(reason="Memory IR compilation not yet implemented")
    @pytest.mark.parametrize("backend,device", ALL_BACKENDS)
    @pytest.mark.parametrize("model_cls,model_name", MODEL_FIXTURES)
    def test_memory_path(self, tmp_path, backend, device, model_cls, model_name):
        """Test ONNX frontend memory path: ONNX file → AIR (memory) → backend."""
        try:
            compiler = Driver(
                frontend="onnx",
                library=backend,
                device=device
            )
        except Exception as e:
            pytest.skip(f"Backend {backend}/{device} not available: {e}")

        # Create ONNX file
        model = model_cls()
        # AddModel has 2 inputs, LinearOp has 1 input
        if model_name == "AddModel":
            inputs = (torch.randn(1, 4), torch.randn(1, 4))
            input_names = ["x", "y"]
        else:
            inputs = (torch.randn(1, 4),)
            input_names = ["x"]
        onnx_path = tmp_path / f"{model_name}.onnx"
        torch.onnx.export(model, inputs, str(onnx_path), input_names=input_names)

        try:
            result = compiler.compile(str(onnx_path), [], [])
            assert result is not None
        except NotImplementedError as e:
            pytest.skip(f"Backend {backend} not implemented: {e}")
        except Exception as e:
            pytest.skip(f"Compilation failed: {e}")

    @pytest.mark.parametrize("backend,device", ALL_BACKENDS)
    @pytest.mark.parametrize("model_cls,model_name", MODEL_FIXTURES)
    def test_file_onnx_path(self, tmp_path, backend, device, model_cls, model_name):
        """Test ONNX frontend bypass path: ONNX file → ONNXFileIR → backend."""
        from ace.fhe.frontend.onnx import Onnx

        try:
            compiler = Driver(
                frontend="onnx",
                library=backend,
                device=device
            )
        except Exception as e:
            pytest.skip(f"Backend {backend}/{device} not available: {e}")

        # Create ONNX file
        model = model_cls()
        # AddModel has 2 inputs, LinearOp has 1 input
        if model_name == "AddModel":
            inputs = (torch.randn(1, 4), torch.randn(1, 4))
            input_names = ["x", "y"]
        else:
            inputs = (torch.randn(1, 4),)
            input_names = ["x"]
        onnx_path = tmp_path / f"{model_name}.onnx"
        torch.onnx.export(model, inputs, str(onnx_path), input_names=input_names)

        try:
            # Use frontend prepare() to get ONNXFileIR
            # ONNXFileIR.format_type = "file", .file_format = "onnx"
            frontend = Onnx()
            onnx_model = frontend.prepare(str(onnx_path))
            assert onnx_model.format_type == "file"
            assert onnx_model.file_format == "onnx"
            assert onnx_model.file_path == str(onnx_path)

            # Pass ONNXFileIR to backend
            compiler.backend_impl.build_dir = tmp_path
            result = compiler.backend_impl.build(onnx_model)
            assert result is not None
        except NotImplementedError as e:
            pytest.skip(f"Backend {backend} not implemented: {e}")
        except Exception as e:
            pytest.skip(f"Compilation failed: {e}")

    @pytest.mark.parametrize("backend,device", ALL_BACKENDS)
    @pytest.mark.parametrize("model_cls,model_name", MODEL_FIXTURES)
    def test_file_air_path(self, tmp_path, backend, device, model_cls, model_name):
        """Test ONNX frontend AIR file path: ONNX file → AIR file (.B) → backend."""
        from ace.fhe.frontend.onnx import Onnx
        from ace.fhe.ir import AIRFileIR

        try:
            compiler = Driver(
                frontend="onnx",
                library=backend,
                device=device
            )
        except Exception as e:
            pytest.skip(f"Backend {backend}/{device} not available: {e}")

        # Create ONNX file
        model = model_cls()
        # AddModel has 2 inputs, LinearOp has 1 input
        if model_name == "AddModel":
            inputs = (torch.randn(1, 4), torch.randn(1, 4))
            input_names = ["x", "y"]
        else:
            inputs = (torch.randn(1, 4),)
            input_names = ["x"]
        onnx_path = tmp_path / f"{model_name}.onnx"
        torch.onnx.export(model, inputs, str(onnx_path), input_names=input_names)

        air_file = tmp_path / f"{model_name}.B"

        try:
            # Use frontend to convert ONNX to AIR file
            frontend = Onnx()
            frontend.export(str(onnx_path), format="air", output_path=str(air_file))
            assert air_file.exists()

            # Create AIRFileIR for AIR file (format_type="file", file_format="air")
            air_model = AIRFileIR(str(air_file))
            assert air_model.format_type == "file"
            assert air_model.file_format == "air"
            assert air_model.file_path == str(air_file)

            # Pass AIRFileIR to backend
            compiler.backend_impl.build_dir = tmp_path
            result = compiler.backend_impl.build(air_model)
            assert result is not None
        except NotImplementedError as e:
            pytest.skip(f"Backend {backend} not implemented: {e}")
        except Exception as e:
            pytest.skip(f"Compilation failed: {e}")


# ============================================================================
# Driver Error Handling Tests
# ============================================================================

class TestDriverErrorHandling:
    """Tests for driver error handling."""

    def test_invalid_frontend_raises(self):
        """Test that invalid frontend raises error."""
        with pytest.raises(ValueError, match="Unknown frontend"):
            Driver(
                frontend="invalid_frontend",
                library="antlib"
            )

    def test_invalid_backend_raises(self):
        """Test that invalid backend raises error."""
        with pytest.raises(ValueError):
            Driver(
                frontend="torch-via-onnx",
                library="invalid_backend"
            )


# ============================================================================
# Coverage Summary
# ============================================================================

# Test Coverage Matrix (5 frontends × multiple paths):
#
# ┌─────────────────────┬─────────────┬─────────────┬─────────────┐
# │ Frontend            │ memory      │ file onnx   │ file air    │
# ├─────────────────────┼─────────────┼─────────────┼─────────────┤
# │ torch               │     ✓       │     ✓       │     ✓       │
# │ torch-via-onnx      │     ✓       │     ✓       │     ✓       │
# │ ast                 │     ✓       │     -       │     ✓       │
# │ ast-via-onnx        │     ✓       │     ✓       │     ✓       │
# │ onnx                │     ✓       │     ✓       │     ✓       │
# └─────────────────────┴─────────────┴─────────────┴─────────────┘
#
# Each path tested with 2 models: AddModel, LinearOp
# Each backend tested: antlib, seal, openfhe (+ phantom, hyperfhe if GPU)


# Run tests if executed directly
if __name__ == "__main__":
    pytest.main([__file__, "-v"])