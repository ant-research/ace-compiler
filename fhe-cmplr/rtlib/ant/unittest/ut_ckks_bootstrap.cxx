//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <complex>

#include "common/rt_config.h"
#include "gtest/gtest.h"
#include "helper.h"
#include "util/ckks_bootstrap_context.h"
#include "util/ckks_decryptor.h"
#include "util/ckks_encryptor.h"
#include "util/ckks_parameters.h"
#include "util/crt.h"
#include "util/fhe_types.h"
#include "util/fhe_utils.h"
#include "util/random_sample.h"

class TEST_BOOTSTRAP_METHOD : public ::testing::Test {
protected:
  void SetUp() override {
    _degree = 64;
    _param  = Alloc_ckks_parameter();
    Init_ckks_parameters_with_prime_size(_param, _degree, HE_STD_NOT_SET, 33,
                                         60, 51, 0);
    _keygen    = Alloc_ckks_key_generator(_param, NULL, 0);
    _relin_key = _keygen->_relin_key;
    _encoder   = Alloc_ckks_encoder(_param);
    _encryptor = Alloc_ckks_encryptor(_param, Get_pk(_keygen), Get_sk(_keygen));
    _decryptor = Alloc_ckks_decryptor(_param, Get_sk(_keygen));
    _eval      = Alloc_ckks_evaluator(_param, _encoder, _decryptor, _keygen);
    _bts_ctx   = Get_bts_ctx(_eval);
    // keep the imag part in the bootstrap testing
    Set_rtlib_config(CONF_BTS_CLEAR_IMAG, 0);
  }

  void TearDown() override {
    Free_ckks_parameters(_param);
    Free_ckks_key_generator(_keygen);
    Free_ckks_encoder(_encoder);
    Free_ckks_encryptor(_encryptor);
    Free_ckks_decryptor(_decryptor);
    Free_ckks_evaluator(_eval);
  }

  size_t          Get_degree() { return _degree; }
  CKKS_PARAMETER* Get_param() { return _param; }
  double          Get_q0_scale_factor_ratio();
  void            Run_test_bootstrap_setup(VL_UI32* level_budget, VL_UI32* dim1,
                                           uint32_t num_slots, VL_DCMPLX* exp_coeff2slots,
                                           VL_DCMPLX* exp_slots2coeffs);
  void Run_test_chebshev(VL_DCMPLX* input, VL_DCMPLX* output, VL_DBL* coeffs,
                         double a, double b);
  void Run_test_bootstrap(VALUE_LIST* vec, VL_UI32* level_budget, VL_UI32* dim1,
                          uint32_t raise_level);
  void Run_test_bootstrap_clear_imag(VALUE_LIST* vec, VL_UI32* level_budget,
                                     VL_UI32* dim1, uint32_t raise_level);
  void Run_test_coeff_to_slot(VALUE_LIST* vec, VALUE_LIST* expected);
  void Run_test_slot_to_coeff(VALUE_LIST* vec, VALUE_LIST* expected);
  void Run_key_switch_ext(VALUE_LIST* vec, bool add_first);
  void Run_fast_rotate_ext(VALUE_LIST* vec, int32_t rotation_idx,
                           bool add_first);

private:
  size_t              _degree;
  CKKS_ENCODER*       _encoder;
  CKKS_ENCRYPTOR*     _encryptor;
  CKKS_DECRYPTOR*     _decryptor;
  CKKS_EVALUATOR*     _eval;
  CKKS_BTS_CTX*       _bts_ctx;
  CKKS_PARAMETER*     _param;
  CKKS_KEY_GENERATOR* _keygen;
  SWITCH_KEY*         _relin_key;
};

double TEST_BOOTSTRAP_METHOD::Get_q0_scale_factor_ratio() {
  CRT_CONTEXT* crt_ctx    = _param->_crt_context;
  MODULUS*     mod0       = Get_modulus_head(Get_q_primes(crt_ctx));
  int64_t      q0         = Get_mod_val(mod0);
  double       sf         = _param->_scaling_factor;
  double       q0_sf_rato = pow(2., round(log2((double)q0 / sf)));
  return q0_sf_rato;
}

void TEST_BOOTSTRAP_METHOD::Run_test_bootstrap_setup(
    VL_UI32* level_budget, VL_UI32* dim1, uint32_t num_slots,
    VL_VL_VL_DCMPLX* exp_coeff2slots, VL_VL_VL_DCMPLX* exp_slots2coeffs) {
  GTEST_SKIP() << "ring_degree changed for sparse test, will improve later";
  Bootstrap_setup(_bts_ctx, level_budget, dim1, num_slots);
  CKKS_BTS_PRECOM* precom    = Get_bts_precom(_bts_ctx, num_slots);
  VL_VL_PLAIN*     u0_prefft = Get_u0_pre_fft(precom);

  VL_VL_PLAIN* u0_hatprefft = Get_u0hatt_pre_fft(precom);
  FOR_ALL_ELEM(u0_hatprefft, idx) {
    VL_PLAIN*     vl     = Get_vl_value_at(u0_hatprefft, idx);
    VL_VL_DCMPLX* exp_vl = Get_vl_value_at(exp_coeff2slots, idx);
    FOR_ALL_ELEM(vl, idx2) {
      PLAINTEXT*  plain = (PLAINTEXT*)Get_ptr_value_at(vl, idx2);
      VALUE_LIST* res   = Alloc_value_list(DCMPLX_TYPE, 8);
      Decode(res, _encoder, plain);
      Check_complex_vector_approx_eq(res, Get_vl_value_at(exp_vl, idx2));
      Free_value_list(res);
    }
  }

  FOR_ALL_ELEM(u0_prefft, idx) {
    VL_PLAIN*     vl     = Get_vl_value_at(u0_prefft, idx);
    VL_VL_DCMPLX* exp_vl = Get_vl_value_at(exp_slots2coeffs, idx);
    FOR_ALL_ELEM(vl, idx2) {
      PLAINTEXT*  plain = (PLAINTEXT*)Get_ptr_value_at(vl, idx2);
      VALUE_LIST* res   = Alloc_value_list(DCMPLX_TYPE, 8);
      Decode(res, _encoder, plain);
      Check_complex_vector_approx_eq(res, Get_vl_value_at(exp_vl, idx2));
      Free_value_list(res);
    }
  }
}

void TEST_BOOTSTRAP_METHOD::Run_test_coeff_to_slot(VALUE_LIST* vec,
                                                   VALUE_LIST* expected) {
  GTEST_SKIP() << "ring_degree changed for sparse test, will improve later";
  uint32_t    slots      = LIST_LEN(vec);
  PLAINTEXT*  plain      = Alloc_plaintext();
  CIPHERTEXT* ciph_input = Alloc_ciphertext();
  CIPHERTEXT* ciph_res   = Alloc_ciphertext();
  PLAINTEXT*  decrypted  = Alloc_plaintext();
  VALUE_LIST* decoded    = Alloc_value_list(DCMPLX_TYPE, slots);

  // step 1: set parmeters
  VL_UI32* level_budget          = Alloc_value_list(UI32_TYPE, 2);
  VL_UI32* dim1                  = Alloc_value_list(UI32_TYPE, 2);
  UI32_VALUE_AT(level_budget, 0) = 3;
  UI32_VALUE_AT(level_budget, 1) = 3;
  UI32_VALUE_AT(dim1, 0)         = 0;
  UI32_VALUE_AT(dim1, 1)         = 0;
  uint32_t num_slots             = _degree / 2;  // full packed

  // step 2: bootstrap setup
  Bootstrap_setup(_bts_ctx, level_budget, dim1, num_slots);

  // step 2: bootstrap keygen
  Bootstrap_keygen(_bts_ctx, num_slots);

  // setp 3: coeffs_to_slots
  uint32_t level_0 = Get_mult_depth(_param) + 1;
  Encode_at_level_internal(plain, _encoder, vec, level_0, num_slots);
  Encrypt_msg(ciph_input, _encryptor, plain);

  CKKS_BTS_PRECOM* precom   = Get_bts_precom(_bts_ctx, slots);
  VL_VL_PLAIN*     conj_pre = Get_u0hatt_pre_fft(precom);
  Coeffs_to_slots(ciph_res, ciph_input, conj_pre, _bts_ctx);

  Decrypt(decrypted, _decryptor, ciph_res, NULL);
  Decode(decoded, _encoder, decrypted);
  Check_complex_vector_approx_eq(vec, expected, 0.001);
  Free_plaintext(plain);
  Free_plaintext(decrypted);
  Free_value_list(decoded);
  Free_ciphertext(ciph_input);
  Free_ciphertext(ciph_res);
  Free_value_list(level_budget);
  Free_value_list(dim1);
}

void TEST_BOOTSTRAP_METHOD::Run_test_slot_to_coeff(VALUE_LIST* vec,
                                                   VALUE_LIST* expected) {
  GTEST_SKIP() << "ring_degree changed for sparse test, will improve later";
  uint32_t    slots      = LIST_LEN(vec);
  PLAINTEXT*  plain      = Alloc_plaintext();
  CIPHERTEXT* ciph_input = Alloc_ciphertext();
  CIPHERTEXT* ciph_res   = Alloc_ciphertext();
  PLAINTEXT*  decrypted  = Alloc_plaintext();
  VALUE_LIST* decoded    = Alloc_value_list(DCMPLX_TYPE, slots);

  // step 1: set parmeters
  VL_UI32* level_budget          = Alloc_value_list(UI32_TYPE, 2);
  VL_UI32* dim1                  = Alloc_value_list(UI32_TYPE, 2);
  UI32_VALUE_AT(level_budget, 0) = 3;
  UI32_VALUE_AT(level_budget, 1) = 3;
  UI32_VALUE_AT(dim1, 0)         = 0;
  UI32_VALUE_AT(dim1, 1)         = 0;
  uint32_t num_slots             = _degree / 2;  // full packed

  // step 2: bootstrap setup
  Bootstrap_setup(_bts_ctx, level_budget, dim1, num_slots);

  // step 2: bootstrap keygen
  Bootstrap_keygen(_bts_ctx, num_slots);

  // setp 3: slots_to_coeffs
  uint32_t level_consume =
      Get_bootstrap_depth(level_budget, 1, _param->_hamming_weight) -
      UI32_VALUE_AT(level_budget, 1);
  size_t level = Get_primes_cnt(Get_q(Get_param_crt(_param))) - level_consume;
  Encode_at_level_internal(plain, _encoder, vec, level, slots);
  Encrypt_msg(ciph_input, _encryptor, plain);

  CKKS_BTS_PRECOM* precom   = Get_bts_precom(_bts_ctx, slots);
  VL_VL_PLAIN*     conj_pre = Get_u0_pre_fft(precom);
  Slots_to_coeffs(ciph_res, ciph_input, conj_pre, _bts_ctx);

  Decrypt(decrypted, _decryptor, ciph_res, NULL);
  Decode(decoded, _encoder, decrypted);
  Check_complex_vector_approx_eq(decoded, expected, 0.001);
  Free_plaintext(plain);
  Free_plaintext(decrypted);
  Free_value_list(decoded);
  Free_ciphertext(ciph_input);
  Free_ciphertext(ciph_res);
  Free_value_list(level_budget);
  Free_value_list(dim1);
}

void TEST_BOOTSTRAP_METHOD::Run_test_bootstrap(VALUE_LIST* vec,
                                               VL_UI32*    level_budget,
                                               VL_UI32*    dim1,
                                               uint32_t    raise_level) {
  size_t len = LIST_LEN(vec);

  PLAINTEXT*  plain           = Alloc_plaintext();
  CIPHERTEXT* ciph            = Alloc_ciphertext();
  CIPHERTEXT* res_ciph        = Alloc_ciphertext();
  PLAINTEXT*  decrypted_plain = Alloc_plaintext();
  VALUE_LIST* decoded_val     = Alloc_value_list(DCMPLX_TYPE, len);

  // encode plain at level = 2
  size_t init_level = 2;
  std::cout << "number of levels before bootstrapping: " << init_level - 1
            << std::endl;
  Encode_at_level_internal(plain, _encoder, vec, init_level, len);

  Encrypt_msg(ciph, _encryptor, plain);

  // step 1: set parmeters
  uint32_t num_slots     = len;
  uint32_t num_iteration = 1;
  uint32_t precision     = 0;

  // step 2: bootstrap setup
  Bootstrap_setup(_bts_ctx, level_budget, dim1, num_slots);

  // step 2: bootstrap keygen
  Bootstrap_keygen(_bts_ctx, num_slots);

  // step 3: call bootstrap driver
  Eval_bootstrap(res_ciph, ciph, num_iteration, precision, raise_level,
                 _bts_ctx);

  std::cout << "number of levels remaining after bootstrapping: "
            << Get_poly_level(Get_c0(res_ciph)) << std::endl;

  Decrypt(decrypted_plain, _decryptor, res_ciph, NULL);
  Decode(decoded_val, _encoder, decrypted_plain);

  Check_complex_vector_approx_eq(vec, decoded_val, 0.001);

  Free_plaintext(plain);
  Free_ciphertext(ciph);
  Free_ciphertext(res_ciph);
  Free_plaintext(decrypted_plain);
  Free_value_list(decoded_val);
}

void TEST_BOOTSTRAP_METHOD::Run_test_bootstrap_clear_imag(
    VALUE_LIST* vec, VL_UI32* level_budget, VL_UI32* dim1,
    uint32_t raise_level) {
  Set_rtlib_config(CONF_BTS_CLEAR_IMAG, 1);
  size_t len = LIST_LEN(vec);

  PLAINTEXT*  plain           = Alloc_plaintext();
  CIPHERTEXT* ciph            = Alloc_ciphertext();
  CIPHERTEXT* res_ciph        = Alloc_ciphertext();
  PLAINTEXT*  decrypted_plain = Alloc_plaintext();
  VALUE_LIST* decoded_val     = Alloc_value_list(DCMPLX_TYPE, len);

  // encode plain at level = 2
  size_t init_level = 2;
  std::cout << "number of levels before bootstrapping: " << init_level - 1
            << std::endl;
  Encode_at_level_internal(plain, _encoder, vec, init_level, len);

  Encrypt_msg(ciph, _encryptor, plain);

  // step 1: set parmeters
  uint32_t num_slots     = len;
  uint32_t num_iteration = 1;
  uint32_t precision     = 0;

  // step 2: bootstrap setup
  Bootstrap_setup(_bts_ctx, level_budget, dim1, num_slots);

  // step 2: bootstrap keygen
  Bootstrap_keygen(_bts_ctx, num_slots);

  // step 3: call bootstrap driver
  Eval_bootstrap(res_ciph, ciph, num_iteration, precision, raise_level,
                 _bts_ctx);

  std::cout << "number of levels remaining after bootstrapping: "
            << Get_poly_level(Get_c0(res_ciph)) << std::endl;

  Decrypt(decrypted_plain, _decryptor, res_ciph, NULL);
  Decode(decoded_val, _encoder, decrypted_plain);

  // clear imag part
  VALUE_LIST* expected = Alloc_value_list(DCMPLX_TYPE, len);
  FOR_ALL_ELEM(vec, idx) {
    DCMPLX elem                    = DCMPLX_VALUE_AT(vec, idx);
    DCMPLX_VALUE_AT(expected, idx) = elem.real();
  }

  Check_complex_vector_approx_eq(expected, decoded_val, 0.001);

  Free_plaintext(plain);
  Free_ciphertext(ciph);
  Free_ciphertext(res_ciph);
  Free_plaintext(decrypted_plain);
  Free_value_list(decoded_val);
}

void TEST_BOOTSTRAP_METHOD::Run_test_chebshev(VL_DCMPLX* input,
                                              VL_DCMPLX* output, VL_DBL* coeffs,
                                              double a, double b) {
  PLAINTEXT*  plain     = Alloc_plaintext();
  PLAINTEXT*  out_plain = Alloc_plaintext();
  CIPHERTEXT* ciph      = Alloc_ciphertext();
  CIPHERTEXT* out_ciph  = Alloc_ciphertext();
  VL_DCMPLX*  decoded   = Alloc_value_list(DCMPLX_TYPE, LIST_LEN(output));
  ENCODE(plain, _encoder, input);
  Encrypt_msg(ciph, _encryptor, plain);

  Eval_chebyshev(_bts_ctx, out_ciph, ciph, coeffs, a, b);

  Decrypt(out_plain, _decryptor, out_ciph, NULL);
  Decode(decoded, _encoder, out_plain);

  Check_complex_vector_approx_eq(decoded, output, 0.001);

  Free_plaintext(plain);
  Free_plaintext(out_plain);
  Free_ciphertext(ciph);
  Free_ciphertext(out_ciph);
  Free_value_list(decoded);
}

void TEST_BOOTSTRAP_METHOD::Run_key_switch_ext(VALUE_LIST* vec,
                                               bool        add_first) {
  PLAINTEXT*   plain       = Alloc_plaintext();
  PLAINTEXT*   out_plain   = Alloc_plaintext();
  CIPHERTEXT*  ciph        = Alloc_ciphertext();
  CIPHERTEXT*  switch_ciph = Alloc_ciphertext();
  CIPHERTEXT*  out_ciph    = Alloc_ciphertext();
  VL_DCMPLX*   decoded     = Alloc_value_list(DCMPLX_TYPE, LIST_LEN(vec));
  CRT_CONTEXT* crt         = _param->_crt_context;

  ENCODE(plain, _encoder, vec);
  Encrypt_msg(ciph, _encryptor, plain);

  // Step1: switch from ciph(Q) to switch_ciph(P*Q)
  Switch_key_ext(switch_ciph, ciph, _eval, add_first);
  Init_ciphertext_from_ciph(out_ciph, ciph, Get_ciph_sfactor(ciph),
                            Get_ciph_sf_degree(ciph));
  // Step2: reduce back from switch_ciph(P*Q) to out_ciph(Q)
  Reduce_rns_base(Get_c0(out_ciph), Get_c0(switch_ciph), crt);
  Reduce_rns_base(Get_c1(out_ciph), Get_c1(switch_ciph), crt);

  // Step3: if switch_key_ext is executed without adding first element,
  // add ciph->c0 after reducing back to Q
  if (!add_first) {
    Add_poly(Get_c0(out_ciph), Get_c0(out_ciph), Get_c0(ciph), crt, NULL);
  }

  Decrypt(out_plain, _decryptor, out_ciph, NULL);
  Decode(decoded, _encoder, out_plain);

  Check_complex_vector_approx_eq(decoded, vec, 0.00000001);

  Free_plaintext(plain);
  Free_plaintext(out_plain);
  Free_ciphertext(ciph);
  Free_ciphertext(switch_ciph);
  Free_ciphertext(out_ciph);
  Free_value_list(decoded);
}

void TEST_BOOTSTRAP_METHOD::Run_fast_rotate_ext(VALUE_LIST* vec,
                                                int32_t     rotation_idx,
                                                bool        add_first) {
  PLAINTEXT*   plain     = Alloc_plaintext();
  PLAINTEXT*   out_plain = Alloc_plaintext();
  CIPHERTEXT*  ciph      = Alloc_ciphertext();
  SWITCH_KEY*  rot_key   = Alloc_switch_key();
  CIPHERTEXT*  rot_ciph  = Alloc_ciphertext();
  CIPHERTEXT*  out_ciph  = Alloc_ciphertext();
  CRT_CONTEXT* crt       = _param->_crt_context;

  size_t      len     = LIST_LEN(vec);
  VL_DCMPLX*  decoded = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* rot_msg = Alloc_value_list(DCMPLX_TYPE, len);
  for (size_t idx = 0; idx < LIST_LEN(vec); idx++) {
    DCMPLX_VALUE_AT(rot_msg, idx) =
        Get_dcmplx_value_at(vec, (idx + rotation_idx) % len);
  }

  // Encode vec at slots = len
  Encode_at_level_internal(plain, _encoder, vec, 0, len);
  Encrypt_msg(ciph, _encryptor, plain);

  // Step1: generate rot key
  Insert_rot_map(_keygen, rotation_idx);
  uint32_t auto_idx = Get_precomp_auto_idx(_keygen, rotation_idx);
  IS_TRUE(auto_idx, "cannot get precompute automorphism index");
  rot_key = Get_auto_key(_keygen, auto_idx);
  IS_TRUE(rot_key, "cannot find auto key");

  // Step2: decompose & mod up
  VALUE_LIST* precomputed = Switch_key_precompute(Get_c1(ciph), crt);

  // Step3: fast rotate without mod reduce(only key switch & rotate)
  //        and return the results in the extended CRT basis P*Q
  Fast_rotate_ext(rot_ciph, ciph, rotation_idx, rot_key, _eval, precomputed,
                  add_first);

  // Step4: reduce back from rot_ciph(P*Q) to out_ciph(Q)
  Init_ciphertext_from_ciph(out_ciph, ciph, Get_ciph_sfactor(ciph),
                            Get_ciph_sf_degree(ciph));
  Reduce_rns_base(Get_c0(out_ciph), Get_c0(rot_ciph), crt);
  Reduce_rns_base(Get_c1(out_ciph), Get_c1(rot_ciph), crt);

  // Step5: if Fast_rotate_ext is executed without adding first element,
  // add rot(Get_c0(ciph)) after reducing back to Q
  if (!add_first) {
    VALUE_LIST* precomp_auto = Get_precomp_auto_order(_keygen, auto_idx);
    POLYNOMIAL  c0;
    Alloc_poly_data(&c0, Get_c0(ciph)->_ring_degree, Get_ciph_prime_cnt(ciph),
                    0);
    Rotate_poly(&c0, Get_c0(ciph), auto_idx, precomp_auto, crt);
    Add_poly(Get_c0(out_ciph), Get_c0(out_ciph), &c0, crt, NULL);
    Free_poly_data(&c0);
  }

  Decrypt(out_plain, _decryptor, out_ciph, NULL);
  Decode(decoded, _encoder, out_plain);

  Check_complex_vector_approx_eq(decoded, rot_msg, 0.00000001);

  Free_plaintext(plain);
  Free_plaintext(out_plain);
  Free_ciphertext(ciph);
  Free_ciphertext(rot_ciph);
  Free_ciphertext(out_ciph);
  Free_switch_key_precomputed(precomputed);
  Free_value_list(decoded);
  Free_value_list(rot_msg);
}

TEST_F(TEST_BOOTSTRAP_METHOD, test_get_colls_fft_params) {
  VL_I32*  params     = Get_colls_fft_params(8, 3, 0);
  int32_t  expected[] = {3, 1, 0, 3, 2, 2, 1, 0, 0};
  int32_t* vals       = Get_i32_values(params);
  for (size_t idx = 0; idx < 9; idx++) {
    EXPECT_EQ(vals[idx], expected[idx]);
  }
  Free_value_list(params);
}

TEST_F(TEST_BOOTSTRAP_METHOD, test_bootstrap_setup) {
  GTEST_SKIP() << "ring_degree changed for sparse test, will improve later";
  VL_UI32* level_budget          = Alloc_value_list(UI32_TYPE, 2);
  VL_UI32* dim1                  = Alloc_value_list(UI32_TYPE, 2);
  UI32_VALUE_AT(level_budget, 0) = 3;
  UI32_VALUE_AT(level_budget, 1) = 3;
  UI32_VALUE_AT(dim1, 0)         = 0;
  UI32_VALUE_AT(dim1, 1)         = 0;

  uint32_t         num_slots = 8;
  VL_VL_VL_DCMPLX* exp_coeffs2slots =
      Alloc_3d_value_list(DCMPLX_TYPE, 3, 3, num_slots);
  VL_VL_VL_DCMPLX* exp_slots2coeffs =
      Alloc_3d_value_list(DCMPLX_TYPE, 3, 3, num_slots);
  DCMPLX exp1[3][3][8] = {
      {{{0, 0},
        {0.035077, -0.035077},
        {0, 0},
        {0.035077, -0.035077},
        {0, 0},
        {0.035077, -0.035077},
        {0, 0},
        {0.035077, -0.035077}},
       {{0.049606, 0},
        {-0.035077, 0.035077},
        {0.049606, 0},
        {-0.035077, 0.035077},
        {0.049606, 0},
        {-0.035077, 0.035077},
        {0.049606, 0},
        {-0.035077, 0.035077}},
       {{0.049606, 0},
        {0, 0},
        {0.049606, 0},
        {0, 0},
        {0.049606, 0},
        {0, 0},
        {0.049606, 0},
        {0, 0}}},

      {{{0, 0},
        {0, 0},
        {0.045830, -0.018984},
        {-0.018984, -0.045830},
        {0, 0},
        {0, 0},
        {0.045830, -0.018984},
        {-0.018984, -0.045830}},
       {{0.049606, 0},
        {0.049606, 0},
        {-0.045830, 0.018984},
        {0.018984, 0.045830},
        {0.049606, 0},
        {0.049606, 0},
        {-0.045830, 0.018984},
        {0.018984, 0.045830}},
       {{0.049606, 0},
        {0.049606, 0},
        {0, 0},
        {0, 0},
        {0.049606, 0},
        {0.049606, 0},
        {0, 0},
        {0, 0}}},

      {{{0, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0.048653, -0.009678},
        {0.027560, -0.041246},
        {0.009678, 0.048653},
        {0.041246, 0.027560}},
       {{0.049606, 0},
        {0.049606, 0},
        {0.049606, 0},
        {0.049606, 0},
        {-0.048653, 0.009678},
        {-0.027560, 0.041246},
        {-0.009678, -0.048653},
        {-0.041246, -0.027560}},
       {{0.049606, 0},
        {0.049606, 0},
        {0.049606, 0},
        {0.049606, 0},
        {0, 0},
        {0, 0},
        {0, 0},
        {0, 0}}}
  };

  DCMPLX exp2[3][3][8] = {
      {{{0, 0}, {1, 0}, {0, 0}, {1, 0}, {0, 0}, {1, 0}, {0, 0}, {1, 0}},
       {{1, 0},
        {-0.707107, -0.707107},
        {1, 0},
        {-0.707107, -0.707107},
        {1, 0},
        {-0.707107, -0.707107},
        {1, 0},
        {-0.707107, -0.707107}},
       {{0.707107, 0.707107},
        {0, 0},
        {0.707107, 0.707107},
        {0, 0},
        {0.707107, 0.707107},
        {0, 0},
        {0.707107, 0.707107}}},

      {{{0, 0}, {0, 0}, {1, 0}, {1, 0}, {0, 0}, {0, 0}, {1, 0}, {1, 0}},
       {{1, 0},
        {1, 0},
        {-0.92388, -0.382683},
        {0.382683, -0.92388},
        {1, 0},
        {1, 0},
        {-0.92388, -0.382683},
        {0.382683, -0.92388}},
       {{0.92388, 0.382683},
        {-0.382683, 0.92388},
        {0, 0},
        {0, 0},
        {0.92388, 0.382683},
        {-0.382683, 0.92388}}},

      {{{0, 0}, {0, 0}, {0, 0}, {0, 0}, {1, 0}, {1, 0}, {1, 0}, {1, 0}},
       {{1, 0},
        {1, 0},
        {1, 0},
        {1, 0},
        {-0.980785, -0.19509},
        {-0.55557, -0.83147},
        {-0.19509, 0.980785},
        {-0.83147, 0.55557}},
       {{0.980785, 0.19509},
        {0.55557, 0.83147},
        {0.19509, -0.980785},
        {0.83147, -0.55557}} }
  };

  double   q0_sf_ratio     = Get_q0_scale_factor_ratio();
  uint32_t level_of_encode = Get_ui32_value_at(level_budget, 0);
  FOR_ALL_ELEM(exp_coeffs2slots, idx1) {
    VL_VL_DCMPLX* vl1 = Get_vl_value_at(exp_coeffs2slots, idx1);
    FOR_ALL_ELEM(vl1, idx2) {
      VL_DCMPLX* vl2 = Get_vl_value_at(vl1, idx2);
      Init_dcmplx_value_list(vl2, num_slots, exp1[idx1][idx2]);

      FOR_ALL_ELEM(vl2, idx3) {
        DCMPLX cmplx_val = Get_dcmplx_value_at(vl2, idx3);
        cmplx_val *= std::pow(q0_sf_ratio, -1. / level_of_encode);
        Set_dcmplx_value(vl2, idx3, cmplx_val);
      }
    }
  }

  FOR_ALL_ELEM(exp_slots2coeffs, idx1) {
    VL_VL_DCMPLX* vl1 = Get_vl_value_at(exp_slots2coeffs, idx1);
    FOR_ALL_ELEM(vl1, idx2) {
      VL_DCMPLX* vl2 = Get_vl_value_at(vl1, idx2);
      Init_dcmplx_value_list(vl2, num_slots, exp2[idx1][idx2]);
    }
  }

  Run_test_bootstrap_setup(level_budget, dim1, num_slots, exp_coeffs2slots,
                           exp_slots2coeffs);
  Free_value_list(exp_slots2coeffs);
  Free_value_list(exp_coeffs2slots);
  Free_value_list(dim1);
  Free_value_list(level_budget);
}

TEST_F(TEST_BOOTSTRAP_METHOD, test_bootstrap_setup_sparse) {
  VL_UI32* level_budget          = Alloc_value_list(UI32_TYPE, 2);
  VL_UI32* dim1                  = Alloc_value_list(UI32_TYPE, 2);
  UI32_VALUE_AT(level_budget, 0) = 2;
  UI32_VALUE_AT(level_budget, 1) = 2;
  UI32_VALUE_AT(dim1, 0)         = 0;
  UI32_VALUE_AT(dim1, 1)         = 0;

  uint32_t         packed_slots = 8;
  VL_VL_VL_DCMPLX* exp_coeffs2slots =
      Alloc_3d_value_list(DCMPLX_TYPE, 2, 3, packed_slots);
  VL_VL_VL_DCMPLX* exp_slots2coeffs =
      Alloc_3d_value_list(DCMPLX_TYPE, 2, 3, packed_slots);
  DCMPLX exp1[2][3][8] = {
      {{{0, 0},
        {0.007812, -0.007812},
        {0, 0},
        {0.007812, -0.007812},
        {0, 0},
        {-0.007812, -0.007812},
        {0, 0},
        {-0.007812, -0.007812}},
       {{0.011049, 0},
        {-0.007812, 0.007812},
        {0.011049, 0},
        {-0.007812, 0.007812},
        {0, -0.011049},
        {0.007812, 0.007812},
        {0, -0.011049},
        {0.007812, 0.007812}},
       {{0, -0.011049},
        {0, 0},
        {0.011049, 0},
        {0, 0},
        {0.011049, 0},
        {0, 0},
        {0, -0.011049},
        {0, 0}}},

      {{{0, 0},
        {0, 0},
        {0.010208, -0.004228},
        {-0.004228, -0.010208},
        {0, 0},
        {0, 0},
        {0.010208, -0.004228},
        {-0.004228, -0.010208}},
       {{0.011049, 0},
        {0.011049, 0},
        {-0.010208, 0.004228},
        {0.004228, 0.010208},
        {0.011049, 0},
        {0.011049, 0},
        {-0.010208, 0.004228},
        {0.004228, 0.010208}},
       {{0.011049, 0},
        {0.011049, 0},
        {0, 0},
        {0, 0},
        {0.011049, 0},
        {0.011049, 0},
        {0, 0},
        {0, 0}}}
  };

  DCMPLX exp2[2][3][8] = {
      {{{0, 0},
        {1, 0},
        {0, 0},
        {1, 0},
        {0, 0},
        {6.12323e-17, 1},
        {0, 0},
        {6.12323e-17, 1}},
       {{1, 0},
        {-0.707107, -0.707107},
        {1, 0},
        {-0.707107, -0.707107},
        {6.12323e-17, 1},
        {0.707107, -0.707107},
        {6.12323e-17, 1},
        {0.707107, -0.707107}},
       {{-0.707107, 0.707107},
        {0, 0},
        {0.707107, 0.707107},
        {0, 0},
        {0.707107, 0.707107},
        {0, 0},
        {-0.707107, 0.707107}}},

      {{{0, 0}, {0, 0}, {1, 0}, {1, 0}, {0, 0}, {0, 0}, {1, 0}, {1, 0}},
       {{1, 0},
        {1, 0},
        {-0.92388, -0.382683},
        {0.382683, -0.92388},
        {1, 0},
        {1, 0},
        {-0.92388, -0.382683},
        {0.382683, -0.92388}},
       {{0.92388, 0.382683},
        {-0.382683, 0.92388},
        {0, 0},
        {0, 0},
        {0.92388, 0.382683},
        {-0.382683, 0.92388}} }
  };

  double   q0_sf_ratio     = Get_q0_scale_factor_ratio();
  uint32_t level_of_encode = Get_ui32_value_at(level_budget, 0);
  FOR_ALL_ELEM(exp_coeffs2slots, idx1) {
    VL_VL_DCMPLX* vl1 = Get_vl_value_at(exp_coeffs2slots, idx1);
    FOR_ALL_ELEM(vl1, idx2) {
      VL_DCMPLX* vl2 = Get_vl_value_at(vl1, idx2);
      Init_dcmplx_value_list(vl2, packed_slots, exp1[idx1][idx2]);

      FOR_ALL_ELEM(vl2, idx3) {
        DCMPLX cmplx_val = Get_dcmplx_value_at(vl2, idx3);
        cmplx_val *= std::pow(q0_sf_ratio, -1. / level_of_encode);
        Set_dcmplx_value(vl2, idx3, cmplx_val);
      }
    }
  }

  FOR_ALL_ELEM(exp_slots2coeffs, idx1) {
    VL_VL_DCMPLX* vl1 = Get_vl_value_at(exp_slots2coeffs, idx1);
    FOR_ALL_ELEM(vl1, idx2) {
      VL_DCMPLX* vl2 = Get_vl_value_at(vl1, idx2);
      Init_dcmplx_value_list(vl2, packed_slots, exp2[idx1][idx2]);
    }
  }

  uint32_t num_slots = 4;
  Run_test_bootstrap_setup(level_budget, dim1, num_slots, exp_coeffs2slots,
                           exp_slots2coeffs);
  Free_value_list(exp_slots2coeffs);
  Free_value_list(exp_coeffs2slots);
  Free_value_list(dim1);
  Free_value_list(level_budget);
}

TEST_F(TEST_BOOTSTRAP_METHOD, test_coeff_to_slot) {
  size_t      len      = Get_degree() / 2;
  VALUE_LIST* vec      = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* expected = Alloc_value_list(DCMPLX_TYPE, len);
  DCMPLX      in[8]    = {
      {5.960464e-08, 0},
      {1.192093e-07, 0},
      {1.788139e-07, 0},
      {2.384186e-08, 0},
      {4.768372e-08, 0},
      {7.152557e-08, 0},
      {9.536743e-08, 0},
      {1.192093e-07, 0}
  };
  DCMPLX out[8] = {
      {7.152557e-07,  0            },
      {3.371748e-08,  -3.371748e-08},
      {-1.724368e-07, 1.981318e-08 },
      {-1.981318e-08, 1.724368e-07 },
      {-2.483199e-08, -1.311342e-08},
      {1.788437e-07,  6.461073e-08 },
      {-6.461073e-08, -1.788437e-07},
      {1.311342e-08,  2.483199e-08 }
  };
  Init_dcmplx_value_list(vec, len, in);
  Init_dcmplx_value_list(expected, len, out);
  Run_test_coeff_to_slot(vec, expected);
  Free_value_list(vec);
  Free_value_list(expected);
}

TEST_F(TEST_BOOTSTRAP_METHOD, test_slot_to_coeff) {
  size_t      len      = Get_degree() / 2;
  VALUE_LIST* vec      = Alloc_value_list(DCMPLX_TYPE, len);
  VALUE_LIST* expected = Alloc_value_list(DCMPLX_TYPE, len);
  DCMPLX      in[8]    = {
      {1.464839e-03,  5.789057e-11 },
      {6.905345e-05,  -6.905334e-05},
      {-3.531504e-04, 4.057745e-05 },
      {-4.057733e-05, 3.531506e-04 },
      {-5.085586e-05, -2.685623e-05},
      {3.662720e-04,  1.323228e-04 },
      {-1.323227e-04, -3.662719e-04},
      {2.685635e-05,  5.085598e-05 }
  };
  DCMPLX out[8] = {
      {9.765574e-04, 5.884695e-10 },
      {1.953120e-03, 1.084244e-10 },
      {2.929682e-03, -7.066840e-11},
      {3.906201e-04, -1.913389e-10},
      {7.812451e-04, -5.632090e-12},
      {1.171870e-03, -3.082914e-11},
      {1.562495e-03, 4.724082e-11 },
      {1.953120e-03, 1.745843e-11 }
  };
  Init_dcmplx_value_list(vec, len, in);
  Init_dcmplx_value_list(expected, len, out);
  Run_test_slot_to_coeff(vec, expected);
  Free_value_list(vec);
  Free_value_list(expected);
}

TEST_F(TEST_BOOTSTRAP_METHOD, test_bootstrap_01) {
  size_t      len = Get_degree() / 2;
  VALUE_LIST* vec = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec), len);
  // set bootstrap setup parameters
  VL_UI32* level_budget          = Alloc_value_list(UI32_TYPE, 2);
  VL_UI32* dim1                  = Alloc_value_list(UI32_TYPE, 2);
  UI32_VALUE_AT(level_budget, 0) = 3;
  UI32_VALUE_AT(level_budget, 1) = 3;
  UI32_VALUE_AT(dim1, 0)         = 0;
  UI32_VALUE_AT(dim1, 1)         = 0;

  Run_test_bootstrap(vec, level_budget, dim1, 0);
  Free_value_list(vec);
  Free_value_list(level_budget);
  Free_value_list(dim1);
}

// bootstrap with raise_level < q_cnt
TEST_F(TEST_BOOTSTRAP_METHOD, test_bootstrap_02) {
  size_t      len = Get_degree() / 2;
  VALUE_LIST* vec = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec), len);
  // set bootstrap setup parameters
  VL_UI32* level_budget          = Alloc_value_list(UI32_TYPE, 2);
  VL_UI32* dim1                  = Alloc_value_list(UI32_TYPE, 2);
  UI32_VALUE_AT(level_budget, 0) = 3;
  UI32_VALUE_AT(level_budget, 1) = 3;
  UI32_VALUE_AT(dim1, 0)         = 0;
  UI32_VALUE_AT(dim1, 1)         = 0;

  size_t q_cnt = Get_primes_cnt(Get_q(Get_param_crt(Get_param())));

  Run_test_bootstrap(vec, level_budget, dim1, q_cnt - 2);

  Run_test_bootstrap(vec, level_budget, dim1, 0);
  Free_value_list(vec);
  Free_value_list(level_budget);
  Free_value_list(dim1);
}

// bootstrap with clear imag part
TEST_F(TEST_BOOTSTRAP_METHOD, test_bootstrap_03) {
  size_t      len = Get_degree() / 2;
  VALUE_LIST* vec = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec), len);
  // set bootstrap setup parameters
  VL_UI32* level_budget          = Alloc_value_list(UI32_TYPE, 2);
  VL_UI32* dim1                  = Alloc_value_list(UI32_TYPE, 2);
  UI32_VALUE_AT(level_budget, 0) = 3;
  UI32_VALUE_AT(level_budget, 1) = 3;
  UI32_VALUE_AT(dim1, 0)         = 0;
  UI32_VALUE_AT(dim1, 1)         = 0;

  Run_test_bootstrap_clear_imag(vec, level_budget, dim1, 0);
  Free_value_list(vec);
  Free_value_list(level_budget);
  Free_value_list(dim1);
}

TEST_F(TEST_BOOTSTRAP_METHOD, test_bootstrap_sparse_01) {
  size_t      len = Get_degree() / 4;
  VALUE_LIST* vec = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec), len);
  // set bootstrap setup parameters
  VL_UI32* level_budget          = Alloc_value_list(UI32_TYPE, 2);
  VL_UI32* dim1                  = Alloc_value_list(UI32_TYPE, 2);
  UI32_VALUE_AT(level_budget, 0) = 2;
  UI32_VALUE_AT(level_budget, 1) = 2;
  UI32_VALUE_AT(dim1, 0)         = 0;
  UI32_VALUE_AT(dim1, 1)         = 0;

  Run_test_bootstrap(vec, level_budget, dim1, 0);
  Free_value_list(vec);
  Free_value_list(level_budget);
  Free_value_list(dim1);
}

TEST_F(TEST_BOOTSTRAP_METHOD, test_bootstrap_sparse_02) {
  size_t      len = Get_degree() / 8;
  VALUE_LIST* vec = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec), len);
  // set bootstrap setup parameters
  VL_UI32* level_budget          = Alloc_value_list(UI32_TYPE, 2);
  VL_UI32* dim1                  = Alloc_value_list(UI32_TYPE, 2);
  UI32_VALUE_AT(level_budget, 0) = 3;
  UI32_VALUE_AT(level_budget, 1) = 3;
  UI32_VALUE_AT(dim1, 0)         = 0;
  UI32_VALUE_AT(dim1, 1)         = 0;

  size_t q_cnt = Get_primes_cnt(Get_q(Get_param_crt(Get_param())));

  Run_test_bootstrap(vec, level_budget, dim1, q_cnt - 2);

  Run_test_bootstrap(vec, level_budget, dim1, q_cnt);

  Free_value_list(vec);
  Free_value_list(level_budget);
  Free_value_list(dim1);
}

TEST_F(TEST_BOOTSTRAP_METHOD, test_chebyshev) {
  VL_DCMPLX* input       = Alloc_value_list(DCMPLX_TYPE, 7);
  VL_DCMPLX* output      = Alloc_value_list(DCMPLX_TYPE, 7);
  DCMPLX     in_values[] = {
      {-3.0, 0},
      {-2.0, 0},
      {-1.0, 0},
      {0,    0},
      {1.0,  0},
      {2.0,  0},
      {3.0,  0}
  };
  DCMPLX out_values[] = {
      {33,       0},
      {9.51029,  0},
      {0.218107, 0},
      {1,        0},
      {2.20165,  0},
      {-3.46091, 0},
      {-13,      0}
  };
  Init_dcmplx_value_list(input, 7, in_values);
  Init_dcmplx_value_list(output, 7, out_values);

  VL_DBL* coeffs         = Alloc_value_list(DBL_TYPE, 6);
  double  coeff_values[] = {9, -17.25, 4.5, -6.75, 1, 1};
  Init_dbl_value_list(coeffs, 6, coeff_values);
  double a = -3;
  double b = 3;
  Run_test_chebshev(input, output, coeffs, a, b);

  Free_value_list(input);
  Free_value_list(output);
  Free_value_list(coeffs);
}

TEST_F(TEST_BOOTSTRAP_METHOD, test_chebyshev_2) {
  VL_DCMPLX* input       = Alloc_value_list(DCMPLX_TYPE, 8);
  VL_DCMPLX* output      = Alloc_value_list(DCMPLX_TYPE, 8);
  DCMPLX     in_values[] = {
      {1.43051e-06,  0},
      {6.7435e-08,   0},
      {-3.44874e-07, 0},
      {-3.96264e-08, 0},
      {-4.9664e-08,  0},
      {3.57687e-07,  0},
      {-1.29221e-07, 0},
      {2.62268e-08,  0}
  };
  DCMPLX out_values[] = {
      {0.971401, 0},
      {0.971399, 0},
      {0.971399, 0},
      {0.971399, 0},
      {0.971399, 0},
      {0.971399, 0},
      {0.971399, 0},
      {0.971399, 0}
  };
  Init_dcmplx_value_list(input, 8, in_values);
  Init_dcmplx_value_list(output, 8, out_values);

  VL_DBL* coeffs = Alloc_value_list(DBL_TYPE, 89);
  Init_dbl_value_list(coeffs, LIST_LEN(coeffs),
                      (double*)G_coefficients_uniform);
  double a = -1;
  double b = 1;
  Run_test_chebshev(input, output, coeffs, a, b);

  Free_value_list(input);
  Free_value_list(output);
  Free_value_list(coeffs);
}

TEST_F(TEST_BOOTSTRAP_METHOD, test_chebyshev_sine) {
  VL_DCMPLX* input       = Alloc_value_list(DCMPLX_TYPE, 8);
  VL_DCMPLX* output      = Alloc_value_list(DCMPLX_TYPE, 8);
  DCMPLX     in_values[] = {
      {-1,   0},
      {-0.8, 0},
      {-0.6, 0},
      {-0.4, 0},
      {-0.2, 0},
      {0,    0},
      {0.2,  0},
      {0.4,  0}
  };
  DCMPLX out_values[] = {
      {6.80601e-09, 0},
      {0.151365,    0},
      {0.0935489,   0},
      {-0.0935489,  0},
      {-0.151365,   0},
      {0,           0},
      {0.151365,    0},
      {0.0935489,   0}
  };

  Init_dcmplx_value_list(input, 8, in_values);
  Init_dcmplx_value_list(output, 8, out_values);

  double coeff_values[] = {
      0., -0.0178446,  0., -0.0171187,    0., -0.0155856,   0., -0.0131009,
      0., -0.00949759, 0., -0.00465513,   0., 0.00139902,   0., 0.00836141,
      0., 0.0155242,   0., 0.0217022,     0., 0.0253027,    0., 0.0246365,
      0., 0.0185273,   0., 0.00714273,    0., -0.00725482,  0., -0.0201827,
      0., -0.0260483,  0., -0.0207132,    0., -0.00473479,  0., 0.0147661,
      0., 0.0261764,   0., 0.0203168,     0., -0.00103552,  0., -0.0225101,
      0., -0.0248192,  0., -0.00315799,   0., 0.0226844,    0., 0.0238252,
      0., -0.00403513, 0., -0.0276106,    0., -0.0133143,   0., 0.0213882,
      0., 0.0230787,   0., -0.0143638,    0., -0.0270401,   0., 0.0116019,
      0., 0.0278743,   0., -0.0149975,    0., -0.025194,    0., 0.0242296,
      0., 0.0143133,   0., -0.0334779,    0., 0.00994475,   0., 0.0256291,
      0., -0.0359815,  0., 0.0150778,     0., 0.0173112,    0., -0.0403029,
      0., 0.0463332,   0., -0.039547,     0., 0.0277765,    0., -0.0168089,
      0., 0.00899558,  0., -0.00433006,   0., 0.00189728,   0., -0.000763553,
      0., 0.000284227, 0., -0.0000984182, 0., 0.0000318501, 0., -9.67162e-6,
      0., 2.76517e-6,  0., -7.46488e-7,   0., 1.90362e-7,   0., -4.39544e-8,
      0.};
  size_t  coeffs_size = sizeof(coeff_values) / sizeof(coeff_values[0]);
  VL_DBL* coeffs      = Alloc_value_list(DBL_TYPE, coeffs_size);
  Init_dbl_value_list(coeffs, coeffs_size, coeff_values);
  double a = -1;
  double b = 1;
  Run_test_chebshev(input, output, coeffs, a, b);

  Free_value_list(input);
  Free_value_list(output);
  Free_value_list(coeffs);
}

// Test for Switch_key_ext + Reduce_rns_base
TEST_F(TEST_BOOTSTRAP_METHOD, test_key_switch_ext_01) {
  size_t      len = Get_degree() / 2;
  VALUE_LIST* vec = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec), len);
  Run_key_switch_ext(vec, TRUE /* add_first */);
  Free_value_list(vec);
}

// Test for Switch_key_ext(without adding first element) + Reduce_rns_base
TEST_F(TEST_BOOTSTRAP_METHOD, test_key_switch_ext_02) {
  size_t      len = Get_degree() / 2;
  VALUE_LIST* vec = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec), len);
  Run_key_switch_ext(vec, FALSE /* add_first */);
  Free_value_list(vec);
}

// Test for Fast_rotate_ext + Reduce_rns_base
TEST_F(TEST_BOOTSTRAP_METHOD, test_fast_rotate_ext_01) {
  size_t      len = Get_degree() / 2;
  VALUE_LIST* vec = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec), len);
  Run_fast_rotate_ext(vec, 2, TRUE /* add_first */);
  Free_value_list(vec);
}

// Test for Fast_rotate_ext(without adding first element) + Reduce_rns_base
TEST_F(TEST_BOOTSTRAP_METHOD, test_fast_rotate_ext_02) {
  size_t      len = Get_degree() / 2;
  VALUE_LIST* vec = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec), len);
  Run_fast_rotate_ext(vec, -1, FALSE /* add_first */);
  Free_value_list(vec);
}