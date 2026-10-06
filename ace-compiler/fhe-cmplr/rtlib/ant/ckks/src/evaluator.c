//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "ckks/evaluator.h"

#include "ckks/bootstrap.h"
#include "ckks/ciphertext.h"
#include "ckks/encoder.h"
#include "ckks/key.h"
#include "ckks/param.h"
#include "ckks/plaintext.h"
#include "ckks/public_key.h"
#include "ckks/switch_key.h"
#include "common/trace.h"
#include "poly/rns_poly.h"
#include "util/crt.h"
#include "util/matrix_operation.h"

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
  Add_poly(c0, Get_c0(ciph1), Get_c0(ciph2));
  Add_poly(c1, Get_c1(ciph1), Get_c1(ciph2));

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
  Sub_poly(Get_c0(res), Get_c0(ciph1), Get_c0(ciph2));
  Sub_poly(Get_c1(res), Get_c1(ciph1), Get_c1(ciph2));

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

  Add_poly(c0, Get_c0(ciph), Get_plain_poly(plain));
  Copy_poly(c1, Get_c1(ciph));
  return res;
}

CIPHERTEXT* Add_scalars(CIPHERTEXT* res, CIPHERTEXT* ciph, VALUE_LIST* scalars,
                        CKKS_EVALUATOR* eval) {
  IS_TRUE(ciph, "ciph is not Ciphertext");
  IS_TRUE(LIST_TYPE(scalars) == I64_TYPE, "invalid value type");
  IS_TRUE(LIST_LEN(scalars) == Get_ciph_num_pq(ciph),
          "length of scalars not match");
  Init_ciphertext_from_ciph(res, ciph, ciph->_scaling_factor, ciph->_sf_degree);

  Scalars_integer_add_poly(Get_c0(res), Get_c0(ciph), scalars);
  Copy_poly(Get_c1(res), Get_c1(ciph));

  return res;
}

CIPHERTEXT* Add_const(CIPHERTEXT* res, CIPHERTEXT* ciph, double const_val,
                      CKKS_EVALUATOR* eval) {
  IS_TRUE(ciph, "ciph is not ciphertext");
  uint32_t level = Get_ciph_level(ciph);
  double   scale = Get_ciph_sfactor(ciph);

  VALUE_LIST* scalars = Alloc_value_list(I64_TYPE, level);
  Encode_val(scalars, const_val, level, scale, Get_ciph_prime_p_cnt(ciph));

  Add_scalars(res, ciph, scalars, eval);

  Free_value_list(scalars);
  return res;
}

CIPHERTEXT3* Mul_ciphertext3(CIPHERTEXT3* res, CIPHERTEXT* ciph1,
                             CIPHERTEXT* ciph2, CKKS_EVALUATOR* eval) {
  IS_TRUE(ciph1, "ciph1 is not Ciphertext");
  IS_TRUE(ciph2, "ciph2 is not Ciphertext");

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
  Mul_poly(c0, Get_c0(ciph1), Get_c0(ciph2));
  Mul_poly(c1, Get_c0(ciph1), Get_c1(ciph2));
  Mul_poly(&ctemp, Get_c1(ciph1), Get_c0(ciph2));
  Add_poly(c1, c1, &ctemp);
  Mul_poly(c2, Get_c1(ciph1), Get_c1(ciph2));

  // recover orignal level
  Restore_level(adj_ciph, orig_level);
  Free_poly_data(&ctemp);
  return res;
}

CIPHERTEXT* Mul_ciphertext(CIPHERTEXT* res, CIPHERTEXT* ciph1,
                           CIPHERTEXT* ciph2, CKKS_EVALUATOR* eval) {
  CIPHERTEXT3 ciph3;
  memset(&ciph3, 0, sizeof(ciph3));
  Mul_ciphertext3(&ciph3, ciph1, ciph2, eval);
  // relinearize 3-dimensional back down to 2 dimensions
  Relinearize_ciph3(res, Get_ciph3_c0(&ciph3), Get_ciph3_c1(&ciph3),
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
  POLYNOMIAL* c0 = Get_c0(res);
  POLYNOMIAL* c1 = Get_c1(res);

  IS_TRUE(c0->_num_primes_p == Get_c0(ciph)->_num_primes_p &&
              c0->_num_primes_p == Get_plain_poly(plain)->_num_primes_p,
          "unmatched mod");
  Mul_poly(c0, Get_c0(ciph), Get_plain_poly(plain));
  Mul_poly(c1, Get_c1(ciph), Get_plain_poly(plain));

  return res;
}

CIPHERTEXT* Mul_scalars(CIPHERTEXT* res, CIPHERTEXT* ciph, VALUE_LIST* scalars,
                        double scale, CKKS_EVALUATOR* eval) {
  IS_TRUE(ciph, "ciph is not Ciphertext");
  IS_TRUE(LIST_TYPE(scalars) == I64_TYPE, "invalid value type");
  IS_TRUE(LIST_LEN(scalars) == Get_ciph_num_pq(ciph),
          "length of scalars not match");

  double new_factor = ciph->_scaling_factor * scale;
  IS_TRUE(((uint32_t)log2(new_factor) <
           Get_coeff_bit_count(Get_ciph_prime_cnt(ciph), eval->_params)),
          "invalid scaling factor for Mul_plaintext");
  Init_ciphertext_from_ciph(res, ciph, new_factor, ciph->_sf_degree + 1);

  Scalars_integer_multiply_poly(Get_c0(res), Get_c0(ciph), scalars);
  Scalars_integer_multiply_poly(Get_c1(res), Get_c1(ciph), scalars);

  return res;
}

CIPHERTEXT* Mul_const(CIPHERTEXT* res, CIPHERTEXT* ciph, double const_val,
                      CKKS_EVALUATOR* eval) {
  uint32_t    level   = Get_ciph_level(ciph);
  VALUE_LIST* scalars = Alloc_value_list(I64_TYPE, level);
  double      scale   = eval->_params->_scaling_factor;
  Encode_val(scalars, const_val, level, scale, Get_ciph_prime_p_cnt(ciph));

  Mul_scalars(res, ciph, scalars, scale, eval);

  Free_value_list(scalars);
  return res;
}

CIPHERTEXT* Mul_integer(CIPHERTEXT* res, CIPHERTEXT* ciph, uint32_t power,
                        CKKS_EVALUATOR* eval) {
  Init_ciphertext_from_ciph(res, ciph, ciph->_scaling_factor, ciph->_sf_degree);
  IS_TRUE(Get_c0(res)->_num_primes_p == Get_c0(ciph)->_num_primes_p,
          "unmatched mod");
  Scalar_integer_multiply_poly(Get_c0(res), Get_c0(ciph), power);
  Scalar_integer_multiply_poly(Get_c1(res), Get_c1(ciph), power);
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
  POLYNOMIAL   monomial;
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

  Mul_poly(Get_c0(res), Get_c0(ciph), &monomial);
  Mul_poly(Get_c1(res), Get_c1(ciph), &monomial);

  Free_poly_data(&monomial);
  return res;
}

CIPHERTEXT* Relinearize_ciph3(CIPHERTEXT* res, POLYNOMIAL* c0, POLYNOMIAL* c1,
                              POLYNOMIAL* c2, double new_scaling_factor,
                              uint32_t new_sf_degree, uint32_t slots,
                              CKKS_EVALUATOR* eval) {
  POLYNOMIAL* res_c0 = Get_c0(res);
  POLYNOMIAL* res_c1 = Get_c1(res);

  Init_ciphertext_from_poly(res, c2, new_scaling_factor, new_sf_degree, slots);

  VALUE_LIST* precomputed = Alloc_precomp(c2);
  Fast_switch_key(res, false /* is_rot */, 0 /* rot_idx */, eval, precomputed,
                  Is_ntt(c0));

  Add_poly(res_c0, res_c0, c0);
  Add_poly(res_c1, res_c1, c1);

  Free_precomp(precomputed);
  return res;
}

CIPHERTEXT* Relinearize_ciph3_ext(CIPHERTEXT* res, CIPHERTEXT3* ciph3,
                                  CKKS_EVALUATOR* eval) {
  POLYNOMIAL* res_c0         = Get_c0(res);
  POLYNOMIAL* res_c1         = Get_c1(res);
  POLYNOMIAL* c0             = Get_ciph3_c0(ciph3);
  POLYNOMIAL* c1             = Get_ciph3_c1(ciph3);
  POLYNOMIAL* c2             = Get_ciph3_c2(ciph3);
  uint32_t    sf_degree      = Get_ciph3_sf_degree(ciph3);
  uint32_t    slots          = Get_ciph3_slots(ciph3);
  double      scaling_factor = Get_ciph3_sfactor(ciph3);

  VALUE_LIST* precomputed = Alloc_precomp(c2);
  size_t size_ql = ((POLYNOMIAL*)Get_ptr_value_at(precomputed, 0))->_num_primes;
  size_t size_p  = Get_p_cnt();

  Init_ciphertext(res, Get_rdgree(c0), size_ql, size_p, scaling_factor,
                  sf_degree, slots);
  Set_is_ntt(Get_c0(res), TRUE);
  Set_is_ntt(Get_c1(res), TRUE);
  Fast_switch_key_ext(res, false /* is_rot */, 0 /* rot_idx */, eval,
                      precomputed, Is_ntt(c0));

  POLYNOMIAL poly_ext;
  Alloc_poly_data(&poly_ext, c0->_ring_degree, size_ql, size_p);
  Extend(&poly_ext, c0);
  Add_poly(res_c0, res_c0, &poly_ext);
  Extend(&poly_ext, c1);
  Add_poly(res_c1, res_c1, &poly_ext);

  Free_poly_data(&poly_ext);
  Free_precomp(precomputed);
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

  Rescale(Get_c0(res), Get_c0(ciph));
  Rescale(Get_c1(res), Get_c1(ciph));

  return res;
}

CIPHERTEXT* Upscale_ciphertext(CIPHERTEXT* res, CIPHERTEXT* ciph,
                               uint32_t mod_size, CKKS_EVALUATOR* eval) {
  double up_scale  = pow(2.0, mod_size);
  double new_scale = Get_ciph_sfactor(ciph) * up_scale;
  IS_TRUE(((uint32_t)log2(new_scale) <
           Get_coeff_bit_count(Get_ciph_prime_cnt(ciph), eval->_params)),
          "Upscale: mod_size is two high");

  uint32_t    level          = Get_ciph_level(ciph);
  VALUE_LIST* scaled_scalars = Alloc_value_list(I64_TYPE, level);
  Encode_val(scaled_scalars, 1.0, level, up_scale, Get_ciph_prime_p_cnt(ciph));
  Mul_scalars(res, ciph, scaled_scalars, up_scale, eval);
  Free_value_list(scaled_scalars);
  return res;
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
  return res;
}

void Modswitch_ciphertext(CIPHERTEXT* ciph, CKKS_EVALUATOR* eval) {
  size_t level          = Get_ciph_level(ciph);
  double scaling_factor = Get_param_sc(eval->_params);
  IS_TRUE((uint32_t)log2(Get_ciph_sfactor(ciph)) <=
              (level - 1) * (uint32_t)log2(scaling_factor),
          "Modswitch: invalid scaling factor of ciph");
  Modswitch(Get_c0(ciph), Get_c0(ciph));
  Modswitch(Get_c1(ciph), Get_c1(ciph));
}

CIPHERTEXT* Fast_switch_key(CIPHERTEXT* res, bool is_rot, int32_t rot_idx,
                            CKKS_EVALUATOR* eval, VALUE_LIST* precomputed,
                            bool output_ntt) {
  CRT_CONTEXT* crt = eval->_params->_crt_context;
  IS_TRUE(LIST_LEN(precomputed) <= Get_num_parts(Get_qpart(crt)),
          "unmatched size");
  POLYNOMIAL* c0 = Get_c0(res);
  POLYNOMIAL* c1 = Get_c1(res);

  CIPHERTEXT* ciph    = Alloc_ciphertext();
  POLYNOMIAL* ciph_c0 = Get_c0(ciph);
  POLYNOMIAL* ciph_c1 = Get_c1(ciph);
  size_t      size_p  = Get_primes_cnt(Get_p(crt));
  size_t size_ql = ((POLYNOMIAL*)Get_ptr_value_at(precomputed, 0))->_num_primes;
  Alloc_poly_data(ciph_c0, c0->_ring_degree, size_ql, size_p);
  Alloc_poly_data(ciph_c1, c0->_ring_degree, size_ql, size_p);
  Set_is_ntt(ciph_c0, TRUE);
  Set_is_ntt(ciph_c1, TRUE);

  Fast_switch_key_ext(ciph, is_rot, rot_idx, eval, precomputed, output_ntt);

  Mod_down(c0, ciph_c0);
  Mod_down(c1, ciph_c1);
  Free_ciphertext(ciph);
  return res;
}

CIPHERTEXT* Fast_switch_key_ext(CIPHERTEXT* res, bool is_rot, int32_t rot_idx,
                                CKKS_EVALUATOR* eval, VALUE_LIST* precomputed,
                                bool output_ntt) {
  size_t part_size = LIST_LEN(precomputed);
  size_t swk_size  = Get_q_parts();
  IS_TRUE(part_size <= swk_size, "unmatched size");

  POLYNOMIAL* c0 = Get_c0(res);
  POLYNOMIAL* c1 = Get_c1(res);
  // get poly array of switch key c0 & c1
  POLYNOMIAL key0[swk_size];
  POLYNOMIAL key1[swk_size];
  Swk_c0(key0, swk_size, is_rot, rot_idx);
  Swk_c1(key1, swk_size, is_rot, rot_idx);
  // Dot product for key switch
  POLYNOMIAL** raised_c1 = (POLYNOMIAL**)Get_ptr_values(precomputed);
  Dot_prod(c0, *raised_c1, key0, part_size);
  Dot_prod(c1, *raised_c1, key1, part_size);

  if (!output_ntt) {
    Conv_ntt2poly_inplace(c0);
    Conv_ntt2poly_inplace(c1);
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

  if (add_first) {
    Extend(c0, ciph_c0);
  }
  Extend(c1, ciph_c1);

  return res;
}

void Rotate_ciphertext(CIPHERTEXT* rot_ciph, CIPHERTEXT* ciph, uint32_t k,
                       CKKS_EVALUATOR* eval) {
  POLYNOMIAL* c0 = Get_c0(ciph);
  POLYNOMIAL* c1 = Get_c1(ciph);
  IS_TRUE(Is_size_match(c0, c1), "unmatched c0 c1");

  VALUE_LIST* precomp = Get_precomp_auto_order(eval->_keygen, k);
  IS_TRUE(precomp, "cannot find precomputed automorphism order");
  Automorphism_transform(Get_c0(rot_ciph), c0, precomp);
  Automorphism_transform(Get_c1(rot_ciph), c1, precomp);

  Set_is_ntt(Get_c0(rot_ciph), Is_ntt(c0));
  Set_is_ntt(Get_c1(rot_ciph), Is_ntt(c1));
}

CIPHERTEXT* Fast_rotate(CIPHERTEXT* rot_ciph, CIPHERTEXT* ciph, int32_t rot_idx,
                        CKKS_EVALUATOR* eval, VALUE_LIST* precomputed) {
  Init_ciphertext_from_ciph(rot_ciph, ciph, ciph->_scaling_factor,
                            ciph->_sf_degree);
  CIPHERTEXT* temp_ciph = Alloc_ciphertext();
  POLYNOMIAL* temp_c0   = Get_c0(temp_ciph);
  POLYNOMIAL* ciph_c0   = Get_c0(ciph);
  Init_ciphertext_from_ciph(temp_ciph, ciph, ciph->_scaling_factor,
                            ciph->_sf_degree);

  Fast_switch_key(temp_ciph, true /* is_rot */, rot_idx, eval, precomputed,
                  Is_ntt(ciph_c0));
  Add_poly(temp_c0, temp_c0, ciph_c0);

  uint32_t k = Get_precomp_auto_idx(eval->_keygen, rot_idx);
  FMT_ASSERT(k, "cannot get precompute automorphism index");
  Rotate_ciphertext(rot_ciph, temp_ciph, k, eval);
  Free_ciphertext(temp_ciph);
  return rot_ciph;
}

CIPHERTEXT* Eval_fast_rotate(CIPHERTEXT* rot_ciph, CIPHERTEXT* ciph,
                             int32_t rot_idx, CKKS_EVALUATOR* eval) {
  VALUE_LIST* precomputed = Alloc_precomp(Get_c1(ciph));
  Fast_rotate(rot_ciph, ciph, rot_idx, eval, precomputed);
  Free_precomp(precomputed);
  return rot_ciph;
}

CIPHERTEXT* Fast_rotate_ext(CIPHERTEXT* rot_ciph, CIPHERTEXT* ciph,
                            int32_t rot_idx, CKKS_EVALUATOR* eval,
                            VALUE_LIST* precomputed, bool add_first) {
  size_t size_p  = Get_p_cnt();
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

  Fast_switch_key_ext(temp_ciph, true /* is_rot */, rot_idx, eval, precomputed,
                      Is_ntt(ciph_c0));

  if (add_first) {
    POLYNOMIAL psi_c0;
    Alloc_poly_data(&psi_c0, ciph_c0->_ring_degree, size_ql, size_p);
    Extend(&psi_c0, ciph_c0);
    Add_poly(temp_c0, temp_c0, &psi_c0);
    Free_poly_data(&psi_c0);
  }

  uint32_t k = Get_precomp_auto_idx(eval->_keygen, rot_idx);
  FMT_ASSERT(k, "cannot get precompute automorphism index");
  Rotate_ciphertext(rot_ciph, temp_ciph, k, eval);
  Free_ciphertext(temp_ciph);
  return rot_ciph;
}

CIPHERTEXT* Conjugate(CIPHERTEXT* conj_ciph, CIPHERTEXT* ciph,
                      CKKS_EVALUATOR* eval) {
  POLYNOMIAL* c1          = Get_c1(ciph);
  VALUE_LIST* precomputed = Alloc_precomp(c1);
  size_t      ring_degree = c1->_ring_degree;
  int64_t     rot_idx     = 2 * ring_degree - 1;

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

  Fast_switch_key(temp_ciph, true /* is_rot */, rot_idx, eval, precomputed,
                  Is_ntt(ciph_c0));
  Add_poly(temp_c0, temp_c0, ciph_c0);

  Rotate_ciphertext(conj_ciph, temp_ciph, rot_idx, eval);

  Free_ciphertext(temp_ciph);
  Free_precomp(precomputed);
  return conj_ciph;
}
