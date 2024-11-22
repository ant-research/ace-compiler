//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_SSA_RENAME_CTX_H
#define AIR_OPT_SSA_RENAME_CTX_H

#include <unordered_map>
#include <unordered_set>

#include "air/base/analyze_ctx.h"
#include "air/opt/ssa_container.h"
#include "air/opt/ssa_node_list.h"
#include "ssa_rename_stack.h"

namespace air {

namespace opt {

//! @brief Context for SIMPLE SSA BUILDER
//! No alias analysis, no irregular control flow.
class RENAME_CTX : public air::base::ANALYZE_CTX {
public:
  RENAME_CTX(SSA_CONTAINER* cont) : _ssa_cont(cont), _stack(cont) {}

  template <typename RETV, typename VISITOR>
  RETV Handle_unknown_domain(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(!node->Is_block());
    return Handle_node<RETV>(visitor, node);
  }

public:
  //! @brief Initialize renaming stack
  void Initialize(air::base::NODE_ID node) { _stack.Initialize(node); }

  //! @brief Finalize renaming stack to make sure all versions are pop'ed
  void Finalize(air::base::NODE_ID node) { _stack.Finalize(node); }

  //! @brief Push a block mark to renaming stack
  void Push_mark(air::base::NODE_PTR node) { _stack.Push_mark(node); }

  //! @brief Pop block mark from renaming stack
  void Pop_mark(air::base::NODE_PTR node) { _stack.Pop_mark(node); }

public:
  //! @brief Create and push new version defined by stmt
  SSA_VER_ID Gen_stmt_def(SSA_SYM_ID sym, air::base::STMT_ID stmt) {
    AIR_ASSERT(sym.Value() < _stack.Size());
    SSA_VER_PTR ver = _ssa_cont->New_ver(VER_DEF_KIND::STMT, sym);
    ver->Set_version(_stack.Next_version(sym));
    ver->Set_def_stmt(stmt);
    _stack.Push_ver(ver);
    return ver->Id();
  }

  //! @brief Push new version to stack and annotate to node
  void Handle_def(air::base::NODE_ID node) {
    SSA_SYM_ID sym = _ssa_cont->Node_sym_id(node);
    SSA_VER_ID ver = Gen_stmt_def(sym, air::base::STMT_ID(node.Value()));
    _ssa_cont->Set_node_ver(node, ver);
  }

  //! @brief Get top of renaming stack and annotate to node
  void Handle_use(air::base::NODE_ID node) {
    SSA_SYM_ID sym = _ssa_cont->Node_sym_id(node);
    _ssa_cont->Set_node_ver(node, _stack.Top_ver_id(sym));
  }

  void Handle_mu(base::NODE_ID node) {
    MU_NODE_PTR mu     = _ssa_cont->Mu_node(_ssa_cont->Node_mu(node));
    SSA_SYM_ID  sym_id = mu->Sym_id();
    AIR_ASSERT(sym_id.Value() < _stack.Size());
    SSA_VER_ID ver_id = _stack.Top_ver_id(sym_id);
    AIR_ASSERT(ver_id != base::Null_id);
    mu->Set_opnd(ver_id);
  }

public:
  //! @brief Rename PHI_NODE list
  void Rename_phi_list(air::base::NODE_PTR node) {
    AIR_ASSERT(SSA_CONTAINER::Has_phi(node));

    auto rename = [](PHI_NODE_PTR phi, SSA_CONTAINER* cont,
                     VERSION_STACK& stk) {
      SSA_SYM_ID sym = phi->Sym_id();
      AIR_ASSERT(sym.Value() < stk.Size());
      SSA_VER_PTR ver = cont->New_ver(VER_DEF_KIND::PHI, sym);
      ver->Set_version(stk.Next_version(sym));
      ver->Set_def_phi(phi->Id());
      phi->Set_result(ver->Id());
      stk.Push_ver(ver);
    };

    PHI_NODE_ID id = _ssa_cont->Node_phi(node->Id());
    PHI_LIST    list(_ssa_cont, id);
    list.For_each(rename, _ssa_cont, _stack);
  }

  //! @brief Rename PHI_NODE operand
  void Rename_phi_opnd(air::base::NODE_PTR node, uint32_t opnd) {
    AIR_ASSERT(SSA_CONTAINER::Has_phi(node));
    AIR_ASSERT(opnd < SSA_CONTAINER::Phi_size(node));

    auto rename = [](PHI_NODE_PTR phi, VERSION_STACK& stk, uint32_t opnd) {
      AIR_ASSERT(phi->Size() > opnd);
      AIR_ASSERT(phi->Opnd_id(opnd) == air::base::Null_id);
      SSA_SYM_ID sym = phi->Sym_id();
      AIR_ASSERT(sym.Value() < stk.Size());
      SSA_VER_ID ver = stk.Top_ver_id(sym);
      AIR_ASSERT(ver != air::base::Null_id);
      phi->Set_opnd(opnd, ver);
    };

    PHI_NODE_ID id = _ssa_cont->Node_phi(node->Id());
    PHI_LIST    list(_ssa_cont, id);
    list.For_each(rename, _stack, opnd);
  }

  //! @brief Rename CHI_NODE list
  void Rename_chi_list(air::base::NODE_PTR node) {
    AIR_ASSERT(SSA_CONTAINER::Has_chi(node));
    auto rename = [](CHI_NODE_PTR chi, SSA_CONTAINER* cont,
                     VERSION_STACK& stk) {
      SSA_SYM_ID sym = chi->Sym_id();
      AIR_ASSERT(sym.Value() < stk.Size());
      // 1. rename opnd
      SSA_VER_ID opnd_ver = stk.Top_ver_id(sym);
      AIR_ASSERT(opnd_ver != air::base::Null_id);
      chi->Set_opnd(opnd_ver);

      // 2. rename result
      SSA_VER_PTR ver = cont->New_ver(VER_DEF_KIND::CHI, sym);
      ver->Set_version(stk.Next_version(sym));
      ver->Set_def_chi(chi->Id());
      chi->Set_result(ver->Id());
      stk.Push_ver(ver);
    };

    CHI_NODE_ID chi_id = _ssa_cont->Node_chi(node->Id());
    CHI_LIST    list(_ssa_cont, chi_id);
    list.For_each(rename, _ssa_cont, _stack);
  }

private:
  SSA_CONTAINER* _ssa_cont;
  VERSION_STACK  _stack;
};

}  // namespace opt

}  // namespace air

#endif  // AIR_OPT_SSA_RENAME_CTX_H
