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

//! @brief Get input cipher by name and index
inline CIPHERTEXT Get_input_data(const char* name, size_t idx) {
  return Openfhe_get_input_data(name, idx);
}

//! @brief Set output cipher by name and index
inline void Set_output_data(const char* name, size_t idx, CIPHER data) {
  Openfhe_set_output_data(name, idx, data);
}

//! @brief Encode float array into plaintext
inline void Encode_float(PLAIN plain, float* input, size_t len,
                         uint32_t sc_degree, uint32_t level) {
  Openfhe_encode_float(plain, input, len, sc_degree, level);
}

// Encode a windowed "mask" plaintext: first `len` slots hold `cst`, the rest 0,
// at `level` with noiseScaleDeg = sc_degree (mirrors ant Encode_float_mask). The
// SIHE Chebyshev-relu value-range control emits
//   Encode_float_mask(&plain, cst, mask_len, sc_degree, Level(&ciph))
// and then Mul_plain(ciph, &plain) -- a windowed constant mask.
inline void Encode_float_mask(PLAIN plain, float cst, size_t len,
                              uint32_t sc_degree, uint32_t level) {
  Openfhe_encode_float_mask(plain, cst, len, sc_degree, level);
}

// HE Operations
inline CIPHER Add_ciph(CIPHER res, CIPHER op1, CIPHER op2) {
  Openfhe_add_ciph(res, op1, op2);
  return res;
}

// Array-element-operand overloads: the -VEC:sharding conv codegen passes
// VS_out_conv_halo*_n[idx] (a CIPHERTEXT object -- rt_def.h defines CIPHERTEXT
// as the shared_ptr object and CIPHER as the pointer to it) directly where
// these read a CIPHER. The compiler passes &-prefixed CIPHER temporaries as
// destinations, but array ELEMENTS as source operands without the & (the
// graph's resnet18-conv2d cross-shard halo sums; ant works because its
// CIPHERTEXT is a C struct that already decays). CIPHERTEXT passed by value is
// a refcounted copy; its address is valid for the synchronous C call below and
// the pointed-to Ciphertext is copied out before this overload returns. Only
// source (operand) positions ever receive the bare object.
inline CIPHER Add_ciph(CIPHER res, CIPHERTEXT op1, CIPHER op2) {
  Openfhe_add_ciph(res, &op1, op2);
  return res;
}
inline CIPHER Add_ciph(CIPHER res, CIPHER op1, CIPHERTEXT op2) {
  Openfhe_add_ciph(res, op1, &op2);
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

// Multiply a ciphertext by a plain double scalar (NO rescale; the generated
// graph emits Rescale_ciph afterward, e.g. GlobalAveragePool's 1/N: gap_hw8 and
// i64x8x8_gap use 0.015625 = 1/64). Mirrors rt_seal/rt_ace/rt_phantom
// Mul_scalar(res, op1, double). EvalMult(cipher, double) in OpenFHE multiplies
// every slot by the scalar without consuming a level.
inline CIPHER Mul_scalar(CIPHER res, CIPHER op1, double op2) {
  Openfhe_mul_scalar(res, op1, op2);
  return res;
}

// Add a plain double scalar to every slot (NO rescale; mirrors Mul_scalar). The
// SIHE Chebyshev-relu path emits Add_scalar(&res, &op, -1) for the low-bound
// shift. Openfhe_add_scalar -> EvalAdd(cipher, double) encodes the scalar at the
// cipher's current scale/level and adds it.
inline CIPHER Add_scalar(CIPHER res, CIPHER op1, double op2) {
  Openfhe_add_scalar(res, op1, op2);
  return res;
}

inline CIPHER Rotate_ciph(CIPHER res, CIPHER op, int step) {
  Openfhe_rotate(res, op, step);
  return res;
}
// Array-element operand (see Add_ciph note).
inline CIPHER Rotate_ciph(CIPHER res, CIPHERTEXT op, int step) {
  Openfhe_rotate(res, &op, step);
  return res;
}

inline CIPHER Rescale_ciph(CIPHER res, CIPHER op) {
  Openfhe_rescale(res, op);
  return res;
}
// Array-element operand (see Add_ciph note).
inline CIPHER Rescale_ciph(CIPHER res, CIPHERTEXT op) {
  Openfhe_rescale(res, &op);
  return res;
}

//! @brief Bootstrap a cipher: refresh near the chain top, then droplevel to the
//! IR target level. Mirrors the 3-arg form emitted by generated graphs
//! (Bootstrap(res, op, level)); the emitted level is the IR target the
//! bootstrapped cipher must land at.
inline CIPHER Bootstrap(CIPHER res, CIPHER op, int level) {
  Openfhe_bootstrap(res, op, level);
  return res;
}

//! @brief 4-arg Bootstrap overload: the -CKKS:sbm SIHE codegen emits
//! Bootstrap(res, op, level, slots). The 4th arg `slots` is informational (the
//! emitted 0 means full packing, which is what the EvalBootstrap context already
//! builds -- one num_slots = ring/2). rt_ace.h / rt_phantom.h carry the same
//! 4-arg form; ignore `slots` and forward to the 3-arg Openfhe_bootstrap.
inline CIPHER Bootstrap(CIPHER res, CIPHER op, int level, int slots) {
  (void)slots;
  Openfhe_bootstrap(res, op, level);
  return res;
}

inline CIPHER Mod_switch(CIPHER res, CIPHER op) {
  Openfhe_mod_switch(res, op);
  return res;
}

inline CIPHER Relin(CIPHER res, CIPHER3 ciph) {
  Openfhe_relin(res, ciph);
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

void Dump_cipher_msg(const char* name, CIPHER ct, uint32_t len);

void Dump_plain_msg(const char* name, PLAIN pt, uint32_t len);

double* Get_msg(CIPHER ct);

double* Get_msg_from_plain(PLAIN pt);

inline uint32_t Get_ciph_slots(CIPHER ct) { return Degree() / 2; }

inline uint32_t Get_plain_slots(PLAIN pt) { return Degree() / 2; }

bool Within_value_range(CIPHER ciph, double* msg, uint32_t len);

// for validate APIs
#include "common/rt_validate.h"

#endif  // RTLIB_RT_OPENFHE_RT_OPENFHE_H
