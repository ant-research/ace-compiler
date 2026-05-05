//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_SIHE_CONFIG_H
#define FHE_SIHE_CONFIG_H

#include <math.h>
#include <stdlib.h>

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
    if (fabs(_relu_value_range_default) < 1e-6) {
      CMPLR_USR_MSG(U_CODE::Incorrect_Option, "relu_vr_def");
      _relu_value_range_default = 1.0;
    }
  }

  double Relu_vr(const char* name) const {
    if (name == nullptr) {
      return _relu_value_range_default;
    }
    const char* str = _relu_value_range.c_str();
    int         len = strlen(name);
    do {
      const char* pos = strstr(str, name);
      if (pos == nullptr) {
        break;
      }
      if ((pos == str || pos[-1] == ';') && pos[len] == '=') {
        char*  end;
        double val = strtod(pos + len + 1, &end);
        if (isnormal(val) && (*end == ';' || *end == '\0')) {
          return val;
        }
        break;
      }
      str += len;
    } while (true);
    return _relu_value_range_default;
  }

};  // struct SIHE_CONFIG

#define DECLARE_SIHE_CONFIG_ACCESS_API(cfg)                            \
  double Relu_vr(const char* name) const { return cfg.Relu_vr(name); } \
  DECLARE_SIHE_OPTION_CONFIG_ACCESS_API(cfg)

}  // namespace sihe
}  // namespace fhe

#endif  // FHE_SIHE_CONFIG_H
