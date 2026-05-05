//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

// #include "helper.h"
#include <stdlib.h>
#include <string.h>

#include "common/common.h"  // for CKKS_PARAMS
#include "common/error.h"

static CKKS_PARAMS* Param = NULL;
extern "C" {

CKKS_PARAMS* Get_context_params() {
  IS_TRUE(Param != NULL, "need Set_context_params first !");
  return Param;
}

RT_DATA_INFO* Get_rt_data_info() { return NULL; }
int           Get_input_count() { return 0; }
int           Get_output_count() { return 0; }
DATA_SCHEME*  Get_encode_scheme(int idx) { return NULL; }
DATA_SCHEME*  Get_decode_scheme(int idx) { return NULL; }
}

CKKS_PARAMS* Set_context_params(uint32_t degree, size_t num_q, size_t q0,
                                size_t sf, size_t qpart, size_t input_level,
                                size_t hw, size_t num_rot, int32_t rot_idxs[],
                                size_t sec_level) {
  if (Param != NULL) {
    free(Param);
  }
  Param = (CKKS_PARAMS*)malloc(sizeof(CKKS_PARAMS) + num_rot * sizeof(int32_t));
  Param->_provider         = LIB_ANT;
  Param->_poly_degree      = degree;
  Param->_sec_level        = sec_level;
  Param->_mul_depth        = num_q - 1;
  Param->_input_level      = input_level;
  Param->_first_mod_size   = q0;
  Param->_scaling_mod_size = sf;
  Param->_num_q_parts      = qpart;
  Param->_hamming_weight   = hw;
  Param->_num_rot_idx      = num_rot;
  memcpy(Param->_rot_idxs, rot_idxs, num_rot * sizeof(int32_t));
  return Param;
}