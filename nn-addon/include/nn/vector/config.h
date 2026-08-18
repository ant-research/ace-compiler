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

#define MAX_SLOT_ALLOWED 65536
#define MIN_SLOT_ALLOWED 128

struct VECTOR_CONFIG : public air::util::COMMON_CONFIG {
public:
  VECTOR_CONFIG(void)
      : _gemm_fast(false),
        _conv_fast(false),
        _conv_parallel(false),
        _sharding(false),
        _stride_slice_fast(false),
        _two_level_ss(false),
        _improve_ss_insert(false),
        _selective_ss(false),
        _decompose_mid_op(false),
        _const_fold(false),
        _mask_fuse(false),
        _max_slots(0) {}

  void Register_options(air::driver::DRIVER_CTX* ctx);
  void Update_options();
  bool Conv_fast(void) const { return _conv_fast; }
  bool Conv_parallel(void) const { return _conv_parallel; }
  bool Sharding(void) const { return _sharding; }
  bool Gemm_fast(void) const { return _gemm_fast; }
  bool Stride_slice_fast(void) const { return _stride_slice_fast; }
  bool Two_level_ss(void) const { return _two_level_ss; }

  void Print(std::ostream& os) const;

  bool Improve_ss_insert(void) const { return _improve_ss_insert; }

  bool     Selective_ss(void) const { return _selective_ss; }
  bool     Decompose_mid_op(void) const { return _decompose_mid_op; }
  bool     Const_fold(void) const { return _const_fold; }
  uint64_t Max_slots(void) const { return _max_slots; }
  bool     Mask_fuse(void) const { return _mask_fuse; }
  bool     Ref_validate() const { return _ref_validate; }
  bool     Python_dsl() const { return _python_dsl; }

  bool _improve_ss_insert;

  bool _conv_fast;
  bool _conv_parallel;
  bool _sharding;
  bool _gemm_fast;
  bool _stride_slice_fast;
  bool _two_level_ss;
  bool _python_dsl;

  bool     _selective_ss;
  bool     _decompose_mid_op;
  bool     _const_fold;
  uint64_t _max_slots;
  bool     _mask_fuse;
  bool     _ref_validate;
};  // struct VECTOR_CONFIG

//! @brief Macro to define API to access TIR2VIR config
#define DECLARE_VECTOR_CONFIG_ACCESS_API(cfg)                            \
  bool     Improve_ss_insert() const { return cfg.Improve_ss_insert(); } \
  bool     Ref_validate() const { return cfg.Ref_validate(); }           \
  bool     Conv_fast() const { return cfg.Conv_fast(); }                 \
  bool     Conv_parallel() const { return cfg.Conv_parallel(); }         \
  bool     Gemm_fast() const { return cfg.Gemm_fast(); }                 \
  bool     Stride_slice_fast() const { return cfg.Stride_slice_fast(); } \
  bool     Two_level_ss() const { return cfg.Two_level_ss(); }           \
  bool     Selective_ss() const { return cfg.Selective_ss(); }           \
  bool     Decompose_mid_op() const { return cfg.Decompose_mid_op(); }   \
  bool     Const_fold() const { return cfg.Const_fold(); }               \
  uint64_t Max_slots() const { return cfg.Max_slots(); }                 \
  bool     Mask_fuse() const { return cfg.Mask_fuse(); }                 \
  bool     Python_dsl() const { return cfg.Python_dsl(); }               \
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
      {"conv_parallel",                                             \
       "conv_parallel",                                             \
       "Conv-parallel lowering strategy " #name,                    \
       &config._conv_parallel,                                      \
       air::util::K_NONE,                                           \
       0,                                                           \
       air::util::V_NONE},                                          \
      {"sharding",                                             \
       "sharding",                                             \
       "Opeartor sharding " #name,                    \
       &config._sharding,                                      \
       air::util::K_NONE,                                           \
       0,                                                           \
       air::util::V_NONE},                                          \
      {"stride_slice_fast",                                         \
       "ss_fast",                                                   \
       "enable strided_slice fast " #name,                          \
       &config._stride_slice_fast,                                  \
       air::util::K_NONE,                                           \
       0,                                                           \
       air::util::V_NONE},                                          \
      {"two_level_ss",                                              \
       "tlss",                                                      \
       "enable 2 level strided_slice version " #name,               \
       &config._two_level_ss,                                       \
       air::util::K_NONE,                                           \
       0,                                                           \
       air::util::V_NONE},                                          \
      {"selective_ss",                                              \
       "sss",                                                       \
       "fuse strided_slice " #name,                                 \
       &config._selective_ss,                                       \
       air::util::K_NONE,                                           \
       0,                                                           \
       air::util::V_NONE},                                          \
      {"decompose_mid_op",                                          \
       "dmo",                                                       \
       "decompose NN op to middle level op " #name,                 \
       &config._decompose_mid_op,                                   \
       air::util::K_NONE,                                           \
       0,                                                           \
       air::util::V_NONE},                                          \
      {"const_fold",                                                \
       "cf",                                                        \
       "simple const folding " #name,                               \
       &config._const_fold,                                         \
       air::util::K_NONE,                                           \
       0,                                                           \
       air::util::V_NONE},                                          \
      {"max_slots",                                                 \
       "ms",                                                        \
       "max available slots " #name,                                \
       &config._max_slots,                                          \
       air::util::K_UINT64,                                         \
       0,                                                           \
       air::util::V_EQUAL},                                         \
      {"mask_fuse",                                                 \
       "mf",                                                        \
       "remove extra masking op " #name,                            \
       &config._mask_fuse,                                          \
       air::util::K_NONE,                                           \
       0,                                                           \
       air::util::V_NONE},                                          \
  {                                                                 \
    "gemm_fast", "gemm_fast", "Gemm-fast lowering strategy " #name, \
        &config._gemm_fast, air::util::K_NONE, 0, air::util::V_NONE \
  },                                                                \
  {                                                                 \
    "python_dsl", "dsl", "Use python DSL to lower operators in " #name, \
        &config._python_dsl, air::util::K_NONE, 0, air::util::V_NONE \
  }

}  // namespace vector
}  // namespace nn

#endif  // NN_VECTOR_CONFIG_H
