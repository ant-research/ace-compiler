//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_DRIVER_FHE_PIPELINE_H
#define FHE_DRIVER_FHE_PIPELINE_H

#include "air/driver/pass_manager.h"
#include "fhe/ckks/pass.h"
#include "fhe/poly/pass.h"
#include "fhe/poly/poly2c_pass.h"
#include "fhe/sihe/pass.h"

namespace fhe {

namespace driver {

enum PASS_ID : int { SIHE, CKKS, POLY, POLY2C };

typedef ::air::driver::PASS_MANAGER<sihe::SIHE_PASS, ckks::CKKS_PASS,
                                    poly::POLY_PASS, poly::POLY2C_PASS>
    FHE_PASS_MANAGER;

}  // namespace driver

}  // namespace fhe

#endif  // FHE_DRIVER_FHE_PIPELINE_H
