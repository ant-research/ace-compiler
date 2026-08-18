//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_PLAINTEXT_H
#define RTLIB_INCLUDE_PLAINTEXT_H

#include <stdlib.h>
#include <string.h>

#include "util/polynomial.h"

// A module to keep track of a plaintext.

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief an instance of a plaintext
 * This is type for a plaintext, which consists
 * of one polynomial
 *
 */
typedef struct {
  POLYNOMIAL _poly;
  uint32_t   _slots;
  double     _scaling_factor;
  uint32_t   _sf_degree;
} PLAINTEXT;

/**
 * @brief get polynomial of plaintext
 *
 * @param plain input plaintext
 * @return POLYNOMIAL*
 */
static inline POLYNOMIAL* Get_plain_poly(PLAINTEXT* plain) {
  return &(plain->_poly);
}

/**
 * @brief get slots of plaintext
 *
 * @param plain input plaintext
 * @return uint32_t
 */
static inline uint32_t Get_plain_slots(PLAINTEXT* plain) {
  return plain->_slots;
}

/**
 * @brief get scaling factor of plaintext
 *
 * @param plain input plaintext
 * @return double
 */
static inline double Get_plain_scaling_factor(PLAINTEXT* plain) {
  return plain->_scaling_factor;
}

/**
 * @brief get degree of scaling factor of plaintext
 *
 * @param plain input plaintext
 * @return uint32_t
 */
static inline uint32_t Get_plain_sf_degree(PLAINTEXT* plain) {
  return plain->_sf_degree;
}

//! @brief get memory size of plaintext
static inline size_t Get_plain_mem_size(PLAINTEXT* plain) {
  return sizeof(PLAINTEXT) + Get_poly_mem_size(Get_plain_poly(plain));
}

/**
 * @brief allocate a plaintext with malloc.
 *
 * @return PLAINTEXT*
 */
static inline PLAINTEXT* Alloc_plaintext() {
  PLAINTEXT* plain = (PLAINTEXT*)malloc(sizeof(PLAINTEXT));
  memset(plain, 0, sizeof(PLAINTEXT));
  return plain;
}

/**
 * @brief cleanup plaintext memory
 *
 * @param plain input plaintext
 */
static inline void Free_plaintext(PLAINTEXT* plain) {
  if (plain == NULL) return;
  Free_poly_data(Get_plain_poly(plain));
  free(plain);
}

/**
 * @brief initialize plaintext with given parameters
 *
 * @param plain plaintext to be initialized
 * @param degree ring degree
 * @param slots given slots
 * @param num_primes numbers of q primes
 * @param num_primes_p numbers of p primes
 * @param scaling_factor scaling factor
 * @param sf_degree degree of scaling factor
 */
static inline void Init_plaintext(PLAINTEXT* plain, uint32_t degree,
                                  uint32_t slots, size_t num_primes,
                                  size_t num_primes_p, double scaling_factor,
                                  uint32_t sf_degree) {
  plain->_scaling_factor = scaling_factor;
  plain->_sf_degree      = sf_degree;
  plain->_slots          = slots;
  POLYNOMIAL* poly       = Get_plain_poly(plain);
  if (Get_poly_len(poly) == 0) {
    Alloc_poly_data(Get_plain_poly(plain), degree, num_primes, num_primes_p);
  } else {
    IS_TRUE(Get_poly_coeffs(poly) && Get_num_q(poly) == num_primes &&
                Get_num_p(poly) == num_primes_p,
            "unmatched size");
  }
}

//! @brief Derive_plain: derive res._poly from plain._poly with num_q & num_p
static inline void Derive_plain(PLAINTEXT* res, PLAINTEXT* plain, size_t num_q,
                                size_t num_p) {
  res->_scaling_factor = plain->_scaling_factor;
  res->_sf_degree      = plain->_sf_degree;
  res->_slots          = plain->_slots;
  Derive_poly(&res->_poly, &plain->_poly, num_q, num_p);
}

/**
 * @brief represents plaintext as a readable string
 *
 * @param fp input file
 * @param plain input plaintext
 */
static inline void Print_plain(FILE* fp, PLAINTEXT* plain) {
  Print_poly(fp, Get_plain_poly(plain));
}

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_PLAINTEXT_H