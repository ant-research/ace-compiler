//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_SIHE2CKKS_LOWER_H
#define FHE_CKKS_SIHE2CKKS_LOWER_H

#include "air/base/container.h"
#include "air/base/handler_retv.h"
#include "air/base/st.h"
#include "air/base/visitor.h"
#include "air/core/handler.h"
#include "fhe/ckks/ckks_gen.h"
#include "fhe/ckks/config.h"
#include "fhe/ckks/default_handler.h"
#include "fhe/ckks/sihe2ckks_ctx.h"
#include "fhe/ckks/sihe2ckks_impl.h"
#include "fhe/sihe/sihe_handler.h"

namespace fhe {
namespace ckks {
using namespace air::base;
using namespace air::core;
using namespace fhe::sihe;

class SIHE2CKKS_LOWER {
public:
  using CORE_HANDLER  = air::core::HANDLER<air::core::DEFAULT_HANDLER>;
  using SIHE_HANDLER  = sihe::HANDLER<SIHE2CKKS_IMPL>;
  using LOWER_VISITOR = VISITOR<SIHE2CKKS_CTX, CORE_HANDLER, SIHE_HANDLER>;

  SIHE2CKKS_LOWER(GLOB_SCOPE* glob_scope, core::LOWER_CTX* ctx,
                  const CKKS_CONFIG* config)
      : _glob_scope(glob_scope), _lower_ctx(ctx), _config(config) {}

  FUNC_SCOPE& Lower_server_func(FUNC_SCOPE* sihe_func_scope) {
    CMPLR_ASSERT(_glob_scope != &sihe_func_scope->Glob_scope(),
                 "cannot lower function into its own global scope");

    FUNC_PTR    sihe_func       = sihe_func_scope->Owning_func();
    FUNC_SCOPE& ckks_func_scope = _glob_scope->New_func_scope(sihe_func->Id());
    ckks_func_scope.Clone(*sihe_func_scope);
    CONTAINER* ckks_container = &ckks_func_scope.Container();

    SIHE2CKKS_CTX trav_ctx(ckks_container, *_lower_ctx, *_config);
    LOWER_VISITOR visitor(trav_ctx);

    NODE_PTR     node = sihe_func_scope->Container().Entry_node();
    HANDLER_RETV retv = visitor.Visit<HANDLER_RETV>(node);
    AIR_ASSERT(retv.Node() != air::base::Null_ptr && retv.Node()->Is_entry());

    ckks_func_scope.Set_entry_stmt(retv.Node()->Stmt());

    return ckks_func_scope;
  }

private:
  GLOB_SCOPE*        _glob_scope;
  core::LOWER_CTX*   _lower_ctx;
  const CKKS_CONFIG* _config;
};

}  // namespace ckks
}  // namespace fhe
#endif
