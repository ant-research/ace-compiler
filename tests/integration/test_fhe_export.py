# tests/integration/test_fhe_export.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Integration tests for fhe.export() top-level API.

Tests the functional form: fhe.export(...)(target) → .export(inputs) → IR file
"""

import os
import pytest
import torch

from ace import fhe
from ace.sample.ops.specs import LINEAR_OP
from ace.sample.funcs.specs import ADD_FUNC
from utils import HAS_TORCH_FX, HAS_FRONTEND, TARGET_PARAMS


def _provider_available(name, device):
    try:
        from ace.fhe.backend import get_library_impl
        pro = get_library_impl(name, device=device)
        return pro.check_available()
    except Exception:
        return False


# =============================================================================
# fhe.export on nn.Module
# =============================================================================

@pytest.mark.skipif(not HAS_TORCH_FX or not HAS_FRONTEND, reason="torch.fx or frontend not available")
class TestExportModel:
    """Tests for fhe.export() on nn.Module models."""

    @pytest.mark.parametrize("name,device", TARGET_PARAMS)
    def test_export_air(self, name, device, tmp_path):
        """fhe.export()(model) with format='air' produces AIR file."""
        if not _provider_available(name, device):
            pytest.skip(f"{name}/{device} compiler not available")

        model = LINEAR_OP.create_model()
        air_path = str(tmp_path / "model.B")

        compiled = fhe.export(
            frontend="torch", library=name, device=device,
            format="air", output_path=air_path,
            encrypt_inputs=LINEAR_OP.encrypt_inputs,
        )(model)

        compiled.export(list(LINEAR_OP.example_inputs))
        assert os.path.exists(air_path)


# =============================================================================
# fhe.export on function
# =============================================================================

@pytest.mark.skipif(not HAS_FRONTEND, reason="frontend not available")
class TestExportFunction:
    """Tests for fhe.export() on Python functions."""

    @pytest.mark.parametrize("name,device", TARGET_PARAMS)
    def test_export_function_air(self, name, device, tmp_path):
        """fhe.export()(func) with format='air' produces AIR file."""
        if not _provider_available(name, device):
            pytest.skip(f"{name}/{device} compiler not available")

        air_path = str(tmp_path / "func.B")

        compiled = fhe.export(
            frontend="ast", library=name, device=device,
            format="air", output_path=air_path,
            encrypt_inputs=ADD_FUNC.encrypt_inputs,
        )(ADD_FUNC.func)

        compiled.export(list(ADD_FUNC.example_inputs))
        assert os.path.exists(air_path)