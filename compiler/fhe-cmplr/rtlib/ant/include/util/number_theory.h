//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_ANT_INCLUDE_UTIL_NUMBER_THEORY_H
#define RTLIB_ANT_INCLUDE_UTIL_NUMBER_THEORY_H

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "util/modular.h"
#include "util/type.h"

// A module with number theory functions necessary for other functions.

#ifdef __cplusplus
extern "C" {
#endif

//! @brief Non-Prime version mod_inv
//! @param val value to find the inverse of
//! @param modulus modulus where computation is performed
//! @return int64_t
int64_t Mod_inv(int64_t val, MODULUS* modulus);

//! @brief Generate index for automorphism in given modulus
//! @param rot_idx input rotation idx
//! @param modulus given modulus
//! @return uint32_t
uint32_t Find_automorphism_index(int32_t rot_idx, MODULUS* modulus);

//! @brief Precomputed order for automorphism
//! @param precomp precomputed value list
//! @param k index of automorphism
//! @param n degree
//! @param is_ntt is ntt or not
void Precompute_automorphism_order(VALUE_LIST* precomp, uint32_t k, uint32_t n,
                                   bool is_ntt);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_ANT_INCLUDE_UTIL_NUMBER_THEORY_H
