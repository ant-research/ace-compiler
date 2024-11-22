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
#include "nn/vector/core_handler.h"
#include "nn/vector/core_preg_handler.h"
#include "nn/vector/t2vslice_handler.h"
#include "nn/vector/tensor2vector_ctx.h"
#include "nn/vector/tensor2vector_handler.h"
#include "nn/vector/tensor_instr.h"
#include "nn/vector/vector_gen.h"

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

  // Handle pad/stride in conv/pool
  GLOB_SCOPE* new_slice_glob = new GLOB_SCOPE(glob->Id(), true);
  AIR_ASSERT(new_slice_glob != nullptr);
  new_slice_glob->Clone(*glob);
  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
       it != glob->End_func_scope(); ++it) {
    FUNC_SCOPE* func     = &(*it);
    FUNC_SCOPE* new_func = &new_slice_glob->New_func_scope(func->Id());
    new_func->Clone(*func);
    CONTAINER& cntr = new_func->Container();

    NODE_PTR                      body = func->Container().Entry_node();
    nn::vector::TENSOR2VECTOR_CTX trav_ctx(&cntr, ctx, driver_ctx, cfg);
    if (!cfg.Improve_ss_insert()) {
      air::base::VISITOR<nn::vector::TENSOR2VECTOR_CTX,
                         air::core::HANDLER<CORE_HANDLER>,
                         nn::core::HANDLER<nn::vector::T2VSLICE_HANDLER> >
                              trav(trav_ctx);
      air::core::HANDLER_RETV retv = trav.Visit<air::core::HANDLER_RETV>(body);
      AIR_ASSERT(retv.Node() != air::base::Null_ptr && retv.Node()->Is_entry());
      new_func->Set_entry_stmt(retv.Node()->Stmt());
    } else {
      air::base::VISITOR<nn::vector::TENSOR2VECTOR_CTX,
                         air::core::HANDLER<CORE_PREG_HANDLER>,
                         nn::core::HANDLER<nn::vector::T2VSLICE_HANDLER> >
                              trav(trav_ctx);
      air::core::HANDLER_RETV retv = trav.Visit<air::core::HANDLER_RETV>(body);
      AIR_ASSERT(retv.Node() != air::base::Null_ptr && retv.Node()->Is_entry());
      new_func->Set_entry_stmt(retv.Node()->Stmt());
    }
  }

  // Lower to VectorIR
  GLOB_SCOPE* new_glob = new GLOB_SCOPE(glob->Id(), true);
  AIR_ASSERT(new_glob != nullptr);
  new_glob->Clone(*new_slice_glob);
  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = new_slice_glob->Begin_func_scope();
       it != new_slice_glob->End_func_scope(); ++it) {
    FUNC_SCOPE* func     = &(*it);
    FUNC_SCOPE* new_func = &new_glob->New_func_scope(func->Id());
    new_func->Clone(*func);
    CONTAINER& cntr = new_func->Container();

    nn::vector::TENSOR2VECTOR_CTX trav_ctx(&cntr, ctx, driver_ctx, cfg);
    air::base::VISITOR<nn::vector::TENSOR2VECTOR_CTX,
                       air::core::HANDLER<nn::vector::CORE_HANDLER>,
                       nn::core::HANDLER<nn::vector::TENSOR2VECTOR_HANDLER> >
             trav(trav_ctx);
    NODE_PTR body = func->Container().Entry_node();
    NODE_PTR retv = trav.Visit<NODE_PTR>(body);
    AIR_ASSERT(retv != air::base::Null_ptr && retv->Is_entry());
    new_func->Set_entry_stmt(retv->Stmt());
  }
  return new_glob;
}

}  // namespace vector
}  // namespace nn
