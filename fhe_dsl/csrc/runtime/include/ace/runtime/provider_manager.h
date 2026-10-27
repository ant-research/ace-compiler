//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef ACE_RUNTIME_PROVIDER_MANAGER_H
#define ACE_RUNTIME_PROVIDER_MANAGER_H

#include "ace/runtime/batch_runner.h"
#include "ace/runtime/kernel_runner.h"

#include <memory>
#include <shared_mutex>
#include <string>
#include <torch/extension.h>
#include <unordered_map>

namespace ace
{
namespace runtime
{

/**
 * @brief Manager for multiple FHE runtime providers.
 *
 * This class manages the registration and execution of multiple runtime
 * providers. Each provider is identified by a unique name and can be
 * executed independently.
 */
class PYBIND11_EXPORT PROVIDER_MANAGER
{
public:
    /**
     * @brief Default constructor.
     */
    PROVIDER_MANAGER() = default;

    /**
     * @brief Register a new runtime provider.
     * @param name Unique identifier for the provider
     * @param lib_path Path to the shared library containing the kernel
     * @param use_cuda_graph If true, use CUDA Graph for kernel execution replay
     * @throws std::runtime_error if kernel loading fails
     */
    void Register_provider(const std::string& name, const std::string& lib_path, bool use_cuda_graph = false);

    /**
     * @brief Initialize a registered provider (dlopen + Load_kernel + Prepare_context).
     *
     * Call once before multiple Run() calls to avoid per-inference context setup.
     * Run() will auto-call Init() if not already done (backward compatible).
     *
     * @param provider_name Name of the registered provider
     * @throws std::runtime_error if provider not found or initialization fails
     */
    void Init(const std::string& provider_name);

    /**
     * @brief Execute a registered provider kernel (single inference).
     * @param provider_name Name of the registered provider
     * @param inputs Vector of input tensors
     * @param output_name Name of the output to retrieve (default: "output")
     * @return Output tensor from the kernel execution
     * @throws std::runtime_error if provider not found or execution fails
     */
    torch::Tensor Run(const std::string& provider_name, const std::vector<torch::Tensor>& inputs,
                      const std::string& output_name = "output");

    //! Mode B-P: run inference bound to a specific session on the calling
    //! thread. Concurrency-safe across sessions (independent phase slots).
    torch::Tensor Run_session(const std::string& provider_name, int session_idx,
                              const std::vector<torch::Tensor>& inputs, const std::string& output_name = "output");

    //! Mode B-P + ciphertext-in/out (P2''). Accepts serialized ciphertext
    //! bytes per input, evals with the session's keys (no encrypt/decrypt),
    //! returns serialized output ciphertext bytes. GIL released during native
    //! execution. input_bytes must be ordered to match the compiled input
    //! scheme (get_encode_scheme_c(i)->_name).
    std::string Run_cipher(const std::string& provider_name, int session_idx,
                           const std::vector<std::string>& input_bytes, const std::string& output_name = "output");

    //! Per-stage seconds [deser_sync, launder_sync, kernel(eval), serialize] of
    //! the last Run_cipher on THIS thread. Caller must be on the same worker
    //! thread that ran Run_cipher (backend timing is thread-local).
    std::vector<double> LastRunCipherTiming(const std::string& provider_name);

    /**
     * @brief Run sequential batch inference on a registered provider.
     *
     * Processes images one at a time. GIL is released during computation.
     *
     * @param provider_name Name of the registered provider
     * @param batch_inputs Vector of input tensor lists (one per image)
     * @param output_name Name of the output to retrieve
     * @param verbose If true, print per-image progress and results
     * @return BATCH_RESULT with outputs, timing, and success/failure counts
     */
    BATCH_RESULT Run_batch(const std::string& provider_name,
                           const std::vector<std::vector<torch::Tensor>>& batch_inputs,
                           const std::string& output_name = "output", bool verbose = false);

    /**
     * @brief Run parallel batch inference on a registered provider (OpenMP).
     *
     * Uses the critical-section pattern: Prepare_input and Get_output are
     * serialized, while Execute runs in parallel across threads.
     * GIL is released during computation.
     *
     * @param provider_name Name of the registered provider
     * @param batch_inputs Vector of input tensor lists (one per image)
     * @param output_name Name of the output to retrieve
     * @param num_threads Number of OpenMP threads (0 = hardware concurrency)
     * @param verbose If true, print per-image progress and results
     * @return BATCH_RESULT with outputs, timing, and success/failure counts
     */
    BATCH_RESULT Run_batch_parallel(const std::string& provider_name,
                                    const std::vector<std::vector<torch::Tensor>>& batch_inputs,
                                    const std::string& output_name = "output", int num_threads = 0,
                                    bool verbose = false);

    /**
     * @brief Finalize a registered provider (Finalize_context + dlclose).
     *
     * Call once after all Run() calls are done to release FHE context resources.
     * After this, Init() must be called again before the next Run().
     *
     * @param provider_name Name of the registered provider
     * @throws std::runtime_error if provider not found
     */
    void Finalize(const std::string& provider_name);

    /**
     * @brief Attempt CUDA Graph capture for a registered provider.
     *
     * Must be called after Init(). Runs Main_graph() once under stream capture.
     * If successful, subsequent Execute() calls will replay the graph.
     *
     * WARNING: Only call this if the kernel's Main_graph() does NOT call
     * cudaMalloc/cudaFree (which are illegal during stream capture).
     *
     * @param provider_name Name of the registered provider
     * @return true if capture succeeded
     * @throws std::runtime_error if provider not found
     */
    bool Capture_graph(const std::string& provider_name);

    //-------------------------------------------------------------------------
    // KMS key management — forwarded to the provider's KERNEL_RUNNER.
    //
    // Requires the provider to have been Init()'d (active FheContext).
    // Replaces the retired Python ctypes FHert_* access path; Python reaches
    // these via KernelExecutor → _C.ProviderManager.
    // Contract: fhe/kms/kms_runtime_api.h.
    //-------------------------------------------------------------------------
    int Save_keys(const std::string& provider_name, const std::string& key_dir);
    int Save_eval_keys(const std::string& provider_name, const std::string& key_dir);
    int Load_keys(const std::string& provider_name, const std::string& key_dir);
    int Load_eval_keys(const std::string& provider_name, const std::string& key_dir);
    int Keys_loaded_from_kms(const std::string& provider_name);
    double Get_galois_load_time_seconds(const std::string& provider_name);

    //! Mode B (multi-user): create an extra session (full key bundle from key_dir). @return session idx >=1, -1 on
    //! error.
    int Create_session(const std::string& provider_name, const std::string& key_dir);
    //! Mode B: switch active session by index (0=primary, >=1=extra). @return 0 on success, -1 on error.
    int Switch_session(const std::string& provider_name, int idx);
    int Release_session(const std::string& provider_name, int idx);

    //! Query the number of sessions currently resident (primary + extra).
    int Num_sessions(const std::string& provider_name);

private:
    //! Get a shared pointer to a runner (throws if not found).
    std::shared_ptr<KERNEL_RUNNER> Get_runner(const std::string& provider_name) const;

    std::unordered_map<std::string, std::shared_ptr<KERNEL_RUNNER>> _runners;  //!< Registered kernel runners
    mutable std::shared_mutex _runners_mutex;  //!< Protects _runners map for concurrent access
};

}  // namespace runtime
}  // namespace ace

#endif  // ACE_RUNTIME_PROVIDER_MANAGER_H