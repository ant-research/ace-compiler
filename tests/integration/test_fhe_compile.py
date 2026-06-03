# tests/integration/test_fhe_compile.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Integration tests for fhe.compile() top-level API.

Tests the functional form: fhe.compile(...)(target) → .fhe_compile(inputs) → CompiledProgram
"""

import pytest
import torch

from ace import fhe
from ace.sample.ops.specs import LINEAR_OP, ADD_OP
from ace.sample.funcs.specs import ADD_FUNC
from utils import HAS_TORCH_FX, HAS_FRONTEND, TARGET_PARAMS

_OP_SPECS = [LINEAR_OP, ADD_OP]
_FUNC_SPECS = [ADD_FUNC]

# Only specs whose model_class supports no-arg construction
_NO_ARG_MODEL_SPECS = [ADD_OP]


def _provider_available(name, device):
    try:
        from ace.fhe.backend import get_library_impl
        pro = get_library_impl(name, device=device)
        return pro.check_available()
    except Exception:
        return False


# =============================================================================
# fhe.compile on nn.Module (torch frontend)
# =============================================================================

@pytest.mark.skipif(not HAS_TORCH_FX or not HAS_FRONTEND, reason="torch.fx or frontend not available")
class TestCompileModel:
    """Tests for fhe.compile() on nn.Module models."""

    @pytest.mark.parametrize("name,device", TARGET_PARAMS)
    @pytest.mark.parametrize("spec", _NO_ARG_MODEL_SPECS, ids=lambda s: s.name)
    def test_compile_model_class(self, name, device, spec):
        """fhe.compile()(ModelClass) attaches fhe_compile."""
        if not _provider_available(name, device):
            pytest.skip(f"{name}/{device} compiler not available")

        compiled = fhe.compile(
            frontend="torch", library=name, device=device,
            encrypt_inputs=spec.encrypt_inputs,
        )(spec.model_class)

        assert hasattr(compiled, 'fhe_compile')
        assert hasattr(compiled, 'compile')

    @pytest.mark.parametrize("name,device", TARGET_PARAMS)
    def test_compile_model_instance(self, name, device):
        """fhe.compile()(model_instance) attaches fhe_compile."""
        if not _provider_available(name, device):
            pytest.skip(f"{name}/{device} compiler not available")

        model = ADD_OP.create_model()
        compiled = fhe.compile(
            frontend="torch", library=name, device=device,
            encrypt_inputs=ADD_OP.encrypt_inputs,
        )(model)

        assert hasattr(compiled, 'fhe_compile')


# =============================================================================
# fhe.compile on function (ast frontend)
# =============================================================================

@pytest.mark.skipif(not HAS_FRONTEND, reason="frontend not available")
class TestCompileFunction:
    """Tests for fhe.compile() on Python functions."""

    @pytest.mark.parametrize("name,device", TARGET_PARAMS)
    @pytest.mark.parametrize("spec", _FUNC_SPECS, ids=lambda s: s.name)
    def test_compile_function(self, name, device, spec):
        """fhe.compile()(func) attaches compile method."""
        if not _provider_available(name, device):
            pytest.skip(f"{name}/{device} compiler not available")

        compiled = fhe.compile(
            frontend="ast", library=name, device=device,
            encrypt_inputs=spec.encrypt_inputs,
        )(spec.func)

        assert hasattr(compiled, 'compile')
        assert hasattr(compiled, '_fhe_compiler')


# =============================================================================
# fhe.compile with torch-via-onnx frontend
# =============================================================================

class TestCompileTorchViaOnnx:
    """Tests for fhe.compile() with torch-via-onnx frontend."""

    @pytest.mark.parametrize("name,device", TARGET_PARAMS)
    @pytest.mark.parametrize("spec", _OP_SPECS, ids=lambda s: s.name)
    def test_compile_model(self, name, device, spec):
        """fhe.compile with torch-via-onnx on model."""
        if not _provider_available(name, device):
            pytest.skip(f"{name}/{device} compiler not available")

        model = spec.create_model()
        compiled = fhe.compile(
            frontend="torch-via-onnx", library=name, device=device,
            encrypt_inputs=spec.encrypt_inputs,
        )(model)

        assert hasattr(compiled, 'fhe_compile')


# =============================================================================
# Error handling
# =============================================================================

class TestCompileErrors:
    """Tests for fhe.compile() error handling."""

    def test_invalid_frontend_raises(self):
        with pytest.raises(ValueError, match="Unknown frontend"):
            fhe.compile(frontend="invalid", library="antlib")

    def test_invalid_library_raises(self):
        with pytest.raises(ValueError, match="Unknown library"):
            fhe.compile(frontend="torch", library="invalid")