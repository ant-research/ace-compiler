//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/poly/config.h"

#include "fhe/poly/option_config.inc"
namespace fhe {
namespace poly {

void POLY_CONFIG::Update_options() {
  POLY_OPTION_CONFIG::Update_options();
  Pre_process_options();
}

void POLY_CONFIG::Pre_process_options() {
  if (Hoist_mdup() || Hoist_mdown()) {
    _lower_to_hpoly = true;
    _lower_to_lpoly = true;
  }
  if (Lower_to_hpoly()) {
    // TODO: support relin/rotate function call
    _inline_rotate = true;
    _inline_relin  = true;
  }
}

}  // namespace poly
}  // namespace fhe
