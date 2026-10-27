//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/poly/poly2c_config.h"

using namespace air::base;
using namespace air::util;

namespace fhe {
namespace poly {

static POLY2C_CONFIG Poly2c_config;

static OPTION_DESC Poly2c_option[] = {
    DECLARE_COMMON_CONFIG(Poly2c_config),
    {"lib", "",
                     "FHE library used by generated code: ant, seal, openfhe, phantom, "
     "ace",                                                                                     &Poly2c_config._prov_str,          air::util::K_STR,  0, V_EQUAL},
    {"df",  "data_file",         "Store weight data in a seperated file",
                     &Poly2c_config._data_file,                                                                                    air::util::K_STR,  0, V_EQUAL},
    {"fp",  "free_poly",
                     "Insert Free_poly right after the last use of the poly or poly in cipher",
                     &Poly2c_config._free_poly,                                                                                    air::util::K_NONE, 0, V_NONE },
    {"fgk", "free_galois_key",
                     "Insert Free_galois_key(step) after the last use of each rotation step "
                     "(on-demand galois-key liveness-driven free; no-op unless KeyMode::ON_DEMAND). "
                     "Legacy bool: true→last_use, false→off. Prefer -P2C:gkfm for explicit control.",
                     &Poly2c_config._free_galois_key,                                                                             air::util::K_NONE, 0, V_NONE },
    {"gkfm", "galois_free_mode",
                     "Galois-key free strategy: off | use_and_free | last_use (default last_use). "
                     "last_use = load each distinct step once before first use, free once after last "
                     "use (program-level liveness via RNUM). use_and_free = legacy F1 (free after each "
                     "non-constant rotate; reloads on recurrence). off = never free (resident whole "
                     "session; release_session reclaims).",
                     &Poly2c_config._gk_fm_str,                                                                                    air::util::K_STR,  0, V_EQUAL},
    {"bes", "bts_explicit_slot",
                     "Emit Eval_bootstrap_ciph with explicit slot parameter instead of "
                     "Bootstrap",                                                               &Poly2c_config._bts_explicit_slot, air::util::K_NONE, 0, V_NONE },
};

static OPTION_DESC_HANDLE Poly2c_option_handle = {
    sizeof(Poly2c_option) / sizeof(Poly2c_option[0]), Poly2c_option};

static OPTION_GRP Poly2c_option_grp = {"P2C", "Translate POLY IR to C code",
                                       ':', air::util::V_EQUAL,
                                       &Poly2c_option_handle};

void POLY2C_CONFIG::Register_options(air::driver::DRIVER_CTX* ctx) {
  ctx->Register_option_group(&Poly2c_option_grp);
}

void POLY2C_CONFIG::Update_options() {
  *this     = Poly2c_config;
  _provider = core::Provider_id(_prov_str.c_str());

  // Resolve galois-key free mode: explicit -P2C:gkfm wins; else legacy -P2C:fgk
  // bool (true→last_use, false→off); else constructor default (last_use).
  if (_gk_fm_str == "off" || _gk_fm_str == "OFF") {
    _gk_free_mode = Gk_free_mode::OFF;
  } else if (_gk_fm_str == "use_and_free" || _gk_fm_str == "USE_AND_FREE") {
    _gk_free_mode = Gk_free_mode::USE_AND_FREE;
  } else if (_gk_fm_str == "last_use" || _gk_fm_str == "LAST_USE") {
    _gk_free_mode = Gk_free_mode::LAST_USE;
  } else if (!_gk_fm_str.empty()) {
    AIR_ASSERT_MSG(false, "unknown -P2C:gkfm value '%s' (off|use_and_free|last_use)",
                   _gk_fm_str.c_str());
  } else {
    _gk_free_mode = _free_galois_key ? Gk_free_mode::LAST_USE : Gk_free_mode::OFF;
  }
}

void POLY2C_CONFIG::Print(std::ostream& os) const { COMMON_CONFIG::Print(os); }

}  // namespace poly
}  // namespace fhe
