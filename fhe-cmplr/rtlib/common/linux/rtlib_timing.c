//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "common/rtlib_timing.h"

#include <stdlib.h>
#include <string.h>

#include "common/error.h"
#include "common/rt_env.h"

#define LONG_BAR  "--------------------"
#define SHORT_BAR "--------"

struct RTM_INFO_STACK Rtm_stack = {{0}, 0, RTLIB_TIMING_MAX_LEVEL};

static uint64_t Rtlib_timing[RTM_LIB_LAST];
static uint64_t Rtlib_sub_timing[RTM_LIB_LAST];
static uint64_t Rtlib_count[RTM_LIB_LAST];

void Init_rtlib_timing() {}

void Append_rtlib_timing(uint32_t event, uint32_t id, uint64_t nsec,
                         uint64_t sub_nsec) {
  Rtlib_timing[event] += nsec;
  Rtlib_sub_timing[event] += sub_nsec;
  Rtlib_count[event]++;
}

void Report_rtlib_timing() {
  static const char* name[RTM_LIB_LAST] = {
#define DECL_RTM(ID, LEVEL) #ID,
      RTM_CORE_EVENT_ALL() RTM_NN_EVENT_ALL() RTM_FHE_EVENT_ALL()
          RTM_LIB_EVENT_ALL()
#undef DECL_RTM
  };
  static const int32_t level[RTM_LIB_LAST] = {
#define DECL_RTM(ID, LEVEL) LEVEL,
      RTM_CORE_EVENT_ALL() RTM_NN_EVENT_ALL() RTM_FHE_EVENT_ALL()
          RTM_LIB_EVENT_ALL()
#undef DECL_RTM
  };

  const char* fname = getenv(ENV_RTLIB_TIMING_OUTPUT);
  if (fname == NULL) {
    return;
  }
  FILE* fp         = NULL;
  bool  need_close = false;
  if (strcmp(fname, "stdout") == 0 || strcmp(fname, "-") == 0) {
    fp = stdout;
  } else if (strcmp(fname, "stderr") == 0) {
    fp = stderr;
  } else {
    fp = fopen(fname, "w");
    if (!fp) {
      return;
    }
    need_close = true;
  }

  fprintf(fp, "%-24s\t%12s\t%12s\n", "RTLib functions", "Count", "Elapse");
  fprintf(fp, "%-24s\t%12s\t%12s\n", LONG_BAR, SHORT_BAR, SHORT_BAR);
  uint64_t sum[RTLIB_TIMING_MAX_LEVEL] = {0};
  uint32_t par[RTLIB_TIMING_MAX_LEVEL] = {0};
  int32_t  index                       = 0;
  for (uint32_t i = 0; i < RTM_LIB_LAST; ++i) {
    if (Rtlib_count[i] > 0 ||
        (i < (RTM_LIB_LAST - 1) && level[i] < level[i + 1])) {
      int32_t curr = level[i];
      IS_TRUE(curr < RTLIB_TIMING_MAX_LEVEL, "exceed max level");
      while (curr < index) {
        // output previous sub total and reset sum to zero
        fprintf(fp, "%*s%-24s\t%12s\t%12.6f sec\n", index - 1, "",
                name[par[index - 1]] + 4, "sub total",
                (double)sum[index] / 1000000000.0);
        sum[index] = 0;
        par[index] = 0;
        --index;
      }
      sum[curr] += (Rtlib_timing[i] - Rtlib_sub_timing[i]);
      par[curr] = i;
      if (Rtlib_count[i] != 0) {
        double ctm = (double)Rtlib_timing[i] / 1000000000.0;
        double stm = (double)Rtlib_sub_timing[i] / 1000000000.0;
        if (stm > 0) {
          fprintf(fp, "%*s%-24s\t%12ld\t%12.6f sec (%.6f - %.6f)\n", curr, "",
                  name[i] + 4, Rtlib_count[i], ctm - stm, ctm, stm);
        } else {
          fprintf(fp, "%*s%-24s\t%12ld\t%12.6f sec\n", curr, "", name[i] + 4,
                  Rtlib_count[i], ctm);
        }
      } else {
        fprintf(fp, "%*s%-24s\t%12c\t%12c\n", curr, "", name[i] + 4, '-', '-');
      }
      index = curr;
    }
  }
  while (index > 0) {
    // output previous sub total
    fprintf(fp, "%*s%-24s\t%12s\t%12.6f sec\n", index - 1, "",
            name[par[index - 1]] + 4, "sub total",
            (double)sum[index] / 1000000000.0);
    --index;
  }

  if (need_close) {
    fclose(fp);
  }
}
