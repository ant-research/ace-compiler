//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_ANT_UTIL_INCLUDE_MODULAR_IMPL_H
#define RTLIB_ANT_UTIL_INCLUDE_MODULAR_IMPL_H

#include <stdbool.h>

#include "util/modular.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @brief Alloc MODULUS for given size
static inline MODULUS* Alloc_modulus(size_t n) {
  return (MODULUS*)malloc(sizeof(MODULUS) * n);
}

//! @brief Print modulus
static inline void Print_modulus(FILE* fp, MODULUS* m) {
  if (m == NULL) return;
  fprintf(fp, "{val = %ld, br_k = %ld, br_m = %ld} ", Get_mod_val(m),
          Get_br_k(m), Get_br_m(m));
}

//! @brief Check if the value is a power of two
static inline bool Is_power_of_two(int64_t value) {
  if (value == 0 || (value & (value - 1)) != 0) {
    return FALSE;
  }
  return TRUE;
}

#ifdef __cplusplus
}
#endif
#endif  // RTLIB_ANT_UTIL_INCLUDE_MODULAR_IMPL_H