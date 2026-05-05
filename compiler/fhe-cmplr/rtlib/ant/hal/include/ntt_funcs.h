//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_ANT_HAL_INCLUDE_NTT_FUNCS_H
#define RTLIB_ANT_HAL_INCLUDE_NTT_FUNCS_H

#include "hal/ntt_math.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @brief 1step-ntt in 1d-format
//! @param dst dst polynomial
//! @param src src polynomial
//! @param len length of polynomial
//! @param q the prime
void One_step_ntt(V64 dst, V64 src, U64 len, U64 q);

//! @brief 1step-intt in 1d-format
//! @param dst dst polynomial
//! @param src src polynomial
//! @param len length of polynomial
//! @param q the prime
void One_step_intt(V64 dst, V64 src, U64 len, U64 q);

//! @brief 4step-ntt in 2d-format
//! @param dst dst poly (after 4step-ntt dst 2d fromat was converted from 2d
//! format)
//! @param src src polynomial (before 4step-ntt src 1d format was converted to
//! 2d format)
//! @param w_2d_ncy "Negative Cyclic Convolution" twiddle factors in HPU
//! 2d-format
//! @param w_2d_apptwi apply-twiddle factors --- w^(ij) in HPU 2d-format.
//! @param w_2d_dir_r_1d twiddle factors in row-direction (1d format is
//! converted from HPU 2d-fromat)
//! @param w_2d_dir_c_1d twiddle factors in column-direction (1d format is
//! converted from HPU 2d-fromat)
//! @param rlen length of rows
//! @param clen length of columns
//! @param q the prime
//! @param input_transpose COE format of input data, true means
//! transposed(column-major), false means row-major.
void Four_step_ntt(V64 dst, V64 src, VV64 w_2d_ncy, VV64 w_2d_apptwi,
                   V64 w_2d_dir_r_1d, V64 w_2d_dir_c_1d, U64 rlen, U64 clen,
                   U64 q, bool input_transpose);

//! @brief 4step-intt in 2d-format
//! @param dst dst poly (after 4step-intt dst 2d fromat was converted from 2d
//! format)
//! @param src src polynomial (before 4step-intt src 1d format was converted to
//! 2d format)
//! @param w_2d_ncy "Negative Cyclic Convolution" twiddle factors in HPU
//! 2d-format
//! @param w_2d_apptwi apply-twiddle factors --- w^(ij) in HPU 2d-format.
//! @param w_2d_dir_r_1d twiddle factors in row-direction (1d format is
//! converted from HPU 2d-fromat)
//! @param w_2d_dir_c_1d twiddle factors in column-direction (1d format is
//! converted from HPU 2d-fromat)
//! @param rlen length of rows
//! @param clen length of columns
//! @param q the prime
//! @param input_transpose COE format of input data, true means
//! transposed(column-major), false means row-major
void Four_step_intt(V64 dst, V64 src, VV64 w_2d_ncy, VV64 w_2d_apptwi,
                    V64 w_2d_dir_r_1d, V64 w_2d_dir_c_1d, U64 rlen, U64 clen,
                    U64 q, bool input_transpose);

#ifdef __cplusplus
}
#endif

#endif