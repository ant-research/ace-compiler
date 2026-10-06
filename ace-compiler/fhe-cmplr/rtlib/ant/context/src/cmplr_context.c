
//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "ckks/encoder.h"
#include "ckks/param.h"
#include "common/cmplr_api.h"
#include "context/ckks_context.h"

//! @brief cmplr_context.c implements APIs for compiler use, it will
//! be incorporated into a library that is linked with the compiler.

#define ATTRIBUTE_WEAK __attribute__((weak))

// global object for single side
ATTRIBUTE_WEAK CKKS_CONTEXT* Context = NULL;

void Prepare_context_for_cmplr(uint32_t degree, uint32_t sec_level,
                               uint32_t depth, uint32_t input_lev,
                               uint32_t first_mod_size,
                               uint32_t scaling_mod_size, uint32_t num_q_parts,
                               uint32_t hamming_weight) {
  if (Context != NULL) return;

  // generate CKKS Context
  CKKS_CONTEXT* ctxt = (CKKS_CONTEXT*)malloc(sizeof(CKKS_CONTEXT));

  // generate ckks params
  CKKS_PARAMETER* params = Alloc_ckks_parameter();
  Set_num_q_parts(params, num_q_parts);
  Init_ckks_parameters_with_prime_size(params, degree, Get_sec_level(sec_level),
                                       depth + 1, input_lev, first_mod_size,
                                       scaling_mod_size, hamming_weight);

  // generate encoder
  CKKS_ENCODER* encoder = Alloc_ckks_encoder(params);
  ctxt->_params         = (PTR_TY)params;
  ctxt->_encoder        = (PTR_TY)encoder;
  Context               = ctxt;
}

void Finalize_context_for_cmplr() {
  if (Context == NULL) return;
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
  return Get_param_crt((CKKS_PARAMETER*)Param());
}

double Get_default_sc() { return Get_param_sc((CKKS_PARAMETER*)Param()); }

size_t Get_q_cnt() { return Get_crt_num_q(Get_crt_context()); }

size_t Get_p_cnt() { return Get_crt_num_p(Get_crt_context()); }

MODULUS* Q_modulus() { return Get_q_modulus_head(Get_crt_context()); }

MODULUS* P_modulus() { return Get_p_modulus_head(Get_crt_context()); }

size_t Get_qlhmodp_dim3_cnt(uint32_t q_idx, uint32_t part_idx) {
  VL_VL_VL_VL_I64* qlhmodp = Get_qlhatmodp(Get_qpart(Get_crt_context()));
  VL_VL_I64*       vl      = Get_vl_l2_value_at(qlhmodp, q_idx, part_idx);
  return LIST_LEN(vl);
}

size_t Get_phmodq_dim1_cnt() {
  VL_VL_I64* phmodq = Get_phatmodq(Get_p(Get_crt_context()));
  return LIST_LEN(phmodq);
}

size_t Fetch_q_primes(uint64_t* primes) {
  VL_CRTPRIME* q_primes = Get_q_primes(Get_crt_context());
  FOR_ALL_ELEM(q_primes, idx) {
    CRT_PRIME* prime = Get_vlprime_at(q_primes, idx);
    primes[idx]      = (uint64_t)Get_modulus_val(prime);
  }
  return LIST_LEN(q_primes);
}

size_t Fetch_p_primes(uint64_t* primes) {
  VL_CRTPRIME* p_primes = Get_p_primes(Get_crt_context());
  FOR_ALL_ELEM(p_primes, idx) {
    CRT_PRIME* prime = Get_vlprime_at(p_primes, idx);
    primes[idx]      = (uint64_t)Get_modulus_val(prime);
  }
  return LIST_LEN(p_primes);
}

void Fetch_qlhinvmodq_at(const uint64_t** vals, size_t* cnt, uint32_t part_idx,
                         uint32_t part_size_idx) {
  VL_VL_VL_I64* qlhinvmodq = Get_qlhatinvmodq(Get_qpart(Get_crt_context()));
  VL_I64*       vl = Get_vl_l2_value_at(qlhinvmodq, part_idx, part_size_idx);
  *vals            = (uint64_t*)Get_i64_values(vl);
  *cnt             = LIST_LEN(vl);
}

void Fetch_qlhmodp_at(const uint64_t** vals, size_t* cnt, uint32_t q_idx,
                      uint32_t part_idx, uint32_t part_size_idx) {
  VL_VL_VL_VL_I64* qlhmodp = Get_qlhatmodp(Get_qpart(Get_crt_context()));
  VL_I64* vl = Get_vl_l3_value_at(qlhmodp, q_idx, part_idx, part_size_idx);
  *vals      = (uint64_t*)Get_i64_values(vl);
  *cnt       = LIST_LEN(vl);
}

void Fetch_qlinvmodq_at(const uint64_t** vals, size_t* cnt, uint32_t idx) {
  VL_I64* vl = Get_ql_inv_mod_qi_at(Get_q(Get_crt_context()), idx);
  *vals      = (uint64_t*)Get_i64_values(vl);
  *cnt       = LIST_LEN(vl);
}

void Fetch_phmodq_at(const uint64_t** vals, size_t* cnt, uint32_t idx) {
  VL_I64* vl = Get_phatmodq_at(Get_p(Get_crt_context()), idx);
  *vals      = (uint64_t*)Get_i64_values(vl);
  *cnt       = LIST_LEN(vl);
}

void Fetch_qlhalfmodq_at(const uint64_t** vals, size_t* cnt, uint32_t idx) {
  VL_I64* vl = Get_ql_div2_mod_qi_at(Get_q(Get_crt_context()), idx);
  *vals      = (uint64_t*)Get_i64_values(vl);
  *cnt       = LIST_LEN(vl);
}

void Fetch_phinvmodp(const uint64_t** vals, size_t* cnt) {
  VL_I64* vl = Get_phatinvmodp(Get_p(Get_crt_context()));
  *vals      = (uint64_t*)Get_i64_values(vl);
  *cnt       = LIST_LEN(vl);
}

void Fetch_pinvmodq(const uint64_t** vals, size_t* cnt) {
  VL_I64* vl = Get_pinvmodq(Get_p(Get_crt_context()));
  *vals      = (uint64_t*)Get_i64_values(vl);
  *cnt       = LIST_LEN(vl);
}
