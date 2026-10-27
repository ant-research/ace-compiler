//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "ace/runtime/kernel_runner.h"

#include "ace/runtime/fhe_profiling.h"
#include "ace/runtime/rtlib_interface.h"

#include <stdexcept>

namespace ace
{
namespace runtime
{

KERNEL_RUNNER::KERNEL_RUNNER(const std::string& lib_path, bool use_cuda_graph)
    : _lib_path(lib_path), _use_cuda_graph(use_cuda_graph)
{
    // RTLD_LOCAL: keep the kernel .so's exported symbols (compiler-generated
    // _cst_N constants, etc.) out of the global symbol table. With RTLD_GLOBAL,
    // loading a second kernel .so in the same process made the dynamic linker
    // resolve its same-named _cst_N references to the FIRST kernel's symbols,
    // corrupting weights/rotation tables and producing garbage rotation steps
    // (e.g. "step count too large"). RTLD_LOCAL still lets the kernel resolve
    // its NEEDED deps (libFHErt_phantom.so, etc.) normally.
    //
    // RTLD_NODELETE: prevent .so unloading on dlclose.  When switching backends
    // (e.g. GPU → ant-cpu), the old .so's global state (Context, Io_init/fini,
    // callback registrations) must not be destroyed while the new .so may still
    // reference symbols in the same shared libFHErt_common.so.  dlclose would
    // make these dangling; RTLD_NODELETE makes dlclose a no-op for the refcount.
    _handle = dlopen(lib_path.c_str(), RTLD_LAZY | RTLD_LOCAL | RTLD_NODELETE);
    if (!_handle) {
        throw std::runtime_error("Failed to load library: " + std::string(dlerror()));
    }

    if (!Load_kernel()) {
        dlclose(_handle);
        _handle = nullptr;
        throw std::runtime_error("Failed to load kernel symbols from: " + lib_path);
    }

#ifdef USE_CUDA
    if (_use_cuda_graph) {
        cudaError_t err = cudaStreamCreateWithFlags(&_stream, cudaStreamNonBlocking);
        if (err != cudaSuccess) {
            _use_cuda_graph = false;
        }
    }
#endif
}

KERNEL_RUNNER::~KERNEL_RUNNER()
{
    Finalize();
#ifdef USE_CUDA
    if (_graph_exec) {
        cudaGraphExecDestroy(_graph_exec);
        _graph_exec = nullptr;
    }
    if (_graph) {
        cudaGraphDestroy(_graph);
        _graph = nullptr;
    }
    if (_stream) {
        cudaStreamDestroy(_stream);
        _stream = nullptr;
    }
#endif
    if (_handle) {
        dlclose(_handle);
        _handle = nullptr;
    }
}

bool KERNEL_RUNNER::Load_kernel(const std::string& symbol_name)
{
    if (!_handle)
        return false;

    dlerror();  // Clear any existing error

    _prepare_ctx = (Prepare_context_func)dlsym(_handle, "Prepare_context");
    _final_ctx = (Finalize_context_func)dlsym(_handle, "Finalize_context");
    _prepare_input = (Prepare_input_func)dlsym(_handle, "Prepare_input");
    _handle_output = (Handle_output_func)dlsym(_handle, "Handle_output");
    _run_kernel = (Kernel_func)dlsym(_handle, "Main_graph");

    // KMS dlsym contract — optional. Resolved when the backend .so exports them
    // (fhe_lib backends do); left null otherwise. Not required for Run() path.
    // Names are Kms_* (not bare Save_keys/...) to avoid colliding with the
    // inline Save_keys/Load_keys wrappers in rt_<backend>.h.
    _kms_save_keys = (Save_keys_func)dlsym(_handle, "Kms_save_keys");
    _kms_load_keys = (Load_keys_func)dlsym(_handle, "Kms_load_keys");
    _kms_save_eval_keys = (Save_eval_keys_func)dlsym(_handle, "Kms_save_eval_keys");
    _kms_load_eval_keys = (Load_eval_keys_func)dlsym(_handle, "Kms_load_eval_keys");
    _kms_keys_loaded = (Keys_loaded_func)dlsym(_handle, "Kms_keys_loaded_from_kms");
    _kms_galois_load_time = (Galois_load_time_func)dlsym(_handle, "Kms_get_galois_load_time_seconds");

    // Mode B (multi-user) — optional, resolved lazily.
    _kms_create_session = (Create_session_func)dlsym(_handle, "Kms_create_session");
    _kms_switch_session = (Switch_session_func)dlsym(_handle, "Kms_switch_session");
    // Mode B-P — optional, resolved lazily.
    _kms_activate_session = (Activate_session_func)dlsym(_handle, "Kms_activate_session");
    _kms_release_session = (Release_session_func)dlsym(_handle, "Kms_release_session");
    // Kms_num_sessions: optional, lazy. Queries how many sessions are resident.
    _kms_num_sessions = (Num_sessions_func)dlsym(_handle, "Kms_num_sessions");
    // P2'': ciphertext-in/out — optional, lazy. Resolved via the kernel.so's
    // NEEDED libFHErt_phantom.so chain (same as the other Kms_* symbols).
    _kms_run_cipher = (Run_cipher_func)dlsym(_handle, "Kms_run_cipher");
    // Per-stage split of the last run_cipher on THIS thread (deser/launder/
    // kernel/serialize). Used by the server to echo timings to the client.
    _kms_last_runcipher_timing = (Last_runcipher_timing_func)dlsym(_handle, "Kms_last_runcipher_timing");

    if (!_prepare_ctx || !_run_kernel) {
        return false;
    }

    return true;
}

bool KERNEL_RUNNER::Init()
{
    if (_initialized.load())
        return true;

    FHE_PROFILE_SCOPE_WITH_MEM("fhe::init");
    record_mem_snapshot("init_before");
    _prepare_ctx();
    record_mem_snapshot("init_after");
    _initialized.store(true);
    return true;
}

double* KERNEL_RUNNER::Run(const std::vector<torch::Tensor>& inputs, const std::string& output_name)
{
    FHE_PROFILE_SCOPE_WITH_MEM("fhe::run");
    // Delegate to Run_session(0) — same phase-split path (Prepare_input_only →
    // Execute → Get_output on session 0). The standalone Alloc_tensor+Free_tensor
    // path is retired; all single-inference calls now share one implementation.
    return Run_session(0, inputs, output_name);
}

void KERNEL_RUNNER::Prepare_input_only(const std::vector<torch::Tensor>& inputs)
{
    FHE_PROFILE_SCOPE_WITH_MEM("fhe::prepare_input");
    std::lock_guard<std::mutex> lock(_io_mutex);

    Phase expected = Phase::IDLE;
    if (!_phase.compare_exchange_strong(expected, Phase::PREPARED)) {
        throw std::runtime_error("Prepare_input_only: invalid phase transition (expected IDLE, got " +
                                 std::to_string(static_cast<int>(expected)) + ")");
    }

    if (!_initialized.load()) {
        if (!Init()) {
            _phase.store(Phase::IDLE);
            throw std::runtime_error("Failed to initialize kernel for: " + _lib_path);
        }
    }

    // Free any previously allocated tensors
    for (auto tensor : _allocated_tensors) {
        Free_tensor(tensor);
    }
    _allocated_tensors.clear();

    record_mem_snapshot("prepare_input_before");

    // Alloc_tensor + Prepare_input
    for (size_t i = 0; i < inputs.size(); ++i) {
        auto t = inputs[i].to(torch::kFloat64);
        auto sizes = t.sizes();
        if (sizes.size() != 4) {
            _phase.store(Phase::IDLE);
            throw std::runtime_error("Input must be 4D [N,C,H,W]");
        }
        size_t n = sizes[0], c = sizes[1], h = sizes[2], w = sizes[3];
        const double* data = static_cast<const double*>(t.data_ptr());

        TENSOR* tensor = Alloc_tensor(n, c, h, w, data);
        _allocated_tensors.push_back(tensor);

        std::string name = get_encode_scheme_c(i)->_name;
        _prepare_input(tensor, name.c_str());
    }

    record_mem_snapshot("prepare_input_after");
}

bool KERNEL_RUNNER::Execute()
{
    FHE_PROFILE_SCOPE_WITH_MEM("fhe::execute");
    Phase expected = Phase::PREPARED;
    if (!_phase.compare_exchange_strong(expected, Phase::EXECUTED)) {
        throw std::runtime_error("Execute: invalid phase transition (expected PREPARED, got " +
                                 std::to_string(static_cast<int>(expected)) + ")");
    }

    if (!_run_kernel) {
        throw std::runtime_error("Kernel not loaded for: " + _lib_path);
    }
    record_mem_snapshot("execute_before");

#ifdef USE_CUDA
    if (_use_cuda_graph && _graph_captured && !_graph_capture_failed) {
        // Replay captured graph
        cudaError_t err = cudaGraphLaunch(_graph_exec, _stream);
        if (err != cudaSuccess) {
            _graph_capture_failed = true;
            bool result = _run_kernel();
            record_mem_snapshot("execute_after");
            return result;
        }
        cudaStreamSynchronize(_stream);
        record_mem_snapshot("execute_after");
        return true;
    }
#endif

    bool result = _run_kernel();
    record_mem_snapshot("execute_after");
    return result;
}

bool KERNEL_RUNNER::Capture_graph()
{
#ifdef USE_CUDA
    if (!_use_cuda_graph || !_initialized.load() || !_run_kernel) {
        return false;
    }

    // Destroy any previously captured graph
    if (_graph_exec) {
        cudaGraphExecDestroy(_graph_exec);
        _graph_exec = nullptr;
    }
    if (_graph) {
        cudaGraphDestroy(_graph);
        _graph = nullptr;
    }
    _graph_captured = false;
    _graph_capture_failed = false;

    // Run Main_graph() once under stream capture
    cudaError_t err = cudaStreamBeginCapture(_stream, cudaStreamCaptureModeGlobal);
    if (err != cudaSuccess) {
        _graph_capture_failed = true;
        return false;
    }

    _run_kernel();

    err = cudaStreamEndCapture(_stream, &_graph);
    if (err != cudaSuccess || _graph == nullptr) {
        _graph_capture_failed = true;
        return false;
    }

    err = cudaGraphInstantiate(&_graph_exec, _graph, 0);
    if (err != cudaSuccess) {
        _graph_capture_failed = true;
        cudaGraphDestroy(_graph);
        _graph = nullptr;
        return false;
    }

    _graph_captured = true;
    return true;
#else
    return false;
#endif
}

double* KERNEL_RUNNER::Get_output(const std::string& output_name)
{
    FHE_PROFILE_SCOPE_WITH_MEM("fhe::get_output");
    std::lock_guard<std::mutex> lock(_io_mutex);

    Phase expected = Phase::EXECUTED;
    if (!_phase.compare_exchange_strong(expected, Phase::IDLE)) {
        throw std::runtime_error("Get_output: invalid phase transition (expected EXECUTED, got " +
                                 std::to_string(static_cast<int>(expected)) + ")");
    }

    double* result = _handle_output(output_name.c_str());

    record_mem_snapshot("get_output_before_free");

    // Free tensors allocated by Prepare_input_only
    for (auto tensor : _allocated_tensors) {
        Free_tensor(tensor);
    }
    _allocated_tensors.clear();

    record_mem_snapshot("get_output_after_free");

    return result;
}

void KERNEL_RUNNER::Finalize()
{
    FHE_PROFILE_SCOPE_WITH_MEM("fhe::finalize");
    if (!_initialized.load() || !_final_ctx)
        return;
    record_mem_snapshot("finalize_before");
    _final_ctx();
    record_mem_snapshot("finalize_after");
    _initialized.store(false);

#ifdef USE_CUDA
    // Destroy captured graph; it references the old FHE context's memory
    if (_graph_exec) {
        cudaGraphExecDestroy(_graph_exec);
        _graph_exec = nullptr;
    }
    if (_graph) {
        cudaGraphDestroy(_graph);
        _graph = nullptr;
    }
    _graph_captured = false;
    _graph_capture_failed = false;
#endif
}

// KMS key management — thin wrappers over the dlsym'd backend symbols.
// Requires an active FheContext (call Init()/Run() first); the backend bodies
// read FheContext::Current().key_mgr().
int KERNEL_RUNNER::KmsSaveKeys(const std::string& key_dir)
{
    if (!_kms_save_keys)
        throw std::runtime_error("KERNEL_RUNNER::KmsSaveKeys: backend does not export Save_keys");
    return _kms_save_keys(key_dir.c_str(), nullptr, 0);
}
int KERNEL_RUNNER::KmsSaveEvalKeys(const std::string& key_dir)
{
    if (!_kms_save_eval_keys)
        throw std::runtime_error("KERNEL_RUNNER::KmsSaveEvalKeys: backend does not export Save_eval_keys");
    return _kms_save_eval_keys(key_dir.c_str(), nullptr, 0);
}
int KERNEL_RUNNER::KmsLoadKeys(const std::string& key_dir)
{
    if (!_kms_load_keys)
        throw std::runtime_error("KERNEL_RUNNER::KmsLoadKeys: backend does not export Load_keys");
    return _kms_load_keys(key_dir.c_str());
}
int KERNEL_RUNNER::KmsLoadEvalKeys(const std::string& key_dir)
{
    if (!_kms_load_eval_keys)
        throw std::runtime_error("KERNEL_RUNNER::KmsLoadEvalKeys: backend does not export Load_eval_keys");
    return _kms_load_eval_keys(key_dir.c_str());
}
int KERNEL_RUNNER::KmsKeysLoadedFromKms()
{
    if (!_kms_keys_loaded)
        throw std::runtime_error("KERNEL_RUNNER::KmsKeysLoadedFromKms: backend does not export Keys_loaded_from_kms");
    return _kms_keys_loaded();
}
double KERNEL_RUNNER::KmsGetGaloisLoadTimeSeconds()
{
    if (!_kms_galois_load_time)
        throw std::runtime_error(
            "KERNEL_RUNNER::KmsGetGaloisLoadTimeSeconds: backend does not export Get_galois_load_time_seconds");
    return _kms_galois_load_time();
}
int KERNEL_RUNNER::KmsCreateSession(const std::string& key_dir)
{
    if (!_kms_create_session)
        throw std::runtime_error("KERNEL_RUNNER::KmsCreateSession: backend does not export Create_session");
    return _kms_create_session(key_dir.c_str());
}
int KERNEL_RUNNER::KmsSwitchSession(int idx)
{
    if (!_kms_switch_session)
        throw std::runtime_error("KERNEL_RUNNER::KmsSwitchSession: backend does not export Switch_session");
    return _kms_switch_session(idx);
}

int KERNEL_RUNNER::KmsActivateSession(int idx)
{
    if (!_kms_activate_session)
        throw std::runtime_error("KERNEL_RUNNER::KmsActivateSession: backend does not export Activate_session "
                                 "(Mode B-P requires the Layer-1 phantom+ace Kms_activate_session export)");
    return _kms_activate_session(idx);
}

int KERNEL_RUNNER::KmsReleaseSession(int idx)
{
    if (!_kms_release_session)
        throw std::runtime_error("KERNEL_RUNNER::KmsReleaseSession: backend does not export Release_session");
    return _kms_release_session(idx);
}

int KERNEL_RUNNER::KmsNumSessions()
{
    if (!_kms_num_sessions)
        return -1;  // lazy detection: old .so doesn't export Kms_num_sessions
    return _kms_num_sessions();
}

// ── Mode B-P: per-session phase-split inference ─────────────────────────────
// Each session_idx has its own phase machine + I/O mutex + tensor list, so
// Run_session(i) and Run_session(j) can run concurrently on two threads. The
// calling thread activates session_idx once (sets thread-local evaluator/key
// routing + backend per-thread stream/device-ctx) before any op; the compiled
// kernel's ops then resolve to THIS thread's session via FheContext::Current().

void KERNEL_RUNNER::Prepare_input_only(int session_idx, const std::vector<torch::Tensor>& inputs)
{
    FHE_PROFILE_SCOPE_WITH_MEM("fhe::prepare_input_session");
    if (session_idx < 0 || session_idx >= kMaxSessions)
        throw std::runtime_error("Prepare_input_only(session): session_idx out of range");
    std::lock_guard<std::mutex> lock(_io_mutex_per_session[session_idx]);

    auto& phase = _phase_per_session[session_idx];
    Phase expected = Phase::IDLE;
    if (!phase.compare_exchange_strong(expected, Phase::PREPARED)) {
        throw std::runtime_error("Prepare_input_only(session): invalid phase transition (expected IDLE, got " +
                                 std::to_string(static_cast<int>(expected)) + ")");
    }

    if (!_initialized.load()) {
        if (!Init()) {
            phase.store(Phase::IDLE);
            throw std::runtime_error("Failed to initialize kernel for: " + _lib_path);
        }
    }

    // Free any previously allocated tensors for this session slot
    auto& tensors = _allocated_tensors_per_session[session_idx];
    for (auto tensor : tensors) {
        Free_tensor(tensor);
    }
    tensors.clear();

    // Activate session_idx on THIS thread (evaluator/key routing + stream).
    if (_kms_activate_session) {
        _kms_activate_session(session_idx);
    }

    for (size_t i = 0; i < inputs.size(); ++i) {
        auto t = inputs[i].to(torch::kFloat64);
        auto sizes = t.sizes();
        if (sizes.size() != 4) {
            phase.store(Phase::IDLE);
            throw std::runtime_error("Input must be 4D [N,C,H,W]");
        }
        size_t n = sizes[0], c = sizes[1], h = sizes[2], w = sizes[3];
        const double* data = static_cast<const double*>(t.data_ptr());

        TENSOR* tensor = Alloc_tensor(n, c, h, w, data);
        tensors.push_back(tensor);

        std::string name = get_encode_scheme_c(i)->_name;
        _prepare_input(tensor, name.c_str());
    }
}

bool KERNEL_RUNNER::Execute(int session_idx)
{
    FHE_PROFILE_SCOPE_WITH_MEM("fhe::execute_session");
    if (session_idx < 0 || session_idx >= kMaxSessions)
        throw std::runtime_error("Execute(session): session_idx out of range");
    auto& phase = _phase_per_session[session_idx];
    Phase expected = Phase::PREPARED;
    if (!phase.compare_exchange_strong(expected, Phase::EXECUTED)) {
        throw std::runtime_error("Execute(session): invalid phase transition (expected PREPARED, got " +
                                 std::to_string(static_cast<int>(expected)) + ")");
    }
    if (!_run_kernel) {
        throw std::runtime_error("Kernel not loaded for: " + _lib_path);
    }
    // Re-activate on THIS thread in case Run_session was scheduled to a fresh
    // worker (ensures thread-local idx + stream are set before Main_graph).
    // No-op cost is µs.
    if (_kms_activate_session) {
        _kms_activate_session(session_idx);
    }

    // CUDA Graph is bypassed for B-P (per-session streams are incompatible with
    // graph capture/replay, which is bound to one stream).
    bool result = _run_kernel();
    return result;
}

double* KERNEL_RUNNER::Get_output(int session_idx, const std::string& output_name)
{
    FHE_PROFILE_SCOPE_WITH_MEM("fhe::get_output_session");
    if (session_idx < 0 || session_idx >= kMaxSessions)
        throw std::runtime_error("Get_output(session): session_idx out of range");
    std::lock_guard<std::mutex> lock(_io_mutex_per_session[session_idx]);
    auto& phase = _phase_per_session[session_idx];

    Phase expected = Phase::EXECUTED;
    if (!phase.compare_exchange_strong(expected, Phase::IDLE)) {
        throw std::runtime_error("Get_output(session): invalid phase transition (expected EXECUTED, got " +
                                 std::to_string(static_cast<int>(expected)) + ")");
    }

    if (_kms_activate_session) {
        _kms_activate_session(session_idx);
    }
    double* result = _handle_output(output_name.c_str());

    auto& tensors = _allocated_tensors_per_session[session_idx];
    for (auto tensor : tensors) {
        Free_tensor(tensor);
    }
    tensors.clear();

    return result;
}

double* KERNEL_RUNNER::Run_session(int session_idx, const std::vector<torch::Tensor>& inputs,
                                   const std::string& output_name)
{
    FHE_PROFILE_SCOPE_WITH_MEM("fhe::run_session");
    if (session_idx < 0 || session_idx >= kMaxSessions)
        throw std::runtime_error("Run_session: session_idx out of range");

    // All three phases run on THIS calling thread; each activates session_idx
    // (thread-local), so ops route to this session's evaluator+keys+stream.
    Prepare_input_only(session_idx, inputs);
    try {
        Execute(session_idx);
        return Get_output(session_idx, output_name);
    }
    catch (...) {
        // Execute/Get_output 异常时 phase 卡在 EXECUTED/PREPARED，下次同 slot
        // 请求会 surface 成 "expected IDLE, got 2" 卡死 slot。强制回 IDLE 让
        // slot 自愈（根因是底层 CUDA race，由 L2 phantom Get_input_data copy
        // 消数据层腐败；此处只兜底 phase 状态机不残留）。
        _phase_per_session[session_idx].store(Phase::IDLE);
        throw;
    }
}

std::string KERNEL_RUNNER::Run_cipher(int session_idx, const std::vector<std::string>& input_bytes,
                                      const std::string& output_name)
{
    FHE_PROFILE_SCOPE_WITH_MEM("fhe::run_cipher");
    if (session_idx < 0 || session_idx >= kMaxSessions)
        throw std::runtime_error("Run_cipher: session_idx out of range");
    if (!_kms_run_cipher)
        throw std::runtime_error("Run_cipher: backend does not export Kms_run_cipher");
    if (!_run_kernel)
        throw std::runtime_error("Run_cipher: kernel not loaded");

    // Lock-only serialization (run_cipher is monolithic; no PREPARED/EXECUTED
    // phase — that's Run_session's split-API concern). The per-session mutex
    // also guards the backend's Io_set_input/Io_get_output on this slot.
    std::lock_guard<std::mutex> lock(_io_mutex_per_session[session_idx]);

    // Build parallel C arrays: names from the compiled input scheme; bufs/sizes
    // from the caller's bytes. Names must match what the kernel's
    // Get_input_data(name, idx) expects — same get_encode_scheme_c(i)->_name
    // that Prepare_input uses, so the Io key lines up.
    size_t n = input_bytes.size();
    if (n == 0)
        throw std::runtime_error("Run_cipher: no inputs");
    std::vector<const char*> input_names;
    std::vector<const char*> input_bufs;
    std::vector<size_t> input_sizes;
    input_names.reserve(n);
    input_bufs.reserve(n);
    input_sizes.reserve(n);
    for (size_t i = 0; i < n; ++i) {
        auto* sch = get_encode_scheme_c(static_cast<int>(i));
        if (sch == nullptr)
            throw std::runtime_error("Run_cipher: input scheme " + std::to_string(i) + " missing");
        input_names.push_back(sch->_name);
        // .data() carries the full byte payload (may contain '\0' — do NOT use
        // .c_str()); .size() is the byte length.
        input_bufs.push_back(input_bytes[i].data());
        input_sizes.push_back(input_bytes[i].size());
    }

    // Activate session on this thread (cheap; the backend re-activates too —
    // idempotent). Io routes to this session's per-session store.
    if (_kms_activate_session)
        _kms_activate_session(session_idx);

    // Two-pass output: pass 1 queries the needed size (out_buf=null, cap=0);
    // pass 2 allocates and fills. Avoids any backend malloc crossing the ABI.
    long need = _kms_run_cipher(session_idx, _run_kernel, input_names.data(), input_bufs.data(), input_sizes.data(), n,
                                /*out_buf=*/nullptr, /*out_cap=*/0, output_name.c_str());
    if (need < 0)
        throw std::runtime_error("Run_cipher: backend eval failed (pass 1)");
    if (need == 0)
        throw std::runtime_error("Run_cipher: backend returned 0 output size");
    std::string out(static_cast<size_t>(need), '\0');
    long got = _kms_run_cipher(session_idx, _run_kernel, input_names.data(), input_bufs.data(), input_sizes.data(), n,
                               &out[0], static_cast<size_t>(need), output_name.c_str());
    if (got < 0)
        throw std::runtime_error("Run_cipher: backend eval failed (pass 2)");
    if (static_cast<size_t>(got) > out.size()) {
        // backend asked for more than it reported in pass 1 — truncate is wrong;
        // surface as error.
        throw std::runtime_error("Run_cipher: backend size grew between passes");
    }
    out.resize(static_cast<size_t>(got));
    return out;
}

std::vector<double> KERNEL_RUNNER::LastRunCipherTiming()
{
    // Returns the last RunCipher's per-stage seconds on THIS thread:
    // [deser_sync, launder_sync, kernel, serialize]. Call on the SAME worker
    // thread that ran Run_cipher (the timing is thread-local in the backend).
    if (!_kms_last_runcipher_timing) {
        throw std::runtime_error(
            "KERNEL_RUNNER::LastRunCipherTiming: backend does not export Kms_last_runcipher_timing");
    }
    double t[4] = {0, 0, 0, 0};
    if (_kms_last_runcipher_timing(t) != 0) {
        throw std::runtime_error("KERNEL_RUNNER::LastRunCipherTiming: getter failed");
    }
    return {t[0], t[1], t[2], t[3]};
}

}  // namespace runtime
}  // namespace ace
