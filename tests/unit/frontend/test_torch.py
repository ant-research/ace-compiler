# tests/unit/frontend/test_torch.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Unit tests for Torch frontend.

Tests two output modes:
1. Memory: prepare() -> TorchTracedModel -> compile() -> AIR IR in memory
2. AIR file: export(format="air") -> .B file

Note: Torch frontend is UNIQUE - it's the only frontend where memory mode works!
It uses FX tracing + custom C++ operators (via IRBuilder) to generate AIR IR in memory.
"""
import pytest
import os

from utils import HAS_TORCH_FX, HAS_FRONTEND

from ace.fhe.ir import TorchTracedModel


from ace.sample.ops.specs import ADD_OP, RELU_OP, LINEAR_OP, CONV2D_RELU_OP, GLOBAL_AVG_POOL_OP
from ace.sample.tensor_ops import (
    ADD_TENSOR_OP,
    SUB_TENSOR_OP,
    MUL_TENSOR_OP,
    COMPOSITE_TENSOR_OP,
    RELU_TENSOR_OP,
    SQRT_TENSOR_OP,
    FLATTEN_TENSOR_OP,
)


# Skip markers
requires_frontend = pytest.mark.skipif(
    not HAS_TORCH_FX or not HAS_FRONTEND,
    reason="torch.fx or C++ extension not available"
)

# All specs for parametrized tests (custom C++ ops + standard PyTorch models)
_OP_SPECS = [
    # Standard PyTorch models (GraphTransform path)
    ADD_OP, RELU_OP, LINEAR_OP, CONV2D_RELU_OP, GLOBAL_AVG_POOL_OP,
    # Tensor ops (FX trace with custom C++ operators)
    ADD_TENSOR_OP, SUB_TENSOR_OP, MUL_TENSOR_OP, RELU_TENSOR_OP,
    SQRT_TENSOR_OP, FLATTEN_TENSOR_OP, COMPOSITE_TENSOR_OP,
]



# =============================================================================
# Test: prepare() - Memory Mode
# =============================================================================

@requires_frontend
class TestPrepare:
    """Tests for prepare() method - Memory mode."""

    @pytest.mark.parametrize("spec", _OP_SPECS, ids=lambda s: s.name)
    def test_returns_torch_traced_model(self, torch_frontend, spec):
        model = spec.create_model()
        result = torch_frontend.prepare(model, list(spec.example_inputs), spec.encrypt_inputs)
        assert isinstance(result, TorchTracedModel)

    @pytest.mark.parametrize("spec", _OP_SPECS, ids=lambda s: s.name)
    def test_memory_mode_properties(self, torch_frontend, spec):
        """prepare() returns memory-mode result: no file, no AIR generated."""
        model = spec.create_model()
        result = torch_frontend.prepare(model, list(spec.example_inputs), spec.encrypt_inputs)
        assert result.format_type == "memory"
        assert result.file_format is None
        assert result.file_path is None
        assert not result.is_air_generated()

    @pytest.mark.parametrize("spec", _OP_SPECS, ids=lambda s: s.name)
    def test_traced_model_has_graph(self, torch_frontend, spec):
        model = spec.create_model()
        result = torch_frontend.prepare(model, list(spec.example_inputs), spec.encrypt_inputs)
        assert result.traced_model is not None
        code = result.get_graph_code()
        assert len(code) > 0

    def test_auto_generate_input_names(self, torch_frontend):
        """Test that input names are auto-generated when not provided."""
        model = ADD_TENSOR_OP.create_model()
        result = torch_frontend.prepare(model, list(ADD_TENSOR_OP.example_inputs))
        assert result is not None


# =============================================================================
# Test: compile() - Memory Mode (Executed)
# =============================================================================

@requires_frontend
class TestCompile:
    """Tests for compile() method - Memory mode (executed)."""

    @pytest.mark.parametrize("spec", _OP_SPECS, ids=lambda s: s.name)
    def test_returns_torch_traced_model(self, torch_frontend, spec):
        model = spec.create_model()
        result = torch_frontend.compile(model, list(spec.example_inputs), spec.encrypt_inputs)
        assert isinstance(result, TorchTracedModel)

    @pytest.mark.parametrize("spec", _OP_SPECS, ids=lambda s: s.name)
    def test_format_type_is_file(self, torch_frontend, spec, tmp_path):
        model = spec.create_model()
        result = torch_frontend.compile(model, list(spec.example_inputs), spec.encrypt_inputs, build_dir=str(tmp_path))
        assert result.format_type == "file"

    @pytest.mark.parametrize("spec", _OP_SPECS, ids=lambda s: s.name)
    def test_air_is_generated(self, torch_frontend, spec, tmp_path):
        model = spec.create_model()
        result = torch_frontend.compile(model, list(spec.example_inputs), spec.encrypt_inputs, build_dir=str(tmp_path))
        assert result.is_air_generated()


# =============================================================================
# Test: export(format="air")
# =============================================================================

@requires_frontend
class TestExportAir:
    """Tests for export(format="air") method."""

    @pytest.mark.parametrize("spec", _OP_SPECS, ids=lambda s: s.name)
    def test_creates_air_file(self, torch_frontend, spec, tmp_path):
        model = spec.create_model()
        output_path = str(tmp_path / "output.B")
        result_path = torch_frontend.export(model, list(spec.example_inputs),
                                       format="air", output_path=output_path,
                                       input_names=spec.encrypt_inputs)
        assert result_path == output_path
        assert os.path.exists(output_path)

    def test_export_ir_properties(self, torch_frontend, tmp_path):
        """After export_ir: format_type='file', file_format='air', file_path set."""
        model = ADD_TENSOR_OP.create_model()
        inputs = list(ADD_TENSOR_OP.example_inputs)
        output_path = str(tmp_path / "output.B")
        traced = torch_frontend.prepare(model, inputs, ADD_TENSOR_OP.encrypt_inputs)
        assert traced.format_type == "memory"
        traced.execute(*inputs)
        traced.export_ir(output_path)
        assert traced.format_type == "file"
        assert traced.file_format == "air"
        assert traced.file_path == output_path


# =============================================================================
# Test: IR Properties
# =============================================================================

@requires_frontend
class TestIRProperties:
    """Tests for IR object properties."""

    @pytest.mark.parametrize("spec", _OP_SPECS, ids=lambda s: s.name)
    def test_entry_name_exists(self, torch_frontend, spec):
        model = spec.create_model()
        result = torch_frontend.compile(model, list(spec.example_inputs), spec.encrypt_inputs)
        assert hasattr(result, "entry_name")
        assert result.entry_name

    @pytest.mark.parametrize("spec", _OP_SPECS, ids=lambda s: s.name)
    def test_get_air_scopes(self, torch_frontend, spec):
        model = spec.create_model()
        result = torch_frontend.compile(model, list(spec.example_inputs), spec.encrypt_inputs)
        func_scope = result.get_air_func_scope()
        glob_scope = result.get_air_glob_scope()
        assert func_scope is not None or glob_scope is not None


# =============================================================================
# Test: TorchTracedModel Methods
# =============================================================================

@requires_frontend
class TestTorchTracedModelMethods:
    """Tests for TorchTracedModel helper methods."""

    @pytest.fixture(autouse=True)
    def setup_spec(self):
        self.spec = ADD_TENSOR_OP
        self.model = self.spec.create_model()
        self.inputs = list(self.spec.example_inputs)
        self.encrypt_inputs = self.spec.encrypt_inputs

    def test_write_ir_success(self, torch_frontend, tmp_path):
        traced = torch_frontend.compile(self.model, self.inputs, self.encrypt_inputs)
        output_file = str(tmp_path / "test_output.air")
        result = traced.write_ir(output_file)
        assert result is True

    def test_write_ir_before_execute_raises(self, torch_frontend):
        traced = torch_frontend.prepare(self.model, self.inputs, self.encrypt_inputs)
        with pytest.raises(RuntimeError, match="AIR IR has not been generated"):
            traced.write_ir("output.air")

    def test_print_graph(self, torch_frontend):
        traced = torch_frontend.compile(self.model, self.inputs, self.encrypt_inputs)
        traced.print_graph()  # Should not raise

    def test_get_graph_code(self, torch_frontend):
        traced = torch_frontend.compile(self.model, self.inputs, self.encrypt_inputs)
        code = traced.get_graph_code()
        assert code is not None
        assert len(code) > 0

    def test_call_equivalent_to_execute(self, torch_frontend):
        traced = torch_frontend.prepare(self.model, self.inputs, self.encrypt_inputs)
        assert not traced.is_air_generated()
        result = traced(*self.inputs)
        assert traced.is_air_generated()

    def test_export_ir_for_backend(self, torch_frontend, tmp_path):
        """Traced model can be exported for backend consumption."""
        traced = torch_frontend.compile(self.model, self.inputs, self.encrypt_inputs)
        output_path = str(tmp_path / "model.B")
        result = traced.export_ir(output_path)
        assert result is True
        assert os.path.exists(output_path)


# =============================================================================
# Test: Standard PyTorch Models (GraphTransform Path)
# =============================================================================

@requires_frontend
class TestStandardPyTorchModels:
    """Tests for standard PyTorch models through torch frontend.

    Validates that GraphTransformPass correctly rewrites standard ops
    (nn.Linear, nn.Conv2d, F.relu, etc.) into custom ops for IR generation.
    """

    def test_add_module_compile(self, torch_frontend):
        """Test AddOp (x + y) through GraphTransform."""
        model = ADD_OP.create_model()
        traced = torch_frontend.compile(model, list(ADD_OP.example_inputs), ADD_OP.encrypt_inputs)
        assert traced.is_air_generated()
        custom_ops = traced.get_custom_ops()
        assert "add" in str(custom_ops)

    def test_relu_module_compile(self, torch_frontend):
        """Test ReluOp through GraphTransform."""
        model = RELU_OP.create_model()
        traced = torch_frontend.compile(model, list(RELU_OP.example_inputs), RELU_OP.encrypt_inputs)
        assert traced.is_air_generated()
        custom_ops = traced.get_custom_ops()
        assert "relu" in str(custom_ops)

    def test_linear_model_compile(self, torch_frontend):
        """Test LinearOp through GraphTransform (nn.Linear -> gemm)."""
        model = LINEAR_OP.create_model()
        traced = torch_frontend.compile(model, list(LINEAR_OP.example_inputs), LINEAR_OP.encrypt_inputs)
        assert traced.is_air_generated()
        custom_ops = traced.get_custom_ops()
        assert "gemm" in str(custom_ops) or "linear" in str(custom_ops).lower()

    def test_conv2d_relu_model_compile(self, torch_frontend):
        """Test Conv2dReluOp through GraphTransform."""
        model = CONV2D_RELU_OP.create_model()
        traced = torch_frontend.compile(model, list(CONV2D_RELU_OP.example_inputs), CONV2D_RELU_OP.encrypt_inputs)
        assert traced.is_air_generated()
        custom_ops = traced.get_custom_ops()
        ops_str = str(custom_ops)
        assert "conv" in ops_str
        assert "relu" in ops_str

    def test_global_avg_pool_model_compile(self, torch_frontend):
        """Test GlobalAvgPool2dOp through GraphTransform."""
        model = GLOBAL_AVG_POOL_OP.create_model()
        traced = torch_frontend.compile(model, list(GLOBAL_AVG_POOL_OP.example_inputs), GLOBAL_AVG_POOL_OP.encrypt_inputs)
        assert traced.is_air_generated()
        custom_ops = traced.get_custom_ops()
        assert "pool" in str(custom_ops).lower() or "avg" in str(custom_ops).lower()

    def test_linear_model_export_air(self, torch_frontend, tmp_path):
        """Test LinearOp export to AIR file."""
        model = LINEAR_OP.create_model()
        output_path = str(tmp_path / "linear.B")
        result_path = torch_frontend.export(model, list(LINEAR_OP.example_inputs), format="air",
                                       output_path=output_path, input_names=LINEAR_OP.encrypt_inputs)
        assert os.path.exists(result_path)

    def test_conv2d_relu_model_export_air(self, torch_frontend, tmp_path):
        """Test Conv2dReluOp export to AIR file."""
        model = CONV2D_RELU_OP.create_model()
        output_path = str(tmp_path / "conv_relu.B")
        result_path = torch_frontend.export(model, list(CONV2D_RELU_OP.example_inputs), format="air",
                                       output_path=output_path, input_names=CONV2D_RELU_OP.encrypt_inputs)
        assert os.path.exists(result_path)

    def test_graph_transform_rewrites_add(self, torch_frontend):
        """Test that GraphTransform rewrites operator.add to torch.ops.tensor.add."""
        model = ADD_OP.create_model()
        traced = torch_frontend.prepare(model, list(ADD_OP.example_inputs), ADD_OP.encrypt_inputs)
        has_custom_add = False
        for node in traced.traced_model.graph.nodes:
            if node.op == "call_function":
                if "tensor" in str(node.target) and "add" in str(node.target):
                    has_custom_add = True
                    break
        assert has_custom_add, "GraphTransform did not rewrite operator.add"

    def test_graph_transform_rewrites_relu(self, torch_frontend):
        """Test that GraphTransform rewrites F.relu to torch.ops.tensor.relu."""
        model = RELU_OP.create_model()
        traced = torch_frontend.prepare(model, list(RELU_OP.example_inputs), RELU_OP.encrypt_inputs)
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
    """Tests for frontend metadata."""

    def test_frontend_name(self, torch_frontend):
        """Test frontend name."""
        assert torch_frontend.name() == "torch"
