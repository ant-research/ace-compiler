//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "gtest/gtest.h"
#include "nn/core/opcode.h"
#include "nn/vector/selective_strided_slice.h"
#include "nn/vector/vector_gen.h"
#include "nn/vector/vector_opcode.h"
#include "nn/vector/vector_utils.h"

using namespace air::base;
using namespace nn::vector;

TEST(ExpandWithZero, expand_with_zero) {
  // prepare data
  int   height = 120;
  int   width  = 400;
  FPMAT input(height);
  for (int i = 0; i < input.size(); i++) {
    input[i].resize(width, 0);
    for (int j = 0; j < width; j++) {
      input[i][j] = j + 1;
    }
  }
  // // help debug
  // for(int i=0; i<input.size(); i++) {
  //   for(int j=0; j<width; j++) {
  //     std::cout << input[i][j]<< " ";
  //   }
  //   std::cout << std::endl;
  // }

  FPVEC input_vec;
  input_vec.reserve(height * width);

  for (const auto& row : input) {
    input_vec.insert(input_vec.end(), row.begin(), row.end());
  }

  float* input_pointer = input_vec.data();

  int group_size         = 5;
  int accumulated_stride = 4;
  int trailing_zero_num  = 32 * 3 + (32 - 5 * 4);
  int target_width       = 16384;
  int channel_size       = 1024;

  FPMAT new_weight_mat = Expand_with_zero(
      input_pointer, height, width, group_size, accumulated_stride,
      trailing_zero_num, target_width, group_size * group_size, channel_size);

  // // help debug
  // for (int i = 0; i < new_weight_mat.size(); i++) {
  //   for (int j = 0; j < target_width; j++) {
  //     std::cout << new_weight_mat[i][j] << " ";
  //     if ((j + 1) % 32 == 0) {
  //       std::cout << std::endl;
  //     }
  //   }
  //   std::cout << std::endl;
  // }

  // target_width = 16384 = 16 *32 *32
  int c = 16;
  int h = 32;
  int w = 32;

  for (int n = 0; n < new_weight_mat.size(); n++) {
    int valid_val_num = 0;  // max value is 400
    for (int i = 0; i < c; i++) {
      bool meet_all_valid_data_in_channel = false;
      for (int j = 0; j < h; j++) {
        if ((valid_val_num - i * group_size * group_size != 0) &&
            (valid_val_num % (group_size * group_size) == 0)) {
          meet_all_valid_data_in_channel = true;
        }
        if (!meet_all_valid_data_in_channel) {
          if (j % accumulated_stride != 0) {
            for (int k = 0; k < w; k++) {
              EXPECT_FLOAT_EQ(new_weight_mat[0][i * h * w + j * w + k], 0);
            }
          } else {
            int vdir = 0;  // valid data in row
            for (int k = 0; k < w; k++) {
              if ((k % accumulated_stride == 0) && (vdir < group_size) &&
                  (valid_val_num <= width)) {
                vdir++;
                valid_val_num++;
                EXPECT_FLOAT_EQ(new_weight_mat[0][i * h * w + j * w + k],
                                valid_val_num);
              } else {
                EXPECT_FLOAT_EQ(new_weight_mat[0][i * h * w + j * w + k], 0);
              }
            }
          }
        } else {
          for (int k = 0; k < w; k++) {
            EXPECT_FLOAT_EQ(new_weight_mat[0][i * h * w + j * w + k], 0);
          }
        }
      }
    }
  }
}

TEST(FuseStridedSlice, fuse_multiple_strided_slice_to_one) {
  bool ret = air::core::Register_core();
  AIR_ASSERT_MSG(ret, "Register core domain failed");
  ret = nn::core::Register_nn();
  AIR_ASSERT_MSG(ret, "Register nn domain failed");

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

  std::vector<int64_t> start_indiex{0, 0};
  std::vector<int64_t> slice_size{20, 20};
  std::vector<int64_t> stride_size{2, 2};
  VECTOR_GEN           vgen(cntr);

  NODE_PTR ss_node1 =
      vgen.New_strided_slice(cntr->New_ld(input_var, spos), start_indiex,
                             slice_size, stride_size, 6, spos);

  NODE_PTR ss_node2 = vgen.New_strided_slice(ss_node1, start_indiex, slice_size,
                                             stride_size, 16, spos);

  std::vector<int64_t> result_shape{h};
  TYPE_PTR             result_rtype =
      New_array_type(glob, "result_float", etype, result_shape, spos);

  PREG_PTR ret_preg = cntr->Parent_func_scope()->New_preg(result_rtype);
  STMT_PTR st_ret   = cntr->New_stp(ss_node2, ret_preg, spos);
  cntr->Stmt_list().Append(st_ret);

  // // weight is const.
  // std::vector<int64_t> weight_shape{h, w};
  // std::vector<float>   weight(h * w);
  // for (int i = 0; i < h * w; i++) weight[i] = i;

  // CONSTANT_PTR weight_const =
  //     New_array_const(glob, "weight_float", h * w, etype, weight_shape,
  //                     (void*)weight.data(), spos);

  // // bias const
  // std::vector<int64_t> bias_shape{h};
  // std::vector<float>   bias(h, 0.1);
  // CONSTANT_PTR         bias_const = New_array_const(
  //     glob, "bias_float", h, etype, bias_shape, (void*)bias.data(), spos);

  // // z = nn::core:GEMM(f, weight_const, bias_const)
  // std::vector<int64_t> result_shape{h};
  // TYPE_PTR             result_rtype =
  //     New_array_type(glob, "result_float", etype, result_shape, spos);
  // NODE_PTR nngemm_node = cntr->New_cust_node(
  //     air::base::OPCODE(nn::core::NN, nn::core::OPCODE::GEMM), result_rtype,
  //     spos);
  // nngemm_node->Set_child(0, ss_node2);
  // nngemm_node->Set_child(1, cntr->New_ldc(weight_const, spos));
  // nngemm_node->Set_child(2, cntr->New_ldc(bias_const, spos));
  // EXPECT_EQ(nngemm_node->Child(0)->Opcode(),
  //           air::base::OPCODE(nn::core::NN,
  //           nn::core::OPCODE::STRIDED_SLICE));

  // input is gemm.strided_slice.strided_slice.input
  // Fuse_strided_slice_into_one will fuse multiple strided_slice into one
  // strided_slice, and then save strided_slice to preg. Gemm will use ldp as
  // its first child.
  nn::vector::VECTOR_CTX      ctx;
  nn::vector::VECTOR_CONFIG   config;
  nn::vector::SS_SFT_MAP      ss_sft_map;
  SELECTIVE_STRIDED_SLICE_CTX sss_ctx(main_scope, ctx, nullptr, config,
                                      ss_sft_map);
  // sss_ctx.Push(st_ret->Node());
  // sss_ctx.Push(ss_node2);
  // sss_ctx.Push(ss_node1);
  // EXPECT_EQ(st_ret->Node()->Child(0)->Child(0)->Opcode(),
  //           air::base::OPCODE(nn::core::NN,
  //           nn::core::OPCODE::STRIDED_SLICE));
  // sss_ctx.Fuse_strided_slice_into_one(ss_node2);
  // EXPECT_NE(st_ret->Node()->Child(0)->Child(0)->Opcode(),
  //           air::base::OPCODE(nn::core::NN,
  //           nn::core::OPCODE::STRIDED_SLICE));
  // EXPECT_NE(nngemm_node->Child(0)->Opcode(),
  //           air::base::OPCODE(nn::core::NN,
  //           nn::core::OPCODE::STRIDED_SLICE));
}

TEST(NextPowerOfTwo, positive_numbers) {
  EXPECT_EQ(Next_power_of_two(1), 1);
  EXPECT_EQ(Next_power_of_two(2), 2);
  EXPECT_EQ(Next_power_of_two(3), 4);
  EXPECT_EQ(Next_power_of_two(4), 4);
  EXPECT_EQ(Next_power_of_two(5), 8);
  EXPECT_EQ(Next_power_of_two(15), 16);
  EXPECT_EQ(Next_power_of_two(16), 16);
  EXPECT_EQ(Next_power_of_two(17), 32);
  EXPECT_EQ(Next_power_of_two(37), 64);
}

TEST(NextPowerOfTwo, edge_case) {
  EXPECT_EQ(Next_power_of_two(0), 1);
  EXPECT_EQ(Next_power_of_two(INT64_MAX), 9223372036854775808ULL);
}

TEST(NextPowerOfTwo, negative_numbers) {
  EXPECT_EQ(Next_power_of_two(-1), 1);
  EXPECT_EQ(Next_power_of_two(-100), 1);
}

TEST(GetLogCcLub, Get_log_cc_lub) {
  EXPECT_EQ(Get_log_cc_lub(2, 10, 10, 2), 1);
  EXPECT_EQ(Get_log_cc_lub(2, 112, 112, 2), 1);
  EXPECT_EQ(Get_log_cc_lub(512, 8, 8, 8), log2(8 * 8));
  EXPECT_EQ(Get_log_cc_lub(512, 2, 2, 2), 2);
  EXPECT_EQ(Get_log_cc_lub(3, 4, 4, 2), 2);
  EXPECT_EQ(Get_log_cc_lub(6, 28, 28, 2), 2);
  EXPECT_EQ(Get_log_cc_lub(32, 32, 32, 4), 4);
}

TEST(GetLogRcLub, Get_log_rc_lub) {
  EXPECT_EQ(Get_log_rc_lub(48, 16, 16), 2);
  EXPECT_EQ(Get_log_rc_lub(16, 2, 2), 1);
  EXPECT_EQ(Get_log_rc_lub(8, 2, 2), 1);
}

TEST(EnableLargeChannelComb, Enable_large_channel_comb) {
  EXPECT_TRUE(Enable_large_channel_comb(256, 2));
  EXPECT_TRUE(Enable_large_channel_comb(512, 2));
  EXPECT_FALSE(Enable_large_channel_comb(256, 8));
}

TEST(EnableLogCcComb, Enable_log_cc_comb) {
  EXPECT_TRUE(Enable_log_cc_comb(64, 8, 8, 1, 1));
  EXPECT_TRUE(Enable_log_cc_comb(32, 8, 8, 1, 1));
  EXPECT_FALSE(Enable_log_cc_comb(31, 8, 8, 1, 1));
}