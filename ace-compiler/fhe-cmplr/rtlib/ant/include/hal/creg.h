//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_ANT_INCLUDE_HAL_CREG
#define RTLIB_ANT_INCLUDE_HAL_CREG

#include <stdint.h>

#include "util/modular.h"
#include "util/type.h"

//! @brief defines APIs for HPU CREG instructions
#ifdef __cplusplus
extern "C" {
#endif
//! @brief Generate the modulus register for all Q and P primes
//! @param qmuk_data array pointer that stores the generated modulus value
//! @param size The size of the array to check write outof bound
void Gen_mod3s_regs(int64_t* qmuk_data, size_t size);

//! @brief Calculate the G and K value for the given rot_idx, poly_degree, mode
//! @param rot_idx: rotation index (input)
//! @param poly_degree(N): degree of the polynomial (input)
//! @param mode: 0 for NTT, 1 for COE (input)
//! @return DSA_CR_AUTOU value (output)
int64_t Gen_autou_creg(int32_t rot_idx, uint64_t poly_degree, bool mode);

//! @brief Get modulus for qmuk_data at given idx
//! @param qmuk_data array pointer that stores the generated modulus value
//! @param idx index of Q and P primes
MODULUS* Ldmod(uint64_t* qmuk_data, size_t idx);

#ifdef __cplusplus
}
#endif

#endif