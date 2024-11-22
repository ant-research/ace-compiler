//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_PUBLIC_KEY_H
#define RTLIB_INCLUDE_PUBLIC_KEY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/polynomial.h"

// A module to keep track of a public key.

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  POLYNOMIAL _p0;  // first element of public key
  POLYNOMIAL _p1;  // second element of public key
} PUBLIC_KEY;

/**
 * @brief Get the first element of public key
 *
 * @param pk input public key
 * @return POLYNOMIAL*
 */
static inline POLYNOMIAL* Get_pk0(PUBLIC_KEY* pk) { return &(pk->_p0); }

/**
 * @brief Get the second element of public key
 *
 * @param pk input public key
 * @return POLYNOMIAL*
 */
static inline POLYNOMIAL* Get_pk1(PUBLIC_KEY* pk) { return &(pk->_p1); }

/**
 * @brief Get the degree of polynomial from public key
 *
 * @param pk input public key
 * @return uint32_t
 */
static inline uint32_t Get_pubkey_degree(PUBLIC_KEY* pk) {
  return pk->_p0._ring_degree;
}

/**
 * @brief Get the number of q prime of polynomial from public key
 *
 * @param pk input public key
 * @return size_t
 */
static inline size_t Get_pubkey_prime_cnt(PUBLIC_KEY* pk) {
  return pk->_p0._num_primes;
}

/**
 * @brief Get the number of p prime of polynomial from public key
 *
 * @param pk input public key
 * @return size_t
 */
static inline size_t Get_pubkey_prime_p_cnt(PUBLIC_KEY* pk) {
  return pk->_p0._num_primes_p;
}

/**
 * @brief alloc public key from given parameters
 *
 * @param ring_degree ring degree of polynomial
 * @param num_primes number of q primes
 * @param num_primes_p number of p primes
 * @return PUBLIC_KEY*
 */
static inline PUBLIC_KEY* Alloc_public_key(uint32_t ring_degree,
                                           size_t   num_primes,
                                           size_t   num_primes_p) {
  PUBLIC_KEY* pk = (PUBLIC_KEY*)malloc(sizeof(PUBLIC_KEY));
  Alloc_poly_data(Get_pk0(pk), ring_degree, num_primes, num_primes_p);
  Alloc_poly_data(Get_pk1(pk), ring_degree, num_primes, num_primes_p);
  return pk;
}

/**
 * @brief copy public key from input public key
 *
 * @param res public key
 * @param pk input public key
 */
static inline void Copy_public_key(PUBLIC_KEY* res, PUBLIC_KEY* pk) {
  Copy_polynomial(Get_pk0(res), Get_pk0(pk));
  Copy_polynomial(Get_pk1(res), Get_pk1(pk));
}

/**
 * @brief cleanup publickey memory
 *
 * @param pk input public key
 */
static inline void Free_publickey(PUBLIC_KEY* pk) {
  if (pk == NULL) return;
  Free_poly_data(Get_pk0(pk));
  Free_poly_data(Get_pk1(pk));
  free(pk);
  pk = NULL;
}

/**
 * @brief Represents PUBLIC_KEY as a string
 *
 * @param fp input file
 * @param pk input public key
 */
static inline void Print_pk(FILE* fp, PUBLIC_KEY* pk) {
  fprintf(fp, "p0: ");
  Print_poly_rawdata(fp, Get_pk0(pk));
  fprintf(fp, "\np1: ");
  Print_poly_rawdata(fp, Get_pk1(pk));
}

//! @brief Get memory size of allocated number of pk
static inline size_t Get_pk_mem_size(PUBLIC_KEY* pk) {
  return sizeof(PUBLIC_KEY) + Get_poly_mem_size(Get_pk0(pk)) +
         Get_poly_mem_size(Get_pk1(pk));
}

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_PUBLIC_KEY_H