//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

/**
 * @brief Defines all POLYNOMIAL OPCODE in this file with the following format:
 *   DEF_OPCODE(NAME, name, category, kid_num, fld_num, property)
 *
 */

DEF_OPCODE(INVALID, invalid, OPR_CAT::EXPR, 0, 0, PROP_EXPR)
// memory related op
DEF_OPCODE(ALLOC, alloc, OPR_CAT::EXPR, 3, 1, PROP_EXPR)
DEF_OPCODE(FREE, free, OPR_CAT::STMT, 1, 0, PROP_STMT)
DEF_OPCODE(INIT_CIPH_SAME_SCALE, init_ciph_same_scale, OPR_CAT::STMT, 3, 0, PROP_STMT)
DEF_OPCODE(INIT_CIPH_UP_SCALE, init_ciph_up_scale, OPR_CAT::STMT, 3, 0, PROP_STMT)
DEF_OPCODE(INIT_CIPH_DOWN_SCALE, init_ciph_down_scale, OPR_CAT::STMT, 2, 0, PROP_STMT)
// polynomial operation related op
DEF_OPCODE(NTT, ntt, OPR_CAT::EXPR, 1, 1, PROP_EXPR)
DEF_OPCODE(INTT, intt, OPR_CAT::EXPR, 1, 1, PROP_EXPR)
DEF_OPCODE(ADD, add, OPR_CAT::EXPR, 2, 1, PROP_EXPR)
DEF_OPCODE(SUB, sub, OPR_CAT::EXPR, 2, 1, PROP_EXPR)
DEF_OPCODE(MUL, mul, OPR_CAT::EXPR, 2, 1, PROP_EXPR)
DEF_OPCODE(ADD_EXT, add_ext, OPR_CAT::EXPR, 2, 1, PROP_EXPR)
DEF_OPCODE(SUB_EXT, sub_ext, OPR_CAT::EXPR, 2, 1, PROP_EXPR)
DEF_OPCODE(MUL_EXT, mul_ext, OPR_CAT::EXPR, 2, 1, PROP_EXPR)
DEF_OPCODE(RESCALE, rescale, OPR_CAT::EXPR, 1, 1, PROP_EXPR)
DEF_OPCODE(ROTATE, rotate, OPR_CAT::EXPR, 2, 1, PROP_EXPR)
DEF_OPCODE(DECOMP, decomp, OPR_CAT::EXPR, 2, 1, PROP_EXPR)
DEF_OPCODE(DECOMP_MODUP, decomp_modup, OPR_CAT::EXPR, 2, 1, PROP_EXPR)
DEF_OPCODE(AUTO_ORDER, auto_order, OPR_CAT::EXPR, 1, 0, PROP_EXPR)
DEF_OPCODE(MOD_UP, mod_up, OPR_CAT::EXPR, 2, 1, PROP_EXPR)
DEF_OPCODE(MOD_DOWN, mod_down, OPR_CAT::EXPR, 1, 1, PROP_EXPR)
DEF_OPCODE(EXTRACT, extract, OPR_CAT::EXPR, 3, 1, PROP_EXPR)
// polynomial field access ops
DEF_OPCODE(COEFFS, coeffs, OPR_CAT::LDST, 2, 0, PROP_EXPR)
DEF_OPCODE(SET_COEFFS, set_coeffs, OPR_CAT::LDST, 3, 0, PROP_STMT | PROP_LIB_CALL)
DEF_OPCODE(LEVEL, level, OPR_CAT::EXPR, 1, 1, PROP_EXPR)
DEF_OPCODE(SET_LEVEL, set_level, OPR_CAT::STMT, 2, 1, PROP_STMT)
DEF_OPCODE(NUM_P, num_p, OPR_CAT::EXPR, 1, 1, PROP_EXPR)
DEF_OPCODE(NUM_ALLOC, num_alloc, OPR_CAT::EXPR, 1, 1, PROP_EXPR)
DEF_OPCODE(NUM_DECOMP, num_decomp, OPR_CAT::EXPR, 1, 0, PROP_EXPR)
// scheme related op
DEF_OPCODE(DEGREE, degree, OPR_CAT::EXPR, 0, 0, PROP_EXPR)
DEF_OPCODE(Q_MODULUS, q_modulus, OPR_CAT::EXPR, 0, 0, PROP_EXPR)
DEF_OPCODE(P_MODULUS, p_modulus, OPR_CAT::EXPR, 0, 0, PROP_EXPR)
// key related op
DEF_OPCODE(SWK, swk, OPR_CAT::EXPR, 2, 0, PROP_EXPR)
DEF_OPCODE(PK0_AT, pk0_at, OPR_CAT::EXPR, 2, 0, PROP_EXPR)
DEF_OPCODE(PK1_AT, pk1_at, OPR_CAT::EXPR, 2, 0, PROP_EXPR)
// hardware related op
DEF_OPCODE(HW_NTT, hw_ntt, OPR_CAT::EXPR, 2, 1, PROP_EXPR | PROP_LIB_CALL)
DEF_OPCODE(HW_INTT, hw_intt, OPR_CAT::EXPR, 2, 1, PROP_EXPR | PROP_LIB_CALL)
DEF_OPCODE(HW_MODADD, hw_modadd, OPR_CAT::EXPR, 3, 1, PROP_EXPR | PROP_LIB_CALL)
DEF_OPCODE(HW_MODSUB, hw_modsub, OPR_CAT::EXPR, 3, 1, PROP_EXPR | PROP_LIB_CALL)
DEF_OPCODE(HW_MODMUL, hw_modmul, OPR_CAT::EXPR, 3, 1, PROP_EXPR | PROP_LIB_CALL)
DEF_OPCODE(HW_ROTATE, hw_rotate, OPR_CAT::EXPR, 3, 1, PROP_EXPR | PROP_LIB_CALL)

