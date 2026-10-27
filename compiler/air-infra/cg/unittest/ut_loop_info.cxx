//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/base/bb_enum.h"
#include "air/base/loop_info.h"
#include "air/cg/cgir_container.h"
#include "air/cg/cgir_decl.h"
#include "gtest/gtest.h"

using namespace air::cg;

namespace {

TEST(CGIR, CONTAINER) {
  using LOOP_INFO_MGR = air::base::LOOP_INFO_MGR<CGIR_CONTAINER, BB_ID, BB_PTR>;
  using LOOP_INFO_PTR = LOOP_INFO_MGR::LOOP_INFO_PTR;
  // create a container
  CGIR_CONTAINER cont(nullptr, false);
  // create a loop:
  // bb0 -> bb1 -> bb2 -> bb3
  //       ^----------|
  LOOP_INFO_MGR loop_mgr(&cont);
  LOOP_INFO_PTR loop = loop_mgr.New_loop();
  BB_PTR        bb0  = cont.New_bb();
  BB_PTR        bb1  = cont.New_bb();
  BB_PTR        bb2  = cont.New_bb();
  BB_PTR        bb3  = cont.New_bb();
  cont.Connect(bb0, bb1);
  cont.Connect(bb1, bb2);
  cont.Connect(bb2, bb1);
  cont.Connect(bb2, bb3);
  // set and check loop preheader
  bool res = loop_mgr.Set_preheader(loop, bb0);
  ASSERT_TRUE(res);
  ASSERT_TRUE(bb0->Has_attr(air::base::BB_LOOP_PREHEADER));
  ASSERT_FALSE(bb0->Has_attr(air::base::BB_LOOP_BODY));
  // set and check loop header
  res = loop_mgr.Set_header(loop, bb1);
  ASSERT_TRUE(res);
  uint32_t bb1_attr = (air::base::BB_LOOP_HEADER | air::base::BB_LOOP_BODY);
  ASSERT_TRUE(bb1->Has_attr(bb1_attr));
  ASSERT_TRUE(bb1->Has_attr(air::base::BB_LOOP_HEADER));
  ASSERT_TRUE(bb1->Has_attr(air::base::BB_LOOP_BODY));
  // set and check loop back
  res = loop_mgr.Set_back(loop, bb2);
  ASSERT_TRUE(res);
  uint32_t bb2_attr = (air::base::BB_LOOP_BACK | air::base::BB_LOOP_BODY);
  ASSERT_TRUE(bb2->Has_attr(bb2_attr));
  ASSERT_TRUE(bb2->Has_attr(air::base::BB_LOOP_BACK));
  ASSERT_TRUE(bb2->Has_attr(air::base::BB_LOOP_BODY));
  // set and checkout loop tail
  res = loop_mgr.Set_tail(loop, bb3);
  ASSERT_TRUE(res);
  ASSERT_TRUE(bb3->Has_attr(air::base::BB_LOOP_TAIL));
  ASSERT_FALSE(bb3->Has_attr(air::base::BB_LOOP_BODY));
  cont.Print();
}

}  // namespace