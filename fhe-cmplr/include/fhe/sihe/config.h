//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_SIHE_CONFIG_H
#define FHE_SIHE_CONFIG_H

#include "air/driver/common_config.h"
#include "air/driver/driver_ctx.h"

namespace fhe {
namespace sihe {

enum TRACE_DETAIL {
  TRACE_RELU_VR = 0,
};

struct SIHE_CONFIG : public air::util::COMMON_CONFIG {
public:
  SIHE_CONFIG(void) {}

  void Register_options(air::driver::DRIVER_CTX* ctx);
  void Update_options();

  void Print(std::ostream& os) const;

  const char* Relu_value_range_msg() const { return _relu_value_range.c_str(); }

  double Relu_value_range(const char* name) const;

  uint32_t Relu_mul_depth() const { return _relu_mul_depth; }
  uint32_t Relu_base_type() const { return _relu_base_type; }

  // leave this member public so that OPTION_DESC can access it
  std::string _relu_value_range;
  double      _relu_value_range_def_val = 1.0;
  uint32_t    _relu_mul_depth           = 0;
  uint32_t    _relu_base_type           = 0;
};  // struct SIHE_CONFIG

#define DECLARE_SIHE_CONFIG_ACCESS_API(cfg)                  \
  DECLARE_COMMON_CONFIG_ACCESS_API(cfg)                      \
  double Relu_value_range(const char* name) {                \
    return cfg.Relu_value_range(name);                       \
  }                                                          \
  uint32_t Relu_mul_depth() { return cfg.Relu_mul_depth(); } \
  uint32_t Relu_base_type() { return cfg.Relu_base_type(); }

}  // namespace sihe
}  // namespace fhe

#endif  // FHE_SIHE_CONFIG_H
