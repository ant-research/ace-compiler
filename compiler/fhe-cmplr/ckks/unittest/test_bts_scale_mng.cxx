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
#include "fhe/ckks/ckks_gen.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/ckks/config.h"
#include "fhe/ckks/sihe2ckks_lower.h"
#include "fhe/core/ctx_param_ana.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/poly/opcode.h"
#include "fhe/sihe/sihe_gen.h"
#include "fhe/sihe/sihe_opcode.h"
#include "fhe/sihe/vector2sihe_lower.h"
#include "gtest/gtest.h"
#include "nn/vector/vector_gen.h"
#include "nn/vector/vector_opcode.h"
#include "nn/vector/vector_utils.h"
#include "resbm.h"
#include "resbm_ctx.h"

using namespace air::base;
using namespace nn::vector;
using namespace fhe::core;
using namespace fhe::sihe;
using namespace fhe::ckks;

class TEST_BTS_SCALE_MNG : public testing::Test {
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

  //! @brief test the case that the src node of a region is not CKKS.mul, as
  //! follows:
  //! x -> MulCP -> MulCP -> Rot -> AddCC -> retv
  //! |      |                        ^
  //! |     Rot-->Add-----------------|
  //! |            ^
  //! ---> MulCP --|
  void Run_test_region_builder();

private:
  GLOB_SCOPE* Glob(void) const { return _glob; }
  const SPOS& Spos(void) const { return _spos; }
  LOWER_CTX*  Ctx(void) { return &_ctx; }

  GLOB_SCOPE* _glob;
  LOWER_CTX   _ctx;
  SPOS        _spos;
};

void TEST_BTS_SCALE_MNG::Run_test_region_builder() {
  // CIPHER Main_graph(CIPHER x) {
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
  Glob()->New_param("x", cipher_type, sig_type, Spos());
  Glob()->New_ret_param(cipher_type->Id(), sig_type->Id());
  sig_type->Set_complete();
  Glob()->New_entry_point(sig_type, func, func_name, Spos());

  // 2. gen IR
  // gen entry stmt
  STMT_PTR entry = cntr->New_func_entry(Spos(), 1);
  func->Entry_point()->Set_program_entry();
  ADDR_DATUM_PTR formal = fs->New_formal(cipher_type->Id(), "x", Spos());
  NODE_PTR       idname = cntr->New_idname(formal, Spos());
  entry->Node()->Set_child(0, idname);

  STMT_LIST    sl  = cntr->Stmt_list();
  TYPE_PTR     f64 = Glob()->Prim_type(PRIMITIVE_TYPE::FLOAT_64);
  CONSTANT_PTR cst1 =
      Glob()->New_const(CONSTANT_KIND::FLOAT, f64, (long double)(2.));
  TYPE_PTR u32 = Glob()->Prim_type(PRIMITIVE_TYPE::INT_U32);

  // x1 = MulCP(x, 2.);
  NODE_PTR ld_x     = cntr->New_ld(formal, Spos());
  NODE_PTR ld_cst   = cntr->New_ldc(cst1, Spos());
  NODE_PTR mulcp    = cntr->New_bin_arith(fhe::ckks::OPC_MUL, cipher_type, ld_x,
                                          ld_cst, Spos());
  ADDR_DATUM_PTR x1 = fs->New_var(cipher_type, "x1", Spos());
  STMT_PTR       st_x1 = cntr->New_st(mulcp, x1, Spos());
  sl.Append(st_x1);

  // x2 = MulCP(x, 2.);
  ld_x   = cntr->New_ld(formal, Spos());
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
  func->Print();

  air::driver::DRIVER_CTX driver_ctx;
  REGION_CONTAINER        region_cntr(Glob(), Ctx());
  REGION_BUILDER          region_builder(&region_cntr, &driver_ctx);
  R_CODE                  ret = region_builder.Perform();
  ASSERT_EQ(ret, R_CODE::NORMAL);
  // check count of region
  ASSERT_EQ(region_cntr.Region_cnt(), 4);
  // check region_2 contain two MulCP
  REGION_PTR reg2    = region_cntr.Region(REGION_ID(2));
  uint32_t   mul_cnt = 0;
  for (REGION::ELEM_ITER iter = reg2->Begin_elem(); iter != reg2->End_elem();
       ++iter) {
    air::opt::DFG_NODE_PTR dfg_node = iter->Dfg_node();
    if (!dfg_node->Is_node()) continue;
    if (dfg_node->Node()->Opcode() != fhe::ckks::OPC_MUL) continue;
    ++mul_cnt;
  }
  ASSERT_EQ(mul_cnt, 2);
  // check region_3 contain one MulCP
  REGION_PTR reg3 = region_cntr.Region(REGION_ID(3));
  mul_cnt         = 0;
  for (REGION::ELEM_ITER iter = reg3->Begin_elem(); iter != reg3->End_elem();
       ++iter) {
    air::opt::DFG_NODE_PTR dfg_node = iter->Dfg_node();
    if (!dfg_node->Is_node()) continue;
    if (dfg_node->Node()->Opcode() != fhe::ckks::OPC_MUL) continue;
    ++mul_cnt;
  }
  ASSERT_EQ(mul_cnt, 1);

  CKKS_CONFIG config;
  config._rgn_scl_bts_mng = true;
  RESBM resbm(&driver_ctx, &config, Glob(), Ctx());
  resbm.Perform();
  MIN_LATENCY_PLAN* plan = ((const RESBM&)resbm).Context()->Plan(REGION_ID(3));
  uint32_t          consume_lev = plan->Used_level();
  ASSERT_EQ(consume_lev, 2);
}

TEST_F(TEST_BTS_SCALE_MNG, Test_region_non_mul_op_src) {
  Run_test_region_builder();
}