//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_ONNX2AIR_CONFIG_H
#define NN_ONNX2AIR_CONFIG_H

#include "air/driver/common_config.h"
#include "air/driver/driver_ctx.h"

namespace nn {
namespace onnx2air {

struct ONNX2AIR_CONFIG : public air::util::COMMON_CONFIG {
public:
  ONNX2AIR_CONFIG(void) {}

  void Register_options(air::driver::DRIVER_CTX* ctx);
  void Update_options();

  void Print(std::ostream& os) const;

};  // struct ONNX2AIR_CONFIG

}  // namespace onnx2air
}  // namespace nn

#endif  // NN_ONNX2AIR_CONFIG_H
