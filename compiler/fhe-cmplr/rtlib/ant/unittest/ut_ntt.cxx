//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "gtest/gtest.h"
#include "helper.h"
#include "ntt_impl.h"
#include "util/ntt.h"
#include "util/type.h"

const size_t  Coeff_len  = 4;
const int64_t Coeffs1[4] = {0, 1, 2, 3};
// degree = 4 & mod = 4503599626321921;
const int64_t Coeffs2[4] = {1168890670389420, 2803840134912590,
                            1418812602632428, 3615655844709404};

class TEST_NTT : public ::testing::Test {
protected:
  void SetUp() override {
    _degree         = 4;
    int64_t mod_val = 4503599626321921;
    _modulus        = (MODULUS*)malloc(sizeof(MODULUS));
    Init_modulus(_modulus, mod_val);
    _ntt = Alloc_nttcontext();
    Init_nttcontext(_ntt, _degree, _modulus);
  }

  void TearDown() override {
    Free_nttcontext(_ntt);
    free(_modulus);
  }

  size_t       _degree;
  MODULUS*     _modulus;
  NTT_CONTEXT* _ntt;
};

TEST_F(TEST_NTT, test_ntt) {
  if (Coeff_len != _degree) {
    GTEST_SKIP() << "skip test_intt since _degree != Coeff_len";
  }
  VALUE_LIST* coeff_list = Alloc_value_list(I64_TYPE, _degree);
  Init_i64_value_list(coeff_list, _degree, (int64_t*)Coeffs1);

  VALUE_LIST* res = Alloc_value_list(I64_TYPE, _degree);

  Ftt_fwd(res, _ntt, coeff_list);

  CHECK_INT_COEFFS(Get_i64_values(res), _degree, Coeffs2);

  Free_value_list(res);
  Free_value_list(coeff_list);
}

TEST_F(TEST_NTT, test_intt) {
  if (Coeff_len != _degree) {
    GTEST_SKIP() << "skip test_intt since _degree != Coeff_len";
  }
  VALUE_LIST* coeff_list = Alloc_value_list(I64_TYPE, _degree);
  Init_i64_value_list(coeff_list, _degree, (int64_t*)Coeffs2);

  VALUE_LIST* res = Alloc_value_list(I64_TYPE, _degree);

  Ftt_inv(res, _ntt, coeff_list);

  CHECK_INT_COEFFS(Get_i64_values(res), 4, Coeffs1);

  Free_value_list(res);
  Free_value_list(coeff_list);
}

TEST_F(TEST_NTT, test_ntt_intt) {
  VALUE_LIST* coeff_list = Alloc_value_list(I64_TYPE, _degree);
  FOR_ALL_ELEM(coeff_list, idx) { I64_VALUE_AT(coeff_list, idx) = idx; }

  VALUE_LIST* ntt_list  = Alloc_value_list(I64_TYPE, _degree);
  VALUE_LIST* intt_list = Alloc_value_list(I64_TYPE, _degree);

  Ftt_fwd(ntt_list, _ntt, coeff_list);
  Ftt_inv(intt_list, _ntt, ntt_list);

  CHECK_INT_COEFFS(Get_i64_values(intt_list), _degree,
                   Get_i64_values(coeff_list));

  Free_value_list(intt_list);
  Free_value_list(ntt_list);
  Free_value_list(coeff_list);
}