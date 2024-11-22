//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_TRANSFORM_CTX_H
#define AIR_BASE_TRANSFORM_CTX_H

#include <tuple>
#include <vector>

#include "air/base/analyze_ctx.h"

namespace air {
namespace base {

//! @brief Context for transform pass used by VISITOR
class TRANSFORM_CTX : public ANALYZE_CTX {
public:
  //! @brief Construct a new TRANSFORM_CTX object with container for new NODE
  //! @param cntr Container for new IR tree
  TRANSFORM_CTX(CONTAINER* cntr) : _cntr(cntr) {}

  //! @brief Destruct TRANSFORM_CTX object
  ~TRANSFORM_CTX() { AIR_ASSERT(_n_blk.empty()); }

  //! @brief Get container for new NODE
  CONTAINER* Container() const { return _cntr; }

  //! @brief Get parent function scope for new NODE
  FUNC_SCOPE* Func_scope() const { return _cntr->Parent_func_scope(); }

  //! @brief Get global scope for new NODE
  GLOB_SCOPE* Glob_scope() const { return _cntr->Glob_scope(); }

  //! @brief Prepend stmt in new container before current stmt
  void Prepend(STMT_PTR stmt) {
    AIR_ASSERT(!_n_blk.empty());
    STMT_LIST list(_n_blk.back());
    list.Append(stmt);
  }

  //! @brief Append stmt in new container after current stmt
  void Append(STMT_PTR stmt) { _post_stmt.push_back(stmt); }

  //! @brief Begin transform a BLOCK
  void Push(const NODE_PTR& old_blk, const NODE_PTR& new_blk) {
    ANALYZE_CTX::Push(old_blk);
    _n_blk.push_back(new_blk);
  }

  //! @brief End transform a BLOCK
  void Pop(const NODE_PTR& old_blk, const NODE_PTR& new_blk) {
    ANALYZE_CTX::Pop(old_blk);
    AIR_ASSERT(!_n_blk.empty() && _n_blk.back() == new_blk);
    _n_blk.pop_back();
  }

  //! @brief Begin transform a STMT
  void Begin_stmt(const NODE_PTR& node) {
    AIR_ASSERT(!_n_blk.empty());
    AIR_ASSERT(_post_stmt.empty());
  }

  //! @brief End transform a STMT
  void End_stmt(const NODE_PTR& node) {
    AIR_ASSERT(!_n_blk.empty());
    if (!_post_stmt.empty()) {
      STMT_LIST list(_n_blk.back());
      for (auto&& it = _post_stmt.begin(); it != _post_stmt.end(); ++it) {
        list.Append(*it);
      }
      _post_stmt.clear();
    }
  }

  //! @brief Default BLOCK handler to traverse original stmt list and
  //  construct new stmt list
  template <typename RETV, typename VISITOR>
  RETV Handle_block(VISITOR* visitor, air::base::NODE_PTR node) {
    NODE_PTR block = _cntr->New_stmt_block(node->Spos());
    Push(node, block);
    STMT_LIST list(block);
    for (STMT_PTR stmt = node->Begin_stmt(); stmt != node->End_stmt();
         stmt          = stmt->Next()) {
      NODE_PTR s_node = stmt->Node();
      Begin_stmt(s_node);
      RETV     retv   = visitor->template Visit<RETV>(s_node);
      NODE_PTR n_node = Get_node_ptr(retv);
      if (n_node != Null_ptr) {
        AIR_ASSERT(n_node->Is_root());
        list.Append(n_node->Stmt());
      }
      End_stmt(s_node);
    }
    Pop(node, block);
    return RETV(block);
  }

  //! @brief Default NODE handler to clone the node
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
    // visit children
    for (uint32_t i = 0; i < node->Num_child(); ++i) {
      RETV     retv    = visitor->template Visit<RETV>(node->Child(i));
      NODE_PTR n_child = Get_node_ptr(retv);
      AIR_ASSERT(n_child != Null_ptr);
      n_node->Set_child(i, n_child);

      // add parent node for nested blocks
      if (n_node->Is_root() && n_child->Is_root()) {
        n_child->Stmt()->Set_parent_node(n_node);
      }
    }
    // pop current node from ANALYZE_CTX's stack
    ANALYZE_CTX::Pop(node);
    return RETV(n_node);
  }

protected:
  // get NODE_PTR from RETV
  template <typename RETV>
  NODE_PTR Get_node_ptr(const RETV& retv) {
    return retv.Node();
  }

  // get NODE_PTR from NODE_PTR
  NODE_PTR Get_node_ptr(NODE_PTR node) { return node; }

  // new container
  CONTAINER* _cntr;

  // new node stack
  NODE_STACK _n_blk;

  // new stmts to be appended after current stmt
  std::vector<STMT_PTR> _post_stmt;

};  // TRANSFORM_CTX

}  // namespace base
}  // namespace air

#endif  // AIR_BASE_TRANSFORM_CTX_H
