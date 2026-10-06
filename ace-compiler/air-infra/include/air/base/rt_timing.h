//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_RT_TIMING_H
#define AIR_BASE_RT_TIMING_H

#include <stdint.h>

#ifdef __cplusplus
namespace air {
namespace base {
#endif

#define RTM_CORE_EVENT_ALL() DECL_RTM(RTM_CORE, 0)

//! @brief Timing event for CORE
enum RTM_CORE_EVENT {
#define DECL_RTM(ID, LEVEL) ID,
  RTM_CORE_EVENT_ALL()
#undef DECL_RTM
      RTM_CORE_LAST,
  RTM_NONE = (uint32_t)-1,
};

#ifdef __cplusplus
}  // namespace base
}  // namespace air
#endif

#endif  // AIR_BASE_RT_TIMING_H
