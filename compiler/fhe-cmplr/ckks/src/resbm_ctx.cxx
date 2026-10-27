//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "resbm_ctx.h"

#include <cfloat>
#include <string>

#include "air/base/id_wrapper.h"
#include "air/opt/dfg_data.h"
#include "air/util/debug.h"
#include "ckks_cost_model.h"
#include "dfg_region.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/ckks/config.h"
#include "min_cut_region.h"
namespace fhe {
namespace ckks {

const CUT_TYPE& RESBM_CTX::Min_cut(REGION_ID region, uint32_t level,
                                   MIN_CUT_KIND kind) {
  REGION_CUT& region_cut =
      (kind == MIN_CUT_RESCALE ? Rescale_cut() : Bootstrap_cut());
  if (region_cut.size() <= region.Value()) {
    region_cut.resize(region.Value() + 1);
  }
  LEVEL2CUT& lev2cut = region_cut[region.Value()];
  if (lev2cut.size() <= level) {
    lev2cut.resize(
        level + 1,
        CUT_TYPE(Region_cntr(), std::set<REGION_ELEM_ID>(), INVALID_LATENCY));
  }

  CUT_TYPE& cut = lev2cut[level];
  if (cut.Empty()) {
    // no saved result found. Perform min_cut and record the result.
    if (Eva_waterline() && kind == MIN_CUT_RESCALE) {
      // EVA waterline rescaling always insert rescaling after CKKS.mul.
      REGION_PTR region_ptr = Region_cntr()->Region(region);
      for (REGION::ELEM_ITER iter = region_ptr->Begin_elem();
           iter != region_ptr->End_elem(); ++iter) {
        REGION_ELEM_PTR        elem     = *iter;
        air::opt::DFG_NODE_PTR dfg_node = elem->Dfg_node();
        if (!dfg_node->Is_node()) continue;
        if (dfg_node->Node()->Opcode() != OPC_MUL) continue;
        cut.Add_elem(elem->Id());
      }
      cut.Set_cost_incr(0.);
    } else {
      MIN_CUT_REGION min_cut(Config(), Driver_ctx(), Region_cntr(), region,
                             level, kind);
      min_cut.Perform();
      cut = min_cut.Min_cut();
    }
    Trace(TD_RESBM, "Min cut region ", region.Value(), " for ", kind,
          " at level= ", level, ":\n");
    Trace_obj(TD_RESBM, &cut);
  }
  return cut;
}

void RESBM_CTX::Print(std::ostream& os, uint32_t indent,
                      REGION_ID region) const {
  os << "SCALE_MNG_PLAN: [1," << region.Value() << "]>>>>>>>>>>>>>>>>>>>>>>>>"
     << std::endl;
  MIN_LATENCY_PLAN* plan = _plan[region.Value()];
  while (plan != nullptr && plan->Src_id().Value() >= 1) {
    plan->Print(os, 4);
    plan = _plan[plan->Src_id().Value()];
  }
  os << "<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
}

double RESBM_CTX::Total_latency(REGION_ID region) {
  if (region.Value() <= 1) return 0.;

  MIN_LATENCY_PLAN* plan = Plan(region);
  if (plan == nullptr) return INVALID_LATENCY;

  double   total_laten = plan->Latency();
  uint32_t poly_deg =
      Region_cntr()->Lower_ctx()->Get_ctx_param().Get_poly_degree();
  for (const ELEM_BTS_INFO& bts_info : plan->Redundant_bootstrap()) {
    total_laten -= Operation_cost(OPC_BOOTSTRAP, bts_info.Bts_lev(), poly_deg);
  }

  REGION_ID start_region = plan->Src_id();
  while (start_region.Value() > 1) {
    plan = Plan(start_region);
    AIR_ASSERT(plan != nullptr);
    total_laten += plan->Latency();
    start_region = plan->Src_id();
  }
  return total_laten;
}

SCALE_LEVEL RESBM_CTX::Scale_level(REGION_ID             region,
                                   CONST_REGION_ELEM_PTR elem) const {
  MIN_LATENCY_PLAN* plan = Plan(region);

  while (plan->Dst_id().Value() >= elem->Region_id().Value()) {
    if (plan->Src_id().Value() <= elem->Region_id().Value() &&
        plan->Dst_id().Value() >= elem->Region_id().Value()) {
      MIN_LATENCY_PLAN::ELEM_SCALE_LEV::const_iterator iter =
          plan->Scale_lev().find(elem->Id());
      if (iter != plan->Scale_lev().end()) return iter->second;
    }
    plan = Plan(plan->Src_id());
  }
  return SCALE_LEVEL();
}

void MIN_LATENCY_PLAN::Print(std::ostream& os, uint32_t indent) const {
  std::string indent0(indent, ' ');
  std::string indent1(indent + 4, ' ');
  std::string indent2(indent + 8, ' ');

  os << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
  os << indent0 << "MIN_LATEN_PLAN of [" << Src_id().Value() << ","
     << Dst_id().Value() << "]"
     << " consume " << (Dst_id().Value() - Src_id().Value())
     << " level:" << std::endl;
  for (uint32_t id = Src_id().Value(); id <= Dst_id().Value(); ++id) {
    os << indent1 << "Region-" << id << " latency= " << Latency(REGION_ID(id))
       << std::endl;
    const CUT_TYPE& cut = Min_cut(REGION_ID(id));
    if (cut.Empty()) continue;
    os << indent1 << (Src_id().Value() == id ? "Bootstrap" : "Rescale")
       << " nodes: " << std::endl;
    for (REGION_ELEM_ID elem : cut.Cut_elem()) {
      AIR_ASSERT(elem != air::base::Null_id);
      os << indent2 << Region_cntr()->Region_elem(elem)->To_str() << std::endl;
    }
  }

  for (const ELEM_BTS_INFO& bts_info : Redundant_bootstrap()) {
    os << indent1 << "Redundant bts point: " << bts_info.To_str() << std::endl;
  }

  os << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
}

}  // namespace ckks
}  // namespace fhe
