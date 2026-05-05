//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef ACE_RUNTIME_RTLIB_INTERFACE_H
#define ACE_RUNTIME_RTLIB_INTERFACE_H

#include "common/rtlib.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Get CKKS context parameters.
 * @return Pointer to CKKS_PARAMS structure, or nullptr if not configured
 */
CKKS_PARAMS* get_context_params_c(void);

/**
 * @brief Get runtime data information.
 * @return Pointer to RT_DATA_INFO structure
 */
RT_DATA_INFO* get_rt_data_info_c(void);

/**
 * @brief Get encoding scheme for input at specified index.
 * @param idx Input index
 * @return Pointer to DATA_SCHEME structure, or nullptr if index out of range
 */
DATA_SCHEME* get_encode_scheme_c(int idx);

/**
 * @brief Get decoding scheme for output at specified index.
 * @param idx Output index
 * @return Pointer to DATA_SCHEME structure, or nullptr if index out of range
 */
DATA_SCHEME* get_decode_scheme_c(int idx);

/**
 * @brief Get number of inputs.
 * @return Number of inputs
 */
int get_input_cnt(void);

/**
 * @brief Get number of outputs.
 * @return Number of outputs
 */
int get_output_cnt(void);

/**
 * @brief Register configuration callback functions with runtime library.
 *
 * This function registers all the getter callbacks with the FHE runtime
 * library, enabling it to retrieve configuration data during execution.
 */
void Register_config_func(void);

#ifdef __cplusplus
}
#endif

#endif // ACE_RUNTIME_RTLIB_INTERFACE_H