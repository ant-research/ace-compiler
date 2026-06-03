//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "gtest/gtest.h"
#include "hal/hpu_ntt_apis.h"
#include "helper.h"
#include "ntt_funcs.h"

class TEST_HPU_NTT : public ::testing::Test {
protected:
  void SetUp() override {
    _poly_len = 65536;
    _rlen     = 256;
    _clen     = 256;
    _test_mod = 4503599626321921;
  }

  void Test_2d_ntt_hpu(bool input_transpose);
  void Test_2d_intt_hpu(bool input_transpose);

  U64 _poly_len;
  U64 _rlen;
  U64 _clen;
  U64 _test_mod;
};

void TEST_HPU_NTT::Test_2d_ntt_hpu(bool input_transpose) {
  U64 peg_num = 1, poly_num = 1;

  // declare w_2d for ntt
  VV64 w_2d_ncy    = Alloc_poly_2d(_rlen, _clen);
  VV64 w_2d_apptwi = Alloc_poly_2d(_rlen, _clen);
  U64  dep         = std::log2(_rlen) / 2;
  VV64 w_2d_dir_r =
      Alloc_poly_2d(dep, _rlen);  // the depth of w_2d_dir_r is log(256)/2
  VV64 w_2d_dir_c =
      Alloc_poly_2d(dep, _rlen);  // the depth of w_2d_dir_c is log(256)/2

  // twiddle factors off-line gen
  Hpu_ntt_ncy_2d_gen(w_2d_ncy, _test_mod, peg_num, poly_num, _rlen, _clen, 0,
                     input_transpose);
  Hpu_ntt_apptwi_2d_gen(w_2d_apptwi, _test_mod, peg_num, poly_num, _rlen, _clen,
                        0, input_transpose);
  Hpu_w_2d_gen(w_2d_dir_r, w_2d_dir_c, _test_mod, peg_num, poly_num, _rlen,
               _clen, 0, input_transpose);

  // used for ntt-R & ntt-C with ntt_1d
  V64 w_2d_dir_r_1d = (V64)malloc(sizeof(U64) * (_rlen >> 1));
  V64 w_2d_dir_c_1d = (V64)malloc(sizeof(U64) * (_clen >> 1));
  Hpu_conv_2d_to_1d(w_2d_dir_r_1d, w_2d_dir_r, poly_num, _rlen, _clen,
                    input_transpose);
  Hpu_conv_2d_to_1d(w_2d_dir_c_1d, w_2d_dir_c, poly_num, _rlen, _clen,
                    input_transpose);

  V64 src       = (V64)malloc(sizeof(U64) * _poly_len);
  V64 dst_1step = (V64)malloc(sizeof(U64) * _poly_len);
  V64 dst_4step = (V64)malloc(sizeof(U64) * _poly_len);

  // step0: prepare input poly
  for (size_t i = 0; i < _poly_len; i++) {
    src[i] = i;
  }

  // step1: 4step ntt for hpu
  Four_step_ntt(dst_4step, src, w_2d_ncy, w_2d_apptwi, w_2d_dir_r_1d,
                w_2d_dir_c_1d, _rlen, _clen, _test_mod, input_transpose);

  // step2: 1step ntt
  One_step_ntt(dst_1step, src, _poly_len, _test_mod);

  // step3: check result
  CHECK_INT_COEFFS(dst_4step, _poly_len, dst_1step);

  // memory free
  Free_poly_2d(w_2d_ncy);
  Free_poly_2d(w_2d_apptwi);
  Free_poly_2d(w_2d_dir_r);
  Free_poly_2d(w_2d_dir_c);
  free(w_2d_dir_r_1d);
  free(w_2d_dir_c_1d);
  free(src);
  free(dst_1step);
  free(dst_4step);
}

void TEST_HPU_NTT::Test_2d_intt_hpu(bool input_transpose) {
  U64 peg_num = 1, poly_num = 1;

  // declare w_2d for ntt
  VV64 w_2d_ncy    = Alloc_poly_2d(_clen, _rlen);
  VV64 w_2d_apptwi = Alloc_poly_2d(_clen, _rlen);
  U64  dep         = std::log2(_clen) / 2;
  VV64 w_2d_dir_r =
      Alloc_poly_2d(dep, _clen);  // the depth of w_2d_dir_r is log(256)/2
  VV64 w_2d_dir_c =
      Alloc_poly_2d(dep, _clen);  // the depth of w_2d_dir_c is log(256)/2

  // twiddle factors off-line gen
  Hpu_ntt_ncy_2d_gen(w_2d_ncy, _test_mod, peg_num, poly_num, _rlen, _clen, 1,
                     input_transpose);
  Hpu_ntt_apptwi_2d_gen(w_2d_apptwi, _test_mod, peg_num, poly_num, _rlen, _clen,
                        1, input_transpose);
  Hpu_w_2d_gen(w_2d_dir_r, w_2d_dir_c, _test_mod, peg_num, poly_num, _rlen,
               _clen, 1, input_transpose);

  // used for ntt-R & ntt-C with ntt_1d
  V64 w_2d_dir_r_1d = (V64)malloc(sizeof(U64) * (_rlen >> 1));
  V64 w_2d_dir_c_1d = (V64)malloc(sizeof(U64) * (_clen >> 1));
  Hpu_conv_2d_to_1d(w_2d_dir_r_1d, w_2d_dir_r, poly_num, _rlen, _clen,
                    input_transpose);
  Hpu_conv_2d_to_1d(w_2d_dir_c_1d, w_2d_dir_c, poly_num, _rlen, _clen,
                    input_transpose);

  V64 src       = (V64)malloc(sizeof(U64) * _poly_len);
  V64 dst_1step = (V64)malloc(sizeof(U64) * _poly_len);
  V64 dst_4step = (V64)malloc(sizeof(U64) * _poly_len);

  // step0: prepare input poly
  for (size_t i = 0; i < _poly_len; i++) {
    src[i] = i;
  }

  // step1: 4step ntt for hpu
  Four_step_intt(dst_4step, src, w_2d_ncy, w_2d_apptwi, w_2d_dir_r_1d,
                 w_2d_dir_c_1d, _rlen, _clen, _test_mod, input_transpose);

  // step2: 1step ntt
  One_step_intt(dst_1step, src, _poly_len, _test_mod);

  // step3: check result
  CHECK_INT_COEFFS(dst_4step, _poly_len, dst_1step);

  // memory free
  Free_poly_2d(w_2d_ncy);
  Free_poly_2d(w_2d_apptwi);
  Free_poly_2d(w_2d_dir_r);
  Free_poly_2d(w_2d_dir_c);
  free(w_2d_dir_r_1d);
  free(w_2d_dir_c_1d);
  free(src);
  free(dst_1step);
  free(dst_4step);
}

TEST_F(TEST_HPU_NTT, test_ntt_hpu) { Test_2d_ntt_hpu(false); }

TEST_F(TEST_HPU_NTT, test_intt_hpu) { Test_2d_intt_hpu(false); }

TEST_F(TEST_HPU_NTT, test_ntt_hpu_trans) { Test_2d_ntt_hpu(true); }

TEST_F(TEST_HPU_NTT, test_intt_hpu_trans) { Test_2d_intt_hpu(true); }