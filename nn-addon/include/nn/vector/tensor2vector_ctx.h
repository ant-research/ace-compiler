//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_TENSOR2VECTOR_CTX_H
#define NN_VECTOR_TENSOR2VECTOR_CTX_H

#include "air/base/transform_ctx.h"
#include "nn/vector/config.h"
#include "nn/vector/vector_ctx.h"
#include "nn/vector/vector_enum.h"
#include "nn/vector/vector_utils.h"

namespace nn {

namespace vector {

using PREG_MAP = std::unordered_map<uint32_t, uint32_t>;

// For node tracing
const auto Trace_node = [](std::ostream& os, air::base::NODE_PTR op) {
  op->Print_tree(os, true, 0);
};

// For constant float array tracing
const auto Trace_float_array = [](std::ostream& os, air::base::CONSTANT_PTR vconst,
                            std::string msg) {
  Print_array_const<float>(os, vconst, msg);
};

// For constant int array tracing
const auto Trace_int_array = [](std::ostream& os, air::base::CONSTANT_PTR vconst,
                          std::string msg) {
  Print_array_const<int>(os, vconst, msg);
};

class TENSOR2VECTOR_CTX : public air::base::TRANSFORM_CTX {
public:
  TENSOR2VECTOR_CTX(air::base::CONTAINER* cont, VECTOR_CTX& ctx,
                    const air::driver::DRIVER_CTX* driver_ctx,
                    const VECTOR_CONFIG&           cfg)
      : air::base::TRANSFORM_CTX(cont),
        _ctx(ctx),
        _driver_ctx(driver_ctx),
        _config(cfg),
        _t2v_preg_map() {}

  // declare access API for VECTOR_CTX
  DECLARE_VECTOR_CTX_ACCESS_API(_ctx)

  // declare access API for VECTOR_CONFIG
  DECLARE_VECTOR_CONFIG_ACCESS_API(_config)

  // declare trace API for detail tracing
  DECLARE_TRACE_DETAIL_API(_config, _driver_ctx)

  PREG_MAP& Get_t2v_preg_map() { return _t2v_preg_map; }

  void Insert_t2v_preg_map(std::pair<uint32_t, uint32_t> preg_pair) {
    _t2v_preg_map.insert(preg_pair);
  }

private:
  VECTOR_CTX&                    _ctx;
  const air::driver::DRIVER_CTX* _driver_ctx;
  const VECTOR_CONFIG&           _config;
  PREG_MAP                       _t2v_preg_map;
};

}  // namespace vector

}  // namespace nn

#endif  // NN_VECTOR_TENSOR2VECTOR_CTX_H
