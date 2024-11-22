//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_LLAMA_CONFIG_H
#define NN_LLAMA_CONFIG_H

#include "air/driver/common_config.h"
#include "air/driver/driver_ctx.h"

namespace nn {
namespace llama {

struct LLAMA_CONFIG : public air::util::COMMON_CONFIG {
public:
  LLAMA_CONFIG(void) {}

  void Register_options(air::driver::DRIVER_CTX* ctx);
  void Update_options();

  void Print(std::ostream& os) const;

};  // struct LLAMA_CONFIG

}  // namespace llama
}  // namespace nn

#endif  // NN_LLAMA_CONFIG_H
