//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <fcntl.h>
#include <unistd.h>

#include "common/block_io.h"
#include "gtest/gtest.h"

namespace {

#define NUM_OF_ENTRY 2

TEST(FHERT_COMMON, FILE_IO) {
  const char* data_name = "/tmp/fhefio_test.bin";
  {
    int fd = open(data_name, O_CREAT | O_WRONLY, 0666);
    EXPECT_GE(fd, 0);
    char buf[32];
    for (int i = 0; i < NUM_OF_ENTRY; ++i) {
      memset(buf, 'a' + i, 32);
      ssize_t sz = write(fd, buf, 32);
      EXPECT_EQ(sz, 32);
    }
    close(fd);
  }
  {
    bool sync_read = true;
    bool ret       = Block_io_init(sync_read);
    EXPECT_TRUE(ret);
    BLOCK_INFO blk;
    EXPECT_EQ(posix_memalign((void**)&blk._iovec.iov_base, 4096, 4096), 0);
    blk._iovec.iov_len = 32;
    blk._blk_sts       = BLK_INVALID;
    int fd             = Block_io_open(data_name, sync_read);
    EXPECT_GE(fd, 0);
    char buf_ref[32];
    for (int i = 0; i < NUM_OF_ENTRY; ++i) {
      memset(blk._iovec.iov_base, 0, 32);
      ret = Block_io_prefetch(fd, i * 32, &blk);
      EXPECT_TRUE(ret);
      EXPECT_EQ(blk._blk_sts, BLK_PREFETCHING);
      ret = Block_io_read(fd, i * 32, &blk);
      EXPECT_TRUE(ret);
      EXPECT_EQ(blk._blk_sts, BLK_READY);
      memset(buf_ref, 'a' + i, 32);
      EXPECT_EQ(memcmp(blk._iovec.iov_base, buf_ref, 32), 0);
      blk._blk_sts = BLK_INVALID;
    }
    free(blk._iovec.iov_base);
    Block_io_close(fd);
    Block_io_fini(sync_read);
  }
  unlink(data_name);
}

}  // namespace
