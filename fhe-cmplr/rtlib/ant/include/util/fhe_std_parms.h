//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_FHE_STD_PARAMS_H
#define RTLIB_INCLUDE_FHE_STD_PARAMS_H

#include "util/fhe_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief security level
 *
 */
typedef enum {
  HE_STD_128_CLASSIC,
  HE_STD_192_CLASSIC,
  HE_STD_256_CLASSIC,
  HE_STD_NOT_SET,
} SECURITY_LEVEL;

/**
 * @brief prime list
 *
 */
typedef struct {
  size_t  _num_primes;
  int64_t _primes[];
} PRIME_LIST;

/**
 * @brief define config
 *
 */
typedef struct {
  SECURITY_LEVEL _level;
  size_t         _log_n;
  size_t         _first_mod;
  size_t         _log_sf;
  size_t         _q_bound;
  PRIME_LIST*    _primes;
} DEF_CONFIG;

/**
 * @brief HE security standard
 *
 */
typedef struct {
  SECURITY_LEVEL _level;
  size_t         _log_n;
  size_t         _q_bound;
} HE_SECURITY_STANDARD;

typedef struct {
  int64_t _order;
  int64_t _prime;
  int64_t _rou;
} PRIME_ROU;

/**
 * @brief get default primes from given parameters
 *
 *
 * @param primes default primes
 * @param l security level
 * @param poly_degree degree of polynomial
 * @param num_q_primes number of q primes
 */
void Get_default_primes(VALUE_LIST* primes, SECURITY_LEVEL l,
                        uint32_t poly_degree, size_t num_q_primes);

/**
 * @brief get bits of the default scaling factor
 *
 * @param l security level
 * @param poly_degree degree of polynomial
 * @param num_q_primes number of q primes
 * @return
 */
size_t Get_default_sf_bits(SECURITY_LEVEL l, uint32_t poly_degree,
                           size_t num_q_primes);

/**
 * @brief get ciphertext modulus Q bound(for Hybrid: P * Q)
 *
 * @param l security level
 * @param poly_degree degree of polynomial
 * @return
 */
size_t Get_qbound(SECURITY_LEVEL l, uint32_t poly_degree);

/**
 * @brief get the max bits of qBound
 *
 * @param l security level
 * @param poly_degree degree of polynomial
 * @return
 */
size_t Get_max_bit_count(SECURITY_LEVEL l, uint32_t poly_degree);

/**
 * @brief get the number of parts of q primes
 *
 * @param mult_depth
 * @return
 */
size_t Get_default_num_q_parts(size_t mult_depth);

/**
 * @brief get root of unity with the given order in the given prime modulus.
 *
 * @param order order n of the root of unity
 * @param prime given modulus
 * @return
 */
int64_t Get_rou(int64_t order, int64_t prime);

/**
 * @brief get security level
 *
 * @param level bits of security against classical computers
 * @return
 */
SECURITY_LEVEL Get_sec_level(size_t level);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_FHE_STD_PARAMS_H
