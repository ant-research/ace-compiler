//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "scale_manager.h"

#include <string>

#include "air/base/container.h"
#include "air/base/container_decl.h"
#include "air/base/opcode.h"
#include "air/base/st_decl.h"
#include "air/util/debug.h"
#include "fhe/ckks/ckks_gen.h"
#include "fhe/ckks/ckks_opcode.h"

namespace fhe {
namespace ckks {

static const char* Prefix_of_tmp_var = "_rescale_tmp_";

void CKKS_SCALE_MANAGER::Handle_encode_in_bin_arith_node(
    STMT_PTR parent_stmt, NODE_PTR bin_node, uint32_t child0_scale) {
  OPCODE bin_arith_op = bin_node->Opcode();
  // only support CKKS.mul/add/sub
  AIR_ASSERT(bin_arith_op.Domain() == CKKS_DOMAIN::ID &&
             (bin_arith_op.Operator() == CKKS_OPERATOR::MUL ||
              bin_arith_op.Operator() == CKKS_OPERATOR::ADD ||
              bin_arith_op.Operator() == CKKS_OPERATOR::SUB));

  NODE_PTR encode_node = bin_node->Child(1);
  OPCODE   encode_op(CKKS_DOMAIN::ID, CKKS_OPERATOR::ENCODE);
  AIR_ASSERT(encode_node->Opcode() == encode_op);
  SPOS spos = bin_node->Spos();

  const uint32_t scale_child_id = 2;
  const uint32_t level_child_id = 3;

  NODE_PTR scale = encode_node->Child(scale_child_id);
  // 1. handle scale set as int_const
  if (scale->Domain() == air::core::CORE &&
      scale->Operator() == air::core::OPCODE::INTCONST) {
    uint32_t scale_val = scale->Intconst();
    if (scale_val == 0) return;

    if (scale_val != child0_scale) {
      scale = _cntr->New_intconst(scale->Rtype(), child0_scale, spos);
      encode_node->Set_child(scale_child_id, scale);
    }
    return;
  }

  // 2. handle scale get from child0
  OPCODE scale_op(CKKS_DOMAIN::ID, CKKS_OPERATOR::SCALE);
  AIR_ASSERT(scale->Opcode() == scale_op);

  NODE_PTR child0 = bin_node->Child(0);
  AIR_ASSERT(_mng_ctx->Lower_ctx()->Is_cipher_type(child0->Rtype_id()));
  if (!child0->Is_ld() || !child0->Has_sym()) {
    std::string    tmp_name(Prefix_of_tmp_var +
                            std::to_string(child0->Id().Value()));
    ADDR_DATUM_PTR tmp_var = _cntr->Parent_func_scope()->New_var(
        child0->Rtype(), tmp_name.c_str(), spos);
    STMT_PTR st_child0 = _cntr->New_st(child0, tmp_var, spos);
    STMT_LIST(parent_stmt->Parent_node()).Prepend(parent_stmt, st_child0);

    child0 = _cntr->New_ld(tmp_var, spos);
    bin_node->Set_child(0, child0);
  } else {
    NODE_PTR cipher_var = scale->Child(0);
    // scale and mul_level get from current child0, not need update
    if (cipher_var->Is_ld() && cipher_var->Has_sym() &&
        cipher_var->Addr_datum() == child0->Addr_datum()) {
      return;
    }
  }
  // reset scale and level nodes of encode
  ADDR_DATUM_PTR cipher_var = child0->Addr_datum();
  scale->Set_child(0, _cntr->New_ld(cipher_var, spos));

  NODE_PTR new_level = _ckks_gen.Gen_get_level(_cntr->New_ld(cipher_var, spos));
  encode_node->Set_child(level_child_id, new_level);
  return;
}

}  // namespace ckks
}  // namespace fhe