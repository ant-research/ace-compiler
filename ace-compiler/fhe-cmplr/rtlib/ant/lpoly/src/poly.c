//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "lpoly/poly.h"

#include "hal/hal.h"

LPOLY Add_lpoly(LPOLY sum, L_POLY poly1, L_POLY poly2, MODULUS* mod) {
  IS_TRUE(Is_lpoly_ntt_match(&poly1, &poly2), "ntt not match");
  Hw_modadd(Get_lpoly_coeffs(sum), Get_lpoly_coeffs(&poly1),
            Get_lpoly_coeffs(&poly2), mod, Get_lpoly_degree(sum));
  Set_lpoly_ntt(sum, Is_lpoly_ntt(&poly1));
  return sum;
}

LPOLY Add_scalar(LPOLY res, L_POLY poly, int64_t scalar, MODULUS* mod) {
  Hw_modadd_scalar(Get_lpoly_coeffs(res), Get_lpoly_coeffs(&poly), scalar, mod,
                   Get_lpoly_degree(res));
  Set_lpoly_ntt(res, Is_lpoly_ntt(&poly));
  return res;
}

LPOLY Sub_lpoly(LPOLY res, L_POLY poly1, L_POLY poly2, MODULUS* mod) {
  IS_TRUE(Is_lpoly_ntt_match(&poly1, &poly2), "ntt not match");
  Hw_modsub(Get_lpoly_coeffs(res), Get_lpoly_coeffs(&poly1),
            Get_lpoly_coeffs(&poly2), mod, Get_lpoly_degree(res));
  Set_lpoly_ntt(res, Is_lpoly_ntt(&poly1));
  return res;
}

LPOLY Sub_scalar(LPOLY res, L_POLY poly, int64_t scalar, MODULUS* mod) {
  Hw_modsub_scalar(Get_lpoly_coeffs(res), Get_lpoly_coeffs(&poly), scalar, mod,
                   Get_lpoly_degree(res));
  Set_lpoly_ntt(res, Is_lpoly_ntt(&poly));
  return res;
}

LPOLY Mul_lpoly(LPOLY res, L_POLY poly1, L_POLY poly2, MODULUS* mod) {
  IS_TRUE(Is_lpoly_ntt_match(&poly1, &poly2), "ntt not match");
  Hw_modmul(Get_lpoly_coeffs(res), Get_lpoly_coeffs(&poly1),
            Get_lpoly_coeffs(&poly2), mod, Get_lpoly_degree(res));
  Set_lpoly_ntt(res, Is_lpoly_ntt(&poly1));
  return res;
}

LPOLY Mul_scalar(LPOLY res, L_POLY poly, int64_t scalar, MODULUS* mod) {
  Hw_modmul_scalar(Get_lpoly_coeffs(res), Get_lpoly_coeffs(&poly), scalar, mod,
                   Get_lpoly_degree(res));
  Set_lpoly_ntt(res, Is_lpoly_ntt(&poly));
  return res;
}

LPOLY Mac_lpoly(LPOLY res, L_POLY poly1, L_POLY poly2, MODULUS* mod) {
  IS_TRUE(Is_lpoly_ntt_match(&poly1, &poly2), "ntt not match");
  Hw_modmuladd(Get_lpoly_coeffs(res), Get_lpoly_coeffs(&poly1),
               Get_lpoly_coeffs(&poly2), mod, Get_lpoly_degree(res));
  Set_lpoly_ntt(res, Is_lpoly_ntt(&poly1));
  return res;
}

LPOLY Mac_scalar(LPOLY res, L_POLY poly, int64_t scalar, MODULUS* mod) {
  Hw_modmuladd_scalar(Get_lpoly_coeffs(res), Get_lpoly_coeffs(&poly), scalar,
                      mod, Get_lpoly_degree(res));
  Set_lpoly_ntt(res, Is_lpoly_ntt(&poly));
  return res;
}

void Ntt(LPOLY res, L_POLY poly, size_t idx) {
  FMT_ASSERT(!Is_lpoly_ntt(&poly), "already ntt form");
  Hw_ntt(Get_lpoly_coeffs(res), Get_lpoly_coeffs(&poly), idx,
         Get_lpoly_degree(res));
  // size_t num_q = Get_q_cnt();
  // FMT_ASSERT(idx < num_q + Get_p_cnt(), "Ntt: idx overflow");
  // CRT_CONTEXT* crt         = Get_crt_context();
  // NTT_CONTEXT* ntt         = idx < num_q
  //                                ? Get_ntt(Get_prime_at(Get_q(crt), idx))
  //                                : Get_ntt(Get_prime_at(Get_p(crt), idx -
  //                                num_q));
  // int64_t*     poly_data   = Get_lpoly_coeffs(&poly);
  // int64_t*     res_data    = Get_lpoly_coeffs(res);
  // uint32_t     ring_degree = Get_lpoly_degree(&poly);
  // VALUE_LIST   tmp_ntt;
  // VALUE_LIST   res_ntt;
  // Init_i64_value_list_no_copy(&tmp_ntt, ring_degree, poly_data);
  // Init_i64_value_list_no_copy(&res_ntt, ring_degree, res_data);
  // Ftt_fwd(&res_ntt, ntt, &tmp_ntt);
  Set_lpoly_ntt(res, true);
}

void Intt(LPOLY res, L_POLY poly, size_t idx) {
  FMT_ASSERT(Is_lpoly_ntt(&poly), "already coeffcient form");
  Hw_intt(Get_lpoly_coeffs(res), Get_lpoly_coeffs(&poly), idx,
          Get_lpoly_degree(res));
  // size_t num_q = Get_q_cnt();
  // FMT_ASSERT(idx < num_q + Get_p_cnt(), "Intt: idx overflow");
  // CRT_CONTEXT* crt         = Get_crt_context();
  // NTT_CONTEXT* ntt         = idx < num_q
  //                                ? Get_ntt(Get_prime_at(Get_q(crt), idx))
  //                                : Get_ntt(Get_prime_at(Get_p(crt), idx -
  //                                num_q));
  // int64_t*     poly_data   = Get_lpoly_coeffs(&poly);
  // int64_t*     res_data    = Get_lpoly_coeffs(res);
  // uint32_t     ring_degree = Get_lpoly_degree(&poly);
  // VALUE_LIST   tmp_ntt;
  // VALUE_LIST   res_ntt;
  // Init_i64_value_list_no_copy(&tmp_ntt, ring_degree, poly_data);
  // Init_i64_value_list_no_copy(&res_ntt, ring_degree, res_data);
  // Ftt_inv(&res_ntt, ntt, &tmp_ntt);
  Set_lpoly_ntt(res, false);
}
