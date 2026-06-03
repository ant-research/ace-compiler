//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn/util/copy_prop.h"

#include "air/base/container.h"
#include "air/base/st.h"

using namespace std;
using namespace air::base;
using namespace air::util;
namespace nn {
namespace opt {

GLOB_SCOPE* Opt_perform_copy_propagation(
    GLOB_SCOPE* glob, const air::driver::DRIVER_CTX* driver_ctx) {
  for (GLOB_SCOPE::FUNC_SCOPE_ITER it = glob->Begin_func_scope();
       it != glob->End_func_scope(); ++it) {
    FUNC_SCOPE*              func = &(*it);
    air::base::ANALYZE_CTX   ana_ctx;
    COPY_PROPAGATION_MANAGER copy_prop(func, &ana_ctx);
    copy_prop.Run();
  }
  return glob;
}
}  // namespace opt
}  // namespace nn
