//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_RESBM_H
#define FHE_CKKS_RESBM_H

#include <cfloat>
#include <climits>
#include <fstream>
#include <ios>
#include <memory>
#include <random>
#include <type_traits>
#include <vector>

#include "../opt/include/scc_builder.h"
#include "air/base/container.h"
#include "air/base/container_decl.h"
#include "air/base/id_wrapper.h"
#include "air/base/st.h"
#include "air/base/st_decl.h"
#include "air/base/transform_ctx.h"
#include "air/driver/driver_ctx.h"
#include "air/opt/dfg_builder.h"
#include "air/opt/scc_container.h"
#include "air/opt/ssa_build.h"
#include "air/opt/ssa_container.h"
#include "air/util/debug.h"
#include "air/util/error.h"
#include "dfg_region.h"
#include "dfg_region_builder.h"
#include "fhe/ckks/config.h"
#include "fhe/core/lower_ctx.h"
#include "resbm_ctx.h"

namespace fhe {
namespace ckks {

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

  RESBM(const CKKS_CONFIG* cfg, const air::driver::DRIVER_CTX* driver_ctx,
        GLOB_SCOPE* glob_scope, LOWER_CTX* lower_ctx)
      : _glob_scope(glob_scope),
        _lower_ctx(lower_ctx),
        _config(cfg),
        _driver_ctx(driver_ctx),
        _region_cntr(glob_scope, lower_ctx),
        _resbm_ctx(cfg, &_region_cntr) {}
  ~RESBM() {}

  R_CODE Perform();

private:
  // REQUIRED UNDEFINED UNWANTED methods
  RESBM(void);
  RESBM(const RESBM&);
  RESBM operator=(const RESBM&);

  void Remove_bootstrap();
  void Cal_min_laten_plan();

  const REGION_CONTAINER* Region_cntr(void) const { return &_region_cntr; }
  REGION_CONTAINER*       Region_cntr(void) { return &_region_cntr; }
  GLOB_SCOPE*             Glob_scope(void) const { return _glob_scope; }
  LOWER_CTX*              Lower_ctx(void) const { return _lower_ctx; }
  const CKKS_CONFIG*      Cfg(void) { return _config; }
  const air::driver::DRIVER_CTX* Driver_ctx(void) { return _driver_ctx; }
  RESBM_CTX*                     Resbm_ctx(void) { return &_resbm_ctx; }

  GLOB_SCOPE*                    _glob_scope;
  core::LOWER_CTX*               _lower_ctx;
  const CKKS_CONFIG*             _config;
  const air::driver::DRIVER_CTX* _driver_ctx;
  REGION_CONTAINER               _region_cntr;
  RESBM_CTX                      _resbm_ctx;
};
}  // namespace ckks
}  // namespace fhe
#endif