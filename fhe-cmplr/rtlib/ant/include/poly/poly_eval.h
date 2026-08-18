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
#include "util/plaintext.h"
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

static inline void Free_polys(POLY* polys) {
  FMT_ASSERT(polys, "null polys");
  POLY     poly_head = polys[0];
  int64_t* poly_data = Get_poly_coeffs(poly_head);
  if (poly_data) free(poly_data);
  if (poly_head) free(poly_head);
}

//! @brief Copy polynomial
static inline void Copy_poly(POLY res, POLY poly) {
  RTLIB_TM_START(RTM_COPY_POLY, rtm);
  Copy_polynomial(res, poly);
  RTLIB_TM_END(RTM_COPY_POLY, rtm);
}

static inline POLY Init_poly_by_opnd(POLY res, POLY opnd1, POLY opnd2,
                                     bool is_ext) {
  CRT_CONTEXT* crt    = Get_crt_context();
  POLY         picked = NULL;
  if (opnd1 == NULL || Get_num_q(opnd1) == 0)
    picked = opnd2;
  else if (opnd2 == NULL || Get_num_q(opnd2) == 0)
    picked = opnd1;
  else {
    // pick small level poly
    picked = Get_num_q(opnd1) <= Get_num_q(opnd2) ? opnd1 : opnd2;
  }
  // if opnd is zero ciph, just return
  if (picked == NULL) return res;
  if (Get_poly_coeffs(res) != NULL && Get_num_q(res) >= Get_num_q(picked)) {
    FMT_ASSERT(Get_num_q(res) >= Get_num_q(picked) &&
                   Get_rdgree(res) == Get_rdgree(picked),
               "unmatched size");
    Set_poly_level(res, Get_num_q(picked));
    if (is_ext) {
      FMT_ASSERT(Get_num_p(res) == Get_crt_num_p(crt), "invalid p number");
    }
  } else {
    if (Get_poly_coeffs(res) != NULL) Free_poly_data(res);
    uint32_t ring_degree = Get_rdgree(picked);
    size_t   num_q       = Get_num_q(picked);
    Alloc_poly_data(res, ring_degree, num_q, is_ext ? Get_crt_num_p(crt) : 0);
  }
  Set_is_ntt(res, true);
  return res;
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

//! @brief Modswitch poly to res
POLY Modswitch(POLY res, POLY poly);

//! @brief Precompute for key switch
POLY* Precomp(POLY input);

//! @brief Dot prod for key switch
POLY Dot_prod(POLY res, POLY* p0, POLY* p1, uint32_t num_part);

void Chk_level(POLY res, POLY p0, POLY p1);

POLY Extend(POLY res, POLY poly);

//! @brief Modswitch poly to res
POLY Modswitch(POLY res, POLY poly);

/**
 * @brief Perform Fused operation: Mod_down->Rescale
 *
 * @param res result poly
 * @param rot_idx rotation index
 * @param poly input extended poly
 */
POLY Mod_down_fuse1(POLY res, POLY poly);

POLY Poly_mulp_fast(POLY res, POLY poly1, POLY poly2, POLY poly2_prec);

void Print_poly_lite(FILE* fp, POLY input);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_POLY_EVAL_H
