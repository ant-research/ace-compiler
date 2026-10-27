//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_POLY_CONFIG_H
#define FHE_POLY_CONFIG_H

#include "air/driver/common_config.h"
#include "air/driver/driver_ctx.h"
#include "fhe/poly/option_config.h"

namespace fhe {
namespace poly {

enum TRACE_DETAIL : uint64_t {
  IR_LOWER                = 0x1,
  IR_BEFORE_MUP           = 0x2,
  IR_AFTER_MUP            = 0x4,
  MUP_FLOW                = 0x8,
  MUP_STATS               = 0x10,
  IR_BEFORE_MD            = 0x20,
  IR_AFTER_MD             = 0x40,
  MD_FLOW                 = 0x80,
  MD_STATS                = 0x100,
  IR_BEFORE_OP_FUSION     = 0x200,
  IR_AFTER_OP_FUSION      = 0x400,
  OP_FUSION_FLOW          = 0x800,
  OP_FUSION_STATS         = 0x1000,
  IR_BEFORE_SSA           = 0x2000,
  IR_AFTER_SSA_INSERT_PHI = 0x4000,
  IR_AFTER_SSA            = 0x8000,
  ATTR_PROP               = 0x10000,
};

struct POLY_CONFIG : public fhe::poly::POLY_OPTION_CONFIG {
public:
  POLY_CONFIG(void) {}

  void Update_options();
  void Pre_process_options();
};

}  // namespace poly
}  // namespace fhe

#endif  // FHE_POLY_CONFIG_H
