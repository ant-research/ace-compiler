//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/core/rt_data_util.h"
#include "fhe/core/rt_data_writer.h"
#include "gtest/gtest.h"

using namespace fhe::core;

namespace {

TEST(RT_DATA_WRITER, INIT_FINI) {
  const char* model = "dummy.onnx";
  const char* uuid  = "XXXX-XXXX-XXXX-XXXX";
  const char* name  = "/tmp/fhe_core_rt_data_writer";
  { fhe::core::RT_DATA_WRITER w_ofs(name, DE_MSG_F32, model, uuid); }
  unlink(name);
}

TEST(RT_DATA_WRITER, WRITE_READ_DUMP) {
  const char* model   = "dummy.onnx";
  const char* uuid    = "XXXX-XXXX-XXXX-XXXX";
  const char* name    = "/tmp/fhe_core_rt_data_writer";
  float       cst_0[] = {0.1,  1.1,  2.1,  3.1,  4.1,  5.1,  6.1,  7.1,
                         8.1,  9.1,  10.1, 11.1, 12.1, 13.1, 14.1, 15.1,
                         16.1, 17.1, 18.1, 19.1, 20.1, 21.1, 22.1, 23.1};
  {
    fhe::core::RT_DATA_WRITER w_ofs(name, DE_MSG_F32, model, uuid);
    w_ofs.Append("cst_0", &cst_0[0], 3);
    w_ofs.Append("cst_1", &cst_0[1], 4);
    w_ofs.Append("cst_2", &cst_0[2], 5);
    w_ofs.Append("cst_3", &cst_0[3], 6);
    w_ofs.Append("cst_4", &cst_0[4], 7);
    w_ofs.Append("cst_5", &cst_0[5], 8);
    w_ofs.Append("cst_6", &cst_0[6], 9);
  }
  {
    std::ifstream             ifs(name, std::ios::binary);
    fhe::core::RT_DATA_READER reader(ifs);
    fhe::core::DATA_FILE_HDR  hdr;
    EXPECT_TRUE(reader.Read_hdr(hdr));
    std::vector<fhe::core::DATA_LUT_ENTRY> lut;
    EXPECT_TRUE(reader.Read_lut(lut));
    EXPECT_EQ(memcmp(hdr._magic, DATA_FILE_MAGIC, sizeof(hdr._magic)), 0);
    EXPECT_EQ(hdr._rt_ver, RT_VERSION_FULL);
    EXPECT_EQ(hdr._ent_type, DE_MSG_F32);
    EXPECT_EQ(hdr._ent_align, 5);
    EXPECT_EQ(hdr._ent_count, 7);
    EXPECT_EQ(hdr._ent_count, lut.size());
    EXPECT_EQ(strcmp(hdr._uuid, uuid), 0);
    for (uint64_t i = 0; i < lut.size(); ++i) {
      EXPECT_EQ(memcmp(lut[i]._name, "cst_", 4), 0);
      EXPECT_EQ(lut[i]._index, i);
      EXPECT_EQ(lut[i]._size, (i + 3) * sizeof(float));
      EXPECT_EQ(lut[i]._ent_ofst % (1 << hdr._ent_align), 0);
      float res[32];
      EXPECT_TRUE(reader.Read_entry(lut[i], (char*)res));
      EXPECT_EQ(memcmp((char*)res, (char*)&cst_0[i], lut[i]._size), 0);
    }
  }
  {
    fhe::core::RT_DATA_DUMPER dumper(std::cout);
    dumper.Dump(name);
  }
  unlink(name);
}

}  // namespace
