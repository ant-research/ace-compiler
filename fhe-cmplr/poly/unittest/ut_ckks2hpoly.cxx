//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <limits.h>

#include "air/base/meta_info.h"
#include "air/base/st.h"
#include "air/base/transform_ctx.h"
#include "air/core/handler.h"
#include "air/core/opcode.h"
#include "ckks2hpoly.h"
#include "fhe/ckks/ckks_gen.h"
#include "fhe/sihe/sihe_gen.h"
#include "fhe/test/gen_ckks_ir.h"
#include "fhe/util/util.h"
#include "gtest/gtest.h"
#include "poly_lower_ctx.h"

using namespace air::base;
using namespace air::util;
using namespace fhe::ckks;
using namespace fhe::poly;
using namespace fhe::poly::test;

namespace {

class TEST_CKKS2HPOLY : public ::testing::Test {
protected:
  TEST_CKKS2HPOLY() : _ckks_ir_gen(_fhe_ctx) {}
  void SetUp() override { _cntr = Create_ckks_ir(_fhe_ctx); }

  void TearDown() {
    META_INFO::Remove_all();
    free(_visitor);
    free(_ctx);
  }

  CKKS2HPOLY_VISITOR* New_visitor() {
    FUNC_SCOPE* fscope = _cntr->Parent_func_scope();

    // clone old scope
    GLOB_SCOPE* old_scope = _cntr->Glob_scope();
    GLOB_SCOPE* new_glob  = new GLOB_SCOPE(1, true);
    new_glob->Clone(*old_scope);
    FUNC_SCOPE* new_fscope = &new_glob->New_func_scope(fscope->Owning_func());
    new_fscope->Clone(*fscope);
    _new_cntr = &(new_fscope->Container());

    _ctx     = new POLY_LOWER_CTX(_poly_config, &_fhe_ctx, _new_cntr);
    _visitor = new CKKS2HPOLY_VISITOR(*_ctx);
    return _visitor;
  }

  SPOS            Spos() { return GLOB_SCOPE::Get()->Unknown_simple_spos(); }
  POLY_LOWER_CTX* Ctx() { return _ctx; }
  POLY_IR_GEN&    Poly_gen() { return _ctx->Poly_gen(); }
  TRANSFORM_CTX*  Trav_ctx() { return _trav_ctx; }
  CONTAINER*      Container() { return _cntr; }
  CONTAINER*      New_container() { return _new_cntr; }
  CONTAINER*      Create_ckks_ir(fhe::core::LOWER_CTX& lower_ctx);
  CKKS_IR_GEN&    Ckks_ir_gen() { return _ckks_ir_gen; }

  ADDR_DATUM_PTR Var_x() { return _var_x; }
  ADDR_DATUM_PTR Var_y() { return _var_y; }
  ADDR_DATUM_PTR Var_z() { return _var_z; }
  ADDR_DATUM_PTR Var_p() { return _var_p; }
  ADDR_DATUM_PTR Var_ciph3() { return _var_ciph3; }
  TYPE_PTR       Ciph_ty() { return Ckks_ir_gen().Ciph_ty(); }
  TYPE_PTR       Ciph3_ty() { return Ckks_ir_gen().Ciph3_ty(); }
  TYPE_PTR       Plain_ty() { return Ckks_ir_gen().Plain_ty(); }

private:
  fhe::poly::POLY_CONFIG       _poly_config;
  fhe::core::LOWER_CTX         _fhe_ctx;
  fhe::poly::test::CKKS_IR_GEN _ckks_ir_gen;
  POLY_LOWER_CTX*              _ctx;
  CKKS2HPOLY_VISITOR*          _visitor;
  TRANSFORM_CTX*               _trav_ctx;
  CONTAINER*                   _cntr;
  CONTAINER*                   _new_cntr;

  ADDR_DATUM_PTR _var_x;
  ADDR_DATUM_PTR _var_y;
  ADDR_DATUM_PTR _var_z;
  ADDR_DATUM_PTR _var_p;
  ADDR_DATUM_PTR _var_ciph3;
};

CONTAINER* TEST_CKKS2HPOLY::Create_ckks_ir(fhe::core::LOWER_CTX& lower_ctx) {
  _var_x     = Ckks_ir_gen().Gen_ciph_var("ciph_x");
  _var_y     = Ckks_ir_gen().Gen_ciph_var("ciph_y");
  _var_z     = Ckks_ir_gen().Gen_ciph_var("ciph_z");
  _var_p     = Ckks_ir_gen().Gen_plain_var("plain");
  _var_ciph3 = Ckks_ir_gen().Gen_ciph3_var("ciph3");
  return Ckks_ir_gen().Main_container();
}

TEST_F(TEST_CKKS2HPOLY, Handle_add_ciph) {
  STMT_PTR stmt = Ckks_ir_gen().Gen_add(Container(), Var_z(), Var_x(), Var_y());
  std::cout << "CKKS2HPOLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  stmt->Print();

  // vistor need a block node to start
  POLY_LOWER_RETV pair =
      New_visitor()->Visit<POLY_LOWER_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2HPOLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  New_container()->Print();
}

TEST_F(TEST_CKKS2HPOLY, Handle_add_plain) {
  STMT_PTR stmt = Ckks_ir_gen().Gen_add(Container(), Var_z(), Var_x(), Var_p());
  std::cout << "CKKS2HPOLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  stmt->Print();

  // vistor need a block node to start
  POLY_LOWER_RETV pair =
      New_visitor()->Visit<POLY_LOWER_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2HPOLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  New_container()->Print();
}

TEST_F(TEST_CKKS2HPOLY, Handle_mul_plain) {
  STMT_PTR stmt = Ckks_ir_gen().Gen_mul(Container(), Var_z(), Var_x(), Var_p());
  std::cout << "CKKS2HPOLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  stmt->Print();

  // vistor need a block node to start
  POLY_LOWER_RETV pair =
      New_visitor()->Visit<POLY_LOWER_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2HPOLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  New_container()->Print();
}

TEST_F(TEST_CKKS2HPOLY, Handle_mul_ciph) {
  STMT_PTR stmt =
      Ckks_ir_gen().Gen_mul(Container(), Var_ciph3(), Var_x(), Var_y());
  std::cout << "CKKS2HPOLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  stmt->Print();

  // vistor need a block node to start
  POLY_LOWER_RETV pair =
      New_visitor()->Visit<POLY_LOWER_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2HPOLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  New_container()->Print();
}

TEST_F(TEST_CKKS2HPOLY, Handle_mul_ciph_preg) {
  PREG_PTR preg_z = Container()->Parent_func_scope()->New_preg(Ciph3_ty());
  STMT_PTR stmt = Ckks_ir_gen().Gen_mul(Container(), preg_z, Var_x(), Var_y());
  std::cout << "CKKS2HPOLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  stmt->Print();

  // vistor need a block node to start
  POLY_LOWER_RETV pair =
      New_visitor()->Visit<POLY_LOWER_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2HPOLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  New_container()->Print();
}

TEST_F(TEST_CKKS2HPOLY, Handle_mul_float) {
  STMT_PTR stmt =
      Ckks_ir_gen().Gen_mul_float(Container(), Var_z(), Var_x(), 3.0);
  std::cout << "CKKS2HPOLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  stmt->Print();

  // vistor need a block node to start
  POLY_LOWER_RETV pair =
      New_visitor()->Visit<POLY_LOWER_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2HPOLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  New_container()->Print();
}

TEST_F(TEST_CKKS2HPOLY, Handle_relin_inline) {
  STMT_PTR stmt = Ckks_ir_gen().Gen_relin(Container(), Var_z(), Var_ciph3());
  std::cout << "CKKS2HPOLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  stmt->Print();

  // vistor need a block node to start
  POLY_LOWER_RETV pair =
      New_visitor()->Visit<POLY_LOWER_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2HPOLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  New_container()->Print();
}

TEST_F(TEST_CKKS2HPOLY, Handle_relin_func) {
  STMT_PTR stmt = Ckks_ir_gen().Gen_relin(Container(), Var_z(), Var_ciph3());
  CKKS2HPOLY_VISITOR* visitor = New_visitor();
  GTEST_SKIP() << "relin call is not supported yet";
  Ctx()->Config().Set_inline_relin(false);
  std::cout << "CKKS2HPOLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  stmt->Print();

  // vistor need a block node to start
  POLY_LOWER_RETV pair =
      visitor->Visit<POLY_LOWER_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2HPOLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  New_container()->Glob_scope()->Print();
}

TEST_F(TEST_CKKS2HPOLY, Handle_relin_func_preg) {
  PREG_PTR preg_z = Container()->Parent_func_scope()->New_preg(Ciph_ty());
  STMT_PTR stmt   = Ckks_ir_gen().Gen_relin(Container(), preg_z, Var_ciph3());
  CKKS2HPOLY_VISITOR* visitor = New_visitor();
  GTEST_SKIP() << "relin call is not supported yet";

  Ctx()->Config().Set_inline_relin(false);
  std::cout << "CKKS2HPOLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  stmt->Print();

  // vistor need a block node to start
  POLY_LOWER_RETV pair =
      visitor->Visit<POLY_LOWER_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2HPOLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  New_container()->Glob_scope()->Print();
}

TEST_F(TEST_CKKS2HPOLY, Handle_load_ciph) {
  NODE_PTR node = Container()->New_ld(Var_x(), Spos());
  std::cout << "CKKS2HPOLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  node->Print();

  POLY_LOWER_RETV pair = New_visitor()->Visit<POLY_LOWER_RETV>(node);

  std::cout << "\nCKKS2HPOLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  pair.Node1()->Print();
  pair.Node2()->Print();
}

TEST_F(TEST_CKKS2HPOLY, Handle_load_plain) {
  NODE_PTR node = Container()->New_ld(Var_p(), Spos());
  std::cout << "CKKS2HPOLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  node->Print();

  POLY_LOWER_RETV pair = New_visitor()->Visit<POLY_LOWER_RETV>(node);

  std::cout << "\nCKKS2HPOLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  pair.Node1()->Print();
  EXPECT_EQ(pair.Node2(), air::base::Null_ptr);
  EXPECT_EQ(pair.Kind(), RETV_KIND::RK_PLAIN_POLY);
}

TEST_F(TEST_CKKS2HPOLY, Handle_rotate_inline) {
  STMT_PTR stmt = Ckks_ir_gen().Gen_rotate(Container(), Var_z(), Var_x(), 5);
  std::cout << "CKKS2HPOLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  stmt->Print();

  // vistor need a block node to start
  POLY_LOWER_RETV pair =
      New_visitor()->Visit<POLY_LOWER_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2HPOLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  New_container()->Print();
}

TEST_F(TEST_CKKS2HPOLY, Handle_rotate_func) {
  STMT_PTR stmt = Ckks_ir_gen().Gen_rotate(Container(), Var_z(), Var_x(), 5);
  CKKS2HPOLY_VISITOR* visitor = New_visitor();

  GTEST_SKIP() << "rotate call is not supported yet";
  std::cout << "CKKS2HPOLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  stmt->Print();
  Ctx()->Config().Set_inline_rotate(false);

  // vistor need a block node to start
  POLY_LOWER_RETV pair =
      visitor->Visit<POLY_LOWER_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2HPOLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  New_container()->Glob_scope()->Print();
}

TEST_F(TEST_CKKS2HPOLY, Handle_rotate_func_preg) {
  PREG_PTR preg_z = Container()->Parent_func_scope()->New_preg(Ciph_ty());
  PREG_PTR preg_x = Container()->Parent_func_scope()->New_preg(Ciph_ty());
  STMT_PTR stmt   = Ckks_ir_gen().Gen_rotate(Container(), preg_z, preg_x, 5);
  CKKS2HPOLY_VISITOR* visitor = New_visitor();

  GTEST_SKIP() << "rotate call is not supported yet";

  std::cout << "CKKS2HPOLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  stmt->Print();
  Ctx()->Config().Set_inline_rotate(false);

  // vistor need a block node to start
  POLY_LOWER_RETV pair =
      visitor->Visit<POLY_LOWER_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2HPOLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  New_container()->Glob_scope()->Print();
}

TEST_F(TEST_CKKS2HPOLY, Handle_rescale) {
  STMT_PTR stmt = Ckks_ir_gen().Gen_rescale(Container(), Var_z(), Var_x());
  std::cout << "CKKS2HPOLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  stmt->Print();

  // vistor need a block node to start
  POLY_LOWER_RETV pair =
      New_visitor()->Visit<POLY_LOWER_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2HPOLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  New_container()->Print();
}

TEST_F(TEST_CKKS2HPOLY, Handle_bts) {
  STMT_PTR stmt = Ckks_ir_gen().Gen_bootstrap(Container(), Var_x());
  std::cout << "CKKS2HPOLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  stmt->Print();

  // vistor need a block node to start
  POLY_LOWER_RETV pair =
      New_visitor()->Visit<POLY_LOWER_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2HPOLY After:" << std::endl;
  std::cout << "====================================" << std::endl;

  New_container()->Print();
}

TEST_F(TEST_CKKS2HPOLY, Handle_encode) {
  STMT_PTR stmt = Ckks_ir_gen().Gen_encode(Container(), Var_p());
  std::cout << "CKKS2HPOLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  stmt->Print();

  // vistor need a block node to start
  POLY_LOWER_RETV pair =
      New_visitor()->Visit<POLY_LOWER_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2HPOLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  New_container()->Print();
  EXPECT_EQ(pair.Node2(), air::base::Null_ptr);
}

// DISABLED as PREG LDID not supported yet
TEST_F(TEST_CKKS2HPOLY, Handle_stp) {
  CONTAINER* cntr = Container();
  STMT_LIST  sl   = cntr->Stmt_list();
  NODE_PTR   n_x  = cntr->New_ld(Var_x(), Spos());
  NODE_PTR   rescale_node =
      cntr->New_cust_node(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                            fhe::ckks::CKKS_OPERATOR::RESCALE),
                          Ciph_ty(), Spos());
  rescale_node->Set_child(0, n_x);
  PREG_PTR tmp          = cntr->Parent_func_scope()->New_preg(Ciph_ty());
  STMT_PTR rescale_stmt = cntr->New_stp(rescale_node, tmp, Spos());
  sl.Append(rescale_stmt);
  std::cout << "CKKS2HPOLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  rescale_stmt->Print();

  // vistor need a block node to start
  POLY_LOWER_RETV pair =
      New_visitor()->Visit<POLY_LOWER_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2HPOLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  New_container()->Print();
  EXPECT_EQ(pair.Node2(), air::base::Null_ptr);
}

TEST_F(TEST_CKKS2HPOLY, Handle_all) {
  Ckks_ir_gen().Gen_add(Container(), Var_z(), Var_x(), Var_y());
  Ckks_ir_gen().Gen_add(Container(), Var_z(), Var_x(), Var_p());
  Ckks_ir_gen().Gen_mul(Container(), Var_z(), Var_x(), Var_p());
  Ckks_ir_gen().Gen_mul(Container(), Var_ciph3(), Var_x(), Var_y());
  Ckks_ir_gen().Gen_mul_float(Container(), Var_z(), Var_x(), 3.0);
  Ckks_ir_gen().Gen_relin(Container(), Var_z(), Var_ciph3());
  Ckks_ir_gen().Gen_relin(Container(), Var_z(), Var_ciph3());
  Ckks_ir_gen().Gen_rotate(Container(), Var_z(), Var_x(), 5);
  Ckks_ir_gen().Gen_rotate(Container(), Var_z(), Var_x(), 5);
  Ckks_ir_gen().Gen_rescale(Container(), Var_z(), Var_x());
  Ckks_ir_gen().Gen_bootstrap(Container(), Var_x());
  Ckks_ir_gen().Gen_encode(Container(), Var_p());
  Ckks_ir_gen().Gen_ret(Container(), Var_z());

  std::cout << "CKKS2HPOLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  Container()->Glob_scope()->Print();

  POLY_LOWER_RETV pair =
      New_visitor()->Visit<POLY_LOWER_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2HPOLY After:" << std::endl;
  std::cout << "====================================" << std::endl;

  New_container()->Glob_scope()->Print();
}

}  // namespace
