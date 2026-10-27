//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_POLY_FLATTEN_UTIL_H
#define FHE_POLY_FLATTEN_UTIL_H

#include "air/base/container.h"
#include "air/base/meta_info.h"
#include "air/base/st.h"
#include "air/base/transform_util.h"
#include "air/core/opcode.h"

namespace fhe {
namespace poly {

//! @brief Utility for flatten
class FLATTEN_UTIL : public air::base::TRANSFORM_UTIL {
public:
  //! @brief Flatten node by storing to preg/var and load preg/var back
  template <typename VISITOR>
  air::base::NODE_PTR Flatten_node(VISITOR* visitor, air::base::NODE_PTR node) {
    if (Is_leaf(node) || node->Is_block()) {
      return node;
    }
    AIR_ASSERT(!node->Is_root());
    air::base::CONTAINER* cntr = visitor->Context().Container();
    const air::base::SPOS spos = node->Spos();

    air::base::STMT_PTR st;
    air::base::NODE_PTR ld;
    if (node->Opcode() != fhe::ckks::OPC_ENCODE) {
      air::base::PREG_PTR preg =
          cntr->Parent_func_scope()->New_preg(node->Rtype());
      st = cntr->New_stp(node, preg, spos);
      visitor->Context().Prepend(st);
      ld = cntr->New_ldp(preg, spos);
    } else {
      // store encode result to var.
      std::string tmp_name("enc_weight" + std::to_string(node->Id().Value()));
      air::base::ADDR_DATUM_PTR var = cntr->Parent_func_scope()->New_var(
          node->Rtype(), tmp_name.c_str(), spos);
      st = cntr->New_st(node, var, spos);
      visitor->Context().Prepend(st);
      ld = cntr->New_ld(var, spos);
    }

    // copy attribute from node to st and ld
    if (air::base::META_INFO::Has_prop<air::base::OPR_PROP::ATTR>(
            node->Opcode())) {
      st->Node()->Copy_attr(node);
      ld->Copy_attr(node);
    }
    return ld;
  }
};

}  // namespace poly

}  // namespace fhe

#endif  // FHE_POLY_FLATTEN_UTIL_H
