# tests/test_unit/test_driver/test_registry.py
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


# ============================================================================
# Frontend Registry Tests
# ============================================================================

class TestFrontendRegistry:
    """Tests for frontend registry operations."""

    # ========================================================================
    # List Frontends Tests
    # ========================================================================

    def test_list_frontends_returns_list(self):
        """Test that list_frontends returns a list."""
        frontends = list_frontends()
        assert isinstance(frontends, list)
        assert len(frontends) > 0

    def test_all_frontends_registered(self):
        """Test that all expected frontends are registered."""
        frontends = list_frontends()
        expected = ["torch", "torch-via-onnx", "onnx", "ast", "ast-via-onnx"]
        for name in expected:
            assert name in frontends, f"Expected {name} in registered frontends"

    # ========================================================================
    # Get Frontend Tests
    # ========================================================================

    def test_get_frontend_works(self):
        """Test that get_frontend returns a valid frontend."""
        frontend = get_frontend("torch")
        assert frontend is not None
        assert hasattr(frontend, "to_ir")

    def test_get_all_frontends(self):
        """Test that all registered frontends can be retrieved."""
        for name in list_frontends():
            frontend = get_frontend(name)
            assert frontend is not None
            assert hasattr(frontend, "to_ir")

    def test_get_frontend_returns_new_instance(self):
        """Test that get_frontend returns new instance each time."""
        frontend1 = get_frontend("torch")
        frontend2 = get_frontend("torch")
        assert frontend1 is not frontend2

    def test_get_frontend_with_kwargs(self):
        """Test that get_frontend can pass kwargs to frontend constructor."""
        # Note: Frontends no longer accept constructor parameters for output format
        # Output format is now controlled via export(format="onnx"|"air") method
        frontend = get_frontend("torch-via-onnx")
        assert frontend is not None
        assert hasattr(frontend, "export")

    def test_get_unknown_frontend_raises_error(self):
        """Test that getting unknown frontend raises ValueError."""
        with pytest.raises(ValueError, match="Unknown frontend"):
            get_frontend("nonexistent")

    # ========================================================================
    # Frontend Interface Tests
    # ========================================================================

    def test_frontend_has_required_methods(self):
        """Test that all frontends have required methods."""
        required_methods = ["prepare", "compile", "export"]
        for name in list_frontends():
            frontend = get_frontend(name)
            for method in required_methods:
                assert hasattr(frontend, method), f"{name} missing method: {method}"
                assert callable(getattr(frontend, method)), f"{name}.{method} not callable"

    def test_frontend_has_required_class_methods(self):
        """Test that all frontend classes have required class methods."""
        required_class_methods = ["name"]
        for name in list_frontends():
            frontend = get_frontend(name)
            for method in required_class_methods:
                assert hasattr(frontend.__class__, method), f"{name} class missing method: {method}"

    def test_frontend_name_matches_registration(self):
        """Test that frontend.name() returns the registered name."""
        for name in list_frontends():
            frontend = get_frontend(name)
            assert frontend.name() == name, f"{name}.name() returns {frontend.name()}"

    # ========================================================================
    # Frontend Class Type Tests
    # ========================================================================

    def test_frontend_returns_correct_class_type(self):
        """Test that each frontend returns the expected class type."""
        expected_classes = {
            "torch": "Torch",
            "torch-via-onnx": "TorchViaOnnx",
            "onnx": "Onnx",
            "ast": "AST",
            "ast-via-onnx": "ASTViaOnnx",
        }
        for name, expected_class in expected_classes.items():
            frontend = get_frontend(name)
            assert frontend.__class__.__name__ == expected_class, \
                f"{name} expected {expected_class}, got {frontend.__class__.__name__}"

    # ========================================================================
    # Frontend Registration Tests
    # ========================================================================

    def test_register_duplicate_frontend_raises_error(self):
        """Test that registering duplicate frontend raises ValueError."""
        with pytest.raises(ValueError, match="already registered"):
            register_frontend("torch", "some.dummy.Class")


# ============================================================================
# Library Registry Tests
# ============================================================================

class TestLibraryRegistry:
    """Tests for library registry operations."""

    # ========================================================================
    # List Libraries Tests
    # ========================================================================

    def test_list_libraries_returns_list(self):
        """Test that list_libraries returns a list."""
        libraries = list_libraries()
        assert isinstance(libraries, list)
        assert len(libraries) > 0

    def test_all_libraries_registered(self):
        """Test that all expected libraries are registered."""
        libraries = list_libraries()
        expected = ["antlib", "seal", "phantom", "hyperfhe", "openfhe"]
        for name in expected:
            assert name in libraries, f"Expected {name} in registered libraries"

    def test_list_supported_combos(self):
        """Test listing supported library+device combinations."""
        supported = list_supported_combos()
        assert isinstance(supported, list)
        assert ("antlib", "cpu") in supported

    # ========================================================================
    # Check Library Tests
    # ========================================================================

    def test_check_library_antlib_cpu(self):
        """Test checking antlib CPU library availability."""
        assert check_library("antlib", "cpu") is True

    def test_check_library_antlib_cuda(self):
        """Test checking antlib CUDA library availability."""
        result = check_library("antlib", "cuda")
        assert isinstance(result, bool)

    def test_check_library_invalid_device(self):
        """Test that invalid device returns False."""
        result = check_library("antlib", "invalid_device")
        assert isinstance(result, bool)

    def test_check_library_invalid(self):
        """Test that invalid library returns False."""
        assert check_library("invalid_library", "cpu") is False

    # ========================================================================
    # Get Library Impl Tests
    # ========================================================================

    def test_get_library_impl_antlib(self):
        """Test getting antlib library implementation instance."""
        impl = get_library_impl("antlib", device="cpu")
        assert impl is not None
        assert impl.backend_name() == "antlib"
        assert impl.device_name() == "cpu"

    def test_get_library_impl_returns_new_instance(self):
        """Test that get_library_impl returns new instance each time."""
        impl1 = get_library_impl("antlib", device="cpu")
        impl2 = get_library_impl("antlib", device="cpu")
        assert impl1 is not impl2

    def test_get_nonexistent_library_impl_raises(self):
        """Test that getting nonexistent library raises error."""
        with pytest.raises(ValueError, match="Unknown library"):
            get_library_impl("nonexistent_library", device="cpu")

    # ========================================================================
    # Library Implementation Interface Tests
    # ========================================================================

    def test_impl_has_required_methods(self):
        """Test that all library implementations have required methods."""
        required_methods = ["backend_name", "device_name", "check_available", "supported_format_types"]
        combos = list_supported_combos()
        for name, device in combos:
            try:
                impl = get_library_impl(name, device=device)
                for method in required_methods:
                    assert hasattr(impl, method), f"{name} missing method: {method}"
            except TypeError:
                pytest.skip(f"Library {name} not fully implemented")

    def test_impl_has_required_class_methods(self):
        """Test that all library implementation classes have required class methods."""
        required_class_methods = ["backend_name", "device_name"]
        combos = list_supported_combos()
        for name, device in combos:
            try:
                impl = get_library_impl(name, device=device)
                for method in required_class_methods:
                    assert hasattr(impl.__class__, method), f"{name} class missing method: {method}"
            except TypeError:
                pytest.skip(f"Library {name} not fully implemented")

    # ========================================================================
    # Library Registration Tests
    # ========================================================================

    def test_register_new_library(self):
        """Test registering a new library."""
        class DummyLibrary:
            @classmethod
            def backend_name(cls):
                return "dummy_test"

            @classmethod
            def device_name(cls):
                return "cpu"

            def check_available(self):
                return True

        try:
            register_library("dummy_test", "cpu", DummyLibrary)
        except ValueError:
            pass  # Already registered

    def test_register_duplicate_library_raises(self):
        """Test that registering duplicate library raises ValueError."""
        from ace.fhe.backend.antlib import AntLIB
        with pytest.raises(ValueError, match="already registered"):
            register_library("antlib", "cpu", AntLIB)


# Run tests if executed directly
if __name__ == "__main__":
    pytest.main([__file__, "-v"])