//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <torch/extension.h>
#include <pybind11/pybind11.h>

#include "backend_manager.h"
#include "config_manager.h"
#include "config_parser.h"

namespace py = pybind11;

namespace {

void Set_fhe_config(const py::dict& config_dict) {
    ace::runtime::Parse_and_set_config(config_dict);
}

std::string Get_config_summary() {
    const auto& config = ace::runtime::CONFIG_MANAGER::Instance().Get_config();
    return ace::runtime::CONFIG_PRINTER::To_string(config);
}

bool Validate_config() {
    const auto& config = ace::runtime::CONFIG_MANAGER::Instance().Get_config();
    return ace::runtime::CONFIG_VALIDATOR::Is_valid(config);
}

std::string Get_validation_errors() {
    const auto &config = ace::runtime::CONFIG_MANAGER::Instance().Get_config();
    return ace::runtime::CONFIG_VALIDATOR::Validate(config);
}

bool Is_fhe_configured() {
    return ace::runtime::CONFIG_MANAGER::Instance().Is_configured();
}

} // anonymous namespace

PYBIND11_MODULE(runtime, m) {
    m.doc() = "ACE Runtime Extension";

    m.def("set_fhe_config", &Set_fhe_config,
        "Set FHE runtime configuration from Python dict");

    m.def("is_fhe_configured", &Is_fhe_configured,
        "Check if FHE configuration has been set");

    m.def("get_config_summary", &Get_config_summary, "Get config summary");
    m.def("validate_config", &Validate_config, "Validate current config");
    m.def("get_validation_errors", &Get_validation_errors, "Get validation error messages");

    py::class_<ace::runtime::BACKEND_MANAGER>(m, "BackendManager")
        .def(py::init<>())
        .def("register_backend", &ace::runtime::BACKEND_MANAGER::Register_backend, py::arg("name"), py::arg("lib_path"))
        .def("run", &ace::runtime::BACKEND_MANAGER::Run, py::arg("backend_name"), py::arg("inputs"), py::arg("output_name") = "output")
        .def("validate", &ace::runtime::BACKEND_MANAGER::Validate_result, py::arg("tensor"));
}