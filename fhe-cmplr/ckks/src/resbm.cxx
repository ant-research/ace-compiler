//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "resbm.h"

#include <cfloat>
#include <ctime>
#include <iostream>
#include <list>
#include <memory>
#include <string>

#include "air/base/container.h"
#include "air/base/container_decl.h"
#include "air/base/id_wrapper.h"
#include "air/base/st_decl.h"
#include "air/core/opcode.h"
#include "air/opt/dfg_data.h"
#include "air/opt/scc_container.h"
#include "air/opt/scc_node.h"
#include "air/util/debug.h"
#include "bootstrap_inserter.h"
#include "dfg_region.h"
#include "dfg_region_builder.h"
#include "fhe/ckks/ckks_opcode.h"
#include "min_cut_region.h"
#include "resbm_ctx.h"
#include "scale_planner.h"
namespace fhe {
namespace ckks {

using namespace air::opt;

void RESBM::Remove_bootstrap() {
  for (GLOB_SCOPE::FUNC_SCOPE_ITER scope_iter =
           Glob_scope()->Begin_func_scope();
       scope_iter != Glob_scope()->End_func_scope(); ++scope_iter) {
    air::base::CONTAINER* cntr = &(*scope_iter).Container();
    air::base::STMT_LIST  sl   = cntr->Stmt_list();
    for (air::base::STMT_PTR stmt = sl.Begin_stmt(); stmt != sl.End_stmt();
         stmt                     = stmt->Next()) {
      air::base::NODE_PTR stmt_node = stmt->Node();
      for (uint32_t id = 0; id < stmt_node->Num_child(); ++id) {
        air::base::NODE_PTR child = stmt_node->Child(id);
        if (child->Opcode() != OPC_BOOTSTRAP) continue;
        stmt_node->Set_child(id, child->Child(0));
      }
    }
  }
}

void RESBM::Cal_min_laten_plan() {
  // 1. initialize noise management plans in context
  Resbm_ctx()->Init_noise_mng_plan(Region_cntr()->Region_cnt());

  // 2. traverse each region, and calculate min
  for (uint32_t start_id = 1; start_id < Region_cntr()->Region_cnt();
       ++start_id) {
    uint32_t max_lev = _config->Max_bts_lev();
    if (start_id == 1)
      max_lev += Lower_ctx()->Get_ctx_param().Mul_depth_of_bootstrap();

    REGION_ID start(start_id);
    double    start_min_laten = Resbm_ctx()->Total_latency(start);
    AIR_ASSERT_MSG(start_min_laten < MAX_LATENCY,
                   "start with invalid total latency");

    for (uint32_t end_id = start_id + 1; end_id < Region_cntr()->Region_cnt();
         ++end_id) {
      REGION_ID            end(end_id);
      MIN_LATENCY_PLAN_PTR plan =
          std::make_unique<MIN_LATENCY_PLAN>(Region_cntr(), start, end);
      SCALE_PLANNER scale_plan(Resbm_ctx(), Region_cntr(), start, end,
                               plan.get());
      bool          res = scale_plan.Perform();

      // failed create scale management plan for regions [start, end].
      if (!res) break;
      // FHE computation in [start, end] consumed more level than allowed.
      uint32_t consume_lev = plan->Consume_level();
      if (consume_lev > max_lev) break;

      // find better noise management plan for current end region,
      // update the optimal noise management plan
      double new_tot_laten = start_min_laten + plan->Latency();
      double pre_tot_laten = Resbm_ctx()->Total_latency(REGION_ID(end_id));
      // plan->Print(std::cout, 0);

      if (new_tot_laten < pre_tot_laten) {
        Resbm_ctx()->Update_noise_mng_plan(REGION_ID(end_id), plan.release());
      }
      if (consume_lev == max_lev) break;
    }
  }
  Resbm_ctx()->Print(Resbm_ctx()->Trace_file(), 0);  //, REGION_ID(11));
}

R_CODE RESBM::Perform() {
  clock_t start = clock();
  Remove_bootstrap();

  // 1. build Region
  REGION_BUILDER region_builder(Region_cntr(), Driver_ctx());
  region_builder.Perform();

  /*
  for (uint32_t depth= 1; depth < 16; ++depth) {
    MIN_CUT min_cut(Region_cntr(), REGION_ID(depth), 2,
                    MIN_CUT_BTS);
                    // MIN_CUT_RESCALE);
    min_cut.Perform();
  } */

  // 2. create bootstrap and rescale plan
  Cal_min_laten_plan();

  // 3. insert bootstrap
  BOOTSTRAP_INSERTER bts_inserter(Resbm_ctx(), Glob_scope(), Lower_ctx());
  bts_inserter.Perform();
  // 4. insert rescale

  clock_t end_t = clock();
  std::cout << "RESBM consume " << 1.0 * (end_t - start) / CLOCKS_PER_SEC
            << " sec." << std::endl;
  return R_CODE::NORMAL;
}

}  // namespace ckks
}  // namespace fhe