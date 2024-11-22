//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_NUMBER_THEORY_H
#define RTLIB_INCLUDE_NUMBER_THEORY_H

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#include "util/fhe_types.h"
#include "util/fhe_utils.h"

// A module with number theory functions necessary for other functions.

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief prime factors
 *
 */
typedef struct {
  size_t  _len;
  int64_t _factor[];
} PRIME_FACTOR;

/**
 * @brief fast computes an exponent in a modulus
 * raises val to power exp in the modulus without overflowing.
 *
 * @param val value we wish to raise the power of
 * @param exp exponent
 * @param modulus modulus where computation is performed.
 * @return int64_t
 */
int64_t Fast_mod_exp(int64_t val, int64_t exp, MODULUS* modulus);

/**
 * @brief computes an exponent in a modulus
 *
 * @param val value we wish to raise the power of
 * @param exp exponent
 * @param modulus modulus where computation is performed.
 * @return int64_t
 */
int64_t Mod_exp(int64_t val, int64_t exp, int64_t modulus);

/**
 * @brief find an inverse in a given prime modulus
 *
 * @param val value to find the inverse of
 * @param modulus modulus where computation is performed
 * @return int64_t
 */
int64_t Mod_inv_prime(int64_t val, MODULUS* modulus);

/**
 * @brief find an inverse in a given prime modulus for bigint
 *
 * @param result The inverse of the given value in the modulus
 * @param val value to find the inverse of
 * @param modulus modulus where computation is performed
 */
void Bi_mod_inv(BIG_INT result, BIG_INT val, int64_t modulus);

/**
 * @brief Non-Prime version mod_inv
 *
 * @param val value to find the inverse of
 * @param modulus modulus where computation is performed
 * @return int64_t
 */
int64_t Mod_inv(int64_t val, MODULUS* modulus);

/**
 * @brief find a generator in the given modulus
 * finds a generator, or primitive root, in the given prime modulus
 *
 * @param modulus modulus to find the generator in. MUST BE PRIME
 * @return int64_t
 */
int64_t Find_generator(MODULUS* modulus);

/**
 * @brief find a root of unity in the given modulus
 * find a root of unity with the given order in the given prime modulus.
 *
 * @param order order n of the root of unity(an nth root of unity).
 * @param modulus modulus to find the root of unity in. MUST BE PRIME
 * @return int64_t
 */
int64_t Root_of_unity(int64_t order, MODULUS* modulus);

/**
 * @brief determines whether a number is prime
 * runs the Miller-Rabin probabilistic primality test many times on the given
 * number
 *
 * @param number number to perform primality test on
 * @return true if number is prime
 * @return false if number is not prime
 */
bool Is_prime(int64_t number);

/**
 * @brief generate index for automorphism in given modulus
 *
 * @param rot_idx input rotation idx
 * @param modulus given modulus
 * @return uint32_t
 */
uint32_t Find_automorphism_index(int32_t rot_idx, MODULUS* modulus);

/**
 * @brief precomputed order for automorphism
 *
 * @param precomp precomputed value list
 * @param k index of automorphism
 * @param n degree
 * @param is_ntt is ntt or not
 */
void Precompute_automorphism_order(VALUE_LIST* precomp, uint32_t k, uint32_t n,
                                   bool is_ntt);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_NUMBER_THEORY_H
