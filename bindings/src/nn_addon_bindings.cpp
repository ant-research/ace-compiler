//
// nn_addon_bindings.cpp - pybind11 bindings for NN domain skip lowering
//
// This module provides Python access to:
// - The REAL nn::vector::SKIP_LOWERING_REGISTRY from nn-addon
// - Opcode constants for nn::core and nn::vector domains
//
// IMPORTANT: This binds to the REAL nn-addon registry, NOT a mock.

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <string>
#include <vector>

// Include the REAL skip lowering registry from nn-addon
#include "nn/vector/skip_lowering.h"

namespace py = pybind11;

// ═══════════════════════════════════════════════════════════════════════════════
// Python module definition
// ═══════════════════════════════════════════════════════════════════════════════

PYBIND11_MODULE(nn_addon, m) {
    m.doc() = "NN Addon - Python bindings for ACE-compiler neural network operations";
    
    // Opcodes as module-level constants
    auto core = m.def_submodule("core", "nn::core opcodes");
    core.attr("ADD") = "nn::core::ADD";
    core.attr("SUB") = "nn::core::SUB";
    core.attr("MUL") = "nn::core::MUL";
    core.attr("CONV") = "nn::core::CONV";
    core.attr("GEMM") = "nn::core::GEMM";
    core.attr("RELU") = "nn::core::RELU";
    core.attr("AVERAGE_POOL") = "nn::core::AVERAGE_POOL";
    core.attr("MAX_POOL") = "nn::core::MAX_POOL";
    core.attr("GLOBAL_AVERAGE_POOL") = "nn::core::GLOBAL_AVERAGE_POOL";
    core.attr("FLATTEN") = "nn::core::FLATTEN";
    core.attr("SOFTMAX") = "nn::core::SOFTMAX";
    core.attr("MATMUL") = "nn::core::MATMUL";
    core.attr("RESHAPE") = "nn::core::RESHAPE";
    core.attr("TRANSPOSE") = "nn::core::TRANSPOSE";
    core.attr("SQRT") = "nn::core::SQRT";
    core.attr("DIVIDE") = "nn::core::DIVIDE";
    core.attr("RMSNORM") = "nn::core::RMSNORM";
    core.attr("STRIDED_SLICE") = "nn::core::STRIDED_SLICE";
    core.attr("ROPE_ROTARY") = "nn::core::ROPE_ROTARY";
    core.attr("RESHAPE_KV") = "nn::core::RESHAPE_KV";
    core.attr("REPEAT_KV") = "nn::core::REPEAT_KV";
    
    auto vector = m.def_submodule("vector", "nn::vector opcodes");
    vector.attr("ADD") = "nn::vector::ADD";
    vector.attr("MUL") = "nn::vector::MUL";
    vector.attr("ROLL") = "nn::vector::ROLL";
    vector.attr("SLICE") = "nn::vector::SLICE";
    vector.attr("PAD") = "nn::vector::PAD";
    vector.attr("RESHAPE") = "nn::vector::RESHAPE";
    vector.attr("READ") = "nn::vector::READ";
    // Runtime validation
    vector.attr("CONV_RTV") = "nn::vector::CONV_RTV";
    vector.attr("GEMM_RTV") = "nn::vector::GEMM_RTV";
    vector.attr("RELU_RTV") = "nn::vector::RELU_RTV";
    
    m.attr("__version__") = "0.1.0";
    m.attr("__is_mock__") = false;
    m.attr("__ace_enabled__") = true;

    // ═══════════════════════════════════════════════════════════════════════════════
    // REAL Skip Lowering Registry from nn-addon
    // ═══════════════════════════════════════════════════════════════════════════════
    
    // Expose the REAL nn::vector::SKIP_LOWERING_REGISTRY
    py::class_<nn::vector::SKIP_LOWERING_REGISTRY>(m, "SkipLoweringRegistry")
        .def_static("instance", &nn::vector::SKIP_LOWERING_REGISTRY::Instance,
                    py::return_value_policy::reference,
                    "Get the singleton instance (REAL nn-addon registry)")
        .def("set_skip_ops", &nn::vector::SKIP_LOWERING_REGISTRY::Set_skip_ops,
             py::arg("ops"),
             "Set the list of ops to skip (format: 'domain::op_name')")
        .def("add_skip_op", &nn::vector::SKIP_LOWERING_REGISTRY::Add_skip_op,
             py::arg("op"),
             "Add a single op to skip")
        .def("clear_skip_ops", &nn::vector::SKIP_LOWERING_REGISTRY::Clear_skip_ops,
             "Clear all skip ops")
        .def("should_skip", 
             py::overload_cast<const std::string&, const std::string&>(&nn::vector::SKIP_LOWERING_REGISTRY::Should_skip, py::const_),
             py::arg("domain"), py::arg("op_name"),
             "Check if an op should be skipped")
        .def("get_skip_ops", [](const nn::vector::SKIP_LOWERING_REGISTRY& self) {
            const auto& ops = self.Get_skip_ops();
            return std::vector<std::string>(ops.begin(), ops.end());
        }, "Get all ops to skip")
        .def("skip_count", &nn::vector::SKIP_LOWERING_REGISTRY::Skip_count,
             "Get the count of ops to skip")
        .def("has_skip_ops", &nn::vector::SKIP_LOWERING_REGISTRY::Has_skip_ops,
             "Check if any ops are registered to skip");
    
    // Module-level convenience functions that call the REAL registry
    m.def("set_skip_ops", &nn::vector::Set_skip_lowering_ops,
          py::arg("ops"),
          "Set ops that C++ passes should skip (REAL nn-addon registry)");
    
    m.def("add_skip_op", &nn::vector::Add_skip_lowering_op,
          py::arg("op"),
          "Add an op to skip list (REAL nn-addon registry)");
    
    m.def("clear_skip_ops", &nn::vector::Clear_skip_lowering_ops,
          "Clear all skip ops (REAL nn-addon registry)");
    
    m.def("should_skip_lowering", 
          py::overload_cast<const std::string&, const std::string&>(&nn::vector::Should_skip_lowering),
          py::arg("domain"), py::arg("op_name"),
          "Check if an op should be skipped by C++ lowering (REAL nn-addon check)");
    
    m.def("get_skip_ops", []() -> std::vector<std::string> {
        const auto& ops = nn::vector::SKIP_LOWERING_REGISTRY::Instance().Get_skip_ops();
        return std::vector<std::string>(ops.begin(), ops.end());
    }, "Get all skip ops as a list (from REAL nn-addon registry)");
}
