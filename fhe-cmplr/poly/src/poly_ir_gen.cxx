//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "poly_ir_gen.h"

#include <iostream>
#include <sstream>

#include "air/base/container.h"
#include "air/core/opcode.h"
#include "fhe/ckks/ckks_opcode.h"

using namespace air::base;
using namespace fhe::core;

namespace fhe {

namespace poly {

typedef struct {
  POLY_PREDEF_VAR _id;
  VAR_TYPE_KIND   _kind;
  const char*     _name;
} POLY_VAR_INFO;

static POLY_VAR_INFO Pred_var_info[] = {
    {VAR_NUM_Q,       INDEX,        "_pgen_num_q"      },
    {VAR_NUM_P,       INDEX,        "_pgen_num_p"      },
    {VAR_P_OFST,      INDEX,        "_pgen_p_ofst"     },
    {VAR_P_IDX,       INDEX,        "_pgen_p_idx"      },
    {VAR_KEY_P_OFST,  INDEX,        "_pgen_key_p_ofst" },
    {VAR_KEY_P_IDX,   INDEX,        "_pgen_key_p_idx"  },
    {VAR_RNS_IDX,     INDEX,        "_pgen_rns_idx"    },
    {VAR_PART_IDX,    INDEX,        "_pgen_part_idx"   },
    {VAR_MODULUS,     MODULUS_PTR,  "_pgen_modulus"    },
    {VAR_SWK,         SWK_PTR,      "_pgen_swk"        },
    {VAR_SWK_C0,      POLY_PTR,     "_pgen_swk_c0"     },
    {VAR_SWK_C1,      POLY_PTR,     "_pgen_swk_c1"     },
    {VAR_DECOMP,      POLY,         "_pgen_decomp"     },
    {VAR_EXT,         POLY_PTR,     "_pgen_ext"        },
    {VAR_PUB_KEY0,    POLY_PTR,     "_pgen_key0"       },
    {VAR_PUB_KEY1,    POLY_PTR,     "_pgen_key1"       },
    {VAR_MOD_DOWN_C0, POLY_PTR,     "_pgen_mod_down_c0"},
    {VAR_MOD_DOWN_C1, POLY_PTR,     "_pgen_mod_down_c1"},
    {VAR_AUTO_ORDER,  INT64_PTR,    "_pgen_order"      },
    {VAR_TMP_POLY,    POLY_PTR,     "_pgen_tmp_poly"   },
    {VAR_TMP_COEFFS,  INT64_PTR,    "_pgen_tmp_coeffs" },
    {VAR_MUL_0_POLY,  POLY,         "_pgen_mul_0"      },
    {VAR_MUL_1_POLY,  POLY,         "_pgen_mul_1"      },
    {VAR_MUL_2_POLY,  POLY,         "_pgen_mul_2"      },
    {VAR_ROT_RES,     CIPH,         "_pgen_rot_res"    },
    {VAR_RELIN_RES,   CIPH,         "_pgen_relin_res"  },
    {VAR_PRECOMP,     POLY_PTR_PTR, "_pgen_precomp"    },
};

//! @brief Poly builtin function table
static const POLY_FUNC_INFO Builtin_func_info[] = {
    {core::ROTATE,         2, CIPH, {CIPH, INT32}, {"ciph", "rot_idx"}, "Rotate"     },
    {core::RELIN,          1, CIPH, {CIPH3},       {"ciph"},            "Relinearize"},
    {core::RNS_ADD,
     3,                       POLY,
     {POLY, POLY, POLY},
     {"res", "p0", "p1"},
     "Rns_add"                                                                       },
    {core::RNS_MUL,
     3,                       POLY,
     {POLY, POLY, POLY},
     {"res", "p0", "p1"},
     "Rns_mul"                                                                       },
    {core::RNS_ROTATE,
     3,                       POLY,
     {POLY, POLY, INT32},
     {"res", "p0", "rot_idx"},
     "Rns_rotate"                                                                    },
    {core::RNS_ADD_EXT,
     3,                       POLY,
     {POLY, POLY, POLY},
     {"res", "p0", "p1"},
     "Rns_add_ext"                                                                   },
    {core::RNS_MUL_EXT,
     3,                       POLY,
     {POLY, POLY, POLY},
     {"res", "p0", "p1"},
     "Rns_mul_ext"                                                                   },
    {core::RNS_ROTATE_EXT,
     3,                       POLY,
     {POLY, POLY, INT32},
     {"res", "p0", "rot_idx"},
     "Rns_rotate_ext"                                                                },
};

//! @brief Get function prototype info with given func id
//!        return null if not found
const POLY_FUNC_INFO* Poly_func_info(core::FHE_FUNC func_id) {
  size_t table_len = sizeof(Builtin_func_info) / sizeof(POLY_FUNC_INFO);
  for (size_t idx = 0; idx < table_len; idx++) {
    if (Builtin_func_info[idx]._func_id == func_id) {
      return &Builtin_func_info[idx];
    }
  }
  return nullptr;
}

void VAR::Print(std::ostream& os, uint32_t indent) const {
  if (Is_sym()) {
    Addr_var()->Print(os, indent);
  } else if (Is_preg()) {
    Preg_var()->Print(os, indent);
  } else {
    AIR_ASSERT(false);
  }
  if (Fld() != Null_ptr) {
    Fld()->Print(os, indent);
  }
}

void VAR::Print() const { Print(std::cout, 0); }

std::string VAR::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os);
  return buf.str();
}

TYPE_PTR POLY_IR_GEN::Get_type(VAR_TYPE_KIND id, const SPOS& spos) {
  CMPLR_ASSERT(id < VAR_TYPE_KIND::LAST_TYPE, "id outof bound")
  GLOB_SCOPE*           gs    = Glob_scope();
  std::vector<TYPE_ID>& types = Types();
  TYPE_PTR              ty    = air::base::Null_ptr;
  if (!types[id].Is_null()) {
    ty = gs->Type(types[id]);
    return ty;
  }
  switch (id) {
    case INDEX:
      ty = gs->Prim_type(air::base::PRIMITIVE_TYPE::INT_U32);
      break;
    case MODULUS_PTR:
      ty = New_modulus_ptr_type(spos);
      break;
    case SWK_PTR:
      ty = New_swk_ptr_type(spos);
      break;
    case PUBKEY_PTR:
      ty = New_pubkey_ptr_type(spos);
      break;
    case INT64_PTR:
      ty = gs->New_ptr_type(gs->Prim_type(PRIMITIVE_TYPE::INT_S64)->Id(),
                            POINTER_KIND::FLAT64);
      break;
    case INT32:
      ty = gs->Prim_type(air::base::PRIMITIVE_TYPE::INT_S32);
      break;
    case POLY:
      ty = Poly_type();
      break;
    case POLY_PTR:
      ty = gs->New_ptr_type(Poly_type()->Id(), POINTER_KIND::FLAT64);
      break;
    case POLY_PTR_PTR:
      ty = gs->New_ptr_type(Get_type(POLY_PTR, spos)->Id(),
                            POINTER_KIND::FLAT64);
      break;
    case PLAIN:
      ty = gs->Type(Lower_ctx()->Get_plain_type_id());
      break;
    case CIPH:
      ty = gs->Type(Lower_ctx()->Get_cipher_type_id());
      break;
    case CIPH_PTR:
      ty = gs->New_ptr_type(Lower_ctx()->Get_cipher_type_id(),
                            POINTER_KIND::FLAT64);
      break;
    case CIPH3:
      ty = gs->Type(Lower_ctx()->Get_cipher3_type_id());
      break;
    default:
      CMPLR_ASSERT(false, "not supported var kind");
  }
  types[id] = ty->Id();
  return ty;
}

CONST_VAR& POLY_IR_GEN::Get_var(POLY_PREDEF_VAR id, const SPOS& spos) {
  CMPLR_ASSERT(id < POLY_PREDEF_VAR::LAST_VAR, "id outof bound")
  FUNC_SCOPE*              fs         = Container()->Parent_func_scope();
  std::map<uint64_t, VAR>& predef_var = Predef_var();

  uint64_t idx = id + ((uint64_t)(fs->Id().Value()) << 32);
  if (predef_var.find(idx) != predef_var.end()) {
    CMPLR_ASSERT(predef_var[idx].Func_scope() == fs, "invalid predef var map");
    return predef_var[idx];
  } else {
    const char*        name = Pred_var_info[id]._name;
    GLOB_SCOPE*        gs   = Container()->Glob_scope();
    air::base::STR_PTR str  = gs->New_str(name);
    TYPE_PTR           ty   = Get_type(Pred_var_info[id]._kind, spos);
    VAR                var(fs, fs->New_var(ty, str, spos));
    predef_var[idx] = var;
    return predef_var[idx];
  }
}

air::base::STR_PTR POLY_IR_GEN::Gen_tmp_name() {
  std::string name("_pgen_tmp_");
  name.append(std::to_string(Tmp_var().size()));
  air::base::GLOB_SCOPE* gs  = Container()->Glob_scope();
  air::base::STR_PTR     str = gs->New_str(name.c_str());
  return str;
}

air::base::ADDR_DATUM_PTR POLY_IR_GEN::New_plain_var(const SPOS& spos) {
  FUNC_SCOPE*    fs = Container()->Parent_func_scope();
  ADDR_DATUM_PTR var =
      fs->New_var(Glob_scope()->Type(Lower_ctx()->Get_plain_type_id()),
                  Gen_tmp_name(), spos);
  Tmp_var().push_back(VAR(fs, var));
  return var;
}

air::base::ADDR_DATUM_PTR POLY_IR_GEN::New_ciph_var(const SPOS& spos) {
  air::base::FUNC_SCOPE*    fs = Container()->Parent_func_scope();
  air::base::ADDR_DATUM_PTR var =
      fs->New_var(Glob_scope()->Type(Lower_ctx()->Get_cipher_type_id()),
                  Gen_tmp_name(), spos);
  Tmp_var().push_back(VAR(fs, var));
  return var;
}

air::base::ADDR_DATUM_PTR POLY_IR_GEN::New_ciph3_var(const SPOS& spos) {
  air::base::FUNC_SCOPE*    fs = Container()->Parent_func_scope();
  air::base::ADDR_DATUM_PTR var =
      fs->New_var(Glob_scope()->Type(Lower_ctx()->Get_cipher3_type_id()),
                  Gen_tmp_name(), spos);
  Tmp_var().push_back(VAR(fs, var));
  return var;
}

air::base::ADDR_DATUM_PTR POLY_IR_GEN::New_poly_var(const SPOS& spos) {
  air::base::FUNC_SCOPE*    fs = Container()->Parent_func_scope();
  air::base::ADDR_DATUM_PTR var =
      fs->New_var(Poly_type(), Gen_tmp_name(), spos);
  Tmp_var().push_back(VAR(fs, var));
  return var;
}

air::base::ADDR_DATUM_PTR POLY_IR_GEN::New_polys_var(const SPOS& spos) {
  air::base::FUNC_SCOPE*    fs = Container()->Parent_func_scope();
  air::base::ADDR_DATUM_PTR var =
      fs->New_var(Get_type(POLY_PTR_PTR, spos), Gen_tmp_name(), spos);
  Tmp_var().push_back(VAR(fs, var));
  return var;
}

FIELD_PTR POLY_IR_GEN::Get_poly_fld(CONST_VAR var, uint32_t fld_id) {
  TYPE_PTR t_var = var.Type();

  CMPLR_ASSERT(fld_id <= 2, "invalid field id");
  CMPLR_ASSERT((Lower_ctx()->Is_cipher3_type(t_var->Id()) ||
                Lower_ctx()->Is_cipher_type(t_var->Id()) ||
                Lower_ctx()->Is_plain_type(t_var->Id())),
               "var is not CIPHER3/CIPHER/PLAIN type");
  CMPLR_ASSERT(t_var->Is_record(), "not a record type");

  FIELD_ITER fld_iter     = t_var->Cast_to_rec()->Begin();
  FIELD_ITER fld_iter_end = t_var->Cast_to_rec()->End();
  uint32_t   idx          = 0;
  while (idx < fld_id) {
    ++fld_iter;
    ++idx;
    CMPLR_ASSERT(fld_iter != fld_iter_end, "fld id outof range")
  }
  TYPE_PTR fld_ty = (*fld_iter)->Type();
  CMPLR_ASSERT(Lower_ctx()->Is_poly_type(fld_ty->Id()),
               "fld is not polynomial type");
  return *fld_iter;
}

void POLY_IR_GEN::Enter_func(FUNC_SCOPE* fscope) {
  _func_scope = fscope;
  _glob_scope = &(fscope->Glob_scope());
  _container  = &(fscope->Container());
}

TYPE_PTR POLY_IR_GEN::Poly_type() {
  TYPE_ID ty_id = Lower_ctx()->Get_poly_type_id();
  CMPLR_ASSERT(!ty_id.Is_null(), "poly type not registered yet");
  return Glob_scope()->Type(ty_id);
}

TYPE_PTR POLY_IR_GEN::New_modulus_ptr_type(const SPOS& spos) {
  GLOB_SCOPE* gs = Glob_scope();
  CMPLR_ASSERT(gs, "null scope");

  STR_PTR type_str = gs->New_str("MODULUS");

  RECORD_TYPE_PTR rec_type =
      gs->New_rec_type(RECORD_KIND::STRUCT, type_str, spos);

  STR_PTR   name_fld1      = gs->New_str("val");
  STR_PTR   name_fld2      = gs->New_str("br_k");
  STR_PTR   name_fld3      = gs->New_str("br_m");
  TYPE_PTR  fld1_type      = gs->Prim_type(PRIMITIVE_TYPE::INT_U64);
  size_t    elem_byte_size = sizeof(uint64_t);
  FIELD_PTR fld1           = gs->New_fld(name_fld1, fld1_type, rec_type, spos);
  FIELD_PTR fld2           = gs->New_fld(name_fld2, fld1_type, rec_type, spos);
  FIELD_PTR fld3           = gs->New_fld(name_fld3, fld1_type, rec_type, spos);
  rec_type->Add_fld(fld1->Id());
  rec_type->Add_fld(fld2->Id());
  rec_type->Add_fld(fld3->Id());
  rec_type->Set_complete();

  TYPE_PTR ptr_type = gs->New_ptr_type(rec_type->Id(), POINTER_KIND::FLAT64);
  return ptr_type;
}

TYPE_PTR POLY_IR_GEN::New_swk_ptr_type(const SPOS& spos) {
  GLOB_SCOPE* gs = Glob_scope();
  CMPLR_ASSERT(gs, "null scope");

  STR_PTR type_str = gs->New_str("SWITCH_KEY");

  // create an empty struct
  RECORD_TYPE_PTR rec_type =
      gs->New_rec_type(RECORD_KIND::STRUCT, type_str, spos);

  TYPE_PTR ptr_type = gs->New_ptr_type(rec_type->Id(), POINTER_KIND::FLAT64);
  return ptr_type;
}

TYPE_PTR POLY_IR_GEN::New_pubkey_ptr_type(const SPOS& spos) {
  GLOB_SCOPE* gs = Glob_scope();
  CMPLR_ASSERT(gs, "null scope");

  STR_PTR type_str = gs->New_str("PUBLIC_KEY");

  // create an empty struct
  RECORD_TYPE_PTR rec_type =
      gs->New_rec_type(RECORD_KIND::STRUCT, type_str, spos);

  TYPE_PTR ptr_type = gs->New_ptr_type(rec_type->Id(), POINTER_KIND::FLAT64);
  return ptr_type;
}

NODE_PTR POLY_IR_GEN::New_bin_arith(uint32_t domain, uint32_t opcode,
                                    NODE_PTR lhs, NODE_PTR rhs,
                                    const SPOS& spos) {
  NODE_PTR bin_op = Container()->New_bin_arith(
      air::base::OPCODE(domain, opcode), lhs, rhs, spos);
  return bin_op;
}

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

NODE_PTR POLY_IR_GEN::New_var_load(CONST_VAR var, const SPOS& spos) {
  CMPLR_ASSERT(!var.Is_null(), "null preg or symbol");
  if (var.Is_preg()) {
    if (var.Fld() == Null_ptr) {
      return Container()->New_ldp(var.Preg_var(), spos);
    } else {
      return Container()->New_ldpf(var.Preg_var(), var.Fld(), spos);
    }
  } else {
    if (var.Fld() == Null_ptr) {
      return Container()->New_ld(var.Addr_var(), spos);
    } else {
      return Container()->New_ldf(var.Addr_var(), var.Fld(), spos);
    }
  }
}

STMT_PTR POLY_IR_GEN::New_var_store(NODE_PTR val, CONST_VAR var,
                                    const SPOS& spos) {
  CMPLR_ASSERT(!var.Is_null(), "null preg or symbol");
  if (var.Is_preg()) {
    if (var.Fld() == Null_ptr) {
      return Container()->New_stp(val, var.Preg_var(), spos);

    } else {
      return Container()->New_stpf(val, var.Preg_var(), var.Fld(), spos);
    }
  } else {
    if (var.Fld() == Null_ptr) {
      return Container()->New_st(val, var.Addr_var(), spos);
    } else {
      return Container()->New_stf(val, var.Addr_var(), var.Fld(), spos);
    }
  }
}

NODE_PAIR POLY_IR_GEN::New_ciph_poly_load(CONST_VAR v_ciph, bool is_rns,
                                          const SPOS& spos) {
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
  AIR_ASSERT_MSG(Lower_ctx()->Is_cipher3_type(v_ciph3.Type_id()),
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
  CMPLR_ASSERT(n_poly->Rtype() == Get_type(POLY, n_poly->Spos()),
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

FUNC_SCOPE* POLY_IR_GEN::New_func(core::FHE_FUNC func_id, const SPOS& spos) {
  const POLY_FUNC_INFO* func_info = Poly_func_info(func_id);
  AIR_ASSERT(func_info);

  GLOB_SCOPE* glob     = Glob_scope();
  STR_PTR     func_str = glob->New_str(func_info->_fname);
  FUNC_PTR    func_ptr = glob->New_func(func_str, spos);
  func_ptr->Set_parent(glob->Comp_env_id());

  SIGNATURE_TYPE_PTR func_sig = glob->New_sig_type();
  glob->New_ret_param(Get_type(func_info->_retv_type, spos), func_sig);

  for (uint32_t idx = 0; idx < func_info->_num_params; idx++) {
    const char* param_name = func_info->_param_names[idx];
    TYPE_PTR    param_type = Get_type(func_info->_param_types[idx], spos);
    STR_PTR     str_parm   = glob->New_str(param_name);
    glob->New_param(str_parm, param_type, func_sig, spos);
  }
  func_sig->Set_complete();

  glob->New_global_entry_point(func_sig, func_ptr, func_str, spos);
  return &(glob->New_func_scope(func_ptr));
}

NODE_PTR POLY_IR_GEN::New_encode(NODE_PTR n_data, NODE_PTR n_len,
                                 NODE_PTR n_scale, NODE_PTR n_level,
                                 NODE_PTR n_is_ext, const SPOS& spos) {
  CONTAINER* cntr = Container();
  NODE_PTR   n_encode =
      cntr->New_cust_node(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                            fhe::ckks::CKKS_OPERATOR::ENCODE),
                          Get_type(PLAIN, spos), spos);
  n_encode->Set_child(0, n_data);
  n_encode->Set_child(1, n_len);
  n_encode->Set_child(2, n_scale);
  n_encode->Set_child(3, n_level);
  n_encode->Set_child(4, n_is_ext);

  return n_encode;
}

NODE_PTR POLY_IR_GEN::New_poly_add(NODE_PTR opnd1, NODE_PTR opnd2,
                                   const SPOS& spos) {
  CMPLR_ASSERT(opnd1->Rtype() == Get_type(POLY, spos), "invalid type");
  CMPLR_ASSERT((opnd1->Rtype() == Get_type(POLY, spos) ||
                opnd2->Rtype()->Is_int() || opnd2->Rtype()->Is_float()),
               "invalid type");
  NODE_PTR add_node = New_poly_node(ADD, opnd1->Rtype(), spos);
  add_node->Set_child(0, opnd1);
  add_node->Set_child(1, opnd2);
  return add_node;
}

NODE_PTR POLY_IR_GEN::New_poly_mul(NODE_PTR opnd1, NODE_PTR opnd2,
                                   const SPOS& spos) {
  CMPLR_ASSERT(opnd1->Rtype() == Get_type(POLY, spos), "invalid type");
  CMPLR_ASSERT((opnd1->Rtype() == Get_type(POLY, spos) ||
                opnd2->Rtype()->Is_int() || opnd2->Rtype()->Is_float()),
               "invalid type");

  NODE_PTR mul_node = New_poly_node(MUL, opnd1->Rtype(), spos);
  mul_node->Set_child(0, opnd1);
  mul_node->Set_child(1, opnd2);
  return mul_node;
}

NODE_PTR POLY_IR_GEN::New_poly_rotate(NODE_PTR opnd1, NODE_PTR opnd2,
                                      const SPOS& spos) {
  CMPLR_ASSERT(opnd1->Rtype() == Get_type(POLY, spos), "invalid type");
  CMPLR_ASSERT(opnd2->Rtype()->Is_int(), "invalid type");

  NODE_PTR n_res = New_poly_node(ROTATE, opnd1->Rtype(), spos);
  n_res->Set_child(0, opnd1);
  n_res->Set_child(1, opnd2);
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
  CMPLR_ASSERT(Lower_ctx()->Is_cipher_type(node->Rtype_id()) ||
                   Lower_ctx()->Is_cipher3_type(node->Rtype_id()),
               "not ciphertext type");

  CONST_VAR& v_res = v_parent.Is_null() ? Node_var(node) : v_parent;
  SPOS       spos  = node->Spos();
  if (node->Domain() == fhe::ckks::CKKS_DOMAIN::ID) {
    switch (node->Operator()) {
      case fhe::ckks::CKKS_OPERATOR::ADD:
      case fhe::ckks::CKKS_OPERATOR::SUB:
      case fhe::ckks::CKKS_OPERATOR::MUL: {
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
        CONST_VAR& v_opnd0 = Node_var(node->Child(0));
        NODE_PTR   n_res   = New_var_load(v_res, spos);
        NODE_PTR   n_opnd0 = New_var_load(v_opnd0, spos);
        return New_init_ciph_down_scale(n_res, n_opnd0, spos);
      }
      default:
        CMPLR_ASSERT(false, "not supported operator");
    }
  } else if (node->Domain() == air::core::CORE &&
             node->Operator() == air::core::LD) {
    VAR v_opnd0(Func_scope(), node->Addr_datum());
    if (v_res != v_opnd0) {
      NODE_PTR n_res   = New_var_load(v_res, spos);
      NODE_PTR n_opnd0 = New_var_load(v_opnd0, spos);
      TYPE_PTR t_opnd1 = Glob_scope()->New_ptr_type(
          Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_U64)->Id(),
          POINTER_KIND::FLAT64);
      NODE_PTR n_opnd1 = Container()->New_zero(t_opnd1, spos);
      return New_init_ciph_same_scale(n_res, n_opnd0, n_opnd1, spos);
    }
  } else {
    CMPLR_ASSERT(false, "not supported operator");
  }
  return STMT_PTR();
}

STMT_PTR POLY_IR_GEN::New_init_poly(CONST_VAR v_res, NODE_PTR node,
                                    bool is_ext) {
  CMPLR_ASSERT(Lower_ctx()->Is_poly_type(node->Rtype_id()), "not poly type");
  AIR_ASSERT(node->Num_child() >= 1);

  SPOS     spos    = node->Spos();
  NODE_PTR n_res   = New_var_load(v_res, spos);
  NODE_PTR n_opnd0 = New_var_load(Node_var(node->Child(0)), spos);
  NODE_PTR n_opnd1 = Null_ptr;
  if (node->Num_child() == 2 &&
      Lower_ctx()->Is_poly_type(node->Child(1)->Rtype_id())) {
    n_opnd1 = New_var_load(Node_var(node->Child(1)), spos);
  }
  return New_init_poly(n_res, n_opnd0, n_opnd1, is_ext, spos);
}

STMT_PTR POLY_IR_GEN::New_init_poly(NODE_PTR n_res, NODE_PTR n_opnd1,
                                    NODE_PTR n_opnd2, bool is_ext,
                                    const SPOS& spos) {
  CMPLR_ASSERT(Lower_ctx()->Is_poly_type(n_res->Rtype_id()) &&
                   Lower_ctx()->Is_poly_type(n_opnd1->Rtype_id()),
               "not poly type");

  TYPE_PTR t_bool = Glob_scope()->Prim_type(PRIMITIVE_TYPE::BOOL);
  STMT_PTR s_init = New_poly_stmt(INIT_POLY, spos);
  s_init->Node()->Set_child(0, n_res);
  s_init->Node()->Set_child(1, n_opnd1);
  if (n_opnd2 != air::base::Null_ptr) {
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

NODE_PTR POLY_IR_GEN::New_degree(const SPOS& spos) {
  air::base::CONST_TYPE_PTR ui32_type =
      Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_U32);
  NODE_PTR n_degree = New_poly_node(DEGREE, ui32_type, spos);
  return n_degree;
}

NODE_PTR POLY_IR_GEN::New_alloc_poly(NODE_PTR poly_node, bool is_ext,
                                     const SPOS& spos) {
  CMPLR_ASSERT(poly_node->Rtype() == Poly_type(), "node is not polynomial");

  NODE_PTR                  n_degree = New_degree(spos);
  NODE_PTR                  n_num_q  = New_get_num_q(poly_node, spos);
  air::base::CONST_TYPE_PTR ui32_type =
      Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_U32);
  NODE_PTR n_is_ext;
  if (is_ext) {
    n_is_ext = Container()->New_intconst(ui32_type, 1, spos);
  } else {
    n_is_ext = Container()->New_intconst(ui32_type, 0, spos);
  }

  return New_alloc_poly(n_degree, n_num_q, n_is_ext, spos);
}

NODE_PTR POLY_IR_GEN::New_alloc_poly(uint32_t prime_cnt, const SPOS& spos) {
  CONTAINER* cntr     = Container();
  NODE_PTR   n_degree = New_degree(spos);
  TYPE_PTR   t_ui32 =
      cntr->Glob_scope()->Prim_type(air::base::PRIMITIVE_TYPE::INT_U32);
  NODE_PTR n_cst_one = cntr->New_intconst(t_ui32, 1, spos);
  NODE_PTR n_is_ext  = cntr->New_intconst(t_ui32, 0, spos);
  return New_alloc_poly(n_degree, n_cst_one, n_is_ext, spos);
}

NODE_PTR POLY_IR_GEN::New_alloc_poly(NODE_PTR n_degree, NODE_PTR n_num_q,
                                     NODE_PTR n_is_ext, const SPOS& spos) {
  NODE_PTR n_alloc = New_poly_node(ALLOC, Get_type(POLY, spos), spos);
  n_alloc->Set_child(0, n_degree);
  n_alloc->Set_child(1, n_num_q);
  n_alloc->Set_child(2, n_is_ext);
  return n_alloc;
}

STMT_PTR POLY_IR_GEN::New_free_poly(CONST_VAR v_poly, const SPOS& spos) {
  CMPLR_ASSERT(v_poly.Type() == Get_type(POLY, spos), "invalid type");

  NODE_PTR n_poly = New_var_load(v_poly, spos);
  STMT_PTR s_free = New_poly_stmt(FREE, spos);
  s_free->Node()->Set_child(0, n_poly);
  return s_free;
}

NODE_PTR POLY_IR_GEN::New_get_num_q(NODE_PTR node, const SPOS& spos) {
  CMPLR_ASSERT(node->Rtype() == Poly_type(), "node is not polynomial");

  NODE_PTR n_level = New_poly_node(
      LEVEL, Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_U32), spos);
  n_level->Set_child(0, node);
  return n_level;
}

NODE_PTR POLY_IR_GEN::New_num_p(NODE_PTR node, const SPOS& spos) {
  CMPLR_ASSERT(node->Rtype() == Poly_type(), "node is not polynomial");
  NODE_PTR n_num_p = New_poly_node(
      NUM_P, Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_U32), spos);
  n_num_p->Set_child(0, node);
  return n_num_p;
}

NODE_PTR POLY_IR_GEN::New_num_alloc(NODE_PTR node, const SPOS& spos) {
  CMPLR_ASSERT(node->Rtype() == Poly_type(), "node is not polynomial");

  NODE_PTR n_num_alloc_primes = New_poly_node(
      NUM_ALLOC, Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_U32), spos);
  n_num_alloc_primes->Set_child(0, node);
  return n_num_alloc_primes;
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

STMT_PTR POLY_IR_GEN::New_loop(CONST_VAR induct_var, NODE_PTR upper_bound,
                               uint64_t start_idx, uint64_t increment,
                               const SPOS& spos) {
  CONTAINER*  cont = Container();
  GLOB_SCOPE* gs   = Glob_scope();
  CMPLR_ASSERT(cont && gs, "null scope");

  air::base::CONST_TYPE_PTR ui64_type =
      Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_U64);

  // cond init node
  NODE_PTR init_node = cont->New_intconst(ui64_type, start_idx, spos);

  // cond node
  NODE_PTR idx_node  = New_var_load(induct_var, spos);
  NODE_PTR comp_node = New_bin_arith(air::core::CORE, air::core::OPCODE::LT,
                                     idx_node, upper_bound, spos);

  // cond increment node
  NODE_PTR inc_val_node = cont->New_intconst(ui64_type, increment, spos);
  NODE_PTR incr_node    = New_bin_arith(air::core::CORE, air::core::OPCODE::ADD,
                                        idx_node, inc_val_node, spos);

  // create body node
  NODE_PTR body = cont->New_stmt_block(spos);

  // create do_loop stmt
  STMT_PTR do_loop = cont->New_do_loop(induct_var.Addr_var(), init_node,
                                       comp_node, incr_node, body, spos);
  return do_loop;
}

NODE_PTR POLY_IR_GEN::New_get_next_modulus(CONST_VAR   mod_var,
                                           const SPOS& spos) {
  NODE_PTR ld_mod   = New_var_load(mod_var, spos);
  NODE_PTR one_node = Container()->New_intconst(
      Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_U64), 1, spos);
  NODE_PTR inc_mod = New_bin_arith(air::core::CORE, air::core::OPCODE::ADD,
                                   ld_mod, one_node, spos);
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

void POLY_IR_GEN::Append_stmt(STMT_PTR stmt, NODE_PTR blk) {
  if (stmt == air::base::Null_ptr) return;
  CMPLR_ASSERT(blk->Is_block(), "not a block");
  air::base::STMT_LIST stmt_list(blk);
  stmt_list.Append(stmt);
}

void POLY_IR_GEN::Append_rns_stmt(STMT_PTR stmt, NODE_PTR blk) {
  if (stmt == air::base::Null_ptr) return;
  CMPLR_ASSERT(blk->Is_block(), "not a block");

  air::base::STMT_LIST stmt_list(blk);
  STMT_PTR             inc_mod = stmt_list.Last_stmt();
  CMPLR_ASSERT(inc_mod != air::base::Null_ptr, "missing inc modulus stmt");
  stmt_list.Prepend(inc_mod, stmt);
}

int32_t POLY_IR_GEN::Get_level(NODE_PTR node) { return UNK_LEVEL; }

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

NODE_PTR POLY_IR_GEN::New_precomp(NODE_PTR node, const SPOS& spos) {
  CMPLR_ASSERT(node->Rtype() == Get_type(POLY, spos),
               "node is not polynonmial");

  NODE_PTR n_res = New_poly_node(PRECOMP, Get_type(POLY_PTR_PTR, spos), spos);
  n_res->Set_child(0, node);
  return n_res;
}

NODE_PTR POLY_IR_GEN::New_dot_prod(NODE_PTR n1, NODE_PTR n2, NODE_PTR n3,
                                   const SPOS& spos) {
  CMPLR_ASSERT((n1->Rtype() == n2->Rtype() &&
                n1->Rtype() == Get_type(POLY_PTR_PTR, spos)),
               "New_dot_prod opnd should be poly lists");
  NODE_PTR n_res = New_poly_node(DOT_PROD, Get_type(POLY, spos), spos);
  n_res->Set_child(0, n1);
  n_res->Set_child(1, n2);
  n_res->Set_child(2, n3);
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
    CMPLR_ASSERT(n_rot_idx != air::base::Null_ptr, "rot_idx is null");
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
    CMPLR_ASSERT(n_rot_idx != air::base::Null_ptr, "rot_idx is null");
    NODE_PTR n_res  = New_poly_node(SWK_C0, Get_type(POLY_PTR_PTR, spos), spos);
    TYPE_PTR t_bool = Glob_scope()->Prim_type(PRIMITIVE_TYPE::BOOL);
    n_res->Set_child(0, Container()->New_one(t_bool, spos));
    n_res->Set_child(1, n_rot_idx);
    return n_res;
  } else {
    // relinearlize key
    NODE_PTR n_res  = New_poly_node(SWK_C0, Get_type(POLY_PTR_PTR, spos), spos);
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
    CMPLR_ASSERT(n_rot_idx != air::base::Null_ptr, "rot_idx is null");
    NODE_PTR n_res  = New_poly_node(SWK_C1, Get_type(POLY_PTR_PTR, spos), spos);
    TYPE_PTR t_bool = Glob_scope()->Prim_type(PRIMITIVE_TYPE::BOOL);
    n_res->Set_child(0, Container()->New_one(t_bool, spos));
    n_res->Set_child(1, n_rot_idx);
    return n_res;
  } else {
    // relinearlize key
    NODE_PTR n_res  = New_poly_node(SWK_C1, Get_type(POLY_PTR_PTR, spos), spos);
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
    NODE_PTR   n_sub          = New_bin_arith(air::core::CORE, air::core::SUB,
                                              n_alloc_primes, n_ld_p_num, spos);
    STMT_PTR   s_pofst        = New_var_store(n_sub, v_p_ofst, spos);
    sl_outer.Append(s_pofst);
  } else {
    // store poly level to variable
    CONST_VAR& v_num_q = Get_var(VAR_NUM_Q, spos);
    v_ub               = v_num_q;
    NODE_PTR n_num_q   = New_get_num_q(node, spos);
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

NODE_PTR POLY_IR_GEN::New_decomp_loop(NODE_PTR               node,
                                      std::vector<STMT_PTR>& body_stmts,
                                      const SPOS&            spos) {
  CONST_VAR& v_part_idx = Get_var(VAR_PART_IDX, spos);

  // loop upper bound
  air::base::CONST_TYPE_PTR ui32_type =
      Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_U32);
  NODE_PTR n_num_q_parts = New_poly_node(NUM_DECOMP, ui32_type, spos);
  n_num_q_parts->Set_child(0, node);

  STMT_PTR s_loop = New_loop(v_part_idx, n_num_q_parts, 0, 1, spos);

  NODE_PTR n_loop_body = s_loop->Node()->Child(3);
  CMPLR_ASSERT(n_loop_body->Is_block(), "not a block node");
  STMT_LIST sl_body = STMT_LIST::Enclosing_list(n_loop_body->End_stmt());

  std::vector<STMT_PTR>::iterator iter;
  for (iter = body_stmts.begin(); iter != body_stmts.end(); ++iter) {
    sl_body.Append(*iter);
  }
  return s_loop->Node();
}

STMT_PTR POLY_IR_GEN::New_adjust_p_idx(const SPOS& spos) {
  // adjust p_idx -> rns_idx + p_ofst
  CONST_VAR& v_rns_idx = Get_var(VAR_RNS_IDX, spos);
  CONST_VAR& v_p_ofst  = Get_var(VAR_P_OFST, spos);
  CONST_VAR& v_p_idx   = Get_var(VAR_P_IDX, spos);
  NODE_PTR   n_level   = New_var_load(v_rns_idx, spos);
  NODE_PTR   n_pofst   = New_var_load(v_p_ofst, spos);
  NODE_PTR   n_add =
      New_bin_arith(air::core::CORE, air::core::ADD, n_level, n_pofst, spos);
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
    NODE_PTR   n_key_pofst = New_get_num_q(New_var_load(v_key0, spos), spos);
    STMT_PTR   s_key_pofst = New_var_store(n_key_pofst, v_key_pofst, spos);
    Append_rns_stmt(s_key_pofst, outer_blk);

    v_key_idx          = Get_var(VAR_KEY_P_IDX, spos);
    n_key_pofst        = New_var_load(v_key_pofst, spos);
    NODE_PTR n_level   = New_var_load(v_rns_idx, spos);
    NODE_PTR n_key_idx = New_bin_arith(air::core::CORE, air::core::ADD, n_level,
                                       n_key_pofst, spos);
    STMT_PTR s_key_idx = New_var_store(n_key_idx, v_key_idx, spos);
    Append_rns_stmt(s_key_idx, body_blk);
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
  const char* attr_name = Lower_ctx()->Attr_name(core::FHE_ATTR_KIND::MUL_CIPH);
  node->Set_attr(attr_name, &mul_ciph, 1);
}

}  // namespace poly

}  // namespace fhe
