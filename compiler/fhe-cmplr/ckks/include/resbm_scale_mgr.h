//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_RESBM_SCALE_MGR_H
#define FHE_CKKS_RESBM_SCALE_MGR_H

#include <vector>

#include "air/opt/scc_container.h"
#include "air/opt/scc_node.h"
#include "air/util/debug.h"
#include "dfg_region.h"
#include "min_cut_region.h"
#include "resbm_ctx.h"
namespace fhe {
namespace ckks {

//! @brief RESBM SCALE_MGR traverses regions from _src to _dst. If _src is the
//! first region, bootstrap is ignored. Otherwise, bootstrap is assumed to be
//! inserted in both _src and _dst. SCALE_MGR calculates bootstrap and rescaling
//! points to minimize total latency using the min_cut method. It sequentially
//! traverses regions from _src to _dst. For the _src/_dst region, it calculates
//! bootstrap points, and for regions in (_src, _dst], it calculates rescale
//! points. The latency of a region is defined as the sum of its FHE operations.
//! The latency of each region is calculated based on the determined
//! bootstrap/rescale points. Additionally, the scale and level of each region
//! element are determined.
class SCALE_MGR {
public:
  using SCC_NODE_ID        = air::opt::SCC_NODE_ID;
  using SCC_NODE_PTR       = air::opt::SCC_NODE_PTR;
  using CONST_SCC_NODE_PTR = const SCC_NODE_PTR&;

  SCALE_MGR(RESBM_CTX* resbm_ctx, const REGION_CONTAINER* reg_cntr,
            REGION_ID src, REGION_ID dst, uint32_t max_lev)
      : _region_cntr(reg_cntr),
        _src(src),
        _dst(dst),
        _resbm_ctx(resbm_ctx),
        _max_lev(max_lev) {
    _plan = resbm_ctx->New_plan(src, dst);
    AIR_ASSERT(src != air::base::Null_id &&
               src.Value() < reg_cntr->Region_cnt());
    AIR_ASSERT(dst != air::base::Null_id &&
               dst.Value() < reg_cntr->Region_cnt());
    AIR_ASSERT_MSG(_region_cntr != nullptr, "nullptr check");
    AIR_ASSERT_MSG(_plan != nullptr, "nullptr check");
  }
  ~SCALE_MGR() {}

  //! @brief Generate scale management plan for regions in [start, end].
  //! Return available min latency plan.
  MIN_LATENCY_PLAN* Perform();

  //! @brief Return plan create by current planner.
  MIN_LATENCY_PLAN* Plan() const { return _plan; }

private:
  //! @brief Set scale and level of formal node.
  void Set_formal_scale_level(CONST_REGION_ELEM_PTR elem,
                              const SCALE_LEVEL&    scale_level);
  //! @brief Update scale and level of region element.
  void Update_scale_level(REGION_ELEM_ID elem_id, const SCALE_LEVEL& scale_lev);
  //! @brief Update scale/level of elements in SCC Node.
  void Update_scale_level(CONST_SCC_NODE_PTR scc_node,
                          const SCALE_LEVEL& scale_lev);
  void Handle_bypass_edge(REGION_PTR region);
  void Remove_redundant_rescale(CUT_TYPE& cut, const SCALE_LEVEL& scale_lev);
  //! @brief Remove redundant bootstraps of which operand level is not less than
  //! that of resulting level.
  void Remove_redundant_bootstrap(CUT_TYPE& cut, const SCALE_LEVEL& scale_lev);
  //! @brief Returns the redundant bootstrap that has only one data flow path
  //! and reaches the element.
  ELEM_BTS_INFO Redundant_bootstrap(REGION_ELEM_PTR         elem,
                                    const MIN_LATENCY_PLAN& plan) const;
  //! @brief Collect redundant bootstrap data depended on by the element.
  void Collect_redundant_bootstrap(REGION_ELEM_PTR elem);
  //! @brief Collect redundant bootstrap data depended on by the scc_node.
  void Collect_redundant_bootstrap(CONST_SCC_NODE_PTR scc_node);
  //! @brief Handle elements ahead of rescale, return latency of operations in
  //! these elements.
  double Handle_elem_ahead_rescale(const air::opt::SCC_CONTAINER* scc_cntr,
                                   const REGION_PTR&              region,
                                   const SCALE_LEVEL&             scale_lev);
  //! @brief Handle the src region. For the first region, calculate latency at
  //! the input level. For other regions, use the min_cut method to find
  //! bootstrap points and calculate latency as the sum of bootstrap latency and
  //! FHE operation latency post-bootstrap.
  SCALE_LEVEL Handle_src_region();
  //! @brief Handle the internal regions in (_src, _dst). Use the min_cut method
  //! to find rescale points. Calculate the latency of rescale and FHE
  //! operations before rescale at level = scale_lev._level. Calculate the
  //! latency of FHE operations after rescale at level = scale_lev._level - 1.
  //! Region latency is the sum of the latencies of each FHE operation.
  SCALE_LEVEL Handle_internal_region(REGION_ID          region,
                                     const SCALE_LEVEL& scale_lev);
  //! @brief Handle the dst region. Use the min_cut method to find rescale and
  //! bootstrap points. Calculate the latency of rescale and FHE operations
  //! before rescale at level = scale_lev._level. Calculate the latency of FHE
  //! operations between rescale and bootstrap at level = scale_lev._level - 1.
  //! Region latency is the sum of the latencies of FHE operations before
  //! bootstrap.
  void Handle_dst_region(const SCALE_LEVEL& scale_lev);

  //! @brief Return latency of rescaling operations inserted at cut.
  double Rescale_latency(const CUT_TYPE& cut, uint32_t level);
  //! @brief Return latency of bootstrap operations inserted at cut.
  double Bootstrap_latency(const CUT_TYPE& cut, uint32_t level);
  //! @brief Return latency of FHE operations in region element.
  double Fhe_op_laten(CONST_REGION_ELEM_PTR elem, uint32_t level);
  //! @brief Return latency of FHE operations in SCC node.
  double Fhe_op_laten(CONST_SCC_NODE_PTR scc_node, uint32_t level);

  uint32_t Used_lev() const {
    AIR_ASSERT(_dst.Value() > _src.Value());
    return _dst.Value() - _src.Value();
  }
  RESBM_CTX* Context() const { return _resbm_ctx; }

  const REGION_CONTAINER* Region_cntr(void) const { return _region_cntr; }
  REGION_ID               Src_region(void) const { return _src; }
  REGION_ID               Dst_region(void) const { return _dst; }
  uint32_t                Max_lev(void) const { return _max_lev; }

  MIN_LATENCY_PLAN*       _plan        = nullptr;
  const REGION_CONTAINER* _region_cntr = nullptr;
  RESBM_CTX*              _resbm_ctx   = nullptr;
  REGION_ID               _src;
  REGION_ID               _dst;
  uint32_t                _max_lev = 0;
};

}  // namespace ckks
}  // namespace fhe
#endif