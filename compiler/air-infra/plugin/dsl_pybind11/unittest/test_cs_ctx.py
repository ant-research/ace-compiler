#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

from instantiator_ckks.cs_ctx import CoeffSlotCtx  # noqa T484
import pytest  # noqa T484
import math
import cmath
import warnings


def get_eval_sin_upper_bound_k(self, hw, even):
    if hw > 0 and hw <= 192:
        return 32
    else:
        return 512


class TestCoeffSlotCtx():
    class MockContext:
        def __init__(self, slots, lvl_budget, dims, hw, even):
            self._slots = slots
            self._m = 32  # m = 4*slots
            self._lvl_bdg = lvl_budget
            self._dims = dims
            self._degree = 16
            self._hw = hw
            self._sf = 2**51
            self._q0 = 2**60
            self._even_poly = even

        def check_config(self):
            log_slots = math.log2(self._slots)
            for i, item in enumerate(self._lvl_bdg):
                if i > 1:
                    break
                else:
                    if (item > log_slots):
                        warnings.warn("the level budget for encoding cannot be this large."
                                      f"The budget was changed to {log_slots}\n", )
                        self._lvl_bdg[i] = int(log_slots)
                    if (item < 1):
                        warnings.warn(" the level budget for encoding has to be at least 1."
                                      " The budget was changed to 1\n")
                        self._lvl_bdg[i] = 1

    def compare_complex_lists(self, list1, list2, abs_tol=1e-6, rel_tol=1e-6):
        """
        Compare two lists of 3D complex numbers using pytest.approx.

        Args:
            list1: First list of 3D complex numbers [(complex, complex, complex), ...]
            list2: Second list of 3D complex numbers [(complex, complex, complex), ...]
            abs_tol: Absolute tolerance for comparison
            rel_tol: Relative tolerance for comparison

        Returns:
            bool: True if all elements are close within tolerances, False otherwise
        """
        if len(list1) != len(list2):
            return False
        dim1 = 0
        dim2 = 0
        for vec1, vec2 in zip(list1, list2):
            for c1, c2 in zip(vec1, vec2):
                # Create approx objects for comparison
                approx_c1 = pytest.approx(c1, abs=abs_tol, rel=rel_tol)
                if c2 != approx_c1:
                    print("list[", dim1, "][", dim2, "]c1, c2", c1, c2)
                    return False
                dim2 = dim2 + 1
            dim1 = dim1 + 1
        return True

    class MockCSCtx:
        def __init__(self, encoding, lvl_budget, layers, gs, bs, num_rots,
                     flag_rem, layers_rem, gs_rem, bs_rem, num_rots_rem):
            self._encoding = encoding
            self._lvl_budget = lvl_budget
            self._layers = layers
            self._gs = gs
            self._bs = bs
            self._num_rots = num_rots
            self._f_rem = flag_rem
            self._layers_rem = layers_rem
            self._gs_rem = gs_rem
            self._bs_rem = bs_rem
            self._num_rots_rem = num_rots_rem

        def match(self, test, other: CoeffSlotCtx):
            assert self._encoding == other._encoding
            assert self._lvl_budget == other._lvl_budget
            assert self._layers == other._layers
            assert self._gs == other._gs
            assert self._bs == other._bs
            assert self._num_rots == other._num_rots
            assert self._f_rem == other._f_rem
            assert self._layers_rem == other._layers_rem
            assert self._gs_rem == other._gs_rem
            assert self._bs_rem == other._bs_rem
            assert self._num_rots_rem == other._num_rots_rem

    def setup_method(self):
        # Default values matching the first test case
        self.slots = 32
        self.lvl_budget = [3, 3]
        self.dims = [0, 0]
        self.hw = 0
        self.even = False

        self.ctx = self.MockContext(self.slots, self.lvl_budget, self.dims, self.hw, self.even)
        self.c2s_ctx = CoeffSlotCtx(self.ctx, is_c2s=True)
        self.s2c_ctx = CoeffSlotCtx(self.ctx, is_c2s=False)

    def setup(self, slots=None, lvl_budget=None, dims=None, hw=None, even=None):
        # Default values matching the first test case
        self.slots = slots or 32
        self.lvl_budget = lvl_budget or [3, 3]
        self.dims = dims or [0, 0]
        self.hw = hw or 0
        self.even = even if even is not None else False

        self.ctx = self.MockContext(self.slots, self.lvl_budget, self.dims, self.hw, self.even)
        self.c2s_ctx = CoeffSlotCtx(self.ctx, is_c2s=True)
        self.s2c_ctx = CoeffSlotCtx(self.ctx, is_c2s=False)

    def test_set_colls_fft_params(self):
        """Test FFT parameter calculation"""
        # Test normal case
        self.c2s_ctx.set_colls_fft_params(32768, 3, 0)
        assert self.c2s_ctx._layers == 5
        assert self.c2s_ctx._gs == 16
        assert self.c2s_ctx._bs == 4
        assert self.c2s_ctx._num_rots == 63
        assert self.c2s_ctx._f_rem is False
        assert self.c2s_ctx._layers_rem == 0
        assert self.c2s_ctx._gs_rem == 0
        assert self.c2s_ctx._bs_rem == 0
        assert self.c2s_ctx._num_rots_rem == 1

        # Test remainder case
        self.c2s_ctx.set_colls_fft_params(16, 3, 0)
        assert self.c2s_ctx._layers == 1
        assert self.c2s_ctx._gs == 2
        assert self.c2s_ctx._bs == 2
        assert self.c2s_ctx._num_rots == 3
        assert self.c2s_ctx._f_rem is True
        assert self.c2s_ctx._layers_rem == 2
        assert self.c2s_ctx._gs_rem == 4
        assert self.c2s_ctx._bs_rem == 2
        assert self.c2s_ctx._num_rots_rem == 7

        self.c2s_ctx.set_colls_fft_params(32, 3, 0)
        assert self.c2s_ctx._layers == 2
        assert self.c2s_ctx._gs == 4
        assert self.c2s_ctx._bs == 2
        assert self.c2s_ctx._num_rots == 7
        assert self.c2s_ctx._f_rem is True
        assert self.c2s_ctx._layers_rem == 1
        assert self.c2s_ctx._gs_rem == 2
        assert self.c2s_ctx._bs_rem == 2
        assert self.c2s_ctx._num_rots_rem == 3

    def test_select_layers(self):
        """Test layer selection algorithm"""
        # Test exact division
        layers, rows, rem = self.c2s_ctx.select_layers(8, 4)
        assert layers == 2
        assert rows == 4
        assert rem == 0

        # Test with remainder
        layers, rows, rem = self.c2s_ctx.select_layers(7, 3)
        assert layers == 3
        assert rows == 2
        assert rem == 1

        layers, rows, rem = self.c2s_ctx.select_layers(4, 3)
        assert layers == 1
        assert rows == 2
        assert rem == 2

        layers, rows, rem = self.c2s_ctx.select_layers(15, 3)
        assert layers == 5
        assert rows == 3
        assert rem == 0

    def test_reduce_rotation(self):
        """Test rotation index normalization"""
        # Test power-of-two case
        assert self.c2s_ctx.reduce_rotation(-5, 16) == 11
        assert self.c2s_ctx.reduce_rotation(20, 16) == 4

        # Test non-power-of-two case
        assert self.c2s_ctx.reduce_rotation(-5, 15) == 10
        assert self.c2s_ctx.reduce_rotation(20, 15) == 5

    def test_coeff_enc_dec_one_level(self):
        """Test coefficient matrix generation"""
        slots = 16
        rot_group = [pow(5, k, 64) for k in range(16)]
        ksi_pows = [cmath.exp(2j * math.pi * k / 64) for k in range(65)]

        # Test encoding path
        enc_coeffs = self.c2s_ctx.coeff_enc_dec_one_level(
            ksi_pows, rot_group, False)
        assert len(enc_coeffs) == 12  # 3*log2(16)
        assert len(enc_coeffs[0]) == 16

        # Test decoding path
        dec_coeffs = self.s2c_ctx.coeff_enc_dec_one_level(
            ksi_pows, rot_group, False)
        assert len(dec_coeffs) == 12

    def test_rotation_precomp(self):
        """Test rotation precomputation"""
        # Setup test coefficients
        slots = 32
        rot_group = [pow(5, k, 64) for k in range(16)]
        ksi_pows = [cmath.exp(2j * math.pi * k / 64) for k in range(65)]
        coeffs = [[[1j for _ in range(slots)] for _ in range(7)] for _ in range(3)]

        # Test encoding path
        self.c2s_ctx.rotate_precomp(self.ctx, coeffs, ksi_pows, 1.0)
        assert len(self.c2s_ctx._plain_vals[0]) == 3

        # Test decoding path
        self.s2c_ctx.rotate_precomp(self.ctx, coeffs, ksi_pows, 1.0)
        assert len(self.s2c_ctx._plain_vals[0]) == 7

    def test_edge_cases(self):
        """Test boundary conditions and error handling"""
        # Test invalid slot count
        self.ctx._slots = 0
        with pytest.raises(ValueError):
            CoeffSlotCtx(self.ctx, True)

        # Test invalid level budget
        self.ctx._lvl_bdg = [0, 0]
        self.ctx._slots = 16
        with pytest.warns(UserWarning):
            self.ctx.check_config()

    EXP_C2S_VALS_FULL_GT192 = [
        [
            [complex(0, 0),
             complex(0.00438461737620848, -0.00438461737620848),
             complex(0, 0),
             complex(0.00438461737620848, -0.00438461737620848),
             complex(0, 0),
             complex(0.00438461737620848, -0.00438461737620848),
             complex(0, 0),
             complex(0.00438461737620848, -0.00438461737620848)],
            [complex(0.00620078535925077, 0),
             complex(-0.00438461737620848, 0.00438461737620848),
             complex(0.00620078535925077, 0),
             complex(-0.00438461737620848, 0.00438461737620848),
             complex(0.00620078535925077, 0),
             complex(-0.00438461737620848, 0.00438461737620848),
             complex(0.00620078535925077, 0),
             complex(-0.00438461737620848, 0.00438461737620848)],
            [complex(0.00620078535925077, 0),
             complex(0, 0),
             complex(0.00620078535925077, 0),
             complex(0, 0),
             complex(0.00620078535925077, 0),
             complex(0, 0),
             complex(0.00620078535925077, 0),
             complex(0, 0)],
        ],
        [
            [complex(0, 0),
             complex(0, 0),
             complex(0.00572877867890744, -0.00237293782463729),
             complex(-0.00237293782463729, -0.00572877867890744),
             complex(0, 0),
             complex(0, 0),
             complex(0.00572877867890744, -0.00237293782463729),
             complex(-0.00237293782463729, -0.00572877867890744)],
            [complex(0.00620078535925078, 0),
             complex(0.00620078535925078, 0),
             complex(-0.00572877867890744, 0.00237293782463729),
             complex(0.00237293782463729, 0.00572877867890744),
             complex(0.00620078535925078, 0),
             complex(0.00620078535925078, 0),
             complex(-0.00572877867890744, 0.00237293782463729),
             complex(0.00237293782463729, 0.00572877867890744)],
            [complex(0.00620078535925078, 0),
             complex(0.00620078535925078, 0),
             complex(0, 0),
             complex(0, 0),
             complex(0.00620078535925078, 0),
             complex(0.00620078535925078, 0),
             complex(0, 0),
             complex(0, 0)],
        ],
        [
            [complex(0, 0),
             complex(0, 0),
             complex(0, 0),
             complex(0, 0),
             complex(0.00608163900729302, -0.00120971321248913),
             complex(0.00344497176694349, -0.00515576459862755),
             complex(0.00120971321248913,
                     0.00608163900729302),
             complex(0.00515576459862755,
                     0.00344497176694349)],
            [complex(0.00620078535925078, 0),
             complex(0.00620078535925078, 0),
             complex(0.00620078535925078, 0),
             complex(0.00620078535925078, 0),
             complex(-0.00608163900729302, 0.00120971321248913),
             complex(-0.00344497176694349,
                     0.00515576459862755),
             complex(-0.00120971321248913,
                     -0.00608163900729302),
             complex(-0.00515576459862755,
                     -0.00344497176694349)],
            [complex(0.00620078535925078, 0),
             complex(0.00620078535925078, 0),
             complex(0.00620078535925078, 0),
             complex(0.00620078535925078, 0),
             complex(0, 0),
             complex(0, 0),
             complex(0, 0),
             complex(0, 0)]
        ]
    ]

    EXP_S2C_VALS_FULL_GT192 = [
        [
            [complex(0, 0),
             complex(1, 0),
             complex(0, 0),
             complex(1, 0),
             complex(0, 0),
             complex(1, 0),
             complex(0, 0),
             complex(1, 0)],
            [complex(1, 0),
             complex(-0.70710678118654757, -0.70710678118654746),
             complex(1, 0),
             complex(-0.70710678118654757, -0.70710678118654746),
             complex(1, 0),
             complex(-0.70710678118654757, -0.70710678118654746),
             complex(1, 0),
             complex(-0.70710678118654757, -0.70710678118654746)],
            [complex(0.70710678118654757, 0.70710678118654746),
             complex(0, 0),
             complex(0.70710678118654757, 0.70710678118654746),
             complex(0, 0),
             complex(0.70710678118654757, 0.70710678118654746),
             complex(0, 0),
             complex(0.70710678118654757, 0.70710678118654746),
             complex(0, 0)],
        ],
        [
            [complex(0, 0),
             complex(0, 0),
             complex(1, 0),
             complex(1, 0),
             complex(0, 0),
             complex(0, 0),
             complex(1, 0),
             complex(1, 0)],
            [complex(1, 0),
             complex(1, 0),
             complex(-0.92387953251128674, -0.38268343236508978),
             complex(0.38268343236508973, -0.92387953251128674),
             complex(1, 0),
             complex(1, 0),
             complex(-0.92387953251128674, -0.38268343236508978),
             complex(0.38268343236508973, -0.92387953251128674)],
            [complex(0.92387953251128674, 0.38268343236508978),
             complex(-0.38268343236508973, 0.92387953251128674),
             complex(0, 0),
             complex(0, 0),
             complex(0.92387953251128674, 0.38268343236508978),
             complex(-0.38268343236508973, 0.92387953251128674),
             complex(0, 0),
             complex(0, 0)],
        ],
        [
            [complex(0, 0),
             complex(0, 0),
             complex(0, 0),
             complex(0, 0),
             complex(1.00000000000000133, 0),
             complex(1.00000000000000133, 0),
             complex(1.00000000000000133, 0),
             complex(1.00000000000000133, 0)],
            [complex(1.00000000000000133, 0),
             complex(1.00000000000000133, 0),
             complex(1.00000000000000133, 0),
             complex(1.00000000000000133, 0),
             complex(-0.98078528040323176, -0.19509032201612850),
             complex(-0.55557023301960307, -0.83146961230254635),
             complex(-0.19509032201612855, 0.98078528040323176),
             complex(-0.83146961230254635, 0.55557023301960295)],
            [complex(0.98078528040323176, 0.19509032201612850),
             complex(0.55557023301960307, 0.83146961230254635),
             complex(0.19509032201612855, -0.98078528040323176),
             complex(0.83146961230254635, -0.55557023301960295),
             complex(0, 0),
             complex(0, 0),
             complex(0, 0),
             complex(0, 0)]
        ]
    ]

    EXP_C2S_VALS_FULL_LE192 = [
        [
            [complex(0, 0),
             complex(0.01104854345603979, -0.01104854345603980),
             complex(0, 0),
             complex(0.01104854345603979, -0.01104854345603980),
             complex(0, 0),
             complex(0.01104854345603979, -0.01104854345603980),
             complex(0, 0),
             complex(0.01104854345603979, -0.01104854345603980)],
            [complex(0.01562499999999998, 0),
             complex(-0.01104854345603979, 0.01104854345603980),
             complex(0.01562499999999998, 0),
             complex(-0.01104854345603979, 0.01104854345603980),
             complex(0.01562499999999998, 0),
             complex(-0.01104854345603979, 0.01104854345603980),
             complex(0.01562499999999998, 0),
             complex(-0.01104854345603979, 0.01104854345603980)],
            [complex(0.01562499999999998, 0),
             complex(0, 0),
             complex(0.01562499999999998, 0),
             complex(0, 0),
             complex(0.01562499999999998, 0),
             complex(0, 0),
             complex(0.01562499999999998, 0),
             complex(0, 0)],
        ],
        [
            [complex(0, 0),
             complex(0, 0),
             complex(0.01443561769548886, -0.00597942863070454),
             complex(-0.00597942863070454, -0.01443561769548886),
             complex(0, 0),
             complex(0, 0),
             complex(0.01443561769548886, -0.00597942863070454),
             complex(-0.00597942863070454, -0.01443561769548886)],
            [complex(0.01562500000000000, 0),
             complex(0.01562500000000000, 0),
             complex(-0.01443561769548886, 0.00597942863070454),
             complex(0.00597942863070454, 0.01443561769548886),
             complex(0.01562500000000000, 0),
             complex(0.01562500000000000, 0),
             complex(-0.01443561769548886, 0.00597942863070454),
             complex(0.00597942863070454, 0.01443561769548886)],
            [complex(0.01562500000000000, 0),
             complex(0.01562500000000000, 0),
             complex(0, 0),
             complex(0, 0),
             complex(0.01562500000000000, 0),
             complex(0.01562500000000000, 0),
             complex(0, 0),
             complex(0, 0)],
        ],
        [
            [complex(0, 0),
             complex(0, 0),
             complex(0, 0),
             complex(0, 0),
             complex(0.01532477000630048, -0.00304828628150201),
             complex(0.00868078489093128, -0.01299171269222728),
             complex(0.00304828628150201, 0.01532477000630048),
             complex(0.01299171269222727,
                     0.00868078489093129)],
            [complex(0.01562500000000000, 0),
             complex(0.01562500000000000, 0),
             complex(0.01562500000000000, 0),
             complex(0.01562500000000000, 0),
             complex(-0.01532477000630048, 0.00304828628150201),
             complex(-0.00868078489093128, 0.01299171269222728),
             complex(-0.00304828628150201, -0.01532477000630048),
             complex(-0.01299171269222727,
                     -0.00868078489093129)],
            [complex(0.01562500000000000, 0),
             complex(0.01562500000000000, 0),
             complex(0.01562500000000000, 0),
             complex(0.01562500000000000, 0),
             complex(0, 0),
             complex(0, 0),
             complex(0, 0),
             complex(0, 0)],
        ],
    ]

    EXP_S2C_VALS_FULL_LE192 = [
        [
            [complex(0, 0),
             complex(1, 0),
             complex(0, 0),
             complex(1, 0),
             complex(0, 0),
             complex(1, 0),
             complex(0, 0),
             complex(1, 0)],
            [complex(1, 0),
             complex(-0.70710678118654757, -0.70710678118654746),
             complex(1, 0),
             complex(-0.70710678118654757, -0.70710678118654746),
             complex(1, 0),
             complex(-0.70710678118654757, -0.70710678118654746),
             complex(1, 0),
             complex(-0.70710678118654757, -0.70710678118654746)],
            [complex(0.70710678118654757, 0.70710678118654746),
             complex(0, 0),
             complex(0.70710678118654757, 0.70710678118654746),
             complex(0, 0),
             complex(0.70710678118654757, 0.70710678118654746),
             complex(0, 0),
             complex(0.70710678118654757, 0.70710678118654746),
             complex(0, 0)],
        ],
        [
            [complex(0, 0),
             complex(0, 0),
             complex(1, 0),
             complex(1, 0),
             complex(0, 0),
             complex(0, 0),
             complex(1, 0),
             complex(1, 0)],
            [complex(1, 0),
             complex(1, 0),
             complex(-0.92387953251128674, -0.38268343236508978),
             complex(0.38268343236508973, -0.92387953251128674),
             complex(1, 0),
             complex(1, 0),
             complex(-0.92387953251128674, -0.38268343236508978),
             complex(0.38268343236508973, -0.92387953251128674)],
            [complex(0.92387953251128674, 0.38268343236508978),
             complex(-0.38268343236508973, 0.92387953251128674),
             complex(0, 0),
             complex(0, 0),
             complex(0.92387953251128674, 0.38268343236508978),
             complex(-0.38268343236508973, 0.92387953251128674),
             complex(0, 0),
             complex(0, 0)],
        ],
        [
            [complex(0, 0),
             complex(0, 0),
             complex(0, 0),
             complex(0, 0),
             complex(1.00000000000000133, 0),
             complex(1.00000000000000133, 0),
             complex(1.00000000000000133, 0),
             complex(1.00000000000000133, 0)],
            [complex(1.00000000000000133, 0),
             complex(1.00000000000000133, 0),
             complex(1.00000000000000133, 0),
             complex(1.00000000000000133, 0),
             complex(-0.98078528040323176, -0.19509032201612850),
             complex(-0.55557023301960307, -0.83146961230254635),
             complex(-0.19509032201612855,
                     0.98078528040323176),
             complex(-0.83146961230254635,
                     0.55557023301960295)],
            [complex(0.98078528040323176, 0.19509032201612850),
             complex(0.55557023301960307, 0.83146961230254635),
             complex(0.19509032201612855,
                     -0.98078528040323176),
             complex(0.83146961230254635,
                     -0.55557023301960295),
             complex(0, 0),
             complex(0, 0),
             complex(0, 0),
             complex(0, 0)],
        ]
    ]

    EXP_C2S_VALS_SPARSE_LE192 = [
        [
            [complex(0, 0),
             complex(0.00138106793200497, -0.00138106793200497),
             complex(0, 0),
             complex(0.00138106793200497, -0.00138106793200497),
             complex(0, 0),
             complex(-0.00138106793200497, -0.00138106793200497),
             complex(0, 0),
             complex(-0.00138106793200497, -0.00138106793200497)],
            [complex(0.00195312500000000, 0),
             complex(-0.00138106793200497, 0.00138106793200497),
             complex(0.00195312500000000, 0),
             complex(-0.00138106793200497, 0.00138106793200497),
             complex(0, -0.00195312500000000),
             complex(0.00138106793200497, 0.00138106793200497),
             complex(0, -0.00195312500000000),
             complex(0.00138106793200497, 0.00138106793200497)],
            [complex(0, -0.00195312500000000),
             complex(0, 0),
             complex(0.00195312500000000, 0),
             complex(0, 0),
             complex(0.00195312500000000, 0),
             complex(0, 0),
             complex(0, -0.00195312500000000),
             complex(0, 0)],
        ],
        [
            [complex(0, 0),
             complex(0, 0),
             complex(0.00180445221193611, -0.00074742857883807),
             complex(-0.00074742857883807, -0.00180445221193611),
             complex(0, 0),
             complex(0, 0),
             complex(0.00180445221193611, -0.00074742857883807),
             complex(-0.00074742857883807, -0.00180445221193611)],
            [complex(0.00195312500000000, 0),
             complex(0.00195312500000000, 0),
             complex(-0.00180445221193611, 0.00074742857883807),
             complex(0.00074742857883807, 0.00180445221193611),
             complex(0.00195312500000000, 0),
             complex(0.00195312500000000, 0),
             complex(-0.00180445221193611, 0.00074742857883807),
             complex(0.00074742857883807, 0.00180445221193611)],
            [complex(0.00195312500000000, 0),
             complex(0.00195312500000000, 0),
             complex(0, 0),
             complex(0, 0),
             complex(0.00195312500000000, 0),
             complex(0.00195312500000000, 0),
             complex(0, 0),
             complex(0, 0)],
        ]
    ]

    EXP_S2C_VALS_SPARSE_LE192 = [
        [
            [complex(0, 0),
             complex(1, 0),
             complex(0, 0),
             complex(1, 0),
             complex(0, 0),
             complex(0.00000000000000006, 1),
             complex(0, 0),
             complex(0.00000000000000006, 1)],
            [complex(1, 0),
             complex(-0.70710678118654757, -0.70710678118654746),
             complex(1, 0),
             complex(-0.70710678118654757, -0.70710678118654746),
             complex(0.00000000000000006, 1),
             complex(0.70710678118654746, -0.70710678118654757),
             complex(0.00000000000000006, 1),
             complex(0.70710678118654746, -0.70710678118654757)],
            [complex(-0.70710678118654746, 0.70710678118654757),
             complex(0, 0),
             complex(0.70710678118654757, 0.70710678118654746),
             complex(0, 0),
             complex(0.70710678118654757, 0.70710678118654746),
             complex(0, 0),
             complex(-0.70710678118654746, 0.70710678118654757),
             complex(0, 0)],
        ],
        [
            [complex(0, 0),
             complex(0, 0),
             complex(1.00000000000000133, 0),
             complex(1.00000000000000133, 0),
             complex(0, 0),
             complex(0, 0),
             complex(1.00000000000000133, 0),
             complex(1.00000000000000133, 0)],
            [complex(1.00000000000000133, 0),
             complex(1.00000000000000133, 0),
             complex(-0.92387953251128796, -0.38268343236509028),
             complex(0.38268343236509023, -0.92387953251128796),
             complex(1.00000000000000133, 0),
             complex(1.00000000000000133, 0),
             complex(-0.92387953251128796, -0.38268343236509028),
             complex(0.38268343236509023, -0.92387953251128796)],
            [complex(0.92387953251128796, 0.38268343236509028),
             complex(-0.38268343236509023, 0.92387953251128796),
             complex(0, 0),
             complex(0, 0),
             complex(0.92387953251128796, 0.38268343236509028),
             complex(-0.38268343236509023, 0.92387953251128796),
             complex(0, 0),
             complex(0, 0)],
        ],
    ]

    EXP_C2S_VALS_SPARSE_ABOVE_192 = [
        [
            [complex(0, 0),
             complex(0.00034526698300124, -0.00034526698300124),
             complex(0, 0),
             complex(0.00034526698300124, -0.00034526698300124),
             complex(0, 0),
             complex(-0.00034526698300124, -0.00034526698300124),
             complex(0, 0),
             complex(-0.00034526698300124, -0.00034526698300124)],
            [complex(0.00048828125000000, 0),
             complex(-0.00034526698300124, 0.00034526698300124),
             complex(0.00048828125000000, 0),
             complex(-0.00034526698300124, 0.00034526698300124),
             complex(0, -0.00048828125000000),
             complex(0.00034526698300124, 0.00034526698300124),
             complex(0, -0.00048828125000000),
             complex(0.00034526698300124, 0.00034526698300124)],
            [complex(0, -0.00048828125000000),
             complex(0, 0),
             complex(0.00048828125000000, 0),
             complex(0, 0),
             complex(0.00048828125000000, 0),
             complex(0, 0),
             complex(0, -0.00048828125000000),
             complex(0, 0)],
        ],
        [
            [complex(0, 0),
             complex(0, 0),
             complex(0.00045111305298403, -0.00018685714470952),
             complex(-0.00018685714470952, -0.00045111305298403),
             complex(0, 0),
             complex(0, 0),
             complex(0.00045111305298403, -0.00018685714470952),
             complex(-0.00018685714470952, -0.00045111305298403)],
            [complex(0.00048828125000000, 0),
             complex(0.00048828125000000, 0),
             complex(-0.00045111305298403, 0.00018685714470952),
             complex(0.00018685714470952, 0.00045111305298403),
             complex(0.00048828125000000, 0),
             complex(0.00048828125000000, 0),
             complex(-0.00045111305298403, 0.00018685714470952),
             complex(0.00018685714470952, 0.00045111305298403)],
            [complex(0.00048828125000000, 0),
             complex(0.00048828125000000, 0),
             complex(0, 0),
             complex(0, 0),
             complex(0.00048828125000000, 0),
             complex(0.00048828125000000, 0),
             complex(0, 0),
             complex(0, 0)],
        ]
    ]

    EXP_S2C_VALS_SPARSE_ABOVE_192 = [
        [
            [complex(0, 0),
             complex(1, 0),
             complex(0, 0),
             complex(1, 0),
             complex(0, 0),
             complex(0.00000000000000006, 1),
             complex(0, 0),
             complex(0.00000000000000006, 1)],
            [complex(1, 0),
             complex(-0.70710678118654757, -0.70710678118654746),
             complex(1, 0),
             complex(-0.70710678118654757, -0.70710678118654746),
             complex(0.00000000000000006, 1),
             complex(0.70710678118654746, -0.70710678118654757),
             complex(0.00000000000000006, 1),
             complex(0.70710678118654746, -0.70710678118654757)],
            [complex(-0.70710678118654746, 0.70710678118654757),
             complex(0, 0),
             complex(0.70710678118654757, 0.70710678118654746),
             complex(0, 0),
             complex(0.70710678118654757, 0.70710678118654746),
             complex(0, 0),
             complex(-0.70710678118654746, 0.70710678118654757),
             complex(0, 0)],
        ],
        [
            [complex(0, 0),
             complex(0, 0),
             complex(1.00000000000000133, 0),
             complex(1.00000000000000133, 0),
             complex(0, 0),
             complex(0, 0),
             complex(1.00000000000000133, 0),
             complex(1.00000000000000133, 0)],
            [complex(1.00000000000000133, 0),
             complex(1.00000000000000133, 0),
             complex(-0.92387953251128796, -0.38268343236509028),
             complex(0.38268343236509023, -0.92387953251128796),
             complex(1.00000000000000133, 0),
             complex(1.00000000000000133, 0),
             complex(-0.92387953251128796, -0.38268343236509028),
             complex(0.38268343236509023, -0.92387953251128796)],
            [complex(0.92387953251128796, 0.38268343236509028),
             complex(-0.38268343236509023, 0.92387953251128796),
             complex(0, 0),
             complex(0, 0),
             complex(0.92387953251128796, 0.38268343236509028),
             complex(-0.38268343236509023, 0.92387953251128796),
             complex(0, 0),
             complex(0, 0)],
        ]
    ]
    exp_conf_full_c2s = MockCSCtx(True, 3, 1, 2, 2, 3, False, 0, 0, 0, 1)
    exp_conf_full_s2c = MockCSCtx(False, 3, 1, 2, 2, 3, False, 0, 0, 0, 1)
    exp_conf_sparse_c2s = MockCSCtx(True, 2, 1, 2, 2, 3, False, 0, 0, 0, 1)
    exp_conf_sparse_s2c = MockCSCtx(False, 2, 1, 2, 2, 3, False, 0, 0, 0, 1)

    @pytest.mark.parametrize("slots, lvl_budget, dims, hw, exp_conf_c2s, exp_conf_s2c, exp_c2s, exp_s2c", [
        (8, [3, 3], [0, 0], 0, exp_conf_full_c2s, exp_conf_full_s2c,
         EXP_C2S_VALS_FULL_GT192, EXP_S2C_VALS_FULL_GT192),
        (8, [3, 3], [0, 0], 192, exp_conf_full_c2s, exp_conf_full_s2c,
         EXP_C2S_VALS_FULL_LE192, EXP_S2C_VALS_FULL_LE192),
        (4, [3, 3], [0, 0], 0, exp_conf_sparse_c2s, exp_conf_sparse_s2c,
         EXP_C2S_VALS_SPARSE_ABOVE_192, EXP_S2C_VALS_SPARSE_ABOVE_192),   # sparse
        (4, [3, 3], [0, 0], 192, exp_conf_sparse_c2s, exp_conf_sparse_s2c,
         EXP_C2S_VALS_SPARSE_LE192, EXP_S2C_VALS_SPARSE_LE192),  # sparse
    ])
    def test_initializations(self, slots, lvl_budget, dims, hw, exp_conf_c2s, exp_conf_s2c, exp_c2s, exp_s2c):
        print("\nTesting with parameters:",
              f"slots={slots}"
              f"lvl_budget={lvl_budget}",
              f"dims={dims}",
              f"hw={hw}")
        # Setup with given parameters
        self.setup(slots, lvl_budget, dims, hw)

        # Verify parameters were passed correctly to MockContext
        assert self.ctx._slots == slots
        assert self.ctx._lvl_bdg == lvl_budget
        assert self.ctx._dims == dims
        assert self.ctx._hw == hw

        # Verify context objects were created
        exp_conf_c2s.match(self, self.c2s_ctx)
        exp_conf_s2c.match(self, self.s2c_ctx)

        # Verify basic properties
        assert self.c2s_ctx._plain_vals is not None
        assert self.s2c_ctx._plain_vals is not None

        assert self.compare_complex_lists(self.c2s_ctx._plain_vals, exp_c2s)
        assert self.compare_complex_lists(self.s2c_ctx._plain_vals, exp_s2c)


if __name__ == "__main__":
    pytest.main()
