//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief test_file_transport.cpp
//! Unit tests for FileTransport base class (host I/O, CRC32, file_size).
//!
//! Tests are parameterized over StagedTransport and MmapTransport to
//! verify that both concrete implementations share the same FileTransport
//! base behavior, eliminating code duplication.

#include "fhe/io/mmap_transport.h"
#include "fhe/io/staged_transport.h"
#include "fhe/io/transport.h"
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
// Parameterized fixture — tests run for both Staged and Mapped
//=============================================================================

//! Parameter: 0 = StagedTransport, 1 = MmapTransport
class FileTransportParamTest : public ::testing::Test, public ::testing::WithParamInterface<int>
{
protected:
    std::unique_ptr<IoTransport> create_transport(const std::string& path, const char* mode)
    {
        if (GetParam() == 0) {
            return std::make_unique<StagedTransport>(path, mode);
        }
        else {
            return std::make_unique<MmapTransport>(path, mode);
        }
    }

    const char* transport_label() const { return GetParam() == 0 ? "staged" : "mapped"; }
};

INSTANTIATE_TEST_SUITE_P(ByTransport, FileTransportParamTest, ::testing::Values(0, 1),
                         [](const ::testing::TestParamInfo<int>& info) {
                             return info.param == 0 ? std::string("Staged") : std::string("Mapped");
                         });

//=============================================================================
// Host write/read
//=============================================================================

TEST_P(FileTransportParamTest, WriteReadHostData)
{
    auto guard = test::make_temp_file("ft", "host.bin");

    {
        auto io = create_transport(guard.path(), "wb");
        const char* msg = "FileTransport base test";
        size_t len = std::strlen(msg) + 1;
        ASSERT_TRUE(io->write(msg, 0, len));
        EXPECT_EQ(io->file_size(), len);
        EXPECT_STREQ(io->transport_name(), transport_label());
        EXPECT_FALSE(io->used_gds());
    }

    {
        auto io = create_transport(guard.path(), "rb");
        char buf[64] = {};
        ASSERT_TRUE(io->read(buf, 0, 24));
        EXPECT_STREQ(buf, "FileTransport base test");
    }
}

TEST_P(FileTransportParamTest, WriteReadMultipleOffsets)
{
    auto guard = test::make_temp_file("ft", "offsets.bin");

    {
        auto io = create_transport(guard.path(), "wb");
        uint32_t val1 = 0xAABBCCDD;
        uint64_t val2 = 0x123456789ABCDEF0ULL;
        ASSERT_TRUE(io->write(&val1, 0, sizeof(val1)));
        ASSERT_TRUE(io->write(&val2, 100, sizeof(val2)));
    }

    {
        auto io = create_transport(guard.path(), "rb");
        uint32_t r1 = 0;
        uint64_t r2 = 0;
        ASSERT_TRUE(io->read(&r1, 0, sizeof(r1)));
        ASSERT_TRUE(io->read(&r2, 100, sizeof(r2)));
        EXPECT_EQ(r1, 0xAABBCCDDu);
        EXPECT_EQ(r2, 0x123456789ABCDEF0ULL);
    }
}

//=============================================================================
// File size tracking
//=============================================================================

TEST_P(FileTransportParamTest, FileSizeTracking)
{
    auto guard = test::make_temp_file("ft", "filesize.bin");

    {
        auto io = create_transport(guard.path(), "wb");
        EXPECT_EQ(io->file_size(), 0u);
        uint8_t data[100] = {};
        ASSERT_TRUE(io->write(data, 0, 100));
        EXPECT_EQ(io->file_size(), 100u);
        ASSERT_TRUE(io->write(data, 200, 50));
        EXPECT_EQ(io->file_size(), 250u);
    }

    {
        auto io = create_transport(guard.path(), "rb");
        EXPECT_EQ(io->file_size(), 250u);
    }
}

//=============================================================================
// Edge cases
//=============================================================================

TEST_P(FileTransportParamTest, ReadNonexistentFile)
{
    std::string path = test::make_temp_path("ft", "nonexistent.bin");
    test::remove_file(path);
    if (GetParam() == 0) {
        EXPECT_THROW(StagedTransport io(path, "rb"), std::runtime_error);
    }
    else {
        EXPECT_THROW(MmapTransport io(path, "rb"), std::runtime_error);
    }
}

TEST_P(FileTransportParamTest, ZeroSizeReadWrite)
{
    auto guard = test::make_temp_file("ft", "zero.bin");

    {
        auto io = create_transport(guard.path(), "wb");
        uint8_t byte = 0xAA;
        ASSERT_TRUE(io->write(&byte, 0, 1));
    }

    {
        auto io = create_transport(guard.path(), "rb");
        EXPECT_TRUE(io->read(nullptr, 0, 0));
        EXPECT_TRUE(io->write(nullptr, 0, 0));
    }
}

//=============================================================================
// CRC32
//=============================================================================

TEST_P(FileTransportParamTest, Crc32EmptyByDefault)
{
    auto guard = test::make_temp_file("ft", "crc_default.bin");
    auto io = create_transport(guard.path(), "wb");
    EXPECT_EQ(io->crc32(), 0u);
}

TEST_P(FileTransportParamTest, Crc32Accumulation)
{
    auto guard = test::make_temp_file("ft", "crc_accum.bin");

    const char* data = "The quick brown fox jumps over the lazy dog";
    size_t data_len = std::strlen(data);

    {
        auto io = create_transport(guard.path(), "wb");
        ASSERT_TRUE(io->write(data, 0, data_len));
    }

    {
        auto io = create_transport(guard.path(), "rb");
        std::vector<char> buf(data_len);
        io->reset_crc();
        ASSERT_TRUE(io->read(buf.data(), 0, data_len));
        EXPECT_NE(io->crc32(), 0u);
    }

    // Partial read CRC != full read CRC
    {
        auto io = create_transport(guard.path(), "rb");
        std::vector<char> buf(data_len / 2);
        io->reset_crc();
        ASSERT_TRUE(io->read(buf.data(), 0, data_len / 2));
        uint32_t partial = io->crc32();

        auto io2 = create_transport(guard.path(), "rb");
        std::vector<char> buf_full(data_len);
        io2->reset_crc();
        ASSERT_TRUE(io2->read(buf_full.data(), 0, data_len));
        uint32_t full = io2->crc32();

        EXPECT_NE(partial, full);
    }
}

TEST_P(FileTransportParamTest, Crc32Consistency)
{
    auto guard = test::make_temp_file("ft", "crc_consist.bin");

    const uint8_t data[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xCA, 0xFE, 0xBA, 0xBE};

    {
        auto io = create_transport(guard.path(), "wb");
        ASSERT_TRUE(io->write(data, 0, sizeof(data)));
    }

    uint32_t crc1 = 0, crc2 = 0;
    {
        auto io = create_transport(guard.path(), "rb");
        uint8_t buf[sizeof(data)];
        io->reset_crc();
        ASSERT_TRUE(io->read(buf, 0, sizeof(data)));
        crc1 = io->crc32();
    }
    {
        auto io = create_transport(guard.path(), "rb");
        uint8_t buf[sizeof(data)];
        io->reset_crc();
        ASSERT_TRUE(io->read(buf, 0, sizeof(data)));
        crc2 = io->crc32();
    }

    EXPECT_EQ(crc1, crc2);
    EXPECT_NE(crc1, 0u);
}

//=============================================================================
// Cross-transport consistency (non-parameterized)
//=============================================================================

TEST(FileTransportCrossTest, FileSizeConsistency)
{
    auto guard = test::make_temp_file("ft", "cross_filesize.bin");

    const uint8_t data[128] = {};

    {
        StagedTransport io(guard.path(), "wb");
        ASSERT_TRUE(io.write(data, 0, 128));
        EXPECT_EQ(io.file_size(), 128u);
    }

    {
        MmapTransport io(guard.path(), "rb");
        EXPECT_EQ(io.file_size(), 128u);
    }

    {
        StagedTransport io(guard.path(), "rb");
        EXPECT_EQ(io.file_size(), 128u);
    }
}

TEST(FileTransportCrossTest, Crc32MatchesAcrossTransports)
{
    auto guard = test::make_temp_file("ft", "cross_crc.bin");

    const char* data = "Cross-transport CRC compatibility check";
    size_t data_len = std::strlen(data);

    {
        StagedTransport io(guard.path(), "wb");
        ASSERT_TRUE(io.write(data, 0, data_len));
    }

    uint32_t staged_crc = 0;
    {
        StagedTransport io(guard.path(), "rb");
        std::vector<char> buf(data_len);
        io.reset_crc();
        ASSERT_TRUE(io.read(buf.data(), 0, data_len));
        staged_crc = io.crc32();
    }

    uint32_t mapped_crc = 0;
    {
        MmapTransport io(guard.path(), "rb");
        std::vector<char> buf(data_len);
        io.reset_crc();
        ASSERT_TRUE(io.read(buf.data(), 0, data_len));
        mapped_crc = io.crc32();
    }

    EXPECT_EQ(staged_crc, mapped_crc);
    EXPECT_NE(staged_crc, 0u);
}
