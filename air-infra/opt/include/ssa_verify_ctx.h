//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_SSA_SIMPLE_VERIFIER_H
#define AIR_OPT_SSA_SIMPLE_VERIFIER_H

#include <unordered_map>
#include <unordered_set>

#include "air/base/analyze_ctx.h"
#include "air/opt/ssa_container.h"
#include "air/opt/ssa_node_list.h"
#include "ssa_rename_stack.h"

namespace air {

namespace opt {

//! @brief Context for SIMPLE SSA VERIFIER
//! No alias analysis, no irregular control flow.
class SIMPLE_VERIFIER_CTX : public air::base::ANALYZE_CTX {
public:
  SIMPLE_VERIFIER_CTX(const SSA_CONTAINER* cont)
      : _ssa_cont(cont), _stack(cont) {}

  template <typename RETV, typename VISITOR>
  RETV Handle_unknown_domain(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(!node->Is_block());
    return Handle_node<RETV>(visitor, node);
  }

public:
  //! @brief Initialize renaming stack
  void Initialize(air::base::NODE_ID node) {
    // VERIFY - SSA container STATE
    AIR_ASSERT(_ssa_cont->State() == SSA_CONTAINER::SSA);

    // VERIFY - SSA Symbol Table
    Verify_symtab();

    // Initialize rename stack
    _stack.Initialize(node);
  }

  //! @brief Finalize renaming stack to make sure all versions are pop'ed
  void Finalize(air::base::NODE_ID node) { _stack.Finalize(node); }

  //! @brief Push a block mark to renaming stack
  void Push_mark(air::base::NODE_PTR node) { _stack.Push_mark(node); }

  //! @brief Pop block mark from renaming stack
  void Pop_mark(air::base::NODE_PTR node) { _stack.Pop_mark(node); }

public:
  //! @brief Verify new version defined by node
  void Verify_stmt_def(SSA_SYM_ID sym, air::base::NODE_ID node) {
    AIR_ASSERT(sym.Value() < _stack.Size());
    SSA_VER_PTR ver = _ssa_cont->Node_ver(node);
    // VERIFY - symbol on node matches symbol in version
    AIR_ASSERT(ver->Sym_id() == sym);
    // VERIFY - version number is set
    AIR_ASSERT(ver->Version() != SSA_VER::NO_VER);
    // VERIFY - version's def_stmt matches with stmt
    AIR_ASSERT(ver->Kind() == VER_DEF_KIND::STMT &&
               ver->Def_stmt_id().Value() == node.Value());
    _stack.Push_ver(ver);
  }

  //! @brief Verify existing version used by node
  void Verify_node_use(SSA_SYM_ID sym, air::base::NODE_ID node) {
    AIR_ASSERT(sym.Value() < _stack.Size());
    SSA_VER_PTR ver = _ssa_cont->Node_ver(node);
    // VERIFY - symbol on node matches symbol in version
    AIR_ASSERT(ver->Sym_id() == sym);
    // VERIFY - version number is set
    // Disable this check because ZERO may be used directly without entry chi
    // AIR_ASSERT(ver->Version() != SSA_VER::NO_VER);
  }

  //! @brief Push new version to stack and annotate to node
  void Handle_def(air::base::NODE_ID node) {
    SSA_SYM_ID sym = _ssa_cont->Node_sym_id(node);
    Verify_stmt_def(sym, node);
  }

  //! @brief Get top of renaming stack and annotate to node
  void Handle_use(air::base::NODE_ID node) {
    SSA_SYM_ID sym = _ssa_cont->Node_sym_id(node);
    Verify_node_use(sym, node);
  }

  void Handle_mu(base::NODE_ID node) {
    MU_NODE_PTR mu     = _ssa_cont->Mu_node(_ssa_cont->Node_mu(node));
    SSA_SYM_ID  sym_id = mu->Sym_id();
    // 1. VERIFY - symbol on mu is not out of range
    AIR_ASSERT(sym_id.Value() < _stack.Size());
    SSA_VER_ID mu_ver_id  = mu->Opnd_id();
    SSA_SYM_ID ver_sym_id = _ssa_cont->Ver(mu_ver_id)->Sym_id();
    // 2. VERIFY - symbol on node matches symbol in version
    AIR_ASSERT(sym_id == ver_sym_id);
    SSA_VER_ID ver_id = _stack.Top_ver_id(sym_id);
    // 3. VERIFY - version of mu matchs with rename top
    AIR_ASSERT(ver_id != base::Null_id);
    AIR_ASSERT(ver_id == mu_ver_id);
  }

public:
  //! @brief Rename PHI_NODE list
  void Rename_phi_list(air::base::NODE_PTR node) {
    AIR_ASSERT(SSA_CONTAINER::Has_phi(node));

    auto rename = [](PHI_NODE_PTR phi, const SSA_CONTAINER* cont,
                     VERSION_STACK& stk) {
      SSA_SYM_ID sym = phi->Sym_id();
      AIR_ASSERT(sym.Value() < stk.Size());
      AIR_ASSERT(phi->Result_id() != air::base::Null_id);
      SSA_VER_PTR ver = cont->Ver(phi->Result_id());
      // VERIFY - symbol on node matches symbol in version
      AIR_ASSERT(ver->Sym_id() == sym);
      // VERIFY - version number is set
      AIR_ASSERT(ver->Version() != SSA_VER::NO_VER);
      // VERIFY - version's def_phi matches with phi
      AIR_ASSERT(ver->Kind() == VER_DEF_KIND::PHI &&
                 ver->Def_phi_id() == phi->Id());
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

    auto rename = [](PHI_NODE_PTR phi, const SSA_CONTAINER* cont,
                     VERSION_STACK& stk, uint32_t opnd) {
      AIR_ASSERT(phi->Size() > opnd);
      AIR_ASSERT(phi->Opnd_id(opnd) != air::base::Null_id);
      SSA_SYM_ID sym = phi->Sym_id();
      AIR_ASSERT(sym.Value() < stk.Size());
      SSA_VER_PTR ver = cont->Ver(phi->Opnd_id(opnd));
      // VERIFY - symbol on node matches symbol in version
      AIR_ASSERT(ver->Sym_id() == sym);
      // VERIFY - version number is set
      // Disable this check because ZERO may be used directly without entry chi
      // AIR_ASSERT(ver->Version() != SSA_VER::NO_VER);
      // VERIFY - version matches with rename top
      AIR_ASSERT(ver->Id() == stk.Top_ver_id(sym));
    };

    PHI_NODE_ID id = _ssa_cont->Node_phi(node->Id());
    PHI_LIST    list(_ssa_cont, id);
    list.For_each(rename, _ssa_cont, _stack, opnd);
  }

  //! @brief Verify CHI_LIST
  void Rename_chi_list(air::base::NODE_PTR node) {
    AIR_ASSERT(SSA_CONTAINER::Has_chi(node));
    auto rename = [](CHI_NODE_PTR chi, const SSA_CONTAINER* cont,
                     VERSION_STACK& stk, base::STMT_ID stmt) {
      SSA_SYM_ID sym = chi->Sym_id();
      AIR_ASSERT(sym.Value() < stk.Size());
      // 1. verify chi opnd
      AIR_ASSERT(chi->Opnd_id() != air::base::Null_id);
      SSA_VER_PTR ver = cont->Ver(chi->Opnd_id());
      // VERIFY - symbol on node matches symbol in version
      AIR_ASSERT(ver->Sym_id() == sym);
      // VERIFY - version matches with rename top
      AIR_ASSERT(ver->Id() == stk.Top_ver_id(sym));

      // 2. verify chi result
      AIR_ASSERT(chi->Result_id() != air::base::Null_id);
      SSA_VER_PTR res_ver = cont->Ver(chi->Result_id());
      // VERIFY - symbol on node matches symbol in version
      AIR_ASSERT(res_ver->Sym_id() == sym);
      // VERIFY - version number is set
      AIR_ASSERT(res_ver->Version() != SSA_VER::NO_VER);
      // VERIFY - version's def_phi matches with phi
      AIR_ASSERT(res_ver->Kind() == VER_DEF_KIND::CHI &&
                 res_ver->Def_chi_id() == chi->Id());
      stk.Push_ver(res_ver);

      // 3. TODO: check define stmt of chi
    };

    CHI_NODE_ID id = _ssa_cont->Node_chi(node->Id());
    CHI_LIST    list(_ssa_cont, id);
    list.For_each(rename, _ssa_cont, _stack, node->Stmt()->Id());
  }

private:
  void Verify_symtab() const {
    // TODO: verify symbols with same AIR ADDR_DATUM/PREG are on the same chain
  }

private:
  const SSA_CONTAINER* _ssa_cont;
  VERSION_STACK        _stack;  // SSA rename stack
};

}  // namespace opt

}  // namespace air

#endif  // AIR_OPT_SSA_SIMPLE_VERIFIER_H
