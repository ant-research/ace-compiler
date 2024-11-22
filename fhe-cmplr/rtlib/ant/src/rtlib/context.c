//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "rtlib/context.h"

#include <stdlib.h>

#include "common/io_api.h"
#include "common/pt_mgr.h"
#include "common/rtlib.h"
#include "common/rtlib_timing.h"
#include "util/ckks_bootstrap_context.h"
#include "util/ckks_decryptor.h"
#include "util/ckks_encoder.h"
#include "util/ckks_encryptor.h"
#include "util/ckks_evaluator.h"
#include "util/ckks_key_generator.h"
#include "util/ckks_parameters.h"
#include "util/fhe_utils.h"

// global object for single side
CKKS_CONTEXT* Context = NULL;

void Prepare_context() {
  Io_init();

  if (Context != NULL) return;

  RTLIB_TM_START(RTM_PREPARE_CONTEXT, rtm);
  // get ctx params
  CKKS_PARAMS* ctx_param = Get_context_params();

  // generate CKKS Context
  CKKS_CONTEXT* ctxt = (CKKS_CONTEXT*)malloc(sizeof(CKKS_CONTEXT));

  // generate ckks params
  CKKS_PARAMETER* params = Alloc_ckks_parameter();
  Set_num_q_parts(params, ctx_param->_num_q_parts);
  Init_ckks_parameters_with_prime_size(
      params, ctx_param->_poly_degree, Get_sec_level(ctx_param->_sec_level),
      ctx_param->_mul_depth + 1, ctx_param->_first_mod_size,
      ctx_param->_scaling_mod_size, ctx_param->_hamming_weight);

  printf(
      "ckks_param: _provider = %d, _poly_degree = %d, _sec_level = %ld, "
      "mul_depth = %ld, _first_mod_size = %ld, _scaling_mod_size = %ld, "
      "_num_q_parts = %ld, _num_p = %ld, _num_rot_idx = %ld,"
      "_hamming_wieght = %ld\n",
      ctx_param->_provider, ctx_param->_poly_degree, ctx_param->_sec_level,
      ctx_param->_mul_depth, ctx_param->_first_mod_size,
      ctx_param->_scaling_mod_size, params->_num_q_parts, params->_num_p_primes,
      ctx_param->_num_rot_idx, ctx_param->_hamming_weight);

  // generate keygen & encoder & encryptor & decryptor
  CKKS_KEY_GENERATOR* keygen = Alloc_ckks_key_generator(
      params, ctx_param->_rot_idxs, ctx_param->_num_rot_idx);
  CKKS_ENCODER*   encoder = Alloc_ckks_encoder(params);
  CKKS_ENCRYPTOR* encryptor =
      Alloc_ckks_encryptor(params, keygen->_public_key, keygen->_secret_key);
  CKKS_DECRYPTOR* decryptor = Alloc_ckks_decryptor(params, keygen->_secret_key);
  CKKS_EVALUATOR* evaluator =
      Alloc_ckks_evaluator(params, encoder, decryptor, keygen);
  ctxt->_params        = (PTR_TY)params;
  ctxt->_key_generator = (PTR_TY)keygen;
  ctxt->_encoder       = (PTR_TY)encoder;
  ctxt->_encryptor     = (PTR_TY)encryptor;
  ctxt->_decryptor     = (PTR_TY)decryptor;
  ctxt->_evaluator     = (PTR_TY)evaluator;

  Context = ctxt;

  // generate bootstrap precom for default slots
  uint32_t default_slots = ctx_param->_poly_degree / 2;
  Bootstrap_precom(default_slots);

  RT_DATA_INFO* data_info = Get_rt_data_info();
  if (data_info != NULL) {
    Pt_mgr_init(data_info->_file_name);
  }
  RTLIB_TM_END(RTM_PREPARE_CONTEXT, rtm);
}

void Finalize_context() {
  RTLIB_TM_START(RTM_FINALIZE_CONTEXT, rtm);
  if (Get_rt_data_info() != NULL) {
    Pt_mgr_fini();
  }

  if (Context->_params) {
    Free_ckks_parameters((CKKS_PARAMETER*)Context->_params);
    Context->_params = NULL;
  }
  if (Context->_key_generator) {
    CKKS_KEY_GENERATOR* key_gen = (CKKS_KEY_GENERATOR*)Context->_key_generator;
    size_t              rot_key_cnt = 0;
    size_t rot_key_size   = Get_rot_key_mem_size(key_gen, &rot_key_cnt);
    size_t total_key_size = Get_total_key_size(key_gen);
    printf(
        "Total memory size for keys: rot_key_cnt = %ld, rot_key_size = %ld "
        "bytes, "
        "total_key_size = %ld bytes\n",
        rot_key_cnt, rot_key_size, total_key_size);
    Free_ckks_key_generator((CKKS_KEY_GENERATOR*)Context->_key_generator);
    Context->_key_generator = NULL;
  }
  if (Context->_encoder) {
    size_t weight_plain_cnt, weight_plain_size;
    Get_weight_plain((CKKS_ENCODER*)Context->_encoder, &weight_plain_size,
                     &weight_plain_cnt);
    printf("Total memory size for weight plain: cnt = %ld, size = %ld bytes\n",
           weight_plain_cnt, weight_plain_size);
    Free_ckks_encoder((CKKS_ENCODER*)Context->_encoder);
    Context->_encoder = NULL;
  }
  if (Context->_encryptor) {
    Free_ckks_encryptor((CKKS_ENCRYPTOR*)Context->_encryptor);
    Context->_encryptor = NULL;
  }
  if (Context->_decryptor) {
    Free_ckks_decryptor((CKKS_DECRYPTOR*)Context->_decryptor);
    Context->_decryptor = NULL;
  }
  if (Context->_evaluator) {
    Free_ckks_evaluator((CKKS_EVALUATOR*)Context->_evaluator);
    Context->_evaluator = NULL;
  }
  free(Context);
  Context = NULL;
  RTLIB_TM_END(RTM_FINALIZE_CONTEXT, rtm);
  RTLIB_TM_REPORT();
  Io_fini();
  Close_trace_file();
}

uint32_t Degree() {
  return Get_param_degree((CKKS_PARAMETER*)Get_param(Context));
}

double Get_default_sc() {
  return Get_param_sc((CKKS_PARAMETER*)Get_param(Context));
}

size_t Get_q_parts() {
  return Get_num_q_parts((CKKS_PARAMETER*)Get_param(Context));
}

CRT_CONTEXT* Get_crt_context() {
  return Get_param_crt((CKKS_PARAMETER*)Get_param(Context));
}

size_t Get_p_cnt() { return Get_crt_num_p(Get_crt_context()); }

MODULUS* Q_modulus() { return Get_q_modulus_head(Get_crt_context()); }

MODULUS* P_modulus() { return Get_p_modulus_head(Get_crt_context()); }

void Bootstrap_precom(uint32_t num_slots) {
  VL_UI32* level_budget          = Alloc_value_list(UI32_TYPE, 2);
  UI32_VALUE_AT(level_budget, 0) = 3;
  UI32_VALUE_AT(level_budget, 1) = 3;
  CKKS_PARAMETER* param          = (CKKS_PARAMETER*)Get_param(Context);
  uint32_t bts_depth = Get_bootstrap_depth(level_budget, 1 /* num_iteration */,
                                           param->_hamming_weight);
  if (Get_mult_depth(param) > bts_depth) {
    // step 1: bootstrap setup
    RTLIB_TM_START(RTM_BS_SETUP, bs_setup);
    VL_UI32* dim1          = Alloc_value_list(UI32_TYPE, 2);
    UI32_VALUE_AT(dim1, 0) = 0;
    UI32_VALUE_AT(dim1, 1) = 0;
    CKKS_BTS_CTX* bts_ctx  = Get_bts_ctx((CKKS_EVALUATOR*)Context->_evaluator);
    Bootstrap_setup(bts_ctx, level_budget, dim1, num_slots);
    Free_value_list(dim1);
    RTLIB_TM_END(RTM_BS_SETUP, bs_setup);
    // step 2: bootstrap keygen
    RTLIB_TM_START(RTM_BS_KEYGEN, bs_keygen);
    Bootstrap_keygen(bts_ctx, num_slots);
    RTLIB_TM_END(RTM_BS_KEYGEN, bs_keygen);
  }
  Free_value_list(level_budget);
}
