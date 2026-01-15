//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "hal/creg.h"

#include "ckks/param.h"
#include "context/ckks_context.h"
#include "util/crt.h"
#include "util/number_theory.h"

#define DSA_CR_AUTOU_TO_LONG_LONG(G, K, mode) \
  ((long long)mode << 34) | ((long long)K << 17) | ((long long)G)

//! @brief Calculate mu and k (used in MMAC) for the given q
//! @param q: modulus (input)
//! @param mu: mu value, mu = (1<<(35+t))/q     (output)
//! @param k: k value, k is the bit-width of q  (output)
void Calc_mu(uint64_t q, uint64_t* mu, uint8_t* k) {
  uint8_t  t      = 0;
  uint64_t temp_q = q;
  while (temp_q > 0) {
    temp_q >>= 1;
    t++;
  }
  *k  = t - 6;
  *mu = ((__uint128_t)1 << (35 + t)) / (__uint128_t)(q);
}

//! @brief Get dsa_creg mod1~3 for the given q
//! @param q: modulus (input)
//! @param cr_mod0~2: DSA_CR_MOD mod1~3 (output)
void Mod3s_reg(uint64_t q, int64_t* cr_mod0, int64_t* cr_mod1,
               int64_t* cr_mod2) {
  uint64_t mu;
  uint8_t  k;
  Calc_mu(q, &mu, &k);
  *cr_mod0 = (int64_t)(q);
  *cr_mod1 = ((int64_t)k << 37) | (int64_t)mu;
  *cr_mod2 = 0;
}

void Gen_mod3s_regs(int64_t* qmuk_data, size_t size) {
  CRT_CONTEXT* crt      = Get_crt_context();
  VL_CRTPRIME* q_primes = Get_q_primes(crt);
  VL_CRTPRIME* p_primes = Get_p_primes(crt);
  uint64_t     qmuk_idx = 0;

  // calculate for q primes
  FOR_ALL_ELEM(q_primes, idx) {
    int64_t q = Get_modulus_val(Get_vlprime_at(q_primes, idx));
    int64_t creg0, creg1, creg2;
    Mod3s_reg((uint64_t)q, &creg0, &creg1, &creg2);
    qmuk_data[qmuk_idx++] = creg0;
    qmuk_data[qmuk_idx++] = creg1;
    qmuk_data[qmuk_idx++] = creg2;
  }

  // calculate for p primes
  FOR_ALL_ELEM(p_primes, idx) {
    int64_t p = Get_modulus_val(Get_vlprime_at(p_primes, idx));
    int64_t creg0, creg1, creg2;
    Mod3s_reg((uint64_t)p, &creg0, &creg1, &creg2);
    qmuk_data[qmuk_idx++] = creg0;
    qmuk_data[qmuk_idx++] = creg1;
    qmuk_data[qmuk_idx++] = creg2;
  }
  FMT_ASSERT(qmuk_idx == size, "qmuk size mismatch")
}

int64_t Gen_autou_creg(int32_t rot_idx, uint64_t poly_degree, bool mode) {
  uint64_t m = poly_degree << 1;
  MODULUS  two_n_modulus;
  Init_modulus(&two_n_modulus, m);
  uint64_t gk = Find_automorphism_index(rot_idx, &two_n_modulus);
  uint64_t g  = Mod_inv((uint64_t)gk, &two_n_modulus);
  uint64_t k  = mode ? 0 : (g >> 1);
  return DSA_CR_AUTOU_TO_LONG_LONG(g, k, mode);
}

MODULUS* Ldmod(uint64_t* qmuk_data, size_t idx) {
  size_t   q_cnt = Get_q_cnt();
  MODULUS* mod = idx < q_cnt ? Q_modulus() + idx : P_modulus() + (idx - q_cnt);
  FMT_ASSERT(qmuk_data[0] == Get_mod_val(mod), "mod_val mismatch");
  return mod;
}