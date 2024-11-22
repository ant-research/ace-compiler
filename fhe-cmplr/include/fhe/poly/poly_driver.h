//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_POLY_POLY_DRIVER_H
#define FHE_POLY_POLY_DRIVER_H

#include "air/base/container.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/poly/config.h"

namespace fhe {

namespace poly {

/**
 * @brief Polynomial Main Driver
 *
 */
class POLY_DRIVER {
public:
  POLY_DRIVER() {}

  air::base::GLOB_SCOPE* Run(POLY_CONFIG& config, air::base::GLOB_SCOPE* glob,
                             core::LOWER_CTX& lower_ctx);
};

}  // namespace poly

}  // namespace fhe

#endif  // FHE_POLY_POLY_DRIVER_H
