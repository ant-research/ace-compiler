//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "common/rtlib_timing.h"
#include "fhe/core/rt_encode_api.h"
#include "rtlib/context.h"
#include "util/ckks_encoder.h"
#include "util/ckks_parameters.h"

#define ATTRIBUTE_WEAK __attribute__((weak))

// global object for single side
ATTRIBUTE_WEAK CKKS_CONTEXT* Context = NULL;

// for dummy rtlib timing
ATTRIBUTE_WEAK void Append_rtlib_timing(RTLIB_TIMING_ID id, uint64_t nsec) {
  // Do nothing
}

void Prepare_encode_context(uint32_t degree, uint32_t sec_level, uint32_t depth,
                            uint32_t first_mod_size,
                            uint32_t scaling_mod_size) {
  if (Context != NULL) return;

  // generate CKKS Context
  CKKS_CONTEXT* ctxt = (CKKS_CONTEXT*)malloc(sizeof(CKKS_CONTEXT));

  // generate ckks params
  CKKS_PARAMETER* params = Alloc_ckks_parameter();
  Init_ckks_parameters_with_prime_size(
      params, degree, Get_sec_level(sec_level), depth + 1, first_mod_size,
      scaling_mod_size, params->_hamming_weight);

  // generate encoder
  CKKS_ENCODER* encoder = Alloc_ckks_encoder(params);
  ctxt->_params         = (PTR_TY)params;
  ctxt->_encoder        = (PTR_TY)encoder;
  Context               = ctxt;
}

void Finalize_encode_context() {
  if (Context->_params) {
    Free_ckks_parameters((CKKS_PARAMETER*)Context->_params);
    Context->_params = NULL;
  }
  if (Context->_encoder) {
    Free_ckks_encoder((CKKS_ENCODER*)Context->_encoder);
    Context->_encoder = NULL;
  }
  free(Context);
  Context = NULL;
}

CRT_CONTEXT* Get_crt_context() {
  return Get_param_crt((CKKS_PARAMETER*)Get_param(Context));
}
