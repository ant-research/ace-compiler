//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "util/ckks_evaluator.h"

#include "common/trace.h"
#include "util/ciphertext.h"
#include "util/ckks_bootstrap_context.h"
#include "util/ckks_encoder.h"
#include "util/ckks_parameters.h"
#include "util/crt.h"
#include "util/matrix_operations.h"
#include "util/plaintext.h"
#include "util/public_key.h"
#include "util/switch_key.h"

CKKS_EVALUATOR* Alloc_ckks_evaluator(CKKS_PARAMETER*     params,
                                     CKKS_ENCODER*       encoder,
                                     CKKS_DECRYPTOR*     decryptor,
                                     CKKS_KEY_GENERATOR* keygen) {
  CKKS_EVALUATOR* eval = (CKKS_EVALUATOR*)malloc(sizeof(CKKS_EVALUATOR));
  eval->_params        = params;
  eval->_encoder       = encoder;
  eval->_decryptor     = decryptor;
  eval->_keygen        = keygen;
  eval->_bts_ctx       = Alloc_ckks_bts_ctx(eval);
  return eval;
}

void Free_ckks_evaluator(CKKS_EVALUATOR* eval) {
  if (eval == NULL) return;
  if (eval->_bts_ctx) {
    Free_ckks_bts_ctx(eval->_bts_ctx);
    eval->_bts_ctx = NULL;
  }
  free(eval);
  eval = NULL;
}

CIPHERTEXT* Add_ciphertext(CIPHERTEXT* res, CIPHERTEXT* ciph1,
                           CIPHERTEXT* ciph2, CKKS_EVALUATOR* eval) {
  IS_TRUE(ciph1, "ciph1 is not Ciphertext");
  IS_TRUE(ciph2, "ciph2 is not Ciphertext");
  IS_TRUE(ciph1->_scaling_factor == ciph2->_scaling_factor,
          "Scaling factors are not equal.");
  size_t      orig_level;
  CIPHERTEXT* ciph         = Adjust_level(ciph1, ciph2, TRUE, &orig_level);
  CIPHERTEXT* smaller_ciph = ciph1 == ciph ? ciph2 : ciph1;
  if (res != ciph1 && res != ciph2) {
    Init_ciphertext_from_ciph(res, smaller_ciph, smaller_ciph->_scaling_factor,
                              smaller_ciph->_sf_degree);
  }

  POLYNOMIAL*  c0  = Get_c0(res);
  POLYNOMIAL*  c1  = Get_c1(res);
  CRT_CONTEXT* crt = eval->_params->_crt_context;
  IS_TRUE(c0->_num_primes_p == Get_c0(ciph1)->_num_primes_p &&
              c0->_num_primes_p == Get_c0(ciph2)->_num_primes_p,
          "unmatched mod");
  VALUE_LIST* p_primes = c0->_num_primes_p ? Get_p_primes(crt) : NULL;
  Add_poly(c0, Get_c0(ciph1), Get_c0(ciph2), crt, p_primes);
  Add_poly(c1, Get_c1(ciph1), Get_c1(ciph2), crt, p_primes);

  if (ciph != res) {
    Restore_level(ciph, orig_level);
  }
  return res;
}

CIPHERTEXT* Sub_ciphertext(CIPHERTEXT* res, CIPHERTEXT* ciph1,
                           CIPHERTEXT* ciph2, CKKS_EVALUATOR* eval) {
  IS_TRUE(ciph1, "ciph1 is not Ciphertext");
  IS_TRUE(ciph2, "ciph2 is not Ciphertext");
  IS_TRUE(ciph1->_scaling_factor == ciph2->_scaling_factor,
          "Scaling factors are not equal.");
  size_t      orig_level;
  CIPHERTEXT* ciph         = Adjust_level(ciph1, ciph2, TRUE, &orig_level);
  CIPHERTEXT* smaller_ciph = ciph1 == ciph ? ciph2 : ciph1;
  if (res != ciph1 && res != ciph2) {
    Init_ciphertext_from_ciph(res, smaller_ciph, smaller_ciph->_scaling_factor,
                              smaller_ciph->_sf_degree);
  }
  POLYNOMIAL*  c0  = Get_c0(res);
  POLYNOMIAL*  c1  = Get_c1(res);
  CRT_CONTEXT* crt = eval->_params->_crt_context;
  Sub_poly(c0, Get_c0(ciph1), Get_c0(ciph2), crt, NULL);
  Sub_poly(c1, Get_c1(ciph1), Get_c1(ciph2), crt, NULL);

  if (ciph != res) {
    Restore_level(ciph, orig_level);
  }
  return res;
}

CIPHERTEXT* Add_plaintext(CIPHERTEXT* res, CIPHERTEXT* ciph, PLAINTEXT* plain,
                          CKKS_EVALUATOR* eval) {
  IS_TRUE(ciph, "ciph is not Ciphertext");
  IS_TRUE(plain, "plain is not Plaintext");
  IS_TRUE(ciph->_scaling_factor == plain->_scaling_factor,
          "Scaling factors are not equal.");
  Init_ciphertext_from_ciph(res, ciph, ciph->_scaling_factor, ciph->_sf_degree);
  POLYNOMIAL* c0 = Get_c0(res);
  POLYNOMIAL* c1 = Get_c1(res);

  Add_poly(c0, Get_c0(ciph), Get_plain_poly(plain), eval->_params->_crt_context,
           NULL);
  Copy_polynomial(c1, Get_c1(ciph));
  return res;
}

CIPHERTEXT* Add_const(CIPHERTEXT* res, CIPHERTEXT* ciph, double const_val,
                      CKKS_EVALUATOR* eval) {
  PLAINTEXT* plain = Alloc_plaintext();
  Encode_val_at_level(plain, eval->_encoder, const_val, Get_ciph_level(ciph),
                      Get_ciph_sf_degree(ciph));
  Add_plaintext(res, ciph, plain, eval);

  IS_TRUE(Get_ciph_sf_degree(res) == Get_ciph_sf_degree(ciph),
          "scaling factor not matched");

  Free_plaintext(plain);
  return res;
}

CIPHERTEXT3* Mul_ciphertext3(CIPHERTEXT3* res, CIPHERTEXT* ciph1,
                             CIPHERTEXT* ciph2, CKKS_EVALUATOR* eval) {
  IS_TRUE(ciph1, "ciph1 is not Ciphertext");
  IS_TRUE(ciph2, "ciph2 is not Ciphertext");

  CRT_CONTEXT* crt = eval->_params->_crt_context;

  // adjust levels
  size_t      orig_level;
  CIPHERTEXT* adj_ciph     = Adjust_level(ciph1, ciph2, TRUE, &orig_level);
  CIPHERTEXT* smaller_ciph = ciph1 == adj_ciph ? ciph2 : ciph1;
  size_t      degree       = Get_ciph_degree(smaller_ciph);
  size_t      prime_cnt    = Get_ciph_prime_cnt(smaller_ciph);
  // check if scaling factor exceeds the bounds of coefficient modulus
  double new_factor = ciph1->_scaling_factor * ciph2->_scaling_factor;
  IS_TRUE(((uint32_t)log2(new_factor) <
           Get_coeff_bit_count(prime_cnt, eval->_params)),
          "invalid scaling factor for Mul_ciphertext3");
  Init_ciphertext3_from_ciph(res, smaller_ciph, new_factor,
                             ciph1->_sf_degree + ciph2->_sf_degree);
  POLYNOMIAL* c0 = Get_ciph3_c0(res);
  POLYNOMIAL* c1 = Get_ciph3_c1(res);
  POLYNOMIAL* c2 = Get_ciph3_c2(res);
  POLYNOMIAL  ctemp;
  Alloc_poly_data(&ctemp, degree, prime_cnt, 0);
  Multiply_poly_fast(c0, Get_c0(ciph1), Get_c0(ciph2), crt, NULL);
  Multiply_poly_fast(c1, Get_c0(ciph1), Get_c1(ciph2), crt, NULL);
  Multiply_poly_fast(&ctemp, Get_c1(ciph1), Get_c0(ciph2), crt, NULL);
  Add_poly(c1, c1, &ctemp, crt, NULL);
  Multiply_poly_fast(c2, Get_c1(ciph1), Get_c1(ciph2), crt, NULL);

  // recover orignal level
  Restore_level(adj_ciph, orig_level);
  Free_poly_data(&ctemp);
  return res;
}

CIPHERTEXT* Mul_ciphertext(CIPHERTEXT* res, CIPHERTEXT* ciph1,
                           CIPHERTEXT* ciph2, SWITCH_KEY* relin_key,
                           CKKS_EVALUATOR* eval) {
  CIPHERTEXT3 ciph3;
  memset(&ciph3, 0, sizeof(ciph3));
  Mul_ciphertext3(&ciph3, ciph1, ciph2, eval);
  // relinearize 3-dimensional back down to 2 dimensions
  Relinearize_ciph3(res, relin_key, Get_ciph3_c0(&ciph3), Get_ciph3_c1(&ciph3),
                    Get_ciph3_c2(&ciph3), Get_ciph3_sfactor(&ciph3),
                    Get_ciph3_sf_degree(&ciph3), Get_ciph3_slots(&ciph3), eval);
  Free_poly_data(&ciph3._c0_poly);
  Free_poly_data(&ciph3._c1_poly);
  Free_poly_data(&ciph3._c2_poly);
  return res;
}

CIPHERTEXT* Mul_plaintext(CIPHERTEXT* res, CIPHERTEXT* ciph, PLAINTEXT* plain,
                          CKKS_EVALUATOR* eval) {
  IS_TRUE(ciph, "ciph is not Ciphertext");
  IS_TRUE(plain, "plain is not Plaintext");
  // multiply computation is too complex, forbidden the result same
  // as operand to avoid any read/write conflict
  // IS_TRUE(res != ciph, "result and operand are the same");
  // check if scaling factor exceeds the bounds of coefficient modulus
  double new_factor = ciph->_scaling_factor * plain->_scaling_factor;
  IS_TRUE(((uint32_t)log2(new_factor) <
           Get_coeff_bit_count(Get_ciph_prime_cnt(ciph), eval->_params)),
          "invalid scaling factor for Mul_plaintext");
  Init_ciphertext_from_ciph(res, ciph, new_factor,
                            ciph->_sf_degree + plain->_sf_degree);
  POLYNOMIAL*  c0  = Get_c0(res);
  POLYNOMIAL*  c1  = Get_c1(res);
  CRT_CONTEXT* crt = eval->_params->_crt_context;

  IS_TRUE(c0->_num_primes_p == Get_c0(ciph)->_num_primes_p &&
              c0->_num_primes_p == Get_plain_poly(plain)->_num_primes_p,
          "unmatched mod");
  VALUE_LIST* p_primes = c0->_num_primes_p ? Get_p_primes(crt) : NULL;
  Multiply_poly_fast(c0, Get_c0(ciph), Get_plain_poly(plain), crt, p_primes);
  Multiply_poly_fast(c1, Get_c1(ciph), Get_plain_poly(plain), crt, p_primes);

  return res;
}

CIPHERTEXT* Mul_const(CIPHERTEXT* res, CIPHERTEXT* ciph, double const_val,
                      CKKS_EVALUATOR* eval) {
  PLAINTEXT* plain = Alloc_plaintext();
  Encode_val_at_level(plain, eval->_encoder, const_val, Get_ciph_level(ciph),
                      1);
  Mul_plaintext(res, ciph, plain, eval);

  Free_plaintext(plain);
  return res;
}

CIPHERTEXT* Mul_integer(CIPHERTEXT* res, CIPHERTEXT* ciph, uint32_t power,
                        CKKS_EVALUATOR* eval) {
  Init_ciphertext_from_ciph(res, ciph, ciph->_scaling_factor, ciph->_sf_degree);
  IS_TRUE(Get_c0(res)->_num_primes_p == Get_c0(ciph)->_num_primes_p,
          "unmatched mod");
  CRT_CONTEXT* crt      = eval->_params->_crt_context;
  VALUE_LIST*  q_primes = Get_q_primes(crt);
  VALUE_LIST* p_primes = Get_c0(ciph)->_num_primes_p ? Get_p_primes(crt) : NULL;
  Scalar_integer_multiply_poly(Get_c0(res), Get_c0(ciph), power, q_primes,
                               p_primes);
  Scalar_integer_multiply_poly(Get_c1(res), Get_c1(ciph), power, q_primes,
                               p_primes);
  return res;
}

CIPHERTEXT* Mul_by_monomial(CIPHERTEXT* res, CIPHERTEXT* ciph, uint32_t power,
                            CKKS_EVALUATOR* eval) {
  Init_ciphertext_from_ciph(res, ciph, ciph->_scaling_factor, ciph->_sf_degree);
  IS_TRUE(Get_c0(res)->_num_primes_p == Get_c0(ciph)->_num_primes_p,
          "unmatched mod");
  CRT_CONTEXT* crt      = eval->_params->_crt_context;
  uint32_t     degree   = Get_c0(ciph)->_ring_degree;
  VALUE_LIST*  q_primes = Get_q_primes(crt);
  VALUE_LIST* p_primes = Get_c0(ciph)->_num_primes_p ? Get_p_primes(crt) : NULL;
  POLYNOMIAL  monomial;
  Alloc_poly_data(&monomial, Get_c0(ciph)->_ring_degree,
                  Get_c0(ciph)->_num_primes, 0);
  uint32_t power_reduced = power % (2 * degree);
  uint32_t index         = power % degree;

  MODULUS* modulus = Get_modulus_head(q_primes);
  for (size_t idx = 0; idx < (&monomial)->_num_primes; idx++) {
    int64_t val = power_reduced < degree ? 1 : Get_mod_val(modulus) - 1;
    Set_coeff_at(&monomial, val, index + idx * degree);
    modulus = Get_next_modulus(modulus);
  }

  Multiply_poly_fast(Get_c0(res), Get_c0(ciph), &monomial, crt, p_primes);
  Multiply_poly_fast(Get_c1(res), Get_c1(ciph), &monomial, crt, p_primes);

  Free_poly_data(&monomial);
  return res;
}

CIPHERTEXT* Relinearize_ciph3(CIPHERTEXT* res, SWITCH_KEY* relin_key,
                              POLYNOMIAL* c0, POLYNOMIAL* c1, POLYNOMIAL* c2,
                              double new_scaling_factor, uint32_t new_sf_degree,
                              uint32_t slots, CKKS_EVALUATOR* eval) {
  CRT_CONTEXT* crt    = eval->_params->_crt_context;
  POLYNOMIAL*  res_c0 = Get_c0(res);
  POLYNOMIAL*  res_c1 = Get_c1(res);

  Init_ciphertext_from_poly(res, c2, new_scaling_factor, new_sf_degree, slots);

  VALUE_LIST* precomputed = Switch_key_precompute(c2, crt);
  Fast_switch_key(res, relin_key, eval, precomputed, Is_ntt(c0));

  Add_poly(res_c0, res_c0, c0, crt, NULL);
  Add_poly(res_c1, res_c1, c1, crt, NULL);

  Free_poly_list(precomputed);
  return res;
}

CIPHERTEXT* Relinearize_ciph3_ext(CIPHERTEXT* res, CIPHERTEXT3* ciph3,
                                  SWITCH_KEY* relin_key, CKKS_EVALUATOR* eval) {
  CRT_CONTEXT* crt            = eval->_params->_crt_context;
  CRT_PRIMES*  q_primes       = Get_q(crt);
  CRT_PRIMES*  p_primes       = Get_p(crt);
  POLYNOMIAL*  res_c0         = Get_c0(res);
  POLYNOMIAL*  res_c1         = Get_c1(res);
  POLYNOMIAL*  c0             = Get_ciph3_c0(ciph3);
  POLYNOMIAL*  c1             = Get_ciph3_c1(ciph3);
  POLYNOMIAL*  c2             = Get_ciph3_c2(ciph3);
  uint32_t     sf_degree      = Get_ciph3_sf_degree(ciph3);
  uint32_t     slots          = Get_ciph3_slots(ciph3);
  double       scaling_factor = Get_ciph3_sfactor(ciph3);

  VALUE_LIST* precomputed = Switch_key_precompute(c2, crt);
  size_t size_ql = ((POLYNOMIAL*)Get_ptr_value_at(precomputed, 0))->_num_primes;
  size_t size_p  = Get_primes_cnt(p_primes);

  Init_ciphertext(res, Get_rdgree(c0), size_ql, size_p, scaling_factor,
                  sf_degree, slots);
  Set_is_ntt(Get_c0(res), TRUE);
  Set_is_ntt(Get_c1(res), TRUE);
  Fast_switch_key_ext(res, relin_key, eval, precomputed, Is_ntt(c0));

  POLYNOMIAL poly_ext;
  VL_I64*    pmodq = Get_pmodq(p_primes);
  Alloc_poly_data(&poly_ext, c0->_ring_degree, size_ql, 0);
  Scalars_integer_multiply_poly(&poly_ext, c0, pmodq, q_primes, NULL);
  Add_poly(res_c0, res_c0, &poly_ext, crt, NULL);

  Scalars_integer_multiply_poly(&poly_ext, c1, pmodq, q_primes, NULL);
  Add_poly(res_c1, res_c1, &poly_ext, crt, NULL);

  Free_poly_data(&poly_ext);
  Free_poly_list(precomputed);
  return res;
}

CIPHERTEXT* Rescale_ciphertext(CIPHERTEXT* res, CIPHERTEXT* ciph,
                               CKKS_EVALUATOR* eval) {
  size_t level = Get_ciph_level(ciph);
  IS_TRUE(level > 1,
          "rescale: multiply level is not big enought for more operation, \
          try to use larger depth");
  double old_factor = ciph->_scaling_factor;
  IS_TRUE(
      ((uint32_t)log2(old_factor) < Get_coeff_bit_count(level, eval->_params)),
      "rescale: scale of input ciph is not large enough");
  double new_factor = old_factor / eval->_params->_scaling_factor;
  IS_TRUE(new_factor, "rescale: new factor is null");

  CRT_CONTEXT* crt = eval->_params->_crt_context;
  Init_ciphertext_from_ciph(res, ciph, new_factor, ciph->_sf_degree - 1);

  Rescale_poly(Get_c0(res), Get_c0(ciph), crt);
  Rescale_poly(Get_c1(res), Get_c1(ciph), crt);

  Modswitch_ciphertext(res, eval);
  return res;
}

CIPHERTEXT* Upscale_ciphertext(CIPHERTEXT* res, CIPHERTEXT* ciph,
                               uint32_t mod_size, CKKS_EVALUATOR* eval) {
  PLAINTEXT* plain     = Alloc_plaintext();
  double     up_scale  = pow(2.0, mod_size);
  double     new_scale = Get_ciph_sfactor(ciph) * up_scale;
  IS_TRUE(((uint32_t)log2(new_scale) <
           Get_coeff_bit_count(Get_ciph_prime_cnt(ciph), eval->_params)),
          "Upscale: mod_size is two high");
  Encode_val_at_level_with_scale(plain, eval->_encoder, 1.0,
                                 Get_ciph_level(ciph), up_scale);
  Mul_plaintext(res, ciph, plain, eval);
  Free_plaintext(plain);
}

CIPHERTEXT* Downscale_ciphertext(CIPHERTEXT* res, CIPHERTEXT* ciph,
                                 uint32_t waterline, CKKS_EVALUATOR* eval) {
  IS_TRUE(
      Get_ciph_prime_cnt(ciph) > 1,
      "Downscale: multiply level is not big enought for more operation, try "
      "to use larger depth");
  uint32_t sf_mod_size = (uint32_t)log2(eval->_params->_scaling_factor);
  IS_TRUE(waterline <= sf_mod_size,
          "Downscale: waterline should not larger than scaling factor");
  uint32_t ciph_sf_mod_size = (uint32_t)log2(ciph->_scaling_factor);
  IS_TRUE(ciph_sf_mod_size > sf_mod_size &&
              ciph_sf_mod_size < waterline + sf_mod_size,
          "Downscale: waterline is set too low or the scale of \
           input ciph is too high");
  uint32_t upscale_mod_size = waterline + sf_mod_size - ciph_sf_mod_size;
  Upscale_ciphertext(ciph, ciph, upscale_mod_size, eval);
  Set_ciph_sf_degree(ciph, Get_ciph_sf_degree(ciph) + 1);
  Rescale_ciphertext(res, ciph, eval);
}

void Modswitch_ciphertext(CIPHERTEXT* ciph, CKKS_EVALUATOR* eval) {
  size_t level          = Get_ciph_level(ciph);
  double scaling_factor = Get_param_sc(eval->_params);
  IS_TRUE((uint32_t)log2(Get_ciph_sfactor(ciph)) <=
              (level - 1) * (uint32_t)log2(scaling_factor),
          "Modswitch: invalid scaling factor of ciph");
  Mod_down_q_primes(Get_c0(ciph));
  Mod_down_q_primes(Get_c1(ciph));
}

CIPHERTEXT* Fast_switch_key(CIPHERTEXT* res, SWITCH_KEY* key,
                            CKKS_EVALUATOR* eval, VALUE_LIST* precomputed,
                            bool output_ntt) {
  IS_TRUE(LIST_LEN(precomputed) <= Get_swk_size(key), "unmatched size");

  CRT_CONTEXT* crt = eval->_params->_crt_context;
  POLYNOMIAL*  c0  = Get_c0(res);
  POLYNOMIAL*  c1  = Get_c1(res);

  CIPHERTEXT* ciph    = Alloc_ciphertext();
  POLYNOMIAL* ciph_c0 = Get_c0(ciph);
  POLYNOMIAL* ciph_c1 = Get_c1(ciph);
  size_t      size_p  = Get_primes_cnt(Get_p(crt));
  size_t size_ql = ((POLYNOMIAL*)Get_ptr_value_at(precomputed, 0))->_num_primes;
  Alloc_poly_data(ciph_c0, c0->_ring_degree, size_ql, size_p);
  Alloc_poly_data(ciph_c1, c0->_ring_degree, size_ql, size_p);
  Set_is_ntt(ciph_c0, TRUE);
  Set_is_ntt(ciph_c1, TRUE);

  Fast_switch_key_ext(ciph, key, eval, precomputed, output_ntt);

  Reduce_rns_base(c0, ciph_c0, crt);
  Reduce_rns_base(c1, ciph_c1, crt);
  Free_ciphertext(ciph);
  return res;
}

CIPHERTEXT* Fast_switch_key_ext(CIPHERTEXT* res, SWITCH_KEY* key,
                                CKKS_EVALUATOR* eval, VALUE_LIST* precomputed,
                                bool output_ntt) {
  size_t part_size = LIST_LEN(precomputed);
  IS_TRUE(part_size <= Get_swk_size(key), "unmatched size");

  CRT_CONTEXT* crt = eval->_params->_crt_context;
  POLYNOMIAL*  c0  = Get_c0(res);
  POLYNOMIAL*  c1  = Get_c1(res);
  for (size_t part = 0; part < part_size; part++) {
    PUBLIC_KEY* pk        = Get_swk_at(key, part);
    POLYNOMIAL* poly_key0 = Get_pk0(pk);
    POLYNOMIAL* poly_key1 = Get_pk1(pk);
    POLYNOMIAL* raised_c1 = (POLYNOMIAL*)Get_ptr_value_at(precomputed, part);
    Set_is_ntt(poly_key1, TRUE);
    if (!Is_ntt(poly_key0)) {
      Conv_poly2ntt_inplace(poly_key0, crt);
    }
    if (!Is_ntt(raised_c1)) {
      Conv_poly2ntt_inplace(raised_c1, crt);
    }

    // enable omp parallel
    // public key is shared data, we cannot change key level when OMP is ON,
    // to fix it, create two local polynomial, and adjust the level in the
    // local variable instead.
    POLYNOMIAL key0_at_level, key1_at_level;
    Derive_poly(&key0_at_level, poly_key0, Get_poly_level(c0),
                Get_num_p(poly_key0));
    Derive_poly(&key1_at_level, poly_key1, Get_poly_level(c1),
                Get_num_p(poly_key1));

    Multiply_add(c0, &key0_at_level, raised_c1, Get_q_primes(crt),
                 Get_p_primes(crt));
    Multiply_add(c1, &key1_at_level, raised_c1, Get_q_primes(crt),
                 Get_p_primes(crt));
  }
  if (!output_ntt) {
    Conv_ntt2poly_inplace(c0, crt);
    Conv_ntt2poly_inplace(c1, crt);
  }
  return res;
}

CIPHERTEXT* Switch_key_ext(CIPHERTEXT* res, CIPHERTEXT* ciph,
                           CKKS_EVALUATOR* eval, bool add_first) {
  POLYNOMIAL* ciph_c0 = Get_c0(ciph);
  POLYNOMIAL* ciph_c1 = Get_c1(ciph);
  IS_TRUE(Get_ciph_prime_p_cnt(ciph) == 0, "ciph should not be extended");

  CRT_CONTEXT* crt     = eval->_params->_crt_context;
  size_t       degree  = eval->_params->_poly_degree;
  size_t       size_ql = Get_ciph_prime_cnt(ciph);
  size_t       size_p  = Get_crt_num_p(crt);

  POLYNOMIAL* c0 = Get_c0(res);
  POLYNOMIAL* c1 = Get_c1(res);
  Init_ciphertext(res, degree, size_ql, size_p, ciph->_scaling_factor,
                  ciph->_sf_degree, ciph->_slots);

  POLYNOMIAL psi;
  Alloc_poly_data(&psi, degree, size_ql, 0);
  if (add_first) {
    Scalars_integer_multiply_poly(&psi, ciph_c0, Get_pmodq(Get_p(crt)),
                                  Get_q(crt), NULL);
    Copy_polynomial(c0, &psi);
  }
  Scalars_integer_multiply_poly(&psi, ciph_c1, Get_pmodq(Get_p(crt)),
                                Get_q(crt), NULL);
  Copy_polynomial(c1, &psi);
  Free_poly_data(&psi);
  return res;
}

void Rotate_ciphertext(CIPHERTEXT* rot_ciph, CIPHERTEXT* ciph, uint32_t k,
                       CKKS_EVALUATOR* eval) {
  POLYNOMIAL* c0 = Get_c0(ciph);
  POLYNOMIAL* c1 = Get_c1(ciph);
  IS_TRUE(Is_size_match(c0, c1), "unmatched c0 c1");

  VALUE_LIST*  precomp = Get_precomp_auto_order(eval->_keygen, k);
  CRT_CONTEXT* crt     = eval->_params->_crt_context;
  Automorphism_transform(Get_c0(rot_ciph), c0, precomp, crt);
  Automorphism_transform(Get_c1(rot_ciph), c1, precomp, crt);

  Set_is_ntt(Get_c0(rot_ciph), Is_ntt(c0));
  Set_is_ntt(Get_c1(rot_ciph), Is_ntt(c1));
}

CIPHERTEXT* Fast_rotate(CIPHERTEXT* rot_ciph, CIPHERTEXT* ciph,
                        int32_t rotation, SWITCH_KEY* rot_key,
                        CKKS_EVALUATOR* eval, VALUE_LIST* precomputed) {
  Init_ciphertext_from_ciph(rot_ciph, ciph, ciph->_scaling_factor,
                            ciph->_sf_degree);
  CRT_CONTEXT* crt       = eval->_params->_crt_context;
  CIPHERTEXT*  temp_ciph = Alloc_ciphertext();
  POLYNOMIAL*  temp_c0   = Get_c0(temp_ciph);
  POLYNOMIAL*  ciph_c0   = Get_c0(ciph);
  Init_ciphertext_from_ciph(temp_ciph, ciph, ciph->_scaling_factor,
                            ciph->_sf_degree);

  Fast_switch_key(temp_ciph, rot_key, eval, precomputed, Is_ntt(ciph_c0));
  Add_poly(temp_c0, temp_c0, ciph_c0, crt, NULL);

  uint32_t k = Get_precomp_auto_idx(eval->_keygen, rotation);
  FMT_ASSERT(k, "cannot get precompute automorphism index");
  Rotate_ciphertext(rot_ciph, temp_ciph, k, eval);
  Free_ciphertext(temp_ciph);
  return rot_ciph;
}

CIPHERTEXT* Eval_fast_rotate(CIPHERTEXT* rot_ciph, CIPHERTEXT* ciph,
                             int32_t rotation, SWITCH_KEY* rot_key,
                             CKKS_EVALUATOR* eval) {
  VALUE_LIST* precomputed =
      Switch_key_precompute(Get_c1(ciph), eval->_params->_crt_context);
  Fast_rotate(rot_ciph, ciph, rotation, rot_key, eval, precomputed);
  Free_switch_key_precomputed(precomputed);
  return rot_ciph;
}

CIPHERTEXT* Fast_rotate_ext(CIPHERTEXT* rot_ciph, CIPHERTEXT* ciph,
                            int32_t rotation, SWITCH_KEY* rot_key,
                            CKKS_EVALUATOR* eval, VALUE_LIST* precomputed,
                            bool add_first) {
  CRT_CONTEXT* crt    = eval->_params->_crt_context;
  size_t       size_p = Get_primes_cnt(Get_p(crt));
  size_t size_ql = ((POLYNOMIAL*)Get_ptr_value_at(precomputed, 0))->_num_primes;
  size_t degree  = Get_c0(ciph)->_ring_degree;
  Init_ciphertext(rot_ciph, degree, size_ql, size_p, ciph->_scaling_factor,
                  ciph->_sf_degree, ciph->_slots);

  CIPHERTEXT* temp_ciph = Alloc_ciphertext();
  POLYNOMIAL* temp_c0   = Get_c0(temp_ciph);
  POLYNOMIAL* temp_c1   = Get_c1(temp_ciph);
  POLYNOMIAL* ciph_c0   = Get_c0(ciph);
  Init_ciphertext_from_ciph(temp_ciph, rot_ciph, ciph->_scaling_factor,
                            ciph->_sf_degree);
  Set_is_ntt(temp_c0, TRUE);
  Set_is_ntt(temp_c1, TRUE);

  Fast_switch_key_ext(temp_ciph, rot_key, eval, precomputed, Is_ntt(ciph_c0));

  if (add_first) {
    POLYNOMIAL psi_c0;
    Alloc_poly_data(&psi_c0, ciph_c0->_ring_degree, size_ql, 0);
    Scalars_integer_multiply_poly(&psi_c0, ciph_c0, Get_pmodq(Get_p(crt)),
                                  Get_q(crt), NULL);
    Add_poly(temp_c0, temp_c0, &psi_c0, crt, NULL);
    Free_poly_data(&psi_c0);
  }

  uint32_t k = Get_precomp_auto_idx(eval->_keygen, rotation);
  FMT_ASSERT(k, "cannot get precompute automorphism index");
  Rotate_ciphertext(rot_ciph, temp_ciph, k, eval);
  Free_ciphertext(temp_ciph);
  return rot_ciph;
}

CIPHERTEXT* Conjugate(CIPHERTEXT* conj_ciph, CIPHERTEXT* ciph,
                      SWITCH_KEY* conj_key, CKKS_EVALUATOR* eval) {
  CRT_CONTEXT* crt         = eval->_params->_crt_context;
  POLYNOMIAL*  c1          = Get_c1(ciph);
  VALUE_LIST*  precomputed = Switch_key_precompute(c1, crt);
  size_t       ring_degree = c1->_ring_degree;
  int64_t      rot_idx     = 2 * ring_degree - 1;

  if (conj_ciph != ciph) {
    // not inplace conjugate, initialze conj_ciph
    Init_ciphertext_from_ciph(conj_ciph, ciph, ciph->_scaling_factor,
                              ciph->_sf_degree);
  }
  CIPHERTEXT* temp_ciph = Alloc_ciphertext();
  POLYNOMIAL* temp_c0   = Get_c0(temp_ciph);
  POLYNOMIAL* ciph_c0   = Get_c0(ciph);
  Init_ciphertext_from_ciph(temp_ciph, ciph, ciph->_scaling_factor,
                            ciph->_sf_degree);

  Fast_switch_key(temp_ciph, conj_key, eval, precomputed, Is_ntt(ciph_c0));
  Add_poly(temp_c0, temp_c0, ciph_c0, crt, NULL);

  Rotate_ciphertext(conj_ciph, temp_ciph, rot_idx, eval);

  Free_ciphertext(temp_ciph);
  Free_switch_key_precomputed(precomputed);
  return conj_ciph;
}
