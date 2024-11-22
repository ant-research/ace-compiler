//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_RT_OPENFHE_OPENFHE_API_H
#define RTLIB_RT_OPENFHE_OPENFHE_API_H

//! @brief openfhe_api.h
//! Define OpenFHE API can be used by rtlib

#include <openfhe.h>

#include "common/tensor.h"
#include "rt_def.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @brief OpenFHE API for context management
void       Openfhe_prepare_input(TENSOR* input, const char* name);
void       Openfhe_set_output_data(const char* name, size_t idx, CIPHER data);
CIPHERTEXT Openfhe_get_input_data(const char* name, size_t idx);
void       Openfhe_encode_from_float(PLAIN plain, float* input, size_t len,
                                     uint32_t sc_degree, uint32_t level);
double*    Openfhe_handle_output(const char* name);

//! @brief OpenFHE API for evaluation
void Openfhe_add_ciph(CIPHER res, CIPHER op1, CIPHER op2);
void Openfhe_add_plain(CIPHER res, CIPHER op1, PLAIN op2);
void Openfhe_mul_ciph(CIPHER res, CIPHER op1, CIPHER op2);
void Openfhe_mul_plain(CIPHER res, CIPHER op1, PLAIN op2);
void Openfhe_rotate(CIPHER res, CIPHER op, int step);
void Openfhe_copy(CIPHER res, CIPHER op);
void Openfhe_zero(CIPHER res);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_RT_OPENFHE_OPENFHE_API_H
