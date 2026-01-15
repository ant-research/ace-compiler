//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_TRANSFORM_UTIL_H
#define AIR_BASE_TRANSFORM_UTIL_H

#include <array>

#include "air/base/container.h"
#include "air/base/meta_info.h"
#include "air/base/st.h"
#include "air/core/opcode.h"

namespace air {
namespace base {

//! @brief Enum for number of children
enum NUM_CHILD : uint32_t {
  ZERO,   //!< Node has zero child
  ONE,    //!< Node has one child
  TWO,    //!< Node has two children
  THREE,  //!< Node has three children
};

//! @brief Utility for transform
class TRANSFORM_UTIL {
public:
  //! @brief check if node can be cloned for reuse without side-effect
  bool Is_leaf(NODE_PTR node) { return node->Num_child() == 0; }

  //! @brief get node data element count
  uint32_t Node_elem_count(CONST_NODE_PTR node) {
    TYPE_PTR type = node->Rtype();
    if (!type->Is_array()) {
      // TODO: try to get a high level type from NODE like this:
      // TYPE_ID vec_ty = node->Attr<TYPE_ID>("vec_type");
      AIR_ASSERT(false);
    }
    AIR_ASSERT(type->Is_array());
    return type->Cast_to_arr()->Elem_count();
  }

  //! @brief Flatten node by storing to PREG and load preg back
  template <typename VISITOR>
  NODE_PTR Flatten_node(VISITOR* visitor, NODE_PTR node) {
    if (Is_leaf(node) || node->Is_block()) {
      return node;
    }
    AIR_ASSERT(!node->Is_root());
    CONTAINER* cntr = visitor->Context().Container();
    const SPOS spos = node->Spos();
    PREG_PTR   preg = cntr->Parent_func_scope()->New_preg(node->Rtype());
    STMT_PTR   stp  = cntr->New_stp(node, preg, spos);
    visitor->Context().Prepend(stp);
    NODE_PTR ldp = cntr->New_ldp(preg, spos);
    // copy attribute from node to stp and ldp
    if (air::base::META_INFO::Has_prop<air::base::OPR_PROP::ATTR>(
            node->Opcode())) {
      stp->Node()->Copy_attr(node);
      ldp->Copy_attr(node);
    }
    return ldp;
  }
};

//! @brief Utiltiy for VALIDATE
template <NUM_CHILD NC>
class VALIDATE_UTIL : TRANSFORM_UTIL {
public:
  VALIDATE_UTIL(CONTAINER* cont, bool enable) : _cont(cont), _enable(enable) {}

  template <typename RETV, typename VISITOR>
  void Initialize(VISITOR* visitor, NODE_PTR node, OPCODE v_op) {
    AIR_ASSERT(node->Num_child() == NC);
    if (_enable) {
      // create validate node
      _v_node = _cont->New_validate_node(node, v_op);
    }
    // process children
    for (int i = 0; i < NC; ++i) {
      RETV retv = visitor->template Visit<RETV>(node->Child(i));
      AIR_ASSERT(retv.Num_node() == 1 && retv.Node() != Null_ptr);
      if (!_enable) {
        _child[i] = retv.Node();
      } else {
        _child[i] = Flatten_node(visitor, retv.Node());
        _v_node->Set_child(i, _cont->Clone_node(_child[i]));
      }
    }
  }

  NODE_PTR Child(uint32_t idx) {
    AIR_ASSERT(idx < NC && _child[idx] != Null_ptr);
    return _child[idx];
  }

  template <typename VISITOR>
  NODE_PTR Finalize(VISITOR* visitor, NODE_PTR node, int32_t eps) {
    if (!_enable) {
      return node;
    }

    const SPOS spos    = node->Spos();
    NODE_PTR   v_value = Null_ptr;
    NODE_PTR   parent  = visitor->Parent(1);
    // if not store to variable, flatten it
    if (!parent->Is_st() || !parent->Has_sym()) {
      PREG_PTR preg = _cont->Parent_func_scope()->New_preg(node->Rtype());
      STMT_PTR stp  = _cont->New_stp(node, preg, spos);
      visitor->Context().Prepend(stp);
      v_value = _cont->New_ldp(preg, spos);
      node    = _cont->New_ldp(preg, spos);
    } else {
      AIR_ASSERT(parent->Opcode() == air::core::OPC_ST);
      // TODO: handle STF, STP
      v_value = _cont->New_ld(parent->Addr_datum(), spos);
    }

    uint32_t len    = Node_elem_count(_v_node);
    NODE_PTR l_node = _cont->New_intconst(PRIMITIVE_TYPE::INT_U64, len, spos);
    NODE_PTR e_node = _cont->New_intconst(PRIMITIVE_TYPE::INT_S32, eps, spos);
    STMT_PTR v_stmt =
        _cont->New_validate_stmt(v_value, _v_node, l_node, e_node, spos);
    visitor->Context().Append(v_stmt);
    return node;
  }

private:
  CONTAINER*               _cont;
  NODE_PTR                 _v_node;
  std::array<NODE_PTR, NC> _child;
  bool                     _enable;
};

}  // namespace base

}  // namespace air

#endif  // AIR_BASE_TRANSFORM_UTIL_H
