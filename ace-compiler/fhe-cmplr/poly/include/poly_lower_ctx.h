
//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_POLY_LOWER_CTX_H
#define FHE_POLY_LOWER_CTX_H

#include "fhe/ckks/ckks_opcode.h"
#include "fhe/poly/config.h"
#include "poly_ir_gen.h"

namespace fhe {

namespace poly {
enum RETV_KIND : uint8_t {
  RK_DEFAULT,         //!< Default return kind
  RK_CIPH,            //!< Return a CIPHERTEXT node
  RK_CIPH_POLY,       //!< Return the pair of polynomial nodes of a CIPHERTEXT
  RK_CIPH_RNS_POLY,   //!< Return the pair of RNS-expanded polynomial nodes of a
                      //!< CIPHERTEXT
  RK_CIPH3,           //!< Return a CIPHERTEXT3 node
  RK_CIPH3_POLY,      //!< Return the triple polynomial nodes of a CIPHERTEXT3
  RK_CIPH3_RNS_POLY,  //!< Return the triple RNS-expanded polynomial nodes of a
                      //!< CIPHERTEXT3
  RK_PLAIN,           //!< Return a PLAINTEXT node
  RK_PLAIN_POLY,      //!< Return the polynomial node of a PLAINTEXT
  RK_PLAIN_RNS_POLY,  //!< Return the RNS-expanded polynomial node of a
                      //!< PLAINTEXT
  RK_BLOCK,           //!< Return a block node
};

//! @brief POLY LOWER handler return class
class POLY_LOWER_RETV {
public:
  POLY_LOWER_RETV() : _kind(RETV_KIND::RK_DEFAULT), _num_node(0) {}

  POLY_LOWER_RETV(air::base::NODE_PTR node)
      : _kind(RETV_KIND::RK_DEFAULT), _num_node(1), _node1(node) {}

  POLY_LOWER_RETV(RETV_KIND kind, air::base::NODE_PTR node)
      : _kind(kind), _num_node(1), _node1(node) {}

  POLY_LOWER_RETV(RETV_KIND kind, air::base::NODE_PTR node1,
                  air::base::NODE_PTR node2)
      : _kind(kind), _num_node(2), _node1(node1), _node2(node2) {}

  POLY_LOWER_RETV(RETV_KIND kind, air::base::NODE_PTR node1,
                  air::base::NODE_PTR node2, air::base::NODE_PTR node3)
      : _kind(kind),
        _num_node(3),
        _node1(node1),
        _node2(node2),
        _node3(node3) {}

  air::base::NODE_PTR Node() const { return _node1; }
  air::base::NODE_PTR Node1() const { return _node1; }
  air::base::NODE_PTR Node2() const { return _node2; }
  air::base::NODE_PTR Node3() const { return _node3; }
  RETV_KIND           Kind() const { return _kind; }
  uint32_t            Num_node() const { return _num_node; }

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
  RETV_KIND           _kind;      //!< Return value kind
  uint8_t             _num_node;  //!< Number of return node
  air::base::NODE_PTR _node1;     //!< The first return node
  air::base::NODE_PTR _node2;     //!< The second return node(optional)
  air::base::NODE_PTR _node3;     //!< The third return node(optional)
};

//! @brief CKKS2HPOLY/H2LPOLY handler context
class POLY_LOWER_CTX : public air::base::TRANSFORM_CTX {
public:
  //! @brief Construct a new ctx object
  POLY_LOWER_CTX(POLY_CONFIG& config, air::driver::DRIVER_CTX* driver_ctx,
                 fhe::core::LOWER_CTX* fhe_ctx, air::base::CONTAINER* cntr)
      : air::base::TRANSFORM_CTX(cntr),
        _config(config),
        _driver_ctx(driver_ctx),
        _fhe_ctx(fhe_ctx),
        _cntr(cntr),
        _pool(),
        _poly_gen(cntr, fhe_ctx, Mem_pool()) {
    _pool.Push();

    if (config.Lower_to_hpoly2()) {
      fhe_ctx->Register_rt_context();
      // make sure q and p constants are created
      fhe_ctx->Get_crt_cst().Get_q(Glob_scope());
      fhe_ctx->Get_crt_cst().Get_p(Glob_scope());
    }
  }

  //! @brief Destroy the ctx object
  ~POLY_LOWER_CTX(void) { _pool.Pop(); }

  //! @brief Get lower context
  fhe::core::LOWER_CTX* Lower_ctx() { return _fhe_ctx; }

  //! Get poly config options
  POLY_CONFIG& Config() { return _config; }

  //! @brief Get current container
  air::base::CONTAINER* Container() { return _cntr; }

  //! @brief Get POLY_IR_GEN instance
  POLY_IR_GEN& Poly_gen() { return _poly_gen; }

  //! @brief Enter a new function scope, update GLOB_SCOPE,
  //! air::base::CONTAINER, air::base::FUNC_SCOPE
  void Enter_func(air::base::FUNC_SCOPE* fscope) {
    _cntr = &(fscope->Container());
    Poly_gen().Enter_func(fscope);
  }

  //! @brief Returns a Memory pool
  POLY_MEM_POOL* Mem_pool() { return &_pool; }

  template <typename RETV, typename VISITOR>
  RETV Handle_unknown_domain(VISITOR* visitor, air::base::NODE_PTR node) {
    return Handle_node<RETV>(visitor, node);
  }

  //! @brief Check if lower current node to HPOLY
  bool Lower_to_hpoly(air::base::NODE_PTR node, air::base::NODE_PTR parent) {
    AIR_ASSERT(parent != air::base::Null_ptr);
    air::base::TYPE_ID    tid       = node->Rtype_id();
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
        case fhe::ckks::CKKS_OPERATOR::RAISE_MOD:
          return true;
        default:
          return false;
      }
    } else if (parent->Is_st() && node->Is_ld()) {
      // lower to ciph/plain/ciph3 fld load/store
      return true;
    }
    return false;
  }

  bool Lower_to_hpoly2(air::base::NODE_PTR node) {
    switch (node->Opcode()) {
      case fhe::poly::OPC_PRECOMP:
      case fhe::poly::OPC_DOT_PROD:
      case fhe::poly::OPC_MOD_DOWN:
      case fhe::poly::OPC_RESCALE:
      case fhe::poly::OPC_RAISE_MOD:
        return true;
      default:
        return false;
    }
  }

  //! @brief Check if lower current node to LPOLY
  bool Lower_to_lpoly(air::base::NODE_PTR node) {
    air::base::TYPE_ID    tid       = node->Rtype_id();
    fhe::core::LOWER_CTX* lower_ctx = Lower_ctx();
    if (!(lower_ctx->Is_rns_poly_type(tid))) {
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

  DECLARE_TRACE_DETAIL_API(_config, _driver_ctx)

private:
  POLY_MEM_POOL            _pool;        //!< Memory pool for lower context
  POLY_CONFIG&             _config;      //!< POLY configs
  air::driver::DRIVER_CTX* _driver_ctx;  //!< Driver context
  fhe::core::LOWER_CTX* _fhe_ctx;   //!< FHE context maintained since SIHE LEVEL
  air::base::CONTAINER* _cntr;      //!< New container
  POLY_IR_GEN           _poly_gen;  //!< Poly IR gen helper
};
}  // namespace poly
}  // namespace fhe
#endif