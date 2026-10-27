//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/base/container.h"
#include "air/base/st.h"
#include "air/base/visitor.h"
#include "air/core/handler.h"
#include "air/core/null_handler.h"
#include "nn/core/handler.h"
#include "nn/core/opcode.h"
#include "nn/vector/core_handler.h"
#include "nn/vector/tensor2vector_handler.h"
#include "nn/vector/vector_gen.h"
#include "nn/vector/vector_opcode.h"
#include "nn/vector/vector_utils.h"

using namespace air::base;
using namespace nn::core;
using namespace nn::vector;

int main() {
  bool ret = air::core::Register_core();
  AIR_ASSERT_MSG(ret, "Register core domain failed");
  ret = nn::core::Register_nn();
  AIR_ASSERT_MSG(ret, "Register nn domain failed");
  ret = nn::vector::Register_vector_domain();
  AIR_ASSERT_MSG(ret, "Register vector domain failed");

  // check opcodes
  bool op_is_valid =
      META_INFO::Valid_operator(VECTOR_DOMAIN::ID, VECTOR_OPCODE::MUL);
  std::cout << "op_is_valid MUL: " << op_is_valid << std::endl;
  op_is_valid =
      META_INFO::Valid_operator(VECTOR_DOMAIN::ID, VECTOR_OPCODE::ROLL);
  std::cout << "op_is_valid ROLL: " << op_is_valid << std::endl;

  GLOB_SCOPE* glob = GLOB_SCOPE::Get();

  SPOS spos = glob->Unknown_simple_spos();
  // name of main function
  STR_PTR name_str = glob->New_str("main");
  // main function
  FUNC_PTR main_func = glob->New_func(name_str, spos);
  main_func->Set_parent(glob->Comp_env_id());
  // signature of main function
  SIGNATURE_TYPE_PTR sig = glob->New_sig_type();
  // return type of main function
  TYPE_PTR main_rtype = glob->Prim_type(PRIMITIVE_TYPE::VOID);
  glob->New_ret_param(main_rtype, sig);
  // parameter argc of function main
  TYPE_PTR etype = glob->Prim_type(PRIMITIVE_TYPE::FLOAT_32);

  std::vector<int64_t> input_shape{1, 3, 3};

  const int h = 4;
  const int w = input_shape[0] * input_shape[1] * input_shape[2];

  TYPE_PTR argc_type =
      New_array_type(glob, "input_float", etype, input_shape, spos);

  STR_PTR argc_str = glob->New_str("argc");
  glob->New_param(argc_str, argc_type, sig, spos);
  // parameter argv of function main, no pointer type yet
  TYPE_PTR argv_type = glob->Prim_type(PRIMITIVE_TYPE::INT_S8);
  STR_PTR  argv_str  = glob->New_str("argv");
  glob->New_param(argv_str, argv_type, sig, spos);
  sig->Set_complete();
  // global entry for main
  ENTRY_PTR entry =
      glob->New_global_entry_point(sig, main_func, name_str, spos);
  // set define before create a new scope
  FUNC_SCOPE* main_scope = &glob->New_func_scope(main_func);
  CONTAINER*  cntr       = &main_scope->Container();
  STMT_PTR    ent_stmt   = cntr->New_func_entry(spos);

  // Tensor input, weight, bias;
  ADDR_DATUM_PTR input_var = main_scope->Formal(0);

  // weight is const.
  std::vector<int64_t> weight_shape{h, w};
  std::vector<float>   weight(h * w);
  for (int i = 0; i < h * w; i++) weight[i] = i;

  CONSTANT_PTR weight_const =
      New_array_const(glob, "weight_float", h * w, etype, weight_shape,
                      (void*)weight.data(), spos);

  // bias const
  std::vector<int64_t> bias_shape{h};
  std::vector<float>   bias(h, 0.1);
  CONSTANT_PTR         bias_const = New_array_const(
      glob, "bias_float", w, etype, bias_shape, (void*)bias.data(), spos);

  // f = nn::core:FLATTEN(input)
  int64_t flatten_size = w;
  // Actually: 2D [1, x]
  std::vector<int64_t> flatten_shape{1, flatten_size};
  TYPE_PTR             flatten_rtype =
      New_array_type(glob, "flatten_float", etype, flatten_shape, spos);
  NODE_PTR flatten_node = cntr->New_cust_node(
      air::base::OPCODE(nn::core::NN, nn::core::OPCODE::FLATTEN), flatten_rtype,
      spos);
  flatten_node->Set_child(0, cntr->New_ld(input_var, spos));

  // z = nn::core:GEMM(f, weight_const, bias_const)
  std::vector<int64_t> result_shape{h};
  TYPE_PTR             result_rtype =
      New_array_type(glob, "result_float", etype, result_shape, spos);
  NODE_PTR nngemm_node = cntr->New_cust_node(
      air::base::OPCODE(nn::core::NN, nn::core::OPCODE::GEMM), result_rtype,
      spos);
  nngemm_node->Set_child(0, flatten_node);
  nngemm_node->Set_child(1, cntr->New_ldc(weight_const, spos));
  nngemm_node->Set_child(2, cntr->New_ldc(bias_const, spos));

  // result = nn::core:GEMM(z, weight2_const, bias_const)
  std::vector<int64_t> weight2_shape{h, h};
  std::vector<float>   weight2(h * h);
  for (int i = 0; i < weight2.size(); i++) weight2[i] = i;

  CONSTANT_PTR weight2_const =
      New_array_const(glob, "weight2_float", h * h, etype, weight2_shape,
                      (void*)weight2.data(), spos);

  NODE_PTR nngemm2_node = cntr->New_cust_node(
      air::base::OPCODE(nn::core::NN, nn::core::OPCODE::GEMM), result_rtype,
      spos);
  nngemm2_node->Set_child(0, nngemm_node);
  nngemm2_node->Set_child(1, cntr->New_ldc(weight2_const, spos));
  nngemm2_node->Set_child(2, cntr->New_ldc(bias_const, spos));

  STR_PTR        result_name = glob->New_str("result");
  ADDR_DATUM_PTR result_var =
      main_scope->New_var(result_rtype, result_name, spos);

  STMT_PTR  nnstmt = cntr->New_st(nngemm2_node, result_var, spos);
  STMT_LIST sl     = cntr->Stmt_list();
  sl.Append(nnstmt);

  // return result;
  NODE_PTR result_node = cntr->New_ld(result_var, spos);
  STMT_PTR ret_stmt    = cntr->New_retv(result_node, spos);
  sl.Append(ret_stmt);
  std::cout << "tensor IR:" << std::endl;
  main_scope->Print();

  // tensor->vector
  nn::vector::VECTOR_CTX    ctx;
  nn::vector::VECTOR_CONFIG config;
  GLOB_SCOPE* new_glob = nn::vector::Vector_driver(glob, ctx, nullptr, config);
  std::cout << "vector IR:" << std::endl;
  new_glob->Open_func_scope(main_scope->Id()).Print();

  return 0;
}
