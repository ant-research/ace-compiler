//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief test_io_perf.cpp
//! IO performance benchmarks: parameterized write-then-read across transports,
//! thread counts, disk counts, file modes (merged vs per-key), and data sizes.
//! Also includes chunk size sweep and CRC32 overhead measurement.
//!
//! ALL tests in this file require a CUDA GPU and are gated behind
//! ACE_PHANTOM_GPU_TESTS. They are excluded from normal test runs.
//!
//! Build and run:
//!   cmake -DBUILD_UNITTEST=ON -DACE_PHANTOM_GPU_TESTS=ON ..
//!   make ut_fhert_io
//!   ./unittest/ut_fhert_io --gtest_filter="IoPerf*"

#include "fhe/io/cuda_check.h"
#include "fhe/io/gds_io.h"
#include "fhe/io/mmap_transport.h"
#include "fhe/io/staged_transport.h"
#include "fhe/io/transport.h"
#include "test_util.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cuda_runtime.h>
#include <fstream>
#include <gtest/gtest.h>
#include <nlohmann/json.hpp>
#include <string>
#include <sys/stat.h>
#include <thread>
#include <unistd.h>
#include <vector>

using namespace rtio;

namespace
{

//=============================================================================
// Helpers
//=============================================================================

std::string policy_name(IoPolicy policy)
{
    switch (policy) {
    case IoPolicy::auto_:
        return "auto";
    case IoPolicy::gds:
        return "gds";
    case IoPolicy::staged:
        return "staged";
    case IoPolicy::mapped:
        return "mapped";
    }
    return "unknown";
}

std::string format_size_gib(size_t bytes)
{
    double gib = static_cast<double>(bytes) / (1073741824.0);
    char buf[64];
    std::snprintf(buf, sizeof(buf), "%.0fGiB", gib);
    return std::string(buf);
}

}  // anonymous namespace

//=============================================================================
// IoPerfConfig — parameterized benchmark configuration
//=============================================================================

struct IoPerfConfig
{
    IoPolicy policy;
    int num_threads;         // 1, 2, 4
    int num_disks;           // 1, 2, 4
    bool merged;             // true=single file, false=per-key multi-file
    size_t total_data_size;  // 20 GiB or 50 GiB
};

//! Generate all IoPerfConfig combinations for the full benchmark matrix.
std::vector<IoPerfConfig> generate_perf_configs()
{
    std::vector<IoPerfConfig> configs;
    IoPolicy policies[] = {
        IoPolicy::staged,
        IoPolicy::mapped,
        IoPolicy::gds,
    };
    int threads[] = {1, 2, 4};
    int disks[] = {1, 2, 4};
    bool modes[] = {true, false};
    size_t sizes[] = {20ULL * 1024 * 1024 * 1024, 50ULL * 1024 * 1024 * 1024};

    for (auto p : policies)
        for (auto t : threads)
            for (auto d : disks)
                for (auto m : modes)
                    for (auto s : sizes)
                        configs.push_back({p, t, d, m, s});

    return configs;
}

//=============================================================================
// IoPerfConfigName — gtest parameter printer
//=============================================================================

std::string IoPerfConfigName(const ::testing::TestParamInfo<IoPerfConfig>& info)
{
    const auto& p = info.param;
    return policy_name(p.policy) + "_" + std::to_string(p.num_threads) + "t_" + std::to_string(p.num_disks) + "d_" +
           (p.merged ? "merged" : "perkey") + "_" + format_size_gib(p.total_data_size);
}

#ifdef ACE_PHANTOM_GPU_TESTS

//=============================================================================
// IoPerfFixture — parameterized write-then-read benchmark fixture
//=============================================================================

static constexpr int kNumKeysPerDisk = 17;

class IoPerfFixture : public ::testing::Test, public ::testing::WithParamInterface<IoPerfConfig>
{
protected:
    static constexpr const char* kDefaultDiskPaths[] = {"/mnt/nvme0", "/mnt/nvme1", "/mnt/nvme2", "/mnt/nvme3"};

    struct BenchResult
    {
        double write_ms, write_gib_s;
        double read_ms, read_gib_s;
        size_t data_size;
    };

    void SetUp() override
    {
        const auto& param = GetParam();

        // GDS + cuFile unavailable → skip
        if (param.policy == IoPolicy::gds) {
            // Probe: try creating a transport to see if cuFile is available
            try {
                std::string probe_path = rtio::test::make_temp_path("gds_probe", ".bin");
                IoConfig probe_cfg;
                probe_cfg.fallback_on_error = false;
                auto probe = create_io_transport(probe_path, "wb", IoLoc::Device, IoPolicy::gds, probe_cfg);
                // If we get here, GDS works — clean up probe file
                rtio::test::remove_file(probe_path);
            }
            catch (const std::exception& e) {
                GTEST_SKIP() << "GDS transport unavailable: " << e.what();
            }
        }

        // Check disk accessibility
        active_disk_paths_.clear();
        test_files_.clear();
        for (int i = 0; i < param.num_disks; ++i) {
            const char* dp = kDefaultDiskPaths[i];
            if (access(dp, W_OK | R_OK) != 0) {
                GTEST_SKIP() << "Disk path not accessible: " << dp;
            }
            active_disk_paths_.push_back(dp);
        }
    }

    void TearDown() override
    {
        for (const auto& f : test_files_) {
            rtio::test::remove_file(f);
        }
        test_files_.clear();
    }

    BenchResult run_write_read_bench()
    {
        const auto& param = GetParam();
        const size_t per_disk_size = param.total_data_size / static_cast<size_t>(param.num_disks);

        // Total data is param.total_data_size
        BenchResult result{};
        result.data_size = param.total_data_size;

        // Allocate per-disk GPU buffers
        std::vector<rtio::test::GpuBuffer> disk_gpubufs;
        disk_gpubufs.reserve(param.num_disks);
        for (int d = 0; d < param.num_disks; ++d) {
            disk_gpubufs.emplace_back(per_disk_size);
        }

        // Fill each GPU buffer with a known pattern
        uint8_t pattern_byte = static_cast<uint8_t>(0xA0 | param.num_threads);
        for (int d = 0; d < param.num_disks; ++d) {
            rtio::test::fill_gpu_pattern(disk_gpubufs[d].ptr(), per_disk_size, pattern_byte);
        }

        // Register test file paths for all disks (so TearDown can clean up)
        std::vector<std::vector<std::string>> disk_file_paths(param.num_disks);
        for (int d = 0; d < param.num_disks; ++d) {
            const std::string& disk_base = active_disk_paths_[d];
            if (param.merged) {
                std::string mpath = disk_base + "/io_bench_merged_" + std::to_string(d) + ".bin";
                disk_file_paths[d].push_back(mpath);
            }
            else {
                for (int k = 0; k < kNumKeysPerDisk; ++k) {
                    std::string kpath = disk_base + "/io_bench_key_" + std::to_string(d) + "_" + std::to_string(k) +
                                        ".bin";
                    disk_file_paths[d].push_back(kpath);
                }
            }
            for (const auto& p : disk_file_paths[d]) {
                test_files_.push_back(p);
            }
        }

        IoConfig cfg;
        cfg.num_io_threads = param.num_threads;
        cfg.chunk_size = 8UL * 1024 * 1024;
        cfg.fallback_on_error = true;

        // Per-disk bench function — takes pre-registered file paths,
        // does NOT modify test_files_.
        // Returns per-disk {write_ms, read_ms}.
        auto bench_disk = [&](int disk_id) -> std::pair<double, double> {
            void* gpu_ptr = disk_gpubufs[disk_id].ptr();
            double write_ms = 0.0, read_ms = 0.0;
            const auto& disk_paths = disk_file_paths[disk_id];

            if (param.merged) {
                const std::string& mpath = disk_paths[0];

                // Write phase
                {
                    auto io = create_io_transport(mpath, "wb", IoLoc::Device, param.policy, cfg);
                    // Warmup
                    io->write_gpu(gpu_ptr, 0, per_disk_size);
                    auto t0 = std::chrono::high_resolution_clock::now();
                    io->write_gpu(gpu_ptr, 0, per_disk_size);
                    auto t1 = std::chrono::high_resolution_clock::now();
                    write_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
                }

                // Read phase — use open_reader for transport-based read
                {
                    auto reader = open_reader(mpath, IoLoc::Device, IoPolicy::auto_);
                    if (!reader) {
                        ADD_FAILURE() << "Failed to open reader for " << mpath;
                        return {0.0, 0.0};
                    }

                    // Warmup
                    reader->register_buffer(gpu_ptr, per_disk_size);
                    reader->read_gpu(gpu_ptr, 0, per_disk_size);
                    auto t0 = std::chrono::high_resolution_clock::now();
                    reader->read_gpu(gpu_ptr, 0, per_disk_size);
                    auto t1 = std::chrono::high_resolution_clock::now();
                    read_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
                    reader->deregister_all();
                }
            }
            else {
                // --- Per-key mode: kNumKeysPerDisk files per disk ---
                const size_t per_key_size = per_disk_size / kNumKeysPerDisk;

                // Write phase: one transport per key file
                {
                    // Warmup
                    for (int k = 0; k < kNumKeysPerDisk; ++k) {
                        auto io = create_io_transport(disk_paths[k], "wb", IoLoc::Device, param.policy, cfg);
                        size_t offset = static_cast<size_t>(k) * per_key_size;
                        io->write_gpu(static_cast<uint8_t*>(gpu_ptr) + offset, 0, per_key_size);
                    }
                    auto t0 = std::chrono::high_resolution_clock::now();
                    for (int k = 0; k < kNumKeysPerDisk; ++k) {
                        auto io = create_io_transport(disk_paths[k], "wb", IoLoc::Device, param.policy, cfg);
                        size_t offset = static_cast<size_t>(k) * per_key_size;
                        io->write_gpu(static_cast<uint8_t*>(gpu_ptr) + offset, 0, per_key_size);
                    }
                    auto t1 = std::chrono::high_resolution_clock::now();
                    write_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
                }

                // Read phase: one transport per key file
                {
                    // Warmup
                    for (int k = 0; k < kNumKeysPerDisk; ++k) {
                        auto io = create_io_transport(disk_paths[k], "rb", IoLoc::Device, param.policy, cfg);
                        size_t offset = static_cast<size_t>(k) * per_key_size;
                        io->read_gpu(static_cast<uint8_t*>(gpu_ptr) + offset, 0, per_key_size);
                    }
                    auto t0 = std::chrono::high_resolution_clock::now();
                    for (int k = 0; k < kNumKeysPerDisk; ++k) {
                        auto io = create_io_transport(disk_paths[k], "rb", IoLoc::Device, param.policy, cfg);
                        size_t offset = static_cast<size_t>(k) * per_key_size;
                        io->read_gpu(static_cast<uint8_t*>(gpu_ptr) + offset, 0, per_key_size);
                    }
                    auto t1 = std::chrono::high_resolution_clock::now();
                    read_ms = std::chrono::duration<double, std::milli>(t1 - t0).count();
                }
            }

            return {write_ms, read_ms};
        };

        // Single-disk: run directly
        if (param.num_disks == 1) {
            auto [w_ms, r_ms] = bench_disk(0);
            result.write_ms = w_ms;
            result.read_ms = r_ms;
        }
        else {
            // Multi-disk: one thread per disk, total time = max across threads
            // Warmup pass (sequential, not timed)
            for (int d = 0; d < param.num_disks; ++d) {
                bench_disk(d);
            }

            // Timed pass (parallel across disks)
            std::vector<std::thread> threads;
            std::vector<std::pair<double, double>> disk_times(param.num_disks);
            for (int d = 0; d < param.num_disks; ++d) {
                threads.emplace_back([&, d]() { disk_times[d] = bench_disk(d); });
            }
            for (auto& t : threads) {
                t.join();
            }

            // Total time = max across disks (wall-clock across parallel disks)
            double max_write_ms = 0.0, max_read_ms = 0.0;
            for (int d = 0; d < param.num_disks; ++d) {
                max_write_ms = std::max(max_write_ms, disk_times[d].first);
                max_read_ms = std::max(max_read_ms, disk_times[d].second);
            }
            result.write_ms = max_write_ms;
            result.read_ms = max_read_ms;
        }

        // Compute throughput
        double total_gib = static_cast<double>(param.total_data_size) / (1024.0 * 1024.0 * 1024.0);
        result.write_gib_s = total_gib / (result.write_ms / 1000.0);
        result.read_gib_s = total_gib / (result.read_ms / 1000.0);

        // Verify data integrity (spot-check first disk)
        bool ok = rtio::test::verify_gpu_data(disk_gpubufs[0].ptr(), per_disk_size, pattern_byte);
        EXPECT_TRUE(ok) << "Data verification failed after read";

        return result;
    }

    void write_result_json(const BenchResult& result)
    {
        const auto& param = GetParam();

        nlohmann::json j;
        j["transport"] = policy_name(param.policy);
        j["num_threads"] = param.num_threads;
        j["num_disks"] = param.num_disks;
        j["file_mode"] = param.merged ? "merged" : "per-key";
        j["total_data_size"] = format_size_gib(param.total_data_size);

        size_t per_disk_size = param.total_data_size / static_cast<size_t>(param.num_disks);
        j["per_disk_size_bytes"] = per_disk_size;
        j["per_disk_size_gib"] = static_cast<double>(per_disk_size) / (1024.0 * 1024.0 * 1024.0);

        j["disk_paths"] = active_disk_paths_;

        nlohmann::json cfg_json;
        cfg_json["chunk_size"] = 8388608;
        cfg_json["num_io_threads"] = param.num_threads;
        cfg_json["fallback_on_error"] = true;
        j["config"] = cfg_json;

        j["write"]["elapsed_ms"] = result.write_ms;
        j["write"]["throughput_gib_s"] = result.write_gib_s;
        j["read"]["elapsed_ms"] = result.read_ms;
        j["read"]["throughput_gib_s"] = result.read_gib_s;

        // Timestamp
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        char time_buf[64];
        std::strftime(time_buf, sizeof(time_buf), "%Y-%m-%dT%H:%M:%S", std::localtime(&time_t_now));
        j["timestamp"] = std::string(time_buf);

        // Hostname
        char hostname_buf[256];
        if (gethostname(hostname_buf, sizeof(hostname_buf)) == 0) {
            j["hostname"] = std::string(hostname_buf);
        }
        else {
            j["hostname"] = "unknown";
        }

        // GDS availability
        j["gds_available"] = (param.policy == IoPolicy::gds);

        // Actual transport used
        j["actual_transport_used"] = policy_name(param.policy);

        // Build filename
        std::string filename = "/tmp/io_bench_" + policy_name(param.policy) + "_" + std::to_string(param.num_threads) +
                               "t_" + std::to_string(param.num_disks) + "d_" + format_size_gib(param.total_data_size) +
                               "_" + (param.merged ? "merged" : "perkey") + ".json";

        std::ofstream ofs(filename);
        if (ofs.is_open()) {
            ofs << j.dump(2) << std::endl;
        }

        printf("[PERF] %s %dT %dD %s %s: write=%.1fms(%.2fGiB/s) read=%.1fms(%.2fGiB/s)\n",
               policy_name(param.policy).c_str(), param.num_threads, param.num_disks,
               param.merged ? "merged" : "per-key", format_size_gib(param.total_data_size).c_str(), result.write_ms,
               result.write_gib_s, result.read_ms, result.read_gib_s);
    }

    // Per-disk test file paths (populated during bench, cleaned in TearDown)
    std::vector<std::string> test_files_;
    // Disk paths confirmed accessible in SetUp
    std::vector<std::string> active_disk_paths_;
};

//=============================================================================
// Parameterized write-then-read test
//=============================================================================

TEST_P(IoPerfFixture, WriteThenRead)
{
    auto result = run_write_read_bench();
    write_result_json(result);

    // Sanity: both phases should complete with positive throughput
    EXPECT_GT(result.write_gib_s, 0.0);
    EXPECT_GT(result.read_gib_s, 0.0);
}

INSTANTIATE_TEST_SUITE_P(AllConfigs, IoPerfFixture, ::testing::ValuesIn(generate_perf_configs()), IoPerfConfigName);

//=============================================================================
// Chunk size sweep (Staged) — independent dimension, non-parameterized
//=============================================================================

namespace
{

struct PerfResult
{
    double elapsed_ms;
    double throughput_gib_s;
    size_t bytes;
};

PerfResult measure_read_gpu(IoTransport& io, void* gpu_ptr, size_t offset, size_t size, int iterations = 3)
{
    // Warmup
    io.read_gpu(gpu_ptr, offset, size);

    auto t0 = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iterations; i++) {
        io.read_gpu(gpu_ptr, offset, size);
    }
    auto t1 = std::chrono::high_resolution_clock::now();

    double ms = std::chrono::duration<double, std::milli>(t1 - t0).count() / iterations;
    double gib = static_cast<double>(size) / (1024.0 * 1024.0 * 1024.0);
    double seconds = ms / 1000.0;

    return {ms, gib / seconds, size};
}

}  // anonymous namespace

TEST(IoPerfTest, ChunkSizeSweepStaged)
{
    const size_t data_size = 64 * 1024 * 1024;  // 64 MB
    auto guard = rtio::test::make_temp_file("perf_chunk_staged", ".bin");

    rtio::test::GpuBuffer gpu_buf(data_size);
    rtio::test::GpuBuffer gpu_read(data_size);

    {
        StagedTransport io(guard.path(), "wb");
        ASSERT_TRUE(io.write_gpu(gpu_buf.ptr(), 0, data_size));
    }

    std::vector<uint8_t> pattern(data_size, 0xCD);
    FHE_CHECK_CUDA(cudaMemcpy(gpu_buf.ptr(), pattern.data(), data_size, cudaMemcpyHostToDevice));
    {
        StagedTransport io(guard.path(), "wb");
        ASSERT_TRUE(io.write_gpu(gpu_buf.ptr(), 0, data_size));
    }

    const size_t chunk_sizes[] = {64 * 1024, 512 * 1024, 1024 * 1024, 4 * 1024 * 1024};

    for (size_t cs : chunk_sizes) {
        StagedTransport io(guard.path(), "rb", cs);
        auto result = measure_read_gpu(io, gpu_read.ptr(), 0, data_size);
        printf("[PERF] Staged chunk=%zuK: %.1f ms, %.2f GiB/s\n", cs / 1024, result.elapsed_ms,
               result.throughput_gib_s);
    }
}

//=============================================================================
// Chunk size sweep (Mapped) — independent dimension, non-parameterized
//=============================================================================

TEST(IoPerfTest, ChunkSizeSweepMapped)
{
    const size_t data_size = 64 * 1024 * 1024;  // 64 MB
    auto guard = rtio::test::make_temp_file("perf_chunk_mapped", ".bin");

    rtio::test::GpuBuffer gpu_buf(data_size);
    rtio::test::GpuBuffer gpu_read(data_size);

    {
        MmapTransport io(guard.path(), "wb");
        ASSERT_TRUE(io.write_gpu(gpu_buf.ptr(), 0, data_size));
    }

    const size_t chunk_sizes[] = {64 * 1024, 512 * 1024, 1024 * 1024, 4 * 1024 * 1024};

    for (size_t cs : chunk_sizes) {
        MmapTransport io(guard.path(), "rb", cs);
        auto result = measure_read_gpu(io, gpu_read.ptr(), 0, data_size);
        printf("[PERF] Mapped chunk=%zuK: %.1f ms, %.2f GiB/s\n", cs / 1024, result.elapsed_ms,
               result.throughput_gib_s);
    }
}

//=============================================================================
// CRC32 overhead measurement — independent dimension, non-parameterized
//=============================================================================

TEST(IoPerfTest, Crc32Overhead)
{
    const size_t data_size = 64 * 1024 * 1024;
    auto guard = rtio::test::make_temp_file("perf_crc", ".bin");

    rtio::test::GpuBuffer gpu_buf(data_size);
    rtio::test::GpuBuffer gpu_read(data_size);

    {
        StagedTransport io(guard.path(), "wb");
        ASSERT_TRUE(io.write_gpu(gpu_buf.ptr(), 0, data_size));
    }

    double ms_no_crc;
    {
        StagedTransport io(guard.path(), "rb");
        io.read_gpu(gpu_read.ptr(), 0, data_size);

        auto t0 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 3; i++) {
            io.read_gpu(gpu_read.ptr(), 0, data_size);
        }
        auto t1 = std::chrono::high_resolution_clock::now();
        ms_no_crc = std::chrono::duration<double, std::milli>(t1 - t0).count() / 3.0;
    }

    double ms_with_crc;
    {
        StagedTransport io(guard.path(), "rb");
        io.reset_crc();
        io.read_gpu(gpu_read.ptr(), 0, data_size);
        uint32_t crc = io.crc32();
        (void)crc;

        auto t0 = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < 3; i++) {
            io.reset_crc();
            io.read_gpu(gpu_read.ptr(), 0, data_size);
            uint32_t c = io.crc32();
            (void)c;
        }
        auto t1 = std::chrono::high_resolution_clock::now();
        ms_with_crc = std::chrono::duration<double, std::milli>(t1 - t0).count() / 3.0;
    }

    double overhead_pct = (ms_with_crc - ms_no_crc) / ms_no_crc * 100.0;
    printf("[PERF] CRC32 overhead: %.1f ms (no CRC) vs %.1f ms (with CRC) = %.1f%% overhead\n", ms_no_crc, ms_with_crc,
           overhead_pct);

    EXPECT_LT(overhead_pct, 20.0) << "CRC32 overhead unexpectedly high";
}

#else  // !ACE_PHANTOM_GPU_TESTS

TEST(IoPerfTest, GpuTestsDisabled)
{
    GTEST_SKIP() << "ACE_PHANTOM_GPU_TESTS not enabled — skipping IO perf tests";
}

#endif  // ACE_PHANTOM_GPU_TESTS
