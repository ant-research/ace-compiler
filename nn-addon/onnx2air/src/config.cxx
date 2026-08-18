//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn/onnx2air/config.h"

using namespace air::base;
using namespace air::util;

namespace nn {
namespace onnx2air {

static ONNX2AIR_CONFIG Onnx2air_config;

static OPTION_DESC Onnx2air_option[] = {
    DECLARE_COMMON_CONFIG(onnx2air, Onnx2air_config)};

static OPTION_DESC_HANDLE Onnx2air_option_handle = {
    sizeof(Onnx2air_option) / sizeof(Onnx2air_option[0]), Onnx2air_option};

static OPTION_GRP Onnx2air_option_grp = {"O2A", "Translate ONNX model to AIR",
                                         ':', air::util::V_EQUAL,
                                         &Onnx2air_option_handle};

void ONNX2AIR_CONFIG::Register_options(air::driver::DRIVER_CTX* ctx) {
  ctx->Register_option_group(&Onnx2air_option_grp);
}

void ONNX2AIR_CONFIG::Update_options() { *this = Onnx2air_config; }

void ONNX2AIR_CONFIG::Print(std::ostream& os) const {
  COMMON_CONFIG::Print(os);
}

}  // namespace onnx2air
}  // namespace nn
