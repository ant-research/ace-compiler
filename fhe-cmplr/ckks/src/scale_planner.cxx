//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "scale_planner.h"

#include <cfloat>
#include <list>
#include <memory>
#include <string>

#include "air/base/container.h"
#include "air/base/container_decl.h"
#include "air/base/id_wrapper.h"
#include "air/base/opcode.h"
#include "air/base/st_decl.h"
#include "air/core/opcode.h"
#include "air/opt/dfg_data.h"
#include "air/opt/scc_container.h"
#include "air/opt/scc_node.h"
#include "air/opt/ssa_container.h"
#include "air/opt/ssa_decl.h"
#include "air/util/debug.h"
#include "ckks_cost_model.h"
#include "dfg_region.h"
#include "dfg_region_builder.h"
#include "fhe/ckks/ckks_opcode.h"
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

double SCALE_PLANNER::Region_elem_latency(CONST_REGION_ELEM_PTR elem,
                                          uint32_t              level) {
  DFG_NODE_PTR dfg_node = elem->Dfg_node();
  if (dfg_node->Is_ssa_ver()) return 0.;
  air::base::OPCODE opc = dfg_node->Node()->Opcode();
  // ignore latency of non-FHE operations
  if (opc.Domain() != CKKS_DOMAIN::ID) return 0.;

  double laten = Operation_cost(opc, level) * dfg_node->Freq();

  air::base::NODE_PTR node = dfg_node->Node();
  if (node->Opcode() == OPC_MUL && node->Child(1)->Opcode() == OPC_ENCODE) {
    laten = Operation_cost(OPC_ENCODE, level) * dfg_node->Freq();
  }
  return laten;
}

double SCALE_PLANNER::Scc_node_latency(CONST_SCC_NODE_PTR scc_node,
                                       uint32_t           level) {
  double scc_laten = 0.;
  for (SCC_NODE::ELEM_ITER iter = scc_node->Begin_elem();
       iter != scc_node->End_elem(); ++iter) {
    CONST_REGION_ELEM_PTR elem =
        Region_cntr()->Region_elem(REGION_ELEM_ID(*iter));
    scc_laten += Region_elem_latency(elem, level);
  }
  return scc_laten;
}

double SCALE_PLANNER::Rescale_latency(const MIN_CUT::CUT_TYPE& cut,
                                      uint32_t                 level) {
  double rescale_laten = 0.;
  for (REGION_ELEM_ID elem_id : cut) {
    REGION_ELEM_PTR     elem     = Region_cntr()->Region_elem(elem_id);
    DFG_NODE_PTR        dfg_node = elem->Dfg_node();
    air::base::TYPE_PTR data_type =
        Region_cntr()->Glob_scope()->Type(dfg_node->Type());
    uint32_t poly_num = Poly_num(Region_cntr()->Lower_ctx(), data_type);
    rescale_laten += Rescale_cost(level, poly_num) * dfg_node->Freq();
  }

  return rescale_laten;
}

double SCALE_PLANNER::Bootstrap_latency(const MIN_CUT::CUT_TYPE& cut,
                                        uint32_t                 level) {
  double bootstrap_laten = 0.;
  double cost            = Operation_cost(OPC_BOOTSTRAP, level);
  for (REGION_ELEM_ID elem_id : cut) {
    REGION_ELEM_PTR elem     = Region_cntr()->Region_elem(elem_id);
    DFG_NODE_PTR    dfg_node = elem->Dfg_node();
    bootstrap_laten += cost * dfg_node->Freq();
  }
  return bootstrap_laten;
}

void SCALE_PLANNER::Remove_redundant_bootstrap(MIN_CUT::CUT_TYPE& cut,
                                               const SCALE_INFO&  scale_info) {
  std::list<REGION_ELEM_PTR> redundant_elem;
  for (REGION_ELEM_ID elem_id : cut) {
    REGION_ELEM_PTR elem      = Region_cntr()->Region_elem(elem_id);
    bool            redundant = true;
    for (uint32_t id = 0; id < elem->Pred_cnt(); ++id) {
      if (elem->Pred_id(id) == air::base::Null_id) continue;

      REGION_ELEM_PTR pred      = elem->Pred(id);
      REGION_ID       region_id = pred->Region();
      if (region_id.Value() >= Start_region().Value()) {
        redundant = false;
        break;
      }
      const SCALE_INFO& pred_scale_info =
          Context()->Scale_info(Start_region(), pred);
      if (pred_scale_info.Level() - pred_scale_info.Scale() <
          scale_info.Level() - scale_info.Scale() + elem->Mul_depth()) {
        redundant = false;
        break;
      }
    }
    if (redundant) redundant_elem.push_back(elem);
  }
  for (REGION_ELEM_PTR elem : redundant_elem) {
    cut.erase(elem->Id());
    std::cout << "Remove redundant bootstrap at " << elem->To_str()
              << std::endl;
  }
}

MIN_LATENCY_PLAN::SCALE_INFO SCALE_PLANNER::Handle_start_region() {
  // 1: for the region of entry, no bootstrap or rescale is needed.
  // set all node at scale= 1 and level= bts_lev.
  Plan()->Set_consume_level(Max_lev());
  double     region_laten = 0.;
  SCALE_INFO scale_info(1, Max_lev());
  REGION_PTR region = Region_cntr()->Region(Start_region());
  if (Start_region().Value() == 1) {
    for (REGION::ELEM_ITER elem_iter = region->Begin_elem();
         elem_iter != region->End_elem(); ++elem_iter) {
      Set_region_elem_scale_info(elem_iter->Id(), scale_info);
      region_laten += Region_elem_latency(*elem_iter, scale_info.Level());
    }
    Plan()->Set_region_latency(Start_region(), region_laten);
    return scale_info;
  }

  // 2: for internal regions, bootstrap is need. Scale and level of the nodes
  // reachable from the bootstrap results are set as 1 and max_lev.
  // 2.1 perform min_cut to find the nodes need bootstrap
  MIN_CUT min_cut(Region_cntr(), Start_region(), Max_lev(), MIN_CUT_BTS);
  min_cut.Perform();
  MIN_CUT::CUT_TYPE& cut = min_cut.Min_cut();
  Remove_redundant_bootstrap(cut, scale_info);

  region_laten += Bootstrap_latency(cut, scale_info.Level());
  Plan()->Set_cut(Start_region(), cut);

  // 2.2 Init traverse state to raw
  const SCC_CONTAINER* scc_cntr = Region_cntr()->Scc_cntr();
  for (REGION::ELEM_ITER elem_iter = region->Begin_elem();
       elem_iter != region->End_elem(); ++elem_iter) {
    SCC_NODE_ID  scc_id   = scc_cntr->Scc_node(elem_iter->Id());
    SCC_NODE_PTR scc_node = Region_cntr()->Scc_cntr()->Node(scc_id);
    scc_node->Set_trav_state(TRAV_STATE_RAW);
  }

  // 2.3 Collect min_cut nodes which are operand of bootstrap
  std::list<SCC_NODE_PTR> worklist;
  for (REGION_ELEM_ID elem : min_cut.Min_cut()) {
    Set_region_elem_scale_info(elem, scale_info);

    SCC_NODE_ID  scc_id   = scc_cntr->Scc_node(elem);
    SCC_NODE_PTR scc_node = Region_cntr()->Scc_cntr()->Node(scc_id);
    scc_node->Set_trav_state(TRAV_STATE_VISITED);
    worklist.push_back(scc_node);
  }

  // 2.4 Set all nodes reachable from bootstrap at scale= 1 and level = bts_lev.
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

      Set_scc_node_scale_info(succ_scc_node, scale_info);
      region_laten += Scc_node_latency(succ_scc_node, scale_info.Level());
    }
  }
  Plan()->Set_region_latency(Start_region(), region_laten);
  return scale_info;
}

void SCALE_PLANNER::Set_formal_scale_info(CONST_REGION_ELEM_PTR elem,
                                          const SCALE_INFO&     scale_info) {
  DFG_NODE_PTR dfg_node = elem->Dfg_node();
  if (!dfg_node->Is_ssa_ver()) return;

  SSA_VER_PTR ver = dfg_node->Ssa_ver();
  if (ver->Version() != SSA_VER::NO_VER) return;

  const SSA_CONTAINER* ssa_cntr =
      Context()->Region_cntr()->Ssa_cntr(elem->Callee());
  SSA_SYM_PTR sym = ssa_cntr->Sym(ver->Sym_id());
  if (!sym->Is_addr_datum()) return;

  air::base::ADDR_DATUM_PTR addr_datum = ssa_cntr->Func_scope()->Addr_datum(
      air::base::ADDR_DATUM_ID(sym->Var_id()));
  if (!addr_datum->Is_formal()) return;

  MIN_LATENCY_PLAN::VAR_SCALE_INFO formal_scale_info(addr_datum, scale_info);
  Plan()->Set_formal_scale_info(elem->Callsite_info(), formal_scale_info);
  formal_scale_info.Print();
}

void SCALE_PLANNER::Set_region_elem_scale_info(REGION_ELEM_ID    elem_id,
                                               const SCALE_INFO& scale_info) {
  Plan()->Set_elem_scale_info(elem_id, scale_info);
  REGION_ELEM_PTR elem = Region_cntr()->Region_elem(elem_id);
  Set_formal_scale_info(elem, scale_info);
  Context()->Trace("    Set scale/level of ", elem->To_str(), " as ",
                   scale_info.To_str(), "\n");
}

void SCALE_PLANNER::Set_scc_node_scale_info(CONST_SCC_NODE_PTR scc_node,
                                            const SCALE_INFO&  scale_info) {
  for (SCC_NODE::ELEM_ITER elem_iter = scc_node->Begin_elem();
       elem_iter != scc_node->End_elem(); ++elem_iter) {
    REGION_ELEM_ID elem_id(*elem_iter);
    Set_region_elem_scale_info(elem_id, scale_info);
  }
}

void SCALE_PLANNER::Handle_bypass_edge(REGION_PTR region) {
  for (REGION::ELEM_ITER elem_iter = region->Begin_elem();
       elem_iter != region->End_elem(); ++elem_iter) {
    const SCALE_INFO& scale_info = Plan()->Scale_info(elem_iter->Id());
    for (uint32_t id = 0; id < elem_iter->Pred_cnt(); ++id) {
      // skip invalid pred node
      if (elem_iter->Pred_id(id) == air::base::Null_id) continue;

      REGION_ELEM_PTR pred = elem_iter->Pred(id);
      // skip pred from non bypass edge
      if (pred->Region().Value() > Start_region().Value()) continue;
      if (pred->Region() == Start_region() &&
          Plan()->Scale_info().find(elem_iter->Id()) !=
              Plan()->Scale_info().end())
        continue;

      const SCALE_INFO& pred_scale_info =
          Context()->Scale_info(Start_region(), pred);
      uint32_t pred_scale = pred_scale_info.Scale();
      uint32_t pred_level = pred_scale_info.Level();
      if (pred_level + scale_info.Scale() >= scale_info.Level() + pred_scale)
        continue;

      uint32_t bts_lev = scale_info.Level() + scale_info.Scale() - 1;
      Plan()->Set_bts_lev(pred->Id(), bts_lev);
      Context()->Trace("In processing [", Start_region().Value(), ", ",
                       End_region().Value(), "], increase level of ",
                       pred->To_str(), " from ", pred_scale_info.To_str(),
                       " to [scale=1,level= ", bts_lev, "]\n");
    }
  }
}

MIN_LATENCY_PLAN::SCALE_INFO SCALE_PLANNER::Handle_internal_region(
    REGION_ID region_id, const SCALE_INFO& scale_info) {
  // 1. perform min_cut to find the nodes need rescale
  MIN_CUT min_cut(Region_cntr(), region_id, Max_lev(), MIN_CUT_RESCALE);
  min_cut.Perform();
  Plan()->Set_cut(region_id, min_cut.Min_cut());
  double region_laten = Rescale_latency(min_cut.Min_cut(), scale_info.Level());

  // 2. Init traverse state to raw and set all nodes at input scale and level
  REGION_PTR           region   = Region_cntr()->Region(region_id);
  const SCC_CONTAINER* scc_cntr = Region_cntr()->Scc_cntr();
  for (REGION::ELEM_ITER elem_iter = region->Begin_elem();
       elem_iter != region->End_elem(); ++elem_iter) {
    SCC_NODE_ID  scc_id   = scc_cntr->Scc_node(elem_iter->Id());
    SCC_NODE_PTR scc_node = Region_cntr()->Scc_cntr()->Node(scc_id);
    scc_node->Set_trav_state(TRAV_STATE_RAW);
  }

  // 3. Collect min_cut nodes which are operand of rescale
  SCALE_INFO rescale_res_scale_info(scale_info.Scale(), scale_info.Level() - 1);
  std::list<SCC_NODE_PTR> worklist;
  SCALE_INFO mul_res_scale_info(scale_info.Scale() + 1, scale_info.Level());
  for (REGION_ELEM_ID elem : min_cut.Min_cut()) {
    SCC_NODE_ID  scc_id   = scc_cntr->Scc_node(elem);
    SCC_NODE_PTR scc_node = Region_cntr()->Scc_cntr()->Node(scc_id);

    Set_scc_node_scale_info(scc_node, mul_res_scale_info);
    Set_region_elem_scale_info(elem, rescale_res_scale_info);
    region_laten += Region_elem_latency(Region_cntr()->Region_elem(elem),
                                        scale_info.Level());

    scc_node->Set_trav_state(TRAV_STATE_VISITED);
    worklist.push_back(scc_node);
  }

  // 4. Set all nodes reachable from rescale at decreased scale and level.
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

      Set_scc_node_scale_info(succ_scc_node, rescale_res_scale_info);
      region_laten +=
          Scc_node_latency(succ_scc_node, rescale_res_scale_info.Level());
    }
  }

  for (REGION::ELEM_ITER elem_iter = region->Begin_elem();
       elem_iter != region->End_elem(); ++elem_iter) {
    SCC_NODE_ID  scc_id   = scc_cntr->Scc_node(elem_iter->Id());
    SCC_NODE_PTR scc_node = Region_cntr()->Scc_cntr()->Node(scc_id);
    if (!scc_node->At_trav_state(TRAV_STATE_RAW)) continue;

    Set_region_elem_scale_info(elem_iter->Id(), mul_res_scale_info);
    region_laten += Region_elem_latency(*elem_iter, mul_res_scale_info.Level());
  }
  Plan()->Set_region_latency(region_id, region_laten);
  // 5. handle bypass edge
  Handle_bypass_edge(region);
  return rescale_res_scale_info;
}

void SCALE_PLANNER::Handle_end_region(const SCALE_INFO& scale_info) {
  AIR_ASSERT(scale_info.Scale() == scale_info.Level());
  MIN_CUT min_cut_rs(Region_cntr(), End_region(), 1, MIN_CUT_RESCALE);
  min_cut_rs.Perform();
  Plan()->Set_cut(End_region(), min_cut_rs.Min_cut());
  double region_laten =
      Rescale_latency(min_cut_rs.Min_cut(), scale_info.Level());

  MIN_CUT min_cut_bts(Region_cntr(), End_region(), Context()->Max_bts_lev(),
                      MIN_CUT_BTS);
  min_cut_bts.Perform();

  // 2. Init traverse state to raw and set all nodes at input scale and level
  REGION_PTR           region   = Region_cntr()->Region(End_region());
  const SCC_CONTAINER* scc_cntr = Region_cntr()->Scc_cntr();
  for (REGION::ELEM_ITER elem_iter = region->Begin_elem();
       elem_iter != region->End_elem(); ++elem_iter) {
    SCC_NODE_ID  scc_id   = scc_cntr->Scc_node(elem_iter->Id());
    SCC_NODE_PTR scc_node = Region_cntr()->Scc_cntr()->Node(scc_id);
    scc_node->Set_trav_state(TRAV_STATE_RAW);
  }

  // 3. Collect min_cut nodes which are operand of rescale
  std::list<SCC_NODE_PTR> worklist;
  SCALE_INFO mul_res_scale_info(scale_info.Scale() + 1, scale_info.Level());
  SCALE_INFO rescale_res_scale_info(scale_info.Scale(), scale_info.Level() - 1);
  for (REGION_ELEM_ID elem : min_cut_rs.Min_cut()) {
    SCC_NODE_ID  scc_id   = scc_cntr->Scc_node(elem);
    SCC_NODE_PTR scc_node = Region_cntr()->Scc_cntr()->Node(scc_id);
    scc_node->Set_trav_state(TRAV_STATE_PROCESSING);
    worklist.push_back(scc_node);

    Set_scc_node_scale_info(scc_node, mul_res_scale_info);
    Set_region_elem_scale_info(elem, rescale_res_scale_info);
    region_laten += Region_elem_latency(Region_cntr()->Region_elem(elem),
                                        scale_info.Level());
  }

  for (REGION_ELEM_ID elem : min_cut_bts.Min_cut()) {
    SCC_NODE_ID  scc_id   = scc_cntr->Scc_node(elem);
    SCC_NODE_PTR scc_node = Region_cntr()->Scc_cntr()->Node(scc_id);
    uint32_t     level    = (scc_node->At_trav_state(TRAV_STATE_PROCESSING)
                                 ? scale_info.Level()
                                 : rescale_res_scale_info.Level());
    region_laten += Scc_node_latency(scc_node, level);

    scc_node->Set_trav_state(TRAV_STATE_VISITED);
  }

  // 4. Set all nodes reachable from rescale and ahead of bootstrap at decreased
  // scale and level.
  while (!worklist.empty()) {
    SCC_NODE_PTR scc_node = worklist.back();
    worklist.pop_back();

    for (SCC_NODE::SCC_EDGE_ITER edge_iter = scc_node->Begin_succ();
         edge_iter != scc_node->End_succ(); ++edge_iter) {
      SCC_NODE_PTR succ_scc_node = edge_iter->Dst();
      // if (!succ_scc_node->At_trav_state(TRAV_STATE_RAW)) continue;
      REGION_ID succ_region = Region_cntr()->Scc_region(succ_scc_node->Id());
      if (succ_region != region->Id()) continue;

      worklist.push_front(succ_scc_node);
      if (!succ_scc_node->At_trav_state(TRAV_STATE_PROCESSING)) {
        succ_scc_node->Set_trav_state(scc_node->Trav_state());
      }
      if (succ_scc_node->At_trav_state(TRAV_STATE_PROCESSING)) {
        Set_scc_node_scale_info(succ_scc_node, rescale_res_scale_info);
        region_laten +=
            Scc_node_latency(succ_scc_node, rescale_res_scale_info.Level());
      }
    }
  }

  for (REGION::ELEM_ITER elem_iter = region->Begin_elem();
       elem_iter != region->End_elem(); ++elem_iter) {
    SCC_NODE_ID  scc_node_id = scc_cntr->Scc_node(elem_iter->Id());
    SCC_NODE_PTR scc_node    = scc_cntr->Node(scc_node_id);
    // current node is reachable from rescale/bootstrap
    if (!scc_node->At_trav_state(TRAV_STATE_RAW)) continue;
    Set_scc_node_scale_info(scc_node, mul_res_scale_info);
    region_laten += Scc_node_latency(scc_node, mul_res_scale_info.Level());
  }
  Plan()->Set_region_latency(End_region(), region_laten);

  // 5. handle bypass edge
  for (REGION::ELEM_ITER elem_iter = region->Begin_elem();
       elem_iter != region->End_elem(); ++elem_iter) {
    SCC_NODE_ID scc_node_id = scc_cntr->Scc_node(elem_iter->Id());
    // current node is reachable from bootstrap
    if (scc_cntr->Node(scc_node_id)->At_trav_state(TRAV_STATE_VISITED))
      continue;

    const SCALE_INFO& scale_info = Plan()->Scale_info(elem_iter->Id());
    for (uint32_t id = 0; id < elem_iter->Pred_cnt(); ++id) {
      // skip invalid pred node
      if (elem_iter->Pred_id(id) == air::base::Null_id) continue;

      REGION_ELEM_PTR pred = elem_iter->Pred(id);
      // skip pred from non bypass edge
      if (pred->Region().Value() > Start_region().Value()) continue;
      if (pred->Region() == Start_region() &&
          Plan()->Scale_info().find(elem_iter->Id()) !=
              Plan()->Scale_info().end())
        continue;

      const SCALE_INFO& pred_scale_info =
          Context()->Scale_info(Start_region(), pred);
      uint32_t pred_scale = pred_scale_info.Scale();
      uint32_t pred_level = pred_scale_info.Level();
      if (pred_level + scale_info.Scale() >= scale_info.Level() + pred_scale)
        continue;

      uint32_t bts_lev = scale_info.Level() + scale_info.Scale() - 1;
      Plan()->Set_bts_lev(pred->Id(), bts_lev);
      Context()->Trace("In processing [", Start_region().Value(), ", ",
                       End_region().Value(), "], increase level of ",
                       pred->To_str(), " from ", pred_scale_info.To_str(),
                       " to [scale=1,level= ", bts_lev, "]\n");
    }
  }
}

bool SCALE_PLANNER::Perform() {
  Context()->Trace("\n>>>>>> Start processing region: [",
                   Start_region().Value(), ", ", End_region().Value(), "]\n");
  // 1. compute scale and level of nodes in the start region
  SCALE_INFO scale_info = Handle_start_region();

  // 2. compute scale and level of nodes in internal regions
  uint32_t id = Start_region().Value() + 1;
  for (; id < End_region().Value(); ++id) {
    scale_info = Handle_internal_region(REGION_ID(id), scale_info);
  }

  // 3. compute scale and level of nodes in the end region
  Handle_end_region(scale_info);

  Context()->Trace("\n<<<<<< End processing region: [", Start_region().Value(),
                   ", ", End_region().Value(), "]\n");
  return true;
}

}  // namespace ckks
}  // namespace fhe