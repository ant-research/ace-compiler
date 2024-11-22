//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/poly/config.h"

using namespace air::base;
using namespace air::util;

namespace fhe {
namespace poly {

static POLY_CONFIG Poly_config;

static OPTION_DESC Poly_option[] = {DECLARE_COMMON_CONFIG(poly, Poly_config),
                                    DECLARE_POLY_CONFIG(poly, Poly_config)};

static OPTION_DESC_HANDLE Poly_option_handle = {
    sizeof(Poly_option) / sizeof(Poly_option[0]), Poly_option};

static OPTION_GRP Poly_option_grp = {
    "POLY", "Polynomial Intermediate Representation", ':', air::util::V_EQUAL,
    &Poly_option_handle};

void POLY_CONFIG::Register_options(air::driver::DRIVER_CTX* ctx) {
  ctx->Register_option_group(&Poly_option_grp);
}

void POLY_CONFIG::Update_options() { *this = Poly_config; }

void POLY_CONFIG::Print(std::ostream& os) const { COMMON_CONFIG::Print(os); }

}  // namespace poly
}  // namespace fhe
