//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_SSA_BUILD_H
#define AIR_OPT_SSA_BUILD_H

#include "air/base/container.h"
#include "air/base/st.h"
#include "air/driver/driver_ctx.h"
#include "air/opt/ssa_container.h"

//! @brief SSA BUILDER
//! There are two SSA BUILDER. One is SIMPLE BUILDER and the other is COMPLEX
//! BUILDER.
//!
//! @brief SSA SIMPLE BUILDER
//! This is a simple builder with the following assumptions:
//!   - No pointer. No alias analyze needed.
//!   - No irregular control flow. Only well-formed LOOP/IF are allowed
//!
//! Under these assumptions, the SSA construction can be very quick:
//!   - Construct SSA Symbol Table by traversing IR once
//!   - Insert PHI for each def
//!   - Rename each def and set up U-D by traversing IR once
//!   - (Optional) Dead store elimination to mark unused PHI_NODE dead
//!
//! No alias info. No CFG/DOM/DF.
//!
//! @brief SSA COMPLEX BUILDER
//! TODO: Add alias analysis and CFG/DOM/DF calculation
//!

namespace air {

namespace opt {

enum SSA_TRACE_DETAIL {
  TRACE_IR_BEFORE_SSA       = 0,
  TRACE_IR_AFTER_INSERT_PHI = 1,
  TRACE_IR_AFTER_SSA        = 2,
};

//! @brief Config for building SSA
class SSA_CONFIG {
public:
  SSA_CONFIG(uint32_t td) : _trace_detail(td) {}
  ~SSA_CONFIG() {}

  void Set_trace_ir_before_ssa(bool val) {
    _trace_detail |= (((uint32_t)val) << TRACE_IR_BEFORE_SSA);
  }
  void Set_trace_ir_after_insert_phi(bool val) {
    _trace_detail |= (((uint32_t)val) << TRACE_IR_AFTER_INSERT_PHI);
  }
  void Set_trace_ir_after_ssa(bool val) {
    _trace_detail |= (((uint32_t)val) << TRACE_IR_AFTER_SSA);
  }
  bool Is_trace(uint32_t flag) const {
    return (_trace_detail & (1U << flag)) != 0;
  }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  SSA_CONFIG(void);
  SSA_CONFIG(const SSA_CONFIG&);
  SSA_CONFIG operator=(const SSA_CONFIG&);

  uint32_t _trace_detail = 0;
};

//
// TODO: possible share code with complex ssa builder
//

//! @brief Build SSA on top of AIR IR
class SSA_BUILDER {
public:
  SSA_BUILDER(air::base::FUNC_SCOPE* scope, SSA_CONTAINER* cont,
              const driver::DRIVER_CTX* driver_ctx)
      : _scope(scope), _cont(cont), _driver_ctx(driver_ctx), _config(0) {}

  void Perform() { Build_simple(); }

  const SSA_CONFIG& Ssa_config() const { return _config; }
  SSA_CONFIG&       Ssa_config() { return _config; }
  DECLARE_TRACE_DETAIL_API(_config, _driver_ctx)

protected:
  void Build_simple();

  void Build_complex() { AIR_ASSERT(false); }

  air::base::FUNC_SCOPE*    _scope;
  SSA_CONTAINER*            _cont;
  const driver::DRIVER_CTX* _driver_ctx;
  SSA_CONFIG                _config;
};

}  // namespace opt

}  // namespace air

#endif  // AIR_OPT_SSA_BUILD_H
