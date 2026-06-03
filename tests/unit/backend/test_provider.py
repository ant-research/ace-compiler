# tests/unit/backend/test_provider.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Unit tests for library provider instantiation, properties, and build commands.

Tests the provider/access layer (get_library_impl, attributes, build_command)
without requiring the actual compiler toolchain.
"""

import pytest

from ace.fhe.backend import get_library_impl
from utils import PROVIDER_SPECS, ALL_PROVIDER


# =============================================================================
# Test: Provider Properties
# =============================================================================

class TestProviderProperties:
    """Tests for library provider instantiation and properties."""

    @pytest.mark.parametrize("name,device", ALL_PROVIDER)
    def test_properties(self, name, device):
        """Provider name, device, and supported formats from implementation."""
        pro = get_library_impl(name, device=device)
        assert pro.backend_name() == name
        assert pro.device_name() == device
        formats = pro.supported_format_types()
        assert isinstance(formats, list)
        assert len(formats) > 0

    @pytest.mark.parametrize("name,device", ALL_PROVIDER)
    def test_options(self, name, device):
        """Provider instantiation with options."""
        pro = get_library_impl(name, device=device, vec={"ms": 16})
        assert pro is not None
        assert pro._options.get("vec") == {"ms": 16}


# =============================================================================
# Test: Build Command
# =============================================================================

# Providers with working build_command (implemented only)
_BUILD_PROVIDERS = [
    pytest.param(name, spec["device"], id=f"{name}-{spec['device']}")
    for name, spec in PROVIDER_SPECS.items()
    if spec["implemented"]
]


class TestBuildCommand:
    """Tests for build command generation."""

    @pytest.mark.parametrize("name,device", _BUILD_PROVIDERS)
    def test_build_command_basic(self, name, device, tmp_path):
        """Basic build command includes compiler, source, output, and flags."""
        pro = get_library_impl(name, device=device)
        kwargs = {"source": "test.cpp", "output": "test.so"}
        if name in ("openfhe", "acelib"):
            kwargs["ace_root"] = str(tmp_path)
        if name == "acelib":
            kwargs["extra_flags"] = []
        cmd = pro.build_command(**kwargs)
        assert "test.cpp" in cmd
        assert "test.so" in cmd
        assert "-shared" in cmd

    @pytest.mark.parametrize("name,device", _BUILD_PROVIDERS)
    def test_build_command_with_flags(self, name, device, tmp_path):
        """Build command with extra flags."""
        pro = get_library_impl(name, device=device)
        kwargs = {"source": "test.cpp", "output": "test.so", "extra_flags": ["-O3", "-march=native"]}
        if name in ("openfhe", "acelib"):
            kwargs["ace_root"] = str(tmp_path)
        cmd = pro.build_command(**kwargs)
        assert "-O3" in cmd
        assert "-march=native" in cmd

    @pytest.mark.parametrize("name,device", _BUILD_PROVIDERS)
    def test_build_command_with_ace_root(self, name, device, tmp_path):
        """Build command with custom ace_root."""
        pro = get_library_impl(name, device=device)
        kwargs = {"source": "test.cpp", "output": "test.so", "ace_root": str(tmp_path)}
        if name == "acelib":
            kwargs["extra_flags"] = []
        cmd = pro.build_command(**kwargs)
        assert isinstance(cmd, list)
        assert len(cmd) > 0


# =============================================================================
# Test: compile_to_lib Error Handling
# =============================================================================

# Providers that validate IR type before processing (raise ValueError for unsupported IR)
# Excludes: acelib (raises NotImplementedError), seal (raises NotImplementedError)
_IR_VALIDATING_PROVIDERS = [
    pytest.param(name, spec["device"], id=f"{name}-{spec['device']}")
    for name, spec in PROVIDER_SPECS.items()
    if name not in ("acelib", "seal")
]


class TestCompileToLibErrors:
    """Tests for compile_to_lib error handling (no compiler needed)."""

    @pytest.mark.parametrize("name,device", _IR_VALIDATING_PROVIDERS)
    def test_compile_to_lib_unsupported_ir(self, name, device):
        """Unsupported IR type raises ValueError."""
        pro = get_library_impl(name, device=device)

        class UnsupportedIR:
            pass

        with pytest.raises(ValueError):
            pro.compile_to_lib(UnsupportedIR(), "/tmp/output")

    def test_openfhe_compile_to_lib_air_not_implemented(self):
        """OpenFHE AIR compilation raises NotImplementedError."""
        pro = get_library_impl("openfhe", device="cpu")

        class MockAIR:
            @property
            def format_type(self):
                return "air"

        with pytest.raises(NotImplementedError, match="AIR compilation for OpenFHE not yet implemented"):
            pro.compile_to_lib(MockAIR(), "/tmp/output")
