//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_T2TSHARDING_ANALYSIS_CTX_H
#define NN_T2TSHARDING_ANALYSIS_CTX_H

#include "air/base/transform_ctx.h"
#include "air/core/opcode.h"
#include "air/driver/driver.h"
#include "air/driver/driver_ctx.h"
#include "nn/vector/config.h"
#include "nn/vector/sharding.h"
#include "nn/vector/vector_ctx.h"

namespace nn {
namespace vector {

class T2TSHARDING_ANALYSIS_CTX : public air::base::ANALYZE_CTX {
public:
  T2TSHARDING_ANALYSIS_CTX(air::base::CONTAINER*          cont,
                           const air::driver::DRIVER_CTX* driver_ctx,
                           const VECTOR_CONFIG& cfg, SHARDING_MAP* sharding)
      : _cntr(cont),
        _driver_ctx(driver_ctx),
        _config(cfg),
        _sharding(sharding) {
    sharding->Set_cntr(cont);
  }

  air::base::CONTAINER* Container() const { return _cntr; }
  SHARDING_MAP*         Sharding_map() const { return _sharding; }

  // declare access API for VECTOR_CONFIG
  DECLARE_VECTOR_CONFIG_ACCESS_API(_config)

  // declare trace API for detail tracing
  DECLARE_TRACE_DETAIL_API(_config, _driver_ctx)

private:
  air::base::CONTAINER*          _cntr;
  const air::driver::DRIVER_CTX* _driver_ctx;
  const VECTOR_CONFIG&           _config;
  SHARDING_MAP*                  _sharding;
};

}  // namespace vector
}  // namespace nn

#endif  // NN_SHARDING_ANALYSIS_CTX_H
