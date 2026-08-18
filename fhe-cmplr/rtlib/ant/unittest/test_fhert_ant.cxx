//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "gtest/gtest.h"
#include "helper.h"

UT_GLOB_ENV::ENV_INFO UT_GLOB_ENV::Env_info[ENV_LAST];
/**
Main entry point for Google Test unit tests.
*/
int main(int argc, char** argv) {
  testing::InitGoogleTest(&argc, argv);
  testing::AddGlobalTestEnvironment(new UT_GLOB_ENV);

  return RUN_ALL_TESTS();
}