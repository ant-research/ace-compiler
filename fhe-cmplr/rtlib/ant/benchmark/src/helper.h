//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_ANT_BENCHMARK_SRC_HELPER_H
#define RTLIB_ANT_BENCHMARK_SRC_HELPER_H

#include <string.h>

#include <iomanip>
#include <iostream>

#include "common/common.h"
#include "common/error.h"

#define RTLIB_BM_ENV()                       \
  DECL_ENV(TEST_DEGREE, "TEST_DEGREE", 4096) \
  DECL_ENV(NUM_Q, "NUM_Q", 30)               \
  DECL_ENV(NUM_Q_PART, "NUM_Q_PART", 0)      \
  DECL_ENV(Q0_BITS, "Q0_BITS", 60)           \
  DECL_ENV(SF_BITS, "SF_BITS", 59)           \
  DECL_ENV(HW, "HW", 0)

typedef enum {
#define DECL_ENV(ID, NAME, DEFVAL) ID,
  RTLIB_BM_ENV()
#undef DECL_ENV
  // last id
  ENV_LAST
} RTLIB_BM_ENV_ID;

class ENV_INFO {
public:
  const char* _env_name;
  size_t      _env_value;
};

//! @brief Global environment for benchmark
class BM_GLOB_ENV {
public:
  static void Initialize() {
#define DECL_ENV(ID, NAME, VALUE) \
  Env_info[ID]._env_name  = NAME; \
  Env_info[ID]._env_value = VALUE;
    RTLIB_BM_ENV()
#undef DECL_ENV
  }

  static size_t Get_env(RTLIB_BM_ENV_ID id) {
    char*  env_str   = getenv(Env_info[id]._env_name);
    size_t env_value = Env_info[id]._env_value;
    if (env_str != nullptr && env_str[0] != '\0') {
      env_value = strtoul(env_str, NULL, 10);
    }
    return env_value;
  }

  static void Print_env() {
    std::cout << "Benchmark parameter: " << std::endl;
    for (int i = 0; i < ENV_LAST; i++) {
      size_t value = Get_env(static_cast<RTLIB_BM_ENV_ID>(i));
      std::cout << std::setw(13) << Env_info[i]._env_name << " = " << value
                << std::endl;
    }
  }

  static ENV_INFO Env_info[ENV_LAST];
};

static CKKS_PARAMS* Ckks_param = NULL;
extern "C" {

CKKS_PARAMS* Get_context_params() {
  IS_TRUE(Ckks_param != NULL, "need Set_context_params first !");
  return Ckks_param;
}

RT_DATA_INFO* Get_rt_data_info() { return NULL; }
int           Get_input_count() { return 0; }
int           Get_output_count() { return 0; }
DATA_SCHEME*  Get_encode_scheme(int idx) { return NULL; }
DATA_SCHEME*  Get_decode_scheme(int idx) { return NULL; }
}

//! @brief Set parameters for CKKS_PARAMS
static CKKS_PARAMS* Set_context_params(uint32_t degree, size_t num_q, size_t q0,
                                       size_t sf, size_t qpart = 0,
                                       size_t input_level = 0, size_t hw = 0,
                                       size_t  num_rot    = 0,
                                       int32_t rot_idxs[] = {},
                                       size_t  sec_level  = 0) {
  if (Ckks_param != NULL) {
    free(Ckks_param);
  }
  Ckks_param =
      (CKKS_PARAMS*)malloc(sizeof(CKKS_PARAMS) + num_rot * sizeof(int32_t));
  Ckks_param->_provider         = LIB_ANT;
  Ckks_param->_poly_degree      = degree;
  Ckks_param->_sec_level        = sec_level;
  Ckks_param->_mul_depth        = num_q - 1;
  Ckks_param->_input_level      = input_level;
  Ckks_param->_first_mod_size   = q0;
  Ckks_param->_scaling_mod_size = sf;
  Ckks_param->_num_q_parts      = qpart;
  Ckks_param->_hamming_weight   = hw;
  Ckks_param->_num_rot_idx      = num_rot;
  memcpy(Ckks_param->_rot_idxs, rot_idxs, num_rot * sizeof(int32_t));
  return Ckks_param;
}

#endif  // RTLIB_ANT_BENCHMARK_SRC_HELPER_H