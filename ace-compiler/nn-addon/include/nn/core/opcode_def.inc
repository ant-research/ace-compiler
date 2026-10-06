//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

/**
 * @brief Defines all NN OPCODE in this file with the following format:
 *   DEF_OPCODE(NAME, name, kid_num, fld_num, property)
 *
 */

DEF_OPCODE(INVALID, invalid, 0, 0, 0)
DEF_OPCODE(ADD, add, 2, 0, PROP_EXPR | PROP_ATTR)
DEF_OPCODE(AVERAGE_POOL, average_pool, 1, 0, PROP_EXPR | PROP_ATTR)
DEF_OPCODE(CONV, conv, 3, 0, PROP_EXPR | PROP_ATTR)
DEF_OPCODE(FLATTEN, flatten, 1, 0, PROP_EXPR | PROP_ATTR)
DEF_OPCODE(GEMM, gemm, 3, 0, PROP_EXPR | PROP_ATTR)
DEF_OPCODE(GLOBAL_AVERAGE_POOL, global_average_pool, 1, 0, PROP_EXPR)
DEF_OPCODE(MAX_POOL, max_pool, 1, 0, PROP_EXPR | PROP_ATTR)
DEF_OPCODE(MUL, mul, 2, 0, PROP_EXPR)
DEF_OPCODE(RELU, relu, 1, 0, PROP_EXPR | PROP_ATTR)
DEF_OPCODE(RESHAPE, reshape, 2, 0, PROP_EXPR | PROP_ATTR)
DEF_OPCODE(STRIDED_SLICE, strided_slice, 4, 0, PROP_EXPR | PROP_ATTR)
DEF_OPCODE(SUB, sub, 2, 0, PROP_EXPR)
DEF_OPCODE(RMSNORM, rmsnorm, 2, 0, PROP_EXPR)
DEF_OPCODE(MATMUL, matmul, 2, 0, PROP_EXPR)
DEF_OPCODE(ROPE_ROTARY, rope_rotary, 5, 0, PROP_EXPR)
DEF_OPCODE(RESHAPE_KV, reshape_kv, 3, 0, PROP_EXPR)
DEF_OPCODE(REPEAT_KV, repeat_kv, 3, 0, PROP_EXPR)
DEF_OPCODE(TRANSPOSE, transpose, 3, 0, PROP_EXPR)
DEF_OPCODE(SQRT, sqrt, 1, 0, PROP_EXPR)
DEF_OPCODE(DIVIDE, divide, 2, 0, PROP_EXPR)
DEF_OPCODE(CONCAT, concat, 2, 0, PROP_EXPR | PROP_ATTR)
DEF_OPCODE(UPDATE_KCACHE, update_kcache, 3, 0, PROP_EXPR)
DEF_OPCODE(UPDATE_VCACHE, update_vcache, 5, 0, PROP_EXPR)
DEF_OPCODE(BMM, bmm, 5, 0, PROP_EXPR)
DEF_OPCODE(SWIGLU, swiglu, 2, 0, PROP_EXPR)
DEF_OPCODE(ROPE, rope, 6, 0, PROP_EXPR)
DEF_OPCODE(UPD_KV, upd_kv, 3, 0, PROP_EXPR)
DEF_OPCODE(COMP_MHA, comp_mha, 5, 0, PROP_EXPR)
DEF_OPCODE(SOFTMAX, softmax, 3, 0, PROP_EXPR)
DEF_OPCODE(UPD_MHA, upd_mha, 5, 0, PROP_EXPR)
DEF_OPCODE(ACCUM, accum, 2, 0, PROP_EXPR)
DEF_OPCODE(SILU, silu, 1, 0, PROP_EXPR)
