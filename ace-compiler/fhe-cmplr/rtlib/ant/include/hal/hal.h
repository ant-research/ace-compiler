//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_ANT_INCLUDE_HAL_HAL_H
#define RTLIB_ANT_INCLUDE_HAL_HAL_H

#include "util/modular.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @brief Mod add of polynomial
//! @param res result value
//! @param val1 input value
//! @param val2 input value
//! @param modulus current Modulus
//! @param degree poly degree
int64_t* Hw_modadd(int64_t* res, int64_t* val1, int64_t* val2, MODULUS* modulus,
                   uint32_t degree);

//! @brief Mod add of polynomial and scalar
//! @param res result value
//! @param val1 input value
//! @param scalar input scalar
//! @param modulus current Modulus
//! @param degree poly degree
int64_t* Hw_modadd_scalar(int64_t* res, int64_t* val1, int64_t scalar,
                          MODULUS* modulus, uint32_t degree);

//! @brief Mod sub of polynomial
//! @param res result value
//! @param val1 input value
//! @param scalar input scalar
//! @param modulus current Modulus
//! @param degree poly degree
int64_t* Hw_modsub(int64_t* res, int64_t* val1, int64_t* val2, MODULUS* modulus,
                   uint32_t degree);

//! @brief Mod sub of polynomial and scalar
//! @param res result value
//! @param val input value
//! @param scalar input scalar
//! @param modulus current Modulus
//! @param degree poly degree
int64_t* Hw_modsub_scalar(int64_t* res, int64_t* val, int64_t scalar,
                          MODULUS* modulus, uint32_t degree);

//! @brief Mod multiply of polynomial
//! @param res result value
//! @param val1 input value
//! @param val2 input value
//! @param modulus current Modulus
//! @param degree poly degree
int64_t* Hw_modmul(int64_t* res, int64_t* val1, int64_t* val2, MODULUS* modulus,
                   uint32_t degree);

//! @brief Mod sub of polynomial and scalar
//! @param res result value
//! @param val input value
//! @param scalar input scalar
//! @param modulus current Modulus
//! @param degree poly degree
int64_t* Hw_modmul_scalar(int64_t* res, int64_t* val, int64_t scalar,
                          MODULUS* modulus, uint32_t degree);

//! @brief Mod multiply add of polynomial
//! @param res result value
//! @param val1 input value
//! @param val2 input value
//! @param modulus current Modulus
//! @param degree poly degree
int64_t* Hw_modmuladd(int64_t* res, int64_t* val1, int64_t* val2,
                      MODULUS* modulus, uint32_t degree);

//! @brief Mod multiply add of polynomial and scalar
//! @param res result value
//! @param val input value
//! @param scalar input scalar
//! @param modulus current Modulus
//! @param degree poly degree
int64_t* Hw_modmuladd_scalar(int64_t* res, int64_t* val, int64_t scalar,
                             MODULUS* modulus, uint32_t degree);

//! @brief HW(Hardware) rotate automorphism transform
//! @param res result value
//! @param val input value
//! @param rot_precomp the precomputed automorphism order
//! @param modulus current Modulus
//! @param degree poly degree
int64_t* Hw_rotate(int64_t* res, int64_t* val, int64_t* rot_precomp,
                   MODULUS* modulus, uint32_t degree);

//! @brief Run NTT of given vector
//! @param res result value
//! @param val input value
//! @param idx idx of primes
//! @param degree poly degree
void Hw_ntt(int64_t* res, int64_t* val, size_t idx, uint32_t degree);

//! @brief Run INTT of given vector
//! @param res result value
//! @param val input value
//! @param idx idx of primes
//! @param degree poly degree
void Hw_intt(int64_t* res, int64_t* val, size_t idx, uint32_t degree);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_ANT_INCLUDE_HAL_HAL_H
