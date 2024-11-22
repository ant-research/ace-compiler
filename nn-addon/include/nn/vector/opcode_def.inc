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

DEF_OPCODE(INVALID, invalid, OPR_CAT::EXPR, 0, 0, 0)
DEF_OPCODE(ADD, add, OPR_CAT::EXPR, 2, 1, PROP_EXPR)
// Elementwise multiplication of two vectors
DEF_OPCODE(MUL, mul, OPR_CAT::EXPR, 2, 1, PROP_EXPR)
DEF_OPCODE(ROLL, roll, OPR_CAT::EXPR, 2, 1, PROP_EXPR | PROP_ATTR)
// The essence of slcie is "tiled load"
DEF_OPCODE(SLICE, slice, OPR_CAT::EXPR, 3, 1, PROP_EXPR)
DEF_OPCODE(PAD, pad, OPR_CAT::EXPR, 2, 1, PROP_EXPR)
DEF_OPCODE(RESHAPE, reshape, OPR_CAT::EXPR, 2, 1, PROP_EXPR)
DEF_OPCODE(READ, read, OPR_CAT::EXPR, 2, 1, PROP_EXPR)

// For runtime validation
DEF_OPCODE(AVERAGE_POOL_RTV,        average_pool_rtv,        OPR_CAT::EXPR, 1, 1, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)
DEF_OPCODE(CONV_RTV,                conv_rtv,                OPR_CAT::EXPR, 3, 1, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)
DEF_OPCODE(GEMM_RTV,                gemm_rtv,                OPR_CAT::EXPR, 3, 1, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)
DEF_OPCODE(GLOBAL_AVERAGE_POOL_RTV, global_average_pool_rtv, OPR_CAT::EXPR, 1, 1, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)
DEF_OPCODE(MAX_POOL_RTV,            max_pool_rtv,            OPR_CAT::EXPR, 1, 1, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)
DEF_OPCODE(RELU_RTV,                relu_rtv,                OPR_CAT::EXPR, 1, 1, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)

// For reference used by runtime validation
DEF_OPCODE(ADD_REF,                 add_ref,                 OPR_CAT::EXPR, 2, 1, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)
DEF_OPCODE(AVERAGE_POOL_REF,        average_pool_ref,        OPR_CAT::EXPR, 1, 1, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)
DEF_OPCODE(CONV_REF,                conv_ref,                OPR_CAT::EXPR, 3, 1, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)
DEF_OPCODE(FLATTEN_REF,             flatten_ref,             OPR_CAT::EXPR, 1, 1, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)
DEF_OPCODE(GEMM_REF,                gemm_ref,                OPR_CAT::EXPR, 3, 1, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)
DEF_OPCODE(GLOBAL_AVERAGE_POOL_REF, global_average_pool_ref, OPR_CAT::EXPR, 1, 1, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)
DEF_OPCODE(MAX_POOL_REF,            max_pool_ref,            OPR_CAT::EXPR, 1, 1, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)
DEF_OPCODE(RELU_REF,                relu_ref,                OPR_CAT::EXPR, 1, 1, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)
DEF_OPCODE(RESHAPE_REF,             reshape_ref,             OPR_CAT::EXPR, 2, 1, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)
