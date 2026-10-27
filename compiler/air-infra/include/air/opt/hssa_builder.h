//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_HSSA_BUILD_H
#define AIR_OPT_HSSA_BUILD_H

#include <stack>

#include "air/base/analyze_ctx.h"
#include "air/base/visitor.h"
#include "air/core/handler.h"
#include "air/driver/driver_ctx.h"
#include "air/opt/cfg.h"
#include "air/opt/hssa_build_ctx.h"
#include "air/opt/hssa_container.h"
#include "air/opt/ssa_container.h"

namespace air {

namespace opt {

class HSSA_CONFIG {};

class HSSA_BUILDER {
public:
  HSSA_BUILDER(air::base::FUNC_SCOPE* scope, HCONTAINER& hssa_cont,
               SSA_CONTAINER& ssa_cont, CFG& cfg,
               const driver::DRIVER_CTX* driver_ctx)
      : _scope(scope),
        _hssa_cont(hssa_cont),
        _ssa_cont(ssa_cont),
        _cfg(cfg),
        _driver_ctx(driver_ctx),
        _config(),
        _cprop(false) {}

  bool Cprop(void) const { return _cprop; }

  template <typename HSSA_VISITOR>
  void Run(HSSA_VISITOR& visitor) {
    visitor.Context().Init(&_cfg, &_hssa_cont, &_ssa_cont, Cprop());
    air::base::NODE_PTR body = _scope->Container().Entry_node();
    visitor.template Visit<HEXPR_PTR>(body);
  }

private:
  air::base::FUNC_SCOPE*    _scope;
  SSA_CONTAINER&            _ssa_cont;
  HCONTAINER&               _hssa_cont;
  CFG&                      _cfg;
  const driver::DRIVER_CTX* _driver_ctx;
  HSSA_CONFIG               _config;
  bool                      _cprop;
};

}  // namespace opt
}  // namespace air
#endif