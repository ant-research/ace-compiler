//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_POLY_HPOLY_VERIFY_CTX_H
#define FHE_POLY_HPOLY_VERIFY_CTX_H

#include "air/base/analyze_ctx.h"
#include "fhe/core/lower_ctx.h"
namespace fhe {
namespace poly {

//! @brief Context for verifying HPOLY IR
class HPOLY_VERIFY_CTX : public air::base::ANALYZE_CTX {
public:
  HPOLY_VERIFY_CTX(core::LOWER_CTX* lower_ctx) : _lower_ctx(lower_ctx) {}

  //! @brief Get lower context
  core::LOWER_CTX* Lower_ctx() { return _lower_ctx; }

  template <typename RETV, typename VISITOR>
  RETV Handle_node(VISITOR* visitor, air::base::NODE_PTR node) {
    for (uint32_t i = 0; i < node->Num_child(); ++i) {
      visitor->template Visit<RETV>(node->Child(i));
    }
    Verify_attr(node);
    return RETV();
  }

  //! @brief default action for unknown domain
  template <typename RETV, typename VISITOR>
  RETV Handle_unknown_domain(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(!node->Is_block());
    return Handle_node<RETV>(visitor, node);
  }

  void Verify_attr(air::base::NODE_PTR node) {
    // Only check attribute for Main_graph
    // for functions such as Relu, there is no fixed level
    if (!node->Container()
             ->Parent_func_scope()
             ->Owning_func()
             ->Entry_point()
             ->Is_program_entry()) {
      return;
    }
    if (node->Has_rtype()) {
      air::base::TYPE_ID tid       = node->Rtype_id();
      core::LOWER_CTX*   lower_ctx = Lower_ctx();
      if (lower_ctx->Is_rns_poly_type(tid)) {
        AIR_ASSERT_MSG(Has_uint32_attr(node, core::FHE_ATTR_KIND::LEVEL),
                       "Node missing the level attribute:\n%s", node->To_str());
        AIR_ASSERT_MSG(Has_uint32_attr(node, core::FHE_ATTR_KIND::SCALE),
                       "Node missing the scale attribute:\n%s", node->To_str());
      }
    }
  }

  bool Has_uint32_attr(air::base::NODE_PTR node, const char* attr) {
    const uint32_t* attr_ptr = node->Attr<uint32_t>(attr);
    if (attr_ptr != nullptr) return true;
    return false;
  }

private:
  core::LOWER_CTX* _lower_ctx;  //!< FHE context maintained since SIHE LEVEL
};

}  // namespace poly
}  // namespace fhe
#endif
