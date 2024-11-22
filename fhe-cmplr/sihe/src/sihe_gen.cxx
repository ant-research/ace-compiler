//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/sihe/sihe_gen.h"

#include <sys/types.h>

#include <map>
#include <vector>

#include "air/base/container.h"
#include "air/base/meta_info.h"
#include "air/base/opcode.h"
#include "air/base/ptr_wrapper.h"
#include "air/base/st.h"
#include "air/base/st_decl.h"
#include "air/base/st_enum.h"
#include "air/base/st_type.h"
#include "air/util/debug.h"
#include "air/util/messg.h"
#include "fhe/sihe/sihe_opcode.h"

namespace fhe {
namespace sihe {
using namespace air::base;

static const char* Sihe_type_name[static_cast<uint32_t>(SIHE_TYPE_KIND::LAST)] =
    {
        "PLAINTEXT",
        "CIPHERTEXT",
};

NODE_PTR SIHE_GEN::Gen_add(NODE_PTR child0, NODE_PTR child1, const SPOS& spos) {
  CMPLR_ASSERT(child0->Container() == _cntr,
               "container of child0 must be same with that of add_node");
  CMPLR_ASSERT(child1->Container() == _cntr,
               "container of child1 must be same with that of add_node");
  OPCODE   opcode(SIHE_DOMAIN::ID, SIHE_OPERATOR::ADD);
  NODE_PTR bin_node = _cntr->New_bin_arith(opcode, child0, child1, spos);
  TYPE_PTR rtype    = child0->Rtype();
  bin_node->Set_rtype(rtype);
  return bin_node;
}

NODE_PTR SIHE_GEN::Gen_mul(NODE_PTR child0, NODE_PTR child1, const SPOS& spos) {
  CMPLR_ASSERT(child0->Container() == _cntr,
               "container of child0 must be same with that of mul_node");
  CMPLR_ASSERT(child1->Container() == _cntr,
               "container of child1 must be same with that of mul_node");
  OPCODE   opcode(SIHE_DOMAIN::ID, SIHE_OPERATOR::MUL);
  NODE_PTR bin_node = _cntr->New_bin_arith(opcode, child0, child1, spos);
  TYPE_PTR rtype    = child0->Rtype();
  bin_node->Set_rtype(rtype);
  return bin_node;
}

NODE_PTR SIHE_GEN::Gen_sub(NODE_PTR child0, NODE_PTR child1, const SPOS& spos) {
  CMPLR_ASSERT(child0->Container() == _cntr,
               "container of child0 must be same with that of sub_node");
  CMPLR_ASSERT(child1->Container() == _cntr,
               "container of child1 must be same with that of sub_node");
  OPCODE   opcode(SIHE_DOMAIN::ID, SIHE_OPERATOR::SUB);
  NODE_PTR bin_node = _cntr->New_bin_arith(opcode, child0, child1, spos);
  TYPE_PTR rtype    = child0->Rtype();
  bin_node->Set_rtype(rtype);
  return bin_node;
}

NODE_PTR SIHE_GEN::Gen_encode(NODE_PTR child, TYPE_PTR plain_type,
                              const SPOS& spos) {
  CMPLR_ASSERT(child->Container() == _cntr,
               "container of child0 must be same with that of encode node");
  // CMPLR_ASSERT(child->Is_const_ld(),
  //              "only support encoding constant vector value");
  OPCODE   opcode(SIHE_DOMAIN::ID, SIHE_OPERATOR::ENCODE);
  NODE_PTR encode_node = _cntr->New_cust_node(opcode, plain_type, spos);
  encode_node->Set_child(0, child);

  ARRAY_TYPE_PTR vec_type = child->Rtype()->Cast_to_arr();
  uint32_t       len      = vec_type->Elem_count();
  TYPE_PTR       u32_type = _glob_scope->Prim_type(PRIMITIVE_TYPE::INT_U32);
  encode_node->Set_child(1, _cntr->New_intconst(u32_type, len, spos));

  return encode_node;
}

NODE_PTR SIHE_GEN::Gen_bootstrap(NODE_PTR child, const SPOS& spos) {
  CMPLR_ASSERT(child->Container() == Container(),
               "container of child must be same with that of bootstrap node");
  CMPLR_ASSERT(Lower_ctx()->Is_cipher_type(child->Rtype_id()),
               "rtype of child must be cipher");

  OPCODE   bootstrap_op(SIHE_DOMAIN::ID, SIHE_OPERATOR::BOOTSTRAP);
  NODE_PTR bs_node =
      Container()->New_cust_node(bootstrap_op, child->Rtype(), spos);
  bs_node->Set_child(0, child);
  return bs_node;
}

void SIHE_GEN::Register_sihe_types() {
  // Register cipher type
  SPOS     spos                = _glob_scope->Unknown_simple_spos();
  uint32_t cipher_type_name_id = static_cast<uint32_t>(SIHE_TYPE_KIND::CIPHER);
  STR_PTR  cipher_type_str =
      _glob_scope->New_str(Sihe_type_name[cipher_type_name_id]);
  RECORD_TYPE_PTR cipher_type =
      _glob_scope->New_rec_type(RECORD_KIND::STRUCT, cipher_type_str, spos);
  _lower_ctx->Set_cipher_type_id(cipher_type->Id());

  // Register plain type
  uint32_t plain_type_name_id = static_cast<uint32_t>(SIHE_TYPE_KIND::PLAIN);
  STR_PTR  plain_type_str =
      _glob_scope->New_str(Sihe_type_name[plain_type_name_id]);
  RECORD_TYPE_PTR plain_type =
      _glob_scope->New_rec_type(RECORD_KIND::STRUCT, plain_type_str, spos);
  _lower_ctx->Set_plain_type_id(plain_type->Id());
}
}  // namespace sihe
}  // namespace fhe
