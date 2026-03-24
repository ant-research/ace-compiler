//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_CONFIG_H
#define NN_VECTOR_CONFIG_H

#include "nn/vector/option_config.h"

namespace nn {
namespace vector {

#define MAX_SLOT_ALLOWED 65536
#define MIN_SLOT_ALLOWED 128

struct VECTOR_CONFIG : public nn::vector::VEC_OPTION_CONFIG {
};  // struct VECTOR_CONFIG

//! @brief Macro to define API to access vector config
#define DECLARE_VECTOR_CONFIG_ACCESS_API(cfg) \
  DECLARE_VEC_OPTION_CONFIG_ACCESS_API(cfg)

}  // namespace vector
}  // namespace nn

#endif  // NN_VECTOR_CONFIG_H
