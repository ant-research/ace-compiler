//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "common/rt_config.h"

#include <stdint.h>

#include "common/rt_env.h"

static int64_t Lib_config[CONF_LAST];

void Init_rtlib_config() {
#define DECL_CONF(ID, VALUE) Lib_config[ID] = VALUE;
  RTLIB_CONFIG_ALL()
#undef DECL_CONF

  const char* decomp_modup = getenv(ENV_OP_FUSION_DECOMP_MODUP);
  if (decomp_modup != NULL) {
    if (atoi(decomp_modup) != 0) {
      Lib_config[CONF_OP_FUSION_DECOMP_MODUP] = 1;
    } else {
      Lib_config[CONF_OP_FUSION_DECOMP_MODUP] = 0;
    }
  }

  const char* bts_clear_imag = getenv(ENV_BOOTSTRAP_CLEAR_IMAG);
  if (bts_clear_imag != NULL) {
    if (atoi(bts_clear_imag) != 0) {
      Lib_config[CONF_BTS_CLEAR_IMAG] = 1;
    } else {
      Lib_config[CONF_BTS_CLEAR_IMAG] = 0;
    }
  }
}

int64_t Get_rtlib_config(RTLIB_CONFIG_ID id) { return Lib_config[id]; }

void Set_rtlib_config(RTLIB_CONFIG_ID id, int64_t value) {
  Lib_config[id] = value;
}
