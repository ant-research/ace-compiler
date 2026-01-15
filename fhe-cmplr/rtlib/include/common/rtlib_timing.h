//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_COMMON_RTLIB_TIMING_H
#define RTLIB_COMMON_RTLIB_TIMING_H

//! @brief rtlib_timing.h
//! Define prototypes for RTLIB internal timing functions

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#ifdef RTLIB_SUPPORT_LINUX
#include <time.h>

#include "air/base/rt_timing.h"
#include "error.h"
#include "fhe/core/rt_timing.h"
#include "nn/core/rt_timing.h"

#ifdef __cplusplus
static constexpr uint32_t RTM_NN_FIRST      = nn::core::RTM_NN_LAST;
static constexpr uint32_t RTM_NN_LAST       = nn::core::RTM_NN_LAST;
static constexpr uint32_t RTM_FHE_FIRST     = fhe::core::RTM_FHE_LAST;
static constexpr uint32_t RTM_FHE_LAST      = fhe::core::RTM_FHE_LAST;
static constexpr uint32_t RTM_FHE_BOOTSTRAP = fhe::core::RTM_FHE_BOOTSTRAP;
#endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef RTLIB_SUPPORT_LINUX
//! define a max nested level for timing items
#define RTLIB_TIMING_MAX_LEVEL 16

//! define all rtlib timing items
#define RTM_LIB_EVENT_ALL()         \
  /* context timing */              \
  DECL_RTM(RTM_FINALIZE_CONTEXT, 0) \
  DECL_RTM(RTM_PREPARE_CONTEXT, 0)  \
  /* block io */                    \
  DECL_RTM(RTM_IO_SUBMIT, 0)        \
  DECL_RTM(RTM_IO_COMPLETE, 0)      \
  /* encoding */                    \
  DECL_RTM(RTM_ENCODE_ARRAY, 0)     \
  DECL_RTM(RTM_ENCODE_VALUE, 0)     \
  /* ntt/fft and intt/ifft */       \
  DECL_RTM(RTM_NTT, 0)              \
  DECL_RTM(RTM_INTT, 0)             \
  /* rtlib api */                   \
  DECL_RTM(RTM_MAIN_GRAPH, 0)       \
  /* coefficient operation */       \
  DECL_RTM(RTM_HW_ADD, 1)           \
  DECL_RTM(RTM_HW_SUB, 1)           \
  DECL_RTM(RTM_HW_MUL, 1)           \
  DECL_RTM(RTM_HW_MAC, 1)           \
  DECL_RTM(RTM_HW_ROT, 1)           \
  /* polynomial operation */        \
  DECL_RTM(RTM_COPY_POLY, 1)        \
  DECL_RTM(RTM_DECOMP, 1)           \
  DECL_RTM(RTM_EXTEND, 1)           \
  DECL_RTM(RTM_PRECOMP, 1)          \
  DECL_RTM(RTM_MOD_DOWN, 1)         \
  DECL_RTM(RTM_MOD_DOWN_RESCALE, 1) \
  DECL_RTM(RTM_MOD_UP, 1)           \
  DECL_RTM(RTM_DECOMP_MODUP, 1)     \
  DECL_RTM(RTM_DOT_PROD, 1)         \
  DECL_RTM(RTM_FAST_DOT_PROD, 1)    \
  DECL_RTM(RTM_MULP, 1)             \
  DECL_RTM(RTM_MULP_FAST, 1)        \
  DECL_RTM(RTM_RESCALE_POLY, 1)     \
  /* cipher operation */            \
  DECL_RTM(RTM_COPY_CIPH, 1)        \
  DECL_RTM(RTM_INIT_CIPH_SM_SC, 1)  \
  DECL_RTM(RTM_INIT_CIPH_UP_SC, 1)  \
  DECL_RTM(RTM_INIT_CIPH_DN_SC, 1)  \
  /* bootstrapping */               \
  DECL_RTM(RTM_BOOTSTRAP, 1)        \
  DECL_RTM(RTM_BS_SETUP, 2)         \
  DECL_RTM(RTM_BS_KEYGEN, 2)        \
  DECL_RTM(RTM_BS_EVAL, 2)          \
  DECL_RTM(RTM_BS_COPY, 3)          \
  DECL_RTM(RTM_BS_PARTIAL_SUM, 3)   \
  DECL_RTM(RTM_BS_COEFF_TO_SLOT, 3) \
  DECL_RTM(RTM_BS_APPROX_MOD, 3)    \
  DECL_RTM(RTM_BS_SLOT_TO_COEFF, 3) \
  /* plaintext encoding */          \
  DECL_RTM(RTM_PT_ENCODE, 1)        \
  /* plaintext manager */           \
  DECL_RTM(RTM_PT_GET, 1)

//! internal timing ID
enum RTM_LIB_EVENT {
  RTM_LIB_FIRST = RTM_FHE_LAST - 1,
#define DECL_RTM(ID, LEVEL) ID,
  RTM_LIB_EVENT_ALL()
#undef DECL_RTM
      RTM_LIB_LAST
};

//! append rtlib timing item
struct RTM_INFO {
  uint32_t _event;     // event: NN_CONV, FHE_BOOTSTRAPPING, etc
  uint32_t _id;        // id
  uint32_t _sec;       // start seconds
  uint32_t _nsec;      // start nanoseconds
  uint64_t _sub_nsec;  // total time in nanoseconds for nested RTM
};

struct RTM_INFO_STACK {
  struct RTM_INFO _info[RTLIB_TIMING_MAX_LEVEL];
  uint32_t        _size;
  uint32_t        _max_size;
};

extern struct RTM_INFO_STACK Rtm_stack;
#ifdef _OPENMP
#pragma omp threadprivate(Rtm_stack)
#endif
#endif

//! init rtm stack
void Init_rtlib_timing();

//! report rtlib timing
void Report_rtlib_timing();

//! append rtlib timing item
void Append_rtlib_timing(uint32_t event, uint32_t id, uint64_t nsec,
                         uint64_t sub_nsec);

#ifdef RTLIB_SUPPORT_LINUX
//! put a mark to indicate timing start
static inline void Mark_rtm_start(uint32_t event, uint32_t id) {
  IS_TRUE(Rtm_stack._size < Rtm_stack._max_size, "FATAL: RTM stack overflow");

  struct RTM_INFO* info = &Rtm_stack._info[Rtm_stack._size];
  info->_event          = event;
  info->_id             = id;
  info->_sub_nsec       = 0;
  ++Rtm_stack._size;

  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  info->_sec  = ts.tv_sec;
  info->_nsec = ts.tv_nsec;
}

//! indicate current timing is end
static inline void Mark_rtm_end(uint32_t event, uint64_t id) {
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);

  IS_TRUE(Rtm_stack._size > 0, "FATAL: RTM stack underflow");
  --Rtm_stack._size;
  struct RTM_INFO* info = &Rtm_stack._info[Rtm_stack._size];
  IS_TRUE(info->_event == event && info->_id == id,
          "FATAL: RTM stack mismatch");
  uint64_t nsec =
      (ts.tv_sec - info->_sec) * 1000000000 + ts.tv_nsec - info->_nsec;
  if (Rtm_stack._size > 1 && ((event >= RTM_NN_FIRST && event <= RTM_NN_LAST) ||
                              event == RTM_FHE_BOOTSTRAP)) {
    // only handle nested NN operators and bootstrap
    Rtm_stack._info[Rtm_stack._size - 1]._sub_nsec += nsec;
  }
  Append_rtlib_timing(event, id, nsec, info->_sub_nsec);
}
#endif

#ifdef RTLIB_SUPPORT_LINUX
#define RTLIB_ENABLE_TIMING
#endif

#ifdef RTLIB_ENABLE_TIMING
#define RTLIB_TM_START(id, mark) Mark_rtm_start(id, 0)
#define RTLIB_TM_END(id, mark)   Mark_rtm_end(id, 0)
#define CMPLR_TM_START(id, s)    Mark_rtm_start(id, s)
#define CMPLR_TM_END(id, s)      Mark_rtm_end(id, s)
#define RTLIB_TM_REPORT()        Report_rtlib_timing()
#else
#define RTLIB_TM_START(id, mark) (void)0
#define RTLIB_TM_END(id, mark)   (void)0
#define CMPLR_TM_START(id)       (void)0
#define CMPLR_TM_END(id)         (void)0
#define RTLIB_TM_REPORT()        (void)0
#endif

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_COMMON_RTLIB_TIMING_H
