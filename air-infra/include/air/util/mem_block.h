//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_MEM_BLOCK_H
#define AIR_UTIL_MEM_BLOCK_H

#include <new>

#include "air/util/mem_util.h"

namespace air {

namespace util {

/**
 * @brief Magic numbers for different memory objects for debug purpose
 *
 */
enum MEM_BLOCK_MAGIC : uint32_t {
  /// @brief Identify a stacked memory marker
  MARKER = 0x0a0a0a0a,
  /// @brief Identify a memory block
  BLOCK = 0xdeadbeef,
  /// @brief Identify the head of the smaller memory chunk
  HEAD = 0xaabbccdd,
  /// @brief Identify the tail of the smaller memory chunk
  TAIL = 0x11223344,
  /// @brief Identify the memory is available for allocation
  AVAIL = 0xdd,
  /// @brief Identify the memory is already freed
  FREED = 0xcc
};  // MEM_BLOCK_MAGIC

/**
 * @brief Header of a normal memory block
 *
 */
struct MEM_BLOCK_HEAD {
  /// @brief Size in byte for memory available for allocation in this block
  uint32_t _avail;
  /// @brief Magic number for debug purpose
  uint32_t _magic;
  /// @brief Pointer to previous block
  MEM_BLOCK_HEAD* _prev;
};  // MEM_BLOCK_HEAD

/**
 * @brief Header of a large memory block
 *
 */
struct MEM_LARGE_BLOCK_HEAD {
  /// @brief Size in byte for this large memory block
  size_t _size;
  /// @brief Pointer to previous large block
  MEM_LARGE_BLOCK_HEAD* _prev;
};  // MEM_LARGE_BLOCK_HEAD

/**
 * @brief Entry for a memory block created by memory map (mmap/munmap)
 *
 */
struct MMAP_BLOCK_ENTRY {
  /// @brief Base address of the mmap memory block
  void* _pmem;
  /// @brief Pointer to previous mmap memory block
  MMAP_BLOCK_ENTRY* _prev;
};  // MMAP_BLOCK_ENTRY

/**
 * @brief A normal memory block, which can be partioned into smaller pieces
 *
 * @tparam BLK_SIZE Size in byte for the block
 */
template <uint32_t BLK_SIZE>
class MEM_BLOCK {
public:
  /**
   * @brief Return watermark of this block. Consider the extra bytes for
   * alignment and debug, the real allocatable size is smaller than watermark.
   * Only used for Push()/Pop() in stacked memory pool.
   *
   * @return uint32_t Watermark of this block
   */
  uint32_t Watermark() const { return _head._avail; }

  /**
   * @brief Reset watermark of this block with given size.
   * Only used for Push()/Pop() in stacked memory pool.
   *
   * @param avail Watermark to be set to
   */
  void Watermark(size_t avail) {
    AIR_ASSERT(avail <= sizeof(_data));
    AIR_ASSERT(Is_aligned(avail, sizeof(uintptr_t)));
    _head._avail = (uint32_t)avail;
#ifdef MPOOL_DEBUG
    memset(&_data[sizeof(_data) - avail], AVAIL, avail);
#endif
  }

  /**
   * @brief Return pointer to previous memory block
   *
   * @return MEM_BLOCK<BLK_SIZE>* Pointer to previous memory block
   */
  MEM_BLOCK<BLK_SIZE>* Prev() const {
    return (MEM_BLOCK<BLK_SIZE>*)_head._prev;
  }

  /**
   * @brief Set previous memory block
   *
   * @param prev Pointer to previous memory block
   */
  void Prev(MEM_BLOCK<BLK_SIZE>* prev) { _head._prev = (MEM_BLOCK_HEAD*)prev; }

  /**
   * @brief Check if p is contained by this block
   *
   * @param p Pointer to be checked
   * @return true if p is in range of this emory block
   * @return false if p is out of range of this memory block
   */
  bool Contains(char* p) const {
    return (p >= &_data[0]) && (p < &_data[sizeof(_data)]);
  }

  /**
   * @brief Allocate n bytes from this block
   *
   * @param n Number of memory in byte to be allocated
   * @return char* Pointer to allocated memory, or nullptr if not enough
   * available memory in this block
   */
  char* Allocate(uint32_t n) {
#ifdef MPOOL_DEBUG
    // allocate 4 extra uint32 to save size and magic for debug
    n += 4 * sizeof(uint32_t);
#endif
    // aligned to at least 8-byte
    n = Align(n, sizeof(uintptr_t));
    if (n > _head._avail) {
      // not enough, return nullptr
      return nullptr;
    }
    char* buf = (char*)this + BLK_SIZE - _head._avail;
    AIR_ASSERT(Is_aligned(buf, sizeof(uintptr_t)));
    _head._avail -= n;
#ifdef MPOOL_DEBUG
    uint32_t* pint32 = (uint32_t*)buf;
    for (uint32_t i = 0; i < (n >> 2); ++i) {
      constexpr uint32_t MAGIC =
          (AVAIL << 24) + (AVAIL << 16) + (AVAIL << 8) + AVAIL;
      AIR_ASSERT(pint32[i] == MAGIC);
    }
    pint32[0]            = n;
    pint32[1]            = MEM_BLOCK_MAGIC::HEAD;
    pint32[(n >> 2) - 2] = MEM_BLOCK_MAGIC::TAIL;
    pint32[(n >> 2) - 1] = n;
    buf += 2 * sizeof(uint32_t);
#endif
    return buf;
  }

  /**
   * @brief Deallocate memory (actually do nothing)
   *
   * @param buf Pointer to buffer to be deallocated
   * @param n Number in byte of the buffer
   */
  void Deallocate(void* buf, uint32_t n) {
    AIR_ASSERT(Is_aligned(buf, sizeof(uintptr_t)));
#ifdef MPOOL_DEBUG
    uint32_t* pint32 = (uint32_t*)buf;
    AIR_ASSERT(pint32[-1] == MEM_BLOCK_MAGIC::HEAD);
    if (n == 0) {
      n = pint32[-2];
    } else {
      // aligned to at least 8-byte
      n = Align(n, sizeof(uintptr_t));
      n += 4 * sizeof(uint32_t);
      AIR_ASSERT(n == pint32[-2]);
    }
    memset(pint32, AVAIL, n - 4 * sizeof(uint32_t));
    AIR_ASSERT(pint32[(n >> 2) - 4] == MEM_BLOCK_MAGIC::TAIL);
    AIR_ASSERT(pint32[(n >> 2) - 3] == n);
#endif
  }

  /**
   * @brief Create a new memory block
   *
   * @return MEM_BLOCK<BLK_SIZE>* Pointer to the memory block
   */
  static MEM_BLOCK<BLK_SIZE>* Create() {
    char* buf = Air_malloc(BLK_SIZE);
    AIR_ASSERT(buf != nullptr);
    return new (buf) MEM_BLOCK();
  }

  /**
   * @brief Destroy the memory block
   *
   * @param blk Pointer to the memory block to be destroyed
   */
  static void Destroy(MEM_BLOCK<BLK_SIZE>* blk) {
    AIR_ASSERT(blk != nullptr);
    AIR_ASSERT(blk->Magic() == MEM_BLOCK_MAGIC::BLOCK);
#ifdef MPOOL_DEBUG
    memset(blk, MEM_BLOCK_MAGIC::FREED, BLK_SIZE);
#endif
    Air_free(blk, BLK_SIZE);
  }

#ifdef MPOOL_DEBUG
  static constexpr size_t MAX_AVAIL_SIZE =
      BLK_SIZE - sizeof(MEM_BLOCK_HEAD) - 4 * sizeof(uint32_t);
#else
  static constexpr size_t MAX_AVAIL_SIZE = BLK_SIZE - sizeof(MEM_BLOCK_HEAD);
#endif

  // disable copy constructor
  MEM_BLOCK(const MEM_BLOCK&) = delete;
  // disable operator =
  MEM_BLOCK& operator=(const MEM_BLOCK&) = delete;
  // disable destructor
  ~MEM_BLOCK() = delete;

private:
  // private constructor, only callable by Create()
  MEM_BLOCK() {
    _head._avail = BLK_SIZE - sizeof(MEM_BLOCK_HEAD);
    _head._magic = MEM_BLOCK_MAGIC::BLOCK;
#ifdef MPOOL_DEBUG
    memset(_data, MEM_BLOCK_MAGIC::AVAIL, sizeof(_data));
#endif
  }

  // Block magic
  uint32_t Magic() const { return _head._magic; }

  // Pointer to beginning of data
  char* Data() const { return _data; }

  // Header for this block
  MEM_BLOCK_HEAD _head;

  // Spaces available for allocation
  char _data[BLK_SIZE - sizeof(_head)];
};  // class MEM_BLOCK

/**
 * @brief A large memory block, which is always allocated/deallocated as a whole
 *
 */
class MEM_LARGE_BLOCK {
public:
  /**
   * @brief Return pointer to previous large memory block
   *
   * @return MEM_LARGE_BLOCK* Pointer to previous large memory block
   */
  MEM_LARGE_BLOCK* Prev() const { return (MEM_LARGE_BLOCK*)_head._prev; }

  /**
   * @brief Set previous large memory block
   *
   * @param prev Pointer to previous large memory block
   */
  void Prev(MEM_LARGE_BLOCK* prev) {
    _head._prev = (MEM_LARGE_BLOCK_HEAD*)prev;
  }

  /**
   * @brief Address of the buffer accessible for user
   *
   * @return char*
   */
  char* Address() const { return (char*)this + sizeof(MEM_LARGE_BLOCK_HEAD); }

  /**
   * @brief Size in byte of this large block
   *
   * @return size_t
   */
  size_t Size() const { return _head._size; }

  /**
   * @brief Create a large memory block with size n in byte
   *
   * @param n Size in byte of the large memory block
   * @return MEM_LARGE_BLOCK* Pointer to the large memory block
   */
  static MEM_LARGE_BLOCK* Create(size_t n) {
    size_t sz = n + sizeof(MEM_LARGE_BLOCK_HEAD);
#ifdef MPOOL_DEBUG
    // allocate 2 extra uint32 to save size and magic for debug
    sz = Align(sz, sizeof(uintptr_t)) + 2 * sizeof(uint32_t);
#endif
    char* buf = Air_malloc(sz);
    AIR_ASSERT(buf != nullptr);
#ifdef MPOOL_DEBUG
    memset(buf, AVAIL, sz);
    uint32_t* pint32      = (uint32_t*)buf;
    pint32[(sz >> 2) - 2] = MEM_BLOCK_MAGIC::TAIL;
    pint32[(sz >> 2) - 1] = (uint32_t)n;  // possibly truncated
#endif
    return new (buf) MEM_LARGE_BLOCK(n);
  }

  /**
   * @brief Destroy a large memory block
   *
   * @param blk Pointer to the large memory block
   */
  static void Destroy(MEM_LARGE_BLOCK* blk) {
    AIR_ASSERT(blk != nullptr);
    AIR_ASSERT(Is_aligned(blk, sizeof(uintptr_t)));
    size_t sz = blk->Size() + sizeof(MEM_LARGE_BLOCK_HEAD);
#ifdef MPOOL_DEBUG
    sz               = Align(sz, sizeof(uintptr_t)) + 2 * sizeof(uint32_t);
    uint32_t* pint32 = (uint32_t*)blk;
    AIR_ASSERT(pint32[(sz >> 2) - 2] == MEM_BLOCK_MAGIC::TAIL);
    AIR_ASSERT(pint32[(sz >> 2) - 1] == (uint32_t)blk->Size());
    memset(blk, FREED, sz);
#endif
    Air_free(blk, sz);
  }

  // disable copy constructor
  MEM_LARGE_BLOCK(const MEM_LARGE_BLOCK&) = delete;
  // disable operator =
  MEM_LARGE_BLOCK& operator=(const MEM_LARGE_BLOCK&) = delete;
  // disable destructor
  ~MEM_LARGE_BLOCK() = delete;

private:
  // private constructor, only callable by Create()
  MEM_LARGE_BLOCK(size_t n) { _head._size = n; }

  // Header of this large block
  MEM_LARGE_BLOCK_HEAD _head;

  // Spaces available for user
  char _data[];
};  // class MEM_LARGE_BLOCK

}  // namespace util

}  // namespace air

#endif  // AIR_UTIL_MEM_BLOCK_H
