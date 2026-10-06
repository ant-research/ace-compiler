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

#ifdef __cplusplus
namespace fhe { namespace core { struct DATA_LUT_ENTRY; } }
#endif

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

//! @brief Get the file descriptor for direct I/O (pread).
//! Returns -1 if the file is not open.
int Rt_data_fd(struct RT_DATA_FILE* file);

//! @brief Get the number of entries in the data file.
uint64_t Rt_data_entry_count(struct RT_DATA_FILE* file);

//! @brief Get a LUT entry by index.
//! Returns a pointer to the entry; valid as long as the file is open.
//! In C++, cast the return value to fhe::core::DATA_LUT_ENTRY*.
void* Rt_data_lut_entry(struct RT_DATA_FILE* file, uint32_t index);

//! @brief Get the scale field of a LUT entry.
//! The entry is identified by index within the data file.
uint32_t Rt_data_lut_scale(struct RT_DATA_FILE* file, uint32_t index);

//! @brief Get the level field of a LUT entry.
uint32_t Rt_data_lut_level(struct RT_DATA_FILE* file, uint32_t index);

//! @brief Get the data size (in bytes) of a LUT entry.
uint64_t Rt_data_lut_size(struct RT_DATA_FILE* file, uint32_t index);

//! @brief Get the data offset (in bytes) of a LUT entry in the file.
uint64_t Rt_data_lut_offset(struct RT_DATA_FILE* file, uint32_t index);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_COMMON_RT_DATA_FILE_H
