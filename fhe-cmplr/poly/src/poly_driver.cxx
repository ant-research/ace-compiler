//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/poly/poly_driver.h"

#include <iostream>

#include "air/base/container.h"
#include "air/base/st.h"
#include "air/core/handler.h"
#include "ckks2poly.h"

using namespace air::base;

namespace fhe {

namespace poly {

GLOB_SCOPE* POLY_DRIVER::Run(POLY_CONFIG& config, GLOB_SCOPE* glob,
                             core::LOWER_CTX& lower_ctx) {
  GLOB_SCOPE* new_glob = new GLOB_SCOPE(glob->Id(), true);
  AIR_ASSERT(new_glob != nullptr);
  new_glob->Clone(*glob);

  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
       it != glob->End_func_scope(); ++it) {
    FUNC_SCOPE* func     = &(*it);
    FUNC_SCOPE* new_func = &new_glob->New_func_scope(func->Id());
    new_func->Clone(*func);
    CONTAINER& cntr = new_func->Container();

    CKKS2POLY_CTX     ctx(config, &lower_ctx, &cntr);
    CKKS2POLY_VISITOR visitor(ctx);

    NODE_PTR       body = func->Container().Entry_node();
    CKKS2POLY_RETV retv = visitor.Visit<CKKS2POLY_RETV>(body);
    AIR_ASSERT(retv.Num_node() == 1 && retv.Node()->Is_entry());

    new_func->Set_entry_stmt(retv.Node()->Stmt());
  }
  return new_glob;
}

}  // namespace poly

}  // namespace fhe
