//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/base/container.h"
#include "air/base/st.h"
#include "air/base/visitor.h"
#include "air/core/handler.h"
#include "air/opt/ssa_build.h"
#include "air/opt/ssa_container.h"
#include "air/util/error.h"
#include "ckks_stats.h"
#include "fhe/ckks/ckks_gen.h"
#include "fhe/ckks/sihe2ckks_lower.h"
#include "fhe/core/ctx_param_ana.h"
#include "fhe/sihe/sihe_handler.h"
#include "resbm.h"
#include "rt_validate_scale_level.h"
#include "scale_manager.h"

using namespace air::base;

namespace fhe {
namespace ckks {

GLOB_SCOPE* Ckks_driver(GLOB_SCOPE* glob, core::LOWER_CTX* lower_ctx,
                        const air::driver::DRIVER_CTX* driver_ctx,
                        const CKKS_CONFIG*             config) {
  GLOB_SCOPE* new_glob = new GLOB_SCOPE(glob->Id(), true);
  AIR_ASSERT(new_glob != nullptr);
  new_glob->Clone(*glob, true);

  // update hamming_weight of CTX_PARAMS with option
  lower_ctx->Get_ctx_param().Set_hamming_weight(config->Hamming_weight());

  SIHE2CKKS_LOWER sihe2ckks_lower(new_glob, lower_ctx, config);
  if (config->Rgn_scl_bts_mng() || config->Rgn_bts_mng()) {
    // 1. lower SIHE to CKKS domain
    for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
         it != glob->End_func_scope(); ++it) {
      FUNC_SCOPE* func      = &(*it);
      FUNC_SCOPE* ckks_func = &sihe2ckks_lower.Lower_server_func(func);
    }
    // 2. perform modular level pass RESBM to insert required scale/level
    // management operations, like bootstrap/rescale/modswitch
    RESBM resbm(driver_ctx, config, new_glob, lower_ctx);
    resbm.Perform();
    // 3. perform other function level pass
    for (GLOB_SCOPE::FUNC_SCOPE_ITER it = new_glob->Begin_func_scope();
         it != new_glob->End_func_scope(); ++it) {
      FUNC_SCOPE*   ckks_func = &(*it);
      SCALE_MANAGER scale_mngr(driver_ctx, config, ckks_func, lower_ctx);
      scale_mngr.Run();
      core::CTX_PARAM_ANA ctx_param_ana(ckks_func, lower_ctx, driver_ctx,
                                        config);
      ctx_param_ana.Run();
    }
  } else {
    for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
         it != glob->End_func_scope(); ++it) {
      FUNC_SCOPE* func      = &(*it);
      FUNC_SCOPE* ckks_func = &sihe2ckks_lower.Lower_server_func(func);

      SCALE_MANAGER scale_mngr(driver_ctx, config, ckks_func, lower_ctx);
      scale_mngr.Run();

      core::CTX_PARAM_ANA ctx_param_ana(ckks_func, lower_ctx, driver_ctx,
                                        config);
      ctx_param_ana.Run();
    }
  }
  delete glob;

  if (config->Trace_stat() || config->Per_op_trace_stat()) {
    CKKS_STATS stats(*new_glob, *lower_ctx, config->Per_op_trace_stat());
    typedef air::base::OP_STATS_CTX<CKKS_STATS> STATS_CTX;
    STATS_CTX                                   stats_ctx(stats);
    for (GLOB_SCOPE::FUNC_SCOPE_ITER it = new_glob->Begin_func_scope();
         it != new_glob->End_func_scope(); ++it) {
      FUNC_SCOPE*                   func = &(*it);
      air::base::VISITOR<STATS_CTX> trav(stats_ctx);
      trav.Visit<void>(func->Container().Entry_node());
    }
    stats.Fixup_call_stats();
    stats.Print(driver_ctx->Tstream());
  }

  if (config->Rt_val_scl_lvl()) {
    RT_VAL_SCL_LVL val_bts_smo(driver_ctx, config, new_glob, lower_ctx);
    val_bts_smo.Run();
  }
  return new_glob;
}  // Ckks_driver

}  // namespace ckks
}  // namespace fhe
