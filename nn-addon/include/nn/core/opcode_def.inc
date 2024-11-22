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
DEF_OPCODE(AVERAGE_POOL, average_pool, OPR_CAT::EXPR, 1, 1, PROP_EXPR | PROP_ATTR)
DEF_OPCODE(CONV, conv, OPR_CAT::EXPR, 3, 1, PROP_EXPR | PROP_ATTR)
DEF_OPCODE(FLATTEN, flatten, OPR_CAT::EXPR, 1, 1, PROP_EXPR | PROP_ATTR)
DEF_OPCODE(GEMM, gemm, OPR_CAT::EXPR, 3, 1, PROP_EXPR | PROP_ATTR)
DEF_OPCODE(GLOBAL_AVERAGE_POOL, global_average_pool, OPR_CAT::EXPR, 1, 1, PROP_EXPR)
DEF_OPCODE(MAX_POOL, max_pool, OPR_CAT::EXPR, 1, 1, PROP_EXPR | PROP_ATTR)
DEF_OPCODE(MUL, mul, OPR_CAT::EXPR, 2, 1, PROP_EXPR)
DEF_OPCODE(RELU, relu, OPR_CAT::EXPR, 1, 1, PROP_EXPR | PROP_ATTR)
DEF_OPCODE(RESHAPE, reshape, OPR_CAT::EXPR, 2, 1, PROP_EXPR | PROP_ATTR)
DEF_OPCODE(STRIDED_SLICE, strided_slice, OPR_CAT::EXPR, 4, 1, PROP_EXPR | PROP_ATTR)
DEF_OPCODE(SUB, sub, OPR_CAT::EXPR, 2, 1, PROP_EXPR)
DEF_OPCODE(RMSNORM, rmsnorm, OPR_CAT::EXPR, 2, 1, PROP_EXPR)
DEF_OPCODE(MATMUL, matmul, OPR_CAT::EXPR, 2, 1, PROP_EXPR)
DEF_OPCODE(ROPE_ROTARY, rope_rotary, OPR_CAT::EXPR, 3, 1, PROP_EXPR)
DEF_OPCODE(RESHAPE_KV, reshape_kv, OPR_CAT::EXPR, 3, 1, PROP_EXPR)
DEF_OPCODE(REPEAT_KV, repeat_kv, OPR_CAT::EXPR, 2, 1, PROP_EXPR)
DEF_OPCODE(TRANSPOSE, transpose, OPR_CAT::EXPR, 3, 1, PROP_EXPR)
DEF_OPCODE(SQRT, sqrt, OPR_CAT::EXPR, 1, 1, PROP_EXPR)
DEF_OPCODE(SOFTMAX, softmax, OPR_CAT::EXPR, 1, 1, PROP_EXPR)
DEF_OPCODE(DIVIDE, divide, OPR_CAT::EXPR, 2, 1, PROP_EXPR)
