//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_SIHE_CONFIG_H
#define FHE_SIHE_CONFIG_H

#include <cmath>
#include <cstdlib>
#include <cstring>

#include "fhe/sihe/option_config.h"

namespace fhe {
namespace sihe {

enum TRACE_DETAIL : uint64_t {
  TD_RELU_VR = 0x1,
};

struct SIHE_CONFIG : public fhe::sihe::SIHE_OPTION_CONFIG {
public:
  SIHE_CONFIG(void) {}

  void Update_options() {
    SIHE_OPTION_CONFIG::Update_options();
    if (std::fabs(_relu_value_range_default) < 1e-6) {
      CMPLR_USR_MSG(U_CODE::Incorrect_Option, "relu_vr_def");
      _relu_value_range_default = 1.0;
    }
  }

  // Parse "name1=val1;name2=val2;..." for a given name.
  // Returns true and sets out if found, false otherwise.
  bool Parse_relu_vr(const char* name, double& out) const {
    if (name == nullptr || _relu_value_range.empty()) {
      return false;
    }
    size_t      nlen  = std::strlen(name);
    const char* start = _relu_value_range.c_str();
    while (*start) {
      const char* semi = std::strchr(start, ';');
      const char* end  = semi ? semi : start + std::strlen(start);
      if (static_cast<size_t>(end - start) > nlen &&
          std::strncmp(start, name, nlen) == 0 && start[nlen] == '=') {
        char*  vend;
        double val = std::strtod(start + nlen + 1, &vend);
        if (std::isnormal(val) && (vend == end || *vend == '\0')) {
          out = val;
          return true;
        }
      }
      if (!semi) break;
      start = semi + 1;
    }
    return false;
  }

  double Relu_vr(const char* name) const {
    double val;
    return Parse_relu_vr(name, val) ? val : _relu_value_range_default;
  }

  bool Has_relu_vr(const char* name) const {
    double val;
    return Parse_relu_vr(name, val);
  }

};  // struct SIHE_CONFIG

#define DECLARE_SIHE_CONFIG_ACCESS_API(cfg)                            \
  double Relu_vr(const char* name) const { return cfg.Relu_vr(name); } \
  bool Has_relu_vr(const char* name) const { return cfg.Has_relu_vr(name); } \
  DECLARE_SIHE_OPTION_CONFIG_ACCESS_API(cfg)

}  // namespace sihe
}  // namespace fhe

#endif  // FHE_SIHE_CONFIG_H