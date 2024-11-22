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
#include "util/fhe_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef char* PTR_TY;

/**
 * @brief ckks context, generated from CKKS_PARAMS
 *
 */
typedef struct {
  PTR_TY _params;
  PTR_TY _key_generator;
  PTR_TY _encoder;
  PTR_TY _encryptor;
  PTR_TY _decryptor;
  PTR_TY _evaluator;
} CKKS_CONTEXT;

extern CKKS_CONTEXT* Context;

/**
 * @brief Get param from CKKS_CONTEXT
 *
 * @param context ckks context
 * @return PTR_TY
 */
static inline PTR_TY Get_param(CKKS_CONTEXT* context) {
  return context->_params;
}

/**
 * @brief Get key generator
 *
 * @param context
 * @return PTR_TY
 */
static inline PTR_TY Get_key_gen(CKKS_CONTEXT* context) {
  return context->_key_generator;
}

//! @brief Get ckks evaluator
static inline PTR_TY Get_eval(CKKS_CONTEXT* context) {
  return context->_evaluator;
}

/**
 * @brief get poly degree from CKKS context
 *
 * @return uint32_t
 */
uint32_t Degree();

/**
 * @brief get the num of q parts
 *
 * @return size_t
 */
size_t Get_q_parts();

/**
 * @brief get crt context
 *
 * @return CRT_CONTEXT*
 */
CRT_CONTEXT* Get_crt_context();

/**
 * @brief get number of p prime
 *
 * @return size_t
 */
size_t Get_p_cnt();

/**
 * @brief Get size of Q Part precomputed
 *
 * @return size_t
 */
size_t Get_part_size();

/**
 * @brief Get q modulus from crt context
 *
 * @return q modulus
 */
MODULUS* Q_modulus();

/**
 * @brief Get p modulus from crt context
 *
 * @return p modulus
 */
MODULUS* P_modulus();

/**
 * @brief Get default scaling factor
 *
 * @return double
 */
double Get_default_sc();

//! @brief Generate precom for bootstrapping with input slots
void Bootstrap_precom(uint32_t num_slots);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_CONTEXT_H
