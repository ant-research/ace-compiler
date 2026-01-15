//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "ckks/encoder.h"
#include "ckks/param.h"
#include "ckks/plaintext.h"
#include "common/rt_api.h"
#include "context/ckks_context.h"
#include "gtest/gtest.h"
#include "helper.h"
#include "util/random_sample.h"

namespace {
class TEST_CKKS_ENCODER : public ::testing::Test {
protected:
  void SetUp() override {
    _degree = 32;
    _params = Alloc_ckks_parameter();
    Set_global_params((PTR_TY)_params);
    Init_ckks_parameters_with_prime_size(_params, _degree, HE_STD_NOT_SET, 4, 4,
                                         60, 54, 0);
    _encoder = Alloc_ckks_encoder(_params);
    Set_global_encoder((PTR_TY)_encoder);
  }

  void TearDown() override { Finalize_context(); }

  size_t   Get_degree() { return _degree; }
  size_t   Get_p_cnt() { return _params->_num_p_primes; }
  double   Get_sc() { return _params->_scaling_factor; }
  uint32_t Get_bit_diff() {
    return _params->_first_mod_size - (uint32_t)log2(Get_sc());
  };

  /**
   * @brief Checks that encode and decode are inverses.
   *
   * @param vec Vector of complex numbers to encode.
   */
  void Run_test_encode_decode(VALUE_LIST* vec) {
    size_t     vec_len = LIST_LEN(vec);
    PLAINTEXT* plain   = Alloc_plaintext();
    ENCODE(plain, _encoder, vec);
    VALUE_LIST* value = Alloc_value_list(DCMPLX_TYPE, vec_len);
    Decode(value, _encoder, plain);
    Check_complex_vector_approx_eq(vec, value, 0.1);
    Free_plaintext(plain);
    Free_value_list(value);
  }

  /**
   * @brief Checks that encode and decode with slots are inverses.
   *
   * @param vec Vector of complex numbers to encode.
   * @param slots
   */
  void Run_test_encode_decode_with_slots(VALUE_LIST* vec, uint32_t slots) {
    size_t     vec_len = LIST_LEN(vec);
    PLAINTEXT* plain   = Alloc_plaintext();
    ENCODE(plain, _encoder, vec, slots);
    VALUE_LIST* value = Alloc_value_list(DCMPLX_TYPE, vec_len);
    Decode(value, _encoder, plain);
    Check_complex_vector_approx_eq(vec, value, 0.1);
    Free_plaintext(plain);
    Free_value_list(value);
  }

  void Run_test_encode_decode_at_level(VALUE_LIST* vec, uint32_t level,
                                       uint32_t slots) {
    size_t     vec_len = LIST_LEN(vec);
    PLAINTEXT* plain   = Alloc_plaintext();
    ENCODE_AT_LEVEL(plain, _encoder, vec, level, slots);
    VALUE_LIST* value = Alloc_value_list(DCMPLX_TYPE, vec_len);
    Decode(value, _encoder, plain);
    Check_complex_vector_approx_eq(vec, value, 0.1);
    Free_plaintext(plain);
    Free_value_list(value);
  }

  void Run_test_encode_decode_single_val(double val, uint32_t level,
                                         uint32_t sf_degree) {
    PLAINTEXT* plain = Alloc_plaintext();
    Encode_val_at_level(plain, _encoder, val, level, sf_degree);

    VALUE_LIST* value = Alloc_value_list(DCMPLX_TYPE, 1);
    Decode(value, _encoder, plain);

    VALUE_LIST* expected = Alloc_value_list(DCMPLX_TYPE, 1);
    Set_dcmplx_value(expected, 0, val);
    Check_complex_vector_approx_eq(expected, value, 0.001);
    Free_plaintext(plain);
    Free_value_list(value);
    Free_value_list(expected);
  }

  void Run_test_encode_decode_single_val_with_scale(double val, uint32_t level,
                                                    double scale) {
    PLAINTEXT* plain = Alloc_plaintext();
    Encode_val_at_level_with_scale(plain, _encoder, val, level, scale);

    VALUE_LIST* value = Alloc_value_list(DCMPLX_TYPE, 1);
    Decode(value, _encoder, plain);

    VALUE_LIST* expected = Alloc_value_list(DCMPLX_TYPE, 1);
    Set_dcmplx_value(expected, 0, val);
    Check_complex_vector_approx_eq(expected, value, 0.001);
    Free_plaintext(plain);
    Free_value_list(value);
    Free_value_list(expected);
  }

  void Run_test_encode_decode_at_level_with_sf(VALUE_LIST* vec, uint32_t level,
                                               uint32_t slots,
                                               uint32_t sf_degree) {
    size_t     vec_len = LIST_LEN(vec);
    PLAINTEXT* plain   = Alloc_plaintext();
    Encode_at_level_with_sf(plain, _encoder, vec, level, slots, sf_degree);
    VALUE_LIST* value = Alloc_value_list(DCMPLX_TYPE, vec_len);
    Decode(value, _encoder, plain);
    Check_complex_vector_approx_eq(vec, value, 0.1);
    Free_plaintext(plain);
    Free_value_list(value);
  }

  void Run_test_encode_decode_at_level_with_sf_scale(VALUE_LIST* vec,
                                                     uint32_t    level,
                                                     uint32_t    slots,
                                                     double      scale,
                                                     uint32_t    p_cnt) {
    size_t     vec_len = LIST_LEN(vec);
    PLAINTEXT* plain   = Alloc_plaintext();
    Encode_at_level_with_scale(plain, _encoder, vec, level, slots, scale,
                               p_cnt);
    VALUE_LIST* value = Alloc_value_list(DCMPLX_TYPE, vec_len);
    Decode(value, _encoder, plain);
    Check_complex_vector_approx_eq(vec, value, 0.1);
    Free_plaintext(plain);
    Free_value_list(value);
  }

  void Run_test_encode_decode_ext_at_level(VALUE_LIST* vec, uint32_t level,
                                           uint32_t slots, uint32_t p_cnt) {
    size_t     vec_len = LIST_LEN(vec);
    PLAINTEXT* plain   = Alloc_plaintext();
    Encode_ext_at_level(plain, _encoder, vec, level, slots, p_cnt);
    VALUE_LIST* value = Alloc_value_list(DCMPLX_TYPE, vec_len);
    Decode(value, _encoder, plain);
    Check_complex_vector_approx_eq(vec, value, 0.1);
    Free_plaintext(plain);
    Free_value_list(value);
  }

private:
  CKKS_PARAMETER* _params;
  CKKS_ENCODER*   _encoder;
  size_t          _degree;
};

TEST_F(TEST_CKKS_ENCODER, test_encode_decode_01) {
  size_t      len = Get_degree() / 2;
  VALUE_LIST* vec = Alloc_value_list(DCMPLX_TYPE, len);
  Sample_random_complex_vector(DCMPLX_VALUES(vec), len);
  Run_test_encode_decode(vec);
  Free_value_list(vec);
}

TEST_F(TEST_CKKS_ENCODER, test_encode_decode_02) {
  uint32_t    slots = 4;
  VALUE_LIST* vec   = Alloc_value_list(DCMPLX_TYPE, slots);
  Sample_random_complex_vector(DCMPLX_VALUES(vec), slots);
  Run_test_encode_decode_with_slots(vec, slots);
  Free_value_list(vec);
}

TEST_F(TEST_CKKS_ENCODER, test_encode_decode_03) {
  uint32_t    slots = 8;
  VALUE_LIST* vec   = Alloc_value_list(DCMPLX_TYPE, 5);
  Sample_random_complex_vector(DCMPLX_VALUES(vec), 5);
  Run_test_encode_decode_with_slots(vec, slots);
  Free_value_list(vec);
}

TEST_F(TEST_CKKS_ENCODER, test_encode_decode_04) {
  uint32_t    slots = 8;
  uint32_t    level = 2;
  VALUE_LIST* vec   = Alloc_value_list(DCMPLX_TYPE, 5);
  Sample_random_complex_vector(DCMPLX_VALUES(vec), 5);
  Run_test_encode_decode_at_level(vec, level, slots);
  Free_value_list(vec);
}

TEST_F(TEST_CKKS_ENCODER, test_encode_decode_05) {
  uint32_t    slots = 8;
  uint32_t    level = 2;
  uint32_t    p_cnt = 2;
  VALUE_LIST* vec   = Alloc_value_list(DCMPLX_TYPE, 5);
  Sample_random_complex_vector(DCMPLX_VALUES(vec), 5);
  Run_test_encode_decode_ext_at_level(vec, level, slots, p_cnt);
  Free_value_list(vec);
}

TEST_F(TEST_CKKS_ENCODER, test_encode_decode_06) {
  uint32_t    slots     = 8;
  uint32_t    level     = 2;
  uint32_t    sf_degree = 2;
  VALUE_LIST* vec       = Alloc_value_list(DCMPLX_TYPE, 5);
  Sample_random_complex_vector(DCMPLX_VALUES(vec), 5);
  Run_test_encode_decode_at_level_with_sf(vec, level, slots, sf_degree);
  Free_value_list(vec);
}

TEST_F(TEST_CKKS_ENCODER, test_encode_decode_07) {
  uint32_t level     = 3;
  uint32_t sf_degree = 1;
  // encode with a large value exceed MAX_BITS_IN_WORD * 2
  double val = pow(10, 22);
  Run_test_encode_decode_single_val(val, level, sf_degree);
}

TEST_F(TEST_CKKS_ENCODER, test_encode_decode_07_1) {
  uint32_t level     = 4;
  uint32_t sf_degree = 2;
  // encode with a large value exceed MAX_BITS_IN_WORD * 2
  double val = pow(10, 22);
  Run_test_encode_decode_single_val(val, level, sf_degree);
}

TEST_F(TEST_CKKS_ENCODER, test_encode_decode_08) {
  uint32_t level     = 2;
  uint32_t sf_degree = 1;
  // encode with a large value exceed MAX_BITS_IN_WORD * 2
  double val = pow(10, 22);
  // the value exceed two level's range, should report encode failure
  EXPECT_DEATH(Run_test_encode_decode_single_val(val, level, sf_degree), "");
  EXPECT_DEATH(Run_test_encode_decode_single_val_with_scale(pow(10, 25), level,
                                                            pow(2, 32)),
               "");
}

TEST_F(TEST_CKKS_ENCODER, test_encode_decode_09) {
  uint32_t level = 3;
  double   scale = pow(2, 30);
  Run_test_encode_decode_single_val_with_scale(0.5, level, scale);
  Run_test_encode_decode_single_val_with_scale(pow(10, 10), level, scale);
  Run_test_encode_decode_single_val_with_scale(-pow(10, 10), level, scale);
  Run_test_encode_decode_single_val_with_scale(pow(10, 30), level, scale);
}

TEST_F(TEST_CKKS_ENCODER, test_encode_decode_10) {
  uint32_t    slots = 8;
  uint32_t    level = 2;
  double      scale = pow(2.0, 40);
  VALUE_LIST* vec   = Alloc_value_list(DCMPLX_TYPE, slots);
  Sample_random_complex_vector(DCMPLX_VALUES(vec), slots);
  Run_test_encode_decode_at_level_with_sf_scale(vec, level, slots, scale, 0);
  Run_test_encode_decode_at_level_with_sf_scale(vec, level, slots, scale,
                                                Get_p_cnt());

  scale = pow(2.0, 55);
  Run_test_encode_decode_at_level_with_sf_scale(vec, level, slots, scale, 0);
  Run_test_encode_decode_at_level_with_sf_scale(vec, level, slots, scale,
                                                Get_p_cnt());
  Free_value_list(vec);
}

TEST_F(TEST_CKKS_ENCODER, test_encode_decode_11) {
  uint32_t slots     = 8;
  uint32_t level     = 1;
  uint32_t sf_degree = 1;
  double   scale     = Get_sc();

  VALUE_LIST* vec = Alloc_value_list(DCMPLX_TYPE, 2);
  // encode with a value out of range specified by double(q0 - scaling_factor)
  // & embedding_inv, which exceed value range, should report encode failure
  uint32_t max_bit        = Get_bit_diff() + log2(slots) - 1;
  double   val            = pow(2, max_bit);
  DCMPLX_VALUE_AT(vec, 0) = val;
  EXPECT_DEATH(
      Run_test_encode_decode_at_level_with_sf(vec, level, slots, sf_degree),
      "");
  EXPECT_DEATH(Run_test_encode_decode_at_level_with_sf_scale(vec, level, slots,
                                                             scale, 0),
               "");

  // encode with val = val - 1, which is within value range
  val -= 1;
  DCMPLX_VALUE_AT(vec, 0) = val;
  Run_test_encode_decode_at_level_with_sf(vec, level, slots, sf_degree);
  Run_test_encode_decode_at_level_with_sf_scale(vec, level, slots, scale, 0);

  // encode with value in bounds
  DCMPLX_VALUE_AT(vec, 0) = 1;
  DCMPLX_VALUE_AT(vec, 1) = val - 1;
  Run_test_encode_decode_at_level_with_sf(vec, level, slots, sf_degree);
  Run_test_encode_decode_at_level_with_sf_scale(vec, level, slots, scale, 0);

  if (Get_bit_diff() > 1) {
    // change scale and encode with value in bounds
    scale *= 2;
    val /= 2;
    DCMPLX_VALUE_AT(vec, 0) = val;
    DCMPLX_VALUE_AT(vec, 1) = 0;
    Run_test_encode_decode_at_level_with_sf_scale(vec, level, slots, scale, 0);

    // encode with val = val + 1, which exceed value range, should report encode
    // failure
    val += 1;
    DCMPLX_VALUE_AT(vec, 0) = val;
    DCMPLX_VALUE_AT(vec, 1) = 0;
    EXPECT_DEATH(Run_test_encode_decode_at_level_with_sf_scale(vec, level,
                                                               slots, scale, 0),
                 "");
  }
  Free_value_list(vec);
}

TEST_F(TEST_CKKS_ENCODER, test_encode_decode_12) {
  uint32_t level     = 1;
  uint32_t sf_degree = 1;
  double   scale     = Get_sc();

  // encode with a value out of range specified by double(q0 -
  // scaling_factor), which exceed value range, should report encode failure
  uint32_t max_bit = Get_bit_diff() - 1;
  double   val     = pow(2, max_bit);
  EXPECT_DEATH(Run_test_encode_decode_single_val(val, level, sf_degree), "");
  EXPECT_DEATH(Run_test_encode_decode_single_val_with_scale(val, level, scale),
               "");
  // encode with val = val - 1, which is within value range
  val -= 1;
  Run_test_encode_decode_single_val(val, level, sf_degree);
  Run_test_encode_decode_single_val_with_scale(val, level, scale);

  if (Get_bit_diff() > 1) {
    // change scale and encode with value in bounds
    scale *= 2;
    val /= 2;
    Run_test_encode_decode_single_val_with_scale(val, level, scale);

    // encode with val = val + 1, which exceed value range, should report encode
    // failure
    val += 1;
    EXPECT_DEATH(
        Run_test_encode_decode_single_val_with_scale(val, level, scale), "");
  }
}

}  // namespace
