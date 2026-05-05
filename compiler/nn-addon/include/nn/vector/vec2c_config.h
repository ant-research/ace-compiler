//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_VEC2C_CONFIG_H
#define NN_VECTOR_VEC2C_CONFIG_H

#include "air/driver/common_config.h"
#include "air/driver/driver_ctx.h"

namespace nn {

namespace vector {

struct VEC2C_CONFIG : public air::util::COMMON_CONFIG {
public:
  VEC2C_CONFIG(void) {}

  void Register_options(air::driver::DRIVER_CTX* ctx);
  void Update_options();

  void Print(std::ostream& os) const;

};  // struct VECTOR_CONFIG

//! @brief Macro to define API to access VEC2C config
#define DECLARE_VEC2C_CONFIG_ACCESS_API(cfg)

#define DECLARE_VEC2C_CONFIG(cfg)

}  // namespace vector

}  // namespace nn

#endif  // NN_VECTOR_VEC2C_CONFIG_H
