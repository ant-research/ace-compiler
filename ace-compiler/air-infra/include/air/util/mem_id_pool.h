//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_MEM_ID_POOL_H
#define AIR_UTIL_MEM_ID_POOL_H

#include <climits>
#include <vector>

#include "air/util/mem_block.h"
#include "air/util/ptr_util.h"

namespace air {

namespace util {

/**
 * @brief A pair of memory address and its Id in the MEM_ID_POOL
 *
 */
class MEM_ID {
public:
  /**
   * @brief Construct a new MEM_ID object
   *
   */
  MEM_ID() : _addr(nullptr), _id(UINT_MAX) {}

  /**
   * @brief Construct a new MEM_ID object
   *
   * @param addr Raw memory address
   * @param id Id in the MEM_ID_POOL
   */
  MEM_ID(char* addr, uint32_t id) : _addr(addr), _id(id) {}

  /**
   * @brief Get raw address of the memory
   *
   * @return char*
   */
  char* Addr() const { return _addr; }

  /**
   * @brief Get Id of the memory in MEM_ID_POOL
   *
   * @return uint32_t
   */
  uint32_t Id() const { return _id; }

private:
  // Raw memory address
  char* _addr;
  // Id in MEM_ID_POOL
  uint32_t _id;
};

/**
 * @brief A memory pool which place underlying memory blocks in array. The
 * memory allocated by this memory pool can be accessed by both raw pointer
 * and Id. The Id is made up by memory block index and offset within the
 * block.
 *
 * @tparam BLK_SIZE Size of underlying memory block
 * @tparam ALIGN_SIZE Minimal alignment for the allocated memory chunk
 */
template <uint32_t BLK_SIZE, uint32_t ALIGN_SIZE>
class MEM_ID_POOL {
public:
  /**
   * @brief Construct a new MEM_ID_POOL object
   *
   */
  MEM_ID_POOL() : _last_mem_blk(UINT_MAX) {
    AIR_STATIC_ASSERT((ALIGN_SIZE & (ALIGN_SIZE - 1)) == 0);
    AIR_STATIC_ASSERT((BLK_SIZE & (BLK_SIZE - 1)) == 0);
    AIR_STATIC_ASSERT((BLK_SIZE % ALIGN_SIZE) == 0);
  }

  /**
   * @brief Destroy the MEM_ID_POOL object. All underlying memory blocks
   * are freed here.
   *
   */
  ~MEM_ID_POOL() {
    for (auto it = _blocks.begin(); it != _blocks.end(); ++it) {
      TAGGED_POINTER blk = *it;
      Blk_free(blk);
    }
  }

  /**
   * @brief Allocate n bytes from underlying memory block.
   *
   * @param n Size in byte to allocate
   * @return MEM_ID A pair of raw memory address and Id in MEM_ID_POOL
   */
  MEM_ID Allocate(size_t n) {
    if (n > NORMAL_BLOCK::MAX_AVAIL_SIZE) {
      // create a large block
      uint32_t         blk_id = _blocks.size();
      MEM_LARGE_BLOCK* ptr    = MEM_LARGE_BLOCK::Create(n);
      AIR_ASSERT(ptr != nullptr);
      _blocks.push_back(TAGGED_POINTER(ptr, LARGE));
      uint32_t id = Gen_id(blk_id, 0);
      return MEM_ID(ptr->Address(), id);
    } else {
      char* buf;
      AIR_ASSERT(_last_mem_blk == UINT_MAX ||
                 (_last_mem_blk < _blocks.size() &&
                  Blk_tag(_blocks[_last_mem_blk]) == NORMAL));
      if ((buf = Normal_allocate(n)) == nullptr) {
        // create a new block to allocate memory
        _last_mem_blk            = _blocks.size();
        MEM_BLOCK<BLK_SIZE>* ptr = MEM_BLOCK<BLK_SIZE>::Create();
        AIR_ASSERT(ptr != nullptr);
        _blocks.push_back(TAGGED_POINTER(ptr, NORMAL));
        buf = ptr->Allocate(n);
        AIR_ASSERT(buf != nullptr);
      }

      uint32_t ofst = buf - Blk_addr(_blocks[_last_mem_blk]);
      uint32_t id   = Gen_id(_last_mem_blk, ofst);
      return MEM_ID(buf, id);
    }
  }

  /**
   * @brief Deallocate the raw pointer allocated from this MEM_ID_POOL
   *
   * @param p Raw pointer to be deallocated
   * @param n Size in byte of the memory chunk
   */
  void Deallocate(char* p, size_t n) {
#ifdef MPOOL_DEBUG
    for (auto it = _blocks.begin(); it != _blocks.end(); ++it) {
      TAGGED_POINTER blk = *it;
      if (Blk_tag(blk) == LARGE) {
        if (Blk_addr(blk) == p) {
          AIR_ASSERT(n == 0 || n > MEM_BLOCK<BLK_SIZE>::MAX_AVAIL_SIZE);
          return;
        }
      } else {
        NORMAL_BLOCK* ptr = blk.Ptr<NORMAL_BLOCK>();
        if (ptr->Contains(p)) {
          AIR_ASSERT(n <= MEM_BLOCK<BLK_SIZE>::MAX_AVAIL_SIZE);
          ptr->Deallocate(p, n);
          return;
        }
      }
    }
    AIR_ASSERT_MSG(false, "deallocate not find the pointer");
#endif
  }

  /**
   * @brief Deallocate a memory chunk by its Id in MEM_ID_POOL
   *
   * @param id Id of the memory chunk
   * @param n Size in byte of the memory chunk
   */
  void Deallocate(uint32_t id, size_t n) {
#ifdef MPOOL_DEBUG
    uint32_t blk_id = Blk_id(id);
    AIR_ASSERT(blk_id < _blocks.size());
    TAGGED_POINTER blk      = _blocks[blk_id];
    uint32_t       blk_ofst = Blk_ofst(id);
    AIR_ASSERT(blk_ofst + n <= Blk_size(blk));

    char* ptr = Id_to_addr(id);
    if (Blk_tag(blk) == LARGE) {
      AIR_ASSERT(blk_ofst == 0);
      AIR_ASSERT(n == 0 || n > MEM_BLOCK<BLK_SIZE>::MAX_AVAIL_SIZE);
      AIR_ASSERT(Blk_addr(blk) == ptr);
    } else {
      AIR_ASSERT(blk.Ptr<NORMAL_BLOCK>()->Contains(ptr));
      blk.Ptr<NORMAL_BLOCK>()->Deallocate(ptr, n);
    }
#endif
  }

  /**
   * @brief Get raw memory address from its Id in MEM_ID_POOL
   *
   * @param id
   * @return char*
   */
  char* Id_to_addr(uint32_t id) const {
    uint32_t blk_id = Blk_id(id);
    AIR_ASSERT(blk_id < _blocks.size());
    TAGGED_POINTER blk      = _blocks[blk_id];
    uint32_t       blk_ofst = Blk_ofst(id);
    AIR_ASSERT(blk_ofst < Blk_size(blk));
    return Blk_addr(blk) + blk_ofst;
  }

  /**
   * @brief Bookmark current MEM_ID_POOL state for a temporary allocation which
   * may be revoked later
   *
   */
  void Bookmark() {
    _state._blk_count    = _blocks.size();
    _state._last_mem_blk = _last_mem_blk;
    if (_last_mem_blk != UINT_MAX) {
      AIR_ASSERT(_last_mem_blk < _state._blk_count);
      _state._watermark =
          _blocks[_last_mem_blk].template Ptr<NORMAL_BLOCK>()->Watermark();
    }
    _state._magic = MEM_BLOCK_MAGIC::MARKER;
  }

  /**
   * @brief Restore MEM_ID_POOL by state saved in Bookmark(). This means the
   * temporary allocation is revoked. Memory chunks allocated since last
   * Bookmark() is invalid after Restore().
   *
   */
  void Restore() {
    AIR_ASSERT(_state._magic == MEM_BLOCK_MAGIC::MARKER);
    _state._magic = 0;
    _last_mem_blk = _state._last_mem_blk;
    if (_last_mem_blk != UINT_MAX) {
      AIR_ASSERT(_last_mem_blk < _state._blk_count);
      _blocks[_last_mem_blk].template Ptr<NORMAL_BLOCK>()->Watermark(
          _state._watermark);
    }
    int32_t free_blocks = _blocks.size() - _state._blk_count;
    while (free_blocks > 0) {
      TAGGED_POINTER blk = _blocks.back();
      Blk_free(blk);
      _blocks.pop_back();
      free_blocks--;
    }
  }

protected:
  typedef MEM_BLOCK<BLK_SIZE> NORMAL_BLOCK;

  // there are two different kinds of memory block in the array,
  // use BLK_TAG to distinguish them.
  enum BLK_TAG { NORMAL, LARGE };

  // try allocate from normal block
  char* Normal_allocate(size_t n) {
    if (_last_mem_blk == UINT_MAX) {
      return nullptr;
    }
    AIR_ASSERT(_last_mem_blk < _blocks.size());
    TAGGED_POINTER blk = _blocks[_last_mem_blk];
    AIR_ASSERT(Blk_tag(blk) == NORMAL);
    return blk.Ptr<NORMAL_BLOCK>()->Allocate(n);
  }

  // get block index from id
  static uint32_t Blk_id(uint32_t id) { return id / (BLK_SIZE / ALIGN_SIZE); }

  // get block offset from id
  static uint32_t Blk_ofst(uint32_t id) {
    return (id % (BLK_SIZE / ALIGN_SIZE)) * ALIGN_SIZE;
  }

  // generate id from block index and offset
  static uint32_t Gen_id(uint32_t blk_id, uint32_t ofst) {
    AIR_ASSERT((ofst % ALIGN_SIZE) == 0);
    AIR_ASSERT(ofst < BLK_SIZE);
    return blk_id * (BLK_SIZE / ALIGN_SIZE) + ofst / ALIGN_SIZE;
  }

  // get block data address
  static char* Blk_addr(TAGGED_POINTER blk) {
    // assume two kinds of blocks have the same offset of data
    AIR_STATIC_ASSERT(sizeof(MEM_BLOCK_HEAD) == sizeof(MEM_LARGE_BLOCK_HEAD));
    return blk.Ptr<MEM_LARGE_BLOCK>()->Address();
  }

  // get block tag
  static BLK_TAG Blk_tag(TAGGED_POINTER blk) { return blk.Tag<BLK_TAG>(); }

  // get block size
  static size_t Blk_size(TAGGED_POINTER blk) {
    BLK_TAG tag = Blk_tag(blk);
    if (tag == LARGE) {
      return blk.Ptr<MEM_LARGE_BLOCK>()->Size();
    } else if (tag == NORMAL) {
      return NORMAL_BLOCK::MAX_AVAIL_SIZE;
    } else {
      AIR_ASSERT_MSG(false, "unknown tag %d", tag);
      return 0;
    }
  }

  // free the block
  static void Blk_free(TAGGED_POINTER blk) {
    BLK_TAG tag = Blk_tag(blk);
    if (Blk_tag(blk) == LARGE) {
      MEM_LARGE_BLOCK::Destroy(blk.Ptr<MEM_LARGE_BLOCK>());
    } else if (Blk_tag(blk) == NORMAL) {
      NORMAL_BLOCK::Destroy(blk.Ptr<NORMAL_BLOCK>());
    } else {
      AIR_ASSERT_MSG(false, "unknown tag %d", tag);
    }
  }

  // save state for the pool
  struct STATE {
    // total block count in _blocks
    uint32_t _blk_count;
    // last normal memory block index
    uint32_t _last_mem_blk;
    // watermark in last normal memory block
    uint32_t _watermark;
    // magic for debug purpose
    uint32_t _magic;
  };

  // state for temporary allocation
  STATE _state;
  // array to contains all blocks
  std::vector<TAGGED_POINTER> _blocks;
  // index to last allocable normal memory block
  uint32_t _last_mem_blk;

};  // class MEM_ID_POOL

}  // namespace util

}  // namespace air

#endif  // AIR_UTIL_MEM_ID_POOL_H
