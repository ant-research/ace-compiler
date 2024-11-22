//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "poly/poly_eval.h"

POLY Decomp(POLY res, POLY poly, uint32_t q_part_idx) {
  RTLIB_TM_START(RTM_DECOMP, rtm);
  CRT_CONTEXT* crt = Get_crt_context();
  Decompose_poly(res, poly, crt, Num_decomp(poly), q_part_idx);
  RTLIB_TM_END(RTM_DECOMP, rtm);
  return res;
}

POLY Mod_up(POLY new_poly, POLY old_poly, uint32_t q_part_idx) {
  RTLIB_TM_START(RTM_MOD_UP, rtm);
  CRT_CONTEXT* crt = Get_crt_context();
  Raise_rns_base_with_parts(new_poly, old_poly, crt, Poly_level(new_poly),
                            q_part_idx);
  RTLIB_TM_END(RTM_MOD_UP, rtm);
  return new_poly;
}

POLY Decomp_modup(POLY res, POLY poly, uint32_t q_part_idx) {
  RTLIB_TM_START(RTM_DECOMP_MODUP, rtm);
  CRT_CONTEXT* crt = Get_crt_context();
  Decompose_modup(res, poly, crt, Num_decomp(poly), q_part_idx);
  RTLIB_TM_END(RTM_DECOMP_MODUP, rtm);
  return res;
}

POLY Mod_down(POLY res, POLY poly) {
  RTLIB_TM_START(RTM_MOD_DOWN, rtm);
  Reduce_rns_base(res, poly, Get_crt_context());
  RTLIB_TM_END(RTM_MOD_DOWN, rtm);
  return res;
}

POLY Rescale(POLY res, POLY poly) {
  RTLIB_TM_START(RTM_RESCALE_POLY, rtm);
  Rescale_poly(res, poly, Get_crt_context());
  Mod_down_q_primes(res);
  RTLIB_TM_END(RTM_RESCALE_POLY, rtm);
  return res;
}

void Print_poly_lite(FILE* fp, POLYNOMIAL* input) {
  const uint32_t def_coeff_len = 8;
  const uint32_t def_level_len = 3;

  POLYNOMIAL p;
  p._data          = NULL;
  POLYNOMIAL* poly = &p;
  Init_poly(poly, input);
  if (input->_is_ntt) {
    Conv_ntt2poly(poly, input, Get_crt_context());
  } else {
    Copy_poly(poly, input);
  }
  uint32_t max_level =
      poly->_num_primes > def_level_len ? def_level_len : poly->_num_primes;
  for (size_t i = 0; i < max_level; i++) {
    fprintf(fp, "Q%ld: [", i);
    int64_t* coeffs = poly->_data + i * poly->_ring_degree;
    for (int64_t j = 0; j < def_coeff_len && j < poly->_ring_degree; j++) {
      fprintf(fp, "%ld ", coeffs[j]);
    }
    fprintf(fp, " ]");
    fprintf(fp, "\n");
  }

  max_level =
      poly->_num_primes_p > def_level_len ? def_level_len : poly->_num_primes_p;
  for (size_t i = 0; i < max_level; i++) {
    fprintf(fp, "P%ld: [", i);
    int64_t* coeffs =
        poly->_data + (poly->_num_primes + i) * poly->_ring_degree;
    for (int64_t j = 0; j < def_coeff_len && j < poly->_ring_degree; j++) {
      fprintf(fp, "%ld ", coeffs[j]);
    }
    fprintf(fp, " ]");
    fprintf(fp, "\n");
  }
  Free_poly_data(poly);
}
