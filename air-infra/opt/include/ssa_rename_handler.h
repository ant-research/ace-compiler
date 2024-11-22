//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_SSA_RENAME_HANDLER_H
#define AIR_OPT_SSA_RENAME_HANDLER_H

#include "air/core/default_handler.h"

namespace air {

namespace opt {

//! @brief Handler for SSA renaming
class RENAME_HANDLER : public air::core::DEFAULT_HANDLER {
public:
  template <typename RETV, typename VISITOR>
  RETV Handle_ld(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->Context().Handle_use(node->Id());
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ild(VISITOR* visitor, air::base::NODE_PTR node) {
    // 1. handle address node
    base::NODE_PTR addr_child = node->Child(0);
    AIR_ASSERT(addr_child->Opcode() == air::core::OPC_ARRAY);
    air::base::NODE_PTR base = addr_child->Child(0);
    if (base->Opcode() == air::core::OPC_LDCA) {
      return RETV();
    }
    AIR_ASSERT(base->Opcode() == air::core::OPC_LDA);
    visitor->template Visit<RETV>(addr_child);

    // 2. handle may used virtual variable
    visitor->Context().Handle_mu(node->Id());
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ldf(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(false);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ldo(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(false);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ldp(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->Context().Handle_use(node->Id());
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ldpf(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(false);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_idname(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->Context().Handle_use(node->Id());
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_st(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->template Visit<RETV>(node->Child(0));
    visitor->Context().Handle_def(node->Id());
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ist(VISITOR* visitor, air::base::NODE_PTR node) {
    // 1. handle rhs node
    base::NODE_PTR rhs_child = node->Child(1);
    visitor->template Visit<RETV>(rhs_child);

    // 2. handle address node
    base::NODE_PTR addr_child = node->Child(0);
    AIR_ASSERT(addr_child->Opcode() == air::core::OPC_ARRAY);
    air::base::NODE_PTR base = addr_child->Child(0);
    AIR_ASSERT(base->Opcode() == air::core::OPC_LDA);
    visitor->template Visit<RETV>(addr_child);

    // 3. handle defined virtual variables
    visitor->Context().Rename_chi_list(node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_stf(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(false);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_sto(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(false);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_stp(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->template Visit<RETV>(node->Child(0));
    visitor->Context().Handle_def(node->Id());
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_stpf(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(false);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_call(VISITOR* visitor, air::base::NODE_PTR node) {
    for (uint32_t i = 0; i < node->Num_child(); ++i) {
      visitor->template Visit<RETV>(node->Child(i));
    }
    air::base::PREG_ID preg = node->Ret_preg_id();
    if (!air::base::Is_null_id(preg)) {
      visitor->Context().Handle_def(node->Id());
    }
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_if(VISITOR* visitor, air::base::NODE_PTR node) {
    // rename if-cond
    visitor->template Visit<RETV>(node->Child(0));
    // handle then block
    visitor->Context().Push_mark(node);
    visitor->template Visit<RETV>(node->Child(1));
    visitor->Context().Rename_phi_opnd(node, 0);
    visitor->Context().Pop_mark(node);
    // handle else block
    visitor->Context().Push_mark(node);
    visitor->template Visit<RETV>(node->Child(2));
    visitor->Context().Rename_phi_opnd(node, 1);
    visitor->Context().Pop_mark(node);
    // rename phi result
    visitor->Context().Rename_phi_list(node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_do_loop(VISITOR* visitor, air::base::NODE_PTR node) {
    // rename DO_LOOP IV-init
    air::base::NODE_PTR init = node->Child(0);
    visitor->template Visit<RETV>(init);
    visitor->Context().Handle_def(init->Id());
    // rename DO_LOOP phi operand 0
    visitor->Context().Rename_phi_opnd(node, PREHEADER_PHI_OPND_ID);
    // rename DO_LOOP phi result
    visitor->Context().Rename_phi_list(node);
    // rename DO_LOOP IV-compare
    visitor->template Visit<RETV>(node->Child(1));
    // rename DO_LOOP body
    visitor->Context().Push_mark(node);
    visitor->template Visit<RETV>(node->Child(3));
    // rename DO_LOOP IV-update
    air::base::NODE_PTR step = node->Child(2);
    visitor->template Visit<RETV>(step);
    visitor->Context().Handle_def(step->Id());
    // rename DO_LOOP phi operand 1
    visitor->Context().Rename_phi_opnd(node, BACK_EDGE_PHI_OPND_ID);
    // done. restore block mark
    visitor->Context().Pop_mark(node);
  }
};

}  // namespace opt

}  // namespace air

#endif  // AIR_OPT_SSA_RENAME_HANDLER_H
