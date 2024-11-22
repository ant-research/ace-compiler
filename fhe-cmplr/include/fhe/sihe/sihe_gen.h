//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CORE_SIHE_GEN_H
#define FHE_CORE_SIHE_GEN_H

#include "air/base/container.h"
#include "air/base/st.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/sihe/config.h"
#include "fhe/sihe/sihe_opcode.h"

namespace fhe {
namespace sihe {

using namespace air::base;
using OPCODE = air::base::OPCODE;

enum class SIHE_TYPE_KIND {
  PLAIN  = 0,
  CIPHER = 1,
  LAST   = 2,
};

class SIHE_GEN {
public:
  SIHE_GEN(GLOB_SCOPE* glob_scope, core::LOWER_CTX* ctx)
      : _glob_scope(glob_scope), _lower_ctx(ctx), _cntr(nullptr) {}

  SIHE_GEN(CONTAINER* cntr, core::LOWER_CTX* ctx)
      : _glob_scope(cntr->Glob_scope()), _lower_ctx(ctx), _cntr(cntr) {}

  ~SIHE_GEN() {}

  NODE_PTR Gen_add(NODE_PTR child0, NODE_PTR child1, const SPOS& spos);
  NODE_PTR Gen_mul(NODE_PTR child0, NODE_PTR child1, const SPOS& spos);
  NODE_PTR Gen_sub(NODE_PTR child0, NODE_PTR child1, const SPOS& spos);
  NODE_PTR Gen_encode(NODE_PTR child, TYPE_PTR plain_type, const SPOS& spos);
  NODE_PTR Gen_bootstrap(NODE_PTR child, const SPOS& spos);
  void     Register_sihe_types();

private:
  // REQUIRED UNDEFINED UNWANTED methods
  SIHE_GEN(void);
  SIHE_GEN(const SIHE_GEN&);
  SIHE_GEN& operator=(const SIHE_GEN&);

  GLOB_SCOPE*      Glob_scope() const { return _glob_scope; }
  core::LOWER_CTX* Lower_ctx() const { return _lower_ctx; }
  CONTAINER*       Container() const { return _cntr; }

  GLOB_SCOPE*      _glob_scope;
  core::LOWER_CTX* _lower_ctx;
  CONTAINER*       _cntr;
};

air::base::GLOB_SCOPE* Sihe_driver(air::base::GLOB_SCOPE*   glob,
                                   core::LOWER_CTX*         lower_ctx,
                                   air::driver::DRIVER_CTX* driver_ctx,
                                   const SIHE_CONFIG&       cfg);

}  // namespace sihe
}  // namespace fhe

#endif
