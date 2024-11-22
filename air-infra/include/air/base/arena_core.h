//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_ARENA_CORE_H
#define AIR_BASE_ARENA_CORE_H

#include <vector>

#include "air/util/mem_allocator.h"
#include "air/util/mem_pool.h"

namespace air {
namespace base {

typedef air::util::MEM_POOL<4096> ARENA_ALLOCATOR;
typedef char*                     BYTE_PTR;

class PRIM_ID_ITER;

class ARENA_ITEM_ARRAY {
  friend class ARENA_CORE;
  friend class PRIM_ID_ITER;
  typedef char* BYTE_PTR;

private:
  ARENA_ITEM_ARRAY(ARENA_ALLOCATOR* allocator, size_t unit_sz, size_t align)
      : _unit_sz(unit_sz),
        _align(align),
        _allocator(allocator),
        _id_array(57),
        _sz_array(57) {
    _id_array.clear();
    _sz_array.clear();
    Reset();
  }

  void* Allocate(size_t bytes, uint32_t* new_id);
  void  Deallocate(uint32_t id);

  uint32_t Compute_new_id(BYTE_PTR addr, size_t sz);
  uint32_t Size() const { return _id_array.size(); }

  void* Find(uint32_t id) const {
    void* ptr = _id_array[id];
    return ptr;
  }
  void Adjust_addr(uint32_t id, size_t ofst) {
    _id_array[id] += ofst;
    _sz_array[id] = -_sz_array[id];
  }

  uint32_t Prev_id(uint32_t base) const;
  uint32_t Next_id(uint32_t base) const;
  void     Close() { Reset(); }
  void     Reset() {}

  typedef PRIM_ID_ITER CONST_ITERATOR;
  typedef PRIM_ID_ITER ITERATOR;

  CONST_ITERATOR Begin() const;
  CONST_ITERATOR End() const;

  constexpr size_t Align() { return _align; }
  constexpr size_t Unit_sz() { return _unit_sz; }

  void Clone(ARENA_ITEM_ARRAY& o) {
    _id_array.clear();
    _sz_array.clear();
    for (uint32_t i = 0; i < o._id_array.size(); i++) {
      uint32_t sz = o._sz_array[i];
      uint32_t new_id;
      BYTE_PTR addr = (BYTE_PTR)Allocate(sz, &new_id);
      memcpy(addr, o._id_array[i], sz);
      AIR_ASSERT(new_id == i);
    }
  }

  BYTE_PTR Archive(BYTE_PTR pos) {
    uint32_t num = _id_array.size();
    memcpy(pos, reinterpret_cast<BYTE_PTR>(&num), sizeof(uint32_t));
    pos += sizeof(uint32_t);

    for (uint32_t i = 0; i < num; i++) {
      uint32_t sz = _sz_array[i];
      memcpy(pos, reinterpret_cast<BYTE_PTR>(&sz), sizeof(uint32_t));
      pos += sizeof(uint32_t);

      memcpy(pos, _id_array[i], sz);
      pos += sz;
    }
    return pos;
  }

  BYTE_PTR Recovery(BYTE_PTR pos) {
    _id_array.clear();
    _sz_array.clear();

    uint32_t num = *reinterpret_cast<uint32_t*>(pos);
    pos += sizeof(uint32_t);

    for (uint32_t i = 0; i < num; i++) {
      uint32_t sz = *reinterpret_cast<uint32_t*>(pos);
      uint32_t new_id;
      BYTE_PTR addr = (BYTE_PTR)Allocate(sz, &new_id);
      memcpy(addr, pos + sizeof(uint32_t), sz);
      pos += sizeof(uint32_t) + sz;
      AIR_ASSERT(new_id == i);
    }
    return pos;
  }

  BYTE_PTR Archive_offset(BYTE_PTR pos, uint32_t* sz) {
    uint32_t num = _id_array.size();
    memcpy(pos, reinterpret_cast<BYTE_PTR>(&num), sizeof(uint32_t));
    pos += sizeof(uint32_t);

    for (uint32_t i = 0; i < num; i++) {
      *sz = (BYTE_PTR)_id_array[i] - _id_array[0];
      memcpy(pos, reinterpret_cast<BYTE_PTR>(sz), sizeof(uint32_t));
      pos += sizeof(uint32_t);
    }
    return pos;
  }

  BYTE_PTR Recovery_offset(BYTE_PTR addr, BYTE_PTR pos) {
    _id_array.clear();
    _sz_array.clear();

    uint32_t num = *reinterpret_cast<uint32_t*>(pos);
    pos += sizeof(uint32_t);

    for (uint32_t i = 0; i < num; i++) {
      uint32_t sz = *reinterpret_cast<uint32_t*>(pos);
      pos += sizeof(uint32_t);
      _id_array.push_back(addr + sz);
    }
    AIR_ASSERT(num == _id_array.size());
    return pos;
  }

  const size_t          _unit_sz;
  const size_t          _align;
  ARENA_ALLOCATOR*      _allocator;
  std::vector<BYTE_PTR> _id_array;
  std::vector<uint32_t> _sz_array;
};

class PRIM_ID_ITER {
  friend class ARENA_ITEM_ARRAY;

public:
  PRIM_ID_ITER() : _id_iter(0), _size(0), _index(0) {}

  PRIM_ID_ITER& operator++() {
    ++_index;
    Adjust();
    return *this;
  }

  uint32_t operator*() const { return _index; }

  bool operator==(const PRIM_ID_ITER& iter) const {
    return (_index == _size) && (iter._index == iter._size);
  }
  bool operator!=(const PRIM_ID_ITER& iter) const { return !(*this == iter); }
  bool Is_null() const { return _index == _size; }
  operator bool() const { return !Is_null(); }

private:
  PRIM_ID_ITER(std::vector<ARENA_ITEM_ARRAY::BYTE_PTR>* arr)
      : _id_iter(arr), _size(arr->size()), _index(0) {
    Adjust();
  }

  void Adjust() {
    for (; _index < _size; _index++) {
      if ((*_id_iter)[_index]) {
        break;
      }
    }
  }

  std::vector<ARENA_ITEM_ARRAY::BYTE_PTR>* _id_iter;

  size_t   _size;
  uint32_t _index;
};

inline ARENA_ITEM_ARRAY::CONST_ITERATOR ARENA_ITEM_ARRAY::Begin() const {
  return CONST_ITERATOR(
      &const_cast<std::vector<ARENA_ITEM_ARRAY::BYTE_PTR>&>(_id_array));
}

inline ARENA_ITEM_ARRAY::CONST_ITERATOR ARENA_ITEM_ARRAY::End() const {
  return CONST_ITERATOR();
}

class ARENA_CORE {
public:
  ARENA_CORE(ARENA_ALLOCATOR* allocator, size_t unit_sz, size_t align,
             uint32_t id, const char* name, bool opened)
      : _item_array(allocator, unit_sz, align),
        _opened(opened),
        _id(id),
        _name(name) {}
  ~ARENA_CORE() { _item_array.Close(); }

  void   Open();
  size_t Close();
  void*  Allocate(size_t bytes, uint32_t* new_id) {
    return _item_array.Allocate(bytes, new_id);
  }
  void     Deallocate(uint32_t id) { _item_array.Deallocate(id); }
  void*    Find(uint32_t id) const { return _item_array.Find(id); }
  uint32_t Size() const { return _item_array.Size(); }
  uint32_t Compute_new_id(char* addr, size_t sz) {
    return _item_array.Compute_new_id(addr, sz);
  }
  void Adjust_addr(uint32_t id, size_t ofst) {
    return _item_array.Adjust_addr(id, ofst);
  }
  typedef ARENA_ITEM_ARRAY::CONST_ITERATOR CONST_ITERATOR;
  CONST_ITERATOR Begin() const { return _item_array.Begin(); }
  CONST_ITERATOR End() const { return _item_array.End(); }

  void Clone(ARENA_CORE& o) { _item_array.Clone(o._item_array); }

  constexpr size_t Align() const { return _item_array._align; }
  constexpr size_t Unit_sz() const { return _item_array._unit_sz; }

  BYTE_PTR Archive(BYTE_PTR pos) { return _item_array.Archive(pos); }
  BYTE_PTR Recovery(BYTE_PTR pos) { return _item_array.Recovery(pos); }

  BYTE_PTR Archive_offset(BYTE_PTR pos, uint32_t* sz) {
    return _item_array.Archive_offset(pos, sz);
  }
  BYTE_PTR Recovery_offset(BYTE_PTR addr, BYTE_PTR pos) {
    return _item_array.Recovery_offset(addr, pos);
  }

  uint32_t    Id() const { return _id; }
  const char* Name() const { return _name; }

private:
  ARENA_ITEM_ARRAY _item_array;
  uint32_t         _id;
  const char*      _name;
  bool             _opened;
};

}  // namespace base
}  // namespace air

#endif
