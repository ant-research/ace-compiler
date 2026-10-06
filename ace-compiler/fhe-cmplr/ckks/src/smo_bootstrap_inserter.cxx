//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "smo_bootstrap_inserter.h"

#include "air/base/container.h"
#include "air/base/container_decl.h"
#include "air/base/spos.h"
#include "air/base/st_decl.h"
#include "air/base/timing_util.h"
#include "air/core/opcode.h"
#include "air/opt/dfg_container.h"
#include "air/opt/ssa_container.h"
#include "air/util/debug.h"
#include "dfg_region.h"
#include "dfg_region_container.h"
#include "fhe/ckks/ckks_gen.h"
#include "fhe/ckks/ckks_handler.h"
#include "fhe/ckks/config.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/core/rt_timing.h"
#include "resbm_ctx.h"
namespace fhe {
namespace ckks {

using namespace air::opt;

STMT_PTR INSERTER_CTX::Bootstrap_ssa_ver(SSA_VER_PTR ssa_ver, uint32_t bts_lev,
                                         bool need_rescale) {
  SSA_SYM_PTR sym  = Ssa_cntr()->Sym(ssa_ver->Sym_id());
  SPOS        spos = Glob_scope()->Unknown_simple_spos();
  if (sym->Is_addr_datum()) {
    ADDR_DATUM_PTR addr_datum =
        Cntr()->Parent_func_scope()->Addr_datum(ADDR_DATUM_ID(sym->Var_id()));
    if (Lower_ctx()->Is_cipher_type(sym->Type_id())) {
      NODE_PTR ld       = Cntr()->New_ld(addr_datum, spos);
      NODE_PTR bts_node = Bootstrap_expr(ld, bts_lev, need_rescale);
      STMT_PTR st       = Cntr()->New_st(bts_node, addr_datum, spos);
      return st;
    } else {
      TYPE_PTR type = Glob_scope()->Type(sym->Type_id());
      AIR_ASSERT(type->Is_array());
      ARRAY_TYPE_PTR arr_type = type->Cast_to_arr();
      AIR_ASSERT(arr_type->Elem_count() == 1 && arr_type->Dim() == 1);
      NODE_PTR lda   = Cntr()->New_lda(addr_datum, POINTER_KIND::FLAT32, spos);
      NODE_PTR array = Cntr()->New_array(lda, 1, spos);
      array->Set_child(1,
                       Cntr()->New_intconst(PRIMITIVE_TYPE::INT_U32, 0, spos));
      NODE_PTR ild = Cntr()->New_ild(array, spos);
      NODE_PTR bts = Bootstrap_expr(ild, bts_lev, need_rescale);
      STMT_PTR ist = Cntr()->New_ist(Cntr()->Clone_node_tree(array), bts, spos);
      return ist;
    }
  } else if (sym->Is_preg()) {
    AIR_ASSERT(Lower_ctx()->Is_cipher_type(sym->Type_id()));
    PREG_PTR preg = Cntr()->Parent_func_scope()->Preg(PREG_ID(sym->Var_id()));
    NODE_PTR ldp  = Cntr()->New_ldp(preg, spos);
    NODE_PTR bts_node = Bootstrap_expr(ldp, bts_lev, need_rescale);
    STMT_PTR stp      = Cntr()->New_stp(bts_node, preg, spos);
    return stp;
  } else {
    AIR_ASSERT_MSG(false, "not supported ssa symbol kind");
    return STMT_PTR();
  }
}

NODE_PTR INSERTER_CTX::Bootstrap_expr(NODE_PTR node, uint32_t bts_lev,
                                      bool need_rescale) {
  CKKS_GEN ckks_gen(_new_cntr, _lower_ctx);
  if (need_rescale) {
    _ctx->Trace(TD_RESBM_BTS_INSERT, "Rescale node: ");
    _ctx->Trace_obj(TD_RESBM_BTS_INSERT, node);
    node = ckks_gen.Gen_rescale(node);
  }
  _ctx->Trace(TD_RESBM_BTS_INSERT, "Bootstrap node: ");
  _ctx->Trace_obj(TD_RESBM_BTS_INSERT, node);
  NODE_PTR bts_node = ckks_gen.Gen_bootstrap(node, node->Spos());
  bts_lev += 1;
  bts_node->Set_attr(core::FHE_ATTR_KIND::LEVEL, &bts_lev, 1);
  return bts_node;
}

NODE_PTR INSERTER_CTX::Rescale_expr(NODE_PTR node) {
  CKKS_GEN ckks_gen(_new_cntr, _lower_ctx);

  _ctx->Trace(TD_RESBM_BTS_INSERT, "Rescale node: ");
  _ctx->Trace_obj(TD_RESBM_BTS_INSERT, node);
  NODE_PTR rs_node = ckks_gen.Gen_rescale(node);
  return rs_node;
}

STMT_PTR INSERTER_CTX::Rescale_ssa_ver(SSA_VER_PTR ssa_ver) {
  if (ssa_ver->Kind() == air::opt::VER_DEF_KIND::STMT) {
    NODE_PTR def_node = ssa_ver->Def_stmt()->Node();
    OPCODE   opc      = def_node->Opcode();
    AIR_ASSERT(opc == air::core::OPC_ST || opc == air::core::OPC_STP ||
               opc == air::core::OPC_IST);
    uint32_t rhs_id  = (opc == air::core::OPC_IST ? 1 : 0);
    NODE_PTR rs_node = Rescale_expr(def_node->Child(rhs_id));
    def_node->Set_child(rhs_id, rs_node);
    return STMT_PTR();
  }
  SSA_SYM_PTR sym  = Ssa_cntr()->Sym(ssa_ver->Sym_id());
  SPOS        spos = Glob_scope()->Unknown_simple_spos();
  if (sym->Is_addr_datum()) {
    ADDR_DATUM_PTR addr_datum =
        Cntr()->Parent_func_scope()->Addr_datum(ADDR_DATUM_ID(sym->Var_id()));
    if (Lower_ctx()->Is_cipher_type(sym->Type_id())) {
      NODE_PTR ld      = Cntr()->New_ld(addr_datum, spos);
      NODE_PTR rs_node = Rescale_expr(ld);
      STMT_PTR st      = Cntr()->New_st(rs_node, addr_datum, spos);
      return st;
    } else {
      TYPE_PTR type = Glob_scope()->Type(sym->Type_id());
      AIR_ASSERT(type->Is_array());
      ARRAY_TYPE_PTR arr_type = type->Cast_to_arr();
      AIR_ASSERT(arr_type->Elem_count() == 1 && arr_type->Dim() == 1);
      NODE_PTR lda   = Cntr()->New_lda(addr_datum, POINTER_KIND::FLAT32, spos);
      NODE_PTR array = Cntr()->New_array(lda, 1, spos);
      array->Set_child(1,
                       Cntr()->New_intconst(PRIMITIVE_TYPE::INT_U32, 0, spos));
      NODE_PTR ild = Cntr()->New_ild(array, spos);
      NODE_PTR rs  = Rescale_expr(ild);
      STMT_PTR ist = Cntr()->New_ist(Cntr()->Clone_node_tree(array), rs, spos);
      return ist;
    }
  } else if (sym->Is_preg()) {
    AIR_ASSERT(Lower_ctx()->Is_cipher_type(sym->Type_id()));
    PREG_PTR preg = Cntr()->Parent_func_scope()->Preg(PREG_ID(sym->Var_id()));
    NODE_PTR ldp  = Cntr()->New_ldp(preg, spos);
    NODE_PTR rs_node = Rescale_expr(ldp);
    STMT_PTR stp     = Cntr()->New_stp(rs_node, preg, spos);
    return stp;
  } else {
    AIR_ASSERT_MSG(false, "not supported ssa symbol kind");
    return STMT_PTR();
  }
}

void INSERTER_CTX::Append_bts(STMT_PTR bts_stmt) {
  if (!Resbm_ctx()->Rt_timing()) {
    Append(bts_stmt);
    return;
  }

  air::base::TIMING_GEN tm(Cntr(), core::RTM_FHE_BOOTSTRAP, _bts_cnt,
                           bts_stmt->Spos());
  Append(tm.New_tm_start());
  Append(bts_stmt);
  Append(tm.New_tm_end());
}

void INSERTER_CTX::Trace_bts_tm(STMT_PTR bts_stmt) {
  if (!Resbm_ctx()->Rt_timing()) return;

  air::base::TIMING_GEN tm(Cntr(), core::RTM_FHE_BOOTSTRAP, _bts_cnt,
                           bts_stmt->Spos());
  STMT_LIST             sl(bts_stmt->Parent_node());
  sl.Prepend(bts_stmt, tm.New_tm_start());
  sl.Append(bts_stmt, tm.New_tm_end());

  Resbm_ctx()->Trace(TD_RESBM_BTS_INSERT, "    Trace runtime of bootstrap for ",
                     _bts_cnt, "\n");
}

void INSERTER_CTX::Handle_ssa_ver(SSA_VER_PTR ver) {
  if (Need_rescale(ver->Id())) {
    STMT_PTR rs_stmt = Rescale_ssa_ver(ver);
    if (rs_stmt != STMT_PTR()) Append(rs_stmt);
    Erase_rs_info(ver->Id());
  }
  BTS_INFO::SET::const_iterator bts_iter = Bootstrap_info(ver->Id());
  if (bts_iter != Bootstrap_info().end()) {
    STMT_PTR bts_stmt = Bootstrap_ssa_ver(ver, bts_iter->Bts_lev(), false);
    Append_bts(bts_stmt);
    Erase_bts_info(bts_iter);
  }
}

NODE_PTR INSERTER_CTX::Handle_expr(NODE_ID orig_id, NODE_PTR new_node) {
  if (Need_rescale(orig_id)) {
    new_node = Rescale_expr(new_node);
    Erase_rs_info(orig_id);
  }

  BTS_INFO::SET_ITER bts_iter = Bootstrap_info(orig_id);
  if (bts_iter != Bootstrap_info().end()) {
    NODE_PTR parent_node = Parent(1);
    if (parent_node->Opcode() == air::core::OPC_ST ||
        parent_node->Opcode() == air::core::OPC_STP) {
      SSA_VER_PTR ver      = Ssa_cntr()->Node_ver(parent_node->Id());
      STMT_PTR    bts_stmt = Bootstrap_ssa_ver(ver, bts_iter->Bts_lev(), false);
      Append_bts(bts_stmt);
    } else {
      new_node = Bootstrap_expr(new_node, bts_iter->Bts_lev(), false);
    }
    Erase_bts_info(bts_iter);
  }
  return new_node;
}

void SMO_BTS_INSERTER::Collect_bootstrap_point(
    REGION_ELEM_ID elem_id, uint32_t bts_lev,
    const ELEM_BTS_INFO::SET& redund_bts) {
  REGION_ELEM_PTR elem = Resbm_ctx()->Region_cntr()->Region_elem(elem_id);
  ELEM_BTS_INFO   bts_info(elem, bts_lev);
  if (redund_bts.find(bts_info) != redund_bts.end()) {
    Resbm_ctx()->Trace(TD_RESBM_BTS_INSERT, "    Skip redundant bts point");
    Resbm_ctx()->Trace_obj(TD_RESBM_BTS_INSERT, elem);
    return;
  }

  BTS_SET& bts_set = Bts_set(elem->Callsite_info());
  if (bts_info.Is_expr()) {
    bts_set.insert(BTS_INFO(bts_info.Expr(), bts_info.Bts_lev()));
  } else {
    AIR_ASSERT(bts_info.Is_ssa_ver());
    bts_set.insert(BTS_INFO(bts_info.Ssa_ver(), bts_info.Bts_lev()));
  }

  Resbm_ctx()->Trace(TD_RESBM_BTS_INSERT, "    Add bootstrap point: ");
  Resbm_ctx()->Trace_obj(TD_RESBM_BTS_INSERT, &bts_info);
}

void SMO_BTS_INSERTER::Collect_formal_scale_level(
    const MIN_LATENCY_PLAN* plan) {
  for (auto& scale_info_pair : plan->Formal_scale_lev()) {
    const CALLSITE_INFO& callsite          = scale_info_pair.first;
    VAR_SCALE_LEV::VEC&  formal_scale_list = Formal_scale_info(callsite);
    for (const VAR_SCALE_LEV& formal_scale : scale_info_pair.second) {
      Resbm_ctx()->Trace_obj(TD_RESBM_BTS_INSERT, &callsite);

      ADDR_DATUM_PTR formal     = formal_scale.Var();
      FUNC_SCOPE*    func_scope = formal->Defining_func_scope();
      uint32_t       formal_id  = 0;
      for (; formal_id < func_scope->Formal_cnt(); ++formal_id) {
        if (func_scope->Formal(formal_id) == formal) break;
      }
      AIR_ASSERT(formal_id < func_scope->Formal_cnt());

      if (formal_scale_list.size() <= formal_id) {
        formal_scale_list.resize(formal_id + 1);
      }
      formal_scale_list[formal_id] = formal_scale;

      Resbm_ctx()->Trace(TD_RESBM_BTS_INSERT, "    Set scale/level of formal ");
      Resbm_ctx()->Trace_obj(TD_RESBM_BTS_INSERT, &formal_scale);
    }
  }
}

void SMO_BTS_INSERTER::Collect_bootstrap_point(const MIN_LATENCY_PLAN* plan,
                                               ELEM_BTS_INFO::SET& redund_bts) {
  // 1. collect bootstrap in the src region
  for (REGION_ELEM_ID elem_id : plan->Bootstrap_point().Cut_elem()) {
    Collect_bootstrap_point(elem_id, plan->Used_level(), redund_bts);
  }
  // 2. collect bootstrap for bypass edge
  for (std::pair<REGION_ELEM_ID, uint32_t> bypass_bts_info :
       plan->Bypass_edge_bts_info()) {
    Collect_bootstrap_point(bypass_bts_info.first, bypass_bts_info.second,
                            redund_bts);
  }
}

void SMO_BTS_INSERTER::Collect_rescale_point(const MIN_LATENCY_PLAN* plan) {
  if (Resbm_ctx()->Rgn_bts_mng()) return;

  for (uint32_t id = (plan->Src_id().Value() + 1); id <= plan->Dst_id().Value();
       ++id) {
    const CUT_TYPE& rs_cut = plan->Min_cut(REGION_ID(id));
    for (REGION_ELEM_ID elem_id : rs_cut.Cut_elem()) {
      REGION_ELEM_PTR elem   = Resbm_ctx()->Region_cntr()->Region_elem(elem_id);
      RS_SET&         rs_set = Rs_set(elem->Callsite_info());
      DFG_NODE_PTR    dfg_node = elem->Dfg_node();
      if (dfg_node->Is_ssa_ver()) {
        rs_set.emplace(NODE_INFO(dfg_node->Ssa_ver_id()));
      } else {
        rs_set.emplace(NODE_INFO(dfg_node->Node_id()));
      }

      Resbm_ctx()->Trace(TD_RESBM_BTS_INSERT, "    Add rescale point: ");
      Resbm_ctx()->Trace_obj(TD_RESBM_BTS_INSERT, elem);
    }
  }
}

void SMO_BTS_INSERTER::Collect_rescale_bts_point() {
  const REGION_CONTAINER* reg_cntr   = Resbm_ctx()->Region_cntr();
  uint32_t                region_cnt = reg_cntr->Region_cnt();
  if (region_cnt <= 2) {
    Resbm_ctx()->Trace(TD_RESBM_BTS_INSERT, "    Region count= ", region_cnt,
                       ", not need rescale/bootstrap.\n");
    return;
  }

  REGION_ID          last_region(region_cnt - 1);
  MIN_LATENCY_PLAN*  plan = Resbm_ctx()->Plan(last_region);
  ELEM_BTS_INFO::SET redundant_bts;
  while (plan != nullptr) {
    Resbm_ctx()->Trace(TD_RESBM_BTS_INSERT,
                       "    >>>>Collecting bootstrap/rescale point for plan: [",
                       plan->Src_id().Value(), ", ", plan->Dst_id().Value(),
                       "]\n");

    // 1. collect bootstrap points
    redundant_bts.merge(plan->Redundant_bootstrap());
    Collect_bootstrap_point(plan, redundant_bts);

    // 2, collect rescale points
    Collect_rescale_point(plan);

    // 3. collect scale and level of formal at each callsite
    Collect_formal_scale_level(plan);

    plan = Resbm_ctx()->Plan(plan->Src_id());
  }
}

void SMO_BTS_INSERTER::Bootstrap_ssa_ver(INSERTER_CTX& ctx, SSA_VER_PTR ver,
                                         uint32_t bts_lev, bool need_rescale) {
  // 1. Get define stmt of SSA_VER.
  STMT_PTR def_stmt;
  if (ver->Kind() == air::opt::VER_DEF_KIND::PHI) {
    def_stmt = ctx.Ssa_cntr()->Phi_node(ver->Def_phi_id())->Def_stmt();
  } else if (ver->Kind() == VER_DEF_KIND::CHI ||
             ver->Kind() == VER_DEF_KIND::STMT) {
    def_stmt = ctx.Cntr()->Stmt(ver->Def_stmt_id());
  } else {
    AIR_ASSERT_MSG(false, "SSA_VER with not supported define kind");
  }

  // 2. Create a st stmt which store bootstrap result in SSA_VER.
  //    Insert the bootstrap stmt after the define stmt.
  STMT_PTR bts_stmt;
  OPCODE   opc = def_stmt->Opcode();
  if (opc == air::core::OPC_ST || opc == air::core::OPC_STP ||
      opc == air::core::OPC_IST) {
    // 1. store rhs in a new preg
    uint32_t rhs_id = (opc == air::core::OPC_IST) ? 1 : 0;
    NODE_PTR rhs    = def_stmt->Node()->Child(rhs_id);
    if (need_rescale) {
      CKKS_GEN gen(ctx.Cntr(), ctx.Lower_ctx());
      rhs = gen.Gen_rescale(rhs);
    }
    AIR_ASSERT_MSG(Lower_ctx()->Is_cipher_type(rhs->Rtype_id()),
                   "currently only support bootstrap single ciphertext");
    SPOS     spos = rhs->Spos();
    PREG_PTR preg = ctx.Cntr()->Parent_func_scope()->New_preg(rhs->Rtype());
    STMT_PTR stp  = ctx.Cntr()->New_stp(rhs, preg, spos);
    STMT_LIST(def_stmt->Parent_node()).Prepend(def_stmt, stp);

    // 2. bootstrap the preg, and replace the rhs with bootstrap node
    NODE_PTR bts_node =
        ctx.Bootstrap_expr(ctx.Cntr()->New_ldp(preg, spos), bts_lev, false);
    def_stmt->Node()->Set_child(rhs_id, bts_node);
    bts_stmt = def_stmt;
  } else {
    bts_stmt = ctx.Bootstrap_ssa_ver(ver, bts_lev, need_rescale);
    if (def_stmt->Node()->Is_entry()) {
      // SSA_VER is defined at entry
      AIR_ASSERT(ver->Version() == 1);
      ctx.Cntr()->Stmt_list().Prepend(bts_stmt);
    } else {
      AIR_ASSERT(bts_stmt != def_stmt);
      STMT_LIST sl(def_stmt->Parent_node());
      sl.Append(def_stmt, bts_stmt);
    }
  }

  // 3. Add time trace stmts if 'Rt_validate' is set to true.
  ctx.Trace_bts_tm(bts_stmt);
}

//! @brief insert bootstrap for an expression.
void SMO_BTS_INSERTER::Bootstrap_expr(INSERTER_CTX& ctx, DFG_NODE_PTR dfg_node,
                                      uint32_t bts_lev, bool need_rescale) {
  AIR_ASSERT(dfg_node->Is_node());
  NODE_PTR node = dfg_node->Node();
  AIR_ASSERT(Lower_ctx()->Is_cipher_type(node->Rtype_id()));
  NODE_PTR bts_node = ctx.Bootstrap_expr(node, bts_lev, need_rescale);
  uint32_t cnt      = 0;
  for (DFG_EDGE_ITER edge_iter = dfg_node->Begin_succ();
       edge_iter != dfg_node->End_succ(); ++edge_iter) {
    DFG_NODE_PTR dst_node = edge_iter->Dst();
    if (dst_node->Is_ssa_ver()) {
      SSA_VER_PTR ssa_ver = dst_node->Ssa_ver();
      Bootstrap_ssa_ver(ctx, ssa_ver, bts_lev, need_rescale);
      ++cnt;
    } else if (dst_node->Is_node()) {
      AIR_ASSERT_MSG(false, "Not supported in CKKS2POLY");
      NODE_PTR parent_node = dst_node->Node();
      for (uint32_t id = 0; id < parent_node->Num_child(); ++id) {
        if (parent_node->Child_id(id) != node->Id()) continue;
        parent_node->Set_child(id, bts_node);
        ++cnt;
      }
    } else {
      AIR_ASSERT_MSG(false, "not supported DFG node");
    }
  }
  AIR_ASSERT(cnt == 1);
}

void SMO_BTS_INSERTER::Bootstrap_entry_func(FUNC_ID func, const RS_SET& rs_set,
                                            const BTS_SET& bts_set) {
  if (bts_set.empty()) return;

  const DFG_CONTAINER* dfg_cntr = Resbm_ctx()->Region_cntr()->Dfg_cntr(func);
  const SSA_CONTAINER* ssa_cntr = dfg_cntr->Ssa_cntr();
  CONTAINER*           cntr     = ssa_cntr->Container();
  INSERTER_CTX         insert_ctx(bts_set, rs_set, ssa_cntr, cntr, Lower_ctx(),
                                  Resbm_ctx());
  for (const BTS_INFO& bts_info : bts_set) {
    if (bts_info.Is_expr()) {
      DFG_NODE_PTR dfg_node =
          dfg_cntr->Node(dfg_cntr->Node_id(bts_info.Expr()));
      bool need_rescale = insert_ctx.Need_rescale(dfg_node->Node_id());
      Bootstrap_expr(insert_ctx, dfg_node, bts_info.Bts_lev(), need_rescale);
    } else if (bts_info.Is_ssa_ver()) {
      SSA_VER_PTR ver          = ssa_cntr->Ver(bts_info.Ssa_ver());
      bool        need_rescale = insert_ctx.Need_rescale(ver->Id());
      Bootstrap_ssa_ver(insert_ctx, ver, bts_info.Bts_lev(), need_rescale);
    } else {
      AIR_ASSERT_MSG(false, "not supported data kind");
    }
  }
}

void SMO_BTS_INSERTER::Rescale_ssa_ver(INSERTER_CTX& ctx, SSA_VER_PTR ver) {
  // 1. Get define stmt of SSA_VER.
  STMT_PTR def_stmt;
  if (ver->Kind() == air::opt::VER_DEF_KIND::PHI) {
    def_stmt = ctx.Ssa_cntr()->Phi_node(ver->Def_phi_id())->Def_stmt();
  } else if (ver->Kind() == VER_DEF_KIND::CHI ||
             ver->Kind() == VER_DEF_KIND::STMT) {
    def_stmt = ctx.Cntr()->Stmt(ver->Def_stmt_id());
  } else {
    AIR_ASSERT_MSG(false, "SSA_VER with not supported define kind");
  }

  // 2. Create a rescale stmt, and store the rescale result in the SSA_VER's
  //    symbol. Then, insert the rescale stmt after the define stmt.
  STMT_PTR rs_stmt = ctx.Rescale_ssa_ver(ver);
  if (rs_stmt != STMT_PTR()) {
    if (def_stmt->Node()->Is_entry()) {
      // SSA_VER is defined at program entry
      ctx.Cntr()->Stmt_list().Prepend(rs_stmt);
    } else {
      STMT_LIST sl(def_stmt->Parent_node());
      sl.Append(def_stmt, rs_stmt);
    }

    Resbm_ctx()->Trace(TD_RESBM_BTS_INSERT,
                       "    Insert rescale stmt in entry func:\n");
    Resbm_ctx()->Trace_obj(TD_RESBM_BTS_INSERT, rs_stmt);
  }
}

//! @brief insert bootstrap for an expression.
void SMO_BTS_INSERTER::Rescale_expr(INSERTER_CTX& ctx, DFG_NODE_PTR dfg_node) {
  AIR_ASSERT(dfg_node->Is_node());
  NODE_PTR node = dfg_node->Node();
  AIR_ASSERT(Lower_ctx()->Is_cipher_type(node->Rtype_id()) ||
             Lower_ctx()->Is_cipher3_type(node->Rtype_id()));
  NODE_PTR rs_node = ctx.Rescale_expr(node);
  for (DFG_EDGE_ITER edge_iter = dfg_node->Begin_succ();
       edge_iter != dfg_node->End_succ(); ++edge_iter) {
    DFG_NODE_PTR dst_node = edge_iter->Dst();
    if (dst_node->Is_node()) {
      NODE_PTR parent_node = dst_node->Node();
      for (uint32_t id = 0; id < parent_node->Num_child(); ++id) {
        if (parent_node->Child_id(id) != node->Id()) continue;
        parent_node->Set_child(id, rs_node);
      }
    } else if (dst_node->Is_ssa_ver()) {
      SSA_VER_PTR ssa_ver = dst_node->Ssa_ver();
      AIR_ASSERT(ssa_ver->Kind() == air::opt::VER_DEF_KIND::STMT);
      STMT_PTR def_stmt = ssa_ver->Def_stmt();
      OPCODE   opc      = def_stmt->Opcode();
      if (opc == air::core::OPC_ST || opc == air::core::OPC_STP) {
        def_stmt->Node()->Set_child(0, rs_node);
      } else if (opc == air::core::OPC_IST) {
        def_stmt->Node()->Set_child(1, rs_node);
      } else {
        AIR_ASSERT_MSG(false, "not supported define opcode");
      }
    } else {
      AIR_ASSERT_MSG(false, "not supported DFG node");
    }
  }
}

void SMO_BTS_INSERTER::Rescale_entry_func(FUNC_ID func, const RS_SET& rs_set,
                                          const BTS_SET& bts_set) {
  if (rs_set.empty()) return;

  const DFG_CONTAINER* dfg_cntr = Resbm_ctx()->Region_cntr()->Dfg_cntr(func);
  const SSA_CONTAINER* ssa_cntr = dfg_cntr->Ssa_cntr();
  CONTAINER*           cntr     = ssa_cntr->Container();
  INSERTER_CTX         insert_ctx(bts_set, rs_set, ssa_cntr, cntr, Lower_ctx(),
                                  Resbm_ctx());
  for (const NODE_INFO& rs_info : rs_set) {
    if (rs_info.Is_expr()) {
      // Delay rescaling insertion. For nodes requiring both rescaling and
      // bootstrapping, insert them simultaneously.
      if (insert_ctx.Need_bootstrap(rs_info.Expr())) continue;

      DFG_NODE_ID dfg_node_id = dfg_cntr->Node_id(rs_info.Expr());
      if (dfg_node_id == Null_id) {
        rs_info.Print();
        cntr->Node(rs_info.Expr())->Print();
      }
      DFG_NODE_PTR dfg_node = dfg_cntr->Node(dfg_node_id);
      Rescale_expr(insert_ctx, dfg_node);
    } else if (rs_info.Is_ssa_ver()) {
      // Delay rescaling insertion. For nodes requiring both rescaling and
      // bootstrapping, insert them simultaneously.
      if (insert_ctx.Need_bootstrap(rs_info.Ssa_ver())) continue;

      SSA_VER_PTR ver = ssa_cntr->Ver(rs_info.Ssa_ver());
      Rescale_ssa_ver(insert_ctx, ver);
    } else {
      AIR_ASSERT_MSG(false, "not supported data kind");
    }
  }
}

void SMO_BTS_INSERTER::Handle_entry_func(const BTS_SET& bts_set,
                                         const RS_SET&  rs_set) {
  FUNC_ID entry_func_id = Resbm_ctx()->Region_cntr()->Entry_func();
  // Rescaling and bootstrapping are inserted based on the DFG, which is not
  // updated during the insertion process. For nodes requiring both, insert
  // rescaling and bootstrapping simultaneously. Initially, insert rescaling for
  // nodes that only require it. Nodes needing both are delayed until
  // bootstrapping insertion.
  Rescale_entry_func(entry_func_id, rs_set, bts_set);
  Bootstrap_entry_func(entry_func_id, rs_set, bts_set);
}

FUNC_PTR SMO_BTS_INSERTER::Planed_func(FUNC_ID func, const BTS_SET& bts_set,
                                       const RS_SET& rs_set) {
  using ITER           = std::map<BTS_SET, FUNC_ID>::iterator;
  FUNC_ID planned_func = Plan_func(bts_set, rs_set);
  if (!planned_func.Is_null()) {
    return Glob_scope()->Func(planned_func);
  }

  FUNC_PTR           orig_func       = Glob_scope()->Func(func);
  FUNC_SCOPE*        orig_func_scope = &Glob_scope()->Open_func_scope(func);
  SIGNATURE_TYPE_PTR sig  = orig_func->Entry_point()->Type()->Cast_to_sig();
  const SPOS&        spos = Glob_scope()->Unknown_simple_spos();

  std::string new_func_name(orig_func->Name()->Char_str());
  new_func_name += "_" + std::to_string(_plan_func.size());

  FUNC_PTR new_func = Glob_scope()->New_func(new_func_name.c_str(), spos);
  Glob_scope()->New_entry_point(sig, new_func, new_func_name.c_str(), spos);

  FUNC_SCOPE* new_func_scope = &Glob_scope()->New_func_scope(new_func);
  new_func_scope->Clone(*orig_func_scope);
  CONTAINER* new_cntr = &new_func_scope->Container();
  new_cntr->New_func_entry(spos);

  const SSA_CONTAINER* ssa_cntr = Resbm_ctx()->Region_cntr()->Ssa_cntr(func);
  INSERTER_CTX inserter_ctx(bts_set, rs_set, ssa_cntr, new_cntr, Lower_ctx(),
                            Resbm_ctx());
  VISITOR      visitor(inserter_ctx);
  (void)visitor.Visit<INSERTER_RETV>(ssa_cntr->Container()->Entry_node());

  AIR_ASSERT_MSG(inserter_ctx.Bootstraped_all(),
                 "exists not processed bootstrap");

  Set_plan_func(bts_set, rs_set, new_func->Id());
  return new_func;
}

void SMO_BTS_INSERTER::Set_param_scale_lev(const CALLSITE_INFO& callsite,
                                           NODE_PTR param, uint32_t formal_id) {
  if (!Lower_ctx()->Is_cipher_type(param->Rtype_id()) &&
      !Lower_ctx()->Is_cipher3_type(param->Rtype_id()))
    return;

  FORMAL_SCALE_LEV::const_iterator iter = Formal_scale_info().find(callsite);
  AIR_ASSERT(iter != Formal_scale_info().end());

  FUNC_SCOPE*    func_scope = &Glob_scope()->Open_func_scope(callsite.Callee());
  ADDR_DATUM_PTR formal     = func_scope->Formal(formal_id);

  const VAR_SCALE_LEV::VEC& formal_scale_vec = iter->second;
  AIR_ASSERT(formal_scale_vec.size() > formal_id);
  const VAR_SCALE_LEV& formal_scale_lev = formal_scale_vec[formal_id];
  AIR_ASSERT(!formal_scale_lev.Scale_lev().Is_invalid());
  uint32_t level = formal_scale_lev.Scale_lev().Level() + 1;

  param->Set_attr(core::FHE_ATTR_KIND::LEVEL, &level, 1);
}

void SMO_BTS_INSERTER::Handle_called_func(const CALLSITE_INFO& callsite,
                                          const BTS_SET&       bts_set,
                                          const RS_SET&        rs_set) {
  const REGION_CONTAINER* reg_cntr    = Resbm_ctx()->Region_cntr();
  FUNC_ID                 caller_func = callsite.Caller();
  FUNC_ID                 callee_func = callsite.Callee();
  AIR_ASSERT(!callee_func.Is_null());
  AIR_ASSERT(!caller_func.Is_null() && caller_func == reg_cntr->Entry_func());
  // clone the called function for new bootstrap plan
  FUNC_PTR        bts_func       = Planed_func(callee_func, bts_set, rs_set);
  CONST_ENTRY_PTR bts_func_entry = bts_func->Entry_point();

  // create call stmt of the cloned function, and replace the call stmt.
  const DFG_CONTAINER* dfg_cntr   = reg_cntr->Dfg_cntr(caller_func);
  FUNC_SCOPE*          func_scope = &Glob_scope()->Open_func_scope(caller_func);
  CONTAINER*           cntr       = &func_scope->Container();
  NODE_PTR             call_node  = dfg_cntr->Node(callsite.Node())->Node();
  STMT_PTR new_call = cntr->New_call(bts_func_entry, call_node->Ret_preg(),
                                     call_node->Num_arg(), call_node->Spos());
  for (uint32_t id = 0; id < call_node->Num_child(); ++id) {
    NODE_PTR child = call_node->Child(id);
    new_call->Node()->Set_child(id, child);
    Set_param_scale_lev(callsite, child, id);
  }
  new_call->Node()->Copy_attr(call_node);

  STMT_LIST stmt_list(call_node->Stmt()->Parent_node());
  stmt_list.Append(call_node->Stmt(), new_call);
  stmt_list.Remove(call_node->Stmt());

  Resbm_ctx()->Trace(TD_RESBM_BTS_INSERT, "    Replace call stmt:\n");
  Resbm_ctx()->Trace_obj(TD_RESBM_BTS_INSERT, call_node);
  Resbm_ctx()->Trace_obj(TD_RESBM_BTS_INSERT, new_call);
}

void SMO_BTS_INSERTER::Insert_smo_bootstrap() {
  // 1. Insert the necessary bootstrap and rescale operations for the entry and
  // called functions.
  const REGION_CONTAINER* reg_cntr      = Resbm_ctx()->Region_cntr();
  FUNC_ID                 entry_func_id = reg_cntr->Entry_func();
  for (const std::pair<CALLSITE_INFO, BTS_SET>& call_bts_pair :
       Callsite_bts()) {
    const CALLSITE_INFO& callsite = call_bts_pair.first;
    const BTS_SET&       bts_set  = call_bts_pair.second;
    const RS_SET&        rs_set   = Rs_set(call_bts_pair.first);
    if (callsite.Callee() == entry_func_id) {
      Handle_entry_func(bts_set, rs_set);
    } else {
      Handle_called_func(callsite, bts_set, rs_set);
    }
    Callsite_rs().erase(callsite);
  }

  // 2. Insert the necessary rescale operations for functions without bootstrap.
  for (const std::pair<CALLSITE_INFO, RS_SET> call_rs_pair : Callsite_rs()) {
    const CALLSITE_INFO& callsite = call_rs_pair.first;
    const RS_SET&        rs_set   = call_rs_pair.second;
    if (rs_set.empty()) continue;
    if (callsite.Callee() == entry_func_id) {
      Handle_entry_func(BTS_SET(), rs_set);
    } else {
      Handle_called_func(callsite, BTS_SET(), rs_set);
    }
  }
}

void SMO_BTS_INSERTER::Perform() {
  // 1. collect region element to insert bootstrap/rescale.
  Collect_rescale_bts_point();

  // 2. insert bootstrap/rescale operations.
  Insert_smo_bootstrap();
}

}  // namespace ckks
}  // namespace fhe
