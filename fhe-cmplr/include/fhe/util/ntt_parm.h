//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_UTIL_NTT_PARM_H
#define FHE_UTIL_NTT_PARM_H

#include <sys/types.h>

#include <cstdint>
#include <utility>
#include <vector>

#include "air/util/debug.h"
#include "ntt_math.h"

namespace fhe {
namespace util {

//! @brief Compute the number of lines of ntt twiddle factors.
//! A the computation length of ntt.
#define GET_W_LINES(A) ((fhe::util::Log2_comp(A) + 1) / 2)

//! @brief 2D NTT Parameter Generator
//! For more details, refer to Viktor Krapivensky's work: Speeding up decimal
//! multiplication. _row: Length of a row in 2D NTT. _col: Length of a column in
//! 2D NTT. _core_width: Coefficients per row handled by each accelerator.
//! _poly_cnt: Polynomials processed in parallel.
//! _core_cnt: Number of accelerator cores invoked.
//! _q: Modulus used in NTT operations.
//! _ncy: Factor for negative cyclic convolution wrapping.
//! _ncy_delta: Difference between successive rows of _ncy in 2D NTT.
//! _twiddle: Twiddle factor.
//! _twiddle_delta: Difference between successive rows of the twiddle factor in
//! 2D NTT. _ntt_w: Factor for both row-direction and col-direction NTT.
template <typename COEFF_T>
class NTT_2D_PARM {
public:
  using COEFF_V  = std::vector<COEFF_T>;
  using COEFF_VV = std::vector<COEFF_V>;

  NTT_2D_PARM(uint32_t row_len, uint32_t col_len, uint32_t core_width,
              uint32_t poly_cnt, uint32_t core_cnt, uint64_t q)
      : _row(row_len),
        _col(col_len),
        _core_width(core_width),
        _poly(poly_cnt),
        _core(core_cnt),
        _q(q) {}
  ~NTT_2D_PARM() {}

  //! @brief Calculate 1D ncy.
  void Calc_1d_ncy(bool intt, bool trs);
  //! @brief Calculate 2D ncy for 2D NTT.
  void            Calc_2d_ncy(bool intt, bool trs);
  const COEFF_VV& Ncy_2d(void) const { return _ncy_2d; }
  COEFF_VV&       Ncy_2d(void) { return _ncy_2d; }

  //! @brief Compute 2D ncy and extract the first row for each core for the
  //! online ncy generator.
  void            Calc_2d_ncy_1st_row(bool intt, bool trs);
  const COEFF_VV& Ncy_2d_1st_row(void) const { return _ncy_2d_1st_row; }
  COEFF_VV&       Ncy_2d_1st_row(void) { return _ncy_2d_1st_row; }

  //! @brief Calculate the 2D ncy delta, representing the difference between
  //! consecutive rows of ncy in 2D NTT.
  void           Calc_ncy_delta(bool intt, bool trs);
  const COEFF_V& Ncy_delta(void) const { return _ncy_delta; }
  COEFF_V&       Ncy_delta(void) { return _ncy_delta; }

  //! @brief Calculate 2D twiddle factor for 2D NTT.
  void            Calc_2d_twiddle(bool intt, bool trs);
  const COEFF_VV& Twiddle_2d(void) const { return _twiddle; }
  COEFF_VV&       Twiddle_2d(void) { return _twiddle; }

  //! @brief Compute 2D twiddle factor and extract the first row for each
  //! core for the online twiddle generator.
  void            Calc_2d_twiddle_1st_row(bool intt, bool trs);
  const COEFF_VV& Twiddle_2d_1st_row(void) const { return _twiddle_1st_row; }
  COEFF_VV&       Twiddle_2d_1st_row(void) { return _twiddle_1st_row; }

  //! @brief Calculate the 2D twiddle factor delta, representing the
  //! difference between consecutive rows of twiddle factor in 2D NTT.
  void           Calc_twiddle_delta(bool intt, bool trs);
  const COEFF_V& Twiddle_delta(void) const { return _twiddle_delta; }
  COEFF_V&       Twiddle_delta(void) { return _twiddle_delta; }

  //! @brief Compute the 2D NTT factor, placing row-direction factors before
  //! col-direction factors.
  void            Calc_2d_ntt_w(bool intt, bool trs);
  const COEFF_VV& Ntt_2d_w(void) const { return _ntt_w; }
  COEFF_VV&       Ntt_2d_w(void) { return _ntt_w; }

  void Set_q(uint64_t q) { _q = q; }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  NTT_2D_PARM(void);
  NTT_2D_PARM(const NTT_2D_PARM&);
  NTT_2D_PARM& operator=(const NTT_2D_PARM&);

  uint64_t       Mod(void) const { return _q; }
  uint32_t       Poly_deg(void) const { return _row * _col; }
  uint32_t       Row(void) const { return _row; }
  uint32_t       Col(void) const { return _col; }
  uint32_t       Core_width(void) const { return _core_width; }
  uint32_t       Poly_cnt(void) const { return _poly; }
  uint32_t       Core_cnt(void) const { return _core; }
  const COEFF_V& Ncy_1d(void) const { return _ncy_1d; }
  COEFF_V&       Ncy_1d(void) { return _ncy_1d; }

  uint64_t _q;    // Modulus used in NTT operations.
  uint32_t _row;  // Length of a row in 2D NTT.
  uint32_t _col;  // Length of a column in 2D NTT.
  uint32_t _core_width;
  uint32_t _poly;
  uint32_t _core;
  COEFF_V  _ncy_1d;
  COEFF_VV _ncy_2d;
  COEFF_VV _ncy_2d_1st_row;
  COEFF_V  _ncy_delta;
  COEFF_VV _twiddle;
  COEFF_VV _twiddle_1st_row;
  COEFF_V  _twiddle_delta;
  COEFF_VV _ntt_w;
};

template <typename COEFF_T>
void NTT_2D_PARM<COEFF_T>::Calc_1d_ncy(bool intt, bool trs) {
  // dim: number of coefficients in 1D ncy
  uint32_t dim = Row() * Col() * Poly_cnt();
  _ncy_1d.resize(dim);
  _ncy_1d[0]   = intt ? Modinv(Poly_deg(), Mod()) : 1;
  uint64_t fin = Find_2nth_root(Poly_deg(), Mod());
  if (intt) {
    fin = Modinv(fin, Mod());
  }
  for (size_t i = 1; i < Poly_deg(); i++) {
    _ncy_1d[i] = Modmul(fin, _ncy_1d[i - 1], Mod());
  }
  // transpose the 1D ncy
  if (trs) {
    // polyN 1d data_tranpose
    COEFF_V ncy_trs(dim, 0);
    for (size_t i = 0; i < Row(); i++) {
      for (size_t j = 0; j < Col(); j++) {
        ncy_trs[j * Row() + i] = _ncy_1d[i * Col() + j];
      }
    }
    _ncy_1d = ncy_trs;
  }
}

template <typename COEFF_T>
void NTT_2D_PARM<COEFF_T>::Calc_2d_ncy(bool intt, bool trs) {
  // 1. Generate the 1D ncy
  Calc_1d_ncy(intt, trs);

  // 2. Generate 2D ncy by splitting the 1D ncy into rows.
  uint32_t row = Row();
  uint32_t col = Col();
  if (trs) std::swap(row, col);
  _ncy_2d.resize(col * row * Poly_cnt() / Core_width(),
                 COEFF_V(Core_width(), 0));

  uint64_t split_num      = Core_width() / (col * Poly_cnt());
  uint64_t depth_per_row  = row / (Core_cnt() * split_num);
  uint64_t width_per_poly = Core_width() / Poly_cnt();
  // from 1d_ntt_ncy_w to 2d format
  for (size_t qi = 0; qi < Poly_cnt(); qi++) {
    for (size_t peg_i = 0; peg_i < Core_cnt(); peg_i++) {
      for (size_t x = 0; x < depth_per_row; x++) {
        for (size_t splt = 0; splt < split_num; splt++) {
          for (size_t y = 0; y < col; y++) {
            uint32_t x_2d = qi * width_per_poly + splt * col + y;
            uint32_t y_2d = peg_i * depth_per_row + x;
            uint32_t x_1d =
                (peg_i * depth_per_row * split_num + splt * depth_per_row + x) *
                    col +
                y;
            Ncy_2d()[y_2d][x_2d] = Ncy_1d()[x_1d];
          }
        }
      }
    }
  }
}

template <typename COEFF_T>
void NTT_2D_PARM<COEFF_T>::Calc_2d_ncy_1st_row(bool intt, bool trs) {
  // 1. Generate 2D ncy
  Calc_2d_ncy(intt, trs);

  // 2. Extract the first row of the 2D ncy for each core for the online
  // generator.
  _ncy_2d_1st_row.resize(Core_cnt(), COEFF_V(Core_width(), 0));
  uint32_t depth_per_core =
      Col() * Row() * Poly_cnt() / (Core_width() * Core_cnt());
  for (size_t core_idx = 0; core_idx < Core_cnt(); core_idx++) {
    for (size_t x = 0; x < Core_width(); x++) {
      _ncy_2d_1st_row[core_idx][x] = _ncy_2d[core_idx * depth_per_core][x];
    }
  }
}

template <typename COEFF_T>
void NTT_2D_PARM<COEFF_T>::Calc_ncy_delta(bool intt, bool trs) {
  // Generate ncy delta, indicating the difference between consecutive rows.
  uint64_t fin = Find_2nth_root(Row() * Col(), Mod());
  if (intt) {
    fin = Modinv(fin, Mod());
  }

  // trs= true: col-direction; trs= false: row-direction
  uint64_t delta = trs ? fin : Modpow(fin, Col(), Mod());
  _ncy_delta.resize(Core_width(), delta);
}

template <typename COEFF_T>
void NTT_2D_PARM<COEFF_T>::Calc_2d_twiddle(bool intt, bool trs) {
  uint32_t row = Row();
  uint32_t col = Col();
  uint64_t fin = Find_2nth_root(row * col, Mod());
  if (intt) {
    fin = Modinv(fin, Mod());
  }
  uint64_t w = Modmul(fin, fin, Mod());
  if (trs) {
    std::swap(row, col);
  }

  Twiddle_2d().resize(col * row * Poly_cnt() / Core_width(),
                      COEFF_V(Core_width(), 0));
  uint32_t split_num = Core_width() / (col * Poly_cnt());
  AIR_ASSERT(split_num > 0);
  uint32_t depth_per_row = row / (Core_cnt() * split_num);
  AIR_ASSERT(depth_per_row > 0);
  uint32_t width_per_poly = Core_width() / Poly_cnt();
  // 1d to 2d
  for (size_t qi = 0; qi < Poly_cnt(); qi++) {
    for (size_t core_idx = 0; core_idx < Core_cnt(); core_idx++) {
      for (size_t x = 0; x < depth_per_row; x++) {
        for (size_t splt = 0; splt < split_num; splt++) {
          for (size_t y = 0; y < col; y++) {
            uint32_t x_2d = qi * width_per_poly + splt * col + y;
            uint32_t y_2d = core_idx * depth_per_row + x;
            uint32_t pow  = (core_idx * depth_per_row * split_num +
                            splt * depth_per_row + x) *
                           y;
            Twiddle_2d()[y_2d][x_2d] = Modpow(w, pow, Mod());
          }
        }
      }
    }
  }
}

template <typename COEFF_T>
void NTT_2D_PARM<COEFF_T>::Calc_2d_twiddle_1st_row(bool intt, bool trs) {
  // 1. Compute 2D twiddle factor.
  Calc_2d_twiddle(intt, trs);

  // 2. Extract the first row of the 2D twiddle factor for each core for the
  // online generator.
  _twiddle_1st_row.resize(Core_cnt(), COEFF_V(Core_width(), 0));
  uint32_t depth_per_core =
      Col() * Row() * Poly_cnt() / (Core_width() * Core_cnt());
  for (size_t core_idx = 0; core_idx < Core_cnt(); core_idx++) {
    for (size_t x = 0; x < Core_width(); x++) {
      _twiddle_1st_row[core_idx][x] =
          Twiddle_2d()[core_idx * depth_per_core][x];
    }
  }
}

template <typename COEFF_T>
void NTT_2D_PARM<COEFF_T>::Calc_twiddle_delta(bool intt, bool trs) {
  // Generate the twiddle factor delta, indicating the difference between
  // consecutive rows.
  uint64_t fin = Find_2nth_root(Row() * Col(), Mod());
  if (intt) fin = Modinv(fin, Mod());
  uint64_t w         = Modmul(fin, fin, Mod());
  uint64_t split_num = Core_width() / (Col() * Poly_cnt());
  AIR_ASSERT(split_num > 0);
  _twiddle_delta.resize(Core_width(), 0);
  for (size_t poly_idx = 0; poly_idx < Poly_cnt(); poly_idx++) {
    for (size_t splt = 0; splt < split_num; splt++) {
      for (size_t j = 0; j < Col(); j++) {
        uint32_t idx = poly_idx * Core_width() / Poly_cnt() + splt * Col() + j;
        _twiddle_delta[idx] = Modpow(w, j, Mod());
      }
    }
  }
}

template <typename COEFF_T>
void NTT_2D_PARM<COEFF_T>::Calc_2d_ntt_w(bool intt, bool trs) {
  uint64_t row_ntt_len = Col();
  uint64_t col_ntt_len = Row();
  if (trs) std::swap(row_ntt_len, col_ntt_len);

  uint64_t log_row_ntt_len = Log2_comp(row_ntt_len);
  uint64_t log_col_ntt_len = Log2_comp(col_ntt_len);
  uint64_t fin             = Find_2nth_root(Row() * Col(), Mod());
  // 1. generate 1d w
  if (intt) {
    fin = Modinv(fin, Mod());
  }
  COEFF_V wn_s_row(row_ntt_len >> 1);
  wn_s_row[0] = 1;
  wn_s_row[1] = Modpow(fin, 2 * col_ntt_len, Mod());
  for (size_t i = 2; i < (row_ntt_len >> 1); i++) {
    wn_s_row[i] = Modmul(wn_s_row[i - 1], wn_s_row[1], Mod());
  }
  COEFF_V wn_s_col(col_ntt_len >> 1);
  wn_s_col[0] = 1;
  wn_s_col[1] = Modpow(fin, 2 * row_ntt_len, Mod());
  for (size_t i = 2; i < (col_ntt_len >> 1); i++) {
    wn_s_col[i] = Modmul(wn_s_col[i - 1], wn_s_col[1], Mod());
  }

  uint64_t split_num_row = Core_width() / (row_ntt_len * Poly_cnt());
  uint64_t split_num_col = Core_width() / (col_ntt_len * Poly_cnt());

  _ntt_w.resize(GET_W_LINES(row_ntt_len) + GET_W_LINES(col_ntt_len),
                COEFF_V(Core_width(), 0));
  // 2. 1d to 2d
  for (size_t qi = 0; qi < Poly_cnt(); qi++) {
    // 2.1 generate ntt_w for row-direction NTT
    for (size_t spln = 0; spln < split_num_row; spln++) {
      for (uint64_t i = 1; i <= log_row_ntt_len; i++) {
        uint64_t door      = 1 << (i - 1);
        size_t   w_cnt     = 0;
        uint64_t ntt_stage = (i - 1) / 2;
        for (uint64_t j = 0; j < row_ntt_len; j += (door << 1)) {
          for (uint64_t k = 0; k < door; k++) {
            uint32_t w_idx = (qi * split_num_row + spln) * row_ntt_len +
                             2 * w_cnt + ((i - 1) % 2);
            _ntt_w[ntt_stage][w_idx] =
                wn_s_row[Get_w_ind(i, k, log_row_ntt_len)];
            w_cnt++;
          }
        }
      }
    }
    // 2.2 generate ntt_w of col-direction NTT
    for (size_t spln = 0; spln < split_num_col; spln++) {
      for (uint64_t i = 1; i <= log_col_ntt_len; i++) {
        uint32_t door      = 1 << (i - 1);
        size_t   w_cnt     = 0;
        uint32_t ntt_stage = (i - 1) / 2 + GET_W_LINES(row_ntt_len);
        for (uint64_t j = 0; j < col_ntt_len; j += (door << 1)) {
          for (uint64_t k = 0; k < door; k++) {
            uint32_t w_idx = (qi * split_num_col + spln) * col_ntt_len +
                             2 * w_cnt + ((i - 1) % 2);
            _ntt_w[ntt_stage][w_idx] =
                wn_s_col[Get_w_ind(i, k, log_col_ntt_len)];
            w_cnt++;
          }
        }
      }
    }
  }
}
}  // namespace util
}  // namespace fhe

#endif  // FHE_UTIL_NTT_PARM_H