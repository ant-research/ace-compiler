# tests/integration/test_fhe_compute.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Integration tests for fhe.compute() top-level API.

Tests the functional form: fhe.compute(...)(target) → call with inputs → FHE result
"""

import pytest
import torch
from utils import HAS_FRONTEND, HAS_TORCH_FX, TARGET_PARAMS

from ace import fhe
from ace.sample.funcs.specs import ADD_FUNC
from ace.sample.ops.specs import ADD_OP


def _provider_available(name, device):
    try:
        from ace.fhe.backend import get_library_impl

        pro = get_library_impl(name, device=device)
        return pro.check_available()
    except Exception:
        return False


# =============================================================================
# fhe.compute on nn.Module
# =============================================================================


@pytest.mark.skipif(not HAS_TORCH_FX or not HAS_FRONTEND, reason="torch.fx or frontend not available")
class TestComputeModel:
    """Tests for fhe.compute() on nn.Module models."""

    @pytest.mark.parametrize("name,device", TARGET_PARAMS)
    def test_torch(self, name, device):
        """fhe.compute()(model) with torch frontend."""
        if not _provider_available(name, device):
            pytest.skip(f"{name}/{device} compiler not available")

        model = ADD_OP.create_model()
        inputs = tuple(t.clone() for t in ADD_OP.example_inputs)

        compiled = fhe.compute(
            frontend="torch",
            library=name,
            device=device,
            encrypt_inputs=ADD_OP.encrypt_inputs,
        )(model)

        result = compiled(*inputs)
        assert result is not None

    @pytest.mark.skip(reason="torch-via-onnx produces zero shapes in DATA_SCHEME, causing runtime segfault")
    @pytest.mark.parametrize("name,device", TARGET_PARAMS)
    def test_torch_via_onnx(self, name, device):
        """fhe.compute()(model) with torch-via-onnx frontend (skipped: shape bug)."""
        if not _provider_available(name, device):
            pytest.skip(f"{name}/{device} compiler not available")

        model = ADD_OP.create_model()
        inputs = tuple(t.clone() for t in ADD_OP.example_inputs)

        compiled = fhe.compute(
            frontend="torch-via-onnx",
            library=name,
            device=device,
            encrypt_inputs=ADD_OP.encrypt_inputs,
        )(model)

        result = compiled(*inputs)
        assert result is not None


# =============================================================================
# fhe.compute on function
# =============================================================================


@pytest.mark.skipif(not HAS_FRONTEND, reason="frontend not available")
class TestComputeFunction:
    """Tests for fhe.compute() on Python functions."""

    @pytest.mark.parametrize("name,device", TARGET_PARAMS)
    def test_ast(self, name, device):
        """fhe.compute()(func) with ast frontend."""
        if not _provider_available(name, device):
            pytest.skip(f"{name}/{device} compiler not available")

        inputs = tuple(t.clone() for t in ADD_FUNC.example_inputs)

        compiled = fhe.compute(
            frontend="ast",
            library=name,
            device=device,
            encrypt_inputs=ADD_FUNC.encrypt_inputs,
        )(ADD_FUNC.func)

        result = compiled(*inputs)
        assert result is not None

    @pytest.mark.skip(reason="torch-via-onnx produces zero shapes in DATA_SCHEME, causing runtime segfault")
    @pytest.mark.parametrize("name,device", TARGET_PARAMS)
    def test_torch_via_onnx(self, name, device):
        """fhe.compute()(func) with torch-via-onnx frontend (skipped: shape bug)."""
        if not _provider_available(name, device):
            pytest.skip(f"{name}/{device} compiler not available")

        inputs = tuple(t.clone() for t in ADD_FUNC.example_inputs)

        compiled = fhe.compute(
            frontend="torch-via-onnx",
            library=name,
            device=device,
            encrypt_inputs=ADD_FUNC.encrypt_inputs,
        )(ADD_FUNC.func)

        result = compiled(*inputs)
        assert result is not None
