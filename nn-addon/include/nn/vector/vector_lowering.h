//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_LOWERING_H
#define NN_VECTOR_LOWERING_H

#include <pybind11/embed.h>
#include <pybind11/pybind11.h>

#include "air/base/container.h"
#include "air/base/st.h"
#include "air/base/transform_ctx.h"
#include "air/core/null_handler.h"
#include "nn/core/default_handler.h"
#include "nn/vector/config.h"
#include "nn/vector/vector_ctx.h"

namespace nn {
namespace vector {

namespace py = pybind11;

using namespace air::base;
using namespace air::core;

GLOB_SCOPE* Dsl_lowering(GLOB_SCOPE* glob, VECTOR_CTX& ctx,
                         const air::driver::DRIVER_CTX* driver_ctx,
                         const VECTOR_CONFIG&           cfg);

class DSL_LOWER_CTX : public air::base::TRANSFORM_CTX {
public:
  DSL_LOWER_CTX(air::base::CONTAINER* cont, VECTOR_CTX& ctx,
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

  FUNC_SCOPE* Cur_func_scope() { return _cur_func_scope; }

  void Set_cur_func_scope(FUNC_SCOPE* func_scope) {
    _cur_func_scope = func_scope;
  }

private:
  VECTOR_CTX&                    _ctx;
  const air::driver::DRIVER_CTX* _driver_ctx;
  const VECTOR_CONFIG&           _config;
  FUNC_SCOPE*                    _cur_func_scope;
};

class DSL_LOWER_HANDLER : public nn::core::DEFAULT_HANDLER {
public:
  DSL_LOWER_HANDLER() {}

  template <typename RETV, typename VISITOR>
  RETV Handle_add(VISITOR* visitor, air::base::NODE_PTR node) {
    DSL_LOWER_CTX& ctx     = visitor->Context();
    NODE_PTR       new_ld0 = visitor->template Visit<RETV>(node->Child(0));
    NODE_PTR       new_ld1 = visitor->template Visit<RETV>(node->Child(1));
    ADDR_DATUM_PTR new_var =
        ctx.Cur_func_scope()->New_var(node->Rtype(), "dsl_out", node->Spos());

    py::scoped_interpreter guard{};
    py::module             m_instantiator = py::module::import("instantiator");
    py::module             m_dsl          = py::module::import("air_dsl");
    py::object             py_fs          = py::cast(ctx.Cur_func_scope());
    py::object             py_ctx         = py::cast((&ctx));
    py::object             py_node        = py::cast(node);
    py::object             py_new_ld0     = py::cast(new_ld0);
    py::object             py_new_ld1     = py::cast(new_ld1);
    pybind11::detail::str_attr_accessor add        = m_instantiator.attr("add");
    py::object                          py_new_var = py::cast(new_var);

    add(py_fs, py_ctx, py_node, py_new_ld0, py_new_ld1, py_new_var);

    NODE_PTR new_load =
        ctx.Cur_func_scope()->Container().New_ld(new_var, node->Spos());
    return RETV(new_load);
  }

  // this function is used to test the `PyContext`
  template <typename RETV, typename VISITOR>
  RETV Handle_conv(VISITOR* visitor, air::base::NODE_PTR node) {
    DSL_LOWER_CTX& ctx     = visitor->Context();
    NODE_PTR       new_ld0 = visitor->template Visit<RETV>(node->Child(0));
    NODE_PTR       new_ld1 = visitor->template Visit<RETV>(node->Child(1));
    NODE_PTR       new_ld2 = visitor->template Visit<RETV>(node->Child(2));
    ADDR_DATUM_PTR new_var =
        ctx.Cur_func_scope()->New_var(node->Rtype(), "dsl_out", node->Spos());
    py::scoped_interpreter guard{};
    py::module             m_instantiator = py::module::import("instantiator");
    py::module             m_dsl          = py::module::import("air_dsl");
    py::object             py_fs          = py::cast(ctx.Cur_func_scope());
    py::object             py_ctx         = py::cast((&ctx));
    py::object             py_node        = py::cast(node);
    py::object             py_new_ld0     = py::cast(new_ld0);
    py::object             py_new_ld1     = py::cast(new_ld1);
    py::object             py_new_ld2     = py::cast(new_ld2);
    py::object             py_new_var     = py::cast(new_var);
    std::vector<int>       ra             = {-4, -3, -2, -1, 0, 1, 2, 3, 4};
#if 0
    std::vector<int64_t> input_grid_shape = (9);
    TYPE_PTR             ra_type = New_array_type(gscope, "ra_int", 1, s32_type,
                                                  input_grid_shape, node->Spos());
    CONSTANT_PTR         ra_const = gscope->New_const(
        CONSTANT_KIND::ARRAY, ra_type, (void*)(ra.data()), 4 * ra.size());
#endif
    auto conv = m_instantiator.attr("conv");

    conv(py_fs, py_ctx, py_node, py_new_ld0, py_new_ld1, py_new_ld2, py_new_var,
         ra);

    NODE_PTR new_load =
        ctx.Cur_func_scope()->Container().New_ld(new_var, node->Spos());
    return RETV(new_load);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_gemm(VISITOR* visitor, air::base::NODE_PTR node) {
    DSL_LOWER_CTX& ctx     = visitor->Context();
    NODE_PTR       new_ld0 = visitor->template Visit<RETV>(node->Child(0));
    NODE_PTR       new_ld1 = visitor->template Visit<RETV>(node->Child(1));
    NODE_PTR       new_ld2 = visitor->template Visit<RETV>(node->Child(2));
    ADDR_DATUM_PTR new_var =
        ctx.Cur_func_scope()->New_var(node->Rtype(), "dsl_out", node->Spos());

    py::scoped_interpreter guard{};
    py::module             m_instantiator = py::module::import("instantiator");
    py::module             m_dsl          = py::module::import("air_dsl");
    py::object             py_fs          = py::cast(ctx.Cur_func_scope());
    py::object             py_ctx         = py::cast((&ctx));
    py::object             py_node        = py::cast(node);
    py::object             py_new_ld0     = py::cast(new_ld0);
    py::object             py_new_ld1     = py::cast(new_ld1);
    py::object             py_new_ld2     = py::cast(new_ld2);
    py::object             py_new_var     = py::cast(new_var);
    auto                   gemm           = m_instantiator.attr("gemm");
    gemm(py_fs, py_ctx, py_node, py_new_ld0, py_new_ld1, py_new_ld2,
         py_new_var);

    NODE_PTR new_load =
        ctx.Cur_func_scope()->Container().New_ld(new_var, node->Spos());
    return RETV(new_load);
  }
};

}  // namespace vector
}  // namespace nn

#endif  // NN_VECTOR_LOWERING_H
