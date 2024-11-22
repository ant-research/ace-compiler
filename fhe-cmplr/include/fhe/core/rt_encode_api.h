//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CORE_RT_ENCODE_API_H
#define FHE_CORE_RT_ENCODE_API_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define PT_BUFFER_MAGIC "ANTPLAIN"

struct PLAINTEXT_BUFFER {
  char     _magic[8];  // magic, "ANTPLAIN"
  uint32_t _version;   // version, 0x00000001
  uint32_t _size;      // size of plaintext without head and padding
  char     _data[];    // real PLAINTEXT buffer
};

//! @brief create context for encoding only
void Prepare_encode_context(uint32_t degree, uint32_t sec_level, uint32_t depth,
                            uint32_t first_mod_size, uint32_t scaling_mod_size);

//! @brief destroy context for encoding only
void Finalize_encode_context();

//! @brief encode float array and return pointer to PLAINTEXT which can be
//! freed as a whole
struct PLAINTEXT_BUFFER* Encode_plain_buffer(const float* input, size_t len,
                                             uint32_t sc_degree,
                                             uint32_t level);

//! @brief free PLAINTEXT_BUFFER
void Free_plain_buffer(struct PLAINTEXT_BUFFER* buf);

//! @brief get pointer to PLAINTEXT from pt buffer
void* Cast_buffer_to_plain(struct PLAINTEXT_BUFFER* buf);

//! @brief compare two PLAINTEXT BUFFER
bool Compare_plain_buffer(const struct PLAINTEXT_BUFFER* pb_x,
                          const struct PLAINTEXT_BUFFER* pb_y);

//! @brief get max plain buffer length
uint64_t Max_plain_buffer_length();

//! @brief get plain buffer length
static inline uint64_t Plain_buffer_length(struct PLAINTEXT_BUFFER* buf) {
  return sizeof(struct PLAINTEXT_BUFFER) + buf->_size;
}

#ifdef __cplusplus
}
#endif

#endif  // FHE_CORE_RT_ENCODE_API_H
