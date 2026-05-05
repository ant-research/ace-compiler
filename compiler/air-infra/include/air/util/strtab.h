//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_STRING_MAP_H
#define AIR_UTIL_STRING_MAP_H

#include <stdio.h>
#include <string.h>

#include <string>
#include <string_view>
#include <unordered_set>

#include "air/util/mem_id_pool.h"

namespace air {

namespace util {

/**
 * @brief String Table implementation with ability to deduplicate strings.
 * If same strings are entered multiple times, same string id returns.
 *
 */
class STRTAB {
public:
  /**
   * @brief Construct a new STRTAB object
   *
   */
  STRTAB() : _strset(0, STRTAB_HASH(this), STRTAB_EQ(this)) {}

  /**
   * @brief Save a new C string into STRTAB
   *
   * @param str C string with '\0'
   * @return uint32_t String Id
   */
  uint32_t Save_str(const char* str) {
    // include last '\0'
    return Save_str(str, strlen(str) + 1);
  }

  /**
   * @brief Save a new C++ string into STRTAB
   *
   * @param str C++ string
   * @return uint32_t String Id
   */
  uint32_t Save_str(const std::string& str) {
    return Save_str(str.c_str(), str.size() + 1);
  }

  /**
   * @brief Save a buffer with length into STRTAB
   *
   * @param str Address of the buffer
   * @param len Size in byte of the buffer
   * @return uint32_t String Id
   */
  uint32_t Save_str(const char* str, size_t len) {
    auto f = [](char* buf, size_t len, const char* src) -> size_t {
      memcpy(buf, src, len);
      return len;
    };
    return Save(f, len, str);
  }

  /**
   * @brief Save a string with integer suffix into STRTAB
   *
   * @param str C string with '\0'
   * @param val Integer suffix appended to the string
   * @return uint32_t String Id
   */
  uint32_t Save_stri(const char* str, int64_t val) {
    size_t len = strlen(str) + 16;  // 15 for val, 1 for last '\0'
    auto f = [](char* buf, size_t len, const char* str, int64_t val) -> size_t {
      return snprintf(buf, len, "%s%ld", str, val) + 1;  // include last '\0'
    };
    return Save(f, len, str, val);
  }

  /**
   * @brief Concat 2 C strings and save into STRTAB
   *
   * @param s1 First C string with '\0'
   * @param s2 Second C string with '\0'
   * @return uint32_t String Id
   */
  uint32_t Save_str2(const char* s1, const char* s2) {
    size_t len = strlen(s1) + strlen(s2) + 1;  // 1 for last '\0'
    auto   f   = [](char* buf, size_t len, const char* s1,
                const char* s2) -> size_t {
      stpcpy(stpcpy(buf, s1), s2);
      return len;
    };
    return Save(f, len, s1, s2);
  }

  /**
   * @brief Concat 2 C strings, append an integer suffix and save into STRTAB
   *
   * @param s1 First C string with '\0'
   * @param s2 Second C string with '\0'
   * @param val Integer suffix appended to the string
   * @return uint32_t String Id
   */
  uint32_t Save_str2i(const char* s1, const char* s2, int64_t val) {
    size_t len = strlen(s1) + strlen(s2) + 16;  // 15 for val, 1 for last '\0'
    auto   f   = [](char* buf, size_t len, const char* s1, const char* s2,
                int64_t val) -> size_t {
      return snprintf(buf, len, "%s%s%ld", s1, s2, val) +
             1;  // include last '\0'
    };
    return Save(f, len, s1, s2, val);
  }

  /**
   * @brief Get raw string address from its Id
   *
   * @param id String Id
   * @return char* Raw buffer address
   */
  char* Id_to_addr(uint32_t id) const { return _mpool.Id_to_addr(id); }

private:
  // Internal Save method.
  // 1. Allocate an temporary space in MEM_ID_POOL
  // 2. Copy content to the space
  // 3. Try insert the new key into unordered set
  // 4. If succeeded, return new String Id
  // 4. If failed (duplicated), restore mem pool state and return existing
  // String Id
  template <typename COPYF, typename... Args>
  uint32_t Save(COPYF f, size_t len, Args&&... args) {
    _mpool.Bookmark();
    MEM_ID item = _mpool.Allocate(len);
    AIR_ASSERT(item.Addr() != NULL);
    len      = f(item.Addr(), len, args...);
    auto res = _strset.insert(STRTAB_KEY(len, item.Id()));
    if (res.second == false) {
      _mpool.Restore();
    }
    return res.first->Id();
  }

private:
  // Key in unordered_set
  struct STRTAB_KEY {
    // size of the entry
    uint32_t _size;
    // string Id
    uint32_t _str_idx;

    // constructor
    STRTAB_KEY(uint32_t size, uint32_t id) : _size(size), _str_idx(id) {}
    // Get Id
    uint32_t Id() const { return _str_idx; }
  };

  // Hash function for the unordered_set key
  struct STRTAB_HASH {
  public:
    // constructor
    STRTAB_HASH(const STRTAB* tab) : _tab(tab) {}

    // operator() to calculate hash value
    size_t operator()(STRTAB_KEY key) const {
      char* buf = _tab->Id_to_addr(key._str_idx);
      AIR_ASSERT(buf != nullptr);
      std::string_view sv(buf, key._size);
      return std::hash<std::string_view>{}(sv);
    }

  private:
    // pointer to strtab to visit string memory pool
    const STRTAB* _tab;
  };  // struct STRTAB_HASH

  // Equal function for the unordered_set key
  struct STRTAB_EQ {
  public:
    // constructor
    STRTAB_EQ(const STRTAB* tab) : _tab(tab) {}

    // operator() to compare two keys
    bool operator()(STRTAB_KEY lhs, STRTAB_KEY rhs) const {
      if (lhs._size != rhs._size) {
        return false;
      }
      char* lhs_str = _tab->Id_to_addr(lhs._str_idx);
      char* rhs_str = _tab->Id_to_addr(rhs._str_idx);
      return memcmp(lhs_str, rhs_str, lhs._size) == 0;
    }

  private:
    // pointer to strtab to visit string memory pool
    const STRTAB* _tab;
  };  // struct STRTAB_EQ

  // string memory bool
  MEM_ID_POOL<4096, 8> _mpool;
  // unordered set for deduplication
  std::unordered_set<STRTAB_KEY, STRTAB_HASH, STRTAB_EQ> _strset;

};  // class STRTAB

}  // namespace util

}  // namespace air

#endif  // AIR_UTIL_STRING_MAP_H
