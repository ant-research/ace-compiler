//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_CORE_INVALID_HANDLER_H
#define AIR_CORE_INVALID_HANDLER_H

#include "air/base/container.h"
#include "air/base/opcode_gen.h"
#include "air/core/opcode.h"
#include "air/util/debug.h"

namespace air {
namespace core {

/**
 * @brief Invalid handler which assert any node by default.
 * New class can inherit from this class and override handler functions for
 * those allowed operators.
 *
 */
class INVALID_HANDLER {
public:
  // special implementation for BLOCK to traverse the stmt list
  template <typename RETV, typename VISITOR>
  RETV Handle_block(VISITOR* visitor, air::base::NODE_PTR node) {
    for (air::base::STMT_PTR stmt       = node->Begin_stmt();
         stmt != node->End_stmt(); stmt = stmt->Next()) {
      visitor->template Visit<RETV>(stmt->Node());
    }
    return RETV();
  }

  // define DEF_OP_BLOCK so that Handle_block() won't be expanded below
#define DEF_OP_BLOCK(NAME, name, category, kid_num, fld_num, property)

  // invalid handler implementation for each OPCODE
#define DEF_OPCODE(NAME, name, cat, kid_num, fld_num, prop) \
  OPCODE_INVALID_HANDLER_GEN(NAME, name, cat, kid_num, fld_num, prop)
#include "opcode_def.inc"
#undef DEF_OPCODE
#undef DEF_OP_BLOCK
};

}  // namespace core
}  // namespace air

#endif  // AIR_CORE_INVALID_HANDLER_H
