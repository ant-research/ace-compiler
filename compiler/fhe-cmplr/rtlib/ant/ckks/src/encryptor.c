//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "ckks/encryptor.h"

#include "ckks/ciphertext.h"
#include "ckks/param.h"
#include "ckks/plaintext.h"
#include "ckks/public_key.h"
#include "ckks/secret_key.h"
#include "common/trace.h"
#include "util/random_sample.h"

void Encrypt_msg(CIPHERTEXT* res, CKKS_ENCRYPTOR* encryptor, PLAINTEXT* plain) {
  POLYNOMIAL* poly = Get_plain_poly(plain);
  FMT_ASSERT(Num_p(poly) == 0, "Encrypt_msg: plain should not be extended");
  PUBLIC_KEY* pub_key = encryptor->_public_key;
  POLYNOMIAL* pk0     = Get_pk0(pub_key);
  POLYNOMIAL* pk1     = Get_pk1(pub_key);
  Init_ciphertext_from_poly(res, poly, plain->_scaling_factor,
                            plain->_sf_degree, Get_plain_slots(plain));

  POLYNOMIAL* c0         = Get_c0(res);
  POLYNOMIAL* c1         = Get_c1(res);
  uint32_t    degree     = encryptor->_params->_poly_degree;
  size_t      num_primes = poly->_num_primes;

  POLYNOMIAL random_vec, error1, error2;
  Alloc_poly_data(&random_vec, degree, num_primes, 0);
  Alloc_poly_data(&error1, degree, num_primes, 0);
  Alloc_poly_data(&error2, degree, num_primes, 0);
  VALUE_LIST* samples = Alloc_value_list(I64_TYPE, degree);
  Sample_triangle(samples);
  Transform_value_to_rns_poly(&random_vec, samples, true);
  Sample_triangle(samples);
  Transform_value_to_rns_poly(&error1, samples, true);
  Sample_triangle(samples);
  Transform_value_to_rns_poly(&error2, samples, true);

  size_t saved_level = Save_poly_level(pk0, Poly_level(c0));
  Mul_poly(c0, pk0, &random_vec);
  Restore_poly_level(pk0, saved_level);
  IS_TRACE("encrypt p0 * random_vec:");
  IS_TRACE_CMD(Print_poly(T_FILE, c0));
  IS_TRACE(S_BAR);

  Add_poly(c0, &error1, c0);
  IS_TRACE("encrypt error1 + c0:");
  IS_TRACE_CMD(Print_poly(T_FILE, c0));
  IS_TRACE(S_BAR);

  Add_poly(c0, c0, &(plain->_poly));
  IS_TRACE("encrypt c0 + plain:");
  IS_TRACE_CMD(Print_poly(T_FILE, c0));
  IS_TRACE(S_BAR);

#ifndef OPENFHE_COMPAT
  Set_is_ntt(pk1, TRUE);  // skip ntt convert
#endif
  saved_level = Save_poly_level(pk1, Poly_level(c1));
  Mul_poly(c1, pk1, &random_vec);
  Restore_poly_level(pk1, saved_level);

#ifndef OPENFHE_COMPAT
  Set_is_ntt(pk1, FALSE);
#endif
  IS_TRACE("encrypt p1 * random_vec:");
  IS_TRACE_CMD(Print_poly(T_FILE, c1));
  IS_TRACE(S_BAR);

  Add_poly(c1, &error2, c1);
  IS_TRACE("encrypt error2 + c1:");
  IS_TRACE_CMD(Print_poly(T_FILE, c1));
  IS_TRACE(S_BAR);

  Free_poly_data(&random_vec);
  Free_poly_data(&error1);
  Free_poly_data(&error2);
  Free_value_list(samples);

  IS_TRACE("ciphertext:");
  IS_TRACE_CMD(Print_ciph(Get_trace_file(), res));
  IS_TRACE(S_BAR);
}
