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

#include "common.h"

#ifdef __cplusplus
extern "C" {
#endif

//! @brief initialize plaintext manager with external file name
bool Pt_mgr_init(const char* fname);

//! @brief finalize plaintext manager
void Pt_mgr_fini();

//! @brief prefetch plaintext from disk to memory
void Pt_prefetch(uint32_t index);

//! @brief get plaintext pointer
void* Pt_get(uint32_t index, size_t len, uint32_t scale, uint32_t level);

//! @brief get plaintext pointer and validate content
void* Pt_get_validate(float* buf, uint32_t index, size_t len, uint32_t scale,
                      uint32_t level);

//! @brief free plaintext by index
void Pt_free(uint32_t index);

//! @brief get message by index
void Pt_from_msg(void* pt, uint32_t index, size_t len, uint32_t scale,
                 uint32_t level);

//! @brief get message by index and validate content
void Pt_from_msg_validate(void* pt, float* buf, uint32_t index, size_t len,
                          uint32_t scale, uint32_t level);

#ifdef __cplusplus
}
#endif

#endif  // RTLIB_COMMON_PT_MGR_H
