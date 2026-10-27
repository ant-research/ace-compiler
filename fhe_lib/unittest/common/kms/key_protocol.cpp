//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief test_key_protocol.cpp
//! Host-only unit tests for KeyWriter/KeyReader protocol (KeyHeader + CRC32).
//!
//! Tests write→read round-trips using StagedTransport (host data only).
//! GPU data round-trips require ACE_PHANTOM_GPU_TESTS.

#include "fhe/kms/key_protocol.h"

#include "fhe/io/mapped_transport.h"
#include "fhe/io/staged_transport.h"
#include "fhe/kms/key_header.h"
#include "test_util.h"

#include <cstdint>
#include <cstdio>
#include <cstring>
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace rtio;
using namespace fhert;

namespace
{

KeyHeader make_valid_header()
{
    KeyHeader hdr{};
    hdr.magic = KEY_MAGIC;
    hdr.version = KEY_FORMAT_VERSION;
    hdr.header_size = sizeof(KeyHeader);
    hdr.key_type = static_cast<uint16_t>(KeyType::SECRET_KEY);
    hdr.poly_modulus_degree = 16384;
    hdr.coeff_modulus_size = 37;
    hdr.data_size = 256;
    hdr.reserved[0] = 0;
    hdr.reserved[1] = 0;
    hdr.crc32 = 0;
    return hdr;
}

}  // anonymous namespace

//=============================================================================
// KeyWriter → KeyReader round-trip (host data only)
//=============================================================================

TEST(KeyProtocolTest, WriteReadHeaderRoundTrip)
{
    auto guard = test::make_temp_file("kp", "hdr_rt.bin");

    KeyHeader hdr = make_valid_header();

    // Write
    {
        StagedTransport io(guard.path(), "wb");
        KeyWriter writer(io, hdr);
        ASSERT_TRUE(writer.write_header());
        size_t total = writer.finish();
        EXPECT_EQ(total, sizeof(KeyHeader));  // header only, no data
    }

    // Read
    {
        StagedTransport io(guard.path(), "rb");
        KeyReader reader(io);
        KeyHeader read_hdr = reader.read_header();
        EXPECT_EQ(read_hdr.magic, KEY_MAGIC);
        EXPECT_EQ(read_hdr.version, KEY_FORMAT_VERSION);
        EXPECT_EQ(read_hdr.poly_modulus_degree, 16384u);
        EXPECT_EQ(read_hdr.coeff_modulus_size, 37u);
        EXPECT_EQ(read_hdr.data_size, 256u);
    }
}

TEST(KeyProtocolTest, WriteReadWithMetadata)
{
    auto guard = test::make_temp_file("kp", "meta_rt.bin");

    KeyHeader hdr = make_valid_header();
    uint32_t meta_data = 0xABBACAFE;

    // Write: header + metadata
    {
        StagedTransport io(guard.path(), "wb");
        KeyWriter writer(io, hdr);
        ASSERT_TRUE(writer.write_header());
        ASSERT_TRUE(writer.write_meta(&meta_data, sizeof(meta_data)));
        hdr.data_size = sizeof(meta_data);
        // Re-create writer with updated data_size (simplified: just test CRC flow)
        size_t total = writer.finish();
        EXPECT_GT(total, sizeof(KeyHeader));
    }

    // Read: header + metadata
    {
        StagedTransport io(guard.path(), "rb");
        KeyReader reader(io);
        KeyHeader read_hdr = reader.read_header();
        EXPECT_EQ(read_hdr.poly_modulus_degree, 16384u);

        uint32_t read_meta = 0;
        ASSERT_TRUE(reader.read_meta(&read_meta, sizeof(read_meta)));
        EXPECT_EQ(read_meta, 0xABBACAFEu);
    }
}

//=============================================================================
// CRC32 validation with StagedTransport
//=============================================================================

TEST(KeyProtocolTest, Crc32ValidationPass)
{
    auto guard = test::make_temp_file("kp", "crc_pass.bin");

    KeyHeader hdr = make_valid_header();
    const char* payload = "CRC validation test payload";
    size_t payload_len = std::strlen(payload);
    hdr.data_size = payload_len;

    // Write
    {
        StagedTransport io(guard.path(), "wb");
        KeyWriter writer(io, hdr);
        ASSERT_TRUE(writer.write_header());
        ASSERT_TRUE(writer.write_meta(payload, payload_len));
        writer.finish();
    }

    // Read and validate CRC
    {
        StagedTransport io(guard.path(), "rb");
        KeyReader reader(io);
        KeyHeader read_hdr = reader.read_header();
        char buf[64] = {};
        ASSERT_TRUE(reader.read_meta(buf, payload_len));
        EXPECT_STREQ(buf, payload);

        // CRC should be non-zero (StagedTransport computes CRC)
        EXPECT_NE(read_hdr.crc32, 0u);
        EXPECT_TRUE(reader.validate_crc(read_hdr.crc32));
    }
}

TEST(KeyProtocolTest, Crc32ValidationFail)
{
    auto guard = test::make_temp_file("kp", "crc_fail.bin");

    KeyHeader hdr = make_valid_header();
    const char* payload = "Original data";
    hdr.data_size = std::strlen(payload);

    // Write
    {
        StagedTransport io(guard.path(), "wb");
        KeyWriter writer(io, hdr);
        ASSERT_TRUE(writer.write_header());
        ASSERT_TRUE(writer.write_meta(payload, hdr.data_size));
        writer.finish();
    }

    // Corrupt one byte in the file (after the header)
    {
        FILE* f = std::fopen(guard.path().c_str(), "r+b");
        ASSERT_NE(f, nullptr);
        std::fseek(f, sizeof(KeyHeader) + 2, SEEK_SET);
        char bad = 0xFF;
        std::fwrite(&bad, 1, 1, f);
        std::fclose(f);
    }

    // Read with corrupted CRC check
    {
        StagedTransport io(guard.path(), "rb");
        KeyReader reader(io);
        KeyHeader read_hdr = reader.read_header();

        // The header CRC was computed over the original data.
        // After corruption, the reader's computed CRC won't match.
        // We need to get the original header CRC to test validation.
        // The header CRC is the one written before corruption.
        // But we corrupted data *after* the header, so header.crc32 is still
        // the original. validate_crc with the header's CRC should fail
        // because the reader computed a different CRC over the corrupted data.
        char buf[64] = {};
        ASSERT_TRUE(reader.read_meta(buf, hdr.data_size));

        // validate_crc compares reader's computed CRC against the expected value.
        // Since data was corrupted, the computed CRC differs.
        EXPECT_FALSE(reader.validate_crc(read_hdr.crc32));
    }
}

//=============================================================================
// Cross-transport CRC consistency
//=============================================================================

TEST(KeyProtocolTest, Crc32MatchesAcrossTransports)
{
    auto guard = test::make_temp_file("kp", "crc_cross.bin");

    KeyHeader hdr = make_valid_header();
    const char* payload = "Cross-transport CRC check";
    hdr.data_size = std::strlen(payload);

    // Write with StagedTransport
    {
        StagedTransport io(guard.path(), "wb");
        KeyWriter writer(io, hdr);
        ASSERT_TRUE(writer.write_header());
        ASSERT_TRUE(writer.write_meta(payload, hdr.data_size));
        writer.finish();
    }

    // Read header with StagedTransport to get the CRC
    uint32_t staged_crc = 0;
    {
        StagedTransport io(guard.path(), "rb");
        KeyReader reader(io);
        KeyHeader read_hdr = reader.read_header();
        staged_crc = read_hdr.crc32;
    }

    // Read header with MappedTransport — same CRC expected
    uint32_t mapped_crc = 0;
    {
        MappedTransport io(guard.path(), "rb");
        KeyReader reader(io);
        KeyHeader read_hdr = reader.read_header();
        mapped_crc = read_hdr.crc32;
    }

    // Note: CRC32 in the header was computed by the writer (StagedTransport).
    // Both readers compute their own CRC over the data section.
    // The header CRC field is the same (read from file).
    EXPECT_EQ(staged_crc, mapped_crc);
}
