# tests/unit/driver/test_pipeline.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Unit tests for Driver compilation pipeline.

Tests the Driver's handling of different frontend → IR → backend compilation
paths with implemented backends. Skipped automatically when compiler toolchain
is not available.
"""

import pytest
import torch

from ace.fhe.driver import Driver
from ace.fhe.backend import get_library_impl
from ace.fhe.ir import ONNXFileIR, AIRFileIR, convert_onnx_to_air
from ace.fhe.frontend.torch import Torch
from ace.fhe.frontend.onnx import Onnx
from ace.sample.ops.specs import LINEAR_OP, ADD_OP
from ace.sample.funcs.specs import ADD_FUNC

from utils import (
    HAS_TORCH_FX,
    HAS_FRONTEND,
    TARGET_PARAMS,
)

# Representative specs for compilation tests
_OP_SPECS = [LINEAR_OP, ADD_OP]
_FUNC_SPECS = [ADD_FUNC]


def _provider_available(name, device):
    """Check if a backend's compiler toolchain is available."""
    try:
        pro = get_library_impl(name, device=device)
        return pro.check_available()
    except Exception:
        return False


def _export_onnx(spec, tmp_path):
    """Helper: create an ONNX file from a ModelSpec."""
    import warnings
    model = spec.create_model()
    model.eval()
    onnx_path = str(tmp_path / f"{spec.name}.onnx")
    with warnings.catch_warnings():
        warnings.filterwarnings("ignore", category=FutureWarning,
                                message=".*isinstance.*LeafSpec.*")
        warnings.filterwarnings("ignore", category=DeprecationWarning,
                                message=".*legacy TorchScript-based ONNX export.*")
        warnings.filterwarnings("ignore", category=DeprecationWarning,
                                message=".*feature will be removed.*")
        torch.onnx.export(
            model, tuple(spec.example_inputs), onnx_path,
            input_names=spec.encrypt_inputs, output_names=["output"], opset_version=14
        )
    return onnx_path


# =============================================================================
# Torch Frontend
# =============================================================================

@pytest.mark.skipif(not HAS_TORCH_FX or not HAS_FRONTEND, reason="torch.fx or frontend not available")
class TestTorchCompile:
    """Tests for torch frontend compilation: FX trace → AIR."""

    @pytest.mark.parametrize("name,device", TARGET_PARAMS)
    @pytest.mark.parametrize("spec", _OP_SPECS, ids=lambda s: s.name)
    def test_air_path(self, tmp_path, name, device, spec):
        """Torch → FX trace → AIR file (.B) → backend build."""
        if not _provider_available(name, device):
            pytest.skip(f"{name}/{device} compiler not available")

        model = spec.create_model()
        air_file = tmp_path / f"{spec.name}.B"

        frontend = Torch()
        traced = frontend.compile(model, list(spec.example_inputs), spec.encrypt_inputs)
        traced.export_ir(str(air_file))
        assert air_file.exists()

        compiler = Driver(frontend="torch", library=name, device=device)
        compiler.backend_impl.build_dir = tmp_path
        result = compiler.backend_impl.build(traced)
        assert result is not None


# =============================================================================
# Torch-via-ONNX Frontend
# =============================================================================

class TestTorchViaOnnxCompile:
    """Tests for torch-via-onnx frontend compilation paths."""

    @pytest.mark.parametrize("name,device", TARGET_PARAMS)
    @pytest.mark.parametrize("spec", _OP_SPECS, ids=lambda s: s.name)
    def test_onnx_path(self, tmp_path, name, device, spec):
        """Torch-via-ONNX → ONNX file → backend build."""
        if not _provider_available(name, device):
            pytest.skip(f"{name}/{device} compiler not available")

        onnx_path = _export_onnx(spec, tmp_path)

        compiler = Driver(frontend="torch-via-onnx", library=name, device=device)
        onnx_model = ONNXFileIR(onnx_path)
        compiler.backend_impl.build_dir = tmp_path
        result = compiler.backend_impl.build(onnx_model)
        assert result is not None

    @pytest.mark.parametrize("name,device", TARGET_PARAMS)
    @pytest.mark.parametrize("spec", _OP_SPECS, ids=lambda s: s.name)
    def test_air_path(self, tmp_path, name, device, spec):
        """Torch-via-ONNX → AIR file (.B) → backend build."""
        if not _provider_available(name, device):
            pytest.skip(f"{name}/{device} compiler not available")

        onnx_path = _export_onnx(spec, tmp_path)
        air_file = tmp_path / f"{spec.name}.B"
        convert_onnx_to_air(onnx_path, str(air_file))
        assert air_file.exists()

        compiler = Driver(frontend="torch-via-onnx", library=name, device=device)
        air_model = AIRFileIR(str(air_file))
        compiler.backend_impl.build_dir = tmp_path
        result = compiler.backend_impl.build(air_model)
        assert result is not None


# =============================================================================
# AST Frontend
# =============================================================================

@pytest.mark.skipif(not HAS_FRONTEND, reason="frontend not available")
class TestASTCompile:
    """Tests for AST frontend compilation paths."""

    @pytest.mark.parametrize("name,device", TARGET_PARAMS)
    @pytest.mark.parametrize("spec", _FUNC_SPECS, ids=lambda s: s.name)
    def test_compile(self, tmp_path, name, device, spec):
        """AST frontend compilation."""
        if not _provider_available(name, device):
            pytest.skip(f"{name}/{device} compiler not available")

        compiler = Driver(frontend="ast", library=name, device=device)
        result = compiler.compile(spec.func, list(spec.example_inputs), spec.encrypt_inputs)
        assert result is not None


# =============================================================================
# ONNX Frontend
# =============================================================================

class TestOnnxCompile:
    """Tests for ONNX frontend compilation paths."""

    @pytest.mark.parametrize("name,device", TARGET_PARAMS)
    @pytest.mark.parametrize("spec", _OP_SPECS, ids=lambda s: s.name)
    def test_onnx_path(self, tmp_path, name, device, spec):
        """ONNX file → ONNXFileIR → backend build."""
        if not _provider_available(name, device):
            pytest.skip(f"{name}/{device} compiler not available")

        onnx_path = _export_onnx(spec, tmp_path)

        compiler = Driver(frontend="onnx", library=name, device=device)
        frontend = Onnx()
        onnx_model = frontend.prepare(onnx_path)
        assert onnx_model.format_type == "file"
        assert onnx_model.file_format == "onnx"
        compiler.backend_impl.build_dir = tmp_path
        result = compiler.backend_impl.build(onnx_model)
        assert result is not None

    @pytest.mark.parametrize("name,device", TARGET_PARAMS)
    @pytest.mark.parametrize("spec", _OP_SPECS, ids=lambda s: s.name)
    def test_air_path(self, tmp_path, name, device, spec):
        """ONNX file → AIR file (.B) → backend build."""
        if not _provider_available(name, device):
            pytest.skip(f"{name}/{device} compiler not available")

        onnx_path = _export_onnx(spec, tmp_path)
        air_file = tmp_path / f"{spec.name}.B"

        frontend = Onnx()
        frontend.export(onnx_path, format="air", output_path=str(air_file))
        assert air_file.exists()

        air_model = AIRFileIR(str(air_file))
        assert air_model.format_type == "file"
        assert air_model.file_format == "air"

        compiler = Driver(frontend="onnx", library=name, device=device)
        compiler.backend_impl.build_dir = tmp_path
        result = compiler.backend_impl.build(air_model)
        assert result is not None


# =============================================================================
# Driver Error Handling
# =============================================================================

class TestDriverErrors:
    """Tests for Driver error handling."""

    def test_invalid_frontend_raises(self):
        with pytest.raises(ValueError, match="Unknown frontend"):
            Driver(frontend="invalid_frontend", library="antlib")

    def test_invalid_backend_raises(self):
        with pytest.raises(ValueError):
            Driver(frontend="torch-via-onnx", library="invalid_backend")