//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "ckks/decryptor.h"

#include "ckks/ciphertext.h"
#include "ckks/param.h"
#include "ckks/plaintext.h"
#include "ckks/secret_key.h"
#include "common/trace.h"
#include "poly/rns_poly.h"
#include "util/crt.h"

void Decrypt(PLAINTEXT* res, CKKS_DECRYPTOR* decryptor, CIPHERTEXT* ciph,
             POLYNOMIAL* c2) {
  IS_TRACE("decrypt ciphertext:");
  IS_TRACE_CMD(Print_ciph(Get_trace_file(), ciph));
  FMT_ASSERT(Get_ciph_prime_p_cnt(ciph) == 0,
             "Decrypt: ciph should not be extended");

  Init_plaintext(res, Get_ciph_degree(ciph), Get_ciph_slots(ciph),
                 Get_ciph_level(ciph), Get_ciph_prime_p_cnt(ciph),
                 Get_ciph_sfactor(ciph), Get_ciph_sf_degree(ciph));
  POLYNOMIAL* res_poly = Get_plain_poly(res);
  POLYNOMIAL* c0       = Get_c0(ciph);
  POLYNOMIAL* c1       = Get_c1(ciph);
  POLYNOMIAL* ntt_sk   = Get_ntt_sk(decryptor->_secret_key);

  IS_TRUE(Is_ntt(ntt_sk), "sk is not ntt");
  size_t c1_saved_level     = Save_poly_level(c1, Poly_level(res_poly));
  size_t ntt_sk_saved_level = Save_poly_level(ntt_sk, Poly_level(res_poly));
  Mul_poly(res_poly, c1, ntt_sk);
  Restore_poly_level(c1, c1_saved_level);
  Restore_poly_level(ntt_sk, ntt_sk_saved_level);
  IS_TRACE("decrypt c1 * secret:");
  IS_TRACE_CMD(Print_poly(T_FILE, res_poly));
  IS_TRACE(S_BAR);

  size_t c0_saved_level = Save_poly_level(c0, Poly_level(res_poly));
  Add_poly(res_poly, c0, res_poly);
  Restore_poly_level(c0, c0_saved_level);
  IS_TRACE("decrypt c0 + message:");
  IS_TRACE_CMD(Print_poly(T_FILE, res_poly));
  IS_TRACE(S_BAR);
  if (c2) {
    POLYNOMIAL s2mulc2;
    Alloc_poly_data(&s2mulc2, Get_rdgree(ntt_sk), Poly_level(ntt_sk),
                    Num_p(ntt_sk));
    Mul_poly(&s2mulc2, ntt_sk, ntt_sk);
    Mul_poly(&s2mulc2, c2, &s2mulc2);
    Add_poly(res_poly, res_poly, &s2mulc2);
    IS_TRACE("decrypt c2:");
    IS_TRACE_CMD(Print_poly(T_FILE, res_poly));
    IS_TRACE(S_BAR);
    Free_poly_data(&s2mulc2);
  }

  IS_TRACE("decrypted:");
  IS_TRACE_CMD(Print_plain(T_FILE, res));
  IS_TRACE(S_BAR);
}
