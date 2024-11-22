//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_DEFAULT_HANDLER_H
#define FHE_CKKS_DEFAULT_HANDLER_H

#include "air/base/container.h"
#include "air/base/opcode_gen.h"
#include "fhe/ckks/opcode_def.inc"

namespace fhe {
namespace ckks {

//! @brief Default handler which always call visitor Context's Handle_node
//! and Handle_block to handle nodes
class DEFAULT_HANDLER {
public:
  // null handler implementation for each OPCODE
#define CKKS_OPCODE(NAME, name, cat, kid_num, fld_num, prop) \
  OPCODE_DEFAULT_HANDLER_GEN(NAME, name, cat, kid_num, fld_num, prop)
#include "opcode_def.inc"
#undef CKKS_OPCODE
};

}  // namespace ckks
}  // namespace fhe

#endif  // FHE_CKKS_DEFAULT_HANDLER_H
