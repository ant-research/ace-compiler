//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <stdlib.h>
#include <string.h>

#include "common/error.h"
#include "common/rt_api.h"
#include "common/rtlib_timing.h"

void Run_main_graph() {
  RTLIB_TM_START(RTM_MAIN_GRAPH, rtm);
  bool ret = Main_graph();
  RTLIB_TM_END(RTM_MAIN_GRAPH, rtm);
  FMT_ASSERT(ret, "run Main_graph failed\n");
}
