# tests/unit/driver/test_registry.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Unit tests for frontend and backend registry.

Tests registration, retrieval, and validation for all registered
frontends and backends.
"""

import pytest

from ace.fhe.driver import (
    list_frontends,
    get_frontend,
    register_frontend,
    list_libraries,
)
from ace.fhe.backend import (
    register_library,
    get_library_impl,
    check_library,
    list_supported_combos,
)
from utils import PROVIDER_SPECS, ALL_PROVIDER


# =============================================================================
# Frontend Registry
# =============================================================================

class TestFrontendRegistry:
    """Tests for frontend registry operations."""

    def test_list_frontends_returns_list(self):
        frontends = list_frontends()
        assert isinstance(frontends, list)
        assert len(frontends) > 0

    def test_all_frontends_registered(self):
        frontends = list_frontends()
        expected = ["torch", "torch-via-onnx", "onnx", "ast", "ast-via-onnx"]
        for name in expected:
            assert name in frontends, f"Expected {name} in registered frontends"

    def test_get_frontend_returns_instance(self):
        frontend = get_frontend("torch")
        assert frontend is not None
        assert hasattr(frontend, "to_ir")

    @pytest.mark.parametrize("name", list_frontends())
    def test_get_frontend_each(self, name):
        frontend = get_frontend(name)
        assert frontend is not None
        assert hasattr(frontend, "to_ir")

    def test_get_frontend_returns_new_instance(self):
        f1 = get_frontend("torch")
        f2 = get_frontend("torch")
        assert f1 is not f2

    def test_get_unknown_frontend_raises(self):
        with pytest.raises(ValueError, match="Unknown frontend"):
            get_frontend("nonexistent")

    @pytest.mark.parametrize("name", list_frontends())
    def test_frontend_has_required_methods(self, name):
        frontend = get_frontend(name)
        for method in ["prepare", "compile", "export"]:
            assert hasattr(frontend, method), f"{name} missing method: {method}"
            assert callable(getattr(frontend, method))

    @pytest.mark.parametrize("name", list_frontends())
    def test_frontend_name_matches_registration(self, name):
        frontend = get_frontend(name)
        assert frontend.name() == name

    @pytest.mark.parametrize("name", list_frontends())
    def test_frontend_class_type(self, name):
        expected_classes = {
            "torch": "Torch",
            "torch-via-onnx": "TorchViaOnnx",
            "onnx": "Onnx",
            "ast": "AST",
            "ast-via-onnx": "ASTViaOnnx",
        }
        frontend = get_frontend(name)
        assert frontend.__class__.__name__ == expected_classes[name]

    def test_register_duplicate_frontend_raises(self):
        with pytest.raises(ValueError, match="already registered"):
            register_frontend("torch", "some.dummy.Class")


# =============================================================================
# Library Registry
# =============================================================================

class TestLibraryRegistry:
    """Tests for library registry operations."""

    def test_list_libraries_returns_list(self):
        libraries = list_libraries()
        assert isinstance(libraries, list)
        assert len(libraries) > 0

    def test_all_providers_registered(self):
        libraries = list_libraries()
        for name in PROVIDER_SPECS:
            assert name in libraries, f"Expected {name} in registered libraries"

    def test_list_supported_combos(self):
        combos = list_supported_combos()
        assert isinstance(combos, list)
        assert ("antlib", "cpu") in combos

    def test_check_library_antlib_cpu(self):
        assert check_library("antlib", "cpu") is True

    def test_check_library_invalid_device(self):
        result = check_library("antlib", "invalid_device")
        assert isinstance(result, bool)

    def test_check_library_invalid_name(self):
        assert check_library("invalid_library", "cpu") is False

    @pytest.mark.parametrize("name,device", ALL_PROVIDER)
    def test_get_library_impl_each(self, name, device):
        pro = get_library_impl(name, device=device)
        assert pro is not None
        assert pro.backend_name() == name
        assert pro.device_name() == device

    def test_get_library_impl_returns_new_instance(self):
        p1 = get_library_impl("antlib", device="cpu")
        p2 = get_library_impl("antlib", device="cpu")
        assert p1 is not p2

    def test_get_nonexistent_library_raises(self):
        with pytest.raises(ValueError, match="Unknown library"):
            get_library_impl("nonexistent_library", device="cpu")

    @pytest.mark.parametrize("name,device", ALL_PROVIDER)
    def test_impl_has_required_methods(self, name, device):
        pro = get_library_impl(name, device=device)
        for method in ["backend_name", "device_name", "check_available", "supported_format_types"]:
            assert hasattr(pro, method), f"{name} missing method: {method}"

    def test_register_duplicate_library_raises(self):
        from ace.fhe.backend.antlib import AntLIB
        with pytest.raises(ValueError, match="already registered"):
            register_library("antlib", "cpu", AntLIB)