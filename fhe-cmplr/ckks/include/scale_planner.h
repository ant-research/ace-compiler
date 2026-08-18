//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_SCALE_PLANNER_H
#define FHE_CKKS_SCALE_PLANNER_H

#include <vector>

#include "air/opt/scc_node.h"
#include "air/util/debug.h"
#include "dfg_region.h"
#include "min_cut_region.h"
#include "resbm_ctx.h"
namespace fhe {
namespace ckks {

class SCALE_PLANNER {
public:
  using SCALE_INFO         = MIN_LATENCY_PLAN::SCALE_INFO;
  using SCC_NODE_ID        = air::opt::SCC_NODE_ID;
  using SCC_NODE_PTR       = air::opt::SCC_NODE_PTR;
  using CONST_SCC_NODE_PTR = const SCC_NODE_PTR&;

  SCALE_PLANNER(RESBM_CTX* resbm_ctx, const REGION_CONTAINER* reg_cntr,
                REGION_ID start_region, REGION_ID end_region,
                MIN_LATENCY_PLAN* plan)
      : _plan(plan),
        _region_cntr(reg_cntr),
        _start_region(start_region),
        _end_region(end_region),
        _resbm_ctx(resbm_ctx) {
    AIR_ASSERT(start_region != air::base::Null_id &&
               start_region.Value() < reg_cntr->Region_cnt());
    AIR_ASSERT(end_region != air::base::Null_id &&
               end_region.Value() < reg_cntr->Region_cnt());
    AIR_ASSERT_MSG(_plan != nullptr, "nullptr check");
    AIR_ASSERT_MSG(_region_cntr != nullptr, "nullptr check");
  }

  //! @brief Generate scale management plan for node in region [start, end].
  //! Return false if failed in generating the plan.
  bool Perform();

private:
  void       Set_formal_scale_info(CONST_REGION_ELEM_PTR elem,
                                   const SCALE_INFO&     scale_info);
  void       Set_region_elem_scale_info(REGION_ELEM_ID    elem_id,
                                        const SCALE_INFO& scale_info);
  void       Set_scc_node_scale_info(CONST_SCC_NODE_PTR scc_node,
                                     const SCALE_INFO&  scale_info);
  void       Handle_bypass_edge(REGION_PTR region);
  void       Remove_redundant_rescale(MIN_CUT::CUT_TYPE& cut,
                                      const SCALE_INFO&  scale_info);
  void       Remove_redundant_bootstrap(MIN_CUT::CUT_TYPE& cut,
                                        const SCALE_INFO&  scale_info);
  SCALE_INFO Handle_start_region();
  SCALE_INFO Handle_internal_region(REGION_ID         region,
                                    const SCALE_INFO& scale_info);
  void       Handle_end_region(const SCALE_INFO& scale_info);

  double Rescale_latency(const MIN_CUT::CUT_TYPE& cut, uint32_t level);
  double Bootstrap_latency(const MIN_CUT::CUT_TYPE& cut, uint32_t level);
  double Region_elem_latency(CONST_REGION_ELEM_PTR elem, uint32_t level);
  double Scc_node_latency(CONST_SCC_NODE_PTR scc_node, uint32_t level);
  double Cal_laten_start_region();
  double Cal_laten_internal_region();
  double Cal_laten_end_region();

  uint32_t Max_lev() const {
    AIR_ASSERT(_end_region.Value() > _start_region.Value());
    return _end_region.Value() - _start_region.Value();
  }
  RESBM_CTX*              Context() const { return _resbm_ctx; }
  MIN_LATENCY_PLAN*       Plan() const { return _plan; }
  const REGION_CONTAINER* Region_cntr() const { return _region_cntr; }
  REGION_ID               Start_region() const { return _start_region; }
  REGION_ID               End_region() const { return _end_region; }

  MIN_LATENCY_PLAN*       _plan;
  const REGION_CONTAINER* _region_cntr;
  RESBM_CTX*              _resbm_ctx;

  REGION_ID _start_region;
  REGION_ID _end_region;
  uint32_t  _max_lev;
};

}  // namespace ckks
}  // namespace fhe
#endif