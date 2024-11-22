//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_IR2C_HANDLER_H
#define FHE_CKKS_IR2C_HANDLER_H

#include "air/base/container.h"
#include "air/base/container_decl.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/ckks/invalid_handler.h"
#include "fhe/ckks/ir2c_ctx.h"

namespace fhe {

namespace ckks {

//! @brief Handler to convert CKKS IR to C
class IR2C_HANDLER : public INVALID_HANDLER {
public:
  //! @brief Handle CKKS ADD operator
  template <typename RETV, typename VISITOR>
  void Handle_add(VISITOR* visitor, air::base::NODE_PTR node) {
    fhe::ckks::IR2C_CTX& ctx = visitor->Context();
    AIR_ASSERT(ctx.Is_cipher_type(node->Child(0)->Rtype_id()));
    air::base::NODE_PTR parent = ctx.Parent(1);
    AIR_ASSERT(parent != air::base::Null_ptr && parent->Is_st());
    if (ctx.Is_plain_type(node->Child(1)->Rtype_id())) {
      ctx << "Add_plain(&";
    } else {
      AIR_ASSERT(ctx.Is_cipher_type(node->Child(1)->Rtype_id()));
      ctx << "Add_ciph(&";
    }
    ctx.template Emit_st_var<RETV, VISITOR>(visitor, parent);
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(1));
    ctx << ")";
  }

  //! @brief Handle CKKS MUL operator
  template <typename RETV, typename VISITOR>
  void Handle_mul(VISITOR* visitor, air::base::NODE_PTR node) {
    fhe::ckks::IR2C_CTX& ctx = visitor->Context();
    AIR_ASSERT(ctx.Is_cipher_type(node->Child(0)->Rtype_id()));
    air::base::NODE_PTR parent = ctx.Parent(1);
    AIR_ASSERT(parent != air::base::Null_ptr && parent->Is_st());
    if (ctx.Is_plain_type(node->Child(1)->Rtype_id())) {
      ctx << "Mul_plain(&";
    } else if (ctx.Is_cipher3_type(node->Child(1)->Rtype_id())) {
      ctx << "Mul_ciph(&";
    } else if (ctx.Is_cipher_type(node->Child(1)->Rtype_id())) {
      ctx << "Mul_ciph(&";
    } else {
      AIR_ASSERT(node->Child(1)->Rtype()->Is_prim());
      ctx << "Mul_ciph(&";
    }
    ctx.template Emit_st_var<RETV, VISITOR>(visitor, parent);
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(1));
    ctx << ")";
  }

  //! @brief Handle CKKS ROTATE operator
  template <typename RETV, typename VISITOR>
  void Handle_encode(VISITOR* visitor, air::base::NODE_PTR node) {
    fhe::ckks::IR2C_CTX& ctx    = visitor->Context();
    air::base::NODE_PTR  parent = ctx.Parent(1);
    AIR_ASSERT(parent != air::base::Null_ptr && parent->Is_st());
    ctx.template Emit_encode<RETV, VISITOR>(visitor, parent, node);
  }

  //! @rief Handle CKKS RELIN operator
  template <typename RETV, typename VISITOR>
  void Handle_relin(VISITOR* visitor, air::base::NODE_PTR node) {
    fhe::ckks::IR2C_CTX& ctx = visitor->Context();
    AIR_ASSERT(ctx.Is_cipher3_type(node->Child(0)->Rtype_id()));
    air::base::NODE_PTR parent = ctx.Parent(1);
    AIR_ASSERT(parent != air::base::Null_ptr && parent->Is_st());
    ctx << "Relin(&";
    ctx.template Emit_st_var<RETV, VISITOR>(visitor, parent);
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ")";
  }

  //! @brief Handle CKKS RESCALE operator
  template <typename RETV, typename VISITOR>
  void Handle_rescale(VISITOR* visitor, air::base::NODE_PTR node) {
    fhe::ckks::IR2C_CTX& ctx = visitor->Context();
    AIR_ASSERT(ctx.Is_cipher_type(node->Child(0)->Rtype_id()));
    air::base::NODE_PTR parent = ctx.Parent(1);
    AIR_ASSERT(parent != air::base::Null_ptr && parent->Is_st());
    ctx << "Rescale_ciph(&";
    ctx.template Emit_st_var<RETV, VISITOR>(visitor, parent);
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ")";
  }

  //! @brief Handle CKKS ROTATE operator
  template <typename RETV, typename VISITOR>
  void Handle_rotate(VISITOR* visitor, air::base::NODE_PTR node) {
    fhe::ckks::IR2C_CTX& ctx = visitor->Context();
    AIR_ASSERT(ctx.Is_cipher_type(node->Child(0)->Rtype_id()));
    AIR_ASSERT(node->Child(1)->Rtype()->Is_signed_int());
    air::base::NODE_PTR parent = ctx.Parent(1);
    AIR_ASSERT(parent != air::base::Null_ptr && parent->Is_st());
    ctx << "Rotate_ciph(&";
    ctx.template Emit_st_var<RETV, VISITOR>(visitor, parent);
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(1));
    ctx << ")";
  }

  //! @brief Handle CKKS LEVEL operator
  template <typename RETV, typename VISITOR>
  void Handle_level(VISITOR* visitor, air::base::NODE_PTR node) {
    fhe::ckks::IR2C_CTX& ctx = visitor->Context();
    ctx << "Level(";
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ")";
  }

  //! @brief Handle CKKS SCALE operator
  template <typename RETV, typename VISITOR>
  void Handle_scale(VISITOR* visitor, air::base::NODE_PTR node) {
    fhe::ckks::IR2C_CTX& ctx = visitor->Context();
    ctx << "Sc_degree(";
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ")";
  }

};  // IR2C_HANDLER

}  // namespace ckks

}  // namespace fhe

#endif  // FHE_CKKS_IR2C_HANDLER_H
