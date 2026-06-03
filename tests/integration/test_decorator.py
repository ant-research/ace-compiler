# tests/integration/test_decorator.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Integration tests for FHE decorator APIs.

Tests the decorator form: @fhe.compile / @fhe.compute / @fhe.export
Models and functions are defined inline since decorators are applied at
definition time and cannot be easily parametrized at module level.

Note: AST frontend is not compatible with decorators (inspect.getsource
resolves to decorators.py instead of the original function), so only
torch/torch-via-onnx frontends are tested here.
"""

import os
import pytest
import torch

from ace import fhe
from utils import HAS_TORCH_FX, HAS_FRONTEND, TARGET_PARAMS


def _provider_available(name, device):
    try:
        from ace.fhe.backend import get_library_impl
        pro = get_library_impl(name, device=device)
        return pro.check_available()
    except Exception:
        return False


# =============================================================================
# @fhe.compile decorator
# =============================================================================

@pytest.mark.skipif(not HAS_TORCH_FX or not HAS_FRONTEND, reason="torch.fx or frontend not available")
class TestCompileDecorator:
    """Tests for @fhe.compile decorator on models and functions."""

    @pytest.mark.parametrize("name,device", TARGET_PARAMS)
    def test_compile_model_class(self, name, device):
        """@fhe.compile on nn.Module class attaches compile/fhe_compile."""
        if not _provider_available(name, device):
            pytest.skip(f"{name}/{device} compiler not available")

        @fhe.compile(frontend="torch", library=name, device=device,
                      encrypt_inputs=["x", "y"])
        class AddModel(torch.nn.Module):
            def forward(self, x, y):
                return x + y

        assert hasattr(AddModel, 'compile')
        assert hasattr(AddModel, 'fhe_compile')

    @pytest.mark.parametrize("name,device", TARGET_PARAMS)
    def test_compile_function(self, name, device):
        """@fhe.compile on function with torch-via-onnx frontend."""
        if not _provider_available(name, device):
            pytest.skip(f"{name}/{device} compiler not available")

        @fhe.compile(frontend="torch-via-onnx", library=name, device=device,
                      encrypt_inputs=["x", "y"])
        def add(x, y):
            return x + y

        assert hasattr(add, 'compile')
        assert hasattr(add, '_fhe_compiler')


# =============================================================================
# @fhe.compute decorator
# =============================================================================

@pytest.mark.skipif(not HAS_TORCH_FX or not HAS_FRONTEND, reason="torch.fx or frontend not available")
class TestComputeDecorator:
    """Tests for @fhe.compute decorator — compile + run in one call."""

    @pytest.mark.parametrize("name,device", TARGET_PARAMS)
    def test_compute_model(self, name, device):
        """@fhe.compute on nn.Module: calling with inputs runs FHE inference."""
        if not _provider_available(name, device):
            pytest.skip(f"{name}/{device} compiler not available")

        @fhe.compute(frontend="torch-via-onnx", library=name, device=device,
                      encrypt_inputs=["x", "y"])
        class AddModel(torch.nn.Module):
            def forward(self, x, y):
                return x + y

        x = torch.randn(2, 3)
        y = torch.randn(2, 3)
        result = AddModel(x, y)
        assert result is not None

    @pytest.mark.parametrize("name,device", TARGET_PARAMS)
    def test_compute_function(self, name, device):
        """@fhe.compute on function with torch-via-onnx frontend."""
        if not _provider_available(name, device):
            pytest.skip(f"{name}/{device} compiler not available")

        @fhe.compute(frontend="torch-via-onnx", library=name, device=device,
                      encrypt_inputs=["x", "y"])
        def add(x, y):
            return x + y

        x = torch.randn(2, 3)
        y = torch.randn(2, 3)
        result = add(x, y)
        assert result is not None


# =============================================================================
# @fhe.export decorator
# =============================================================================

@pytest.mark.skipif(not HAS_TORCH_FX or not HAS_FRONTEND, reason="torch.fx or frontend not available")
class TestExportDecorator:
    """Tests for @fhe.export decorator — compile + export to file."""

    @pytest.mark.parametrize("name,device", TARGET_PARAMS)
    def test_export_model_air(self, name, device, tmp_path):
        """@fhe.export on nn.Module: .export(inputs) writes AIR file."""
        if not _provider_available(name, device):
            pytest.skip(f"{name}/{device} compiler not available")

        air_path = str(tmp_path / "model.B")

        @fhe.export(frontend="torch", library=name, device=device,
                     format="air", output_path=air_path,
                     encrypt_inputs=["x"])
        class LinearModel(torch.nn.Module):
            def __init__(self):
                super().__init__()
                self.linear = torch.nn.Linear(3, 2)

            def forward(self, x):
                return self.linear(x)

        x = torch.randn(1, 3)
        LinearModel.export([x])
        assert os.path.exists(air_path)

    @pytest.mark.parametrize("name,device", TARGET_PARAMS)
    def test_export_function_air(self, name, device, tmp_path):
        """@fhe.export on function with torch-via-onnx frontend."""
        if not _provider_available(name, device):
            pytest.skip(f"{name}/{device} compiler not available")

        air_path = str(tmp_path / "func.B")

        @fhe.export(frontend="torch-via-onnx", library=name, device=device,
                     format="air", output_path=air_path,
                     encrypt_inputs=["x", "y"])
        def add(x, y):
            return x + y

        x = torch.randn(2, 3)
        y = torch.randn(2, 3)
        add.export([x, y])
        assert os.path.exists(air_path)