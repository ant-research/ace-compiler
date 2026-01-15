//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "poly_ir_gen.h"

#include "air/base/container.h"
#include "air/core/opcode.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/poly/ir_gen.h"
namespace fhe {
namespace poly {

using namespace air::base;
using namespace fhe::core;

NODE_PTR POLY_IR_GEN::New_poly_node(OPCODE opcode, CONST_TYPE_PTR rtype,
                                    const SPOS& spos) {
  CONTAINER* cont = Container();
  CMPLR_ASSERT(cont, "null scope");

  return cont->New_cust_node(air::base::OPCODE(POLYNOMIAL_DID, opcode), rtype,
                             spos);
}

STMT_PTR POLY_IR_GEN::New_poly_stmt(OPCODE opcode, const SPOS& spos) {
  CONTAINER* cont = Container();
  CMPLR_ASSERT(cont, "null scope");

  return cont->New_cust_stmt(air::base::OPCODE(POLYNOMIAL_DID, opcode), spos);
}

NODE_PAIR POLY_IR_GEN::New_ciph_poly_load(CONST_VAR v_ciph, bool is_rns,
                                          const SPOS& spos) {
  if (!Lower_ctx()->Is_cipher_type(v_ciph.Type_id())) {
    std::cerr << "[DEBUG POLY] v_ciph type_id: 0x" << std::hex
              << v_ciph.Type_id().Value() << ", cipher_type_id: 0x"
              << Lower_ctx()->Get_cipher_type_id().Value()
              << ", cipher3_type_id: 0x"
              << Lower_ctx()->Get_cipher3_type_id().Value();
    if (v_ciph.Is_preg()) {
      std::cerr << " (PREG)";
    } else if (!v_ciph.Is_null()) {
      std::cerr << " (VAR: " << v_ciph.Addr_var()->Name()->Char_str() << ")";
    }
    std::cerr << std::dec << "\n";
  }

  CMPLR_ASSERT(Lower_ctx()->Is_cipher_type(v_ciph.Type_id()),
               "v_ciph is not ciphertext");

  NODE_PTR n_c0 = New_poly_load(v_ciph, 0, spos);
  NODE_PTR n_c1 = New_poly_load(v_ciph, 1, spos);
  if (is_rns) {
    CONST_VAR& v_rns_idx     = Get_var(VAR_RNS_IDX, spos);
    NODE_PTR   n_c0_rns_load = New_poly_load_at_level(n_c0, v_rns_idx);
    NODE_PTR   n_c1_rns_load = New_poly_load_at_level(n_c1, v_rns_idx);
    return std::make_pair(n_c0_rns_load, n_c1_rns_load);
  } else {
    return std::make_pair(n_c0, n_c1);
  }
}

NODE_PTR POLY_IR_GEN::New_plain_poly_load(CONST_VAR v_plain, bool is_rns,
                                          const SPOS& spos) {
  CMPLR_ASSERT(Lower_ctx()->Is_plain_type(v_plain.Type_id()),
               "v_plain is not plaintext");

  NODE_PTR n_plain = New_poly_load(v_plain, false, spos);
  if (is_rns) {
    CONST_VAR& v_rns_idx       = Get_var(VAR_RNS_IDX, spos);
    NODE_PTR   n_poly_rns_load = New_poly_load_at_level(n_plain, v_rns_idx);
    return n_poly_rns_load;
  } else {
    return n_plain;
  }
}

STMT_PTR POLY_IR_GEN::New_plain_poly_store(CONST_VAR v_plain, NODE_PTR node,
                                           bool is_rns, const SPOS& spos) {
  if (is_rns) {
    CONST_VAR& v_rns_idx = Get_var(VAR_RNS_IDX, spos);
    NODE_PTR   lhs       = New_plain_poly_load(v_plain, false, spos);
    return New_poly_store_at_level(lhs, node, v_rns_idx);
  } else {
    return New_poly_store(node, v_plain, 0, spos);
  }
}

STMT_PAIR POLY_IR_GEN::New_ciph_poly_store(CONST_VAR v_ciph, NODE_PTR n_c0,
                                           NODE_PTR n_c1, bool is_rns,
                                           const SPOS& spos) {
  if (!Lower_ctx()->Is_cipher_type(v_ciph.Type_id())) {
    std::cerr << "[DEBUG POLY] store v_ciph type_id: 0x" << std::hex
              << v_ciph.Type_id().Value() << ", cipher_type_id: 0x"
              << Lower_ctx()->Get_cipher_type_id().Value();
    if (v_ciph.Is_preg()) {
      std::cerr << " (PREG)";
    } else if (!v_ciph.Is_null()) {
      std::cerr << " (VAR: " << v_ciph.Addr_var()->Name()->Char_str() << ")";
    }
    std::cerr << std::dec << "\n";
  }
  CMPLR_ASSERT(Lower_ctx()->Is_cipher_type(v_ciph.Type_id()),
               "v_ciph is not ciphertext");

  STMT_PTR s_c0;
  STMT_PTR s_c1;
  if (is_rns) {
    CONST_VAR& v_rns_idx  = Get_var(VAR_RNS_IDX, spos);
    NODE_PAIR  n_lhs_pair = New_ciph_poly_load(v_ciph, false, spos);
    s_c0 = New_poly_store_at_level(n_lhs_pair.first, n_c0, v_rns_idx);
    s_c1 = New_poly_store_at_level(n_lhs_pair.second, n_c1, v_rns_idx);
  } else {
    s_c0 = New_poly_store(n_c0, v_ciph, 0, spos);
    s_c1 = New_poly_store(n_c1, v_ciph, 1, spos);
  }
  return std::make_pair(s_c0, s_c1);
}

NODE_PTR POLY_IR_GEN::New_poly_load(CONST_VAR var, uint32_t load_idx,
                                    const SPOS& spos) {
  FIELD_PTR fld_ptr = Get_poly_fld(var, load_idx);
  if (var.Is_sym()) {
    return Container()->New_ldf(var.Addr_var(), fld_ptr, spos);
  } else {
    return Container()->New_ldpf(var.Preg_var(), fld_ptr, spos);
  }
}

NODE_TRIPLE POLY_IR_GEN::New_ciph3_poly_load(CONST_VAR v_ciph3, bool is_rns,
                                             const SPOS& spos) {
  CMPLR_ASSERT(Lower_ctx()->Is_cipher3_type(v_ciph3.Type_id()),
               "v_ciph3 is not CIPHER3 type");

  NODE_PTR n_c0 = New_poly_load(v_ciph3, 0, spos);
  NODE_PTR n_c1 = New_poly_load(v_ciph3, 1, spos);
  NODE_PTR n_c2 = New_poly_load(v_ciph3, 2, spos);
  if (is_rns) {
    CONST_VAR& v_rns_idx     = Get_var(VAR_RNS_IDX, spos);
    NODE_PTR   n_c0_rns_load = New_poly_load_at_level(n_c0, v_rns_idx);
    NODE_PTR   n_c1_rns_load = New_poly_load_at_level(n_c1, v_rns_idx);
    NODE_PTR   n_c2_rns_load = New_poly_load_at_level(n_c2, v_rns_idx);
    return std::make_tuple(n_c0_rns_load, n_c1_rns_load, n_c2_rns_load);
  } else {
    return std::make_tuple(n_c0, n_c1, n_c2);
  }
}

STMT_TRIPLE POLY_IR_GEN::New_ciph3_poly_store(CONST_VAR v_ciph3, NODE_PTR n_c0,
                                              NODE_PTR n_c1, NODE_PTR n_c2,
                                              bool is_rns, const SPOS& spos) {
  CMPLR_ASSERT(Lower_ctx()->Is_cipher3_type(v_ciph3.Type_id()),
               "v_ciph3 is not CIPHER3 type");

  STMT_PTR s_c0;
  STMT_PTR s_c1;
  STMT_PTR s_c2;
  if (is_rns) {
    CONST_VAR&  v_rns_idx = Get_var(VAR_RNS_IDX, spos);
    NODE_TRIPLE n_lhs_tpl = New_ciph3_poly_load(v_ciph3, false, spos);
    s_c0 = New_poly_store_at_level(std::get<0>(n_lhs_tpl), n_c0, v_rns_idx);
    s_c1 = New_poly_store_at_level(std::get<1>(n_lhs_tpl), n_c1, v_rns_idx);
    s_c2 = New_poly_store_at_level(std::get<2>(n_lhs_tpl), n_c2, v_rns_idx);
  } else {
    s_c0 = New_poly_store(n_c0, v_ciph3, 0, spos);
    s_c1 = New_poly_store(n_c1, v_ciph3, 1, spos);
    s_c2 = New_poly_store(n_c2, v_ciph3, 2, spos);
  }
  return std::make_tuple(s_c0, s_c1, s_c2);
}

STMT_PTR POLY_IR_GEN::New_poly_store(NODE_PTR store_val, CONST_VAR var,
                                     uint32_t store_idx, const SPOS& spos) {
  FIELD_PTR fld_ptr = Get_poly_fld(var, store_idx);
  if (var.Is_sym()) {
    STMT_PTR stmt =
        Container()->New_stf(store_val, var.Addr_var(), fld_ptr, spos);
    return stmt;
  } else {
    STMT_PTR stmt =
        Container()->New_stpf(store_val, var.Preg_var(), fld_ptr, spos);
    return stmt;
  }
}

NODE_PTR POLY_IR_GEN::New_poly_load_at_level(NODE_PTR  n_poly,
                                             CONST_VAR v_level) {
  CONTAINER* cont = Container();
  CMPLR_ASSERT(cont, "null scope");

  NODE_PTR n_level = New_var_load(v_level, n_poly->Spos());
  return New_poly_load_at_level(n_poly, n_level);
}

NODE_PTR POLY_IR_GEN::New_poly_load_at_level(NODE_PTR n_poly,
                                             NODE_PTR n_level) {
  AIR_ASSERT_MSG(n_poly->Rtype() == Get_type(POLY, n_poly->Spos()),
                 "invalid type");

  NODE_PTR ret = New_poly_node(COEFFS, Get_type(INT64_PTR, n_poly->Spos()),
                               n_poly->Spos());
  ret->Set_child(0, n_poly);
  ret->Set_child(1, n_level);
  return ret;
}

STMT_PTR POLY_IR_GEN::New_poly_store_at_level(NODE_PTR n_lhs, NODE_PTR n_rhs,
                                              CONST_VAR v_level) {
  CONTAINER* cont = Container();
  CMPLR_ASSERT(cont, "null scope");

  NODE_PTR n_level = New_var_load(v_level, n_rhs->Spos());
  return New_poly_store_at_level(n_lhs, n_rhs, n_level);
}

STMT_PTR POLY_IR_GEN::New_poly_store_at_level(NODE_PTR n_lhs, NODE_PTR n_rhs,
                                              NODE_PTR n_level) {
  CMPLR_ASSERT(n_lhs->Rtype() == Get_type(POLY, n_lhs->Spos()), "invalid type");
  CMPLR_ASSERT(n_rhs->Rtype() == Get_type(INT64_PTR, n_rhs->Spos()),
               "invalid type");

  STMT_PTR stmt = New_poly_stmt(SET_COEFFS, n_rhs->Spos());
  stmt->Node()->Set_child(0, n_lhs);
  stmt->Node()->Set_child(1, n_level);
  stmt->Node()->Set_child(2, n_rhs);

  // Generate through existing store with variable offset?
  // (&lhs + level * (N * sizeof(uint64_t))) = rhs
  //   LD rhs
  //     LDA lhs
  //       LD level
  //       LDC N * sizeof(uint64_t)
  //     MUL
  //   ADD
  // ISTORE
  return stmt;
}

NODE_PTR POLY_IR_GEN::New_encode(NODE_PTR n_data, NODE_PTR n_len,
                                 NODE_PTR n_scale, NODE_PTR n_level,
                                 const SPOS& spos) {
  CONTAINER* cntr = Container();
  NODE_PTR   n_encode =
      cntr->New_cust_node(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                            fhe::ckks::CKKS_OPERATOR::ENCODE),
                          Get_type(PLAIN, spos), spos);
  n_encode->Set_child(0, n_data);
  n_encode->Set_child(1, n_len);
  n_encode->Set_child(2, n_scale);
  n_encode->Set_child(3, n_level);

  return n_encode;
}

// Type mapping rules from CKKS to HPOLY
// | CKKS_OP        | HPOLY_OP | opnd0_ty | opnd1_ty |
// | -------------- | -------- | -------- | -------- |
// | Add/Sub_ciph   | Add/Sub  | ciph     | ciph     |
// | Add/Sub_plain  | Add/Sub  | ciph     | plain    |
// | Add/Sub_double | Add/Sub  | ciph     | int64[]  |
// | Rescale        | Add/Sub  | ciph     | int64    |
// | Mul_ciph       | Mul      | ciph     | ciph     |
// | Mul_plain      | Mul      | ciph     | plain    |
// | Mul_int        | Mul      | ciph     | int64    |
// | Mul_double     | Mul      | ciph     | int64[]  |
NODE_PTR POLY_IR_GEN::New_bin_arith(OPCODE opc, NODE_PTR opnd0, NODE_PTR opnd1,
                                    const air::base::SPOS& spos) {
  CMPLR_ASSERT(opnd0->Rtype() == Get_type(POLY, spos), "invalid type");
  // opnd1 type can be POLY|int|int*
  CMPLR_ASSERT((opnd1->Rtype() == Get_type(POLY) ||
                opnd1->Rtype() == Get_type(INT64_PTR) ||
                opnd1->Rtype() == Get_type(UINT64_PTR) ||
                Is_int_array(opnd1->Rtype()) || opnd1->Rtype()->Is_int()),
               "invalid type");
  NODE_PTR bin_arith = New_poly_node(opc, opnd0->Rtype(), spos);
  bin_arith->Set_child(0, opnd0);
  bin_arith->Set_child(1, opnd1);
  return bin_arith;
}

NODE_PTR POLY_IR_GEN::New_poly_mac(NODE_PTR opnd0, NODE_PTR opnd1,
                                   NODE_PTR opnd2, bool is_ext,
                                   const SPOS& spos) {
  CMPLR_ASSERT(opnd0->Rtype() == Get_type(POLY, spos), "invalid type");
  CMPLR_ASSERT(opnd1->Rtype() == Get_type(POLY, spos), "invalid type");
  // opnd2 type can be POLY|int|int*
  CMPLR_ASSERT(
      (opnd2->Rtype() == Get_type(POLY) ||
       opnd2->Rtype() == Get_type(INT64_PTR) ||
       opnd2->Rtype() == Get_type(UINT64_PTR) || Is_int_array(opnd2->Rtype())),
      "invalid type");

  NODE_PTR n_mac = New_poly_node(is_ext ? MAC_EXT : MAC, opnd0->Rtype(), spos);
  n_mac->Set_child(0, opnd0);
  n_mac->Set_child(1, opnd1);
  n_mac->Set_child(2, opnd2);
  return n_mac;
}

NODE_PTR POLY_IR_GEN::New_poly_rotate(NODE_PTR opnd0, NODE_PTR opnd1,
                                      const SPOS& spos) {
  CMPLR_ASSERT(opnd0->Rtype() == Get_type(POLY, spos), "invalid type");
  CMPLR_ASSERT(opnd1->Rtype()->Is_int(), "invalid type");

  NODE_PTR n_res = New_poly_node(ROTATE, opnd0->Rtype(), spos);
  n_res->Set_child(0, opnd0);
  n_res->Set_child(1, opnd1);
  return n_res;
}

STMT_PTR POLY_IR_GEN::New_init_ciph_same_scale(NODE_PTR lhs, NODE_PTR opnd0,
                                               NODE_PTR    opnd1,
                                               const SPOS& spos) {
  STMT_PTR stmt      = New_poly_stmt(INIT_CIPH_SAME_SCALE, spos);
  NODE_PTR stmt_node = stmt->Node();
  stmt_node->Set_child(0, lhs);
  stmt_node->Set_child(1, opnd0);
  stmt_node->Set_child(2, opnd1);
  return stmt;
}

STMT_PTR POLY_IR_GEN::New_init_ciph_up_scale(NODE_PTR lhs, NODE_PTR opnd0,
                                             NODE_PTR opnd1, const SPOS& spos) {
  STMT_PTR stmt      = New_poly_stmt(INIT_CIPH_UP_SCALE, spos);
  NODE_PTR stmt_node = stmt->Node();
  stmt_node->Set_child(0, lhs);
  stmt_node->Set_child(1, opnd0);
  stmt_node->Set_child(2, opnd1);
  return stmt;
}

STMT_PTR POLY_IR_GEN::New_init_ciph_down_scale(NODE_PTR res, NODE_PTR opnd,
                                               const SPOS& spos) {
  CMPLR_ASSERT(res->Rtype_id() == opnd->Rtype_id() &&
                   (Lower_ctx()->Is_cipher_type(res->Rtype_id()) ||
                    Lower_ctx()->Is_cipher3_type(res->Rtype_id())),
               "down scale opnds are not ciphertext type");
  STMT_PTR stmt      = New_poly_stmt(INIT_CIPH_DOWN_SCALE, spos);
  NODE_PTR stmt_node = stmt->Node();
  stmt_node->Set_child(0, res);
  stmt_node->Set_child(1, opnd);
  return stmt;
}

// This is a tempory place to insert init node
// A better design is: postpone to CG IR phase
STMT_PTR POLY_IR_GEN::New_init_ciph(CONST_VAR v_parent, NODE_PTR node) {
  TYPE_ID tid = node->Rtype_id();
  AIR_ASSERT_MSG(Lower_ctx()->Is_cipher_type(tid) ||
                     Lower_ctx()->Is_cipher3_type(tid) ||
                     node->Opcode() == air::core::OPC_ZERO,
                 "not ciphertext type");
  CONST_VAR& v_res = v_parent.Is_null() ? Node_var(node) : v_parent;
  SPOS       spos  = node->Spos();
  if (node->Domain() == fhe::ckks::CKKS_DOMAIN::ID) {
    switch (node->Operator()) {
      case fhe::ckks::CKKS_OPERATOR::ADD:
      case fhe::ckks::CKKS_OPERATOR::SUB:
      case fhe::ckks::CKKS_OPERATOR::MUL: {
        AIR_ASSERT(Has_node_var(node->Child(0)) &&
                   Has_node_var(node->Child(1)));
        CONST_VAR& v_opnd0 = Node_var(node->Child(0));
        CONST_VAR& v_opnd1 = Node_var(node->Child(1));
        if (v_res != v_opnd0 || v_res != v_opnd1) {
          NODE_PTR n_res   = New_var_load(v_res, spos);
          NODE_PTR n_opnd0 = New_var_load(v_opnd0, spos);
          NODE_PTR n_opnd1 = New_var_load(v_opnd1, spos);
          if (node->Operator() == fhe::ckks::CKKS_OPERATOR::MUL) {
            return New_init_ciph_up_scale(n_res, n_opnd0, n_opnd1, spos);
          } else
            return New_init_ciph_same_scale(n_res, n_opnd0, n_opnd1, spos);
        }
        break;
      }
      case fhe::ckks::CKKS_OPERATOR::RELIN:
      case fhe::ckks::CKKS_OPERATOR::ROTATE:
      case fhe::ckks::CKKS_OPERATOR::MODSWITCH: {
        AIR_ASSERT(Has_node_var(node->Child(0)));
        CONST_VAR& v_opnd0 = Node_var(node->Child(0));
        if (v_res != v_opnd0) {
          NODE_PTR n_res   = New_var_load(v_res, spos);
          NODE_PTR n_opnd0 = New_var_load(v_opnd0, spos);
          TYPE_PTR t_opnd1 = Glob_scope()->New_ptr_type(
              Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_U64)->Id(),
              POINTER_KIND::FLAT64);
          NODE_PTR n_opnd1 = Container()->New_zero(t_opnd1, spos);
          return New_init_ciph_same_scale(n_res, n_opnd0, n_opnd1, spos);
        }
        break;
      }
      case fhe::ckks::CKKS_OPERATOR::RESCALE: {
        AIR_ASSERT(Has_node_var(node->Child(0)));
        CONST_VAR& v_opnd0 = Node_var(node->Child(0));
        NODE_PTR   n_res   = New_var_load(v_res, spos);
        NODE_PTR   n_opnd0 = New_var_load(v_opnd0, spos);
        return New_init_ciph_down_scale(n_res, n_opnd0, spos);
      }
      default:
        CMPLR_ASSERT(false, "not supported operator");
    }
  } else if (node->Opcode() == air::core::OPC_LD ||
             node->Opcode() == air::core::OPC_LDP) {
    CONST_VAR& v_opnd0 = Node_var(node);
    if (v_res != v_opnd0) {
      NODE_PTR n_res   = New_var_load(v_res, spos);
      NODE_PTR n_opnd0 = New_var_load(v_opnd0, spos);
      TYPE_PTR t_opnd1 = Glob_scope()->New_ptr_type(
          Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_U64)->Id(),
          POINTER_KIND::FLAT64);
      NODE_PTR n_opnd1 = Container()->New_zero(t_opnd1, spos);
      return New_init_ciph_same_scale(n_res, n_opnd0, n_opnd1, spos);
    }
  } else if (node->Opcode() == air::core::OPC_ILD) {
    NODE_PTR n_res   = New_var_load(v_res, spos);
    NODE_PTR n_opnd0 = Container()->Clone_node_tree(node);
    TYPE_PTR t_opnd1 = Glob_scope()->New_ptr_type(
        Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_U64)->Id(),
        POINTER_KIND::FLAT64);
    NODE_PTR n_opnd1 = Container()->New_zero(t_opnd1, spos);
    return New_init_ciph_same_scale(n_res, n_opnd0, n_opnd1, spos);
  } else if (node->Opcode() == air::core::OPC_ZERO) {
    // nothing to do for ciph = 0
    return STMT_PTR();
  } else {
    AIR_ASSERT_MSG(false, "not supported operator");
  }
  return STMT_PTR();
}

STMT_PTR POLY_IR_GEN::New_init_poly_by_opnd(CONST_VAR v_res, NODE_PTR node,
                                            bool is_ext, const SPOS& spos) {
  CMPLR_ASSERT(Lower_ctx()->Is_rns_poly_type(node->Rtype_id()),
               "not poly type");
  AIR_ASSERT(node->Num_child() >= 1);

  NODE_PTR n_res   = New_var_load(v_res, spos);
  NODE_PTR n_opnd0 = New_var_load(Node_var(node->Child(0)), spos);
  NODE_PTR n_opnd1 = Null_ptr;
  if (node->Num_child() == 2 &&
      Lower_ctx()->Is_rns_poly_type(node->Child(1)->Rtype_id())) {
    n_opnd1 = New_var_load(Node_var(node->Child(1)), spos);
  }
  return New_init_poly_by_opnd(n_res, n_opnd0, n_opnd1, is_ext, spos);
}

STMT_PTR POLY_IR_GEN::New_init_poly_by_opnd(NODE_PTR n_res, NODE_PTR n_opnd1,
                                            NODE_PTR n_opnd2, bool is_ext,
                                            const SPOS& spos) {
  CMPLR_ASSERT(Lower_ctx()->Is_rns_poly_type(n_res->Rtype_id()) &&
                   Lower_ctx()->Is_rns_poly_type(n_opnd1->Rtype_id()),
               "not poly type");

  TYPE_PTR t_bool = Glob_scope()->Prim_type(PRIMITIVE_TYPE::BOOL);
  STMT_PTR s_init = New_poly_stmt(INIT_POLY_BY_OPND, spos);
  s_init->Node()->Set_child(0, n_res);
  s_init->Node()->Set_child(1, n_opnd1);
  if (n_opnd2 != Null_ptr) {
    s_init->Node()->Set_child(2, n_opnd2);
  } else {
    TYPE_PTR t_int = Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32);
    NODE_PTR zero  = Container()->New_intconst(t_int, 0, spos);
    s_init->Node()->Set_child(2, zero);
  }
  s_init->Node()->Set_child(3, is_ext ? Container()->New_one(t_bool, spos)
                                      : Container()->New_zero(t_bool, spos));
  return s_init;
}

STMT_PTR POLY_IR_GEN::New_init_poly(CONST_VAR v_res, uint32_t num_q,
                                    bool is_ext, const SPOS& spos) {
  TYPE_PTR t_uint  = Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_U32);
  NODE_PTR n_num_q = Container()->New_intconst(t_uint, num_q, spos);
  return New_init_poly(v_res, n_num_q, is_ext, spos);
}

STMT_PTR POLY_IR_GEN::New_init_poly(CONST_VAR v_res, NODE_PTR n_num_q,
                                    bool is_ext, const SPOS& spos) {
  CMPLR_ASSERT(Lower_ctx()->Is_rns_poly_type(v_res.Type_id()), "not poly type");

  TYPE_PTR t_bool   = Glob_scope()->Prim_type(PRIMITIVE_TYPE::BOOL);
  NODE_PTR n_res    = New_var_load(v_res, spos);
  NODE_PTR n_is_ext = is_ext ? Container()->New_one(t_bool, spos)
                             : Container()->New_zero(t_bool, spos);
  STMT_PTR s_init   = New_poly_stmt(INIT_POLY, spos);
  s_init->Node()->Set_child(0, n_res);
  s_init->Node()->Set_child(1, n_num_q);
  s_init->Node()->Set_child(2, n_is_ext);
  return s_init;
}

NODE_PTR POLY_IR_GEN::New_alloc_n(NODE_PTR n_cnt, NODE_PTR n_degree,
                                  NODE_PTR n_num_q, NODE_PTR n_num_p,
                                  const SPOS& spos) {
  NODE_PTR n_alloc = New_poly_node(ALLOC_N, Get_type(POLY_PTR), spos);
  n_alloc->Set_child(0, n_cnt);
  n_alloc->Set_child(1, n_degree);
  n_alloc->Set_child(2, n_num_q);
  n_alloc->Set_child(3, n_num_p);
  return n_alloc;
}

STMT_PTR POLY_IR_GEN::New_free_poly(CONST_VAR v_poly, const SPOS& spos) {
  CMPLR_ASSERT(v_poly.Type() == Get_type(POLY, spos), "invalid type");

  NODE_PTR n_poly = New_var_load(v_poly, spos);
  STMT_PTR s_free = New_poly_stmt(FREE, spos);
  s_free->Node()->Set_child(0, n_poly);
  return s_free;
}

NODE_PTR POLY_IR_GEN::New_degree(const SPOS& spos) {
  CONST_TYPE_PTR ui32_type = Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_U32);
  NODE_PTR       n_degree  = New_poly_node(DEGREE, ui32_type, spos);
  return n_degree;
}

NODE_PTR POLY_IR_GEN::New_num_q(NODE_PTR node, const SPOS& spos) {
  AIR_ASSERT_MSG(node->Rtype() == Get_type(POLY, spos),
                 "node is not polynomial");

  NODE_PTR n_level = New_poly_node(
      LEVEL, Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_U32), spos);
  n_level->Set_child(0, node);
  return n_level;
}

NODE_PTR POLY_IR_GEN::New_num_p(NODE_PTR node, const SPOS& spos) {
  CMPLR_ASSERT(node->Rtype() == Get_type(POLY, spos), "node is not polynomial");
  NODE_PTR n_num_p = New_poly_node(
      NUM_P, Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_U32), spos);
  n_num_p->Set_child(0, node);
  return n_num_p;
}

NODE_PTR POLY_IR_GEN::New_num_alloc(NODE_PTR node, const SPOS& spos) {
  CMPLR_ASSERT(node->Rtype() == Get_type(POLY, spos), "node is not polynomial");

  NODE_PTR n_num_alloc_primes = New_poly_node(
      NUM_ALLOC, Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_U32), spos);
  n_num_alloc_primes->Set_child(0, node);
  return n_num_alloc_primes;
}

NODE_PTR POLY_IR_GEN::New_num_decomp(NODE_PTR node, const SPOS& spos) {
  CMPLR_ASSERT(node->Rtype() == Get_type(POLY, spos), "node is not polynomial");

  NODE_PTR n_num_decomp = New_poly_node(
      NUM_DECOMP, Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_U32), spos);
  n_num_decomp->Set_child(0, node);
  return n_num_decomp;
}

NODE_PTR POLY_IR_GEN::New_hw_modadd(NODE_PTR opnd0, NODE_PTR opnd1,
                                    NODE_PTR opnd2, const SPOS& spos) {
  CMPLR_ASSERT((opnd0->Rtype() == opnd1->Rtype() &&
                opnd0->Rtype() == Get_type(INT64_PTR, spos) &&
                opnd2->Rtype() == Get_type(MODULUS_PTR, spos)),
               "unmatched type");

  NODE_PTR add_node = New_poly_node(HW_MODADD, opnd0->Rtype(), spos);
  add_node->Set_child(0, opnd0);
  add_node->Set_child(1, opnd1);
  add_node->Set_child(2, opnd2);
  return add_node;
}

NODE_PTR POLY_IR_GEN::New_hw_modmul(NODE_PTR opnd0, NODE_PTR opnd1,
                                    NODE_PTR opnd2, const SPOS& spos) {
  CMPLR_ASSERT((opnd0->Rtype() == opnd1->Rtype() &&
                opnd0->Rtype() == Get_type(INT64_PTR, spos) &&
                opnd2->Rtype() == Get_type(MODULUS_PTR, spos)),
               "unmatched type");

  NODE_PTR add_node = New_poly_node(HW_MODMUL, opnd0->Rtype(), spos);
  add_node->Set_child(0, opnd0);
  add_node->Set_child(1, opnd1);
  add_node->Set_child(2, opnd2);
  return add_node;
}

NODE_PTR POLY_IR_GEN::New_hw_rotate(NODE_PTR opnd0, NODE_PTR opnd1,
                                    NODE_PTR opnd2, const SPOS& spos) {
  CMPLR_ASSERT(opnd0->Rtype() == Get_type(INT64_PTR, spos), "invalid type");
  CMPLR_ASSERT(opnd1->Rtype() == Get_type(INT64_PTR, spos), "invalid type");
  CMPLR_ASSERT(opnd2->Rtype() == Get_type(MODULUS_PTR, spos), "invalid type");

  NODE_PTR n_res = New_poly_node(HW_ROTATE, opnd0->Rtype(), spos);
  n_res->Set_child(0, opnd0);
  n_res->Set_child(1, opnd1);
  n_res->Set_child(2, opnd2);
  return n_res;
}

NODE_PTR POLY_IR_GEN::New_rescale(NODE_PTR n_opnd, const SPOS& spos) {
  CMPLR_ASSERT(n_opnd->Rtype() == Get_type(POLY, spos), "invalid type");

  NODE_PTR n_res = New_poly_node(RESCALE, n_opnd->Rtype(), spos);
  n_res->Set_child(0, n_opnd);
  return n_res;
}

//! @brief Create MODSWITCH NODE
NODE_PTR POLY_IR_GEN::New_modswitch(NODE_PTR n_opnd, const SPOS& spos) {
  CMPLR_ASSERT(n_opnd->Rtype() == Get_type(POLY, spos), "invalid type");

  NODE_PTR n_res = New_poly_node(MODSWITCH, n_opnd->Rtype(), spos);
  n_res->Set_child(0, n_opnd);
  return n_res;
}

//! @brief Create RAISE_MOD NODE
NODE_PTR POLY_IR_GEN::New_raise_mod(NODE_PTR n_opnd0, NODE_PTR n_opnd1,
                                    const SPOS& spos) {
  CMPLR_ASSERT(Is_type_of(n_opnd0->Rtype_id(), POLY), "invalid type");
  CMPLR_ASSERT(n_opnd1->Rtype()->Is_int(), "invalid type");

  NODE_PTR n_res = New_poly_node(RAISE_MOD, n_opnd0->Rtype(), spos);
  n_res->Set_child(0, n_opnd0);
  n_res->Set_child(1, n_opnd1);
  return n_res;
}

NODE_PTR POLY_IR_GEN::New_get_next_modulus(CONST_VAR   mod_var,
                                           const SPOS& spos) {
  NODE_PTR ld_mod   = New_var_load(mod_var, spos);
  NODE_PTR one_node = Container()->New_intconst(
      Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_U64), 1, spos);
  NODE_PTR inc_mod =
      IR_GEN::New_bin_arith(air::core::CORE, air::core::OPCODE::ADD,
                            ld_mod->Rtype(), ld_mod, one_node, spos);
  return inc_mod;
}

NODE_PTR POLY_IR_GEN::New_q_modulus(const SPOS& spos) {
  CONTAINER* cont = Container();
  CMPLR_ASSERT(cont, "null scope");

  NODE_PTR ret = New_poly_node(Q_MODULUS, Get_type(MODULUS_PTR, spos), spos);
  return ret;
}

NODE_PTR POLY_IR_GEN::New_p_modulus(const SPOS& spos) {
  CONTAINER* cont = Container();
  CMPLR_ASSERT(cont, "null scope");

  NODE_PTR ret = New_poly_node(P_MODULUS, Get_type(MODULUS_PTR, spos), spos);
  return ret;
}

void POLY_IR_GEN::Append_rns_stmt(STMT_PTR stmt, NODE_PTR parent_blk) {
  if (stmt == Null_ptr) return;
  CMPLR_ASSERT(parent_blk->Is_block(), "not a block");

  STMT_LIST stmt_list(parent_blk);
  STMT_PTR  inc_mod = stmt_list.Last_stmt();
  CMPLR_ASSERT(inc_mod != Null_ptr, "missing inc modulus stmt");
  stmt_list.Prepend(inc_mod, stmt);
}

NODE_PTR POLY_IR_GEN::New_mod_up(NODE_PTR node, CONST_VAR v_part_idx,
                                 const SPOS& spos) {
  CMPLR_ASSERT(node->Rtype() == Get_type(POLY, spos),
               "node is not polynonmial");

  NODE_PTR n_res   = New_poly_node(MOD_UP, node->Rtype(), spos);
  NODE_PTR n_opnd1 = New_var_load(v_part_idx, spos);
  n_res->Set_child(0, node);
  n_res->Set_child(1, n_opnd1);
  return n_res;
}

NODE_PTR POLY_IR_GEN::New_decomp_modup(NODE_PTR node, CONST_VAR v_part_idx,
                                       const SPOS& spos) {
  CMPLR_ASSERT(node->Rtype() == Get_type(POLY, spos),
               "node is not polynonmial");

  NODE_PTR n_res   = New_poly_node(DECOMP_MODUP, node->Rtype(), spos);
  NODE_PTR n_opnd1 = New_var_load(v_part_idx, spos);
  n_res->Set_child(0, node);
  n_res->Set_child(1, n_opnd1);
  return n_res;
}

NODE_PTR POLY_IR_GEN::New_alloc_for_precomp(NODE_PTR node, const SPOS& spos) {
  CMPLR_ASSERT(node->Rtype() == Get_type(POLY, spos),
               "node is not polynonmial");
  CONTAINER* cntr     = Container();
  NODE_PTR   n_cnt    = New_num_decomp(cntr->Clone_node_tree(node), spos);
  NODE_PTR   n_degree = New_degree(spos);
  NODE_PTR   n_num_q  = New_num_q(cntr->Clone_node_tree(node), spos);
  NODE_PTR   n_num_p  = cntr->New_intconst(
      Get_type(UINT32), Lower_ctx()->Get_ctx_param().Get_p_prime_num(), spos);
  NODE_PTR n_res = New_alloc_n(n_cnt, n_degree, n_num_q, n_num_p, spos);
  return n_res;
}

NODE_PTR POLY_IR_GEN::New_precomp(NODE_PTR node, const SPOS& spos) {
  CMPLR_ASSERT(node->Rtype() == Get_type(POLY, spos),
               "node is not polynonmial");

  NODE_PTR n_res = New_poly_node(PRECOMP, Get_type(POLY_PTR, spos), spos);
  n_res->Set_child(0, node);
  return n_res;
}

NODE_PTR POLY_IR_GEN::New_dot_prod(NODE_PTR n0, NODE_PTR n1, NODE_PTR n2,
                                   const SPOS& spos) {
  CMPLR_ASSERT(
      (n0->Rtype() == Get_type(POLY_PTR, spos) || Is_poly_array(n0->Rtype())),
      "New_dot_prod opnd0 should be poly ptr or poly array");
  CMPLR_ASSERT(
      (n1->Rtype() == Get_type(POLY_PTR, spos) || Is_poly_array(n1->Rtype())),
      "New_dot_prod opnd1 should be poly ptr or poly array")
  NODE_PTR n_res = New_poly_node(DOT_PROD, Get_type(POLY, spos), spos);
  n_res->Set_child(0, n0);
  n_res->Set_child(1, n1);
  n_res->Set_child(2, n2);
  return n_res;
}

NODE_PTR POLY_IR_GEN::New_mod_down(NODE_PTR node, const SPOS& spos) {
  CMPLR_ASSERT(node->Rtype() == Get_type(POLY, spos),
               "node is not polynonmial");

  NODE_PTR n_res = New_poly_node(MOD_DOWN, node->Rtype(), spos);
  n_res->Set_child(0, node);
  return n_res;
}

NODE_PTR POLY_IR_GEN::New_auto_order(NODE_PTR node, const SPOS& spos) {
  CMPLR_ASSERT(node->Rtype()->Is_int(), "node is not integer");

  NODE_PTR n_res = New_poly_node(AUTO_ORDER, Get_type(INT64_PTR, spos), spos);
  n_res->Set_child(0, node);
  return n_res;
}

NODE_PTR POLY_IR_GEN::New_swk(bool is_rot, const SPOS& spos,
                              NODE_PTR n_rot_idx) {
  if (is_rot) {
    // rotation key
    CMPLR_ASSERT(n_rot_idx != Null_ptr, "rot_idx is null");
    NODE_PTR n_res  = New_poly_node(SWK, Get_type(SWK_PTR, spos), spos);
    TYPE_PTR t_bool = Glob_scope()->Prim_type(PRIMITIVE_TYPE::BOOL);
    n_res->Set_child(0, Container()->New_one(t_bool, spos));
    n_res->Set_child(1, n_rot_idx);
    return n_res;
  } else {
    // relinearlize key
    NODE_PTR n_res  = New_poly_node(SWK, Get_type(SWK_PTR, spos), spos);
    TYPE_PTR t_bool = Glob_scope()->Prim_type(PRIMITIVE_TYPE::BOOL);
    TYPE_PTR t_int  = Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32);
    n_res->Set_child(0, Container()->New_zero(t_bool, spos));
    n_res->Set_child(1, Container()->New_intconst(t_int, 0, spos));
    return n_res;
  }
}

NODE_PTR POLY_IR_GEN::New_swk_c0(bool is_rot, const SPOS& spos,
                                 NODE_PTR n_rot_idx) {
  if (is_rot) {
    // rotation key
    CMPLR_ASSERT(n_rot_idx != Null_ptr, "rot_idx is null");
    NODE_PTR n_res  = New_poly_node(SWK_C0, Get_type(SWK_POLYS), spos);
    TYPE_PTR t_bool = Glob_scope()->Prim_type(PRIMITIVE_TYPE::BOOL);
    n_res->Set_child(0, Container()->New_one(t_bool, spos));
    n_res->Set_child(1, n_rot_idx);
    return n_res;
  } else {
    // relinearlize key
    NODE_PTR n_res  = New_poly_node(SWK_C0, Get_type(SWK_POLYS), spos);
    TYPE_PTR t_bool = Glob_scope()->Prim_type(PRIMITIVE_TYPE::BOOL);
    TYPE_PTR t_int  = Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32);
    n_res->Set_child(0, Container()->New_zero(t_bool, spos));
    n_res->Set_child(1, Container()->New_intconst(t_int, 0, spos));
    return n_res;
  }
}

NODE_PTR POLY_IR_GEN::New_swk_c1(bool is_rot, const SPOS& spos,
                                 NODE_PTR n_rot_idx) {
  if (is_rot) {
    // rotation key
    CMPLR_ASSERT(n_rot_idx != Null_ptr, "rot_idx is null");
    NODE_PTR n_res  = New_poly_node(SWK_C1, Get_type(SWK_POLYS), spos);
    TYPE_PTR t_bool = Glob_scope()->Prim_type(PRIMITIVE_TYPE::BOOL);
    n_res->Set_child(0, Container()->New_one(t_bool, spos));
    n_res->Set_child(1, n_rot_idx);
    return n_res;
  } else {
    // relinearlize key
    NODE_PTR n_res  = New_poly_node(SWK_C1, Get_type(SWK_POLYS), spos);
    TYPE_PTR t_bool = Glob_scope()->Prim_type(PRIMITIVE_TYPE::BOOL);
    TYPE_PTR t_int  = Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_S32);
    n_res->Set_child(0, Container()->New_zero(t_bool, spos));
    n_res->Set_child(1, Container()->New_intconst(t_int, 0, spos));
    return n_res;
  }
}

NODE_PTR POLY_IR_GEN::New_decomp(NODE_PTR node, CONST_VAR v_part_idx,
                                 const SPOS& spos) {
  CMPLR_ASSERT(node->Rtype() == Get_type(POLY, spos), "node is not polynomial");

  NODE_PTR n_part_idx = New_var_load(v_part_idx, spos);
  NODE_PTR n_res      = New_poly_node(DECOMP, node->Rtype(), spos);
  n_res->Set_child(0, node);
  n_res->Set_child(1, n_part_idx);
  return n_res;
}

NODE_PTR POLY_IR_GEN::New_pk0_at(CONST_VAR v_swk, CONST_VAR v_part_idx,
                                 const SPOS& spos) {
  CMPLR_ASSERT(v_swk.Type() == Get_type(SWK_PTR, spos),
               "node is not switch key");

  NODE_PTR n_opnd0 = New_var_load(v_swk, spos);
  NODE_PTR n_opnd1 = New_var_load(v_part_idx, spos);
  NODE_PTR n_res   = New_poly_node(PK0_AT, Get_type(POLY, spos), spos);
  n_res->Set_child(0, n_opnd0);
  n_res->Set_child(1, n_opnd1);
  return n_res;
}

NODE_PTR POLY_IR_GEN::New_pk1_at(CONST_VAR v_swk, CONST_VAR v_part_idx,
                                 const SPOS& spos) {
  CMPLR_ASSERT(v_swk.Type() == Get_type(SWK_PTR, spos),
               "node is not switch key");

  NODE_PTR n_opnd0 = New_var_load(v_swk, spos);
  NODE_PTR n_opnd1 = New_var_load(v_part_idx, spos);
  NODE_PTR n_res   = New_poly_node(PK1_AT, Get_type(POLY, spos), spos);
  n_res->Set_child(0, n_opnd0);
  n_res->Set_child(1, n_opnd1);
  return n_res;
}

NODE_PAIR POLY_IR_GEN::New_rns_loop(NODE_PTR node, bool is_ext) {
  SPOS spos = node->Spos();

  // 1. create a new block to save lowered loop IR
  NODE_PTR  n_outer_blk = Container()->New_stmt_block(node->Spos());
  STMT_LIST sl_outer    = STMT_LIST::Enclosing_list(n_outer_blk->End_stmt());

  // 2. before loop
  // generate get modulus node
  CONST_VAR& v_modulus = Get_var(VAR_MODULUS, spos);
  NODE_PTR   n_modulus = is_ext ? New_p_modulus(spos) : New_q_modulus(spos);
  STMT_PTR   s_modulus = New_var_store(n_modulus, v_modulus, spos);
  sl_outer.Append(s_modulus);

  // extra statements for extended polynomial
  VAR v_ub;
  if (is_ext) {
    // store poly num of p to variable
    CONST_VAR& v_num_p = Get_var(VAR_NUM_P, spos);
    NODE_PTR   n_num_p = New_num_p(node, spos);
    STMT_PTR   s_num_p = New_var_store(n_num_p, v_num_p, spos);
    v_ub               = v_num_p;
    sl_outer.Append(s_num_p);

    // generate p_ofst node, p_ofst = num_alloc(node) -
    // get_num_primes_p(node)
    // TODO: p_ofst is known at compile time
    CONST_VAR& v_p_ofst       = Get_var(VAR_P_OFST, spos);
    NODE_PTR   n_alloc_primes = New_num_alloc(node, spos);
    NODE_PTR   n_ld_p_num     = New_var_load(v_num_p, spos);
    NODE_PTR   n_sub   = IR_GEN::New_bin_arith(air::core::CORE, air::core::SUB,
                                               n_alloc_primes->Rtype(),
                                               n_alloc_primes, n_ld_p_num, spos);
    STMT_PTR   s_pofst = New_var_store(n_sub, v_p_ofst, spos);
    sl_outer.Append(s_pofst);
  } else {
    // store poly level to variable
    CONST_VAR& v_num_q = Get_var(VAR_NUM_Q, spos);
    v_ub               = v_num_q;
    NODE_PTR n_num_q   = New_num_q(node, spos);
    STMT_PTR s_num_q   = New_var_store(n_num_q, v_num_q, spos);
    sl_outer.Append(s_num_q);
  }

  // 3. generate loop node
  NODE_PTR   n_ub        = New_var_load(v_ub, spos);
  CONST_VAR& v_idx       = Get_var(VAR_RNS_IDX, spos);
  STMT_PTR   s_loop      = New_loop(v_idx, n_ub, 0, 1, spos);
  NODE_PTR   n_loop_body = s_loop->Node()->Child(3);
  CMPLR_ASSERT(n_loop_body->Is_block(), "not a block node");
  STMT_LIST sl_body = STMT_LIST::Enclosing_list(n_loop_body->End_stmt());
  sl_outer.Append(s_loop);

  // generate modulus++
  NODE_PTR n_inc_mod = New_get_next_modulus(v_modulus, spos);
  STMT_PTR s_inc_mod = New_var_store(n_inc_mod, v_modulus, spos);
  sl_body.Append(s_inc_mod);

  return std::make_pair(n_outer_blk, n_loop_body);
}

STMT_PTR POLY_IR_GEN::New_decomp_loop(NODE_PTR node, const SPOS& spos) {
  CONST_VAR& v_part_idx = Get_var(VAR_PART_IDX, spos);

  // loop upper bound
  CONST_TYPE_PTR ui32_type = Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_U32);
  NODE_PTR       n_num_q_parts = New_poly_node(NUM_DECOMP, ui32_type, spos);
  n_num_q_parts->Set_child(0, node);

  STMT_PTR s_loop = New_loop(v_part_idx, n_num_q_parts, 0, 1, spos);
  return s_loop;
}

STMT_PTR POLY_IR_GEN::New_adjust_p_idx(const SPOS& spos) {
  // adjust p_idx -> rns_idx + p_ofst
  CONST_VAR& v_rns_idx = Get_var(VAR_RNS_IDX, spos);
  CONST_VAR& v_p_ofst  = Get_var(VAR_P_OFST, spos);
  CONST_VAR& v_p_idx   = Get_var(VAR_P_IDX, spos);
  NODE_PTR   n_level   = New_var_load(v_rns_idx, spos);
  NODE_PTR   n_pofst   = New_var_load(v_p_ofst, spos);
  NODE_PTR   n_add =
      IR_GEN::New_bin_arith(air::core::CORE, air::core::ADD, n_level->Rtype(),
                            n_level, n_pofst, spos);
  STMT_PTR s_pidx = New_var_store(n_add, v_p_idx, spos);
  return s_pidx;
}

NODE_PTR POLY_IR_GEN::New_key_switch(CONST_VAR v_swk_c0, CONST_VAR v_swk_c1,
                                     CONST_VAR v_c1_ext, CONST_VAR v_key0,
                                     CONST_VAR v_key1, const SPOS& spos,
                                     bool is_ext) {
  NODE_PTR  n_c1_ext  = New_var_load(v_c1_ext, spos);
  NODE_PAIR n_blks    = New_rns_loop(n_c1_ext, is_ext);
  NODE_PTR  outer_blk = n_blks.first;
  NODE_PTR  body_blk  = n_blks.second;

  CONST_VAR& v_tmp_poly = Get_var(VAR_TMP_POLY, spos);
  VAR        v_rns_idx  = Get_var(VAR_RNS_IDX, spos);
  VAR        v_key_idx  = v_rns_idx;

  if (is_ext) {
    STMT_PTR s_pidx = New_adjust_p_idx(spos);
    Append_rns_stmt(s_pidx, body_blk);

    // key_idx = rns_idx + key_p_ofst
    // key's p start ofst is different with input poly's p ofst
    // it always start from key's q count
    CONST_VAR& v_key_pofst = Get_var(VAR_KEY_P_OFST, spos);
    NODE_PTR   n_key_pofst = New_num_q(New_var_load(v_key0, spos), spos);
    STMT_PTR   s_key_pofst = New_var_store(n_key_pofst, v_key_pofst, spos);
    Append_rns_stmt(s_key_pofst, outer_blk);

    v_key_idx        = Get_var(VAR_KEY_P_IDX, spos);
    n_key_pofst      = New_var_load(v_key_pofst, spos);
    NODE_PTR n_level = New_var_load(v_rns_idx, spos);
    NODE_PTR n_key_idx =
        IR_GEN::New_bin_arith(air::core::CORE, air::core::ADD, n_level->Rtype(),
                              n_level, n_key_pofst, spos);
    STMT_PTR s_key_idx = New_var_store(n_key_idx, v_key_idx, spos);
    Append_rns_stmt(s_key_idx, body_blk);
    v_rns_idx = Get_var(VAR_P_IDX, spos);
  }

  NODE_PTR n_key0     = New_var_load(v_key0, spos);
  NODE_PTR n_key1     = New_var_load(v_key1, spos);
  NODE_PTR n_swk_c0   = New_var_load(v_swk_c0, spos);
  NODE_PTR n_swk_c1   = New_var_load(v_swk_c1, spos);
  NODE_PTR n_tmp_poly = New_var_load(v_tmp_poly, spos);
  NODE_PTR n_zero     = Container()->New_intconst(
      Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_U32), 0, spos);

  NODE_PTR n_key0_at_level     = New_poly_load_at_level(n_key0, v_key_idx);
  NODE_PTR n_key1_at_level     = New_poly_load_at_level(n_key1, v_key_idx);
  NODE_PTR n_swk_c0_at_level   = New_poly_load_at_level(n_swk_c0, v_rns_idx);
  NODE_PTR n_swk_c1_at_level   = New_poly_load_at_level(n_swk_c1, v_rns_idx);
  NODE_PTR n_c1_ext_at_level   = New_poly_load_at_level(n_c1_ext, v_rns_idx);
  NODE_PTR n_tmp_poly_at_level = New_poly_load_at_level(n_tmp_poly, n_zero);

  CONST_VAR& v_modulus = Get_var(VAR_MODULUS, spos);
  NODE_PTR   n_modulus = New_var_load(v_modulus, spos);

  // v_tmp_poly = hw_modmul(v_key0, v_c1_ext)
  // v_swk_c0 = hw_modadd(v_swk_c0, v_tmp_poly)
  NODE_PTR n_mul_key0 =
      New_hw_modmul(n_key0_at_level, n_c1_ext_at_level, n_modulus, spos);
  STMT_PTR s_tmp = New_poly_store_at_level(n_tmp_poly, n_mul_key0, n_zero);
  NODE_PTR n_add_c0 =
      New_hw_modadd(n_swk_c0_at_level, n_tmp_poly_at_level, n_modulus, spos);
  STMT_PTR s_swk_c0 = New_poly_store_at_level(n_swk_c0, n_add_c0, v_rns_idx);
  Append_rns_stmt(s_tmp, body_blk);
  Append_rns_stmt(s_swk_c0, body_blk);

  // v_tmp_poly = hw_modmul(v_key1, v_c1_ext)
  // v_swk_c1 = hw_modadd(v_swk_c1, v_tmp_poly)
  NODE_PTR n_mul_key1 =
      New_hw_modmul(n_key1_at_level, n_c1_ext_at_level, n_modulus, spos);
  s_tmp = New_poly_store_at_level(n_tmp_poly, n_mul_key1, n_zero);
  NODE_PTR n_add_c1 =
      New_hw_modadd(n_swk_c1_at_level, n_tmp_poly_at_level, n_modulus, spos);
  // can reused n_swk_c0 and n_swk_c1 node?
  STMT_PTR s_swk_c1 = New_poly_store_at_level(n_swk_c1, n_add_c1, v_rns_idx);
  Append_rns_stmt(s_tmp, body_blk);
  Append_rns_stmt(s_swk_c1, body_blk);

  return outer_blk;
}

void POLY_IR_GEN::Set_mul_ciph(NODE_PTR node) {
  uint32_t    mul_ciph  = 1;
  const char* attr_name = core::FHE_ATTR_KIND::MUL_CIPH;
  node->Set_attr(attr_name, &mul_ciph, 1);
}

NODE_PTR POLY_IR_GEN::New_extract(NODE_PTR n0, NODE_PTR n1, NODE_PTR n2,
                                  const SPOS& spos) {
  AIR_ASSERT(Lower_ctx()->Is_rns_poly_type(n0->Rtype_id()) &&
             n1->Rtype()->Is_int() && n2->Rtype()->Is_int());

  NODE_PTR ret = New_poly_node(EXTRACT, n0->Rtype(), spos);
  ret->Set_child(0, n0);
  ret->Set_child(1, n1);
  ret->Set_child(2, n2);
  return ret;
}

NODE_PTR POLY_IR_GEN::New_extract(NODE_PTR n0, uint32_t s_idx, uint32_t e_idx,
                                  const SPOS& spos) {
  uint32_t tot_primes = Lower_ctx()->Get_ctx_param().Get_tot_prime_num();
  AIR_ASSERT(s_idx < tot_primes && e_idx < tot_primes && e_idx >= s_idx);
  TYPE_PTR t_ui32 = Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_U32);
  NODE_PTR n1     = Container()->New_intconst(t_ui32, s_idx, spos);
  NODE_PTR n2     = Container()->New_intconst(t_ui32, e_idx, spos);
  return New_extract(n0, n1, n2, spos);
}

NODE_PTR POLY_IR_GEN::New_concat(NODE_PTR n0, NODE_PTR n1, const SPOS& spos) {
  AIR_ASSERT(Lower_ctx()->Is_rns_poly_type(n0->Rtype_id()) &&
             Lower_ctx()->Is_rns_poly_type(n1->Rtype_id()));

  NODE_PTR ret = New_poly_node(CONCAT, n0->Rtype(), spos);
  ret->Set_child(0, n0);
  ret->Set_child(1, n1);
  return ret;
}

NODE_PTR POLY_IR_GEN::New_bconv(NODE_PTR n0, NODE_PTR n1, uint32_t s_idx,
                                uint32_t e_idx, const SPOS& spos) {
  uint32_t tot_primes = Lower_ctx()->Get_ctx_param().Get_tot_prime_num();
  AIR_ASSERT(s_idx < tot_primes && e_idx < tot_primes && e_idx >= s_idx);
  AIR_ASSERT(Lower_ctx()->Is_rns_poly_type(n0->Rtype_id()) &&
             Is_int_array(n1->Rtype()));

  TYPE_PTR ty_idx = Get_type(UINT64, spos);
  NODE_PTR ret    = New_poly_node(BCONV, n0->Rtype(), spos);
  ret->Set_child(0, n0);
  ret->Set_child(1, n1);
  ret->Set_child(2, Container()->New_intconst(ty_idx, s_idx, spos));
  ret->Set_child(3, Container()->New_intconst(ty_idx, e_idx, spos));
  return ret;
}

NODE_PTR POLY_IR_GEN::New_bswitch(NODE_PTR n0, NODE_PTR n1, uint32_t s_idx,
                                  uint32_t e_idx, const SPOS& spos) {
  uint32_t tot_primes = Lower_ctx()->Get_ctx_param().Get_tot_prime_num();
  AIR_ASSERT(s_idx < tot_primes && e_idx < tot_primes && e_idx >= s_idx);
  AIR_ASSERT(Lower_ctx()->Is_rns_poly_type(n0->Rtype_id()) &&
             Is_int_array(n1->Rtype()));

  TYPE_PTR ty_idx = Get_type(UINT64, spos);
  NODE_PTR ret    = New_poly_node(BSWITCH, n0->Rtype(), spos);
  ret->Set_child(0, n0);
  ret->Set_child(1, n1);
  ret->Set_child(2, Container()->New_intconst(ty_idx, s_idx, spos));
  ret->Set_child(3, Container()->New_intconst(ty_idx, e_idx, spos));
  return ret;
}

NODE_PTR POLY_IR_GEN::New_ntt(NODE_PTR n0, const SPOS& spos) {
  AIR_ASSERT(Lower_ctx()->Is_rns_poly_type(n0->Rtype_id()));

  NODE_PTR ret = New_poly_node(NTT, n0->Rtype(), spos);
  ret->Set_child(0, n0);
  return ret;
}

NODE_PTR POLY_IR_GEN::New_intt(NODE_PTR n0, const SPOS& spos) {
  AIR_ASSERT(Lower_ctx()->Is_rns_poly_type(n0->Rtype_id()));

  NODE_PTR ret = New_poly_node(INTT, n0->Rtype(), spos);
  ret->Set_child(0, n0);
  return ret;
}

NODE_PTR POLY_IR_GEN::New_qlhinvmodq(uint32_t part_idx, uint32_t part_size_idx,
                                     const SPOS& spos) {
  CONSTANT_PTR cst = Lower_ctx()->Get_crt_cst().Get_qlhinvmodq(
      Glob_scope(), part_idx, part_size_idx);
  NODE_PTR ret = Container()->New_ldc(cst, spos);
  return ret;
}

NODE_PTR POLY_IR_GEN::New_qlinvmodq(uint32_t idx, const air::base::SPOS& spos) {
  CONSTANT_PTR cst =
      Lower_ctx()->Get_crt_cst().Get_qlinvmodq(Glob_scope(), idx);
  NODE_PTR ret = Container()->New_ldc(cst, spos);
  return ret;
}

NODE_PTR POLY_IR_GEN::New_qlhalfmodq(uint32_t               idx,
                                     const air::base::SPOS& spos) {
  CONSTANT_PTR cst =
      Lower_ctx()->Get_crt_cst().Get_qlhalfmodq(Glob_scope(), idx);
  NODE_PTR ret = Container()->New_ldc(cst, spos);
  return ret;
}

NODE_PTR POLY_IR_GEN::New_q0halfmodq(const air::base::SPOS& spos) {
  CONSTANT_PTR cst = Lower_ctx()->Get_crt_cst().Get_q0halfmodq(Glob_scope());
  NODE_PTR     ret = Container()->New_ldc(cst, spos);
  return ret;
}

NODE_PTR POLY_IR_GEN::New_qlhmodp(uint32_t q_idx, uint32_t part_idx,
                                  uint32_t start_idx, uint32_t len,
                                  const SPOS& spos) {
  CONSTANT_PTR cst = Lower_ctx()->Get_crt_cst().Get_qlhmodp(
      Glob_scope(), q_idx, part_idx, start_idx, len);
  NODE_PTR ret = Container()->New_ldc(cst, spos);
  return ret;
}

NODE_PTR POLY_IR_GEN::New_phinvmodp(const air::base::SPOS& spos) {
  CONSTANT_PTR cst = Lower_ctx()->Get_crt_cst().Get_phinvmodp(Glob_scope());
  NODE_PTR     ret = Container()->New_ldc(cst, spos);
  return ret;
}

NODE_PTR POLY_IR_GEN::New_phmodq(const air::base::SPOS& spos) {
  CONSTANT_PTR cst = Lower_ctx()->Get_crt_cst().Get_phmodq(Glob_scope());
  NODE_PTR     ret = Container()->New_ldc(cst, spos);
  return ret;
}

NODE_PTR POLY_IR_GEN::New_pinvmodq(const air::base::SPOS& spos) {
  CONSTANT_PTR cst = Lower_ctx()->Get_crt_cst().Get_pinvmodq(Glob_scope());
  NODE_PTR     ret = Container()->New_ldc(cst, spos);
  return ret;
}

}  // namespace poly

}  // namespace fhe
