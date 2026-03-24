//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_POLY_IR_GEN_H
#define FHE_POLY_IR_GEN_H

#include "fhe/poly/ir_gen.h"

#include <iostream>
#include <sstream>

#include "air/base/container.h"
#include "air/core/opcode.h"
#include "fhe/core/rt_context.h"

namespace fhe {
namespace poly {

using namespace air::base;

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
    {VAR_BCONV_IDX,   INDEX,        "_pgen_bconv_idx"  },
    {VAR_MODULUS,     MODULUS_PTR,  "_pgen_modulus"    },
    {VAR_SWK,         SWK_PTR,      "_pgen_swk"        },
    {VAR_SWK_C0,      POLY,         "_pgen_swk_c0"     },
    {VAR_SWK_C1,      POLY,         "_pgen_swk_c1"     },
    {VAR_DECOMP,      POLY,         "_pgen_decomp"     },
    {VAR_EXT,         POLY,         "_pgen_ext"        },
    {VAR_PUB_KEY0,    POLY,         "_pgen_key0"       },
    {VAR_PUB_KEY1,    POLY,         "_pgen_key1"       },
    {VAR_MOD_DOWN_C0, POLY,         "_pgen_mod_down_c0"},
    {VAR_MOD_DOWN_C1, POLY,         "_pgen_mod_down_c1"},
    {VAR_AUTO_ORDER,  INT64_PTR,    "_pgen_order"      },
    {VAR_TMP_POLY,    POLY,         "_pgen_tmp_poly"   },
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

TYPE_PTR IR_GEN::Get_type(VAR_TYPE_KIND id, const SPOS& spos) {
  CMPLR_ASSERT(id < VAR_TYPE_KIND::LAST_TYPE, "id outof bound")
  GLOB_SCOPE*           gs    = Glob_scope();
  std::vector<TYPE_ID>& types = Types();
  TYPE_PTR              ty    = Null_ptr;
  if (!types[id].Is_null()) {
    ty = gs->Type(types[id]);
    return ty;
  }
  switch (id) {
    case INDEX:
      ty = gs->Prim_type(PRIMITIVE_TYPE::INT_U32);
      break;
    case MODULUS_PTR:
      ty = New_modulus_ptr_type(spos);
      break;
    case SWK_POLYS:
      ty = New_swk_type();
      break;
    case SWK_PTR:
      ty = New_swk_ptr_type(spos);
      break;
    case PUBKEY_PTR:
      ty = New_pubkey_ptr_type(spos);
      break;
    case UINT32:
      ty = gs->Prim_type(PRIMITIVE_TYPE::INT_U32);
      break;
    case INT32:
      ty = gs->Prim_type(PRIMITIVE_TYPE::INT_S32);
      break;
    case UINT64:
      ty = gs->Prim_type(PRIMITIVE_TYPE::INT_U64);
      break;
    case INT64:
      ty = gs->Prim_type(PRIMITIVE_TYPE::INT_S64);
      break;
    case INT64_PTR:
      ty = gs->New_ptr_type(gs->Prim_type(PRIMITIVE_TYPE::INT_S64)->Id(),
                            POINTER_KIND::FLAT64);
      break;
    case UINT64_PTR:
      ty = gs->New_ptr_type(gs->Prim_type(PRIMITIVE_TYPE::INT_U64)->Id(),
                            POINTER_KIND::FLAT64);
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

bool IR_GEN::Is_type_of(air::base::TYPE_ID ty_id, VAR_TYPE_KIND kind) {
  CMPLR_ASSERT(kind < VAR_TYPE_KIND::LAST_TYPE, "kind outof bound")
  return Glob_scope()->Type(ty_id)->Is_compatible_type(Get_type(kind));
}

bool IR_GEN::Is_poly_array(air::base::TYPE_PTR ty) {
  return ty->Is_array() && Is_type_of(ty->Cast_to_arr()->Elem_type_id(), POLY);
}

bool IR_GEN::Is_int_array(air::base::TYPE_PTR ty) {
  return ty->Is_array() && ty->Cast_to_arr()->Elem_type()->Is_int();
}

bool IR_GEN::Is_fhe_type(air::base::TYPE_ID ty_id) {
  return (Is_type_of(ty_id, CIPH) || Is_type_of(ty_id, CIPH3) ||
          Is_type_of(ty_id, PLAIN) || Is_type_of(ty_id, POLY) ||
          Is_type_of(ty_id, POLY_PTR));
}

CONST_VAR& IR_GEN::Get_var(POLY_PREDEF_VAR id, const SPOS& spos) {
  CMPLR_ASSERT(id < POLY_PREDEF_VAR::LAST_VAR, "id outof bound")
  FUNC_SCOPE*              fs         = Container()->Parent_func_scope();
  std::map<uint64_t, VAR>& predef_var = Predef_var();

  uint64_t idx = id + ((uint64_t)(fs->Id().Value()) << 32);
  if (predef_var.find(idx) != predef_var.end()) {
    CMPLR_ASSERT(predef_var[idx].Func_scope() == fs, "invalid predef var map");
    return predef_var[idx];
  } else {
    const char* name = Pred_var_info[id]._name;
    GLOB_SCOPE* gs   = Container()->Glob_scope();
    STR_PTR     str  = gs->New_str(name);
    TYPE_PTR    ty   = Get_type(Pred_var_info[id]._kind, spos);
    VAR         var(fs, fs->New_var(ty, str, spos));
    predef_var[idx] = var;
    return predef_var[idx];
  }
}

STR_PTR IR_GEN::Gen_tmp_name() {
  std::string name("_pgen_tmp_");
  name.append(std::to_string(Tmp_var().size()));
  GLOB_SCOPE* gs  = Container()->Glob_scope();
  STR_PTR     str = gs->New_str(name.c_str());
  return str;
}

void IR_GEN::Enter_func(FUNC_SCOPE* fscope) {
  _func_scope = fscope;
  _glob_scope = &(fscope->Glob_scope());
  _container  = &(fscope->Container());
}

TYPE_PTR IR_GEN::Poly_type() {
  TYPE_ID ty_id = Lower_ctx()->Get_rns_poly_type_id();
  CMPLR_ASSERT(!ty_id.Is_null(), "poly type not registered yet");
  return Glob_scope()->Type(ty_id);
}

ADDR_DATUM_PTR IR_GEN::New_plain_var(const SPOS& spos) {
  FUNC_SCOPE*    fs = Container()->Parent_func_scope();
  ADDR_DATUM_PTR var =
      fs->New_var(Glob_scope()->Type(Lower_ctx()->Get_plain_type_id()),
                  Gen_tmp_name(), spos);
  Tmp_var().emplace_back(fs, var);
  return var;
}

ADDR_DATUM_PTR IR_GEN::New_ciph_var(const SPOS& spos) {
  FUNC_SCOPE*    fs = Container()->Parent_func_scope();
  ADDR_DATUM_PTR var =
      fs->New_var(Glob_scope()->Type(Lower_ctx()->Get_cipher_type_id()),
                  Gen_tmp_name(), spos);
  Tmp_var().emplace_back(fs, var);
  return var;
}

ADDR_DATUM_PTR IR_GEN::New_ciph3_var(const SPOS& spos) {
  FUNC_SCOPE*    fs = Container()->Parent_func_scope();
  ADDR_DATUM_PTR var =
      fs->New_var(Glob_scope()->Type(Lower_ctx()->Get_cipher3_type_id()),
                  Gen_tmp_name(), spos);
  Tmp_var().emplace_back(fs, var);
  return var;
}

ADDR_DATUM_PTR IR_GEN::New_poly_var(const SPOS& spos) {
  FUNC_SCOPE*    fs  = Container()->Parent_func_scope();
  ADDR_DATUM_PTR var = fs->New_var(Poly_type(), Gen_tmp_name(), spos);
  Tmp_var().emplace_back(fs, var);
  return var;
}

CONST_VAR IR_GEN::New_poly_preg() {
  FUNC_SCOPE* fs   = Container()->Parent_func_scope();
  PREG_PTR    preg = fs->New_preg(Poly_type());
  Tmp_var().emplace_back(fs, preg);
  return Tmp_var().back();
}

ADDR_DATUM_PTR IR_GEN::New_polys_var(const SPOS& spos) {
  FUNC_SCOPE*    fs = Container()->Parent_func_scope();
  ADDR_DATUM_PTR var =
      fs->New_var(Get_type(POLY_PTR, spos), Gen_tmp_name(), spos);
  Tmp_var().emplace_back(fs, var);
  return var;
}

CONST_VAR IR_GEN::New_ciph_preg() {
  FUNC_SCOPE* fs   = Container()->Parent_func_scope();
  PREG_PTR    preg = fs->New_preg(Get_type(CIPH));
  Tmp_var().emplace_back(fs, preg);
  return Tmp_var().back();
}

ADDR_DATUM_PTR IR_GEN::New_swk_var(const air::base::SPOS& spos) {
  FUNC_SCOPE*    fs = Container()->Parent_func_scope();
  ADDR_DATUM_PTR var =
      fs->New_var(Get_type(SWK_POLYS, spos), Gen_tmp_name(), spos);
  Tmp_var().emplace_back(fs, var);
  return var;
}

FIELD_PTR IR_GEN::Get_poly_fld(CONST_VAR var, uint32_t fld_id) {
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
  CMPLR_ASSERT(Lower_ctx()->Is_rns_poly_type(fld_ty->Id()),
               "fld is not polynomial type");
  return *fld_iter;
}

TYPE_PTR IR_GEN::New_modulus_ptr_type(const SPOS& spos) {
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

TYPE_PTR IR_GEN::New_swk_type() {
  GLOB_SCOPE* gs = Glob_scope();
  CMPLR_ASSERT(gs, "null scope");
  uint32_t num_decomp = Lower_ctx()->Get_ctx_param().Get_q_part_num();
  AIR_ASSERT(num_decomp > 0);

  std::vector<int64_t> dims{num_decomp};
  TYPE_PTR             t_swk = gs->New_arr_type(Get_type(POLY), dims, SPOS());
  return t_swk;
}

TYPE_PTR IR_GEN::New_swk_ptr_type(const SPOS& spos) {
  GLOB_SCOPE* gs = Glob_scope();
  CMPLR_ASSERT(gs, "null scope");

  STR_PTR type_str = gs->New_str("SWITCH_KEY");

  // create an empty struct
  RECORD_TYPE_PTR rec_type =
      gs->New_rec_type(RECORD_KIND::STRUCT, type_str, spos);

  TYPE_PTR ptr_type = gs->New_ptr_type(rec_type->Id(), POINTER_KIND::FLAT64);
  return ptr_type;
}

TYPE_PTR IR_GEN::New_pubkey_ptr_type(const SPOS& spos) {
  GLOB_SCOPE* gs = Glob_scope();
  CMPLR_ASSERT(gs, "null scope");

  STR_PTR type_str = gs->New_str("PUBLIC_KEY");

  // create an empty struct
  RECORD_TYPE_PTR rec_type =
      gs->New_rec_type(RECORD_KIND::STRUCT, type_str, spos);

  TYPE_PTR ptr_type = gs->New_ptr_type(rec_type->Id(), POINTER_KIND::FLAT64);
  return ptr_type;
}

NODE_PTR IR_GEN::New_bin_arith(uint32_t domain, uint32_t opcode,
                               CONST_TYPE_PTR rtype, NODE_PTR lhs, NODE_PTR rhs,
                               const SPOS& spos) {
  NODE_PTR bin_op =
      Container()->New_bin_arith(OPCODE(domain, opcode), rtype, lhs, rhs, spos);
  return bin_op;
}

NODE_PTR IR_GEN::New_var_load(CONST_VAR var, const SPOS& spos) {
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

STMT_PTR IR_GEN::New_var_store(NODE_PTR val, CONST_VAR var, const SPOS& spos) {
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

FUNC_SCOPE* IR_GEN::New_func(core::FHE_FUNC func_id, const SPOS& spos) {
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

STMT_PTR IR_GEN::New_loop(CONST_VAR induct_var, NODE_PTR upper_bound,
                          uint64_t start_idx, uint64_t increment,
                          const SPOS& spos) {
  CONTAINER*  cont = Container();
  GLOB_SCOPE* gs   = Glob_scope();
  CMPLR_ASSERT(cont && gs, "null scope");

  CONST_TYPE_PTR ui64_type = Glob_scope()->Prim_type(PRIMITIVE_TYPE::INT_U64);

  // cond init node
  NODE_PTR init_node = cont->New_intconst(ui64_type, start_idx, spos);

  // cond node
  NODE_PTR idx_node  = New_var_load(induct_var, spos);
  NODE_PTR comp_node = New_bin_arith(air::core::CORE, air::core::OPCODE::LT,
                                     ui64_type, idx_node, upper_bound, spos);

  // cond increment node
  NODE_PTR inc_val_node = cont->New_intconst(ui64_type, increment, spos);
  NODE_PTR incr_node    = New_bin_arith(air::core::CORE, air::core::OPCODE::ADD,
                                        ui64_type, idx_node, inc_val_node, spos);

  // create body node
  NODE_PTR body = cont->New_stmt_block(spos);

  // create do_loop stmt
  STMT_PTR do_loop = cont->New_do_loop(induct_var.Addr_var(), init_node,
                                       comp_node, incr_node, body, spos);
  return do_loop;
}

TYPE_PTR IR_GEN::Node_ty(NODE_PTR node) const {
  if (node->Has_rtype()) {
    return node->Rtype();
  } else if (node->Has_access_type()) {
    return node->Access_type();
  } else if (node->Has_ret_var()) {
    return node->Ret_preg()->Type();
  } else {
    AIR_ASSERT_MSG(false, "unknown node type");
  }
  return air::base::Null_ptr;
}

// get num_q value from attribute fhe::core::FHE_ATTR_KIND::LEVEL
uint32_t IR_GEN::Get_num_q(NODE_PTR node) {
  AIR_ASSERT_MSG(Is_fhe_type(Node_ty_id(node)), "node is not a fhe type");
  uint32_t        num_q = 0;
  const uint32_t* num_q_attr =
      node->Attr<uint32_t>(fhe::core::FHE_ATTR_KIND::LEVEL);
  AIR_ASSERT_MSG(num_q_attr, "missing level attribute");
  num_q = *num_q_attr;
  AIR_ASSERT_MSG(num_q <= Lower_ctx()->Get_ctx_param().Get_mul_level(),
                 "invalid level");
  return num_q;
}

// get num_p value from attribute fhe::core::FHE_ATTR_KIND::NUM_P
uint32_t IR_GEN::Get_num_p(NODE_PTR node) {
  AIR_ASSERT_MSG(Is_fhe_type(Node_ty_id(node)), "node is not a fhe type");
  uint32_t        num_p = 0;
  const uint32_t* num_p_attr =
      node->Attr<uint32_t>(fhe::core::FHE_ATTR_KIND::NUM_P);
  if (num_p_attr) {
    num_p = *num_p_attr;
    AIR_ASSERT_MSG(
        num_p == Lower_ctx()->Get_ctx_param().Get_p_prime_num() || num_p == 0,
        "invalid num_p");
  }
  return num_p;
}

// num_qp = num_q + num_p
uint32_t IR_GEN::Get_num_qp(NODE_PTR node) {
  return Get_num_q(node) + Get_num_p(node);
}

uint32_t IR_GEN::Get_num_decomp(air::base::NODE_PTR node) {
  return Lower_ctx()->Get_ctx_param().Get_num_decomp(Get_num_q(node));
}

uint32_t IR_GEN::Get_poly_ofst(NODE_PTR node) {
  CONST_VAR& var = Node_var(node);
  VAR        base_var;
  AIR_ASSERT(var.Has_fld());
  if (var.Is_preg()) {
    base_var.Set_var(var.Func_scope(), var.Preg_var());
  } else if (var.Is_sym()) {
    base_var.Set_var(var.Func_scope(), var.Addr_var());
  }
  AIR_ASSERT(Lower_ctx()->Is_cipher_type(base_var.Type_id()) ||
             Lower_ctx()->Is_cipher3_type(base_var.Type_id()) ||
             Lower_ctx()->Is_plain_type(base_var.Type_id()));

  uint32_t num_qp = Get_num_qp(node);
  FIELD_ID fld_id = var.Fld()->Id();
  // ciphertext layout |c0|c1|c2|
  if (fld_id == Get_poly_fld(base_var, 0)->Id()) {
    return 0;  // c0
  } else if (fld_id == Get_poly_fld(base_var, 1)->Id()) {
    return num_qp;  // c1
  } else if (Lower_ctx()->Is_cipher3_type(base_var.Type_id()) &&
             fld_id == Get_poly_fld(base_var, 2)->Id()) {
    return 2 * num_qp;  // c2
  } else {
    AIR_ASSERT_MSG(false, "unexpect ciphertext field load");
    return 0;
  }
}

uint32_t IR_GEN::Get_sbase(NODE_PTR node) {
  // get the start RNS base index from node attribute, and add with RNS loop
  // index if s_base is non-zero
  uint32_t        s_base = 0;
  const uint32_t* s_base_attr =
      node->Attr<uint32_t>(fhe::core::FHE_ATTR_KIND::S_BASE);
  if (s_base_attr != NULL) {
    s_base = *s_base_attr;
  }
  return s_base;
}

uint32_t IR_GEN::Get_sf_deg(NODE_PTR node) {
  // get the scaling factor degree from node attribute
  uint32_t        sf_deg = 1;
  const uint32_t* sf_deg_attr =
      node->Attr<uint32_t>(fhe::core::FHE_ATTR_KIND::SCALE);
  if (sf_deg_attr != NULL) {
    sf_deg = *sf_deg_attr;
  }
  return sf_deg;
}

bool IR_GEN::Is_coeff_mode(air::base::NODE_PTR node) {
  bool            is_coeff = false;
  const uint32_t* coeff_attr =
      node->Attr<uint32_t>(fhe::core::FHE_ATTR_KIND::COEFF_MODE);
  if (coeff_attr != NULL) {
    is_coeff = (*coeff_attr != 0);
  }
  return is_coeff;
}

void IR_GEN::Set_num_q(air::base::NODE_PTR node, uint32_t num_q) {
  TYPE_ID tid = Node_ty_id(node);
  AIR_ASSERT_MSG(
      num_q >= 0 && num_q <= Lower_ctx()->Get_ctx_param().Get_mul_level(),
      "invalid level");
  AIR_ASSERT_MSG(Is_fhe_type(tid), "node is not a fhe type");

  node->Set_attr(fhe::core::FHE_ATTR_KIND::LEVEL, &num_q, 1);
}

void IR_GEN::Set_num_p(air::base::NODE_PTR node, uint32_t num_p) {
  TYPE_ID tid = Node_ty_id(node);
  AIR_ASSERT_MSG(
      num_p == Lower_ctx()->Get_ctx_param().Get_p_prime_num() || num_p == 0,
      "invalid num_p");
  AIR_ASSERT_MSG(Is_fhe_type(tid), "node is not a fhe type");

  node->Set_attr(fhe::core::FHE_ATTR_KIND::NUM_P, &num_p, 1);
}

void IR_GEN::Set_sbase(air::base::NODE_PTR node, uint32_t idx) {
  TYPE_ID tid = Node_ty_id(node);
  AIR_ASSERT_MSG(idx < Lower_ctx()->Get_ctx_param().Get_tot_prime_num(),
                 "invalid s_base index");
  AIR_ASSERT_MSG(Is_fhe_type(tid), "node is not a fhe type");

  node->Set_attr(fhe::core::FHE_ATTR_KIND::S_BASE, &idx, 1);
}

void IR_GEN::Set_sf_deg(air::base::NODE_PTR node, uint32_t sf) {
  TYPE_ID tid = Node_ty_id(node);
  AIR_ASSERT_MSG(Is_fhe_type(tid), "node is not a fhe type");

  node->Set_attr(fhe::core::FHE_ATTR_KIND::SCALE, &sf, 1);
}

void IR_GEN::Set_coeff_mode(air::base::NODE_PTR node, uint32_t value) {
  TYPE_ID tid = Node_ty_id(node);
  AIR_ASSERT_MSG(Is_fhe_type(tid), "node is not a fhe type");
  node->Set_attr(fhe::core::FHE_ATTR_KIND::COEFF_MODE, &value, 1);
}

}  // namespace poly
}  // namespace fhe
#endif