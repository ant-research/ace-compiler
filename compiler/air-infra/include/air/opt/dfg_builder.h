//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_DFG_BUILDER_H
#define AIR_OPT_DFG_BUILDER_H

#include <tuple>

#include "air/base/visitor.h"
#include "air/core/handler.h"
#include "air/opt/core_dfg_handler.h"
#include "air/opt/ssa_container.h"
#include "air/util/debug.h"
#include "dfg_build_ctx.h"

namespace air {
namespace opt {

//! @brief return value of DFG builder handlers
class DFG_RETV {
public:
  using DFG_NODE_PTR = air::opt::DFG_NODE_PTR;

  DFG_RETV(void) : _node() {}
  explicit DFG_RETV(DFG_NODE_PTR node) : _node(node) {}
  ~DFG_RETV() {}

  DFG_NODE_PTR Node() const { return _node; }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  DFG_RETV(const DFG_RETV&);
  const DFG_RETV& operator=(const DFG_RETV&);

  DFG_NODE_PTR _node;
};

//! @brief Impl of DFG builder. Current impl creates DFG with SSA form IR.
//! The handler of CORE domain IR is provided by default in core_dfg_handler.h.
//! Domain specific IR handlers are optional. If a specific domain handler is
//! not present, nodes of this domain are processed in
//! DFG_BUILD_CTX::Handle_node/Handle_unknown_domain, which creates a DFG node
//! for each node and connects it to the DFG nodes of its operands. Examples of
//! DFG_BUILDER are provided in ut_dfg.cxx.
template <typename... DOMAIN_HANDLERS>
class DFG_BUILDER {
public:
  using FUNC_SCOPE    = air::base::FUNC_SCOPE;
  using SSA_CONTAINER = air::opt::SSA_CONTAINER;
  using DFG_BUILD_CTX = air::opt::DFG_BUILD_CTX;
  using CORE_HANDLER  = air::core::HANDLER<air::opt::CORE_DATA_FLOW_HANDLER>;
  using VISITOR =
      air::base::VISITOR<DFG_BUILD_CTX, CORE_HANDLER, DOMAIN_HANDLERS...>;
  using DFG = air::opt::DFG_CONTAINER::DFG;

  DFG_BUILDER(DFG* dfg, const FUNC_SCOPE* func_scope,
              const SSA_CONTAINER*           ssa_cntr,
              std::tuple<DOMAIN_HANDLERS...> domain_handler)
      : _func_scope(func_scope),
        _ssa_cntr(ssa_cntr),
        _domain_handler(domain_handler),
        _ctx(dfg, _ssa_cntr) {
    AIR_ASSERT(dfg != nullptr);
    AIR_ASSERT(func_scope != nullptr);
    AIR_ASSERT(ssa_cntr != nullptr);
  }
  ~DFG_BUILDER() {}

  R_CODE Perform() {
    // build DFG
    VISITOR visitor(
        _ctx, std::tuple_cat(std::make_tuple(CORE_HANDLER()), _domain_handler));
    air::base::NODE_PTR func_body = _func_scope->Container().Entry_node();
    (void)visitor.template Visit<DFG_RETV>(func_body);

    return R_CODE::NORMAL;
  }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  DFG_BUILDER(void);
  DFG_BUILDER(const DFG_BUILDER&);
  DFG_BUILDER operator=(const DFG_BUILDER&);

  const FUNC_SCOPE*              _func_scope;
  const SSA_CONTAINER*           _ssa_cntr;
  DFG_BUILD_CTX                  _ctx;
  std::tuple<DOMAIN_HANDLERS...> _domain_handler;
};

}  // namespace opt
}  // namespace air

#endif  // AIR_OPT_DFG_BUILDER_H