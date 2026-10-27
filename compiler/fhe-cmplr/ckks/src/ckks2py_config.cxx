//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/ckks/ckks2py_config.h"

using namespace air::base;
using namespace air::util;

namespace fhe {
namespace ckks {

// Static singleton populated by the option parser before Update_options().
static CKKS2PY_CONFIG Ckks2py_config;

static OPTION_DESC Ckks2py_option[] = {
    DECLARE_COMMON_CONFIG(Ckks2py_config),
    {"rt", "rt_module",
     "Python runtime module under rtlib_py baked into the emitted "
     "'from rtlib_py.<module> import *' (rt_ref | rt_openfhe | rt_fhert). "
     "Activates the pass. The emitted 'provider' tag is derived from this "
     "(rt_openfhe/rt_fhert -> LIB_OPENFHE; rt_ref -> rt_ref). Default: unset "
     "(pass inactive; default C output).",
     &Ckks2py_config._py_rt_module, air::util::K_STR, 0, V_EQUAL},
    {"df", "data_file", "Store weight data in a separated file",
     &Ckks2py_config._data_file, air::util::K_STR, 0, V_EQUAL},
    {"fp", "free_poly",
     "Insert Free_cipher right after the last use of a cipher/plain",
     &Ckks2py_config._free_poly, air::util::K_NONE, 0, V_NONE},
};

static OPTION_DESC_HANDLE Ckks2py_option_handle = {
    sizeof(Ckks2py_option) / sizeof(Ckks2py_option[0]), Ckks2py_option};

static OPTION_GRP Ckks2py_option_grp = {"CKKS2PY",
                                        "Translate CKKS IR to Python code",
                                        ':',
                                        air::util::V_EQUAL,
                                        &Ckks2py_option_handle};

void CKKS2PY_CONFIG::Register_options(air::driver::DRIVER_CTX* ctx) {
  ctx->Register_option_group(&Ckks2py_option_grp);
}

// Computes _provider from -CKKS2PY:rt= only. The registered CKKS2PY
// sub-flags are rt / df / fp (see Ckks2py_option[] above); -CKKS2PY:lib= is
// not among them, so the air parser rejects any -CKKS2PY:lib=* token. The
// emitted 'provider' tag is derived from this via ctx::Provider_tag
// (rt_openfhe/rt_fhert -> LIB_OPENFHE; rt_ref and other -> the honest module
// name). No -CKKS2PY:rt= given -> _provider=INVALID -> pass inactive
// (default C output, byte-identical).
void CKKS2PY_CONFIG::Update_options() {
  *this = Ckks2py_config;
  if (_py_rt_module.empty()) {
    _provider = core::PROVIDER::INVALID;   // no -CKKS2PY:rt= given -> inactive
  } else if (_py_rt_module == "rt_openfhe" || _py_rt_module == "rt_fhert") {
    _provider = core::PROVIDER::OPENFHE;   // tag -> "LIB_OPENFHE"
  } else {
    _provider = core::PROVIDER::INVALID;   // rt_ref etc -> Provider_tag fallback
  }
}

}  // namespace ckks
}  // namespace fhe