//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_POLY_CONFIG_H
#define FHE_POLY_CONFIG_H

#include "air/driver/common_config.h"
#include "air/driver/driver_ctx.h"

namespace fhe {
namespace poly {

enum TRACE_DETAIL {
  TRACE_IR_BEFORE_MUP       = 0,  // 0x1
  TRACE_IR_AFTER_MUP        = 1,  // 0x2
  TRACE_MUP_FLOW            = 2,  // 0x4
  TRACE_IR_BEFORE_MD        = 3,  // 0x8
  TRACE_IR_AFTER_MD         = 4,  // 0x10
  TRACE_MD_FLOW             = 5,  // 0x20
  TRACE_MD_STATICS          = 6,  // 0x40
  TRACE_IR_BEFORE_MD_FUSION = 7,  // 0x80
  TRACE_IR_AFTER_MD_FUSION  = 8,  // 0x100
  TRACE_MD_FUSION           = 9,  // 0x101
};

struct POLY_CONFIG : public air::util::COMMON_CONFIG {
public:
  POLY_CONFIG(void)
      : _inline_rotate(true),
        _inline_relin(true),
        _reuse_preg_as_retv(true),
        _fuse_decomp_modup(true),
        _lower_to_hpoly(true),
        _lower_to_lpoly(true),
        _mup_hoisting(false),
        _mdown_hoisting(false),
        _mdown_fusion(false),
        _mul_encode_fusion(false),
        _dotprod_fusion(false),
        _inline_rns_op(false),
        _normalize(false) {}

  void Register_options(air::driver::DRIVER_CTX* ctx);
  void Update_options();

  bool Inline_rotate(void) { return _inline_rotate; }

  void Set_inline_rotate(bool v) { _inline_rotate = v; }

  bool Inline_relin(void) { return _inline_relin; }

  void Set_inline_relin(bool v) { _inline_relin = v; }

  bool Inline_rns_op(void) { return _inline_rns_op; }

  bool Set_inline_rns_op(bool v) { _inline_rns_op = v; }

  bool Reuse_preg_as_retv() { return _reuse_preg_as_retv; }

  void Set_reuse_preg_as_retv(bool v) { _reuse_preg_as_retv = v; }

  bool Fuse_decomp_modup() { return _fuse_decomp_modup; }

  void Set_fuse_decomp_modup(bool v) { _fuse_decomp_modup = v; }

  bool Lower_to_hpoly() { return _lower_to_hpoly; }

  void Set_lower_to_hpoly(bool v) { _lower_to_hpoly = v; }

  bool Lower_to_lpoly() { return _lower_to_lpoly; }

  void Set_lower_to_lpoly(bool v) { _lower_to_lpoly = v; }

  bool Mup_hoisting() { return _mup_hoisting; }

  void Set_mup_hoisting(bool v) { _mup_hoisting = v; }

  bool Mdown_hoisting() { return _mdown_hoisting; }

  void Set_mdown_hoisting(bool v) { _mdown_hoisting = v; }

  bool Mdown_fusion() { return _mdown_fusion; }

  void Set_mdown_fusion(bool v) { _mdown_fusion = v; }

  bool Mue_fusion() const { return _mul_encode_fusion; }

  void Set_mue_fusion(bool v) { _mul_encode_fusion = v; }

  bool Dotprod_fusion() const { return _dotprod_fusion; }

  void Set_dotprod_fusion(bool v) { _dotprod_fusion = v; }

  bool Normalize() { return _normalize; }

  void Set_normalize(bool v) { _normalize = v; }

  void Pre_process_options();
  void Print(std::ostream& os) const;

  bool _inline_rotate;
  bool _inline_relin;
  bool _reuse_preg_as_retv;
  bool _inline_rns_op;
  bool _fuse_decomp_modup;
  bool _lower_to_hpoly;
  bool _lower_to_lpoly;
  bool _run_pre;
  bool _mup_hoisting;
  bool _mdown_hoisting;
  bool _mdown_fusion;
  bool _mul_encode_fusion;
  bool _dotprod_fusion;
  bool _normalize;
};  // struct POLY_CONFIG

#define DECLARE_POLY_CONFIG(name, config)                                     \
  {"inline_rotate",        "inl_rot",         "Inline rotation IR in ",       \
   &config._inline_rotate, air::util::K_NONE, 0,                              \
   air::util::V_NONE},                                                        \
      {"inline_relin",                                                        \
       "inl_relin",                                                           \
       "Inline relinearize IR in " #name,                                     \
       &config._inline_relin,                                                 \
       air::util::K_NONE,                                                     \
       0,                                                                     \
       air::util::V_NONE},                                                    \
      {"reuse_preg_as_retv",                                                  \
       "reuse_preg",                                                          \
       "Reuse preg as call retv " #name,                                      \
       &config._reuse_preg_as_retv,                                           \
       air::util::K_NONE,                                                     \
       0,                                                                     \
       air::util::V_NONE},                                                    \
      {"op_fusion_decomp_modup",                                              \
       "decomp_modup",                                                        \
       "Fuse decompose and modup" #name,                                      \
       &config._fuse_decomp_modup,                                            \
       air::util::K_NONE,                                                     \
       0,                                                                     \
       air::util::V_NONE},                                                    \
      {"hpoly",                                                               \
       "hpoly",                                                               \
       "Lower to hpoly" #name,                                                \
       &config._lower_to_hpoly,                                               \
       air::util::K_NONE,                                                     \
       0,                                                                     \
       air::util::V_NONE},                                                    \
      {"lpoly",                                                               \
       "lpoly",                                                               \
       "Lower to lpoly" #name,                                                \
       &config._lower_to_lpoly,                                               \
       air::util::K_NONE,                                                     \
       0,                                                                     \
       air::util::V_NONE},                                                    \
      {"pre",                                                                 \
       "pre",                                                                 \
       "Run ssa pre" #name,                                                   \
       &config._run_pre,                                                      \
       air::util::K_NONE,                                                     \
       0,                                                                     \
       air::util::V_NONE},                                                    \
      {"modup_hoisting",           "muh",                                     \
       "Run modup hoisting" #name, &config._mup_hoisting,                     \
       air::util::K_NONE,          0,                                         \
       air::util::V_NONE},                                                    \
      {"mdown_hoisting",                                                      \
       "mdh",                                                                 \
       "Run moddown hoisting" #name,                                          \
       &config._mdown_hoisting,                                               \
       air::util::K_NONE,                                                     \
       0,                                                                     \
       air::util::V_NONE},                                                    \
      {"mdown_fusion",                                                        \
       "mdf",                                                                 \
       "Run moddown fusion" #name,                                            \
       &config._mdown_fusion,                                                 \
       air::util::K_NONE,                                                     \
       0,                                                                     \
       air::util::V_NONE},                                                    \
      {"mul_encode_fusion",                                                   \
       "mef",                                                                 \
       "Run mul encode fusion" #name,                                         \
       &config._mul_encode_fusion,                                            \
       air::util::K_NONE,                                                     \
       0,                                                                     \
       air::util::V_NONE},                                                    \
      {"normalize",       "nor", "Normalize kids " #name, &config._normalize, \
       air::util::K_NONE, 0,     air::util::V_NONE},

}  // namespace poly
}  // namespace fhe

#endif  // FHE_POLY_CONFIG_H
