//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <random>

#include "air/base/container_decl.h"
#include "air/base/meta_info.h"
#include "air/base/node.h"
#include "air/base/opcode.h"
#include "air/base/st.h"
#include "air/base/st_decl.h"
#include "air/base/st_enum.h"
#include "air/base/st_type.h"
#include "air/base/visitor.h"
#include "air/core/handler.h"
#include "air/core/opcode.h"
#include "fhe/sihe/config.h"
#include "fhe/sihe/sihe_gen.h"
#include "fhe/sihe/sihe_opcode.h"
#include "fhe/sihe/vector2sihe_lower.h"
#include "gtest/gtest.h"
#include "nn/vector/vector_gen.h"
#include "nn/vector/vector_opcode.h"

using namespace air::base;
using namespace fhe::sihe;
using namespace fhe::core;
using namespace nn::vector;
using OPCODE = air::base::OPCODE;

TEST(CKKS, lower_vector2sihe) {
  META_INFO::Remove_all();
  air::core::Register_core();
  bool reg_sihe = Register_sihe_domain();
  ASSERT_TRUE(reg_sihe);

  bool reg_vec = Register_vector_domain();
  ASSERT_TRUE(reg_vec);

  GLOB_SCOPE* glob = GLOB_SCOPE::Get();
  LOWER_CTX   lower_ctx;
  SIHE_CONFIG sihe_gen_cfg;
  SIHE_GEN(glob, &lower_ctx).Register_sihe_types();

  SPOS spos = glob->Unknown_simple_spos();

  STR_PTR add_str = glob->New_str("My_add");
  // My_add function
  FUNC_PTR add_func = glob->New_func(add_str, spos);

  TYPE_PTR             f32_type = glob->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
  std::vector<int64_t> dim{20};
  TYPE_PTR             array_type =
      New_array_type(glob, "client_array", f32_type, dim, spos);

  add_func->Set_parent(glob->Comp_env_id());
  // signature of My_add function
  SIGNATURE_TYPE_PTR add_sig = glob->New_sig_type();
  // return type of My_add function
  glob->New_ret_param(array_type, add_sig);
  STR_PTR x_str = glob->New_str("x");
  glob->New_param(x_str, array_type, add_sig, spos);
  STR_PTR y_str = glob->New_str("y");
  glob->New_param(y_str, array_type, add_sig, spos);
  add_sig->Set_complete();
  glob->New_entry_point(add_sig, add_func, add_str, spos);

  // My_add definition
  FUNC_SCOPE*    add_scope = &glob->New_func_scope(add_func);
  CONTAINER*     add_cntr  = &add_scope->Container();
  STMT_PTR       ent_stmt  = add_cntr->New_func_entry(spos);
  ADDR_DATUM_PTR formal_x  = add_scope->Formal(0);
  ADDR_DATUM_PTR formal_y  = add_scope->Formal(1);

  STR_PTR        z_str = glob->New_str("z");
  ADDR_DATUM_PTR var_z = add_scope->New_var(array_type, z_str, spos);

  // load x
  NODE_PTR load_x = add_cntr->New_ld(formal_x, spos);
  // load y
  NODE_PTR load_y = add_cntr->New_ld(formal_y, spos);

  // x + y
  NODE_PTR add_node = VECTOR_GEN(add_cntr).New_add(load_x, load_y, spos);

  // 0.25 * (x + y)
  CONSTANT_PTR cst =
      glob->New_const(CONSTANT_KIND::FLOAT, f32_type, (long double)(0.25));
  NODE_PTR ldc      = add_cntr->New_ldc(cst, spos);
  NODE_PTR mul_node = VECTOR_GEN(add_cntr).New_mul(add_node, ldc, spos);

  // z = 0.25 * (x + y)
  STMT_PTR  store_stmt = add_cntr->New_st(mul_node, var_z, spos);
  STMT_LIST sl         = add_cntr->Stmt_list();
  sl.Append(store_stmt);

  // ret (z);
  NODE_PTR load_z  = add_cntr->New_ld(var_z, spos);
  STMT_PTR add_ret = add_cntr->New_retv(load_z, spos);
  sl.Append(add_ret);

  std::cout << "vector Add_func: " << std::endl;
  std::cout << add_scope->To_str() << std::endl;

  GLOB_SCOPE* sihe_glob_scope =
      Sihe_driver(glob, &lower_ctx, nullptr, sihe_gen_cfg);
  GLOB_SCOPE::FUNC_SCOPE_ITER scope_iter = sihe_glob_scope->Begin_func_scope();
  GLOB_SCOPE::FUNC_SCOPE_ITER end_scope_iter =
      sihe_glob_scope->End_func_scope();
  std::cout << "sihe Add_func: " << std::endl;
  for (; scope_iter != end_scope_iter; ++scope_iter) {
    const FUNC_SCOPE& func_scope = *scope_iter;
    std::cout << func_scope.To_str() << std::endl;
  }
}
