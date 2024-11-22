//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_COMMON_RT_DATA_FILE_H
#define RTLIB_COMMON_RT_DATA_FILE_H

#include <stdint.h>

#include "common/block_io.h"

//! @brief rt_data_reader.h
//! define API to read a runtime data file

#ifdef __cplusplus
extern "C" {
#endif

struct RT_DATA_FILE;

struct RT_DATA_FILE* Rt_data_open(const char* fname, bool sync_read);
void                 Rt_data_close(struct RT_DATA_FILE* file);

bool  Rt_data_prefetch(struct RT_DATA_FILE* file, uint32_t index,
                       BLOCK_INFO* blk, bool sync_read);
void* Rt_data_read(struct RT_DATA_FILE* file, uint32_t index, BLOCK_INFO* blk,
                   bool sync_read);

bool Rt_data_fill(struct RT_DATA_FILE* file, void* buf, uint64_t sz);

bool Rt_data_is_plaintext(struct RT_DATA_FILE* file);

uint64_t Rt_data_size(struct RT_DATA_FILE* file);

uint64_t Rt_data_entry_offset(struct RT_DATA_FILE* file, uint32_t index,
                              uint64_t size);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_COMMON_RT_DATA_FILE_H
