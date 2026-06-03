//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_CONTEXT_H
#define RTLIB_INCLUDE_CONTEXT_H

#include "util/crt.h"
#include "util/modular.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef char* PTR_TY;

//! @brief CKKS_CONTEXT, generated from CKKS_PARAMS
typedef struct {
  PTR_TY _params;
  PTR_TY _key_generator;
  PTR_TY _encoder;
  PTR_TY _encryptor;
  PTR_TY _decryptor;
  PTR_TY _evaluator;
} CKKS_CONTEXT;

extern CKKS_CONTEXT* Context;

//! @brief Allocate a ckks context with malloc
static inline CKKS_CONTEXT* Alloc_ckks_context() {
  CKKS_CONTEXT* ctxt = (CKKS_CONTEXT*)malloc(sizeof(CKKS_CONTEXT));
  memset(ctxt, 0, sizeof(CKKS_CONTEXT));
  return ctxt;
}

//! @brief Get param from CKKS_CONTEXT
//! @param context ckks context
//! @return PTR_TY
static inline PTR_TY Param() {
  FMT_ASSERT(Context, "invalid Context");
  return Context->_params;
}

//! @brief Get key generator
//! @param context
//! @return PTR_TY
static inline PTR_TY Keygen() {
  FMT_ASSERT(Context, "invalid Context");
  return Context->_key_generator;
}

//! @brief Get ckks encoder
static inline PTR_TY Encoder() {
  FMT_ASSERT(Context, "invalid Context");
  return Context->_encoder;
}

//! @brief Get ckks encryptor
static inline PTR_TY Encryptor() {
  FMT_ASSERT(Context, "invalid Context");
  return Context->_encryptor;
}

//! @brief Get ckks decryptor
static inline PTR_TY Decryptor() {
  FMT_ASSERT(Context, "invalid Context");
  return Context->_decryptor;
}

//! @brief Get ckks evaluator
static inline PTR_TY Eval() {
  FMT_ASSERT(Context, "invalid Context");
  return Context->_evaluator;
}

//! @brief Get poly degree from CKKS context
//! @return uint32_t
uint32_t Degree();

//! @brief Get the num of q parts
//! @return size_t
size_t Get_q_parts();

//! @brief Get crt context
//! @return CRT_CONTEXT*
CRT_CONTEXT* Get_crt_context();

//! @brief Get number of q prime
//! @return size_t
size_t Get_q_cnt();

//! @brief Get number of p prime
//! @return size_t
size_t Get_p_cnt();

//! @brief Get size of Q Part precomputed
//! @return size_t
size_t Get_part_size();

//! @brief Get q modulus from crt context
//! @return q modulus
MODULUS* Q_modulus();

//! @brief Get p modulus from crt context
//! @return p modulus
MODULUS* P_modulus();

//! @brief Get default scaling factor
//! @return double
double Get_default_sc();

//! @brief Get NTT_CONTEXT at given idx
NTT_CONTEXT* Get_ntt_ctx(size_t idx);

//! @brief Generate precom for bootstrapping with input slots
void Bootstrap_precom(uint32_t num_slots);

//! @brief Set crt context to global CKKS_CONTEXT
void Set_global_crt_context(PTR_TY crt);

//! @brief Set ckks parameter to global CKKS_CONTEXT
void Set_global_params(PTR_TY params);

//! @brief Set ckks encoder to global CKKS_CONTEXT
void Set_global_encoder(PTR_TY encoder);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_CONTEXT_H
