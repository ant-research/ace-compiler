//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "ace/common/logging.h"
#include "ace/runtime/batch_runner.h"
#include "ace/runtime/config_manager.h"
#include "ace/runtime/config_parser.h"
#include "ace/runtime/provider_manager.h"
#include "ace/runtime/validator.h"
#include "common/rt_runtime_config.h"
#include "fhe/kms/key_mgr.h"  // fhert::KeyMode

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cuda_runtime.h>
#include <memory>
#include <pybind11/pybind11.h>
#include <thread>
#include <torch/extension.h>

// I/O transport configuration bindings
#include "fhe/io/cufile_api.h"
#include "fhe/io/io_bulk_reader.h"
#include "fhe/io/io_policy.h"
#include "fhe/io/transport.h"

namespace py = pybind11;
using namespace pybind11::literals;

namespace
{

void Set_fhe_config(const py::dict& config_dict)
{
    ace::runtime::Parse_and_set_config(config_dict);
}

std::string Get_config_summary()
{
    return ace::runtime::CONFIG_MANAGER::Instance().Get_config().To_string();
}

bool Validate_config()
{
    const auto config = ace::runtime::CONFIG_MANAGER::Instance().Get_config();
    return config.Is_valid();
}

std::string Get_validation_errors()
{
    const auto config = ace::runtime::CONFIG_MANAGER::Instance().Get_config();
    return config.Validate();
}

}  // anonymous namespace

// === IO benchmark shared types and helpers ===
struct BenchDiskGroup
{
    std::string disk_path;
    std::vector<std::string> files;
    size_t total_bytes = 0;
};

struct BenchGpuBuf
{
    void* ptr = nullptr;
    explicit BenchGpuBuf(size_t size)
    {
        if (cudaMalloc(&ptr, size) != cudaSuccess)
            ptr = nullptr;
    }
    ~BenchGpuBuf()
    {
        if (ptr)
            cudaFree(ptr);
    }
    BenchGpuBuf(const BenchGpuBuf&) = delete;
    BenchGpuBuf& operator=(const BenchGpuBuf&) = delete;
    operator bool() const { return ptr != nullptr; }
};

inline std::pair<std::vector<BenchDiskGroup>, size_t>
bench_build_disk_groups(const std::string& path, size_t data_size, int num_files,
                        const std::vector<std::string>& disk_paths, bool mkdir)
{
    int n_disks = std::max<int>(1, static_cast<int>(disk_paths.size()));
    int actual_files = std::max(num_files, 1);
    size_t per_file_size = (actual_files <= 1) ? data_size : (data_size / actual_files);
    if (per_file_size == 0)
        per_file_size = data_size;

    std::vector<BenchDiskGroup> groups(n_disks);
    for (int d = 0; d < n_disks; d++) {
        groups[d].disk_path = disk_paths.empty() ? "" : disk_paths[d % disk_paths.size()];
    }

    if (num_files <= 1) {
        per_file_size = data_size;
        for (int d = 0; d < n_disks; d++) {
            std::string dir = groups[d].disk_path + "/io_bench";
            if (mkdir)
                system(("mkdir -p " + dir).c_str());
            std::string fp = dir + "/" + path.substr(path.find_last_of('/') + 1) + "_disk" + std::to_string(d);
            groups[d].files.push_back(fp);
            groups[d].total_bytes = per_file_size;
        }
    }
    else {
        for (int i = 0; i < actual_files; i++) {
            int d = i % n_disks;
            std::string dir = groups[d].disk_path + "/io_bench";
            if (mkdir)
                system(("mkdir -p " + dir).c_str());
            std::string fp = dir + "/" + path.substr(path.find_last_of('/') + 1) + "_" + std::to_string(i);
            groups[d].files.push_back(fp);
            groups[d].total_bytes += per_file_size;
        }
    }
    return {groups, per_file_size};
}

inline void bench_fill_random_gpu(void* gpu_ptr, size_t size)
{
    const size_t blk = 4 * 1024;
    std::vector<uint8_t> host_buf(size);
    uint32_t rng_state = 0xDEADBEEFu;
    for (size_t off = 0; off < size; off += blk) {
        size_t n = std::min(blk, size - off);
        for (size_t i = 0; i < n; i += 4) {
            rng_state = rng_state * 1103515245u + 12345u;
            uint32_t v = rng_state;
            v ^= static_cast<uint32_t>(off & 0xFFFFFFFFu);
            host_buf[off + i + 0] = static_cast<uint8_t>(v & 0xFF);
            host_buf[off + i + 1] = static_cast<uint8_t>((v >> 8) & 0xFF);
            host_buf[off + i + 2] = static_cast<uint8_t>((v >> 16) & 0xFF);
            host_buf[off + i + 3] = static_cast<uint8_t>((v >> 24) & 0xFF);
        }
    }
    cudaMemcpy(gpu_ptr, host_buf.data(), size, cudaMemcpyHostToDevice);
}

inline std::vector<uint8_t> bench_make_golden_bytes()
{
    const size_t blk = 4 * 1024;
    const size_t golden_size = 256;
    std::vector<uint8_t> golden(golden_size);
    uint32_t rng_state = 0xDEADBEEFu;
    size_t off = 0;
    for (size_t i = 0; i < golden_size; i += 4) {
        rng_state = rng_state * 1103515245u + 12345u;
        uint32_t v = rng_state;
        v ^= static_cast<uint32_t>(off & 0xFFFFFFFFu);
        golden[i + 0] = static_cast<uint8_t>(v & 0xFF);
        golden[i + 1] = static_cast<uint8_t>((v >> 8) & 0xFF);
        golden[i + 2] = static_cast<uint8_t>((v >> 16) & 0xFF);
        golden[i + 3] = static_cast<uint8_t>((v >> 24) & 0xFF);
    }
    return golden;
}

PYBIND11_MODULE(runtime, m)
{
    m.doc() = "ACE Runtime Extension";

    // Initialize spdlog with Python sink on module import
    ace::common::Init_logging("ace.runtime");

    m.def("set_config", &Set_fhe_config, "Set FHE runtime configuration from Python dict");

    m.def("config_summary", &Get_config_summary, "Get config summary string");
    m.def("validate_config", &Validate_config, "Validate current config");
    m.def("config_errors", &Get_validation_errors, "Get config validation error messages");

    m.def("set_log_level", &ace::common::Set_log_level,
          "Set C++ runtime log level (TRACE, DEBUG, INFO, WARN, ERROR, CRITICAL, OFF)");

    // Key path management for pre-generated keys (KMS)
    // Both CONFIG_MANAGER (C++) and rtlib C storage must be kept in sync:
    // - CONFIG_MANAGER stores the path for get_key_path_c() callback
    // - Set_key_path() stores it for fallback Get_key_path() in libFHErt_common
    m.def(
        "set_key_path",
        [](const std::string& path) {
            ace::runtime::CONFIG_MANAGER::Instance().Set_key_path(path);
            // Also set the C-level storage so Get_key_path() fallback works
            // when the callback hasn't been registered yet
            ::Set_key_path(path.c_str());
        },
        py::arg("path"), "Set key bundle path for pre-generated keys. Pass empty string to clear.");

    m.def(
        "get_key_path",
        []() -> py::object {
            std::string path = ace::runtime::CONFIG_MANAGER::Instance().Get_key_path();
            if (path.empty()) {
                return py::none();
            }
            return py::str(path);
        },
        "Get current key bundle path, or None if not set.");

    m.def(
        "clear_key_path",
        []() {
            ace::runtime::CONFIG_MANAGER::Instance().Set_key_path("");
            ::Clear_key_path();
        },
        "Clear key bundle path (keys will be generated at runtime).");

    // Bind KeyMode enum
    py::enum_<fhert::KeyMode>(m, "KeyMode")
        .value("online", fhert::KeyMode::ONLINE, "Generate all keys on GPU (default)")
        .value("preload", fhert::KeyMode::PRELOAD, "Load eval keys (RLK+GK) from disk at init (compute-only)")
        .value("on_demand", fhert::KeyMode::ON_DEMAND, "Load eval keys on-demand at runtime (reserved)")
        .value("preload_full", fhert::KeyMode::PRELOAD_FULL,
               "Load all keys (SK+PK+RLK+GK) from disk at init (can verify)")
        .value("on_demand_full", fhert::KeyMode::ON_DEMAND_FULL, "Load all keys on-demand at runtime (reserved)")
        .value("key_offline", fhert::KeyMode::PRELOAD_FULL, "Alias for preload_full")
        .value("server_only", fhert::KeyMode::PRELOAD, "Alias for preload (compute-only)");

    m.def(
        "set_key_mode", [](int mode) { ::Set_key_mode(mode); }, py::arg("mode"),
        "Set key mode (0=ONLINE, 1=PRELOAD, 2=ON_DEMAND, 3=PRELOAD_FULL, 4=ON_DEMAND_FULL). "
        "Must be called before init().");

    m.def(
        "get_key_mode", []() -> int { return ::Get_key_mode(); }, "Get the current key mode.");

    py::enum_<fhert::EncPolicy>(m, "EncPolicy")
        .value("load_raw", fhert::EncPolicy::LOAD_RAW, "Read weight file into memory at init, no encoding (default)")
        .value("load_encoded", fhert::EncPolicy::LOAD_ENCODED,
               "Read file into memory and encode raw (msg) weights at init")
        .value("load_on_demand", fhert::EncPolicy::LOAD_ON_DEMAND,
               "Read per weight at inference (reserved — not implemented)");

    m.def(
        "set_enc_policy", [](int policy) { ::Set_enc_policy(policy); }, py::arg("policy"),
        "Set enc policy (0=LOAD_RAW, 1=LOAD_ENCODED, 2=LOAD_ON_DEMAND). "
        "Must be called before init().");

    m.def(
        "get_enc_policy", []() -> int { return ::Get_enc_policy(); }, "Get the current enc policy.");

    m.def("validate_result", &ace::runtime::Validate_result, py::arg("result"), py::arg("expected"),
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
        .def("register_kernel", &ace::runtime::PROVIDER_MANAGER::Register_provider, py::arg("name"),
             py::arg("lib_path"), py::arg("use_cuda_graph") = false)
        .def("init", &ace::runtime::PROVIDER_MANAGER::Init, py::arg("name"))
        .def(
            "execute",
            [](ace::runtime::PROVIDER_MANAGER& self, const std::string& name,
               const std::vector<torch::Tensor>& inputs) { return self.Run(name, inputs, "output"); },
            py::arg("name"), py::arg("inputs"), "Run single FHE inference")
        .def(
            "execute_batch",
            [](ace::runtime::PROVIDER_MANAGER& self, const std::string& name,
               const std::vector<std::vector<torch::Tensor>>& batch_inputs, bool parallel, int num_threads,
               bool verbose) {
                if (parallel) {
                    return self.Run_batch_parallel(name, batch_inputs, "output", num_threads, verbose);
                }
                else {
                    return self.Run_batch(name, batch_inputs, "output", verbose);
                }
            },
            py::arg("name"), py::arg("batch_inputs"), py::arg("parallel") = false, py::arg("num_threads") = 0,
            py::arg("verbose") = false, "Run batch FHE inference (sequential or parallel with OpenMP)")
        .def("finalize", &ace::runtime::PROVIDER_MANAGER::Finalize, py::arg("name"))
        .def("capture_graph", &ace::runtime::PROVIDER_MANAGER::Capture_graph, py::arg("name"),
             "Attempt CUDA Graph capture for Execute() replay")
        // KMS key management — forwarded to the provider's KERNEL_RUNNER.
        // Requires Init() first (active FheContext). Replaces the retired
        // Python ctypes FHert_* path; see fhe/kms/kms_runtime_api.h.
        .def("save_keys", &ace::runtime::PROVIDER_MANAGER::Save_keys, py::arg("name"), py::arg("key_dir"),
             "Save all keys (SK+PK+RLK+GK) to key_dir; returns 0 on success, -1 on failure")
        .def("save_eval_keys", &ace::runtime::PROVIDER_MANAGER::Save_eval_keys, py::arg("name"), py::arg("key_dir"),
             "Save eval keys (RLK+GK) only; returns 0 on success, -1 on failure")
        .def("load_keys", &ace::runtime::PROVIDER_MANAGER::Load_keys, py::arg("name"), py::arg("key_dir"),
             "Load all keys from key_dir; returns 0 on success, -1 on failure")
        .def("load_eval_keys", &ace::runtime::PROVIDER_MANAGER::Load_eval_keys, py::arg("name"), py::arg("key_dir"),
             "Load eval keys (RLK+GK) only; returns 0 on success, -1 on failure")
        .def("keys_loaded_from_kms", &ace::runtime::PROVIDER_MANAGER::Keys_loaded_from_kms, py::arg("name"),
             "Return 1 if keys were loaded from KMS, 0 if generated at runtime")
        .def("get_galois_load_time_seconds", &ace::runtime::PROVIDER_MANAGER::Get_galois_load_time_seconds,
             py::arg("name"), "Wall-clock seconds of the most recent galois-key load (-1.0 if none)")
        // Mode B (multi-user): one shared context + N key bundles, switch by index.
        .def("create_session", &ace::runtime::PROVIDER_MANAGER::Create_session, py::arg("name"), py::arg("key_dir"),
             "Create an extra session sharing the active context, loading a full key bundle from key_dir; "
             "returns session index >=1 on success, -1 on failure")
        .def("switch_session", &ace::runtime::PROVIDER_MANAGER::Switch_session, py::arg("name"), py::arg("idx"),
             "Switch active session by index (0=primary, >=1=extra); returns 0 on success, -1 on failure")
        .def("release_session", &ace::runtime::PROVIDER_MANAGER::Release_session, py::arg("name"), py::arg("idx"),
             "Release all galois-key slots for session idx (free HBM); session structure stays intact; "
             "Ensure re-loads from disk on next use; returns 0 on success, -1 on failure")
        .def("num_sessions", &ace::runtime::PROVIDER_MANAGER::Num_sessions, py::arg("name"),
             "Return the number of sessions currently resident (primary + extra); -1 if not available")
        .def(
            "execute_session",
            [](ace::runtime::PROVIDER_MANAGER& self, const std::string& name, int session_idx,
               const std::vector<torch::Tensor>& inputs) {
                return self.Run_session(name, session_idx, inputs, "output");
            },
            py::arg("name"), py::arg("session_idx"), py::arg("inputs"),
            "Mode B-P: run inference bound to a specific session on the calling thread "
            "(concurrency-safe across sessions; GIL released during native execution)")
        .def(
            "run_cipher",
            [](ace::runtime::PROVIDER_MANAGER& self, const std::string& name, int session_idx,
               const std::vector<std::string>& input_bytes, const std::string& output_name) {
                std::string out = self.Run_cipher(name, session_idx, input_bytes, output_name);
                return py::bytes(out);
            },
            py::arg("name"), py::arg("session_idx"), py::arg("input_bytes"), py::arg("output_name") = "output",
            "Mode B-P + ciphertext-in/out: accept serialized input ciphertext bytes (one per "
            "input, ordered to match the compiled input scheme), eval with the session's keys "
            "(no encrypt/decrypt), return serialized output ciphertext bytes. GIL released.")
        .def(
            "last_runcipher_timing",
            [](ace::runtime::PROVIDER_MANAGER& self, const std::string& name) {
                return self.LastRunCipherTiming(name);  // std::vector<double> -> Python list
            },
            py::arg("name"),
            "Per-stage seconds [deser_sync, launder_sync, kernel(eval), serialize] of the last "
            "run_cipher on THIS thread. Call on the SAME worker thread that ran run_cipher.");

    // Bind IoPolicy — the primary IO transport path selector.
    py::enum_<rtio::IoPolicy>(m, "IoPolicy")
        .value("auto", rtio::IoPolicy::auto_, "Auto-detect: GDS if cuFile available, else staged")
        .value("gds", rtio::IoPolicy::gds, "GDS (cuFile DMA)")
        .value("staged", rtio::IoPolicy::staged, "Host staged: fread + cudaMemcpy")
        .value("mapped", rtio::IoPolicy::mapped, "Host mapped: mmap + cudaMemcpy");

    // Bind IoLoc — data location hint (Device for GPU, Host for CPU)
    py::enum_<rtio::IoLoc>(m, "IoLoc")
        .value("Device", rtio::IoLoc::Device, "Data in GPU memory (GDS eligible)")
        .value("Host", rtio::IoLoc::Host, "Data in host/CPU memory (GDS never used)");

    // Global IO policy — primary configuration knob
    m.def("set_io_policy", &rtio::set_global_io_policy, py::arg("policy"), "Set global IO policy (primary knob)");
    m.def("get_io_policy", &rtio::get_global_io_policy, "Get current global IO policy");

    // Convenience: set policy + disk_paths together
    m.def(
        "set_io_policy_with_disk_paths",
        [](rtio::IoPolicy policy, const std::vector<std::string>& disk_paths) {
            rtio::set_global_io_policy(policy);
            rtio::IoConfig cfg = rtio::get_global_io_config();
            cfg.disk_paths = disk_paths;
            rtio::set_global_io_config(cfg);
        },
        py::arg("policy"), py::arg("disk_paths"), "Set global IO policy and multi-disk paths together");

    // Expert override bindings — prefer IoPolicy for normal use
    py::class_<rtio::IoConfig>(m, "IoConfig")
        .def(py::init<>())
        .def_readwrite("fallback_on_error", &rtio::IoConfig::fallback_on_error,
                       "If true, GDS errors silently fall back to staged I/O")
        .def_readwrite("chunk_size", &rtio::IoConfig::chunk_size,
                       "I/O chunk size in bytes (8 MiB optimal for GDS DMA throughput)")
        .def_readwrite("num_io_threads", &rtio::IoConfig::num_io_threads,
                       "Number of parallel I/O threads for GDS (1=single-threaded, 4-8 recommended)")
        .def_readwrite("disk_paths", &rtio::IoConfig::disk_paths,
                       "Multi-disk paths for per-key galois key distribution (empty=single-disk)")
        .def_readwrite("galois_merged_format", &rtio::IoConfig::galois_merged_format,
                       "Save galois keys as one merged file (true) or per-key files (false)")
        .def_readwrite("exec_mode", &rtio::IoConfig::exec_mode,
                       "Execution mode: sync (blocking), reserved for future async/pipeline");

    // Bind IoExecMode — reserved for future async/pipeline IO support
    py::enum_<rtio::IoExecMode>(m, "IoExecMode")
        .value("sync", rtio::IoExecMode::sync, "Synchronous blocking IO (current default)");

    m.def(
        "gds_available", []() -> bool { return rtio::GdsDriver::instance().available(); },
        "Query whether cuFile/GDS is available at runtime");

    // Expert: global IoConfig override — prefer set_io_policy for normal use
    m.def(
        "set_io_config", [](const rtio::IoConfig& cfg) { rtio::set_global_io_config(cfg); }, py::arg("config"),
        "Set global I/O transport configuration (expert override)");

    m.def(
        "get_io_config", []() -> rtio::IoConfig { return rtio::get_global_io_config(); },
        "Get current global I/O transport configuration");

    // =======================================================================
    // IO benchmark roundtrip — raw GPU buffer write-read via IoPolicy
    // =======================================================================
    // Python sets global IO config via IoPolicySettings.apply() or
    // IoConfig.apply(), then calls this function with only path/data_size/
    // num_files.  C++ reads get_global_io_policy() + get_global_io_config()
    // internally, ensuring the benchmark exercises the exact same code path
    // as business code.

    // ================================================================
    // bench_io_write_prep: write benchmark files using staged I/O only.
    // Files remain on disk for subsequent bench_io_read calls.
    // ================================================================
    m.def(
        "bench_io_write_prep",
        [](const std::string& path, size_t data_size, int num_files,
           const std::vector<std::string>& disk_paths) -> py::dict {
            using namespace std::chrono;
            const size_t GiB = 1024ULL * 1024 * 1024;

            std::vector<BenchDiskGroup> groups;
            size_t per_file_size = 0;
            std::tie(groups, per_file_size) = bench_build_disk_groups(path, data_size, num_files, disk_paths,
                                                                      /*mkdir=*/true);
            int n_disks = static_cast<int>(groups.size());

            // Always use staged write (safest path) regardless of global config.
            rtio::IoPolicy write_policy = rtio::IoPolicy::staged;

            BenchGpuBuf write_buf(per_file_size);
            if (!write_buf) {
                throw std::runtime_error("bench_io_write_prep: cudaMalloc failed");
            }
            bench_fill_random_gpu(write_buf.ptr, per_file_size);

            auto t0 = high_resolution_clock::now();

            if (n_disks <= 1) {
                for (auto& grp : groups) {
                    for (auto& fp : grp.files) {
                        auto writer = rtio::open_writer(fp, rtio::IoLoc::Device, write_policy);
                        if (!writer || !writer->write_gpu(write_buf.ptr, 0, per_file_size)) {
                            throw std::runtime_error("bench_io_write_prep: write failed for " + fp);
                        }
                    }
                }
            }
            else {
                std::atomic<bool> failed{false};
                std::vector<std::thread> threads;
                for (size_t gi = 0; gi < groups.size(); gi++) {
                    threads.emplace_back(
                        [grp = groups[gi], &write_buf, fps = per_file_size, wp = write_policy, &failed]() {
                            cudaSetDevice(0);  // ensure CUDA context in this thread
                            for (auto& fp : grp.files) {
                                if (failed.load())
                                    return;
                                auto writer = rtio::open_writer(fp, rtio::IoLoc::Device, wp);
                                if (!writer || !writer->write_gpu(write_buf.ptr, 0, fps)) {
                                    failed.store(true);
                                    return;
                                }
                            }
                        });
                }
                for (auto& t : threads)
                    t.join();
                if (failed.load()) {
                    throw std::runtime_error("bench_io_write_prep: parallel write failed");
                }
            }

            auto t1 = high_resolution_clock::now();
            double write_ms = duration<double, std::milli>(t1 - t0).count();
            int actual_files_val = std::max(num_files, 1);
            double total_gib = (actual_files_val <= 1) ? static_cast<double>(data_size) * n_disks / GiB
                                                       : static_cast<double>(data_size) / GiB;

            return py::dict("write_ms"_a = write_ms,
                            "write_gib_s"_a = (write_ms > 0) ? total_gib / (write_ms / 1000.0) : 0.0,
                            "transport_name"_a = "staged", "data_size_gib"_a = total_gib, "num_files"_a = num_files,
                            "num_disks"_a = n_disks, "gds_available"_a = rtio::GdsDriver::instance().available());
        },
        py::arg("path"), py::arg("data_size") = 10ULL * 1024 * 1024 * 1024, py::arg("num_files") = 1,
        py::arg("disk_paths") = std::vector<std::string>{},
        "Write benchmark data files via staged I/O (data prep for bench_io_read)");

    // ================================================================
    // bench_io_read: read-only benchmark using global IoPolicy/IoConfig.
    // Assumes files already exist on disk (created by bench_io_write_prep).
    // Does NOT delete files after reading.
    // Uses IoBulkReader for parallel multi-disk reads.
    // ================================================================
    m.def(
        "bench_io_read",
        [](const std::string& path, size_t data_size, int num_files,
           const std::vector<std::string>& disk_paths) -> py::dict {
            using namespace std::chrono;
            const size_t GiB = 1024ULL * 1024 * 1024;

            std::vector<BenchDiskGroup> groups;
            size_t per_file_size = 0;
            std::tie(groups, per_file_size) = bench_build_disk_groups(path, data_size, num_files, disk_paths,
                                                                      /*mkdir=*/false);
            int n_disks = static_cast<int>(groups.size());
            int actual_files_val = std::max(num_files, 1);
            rtio::IoPolicy policy = rtio::get_global_io_policy();

            // Build file list and allocate per-file buffers for IoBulkReader
            std::vector<rtio::BulkReadFile> bulk_files;
            std::vector<void*> bulk_buffers;
            // First buffer pointer for verification
            void* first_buf = nullptr;

            for (int d = 0; d < n_disks; d++) {
                for (const auto& fp : groups[d].files) {
                    bulk_files.push_back({fp, per_file_size});
                    void* buf;
                    if (cudaMalloc(&buf, per_file_size) != cudaSuccess) {
                        throw std::runtime_error("bench_io_read: cudaMalloc for read failed");
                    }
                    bulk_buffers.push_back(buf);
                    if (!first_buf)
                        first_buf = buf;
                }
            }

            // Use IoBulkReader for parallel reads
            rtio::IoBulkReader reader(policy, disk_paths, /*num_io_threads=*/1);
            auto result = reader.read(bulk_files, bulk_buffers);

            double read_ms = result.elapsed_seconds * 1000.0;
            // When num_files <= 1, each disk reads data_size bytes (total = data_size * n_disks).
            // When num_files > 1, files are round-robin distributed and total read = data_size.
            double total_gib = (actual_files_val <= 1) ? static_cast<double>(data_size) * n_disks / GiB
                                                       : static_cast<double>(data_size) / GiB;

            // Verify: compare first 256 bytes of first read buffer against
            // deterministic golden bytes from the same LCG used in bench_fill_random_gpu.
            bool verified = true;
            auto golden = bench_make_golden_bytes();
            {
                std::vector<uint8_t> host_check(256);
                if (cudaMemcpy(host_check.data(), first_buf, host_check.size(), cudaMemcpyDeviceToHost) !=
                    cudaSuccess) {
                    verified = false;
                }
                else if (memcmp(host_check.data(), golden.data(), host_check.size()) != 0) {
                    verified = false;
                }
            }

            // Free read buffers
            for (auto buf : bulk_buffers) {
                if (buf)
                    cudaFree(buf);
            }

            if (!result.success) {
                throw std::runtime_error("bench_io_read: read_gpu failed");
            }

            // Do NOT delete files — caller manages cleanup via bench_io_cleanup.

            return py::dict("read_ms"_a = read_ms,
                            "read_gib_s"_a = (read_ms > 0) ? total_gib / (read_ms / 1000.0) : 0.0,
                            "transport_name"_a = result.transport_name, "data_size_gib"_a = total_gib,
                            "num_files"_a = num_files, "num_disks"_a = n_disks, "verified"_a = verified,
                            "gds_available"_a = rtio::GdsDriver::instance().available());
        },
        py::arg("path"), py::arg("data_size") = 10ULL * 1024 * 1024 * 1024, py::arg("num_files") = 1,
        py::arg("disk_paths") = std::vector<std::string>{},
        "Read-only IO benchmark using global IoPolicy (files must already exist)");

    // ================================================================
    // bench_io_cleanup: remove benchmark files from disk.
    // ================================================================
    m.def(
        "bench_io_cleanup",
        [](const std::string& path, int num_files, const std::vector<std::string>& disk_paths) {
            std::vector<BenchDiskGroup> groups;
            size_t per_file_size_unused = 0;
            std::tie(groups, per_file_size_unused) = bench_build_disk_groups(path, 0, num_files, disk_paths,
                                                                             /*mkdir=*/false);
            for (auto& grp : groups) {
                for (auto& fp : grp.files)
                    std::remove(fp.c_str());
            }
        },
        py::arg("path"), py::arg("num_files") = 1, py::arg("disk_paths") = std::vector<std::string>{},
        "Remove benchmark files created by bench_io_write_prep");

    // ================================================================
    // bench_io_roundtrip: full write + read + verify + cleanup
    // (kept for backward compatibility and correctness testing)
    // ================================================================
    m.def(
        "bench_io_roundtrip",
        [](const std::string& path, size_t data_size, int num_files,
           const std::vector<std::string>& disk_paths) -> py::dict {
            using namespace std::chrono;

            const size_t GiB = 1024ULL * 1024 * 1024;
            rtio::IoPolicy policy = rtio::get_global_io_policy();

            auto [groups_, per_file_size_] = bench_build_disk_groups(path, data_size, num_files, disk_paths,
                                                                     /*mkdir=*/true);
            // Structured bindings cannot be captured by nested lambdas;
            // assign to ordinary local variables.
            auto groups = groups_;
            size_t per_file_size = per_file_size_;
            int n_disks = static_cast<int>(groups.size());

            // === Write phase ===
            BenchGpuBuf write_buf(per_file_size);
            if (!write_buf) {
                throw std::runtime_error("bench_io_roundtrip: cudaMalloc for write failed");
            }
            bench_fill_random_gpu(write_buf.ptr, per_file_size);

            auto t_write_start = high_resolution_clock::now();

            if (n_disks <= 1) {
                // Serial: single disk, all files sequentially
                for (auto& grp : groups) {
                    for (auto& fp : grp.files) {
                        auto writer = rtio::open_writer(fp, rtio::IoLoc::Device, policy);
                        if (!writer || !writer->write_gpu(write_buf.ptr, 0, per_file_size)) {
                            throw std::runtime_error("bench_io_roundtrip: write_gpu failed for " + fp);
                        }
                    }
                }
            }
            else {
                // Parallel: one thread per disk, each writes its files sequentially
                std::atomic<bool> write_failed{false};
                std::vector<std::thread> write_threads;
                for (size_t gi = 0; gi < groups.size(); gi++) {
                    write_threads.emplace_back(
                        [grp = groups[gi], &write_buf, fps = per_file_size, pol = policy, &write_failed]() {
                            cudaSetDevice(0);
                            for (auto& fp : grp.files) {
                                if (write_failed.load())
                                    return;
                                auto writer = rtio::open_writer(fp, rtio::IoLoc::Device, pol);
                                if (!writer || !writer->write_gpu(write_buf.ptr, 0, fps)) {
                                    write_failed.store(true);
                                    return;
                                }
                            }
                        });
                }
                for (auto& t : write_threads)
                    t.join();
                if (write_failed.load()) {
                    throw std::runtime_error("bench_io_roundtrip: parallel write_gpu failed");
                }
            }

            auto t_write_end = high_resolution_clock::now();
            double write_ms = duration<double, std::milli>(t_write_end - t_write_start).count();

            // Save golden bytes for verification (before freeing write buffer)
            std::vector<uint8_t> golden(256);
            cudaMemcpy(golden.data(), write_buf.ptr, golden.size(), cudaMemcpyDeviceToHost);

            // Free write buffer early
            write_buf.~BenchGpuBuf();

            // === Read phase ===
            // Build file list and allocate per-file buffers for IoBulkReader
            std::vector<rtio::BulkReadFile> bulk_files;
            std::vector<void*> bulk_buffers;
            void* first_read_buf = nullptr;

            for (int d = 0; d < n_disks; d++) {
                for (const auto& fp : groups[d].files) {
                    bulk_files.push_back({fp, per_file_size});
                    void* buf;
                    if (cudaMalloc(&buf, per_file_size) != cudaSuccess) {
                        throw std::runtime_error("bench_io_roundtrip: cudaMalloc for read failed");
                    }
                    bulk_buffers.push_back(buf);
                    if (!first_read_buf)
                        first_read_buf = buf;
                }
            }

            auto t_read_start = high_resolution_clock::now();

            rtio::IoBulkReader bulk_reader(policy, disk_paths, /*num_io_threads=*/1);
            auto read_result = bulk_reader.read(bulk_files, bulk_buffers);

            auto t_read_end = high_resolution_clock::now();
            double read_ms = duration<double, std::milli>(t_read_end - t_read_start).count();

            // === Verify ===
            bool verified = true;
            {
                std::vector<uint8_t> host_check(256);
                if (cudaMemcpy(host_check.data(), first_read_buf, host_check.size(), cudaMemcpyDeviceToHost) !=
                    cudaSuccess) {
                    verified = false;
                }
                else if (memcmp(host_check.data(), golden.data(), host_check.size()) != 0) {
                    verified = false;
                }
            }

            // Free read buffers
            for (auto buf : bulk_buffers) {
                if (buf)
                    cudaFree(buf);
            }

            // === Cleanup files ===
            for (auto& grp : groups) {
                for (auto& fp : grp.files)
                    std::remove(fp.c_str());
            }

            if (!read_result.success) {
                throw std::runtime_error("bench_io_roundtrip: read_gpu failed");
            }

            // === Build result dict ===
            // When num_files <= 1, each disk reads data_size bytes (total = data_size * n_disks).
            // When num_files > 1, files are round-robin distributed and total read = data_size.
            int actual_files_val = std::max(num_files, 1);
            double total_gib = (actual_files_val <= 1) ? static_cast<double>(data_size) * n_disks / GiB
                                                       : static_cast<double>(data_size) / GiB;

            return py::dict("write_ms"_a = write_ms, "read_ms"_a = read_ms,
                            "write_gib_s"_a = (write_ms > 0) ? total_gib / (write_ms / 1000.0) : 0.0,
                            "read_gib_s"_a = (read_ms > 0) ? total_gib / (read_ms / 1000.0) : 0.0,
                            "transport_name"_a = read_result.transport_name, "data_size_gib"_a = total_gib,
                            "num_files"_a = num_files, "num_disks"_a = n_disks, "verified"_a = verified,
                            "gds_available"_a = rtio::GdsDriver::instance().available());
        },
        py::arg("path"), py::arg("data_size") = 10ULL * 1024 * 1024 * 1024, py::arg("num_files") = 1,
        py::arg("disk_paths") = std::vector<std::string>{},
        "IO write-read roundtrip benchmark via global IoPolicy/IoConfig");
}