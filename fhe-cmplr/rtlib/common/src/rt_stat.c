//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "common/rt_stat.h"

#include <stdio.h>
#include <time.h>

typedef struct {
  clock_t _time_stamp;
} TM_STAT;

static TM_STAT Tm_stat;
#pragma omp threadprivate(Tm_stat)

void Tm_start(const char* msg) { Tm_stat._time_stamp = clock(); }

void Tm_taken(const char* msg) {
  clock_t cur_stamp = clock();
  fprintf(stdout, "[RT_STAT] %s takes %.3f seconds.\n", msg,
          ((double)(cur_stamp - Tm_stat._time_stamp)) / (double)CLOCKS_PER_SEC);
  Tm_stat._time_stamp = cur_stamp;
}
