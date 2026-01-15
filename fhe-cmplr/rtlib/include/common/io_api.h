//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_COMMON_IO_API_H
#define RTLIB_COMMON_IO_API_H

//! @brief io_api.h
//! Define API for input/output manipulation

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @brief Initialize input/output data structure
void Io_init();

//! @brief Finalize input/output data structure
void Io_fini();

//! @brief Set ciphertext to input data, called at client side
void Io_set_input(const char* name, size_t idx, void* ct);

//! @brief Get ciphertext from input data, called at server side
void* Io_get_input(const char* name, size_t idx);

//! @brief Set ciphertext to output data, called at server side
void Io_set_output(const char* name, size_t idx, void* ct);

//! @brief Get ciphertext from output data, called at client side
void* Io_get_output(const char* name, size_t idx);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_COMMON_IO_API_H
