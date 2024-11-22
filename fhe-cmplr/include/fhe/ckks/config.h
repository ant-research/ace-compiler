//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_CONFIG_H
#define FHE_CKKS_CONFIG_H

#include "air/driver/common_config.h"
#include "air/driver/driver_ctx.h"

namespace fhe {
namespace ckks {

enum TRACE_DETAIL {
  TRACE_CKKS_ANA_RES            = 0,
  TRACE_CKKS_TRAN_RES           = 1,
  TRACE_IR_BEFORE_SSA           = 2,
  TRACE_IR_AFTER_SSA_INSERT_PHI = 3,
  TRACE_IR_AFTER_SSA            = 4,
};

struct CKKS_CONFIG : public air::util::COMMON_CONFIG {
public:
  CKKS_CONFIG(void) {}

  void Register_options(air::driver::DRIVER_CTX* ctx);
  void Update_options();

  void     Print(std::ostream& os) const;
  uint64_t Hamming_weight() const { return _secret_key_hamming_weight; }
  uint32_t Q0_bit_num() const { return _q0; }
  uint32_t Scale_factor_bit_num() const { return _sf; }
  uint32_t Poly_deg() const { return _poly_deg; }
  // leave this member public so that OPTION_DESC can access it
  uint64_t _secret_key_hamming_weight = 0;
  uint32_t _q0                        = 0;
  uint32_t _sf                        = 0;
  uint32_t _poly_deg                  = 0;
};

//! @brief Macro to define API to access CKKS config
#define DECLARE_CKKS_CONFIG_ACCESS_API(cfg)                                    \
  uint64_t Hamming_weight() const { return cfg.Hamming_weight(); }             \
  uint64_t Q0_bit_num() const { return cfg.Q0_bit_num(); }                     \
  uint64_t Scale_factor_bit_num() const { return cfg.Scale_factor_bit_num(); } \
  uint64_t Poly_deg() const { return cfg.Poly_deg(); }                         \
  DECLARE_COMMON_CONFIG_ACCESS_API(cfg)

}  // namespace ckks
}  // namespace fhe

#endif  // FHE_CKKS_CONFIG_H
