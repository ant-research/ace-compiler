//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_POLY_ARITH_H
#define RTLIB_INCLUDE_POLY_ARITH_H

#include "util/fhe_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief mod add of polynomial
 *
 * @param res result value
 * @param val1 input value
 * @param val2 input value
 * @param modulus current Modulus
 * @param degree poly degree
 */
int64_t* Hw_modadd(int64_t* res, int64_t* val1, int64_t* val2, MODULUS* modulus,
                   uint32_t degree);

/**
 * @brief mod multiply of polynomial
 *
 * @param res result value
 * @param val1 input value
 * @param val2 input value
 * @param modulus current Modulus
 * @param degree poly degree
 */
int64_t* Hw_modmul(int64_t* res, int64_t* val1, int64_t* val2, MODULUS* modulus,
                   uint32_t degree);

/**
 * @brief HW(Hardware) rotate automorphism transform
 *
 * @param res result value
 * @param val input value
 * @param rot_precomp the precomputed automorphism order
 * @param modulus current Modulus
 * @param degree poly degree
 */
int64_t* Hw_rotate(int64_t* res, int64_t* val, int64_t* rot_precomp,
                   MODULUS* modulus, uint32_t degree);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_POLY_ARITH_H
