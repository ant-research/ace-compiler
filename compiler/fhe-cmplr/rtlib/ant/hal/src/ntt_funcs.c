//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "ntt_funcs.h"

// convert from 1d-format to 2d-format
static void Conv_1d_to_2d(VV64 dst, V64 src, U64 rlen, U64 clen,
                          bool is_trans) {
  for (size_t i = 0; i < rlen; i++) {
    for (size_t j = 0; j < clen; j++) {
      if (is_trans) {
        Set_element(dst, j, i, rlen, src[i * clen + j]);
      } else {
        Set_element(dst, i, j, clen, src[i * clen + j]);
      }
    }
  }
}

// convert from 2d-format to 1d-format
static void Conv_2d_to_1d(V64 dst, VV64 src, U64 rlen, U64 clen,
                          bool is_trans) {
  for (size_t i = 0; i < clen; i++) {
    for (size_t j = 0; j < rlen; j++) {
      dst[i * rlen + j] =
          is_trans ? Element(src, j, i, clen) : Element(src, i, j, rlen);
    }
  }
}

// transformat 2d-format
static void Fhe_trans_m(VV64 dst, VV64 src, U64 rlen, U64 clen) {
  assert(dst != src);
  for (size_t i = 0; i < rlen; i++) {
    for (size_t j = 0; j < clen; j++) {
      Set_element(dst, j, i, rlen, Element(src, i, j, clen));
    }
  }
}

// modular multiplication for 2d-fromat
static void Fhe_mul_mm(VV64 dst, VV64 src1, VV64 src2, U64 rlen, U64 clen,
                       U64 q) {
  for (size_t i = 0; i < rlen; i++) {
    for (size_t j = 0; j < clen; j++) {
      Set_element(
          dst, i, j, clen,
          Modmul(Element(src1, i, j, clen), Element(src2, i, j, clen), q));
    }
  }
}

// transform for 1d-format, which is used in Fhe_ntt_mm()
// same with Ntt_1d/Intt_1d except precomputed part are extracted and passed in
// as parameters prec.
static void Transform(V64 f, V64 prec, V64 t, U64 len, U64 q) {
  U64 log_len = Log2_comp(len);
  U64 i, j, k;
  U64 antennae, door;
  U64 a, b, w, c, d;
  Poly_bit_rev(f, t, log_len);
  for (i = 1; i <= log_len; i++) {
    door = 1 << (i - 1);
    for (j = 0; j < len; j += (door << 1)) {
      for (k = 0; k < door; k++) {
        antennae           = j + k;
        a                  = f[antennae];
        b                  = f[antennae + door];
        w                  = prec[Get_w_ind(i, k, log_len)];
        U64 wb             = Modmul(w, b, q);
        c                  = Modadd(a, wb, q);
        d                  = Modsub(a, wb, q);
        f[antennae]        = c;
        f[antennae + door] = d;
      }
    }
  }
}

// perform ntt in 2d-format
static void Fhe_ntt_mm(VV64 dst, VV64 src, U64 rlen, U64 clen, U64 q,
                       V64 w_dir_1d, bool is_ntt_r) {
  size_t outer_loop = is_ntt_r ? rlen : clen;
  size_t inner_loop = is_ntt_r ? clen : rlen;
  V64    src_inner  = (V64)malloc(sizeof(U64) * inner_loop);
  V64    dst_inner  = (V64)malloc(sizeof(U64) * inner_loop);
  assert(src != NULL && dst != NULL);
  for (size_t i = 0; i < outer_loop; i++) {
    for (size_t j = 0; j < inner_loop; j++) {
      src_inner[j] = Element(src, i, j, inner_loop);
    }
    Transform(dst_inner, w_dir_1d, src_inner, inner_loop, q);
    for (size_t j = 0; j < inner_loop; j++) {
      Set_element(dst, i, j, inner_loop, dst_inner[j]);
    }
  }
  free(src_inner);
  free(dst_inner);
}

// 1step-ntt in 1d-format
static void Ntt_1d(V64 f, V64 t, U64 len, U64 q) {
  U64 log_len = Log2_comp(len);
  V64 wn_s    = (V64)malloc(sizeof(U64) * (len >> 1));
  wn_s[0]     = 1;
  U64 fin     = Find_2nth_root(len, q);
  wn_s[1]     = Modmul(fin, fin, q);
  for (size_t i = 2; i < (len >> 1); i++) {
    wn_s[i] = Modmul(wn_s[i - 1], wn_s[1], q);
  }
  U64 i, j, k;
  U64 antennae, door;
  U64 a, b, w, c, d;
  Poly_bit_rev(f, t, log_len);
  for (i = 1; i <= log_len; i++) {
    door = 1 << (i - 1);
    for (j = 0; j < len; j += (door << 1)) {
      for (k = 0; k < door; k++) {
        antennae           = j + k;
        a                  = f[antennae];
        b                  = f[antennae + door];
        w                  = wn_s[Get_w_ind(i, k, log_len)];
        U64 wb             = Modmul(w, b, q);
        c                  = Modadd(a, wb, q);
        d                  = Modsub(a, wb, q);
        f[antennae]        = c;
        f[antennae + door] = d;
      }
    }
  }
  free(wn_s);
}

// reference model for 1step-intt in 1d-format.
static void Intt_1d(V64 f, V64 t, U64 len, U64 q) {
  U64 log_len = Log2_comp(len);
  V64 wn_s    = (V64)malloc(sizeof(U64) * (len >> 1));
  wn_s[0]     = 1;
  U64 fin     = Find_2nth_root(len, q);
  fin         = Modinv(fin, q);
  wn_s[1]     = Modmul(fin, fin, q);
  for (size_t i = 2; i < (len >> 1); i++) {
    wn_s[i] = Modmul(wn_s[i - 1], wn_s[1], q);
  }
  U64 i, j, k;
  U64 antennae, door;
  U64 a, b, w, c, d;
  Poly_bit_rev(f, t, log_len);
  for (i = 1; i <= log_len; i++) {
    door = 1 << (i - 1);
    for (j = 0; j < len; j += (door << 1)) {
      for (k = 0; k < door; k++) {
        antennae           = j + k;
        a                  = f[antennae];
        b                  = f[antennae + door];
        w                  = wn_s[Get_w_ind(i, k, log_len)];
        U64 wb             = Modmul(w, b, q);
        c                  = Modadd(a, wb, q);
        d                  = Modsub(a, wb, q);
        f[antennae]        = c;
        f[antennae + door] = d;
      }
    }
  }
  free(wn_s);
}

// function for generating "Negative Cyclic Convolution" twiddle factors in
// natural 1d-format.
static void Ntt_ncy_1d_ref(V64 poly_ncy, U64 len, U64 q, bool is_inv) {
  poly_ncy[0] = is_inv ? Modinv(len, q) : 1;
  U64 fin     = Find_2nth_root(len, q);
  if (is_inv) {
    fin = Modinv(fin, q);
  }
  for (size_t i = 1; i < len; i++) {
    poly_ncy[i] = Modmul(fin, poly_ncy[i - 1], q);
  }
}

void One_step_ntt(V64 dst, V64 src, U64 len, U64 q) {
  V64 ncy_w = (V64)malloc(sizeof(U64) * len);
  Ntt_ncy_1d_ref(ncy_w, len, q, 0);
  for (size_t i = 0; i < len; i++) {
    dst[i] = Modmul(ncy_w[i], src[i], q);
  }
  free(ncy_w);
  Ntt_1d(dst, dst, len, q);
}

void One_step_intt(V64 dst, V64 src, U64 len, U64 q) {
  Intt_1d(dst, src, len, q);
  V64 ncy_w = (V64)malloc(sizeof(U64) * len);
  Ntt_ncy_1d_ref(ncy_w, len, q, 1);
  for (size_t i = 0; i < len; i++) {
    dst[i] = Modmul(ncy_w[i], dst[i], q);
  }
  free(ncy_w);
}

void Four_step_ntt(V64 dst, V64 src, VV64 w_2d_ncy, VV64 w_2d_apptwi,
                   V64 w_2d_dir_r_1d, V64 w_2d_dir_c_1d, U64 rlen, U64 clen,
                   U64 q, bool input_transpose) {
  VV64 src_2d       = Alloc_poly_2d(rlen, clen);
  VV64 src_2d_trans = Alloc_poly_2d(rlen, clen);
  VV64 dst_2d       = Alloc_poly_2d(rlen, clen);

  // step0: prepare input poly (convert src from 1d to 2d)
  Conv_1d_to_2d(src_2d, src, rlen, clen, input_transpose);

  // step1: ncy
  Fhe_mul_mm(src_2d, src_2d, w_2d_ncy, rlen, clen, q);

  // step2: trans for no-transposed format
  if (input_transpose) {
    Copy_poly_2d(src_2d_trans, src_2d, rlen, clen);
  } else {
    Fhe_trans_m(src_2d_trans, src_2d, rlen, clen);
  }

  // step3: ntt_C
  Fhe_ntt_mm(src_2d_trans, src_2d_trans, rlen, clen, q, w_2d_dir_r_1d, false);

  // step4: trans
  Fhe_trans_m(src_2d, src_2d_trans, rlen, clen);

  // step5: w_2d_apptwi
  Fhe_mul_mm(src_2d, src_2d, w_2d_apptwi, rlen, clen, q);

  // step6: ntt_R
  Fhe_ntt_mm(src_2d, src_2d, rlen, clen, q, w_2d_dir_c_1d, true);

  // step7: trans
  Fhe_trans_m(dst_2d, src_2d, rlen, clen);

  // step8: conv dst to 1d
  Conv_2d_to_1d(dst, dst_2d, rlen, clen, false);

  // free memory
  Free_poly_2d(src_2d);
  Free_poly_2d(src_2d_trans);
  Free_poly_2d(dst_2d);
}

void Four_step_intt(V64 dst, V64 src, VV64 w_2d_ncy, VV64 w_2d_apptwi,
                    V64 w_2d_dir_r_1d, V64 w_2d_dir_c_1d, U64 rlen, U64 clen,
                    U64 q, bool input_transpose) {
  VV64 src_2d       = Alloc_poly_2d(clen, rlen);
  VV64 src_2d_trans = Alloc_poly_2d(clen, rlen);
  VV64 dst_2d       = Alloc_poly_2d(rlen, clen);

  // step0: prepare input poly (convert src from 1d to 2d)
  Conv_1d_to_2d(src_2d, src, rlen, clen, input_transpose);

  // step1: trans for no-transposed format
  if (input_transpose) {
    Copy_poly_2d(src_2d_trans, src_2d, rlen, clen);
  } else {
    Fhe_trans_m(src_2d_trans, src_2d, rlen, clen);
  }

  // step2: ntt_R
  Fhe_ntt_mm(src_2d_trans, src_2d_trans, rlen, clen, q, w_2d_dir_c_1d, true);

  // step3: w_2d_apptwi
  Fhe_mul_mm(src_2d_trans, src_2d_trans, w_2d_apptwi, rlen, clen, q);

  // step4: trans
  Fhe_trans_m(src_2d, src_2d_trans, rlen, clen);

  // step5: ntt_C
  Fhe_ntt_mm(src_2d, src_2d, rlen, clen, q, w_2d_dir_r_1d, false);

  // step6: trans for no-transposed format
  if (input_transpose) {
    Copy_poly_2d(dst_2d, src_2d, rlen, clen);
  } else {
    Fhe_trans_m(dst_2d, src_2d, rlen, clen);
  }

  // step7: ncy
  Fhe_mul_mm(dst_2d, dst_2d, w_2d_ncy, rlen, clen, q);

  // step8: dst conv to 1d
  Conv_2d_to_1d(dst, dst_2d, rlen, clen, input_transpose);

  // free memory
  Free_poly_2d(src_2d);
  Free_poly_2d(src_2d_trans);
  Free_poly_2d(dst_2d);
}