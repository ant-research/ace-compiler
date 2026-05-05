//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_RESBM_H
#define FHE_CKKS_RESBM_H

#include "air/base/transform_ctx.h"
#include "air/driver/driver_ctx.h"
#include "air/opt/dfg_builder.h"
#include "air/opt/scc_container.h"
#include "air/opt/ssa_build.h"
#include "air/opt/ssa_container.h"
#include "dfg_region.h"
#include "dfg_region_builder.h"
#include "fhe/ckks/config.h"
#include "fhe/core/lower_ctx.h"
#include "resbm_ctx.h"

namespace fhe {
namespace ckks {
//! @brief Implementation of region-based scale and bootstrap management.
//! Constraints on input program: ReSBM requires each ciphertext
//! (variable or FHE operation result) to have a fixed scale and level. Loops
//! where ciphertext scale increases with each iteration must be unrolled before
//! ReSBM. Recursive calls with increasing ciphertext scale in each recursion
//! must be inlined prior to ReSBM. The bootstrap and other scale management
//! operation should be removed before ReSBM.
//! RESBM includes 3 steps.
//!
//! 1. Create region based on data flow graph:
//! The input program only consists of FHE arithmetic and rotation operations,
//! and loops with compile-time determined loop bounds. Leveraging the key
//! insight that only FHE multiplication increase ciphertext scale, and
//! ciphertext level and scale remain unchanged after FHE addition,
//! relinearization, and rotation. ReSBM partitions a program's DFG into
//! regions, ensuring each region contains FHE multiplications that establish a
//! multiplicative depth of exactly one. Consequently, the number of regions
//! equals the program's maximum multiplicative depth plus one. Moreover, the
//! multiplications within a region are always positioned at its beginning.
//!
//! 2. Create minimal latency scale/bootstrap mng plan:
//! The max ciphertext level after bootstrap is set to
//! CKKS_CONFIG::_max_bts_lev. If the input level is configured, set the
//! ciphertext level in the first region (with mul_depth = 1) to
//! CKKS_CONFIG::_input_lev.
//! The minimal latency scale and bootstrap management plan are determined using
//! dynamic programming. The latency of each region is the sum of its FHE
//! operations. Regions are traversed sequentially from the first region (with
//! mul_depth = 1) to the last. For a source (src) region, consider the regions
//! within (src, src + CKKS_CONFIG::_max_bts_lev) as destination (dst) regions.
//! Assuming bootstrap insertion in src and dst, we calculate the minimal
//! latency of each region in [src, dst] using the min_cut method. The total
//! latency from program entry to dst is calculated with:
//! tot_laten(dst) = tot_laten(src) + sum(laten(src), .., laten(dst))
//! If tot_laten(dst) reaches a lower value, update the scale/bootstrap
//! management plan of dst with the current plan. A forward pass to the last
//! region ensures the optimal scale/bootstrap management for the program,
//! which is the plan of the last region.
//!
//! 3. Insert scale management operation and bootstrap:
//! For the entry function, scale management operations and bootstraps are
//! inserted directly according to the scale/bootstrap management plan. For
//! called functions, clone the function for each different scale/bootstrap
//! management plan and replace the calls with the appropriate cloned version.
class RESBM {
public:
  using GLOB_SCOPE    = air::base::GLOB_SCOPE;
  using FUNC_SCOPE    = air::base::FUNC_SCOPE;
  using FUNC_ID       = air::base::FUNC_ID;
  using LOWER_CTX     = core::LOWER_CTX;
  using SSA_CONTAINER = air::opt::SSA_CONTAINER;
  using DFG_CONTAINER = air::opt::DFG_CONTAINER;
  using SCC_CONTAINER = air::opt::SCC_CONTAINER;
  using DFG           = DFG_CONTAINER::DFG;
  using SCC_GRAPH     = SCC_CONTAINER::SCC_GRAPH;
  using SCC_BUILDER   = air::opt::SCC_BUILDER<DFG>;
  using SSA_CNTR_MAP  = std::map<FUNC_ID, SSA_CONTAINER>;
  using DFG_CNTR_MAP  = std::map<FUNC_ID, DFG_CONTAINER>;
  using SCC_CNTR_MAP  = std::map<FUNC_ID, SCC_CONTAINER>;

  RESBM(const air::driver::DRIVER_CTX* driver_ctx, const CKKS_CONFIG* cfg,
        GLOB_SCOPE* glob_scope, LOWER_CTX* lower_ctx)
      : _glob_scope(glob_scope),
        _lower_ctx(lower_ctx),
        _config(cfg),
        _driver_ctx(driver_ctx),
        _region_cntr(glob_scope, lower_ctx),
        _resbm_ctx(driver_ctx, cfg, &_region_cntr) {}
  ~RESBM() {}

  //! @brief Perform RESBM.
  R_CODE           Perform();
  const RESBM_CTX* Context(void) const { return &_resbm_ctx; }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  RESBM(void);
  RESBM(const RESBM&);
  RESBM operator=(const RESBM&);

  //! @brief Remove previously inserted bootstrap operations.
  void Remove_bootstrap();
  //! @brief Calculate the minimal latency scale/bootstrap management plan.
  //! Traverse each region forward, treating a region as the source (src) and a
  //! later region as the destination (dst). Determine the minimal latency of
  //! regions between src and dst, and the total latency from region 1 to dst.
  //! If a smaller latency is found at dst, update dst's min_latency_plan with
  //! the current plan.
  void Cal_min_laten_plan();

  //! @brief Update trace detail for RESBM based on CKKS_CONFIG.
  void                    Update_trace_detail(void) const;
  const REGION_CONTAINER* Region_cntr(void) const { return &_region_cntr; }
  REGION_CONTAINER*       Region_cntr(void) { return &_region_cntr; }
  GLOB_SCOPE*             Glob_scope(void) const { return _glob_scope; }
  LOWER_CTX*              Lower_ctx(void) const { return _lower_ctx; }
  const CKKS_CONFIG*      Ckks_cfg(void) const { return _config; }
  const air::driver::DRIVER_CTX* Driver_ctx(void) { return _driver_ctx; }
  RESBM_CTX*                     Context(void) { return &_resbm_ctx; }

  GLOB_SCOPE*                    _glob_scope = nullptr;
  core::LOWER_CTX*               _lower_ctx  = nullptr;
  const CKKS_CONFIG*             _config     = nullptr;
  const air::driver::DRIVER_CTX* _driver_ctx = nullptr;
  REGION_CONTAINER               _region_cntr;
  RESBM_CTX                      _resbm_ctx;
};
}  // namespace ckks
}  // namespace fhe
#endif