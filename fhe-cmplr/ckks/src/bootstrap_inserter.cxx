//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "bootstrap_inserter.h"

#include <string>
#include <utility>

#include "air/base/container.h"
#include "air/base/container_decl.h"
#include "air/base/ptr_wrapper.h"
#include "air/base/st_decl.h"
#include "air/opt/dfg_container.h"
#include "air/opt/dfg_node.h"
#include "air/opt/ssa_container.h"
#include "air/util/debug.h"
#include "dfg_region.h"
#include "dfg_region_container.h"
#include "fhe/ckks/ckks_handler.h"
#include "fhe/core/lower_ctx.h"
#include "min_cut_region.h"
#include "resbm_ctx.h"
namespace fhe {
namespace ckks {

using namespace air::opt;

void BOOTSTRAP_INSERTER::Collect_bootstrap_points() {
  const REGION_CONTAINER* reg_cntr   = Resbm_ctx()->Region_cntr();
  uint32_t                region_cnt = reg_cntr->Region_cnt();
  AIR_ASSERT_MSG(region_cnt >= 1, "");
  REGION_ID         last_region(region_cnt - 1);
  MIN_LATENCY_PLAN* plan = Resbm_ctx()->Noise_mng_plan(last_region);
  while (plan != nullptr) {
    MIN_CUT::CUT_TYPE cut = plan->Bootstrap_point();
    for (REGION_ELEM_ID elem_id : cut) {
      REGION_ELEM_PTR elem     = reg_cntr->Region_elem(elem_id);
      FUNC_ID         callee   = elem->Callee();
      DFG_NODE_PTR    dfg_node = elem->Dfg_node();
      BTS_POINT&      bts_point =
          _bts_point[callee]
              .emplace(elem->Callsite_info(), BTS_POINT(callee))
              .first->second;
      if (dfg_node->Is_node()) {
        bts_point.Add_expr(dfg_node->Node_id(), plan->Consume_level());
      } else if (dfg_node->Is_ssa_ver()) {
        bts_point.Add_ver(dfg_node->Ssa_ver_id(), plan->Consume_level());
      } else {
        AIR_ASSERT_MSG(false, "not supported DFG node kind");
      }
    }

    for (auto& scale_info_pair : plan->Formal_scale_info()) {
      std::list<VAR_SCALE_INFO>& scale_info =
          Formal_scale_info()[scale_info_pair.first];
      for (const VAR_SCALE_INFO& new_formal_scale : scale_info_pair.second) {
        bool recorded_formal = false;
        for (VAR_SCALE_INFO& pre_formal_scale : scale_info) {
          if (pre_formal_scale.Var() == new_formal_scale.Var()) {
            pre_formal_scale.Set_scale_info(new_formal_scale.Scale_info());
            recorded_formal = true;
            break;
          }
        }
        if (!recorded_formal) scale_info.push_back(new_formal_scale);
      }
    }
    plan = Resbm_ctx()->Noise_mng_plan(plan->Start_region());
  }
}

air::base::FUNC_SCOPE* BOOTSTRAP_INSERTER::Gen_new_func(FUNC_ID     func,
                                                        const char* suffix) {
  air::base::FUNC_PTR old_func       = Glob_scope()->Func(func);
  FUNC_SCOPE*         old_func_scope = &Glob_scope()->Open_func_scope(func);
  std::string         new_name(old_func->Name()->Char_str());
  new_name += suffix;
  air::base::STR_PTR     name_str  = Glob_scope()->New_str(new_name.c_str());
  const air::base::SPOS& spos      = old_func->Spos();
  air::base::FUNC_PTR    new_func  = Glob_scope()->New_func(name_str, spos);
  air::base::ENTRY_PTR   new_entry = Glob_scope()->New_entry_point(
      old_func->Entry_point()->Type_id(), new_func->Id(), name_str->Id(), spos);
  FUNC_SCOPE* new_func_scope = &Glob_scope()->New_func_scope(new_func->Id());
  new_func_scope->Clone(*old_func_scope);
  air::base::STMT_PTR entry_stmt = new_func_scope->Container().Clone_stmt_tree(
      old_func_scope->Container().Entry_stmt());
  new_func_scope->Set_entry_stmt(entry_stmt->Id());
  entry_stmt->Node()->Set_entry(new_entry->Id());
  return new_func_scope;
}

void BOOTSTRAP_INSERTER::Insert_bootstrap_ver(BTS_INSERTER_CTX& ctx,
                                              SSA_VER_PTR       ver,
                                              uint32_t          bts_lev) {
  // 1. Create a bootstrap stmt, and store the bootstrap result in the SSA_VER's
  // ADDR_DATUM/PREG.
  STMT_PTR def_stmt;
  STMT_PTR bts_stmt = ctx.Bootstrap_ssa_ver(ver, bts_lev);
  if (ver->Kind() == air::opt::VER_DEF_KIND::PHI) {
    def_stmt = ctx.Ssa_cntr()->Phi_node(ver->Def_phi_id())->Def_stmt();
  } else if (ver->Kind() == VER_DEF_KIND::CHI ||
             ver->Kind() == VER_DEF_KIND::STMT) {
    def_stmt = ctx.Cntr()->Stmt(ver->Def_stmt_id());
  } else {
    // SSA_VER is formal and has no define stmt, insert bootstrap at entry
    AIR_ASSERT(ver->Version() == SSA_VER::NO_VER);
    ctx.Cntr()->Stmt_list().Prepend(bts_stmt);
    return;
  }

  // 2. Insert the bootstrap statement into the statement list and
  //    append a time trace statement if 'Rt_validate' is set to true.
  STMT_LIST sl(def_stmt->Parent_node());
  sl.Append(def_stmt, bts_stmt);
  if (Resbm_ctx()->Rt_validate()) {
    STMT_PTR tm_start = ctx.Bts_tm_start();
    STMT_PTR tm_taken = ctx.Bts_tm_taken();
    sl.Prepend(bts_stmt, tm_start);
    sl.Append(bts_stmt, tm_taken);
  }
  return;
}

//! @brief insert bootstrap for an EXPR.
void BOOTSTRAP_INSERTER::Insert_bootstrap_expr(BTS_INSERTER_CTX& ctx,
                                               DFG_NODE_PTR      dfg_node,
                                               uint32_t          bts_lev) {
  AIR_ASSERT(dfg_node->Is_node());
  NODE_PTR node = dfg_node->Node();
  AIR_ASSERT(Lower_ctx()->Is_cipher_type(node->Rtype_id()));
  NODE_PTR bts_node = ctx.Bootstrap_node(node, bts_lev);
  uint32_t cnt      = 0;
  for (DFG_EDGE_ITER edge_iter = dfg_node->Begin_succ();
       edge_iter != dfg_node->End_succ(); ++edge_iter) {
    DFG_NODE_PTR dst_node = edge_iter->Dst();
    if (dst_node->Is_node()) {
      NODE_PTR parent_node = dst_node->Node();
      for (uint32_t id = 0; id < parent_node->Num_child(); ++id) {
        if (parent_node->Child_id(id) != node->Id()) continue;
        parent_node->Set_child(id, bts_node);
        ++cnt;
      }
    } else if (dst_node->Is_ssa_ver()) {
      Insert_bootstrap_ver(ctx, dst_node->Ssa_ver(), bts_lev);
      ++cnt;
    } else {
      AIR_ASSERT_MSG(false, "not supported DFG node");
    }
  }
  AIR_ASSERT(cnt == 1);
}

void BOOTSTRAP_INSERTER::Insert_bootstrap_orig_func(
    FUNC_ID func, const BTS_POINT& bts_point) {
  const DFG_CONTAINER* dfg_cntr = Resbm_ctx()->Region_cntr()->Dfg_cntr(func);
  const SSA_CONTAINER* ssa_cntr = dfg_cntr->Ssa_cntr();
  CONTAINER*           cntr     = ssa_cntr->Container();
  BTS_INSERTER_CTX     insert_ctx(bts_point, ssa_cntr, cntr, Lower_ctx(),
                                  Resbm_ctx());
  for (const BTS_INFO& bts_info : bts_point.Bts_info()) {
    if (bts_info.Is_expr()) {
      DFG_NODE_PTR dfg_node =
          dfg_cntr->Node(dfg_cntr->Node_id(bts_info.Expr()));
      Insert_bootstrap_expr(insert_ctx, dfg_node, bts_info.Bts_lev());
    } else if (bts_info.Is_ssa_ver()) {
      SSA_VER_PTR ver = ssa_cntr->Ver(bts_info.Ssa_ver());
      Insert_bootstrap_ver(insert_ctx, ver, bts_info.Bts_lev());
    } else {
      AIR_ASSERT_MSG(false, "not supported data kind");
    }
  }
}

FUNC_PTR BOOTSTRAP_INSERTER::Insert_bootstrap_clone_func(
    FUNC_ID func, const BTS_POINT& bts_point) {
  using ITER = std::map<BTS_POINT, FUNC_ID>::iterator;
  std::pair<ITER, bool> res =
      _processed_bts_point.emplace(bts_point, FUNC_ID());
  if (!res.second) {
    FUNC_ID new_func = res.first->second;
    AIR_ASSERT(!new_func.Is_null());
    return Glob_scope()->Func(new_func);
  }

  using CORE_HANDLER = air::core::HANDLER<CORE_BTS_INSERTER_IMPL>;
  using CKKS_HANDLER = HANDLER<CKKS_BTS_INSERTER_IMPL>;
  using VISITOR      = VISITOR<BTS_INSERTER_CTX, CORE_HANDLER, CKKS_HANDLER>;
  const SSA_CONTAINER* ssa_cntr = Resbm_ctx()->Region_cntr()->Ssa_cntr(func);

  FUNC_PTR           orig_func       = Glob_scope()->Func(func);
  FUNC_SCOPE*        orig_func_scope = &Glob_scope()->Open_func_scope(func);
  SIGNATURE_TYPE_PTR sig  = orig_func->Entry_point()->Type()->Cast_to_sig();
  const SPOS&        spos = Glob_scope()->Unknown_simple_spos();
  std::string        new_func_name(orig_func->Name()->Char_str());
  new_func_name += "_" + std::to_string(_processed_bts_point.size());
  FUNC_PTR new_func = Glob_scope()->New_func(new_func_name.c_str(), spos);
  Glob_scope()->New_entry_point(sig, new_func, new_func_name.c_str(), spos);
  FUNC_SCOPE* new_func_scope = &Glob_scope()->New_func_scope(new_func);
  new_func_scope->Clone(*orig_func_scope);
  CONTAINER* new_cntr = &new_func_scope->Container();
  new_cntr->New_func_entry(spos);

  BTS_INSERTER_CTX inserter_ctx(bts_point, ssa_cntr, new_cntr, Lower_ctx(),
                                Resbm_ctx());
  VISITOR          visitor(inserter_ctx);
  (void)visitor.Visit<INSERTER_RETV>(ssa_cntr->Container()->Entry_node());

  AIR_ASSERT_MSG(inserter_ctx.Have_bootstraped_all(),
                 "exists not processed bootstrap");

  res.first->second = new_func->Id();
  return new_func;
}

void BOOTSTRAP_INSERTER::Set_param_scale_info(const CALLSITE_INFO& callsite,
                                              NODE_PTR             param,
                                              uint32_t             formal_id) {
  if (!Lower_ctx()->Is_cipher_type(param->Rtype_id()) &&
      !Lower_ctx()->Is_cipher3_type(param->Rtype_id()))
    return;

  FORMAL_SCALE_INFO::const_iterator iter = Formal_scale_info().find(callsite);
  AIR_ASSERT(iter != Formal_scale_info().end());

  FUNC_SCOPE*    func_scope = &Glob_scope()->Open_func_scope(callsite.Callee());
  ADDR_DATUM_PTR formal     = func_scope->Formal(formal_id);
  bool           find_param = false;
  for (const VAR_SCALE_INFO& var_scale_info : iter->second) {
    if (var_scale_info.Var() != formal) continue;
    uint32_t level = var_scale_info.Scale_info().Level() + 1;
    param->Set_attr(Lower_ctx()->Attr_name(core::FHE_ATTR_KIND::LEVEL), &level,
                    1);
    find_param = true;
    break;
  }
  AIR_ASSERT_MSG(find_param, "failed finding target param");
}

void BOOTSTRAP_INSERTER::Insert_bootstrap_points() {
  const REGION_CONTAINER* reg_cntr      = Resbm_ctx()->Region_cntr();
  FUNC_ID                 entry_func_id = reg_cntr->Entry_func();
  CONTAINER*              entry_cntr =
      &Glob_scope()->Open_func_scope(entry_func_id).Container();
  for (auto func_bts_info : _bts_point) {
    air::base::FUNC_ID func_id = func_bts_info.first;
    // 1. insert required bootstrap for entry function
    if (func_id == entry_func_id) {
      Insert_bootstrap_orig_func(func_id, func_bts_info.second.begin()->second);
      continue;
    }

    // 2. clone and insert bootstrap for each called function
    for (auto callsite_info_pair : func_bts_info.second) {
      const CALLSITE_INFO& callsite_info = callsite_info_pair.first;
      FUNC_ID              caller_func   = callsite_info.Caller();
      AIR_ASSERT(caller_func == entry_func_id);
      const DFG_CONTAINER* dfg_cntr = reg_cntr->Dfg_cntr(caller_func);
      NODE_ID  call_node_id = dfg_cntr->Node(callsite_info.Node())->Node_id();
      NODE_PTR call_node    = entry_cntr->Node(call_node_id);
      // clone the called function for current bootstrap plan
      const BTS_POINT& bts_point = callsite_info_pair.second;
      FUNC_PTR new_func = Insert_bootstrap_clone_func(func_id, bts_point);

      // create call stmt of the cloned function, and replace the call stmt.
      CONST_ENTRY_PTR new_func_entry = new_func->Entry_point();
      STMT_PTR        new_call =
          entry_cntr->New_call(new_func_entry, call_node->Ret_preg(),
                               call_node->Num_arg(), call_node->Spos());
      for (uint32_t id = 0; id < call_node->Num_child(); ++id) {
        NODE_PTR child = call_node->Child(id);
        new_call->Node()->Set_child(id, child);
        Set_param_scale_info(callsite_info, child, id);
      }
      new_call->Node()->Copy_attr(call_node);
      new_call->Print();

      STMT_LIST stmt_list(call_node->Stmt()->Parent_node());
      stmt_list.Append(call_node->Stmt(), new_call);
      stmt_list.Remove(call_node->Stmt());
    }
  }
}

void BOOTSTRAP_INSERTER::Perform() {
  Collect_bootstrap_points();
  Insert_bootstrap_points();
}

}  // namespace ckks
}  // namespace fhe