//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_CG_VISITOR_H
#define AIR_CG_VISITOR_H

#include <tuple>
#include <vector>

#include "air/cg/cgir_container.h"
#include "air/cg/cgir_util.h"

namespace air {

namespace cg {

//! @brief Top level visitor to traverse CGIR and dispatch inst to handler(s).
//  If no handler, all inst will be dispatched to CONTEXT's Handle_inst()
//  If only 1 handler is available, all inst will be dispatched to this handler.
//  If more than 1 handlers are available, inst will be dispatched according to
//  its ISA id
//! @tparam CONTEXT Context for the visitor
//! @tparam HANDLERS Handlers for all ISAs
template <typename CONTEXT, typename... HANDLERS>
class VISITOR {
  using THIS_TYPE = VISITOR<CONTEXT, HANDLERS...>;

public:
  //! @brief Construct a new VISITOR object with defult handler objects
  VISITOR(CONTEXT& ctx) : _ctx(ctx) {}

  //! @brief Construct a new VISITOR object with external handlers tuple
  //! @param ctx visitor context object
  //! @param handlers tuple of external handler objects
  VISITOR(CONTEXT& ctx, const std::tuple<HANDLERS...>&& handlers)
      : _ctx(ctx), _handlers(handlers) {}

  //! @brief Destruct VISITOR object
  ~VISITOR() { AIR_ASSERT(_ctx.Empty()); }

  //! @brief Visit each inst with BB and INST iterator
  //! @param cntr CGIR_CONTAINER to be visited
  template <typename BB_ITER, typename INST_ITER>
  void Visit(BB_ITER bb_iter) {
    for (auto&& bb : bb_iter) {
      _ctx.Begin_bb(bb);
      for (auto&& inst : INST_ITER(bb)) {
        _ctx.Begin_inst(inst);
        if constexpr (sizeof...(HANDLERS) == 0) {
          Visit_inst(inst);
        } else if constexpr (sizeof...(HANDLERS) == 1) {
          std::get<0>(_handlers).template Handle<THIS_TYPE>(this, inst);
        } else {
          Forward<0>(inst->Isa(), inst);
        }
        _ctx.End_inst(inst);
      }
      _ctx.End_bb(bb);
    }
  }

  //! @brief get CONTEXT object
  CONTEXT& Context() { return _ctx; }

private:
  // forward traverse handlers tuple and dispatch inst to handler with correct
  // isa id
  template <uint32_t I>
  void Forward(uint32_t isa, INST_PTR inst) {
    if (isa == std::get<I>(_handlers).ID) {
      std::get<I>(_handlers).template Handle<THIS_TYPE>(this, inst);
    } else if constexpr (I + 1 < sizeof...(HANDLERS)) {
      Forward<I + 1>(isa, inst);
    } else {
      Visit_inst(inst);
    }
  }

  // visit inst directly without isa handler
  void Visit_inst(INST_PTR inst) {
    return _ctx.template Handle_inst<THIS_TYPE>(this, inst);
  }

  // context for visitor
  CONTEXT& _ctx;

  // tuple of all handlers
  std::tuple<HANDLERS...> _handlers;
};  // VISITOR

}  // namespace cg

}  // namespace air

#endif  // AIR_CG_VISITOR_H
