//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================
#include <vector>

#include "common/cmplr_api.h"
#include "fhe/core/lower_ctx.h"
#include "gtest/gtest.h"

using namespace air::base;
using namespace fhe::rt_context;

namespace fhe {
namespace core {
namespace test {
class TEST_CRT_CST : public ::testing::Test {
protected:
  void SetUp() override {
    _param.Set_mul_level(_level, false);
    _param.Set_poly_degree(_degree, true);
    _param.Set_first_prime_bit_num(_q0_bits);
    _param.Set_scaling_factor_bit_num(_sf_bits);
    _param.Set_q_part_num(_qpart);
    fhe::rt_context::Register_context(_param);
  }
  void TearDown() override { fhe::rt_context::Release_context(); }

  uint32_t         Level() const { return _level; }
  GLOB_SCOPE*      Glob() const { return _gs; }
  const CTX_PARAM& Param() const { return _param; }

  void Check_2d_array(std::vector<std::vector<uint64_t>>& expected,
                      CONSTANT_PTR                        cst) {
    ARRAY_TYPE_PTR       cst_ty = cst->Type()->Cast_to_arr();
    std::vector<int64_t> shape  = cst_ty->Shape();

    EXPECT_EQ(shape.size(), 2);
    EXPECT_EQ(shape.size(), cst_ty->Dim());
    EXPECT_EQ(shape[0], expected.size());
    size_t idx = 0;
    for (size_t dim1_idx = 0; dim1_idx < expected.size(); dim1_idx++) {
      EXPECT_EQ(shape[1], expected[dim1_idx].size());
      for (size_t dim2_idx = 0; dim2_idx < expected[dim1_idx].size();
           dim2_idx++) {
        EXPECT_EQ(cst->Array_elem<uint64_t>(idx++),
                  expected[dim1_idx][dim2_idx]);
      }
    }
  }

  void Check_1d_array(std::vector<uint64_t>& expected, CONSTANT_PTR cst) {
    ARRAY_TYPE_PTR       cst_ty = cst->Type()->Cast_to_arr();
    std::vector<int64_t> shape  = cst_ty->Shape();

    EXPECT_EQ(shape.size(), 1);
    EXPECT_EQ(shape.size(), cst_ty->Dim());
    EXPECT_EQ(shape[0], expected.size());
    for (size_t dim_idx = 0; dim_idx < expected.size(); dim_idx++) {
      EXPECT_EQ(cst->Array_elem<uint64_t>(dim_idx), expected[dim_idx]);
    }
  }

  // extract second dim from [start_idx, start_idx + len]
  void Sub_transpose(std::vector<std::vector<uint64_t>>& out,
                     std::vector<std::vector<uint64_t>>& in, uint32_t start_idx,
                     uint32_t len) {
    std::vector<std::vector<uint64_t>> sub;
    for (const auto& row : in) {
      std::vector<uint64_t> sub_row;
      sub_row.insert(sub_row.end(), row.begin() + start_idx,
                     row.begin() + start_idx + len);
      sub.emplace_back(sub_row);
    }
    out.resize(sub[0].size());
    for (size_t i = 0; i < sub[0].size(); ++i) {
      out[i].resize(sub.size());
      for (size_t j = 0; j < sub.size(); j++) {
        out[i][j] = sub[j][i];
      }
    }
  }

private:
  uint32_t    _level   = 5;
  uint32_t    _degree  = 16;
  uint32_t    _q0_bits = 60;
  uint32_t    _sf_bits = 56;
  uint32_t    _qpart   = 3;
  CTX_PARAM   _param;
  GLOB_SCOPE* _gs = GLOB_SCOPE::Get();
};

TEST_F(TEST_CRT_CST, get_q) {
  CRT_CST               crt_cst;
  CONSTANT_PTR          cst      = crt_cst.Get_q(Glob());
  std::vector<uint64_t> expected = {1152921504606845473, 72057594037926529,
                                    72057594037928897, 72057594037927073,
                                    72057594037928033};
  Check_1d_array(expected, cst);
}

TEST_F(TEST_CRT_CST, get_p) {
  CRT_CST               crt_cst;
  CONSTANT_PTR          cst      = crt_cst.Get_p(Glob());
  std::vector<uint64_t> expected = {1152921504606844513, 1152921504606844417};
  Check_1d_array(expected, cst);
}

TEST_F(TEST_CRT_CST, Get_prime) {
  CRT_CST               crt_cst;
  std::vector<uint64_t> expected = {1152921504606845473, 72057594037926529,
                                    72057594037928897,   72057594037927073,
                                    72057594037928033,   1152921504606844513,
                                    1152921504606844417};
  for (size_t idx = 0; idx < expected.size(); idx++) {
    EXPECT_EQ(crt_cst.Get_prime(Glob(), idx), expected[idx]);
  }
}

TEST_F(TEST_CRT_CST, Get_prime_const) {
  std::vector<uint64_t> expected = {1152921504606845473, 72057594037926529,
                                    72057594037928897,   72057594037927073,
                                    72057594037928033,   1152921504606844513,
                                    1152921504606844417};
  CRT_CST               crt_cst_base;
  const CRT_CST         crt_cst1 = crt_cst_base;

  // q and p primes not yet setup, expected failure
  for (size_t idx = 0; idx < expected.size(); idx++) {
    EXPECT_DEATH(crt_cst1.Get_prime(Glob(), idx), "");
  }

  // setup the q and p primes
  crt_cst_base.Get_q(Glob());
  crt_cst_base.Get_p(Glob());
  const CRT_CST crt_cst2 = crt_cst_base;
  for (size_t idx = 0; idx < expected.size(); idx++) {
    EXPECT_EQ(crt_cst2.Get_prime(Glob(), idx), expected[idx]);
  }
}

TEST_F(TEST_CRT_CST, get_qlhinvmodq) {
  CRT_CST                                         crt_cst;
  std::vector<std::vector<std::vector<uint64_t>>> expected = {
      {{1}, {19152249279251229, 70860578457973349}},
      {{1}, {25322871588987074, 46734722448940640}},
      {{1}}
  };

  for (size_t dim1_idx = 0; dim1_idx < expected.size(); dim1_idx++) {
    for (size_t dim2_idx = 0; dim2_idx < expected[dim1_idx].size();
         dim2_idx++) {
      CONSTANT_PTR cst = crt_cst.Get_qlhinvmodq(Glob(), dim1_idx, dim2_idx);
      Check_1d_array(expected[dim1_idx][dim2_idx], cst);
    }
  }
}

TEST_F(TEST_CRT_CST, get_qlhmodp) {
  CRT_CST                                                      crt_cst;
  std::vector<std::vector<std::vector<std::vector<uint64_t>>>> expected = {
      {{
          {1, 1},
      }},
      {{
          {72057594037926529, 72057594037926529},
          {960, 1056},
      }},
      {{
           {72057594037926529, 72057594037926529, 72057594037926529},
           {72057594037912018, 960, 1056},
       }, {{1, 1, 1, 1}}},
      {{{72057594037926529, 72057594037926529, 72057594037926529,
         72057594037926529},
        {72057594037912018, 12305, 960, 1056}},
       {{72057594037927073, 544, 72057594037927073, 72057594037927073},
        {72057594037928897, 2368, 72057594037928897, 72057594037928897}}},
      {{
           {72057594037926529, 72057594037926529, 72057594037926529,
            72057594037926529, 72057594037926529},
           {72057594037912018, 12305, 72057594037924978, 960, 1056},
       }, {{72057594037927073, 544, 72057594037927073, 72057594037927073,
         72057594037927073},
        {72057594037928897, 2368, 864, 72057594037928897, 72057594037928897}},
       {{1, 1, 1, 1, 1, 1}}}
  };

  uint32_t per_part_size = Param().Get_per_part_size();
  uint32_t num_p         = Param().Get_p_prime_num();
  for (size_t dim1_idx = 0; dim1_idx < expected.size(); dim1_idx++) {
    for (size_t dim2_idx = 0;
         dim2_idx < expected[dim1_idx].size();  // index of digit
         dim2_idx++) {
      // check for decompose part I
      uint32_t num_q  = dim1_idx + 1;
      uint32_t num_p1 = per_part_size * dim2_idx;
      uint32_t num_p2 = Param().Get_part2_size(num_q, dim2_idx);
      uint32_t num_p3 = num_q + num_p - num_p1 - num_p2;
      if (num_p1 > 0) {
        CONSTANT_PTR cst_part1 =
            crt_cst.Get_qlhmodp(Glob(), dim1_idx, dim2_idx, 0, num_p1);
        std::vector<std::vector<uint64_t>> expected_p1;
        Sub_transpose(expected_p1, expected[dim1_idx][dim2_idx], 0, num_p1);
        Check_2d_array(expected_p1, cst_part1);
      }
      // check for decompose part III
      CONSTANT_PTR cst_part3 =
          crt_cst.Get_qlhmodp(Glob(), dim1_idx, dim2_idx, num_p1, num_p3);
      std::vector<std::vector<uint64_t>> expected_p3;
      Sub_transpose(expected_p3, expected[dim1_idx][dim2_idx], num_p1, num_p3);
      Check_2d_array(expected_p3, cst_part3);
    }
  }
}

TEST_F(TEST_CRT_CST, get_qlinvmodq) {
  CRT_CST                            crt_cst;
  std::vector<std::vector<uint64_t>> expected = {
      {19152249279251229},
      {1145544555587499581, 54499641436624330},
      {1007881074770892869, 12583587194123199, 25322871588987074},
      {684205135139839883, 64343981910196362, 69472194251845800,
       16738378615060143},
  };
  for (size_t dim1_idx = 0; dim1_idx < expected.size(); dim1_idx++) {
    CONSTANT_PTR cst = crt_cst.Get_qlinvmodq(Glob(), dim1_idx);
    Check_1d_array(expected[dim1_idx], cst);
  }
}

TEST_F(TEST_CRT_CST, get_phmodq) {
  CRT_CST                            crt_cst;
  std::vector<std::vector<uint64_t>> expected = {
      {1152921504606844417, 1152921504606844513},
      {19953,               20049              },
      {72057594037910962,   72057594037911058  },
      {11249,               11345              },
      {72057594037923922,   72057594037924018  },
  };
  CONSTANT_PTR cst = crt_cst.Get_phmodq(Glob());
  Check_2d_array(expected, cst);
}

TEST_F(TEST_CRT_CST, get_qlhalfmodq) {
  CRT_CST                            crt_cst;
  std::vector<std::vector<uint64_t>> expected = {
      {36028797018963264},
      {36028797018964448, 36028797018964448},
      {36028797018963536, 36028797018963536, 36028797018963536},
      {36028797018964016, 36028797018964016, 36028797018964016,
       36028797018964016},
  };
  for (size_t dim1_idx = 0; dim1_idx < expected.size(); dim1_idx++) {
    CONSTANT_PTR cst = crt_cst.Get_qlhalfmodq(Glob(), dim1_idx);
    Check_1d_array(expected[dim1_idx], cst);
  }
}

TEST_F(TEST_CRT_CST, get_phinvmodp) {
  CRT_CST               crt_cst;
  std::vector<uint64_t> expected = {12009599006321297, 1140911905600523121};
  CONSTANT_PTR          cst      = crt_cst.Get_phinvmodp(Glob());
  Check_1d_array(expected, cst);
}

TEST_F(TEST_CRT_CST, get_pinvmodq) {
  CRT_CST               crt_cst;
  std::vector<uint64_t> expected = {600369634870647437, 63840762042067859,
                                    16773061119573931, 33600877479875797,
                                    46791840413936287};
  CONSTANT_PTR          cst      = crt_cst.Get_pinvmodq(Glob());
  Check_1d_array(expected, cst);
}

}  // namespace test
}  // namespace core

}  // namespace fhe