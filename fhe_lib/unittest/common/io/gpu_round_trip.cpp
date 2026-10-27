//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief test_gpu_round_trip.cpp
//! GPU round-trip tests for IoTransport (write_gpu → read_gpu).
//!
//! These tests require ACE_PHANTOM_GPU_TESTS to be enabled and a CUDA GPU
//! available. They verify that each transport type (Staged, Mapped, GDS)
//! correctly moves data through GPU memory.

#include "fhe/io/cufile_api.h"
#include "fhe/io/gds_io.h"
#include "fhe/io/gds_transport.h"
#include "fhe/io/mmap_transport.h"
#include "fhe/io/staged_transport.h"
#include "fhe/io/transport.h"
#include "test_util.h"

#include <cstdint>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

#ifdef ACE_PHANTOM_GPU_TESTS
#include "fhe/io/cuda_check.h"

#include <cuda_runtime.h>
#endif

using namespace rtio;

#ifdef ACE_PHANTOM_GPU_TESTS

class IoTransportGpuTest : public ::testing::Test, public ::testing::WithParamInterface<IoPolicy>
{
protected:
    void SetUp() override
    {
        if (GetParam() == IoPolicy::gds && !gds_driver_available()) {
            GTEST_SKIP() << "cuFile not available";
        }

        guard_ = test::make_temp_file("io", "gpu.bin");
        gpu_write_ = std::make_unique<test::GpuBuffer>(kDataSize);
        gpu_read_ = std::make_unique<test::GpuBuffer>(kDataSize);

        std::vector<uint8_t> pattern(kDataSize);
        for (size_t i = 0; i < kDataSize; i++) {
            pattern[i] = static_cast<uint8_t>(i & 0xFF);
        }
        FHE_CHECK_CUDA(cudaMemcpy(gpu_write_->ptr(), pattern.data(), kDataSize, cudaMemcpyHostToDevice));
    }

    void TearDown() override
    {
        gpu_write_.reset();
        gpu_read_.reset();
    }

    IoConfig make_config() const
    {
        IoConfig cfg;
        return cfg;
    }

    static constexpr size_t kDataSize = 1024 * 1024;
    test::TempFileGuard guard_{"/tmp/fhert_io_test_placeholder"};
    std::unique_ptr<test::GpuBuffer> gpu_write_;
    std::unique_ptr<test::GpuBuffer> gpu_read_;
};

INSTANTIATE_TEST_SUITE_P(ByIoPolicy, IoTransportGpuTest,
                         ::testing::Values(IoPolicy::staged, IoPolicy::mapped, IoPolicy::gds),
                         [](const ::testing::TestParamInfo<IoPolicy>& info) {
                             switch (info.param) {
                             case IoPolicy::staged:
                                 return "Staged";
                             case IoPolicy::mapped:
                                 return "Mapped";
                             case IoPolicy::gds:
                                 return "Gds";
                             default:
                                 return "Unknown";
                             }
                         });

TEST_P(IoTransportGpuTest, WriteReadGpuRoundTrip)
{
    {
        auto io = create_io_transport(guard_.path(), "wb", IoLoc::Device, GetParam(), make_config());
        ASSERT_TRUE(io->write_gpu(gpu_write_->ptr(), 0, kDataSize));
        EXPECT_EQ(io->file_size(), kDataSize);
    }

    {
        auto io = create_io_transport(guard_.path(), "rb", IoLoc::Device, GetParam(), make_config());
        ASSERT_TRUE(io->read_gpu(gpu_read_->ptr(), 0, kDataSize));
    }

    std::vector<uint8_t> result(kDataSize);
    FHE_CHECK_CUDA(cudaMemcpy(result.data(), gpu_read_->ptr(), kDataSize, cudaMemcpyDeviceToHost));
    std::vector<uint8_t> expected(kDataSize);
    for (size_t i = 0; i < kDataSize; i++) {
        expected[i] = static_cast<uint8_t>(i & 0xFF);
    }
    EXPECT_EQ(result, expected);
}

#else  // !ACE_PHANTOM_GPU_TESTS

TEST(StagedTransportGpuTest, GpuTestsDisabled)
{
    GTEST_SKIP() << "ACE_PHANTOM_GPU_TESTS not enabled — skipping GPU round-trip tests";
}

#endif  // ACE_PHANTOM_GPU_TESTS
