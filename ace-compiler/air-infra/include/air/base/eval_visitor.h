//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_EVAL_VISITOR_H
#define AIR_BASE_EVAL_VISITOR_H

#include "air/base/node.h"
#include "air/core/opcode.h"

namespace air {
namespace base {

//! @brief EVAL_CTX Base context to evaluate a NODE at compile-time
class EVAL_CTX {
public:
  template <typename RETV, typename VISITOR>
  RETV Handle_add(VISITOR* visitor, NODE_PTR node) const {
    return visitor->template Visit<RETV>(node->Child(0)) +
           visitor->template Visit<RETV>(node->Child(1));
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_intconst(VISITOR* visitor, NODE_PTR node) const {
    return node->Intconst();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ild(VISITOR* visitor, NODE_PTR node) const {
    AIR_ASSERT(node->Child(0)->Opcode() == air::core::OPC_ARRAY);
    AIR_ASSERT(node->Child(0)->Child(0)->Opcode() == air::core::OPC_LDCA);
    CONSTANT_PTR cst = node->Child(0)->Child(0)->Const();
    AIR_ASSERT(cst->Kind() == air::base::CONSTANT_KIND::ARRAY);
    AIR_ASSERT(cst->Type()->Is_array());
    ARRAY_TYPE_PTR cst_type = cst->Type()->Cast_to_arr();
    AIR_ASSERT(cst_type->Elem_type()->Is_prim());
    AIR_ASSERT(cst_type->Dim() == node->Child(0)->Num_child() - 1);
    AIR_ASSERT_MSG(cst_type->Dim() == 1, "TODO: support multi-dim array");
    DIM_ITER it       = ++(cst_type->Begin_dim());  // skip first dim
    int      chld_idx = 1;
    int64_t  elem_idx = 0;
    for (; it != cst_type->End_dim(); ++it, ++chld_idx) {
      int64_t tc = ((*it)->Ub_val() - (*it)->Lb_val()) / (*it)->Stride_val();
      elem_idx +=
          visitor->template Visit<int64_t>(node->Child(0)->Child(chld_idx)) *
          tc;
    }
    AIR_ASSERT(chld_idx == node->Child(0)->Num_child() - 1);
    elem_idx +=
        visitor->template Visit<int64_t>(node->Child(0)->Child(chld_idx));

    PRIM_TYPE_PTR elem_type = cst_type->Elem_type()->Cast_to_prim();
    switch (elem_type->Encoding()) {
      case PRIMITIVE_TYPE::INT_S32:
        return cst->Array_elem<int32_t>(elem_idx);
      case PRIMITIVE_TYPE::INT_U32:
        return cst->Array_elem<uint32_t>(elem_idx);
      default:
        AIR_ASSERT(false);
    }
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_mul(VISITOR* visitor, NODE_PTR node) const {
    return visitor->template Visit<RETV>(node->Child(0)) *
           visitor->template Visit<RETV>(node->Child(1));
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_shl(VISITOR* visitor, NODE_PTR node) const {
    return visitor->template Visit<RETV>(node->Child(0))
           << visitor->template Visit<int32_t>(node->Child(1));
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_unknown_node(VISITOR* visitor, NODE_PTR node) const {
    AIR_ASSERT(false);
    return RETV();
  }
};

//! @brief EVAL_VISITOR Utility to evaluate a NODE at compile-time
template <typename EVAL_CTX>
class EVAL_VISITOR {
public:
  EVAL_VISITOR(EVAL_CTX& ctx) : _eval_ctx(ctx) {}

  template <typename RETV>
  RETV Visit(NODE_PTR node) const {
    air::base::OPCODE opc = node->Opcode();
    if (opc == air::core::OPC_ADD) {
      return _eval_ctx.template Handle_add<RETV>(this, node);
    } else if (opc == air::core::OPC_INTCONST) {
      return _eval_ctx.template Handle_intconst<RETV>(this, node);
    } else if (opc == air::core::OPC_ILD) {
      return _eval_ctx.template Handle_ild<RETV>(this, node);
    } else if (opc == air::core::OPC_LD) {
      return _eval_ctx.template Handle_ld<RETV>(this, node);
    } else if (opc == air::core::OPC_MUL) {
      return _eval_ctx.template Handle_mul<RETV>(this, node);
    } else if (opc == air::core::OPC_SHL) {
      return _eval_ctx.template Handle_shl<RETV>(this, node);
    } else {
      AIR_ASSERT(false);
      return _eval_ctx.template Handle_unknown_node<RETV>(this, node);
    }
  }

private:
  EVAL_VISITOR(const EVAL_VISITOR&)            = delete;
  EVAL_VISITOR& operator=(const EVAL_VISITOR&) = delete;

  EVAL_CTX& _eval_ctx;
};

}  // namespace base
}  // namespace air

#endif  // AIR_BASE_EVAL_VISITOR_H
