#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

from .cheby_ctx import ChebyCtx  # noqa T484
from .cs_ctx import CoeffSlotCtx  # noqa T484
from typing import List
import warnings
import math


class BtsCtx:
    # Input parameters
    _slots: int  # number of slots for which the bootstrapping is performed
    _m: int  # ring_degree * 2
    _hw: int  # hamming weight
    _even_poly: bool  # is even poly
    _lvl_bdg: List[int]  # Level budget for coeffs-slots transformation
    _dims: List[int]  # inner/outer dimension in the baby-step gaint-step strategy
    _q0: int
    _sf: float
    # if set to [0, 0], the value will be calculated in setup_btsctx
    _clear_image: bool  # clear image after slot2coeff

    # precomputed parameters
    _bts_depth: int  # bootstrap depth
    _q0_sf_ratio: int  # round(log2(q0/sf))
    _cs: CoeffSlotCtx
    _sc: CoeffSlotCtx
    _cheby: ChebyCtx

    def __init__(self, slots, m, hw, level_budget, dims, q0, sf, clear_image, even_poly):
        self._slots = slots
        self._m = m
        self._hw = hw
        self._lvl_bdg = level_budget
        self._dims = dims
        self._clear_image = clear_image
        self._even_poly = even_poly
        self._q0 = q0
        self._sf = sf
        self._q0_sf_ratio = round(math.log2(q0 / sf))
        self.check_config()

        self._cs = CoeffSlotCtx(self, True)
        self._sc = CoeffSlotCtx(self, False)
        self._cheby = ChebyCtx(self, even_poly)
        self._bts_depth = self.compute_bts_depth()
        # precompute for partial sum rotation indexs
        self._psum_rot_idx = []
        j = 1
        while j < m // (4 * self._slots):
            self._psum_rot_idx.append(j * self._slots)
            j = j << 1

    def compute_bts_depth(self):
        approx_depth = self._cheby.get_approx_depth()
        depth = approx_depth + self._lvl_bdg[0] + self._lvl_bdg[1]
        return depth

    def check_config(self):
        if self._slots == 0:
            self._slots = self._m // 4

        log_slots = math.log2(self._slots)
        if log_slots == 0:
            raise ValueError("slots should > 1")

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
