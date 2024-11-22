//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn/vector/config.h"

using namespace air::base;
using namespace air::util;

namespace nn {
namespace vector {

static VECTOR_CONFIG Vector_config;

static OPTION_DESC Vector_option[] = {
    DECLARE_COMMON_CONFIG(vector, Vector_config),
    DECLARE_VECTOR_CONFIG(vector, Vector_config)};

static OPTION_DESC_HANDLE Vector_option_handle = {
    sizeof(Vector_option) / sizeof(Vector_option[0]), Vector_option};

static OPTION_GRP Vector_option_grp = {
    "VEC", "Vector Intermediate Representation", ':', air::util::V_EQUAL,
    &Vector_option_handle};

void VECTOR_CONFIG::Register_options(air::driver::DRIVER_CTX* ctx) {
  ctx->Register_option_group(&Vector_option_grp);
}

void VECTOR_CONFIG::Update_options() { *this = Vector_config; }

void VECTOR_CONFIG::Print(std::ostream& os) const { COMMON_CONFIG::Print(os); }

}  // namespace vector
}  // namespace nn
