//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_ANT_UTIL_INCLUDE_NUMBER_THEORY_IMPL_H
#define RTLIB_ANT_UTIL_INCLUDE_NUMBER_THEORY_IMPL_H

#include <stdbool.h>

#include "util/bignumber.h"
#include "util/modular.h"

// A module with number theory functions necessary for other functions.

#ifdef __cplusplus
extern "C" {
#endif

//! @brief Fast computes an exponent in a modulus
//! raises val to power exp in the modulus without overflowing.
//! @param exp exponent
int64_t Fast_mod_exp(int64_t val, int64_t exp, MODULUS* modulus);

//! @brief Computes an exponent in a modulus
//! @param exp exponent
int64_t Mod_exp(int64_t val, int64_t exp, int64_t modulus);

//! @brief Find an inverse in a given prime modulus
int64_t Mod_inv_prime(int64_t val, MODULUS* modulus);

//! @brief Find an inverse in a given prime modulus for bigint
//! @param val value to find the inverse of
void Bi_mod_inv(BIG_INT result, BIG_INT val, int64_t modulus);

//! @brief Find a generator in the given modulus
//! finds a generator, or primitive root, in the given prime modulus
int64_t Find_generator(MODULUS* modulus);

//! @brief Find a root of unity in the given modulus
//! find a root of unity with the given order in the given prime modulus.
//! @param order order n of the root of unity(an nth root of unity).
int64_t Root_of_unity(int64_t order, MODULUS* modulus);

//! @brief Determines whether a number is prime
//! runs the Miller-Rabin probabilistic primality test many times on the given
//! number
bool Is_prime(int64_t number);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_ANT_UTIL_INCLUDE_NUMBER_THEORY_IMPL_H
