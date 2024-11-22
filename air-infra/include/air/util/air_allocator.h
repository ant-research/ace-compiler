//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_AIR_ALLOCATOR_H
#define AIR_UTIL_AIR_ALLOCATOR_H

#include "air/util/mem_pool.h"

namespace air {

namespace util {

/**
 * @brief Object allocator for AIR node.
 *
 * @tparam BLK_SIZE Size of underlying memory block.
 */
template <uint32_t BLK_SIZE>
class NODE_ALLOCATOR {
public:
  /**
   * @brief Construct a new node allocator
   *
   * @param mp The memory pool where the allocator creates nodes
   */
  NODE_ALLOCATOR(MEM_POOL<BLK_SIZE>* mp) : _mpool(mp) {
    AIR_ASSERT(mp != nullptr);
    for (uint32_t i = 0; i < MAX_FREE_SLOTS; ++i) {
      _free_list[i] = nullptr;
    }
  }

  /**
   * @brief Allocate memory and construct the node object
   *
   * @tparam OBJ Type of AIR node to be constructed
   * @tparam Args Types of AIR node constructor parameters
   * @param kid_count Number of kids of this node
   * @param args AIR node constructor parameters
   * @return OBJ* Pointer to the AIR node
   */
  template <typename OBJ, typename... Args>
  OBJ* Allocate(uint32_t kid_count, Args&&... args) {
    AIR_STATIC_ASSERT(sizeof(OBJ) >= sizeof(FREE_ENTRY));
    AIR_STATIC_ASSERT((sizeof(OBJ) % sizeof(uintptr_t)) == 0);
    char*    buf;
    uint32_t free_slot =
        (kid_count > MAX_FREE_SLOTS - 1) ? MAX_FREE_SLOTS - 1 : kid_count;
    if ((kid_count < MAX_FREE_SLOTS - 1) && _free_list[kid_count]) {
      // search free list to reuse deallocated nodes with fixed small kid count
      AIR_ASSERT(_free_list[kid_count]->_kid_count == kid_count);
      buf                   = (char*)_free_list[kid_count];
      _free_list[kid_count] = _free_list[kid_count]->_next;
    } else if (_free_list[MAX_FREE_SLOTS - 1]) {
      // search free list for nodes with large kid count
      FREE_ENTRY* ptr  = _free_list[MAX_FREE_SLOTS - 1];
      FREE_ENTRY* prev = nullptr;
      while (ptr && ptr->_kid_count != kid_count) {
        prev = ptr;
        ptr  = ptr->_next;
      }
      if (ptr != nullptr) {
        // reuse this node
        if (prev != nullptr) {
          AIR_ASSERT(_free_list[MAX_FREE_SLOTS - 1] != ptr);
          prev->_next = ptr->_next;
        } else {
          AIR_ASSERT(_free_list[MAX_FREE_SLOTS - 1] == ptr);
          _free_list[MAX_FREE_SLOTS - 1] = ptr->_next;
        }
        buf = (char*)ptr;
      } else {
        // allocate new node
        buf = _mpool->Allocate(sizeof(OBJ) + kid_count * sizeof(uintptr_t));
      }
    } else {
      // allocate new node
      buf = _mpool->Allocate(sizeof(OBJ) + kid_count * sizeof(uintptr_t));
    }
    AIR_ASSERT(buf != nullptr);
    // not sure if ctor is needed
    return new (buf) OBJ(args...);
  }

  /**
   * @brief Destruct the AIR node object and deallocate memory
   *
   * @tparam OBJ Type of AIR node to be destructed
   * @param obj Pointer to AIR node to be destructed
   * @param kid_count Number of kids of this node
   */
  template <typename OBJ>
  void Deallocate(OBJ* obj, uint32_t kid_count) {
    AIR_STATIC_ASSERT(sizeof(OBJ) >= sizeof(FREE_ENTRY));
    // not sure if dtor is needed
    obj->~OBJ();
    _mpool->Deallocate((char*)obj, sizeof(OBJ) + kid_count * sizeof(uintptr_t));
    // add to free list
    uint32_t free_slot =
        (kid_count > MAX_FREE_SLOTS - 1) ? MAX_FREE_SLOTS - 1 : kid_count;
    ((FREE_ENTRY*)obj)->_next      = _free_list[free_slot];
    ((FREE_ENTRY*)obj)->_kid_count = kid_count;
    _free_list[free_slot]          = (FREE_ENTRY*)obj;
  }

private:
  // Number of free list slots to reuse AIR nodes
  static constexpr uint32_t MAX_FREE_SLOTS = 7;

  // Memory pool where the AIR nodes are constructed
  MEM_POOL<BLK_SIZE>* _mpool;

  // Linked list for deallocated AIR nodes for reuse
  struct FREE_ENTRY {
    FREE_ENTRY* _next;
    uint32_t    _kid_count;
  };

  // _free_list[0 ~ MAX_FREE_SLOTS-2] are used for AIR nodes with kids = 0 ~
  // MAX_FREE_SLOTS-2 _free_list[MAX_FREE_SLOTS-1] is used for AIR nodes with
  // kids >= MAX_FREE_SLOTS-1
  FREE_ENTRY* _free_list[MAX_FREE_SLOTS];
};  // class MEM_ALLOCATOR

/**
 * @brief Object allocator for AIR symtab entries
 *
 * @tparam BLK_SIZE Size of underlying memory block
 */
template <uint32_t BLK_SIZE>
class SYMTAB_ALLOCATOR {
public:
  /**
   * @brief Construct a new AIR symtab allocator object
   *
   * @param mp The memory pool where the allocator creates AIR symtab entries
   */
  SYMTAB_ALLOCATOR(MEM_POOL<BLK_SIZE>* mp) : _mpool(mp), _free_list(nullptr) {
    AIR_ASSERT(mp != nullptr);
  }

  /**
   * @brief Allocate memory and construct AIR symtab entry
   *
   * @tparam OBJ Type of AIR symtab entry
   * @tparam Args Types of AIR symtab entry constructor parameters
   * @param args AIR symtab entry constructor parameters
   * @return OBJ* Pointer to the AIR symtab entry
   */
  template <typename OBJ, typename... Args>
  OBJ* Allocate(Args&&... args) {
    AIR_STATIC_ASSERT(sizeof(OBJ) >= sizeof(FREE_ENTRY));
    char* buf;
    if (_free_list) {
      // reuse deallocated symtab entry
      buf        = (char*)_free_list;
      _free_list = _free_list->_next;
    } else {
      // allocate new entry
      buf = _mpool->Allocate(sizeof(OBJ));
    }
    AIR_ASSERT(buf != nullptr);
    // not sure if ctor is needed
    return new (buf) OBJ(args...);
  }

  /**
   * @brief Destruct the AIR symtab entry and deallocate memory
   *
   * @tparam OBJ Type of AIR symtab entry to be destructed
   * @param obj Pointer to AIR symtab entry to be destructed
   */
  template <typename OBJ>
  void Deallocate(OBJ* obj) {
    AIR_STATIC_ASSERT(sizeof(OBJ) >= sizeof(FREE_ENTRY));
    // not sure if dtor is needed
    obj->~OBJ();
    _mpool->Deallocate((char*)obj, sizeof(OBJ));
    // add to free list
    ((FREE_ENTRY*)obj)->_next = _free_list;
    _free_list                = (FREE_ENTRY*)obj;
  }

private:
  // Memory pool where the AIR symtab entries are constructed
  MEM_POOL<BLK_SIZE>* _mpool;

  // Linked list for deallocated AIR symtab entries for reuse
  struct FREE_ENTRY {
    FREE_ENTRY* _next;
  };

  // Free list for reuse
  FREE_ENTRY* _free_list;
};  // CXX_MEM_ALLOCATOR

}  // namespace util

}  // namespace air

#endif  // AIR_UTIL_MEM_ALLOCATOR_H
