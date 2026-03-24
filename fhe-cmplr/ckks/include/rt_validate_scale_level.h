//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_RT_VAL_SCL_LVL_H
#define FHE_CKKS_RT_VAL_SCL_LVL_H
#include <algorithm>

#include "air/base/analyze_ctx.h"
#include "air/base/container.h"
#include "air/base/container_decl.h"
#include "air/base/st.h"
#include "air/base/st_decl.h"
#include "air/base/st_enum.h"
#include "air/base/visitor.h"
#include "air/core/opcode.h"
#include "air/driver/driver_ctx.h"
#include "air/util/debug.h"
#include "air/util/error.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/ckks/config.h"
#include "fhe/core/lower_ctx.h"
#include "fhe/core/scheme_info.h"
namespace fhe {
namespace ckks {

//! @brief Lower the polynomial degree to MAX_POLY_DEG for faster FHE operations
//! during runtime validation of ciphertext scale and level.
#define MAX_POLY_DEG 1024U

//! @brief Rotate index used during runtime validation of ciphertext scale and
//! level.
#define DEFAULT_ROT_IDX 1

//! @brief Value used to replace multiplied or added plaintext or constant in
//! ciphertext-plaintext multiplication/addition to prevent ciphertext growth,
//! especially in approximated activation functions like ReLU. A small float
//! value instead of zero is used here to avoid any kind optimization in follow
//! domains.
#define SMALL_FLT_CST (1.0E-10)

//! @brief Context used in update CKKS IR for runtime validating ciphertext
//! scale and level.
class RT_VALIDATE_SCL_LVL_CTX : public air::base::ANALYZE_CTX {
  using DRIVER_CTX = air::driver::DRIVER_CTX;
  using NODE_PTR   = air::base::NODE_PTR;
  using TYPE_PTR   = air::base::TYPE_PTR;
  using TYPE_ID    = air::base::TYPE_ID;

public:
  RT_VALIDATE_SCL_LVL_CTX(const DRIVER_CTX* driver_ctx, const CKKS_CONFIG* cfg,
                          core::LOWER_CTX* fhe_ctx)
      : air::base::ANALYZE_CTX(),
        _driver_ctx(driver_ctx),
        _config(cfg),
        _fhe_ctx(fhe_ctx) {}
  ~RT_VALIDATE_SCL_LVL_CTX() {}

  //! @brief Default NODE handler:
  //! Reset rotate index to DEFAULT_ROT_IDX to reduce memory usage for rotation
  //! keys. Set multiplied or added plaintext/float value to SMALL_FLT_CST to
  //! prevent ciphertext value increase.
  template <typename RETV, typename VISITOR>
  RETV Handle_node(VISITOR* visitor, NODE_PTR node) {
    switch (node->Opcode()) {
      case OPC_ROTATE: {
        AIR_ASSERT(node->Num_child() == 2);
        constexpr uint32_t ROT_IDX      = 1U;
        NODE_PTR           orig_rot_idx = node->Child(ROT_IDX);
        NODE_PTR           new_rot_idx  = node->Container()->New_intconst(
            orig_rot_idx->Rtype(), DEFAULT_ROT_IDX, orig_rot_idx->Spos());
        node->Set_child(ROT_IDX, new_rot_idx);
        _fhe_ctx->Get_ctx_param().Add_rotate_index(ROT_IDX);

        Trace(TD_RT_VAL, "RT_VALIDATE_SCL_LVL reset rotate index to ",
              DEFAULT_ROT_IDX, " of: ");
        Trace_obj(TD_RT_VAL, node);
        break;
      }
      case OPC_MUL:
      case OPC_ADD: {
        AIR_ASSERT(node->Num_child() == 2);
        NODE_PTR opnd1 = node->Child(1);
        TYPE_PTR type  = opnd1->Rtype();
        if (type->Id() == _fhe_ctx->Get_cipher_type_id()) {
          break;
        } else {
          AIR_ASSERT(type->Id() == _fhe_ctx->Get_plain_type_id() ||
                     type->Is_float());
        }
        air::base::CONTAINER*  cont = node->Container();
        air::base::GLOB_SCOPE* glob = cont->Glob_scope();
        if (!type->Is_float()) {
          type = glob->Prim_type(air::base::PRIMITIVE_TYPE::FLOAT_32);
        }
        air::base::CONSTANT_PTR cst =
            glob->New_const(air::base::CONSTANT_KIND::FLOAT, type,
                            (long double)(SMALL_FLT_CST));
        NODE_PTR ldc = cont->New_ldc(cst, node->Spos());
        node->Set_child(1, ldc);
        Trace(TD_RT_VAL,
              "RT_VALIDATE_SCL_LVL reset mul/add plaintext/float to ",
              SMALL_FLT_CST, " of: ");
        Trace_obj(TD_RT_VAL, node);
        break;
      }
      case air::core::OPC_BLOCK: {
        Handle_block<RETV>(visitor, node);
        break;
      }
      default: {
        break;
      }
    }
    for (uint32_t i = 0; i < node->Num_child(); ++i) {
      this->template Handle_node<RETV>(visitor, node->Child(i));
    }
    return RETV();
  }

  //! @brief unknown DOMAIN handler: traverse each child node.
  template <typename RETV, typename VISITOR>
  RETV Handle_unknown_domain(VISITOR* visitor, NODE_PTR node) {
    for (uint32_t i = 0; i < node->Num_child(); ++i) {
      this->template Handle_node<RETV>(visitor, node->Child(i));
    }
    return RETV();
  }

  DECLARE_TRACE_DETAIL_API((*_config), _driver_ctx)
private:
  // REQUIRED UNDEFINED UNWANTED methods
  RT_VALIDATE_SCL_LVL_CTX(void);
  RT_VALIDATE_SCL_LVL_CTX(const RT_VALIDATE_SCL_LVL_CTX&);
  RT_VALIDATE_SCL_LVL_CTX& operator=(const RT_VALIDATE_SCL_LVL_CTX&);

  const DRIVER_CTX*  _driver_ctx = nullptr;
  const CKKS_CONFIG* _config     = nullptr;
  core::LOWER_CTX*   _fhe_ctx    = nullptr;
};

//! @brief RT_VAL_SCL_LVL ensures runtime validation of ciphertext scale
//! and level correctness. RT_VAL_SCL_LVL does not preserve message
//! correctness in ciphertexts. To accelerate FHE operations and reduce memory
//! usage, it reduces the polynomial degree to MAX_POLY_DEG, resets each
//! rotation index to DEFAULT_ROT_IDX, and sets each multiplied or added
//! plaintext/float value to a small float value.
class RT_VAL_SCL_LVL {
  using DRIVER_CTX = air::driver::DRIVER_CTX;
  using GLOB_SCOPE = air::base::GLOB_SCOPE;

public:
  RT_VAL_SCL_LVL(const DRIVER_CTX* driver_ctx, const CKKS_CONFIG* cfg,
                 GLOB_SCOPE* glob, core::LOWER_CTX* lower_ctx)
      : _driver_ctx(driver_ctx),
        _config(cfg),
        _glob(glob),
        _fhe_ctx(lower_ctx) {
    AIR_ASSERT(cfg->Rt_val_scl_lvl());
  }
  ~RT_VAL_SCL_LVL() {}

  void Run() {
    // 1. Reset the polynomial degree to a maximum of MAX_POLY_DEG to reduce
    // execution time.
    core::CTX_PARAM& ctx_param = _fhe_ctx->Get_ctx_param();
    uint32_t poly_deg = std::min(MAX_POLY_DEG, ctx_param.Get_poly_degree());
    ctx_param.Set_poly_degree(poly_deg, false);
    ctx_param.Clear_rotate_index();
    // 2. traverse each function replace rotation index with DEFAULT_ROT_IDX,
    // and sets each multiplied or added plaintext to a small value.
    for (GLOB_SCOPE::FUNC_SCOPE_ITER func_iter = _glob->Begin_func_scope();
         func_iter != _glob->End_func_scope(); ++func_iter) {
      RT_VALIDATE_SCL_LVL_CTX ctx(_driver_ctx, _config, _fhe_ctx);
      air::base::VISITOR<RT_VALIDATE_SCL_LVL_CTX> visitor(ctx);
      air::base::NODE_PTR entry = (*func_iter).Container().Entry_node();
      visitor.Handle_node<void>(entry);
    }
  }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  RT_VAL_SCL_LVL(void);
  RT_VAL_SCL_LVL(const RT_VAL_SCL_LVL&);
  RT_VAL_SCL_LVL& operator=(const RT_VAL_SCL_LVL&);

  const DRIVER_CTX*  _driver_ctx = nullptr;
  const CKKS_CONFIG* _config     = nullptr;
  GLOB_SCOPE*        _glob       = nullptr;
  core::LOWER_CTX*   _fhe_ctx;
};

}  // namespace ckks
}  // namespace fhe

#endif  // FHE_CKKS_RT_VAL_SCL_LVL_H
