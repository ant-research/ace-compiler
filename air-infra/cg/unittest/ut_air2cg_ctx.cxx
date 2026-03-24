//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "ut_air2cg_ctx.h"

#include "gtest/gtest.h"
#include "ut_air2cg_util.h"

namespace {

class TEST_AIR2CGIR : public ::testing::Test {
protected:
  void SetUp() override {
    air::core::Register_core();
    Ut_register_ti();
  }
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

void TEST_AIR2CGIR::Test_air2cgir_if() {
  TEST_AIR2CGIR_UTIL util;
  STMT_PTR           if_stmt  = util.Create_if({1, 2, 3});
  STMT_PTR           ret_stmt = util.Create_retv(4);
  STMT_LIST          sl       = util.Stmt_list();
  sl.Append(if_stmt);
  sl.Append(ret_stmt);

  CGIR_CONTAINER cg_cont(util.Func_scope(), true);
  EXPECT_EQ(cg_cont.Bb_count(), 2);
  BB_PTR entry = cg_cont.Entry();  // bb0
  BB_PTR exit  = cg_cont.Exit();   // bb1

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

  // should create 4 new BB and 6 edges for if
  EXPECT_EQ(cg_cont.Bb_count(), 6);
  EXPECT_EQ(cg_cont.Edge_count(), 6);
  // entry/exit should be the same
  EXPECT_EQ(entry, cg_cont.Entry());
  EXPECT_EQ(exit, cg_cont.Exit());

  // verify bb successors and instructions
  // entry should has 1 succ
  EXPECT_EQ(entry->Succ_cnt(), 1);
  // bb2 succ contains "1" and has 2 succ
  BB_PTR bb2 = cg_cont.Bb(entry->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(1), bb2->Id().Value());
  EXPECT_EQ(bb2->Succ_cnt(), 2);
  // bb2 first succ contains "2", then bb, and has 1 succ
  BB_PTR bb3 = cg_cont.Bb(bb2->Succ_id(0));
  EXPECT_EQ(bb3->Succ_cnt(), 1);
  EXPECT_EQ(ctx.Val_bb(2), bb3->Id().Value());
  // bb2 second succ contains "3", else bb, and has 1 succ
  BB_PTR bb4 = cg_cont.Bb(bb2->Succ_id(1));
  EXPECT_EQ(bb4->Succ_cnt(), 1);
  EXPECT_EQ(ctx.Val_bb(3), bb4->Id().Value());
  // bb3 and bb4 has same succ
  EXPECT_EQ(bb3->Succ_id(0), bb4->Succ_id(0));
  // bb5 contains "4" and has 1 succ, exit
  BB_PTR bb5 = cg_cont.Bb(bb3->Succ_id(0));
  EXPECT_EQ(bb5->Succ_cnt(), 1);
  EXPECT_EQ(ctx.Val_bb(4), bb5->Id().Value());
  EXPECT_EQ(bb5->Succ_id(0), exit->Id());

  // verify predecessors
  // exit bb has 1 pred, which is bb5
  EXPECT_EQ(exit->Pred_cnt(), 1);
  EXPECT_EQ(exit->Pred_id(0), bb5->Id());
  // bb5 has 2 pred, which is bb3 & bb4
  EXPECT_EQ(bb5->Pred_cnt(), 2);
  EXPECT_EQ(bb5->Pred_id(0), bb3->Id());
  EXPECT_EQ(bb5->Pred_id(1), bb4->Id());
  // bb3 and bb4 has 1 pred, which is bb2
  EXPECT_EQ(bb3->Pred_cnt(), 1);
  EXPECT_EQ(bb4->Pred_cnt(), 1);
  EXPECT_EQ(bb3->Pred_id(0), bb2->Id());
  EXPECT_EQ(bb4->Pred_id(0), bb2->Id());
  // bb2 has 1 pred, which is entry
  EXPECT_EQ(bb2->Pred_cnt(), 1);
  EXPECT_EQ(bb2->Pred_id(0), entry->Id());

  // verify BB layout
  // entry(bb0) - bb2 - bb3 - bb4 - bb5 - exit(bb1)
  EXPECT_EQ(entry->Next_id(), bb2->Id());
  EXPECT_EQ(bb2->Next_id(), bb3->Id());
  EXPECT_EQ(bb3->Next_id(), bb4->Id());
  EXPECT_EQ(bb4->Next_id(), bb5->Id());
  EXPECT_EQ(bb5->Next_id(), exit->Id());
  EXPECT_EQ(exit->Next_id(), NULL_BB_ID);
  EXPECT_EQ(exit->Prev_id(), bb5->Id());
  EXPECT_EQ(bb5->Prev_id(), bb4->Id());
  EXPECT_EQ(bb4->Prev_id(), bb3->Id());
  EXPECT_EQ(bb3->Prev_id(), bb2->Id());
  EXPECT_EQ(bb2->Prev_id(), entry->Id());
  EXPECT_EQ(entry->Prev_id(), NULL_BB_ID);
}

void TEST_AIR2CGIR::Test_air2cgir_nested_if() {
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

  CGIR_CONTAINER cg_cont(util.Func_scope(), true);
  EXPECT_EQ(cg_cont.Bb_count(), 2);
  BB_PTR entry = cg_cont.Entry();  // bb0
  BB_PTR exit  = cg_cont.Exit();   // bb1

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

  // should create 10 new BB and 14 edges for nested if
  EXPECT_EQ(cg_cont.Bb_count(), 12);
  EXPECT_EQ(cg_cont.Edge_count(), 14);
  // entry/exit should be the same
  EXPECT_EQ(entry, cg_cont.Entry());
  EXPECT_EQ(exit, cg_cont.Exit());

  // verify bb successors and instructions
  // entry should has 1 succ
  EXPECT_EQ(entry->Succ_cnt(), 1);
  // bb2 succ contains "1" and has 2 succ
  BB_PTR bb2 = cg_cont.Bb(entry->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(1), bb2->Id().Value());
  EXPECT_EQ(bb2->Succ_cnt(), 2);
  // bb2 first succ contains "2" "4", and has 2 succ
  BB_PTR bb3 = cg_cont.Bb(bb2->Succ_id(0));
  EXPECT_EQ(bb3->Succ_cnt(), 2);
  EXPECT_EQ(ctx.Val_bb(2), bb3->Id().Value());
  EXPECT_EQ(ctx.Val_bb(4), bb3->Id().Value());
  // bb3 first succ contains "5", and has 1 succ
  BB_PTR bb4 = cg_cont.Bb(bb3->Succ_id(0));
  EXPECT_EQ(bb4->Succ_cnt(), 1);
  EXPECT_EQ(ctx.Val_bb(5), bb4->Id().Value());
  // bb3 second succ contains "6", and has 1 succ
  BB_PTR bb5 = cg_cont.Bb(bb3->Succ_id(1));
  EXPECT_EQ(bb5->Succ_cnt(), 1);
  EXPECT_EQ(ctx.Val_bb(6), bb5->Id().Value());
  // bb4 and bb5 has same succ: bb6, has 1 succ
  BB_PTR bb6 = cg_cont.Bb(bb4->Succ_id(0));
  EXPECT_EQ(bb6->Succ_cnt(), 1);
  EXPECT_EQ(bb4->Succ_id(0), bb5->Succ_id(0));
  // bb2 second succ contains "3" "7", and has 2 succ
  BB_PTR bb7 = cg_cont.Bb(bb2->Succ_id(1));
  EXPECT_EQ(bb7->Succ_cnt(), 2);
  EXPECT_EQ(ctx.Val_bb(3), bb7->Id().Value());
  EXPECT_EQ(ctx.Val_bb(7), bb7->Id().Value());
  // bb7 first succ contains "8", and has 1 succ
  BB_PTR bb8 = cg_cont.Bb(bb7->Succ_id(0));
  EXPECT_EQ(bb8->Succ_cnt(), 1);
  EXPECT_EQ(ctx.Val_bb(8), bb8->Id().Value());
  // bb7 second succ contains "9", and has 1 succ
  BB_PTR bb9 = cg_cont.Bb(bb7->Succ_id(1));
  EXPECT_EQ(bb9->Succ_cnt(), 1);
  EXPECT_EQ(ctx.Val_bb(9), bb9->Id().Value());
  // bb8 and bb9 has same succ: bb10, has 1 succ
  BB_PTR bb10 = cg_cont.Bb(bb8->Succ_id(0));
  EXPECT_EQ(bb10->Succ_cnt(), 1);
  EXPECT_EQ(bb8->Succ_id(0), bb9->Succ_id(0));
  // bb6 and bb10 has same succ, bb11, contain "10"
  BB_PTR bb11 = cg_cont.Bb(bb6->Succ_id(0));
  EXPECT_EQ(bb6->Succ_id(0), bb10->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(10), bb11->Id().Value());
  // bb11 has 1 succ, exit
  EXPECT_EQ(bb11->Succ_cnt(), 1);
  EXPECT_EQ(bb11->Succ_id(0), exit->Id());

  // verify predecessors
  // exit bb has 1 pred, which is bb11
  EXPECT_EQ(exit->Pred_cnt(), 1);
  EXPECT_EQ(exit->Pred_id(0), bb11->Id());
  // bb11 has 2 preds, which is bb6 & bb10
  EXPECT_EQ(bb11->Pred_cnt(), 2);
  EXPECT_EQ(bb11->Pred_id(0), bb6->Id());
  EXPECT_EQ(bb11->Pred_id(1), bb10->Id());
  // bb10 has 2 preds, which is bb8 & bb9
  EXPECT_EQ(bb10->Pred_cnt(), 2);
  EXPECT_EQ(bb10->Pred_id(0), bb8->Id());
  EXPECT_EQ(bb10->Pred_id(1), bb9->Id());
  // bb9 has 1 pred, which is bb7
  EXPECT_EQ(bb9->Pred_cnt(), 1);
  EXPECT_EQ(bb9->Pred_id(0), bb7->Id());
  // bb8 has 1 pred, which is bb7
  EXPECT_EQ(bb8->Pred_cnt(), 1);
  EXPECT_EQ(bb8->Pred_id(0), bb7->Id());
  // bb7 has 1 pred, which is bb2
  EXPECT_EQ(bb7->Pred_cnt(), 1);
  EXPECT_EQ(bb7->Pred_id(0), bb2->Id());
  // bb6 has 2 preds, which is bb4 & bb5
  EXPECT_EQ(bb6->Pred_cnt(), 2);
  EXPECT_EQ(bb6->Pred_id(0), bb4->Id());
  EXPECT_EQ(bb6->Pred_id(1), bb5->Id());
  // bb5 has 1 pred, which is bb3
  EXPECT_EQ(bb5->Pred_cnt(), 1);
  EXPECT_EQ(bb5->Pred_id(0), bb3->Id());
  // bb4 has 1 pred, which is bb3
  EXPECT_EQ(bb4->Pred_cnt(), 1);
  EXPECT_EQ(bb4->Pred_id(0), bb3->Id());
  // bb3 has 1 pres, which is bb2
  EXPECT_EQ(bb3->Pred_cnt(), 1);
  EXPECT_EQ(bb3->Pred_id(0), bb2->Id());
  // bb2 has 1 pred, which is entry
  EXPECT_EQ(bb2->Pred_cnt(), 1);
  EXPECT_EQ(bb2->Pred_id(0), entry->Id());

  // verify bb layout
  // entry - bb2 - bb3 - bb4 - bb5 - bb6 - bb7 - bb8 - bb9 - bb10 - bb11 - exit
  EXPECT_EQ(entry->Next_id(), bb2->Id());
  EXPECT_EQ(bb2->Next_id(), bb3->Id());
  EXPECT_EQ(bb3->Next_id(), bb4->Id());
  EXPECT_EQ(bb4->Next_id(), bb5->Id());
  EXPECT_EQ(bb5->Next_id(), bb6->Id());
  EXPECT_EQ(bb6->Next_id(), bb7->Id());
  EXPECT_EQ(bb7->Next_id(), bb8->Id());
  EXPECT_EQ(bb8->Next_id(), bb9->Id());
  EXPECT_EQ(bb9->Next_id(), bb10->Id());
  EXPECT_EQ(bb10->Next_id(), bb11->Id());
  EXPECT_EQ(bb11->Next_id(), exit->Id());
  EXPECT_EQ(exit->Next_id(), NULL_BB_ID);
  EXPECT_EQ(exit->Prev_id(), bb11->Id());
  EXPECT_EQ(bb11->Prev_id(), bb10->Id());
  EXPECT_EQ(bb10->Prev_id(), bb9->Id());
  EXPECT_EQ(bb9->Prev_id(), bb8->Id());
  EXPECT_EQ(bb8->Prev_id(), bb7->Id());
  EXPECT_EQ(bb7->Prev_id(), bb6->Id());
  EXPECT_EQ(bb6->Prev_id(), bb5->Id());
  EXPECT_EQ(bb5->Prev_id(), bb4->Id());
  EXPECT_EQ(bb4->Prev_id(), bb3->Id());
  EXPECT_EQ(bb3->Prev_id(), bb2->Id());
  EXPECT_EQ(bb2->Prev_id(), entry->Id());
  EXPECT_EQ(entry->Prev_id(), NULL_BB_ID);
}

void TEST_AIR2CGIR::Test_air2cgir_if_do_while() {
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

  CGIR_CONTAINER cg_cont(util.Func_scope(), true);
  EXPECT_EQ(cg_cont.Bb_count(), 2);
  BB_PTR entry = cg_cont.Entry();  // bb0
  BB_PTR exit  = cg_cont.Exit();   // bb1

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

  // should create 8 new BB and 12 edges for if-do-while
  EXPECT_EQ(cg_cont.Bb_count(), 10);
  EXPECT_EQ(cg_cont.Edge_count(), 12);
  // entry/exit should be the same
  EXPECT_EQ(entry, cg_cont.Entry());
  EXPECT_EQ(exit, cg_cont.Exit());

  // verify bb successors and instructions
  // entry should has 1 succ
  EXPECT_EQ(entry->Succ_cnt(), 1);
  // bb2 succ contains "1" and has 2 succ
  BB_PTR bb2 = cg_cont.Bb(entry->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(1), bb2->Id().Value());
  EXPECT_EQ(bb2->Succ_cnt(), 2);
  // bb3 first succ of bb2, contains "4", and has 1 succ
  BB_PTR bb3 = cg_cont.Bb(bb2->Succ_id(0));
  EXPECT_EQ(bb3->Succ_cnt(), 1);
  EXPECT_EQ(ctx.Val_bb(4), bb3->Id().Value());
  // bb4 contains "6" "7" "5", and has 2 succ
  BB_PTR bb4 = cg_cont.Bb(bb3->Succ_id(0));
  EXPECT_EQ(bb4->Succ_cnt(), 2);
  EXPECT_EQ(ctx.Val_bb(6), bb4->Id().Value());
  EXPECT_EQ(ctx.Val_bb(7), bb4->Id().Value());
  EXPECT_EQ(ctx.Val_bb(5), bb4->Id().Value());
  // bb5 first succ of bb4, has 1 succ
  BB_PTR bb5 = cg_cont.Bb(bb4->Succ_id(0));
  EXPECT_EQ(bb5->Succ_cnt(), 1);
  // bb4 second succ is itself
  EXPECT_EQ(bb4->Succ_id(1), bb4->Id());
  // bb6 second succ of bb2, contains "8", and has 1 succ
  BB_PTR bb6 = cg_cont.Bb(bb2->Succ_id(1));
  EXPECT_EQ(bb6->Succ_cnt(), 1);
  EXPECT_EQ(ctx.Val_bb(8), bb6->Id().Value());
  // bb7 contains "10" "11" "9", and has 2 succ
  BB_PTR bb7 = cg_cont.Bb(bb6->Succ_id(0));
  EXPECT_EQ(bb7->Succ_cnt(), 2);
  EXPECT_EQ(ctx.Val_bb(10), bb7->Id().Value());
  EXPECT_EQ(ctx.Val_bb(11), bb7->Id().Value());
  EXPECT_EQ(ctx.Val_bb(9), bb7->Id().Value());
  // bb8 first succ of bb6, has 1 succ
  BB_PTR bb8 = cg_cont.Bb(bb7->Succ_id(0));
  EXPECT_EQ(bb8->Succ_cnt(), 1);
  // bb7 second succ is itself
  EXPECT_EQ(bb7->Succ_id(1), bb7->Id());
  // bb9 contains "12", and has 1 succ, exit
  BB_PTR bb9 = cg_cont.Bb(bb5->Succ_id(0));
  EXPECT_EQ(bb9->Succ_cnt(), 1);
  EXPECT_EQ(bb5->Succ_id(0), bb8->Succ_id(0));
  EXPECT_EQ(bb9->Succ_id(0), exit->Id());

  // verify predecessors
  // exit bb has 1 pred, which is bb9
  EXPECT_EQ(exit->Pred_cnt(), 1);
  EXPECT_EQ(exit->Pred_id(0), bb9->Id());
  // bb9 has 2 preds, bb5 & bb8
  EXPECT_EQ(bb9->Pred_cnt(), 2);
  EXPECT_EQ(bb9->Pred_id(0), bb5->Id());
  EXPECT_EQ(bb9->Pred_id(1), bb8->Id());
  // bb8 has 1 pred, which is bb7
  EXPECT_EQ(bb8->Pred_cnt(), 1);
  EXPECT_EQ(bb8->Pred_id(0), bb7->Id());
  // bb7 has 2 preds, bb6 & bb7
  EXPECT_EQ(bb7->Pred_cnt(), 2);
  EXPECT_EQ(bb7->Pred_id(0), bb6->Id());
  EXPECT_EQ(bb7->Pred_id(1), bb7->Id());
  // bb6 has 1 pred, which is bb2
  EXPECT_EQ(bb6->Pred_cnt(), 1);
  EXPECT_EQ(bb6->Pred_id(0), bb2->Id());
  // bb5 has 1 pred, which is bb4
  EXPECT_EQ(bb5->Pred_cnt(), 1);
  EXPECT_EQ(bb5->Pred_id(0), bb4->Id());
  // bb4 has 2 preds, bb3 & bb4
  EXPECT_EQ(bb4->Pred_cnt(), 2);
  EXPECT_EQ(bb4->Pred_id(0), bb3->Id());
  EXPECT_EQ(bb4->Pred_id(1), bb4->Id());
  // bb3 has 1 pred, which is bb2
  EXPECT_EQ(bb3->Pred_cnt(), 1);
  EXPECT_EQ(bb3->Pred_id(0), bb2->Id());
  // bb2 has 1 pred, which is entry
  EXPECT_EQ(bb2->Pred_cnt(), 1);
  EXPECT_EQ(bb2->Pred_id(0), entry->Id());

  // verify BB layout
  // entry(bb0) - bb2 - bb3 - bb4 - bb5 - bb6 - bb7 - bb8 - bb9 - exit(bb1)
  EXPECT_EQ(entry->Next_id(), bb2->Id());
  EXPECT_EQ(bb2->Next_id(), bb3->Id());
  EXPECT_EQ(bb3->Next_id(), bb4->Id());
  EXPECT_EQ(bb4->Next_id(), bb5->Id());
  EXPECT_EQ(bb5->Next_id(), bb6->Id());
  EXPECT_EQ(bb6->Next_id(), bb7->Id());
  EXPECT_EQ(bb7->Next_id(), bb8->Id());
  EXPECT_EQ(bb8->Next_id(), bb9->Id());
  EXPECT_EQ(bb9->Next_id(), exit->Id());
  EXPECT_EQ(exit->Next_id(), NULL_BB_ID);
  EXPECT_EQ(exit->Prev_id(), bb9->Id());
  EXPECT_EQ(bb9->Prev_id(), bb8->Id());
  EXPECT_EQ(bb8->Prev_id(), bb7->Id());
  EXPECT_EQ(bb7->Prev_id(), bb6->Id());
  EXPECT_EQ(bb6->Prev_id(), bb5->Id());
  EXPECT_EQ(bb5->Prev_id(), bb4->Id());
  EXPECT_EQ(bb4->Prev_id(), bb3->Id());
  EXPECT_EQ(bb3->Prev_id(), bb2->Id());
  EXPECT_EQ(bb2->Prev_id(), entry->Id());
  EXPECT_EQ(entry->Prev_id(), NULL_BB_ID);
}

void TEST_AIR2CGIR::Test_air2cgir_if_while_do() {
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

  CGIR_CONTAINER cg_cont(util.Func_scope(), true);
  EXPECT_EQ(cg_cont.Bb_count(), 2);
  BB_PTR entry = cg_cont.Entry();  // bb0
  BB_PTR exit  = cg_cont.Exit();   // bb1

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

  // should create 10 new BB and 14 edges for if...while_do
  EXPECT_EQ(cg_cont.Bb_count(), 12);
  EXPECT_EQ(cg_cont.Edge_count(), 14);
  // entry/exit should be the same
  EXPECT_EQ(entry, cg_cont.Entry());
  EXPECT_EQ(exit, cg_cont.Exit());

  // verify bb successors and instructions
  // entry should has 1 succ
  EXPECT_EQ(entry->Succ_cnt(), 1);
  // bb2 contains "1" and has 2 succ
  BB_PTR bb2 = cg_cont.Bb(entry->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(1), bb2->Id().Value());
  EXPECT_EQ(bb2->Succ_cnt(), 2);
  // bb3 first succ of bb2, contains "4" and has 1 succ
  BB_PTR bb3 = cg_cont.Bb(bb2->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(4), bb3->Id().Value());
  EXPECT_EQ(bb3->Succ_cnt(), 1);
  // bb4 contains "5" and has 2 succ
  BB_PTR bb4 = cg_cont.Bb(bb3->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(5), bb4->Id().Value());
  EXPECT_EQ(bb4->Succ_cnt(), 2);
  // bb5 first succ of bb4, contains "6" "7", and has 1 succ bb4
  BB_PTR bb5 = cg_cont.Bb(bb4->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(6), bb5->Id().Value());
  EXPECT_EQ(ctx.Val_bb(7), bb5->Id().Value());
  EXPECT_EQ(bb5->Succ_cnt(), 1);
  EXPECT_EQ(bb5->Succ_id(0), bb4->Id());
  // bb6 second succ of bb4, has 1 succ
  BB_PTR bb6 = cg_cont.Bb(bb4->Succ_id(1));
  EXPECT_EQ(bb6->Succ_cnt(), 1);
  // bb7 second succ of bb2, contains "8", and has 1 succ
  BB_PTR bb7 = cg_cont.Bb(bb2->Succ_id(1));
  EXPECT_EQ(ctx.Val_bb(8), bb7->Id().Value());
  EXPECT_EQ(bb7->Succ_cnt(), 1);
  // bb8 contains "9" and has 2 succ
  BB_PTR bb8 = cg_cont.Bb(bb7->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(9), bb8->Id().Value());
  EXPECT_EQ(bb8->Succ_cnt(), 2);
  // bb9 first succ of bb8, contains "10" "11", and has 1 succ bb8
  BB_PTR bb9 = cg_cont.Bb(bb8->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(10), bb9->Id().Value());
  EXPECT_EQ(ctx.Val_bb(11), bb9->Id().Value());
  EXPECT_EQ(bb9->Succ_cnt(), 1);
  EXPECT_EQ(bb9->Succ_id(0), bb8->Id());
  // bb10 second succ of bb8, has 1 succ
  BB_PTR bb10 = cg_cont.Bb(bb8->Succ_id(1));
  EXPECT_EQ(bb10->Succ_cnt(), 1);
  // bb11 succ of bb6 & bb10, contains "12" has 1 succ exit
  BB_PTR bb11 = cg_cont.Bb(bb6->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(12), bb11->Id().Value());
  EXPECT_EQ(bb6->Succ_id(0), bb10->Succ_id(0));
  EXPECT_EQ(bb11->Succ_cnt(), 1);
  EXPECT_EQ(bb11->Succ_id(0), exit->Id());

  // verify predecessors
  // exit bb has 1 pred, which is bb11
  EXPECT_EQ(exit->Pred_cnt(), 1);
  EXPECT_EQ(exit->Pred_id(0), bb11->Id());
  // bb11 has 2 pred, bb6 & bb10
  EXPECT_EQ(bb11->Pred_cnt(), 2);
  EXPECT_EQ(bb11->Pred_id(0), bb6->Id());
  EXPECT_EQ(bb11->Pred_id(1), bb10->Id());
  // bb10 has 1 pred, which is bb8
  EXPECT_EQ(bb10->Pred_cnt(), 1);
  EXPECT_EQ(bb10->Pred_id(0), bb8->Id());
  // bb9 has 1 pred, which is bb8
  EXPECT_EQ(bb9->Pred_cnt(), 1);
  EXPECT_EQ(bb9->Pred_id(0), bb8->Id());
  // bb8 has 2 pred, which is bb7 & bb9
  EXPECT_EQ(bb8->Pred_cnt(), 2);
  EXPECT_EQ(bb8->Pred_id(0), bb7->Id());
  EXPECT_EQ(bb8->Pred_id(1), bb9->Id());
  // bb7 has 1 pred, which is bb2
  EXPECT_EQ(bb7->Pred_cnt(), 1);
  EXPECT_EQ(bb7->Pred_id(0), bb2->Id());
  // bb6 has 1 pred, which is bb4
  EXPECT_EQ(bb6->Pred_cnt(), 1);
  EXPECT_EQ(bb6->Pred_id(0), bb4->Id());
  // bb5 has 1 pred, which is bb4
  EXPECT_EQ(bb5->Pred_cnt(), 1);
  EXPECT_EQ(bb5->Pred_id(0), bb4->Id());
  // bb4 has 2 pred, bb3 & bb5
  EXPECT_EQ(bb4->Pred_cnt(), 2);
  EXPECT_EQ(bb4->Pred_id(0), bb3->Id());
  EXPECT_EQ(bb4->Pred_id(1), bb5->Id());
  // bb3 has 1 pred, which is bb2
  EXPECT_EQ(bb3->Pred_cnt(), 1);
  EXPECT_EQ(bb3->Pred_id(0), bb2->Id());
  // bb2 has 1 pred, which is entry
  EXPECT_EQ(bb2->Pred_cnt(), 1);
  EXPECT_EQ(bb2->Pred_id(0), entry->Id());

  // verify BB layout
  // entry(bb0) - bb2 - bb3 - bb4 - bb5 - bb6 - bb7 - bb8 - bb9 - bb10 - bb11 -
  // exit(bb1)
  EXPECT_EQ(entry->Next_id(), bb2->Id());
  EXPECT_EQ(bb2->Next_id(), bb3->Id());
  EXPECT_EQ(bb3->Next_id(), bb4->Id());
  EXPECT_EQ(bb4->Next_id(), bb5->Id());
  EXPECT_EQ(bb5->Next_id(), bb6->Id());
  EXPECT_EQ(bb6->Next_id(), bb7->Id());
  EXPECT_EQ(bb7->Next_id(), bb8->Id());
  EXPECT_EQ(bb8->Next_id(), bb9->Id());
  EXPECT_EQ(bb9->Next_id(), bb10->Id());
  EXPECT_EQ(bb10->Next_id(), bb11->Id());
  EXPECT_EQ(bb11->Next_id(), exit->Id());
  EXPECT_EQ(exit->Next_id(), NULL_BB_ID);
  EXPECT_EQ(exit->Prev_id(), bb11->Id());
  EXPECT_EQ(bb11->Prev_id(), bb10->Id());
  EXPECT_EQ(bb10->Prev_id(), bb9->Id());
  EXPECT_EQ(bb9->Prev_id(), bb8->Id());
  EXPECT_EQ(bb8->Prev_id(), bb7->Id());
  EXPECT_EQ(bb7->Prev_id(), bb6->Id());
  EXPECT_EQ(bb6->Prev_id(), bb5->Id());
  EXPECT_EQ(bb5->Prev_id(), bb4->Id());
  EXPECT_EQ(bb4->Prev_id(), bb3->Id());
  EXPECT_EQ(bb3->Prev_id(), bb2->Id());
  EXPECT_EQ(bb2->Prev_id(), entry->Id());
  EXPECT_EQ(entry->Prev_id(), NULL_BB_ID);
}

void TEST_AIR2CGIR::Test_air2cgir_do_while() {
  TEST_AIR2CGIR_UTIL util;
  STMT_PTR           if_stmt  = util.Create_do_loop({1, 2, 3, 4});
  STMT_PTR           ret_stmt = util.Create_retv(5);
  STMT_LIST          sl       = util.Stmt_list();
  sl.Append(if_stmt);
  sl.Append(ret_stmt);

  CGIR_CONTAINER cg_cont(util.Func_scope(), true);
  EXPECT_EQ(cg_cont.Bb_count(), 2);
  BB_PTR entry = cg_cont.Entry();  // bb0
  BB_PTR exit  = cg_cont.Exit();   // bb1

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

  // should create 3 new BB and 5 edges for do_while
  EXPECT_EQ(cg_cont.Bb_count(), 5);
  EXPECT_EQ(cg_cont.Edge_count(), 5);
  // entry/exit should be the same
  EXPECT_EQ(entry, cg_cont.Entry());
  EXPECT_EQ(exit, cg_cont.Exit());

  // verify bb successors and instructions
  // entry should has 1 succ
  EXPECT_EQ(entry->Succ_cnt(), 1);
  // bb2 contains "1" and has 1 succ (init)
  BB_PTR bb2 = cg_cont.Bb(entry->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(1), bb2->Id().Value());
  EXPECT_EQ(bb2->Succ_cnt(), 1);
  // bb3 contains "2", "3", "4" and has 2 succ (body, incr, cmp)
  BB_PTR bb3 = cg_cont.Bb(bb2->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(2), bb3->Id().Value());
  EXPECT_EQ(ctx.Val_bb(3), bb3->Id().Value());
  EXPECT_EQ(ctx.Val_bb(4), bb3->Id().Value());
  EXPECT_EQ(bb3->Succ_cnt(), 2);
  // bb3 first succ contains "5", tail bb, and has 1 succ
  BB_PTR bb4 = cg_cont.Bb(bb3->Succ_id(0));
  EXPECT_EQ(bb4->Succ_cnt(), 1);
  EXPECT_EQ(ctx.Val_bb(5), bb4->Id().Value());
  EXPECT_EQ(bb4->Succ_id(0), exit->Id());
  // bb3 second succ is bb3 itseld
  EXPECT_EQ(bb3->Id(), bb3->Succ_id(1));

  // verify predecessors
  // exit bb has 1 pred, which is bb4
  EXPECT_EQ(exit->Pred_cnt(), 1);
  EXPECT_EQ(exit->Pred_id(0), bb4->Id());
  // bb4 has 1 pred, which is bb3
  EXPECT_EQ(bb4->Pred_cnt(), 1);
  EXPECT_EQ(bb4->Pred_id(0), bb3->Id());
  // bb3 has 2 pred, which is bb2 and bb3
  EXPECT_EQ(bb3->Pred_cnt(), 2);
  EXPECT_EQ(bb3->Pred_id(0), bb2->Id());
  EXPECT_EQ(bb3->Pred_id(1), bb3->Id());
  // bb2 has 1 pred, which is entry
  EXPECT_EQ(bb2->Pred_cnt(), 1);
  EXPECT_EQ(bb2->Pred_id(0), entry->Id());

  // verify BB layout
  // entry(bb0) - bb2 - bb3 - bb4 - exit(bb1)
  EXPECT_EQ(entry->Next_id(), bb2->Id());
  EXPECT_EQ(bb2->Next_id(), bb3->Id());
  EXPECT_EQ(bb3->Next_id(), bb4->Id());
  EXPECT_EQ(bb4->Next_id(), exit->Id());
  EXPECT_EQ(exit->Next_id(), NULL_BB_ID);
  EXPECT_EQ(exit->Prev_id(), bb4->Id());
  EXPECT_EQ(bb4->Prev_id(), bb3->Id());
  EXPECT_EQ(bb3->Prev_id(), bb2->Id());
  EXPECT_EQ(bb2->Prev_id(), entry->Id());
  EXPECT_EQ(entry->Prev_id(), NULL_BB_ID);
}

void TEST_AIR2CGIR::Test_air2cgir_nested_do_while() {
  TEST_AIR2CGIR_UTIL util;
  STMT_PTR           do_stmt   = util.Create_do_loop({1, 2, 3, 4});
  STMT_PTR           nested_do = util.Create_do_loop({5, 6, 7, 8});
  STMT_PTR           ret_stmt  = util.Create_retv(9);
  STMT_LIST          body_sl   = STMT_LIST(do_stmt->Node()->Body_blk());
  body_sl.Append(nested_do);
  STMT_LIST sl = util.Stmt_list();
  sl.Append(do_stmt);
  sl.Append(ret_stmt);

  CGIR_CONTAINER cg_cont(util.Func_scope(), true);
  EXPECT_EQ(cg_cont.Bb_count(), 2);
  BB_PTR entry = cg_cont.Entry();  // bb0
  BB_PTR exit  = cg_cont.Exit();   // bb1

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

  // should create 5 new BB and 8 edges for do_while
  EXPECT_EQ(cg_cont.Bb_count(), 7);
  EXPECT_EQ(cg_cont.Edge_count(), 8);
  // entry/exit should be the same
  EXPECT_EQ(entry, cg_cont.Entry());
  EXPECT_EQ(exit, cg_cont.Exit());

  // verify bb successors and instructions
  // entry should has 1 succ
  EXPECT_EQ(entry->Succ_cnt(), 1);
  // bb2 contains "1" and has 1 succ
  BB_PTR bb2 = cg_cont.Bb(entry->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(1), bb2->Id().Value());
  EXPECT_EQ(bb2->Succ_cnt(), 1);
  // bb3 contains "3" "5" and has 1 succ
  BB_PTR bb3 = cg_cont.Bb(bb2->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(3), bb3->Id().Value());
  EXPECT_EQ(ctx.Val_bb(5), bb3->Id().Value());
  EXPECT_EQ(bb3->Succ_cnt(), 1);
  // bb4 contains "7" "8" "6" and has 2 succ
  BB_PTR bb4 = cg_cont.Bb(bb3->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(7), bb4->Id().Value());
  EXPECT_EQ(ctx.Val_bb(8), bb4->Id().Value());
  EXPECT_EQ(ctx.Val_bb(6), bb4->Id().Value());
  EXPECT_EQ(bb4->Succ_cnt(), 2);
  // bb4 second succ is itself
  EXPECT_EQ(bb4->Succ_id(1), bb4->Id());
  // bb5 contains "4" "2" and has 2 succ
  BB_PTR bb5 = cg_cont.Bb(bb4->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(4), bb5->Id().Value());
  EXPECT_EQ(ctx.Val_bb(2), bb5->Id().Value());
  EXPECT_EQ(bb5->Succ_cnt(), 2);
  // bb5 second succ is bb3
  EXPECT_EQ(bb5->Succ_id(1), bb3->Id());
  // bb6 contains "9" and has 1 succ exit
  BB_PTR bb6 = cg_cont.Bb(bb5->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(9), bb6->Id().Value());
  EXPECT_EQ(bb6->Succ_cnt(), 1);
  EXPECT_EQ(bb6->Succ_id(0), exit->Id());

  // verify predecessors
  // exit bb has 1 pred, which is bb6
  EXPECT_EQ(exit->Pred_cnt(), 1);
  EXPECT_EQ(exit->Pred_id(0), bb6->Id());
  // bb6 has 1 pred, which is bb5
  EXPECT_EQ(bb6->Pred_cnt(), 1);
  EXPECT_EQ(bb6->Pred_id(0), bb5->Id());
  // bb5 has 1 pred, which is bb4
  EXPECT_EQ(bb5->Pred_cnt(), 1);
  EXPECT_EQ(bb5->Pred_id(0), bb4->Id());
  // bb4 has 2 pred, bb3 & bb4
  EXPECT_EQ(bb4->Pred_cnt(), 2);
  EXPECT_EQ(bb4->Pred_id(0), bb3->Id());
  EXPECT_EQ(bb4->Pred_id(1), bb4->Id());
  // bb3 has 2 pred, bb2 & bb5
  EXPECT_EQ(bb3->Pred_cnt(), 2);
  EXPECT_EQ(bb3->Pred_id(0), bb2->Id());
  EXPECT_EQ(bb3->Pred_id(1), bb5->Id());
  // bb2 has 1 pred, which is entry
  EXPECT_EQ(bb2->Pred_cnt(), 1);
  EXPECT_EQ(bb2->Pred_id(0), entry->Id());

  // verify BB layout
  // entry(bb0) - bb2 - bb3 - bb4 - bb5 - bb6 - exit(bb1)
  EXPECT_EQ(entry->Next_id(), bb2->Id());
  EXPECT_EQ(bb2->Next_id(), bb3->Id());
  EXPECT_EQ(bb3->Next_id(), bb4->Id());
  EXPECT_EQ(bb4->Next_id(), bb5->Id());
  EXPECT_EQ(bb5->Next_id(), bb6->Id());
  EXPECT_EQ(bb6->Next_id(), exit->Id());
  EXPECT_EQ(exit->Next_id(), NULL_BB_ID);
  EXPECT_EQ(exit->Prev_id(), bb6->Id());
  EXPECT_EQ(bb6->Prev_id(), bb5->Id());
  EXPECT_EQ(bb5->Prev_id(), bb4->Id());
  EXPECT_EQ(bb4->Prev_id(), bb3->Id());
  EXPECT_EQ(bb3->Prev_id(), bb2->Id());
  EXPECT_EQ(bb2->Prev_id(), entry->Id());
  EXPECT_EQ(entry->Prev_id(), NULL_BB_ID);
}

void TEST_AIR2CGIR::Test_air2cgir_do_while_if() {
  TEST_AIR2CGIR_UTIL util;
  STMT_PTR           do_stmt  = util.Create_do_loop({1, 2, 3, 4});
  STMT_PTR           if_stmt  = util.Create_if({5, 6, 7});
  STMT_PTR           ret_stmt = util.Create_retv(8);
  STMT_LIST          body_sl  = STMT_LIST(do_stmt->Node()->Body_blk());
  body_sl.Append(if_stmt);
  STMT_LIST sl = util.Stmt_list();
  sl.Append(do_stmt);
  sl.Append(ret_stmt);

  CGIR_CONTAINER cg_cont(util.Func_scope(), true);
  EXPECT_EQ(cg_cont.Bb_count(), 2);
  BB_PTR entry = cg_cont.Entry();  // bb0
  BB_PTR exit  = cg_cont.Exit();   // bb1

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

  // should create 6 new BB and 9 edges for do_while
  EXPECT_EQ(cg_cont.Bb_count(), 8);
  EXPECT_EQ(cg_cont.Edge_count(), 9);
  // entry/exit should be the same
  EXPECT_EQ(entry, cg_cont.Entry());
  EXPECT_EQ(exit, cg_cont.Exit());

  // verify bb successors and instructions
  // entry should has 1 succ
  EXPECT_EQ(entry->Succ_cnt(), 1);
  // bb2 contains "1" and has 1 succ
  BB_PTR bb2 = cg_cont.Bb(entry->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(1), bb2->Id().Value());
  EXPECT_EQ(bb2->Succ_cnt(), 1);
  // bb3 contains "3" "5" and has 2 succ
  BB_PTR bb3 = cg_cont.Bb(bb2->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(3), bb3->Id().Value());
  EXPECT_EQ(ctx.Val_bb(5), bb3->Id().Value());
  EXPECT_EQ(bb3->Succ_cnt(), 2);
  // bb4 contains "6" and has 1 succ
  BB_PTR bb4 = cg_cont.Bb(bb3->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(6), bb4->Id().Value());
  EXPECT_EQ(bb4->Succ_cnt(), 1);
  // bb5 contains "7" and has 1 succ
  BB_PTR bb5 = cg_cont.Bb(bb3->Succ_id(1));
  EXPECT_EQ(ctx.Val_bb(7), bb5->Id().Value());
  EXPECT_EQ(bb5->Succ_cnt(), 1);
  // bb4 and bb5 has same succ
  EXPECT_EQ(bb4->Succ_id(0), bb5->Succ_id(0));
  // bb6 contains "4" "2" and has 2 succs
  BB_PTR bb6 = cg_cont.Bb(bb4->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(4), bb6->Id().Value());
  EXPECT_EQ(ctx.Val_bb(2), bb6->Id().Value());
  EXPECT_EQ(bb6->Succ_cnt(), 2);
  // bb6 second succ is bb3
  EXPECT_EQ(bb6->Succ_id(1), bb3->Id());
  // bb7 contains "8" and has 1 succs: exit
  BB_PTR bb7 = cg_cont.Bb(bb6->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(8), bb7->Id().Value());
  EXPECT_EQ(bb7->Succ_cnt(), 1);
  EXPECT_EQ(bb7->Succ_id(0), exit->Id());

  // verify predecessors
  // exit bb has 1 pred, which is bb7
  EXPECT_EQ(exit->Pred_cnt(), 1);
  EXPECT_EQ(exit->Pred_id(0), bb7->Id());
  // bb7 has 1 pred, which is bb6
  EXPECT_EQ(bb7->Pred_cnt(), 1);
  EXPECT_EQ(bb7->Pred_id(0), bb6->Id());
  // bb6 has 2 pred, bb4 & bb5
  EXPECT_EQ(bb6->Pred_cnt(), 2);
  EXPECT_EQ(bb6->Pred_id(0), bb4->Id());
  EXPECT_EQ(bb6->Pred_id(1), bb5->Id());
  // bb5 has 1 pred, which is bb3
  EXPECT_EQ(bb5->Pred_cnt(), 1);
  EXPECT_EQ(bb5->Pred_id(0), bb3->Id());
  // bb4 has 1 pred, which is bb3
  EXPECT_EQ(bb4->Pred_cnt(), 1);
  EXPECT_EQ(bb4->Pred_id(0), bb3->Id());
  // bb3 has 2 pred, bb2 & bb6
  EXPECT_EQ(bb3->Pred_cnt(), 2);
  EXPECT_EQ(bb3->Pred_id(0), bb2->Id());
  EXPECT_EQ(bb3->Pred_id(1), bb6->Id());
  // bb2 has 1 pred, which is entry
  EXPECT_EQ(bb2->Pred_cnt(), 1);
  EXPECT_EQ(bb2->Pred_id(0), entry->Id());

  // verify BB layout
  // entry(bb0) - bb2 - bb3 - bb4 - bb5 - bb7 - exit(bb1)
  EXPECT_EQ(entry->Next_id(), bb2->Id());
  EXPECT_EQ(bb2->Next_id(), bb3->Id());
  EXPECT_EQ(bb3->Next_id(), bb4->Id());
  EXPECT_EQ(bb4->Next_id(), bb5->Id());
  EXPECT_EQ(bb5->Next_id(), bb6->Id());
  EXPECT_EQ(bb6->Next_id(), bb7->Id());
  EXPECT_EQ(bb7->Next_id(), exit->Id());
  EXPECT_EQ(exit->Next_id(), NULL_BB_ID);
  EXPECT_EQ(exit->Prev_id(), bb7->Id());
  EXPECT_EQ(bb7->Prev_id(), bb6->Id());
  EXPECT_EQ(bb6->Prev_id(), bb5->Id());
  EXPECT_EQ(bb5->Prev_id(), bb4->Id());
  EXPECT_EQ(bb4->Prev_id(), bb3->Id());
  EXPECT_EQ(bb3->Prev_id(), bb2->Id());
  EXPECT_EQ(bb2->Prev_id(), entry->Id());
  EXPECT_EQ(entry->Prev_id(), NULL_BB_ID);
}

void TEST_AIR2CGIR::Test_air2cgir_while_do() {
  TEST_AIR2CGIR_UTIL util;
  STMT_PTR           if_stmt  = util.Create_do_loop({1, 2, 3, 4});
  STMT_PTR           ret_stmt = util.Create_retv(5);
  STMT_LIST          sl       = util.Stmt_list();
  sl.Append(if_stmt);
  sl.Append(ret_stmt);

  CGIR_CONTAINER cg_cont(util.Func_scope(), true);
  EXPECT_EQ(cg_cont.Bb_count(), 2);
  BB_PTR entry = cg_cont.Entry();  // bb0
  BB_PTR exit  = cg_cont.Exit();   // bb1

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

  // should create 4 new BB and 6 edges for while_do
  EXPECT_EQ(cg_cont.Bb_count(), 6);
  EXPECT_EQ(cg_cont.Edge_count(), 6);
  // entry/exit should be the same
  EXPECT_EQ(entry, cg_cont.Entry());
  EXPECT_EQ(exit, cg_cont.Exit());

  // verify bb successors and instructions
  // entry should has 1 succ
  EXPECT_EQ(entry->Succ_cnt(), 1);
  // bb2 contains "1" and has 1 succ (pre-head)
  BB_PTR bb2 = cg_cont.Bb(entry->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(1), bb2->Id().Value());
  EXPECT_EQ(bb2->Succ_cnt(), 1);
  // bb3 contains "2" and has 2 succ (head)
  BB_PTR bb3 = cg_cont.Bb(bb2->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(2), bb3->Id().Value());
  EXPECT_EQ(bb3->Succ_cnt(), 2);
  // bb3 first succ contains "3", "4", body & incr, and has 1 succ
  BB_PTR bb4 = cg_cont.Bb(bb3->Succ_id(0));
  EXPECT_EQ(bb4->Succ_cnt(), 1);
  EXPECT_EQ(ctx.Val_bb(3), bb4->Id().Value());
  EXPECT_EQ(ctx.Val_bb(4), bb4->Id().Value());
  // bb3 second succ contains "5", tail bb, and has 1 succ
  BB_PTR bb5 = cg_cont.Bb(bb3->Succ_id(1));
  EXPECT_EQ(bb5->Succ_cnt(), 1);
  EXPECT_EQ(ctx.Val_bb(5), bb5->Id().Value());
  // bb2 and bb4 has same succ
  EXPECT_EQ(bb2->Succ_id(0), bb4->Succ_id(0));
  // bb5 contains "5" and has 1 succ, exit
  EXPECT_EQ(bb5->Succ_cnt(), 1);
  EXPECT_EQ(ctx.Val_bb(5), bb5->Id().Value());
  EXPECT_EQ(bb5->Succ_id(0), exit->Id());

  // verify predecessors
  // exit bb has 1 pred, which is bb5
  EXPECT_EQ(exit->Pred_cnt(), 1);
  EXPECT_EQ(exit->Pred_id(0), bb5->Id());
  // bb5 has 1 pred, which is bb3
  EXPECT_EQ(bb5->Pred_cnt(), 1);
  EXPECT_EQ(bb5->Pred_id(0), bb3->Id());
  // bb4 has 1 pred, which is bb3
  EXPECT_EQ(bb4->Pred_cnt(), 1);
  EXPECT_EQ(bb4->Pred_id(0), bb3->Id());
  // bb3 has 2 pred, which is bb2 and bb4
  EXPECT_EQ(bb3->Pred_cnt(), 2);
  EXPECT_EQ(bb3->Pred_id(0), bb2->Id());
  EXPECT_EQ(bb3->Pred_id(1), bb4->Id());
  // bb2 has 1 pred, which is entry
  EXPECT_EQ(bb2->Pred_cnt(), 1);
  EXPECT_EQ(bb2->Pred_id(0), entry->Id());

  // verify BB layout
  // entry(bb0) - bb2 - bb3 - bb4 - bb5 - exit(bb1)
  EXPECT_EQ(entry->Next_id(), bb2->Id());
  EXPECT_EQ(bb2->Next_id(), bb3->Id());
  EXPECT_EQ(bb3->Next_id(), bb4->Id());
  EXPECT_EQ(bb4->Next_id(), bb5->Id());
  EXPECT_EQ(bb5->Next_id(), exit->Id());
  EXPECT_EQ(exit->Next_id(), NULL_BB_ID);
  EXPECT_EQ(exit->Prev_id(), bb5->Id());
  EXPECT_EQ(bb5->Prev_id(), bb4->Id());
  EXPECT_EQ(bb4->Prev_id(), bb3->Id());
  EXPECT_EQ(bb3->Prev_id(), bb2->Id());
  EXPECT_EQ(bb2->Prev_id(), entry->Id());
  EXPECT_EQ(entry->Prev_id(), NULL_BB_ID);
}

void TEST_AIR2CGIR::Test_air2cgir_nested_while_do() {
  TEST_AIR2CGIR_UTIL util;
  STMT_PTR           do_stmt   = util.Create_do_loop({1, 2, 3, 4});
  STMT_PTR           nested_do = util.Create_do_loop({5, 6, 7, 8});
  STMT_PTR           ret_stmt  = util.Create_retv(9);
  STMT_LIST          body_sl   = STMT_LIST(do_stmt->Node()->Body_blk());
  body_sl.Append(nested_do);
  STMT_LIST sl = util.Stmt_list();
  sl.Append(do_stmt);
  sl.Append(ret_stmt);

  CGIR_CONTAINER cg_cont(util.Func_scope(), true);
  EXPECT_EQ(cg_cont.Bb_count(), 2);
  BB_PTR entry = cg_cont.Entry();  // bb0
  BB_PTR exit  = cg_cont.Exit();   // bb1

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

  // should create 7 new BB and 10 edges for while_do
  EXPECT_EQ(cg_cont.Bb_count(), 9);
  EXPECT_EQ(cg_cont.Edge_count(), 10);
  // entry/exit should be the same
  EXPECT_EQ(entry, cg_cont.Entry());
  EXPECT_EQ(exit, cg_cont.Exit());

  // verify bb successors and instructions
  // entry should has 1 succ
  EXPECT_EQ(entry->Succ_cnt(), 1);
  // bb2 contains "1" and has 1 succ (pre-head)
  BB_PTR bb2 = cg_cont.Bb(entry->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(1), bb2->Id().Value());
  EXPECT_EQ(bb2->Succ_cnt(), 1);
  // bb3 contains "2" and has 2 succ (head)
  BB_PTR bb3 = cg_cont.Bb(bb2->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(2), bb3->Id().Value());
  EXPECT_EQ(bb3->Succ_cnt(), 2);
  // bb4 first succ of bb3 contains "3" "5", body&incr, has 1 succ
  BB_PTR bb4 = cg_cont.Bb(bb3->Succ_id(0));
  EXPECT_EQ(bb4->Succ_cnt(), 1);
  EXPECT_EQ(ctx.Val_bb(3), bb4->Id().Value());
  EXPECT_EQ(ctx.Val_bb(5), bb4->Id().Value());
  // bb5 contains "6" and has 2 succ
  BB_PTR bb5 = cg_cont.Bb(bb4->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(6), bb5->Id().Value());
  EXPECT_EQ(bb5->Succ_cnt(), 2);
  // bb6 first succ of bb5 contains "7" "8" and has 1 succ bb5
  BB_PTR bb6 = cg_cont.Bb(bb5->Succ_id(0));
  EXPECT_EQ(bb6->Succ_cnt(), 1);
  EXPECT_EQ(ctx.Val_bb(7), bb6->Id().Value());
  EXPECT_EQ(ctx.Val_bb(8), bb6->Id().Value());
  EXPECT_EQ(bb6->Succ_id(0), bb5->Id());
  // bb7 second succ of bb5 contains "4" and has 1 succ bb3
  BB_PTR bb7 = cg_cont.Bb(bb5->Succ_id(1));
  EXPECT_EQ(bb7->Succ_cnt(), 1);
  EXPECT_EQ(ctx.Val_bb(4), bb7->Id().Value());
  EXPECT_EQ(bb7->Succ_id(0), bb3->Id());
  // bb8 second succ of bb3 contains "9" and has 1 succ exit
  BB_PTR bb8 = cg_cont.Bb(bb3->Succ_id(1));
  EXPECT_EQ(bb8->Succ_cnt(), 1);
  EXPECT_EQ(ctx.Val_bb(9), bb8->Id().Value());
  EXPECT_EQ(bb8->Succ_id(0), exit->Id());

  // verify predecessors
  // exit bb has 1 pred, which is bb8
  EXPECT_EQ(exit->Pred_cnt(), 1);
  EXPECT_EQ(exit->Pred_id(0), bb8->Id());
  // bb8 has 1 pred, which is bb3
  EXPECT_EQ(bb8->Pred_cnt(), 1);
  EXPECT_EQ(bb8->Pred_id(0), bb3->Id());
  // bb7 has 1 pred, which is bb5
  EXPECT_EQ(bb7->Pred_cnt(), 1);
  EXPECT_EQ(bb7->Pred_id(0), bb5->Id());
  // bb6 has 1 pred, which is bb5
  EXPECT_EQ(bb6->Pred_cnt(), 1);
  EXPECT_EQ(bb6->Pred_id(0), bb5->Id());
  // bb5 has 2 pred, bb4 & bb6
  EXPECT_EQ(bb5->Pred_cnt(), 2);
  EXPECT_EQ(bb5->Pred_id(0), bb4->Id());
  EXPECT_EQ(bb5->Pred_id(1), bb6->Id());
  // bb4 has 1 pred, which is bb3
  EXPECT_EQ(bb4->Pred_cnt(), 1);
  EXPECT_EQ(bb4->Pred_id(0), bb3->Id());
  // bb3 has 2 pred, bb2 & bb7
  EXPECT_EQ(bb3->Pred_cnt(), 2);
  EXPECT_EQ(bb3->Pred_id(0), bb2->Id());
  EXPECT_EQ(bb3->Pred_id(1), bb7->Id());
  // bb2 has 1 pred, which is entry
  EXPECT_EQ(bb2->Pred_cnt(), 1);
  EXPECT_EQ(bb2->Pred_id(0), entry->Id());

  // verify BB layout
  // entry(bb0) - bb2 - bb3 - bb4 - bb5 - bb6 - bb7 - bb8 - exit(bb1)
  EXPECT_EQ(entry->Next_id(), bb2->Id());
  EXPECT_EQ(bb2->Next_id(), bb3->Id());
  EXPECT_EQ(bb3->Next_id(), bb4->Id());
  EXPECT_EQ(bb4->Next_id(), bb5->Id());
  EXPECT_EQ(bb5->Next_id(), bb6->Id());
  EXPECT_EQ(bb6->Next_id(), bb7->Id());
  EXPECT_EQ(bb7->Next_id(), bb8->Id());
  EXPECT_EQ(bb8->Next_id(), exit->Id());
  EXPECT_EQ(exit->Next_id(), NULL_BB_ID);
  EXPECT_EQ(exit->Prev_id(), bb8->Id());
  EXPECT_EQ(bb8->Prev_id(), bb7->Id());
  EXPECT_EQ(bb7->Prev_id(), bb6->Id());
  EXPECT_EQ(bb6->Prev_id(), bb5->Id());
  EXPECT_EQ(bb5->Prev_id(), bb4->Id());
  EXPECT_EQ(bb4->Prev_id(), bb3->Id());
  EXPECT_EQ(bb3->Prev_id(), bb2->Id());
  EXPECT_EQ(bb2->Prev_id(), entry->Id());
  EXPECT_EQ(entry->Prev_id(), NULL_BB_ID);
}

void TEST_AIR2CGIR::Test_air2cgir_while_do_if() {
  TEST_AIR2CGIR_UTIL util;
  STMT_PTR           do_stmt  = util.Create_do_loop({1, 2, 3, 4});
  STMT_PTR           if_stmt  = util.Create_if({5, 6, 7});
  STMT_PTR           ret_stmt = util.Create_retv(8);
  STMT_LIST          body_sl  = STMT_LIST(do_stmt->Node()->Body_blk());
  body_sl.Append(if_stmt);
  STMT_LIST sl = util.Stmt_list();
  sl.Append(do_stmt);
  sl.Append(ret_stmt);

  CGIR_CONTAINER cg_cont(util.Func_scope(), true);
  EXPECT_EQ(cg_cont.Bb_count(), 2);
  BB_PTR entry = cg_cont.Entry();  // bb0
  BB_PTR exit  = cg_cont.Exit();   // bb1

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

  // should create 7 new BB and 10 edges for while_do
  EXPECT_EQ(cg_cont.Bb_count(), 9);
  EXPECT_EQ(cg_cont.Edge_count(), 10);
  // entry/exit should be the same
  EXPECT_EQ(entry, cg_cont.Entry());
  EXPECT_EQ(exit, cg_cont.Exit());

  // verify bb successors and instructions
  // entry should has 1 succ
  EXPECT_EQ(entry->Succ_cnt(), 1);
  // bb2 contains "1" and has 1 succ (pre-head)
  BB_PTR bb2 = cg_cont.Bb(entry->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(1), bb2->Id().Value());
  EXPECT_EQ(bb2->Succ_cnt(), 1);
  // bb3 contains "2" and has 2 succ
  BB_PTR bb3 = cg_cont.Bb(bb2->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(2), bb3->Id().Value());
  EXPECT_EQ(bb3->Succ_cnt(), 2);
  // bb4 first succ of bb3, contains "3" "5" and has 2 succ
  BB_PTR bb4 = cg_cont.Bb(bb3->Succ_id(0));
  EXPECT_EQ(bb4->Succ_cnt(), 2);
  EXPECT_EQ(ctx.Val_bb(3), bb4->Id().Value());
  EXPECT_EQ(ctx.Val_bb(5), bb4->Id().Value());
  // bb5 first succ of bb4, contains "6" and has 1 succ
  BB_PTR bb5 = cg_cont.Bb(bb4->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(6), bb5->Id().Value());
  EXPECT_EQ(bb5->Succ_cnt(), 1);
  // bb6 second succ of bb4, contains "7" and has 1 succ
  BB_PTR bb6 = cg_cont.Bb(bb4->Succ_id(1));
  EXPECT_EQ(ctx.Val_bb(7), bb6->Id().Value());
  EXPECT_EQ(bb6->Succ_cnt(), 1);
  // bb5 and bb6 has same succ
  EXPECT_EQ(bb5->Succ_id(0), bb6->Succ_id(0));
  // bb7 contains "4" and has 1 succ bb3
  BB_PTR bb7 = cg_cont.Bb(bb5->Succ_id(0));
  EXPECT_EQ(ctx.Val_bb(4), bb7->Id().Value());
  EXPECT_EQ(bb7->Succ_cnt(), 1);
  EXPECT_EQ(bb7->Succ_id(0), bb3->Id());
  // bb8 second succ of bb3 contains "8" and has 1 succ exit
  BB_PTR bb8 = cg_cont.Bb(bb3->Succ_id(1));
  EXPECT_EQ(ctx.Val_bb(8), bb8->Id().Value());
  EXPECT_EQ(bb8->Succ_cnt(), 1);
  EXPECT_EQ(bb8->Succ_id(0), exit->Id());

  // verify predecessors
  // exit bb has 1 pred, which is bb8
  EXPECT_EQ(exit->Pred_cnt(), 1);
  EXPECT_EQ(exit->Pred_id(0), bb8->Id());
  // bb8 has 1 pred, which is bb3
  EXPECT_EQ(bb8->Pred_cnt(), 1);
  EXPECT_EQ(bb8->Pred_id(0), bb3->Id());
  // bb7 has 2 pred, bb5 & bb6
  EXPECT_EQ(bb7->Pred_cnt(), 2);
  EXPECT_EQ(bb7->Pred_id(0), bb5->Id());
  EXPECT_EQ(bb7->Pred_id(1), bb6->Id());
  // bb6 has 1 pred, which is bb4
  EXPECT_EQ(bb6->Pred_cnt(), 1);
  EXPECT_EQ(bb6->Pred_id(0), bb4->Id());
  // bb5 has 1 pred, which is bb4
  EXPECT_EQ(bb5->Pred_cnt(), 1);
  EXPECT_EQ(bb5->Pred_id(0), bb4->Id());
  // bb4 has 1 pred, which is bb3
  EXPECT_EQ(bb4->Pred_cnt(), 1);
  EXPECT_EQ(bb4->Pred_id(0), bb3->Id());
  // bb3 has 2 pred, bb2 & b7b7
  EXPECT_EQ(bb3->Pred_cnt(), 2);
  EXPECT_EQ(bb3->Pred_id(0), bb2->Id());
  EXPECT_EQ(bb3->Pred_id(1), bb7->Id());
  // bb2 has 1 pred, entry
  EXPECT_EQ(bb2->Pred_cnt(), 1);
  EXPECT_EQ(bb2->Pred_id(0), entry->Id());

  // verify BB layout
  // entry(bb0) - bb2 - bb3 - bb4 - bb5 - bb6 - bb7 - bb8 - exit(bb1)
  EXPECT_EQ(entry->Next_id(), bb2->Id());
  EXPECT_EQ(bb2->Next_id(), bb3->Id());
  EXPECT_EQ(bb3->Next_id(), bb4->Id());
  EXPECT_EQ(bb4->Next_id(), bb5->Id());
  EXPECT_EQ(bb5->Next_id(), bb6->Id());
  EXPECT_EQ(bb6->Next_id(), bb7->Id());
  EXPECT_EQ(bb7->Next_id(), bb8->Id());
  EXPECT_EQ(bb8->Next_id(), exit->Id());
  EXPECT_EQ(exit->Next_id(), NULL_BB_ID);
  EXPECT_EQ(exit->Prev_id(), bb8->Id());
  EXPECT_EQ(bb8->Prev_id(), bb7->Id());
  EXPECT_EQ(bb7->Prev_id(), bb6->Id());
  EXPECT_EQ(bb6->Prev_id(), bb5->Id());
  EXPECT_EQ(bb5->Prev_id(), bb4->Id());
  EXPECT_EQ(bb4->Prev_id(), bb3->Id());
  EXPECT_EQ(bb3->Prev_id(), bb2->Id());
  EXPECT_EQ(bb2->Prev_id(), entry->Id());
  EXPECT_EQ(entry->Prev_id(), NULL_BB_ID);
}

TEST_F(TEST_AIR2CGIR, air2cgir_if) { Test_air2cgir_if(); }

TEST_F(TEST_AIR2CGIR, air2cgir_nested_if) { Test_air2cgir_nested_if(); }

TEST_F(TEST_AIR2CGIR, air2cgir_if_do_while) { Test_air2cgir_if_do_while(); }

TEST_F(TEST_AIR2CGIR, air2cgir_if_while_do) { Test_air2cgir_if_while_do(); }

TEST_F(TEST_AIR2CGIR, air2cgir_do_while) { Test_air2cgir_do_while(); }

TEST_F(TEST_AIR2CGIR, air2cgir_nested_do_while) {
  Test_air2cgir_nested_do_while();
}

TEST_F(TEST_AIR2CGIR, air2cgir_do_while_if) { Test_air2cgir_do_while_if(); }

TEST_F(TEST_AIR2CGIR, air2cgir_while_do) { Test_air2cgir_while_do(); }

TEST_F(TEST_AIR2CGIR, air2cgir_nested_while_do) {
  Test_air2cgir_nested_while_do();
}

TEST_F(TEST_AIR2CGIR, air2cgir_while_do_if) { Test_air2cgir_while_do_if(); }

}  // namespace
