# tests/unit/config/test_options.py
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
#

"""
Unit tests for FHE configuration options.

Tests CompileOptions, ComputeOptions, FHEConfig, and BaseOption.
"""

import pytest

from ace.fhe.config import CompileOptions, ComputeOptions
from ace.fhe.config.compile_options import FHEConfig
from ace.fhe.config.base import BaseOption


# =============================================================================
# FHEConfig
# =============================================================================

class TestFHEConfig:
    """Tests for FHEConfig defaults and customization."""

    def test_defaults(self):
        config = FHEConfig()
        assert config.scheme == "CKKS"
        assert config.poly_modulus_degree == 8192
        assert config.multiplication_depth == 2
        assert config.backend == "CPU"

    def test_custom(self):
        config = FHEConfig(scheme="TFHE", poly_modulus_degree=4096,
                           multiplication_depth=3, backend="GPU")
        assert config.scheme == "TFHE"
        assert config.poly_modulus_degree == 4096
        assert config.multiplication_depth == 3
        assert config.backend == "GPU"


# =============================================================================
# CompileOptions
# =============================================================================

class TestCompileOptions:
    """Tests for CompileOptions."""

    def test_defaults(self):
        opts = CompileOptions()
        assert opts.encrypt_inputs is None
        assert opts.config.scheme == "CKKS"
        assert opts.config.poly_modulus_degree == 8192
        assert opts.vec is None
        assert opts.ckks is None
        assert opts.sihe is None
        assert opts.p2c is None
        assert opts.o2a is None
        assert opts.fhe_scheme is None
        assert opts.poly is None
        assert opts.relu_vr_data is None
        assert opts.relu_vr_file is None
        assert opts.profile_relu is False

    def test_encrypt_inputs_by_name(self):
        opts = CompileOptions(encrypt_inputs=["x", "y"])
        assert opts.encrypt_inputs == ["x", "y"]

    def test_encrypt_inputs_by_index(self):
        opts = CompileOptions(encrypt_inputs=[0, 1])
        assert opts.encrypt_inputs == [0, 1]

    def test_custom_config(self):
        config = FHEConfig(scheme="TFHE", poly_modulus_degree=4096)
        opts = CompileOptions(config=config)
        assert opts.config.scheme == "TFHE"
        assert opts.config.poly_modulus_degree == 4096

    def test_compiler_options(self):
        opts = CompileOptions(vec={"ms": 256}, ckks={"N": 4096, "hw": 192})
        assert opts.vec == {"ms": 256}
        assert opts.ckks == {"N": 4096, "hw": 192}

    def test_relu_options(self):
        opts = CompileOptions(
            relu_vr_data={"relu_0": 4.0},
            relu_vr_file="/tmp/vr.json",
            profile_relu=True,
        )
        assert opts.relu_vr_data == {"relu_0": 4.0}
        assert opts.relu_vr_file == "/tmp/vr.json"
        assert opts.profile_relu is True


# =============================================================================
# ComputeOptions
# =============================================================================

class TestComputeOptions:
    """Tests for ComputeOptions (extends CompileOptions)."""

    def test_defaults(self):
        opts = ComputeOptions()
        assert opts.encrypt_inputs is None
        assert opts.validate is True
        assert opts.server_url is None
        assert opts.config.scheme == "CKKS"

    def test_inherits_compile_options(self):
        opts = ComputeOptions(encrypt_inputs=["x"], vec={"ms": 16})
        assert opts.encrypt_inputs == ["x"]
        assert opts.vec == {"ms": 16}

    def test_compute_specific_fields(self):
        opts = ComputeOptions(validate=False, server_url="http://localhost:8080")
        assert opts.validate is False
        assert opts.server_url == "http://localhost:8080"

    def test_all_fields(self):
        opts = ComputeOptions(
            encrypt_inputs=["x", "y"],
            config=FHEConfig(scheme="TFHE"),
            vec={"ms": 256},
            validate=False,
            server_url="http://example.com",
        )
        assert opts.encrypt_inputs == ["x", "y"]
        assert opts.config.scheme == "TFHE"
        assert opts.vec == {"ms": 256}
        assert opts.validate is False
        assert opts.server_url == "http://example.com"


# =============================================================================
# BaseOption
# =============================================================================

class TestBaseOption:
    """Tests for BaseOption.to_compiler_options()."""

    def test_empty(self):
        opts = BaseOption()
        assert opts.to_compiler_options() == {}

    def test_verbose_only(self):
        opts = BaseOption(verbose=True)
        assert opts.to_compiler_options() == {}

    def test_compiler_options_extracted(self):
        opts = CompileOptions(vec={"ms": 256}, ckks={"N": 4096})
        result = opts.to_compiler_options()
        assert result == {"vec": {"ms": 256}, "ckks": {"N": 4096}}

    def test_none_options_excluded(self):
        opts = CompileOptions(vec={"ms": 256})
        result = opts.to_compiler_options()
        assert "vec" in result
        assert "ckks" not in result
        assert "sihe" not in result