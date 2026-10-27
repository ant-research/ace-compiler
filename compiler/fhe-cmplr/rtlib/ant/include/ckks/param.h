//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_CKKS_PARAMETERS_H
#define RTLIB_INCLUDE_CKKS_PARAMETERS_H

#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "util/crt.h"
#include "util/std_param.h"
#include "util/type.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @brief An instance of parameters for the CKKS RNS scheme
typedef struct {
  uint32_t       _poly_degree;
  SECURITY_LEVEL _sec_level;
  size_t         _multiply_depth;
  size_t         _input_level;
  size_t         _num_primes;
  size_t         _num_p_primes;
  size_t         _num_q_parts;
  size_t         _hamming_weight;
  CRT_CONTEXT*   _crt_context;
  uint32_t       _first_mod_size;  // bits of first modulus
  double         _scaling_factor;
} CKKS_PARAMETER;

//! @brief Initialize Parameters with multiply_depth
//! get from pre-compute default primes
//! @param param parameters to be initialized
//! @param poly_degree degree d of polynomial of ring R
//! @param level security level
//! @param multiply_depth max depth of multiply
void Init_ckks_parameters_with_multiply_depth(CKKS_PARAMETER* param,
                                              uint32_t        poly_degree,
                                              SECURITY_LEVEL  level,
                                              size_t          multiply_depth,
                                              size_t          hamming_weight);

//! @brief Initialize Parameters with mod size
//! @param param parameters to be initialized
//! @param poly_degree degree d of polynomial of ring R
//! @param level security level
//! @param prime_len length of q primes
//! @param input_lev level of input ciphertext
//! @param first_mod_size bit size of first mod
//! @param scaling_mod_size bit size of scaling factor
void Init_ckks_parameters_with_prime_size(
    CKKS_PARAMETER* param, uint32_t poly_degree, SECURITY_LEVEL level,
    size_t prime_len, size_t input_lev, size_t first_mod_size,
    size_t scaling_mod_size, size_t hamming_weight);

//! @brief Alloca memory for ckks parameter
CKKS_PARAMETER* Alloc_ckks_parameter();

//! @brief Get bit counts of coefficient modulus(Q)
static inline size_t Get_coeff_bit_count(size_t q_cnt, CKKS_PARAMETER* param) {
  IS_TRUE(param->_num_primes > 0,
          "length of q primes should be greater than 0");
  return param->_first_mod_size +
         (q_cnt - 1) * (uint32_t)log2(param->_scaling_factor);
}

//! @brief Set the mult depth for ckks parameters
//! @param param ckks parameters
//! @param depth multiply depth
static inline void Set_mult_depth(CKKS_PARAMETER* param, size_t depth) {
  param->_multiply_depth = depth;
}

//! @brief Get the mult depth from ckks parameters
//! @param param ckks parameters
//! @return size_t
static inline size_t Get_mult_depth(CKKS_PARAMETER* param) {
  return param->_multiply_depth;
}

//! @brief Set the num q parts for parameters
//! @param param ckks parameter
//! @param digits input number
static inline void Set_num_q_parts(CKKS_PARAMETER* param, size_t digits) {
  param->_num_q_parts = digits;
}

//! @brief Get the num q parts of parameters
//! @param param ckks parameter
static inline size_t Get_num_q_parts(CKKS_PARAMETER* param) {
  return param->_num_q_parts;
}

//! @brief Get poly degree
//! @param param ckks paramter
//! @return uint32_t
static inline uint32_t Get_param_degree(CKKS_PARAMETER* param) {
  return param->_poly_degree;
}

//! @brief Get scaling factor
//! @param param ckks paramter
//! @return double
static inline double Get_param_sc(CKKS_PARAMETER* param) {
  return param->_scaling_factor;
}

//! @brief Get crt context
//! @param param ckks paramter
//! @return CRT_CONTEXT*
static inline CRT_CONTEXT* Get_param_crt(CKKS_PARAMETER* param) {
  return param->_crt_context;
}

//! @brief Set crt context
static inline void Set_param_crt(CKKS_PARAMETER* param, CRT_CONTEXT* crt) {
  if (param->_crt_context) {
    Free_crtcontext(param->_crt_context);
  }
  param->_crt_context = crt;
}

//! @brief Cleanup ckks parameter memory
//! @param param ckks parameter need to cleanup
void Free_ckks_parameters(CKKS_PARAMETER* param);

//! @brief Prints ckks prameters
//! @param fp output file
//! @param param CKKS_PARAMETER*
void Print_param(FILE* fp, CKKS_PARAMETER* param);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_CKKS_PARAMETERS_H