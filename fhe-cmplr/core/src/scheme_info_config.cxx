//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/core/scheme_info_config.h"

#include "air/util/debug.h"
#include "air/util/option.h"

namespace fhe {
namespace core {

static SCHEME_INFO_CONFIG Scheme_info_config;

using namespace air::util;

static OPTION_DESC Scheme_info_option[] = {
    DECLARE_COMMON_CONFIG(scheme_info, Scheme_info_config),
    {"secret_key_hamming_weight", "sk_hw",
                                       "Hamming weight used in generating secret key",
                                       &Scheme_info_config._secret_key_hamming_weight,                                                              K_UINT64, 0, V_EQUAL},
    {"security_level",            "sec_lev", "Security level of homomorphic encryption",
                                       &Scheme_info_config._security_level,                                                                         K_UINT64, 0, V_EQUAL},
    {"q0",                        "q0",      "Bit number of q0",                         &Scheme_info_config._q0, K_UINT64, 0,
                                       V_EQUAL                                                                                                                          },
    {"scale_factor",              "sf",      "Bit number of scale factor",
                                       &Scheme_info_config._sf,                                                                                     K_UINT64, 0, V_EQUAL},
};

static OPTION_DESC_HANDLE Scheme_info_option_handle = {
    sizeof(Scheme_info_option) / sizeof(Scheme_info_option[0]),
    Scheme_info_option};

static OPTION_GRP Scheme_info_option_grp = {
    "FHE_SCHEME", "Set FHE scheme parameters", ':', air::util::V_EQUAL,
    &Scheme_info_option_handle};

void SCHEME_INFO_CONFIG::Register_options(air::driver::DRIVER_CTX* ctx) {
  ctx->Register_option_group(&Scheme_info_option_grp);
}

void SCHEME_INFO_CONFIG::Update_options() { *this = Scheme_info_config; }

void SCHEME_INFO_CONFIG::Print(std::ostream& os) const {
  os << "SCHEME_INFO_CONFIG {" << std::endl;
  COMMON_CONFIG::Print(os);
  os << "  Secret key hamming weight: " << Hamming_weight() << std::endl;
  os << "  Bit num of q0:             " << Bit_num_q0() << std::endl;
  os << "  Bit num of sf:             " << Bit_num_sf() << std::endl;
  os << "  Security level:            ";
  if (Security_level() == HE_STD_NOT_SET) {
    os << "not set" << std::endl;
  } else {
    os << Security_level() << std::endl;
  }
  os << "}" << std::endl;
}

}  // namespace core
}  // namespace fhe
