# tests/unit/frontend/test_torch_via_onnx.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Unit tests for torch-via-onnx frontend.

Tests the ONNX export path:
1. prepare() -> ONNXFileIR (bypass mode)
2. compile() -> ONNXFileIR (same as prepare for this frontend)
"""
import pytest
import torch

from utils import skip_if_no_onnx
from ace.fhe.ir import ONNXFileIR
from ace.sample.ops.specs import LINEAR_OP, ADD_OP, RELU_OP
from ace.sample.funcs.specs import ADD_FUNC

# Representative specs: single-input (linear, relu) + multi-input (add)
_OP_SPECS = [LINEAR_OP, RELU_OP, ADD_OP]


# =============================================================================
# Test: prepare() - Bypass Mode
# =============================================================================

@skip_if_no_onnx
class TestPrepare:
    """Tests for prepare() method - Bypass mode."""

    @pytest.mark.parametrize("spec", _OP_SPECS, ids=lambda s: s.name)
    def test_returns_onnx_file_ir(self, torch_via_onnx_frontend, spec):
        model = spec.create_model()
        result = torch_via_onnx_frontend.prepare(model, list(spec.example_inputs), spec.encrypt_inputs)
        assert isinstance(result, ONNXFileIR)

    @pytest.mark.parametrize("spec", _OP_SPECS, ids=lambda s: s.name)
    def test_file_mode_properties(self, torch_via_onnx_frontend, spec):
        """prepare() returns file-mode ONNX IR."""
        model = spec.create_model()
        result = torch_via_onnx_frontend.prepare(model, list(spec.example_inputs), spec.encrypt_inputs)
        assert result.format_type == "file"
        assert result.file_format == "onnx"

    def test_with_multi_input_function(self, torch_via_onnx_frontend):
        result = torch_via_onnx_frontend.prepare(
            ADD_FUNC.func, list(ADD_FUNC.example_inputs), ADD_FUNC.encrypt_inputs)
        assert isinstance(result, ONNXFileIR)


# =============================================================================
# Test: compile() - Returns ONNXFileIR
# =============================================================================

@skip_if_no_onnx
class TestCompile:
    """Tests for compile() method - returns ONNXFileIR."""

    @pytest.mark.parametrize("spec", _OP_SPECS, ids=lambda s: s.name)
    def test_returns_onnx_file_ir(self, torch_via_onnx_frontend, spec):
        model = spec.create_model()
        result = torch_via_onnx_frontend.compile(model, list(spec.example_inputs), spec.encrypt_inputs)
        assert isinstance(result, ONNXFileIR)


# =============================================================================
# Test: IR Properties
# =============================================================================

@skip_if_no_onnx
class TestIRProperties:
    """Tests for IR object properties."""

    @pytest.mark.parametrize("spec", _OP_SPECS, ids=lambda s: s.name)
    def test_onnx_file_ir_properties(self, torch_via_onnx_frontend, spec):
        model = spec.create_model()
        result = torch_via_onnx_frontend.prepare(model, list(spec.example_inputs), spec.encrypt_inputs)
        assert hasattr(result, "entry_name")
        assert result.entry_name
        assert hasattr(result, "onnx_path")
        assert result.onnx_path == result.file_path


# =============================================================================
# Test: Edge Cases
# =============================================================================

@skip_if_no_onnx
class TestEdgeCases:
    """Tests for edge cases and error handling."""

    def test_prepare_with_none_input(self, torch_via_onnx_frontend):
        """Test prepare() with None input raises appropriate error."""
        from torch.onnx._internal.exporter._errors import TorchExportError
        with pytest.raises((TypeError, AttributeError, TorchExportError)):
            torch_via_onnx_frontend.prepare(None, [torch.randn(1, 4)], ["x"])

    def test_prepare_with_empty_inputs(self, torch_via_onnx_frontend):
        """Test prepare() with empty inputs list raises AssertionError."""
        model = LINEAR_OP.create_model()
        with pytest.raises(AssertionError):
            torch_via_onnx_frontend.prepare(model, [], LINEAR_OP.encrypt_inputs)

    def test_prepare_with_wrong_input_count(self, torch_via_onnx_frontend):
        """Test prepare() with wrong number of inputs."""
        model = LINEAR_OP.create_model()
        inputs = [torch.randn(1, 4), torch.randn(1, 4)]
        with pytest.raises(AssertionError):
            torch_via_onnx_frontend.prepare(model, inputs, LINEAR_OP.encrypt_inputs)


# =============================================================================
# Test: Frontend Metadata
# =============================================================================

@skip_if_no_onnx
class TestFrontendMeta:
    """Tests for frontend metadata."""

    def test_frontend_name(self, torch_via_onnx_frontend):
        assert torch_via_onnx_frontend.name() == "torch-via-onnx"