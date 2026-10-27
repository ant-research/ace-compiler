//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <Python.h>

#include "air/plugin/call_guard.h"

namespace air {
namespace plugin {

//! @brief Run Python code snippets
void CALL_PYCAPI::Run(CONST_BYTE_PTR pystr) {
  // Initializes the Python interpreter
  Py_Initialize();
  // Executing Python code
  PyObject* main_module = PyImport_AddModule("__main__");
  PyObject* global_dict = PyModule_GetDict(main_module);
  PyObject* result =
      PyRun_String(pystr, Py_file_input, global_dict, global_dict);
  // AIR_ASSERT(result != nullptr);

  // Freeing Python objects
  Py_DECREF(main_module);
  Py_DECREF(global_dict);
  Py_DECREF(result);

  // Close the Python file and interpreter
  // fclose(file);
  Py_Finalize();
}

//! @brief Run Python file
void CALL_PYCAPI::Run(std::string& pyfile) {
  // Initializes the Python interpreter
  Py_Initialize();

  CONST_BYTE_PTR file_name = (CONST_BYTE_PTR)pyfile.c_str();

  // Open a Python file
  FILE* file = fopen(file_name, "r");
  AIR_ASSERT(file != nullptr);

  // Executing Python code
  PyObject* main_module = PyImport_AddModule("__main__");
  PyObject* global_dict = PyModule_GetDict(main_module);
  PyObject* result =
      PyRun_FileEx(file, file_name, Py_file_input, global_dict, global_dict, 1);
  AIR_ASSERT(result != nullptr);

  // Freeing Python objects
  Py_DECREF(main_module);
  Py_DECREF(global_dict);
  Py_DECREF(result);

  // Close the Python file and interpreter
  // fclose(file);
  Py_Finalize();
}

//! @brief Run torch code snippets
NODE_PTR CALL_PYCAPI::Callback(const std::vector<std::any>& cntr,
                               const std::string&           action,
                               const std::vector<std::any>& params) {
  Py_Initialize();

  PyObject* py_entry  = PyUnicode_FromString(_entry.c_str());
  PyObject* py_module = PyImport_Import(py_entry);
  Py_DECREF(py_entry);
  NODE* node_ptr;

  if (py_module != nullptr) {
    PyObject* py_func = PyObject_GetAttrString(py_module, _func.c_str());

    if (py_func && PyCallable_Check(py_func)) {
      std::string phase    = _info["phase"];
      PyObject*   py_phase = PyUnicode_FromString(phase.c_str());
      PyObject*   py_cntr =
          PyCapsule_New(CALL_HELPER::To_ptr(cntr), "CONTAINER", nullptr);
      PyObject* py_action = PyUnicode_FromString(action.c_str());
      PyObject* py_param =
          PyCapsule_New(CALL_HELPER::To_ptr(params), "VECTOR", nullptr);

      PyObject* py_args =
          PyTuple_Pack(4, py_phase, py_cntr, py_action, py_param);
      PyObject* py_value = PyObject_CallObject(py_func, py_args);

      Py_DECREF(py_args);
      Py_DECREF(py_phase);
      Py_DECREF(py_cntr);
      Py_DECREF(py_action);
      Py_DECREF(py_param);

      if (py_value != nullptr) {
        node_ptr = static_cast<NODE*>(PyCapsule_GetPointer(py_value, "NODE"));
        Py_DECREF(py_value);
      } else {
        PyErr_Print();
      }
    }
    Py_XDECREF(py_func);
    Py_DECREF(py_module);
  } else {
    PyErr_Print();
  }
  Py_Finalize();

  NODE_PTR new_node;
  return new_node;
}

}  // namespace plugin
}  // namespace air