//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_PLAIN_DATA_H
#define RTLIB_INCLUDE_PLAIN_DATA_H

#include "rtlib/context.h"
#include "util/ckks_encoder.h"
#include "util/crt.h"
#include "util/plaintext.h"
#include "util/polynomial.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef PLAINTEXT* PLAIN;

//! @brief Encode plaintext from input float value list
//! @param len length of input float vector
//! @param level level of plaintext: num of q primes
void Encode_plain_from_float(PLAIN plain, float* input, size_t len,
                             uint32_t sc_degree, uint32_t level);

//! @brief Encode plaintext from input double value list
//! @param len length of input double vector
//! @param level level of plaintext: num of q primes
void Encode_plain_from_double(PLAIN plain, double* input, size_t len,
                              uint32_t sc_degree, uint32_t level);

//! @brief Encode plaintext from input float value list with scale
void Encode_plain_from_float_with_scale(PLAIN plain, float* input, size_t len,
                                        double scale, uint32_t level);

//! @brief Get the message(only real part) content obtained by decoding
//! plaintext
double* Get_msg_from_plain(PLAIN plain);

//! @brief Get the DCMPLX message(with imag part) content obtained by decoding
//! plaintext
DCMPLX* Get_dcmplx_msg_from_plain(PLAIN plain);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_PLAIN_DATA_H
