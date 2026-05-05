//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef ACE_RUNTIME_BACKEND_MANAGER_H
#define ACE_RUNTIME_BACKEND_MANAGER_H

#include <torch/extension.h>
#include <string>
#include <unordered_map>
#include <memory>

#include "kernel_runner.h"
#include "log.h"
#include "validator.h"

namespace ace {
namespace runtime {

/**
 * @brief Manager for multiple FHE backend kernels.
 *
 * This class manages the registration and execution of multiple kernel
 * backends. Each backend is identified by a unique name and can be
 * executed independently.
 */
class PYBIND11_EXPORT BACKEND_MANAGER {
public:
    /**
     * @brief Default constructor.
     */
    BACKEND_MANAGER() = default;

    /**
     * @brief Register a new backend kernel.
     * @param name Unique identifier for the backend
     * @param lib_path Path to the shared library containing the kernel
     * @throws std::runtime_error if kernel loading fails
     */
    void Register_backend(const std::string& name, const std::string& lib_path);

    /**
     * @brief Execute a registered backend kernel.
     * @param backend_name Name of the registered backend
     * @param inputs Vector of input tensors
     * @param output_name Name of the output to retrieve (default: "output")
     * @return Output tensor from the kernel execution
     * @throws std::runtime_error if backend not found or execution fails
     */
    torch::Tensor Run(
        const std::string& backend_name,
        const std::vector<torch::Tensor>& inputs,
        const std::string& output_name = "output"
    );

    /**
     * @brief Validate execution result against expected values.
     * @param tensor Expected output tensor
     * @return true if validation passes, false otherwise
     */
    bool Validate_result(const torch::Tensor& tensor);

private:
    double*     _result = nullptr;      //!< Pointer to last execution result
    std::unordered_map<std::string, std::unique_ptr<KERNEL_RUNNER>> _runners;  //!< Registered kernel runners
};

} // namespace runtime
} // namespace ace

#endif // ACE_RUNTIME_BACKEND_MANAGER_H