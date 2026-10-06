//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_HSSA_DEFAULT_HANDLER_H
#define AIR_OPT_HSSA_DEFAULT_HANDLER_H

#include "air/base/container.h"
#include "air/base/opcode_gen.h"
#include "air/core/opcode.h"

namespace air {
namespace opt {

//! @brief Default handler which always call visitor Context's Handle_node
//! and Handle_block to handle nodes
class HSSA_DEFAULT_HANDLER {
public:
  // define DEF_OP_BLOCK so that Handle_block() won't be expanded below
  // #define DEF_OP_BLOCK(NAME, name, category, kid_num, fld_num, property)

  // null handler implementation for each OPCODE
#define DEF_OPCODE(NAME, name, kid_num, fld_num, prop) \
  OPCODE_DEFAULT_HANDLER_GEN_EXPR(NAME, name, kid_num, fld_num, prop)
#include "air/core/opcode_def.inc"
#undef DEF_OPCODE

#define DEF_OPCODE(NAME, name, kid_num, fld_num, prop) \
  OPCODE_DEFAULT_HANDLER_GEN_STMT(NAME, name, kid_num, fld_num, prop)
#include "air/core/opcode_def.inc"
#undef DEF_OPCODE

#undef DEF_OP_BLOCK
};

}  // namespace opt
}  // namespace air

#endif  // AIR_CORE_DEFAULT_HANDLER_H
