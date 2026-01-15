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
#include "nn/vector/sharding_opt.h"
#include "nn/vector/simp.h"
#include "nn/vector/strided_slice_fusion.h"
#include "nn/vector/t2mv_handler.h"
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

static void Instrument_nn_ir(GLOB_SCOPE* glob, VECTOR_CTX& ctx,
                             const air::driver::DRIVER_CTX* driver_ctx,
                             const VECTOR_CONFIG&           cfg) {
  // instrument original IR to dump result if Rt_dump is set
  if (cfg.Rt_dump() || cfg.Rt_timing() || cfg.Rt_validate()) {
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
}

static void Analyze_vec_context(GLOB_SCOPE* glob, VECTOR_CTX& ctx,
                                const air::driver::DRIVER_CTX* driver_ctx,
                                const VECTOR_CONFIG&           cfg) {
  // analyze vector context information
  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
       it != glob->End_func_scope(); ++it) {
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
}

static GLOB_SCOPE* Stride_slice_opt(GLOB_SCOPE* glob, VECTOR_CTX& ctx,
                                    const air::driver::DRIVER_CTX* driver_ctx,
                                    const VECTOR_CONFIG&           cfg) {
  // Handle pad/stride in conv/pool
  GLOB_SCOPE* new_glob = new GLOB_SCOPE(glob->Id(), true);
  AIR_ASSERT(new_glob != nullptr);
  new_glob->Clone(*glob);
  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
       it != glob->End_func_scope(); ++it) {
    FUNC_SCOPE* func     = &(*it);
    FUNC_SCOPE* new_func = &new_glob->New_func_scope(func->Id());
    new_func->Clone(*func);
    CONTAINER& cntr = new_func->Container();

    NODE_PTR                      body = func->Container().Entry_node();
    nn::vector::TENSOR2VECTOR_CTX trav_ctx(&cntr, ctx, driver_ctx, cfg);

    air::base::VISITOR<nn::vector::TENSOR2VECTOR_CTX,
                       air::core::HANDLER<CORE_HANDLER>,
                       nn::core::HANDLER<nn::vector::T2VSLICE_HANDLER> >
                            trav(trav_ctx);
    air::base::HANDLER_RETV retv = trav.Visit<air::base::HANDLER_RETV>(body);
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

  delete glob;
  return new_glob;
}

static void Constant_folding_opt(GLOB_SCOPE* glob, VECTOR_CTX& ctx,
                                 const air::driver::DRIVER_CTX* driver_ctx,
                                 const VECTOR_CONFIG&           cfg) {
  if (cfg.Const_fold()) {
    for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
         it != glob->End_func_scope(); ++it) {
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
}

static void Mask_fusing_opt(GLOB_SCOPE* glob, VECTOR_CTX& ctx,
                            const air::driver::DRIVER_CTX* driver_ctx,
                            const VECTOR_CONFIG&           cfg) {
  if (cfg.Mask_fuse()) {
    for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
         it != glob->End_func_scope(); ++it) {
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
}

static GLOB_SCOPE* T2mv_opt(GLOB_SCOPE* glob, VECTOR_CTX& ctx,
                            const air::driver::DRIVER_CTX* driver_ctx,
                            const VECTOR_CONFIG&           cfg) {
  if (cfg.Decompose_mid_op()) {
    GLOB_SCOPE* new_glob = new GLOB_SCOPE(glob->Id(), true);
    AIR_ASSERT(new_glob != nullptr);
    new_glob->Clone(*glob);
    for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
         it != glob->End_func_scope(); ++it) {
      FUNC_SCOPE* func     = &(*it);
      FUNC_SCOPE* new_func = &new_glob->New_func_scope(func->Id());
      new_func->Clone(*func);
      CONTAINER& cntr = new_func->Container();

      nn::vector::TENSOR2VECTOR_CTX trav_ctx(&cntr, ctx, driver_ctx, cfg);
      air::base::VISITOR<nn::vector::TENSOR2VECTOR_CTX,
                         air::core::HANDLER<nn::vector::CORE_HANDLER>,
                         nn::core::HANDLER<nn::vector::T2MV_HANDLER> >
                              trav(trav_ctx);
      NODE_PTR                body = func->Container().Entry_node();
      air::base::HANDLER_RETV retv = trav.Visit<air::base::HANDLER_RETV>(body);
      AIR_ASSERT(retv.Node() != air::base::Null_ptr && retv.Node()->Is_entry());
      new_func->Set_entry_stmt(retv.Node()->Stmt());
      // new_func->Print();
    }
    delete glob;
    return new_glob;
  }
  return glob;
}

static GLOB_SCOPE* Mv2v_opt(GLOB_SCOPE* glob, VECTOR_CTX& ctx,
                            const air::driver::DRIVER_CTX* driver_ctx,
                            const VECTOR_CONFIG&           cfg) {
  if (cfg.Decompose_mid_op()) {
    // Lower middle level vector ops in IR to low level vector ops, currently
    // only roll_sum op is lowered.
    GLOB_SCOPE* new_glob = new GLOB_SCOPE(glob->Id(), true);
    AIR_ASSERT(new_glob != nullptr);
    new_glob->Clone(*glob);
    for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
         it != glob->End_func_scope(); ++it) {
      FUNC_SCOPE* func     = &(*it);
      FUNC_SCOPE* new_func = &new_glob->New_func_scope(func->Id());
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
    delete glob;
    return new_glob;
  }
  return glob;
}

static GLOB_SCOPE* Lower_to_vector(GLOB_SCOPE* glob, VECTOR_CTX& ctx,
                                   const air::driver::DRIVER_CTX* driver_ctx,
                                   const VECTOR_CONFIG&           cfg) {
  GLOB_SCOPE* new_glob = new GLOB_SCOPE(glob->Id(), true);
  AIR_ASSERT(new_glob != nullptr);
  new_glob->Clone(*glob);
  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
       it != glob->End_func_scope(); ++it) {
    FUNC_SCOPE* func     = &(*it);
    FUNC_SCOPE* new_func = &new_glob->New_func_scope(func->Id());
    new_func->Clone(*func);
    CONTAINER& cntr = new_func->Container();

    nn::vector::TENSOR2VECTOR_CTX trav_ctx(&cntr, ctx, driver_ctx, cfg);
    trav_ctx.Set_cur_func_scope(new_func);
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

  delete glob;
  return new_glob;
}

GLOB_SCOPE* Vector_driver(GLOB_SCOPE* glob, VECTOR_CTX& ctx,
                          const air::driver::DRIVER_CTX* driver_ctx,
                          const VECTOR_CONFIG&           cfg) {
  Instrument_nn_ir(glob, ctx, driver_ctx, cfg);

  GLOB_SCOPE* tmp_glob = Run_sharding_opt(glob, ctx, driver_ctx, cfg);

  Analyze_vec_context(tmp_glob, ctx, driver_ctx, cfg);

  tmp_glob = Stride_slice_opt(tmp_glob, ctx, driver_ctx, cfg);

  tmp_glob = T2mv_opt(tmp_glob, ctx, driver_ctx, cfg);

  Constant_folding_opt(tmp_glob, ctx, driver_ctx, cfg);

  Mask_fusing_opt(tmp_glob, ctx, driver_ctx, cfg);

  tmp_glob = Lower_to_vector(tmp_glob, ctx, driver_ctx, cfg);

  tmp_glob = Mv2v_opt(tmp_glob, ctx, driver_ctx, cfg);

  return tmp_glob;
}

}  // namespace vector
}  // namespace nn
