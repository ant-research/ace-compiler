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
#include "ckks2poly.h"
#include "fhe/ckks/ckks_gen.h"
#include "fhe/sihe/sihe_gen.h"
#include "fhe/util/util.h"
#include "gtest/gtest.h"

using namespace air::base;
using namespace air::util;
using namespace fhe::ckks;
using namespace fhe::poly;

namespace {

class TEST_CKKS2POLY : public ::testing::Test {
protected:
  void SetUp() override { _cntr = Create_ckks_ir(_fhe_ctx); }

  void TearDown() {
    META_INFO::Remove_all();
    free(_visitor);
    free(_ctx);
  }

  CKKS2POLY_VISITOR* New_visitor() {
    FUNC_SCOPE* fscope = _cntr->Parent_func_scope();

    // clone old scope
    GLOB_SCOPE* old_scope = _cntr->Glob_scope();
    GLOB_SCOPE* new_glob  = new GLOB_SCOPE(1, true);
    new_glob->Clone(*old_scope);
    FUNC_SCOPE* new_fscope = &new_glob->New_func_scope(fscope->Owning_func());
    new_fscope->Clone(*fscope);
    _new_cntr = &(new_fscope->Container());

    _ctx     = new CKKS2POLY_CTX(_poly_config, &_fhe_ctx, _new_cntr);
    _visitor = new CKKS2POLY_VISITOR(*_ctx);
    return _visitor;
  }

  SPOS           Spos() { return GLOB_SCOPE::Get()->Unknown_simple_spos(); }
  CKKS2POLY_CTX* Ctx() { return _ctx; }
  POLY_IR_GEN&   Poly_gen() { return _ctx->Poly_gen(); }
  TRANSFORM_CTX* Trav_ctx() { return _trav_ctx; }
  CONTAINER*     Container() { return _cntr; }
  CONTAINER*     New_container() { return _new_cntr; }
  CONTAINER*     Create_ckks_ir(fhe::core::LOWER_CTX& lower_ctx);

  ADDR_DATUM_PTR Var_x() { return _var_x; }
  ADDR_DATUM_PTR Var_y() { return _var_y; }
  ADDR_DATUM_PTR Var_z() { return _var_z; }
  ADDR_DATUM_PTR Var_p() { return _var_p; }
  ADDR_DATUM_PTR Var_ciph3() { return _var_ciph3; }
  TYPE_PTR       Ciph_ty() { return _ciph_ty; }
  TYPE_PTR       Ciph3_ty() { return _ciph3_ty; }
  TYPE_PTR       Plain_ty() { return _plain_ty; }

  STMT_PTR Gen_add(CONTAINER* cntr, ADDR_DATUM_PTR var_z, ADDR_DATUM_PTR var_x,
                   ADDR_DATUM_PTR var_y);
  STMT_PTR Gen_mul(CONTAINER* cntr, ADDR_DATUM_PTR var_z, ADDR_DATUM_PTR var_x,
                   ADDR_DATUM_PTR var_y);
  STMT_PTR Gen_mul(CONTAINER* cntr, PREG_PTR preg_z, ADDR_DATUM_PTR var_x,
                   ADDR_DATUM_PTR var_y);
  STMT_PTR Gen_rotate(CONTAINER* cntr, ADDR_DATUM_PTR var_z,
                      ADDR_DATUM_PTR var_x, int32_t rot_idx);
  STMT_PTR Gen_rotate(CONTAINER* cntr, PREG_PTR var_z, PREG_PTR var_x,
                      int32_t rot_idx);
  STMT_PTR Gen_rescale(CONTAINER* cntr, ADDR_DATUM_PTR var_z,
                       ADDR_DATUM_PTR var_x);
  STMT_PTR Gen_relin(CONTAINER* cntr, ADDR_DATUM_PTR var_z,
                     ADDR_DATUM_PTR var_x);
  STMT_PTR Gen_relin(CONTAINER* cntr, PREG_PTR preg_z, ADDR_DATUM_PTR var_x);
  STMT_PTR Gen_mul_float(CONTAINER* cntr, ADDR_DATUM_PTR var_z,
                         ADDR_DATUM_PTR var_x, long double f);
  STMT_PTR Gen_encode(CONTAINER* cntr, ADDR_DATUM_PTR var_p);
  STMT_PTR Gen_bootstrap(CONTAINER* cntr, ADDR_DATUM_PTR var_x);
  STMT_PTR Gen_ret(CONTAINER* cntr, ADDR_DATUM_PTR var_z);

private:
  fhe::poly::POLY_CONFIG _poly_config;
  fhe::core::LOWER_CTX   _fhe_ctx;
  CKKS2POLY_CTX*         _ctx;
  CKKS2POLY_VISITOR*     _visitor;
  TRANSFORM_CTX*         _trav_ctx;
  CONTAINER*             _cntr;
  CONTAINER*             _new_cntr;

  ADDR_DATUM_PTR _var_x;
  ADDR_DATUM_PTR _var_y;
  ADDR_DATUM_PTR _var_z;
  ADDR_DATUM_PTR _var_p;
  ADDR_DATUM_PTR _var_ciph3;
  TYPE_PTR       _ciph_ty;
  TYPE_PTR       _ciph3_ty;
  TYPE_PTR       _plain_ty;
};

CONTAINER* TEST_CKKS2POLY::Create_ckks_ir(fhe::core::LOWER_CTX& lower_ctx) {
  bool ret = air::core::Register_core();
  CMPLR_ASSERT(ret, "core register failed");
  ret = fhe::ckks::Register_ckks_domain();
  CMPLR_ASSERT(ret, "ckks register failed");
  ret = Register_polynomial();
  CMPLR_ASSERT(ret, "polynomial register failed");

  GLOB_SCOPE* glob = GLOB_SCOPE::Get();
  fhe::sihe::SIHE_GEN(glob, &lower_ctx).Register_sihe_types();
  fhe::ckks::CKKS_GEN(glob, &lower_ctx).Register_ckks_types();

  // Ciphertext x, y, z; Plaintext p;
  _ciph_ty  = glob->Type(lower_ctx.Get_cipher_type_id());
  _plain_ty = glob->Type(lower_ctx.Get_plain_type_id());
  _ciph3_ty = glob->Type(lower_ctx.Get_cipher3_type_id());

  // name of main function
  STR_PTR name_str = glob->New_str("main");
  // main function
  FUNC_PTR main_func = glob->New_func(name_str, Spos());
  main_func->Set_parent(glob->Comp_env_id());
  // signature of main function
  SIGNATURE_TYPE_PTR sig = glob->New_sig_type();
  // return type of main function
  TYPE_PTR main_rtype = glob->Prim_type(PRIMITIVE_TYPE::VOID);
  glob->New_ret_param(main_rtype, sig);
  // parameter argc of function main
  TYPE_PTR argc_type = glob->Prim_type(PRIMITIVE_TYPE::INT_S32);
  STR_PTR  x_str     = glob->New_str("ciph_x");
  glob->New_param(x_str, _ciph_ty, sig, Spos());
  // parameter argv of function main, no pointer type yet
  STR_PTR y_str = glob->New_str("ciph_y");
  glob->New_param(y_str, _ciph_ty, sig, Spos());
  sig->Set_complete();
  // global entry for main
  ENTRY_PTR entry =
      glob->New_global_entry_point(sig, main_func, name_str, Spos());
  // set define before create a new scope
  FUNC_SCOPE* main_scope = &glob->New_func_scope(main_func);
  CONTAINER*  cntr       = &main_scope->Container();
  STMT_PTR    entry_stmt = cntr->New_func_entry(Spos());

  _var_x = main_scope->Formal(0);
  _var_y = main_scope->Formal(1);

  STR_PTR p_str = glob->New_str("plain");
  _var_p        = main_scope->New_var(_plain_ty, p_str, Spos());

  STR_PTR z1_str = glob->New_str("ciph_z");
  _var_z         = main_scope->New_var(_ciph_ty, z1_str, Spos());

  STR_PTR ciph3_str = glob->New_str("ciph3");
  _var_ciph3        = main_scope->New_var(_ciph3_ty, ciph3_str, Spos());
  return cntr;
}

// z = x + y
STMT_PTR TEST_CKKS2POLY::Gen_add(CONTAINER* cntr, ADDR_DATUM_PTR var_z,
                                 ADDR_DATUM_PTR var_x, ADDR_DATUM_PTR var_y) {
  STMT_LIST sl     = cntr->Stmt_list();
  NODE_PTR  x_node = cntr->New_ld(var_x, Spos());
  NODE_PTR  y_node = cntr->New_ld(var_y, Spos());
  NODE_PTR  add_node =
      cntr->New_bin_arith(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                            fhe::ckks::CKKS_OPERATOR::ADD),
                          x_node, y_node, Spos());
  add_node->Set_rtype(Ciph_ty());
  STMT_PTR add_stmt = cntr->New_st(add_node, var_z, Spos());
  sl.Append(add_stmt);
  return add_stmt;
}

// var_z = var_x * var_p
STMT_PTR TEST_CKKS2POLY::Gen_mul(CONTAINER* cntr, ADDR_DATUM_PTR var_z,
                                 ADDR_DATUM_PTR var_x, ADDR_DATUM_PTR var_y) {
  STMT_LIST sl     = cntr->Stmt_list();
  NODE_PTR  x_node = cntr->New_ld(var_x, Spos());
  NODE_PTR  y_node = cntr->New_ld(var_y, Spos());
  NODE_PTR  mul_node =
      cntr->New_bin_arith(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                            fhe::ckks::CKKS_OPERATOR::MUL),
                          x_node, y_node, Spos());
  if (var_y->Type() == Ciph_ty()) {
    mul_node->Set_rtype(Ciph3_ty());
  } else {
    mul_node->Set_rtype(Ciph_ty());
  }
  STMT_PTR mul_stmt = cntr->New_st(mul_node, var_z, Spos());
  sl.Append(mul_stmt);
  return mul_stmt;
}

STMT_PTR TEST_CKKS2POLY::Gen_mul(CONTAINER* cntr, PREG_PTR preg_z,
                                 ADDR_DATUM_PTR var_x, ADDR_DATUM_PTR var_y) {
  STMT_LIST sl     = cntr->Stmt_list();
  NODE_PTR  x_node = cntr->New_ld(var_x, Spos());
  NODE_PTR  y_node = cntr->New_ld(var_y, Spos());
  NODE_PTR  mul_node =
      cntr->New_bin_arith(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                            fhe::ckks::CKKS_OPERATOR::MUL),
                          x_node, y_node, Spos());
  if (var_y->Type() == Ciph_ty()) {
    mul_node->Set_rtype(Ciph3_ty());
  } else {
    mul_node->Set_rtype(Ciph_ty());
  }
  STMT_PTR mul_stmt = cntr->New_stp(mul_node, preg_z, Spos());
  sl.Append(mul_stmt);
  return mul_stmt;
}

// var_z = rotate(var_x, 5)
STMT_PTR TEST_CKKS2POLY::Gen_rotate(CONTAINER* cntr, ADDR_DATUM_PTR var_z,
                                    ADDR_DATUM_PTR var_x, int32_t rot_idx) {
  STMT_LIST sl        = cntr->Stmt_list();
  NODE_PTR  x_node    = cntr->New_ld(var_x, Spos());
  NODE_PTR  n_rot_idx = cntr->New_intconst(
      cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32), rot_idx, Spos());
  NODE_PTR rotate_node =
      cntr->New_cust_node(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                            fhe::ckks::CKKS_OPERATOR::ROTATE),
                          Ciph_ty(), Spos());
  rotate_node->Set_child(0, x_node);
  rotate_node->Set_child(1, n_rot_idx);
  STMT_PTR rotate_stmt = cntr->New_st(rotate_node, var_z, Spos());
  sl.Append(rotate_stmt);
  return rotate_stmt;
}

// preg_z = rotate(preg_x, rot_idx)
STMT_PTR TEST_CKKS2POLY::Gen_rotate(CONTAINER* cntr, PREG_PTR preg_z,
                                    PREG_PTR preg_x, int32_t rot_idx) {
  STMT_LIST sl        = cntr->Stmt_list();
  NODE_PTR  x_node    = cntr->New_ldp(preg_x, Spos());
  NODE_PTR  n_rot_idx = cntr->New_intconst(
      cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32), rot_idx, Spos());
  NODE_PTR rotate_node =
      cntr->New_cust_node(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                            fhe::ckks::CKKS_OPERATOR::ROTATE),
                          Ciph_ty(), Spos());
  rotate_node->Set_child(0, x_node);
  rotate_node->Set_child(1, n_rot_idx);
  STMT_PTR rotate_stmt = cntr->New_stp(rotate_node, preg_z, Spos());
  sl.Append(rotate_stmt);
  return rotate_stmt;
}

// var_z = rescale(var_x)
STMT_PTR TEST_CKKS2POLY::Gen_rescale(CONTAINER* cntr, ADDR_DATUM_PTR var_z,
                                     ADDR_DATUM_PTR var_x) {
  STMT_LIST sl     = cntr->Stmt_list();
  NODE_PTR  x_node = cntr->New_ld(var_x, Spos());
  NODE_PTR  n_x    = cntr->New_ld(var_x, Spos());
  NODE_PTR  rescale_node =
      cntr->New_cust_node(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                            fhe::ckks::CKKS_OPERATOR::RESCALE),
                          Ciph_ty(), Spos());
  rescale_node->Set_child(0, n_x);
  STMT_PTR rescale_stmt = cntr->New_st(rescale_node, var_z, Spos());
  sl.Append(rescale_stmt);
  return rescale_stmt;
}

// var_z = relin(var_x)
STMT_PTR TEST_CKKS2POLY::Gen_relin(CONTAINER* cntr, ADDR_DATUM_PTR var_z,
                                   ADDR_DATUM_PTR var_x) {
  STMT_LIST sl  = cntr->Stmt_list();
  NODE_PTR  n_x = cntr->New_ld(var_x, Spos());
  NODE_PTR  relin_node =
      cntr->New_una_arith(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                            fhe::ckks::CKKS_OPERATOR::RELIN),
                          n_x, Spos());
  relin_node->Set_rtype(Ciph_ty());
  STMT_PTR relin_stmt = cntr->New_st(relin_node, var_z, Spos());
  sl.Append(relin_stmt);
  return relin_stmt;
}

// preg_z = relin(var_x)
STMT_PTR TEST_CKKS2POLY::Gen_relin(CONTAINER* cntr, PREG_PTR preg_z,
                                   ADDR_DATUM_PTR var_x) {
  STMT_LIST sl  = cntr->Stmt_list();
  NODE_PTR  n_x = cntr->New_ld(var_x, Spos());
  NODE_PTR  relin_node =
      cntr->New_una_arith(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                            fhe::ckks::CKKS_OPERATOR::RELIN),
                          n_x, Spos());
  relin_node->Set_rtype(Ciph_ty());
  STMT_PTR relin_stmt = cntr->New_stp(relin_node, preg_z, Spos());
  sl.Append(relin_stmt);
  return relin_stmt;
}

// var_z = mul_float(x, f)
STMT_PTR TEST_CKKS2POLY::Gen_mul_float(CONTAINER* cntr, ADDR_DATUM_PTR var_z,
                                       ADDR_DATUM_PTR var_x, long double f) {
  STMT_LIST    sl        = cntr->Stmt_list();
  CONSTANT_PTR cst_float = cntr->Glob_scope()->New_const(
      CONSTANT_KIND::FLOAT,
      cntr->Glob_scope()->Prim_type(PRIMITIVE_TYPE::FLOAT_64), f);
  NODE_PTR n_x    = cntr->New_ld(var_x, Spos());
  NODE_PTR f_node = cntr->New_ldc(cst_float, Spos());
  NODE_PTR mul_float_node =
      cntr->New_bin_arith(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                            fhe::ckks::CKKS_OPERATOR::MUL),
                          n_x, f_node, Spos());
  STMT_PTR mul_float_stmt = cntr->New_st(mul_float_node, var_z, Spos());
  sl.Append(mul_float_stmt);
  return mul_float_stmt;
}

// var_z = encode()
STMT_PTR TEST_CKKS2POLY::Gen_encode(CONTAINER* cntr, ADDR_DATUM_PTR var_p) {
  STMT_LIST     sl        = cntr->Stmt_list();
  GLOB_SCOPE*   glob      = cntr->Glob_scope();
  PRIM_TYPE_PTR u32_type  = glob->Prim_type(PRIMITIVE_TYPE::INT_U32);
  STR_PTR       type_name = glob->Undefined_name();
  ARB_PTR       arb       = glob->New_arb(1, 0, 1, 1);
  TYPE_PTR      arr_type = glob->New_arr_type(type_name, u32_type, arb, Spos());
  NODE_PTR      encode_node =
      cntr->New_cust_node(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                            fhe::ckks::CKKS_OPERATOR::ENCODE),
                          Plain_ty(), Spos());
  void*        data_buffer = malloc(sizeof(uint32_t));
  CONSTANT_PTR cst         = glob->New_const(CONSTANT_KIND::ARRAY, arr_type,
                                             data_buffer, sizeof(uint32_t));
  encode_node->Set_child(0, cntr->New_ldc(cst, Spos()));
  encode_node->Set_child(1, cntr->New_intconst(u32_type, 4, Spos()));
  encode_node->Set_child(2, cntr->New_intconst(u32_type, 0, Spos()));
  encode_node->Set_child(3, cntr->New_intconst(u32_type, 0, Spos()));
  STMT_PTR encode_stmt = cntr->New_st(encode_node, var_p, Spos());
  sl.Append(encode_stmt);
  return encode_stmt;
}

// tmp = bootstrap(var_x)
STMT_PTR TEST_CKKS2POLY::Gen_bootstrap(CONTAINER* cntr, ADDR_DATUM_PTR var_x) {
  STMT_LIST sl       = cntr->Stmt_list();
  NODE_PTR  bts_node = cntr->New_cust_node(
      air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                         fhe::ckks::CKKS_OPERATOR::BOOTSTRAP),
      Ciph_ty(), Spos());
  bts_node->Set_child(0, cntr->New_ld(var_x, Spos()));
  PREG_PTR tmp      = cntr->Parent_func_scope()->New_preg(Ciph_ty());
  STMT_PTR bts_stmt = cntr->New_stp(bts_node, tmp, Spos());
  sl.Append(bts_stmt);
  return bts_stmt;
}

// return var_z
STMT_PTR TEST_CKKS2POLY::Gen_ret(CONTAINER* cntr, ADDR_DATUM_PTR var_z) {
  STMT_LIST sl       = cntr->Stmt_list();
  NODE_PTR  ld_z     = cntr->New_ld(var_z, Spos());
  STMT_PTR  ret_stmt = cntr->New_retv(ld_z, Spos());
  sl.Append(ret_stmt);
  return ret_stmt;
}

TEST_F(TEST_CKKS2POLY, Handle_add_ciph) {
  STMT_PTR stmt = Gen_add(Container(), Var_z(), Var_x(), Var_y());
  std::cout << "CKKS2POLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  stmt->Print();

  // vistor need a block node to start
  CKKS2POLY_RETV pair =
      New_visitor()->Visit<CKKS2POLY_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2POLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  New_container()->Print();
}

TEST_F(TEST_CKKS2POLY, Handle_add_plain) {
  STMT_PTR stmt = Gen_add(Container(), Var_z(), Var_x(), Var_p());
  std::cout << "CKKS2POLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  stmt->Print();

  // vistor need a block node to start
  CKKS2POLY_RETV pair =
      New_visitor()->Visit<CKKS2POLY_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2POLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  New_container()->Print();
}

TEST_F(TEST_CKKS2POLY, Handle_mul_plain) {
  STMT_PTR stmt = Gen_mul(Container(), Var_z(), Var_x(), Var_p());
  std::cout << "CKKS2POLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  stmt->Print();

  // vistor need a block node to start
  CKKS2POLY_RETV pair =
      New_visitor()->Visit<CKKS2POLY_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2POLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  New_container()->Print();
}

TEST_F(TEST_CKKS2POLY, Handle_mul_ciph) {
  STMT_PTR stmt = Gen_mul(Container(), Var_ciph3(), Var_x(), Var_y());
  std::cout << "CKKS2POLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  stmt->Print();

  // vistor need a block node to start
  CKKS2POLY_RETV pair =
      New_visitor()->Visit<CKKS2POLY_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2POLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  New_container()->Print();
}

TEST_F(TEST_CKKS2POLY, Handle_mul_ciph_preg) {
  PREG_PTR preg_z = Container()->Parent_func_scope()->New_preg(Ciph3_ty());
  STMT_PTR stmt   = Gen_mul(Container(), preg_z, Var_x(), Var_y());
  std::cout << "CKKS2POLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  stmt->Print();

  // vistor need a block node to start
  CKKS2POLY_RETV pair =
      New_visitor()->Visit<CKKS2POLY_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2POLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  New_container()->Print();
}

TEST_F(TEST_CKKS2POLY, Handle_mul_float) {
  STMT_PTR stmt = Gen_mul_float(Container(), Var_z(), Var_x(), 3.0);
  std::cout << "CKKS2POLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  stmt->Print();

  // vistor need a block node to start
  CKKS2POLY_RETV pair =
      New_visitor()->Visit<CKKS2POLY_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2POLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  New_container()->Print();
}

TEST_F(TEST_CKKS2POLY, Handle_relin_inline) {
  STMT_PTR stmt = Gen_relin(Container(), Var_z(), Var_ciph3());
  std::cout << "CKKS2POLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  stmt->Print();

  // vistor need a block node to start
  CKKS2POLY_RETV pair =
      New_visitor()->Visit<CKKS2POLY_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2POLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  New_container()->Print();
}

TEST_F(TEST_CKKS2POLY, Handle_relin_func) {
  STMT_PTR           stmt    = Gen_relin(Container(), Var_z(), Var_ciph3());
  CKKS2POLY_VISITOR* visitor = New_visitor();
  Ctx()->Config().Set_inline_relin(false);
  std::cout << "CKKS2POLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  stmt->Print();

  // vistor need a block node to start
  CKKS2POLY_RETV pair =
      visitor->Visit<CKKS2POLY_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2POLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  New_container()->Glob_scope()->Print();
}

TEST_F(TEST_CKKS2POLY, Handle_relin_func_preg) {
  PREG_PTR preg_z = Container()->Parent_func_scope()->New_preg(Ciph_ty());
  STMT_PTR stmt   = Gen_relin(Container(), preg_z, Var_ciph3());
  CKKS2POLY_VISITOR* visitor = New_visitor();
  Ctx()->Config().Set_inline_relin(false);
  std::cout << "CKKS2POLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  stmt->Print();

  // vistor need a block node to start
  CKKS2POLY_RETV pair =
      visitor->Visit<CKKS2POLY_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2POLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  New_container()->Glob_scope()->Print();
}

TEST_F(TEST_CKKS2POLY, Handle_load_ciph) {
  NODE_PTR node = Container()->New_ld(Var_x(), Spos());
  std::cout << "CKKS2POLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  node->Print();

  CKKS2POLY_RETV pair = New_visitor()->Visit<CKKS2POLY_RETV>(node);

  std::cout << "\nCKKS2POLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  pair.Node1()->Print();
  pair.Node2()->Print();
}

TEST_F(TEST_CKKS2POLY, Handle_load_plain) {
  NODE_PTR node = Container()->New_ld(Var_p(), Spos());
  std::cout << "CKKS2POLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  node->Print();

  CKKS2POLY_RETV pair = New_visitor()->Visit<CKKS2POLY_RETV>(node);

  std::cout << "\nCKKS2POLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  pair.Node1()->Print();
  EXPECT_EQ(pair.Node2(), air::base::Null_ptr);
  EXPECT_EQ(pair.Kind(), RETV_KIND::RK_PLAIN_POLY);
}

TEST_F(TEST_CKKS2POLY, Handle_rotate_inline) {
  STMT_PTR stmt = Gen_rotate(Container(), Var_z(), Var_x(), 5);
  std::cout << "CKKS2POLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  stmt->Print();

  // vistor need a block node to start
  CKKS2POLY_RETV pair =
      New_visitor()->Visit<CKKS2POLY_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2POLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  New_container()->Print();
}

TEST_F(TEST_CKKS2POLY, Handle_rotate_func) {
  STMT_PTR           stmt    = Gen_rotate(Container(), Var_z(), Var_x(), 5);
  CKKS2POLY_VISITOR* visitor = New_visitor();
  std::cout << "CKKS2POLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  stmt->Print();
  Ctx()->Config().Set_inline_rotate(false);

  // vistor need a block node to start
  CKKS2POLY_RETV pair =
      visitor->Visit<CKKS2POLY_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2POLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  New_container()->Glob_scope()->Print();
}

TEST_F(TEST_CKKS2POLY, Handle_rotate_func_preg) {
  PREG_PTR preg_z = Container()->Parent_func_scope()->New_preg(Ciph_ty());
  PREG_PTR preg_x = Container()->Parent_func_scope()->New_preg(Ciph_ty());
  STMT_PTR stmt   = Gen_rotate(Container(), preg_z, preg_x, 5);
  CKKS2POLY_VISITOR* visitor = New_visitor();
  std::cout << "CKKS2POLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  stmt->Print();
  Ctx()->Config().Set_inline_rotate(false);

  // vistor need a block node to start
  CKKS2POLY_RETV pair =
      visitor->Visit<CKKS2POLY_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2POLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  New_container()->Glob_scope()->Print();
}

TEST_F(TEST_CKKS2POLY, Handle_rescale) {
  STMT_PTR stmt = Gen_rescale(Container(), Var_z(), Var_x());
  std::cout << "CKKS2POLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  stmt->Print();

  // vistor need a block node to start
  CKKS2POLY_RETV pair =
      New_visitor()->Visit<CKKS2POLY_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2POLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  New_container()->Print();
}

TEST_F(TEST_CKKS2POLY, Handle_bts) {
  STMT_PTR stmt = Gen_bootstrap(Container(), Var_x());
  std::cout << "CKKS2POLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  stmt->Print();

  // vistor need a block node to start
  CKKS2POLY_RETV pair =
      New_visitor()->Visit<CKKS2POLY_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2POLY After:" << std::endl;
  std::cout << "====================================" << std::endl;

  New_container()->Print();
}

TEST_F(TEST_CKKS2POLY, Handle_encode) {
  STMT_PTR stmt = Gen_encode(Container(), Var_p());
  std::cout << "CKKS2POLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  stmt->Print();

  // vistor need a block node to start
  CKKS2POLY_RETV pair =
      New_visitor()->Visit<CKKS2POLY_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2POLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  New_container()->Print();
  EXPECT_EQ(pair.Node2(), air::base::Null_ptr);
}

// DISABLED as PREG LDID not supported yet
TEST_F(TEST_CKKS2POLY, Handle_stp) {
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
  std::cout << "CKKS2POLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  rescale_stmt->Print();

  // vistor need a block node to start
  CKKS2POLY_RETV pair =
      New_visitor()->Visit<CKKS2POLY_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2POLY After:" << std::endl;
  std::cout << "====================================" << std::endl;
  New_container()->Print();
  EXPECT_EQ(pair.Node2(), air::base::Null_ptr);
}

TEST_F(TEST_CKKS2POLY, Handle_all) {
  Gen_add(Container(), Var_z(), Var_x(), Var_y());
  Gen_add(Container(), Var_z(), Var_x(), Var_p());
  Gen_mul(Container(), Var_z(), Var_x(), Var_p());
  Gen_mul(Container(), Var_ciph3(), Var_x(), Var_y());
  Gen_mul_float(Container(), Var_z(), Var_x(), 3.0);
  Gen_relin(Container(), Var_z(), Var_ciph3());
  Gen_relin(Container(), Var_z(), Var_ciph3());
  Gen_rotate(Container(), Var_z(), Var_x(), 5);
  Gen_rotate(Container(), Var_z(), Var_x(), 5);
  Gen_rescale(Container(), Var_z(), Var_x());
  Gen_bootstrap(Container(), Var_x());
  Gen_encode(Container(), Var_p());
  Gen_ret(Container(), Var_z());

  std::cout << "CKKS2POLY Before:" << std::endl;
  std::cout << "====================================" << std::endl;
  Container()->Glob_scope()->Print();

  CKKS2POLY_RETV pair =
      New_visitor()->Visit<CKKS2POLY_RETV>(Container()->Entry_node());
  AIR_ASSERT(pair.Num_node() == 1);
  New_container()->Parent_func_scope()->Set_entry_stmt(pair.Node()->Stmt());

  std::cout << "\nCKKS2POLY After:" << std::endl;
  std::cout << "====================================" << std::endl;

  New_container()->Glob_scope()->Print();
}

}  // namespace
