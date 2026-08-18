//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/ckks/config.h"

#include <complex.h>

#include "air/util/option.h"

using namespace air::base;
using namespace air::util;

namespace fhe {
namespace ckks {

static CKKS_CONFIG Ckks_config;

static OPTION_DESC Ckks_option[] = {
    DECLARE_COMMON_CONFIG(ckks_gen, Ckks_config),
    {"secret_key_hamming_weight",       "sk_hw",
                             "Hamming weight used in generating secret key",                                           &Ckks_config._secret_key_hamming_weight, air::util::K_UINT64, 0, V_EQUAL},
    {"first_q_bit_num",                 "q0",              "Bit number of first q prime",      &Ckks_config._q0,
                             air::util::K_UINT64,                                                                                                                                    0, V_EQUAL},
    {"scale_factor_bit_num",            "sf",              "Bit number of scale factor",
                             &Ckks_config._sf,                                                                                                                  air::util::K_UINT64, 0, V_EQUAL},
    {"poly_degree",                     "N",               "Poly degree",                      &Ckks_config._poly_deg,
                             air::util::K_UINT64,                                                                                                                                    0, V_EQUAL},
    {"input_cipher_level",              "in_lev",          "Level of input ciphertext",
                             &Ckks_config._input_lev,                                                                                                           air::util::K_UINT64, 0, V_EQUAL},
    {"max_bootstrap_level",             "lbts",            "Max resulting level of bootstrap",
                             &Ckks_config._lev_bts,                                                                                                             air::util::K_UINT64, 0, V_EQUAL},
    {"optimize_bootstrap_result_level", "opt_bts_res_lev",
                             "Set resulting ciphertext level of bootstrapping as the consumed value",
                             &Ckks_config._opt_bts_res_lev,                                                                                                     air::util::K_UINT64, 0, V_EQUAL},
    {"set_const_encode_level",          "cst_enc_lev",
                             "Set resulting plaintext level of encode as const",                                       &Ckks_config._cst_encode_lev,            air::util::K_UINT64, 0, V_EQUAL},
    {"set_const_encode_scale",          "cst_enc_scale",
                             "Set resulting plaintext scale of encode as const",                                       &Ckks_config._cst_encode_scale,          air::util::K_UINT64, 0, V_EQUAL},
    {"rescale_phi_result",              "rs_phi_res",      "Rescale result of phi nodes",
                             &Ckks_config._rescale_phi_res,                                                                                                     air::util::K_UINT64, 0, V_EQUAL},
    {"region_base_scale_bts_opt",       "resbm",
                             "Optimize scale and bootstrap with regioned dfg",                                         &Ckks_config._resbm,
                             air::util::K_UINT64,                                                                                                                                    0, V_EQUAL},
    {"trace_region_base_scale_bts_opt", "tr",              "Trace RESBM",
                             &Ckks_config._trace_resbm,                                                                                                         air::util::K_UINT64, 0, V_EQUAL},
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
  os << "  Level of input ciphertext:   " << Input_lev() << std::endl;
  os << "  Optimize bootstrapping result level: " << Opt_bts_res_level()
     << std::endl;
  os << "  Encode plaintext with const level: " << Const_encode_level()
     << std::endl;
  os << "  Encode plaintext with const scale: " << Const_encode_scale()
     << std::endl;
  os << "  Rescale phi result: " << Rescale_phi_res() << std::endl;
  os << "  Perform RESBM: " << Resbm() << std::endl;
  os << "  Trace RESBM: " << Trace_resbm() << std::endl;
  os << "  Max bootstrapping level: " << Max_bts_lev() << std::endl;
}

}  // namespace ckks
}  // namespace fhe
