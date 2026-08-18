//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "poly/poly_eval.h"

#include "common/error.h"
#include "common/rt_config.h"
#include "util/polynomial.h"

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
  FMT_ASSERT(Get_poly_level(poly) >= 2, "Level of rescale opnd is too small");
  Rescale_poly(res, poly, Get_crt_context());
  Mod_down_q_primes(res);
  RTLIB_TM_END(RTM_RESCALE_POLY, rtm);
  return res;
}

POLY Modswitch(POLY res, POLY poly) {
  uint32_t level = Get_poly_level(poly);
  FMT_ASSERT(level >= 2, "polynomial level is too small to support modswitch");
  Set_poly_level(res, level - 1);
  if (res != poly) {
    Copy_low_level_polynomial(res, poly);
  }
}
//! @brief Precompute for key switch
POLY* Precomp(POLY input) {
  RTLIB_TM_START(RTM_PRECOMP, rtm);
  VALUE_LIST* poly_list = Switch_key_precompute(input, Get_crt_context());
  POLY*       ret       = (POLY*)Get_ptr_values(poly_list);
  free(poly_list);
  RTLIB_TM_END(RTM_PRECOMP, rtm);
  return ret;
}

POLY Extend(POLY res, POLY poly) {
  RTLIB_TM_START(RTM_EXTEND, rtm);
  CRT_CONTEXT* crt = Get_crt_context();
  if (Get_poly_coeffs(poly) != NULL) {
    memset(Get_poly_coeffs(res), 0, Get_poly_mem_size(res));
    // reset size
    res->_num_alloc_primes = Get_num_q(poly) + Get_crt_num_p(crt);
    Scalars_integer_multiply_poly(res, poly, Get_pmodq(Get_p(crt)), Get_q(crt),
                                  NULL);
  }
  RTLIB_TM_END(RTM_EXTEND, rtm);
  return res;
}

//! @brief Fast Dot prod for key switch with lazy mod
POLY Fast_dot_prod(POLY res, POLY* p0, POLY* p1, uint32_t num_part) {
  RTLIB_TM_START(RTM_FAST_DOT_PROD, rtm);
  CRT_CONTEXT* crt = Get_crt_context();
  Init_poly(res, p0[0]);
  POLYNOMIAL** p1_adjust =
      (POLYNOMIAL**)malloc((sizeof(POLYNOMIAL*)) * num_part);
  POLYNOMIAL poly[num_part];
  for (uint32_t idx = 0; idx < num_part; idx++) {
    p1_adjust[idx] = &poly[idx];
    Derive_poly(&poly[idx], p1[idx], Get_poly_level(res), Get_num_p(res));
  }
  Fast_dotprod_poly(res, p0, p1_adjust, Get_q_primes(crt), Get_p_primes(crt),
                    num_part);
  free(p1_adjust);
  RTLIB_TM_END(RTM_FAST_DOT_PROD, rtm);
  return res;
}

//! @brief Dot prod for key switch
POLY Dot_prod(POLY res, POLY* p0, POLY* p1, uint32_t num_part) {
  if (Get_rtlib_config(CONF_FAST_DOT_PROD)) {
    return Fast_dot_prod(res, p0, p1, num_part);
  }
  RTLIB_TM_START(RTM_DOT_PROD, rtm);
  CRT_CONTEXT* crt = Get_crt_context();
  Init_poly(res, p0[0]);
  POLYNOMIAL** p1_adjust =
      (POLYNOMIAL**)malloc((sizeof(POLYNOMIAL*)) * num_part);
  POLYNOMIAL poly[num_part];
  for (uint32_t idx = 0; idx < num_part; idx++) {
    p1_adjust[idx] = &poly[idx];
    Derive_poly(&poly[idx], p1[idx], Get_poly_level(res), Get_num_p(res));
  }
  Dotprod_poly(res, p0, p1_adjust, Get_q_primes(crt), Get_p_primes(crt),
               num_part);
  free(p1_adjust);
  RTLIB_TM_END(RTM_DOT_PROD, rtm);
  return res;
}

POLY Mod_down_fuse1(POLY res, POLY poly) {
  RTLIB_TM_START(RTM_MOD_DOWN_FUSE1, rtm);
  CRT_CONTEXT* crt = Get_crt_context();
  Reduce_and_rescale(res, poly, crt, true);
  RTLIB_TM_END(RTM_MOD_DOWN_FUSE1, rtm);
  return res;
}

POLY Poly_mulp_fast(POLY res, POLY poly1, POLY poly2, POLY poly2_prec) {
  RTLIB_TM_START(RTM_MULP_FAST, rtm);
  CRT_CONTEXT* crt = Get_crt_context();
  Init_poly(res, poly2);
  if (poly2_prec->_data == NULL) {
    Multiply_ntt(res, poly1, poly2, Get_q_primes(crt), Get_p_primes(crt));
  } else {
    Multiply_plain_fast(res, poly1, poly2, poly2_prec, Get_q_primes(crt),
                        Get_p_primes(crt));
  }
  RTLIB_TM_END(RTM_MULP_FAST, rtm);
  return res;
}

void Chk_level(POLY res, POLY p0, POLY p1) {
#if 0
  FMT_ASSERT(res && p0, "null res or opnd");
  FMT_ASSERT(Poly_level(res) == Poly_level(p0), "level unmatch");
  if (p1) {
    FMT_ASSERT(Poly_level(res) == Poly_level(p0), "level unmatch");
  }
#else
  if (Poly_level(res) != Poly_level(p0) || Num_p(res) != Num_p(p0)) {
    printf("level not match\n");
  }
  if (p1 && (Poly_level(res) != Poly_level(p1) || Num_p(res) != Num_p(p1))) {
    printf("level not match\n");
  }

#endif
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
