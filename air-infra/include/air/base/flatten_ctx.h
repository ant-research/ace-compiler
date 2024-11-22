//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_FLATTEN_CTX_H
#define AIR_BASE_FLATTEN_CTX_H

#include "air/base/transform_ctx.h"
#include "air/base/transform_util.h"

namespace air {

namespace base {

//! @brief Context for flatten pass
class FLATTEN_CTX : public TRANSFORM_CTX {
public:
  //! @brief Construct a new FLATTEN CTX object with container for new NODE
  FLATTEN_CTX(CONTAINER* cntr, std::function<bool(NODE_PTR)>&& func)
      : TRANSFORM_CTX(cntr), _flatten_func(func) {}

  //! @brief Visit kid and flatten child if needed
  template <typename RETV, typename VISITOR>
  RETV Handle_node(VISITOR* visitor, NODE_PTR node) {
    // push current node to ANALYZE_CTX's stack
    ANALYZE_CTX::Push(node);
    NODE_PTR n_node = Null_ptr;
    if (node->Is_root()) {
      STMT_PTR n_stmt = _cntr->Clone_stmt(node->Stmt());
      n_node          = n_stmt->Node();
    } else {
      n_node = _cntr->Clone_node(node);
    }
    // visit children and do flatten if needed
    for (uint32_t i = 0; i < node->Num_child(); ++i) {
      RETV retv = visitor->template Visit<RETV>(node->Child(i));
      AIR_ASSERT(Get_node_ptr(retv) != Null_ptr);
      NODE_PTR child = Get_node_ptr(retv);
      if (!node->Is_root() && node->Num_child() > 0 && _flatten_func(child)) {
        TRANSFORM_UTIL util;
        child = util.Flatten_node(visitor, child);
      }
      n_node->Set_child(i, child);
    }
    // pop current node from ANALYZE_CTX's stack
    ANALYZE_CTX::Pop(node);
    return RETV(n_node);
  }

  //! @brief default action for unknown domain
  template <typename RETV, typename VISITOR>
  RETV Handle_unknown_domain(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(!node->Is_block());
    return Handle_node<RETV>(visitor, node);
  }

private:
  std::function<bool(NODE_PTR)> _flatten_func;

};  // FLATTEN_CTX

}  // namespace base

}  // namespace air

#endif  // AIR_BASE_FLATTEN_CTX_H
