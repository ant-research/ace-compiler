//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_CG_CODE_GEN_CONFIG_H
#define AIR_CG_CODE_GEN_CONFIG_H

#include "air/cg/option_config.h"
namespace air {

namespace cg {

enum TD_KIND : uint32_t {
  TRACE_ISELECT  = 0x0001,
  TRACE_LSRA     = 0x0002,
  TRACE_LI_BUILD = 0x0004,
};

struct CODE_GEN_CONFIG : public AIR_CG_OPTION_CONFIG {
};  // struct AIR2HPU_CONFIG

//! @brief Macro to define API to access vector config
#define DECLARE_AIR2HPU_CONFIG_ACCESS_API(cfg) \
  DECLARE_AIR2HPU_OPTION_CONFIG_ACCESS_API(cfg)

}  // namespace cg
}  // namespace air

#endif  // AIR_CG_CODE_GEN_CONFIG_H