//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef ACE_RUNTIME_KERNEL_RUNNER_H
#define ACE_RUNTIME_KERNEL_RUNNER_H

#include <array>
#include <atomic>
#include <dlfcn.h>
#include <mutex>
#include <pybind11/pybind11.h>
#include <string>
#include <torch/extension.h>
#include <vector>

#ifdef USE_CUDA
#include <cuda_runtime_api.h>
#endif

#include "common/io_api.h"  // IO_MAX_SLOTS — unified with rtlib per-session Io limit
#include "common/tensor.h"

namespace ace
{
namespace runtime
{

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
class KERNEL_RUNNER
{
public:
    //! Phase state for split inference lifecycle enforcement.
    enum class Phase : int
    {
        IDLE,      //!< No active inference; ready for Prepare_input_only
        PREPARED,  //!< Input prepared; ready for Execute
        EXECUTED,  //!< Computation done; ready for Get_output
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
    double* Run(const std::vector<torch::Tensor>& inputs, const std::string& output_name = "output");

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

    //-------------------------------------------------------------------------
    // KMS key management — dlsym contract on the backend .so
    //
    // Thin wrappers over the Save_keys / Load_keys / ... symbols resolved in
    // Load_kernel(). Backed by FheContext::Current().key_mgr() inside the
    // backend .so (so a context must be live — Init() / Run() first). Exposed
    // to Python via _C / FHERuntime instead of the retired ctypes FHert_* path.
    // Contract: fhe/kms/kms_runtime_api.h.
    //-------------------------------------------------------------------------

    //! Save all keys (SK+PK+RLK+GK) to `key_dir`. @return 0 on success, -1 on failure.
    int KmsSaveKeys(const std::string& key_dir);

    //! Save evaluation keys (RLK+GK) only. @return 0 on success, -1 on failure.
    int KmsSaveEvalKeys(const std::string& key_dir);

    //! Load all keys from `key_dir`. @return 0 on success, -1 on failure.
    int KmsLoadKeys(const std::string& key_dir);

    //! Load evaluation keys (RLK+GK) only. @return 0 on success, -1 on failure.
    int KmsLoadEvalKeys(const std::string& key_dir);

    //! @return 1 if keys were loaded from KMS, 0 if generated at runtime.
    int KmsKeysLoadedFromKms();

    //! @return wall-clock seconds of the most recent galois-key load (-1.0 if none).
    double KmsGetGaloisLoadTimeSeconds();

    //! Mode B (multi-user): create an extra session bound to the current
    //! context, loading a full key bundle from `key_dir`. @return session index >=1, -1 on error.
    int KmsCreateSession(const std::string& key_dir);

    //! Mode B: switch active session by index (0=primary, >=1=extra). @return 0 on success, -1 on error.
    int KmsSwitchSession(int idx);

    //! Mode B-P: activate session `idx` on the calling thread (sets thread-local
    //! evaluator/key routing + backend per-thread stream/device-ctx). @return 0/-1.
    int KmsActivateSession(int idx);

    //! Release all galois-key slots for session idx (free HBM). Session
    //! structure stays; only galois key bytes evicted. @return 0/-1.
    int KmsReleaseSession(int idx);

    //! Query how many sessions are currently resident (primary + extra).
    //! @return number of sessions (>=1), or -1 on error.
    int KmsNumSessions();

    //! Mode B-P: run inference bound to a SPECIFIC session. Concurrency-safe:
    //! each session has its own phase slot + I/O mutex + tensor list, so
    //! Run_session(i) and Run_session(j) can execute concurrently on two threads.
    //! CUDA graph is bypassed (per-session streams are incompatible with graph
    //! capture/replay). @return raw double* result buffer (caller frees).
    double* Run_session(int session_idx, const std::vector<torch::Tensor>& inputs,
                        const std::string& output_name = "output");

    //! Mode B-P + ciphertext-in/out: like Run_session but takes serialized
    //! ciphertext bytes (client-encrypted) per input, evals with the session's
    //! keys (no server-side encrypt/decrypt — backend does serialize/Io/eval
    //! via the dlsym'd Kms_run_cipher), and returns the serialized output
    //! ciphertext bytes. Input names come from get_encode_scheme_c(i)->_name.
    //! phase: lock-only (no PREPARED/EXECUTED — run_cipher is monolithic).
    //! @return output ciphertext bytes; empty on error.
    std::string Run_cipher(int session_idx, const std::vector<std::string>& input_bytes,
                           const std::string& output_name = "output");

    //! Per-stage seconds of the last Run_cipher on THIS thread:
    //! [deser_sync, launder_sync, kernel(eval), serialize]. Call on the same
    //! worker thread that ran Run_cipher (timing is thread-local in the backend).
    std::vector<double> LastRunCipherTiming();

    //! Mode B-P: per-session phase-split API (see Prepare_input_only/Execute/
    //! Get_output). Each takes a session_idx; slots are independent.
    void Prepare_input_only(int session_idx, const std::vector<torch::Tensor>& inputs);
    bool Execute(int session_idx);
    double* Get_output(int session_idx, const std::string& output_name = "output");

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
    std::string _lib_path;                   //!< Path to the shared library
    void* _handle = nullptr;                 //!< dlopen handle to the library
    std::atomic<bool> _initialized{false};   //!< Whether Init() has been called
    std::atomic<Phase> _phase{Phase::IDLE};  //!< Current phase in split inference
    bool _use_cuda_graph;                    //!< Whether to use CUDA Graph for Execute()

    //! Mutex for serializing I/O operations in split inference mode
    std::mutex _io_mutex;

    //! Tensors allocated by Prepare_input_only, freed by Get_output
    std::vector<TENSOR*> _allocated_tensors;

    //! Mode B-P: max concurrent sessions supported by the per-session slots.
    //! Mirrors IO_MAX_SLOTS in rtlib/io_api.h — a static_assert catches
    //! divergence; if they ever drift, one path breaks silently.
    static constexpr int kMaxSessions = IO_MAX_SLOTS;

    static_assert(kMaxSessions == IO_MAX_SLOTS, "KERNEL_RUNNER::kMaxSessions must match IO_MAX_SLOTS");
    //! Per-session split-inference state (Mode B-P). Index 0..kMaxSessions-1.
    //! Each session has an independent phase machine + I/O mutex + tensor list,
    //! so Run_session(i)/Run_session(j) can run concurrently on two threads.
    std::array<std::atomic<Phase>, kMaxSessions> _phase_per_session{};  // all Phase::IDLE
    std::array<std::mutex, kMaxSessions> _io_mutex_per_session;         //!< guarded (constructed default)
    std::array<std::vector<TENSOR*>, kMaxSessions> _allocated_tensors_per_session;

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

    // KMS dlsym signatures (fhe/kms/kms_runtime_api.h).
    using Save_keys_func = int (*)(const char*, const uint32_t*, size_t);
    using Load_keys_func = int (*)(const char*);
    using Save_eval_keys_func = int (*)(const char*, const uint32_t*, size_t);
    using Load_eval_keys_func = int (*)(const char*);
    using Keys_loaded_func = int (*)();
    using Galois_load_time_func = double (*)();
    using Create_session_func = int (*)(const char*);  //!< Mode B: create extra session
    using Switch_session_func = int (*)(int);          //!< Mode B: switch active session
    using Activate_session_func = int (*)(int);        //!< Mode B-P: activate session on calling thread
    using Release_session_func = int (*)(int);         //!< Release session galois keys (free HBM)
    using Num_sessions_func = int (*)();               //!< Query number of resident sessions
    // Kms_run_cipher: ciphertext-in/out. kernel_fn = _run_kernel (Main_graph).
    using Run_cipher_func = long (*)(int, bool (*)(), const char* const*, const char* const*, const size_t*, size_t,
                                     char*, size_t, const char*);
    // Kms_last_runcipher_timing: returns the last RunCipher's per-stage seconds
    // (deser_sync/launder_sync/kernel/serialize) for THIS thread — so the server
    // can echo the split to the client. Optional, lazily resolved (old .so: null).
    using Last_runcipher_timing_func = int (*)(double*);

    Prepare_context_func _prepare_ctx = nullptr;  //!< Prepare context callback
    Finalize_context_func _final_ctx = nullptr;   //!< Finalize context callback
    Prepare_input_func _prepare_input = nullptr;  //!< Prepare input callback
    Handle_output_func _handle_output = nullptr;  //!< Handle output callback
    Kernel_func _run_kernel = nullptr;            //!< Main kernel execution callback

    // KMS callbacks — optional: resolved lazily, null if backend doesn't export.
    Save_keys_func _kms_save_keys = nullptr;
    Load_keys_func _kms_load_keys = nullptr;
    Save_eval_keys_func _kms_save_eval_keys = nullptr;
    Load_eval_keys_func _kms_load_eval_keys = nullptr;
    Keys_loaded_func _kms_keys_loaded = nullptr;
    Galois_load_time_func _kms_galois_load_time = nullptr;
    Create_session_func _kms_create_session = nullptr;      //!< Mode B
    Switch_session_func _kms_switch_session = nullptr;      //!< Mode B
    Activate_session_func _kms_activate_session = nullptr;  //!< Mode B-P
    Release_session_func _kms_release_session = nullptr;    //!< Release galois keys (free HBM)
    Num_sessions_func _kms_num_sessions = nullptr;          //!< Query number of resident sessions
    Run_cipher_func _kms_run_cipher = nullptr;              //!< Ciphertext-in/out (P2'')
    Last_runcipher_timing_func _kms_last_runcipher_timing =
        nullptr;  //!< Per-stage split of last RunCipher (this thread)
};

}  // namespace runtime
}  // namespace ace

#endif  // ACE_RUNTIME_KERNEL_RUNNER_H