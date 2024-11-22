//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_CORE_HANDLER_H
#define NN_VECTOR_CORE_HANDLER_H

#include "air/base/container.h"
#include "air/base/st.h"
#include "air/core/default_handler.h"
#include "air/core/opcode.h"

namespace nn {
namespace vector {

using namespace air::base;

//! @brief Core handler for Vector Lowering
class CORE_HANDLER : public air::core::DEFAULT_HANDLER {
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

#endif  // NN_VECTOR_CORE_HANDLER_H
