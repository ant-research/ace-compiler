//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn/vector/vec2c_config.h"

using namespace air::base;
using namespace air::util;

namespace nn {

namespace vector {

static VEC2C_CONFIG Vec2c_config;

static OPTION_DESC Vec2c_option[] = {DECLARE_COMMON_CONFIG(Vec2c_config),
                                     DECLARE_VEC2C_CONFIG(Vec2c_config)};

static OPTION_DESC_HANDLE Vec2c_option_handle = {
    sizeof(Vec2c_option) / sizeof(Vec2c_option[0]), Vec2c_option};

static OPTION_GRP Vec2c_option_grp = {"VEC2C", "Translate VEC IR to C code",
                                      ':', air::util::V_EQUAL,
                                      &Vec2c_option_handle};

void VEC2C_CONFIG::Register_options(air::driver::DRIVER_CTX* ctx) {
  ctx->Register_option_group(&Vec2c_option_grp);
}

void VEC2C_CONFIG::Update_options() { *this = Vec2c_config; }

void VEC2C_CONFIG::Print(std::ostream& os) const { COMMON_CONFIG::Print(os); }

}  // namespace vector

}  // namespace nn
