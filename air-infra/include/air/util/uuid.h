//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_UUID_H
#define AIR_UTIL_UUID_H

#include <stdint.h>
#include <string.h>

#include <random>
#include <string>

#include "air/util/debug.h"

namespace air {

namespace util {

//! @brief Universally-unique identifier (UUID) utility
//! Note: this implementation doesn't conform to RFC-4122. All fields are just
//! filled by random numbers.
class UUID {
public:
  UUID() {
    std::random_device              rd;
    std::mt19937_64                 rnd(rd());
    std::uniform_int_distribution<> dist(0, 255);
    for (uint32_t i = 0; i < sizeof(_uuid); ++i) {
      _uuid[i] = dist(rnd);
    }
  }

  UUID(const UUID& id) { memcpy(_uuid, id._uuid, sizeof(_uuid)); }

  UUID(const char* str) { Parse_str(str); }

  UUID(const std::string& str) { Parse_str(str.c_str()); }

  UUID& operator=(const UUID& id) {
    memcpy(_uuid, id._uuid, sizeof(_uuid));
    return *this;
  }

  std::string To_str() const {
    std::string str;
    str.resize(37);  // include last ending '\0';
    char* buf = str.data();
    for (uint32_t i = 0; i < sizeof(_uuid); ++i) {
      *(buf++) = To_str(_uuid[i] >> 4);
      *(buf++) = To_str(_uuid[i] & 0xF);
      if (i == 3 || i == 5 || i == 7 || i == 9) {
        *(buf++) = '-';
      }
    }
    AIR_ASSERT(buf == str.data() + 36);
    *buf = '\0';
    return str;
  }

private:
  void Parse_str(const char* str) {
    uint32_t len = strlen(str);
    AIR_ASSERT(len == 36);  // 32 digits + 4 dashes
    for (uint32_t i = 0; i < sizeof(_uuid); ++i) {
      _uuid[i] = (To_uint(str) << 4) + To_uint(str + 1);
      str += 2;
      if (i == 3 || i == 5 || i == 7 || i == 9) {
        AIR_ASSERT(*str == '-');
        ++str;
      }
    }
  }

  static uint32_t To_uint(const char* str) {
    char ch = *str;
    if (ch >= '0' && ch <= '9') {
      return ch - '0';
    }
    if (ch >= 'a' && ch <= 'f') {
      return ch + 10 - 'a';
    }
    if (ch >= 'A' && ch <= 'F') {
      return ch + 10 - 'A';
    }
    AIR_ASSERT(false);
    return 0;
  }

  static char To_str(uint8_t val) {
    if (val >= 0 && val <= 9) {
      return '0' + val;
    }
    if (val >= 10 && val <= 15) {
      return 'A' + val - 10;
    }
    AIR_ASSERT(false);
    return '0';
  }

  uint8_t _uuid[16];
};  // class UUID

bool operator==(const UUID& lhs, const UUID& rhs) {
  return memcmp(&lhs, &rhs, sizeof(UUID)) == 0;
}

bool operator!=(const UUID& lhs, const UUID& rhs) {
  return memcmp(&lhs, &rhs, sizeof(UUID)) != 0;
}

}  // namespace util

}  // namespace air

#endif  // AIR_UTIL_UUID_H
