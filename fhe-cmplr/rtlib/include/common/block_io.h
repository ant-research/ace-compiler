//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_COMMON_BLOCK_IO_H
#define RTLIB_COMMON_BLOCK_IO_H

//! @brief block_io.h
//! Define API for block I/O from file

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/uio.h>

#ifdef __cplusplus
extern "C" {
#endif

//! @brief block state in memory
typedef enum {
  BLK_INVALID,      //!< invalid state
  BLK_PREFETCHING,  //!< prefetching state
  BLK_READY,        //!< prefetch done, ready state
} BLK_STATUS;

typedef struct {
  uint32_t     _blk_idx;   //!< block index in file
  uint16_t     _blk_sts;   //!< block status
  uint16_t     _mem_next;  //!< next block index in memory
  struct iovec _iovec;     //!< address and size
} BLOCK_INFO;

//! @brief initialize block I/O context
bool Block_io_init(bool async);

//! @brief finalize block I/O context
void Block_io_fini(bool sync_read);

//! @brief open a file for I/O
int Block_io_open(const char* fname, bool sync_read);

//! @brief close a file
void Block_io_close(int fd);

//! @brief prefetch block into buffer
bool Block_io_prefetch(int fd, uint64_t ofst, BLOCK_INFO* blk);

//! @brief make sure reading is done
bool Block_io_read(int fd, uint64_t ofst, BLOCK_INFO* blk);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_COMMON_BLOCK_IO_H
