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

struct POLY_CONFIG : public air::util::COMMON_CONFIG {
public:
  POLY_CONFIG(void)
      : _inline_rotate(false),
        _inline_relin(false),
        _reuse_preg_as_retv(true),
        _fuse_decomp_modup(true) {}

  void Register_options(air::driver::DRIVER_CTX* ctx);
  void Update_options();

  bool Inline_rotate(void) { return _inline_rotate; }

  void Set_inline_rotate(bool v) { _inline_rotate = v; }

  bool Inline_relin(void) { return _inline_relin; }

  void Set_inline_relin(bool v) { _inline_relin = v; }

  bool Reuse_preg_as_retv() { return _reuse_preg_as_retv; }

  void Set_reuse_preg_as_retv(bool v) { _reuse_preg_as_retv = v; }

  bool Fuse_decomp_modup() { return _fuse_decomp_modup; }

  void Set_fuse_decomp_modup(bool v) { _fuse_decomp_modup = v; }

  void Print(std::ostream& os) const;

  bool _inline_rotate;
  bool _inline_relin;
  bool _reuse_preg_as_retv;
  bool _fuse_decomp_modup;
};  // struct POLY_CONFIG

#define DECLARE_POLY_CONFIG(name, config)                               \
  {"inline_rotate",        "inl_rot",         "Inline rotation IR in ", \
   &config._inline_rotate, air::util::K_NONE, 0,                        \
   air::util::V_NONE},                                                  \
      {"inline_relin",                                                  \
       "inl_relin",                                                     \
       "Inline relinearize IR in " #name,                               \
       &config._inline_relin,                                           \
       air::util::K_NONE,                                               \
       0,                                                               \
       air::util::V_NONE},                                              \
      {"reuse_preg_as_retv",                                            \
       "reuse_preg",                                                    \
       "Reuse preg as call retv " #name,                                \
       &config._reuse_preg_as_retv,                                     \
       air::util::K_NONE,                                               \
       0,                                                               \
       air::util::V_NONE},                                              \
      {"op_fusion_decomp_modup",                                        \
       "decomp_modup",                                                  \
       "Fuse decompose and modup" #name,                                \
       &config._fuse_decomp_modup,                                      \
       air::util::K_NONE,                                               \
       0,                                                               \
       air::util::V_NONE},

}  // namespace poly
}  // namespace fhe

#endif  // FHE_POLY_CONFIG_H
