//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_CORE_RT_TIMING_H
#define NN_CORE_RT_TIMING_H

#include "air/base/rt_timing.h"

#ifdef __cplusplus
namespace nn {
namespace core {
static constexpr uint32_t RTM_CORE_LAST = air::base::RTM_CORE_LAST;
#endif

#define RTM_NN_EVENT_ALL()           \
  DECL_RTM(RTM_NN, 0)                \
  DECL_RTM(RTM_NN_ADD, 1)            \
  DECL_RTM(RTM_NN_AVGPOOL, 1)        \
  DECL_RTM(RTM_NN_CONCAT, 1)         \
  DECL_RTM(RTM_NN_CONV, 1)           \
  DECL_RTM(RTM_NN_FLATTEN, 1)        \
  DECL_RTM(RTM_NN_GEMM, 1)           \
  DECL_RTM(RTM_NN_GLOBAL_AVGPOOL, 1) \
  DECL_RTM(RTM_NN_MUL, 1)            \
  DECL_RTM(RTM_NN_RELU, 1)

//! @brief Timing event for NN
enum RTM_NN_EVENT {
  RTM_NN_FIRST = RTM_CORE_LAST - 1,
#define DECL_RTM(ID, LEVEL) ID,
  RTM_NN_EVENT_ALL()
#undef DECL_RTM
      RTM_NN_LAST
};

#ifdef __cplusplus
}  // namespace core
}  // namespace nn
#endif

#endif  // NN_CORE_RT_TIMING_H
