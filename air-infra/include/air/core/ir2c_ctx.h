//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_CORE_IR2C_CTX_H
#define AIR_CORE_IR2C_CTX_H

#include "air/base/ir2c_ctx.h"
#include "air/core/opcode.h"

namespace air {

namespace core {

//! @brief Context for CORE IR2C
class IR2C_CTX : public air::base::IR2C_CTX {
public:
  IR2C_CTX(std::ostream& os) : air::base::IR2C_CTX(os) {}

  //! @brief Emit symbol name or expr for store lhs
  template <typename RETV, typename VISITOR>
  void Emit_st_var(VISITOR* visitor, air::base::NODE_PTR node) {
    if (node->Opcode() == OPC_IST) {
      visitor->template Visit<RETV>(node->Child(0));
    } else {
      Emit_st_var(node);
    }
  }

  //! @brief Emit symbol for store lhs
  void Emit_st_var(air::base::NODE_PTR node) {
    if (node->Opcode() == OPC_ST) {
      Emit_var(node);
    } else if (node->Opcode() == OPC_STF) {
      Emit_var(node);
      _os << ".";
      Emit_field(node);
    } else if (node->Opcode() == OPC_STP) {
      Emit_preg_id(node->Preg_id());
    } else if (node->Opcode() == OPC_STPF) {
      Emit_preg_id(node->Preg_id());
      _os << ".";
      Emit_field(node);
    } else if (node->Opcode() == OPC_STO) {
      AIR_ASSERT(false);
    } else {
      AIR_ASSERT(false);
    }
  }

};  // IR2C_CTX

}  // namespace core

}  // namespace air

#endif  // AIR_CORE_IR2C_CTX_H
