//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "common/pt_mgr.h"
#include "common/rt_data_file.h"
#include "fhe/core/rt_data_writer.h"
#include "fhe/core/rt_encode_api.h"
#include "gtest/gtest.h"

namespace {

#define NUM_OF_ENTRY 16

TEST(FHERT_COMMON, RT_DATA_FILE) {
  const char* data_name  = "/tmp/fhept_test.bin";
  const char* data_uuid  = "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX";
  const char* model_name = "dummy.onnx";
  char*       pt_buf[NUM_OF_ENTRY];
  {
    fhe::core::DATA_ENTRY_TYPE ent_type = fhe::core::DE_PLAINTEXT;
    fhe::core::RT_DATA_WRITER  writter(data_name, ent_type, model_name,
                                       data_uuid);
    char                       ent_name[32];
    for (uint32_t i = 0; i < NUM_OF_ENTRY; ++i) {
      pt_buf[i] = (char*)malloc(16);
      memset(pt_buf[i], i, 16);
      snprintf(ent_name, 32, "ent_%d", i);
      writter.Append_pt(ent_name, pt_buf[i], 16);
    }
  }
  {
    bool sync_read = true;
    bool ret       = Block_io_init(sync_read);
    EXPECT_TRUE(ret);
    BLOCK_INFO blk;
    blk._iovec.iov_base    = (char*)malloc(16);
    blk._iovec.iov_len     = 16;
    struct RT_DATA_FILE* f = Rt_data_open(data_name, sync_read);
    EXPECT_TRUE(f != NULL);
    for (uint32_t i = 0; i < NUM_OF_ENTRY; ++i) {
      blk._blk_idx = i;
      blk._blk_sts = BLK_INVALID;
      Rt_data_prefetch(f, i, &blk, sync_read);
      void* buf = Rt_data_read(f, i, &blk, sync_read);
      EXPECT_EQ(blk._blk_sts, BLK_READY);
      EXPECT_EQ(memcmp(buf, pt_buf[i], 16), 0);
      free(pt_buf[i]);
    }
    free(blk._iovec.iov_base);
    Rt_data_close(f);
    Block_io_fini(sync_read);
  }
  unlink(data_name);
}

TEST(FHERT_COMMON, PT_MGR) {
  const char* data_name      = "/tmp/fhept_test.bin";
  const char* data_uuid      = "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX";
  const char* model_name     = "dummy.onnx";
  uint32_t    ring_degree    = 4096;
  uint32_t    sec_level      = 0;
  uint32_t    mul_depth      = 8;
  uint32_t    first_mod_size = 53;
  uint32_t    scale_mod_size = 50;
  Prepare_encode_context(ring_degree, sec_level, mul_depth, first_mod_size,
                         scale_mod_size);
  PLAINTEXT_BUFFER* pt_buf[NUM_OF_ENTRY];
  {
    fhe::core::DATA_ENTRY_TYPE ent_type = fhe::core::DE_PLAINTEXT;
    fhe::core::RT_DATA_WRITER  writter(data_name, ent_type, model_name,
                                       data_uuid);
    char                       ent_name[32];
    float                      msg_buf[128];
    for (uint32_t i = 0; i < NUM_OF_ENTRY; ++i) {
      for (uint32_t j = 0; j < 128; j++) {
        msg_buf[j] = (float)i;
      }
      snprintf(ent_name, 32, "ent_%d", i);
      pt_buf[i]    = Encode_plain_buffer(msg_buf, 128, 1, 0);
      uint64_t idx = writter.Append_pt(ent_name, (const char*)pt_buf[i],
                                       Plain_buffer_length(pt_buf[i]));
      EXPECT_EQ(idx, i);
    }
  }
  {
    bool ret = Pt_mgr_init(data_name);
    EXPECT_TRUE(ret);
    for (uint32_t i = 0; i < NUM_OF_ENTRY; ++i) {
      void*                    pt = Pt_get(i, 128, 1, 0);
      struct PLAINTEXT_BUFFER* pb =
          (struct PLAINTEXT_BUFFER*)((char*)pt -
                                     sizeof(struct PLAINTEXT_BUFFER));
      EXPECT_TRUE(Compare_plain_buffer(pb, pt_buf[i]));
      Free_plain_buffer(pt_buf[i]);
    }
    Pt_mgr_fini();
  }
  Finalize_encode_context();
  unlink(data_name);
}

}  // namespace
