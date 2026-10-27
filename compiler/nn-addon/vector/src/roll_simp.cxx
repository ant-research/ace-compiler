//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn/vector/roll_simp.h"

#include "air/base/handler_retv.h"
#include "air/base/visitor.h"
#include "air/core/handler.h"
#include "nn/core/handler.h"
#include "nn/vector/handler.h"

namespace nn {
namespace vector {

using namespace air::base;

GLOB_SCOPE* Roll_simp_opt(GLOB_SCOPE* glob, VECTOR_CTX& ctx,
                          const air::driver::DRIVER_CTX* driver_ctx,
                          const VECTOR_CONFIG&           cfg) {
  if (!cfg.Roll_simp()) {
    return glob;
  }

  GLOB_SCOPE* new_glob = new GLOB_SCOPE(glob->Id(), true);
  AIR_ASSERT(new_glob != nullptr);
  new_glob->Clone(*glob);

  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
       it != glob->End_func_scope(); ++it) {
    FUNC_SCOPE* func     = &(*it);
    FUNC_SCOPE* new_func = &new_glob->New_func_scope(func->Id());
    new_func->Clone(*func);
    CONTAINER& cntr = new_func->Container();

    ROLL_SIMP_CTX trav_ctx(&cntr, ctx, driver_ctx, cfg);
    trav_ctx.Set_cur_func_scope(new_func);

    air::base::VISITOR<ROLL_SIMP_CTX,
                       air::core::HANDLER<ROLL_SIMP_CORE_HANDLER>,
                       nn::core::HANDLER<nn::core::DEFAULT_HANDLER>,
                       nn::vector::HANDLER<ROLL_SIMP_VECTOR_HANDLER>>
             trav(trav_ctx);

    NODE_PTR body = func->Container().Entry_node();
    HANDLER_RETV retv = trav.Visit<HANDLER_RETV>(body);
    AIR_ASSERT(retv.Node() != air::base::Null_ptr && retv.Node()->Is_entry());
    new_func->Set_entry_stmt(retv.Node()->Stmt());
  }

  return new_glob;
}

}  // namespace vector
}  // namespace nn
