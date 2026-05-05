//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================
#include <iostream>
#include <memory>
#include <ostream>
#include <tuple>
#include <type_traits>

#include "air/base/container.h"
#include "air/base/container_decl.h"
#include "air/base/meta_info.h"
#include "air/base/node.h"
#include "air/base/opcode.h"
#include "air/base/st.h"
#include "air/base/st_decl.h"
#include "air/base/st_enum.h"
#include "air/base/visitor.h"
#include "air/core/opcode.h"
#include "air/driver/driver_ctx.h"
#include "air/opt/dfg_data.h"
#include "air/util/debug.h"
#include "dfg_region.h"
#include "dfg_region_builder.h"
#include "dfg_region_container.h"
#include "fhe/ckks/ckks2c_mfree.h"
#include "fhe/ckks/ckks_gen.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/ckks/config.h"
#include "fhe/ckks/sihe2ckks_lower.h"
#include "fhe/core/ctx_param_ana.h"
#include "fhe/core/ir2c_ctx.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/poly/opcode.h"
#include "fhe/sihe/sihe_gen.h"
#include "fhe/sihe/sihe_opcode.h"
#include "fhe/sihe/vector2sihe_lower.h"
#include "gtest/gtest.h"
#include "nn/vector/vector_gen.h"
#include "nn/vector/vector_opcode.h"
#include "nn/vector/vector_utils.h"

using namespace air::base;
using namespace nn::vector;
using namespace fhe::core;
using namespace fhe::sihe;
using namespace fhe::ckks;

class TEST_CKKS_FREE_CIPH : public testing::Test {
protected:
  void SetUp() override {
    META_INFO::Remove_all();
    air::core::Register_core();
    fhe::ckks::Register_ckks_domain();
    _glob = new GLOB_SCOPE(0, true);
    _spos = _glob->Unknown_simple_spos();
    SIHE_GEN(_glob, &_ctx).Register_sihe_types();
    CKKS_GEN(_glob, &_ctx).Register_ckks_types();
  }
  void Register_domains() {
    air::core::Register_core();
    bool reg_nn = nn::core::Register_nn();
    ASSERT_TRUE(reg_nn);

    bool reg_vec = Register_vector_domain();
    ASSERT_TRUE(reg_vec) << "failed register vector domain";

    bool reg_sihe = Register_sihe_domain();
    ASSERT_TRUE(reg_sihe) << "failed register sihe domain";

    bool reg_ckks = Register_ckks_domain();
    ASSERT_TRUE(reg_ckks) << "failed register ckks domain";
  }

  void Run_test_ckks_free_ciph();

private:
  GLOB_SCOPE* Glob(void) const { return _glob; }
  const SPOS& Spos(void) const { return _spos; }
  LOWER_CTX*  Ctx(void) { return &_ctx; }

  GLOB_SCOPE* _glob;
  LOWER_CTX   _ctx;
  SPOS        _spos;
};

void TEST_CKKS_FREE_CIPH ::Run_test_ckks_free_ciph() {
  // CIPHER Main_graph() {
  //   CIPHER x
  //   x1 = MulCP(x, 2.);
  //   x2 = MulCP(x, 2.);
  //   x3 = Rotate(x2, 1);
  //   x4 = AddCC(x1, x3);
  //   x5 = MulCP(x2, 2.);
  //   x6 = Rotate(x5, 1);
  //   x7 = AddCC(x6, x4)
  //   return x7;
  //}
  const char* func_name   = "Main_graph";
  FUNC_PTR    func        = Glob()->New_func(func_name, Spos());
  FUNC_SCOPE* fs          = &Glob()->New_func_scope(func);
  CONTAINER*  cntr        = &fs->Container();
  TYPE_PTR    cipher_type = Ctx()->Get_cipher_type(Glob());

  // 1. gen signature type
  SIGNATURE_TYPE_PTR sig_type = Glob()->New_sig_type();
  Glob()->New_ret_param(cipher_type->Id(), sig_type->Id());
  sig_type->Set_complete();
  Glob()->New_entry_point(sig_type, func, func_name, Spos());

  // 2. gen IR
  // gen entry stmt
  STMT_PTR entry = cntr->New_func_entry(Spos(), 0);
  func->Entry_point()->Set_program_entry();

  STMT_LIST    sl  = cntr->Stmt_list();
  TYPE_PTR     f64 = Glob()->Prim_type(PRIMITIVE_TYPE::FLOAT_64);
  CONSTANT_PTR cst1 =

      Glob()->New_const(CONSTANT_KIND::FLOAT, f64, (long double)(2.));
  TYPE_PTR u32 = Glob()->Prim_type(PRIMITIVE_TYPE::INT_U32);

  ADDR_DATUM_PTR x = fs->New_var(cipher_type, "x", Spos());

  // x1 = MulCP(x, 2.);
  NODE_PTR ld_x     = cntr->New_ld(x, Spos());
  NODE_PTR ld_cst   = cntr->New_ldc(cst1, Spos());
  NODE_PTR mulcp    = cntr->New_bin_arith(fhe::ckks::OPC_MUL, cipher_type, ld_x,
                                          ld_cst, Spos());
  ADDR_DATUM_PTR x1 = fs->New_var(cipher_type, "x1", Spos());
  STMT_PTR       st_x1 = cntr->New_st(mulcp, x1, Spos());
  sl.Append(st_x1);

  // x2 = MulCP(x, 2.);
  ld_x   = cntr->New_ld(x, Spos());
  ld_cst = cntr->New_ldc(cst1, Spos());
  mulcp  = cntr->New_bin_arith(fhe::ckks::OPC_MUL, cipher_type, ld_x, ld_cst,
                               Spos());
  ADDR_DATUM_PTR x2    = fs->New_var(cipher_type, "x2", Spos());
  STMT_PTR       st_x2 = cntr->New_st(mulcp, x2, Spos());
  sl.Append(st_x2);

  // x3 = Rotate(x2, 1);
  NODE_PTR ld_x2       = cntr->New_ld(x2, Spos());
  NODE_PTR intcst_1    = cntr->New_intconst(u32, (uint64_t)1, Spos());
  NODE_PTR rot_x2_1    = cntr->New_bin_arith(fhe::ckks::OPC_ROTATE, cipher_type,
                                             ld_x2, intcst_1, Spos());
  ADDR_DATUM_PTR x3    = fs->New_var(cipher_type, "x3", Spos());
  STMT_PTR       st_x3 = cntr->New_st(rot_x2_1, x3, Spos());
  sl.Append(st_x3);

  // x4 = AddCC(x1, x3);
  NODE_PTR ld_x1 = cntr->New_ld(x1, Spos());
  NODE_PTR ld_x3 = cntr->New_ld(x3, Spos());
  NODE_PTR addcc = cntr->New_bin_arith(fhe::ckks::OPC_ADD, cipher_type, ld_x1,
                                       ld_x3, Spos());
  ADDR_DATUM_PTR x4    = fs->New_var(cipher_type, "x4", Spos());
  STMT_PTR       st_x4 = cntr->New_st(addcc, x4, Spos());
  sl.Append(st_x4);

  // x5 = MulCP(x2, 2.);
  ld_x2  = cntr->New_ld(x2, Spos());
  ld_cst = cntr->New_ldc(cst1, Spos());
  mulcp  = cntr->New_bin_arith(fhe::ckks::OPC_MUL, cipher_type, ld_x2, ld_cst,
                               Spos());
  ADDR_DATUM_PTR x5    = fs->New_var(cipher_type, "x5", Spos());
  STMT_PTR       st_x5 = cntr->New_st(mulcp, x5, Spos());
  sl.Append(st_x5);

  // x6 = Rotate(x5, 1);
  NODE_PTR ld_x5       = cntr->New_ld(x5, Spos());
  intcst_1             = cntr->New_intconst(u32, (uint64_t)1, Spos());
  NODE_PTR rot_x5_1    = cntr->New_bin_arith(fhe::ckks::OPC_ROTATE, cipher_type,
                                             ld_x5, intcst_1, Spos());
  ADDR_DATUM_PTR x6    = fs->New_var(cipher_type, "x6", Spos());
  STMT_PTR       st_x6 = cntr->New_st(rot_x5_1, x6, Spos());
  sl.Append(st_x6);

  // x7 = AddCC(x6, x4)
  NODE_PTR ld_x6 = cntr->New_ld(x6, Spos());
  NODE_PTR ld_x4 = cntr->New_ld(x4, Spos());
  addcc = cntr->New_bin_arith(fhe::ckks::OPC_ADD, cipher_type, ld_x6, ld_x4,
                              Spos());
  ADDR_DATUM_PTR x7    = fs->New_var(cipher_type, "x7", Spos());
  STMT_PTR       st_x7 = cntr->New_st(addcc, x7, Spos());
  sl.Append(st_x7);

  // return x7;
  NODE_PTR ld_x7 = cntr->New_ld(x7, Spos());
  STMT_PTR retv  = cntr->New_retv(ld_x7, Spos());
  sl.Append(retv);
  std::cout << "Before insert mfree: \n";
  std::cout << sl.To_str() << std::endl;
  air::base::NODE_PTR   body = fs->Container().Stmt_list().Block_node();
  fhe::ckks::MFREE_PASS mfree(_ctx);
  mfree.Perform(body);
  std::cout << "After insert mfree: \n";
  std::cout << sl.To_str() << std::endl;
  int free_ciph_count = 0;
  for (auto stmt = sl.Begin_stmt(); stmt != sl.End_stmt();
       stmt      = stmt->Next()) {
    if (stmt->Opcode() == fhe::ckks::OPC_FREE) {
      free_ciph_count++;
    }
  }
  std::string res = sl.To_str();

  std::string right_res =
      "st \"x1\" VAR[0x10000001] ID(0x6) LINE(0:0:0)\n"
      "  CKKS.mul RTYPE[0x12](CIPHERTEXT)\n"
      "    ld \"x\" VAR[0x10000000] RTYPE[0x12](CIPHERTEXT)\n"
      "    ldc #2 RTYPE[0x9](float64_t)\n"
      "st \"x2\" VAR[0x10000002] ID(0xa) LINE(0:0:0)\n"
      "  CKKS.mul RTYPE[0x12](CIPHERTEXT)\n"
      "    ld \"x\" VAR[0x10000000] RTYPE[0x12](CIPHERTEXT)\n"
      "    ldc #2 RTYPE[0x9](float64_t)\n"
      "CKKS.free ID(0x2e) LINE(0:0:0)\n"
      "  ld \"x\" VAR[0x10000000] RTYPE[0x12](CIPHERTEXT)\n"
      "st \"x3\" VAR[0x10000003] ID(0xe) LINE(0:0:0)\n"
      "  CKKS.rotate RTYPE[0x12](CIPHERTEXT)\n"
      "    ld \"x2\" VAR[0x10000002] RTYPE[0x12](CIPHERTEXT)\n"
      "    intconst #0x1 RTYPE[0x6](uint32_t)\n"
      "st \"x4\" VAR[0x10000004] ID(0x12) LINE(0:0:0)\n"
      "  CKKS.add RTYPE[0x12](CIPHERTEXT)\n"
      "    ld \"x1\" VAR[0x10000001] RTYPE[0x12](CIPHERTEXT)\n"
      "    ld \"x3\" VAR[0x10000003] RTYPE[0x12](CIPHERTEXT)\n"
      "CKKS.free ID(0x2c) LINE(0:0:0)\n"
      "  ld \"x3\" VAR[0x10000003] RTYPE[0x12](CIPHERTEXT)\n"
      "CKKS.free ID(0x2a) LINE(0:0:0)\n"
      "  ld \"x1\" VAR[0x10000001] RTYPE[0x12](CIPHERTEXT)\n"
      "st \"x5\" VAR[0x10000005] ID(0x16) LINE(0:0:0)\n"
      "  CKKS.mul RTYPE[0x12](CIPHERTEXT)\n"
      "    ld \"x2\" VAR[0x10000002] RTYPE[0x12](CIPHERTEXT)\n"
      "    ldc #2 RTYPE[0x9](float64_t)\n"
      "CKKS.free ID(0x28) LINE(0:0:0)\n"
      "  ld \"x2\" VAR[0x10000002] RTYPE[0x12](CIPHERTEXT)\n"
      "st \"x6\" VAR[0x10000006] ID(0x1a) LINE(0:0:0)\n"
      "  CKKS.rotate RTYPE[0x12](CIPHERTEXT)\n"
      "    ld \"x5\" VAR[0x10000005] RTYPE[0x12](CIPHERTEXT)\n"
      "    intconst #0x1 RTYPE[0x6](uint32_t)\n"
      "CKKS.free ID(0x26) LINE(0:0:0)\n"
      "  ld \"x5\" VAR[0x10000005] RTYPE[0x12](CIPHERTEXT)\n"
      "st \"x7\" VAR[0x10000007] ID(0x1e) LINE(0:0:0)\n"
      "  CKKS.add RTYPE[0x12](CIPHERTEXT)\n"
      "    ld \"x6\" VAR[0x10000006] RTYPE[0x12](CIPHERTEXT)\n"
      "    ld \"x4\" VAR[0x10000004] RTYPE[0x12](CIPHERTEXT)\n"
      "CKKS.free ID(0x24) LINE(0:0:0)\n"
      "  ld \"x4\" VAR[0x10000004] RTYPE[0x12](CIPHERTEXT)\n"
      "CKKS.free ID(0x22) LINE(0:0:0)\n"
      "  ld \"x6\" VAR[0x10000006] RTYPE[0x12](CIPHERTEXT)\n"
      "retv ID(0x20) LINE(0:0:0)\n"
      "  ld \"x7\" VAR[0x10000007] RTYPE[0x12](CIPHERTEXT)\n";

  // Check the number of free ciphers [x,x3,x1,x2,x5,x4,x6]
  EXPECT_EQ(free_ciph_count, 7) << "free ciph count is not correct";
  // Check the result
  EXPECT_EQ(res, right_res) << "result is not correct";
}

TEST_F(TEST_CKKS_FREE_CIPH, Test_ckks_free_ciph) { Run_test_ckks_free_ciph(); }