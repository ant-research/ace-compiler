//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "rns_poly_impl.h"

#include "util/bignumber.h"

void Multiply_add(POLYNOMIAL* res, POLYNOMIAL* poly1, POLYNOMIAL* poly2) {
  IS_TRUE(Is_ntt(res) && Is_ntt(poly1) && Is_ntt(poly2), "opnd/res is not ntt");
  IS_TRUE(Is_size_match(res, poly1) && Is_size_match(res, poly2),
          "level not matched");
  int64_t* res_data   = Get_poly_coeffs(res);
  int64_t* poly1_data = Get_poly_coeffs(poly1);
  int64_t* poly2_data = Get_poly_coeffs(poly2);
  MODULUS* q_modulus  = Q_modulus();
  for (size_t module_idx = 0; module_idx < Poly_level(res); module_idx++) {
    for (uint32_t idx = 0; idx < Get_rdgree(res); idx++) {
      int64_t tmp = Mul_int64_mod_barret(*poly1_data, *poly2_data, q_modulus);
      *res_data   = Add_int64_with_mod(*res_data, tmp, Get_mod_val(q_modulus));
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
    res_data           = Get_p_coeffs(res);
    poly1_data         = Get_p_coeffs(poly1);
    poly2_data         = Get_p_coeffs(poly2);
    for (size_t module_idx = 0; module_idx < p_cnt; module_idx++) {
      for (uint32_t idx = 0; idx < Get_rdgree(res); idx++) {
        int64_t tmp = Mul_int64_mod_barret(*poly1_data, *poly2_data, p_modulus);
        *res_data = Add_int64_with_mod(*res_data, tmp, Get_mod_val(p_modulus));
        poly1_data++;
        poly2_data++;
        res_data++;
      }
      p_modulus++;
    }
  }
}

void Rotate_poly_with_rotation_idx(POLYNOMIAL* res, POLYNOMIAL* poly,
                                   int32_t rotation) {
  size_t  degree = Get_rdgree(poly);
  MODULUS two_n_modulus;
  Init_modulus(&two_n_modulus, 2 * degree);
  uint32_t    k       = Find_automorphism_index(rotation, &two_n_modulus);
  VALUE_LIST* precomp = Alloc_value_list(I64_TYPE, degree);
  Precompute_automorphism_order(precomp, k, degree, Is_ntt(poly));
  Automorphism_transform(res, poly, precomp);
  Free_value_list(precomp);
}

POLYNOMIAL* Barrett_poly(POLYNOMIAL* res, POLYNOMIAL* poly) {
  // Barrett_poly is used for fast modular multiplication, the poly should be
  // NTT form
  IS_TRUE(Is_ntt(poly), "Barrett_poly: poly must be ntt form");
  IS_TRUE(Is_size_match(res, poly), "size not match");
  IS_TRUE(Poly_level(res) <= Get_q_cnt(), "primes not match");
  int64_t* res_coeffs  = Get_poly_coeffs(res);
  int64_t* poly_coeffs = Get_poly_coeffs(poly);
  MODULUS* modulus     = Q_modulus();
  for (size_t module_idx = 0; module_idx < Poly_level(res); module_idx++) {
    int64_t mod_val = Get_mod_val(modulus);
    for (uint32_t poly_idx = 0; poly_idx < Get_rdgree(res); poly_idx++) {
      *res_coeffs = Precompute_const(*poly_coeffs, mod_val);
      res_coeffs++;
      poly_coeffs++;
    }
    modulus++;
  }
  size_t p_cnt = Num_p(res);
  if (p_cnt) {
    IS_TRUE(Is_p_cnt_match(res, poly), "unmatched p primes cnt");
    res_coeffs  = Get_p_coeffs(res);
    poly_coeffs = Get_p_coeffs(poly);
    modulus     = P_modulus();
    for (size_t module_idx = 0; module_idx < p_cnt; module_idx++) {
      int64_t mod_val = Get_mod_val(modulus);
      for (uint32_t poly_idx = 0; poly_idx < Get_rdgree(res); poly_idx++) {
        *res_coeffs = Precompute_const(*poly_coeffs, mod_val);
        res_coeffs++;
        poly_coeffs++;
      }
      modulus++;
    }
  }
  Set_is_ntt(res, Is_ntt(poly));
  return res;
}

POLYNOMIAL* Fast_dotprod_poly(POLYNOMIAL* res, POLY_ARR p0, POLY_ARR p1,
                              uint32_t num) {
  IS_TRUE(Is_ntt(res) && Is_ntt(&p0[0]) && Is_ntt(&p1[0]),
          "opnd/res is not ntt");
  IS_TRUE(Is_size_match(res, &p0[0]) && Is_size_match(res, &p1[0]),
          "level not matched");
  MODULUS* q_modulus = Q_modulus();
  int64_t* res_data  = Get_poly_coeffs(res);
  int64_t* p0_data[num];
  int64_t* p1_data[num];
  for (uint32_t idx = 0; idx < num; idx++) {
    p0_data[idx] = Get_poly_coeffs(&p0[idx]);
    p1_data[idx] = Get_poly_coeffs(&p1[idx]);
  }
  for (size_t module_idx = 0; module_idx < Poly_level(res); module_idx++) {
    for (uint32_t idx = 0; idx < Get_rdgree(res); idx++) {
      UINT128_T tmp = 0;
      for (uint32_t arr_idx = 0; arr_idx < num; arr_idx++) {
        tmp += (UINT128_T)(*p0_data[arr_idx]) * (UINT128_T)(*p1_data[arr_idx]);
        p0_data[arr_idx]++;
        p1_data[arr_idx]++;
      }
      *res_data = Mod_barrett_128(tmp, q_modulus);
      res_data++;
    }
    q_modulus++;
  }
  size_t p_cnt = Num_p(res);
  if (p_cnt) {
    MODULUS* p_modulus = P_modulus();
    res_data           = Get_p_coeffs(res);

    for (uint32_t idx = 0; idx < num; idx++) {
      p0_data[idx] = Get_p_coeffs(&p0[idx]);
      p1_data[idx] = Get_p_coeffs(&p1[idx]);
    }
    for (size_t module_idx = 0; module_idx < p_cnt; module_idx++) {
      for (uint32_t idx = 0; idx < Get_rdgree(res); idx++) {
        UINT128_T tmp = 0;
        for (uint32_t arr_idx = 0; arr_idx < num; arr_idx++) {
          tmp +=
              (UINT128_T)(*p0_data[arr_idx]) * (UINT128_T)(*p1_data[arr_idx]);
          p0_data[arr_idx]++;
          p1_data[arr_idx]++;
        }
        *res_data = Mod_barrett_128(tmp, p_modulus);
        res_data++;
      }
      p_modulus++;
    }
  }
  return res;
}

POLYNOMIAL* Dotprod_poly(POLYNOMIAL* res, POLY_ARR p0, POLY_ARR p1,
                         uint32_t num) {
  for (uint32_t idx = 0; idx < num; idx++) {
    Multiply_add(res, &p0[idx], &p1[idx]);
  }
  return res;
}

void Conv_poly2ntt_inplace_with_primes(POLYNOMIAL* poly, VL_CRTPRIME* primes) {
  FMT_ASSERT(!Is_ntt(poly), "already ntt form");
  IS_TRUE(LIST_LEN(primes) == Poly_level(poly), "primes size not match");
  int64_t*   poly_data   = Get_poly_coeffs(poly);
  uint32_t   ring_degree = Get_rdgree(poly);
  VALUE_LIST tmp_ntt;
  Init_i64_value_list_no_copy(&tmp_ntt, ring_degree, poly_data);

  for (size_t qi = 0; qi < Poly_level(poly); qi++) {
    CRT_PRIME*   prime = Get_vlprime_at(primes, qi);
    NTT_CONTEXT* ntt   = Get_ntt(prime);
    Ftt_fwd(&tmp_ntt, ntt, &tmp_ntt);
    tmp_ntt._vals._i += ring_degree;
  }
  Set_is_ntt(poly, TRUE);
}

void Conv_poly2ntt_with_primes(POLYNOMIAL* res, POLYNOMIAL* poly,
                               VL_CRTPRIME* primes) {
  FMT_ASSERT(!Is_ntt(poly), "already ntt form");
  IS_TRUE(LIST_LEN(primes) == Poly_level(poly), "primes size not match");
  int64_t*   poly_data   = Get_poly_coeffs(poly);
  int64_t*   res_data    = Get_poly_coeffs(res);
  uint32_t   ring_degree = Get_rdgree(poly);
  VALUE_LIST tmp_ntt, res_ntt;
  Init_i64_value_list_no_copy(&tmp_ntt, ring_degree, poly_data);
  Init_i64_value_list_no_copy(&res_ntt, ring_degree, res_data);

  for (size_t qi = 0; qi < Poly_level(poly); qi++) {
    CRT_PRIME*   prime = Get_vlprime_at(primes, qi);
    NTT_CONTEXT* ntt   = Get_ntt(prime);
    Ftt_fwd(&res_ntt, ntt, &tmp_ntt);
    tmp_ntt._vals._i += ring_degree;
    res_ntt._vals._i += ring_degree;
  }
  Set_is_ntt(res, TRUE);
}

void Conv_ntt2poly(POLYNOMIAL* res, POLYNOMIAL* poly) {
  FMT_ASSERT(Is_ntt(poly), "already coeffcient form");
  FMT_ASSERT(Is_size_match(res, poly), "size not match");

  CRT_CONTEXT* crt         = Get_crt_context();
  CRT_PRIME*   q_prime     = Get_prime_head(Get_q(crt));
  CRT_PRIME*   p_prime     = Get_prime_head(Get_p(crt));
  int64_t*     poly_data   = Get_poly_coeffs(poly);
  int64_t*     res_data    = Get_poly_coeffs(res);
  uint32_t     ring_degree = Get_rdgree(poly);
  VALUE_LIST   tmp_ntt, res_ntt;
  Init_i64_value_list_no_copy(&tmp_ntt, ring_degree, poly_data);
  Init_i64_value_list_no_copy(&res_ntt, ring_degree, res_data);
  for (size_t qi = 0; qi < Poly_level(poly); qi++) {
    NTT_CONTEXT* ntt = Get_ntt(q_prime);
    Ftt_inv(&res_ntt, ntt, &tmp_ntt);
    tmp_ntt._vals._i += ring_degree;
    res_ntt._vals._i += ring_degree;
    q_prime = Get_next_prime(q_prime);
  }
  if (Num_p(poly)) {
    tmp_ntt._vals._i = Get_p_coeffs(poly);
    res_ntt._vals._i = Get_p_coeffs(res);
    for (size_t pi = 0; pi < Num_p(poly); pi++) {
      NTT_CONTEXT* ntt = Get_ntt(p_prime);
      Ftt_inv(&res_ntt, ntt, &tmp_ntt);
      tmp_ntt._vals._i += ring_degree;
      res_ntt._vals._i += ring_degree;
      p_prime = Get_next_prime(p_prime);
    }
  }
  Set_is_ntt(res, FALSE);
}

void Conv_ntt2poly_with_primes(POLYNOMIAL* res, POLYNOMIAL* poly,
                               VL_CRTPRIME* primes) {
  FMT_ASSERT(Is_ntt(poly), "already coeffcient form");
  FMT_ASSERT(Is_size_match(res, poly), "size not match");
  int64_t*   poly_data   = Get_poly_coeffs(poly);
  int64_t*   res_data    = Get_poly_coeffs(res);
  uint32_t   ring_degree = Get_rdgree(poly);
  VALUE_LIST tmp_ntt, res_ntt;
  Init_i64_value_list_no_copy(&tmp_ntt, ring_degree, poly_data);
  Init_i64_value_list_no_copy(&res_ntt, ring_degree, res_data);

  for (size_t qi = 0; qi < Poly_level(poly); qi++) {
    CRT_PRIME*   prime = Get_vlprime_at(primes, qi);
    NTT_CONTEXT* ntt   = Get_ntt(prime);
    Ftt_inv(&res_ntt, ntt, &tmp_ntt);
    tmp_ntt._vals._i += ring_degree;
    res_ntt._vals._i += ring_degree;
  }
  Set_is_ntt(res, FALSE);
}

void Conv_ntt2poly_inplace_with_primes(POLYNOMIAL* poly, VL_CRTPRIME* primes) {
  FMT_ASSERT(Is_ntt(poly), "already coeffcient form");
  FMT_ASSERT(Poly_level(poly) == LIST_LEN(primes), "primes size not match");
  int64_t*   poly_data   = Get_poly_coeffs(poly);
  uint32_t   ring_degree = Get_rdgree(poly);
  VALUE_LIST tmp_ntt;
  Init_i64_value_list_no_copy(&tmp_ntt, ring_degree, poly_data);
  for (size_t qi = 0; qi < Poly_level(poly); qi++) {
    CRT_PRIME*   prime = Get_vlprime_at(primes, qi);
    NTT_CONTEXT* ntt   = Get_ntt(prime);
    Ftt_inv(&tmp_ntt, ntt, &tmp_ntt);
    tmp_ntt._vals._i += ring_degree;
    poly_data += ring_degree;
  }
  Set_is_ntt(poly, FALSE);
}

void Extract_poly(POLYNOMIAL* res, POLYNOMIAL* poly, size_t start_prime_ofst,
                  size_t prime_cnt) {
  FMT_ASSERT(poly && start_prime_ofst <= Num_alloc(poly) &&
                 start_prime_ofst + prime_cnt <= Num_alloc(poly),
             "invalid range");
  int64_t* start_data =
      Get_poly_coeffs(poly) + start_prime_ofst * Get_rdgree(poly);
  res->_num_alloc_primes = prime_cnt;
  Init_poly_data(res, Get_rdgree(poly), prime_cnt, 0, start_data);
  Set_is_ntt(res, Is_ntt(poly));
}

// fast convert polynomial RNS bases from old_primes to new_primes
void Fast_base_conv(POLYNOMIAL* new_poly, POLYNOMIAL* old_poly,
                    CRT_PRIMES* new_primes, CRT_PRIMES* old_primes) {
  uint32_t ring_degree = Get_rdgree(old_poly);
  // NOTE: after rescale num_primes of poly may not equal to num_primes of crt
  IS_TRUE(Poly_level(new_poly) <= Get_primes_cnt(new_primes) &&
              Poly_level(old_poly) <= Get_primes_cnt(old_primes),
          "unmatched size");
  IS_TRUE(Get_rdgree(new_poly) == Get_rdgree(old_poly),
          "unmatched ring_degree");

  size_t      old_level    = Poly_level(old_poly);
  size_t      new_level    = Poly_level(new_poly);
  CRT_PRIME*  old_prime    = Get_prime_head(old_primes);
  CRT_PRIME*  new_prime    = Get_prime_head(new_primes);
  VALUE_LIST* inv_mod_self = Is_q(old_primes)
                                 ? Get_qhatinvmodq_at(old_primes, old_level - 1)
                                 : Get_phatinvmodp(old_primes);
  VALUE_LIST* inv_mod_self_prec =
      Is_q(old_primes) ? Get_qhatinvmodq_prec_at(old_primes, old_level - 1)
                       : Get_phatinvmodp_prec(old_primes);

  // element-wise old_poly * inv_mod_self
  int64_t* arr_mul_inv_modself =
      malloc(sizeof(int64_t) * ring_degree * old_level);
  int64_t* p_mul_inv_modself = arr_mul_inv_modself;
  int64_t* old_coeffs        = Get_poly_coeffs(old_poly);
  for (size_t o_idx = 0; o_idx < old_level; o_idx++) {
    int64_t mod              = Get_modulus_val(Get_nth_prime(old_prime, o_idx));
    int64_t inv_mod_self_val = Get_i64_value_at(inv_mod_self, o_idx);
    int64_t inv_mod_self_val_prec = Get_i64_value_at(inv_mod_self_prec, o_idx);
    for (uint32_t d_idx = 0; d_idx < ring_degree; d_idx++) {
      *p_mul_inv_modself = Fast_mul_const_with_mod(
          *old_coeffs, inv_mod_self_val, inv_mod_self_val_prec, mod);
      p_mul_inv_modself++;
      old_coeffs++;
    }
  }
  for (size_t n_idx = 0; n_idx < new_level; n_idx++) {
    MODULUS*    new_modulus = Get_modulus(Get_nth_prime(new_prime, n_idx));
    VALUE_LIST* mod_nb      = Get_phatmodq_at(old_primes, n_idx);
    for (uint32_t d_idx = 0; d_idx < ring_degree; d_idx++) {
      INT128_T sum = 0;
      for (size_t o_idx = 0; o_idx < old_level; o_idx++) {
        sum += (INT128_T)(arr_mul_inv_modself[o_idx * ring_degree + d_idx]) *
               Get_i64_value_at(mod_nb, o_idx);
      }
      int64_t new_value = Mod_barrett_128(sum, new_modulus);
      Set_coeff_at(new_poly, new_value, n_idx * ring_degree + d_idx);
    }
  }
  Set_is_ntt(new_poly, Is_ntt(old_poly));
  free(arr_mul_inv_modself);
}

void Fast_base_conv_with_parts(POLYNOMIAL* new_poly, POLYNOMIAL* old_poly,
                               CRT_PRIMES* qpart, VL_CRTPRIME* new_primes,
                               size_t part_idx, size_t level) {
  uint32_t     ring_degree = Get_rdgree(old_poly);
  VL_CRTPRIME* qpart_prime = Get_qpart_prime_at_l1(qpart, part_idx);
  size_t       partq_level = Poly_level(old_poly) - 1;
  IS_TRUE(Get_rdgree(new_poly) == Get_rdgree(old_poly),
          "unmatched ring_degree");
  VALUE_LIST* ql_hatinvmodq =
      VL_L2_VALUE_AT(Get_qlhatinvmodq(qpart), part_idx, partq_level);
  VL_VL_I64* ql_hatmodp = VL_L2_VALUE_AT(Get_qlhatmodp(qpart), level, part_idx);
  size_t     size_q     = Poly_level(old_poly) > LIST_LEN(qpart_prime)
                              ? LIST_LEN(qpart_prime)
                              : Poly_level(old_poly);
  size_t     size_p     = Poly_level(new_poly);

  INT128_T sum[size_p];
  memset(sum, 0, size_p * sizeof(INT128_T));

  for (size_t n = 0; n < ring_degree; n++) {
    for (size_t i = 0; i < size_q; i++) {
      int64_t  val = Get_coeff_at(old_poly, i * ring_degree + n);
      MODULUS* qi  = (MODULUS*)Get_modulus(Get_vlprime_at(qpart_prime, i));
      int64_t  mul_inv_modq =
          Mul_int64_mod_barret(val, I64_VALUE_AT(ql_hatinvmodq, i), qi);
      VALUE_LIST* qhat_modp = VL_VALUE_AT(ql_hatmodp, i);
      for (size_t j = 0; j < size_p; j++) {
        sum[j] += (INT128_T)mul_inv_modq * Get_i64_value_at(qhat_modp, j);
      }
    }
    for (size_t j = 0; j < size_p; j++) {
      int64_t new_value =
          Mod_barrett_128(sum[j], Get_modulus(Get_vlprime_at(new_primes, j)));
      Set_coeff_at(new_poly, new_value, j * ring_degree + n);
      sum[j] = 0;
    }
  }
}

void Modup_inplace(POLYNOMIAL* raised, POLYNOMIAL* part2_poly,
                   size_t decomp_idx) {
  uint32_t     degree    = Get_rdgree(raised);
  CRT_CONTEXT* crt       = Get_crt_context();
  CRT_PRIMES*  q_parts   = Get_qpart(crt);
  size_t       part_size = Get_per_part_size(q_parts);
  size_t       num_q     = Poly_level(raised);
  VL_CRTPRIME* compl_primes =
      Get_qpart_compl_at(Get_qpart_compl(crt), num_q - 1, decomp_idx);
  VL_CRTPRIME* part2_primes = Get_vl_value_at(Get_primes(q_parts), decomp_idx);

  // number of primes of part1,2,3
  size_t num_part1       = part_size * decomp_idx;
  size_t num_part2       = Poly_level(part2_poly);
  size_t num_part3       = Get_num_pq(raised) - num_part1 - num_part2;
  size_t num_compl       = LIST_LEN(compl_primes);
  size_t part3_start_idx = num_part1 + num_part2;

  VALUE_LIST* ql_hatinvmodq =
      VL_L2_VALUE_AT(Get_qlhatinvmodq(q_parts), decomp_idx, num_part2 - 1);
  VL_VL_I64* ql_hatmodp =
      VL_L2_VALUE_AT(Get_qlhatmodp(q_parts), num_q - 1, decomp_idx);
  int64_t* raised_part1 = Get_poly_coeffs_at(raised, 0);
  int64_t* raised_part3 = Get_poly_coeffs_at(raised, part3_start_idx);
  int64_t* raised_data  = raised_part1;

  int64_t* arr_mul_inv_modq = malloc(sizeof(int64_t) * degree * num_part2);
  int64_t* p_mul_inv_modq   = arr_mul_inv_modq;
  int64_t* part2_coeffs     = Get_poly_coeffs(part2_poly);
  for (size_t o_idx = 0; o_idx < num_part2; o_idx++) {
    int64_t  qlhatinv = Get_i64_value_at(ql_hatinvmodq, o_idx);
    MODULUS* qi = (MODULUS*)Get_modulus(Get_vlprime_at(part2_primes, o_idx));
    for (uint32_t d_idx = 0; d_idx < degree; d_idx++) {
      *p_mul_inv_modq = Mul_int64_mod_barret(*part2_coeffs, qlhatinv, qi);
      p_mul_inv_modq++;
      part2_coeffs++;
    }
  }
  for (size_t n_idx = 0; n_idx < num_compl; n_idx++) {
    MODULUS* new_modulus = Get_modulus(Get_vlprime_at(compl_primes, n_idx));
    if (n_idx == num_part1) raised_data = raised_part3;
    for (uint32_t d_idx = 0; d_idx < degree; d_idx++) {
      INT128_T sum = 0;
      for (size_t o_idx = 0; o_idx < num_part2; o_idx++) {
        VALUE_LIST* qhat_modp = VL_VALUE_AT(ql_hatmodp, o_idx);
        sum += (INT128_T)arr_mul_inv_modq[o_idx * degree + d_idx] *
               Get_i64_value_at(qhat_modp, n_idx);
      }
      int64_t new_value = Mod_barrett_128(sum, new_modulus);
      *raised_data++    = new_value;
    }
  }
  free(arr_mul_inv_modq);
  Set_is_ntt(raised, false);
}

void Print_poly_detail(FILE* fp, POLYNOMIAL* poly, VL_CRTPRIME* primes,
                       bool detail) {
  IS_TRUE(poly, "null polynomial");
  if (Get_poly_len(poly) == 0) {
    fprintf(fp, "{}\n");
    return;
  }
  fprintf(fp, "level = %ld; ", Poly_level(poly));
  fprintf(fp, "%d * %ld", poly->_ring_degree, poly->_num_primes);
  if (poly->_num_primes_p) {
    fprintf(fp, "* %ld", poly->_num_primes_p);
  }
  fprintf(fp, " {");
  for (size_t i = 0; i < poly->_num_primes; i++) {
    fprintf(fp, " {");
    int64_t* coeffs = poly->_data + i * poly->_ring_degree;
    for (int64_t j = poly->_ring_degree - 1; j >= 0; j--) {
      if (coeffs[j]) {
        fprintf(fp, "%ld", coeffs[j]);
        if (j != 0) {
          fprintf(fp, "x^%ld + ", j);
        }
      }
    }
    fprintf(fp, " }");
  }
  for (size_t i = 0; i < poly->_num_primes_p; i++) {
    fprintf(fp, " {");
    int64_t* coeffs = poly->_data + poly->_num_primes * poly->_ring_degree +
                      i * poly->_ring_degree;
    for (int64_t j = poly->_ring_degree - 1; j >= 0; j--) {
      if (coeffs[j]) {
        fprintf(fp, "%ld", coeffs[j]);
        if (j != 0) {
          fprintf(fp, "x^%ld + ", j);
        }
      }
    }
    fprintf(fp, " }");
  }
  fprintf(fp, "}\n");
  Print_poly_rawdata(fp, poly);

  if (detail && primes) {
    if (poly->_is_ntt) {
      POLYNOMIAL non_ntt;
      Alloc_poly_data(&non_ntt, poly->_ring_degree, poly->_num_primes,
                      poly->_num_primes_p);
      Conv_ntt2poly_with_primes(&non_ntt, poly, primes);
      Print_poly_rawdata(fp, &non_ntt);
      Free_poly_data(&non_ntt);
    } else {
      POLYNOMIAL ntt;
      Alloc_poly_data(&ntt, poly->_ring_degree, poly->_num_primes,
                      poly->_num_primes_p);
      Conv_poly2ntt_with_primes(&ntt, poly, primes);
      Print_poly_rawdata(fp, &ntt);
      Free_poly_data(&ntt);
    }
  }
  fprintf(fp, "\n\n");
}

void Print_rns_poly(FILE* fp, POLYNOMIAL* poly) {
  VALUE_LIST* res = Alloc_value_list(BIGINT_TYPE, poly->_ring_degree);
  Reconstruct_rns_poly_to_value(res, poly);
  for (int64_t idx = poly->_ring_degree - 1; idx >= 0; idx--) {
    if (BI_CMP_SI(BIGINT_VALUE_AT(res, idx), 0) == 0) continue;
    BI_FPRINTF(fp, BI_FORMAT, BIGINT_VALUE_AT(res, idx));
    if (idx != 0) {
      BI_FPRINTF(fp, "x^%ld + ", idx);
    }
  }
  fprintf(fp, "\n");
  Free_value_list(res);
}

void Print_poly_list(FILE* fp, VALUE_LIST* poly_list) {
  FOR_ALL_ELEM(poly_list, idx) {
    POLYNOMIAL* poly = (POLYNOMIAL*)Get_ptr_value_at(poly_list, idx);
    fprintf(fp, "POLY[%ld]\n", idx);
    Print_poly(fp, poly);
  }
}

VALUE_LIST* Reduce_and_rescale_modup(POLYNOMIAL* res, POLYNOMIAL* poly) {
  CRT_CONTEXT* crt         = Get_crt_context();
  CRT_PRIMES*  q_primes    = Get_q(crt);
  CRT_PRIMES*  p_primes    = Get_p(crt);
  CRT_PRIMES*  q_parts     = Get_qpart(crt);
  size_t       part_size   = Get_per_part_size(q_parts);
  size_t       num_p       = Num_p(poly);
  size_t       num_q       = Poly_level(poly);
  uint32_t     ring_degree = Get_rdgree(poly);

  // reuse res for reduce and rescale, make sure res num_q size as large as poly
  IS_TRUE(poly && num_q > 1 && Get_primes_cnt(p_primes) == num_p,
          "Reduce_and_rescale_modup: size not match");

  if (Is_ntt(poly)) {
    Conv_ntt2poly_inplace(poly);
  }

  size_t      num_decomp       = Get_num_decomp(crt, num_q - 1);
  VALUE_LIST* poly_raised_list = Alloc_value_list(PTR_TYPE, num_decomp);
  POLY_ARR    arr = Alloc_polys(num_decomp, ring_degree, num_q - 1, num_p);
  FOR_ALL_ELEM(poly_raised_list, idx) {
    Set_ptr_value(poly_raised_list, idx, (PTR)(&arr[idx]));
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

  int64_t* old_data           = Get_poly_coeffs(poly);
  int64_t* res_data           = Get_poly_coeffs(res);
  VL_I64*  ql_inv_mod_qi      = Get_ql_inv_mod_qi_at(q_primes, num_q - 2);
  VL_I64*  ql_inv_mod_qi_prec = Get_ql_inv_mod_qi_prec_at(q_primes, num_q - 2);
  size_t   decomp_idx         = 0;
  size_t   part_idx           = 0;
  POLYNOMIAL  part2_poly;
  POLYNOMIAL* raised           = NULL;
  int64_t*    new_data         = NULL;
  size_t      last_part2_size  = last_idx - part_size * (num_decomp - 1);
  bool        is_last_part     = (decomp_idx == num_decomp - 1);
  size_t      cur_part_size    = is_last_part ? last_part2_size : part_size;
  int64_t*    arr_mul_inv_modq = NULL;
  int64_t*    p_mul_inv_modq   = NULL;
  for (size_t q_idx = 0; q_idx < last_idx; q_idx++) {
    MODULUS* q_modulus          = Get_modulus(Get_prime_at(q_primes, q_idx));
    int64_t  qi                 = Get_mod_val(q_modulus);
    int64_t  ql_inv             = Get_i64_value_at(ql_inv_mod_qi, q_idx);
    int64_t  ql_inv_prec        = Get_i64_value_at(ql_inv_mod_qi_prec, q_idx);
    int64_t  p_m_inv_modqi      = p_m_inv_modq[q_idx];
    int64_t  p_m_inv_modqi_prec = p_m_inv_modq_prec[q_idx];

    VALUE_LIST* ql_hatinvmodq = VL_L2_VALUE_AT(Get_qlhatinvmodq(q_parts),
                                               decomp_idx, cur_part_size - 1);
    // int64_t     qlhatinv      = Get_i64_value_at(ql_hatinvmodq, part_idx);
    if (part_idx == 0) {
      raised = (POLYNOMIAL*)Get_ptr_value_at(poly_raised_list, decomp_idx);
      Extract_poly(&part2_poly, raised, part_size * decomp_idx, cur_part_size);
      new_data = Get_poly_coeffs(&part2_poly);
      // arr_mul_inv_modq = malloc(sizeof(int64_t) * ring_degree *
      // cur_part_size); p_mul_inv_modq   = arr_mul_inv_modq;
    }
    for (uint32_t d_idx = 0; d_idx < ring_degree; d_idx++) {
      int64_t new_val = Sub_mul_const_mod(*old_data, *res_data, p_m_inv_modqi,
                                          p_m_inv_modqi_prec, qi);
      new_val =
          Sub_mul_const_mod(new_val, last_new[d_idx], ql_inv, ql_inv_prec, qi);
      // *p_mul_inv_modq++ = Mul_int64_mod_barret(new_val, qlhatinv, q_modulus);
      *new_data = new_val;
      old_data++;
      new_data++;
      res_data++;
    }
    if (part_idx == cur_part_size - 1) {
      Modup_inplace(raised, &part2_poly, decomp_idx);
      decomp_idx++;
      part_idx      = 0;
      is_last_part  = (decomp_idx == num_decomp - 1);
      cur_part_size = is_last_part ? last_part2_size : part_size;
    } else {
      part_idx++;
    }
  }
  Modswitch(res, res);
  return poly_raised_list;
}

void Free_precomp(VALUE_LIST* precomputed) {
  if (precomputed == NULL) return;
  // the value list stores array of POLYNOMIAL pointer allocated
  // by Alloc_polys, first free the polys and then free the
  // value list
  POLY_ARR arr_head = (POLY_ARR)PTR_VALUE_AT(precomputed, 0);
  Free_polys(arr_head);
  Free_value_list(precomputed);
}
