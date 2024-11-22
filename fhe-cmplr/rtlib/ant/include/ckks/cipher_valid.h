//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================
#ifndef RTLIB_INCLUDE_CIPHER_VALID_H
#define RTLIB_INCLUDE_CIPHER_VALID_H

#include "ckks/cipher_eval.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @brief Validate by decrypt & decode ciphertext and compare with msg
void Validate(CIPHER ciph, double* msg, uint32_t len, int32_t epsilon);

//! @brief Add ciphertext with plaintext by decrypt & decode
double* Add_plain_msg(CIPHER op0, PLAIN op1);

//! @brief Add ciphertext with ciphertext by decrypt & decode
double* Add_msg(CIPHER op0, CIPHER op1, uint64_t len);
double* Add_ref(double* op0, double* op1, uint64_t len);

//! @brief Mul ciphertext with plaintext by decrypt & decode
double* Mul_plain_msg(CIPHER op0, PLAIN op1);

//! @brief Mul ciphertext with ciphertext by decrypt & decode
double* Mul_msg(CIPHER op0, CIPHER op1);

//! @brief Rotate ciphertext by decrypt & decode
double* Rotate_msg(CIPHER op0, int32_t rotation);

//! @brief Relu ciphertext by decrypt & decode
double* Relu_msg(CIPHER op0, uint64_t len);
double* Relu_rtv(CIPHER op0, uint64_t len);
double* Relu_ref(double* op0, uint64_t len);

double* Bootstrap_msg(CIPHER op0);

//! @brief Conv ciphertext with weight and bias
double* Conv_rtv(CIPHER op0, int n, int c, int h, int w, float* weight, int kn,
                 int kc, int kh, int kw, float* bias, int bw, int sh, int sw,
                 int pn, int pc, int ph, int pw);
double* Conv_ref(double* op0, int n, int c, int h, int w, float* weight, int kn,
                 int kc, int kh, int kw, float* bias, int bw, int sh, int sw,
                 int pn, int pc, int ph, int pw);

double* Gemm_rtv(CIPHER op0, int h, int w, float* weight, int wh, int ww,
                 float* bias, int bw);
double* Gemm_ref(double* op0, int h, int w, float* weight, int wh, int ww,
                 float* bias, int bw);

double* Average_pool_rtv(CIPHER op0, int n, int c, int h, int w, int kh, int kw,
                         int sh, int sw, int pn, int pc, int ph, int pw);
double* Average_pool_ref(double* op0, int n, int c, int h, int w, int kh,
                         int kw, int sh, int sw, int pn, int pc, int ph,
                         int pw);

double* Max_pool_rtv(CIPHER op0, int n, int c, int h, int w, int kh, int kw,
                     int sh, int sw, int pn, int pc, int ph, int pw);
double* Max_pool_ref(double* op0, int n, int c, int h, int w, int kh, int kw,
                     int sh, int sw, int pn, int pc, int ph, int pw);

double* Global_average_pool_rtv(CIPHER op0, int n, int c, int h, int w);
double* Global_average_pool_ref(double* op0, int n, int c, int h, int w);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_CIPHER_VALID_H
