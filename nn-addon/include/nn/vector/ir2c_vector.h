//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_IR2C_VECTOR_H
#define NN_VECTOR_IR2C_VECTOR_H

#include "air/base/container.h"
#include "air/core/ir2c_ctx.h"
#include "nn/vector/invalid_handler.h"

namespace nn {
namespace vector {

//! @brief Special IR2C handler for VECTOR operators
class IR2C_VECTOR : public INVALID_HANDLER {
public:
  //! @brief Emit constant id for operator SLICE
  template <typename RETV, typename VISITOR>
  void Handle_slice(VISITOR* visitor, air::base::NODE_PTR node) {
    air::core::IR2C_CTX& ctx = visitor->Context();
    ctx << "Slice(";
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(1));
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(2));
    ctx << ")";
  }

  //! @brief the follow functions are used to emit code for runtime validation.
  //! _rtv means "Runtime Validation", which takes current input of the
  //!      node and validate if lowering is correctly
  //! _ref means "Reference Validation", which takes original input of the
  //!      program and calculate totally in another way as a reference
  template <typename RETV, typename VISITOR>
  void Handle_add_ref(VISITOR* visitor, air::base::NODE_PTR node) {
    Generate_assign_lhs<RETV>(visitor);
    Generate_add_call<RETV>(visitor, node, "Add_ref");
  }

  template <typename RETV, typename VISITOR>
  void Handle_average_pool_rtv(VISITOR* visitor, air::base::NODE_PTR node) {
    Generate_pool_call<RETV>(visitor, node, "Average_pool_rtv");
  }

  template <typename RETV, typename VISITOR>
  void Handle_average_pool_ref(VISITOR* visitor, air::base::NODE_PTR node) {
    Generate_assign_lhs<RETV>(visitor);
    Generate_pool_call<RETV>(visitor, node, "Average_pool_ref");
  }

  template <typename RETV, typename VISITOR>
  void Handle_conv_rtv(VISITOR* visitor, air::base::NODE_PTR node) {
    Generate_conv_call<RETV>(visitor, node, "Conv_rtv");
  }

  template <typename RETV, typename VISITOR>
  void Handle_conv_ref(VISITOR* visitor, air::base::NODE_PTR node) {
    Generate_assign_lhs<RETV>(visitor);
    Generate_conv_call<RETV>(visitor, node, "Conv_ref");
  }

  template <typename RETV, typename VISITOR>
  void Handle_flatten_ref(VISITOR* visitor, air::base::NODE_PTR node) {
    Generate_assign_lhs<RETV>(visitor);
    visitor->Context() << "/* flatten */ ";
    visitor->template Visit<RETV>(node->Child(0));
  }

  template <typename RETV, typename VISITOR>
  void Handle_gemm_rtv(VISITOR* visitor, air::base::NODE_PTR node) {
    Generate_gemm_call<RETV>(visitor, node, "Gemm_rtv");
  }

  template <typename RETV, typename VISITOR>
  void Handle_gemm_ref(VISITOR* visitor, air::base::NODE_PTR node) {
    Generate_assign_lhs<RETV>(visitor);
    Generate_gemm_call<RETV>(visitor, node, "Gemm_ref");
  }

  template <typename RETV, typename VISITOR>
  void Handle_global_average_pool_rtv(VISITOR*            visitor,
                                      air::base::NODE_PTR node) {
    Generate_global_average_pool_call<RETV>(visitor, node,
                                            "Global_average_pool_rtv");
  }

  template <typename RETV, typename VISITOR>
  void Handle_global_average_pool_ref(VISITOR*            visitor,
                                      air::base::NODE_PTR node) {
    Generate_assign_lhs<RETV>(visitor);
    Generate_global_average_pool_call<RETV>(visitor, node,
                                            "Global_average_pool_ref");
  }

  template <typename RETV, typename VISITOR>
  void Handle_max_pool_rtv(VISITOR* visitor, air::base::NODE_PTR node) {
    Generate_pool_call<RETV>(visitor, node, "Max_pool_rtv");
  }

  template <typename RETV, typename VISITOR>
  void Handle_max_pool_ref(VISITOR* visitor, air::base::NODE_PTR node) {
    Generate_assign_lhs<RETV>(visitor);
    Generate_pool_call<RETV>(visitor, node, "Max_pool_ref");
  }

  template <typename RETV, typename VISITOR>
  void Handle_relu_rtv(VISITOR* visitor, air::base::NODE_PTR node) {
    Generate_relu_call<RETV>(visitor, node, "Relu_rtv");
  }

  template <typename RETV, typename VISITOR>
  void Handle_relu_ref(VISITOR* visitor, air::base::NODE_PTR node) {
    Generate_assign_lhs<RETV>(visitor);
    Generate_relu_call<RETV>(visitor, node, "Relu_ref");
  }

  template <typename RETV, typename VISITOR>
  void Handle_reshape_ref(VISITOR* visitor, air::base::NODE_PTR node) {
    Generate_assign_lhs<RETV>(visitor);
    visitor->Context() << "/* reshape */ ";
    visitor->template Visit<RETV>(node->Child(0));
  }

private:
  template <typename RETV, typename VISITOR>
  void Generate_add_call(VISITOR* visitor, air::base::NODE_PTR node,
                         const char* fname) {
    uint32_t             elem_count = 0;
    air::core::IR2C_CTX& ctx        = visitor->Context();
    ctx << fname << "(";
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(1));
    const int64_t* x_shape = node->Attr<int64_t>("x_shape", &elem_count);
    AIR_ASSERT(x_shape != nullptr && elem_count == 4);
    uint64_t len = x_shape[0] * x_shape[1] * x_shape[2] * x_shape[3];
    ctx << ", " << len << ")";
  }

  template <typename RETV, typename VISITOR>
  void Generate_assign_lhs(VISITOR* visitor) {
    air::core::IR2C_CTX& ctx    = visitor->Context();
    air::base::NODE_PTR  parent = ctx.Parent(1);
    ctx.template Emit_st_var<RETV, VISITOR>(visitor, parent);
    ctx << " = ";
  }

  template <typename RETV, typename VISITOR>
  void Generate_conv_call(VISITOR* visitor, air::base::NODE_PTR node,
                          const char* fname) {
    uint32_t             elem_count = 0;
    air::core::IR2C_CTX& ctx        = visitor->Context();
    ctx << fname << "(";
    // image data and shape
    visitor->template Visit<RETV>(node->Child(0));
    const int64_t* x_shape = node->Attr<int64_t>("x_shape", &elem_count);
    AIR_ASSERT(x_shape != nullptr && elem_count == 4);
    ctx << ", /* n */ " << x_shape[0] << ", /* c */ " << x_shape[1];
    ctx << ", /* h */ " << x_shape[2] << ", /* w */ " << x_shape[3];
    ctx << ", ";
    // weight data and shape
    visitor->template Visit<RETV>(node->Child(1));
    const int64_t* w_shape = node->Attr<int64_t>("w_shape", &elem_count);
    AIR_ASSERT(w_shape != nullptr && elem_count == 4);
    ctx << ", /* kn */ " << w_shape[0] << ", /* kc */ " << w_shape[1];
    ctx << ", /* kh */ " << w_shape[2] << ", /* kw */ " << w_shape[3];
    ctx << ", ";
    // bias data ans shape
    visitor->template Visit<RETV>(node->Child(2));
    const int64_t* b_shape = node->Attr<int64_t>("b_shape", &elem_count);
    AIR_ASSERT(b_shape != nullptr && elem_count == 1);
    ctx << ", /* b */ " << b_shape[0];
    // strides
    const int* strides = node->Attr<int>("strides", &elem_count);
    AIR_ASSERT(strides != nullptr && elem_count == 2);
    ctx << ", /* sh */ " << strides[0] << ", /* sw */ " << strides[1];
    // padding
    const int* pads = node->Attr<int>("pads", &elem_count);
    AIR_ASSERT(pads != nullptr && elem_count == 4);
    ctx << ", /* pn */ " << pads[0] << ", /* pc */ " << pads[1];
    ctx << ", /* ph */ " << pads[2] << ", /* pw */ " << pads[3];
    ctx << ")";
  }

  template <typename RETV, typename VISITOR>
  void Generate_gemm_call(VISITOR* visitor, air::base::NODE_PTR node,
                          const char* fname) {
    uint32_t             elem_count = 0;
    air::core::IR2C_CTX& ctx        = visitor->Context();
    ctx << fname << "(";
    // image data and shape
    visitor->template Visit<RETV>(node->Child(0));
    const int64_t* x_shape = node->Attr<int64_t>("x_shape", &elem_count);
    AIR_ASSERT(x_shape != nullptr && elem_count == 2);
    ctx << ", /* h */ " << x_shape[0] << ", /* w */ " << x_shape[1];
    ctx << ", ";
    // weight and shape
    visitor->template Visit<RETV>(node->Child(1));
    const int64_t* w_shape = node->Attr<int64_t>("w_shape", &elem_count);
    AIR_ASSERT(w_shape != nullptr && elem_count == 2);
    ctx << ", /* h */ " << w_shape[0] << ", /* w */ " << w_shape[1];
    ctx << ", ";
    // bias and shape
    visitor->template Visit<RETV>(node->Child(2));
    const int64_t* b_shape = node->Attr<int64_t>("b_shape", &elem_count);
    AIR_ASSERT(b_shape != nullptr && elem_count == 1);
    ctx << ", /* w */ " << w_shape[0];
    ctx << ")";
  }

  template <typename RETV, typename VISITOR>
  void Generate_global_average_pool_call(VISITOR*            visitor,
                                         air::base::NODE_PTR node,
                                         const char*         fname) {
    uint32_t             elem_count = 0;
    air::core::IR2C_CTX& ctx        = visitor->Context();
    ctx << fname << "(";
    // input data and shape
    visitor->template Visit<RETV>(node->Child(0));
    const int64_t* x_shape = node->Attr<int64_t>("x_shape", &elem_count);
    AIR_ASSERT(x_shape != nullptr && elem_count == 4);
    ctx << ", /* n */ " << x_shape[0] << ", /* c */ " << x_shape[1];
    ctx << ", /* h */ " << x_shape[2] << ", /* w */ " << x_shape[3];
    ctx << ")";
  }

  template <typename RETV, typename VISITOR>
  void Generate_pool_call(VISITOR* visitor, air::base::NODE_PTR node,
                          const char* fname) {
    uint32_t             elem_count = 0;
    air::core::IR2C_CTX& ctx        = visitor->Context();
    ctx << fname << "(";
    // image data and shape
    visitor->template Visit<RETV>(node->Child(0));
    const int64_t* x_shape = node->Attr<int64_t>("x_shape", &elem_count);
    AIR_ASSERT(x_shape != nullptr && elem_count == 4);
    ctx << ", /* n */ " << x_shape[0] << ", /* c */ " << x_shape[1];
    ctx << ", /* h */ " << x_shape[2] << ", /* w */ " << x_shape[3];
    // kernel
    const int* kernel = node->Attr<int>("kernel_shape", &elem_count);
    AIR_ASSERT(kernel != nullptr && elem_count == 2);
    ctx << ", /* kh */ " << kernel[0] << ", /* kw */ " << kernel[1];
    // stride
    const int* strides = node->Attr<int>("strides", &elem_count);
    AIR_ASSERT(strides != nullptr && elem_count == 2);
    ctx << ", /* sh */ " << strides[0] << ", /* sw */ " << strides[1];
    // padding
    const int* pads = node->Attr<int>("pads", &elem_count);
    AIR_ASSERT(pads != nullptr && elem_count == 4);
    ctx << ", /* pn */ " << x_shape[0] << ", /* pc */ " << x_shape[1];
    ctx << ", /* ph */ " << x_shape[2] << ", /* pw */ " << x_shape[3];
    ctx << ")";
  }

  template <typename RETV, typename VISITOR>
  void Generate_relu_call(VISITOR* visitor, air::base::NODE_PTR node,
                          const char* fname) {
    uint32_t             elem_count = 0;
    air::core::IR2C_CTX& ctx        = visitor->Context();
    ctx << fname << "(";
    visitor->template Visit<RETV>(node->Child(0));
    const int64_t* x_shape = node->Attr<int64_t>("x_shape", &elem_count);
    AIR_ASSERT(x_shape != nullptr && (elem_count == 2 || elem_count == 4));
    uint64_t len = 1;
    for (uint32_t i = 0; i < elem_count; ++i) {
      len *= x_shape[i];
    }
    ctx << ", " << len << ")";
  }
};  // IR2C_VECTOR

}  // namespace vector

}  // namespace nn

#endif  // NN_VECTOR_IR2C_VECTOR_H
