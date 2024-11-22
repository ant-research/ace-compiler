//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_POLYNOMIAL_H
#define RTLIB_INCLUDE_POLYNOMIAL_H

#include "common/rtlib_timing.h"
#include "util/crt.h"
#include "util/fhe_types.h"
#include "util/fhe_utils.h"
#include "util/ntt.h"
#include "util/number_theory.h"

#ifdef __cplusplus
extern "C" {
#endif

// A module to handle polynomial arithmetic in the quotient ring Z_a[x]/f(x).

#define FOR_ALL_COEFF(poly, idx) \
  for (size_t idx = 0; idx < Get_poly_alloc_len(poly); idx++)

/**
 * @brief A polynomial in the ring R_a
 * Here, R is the quotient ring Z[x]/f(x), where f(x) = x^d + 1.
 * The polynomial keeps track of the ring degree d, the coefficient
 * modulus a, and the coefficients in an array.
 *
 */
typedef struct {
  uint32_t _ring_degree;     // degree d of polynomial that determines the
                             // quotient ring R
  size_t _num_alloc_primes;  // number of allocated number of primes
                             // including q and p
  size_t   _num_primes;      // number of q primes
  size_t   _num_primes_p;    // number of p primes
  bool     _is_ntt;          // polynomial is ntt or intt
  int64_t* _data;            // array of coefficients in polynomial
} POLYNOMIAL;

/**
 * @brief alloc polynomial from given parameters
 *
 * @param poly return polynomial
 * @param ring_degree ring degree of polynomial
 * @param num_primes number of q primes
 * @param num_primes_p number of p primes
 */
static inline void Alloc_poly_data(POLYNOMIAL* poly, uint32_t ring_degree,
                                   size_t num_primes, size_t num_primes_p) {
  poly->_ring_degree      = ring_degree;
  poly->_num_primes       = num_primes;
  poly->_num_primes_p     = num_primes_p;
  poly->_num_alloc_primes = num_primes + num_primes_p;
  size_t alloc_size = sizeof(int64_t) * poly->_num_alloc_primes * ring_degree;
  poly->_data       = (int64_t*)malloc(alloc_size);
  poly->_is_ntt     = FALSE;
  memset(poly->_data, 0, alloc_size);
}

/**
 * @brief cleanup data of polynomial
 *
 * @param poly
 */
static inline void Free_poly_data(POLYNOMIAL* poly) {
  if (poly->_data) {
    free(poly->_data);
    poly->_data = NULL;
  }
  poly->_num_alloc_primes = 0;
}

//! @brief cleanup polynomial
static inline void Free_polynomial(POLYNOMIAL* poly) {
  Free_poly_data(poly);
  free(poly);
  poly = NULL;
}

/**
 * @brief get ring degree from polynomial
 *
 * @param poly
 * @return uint32_t
 */
static inline uint32_t Get_rdgree(POLYNOMIAL* poly) {
  return poly->_ring_degree;
}

/**
 * @brief check if polynomial is NTT or not
 *
 * @param poly
 * @return true
 * @return false
 */
static inline bool Is_ntt(POLYNOMIAL* poly) { return poly->_is_ntt; }

/**
 * @brief get level of polynomial
 *
 * @param poly
 * @return size_t
 */
static inline size_t Get_poly_level(POLYNOMIAL* poly) {
  return poly->_num_primes;
}

/**
 * @brief get number of q primes
 *
 * @param poly
 * @return size_t
 */
static inline size_t Get_num_q(POLYNOMIAL* poly) { return poly->_num_primes; }

/**
 * @brief get number of p primes
 *
 * @param poly
 * @return size_t
 */
static inline size_t Get_num_p(POLYNOMIAL* poly) { return poly->_num_primes_p; }

/**
 * @brief get number of p and q primes
 *
 * @param poly
 * @return size_t
 */
static inline size_t Get_num_pq(POLYNOMIAL* poly) {
  return poly->_num_primes_p + poly->_num_primes;
}

/**
 * @brief get number of allocated number of primes, include P & Q
 *
 * @param poly
 * @return size_t
 */
static inline size_t Get_num_alloc_primes(POLYNOMIAL* poly) {
  return poly->_num_alloc_primes;
}

/**
 * @brief Get number of decompose polynomial
 *
 * @param poly input polynomial
 * @param crt crt context
 * @return size_t
 */
static inline size_t Get_num_decomp_poly(POLYNOMIAL* poly, CRT_CONTEXT* crt) {
  CRT_PRIMES* q_parts    = Get_qpart(crt);
  size_t      ql         = Get_num_q(poly);
  size_t      part_size  = Get_per_part_size(q_parts);
  size_t      num_qpartl = ceil((double)ql / part_size);
  size_t      num_qpart  = Get_num_parts(q_parts);
  if (num_qpartl > num_qpart) {
    num_qpartl = num_qpart;
  }
  return num_qpartl;
}

/**
 * @brief get length of polynomial data
 *
 * @param poly
 * @return size_t
 */
static inline size_t Get_poly_len(POLYNOMIAL* poly) {
  return Get_rdgree(poly) * Get_num_pq(poly);
}

/**
 * @brief get length of allocated number of primes
 *
 * @param poly
 * @return size_t
 */
static inline size_t Get_poly_alloc_len(POLYNOMIAL* poly) {
  return Get_rdgree(poly) * Get_num_alloc_primes(poly);
}

/**
 * @brief get memory size of allocated number of primes
 *
 * @param poly
 * @return size_t
 */
static inline size_t Get_poly_mem_size(POLYNOMIAL* poly) {
  return Get_num_alloc_primes(poly) * Get_rdgree(poly) * sizeof(int64_t);
}

/**
 * @brief get starting address of data of polynomial
 *
 * @param poly
 * @return int64_t*
 */
static inline int64_t* Get_poly_coeffs(POLYNOMIAL* poly) { return poly->_data; }

/**
 * @brief get starting address of p data of polynomial
 *
 * @param poly
 * @return int64_t*
 */
static inline int64_t* Get_p_coeffs(POLYNOMIAL* poly) {
  return poly->_data +
         (Get_num_alloc_primes(poly) - Get_num_p(poly)) * Get_rdgree(poly);
}

/**
 * @brief get data at given idx of polynomial
 *
 * @param poly
 * @param idx given index
 * @return
 */
static inline int64_t Get_coeff_at(POLYNOMIAL* poly, uint32_t idx) {
  FMT_ASSERT(poly, "null poly");
  FMT_ASSERT(idx < Get_poly_alloc_len(poly), "idx outof bound");
  return poly->_data[idx];
}

/**
 * @brief set data for the given idx of polynomial
 *
 * @param poly
 * @param val
 * @param idx
 */
static inline void Set_coeff_at(POLYNOMIAL* poly, int64_t val, uint32_t idx) {
  FMT_ASSERT(poly, "null poly");
  FMT_ASSERT(idx < Get_poly_alloc_len(poly), "idx outof bound");
  poly->_data[idx] = val;
}

/**
 * @brief set polynomial is ntt or not
 *
 * @param poly
 * @param v
 */
static inline void Set_is_ntt(POLYNOMIAL* poly, bool v) { poly->_is_ntt = v; }

/**
 * @brief set level of polynomial
 *
 * @param res
 * @param level
 */
static inline void Set_poly_level(POLYNOMIAL* res, size_t level) {
  FMT_ASSERT(Get_num_alloc_primes(res) >= level + Get_num_p(res),
             "raised level exceed alloc_size");
  res->_num_primes = level;
}

//! @brief set number of p of polynomial
static inline void Set_num_p(POLYNOMIAL* res, size_t num_p) {
  FMT_ASSERT(Get_num_alloc_primes(res) >= num_p + Get_num_q(res),
             "raised level exceed alloc_size");
  res->_num_primes_p = num_p;
}

/**
 * @brief set new level of polynomial, and also saved the original level
 *
 * @param res
 * @param level
 * @return size_t
 */
static inline size_t Save_poly_level(POLYNOMIAL* res, size_t level) {
  size_t old_level = Get_poly_level(res);
  Set_poly_level(res, level);
  return old_level;
}

/**
 * @brief set level of polynomial back to original level
 *
 * @param res
 * @param level
 */
static inline void Restore_poly_level(POLYNOMIAL* res, size_t level) {
  Set_poly_level(res, level);
}

/**
 * @brief reduce the number of q primes by one, used for rescale
 *
 * @param res
 */
static inline void Mod_down_q_primes(POLYNOMIAL* res) {
  Set_poly_level(res, Get_poly_level(res) - 1);
}

/**
 * @brief initialize polynomial with given parameters
 *
 * @param poly polynomial to be initialized
 * @param ring_degree ring degree of polynomial
 * @param num_primes number of q primes
 * @param num_primes_p number of p primes
 * @param data init value
 */
static inline void Init_poly_data(POLYNOMIAL* poly, uint32_t ring_degree,
                                  size_t num_primes, size_t num_primes_p,
                                  int64_t* data) {
  poly->_ring_degree  = ring_degree;
  poly->_num_primes   = num_primes;
  poly->_num_primes_p = num_primes_p;
  poly->_data         = data;
  if (poly->_num_alloc_primes == 0)
    poly->_num_alloc_primes = num_primes + num_primes_p;
  poly->_is_ntt = FALSE;
}

/**
 * @brief initialize poly by given poly size, do not copy contents
 *
 * @param res polynomial to be initialized
 * @param poly given polynomial
 */
static inline void Init_poly(POLYNOMIAL* res, POLYNOMIAL* poly) {
  uint32_t ring_degree  = Get_rdgree(poly);
  size_t   num_primes   = Get_num_q(poly);
  size_t   num_primes_p = Get_num_p(poly);
  if (res->_data == NULL) {
    Alloc_poly_data(res, ring_degree, num_primes, num_primes_p);
  } else {
    // res already allocated memory, reuse the memory
    IS_TRUE(Get_rdgree(res) == ring_degree, "unmatched ring degree");
    if (Get_num_alloc_primes(res) * Get_rdgree(res) < Get_poly_len(poly)) {
      Free_poly_data(res);
      Alloc_poly_data(res, ring_degree, num_primes, num_primes_p);
    } else {
      memset(res->_data, 0, Get_poly_mem_size(res));
      Init_poly_data(res, ring_degree, num_primes, num_primes_p, res->_data);
    }
  }
}

//! @brief Allocate list of polynomials, the polynomials and data only allocate
//!  once to reduce the malloc time, the polynomials memory can only be freed
//!  by call Free_poly_list.
static inline void Alloc_poly_list(VALUE_LIST* poly_list, uint32_t ring_degree,
                                   size_t num_primes, size_t num_primes_p) {
  IS_TRUE(poly_list && poly_list->_type == PTR_TYPE, "invalid value list");

  size_t arr_len = LIST_LEN(poly_list);
  size_t per_data_size =
      sizeof(int64_t) * (num_primes + num_primes_p) * ring_degree;
  char* data = (char*)malloc(per_data_size * arr_len);
  memset(data, 0, per_data_size * arr_len);
  POLYNOMIAL* polys = (POLYNOMIAL*)malloc(sizeof(POLYNOMIAL) * arr_len);

  FOR_ALL_ELEM(poly_list, idx) {
    polys->_num_alloc_primes = 0;
    Init_poly_data(polys, ring_degree, num_primes, num_primes_p,
                   (int64_t*)data);
    Set_ptr_value(poly_list, idx, (PTR)polys);
    data += per_data_size;
    polys++;
  }
}

//! @brief Free poly list allocated by Alloc_poly_list
//! Note: The POLYNOMIAL and its data are only allocated once, so only need
//! to free the head pointer
static inline void Free_poly_list(VALUE_LIST* poly_list) {
  if (poly_list == NULL) return;
  POLYNOMIAL* poly_head = (POLYNOMIAL*)PTR_VALUE_AT(poly_list, 0);
  int64_t*    data_head = Get_poly_coeffs(poly_head);
  if (data_head) free(data_head);
  if (poly_head) free(poly_head);
  Free_value_list(poly_list);
}

//! @brief Print poly list
void Print_poly_list(FILE* fp, VALUE_LIST* poly_list);

/**
 * @brief check if the size of two given polynomial match
 *
 * @param poly1
 * @param poly2
 * @return
 */
static inline bool Is_size_match(POLYNOMIAL* poly1, POLYNOMIAL* poly2) {
  if (poly1 == NULL || poly2 == NULL) {
    return FALSE;
  }
  if (Get_rdgree(poly1) == Get_rdgree(poly2) &&
      Get_poly_level(poly1) == Get_poly_level(poly2)) {
    return TRUE;
  }
  return FALSE;
}

/**
 * @brief check if two given polynomial ntt format match
 *
 * @param poly1
 * @param poly2
 * @return true
 * @return false
 */
static inline bool Is_ntt_match(POLYNOMIAL* poly1, POLYNOMIAL* poly2) {
  return (Is_ntt(poly1) == Is_ntt(poly2));
}

/**
 * @brief copy polynomial from another polynomial
 *
 * @param dest
 * @param src
 */
static inline void Copy_polynomial(POLYNOMIAL* dest, POLYNOMIAL* src) {
  FMT_ASSERT(Is_size_match(dest, src), "unmatched primes");
  FMT_ASSERT(Get_num_p(dest) >= Get_num_p(src), "unmatched p primes cnt");

  // copy q coeffs
  memcpy(Get_poly_coeffs(dest), Get_poly_coeffs(src),
         sizeof(int64_t) * Get_num_q(dest) * Get_rdgree(dest));
  // copy p coeffs only if src has p coeffs
  if (Get_num_p(src)) {
    memcpy(Get_p_coeffs(dest), Get_p_coeffs(src),
           sizeof(int64_t) * Get_num_p(dest) * Get_rdgree(dest));
  }
  Set_is_ntt(dest, Is_ntt(src));
}

//! @brief Extract coeffcients from poly between level range
//! [start_ofst, start_ofst + prime_cnt)
void Extract_poly(POLYNOMIAL* res, POLYNOMIAL* poly, size_t start_ofst,
                  size_t prime_cnt);

//! @brief Derive coeffcients from poly with q_cnt & p_cnt
void Derive_poly(POLYNOMIAL* res, POLYNOMIAL* poly, size_t q_cnt, size_t p_cnt);

// Print polynomial
/**
 * @brief print raw data of polynomial
 *
 * @param fp
 * @param poly given polynomial
 */
void Print_poly_rawdata(FILE* fp, POLYNOMIAL* poly);

/**
 * @brief print polynomial with detail
 *
 * @param fp output
 * @param poly given polynomial
 * @param primes convert ntt to poly with given prime
 * @param detail print detail or not
 */
void Print_poly_detail(FILE* fp, POLYNOMIAL* poly, VL_CRTPRIME* primes,
                       bool detail);

/**
 * @brief print polynomial
 *
 * @param fp output
 * @param poly given polynomial
 */
void Print_poly(FILE* fp, POLYNOMIAL* poly);

/**
 * @brief print polynomial after reconstruct to bigint value
 *
 * @param fp
 * @param poly given polynomial
 */
void Print_rns_poly(FILE* fp, POLYNOMIAL* poly);

//! @brief A lite version of print polynomial coefficients
void Print_poly_lite(FILE* fp, POLYNOMIAL* input);

/**
 * @brief Adds the current polynomial to poly inside the ring R_a.
 *
 * @param sum POLYNOMIAL which is the sum of the two
 * @param poly1 POLYNOMIAL to be added
 * @param poly2 POLYNOMIAL to be added
 * @param crt crt context
 * @param P_modulus vector of modulus for P
 * @return POLYNOMIAL*
 */
POLYNOMIAL* Add_poly(POLYNOMIAL* sum, POLYNOMIAL* poly1, POLYNOMIAL* poly2,
                     CRT_CONTEXT* crt, VL_CRTPRIME* p_modulus);

/**
 * @brief Subtracts second polynomial from first polynomial in the ring.
 *
 * @param poly_diff poly which is the difference between the two polys
 * @param poly1 POLYNOMIAL to be subtracted
 * @param poly2 POLYNOMIAL to be subtracted
 * @param crt crt context
 * @param p_modulus vector of modulus for P
 * @return POLYNOMIAL*
 */
POLYNOMIAL* Sub_poly(POLYNOMIAL* poly_diff, POLYNOMIAL* poly1,
                     POLYNOMIAL* poly2, CRT_CONTEXT* crt,
                     VALUE_LIST* p_modulus);

/**
 * @brief convert polynomial from coefficient form to NTT
 *
 * @param poly
 * @param crt
 */
void Conv_poly2ntt_inplace(POLYNOMIAL* poly, CRT_CONTEXT* crt);
void Conv_poly2ntt_inplace_with_primes(POLYNOMIAL* poly, CRT_CONTEXT* crt,
                                       VL_CRTPRIME* primes);
void Conv_poly2ntt_with_primes(POLYNOMIAL* res, POLYNOMIAL* poly,
                               CRT_CONTEXT* crt, VL_CRTPRIME* primes);
void Conv_poly2ntt(POLYNOMIAL* res, POLYNOMIAL* poly, CRT_CONTEXT* crt);

/**
 * @brief convert polynomial from NTT to coefficient form
 *
 * @param res
 * @param poly
 * @param crt
 */
void Conv_ntt2poly(POLYNOMIAL* res, POLYNOMIAL* poly, CRT_CONTEXT* crt);
void Conv_ntt2poly_with_primes(POLYNOMIAL* res, POLYNOMIAL* poly,
                               CRT_CONTEXT* crt, VL_CRTPRIME* primes);
void Conv_ntt2poly_inplace_with_primes(POLYNOMIAL* poly, CRT_CONTEXT* crt,
                                       VL_CRTPRIME* primes);
void Conv_ntt2poly_inplace(POLYNOMIAL* poly, CRT_CONTEXT* crt);

/**
 * @brief Multiplies two polynomials in the ring using NTT.
 *
 * @param res A POLYNOMIAL which is the product of the two polynomials
 * @param poly1 input poly to be multiply
 * @param poly2 input poly to be multiply
 * @param q_primes vector of modulus for q
 * @param p_primes vector of modulus for p
 */
void Multiply_ntt(POLYNOMIAL* res, POLYNOMIAL* poly1, POLYNOMIAL* poly2,
                  VALUE_LIST* q_primes, VALUE_LIST* p_primes);

/**
 * @brief Adds two polynomial
 * res = res + (poly1 * poly2)
 *
 * @param res POLYNOMIAL which is the sum of the two
 * @param poly1 input polynomial
 * @param poly2 input polynomial
 * @param q_primes vector of modulus for q
 * @param p_primes vector of modulus for p
 */
void Multiply_add(POLYNOMIAL* res, POLYNOMIAL* poly1, POLYNOMIAL* poly2,
                  VALUE_LIST* q_primes, VALUE_LIST* p_primes);

/**
 * @brief Multiplies two polynomials in the ring using NTT.
 * Multiplies the current polynomial to poly inside the ring R_a
 * using the Number Theoretic Transform (NTT) in O(nlogn).
 *
 * @param res A POLYNOMIAL which is the product of the two polynomials
 * @param poly1 input poly to be multiply
 * @param poly2 input poly to be multiply
 * @param crt crt context
 * @param p_primes vector of modulus for P
 * @return POLYNOMIAL*
 */
POLYNOMIAL* Multiply_poly_fast(POLYNOMIAL* res, POLYNOMIAL* poly1,
                               POLYNOMIAL* poly2, CRT_CONTEXT* crt,
                               VALUE_LIST* p_primes);

/**
 * @brief Multiplies polynomial by a scalar.
 *
 * @param res the product of the polynomial mul the scalar
 * @param poly given polynomial
 * @param scalar Scalar to be multiplied to the current polynomial.
 * @param q_modulus vector of modulus for Q
 * @param p_modulus vector of modulus for P
 * @return POLYNOMIAL*
 */
POLYNOMIAL* Scalar_integer_multiply_poly(POLYNOMIAL* res, POLYNOMIAL* poly,
                                         int64_t scalar, VALUE_LIST* q_modulus,
                                         VALUE_LIST* p_modulus);

/**
 * @brief Multiplies polynomial by a scalar vector.
 *
 * @param res the product of the polynomial mul
 * @param poly given polynomial
 * @param scalars scalar list to be multiplied to the current polynomial.
 * @param q crt primes for Q
 * @param p crt primes for P
 * @return POLYNOMIAL*
 */
POLYNOMIAL* Scalars_integer_multiply_poly(POLYNOMIAL* res, POLYNOMIAL* poly,
                                          VALUE_LIST* scalars, CRT_PRIMES* q,
                                          CRT_PRIMES* p);

/**
 * @brief Multiplies polynomial by a scalar vector with qpart
 *
 * @param res the product of the polynomial mul
 * @param poly given polynomial
 * @param scalars scalar list to be multiplied to the current polynomial.
 * @param q crt primes for Q
 * @param qpart crt primes for qPart
 * @param part_idx index of qPart
 * @return
 */
POLYNOMIAL* Scalars_integer_multiply_poly_qpart(
    POLYNOMIAL* res, POLYNOMIAL* poly, VALUE_LIST* scalars, CRT_PRIMES* q,
    CRT_PRIMES* qpart, size_t part_idx);

/**
 * @brief transformation for automorphism
 *
 * @param res the product of the automorphism
 * @param poly given polynomial
 * @param precomp precomputed from Precompute_automorphism_order
 * @param crt crt context
 * @return
 */
POLYNOMIAL* Automorphism_transform(POLYNOMIAL* res, POLYNOMIAL* poly,
                                   VALUE_LIST* precomp, CRT_CONTEXT* crt);

// ! @brief Rotate poly with the k (precomputed automorphism index) and precomp
// (precomputed automorphism order)
POLYNOMIAL* Rotate_poly(POLYNOMIAL* res, POLYNOMIAL* poly, uint32_t k,
                        VALUE_LIST* precomp, CRT_CONTEXT* crt);

//! @brief Rotate poly with rotation index
void Rotate_poly_with_rotation_idx(POLYNOMIAL* res, POLYNOMIAL* poly,
                                   int32_t rotation, CRT_CONTEXT* crt);

/**
 * @brief transform regular value to rns polynomial from given primes
 *
 * @param res rns polynomial
 * @param res_len length of rns polynomial
 * @param crt_primes crt primes
 * @param vals input original value list
 * @param without_mod within modulus or not
 */
void Transform_values_to_rns(int64_t* res, size_t res_len,
                             CRT_PRIMES* crt_primes, VALUE_LIST* vals,
                             bool without_mod);

/**
 * @brief transform regular value to rns polynomial (Q)
 *
 * @param poly rns polynomial
 * @param crt crt context
 * @param value input original value
 * @param without_mod within modulus or not
 */
void Transform_values_to_qbase(POLYNOMIAL* poly, CRT_CONTEXT* crt,
                               VALUE_LIST* value, bool without_mod);

/**
 * @brief transform regular value to rns polynomial at specifical level
 *
 * @param poly rns polynomial
 * @param crt crt context
 * @param value input original value
 * @param without_mod with modulus or not
 * @param q_level given q level
 * @param p_cnt number of p primes
 */
void Transform_values_at_level(POLYNOMIAL* poly, CRT_CONTEXT* crt,
                               VALUE_LIST* value, bool without_mod,
                               size_t q_level, size_t p_cnt);

/**
 * @brief transform regular value to rns polynomial (P & Q)
 *
 * @param poly rns polynomial
 * @param crt crt context
 * @param value input original value
 * @param without_mod within modulus or not
 */
void Transform_values_to_qpbase(POLYNOMIAL* poly, CRT_CONTEXT* crt,
                                VALUE_LIST* value, bool without_mod);

/**
 * @brief Reconstructs original polynomial from vals from the
 * CRT representation to the regular representation.
 *
 * @param res A value list whose values are reconstructed.
 * @param rns_poly rns polynomial
 * @param crt_primes crt primes
 */
void Reconstruct_rns_poly_to_values(VALUE_LIST* res, POLYNOMIAL* rns_poly,
                                    CRT_PRIMES* crt_primes);

/**
 * @brief Reconstructs polynomial (Q base) from the
 * CRT representation to the regular representation.
 *
 * @param res A value list whose values are reconstructed.
 * @param poly rns polynomial
 * @param crt crt primes
 */
void Reconstruct_qbase_to_values(VALUE_LIST* res, POLYNOMIAL* poly,
                                 CRT_CONTEXT* crt);

/**
 * @brief Reconstructs polynomial (P & Q base) from the
 * CRT representation to the regular representation.
 *
 * @param res A value list whose values are reconstructed.
 * @param poly rns polynomial
 * @param crt crt primes
 */
void Reconstruct_qpbase_to_values(VALUE_LIST* res, POLYNOMIAL* poly,
                                  CRT_CONTEXT* crt);

/**
 * @brief Get decomposed poly of input index of q part
 *
 * @param res decomposed poly
 * @param poly original polynomial
 * @param num_qpartl number of given q part
 *  may not equal to num_q_part from parameter
 * @param q_part_idx index of q part
 */
void Decompose_poly(POLYNOMIAL* res, POLYNOMIAL* poly, CRT_CONTEXT* crt,
                    size_t num_qpartl, uint32_t q_part_idx);

/**
 * @brief Raise polynomial RNS from Q base to P & Q base
 *
 * @param new_poly raised polynomial (P & Q base)
 * @param old_poly input polynomial (Q base)
 * @param crt crt context
 * @param q_cnt number of q primes
 * @param q_part_idx index of q part
 */
void Raise_rns_base_with_parts(POLYNOMIAL* new_poly, POLYNOMIAL* old_poly,
                               CRT_CONTEXT* crt, size_t q_cnt,
                               size_t q_part_idx);

//! @brief Perform decompose and raise at decomp index
//! @param raised result raised polynomial (P & Q base)
//! @param poly input polynomial (Q base)
//! @param crt crt context
//! @param num_decomp total number of decompose
//! @param decomp_idx decompose index
void Decompose_modup(POLYNOMIAL* raised, POLYNOMIAL* poly, CRT_CONTEXT* crt,
                     size_t num_decomp, size_t decomp_idx);

/**
 * @brief Reduce polynomial RNS from P*Q to Q
 *
 * @param new_poly
 * @param old_poly
 * @param crt
 */
void Reduce_rns_base(POLYNOMIAL* new_poly, POLYNOMIAL* old_poly,
                     CRT_CONTEXT* crt);

//! @brief Rescale polynomial
POLYNOMIAL* Rescale_poly(POLYNOMIAL* res, POLYNOMIAL* ciph, CRT_CONTEXT* crt);

//! @brief Precompute for switch key, note that new memory is returned
VALUE_LIST* Switch_key_precompute(POLYNOMIAL* poly, CRT_CONTEXT* crt);

//! @brief Cleanup VALUE_LIST for precomputation switch key
void Free_switch_key_precomputed(VALUE_LIST* precompute);

//! @brief Sample for polynomial coeffcient with uniform distribution
//! @param poly Sampled polynomial
//! @param q_primes q primes which is used for RNS representation
//! @param p_primes p primes which is used for RNS representation
void Sample_uniform_poly(POLYNOMIAL* poly, VL_CRTPRIME* q_primes,
                         VL_CRTPRIME* p_primes);

//! @brief Sample for polynomial coeffcient with ternary distribution
//! @param poly Sampled polynomial
//! @param q_primes q primes which is used for RNS representation
//! @param p_primes p primes which is used for RNS representation
//! @param hamming_weight Hamming weight for sparse ternary distribution,
//! for uniform distribution the value is zero
void Sample_ternary_poly(POLYNOMIAL* poly, VL_CRTPRIME* q_primes,
                         VL_CRTPRIME* p_primes, size_t hamming_weight);
#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_POLYNOMIAL_H