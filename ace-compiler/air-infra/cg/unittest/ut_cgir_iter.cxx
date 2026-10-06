//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/cg/cgir_util.h"
#include "gtest/gtest.h"
#include "ut_air2cg_ctx.h"
#include "ut_air2cg_util.h"

namespace {

class TEST_CGIR_ITER : public ::testing::Test {
protected:
  void SetUp() override { air::core::Register_core(); }
  void TearDown() override {}

  void Test_air2cgir_if();
  void Test_air2cgir_nested_if();
  void Test_air2cgir_if_do_while();
  void Test_air2cgir_if_while_do();
  void Test_air2cgir_do_while();
  void Test_air2cgir_nested_do_while();
  void Test_air2cgir_do_while_if();
  void Test_air2cgir_while_do();
  void Test_air2cgir_nested_while_do();
  void Test_air2cgir_while_do_if();
};

void TEST_CGIR_ITER::Test_air2cgir_if() {
  TEST_AIR2CGIR_UTIL util;
  STMT_PTR           if_stmt  = util.Create_if({1, 2, 3});
  STMT_PTR           ret_stmt = util.Create_retv(4);
  STMT_LIST          sl       = util.Stmt_list();
  sl.Append(if_stmt);
  sl.Append(ret_stmt);

  CGIR_CONTAINER  cg_cont(util.Func_scope(), true);
  UT_AIR2CGIR_CTX ctx(&cg_cont);
  util.Convert_to_cgir(ctx);

  // CFG:
  //       entry(bb0)
  //          |
  //        cmp(bb2)         bb2: i != 1
  //        /    \
  //  then(bb3)  else(bb4)   bb3: x = 2
  //        \    /           bb4: x = 3
  //        merge(bb5)       bb5: ret 4
  //           |
  //         exit (bb1)

  uint32_t order_expected[] = {0, 2, 3, 4, 5, 1};
  uint32_t index            = 0;
  for (auto&& bb : LAYOUT_ITER(&cg_cont)) {
    EXPECT_EQ(order_expected[index++], bb->Id().Value());
  }
  EXPECT_EQ(index, 6);
}

void TEST_CGIR_ITER::Test_air2cgir_nested_if() {
  TEST_AIR2CGIR_UTIL util;
  STMT_PTR           if_stmt  = util.Create_if({1, 2, 3});
  STMT_PTR           then_if  = util.Create_if({4, 5, 6});
  STMT_PTR           else_if  = util.Create_if({7, 8, 9});
  STMT_PTR           ret_stmt = util.Create_retv(10);
  STMT_LIST          then_sl  = STMT_LIST(if_stmt->Node()->Then_blk());
  then_sl.Append(then_if);
  STMT_LIST else_sl = STMT_LIST(if_stmt->Node()->Else_blk());
  else_sl.Append(else_if);
  STMT_LIST sl = util.Stmt_list();
  sl.Append(if_stmt);
  sl.Append(ret_stmt);

  CGIR_CONTAINER  cg_cont(util.Func_scope(), true);
  UT_AIR2CGIR_CTX ctx(&cg_cont);
  util.Convert_to_cgir(ctx);

  // CFG:
  //           entry(bb0)               bb2:  i == 1
  //              |                     bb3:  x =  2
  //            cmp(bb2)                      i == 4
  //           /        \               bb4:  x =  5
  //     cmp(bb3)       cmp(bb7)        bb5:  x =  6
  //     /    \          /    \         bb6:  N/A
  //  then(4) else(5) then(8) else(9)   bb7:  x =  3
  //     \    /          \    /               i == 7
  //    merge(bb6)     merge(bb10)      bb8:  x =  8
  //           \        /               bb9:  x =  9
  //            merge(bb11)             bb10: N/A
  //               |                    bb11: ret  10
  //             exit(bb1)

  uint32_t order_expected[] = {0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 1};
  uint32_t index            = 0;
  for (auto&& bb : LAYOUT_ITER(&cg_cont)) {
    EXPECT_EQ(order_expected[index++], bb->Id().Value());
  }
  EXPECT_EQ(index, 12);
}

void TEST_CGIR_ITER::Test_air2cgir_if_do_while() {
  TEST_AIR2CGIR_UTIL util;
  STMT_PTR           if_stmt   = util.Create_if({1, 2, 3});
  STMT_PTR           then_loop = util.Create_do_loop({4, 5, 6, 7});
  STMT_PTR           else_loop = util.Create_do_loop({8, 9, 10, 11});
  STMT_PTR           ret_stmt  = util.Create_retv(12);
  STMT_LIST          then_sl   = STMT_LIST(if_stmt->Node()->Then_blk());
  then_sl.Append(then_loop);
  STMT_LIST else_sl = STMT_LIST(if_stmt->Node()->Else_blk());
  else_sl.Append(else_loop);
  STMT_LIST sl = util.Stmt_list();
  sl.Append(if_stmt);
  sl.Append(ret_stmt);

  CGIR_CONTAINER  cg_cont(util.Func_scope(), true);
  UT_AIR2CGIR_CTX ctx(&cg_cont);
  util.Convert_to_cgir(ctx);

  // CFG:
  //              entry(bb0)                bb2:  i != 1
  //                 |                      bb3:  i =  4
  //               cmp(bb2)                 bb4:  x =  6
  //              /       \                       i += 7
  //           init(bb3)  init(bb6)               i <  5
  //             |          |               bb5:  N/A
  //      +->  body(bb4)  body(bb7) <-+     bb6:  i =  8
  //      |    incr(bb4)  incr(bb7)   |     bb7:  x =  10
  //      +---  cmp(bb4)   cmp(bb7) --+           i += 11
  //             |          |                     i <  9
  //           tail(bb5)  tail(bb8)         bb8:  N/A
  //              \        /                bb9:  ret 12
  //               merge(bb9)
  //                 |
  //               exit(bb1)

  uint32_t order_expected[] = {0, 2, 3, 4, 5, 6, 7, 8, 9, 1};
  uint32_t index            = 0;
  for (auto&& bb : LAYOUT_ITER(&cg_cont)) {
    EXPECT_EQ(order_expected[index++], bb->Id().Value());
  }
  EXPECT_EQ(index, 10);
}

void TEST_CGIR_ITER::Test_air2cgir_if_while_do() {
  TEST_AIR2CGIR_UTIL util;
  STMT_PTR           if_stmt   = util.Create_if({1, 2, 3});
  STMT_PTR           then_loop = util.Create_do_loop({4, 5, 6, 7});
  STMT_PTR           else_loop = util.Create_do_loop({8, 9, 10, 11});
  STMT_PTR           ret_stmt  = util.Create_retv(12);
  STMT_LIST          then_sl   = STMT_LIST(if_stmt->Node()->Then_blk());
  then_sl.Append(then_loop);
  STMT_LIST else_sl = STMT_LIST(if_stmt->Node()->Else_blk());
  else_sl.Append(else_loop);
  STMT_LIST sl = util.Stmt_list();
  sl.Append(if_stmt);
  sl.Append(ret_stmt);

  CGIR_CONTAINER  cg_cont(util.Func_scope(), true);
  UT_AIR2CGIR_CTX ctx(&cg_cont, true);
  util.Convert_to_cgir(ctx);

  // CFG:
  //             entry(bb0)                bb2:  i != 1
  //                |                      bb3:  i =  4
  //              cmp(bb2)                 bb4:  i <  5
  //             /        \                bb5:  x =  6
  //     init(bb3)     init(bb7)                 i += 7
  //       |             |                 bb6:  N/A
  //   +->cmp(bb4)-+ +->cmp(bb8) --+       bb7:  i =  8
  //   |   |       | |   |         |       bb8:  i <  9
  //   | body(bb5) | |  body(bb9)  |       bb9:  x =  10
  //   +-incr(bb5) | +- incr(bb9)  |             i += 11
  //               V               V       bb10: N/A
  //            tail(bb6)    tail(bb10)    bb11: ret 12
  //                    \     /
  //                    merge(bb11)
  //                       |
  //                     exit(bb1)

  uint32_t order_expected[] = {0, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 1};
  uint32_t index            = 0;
  for (auto&& bb : LAYOUT_ITER(&cg_cont)) {
    EXPECT_EQ(order_expected[index++], bb->Id().Value());
  }
  EXPECT_EQ(index, 12);
}

void TEST_CGIR_ITER::Test_air2cgir_do_while() {
  TEST_AIR2CGIR_UTIL util;
  STMT_PTR           if_stmt  = util.Create_do_loop({1, 2, 3, 4});
  STMT_PTR           ret_stmt = util.Create_retv(5);
  STMT_LIST          sl       = util.Stmt_list();
  sl.Append(if_stmt);
  sl.Append(ret_stmt);

  CGIR_CONTAINER  cg_cont(util.Func_scope(), true);
  UT_AIR2CGIR_CTX ctx(&cg_cont, false);
  util.Convert_to_cgir(ctx);

  // CFG:
  //     entry(bb0)
  //       |
  //     init(bb2)     bb2: i =  1
  //       |
  //  +->body(bb3)     bb3: x =  3
  //  |  incr(bb3)          i += 4
  //  +- cmp(bb3)           i <  2
  //       |
  //     tail(bb4)     bb4: ret  5
  //       |
  //     exit(bb0)

  uint32_t order_expected[] = {0, 2, 3, 4, 1};
  uint32_t index            = 0;
  for (auto&& bb : LAYOUT_ITER(&cg_cont)) {
    EXPECT_EQ(order_expected[index++], bb->Id().Value());
  }
  EXPECT_EQ(index, 5);
}

void TEST_CGIR_ITER::Test_air2cgir_nested_do_while() {
  TEST_AIR2CGIR_UTIL util;
  STMT_PTR           do_stmt   = util.Create_do_loop({1, 2, 3, 4});
  STMT_PTR           nested_do = util.Create_do_loop({5, 6, 7, 8});
  STMT_PTR           ret_stmt  = util.Create_retv(9);
  STMT_LIST          body_sl   = STMT_LIST(do_stmt->Node()->Body_blk());
  body_sl.Append(nested_do);
  STMT_LIST sl = util.Stmt_list();
  sl.Append(do_stmt);
  sl.Append(ret_stmt);

  CGIR_CONTAINER  cg_cont(util.Func_scope(), true);
  UT_AIR2CGIR_CTX ctx(&cg_cont);
  util.Convert_to_cgir(ctx);

  // CFG:
  //      entry(bb0)
  //         |
  //       init(bb2)         bb2: i =  1
  //         |               bb3: x =  3
  //  +--->init(bb3)              i =  5
  //  |      |               bb4: x =  7
  //  | +->body(bb4)              i += 8
  //  | |  incr(bb4)              i <  6
  //  | +- cmp(bb4)          bb5: i += 4
  //  |      |                    i <  2
  //  |   incr(bb5)          bb6: ret  9
  //  +--- cmp(bb5)
  //         |
  //       tail(bb6)
  //         |
  //       exit(bb1)

  uint32_t order_expected[] = {0, 2, 3, 4, 5, 6, 1};
  uint32_t index            = 0;
  for (auto&& bb : LAYOUT_ITER(&cg_cont)) {
    EXPECT_EQ(order_expected[index++], bb->Id().Value());
  }
  EXPECT_EQ(index, 7);
}

void TEST_CGIR_ITER::Test_air2cgir_do_while_if() {
  TEST_AIR2CGIR_UTIL util;
  STMT_PTR           do_stmt  = util.Create_do_loop({1, 2, 3, 4});
  STMT_PTR           if_stmt  = util.Create_if({5, 6, 7});
  STMT_PTR           ret_stmt = util.Create_retv(8);
  STMT_LIST          body_sl  = STMT_LIST(do_stmt->Node()->Body_blk());
  body_sl.Append(if_stmt);
  STMT_LIST sl = util.Stmt_list();
  sl.Append(do_stmt);
  sl.Append(ret_stmt);

  CGIR_CONTAINER  cg_cont(util.Func_scope(), true);
  UT_AIR2CGIR_CTX ctx(&cg_cont);
  util.Convert_to_cgir(ctx);

  // CFG:
  //      entry(bb0)
  //          |
  //       init(bb2)         bb2: i =  1
  //          |              bb3: x =  3
  //  +--->body(bb3)              i != 5
  //  |     cmp(bb3)         bb4: x =  6
  //  |     /    \           bb5: x =  7
  //  | then(4)  else(5)     bb6: i += 4
  //  |     \    /                i <  2
  //  |     merge(bb6)       bb7: ret  8
  //  |     incr(bb6)
  //  +---- cmp(bb6)
  //          |
  //       tail(bb7)
  //          |
  //       exit(bb1)

  uint32_t order_expected[] = {0, 2, 3, 4, 5, 6, 7, 1};
  uint32_t index            = 0;
  for (auto&& bb : LAYOUT_ITER(&cg_cont)) {
    EXPECT_EQ(order_expected[index++], bb->Id().Value());
  }
  EXPECT_EQ(index, 8);
}

void TEST_CGIR_ITER::Test_air2cgir_while_do() {
  TEST_AIR2CGIR_UTIL util;
  STMT_PTR           if_stmt  = util.Create_do_loop({1, 2, 3, 4});
  STMT_PTR           ret_stmt = util.Create_retv(5);
  STMT_LIST          sl       = util.Stmt_list();
  sl.Append(if_stmt);
  sl.Append(ret_stmt);

  CGIR_CONTAINER  cg_cont(util.Func_scope(), true);
  UT_AIR2CGIR_CTX ctx(&cg_cont, true);
  util.Convert_to_cgir(ctx);

  // CFG:
  //     entry(bb0)
  //       |
  //     init(bb2)       bb2: i = 1
  //       |
  //  +-- cmp(bb3)<-+    bb3: i < 2
  //  |    |        |
  //  |  body(bb4)  |    bb4: x = 3
  //  |  incr(bb4) -+         i += 4
  //  V
  // tail(bb5)           bb5: ret 5
  //  |
  // exit(bb1)

  uint32_t order_expected[] = {0, 2, 3, 4, 5, 1};
  uint32_t index            = 0;
  for (auto&& bb : LAYOUT_ITER(&cg_cont)) {
    EXPECT_EQ(order_expected[index++], bb->Id().Value());
  }
  EXPECT_EQ(index, 6);
}

void TEST_CGIR_ITER::Test_air2cgir_nested_while_do() {
  TEST_AIR2CGIR_UTIL util;
  STMT_PTR           do_stmt   = util.Create_do_loop({1, 2, 3, 4});
  STMT_PTR           nested_do = util.Create_do_loop({5, 6, 7, 8});
  STMT_PTR           ret_stmt  = util.Create_retv(9);
  STMT_LIST          body_sl   = STMT_LIST(do_stmt->Node()->Body_blk());
  body_sl.Append(nested_do);
  STMT_LIST sl = util.Stmt_list();
  sl.Append(do_stmt);
  sl.Append(ret_stmt);

  CGIR_CONTAINER  cg_cont(util.Func_scope(), true);
  UT_AIR2CGIR_CTX ctx(&cg_cont, true);
  util.Convert_to_cgir(ctx);

  // CFG:
  //       entry(bb0)
  //         |
  //       init(bb2)               bb2:  i =  1
  //         |                     bb3:  i <  2
  // +----> cmp(bb3) -----+        bb4:  x =  3
  // |       |            |              i =  5
  // |     body(bb4)   tail(bb8)   bb5:  i <  6
  // |     init(bb4)      |        bb6:  x =  7
  // |       |         exit(bb1)         i += 8
  // |   +--cmp(bb5)<-+            bb7:  i += 4
  // |   |   |        |            bb8:  ret  9
  // |   | body(bb6)  |
  // |   | incr(bb6) -+
  // |   |
  // |   |
  // |   V
  // +-incr(bb7)

  uint32_t order_expected[] = {0, 2, 3, 4, 5, 6, 7, 8, 1};
  uint32_t index            = 0;
  for (auto&& bb : LAYOUT_ITER(&cg_cont)) {
    EXPECT_EQ(order_expected[index++], bb->Id().Value());
  }
  EXPECT_EQ(index, 9);
}

void TEST_CGIR_ITER::Test_air2cgir_while_do_if() {
  TEST_AIR2CGIR_UTIL util;
  STMT_PTR           do_stmt  = util.Create_do_loop({1, 2, 3, 4});
  STMT_PTR           if_stmt  = util.Create_if({5, 6, 7});
  STMT_PTR           ret_stmt = util.Create_retv(8);
  STMT_LIST          body_sl  = STMT_LIST(do_stmt->Node()->Body_blk());
  body_sl.Append(if_stmt);
  STMT_LIST sl = util.Stmt_list();
  sl.Append(do_stmt);
  sl.Append(ret_stmt);

  CGIR_CONTAINER  cg_cont(util.Func_scope(), true);
  UT_AIR2CGIR_CTX ctx(&cg_cont, true);
  util.Convert_to_cgir(ctx);

  // CFG:
  //       entry(bb0)
  //         |
  //       init(bb2)         bb2:  i =  1
  //         |               bb3:  i <  2
  //  +--- cmp(bb3) <----+   bb4:  x =  3
  //  |      |           |         i != 5
  //  |    body(bb4)     |   bb5:  x =  6
  //  |    cmp(bb4)      |   bb6:  x =  7
  //  |     /     \      |   bb7:  i += 4
  //  | then(5)  else(6) |   bb8:  ret  8
  //  |     \     /      |
  //  |    merge(bb7)    |
  //  |    incr(bb7) ----+
  //  V
  // tail(bb8)
  //  |
  // exit(bb1)

  uint32_t order_expected[] = {0, 2, 3, 4, 5, 6, 7, 8, 1};
  uint32_t index            = 0;
  for (auto&& bb : LAYOUT_ITER(&cg_cont)) {
    EXPECT_EQ(order_expected[index++], bb->Id().Value());
  }
  EXPECT_EQ(index, 9);
}

TEST_F(TEST_CGIR_ITER, air2cgir_if) { Test_air2cgir_if(); }

TEST_F(TEST_CGIR_ITER, air2cgir_nested_if) { Test_air2cgir_nested_if(); }

TEST_F(TEST_CGIR_ITER, air2cgir_if_do_while) { Test_air2cgir_if_do_while(); }

TEST_F(TEST_CGIR_ITER, air2cgir_if_while_do) { Test_air2cgir_if_while_do(); }

TEST_F(TEST_CGIR_ITER, air2cgir_do_while) { Test_air2cgir_do_while(); }

TEST_F(TEST_CGIR_ITER, air2cgir_nested_do_while) {
  Test_air2cgir_nested_do_while();
}

TEST_F(TEST_CGIR_ITER, air2cgir_do_while_if) { Test_air2cgir_do_while_if(); }

TEST_F(TEST_CGIR_ITER, air2cgir_while_do) { Test_air2cgir_while_do(); }

TEST_F(TEST_CGIR_ITER, air2cgir_nested_while_do) {
  Test_air2cgir_nested_while_do();
}

TEST_F(TEST_CGIR_ITER, air2cgir_while_do_if) { Test_air2cgir_while_do_if(); }

}  // namespace
