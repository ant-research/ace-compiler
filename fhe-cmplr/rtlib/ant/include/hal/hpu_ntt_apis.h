//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_ANT_INCLUDE_HAL_HPU_NTT_APIS_H
#define RTLIB_ANT_INCLUDE_HAL_HPU_NTT_APIS_H

#include "ntt_math.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PE_PER_PEB  16
#define PEB_PER_PEG 16
#define PEG_WIDTH   (PEB_PER_PEG * PE_PER_PEB)

//! @brief function for generating "Negative Cyclic Convolution" twiddle factors
//! in HPU 2d-format.
//! @param poly2d output ncy twiddle factors.
//! @param qs parameter q.
//! @param peg_num the number of pegs.
//! @param poly_num the number of polynomials in each PEG.
//! @param row_length the number of rows of 2d_poly.
//! @param column_length the number of columns of 2d_poly.
//! @param is_inv whether it's intt, if is_inv is true, it will be intt.
//! @param trs whether it's transposed.
void Hpu_ntt_ncy_2d_gen(VV64 poly_2d, U64 qs, U8 peg_num, U8 poly_num,
                        U32 row_length, U64 column_length, bool is_inv,
                        bool trs);

//! @brief function for generating apply-twiddle factors --- w^(ij) in HPU
//! 2d-format.
//! @param poly2d output ncy twiddle factors.
//! @param qs parameter q.
//! @param peg_num the number of pegs.
//! @param poly_num the number of polynomials in each PEG.
//! @param row_length the number of rows of 2d_poly.
//! @param column_length the number of columns of 2d_poly.
//! @param is_inv whether it's intt, if is_inv is true, it will be intt.
//! @param trs whether it's transposed.
void Hpu_ntt_apptwi_2d_gen(VV64 poly_2d, U64 qs, U8 peg_num, U8 poly_num,
                           U32 row_length, U64 column_length, bool is_inv,
                           bool trs);

//! @brief function for generating ntt twiddle factors in HPU 2d-format.
//! @param w_2d_r output twiddle factors in row-direction.
//! @param w_2d_c output twiddle factors in column-direction.
//! @param qs parameter q.
//! @param peg_num the number of pegs.
//! @param poly_num the number of polynomials in each PEG.
//! @param row_length the number of rows of 2d_poly.
//! @param column_length the number of columns of 2d_poly.
//! @param is_inv whether it's intt, if is_inv is true, it will be intt.
//! @param trs whether it's transposed.
void Hpu_w_2d_gen(VV64 w_2d_r, VV64 w_2d_c, U64 qs, U8 peg_num, U8 poly_num,
                  U32 row_length, U64 column_length, bool is_inv, bool trs);

//! @brief function to convert twiddle factors from HPU 2d-format to 1d-format.
//! This function provides a workaround for simplifying the process of using
//! twiddle factors, as the NTT with twiddle factors in HPU 2d-format is too
//! complex. Instead, converting them to 1d-format allow for easier
//! verification.
//! @param w_1d output twiddle factors in 1d-format.
//! @param w_2d input twiddle factors in 2d-format.
//! @param poly_num the number of polynomials in each PEG.
//! @param row_length the number of rows of 2d_poly.
//! @param column_length the number of columns of 2d_poly.
//! @param trs whether it's transposed.
void Hpu_conv_2d_to_1d(V64 w_1d, VV64 w_2d, U8 poly_num, U32 row_length,
                       U64 column_length, bool trs);
#ifdef __cplusplus
}
#endif

#endif