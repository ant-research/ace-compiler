//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_BINARY_TAR_HDR_H
#define AIR_UTIL_BINARY_TAR_HDR_H

#include <array>
#include <cstring>

#include "air/util/debug.h"

namespace air {

namespace util {

//! @brief tar file block size is 512
constexpr int32_t TAR_BLOCK_SIZE = 512;

//! @brief tar file header
class TAR_HDR {
public:
  //! @brief check if it's a UStar header
  bool Is_ustar() const { return memcmp(_ustar_id, "ustar", 5) == 0; }

  //! @brief check if it's a normal file
  bool Is_file() const { return _type == '0' || _type == 0; }

  //! @brief check if it's a directory
  bool Is_dir() const { return _type == '5'; }

  //! @brief check if it's a long file name entry
  bool Has_long_name() const { return _type == 'L'; }

  //! @brief get file size
  size_t File_size() const { return Decode(_filesize, sizeof(_filesize)); }

  //! @brief valiate the checksum
  bool Validate() const {
    uint64_t       checksum = 0;
    const uint8_t* ptr      = (const uint8_t*)this;
    int            pos      = 0;
    for (; pos < sizeof(_filename) + sizeof(_mode) + sizeof(_uid) +
                     sizeof(_gid) + sizeof(_filesize) + sizeof(_last_mod);
         ++pos) {
      checksum += ptr[pos];
    }
    pos += sizeof(_checksum);
    checksum += ' ' * sizeof(_checksum);
    for (; pos < TAR_BLOCK_SIZE; ++pos) {
      checksum += ptr[pos];
    }
    return Decode(_checksum, sizeof(_checksum)) == checksum;
  }

private:
  uint64_t Decode(const char* data, size_t sz) const {
    uint8_t* ptr = (uint8_t*)data + sz - 1;
    // skip tailing spaces
    while (ptr >= (uint8_t*)data) {
      if (*ptr != 0 && *ptr != ' ') {
        break;
      }
      --ptr;
    }
    uint64_t sum   = 0;
    uint64_t scale = 1;
    while (ptr >= (uint8_t*)data) {
      sum += scale * ((*ptr) - '0');
      scale *= 8;
      --ptr;
    }
    return sum;
  }

  char _filename[100];
  char _mode[8];
  char _uid[8];
  char _gid[8];
  char _filesize[12];
  char _last_mod[12];
  char _checksum[8];
  char _type;
  char _linked_filename[100];
  char _ustar_id[6];
  char _ustar_ver[2];
  char _owner_user[32];
  char _owner_group[32];
  char _dev_major[8];
  char _dev_minor[8];
  char _filename_prefix[155];
  char _padding[12];
};

// make sure header size if TAR_BLOCK_SIZE (512 bytes)
AIR_STATIC_ASSERT(sizeof(TAR_HDR) == TAR_BLOCK_SIZE);

}  // namespace util

}  // namespace air

#endif  // AIR_UTIL_BINARY_TAR_HDR_H
