//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/base/handler_retv.h"
#include "air/base/transform_ctx.h"
#include "air/base/visitor.h"
#include "air/core/handler.h"
#include "nn/core/handler.h"
#include "nn/vector/copy_prop.h"
#include "nn/vector/core_handler.h"
#include "nn/vector/handler.h"
#include "nn/vector/mask_fusion.h"
#include "nn/vector/mv2v_handler.h"
#include "nn/vector/selective_strided_slice.h"
#include "nn/vector/simp.h"
#include "nn/vector/strided_slice_fusion.h"
#include "nn/vector/t2mv_handler.h"
#include "nn/vector/sharding_opt.h"
#include "nn/vector/t2vslice_handler.h"
#include "nn/vector/tensor2vector_ctx.h"
#include "nn/vector/tensor2vector_handler.h"
#include "nn/vector/tensor_instr.h"
#include "nn/vector/vec_analyze_ctx.h"
#include "nn/vector/vector_gen.h"
#include "nn/vector/vector_lowering.h"

using namespace air::base;
using namespace nn::vector;

namespace nn {
namespace vector {

GLOB_SCOPE* Vector_driver(GLOB_SCOPE* glob, VECTOR_CTX& ctx,
                          const air::driver::DRIVER_CTX* driver_ctx,
                          const VECTOR_CONFIG&           cfg) {
  // instrument original IR to dump result if Rt_dump is set
  if (cfg.Rt_dump() || cfg.Rt_validate()) {
    for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
         it != glob->End_func_scope(); ++it) {
      NODE_PTR         body = (*it).Container().Entry_node();
      TENSOR_INSTR_CTX ctx(cfg);
      air::base::VISITOR<TENSOR_INSTR_CTX,
                         air::core::HANDLER<TENSOR_INSTR_CORE_HANDLER> >
          trav(ctx);
      trav.Visit<void>(body);
    }
  }

  if (cfg.Python_dsl()) {
    return Dsl_lowering(glob, ctx, driver_ctx, cfg);
  }

  GLOB_SCOPE* tmp_glob = Run_sharding_opt(glob, ctx, driver_ctx, cfg);

  // analyze vector context information
  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = tmp_glob->Begin_func_scope();
       it != tmp_glob->End_func_scope(); ++it) {
    FUNC_SCOPE* func = &(*it);
    CONTAINER&  cntr = func->Container();
    NODE_PTR    body = cntr.Entry_node();

    nn::vector::VEC_ANALYZE_CTX trav_ctx(&cntr, ctx, driver_ctx, cfg);
    air::base::VISITOR<nn::vector::VEC_ANALYZE_CTX,
                       air::core::HANDLER<air::core::DEFAULT_HANDLER>,
                       nn::core::HANDLER<nn::vector::VEC_ANALYZE_HANDLER> >
        trav(trav_ctx);
    trav.Visit<void>(body);
  }

  // Handle pad/stride in conv/pool
  GLOB_SCOPE* new_slice_glob = new GLOB_SCOPE(tmp_glob->Id(), true);
  AIR_ASSERT(new_slice_glob != nullptr);
  new_slice_glob->Clone(*tmp_glob);
  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = tmp_glob->Begin_func_scope();
       it != tmp_glob->End_func_scope(); ++it) {
    FUNC_SCOPE* func     = &(*it);
    FUNC_SCOPE* new_func = &new_slice_glob->New_func_scope(func->Id());
    new_func->Clone(*func);
    CONTAINER& cntr = new_func->Container();

    NODE_PTR                      body = func->Container().Entry_node();
    nn::vector::TENSOR2VECTOR_CTX trav_ctx(&cntr, ctx, driver_ctx, cfg);

    air::base::VISITOR<nn::vector::TENSOR2VECTOR_CTX,
                       air::core::HANDLER<CORE_HANDLER>,
                       nn::core::HANDLER<nn::vector::T2VSLICE_HANDLER> >
                            trav(trav_ctx);
    air::core::HANDLER_RETV retv = trav.Visit<air::core::HANDLER_RETV>(body);
    AIR_ASSERT(retv.Node() != air::base::Null_ptr && retv.Node()->Is_entry());
    new_func->Set_entry_stmt(retv.Node()->Stmt());
    // new_func->Print();

    if (cfg.Selective_ss()) {
      nn::vector::SS_FUSION_CTX ss_fusion_ctx(new_func, ctx, driver_ctx, cfg);
      ss_fusion_ctx.Build_ssa();
      ss_fusion_ctx.Build_dfg();

      SS_SFT_MAP sft_map = ss_fusion_ctx.Find_fusion_target();

      // do selective ss: through partial copy prop and simplifications.
      nn::vector::SELECTIVE_STRIDED_SLICE_CTX sss_trav_ctx(
          new_func, ctx, driver_ctx, cfg, sft_map);
      sss_trav_ctx.Build_ssa();
      air::base::VISITOR<
          nn::vector::SELECTIVE_STRIDED_SLICE_CTX,
          air::core::HANDLER<nn::vector::SELECTIVE_STRIDED_SLICE_CORE_HANDLER>,
          nn::core::HANDLER<nn::core::DEFAULT_HANDLER> >
               trav(sss_trav_ctx);
      NODE_PTR body = new_func->Container().Entry_node();
      trav.Visit<void>(body);
    }
  }

  tmp_glob = new_slice_glob;

  if (cfg.Decompose_mid_op()) {
    // Decompose several NN operators to middle level vector operators.
    GLOB_SCOPE* mv_glob = new GLOB_SCOPE(glob->Id(), true);
    AIR_ASSERT(mv_glob != nullptr);
    mv_glob->Clone(*tmp_glob);
    for (GLOB_SCOPE::FUNC_SCOPE_ITER it = tmp_glob->Begin_func_scope();
         it != tmp_glob->End_func_scope(); ++it) {
      FUNC_SCOPE* func     = &(*it);
      FUNC_SCOPE* new_func = &mv_glob->New_func_scope(func->Id());
      new_func->Clone(*func);
      CONTAINER& cntr = new_func->Container();

      nn::vector::TENSOR2VECTOR_CTX trav_ctx(&cntr, ctx, driver_ctx, cfg);
      air::base::VISITOR<nn::vector::TENSOR2VECTOR_CTX,
                         air::core::HANDLER<nn::vector::CORE_HANDLER>,
                         nn::core::HANDLER<nn::vector::T2MV_HANDLER> >
                              trav(trav_ctx);
      NODE_PTR                body = func->Container().Entry_node();
      air::core::HANDLER_RETV retv = trav.Visit<air::core::HANDLER_RETV>(body);
      AIR_ASSERT(retv.Node() != air::base::Null_ptr && retv.Node()->Is_entry());
      new_func->Set_entry_stmt(retv.Node()->Stmt());
      // new_func->Print();
    }
    tmp_glob = mv_glob;
  }

  // constant folding
  if (cfg.Const_fold()) {
    for (GLOB_SCOPE::FUNC_SCOPE_ITER it = tmp_glob->Begin_func_scope();
         it != tmp_glob->End_func_scope(); ++it) {
      FUNC_SCOPE* func = &(*it);

      // 1. do copy propagation first
      nn::vector::COPY_PROP_CTX cp_trav_ctx(func, ctx, driver_ctx, cfg);
      cp_trav_ctx.Build_ssa();
      air::base::VISITOR<nn::vector::COPY_PROP_CTX,
                         air::core::HANDLER<nn::vector::COPY_PROP_CORE_HANDLER>,
                         nn::core::HANDLER<nn::core::DEFAULT_HANDLER>,
                         nn::vector::HANDLER<nn::vector::DEFAULT_HANDLER> >
               cp_trav(cp_trav_ctx);
      NODE_PTR cp_body = func->Container().Entry_node();
      cp_trav.Visit<void>(cp_body);

      // 2. do constant folding
      nn::vector::SIMP_CTX simp_trav_ctx(func, ctx, driver_ctx, cfg);
      simp_trav_ctx.Build_ssa();
      air::base::VISITOR<nn::vector::SIMP_CTX,
                         air::core::HANDLER<nn::vector::SIMP_CORE_HANDLER> >
               simp_trav(simp_trav_ctx);
      NODE_PTR simp_body = func->Container().Entry_node();
      simp_trav.Visit<void>(simp_body);
      // func->Print();
    }
  }

  if (cfg.Mask_fuse()) {
    for (GLOB_SCOPE::FUNC_SCOPE_ITER it = tmp_glob->Begin_func_scope();
         it != tmp_glob->End_func_scope(); ++it) {
      FUNC_SCOPE* func = &(*it);

      nn::vector::MASK_FUSION_CTX mf_trav_ctx(func, ctx, driver_ctx, cfg);
      mf_trav_ctx.Build_ssa();
      mf_trav_ctx.Build_dfg();

      air::base::VISITOR<nn::vector::MASK_FUSION_CTX,
                         air::core::HANDLER<air::core::DEFAULT_HANDLER>,
                         nn::core::HANDLER<nn::vector::MASK_FUSION_HANDLER> >
               trav(mf_trav_ctx);
      NODE_PTR body = func->Container().Entry_node();
      trav.Visit<void>(body);

      mf_trav_ctx.Mask_fusion();
      // func->Print();
    }
  }

  // Lower NN ops to lower vector ops
  GLOB_SCOPE* n2lv_glob = new GLOB_SCOPE(glob->Id(), true);
  AIR_ASSERT(n2lv_glob != nullptr);
  n2lv_glob->Clone(*tmp_glob);
  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = tmp_glob->Begin_func_scope();
       it != tmp_glob->End_func_scope(); ++it) {
    FUNC_SCOPE* func     = &(*it);
    FUNC_SCOPE* new_func = &n2lv_glob->New_func_scope(func->Id());
    new_func->Clone(*func);
    CONTAINER& cntr = new_func->Container();

    nn::vector::TENSOR2VECTOR_CTX trav_ctx(&cntr, ctx, driver_ctx, cfg);
    air::base::VISITOR<nn::vector::TENSOR2VECTOR_CTX,
                       air::core::HANDLER<nn::vector::CORE_HANDLER>,
                       nn::vector::HANDLER<nn::vector::DEFAULT_HANDLER>,
                       nn::core::HANDLER<nn::vector::TENSOR2VECTOR_HANDLER> >
             trav(trav_ctx);
    NODE_PTR body = func->Container().Entry_node();
    NODE_PTR retv = trav.Visit<NODE_PTR>(body);
    AIR_ASSERT(retv != air::base::Null_ptr && retv->Is_entry());
    new_func->Set_entry_stmt(retv->Stmt());
  }

  GLOB_SCOPE* final_glob = n2lv_glob;

  if (cfg.Decompose_mid_op()) {
    // Lower middle level vector ops in IR to low level vector ops, currently
    // only roll_sum op is lowered.
    final_glob = new GLOB_SCOPE(glob->Id(), true);
    AIR_ASSERT(final_glob != nullptr);
    final_glob->Clone(*n2lv_glob);
    for (GLOB_SCOPE::FUNC_SCOPE_ITER it = n2lv_glob->Begin_func_scope();
         it != n2lv_glob->End_func_scope(); ++it) {
      FUNC_SCOPE* func     = &(*it);
      FUNC_SCOPE* new_func = &final_glob->New_func_scope(func->Id());
      new_func->Clone(*func);
      CONTAINER& cntr = new_func->Container();

      nn::vector::TENSOR2VECTOR_CTX trav_ctx(&cntr, ctx, driver_ctx, cfg);
      air::base::VISITOR<nn::vector::TENSOR2VECTOR_CTX,
                         air::core::HANDLER<nn::vector::CORE_HANDLER>,
                         nn::vector::HANDLER<nn::vector::MV2V_HANDLER>,
                         nn::core::HANDLER<nn::core::DEFAULT_HANDLER> >
               trav(trav_ctx);
      NODE_PTR body = func->Container().Entry_node();
      NODE_PTR retv = trav.Visit<NODE_PTR>(body);
      AIR_ASSERT(retv != air::base::Null_ptr && retv->Is_entry());
      new_func->Set_entry_stmt(retv->Stmt());
    }
  }
  return final_glob;
}

}  // namespace vector
}  // namespace nn
