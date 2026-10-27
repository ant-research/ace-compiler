//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/plugin/pyrun.h"

#include <Python.h>

#include <iostream>

namespace air {
namespace plugin {

PYRUN::PYRUN() {
  // Initializes the Python interpreter
  Py_Initialize();
}

PYRUN::~PYRUN() {
  // Close the Python file and interpreter
  // fclose(file);
  Py_Finalize();
}

//! @brief Run Python code snippets
void PYRUN::Run(CONST_BYTE_PTR pystr) {
  // Executing Python code
  PyObject* main_module = PyImport_AddModule("__main__");
  PyObject* global_dict = PyModule_GetDict(main_module);
  PyObject* result =
      PyRun_String(pystr, Py_file_input, global_dict, global_dict);
  AIR_ASSERT(result != nullptr);

  // Freeing Python objects
  Py_DECREF(main_module);
  Py_DECREF(global_dict);
  Py_DECREF(result);
}

//! @brief Run Python file
void PYRUN::Run(std::string& pyfile) {
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
}

}  // namespace plugin
}  // namespace air