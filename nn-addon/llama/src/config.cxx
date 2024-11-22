//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn/llama/config.h"

using namespace air::base;
using namespace air::util;

namespace nn {
namespace llama {

static LLAMA_CONFIG Llama_config;

static OPTION_DESC Llama_option[] = {
    DECLARE_COMMON_CONFIG(llama, Llama_config)};

static OPTION_DESC_HANDLE Llama_option_handle = {
    sizeof(Llama_option) / sizeof(Llama_option[0]), Llama_option};

static OPTION_GRP Llama_option_grp = {"LLAMA", "Translate Llama model to AIR",
                                      ':', air::util::V_EQUAL,
                                      &Llama_option_handle};

void LLAMA_CONFIG::Register_options(air::driver::DRIVER_CTX* ctx) {
  ctx->Register_option_group(&Llama_option_grp);
}

void LLAMA_CONFIG::Update_options() { *this = Llama_config; }

void LLAMA_CONFIG::Print(std::ostream& os) const { COMMON_CONFIG::Print(os); }

}  // namespace llama
}  // namespace nn
