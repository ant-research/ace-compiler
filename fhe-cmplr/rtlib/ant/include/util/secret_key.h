//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_SECRET_KEY_H
#define RTLIB_INCLUDE_SECRET_KEY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/polynomial.h"

// A module to keep track of a secret key.

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief An instance of a secret key
 *
 */
typedef struct {
  POLYNOMIAL _s;
  POLYNOMIAL _ntt_s;  // ntt version of secret key
} SECRET_KEY;

/**
 * @brief Get the poly of secret key
 *
 * @param sk input secret key
 * @return POLYNOMIAL*
 */
static inline POLYNOMIAL* Get_sk_poly(SECRET_KEY* sk) { return &(sk->_s); }

/**
 * @brief Get the poly(ntt version) of secret key
 *
 * @param sk input secret key
 * @return POLYNOMIAL*
 */
static inline POLYNOMIAL* Get_ntt_sk(SECRET_KEY* sk) { return &(sk->_ntt_s); }

/**
 * @brief alloc secret key from given parameters
 *
 * @param ring_degree ring degree of polynomial
 * @param num_primes number of q primes
 * @param num_primes_p number of p primes
 * @return SECRET_KEY*
 */
static inline SECRET_KEY* Alloc_secret_key(uint32_t ring_degree,
                                           size_t   num_primes,
                                           size_t   num_primes_p) {
  SECRET_KEY* sk = (SECRET_KEY*)malloc(sizeof(SECRET_KEY));
  Alloc_poly_data(Get_sk_poly(sk), ring_degree, num_primes, num_primes_p);
  Alloc_poly_data(Get_ntt_sk(sk), ring_degree, num_primes, num_primes_p);
  return sk;
}

/**
 * @brief cleanup secretkey memory
 *
 * @param sk input secret key
 */
static inline void Free_secretkey(SECRET_KEY* sk) {
  if (sk == NULL) return;
  Free_poly_data(Get_sk_poly(sk));
  Free_poly_data(Get_ntt_sk(sk));
  free(sk);
}

/**
 * @brief Represents secret key as a string
 *
 * @param fp input file
 * @param sk input secret key
 */
static inline void Print_sk(FILE* fp, SECRET_KEY* sk) {
  Print_poly(fp, Get_sk_poly(sk));
  fprintf(fp, "ntt sk: ");
  Print_poly(fp, Get_ntt_sk(sk));
}

//! @brief Get memory size of allocated number of sk
static inline size_t Get_sk_mem_size(SECRET_KEY* sk) {
  return sizeof(SECRET_KEY) + Get_poly_mem_size(Get_sk_poly(sk)) +
         Get_poly_mem_size(Get_ntt_sk(sk));
}

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_SECRET_KEY_H
