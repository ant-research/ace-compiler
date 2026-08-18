
//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_POLY_LOWER_CTX_H
#define FHE_POLY_LOWER_CTX_H

// #include "fhe/core/lower_ctx.h"
#include "fhe/poly/config.h"
#include "poly_ir_gen.h"

namespace fhe {

namespace poly {
enum RETV_KIND : uint8_t {
  RK_DEFAULT,
  RK_CIPH,
  RK_CIPH_POLY,
  RK_CIPH_RNS_POLY,
  RK_CIPH3,
  RK_CIPH3_POLY,
  RK_CIPH3_RNS_POLY,
  RK_PLAIN,
  RK_PLAIN_POLY,
  RK_PLAIN_RNS_POLY,
  RK_BLOCK,
};

//! @brief POLY LOWER handler return type
class POLY_LOWER_RETV {
public:
  POLY_LOWER_RETV() : _kind(RETV_KIND::RK_DEFAULT), _num_node(0) {}

  POLY_LOWER_RETV(NODE_PTR node)
      : _kind(RETV_KIND::RK_DEFAULT), _num_node(1), _node1(node) {}

  POLY_LOWER_RETV(RETV_KIND kind, NODE_PTR node)
      : _kind(kind), _num_node(1), _node1(node) {}

  POLY_LOWER_RETV(RETV_KIND kind, NODE_PTR node1, NODE_PTR node2)
      : _kind(kind), _num_node(2), _node1(node1), _node2(node2) {}

  POLY_LOWER_RETV(RETV_KIND kind, NODE_PTR node1, NODE_PTR node2,
                  NODE_PTR node3)
      : _kind(kind),
        _num_node(3),
        _node1(node1),
        _node2(node2),
        _node3(node3) {}

  NODE_PTR  Node() const { return _node1; }
  NODE_PTR  Node1() const { return _node1; }
  NODE_PTR  Node2() const { return _node2; }
  NODE_PTR  Node3() const { return _node3; }
  RETV_KIND Kind() const { return _kind; }
  uint32_t  Num_node() const { return _num_node; }

  bool Is_null() const {
    switch (Kind()) {
      case RK_DEFAULT:
      case RK_PLAIN_POLY:
      case RK_PLAIN_RNS_POLY:
      case RK_BLOCK:
        return Node1() == air::base::Null_ptr;
      case RK_CIPH_POLY:
      case RK_CIPH_RNS_POLY:
        return (Node1() == air::base::Null_ptr ||
                Node2() == air::base::Null_ptr);
      case RK_CIPH3_POLY:
      case RK_CIPH3_RNS_POLY:
        return (Node1() == air::base::Null_ptr ||
                Node2() == air::base::Null_ptr ||
                Node3() == air::base::Null_ptr);

      default:
        CMPLR_ASSERT(false, "unsupported POLY_LOWER_RETV kind");
    }
    return true;
  }

private:
  RETV_KIND _kind;
  uint8_t   _num_node;
  NODE_PTR  _node1;
  NODE_PTR  _node2;
  NODE_PTR  _node3;
};

//! @brief CKKS2HPOLY handler context
class POLY_LOWER_CTX : public air::base::TRANSFORM_CTX {
public:
  //! @brief Construct a new CKKS2HPOLY ctx object
  POLY_LOWER_CTX(POLY_CONFIG& config, fhe::core::LOWER_CTX* fhe_ctx,
                 CONTAINER* cntr)
      : air::base::TRANSFORM_CTX(cntr),
        _config(config),
        _fhe_ctx(fhe_ctx),
        _cntr(cntr),
        _pool(),
        _poly_gen(cntr, fhe_ctx, Mem_pool()) {
    _pool.Push();
  }

  //! @brief Destroy the CKKS2HPOLY ctx object
  ~POLY_LOWER_CTX(void) { _pool.Pop(); }

  //! @brief Get lower context
  fhe::core::LOWER_CTX* Lower_ctx() { return _fhe_ctx; }

  //! Get poly config options
  POLY_CONFIG& Config() { return _config; }

  //! @brief Get current container
  CONTAINER* Container() { return _cntr; }

  //! @brief Get POLY_IR_GEN instance
  POLY_IR_GEN& Poly_gen() { return _poly_gen; }

  //! @brief Enter a new function scope, update GLOB_SCOPE, CONTAINER,
  //! FUNC_SCOPE
  void Enter_func(FUNC_SCOPE* fscope) {
    _cntr = &(fscope->Container());
    Poly_gen().Enter_func(fscope);
  }

  //! @brief Returns a Memory pool
  POLY_MEM_POOL* Mem_pool() { return &_pool; }

  template <typename RETV, typename VISITOR>
  RETV Handle_unknown_domain(VISITOR* visitor, NODE_PTR node) {
    return Handle_node<RETV>(visitor, node);
  }

  bool Lower_to_poly(air::base::NODE_PTR node, air::base::NODE_PTR parent) {
    TYPE_ID               tid       = node->Rtype_id();
    fhe::core::LOWER_CTX* lower_ctx = Lower_ctx();
    if (!(lower_ctx->Is_cipher_type(tid) || lower_ctx->Is_cipher3_type(tid) ||
          lower_ctx->Is_plain_type(tid))) {
      return false;
    }
    if (parent->Domain() == fhe::ckks::CKKS_DOMAIN::ID) {
      switch (parent->Operator()) {
        case fhe::ckks::CKKS_OPERATOR::ADD:
        case fhe::ckks::CKKS_OPERATOR::SUB:
        case fhe::ckks::CKKS_OPERATOR::MUL:
        case fhe::ckks::CKKS_OPERATOR::ROTATE:
        case fhe::ckks::CKKS_OPERATOR::RESCALE:
        case fhe::ckks::CKKS_OPERATOR::RELIN:
        case fhe::ckks::CKKS_OPERATOR::MODSWITCH:
          return true;
        default:
          return false;
      }
    }
    return false;
  }

  bool Lower_to_lpoly(air::base::NODE_PTR node) {
    TYPE_ID               tid       = node->Rtype_id();
    fhe::core::LOWER_CTX* lower_ctx = Lower_ctx();
    if (!(lower_ctx->Is_poly_type(tid))) {
      return false;
    }
    if (node->Domain() == fhe::poly::POLYNOMIAL_DID) {
      switch (node->Operator()) {
        case fhe::poly::ADD:
        case fhe::poly::SUB:
        case fhe::poly::MUL:
        case fhe::poly::ROTATE:
        case fhe::poly::EXTEND:
        case fhe::poly::MODSWITCH:
          return true;
        default:
          return false;
      }
    }
    return false;
  }

private:
  POLY_MEM_POOL         _pool;
  POLY_CONFIG&          _config;
  fhe::core::LOWER_CTX* _fhe_ctx;
  CONTAINER*            _cntr;
  POLY_IR_GEN           _poly_gen;
};
}  // namespace poly
}  // namespace fhe
#endif