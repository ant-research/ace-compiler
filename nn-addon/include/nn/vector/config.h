//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_CONFIG_H
#define NN_VECTOR_CONFIG_H

#include "air/driver/common_config.h"
#include "air/driver/driver_ctx.h"

namespace nn {
namespace vector {

struct VECTOR_CONFIG : public air::util::COMMON_CONFIG {
public:
  VECTOR_CONFIG(void)
      : _gemm_fast(false), _conv_fast(false), _improve_ss_insert(false) {}

  void Register_options(air::driver::DRIVER_CTX* ctx);
  void Update_options();
  bool Conv_fast(void) const { return _conv_fast; }
  bool Gemm_fast(void) const { return _gemm_fast; }

  void Print(std::ostream& os) const;

  bool Improve_ss_insert(void) const { return _improve_ss_insert; }

  bool Ref_validate() const { return _ref_validate; }

  bool _improve_ss_insert;

  bool _conv_fast;
  bool _gemm_fast;

  bool _ref_validate;
};  // struct VECTOR_CONFIG

//! @brief Macro to define API to access TIR2VIR config
#define DECLARE_VECTOR_CONFIG_ACCESS_API(cfg)                        \
  bool Improve_ss_insert() const { return cfg.Improve_ss_insert(); } \
  bool Ref_validate() const { return cfg.Ref_validate(); }           \
  bool Conv_fast() const { return cfg.Conv_fast(); }                 \
  bool Gemm_fast() const { return cfg.Gemm_fast(); }                 \
  DECLARE_COMMON_CONFIG_ACCESS_API(cfg)

#define DECLARE_VECTOR_CONFIG(name, config)                         \
  {"improve_ss_insert",                                             \
   "improve_ssi",                                                   \
   "improve stride slice insert in " #name,                         \
   &config._improve_ss_insert,                                      \
   air::util::K_NONE,                                               \
   0,                                                               \
   air::util::V_NONE},                                              \
      {"ref_validate",                                              \
       "rfv",                                                       \
       "runtime validation with reference in " #name,               \
       &config._ref_validate,                                       \
       air::util::K_NONE,                                           \
       0,                                                           \
       air::util::V_NONE},                                          \
      {"conv_fast",                                                 \
       "conv_fast",                                                 \
       "Conv-fast lowering strategy " #name,                        \
       &config._conv_fast,                                          \
       air::util::K_NONE,                                           \
       0,                                                           \
       air::util::V_NONE},                                          \
  {                                                                 \
    "gemm_fast", "gemm_fast", "Gemm-fast lowering strategy " #name, \
        &config._gemm_fast, air::util::K_NONE, 0, air::util::V_NONE \
  }

}  // namespace vector
}  // namespace nn

#endif  // NN_VECTOR_CONFIG_H
