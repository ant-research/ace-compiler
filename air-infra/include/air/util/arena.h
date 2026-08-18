//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_ARENA_H
#define AIR_UTIL_ARENA_H

#include "air/util/mem_id_pool.h"

namespace air {

namespace util {

/**
 * @brief A pair of object pointer and its Id in ARENA
 *
 * @tparam OBJ
 */
template <typename OBJ>
class OBJ_ID {
public:
  /**
   * @brief Construct an object with default value
   *
   */
  OBJ_ID() : _ptr(nullptr), _id(UINT_MAX) {}

  /**
   * @brief Construct an object with object pointer and Id
   *
   * @param ptr
   * @param id
   */
  OBJ_ID(OBJ* ptr, uint32_t id) : _ptr(ptr), _id(id) {}

  /**
   * @brief Get object pointer
   *
   * @return OBJ* Object pointer
   */
  OBJ* Ptr() const { return _ptr; }

  /**
   * @brief Get Id in ARENA
   *
   * @return uint32_t Object Id
   */
  uint32_t Id() const { return _id; }

private:
  // pointer to the object
  OBJ* _ptr;
  // Id of the object
  uint32_t _id;
};  // OBJ_ID

/**
 * @brief Allocates and deallocates object from underlying memory pool.
 * For allocation, constructor will be called. Both pointer to object and Id
 * of object is returned.
 * For allocation of raw memory, both pointer to the header and Id of the
 * buffer is returned.
 *
 * @tparam BLK_SIZE Size in bytes of underlying memory block.
 */
template <uint32_t BLK_SIZE>
class ARENA {
public:
  // underlying memory pool type
  typedef MEM_ID_POOL<BLK_SIZE, 8> ARENA_POOL;

  /**
   * @brief Construct a new ARENA object. If mp is NULL, an ARENA_POOL will
   * be created and destroyed in destructor. If mp is not NULL, mp won't be
   * destroyed in destructor.
   *
   * @param mp Underlying memory pool for this ARENA
   */
  ARENA(ARENA_POOL* mp = nullptr) : _mpool(mp) {
    if (_mpool == nullptr) {
      _mpool     = new ARENA_POOL;
      _free_pool = true;
    } else {
      _free_pool = false;
    }
  }

  /**
   * @brief Destroy the ARENA object. Free underlying memory pool if it's
   * created in constructor.
   *
   */
  ~ARENA() {
    if (_free_pool) {
      delete _mpool;
    }
  }

  /**
   * @brief Allocate object with fixed size. Constructor of the object will
   * be called. Both object pointer and Id is returned.
   *
   * @tparam OBJ Type of the object to construct
   * @tparam Args Type of the object constructor parameters
   * @param args Object constructor parameters
   * @return OBJ_ID<OBJ> Both pointer and Id of the allocated object
   */
  template <typename OBJ, typename... Args>
  OBJ_ID<OBJ> Allocate(Args&&... args) {
    MEM_ID ret = _mpool->Allocate(sizeof(OBJ));
    AIR_ASSERT(ret.Addr() != nullptr);
    OBJ* obj = new (ret.Addr()) OBJ(args...);
    return OBJ_ID(obj, ret.Id());
  }

  /**
   * @brief Destruct the object and deallocate the memory
   *
   * @tparam OBJ Type of the object to be destructed
   * @param obj Pointer to the object to be destructed
   */
  template <typename OBJ>
  void Deallocate(OBJ* obj) {
    obj->~OBJ();
    _mpool->Deallocate((char*)obj, sizeof(OBJ));
  }

  /**
   * @brief Allocate object with extra size. Actually allocated size is
   * sizeof(OBJ) + extra_size in byte. Constructor of the object will be
   * called. Both object pointer and Id is returned.
   *
   * @tparam OBJ Type of the object to construct
   * @tparam Args Type of the object constructor parameters
   * @param extra_bytes Extra bytes to be allocated
   * @param args Object constructor parameters
   * @return OBJ_ID<OBJ> Both pointer and Id of the allocated object
   */
  template <typename OBJ, typename... Args>
  OBJ_ID<OBJ> Allocate_extra(uint32_t extra_bytes, Args&&... args) {
    MEM_ID ret = _mpool->Allocate(sizeof(OBJ) + extra_bytes);
    AIR_ASSERT(ret.Addr() != nullptr);
    OBJ* obj = new (ret.Addr()) OBJ(args...);
    return OBJ_ID(obj, ret.Id());
  }

  /**
   * @brief Destruct the object and deallocate the memory
   *
   * @tparam OBJ OBJ Type of the object to be destructed
   * @param obj Pointer to the object to be destructed
   * @param extra_bytes Extra bytes allocated in Allocate_extra()
   */
  template <typename OBJ>
  void Deallocate_extra(OBJ* obj, uint32_t extra_bytes) {
    obj->~OBJ();
    _mpool->Deallocate((char*)obj, sizeof(OBJ) + extra_bytes);
  }

  /**
   * @brief Allocate raw buffer
   *
   * @param bytes Size of the buffer in byte
   * @return MEM_ID Both raw buffer pointer and Id in ARENA
   */
  MEM_ID Allocate_raw(size_t bytes) { return _mpool->Allocate(bytes); }

  /**
   * @brief Deallocate raw buffer
   *
   * @param ptr Pointer to the raw buffer
   * @param bytes Size of the buffer in byte
   */
  void Deallocate_raw(char* ptr, size_t bytes) {
    return _mpool->Deallocate(ptr, bytes);
  }

  /**
   * @brief Get object pointer from its Id in ARENA
   *
   * @tparam OBJ Type of the object
   * @param id Id of the object in ARENA
   * @return OBJ* Pointer to the object
   */
  template <typename OBJ>
  OBJ* Id_to_ptr(uint32_t id) const {
    return (OBJ*)_mpool->Id_to_addr(id);
  }

  /**
   * @brief Get raw pointer from its Id in ARENA
   *
   * @param id Id of the object in ARENA
   * @return char* Pointer to the raw buffer
   */
  char* Id_to_raw(uint32_t id) const { return _mpool->Id_to_addr(id); }

private:
  // underlying memory of the ARENA
  ARENA_POOL* _mpool;
  // should the _mpool be freed in destructor
  bool _free_pool;
};  // ARENA

}  // namespace util

}  // namespace air

#endif  // AIR_UTIL_ARENA_H
