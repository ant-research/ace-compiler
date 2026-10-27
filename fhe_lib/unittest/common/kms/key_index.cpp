//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief test_key_index.cpp
//! Host-only unit tests for GaloisKeyIndex JSON serialization.

#include "fhe/kms/key_index.h"

#include "test_util.h"

#include <cstdint>
#include <cstring>
#include <gtest/gtest.h>
#include <string>
#include <vector>

using namespace fhert;
using rtio::test::make_temp_file;
using rtio::test::TempFileGuard;

//=============================================================================
// Write/Read round-trip
//=============================================================================

TEST(KeyIndexTest, WriteReadRoundTrip)
{
    auto guard = make_temp_file("ki", "index.json");

    GaloisKeyIndex idx;
    idx.version = 1;
    idx.poly_modulus_degree = 16384;
    idx.coeff_modulus_size = 37;
    idx.key_component_bytes = 2 * 16384 * 37 * sizeof(uint64_t);
    idx.galois_elts = {3, 5, 7, 9, 11};
    idx.disk_paths = {"/mnt/nvme0", "/mnt/nvme1"};

    idx.relin_keys.resize(5);
    for (int i = 0; i < 5; i++) {
        idx.relin_keys[i].file = "galois_key." + std::to_string(i) + ".bin";
        idx.relin_keys[i].disk_index = i % 2;
        idx.relin_keys[i].offset = 0;
        idx.relin_keys[i].size = idx.key_component_bytes;
        idx.relin_keys[i].nk = 2;
        idx.relin_keys[i].crc32 = 0xDEADBEEFu;
    }

    ASSERT_TRUE(write_galois_key_index(guard.path(), idx));

    GaloisKeyIndex read_idx;
    ASSERT_TRUE(read_galois_key_index(guard.path(), read_idx));

    EXPECT_EQ(read_idx.version, 1u);
    EXPECT_EQ(read_idx.poly_modulus_degree, 16384u);
    EXPECT_EQ(read_idx.coeff_modulus_size, 37u);
    EXPECT_EQ(read_idx.key_component_bytes, idx.key_component_bytes);
    EXPECT_EQ(read_idx.galois_elts.size(), 5u);
    EXPECT_EQ(read_idx.galois_elts[0], 3u);
    EXPECT_EQ(read_idx.galois_elts[4], 11u);
    EXPECT_EQ(read_idx.disk_paths.size(), 2u);
    EXPECT_EQ(read_idx.disk_paths[0], "/mnt/nvme0");
    EXPECT_EQ(read_idx.disk_paths[1], "/mnt/nvme1");
    EXPECT_EQ(read_idx.relin_keys.size(), 5u);
    EXPECT_EQ(read_idx.relin_keys[2].file, "galois_key.2.bin");
    EXPECT_EQ(read_idx.relin_keys[2].disk_index, 0);
    EXPECT_EQ(read_idx.relin_keys[2].crc32, 0xDEADBEEFu);
}

//=============================================================================
// Per-key format (version 1)
//=============================================================================

TEST(KeyIndexTest, PerKeyFormat)
{
    auto guard = make_temp_file("ki", "perkey.json");

    GaloisKeyIndex idx;
    idx.version = 1;
    idx.poly_modulus_degree = 8192;
    idx.coeff_modulus_size = 20;
    idx.galois_elts = {1, 3};
    idx.relin_keys.resize(2);
    idx.relin_keys[0].file = "galois_key.00.bin";
    idx.relin_keys[0].offset = 0;
    idx.relin_keys[0].size = 1024;
    idx.relin_keys[1].file = "galois_key.01.bin";
    idx.relin_keys[1].offset = 0;
    idx.relin_keys[1].size = 1024;

    ASSERT_TRUE(write_galois_key_index(guard.path(), idx));

    GaloisKeyIndex read_idx;
    ASSERT_TRUE(read_galois_key_index(guard.path(), read_idx));
    EXPECT_EQ(read_idx.version, 1u);
    EXPECT_EQ(read_idx.relin_keys[0].offset, 0u);
    EXPECT_EQ(read_idx.relin_keys[1].offset, 0u);
}

//=============================================================================
// Merged format (version 2)
//=============================================================================

TEST(KeyIndexTest, MergedFormat)
{
    auto guard = make_temp_file("ki", "merged.json");

    GaloisKeyIndex idx;
    idx.version = 2;
    idx.poly_modulus_degree = 65536;
    idx.coeff_modulus_size = 37;
    idx.key_component_bytes = 2 * 65536 * 37 * 8;
    idx.relin_keys.resize(3);
    for (int i = 0; i < 3; i++) {
        idx.relin_keys[i].file = "galois_key.merged.bin";
        idx.relin_keys[i].offset = i * idx.key_component_bytes;
        idx.relin_keys[i].size = idx.key_component_bytes;
    }

    ASSERT_TRUE(write_galois_key_index(guard.path(), idx));

    GaloisKeyIndex read_idx;
    ASSERT_TRUE(read_galois_key_index(guard.path(), read_idx));
    EXPECT_EQ(read_idx.version, 2u);
    EXPECT_EQ(read_idx.relin_keys[0].offset, 0u);
    EXPECT_GT(read_idx.relin_keys[1].offset, 0u);
    EXPECT_GT(read_idx.relin_keys[2].offset, read_idx.relin_keys[1].offset);
    EXPECT_EQ(read_idx.relin_keys[0].file, "galois_key.merged.bin");
}

//=============================================================================
// Multi-disk paths
//=============================================================================

TEST(KeyIndexTest, MultiDiskPaths)
{
    auto guard = make_temp_file("ki", "multidisk.json");

    GaloisKeyIndex idx;
    idx.version = 1;
    idx.poly_modulus_degree = 16384;
    idx.coeff_modulus_size = 37;
    idx.disk_paths = {"/nvme0", "/nvme1", "/nvme2", "/nvme3"};
    // Index requires at least one relin_key entry
    idx.relin_keys.resize(1);
    idx.relin_keys[0].file = "galois_key.00.bin";
    idx.relin_keys[0].size = 1024;

    ASSERT_TRUE(write_galois_key_index(guard.path(), idx));

    GaloisKeyIndex read_idx;
    ASSERT_TRUE(read_galois_key_index(guard.path(), read_idx));
    EXPECT_EQ(read_idx.disk_paths.size(), 4u);
    EXPECT_EQ(read_idx.disk_paths[2], "/nvme2");
}

//=============================================================================
// Read nonexistent file
//=============================================================================

TEST(KeyIndexTest, ReadNonexistentFile)
{
    std::string path = rtio::test::make_temp_path("ki", "nonexistent.json");
    rtio::test::remove_file(path);

    GaloisKeyIndex idx;
    EXPECT_FALSE(read_galois_key_index(path, idx));
}

//=============================================================================
// Minimal valid index
//=============================================================================

TEST(KeyIndexTest, MinimalIndex)
{
    auto guard = make_temp_file("ki", "minimal.json");

    GaloisKeyIndex idx;
    idx.version = 1;
    idx.poly_modulus_degree = 4096;
    idx.coeff_modulus_size = 20;
    idx.relin_keys.resize(1);
    idx.relin_keys[0].file = "galois_key.00.bin";
    idx.relin_keys[0].size = 512;

    ASSERT_TRUE(write_galois_key_index(guard.path(), idx));

    GaloisKeyIndex read_idx;
    ASSERT_TRUE(read_galois_key_index(guard.path(), read_idx));
    EXPECT_EQ(read_idx.relin_keys.size(), 1u);
    EXPECT_EQ(read_idx.relin_keys[0].file, "galois_key.00.bin");
}

//=============================================================================
// Multi-file per-disk format (version 3) — with file_slot
//=============================================================================

TEST(KeyIndexTest, MultiFileSlotFormat)
{
    auto guard = make_temp_file("ki", "v3_slot.json");

    GaloisKeyIndex idx;
    idx.version = 3;
    idx.poly_modulus_degree = 65536;
    idx.coeff_modulus_size = 37;
    idx.key_component_bytes = 2 * 65536 * 37 * 8;
    idx.disk_paths = {"/mnt/nvme0/kms_4d/model", "/mnt/nvme1/kms_4d/model", "/mnt/nvme2/kms_4d/model",
                      "/mnt/nvme3/kms_4d/model"};
    idx.galois_elts = {3, 5, 7};

    // 2 disks × 2 slots = 4 entries
    idx.relin_keys.resize(4);
    for (int d = 0; d < 2; d++) {
        for (int s = 0; s < 2; s++) {
            int i = d * 2 + s;
            idx.relin_keys[i].file = "galois_key.merged." + std::to_string(d) + "." + std::to_string(s) + ".bin";
            idx.relin_keys[i].disk_index = d;
            idx.relin_keys[i].file_slot = s;
            idx.relin_keys[i].offset = 0;
            idx.relin_keys[i].size = idx.key_component_bytes;
            idx.relin_keys[i].nk = 2;
            idx.relin_keys[i].crc32 = 0;
        }
    }

    ASSERT_TRUE(write_galois_key_index(guard.path(), idx));

    GaloisKeyIndex read_idx;
    ASSERT_TRUE(read_galois_key_index(guard.path(), read_idx));
    EXPECT_EQ(read_idx.version, 3u);
    EXPECT_EQ(read_idx.disk_paths.size(), 4u);
    EXPECT_EQ(read_idx.relin_keys.size(), 4u);

    // Verify disk_index and file_slot round-trip correctly
    EXPECT_EQ(read_idx.relin_keys[0].disk_index, 0);
    EXPECT_EQ(read_idx.relin_keys[0].file_slot, 0);
    EXPECT_EQ(read_idx.relin_keys[1].disk_index, 0);
    EXPECT_EQ(read_idx.relin_keys[1].file_slot, 1);
    EXPECT_EQ(read_idx.relin_keys[2].disk_index, 1);
    EXPECT_EQ(read_idx.relin_keys[2].file_slot, 0);
    EXPECT_EQ(read_idx.relin_keys[3].disk_index, 1);
    EXPECT_EQ(read_idx.relin_keys[3].file_slot, 1);

    // Verify file name preserved
    EXPECT_EQ(read_idx.relin_keys[2].file, "galois_key.merged.1.0.bin");
}
