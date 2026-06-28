//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_COMMON_RT_DYNAMIC_H
#define RTLIB_COMMON_RT_DYNAMIC_H

#ifdef RTLIB_SUPPORT_DYNAMIC

//! @brief rt_dynamic.h
//! Dynamic library callback registration API for FHE runtime library.
//!
//! This module provides callback registration mechanism for dynamic library
//! extensions (e.g., Python bindings) to integrate with compiler-generated
//! FHE code. The callbacks allow external code to provide runtime data
//! (context params, schemes, etc.) to the FHE runtime.
//!
//! NOTE: This module is only built into the shared library (libFHErt_common.so)
//! when BUILD_SHARED=ON. The static library (libFHErt_common.a) does not include
//! this module to keep it clean for bare-metal/Linux builds.
//!
//! Usage:
//!   1. Extension implements callback functions
//!   2. Extension calls Register_runtime_callbacks() at initialization
//!   3. Compiler-generated code calls Get_*() functions to retrieve data

#include "common.h"
#include "tensor.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Callback function types for dynamic library extensions
 * These are registered by external code and called by compiler-generated code
 */

//! @brief Callback to get CKKS context parameters
typedef CKKS_PARAMS* (*GET_CONTEXT_PARAMS_FN)(void);

//! @brief Callback to get runtime data info (weight file info)
typedef RT_DATA_INFO* (*GET_RT_DATA_INFO_FN)(void);

//! @brief Callback to get decode scheme for output at given index
typedef DATA_SCHEME* (*GET_DECODE_SCHEME_FN)(int idx);

//! @brief Callback to get encode scheme for input at given index
typedef DATA_SCHEME* (*GET_ENCODE_SCHEME_FN)(int idx);

//! @brief Callback to get input parameter count
typedef int (*GET_INPUT_COUNT_FN)(void);

//! @brief Callback to get output parameter count
typedef int (*GET_OUTPUT_COUNT_FN)(void);

/**
 * Register_runtime_callbacks - Register callback functions for dynamic library extensions
 *
 * This function must be called by extension code during initialization
 * before any FHE computation. Pass NULL for callbacks that are not needed.
 *
 * @param get_context_params: Function to get CKKS context parameters
 * @param get_rt_data_info: Function to get runtime data info (weight file info)
 * @param get_decode_scheme: Function to get decode scheme for output
 * @param get_encode_scheme: Function to get encode scheme for input
 * @param get_input_count: Function to get input parameter count
 * @param get_output_count: Function to get output parameter count
 *
 * Example:
 *   Register_runtime_callbacks(
 *       my_get_context_params,
 *       my_get_rt_data_info,
 *       my_get_decode_scheme,
 *       my_get_encode_scheme,
 *       my_get_input_count,
 *       my_get_output_count
 *   );
 */
void Register_runtime_callbacks(
    GET_CONTEXT_PARAMS_FN get_context_params,
    GET_RT_DATA_INFO_FN get_rt_data_info,
    GET_DECODE_SCHEME_FN get_decode_scheme,
    GET_ENCODE_SCHEME_FN get_encode_scheme,
    GET_INPUT_COUNT_FN get_input_count,
    GET_OUTPUT_COUNT_FN get_output_count);

/**
 * Clear_runtime_callbacks - Clear all registered callbacks
 *
 * This function can be called during cleanup to reset all callbacks to NULL.
 * Useful for testing or when reloading dynamic library extensions.
 */
void Clear_runtime_callbacks(void);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_SUPPORT_DYNAMIC

#endif  // RTLIB_COMMON_RT_DYNAMIC_H