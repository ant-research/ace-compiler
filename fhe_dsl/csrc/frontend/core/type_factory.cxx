//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "ace/frontend/core/type_factory.h"
#include "ace/frontend/core/air_context.h"

namespace ace {
namespace frontend {

air::base::TYPE_PTR TYPE_FACTORY::New_tensor_type(const std::vector<int64_t>& shape) {
    return New_array_type(air::base::PRIMITIVE_TYPE::FLOAT_32, shape);
}

air::base::TYPE_PTR TYPE_FACTORY::New_array_type(air::base::PRIMITIVE_TYPE elem_type,
                                                     const std::vector<int64_t>& shape) {
    air::base::GLOB_SCOPE* glob = _ctx->Get_glob();
    if (!glob) {
        return air::base::TYPE_PTR();
    }

    // Get element type
    air::base::TYPE_PTR elem_type_ptr = glob->Prim_type(elem_type);

    // Create array bounds
    air::base::ARB_PTR arb_tail, arb_head;
    for (size_t i = 0; i < shape.size(); ++i) {
        air::base::ARB_PTR arb = glob->New_arb(i + 1, 0, shape[i], 1);
        if (i != 0) {
            arb_tail->Set_next(arb->Id());
        } else {
            arb_head = arb;
        }
        arb_tail = arb;
    }

    // Create array type
    air::base::STR_PTR type_name = glob->Undefined_name();
    return glob->New_arr_type(type_name, elem_type_ptr, arb_head, _ctx->Get_spos());
}

}  // namespace frontend
}  // namespace ace