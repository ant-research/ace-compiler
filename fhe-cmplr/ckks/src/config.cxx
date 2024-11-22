//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/ckks/config.h"

using namespace air::base;
using namespace air::util;

namespace fhe {
namespace ckks {

static CKKS_CONFIG Ckks_config;

static OPTION_DESC Ckks_option[] = {
    DECLARE_COMMON_CONFIG(ckks_gen, Ckks_config),
    {"secret_key_hamming_weight", "sk_hw",
                             "Hamming weight used in generating secret key",                      &Ckks_config._secret_key_hamming_weight, air::util::K_UINT64, 0, V_EQUAL},
    {"first_q_bit_num",           "q0",    "Bit number of first q prime", &Ckks_config._q0,
                             air::util::K_UINT64,                                                                                                               0, V_EQUAL},
    {"scale_factor_bit_num",      "sf",    "Bit number of scale factor",
                             &Ckks_config._sf,                                                                                             air::util::K_UINT64, 0, V_EQUAL},
    {"poly_degree",               "N",     "Poly degree",                 &Ckks_config._poly_deg,
                             air::util::K_UINT64,                                                                                                               0, V_EQUAL},
};

static OPTION_DESC_HANDLE Ckks_option_handle = {
    sizeof(Ckks_option) / sizeof(Ckks_option[0]), Ckks_option};

static OPTION_GRP Ckks_option_grp = {
    "CKKS", "Cheon-Kim-Kim-Song Homomorphic Encryption Scheme", ':',
    air::util::V_EQUAL, &Ckks_option_handle};

void CKKS_CONFIG::Register_options(air::driver::DRIVER_CTX* ctx) {
  ctx->Register_option_group(&Ckks_option_grp);
}

void CKKS_CONFIG::Update_options() { *this = Ckks_config; }

void CKKS_CONFIG::Print(std::ostream& os) const {
  COMMON_CONFIG::Print(os);
  os << "  Secret key hamming weight:   " << Hamming_weight() << std::endl;
  os << "  Bit number of first q prime: " << Q0_bit_num() << std::endl;
  os << "  Bit number of scale factor:  " << Scale_factor_bit_num()
     << std::endl;
  os << "  Poly degree N:               " << Poly_deg() << std::endl;
}

}  // namespace ckks
}  // namespace fhe
