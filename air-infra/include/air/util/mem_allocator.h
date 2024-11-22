//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_MEM_ALLOCATOR_H
#define AIR_UTIL_MEM_ALLOCATOR_H

#include "air/util/mem_pool.h"

namespace air {

namespace util {

//! @brief General object allocator
//! @tparam MEM_POOL Underlying memory pool to construct the objects
template <typename MEM_POOL>
class MEM_ALLOCATOR {
public:
  //! @brief Construct a new general object allocator
  //! @param mp The memory pool where the allocator creates objects
  MEM_ALLOCATOR(MEM_POOL* mp) : _mpool(mp) { AIR_ASSERT(mp != nullptr); }

  //! @brief Allocate memory and construct the object
  //! @tparam OBJ Type of the object to be constructed
  //! @tparam Args Types of the object constructor parameters
  //! @param args Object constructor parameters
  //! @return OBJ* Pointer to the object just constructed
  template <typename OBJ, typename... Args>
  OBJ* Allocate(Args&&... args) {
    char* buf = _mpool->Allocate(sizeof(OBJ));
    AIR_ASSERT(buf != nullptr);
    return new (buf) OBJ(args...);
  }

  //! @brief Destruct the object and deallocate memory
  //! @tparam OBJ Type of the object to be destructed
  //! @param obj Pointer to object to be destructed
  template <typename OBJ>
  void Deallocate(OBJ* obj) {
    obj->~OBJ();
    _mpool->Deallocate((char*)obj, sizeof(OBJ));
  }

  //! @brief Allocate memory and construct the object array
  //! @tparam OBJ Type of the object to be constructed
  //! @tparam Args Types of the object constructor parameters
  //! @param n Number of objects in the array to be constructed
  //! @param args Object constructor parameters
  //! @return OBJ* Pointer to the object array just constructed
  template <typename OBJ, typename... Args>
  OBJ* Allocate_array(uint32_t n, Args&&... args) {
    OBJ* obj = (OBJ*)_mpool->Allocate(n * sizeof(OBJ));
    AIR_ASSERT(obj != nullptr);
    for (uint32_t i = 0; i < n; ++i) {
      new (&obj[i]) OBJ(args...);
    }
    return obj;
  }

  //! @brief Destruct all objects in the array and deallocate memory
  //! @tparam OBJ Type of the object to be destructed
  //! @param obj Pointer to the object array just destructed
  //! @param n Number of objects in the array to be destructed
  template <typename OBJ>
  void Deallocate_array(OBJ* obj, uint32_t n) {
    for (uint32_t i = 0; i < n; ++i) {
      obj[i].~OBJ();
    }
    _mpool->Deallocate((char*)obj, n * sizeof(OBJ));
  }

private:
  // Memory pool where the objects are constructed
  MEM_POOL* _mpool;
};  // class MEM_ALLOCATOR

//! @brief C++ STL memory allocator compatible allocator
//! @tparam OBJ Type of the object to be constructed/destructed
//! @tparam MEM_POOL Underlying memory pool where the objects are constructed
template <typename OBJ, typename MEM_POOL>
class CXX_MEM_ALLOCATOR {
public:
  // NOLINTBEGIN (readability-identifier-naming)
  typedef size_t     size_type;
  typedef ptrdiff_t  difference_type;
  typedef OBJ*       pointer;
  typedef const OBJ* const_pointer;
  typedef OBJ&       reference;
  typedef const OBJ& const_reference;
  typedef OBJ        value_type;

  template <class U>
  struct rebind {
    typedef CXX_MEM_ALLOCATOR<U, MEM_POOL> other;
  };

  pointer       address(reference x) const { return &x; }
  const_pointer address(const_reference x) const { return &x; }

  pointer allocate(size_type n, const void* = 0) {
    return n != 0 ? pointer(_mpool->Allocate(n * sizeof(OBJ))) : pointer(0);
  }

  void deallocate(pointer p, size_type n) {
    if (p) {
      _mpool->Deallocate((char*)p, n * sizeof(OBJ));
    }
  }

  size_type max_size() const { return size_t(-1) / sizeof(OBJ); }

  void construct(pointer p, const OBJ& val) { new (p) OBJ(val); }

  void destroy(pointer p) { p->~OBJ(); }

  bool operator==(CXX_MEM_ALLOCATOR<OBJ, MEM_POOL>& p) {
    return _mpool == p._mpool;
  }

  bool operator!=(CXX_MEM_ALLOCATOR<OBJ, MEM_POOL>& p) {
    return _mpool != p._mpool;
  }

  bool operator==(const CXX_MEM_ALLOCATOR<OBJ, MEM_POOL>& p) const {
    return _mpool == p._mpool;
  }

  bool operator!=(const CXX_MEM_ALLOCATOR<OBJ, MEM_POOL>& p) const {
    return _mpool != p._mpool;
  }
  // NOLINTEND (readability-identifier-naming)

public:
  //! @brief Construct a new C++ STL compatible allocator
  //! @param mp The memory pool where the allocator creates objects
  CXX_MEM_ALLOCATOR(MEM_POOL* mp) : _mpool(mp) { AIR_ASSERT(mp != nullptr); }

  //! @brief Copy constructor
  CXX_MEM_ALLOCATOR(const CXX_MEM_ALLOCATOR<OBJ, MEM_POOL>& p)
      : _mpool(p.Mem_pool()) {
    AIR_ASSERT(_mpool != nullptr);
  }

  //! @brief Another copy constructor
  template <class U>
  CXX_MEM_ALLOCATOR(const CXX_MEM_ALLOCATOR<U, MEM_POOL>& p)
      : _mpool(p.Mem_pool()) {
    AIR_ASSERT(_mpool != nullptr);
  }

  //! @brief Allocate memory and construct the object
  //! @tparam Args Types of the object constructor parameters
  //! @param args Object constructor parameters
  //! @return OBJ* Pointer to object just constructed
  template <typename... Args>
  OBJ* Allocate(Args&&... args) {
    char* buf = _mpool->Allocate(sizeof(OBJ));
    AIR_ASSERT(buf != nullptr);
    return new (buf) OBJ(args...);
  }

  //! @brief Destruct the object and deallocate memory
  //! @param obj Pointer to the object to be deallocated
  //! @param n Size of the memory allocated for the object
  void Deallocate(OBJ* obj, size_t n) {
    obj->~OBJ();
    _mpool->Deallocate((char*)obj, n);
  }

  //! @brief Get underlying MEM_POOL
  MEM_POOL* Mem_pool() const { return _mpool; }

private:
  // Memory pool where the objects are constructed
  MEM_POOL* _mpool;
};  // CXX_MEM_ALLOCATOR

}  // namespace util

}  // namespace air

#endif  // AIR_UTIL_MEM_ALLOCATOR_H
