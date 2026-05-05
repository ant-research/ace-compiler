# tests/test_unit/test_frontend/test_torch.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Unit tests for Torch torch_frontend.

Tests three output modes:
1. Memory: prepare() -> TorchTracedModel -> compile() -> AIR IR in memory
2. AIR file: export(format="air") -> .B file
3. ONNX file: export(format="onnx") -> ONNX file

Note: Torch torch_frontend is UNIQUE - it's the only torch_frontend where memory mode works!
It uses FX tracing + custom C++ operators (via IRBuilder) to generate AIR IR in memory.
"""
import pytest
import torch
import torch.nn as nn
import os

from test_utils import HAS_TORCH_FX, HAS_FRONTEND

from ace.fhe.ir import TorchTracedModel

from ace.samples.tensor_ops import (
    AddTensorOp,
    SubTensorOp,
    MulTensorOp,
    CompositeTensorOp,
    ReLUTensorOp,
    SqrtTensorOp,
    FlattenTensorOp,
)

# Standard PyTorch models - test the GraphTransform path
from ace.samples.ops import (
    AddOp,
    ReluOp,
    LinearOp,
    Conv2dReluOp,
    GlobalAvgPool2dOp,
)


# Skip markers
requires_frontend = pytest.mark.skipif(
    not HAS_TORCH_FX or not HAS_FRONTEND,
    reason="torch.fx or torch_frontend not available"
)


# =============================================================================
# Test: prepare() - Memory Mode
# =============================================================================

@requires_frontend
class TestPrepare:
    """Tests for prepare() method - Memory mode."""

    def test_returns_torch_traced_model(self, torch_frontend, input_pair):
        """Test that prepare() returns TorchTracedModel."""
        model = AddTensorOp()
        result = torch_frontend.prepare(model, list(input_pair), ["x", "y"])
        assert isinstance(result, TorchTracedModel)

    def test_format_type_is_memory(self, torch_frontend, input_pair):
        """Test that format_type is 'memory'."""
        model = AddTensorOp()
        result = torch_frontend.prepare(model, list(input_pair), ["x", "y"])
        assert result.format_type == "memory"

    def test_file_format_is_none(self, torch_frontend, input_pair):
        """Test that file_format is None."""
        model = AddTensorOp()
        result = torch_frontend.prepare(model, list(input_pair), ["x", "y"])
        assert result.file_format is None

    def test_file_path_is_none(self, torch_frontend, input_pair):
        """Test that file_path is None."""
        model = AddTensorOp()
        result = torch_frontend.prepare(model, list(input_pair), ["x", "y"])
        assert result.file_path is None

    def test_air_not_generated(self, torch_frontend, input_pair):
        """Test that AIR IR is not generated after prepare()."""
        model = AddTensorOp()
        result = torch_frontend.prepare(model, list(input_pair), ["x", "y"])
        assert not result.is_air_generated()

    def test_traced_model_has_graph(self, torch_frontend, input_pair):
        """Test that traced model has FX graph."""
        model = AddTensorOp()
        result = torch_frontend.prepare(model, list(input_pair), ["x", "y"])
        assert result.traced_model is not None
        code = result.get_graph_code()
        assert len(code) > 0

    def test_auto_generate_input_names(self, torch_frontend, input_pair):
        """Test that input names are auto-generated."""
        model = AddTensorOp()
        result = torch_frontend.prepare(model, list(input_pair))
        assert result is not None


# =============================================================================
# Test: compile() - Memory Mode (Executed)
# =============================================================================

@requires_frontend
class TestCompile:
    """Tests for compile() method - Memory mode (executed)."""

    def test_returns_torch_traced_model(self, torch_frontend, input_pair):
        """Test that compile() returns TorchTracedModel."""
        model = AddTensorOp()
        result = torch_frontend.compile(model, list(input_pair), ["x", "y"])
        assert isinstance(result, TorchTracedModel)

    def test_format_type_is_file(self, torch_frontend, input_pair, tmp_path):
        """Test that format_type is 'file' after compile."""
        model = AddTensorOp()
        result = torch_frontend.compile(model, list(input_pair), ["x", "y"], build_dir=str(tmp_path))
        assert result.format_type == "file"

    def test_air_is_generated(self, torch_frontend, input_pair, tmp_path):
        """Test that AIR IR is generated."""
        model = AddTensorOp()
        result = torch_frontend.compile(model, list(input_pair), ["x", "y"], build_dir=str(tmp_path))
        assert result.is_air_generated()

    def test_with_custom_op_model(self, torch_frontend, input_pair):
        """Test compile() with custom ops."""
        model = AddTensorOp()
        result = torch_frontend.compile(model, list(input_pair), ["x", "y"])
        assert result.is_air_generated()
        custom_ops = result.get_custom_ops()
        assert "add" in str(custom_ops)

    def test_with_composite_model(self, torch_frontend, input_pair):
        """Test compile() with composite model."""
        model = CompositeTensorOp()
        result = torch_frontend.compile(model, list(input_pair), ["x", "y"])
        assert result.is_air_generated()
        custom_ops = result.get_custom_ops()
        assert len(custom_ops) >= 3  # add, mul, relu


# =============================================================================
# Test: export(format="air")
# =============================================================================

@requires_frontend
class TestExportAir:
    """Tests for export(format="air") method."""

    def test_creates_air_file(self, torch_frontend, input_pair, tmp_path):
        """Test that export(format="air") creates .B file."""
        model = AddTensorOp()
        output_path = str(tmp_path / "output.B")
        result_path = torch_frontend.export(model, list(input_pair),
                                       format="air", output_path=output_path,
                                       input_names=["x", "y"])
        assert result_path == output_path
        assert os.path.exists(output_path)

    def test_format_type_becomes_file(self, torch_frontend, input_pair, tmp_path):
        """Test that format_type becomes 'file' after export."""
        model = AddTensorOp()
        output_path = str(tmp_path / "output.B")
        traced = torch_frontend.prepare(model, list(input_pair), ["x", "y"])
        assert traced.format_type == "memory"
        traced.execute(*input_pair)
        traced.export_ir(output_path)
        assert traced.format_type == "file"

    def test_file_format_is_air(self, torch_frontend, input_pair, tmp_path):
        """Test that file_format is 'air' after export."""
        model = AddTensorOp()
        output_path = str(tmp_path / "output.B")
        traced = torch_frontend.compile(model, list(input_pair), ["x", "y"])
        traced.export_ir(output_path)
        assert traced.file_format == "air"

    def test_file_path_is_set(self, torch_frontend, input_pair, tmp_path):
        """Test that file_path is set after export."""
        model = AddTensorOp()
        output_path = str(tmp_path / "output.B")
        traced = torch_frontend.compile(model, list(input_pair), ["x", "y"])
        traced.export_ir(output_path)
        assert traced.file_path == output_path


# =============================================================================
# Test: export(format="onnx")
# =============================================================================

@requires_frontend
class TestExportOnnx:
    """Tests for export(format="onnx") method."""

    def test_creates_onnx_file(self, torch_frontend, add_model, input_pair, tmp_path):
        """Test that export(format="onnx") creates ONNX file."""
        output_path = str(tmp_path / "output.onnx")
        result_path = torch_frontend.export(add_model, list(input_pair),
                                       format="onnx", output_path=output_path,
                                       input_names=["x", "y"])
        assert result_path == output_path
        assert os.path.exists(output_path)

    def test_creates_valid_onnx(self, torch_frontend, add_model, input_pair, tmp_path):
        """Test that exported ONNX file is valid."""
        output_path = str(tmp_path / "output.onnx")
        torch_frontend.export(add_model, list(input_pair),
                        format="onnx", output_path=output_path,
                        input_names=["x", "y"])
        import onnx
        onnx_model = onnx.load(output_path)
        onnx.checker.check_model(onnx_model)

    def test_with_multi_input_model(self, torch_frontend, add_model, input_pair, tmp_path):
        """Test export ONNX with multi-input model."""
        output_path = str(tmp_path / "multi.onnx")
        result_path = torch_frontend.export(add_model, list(input_pair),
                                       format="onnx", output_path=output_path,
                                       input_names=["x", "y"])
        assert os.path.exists(result_path)
        import onnx
        onnx_model = onnx.load(output_path)
        assert len(onnx_model.graph.input) == 2


# =============================================================================
# Test: IR Properties
# =============================================================================

@requires_frontend
class TestIRProperties:
    """Tests for IR object properties."""

    def test_entry_name_exists(self, torch_frontend, input_pair):
        """Test that TorchTracedModel has entry_name."""
        model = AddTensorOp()
        result = torch_frontend.compile(model, list(input_pair), ["x", "y"])
        assert hasattr(result, "entry_name")
        assert result.entry_name

    def test_get_air_scopes(self, torch_frontend, input_pair):
        """Test getting AIR scopes after execution."""
        model = AddTensorOp()
        result = torch_frontend.compile(model, list(input_pair), ["x", "y"])
        func_scope = result.get_air_func_scope()
        glob_scope = result.get_air_glob_scope()
        assert func_scope is not None or glob_scope is not None


# =============================================================================
# Test: TorchTracedModel Methods
# =============================================================================

@requires_frontend
class TestTorchTracedModelMethods:
    """Tests for TorchTracedModel helper methods."""

    def test_write_ir_success(self, torch_frontend, input_pair, tmp_path):
        """Test writing AIR IR to file."""
        model = AddTensorOp()
        traced = torch_frontend.compile(model, list(input_pair), ["x", "y"])
        output_file = str(tmp_path / "test_output.air")
        result = traced.write_ir(output_file)
        assert result is True

    def test_write_ir_before_execute_raises(self, torch_frontend, input_pair):
        """Test that write_ir raises error if called before execute."""
        model = AddTensorOp()
        traced = torch_frontend.prepare(model, list(input_pair), ["x", "y"])
        with pytest.raises(RuntimeError, match="AIR IR has not been generated"):
            traced.write_ir("output.air")

    def test_print_graph(self, torch_frontend, input_pair):
        """Test printing traced graph."""
        model = AddTensorOp()
        traced = torch_frontend.compile(model, list(input_pair), ["x", "y"])
        traced.print_graph()  # Should not raise

    def test_get_graph_code(self, torch_frontend, input_pair):
        """Test getting graph code."""
        model = AddTensorOp()
        traced = torch_frontend.compile(model, list(input_pair), ["x", "y"])
        code = traced.get_graph_code()
        assert code is not None
        assert len(code) > 0

    def test_call_equivalent_to_execute(self, torch_frontend, input_pair):
        """Test that __call__ is equivalent to execute()."""
        model = AddTensorOp()
        traced = torch_frontend.prepare(model, list(input_pair), ["x", "y"])
        assert not traced.is_air_generated()
        result = traced(*input_pair)
        assert traced.is_air_generated()


# =============================================================================
# Test: Binary Operators
# =============================================================================

@requires_frontend
class TestBinaryOperators:
    """Tests for binary operators and AIR generation."""

    @pytest.mark.parametrize("model_class,op_name", [
        (AddTensorOp, "add"),
        (SubTensorOp, "sub"),
        (MulTensorOp, "mul"),
    ])
    def test_binary_op_to_ir(self, torch_frontend, model_class, op_name, input_pair):
        """Test binary operator model to traced model."""
        model = model_class()
        traced = torch_frontend.compile(model, list(input_pair), ["x", "y"])
        assert traced is not None
        assert traced.is_air_generated()
        custom_ops = traced.get_custom_ops()
        assert op_name in str(custom_ops)


# =============================================================================
# Test: Unary Operators
# =============================================================================

@requires_frontend
class TestUnaryOperators:
    """Tests for unary operators and AIR generation."""

    def test_relu_to_ir(self, torch_frontend):
        """Test relu operator model to traced model."""
        model = ReLUTensorOp()
        x = torch.tensor([-1.0, 2.0, -3.0])
        traced = torch_frontend.compile(model, [x], ["x"])
        assert traced.is_air_generated()
        assert "relu" in str(traced.get_custom_ops())

    def test_sqrt_to_ir(self, torch_frontend):
        """Test sqrt operator model to traced model."""
        model = SqrtTensorOp()
        x = torch.tensor([1.0, 4.0, 9.0])
        traced = torch_frontend.compile(model, [x], ["x"])
        assert traced.is_air_generated()
        assert "sqrt" in str(traced.get_custom_ops())

    def test_flatten_to_ir(self, torch_frontend, input_4d):
        """Test flatten operator model to traced model."""
        model = FlattenTensorOp()
        traced = torch_frontend.compile(model, [input_4d], ["x"])
        assert traced.is_air_generated()
        assert "flatten" in str(traced.get_custom_ops())


# =============================================================================
# Test: Standard PyTorch Models (GraphTransform Path)
# =============================================================================

@requires_frontend
class TestStandardPyTorchModels:
    """Tests for standard PyTorch models through torch torch_frontend.

    Validates that GraphTransformPass correctly rewrites standard ops
    (nn.Linear, nn.Conv2d, F.relu, etc.) into custom ops for IR generation.
    """

    def test_add_module_compile(self, torch_frontend, input_pair):
        """Test AddOp (x + y) through GraphTransform."""
        model = AddOp()
        traced = torch_frontend.compile(model, list(input_pair), ["x", "y"])
        assert traced.is_air_generated()
        custom_ops = traced.get_custom_ops()
        assert "add" in str(custom_ops)

    def test_relu_module_compile(self, torch_frontend):
        """Test ReluOp through GraphTransform."""
        model = ReluOp()
        x = torch.randn(1, 4)
        traced = torch_frontend.compile(model, [x], ["x"])
        assert traced.is_air_generated()
        custom_ops = traced.get_custom_ops()
        assert "relu" in str(custom_ops)

    def test_linear_model_compile(self, torch_frontend):
        """Test LinearOp through GraphTransform (nn.Linear -> gemm)."""
        model = LinearOp(4, 2)
        x = torch.randn(1, 4)
        traced = torch_frontend.compile(model, [x], ["x"])
        assert traced.is_air_generated()
        custom_ops = traced.get_custom_ops()
        assert "gemm" in str(custom_ops) or "linear" in str(custom_ops).lower()

    def test_conv2d_relu_model_compile(self, torch_frontend):
        """Test Conv2dReluOp through GraphTransform."""
        model = Conv2dReluOp(3, 3, 3)
        x = torch.randn(1, 3, 8, 8)
        traced = torch_frontend.compile(model, [x], ["x"])
        assert traced.is_air_generated()
        custom_ops = traced.get_custom_ops()
        ops_str = str(custom_ops)
        assert "conv" in ops_str
        assert "relu" in ops_str

    def test_global_avg_pool_model_compile(self, torch_frontend):
        """Test GlobalAvgPool2dOp through GraphTransform."""
        model = GlobalAvgPool2dOp()
        x = torch.randn(1, 3, 8, 8)
        traced = torch_frontend.compile(model, [x], ["x"])
        assert traced.is_air_generated()
        custom_ops = traced.get_custom_ops()
        assert "pool" in str(custom_ops).lower() or "avg" in str(custom_ops).lower()

    def test_linear_model_export_air(self, torch_frontend, tmp_path):
        """Test LinearOp export to AIR file."""
        model = LinearOp(4, 2)
        x = torch.randn(1, 4)
        output_path = str(tmp_path / "linear.B")
        result_path = torch_frontend.export(model, [x], format="air",
                                       output_path=output_path, input_names=["x"])
        assert os.path.exists(result_path)

    def test_conv2d_relu_model_export_air(self, torch_frontend, tmp_path):
        """Test Conv2dReluOp export to AIR file."""
        model = Conv2dReluOp(3, 3, 3)
        x = torch.randn(1, 3, 8, 8)
        output_path = str(tmp_path / "conv_relu.B")
        result_path = torch_frontend.export(model, [x], format="air",
                                       output_path=output_path, input_names=["x"])
        assert os.path.exists(result_path)

    def test_graph_transform_rewrites_add(self, torch_frontend, input_pair):
        """Test that GraphTransform rewrites operator.add to torch.ops.tensor.add."""
        model = AddOp()
        traced = torch_frontend.prepare(model, list(input_pair), ["x", "y"])
        has_custom_add = False
        for node in traced.traced_model.graph.nodes:
            if node.op == "call_function":
                if "tensor" in str(node.target) and "add" in str(node.target):
                    has_custom_add = True
                    break
        assert has_custom_add, "GraphTransform did not rewrite operator.add"

    def test_graph_transform_rewrites_relu(self, torch_frontend):
        """Test that GraphTransform rewrites F.relu to torch.ops.tensor.relu."""
        model = ReluOp()
        x = torch.randn(1, 4)
        traced = torch_frontend.prepare(model, [x], ["x"])
        has_custom_relu = False
        for node in traced.traced_model.graph.nodes:
            if node.op == "call_function":
                if "tensor" in str(node.target) and "relu" in str(node.target):
                    has_custom_relu = True
                    break
        assert has_custom_relu, "GraphTransform did not rewrite F.relu"


# =============================================================================
# Test: Frontend Metadata
# =============================================================================

@requires_frontend
class TestFrontendMeta:
    """Tests for torch_frontend metadata."""

    def test_frontend_name(self, torch_frontend):
        """Test torch_frontend name."""
        assert torch_frontend.name() == "torch"


# =============================================================================
# Test: Backend Integration
# =============================================================================

@requires_frontend
class TestBackendIntegration:
    """Tests for backend integration."""

    def test_traced_model_can_be_exported(self, torch_frontend, input_pair, tmp_path):
        """Test that traced model can be exported for backend consumption."""
        model = AddTensorOp()
        traced = torch_frontend.compile(model, list(input_pair), ["x", "y"])
        output_path = str(tmp_path / "model.B")
        result = traced.export_ir(output_path)
        assert result is True
        assert os.path.exists(output_path)


# Run tests if executed directly
if __name__ == "__main__":
    pytest.main([__file__, "-v"])