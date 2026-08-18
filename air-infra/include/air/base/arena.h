//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_ARENA_H
#define AIR_BASE_ARENA_H

#include <cstdlib>
#include <map>

#include "air/base/arena_core.h"
#include "air/base/id_wrapper.h"
#include "air/base/ptr_wrapper.h"

namespace air {
namespace base {

template <size_t SZ, size_t AL, bool LA>
class ARENA {
  typedef ARENA<SZ, AL, LA> MY_TYPE;

public:
  ARENA(ARENA_ALLOCATOR* allocator, uint32_t kind, const char* name,
        bool opened)
      : _core(allocator, SZ, AL, kind, name, opened) {}
  ~ARENA() {}

  typedef PRIM_ID_ITER ITERATOR;

  ITERATOR Begin() const { return ITERATOR(_core.Begin()); }
  ITERATOR End() const { return ITERATOR(_core.End()); }

  template <typename T>
  PTR_FROM_DATA<T> Allocate() {
    const size_t     bytes = sizeof(T);
    uint32_t         id;
    void*            addr = _core.Allocate(bytes, &id);
    PTR_FROM_DATA<T> ret(reinterpret_cast<T*>(addr), ID<T>(id));
    return ret;
  }

  template <typename T>
  PTR_FROM_DATA<T> Allocate_array(size_t num_elem) {
    const size_t     bytes = sizeof(T);
    uint32_t         id;
    void*            addr = _core.Allocate(bytes * num_elem, &id);
    PTR_FROM_DATA<T> ret(static_cast<T*>(addr), ID<T>(id));
    return ret;
  }

  template <typename T>
  void Delete(const PTR_FROM_DATA<T>& ptr) {
    ptr.Addr()->~T();
    _core.Deallocate(ptr.Id().Value());
  }

  template <typename T>
  void Delete_array(const PTR_FROM_DATA<T>& ptr, size_t num_elem) {
    const size_t bytes = sizeof(T);
    for (T* elem = ptr.Addr() + num_elem - 1; num_elem > 0;
         --num_elem, --elem) {
      elem->~T();
    }
    _core.Deallocate(ptr.Id().Value());
  }

  template <typename T>
  PTR_FROM_DATA<T> Find(ID<T> id) const {
    T* const addr = reinterpret_cast<T*>(_core.Find(id.Value()));
    return PTR_FROM_DATA<T>(addr, id);
  }

  void* Find(uint32_t base) const { return _core.Find(base); }

  uint32_t Size() const { return _core.Size(); }

  void Adjust_addr(uint32_t base_id, size_t ofst) {
    _core.Adjust_addr(base_id, ofst);
  }

  void Free(const PTR_FROM_DATA<void>& ptr, size_t sz);

  PTR_FROM_DATA<void> Malloc(size_t sz) {
    uint32_t            id;
    void*               addr = _core.Allocate(sz * SZ, &id);
    PTR_FROM_DATA<void> ret(addr, ID<void>(id));
    return ret;
  }

  void Clone(MY_TYPE& o) { _core.Clone(o._core); }

  static size_t Unit_size() { return SZ; }

protected:
  friend class SCOPE_BASE;
  friend class FUNC_SCOPE;
  friend class GLOB_SCOPE;
  friend class CONTAINER;
  friend class IR_WRITE;
  friend class IR_READ;

  void   Open() { _core.Open(); }
  size_t Close() { return _core.Close(); }

  constexpr size_t Align() { return _core.Align(); }
  constexpr size_t Unit_sz() { return _core.Unit_sz(); }

  //! @brief Archive to file
  //! Item_data Layout: num(uint32_t) + size(uint32_t) + data(size) + ...
  BYTE_PTR Archive(BYTE_PTR pos) { return _core.Archive(pos); }

  //! @brief Recovery from file
  //! Item_data Layout: num(uint32_t) + size(uint32_t) + data(size) + ...
  BYTE_PTR Recovery(BYTE_PTR pos) { return _core.Recovery(pos); }

  //! @brief Archive Item_data address to offset
  BYTE_PTR Archive_offset(BYTE_PTR pos, uint32_t* sz) {
    return _core.Archive_offset(pos, sz);
  }

  //! @brief Recovery Item_data offset to address
  BYTE_PTR Recovery_offset(BYTE_PTR addr, BYTE_PTR pos) {
    return _core.Recovery_offset(addr, pos);
  }

private:
  ARENA(const MY_TYPE& o);
  MY_TYPE& operator=(const MY_TYPE&);

  ARENA_CORE _core;
};

}  // namespace base
}  // namespace air

#endif
