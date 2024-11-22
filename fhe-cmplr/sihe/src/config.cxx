//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/sihe/config.h"

#include <math.h>
#include <stdlib.h>

using namespace air::base;
using namespace air::util;

namespace fhe {
namespace sihe {

static SIHE_CONFIG Sihe_config;

static OPTION_DESC Sihe_option[] = {
    DECLARE_COMMON_CONFIG(sihe, Sihe_config),
    {"relu_value_range_default", "relu_vr_def",
                         "Default value range (1.0) of relu",                                       &Sihe_config._relu_value_range_def_val, air::util::K_DOUBLE, 0, V_EQUAL},
    {"relu_value_range",         "relu_vr",     "Value range of relu",
                         &Sihe_config._relu_value_range,                                                                                    air::util::K_STR,    0, V_EQUAL},
    {"relu_mul_depth",           "relu_depth",  "Multiplication depth of relu",
                         &Sihe_config._relu_mul_depth,                                                                                      air::util::K_INT64,  0, V_EQUAL},
    {"relu_base_poly_type",      "relu_basis",  "Base polynomial type of relu",
                         &Sihe_config._relu_base_type,                                                                                      air::util::K_INT64,  0, V_EQUAL},
};

static OPTION_DESC_HANDLE Sihe_option_handle = {
    sizeof(Sihe_option) / sizeof(Sihe_option[0]), Sihe_option};

static OPTION_GRP Sihe_option_grp = {
    "SIHE", "Scheme-Independent Homomorphic Encryption", ':',
    air::util::V_EQUAL, &Sihe_option_handle};

void SIHE_CONFIG::Register_options(air::driver::DRIVER_CTX* ctx) {
  ctx->Register_option_group(&Sihe_option_grp);
}

void SIHE_CONFIG::Update_options() {
  *this = Sihe_config;
  if (fabs(_relu_value_range_def_val) < 1e-6) {
    CMPLR_USR_MSG(U_CODE::Incorrect_Option, "relu_vr_def");
    _relu_value_range_def_val = 1.0;
  }
}

void SIHE_CONFIG::Print(std::ostream& os) const {
  COMMON_CONFIG::Print(os);
  os << "Default value rangle of ReLU: [" << -1. * _relu_value_range_def_val
     << ", " << _relu_value_range_def_val << "]" << std::endl;
  os << "Value rangle of ReLU: " << Relu_value_range_msg() << std::endl;
  os << "Multiplication depth of ReLU: " << Relu_mul_depth() << std::endl;
  os << "Base polynomial type of ReLU: " << Relu_base_type() << std::endl;
}

double SIHE_CONFIG::Relu_value_range(const char* name) const {
  if (name == nullptr) {
    return _relu_value_range_def_val;
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
  return _relu_value_range_def_val;
}

}  // namespace sihe
}  // namespace fhe
