//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/core/opcode.h"
#include "e2e_driver.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/poly/opcode.h"
#include "fhe/sihe/sihe_opcode.h"
#include "nn/core/opcode.h"
#include "nn/vector/vector_opcode.h"

using namespace air::base;

#include "e2e_driver.inc"

void fhe::test::E2E_DRIVER::Build_vector_ir(air::base::GLOB_SCOPE* glob) {
  IR_BUILDER irb(glob);
  TYPE_PTR   arr_ty = irb.Arr_f32(8);
  irb.New_func("Main_graph", {
                                 {arr_ty, "input"}
  });
  ADDR_DATUM_PTR input  = irb.Formal(0);
  ADDR_DATUM_PTR output = irb.New_var(arr_ty, "output");

  int32_t  roll_idx = 2;
  NODE_PTR roll_node =
      irb.New_cust_node(air::base::OPCODE(nn::vector::VECTOR_DOMAIN::ID,
                                          nn::vector::VECTOR_OPCODE::ROLL),
                        arr_ty);
  roll_node->Set_child(0, irb.New_ld(input));
  roll_node->Set_child(1, irb.New_const_s32(roll_idx));
  roll_node->Set_attr("nums", &roll_idx, 1);

  STMT_PTR roll_stmt = irb.New_st(roll_node, output);
  irb.Append(roll_stmt);

  STMT_PTR ret_stmt = irb.New_retv(irb.New_ld(output));
  irb.Append(ret_stmt);
}
