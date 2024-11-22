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
    DECLARE_COMMON_CONFIG(poly2c, Poly2c_config),
    {"lib", "",                      "FHE library used by generated code: ant, seal, openfhe",
                             &Poly2c_config._prov_str,  air::util::K_STR,  0, V_EQUAL},
    {"df",  "data_file",             "Store weight data in a seperated file",
                             &Poly2c_config._data_file, air::util::K_STR,  0, V_EQUAL},
    {"cte", "compile-time encoding",
                             "Encode weight data into plaintext at compile-time",
                             &Poly2c_config._ct_encode, air::util::K_NONE, 0, V_NONE },
    {"fp",  "free_poly",
                             "Insert Free_poly right after the last use of the poly or poly in cipher",
                             &Poly2c_config._free_poly, air::util::K_NONE, 0, V_NONE },
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
}

void POLY2C_CONFIG::Print(std::ostream& os) const { COMMON_CONFIG::Print(os); }

}  // namespace poly
}  // namespace fhe
