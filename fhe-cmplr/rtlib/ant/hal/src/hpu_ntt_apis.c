//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "hal/hpu_ntt_apis.h"

#include <math.h>

void Hpu_ntt_ncy_2d_gen(VV64 poly_2d, U64 qs, U8 peg_num, U8 poly_num,
                        U32 row_length, U64 column_length, bool is_inv,
                        bool trs) {
  U64 len = row_length * column_length;
  assert(len * poly_num == peg_num * row_length * column_length);
  // generate 1d ntt_ncy_w
  V64 poly_n = (V64)malloc(len * sizeof(U64));
  assert(poly_n != NULL);

  poly_n[0] = is_inv ? Modinv(len, qs) : 1;
  U64 fin   = Find_2nth_root(row_length * column_length, qs);
  if (is_inv) {
    fin = Modinv(fin, qs);
  }
  for (size_t i = 1; i < len; i++) {
    poly_n[i] = Modmul(fin, poly_n[i - 1], qs);
  }
  if (trs) {
    V64 poly_n_trans = (V64)malloc(len * sizeof(U64));
    assert(poly_n_trans != NULL);

    // poly_n 1d data_tranpose
    for (size_t i = 0; i < row_length; i++) {
      for (size_t j = 0; j < column_length; j++) {
        poly_n_trans[j * row_length + i] = poly_n[i * column_length + j];
      }
    }
    for (size_t i = 0; i < len; i++) {
      poly_n[i] = poly_n_trans[i];
    }
    free(poly_n_trans);

    U64 swap      = row_length;
    row_length    = column_length;
    column_length = swap;
  }

  U64 split_num      = PEG_WIDTH / (column_length * poly_num);
  U64 depth          = row_length / (peg_num * split_num);
  U64 width_per_poly = PEG_WIDTH / poly_num;
  // from 1d_ntt_ncy_w to 2d format
  for (size_t qi = 0; qi < poly_num; qi++) {
    for (size_t peg_i = 0; peg_i < peg_num; peg_i++) {
      for (size_t x = 0; x < depth; x++) {
        for (size_t splt = 0; splt < split_num; splt++) {
          for (size_t y = 0; y < column_length; y++) {
            Set_element(poly_2d, peg_i * depth + x,
                        qi * width_per_poly + splt * column_length + y,
                        row_length,
                        poly_n[(peg_i * depth * split_num + splt * depth + x) *
                                   column_length +
                               y]);
          }
        }
      }
    }
  }
  free(poly_n);
}

void Hpu_ntt_apptwi_2d_gen(VV64 poly_2d, U64 qs, U8 peg_num, U8 poly_num,
                           U32 row_length, U64 column_length, bool is_inv,
                           bool trs) {
  if (trs) {
    U64 swap      = row_length;
    row_length    = column_length;
    column_length = swap;
  }

  U64 fin = Find_2nth_root(row_length * column_length, qs);
  if (is_inv) {
    fin = Modinv(fin, qs);
  }
  U64 w = Modmul(fin, fin, qs);

  U64 split_num      = PEG_WIDTH / (row_length * poly_num);
  U64 depth          = column_length / (peg_num * split_num);
  U64 width_per_poly = PEG_WIDTH / poly_num;
  // 1d to 2d
  for (size_t qi = 0; qi < poly_num; qi++) {
    for (size_t peg_i = 0; peg_i < peg_num; peg_i++) {
      for (size_t x = 0; x < depth; x++) {
        for (size_t splt = 0; splt < split_num; splt++) {
          for (size_t y = 0; y < row_length; y++) {
            Set_element(
                poly_2d, peg_i * depth + x,
                qi * width_per_poly + splt * row_length + y, row_length,
                Modpow(w, (peg_i * depth * split_num + splt * depth + x) * y,
                       qs));
          }
        }
      }
    }
  }
}

void Hpu_w_2d_gen(VV64 w_2d_r, VV64 w_2d_c, U64 qs, U8 peg_num, U8 poly_num,
                  U32 row_length, U64 column_length, bool is_inv, bool trs) {
  U64 r_nttlen, c_nttlen;
  if (trs) {
    r_nttlen = row_length;
    c_nttlen = column_length;
  } else {
    r_nttlen = column_length;
    c_nttlen = row_length;
  }
  U64 log_r_nttlen = Log2_comp(r_nttlen);
  U64 log_c_nttlen = Log2_comp(c_nttlen);
  U64 fin          = Find_2nth_root(row_length * column_length, qs);

  // generate 1d w
  if (is_inv) {
    fin = Modinv(fin, qs);
  }

  V64 wn_s_r = (V64)malloc((r_nttlen >> 1) * sizeof(U64));
  wn_s_r[0]  = 1;
  wn_s_r[1]  = Modpow(fin, 2 * c_nttlen, qs);
  for (size_t i = 2; i < (r_nttlen >> 1); i++) {
    wn_s_r[i] = Modmul(wn_s_r[i - 1], wn_s_r[1], qs);
  }

  V64 wn_s_c = (V64)malloc((c_nttlen >> 1) * sizeof(U64));
  wn_s_c[0]  = 1;
  wn_s_c[1]  = Modpow(fin, 2 * r_nttlen, qs);
  for (size_t i = 2; i < (c_nttlen >> 1); i++) {
    wn_s_c[i] = Modmul(wn_s_c[i - 1], wn_s_c[1], qs);
  }

  U64 split_num_r = PEG_WIDTH / (r_nttlen * poly_num);
  U64 split_num_c = PEG_WIDTH / (c_nttlen * poly_num);

  // 1d to 2d
  for (size_t qi = 0; qi < poly_num; qi++) {
    // generate w_2d_r
    for (size_t spln = 0; spln < split_num_r; spln++) {
      U64 i, j, k;
      U64 door;
      for (i = 1; i <= log_r_nttlen; i++) {
        door         = 1 << (i - 1);
        size_t w_cnt = 0;
        for (j = 0; j < r_nttlen; j += (door << 1)) {
          for (k = 0; k < door; k++) {
            Set_element(w_2d_r, (i - 1) / 2,
                        (qi * split_num_r + spln) * r_nttlen + 2 * w_cnt +
                            ((i - 1) % 2),
                        c_nttlen, wn_s_r[Get_w_ind(i, k, log_r_nttlen)]);
            w_cnt++;
          }
        }
        assert(w_cnt == r_nttlen / 2);
      }
    }
    // generate w_2d_c
    for (size_t spln = 0; spln < split_num_c; spln++) {
      U64 i, j, k;
      U64 door;
      for (i = 1; i <= log_c_nttlen; i++) {
        door         = 1 << (i - 1);
        size_t w_cnt = 0;
        for (j = 0; j < c_nttlen; j += (door << 1)) {
          for (k = 0; k < door; k++) {
            Set_element(w_2d_c, (i - 1) / 2,
                        (qi * split_num_c + spln) * c_nttlen + 2 * w_cnt +
                            ((i - 1) % 2),
                        c_nttlen, wn_s_c[Get_w_ind(i, k, log_c_nttlen)]);
            w_cnt++;
          }
        }
        assert(w_cnt == c_nttlen / 2);
      }
    }
  }
  free(wn_s_r);
  free(wn_s_c);
}

void Hpu_conv_2d_to_1d(V64 w_1d, VV64 w_2d, U8 poly_num, U32 row_length,
                       U64 column_length, bool trs) {
  U64 r_nttlen;
  if (trs) {
    r_nttlen = row_length;
  } else {
    r_nttlen = column_length;
  }
  U64 split_num_r  = PEG_WIDTH / (r_nttlen * poly_num);
  U64 log_r_nttlen = Log2_comp(r_nttlen);

  // 2d to 1d transformation
  for (size_t qi = 0; qi < poly_num; qi++) {
    for (size_t spln = 0; spln < split_num_r; spln++) {
      U64 i, j, k;
      U64 door;
      for (i = 1; i <= log_r_nttlen; i++) {
        door         = 1 << (i - 1);
        size_t w_cnt = 0;
        for (j = 0; j < r_nttlen; j += (door << 1)) {
          for (k = 0; k < door; k++) {
            w_1d[Get_w_ind(i, k, log_r_nttlen)] =
                Element(w_2d, (i - 1) / 2,
                        (qi * split_num_r + spln) * r_nttlen + 2 * w_cnt +
                            ((i - 1) % 2),
                        r_nttlen);
            w_cnt++;
          }
        }
        assert(w_cnt == r_nttlen / 2);
      }
    }
  }
}