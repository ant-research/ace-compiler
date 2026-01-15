//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_PLAIN_DATA_H
#define RTLIB_INCLUDE_PLAIN_DATA_H

#include "ckks/encoder.h"
#include "ckks/plaintext.h"
#include "context/ckks_context.h"
#include "lpoly/poly.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef PLAINTEXT* PLAIN;

//! @brief Encode plaintext from input float value list
//! @param len length of input float vector
//! @param level level of plaintext: num of q primes
void Encode_float(PLAIN plain, float* input, size_t len, uint32_t sc_degree,
                  uint32_t level);

//! @brief Encode plaintext from input double value list
//! @param len length of input double vector
//! @param level level of plaintext: num of q primes
void Encode_double(PLAIN plain, double* input, size_t len, uint32_t sc_degree,
                   uint32_t level);

//! @brief Encode plaintext for mask with float value
//! @param len length of non-zero values in mask vector
//! @param level level of plaintext: num of q primes
void Encode_float_mask(PLAIN plain, float cst, size_t len, uint32_t sc_degree,
                       uint32_t level);

//! @brief Encode plaintext for mask with double value
//! @param len length of non-zero values in mask vector
//! @param level level of plaintext: num of q primes
void Encode_double_mask(PLAIN plain, double cst, size_t len, uint32_t sc_degree,
                        uint32_t level);

//! @brief Encode plaintext from input float value list with scale
void Encode_float_with_scale(PLAIN plain, float* input, size_t len,
                             double scale, uint32_t level);

//! @brief Encode single float value to scaled value list with input sc_degree &
//! level
//! @param res pointer to the int64_t array
//! @param level encode level, which also determines the array size.
void Encode_single_float(int64_t* res, float input, uint32_t sc_degree,
                         uint32_t level);

//! @brief Encode single double value to scaled value list with input sc_degree
//! & level
//! @param res pointer to the int64_t array
//! @param level encode level, which also determines the array size.
void Encode_single_double(int64_t* res, double input, uint32_t sc_degree,
                          uint32_t level);

//! @brief Get the message(only real part) content obtained by decoding
//! plaintext
double* Get_msg_from_plain(PLAIN plain);

//! @brief Get the DCMPLX message(with imag part) content obtained by decoding
//! plaintext
DCMPLX* Get_dcmplx_msg_from_plain(PLAIN plain);

//! @brief Get lpoly from PLAINTEXT at given idx
//! need free data by Free_lpoly_data().
L_POLY Lpoly_from_plain(PLAIN plain, size_t idx);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_PLAIN_DATA_H
