//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_CIPHERTEXT_H
#define RTLIB_INCLUDE_CIPHERTEXT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/fhe_types.h"
#include "util/public_key.h"

// A module to keep track of a ciphertext.

#ifdef __cplusplus
extern "C" {
#endif

#define DEF_MSG_LEN 10  // default ciphertext message print length

/**
 * @brief This is type for a ciphertext, which consists
 * of two polynomial.
 *
 */
typedef struct {
  POLYNOMIAL _c0_poly;
  POLYNOMIAL _c1_poly;
  uint32_t   _slots;
  double     _scaling_factor;
  uint32_t   _sf_degree;
} CIPHERTEXT;

/**
 * @brief Get the first element of ciphertext
 *
 * @param ciph
 * @return
 */
static inline POLYNOMIAL* Get_c0(CIPHERTEXT* ciph) { return &(ciph->_c0_poly); }

/**
 * @brief Get the second element of ciphertext
 *
 * @param ciph
 * @return
 */
static inline POLYNOMIAL* Get_c1(CIPHERTEXT* ciph) { return &(ciph->_c1_poly); }

/**
 * @brief Get ring degree of polynomial from ciphertext
 *
 * @param ciph
 * @return uint32_t
 */
static inline uint32_t Get_ciph_degree(CIPHERTEXT* ciph) {
  return ciph->_c0_poly._ring_degree;
}

/**
 * @brief Get the number of q prime from ciphertext
 *
 * @param ciph
 * @return size_t
 */
static inline size_t Get_ciph_prime_cnt(CIPHERTEXT* ciph) {
  return ciph->_c0_poly._num_primes;
}

/**
 * @brief Get the number of p prime from ciphertext
 *
 * @param ciph
 * @return size_t
 */
static inline size_t Get_ciph_prime_p_cnt(CIPHERTEXT* ciph) {
  return ciph->_c0_poly._num_primes_p;
}

/**
 * @brief Get scaling factor from ciphertext
 *
 * @param ciph
 * @return double
 */
static inline double Get_ciph_sfactor(CIPHERTEXT* ciph) {
  return ciph->_scaling_factor;
}

/**
 * @brief Set scaling factor for ciphertext
 *
 * @param ciph given ciphertext
 * @param sf given scaling factor
 */
static inline void Set_ciph_sfactor(CIPHERTEXT* ciph, double sf) {
  ciph->_scaling_factor = sf;
}

/**
 * @brief Set degree of scaling factor from ciphertext
 *
 * @param ciph given ciphertext
 * @return uint32_t
 */
static inline uint32_t Get_ciph_sf_degree(CIPHERTEXT* ciph) {
  return ciph->_sf_degree;
}

/**
 * @brief Set degree of scaling factor for ciphertext
 *
 * @param ciph
 * @param degree
 */
static inline void Set_ciph_sf_degree(CIPHERTEXT* ciph, uint32_t degree) {
  ciph->_sf_degree = degree;
}

/**
 * @brief Get slots from ciphertext
 *
 * @param ciph
 * @return uint32_t
 */
static inline uint32_t Get_ciph_slots(CIPHERTEXT* ciph) { return ciph->_slots; }

/**
 * @brief Set slots to ciphertext
 *
 * @param ciph
 * @param slots
 */
static inline void Set_ciph_slots(CIPHERTEXT* ciph, uint32_t slots) {
  ciph->_slots = slots;
}

/**
 * @brief Allocate a ciphertext with malloc
 *
 * @return CIPHERTEXT*
 */
static inline CIPHERTEXT* Alloc_ciphertext() {
  CIPHERTEXT* ciph = (CIPHERTEXT*)malloc(sizeof(CIPHERTEXT));
  memset(ciph, 0, sizeof(CIPHERTEXT));
  return ciph;
}

/**
 * @brief Cleanup ciphertext memory
 *
 * @param ciph
 */
static inline void Free_ciphertext(CIPHERTEXT* ciph) {
  if (ciph == NULL) return;
  Free_poly_data(Get_c0(ciph));
  Free_poly_data(Get_c1(ciph));
  free(ciph);
  ciph = NULL;
}

/**
 * @brief Initialize ciphertext from degree & num_primes & num_primes_p
 *
 * @param res ciphertext to be initialized
 * @param ring_degree Ring degree of polynomial
 * @param num_primes Number of q primes
 * @param num_primes_p Number of [] primes
 * @param scaling_factor Scaling factor
 * @param sf_degree degree of scaling factor for ciphertext
 * @param slots slots of ciphertext
 */
static inline void Init_ciphertext(CIPHERTEXT* res, uint32_t ring_degree,
                                   size_t num_primes, size_t num_primes_p,
                                   double scaling_factor, uint32_t sf_degree,
                                   uint32_t slots) {
  res->_scaling_factor = scaling_factor;
  res->_sf_degree      = sf_degree;
  res->_slots          = slots;
  POLYNOMIAL* c0       = Get_c0(res);
  POLYNOMIAL* c1       = Get_c1(res);
  if (Get_poly_coeffs(c0) == NULL) {
    Alloc_poly_data(c0, ring_degree, num_primes, num_primes_p);
  } else {
    IS_TRUE(Get_rdgree(c0) == ring_degree && Get_num_q(c0) == num_primes,
            "unmatched ciphertxt");
  }
  if (Get_poly_coeffs(c1) == NULL) {
    Alloc_poly_data(Get_c1(res), ring_degree, num_primes, num_primes_p);
  } else {
    IS_TRUE(Get_rdgree(c1) == ring_degree && Get_num_q(c1) == num_primes,
            "unmatched ciphertxt");
  }
}

/**
 * @brief Init ciphertext from another ciphertext
 *
 * @param res Ciphertext to be initialized
 * @param ciph Input ciphertext
 * @param scaling_factor Scaling factor
 * @param sf_degree Degree of scaling factor for ciphertext
 */
static inline void Init_ciphertext_from_ciph(CIPHERTEXT* res, CIPHERTEXT* ciph,
                                             double   scaling_factor,
                                             uint32_t sf_degree) {
  res->_scaling_factor = scaling_factor;
  res->_sf_degree      = sf_degree;
  res->_slots          = ciph->_slots;
  if (res == ciph) return;
  Init_poly(Get_c0(res), Get_c0(ciph));
  Init_poly(Get_c1(res), Get_c1(ciph));
}

/**
 * @brief Initialize ciphertext from poly
 *
 * @param res Ciphertext to be initialized
 * @param poly Input polynomial
 * @param scaling_factor Scaling factor
 * @param sf_degree Degree of scaling factor for ciphertext
 * @param slots Slots of ciphertext
 */
static inline void Init_ciphertext_from_poly(CIPHERTEXT* res, POLYNOMIAL* poly,
                                             double   scaling_factor,
                                             uint32_t sf_degree,
                                             uint32_t slots) {
  res->_scaling_factor = scaling_factor;
  res->_sf_degree      = sf_degree;
  res->_slots          = slots;
  Init_poly(Get_c0(res), poly);
  Init_poly(Get_c1(res), poly);
}

/**
 * @brief copy ciphertext from another ciphertext
 *
 * @param res ciphertext to be initialized
 * @param ciph input ciphertext
 */
static inline void Copy_ciphertext(CIPHERTEXT* res, CIPHERTEXT* ciph) {
  Init_ciphertext_from_ciph(res, ciph, Get_ciph_sfactor(ciph),
                            Get_ciph_sf_degree(ciph));
  Copy_polynomial(Get_c0(res), Get_c0(ciph));
  Copy_polynomial(Get_c1(res), Get_c1(ciph));
}

/**
 * @brief Set level of ciphertext
 *
 * @param ciph
 * @param level
 */
static inline void Set_ciph_level(CIPHERTEXT* ciph, size_t level) {
  POLYNOMIAL* c0 = Get_c0(ciph);
  POLYNOMIAL* c1 = Get_c1(ciph);
  Set_poly_level(c0, level);
  Set_poly_level(c1, level);
}

/**
 * @brief Get the level of ciphertext
 *
 * @param ciph
 * @return size_t
 */
static inline size_t Get_ciph_level(CIPHERTEXT* ciph) {
  POLYNOMIAL* c0 = Get_c0(ciph);
  IS_TRUE(Get_poly_level(c0) == Get_poly_level(Get_c1(ciph)),
          "unmatched level in ciph");
  return Get_poly_level(c0);
}

//! @brief Adjust level of ciph1 & ciph2, return ciph with smaller level
//! @param resize return true if ciph with larger level will be resize
//! @param orig_size the original level of modified ciphertext
static inline CIPHERTEXT* Adjust_level(CIPHERTEXT* ciph1, CIPHERTEXT* ciph2,
                                       bool resize, size_t* orig_size) {
  FMT_ASSERT(ciph1 && ciph2, "invalid ciph");
  if (Get_poly_coeffs(Get_c0(ciph1)) == NULL) {
    return ciph2;
  } else if (Get_poly_coeffs(Get_c0(ciph2)) == NULL) {
    FMT_ASSERT(false, "poly coeffs of input ciph is invalid");
  }
  IS_TRUE(Get_ciph_degree(ciph1) == Get_ciph_degree(ciph2), "degree not match");
  IS_TRUE(Get_num_p(Get_c0(ciph1)) == Get_num_p(Get_c0(ciph2)),
          "p prime not match");
  size_t l1 = Get_ciph_level(ciph1);
  size_t l2 = Get_ciph_level(ciph2);
  IS_TRUE(l1 > 0 && l2 > 0, "invalid q primes");
  if (resize) {
    if (l1 == l2) {
      *orig_size = l1;
      return ciph1;
    } else if (l1 > l2) {
      Set_ciph_level(ciph1, l2);
      *orig_size = l1;
      return ciph1;
    } else {
      Set_ciph_level(ciph2, l1);
      *orig_size = l2;
      return ciph2;
    }
  } else {
    if (l1 == l2) {
      return ciph1;
    } else if (l1 > l2) {
      return ciph2;
    } else {
      return ciph1;
    }
  }
}

//! @brief Reset poly level with given level
static inline void Restore_level(CIPHERTEXT* ciph, size_t level) {
  IS_TRUE(ciph, "null ciph");
  Set_poly_level(Get_c0(ciph), level);
  Set_poly_level(Get_c1(ciph), level);
}

/**
 * @brief print CIPHERTEXT
 *
 * @param fp
 * @param ciph
 */
static inline void Print_ciph(FILE* fp, CIPHERTEXT* ciph) {
  fprintf(fp, "ciph_info: %lf %d %d %ld %ld\n", Get_ciph_sfactor(ciph),
          Get_ciph_sf_degree(ciph), Get_ciph_slots(ciph),
          Get_ciph_prime_cnt(ciph), Get_ciph_prime_p_cnt(ciph));
  fprintf(fp, "c0: ");
  Print_poly(fp, Get_c0(ciph));
  fprintf(fp, "c1: ");
  Print_poly(fp, Get_c1(ciph));
}

//! @brief This is type for a ciphertext3, which consists
//! of three polynomial.
typedef struct {
  POLYNOMIAL _c0_poly;
  POLYNOMIAL _c1_poly;
  POLYNOMIAL _c2_poly;
  uint32_t   _slots;
  double     _scaling_factor;
  uint32_t   _sf_degree;
} CIPHERTEXT3;

//! @brief Get the first element of ciphertext3
static inline POLYNOMIAL* Get_ciph3_c0(CIPHERTEXT3* ciph) {
  return &(ciph->_c0_poly);
}

//! @brief Get the second element of ciphertext3
static inline POLYNOMIAL* Get_ciph3_c1(CIPHERTEXT3* ciph) {
  return &(ciph->_c1_poly);
}

//! @brief Get the third element of ciphertext3
static inline POLYNOMIAL* Get_ciph3_c2(CIPHERTEXT3* ciph) {
  return &(ciph->_c2_poly);
}

//! @brief Get scaling factor for ciphertext3
static inline double Get_ciph3_sfactor(CIPHERTEXT3* ciph) {
  return ciph->_scaling_factor;
}

//! @brief Set scaling factor for ciphertext3
static inline void Set_ciph3_sfactor(CIPHERTEXT3* ciph, double sf) {
  ciph->_scaling_factor = sf;
}

//! @brief Get degree of scaling factor for ciphertext3
static inline uint32_t Get_ciph3_sf_degree(CIPHERTEXT3* ciph) {
  return ciph->_sf_degree;
}

//! @brief Set degree of scaling factor for ciphertext3
static inline void Set_ciph3_sf_degree(CIPHERTEXT3* ciph, uint32_t degree) {
  ciph->_sf_degree = degree;
}

static inline size_t Get_ciph3_prime_p_cnt(CIPHERTEXT3* ciph) {
  return ciph->_c0_poly._num_primes_p;
}

//! @brief Get slots from ciphertext3
static inline uint32_t Get_ciph3_slots(CIPHERTEXT3* ciph) {
  return ciph->_slots;
}

//! @brief Get level of ciphertext3
static inline size_t Get_ciph3_level(CIPHERTEXT3* ciph) {
  POLYNOMIAL* c0 = Get_ciph3_c0(ciph);
  IS_TRUE(Get_poly_level(c0) == Get_poly_level(Get_ciph3_c1(ciph)) &&
              Get_poly_level(c0) == Get_poly_level(Get_ciph3_c2(ciph)),
          "unmatched level in ciph");
  return Get_poly_level(c0);
}

//! @brief Set level of ciphertext3
static inline void Set_ciph3_level(CIPHERTEXT3* ciph, size_t level) {
  Set_poly_level(Get_ciph3_c0(ciph), level);
  Set_poly_level(Get_ciph3_c1(ciph), level);
  Set_poly_level(Get_ciph3_c2(ciph), level);
}

//! @brief Allocate a ciphertext3 with malloc
static inline CIPHERTEXT3* Alloc_ciphertext3() {
  CIPHERTEXT3* ciph = (CIPHERTEXT3*)malloc(sizeof(CIPHERTEXT3));
  memset(ciph, 0, sizeof(CIPHERTEXT3));
  return ciph;
}

//! @brief Cleanup ciphertext3 memory
static inline void Free_ciphertext3(CIPHERTEXT3* ciph) {
  if (ciph == NULL) return;
  Free_poly_data(Get_ciph3_c0(ciph));
  Free_poly_data(Get_ciph3_c1(ciph));
  Free_poly_data(Get_ciph3_c2(ciph));
  free(ciph);
  ciph = NULL;
}

//! @brief Init CIPHERTEXT3 from CIPHERTEXT
static inline void Init_ciphertext3_from_ciph(CIPHERTEXT3* res,
                                              CIPHERTEXT*  ciph,
                                              double       scaling_factor,
                                              uint32_t     sf_degree) {
  res->_scaling_factor = scaling_factor;
  res->_sf_degree      = sf_degree;
  res->_slots          = ciph->_slots;
  IS_TRUE(Get_poly_level(Get_c0(ciph)) == Get_poly_level(Get_c1(ciph)),
          "unmatched level in ciph");
  IS_TRUE(Get_ciph_prime_p_cnt(ciph) == 0, "invalid num of p primes");
  Init_poly(Get_ciph3_c0(res), Get_c0(ciph));
  Init_poly(Get_ciph3_c1(res), Get_c0(ciph));
  Init_poly(Get_ciph3_c2(res), Get_c0(ciph));
}

//! @brief Init CIPHERTEXT3 from CIPHERTEXT
static inline void Init_ciphertext3_from_ciph3(CIPHERTEXT3* res,
                                               CIPHERTEXT3* ciph,
                                               double       scaling_factor,
                                               uint32_t     sf_degree) {
  res->_scaling_factor = scaling_factor;
  res->_sf_degree      = sf_degree;
  res->_slots          = ciph->_slots;
  IS_TRUE(
      Get_poly_level(Get_ciph3_c0(ciph)) == Get_poly_level(Get_ciph3_c1(ciph)),
      "unmatched level in ciph");
  IS_TRUE(
      Get_poly_level(Get_ciph3_c0(ciph)) == Get_poly_level(Get_ciph3_c2(ciph)),
      "unmatched level in ciph");
  IS_TRUE(Get_ciph3_prime_p_cnt(ciph) == 0, "invalid num of p primes");
  Init_poly(Get_ciph3_c0(res), Get_ciph3_c0(ciph));
  Init_poly(Get_ciph3_c1(res), Get_ciph3_c1(ciph));
  Init_poly(Get_ciph3_c2(res), Get_ciph3_c2(ciph));
}

//! @brief Init CIPHERTEXT from CIPHERTEXT3
static inline void Init_ciphertext_from_ciph3(CIPHERTEXT*  res,
                                              CIPHERTEXT3* ciph,
                                              double       scaling_factor,
                                              uint32_t     sf_degree) {
  res->_scaling_factor = scaling_factor;
  res->_sf_degree      = sf_degree;
  res->_slots          = ciph->_slots;
  IS_TRUE(
      Get_poly_level(Get_ciph3_c0(ciph)) == Get_poly_level(Get_ciph3_c1(ciph)),
      "unmatched level in ciph");
  Init_poly(Get_c0(res), Get_ciph3_c0(ciph));
  Init_poly(Get_c1(res), Get_ciph3_c1(ciph));
}

//! @brief Adjust level of ciph1 & ciph2, return ciphtext3 & with smaller level
static inline CIPHERTEXT3* Adjust_ciph3_level(CIPHERTEXT3* ciph1,
                                              CIPHERTEXT3* ciph2, bool resize,
                                              size_t* orig_size) {
  FMT_ASSERT(ciph1 && ciph2, "invalid ciph");
  POLYNOMIAL* ciph1_c0 = Get_ciph3_c0(ciph1);
  POLYNOMIAL* ciph2_c0 = Get_ciph3_c0(ciph2);
  if (Get_poly_coeffs(ciph1_c0) == NULL) {
    return ciph2;
  } else if (Get_poly_coeffs(ciph2_c0) == NULL) {
    FMT_ASSERT(false, "poly coeffs of input ciph is invalid");
  }
  IS_TRUE(Get_rdgree(ciph1_c0) == Get_rdgree(ciph2_c0), "degree not match");
  IS_TRUE(Get_ciph3_sfactor(ciph1) == Get_ciph3_sfactor(ciph2),
          "scaling factors are not equal");
  IS_TRUE(Get_num_p(ciph1_c0) == Get_num_p(ciph2_c0), "p prime not match");
  size_t l1 = Get_ciph3_level(ciph1);
  size_t l2 = Get_ciph3_level(ciph2);
  IS_TRUE(l1 > 0 && l2 > 0, "invalid q primes");
  if (resize) {
    if (l1 == l2) {
      *orig_size = l1;
      return ciph1;
    } else if (l1 > l2) {
      Set_ciph3_level(ciph1, l2);
      *orig_size = l1;
      return ciph1;
    } else {
      Set_ciph3_level(ciph2, l1);
      *orig_size = l2;
      return ciph2;
    }
  } else {
    if (l1 == l2) {
      return ciph1;
    } else if (l1 > l2) {
      return ciph2;
    } else {
      return ciph1;
    }
  }
}

//! @brief print CIPHERTEXT3
static inline void Print_ciph3(FILE* fp, CIPHERTEXT3* ciph) {
  fprintf(fp, "ciph_info: %lf %d %d %ld %ld\n", ciph->_scaling_factor,
          ciph->_sf_degree, ciph->_slots, Get_ciph3_c0(ciph)->_num_primes,
          Get_ciph3_c0(ciph)->_num_primes_p);
  fprintf(fp, "c0: ");
  Print_poly(fp, Get_ciph3_c0(ciph));
  fprintf(fp, "c1: ");
  Print_poly(fp, Get_ciph3_c1(ciph));
  fprintf(fp, "c2: ");
  Print_poly(fp, Get_ciph3_c2(ciph));
}
#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_CIPHERTEXT_H
