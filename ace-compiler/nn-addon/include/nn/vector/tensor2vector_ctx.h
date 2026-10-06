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
#include "nn/vector/sharding.h"
#include "nn/vector/vector_ctx.h"
#include "nn/vector/vector_enum.h"
#include "nn/vector/vector_utils.h"

namespace nn {

namespace vector {

using PREG_MAP = std::unordered_map<uint32_t, uint32_t>;

// For node tracing
static auto Trace_node = [](std::ostream& os, air::base::NODE_PTR op) {
  op->Print_tree(os, true, 0);
};

static auto Trace_op_sharding = [](std::ostream& os, nn::vector::OP_SHARDING sh,
                                   std::string msg) {
  os << msg << " ";
  sh.Print(os);
};

static auto Trace_array_type = [](std::ostream& os, air::base::TYPE_PTR ty,
                                  std::string msg) {
  ARRAY_TYPE_PTR       aty       = ty->Cast_to_arr();
  std::vector<int64_t> shape     = aty->Shape();
  int                  dim       = aty->Dim();
  TYPE_PTR             elem_type = aty->Elem_type();

  if (elem_type->Is_array())
    os << msg << " SHARDING";
  else
    os << msg;
  os << std::dec << ": shape=[";
  for (int i = 0; i < dim - 1; i++) os << shape[i] << ", ";
  os << shape[dim - 1] << "] size=" << aty->Elem_count();

  if (elem_type->Is_array()) {
    ARRAY_TYPE_PTR       aty_block   = elem_type->Cast_to_arr();
    std::vector<int64_t> shape_block = aty_block->Shape();
    int                  dim_block   = aty_block->Dim();
    os << std::dec << " -> block[";
    for (int i = 0; i < dim_block - 1; i++) os << shape_block[i] << ", ";
    os << shape_block[dim_block - 1]
       << "] block_size=" << aty_block->Elem_count();
  }
  os << std::endl;
};

// For constant float array tracing
static auto Trace_float_array =
    [](std::ostream& os, air::base::CONSTANT_PTR vconst, std::string msg) {
      Print_array_const<float>(os, vconst, msg);
    };

// For constant int array tracing
static auto Trace_int_array =
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
        _config(cfg),
        _cur_func_scope(nullptr) {}

  // declare access API for VECTOR_CTX
  DECLARE_VECTOR_CTX_ACCESS_API(_ctx)

  // declare access API for VECTOR_CONFIG
  DECLARE_VECTOR_CONFIG_ACCESS_API(_config)

  // declare trace API for detail tracing
  DECLARE_TRACE_DETAIL_API(_config, _driver_ctx)

  STMT_LIST Get_insertpoint(int outer) {
    AIR_ASSERT_MSG(_n_blk.size() >= outer + 1,
                   "outer is larger than block stack");
    NODE_PTR  insert_node = *(_n_blk.rbegin() + outer);
    STMT_LIST list(insert_node);
    return list;
  }

  // avoid duplicate computation, can be removed after CSE is implemented.
  NODE_PTR Store_temp_result_to_preg(NODE_PTR input, int outer = 0) {
    AIR_ASSERT_MSG(
        input->Opcode() != air::core::LD && input->Opcode() != air::core::LDP,
        "input should be some expression op");

    return Store_and_load_new_preg(input, outer);
  }

  NODE_PTR Store_and_load_new_preg(NODE_PTR input, int outer = 0) {
    CONTAINER* cntr = this->Container();

    PREG_PTR input_preg = cntr->Parent_func_scope()->New_preg(input->Rtype());
    STMT_PTR st_stmt    = cntr->New_stp(input, input_preg, input->Spos());
    if (outer)
      Get_insertpoint(outer).Append(st_stmt);
    else
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
  const VECTOR_CONFIG& Config() { return _config; }
  FUNC_SCOPE*          Cur_func_scope() { return _cur_func_scope; }

  void Set_cur_func_scope(FUNC_SCOPE* func_scope) {
    _cur_func_scope = func_scope;
  }

private:
  VECTOR_CTX&                    _ctx;
  const air::driver::DRIVER_CTX* _driver_ctx;
  const VECTOR_CONFIG&           _config;
  FUNC_SCOPE*                    _cur_func_scope;
};

}  // namespace vector

}  // namespace nn

#endif  // NN_VECTOR_TENSOR2VECTOR_CTX_H
