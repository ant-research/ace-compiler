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

#include <atomic>
#include <mutex>
#include <string>
#include <vector>
#include <pybind11/pybind11.h>
#include <dlfcn.h>

#ifdef USE_CUDA
#include <cuda_runtime_api.h>
#endif

#include "common/tensor.h"

namespace ace {
namespace runtime {

/**
 * @brief Kernel runner for loading and executing compiled FHE kernels.
 *
 * This class manages the lifecycle of a dynamically loaded shared library
 * containing compiled FHE computation kernels. It handles library loading,
 * symbol resolution, and kernel execution.
 *
 * Phase-split inference:
 *   The Prepare_input_only -> Execute -> Get_output sequence must be called
 *   in that exact order. Execute() can run in parallel across threads, but
 *   Prepare_input_only and Get_output must be serialized (e.g., via
 *   #pragma omp critical). The internal _io_mutex enforces this serialization.
 *   Concurrent calls to the phase-split API on the same KERNEL_RUNNER instance
 *   outside of the BATCH_RUNNER critical-section protocol are NOT safe.
 */
class KERNEL_RUNNER {
public:
    //! Phase state for split inference lifecycle enforcement.
    enum class Phase : int {
        IDLE,          //!< No active inference; ready for Prepare_input_only
        PREPARED,      //!< Input prepared; ready for Execute
        EXECUTED,      //!< Computation done; ready for Get_output
    };
    /**
     * @brief Construct a kernel runner with the specified library path.
     *
     * Loads the shared library and resolves kernel symbols immediately.
     * Call Init() to set up the FHE context before Run().
     *
     * @param lib_path Path to the shared library (.so file)
     * @param use_cuda_graph If true, capture CUDA graph on first Execute()
     *                      and replay on subsequent calls to reduce launch
     *                      overhead. Only effective when USE_CUDA is defined.
     * @throws std::runtime_error if library loading or symbol resolution fails
     */
    explicit KERNEL_RUNNER(const std::string& lib_path, bool use_cuda_graph = false);

    /**
     * @brief Destructor that finalizes and closes the loaded library.
     */
    ~KERNEL_RUNNER();

    /**
     * @brief Load kernel symbols from the shared library.
     * @param symbol_name Symbol name prefix (default: "run_cipher_op")
     * @return true if symbols loaded successfully, false otherwise
     */
    bool Load_kernel(const std::string& symbol_name = "run_cipher_op");

    /**
     * @brief Initialize the FHE context (Prepare_context).
     *
     * Call once before multiple Run() calls. If already initialized, returns true.
     * Run() will auto-call this if not already done (backward compatible).
     *
     * @return true if initialization succeeded
     * @throws std::runtime_error if context setup fails
     */
    bool Init();

    /**
     * @brief Execute the loaded kernel with given inputs.
     *
     * Per-inference: Prepare_input + Main_graph + Handle_output.
     * Auto-calls Init() if not already initialized.
     *
     * @param inputs Vector of input tensors
     * @param output_name Name of the output to retrieve (default: "output")
     * @return Pointer to the result data array (caller must free)
     * @throws std::runtime_error if kernel not loaded or execution fails
     */
    double* Run(
        const std::vector<torch::Tensor>& inputs,
        const std::string& output_name = "output"
    );

    /**
     * @brief Prepare input only (phase 1 of split inference).
     *
     * Allocates tensors and calls Prepare_input. Must be called when
     * phase is IDLE; transitions phase to PREPARED.
     * Thread-safe via internal mutex for I/O serialization.
     *
     * @param inputs Vector of input tensors
     * @throws std::runtime_error if kernel not initialized or wrong phase
     */
    void Prepare_input_only(const std::vector<torch::Tensor>& inputs);

    /**
     * @brief Execute the FHE computation (phase 2 of split inference).
     *
     * Calls Main_graph(). Must be called when phase is PREPARED;
     * transitions phase to EXECUTED. Can run in parallel with other
     * threads' Execute() calls (no mutex held).
     *
     * If CUDA Graph was previously captured via Capture_graph(), replays
     * the captured graph instead of calling Main_graph() directly.
     *
     * @return true if execution succeeded
     * @throws std::runtime_error if kernel not loaded or wrong phase
     */
    bool Execute();

    /**
     * @brief Attempt to capture a CUDA Graph for the Execute() phase.
     *
     * Must be called after Init() and before the first Execute().
     * Runs Main_graph() once under stream capture. If capture succeeds,
     * subsequent Execute() calls will replay the graph instead of calling
     * Main_graph() directly, reducing kernel launch overhead.
     *
     * WARNING: The loaded kernel's Main_graph() must NOT call
     * cudaMalloc/cudaFree during execution, as these are illegal during
     * stream capture and will cause undefined behavior. Kernels that
     * pre-allocate all GPU memory in Prepare_context() are compatible.
     *
     * @return true if capture succeeded, false if capture failed
     *         (Execute() will fall back to normal execution)
     */
    bool Capture_graph();

    /**
     * @brief Retrieve output (phase 3 of split inference).
     *
     * Calls Handle_output and frees tensors allocated by Prepare_input_only.
     * Must be called when phase is EXECUTED; transitions phase back to IDLE.
     * Thread-safe via internal mutex for I/O serialization.
     *
     * @param output_name Name of the output to retrieve (default: "output")
     * @return Pointer to the result data array (caller must free)
     */
    double* Get_output(const std::string& output_name = "output");

    /**
     * @brief Finalize the FHE context (Finalize_context).
     *
     * Call once after all Run() calls are done. After this, Init() must be
     * called again before the next Run(). If not initialized, does nothing.
     * The shared library remains loaded; it is released on destruction.
     */
    void Finalize();

    /**
     * @brief Check if the kernel is initialized and ready for execution.
     * @return true if initialized, false otherwise
     */
    bool Is_initialized() const { return _initialized.load(); }

    /**
     * @brief Check if the kernel library is loaded.
     * @return true if loaded, false otherwise
     */
    bool Is_loaded() const { return _handle != nullptr; }

private:
    std::string _lib_path;              //!< Path to the shared library
    void* _handle = nullptr;            //!< dlopen handle to the library
    std::atomic<bool> _initialized{false};  //!< Whether Init() has been called
    std::atomic<Phase> _phase{Phase::IDLE}; //!< Current phase in split inference
    bool _use_cuda_graph;               //!< Whether to use CUDA Graph for Execute()

    //! Mutex for serializing I/O operations in split inference mode
    std::mutex _io_mutex;

    //! Tensors allocated by Prepare_input_only, freed by Get_output
    std::vector<TENSOR*> _allocated_tensors;

#ifdef USE_CUDA
    cudaGraph_t _graph = nullptr;           //!< Captured CUDA graph
    cudaGraphExec_t _graph_exec = nullptr;  //!< Instantiated executable graph
    bool _graph_captured = false;           //!< Whether graph has been captured
    bool _graph_capture_failed = false;     //!< Whether graph capture failed (fallback)
    cudaStream_t _stream = nullptr;         //!< CUDA stream for graph capture
#endif

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