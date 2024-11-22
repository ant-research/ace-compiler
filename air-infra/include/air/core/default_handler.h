//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_CORE_DEFAULT_HANDLER_H
#define AIR_CORE_DEFAULT_HANDLER_H

#include "air/base/container.h"
#include "air/base/opcode_gen.h"
#include "air/core/opcode.h"

namespace air {
namespace core {

//! @brief Default handler which always call visitor Context's Handle_node
//! and Handle_block to handle nodes
class DEFAULT_HANDLER {
public:
  // special implementation for BLOCK to call Context()->Handle_block()
  template <typename RETV, typename VISITOR>
  RETV Handle_block(VISITOR* visitor, air::base::NODE_PTR node) {
    return visitor->Context().template Handle_block<RETV, VISITOR>(visitor,
                                                                   node);
  }

  // define DEF_OP_BLOCK so that Handle_block() won't be expanded below
#define DEF_OP_BLOCK(NAME, name, category, kid_num, fld_num, property)

  // null handler implementation for each OPCODE
#define DEF_OPCODE(NAME, name, cat, kid_num, fld_num, prop) \
  OPCODE_DEFAULT_HANDLER_GEN(NAME, name, cat, kid_num, fld_num, prop)
#include "opcode_def.inc"
#undef DEF_OPCODE
#undef DEF_OP_BLOCK
};

}  // namespace core
}  // namespace air

#endif  // AIR_CORE_DEFAULT_HANDLER_H
