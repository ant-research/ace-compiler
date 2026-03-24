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
#include "air/driver/driver_ctx.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/poly/config.h"

namespace fhe {

namespace poly {

enum POLY_LAYER {
  HPOLY,
  HPOLY_P1 = HPOLY,  // high poly abstraction phase I for rns operation
  HPOLY_P2,          // high poly abstraction phase II for rns operation
  LPOLY,             // low poly abstraction for decomposed rns operation
  SPOLY,             // single poly abstraction
};

//! @brief Polynomial Main Driver
class POLY_DRIVER {
public:
  POLY_DRIVER() {}

  air::base::GLOB_SCOPE* Run(POLY_CONFIG& config, air::base::GLOB_SCOPE* glob,
                             core::LOWER_CTX&         lower_ctx,
                             air::driver::DRIVER_CTX* driver_ctx);

private:
  air::base::GLOB_SCOPE* Lower_to_poly(POLY_CONFIG&             config,
                                       air::base::GLOB_SCOPE*   glob,
                                       air::driver::DRIVER_CTX* driver_ctx,
                                       core::LOWER_CTX&         lower_ctx,
                                       POLY_LAYER               target_layer);

  air::base::GLOB_SCOPE* Run_mup_opt(POLY_CONFIG&             config,
                                     air::base::GLOB_SCOPE*   glob,
                                     air::driver::DRIVER_CTX* driver_ctx);

  air::base::GLOB_SCOPE* Run_mdown_opt(POLY_CONFIG&             config,
                                       air::base::GLOB_SCOPE*   glob,
                                       air::driver::DRIVER_CTX* driver_ctx,
                                       core::LOWER_CTX*         lower_ctx);

  air::base::GLOB_SCOPE* Run_op_fusion_opt(POLY_CONFIG&             config,
                                           air::base::GLOB_SCOPE*   glob,
                                           air::driver::DRIVER_CTX* driver_ctx,
                                           core::LOWER_CTX*         lower_ctx);

  air::base::GLOB_SCOPE* Run_flatten(air::base::GLOB_SCOPE* glob,
                                     POLY_LAYER             tgt_layer);

  void Run_attr_prop(POLY_CONFIG& config, air::base::GLOB_SCOPE* glob,
                     air::driver::DRIVER_CTX* driver_ctx,
                     core::LOWER_CTX&         lower_ctx);

  air::base::GLOB_SCOPE* Clone_glob(air::base::GLOB_SCOPE* src_glob);

  void Verify_hpoly(POLY_CONFIG& config, air::base::GLOB_SCOPE* glob,
                    core::LOWER_CTX* lower_ctx);
};

}  // namespace poly

}  // namespace fhe

#endif  // FHE_POLY_POLY_DRIVER_H
