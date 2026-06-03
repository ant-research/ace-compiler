//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <torch/extension.h>
#include <pybind11/pybind11.h>

#include "ace/common/logging.h"
#include "ace/runtime/provider_manager.h"
#include "ace/runtime/config_manager.h"
#include "ace/runtime/config_parser.h"
#include "ace/runtime/validator.h"
#include "ace/runtime/batch_runner.h"

namespace py = pybind11;

namespace {

void Set_fhe_config(const py::dict& config_dict) {
    ace::runtime::Parse_and_set_config(config_dict);
}

std::string Get_config_summary() {
    return ace::runtime::CONFIG_MANAGER::Instance().Get_config().To_string();
}

bool Validate_config() {
    const auto config = ace::runtime::CONFIG_MANAGER::Instance().Get_config();
    return config.Is_valid();
}

std::string Get_validation_errors() {
    const auto config = ace::runtime::CONFIG_MANAGER::Instance().Get_config();
    return config.Validate();
}

} // anonymous namespace

PYBIND11_MODULE(runtime, m) {
    m.doc() = "ACE Runtime Extension";

    // Initialize spdlog with Python sink on module import
    ace::common::Init_logging("ace.runtime");

    m.def("set_config", &Set_fhe_config,
        "Set FHE runtime configuration from Python dict");

    m.def("config_summary", &Get_config_summary, "Get config summary string");
    m.def("validate_config", &Validate_config, "Validate current config");
    m.def("config_errors", &Get_validation_errors, "Get config validation error messages");

    m.def("set_log_level", &ace::common::Set_log_level,
        "Set C++ runtime log level (TRACE, DEBUG, INFO, WARN, ERROR, CRITICAL, OFF)");

    m.def("validate_result", &ace::runtime::Validate_result,
        py::arg("result"), py::arg("expected"),
        "Validate FHE result tensor against expected tensor");

    // Bind BATCH_TIMING
    py::class_<ace::runtime::BATCH_TIMING>(m, "BatchTiming")
        .def(py::init<>())
        .def_readonly("total_ms", &ace::runtime::BATCH_TIMING::total_ms)
        .def_readonly("avg_per_image_ms", &ace::runtime::BATCH_TIMING::avg_per_image_ms)
        .def_readonly("min_image_ms", &ace::runtime::BATCH_TIMING::min_image_ms)
        .def_readonly("max_image_ms", &ace::runtime::BATCH_TIMING::max_image_ms)
        .def_readonly("num_images", &ace::runtime::BATCH_TIMING::num_images);

    // Bind BATCH_RESULT
    py::class_<ace::runtime::BATCH_RESULT>(m, "BatchResult")
        .def(py::init<>())
        .def_readonly("outputs", &ace::runtime::BATCH_RESULT::outputs)
        .def_readonly("timing", &ace::runtime::BATCH_RESULT::timing)
        .def_readonly("num_success", &ace::runtime::BATCH_RESULT::num_success)
        .def_readonly("num_failure", &ace::runtime::BATCH_RESULT::num_failure);

    // Bind PROVIDER_MANAGER
    py::class_<ace::runtime::PROVIDER_MANAGER>(m, "ProviderManager")
        .def(py::init<>())
        .def("register_kernel", &ace::runtime::PROVIDER_MANAGER::Register_provider,
             py::arg("name"), py::arg("lib_path"),
             py::arg("use_cuda_graph") = false)
        .def("init", &ace::runtime::PROVIDER_MANAGER::Init, py::arg("name"))
        .def("execute", [](ace::runtime::PROVIDER_MANAGER& self,
                    const std::string& name,
                    const std::vector<torch::Tensor>& inputs) {
            return self.Run(name, inputs, "output");
        }, py::arg("name"), py::arg("inputs"),
           "Run single FHE inference")
        .def("execute_batch", [](ace::runtime::PROVIDER_MANAGER& self,
                             const std::string& name,
                             const std::vector<std::vector<torch::Tensor>>& batch_inputs,
                             bool parallel,
                             int num_threads,
                             bool verbose) {
            if (parallel) {
                return self.Run_batch_parallel(name, batch_inputs, "output", num_threads, verbose);
            } else {
                return self.Run_batch(name, batch_inputs, "output", verbose);
            }
        }, py::arg("name"), py::arg("batch_inputs"),
           py::arg("parallel") = false,
           py::arg("num_threads") = 0,
           py::arg("verbose") = false,
           "Run batch FHE inference (sequential or parallel with OpenMP)")
        .def("finalize", &ace::runtime::PROVIDER_MANAGER::Finalize,
             py::arg("name"))
        .def("capture_graph", &ace::runtime::PROVIDER_MANAGER::Capture_graph,
             py::arg("name"),
             "Attempt CUDA Graph capture for Execute() replay");
}