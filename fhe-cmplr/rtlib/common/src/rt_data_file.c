//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "common/rt_data_file.h"

#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "common/common.h"
#include "common/error.h"
#include "common/rt_env.h"
#include "fhe/core/rt_data_def.h"

struct RT_DATA_FILE {
  struct DATA_FILE_HDR   _hdr;
  struct DATA_LUT_ENTRY* _lut;
  int                    _fd;
};

struct RT_DATA_FILE* Rt_data_open(const char* fname, bool sync_read) {
  struct RT_DATA_FILE* file =
      (struct RT_DATA_FILE*)malloc(sizeof(struct RT_DATA_FILE));
  IS_TRUE(file != NULL, "failed to malloc memory for RT_DATA_FILE");
  file->_fd = Block_io_open(fname, sync_read);
  if (file->_fd == -1) {
    IS_TRUE(file->_fd != -1, "failed to open rt data file");
    free(file);
    return NULL;
  }
  ssize_t ret = pread(file->_fd, &file->_hdr, sizeof(struct DATA_FILE_HDR), 0);
  if (ret != sizeof(struct DATA_FILE_HDR)) {
    IS_TRUE(ret == sizeof(struct DATA_FILE_HDR),
            "failed to read rt data file header");
    free(file);
    return NULL;
  }
  uint64_t lut_size = sizeof(struct DATA_LUT_ENTRY) * file->_hdr._ent_count;
  file->_lut        = (struct DATA_LUT_ENTRY*)malloc(lut_size);
  IS_TRUE(file->_lut != NULL, "failed to malloc memory for LUT");
  ret = pread(file->_fd, file->_lut, lut_size, file->_hdr._lut_ofst);
  if (ret != lut_size) {
    IS_TRUE(ret == lut_size, "failed to read rt data file lookup table");
    free(file);
    return NULL;
  }
  return file;
}

void Rt_data_close(struct RT_DATA_FILE* file) {
  Block_io_close(file->_fd);
  free(file->_lut);
  free(file);
}

bool Rt_data_prefetch(struct RT_DATA_FILE* file, uint32_t index,
                      BLOCK_INFO* blk, bool sync_read) {
  IS_TRUE(file->_hdr._ent_type == DE_PLAINTEXT, "bad entry type");
  if (index >= file->_hdr._ent_count) return true;
  IS_TRUE(index < file->_hdr._ent_count, "index out of entry range");
  struct DATA_LUT_ENTRY* lut = &(file->_lut[index]);
  IS_TRUE(blk->_iovec.iov_len >= lut->_size, "buffer too small");
  IS_TRUE(blk->_blk_idx == index, "block index mismatch");
  if (sync_read) {
    blk->_blk_sts = BLK_PREFETCHING;
    ssize_t ret =
        pread(file->_fd, blk->_iovec.iov_base, lut->_size, lut->_ent_ofst);
    IS_TRUE(ret == lut->_size, "failed to read rt data entry");
    blk->_blk_sts = BLK_READY;
    return (ret == lut->_size);
  } else {
    blk->_iovec.iov_len = lut->_size;
    return Block_io_prefetch(file->_fd, lut->_ent_ofst, blk);
  }
}

void* Rt_data_read(struct RT_DATA_FILE* file, uint32_t index, BLOCK_INFO* blk,
                   bool sync_read) {
  IS_TRUE(file->_hdr._ent_type == DE_PLAINTEXT, "bad entry type");
  if (blk->_blk_sts == BLK_READY) {
    return blk->_iovec.iov_base;
  }
  if (sync_read) {
    IS_TRUE(false, "sync_read but not prefetched.");
    return NULL;
  } else {
    IS_TRUE(index < file->_hdr._ent_count, "index out of entry range");
    struct DATA_LUT_ENTRY* lut = &(file->_lut[index]);
    IS_TRUE(blk->_iovec.iov_len >= lut->_size, "buffer too small");
    IS_TRUE(blk->_blk_idx == index, "block index mismatch");
    blk->_iovec.iov_len = lut->_size;
    bool ret            = Block_io_read(file->_fd, lut->_ent_ofst, blk);
    IS_TRUE(ret == true, "failed to read block");
    return blk->_iovec.iov_base;
  }
}

bool Rt_data_fill(struct RT_DATA_FILE* file, void* buf, uint64_t sz) {
  IS_TRUE(file->_hdr._ent_type != DE_PLAINTEXT, "bad entry type");
  ssize_t ret = pread(file->_fd, buf, sz, DATA_FILE_PAGE_SIZE);
  IS_TRUE(ret == sz, "failed to fill data from file");
  return ret == sz;
}

bool Rt_data_is_plaintext(struct RT_DATA_FILE* file) {
  return file->_hdr._ent_type == DE_PLAINTEXT;
}

uint64_t Rt_data_size(struct RT_DATA_FILE* file) {
  return file->_hdr._lut_ofst - DATA_FILE_PAGE_SIZE;
}

uint64_t Rt_data_entry_offset(struct RT_DATA_FILE* file, uint32_t index,
                              uint64_t size) {
  IS_TRUE(file->_hdr._ent_type != DE_PLAINTEXT, "bad entry type");
  IS_TRUE(index < file->_hdr._ent_count, "index out of entry range");
  IS_TRUE(file->_lut[index]._size >= size, "entry size too small");
  uint64_t ofst = file->_lut[index]._ent_ofst;
  IS_TRUE(ofst >= DATA_FILE_PAGE_SIZE, "entry offset too small");
  return ofst - DATA_FILE_PAGE_SIZE;
}
