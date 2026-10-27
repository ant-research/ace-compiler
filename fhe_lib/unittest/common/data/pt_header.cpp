//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief test_pt_header.cpp
//! Host-only unit tests for PtHeader validation and constants.
//! No GPU required — these test the PtHeader struct and
//! validate_pt_header() logic only.
//!
//! Round-trip tests (serialize + deserialize via PtIo) require a GPU
//! and are not included here.

#include "fhe/runtime/data/pt_header.h"

#include <cstring>
#include <gtest/gtest.h>

using namespace fhert;

//=============================================================================
// Header validation
//=============================================================================

TEST(PtHeaderTest, HeaderValidationValid)
{
    PtHeader hdr{};
    std::memcpy(hdr.magic, PT_MAGIC_STR, 8);
    hdr.version = PT_FORMAT_VERSION;
    hdr.reserved = 0;
    hdr.header_size = sizeof(PtHeader);
    hdr.poly_modulus_degree = 8192;
    hdr.coeff_modulus_size = 3;
    hdr.chain_index = 2;
    hdr.scale = 65536.0;
    hdr.data_size = 8192 * 3 * sizeof(uint64_t);
    hdr.crc32 = 0;
    hdr.reserved2 = 0;

    EXPECT_TRUE(validate_pt_header(hdr));
}

TEST(PtHeaderTest, HeaderValidationBadMagic)
{
    PtHeader hdr{};
    hdr.magic[0] = 'X';  // wrong magic
    hdr.magic[1] = 'X';
    hdr.version = PT_FORMAT_VERSION;
    hdr.header_size = sizeof(PtHeader);
    hdr.poly_modulus_degree = 8192;
    hdr.coeff_modulus_size = 3;
    hdr.data_size = 100;

    EXPECT_FALSE(validate_pt_header(hdr));
}

TEST(PtHeaderTest, HeaderValidationBadVersion)
{
    PtHeader hdr{};
    std::memcpy(hdr.magic, PT_MAGIC_STR, 8);
    hdr.version = 99;  // wrong version
    hdr.header_size = sizeof(PtHeader);
    hdr.poly_modulus_degree = 8192;
    hdr.coeff_modulus_size = 3;
    hdr.data_size = 100;

    EXPECT_FALSE(validate_pt_header(hdr));
}

TEST(PtHeaderTest, HeaderValidationZeroN)
{
    PtHeader hdr{};
    std::memcpy(hdr.magic, PT_MAGIC_STR, 8);
    hdr.version = PT_FORMAT_VERSION;
    hdr.reserved = 0;
    hdr.header_size = sizeof(PtHeader);
    hdr.poly_modulus_degree = 0;  // invalid
    hdr.coeff_modulus_size = 3;
    hdr.data_size = 100;

    EXPECT_FALSE(validate_pt_header(hdr));
}

TEST(PtHeaderTest, HeaderValidationZeroL)
{
    PtHeader hdr{};
    std::memcpy(hdr.magic, PT_MAGIC_STR, 8);
    hdr.version = PT_FORMAT_VERSION;
    hdr.reserved = 0;
    hdr.header_size = sizeof(PtHeader);
    hdr.poly_modulus_degree = 8192;
    hdr.coeff_modulus_size = 0;  // invalid
    hdr.data_size = 100;

    EXPECT_FALSE(validate_pt_header(hdr));
}

TEST(PtHeaderTest, HeaderValidationZeroDataSize)
{
    PtHeader hdr{};
    std::memcpy(hdr.magic, PT_MAGIC_STR, 8);
    hdr.version = PT_FORMAT_VERSION;
    hdr.reserved = 0;
    hdr.header_size = sizeof(PtHeader);
    hdr.poly_modulus_degree = 8192;
    hdr.coeff_modulus_size = 3;
    hdr.data_size = 0;  // invalid

    EXPECT_FALSE(validate_pt_header(hdr));
}

TEST(PtHeaderTest, HeaderValidationReservedNotZero)
{
    PtHeader hdr{};
    std::memcpy(hdr.magic, PT_MAGIC_STR, 8);
    hdr.version = PT_FORMAT_VERSION;
    hdr.reserved = 1;  // must be 0
    hdr.header_size = sizeof(PtHeader);
    hdr.poly_modulus_degree = 8192;
    hdr.coeff_modulus_size = 3;
    hdr.data_size = 100;

    EXPECT_FALSE(validate_pt_header(hdr));
}

TEST(PtHeaderTest, HeaderValidationReserved2NotZero)
{
    PtHeader hdr{};
    std::memcpy(hdr.magic, PT_MAGIC_STR, 8);
    hdr.version = PT_FORMAT_VERSION;
    hdr.reserved = 0;
    hdr.header_size = sizeof(PtHeader);
    hdr.poly_modulus_degree = 8192;
    hdr.coeff_modulus_size = 3;
    hdr.data_size = 100;
    hdr.reserved2 = 42;  // must be 0

    EXPECT_FALSE(validate_pt_header(hdr));
}

//=============================================================================
// Buffer length / header size
//=============================================================================

TEST(PtHeaderTest, HeaderSizeIs64Bytes)
{
    EXPECT_EQ(sizeof(PtHeader), 64u);
}

TEST(PtHeaderTest, HeaderMagicString)
{
    EXPECT_EQ(PT_MAGIC_STR[0], 'F');
    EXPECT_EQ(PT_MAGIC_STR[1], 'H');
    EXPECT_EQ(PT_MAGIC_STR[2], 'E');
    EXPECT_EQ(PT_MAGIC_STR[3], 'P');
    EXPECT_EQ(PT_MAGIC_STR[4], 'T');
    EXPECT_EQ(PT_MAGIC_STR[5], '\0');
    EXPECT_EQ(PT_MAGIC_STR[6], '\0');
    EXPECT_EQ(PT_MAGIC_STR[7], '\0');
}

TEST(PtHeaderTest, HeaderVersionIsOne)
{
    EXPECT_EQ(PT_FORMAT_VERSION, 1u);
}
