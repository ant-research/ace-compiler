//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_VEC2C_DRIVER_H
#define NN_VECTOR_VEC2C_DRIVER_H

#include "air/base/container.h"
#include "nn/vector/ir2c_ctx.h"

namespace nn {

namespace vector {

class VEC2C_DRIVER {
public:
  VEC2C_DRIVER(std::ostream& os, const VEC2C_CONFIG& cfg) : _ctx(os, cfg) {}

  void Run(air::base::GLOB_SCOPE* glob);

private:
  IR2C_CTX _ctx;
};

}  // namespace vector

}  // namespace nn

#endif  // NN_VECTOR_VEC2C_DRIVER_H
