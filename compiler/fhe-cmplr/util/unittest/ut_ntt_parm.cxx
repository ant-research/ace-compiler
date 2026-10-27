//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <cstdint>
#include <memory>

#include "fhe/util/ntt_parm.h"
#include "gtest/gtest.h"
#include "intt_2d_1st_row_PEG4_N32768_P1.inc"
#include "intt_2d_1st_row_PEG4_N65536_P1.inc"
#include "intt_2d_1st_row_PEG8_N32768_P1.inc"
#include "intt_2d_1st_row_PEG8_N65536_P1.inc"
#include "ntt_2d_1st_row_PEG4_N32768_P1.inc"
#include "ntt_2d_1st_row_PEG4_N65536_P1.inc"
#include "ntt_2d_1st_row_PEG8_N32768_P1.inc"
#include "ntt_2d_1st_row_PEG8_N65536_P1.inc"

#define Q          1152921504606584833
#define CORE_WIDTH 256  // PEB_per_PEG * PE_per_PEB = 16 * 16

using namespace fhe::util;
using NTT_PARM = fhe::util::NTT_2D_PARM<uint64_t>;

class NTT_PARM_TEST : public testing::Test {
public:
  void Test_ntt_parm(uint64_t core_cnt, uint64_t poly_cnt, uint64_t col,
                     uint64_t row, bool intt, bool ncy_trs, bool twi_trs,
                     uint64_t ncy[][CORE_WIDTH], uint64_t ncy_delta[CORE_WIDTH],
                     uint64_t twiddle[][CORE_WIDTH],
                     uint64_t twi_delta[CORE_WIDTH],
                     uint64_t ntt_w[][CORE_WIDTH]);

  void Test_peg8_n65536_poly1_ntt();
  void Test_peg8_n65536_poly1_intt();

  void Test_peg4_n65536_poly1_ntt();
  void Test_peg4_n65536_poly1_intt();

  void Test_peg8_n32768_poly1_ntt();
  void Test_peg8_n32768_poly1_intt();
  void Test_peg4_n32768_poly1_ntt();
  void Test_peg4_n32768_poly1_intt();

protected:
  void SetUp() override {}
  void TearDown() override {}

  void Reinit_parm_gen(uint32_t row_len, uint32_t col_len, uint32_t core_depth,
                       uint32_t poly_cnt, uint32_t core_cnt, uint64_t q) {
    _ntt_parm = std::make_unique<NTT_PARM>(row_len, col_len, CORE_WIDTH,
                                           poly_cnt, core_cnt, q);
  }

private:
  std::unique_ptr<NTT_PARM> _ntt_parm;
};

void NTT_PARM_TEST::Test_ntt_parm(
    uint64_t core_cnt, uint64_t poly_cnt, uint64_t col, uint64_t row, bool intt,
    bool ncy_trs, bool twi_trs, uint64_t ncy[][CORE_WIDTH],
    uint64_t ncy_delta[CORE_WIDTH], uint64_t twiddle[][CORE_WIDTH],
    uint64_t twi_delta[CORE_WIDTH], uint64_t ntt_w[][CORE_WIDTH]) {
  Reinit_parm_gen(row, col, 1, poly_cnt, core_cnt, Q);

  // 1.1 verify 2D-NTT ncy
  _ntt_parm->Calc_2d_ncy_1st_row(intt, ncy_trs);
  for (uint32_t core = 0; core < core_cnt; ++core) {
    for (uint32_t idx = 0; idx < CORE_WIDTH; ++idx) {
      uint64_t expect_val = ncy[core][idx];
      uint64_t val        = _ntt_parm->Ncy_2d_1st_row()[core][idx];
      if (val != expect_val) {
        std::cout << "Find diff ncy at: core_" << core << ", id_" << idx
                  << std::endl;
      }
      EXPECT_EQ(expect_val, val);
    }
  }
  // 1.2 verify 2D-NTT ncy delta
  _ntt_parm->Calc_ncy_delta(intt, ncy_trs);
  for (uint32_t idx = 0; idx < CORE_WIDTH; ++idx) {
    uint64_t expect_val = ncy_delta[idx];
    uint64_t val        = _ntt_parm->Ncy_delta()[idx];
    if (val != expect_val) {
      std::cout << "Find diff ncy delta at: id_" << idx << std::endl;
    }
    EXPECT_EQ(expect_val, val);
  }

  // 2.1. verify 2D-NTT twiddle factor
  _ntt_parm->Calc_2d_twiddle_1st_row(intt, twi_trs);
  for (uint32_t core = 0; core < core_cnt; ++core) {
    for (uint32_t idx = 0; idx < CORE_WIDTH; ++idx) {
      uint64_t expect_val = twiddle[core][idx];
      uint64_t val        = _ntt_parm->Twiddle_2d_1st_row()[core][idx];
      if (val != expect_val) {
        std::cout << "Find diff twiddle factor at: core_" << core << ", id_"
                  << idx << std::endl;
      }
      EXPECT_EQ(expect_val, val);
    }
  }
  // 2.2. verify 2D-NTT twiddle factor delta
  _ntt_parm->Calc_twiddle_delta(intt, twi_trs);
  for (uint32_t idx = 0; idx < CORE_WIDTH; ++idx) {
    uint64_t expect_val = twi_delta[idx];
    uint64_t val        = _ntt_parm->Twiddle_delta()[idx];
    if (val != expect_val) {
      std::cout << "Find diff twiddle delta at: id_" << idx << std::endl;
    }
    EXPECT_EQ(expect_val, val);
  }

  // 3. verify 2D-NTT factor
  _ntt_parm->Calc_2d_ntt_w(intt, true);
  uint64_t row_ntt_len = col;
  uint64_t col_ntt_len = row;
  for (uint64_t line = 0; line < GET_W_LINES(row_ntt_len); ++line) {
    for (uint64_t idx = 0; idx < CORE_WIDTH; ++idx) {
      uint64_t expect_val = ntt_w[line][idx];
      uint64_t val        = _ntt_parm->Ntt_2d_w()[line][idx];
      if (val != expect_val) {
        std::cout << "Find diff row ntt factor at: line_" << line << ", id_"
                  << idx << std::endl;
      }
      EXPECT_EQ(expect_val, val);
    }
  }
  for (uint64_t line = 0; line < GET_W_LINES(col_ntt_len); ++line) {
    for (uint64_t idx = 0; idx < CORE_WIDTH; ++idx) {
      uint64_t expect_val = ntt_w[line + GET_W_LINES(row_ntt_len)][idx];
      uint64_t val =
          _ntt_parm->Ntt_2d_w()[line + GET_W_LINES(row_ntt_len)][idx];
      if (val != expect_val) {
        std::cout << "Find diff column ntt factor at: line_" << line << ", id_"
                  << idx << std::endl;
      }
      EXPECT_EQ(expect_val, val);
    }
  }
}
void NTT_PARM_TEST::Test_peg8_n65536_poly1_ntt() {
  constexpr uint64_t ROW      = 256;
  constexpr uint64_t COL      = 256;
  constexpr uint64_t CORE_CNT = 8;
  constexpr uint64_t POLY_CNT = 1;
  constexpr bool     INTT     = false;
  constexpr bool     NCY_TRS  = true;
  constexpr bool     TWI_TRS  = false;

  Test_ntt_parm(CORE_CNT, POLY_CNT, COL, ROW, INTT, NCY_TRS, TWI_TRS,
                ntt_2d_1st_row_PEG8_N65536_P1_NTT_TRS_ncy,
                ntt_2d_1st_row_PEG8_N65536_P1_NTT_TRS_ncy_delta,
                ntt_2d_1st_row_PEG8_N65536_P1_NTT_twi,
                ntt_2d_1st_row_PEG8_N65536_P1_NTT_twi_delta,
                ntt_2d_1st_row_PEG8_N65536_P1_NTT_TRS_ntt_w);
}

void NTT_PARM_TEST::Test_peg8_n65536_poly1_intt() {
  constexpr uint64_t ROW      = 256;
  constexpr uint64_t COL      = 256;
  constexpr uint64_t CORE_CNT = 8;
  constexpr uint64_t POLY_CNT = 1;
  constexpr bool     INTT     = true;
  constexpr bool     NCY_TRS  = true;
  constexpr bool     TWI_TRS  = false;
  Test_ntt_parm(CORE_CNT, POLY_CNT, COL, ROW, INTT, NCY_TRS, TWI_TRS,
                intt_2d_1st_row_PEG8_N65536_P1_INTT_TRS_ncy,
                intt_2d_1st_row_PEG8_N65536_P1_INTT_TRS_ncy_delta,
                intt_2d_1st_row_PEG8_N65536_P1_INTT_twi,
                intt_2d_1st_row_PEG8_N65536_P1_INTT_twi_delta,
                intt_2d_1st_row_PEG8_N65536_P1_INTT_ntt_w);
}

void NTT_PARM_TEST::Test_peg4_n65536_poly1_ntt() {
  constexpr uint64_t ROW      = 256;
  constexpr uint64_t COL      = 256;
  constexpr uint64_t CORE_CNT = 4;
  constexpr uint64_t POLY_CNT = 1;
  constexpr bool     INTT     = false;
  constexpr bool     NCY_TRS  = true;
  constexpr bool     TWI_TRS  = false;

  Test_ntt_parm(CORE_CNT, POLY_CNT, COL, ROW, INTT, NCY_TRS, TWI_TRS,
                ntt_2d_1st_row_PEG4_N65536_P1_NTT_TRS_ncy,
                ntt_2d_1st_row_PEG4_N65536_P1_NTT_TRS_ncy_delta,
                ntt_2d_1st_row_PEG4_N65536_P1_NTT_twi,
                ntt_2d_1st_row_PEG4_N65536_P1_NTT_twi_delta,
                ntt_2d_1st_row_PEG4_N65536_P1_NTT_TRS_ntt_w);
}
void NTT_PARM_TEST::Test_peg4_n65536_poly1_intt() {
  constexpr uint64_t ROW      = 256;
  constexpr uint64_t COL      = 256;
  constexpr uint64_t CORE_CNT = 4;
  constexpr uint64_t POLY_CNT = 1;
  constexpr bool     INTT     = true;
  constexpr bool     NCY_TRS  = true;
  constexpr bool     TWI_TRS  = false;
  Test_ntt_parm(CORE_CNT, POLY_CNT, COL, ROW, INTT, NCY_TRS, TWI_TRS,
                intt_2d_1st_row_PEG4_N65536_P1_INTT_TRS_ncy,
                intt_2d_1st_row_PEG4_N65536_P1_INTT_TRS_ncy_delta,
                intt_2d_1st_row_PEG4_N65536_P1_INTT_twi,
                intt_2d_1st_row_PEG4_N65536_P1_INTT_twi_delta,
                intt_2d_1st_row_PEG4_N65536_P1_INTT_ntt_w);
}

void NTT_PARM_TEST::Test_peg8_n32768_poly1_ntt() {
  constexpr uint64_t ROW      = 256;
  constexpr uint64_t COL      = 128;
  constexpr uint64_t CORE_CNT = 8;
  constexpr uint64_t POLY_CNT = 1;
  constexpr bool     INTT     = false;
  constexpr bool     NCY_TRS  = true;
  constexpr bool     TWI_TRS  = false;

  Test_ntt_parm(CORE_CNT, POLY_CNT, COL, ROW, INTT, NCY_TRS, TWI_TRS,
                ntt_2d_1st_row_PEG8_N32768_P1_NTT_TRS_ncy,
                ntt_2d_1st_row_PEG8_N32768_P1_NTT_TRS_ncy_delta,
                ntt_2d_1st_row_PEG8_N32768_P1_NTT_twi,
                ntt_2d_1st_row_PEG8_N32768_P1_NTT_twi_delta,
                ntt_2d_1st_row_PEG8_N32768_P1_NTT_TRS_ntt_w);
}
void NTT_PARM_TEST::Test_peg8_n32768_poly1_intt() {
  constexpr uint64_t ROW      = 256;
  constexpr uint64_t COL      = 128;
  constexpr uint64_t CORE_CNT = 8;
  constexpr uint64_t POLY_CNT = 1;
  constexpr bool     INTT     = true;
  constexpr bool     NCY_TRS  = true;
  constexpr bool     TWI_TRS  = false;
  Test_ntt_parm(CORE_CNT, POLY_CNT, COL, ROW, INTT, NCY_TRS, TWI_TRS,
                intt_2d_1st_row_PEG8_N32768_P1_INTT_TRS_ncy,
                intt_2d_1st_row_PEG8_N32768_P1_INTT_TRS_ncy_delta,
                intt_2d_1st_row_PEG8_N32768_P1_INTT_twi,
                intt_2d_1st_row_PEG8_N32768_P1_INTT_twi_delta,
                intt_2d_1st_row_PEG8_N32768_P1_INTT_ntt_w);
}

void NTT_PARM_TEST::Test_peg4_n32768_poly1_ntt() {
  constexpr uint64_t ROW      = 256;
  constexpr uint64_t COL      = 128;
  constexpr uint64_t CORE_CNT = 4;
  constexpr uint64_t POLY_CNT = 1;
  constexpr bool     INTT     = false;
  constexpr bool     NCY_TRS  = true;
  constexpr bool     TWI_TRS  = false;

  Test_ntt_parm(CORE_CNT, POLY_CNT, COL, ROW, INTT, NCY_TRS, TWI_TRS,
                ntt_2d_1st_row_PEG4_N32768_P1_NTT_TRS_ncy,
                ntt_2d_1st_row_PEG4_N32768_P1_NTT_TRS_ncy_delta,
                ntt_2d_1st_row_PEG4_N32768_P1_NTT_twi,
                ntt_2d_1st_row_PEG4_N32768_P1_NTT_twi_delta,
                ntt_2d_1st_row_PEG4_N32768_P1_NTT_TRS_ntt_w);
}
void NTT_PARM_TEST::Test_peg4_n32768_poly1_intt() {
  constexpr uint64_t ROW      = 256;
  constexpr uint64_t COL      = 128;
  constexpr uint64_t CORE_CNT = 4;
  constexpr uint64_t POLY_CNT = 1;
  constexpr bool     INTT     = true;
  constexpr bool     NCY_TRS  = true;
  constexpr bool     TWI_TRS  = false;
  Test_ntt_parm(CORE_CNT, POLY_CNT, COL, ROW, INTT, NCY_TRS, TWI_TRS,
                intt_2d_1st_row_PEG4_N32768_P1_INTT_TRS_ncy,
                intt_2d_1st_row_PEG4_N32768_P1_INTT_TRS_ncy_delta,
                intt_2d_1st_row_PEG4_N32768_P1_INTT_twi,
                intt_2d_1st_row_PEG4_N32768_P1_INTT_twi_delta,
                intt_2d_1st_row_PEG4_N32768_P1_INTT_ntt_w);
}

TEST_F(NTT_PARM_TEST, test_peg8_n65536_ntt) { Test_peg8_n65536_poly1_ntt(); }
TEST_F(NTT_PARM_TEST, test_peg8_n65536_intt) { Test_peg8_n65536_poly1_intt(); }
TEST_F(NTT_PARM_TEST, test_peg4_n65536_ntt) { Test_peg4_n65536_poly1_ntt(); }
TEST_F(NTT_PARM_TEST, test_peg4_n65536_intt) { Test_peg4_n65536_poly1_intt(); }
TEST_F(NTT_PARM_TEST, test_peg8_n32768_ntt) { Test_peg8_n32768_poly1_ntt(); }
TEST_F(NTT_PARM_TEST, test_peg8_n32768_intt) { Test_peg8_n32768_poly1_intt(); }
TEST_F(NTT_PARM_TEST, test_peg4_n32768_ntt) { Test_peg4_n32768_poly1_ntt(); }
TEST_F(NTT_PARM_TEST, test_peg4_n32768_intt) { Test_peg4_n32768_poly1_intt(); }