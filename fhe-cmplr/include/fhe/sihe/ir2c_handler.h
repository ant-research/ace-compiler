//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_SIHE_IR2C_HANDLER_H
#define FHE_SIHE_IR2C_HANDLER_H

#include "air/base/container.h"
#include "air/base/container_decl.h"
#include "fhe/core/ir2c_ctx.h"
#include "fhe/sihe/invalid_handler.h"
#include "fhe/sihe/sihe_opcode.h"

namespace fhe {

namespace sihe {

//! @brief Handler to convert SIHE IR to C
class IR2C_HANDLER : public INVALID_HANDLER {
public:
  //! @brief Special handling for ADD_MSG operator
  template <typename RETV, typename VISITOR>
  void Handle_add_msg(VISITOR* visitor, air::base::NODE_PTR node) {
    core::IR2C_CTX& ctx   = visitor->Context();
    const char*     fname = ctx.Is_plain_type(node->Child(1)->Rtype_id())
                                ? "Add_plain_msg"
                                : "Add_msg";
    ctx << fname << "(";
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(1));
    ctx << ")";
  }

  //! @brief Special handling for MUL_MSG operator
  template <typename RETV, typename VISITOR>
  void Handle_mul_msg(VISITOR* visitor, air::base::NODE_PTR node) {
    core::IR2C_CTX& ctx   = visitor->Context();
    const char*     fname = ctx.Is_plain_type(node->Child(1)->Rtype_id())
                                ? "Mul_plain_msg"
                                : "Mul_msg";
    ctx << fname << "(";
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(1));
    ctx << ")";
  }

  //! @brief Special handling for ROTATE_MSG operator
  template <typename RETV, typename VISITOR>
  void Handle_rotate_msg(VISITOR* visitor, air::base::NODE_PTR node) {
    core::IR2C_CTX& ctx = visitor->Context();
    ctx << "Rotate_msg(";
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(1));
    ctx << ")";
  }

  //! @brief Special handling for RELU_MSG operator
  template <typename RETV, typename VISITOR>
  void Handle_relu_msg(VISITOR* visitor, air::base::NODE_PTR node) {
    uint32_t        elem_count = 0;
    core::IR2C_CTX& ctx        = visitor->Context();
    ctx << "Relu_msg(";
    visitor->template Visit<RETV>(node->Child(0));
    const int64_t* x_shape = node->Attr<int64_t>("x_shape", &elem_count);
    AIR_ASSERT(x_shape != nullptr && (elem_count == 2 || elem_count == 4));
    uint64_t len = 1;
    for (uint32_t i = 0; i < elem_count; ++i) {
      len *= x_shape[i];
    }
    ctx << ", " << len << ")";
  }

  //! @brief Special handling for BOOTSTRAP_MSG operator
  template <typename RETV, typename VISITOR>
  void Handle_bootstrap_msg(VISITOR* visitor, air::base::NODE_PTR node) {
    core::IR2C_CTX& ctx = visitor->Context();
    ctx << "Bootstrap_msg(";
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ")";
  }

};  // IR2C_HANDLER

}  // namespace sihe

}  // namespace fhe

#endif  // FHE_SIHE_IR2C_HANDLER_H
