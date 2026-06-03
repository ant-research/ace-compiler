# tests/regression/resnet/test_resnet_compile.py
"""
ResNet compilation pipeline tests.

Frontend coverage:
- torch: FX trace → AIR
- torch-via-onnx: PyTorch → ONNX → AIR
- onnx: ONNX file → AIR

Other:
- BN folding (cross-frontend)
- Export (AIR/ONNX)
"""
import pytest

from ace import fhe
from ace.fhe.frontend import get_frontend
from ace.fhe.ir import extract_ir_structure
from utils import requires_torch, TARGET_PARAMS


# ============================================================================
# Torch Frontend Tests
# ============================================================================

@requires_torch
class TestResNetFrontendTorch:
    """Test torch frontend (FX trace → AIR)."""

    def test_trace(self, resnet_case, data_regression):
        """Test IR structure after torch frontend tracing."""
        model = resnet_case.create_model()
        model.eval()

        frontend = get_frontend("torch")
        traced = frontend.prepare(model, list(resnet_case.example_inputs))

        structure = extract_ir_structure(traced)
        data_regression.check(structure)

    def test_bn_folded(self, resnet_case, data_regression):
        """Test IR structure after BN folding."""
        from ace.fhe.frontend.torch.passes.model_prepare import ModelPreparePass

        model = resnet_case.create_model()
        prepare_pass = ModelPreparePass()
        fused_model = prepare_pass.apply(model)

        frontend = get_frontend("torch")
        traced = frontend.prepare(fused_model, list(resnet_case.example_inputs))

        structure = extract_ir_structure(traced)
        data_regression.check(structure)

    @pytest.mark.parametrize("library,device", TARGET_PARAMS)
    def test_compile(self, resnet_case, library, device):
        """Test torch frontend compilation to backend."""
        import torch

        model = resnet_case.create_model()
        encrypt_inputs = resnet_case.encrypt_inputs
        example_inputs = tuple(
            torch.randn(inp.shape, dtype=inp.dtype) for inp in resnet_case.example_inputs
        )

        compiled_model = fhe.compile(
            frontend="torch",
            library=library,
            device=device,
            encrypt_inputs=encrypt_inputs
        )(model)

        prog = compiled_model.fhe_compile(example_inputs)

        assert prog is not None
        assert hasattr(prog, 'package')
        assert hasattr(prog, 'runtime')


# ============================================================================
# Torch-via-ONNX Frontend Tests
# ============================================================================

@requires_torch
class TestResNetFrontendTorchViaOnnx:
    """Test torch-via-onnx frontend (PyTorch → ONNX → AIR)."""

    def test_to_air(self, resnet_case):
        """Test converting ResNet model to AIR via ONNX."""
        model = resnet_case.create_model()
        model.eval()

        frontend = get_frontend("torch-via-onnx")
        air = frontend.to_ir(model, resnet_case.example_inputs)

        assert air is not None
        # ONNXFileIR has file_path, not nodes - just verify it's valid
        assert air.file_path is not None or hasattr(air, 'nodes')


# ============================================================================
# ONNX Frontend Tests (direct ONNX file loading)
# ============================================================================

@requires_torch
class TestResNetFrontendOnnx:
    """Test onnx frontend (ONNX file → AIR)."""

    def test_to_air(self, resnet_case, tmp_path):
        """Test loading ONNX file directly via onnx frontend."""
        import torch
        import warnings

        model = resnet_case.create_model()
        model.eval()

        # Export to ONNX file first
        onnx_path = tmp_path / "model.onnx"
        with warnings.catch_warnings():
            warnings.filterwarnings("ignore", category=FutureWarning,
                                    message=".*isinstance.*LeafSpec.*")
            warnings.filterwarnings("ignore", category=DeprecationWarning,
                                    message=".*legacy TorchScript-based ONNX export.*")
            warnings.filterwarnings("ignore", category=DeprecationWarning,
                                    message=".*feature will be removed.*")
            torch.onnx.export(
                model,
                resnet_case.example_inputs,
                str(onnx_path),
                input_names=["input"],
                output_names=["output"],
                opset_version=11,
            )

        # Use onnx frontend to load and convert to AIR
        frontend = get_frontend("onnx")
        air = frontend.to_ir(str(onnx_path))

        assert air is not None
        # Verify we got a valid IR object (ONNXFileIR or similar)
        assert hasattr(air, 'file_path') or hasattr(air, 'nodes')


# ============================================================================
# BN Folding Tests (cross-frontend)
# ============================================================================

@requires_torch
class TestResNetBNFolding:
    """Test BatchNorm folding (applies to torch-based frontends)."""

    def test_bn_folding(self, resnet_case):
        """Test that all BatchNorm layers are folded."""
        import torch.nn as nn
        from ace.fhe.frontend.torch.passes.model_prepare import ModelPreparePass

        model = resnet_case.create_model()

        bn_before = sum(1 for m in model.modules() if isinstance(m, nn.BatchNorm2d))
        prepare_pass = ModelPreparePass()
        fused_model = prepare_pass.apply(model)
        bn_after = sum(1 for m in fused_model.modules() if isinstance(m, nn.BatchNorm2d))
        identity_count = sum(1 for m in fused_model.modules() if isinstance(m, nn.Identity))

        assert bn_after == 0, f"Expected 0 BatchNorm modules after folding, got {bn_after}"
        assert identity_count > 0, "Expected some Identity modules (replaced BN)"

    def test_bn_folding_output_equivalence(self, resnet_case):
        """Test that fused model produces equivalent output."""
        import torch
        from ace.fhe.frontend.torch.passes.model_prepare import ModelPreparePass

        model = resnet_case.create_model()
        prepare_pass = ModelPreparePass()
        fused_model = prepare_pass.apply(model)

        x = resnet_case.example_inputs[0]
        with torch.no_grad():
            y_before = model(x)
            y_after = fused_model(x)

        max_diff = (y_before - y_after).abs().max().item()
        assert max_diff < 1e-4, f"Output difference too large: {max_diff}"


# ============================================================================
# Export Tests (AIR/ONNX)
# ============================================================================

@requires_torch
class TestResNetExport:
    """Test export functionality (AIR/ONNX)."""

    def test_export_air(self, resnet_case, tmp_path):
        """Test AIR IR export via fhe.export."""
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

    def test_export_onnx(self, resnet_case, tmp_path):
        """Test ONNX export: export, validate, infer, compare with PyTorch."""
        import torch
        import onnx
        import onnxruntime as ort
        import numpy as np
        import warnings

        model = resnet_case.create_model()
        model.eval()

        # Export to ONNX
        onnx_path = tmp_path / "model.onnx"
        with warnings.catch_warnings():
            warnings.filterwarnings("ignore", category=FutureWarning,
                                    message=".*isinstance.*LeafSpec.*")
            warnings.filterwarnings("ignore", category=DeprecationWarning,
                                    message=".*legacy TorchScript-based ONNX export.*")
            warnings.filterwarnings("ignore", category=DeprecationWarning,
                                    message=".*feature will be removed.*")
            torch.onnx.export(
                model,
                resnet_case.example_inputs,
                str(onnx_path),
                input_names=["input"],
                output_names=["output"],
                opset_version=11,
            )

        # Verify file exists and non-empty
        assert onnx_path.exists()
        assert onnx_path.stat().st_size > 0

        # Validate ONNX model
        onnx_model = onnx.load(str(onnx_path))
        onnx.checker.check_model(onnx_model)

        # Run inference with ONNX Runtime and compare with PyTorch
        session = ort.InferenceSession(str(onnx_path))
        input_name = session.get_inputs()[0].name
        input_data = resnet_case.example_inputs[0].numpy()
        onnx_output = session.run(None, {input_name: input_data})[0]

        with torch.no_grad():
            pytorch_output = model(*resnet_case.example_inputs)

        assert np.allclose(pytorch_output.numpy(), onnx_output, rtol=1e-4, atol=1e-5)
