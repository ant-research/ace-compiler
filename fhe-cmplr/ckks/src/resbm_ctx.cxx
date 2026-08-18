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
#include "air/util/debug.h"
#include "dfg_region.h"
#include "min_cut_region.h"
namespace fhe {
namespace ckks {

double RESBM_CTX::Total_latency(REGION_ID region) {
  if (region.Value() <= 1) return 0.;
  AIR_ASSERT(region.Value() < _min_laten_plan.size());

  MIN_LATENCY_PLAN* plan = Noise_mng_plan(region);
  if (plan == nullptr) return INVALID_LATENCY;

  double total_laten = plan->Latency();

  REGION_ID start_region = plan->Start_region();
  while (start_region.Value() > 1) {
    plan = Noise_mng_plan(start_region);
    AIR_ASSERT(plan != nullptr);
    total_laten += plan->Latency();
    start_region = plan->Start_region();
  }
  return total_laten;
}

MIN_LATENCY_PLAN::SCALE_INFO RESBM_CTX::Scale_info(REGION_ID       region,
                                                   REGION_ELEM_PTR elem) const {
  MIN_LATENCY_PLAN* plan = Noise_mng_plan(region);

  while (plan->Start_region().Value() >= elem->Region().Value()) {
    if (plan->Start_region().Value() <= elem->Region().Value() &&
        plan->End_region().Value() >= elem->Region().Value()) {
      MIN_LATENCY_PLAN::ELEM_SCALE_INFO::const_iterator iter =
          plan->Scale_info().find(elem->Id());
      if (iter != plan->Scale_info().end()) return iter->second;
    }
    plan = Noise_mng_plan(plan->Start_region());
  }
  return SCALE_INFO();
}

void MIN_LATENCY_PLAN::Print(std::ostream& os, uint32_t indent) {
  std::string indent0(indent, ' ');
  std::string indent1(indent + 4, ' ');
  std::string indent2(indent + 8, ' ');

  os << ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << std::endl;
  os << indent0 << "Min latency plan for regions [" << Start_region().Value()
     << "," << End_region().Value() << "]:" << std::endl;
  for (uint32_t id = Start_region().Value(); id <= End_region().Value(); ++id) {
    os << indent1 << "Region-" << id
       << " latency= " << Region_latency(REGION_ID(id)) << std::endl;
    const MIN_CUT::CUT_TYPE& cut = Min_cut(REGION_ID(id));
    if (cut.empty()) continue;
    os << indent1 << (Start_region().Value() == id ? "Bootstrap" : "Rescale")
       << " nodes: " << std::endl;
    for (REGION_ELEM_ID elem : cut) {
      AIR_ASSERT(elem != air::base::Null_id);
      os << indent2 << Region_cntr()->Region_elem(elem)->To_str() << std::endl;
    }
  }
  os << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<" << std::endl;
}

}  // namespace ckks
}  // namespace fhe