//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_SWITCH_KEY_H
#define RTLIB_INCLUDE_SWITCH_KEY_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "util/fhe_types.h"
#include "util/polynomial.h"
#include "util/public_key.h"

// A module to keep track of a switch key.

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief An instance of a switch key
 * The switch key consists of number of q parts public keys
 *
 */
typedef struct {
  VALUE_LIST* _public_keys;  // PTR_TYPE, num of parts public keys
} SWITCH_KEY;

/**
 * @brief Get the switch key at part_idx
 *
 * @param swk input switch key
 * @param part_idx part idx of public keys
 * @return PUBLIC_KEY*
 */
static inline PUBLIC_KEY* Get_swk_at(SWITCH_KEY* swk, size_t part_idx) {
  IS_TRUE(swk && swk->_public_keys, " null swk");
  IS_TRUE(part_idx < LIST_LEN(swk->_public_keys), "idx outof bound");
  return (PUBLIC_KEY*)PTR_VALUE_AT(swk->_public_keys, part_idx);
}

/**
 * @brief Get the size of switch key size
 *
 * @param swk input switch key
 * @return size_t
 */
static inline size_t Get_swk_size(SWITCH_KEY* swk) {
  IS_TRUE(swk && swk->_public_keys, " null swk");
  return LIST_LEN(swk->_public_keys);
}

/**
 * @brief alloc switch key
 *
 * @return SWITCH_KEY*
 */
static inline SWITCH_KEY* Alloc_switch_key() {
  SWITCH_KEY* swk   = (SWITCH_KEY*)malloc(sizeof(SWITCH_KEY));
  swk->_public_keys = NULL;
  return swk;
}

/**
 * @brief initialize switch key
 *
 * @param swk return switch key
 * @param ring_degree ring degree of polynomial
 * @param num_primes number of q primes
 * @param num_primes_p number of p primes
 * @param num_parts number of parts of q primes
 */
static inline void Init_switch_key(SWITCH_KEY* swk, uint32_t ring_degree,
                                   size_t num_primes, size_t num_primes_p,
                                   size_t num_parts) {
  swk->_public_keys = Alloc_value_list(PTR_TYPE, num_parts);
  for (size_t idx = 0; idx < num_parts; idx++) {
    PUBLIC_KEY* pk = Alloc_public_key(ring_degree, num_primes, num_primes_p);
    PTR_VALUE_AT(swk->_public_keys, idx) = (PTR)pk;
  }
}

/**
 * @brief cleanup switch key memory
 *
 * @param swk input switch key
 */
static inline void Free_switch_key(SWITCH_KEY* swk) {
  if (swk == NULL) return;
  if (swk->_public_keys == NULL) return;
  for (size_t idx = 0; idx < LIST_LEN(swk->_public_keys); idx++) {
    PUBLIC_KEY* pk = (PUBLIC_KEY*)PTR_VALUE_AT(swk->_public_keys, idx);
    Free_publickey(pk);
  }
  Free_value_list(swk->_public_keys);
  free(swk);
}

/**
 * @brief Represents switch key as a string
 *
 * @param fp input file
 * @param swk input switch key
 */
static inline void Print_switch_key(FILE* fp, SWITCH_KEY* swk) {
  if (swk == NULL) return;
  if (swk->_public_keys == NULL) return;
  fprintf(fp, "switch keys: \n");
  for (size_t idx = 0; idx < LIST_LEN(swk->_public_keys); idx++) {
    fprintf(fp, "\npart %ld \n", idx);
    PUBLIC_KEY* pk = (PUBLIC_KEY*)PTR_VALUE_AT(swk->_public_keys, idx);
    Print_pk(fp, pk);
  }
}

//! @brief Get memory size of allocated number of switch key
static inline size_t Get_swk_mem_size(SWITCH_KEY* swk) {
  size_t dnum    = Get_swk_size(swk);
  size_t pk_size = Get_pk_mem_size(Get_swk_at(swk, 0));
  return sizeof(SWITCH_KEY) + sizeof(VALUE_LIST) +
         dnum * Value_mem_size(PTR_TYPE) + dnum * pk_size;
}

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_SWITCH_KEY_H
