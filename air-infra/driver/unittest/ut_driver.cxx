//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <limits.h>

#include <filesystem>

#include "air/driver/driver.h"
#include "air/util/debug.h"
#include "gtest/gtest.h"

using namespace air::driver;
using namespace air::util;

namespace {

TEST(DRIVER_CTX, TFileReg) {
  {
    DRIVER_CTX ctx;
    TFILE&     tfile   = ctx.Tfile();
    TFILE&     tfile_0 = ctx.Tfile(DEFAULT_TFILE);
    TFILE&     tfile_1 = ctx.Register_tfile(DEFAULT_TFILE, "tmp.default");
    TFILE&     pfile   = ctx.Tfile(DEFAULT_PFILE);
    TFILE&     pfile_0 = ctx.Register_tfile(DEFAULT_PFILE, "tmp.default");
    EXPECT_EQ(&tfile, &tfile_0);
    EXPECT_EQ(&tfile, &tfile_1);
    EXPECT_EQ(&pfile, &pfile_0);

    TFILE& xfile   = ctx.Register_tfile(100, "tmp.xfile");
    TFILE& xfile_0 = ctx.Register_tfile(100, "tmp.xfile.0");
    TFILE& yfile   = ctx.Register_tfile(200, "tmp.yfile");
    EXPECT_EQ(&xfile, &xfile_0);
    EXPECT_EQ(&xfile, &ctx.Tfile(100));
    EXPECT_EQ(&yfile, &ctx.Tfile(200));
  }
  EXPECT_FALSE(std::filesystem::exists("tmp.default"));
  EXPECT_FALSE(std::filesystem::exists("tmp.default.t"));
  EXPECT_TRUE(std::filesystem::exists("tmp.xfile.t"));
  EXPECT_TRUE(std::filesystem::exists("tmp.yfile.t"));
  std::filesystem::remove("tmp.xfile.t");
  std::filesystem::remove("tmp.yfile.t");
}

}  // namespace
