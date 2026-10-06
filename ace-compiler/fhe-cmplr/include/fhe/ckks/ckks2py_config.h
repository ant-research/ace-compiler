//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_CKKS2PY_CONFIG_H
#define FHE_CKKS_CKKS2PY_CONFIG_H

#include <string>

#include "air/driver/common_config.h"
#include "air/driver/driver_ctx.h"
#include "air/util/debug.h"
#include "air/util/option.h"
#include "fhe/core/lib_provider.h"
#include "fhe/poly/poly2c_config.h"

namespace fhe {
namespace ckks {

//! @brief Config for the ckks IR -> Python backend.
//!
//! CKKS2PY_CONFIG inherits POLY2C_CONFIG so that the existing IR2C_CTX chain
//! (which stores `const POLY2C_CONFIG& _config`, see fhe/core/ir2c_ctx.h) can
//! bind to a CKKS2PY_CONFIG and read provider / data_file / config-file
//! accessors unchanged. The Python backend only overrides option *parsing*
//! (its own `-CKKS2PY:` option group) and the inactive-by-default provider: no
//! `-CKKS2PY:rt=` means "emit nothing", keeping the default C output
//! byte-identical.
//!
//! Unlike POLY2C, the Python backend mirrors the non-ant path only (it emits
//! directly from CKKS IR and never lowers to poly IR).
struct CKKS2PY_CONFIG : public fhe::poly::POLY2C_CONFIG {
public:
  CKKS2PY_CONFIG(void) : fhe::poly::POLY2C_CONFIG() {
    // Inactive by default: empty _py_rt_module means no -CKKS2PY:rt= was given.
    // -CKKS2PY:lib= is not a recognized sub-flag (the pass is activated by
    // -CKKS2PY:rt= alone); no _prov_str plumbing on the Python path.
    _provider = fhe::core::PROVIDER::INVALID;
    // Empty default = no -CKKS2PY:rt= given -> pass inactive. A non-empty rt
    // activates the pass and selects the rtlib_py provider; the emitted
    // 'provider' tag is synthesized from it (ctx::Provider_tag in ir2py_ctx.h).
    _py_rt_module = "";
  }

  void Register_options(air::driver::DRIVER_CTX* ctx);
  void Update_options();

  //! @brief True when no `-CKKS2PY:rt=` was given. This is the pass-inactivity
  //! condition checked in Pre_run (the pass activates on -CKKS2PY:rt= only).
  bool Py_rt_empty() const { return _py_rt_module.empty(); }

  //! @brief The rtlib_py submodule the emitted module imports as its runtime
  //! (`from rtlib_py.<Py_rt_module()> import *`). Empty by default -> pass
  //! inactive; set via `-CKKS2PY:rt=<rt_ref|rt_openfhe|rt_fhert>` to activate
  //! and select the provider. This is the ONLY user-facing compiler surface
  //! added by Phase 2.0.
  const char* Py_rt_module() const { return _py_rt_module.c_str(); }

  // leave this member public so that OPTION_DESC can access it (inherits the
  // provider/data-file accessors from POLY2C_CONFIG, the C POLY2C base; no
  // separate provider-string field is plumbed on the python path).
  std::string _py_rt_module;  // rtlib_py submodule name (empty default; rt_ref fallback in ctx::Py_rt_or_default)
};

}  // namespace ckks
}  // namespace fhe

#endif  // FHE_CKKS_CKKS2PY_CONFIG_H