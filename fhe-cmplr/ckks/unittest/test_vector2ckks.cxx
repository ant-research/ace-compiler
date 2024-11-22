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
#include "air/util/debug.h"
#include "fhe/ckks/ckks_gen.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/ckks/sihe2ckks_lower.h"
#include "fhe/core/ctx_param_ana.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/sihe/sihe_gen.h"
#include "fhe/sihe/vector2sihe_lower.h"
#include "gtest/gtest.h"
#include "nn/vector/vector_gen.h"
#include "nn/vector/vector_opcode.h"
#include "nn/vector/vector_utils.h"

using namespace air::base;
using namespace fhe::sihe;
using namespace fhe::ckks;
using namespace fhe::core;

class SIHE2CKKSTEST : public testing::Test {
public:
  FUNC_SCOPE*           Gen_uni_formal_func(const char* func_name);
  FUNC_SCOPE*           Gen_bin_formal_func(const char* func_name);
  fhe::core::LOWER_CTX& Lower_ctx() { return _lower_ctx; }
  uint32_t              Lower_vector_func(FUNC_SCOPE* vec_func);

protected:
  void SetUp() override {
    _glob_scope = GLOB_SCOPE::Get();
    Register_domains();
    Register_types();
    _array_type    = Gen_array_type({20});
    _array_2d_type = Gen_array_type({2, 4});
  }

  void TearDown() override {
    META_INFO::Remove_all();
    // delete _glob_scope;
  }

  void           Register_domains();
  void           Register_types();
  ARRAY_TYPE_PTR Gen_array_type(const std::vector<int64_t>& dim);

  GLOB_SCOPE*          _glob_scope;
  fhe::core::LOWER_CTX _lower_ctx;
  ARRAY_TYPE_PTR       _array_type;
  ARRAY_TYPE_PTR       _array_2d_type;
  char                 _array_cst_buf[64];
};

void SIHE2CKKSTEST::Register_domains() {
  META_INFO::Remove_all();
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

void SIHE2CKKSTEST::Register_types() {
  SIHE_GEN(_glob_scope, &_lower_ctx).Register_sihe_types();
  CKKS_GEN(_glob_scope, &_lower_ctx).Register_ckks_types();
}

ARRAY_TYPE_PTR SIHE2CKKSTEST::Gen_array_type(const std::vector<int64_t>& dim) {
  TYPE_PTR f32_type   = _glob_scope->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
  TYPE_PTR array_type = New_array_type(_glob_scope, "client_array", f32_type,
                                       dim, _glob_scope->Unknown_simple_spos());
  return array_type->Cast_to_arr();
}

FUNC_SCOPE* SIHE2CKKSTEST::Gen_bin_formal_func(const char* func_name) {
  STR_PTR name_ptr = _glob_scope->New_str(func_name);
  SPOS    spos     = _glob_scope->Unknown_simple_spos();

  // gen function
  FUNC_PTR    bin_formal_func = _glob_scope->New_func(name_ptr, spos);
  FUNC_SCOPE* func_scope      = &_glob_scope->New_func_scope(bin_formal_func);

  bin_formal_func->Set_parent(_glob_scope->Comp_env_id());
  // signature of function
  SIGNATURE_TYPE_PTR add_sig = _glob_scope->New_sig_type();
  // return type of function
  _glob_scope->New_ret_param(_array_type, add_sig);
  STR_PTR x_str = _glob_scope->New_str("x");
  _glob_scope->New_param(x_str, _array_type, add_sig, spos);
  STR_PTR y_str = _glob_scope->New_str("y");
  _glob_scope->New_param(y_str, _array_type, add_sig, spos);
  add_sig->Set_complete();
  _glob_scope->New_entry_point(add_sig, bin_formal_func, name_ptr, spos);

  return func_scope;
}

FUNC_SCOPE* SIHE2CKKSTEST::Gen_uni_formal_func(const char* func_name) {
  STR_PTR name_ptr = _glob_scope->New_str(func_name);
  SPOS    spos     = _glob_scope->Unknown_simple_spos();

  // gen function
  FUNC_PTR    uni_formal_func = _glob_scope->New_func(name_ptr, spos);
  FUNC_SCOPE* func_scope      = &_glob_scope->New_func_scope(uni_formal_func);

  uni_formal_func->Set_parent(_glob_scope->Comp_env_id());
  // signature of function
  SIGNATURE_TYPE_PTR add_sig = _glob_scope->New_sig_type();
  // return type of function
  _glob_scope->New_ret_param(_array_type, add_sig);
  STR_PTR x_str = _glob_scope->New_str("x");
  _glob_scope->New_param(x_str, _array_type, add_sig, spos);
  add_sig->Set_complete();
  _glob_scope->New_entry_point(add_sig, uni_formal_func, name_ptr, spos);

  return func_scope;
}

uint32_t SIHE2CKKSTEST::Lower_vector_func(FUNC_SCOPE* vec_func_scope) {
  std::cout << "vector func: " << std::endl;
  std::cout << vec_func_scope->To_str() << std::endl;

  fhe::sihe::SIHE_CONFIG sihe_gen_cfg;
  GLOB_SCOPE*            sihe_glob_scope =
      Sihe_driver(_glob_scope, &_lower_ctx, nullptr, sihe_gen_cfg);

  FUNC_SCOPE*                 sihe_func_scope = nullptr;
  GLOB_SCOPE::FUNC_SCOPE_ITER scope_iter = sihe_glob_scope->Begin_func_scope();
  GLOB_SCOPE::FUNC_SCOPE_ITER end_scope_iter =
      sihe_glob_scope->End_func_scope();
  for (; scope_iter != end_scope_iter; ++scope_iter) {
    FUNC_SCOPE& func_scope = *scope_iter;
    if (func_scope.Owning_func()->Name_id() !=
        vec_func_scope->Owning_func()->Name_id()) {
      continue;
    }
    sihe_func_scope = &func_scope;
  }
  AIR_ASSERT(sihe_func_scope != nullptr);
  std::cout << "sihe func: " << std::endl;
  std::cout << sihe_func_scope->To_str() << std::endl;

  air::driver::DRIVER_CTX driver_ctx;
  fhe::ckks::CKKS_CONFIG  ckks_cfg;
  GLOB_SCOPE*             ckks_glob_scope =
      Ckks_driver(sihe_glob_scope, &_lower_ctx, &driver_ctx, &ckks_cfg);
  std::cout << "ckks func: " << std::endl;
  FUNC_SCOPE& ckks_func_scope =
      ckks_glob_scope->Open_func_scope(sihe_func_scope->Owning_func_id());
  std::cout << ckks_func_scope.To_str() << std::endl;

  CTX_PARAM_ANA param_ana(&ckks_func_scope, &_lower_ctx, &driver_ctx,
                          &ckks_cfg);
  param_ana.Run();
  return param_ana.Get_ana_ctx().Get_mul_level();
}

TEST_F(SIHE2CKKSTEST, add_func) {
  FUNC_SCOPE* func_scope = Gen_bin_formal_func("add_func");
  FUNC_PTR    func       = func_scope->Owning_func();
  CONTAINER*  cntr       = &func_scope->Container();
  SPOS        spos       = _glob_scope->Unknown_simple_spos();
  cntr->New_func_entry(spos);

  STR_PTR        z_name = _glob_scope->New_str("z");
  ADDR_DATUM_PTR var_z  = func_scope->New_var(_array_type, z_name, spos);

  FORMAL_ITER formal_itr = func_scope->Begin_formal();
  // load frist formal x
  NODE_PTR load_x = cntr->New_ld(*formal_itr, spos);
  // load second formal y
  NODE_PTR load_y = cntr->New_ld(*(++formal_itr), spos);

  // x + y
  NODE_PTR add_cipher_node = VECTOR_GEN(cntr).New_add(load_x, load_y, spos);

  // (x + y) + cst_array
  CONSTANT_PTR cst_array = _glob_scope->New_const(
      CONSTANT_KIND::ARRAY, _array_type, _array_cst_buf, 80);
  NODE_PTR ld_cst_array = cntr->New_ldc(cst_array, spos);
  NODE_PTR add_plain_node =
      VECTOR_GEN(cntr).New_add(add_cipher_node, ld_cst_array, spos);

  // z = (x + y) + cst_array
  STMT_PTR  store_stmt = cntr->New_st(add_plain_node, var_z, spos);
  STMT_LIST sl         = cntr->Stmt_list();
  sl.Append(store_stmt);

  // ret (z);
  NODE_PTR load_z  = cntr->New_ld(var_z, spos);
  STMT_PTR add_ret = cntr->New_retv(load_z, spos);
  sl.Append(add_ret);

  uint32_t mul_level = Lower_vector_func(func_scope);
  ASSERT_EQ(mul_level, 1) << " mult level of add_func is 1";
}

TEST_F(SIHE2CKKSTEST, mul_func) {
  FUNC_SCOPE* func_scope = Gen_bin_formal_func("mul_func");
  FUNC_PTR    func       = func_scope->Owning_func();
  CONTAINER*  cntr       = &func_scope->Container();
  SPOS        spos       = _glob_scope->Unknown_simple_spos();
  cntr->New_func_entry(spos);

  STR_PTR        z_name = _glob_scope->New_str("z");
  ADDR_DATUM_PTR var_z  = func_scope->New_var(_array_type, z_name, spos);

  FORMAL_ITER formal_itr = func_scope->Begin_formal();
  // load frist formal x
  NODE_PTR load_x = cntr->New_ld(*formal_itr, spos);
  // load second formal y
  NODE_PTR load_y = cntr->New_ld(*(++formal_itr), spos);

  // x * y
  air::base::OPCODE mul_op(VECTOR_DOMAIN::ID, VECTOR_OPCODE::MUL);
  NODE_PTR mul_cipher_node = cntr->New_bin_arith(mul_op, load_x, load_y, spos);
  mul_cipher_node->Set_rtype(_array_type);

  // (x * y) * cst_array
  CONSTANT_PTR cst_array = _glob_scope->New_const(
      CONSTANT_KIND::ARRAY, _array_type, _array_cst_buf, 80);
  NODE_PTR ld_cst_array = cntr->New_ldc(cst_array, spos);
  NODE_PTR mul_plain_node =
      cntr->New_bin_arith(mul_op, mul_cipher_node, ld_cst_array, spos);
  mul_plain_node->Set_rtype(_array_type);

  // z = (x * y) * cst_array
  STMT_PTR  store_stmt = cntr->New_st(mul_plain_node, var_z, spos);
  STMT_LIST sl         = cntr->Stmt_list();
  sl.Append(store_stmt);

  // ret (z);
  NODE_PTR load_z = cntr->New_ld(var_z, spos);
  STMT_PTR ret_z  = cntr->New_retv(load_z, spos);
  sl.Append(ret_z);

  uint32_t mul_level = Lower_vector_func(func_scope);
  ASSERT_EQ(mul_level, 3) << " mult level of mul_func is 3";
}

TEST_F(SIHE2CKKSTEST, rotate_func) {
  FUNC_SCOPE* func_scope = Gen_uni_formal_func("rotate_func");
  FUNC_PTR    func       = func_scope->Owning_func();
  CONTAINER*  cntr       = &func_scope->Container();
  SPOS        spos       = _glob_scope->Unknown_simple_spos();
  cntr->New_func_entry(spos);

  STR_PTR        z_name = _glob_scope->New_str("z");
  ADDR_DATUM_PTR var_z  = func_scope->New_var(_array_type, z_name, spos);

  FORMAL_ITER formal_itr = func_scope->Begin_formal();
  // load frist formal x
  NODE_PTR load_x0 = cntr->New_ld(*formal_itr, spos);
  NODE_PTR load_x1 = cntr->New_ld(*formal_itr, spos);
  // x * x
  air::base::OPCODE mul_op(VECTOR_DOMAIN::ID, VECTOR_OPCODE::MUL);
  NODE_PTR mul_node = cntr->New_bin_arith(mul_op, load_x0, load_x1, spos);

  int      roll_nums = 2;
  TYPE_PTR s32_type  = _glob_scope->Prim_type(PRIMITIVE_TYPE::INT_S32);
  NODE_PTR ld_cst2   = cntr->New_intconst(s32_type, roll_nums, spos);
  // roll(x * x, 2)
  air::base::OPCODE roll_op(VECTOR_DOMAIN::ID, VECTOR_OPCODE::ROLL);
  NODE_PTR rotate_node = cntr->New_cust_node(roll_op, _array_type, spos);
  rotate_node->Set_child(0, mul_node);
  rotate_node->Set_child(1, ld_cst2);
  rotate_node->Set_attr("nums", &roll_nums, 1);

  // z = roll(x, 2)
  STMT_PTR  store_stmt = cntr->New_st(rotate_node, var_z, spos);
  STMT_LIST sl         = cntr->Stmt_list();
  sl.Append(store_stmt);

  // ret (z);
  NODE_PTR load_z  = cntr->New_ld(var_z, spos);
  STMT_PTR add_ret = cntr->New_retv(load_z, spos);
  sl.Append(add_ret);

  uint32_t mul_level = Lower_vector_func(func_scope);
  ASSERT_EQ(mul_level, 2) << " mult level of mul_func is 2";
}

// TODO: Temporarily close test_case which inc cipher mul_level in each loop
// iteration. We will reopen it after optimize CKKS_SCALE_MANAGER with SSA.
/*
TEST_F(SIHE2CKKSTEST, loop_func1) {
  FUNC_SCOPE* func_scope = Gen_uni_formal_func("loop_func1");
  FUNC_PTR    func       = func_scope->Owning_func();
  CONTAINER*  cntr       = &func_scope->Container();
  SPOS        spos       = _glob_scope->Unknown_simple_spos();
  cntr->New_func_entry(spos);

  ADDR_DATUM_PTR var_z = func_scope->New_var(_array_type, "z", spos);

  FORMAL_ITER formal_itr = func_scope->Begin_formal();
  // load frist formal x
  NODE_PTR load_x = cntr->New_ld(*formal_itr, spos);
  // z = x
  STMT_PTR  st_z = cntr->New_st(load_x, var_z, spos);
  STMT_LIST sl   = cntr->Stmt_list();
  sl.Append(st_z);

  NODE_PTR block_node = cntr->New_stmt_block(spos);

  TYPE_PTR s32_type = _glob_scope->Prim_type(PRIMITIVE_TYPE::INT_S32);
  // init value of iv
  NODE_PTR ld_cst0 = cntr->New_intconst(s32_type, 0, spos);
  // cmp opnd
  NODE_PTR          ld_cst4 = cntr->New_intconst(s32_type, 4, spos);
  ADDR_DATUM_PTR    var_iv  = func_scope->New_var(s32_type, "i", spos);
  NODE_PTR          ld_iv   = cntr->New_ld(var_iv, spos);
  air::base::OPCODE lt_op(air::core::CORE, air::core::OPCODE::LT);
  NODE_PTR          cmp_node = cntr->New_bin_arith(lt_op, ld_iv, ld_cst4, spos);
  NODE_PTR          ld_cst1  = cntr->New_intconst(s32_type, 1, spos);

  air::base::OPCODE add_op(air::core::CORE, air::core::OPCODE::ADD);
  ld_iv             = cntr->New_ld(var_iv, spos);
  NODE_PTR add_node = cntr->New_bin_arith(add_op, ld_iv, ld_cst1, spos);
  STMT_PTR do_loop =
      cntr->New_do_loop(var_iv, ld_cst0, cmp_node, add_node, block_node, spos);
  sl.Append(do_loop);

  STMT_LIST loop_sl(block_node);
  // z = z * z
  NODE_PTR          load_z = cntr->New_ld(var_z, spos);
  air::base::OPCODE mul_op(VECTOR_DOMAIN::ID, VECTOR_OPCODE::MUL);
  NODE_PTR mul_node = cntr->New_bin_arith(mul_op, load_z, load_z, spos);
  st_z              = cntr->New_st(mul_node, var_z, spos);
  loop_sl.Append(st_z);

  // z = rotate(z, iv)
  load_z = cntr->New_ld(var_z, spos);
  ld_iv  = cntr->New_ld(var_iv, spos);
  air::base::OPCODE rot_op(VECTOR_DOMAIN::ID, VECTOR_OPCODE::ROLL);
  NODE_PTR          rot_node = cntr->New_bin_arith(rot_op, load_z, ld_iv, spos);
  std::vector<int> roll_nums({0, 1, 2, 3});
  rot_node->Set_attr("nums", roll_nums.data(), roll_nums.size());
  st_z                       = cntr->New_st(rot_node, var_z, spos);
  loop_sl.Append(st_z);

  // ret (z);
  load_z           = cntr->New_ld(var_z, spos);
  STMT_PTR add_ret = cntr->New_retv(load_z, spos);
  sl.Append(add_ret);

  uint32_t mul_level = Lower_vector_func(func_scope);
  ASSERT_EQ(mul_level, 5) << " mult level of loop_func is 5";
} */

TEST_F(SIHE2CKKSTEST, loop_func2) {
  FUNC_SCOPE* func_scope = Gen_uni_formal_func("loop_func2");
  FUNC_PTR    func       = func_scope->Owning_func();
  CONTAINER*  cntr       = &func_scope->Container();
  SPOS        spos       = _glob_scope->Unknown_simple_spos();
  cntr->New_func_entry(spos);

  ADDR_DATUM_PTR var_z = func_scope->New_var(_array_type, "z", spos);

  FORMAL_ITER formal_itr = func_scope->Begin_formal();
  // load frist formal x
  NODE_PTR load_x = cntr->New_ld(*formal_itr, spos);
  // z = x
  STMT_PTR  st_z = cntr->New_st(load_x, var_z, spos);
  STMT_LIST sl   = cntr->Stmt_list();
  sl.Append(st_z);

  NODE_PTR block_node = cntr->New_stmt_block(spos);

  TYPE_PTR s32_type = _glob_scope->Prim_type(PRIMITIVE_TYPE::INT_S32);
  // init value of iv
  NODE_PTR ld_cst0 = cntr->New_intconst(s32_type, 0, spos);
  // cmp opnd
  NODE_PTR          ld_cst4 = cntr->New_intconst(s32_type, 4, spos);
  ADDR_DATUM_PTR    var_iv  = func_scope->New_var(s32_type, "i", spos);
  NODE_PTR          ld_iv   = cntr->New_ld(var_iv, spos);
  air::base::OPCODE lt_op(air::core::CORE, air::core::OPCODE::LE);
  NODE_PTR          cmp_node = cntr->New_bin_arith(lt_op, ld_iv, ld_cst4, spos);
  // incr opnd
  air::base::OPCODE sub_op(air::core::CORE, air::core::OPCODE::SUB);
  NODE_PTR          ld_cst_m1 = cntr->New_intconst(s32_type, -1, spos);
  ld_iv                       = cntr->New_ld(var_iv, spos);
  NODE_PTR sub_node = cntr->New_bin_arith(sub_op, ld_iv, ld_cst_m1, spos);
  STMT_PTR do_loop =
      cntr->New_do_loop(var_iv, ld_cst0, cmp_node, sub_node, block_node, spos);
  sl.Append(do_loop);

  VECTOR_GEN vec_gen(cntr);
  STMT_LIST  loop_sl(block_node);
  // x * x
  NODE_PTR load_x0      = cntr->New_ld(*formal_itr, spos);
  NODE_PTR load_x1      = cntr->New_ld(*formal_itr, spos);
  NODE_PTR vec_mul_node = vec_gen.New_mul(load_x0, load_x1, spos);

  // z = z + rotate(x * x, (2 << iv) * 4)
  ld_iv                     = cntr->New_ld(var_iv, spos);
  NODE_PTR          ld_cst2 = cntr->New_intconst(s32_type, 2, spos);
  air::base::OPCODE shl_op(air::core::CORE, air::core::OPCODE::SHL);
  NODE_PTR          shl_iv = cntr->New_bin_arith(shl_op, ld_cst2, ld_iv, spos);
  ld_cst4                  = cntr->New_intconst(s32_type, 4, spos);
  air::base::OPCODE mul_op(air::core::CORE, air::core::OPCODE::MUL);
  NODE_PTR base_mul_node = cntr->New_bin_arith(mul_op, shl_iv, ld_cst4, spos);

  NODE_PTR rot_node = vec_gen.New_roll(vec_mul_node, base_mul_node, spos);
  std::vector<int> roll_nums({8, 16, 32, 64});
  rot_node->Set_attr("nums", roll_nums.data(), roll_nums.size());

  NODE_PTR load_z       = cntr->New_ld(var_z, spos);
  NODE_PTR vec_add_node = vec_gen.New_add(load_z, rot_node, spos);
  st_z                  = cntr->New_st(vec_add_node, var_z, spos);
  loop_sl.Append(st_z);

  // ret (z);
  load_z           = cntr->New_ld(var_z, spos);
  STMT_PTR add_ret = cntr->New_retv(load_z, spos);
  sl.Append(add_ret);

  uint32_t mul_level = Lower_vector_func(func_scope);
  ASSERT_EQ(mul_level, 2) << " mult level of loop_func is 2";
}

TEST_F(SIHE2CKKSTEST, slice_func) {
  FUNC_SCOPE* func_scope = Gen_uni_formal_func("slice_func");
  FUNC_PTR    func       = func_scope->Owning_func();
  CONTAINER*  cntr       = &func_scope->Container();
  SPOS        spos       = _glob_scope->Unknown_simple_spos();
  cntr->New_func_entry(spos);

  // slice(cst_array, 2, 4)
  CONSTANT_PTR cst_array = _glob_scope->New_const(
      CONSTANT_KIND::ARRAY, _array_2d_type, _array_cst_buf, 32);
  NODE_PTR ld_cst_array = cntr->New_ldc(cst_array, spos);
  TYPE_PTR int32_type   = _glob_scope->Prim_type(PRIMITIVE_TYPE::INT_S32);
  NODE_PTR intcst_2     = cntr->New_intconst(int32_type, 2, spos);
  NODE_PTR intcst_4     = cntr->New_intconst(int32_type, 4, spos);
  NODE_PTR slice =
      VECTOR_GEN(cntr).New_slice(ld_cst_array, intcst_2, intcst_4, spos);

  FORMAL_ITER formal_itr = func_scope->Begin_formal();
  // load frist formal x
  NODE_PTR load_x = cntr->New_ld(*formal_itr, spos);

  // x + slice(cst_array, 2, 4)
  NODE_PTR add_slice_node = VECTOR_GEN(cntr).New_add(load_x, slice, spos);

  STR_PTR        z_name = _glob_scope->New_str("z");
  ADDR_DATUM_PTR var_z  = func_scope->New_var(_array_2d_type, z_name, spos);
  // z = x + slice(cst_array, 2, 4)
  STMT_PTR  store_stmt = cntr->New_st(add_slice_node, var_z, spos);
  STMT_LIST sl         = cntr->Stmt_list();
  sl.Append(store_stmt);

  // ret (z);
  NODE_PTR load_z  = cntr->New_ld(var_z, spos);
  STMT_PTR add_ret = cntr->New_retv(load_z, spos);
  sl.Append(add_ret);

  uint32_t mul_level = Lower_vector_func(func_scope);
  ASSERT_EQ(mul_level, 1) << " mult level of loop_func is 1";
}

TEST_F(SIHE2CKKSTEST, relu_func) {
  FUNC_SCOPE* func_scope = Gen_uni_formal_func("relu_func");
  FUNC_PTR    func       = func_scope->Owning_func();
  CONTAINER*  cntr       = &func_scope->Container();
  SPOS        spos       = _glob_scope->Unknown_simple_spos();
  cntr->New_func_entry(spos);

  TYPE_PTR       f32_type = _glob_scope->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
  ADDR_DATUM_PTR var_z    = func_scope->New_var(_array_type, "z", spos);

  // load x
  ADDR_DATUM_PTR formal_x = *func_scope->Begin_formal();
  NODE_PTR       load_x   = cntr->New_ld(formal_x, spos);

  // relu(x)
  air::base::OPCODE relu_opcode(nn::core::NN, nn::core::OPCODE::RELU);
  NODE_PTR relu_node = cntr->New_cust_node(relu_opcode, load_x->Rtype(), spos);
  relu_node->Set_child(0, load_x);

  // z = relu(x)
  STMT_PTR  store_stmt = cntr->New_st(relu_node, var_z, spos);
  STMT_LIST sl         = cntr->Stmt_list();
  sl.Append(store_stmt);

  // ret (z);
  NODE_PTR load_z  = cntr->New_ld(var_z, spos);
  STMT_PTR add_ret = cntr->New_retv(load_z, spos);
  sl.Append(add_ret);

  std::cout << "vector relu_func: " << std::endl;
  std::cout << func_scope->To_str() << std::endl;
  uint32_t mul_level = Lower_vector_func(func_scope);

  uint32_t relu_mul_level =
      Lower_ctx().Get_approx_relu_func_info().Get_mul_depth();
  uint32_t func_mul_level =
      relu_mul_level + Lower_ctx().Get_ctx_param().Mul_depth_of_bootstrap() + 1;
  ASSERT_EQ(mul_level, func_mul_level)
      << " mult level of loop_func is " << func_mul_level;
}
