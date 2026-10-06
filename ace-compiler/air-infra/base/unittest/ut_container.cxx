//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/base/container.h"
#include "air/base/container_decl.h"
#include "air/base/meta_info.h"
#include "air/base/opcode.h"
#include "air/base/spos.h"
#include "air/base/st.h"
#include "air/base/st_decl.h"
#include "air/base/st_enum.h"
#include "air/core/opcode.h"
#include "gtest/gtest.h"

using namespace air::base;
using namespace air::core;
using namespace air::util;
using namespace testing;

class TEST_CONTAINER : public ::testing::Test {
protected:
  void SetUp() override {
    META_INFO::Remove_all();
    air::core::Register_core();
    _glob = new GLOB_SCOPE(0, true);
    SPOS spos(0, 1, 1, 0);

    /*  Define function

      int My_add(int x, int y) {
        int z = x + y;
        return z;
      }

    */
    STR_PTR  str_add  = _glob->New_str("My_add");
    FUNC_PTR func_add = _glob->New_func(str_add, spos);
    func_add->Set_parent(_glob->Comp_env_id());
    SIGNATURE_TYPE_PTR sig_add  = _glob->New_sig_type();
    TYPE_PTR           int_type = _glob->Prim_type(PRIMITIVE_TYPE::INT_S32);
    _glob->New_ret_param(int_type, sig_add);
    _glob->New_param("x", int_type, sig_add, spos);
    _glob->New_param("y", int_type, sig_add, spos);
    sig_add->Set_complete();
    ENTRY_PTR entry_add =
        _glob->New_entry_point(sig_add, func_add, str_add, spos);
    _fs_add             = &_glob->New_func_scope(func_add);
    CONTAINER* cntr_add = &_fs_add->Container();
    cntr_add->New_func_entry(spos);
    ADDR_DATUM_PTR formal_x = _fs_add->Formal(0);
    ADDR_DATUM_PTR formal_y = _fs_add->Formal(1);
    ADDR_DATUM_PTR var_z    = _fs_add->New_var(int_type, "z", spos);
    // x + y
    NODE_PTR node_x   = cntr_add->New_ld(formal_x, spos);
    NODE_PTR node_y   = cntr_add->New_ld(formal_y, spos);
    NODE_PTR node_add = cntr_add->New_bin_arith(air::core::OPC_ADD, int_type,
                                                node_x, node_y, spos);
    // z = x + y;
    STMT_PTR stmt_st = cntr_add->New_st(node_add, var_z, spos);
    cntr_add->Stmt_list().Append(stmt_st);
    // return z;
    NODE_PTR node_z       = cntr_add->New_ld(var_z, spos);
    STMT_PTR stmt_ret_add = cntr_add->New_retv(node_z, spos);
    cntr_add->Stmt_list().Append(stmt_ret_add);

    /*  Define function

        int main(int argc, char** argv) {
          int a = 5;
          int b = 6;
          int c = My_add(a, b);
          return c;
        }
    */
    STR_PTR  str_main  = _glob->New_str("main");
    FUNC_PTR func_main = _glob->New_func(str_main, spos);
    func_main->Set_parent(_glob->Comp_env_id());
    SIGNATURE_TYPE_PTR sig_main = _glob->New_sig_type();
    _glob->New_ret_param(int_type, sig_main);
    _glob->New_param("argc", int_type, sig_main, spos);
    TYPE_PTR argv_ptr_to = _glob->Prim_type(PRIMITIVE_TYPE::INT_S8);
    TYPE_PTR argv_type =
        _glob->New_ptr_type(argv_ptr_to->Id(), POINTER_KIND::FLAT64);
    _glob->New_param("argv", argv_type, sig_main, spos);
    sig_main->Set_complete();
    ENTRY_PTR entry_main =
        _glob->New_global_entry_point(sig_main, func_main, str_main, spos);
    _fs_main             = &_glob->New_func_scope(func_main);
    CONTAINER* cntr_main = &_fs_main->Container();
    cntr_main->New_func_entry(spos);
    // int a = 5;
    ADDR_DATUM_PTR var_a  = _fs_main->New_var(int_type, "a", spos);
    NODE_PTR       cst_5  = cntr_main->New_intconst(int_type, 5, spos);
    STMT_PTR       stmt_a = cntr_main->New_st(cst_5, var_a, spos);
    cntr_main->Stmt_list().Append(stmt_a);
    // int b = 6;
    ADDR_DATUM_PTR var_b  = _fs_main->New_var(int_type, "b", spos);
    NODE_PTR       cst_6  = cntr_main->New_intconst(int_type, 6, spos);
    STMT_PTR       stmt_b = cntr_main->New_st(cst_6, var_b, spos);
    cntr_main->Stmt_list().Append(stmt_b);
    // int c = My_add(a, b);
    ADDR_DATUM_PTR var_c = _fs_main->New_var(int_type, "c", spos);
    PREG_PTR       retv  = _fs_main->New_preg(int_type);
    _call_stmt           = cntr_main->New_call(entry_add, retv, 2, spos);
    NODE_PTR param_a     = cntr_main->New_ld(var_a, spos);
    NODE_PTR param_b     = cntr_main->New_ld(var_b, spos);
    cntr_main->New_arg(_call_stmt, 0, param_a);
    cntr_main->New_arg(_call_stmt, 1, param_b);
    cntr_main->Stmt_list().Append(_call_stmt);
    NODE_PTR node_ret = cntr_main->New_ldp(retv, spos);
    STMT_PTR stmt_c   = cntr_main->New_st(node_ret, var_c, spos);
    cntr_main->Stmt_list().Append(stmt_c);
    // return c;
    NODE_PTR node_c        = cntr_main->New_ld(var_c, spos);
    STMT_PTR stmt_ret_main = cntr_main->New_retv(node_c, spos);
    cntr_main->Stmt_list().Append(stmt_ret_main);
  }

  void TearDown() override { delete _glob; }
  void Run_test_ctor_dtor();
  void Run_test_stmt_list_prepend();
  void Run_test_stmt_list_append();
  void Run_test_func_add_def();
  void Run_test_func_main_def();
  void Run_test_stmt_attr();
  void Run_test_clone();
  void Run_test_opcode();
  void Run_test_verify_func_add();
  void Run_test_verify_func_main();
  void Run_test_verify_fail();
  void Run_test_func_entry_node();
  void Run_test_func_no_ret();

  GLOB_SCOPE* _glob;
  FUNC_SCOPE* _fs_add;
  FUNC_SCOPE* _fs_main;
  STMT_PTR    _call_stmt;
};

void TEST_CONTAINER::Run_test_ctor_dtor() {
  CONTAINER* cntr = new CONTAINER(_fs_add, true);
  delete cntr;
  cntr = CONTAINER::New(_fs_add, true);
  cntr->Delete();
}

void TEST_CONTAINER::Run_test_stmt_list_prepend() {
  CONTAINER* cntr_add = &_fs_add->Container();
  SPOS       spos(0, 2, 1, 0);
  STMT_LIST  ori_slist = cntr_add->Stmt_list();
  STMT_LIST  new_slist(cntr_add->New_stmt_block(spos));

  // case 1: prepend empty list to none empty one
  STMT_ID     first   = ori_slist.Begin_stmt_id();
  std::string tgt_exp = ori_slist.To_str();
  std::string src_exp = new_slist.To_str();
  STMT_ID     stmt    = ori_slist.Prepend(new_slist)->Id();
  std::string tgt_res = ori_slist.To_str();
  std::string src_res = new_slist.To_str();
  EXPECT_TRUE(first == stmt);
  EXPECT_EQ(tgt_res, tgt_exp);
  EXPECT_EQ(src_res, src_exp);

  // case 2: prepend none empty list to empty one
  stmt = new_slist.Prepend(ori_slist)->Id();
  tgt_exp =
      "block ID(0xc) LINE(2:1:0)\n"
      "  st \"z\" VAR[0x10000002] ID(0x8) LINE(1:1:0)\n"
      "    add RTYPE[0x2](int32_t)\n"
      "      ld \"x\" FML[0x10000000] RTYPE[0x2](int32_t)\n"
      "      ld \"y\" FML[0x10000001] RTYPE[0x2](int32_t)\n"
      "  retv ID(0xa) LINE(1:1:0)\n"
      "    ld \"z\" VAR[0x10000002] RTYPE[0x2](int32_t)\n"
      "end_block ID(0xc)\n";
  src_exp =
      "block ID(0x4) LINE(1:1:0)\n"
      "end_block ID(0x4)\n";
  tgt_res = new_slist.Block_node()->To_str();
  src_res = ori_slist.Block_node()->To_str();
  EXPECT_TRUE(first == stmt);
  EXPECT_EQ(tgt_res, tgt_exp);
  EXPECT_EQ(src_res, src_exp);
  for (STMT_PTR st = new_slist.Begin_stmt(); st != new_slist.End_stmt();
       st          = st->Next()) {
    EXPECT_TRUE(st->Parent_node_id() == new_slist.Block_node()->Id());
    EXPECT_TRUE(st->Next()->Prev_id() == st->Id());
  }

  // case 3: prepend none empty list to none empty one
  SPOS      spos1(0, 3, 1, 0);
  NODE_PTR  new_blk1 = cntr_add->New_stmt_block(spos1);
  STMT_LIST stmt_list1(new_blk1);
  TYPE_PTR  int_type = _glob->Prim_type(PRIMITIVE_TYPE::INT_S32);
  // x = x + 1
  NODE_PTR node_x   = cntr_add->New_ld(_fs_add->Formal(0), spos1);
  NODE_PTR node_1   = cntr_add->New_intconst(int_type, 1, spos1);
  NODE_PTR node_add = cntr_add->New_bin_arith(air::core::OPC_ADD, int_type,
                                              node_x, node_1, spos1);
  STMT_PTR stmt_st  = cntr_add->New_st(node_add, node_x->Addr_datum(), spos1);
  stmt_list1.Append(stmt_st);
  // y = y - 1
  SPOS     spos2(0, 4, 1, 0);
  NODE_PTR node_y   = cntr_add->New_ld(_fs_add->Formal(1), spos2);
  node_1            = cntr_add->New_intconst(int_type, 1, spos2);
  NODE_PTR node_sub = cntr_add->New_bin_arith(air::core::OPC_SUB, int_type,
                                              node_y, node_1, spos2);
  stmt_st           = cntr_add->New_st(node_sub, node_y->Addr_datum(), spos2);
  stmt_list1.Append(stmt_st);
  src_exp =
      "block ID(0xe) LINE(3:1:0)\n"
      "  st \"x\" FML[0x10000000] ID(0x12) LINE(3:1:0)\n"
      "    add RTYPE[0x2](int32_t)\n"
      "      ld \"x\" FML[0x10000000] RTYPE[0x2](int32_t)\n"
      "      intconst #0x1 RTYPE[0x2](int32_t)\n"
      "  st \"y\" FML[0x10000001] ID(0x16) LINE(4:1:0)\n"
      "    sub RTYPE[0x2](int32_t)\n"
      "      ld \"y\" FML[0x10000001] RTYPE[0x2](int32_t)\n"
      "      intconst #0x1 RTYPE[0x2](int32_t)\n"
      "end_block ID(0xe)\n";
  src_res = new_blk1->To_str();
  EXPECT_EQ(src_res, src_exp);

  first = stmt_list1.Begin_stmt_id();
  stmt  = new_slist.Prepend(stmt_list1)->Id();
  tgt_exp =
      "block ID(0xc) LINE(2:1:0)\n"
      "  st \"x\" FML[0x10000000] ID(0x12) LINE(3:1:0)\n"
      "    add RTYPE[0x2](int32_t)\n"
      "      ld \"x\" FML[0x10000000] RTYPE[0x2](int32_t)\n"
      "      intconst #0x1 RTYPE[0x2](int32_t)\n"
      "  st \"y\" FML[0x10000001] ID(0x16) LINE(4:1:0)\n"
      "    sub RTYPE[0x2](int32_t)\n"
      "      ld \"y\" FML[0x10000001] RTYPE[0x2](int32_t)\n"
      "      intconst #0x1 RTYPE[0x2](int32_t)\n"
      "  st \"z\" VAR[0x10000002] ID(0x8) LINE(1:1:0)\n"
      "    add RTYPE[0x2](int32_t)\n"
      "      ld \"x\" FML[0x10000000] RTYPE[0x2](int32_t)\n"
      "      ld \"y\" FML[0x10000001] RTYPE[0x2](int32_t)\n"
      "  retv ID(0xa) LINE(1:1:0)\n"
      "    ld \"z\" VAR[0x10000002] RTYPE[0x2](int32_t)\n"
      "end_block ID(0xc)\n";
  src_exp =
      "block ID(0xe) LINE(3:1:0)\n"
      "end_block ID(0xe)\n";
  tgt_res = new_slist.Block_node()->To_str();
  src_res = stmt_list1.Block_node()->To_str();
  EXPECT_TRUE(first == stmt);
  EXPECT_EQ(tgt_res, tgt_exp);
  EXPECT_EQ(src_res, src_exp);
  for (STMT_PTR st = new_slist.Begin_stmt(); st != new_slist.End_stmt();
       st          = st->Next()) {
    EXPECT_TRUE(st->Parent_node_id() == new_slist.Block_node()->Id());
    EXPECT_TRUE(st->Next()->Prev_id() == st->Id());
  }
}

void TEST_CONTAINER::Run_test_stmt_list_append() {
  CONTAINER* cntr_add = &_fs_add->Container();
  SPOS       spos(0, 2, 1, 0);
  STMT_LIST  ori_slist = cntr_add->Stmt_list();
  STMT_LIST  new_slist(cntr_add->New_stmt_block(spos));

  // case 1: append empty list to none empty one
  STMT_ID     first   = ori_slist.Begin_stmt_id();
  std::string tgt_exp = ori_slist.To_str();
  std::string src_exp = new_slist.To_str();
  STMT_ID     stmt    = ori_slist.Append(new_slist)->Id();
  std::string tgt_res = ori_slist.To_str();
  std::string src_res = new_slist.To_str();
  EXPECT_TRUE(first == stmt);
  EXPECT_EQ(tgt_res, tgt_exp);
  EXPECT_EQ(src_res, src_exp);

  // case 2: append none empty list to empty one
  stmt = new_slist.Append(ori_slist)->Id();
  tgt_exp =
      "block ID(0xc) LINE(2:1:0)\n"
      "  st \"z\" VAR[0x10000002] ID(0x8) LINE(1:1:0)\n"
      "    add RTYPE[0x2](int32_t)\n"
      "      ld \"x\" FML[0x10000000] RTYPE[0x2](int32_t)\n"
      "      ld \"y\" FML[0x10000001] RTYPE[0x2](int32_t)\n"
      "  retv ID(0xa) LINE(1:1:0)\n"
      "    ld \"z\" VAR[0x10000002] RTYPE[0x2](int32_t)\n"
      "end_block ID(0xc)\n";
  src_exp =
      "block ID(0x4) LINE(1:1:0)\n"
      "end_block ID(0x4)\n";
  tgt_res = new_slist.Block_node()->To_str();
  src_res = ori_slist.Block_node()->To_str();
  EXPECT_TRUE(first == stmt);
  EXPECT_EQ(tgt_res, tgt_exp);
  EXPECT_EQ(src_res, src_exp);
  for (STMT_PTR st = new_slist.Begin_stmt(); st != new_slist.End_stmt();
       st          = st->Next()) {
    EXPECT_TRUE(st->Parent_node_id() == new_slist.Block_node()->Id());
    EXPECT_TRUE(st->Next()->Prev_id() == st->Id());
  }

  // case 3: append none empty list to none empty one
  SPOS      spos1(0, 3, 1, 0);
  NODE_PTR  new_blk1 = cntr_add->New_stmt_block(spos1);
  STMT_LIST stmt_list1(new_blk1);
  TYPE_PTR  int_type = _glob->Prim_type(PRIMITIVE_TYPE::INT_S32);
  // x = x + 1
  ADDR_DATUM_PTR ad_x   = _fs_add->Formal(0);
  NODE_PTR       node_x = cntr_add->New_ld(ad_x, spos1);
  NODE_PTR       node_1 = cntr_add->New_intconst(int_type, 1, spos1);
  NODE_PTR node_add     = cntr_add->New_bin_arith(air::core::OPC_ADD, int_type,
                                                  node_x, node_1, spos1);
  STMT_PTR stmt_st      = cntr_add->New_st(node_add, ad_x, spos1);
  stmt_list1.Append(stmt_st);
  // y = y - 1
  SPOS           spos2(0, 4, 1, 0);
  ADDR_DATUM_PTR ad_y   = _fs_add->Formal(1);
  NODE_PTR       node_y = cntr_add->New_ld(ad_y, spos2);
  node_1                = cntr_add->New_intconst(int_type, 1, spos2);
  NODE_PTR node_sub     = cntr_add->New_bin_arith(air::core::OPC_SUB, int_type,
                                                  node_y, node_1, spos2);
  stmt_st               = cntr_add->New_st(node_sub, ad_y, spos2);
  stmt_list1.Append(stmt_st);
  src_exp =
      "block ID(0xe) LINE(3:1:0)\n"
      "  st \"x\" FML[0x10000000] ID(0x12) LINE(3:1:0)\n"
      "    add RTYPE[0x2](int32_t)\n"
      "      ld \"x\" FML[0x10000000] RTYPE[0x2](int32_t)\n"
      "      intconst #0x1 RTYPE[0x2](int32_t)\n"
      "  st \"y\" FML[0x10000001] ID(0x16) LINE(4:1:0)\n"
      "    sub RTYPE[0x2](int32_t)\n"
      "      ld \"y\" FML[0x10000001] RTYPE[0x2](int32_t)\n"
      "      intconst #0x1 RTYPE[0x2](int32_t)\n"
      "end_block ID(0xe)\n";
  src_res = new_blk1->To_str();
  EXPECT_EQ(src_res, src_exp);

  stmt = new_slist.Append(stmt_list1)->Id();
  tgt_exp =
      "block ID(0xc) LINE(2:1:0)\n"
      "  st \"z\" VAR[0x10000002] ID(0x8) LINE(1:1:0)\n"
      "    add RTYPE[0x2](int32_t)\n"
      "      ld \"x\" FML[0x10000000] RTYPE[0x2](int32_t)\n"
      "      ld \"y\" FML[0x10000001] RTYPE[0x2](int32_t)\n"
      "  retv ID(0xa) LINE(1:1:0)\n"
      "    ld \"z\" VAR[0x10000002] RTYPE[0x2](int32_t)\n"
      "  st \"x\" FML[0x10000000] ID(0x12) LINE(3:1:0)\n"
      "    add RTYPE[0x2](int32_t)\n"
      "      ld \"x\" FML[0x10000000] RTYPE[0x2](int32_t)\n"
      "      intconst #0x1 RTYPE[0x2](int32_t)\n"
      "  st \"y\" FML[0x10000001] ID(0x16) LINE(4:1:0)\n"
      "    sub RTYPE[0x2](int32_t)\n"
      "      ld \"y\" FML[0x10000001] RTYPE[0x2](int32_t)\n"
      "      intconst #0x1 RTYPE[0x2](int32_t)\n"
      "end_block ID(0xc)\n";
  src_exp =
      "block ID(0xe) LINE(3:1:0)\n"
      "end_block ID(0xe)\n";
  tgt_res = new_slist.Block_node()->To_str();
  src_res = stmt_list1.Block_node()->To_str();
  EXPECT_TRUE(first == stmt);
  EXPECT_EQ(tgt_res, tgt_exp);
  EXPECT_EQ(src_res, src_exp);
  for (STMT_PTR st = new_slist.Begin_stmt(); st != new_slist.End_stmt();
       st          = st->Next()) {
    EXPECT_TRUE(st->Parent_node_id() == new_slist.Block_node()->Id());
    EXPECT_TRUE(st->Next()->Prev_id() == st->Id());
  }

  // case 4: nested loop
  SPOS     spos3(0, 5, 1, 0);
  NODE_PTR init = cntr_add->New_intconst(int_type, 0, spos3);
  NODE_PTR comp = cntr_add->New_bin_arith(
      air::core::OPC_LT, int_type, cntr_add->New_ld(ad_x, spos3),
      cntr_add->New_intconst(int_type, 10, spos3), spos3);
  NODE_PTR incr = cntr_add->New_bin_arith(
      air::core::OPC_ADD, int_type, cntr_add->New_ld(ad_x, spos3),
      cntr_add->New_intconst(int_type, 1, spos3), spos3);
  NODE_PTR loop_body = cntr_add->New_stmt_block(spos3);
  STMT_PTR outer_lst =
      cntr_add->New_do_loop(ad_x, init, comp, incr, loop_body, spos3);
  ori_slist.Append(outer_lst);
  STMT_LIST outer_loop_slist(loop_body);

  SPOS spos4(0, 6, 1, 0);
  init = cntr_add->New_intconst(int_type, 0, spos4);
  comp = cntr_add->New_bin_arith(
      air::core::OPC_LT, int_type, cntr_add->New_ld(ad_y, spos4),
      cntr_add->New_intconst(int_type, 20, spos4), spos4);
  incr = cntr_add->New_bin_arith(
      air::core::OPC_ADD, int_type, cntr_add->New_ld(ad_y, spos4),
      cntr_add->New_intconst(int_type, 2, spos4), spos4);
  loop_body = cntr_add->New_stmt_block(spos4);
  STMT_LIST inner_loop_slist(loop_body);
  STMT_PTR  inner_lst =
      cntr_add->New_do_loop(ad_y, init, comp, incr, loop_body, spos4);
  stmt_st = cntr_add->New_st(
      cntr_add->New_bin_arith(air::core::OPC_ADD, int_type,
                              cntr_add->New_ld(ad_x, spos4),
                              cntr_add->New_ld(ad_y, spos4), spos4),
      ad_y, spos4);
  inner_loop_slist.Append(stmt_st);
  outer_loop_slist.Append(inner_lst);
  stmt_st = cntr_add->New_st(
      cntr_add->New_bin_arith(air::core::OPC_SUB, int_type,
                              cntr_add->New_ld(ad_x, spos4),
                              cntr_add->New_ld(ad_y, spos4), spos4),
      ad_x, spos4);
  outer_loop_slist.Append(stmt_st);
  SPOS spos5(0, 7, 1, 0);
  stmt_st = cntr_add->New_retv(cntr_add->New_ld(ad_x, spos5), spos5);
  ori_slist.Append(stmt_st);
  tgt_exp =
      "block ID(0x4) LINE(1:1:0)\n"
      "  do_loop ID(0x20) LINE(5:1:0)\n"
      "    intconst #0 RTYPE[0x2](int32_t)\n"
      "    lt RTYPE[0x2](int32_t)\n"
      "      ld \"x\" FML[0x10000000] RTYPE[0x2](int32_t)\n"
      "      intconst #0xa RTYPE[0x2](int32_t)\n"
      "    add RTYPE[0x2](int32_t)\n"
      "      ld \"x\" FML[0x10000000] RTYPE[0x2](int32_t)\n"
      "      intconst #0x1 RTYPE[0x2](int32_t)\n"
      "    block ID(0x1f) LINE(5:1:0)\n"
      "      do_loop ID(0x2a) LINE(6:1:0)\n"
      "        intconst #0 RTYPE[0x2](int32_t)\n"
      "        lt RTYPE[0x2](int32_t)\n"
      "          ld \"y\" FML[0x10000001] RTYPE[0x2](int32_t)\n"
      "          intconst #0x14 RTYPE[0x2](int32_t)\n"
      "        add RTYPE[0x2](int32_t)\n"
      "          ld \"y\" FML[0x10000001] RTYPE[0x2](int32_t)\n"
      "          intconst #0x2 RTYPE[0x2](int32_t)\n"
      "        block ID(0x29) LINE(6:1:0)\n"
      "          st \"y\" FML[0x10000001] ID(0x2e) LINE(6:1:0)\n"
      "            add RTYPE[0x2](int32_t)\n"
      "              ld \"x\" FML[0x10000000] RTYPE[0x2](int32_t)\n"
      "              ld \"y\" FML[0x10000001] RTYPE[0x2](int32_t)\n"
      "        end_block ID(0x29)\n"
      "      st \"x\" FML[0x10000000] ID(0x32) LINE(6:1:0)\n"
      "        sub RTYPE[0x2](int32_t)\n"
      "          ld \"x\" FML[0x10000000] RTYPE[0x2](int32_t)\n"
      "          ld \"y\" FML[0x10000001] RTYPE[0x2](int32_t)\n"
      "    end_block ID(0x1f)\n"
      "  retv ID(0x34) LINE(7:1:0)\n"
      "    ld \"x\" FML[0x10000000] RTYPE[0x2](int32_t)\n"
      "end_block ID(0x4)\n";
  tgt_res = ori_slist.Block_node()->To_str();
  EXPECT_EQ(tgt_res, tgt_exp);
  for (STMT_PTR st = ori_slist.Begin_stmt(); st != ori_slist.End_stmt();
       st          = st->Next()) {
    EXPECT_TRUE(st->Enclosing_stmt_id() == _fs_add->Entry_stmt_id());
  }
  for (STMT_PTR st                           = outer_loop_slist.Begin_stmt();
       st != outer_loop_slist.End_stmt(); st = st->Next()) {
    EXPECT_TRUE(st->Enclosing_stmt_id() == outer_lst->Id());
  }
  for (STMT_PTR st                           = inner_loop_slist.Begin_stmt();
       st != inner_loop_slist.End_stmt(); st = st->Next()) {
    EXPECT_TRUE(st->Enclosing_stmt_id() == inner_lst->Id());
  }
  EXPECT_TRUE(cntr_add->Verify());
}

void TEST_CONTAINER::Run_test_func_add_def() {
  std::string expected(
      "FUN[0] \"My_add\"\n"
      "  FML[0x10000000] \"x\", TYP[0x2](primitive,\"int32_t\")\n"
      "  FML[0x10000001] \"y\", TYP[0x2](primitive,\"int32_t\")\n"
      "  VAR[0x10000002] \"z\"\n"
      "    scope_level[0x1], TYP[0x2](primitive,\"int32_t\")\n"
      "\n"
      "  func_entry \"My_add\" ENT[0x1] ID(0) LINE(1:1:0)\n"
      "    idname \"x\" FML[0x10000000] RTYPE[0x2](int32_t)\n"
      "    idname \"y\" FML[0x10000001] RTYPE[0x2](int32_t)\n"
      "    block ID(0x4) LINE(1:1:0)\n"
      "      st \"z\" VAR[0x10000002] ID(0x8) LINE(1:1:0)\n"
      "        add RTYPE[0x2](int32_t)\n"
      "          ld \"x\" FML[0x10000000] RTYPE[0x2](int32_t)\n"
      "          ld \"y\" FML[0x10000001] RTYPE[0x2](int32_t)\n"
      "      retv ID(0xa) LINE(1:1:0)\n"
      "        ld \"z\" VAR[0x10000002] RTYPE[0x2](int32_t)\n"
      "    end_block ID(0x4)\n");
  std::string result(_fs_add->To_str());
  EXPECT_EQ(result, expected);
}

void TEST_CONTAINER::Run_test_func_main_def() {
  std::string expected(
      "FUN[0x2] \"main\"\n"
      "  FML[0x10000000] \"argc\", TYP[0x2](primitive,\"int32_t\")\n"
      "  FML[0x10000001] \"argv\", TYP[0x14](pointer,\"_noname\")\n"
      "  VAR[0x10000002] \"a\"\n"
      "    scope_level[0x1], TYP[0x2](primitive,\"int32_t\")\n"
      "  VAR[0x10000003] \"b\"\n"
      "    scope_level[0x1], TYP[0x2](primitive,\"int32_t\")\n"
      "  VAR[0x10000004] \"c\"\n"
      "    scope_level[0x1], TYP[0x2](primitive,\"int32_t\")\n"
      "  PREG[0x10000000] TYP[0x2](primitive,\"int32_t\",size:4), "
      "Home[0xffffffff]\n"
      "\n"
      "  func_entry \"main\" ENT[0x3] ID(0) LINE(1:1:0)\n"
      "    idname \"argc\" FML[0x10000000] RTYPE[0x2](int32_t)\n"
      "    idname \"argv\" FML[0x10000001] RTYPE[0x14](_noname)\n"
      "    block ID(0x4) LINE(1:1:0)\n"
      "      st \"a\" VAR[0x10000002] ID(0x6) LINE(1:1:0)\n"
      "        intconst #0x5 RTYPE[0x2](int32_t)\n"
      "      st \"b\" VAR[0x10000003] ID(0x8) LINE(1:1:0)\n"
      "        intconst #0x6 RTYPE[0x2](int32_t)\n"
      "      call \"My_add\" ENT[0x1] ID(0x9) LINE(1:1:0)\n"
      "        ld \"a\" VAR[0x10000002] RTYPE[0x2](int32_t)\n"
      "        ld \"b\" VAR[0x10000003] RTYPE[0x2](int32_t)\n"
      "      st \"c\" VAR[0x10000004] ID(0xd) LINE(1:1:0)\n"
      "        ldp PREG[0x10000000] RTYPE[0x2](int32_t)\n"
      "      retv ID(0xf) LINE(1:1:0)\n"
      "        ld \"c\" VAR[0x10000004] RTYPE[0x2](int32_t)\n"
      "    end_block ID(0x4)\n");
  std::string result(_fs_main->To_str());
  EXPECT_EQ(result, expected);
}

void TEST_CONTAINER::Run_test_stmt_attr() {
  NODE_PTR call_node = _call_stmt->Node();
  call_node->Set_attr("key1", "value1");
  call_node->Set_attr("key2", "value2");
  call_node->Set_attr("key3", "");
  std::string_view key1 = call_node->Attr("key1");
  std::string_view key2 = call_node->Attr("key2");
  const char*      key3 = call_node->Attr("key3");
  const char*      key4 = call_node->Attr("key4");
  EXPECT_EQ(key1, "value1");
  EXPECT_EQ(key2, "value2");
  EXPECT_STREQ(key3, "");
  EXPECT_TRUE(key4 == NULL);
  int attr_cnt = 0;
  for (ATTR_ITER ait = call_node->Begin_attr(); ait != call_node->End_attr();
       ++ait) {
    if (strcmp((*ait)->Key(), "key1") == 0) {
      EXPECT_EQ((*ait)->Value(), "value1");
      ++attr_cnt;
    } else if (strcmp((*ait)->Key(), "key2") == 0) {
      EXPECT_EQ((*ait)->Value(), "value2");
      ++attr_cnt;
    } else if (strcmp((*ait)->Key(), "key3") == 0) {
      EXPECT_EQ((*ait)->Value(), "");
      ++attr_cnt;
    } else {
      EXPECT_TRUE(false);
      ++attr_cnt;
    }
  }
  EXPECT_EQ(attr_cnt, 3);
  call_node->Set_attr("key2", "value4");
  key2 = call_node->Attr("key2");
  EXPECT_EQ(key2, "value4");
  for (ATTR_ITER ait = call_node->Begin_attr(); ait != call_node->End_attr();
       ++ait) {
    if (strcmp((*ait)->Key(), "key1") == 0) {
      EXPECT_EQ((*ait)->Value(), "value1");
      ++attr_cnt;
    } else if (strcmp((*ait)->Key(), "key2") == 0) {
      EXPECT_EQ((*ait)->Value(), "value4");
      ++attr_cnt;
    } else if (strcmp((*ait)->Key(), "key3") == 0) {
      EXPECT_EQ((*ait)->Value(), "");
      ++attr_cnt;
    } else {
      EXPECT_TRUE(false);
      ++attr_cnt;
    }
  }
  EXPECT_EQ(attr_cnt, 6);
  int iarr[] = {1, 2, 3, 4};
  call_node->Set_attr("iarr", iarr, 4);
  const int* iarr_val = call_node->Attr<int>("iarr");
  EXPECT_TRUE(iarr_val != nullptr);
  EXPECT_EQ(iarr_val[0], 1);
  EXPECT_EQ(iarr_val[1], 2);
  EXPECT_EQ(iarr_val[2], 3);
  EXPECT_EQ(iarr_val[3], 4);
}

void TEST_CONTAINER::Run_test_clone() {
  GLOB_SCOPE* cloned_glob = new GLOB_SCOPE(1, true);
  cloned_glob->Clone(*_glob);
  FUNC_SCOPE* cloned_fs = &cloned_glob->New_func_scope(_fs_main->Owning_func());
  cloned_fs->Clone(*_fs_main);
  CONTAINER* cloned_cntr = &cloned_fs->Container();
  cloned_cntr->New_func_entry(SPOS());
  STMT_PTR cloned_stmt = cloned_cntr->Clone_stmt_tree(_call_stmt);
  cloned_cntr->Stmt_list().Append(cloned_stmt);

  // the statement ID is different from the original one, which is 0x9
  std::string expected(
      "call \"My_add\" ENT[0x1] ID(0x5) LINE(1:1:0)\n"
      "  ld \"a\" VAR[0x10000002] RTYPE[0x2](int32_t)\n"
      "  ld \"b\" VAR[0x10000003] RTYPE[0x2](int32_t)\n");
  std::string result(cloned_stmt->To_str());
  EXPECT_EQ(result, expected);

  delete cloned_glob;
}

void TEST_CONTAINER::Run_test_opcode() {
  std::string expected(
      "Domain 0: CORE\n"
      "  invalid          kids: 0   \n"
      "  end_stmt_list    kids: 0   END_BB,STMT\n"
      "  void             kids: 0   EXPR\n"
      "  block            kids: 0   SCF,EXPR\n"
      "  add              kids: 2   EXPR\n"
      "  sub              kids: 2   EXPR\n"
      "  mul              kids: 2   EXPR\n"
      "  shl              kids: 2   EXPR\n"
      "  ashr             kids: 2   EXPR\n"
      "  lshr             kids: 2   EXPR\n"
      "  eq               kids: 2   EXPR,COMPARE\n"
      "  ne               kids: 2   EXPR,COMPARE\n"
      "  lt               kids: 2   EXPR,COMPARE\n"
      "  le               kids: 2   EXPR,COMPARE\n"
      "  gt               kids: 2   EXPR,COMPARE\n"
      "  ge               kids: 2   EXPR,COMPARE\n"
      "  ld               kids: 0   LEAF,EXPR,LOAD,SYM,ATTR,ACC_TYPE\n"
      "  ild              kids: 1   EXPR,LOAD,ATTR,ACC_TYPE\n"
      "  ldf              kids: 0   LEAF,EXPR,LOAD,SYM,FIELD_ID,ATTR,ACC_TYPE\n"
      "  ldo              kids: 0   LEAF,EXPR,LOAD,SYM,OFFSET,ACC_TYPE\n"
      "  ldp              kids: 0   LEAF,EXPR,LOAD,ATTR,ACC_TYPE,PREG\n"
      "  ldpf             kids: 0   "
      "LEAF,EXPR,LOAD,FIELD_ID,ATTR,ACC_TYPE,PREG\n"
      "  ldc              kids: 0   LEAF,EXPR,CONST_ID\n"
      "  lda              kids: 0   LEAF,EXPR,SYM\n"
      "  ldca             kids: 0   LEAF,EXPR,CONST_ID\n"
      "  func_entry       kids: 1   EX_CHILD,EX_FIELD,SCF,STMT\n"
      "  idname           kids: 0   LEAF,EXPR,SYM,ATTR\n"
      "  st               kids: 1   STMT,STORE,SYM,ATTR,ACC_TYPE\n"
      "  ist              kids: 2   STMT,STORE,ATTR,ACC_TYPE\n"
      "  stf              kids: 1   STMT,STORE,SYM,FIELD_ID,ATTR,ACC_TYPE\n"
      "  sto              kids: 1   STMT,STORE,SYM,OFFSET,ACC_TYPE\n"
      "  stp              kids: 1   STMT,STORE,ATTR,ACC_TYPE,PREG\n"
      "  stpf             kids: 1   STMT,STORE,FIELD_ID,ATTR,ACC_TYPE,PREG\n"
      "  entry            kids: 0   SCF,STMT\n"
      "  ret              kids: 0   LEAF,STMT\n"
      "  retv             kids: 1   STMT,ATTR\n"
      "  pragma           kids: 0   LEAF,STMT\n"
      "  call             kids: 0   "
      "EX_CHILD,EX_FIELD,RET_VAR,STMT,CALL,FLAGS,ATTR\n"
      "  do_loop          kids: 4   SCF,STMT\n"
      "  intconst         kids: 0   LEAF,EXPR\n"
      "  zero             kids: 0   LEAF,EXPR,ATTR\n"
      "  one              kids: 0   LEAF,EXPR,ATTR\n"
      "  if               kids: 3   SCF,STMT\n"
      "  array            kids: 1   EX_CHILD,EXPR\n"
      "  validate         kids: 4   STMT,LIB_CALL\n"
      "  dump_var         kids: 3   STMT,LIB_CALL\n"
      "  intrn_call       kids: 0   EX_CHILD,RET_VAR,STMT,FLAGS,ATTR\n"
      "  intrn_op         kids: 0   EX_CHILD,EXPR,FLAGS,ATTR\n"
      "  comment          kids: 0   STMT\n"
      "  and              kids: 2   EXPR\n"
      "  or               kids: 2   EXPR\n"
      "  band             kids: 2   EXPR\n"
      "  bor              kids: 2   EXPR\n"
      "  bnot             kids: 1   EXPR\n"
      "  floordiv         kids: 2   EXPR\n"
      "  mod              kids: 2   EXPR\n"
      "\n");
  std::stringbuf buf;
  std::ostream   os(&buf);
  META_INFO::Print(os);
  std::string result(buf.str());
  EXPECT_EQ(result, expected);
}

void TEST_CONTAINER::Run_test_verify_func_add() {
  EXPECT_TRUE(_fs_add->Container().Verify());
}

void TEST_CONTAINER::Run_test_verify_func_main() {
  EXPECT_TRUE(_fs_main->Container().Verify());
}

void TEST_CONTAINER::Run_test_verify_fail() {
  STR_PTR     name     = _glob->New_str("Bad_func");
  FUNC_PTR    func_ptr = _glob->New_func(name, SPOS());
  FUNC_SCOPE* bad_fs   = &_glob->New_func_scope(func_ptr);
  EXPECT_FALSE(bad_fs->Container().Verify());
}

void TEST_CONTAINER::Run_test_func_entry_node() {
  // 1. create a new binary input function
  const char* func_name  = "Bin_func";
  STR_PTR     name       = _glob->New_str(func_name);
  FUNC_PTR    func       = _glob->New_func(name, SPOS());
  FUNC_SCOPE* func_scope = &_glob->New_func_scope(func);
  CONTAINER*  cntr       = &func_scope->Container();

  // 2. create entry symbol
  SIGNATURE_TYPE_PTR sig = _glob->New_sig_type();
  sig->Set_complete();
  ENTRY_PTR entry = _glob->New_entry_point(sig, func, func_name, SPOS());
  func->Add_entry_point(entry->Id());

  // 3. create entry node
  TYPE_PTR       f32        = _glob->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
  STMT_PTR       entry_stmt = cntr->New_func_entry(SPOS(), 2);
  ADDR_DATUM_PTR formal0    = func_scope->New_formal(f32->Id(), "x", SPOS());
  NODE_PTR       idname0    = cntr->New_idname(formal0, SPOS());
  ADDR_DATUM_PTR formal1    = func_scope->New_formal(f32->Id(), "y", SPOS());
  NODE_PTR       idname1    = cntr->New_idname(formal1, SPOS());
  entry_stmt->Node()->Set_child(0, idname0);
  entry_stmt->Node()->Set_child(1, idname1);

  // 4. verify entry node
  ASSERT_TRUE(cntr->Verify_node(entry_stmt->Node()));
  std::string result = entry_stmt->To_str();
  std::string expected(
      "func_entry \"Bin_func\" ENT[0x5] ID(0) LINE(0:0:0)\n"
      "  idname \"x\" FML[0x10000000] RTYPE[0x8](float32_t)\n"
      "  idname \"y\" FML[0x10000001] RTYPE[0x8](float32_t)\n"
      "  block ID(0x2) LINE(0:0:0)\n"
      "  end_block ID(0x2)\n");
  EXPECT_EQ(result, expected);
}

void TEST_CONTAINER::Run_test_func_no_ret() {
  STR_PTR  name = _glob->New_str("No_ret");
  FUNC_PTR func = _glob->New_func(name, SPOS());
  func->Set_parent(_glob->Comp_env_id());
  SIGNATURE_TYPE_PTR sig   = _glob->New_sig_type();
  TYPE_PTR           rtype = _glob->Prim_type(PRIMITIVE_TYPE::VOID);
  PREG_PTR           retv  = _fs_add->New_preg(rtype);
  _glob->New_ret_param(rtype, sig);
  sig->Set_complete();
  ENTRY_PTR  entry = _glob->New_entry_point(sig, func, name, SPOS());
  CONTAINER* cntr  = &_fs_add->Container();
  STMT_PTR   stmt  = cntr->New_call(entry, retv, 0, SPOS());

  EXPECT_FALSE(stmt->Node()->Entry()->Has_retv());
  EXPECT_TRUE(retv->Id() == stmt->Node()->Ret_preg_id());

  stmt = cntr->New_call(entry, PREG_PTR(), 0, SPOS());
  EXPECT_FALSE(stmt->Node()->Entry()->Has_retv());

  stmt = cntr->New_call(entry, Null_ptr, 0, SPOS());
  EXPECT_FALSE(stmt->Node()->Entry()->Has_retv());
}

TEST_F(TEST_CONTAINER, ctor_dtor) { Run_test_ctor_dtor(); }
TEST_F(TEST_CONTAINER, stmt_list_prepend) { Run_test_stmt_list_prepend(); }
TEST_F(TEST_CONTAINER, stmt_list_append) { Run_test_stmt_list_append(); }
TEST_F(TEST_CONTAINER, func_add_def) { Run_test_func_add_def(); }
TEST_F(TEST_CONTAINER, func_main_def) { Run_test_func_main_def(); }
TEST_F(TEST_CONTAINER, stmt_attr) { Run_test_stmt_attr(); }
TEST_F(TEST_CONTAINER, clone) { Run_test_clone(); }
TEST_F(TEST_CONTAINER, opcode) { Run_test_opcode(); }
TEST_F(TEST_CONTAINER, verify_func_add) { Run_test_verify_func_add(); }
TEST_F(TEST_CONTAINER, verify_func_main) { Run_test_verify_func_main(); }
TEST_F(TEST_CONTAINER, verify_fail) { Run_test_verify_fail(); }
TEST_F(TEST_CONTAINER, func_entry_node) { Run_test_func_entry_node(); }
TEST_F(TEST_CONTAINER, func_no_ret) { Run_test_func_no_ret(); }
