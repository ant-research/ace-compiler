//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/util/debug.h"

using namespace air::util;

/**
 * @brief test macro AIR_TRACE
 *
 * @param file only support trace to file
 */
static void Test_air_trace(const std::string file) {
  TFILE trace(file);
  AIR_TRACE(trace, "API: AIR_TRACE to file %s \n", file);

  // must prints value to file , so no trace output
  TFILE trace_null(NULL);
  AIR_TRACE(trace_null, "API: AIR_TRACE to file null .... \n");

  // must prints value to file , so no trace output
  TFILE trace_cout;
  AIR_TRACE(trace_cout, "API: AIR_TRACE to stdout .... \n");
}

int main() {
  // const char *file = "air_trace.t";
  std::string path = __FILE__;
  std::string file = path.substr(path.find_last_of("/\\") + 1);
  file             = file.substr(0, file.find_last_of(".")) + ".t";

  Test_air_trace(file);

  return 0;
}
