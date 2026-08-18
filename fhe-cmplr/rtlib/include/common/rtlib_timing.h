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
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

//! define a max nested level for timing items
#define RTLIB_TIMING_MAX_LEVEL 16

//! define all rtlib timing items
#define RTLIB_TIMING_ALL()          \
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
  DECL_RTM(RTM_HW_MUL, 1)           \
  DECL_RTM(RTM_MULP_FAST, 1)        \
  DECL_RTM(RTM_HW_ROT, 1)           \
  /* polynomial operation */        \
  DECL_RTM(RTM_COPY_POLY, 1)        \
  DECL_RTM(RTM_DECOMP, 1)           \
  DECL_RTM(RTM_EXTEND, 1)           \
  DECL_RTM(RTM_PRECOMP, 1)          \
  DECL_RTM(RTM_MOD_DOWN, 1)         \
  DECL_RTM(RTM_MOD_DOWN_FUSE1, 1)   \
  DECL_RTM(RTM_MOD_UP, 1)           \
  DECL_RTM(RTM_DECOMP_MODUP, 1)     \
  DECL_RTM(RTM_DOT_PROD, 1)         \
  DECL_RTM(RTM_FAST_DOT_PROD, 1)    \
  DECL_RTM(RTM_ROTATE_EXT, 1)       \
  DECL_RTM(RTM_RELIN, 1)            \
  DECL_RTM(RTM_RELU, 1)             \
  DECL_RTM(RTM_KEYSWITCH, 1)        \
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
  DECL_RTM(RTM_CHEBY_1, 4)          \
  DECL_RTM(RTM_CHEBY_2, 4)          \
  DECL_RTM(RTM_CHEBY_MUL_PLAIN, 4)  \
  DECL_RTM(RTM_BS_SLOT_TO_COEFF, 3) \
  /* bootstrap sub-operations */    \
  DECL_RTM(RTM_BS_PRECOMP, 3)       \
  DECL_RTM(RTM_BS_MOD_DOWN, 3)      \
  DECL_RTM(RTM_BS_DOT_PROD, 3)      \
  DECL_RTM(RTM_FAST_SWITCH_KEY, 2)  \
  /* plaintext encoding */          \
  DECL_RTM(RTM_PT_ENCODE, 1)        \
  /* plaintext manager */           \
  DECL_RTM(RTM_PT_GET, 1)

//! internal timing ID
typedef enum {
#define DECL_RTM(ID, LEVEL) ID,
  RTLIB_TIMING_ALL()
#undef DECL_RTM

  // last id
  RTM_LAST
} RTLIB_TIMING_ID;

//! append rtlib timing item
void Append_rtlib_timing(RTLIB_TIMING_ID id, uint64_t nsec);

//! report rtlib timing
void Report_rtlib_timing();

//! flag to indicate timing is inside bootstrap evaluation
extern bool Rtlib_in_bootstrap;

//! put a mark to indicate timing start
static inline uint64_t Mark_rtm_start() {
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  return (ts.tv_sec << 32) | (uint32_t)ts.tv_nsec;
}

//! indicate current timing is end
static inline void Mark_rtm_end(RTLIB_TIMING_ID id, uint64_t start) {
  struct timespec ts;
  clock_gettime(CLOCK_REALTIME, &ts);
  uint64_t nsec =
      (ts.tv_sec - (start >> 32)) * 1000000000 + ts.tv_nsec - (uint32_t)start;
  Append_rtlib_timing(id, nsec);
}

#define RTLIB_ENABLE_TIMING

#ifdef RTLIB_ENABLE_TIMING
#define RTLIB_TM_START(id, mark) uint64_t mark = Mark_rtm_start()
#define RTLIB_TM_END(id, mark)   Mark_rtm_end(id, mark)
#define RTLIB_TM_REPORT()        Report_rtlib_timing()
#else
#define RTLIB_TM_START(id, mark) (void)0
#define RTLIB_TM_END(id, mark)   (void)0
#define RTLIB_TM_REPORT()        (void)0
#endif

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_COMMON_RTLIB_TIMING_H
