//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_RT_SEAL_SEAL_API_H
#define RTLIB_RT_SEAL_SEAL_API_H

//! @brief seal_api.h
//! Define seal API can be used by rtlib

#include <seal/seal.h>

#include "common/tensor.h"
#include "rt_def.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @brief seal API for context management
void       Seal_prepare_input(TENSOR* input, const char* name);
void       Seal_set_output_data(const char* name, size_t idx, CIPHER data);
CIPHERTEXT Seal_get_input_data(const char* name, size_t idx);
void       Seal_encode_from_float(PLAIN plain, float* input, size_t len,
                                  uint32_t sc_degree, uint32_t level);
double*    Seal_handle_output(const char* name);

//! @brief seal API for evaluation
void Seal_add_ciph(CIPHER res, CIPHER op1, CIPHER op2);
void Seal_add_plain(CIPHER res, CIPHER op1, PLAIN op2);
void Seal_mul_ciph(CIPHER res, CIPHER op1, CIPHER op2);
void Seal_mul_plain(CIPHER res, CIPHER op1, PLAIN op2);
void Seal_rotate(CIPHER res, CIPHER op, int step);
void Seal_copy(CIPHER res, CIPHER op);
void Seal_zero(CIPHER res);

uint64_t Seal_sc_degree(CIPHER res);
uint64_t Seal_level(CIPHER res);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_RT_SEAL_SEAL_API_H
