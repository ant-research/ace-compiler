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
    NODE_PTR node_x = cntr_add->New_ld(formal_x, spos);
    NODE_PTR node_y = cntr_add->New_ld(formal_y, spos);
    NODE_PTR node_add =
        cntr_add->New_bin_arith(air::core::OPC_ADD, node_x, node_y, spos);
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
  void Run_test_func_add_def();
  void Run_test_func_main_def();
  void Run_test_stmt_attr();
  void Run_test_clone();
  void Run_test_opcode();
  void Run_test_verify_func_add();
  void Run_test_verify_func_main();
  void Run_test_verify_fail();
  void Run_test_func_entry_node();

  GLOB_SCOPE* _glob;
  FUNC_SCOPE* _fs_add;
  FUNC_SCOPE* _fs_main;
  STMT_PTR    _call_stmt;
};

void TEST_CONTAINER::Run_test_func_add_def() {
  std::string expected(
      "FUN[0] \"My_add\"\n"
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
      "  void             kids: 0   \n"
      "  block            kids: 0   SCF\n"
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
      "  ld               kids: 0   EXPR,LOAD,SYM,ATTR,ACC_TYPE\n"
      "  ild              kids: 1   EXPR,LOAD,ATTR,ACC_TYPE\n"
      "  ldf              kids: 0   EXPR,LOAD,SYM,FIELD_ID,ACC_TYPE\n"
      "  ldo              kids: 0   EXPR,LOAD,SYM,OFFSET,ACC_TYPE\n"
      "  ldp              kids: 0   EXPR,LOAD,ATTR,ACC_TYPE,PREG\n"
      "  ldpf             kids: 0   EXPR,LOAD,FIELD_ID,ACC_TYPE,PREG\n"
      "  ldc              kids: 0   EXPR,CONST_ID\n"
      "  lda              kids: 0   EXPR,SYM\n"
      "  ldca             kids: 0   EXPR,CONST_ID\n"
      "  func_entry       kids: 1   EX_CHILD,EX_FIELD,SCF\n"
      "  idname           kids: 0   EXPR,SYM,ATTR\n"
      "  st               kids: 1   STMT,STORE,SYM,ATTR,ACC_TYPE\n"
      "  ist              kids: 2   STMT,STORE,ATTR,ACC_TYPE\n"
      "  stf              kids: 1   STMT,STORE,SYM,FIELD_ID,ACC_TYPE\n"
      "  sto              kids: 1   STMT,STORE,SYM,OFFSET,ACC_TYPE\n"
      "  stp              kids: 1   STMT,STORE,ATTR,ACC_TYPE,PREG\n"
      "  stpf             kids: 1   STMT,STORE,FIELD_ID,ACC_TYPE,PREG\n"
      "  entry            kids: 0   SCF\n"
      "  ret              kids: 0   STMT\n"
      "  retv             kids: 1   STMT,ATTR\n"
      "  pragma           kids: 0   LEAF,STMT,SYM,VALUE\n"
      "  call             kids: 0   "
      "EX_CHILD,EX_FIELD,RET_VAR,STMT,CALL,FLAGS,ATTR\n"
      "  do_loop          kids: 4   SCF\n"
      "  intconst         kids: 0   EXPR\n"
      "  zero             kids: 0   EXPR\n"
      "  one              kids: 0   EXPR\n"
      "  if               kids: 3   SCF\n"
      "  array            kids: 1   EX_CHILD,EXPR\n"
      "  validate         kids: 4   STMT,LIB_CALL\n"
      "  tm_start         kids: 0   STMT,CONST_ID,LIB_CALL\n"
      "  tm_taken         kids: 0   STMT,CONST_ID,LIB_CALL\n"
      "  dump_var         kids: 3   STMT,LIB_CALL\n\n");
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
      "  end_block ID(2)\n");
  EXPECT_EQ(result, expected);
}

TEST_F(TEST_CONTAINER, func_add_def) { Run_test_func_add_def(); }
TEST_F(TEST_CONTAINER, func_main_def) { Run_test_func_main_def(); }
TEST_F(TEST_CONTAINER, stmt_attr) { Run_test_stmt_attr(); }
TEST_F(TEST_CONTAINER, clone) { Run_test_clone(); }
TEST_F(TEST_CONTAINER, opcode) { Run_test_opcode(); }
TEST_F(TEST_CONTAINER, verify_func_add) { Run_test_verify_func_add(); }
TEST_F(TEST_CONTAINER, verify_func_main) { Run_test_verify_func_main(); }
TEST_F(TEST_CONTAINER, verify_fail) { Run_test_verify_fail(); }
TEST_F(TEST_CONTAINER, func_entry_node) { Run_test_func_entry_node(); }
