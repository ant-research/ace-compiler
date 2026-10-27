//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn/vector/tensor2vector_py_airgen.h"

#include <pybind11/embed.h>
#include <pybind11/pybind11.h>

#include <cmath>

#include "air/core/opcode.h"
#include "nn/core/opcode.h"
#include "nn/vector/vector_opcode.h"

namespace nn {
namespace vector {

namespace py = pybind11;

#define UPDATE_BODY(body)                                               \
  for (STMT_PTR stmt = body->Begin_stmt(); stmt != body->End_stmt();) { \
    STMT_PTR next_stmt = stmt->Next();                                  \
    _ctx.Prepend(stmt);                                                 \
    stmt = next_stmt;                                                   \
  }

NODE_PTR TENSOR2VECTOR_PY_IMPL::New_py_add(NODE_PTR node, NODE_PTR new_ld0,
                                           NODE_PTR new_ld1, const SPOS& spos) {
  ADDR_DATUM_PTR new_var =
      _ctx.Cur_func_scope()->New_var(node->Rtype(), "dsl_out", spos);

  py::scoped_interpreter guard{};
  py::module             m_instantiator   = py::module::import("instantiator");
  py::module             m_dsl            = py::module::import("air_dsl");
  py::object             py_fs            = py::cast(_ctx.Cur_func_scope());
  py::object             py_node          = py::cast(node);
  py::object             py_new_ld0       = py::cast(new_ld0);
  py::object             py_new_ld1       = py::cast(new_ld1);
  pybind11::detail::str_attr_accessor add = m_instantiator.attr("add");
  py::object                          py_new_var = py::cast(new_var);
  NODE_PTR                            body =
      _ctx.Cur_func_scope()->Container().New_stmt_block(node->Spos());
  py::object py_body = py::cast(&body);
  add(py_fs, py_body, py_node, py_new_ld0, py_new_ld1, py_new_var);

  UPDATE_BODY(body);

  NODE_PTR new_load =
      _ctx.Cur_func_scope()->Container().New_ld(new_var, node->Spos());
  return new_load;
}

NODE_PTR TENSOR2VECTOR_PY_IMPL::New_py_conv(
    NODE_PTR node, NODE_PTR new_input1d, NODE_PTR new_weight, NODE_PTR new_bias,
    const SPOS& spos, std::vector<int> ra, int channel_in, int channel_out,
    int output_height, int output_width, int kernel_hw, int stride) {
  ADDR_DATUM_PTR new_var =
      _ctx.Cur_func_scope()->New_var(node->Rtype(), "dsl_out", spos);
  py::scoped_interpreter guard{};
  py::module             m_instantiator = py::module::import("instantiator");
  py::module             m_dsl          = py::module::import("air_dsl");
  py::object             py_fs          = py::cast(_ctx.Cur_func_scope());
  py::object             py_node        = py::cast(node);
  py::object             py_new_ld0     = py::cast(new_input1d);
  py::object             py_new_ld1     = py::cast(new_weight);
  py::object             py_new_ld2     = py::cast(new_bias);
  py::object             py_new_var     = py::cast(new_var);
  std::vector<int64_t>   ra_shape(1, ra.size());

  GLOB_SCOPE* gscope = _ctx.Container()->Glob_scope();
  for (int i = 0; i < ra.size(); i++) ra[i] *= stride;
  CONST_TYPE_PTR s32_type = gscope->Prim_type(PRIMITIVE_TYPE::INT_S32);
  TYPE_PTR       ra_type =
      New_array_type(gscope, "ra_int", 2, s32_type, ra_shape, spos);
  CONSTANT_PTR ra_const       = gscope->New_const(CONSTANT_KIND::ARRAY, ra_type,
                                                  (void*)(ra.data()), 4 * ra.size());
  py::object   py_ra          = py::cast(ra_const);
  py::object   py_channel_in  = py::cast(channel_in);
  py::object   py_channel_out = py::cast(channel_out);
  py::object   py_output_height = py::cast(output_height);
  py::object   py_output_width  = py::cast(output_width);
  int64_t      kernel_size      = kernel_hw;
  py::object   py_kernel_size   = py::cast(kernel_size);
  auto         conv             = m_instantiator.attr("conv");
  NODE_PTR     body =
      _ctx.Cur_func_scope()->Container().New_stmt_block(node->Spos());
  py::object py_body = py::cast(&body);
  conv(py_fs, py_body, py_node, py_new_ld0, py_new_ld1, py_new_ld2, py_new_var,
       py_ra, py_channel_in, py_channel_out, py_output_height, py_output_width,
       py_kernel_size);

  UPDATE_BODY(body);

  NODE_PTR new_load =
      _ctx.Cur_func_scope()->Container().New_ld(new_var, node->Spos());
  return new_load;
}

NODE_PTR TENSOR2VECTOR_PY_IMPL::New_py_gemm(NODE_PTR node, NODE_PTR new_ld0,
                                            NODE_PTR    new_weight,
                                            NODE_PTR    new_bias,
                                            const SPOS& spos) {
  ADDR_DATUM_PTR new_var =
      _ctx.Cur_func_scope()->New_var(node->Rtype(), "dsl_out", spos);

  py::scoped_interpreter guard{};
  py::module             m_instantiator = py::module::import("instantiator");
  py::module             m_dsl          = py::module::import("air_dsl");
  py::object             py_fs          = py::cast(_ctx.Cur_func_scope());
  py::object             py_node        = py::cast(node);
  py::object             py_new_ld0     = py::cast(new_ld0);
  py::object             py_new_weight  = py::cast(new_weight);
  py::object             py_new_bias    = py::cast(new_bias);
  py::object             py_new_var     = py::cast(new_var);
  auto                   gemm           = m_instantiator.attr("gemm");
  NODE_PTR               body =
      _ctx.Cur_func_scope()->Container().New_stmt_block(node->Spos());
  py::object py_body = py::cast(&body);
  gemm(py_fs, py_body, py_node, py_new_ld0, py_new_weight, py_new_bias,
       py_new_var);

  UPDATE_BODY(body);
  NODE_PTR new_load =
      _ctx.Cur_func_scope()->Container().New_ld(new_var, node->Spos());
  return new_load;
}
}  // namespace vector
}  // namespace nn
