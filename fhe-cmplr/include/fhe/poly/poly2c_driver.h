//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_POLY_POLY2C_DRIVER_H
#define FHE_POLY_POLY2C_DRIVER_H

#include "air/base/container.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/poly/ir2c_ctx.h"

namespace fhe {

namespace poly {

/**
 * @brief Polynomial To C Main Driver
 *
 */
class POLY2C_DRIVER {
public:
  POLY2C_DRIVER(std::ostream& os, core::LOWER_CTX& lower_ctx,
                const fhe::poly::POLY2C_CONFIG& cfg)
      : _ctx(os, lower_ctx, cfg) {}

  air::base::GLOB_SCOPE* Flatten(air::base::GLOB_SCOPE* in_scope);

  void Run(air::base::GLOB_SCOPE* in_scope);

private:
  void Emit_get_context_params();
  void Emit_helper_function(air::base::FUNC_SCOPE* func_scope);

  IR2C_CTX _ctx;
};  // POLY2C_DRIVER

}  // namespace poly

}  // namespace fhe

#endif  // FHE_POLY_POLY2C_DRIVER_H
