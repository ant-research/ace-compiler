#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import math
import warnings
import pytest
from instantiator_ckks.bootstrap_ctx import BtsCtx
from instantiator_ckks.cheby_ctx import ChebyCtx
from instantiator_ckks.cs_ctx import CoeffSlotCtx


class TestBootstrapCtx:
    class MockChebyCtx:
        def __init__(self, bts_ctx, even_poly):
            self._bts_ctx = bts_ctx
            self._even_poly = even_poly
            self._double_angle = 32

        def get_approx_depth(self):
            return 5  # Fixed test value

    class MockCoeffSlotCtx:
        def __init__(self, bts_ctx, is_c2s):
            self._bts_ctx = bts_ctx
            self._is_c2s = is_c2s

    @pytest.fixture(autouse=True)
    def setup(self):
        # Default test parameters
        self.slots = 32
        self.m = 128  # ring_degree * 2
        self.hw = 0  # hamming weight
        self.lvl_bdg = [3, 3]  # level budget
        self.dims = [0, 0]  # dimensions
        self.q0 = 2**60  # ciphertext modulus
        self.sf = 2**51  # scaling factor
        self.clear_image = False
        self.even_poly = False

    def test_initialization(self):
        """Test BtsCtx initialization with valid parameters"""
        bts_ctx = BtsCtx(
            slots=self.slots,
            m=self.m,
            hw=self.hw,
            level_budget=self.lvl_bdg,
            dims=self.dims,
            q0=self.q0,
            sf=self.sf,
            clear_image=self.clear_image,
            even_poly=self.even_poly
        )

        # Verify parameters were set correctly
        assert bts_ctx._slots == self.slots
        assert bts_ctx._m == self.m
        assert bts_ctx._hw == self.hw
        assert bts_ctx._lvl_bdg == self.lvl_bdg
        assert bts_ctx._dims == self.dims
        assert bts_ctx._q0 == self.q0
        assert bts_ctx._sf == self.sf
        assert bts_ctx._clear_image == self.clear_image
        assert bts_ctx._even_poly == self.even_poly

        # Verify derived values
        assert bts_ctx._q0_sf_ratio == round(math.log2(self.q0 / self.sf))
        assert isinstance(bts_ctx._cs, CoeffSlotCtx)
        assert isinstance(bts_ctx._sc, CoeffSlotCtx)
        assert isinstance(bts_ctx._cheby, ChebyCtx)

    def test_compute_bts_depth(self):
        """Test bootstrap depth calculation"""
        bts_ctx = BtsCtx(
            slots=self.slots,
            m=self.m,
            hw=self.hw,
            level_budget=self.lvl_bdg,
            dims=self.dims,
            q0=self.q0,
            sf=self.sf,
            clear_image=self.clear_image,
            even_poly=self.even_poly
        )

        # Mock cheby.get_approx_depth() to return fixed value
        bts_ctx._cheby = self.MockChebyCtx(bts_ctx, self.even_poly)

        # cheby_depth + lvl_bdg[0] + lvl_bdg[1]
        expected_depth = 5 + self.lvl_bdg[0] + self.lvl_bdg[1]
        assert bts_ctx.compute_bts_depth() == expected_depth

    def test_check_config(self):
        """Test configuration validation"""
        # Test valid config
        bts_ctx = BtsCtx(
            slots=self.slots,
            m=self.m,
            hw=self.hw,
            level_budget=self.lvl_bdg,
            dims=self.dims,
            q0=self.q0,
            sf=self.sf,
            clear_image=self.clear_image,
            even_poly=self.even_poly
        )
        bts_ctx.check_config()  # Should not raise warnings

        # Test invalid slot count
        bts_ctx._slots = 1
        with pytest.raises(ValueError):
            bts_ctx.check_config()

        bts_ctx._slots = 0
        with warnings.catch_warnings(record=True) as w:
            bts_ctx.check_config()
            assert bts_ctx._slots == self.m // 4
            assert len(w) == 0  # No warning for slots=0

        # Test invalid level budget (too large)
        bts_ctx._lvl_bdg = [10, 10]  # > log2(slots)
        with warnings.catch_warnings(record=True) as w:
            bts_ctx.check_config()
            assert bts_ctx._lvl_bdg[0] == int(math.log2(bts_ctx._slots))
            assert len(w) == 1
            assert "level budget for encoding cannot be this large" in str(w[0].message)

        # Test invalid level budget (too small)
        bts_ctx._lvl_bdg = [0, 0]
        with warnings.catch_warnings(record=True) as w:
            bts_ctx.check_config()
            assert bts_ctx._lvl_bdg[0] == 1
            assert len(w) == 1
            assert "level budget for encoding has to be at least 1" in str(w[0].message)

    def test_edge_cases(self):
        """Test boundary conditions"""

        # Test maximum slots (m//4)
        bts_ctx = BtsCtx(
            slots=0,  # will be set to m//4
            m=128,
            hw=0,
            level_budget=[3, 3],
            dims=[0, 0],
            q0=2**10,
            sf=2**5,
            clear_image=False,
            even_poly=False
        )
        assert bts_ctx._slots == 32

    def test_psum_rot_idx(self):
        """Test partial sum rotation index calculation"""

        # Test non_sparse
        bts_ctx = BtsCtx(
            slots=8,
            m=32,
            hw=0,
            level_budget=[3, 3],
            dims=[0, 0],
            q0=2**10,
            sf=2**5,
            clear_image=False,
            even_poly=False
        )
        assert bts_ctx._psum_rot_idx == []

        # Test sparse
        bts_ctx = BtsCtx(
            slots=4,
            m=128,
            hw=0,
            level_budget=[3, 3],
            dims=[0, 0],
            q0=2**10,
            sf=2**5,
            clear_image=False,
            even_poly=False
        )
        assert bts_ctx._psum_rot_idx == [4, 8, 16]


if __name__ == "__main__":
    pytest.main()
