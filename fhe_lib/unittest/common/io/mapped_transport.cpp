//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief test_mapped_transport.cpp
//! Unit tests for MappedTransport-specific edge cases.
//!
//! Tests that are specific to MappedTransport's mmap-based implementation
//! (unaligned offsets, read-beyond-file) and cross-transport CRC consistency
//! with StagedTransport. Parameterized FileTransport tests are in
//! file_transport.cpp.

#include "fhe/io/mapped_transport.h"

#include "fhe/io/staged_transport.h"
#include "test_util.h"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace rtio;

//=============================================================================
// MappedTransport-specific edge cases
//=============================================================================

TEST(MappedTransportTest, UnalignedOffset)
{
    auto guard = test::make_temp_file("ft", "mapped_unaligned.bin");

    const size_t header_size = 64;
    const size_t data_size = 256;
    const uint8_t fill_byte = 0x42;

    {
        MappedTransport io(guard.path(), "wb");
        std::vector<uint8_t> header(header_size, 0);
        ASSERT_TRUE(io.write(header.data(), 0, header_size));
        std::vector<uint8_t> data(data_size, fill_byte);
        ASSERT_TRUE(io.write(data.data(), header_size, data_size));
    }

    {
        MappedTransport io(guard.path(), "rb");
        std::vector<uint8_t> buf(data_size, 0);
        ASSERT_TRUE(io.read(buf.data(), header_size, data_size));
        for (size_t i = 0; i < data_size; i++) {
            EXPECT_EQ(buf[i], fill_byte) << "mismatch at byte " << i;
        }
    }
}

TEST(MappedTransportTest, ReadBeyondFile)
{
    auto guard = test::make_temp_file("ft", "mapped_beyond.bin");

    {
        MappedTransport io(guard.path(), "wb");
        uint8_t byte = 0xFF;
        ASSERT_TRUE(io.write(&byte, 0, 1));
    }

    {
        MappedTransport io(guard.path(), "rb");
        std::vector<uint8_t> buf(1024, 0);
        // Trying to read 1024 bytes from a 1-byte file should fail
        EXPECT_FALSE(io.read(buf.data(), 0, 1024));
    }
}

TEST(MappedTransportTest, Crc32MatchesStagedTransport)
{
    auto guard = test::make_temp_file("ft", "mapped_staged_crc.bin");

    const char* data = "Cross-transport CRC compatibility test";
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
        MappedTransport io(guard.path(), "rb");
        std::vector<char> buf(data_len);
        io.reset_crc();
        ASSERT_TRUE(io.read(buf.data(), 0, data_len));
        mapped_crc = io.crc32();
    }

    EXPECT_EQ(staged_crc, mapped_crc);
    EXPECT_NE(staged_crc, 0u);
}
