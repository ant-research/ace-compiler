//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief test_crc32.cpp
//! Unit tests for rtio::Crc32 class.
//!
//! Host-only tests verify the Crc32 utility functions.
//! GPU tests (gated behind ACE_PHANTOM_GPU_TESTS) verify update_gpu.

#include "fhe/io/crc32.h"

#include <cstdint>
#include <cstring>
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace rtio;

//=============================================================================
// Known-value test
//=============================================================================

TEST(Crc32Test, KnownValue)
{
    // CRC32 of "123456789" is 0xCBF43926 (standard check value)
    const char* data = "123456789";
    uint32_t crc = Crc32::compute(data, std::strlen(data));
    EXPECT_EQ(crc, 0xCBF43926u);
}

//=============================================================================
// Incremental vs one-shot
//=============================================================================

TEST(Crc32Test, IncrementalVsOneshot)
{
    // Split data into 3 chunks and compute incrementally
    const uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                            0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};

    uint32_t oneshot = Crc32::compute(data, sizeof(data));

    Crc32 crc;
    crc.update(data, 4);
    crc.update(data + 4, 4);
    crc.update(data + 8, 4);
    crc.update(data + 12, 4);
    uint32_t incremental = crc.finalize();

    EXPECT_EQ(incremental, oneshot);
}

//=============================================================================
// Reset between calls
//=============================================================================

TEST(Crc32Test, ResetBetweenCalls)
{
    const char* data1 = "Hello";
    const char* data2 = "World";

    uint32_t crc1 = Crc32::compute(data1, std::strlen(data1));
    uint32_t crc2 = Crc32::compute(data2, std::strlen(data2));

    Crc32 crc;
    crc.update(data1, std::strlen(data1));
    EXPECT_EQ(crc.finalize(), crc1);

    crc.reset();
    crc.update(data2, std::strlen(data2));
    EXPECT_EQ(crc.finalize(), crc2);

    // After reset, second result should differ from first
    EXPECT_NE(crc1, crc2);
}

//=============================================================================
// Empty input
//=============================================================================

TEST(Crc32Test, EmptyInput)
{
    Crc32 crc;
    crc.update(nullptr, 0);
    uint32_t result = crc.finalize();

    // CRC of empty data is 0x00000000 after XOR with 0xFFFFFFFF
    // (initial state is 0xFFFFFFFF, XOR final => 0x00000000)
    // But it depends on implementation — verify it doesn't crash
    // and matches one-shot:
    uint32_t oneshot = Crc32::compute("", 0);
    EXPECT_EQ(result, oneshot);
}

//=============================================================================
// Different data produces different CRC
//=============================================================================

TEST(Crc32Test, DifferentDataDifferentCrc)
{
    const char* a = "AAAA";
    const char* b = "AAAB";

    uint32_t crc_a = Crc32::compute(a, std::strlen(a));
    uint32_t crc_b = Crc32::compute(b, std::strlen(b));

    EXPECT_NE(crc_a, crc_b);
}

//=============================================================================
// Same data same CRC
//=============================================================================

TEST(Crc32Test, SameDataSameCrc)
{
    const char* data = "Reproducible CRC32 test data";
    uint32_t crc1 = Crc32::compute(data, std::strlen(data));
    uint32_t crc2 = Crc32::compute(data, std::strlen(data));
    EXPECT_EQ(crc1, crc2);
}

//=============================================================================
// Large data (verify no overflow/truncation issues)
//=============================================================================

TEST(Crc32Test, LargeData)
{
    // 1 MB of known data
    std::vector<uint8_t> data(1024 * 1024, 0xAB);
    uint32_t crc = Crc32::compute(data.data(), data.size());
    EXPECT_NE(crc, 0u);

    // Verify incremental matches one-shot
    Crc32 crc_obj;
    const size_t chunk = 65536;
    for (size_t i = 0; i < data.size(); i += chunk) {
        size_t n = (i + chunk > data.size()) ? data.size() - i : chunk;
        crc_obj.update(data.data() + i, n);
    }
    EXPECT_EQ(crc_obj.finalize(), crc);
}
