//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_ANT_INCLUDE_UTIL_STD_PARAM_H
#define RTLIB_ANT_INCLUDE_UTIL_STD_PARAM_H

#include <stdint.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

//! @brief Security level
typedef enum {
  HE_STD_128_CLASSIC,
  HE_STD_192_CLASSIC,
  HE_STD_256_CLASSIC,
  HE_STD_NOT_SET,
} SECURITY_LEVEL;

//! @brief Get bits of the default scaling factor
//! @param l security level
//! @param poly_degree degree of polynomial
//! @param num_q_primes number of q primes
size_t Get_default_sf_bits(SECURITY_LEVEL l, uint32_t poly_degree,
                           size_t num_q_primes);

//! @brief Get ciphertext modulus Q bound(for Hybrid: P * Q)
//! @param l security level
//! @param poly_degree degree of polynomial
size_t Get_qbound(SECURITY_LEVEL l, uint32_t poly_degree);

//! @brief Get the max bits of qBound
//! @param l security level
//! @param poly_degree degree of polynomial
size_t Get_max_bit_count(SECURITY_LEVEL l, uint32_t poly_degree);

//! @brief Get the number of parts of q primes
//! @param mult_depth multiply depth
size_t Get_default_num_q_parts(size_t mult_depth);

//! @brief Get security level
//! @param level bits of security against classical computers
SECURITY_LEVEL Get_sec_level(size_t level);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_ANT_INCLUDE_UTIL_STD_PARAM_H
