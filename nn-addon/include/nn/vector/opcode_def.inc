//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

/**
 * @brief Defines all NN OPCODE in this file with the following format:
 *   DEF_OPCODE(NAME, name, category, kid_num, fld_num, property)
 *
 */

DEF_OPCODE(INVALID, invalid, 0, 0, 0)
DEF_OPCODE(ADD, add, 2, 0, PROP_EXPR | PROP_ATTR)
// Elementwise multiplication of two vectors
DEF_OPCODE(MUL, mul, 2, 0, PROP_EXPR | PROP_ATTR)
DEF_OPCODE(ROLL, roll, 2, 0, PROP_EXPR | PROP_ATTR)
// The essence of slice is "tiled load"
DEF_OPCODE(SLICE, slice, 3, 0, PROP_EXPR)
DEF_OPCODE(PAD, pad, 2, 0, PROP_EXPR)
DEF_OPCODE(RESHAPE, reshape, 2, 0, PROP_EXPR)
DEF_OPCODE(READ, read, 2, 0, PROP_EXPR)

// For runtime validation
DEF_OPCODE(AVERAGE_POOL_RTV,        average_pool_rtv,        1, 0, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)
DEF_OPCODE(CONV_RTV,                conv_rtv,                3, 0, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)
DEF_OPCODE(GEMM_RTV,                gemm_rtv,                3, 0, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)
DEF_OPCODE(GLOBAL_AVERAGE_POOL_RTV, global_average_pool_rtv, 1, 0, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)
DEF_OPCODE(MAX_POOL_RTV,            max_pool_rtv,            1, 0, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)
DEF_OPCODE(RELU_RTV,                relu_rtv,                1, 0, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)

// For reference used by runtime validation
DEF_OPCODE(ADD_REF,                 add_ref,                 2, 0, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)
DEF_OPCODE(AVERAGE_POOL_REF,        average_pool_ref,        1, 0, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)
DEF_OPCODE(CONV_REF,                conv_ref,                3, 0, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)
DEF_OPCODE(FLATTEN_REF,             flatten_ref,             1, 0, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)
DEF_OPCODE(GEMM_REF,                gemm_ref,                3, 0, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)
DEF_OPCODE(GLOBAL_AVERAGE_POOL_REF, global_average_pool_ref, 1, 0, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)
DEF_OPCODE(MAX_POOL_REF,            max_pool_ref,            1, 0, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)
DEF_OPCODE(RELU_REF,                relu_ref,                1, 0, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)
DEF_OPCODE(RESHAPE_REF,             reshape_ref,             2, 0, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)

// medium level opcode
// roll_sum, replicate and compact are same except that rotate step is different
DEF_OPCODE(ROLL_SUM, roll_sum, 1, 1, PROP_EXPR|PROP_ATTR)            // kids: input