//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_LOWERING_H
#define NN_VECTOR_LOWERING_H

#include "air/base/container.h"
#include "air/base/st.h"
#include "air/base/transform_ctx.h"
#include "air/core/null_handler.h"
#include "nn/core/default_handler.h"
#include "nn/vector/config.h"
#include "nn/vector/vector_ctx.h"

#include <pybind11/embed.h>
#include <pybind11/pybind11.h>

namespace nn {
namespace vector {

namespace py = pybind11;

using namespace air::base;
using namespace air::core;

GLOB_SCOPE *Dsl_lowering(GLOB_SCOPE *glob, VECTOR_CTX &ctx,
                         const air::driver::DRIVER_CTX *driver_ctx,
                         const VECTOR_CONFIG &cfg);

class DSL_LOWER_CTX : public air::base::TRANSFORM_CTX {
public:
  DSL_LOWER_CTX(air::base::CONTAINER *cont, VECTOR_CTX &ctx,
                const air::driver::DRIVER_CTX *driver_ctx,
                const VECTOR_CONFIG &cfg)
      : air::base::TRANSFORM_CTX(cont), _ctx(ctx), _driver_ctx(driver_ctx),
        _config(cfg), _cur_func_scope(nullptr) {}

  // declare access API for VECTOR_CTX
  DECLARE_VECTOR_CTX_ACCESS_API(_ctx)

  // declare access API for VECTOR_CONFIG
  DECLARE_VECTOR_CONFIG_ACCESS_API(_config)

  // declare trace API for detail tracing
  DECLARE_TRACE_DETAIL_API(_config, _driver_ctx)

  FUNC_SCOPE *Cur_func_scope() { return _cur_func_scope; }

  void Set_cur_func_scope(FUNC_SCOPE *func_scope) {
    _cur_func_scope = func_scope;
  }

private:
  VECTOR_CTX &_ctx;
  const air::driver::DRIVER_CTX *_driver_ctx;
  const VECTOR_CONFIG &_config;
  FUNC_SCOPE *_cur_func_scope;
};

class DSL_LOWER_HANDLER : public nn::core::DEFAULT_HANDLER {
public:
  DSL_LOWER_HANDLER() {}

  template <typename RETV, typename VISITOR>
  RETV Handle_add(VISITOR *visitor, air::base::NODE_PTR node) {
    DSL_LOWER_CTX &ctx = visitor->Context();
    NODE_PTR new_ld0 = visitor->template Visit<RETV>(node->Child(0));
    NODE_PTR new_ld1 = visitor->template Visit<RETV>(node->Child(1));

    py::scoped_interpreter guard{};
    py::module m_instantiator = py::module::import("instantiator");
    py::module m_dsl = py::module::import("air_dsl");
    py::object py_fs = py::cast(ctx.Cur_func_scope());
    py::object py_node = py::cast(node);
    py::object py_new_ld0 = py::cast(new_ld0);
    py::object py_new_ld1 = py::cast(new_ld1);
    auto add = m_instantiator.attr("add");
    return add(py_fs, py_node, py_new_ld0, py_new_ld1).cast<RETV>();
  }
};

} // namespace vector
} // namespace nn

#endif // NN_VECTOR_LOWERING_H