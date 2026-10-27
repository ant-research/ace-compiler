#!/usr/bin/env python3
#
# Copyright (c) Ant Group Co., Ltd
# SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception

"""
Provides lower ckks bootstrap op.
"""
# flake8: noqa: T484
from .bootstrap_ctx import *
import pydsl.ckks as ckks   # noqa T484
import sys
import math
from pathlib import Path
from air_dsl import *  # noqa F403
from pydsl.compiler import py_compile, FunctionScopeManager   # noqa T484

sys.path.append(str(Path(__file__).parent / '.'))


def kernel_impl():
    '''
    implementation of the kernel
    '''

    def __main__(c_in, level_out):
        '''
        implementation of the bootstrap
        '''
        # step1: raise ciphertext modulus
        l_rise = level_out + btsctx._bts_depth
        c_rise = ckks.RaiseMod(c_in, l_rise)

        if (btsctx._slots == btsctx._m / 4):
            # step2: coeff to slots - encode (fully packed)
            c_enc = coeff2slot(c_rise)

            c_conj = ckks.Conjugate(c_enc)
            c_sub = ckks.Sub(c_enc, c_conj)  # clear real part, double imag part
            c_enc = ckks.Add(c_enc, c_conj)  # clear imag part, double real part
            c_sub = ckks.MulMono(c_sub, 3 * btsctx._m / 4)
            for i in range(btsctx._cs._rs_cnt):
                c_sub = ckks.Rescale(c_sub)
                c_enc = ckks.Rescale(c_enc)

            # step3: approx mod reduction chebyshev + double angle
            # approx mode reductin for c_enc and c_sub
            c_enc = approx_mod(c_enc)
            c_sub = approx_mod(c_sub)

            c_sub = ckks.MulMono(c_sub, btsctx._m / 4)
            c_enc = ckks.Add(c_enc, c_sub)

            # step4: slots to coeffs
            c_dec = slot2coeff(c_enc)
        else:
            # sparsely packed
            # step1: Running PartialSum
            for j in bts._psum_rot_idx:
                c_tmp = ckks.Rotate(c_rise, j)
                c_rise = ckks.Add(c_tmp, c_rise)

            # step2: Running coeffs to slots
            c_enc = coeffs2slot(c_rise)
            # double real part, clear imag part
            c_conj = ckks.Conjugate(c_enc)
            c_enc = ckks.Add(c_enc, c_conj)

            for i in range(btsctx._cs._rs_cnt_sparse):
                c_enc = ckks.Rescale(c_enc)

            # step3: approximate reduction
            c_apx = approx_mod(c_enc)

            # step4: Running slots to coeffs
            c_dec = slot2coeff(c_apx)
            c_tmp = ckks.Rotate(c_dec, btsctx._slots)
            c_dec = ckks.Add(c_dec, c_tmp)

        # step5: recover msg and clear image on demand
        if btsctx._clear_image and btsctx._q0_sf_ratio >= 1:
            # use conjugate + add to clear the image part after slot2coeff.
            # As real part will be doubled, 0.5 should be multiplied to the result,
            # we can incorporate the value with deg. Mul_integer is no-need to
            # perform if deg = 1 (for ex: q0=60, sf=59)
            c_conj = ckks.Conjugate(c_dec)
            c_temp = ckks.Add(c_dec, c_conj)
            q0_sf_ratio = math.pow(2., btsctx._q0_sf_ratio - 1)
            if q0_sf_ratio > 1:
                # mul q0/sf to transform (msg / (q0/sf)) to msg
                c_out = ckks.Mul(c_temp, q0_sf_ratio)
        else:
            q0_sf_ratio = math.pow(2., btsctx._q0_sf_ratio)
            c_out = ckks.Mul(c_dec, q0_sf_ratio)
        return c_out

    def matmul(c_in: Cipher, baby_step: int, gaint_step: int,
               num_rots: int, rot_in: list[int], rot_out: list[int],
               weight: list[list[float]]) -> Cipher:  # noqa T484
        c_rot = []  # noqa T484
        for j in range(gaint_step):
            c_rot[j] = ckks.Rotate(c_in, rot_in[j])
        t_input = ckks.Type(c_in)
        c_out = ckks.Zero(t_input)
        for i in range(baby_step):
            c_inner = ckks.Mul(c_rot[0], weight[i])
            for j in range(gaint_step):
                pl_idx = gaint_step * i + j
                if pl_idx != num_rots:
                    c_tmp1 = ckks.Mul(c_rot[j], weight[pl_idx])
                    c_inner = ckks.Add(c_inner, c_tmp1)
            c_tmp2 = ckks.Rotate(c_inner, rot_out[i])
            c_out = ckks.Add(c_out, c_tmp2)
        return c_out

    def coeff2slot(c_in: Cipher) -> Cipher:  # noqa T484
        c2s_ctx = btsctx._cs
        c_out = c_in
        for s in range(c2s_ctx._enc_end, c2s_ctx._enc_start, -1):
            if s != c2s_ctx._lvl_bdg - 1:
                c_out = ckks.Rescale(c_out)
            c_out = matmul(c_out, c2s_ctx._bs, c2s_ctx._gs,
                           c2s_ctx._enc_rot_in[s], c2s_ctx._enc_rot_out[s],
                           c2s_ctx._num_rots, c2s_ctx._plain_vals)

        if c2s_ctx._f_rem:
            c_out = ckks.Rescale(c_out)
            c_out = matmul(c_out, c2s_ctx._bs_rem, c2s_ctx._gs_rem,
                           c2s_ctx._enc_rot_in[0], c2s_ctx._enc_rot_out[0],
                           c2s_ctx._num_rots_rem, c2s_ctx._plain_vals)

        return c_out

    def slot2coeff(c_in: Cipher) -> Cipher:
        s2c_ctx = btsctx._sc
        c_out = c_in
        for s in range(s2c_ctx._dec_start, s2c_ctx._dec_end, 1):
            if s != s2c_ctx._lvl_bdg - 1:
                c_out = ckks.Rescale(c_out)
            c_out = matmul(c_out, s2c_ctx._bs, s2c_ctx._gs,
                           s2c_ctx._dec_rot_in[s], s2c_ctx._dec_rot_out[s],
                           s2c_ctx._num_rots, s2c_ctx._plain_vals)

        if s2c_ctx._f_rem:
            s = s2c_ctx._lvl_bdg - 1
            c_out = ckks.Rescale(c_out)
            c_out = matmul(c_out, s2c_ctx._bs_rem, s2c_ctx._gs_rem,
                           s2c_ctx._dec_rot_in[s], s2c_ctx._dec_rot_out[s],
                           s2c_ctx._num_rots_rem, s2c_ctx._plain_vals)

        return c_out

    def approx_mod(c_in: Cipher) -> Cipher:
        cheby_ctx = btsctx._cheby
        # for even polynomial, perform coordinate transformation: y= x-1/(4*K)
        if cheby_ctx.is_even_poly():
            c_in = ckks.Add(c_in, -1.0 / 4.0 * cheby_ctx._pinfo._upper_bound)
        # TODO: support evaluate get_degree value
        # d_coeffs = get_degree(btsctx._coeffs)
        d_coeffs = cheby_ctx._d_coeffs
        if d_coeffs < 5:
            c_cheby = chebyshev_linear(c_in)
        else:
            c_cheby = chebyshev_ps(c_in)

        # apply double angle
        for i in range(1, cheby_ctx._num_iter + 1):
            c_temp = ckks.Mul(c_cheby, c_cheby)
            c_temp = ckks.Rescale(c_temp)
            c_temp = ckks.Add(c_temp, c_temp)
            c_cheby = ckks.Add(c_temp,  cheby_ctx._dangle_adj[i])

    def chebyshev_ps(c_in: Cipher) -> Cipher:
        cheby_ctx = btsctx._cheby
        k = cheby_ctx._k  # Base degree for polynomial decomposition
        m = cheby_ctx._m  # Number of decomposition levels
        c_klist = []  # Precomputed Chebyshev polynomials T_{1},...,T_{k}
        c_mlist = []  # Precomputed scaled Chebyshev terms T_{2k},T_{4k}, .., T_{2^{m -1}k}

        # no linear transformations are needed for Chebyshev series
        # as the range has been normalized to [-1,1]

        c_klist[0] = c_in

        # Step 1: compute k part T_1(y), T_2(y), ..., T_k(y), where T_i(y) = c_klist[i-1]
        # for even i, T_i(y) = 2 * T_{i/2}(y) ^2 - 1
        for i in range(2, k+1, 2):
            j = i - 1
            if cheby_ctx._is_even_poly() and (i/2 % 2 == 1):
                # T_i(y) = 2*T_{i/2+1}(y) * T_{i/2-1}(y) - T_2(y)
                c_prod = ckks.Mul(c_klist[i/2], c_klist[i/2 - 2])
                c_temp = ckks.Add(c_prod, c_prod)
                c_klist[j] = ckks.sub(c_temp, c_klist[1])
            else:
                c_prod = ckks.Mul(c_klist[i/2 - 1], c_klist[i/2 - 1])
                c_temp = ckks.Add(c_prod, c_prod)
                c_klist[j] = ckks.Add(c_temp, -1.0)

        # for odd i, T_i(y) = 2 *  T_{i/2}(y) * T_{i/2 + 1}(y) - T_1(y)
        # for even polynomial, there is no odd terms
        if not cheby_ctx._is_even_poly():
            for i in range(3, k+1, 2):
                j = i - 1
                c_prod = ckks.Mul(c_klist[i/2 - 1], c_klist[i/2])
                c_temp = ckks.Add(c_prod, c_prod)
                c_klist[j] = ckks.sub(c_temp, c_klist[0])

        # Adjust T_i levels to the level of T_k
        l_last = ckks.Level(c_klist[k-1])
        for i in range(1, k):
            j = i - 1
            if not (cheby_ctx._is_even_poly() and i % 2 == 1):
                l_j = ckks.Level(c_klist[j])
                l_diff = l_j - l_last
                for d in range(1, l_diff + 1):
                    c_klist[j] = ckks.Modswitch(c_klist[j])

        # Step 2: compute m part T_2k(y), T_4k(y), ..., T_2^{m-1}k(y)
        # to compose the higher degree, where T_2^{i}k(y) = c_mlist[i]
        # i = (0,1,...,m-1)
        # T_{2^{i}k} = 2 * T_{2^{i-1}k} * T_{2^{i-1}k} - 1.0
        c_mlist[0] = c_klist[k-1]
        for i in range(1, m):
            c_prod = ckks.Mul(c_mlist[i-1], c_mlist[i-1])
            c_temp = ckks.Add(c_prod, c_prod)
            c_temp = ckks.Rescale(c_temp)
            c_mlist[i] = ckks.Add(c_temp, -1.0)

        # Step 3: compute T_{(2^m -1)k}(y)
        # T_{(2^i - 1)k} = 2*T_{(2^{i-1}-1)k}(y) * T_{(2^{i}k}(y) - T_{k}(y)
        # where T_{k{2^i - 1}} = c_mlist, i = (0,2,...,m-1)
        c_t2km1 = c_mlist[0]
        for i in range(1, m):
            c_prod = ckks.Mul(c_t2km1, c_mlist[i])
            c_temp = ckks.Add(c_prod, c_prod)
            c_temp = ckks.Rescale(c_temp)
            c_t2km1 = ckks.Sub(c_t2km1, c_mlist[0])

        # Step 4: Recursively decomposes high-degree polynomial evaluation
        # into smaller components by Paterson-Stockmeyer algorithm
        rec_idx = [1]
        rec_idx[0] = 0
        c_out = inner_chebyshev_ps(c_in, cheby_ctx._f2, k, m,
                                   c_klist, c_mlist, rec_idx)
        c_out = ckks.Sub(c_out, c_t2km1)
        return c_out

    def inner_chebyshev_ps(c_in, coeffs, k, m, c_klist, c_mlist, rec_idx):
        '''
        Recursively decomposes high-degree polynomial evaluation into 
        smaller components by Paterson-Stockmeyer algorithm
        '''
        div_q = btsctx._cheby._divq[rec_idx[0]]
        divr2_q = btsctx._cheby._divr2_q[rec_idx[0]]
        s2 = btsctx._cheby._s2[rec_idx[0]]

        # Step 1: compute c_cu (Coefficient component evaluated at u)
        # Evaluates the quotient part (divr2_q) using linear
        # combination of precomputed Chebyshev terms
        d_cu = divr2_q.degree
        c_cu = 0  # Coefficient component
        for i in range(0, d_cu):
            c_temp = ckks.Mul(c_klist[i], divr2_q.coeffs[i])
            c_cu = ckks.Add(c_cu, c_temp)
            c_cu = ckks.Rescale(c_cu)
        c_cu = ckks.Add(c_cu, divr2_q.coeffs[0]/2)

        # Step 2: compute c_qu (Quotient polynomial evaluated at u)
        # Recursively evaluates the quotient polynomial (div_q)
        # if its degree is larger than k, otherwise uses Eval_quot_or_rem.
        d_qu = div_q.degree
        if d_qu > k:
            rec_idx[0] += 1
            c_qu = inner_chebyshev_ps(c_in, div_q.coeffs, k, m - 1,
                                      c_klist, c_mlist, rec_idx)
        else:
            # Eval_quot_or_rem
            d_qu_k = div_q.degree
            c_qu = 0
            if d_qu_k > 0:
                for i in range(0, d_qu_k):
                    c_temp = ckks.Mul(c_klist[i], div_q.coeffs[i+1])
                    c_qu = ckks.Add(c_qu, c_temp)
                    c_qu = ckks.Rescale(c_qu)
                if rec_idx[0] > 0:
                    c_sum = c_klist[k - 1]
                    for i in (0, math.log2(div_q.coeffs[-1])):
                        c_sum = ckks.Add(c_sum, c_sum)
                    c_qu = ckks.Add(c_qu, c_sum)
                else:
                    # the highest order coefficient will always be 2 after
                    # one division because of the Chebyshev division rule
                    c_qu = ckks.Add(c_qu, c_klist[k - 1])
                    c_qu = ckks.Add(c_qu, c_klist[k - 1])
            else:
                c_qu = c_klist[k - 1]
                if rec_idx[0] > 0:
                    end = math.log2(div_q.coeffs[-1])
                else:
                    end = div_q.coeffs[-1]
                for i in (0, end):
                    c_qu = ckks.Add(c_qu, c_klist[k-1])
            c_qu = ckks.Add(c_qu, div_q.coeffs[0]/2)

        # Step 3: compute c_su(Adjusted remainder polynomial evaluated at u)
        # Recursively evaluates the quotient polynomial (s2)
        # if its degree is larger than k, otherwise uses Eval_quot_or_rem.
        d_su = s2.degree
        if d_su > k:
            rec_idx[0] += 1
            c_su = inner_chebyshev_ps(c_in, s2.coeffs, k, m - 1,
                                      c_klist, c_mlist, rec_idx)
        else:
            # Eval_quot_or_rem
            d_su_k = s2.degree
            c_su = 0
            for i in range(0, d_su_k):
                c_temp = ckks.Mul(c_klist[i], s2.coeffs[i+1])
                c_su = ckks.Add(c_su, c_temp)
                c_su = ckks.Rescale(c_su)
            c_su = ckks.Add(c_su, c_klist[k-1])
            c_su = ckks.Add(c_su, s2.coeffs[0]/2)

        # Step 4: Combine polynomials
        c_out = ckks.Add(c_mlist[-1], c_cu)  # Adjust level for c_cu?
        c_out = ckks.Mul(c_out, c_qu)
        c_out = ckks.Rescale(c_out)
        c_out = ckks.Add(c_out, c_su)
        return c_out

    def chebyshev_linear(c_in: Cipher) -> Cipher:
        raise ValueError("not supported")


def bootstrap(func_scope, ctx, node, outvar, c_in, level_out):
    '''
    compile the Kernel_impl
    '''
    btsctx = BtsCtx(32, 64, 192, [3, 3], [0, 0], 1152921504606845473,
                    72057594037927936.0, True, True)
    scope_manager = FunctionScopeManager()
    scope_manager.fs = func_scope
    scope_manager.node_operator = node
    scope_manager.add_exprs([c_in, level_out])
    scope_manager.ctx = ctx
    scope_manager.output_var = outvar
    glob = globals()
    glob["btsctx"] = btsctx
    decorator_f = py_compile(locals(), glob, scope_manager, dump_air=True)(kernel_impl)
    decorator_f()
