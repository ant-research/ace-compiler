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
 * @brief test macro CMPLR_WARN_MSG
 *
 * @param file trace to file
 */
static void Test_cmplr_warn_msg(const std::string file) {
  // test trace warn to file
  TFILE warn(file);
  CMPLR_WARN_MSG(warn, "API: CMPLR_WARN_MSG trace to file %s .... \n", file);

  // test trace warn to stdout
  TFILE warn_null(NULL);
  CMPLR_WARN_MSG(warn_null, "API: CMPLR_WARN_MSG trace to null .... \n");

  // test trace warn to stdout
  TFILE warn_stdout;
  CMPLR_WARN_MSG(warn_stdout, "API: CMPLR_WARN_MSG trace to stdout .... \n");
}

int main() {
  // const char *file = "cmplr_warn_msg.t";
  std::string path = __FILE__;
  std::string file = path.substr(path.find_last_of("/\\") + 1);
  file             = file.substr(0, file.find_last_of(".")) + ".t";

  Test_cmplr_warn_msg(file);

  return 0;
}
