//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_CORE_PREG_HANDLER_H
#define NN_VECTOR_CORE_PREG_HANDLER_H

#include "air/base/container.h"
#include "air/base/st.h"
#include "air/core/default_handler.h"
#include "air/core/opcode.h"
#include "nn/vector/tensor2vector_ctx.h"

namespace nn {
namespace vector {

using namespace air::base;

//! @brief Core preg handler for Vector Lowering
class CORE_PREG_HANDLER : public air::core::DEFAULT_HANDLER {
public:
  template <typename RETV, typename VISITOR>
  RETV Handle_ld(VISITOR* visitor, air::base::NODE_PTR node) {
    CONTAINER*     cntr = visitor->Context().Container();
    ADDR_DATUM_PTR data =
        cntr->Parent_func_scope()->Addr_datum(node->Addr_datum_id());
    NODE_PTR new_load = cntr->New_ld(data, node->Spos());
    return RETV(new_load);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_st(VISITOR* visitor, air::base::NODE_PTR node) {
    CONTAINER*     cntr  = visitor->Context().Container();
    NODE_PTR       child = Node(visitor->template Visit<RETV>(node->Child(0)));
    ADDR_DATUM_PTR data =
        cntr->Parent_func_scope()->Addr_datum(node->Addr_datum_id());
    data->Set_type(child->Rtype());
    STMT_PTR new_store = cntr->New_st(child, data, node->Spos());
    return RETV(new_store->Node());
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ldp(VISITOR* visitor, air::base::NODE_PTR node) {
    TENSOR2VECTOR_CTX& ctx  = visitor->Context();
    CONTAINER*         cntr = visitor->Context().Container();
    PREG_PTR orig_preg      = cntr->Parent_func_scope()->Preg(node->Preg_id());
    PREG_MAP t2v_preg_map   = ctx.Get_t2v_preg_map();
    PREG_MAP::iterator iter = t2v_preg_map.find(orig_preg->Id().Value());
    if (iter != t2v_preg_map.end()) {
      PREG_PTR used_preg =
          cntr->Parent_func_scope()->Preg(PREG_ID(iter->second));
      NODE_PTR new_load = cntr->New_ldp(used_preg, node->Spos());
      return RETV(new_load);
    } else {
      NODE_PTR new_load = cntr->New_ldp(orig_preg, node->Spos());
      return RETV(new_load);
    }
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_stp(VISITOR* visitor, air::base::NODE_PTR node) {
    TENSOR2VECTOR_CTX& ctx  = visitor->Context();
    CONTAINER*         cntr = visitor->Context().Container();

    NODE_PTR child = Node(visitor->template Visit<RETV>(node->Child(0)));
    PREG_PTR preg  = cntr->Parent_func_scope()->Preg(node->Preg_id());

    // all op after stride slice insert no need to handle stp and ldp
    // specifically
    if (ctx.Improve_ss_insert() && (ctx.Get_num_op_ca_th() == 4)) {
      STMT_PTR new_store = cntr->New_stp(child, preg, node->Spos());
      return RETV(new_store->Node());
    } else {
      PREG_MAP           t2v_preg_map = ctx.Get_t2v_preg_map();
      PREG_MAP::iterator iter         = t2v_preg_map.find(preg->Id().Value());
      AIR_ASSERT_MSG(iter == t2v_preg_map.end(), "preg has already occurred");

      PREG_PTR new_type_preg =
          cntr->Parent_func_scope()->New_preg(child->Rtype(), preg->Home_sym());

      ctx.Insert_t2v_preg_map(
          {preg->Id().Value(), new_type_preg->Id().Value()});

      STMT_PTR new_store = cntr->New_stp(child, new_type_preg, node->Spos());
      return RETV(new_store->Node());
    }
  }

private:
  template <typename RETV>
  NODE_PTR Node(RETV retv) {
    AIR_ASSERT(retv.Num_node() == 1);
    return retv.Node();
  }

  NODE_PTR Node(NODE_PTR node) { return node; }
};

}  // namespace vector
}  // namespace nn

#endif  // NN_VECTOR_CORE_PREG_HANDLER_H
