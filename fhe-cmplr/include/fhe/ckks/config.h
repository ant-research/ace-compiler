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

#define DEFAULT_MAX_BTS_LEV 16

enum TRACE_DETAIL {
  TRACE_CKKS_ANA_RES            = 0,
  TRACE_CKKS_TRAN_RES           = 1,
  TRACE_IR_BEFORE_SSA           = 2,
  TRACE_IR_AFTER_SSA_INSERT_PHI = 3,
  TRACE_IR_AFTER_SSA            = 4,
  TRACE_RESBM                   = 5,
};

struct CKKS_CONFIG : public air::util::COMMON_CONFIG {
public:
  CKKS_CONFIG(void) {}

  void Register_options(air::driver::DRIVER_CTX* ctx);
  void Update_options();

  void     Print(std::ostream& os) const;
  uint64_t Hamming_weight() const { return _secret_key_hamming_weight; }
  uint64_t Q0_bit_num() const { return _q0; }
  uint64_t Scale_factor_bit_num() const { return _sf; }
  uint64_t Poly_deg() const { return _poly_deg; }
  uint64_t Input_lev() const { return _input_lev; }
  uint64_t Max_bts_lev() const { return _lev_bts; }
  bool     Opt_bts_res_level() const { return _opt_bts_res_lev; }
  bool     Const_encode_level() const { return _cst_encode_lev; }
  bool     Const_encode_scale() const { return _cst_encode_scale; }
  bool     Rescale_phi_res() const { return _rescale_phi_res; }
  bool     Resbm() const { return _resbm; }
  bool     Trace_resbm() const { return _trace_resbm; }

  // leave this member public so that OPTION_DESC can access it
  // uint32_t fields changed to uint64_t to match K_UINT64 option descriptor type.
  // The option parser writes 8 bytes via *((uint64_t*)field) = value, so
  // uint32_t fields get their upper 4 bytes spilled into the next field,
  // corrupting adjacent values (e.g. -CKKS:sf=56 overwrote _poly_deg).
  uint64_t _secret_key_hamming_weight = 0;
  uint64_t _q0                        = 0;
  uint64_t _sf                        = 0;
  uint64_t _poly_deg                  = 0;
  uint64_t _input_lev                 = 0;
  uint64_t _lev_bts                   = DEFAULT_MAX_BTS_LEV;
  bool     _opt_bts_res_lev           = true;
  bool     _cst_encode_lev            = true;
  bool     _cst_encode_scale          = true;
  bool     _rescale_phi_res           = true;
  bool     _resbm                     = false;
  bool     _trace_resbm               = false;
};

//! @brief Macro to define API to access CKKS config
#define DECLARE_CKKS_CONFIG_ACCESS_API(cfg)                                    \
  uint64_t Hamming_weight() const { return cfg.Hamming_weight(); }             \
  uint64_t Q0_bit_num() const { return cfg.Q0_bit_num(); }                     \
  uint64_t Scale_factor_bit_num() const { return cfg.Scale_factor_bit_num(); } \
  uint64_t Poly_deg() const { return cfg.Poly_deg(); }                         \
  uint64_t Input_lev() const { return cfg.Input_lev(); }                       \
  uint64_t Max_bts_lev() const { return cfg.Max_bts_lev(); }                   \
  bool     Opt_bts_res_lev() const { return cfg.Opt_bts_res_level(); }         \
  bool     Const_encode_level() const { return cfg.Const_encode_level(); }     \
  bool     Const_encode_scale() const { return cfg.Const_encode_scale(); }     \
  bool     Rescale_phi_res() const { return cfg.Rescale_phi_res(); }           \
  bool     Resbm() const { return cfg.Resbm(); }                               \
  bool     Trace_resbm() const { return cfg.Trace_resbm(); }                   \
  DECLARE_COMMON_CONFIG_ACCESS_API(cfg)

}  // namespace ckks
}  // namespace fhe

#endif  // FHE_CKKS_CONFIG_H
