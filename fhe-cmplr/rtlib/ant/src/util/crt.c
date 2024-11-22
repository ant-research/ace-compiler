//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "util/crt.h"

#include "common/trace.h"
#include "util/ckks_parameters.h"
#include "util/ntt.h"
#include "util/number_theory.h"

int64_t Gen_first_prime(uint32_t ring_degree, size_t mod_size) {
  IS_TRUE(mod_size <= 64, "Make sure primes are within 64 bits");
  size_t  order          = 2 * ring_degree;
  int64_t possible_prime = (1L << mod_size) + order + 1;
  while (!Is_prime(possible_prime)) {
    possible_prime += order;
  }
  return possible_prime;
}

int64_t Gen_previous_prime(int64_t mod, size_t order) {
  int64_t possible_prime = mod - order;
  while (!Is_prime(possible_prime)) {
    possible_prime -= order;
  }
  return possible_prime;
}

int64_t Gen_next_prime(int64_t mod, size_t order) {
  int64_t possible_prime = mod + order;
  do {
    possible_prime += order;
    IS_TRUE(possible_prime > mod, "next prime overflow growing candidate");
  } while (!Is_prime(possible_prime));
  return possible_prime;
}

//! @brief Generates primes for p
//! @param The new generated prime should not equal to primes in base
void Generate_p_primes(CRT_PRIMES* crt_primes, size_t num_primes,
                       size_t mod_size, uint32_t ring_degree,
                       CRT_PRIMES* base) {
  IS_TRUE(base != NULL,
          "make sure moduli in P and Q are different, input base should not be "
          "NULL");
  VALUE_LIST* base_primes = Get_primes(base);
  IS_TRUE(!base_primes || LIST_TYPE(base_primes) == PTR_TYPE,
          "base_primes is not PTR_TYPE");
  FMT_ASSERT(mod_size <= 64, "make sure primes are within 64 bits");

  int64_t     possible_prime = Gen_first_prime(ring_degree, mod_size);
  int64_t     p_prev         = possible_prime;
  VALUE_LIST* primes         = Alloc_value_list(I64_TYPE, num_primes);
  size_t      mod            = 2 * ring_degree;
  for (size_t i = 0; i < num_primes; i++) {
    bool found_in_base = FALSE;
    do {
      possible_prime = Gen_previous_prime(p_prev, mod);
      found_in_base  = FALSE;
      for (size_t j = 0; j < LIST_LEN(base_primes); j++) {
        if (possible_prime == Get_modulus_val(Get_prime_at(base, j))) {
          found_in_base = TRUE;
          break;
        }
      }
      p_prev = possible_prime;
    } while (found_in_base);
    I64_VALUE_AT(primes, i) = possible_prime;
  }
  Set_primes(crt_primes, primes, ring_degree);
  Free_value_list(primes);
}

/**
 * @brief generates primes that are 1 (mod M), where M is twice the polynomial
 * degree
 *
 * @param crt_primes crt primes
 * @param num_primes number of primes
 * @param first_mod_size bit size of first mod
 * @param scaling_mod_size bit size of scaling mod
 * @param ring_degree ring degree of polynomial
 */
void Generate_q_primes(CRT_PRIMES* crt_primes, size_t num_primes,
                       size_t first_mod_size, size_t scaling_mod_size,
                       uint32_t ring_degree) {
  FMT_ASSERT(first_mod_size <= 64 && scaling_mod_size <= 64,
             "Make sure primes are within 64 bits");

  size_t      mod            = 2 * ring_degree;
  VALUE_LIST* primes         = Alloc_value_list(I64_TYPE, num_primes);
  int64_t     possible_prime = Gen_first_prime(ring_degree, scaling_mod_size);
  I64_VALUE_AT(primes, num_primes - 1) = possible_prime;
  int64_t q_next                       = possible_prime;
  int64_t q_prev                       = possible_prime;

  if (num_primes > 1) {
    uint32_t cnt = 0;
    for (size_t i = num_primes - 2; i >= 1; i--) {
      if ((cnt % 2) == 0) {
        q_prev         = Gen_previous_prime(q_prev, mod);
        possible_prime = q_prev;
      } else {
        q_next         = Gen_next_prime(q_next, mod);
        possible_prime = q_next;
      }
      I64_VALUE_AT(primes, i) = possible_prime;
      cnt++;
    }
  }
  // generate first prime at given first_mod_size
  if (first_mod_size == scaling_mod_size) {
    I64_VALUE_AT(primes, 0) = Gen_previous_prime(q_prev, mod);
  } else {
    int64_t first_p         = Gen_first_prime(ring_degree, first_mod_size);
    I64_VALUE_AT(primes, 0) = Gen_previous_prime(first_p, mod);
  }
  Set_primes(crt_primes, primes, ring_degree);
  Free_value_list(primes);
}

//

/**
 * @brief for FLEXIBLEAUTO scaling technical:
 * once one scaling factor gets far enough from the original scaling factor,
 * subsequent level scaling factors quickly diverge to either 0 or infinity.
 * This goal is to maintain the scaling factor of all levels as close to the
 * original scale factor of level 0 as possible.
 *
 * @param crt_primes generated crt primes
 * @param num_primes number of q primes
 * @param first_mod_size bit size of first mod
 * @param scaling_mod_size bit size of scaling mod
 * @param ring_degree ring degree of polynomial
 */
void Generate_q_primes_with_smaller_deviation(CRT_PRIMES* crt_primes,
                                              size_t      num_primes,
                                              size_t      first_mod_size,
                                              size_t      scaling_mod_size,
                                              uint32_t    ring_degree) {
  IS_TRUE(first_mod_size <= 64 && scaling_mod_size <= 64,
          "Make sure primes are within 64 bits");

  size_t      mod    = 2 * ring_degree;
  VALUE_LIST* primes = Alloc_value_list(I64_TYPE, num_primes);
  int64_t     q      = Gen_first_prime(ring_degree, scaling_mod_size);
  I64_VALUE_AT(primes, num_primes - 1) = q;
  int64_t q_next                       = q;
  int64_t q_prev                       = q;
  double  sf = (double)I64_VALUE_AT(primes, num_primes - 1);

  size_t cnt = 0;
  if (num_primes > 1) {
    for (size_t i = num_primes - 2; i >= 1; i--) {
      sf = (double)(pow(sf, 2) / (double)I64_VALUE_AT(primes, i + 1));
      int64_t sf_int    = llround(sf);
      int64_t sf_rem    = Mod_int64(sf_int, mod);
      int64_t gen_prime = 0;
      if ((cnt % 2) == 0) {
        q_prev            = q_prev == q ? sf_int - mod - sf_rem + 1 : q_prev;
        bool has_same_mod = TRUE;
        while (has_same_mod) {
          has_same_mod = FALSE;
          q_prev       = Gen_previous_prime(q_prev, mod);
          for (size_t j = i + 1; j < num_primes; j++) {
            if (q_prev == I64_VALUE_AT(primes, j)) {
              has_same_mod = TRUE;
            }
          }
        }
        gen_prime = q_prev;
      } else {
        q_next            = q_next == q ? sf_int + mod - sf_rem + 1 : q_next;
        bool has_same_mod = TRUE;
        while (has_same_mod) {
          has_same_mod = FALSE;
          q_next       = Gen_next_prime(q_next, mod);
          for (size_t j = i + 1; j < num_primes; j++) {
            if (q_next == I64_VALUE_AT(primes, j)) {
              has_same_mod = TRUE;
            }
          }
        }
        gen_prime = q_next;
      }
      I64_VALUE_AT(primes, i) = gen_prime;
      cnt++;
    }
  }
  if (first_mod_size == scaling_mod_size) {
    I64_VALUE_AT(primes, 0) = Gen_previous_prime(q_prev, mod);
  } else {
    int64_t first_p         = Gen_first_prime(ring_degree, first_mod_size);
    I64_VALUE_AT(primes, 0) = Gen_previous_prime(first_p, mod);
  }
  Set_primes(crt_primes, primes, ring_degree);
  Free_value_list(primes);
}

void Precompute_primes(CRT_PRIMES* crt_primes, bool without_rescale) {
  IS_TRUE(Is_q(crt_primes) || Is_p(crt_primes), "invalid crt prime type");
  PRE_COMP* precomp       = Alloc_precompute();
  crt_primes->_u._precomp = precomp;
  VALUE_LIST* primes      = Get_primes(crt_primes);
  if (primes) {
    size_t num_primes = Get_primes_cnt(crt_primes);

    BI_INIT_ASSIGN_SI(GET_BIG_M(crt_primes), 1);
    for (size_t i = 0; i < Get_primes_cnt(crt_primes); i++) {
      BI_MUL_UI(GET_BIG_M(crt_primes), GET_BIG_M(crt_primes),
                Get_modulus_val(Get_vlprime_at(primes, i)));
    }
    // NOTE: crt_vals and crt_inv are bigint,
    //       the values are calculated from (p0 * p1...*pn) /pi
    precomp->_hat     = Alloc_value_list(BIGINT_TYPE, num_primes);
    precomp->_hat_inv = Alloc_value_list(BIGINT_TYPE, num_primes);
    for (size_t i = 0; i < num_primes; i++) {
      int64_t qi = Get_modulus_val(Get_vlprime_at(primes, i));
      BI_DIV_UI(BIGINT_VALUE_AT(Get_hat(crt_primes), i), GET_BIG_M(crt_primes),
                qi);
      Bi_mod_inv(BIGINT_VALUE_AT(Get_hat_inv(crt_primes), i),
                 BIGINT_VALUE_AT(Get_hat(crt_primes), i), qi);
      IS_TRUE(BIGINT_VALUE_AT(Get_hat_inv(crt_primes), i),
              "mod res outof 64 bit");
    }

    precomp->_hat_inv_mod_self =
        Alloc_value_list(VL_PTR_TYPE, Get_reducible_levels(crt_primes));
    precomp->_hat_inv_mod_self_prec =
        Alloc_value_list(VL_PTR_TYPE, Get_reducible_levels(crt_primes));
    for (size_t idx = 0; idx < Get_reducible_levels(crt_primes); idx++) {
      VALUE_LIST* hat_inv_mod_self = Alloc_value_list(I64_TYPE, num_primes);
      VALUE_LIST* hat_inv_mod_self_prec =
          Alloc_value_list(I64_TYPE, num_primes);
      VL_VALUE_AT(precomp->_hat_inv_mod_self, idx)      = hat_inv_mod_self;
      VL_VALUE_AT(precomp->_hat_inv_mod_self_prec, idx) = hat_inv_mod_self_prec;
      size_t max_level = Is_q(crt_primes) ? idx + 1 : num_primes;
      for (size_t l = 0; l < max_level; ++l) {
        MODULUS* m_prime      = Get_modulus(Get_vlprime_at(primes, l));
        int64_t  prime        = Get_mod_val(m_prime);
        int64_t  hat_mod_self = 1;
        for (size_t hat_idx = 0; hat_idx < l; ++hat_idx) {
          int64_t temp = Get_modulus_val(Get_vlprime_at(primes, hat_idx));
          hat_mod_self = Mul_int64_with_mod(hat_mod_self, temp, prime);
        }
        for (size_t hat_idx = l + 1; hat_idx < max_level; ++hat_idx) {
          int64_t temp = Get_modulus_val(Get_vlprime_at(primes, hat_idx));
          hat_mod_self = Mul_int64_with_mod(hat_mod_self, temp, prime);
        }
        int64_t mod_inv = Mod_inv_prime(hat_mod_self, m_prime);
        Set_i64_value(hat_inv_mod_self, l, mod_inv);

        // precompute barret inverse values for _hat_inv_mod_self
        Set_i64_value(hat_inv_mod_self_prec, l,
                      Precompute_const(mod_inv, prime));
      }
    }
    // Pre-compute values for rescaling
    // _ql_div2_mod_qi[i] = {((q_i+1)/2)) % q_0, ...((q_i+1)/2)) % q_i}
    // _ql_inv_mod_qi[i]  = {((q_i+1)^-1) % q_0, ...((q_i+1)^-1)) % q_i}
    // _ql_ql_inv_mod_ql_div_ql_mod_qi[i] =
    //    {((Q/q_i+1)*((Q/q_i+1)^-1 % q_i+1))/q_i+1) % q_0,
    //    ...((Q/q_i+1)*((Q/q_i+1)^-1 % q_i+1))/q_i+1) % q_i}
    if (Is_q(crt_primes) && !without_rescale) {
      size_t len                   = num_primes - 1;
      precomp->_ql_div2_mod_qi     = Alloc_value_list(VL_PTR_TYPE, len);
      precomp->_ql_inv_mod_qi      = Alloc_value_list(VL_PTR_TYPE, len);
      precomp->_ql_inv_mod_qi_prec = Alloc_value_list(VL_PTR_TYPE, len);
      precomp->_ql_ql_inv_mod_ql_div_ql_mod_qi =
          Alloc_value_list(VL_PTR_TYPE, len);
      precomp->_ql_ql_inv_mod_ql_div_ql_mod_qi_prec =
          Alloc_value_list(VL_PTR_TYPE, len);
      BIG_INT ql, last, ql_inv_modql, result, qi, res;
      BI_INITS(ql, last, ql_inv_modql, result, qi, res);
      for (size_t k = 0; k < len; k++) {
        size_t      level              = k + 1;
        VALUE_LIST* ql_div2_mod_qi     = Alloc_value_list(I64_TYPE, level);
        VALUE_LIST* ql_inv_mod_qi      = Alloc_value_list(I64_TYPE, level);
        VALUE_LIST* ql_inv_mod_qi_prec = Alloc_value_list(I64_TYPE, level);
        VALUE_LIST* ql_ql_inv_mod_ql_div_ql_mod_qi =
            Alloc_value_list(I64_TYPE, level);
        VALUE_LIST* ql_ql_inv_mod_ql_div_ql_mod_qi_prec =
            Alloc_value_list(I64_TYPE, level);
        VL_VALUE_AT(precomp->_ql_div2_mod_qi, k)     = ql_div2_mod_qi;
        VL_VALUE_AT(precomp->_ql_inv_mod_qi, k)      = ql_inv_mod_qi;
        VL_VALUE_AT(precomp->_ql_inv_mod_qi_prec, k) = ql_inv_mod_qi_prec;
        VL_VALUE_AT(precomp->_ql_ql_inv_mod_ql_div_ql_mod_qi, k) =
            ql_ql_inv_mod_ql_div_ql_mod_qi;
        VL_VALUE_AT(precomp->_ql_ql_inv_mod_ql_div_ql_mod_qi_prec, k) =
            ql_ql_inv_mod_ql_div_ql_mod_qi_prec;
        CRT_PRIME* last_prime = Get_vlprime_at(primes, level);
        int64_t    last_mod   = Get_mod_val(Get_modulus(last_prime));
        int64_t    half       = last_mod >> 1;

        BI_ASSIGN_SI(last, last_mod);
        BI_ASSIGN(ql_inv_modql,
                  BIGINT_VALUE_AT(Get_hat_inv(crt_primes), level));
        BI_MOD(ql_inv_modql, ql_inv_modql, last);
        BI_MUL(ql_inv_modql, ql_inv_modql,
               BIGINT_VALUE_AT(Get_hat(crt_primes), level));
        BI_DIV(ql_inv_modql, ql_inv_modql, last);
        for (size_t i = 0; i < level; i++) {
          CRT_PRIME* prime   = Get_vlprime_at(primes, i);
          MODULUS*   modulus = Get_modulus(prime);
          int64_t    mod_inv = Mod_inv_prime(last_mod, modulus);
          BI_ASSIGN_SI(qi, Get_mod_val(Get_modulus(prime)));
          BI_MOD(res, ql_inv_modql, qi);
          int64_t result = BI_GET_SI(res);

          Set_i64_value(ql_div2_mod_qi, i, Mod_barrett_64(half, modulus));
          Set_i64_value(ql_inv_mod_qi, i, mod_inv);
          Set_i64_value(ql_inv_mod_qi_prec, i,
                        Precompute_const(mod_inv, Get_mod_val(modulus)));
          Set_i64_value(ql_ql_inv_mod_ql_div_ql_mod_qi, i, result);
          Set_i64_value(ql_ql_inv_mod_ql_div_ql_mod_qi_prec, i,
                        Precompute_const(result, Get_mod_val(modulus)));
        }
      }
      BI_FREES(ql, last, ql_inv_modql, result, qi, res);
    }
  }
}

void Precompute_new_base(CRT_PRIMES* base, CRT_PRIMES* new_base) {
  size_t    base_cnt = Get_primes_cnt(base);
  size_t    nb_cnt   = Get_primes_cnt(new_base);
  size_t    levels   = Get_reducible_levels(base);
  PRE_COMP* precomp  = Get_precomp(base);
  IS_TRUE(precomp, "null precomputed ptr");

  precomp->_m_mod_nb     = Alloc_value_list(VL_PTR_TYPE, levels);
  precomp->_m_inv_mod_nb = Alloc_value_list(VL_PTR_TYPE, levels);
  precomp->_hat_mod_nb   = Alloc_value_list(VL_PTR_TYPE, levels);

  VALUE_LIST* new_base_primes = Get_primes(new_base);
  VALUE_LIST* base_primes     = Get_primes(base);
  for (size_t base_idx = 0; base_idx < Get_reducible_levels(base); base_idx++) {
    VALUE_LIST* m_mod_nb     = Alloc_value_list(I64_TYPE, nb_cnt);
    VALUE_LIST* m_inv_mod_nb = Alloc_value_list(I64_TYPE, nb_cnt);
    VALUE_LIST* hat_mod_nb   = Alloc_value_list(VL_PTR_TYPE, nb_cnt);
    VL_VALUE_AT(precomp->_m_mod_nb, base_idx)     = m_mod_nb;
    VL_VALUE_AT(precomp->_m_inv_mod_nb, base_idx) = m_inv_mod_nb;
    VL_VALUE_AT(precomp->_hat_mod_nb, base_idx)   = hat_mod_nb;
    size_t base_end = Is_q(base) ? base_idx + 1 : base_cnt;
    for (size_t nb_idx = 0; nb_idx < nb_cnt; nb_idx++) {
      MODULUS* new_prime_m =
          Get_modulus(Get_vlprime_at(new_base_primes, nb_idx));
      int64_t     new_prime           = Get_mod_val(new_prime_m);
      int64_t     val                 = 1;
      int64_t     hat_val             = 1;
      VALUE_LIST* hat_mod_nb_at_nb    = Alloc_value_list(I64_TYPE, base_end);
      VL_VALUE_AT(hat_mod_nb, nb_idx) = hat_mod_nb_at_nb;
      for (size_t l = 0; l < base_end; ++l) {
        int64_t base_prime = Get_modulus_val(Get_vlprime_at(base_primes, l));
        int64_t temp       = Mod_int64(base_prime, new_prime);
        val                = Mul_int64_with_mod(val, temp, new_prime);
        // calculate hat value, skip index at l
        hat_val = 1;
        for (size_t hat_idx = 0; hat_idx < l; ++hat_idx) {
          int64_t temp1 = Get_modulus_val(Get_vlprime_at(base_primes, hat_idx));
          int64_t temp2 = Mod_int64(temp1, new_prime);
          hat_val       = Mul_int64_with_mod(hat_val, temp2, new_prime);
        }
        for (size_t hat_idx = l + 1; hat_idx < base_end; ++hat_idx) {
          int64_t temp1 = Get_modulus_val(Get_vlprime_at(base_primes, hat_idx));
          int64_t temp2 = Mod_int64(temp1, new_prime);
          hat_val       = Mul_int64_with_mod(hat_val, temp2, new_prime);
        }
        Set_i64_value(hat_mod_nb_at_nb, l, hat_val);
      }
      Set_i64_value(m_mod_nb, nb_idx, val);
      Set_i64_value(m_inv_mod_nb, nb_idx, Mod_inv_prime(val, new_prime_m));
    }
  }
}

void Precompute_qpart(CRT_PRIMES* crt_qpart, CRT_PRIMES* q_crt_primes,
                      size_t num_parts) {
  size_t num_q         = Get_primes_cnt(q_crt_primes);
  size_t num_per_parts = ceil((double)(num_q) / num_parts);
  FMT_ASSERT(num_q > num_per_parts * (num_parts - 1),
             "invalid number of q parts");

  QPART_PRE_COMP* precomp        = Alloc_qpart_precomp();
  VALUE_LIST*     q_part_primes  = Alloc_value_list(VL_PTR_TYPE, num_parts);
  VALUE_LIST*     q_part_modulus = Alloc_value_list(BIGINT_TYPE, num_parts);
  crt_qpart->_u._qpart_precomp   = precomp;
  crt_qpart->_primes             = q_part_primes;
  precomp->_modulus              = q_part_modulus;
  precomp->_per_part_size        = num_per_parts;

  size_t max_bits = 0;
  for (size_t j = 0; j < num_parts; j++) {
    size_t     digit = 0;
    size_t     bits  = 0;
    CRT_PRIME* q_data[num_per_parts];
    BIG_INT*   bi_modulus = Get_bint_value_at(q_part_modulus, j);
    BI_ASSIGN_SI(*bi_modulus, 1);
    for (size_t i = j * num_per_parts; i < (j + 1) * num_per_parts; i++) {
      if (i < num_q) {
        CRT_PRIME* prime = Get_prime_at(q_crt_primes, i);
        q_data[digit]    = prime;
        BI_MUL_UI(*bi_modulus, *bi_modulus, Get_modulus_val(prime));
        digit++;
      }
    }
    bits = BI_SIZE_INBASE(*bi_modulus, 2);
    if (bits > max_bits) {
      max_bits = bits;
    }
    VALUE_LIST* dim2              = Alloc_value_list(PTR_TYPE, digit);
    VL_VALUE_AT(q_part_primes, j) = dim2;
    for (size_t k = 0; k < digit; k++) {
      PTR_VALUE_AT(dim2, k) = (PTR)q_data[k];
    }
  }
  precomp->_num_p = ceil((double)(max_bits) / AUXBITS);
}

void Precompute_qpart_new_base(CRT_PRIMES* qpart, CRT_PRIMES* qpart_compl,
                               CRT_PRIMES* q, CRT_PRIMES* p) {
  size_t          num_parts     = Get_num_parts(qpart);
  size_t          num_per_parts = Get_per_part_size(qpart);
  size_t          num_q         = Get_primes_cnt(q);
  size_t          num_p         = Get_primes_cnt(p);
  VALUE_LIST*     qpart_modulus = Get_qpart_modulus(qpart);
  QPART_PRE_COMP* qpart_precomp = Get_qpart_precomp(qpart);

  // precompute qpartl hat inverse mod qi
  qpart_precomp->_l_hat_inv_modq = Alloc_value_list(VL_PTR_TYPE, num_parts);
  BIG_INT qpart_moduli, qhat, qhat_inv_mod;
  BI_INITS(qpart_moduli, qhat, qhat_inv_mod);
  for (size_t j = 0; j < num_parts; j++) {
    VALUE_LIST* q_part_prime = Get_qpart_prime_at_l1(qpart, j);
    // MODULUS *qi_part = Get_modulus_head(q_part_prime);
    size_t      num_part_qj = LIST_LEN(q_part_prime);
    VALUE_LIST* vl_dim2     = Alloc_value_list(VL_PTR_TYPE, num_part_qj);
    VL_VALUE_AT(qpart_precomp->_l_hat_inv_modq, j) = vl_dim2;
    BI_ASSIGN(qpart_moduli, BIGINT_VALUE_AT(qpart_modulus, j));
    for (size_t l = 0; l < num_part_qj; l++) {
      size_t dim3_len = num_part_qj - l;
      if (l > 0) {
        BI_DIV_UI(
            qpart_moduli, qpart_moduli,
            Get_modulus_val((CRT_PRIME*)PTR_VALUE_AT(q_part_prime, dim3_len)));
      }
      VALUE_LIST* vl_dim3                = Alloc_value_list(I64_TYPE, dim3_len);
      VL_VALUE_AT(vl_dim2, dim3_len - 1) = vl_dim3;
      for (size_t i = 0; i < dim3_len; i++) {
        int64_t qi = Get_modulus_val((CRT_PRIME*)PTR_VALUE_AT(q_part_prime, i));
        BI_DIV_UI(qhat, qpart_moduli, qi);
        Bi_mod_inv(qhat_inv_mod, qhat, qi);
        I64_VALUE_AT(vl_dim3, i) = BI_GET_SI(qhat_inv_mod);
      }
    }
  }

  // precompute qpart complementary partitions
  VALUE_LIST* complement   = Alloc_value_list(VL_PTR_TYPE, num_q);
  qpart_compl->_primes     = complement;
  qpart_compl->_u._precomp = NULL;
  for (int64_t l = num_q - 1; l >= 0; l--) {
    size_t      dim2           = ceil((double)(l + 1) / num_per_parts);
    VALUE_LIST* vl_dim2        = Alloc_value_list(VL_PTR_TYPE, dim2);
    VL_VALUE_AT(complement, l) = vl_dim2;
    for (size_t j = 0; j < dim2; j++) {
      size_t num_part_qj = LIST_LEN(Get_qpart_prime_at_l1(qpart, j));
      if (j == dim2 - 1) {
        num_part_qj = (l + 1) - j * num_per_parts;
      }
      size_t      num_qpart_compl = (l + 1) - num_part_qj + num_p;
      VALUE_LIST* vl_dim3         = Alloc_value_list(PTR_TYPE, num_qpart_compl);
      VL_VALUE_AT(vl_dim2, j)     = vl_dim3;
      for (size_t k = 0; k < num_qpart_compl; k++) {
        if (k < (l + 1) - num_part_qj) {
          size_t cur_digit = k / num_per_parts;
          if (cur_digit >= j) {
            cur_digit++;
          }
          PTR_VALUE_AT(vl_dim3, k) =
              (PTR)Get_qpart_prime_at_l2(qpart, cur_digit, k % num_per_parts);
        } else {
          PTR_VALUE_AT(vl_dim3, k) =
              (PTR)Get_prime_at(p, k - ((l + 1) - num_part_qj));
        }
      }
    }
  }

  // precompute qpartl hat mod pi
  qpart_precomp->_l_hat_modp = Alloc_value_list(VL_PTR_TYPE, num_q);
  for (size_t l = 0; l < num_q; l++) {
    size_t      dim2            = ceil((double)(l + 1) / num_per_parts);
    VALUE_LIST* l_hat_modp_dim2 = Alloc_value_list(VL_PTR_TYPE, dim2);
    VL_VALUE_AT(qpart_precomp->_l_hat_modp, l) = l_hat_modp_dim2;
    for (size_t k = 0; k < dim2; k++) {
      BI_ASSIGN(qpart_moduli, BIGINT_VALUE_AT(qpart_modulus, k));
      VALUE_LIST* q_part_prime = Get_qpart_prime_at_l1(qpart, k);
      // MODULUS *qk_parts_modulus = Get_modulus_head(qk_parts);
      size_t num_part_qk = LIST_LEN(q_part_prime);
      if (k == dim2 - 1) {
        num_part_qk = l + 1 - k * num_per_parts;
        for (size_t idx = num_part_qk; idx < LIST_LEN(q_part_prime); idx++) {
          BI_DIV_UI(
              qpart_moduli, qpart_moduli,
              Get_modulus_val((CRT_PRIME*)PTR_VALUE_AT(q_part_prime, idx)));
        }
      }
      VALUE_LIST* l_hat_modp_dim3 = Alloc_value_list(VL_PTR_TYPE, num_part_qk);
      VL_VALUE_AT(l_hat_modp_dim2, k) = l_hat_modp_dim3;
      VALUE_LIST* comp_basis          = Get_qpart_compl_at(qpart_compl, l, k);
      for (size_t i = 0; i < num_part_qk; i++) {
        BI_DIV_UI(qhat, qpart_moduli,
                  Get_modulus_val((CRT_PRIME*)PTR_VALUE_AT(q_part_prime, i)));
        VALUE_LIST* l_hat_modp_dim4 =
            Alloc_value_list(I64_TYPE, LIST_LEN(comp_basis));
        VL_VALUE_AT(l_hat_modp_dim3, i) = l_hat_modp_dim4;
        for (size_t j = 0; j < LIST_LEN(comp_basis); j++) {
          BI_MOD_UI(qhat_inv_mod, qhat,
                    Get_modulus_val((CRT_PRIME*)PTR_VALUE_AT(comp_basis, j)));
          I64_VALUE_AT(l_hat_modp_dim4, j) = BI_GET_SI(qhat_inv_mod);
        }
      }
    }
  }
  BI_FREES(qpart_moduli, qhat, qhat_inv_mod);
}

void Precompute_crt(CRT_CONTEXT* crt, uint32_t poly_degree, size_t num_parts) {
  CRT_PRIMES* q           = Get_q(crt);
  CRT_PRIMES* p           = Get_p(crt);
  CRT_PRIMES* qpart       = Get_qpart(crt);
  CRT_PRIMES* qpart_compl = Get_qpart_compl(crt);
  Precompute_primes(q, false);
  Precompute_qpart(qpart, q, num_parts);
  Generate_p_primes(p, Get_crt_num_p(crt), AUXBITS, poly_degree, q);
  Precompute_primes(p, false);

  // precomputed values for base switching
  Precompute_new_base(q, p);
  Precompute_new_base(p, q);
  Precompute_qpart_new_base(qpart, qpart_compl, q, p);
}

CRT_CONTEXT* Alloc_crtcontext() {
  CRT_CONTEXT* crt = (CRT_CONTEXT*)malloc(sizeof(CRT_CONTEXT));
  memset(crt, 0, sizeof(CRT_CONTEXT));
  Set_prime_type(Get_q(crt), Q_TYPE);
  Set_prime_type(Get_p(crt), P_TYPE);
  Set_prime_type(Get_qpart(crt), QPART_TYPE);
  Set_prime_type(Get_qpart_compl(crt), QPART_COMP_TYPE);
  return crt;
}

void Init_crtcontext(CRT_CONTEXT* crt, SECURITY_LEVEL level,
                     uint32_t poly_degree, size_t mult_depth,
                     size_t num_parts) {
  // Get default primes at given level and degree
  VALUE_LIST* q_primes = Alloc_value_list(I64_TYPE, mult_depth + 1);
  Get_default_primes(q_primes, level, poly_degree, LIST_LEN(q_primes));
  Set_primes(Get_q(crt), q_primes, poly_degree);

  Precompute_crt(crt, poly_degree, num_parts);

  Free_value_list(q_primes);
}

void Init_crtcontext_with_prime_size(CRT_CONTEXT* crt, SECURITY_LEVEL level,
                                     uint32_t poly_degree, size_t num_primes,
                                     size_t first_mod_size,
                                     size_t scaling_mod_size,
                                     size_t num_parts) {
  FMT_ASSERT(Is_power_of_two(poly_degree), "invalid poly degree");

  Generate_q_primes(Get_q(crt), num_primes, first_mod_size, scaling_mod_size,
                    poly_degree);

  Precompute_crt(crt, poly_degree, num_parts);
}

void Free_crtcontext(CRT_CONTEXT* crt) {
  if (crt == NULL) return;
  Free_crtprimes(Get_q(crt));
  Free_crtprimes(Get_p(crt));
  Free_crtprimes(Get_qpart(crt));
  Free_crtprimes(Get_qpart_compl(crt));
  free(crt);
}

void Transform_to_crt(VALUE_LIST* crt_ret, CRT_PRIMES* crt_primes,
                      BIG_INT value) {
  size_t len = Get_primes_cnt(crt_primes);
  for (size_t i = 0; i < len; i++) {
    CRT_PRIME* prime         = Get_prime_at(crt_primes, i);
    I64_VALUE_AT(crt_ret, i) = Mod_bigint(value, Get_modulus_val(prime));
  }
}

void Transform_to_qpcrt(VALUE_LIST* crt_ret, CRT_CONTEXT* crt, BIG_INT value) {
  CRT_PRIMES* q_primes = Get_q(crt);
  CRT_PRIMES* p_primes = Get_p(crt);
  IS_TRUE(
      LIST_LEN(crt_ret) == Get_primes_cnt(q_primes) + Get_primes_cnt(p_primes),
      "Transform_to_crt: length not match");
  for (size_t i = 0; i < Get_primes_cnt(q_primes); i++) {
    CRT_PRIME* prime         = Get_prime_at(q_primes, i);
    I64_VALUE_AT(crt_ret, i) = Mod_bigint(value, Get_modulus_val(prime));
  }
  for (size_t j = 0; j < Get_primes_cnt(p_primes); j++) {
    CRT_PRIME* prime = Get_prime_at(p_primes, j);
    I64_VALUE_AT(crt_ret, j + Get_primes_cnt(q_primes)) =
        Mod_bigint(value, Get_modulus_val(prime));
  }
}

void Reconstruct(BIG_INT res, CRT_PRIMES* crt_primes, VALUE_LIST* values) {
  size_t     value_len  = LIST_LEN(values);
  CRT_PRIME* prime_head = Get_prime_head(crt_primes);
  IS_TRUE(value_len == Get_primes_cnt(crt_primes), "length not equal");

  // TODO: Remove these big int
  BIG_INT intermed_val, tmp_val, half_mod;
  BI_INITS(intermed_val, half_mod);
  BI_INIT_ASSIGN_SI(tmp_val, 0);
  BI_DIV_UI(half_mod, GET_BIG_M(crt_primes), 2);

  for (size_t i = 0; i < value_len; i++) {
    BI_MUL_UI(intermed_val, BIGINT_VALUE_AT(Get_hat_inv(crt_primes), i),
              I64_VALUE_AT(values, i));
    BI_MOD_UI(intermed_val, intermed_val,
              Get_modulus_val(Get_nth_prime(prime_head, i)));
    BI_MUL(intermed_val, intermed_val, BIGINT_VALUE_AT(Get_hat(crt_primes), i));
    BI_MOD(intermed_val, intermed_val, GET_BIG_M(crt_primes));
    BI_ADD(tmp_val, tmp_val, intermed_val);
    BI_MOD(tmp_val, tmp_val, GET_BIG_M(crt_primes));
    if (BI_CMP(tmp_val, half_mod) > 0) {
      BI_SUB(tmp_val, tmp_val, GET_BIG_M(crt_primes));
    }
  }
  // regular_rep_val may be a negative value
  BI_INIT_ASSIGN(res, tmp_val);
  BI_FREES(intermed_val, tmp_val, half_mod);
}

////////////////////////////////////////////////////
void Print_crtprime(FILE* fp, CRT_PRIME* prime) {
  if (prime == NULL) return;
  fprintf(fp, "\n  * Modulus: \n    ");
  Print_modulus(fp, Get_modulus(prime));
  Print_ntt(fp, Get_ntt(prime));
}

void Free_crtprime(CRT_PRIME* prime) {
  if (prime == NULL) return;
  Free_ntt_members(Get_ntt(prime));
  // DO NOT FREE itself, let Owner CRT_PRIMES frees it
}

////////////////////////////////////////////////////
void Print_precompute(FILE* fp, PRE_COMP* precomp) {
  if (precomp == NULL) return;
  if (precomp->_m_mod_nb) {
    fprintf(fp, "\n  * M mod nb: \n    ");
    Print_value_list(fp, precomp->_m_mod_nb);
  }
  if (precomp->_m_inv_mod_nb) {
    fprintf(fp, "\n  * M_inv mod nb: \n    ");
    Print_value_list(fp, precomp->_m_inv_mod_nb);
  }
  if (precomp->_hat) {
    fprintf(fp, "\n  * hat: \n    ");
    Print_value_list(fp, precomp->_hat);
  }
  if (precomp->_hat_mod_nb) {
    fprintf(fp, "\n  * hat mod nb: \n    ");
    Print_value_list(fp, precomp->_hat_mod_nb);
  }
  if (precomp->_hat_inv) {
    fprintf(fp, "\n  * hat inv:\n    ");
    Print_value_list(fp, precomp->_hat_inv);
  }
  if (precomp->_hat_inv_mod_self) {
    fprintf(fp, "\n  * hat inv mod self: \n    ");
    Print_value_list(fp, precomp->_hat_inv_mod_self);
  }
  if (precomp->_hat_inv_mod_self_prec) {
    fprintf(fp, "\n  * hat inv mod self prec for barret: \n    ");
    Print_value_list(fp, precomp->_hat_inv_mod_self_prec);
  }
  if (precomp->_ql_div2_mod_qi) {
    fprintf(fp, "\n  * ql div2 mod qi: \n    ");
    Print_value_list(fp, precomp->_ql_div2_mod_qi);
  }
  if (precomp->_ql_inv_mod_qi) {
    fprintf(fp, "\n  * ql inv mod qi: \n    ");
    Print_value_list(fp, precomp->_ql_inv_mod_qi);
  }
  if (precomp->_ql_inv_mod_qi_prec) {
    fprintf(fp, "\n  * ql inv mod qi prec: \n    ");
    Print_value_list(fp, precomp->_ql_inv_mod_qi_prec);
  }
}

void Free_precompute(PRE_COMP* precomp) {
  if (precomp == NULL) return;
  if (precomp->_m_mod_nb) {
    Free_value_list(precomp->_m_mod_nb);
    precomp->_m_mod_nb = NULL;
  }
  if (precomp->_m_inv_mod_nb) {
    Free_value_list(precomp->_m_inv_mod_nb);
    precomp->_m_inv_mod_nb = NULL;
  }
  if (precomp->_hat) {
    Free_value_list(precomp->_hat);
    precomp->_hat = NULL;
  }
  if (precomp->_hat_mod_nb) {
    Free_value_list(precomp->_hat_mod_nb);
    precomp->_hat_mod_nb = NULL;
  }
  if (precomp->_hat_inv) {
    Free_value_list(precomp->_hat_inv);
    precomp->_hat_inv = NULL;
  }
  if (precomp->_hat_inv_mod_self) {
    Free_value_list(precomp->_hat_inv_mod_self);
    precomp->_hat_inv_mod_self = NULL;
  }
  if (precomp->_hat_inv_mod_self_prec) {
    Free_value_list(precomp->_hat_inv_mod_self_prec);
    precomp->_hat_inv_mod_self_prec = NULL;
  }
  if (precomp->_ql_div2_mod_qi) {
    Free_value_list(precomp->_ql_div2_mod_qi);
    precomp->_ql_div2_mod_qi = NULL;
  }
  if (precomp->_ql_inv_mod_qi) {
    Free_value_list(precomp->_ql_inv_mod_qi);
    precomp->_ql_inv_mod_qi = NULL;
  }
  if (precomp->_ql_inv_mod_qi_prec) {
    Free_value_list(precomp->_ql_inv_mod_qi_prec);
    precomp->_ql_inv_mod_qi_prec = NULL;
  }
  if (precomp->_ql_ql_inv_mod_ql_div_ql_mod_qi) {
    Free_value_list(precomp->_ql_ql_inv_mod_ql_div_ql_mod_qi);
    precomp->_ql_ql_inv_mod_ql_div_ql_mod_qi = NULL;
  }
  if (precomp->_ql_ql_inv_mod_ql_div_ql_mod_qi_prec) {
    Free_value_list(precomp->_ql_ql_inv_mod_ql_div_ql_mod_qi_prec);
    precomp->_ql_ql_inv_mod_ql_div_ql_mod_qi_prec = NULL;
  }
  BI_FREES(precomp->_modulus);
  free(precomp);
}

////////////////////////////////////////////////////
void Print_qpart_precompute(FILE* fp, QPART_PRE_COMP* precomp) {
  if (precomp == NULL) return;

  if (precomp->_l_hat_inv_modq) {
    fprintf(fp, "\n  * leveled hat inverse modq: \n    ");
    Print_value_list(fp, precomp->_l_hat_inv_modq);
  }
  if (precomp->_l_hat_modp) {
    fprintf(fp, "\n  * leveled hat modp: \n    ");
    Print_value_list(fp, precomp->_l_hat_modp);
  }
  if (precomp->_modulus) {
    fprintf(fp, "\n  * big modulus: \n    ");
    Print_value_list(fp, precomp->_modulus);
  }
}

void Free_qpart_precompute(QPART_PRE_COMP* precomp) {
  if (precomp == NULL) return;
  if (precomp->_modulus) {
    Free_value_list(precomp->_modulus);
    precomp->_modulus = NULL;
  }
  if (precomp->_l_hat_inv_modq) {
    Free_value_list(precomp->_l_hat_inv_modq);
    precomp->_l_hat_inv_modq = NULL;
  }
  if (precomp->_l_hat_modp) {
    Free_value_list(precomp->_l_hat_modp);
    precomp->_l_hat_modp = NULL;
  }
  free(precomp);
}

////////////////////////////////////////////////////

void Copy_vl_crtprime(VL_CRTPRIME* res, VL_CRTPRIME* input, size_t cnt) {
  FMT_ASSERT(LIST_LEN(res) >= cnt && LIST_LEN(input) >= cnt,
             "copy size outof bond");
  for (size_t idx = 0; idx < cnt; idx++) {
    PTR_VALUE_AT(res, idx) = PTR_VALUE_AT(input, idx);
  }
}

void Print_vl_crtprime(FILE* fp, VL_CRTPRIME* crt_primes) {
  IS_TRUE(crt_primes && LIST_TYPE(crt_primes) == PTR_TYPE,
          "list is not PTR_TYPE");
  fprintf(fp, "\n    {");
  for (size_t idx = 0; idx < LIST_LEN(crt_primes); idx++) {
    CRT_PRIME* prime = (CRT_PRIME*)PTR_VALUE_AT(crt_primes, idx);
    Print_crtprime(fp, prime);
  }
  fprintf(fp, "\n    }");
}
void Print_crtprimes(FILE* fp, CRT_PRIMES* crt_primes) {
  if (Is_q(crt_primes) || Is_p(crt_primes)) {
    double    mod;
    PRE_COMP* precomp = Get_precomp(crt_primes);
    if (precomp) {
      BI_LOG2(mod, GET_BIG_M(crt_primes));
      fprintf(fp, "  * log2(modulus) = %f, ", mod);
      BI_FPRINTF(fp, "modulus = %Zd\n", GET_BIG_M(crt_primes));
    }
  }

  if (Is_q(crt_primes))
    fprintf(fp, "  * Q primes: ");
  else if (Is_p(crt_primes))
    fprintf(fp, "  * P primes: ");
  else if (Is_qpart(crt_primes))
    fprintf(fp, " * QPart primes");
  else if (Is_qpart_compl(crt_primes))
    fprintf(fp, " * QPart Complement");

  VALUE_LIST* primes = Get_primes(crt_primes);
  if (primes) {
    if (Is_q(crt_primes) || Is_p(crt_primes)) {
      Print_vl_crtprime(fp, crt_primes->_primes);
    } else if (Is_qpart(crt_primes)) {
      for (size_t idx = 0; idx < LIST_LEN(crt_primes->_primes); idx++) {
        VL_CRTPRIME* dim2 = Get_qpart_prime_at_l1(crt_primes, idx);
        fprintf(fp, "\n  Part%ld:", idx);
        Print_vl_crtprime(fp, dim2);
      }
    } else if (Is_qpart_compl(crt_primes)) {
      for (size_t idx = 0; idx < LIST_LEN(crt_primes->_primes); idx++) {
        VL_VL_CRTPRIME* dim2 = VL_VALUE_AT(crt_primes->_primes, idx);
        for (size_t idx2 = 0; idx2 < LIST_LEN(dim2); idx2++) {
          fprintf(fp, "\n  Part[%ld][%ld]:", idx, idx2);
          VL_CRTPRIME* dim3 = VL_VALUE_AT(dim2, idx2);
          Print_vl_crtprime(fp, dim3);
        }
      }
    } else {
      IS_TRUE(FALSE, "invalid type");
    }
  }

  if (Is_qpart(crt_primes)) {
    Print_qpart_precompute(fp, crt_primes->_u._qpart_precomp);
  } else {
    Print_precompute(fp, crt_primes->_u._precomp);
  }
}

void Free_crtprimes(CRT_PRIMES* crt_primes) {
  if (crt_primes == NULL) return;
  if (crt_primes->_primes) {
    if (Is_q(crt_primes) || Is_p(crt_primes)) {
      free(PTR_VALUE_AT(Get_mod_val_list(crt_primes), 0)); // modulus
      Free_value_list(Get_mod_val_list(crt_primes));
      for (size_t idx = 0; idx < LIST_LEN(crt_primes->_primes); idx++) {
        CRT_PRIME* prime = Get_prime_at(crt_primes, idx);
        Free_crtprime(prime);
      }
      CRT_PRIME* head = Get_prime_head(crt_primes);
      free(head);
    }
    Free_value_list(crt_primes->_primes);
  }
  if (Is_qpart(crt_primes)) {
    Free_qpart_precompute(crt_primes->_u._qpart_precomp);
  } else {
    Free_precompute(crt_primes->_u._precomp);
  }
}

void Print_crt(FILE* fp, CRT_CONTEXT* crt) {
  fprintf(fp, "---------- CRT Dump ----------\n");

  fprintf(fp, "CRT_PRIMES Q:\n");
  Print_crtprimes(fp, Get_q(crt));
  fprintf(fp, "\nCRT_PRIMES P:\n");
  Print_crtprimes(fp, Get_p(crt));
  fprintf(fp, "\nCRT_PRIMES QPart:");
  Print_crtprimes(fp, Get_qpart(crt));
  fprintf(fp, "\nCRT_PRIMES QPartCompl :");
  Print_crtprimes(fp, Get_qpart_compl(crt));

  fprintf(fp, "\n---------- End of CRT Dump ----------\n");
}
