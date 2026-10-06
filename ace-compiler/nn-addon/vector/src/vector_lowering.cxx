//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn/vector/vector_lowering.h"

#include "air/base/visitor.h"
#include "air/core/default_handler.h"
#include "air/core/handler.h"
#include "nn/core/handler.h"
#include "nn/vector/core_handler.h"

namespace nn {
namespace vector {
GLOB_SCOPE* Dsl_lowering(GLOB_SCOPE* glob, VECTOR_CTX& ctx,
                         const air::driver::DRIVER_CTX* driver_ctx,
                         const VECTOR_CONFIG&           cfg) {
  printf("Dsl_lowering\n");
  GLOB_SCOPE* new_glob = new GLOB_SCOPE(glob->Id(), true);
  new_glob->Clone(*glob);

  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
       it != glob->End_func_scope(); ++it) {
    FUNC_SCOPE* func     = &(*it);
    FUNC_SCOPE* new_func = &new_glob->New_func_scope(func->Id());

    new_func->Clone(*func);
    CONTAINER& cntr = new_func->Container();
    NODE_PTR   body = func->Container().Entry_node();

    DSL_LOWER_CTX lower_ctx(&cntr, ctx, driver_ctx, cfg);
    lower_ctx.Set_cur_func_scope(new_func);
    air::base::VISITOR<DSL_LOWER_CTX, air::core::HANDLER<CORE_HANDLER>,
                       nn::core::HANDLER<DSL_LOWER_HANDLER>>
             trav(lower_ctx);
    NODE_PTR retv = trav.Visit<NODE_PTR>(body);
    new_func->Set_entry_stmt(retv->Stmt());
    new_func->Print();
  }

  return new_glob;
}

}  // namespace vector
}  // namespace nn