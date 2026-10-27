//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief test_transport.cpp
//! Unit tests for IoConfig, IoTransport factory, and non-GPU transport
//! behavior (StagedTransport, MmapTransport).
//!
//! Factory tests are parameterized over IoPolicy (staged, mapped, gds)
//! to eliminate duplication.

#include "fhe/io/transport.h"

#include "fhe/io/cufile_api.h"
#include "fhe/io/gds_io.h"
#include "fhe/io/gds_transport.h"
#include "fhe/io/mmap_transport.h"
#include "fhe/io/staged_transport.h"
#include "test_util.h"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <gtest/gtest.h>
#include <memory>
#include <string>
#include <vector>

using namespace rtio;

//=============================================================================
// IoConfig tests (non-parameterized)
//=============================================================================

TEST(IoConfigTest, DefaultValues)
{
    IoConfig cfg;
    EXPECT_TRUE(cfg.fallback_on_error);
    EXPECT_EQ(cfg.chunk_size, 8UL * 1024 * 1024);
    EXPECT_EQ(cfg.num_io_threads, 4);
    EXPECT_TRUE(cfg.disk_paths.empty());
    EXPECT_TRUE(cfg.galois_merged_format);
    EXPECT_EQ(cfg.exec_mode, IoExecMode::sync);
}

TEST(IoConfigTest, GlobalConfigRoundTrip)
{
    IoConfig original;
    original.chunk_size = 1024 * 1024;
    original.num_io_threads = 8;
    original.fallback_on_error = false;

    set_global_io_config(original);
    IoConfig retrieved = get_global_io_config();

    EXPECT_EQ(retrieved.chunk_size, 1024UL * 1024);
    EXPECT_EQ(retrieved.num_io_threads, 8);
    EXPECT_FALSE(retrieved.fallback_on_error);

    IoConfig defaults;
    set_global_io_config(defaults);
}

//=============================================================================
// IoTransport factory — parameterized by IoPolicy
//=============================================================================

class IoTransportParamTest : public ::testing::Test, public ::testing::WithParamInterface<IoPolicy>
{
protected:
    IoConfig make_config() const
    {
        IoConfig cfg;
        return cfg;
    }

    std::unique_ptr<IoTransport> create_transport(const std::string& path, const char* mode)
    {
        return create_io_transport(path, mode, IoLoc::Device, GetParam(), make_config());
    }

    bool should_skip_gds() const { return GetParam() == IoPolicy::gds && !gds_driver_available(); }
};

INSTANTIATE_TEST_SUITE_P(ByIoPolicy, IoTransportParamTest,
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

TEST_P(IoTransportParamTest, FactoryReturnsCorrectType)
{
    if (should_skip_gds()) {
        GTEST_SKIP() << "cuFile not available";
    }

    auto guard = test::make_temp_file("io", "factory.bin");

    {
        auto io = create_transport(guard.path(), "wb");
        ASSERT_NE(io, nullptr);

        std::string name = io->transport_name();
        switch (GetParam()) {
        case IoPolicy::staged:
            EXPECT_STREQ(name.c_str(), "staged");
            EXPECT_FALSE(io->used_gds());
            break;
        case IoPolicy::mapped:
            EXPECT_STREQ(name.c_str(), "mapped");
            EXPECT_FALSE(io->used_gds());
            break;
        case IoPolicy::gds:
            EXPECT_TRUE(name == "gds" || name == "gds+mt" || name == "gds(fallback)")
                << "Unexpected GDS transport name: " << name;
            break;
        default:
            break;
        }

        uint32_t val = 42;
        ASSERT_TRUE(io->write(&val, 0, sizeof(val)));
    }

    {
        auto io = create_transport(guard.path(), "rb");
        ASSERT_NE(io, nullptr);
        uint32_t val = 0;
        ASSERT_TRUE(io->read(&val, 0, sizeof(val)));
        EXPECT_EQ(val, 42u);
    }
}

//=============================================================================
// Factory — non-parameterized special cases
//=============================================================================

TEST(IoTransportFactoryTest, HostLocationReturnsStaged)
{
    auto guard = test::make_temp_file("io", "hostloc.bin");

    IoConfig cfg;

    auto io = create_io_transport(guard.path(), "wb", IoLoc::Host, IoPolicy::auto_, cfg);
    ASSERT_NE(io, nullptr);
    EXPECT_STREQ(io->transport_name(), "staged");
}

TEST(IoTransportFactoryTest, AutoPolicySelectsTransport)
{
    auto guard = test::make_temp_file("io", "auto.bin");

    {
        StagedTransport io(guard.path(), "wb");
        uint8_t byte = 0;
        io.write(&byte, 0, 1);
    }

    IoConfig cfg;

    auto io = create_io_transport(guard.path(), "rb", IoLoc::Device, IoPolicy::auto_, cfg);
    ASSERT_NE(io, nullptr);
    std::string name = io->transport_name();
    EXPECT_TRUE(name == "staged" || name == "gds" || name == "gds+mt" || name == "gds(fallback)")
        << "Unexpected transport name: " << name;
}

//=============================================================================
// Transport name / used_gds (non-parameterized)
//=============================================================================

TEST(StagedTransportTest, TransportName)
{
    auto guard = test::make_temp_file("io", "staged_name.bin");
    StagedTransport io(guard.path(), "wb");
    EXPECT_STREQ(io.transport_name(), "staged");
    EXPECT_FALSE(io.used_gds());
}

TEST(MmapTransportTest, TransportName)
{
    auto guard = test::make_temp_file("io", "mapped_name.bin");
    MmapTransport io(guard.path(), "wb");
    EXPECT_STREQ(io.transport_name(), "mapped");
    EXPECT_FALSE(io.used_gds());
}
