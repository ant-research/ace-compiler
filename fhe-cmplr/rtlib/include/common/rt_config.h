//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_CONFIG_H
#define RTLIB_INCLUDE_CONFIG_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RTLIB_CONFIG_ALL()                  \
  DECL_CONF(CONF_OP_FUSION_DECOMP_MODUP, 1) \
  DECL_CONF(CONF_BTS_CLEAR_IMAG, 0)

typedef enum {
#define DECL_CONF(ID, VALUE) ID,
  RTLIB_CONFIG_ALL()
#undef DECL_CONF
  // last ID
  CONF_LAST
} RTLIB_CONFIG_ID;

//! @brief Initialize rtlib configuration
void Init_rtlib_config();

//! @brief Get rtlib config for id
//! @param id RTLIB_CONFIG_ID
//! @return config value
int64_t Get_rtlib_config(RTLIB_CONFIG_ID id);

//! @brief Set rtlib config id with value
//! @param id RTLIB_CONFIG_ID
//! @param value value to set
void Set_rtlib_config(RTLIB_CONFIG_ID id, int64_t value);

#ifdef __cplusplus
}
#endif

#endif