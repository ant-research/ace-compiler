//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "context/ckks_context.h"

#include <stdlib.h>

#include "ckks/bootstrap.h"
#include "ckks/decryptor.h"
#include "ckks/encoder.h"
#include "ckks/encryptor.h"
#include "ckks/evaluator.h"
#include "ckks/key_gen.h"
#include "ckks/param.h"
#include "common/io_api.h"
#include "common/pt_mgr.h"
#include "common/rtlib.h"
#include "common/rtlib_timing.h"
#include "util/modular.h"

// global object for single side
CKKS_CONTEXT* Context = NULL;

static int Contains_rot_idx(const int32_t* rot_idxs, size_t num_rot_idx,
                            int32_t rot_idx) {
  for (size_t i = 0; i < num_rot_idx; ++i) {
    if (rot_idxs[i] == rot_idx) {
      return 1;
    }
  }
  return 0;
}

static CKKS_PARAMS* Merge_context_params(CKKS_PARAMS* base, CKKS_PARAMS* extra) {
  if (extra == NULL || extra->_num_rot_idx == 0) {
    return NULL;
  }
  if (base->_provider != extra->_provider ||
      base->_poly_degree != extra->_poly_degree) {
    fprintf(stderr,
            "ERROR: extra context params mismatch: provider/degree differ\n");
    abort();
  }

  size_t merged_rot_cnt = base->_num_rot_idx;
  for (size_t i = 0; i < extra->_num_rot_idx; ++i) {
    if (!Contains_rot_idx(base->_rot_idxs, base->_num_rot_idx,
                          extra->_rot_idxs[i])) {
      ++merged_rot_cnt;
    }
  }
  if (merged_rot_cnt == base->_num_rot_idx) {
    return NULL;
  }

  CKKS_PARAMS* merged =
      (CKKS_PARAMS*)malloc(sizeof(CKKS_PARAMS) +
                           merged_rot_cnt * sizeof(int32_t));
  if (merged == NULL) {
    fprintf(stderr, "ERROR: failed to allocate merged context params\n");
    abort();
  }

  *merged               = *base;
  merged->_num_rot_idx  = merged_rot_cnt;
  size_t merged_rot_idx = 0;
  for (size_t i = 0; i < base->_num_rot_idx; ++i) {
    merged->_rot_idxs[merged_rot_idx++] = base->_rot_idxs[i];
  }
  for (size_t i = 0; i < extra->_num_rot_idx; ++i) {
    int32_t rot_idx = extra->_rot_idxs[i];
    if (!Contains_rot_idx(merged->_rot_idxs, merged_rot_idx, rot_idx)) {
      merged->_rot_idxs[merged_rot_idx++] = rot_idx;
    }
  }
  return merged;
}

void Prepare_context() {
  Init_rtlib_timing();
  Io_init();

  if (Context != NULL) return;

  RTLIB_TM_START(RTM_PREPARE_CONTEXT, rtm);
  // get ctx params
  CKKS_PARAMS* base_ctx_param = Get_context_params();
  CKKS_PARAMS* ctx_param      = base_ctx_param;
  CKKS_PARAMS* merged_ctx_param =
      (Get_extra_context_params != NULL)
          ? Merge_context_params(base_ctx_param, Get_extra_context_params())
          : NULL;
  if (merged_ctx_param != NULL) {
    ctx_param = merged_ctx_param;
  }

  // generate CKKS Context
  Context = Alloc_ckks_context();

  // generate ckks params
  CKKS_PARAMETER* params = Alloc_ckks_parameter();
  Set_num_q_parts(params, ctx_param->_num_q_parts);
  Init_ckks_parameters_with_prime_size(
      params, ctx_param->_poly_degree, Get_sec_level(ctx_param->_sec_level),
      ctx_param->_mul_depth + 1, ctx_param->_input_level,
      ctx_param->_first_mod_size, ctx_param->_scaling_mod_size,
      ctx_param->_hamming_weight);
  Context->_params = (PTR_TY)params;

  printf(
      "ckks_param: _provider = %d, _poly_degree = %d, "
      "mul_depth = %ld, input_lev = %ld, _first_mod_size = %ld, "
      "_scaling_mod_size = %ld, "
      "_num_q_parts = %ld, _num_p = %ld, _num_rot_idx = %ld,"
      "_hamming_weight = %ld\n",
      ctx_param->_provider, ctx_param->_poly_degree, ctx_param->_mul_depth,
      ctx_param->_input_level, ctx_param->_first_mod_size,
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
  Context->_key_generator = (PTR_TY)keygen;
  Context->_encoder       = (PTR_TY)encoder;
  Context->_encryptor     = (PTR_TY)encryptor;
  Context->_decryptor     = (PTR_TY)decryptor;
  Context->_evaluator     = (PTR_TY)evaluator;

  // Skip runtime bootstrap precompute when the generated program does not use
  // rtlib bootstrap entry points. This is useful for the decomposition-based
  // DSL path, which has its own explicit bootstrap body.
  const char* disable_bts_precom = getenv("RTLIB_DISABLE_BOOTSTRAP_PRECOM");
  if (!(disable_bts_precom != NULL && disable_bts_precom[0] != '\0' &&
        disable_bts_precom[0] != '0')) {
    uint32_t default_slots = ctx_param->_poly_degree / 2;
    Bootstrap_precom(default_slots);
  }

  RT_DATA_INFO* data_info = Get_rt_data_info();
  if (data_info != NULL) {
    Pt_mgr_init(data_info->_file_name);
  }
  if (merged_ctx_param != NULL) {
    free(merged_ctx_param);
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

uint32_t Degree() { return Get_param_degree((CKKS_PARAMETER*)Param()); }

size_t Get_q_parts() { return Get_num_q_parts((CKKS_PARAMETER*)Param()); }

/*
 * FIXME: These functions are declared extern to avoid duplicate definition errors.
 * They are defined in another translation unit (likely generated by compiler).
 * TODO: Clean up the root cause of duplicate definitions.
 */
extern double Get_default_sc(void);
extern CRT_CONTEXT* Get_crt_context(void);
extern size_t Get_q_cnt(void);
extern size_t Get_p_cnt(void);
extern MODULUS* Q_modulus(void);
extern MODULUS* P_modulus(void);

NTT_CONTEXT* Get_ntt_ctx(size_t idx) {
  size_t num_q = Get_q_cnt();
  FMT_ASSERT(idx < num_q + Get_p_cnt(), "idx overflow");
  CRT_CONTEXT* crt = Get_crt_context();
  NTT_CONTEXT* ntt = idx < num_q
                         ? Get_ntt(Get_prime_at(Get_q(crt), idx))
                         : Get_ntt(Get_prime_at(Get_p(crt), idx - num_q));
  return ntt;
}

void Bootstrap_precom(uint32_t num_slots) {
  VL_UI32* level_budget          = Alloc_value_list(UI32_TYPE, 2);
  UI32_VALUE_AT(level_budget, 0) = 3;
  UI32_VALUE_AT(level_budget, 1) = 3;
  CKKS_PARAMETER* param          = (CKKS_PARAMETER*)Param();
  uint32_t        bts_depth =
      Get_bootstrap_depth(level_budget, param->_hamming_weight);
  if (Get_mult_depth(param) > bts_depth) {
    // step 1: bootstrap setup
    RTLIB_TM_START(RTM_BS_SETUP, bs_setup);
    VL_UI32* dim1          = Alloc_value_list(UI32_TYPE, 2);
    UI32_VALUE_AT(dim1, 0) = 0;
    UI32_VALUE_AT(dim1, 1) = 0;
    CKKS_BTS_CTX* bts_ctx  = Get_bts_ctx((CKKS_EVALUATOR*)Eval());
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

void Set_global_crt_context(PTR_TY crt) {
  if (Context == NULL) {
    CKKS_CONTEXT* ctxt = Alloc_ckks_context();
    Context            = ctxt;
  }
  if (Context->_params == NULL) {
    CKKS_PARAMETER* params = Alloc_ckks_parameter();
    Context->_params       = (PTR_TY)params;
  }
  Set_param_crt((CKKS_PARAMETER*)Param(), (CRT_CONTEXT*)crt);
}

void Set_global_params(PTR_TY params) {
  if (Context == NULL) {
    CKKS_CONTEXT* ctxt = Alloc_ckks_context();
    Context            = ctxt;
  }
  CKKS_PARAMETER* global_params = (CKKS_PARAMETER*)Param();
  if (global_params) {
    Free_ckks_parameters(global_params);
  }
  Context->_params = params;
}

void Set_global_encoder(PTR_TY encoder) {
  if (Context == NULL) {
    CKKS_CONTEXT* ctxt = Alloc_ckks_context();
    Context            = ctxt;
  }
  CKKS_ENCODER* global_encoder = (CKKS_ENCODER*)Encoder();
  if (global_encoder) {
    Free_ckks_encoder(global_encoder);
  }
  Context->_encoder = encoder;
}
