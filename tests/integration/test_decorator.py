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
from utils import HAS_FRONTEND, HAS_TORCH_FX, TARGET_PARAMS

from ace import fhe


def _provider_available(name, device):
    try:
        from ace.fhe.backend import get_library_impl

        pro = get_library_impl(name, device=device)
        return pro.check_available()
    except Exception:
        return False


# =============================================================================
# @fhe.compile
# =============================================================================


@pytest.mark.skipif(not HAS_TORCH_FX or not HAS_FRONTEND, reason="torch.fx or frontend not available")
class TestCompile:
    """Tests for @fhe.compile decorator on models and functions."""

    @pytest.mark.parametrize("name,device", TARGET_PARAMS)
    def test_model_torch(self, name, device):
        """@fhe.compile on nn.Module class attaches compile/fhe_compile."""
        if not _provider_available(name, device):
            pytest.skip(f"{name}/{device} compiler not available")

        @fhe.compile(frontend="torch", library=name, device=device, encrypt_inputs=["x", "y"])
        class AddModel(torch.nn.Module):
            def forward(self, x, y):
                return x + y

        assert hasattr(AddModel, "compile")
        assert hasattr(AddModel, "fhe_compile")

    @pytest.mark.parametrize("name,device", TARGET_PARAMS)
    def test_function_torch_via_onnx(self, name, device):
        """@fhe.compile on function with torch-via-onnx frontend."""
        if not _provider_available(name, device):
            pytest.skip(f"{name}/{device} compiler not available")

        @fhe.compile(frontend="torch-via-onnx", library=name, device=device, encrypt_inputs=["x", "y"])
        def add(x, y):
            return x + y

        assert hasattr(add, "compile")
        assert hasattr(add, "_fhe_compiler")


# =============================================================================
# @fhe.compute
# =============================================================================


@pytest.mark.skipif(not HAS_TORCH_FX or not HAS_FRONTEND, reason="torch.fx or frontend not available")
class TestCompute:
    """Tests for @fhe.compute decorator — compile + run in one call."""

    @pytest.mark.parametrize("name,device", TARGET_PARAMS)
    def test_model_torch(self, name, device):
        """@fhe.compute on nn.Module with torch frontend."""
        if not _provider_available(name, device):
            pytest.skip(f"{name}/{device} compiler not available")

        @fhe.compute(frontend="torch", library=name, device=device, encrypt_inputs=["x", "y"])
        class AddModel(torch.nn.Module):
            def forward(self, x, y):
                return x + y

        x = torch.randn(2, 3)
        y = torch.randn(2, 3)
        result = AddModel(x, y)
        assert result is not None

    @pytest.mark.parametrize("name,device", TARGET_PARAMS)
    def test_function_torch(self, name, device):
        """@fhe.compute on function with torch frontend."""
        if not _provider_available(name, device):
            pytest.skip(f"{name}/{device} compiler not available")

        @fhe.compute(frontend="torch", library=name, device=device, encrypt_inputs=["x", "y"])
        def add(x, y):
            return x + y

        x = torch.randn(2, 3)
        y = torch.randn(2, 3)
        result = add(x, y)
        assert result is not None

    @pytest.mark.skip(reason="torch-via-onnx produces zero shapes in DATA_SCHEME, causing runtime segfault")
    @pytest.mark.parametrize("name,device", TARGET_PARAMS)
    def test_model_torch_via_onnx(self, name, device):
        """@fhe.compute on nn.Module with torch-via-onnx frontend (skipped: shape bug)."""
        if not _provider_available(name, device):
            pytest.skip(f"{name}/{device} compiler not available")

        @fhe.compute(frontend="torch-via-onnx", library=name, device=device, encrypt_inputs=["x", "y"])
        class AddModel(torch.nn.Module):
            def forward(self, x, y):
                return x + y

        x = torch.randn(2, 3)
        y = torch.randn(2, 3)
        result = AddModel(x, y)
        assert result is not None

    @pytest.mark.skip(reason="torch-via-onnx produces zero shapes in DATA_SCHEME, causing runtime segfault")
    @pytest.mark.parametrize("name,device", TARGET_PARAMS)
    def test_function_torch_via_onnx(self, name, device):
        """@fhe.compute on function with torch-via-onnx frontend (skipped: shape bug)."""
        if not _provider_available(name, device):
            pytest.skip(f"{name}/{device} compiler not available")

        @fhe.compute(frontend="torch-via-onnx", library=name, device=device, encrypt_inputs=["x", "y"])
        def add(x, y):
            return x + y

        x = torch.randn(2, 3)
        y = torch.randn(2, 3)
        result = add(x, y)
        assert result is not None


# =============================================================================
# @fhe.export
# =============================================================================


@pytest.mark.skipif(not HAS_TORCH_FX or not HAS_FRONTEND, reason="torch.fx or frontend not available")
class TestExport:
    """Tests for @fhe.export decorator — compile + export to file."""

    @pytest.mark.parametrize("name,device", TARGET_PARAMS)
    def test_model_torch(self, name, device, tmp_path):
        """@fhe.export on nn.Module with torch frontend: writes AIR file."""
        if not _provider_available(name, device):
            pytest.skip(f"{name}/{device} compiler not available")

        air_path = str(tmp_path / "model.B")

        @fhe.export(
            frontend="torch", library=name, device=device, format="air", output_path=air_path, encrypt_inputs=["x"]
        )
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
    def test_function_torch(self, name, device, tmp_path):
        """@fhe.export on function with torch frontend."""
        if not _provider_available(name, device):
            pytest.skip(f"{name}/{device} compiler not available")

        air_path = str(tmp_path / "func.B")

        @fhe.export(
            frontend="torch",
            library=name,
            device=device,
            format="air",
            output_path=air_path,
            encrypt_inputs=["x", "y"],
        )
        def add(x, y):
            return x + y

        x = torch.randn(2, 3)
        y = torch.randn(2, 3)
        add.export([x, y])
        assert os.path.exists(air_path)

    @pytest.mark.skip(reason="torch-via-onnx produces zero shapes in DATA_SCHEME")
    @pytest.mark.parametrize("name,device", TARGET_PARAMS)
    def test_function_torch_via_onnx(self, name, device, tmp_path):
        """@fhe.export on function with torch-via-onnx frontend (skipped: shape bug)."""
        if not _provider_available(name, device):
            pytest.skip(f"{name}/{device} compiler not available")

        air_path = str(tmp_path / "func.B")

        @fhe.export(
            frontend="torch-via-onnx",
            library=name,
            device=device,
            format="air",
            output_path=air_path,
            encrypt_inputs=["x", "y"],
        )
        def add(x, y):
            return x + y

        x = torch.randn(2, 3)
        y = torch.randn(2, 3)
        add.export([x, y])
        assert os.path.exists(air_path)
