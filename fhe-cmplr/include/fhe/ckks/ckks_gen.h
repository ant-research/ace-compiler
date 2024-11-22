//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_CKKS_GEN_H
#define FHE_CKKS_CKKS_GEN_H

#include "air/base/container.h"
#include "air/base/st.h"
#include "air/core/opcode.h"
#include "air/driver/driver_ctx.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/ckks/config.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/sihe/sihe_opcode.h"

namespace fhe {
namespace ckks {
using namespace air::base;
using OPCODE = air::base::OPCODE;

class CKKS_GEN {
public:
  CKKS_GEN(GLOB_SCOPE* glob_scope, core::LOWER_CTX* ctx)
      : _glob_scope(glob_scope), _lower_ctx(ctx) {}

  CKKS_GEN(CONTAINER* cntr, core::LOWER_CTX* ctx)
      : _glob_scope(cntr->Glob_scope()), _lower_ctx(ctx), _cntr(cntr) {}

  ~CKKS_GEN() {}

  NODE_PTR Gen_rescale(NODE_PTR child) {
    AIR_ASSERT(child->Container() == _cntr);
    AIR_ASSERT(_lower_ctx->Is_cipher_type(child->Rtype_id()));
    OPCODE   rescale_op(CKKS_DOMAIN::ID, CKKS_OPERATOR::RESCALE);
    NODE_PTR rescale_node =
        _cntr->New_cust_node(rescale_op, child->Rtype(), child->Spos(), 0);
    rescale_node->Set_child(0, child);
    return rescale_node;
  }

  NODE_PTR Gen_mod_switch(NODE_PTR child) {
    AIR_ASSERT(child->Container() == _cntr);
    AIR_ASSERT(_lower_ctx->Is_cipher_type(child->Rtype_id()));
    OPCODE   ms_op(CKKS_DOMAIN::ID, CKKS_OPERATOR::MOD_SWITCH);
    NODE_PTR ms_node =
        _cntr->New_cust_node(ms_op, child->Rtype(), child->Spos(), 0);
    ms_node->Set_child(0, child);
    return ms_node;
  }

  NODE_PTR Gen_get_level(NODE_PTR child) {
    AIR_ASSERT(child->Container() == _cntr);
    AIR_ASSERT(_lower_ctx->Is_cipher_type(child->Rtype_id()));
    OPCODE   level_op(CKKS_DOMAIN::ID, CKKS_OPERATOR::LEVEL);
    TYPE_PTR u32_type = _glob_scope->Prim_type(PRIMITIVE_TYPE::INT_U32);
    NODE_PTR level_node =
        _cntr->New_cust_node(level_op, u32_type, child->Spos(), 0);
    level_node->Set_child(0, child);
    return level_node;
  }

  NODE_PTR Gen_bootstrap(NODE_PTR child, const SPOS& spos) {
    AIR_ASSERT(child->Container() == _cntr);
    AIR_ASSERT(_lower_ctx->Is_cipher_type(child->Rtype_id()));
    OPCODE   bootstrap_op(CKKS_DOMAIN::ID, CKKS_OPERATOR::BOOTSTRAP);
    NODE_PTR bootstrap_node =
        _cntr->New_cust_node(bootstrap_op, child->Rtype(), spos);
    bootstrap_node->Set_child(0, child);
    return bootstrap_node;
  }

  void Register_ckks_types();

private:
  // REQUIRED UNDEFINED UNWANTED methods
  CKKS_GEN(void);
  CKKS_GEN(const CKKS_GEN&);
  CKKS_GEN& operator=(const CKKS_GEN&);

  TYPE_ID Gen_poly_type();
  TYPE_ID Gen_cipher3_type();
  TYPE_ID Update_plain_type();
  TYPE_ID Update_cipher_type();

  GLOB_SCOPE*      _glob_scope;
  core::LOWER_CTX* _lower_ctx;
  CONTAINER*       _cntr;
};

air::base::GLOB_SCOPE* Ckks_driver(air::base::GLOB_SCOPE*         glob,
                                   core::LOWER_CTX*               lower_ctx,
                                   const air::driver::DRIVER_CTX* driver_ctx,
                                   const CKKS_CONFIG*             config);

}  // namespace ckks
}  // namespace fhe
#endif  // FHE_CKKS_CKKS_GEN_H
