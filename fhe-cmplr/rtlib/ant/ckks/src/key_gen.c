//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "ckks/key_gen.h"

#include "util/number_theory.h"
#include "util/prng.h"
#include "util/random_sample.h"

CKKS_KEY_GENERATOR* Alloc_ckks_key_generator(CKKS_PARAMETER* params,
                                             int32_t*        rot_idx,
                                             size_t          num_rot_idx) {
  CKKS_KEY_GENERATOR* generator =
      (CKKS_KEY_GENERATOR*)malloc(sizeof(CKKS_KEY_GENERATOR));

  uint32_t degree        = params->_poly_degree;
  size_t   num_primes    = params->_num_primes;
  size_t   num_p_primes  = params->_num_p_primes;
  generator->_params     = params;
  generator->_secret_key = Alloc_secret_key(degree, num_primes, num_p_primes);
  Generate_secret_key(generator);
  generator->_public_key = Alloc_public_key(degree, num_primes, 0);
  Generate_public_key(generator);
  generator->_relin_key = Alloc_switch_key();
  Generate_relin_key(generator);

  generator->_precomp_auto_idx_map   = NULL;
  generator->_precomp_auto_order_map = NULL;
  generator->_auto_key_map           = NULL;
  if (num_rot_idx) {
    Generate_rot_maps(generator, num_rot_idx, rot_idx);
  }
  return generator;
}

// cleanup CKKS_KEY_GENERATOR
void Free_ckks_key_generator(CKKS_KEY_GENERATOR* generator) {
  if (generator == NULL) return;
  if (generator->_secret_key) {
    Free_secretkey(generator->_secret_key);
    generator->_secret_key = NULL;
  }
  if (generator->_public_key) {
    Free_publickey(generator->_public_key);
    generator->_public_key = NULL;
  }
  if (generator->_relin_key) {
    Free_switch_key(generator->_relin_key);
    generator->_relin_key = NULL;
  }
  if (generator->_precomp_auto_idx_map) {
    Free_precomp_auto_idx_map(generator->_precomp_auto_idx_map);
    generator->_precomp_auto_idx_map = NULL;
  }
  if (generator->_precomp_auto_order_map) {
    Free_precomp_auto_order_map(generator->_precomp_auto_order_map);
    generator->_precomp_auto_order_map = NULL;
  }
  if (generator->_auto_key_map) {
    Free_auto_keys(generator->_auto_key_map);
    generator->_auto_key_map = NULL;
  }
  free(generator);
  Free_prng();
}

void Generate_secret_key(CKKS_KEY_GENERATOR* generator) {
  SECRET_KEY* sk   = generator->_secret_key;
  POLYNOMIAL* poly = Get_sk_poly(sk);

  // gen sk with CKKS_PARAMETER::_hamming_weight
  Sample_ternary_poly(poly, generator->_params->_hamming_weight);

  Conv_poly2ntt(Get_ntt_sk(sk), poly);
  IS_TRACE("secret key: ");
  IS_TRACE_CMD(Print_sk(T_FILE, generator->_secret_key));
  IS_TRACE(S_BAR);
}

void Generate_public_key(CKKS_KEY_GENERATOR* generator) {
  POLYNOMIAL*  pk0 = Get_pk0(generator->_public_key);
  POLYNOMIAL*  pk1 = Get_pk1(generator->_public_key);
  POLYNOMIAL   pk_error;
  CRT_CONTEXT* crt         = generator->_params->_crt_context;
  uint32_t     ring_degree = generator->_params->_poly_degree;
  size_t       num_primes  = generator->_params->_num_primes;

  // generate e'
  VALUE_LIST* tri_samples = Alloc_value_list(I64_TYPE, ring_degree);
  Sample_triangle(tri_samples);
  Alloc_poly_data(&pk_error, ring_degree, num_primes, 0);
  Transform_value_to_rns_poly(&pk_error, tri_samples, TRUE);

  // generate a
  Sample_uniform_poly(pk1);
  IS_TRACE("pk1: ");
  IS_TRACE_CMD(Print_poly(T_FILE, pk1));
  IS_TRACE(S_BAR);

  // a * s
  Set_is_ntt(pk1, TRUE);  // skip one NTT convert
  Mul_poly(pk0, pk1, Get_ntt_sk(generator->_secret_key));

  IS_TRACE("generate pk coeff * secret: ");
  IS_TRACE_CMD(Print_poly(T_FILE, pk0));
  IS_TRACE(S_BAR);
  // - a * s
  Scalar_integer_multiply_poly(pk0, pk0, -1);
  IS_TRACE("generate pk p0 * -1: ");
  IS_TRACE_CMD(Print_poly(T_FILE, pk0));
  IS_TRACE(S_BAR);
  // -a * s + e
  Add_poly(pk0, pk0, &pk_error);
  IS_TRACE("public key: ");
  IS_TRACE_CMD(Print_pk(T_FILE, generator->_public_key));
  IS_TRACE(S_BAR);

  Free_value_list(tri_samples);
  Free_poly_data(&pk_error);
}

void Generate_switching_key(SWITCH_KEY* res, CKKS_KEY_GENERATOR* generator,
                            POLYNOMIAL* new_key, POLYNOMIAL* old_key,
                            bool is_fast) {
  CRT_CONTEXT* crt          = generator->_params->_crt_context;
  uint32_t     ring_degree  = generator->_params->_poly_degree;
  size_t       num_primes   = generator->_params->_num_primes;
  size_t       num_primes_p = generator->_params->_num_p_primes;
  size_t       num_qpart    = Get_num_parts(Get_qpart(crt));

  Init_switch_key(res, ring_degree, num_primes, num_primes_p, num_qpart);

  IS_TRACE("old key: ");
  IS_TRACE_CMD(Print_poly(T_FILE, old_key));
  IS_TRACE("new key: ");
  IS_TRACE_CMD(Print_poly(T_FILE, new_key));

  VALUE_LIST* uniform_samples = Alloc_value_list(I64_TYPE, ring_degree);
  VALUE_LIST* tri_samples     = Alloc_value_list(I64_TYPE, ring_degree);
  POLYNOMIAL  e;
  Alloc_poly_data(&e, ring_degree, num_primes, num_primes_p);
  for (size_t part = 0; part < num_qpart; part++) {
    Set_is_ntt(&e, FALSE);
    PUBLIC_KEY* pk = Get_swk_at(res, part);
    POLYNOMIAL* b  = Get_pk0(pk);
    POLYNOMIAL* a  = Get_pk1(pk);
    // a
    Sample_uniform_poly(a);

#ifndef OPENFHE_COMPAT
    if (!is_fast) {
      Set_is_ntt(a, TRUE);
    }
#else
    Set_is_ntt(a, TRUE);
#endif
    IS_TRACE("key a: ");
    IS_TRACE_CMD(Print_poly(T_FILE, a));

    // b = a * old_key
    Mul_poly(b, a, old_key);
    IS_TRACE("b = a * old_key:");
    IS_TRACE_CMD(Print_poly(T_FILE, b));

    // P * new_key
    POLYNOMIAL pmuls;
    Alloc_poly_data(&pmuls, ring_degree, num_primes, num_primes_p);
    Scalars_integer_multiply_poly_qpart(&pmuls, new_key, Get_pmodq(Get_p(crt)),
                                        part);
    IS_TRACE("P * new_key: ");
    IS_TRACE_CMD(Print_poly(T_FILE, &pmuls));

    // e
    Sample_triangle(tri_samples);
    Transform_value_to_rns_poly(&e, tri_samples, TRUE);
    IS_TRACE("e = :");
    IS_TRACE_CMD(Print_poly(T_FILE, &e));

    // e + P * new_key
    Add_poly(&e, &e, &pmuls);
    IS_TRACE("e + P * new_key: ");
    IS_TRACE_CMD(Print_poly(T_FILE, &e));

    // b = e - b = e + P * new_key - a * old_key
    Sub_poly(b, &e, b);

    IS_TRACE("part %ld key b = -a * old_key + P * new_key + e: ", part);
    IS_TRACE_CMD(Print_poly(T_FILE, b));

    Free_poly_data(&pmuls);
  }
  Free_poly_data(&e);
  Free_value_list(tri_samples);
  Free_value_list(uniform_samples);
}

// 𝑒𝑣𝑘 = (𝑏′, 𝑎′)
// 𝑏′ = −(𝑎's + 𝑒 + PS^2) (mod P∙𝑞_𝐿)
void Generate_relin_key(CKKS_KEY_GENERATOR* generator) {
  POLYNOMIAL* ntt_sk = Get_ntt_sk(generator->_secret_key);
  POLYNOMIAL  sk_squared;
  Alloc_poly_data(&sk_squared, ntt_sk->_ring_degree, ntt_sk->_num_primes, 0);
  Mul_poly(&sk_squared, ntt_sk, ntt_sk);
  IS_TRACE("sk_squared:");
  IS_TRACE_CMD(Print_poly(T_FILE, &sk_squared));
  Generate_switching_key(generator->_relin_key, generator, &sk_squared, ntt_sk,
                         FALSE);
  Free_poly_data(&sk_squared);
}

void Generate_conj_key(SWITCH_KEY* res, CKKS_KEY_GENERATOR* generator) {
  size_t      ring_degree  = generator->_params->_poly_degree;
  size_t      num_primes   = generator->_params->_num_primes;
  size_t      num_primes_p = generator->_params->_num_p_primes;
  POLYNOMIAL* ntt_sk       = Get_ntt_sk(generator->_secret_key);
  POLYNOMIAL* sk           = Get_sk_poly(generator->_secret_key);
  uint32_t    rot_idx      = 2 * ring_degree - 1;
  POLYNOMIAL  new_key;
  Alloc_poly_data(&new_key, ring_degree, num_primes, num_primes_p);

  VALUE_LIST* precomp = Alloc_value_list(I64_TYPE, ring_degree);
  Precompute_automorphism_order(precomp, rot_idx, ring_degree, TRUE);
  Insert_precomp_auto_order(generator, rot_idx, precomp);
  Automorphism_transform(&new_key, ntt_sk, precomp);
  // old_key & new_key is switched for fast conjugate
  Generate_switching_key(res, generator, sk, &new_key, TRUE);
  Free_poly_data(&new_key);
}

void Generate_rot_key(SWITCH_KEY* res, CKKS_KEY_GENERATOR* generator,
                      uint32_t auto_idx, bool is_fast) {
  IS_TRACE("\ngenerate rotation key\n");
  uint32_t    ring_degree  = generator->_params->_poly_degree;
  size_t      num_primes   = generator->_params->_num_primes;
  size_t      num_primes_p = generator->_params->_num_p_primes;
  POLYNOMIAL* sk           = Get_sk_poly(generator->_secret_key);
  POLYNOMIAL* ntt_sk       = Get_ntt_sk(generator->_secret_key);

  POLYNOMIAL new_key;
  Alloc_poly_data(&new_key, ring_degree, num_primes, num_primes_p);
  // need extra mod inv for automorphism index
  MODULUS modulus;
  Init_modulus(&modulus, 2 * ring_degree);
  uint32_t    gen_idx = Mod_inv(auto_idx, &modulus);
  VALUE_LIST* precomp = Alloc_value_list(I64_TYPE, ring_degree);
  Precompute_automorphism_order(precomp, gen_idx, ring_degree, TRUE);
  if (is_fast) {
    Automorphism_transform(&new_key, ntt_sk, precomp);
    // old_key & new_key is switched for fast rotate
    Generate_switching_key(res, generator, sk, &new_key, TRUE);
  } else {
    Automorphism_transform(&new_key, sk, precomp);
    Generate_switching_key(res, generator, &new_key, ntt_sk, FALSE);
  }
  Free_poly_data(&new_key);
  Free_value_list(precomp);
}

SWITCH_KEY* Insert_rot_map(CKKS_KEY_GENERATOR* generator, int32_t rotation) {
  uint32_t degree   = generator->_params->_poly_degree;
  uint32_t auto_idx = Get_precomp_auto_idx(generator, rotation);
  if (!auto_idx) {
    // generate precompute automorphism index
    MODULUS two_n_modulus;
    Init_modulus(&two_n_modulus, 2 * degree);
    auto_idx = Find_automorphism_index(rotation, &two_n_modulus);
    Insert_precomp_auto_idx(generator, rotation, auto_idx);

    VALUE_LIST* precomp_list = Get_precomp_auto_order(generator, auto_idx);
    if (!precomp_list) {
      // generate precomputed order for automorphism
      precomp_list = Alloc_value_list(I64_TYPE, degree);
      Precompute_automorphism_order(precomp_list, auto_idx, degree, TRUE);
      Insert_precomp_auto_order(generator, auto_idx, precomp_list);

      // generate rot key
      SWITCH_KEY* key = Alloc_switch_key();
      Generate_rot_key(key, generator, auto_idx, TRUE);
      Insert_auto_key(generator, auto_idx, key);
      return key;
    }
  }
  return Get_auto_key(generator, auto_idx);
}

void Generate_rot_maps(CKKS_KEY_GENERATOR* generator, size_t num_rot_idx,
                       int32_t* rot_idxs) {
  uint32_t degree = generator->_params->_poly_degree;
  MODULUS  two_n_modulus;
  Init_modulus(&two_n_modulus, 2 * degree);

  AUTO_IDX_LIST* auto_indices  = NULL;
  VALUE_LIST*    auto_list     = Alloc_value_list(UI32_TYPE, num_rot_idx);
  size_t         auto_list_cnt = 0;
  for (size_t i = 0; i < num_rot_idx; i++) {
    int32_t rotation = rot_idxs[i];
    // generate precompute automorphism index
    uint32_t auto_idx = Get_precomp_auto_idx(generator, rotation);
    // if <rotation, auto_idx> exist in _precomp_auto_idx_map, continue the loop
    if (auto_idx) continue;
    auto_idx = Find_automorphism_index(rotation, &two_n_modulus);
    Insert_precomp_auto_idx(generator, rotation, auto_idx);

    AUTO_IDX_LIST* entry;
    HASH_FIND_INT(auto_indices, &auto_idx, entry);
    // if auto_idx already exist in auto_indices or _precomp_auto_order_map,
    // continue the loop
    if (entry != NULL || Get_precomp_auto_order(generator, auto_idx)) continue;

    entry = (AUTO_IDX_LIST*)malloc(sizeof(AUTO_IDX_LIST));
    IS_TRUE(entry != NULL, "failed to malloc AUTO_IDX_LIST");
    entry->_auto_idx = auto_idx;
    HASH_ADD_INT(auto_indices, _auto_idx, entry);
    Set_ui32_value(auto_list, auto_list_cnt++, auto_idx);
  }
  IS_TRACE(HASH_COUNT(auto_indices) == auto_list_cnt,
           "auto list count mismatch");
  Free_auto_idx_list(auto_indices);

  VALUE_LIST** precomp_list =
      (VALUE_LIST**)malloc(sizeof(VALUE_LIST*) * auto_list_cnt);
  SWITCH_KEY** key_list =
      (SWITCH_KEY**)malloc(sizeof(SWITCH_KEY*) * auto_list_cnt);

#pragma omp parallel for
  for (size_t j = 0; j < auto_list_cnt; j++) {
    uint32_t auto_idx = Get_ui32_value_at(auto_list, j);
    // generate precomputed order for automorphism
    VALUE_LIST* precomp = Alloc_value_list(I64_TYPE, degree);
    Precompute_automorphism_order(precomp, auto_idx, degree, TRUE);
    precomp_list[j] = precomp;

    // generate rot key
    SWITCH_KEY* key = Alloc_switch_key();
    Generate_rot_key(key, generator, auto_idx, TRUE);
    key_list[j] = key;
  }

  for (size_t j = 0; j < auto_list_cnt; j++) {
    uint32_t auto_idx = Get_ui32_value_at(auto_list, j);
    Insert_precomp_auto_order(generator, auto_idx, precomp_list[j]);
    Insert_auto_key(generator, auto_idx, key_list[j]);
  }

  free(precomp_list);
  free(key_list);
  Free_value_list(auto_list);
}