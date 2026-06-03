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
#include "nn/vector/vector_gen.h"
#include "nn/vector/vector_opcode.h"

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
  int64_t channel_in = 1, input_height = 4, input_width = 4;
  int64_t channel_out = 1, kernel_height = 3, kernel_width = 3;

  TYPE_PTR             etype = glob->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
  std::vector<int64_t> input_shape{channel_in, input_height, input_width};

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
  int64_t weight_size = channel_out * channel_in * kernel_height * kernel_width;
  std::vector<int64_t> weight_shape{channel_out, channel_in, kernel_height,
                                    kernel_width};
  std::vector<float>   weight(channel_out * channel_in * kernel_height *
                              kernel_width);
  for (int i = 0; i < weight_size; i++) weight[i] = 1.0 * (i % 10);

  CONSTANT_PTR weight_const =
      New_array_const(glob, "weight_float",
                      channel_out * channel_in * kernel_height * kernel_width,
                      etype, weight_shape, (void*)weight.data(), spos);

  // bias const
  std::vector<int64_t> bias_shape{channel_out};
  std::vector<float>   bias(channel_out);
  for (int i = 0; i < channel_out; i++) bias[i] = 1.0 * i;

  CONSTANT_PTR bias_const =
      New_array_const(glob, "bias_float", channel_out, etype, bias_shape,
                      (void*)bias.data(), spos);

  STR_PTR        result_name = glob->New_str("result");
  ADDR_DATUM_PTR result_var = main_scope->New_var(argc_type, result_name, spos);

  // z = nn::core:CONV(input, weight_const, bias_const)
  NODE_PTR input_node  = cntr->New_ld(input_var, spos);
  NODE_PTR nngemm_node = cntr->New_cust_node(
      air::base::OPCODE(nn::core::NN, nn::core::OPCODE::CONV),
      input_node->Rtype(), spos);
  nngemm_node->Set_child(0, input_node);
  nngemm_node->Set_child(1, cntr->New_ldc(weight_const, spos));
  nngemm_node->Set_child(2, cntr->New_ldc(bias_const, spos));

  STMT_PTR  nnstmt = cntr->New_st(nngemm_node, result_var, spos);
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
