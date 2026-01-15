//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_COMMON_PT_MGR_H
#define RTLIB_COMMON_PT_MGR_H

//! @brief pt_mgr.h
//! define API to manage buffer for plaintext
//! @note To avoid undefined reference issues, ensure that `pt_mgr.c` is
//! included in the `libFHErt_xxx.a`. If other libraries require their own
//! implementation, they should include `pt_mgr.c` within their own library.

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @brief Initialize plaintext manager with external file name
bool Pt_mgr_init(const char* fname);

//! @brief Finalize plaintext manager
void Pt_mgr_fini();

//! @brief Prefetch plaintext from disk to memory
void Pt_prefetch(uint32_t index);

//! @brief Get plaintext pointer
void* Pt_get(uint32_t index, size_t len, uint32_t scale, uint32_t level);

//! @brief Get plaintext pointer and validate content
void* Pt_get_validate(float* buf, uint32_t index, size_t len, uint32_t scale,
                      uint32_t level);

//! @brief Free plaintext by index
void Pt_free(uint32_t index);

//! @brief Free plaintext poly data
void Free_data(void* poly);

//! @brief Get message by index
void* Pt_from_msg(void* pt, uint32_t index, size_t len, uint32_t scale,
                  uint32_t level);

//! @brief Get message by index and offset
void* Pt_from_msg_ofst(void* pt, uint32_t index, size_t ofst, size_t len,
                       uint32_t scale, uint32_t level);

//! @brief Get message by index and validate content
void Pt_from_msg_validate(void* pt, float* buf, uint32_t index, size_t len,
                          uint32_t scale, uint32_t level);

//! @brief Get message by index/offset and validate content
void Pt_from_msg_ofst_validate(void* pt, float* buf, uint32_t index,
                               size_t offset, size_t len, uint32_t scale,
                               uint32_t level);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_COMMON_PT_MGR_H
