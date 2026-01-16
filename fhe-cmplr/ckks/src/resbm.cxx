//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "resbm.h"

#include <algorithm>
#include <ctime>

#include "air/base/container.h"
#include "air/base/ptr_wrapper.h"
#include "air/opt/scc_container.h"
#include "air/opt/scc_node.h"
#include "dfg_region.h"
#include "dfg_region_builder.h"
#include "fhe/ckks/config.h"
#include "min_cut_region.h"
#include "resbm_ctx.h"
#include "resbm_scale_mgr.h"
#include "smo_bootstrap_inserter.h"
namespace fhe {
namespace ckks {

using namespace air::opt;

void RESBM::Update_trace_detail(void) const {
  CKKS_CONFIG* config = const_cast<CKKS_CONFIG*>(Ckks_cfg());
  config->_trace_detail |= config->Trace_resbm() ? TD_RESBM_SCALE_MGR : 0;
  config->_trace_detail |= config->Trace_min_cut() ? TD_RESBM_MIN_CUT : 0;
  config->_trace_detail |= config->Trace_scl_mgr() ? TD_CKKS_SCALE_MGT : 0;
  config->_trace_detail |= config->Trace_rsc_insert() ? TD_RESBM_RS_INSERT : 0;
  config->_trace_detail |= config->Trace_bts_insert() ? TD_RESBM_BTS_INSERT : 0;
  config->_trace_detail |= config->Trace_sbm_plan() ? TD_SBM_PLAN : 0;
}

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
  // forward traverse each region, and calculate min latency rescale/bootstrap
  // plan
  for (uint32_t src_id = 1; src_id < Region_cntr()->Region_cnt(); ++src_id) {
    // calculate max allowed cipher level of start region.
    uint32_t consumable_level = Ckks_cfg()->Max_bts_lvl();
    uint32_t min_bts_level    = (src_id == 1 ? 1 : Context()->Min_bts_lev());
    if (src_id == 1 && Ckks_cfg()->Input_cipher_lvl() > 0) {
      // ciphertext level is one higher than the consumable value.
      consumable_level = Ckks_cfg()->Input_cipher_lvl() - 1;
    }

    REGION_ID src(src_id);
    if (src_id != 1 && Context()->Plan(src) == nullptr) continue;
    // filter regions that require excessive bootstrapping.
    if (Context()->Min_cut(src, Context()->Max_bts_lvl(), MIN_CUT_BTS).Size() >
        Context()->Max_bts_cnt())
      continue;

    double src_min_laten = Context()->Total_latency(src);
    AIR_ASSERT_MSG(src_min_laten < MAX_LATENCY,
                   "src with invalid total latency");
    Context()->Trace(TD_RESBM, "Cal plan from src region_", src_id,
                     ", src min laten= ", src_min_laten, "\n");
    for (uint32_t dst_id = src_id + min_bts_level;
         dst_id < Region_cntr()->Region_cnt(); ++dst_id) {
      REGION_ID dst(dst_id);
      // filter regions that require excessive bootstrapping.
      if (Context()
              ->Min_cut(dst, Context()->Max_bts_lvl(), MIN_CUT_BTS)
              .Size() > Context()->Max_bts_cnt())
        continue;

      SCALE_MGR scale_mgr(Context(), Region_cntr(), src, dst, consumable_level);
      MIN_LATENCY_PLAN* plan = scale_mgr.Perform();
      // failed create scale management plan for regions [src, dst].
      if (plan == nullptr) break;
      uint32_t consumed_lev = plan->Used_level();

      // find better scale/bootstrap management plan for the dst region,
      // update the optimal plan
      double plan_laten    = plan->Latency();
      double new_tot_laten = src_min_laten + plan_laten;
      double pre_tot_laten = Context()->Total_latency(dst);
      Context()->Trace(TD_RESBM, "Plan of [", src_id, ",", dst_id, "]",
                       ": laten= ", plan_laten,
                       ", new total latency= ", new_tot_laten,
                       ", pre total latency= ", pre_tot_laten, "\n");
      if (new_tot_laten < pre_tot_laten) {
        Context()->Update_plan(dst, plan);
      } else {
        Context()->Release(plan);
      }
      if (consumed_lev == consumable_level) break;
      AIR_ASSERT(consumed_lev < consumable_level);
    }
  }
  Context()->Trace_obj(TD_RESBM, Context());
}

R_CODE RESBM::Perform() {
  // Update trace flag of RESBM
  Update_trace_detail();
  Context()->Trace(TD_RESBM, "RESBM start:\n");
  clock_t start = clock();
  // 1. remove pre-inserted bootstrap
  Remove_bootstrap();

  // 2. build Region
  REGION_BUILDER region_builder(Region_cntr(), Driver_ctx());
  region_builder.Perform();
  if (Context()->Is_trace(TD_RESBM)) {
    Region_cntr()->Print_dot("Region.dot");
  }

  // 3. create bootstrap and rescale plan
  Cal_min_laten_plan();

  // 4. insert bootstrap
  SMO_BTS_INSERTER inserter(Context(), Glob_scope(), Lower_ctx());
  inserter.Perform();

  clock_t end_t = clock();
  Context()->Trace(TD_RESBM, "RESBM end. Consume ",
                   1.0 * (end_t - start) / CLOCKS_PER_SEC, " s.\n");
  Context()->Trace_obj(TD_SBM_PLAN, Context());
  Context()->Trace(TD_SBM_PLAN, "RESBM end. Consume ",
                   1.0 * (end_t - start) / CLOCKS_PER_SEC, " s.\n");
  return R_CODE::NORMAL;
}

}  // namespace ckks
}  // namespace fhe
