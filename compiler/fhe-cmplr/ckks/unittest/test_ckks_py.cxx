//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/ckks/ckks_py_airgen.h"
#include "fhe/sihe/sihe_opcode.h"
#include "fhe/test/lower_ckks.h"
using namespace air::base;
using namespace air::util;
using namespace fhe::ckks;
using namespace fhe::sihe;
using namespace fhe::poly::test;

namespace fhe {
namespace ckks {
namespace test {

class TEST_CKKS_PY : public TEST_LOWER_CKKS<TEST_CONFIG> {
public:
  void Lower_add(NODE_PTR n_add, NODE_PTR blk) {
    Ofile() << "PY Lower Before:" << std::endl;
    Ofile() << "====================================" << std::endl;
    Container()->Glob_scope()->Print(Ofile());

    FUNC_SCOPE* fs = Container()->Parent_func_scope();
    GLOB_SCOPE* new_glob =
        new GLOB_SCOPE(Container()->Glob_scope()->Id(), true);
    AIR_ASSERT(new_glob != nullptr);
    new_glob->Clone(*Container()->Glob_scope());
    FUNC_SCOPE&   new_fs   = new_glob->New_func_scope(fs->Id());
    CONTAINER*    new_cntr = &new_fs.Container();
    CKKS_CONFIG   config;
    SIHE2CKKS_CTX ctx(new_cntr, Fhe_ctx(), config);
    STMT_PTR      new_func_entry =
        new_cntr->New_func_entry(fs->Owning_func()->Spos());
    NODE_PTR new_blk = new_func_entry->Node()->Last_child();
    ctx.Push(blk, new_blk);
    CKKS_PY_IMPL py_handler(ctx);
    NODE_PTR     new_node = py_handler.New_py_add(
        n_add, new_cntr->Clone_node(n_add->Child(0)),
        new_cntr->Clone_node(n_add->Child(1)), n_add->Spos());
    ctx.End_stmt(new_node);
    ctx.Pop(blk, new_blk);

    Ofile() << "PY Lower After:" << std::endl;
    Ofile() << "====================================" << std::endl;
    new_cntr->Print(Ofile());
    Ofile() << "====================================" << std::endl;
  }

  void Lower_bts(NODE_PTR bts_node) {
    Ofile() << "PY Lower Before:" << std::endl;
    Ofile() << "====================================" << std::endl;
    Container()->Glob_scope()->Print(Ofile());

    CKKS_CONFIG   config;
    SIHE2CKKS_CTX ctx(Container(), Fhe_ctx(), config);
    CKKS_PY_IMPL  py_handler(ctx);
    FUNC_SCOPE*   fs = py_handler.New_py_bts(bts_node, bts_node->Spos());

    Ofile() << "PY Lower After:" << std::endl;
    Ofile() << "====================================" << std::endl;
    fs->Print(Ofile());
  }
};

TEST_P(TEST_CKKS_PY, Handle_bts) {
  GTEST_SKIP() << "Test skipped due to pydsl library not installed by default.";
  CKKS_IR_GEN& irgen = Ckks_ir_gen();
  NODE_PTR bts_node  = irgen.Gen_bootstrap_node(Container(), irgen.Input_var());
  uint32_t level_out = 0;  // full level after bts
  bts_node->Set_attr(fhe::core::FHE_ATTR_KIND::LEVEL, &level_out, 1);
  STMT_PTR bts_stmt =
      Container()->New_st(bts_node, irgen.Output_var(), irgen.Spos());
  Container()->Stmt_list().Append(bts_stmt);
  Lower_bts(bts_node);
}

TEST_P(TEST_CKKS_PY, Handle_add) {
  GTEST_SKIP() << "Test skipped due to pydsl library not installed by default.";
  CKKS_IR_GEN& irgen = Ckks_ir_gen();
  NODE_PTR     n_op0 = Container()->New_ld(irgen.Input_var(), irgen.Spos());
  NODE_PTR     n_op1 = Container()->New_ld(irgen.Input_var(), irgen.Spos());
  air::base::OPCODE add_op(fhe::sihe::SIHE_DOMAIN::ID,
                           fhe::sihe::SIHE_OPERATOR::ADD);
  NODE_PTR n_add = Container()->New_bin_arith(add_op, irgen.Ciph_ty(), n_op0,
                                              n_op1, irgen.Spos());
  STMT_PTR s_add = Container()->New_st(n_add, irgen.Output_var(), irgen.Spos());
  Container()->Stmt_list().Append(s_add);
  Lower_add(n_add, Container()->Entry_node()->Last_child());
}

// testing same tests with different config
INSTANTIATE_TEST_SUITE_P(BTSTests, TEST_CKKS_PY,
                         ::testing::Values(TEST_CONFIG(".bts.t",
                                                       fhe::poly::HPOLY)));

}  // namespace test
}  // namespace ckks
}  // namespace fhe
