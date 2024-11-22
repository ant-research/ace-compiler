//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_POLY_EVAL_H
#define RTLIB_INCLUDE_POLY_EVAL_H

#include "rtlib/context.h"
#include "util/polynomial.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef POLYNOMIAL* POLY;

/**
 * @brief Alloc polynomial
 *
 * @param degree poly degree
 * @param q_primes number of Q primes
 * @param p_primes numbe of P primes
 * @return
 */
static inline POLY Alloc_poly(uint32_t degree, size_t q_primes, bool extend_p) {
  FMT_ASSERT(q_primes > 0, "Alloc_poly: q primes should not be NULL");
  POLY poly = (POLY)malloc(sizeof(POLYNOMIAL));
  Alloc_poly_data(poly, degree, q_primes,
                  extend_p ? Get_crt_num_p(Get_crt_context()) : 0);
  // hard code for now, ntt should be set by compiler
  Set_is_ntt(poly, TRUE);
  return poly;
}

/**
 * @brief Cleanup polynomial
 *
 * @param poly
 */
static inline void Free_poly(POLY poly) { Free_polynomial(poly); }

//! @brief Copy polynomial
static inline void Copy_poly(POLY res, POLY poly) {
  RTLIB_TM_START(RTM_COPY_POLY, rtm);
  Copy_polynomial(res, poly);
  RTLIB_TM_END(RTM_COPY_POLY, rtm);
}

/**
 * @brief Get coefficients from polynomial
 *
 * @param poly polynomial
 * @param level current level
 * @param degree poly degree of polynomial
 * @return int64_t*
 */
static inline int64_t* Coeffs(POLY poly, size_t level, uint32_t degree) {
  assert(level <= Get_num_pq(poly) && "index overflow");
  return Get_poly_coeffs(poly) + level * degree;
}

/**
 * @brief Set coefficients for destination polynomial
 *
 * @param dst destination polynomial
 * @param level current level
 * @param degree poly degree of polynomial
 * @param src input coefficients
 */
static inline void Set_coeffs(POLY dst, uint32_t level, uint32_t degree,
                              int64_t* src) {
  assert(level <= Get_num_pq(dst) && "index overflow");
  int64_t* dst_coeffs = Coeffs(dst, level, degree);
  memcpy(dst_coeffs, src, sizeof(int64_t) * degree);
}

/**
 * @brief Get level of poly
 *
 * @param poly input poly
 * @return size_t
 */
static inline size_t Poly_level(POLY poly) { return Get_poly_level(poly); }

/**
 * @brief Get number of allocated number of primes, include P & Q
 *
 * @param poly input poly
 * @return size_t
 */
static inline size_t Num_alloc(POLY poly) { return Get_num_alloc_primes(poly); }

/**
 * @brief Get number of p primes from polynomial
 *
 * @param poly given polynomial
 * @return size_t
 */
static inline size_t Num_p(POLY poly) { return Get_num_p(poly); }

/**
 * @brief Get length of decomposed poly
 *
 * @param poly input poly
 * @return size_t
 */
static inline size_t Num_decomp(POLY poly) {
  return Get_num_decomp_poly(poly, Get_crt_context());
}

/**
 * @brief Digit decompose of given part
 *
 * @param res result poly
 * @param poly input poly
 * @param q_part_idx index of q part
 */
POLY Decomp(POLY res, POLY poly, uint32_t q_part_idx);

/**
 * @brief Raise poly from part Q base to P*partQ base
 *
 * @param res result poly
 * @param poly input poly
 * @param q_part_idx index of q part
 */
POLY Mod_up(POLY res, POLY poly, uint32_t q_part_idx);

//! @brief Decompose and raise at given part index
//! @param res result poly
//! @param poly input poly
//! @param q_part_idx index of q part
POLY Decomp_modup(POLY res, POLY poly, uint32_t q_part_idx);

/**
 * @brief Reduce poly from P*Q to Q
 *
 * @param res result poly
 * @param poly input poly
 */
POLY Mod_down(POLY res, POLY poly);

//! @brief Rescale poly to res
POLY Rescale(POLY res, POLY poly);

void Print_poly_lite(FILE* fp, POLY input);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_POLY_EVAL_H
