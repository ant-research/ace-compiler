//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/util/uuid.h"
#include "gtest/gtest.h"

namespace {

TEST(uuid, UUID_API) {
  air::util::UUID u1;

  air::util::UUID u2(u1);
  EXPECT_EQ(u1, u2);
  EXPECT_EQ(u1.To_str(), u2.To_str());
  EXPECT_STREQ(u1.To_str().c_str(), u2.To_str().c_str());

  const char*     id_str_u = "DB72FAD2-8BFF-44E3-A128-1BA8CD9BF820";
  const char*     id_str_l = "db72fad2-8bff-44e3-a128-1ba8cd9bf820";
  air::util::UUID u3(id_str_u);
  air::util::UUID u4(id_str_l);
  EXPECT_EQ(u3, u4);

  u2 = u4;
  EXPECT_EQ(u2, u3);
  EXPECT_NE(u1, u2);

  std::string uid = u3.To_str();
  EXPECT_EQ(strcmp(uid.c_str(), id_str_u), 0);
  EXPECT_EQ(strcasecmp(uid.c_str(), id_str_l), 0);
}

}  // namespace
