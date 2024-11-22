//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_RT_SEAL_RT_SEAL_H
#define RTLIB_RT_SEAL_RT_SEAL_H

//! @brief rt_eval.h
//! Define common API for fhe-cmplr. Dispatch to specific implementations
//! built on top of SEAL libraries.

#include "common/rt_api.h"
#include "seal_api.h"

//! @brief Get polynomial degree
inline uint32_t Degree() { return Get_context_params()->_poly_degree; }

//! @brief get input cipher by name and index
inline CIPHERTEXT Get_input_data(const char* name, size_t idx) {
  return Seal_get_input_data(name, idx);
}

//! @brief set output cipher by name and index
inline void Set_output_data(const char* name, size_t idx, CIPHER data) {
  Seal_set_output_data(name, idx, data);
}

//! @brief encode float array into plaintext
inline void Encode_plain_from_float(PLAIN plain, float* input, size_t len,
                                    uint32_t sc_degree, uint32_t level) {
  Seal_encode_from_float(plain, input, len, sc_degree, level);
}

// HE Operations
inline CIPHER Add_ciph(CIPHER res, CIPHER op1, CIPHER op2) {
  Seal_add_ciph(res, op1, op2);
  return res;
}

inline CIPHER Add_plain(CIPHER res, CIPHER op1, PLAIN op2) {
  Seal_add_plain(res, op1, op2);
  return res;
}

inline CIPHER Mul_ciph(CIPHER res, CIPHER op1, CIPHER op2) {
  Seal_mul_ciph(res, op1, op2);
  return res;
}

inline CIPHER Mul_plain(CIPHER res, CIPHER op1, PLAIN op2) {
  Seal_mul_plain(res, op1, op2);
  return res;
}

inline CIPHER Rotate_ciph(CIPHER res, CIPHER op, int step) {
  Seal_rotate(res, op, step);
  return res;
}

inline void Copy_ciph(CIPHER res, CIPHER op) {
  if (res != op) {
    Seal_copy(res, op);
  }
}

inline void Zero_ciph(CIPHER res) { Seal_zero(res); }

// Level/Scale
uint64_t Sc_degree(CIPHER ct) { return Seal_sc_degree(ct); }

uint64_t Level(CIPHER ct) { return Seal_level(ct); }

// Dump utilities
void Dump_ciph(CIPHER ct, size_t start, size_t len);

void Dump_plain(PLAIN pt, size_t start, size_t len);

#endif  // RTLIB_RT_SEAL_RT_SEAL_H
