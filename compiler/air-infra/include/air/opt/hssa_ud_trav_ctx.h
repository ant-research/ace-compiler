//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_HSSA_UD_TRAV_CTX_H
#define AIR_OPT_HSSA_UD_TRAV_CTX_H

#include <vector>

#include "air/opt/cfg.h"
#include "air/opt/hssa_container.h"
#include "air/opt/hssa_decl.h"
#include "air/opt/hssa_stmt.h"
#include "air/opt/hssa_visitor.h"
#include "air/util/debug.h"

namespace air {
namespace opt {

//! @brief context for analyze pass used by VISITOR
class HSSA_UD_TRAV_CTX {
public:
  //! @brief construct a new analyze context
  HSSA_UD_TRAV_CTX(CFG& cfg)
      : _cfg(cfg), _parent(air::base::Null_ptr), _is_stop(false) {}

  //! @brief destruct TRANSFORM object
  ~HSSA_UD_TRAV_CTX() { AIR_ASSERT(_stack.empty()); }

  CFG&        Cfg() const { return _cfg; }
  HCONTAINER& Hssa_cont() const { return *(_cfg.Hssa_cont()); }

  HEXPR_PTR Parent(size_t nth) const {
    AIR_ASSERT(!_stack.empty());
    size_t sz = _stack.size();
    return sz > nth ? _stack[sz - 1 - nth] : air::base::Null_ptr;
  }

  //! @brief get parent stmt
  HSTMT_PTR Parent_stmt() const { return _parent; }

  void Set_parent_stmt(HSTMT_PTR stmt) { _parent = stmt; }

  //! @brief manually push a node to visiting stack
  //! @param node node to be pushed onto stack
  void Push(const HEXPR_PTR& expr) { _stack.push_back(expr); }

  //! @brief manually pop a node from visiting stack
  //! @param node node to be poped from stack
  void Pop(const HEXPR_PTR& expr) {
    AIR_ASSERT(!_stack.empty() && _stack.back() == expr);
    _stack.pop_back();
  }

  //! @brief check if node stack is empty
  bool Empty() const { return _stack.empty(); }

  bool Is_stop() const { return _is_stop; }
  void Set_stop(bool v) { _is_stop = v; }

  template <typename RETV, typename VISITOR>
  RETV Handle_expr(VISITOR* visitor, HEXPR_PTR node) {
    if (Is_stop()) return RETV();
    switch (node->Kind()) {
      case EK_VAR:
        return Handle_var<RETV>(visitor, node);
      case EK_OP:
        return Handle_op<RETV>(visitor, node);
      default:
        CMPLR_ASSERT(false, "node not handled");
    }
    return RETV(node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_var(VISITOR* visitor, HEXPR_PTR node) {
    AIR_ASSERT(node->Kind() == EK_VAR);
    VAR_DATA_PTR var_expr = node->Cast_to_var_expr();
    if (var_expr->Def_by_stmt()) {
      HSTMT_PTR def_stmt = Hssa_cont().Stmt_ptr(var_expr->Def_stmt());
      AIR_ASSERT(def_stmt->Kind() == SK_ASSIGN)
      // std::cout << "Def by stmt" << std::endl;
      // def_stmt->Print(std::cout);
      // std::cout << std::endl;
      return visitor->template Visit<RETV>(def_stmt);
    } else if (var_expr->Def_by_phi()) {
      HPHI_PTR phi = node->Def_phi();
      // std::cout << "Def by phi" << std::endl;
      // phi->Print(std::cout);
      // std::cout << std::endl;
      if (_visited_phi.find(phi->Id()) == _visited_phi.end()) {
        _visited_phi.insert(phi->Id());
        for (uint32_t idx = 0; idx < phi->Size(); idx++) {
          HEXPR_PTR opnd = phi->Opnd(idx);
          // std::cout << "expr" << phi->Result_id().Value() << "--phi-->expr"
          //          << opnd->Id().Value() << std::endl;
          visitor->template Visit<RETV>(opnd);
        }
      } else {
        std::cout << "def phi already visited\n";
      }
    } else if (var_expr->Def_by_chi()) {
      HCHI_PTR chi = Hssa_cont().Chi_ptr(var_expr->Def_chi());
      AIR_ASSERT(chi != Null_ptr);
      HSTMT_PTR stmt = Hssa_cont().Stmt_ptr(chi->Stmt());
      // std::cout << "Def by chi" << std::endl;
      // stmt->Print(std::cout);
      // std::cout << std::endl;
      return visitor->template Visit<RETV>(stmt);
    } else {
      // AIR_ASSERT_MSG(false, "unknown var def by");
    }
    return RETV(node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_op(VISITOR* visitor, HEXPR_PTR node) {
    AIR_ASSERT(node->Kind() == EK_OP);
    OP_DATA_PTR op_expr = node->Cast_to_op_expr();
    for (size_t idx = 0; idx < op_expr->Kid_cnt(); idx++) {
      visitor->template Visit<RETV>(Hssa_cont().Expr_ptr(op_expr->Kid(idx)));
    }
    return RETV(node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_stmt(VISITOR* visitor, HSTMT_PTR node) {
    if (Is_stop()) return RETV();
    switch (node->Kind()) {
      case SK_ASSIGN: {
        ASSIGN_DATA_PTR ass_stmt = node->Cast_to_assign();
        return visitor->template Visit<RETV>(
            Hssa_cont().Expr_ptr(ass_stmt->Rhs()));
      }
      case SK_NARY: {
        NARY_DATA_PTR op_stmt = node->Cast_to_nary();
        uint32_t      kid_cnt = op_stmt->Kid_cnt();
        // special handle for func entry
        if (node->Opcode() == air::core::FUNC_ENTRY) {
          kid_cnt--;
        }
        for (size_t idx = 0; idx < kid_cnt; idx++) {
          visitor->template Visit<RETV>(
              Hssa_cont().Expr_ptr(op_stmt->Kid(idx)));
        }
      }
      case SK_CALL: {
        CALL_DATA_PTR call_stmt = node->Cast_to_call();
        uint32_t      kid_cnt   = call_stmt->Arg_cnt();
        for (size_t idx = 0; idx < kid_cnt; idx++) {
          visitor->template Visit<RETV>(
              Hssa_cont().Expr_ptr(call_stmt->Arg(idx)));
        }
      }
      default:
        AIR_ASSERT_MSG(false, "not yet implemented");
    }
    return RETV();
  }

  //! @brief unknown DOMAIN handler
  template <typename RETV, typename VISITOR>
  RETV Handle_unknown_domain(VISITOR* visitor, HSTMT_PTR node) {
    if (Is_stop()) return RETV();
    return visitor->Context().template Handle_stmt<RETV, VISITOR>(visitor,
                                                                  node);
    // AIR_ASSERT_MSG(false, "Internal error: unknown domain");
    // return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_unknown_domain(VISITOR* visitor, HEXPR_PTR node) {
    if (Is_stop()) return RETV();
    return visitor->Context().template Handle_expr<RETV, VISITOR>(visitor,
                                                                  node);
    // AIR_ASSERT_MSG(false, "Internal error: unknown domain");
    // return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Pre_handle_stmt(VISITOR* visitor, HSTMT_PTR node) {
    if (Is_stop()) return RETV();
    Set_parent_stmt(node);
    // std::cout << "======================\n";
    // std::cout << "Enter stmt[" << node->Id().Value() << "]" << std::endl;
    // node->Print();
    // std::cout << std::endl;

    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Pre_handle_expr(VISITOR* visitor, HEXPR_PTR node) {
    if (Is_stop()) return RETV();
    // std::cout << "======================\n";
    // std::cout << "Enter expr[" << node->Id().Value() << "]" << std::endl;
    // node->Print();
    // std::cout << std::endl;

    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Post_handle_stmt(VISITOR* visitor, HSTMT_PTR node) {
    if (Is_stop()) return RETV();
    // std::cout << "Exit stmt[" << node->Id().Value() << "]" << std::endl;
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Post_handle_expr(VISITOR* visitor, HEXPR_PTR node) {
    if (Is_stop()) return RETV();
    // std::cout << "Exit expr[" << node->Id().Value() << "]" << std::endl;
    return RETV();
  }

  //! @brief define type for NODE_STACK
  using NODE_STACK = std::vector<HEXPR_PTR>;

  //! @brief guard to push/pop node automatically
  class GUARD {
  public:
    //! @brief constructor, push node onto stack
    GUARD(HSSA_UD_TRAV_CTX& ctx, const HEXPR_PTR& node)
        : _stack(ctx.Node_stack()), _node(node) {
      _stack.push_back(node);
    }

    //! @brief destructor, pop node from stack
    ~GUARD() {
      AIR_ASSERT(!_stack.empty() && _stack.back() == _node);
      _stack.pop_back();
    }

  private:
    NODE_STACK&      _stack;  // node stack
    const HEXPR_PTR& _node;   // current node
  };                          // GUARD

private:
  HSSA_UD_TRAV_CTX(const HSSA_UD_TRAV_CTX&)            = delete;
  HSSA_UD_TRAV_CTX(const HSSA_UD_TRAV_CTX&&)           = delete;
  HSSA_UD_TRAV_CTX& operator=(const HSSA_UD_TRAV_CTX&) = delete;

protected:
  // enable GUARD access _stack
  friend class GUARD;

  // Get stack for all parents of current node
  NODE_STACK&        Node_stack() { return _stack; }
  std::set<HPHI_ID>& Visited_phi() { return _visited_phi; }

  CFG& _cfg;

  // stack for parent node in old container
  NODE_STACK _stack;

  HSTMT_PTR         _parent;
  bool              _is_stop;  // stop traverse
  std::set<HPHI_ID> _visited_phi;

};  // HSSA_UD_TRAV_CTX

}  // namespace opt
}  // namespace air

#endif  // AIR_BASE_ANALYZE_CTX_H
