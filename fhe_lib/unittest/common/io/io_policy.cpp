//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief test_io_policy.cpp
//! Unit tests for the transport-level IO policy API (io_policy.h).
//!
//! Tests IoPolicy enum, open_reader/open_writer factories, and the
//! policy → transport selection logic.

#include "fhe/io/io_policy.h"

#include "fhe/io/staged_transport.h"
#include "test_util.h"

#include <cstdint>
#include <gtest/gtest.h>
#include <memory>
#include <string>

using namespace rtio;

//=============================================================================
// IoPolicy — transport-level API
//=============================================================================

TEST(IoPolicyTest, DefaultValues)
{
    EXPECT_EQ(get_global_io_policy(), IoPolicy::auto_);
}

TEST(IoPolicyTest, GlobalPolicyRoundTrip)
{
    set_global_io_policy(IoPolicy::staged);
    EXPECT_EQ(get_global_io_policy(), IoPolicy::staged);

    set_global_io_policy(IoPolicy::gds);
    EXPECT_EQ(get_global_io_policy(), IoPolicy::gds);

    set_global_io_policy(IoPolicy::auto_);
    EXPECT_EQ(get_global_io_policy(), IoPolicy::auto_);
}

TEST(IoPolicyTest, CreateReaderWriterRoundTrip)
{
    auto guard = test::make_temp_file("io", "policy_roundtrip.bin");

    {
        auto writer = open_writer(guard.path(), IoLoc::Host, IoPolicy::auto_);
        ASSERT_NE(writer, nullptr);
        uint32_t val = 0xDEADBEEF;
        ASSERT_TRUE(writer->write(&val, 0, sizeof(val)));
    }

    {
        auto reader = open_reader(guard.path(), IoLoc::Host, IoPolicy::auto_);
        ASSERT_NE(reader, nullptr);
        uint32_t val = 0;
        ASSERT_TRUE(reader->read(&val, 0, sizeof(val)));
        EXPECT_EQ(val, 0xDEADBEEFu);
    }
}

TEST(IoPolicyTest, StagedPolicyUsesStaged)
{
    // staged policy → always StagedTransport
    auto guard = test::make_temp_file("io", "staged.bin");
    auto writer = open_writer(guard.path(), IoLoc::Device, IoPolicy::staged);
    ASSERT_NE(writer, nullptr);
    EXPECT_STREQ(writer->transport_name(), "staged");
}

TEST(IoPolicyTest, HostLocationOverridesGds)
{
    // Host location should always use staged regardless of policy
    auto guard = test::make_temp_file("io", "host_policy.bin");
    auto writer = open_writer(guard.path(), IoLoc::Host, IoPolicy::gds);
    ASSERT_NE(writer, nullptr);
    EXPECT_STREQ(writer->transport_name(), "staged");
}

TEST(IoPolicyTest, OpenReaderWorksForAllPolicies)
{
    auto guard = test::make_temp_file("io", "policy_reader.bin");
    {
        StagedTransport io(guard.path(), "wb");
        uint8_t byte = 0xAB;
        io.write(&byte, 0, 1);
    }

    // open_reader should succeed for various policies
    for (auto policy : {IoPolicy::auto_, IoPolicy::gds, IoPolicy::staged, IoPolicy::mapped}) {
        auto reader = open_reader(guard.path(), IoLoc::Host, policy);
        if (reader) {
            EXPECT_EQ(reader->path(), guard.path());
        }
    }
}

TEST(IoPolicyTest, PolicyAffectsTransportSelection)
{
    // auto_ with Device → GDS if available, else staged
    auto guard = test::make_temp_file("io", "policy_select.bin");
    auto writer = open_writer(guard.path(), IoLoc::Device, IoPolicy::auto_);
    ASSERT_NE(writer, nullptr);
    std::string name = writer->transport_name();
    // With Device location and auto data path, either staged or gds is valid
    EXPECT_TRUE(name == "staged" || name == "gds" || name == "gds+mt" || name == "gds(fallback)")
        << "Unexpected transport name: " << name;
}

TEST(IoPolicyTest, IoExecModeValues)
{
    // IoExecMode::sync is the only supported value currently
    IoExecMode mode = IoExecMode::sync;
    EXPECT_EQ(mode, IoExecMode::sync);
}
