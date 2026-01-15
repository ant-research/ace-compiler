//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <Python.h>
#include <pybind11/embed.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "air/plugin/call_guard.h"

namespace py = pybind11;

namespace air {
namespace plugin {

bool CALL_PYBIND11::Pass_lowering_info(
    std::unordered_map<std::string, std::string> info) {
  try {
    _info = info;
  } catch (const std::exception& e) {
    AIR_ASSERT_MSG(false, "Error: %s", e.what());
    return false;
  }
  return true;
}

NODE_PTR CALL_PYBIND11::Callback(const std::vector<std::any>& cntr,
                                 const std::string&           action,
                                 const std::vector<std::any>& params) {
  py::scoped_interpreter guard{};

  std::string loc = "domain." + _info["phase"];
  AIR_DEBUG("Callback location : %s\n", loc);

  // import Python module: extension & python script
  py::module py_extension = py::module::import(_extension.c_str());
  py::module py_module    = py::module::import(_entry.c_str());

  py::object py_node;

  AIR_DEBUG("Callback Action : %s\n", action);
  if (py::hasattr(py_module, _func.c_str())) {
    py::object py_func = py_module.attr(_func.c_str());
    if (py::isinstance<py::function>(py_func)) {
      py::object py_cntr =
          py::capsule(CALL_HELPER::To_ptr(cntr), [](void* ptr) {});
      py::object py_param =
          py::capsule(CALL_HELPER::To_ptr(params), [](void* ptr) {});
      py_func(_info["phase"], py_cntr, action.c_str(), py_param);
      AIR_DEBUG("Callback() Callback action %s\n", action);
    } else {
      AIR_ASSERT_MSG(false, "Action: %s Can't callable!", action);
    }
  } else {
    AIR_ASSERT_MSG(false, "Action: %s Does not exist!", action);
  }

  NODE_PTR new_node =
      py_node.cast<NODE_PTR>();  // std::any(py_node.cast<NODE_PTR>());
  return new_node;
}

}  // namespace plugin
}  // namespace air