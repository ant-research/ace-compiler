//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_COMMON_RT_VALIDATE_H
#define RTLIB_COMMON_RT_VALIDATE_H

//! @brief rt_validate.h
//! Define API prototypes for runtime validation

#include "common.h"
#include "error.h"
#include "tensor.h"

#ifdef __cplusplus
extern "C" {
#endif

// external function to compute on raw data
void    Validate_impl(double* res, double* msg, uint32_t len, int32_t epsilon);
void    Validate_value_range(bool within_range);
double* Add_impl(double* op0, double* op1, uint32_t len);
double* Mul_impl(double* op0, double* op1, uint32_t len);
double* Rotate_impl(double* op, uint32_t len, int32_t step);
double* Relu_impl(double* op, uint32_t len);
double* Conv_impl(double* msg0, int n, int c, int h, int w, float* weight,
                  int kn, int kc, int kh, int kw, float* bias, int bw, int sh,
                  int sw, int pn, int pc, int ph, int pw);
double* Gemm_impl(double* msg0, int h, int w, float* weight, int wh, int ww,
                  float* bias, int bw);
double* Average_pool_impl(double* msg0, int n, int c, int h, int w, int kh,
                          int kw, int sh, int sw, int pn, int pc, int ph,
                          int pw);
double* Global_average_pool_impl(double* msg0, int n, int c, int h, int w);

#ifdef CIPHER_DEFINED
// The following APIs are only valid when CIPHER/PLAIN is defined
// All functions below are "static inline" so that they can work with different
// definition of CIPHER/PLAIN

#ifndef PLAIN_DEFINED
#error "PLAIN is not defined."
#endif

static inline void Validate(CIPHER ciph, double* msg, uint32_t len,
                            int32_t epsilon) {
  double* res = Get_msg(ciph);
  Validate_impl(res, msg, len, epsilon);
  free(res);
  Validate_value_range(Within_value_range(ciph, msg, len));
}

static inline void Sharding_validate(CIPHER ciph, int n, int c, int h, int w,
                                     int n_n, int n_c, int n_h, int n_w,
                                     int o_h, int o_w, double* msg,
                                     int32_t epsilon) {
  int     len   = n * c * h * w;
  double* res   = (double*)malloc(len * sizeof(double));
  int     chunk = n_n * n_c * n_h * n_w;
  int     count = (n * c * h * w) / chunk;
  for (int i = 0; i < chunk; ++i) {
    double* data = Get_msg(&ciph[i]);
    if (o_h != 0 && chunk > 1) {
      if (i == 0) {
        memcpy(res + i * count, data, count);
      } else {
        memcpy(res + i * count, data + o_h * w, count);
      }
    }
    free(data);
  }
  Validate_impl(res, msg, len, epsilon);
  free(res);
}

static inline double* Add_plain_msg(CIPHER op0, PLAIN op1, uint32_t len) {
  IS_TRUE(len < Get_ciph_slots(op0), "op0 length mismatch");
  IS_TRUE(len < Get_plain_slots(op1), "op1 length mismatch");
  double* msg0 = Get_msg(op0);
  double* msg1 = Get_msg_from_plain(op1);
  double* res  = Add_impl(msg0, msg1, len);
  free(msg0);
  free(msg1);
  return res;
}

static inline double* Add_msg(CIPHER op0, CIPHER op1, uint32_t len) {
  IS_TRUE(len < Get_ciph_slots(op0), "op0 length mismatch");
  IS_TRUE(len < Get_ciph_slots(op1), "op1 length mismatch");
  double* msg0 = Get_msg(op0);
  double* msg1 = Get_msg(op1);
  double* res  = Add_impl(msg0, msg1, len);
  free(msg0);
  free(msg1);
  return res;
}

static inline double* Add_ref(double* op0, double* op1, uint32_t len) {
  return Add_impl(op0, op1, len);
}

static inline double* Mul_plain_msg(CIPHER op0, PLAIN op1, uint32_t len) {
  IS_TRUE(len < Get_ciph_slots(op0), "op0 length mismatch");
  IS_TRUE(len < Get_plain_slots(op1), "op1 length mismatch");
  double* msg0 = Get_msg(op0);
  double* msg1 = Get_msg_from_plain(op1);
  double* res  = Mul_impl(msg0, msg1, len);
  free(msg0);
  free(msg1);
  return res;
}

static inline double* Mul_msg(CIPHER op0, CIPHER op1, uint32_t len) {
  IS_TRUE(len < Get_ciph_slots(op0), "op0 length mismatch");
  IS_TRUE(len < Get_ciph_slots(op1), "op1 length mismatch");
  double* msg0 = Get_msg(op0);
  double* msg1 = Get_msg(op1);
  double* res  = Mul_impl(msg0, msg1, len);
  free(msg0);
  free(msg1);
  return res;
}

static inline double* Rotate_msg(CIPHER op0, int32_t step) {
  double*  msg0 = Get_msg(op0);
  uint32_t len  = Get_ciph_slots(op0);
  double*  res  = Rotate_impl(msg0, len, step);
  free(msg0);
  return res;
}

static inline double* Relu_msg(CIPHER op0, uint32_t len) {
  IS_TRUE(len < Get_ciph_slots(op0), "op0 length mismatch");
  double* msg0 = Get_msg(op0);
  double* res  = Relu_impl(msg0, len);
  free(msg0);
  return res;
}

static inline double* Relu_rtv(CIPHER op0, uint32_t len) {
  return Relu_msg(op0, len);
}

static inline double* Relu_ref(double* op0, uint32_t len) {
  return Relu_impl(op0, len);
}

static inline double* Bootstrap_msg(CIPHER op0) { return Get_msg(op0); }

static inline double* Conv_rtv(CIPHER op0, int n, int c, int h, int w,
                               float* weight, int kn, int kc, int kh, int kw,
                               float* bias, int bw, int sh, int sw, int pn,
                               int pc, int ph, int pw) {
  double* msg0 = Get_msg(op0);
  double* res  = Conv_impl(msg0, n, c, h, w, weight, kn, kc, kh, kw, bias, bw,
                           sh, sw, pn, pc, ph, pw);
  free(msg0);
  return res;
}

static inline double* Conv_ref(double* op0, int n, int c, int h, int w,
                               float* weight, int kn, int kc, int kh, int kw,
                               float* bias, int bw, int sh, int sw, int pn,
                               int pc, int ph, int pw) {
  return Conv_impl(op0, n, c, h, w, weight, kn, kc, kh, kw, bias, bw, sh, sw,
                   pn, pc, ph, pw);
}

static inline double* Sharding_conv_rtv(CIPHER op0, int n, int c, int h, int w,
                                        int n_n, int n_c, int n_h, int n_w,
                                        int o_h, int o_w, float* weight, int kn,
                                        int kc, int kh, int kw, float* bias,
                                        int bw, int sh, int sw, int pn, int pc,
                                        int ph, int pw) {
  int     chunk = n_n * n_c * n_h * n_w;
  double* msg0  = (double*)malloc(n * c * h * w * sizeof(double));
  int     count = (n * c * h * w) / chunk;
  for (int i = 0; i < chunk; ++i) {
    double* data = Get_msg(&op0[i]);
    if (o_h != 0 && chunk > 1) {
      if (i == 0) {
        memcpy(msg0 + i * count, data, count);
      } else {
        memcpy(msg0 + i * count, data + o_h * w, count);
      }
    }
    free(data);
  }
  double* res = Conv_impl(msg0, n, c, h, w, weight, kn, kc, kh, kw, bias, bw,
                          sh, sw, pn, pc, ph, pw);
  free(msg0);
  return res;
}

static inline double* Gemm_rtv(CIPHER op0, int h, int w, float* weight, int wh,
                               int ww, float* bias, int bw) {
  double* msg0 = Get_msg(op0);
  double* res  = Gemm_impl(msg0, h, w, weight, wh, ww, bias, bw);
  free(msg0);
  return res;
}

static inline double* Gemm_ref(double* op0, int h, int w, float* weight, int wh,
                               int ww, float* bias, int bw) {
  return Gemm_impl(op0, h, w, weight, wh, ww, bias, bw);
}

static inline double* Average_pool_rtv(CIPHER op0, int n, int c, int h, int w,
                                       int kh, int kw, int sh, int sw, int pn,
                                       int pc, int ph, int pw) {
  double* msg0 = Get_msg(op0);
  double* res =
      Average_pool_impl(msg0, n, c, h, w, kh, kw, sh, sw, pn, pc, ph, pw);
  free(msg0);
  return res;
}

static inline double* Average_pool_ref(double* op0, int n, int c, int h, int w,
                                       int kh, int kw, int sh, int sw, int pn,
                                       int pc, int ph, int pw) {
  return Average_pool_impl(op0, n, c, h, w, kh, kw, sh, sw, pn, pc, ph, pw);
}

static inline double* Global_average_pool_rtv(CIPHER op0, int n, int c, int h,
                                              int w) {
  double* msg0 = Get_msg(op0);
  double* res  = Global_average_pool_impl(msg0, n, c, h, w);
  free(msg0);
  return res;
}

static inline double* Global_average_pool_ref(double* op0, int n, int c, int h,
                                              int w) {
  return Global_average_pool_impl(op0, n, c, h, w);
}

static inline double* Max_pool_rtv(CIPHER op0, int n, int c, int h, int w,
                                   int kh, int kw, int sh, int sw, int pn,
                                   int pc, int ph, int pw) {
  return Average_pool_rtv(op0, n, c, h, w, kh, kw, sh, sw, pn, pc, ph, pw);
}

static inline double* Max_pool_ref(double* op0, int n, int c, int h, int w,
                                   int kh, int kw, int sh, int sw, int pn,
                                   int pc, int ph, int pw) {
  return Average_pool_ref(op0, n, c, h, w, kh, kw, sh, sw, pn, pc, ph, pw);
}

#endif  // defined(CIPHER_DEFINED)

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_COMMON_RT_VALIDATE_H
