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
#include "air/driver/driver.h"
#include "air/driver/driver_ctx.h"
#include "nn/vector/config.h"
#include "nn/vector/vector_ctx.h"

using namespace air::base;
using namespace nn::vector;

namespace nn {
namespace vector {

GLOB_SCOPE* Run_sharding_opt(GLOB_SCOPE* glob, VECTOR_CTX& ctx,
                             const air::driver::DRIVER_CTX* driver_ctx,
                             const VECTOR_CONFIG&           cfg);

}  // namespace vector
}  // namespace nn
