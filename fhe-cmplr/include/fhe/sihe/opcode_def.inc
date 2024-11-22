//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef SIHE_OPCODE
  #define SIHE_OPCODE(NAME, name, category, kid_num, fld_num, property)
#endif

SIHE_OPCODE(ROTATE, rotate, OPR_CAT::EXPR, 2, 0, PROP_EXPR | PROP_TYPE | PROP_ATTR)
SIHE_OPCODE(ADD,    add,    OPR_CAT::EXPR, 2, 0, PROP_EXPR | PROP_TYPE)
SIHE_OPCODE(SUB,    sub,    OPR_CAT::EXPR, 2, 0, PROP_EXPR | PROP_TYPE)
SIHE_OPCODE(MUL,    mul,    OPR_CAT::EXPR, 2, 0, PROP_EXPR | PROP_TYPE)
SIHE_OPCODE(NEG,    neg,    OPR_CAT::EXPR, 1, 0, PROP_EXPR | PROP_TYPE)

// opcode for runtime validate
SIHE_OPCODE(ROTATE_MSG, rotate_msg, OPR_CAT::EXPR, 2, 1, PROP_EXPR | PROP_TYPE | PROP_LIB_CALL)
SIHE_OPCODE(ADD_MSG,    add_msg,    OPR_CAT::EXPR, 2, 1, PROP_EXPR | PROP_TYPE | PROP_LIB_CALL)
SIHE_OPCODE(MUL_MSG,    mul_msg,    OPR_CAT::EXPR, 2, 1, PROP_EXPR | PROP_TYPE | PROP_LIB_CALL)
SIHE_OPCODE(RELU_MSG,   relu_msg,   OPR_CAT::EXPR, 1, 1, PROP_EXPR | PROP_TYPE | PROP_ATTR | PROP_LIB_CALL)

// encode opcode transform vector to plaintext:
SIHE_OPCODE(ENCODE,     encode,     OPR_CAT::EXPR, 2, 0, PROP_EXPR | PROP_TYPE)

SIHE_OPCODE(BOOTSTRAP,  bootstrap,  OPR_CAT::EXPR, 1, 0, PROP_EXPR | PROP_TYPE)

SIHE_OPCODE(BOOTSTRAP_MSG, bootstrap_msg, OPR_CAT::EXPR, 1, 0, PROP_EXPR | PROP_ATTR | PROP_LIB_CALL)

#undef SIHE_OPCODE
