//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <cstdint>
#include <string>
#include <functional>
#include <iostream>

#include "frontend/core/ir_builder.h"
#include "frontend/core/symbol_table.h"

#include "frontend/ops/op_registry.h"
#include "frontend/ops/torch_op_handler.h"

#include "fhe/driver/fhe_cmplr.h"
#include "air/driver/driver.h"

// Domain registration headers
#include "air/core/opcode.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/poly/opcode.h"
#include "fhe/sihe/sihe_opcode.h"
#include "nn/core/opcode.h"
#include "nn/vector/vector_opcode.h"

namespace py = pybind11;

// Use ace::frontend namespace for frontend classes
using namespace ace::frontend;

//=============================================================================
// Python Module Definition
//=============================================================================

PYBIND11_MODULE(frontend, m) {
    m.doc() = "Frontend Module for AIR IR generation";

    // Register all operators on module load
    Register_all_ops();

    // =========================================================================
    // IR_BUILDER Class Binding (Main entry point for AIR IR generation)
    // =========================================================================

    // Use unique_ptr with nodelete holder for non-copyable singleton
    py::class_<IR_BUILDER, std::unique_ptr<IR_BUILDER, py::nodelete>>(m, "Frontend")
        .def_static("get_instance",
            []() -> IR_BUILDER* {
                return &IR_BUILDER::Instance();
            },
            py::return_value_policy::reference,
            "Get the singleton instance of IR_BUILDER")

        // Function-level operations
        .def("begin_function", &IR_BUILDER::Begin_func,
             "Begin building a new AIR function",
             py::arg("name"))

        .def("add_input", &IR_BUILDER::New_param,
             "Add an input parameter to the current function",
             py::arg("name"), py::arg("shape"))

        .def("add_constant",
             [](IR_BUILDER& self, const std::string& name,
                const std::vector<int64_t>& shape,
                const std::vector<float>& data) {
                 self.New_const(name, shape, data.data(), data.size() * sizeof(float));
             },
             "Add a float32 constant parameter to the current function",
             py::arg("name"), py::arg("shape"), py::arg("data"))

        .def("add_constant_int64",
             [](IR_BUILDER& self, const std::string& name,
                const std::vector<int64_t>& shape,
                const std::vector<int64_t>& data) {
                 self.New_const_int64(name, shape,
                                       data.data(), data.size() * sizeof(int64_t));
             },
             "Add an int64 constant parameter to the current function",
             py::arg("name"), py::arg("shape"), py::arg("data"))

        .def("end_function", &IR_BUILDER::End_func,
             "End the current function definition",
             py::arg("output_shape"))

        .def("finalize", &IR_BUILDER::Complete_func,
             "Finalize and complete the AIR function")

        // Operation-level operations
        .def("add_operation", &IR_BUILDER::Add_operation,
             "Add an operation to the current function",
             py::arg("op_name"), py::arg("input_names"),
             py::arg("attrs") = std::map<std::string, py::object>(),
             py::arg("metadata") = std::map<std::string, std::string>(),
             py::arg("output_shape") = std::vector<int64_t>())

        // Output operations
        .def("write_ir", &IR_BUILDER::Write_ir,
             "Write the generated AIR IR to a file",
             py::arg("filename"), py::arg("phase") = "ONNX2AIR")

        .def("print_ir", &IR_BUILDER::Print_ir,
             "Print the generated AIR IR to stdout")

        // State queries
        .def("is_building", &IR_BUILDER::Is_building,
             "Check if AIR function is currently being built")

        .def("get_output", &IR_BUILDER::Get_output, "Get output tensor info")

        .def("get_func_scope",
             [](IR_BUILDER& self) -> uintptr_t {
                 return reinterpret_cast<uintptr_t>(self.Get_func_scope());
             },
             "Get the current function scope as opaque handle")

        .def("get_glob_scope",
             [](IR_BUILDER& self) -> uintptr_t {
                 return reinterpret_cast<uintptr_t>(self.Get_glob());
             },
             "Get the global scope as opaque handle")

        // Level management
        .def("set_level",
             (void (IR_BUILDER::*)(const std::string&)) &IR_BUILDER::Set_level,
             "Set the current level by name",
             py::arg("level_name"))

        .def("get_level", &IR_BUILDER::Get_current_level_name,
             "Get the current level name")

        // Tensor name registry (for Path 1 / direct mode)
        .def("register_tensor_name",
            [](IR_BUILDER& self, int64_t data_ptr, const std::string& name) {
                TORCH_OP_HANDLER::Register_tensor_name(
                    static_cast<uintptr_t>(data_ptr), name);
            },
            "Register a data_ptr to name mapping for direct mode",
            py::arg("data_ptr"), py::arg("name"))

        .def("clear_tensor_names",
            [](IR_BUILDER& self) {
                TORCH_OP_HANDLER::Clear_tensor_names();
            },
            "Clear all data_ptr to name mappings");

    // =========================================================================
    // Utility Functions
    // =========================================================================

    // Debug API: Read and print .B file
    m.def("print_bfile_ir", [](const std::string& filename) -> std::string {
          air::driver::DRIVER driver(false);
          air::base::GLOB_SCOPE* glob = air::base::GLOB_SCOPE::Get();
          driver.Update_glob_scope(glob);
          driver.Read_ir(filename);
          std::ostringstream oss;
          glob->Print_ir(oss);
          return oss.str();
      },
      "Read .B file and print AIR IR to string for debugging",
      py::arg("filename"));
}