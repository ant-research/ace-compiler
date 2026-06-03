//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_ANT_UTIL_INCLUDE_FHE_STD_PARAM_IMPL_H
#define RTLIB_ANT_UTIL_INCLUDE_FHE_STD_PARAM_IMPL_H

#include "util/std_param.h"
#include "util/type.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @brief Get default primes from given parameters
//! @param primes default primes
//! @param l security level
//! @param poly_degree degree of polynomial
//! @param num_q_primes number of q primes
void Get_default_primes(VALUE_LIST* primes, SECURITY_LEVEL l,
                        uint32_t poly_degree, size_t num_q_primes);

//! @brief Get root of unity with the given order in the given prime modulus.
//! @param order order n of the root of unity
//! @param prime given modulus
int64_t Get_rou(int64_t order, int64_t prime_value);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_ANT_UTIL_INCLUDE_FHE_STD_PARAM_IMPL_H
