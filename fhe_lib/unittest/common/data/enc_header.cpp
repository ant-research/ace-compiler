//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief test_pt_header.cpp
//! Host-only unit tests for EncHeader validation and constants.
//! No GPU required — these test the EncHeader struct and
//! validate_enc_header() logic only.
//!
//! Round-trip tests (serialize + deserialize via EncIo) require a GPU
//! and are not included here.

#include "fhe/enc/enc_header.h"

#include <cstring>
#include <gtest/gtest.h>

using namespace fhert;

//=============================================================================
// Header validation
//=============================================================================

TEST(EncHeaderTest, HeaderValidationValid)
{
    EncHeader hdr{};
    std::memcpy(hdr.magic, ENC_MAGIC_STR, 8);
    hdr.version = ENC_FORMAT_VERSION;
    hdr.reserved = 0;
    hdr.header_size = sizeof(EncHeader);
    hdr.poly_modulus_degree = 8192;
    hdr.coeff_modulus_size = 3;
    hdr.chain_index = 2;
    hdr.scale = 65536.0;
    hdr.data_size = 8192 * 3 * sizeof(uint64_t);
    hdr.crc32 = 0;
    hdr.reserved2 = 0;

    EXPECT_TRUE(validate_enc_header(hdr));
}

TEST(EncHeaderTest, HeaderValidationBadMagic)
{
    EncHeader hdr{};
    hdr.magic[0] = 'X';  // wrong magic
    hdr.magic[1] = 'X';
    hdr.version = ENC_FORMAT_VERSION;
    hdr.header_size = sizeof(EncHeader);
    hdr.poly_modulus_degree = 8192;
    hdr.coeff_modulus_size = 3;
    hdr.data_size = 100;

    EXPECT_FALSE(validate_enc_header(hdr));
}

TEST(EncHeaderTest, HeaderValidationBadVersion)
{
    EncHeader hdr{};
    std::memcpy(hdr.magic, ENC_MAGIC_STR, 8);
    hdr.version = 99;  // wrong version
    hdr.header_size = sizeof(EncHeader);
    hdr.poly_modulus_degree = 8192;
    hdr.coeff_modulus_size = 3;
    hdr.data_size = 100;

    EXPECT_FALSE(validate_enc_header(hdr));
}

TEST(EncHeaderTest, HeaderValidationZeroN)
{
    EncHeader hdr{};
    std::memcpy(hdr.magic, ENC_MAGIC_STR, 8);
    hdr.version = ENC_FORMAT_VERSION;
    hdr.reserved = 0;
    hdr.header_size = sizeof(EncHeader);
    hdr.poly_modulus_degree = 0;  // invalid
    hdr.coeff_modulus_size = 3;
    hdr.data_size = 100;

    EXPECT_FALSE(validate_enc_header(hdr));
}

TEST(EncHeaderTest, HeaderValidationZeroL)
{
    EncHeader hdr{};
    std::memcpy(hdr.magic, ENC_MAGIC_STR, 8);
    hdr.version = ENC_FORMAT_VERSION;
    hdr.reserved = 0;
    hdr.header_size = sizeof(EncHeader);
    hdr.poly_modulus_degree = 8192;
    hdr.coeff_modulus_size = 0;  // invalid
    hdr.data_size = 100;

    EXPECT_FALSE(validate_enc_header(hdr));
}

TEST(EncHeaderTest, HeaderValidationZeroDataSize)
{
    EncHeader hdr{};
    std::memcpy(hdr.magic, ENC_MAGIC_STR, 8);
    hdr.version = ENC_FORMAT_VERSION;
    hdr.reserved = 0;
    hdr.header_size = sizeof(EncHeader);
    hdr.poly_modulus_degree = 8192;
    hdr.coeff_modulus_size = 3;
    hdr.data_size = 0;  // invalid

    EXPECT_FALSE(validate_enc_header(hdr));
}

TEST(EncHeaderTest, HeaderValidationReservedNotZero)
{
    EncHeader hdr{};
    std::memcpy(hdr.magic, ENC_MAGIC_STR, 8);
    hdr.version = ENC_FORMAT_VERSION;
    hdr.reserved = 1;  // must be 0
    hdr.header_size = sizeof(EncHeader);
    hdr.poly_modulus_degree = 8192;
    hdr.coeff_modulus_size = 3;
    hdr.data_size = 100;

    EXPECT_FALSE(validate_enc_header(hdr));
}

TEST(EncHeaderTest, HeaderValidationReserved2NotZero)
{
    EncHeader hdr{};
    std::memcpy(hdr.magic, ENC_MAGIC_STR, 8);
    hdr.version = ENC_FORMAT_VERSION;
    hdr.reserved = 0;
    hdr.header_size = sizeof(EncHeader);
    hdr.poly_modulus_degree = 8192;
    hdr.coeff_modulus_size = 3;
    hdr.data_size = 100;
    hdr.reserved2 = 42;  // must be 0

    EXPECT_FALSE(validate_enc_header(hdr));
}

//=============================================================================
// Buffer length / header size
//=============================================================================

TEST(EncHeaderTest, HeaderSizeIs64Bytes)
{
    EXPECT_EQ(sizeof(EncHeader), 64u);
}

TEST(EncHeaderTest, HeaderMagicString)
{
    EXPECT_EQ(ENC_MAGIC_STR[0], 'F');
    EXPECT_EQ(ENC_MAGIC_STR[1], 'H');
    EXPECT_EQ(ENC_MAGIC_STR[2], 'E');
    EXPECT_EQ(ENC_MAGIC_STR[3], 'P');
    EXPECT_EQ(ENC_MAGIC_STR[4], 'T');
    EXPECT_EQ(ENC_MAGIC_STR[5], '\0');
    EXPECT_EQ(ENC_MAGIC_STR[6], '\0');
    EXPECT_EQ(ENC_MAGIC_STR[7], '\0');
}

TEST(EncHeaderTest, HeaderVersionIsOne)
{
    EXPECT_EQ(ENC_FORMAT_VERSION, 1u);
}
