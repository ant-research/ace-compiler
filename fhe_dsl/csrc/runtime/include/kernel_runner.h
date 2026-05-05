//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef ACE_RUNTIME_KERNEL_RUNNER_H
#define ACE_RUNTIME_KERNEL_RUNNER_H

#include <torch/extension.h>

#include <string>
#include <vector>
#include <pybind11/pybind11.h>
#include <dlfcn.h>

#include "common/tensor.h"

namespace ace {
namespace runtime {

/**
 * @brief Kernel runner for loading and executing compiled FHE kernels.
 *
 * This class manages the lifecycle of a dynamically loaded shared library
 * containing compiled FHE computation kernels. It handles library loading,
 * symbol resolution, and kernel execution.
 */
class KERNEL_RUNNER {
public:
    /**
     * @brief Construct a kernel runner with the specified library path.
     * @param lib_path Path to the shared library (.so file)
     * @throws std::runtime_error if library loading fails
     */
    explicit KERNEL_RUNNER(const std::string& lib_path);

    /**
     * @brief Destructor that closes the loaded library.
     */
    ~KERNEL_RUNNER();

    /**
     * @brief Load kernel symbols from the shared library.
     * @param symbol_name Symbol name prefix (default: "run_cipher_op")
     * @return true if symbols loaded successfully, false otherwise
     */
    bool Load_kernel(const std::string& symbol_name = "run_cipher_op");

    /**
     * @brief Execute the loaded kernel with given inputs.
     * @param inputs Vector of input tensors
     * @param output_name Name of the output to retrieve (default: "output")
     * @return Pointer to the result data array
     * @throws std::runtime_error if kernel not loaded or execution fails
     */
    double* Run(
        const std::vector<torch::Tensor>& inputs,
        const std::string& output_name = "output"
    );

    /**
     * @brief Check if the kernel is loaded and ready for execution.
     * @return true if loaded, false otherwise
     */
    bool Is_loaded() const { return _handle != nullptr; }

private:
    std::string _lib_path;              //!< Path to the shared library
    void* _handle = nullptr;            //!< dlopen handle to the library

    //! Function pointer types for kernel callbacks
    using Prepare_context_func = void (*)();
    using Finalize_context_func = void (*)();
    using Prepare_input_func = void (*)(TENSOR*, const char*);
    using Handle_output_func = double* (*)(const char*);
    using Kernel_func = bool (*)();

    Prepare_context_func _prepare_ctx = nullptr;       //!< Prepare context callback
    Finalize_context_func _final_ctx  = nullptr;       //!< Finalize context callback
    Prepare_input_func _prepare_input = nullptr;       //!< Prepare input callback
    Handle_output_func _handle_output = nullptr;       //!< Handle output callback
    Kernel_func _run_kernel           = nullptr;       //!< Main kernel execution callback
};

} // namespace runtime
} // namespace ace

#endif // ACE_RUNTIME_KERNEL_RUNNER_H