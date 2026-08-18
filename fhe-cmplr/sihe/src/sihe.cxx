//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/base/container.h"
#include "air/base/st.h"
#include "air/base/st_decl.h"
#include "air/base/st_iter.h"
#include "air/base/visitor.h"
#include "air/core/handler.h"
#include "air/driver/driver_ctx.h"
#include "air/util/debug.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/sihe/sihe_gen.h"
#include "fhe/sihe/sihe_handler.h"
#include "fhe/sihe/vector2sihe_lower.h"
#include "nn/vector/handler.h"
#include "nn/vector/vector_gen.h"

using namespace air::base;

namespace fhe {
namespace sihe {

GLOB_SCOPE* Sihe_driver(GLOB_SCOPE* glob, core::LOWER_CTX* lower_ctx,
                        air::driver::DRIVER_CTX* driver_ctx,
                        const SIHE_CONFIG&       cfg) {
  GLOB_SCOPE* new_glob = new GLOB_SCOPE(glob->Id(), true);
  AIR_ASSERT(new_glob != nullptr);
  new_glob->Clone(*glob, false);

  VECTOR2SIHE_LOWER vec2sihe_lower(new_glob, lower_ctx, driver_ctx, cfg);
  // 1. lower function symbol and signature
  vec2sihe_lower.Lower_func_tab(glob);

  // 2. lower function scope
  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
       it != glob->End_func_scope(); ++it) {
    FUNC_SCOPE* func = &(*it);
    (void)vec2sihe_lower.Lower_vec_func(func);
  }
  return new_glob;
}

}  // namespace sihe
}  // namespace fhe
