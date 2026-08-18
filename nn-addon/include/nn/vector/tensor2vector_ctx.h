//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_TENSOR2VECTOR_CTX_H
#define NN_VECTOR_TENSOR2VECTOR_CTX_H

#include "air/base/transform_ctx.h"
#include "air/core/opcode.h"
#include "nn/vector/config.h"
#include "nn/vector/vector_ctx.h"
#include "nn/vector/vector_enum.h"
#include "nn/vector/vector_utils.h"

namespace nn {

namespace vector {

using PREG_MAP = std::unordered_map<uint32_t, uint32_t>;

// For node tracing
inline auto Trace_node = [](std::ostream& os, air::base::NODE_PTR op) {
  op->Print_tree(os, true, 0);
};

// For constant float array tracing
inline auto Trace_float_array =
    [](std::ostream& os, air::base::CONSTANT_PTR vconst, std::string msg) {
      Print_array_const<float>(os, vconst, msg);
    };

// For constant int array tracing
inline auto Trace_int_array =
    [](std::ostream& os, air::base::CONSTANT_PTR vconst, std::string msg) {
      Print_array_const<int>(os, vconst, msg);
    };

class TENSOR2VECTOR_CTX : public air::base::TRANSFORM_CTX {
public:
  TENSOR2VECTOR_CTX(air::base::CONTAINER* cont, VECTOR_CTX& ctx,
                    const air::driver::DRIVER_CTX* driver_ctx,
                    const VECTOR_CONFIG&           cfg)
      : air::base::TRANSFORM_CTX(cont),
        _ctx(ctx),
        _driver_ctx(driver_ctx),
        _config(cfg) {}

  // declare access API for VECTOR_CTX
  DECLARE_VECTOR_CTX_ACCESS_API(_ctx)

  // declare access API for VECTOR_CONFIG
  DECLARE_VECTOR_CONFIG_ACCESS_API(_config)

  // declare trace API for detail tracing
  DECLARE_TRACE_DETAIL_API(_config, _driver_ctx)

  // avoid duplicate computation, can be removed after CSE is implemented.
  NODE_PTR Store_temp_result_to_preg(NODE_PTR input) {
    AIR_ASSERT_MSG(
        input->Opcode() != air::core::LD && input->Opcode() != air::core::LDP,
        "input should be some expression op");

    return Store_and_load_new_preg(input);
  }

  NODE_PTR Store_and_load_new_preg(NODE_PTR input) {
    CONTAINER* cntr = this->Container();

    PREG_PTR input_preg = cntr->Parent_func_scope()->New_preg(input->Rtype());
    STMT_PTR st_stmt    = cntr->New_stp(input, input_preg, input->Spos());
    this->Prepend(st_stmt);

    NODE_PTR new_input = cntr->New_ldp(input_preg, input->Spos());
    return new_input;
  }

  bool Is_last_op() {
    NODE_PTR parent_node = this->Parent(1);
    bool     last_op     = false;
    // till now, result stored to output variable should be the last op
    if (parent_node->Opcode() == air::core::ST &&
        (strncmp(parent_node->Addr_datum()->Name()->Char_str(), "output", 6) ==
         0)) {
      last_op = true;
    }
    return last_op;
  }

  int Get_slot() const {
    // slot option value has high priority
    // TODO: need to align slot type later.
    if (_config.Max_slots() != 0) {
      return _config.Max_slots();
    } else {
      return _ctx.Slot();
    }
  }

private:
  VECTOR_CTX&                    _ctx;
  const air::driver::DRIVER_CTX* _driver_ctx;
  const VECTOR_CONFIG&           _config;
};

}  // namespace vector

}  // namespace nn

#endif  // NN_VECTOR_TENSOR2VECTOR_CTX_H
