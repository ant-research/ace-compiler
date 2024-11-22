//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef CKKS_OPCODE
  #define CKKS_OPCODE(NAME, name, category, kid_num, fld_num, property)
#endif

CKKS_OPCODE(ROTATE, rotate, OPR_CAT::EXPR, 2, 0, PROP_EXPR | PROP_TYPE | PROP_ATTR | PROP_LIB_CALL)
CKKS_OPCODE(ADD,    add,    OPR_CAT::EXPR, 2, 0, PROP_EXPR | PROP_TYPE | PROP_ATTR | PROP_LIB_CALL)
CKKS_OPCODE(SUB,    sub,    OPR_CAT::EXPR, 2, 0, PROP_EXPR | PROP_TYPE | PROP_ATTR | PROP_LIB_CALL)
CKKS_OPCODE(MUL,    mul,    OPR_CAT::EXPR, 2, 0, PROP_EXPR | PROP_TYPE | PROP_ATTR | PROP_LIB_CALL)
CKKS_OPCODE(NEG,    neg,    OPR_CAT::EXPR, 1, 0, PROP_EXPR | PROP_TYPE | PROP_ATTR | PROP_LIB_CALL)

// encode opcode transform vector to plaintext:
CKKS_OPCODE(ENCODE,     encode,     OPR_CAT::EXPR, 4, 0, PROP_EXPR | PROP_TYPE | PROP_LIB_CALL)

// scale management opcodes:
CKKS_OPCODE(RESCALE,    rescale,    OPR_CAT::EXPR, 1, 0, PROP_EXPR | PROP_TYPE | PROP_ATTR | PROP_LIB_CALL)
CKKS_OPCODE(UPSCALE,    upscale,    OPR_CAT::EXPR, 2, 0, PROP_EXPR | PROP_TYPE | PROP_ATTR | PROP_LIB_CALL)
CKKS_OPCODE(MOD_SWITCH, mod_switch, OPR_CAT::EXPR, 1, 0, PROP_EXPR | PROP_TYPE | PROP_ATTR | PROP_LIB_CALL)
CKKS_OPCODE(RELIN,      relin,      OPR_CAT::EXPR, 1, 0, PROP_EXPR | PROP_TYPE | PROP_ATTR | PROP_LIB_CALL)
CKKS_OPCODE(BOOTSTRAP,  bootstrap,  OPR_CAT::EXPR, 1, 0, PROP_EXPR | PROP_TYPE | PROP_ATTR | PROP_LIB_CALL)

// opcodes get scale/level/batch_size of ciphertext
CKKS_OPCODE(SCALE,      scale,      OPR_CAT::EXPR, 1, 0, PROP_EXPR | PROP_TYPE)
CKKS_OPCODE(LEVEL,      level,      OPR_CAT::EXPR, 1, 0, PROP_EXPR | PROP_TYPE)
CKKS_OPCODE(BATCH_SIZE, batch_size, OPR_CAT::EXPR, 1, 0, PROP_EXPR | PROP_TYPE)

#undef CKKS_OPCODE
