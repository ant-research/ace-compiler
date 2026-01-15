//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "poly/rns_poly.h"

#include "common/error.h"
#include "common/rt_config.h"
#include "common/rtlib_timing.h"
#include "poly/rns_poly.h"
#include "rns_poly_impl.h"
#include "util/random_sample.h"

void Init_poly(POLY res, POLY poly) {
  if (poly == res) return;
  uint32_t ring_degree  = Get_rdgree(poly);
  size_t   num_primes   = Poly_level(poly);
  size_t   num_primes_p = Num_p(poly);
  if (res->_data == NULL) {
    Alloc_poly_data(res, ring_degree, num_primes, num_primes_p);
  } else {
    // res already allocated memory, reuse the memory
    IS_TRUE(Get_rdgree(res) == ring_degree, "unmatched ring degree");
    if (Num_alloc(res) * Get_rdgree(res) < Get_poly_len(poly)) {
      Free_poly_data(res);
      Alloc_poly_data(res, ring_degree, num_primes, num_primes_p);
    } else {
      memset(res->_data, 0, Get_poly_mem_size(res));
      Init_poly_data(res, ring_degree, num_primes, num_primes_p, res->_data);
    }
  }
  Set_is_ntt(res, Is_ntt(poly));
}

POLY_ARR Alloc_polys(uint32_t cnt, uint32_t degree, size_t num_q,
                     size_t num_p) {
  FMT_ASSERT(cnt > 0, "Alloc_polys: zero array size specified");

  POLY_ARR res = (POLY_ARR)malloc(sizeof(POLYNOMIAL) * cnt);
  memset(res, 0, sizeof(POLYNOMIAL) * cnt);
  size_t per_data_size = sizeof(int64_t) * (num_q + num_p) * degree;
  char*  data          = (char*)malloc(per_data_size * cnt);
  memset(data, 0, per_data_size * cnt);

  for (uint32_t idx = 0; idx < cnt; idx++) {
    POLY elem = &res[idx];
    Init_poly_data(elem, degree, num_q, num_p, (int64_t*)data);
    data += per_data_size;
  }
  return res;
}

void Copy_poly(POLY res, POLY poly) {
  RTLIB_TM_START(RTM_COPY_POLY, rtm);
  FMT_ASSERT(Is_size_match(res, poly), "unmatched primes");
  FMT_ASSERT(Num_p(res) >= Num_p(poly), "unmatched p primes cnt");

  // copy q coeffs
  memcpy(Get_poly_coeffs(res), Get_poly_coeffs(poly),
         sizeof(int64_t) * Poly_level(res) * Get_rdgree(res));
  // copy p coeffs only if src has p coeffs
  if (Num_p(poly)) {
    memcpy(Get_p_coeffs(res), Get_p_coeffs(poly),
           sizeof(int64_t) * Num_p(res) * Get_rdgree(res));
  }
  Set_is_ntt(res, Is_ntt(poly));
  RTLIB_TM_END(RTM_COPY_POLY, rtm);
}

POLY Decomp(POLY res, POLY poly, uint32_t q_part_idx) {
  RTLIB_TM_START(RTM_DECOMP, rtm);
  CRT_CONTEXT* crt             = Get_crt_context();
  size_t       num_qpartl      = Num_decomp(poly);
  size_t       ql              = Poly_level(poly);
  uint32_t     degree          = Get_rdgree(poly);
  CRT_PRIMES*  q_parts         = Get_qpart(crt);
  size_t       part_size       = Get_per_part_size(q_parts);
  VL_CRTPRIME* q_parts_primes  = VL_VALUE_AT(Get_primes(q_parts), q_part_idx);
  size_t       decomp_poly_len = LIST_LEN(q_parts_primes);
  if (q_part_idx == num_qpartl - 1) {
    decomp_poly_len = ql - part_size * q_part_idx;
  }
  if (res->_num_alloc_primes < decomp_poly_len) {
    Free_poly_data(res);
    Alloc_poly_data(res, degree, decomp_poly_len, 0);
  } else {
    res->_num_primes   = decomp_poly_len;
    res->_num_primes_p = 0;
  }
  size_t   start     = part_size * q_part_idx;
  int64_t* coeffs    = Get_poly_coeffs(res);
  int64_t* c1_coeffs = Get_poly_coeffs(poly);
  for (size_t i = 0, idx = start; i < decomp_poly_len; i++, idx++) {
    for (uint32_t d = 0; d < degree; d++) {
      coeffs[i * degree + d] = c1_coeffs[idx * degree + d];
    }
  }
  Set_is_ntt(res, Is_ntt(poly));
  RTLIB_TM_END(RTM_DECOMP, rtm);
  return res;
}

POLY Mod_up(POLY new_poly, POLY old_poly, uint32_t q_part_idx) {
  RTLIB_TM_START(RTM_MOD_UP, rtm);
  CRT_CONTEXT* crt   = Get_crt_context();
  size_t       q_cnt = Poly_level(new_poly);
  size_t       level = q_cnt - 1;
  VL_CRTPRIME* new_primes =
      Get_qpart_compl_at(Get_qpart_compl(crt), level, q_part_idx);
  VL_CRTPRIME* old_primes = VL_VALUE_AT(Get_primes(Get_qpart(crt)), q_part_idx);
  CRT_PRIMES*  q_parts    = Get_qpart(crt);
  uint32_t     ring_degree = Get_rdgree(new_poly);
  size_t       ext_len     = LIST_LEN(new_primes);
  size_t       old_q_cnt   = Poly_level(old_poly);
  size_t       old_p_cnt   = Num_p(old_poly);
  IS_TRUE(new_poly && ring_degree == Get_rdgree(old_poly) &&
              Num_p(new_poly) == Get_primes_cnt(Get_p(crt)),
          "raise_rns_base: result size not match");

  POLYNOMIAL qparts_ext2p;
  if (Is_ntt(old_poly)) {
    POLYNOMIAL cloned_old_poly;
    Alloc_poly_data(&cloned_old_poly, ring_degree, old_q_cnt, old_p_cnt);
    Conv_ntt2poly_with_primes(&cloned_old_poly, old_poly, old_primes);
    Alloc_poly_data(&qparts_ext2p, ring_degree, ext_len, 0);

    Fast_base_conv_with_parts(&qparts_ext2p, &cloned_old_poly, q_parts,
                              new_primes, q_part_idx, level);
    Conv_poly2ntt_inplace_with_primes(&qparts_ext2p, new_primes);
    Free_poly_data(&cloned_old_poly);
  } else {
    Alloc_poly_data(&qparts_ext2p, ring_degree, ext_len, 0);
    Fast_base_conv_with_parts(&qparts_ext2p, old_poly, q_parts, new_primes,
                              q_part_idx, level);
  }
  int64_t* new_coeffs = Get_poly_coeffs(new_poly);
  int64_t* ext_coeffs = Get_poly_coeffs(&qparts_ext2p);
  int64_t* old_coeffs = Get_poly_coeffs(old_poly);
  size_t   start_idx  = Get_per_part_size(q_parts) * q_part_idx;
  size_t   end_idx    = start_idx + old_q_cnt;
  size_t   copy_size  = sizeof(int64_t) * ring_degree;

  memcpy(new_coeffs, ext_coeffs, copy_size * start_idx);
  memcpy(new_coeffs + start_idx * ring_degree, old_coeffs,
         copy_size * (end_idx - start_idx));
  memcpy(new_coeffs + end_idx * ring_degree,
         ext_coeffs + (end_idx - old_q_cnt) * ring_degree,
         (Get_num_pq(new_poly) - end_idx) * copy_size);
  Free_poly_data(&qparts_ext2p);

  Set_is_ntt(new_poly, Is_ntt(old_poly));
  RTLIB_TM_END(RTM_MOD_UP, rtm);
  return new_poly;
}

POLY Decomp_modup(POLY res, POLY poly, uint32_t q_part_idx) {
  RTLIB_TM_START(RTM_DECOMP_MODUP, rtm);
  CRT_CONTEXT* crt        = Get_crt_context();
  size_t       num_decomp = Num_decomp(poly);
  uint32_t     degree     = Get_rdgree(poly);
  CRT_PRIMES*  q_parts    = Get_qpart(crt);
  size_t       part_size  = Get_per_part_size(q_parts);
  size_t       num_q      = Poly_level(poly);
  VL_CRTPRIME* compl_primes =
      Get_qpart_compl_at(Get_qpart_compl(crt), num_q - 1, q_part_idx);
  VL_CRTPRIME* part2_primes = Get_vl_value_at(Get_primes(q_parts), q_part_idx);

  // number of primes of part1,2,3
  size_t num_part1       = Get_per_part_size(q_parts) * q_part_idx;
  size_t num_part2       = (q_part_idx == num_decomp - 1)
                               ? num_q - part_size * q_part_idx
                               : LIST_LEN(part2_primes);
  size_t num_part3       = Get_num_pq(res) - num_part1 - num_part2;
  size_t num_compl       = LIST_LEN(compl_primes);
  size_t part2_start_idx = num_part1;
  size_t part2_end_idx   = part2_start_idx + num_part2;
  size_t part3_start_idx = num_part1 + num_part2;

  // decompose poly at q_part_idx to part2
  POLYNOMIAL part2_poly;
  Extract_poly(&part2_poly, res, part2_start_idx, num_part2);
  int64_t* part2_coeffs = Get_poly_coeffs(res);
  int64_t* poly_coeffs  = Get_poly_coeffs(poly);
  for (size_t i = part2_start_idx; i < part2_end_idx; i++) {
    for (size_t d = 0, idx = i * degree; d < degree; d++, idx++) {
      part2_coeffs[idx] = poly_coeffs[idx];
    }
  }
  Set_is_ntt(&part2_poly, Is_ntt(poly));

  // raise part2 to part1 and part3
  POLYNOMIAL  part2_poly_intt;
  POLYNOMIAL* part2_poly_intt_ptr;
  if (Is_ntt(poly)) {
    Alloc_poly_data(&part2_poly_intt, degree, num_part2, 0);
    Conv_ntt2poly_with_primes(&part2_poly_intt, &part2_poly, part2_primes);
    part2_poly_intt_ptr = &part2_poly_intt;
  } else {
    part2_poly_intt_ptr = &part2_poly;
  }

  VALUE_LIST* ql_hatinvmodq =
      VL_L2_VALUE_AT(Get_qlhatinvmodq(q_parts), q_part_idx, num_part2 - 1);
  VL_VL_I64* ql_hatmodp =
      VL_L2_VALUE_AT(Get_qlhatmodp(q_parts), num_q - 1, q_part_idx);

  VL_CRTPRIME part1_primes, part3_primes;
  Extract_value_list(&part1_primes, compl_primes, 0, num_part1);
  Extract_value_list(&part3_primes, compl_primes, num_part1, num_part3);

  INT128_T sum[num_compl];
  memset(sum, 0, num_compl * sizeof(INT128_T));

  for (size_t n = 0; n < degree; n++) {
    for (size_t i = 0; i < num_part2; i++) {
      int64_t  val = Get_coeff_at(part2_poly_intt_ptr, i * degree + n);
      MODULUS* qi  = (MODULUS*)Get_modulus(Get_vlprime_at(part2_primes, i));
      int64_t  mul_inv_modq =
          Mul_int64_mod_barret(val, Get_i64_value_at(ql_hatinvmodq, i), qi);
      VALUE_LIST* qhat_modp = VL_VALUE_AT(ql_hatmodp, i);
      for (size_t j = 0; j < num_compl; j++) {
        sum[j] += (INT128_T)mul_inv_modq * Get_i64_value_at(qhat_modp, j);
      }
    }
    for (size_t j = 0; j < num_part1; j++) {
      int64_t new_value = Mod_barrett_128(
          sum[j], Get_modulus(Get_vlprime_at(&part1_primes, j)));
      Set_coeff_at(res, new_value, j * degree + n);
      sum[j] = 0;
    }

    for (size_t j1 = 0, j2 = num_part1; j1 < num_part3; j1++, j2++) {
      int64_t new_value = Mod_barrett_128(
          sum[j2], Get_modulus(Get_vlprime_at(&part3_primes, j1)));
      Set_coeff_at(res, new_value, (j1 + part3_start_idx) * degree + n);
      sum[j2] = 0;
    }
  }
  if (Is_ntt(poly)) {
    POLYNOMIAL part1_poly, part3_poly;
    Extract_poly(&part1_poly, res, 0, num_part1);
    Extract_poly(&part3_poly, res, part3_start_idx, num_part3);
    Set_is_ntt(&part1_poly, false);
    Set_is_ntt(&part3_poly, false);
    Conv_poly2ntt_inplace_with_primes(&part1_poly, &part1_primes);
    Conv_poly2ntt_inplace_with_primes(&part3_poly, &part3_primes);
    Set_is_ntt(res, true);
    Free_poly_data(&part2_poly_intt);
  } else {
    Set_is_ntt(res, false);
  }
  RTLIB_TM_END(RTM_DECOMP_MODUP, rtm);
  return res;
}

POLY Mod_down(POLY res, POLY poly) {
  RTLIB_TM_START(RTM_MOD_DOWN, rtm);
  CRT_CONTEXT* crt = Get_crt_context();
  if (res->_data == NULL) {
    Alloc_poly_data(res, Get_rdgree(poly), Poly_level(poly), 0);
  }
  CRT_PRIMES* q_primes    = Get_q(crt);
  CRT_PRIMES* p_primes    = Get_p(crt);
  size_t      p_prime_len = Get_primes_cnt(p_primes);
  uint32_t    ring_degree = Get_rdgree(res);
  IS_TRUE(poly && Get_rdgree(poly) == Get_rdgree(res) &&
              Poly_level(poly) == Poly_level(res) && Num_p(poly) == p_prime_len,
          "Mod_down: result size not match");
  // NOTE: after rescale num_primes of poly may not equal to num_primes of crt
  IS_TRUE(Poly_level(poly) <= Get_primes_cnt(q_primes),
          "Mod_down: unmatched num primes");

  // extract p part from old poly and copy it out to make sure old poly is not
  // changed after Mod_down
  POLYNOMIAL poly_p_part, p_poly;
  Extract_poly(&poly_p_part, poly, Poly_level(res), p_prime_len);
  Alloc_poly_data(&p_poly, Get_rdgree(&poly_p_part), Poly_level(&poly_p_part),
                  0);
  Copy_poly(&p_poly, &poly_p_part);

  if (Is_ntt(poly)) {
    Conv_ntt2poly_inplace_with_primes(&p_poly, Get_p_primes(crt));
  }
  Fast_base_conv(res, &p_poly, q_primes, p_primes);
  if (Is_ntt(poly)) {
    Conv_poly2ntt_inplace(res);
  }
  int64_t* p_m_inv_modq = Get_i64_values(Get_pinvmodq(p_primes));
  int64_t* new_data     = Get_poly_coeffs(res);
  int64_t* old_q_data   = Get_poly_coeffs(poly);
  for (size_t q_idx = 0; q_idx < Poly_level(poly); q_idx++) {
    MODULUS* q_modulus = Get_modulus(Get_prime_at(q_primes, q_idx));
    int64_t  qi        = Get_mod_val(q_modulus);
    for (uint32_t d_idx = 0; d_idx < ring_degree; d_idx++) {
      int64_t new_value = Sub_int64_with_mod(*old_q_data++, *new_data, qi);
      *new_data++ = Mul_int64_mod_barret(new_value, *p_m_inv_modq, q_modulus);
    }
    p_m_inv_modq++;
  }
  Set_is_ntt(res, Is_ntt(poly));
  Free_poly_data(&p_poly);
  RTLIB_TM_END(RTM_MOD_DOWN, rtm);
  return res;
}

POLY Rescale(POLY res, POLY poly) {
  RTLIB_TM_START(RTM_RESCALE_POLY, rtm);
  FMT_ASSERT(Poly_level(poly) >= 2, "Level of rescale opnd is too small");
  CRT_CONTEXT* crt           = Get_crt_context();
  VALUE_LIST*  coeff_modulus = Get_q_primes(crt);
  size_t       level         = Poly_level(poly);
  size_t       res_level     = Poly_level(res);
  IS_TRUE(res_level == level && level <= LIST_LEN(coeff_modulus),
          "Rescale: primes not match");
  IS_TRUE(Get_rdgree(res) == Get_rdgree(poly), "Rescale: degree not match");

  size_t       degree      = Get_rdgree(poly);
  int64_t*     result      = Get_poly_coeffs(res);
  int64_t*     poly_coeffs = Get_poly_coeffs(poly);
  int64_t*     last_coeffs = poly_coeffs + (level - 1) * degree;
  CRT_PRIMES*  primes      = Get_q(crt);
  VL_CRTPRIME* q_primes    = Get_primes(primes);
  CRT_PRIME*   last_prime  = Get_vlprime_at(q_primes, level - 1);
  int64_t      last_mod    = Get_modulus_val(last_prime);
  // get precomputed value
  VL_I64* ql_inv_mod_qi      = Get_ql_inv_mod_qi_at(primes, level - 2);
  VL_I64* ql_inv_mod_qi_prec = Get_ql_inv_mod_qi_prec_at(primes, level - 2);
  VL_I64* ql_ql_inv_mod_ql_div_ql_mod_qi =
      Get_ql_ql_inv_mod_ql_div_ql_mod_qi_at(primes, level - 2);
  VL_I64* ql_ql_inv_mod_ql_div_ql_mod_qi_prec =
      Get_ql_ql_inv_mod_ql_div_ql_mod_qi_prec_at(primes, level - 2);
  if (Is_ntt(poly)) {
    // Convert last input to non-NTT form
    VALUE_LIST* last_input_ntt_form = Alloc_value_list(I64_TYPE, degree);
    Init_i64_value_list(last_input_ntt_form, Get_rdgree(poly), last_coeffs);
    NTT_CONTEXT* last_ntt_table = Get_ntt(last_prime);
    Ftt_inv(last_input_ntt_form, last_ntt_table, last_input_ntt_form);
    last_coeffs = Get_i64_values(last_input_ntt_form);

    VALUE_LIST* last_input = Alloc_value_list(I64_TYPE, degree);
    for (size_t module_idx = 0; module_idx < Poly_level(poly) - 1;
         module_idx++) {
      CRT_PRIME*   prime   = Get_vlprime_at(q_primes, module_idx);
      NTT_CONTEXT* ntt     = Get_ntt(prime);
      MODULUS*     modulus = Get_modulus(prime);
      int64_t      mod     = Get_mod_val(modulus);
      int64_t      qlql_mod_inv =
          Get_i64_value_at(ql_ql_inv_mod_ql_div_ql_mod_qi, module_idx);
      int64_t qlql_mod_inv_prec =
          Get_i64_value_at(ql_ql_inv_mod_ql_div_ql_mod_qi_prec, module_idx);
      int64_t mod_inverse = Get_i64_value_at(ql_inv_mod_qi, module_idx);
      int64_t mod_inverse_prec =
          Get_i64_value_at(ql_inv_mod_qi_prec, module_idx);
      // Switch mod for last input from ql to qi
      for (uint32_t poly_idx = 0; poly_idx < Get_rdgree(res); poly_idx++) {
        I64_VALUE_AT(last_input, poly_idx) = Fast_mul_const_with_mod(
            Switch_modulus(last_coeffs[poly_idx], last_mod, mod), qlql_mod_inv,
            qlql_mod_inv_prec, mod);
      }
      // Transform last input back to ntt
      Ftt_fwd(last_input, ntt, last_input);
      for (size_t poly_idx = 0; poly_idx < Get_rdgree(res); poly_idx++) {
        *result = Fast_mul_const_with_mod(*poly_coeffs, mod_inverse,
                                          mod_inverse_prec, mod);
        *result = Add_int64_with_mod(
            *result, Get_i64_value_at(last_input, poly_idx), mod);
        poly_coeffs++;
        result++;
      }
    }
    Free_value_list(last_input_ntt_form);
    Free_value_list(last_input);
    Set_is_ntt(res, TRUE);
  } else {
    CRT_PRIME* prime_head = Get_vlprime_at(coeff_modulus, 0);
    CRT_PRIME* prime      = prime_head;
    for (size_t module_idx = 0; module_idx < Poly_level(poly) - 1;
         module_idx++) {
      MODULUS* modulus     = Get_modulus(prime);
      int64_t  mod         = Get_mod_val(modulus);
      int64_t  mod_inverse = Get_i64_value_at(ql_inv_mod_qi, module_idx);
      int64_t  mod_inverse_prec =
          Get_i64_value_at(ql_inv_mod_qi_prec, module_idx);
      for (size_t poly_idx = 0; poly_idx < Get_rdgree(res); poly_idx++) {
        int64_t last_coeff = Mod_barrett_64(last_coeffs[poly_idx], modulus);
        int64_t new_val    = Sub_int64_with_mod(*poly_coeffs, last_coeff, mod);
        *result            = Fast_mul_const_with_mod(new_val, mod_inverse,
                                                     mod_inverse_prec, mod);
        poly_coeffs++;
        result++;
      }
      prime = Get_next_prime(prime);
    }
    Set_is_ntt(res, FALSE);
  }

  Modswitch(res, res);
  RTLIB_TM_END(RTM_RESCALE_POLY, rtm);
  return res;
}

POLY Modswitch(POLY res, POLY poly) {
  uint32_t level = Poly_level(poly);
  FMT_ASSERT(level >= 2, "polynomial level is too small to support modswitch");
  Set_poly_level(res, level - 1);
  if (res != poly) {
    Copy_low_level_polynomial(res, poly);
  }
  return res;
}

static POLY Switch_key_precompute_no_fusion(POLY res, size_t size, POLY poly) {
  CRT_CONTEXT* crt         = Get_crt_context();
  CRT_PRIMES*  q_parts     = Get_qpart(crt);
  CRT_PRIMES*  p           = Get_p(crt);
  size_t       ql          = poly->_num_primes;
  size_t       num_p       = Get_primes_cnt(p);
  size_t       part_size   = Get_per_part_size(q_parts);
  size_t       num_qpart   = Get_num_parts(q_parts);
  size_t       num_qpartl  = ceil((double)ql / part_size);
  size_t       ring_degree = poly->_ring_degree;
  if (num_qpartl > num_qpart) {
    num_qpartl = num_qpart;
  }
  IS_TRUE(size == num_qpartl, "length of precompute not match");
  POLY res_start = res;

  POLYNOMIAL decomp_poly;
  Alloc_poly_data(&decomp_poly, ring_degree, ql + num_p, 0);
  for (size_t part = 0; part < size; part++) {
    VL_CRTPRIME* q_parts_primes = Get_vl_value_at(Get_primes(q_parts), part);
    size_t decomp_poly_len      = part == num_qpartl - 1 ? ql - part_size * part
                                                         : LIST_LEN(q_parts_primes);
    Set_poly_level(&decomp_poly, decomp_poly_len);
    Decomp(&decomp_poly, poly, part);
    Mod_up(res, &decomp_poly, part);
    res++;
  }
  Free_poly_data(&decomp_poly);
  return res_start;
}

//! @brief Fusion version of precompute for switch key
//! combined decompose and raise, faster than Switch_key_precompute_no_fusion
//! about 10%, note that new memory is returned
static POLY Switch_key_precompute_fusion(POLY res, size_t size, POLY poly) {
  IS_TRUE(size == Num_decomp(poly), "length of precompute not match");
  POLY res_start = res;
  for (size_t part = 0; part < size; part++) {
    Decomp_modup(res, poly, part);
    res++;
  }
  return res_start;
}

POLY_ARR Precomp(POLY_ARR res, size_t size, POLY input) {
  RTLIB_TM_START(RTM_PRECOMP, rtm);
  if (Get_rtlib_config(CONF_OP_FUSION_DECOMP_MODUP)) {
    res = Switch_key_precompute_fusion(res, size, input);
  } else {
    res = Switch_key_precompute_no_fusion(res, size, input);
  }
  RTLIB_TM_END(RTM_PRECOMP, rtm);
  return res;
}

VALUE_LIST* Alloc_precomp(POLY input) {
  size_t      num_decomp = Num_decomp(input);
  VALUE_LIST* poly_list  = Alloc_value_list(PTR_TYPE, num_decomp);
  POLY_ARR arr = Alloc_polys(num_decomp, Get_rdgree(input), Poly_level(input),
                             Get_p_cnt());
  FOR_ALL_ELEM(poly_list, idx) {
    Set_ptr_value(poly_list, idx, (PTR)(&arr[idx]));
  }
  Precomp(arr, LIST_LEN(poly_list), input);
  return poly_list;
}

POLY Extend(POLY res, POLY poly) {
  RTLIB_TM_START(RTM_EXTEND, rtm);
  FMT_ASSERT(Is_size_match(res, poly), "Extend: size not match");
  CRT_CONTEXT* crt = Get_crt_context();
  if (Get_poly_coeffs(poly) != NULL) {
    memset(Get_poly_coeffs(res), 0, Get_poly_mem_size(res));
    FMT_ASSERT(Num_alloc(res) == Poly_level(poly) + Get_crt_num_p(crt),
               "Extend: alloc_size not match");
    // multiply poly with by a scalar vector ignore p part
    Set_num_p(res, 0);
    Scalars_integer_multiply_poly(res, poly, Get_pmodq(Get_p(crt)));
    Set_num_p(res, Get_crt_num_p(crt));
  } else {
    FMT_ASSERT(false, "Extend: data of input poly not allocated");
  }
  RTLIB_TM_END(RTM_EXTEND, rtm);
  return res;
}

//! @brief Fast Dot prod for key switch with lazy mod
POLY Fast_dot_prod(POLY res, POLY p0, POLY p1, uint32_t num_part) {
  RTLIB_TM_START(RTM_FAST_DOT_PROD, rtm);
  Init_poly(res, &(p0[0]));
  POLYNOMIAL p1_adjust[num_part];
  for (uint32_t idx = 0; idx < num_part; idx++) {
    Derive_poly(&p1_adjust[idx], &p1[idx], Poly_level(res), Num_p(res));
  }
  Fast_dotprod_poly(res, p0, p1_adjust, num_part);
  RTLIB_TM_END(RTM_FAST_DOT_PROD, rtm);
  return res;
}

//! @brief Dot prod for key switch
POLY Dot_prod(POLY res, POLY_ARR p0, POLY_ARR p1, uint32_t num_part) {
  if (Get_rtlib_config(CONF_FAST_DOT_PROD)) {
    return Fast_dot_prod(res, p0, p1, num_part);
  }
  RTLIB_TM_START(RTM_DOT_PROD, rtm);
  Init_poly(res, &p0[0]);
  POLYNOMIAL p1_adjust[num_part];
  for (uint32_t idx = 0; idx < num_part; idx++) {
    Derive_poly(&p1_adjust[idx], &p1[idx], Poly_level(res), Num_p(res));
  }
  Dotprod_poly(res, p0, p1_adjust, num_part);
  RTLIB_TM_END(RTM_DOT_PROD, rtm);
  return res;
}

POLY Mod_down_rescale(POLY res, POLY poly) {
  RTLIB_TM_START(RTM_MOD_DOWN_RESCALE, rtm);
  CRT_CONTEXT* crt = Get_crt_context();

  CRT_PRIMES* q_primes    = Get_q(crt);
  CRT_PRIMES* p_primes    = Get_p(crt);
  size_t      num_p       = Num_p(poly);
  size_t      num_q       = Poly_level(poly);
  uint32_t    ring_degree = Get_rdgree(res);

  // reuse res for reduce and rescale, make sure res num_q size as large as poly
  IS_TRUE(poly && res && ring_degree == Get_rdgree(poly) &&
              num_q == Poly_level(res) && num_q > 1 &&
              Get_primes_cnt(p_primes) == num_p && Num_p(res) == 0,
          "Reduce_and_rescale_rns: size not match");

  if (Is_ntt(poly)) {
    Conv_ntt2poly_inplace(poly);
  }

  POLYNOMIAL poly_p_part;
  Extract_poly(&poly_p_part, poly, Poly_level(poly), num_p);
  Fast_base_conv(res, &poly_p_part, q_primes, p_primes);

  size_t   last_idx          = num_q - 1;
  int64_t* p_m_inv_modq      = Get_i64_values(Get_pinvmodq(p_primes));
  int64_t* p_m_inv_modq_prec = Get_i64_values(Get_pinvmodq_prec(p_primes));
  int64_t  last_p_m_inv      = p_m_inv_modq[last_idx];
  int64_t  last_p_m_inv_prec = p_m_inv_modq_prec[last_idx];
  int64_t* last_old          = Get_poly_coeffs_at(poly, last_idx);
  int64_t* last_new          = Get_poly_coeffs_at(res, last_idx);
  MODULUS* last_modulus      = Get_modulus(Get_prime_at(q_primes, last_idx));
  int64_t  last_qi           = Get_mod_val(last_modulus);
  for (uint32_t d_idx = 0; d_idx < ring_degree; d_idx++) {
    last_new[d_idx] =
        Sub_mul_const_mod(last_old[d_idx], last_new[d_idx], last_p_m_inv,
                          last_p_m_inv_prec, last_qi);
  }

  int64_t* new_data = Get_poly_coeffs(res);
  int64_t* old_data = Get_poly_coeffs(poly);

  VL_I64* ql_inv_mod_qi      = Get_ql_inv_mod_qi_at(q_primes, num_q - 2);
  VL_I64* ql_inv_mod_qi_prec = Get_ql_inv_mod_qi_prec_at(q_primes, num_q - 2);
  for (size_t q_idx = 0; q_idx < last_idx; q_idx++) {
    MODULUS* q_modulus          = Get_modulus(Get_prime_at(q_primes, q_idx));
    int64_t  qi                 = Get_mod_val(q_modulus);
    int64_t  ql_inv             = Get_i64_value_at(ql_inv_mod_qi, q_idx);
    int64_t  ql_inv_prec        = Get_i64_value_at(ql_inv_mod_qi_prec, q_idx);
    int64_t  p_m_inv_modqi      = p_m_inv_modq[q_idx];
    int64_t  p_m_inv_modqi_prec = p_m_inv_modq_prec[q_idx];
    for (uint32_t d_idx = 0; d_idx < ring_degree; d_idx++) {
      int64_t new_val = Sub_mul_const_mod(*old_data, *new_data, p_m_inv_modqi,
                                          p_m_inv_modqi_prec, qi);
      *new_data =
          Sub_mul_const_mod(new_val, last_new[d_idx], ql_inv, ql_inv_prec, qi);
      old_data++;
      new_data++;
    }
  }
  Modswitch(res, res);

  Conv_poly2ntt_inplace(res);

  RTLIB_TM_END(RTM_MOD_DOWN_RESCALE, rtm);
  return res;
}

void Conv_poly2ntt_inplace(POLY poly) {
  FMT_ASSERT(!Is_ntt(poly), "already ntt form");
  CRT_CONTEXT* crt         = Get_crt_context();
  CRT_PRIME*   q_prime     = Get_prime_head(Get_q(crt));
  CRT_PRIME*   p_prime     = Get_prime_head(Get_p(crt));
  int64_t*     poly_data   = Get_poly_coeffs(poly);
  uint32_t     ring_degree = Get_rdgree(poly);
  VALUE_LIST   tmp_ntt;
  Init_i64_value_list_no_copy(&tmp_ntt, ring_degree, poly_data);
  for (size_t qi = 0; qi < Poly_level(poly); qi++) {
    NTT_CONTEXT* ntt = Get_ntt(q_prime);
    Ftt_fwd(&tmp_ntt, ntt, &tmp_ntt);
    tmp_ntt._vals._i += ring_degree;
    poly_data += ring_degree;
    q_prime = Get_next_prime(q_prime);
  }
  if (Num_p(poly)) {
    tmp_ntt._vals._i = Get_p_coeffs(poly);
    for (size_t pi = 0; pi < Num_p(poly); pi++) {
      NTT_CONTEXT* ntt = Get_ntt(p_prime);
      Ftt_fwd(&tmp_ntt, ntt, &tmp_ntt);
      tmp_ntt._vals._i += ring_degree;
      poly_data += ring_degree;
      p_prime = Get_next_prime(p_prime);
    }
  }
  Set_is_ntt(poly, TRUE);
}

void Conv_poly2ntt(POLY res, POLY poly) {
  FMT_ASSERT(!Is_ntt(poly), "already ntt form");
  FMT_ASSERT(Is_size_match(res, poly), "size not match");
  CRT_CONTEXT* crt         = Get_crt_context();
  CRT_PRIME*   q_prime     = Get_prime_head(Get_q(crt));
  CRT_PRIME*   p_prime     = Get_prime_head(Get_p(crt));
  int64_t*     poly_data   = Get_poly_coeffs(poly);
  int64_t*     res_data    = Get_poly_coeffs(res);
  uint32_t     ring_degree = Get_rdgree(poly);
  VALUE_LIST   tmp_ntt;
  Init_i64_value_list_no_copy(&tmp_ntt, ring_degree, poly_data);
  VALUE_LIST res_ntt;
  Init_i64_value_list_no_copy(&res_ntt, ring_degree, res_data);
  for (size_t qi = 0; qi < Poly_level(poly); qi++) {
    NTT_CONTEXT* ntt = Get_ntt(q_prime);
    Ftt_fwd(&res_ntt, ntt, &tmp_ntt);
    tmp_ntt._vals._i += ring_degree;
    res_ntt._vals._i += ring_degree;
    q_prime = Get_next_prime(q_prime);
  }

  if (Num_p(poly)) {
    tmp_ntt._vals._i = Get_p_coeffs(poly);
    res_ntt._vals._i = Get_p_coeffs(res);
    for (size_t pi = 0; pi < Num_p(poly); pi++) {
      NTT_CONTEXT* ntt = Get_ntt(p_prime);
      Ftt_fwd(&res_ntt, ntt, &tmp_ntt);
      tmp_ntt._vals._i += ring_degree;
      res_ntt._vals._i += ring_degree;
      p_prime = Get_next_prime(p_prime);
    }
  }
  Set_is_ntt(res, TRUE);
}

void Conv_ntt2poly_inplace(POLY poly) {
  FMT_ASSERT(Is_ntt(poly), "already coeffcient form");
  CRT_CONTEXT* crt         = Get_crt_context();
  CRT_PRIME*   q_prime     = Get_prime_head(Get_q(crt));
  CRT_PRIME*   p_prime     = Get_prime_head(Get_p(crt));
  int64_t*     poly_data   = Get_poly_coeffs(poly);
  uint32_t     ring_degree = Get_rdgree(poly);
  VALUE_LIST   tmp_ntt;
  Init_i64_value_list_no_copy(&tmp_ntt, ring_degree, poly_data);
  for (size_t qi = 0; qi < Poly_level(poly); qi++) {
    NTT_CONTEXT* ntt = Get_ntt(q_prime);
    Ftt_inv(&tmp_ntt, ntt, &tmp_ntt);
    tmp_ntt._vals._i += ring_degree;
    poly_data += ring_degree;
    q_prime = Get_next_prime(q_prime);
  }
  if (Num_p(poly)) {
    tmp_ntt._vals._i = Get_p_coeffs(poly);
    for (size_t pi = 0; pi < Num_p(poly); pi++) {
      NTT_CONTEXT* ntt = Get_ntt(p_prime);
      Ftt_inv(&tmp_ntt, ntt, &tmp_ntt);
      tmp_ntt._vals._i += ring_degree;
      poly_data += ring_degree;
      p_prime = Get_next_prime(p_prime);
    }
  }
  Set_is_ntt(poly, FALSE);
}

void Derive_poly(POLY res, POLY poly, size_t q_cnt, size_t p_cnt) {
  FMT_ASSERT(poly && res && q_cnt <= Poly_level(poly) && p_cnt <= Num_p(poly),
             "invalid range");
  res->_num_alloc_primes = Num_alloc(poly);
  Init_poly_data(res, Get_rdgree(poly), q_cnt, p_cnt, Get_poly_coeffs(poly));
  Set_is_ntt(res, Is_ntt(poly));
}

POLY Scalars_integer_add_poly(POLY res, POLY poly, VALUE_LIST* scalars) {
  IS_TRUE(Is_size_match(res, poly), "size not match");
  size_t level = Poly_level(res);
  IS_TRUE(level <= Get_q_cnt(), "level not match");

  int64_t* res_coeffs  = Get_poly_coeffs(res);
  int64_t* poly_coeffs = Get_poly_coeffs(poly);
  MODULUS* modulus     = Q_modulus();
  for (size_t module_idx = 0; module_idx < level; module_idx++) {
    int64_t scalar = Get_i64_value_at(scalars, module_idx);
    for (uint32_t poly_idx = 0; poly_idx < Get_rdgree(res); poly_idx++) {
      *res_coeffs =
          Add_int64_with_mod(*poly_coeffs, scalar, Get_mod_val(modulus));
      res_coeffs++;
      poly_coeffs++;
    }
    modulus++;
  }
  size_t p_cnt = Num_p(res);
  if (p_cnt) {
    IS_TRUE(Is_p_cnt_match(res, poly), "unmatched p primes cnt");
    IS_TRUE(LIST_LEN(scalars) == level + p_cnt, "length of scalars not match");
    modulus     = P_modulus();
    res_coeffs  = Get_p_coeffs(res);
    poly_coeffs = Get_p_coeffs(poly);
    for (size_t module_idx = 0; module_idx < p_cnt; module_idx++) {
      int64_t scalar = Get_i64_value_at(scalars, module_idx + level);
      for (uint32_t poly_idx = 0; poly_idx < Get_rdgree(res); poly_idx++) {
        *res_coeffs =
            Add_int64_with_mod(*poly_coeffs, scalar, Get_mod_val(modulus));
        res_coeffs++;
        poly_coeffs++;
      }
      modulus++;
    }
  }
  Set_is_ntt(res, Is_ntt(poly));
  return res;
}

POLY Scalar_integer_multiply_poly(POLY res, POLY poly, int64_t scalar) {
  IS_TRUE(Is_size_match(res, poly), "size not match");
  IS_TRUE(Poly_level(res) <= Get_q_cnt(), "primes not match");

  int64_t* res_coeffs  = Get_poly_coeffs(res);
  int64_t* poly_coeffs = Get_poly_coeffs(poly);
  MODULUS* modulus     = Q_modulus();
  for (size_t module_idx = 0; module_idx < Poly_level(res); module_idx++) {
    for (uint32_t poly_idx = 0; poly_idx < Get_rdgree(res); poly_idx++) {
      *res_coeffs =
          Mul_int64_with_mod(*poly_coeffs, scalar, Get_mod_val(modulus));
      res_coeffs++;
      poly_coeffs++;
    }
    modulus++;
  }
  size_t p_cnt = Num_p(res);
  if (p_cnt) {
    IS_TRUE(Is_p_cnt_match(res, poly), "unmatched p primes cnt");
    modulus     = P_modulus();
    res_coeffs  = Get_p_coeffs(res);
    poly_coeffs = Get_p_coeffs(poly);
    for (size_t module_idx = 0; module_idx < p_cnt; module_idx++) {
      for (uint32_t poly_idx = 0; poly_idx < Get_rdgree(res); poly_idx++) {
        *res_coeffs =
            Mul_int64_with_mod(*poly_coeffs, scalar, Get_mod_val(modulus));
        res_coeffs++;
        poly_coeffs++;
      }
      modulus++;
    }
  }
  Set_is_ntt(res, Is_ntt(poly));
  return res;
}

POLY Scalars_integer_multiply_poly(POLY res, POLY poly, VALUE_LIST* scalars) {
  IS_TRUE(Is_size_match(res, poly), "size not match");
  size_t level = Poly_level(res);
  IS_TRUE(level <= Get_q_cnt(), "primes not match");

  int64_t* res_coeffs  = Get_poly_coeffs(res);
  int64_t* poly_coeffs = Get_poly_coeffs(poly);
  MODULUS* modulus     = Q_modulus();
  for (size_t module_idx = 0; module_idx < level; module_idx++) {
    int64_t scalar = Get_i64_value_at(scalars, module_idx);
    for (uint32_t poly_idx = 0; poly_idx < Get_rdgree(res); poly_idx++) {
      *res_coeffs = Mul_int64_mod_barret(*poly_coeffs, scalar, modulus);
      res_coeffs++;
      poly_coeffs++;
    }
    modulus++;
  }
  size_t p_cnt = Num_p(res);
  if (p_cnt) {
    IS_TRUE(Is_p_cnt_match(res, poly), "unmatched p primes cnt");
    IS_TRUE(LIST_LEN(scalars) == level + p_cnt, "length of scalars not match");
    modulus     = P_modulus();
    res_coeffs  = Get_p_coeffs(res);
    poly_coeffs = Get_p_coeffs(poly);
    for (size_t module_idx = 0; module_idx < p_cnt; module_idx++) {
      int64_t scalar = Get_i64_value_at(scalars, module_idx + level);
      for (uint32_t poly_idx = 0; poly_idx < Get_rdgree(res); poly_idx++) {
        *res_coeffs = Mul_int64_mod_barret(*poly_coeffs, scalar, modulus);
        res_coeffs++;
        poly_coeffs++;
      }
      modulus++;
    }
  }
  Set_is_ntt(res, Is_ntt(poly));
  return res;
}

POLY Scalars_integer_multiply_poly_qpart(POLY res, POLY poly,
                                         VALUE_LIST* scalars, size_t part_idx) {
  IS_TRUE(Is_size_match(res, poly), "level not match");
  IS_TRUE(Poly_level(res) <= Get_q_cnt(), "primes not match");
  CRT_CONTEXT* crt          = Get_crt_context();
  size_t       num_per_part = Get_per_part_size(Get_qpart(crt));

  size_t   start_part_idx = num_per_part * part_idx;
  size_t   end_part_idx   = (Poly_level(res) > (start_part_idx + num_per_part))
                                ? (start_part_idx + num_per_part)
                                : Poly_level(res);
  int64_t* res_coeffs     = Get_poly_coeffs(res);
  int64_t* poly_coeffs    = Get_poly_coeffs(poly);
  MODULUS* modulus        = Q_modulus();
  for (size_t i = 0; i < Poly_level(res); i++) {
    if (i >= start_part_idx && i < end_part_idx) {
      int64_t scalar = Get_i64_value_at(scalars, i);
      for (uint32_t j = 0; j < Get_rdgree(res); j++) {
        size_t idx = i * Get_rdgree(res) + j;
        res_coeffs[idx] =
            Mul_int64_mod_barret(poly_coeffs[idx], scalar, modulus);
      }
    }
    modulus++;
  }
  Set_is_ntt(res, Is_ntt(poly));
  return res;
}

POLY Add_poly(POLY sum, POLY poly1, POLY poly2) {
  if (!Is_ntt_match(poly1, poly2)) {
    if (!Is_ntt(poly1)) {
      Conv_poly2ntt_inplace(poly1);
    } else if (!Is_ntt(poly2)) {
      Conv_poly2ntt_inplace(poly2);
    }
  }
  IS_TRUE(Is_size_match(poly1, poly2) && Is_size_match(sum, poly1),
          "size not match");
  IS_TRUE(Poly_level(sum) <= Get_q_cnt(), "primes not match");
  int64_t* sum_coeffs   = Get_poly_coeffs(sum);
  int64_t* poly1_coeffs = Get_poly_coeffs(poly1);
  int64_t* poly2_coeffs = Get_poly_coeffs(poly2);
  MODULUS* modulus      = Q_modulus();
  for (size_t module_idx = 0; module_idx < Poly_level(sum); module_idx++) {
    for (uint32_t poly_idx = 0; poly_idx < Get_rdgree(sum); poly_idx++) {
      *sum_coeffs = Add_int64_with_mod(*poly1_coeffs, *poly2_coeffs,
                                       Get_mod_val(modulus));
      sum_coeffs++;
      poly1_coeffs++;
      poly2_coeffs++;
    }
    modulus++;
  }
  size_t p_cnt = Num_p(sum);
  if (p_cnt) {
    IS_TRUE(Is_p_cnt_match(sum, poly1) && Is_p_cnt_match(sum, poly2),
            "unmatched p primes cnt");
    sum_coeffs   = Get_p_coeffs(sum);
    poly1_coeffs = Get_p_coeffs(poly1);
    poly2_coeffs = Get_p_coeffs(poly2);
    modulus      = P_modulus();
    for (size_t module_idx = 0; module_idx < p_cnt; module_idx++) {
      for (uint32_t poly_idx = 0; poly_idx < Get_rdgree(sum); poly_idx++) {
        *sum_coeffs = Add_int64_with_mod(*poly1_coeffs, *poly2_coeffs,
                                         Get_mod_val(modulus));
        sum_coeffs++;
        poly1_coeffs++;
        poly2_coeffs++;
      }
      modulus++;
    }
  }
  Set_is_ntt(sum, Is_ntt(poly1));
  return sum;
}

POLY Sub_poly(POLY poly_diff, POLY poly1, POLY poly2) {
  if (!Is_ntt_match(poly1, poly2)) {
    if (!Is_ntt(poly1)) {
      Conv_poly2ntt_inplace(poly1);
    } else if (!Is_ntt(poly2)) {
      Conv_poly2ntt_inplace(poly2);
    }
  }
  IS_TRUE(Is_size_match(poly1, poly2) && Is_size_match(poly_diff, poly1),
          "size not match");
  IS_TRUE(Poly_level(poly_diff) <= Get_q_cnt(), "primes not match");
  int64_t* poly_diff_coeffs = Get_poly_coeffs(poly_diff);
  int64_t* poly1_coeffs     = Get_poly_coeffs(poly1);
  int64_t* poly2_coeffs     = Get_poly_coeffs(poly2);
  MODULUS* modulus          = Q_modulus();
  for (size_t mod_idx = 0; mod_idx < Poly_level(poly_diff); mod_idx++) {
    for (uint32_t poly_idx = 0; poly_idx < Get_rdgree(poly_diff); poly_idx++) {
      *poly_diff_coeffs = Sub_int64_with_mod(*poly1_coeffs, *poly2_coeffs,
                                             Get_mod_val(modulus));
      poly_diff_coeffs++;
      poly1_coeffs++;
      poly2_coeffs++;
    }
    modulus++;
  }
  size_t p_cnt = Num_p(poly_diff);
  if (p_cnt) {
    IS_TRUE(
        Is_p_cnt_match(poly_diff, poly1) && Is_p_cnt_match(poly_diff, poly2),
        "unmatched p primes cnt");
    poly_diff_coeffs = Get_p_coeffs(poly_diff);
    poly1_coeffs     = Get_p_coeffs(poly1);
    poly2_coeffs     = Get_p_coeffs(poly2);
    modulus          = P_modulus();
    for (size_t mod_idx = 0; mod_idx < p_cnt; mod_idx++) {
      for (uint32_t poly_idx = 0; poly_idx < Get_rdgree(poly_diff);
           poly_idx++) {
        *poly_diff_coeffs = Sub_int64_with_mod(*poly1_coeffs, *poly2_coeffs,
                                               Get_mod_val(modulus));
        poly_diff_coeffs++;
        poly1_coeffs++;
        poly2_coeffs++;
      }
      modulus++;
    }
  }
  Set_is_ntt(poly_diff, Is_ntt(poly1));
  return poly_diff;
}

POLY Mul_poly(POLY res, POLY poly1, POLY poly2) {
  RTLIB_TM_START(RTM_MULP, rtm);
  if (!Is_ntt(poly1)) {
    Conv_poly2ntt_inplace(poly1);
  }
  if (!Is_ntt(poly2)) {
    Conv_poly2ntt_inplace(poly2);
  }
  FMT_ASSERT(Is_size_match(res, poly1) && Is_size_match(res, poly2),
             "size not match");
  int64_t* res_data   = Get_poly_coeffs(res);
  int64_t* poly1_data = Get_poly_coeffs(poly1);
  int64_t* poly2_data = Get_poly_coeffs(poly2);
  MODULUS* q_modulus  = Q_modulus();
  for (size_t module_idx = 0; module_idx < Poly_level(res); module_idx++) {
    for (uint32_t idx = 0; idx < Get_rdgree(res); idx++) {
      *res_data = Mul_int64_mod_barret(*poly1_data, *poly2_data, q_modulus);
      poly1_data++;
      poly2_data++;
      res_data++;
    }
    q_modulus++;
  }
  size_t p_cnt = Num_p(res);
  if (p_cnt) {
    IS_TRUE(Is_p_cnt_match(res, poly1) && Is_p_cnt_match(res, poly2),
            "unmatched p primes cnt");
    MODULUS* p_modulus = P_modulus();
    poly1_data         = Get_p_coeffs(poly1);
    poly2_data         = Get_p_coeffs(poly2);
    res_data           = Get_p_coeffs(res);
    for (size_t module_idx = 0; module_idx < p_cnt; module_idx++) {
      for (uint32_t idx = 0; idx < Get_rdgree(res); idx++) {
        *res_data = Mul_int64_mod_barret(*poly1_data, *poly2_data, p_modulus);
        poly1_data++;
        poly2_data++;
        res_data++;
      }
      p_modulus++;
    }
  }
  Set_is_ntt(res, TRUE);
  RTLIB_TM_END(RTM_MULP, rtm);
  return res;
}

POLY Mul_poly_fast(POLY res, POLY poly1, POLY poly2, POLY poly2_prec) {
  RTLIB_TM_START(RTM_MULP_FAST, rtm);
  FMT_ASSERT(poly2_prec->_data, "invalid precomputed of poly2");
  if (!Is_ntt(poly1)) {
    Conv_poly2ntt_inplace(poly1);
  }
  if (!Is_ntt(poly2)) {
    Conv_poly2ntt_inplace(poly2);
  }
  if (!Is_ntt(poly2_prec)) {
    Conv_poly2ntt_inplace(poly2_prec);
  }
  IS_TRUE(Is_size_match(res, poly1) && Is_size_match(res, poly2) &&
              Is_size_match(res, poly2_prec),
          "level not matched");
  int64_t* res_data        = Get_poly_coeffs(res);
  int64_t* poly1_data      = Get_poly_coeffs(poly1);
  int64_t* poly2_data      = Get_poly_coeffs(poly2);
  int64_t* poly2_prec_data = Get_poly_coeffs(poly2_prec);
  MODULUS* q_modulus       = Q_modulus();
  for (size_t module_idx = 0; module_idx < Poly_level(res); module_idx++) {
    int64_t mod_val = Get_mod_val(q_modulus);
    for (uint32_t idx = 0; idx < Get_rdgree(res); idx++) {
      *res_data = Fast_mul_const_with_mod(*poly1_data, *poly2_data,
                                          *poly2_prec_data, mod_val);
      poly1_data++;
      poly2_data++;
      poly2_prec_data++;
      res_data++;
    }
    q_modulus++;
  }
  size_t p_cnt = Num_p(res);
  if (p_cnt) {
    IS_TRUE(Is_p_cnt_match(res, poly1) && Is_p_cnt_match(res, poly2) &&
                Is_p_cnt_match(res, poly2_prec),
            "unmatched p primes cnt");
    MODULUS* p_modulus = P_modulus();
    res_data           = Get_p_coeffs(res);
    poly1_data         = Get_p_coeffs(poly1);
    poly2_data         = Get_p_coeffs(poly2);
    poly2_prec_data    = Get_p_coeffs(poly2_prec);
    for (size_t module_idx = 0; module_idx < p_cnt; module_idx++) {
      int64_t mod_val = Get_mod_val(p_modulus);
      for (uint32_t idx = 0; idx < Get_rdgree(res); idx++) {
        *res_data = Fast_mul_const_with_mod(*poly1_data, *poly2_data,
                                            *poly2_prec_data, mod_val);
        poly1_data++;
        poly2_data++;
        poly2_prec_data++;
        res_data++;
      }
      p_modulus++;
    }
  }
  Set_is_ntt(res, TRUE);
  RTLIB_TM_END(RTM_MULP_FAST, rtm);
  return res;
}

POLY Automorphism_transform(POLY res, POLY poly, VALUE_LIST* precomp) {
  uint32_t degree = Get_rdgree(poly);
  IS_TRUE(Is_size_match(res, poly), "op not matched");
  IS_TRUE(LIST_LEN(precomp) == degree, "invalid index table length");

  int64_t* poly_coeffs  = Get_poly_coeffs(poly);
  int64_t* res_coeffs   = Get_poly_coeffs(res);
  int64_t* precomp_data = Get_i64_values(precomp);
  MODULUS* modulus      = Q_modulus();
  for (size_t module_idx = 0; module_idx < Poly_level(res); module_idx++) {
    int64_t qi = Get_mod_val(modulus);
    for (uint32_t poly_idx = 0; poly_idx < degree; poly_idx++) {
      int64_t map_idx = precomp_data[poly_idx];
      if (map_idx >= 0) {
        *res_coeffs = poly_coeffs[module_idx * degree + map_idx];
      } else {
        *res_coeffs = qi - poly_coeffs[module_idx * degree - map_idx];
      }
      res_coeffs++;
    }
    modulus++;
  }
  size_t p_cnt = Num_p(res);
  if (p_cnt) {
    modulus     = P_modulus();
    res_coeffs  = Get_p_coeffs(res);
    poly_coeffs = Get_p_coeffs(poly);
    for (size_t module_idx = 0; module_idx < p_cnt; module_idx++) {
      int64_t pi = Get_mod_val(modulus);
      for (uint32_t poly_idx = 0; poly_idx < degree; poly_idx++) {
        int64_t map_idx = precomp_data[poly_idx];
        if (map_idx >= 0) {
          *res_coeffs = poly_coeffs[module_idx * degree + map_idx];
        } else {
          *res_coeffs = pi - poly_coeffs[module_idx * degree - map_idx];
        }
        res_coeffs++;
      }
      modulus++;
    }
  }
  Set_is_ntt(res, Is_ntt(poly));
  return res;
}

void Transform_values_from_level0(POLY res, POLY poly) {
  uint32_t degree = Get_rdgree(res);
  IS_TRUE(degree == Get_rdgree(poly), "unmatched ring degree");
  IS_TRUE(Get_poly_alloc_len(res) >= Get_poly_len(poly) &&
              Num_alloc(res) <= Get_q_cnt(),
          "not enough memory");
  // just copy data for first level
  int64_t* res_data = Get_poly_coeffs(res);
  memcpy(res_data, Get_poly_coeffs(poly), sizeof(int64_t) * degree);
  res_data += degree;
  // fill up the rest data with proper moduli
  MODULUS* q_modulus = Q_modulus();
  int64_t  old_mod   = Get_mod_val(q_modulus);
  for (size_t i = 1; i < Poly_level(res); i++) {
    q_modulus++;
    int64_t new_mod = Get_mod_val(q_modulus);
    for (size_t val_idx = 0; val_idx < degree; val_idx++) {
      *res_data = Switch_modulus(Get_coeff_at(poly, val_idx), old_mod, new_mod);
      res_data++;
    }
  }
}

void Transform_value_to_rns_poly(POLY poly, VALUE_LIST* value,
                                 bool without_mod) {
  CRT_CONTEXT* crt   = Get_crt_context();
  size_t       q_cnt = Poly_level(poly);
  IS_TRUE(LIST_LEN(value) == Get_rdgree(poly) && q_cnt <= Get_crt_num_q(crt),
          "length not match");
  Transform_to_dcrt(Get_poly_coeffs(poly), Get_poly_len(poly), value, q_cnt,
                    Num_p(poly) ? true : false, without_mod, crt);
}

void Reconstruct_rns_poly_to_value(VALUE_LIST* res, POLY poly) {
  IS_TRUE(!Is_ntt(poly), "rns_poly should be intt-form");
  size_t       q_cnt = Poly_level(poly);
  CRT_CONTEXT* crt   = Get_crt_context();
  IS_TRUE(LIST_LEN(res) == Get_rdgree(poly) && q_cnt <= Get_crt_num_q(crt),
          "length not match");
  Reconstruct_from_dcrt(res, Get_poly_coeffs(poly), Get_poly_len(poly), q_cnt,
                        Num_p(poly) ? true : false, crt);
}

void Sample_uniform_poly(POLY poly) {
  MODULUS*   q_modulus = Q_modulus();
  VALUE_LIST samples;
  int64_t*   coeffs   = Get_poly_coeffs(poly);
  uint32_t   r_degree = Get_rdgree(poly);
  for (size_t module_idx = 0; module_idx < Poly_level(poly); module_idx++) {
    Init_i64_value_list_no_copy(&samples, r_degree,
                                coeffs + (module_idx * r_degree));
    Sample_uniform(&samples, Get_mod_val(q_modulus));
    q_modulus++;
  }
  if (Num_p(poly)) {
    coeffs             = Get_p_coeffs(poly);
    MODULUS* p_modulus = P_modulus();
    for (size_t module_idx = 0; module_idx < Num_p(poly); module_idx++) {
      Init_i64_value_list_no_copy(&samples, r_degree,
                                  coeffs + (module_idx * r_degree));
      Sample_uniform(&samples, Get_mod_val(p_modulus));
      p_modulus++;
    }
  }
}

void Sample_ternary_poly(POLY poly, size_t hamming_weight) {
  MODULUS* q_modulus = Q_modulus();
  uint32_t r_degree  = Get_rdgree(poly);

  // ternary uniform for secret key only need to sample one group of r_degree
  // data, then the sampled value transformed to different q base
  VALUE_LIST* samples = Alloc_value_list(I64_TYPE, r_degree);
  Sample_ternary(samples, hamming_weight);

  int64_t* coeffs      = Get_poly_coeffs(poly);
  int64_t* sample_data = Get_i64_values(samples);
  for (size_t module_idx = 0; module_idx < Poly_level(poly); module_idx++) {
    for (size_t idx = 0; idx < r_degree; idx++) {
      coeffs[idx] = sample_data[idx];
      if (coeffs[idx] < 0) {
        coeffs[idx] += Get_mod_val(q_modulus);
      }
    }
    coeffs += r_degree;
    q_modulus++;
  }

  // for p part, switch to p modulus based on q0
  if (Num_p(poly)) {
    coeffs              = Get_p_coeffs(poly);
    int64_t* q0         = Get_poly_coeffs(poly);
    int64_t  q0_mod_val = Get_mod_val(Q_modulus());
    MODULUS* p_modulus  = P_modulus();
    for (size_t module_idx = 0; module_idx < Num_p(poly); module_idx++) {
      for (size_t idx = 0; idx < r_degree; idx++) {
        coeffs[idx] =
            Switch_modulus(q0[idx], q0_mod_val, Get_mod_val(p_modulus));
      }
      coeffs += r_degree;
      p_modulus++;
    }
  }
  Free_value_list(samples);
}

void Print_poly_lite(FILE* fp, POLYNOMIAL* input) {
  const uint32_t def_coeff_len = 8;
  const uint32_t def_level_len = 3;

  POLYNOMIAL p;
  p._data          = NULL;
  POLYNOMIAL* poly = &p;
  Init_poly(poly, input);
  if (input->_is_ntt) {
    Conv_ntt2poly(poly, input);
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

void Print_poly_rawdata(FILE* fp, POLY poly) {
  IS_TRUE(poly, "null polynomial");
  if (Get_poly_len(poly) == 0) {
    fprintf(fp, "{}\n");
    return;
  }
  fprintf(fp, "is_ntt: %s, raw data: \n", poly->_is_ntt ? "Y" : "N");
  fprintf(fp, "\ndata: [");
  for (size_t i = 0; i < poly->_num_primes * poly->_ring_degree; i++) {
    fprintf(fp, " %ld", poly->_data[i]);
  }
  fprintf(fp, "]\n");
  for (size_t i = 0; i < poly->_num_primes; i++) {
    fprintf(fp, "\nQ%ld: [", i);
    int64_t* coeffs = poly->_data + i * poly->_ring_degree;
    for (int64_t j = 0; j < poly->_ring_degree; j++) {
      fprintf(fp, "%ld ", coeffs[j]);
    }
    fprintf(fp, " ]");
  }
  for (size_t i = 0; i < poly->_num_primes_p; i++) {
    fprintf(fp, "\nP%ld: [", i);
    int64_t* coeffs =
        poly->_data + (poly->_num_primes + i) * poly->_ring_degree;
    for (int64_t j = 0; j < poly->_ring_degree; j++) {
      fprintf(fp, "%ld ", coeffs[j]);
    }
    fprintf(fp, " ]");
  }
  fprintf(fp, "\n");
}

void Print_poly(FILE* fp, POLY poly) {
  // Print_poly_detail(fp, poly, NULL, FALSE);
  Print_poly_rawdata(fp, poly);
}

L_POLY Lpoly_from_poly(POLY poly, size_t idx) {
  FMT_ASSERT(idx < Get_num_pq(poly), "idx overflow");
  // Init lpoly from rns poly
  L_POLY lpoly;
  size_t degree = Get_rdgree(poly);
  Init_lpoly_data(&lpoly, degree, Coeffs(poly, idx, degree));
  Set_lpoly_ntt(&lpoly, Is_ntt(poly));
  return lpoly;
}

L_POLY Lpoly_from_poly_arr(size_t size, POLY_ARR poly_arr, size_t idx) {
  size_t num_pq = Get_q_cnt() + Get_p_cnt();
  FMT_ASSERT(idx < size * num_pq, "idx overflow");
  size_t poly_idx = idx / num_pq;
  POLY   poly     = &poly_arr[poly_idx];
  L_POLY lpoly;
  size_t degree = Get_rdgree(poly);
  // Init lpoly from rns poly
  Init_lpoly_data(&lpoly, degree,
                  Coeffs(poly, idx - poly_idx * num_pq, degree));
  Set_lpoly_ntt(&lpoly, Is_ntt(poly));
  return lpoly;
}

void Set_poly_data(POLY poly, L_POLY lpoly, size_t idx) {
  size_t num_pq = Get_num_pq(poly);
  FMT_ASSERT(idx < num_pq, "Set_poly_data_from_lpoly: idx overflow");
  Set_coeffs(poly, idx, Get_rdgree(poly), Get_lpoly_coeffs(&lpoly));
}

void Free_poly_data(POLY poly) {
  if (poly->_data) {
    free(poly->_data);
    poly->_data = NULL;
  }
  poly->_num_alloc_primes = 0;
}