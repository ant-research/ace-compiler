//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_RT_OPENFHE_RT_OPENFHE_H
#define RTLIB_RT_OPENFHE_RT_OPENFHE_H

//! @brief rt_eval.h
//! Define common API for fhe-cmplr. Dispatch to specific implementations
//! built on top of OpenFHE libraries.

#include "common/rt_api.h"
#include "openfhe_api.h"

//! @brief Get polynomial degree
inline uint32_t Degree() { return Get_context_params()->_poly_degree; }

//! @brief get input cipher by name and index
inline CIPHERTEXT Get_input_data(const char* name, size_t idx) {
  return Openfhe_get_input_data(name, idx);
}

//! @brief set output cipher by name and index
inline void Set_output_data(const char* name, size_t idx, CIPHER data) {
  Openfhe_set_output_data(name, idx, data);
}

//! @brief encode float array into plaintext
inline void Encode_plain_from_float(PLAIN plain, float* input, size_t len,
                                    uint32_t sc_degree, uint32_t level) {
  Openfhe_encode_from_float(plain, input, len, sc_degree, level);
}

// HE Operations
inline CIPHER Add_ciph(CIPHER res, CIPHER op1, CIPHER op2) {
  Openfhe_add_ciph(res, op1, op2);
  return res;
}

inline CIPHER Add_plain(CIPHER res, CIPHER op1, PLAIN op2) {
  Openfhe_add_plain(res, op1, op2);
  return res;
}

inline CIPHER Mul_ciph(CIPHER res, CIPHER op1, CIPHER op2) {
  Openfhe_mul_ciph(res, op1, op2);
  return res;
}

inline CIPHER Mul_plain(CIPHER res, CIPHER op1, PLAIN op2) {
  Openfhe_mul_plain(res, op1, op2);
  return res;
}

inline CIPHER Rotate_ciph(CIPHER res, CIPHER op, int step) {
  Openfhe_rotate(res, op, step);
  return res;
}

inline void Copy_ciph(CIPHER res, CIPHER op) {
  if (res != op) {
    Openfhe_copy(res, op);
  }
}

inline void Zero_ciph(CIPHER res) { Openfhe_zero(res); }

// Level/Scale
uint64_t Sc_degree(CIPHER ct) { return (*ct)->GetNoiseScaleDeg(); }

inline uint64_t Level(CIPHER ct) { return (*ct)->GetLevel(); }

// Dump utilities
void Dump_ciph(CIPHER ct, size_t start, size_t len);

void Dump_plain(PLAIN pt, size_t start, size_t len);

#endif  // RTLIB_RT_OPENFHE_RT_OPENFHE_H
