//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CORE_RT_TIMING_H
#define FHE_CORE_RT_TIMING_H

#include "nn/core/rt_timing.h"

#ifdef __cplusplus
namespace fhe {
namespace core {
static constexpr uint32_t RTM_NN_LAST = nn::core::RTM_NN_LAST;
#endif

#define RTM_FHE_EVENT_ALL()      \
  DECL_RTM(RTM_FHE, 0)           \
  DECL_RTM(RTM_FHE_ADDCC, 1)     \
  DECL_RTM(RTM_FHE_ADDCP, 1)     \
  DECL_RTM(RTM_FHE_BOOTSTRAP, 1) \
  DECL_RTM(RTM_FHE_MODSWITCH, 1) \
  DECL_RTM(RTM_FHE_MULCC, 1)     \
  DECL_RTM(RTM_FHE_MULCP, 1)     \
  DECL_RTM(RTM_FHE_RELIN, 1)     \
  DECL_RTM(RTM_FHE_RESCALE, 1)   \
  DECL_RTM(RTM_FHE_ROTATE, 1)

//! @brief Timing event for FHE
enum RTM_FHE_EVENT {
  RTM_FHE_FIRST = RTM_NN_LAST - 1,
#define DECL_RTM(ID, LEVEL) ID,
  RTM_FHE_EVENT_ALL()
#undef DECL_RTM
      RTM_FHE_LAST
};

#ifdef __cplusplus
}  // namespace core
}  // namespace fhe
#endif

#endif  // FHE_CORE_RT_TIMING_H
