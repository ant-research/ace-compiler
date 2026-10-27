#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

import math
import cmath
import warnings
from typing import List, Tuple
from .cheby_ctx import get_eval_sin_upper_bound_k  # noqa T484

"""
Provides preocmputed parameters for bootstrap
"""


class CoeffSlotCtx:
    """Context tracking parameters for coefficient-to-slot transformation in CKKS bootstrapping.

    Attributes:
        _rs_cnt (int): Rescale count - number of rescales performed after coeffs to slots
        _gs (int): Giant steps - the giant step in the baby-step giant-step strategy
        _bs (int): Baby steps - the baby step in the baby-step giant-step strategy
        _f_rem (bool): Flag indicating if residual budget exists after transformation
        _bs_rem (int): Remaining budget steps after current allocation
        _gs_rem (int): Remaining grow steps after current allocation
        _enc_start (int): Starting index for encoding polynomial evaluation
        _enc_end (int): Ending index for encoding polynomial evaluation
        _enc_rot_in (List[List[int]]): Input rotation indices for encoding steps
        _enc_rot_out (List[List[int]]): Output rotation indices for encoding steps
        _dec_start (int): Starting index for decoding polynomial evaluation
        _dec_end (int): Ending index for decoding polynomial evaluation
        _dec_rot_in (List[List[int]]): Input rotation indices for decoding steps
        _dec_rot_out (List[List[int]]): Output rotation indices for decoding steps
        _num_rots (int): Total number of rotations required
        _num_rots_rem (int): Number of remaining rotations after current allocation
        _plain_vals (List[List[complex]]): Plaintext coefficients storage (2D array)
    """
    _encoding: bool
    _lvl_budget: int
    # BSGS config
    _layers: int
    _gs: int
    _bs: int
    _num_rots: int
    # BSGS rem config
    _f_rem: bool
    _layers_rem: int
    _bs_rem: int
    _gs_rem: int
    _num_rots_rem: int
    # encoding config
    _enc_start: int
    _enc_end: int
    _enc_rot_in: List[List[int]]
    _enc_rot_out: List[List[int]]
    # decoding config
    _dec_start: int
    _dec_end: int
    _dec_rot_in: List[List[int]]
    _dec_rot_out: List[List[int]]

    _plain_vals: List[List[List[complex]]]
    _rs_cnt: int

    def __init__(self, ctx, is_c2s):
        ctx.check_config()
        self._encoding = is_c2s
        slots = ctx._slots if ctx._slots else ctx._m // 4
        slots_4 = 4 * slots
        is_sparse = (ctx._m != slots_4)
        is_lt_bts = (ctx._lvl_bdg[0] == 1 and ctx._lvl_bdg[1] == 1)
        if is_c2s:
            level_budget = ctx._lvl_bdg[0]
            dim = ctx._dims[0]
        else:
            level_budget = ctx._lvl_bdg[1]
            dim = ctx._dims[1]
        self._lvl_budget = level_budget
        self._plain_vals = None

        # 1. Compute encoding/decoding parameters
        self.set_colls_fft_params(ctx._slots, level_budget, dim)

        # 2. Compute rotation group (5^k mod slots_4)
        rot_group = [pow(5, k, slots_4) for k in range(slots)]

        # 3. Compute ksi powers (roots of unity)
        ksi_pows = [cmath.exp(2j * math.pi * k / slots_4) for k in range(slots_4 + 1)]

        # 4. Compute plain matrices
        if is_lt_bts:
            raise ValueError("lt_bts is not supported")
            # Generate u0, u1 matrices
            u0 = [[0j] * slots for _ in range(slots)]
            u0hat_t = [[0j] * slots for _ in range(slots)]
            val = 1j  # Imaginary unit

            for row in range(slots):
                for col in range(slots):
                    temp_idx = (col * rot_group[row]) % slots_4
                    temp1 = ksi_pows[temp_idx]
                    u0[row][col] = temp1
                    u0hat_t[col][row] = temp1.conjugate()

            if not is_sparse:
                if is_c2s:
                    self.linear_trans_precomp(u0hat_t, scale=1.0)
                else:
                    self.linear_trans_precomp(u0, scale=1.0)
            else:
                # For sparse case generate u1 matrices
                u1 = [[val * ksi_pows[(col * rot_group[row]) % slots_4]
                       for col in range(slots)] for row in range(slots)]
                u1hat_t = [[x.conjugate() for x in row] for row in zip(*u1)]

                if is_c2s:
                    self.linear_trans_precomp_sparse(u0hat_t, u1hat_t, 0)
                else:
                    self.linear_trans_precomp_sparse(u0, u1, 1)

        else:
            # Handle FFT-based coeffs2slots
            self.coeff_slots_precomp(ctx, ksi_pows, rot_group)

    def __str__(self):
        return (
            f"CoeffSlotCtx("
            f"encoding={self._encoding}, "
            f"lvl_budget={self._lvl_budget}, "
            f"layers={self._layers}, "
            f"gs={self._gs}, "
            f"bs={self._bs}, "
            f"num_rots={self._num_rots}, "
            f"flag_rem={self._f_rem}, "
            f"layers_rem={self._layers_rem}, "
            f"gs_rem={self._gs_rem}, "
            f"bs_rem={self._bs_rem}, "
            f"num_rot_rem={self._num_rots_rem}"
            f")"
        )

    def set_colls_fft_params(self, slots: int, level_budget: int, dim: int):
        """Python implementation of Get_colls_fft_params from bootstrap.c
        Set params for encode and decode

        Args:
            slots: Number of slots in the ciphertext
            level_budget: Number of levels allocated for the transformation
            dim: Inner dimension parameter from precomputation
        """
        # Calculate layer distribution using select_layers
        log_slots = int(math.log2(slots))
        layers_coll, _, rem_coll = self.select_layers(log_slots, level_budget)

        # Calculate number of rotations and remainder rotations
        num_rot = (1 << (layers_coll + 1)) - 1
        num_rot_rem = (1 << (rem_coll + 1)) - 1 if rem_coll else 1

        # Calculate baby-step (b) and giant-step (g) parameters
        if dim == 0 or dim > num_rot:
            if num_rot > 7:
                g = 1 << ((layers_coll // 2) + 2)
            else:
                g = 1 << ((layers_coll // 2) + 1)
        else:
            g = dim

        b = (num_rot + 1) // g

        # Calculate remainder parameters if needed
        b_rem, g_rem = 0, 0
        flag_rem = True if rem_coll else False

        if flag_rem:
            if num_rot_rem > 7:
                g_rem = 1 << ((rem_coll // 2) + 2)
            else:
                g_rem = 1 << ((rem_coll // 2) + 1)
            b_rem = (num_rot_rem + 1) // g_rem

        self._layers = layers_coll
        self._gs = g
        self._bs = b
        self._num_rots = num_rot
        self._f_rem = flag_rem
        self._layers_rem = rem_coll
        self._gs_rem = g_rem
        self._bs_rem = b_rem
        self._num_rots_rem = num_rot_rem

    def select_layers(self, log_slots: int, budget: int) -> Tuple[int, int, int]:
        layers = math.ceil(log_slots / budget)
        rows = log_slots // layers
        rem = log_slots % layers
        dim = rows + 1 if rem else rows

        # above choice ensures dim <= budget
        if dim < budget:
            layers -= 1
            rows = log_slots // layers
            rem = log_slots - rows * layers
            dim = rows + 1 if rem else rows

            # above choice ensures dim >= budget
            while dim != budget:
                rows -= 1
                rem = log_slots - rows * layers
                dim = rows + 1 if rem else rows

        return layers, rows, rem

    def get_list_shape(self, lst):
        if not isinstance(lst, List):
            return []

        shape = []
        current_level = lst

        while isinstance(current_level, List):
            shape.append(len(current_level))

            # Move to the next level if possible; otherwise, break
            if len(current_level) > 0 and isinstance(current_level[0], List):
                current_level = current_level[0]
            else:
                break

        return shape

    def merge(self, lst1, lst2):
        """
        Merges two multi-dimensional lists at their last dimension.
        Both lists should have the same shape except for the last dimension.
        """

        # Ensure the lists are not empty
        if not lst1:
            return lst2
        if not lst2:
            return lst1

        # If elements are lists, we need to go deeper
        if isinstance(lst1[0], list) and isinstance(lst2[0], list):
            return [self.merge(sub1, sub2) for sub1, sub2 in zip(lst1, lst2)]

        # Otherwise, we're at the last dimension, so concatenate
        return lst1 + lst2

    def coeff_slots_precomp(self, ctx, ksi_pows: List[complex],
                            rot_group: List[int]):
        """Python implementation of Coeffs2slots_precomp from bootstrap.c

        Args:
            bts_ctx: Bootstrapping context dictionary
            ksi_pows: List of complex roots of unity
            rot_group: Rotation group indices

        Returns:
            2D List of precomputed plaintexts values for coefficient-to-slot transformation
        """
        slots = len(rot_group)
        m = ctx._m
        level_budget = self._lvl_budget

        # 1. Compute coefficients collapse
        if slots == m // 4:
            coeffs = self.coeff_collapse(ksi_pows, rot_group, level_budget, False)
        else:
            coeffs1 = self.coeff_collapse(ksi_pows, rot_group, level_budget, False)
            coeffs2 = self.coeff_collapse(ksi_pows, rot_group, level_budget, True)
            coeffs = self.merge(coeffs1, coeffs2)
        if (len(coeffs) != level_budget):
            warnings.warn("number of decomposed matrix should be level_budget")

        q0 = ctx._q0
        pre = q0 / (1 << round(math.log2(q0)))
        if self._encoding:
            # 2. Apply scaling factors
            # multiply coeffs with (1/N * 1/K * 1/(q0/sf)) to reduce redundant Mul_const
            # and mul_level consumption. N is poly_degree in (inv_SF = (1/N) *
            # trans(conj_SF)). K is norm of |I(X)| in ((q0/sf)*I + m). q0 is first prime,
            # and sf is scale factor. to reduce precision loss in encoding SF,
            # decompose factor into multiple-part, then multiply each part with
            # decomposed matrix of SF.
            factor = (1.0 / (ctx._m / 2)) / get_eval_sin_upper_bound_k(ctx._hw, ctx._even_poly)

            sf = ctx._sf
            q0_sf_ratio = round(math.log2(q0 / sf))
            factor /= 2**q0_sf_ratio
            factor = factor ** (1.0 / level_budget)
            scale = pre

            # Apply factor to all coefficients
            for lvl in range(level_budget):
                for row in range(len(coeffs[lvl])):
                    for col in range(len(coeffs[lvl][row])):
                        coeffs[lvl][row][col] *= factor
        else:
            scale = 1 / pre

        # 3. Precompute rotated plaintexts
        self.rotate_precomp(ctx, coeffs, ksi_pows, scale)

    def coeff_collapse(self, ksi_pows: List[complex], rot_group: List[int],
                       level_budget: int, flag: bool) -> List[List[List[complex]]]:
        """Python implementation of Coeff_collapse from bootstrap.c

        Args:
            flag: that is 0 when we compute the coefficients for U_0 and is 1 for i*U_0.
        """
        slots = len(rot_group)
        log_slots = int(math.log2(slots))
        layers, _, rem = self.select_layers(log_slots, level_budget)

        # Computing the coefficients for encoding for the given level budget
        coeff1 = self.coeff_enc_dec_one_level(ksi_pows, rot_group, flag)

        # Initialize 3D coefficient array
        coeffs: List[List[List[complex]]] = [[] for _ in range(level_budget)]
        rem_idx = -1
        if self._f_rem:
            rem_idx = 0 if self._encoding else level_budget - 1
        for i in range(level_budget):
            if self._f_rem is True:
                dim2 = self._num_rots_rem if i == rem_idx else self._num_rots
            else:
                dim2 = self._num_rots
            coeffs[i] = [[] for _ in range(dim2)]
            for j in range(dim2):
                coeffs[i][j] = [0j] * slots

        for i in range(level_budget):
            top = log_slots - (level_budget - 1 - i) * layers - 1 if self._encoding else i * layers
            end_col = rem if i == rem_idx else layers
            for j in range(end_col):
                if j == 0:
                    coeffs[i][0] = coeff1[top]
                    coeffs[i][1] = coeff1[top + log_slots]
                    coeffs[i][2] = coeff1[top + 2 * log_slots]
                elif self._encoding:
                    t = 0
                    coeff_temp = [[0j for _ in range(len(coeffs[i][0]))]
                                  for _ in range(len(coeffs[i]))]
                    for u in range((1 << (j + 1)) - 1):
                        for k in range(slots):
                            idx = k - (1 << (top - j))
                            rot_idx = self.reduce_rotation(idx, slots)
                            idx2 = k + (1 << (top - j))
                            rot_idx2 = self.reduce_rotation(idx2, slots)
                            coeff_temp[u + t][k] += coeff1[top - j][k] * coeffs[i][u][rot_idx]
                            coeff_temp[u + t + 1][k] += coeff1[top -
                                                               j + log_slots][k] * coeffs[i][u][k]
                            coeff_temp[u + t + 2][k] += coeff1[top - j +
                                                               2 * log_slots][k] * coeffs[i][u][rot_idx2]
                        t += 1
                    coeffs[i] = coeff_temp
                else:
                    coeff_temp = [[0j for _ in range(len(coeffs[i][0]))]
                                  for _ in range(len(coeffs[i]))]
                    for t in range(3):
                        for u in range((1 << (j + 1)) - 1):
                            for k in range(slots):
                                if t == 0:
                                    coeff_temp[u][k] += coeff1[top + j][k] * coeffs[i][u][k]
                                elif t == 1:
                                    coeff_temp[u + (1 << j)][k] += coeff1[top + j +
                                                                          log_slots][k] * coeffs[i][u][k]
                                elif t == 2:
                                    t_dim1 = u + (1 << (j + 1))
                                    s_dim1 = top + j + 2 * log_slots
                                    coeff_temp[t_dim1][k] += coeff1[s_dim1][k] * coeffs[i][u][k]
                    coeffs[i] = coeff_temp
        return coeffs

    def coeff_enc_dec_one_level(self, ksipows, rot_group, flag):
        """
        Python implementation of Coeff_enc_one_level  Coeff_dec_one_level from bootstrap.c
        Each outer iteration from the FFT algorithm can be written a weighted sum
        of three terms: the input shifted right by a power of two, the unshifted
        input, and the input shifted left by a power of two. For each outer
        iteration (log2(size) in total), the matrix coeff stores the coefficients
        in the following order: the coefficients associated to the input shifted
        right, the coefficients for the non-shifted input and the coefficients
        associated to the input shifted left.

        Args:
            ksipows: List[complex] - Precomputed roots of unity (ζ^i values)
            rot_group: List[int] - Rotation group indices
            flag: bool - that is 0 when we compute the coefficients for U_0 and is 1 for i*U_0.
        Returns:
            List[List[complex]] - Coefficient matrix for encoding or decoding
        """
        dim = len(ksipows) - 1
        slots = len(rot_group)
        log_slots = int(math.log2(slots))

        # Initialize coefficient matrix: 3*log_slots rows x slots columns
        coeff = [[0j for _ in range(slots)] for _ in range(3 * log_slots)]

        for m in [slots // (2**i) for i in range(log_slots)]:
            if m < 2:
                continue

            s = int(math.log2(m)) - 1
            lenh = m // 2
            lenq = m * 4

            val = cmath.exp(-1j * math.pi / 2) if self._encoding else cmath.exp(1j * math.pi / 2)
            for k in range(0, slots, m):
                for j in range(lenh):
                    idx = j + k
                    rot_idx = rot_group[j] % lenq
                    j_twiddle = (lenq - rot_idx) * \
                        (dim // lenq) if self._encoding else rot_idx * (dim // lenq)

                    # Calculate base complex value
                    base_val = ksipows[j_twiddle]

                    if flag and m == 2:
                        w = val * base_val
                        val1 = val
                        val2 = val if self._encoding else w
                        val3 = -w
                        val4 = w if self._encoding else val

                        # coeff_logslots not shifted
                        coeff[s + log_slots][idx] = val1
                        # coeff_2logslots shift left
                        coeff[s + 2 * log_slots][idx] = val2
                        # coeff_logslots not shifted
                        coeff[s + log_slots][idx + lenh] = val3
                        # coeff_s shift right
                        coeff[s][idx + lenh] = val4
                    else:
                        w = base_val
                        val1 = 1
                        val2 = 1 if self._encoding else w
                        val3 = -w
                        val4 = w if self._encoding else 1

                        # coeff_logslots  not shifted
                        coeff[s + log_slots][idx] = val1
                        # coeff_2logslots shift left
                        coeff[s + 2 * log_slots][idx] = val2
                        # coeff_logslots not shifted
                        coeff[s + log_slots][idx + lenh] = val3
                        # coeff_s shift right
                        coeff[s][idx + lenh] = val4

        return coeff

    def rotate_precomp(
        self,
        ctx,
        coeffs: List[List[List[complex]]],
        ksipows: List[complex],
        scale: float
    ) -> None:
        """Precompute rotated plaintexts for baby-step giant-step algorithm

        Args:
            bts_ctx: Bootstrapping context with parameters
            coeffs: 3D array of coefficients matrices
            ksipows: Precomputed powers of the root of unity
            rot_group: Rotation group indices
            flag: Special handling flag for sparse cases

        Returns:
            2D List of precomputed plaintexts, a plaintext composed by
            List[complex]
        """
        m = ctx._m
        level_budget = self._lvl_budget

        # Initialize result structure
        self._plain_vals: List[List[List[complex]]] = []
        rem_idx = 0 if self._encoding else level_budget - 1
        for i in range(level_budget):
            # remainder corresponds to index 0 in encoding
            cols = self._num_rots_rem if self._f_rem is True and i == rem_idx else self._num_rots
            row: List[List[complex]] = [] * cols
            self._plain_vals.append(row)

        # Handle remainder flag
        stop = -1
        if self._f_rem is True:
            stop = 0

        # Determine loop bounds based on encoding direction
        start = stop + 1 if self._encoding else 0
        end = level_budget if self._encoding else level_budget - self._f_rem
        cond = start if self._encoding else end - 1

        if len(coeffs) < end - start:
            raise ValueError("invalid coeffs allocated")
        for s in range(start, end):
            # Get coefficient matrix for current level
            plain_dim2 = []
            for i in range(self._bs):
                for j in range(self._gs):
                    dim2 = self._gs * i + j
                    if dim2 == self._num_rots:
                        continue

                    # Calculate rotation parameters
                    shift_val = (s - self._f_rem) * self._layers + \
                        self._layers_rem if self._encoding else s * self._layers

                    # Calculate rotation index
                    idx = -self._gs * i * (1 << shift_val)
                    rot_idx = self.reduce_rotation(idx, m // 4)

                    # Get coefficients to rotate
                    coeff_vector = coeffs[s][dim2]

                    # Apply scaling to first/last set of coefficients
                    # for encoding: do the scaling only at the first set of
                    # coefficients
                    # for decoding: do the scaling only at the last set of
                    # coefficients
                    if (self._f_rem is False) and (s == cond):
                        coeff_vector = [c * scale for c in coeff_vector]

                    # Rotate coefficients
                    rot_idx %= len(coeff_vector)
                    rotated = coeff_vector[rot_idx:] + coeff_vector[:rot_idx]

                    # Store in appropriate position
                    plain_dim2.append(rotated)
            self._plain_vals[s] = plain_dim2

        # Handle remainder case
        if self._f_rem is True:
            s = 0 if self._encoding else level_budget - 1
            shift_val = 1 if self._encoding else (1 << (s * self._layers))
            plain_dim2 = []
            for i in range(self._bs_rem):
                for j in range(self._gs_rem):
                    dim2 = self._gs_rem * i + j
                    if dim2 == self._num_rots_rem:
                        continue
                    idx = -self._gs_rem * i * shift_val
                    rot_idx = self.reduce_rotation(idx, m // 4)

                    # Get coefficients and apply scaling
                    coeff_vector = coeffs[s][dim2]
                    coeff_vector = [c * scale for c in coeff_vector]

                    # Rotate coefficients
                    rot_idx %= len(coeff_vector)
                    rotated = coeff_vector[rot_idx:] + coeff_vector[:rot_idx]
                    # Store in appropriate position
                    plain_dim2.append(rotated)
            self._plain_vals[s] = plain_dim2

    def reduce_rotation(self, index: int, slots: int) -> int:
        """Reduce rotation index to valid range [0, slots)
        Args:
            index: Rotation amount (can be negative)
            slots: Number of slots in the ciphertext

        Returns:
            Positive rotation index within valid range
        """
        if (slots & (slots - 1)) == 0:  # Power-of-two case
            n = int(math.log2(slots))
            if index >= 0:
                return index - ((index >> n) << n)
            else:
                return index + slots + (((-index) >> n) << n)
        return (slots + index % slots) % slots
