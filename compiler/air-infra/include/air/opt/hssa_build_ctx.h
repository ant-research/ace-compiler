
//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_HSSA_BUILD_CTX_H
#define AIR_OPT_HSSA_BUILD_CTX_H

#include "air/opt/cfg.h"
#include "air/opt/hssa_container.h"
#include "air/opt/ssa_container.h"

namespace air {
namespace opt {
class HSSA_BUILDER_CTX : public air::base::ANALYZE_CTX {
public:
  HSSA_BUILDER_CTX()
      : _cfg(nullptr), _hssa_cont(nullptr), _ssa_cont(nullptr), _cprop(false) {}
  void Init(CFG* cfg, HCONTAINER* hssa_cont, SSA_CONTAINER* ssa_cont,
            bool cprop) {
    _cfg       = cfg;
    _hssa_cont = hssa_cont;
    _ssa_cont  = ssa_cont;
    _cprop     = cprop;
    Set_cur_bb(air::base::Null_ptr);
  }

  HCONTAINER&    Hssa_cont() { return *_hssa_cont; }
  SSA_CONTAINER& Ssa_cont() { return *_ssa_cont; }
  CFG&           Cfg() { return *_cfg; }

  template <typename RETV, typename VISITOR>
  RETV Handle_block(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(Cur_bb() != air::base::Null_ptr);
    // if current bb is the end bb of IF or DO_LOOP
    // start a new block for subsequent blocks
    if (Cur_bb()->Kind() == BB_ENTRY || Cur_bb()->Kind() == BB_LOOP_EXIT ||
        Cur_bb()->Kind() == BB_IF_PHI) {
      BB_PTR bb = Cfg().New_bb(BB_DEF, node->Spos());
      Cfg().Append_bb(bb);
      Cfg().Connect_with_succ(Cur_bb(), bb);
      Set_cur_bb(bb);
      if (In_loop()) {
        bb->Set_loop_info(Cur_loop_info());
      }
    }
    for (air::base::STMT_PTR stmt       = node->Begin_stmt();
         stmt != node->End_stmt(); stmt = stmt->Next()) {
      visitor->template Visit<RETV>(stmt->Node());
    }
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_node(VISITOR* visitor, air::base::NODE_PTR node) {
    HCONTAINER* cont = &(Hssa_cont());
    if (node->Is_root()) {
      HSTMT_PTR op_stmt = cont->New_op_stmt(node);
      for (uint32_t i = 0; i < node->Num_child(); ++i) {
        HEXPR_PTR child = visitor->template Visit<RETV>(node->Child(i));
        op_stmt->Cast_to_nary()->Set_kid(i, child->Id());
      }
      Append_stmt(op_stmt);
      return RETV();
    } else {
      OP_DATA* op_expr = OP_DATA::Alloc(node->Num_child());
      new (op_expr) OP_DATA(node);
      for (uint32_t i = 0; i < node->Num_child(); ++i) {
        HEXPR_PTR child = visitor->template Visit<RETV>(node->Child(i));
        op_expr->Set_kid(i, child->Id());
      }
      HEXPR_DATA_PTR op_ptr(op_expr, HEXPR_ID());
      HEXPR_PTR ret = cont->Find_or_new_expr(HEXPR_PTR(HEXPR(cont, op_ptr)));
      free(op_expr);
      return ret;
    }
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_unknown_domain(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_node<RETV, VISITOR>(visitor, node);
  }

  void Build_chi_list(HSTMT_PTR stmt, CHI_NODE_ID chi) {
    auto build_chi = [](CHI_NODE_PTR chi, HCHI_ID& list, HCONTAINER& hssa_cont,
                        HSTMT_PTR stmt) {
      HCHI_PTR  hchi = hssa_cont.New_chi(stmt);
      HEXPR_PTR res  = hssa_cont.Find_or_new_var_expr(chi->Result_id());
      HEXPR_PTR opnd = hssa_cont.Find_or_new_var_expr(chi->Opnd_id());
      hchi->Set_result(res->Id());
      hchi->Set_opnd(opnd->Id());
      hchi->Set_next(list);  // link to head
      res->Cast_to_var_expr()->Set_def_chi(hchi->Id());
      list = hchi->Id();
    };

    if (chi != CHI_NODE_ID()) {
      CHI_LIST list(_ssa_cont, chi);
      HCHI_ID  hlist = HCHI_ID();
      list.For_each(build_chi, hlist, Hssa_cont(), stmt);
      stmt->Set_chi(hlist);
    }
  }

  BB_PTR Cur_bb() { return _cur_bb; }
  void   Set_cur_bb(BB_PTR cur_bb) { _cur_bb = cur_bb; }

  void Append_stmt(HSTMT_PTR stmt) {
    AIR_ASSERT(!Cur_bb()->Is_null());
    Cur_bb()->Append_stmt(stmt);
  }

  bool In_loop() { return !_loop_stack.empty(); }

  void Push_loop_info(LOOP_INFO_PTR& loop_info) { _loop_stack.push(loop_info); }

  void Pop_loop_info(LOOP_INFO_PTR& loop_info) {
    AIR_ASSERT(!_loop_stack.empty());
    AIR_ASSERT(loop_info == _loop_stack.top());
    _loop_stack.pop();
  }

  LOOP_INFO_PTR& Cur_loop_info() {
    AIR_ASSERT(!_loop_stack.empty());
    return _loop_stack.top();
  }

private:
  CFG*                      _cfg;
  HCONTAINER*               _hssa_cont;
  SSA_CONTAINER*            _ssa_cont;
  std::vector<HSTMT_ID>     _hstmt_stack;
  BB_PTR                    _cur_bb;
  std::stack<LOOP_INFO_PTR> _loop_stack;
  bool                      _cprop;
};
}  // namespace opt
}  // namespace air
#endif
