//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief rt_dynamic.c
//! Dynamic library callback registration implementation for FHE runtime library.
//!
//! This module is only compiled into libFHErt_common.so when BUILD_SHARED=ON.
//! The static library (libFHErt_common.a) does not include this module.

#include <stdlib.h>
#include <string.h>

#include "common/rt_dynamic.h"

/*
 * Global callback registration for dynamic library extensions
 * These callbacks are set by external code and invoked by compiler-generated code
 */
static GET_CONTEXT_PARAMS_FN Get_context_params_cb = NULL;
static GET_RT_DATA_INFO_FN Get_rt_data_info_cb = NULL;
static GET_DECODE_SCHEME_FN Get_decode_scheme_cb = NULL;
static GET_ENCODE_SCHEME_FN Get_encode_scheme_cb = NULL;
static GET_INPUT_COUNT_FN Get_input_count_cb = NULL;
static GET_OUTPUT_COUNT_FN Get_output_count_cb = NULL;

/**
 * Register_runtime_callbacks - Register callback functions for dynamic library extensions
 * @get_context_params: Function to get CKKS context parameters
 * @get_rt_data_info: Function to get runtime data info (weight file info)
 * @get_decode_scheme: Function to get decode scheme for output
 * @get_encode_scheme: Function to get encode scheme for input
 * @get_input_count: Function to get input parameter count
 * @get_output_count: Function to get output parameter count
 */
void Register_runtime_callbacks(
    GET_CONTEXT_PARAMS_FN get_context_params,
    GET_RT_DATA_INFO_FN get_rt_data_info,
    GET_DECODE_SCHEME_FN get_decode_scheme,
    GET_ENCODE_SCHEME_FN get_encode_scheme,
    GET_INPUT_COUNT_FN get_input_count,
    GET_OUTPUT_COUNT_FN get_output_count)
{
  if (get_context_params) Get_context_params_cb = get_context_params;
  if (get_rt_data_info) Get_rt_data_info_cb = get_rt_data_info;
  if (get_decode_scheme) Get_decode_scheme_cb = get_decode_scheme;
  if (get_encode_scheme) Get_encode_scheme_cb = get_encode_scheme;
  if (get_input_count) Get_input_count_cb = get_input_count;
  if (get_output_count) Get_output_count_cb = get_output_count;
}

/**
 * Clear_runtime_callbacks - Clear all registered callbacks
 */
void Clear_runtime_callbacks(void) {
  Get_context_params_cb = NULL;
  Get_rt_data_info_cb = NULL;
  Get_decode_scheme_cb = NULL;
  Get_encode_scheme_cb = NULL;
  Get_input_count_cb = NULL;
  Get_output_count_cb = NULL;
}

/*
 * Wrapper functions that redirect to registered callbacks
 * These maintain compatibility with existing compiler-generated code
 */
CKKS_PARAMS* Get_context_params(void) {
  return Get_context_params_cb ? Get_context_params_cb() : NULL;
}

RT_DATA_INFO* Get_rt_data_info(void) {
  return Get_rt_data_info_cb ? Get_rt_data_info_cb() : NULL;
}

DATA_SCHEME* Get_decode_scheme(int idx) {
  return Get_decode_scheme_cb ? Get_decode_scheme_cb(idx) : NULL;
}

DATA_SCHEME* Get_encode_scheme(int idx) {
  return Get_encode_scheme_cb ? Get_encode_scheme_cb(idx) : NULL;
}

int Get_input_count(void) {
  return Get_input_count_cb ? Get_input_count_cb() : 0;
}

int Get_output_count(void) {
  return Get_output_count_cb ? Get_output_count_cb() : 0;
}