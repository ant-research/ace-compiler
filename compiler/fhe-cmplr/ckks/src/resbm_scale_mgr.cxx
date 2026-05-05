//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "resbm_scale_mgr.h"

#include <stdint.h>

#include "air/base/container.h"
#include "air/base/opcode.h"
#include "air/core/opcode.h"
#include "air/opt/dfg_data.h"
#include "air/opt/scc_container.h"
#include "air/opt/scc_node.h"
#include "air/opt/ssa_container.h"
#include "air/util/debug.h"
#include "ckks_cost_model.h"
#include "dfg_region.h"
#include "dfg_region_builder.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/ckks/config.h"
#include "fhe/core/lower_ctx.h"
#include "min_cut_region.h"
#include "resbm.h"
#include "resbm_ctx.h"
namespace fhe {
namespace ckks {

using namespace air::opt;

static uint32_t Poly_num(const core::LOWER_CTX* lower_ctx,
                         air::base::TYPE_PTR    type) {
  if (type->Is_array()) type = type->Cast_to_arr()->Elem_type();
  if (lower_ctx->Is_cipher_type(type->Id())) return 2;
  if (lower_ctx->Is_cipher3_type(type->Id())) return 3;
  if (lower_ctx->Is_plain_type(type->Id())) return 1;
  return 0;
}

static void Init_scc_trav_state(const SCC_CONTAINER* scc_cntr,
                                const REGION_PTR&    region) {
  for (REGION::ELEM_ITER elem_iter = region->Begin_elem();
       elem_iter != region->End_elem(); ++elem_iter) {
    SCC_NODE_ID  scc_id   = scc_cntr->Scc_node(elem_iter->Id());
    SCC_NODE_PTR scc_node = scc_cntr->Node(scc_id);
    scc_node->Set_trav_state(TRAV_STATE_RAW);
  }
}

double SCALE_MGR::Fhe_op_laten(CONST_REGION_ELEM_PTR elem, uint32_t level) {
  DFG_NODE_PTR dfg_node = elem->Dfg_node();
  if (dfg_node->Is_ssa_ver()) return 0.;

  air::base::OPCODE opc = dfg_node->Node()->Opcode();
  // ignore latency of non-FHE operations
  if (opc.Domain() != CKKS_DOMAIN::ID) return 0.;

  uint32_t poly_deg =
      Region_cntr()->Lower_ctx()->Get_ctx_param().Get_poly_degree();
  double laten = Operation_cost(opc, level, poly_deg) * dfg_node->Freq();

  air::base::NODE_PTR node = dfg_node->Node();
  if (node->Opcode() == OPC_MUL && node->Child(1)->Opcode() == OPC_ENCODE) {
    laten = Operation_cost(OPC_ENCODE, level, poly_deg) * dfg_node->Freq();
  }
  return laten;
}

double SCALE_MGR::Fhe_op_laten(CONST_SCC_NODE_PTR scc_node, uint32_t level) {
  double scc_laten = 0.;
  for (SCC_NODE::ELEM_ITER iter = scc_node->Begin_elem();
       iter != scc_node->End_elem(); ++iter) {
    CONST_REGION_ELEM_PTR elem =
        Region_cntr()->Region_elem(REGION_ELEM_ID(*iter));
    scc_laten += Fhe_op_laten(elem, level);
  }
  return scc_laten;
}

double SCALE_MGR::Rescale_latency(const CUT_TYPE& cut, uint32_t level) {
  double   rescale_laten = 0.;
  uint32_t poly_deg =
      Region_cntr()->Lower_ctx()->Get_ctx_param().Get_poly_degree();
  for (REGION_ELEM_ID elem_id : cut.Cut_elem()) {
    REGION_ELEM_PTR     elem     = Region_cntr()->Region_elem(elem_id);
    DFG_NODE_PTR        dfg_node = elem->Dfg_node();
    air::base::TYPE_PTR data_type =
        Region_cntr()->Glob_scope()->Type(dfg_node->Type());
    uint32_t poly_num = Poly_num(Region_cntr()->Lower_ctx(), data_type);
    rescale_laten += Rescale_cost(level, poly_num, poly_deg) * dfg_node->Freq();
  }

  return rescale_laten;
}

//! @brief Return latency of bootstrap operations inserted at cut.
double SCALE_MGR::Bootstrap_latency(const CUT_TYPE& cut, uint32_t level) {
  double   bootstrap_laten = 0.;
  uint32_t poly_deg =
      Region_cntr()->Lower_ctx()->Get_ctx_param().Get_poly_degree();
  double cost = Operation_cost(OPC_BOOTSTRAP, level, poly_deg);
  for (REGION_ELEM_ID elem_id : cut.Cut_elem()) {
    REGION_ELEM_PTR elem     = Region_cntr()->Region_elem(elem_id);
    DFG_NODE_PTR    dfg_node = elem->Dfg_node();
    bootstrap_laten += cost * dfg_node->Freq();
  }
  return bootstrap_laten;
}

//! @brief Remove redundant bootstraps of which operand level is not less than
//! that of resulting level.
void SCALE_MGR::Remove_redundant_bootstrap(CUT_TYPE&          cut,
                                           const SCALE_LEVEL& scale_info) {
  std::list<REGION_ELEM_PTR> redundant_elem;
  for (REGION_ELEM_ID elem_id : cut.Cut_elem()) {
    REGION_ELEM_PTR elem      = Region_cntr()->Region_elem(elem_id);
    bool            redundant = true;
    // check if bootstrap element is redundant.
    for (uint32_t id = 0; id < elem->Pred_cnt(); ++id) {
      if (elem->Pred_id(id) == air::base::Null_id) continue;

      REGION_ELEM_PTR pred      = elem->Pred(id);
      REGION_ID       region_id = pred->Region_id();
      if (region_id.Value() >= Src_region().Value()) {
        redundant = false;
        break;
      }

      // Check if the operand level is not lower than the resulting bootstrap
      // level.
      const SCALE_LEVEL& pred_scale_info =
          Context()->Scale_level(Src_region(), pred);
      if (pred_scale_info.Level() - pred_scale_info.Scale() <
          scale_info.Level() - scale_info.Scale() + elem->Mul_depth()) {
        redundant = false;
        break;
      }
    }
    if (redundant) redundant_elem.push_back(elem);
  }
  for (CONST_REGION_ELEM_PTR elem : redundant_elem) {
    cut.Erase_elem(elem->Id());

    Context()->Trace(TD_RESBM_SCALE_MGR, "    Remove redundant bootstrap: ");
    Context()->Trace_obj(TD_RESBM_SCALE_MGR, elem);
  }
}

SCALE_LEVEL SCALE_MGR::Handle_src_region() {
  // 1: for the region of entry, no bootstrap or rescale is needed.
  // set all node at scale= 1 and level= bts_lev.
  double      region_laten = 0.;
  SCALE_LEVEL scale_info(1, Used_lev());
  REGION_PTR  region = Region_cntr()->Region(Src_region());
  if (Src_region().Value() == 1) {
    for (REGION::ELEM_ITER elem_iter = region->Begin_elem();
         elem_iter != region->End_elem(); ++elem_iter) {
      Update_scale_level(elem_iter->Id(), scale_info);
      region_laten += Fhe_op_laten(*elem_iter, scale_info.Level());
    }
    Plan()->Set_latency(Src_region(), region_laten);
    return scale_info;
  }

  // 2: for internal regions, bootstrap is needed. Scale/level of the elements
  // reachable from the bootstrap are set as 1 and max_lev.
  // 2.1 perform min_cut to find the nodes need bootstrap
  CUT_TYPE cut = Context()->Min_cut(Src_region(), Used_lev(), MIN_CUT_BTS);
  Remove_redundant_bootstrap(cut, scale_info);
  Plan()->Set_cut(Src_region(), cut);
  region_laten += Bootstrap_latency(cut, scale_info.Level());

  // 2.2 Init traverse state to raw
  const SCC_CONTAINER* scc_cntr = Region_cntr()->Scc_cntr();
  Init_scc_trav_state(scc_cntr, region);

  // 2.3 Collect min_cut nodes which are operand of bootstrap
  std::list<SCC_NODE_PTR> worklist;
  for (REGION_ELEM_ID elem : cut.Cut_elem()) {
    Update_scale_level(elem, scale_info);
    SCC_NODE_ID  scc_id   = scc_cntr->Scc_node(elem);
    SCC_NODE_PTR scc_node = scc_cntr->Node(scc_id);
    scc_node->Set_trav_state(TRAV_STATE_VISITED);
    worklist.push_back(scc_node);
  }

  // 2.4 Set all nodes reachable from bootstrap at scale= 1 and level= bts_lev.
  while (!worklist.empty()) {
    SCC_NODE_PTR scc_node = worklist.back();
    worklist.pop_back();
    scc_node->Set_trav_state(TRAV_STATE_VISITED);

    for (SCC_NODE::SCC_EDGE_ITER edge_iter = scc_node->Begin_succ();
         edge_iter != scc_node->End_succ(); ++edge_iter) {
      SCC_NODE_PTR succ_scc_node = edge_iter->Dst();
      if (!succ_scc_node->At_trav_state(TRAV_STATE_RAW)) continue;

      REGION_ID succ_region = Region_cntr()->Scc_region(succ_scc_node->Id());
      if (succ_region != region->Id()) continue;

      succ_scc_node->Set_trav_state(TRAV_STATE_PROCESSING);
      worklist.push_front(succ_scc_node);

      Update_scale_level(succ_scc_node, scale_info);
      region_laten += Fhe_op_laten(succ_scc_node, scale_info.Level());
    }
  }
  Plan()->Set_latency(Src_region(), region_laten);
  Context()->Trace(TD_RESBM_SCALE_MGR, "    Set latency of src region_",
                   Src_region().Value(), "= ", region_laten, " ms.\n");
  return scale_info;
}

void SCALE_MGR::Set_formal_scale_level(CONST_REGION_ELEM_PTR elem,
                                       const SCALE_LEVEL&    scale_lev) {
  DFG_NODE_PTR dfg_node = elem->Dfg_node();
  if (!dfg_node->Is_ssa_ver()) return;

  const SSA_CONTAINER* ssa_cntr =
      Context()->Region_cntr()->Ssa_cntr(elem->Parent_func());
  SSA_VER_PTR ver = dfg_node->Ssa_ver();
  if (ver->Kind() != VER_DEF_KIND::CHI) return;

  air::base::STMT_PTR def_stmt =
      ssa_cntr->Chi_node(ver->Def_chi_id())->Def_stmt();
  if (def_stmt->Opcode() != air::core::OPC_FUNC_ENTRY) return;

  SSA_SYM_PTR sym = ssa_cntr->Sym(ver->Sym_id());
  if (!sym->Is_addr_datum()) return;

  air::base::ADDR_DATUM_PTR addr_datum = ssa_cntr->Func_scope()->Addr_datum(
      air::base::ADDR_DATUM_ID(sym->Var_id()));
  if (!addr_datum->Is_formal()) return;

  VAR_SCALE_LEV formal_scale_lev(addr_datum, scale_lev);
  Plan()->Set_scale_lev(elem->Callsite_info(), formal_scale_lev);

  Context()->Trace(TD_RESBM_SCALE_MGR, "    Set scale/level of formal: ");
  Context()->Trace_obj(TD_RESBM_SCALE_MGR, elem);
  Context()->Trace_obj(TD_RESBM_SCALE_MGR, &formal_scale_lev);
}

void SCALE_MGR::Update_scale_level(REGION_ELEM_ID     elem_id,
                                   const SCALE_LEVEL& scale_lev) {
  Plan()->Set_scale_lev(elem_id, scale_lev);
  REGION_ELEM_PTR elem = Region_cntr()->Region_elem(elem_id);
  Set_formal_scale_level(elem, scale_lev);

  Context()->Trace(TD_RESBM_SCALE_MGR, "    Set scale/level of ");
  Context()->Trace_obj(TD_RESBM_SCALE_MGR, elem);
  Context()->Trace(TD_RESBM_SCALE_MGR, " as ");
  Context()->Trace_obj(TD_RESBM_SCALE_MGR, &scale_lev);
}

void SCALE_MGR::Update_scale_level(CONST_SCC_NODE_PTR scc_node,
                                   const SCALE_LEVEL& scale_lev) {
  for (SCC_NODE::ELEM_ITER elem_iter = scc_node->Begin_elem();
       elem_iter != scc_node->End_elem(); ++elem_iter) {
    REGION_ELEM_ID elem_id(*elem_iter);
    Update_scale_level(elem_id, scale_lev);
  }
}

void SCALE_MGR::Handle_bypass_edge(REGION_PTR region) {
  for (REGION::ELEM_ITER elem_iter = region->Begin_elem();
       elem_iter != region->End_elem(); ++elem_iter) {
    const SCALE_LEVEL& scale_info = Plan()->Scale_lev(elem_iter->Id());
    if (scale_info.Is_invalid()) continue;

    for (uint32_t id = 0; id < elem_iter->Pred_cnt(); ++id) {
      // skip invalid pred node
      if (elem_iter->Pred_id(id) == air::base::Null_id) continue;

      REGION_ELEM_PTR pred = elem_iter->Pred(id);
      // skip pred from non bypass edge
      if (pred->Region_id().Value() > Src_region().Value()) continue;
      if (pred->Region_id() == Src_region() &&
          Plan()->Scale_lev().find(elem_iter->Id()) !=
              Plan()->Scale_lev().end())
        continue;

      const SCALE_LEVEL& pred_scale_info =
          Context()->Scale_level(Src_region(), pred);
      uint32_t pred_scale = pred_scale_info.Scale();
      uint32_t pred_level = pred_scale_info.Level();
      uint32_t req_scale  = scale_info.Scale();
      uint32_t req_level  = scale_info.Level();
      AIR_ASSERT(pred_scale > 0);
      AIR_ASSERT(pred_level + 1 >= pred_scale);
      AIR_ASSERT(req_scale > 0);
      AIR_ASSERT(req_level + 1 >= req_scale);
      if (elem_iter->Dfg_node()->Is_node() &&
          elem_iter->Dfg_node()->Node()->Opcode() == OPC_MUL &&
          req_scale >= 2) {
        --req_scale;
      } else if (Plan()->Min_cut(region->Id()).Find(elem_iter->Id()) !=
                 Plan()->Min_cut(region->Id()).End()) {
        // for elements requiring rescaling, scale_info is that of the result.
        ++req_level;
      }

      if (pred_level + req_scale >= req_level + pred_scale) continue;

      uint32_t bts_lev = req_level + req_scale - 1;
      Plan()->Set_bts_lev(pred->Id(), bts_lev);

      Context()->Trace(TD_RESBM_SCALE_MGR,
                       "    Bootstrap pred from bypass edge: ");
      Context()->Trace_obj(TD_RESBM_SCALE_MGR, pred);
      Context()->Trace(TD_RESBM_SCALE_MGR, " to [scale=1,level= ", bts_lev,
                       "]\n");
    }
  }
}

double SCALE_MGR::Handle_elem_ahead_rescale(
    const air::opt::SCC_CONTAINER* scc_cntr, const REGION_PTR& region,
    const SCALE_LEVEL& scale_lev) {
  double laten = 0.;
  for (REGION::ELEM_ITER elem_iter = region->Begin_elem();
       elem_iter != region->End_elem(); ++elem_iter) {
    SCC_NODE_ID  scc_id   = scc_cntr->Scc_node(elem_iter->Id());
    SCC_NODE_PTR scc_node = scc_cntr->Node(scc_id);
    if (!scc_node->At_trav_state(TRAV_STATE_RAW)) continue;

    Update_scale_level(elem_iter->Id(), scale_lev);
    laten += Fhe_op_laten(*elem_iter, scale_lev.Level());
  }
  return laten;
}

SCALE_LEVEL SCALE_MGR::Handle_internal_region(REGION_ID          region_id,
                                              const SCALE_LEVEL& scale_lev) {
  // 1. Perform min_cut to find elements need rescale
  const CUT_TYPE& cut =
      Context()->Min_cut(region_id, scale_lev.Level(), MIN_CUT_RESCALE);
  Plan()->Set_cut(region_id, cut);
  double region_laten = Rescale_latency(cut, scale_lev.Level());

  // 2. Init traverse state to raw and set all nodes at input scale and level
  REGION_PTR           region   = Region_cntr()->Region(region_id);
  const SCC_CONTAINER* scc_cntr = Region_cntr()->Scc_cntr();
  Init_scc_trav_state(scc_cntr, region);

  // 3. Collect min_cut nodes which are operand of rescale
  // scale and level after rescale
  SCALE_LEVEL rescale_scale_lev(scale_lev.Scale(), scale_lev.Level() - 1);
  // scale and level after mul at the region beginning
  SCALE_LEVEL mul_scale_lev(scale_lev.Scale() + 1, scale_lev.Level());
  std::list<SCC_NODE_PTR> worklist;
  for (REGION_ELEM_ID elem_id : cut.Cut_elem()) {
    SCC_NODE_ID  scc_id   = scc_cntr->Scc_node(elem_id);
    SCC_NODE_PTR scc_node = scc_cntr->Node(scc_id);
    if (scc_node->Elem_cnt() > 1) {
      Update_scale_level(scc_node, mul_scale_lev);
    }

    Update_scale_level(elem_id, rescale_scale_lev);
    REGION_ELEM_PTR elem = Region_cntr()->Region_elem(elem_id);
    region_laten += Fhe_op_laten(elem, scale_lev.Level());

    scc_node->Set_trav_state(TRAV_STATE_VISITED);
    worklist.push_back(scc_node);
  }

  // 4. Calculate the elements reachable from rescale, set their scale and level
  // to the rescale result, and determine their latency.
  while (!worklist.empty()) {
    SCC_NODE_PTR scc_node = worklist.back();
    worklist.pop_back();
    for (SCC_NODE::SCC_EDGE_ITER edge_iter = scc_node->Begin_succ();
         edge_iter != scc_node->End_succ(); ++edge_iter) {
      SCC_NODE_PTR succ_scc_node = edge_iter->Dst();
      if (succ_scc_node->Trav_state() == TRAV_STATE_VISITED) continue;

      REGION_ID succ_region = Region_cntr()->Scc_region(succ_scc_node->Id());
      if (succ_region != region->Id()) continue;
      succ_scc_node->Set_trav_state(TRAV_STATE_VISITED);
      worklist.push_front(succ_scc_node);

      Update_scale_level(succ_scc_node, rescale_scale_lev);
      region_laten += Fhe_op_laten(succ_scc_node, rescale_scale_lev.Level());
    }
  }

  // 5. Handle elements ahead of rescale.
  region_laten += Handle_elem_ahead_rescale(scc_cntr, region, mul_scale_lev);
  Plan()->Set_latency(region_id, region_laten);

  Context()->Trace(TD_RESBM_SCALE_MGR, "    Set latency of region_",
                   region_id.Value(), "= ", region_laten, " ms.\n");
  // 6. handle bypass edge
  Handle_bypass_edge(region);
  return rescale_scale_lev;
}

ELEM_BTS_INFO SCALE_MGR::Redundant_bootstrap(
    REGION_ELEM_PTR elem, const MIN_LATENCY_PLAN& plan) const {
  // skip element contains multiplication
  if (elem->Mul_depth() > 0) return ELEM_BTS_INFO();
  if (elem->Succ_cnt() > 1) {
    SCC_NODE_ID scc_node_id = Region_cntr()->Scc_cntr()->Scc_node(elem->Id());
    CONST_SCC_NODE_PTR scc_node = Region_cntr()->Scc_cntr()->Node(scc_node_id);
    if (scc_node->Succ_cnt() > 1) return ELEM_BTS_INFO();
  }
  if (plan.Bootstrap_point().Find(elem->Id()) != plan.Bootstrap_point().End()) {
    return ELEM_BTS_INFO(elem, plan.Used_level());
  }

  if (elem->Pred_cnt() > 1) return ELEM_BTS_INFO();
  if (elem->Pred_id(0) == air::base::Null_id) return ELEM_BTS_INFO();
  REGION_ELEM_PTR pred = elem->Pred(0);
  if (pred->Region_id() != elem->Region_id()) return ELEM_BTS_INFO();

  return Redundant_bootstrap(pred, plan);
}

void SCALE_MGR::Collect_redundant_bootstrap(REGION_ELEM_PTR elem) {
  for (uint32_t id = 0; id < elem->Pred_cnt(); ++id) {
    REGION_ELEM_ID pred_id = elem->Pred_id(id);
    if (pred_id == air::base::Null_id) continue;
    REGION_ELEM_PTR   pred = Region_cntr()->Region_elem(pred_id);
    MIN_LATENCY_PLAN* plan = Plan();
    while (plan != nullptr &&
           pred->Region_id().Value() <= plan->Src_id().Value()) {
      if (pred->Region_id() == plan->Src_id()) {
        ELEM_BTS_INFO redund_bts = Redundant_bootstrap(pred, *plan);
        if (!redund_bts.Is_invalid()) {
          REGION_ELEM_PTR bts_elem = redund_bts.Region_elem();
          if (bts_elem->Region_id() == Src_region()) {
            plan->Bootstrap_point().Cut_elem().erase(bts_elem->Id());
            double   pre_latency = plan->Latency(Src_region());
            uint32_t poly_deg =
                Region_cntr()->Lower_ctx()->Get_ctx_param().Get_poly_degree();
            double bts_cost =
                Operation_cost(OPC_BOOTSTRAP, redund_bts.Bts_lev(), poly_deg);
            double new_latency = pre_latency - bts_cost;
            plan->Set_latency(Src_region(), new_latency);
            Context()->Trace(TD_RESBM_SCALE_MGR,
                             "    Remove redundant bootstrap: ");
            Context()->Trace_obj(TD_RESBM_SCALE_MGR, bts_elem);
          } else {
            plan->Add_redundant_bootstrap(redund_bts);
            Context()->Trace(TD_RESBM_SCALE_MGR, "Find redundant bts point: ");
            Context()->Trace_obj(TD_RESBM_SCALE_MGR, &redund_bts);
          }
        }
      }
      plan = Context()->Plan(plan->Src_id());
      if (plan == nullptr) break;
    }
  }
}

void SCALE_MGR::Collect_redundant_bootstrap(CONST_SCC_NODE_PTR scc_node) {
  if (scc_node->Elem_cnt() > 1) return;
  REGION_ELEM_ID id(*scc_node->Begin_elem());
  Collect_redundant_bootstrap(Region_cntr()->Region_elem(id));
}

void SCALE_MGR::Handle_dst_region(const SCALE_LEVEL& scale_lev) {
  // 1. Min_cut for rescale points
  AIR_ASSERT(scale_lev.Scale() == scale_lev.Level());
  const CUT_TYPE& rs_cut = Context()->Min_cut(Dst_region(), 1, MIN_CUT_RESCALE);
  Plan()->Set_cut(Dst_region(), rs_cut);
  double region_laten = Rescale_latency(rs_cut, scale_lev.Level());

  // 2. Min_cut for bootstrap points
  const CUT_TYPE& bts_cut =
      Context()->Min_cut(Dst_region(), Context()->Max_bts_lvl(), MIN_CUT_BTS);

  // 3. Init traverse state to raw and set all nodes at input scale and level
  REGION_PTR           region   = Region_cntr()->Region(Dst_region());
  const SCC_CONTAINER* scc_cntr = Region_cntr()->Scc_cntr();
  Init_scc_trav_state(scc_cntr, region);

  // 4. Handle rescale operands
  std::list<SCC_NODE_PTR> worklist;
  SCALE_LEVEL mul_scale_lev(scale_lev.Scale() + 1, scale_lev.Level());
  SCALE_LEVEL rescale_scale_lev(scale_lev.Scale(), scale_lev.Level() - 1);
  for (REGION_ELEM_ID elem_id : rs_cut.Cut_elem()) {
    SCC_NODE_ID  scc_id   = scc_cntr->Scc_node(elem_id);
    SCC_NODE_PTR scc_node = scc_cntr->Node(scc_id);
    scc_node->Set_trav_state(TRAV_STATE_PROCESSING);
    worklist.push_back(scc_node);

    Update_scale_level(scc_node, mul_scale_lev);
    Update_scale_level(elem_id, rescale_scale_lev);
    REGION_ELEM_PTR elem = Region_cntr()->Region_elem(elem_id);
    region_laten += Fhe_op_laten(elem, scale_lev.Level());
  }

  // 5. Handle bootstrap operands
  for (REGION_ELEM_ID elem : bts_cut.Cut_elem()) {
    SCC_NODE_ID  scc_id   = scc_cntr->Scc_node(elem);
    SCC_NODE_PTR scc_node = scc_cntr->Node(scc_id);
    uint32_t     level    = (scc_node->At_trav_state(TRAV_STATE_PROCESSING)
                                 ? scale_lev.Level()
                                 : rescale_scale_lev.Level());
    region_laten += Fhe_op_laten(scc_node, level);
    if (scc_node->At_trav_state(TRAV_STATE_RAW)) {
      Collect_redundant_bootstrap(scc_node);
    }
    scc_node->Set_trav_state(TRAV_STATE_VISITED);
  }

  // 6. Set scale/level of elements between rescale and bootstrap to rescale
  // result.
  while (!worklist.empty()) {
    SCC_NODE_PTR scc_node = worklist.back();
    worklist.pop_back();

    for (SCC_NODE::SCC_EDGE_ITER edge_iter = scc_node->Begin_succ();
         edge_iter != scc_node->End_succ(); ++edge_iter) {
      SCC_NODE_PTR succ_scc_node = edge_iter->Dst();
      REGION_ID    succ_region = Region_cntr()->Scc_region(succ_scc_node->Id());
      if (succ_region != region->Id()) continue;

      worklist.push_front(succ_scc_node);
      if (!succ_scc_node->At_trav_state(TRAV_STATE_VISITED)) {
        succ_scc_node->Set_trav_state(scc_node->Trav_state());
      }
      if (succ_scc_node->At_trav_state(TRAV_STATE_PROCESSING)) {
        Update_scale_level(succ_scc_node, rescale_scale_lev);
        region_laten += Fhe_op_laten(succ_scc_node, rescale_scale_lev.Level());
        Collect_redundant_bootstrap(succ_scc_node);
      }
    }
  }

  // 7. Set scale/level of elements ahead of rescale to multiplication result.
  region_laten += Handle_elem_ahead_rescale(scc_cntr, region, mul_scale_lev);
  Plan()->Set_latency(Dst_region(), region_laten);
  Context()->Trace(TD_RESBM_SCALE_MGR, "    Set latency of dst region_",
                   region->Id().Value(), "= ", region_laten, " ms.\n");
  // 8. handle bypass edge.
  Handle_bypass_edge(region);
}

MIN_LATENCY_PLAN* SCALE_MGR::Perform() {
  Context()->Trace(TD_RESBM_SCALE_MGR, "\n>>>>>>SCALEMGR: [",
                   Src_region().Value(), ", ", Dst_region().Value(), "]\n");
  if (Used_lev() > Max_lev()) {
    Context()->Release(Plan());
    Context()->Trace(TD_RESBM_SCALE_MGR, "required_level=", Used_lev(), " > ",
                     " max_lev=", Max_lev());
    return nullptr;
  }
  Plan()->Set_used_level(Used_lev());
  // 1. compute scale and level of elements in src region
  SCALE_LEVEL scale_info = Handle_src_region();

  // 2. compute scale and level of elements in internal regions
  for (uint32_t id = Src_region().Value() + 1; id < Dst_region().Value();
       ++id) {
    scale_info = Handle_internal_region(REGION_ID(id), scale_info);
  }

  // 3. compute scale and level of elements in dst region
  Handle_dst_region(scale_info);

  Context()->Trace(TD_RESBM_SCALE_MGR, "\n<<<<<< End SCALEMGR: [",
                   Src_region().Value(), ", ", Dst_region().Value(), "]\n");
  return Plan();
}

}  // namespace ckks
}  // namespace fhe
