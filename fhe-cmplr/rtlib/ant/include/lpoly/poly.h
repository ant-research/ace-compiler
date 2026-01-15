//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_ANT_INCLUDE_LPOLY_POLY_H
#define RTLIB_ANT_INCLUDE_LPOLY_POLY_H

#include "context/ckks_context.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @brief A lpoly in the ring R_a
typedef struct {
  uint32_t _ring_degree;  // degree d of lpoly that determines the
                          // quotient ring R
  bool     _is_ntt;       // lpoly is ntt or intt
  int64_t* _data;         // array of coefficients in lpoly
} L_POLY;

typedef L_POLY* LPOLY;
typedef L_POLY* LPOLY_ARR;

//! @brief Get ring degree from lpoly
static inline uint32_t Get_lpoly_degree(LPOLY poly) {
  return poly->_ring_degree;
}

//! @brief Get starting address of data of lpoly
static inline int64_t* Get_lpoly_coeffs(LPOLY poly) { return poly->_data; }

//! @brief Check if lpoly is NTT or not
static inline bool Is_lpoly_ntt(LPOLY poly) { return poly->_is_ntt; }

//! @brief Set lpoly is ntt or not
static inline void Set_lpoly_ntt(LPOLY poly, bool v) { poly->_is_ntt = v; }

//! @brief Check if two given lpoly ntt format match
static inline bool Is_lpoly_ntt_match(LPOLY poly1, LPOLY poly2) {
  if (Is_lpoly_ntt(poly1) == Is_lpoly_ntt(poly2)) {
    return true;
  }
  return false;
}

//! @brief Print lpoly
static inline void Print_lpoly(FILE* fp, LPOLY poly) {
  IS_TRUE(poly, "null lpoly");
  fprintf(fp, "is_ntt: %s, raw data: \n", poly->_is_ntt ? "Y" : "N");
  for (int64_t j = 0; j < poly->_ring_degree; j++) {
    fprintf(fp, "%ld ", poly->_data[j]);
  }
  fprintf(fp, "\n");
}

//! @brief Alloc lpoly from given parameters
static inline void Alloc_lpoly_data(LPOLY poly, uint32_t ring_degree) {
  poly->_ring_degree = ring_degree;
  size_t alloc_size  = sizeof(int64_t) * ring_degree;
  poly->_data        = (int64_t*)malloc(alloc_size);
  poly->_is_ntt      = false;
  memset(poly->_data, 0, alloc_size);
}

//! @brief Initialize lpoly with given parameters
static inline void Init_lpoly_data(LPOLY poly, uint32_t ring_degree,
                                   int64_t* data) {
  poly->_ring_degree = ring_degree;
  poly->_data        = data;
  poly->_is_ntt      = FALSE;
}

//! @brief Cleanup data of lpoly
static inline void Free_lpoly_data(LPOLY poly) {
  if (poly->_data) {
    free(poly->_data);
    poly->_data = NULL;
  }
}

//! @brief Alloc lpoly
static inline LPOLY Alloc_lpoly(uint32_t degree) {
  LPOLY poly = (LPOLY)malloc(sizeof(L_POLY));
  Alloc_lpoly_data(poly, degree);
  // hard code for now, ntt should be set by compiler
  Set_lpoly_ntt(poly, true);
  return poly;
}

//! @brief Cleanup lpoly
static inline void Free_lpoly(LPOLY poly) {
  Free_lpoly_data(poly);
  free(poly);
  poly = NULL;
}

//! @brief Clear lpoly
static inline void Zero_lpoly(LPOLY poly) {
  size_t alloc_size = sizeof(int64_t) * Get_lpoly_degree(poly);
  memset(poly->_data, 0, alloc_size);
}

//! @brief Copy lpoly
static inline void Copy_lpoly(LPOLY res, L_POLY poly) {
  memcpy(Get_lpoly_coeffs(res), Get_lpoly_coeffs(&poly),
         sizeof(int64_t) * Get_lpoly_degree(res));
  Set_lpoly_ntt(res, Is_lpoly_ntt(&poly));
}

//! @brief Adds two lpolys
LPOLY Add_lpoly(LPOLY sum, L_POLY poly1, L_POLY poly2, MODULUS* mod);

//! @brief Adds two lpolys
LPOLY Add_scalar(LPOLY res, L_POLY poly, int64_t scalar, MODULUS* mod);

//! @brief Subtracts two lpolys
LPOLY Sub_lpoly(LPOLY res, L_POLY poly1, L_POLY poly2, MODULUS* mod);

//! @brief Subtracts lpoly with scalar
LPOLY Sub_scalar(LPOLY res, L_POLY poly, int64_t scalar, MODULUS* mod);

//! @brief Mulitipy two lpolys
LPOLY Mul_lpoly(LPOLY res, L_POLY poly1, L_POLY poly2, MODULUS* mod);

//! @brief Multiply lpoly with scalar
LPOLY Mul_scalar(LPOLY res, L_POLY poly, int64_t scalar, MODULUS* mod);

//! @brief Adds the product of two lpolys res = res + (poly1 * poly2)
LPOLY Mac_lpoly(LPOLY res, L_POLY poly1, L_POLY poly2, MODULUS* mod);

//! @brief Adds the product of lpoly & scalar res = res + (poly * scalar)
LPOLY Mac_scalar(LPOLY res, L_POLY poly, int64_t scalar, MODULUS* mod);

//! @brief Run NTT for lpoly with given idx
void Ntt(LPOLY res, L_POLY poly, size_t idx);

//! @brief Run INTT for lpoly with given idx
void Intt(LPOLY res, L_POLY poly, size_t idx);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_ANT_INCLUDE_LPOLY_POLY_H
