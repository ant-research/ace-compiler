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

//! @brief Get input cipher by name and index
inline CIPHERTEXT Get_input_data(const char* name, size_t idx) {
  return Seal_get_input_data(name, idx);
}

//! @brief Set output cipher by name and index
inline void Set_output_data(const char* name, size_t idx, CIPHER data) {
  Seal_set_output_data(name, idx, data);
}

//! @brief Encode float array into plaintext.
//!  The 4th parameter is named with 'sc_degree' to match with other libraries
//!  but it's actually the scale value = pow(2.0, sc_degree).
inline void Encode_float(PLAIN plain, float* input, size_t len,
                         SCALE_T sc_degree, LEVEL_T level) {
  Seal_encode_float(plain, input, len, sc_degree, level);
}

inline void Encode_float_cst_lvl(PLAIN plain, float* input, size_t len,
                                 SCALE_T sc_degree, int level) {
  Seal_encode_float_cst_lvl(plain, input, len, sc_degree, level);
}

inline void Encode_double(PLAIN plain, double* input, size_t len,
                          SCALE_T sc_degree, LEVEL_T level) {
  Seal_encode_double(plain, input, len, sc_degree, level);
}

inline void Encode_double_cst_lvl(PLAIN plain, double* input, size_t len,
                                  SCALE_T sc_degree, int level) {
  Seal_encode_double_cst_lvl(plain, input, len, sc_degree, level);
}

inline void Encode_float_mask(PLAIN plain, float input, size_t len,
                              SCALE_T sc_degree, LEVEL_T level) {
  Seal_encode_float_mask(plain, input, len, sc_degree, level);
}

inline void Encode_float_mask_cst_lvl(PLAIN plain, float input, size_t len,
                                      SCALE_T sc_degree, int level) {
  Seal_encode_float_mask_cst_lvl(plain, input, len, sc_degree, level);
}

inline void Encode_double_mask(PLAIN plain, double input, size_t len,
                               SCALE_T sc_degree, LEVEL_T level) {
  Seal_encode_double_mask(plain, input, len, sc_degree, level);
}

inline void Encode_double_mask_cst_lvl(PLAIN plain, double input, size_t len,
                                       SCALE_T sc_degree, int level) {
  Seal_encode_double_mask_cst_lvl(plain, input, len, sc_degree, level);
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

inline CIPHER Add_scalar(CIPHER res, CIPHER op1, double op2) {
  Seal_add_scalar(res, op1, op2);
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

inline CIPHER Mul_scalar(CIPHER res, CIPHER op1, double op2) {
  Seal_mul_scalar(res, op1, op2);
  return res;
}

inline CIPHER Rotate_ciph(CIPHER res, CIPHER op, int step) {
  Seal_rotate(res, op, step);
  return res;
}

inline CIPHER Rescale_ciph(CIPHER res, CIPHER op) {
  Seal_rescale(res, op);
  return res;
}

inline CIPHER Mod_switch(CIPHER res, CIPHER op) {
  Seal_mod_switch(res, op);
  return res;
}

inline CIPHER Relin(CIPHER res, CIPHER3 ciph) {
  Seal_relin(res, ciph);
  return res;
}

#ifdef SEAL_BTS_MACRO
inline CIPHER Bootstrap(CIPHER res, CIPHER op, int level) {
  Seal_bootstrap(res, op, level);
  return res;
}
#endif

inline void Copy_ciph(CIPHER res, CIPHER op) {
  if (res != op) {
    Seal_copy(res, op);
  }
}

inline void Zero_ciph(CIPHER res) { Seal_zero(res); }

inline void Free_ciph(CIPHER res) { res->release(); }

inline void Free_plain(PLAIN res) { res->release(); }

// We can get scale = pow(2.0, sc_degree) from SEAL Ciphertext, so we continue
// use the double value but keep the interface named with "Sc_degree" to match
// with other libraries.
SCALE_T Sc_degree(CIPHER ct) { return Seal_scale(ct); }

LEVEL_T Level(CIPHER ct) { return Seal_level(ct); }

// Dump utilities
void Dump_ciph(CIPHER ct, size_t start, size_t len);

void Dump_plain(PLAIN pt, size_t start, size_t len);

void Dump_cipher_msg(const char* name, CIPHER ct, uint32_t len);

void Dump_plain_msg(const char* name, PLAIN pt, uint32_t len);

double* Get_msg(CIPHER ct);

double* Get_msg_from_plain(PLAIN pt);

inline uint32_t Get_ciph_slots(CIPHER ct) { return Degree() / 2; }

inline uint32_t Get_plain_slots(PLAIN pt) { return Degree() / 2; }

bool Within_value_range(CIPHER ciph, double* msg, uint32_t len);

// for validate APIs
#include "common/rt_validate.h"

#endif  // RTLIB_RT_SEAL_RT_SEAL_H
