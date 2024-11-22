//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/core/rt_data_util.h"

int main(int argc, char* argv[]) {
  if (argc == 1) {
    printf("Usage: %s <rt_data_file>\n", argv[0]);
    return 0;
  }
  fhe::core::RT_DATA_DUMPER dumper(std::cout);
  if (dumper.Dump(argv[1]) == false) {
    printf("Error: failed to dump file %s.\n", argv[1]);
    return 1;
  }
  return 0;
}
