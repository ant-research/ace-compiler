# test_regression/resnet/test_compile.py
"""
ResNet compilation pipeline tests.

Tests cover:
- BN Folding (BatchNorm folding into Conv)
- Torch frontend compilation (FX trace → AIR IR → backend)
- ONNX export and validation
- AIR IR generation
"""
import pytest

from ace import fhe
from test_utils import TORCH_FX_AVAILABLE


requires_torch_fx = pytest.mark.skipif(
    not TORCH_FX_AVAILABLE,
    reason="torch.fx not available"
)

BACKEND_PARAMS = [
    pytest.param("antlib", "cpu", id="antlib-cpu"),
    pytest.param("phantom", "cuda",
                 marks=pytest.mark.skipif(not fhe.gpu_available(), reason="GPU not available"),
                 id="phantom-cuda"),
    pytest.param("hyperfhe", "cuda",
                 marks=pytest.mark.skipif(not fhe.gpu_available(), reason="GPU not available"),
                 id="hyperfhe-cuda"),
]


# ============================================================================
# BN Folding Tests
# ============================================================================

@requires_torch_fx
class TestResNetBNFolding:
    """Test BatchNorm folding in ResNet models."""

    def test_bn_folding(self, resnet_case):
        """Test that all BatchNorm layers are folded."""
        import torch.nn as nn
        from ace.fhe.frontend.bn_folding import fuse_modules

        model = resnet_case.create_model()
        model.eval()

        bn_before = sum(1 for m in model.modules() if isinstance(m, nn.BatchNorm2d))
        fused_model = fuse_modules(model)
        bn_after = sum(1 for m in fused_model.modules() if isinstance(m, nn.BatchNorm2d))
        identity_count = sum(1 for m in fused_model.modules() if isinstance(m, nn.Identity))

        assert bn_after == 0, f"Expected 0 BatchNorm modules after folding, got {bn_after}"
        assert identity_count > 0, "Expected some Identity modules (replaced BN)"

    def test_bn_folding_output_equivalence(self, resnet_case):
        """Test that fused model produces equivalent output."""
        import torch
        from ace.fhe.frontend.bn_folding import fuse_modules

        model = resnet_case.create_model()
        model.eval()
        fused_model = fuse_modules(model)

        x = resnet_case.example_inputs[0]
        with torch.no_grad():
            y_before = model(x)
            y_after = fused_model(x)

        max_diff = (y_before - y_after).abs().max().item()
        assert max_diff < 1e-4, f"Output difference too large: {max_diff}"


# ============================================================================
# Torch Frontend Compilation Tests
# ============================================================================

@requires_torch_fx
@pytest.mark.slow
@pytest.mark.parametrize("backend,device", BACKEND_PARAMS)
class TestCompileResNet:
    """Test ResNet compilation with torch frontend."""

    def test_compile_success(self, resnet_case, model_inputs, backend, device):
        """Test ResNet model compilation through torch path."""
        model = resnet_case.create_model()
        encrypt_inputs = resnet_case.encrypt_inputs

        compiled_model = fhe.compile(
            frontend="torch",
            library=backend,
            device=device,
            encrypt_inputs=encrypt_inputs
        )(model)

        prog = compiled_model.compile(model_inputs)

        assert prog is not None
        assert "kernel" in prog
        assert "model" in prog

    def test_compile_expected_ops(self, resnet_case, backend, device):
        """Test that compiled IR contains expected operations after BN folding."""
        import torch
        from ace.fhe.frontend import get_frontend

        model = resnet_case.create_model()
        model.eval()

        frontend = get_frontend("torch")
        traced = frontend.prepare(model, [resnet_case.example_inputs[0]])

        op_types_found = set()
        for name, module in traced.traced_model.named_modules():
            if isinstance(module, (torch.nn.Conv2d, torch.nn.ReLU, torch.nn.Linear)):
                op_types_found.add(type(module).__name__)
            elif isinstance(module, torch.nn.AdaptiveAvgPool2d):
                op_types_found.add('AdaptiveAvgPool2d')

        op_mapping = {
            'Conv': 'Conv2d',
            'Relu': 'ReLU',
            'Gemm': 'Linear',
            'AvgPool': 'AdaptiveAvgPool2d',
            'Add': 'add',
        }

        for expected_op in resnet_case.expected_ops:
            mapped_op = op_mapping.get(expected_op, expected_op)
            if mapped_op == 'add':
                has_add = False
                for node in traced.traced_model.graph.nodes:
                    if node.op == 'call_function' and 'add' in str(node.target).lower():
                        has_add = True
                        break
                assert has_add, "Expected 'add' operation not found in graph"
            else:
                assert mapped_op in op_types_found, \
                    f"Expected op '{expected_op}' (mapped to '{mapped_op}') not found. Found: {op_types_found}"


# ============================================================================
# AIR IR Generation Tests
# ============================================================================

@requires_torch_fx
@pytest.mark.slow
class TestResNetAIRGeneration:
    """Test AIR IR generation for ResNet models."""

    def test_air_ir_generation(self, resnet_case, tmp_path):
        """Test that AIR IR is generated correctly using fhe.export."""
        import os
        model = resnet_case.create_model()
        model_inputs = resnet_case.example_inputs
        encrypt_inputs = resnet_case.encrypt_inputs

        export_model = fhe.export(
            frontend="torch",
            library="antlib",
            device="cpu",
            encrypt_inputs=encrypt_inputs
        )(model)

        output_path = str(tmp_path / "model.B")
        result = export_model.export(model_inputs, output_path=output_path, format="air")

        assert result is not None
        assert isinstance(result, str)
        assert os.path.exists(result), f"AIR file not found: {result}"
        assert os.path.getsize(result) > 0, "AIR file is empty"

    def test_export_success(self, resnet_case, tmp_path):
        """Test model frontend IR export."""
        import os
        model = resnet_case.create_model()
        model_inputs = resnet_case.example_inputs
        encrypt_inputs = resnet_case.encrypt_inputs

        export_model = fhe.export(
            frontend="torch",
            library="antlib",
            device="cpu",
            encrypt_inputs=encrypt_inputs
        )(model)

        output_path = str(tmp_path / "model.B")
        result = export_model.export(model_inputs, output_path=output_path, format="air")

        assert result is not None
        assert isinstance(result, str)
        assert os.path.exists(result), f"Exported file not found: {result}"


# ============================================================================
# ONNX Export Tests
# ============================================================================

@requires_torch_fx
@pytest.mark.resnet
@pytest.mark.model
class TestResNetFrontendOnnx:
    """Tests for ResNet model ONNX export and validation."""

    def test_model_to_air_via_onnx(self, resnet_case):
        """Test converting ResNet model to AIR via ONNX."""
        from ace.fhe.frontend import get_frontend

        model = resnet_case.create_model()
        model.eval()

        frontend = get_frontend("torch-via-onnx")
        air = frontend.to_ir(model, resnet_case.example_inputs)

        assert air is not None
        assert len(air.nodes) > 0

    def test_model_onnx_export(self, resnet_case, tmp_path):
        """Test exporting ResNet model to ONNX file."""
        import torch

        model = resnet_case.create_model()
        model.eval()

        onnx_path = tmp_path / "resnet20.onnx"
        torch.onnx.export(
            model,
            resnet_case.example_inputs,
            str(onnx_path),
            input_names=["input"],
            output_names=["output"],
            opset_version=11,
        )

        assert onnx_path.exists()
        assert onnx_path.stat().st_size > 0

    def test_model_onnx_load_and_infer(self, resnet_case, tmp_path):
        """Test that exported ONNX model can be loaded and run inference."""
        import torch
        import onnx
        import onnxruntime as ort

        model = resnet_case.create_model()
        model.eval()

        onnx_path = tmp_path / "resnet20.onnx"
        torch.onnx.export(
            model,
            resnet_case.example_inputs,
            str(onnx_path),
            input_names=["input"],
            output_names=["output"],
            opset_version=11,
        )

        onnx_model = onnx.load(str(onnx_path))
        onnx.checker.check_model(onnx_model)

        session = ort.InferenceSession(str(onnx_path))
        input_name = session.get_inputs()[0].name

        import numpy as np
        input_data = resnet_case.example_inputs[0].numpy()
        outputs = session.run(None, {input_name: input_data})

        assert len(outputs) == 1
        assert outputs[0].shape == (1, 10)

    def test_model_onnx_matches_pytorch(self, resnet_case, tmp_path):
        """Test that ONNX model output matches PyTorch model output."""
        import torch
        import onnxruntime as ort
        import numpy as np

        model = resnet_case.create_model()
        model.eval()

        with torch.no_grad():
            pytorch_output = model(*resnet_case.example_inputs)

        onnx_path = tmp_path / "resnet20.onnx"
        torch.onnx.export(
            model,
            resnet_case.example_inputs,
            str(onnx_path),
            input_names=["input"],
            output_names=["output"],
            opset_version=11,
        )

        session = ort.InferenceSession(str(onnx_path))
        input_name = session.get_inputs()[0].name
        input_data = resnet_case.example_inputs[0].numpy()
        onnx_output = session.run(None, {input_name: input_data})[0]

        assert np.allclose(pytorch_output.numpy(), onnx_output, rtol=1e-4, atol=1e-5)