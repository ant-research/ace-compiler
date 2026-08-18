//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef SIHE_OPCODE
  #define SIHE_OPCODE(NAME, name, kid_num, fld_num, property)
#endif

SIHE_OPCODE(ROTATE, rotate, 2, 0, PROP_EXPR | PROP_TYPE | PROP_ATTR)
SIHE_OPCODE(ADD,    add,    2, 0, PROP_EXPR | PROP_TYPE)
SIHE_OPCODE(SUB,    sub,    2, 0, PROP_EXPR | PROP_TYPE)
SIHE_OPCODE(MUL,    mul,    2, 0, PROP_EXPR | PROP_TYPE)
SIHE_OPCODE(NEG,    neg,    1, 0, PROP_EXPR | PROP_TYPE)

// opcode for runtime validate
SIHE_OPCODE(ROTATE_MSG, rotate_msg, 2, 0, PROP_EXPR | PROP_TYPE | PROP_LIB_CALL)
SIHE_OPCODE(ADD_MSG,    add_msg,    2, 0, PROP_EXPR | PROP_TYPE | PROP_LIB_CALL)
SIHE_OPCODE(MUL_MSG,    mul_msg,    2, 0, PROP_EXPR | PROP_TYPE | PROP_LIB_CALL)
SIHE_OPCODE(RELU_MSG,   relu_msg,   1, 0, PROP_EXPR | PROP_TYPE | PROP_ATTR | PROP_LIB_CALL)

// encode opcode transform vector to plaintext:
SIHE_OPCODE(ENCODE,     encode,     2, 0, PROP_EXPR | PROP_ATTR | PROP_TYPE)

SIHE_OPCODE(BOOTSTRAP,  bootstrap,  1, 0, PROP_EXPR | PROP_TYPE)

SIHE_OPCODE(BOOTSTRAP_MSG, bootstrap_msg, 1, 0, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)

#undef SIHE_OPCODE
