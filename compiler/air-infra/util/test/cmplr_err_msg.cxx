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
 * @brief test macro CMPLR_ERR_MSG
 *
 * @param file trace to file
 */
static void Test_cmplr_err_msg(const std::string file) {
  // cmplr trace err msg to file
  TFILE err(file);
  CMPLR_ERR_MSG(err, "API: CMPLR_ERR_MSG trace to file %s ... \n", file);

  // cmplr trace err msg to stdout
  TFILE err_null(NULL);
  CMPLR_ERR_MSG(err_null, "API: CMPLR_ERR_MSG trace to null ... \n");

  // cmplr trace err msg to stdout
  TFILE err_stdout;
  CMPLR_ERR_MSG(err_stdout, "API: CMPLR_ERR_MSG trace to stdout ... \n");
}

int main() {
  // const char *file = "cmplr_err_msg.t";
  std::string path = __FILE__;
  std::string file = path.substr(path.find_last_of("/\\") + 1);
  file             = file.substr(0, file.find_last_of(".")) + ".t";

  Test_cmplr_err_msg(file);

  return 0;
}
