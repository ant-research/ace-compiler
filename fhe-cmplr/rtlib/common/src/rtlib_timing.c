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

static uint64_t Rtlib_timing[RTM_LAST];
static uint64_t Rtlib_count[RTM_LAST];

void Append_rtlib_timing(RTLIB_TIMING_ID id, uint64_t nsec) {
  Rtlib_timing[id] += nsec;
  Rtlib_count[id]++;
}

void Report_rtlib_timing() {
  static const char* name[RTM_LAST] = {
#define DECL_RTM(ID, LEVEL) #ID,
      RTLIB_TIMING_ALL()
#undef DECL_RTM
  };
  static const int32_t level[RTM_LAST] = {
#define DECL_RTM(ID, LEVEL) LEVEL,
      RTLIB_TIMING_ALL()
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
  for (uint32_t i = 0; i < RTM_LAST; ++i) {
    if (Rtlib_count[i] > 0) {
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
      sum[curr] += Rtlib_timing[i];
      par[curr] = i;
      fprintf(fp, "%*s%-24s\t%12ld\t%12.6f sec\n", curr, "", name[i] + 4,
              Rtlib_count[i], (double)Rtlib_timing[i] / 1000000000.0);
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
