//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn2a_driver.h"

int main() {
  int                     argc   = 2;
  const char*             argv[] = {"driver", "/dev/null"};
  nn::driver::NN2A_DRIVER nn2a_driver(true, argc, (char**)argv);
  nn2a_driver.Init();
  nn2a_driver.Run();
  nn2a_driver.Fini();

  return 0;
}
