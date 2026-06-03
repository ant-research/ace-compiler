//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_MEM_UTIL_H
#define AIR_UTIL_MEM_UTIL_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "air/util/align.h"

namespace air {

namespace util {

/**
 * @brief Memory consumption statistics
 *
 */
class MEM_STATS {
protected:
  // How many times and bytes allocated
  void Allocate(size_t n) {
    _alloc_size += n;
    _alloc_times++;
  }

  // How many times and bytes deallocated
  void Deallocate(size_t n) {
    _free_size += n;
    _free_times++;
  }

  // If all allocated memory are deallocated
  bool All_freed() const {
    return (_alloc_size == _free_size) && (_alloc_times == _free_times);
  }

  // Constructor, set all metrics to 0
  MEM_STATS()
      : _next(nullptr),
        _msg(nullptr),
        _alloc_size(0),
        _alloc_times(0),
        _free_size(0),
        _free_times(0) {}

  friend class MEM_POOL_MANAGER;

private:
  // Pointer to next stats entry
  MEM_STATS* _next;
  // A string to idenfify this stats for debug purpose
  const char* _msg;
  // Size in byte allocated
  uint64_t _alloc_size;
  // Times allocated
  uint64_t _alloc_times;
  // Size in byte deallocated
  uint64_t _free_size;
  // Times deallocated
  uint64_t _free_times;

};  // class MEM_STATS

/**
 * @brief Memory pool manager to debug memory consumption issue
 *
 */
class MEM_POOL_MANAGER {
public:
  /**
   * @brief Register a new memory stats entry.
   * Called when a new memory pool is created.
   *
   * @param mstat
   */
  static void Register(MEM_STATS* mstat) {
    Mem_pool_stats->_next = mstat;
    Mem_pool_stats        = mstat;
  }

  /**
   * @brief Deregister a memory stats entry.
   * Called when a memory pool is destroyed.
   *
   * @param mstat
   */
  static void Deregister(MEM_STATS* mstat) {
    MEM_STATS* ptr  = Mem_pool_stats;
    MEM_STATS* prev = nullptr;
    while (ptr != mstat) {
      prev = ptr;
      ptr  = ptr->_next;
    }
    AIR_ASSERT(ptr == mstat);
    if (prev) {
      AIR_ASSERT(prev->_next == ptr);
      prev->_next = ptr->_next;
    } else {
      AIR_ASSERT(Mem_pool_stats == ptr);
      Mem_pool_stats = ptr->_next;
    }
  }

  /**
   * @brief Record an allocation of n bytes
   *
   * @param n
   */
  static void Allocate(size_t n) { Non_pool_stats.Allocate(n); }

  /**
   * @brief Record a deallocation of n bytes
   *
   * @param n
   */
  static void Deallocate(size_t n) { Non_pool_stats.Deallocate(n); }

  /**
   * @brief Check if all memory are freed
   *
   * @return true
   * @return false
   */
  static bool All_freed() {
    if (!Non_pool_stats.All_freed()) {
      return false;
    }
    MEM_STATS* ptr = Mem_pool_stats;
    while (ptr) {
      if (!ptr->All_freed()) {
        return false;
      }
      ptr = ptr->_next;
    }
    return true;
  }

  /**
   * @brief Print current memory consumption statistics.
   *
   */
  static void Print();

private:
  // linked list for statistics of all memory pools
  static MEM_STATS* Mem_pool_stats;
  // For memory statistics for raw air_malloc/air_free
  static MEM_STATS Non_pool_stats;
};  // class MEM_POOL_MANAGER

/**
 * @brief Wrapper of libc malloc() so that the size of memory allocated can
 * be recorded.
 *
 * @param n Size in byte to be allocated
 * @return char* Pointer to memory allocated
 */
static inline char* Air_malloc(size_t n) {
#ifdef MPOOL_DEBUG
  MEM_POOL_MANAGER::Allocate(n);
#endif
  return (char*)malloc(n);
}  // air_malloc

/**
 * @brief Wrapper of libc free() so that the size of memory deallocated can
 * be recorded.
 *
 * @param p Pointer to the memory to be deallocated
 * @param n Size in byte to be deallocated
 */
static inline void Air_free(void* p, size_t n) {
#ifdef MPOOL_DEBUG
  MEM_POOL_MANAGER::Deallocate(n);
#endif
  free(p);
}  // air_free

}  // namespace util

}  // namespace air

#endif  // AIR_UTIL_MEM_UTIL_H
