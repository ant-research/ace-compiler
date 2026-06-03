//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_HSSA_CORE_HANDLER_H
#define AIR_OPT_HSSA_CORE_HANDLER_H

#include "air/core/default_handler.h"
#include "air/opt/bb.h"
#include "air/opt/cfg.h"
#include "air/opt/hssa_build_ctx.h"
#include "air/opt/hssa_container.h"
#include "air/opt/hssa_mu_chi.h"
#include "air/opt/ssa_container.h"

namespace air {

namespace opt {
class HSSA_CORE_HANDLER : public air::core::DEFAULT_HANDLER {
public:
  template <typename RETV, typename VISITOR>
  RETV Handle_func_entry(VISITOR* visitor, air::base::NODE_PTR node) {
    HSSA_BUILDER_CTX& ctx       = visitor->Context();
    HCONTAINER&       hssa_cont = ctx.Hssa_cont();
    CFG&              cfg       = ctx.Cfg();
    BB_PTR            entry_bb  = cfg.New_bb(BB_ENTRY, node->Spos());
    ctx.Set_cur_bb(entry_bb);
    HSTMT_PTR entry_stmt = hssa_cont.New_entry_stmt(node);
    for (uint32_t i = 0; i < node->Num_child() - 1; ++i) {
      HEXPR_PTR child = visitor->template Visit<RETV>(node->Child(i));
      entry_stmt->Cast_to_nary()->Set_kid(i, child->Id());
    }
    ctx.Append_stmt(entry_stmt);
    visitor->template Visit<RETV>(node->Child(node->Num_child() - 1));

    BB_PTR exit_bb = cfg.New_bb(BB_EXIT, node->Spos());
    cfg.Connect_with_succ(ctx.Cur_bb(), exit_bb);
    cfg.Append_bb(exit_bb);
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_intconst(VISITOR* visitor, air::base::NODE_PTR node) {
    HCONTAINER&    hssa_cont = visitor->Context().Hssa_cont();
    CST_DATA       cst_expr(node);
    HEXPR_DATA_PTR cst_ptr(&cst_expr, HEXPR_ID());
    HEXPR_PTR      ret =
        hssa_cont.Find_or_new_expr(HEXPR_PTR(HEXPR(&hssa_cont, cst_ptr)));
    return ret;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ldc(VISITOR* visitor, air::base::NODE_PTR node) {
    HCONTAINER&    hssa_cont = visitor->Context().Hssa_cont();
    CST_DATA       cst_expr(node);
    HEXPR_DATA_PTR cst_ptr(&cst_expr, HEXPR_ID());
    HEXPR_PTR      ret =
        hssa_cont.Find_or_new_expr(HEXPR_PTR(HEXPR(&hssa_cont, cst_ptr)));
    return ret;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ldca(VISITOR* visitor, air::base::NODE_PTR node) {
    HCONTAINER&    hssa_cont = visitor->Context().Hssa_cont();
    CST_DATA       cst_expr(node);
    HEXPR_DATA_PTR cst_ptr(&cst_expr, HEXPR_ID());
    HEXPR_PTR      ret =
        hssa_cont.Find_or_new_expr(HEXPR_PTR(HEXPR(&hssa_cont, cst_ptr)));
    return ret;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_st(VISITOR* visitor, air::base::NODE_PTR node) {
    HSSA_BUILDER_CTX& ctx       = visitor->Context();
    HCONTAINER&       hssa_cont = ctx.Hssa_cont();
    SSA_CONTAINER&    ssa_cont  = ctx.Ssa_cont();
    HSTMT_PTR         stmt      = hssa_cont.New_assign_stmt(node);
    HEXPR_PTR         rhs       = visitor->template Visit<RETV>(node->Child(0));
    SSA_VER_ID        lhs_ver_id = ssa_cont.Node_ver_id(node->Id());
    HEXPR_PTR         lhs        = hssa_cont.Find_or_new_var_expr(lhs_ver_id);
    stmt->Set_lhs(lhs->Id());
    stmt->Set_rhs(rhs->Id());
    lhs->Cast_to_var_expr()->Set_def_stmt(stmt->Id());
    ctx.Build_chi_list(stmt, ssa_cont.Node_chi(node->Id()));
    ctx.Append_stmt(stmt);
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_stf(VISITOR* visitor, air::base::NODE_PTR node) {
    HSSA_BUILDER_CTX& ctx       = visitor->Context();
    HCONTAINER&       hssa_cont = ctx.Hssa_cont();
    SSA_CONTAINER&    ssa_cont  = ctx.Ssa_cont();
    HSTMT_PTR         stmt      = hssa_cont.New_assign_stmt(node);
    HEXPR_PTR         rhs       = visitor->template Visit<RETV>(node->Child(0));
    SSA_VER_ID        lhs_ver_id = ssa_cont.Node_ver_id(node->Id());
    HEXPR_PTR         lhs        = hssa_cont.Find_or_new_var_expr(lhs_ver_id);
    stmt->Set_lhs(lhs->Id());
    stmt->Set_rhs(rhs->Id());
    lhs->Cast_to_var_expr()->Set_def_stmt(stmt->Id());
    ctx.Build_chi_list(stmt, ssa_cont.Node_chi(node->Id()));
    ctx.Append_stmt(stmt);
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_stp(VISITOR* visitor, air::base::NODE_PTR node) {
    HSSA_BUILDER_CTX& ctx       = visitor->Context();
    HCONTAINER&       hssa_cont = ctx.Hssa_cont();
    SSA_CONTAINER&    ssa_cont  = ctx.Ssa_cont();
    HSTMT_PTR         stmt      = hssa_cont.New_assign_stmt(node);
    HEXPR_PTR         rhs       = visitor->template Visit<RETV>(node->Child(0));
    SSA_VER_ID        lhs_ver_id = ssa_cont.Node_ver_id(node->Id());
    HEXPR_PTR         lhs        = hssa_cont.Find_or_new_var_expr(lhs_ver_id);
    stmt->Set_lhs(lhs->Id());
    stmt->Set_rhs(rhs->Id());
    lhs->Cast_to_var_expr()->Set_def_stmt(stmt->Id());
    ctx.Build_chi_list(stmt, ssa_cont.Node_chi(node->Id()));
    ctx.Append_stmt(stmt);
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_stpf(VISITOR* visitor, air::base::NODE_PTR node) {
    HSSA_BUILDER_CTX& ctx       = visitor->Context();
    HCONTAINER&       hssa_cont = ctx.Hssa_cont();
    SSA_CONTAINER&    ssa_cont  = ctx.Ssa_cont();
    HSTMT_PTR         stmt      = hssa_cont.New_assign_stmt(node);
    HEXPR_PTR         rhs       = visitor->template Visit<RETV>(node->Child(0));
    SSA_VER_ID        lhs_ver_id = ssa_cont.Node_ver_id(node->Id());
    HEXPR_PTR         lhs        = hssa_cont.Find_or_new_var_expr(lhs_ver_id);
    stmt->Set_lhs(lhs->Id());
    stmt->Set_rhs(rhs->Id());
    lhs->Cast_to_var_expr()->Set_def_stmt(stmt->Id());
    ctx.Build_chi_list(stmt, ssa_cont.Node_chi(node->Id()));
    ctx.Append_stmt(stmt);
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ld(VISITOR* visitor, air::base::NODE_PTR node) {
    HSSA_BUILDER_CTX& ctx    = visitor->Context();
    SSA_VER_ID        ver_id = ctx.Ssa_cont().Node_ver_id(node->Id());
    AIR_ASSERT(ver_id != SSA_VER_ID());
    HEXPR_PTR expr = ctx.Hssa_cont().Find_or_new_var_expr(ver_id);
    return expr;
  }

  // TODO: create new HEXPR type for LDA
  template <typename RETV, typename VISITOR>
  RETV Handle_lda(VISITOR* visitor, air::base::NODE_PTR node) {
    HSSA_BUILDER_CTX& ctx        = visitor->Context();
    HCONTAINER&       hssa_cont  = ctx.Hssa_cont();
    SSA_CONTAINER&    ssa_cont   = ctx.Ssa_cont();
    SSA_VER_ID        ver_id     = SSA_VER_ID();
    ADDR_DATUM_PTR    addr_datum = node->Addr_datum();
    MU_NODE_ID        mu         = ssa_cont.Node_mu(node->Id());
    // find ssa version from mu list
    while (mu != air::base::Null_id) {
      MU_NODE_PTR mu_ptr = ssa_cont.Mu_node(mu);
      SSA_SYM_PTR mu_sym = mu_ptr->Sym();
      if (mu_sym->Is_addr_datum() &&
          mu_sym->Var_id() == addr_datum->Id().Value() &&
          mu_sym->Index() == SSA_SYM::NO_INDEX) {
        ver_id = mu_ptr->Opnd_id();
        break;
      }
      mu = mu_ptr->Next_id();
    }
    AIR_ASSERT(ver_id != SSA_VER_ID());
    HEXPR_PTR sym_expr = hssa_cont.Find_or_new_var_expr(ver_id);
    // treat symbol as
    uint32_t kid_cnt = 1;
    OP_DATA* op_expr = OP_DATA::Alloc(node->Num_child() + 1);
    new (op_expr) OP_DATA(node);
    HEXPR_DATA_PTR op_ptr(op_expr, HEXPR_ID());
    op_expr->Set_kid_cnt(node->Num_child() + 1);
    HEXPR_PTR ret =
        hssa_cont.Find_or_new_expr(HEXPR_PTR(HEXPR(&hssa_cont, op_ptr)));
    ret->Set_kid(0, sym_expr->Id());
    free(op_expr);
    return ret;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ldf(VISITOR* visitor, air::base::NODE_PTR node) {
    HSSA_BUILDER_CTX& ctx    = visitor->Context();
    SSA_VER_ID        ver_id = ctx.Ssa_cont().Node_ver_id(node->Id());
    HEXPR_PTR         expr   = ctx.Hssa_cont().Find_or_new_var_expr(ver_id);
    return expr;
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_ldp(VISITOR* visitor, air::base::NODE_PTR node) {
    HSSA_BUILDER_CTX& ctx    = visitor->Context();
    SSA_VER_ID        ver_id = ctx.Ssa_cont().Node_ver_id(node->Id());
    HEXPR_PTR         expr   = ctx.Hssa_cont().Find_or_new_var_expr(ver_id);
    return expr;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ldpf(VISITOR* visitor, air::base::NODE_PTR node) {
    HSSA_BUILDER_CTX& ctx    = visitor->Context();
    SSA_VER_ID        ver_id = ctx.Ssa_cont().Node_ver_id(node->Id());
    HEXPR_PTR         expr   = ctx.Hssa_cont().Find_or_new_var_expr(ver_id);
    return expr;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_call(VISITOR* visitor, air::base::NODE_PTR node) {
    HSSA_BUILDER_CTX& ctx       = visitor->Context();
    HCONTAINER&       hssa_cont = ctx.Hssa_cont();
    HSTMT_PTR         stmt      = hssa_cont.New_call(node);
    PREG_PTR          ret_preg  = node->Ret_preg();
    CALL_DATA_PTR     call_stmt = stmt->Cast_to_call();
    if (ret_preg != air::base::Null_ptr) {
      SSA_VER_ID ver_id = ctx.Ssa_cont().Node_ver_id(node->Id());
      HEXPR_PTR  retv   = hssa_cont.Find_or_new_var_expr(ver_id);
      call_stmt->Set_retv(retv->Id());
    }

    for (uint32_t i = 0; i < node->Num_arg(); i++) {
      NODE_PTR  arg      = node->Child(i);
      HEXPR_PTR arg_expr = visitor->template Visit<RETV>(arg);
      call_stmt->Set_arg(i, arg_expr->Id());
    }
    ctx.Append_stmt(stmt);
    return RETV();
  }

  HPHI_PTR Create_phis(HSSA_BUILDER_CTX& ctx, BB_PTR bb, NODE_PTR node) {
    PHI_NODE_ID phi_id = ctx.Ssa_cont().Node_phi(node->Id());

    auto create_phi = [](PHI_NODE_PTR phi, BB_PTR bb, HCONTAINER& hssa_cont) {
      HPHI_PTR hphi = hssa_cont.New_phi(bb, phi->Size());
      bb->Append_phi(hphi);
    };
    PHI_LIST list(&ctx.Ssa_cont(), phi_id);
    list.For_each(create_phi, bb, ctx.Hssa_cont());
    return bb->Begin_phi();
  }

  void Handle_phi_opnd(HSSA_BUILDER_CTX& ctx, HPHI_PTR hphi, PHI_NODE_ID phi_id,
                       uint32_t phi_idx) {
    auto meet = [](PHI_NODE_PTR phi, HSSA_BUILDER_CTX& ctx, HPHI_PTR& hphi,
                   uint32_t phi_idx) {
      AIR_ASSERT(!hphi->Is_null());
      HCONTAINER& hssa_cont = ctx.Hssa_cont();
      HEXPR_PTR   expr = hssa_cont.Find_or_new_var_expr(phi->Opnd_id(phi_idx));
      hphi->Set_opnd(phi_idx, expr->Id());
      if (hphi->Next_id() != HPHI_ID()) {
        hphi = hssa_cont.Phi_ptr(hphi->Next_id());
      } else {
        hphi = Null_ptr;
      }
    };
    PHI_LIST phi_list(&ctx.Ssa_cont(), phi_id);
    phi_list.For_each(meet, ctx, hphi, phi_idx);
  }

  void Handle_phi_res(HSSA_BUILDER_CTX& ctx, HPHI_PTR hphi,
                      PHI_NODE_ID phi_id) {
    auto meet = [](PHI_NODE_PTR phi, HSSA_BUILDER_CTX& ctx, HPHI_PTR& hphi) {
      HEXPR_PTR expr = ctx.Hssa_cont().Find_or_new_var_expr(phi->Result_id());
      expr->Set_flag(EF_DEF_BY_PHI);
      expr->Set_defphi(hphi);
      hphi->Set_result(expr->Id());
      if (hphi->Next_id() != HPHI_ID()) {
        hphi = ctx.Hssa_cont().Phi_ptr(hphi->Next_id());
      } else {
        hphi = Null_ptr;
      }
    };
    PHI_LIST phi_list(&ctx.Ssa_cont(), phi_id);
    phi_list.For_each(meet, ctx, hphi);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_do_loop(VISITOR* visitor, air::base::NODE_PTR node) {
    HSSA_BUILDER_CTX& ctx       = visitor->Context();
    HCONTAINER&       hssa_cont = ctx.Hssa_cont();
    SSA_CONTAINER&    ssa_cont  = ctx.Ssa_cont();
    CFG&              cfg       = ctx.Cfg();

    BB_PTR old_cur = ctx.Cur_bb();
    AIR_ASSERT(!old_cur->Is_null());
    LOOP_INFO_PTR par_loop_info =
        ctx.In_loop() ? ctx.Cur_loop_info() : air::base::Null_ptr;
    LOOP_INFO_PTR loop_info = cfg.New_loop_info();
    ctx.Push_loop_info(loop_info);
    BB_PTR init_bb = cfg.New_bb(BB_LOOP_INIT, node->Child(0)->Spos());
    BB_PTR phi_bb  = cfg.New_bb(BB_LOOP_PHI, node->Spos());
    BB_PTR body_bb = cfg.New_bb(BB_LOOP_BODY, node->Child(3)->Spos());
    cfg.Append_bb(init_bb);
    cfg.Append_bb(phi_bb);
    cfg.Append_bb(body_bb);
    cfg.Connect_with_succ(old_cur, init_bb);
    cfg.Connect_with_succ(init_bb, phi_bb);
    cfg.Connect_with_succ(phi_bb, body_bb);

    // create hssa stmtrep and phi_node
    HPHI_PTR hphi = Create_phis(ctx, phi_bb, node);

    // create hssa for init stmt
    ctx.Set_cur_bb(init_bb);
    HEXPR_PTR init_expr = visitor->template Visit<RETV>(node->Child(0));
    HEXPR_PTR init_lhs  = hssa_cont.Find_or_new_var_expr(
        ssa_cont.Node_ver(node->Child(0)->Id())->Id());
    HSTMT_PTR init_stmt = hssa_cont.New_assign_stmt(init_lhs, init_expr);
    init_stmt->Set_lhs(init_lhs->Id());
    init_stmt->Set_rhs(init_expr->Id());
    ctx.Append_stmt(init_stmt);

    // Assume loop executed at least once, pre condition check is not generated
    // after init stmt
    Handle_phi_opnd(ctx, hphi, ssa_cont.Node_phi(node->Id()), 0);

    // create hssa for DO_LOOP body
    ctx.Set_cur_bb(body_bb);
    visitor->template Visit<RETV>(node->Child(3));

    // create hssa for DO_LOOP IV-update
    air::base::NODE_PTR step      = node->Child(2);
    HEXPR_PTR           incr_expr = visitor->template Visit<RETV>(step);
    HEXPR_PTR           incr_lhs  = hssa_cont.Find_or_new_var_expr(
        ssa_cont.Node_ver(node->Child(2)->Id())->Id());

    // TODO: do not append incr stmt, consider a better way to maintain it
    // THINK: where to put incr_stmt
    HSTMT_PTR incr_stmt = hssa_cont.New_assign_stmt(incr_lhs, incr_expr);
    // ctx.Append_stmt(incr_stmt);

    // create hssa for phi result
    Handle_phi_res(ctx, hphi, ssa_cont.Node_phi(node->Id()));

    // create hssa for DO_LOOP IV-compare
    BB_PTR cond_bb = cfg.New_bb(BB_COND, node->Child(1)->Spos());
    cfg.Append_bb(cond_bb);
    cfg.Connect_with_succ(ctx.Cur_bb(), cond_bb);
    ctx.Set_cur_bb(cond_bb);
    HEXPR_PTR cond      = visitor->template Visit<RETV>(node->Child(1));
    HSTMT_PTR cond_stmt = hssa_cont.New_if(node->Child(1), cond);
    ctx.Append_stmt(cond_stmt);

    Handle_phi_opnd(ctx, hphi, ssa_cont.Node_phi(node->Id()), 1);

    BB_PTR exit_bb = cfg.New_bb(BB_LOOP_EXIT, node->Spos());
    cfg.Append_bb(exit_bb);
    ctx.Set_cur_bb(exit_bb);
    cfg.Connect_with_succ(cond_bb, phi_bb);
    cfg.Connect_with_succ(cond_bb, exit_bb);

    loop_info->Init(init_bb, phi_bb, body_bb, cond_bb, exit_bb, init_stmt,
                    incr_stmt, cond, init_lhs, par_loop_info);

    ctx.Pop_loop_info(loop_info);
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_if(VISITOR* visitor, air::base::NODE_PTR node) {
    HSSA_BUILDER_CTX& ctx      = visitor->Context();
    CFG&              cfg      = ctx.Cfg();
    BB_PTR            old_cur  = ctx.Cur_bb();
    BB_PTR            cond_bb  = cfg.New_bb(BB_COND, node->Child(0)->Spos());
    BB_PTR            true_bb  = cfg.New_bb(BB_TRUE, node->Child(1)->Spos());
    BB_PTR            false_bb = cfg.New_bb(BB_FALSE, node->Child(2)->Spos());
    BB_PTR            phi_bb   = cfg.New_bb(BB_IF_PHI, node->Spos());

    cfg.Append_bb(cond_bb);
    cfg.Append_bb(true_bb);
    cfg.Append_bb(false_bb);
    cfg.Append_bb(phi_bb);

    AIR_ASSERT(!old_cur->Is_null());

    cfg.Connect_with_succ(old_cur, cond_bb);
    cfg.Connect_with_succ(cond_bb, true_bb);
    cfg.Connect_with_succ(cond_bb, false_bb);
    cfg.Connect_with_succ(true_bb, phi_bb);
    cfg.Connect_with_succ(false_bb, phi_bb);

    // create hssa stmtrep and phi_node
    HPHI_PTR hphi = Create_phis(ctx, phi_bb, node);

    // create hssa for if cond
    ctx.Set_cur_bb(cond_bb);
    HEXPR_PTR cond      = visitor->template Visit<RETV>(node->Child(0));
    HSTMT_PTR cond_stmt = ctx.Hssa_cont().New_if(node->Child(0), cond);

    // create hssa for then block
    ctx.Set_cur_bb(true_bb);
    visitor->template Visit<RETV>(node->Child(1));

    // create hssa for else block
    ctx.Set_cur_bb(false_bb);
    visitor->template Visit<RETV>(node->Child(2));
    // update hssa phi opnds
    Handle_phi_opnd(ctx, hphi, ctx.Ssa_cont().Node_phi(node->Id()), 0);
    Handle_phi_opnd(ctx, hphi, ctx.Ssa_cont().Node_phi(node->Id()), 1);
    Handle_phi_res(ctx, hphi, ctx.Ssa_cont().Node_phi(node->Id()));

    ctx.Set_cur_bb(phi_bb);
    return RETV();
  }
};

}  // namespace opt
}  // namespace air

#endif