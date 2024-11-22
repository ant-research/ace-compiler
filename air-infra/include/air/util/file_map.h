//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_FILE_MAP_H
#define AIR_UTIL_FILE_MAP_H

#include "air/util/debug.h"

namespace air {
namespace util {

// For 4K page, each kernel page maps to 4Mbytes user address space
#define MAPPED_SIZE 0x400000

//! @brief Encapsulate mmap and provide buffer and pos for upper-layer calls
class FILE_MAP {
public:
  //! @brief Construct a new mmap object
  //! @param op: false is read, true is write
  FILE_MAP(const char* name, bool op);

  //! @brief Destruct mmap object
  ~FILE_MAP() {
    // Don't release map objects for reduce memcpy when Recovery function
    // Code_arena
    if (_op) {
      Unmap();
    }
  }

  //! @brief Get map address of a opened file
  char* Get_map_addr() { return _map; }

  //! @brief Get map size of a opened file
  off_t Get_map_size() { return _map_size; }

  //! @brief Set map address of a opened file
  void Set_map_addr(char* map) { _map = map; }

  //! @brief Set map size of a opened file
  void Set_map_size(off_t size) { _map_size = size; }

  //! @brief remap a opened file
  void Remap(uint32_t sz);

private:
  const char* _file;
  int32_t     _fd;
  char*       _map;
  uint32_t    _file_size;
  off_t       _map_size;
  bool        _op;

  //! @brief Open a file for read | write
  //! @param op is O_RDONLY | O_RDWR | O_CREAT | O_TRUNC
  void Open(uint32_t op);

  //! @brief Close a opened file
  void Close();

  //! @brief mmap for read a file
  //! @param prot is PROT_READ
  //! @param flags is MAP_SHARED | MAP_PRIVATE | MAP_DENYWRITE
  void Read(uint32_t prot, uint32_t flags);

  //! @brief mmap for write a file
  //! @param prot is PROT_READ | PROT_WRITE
  //! @param flags is MAP_SHARED | MAP_PRIVATE | MAP_DENYWRITE
  void Write(uint32_t prot, uint32_t flags);

  //! @brief Unmap a opened file
  void Unmap();

  //! @brief Obtain file name
  const char* Get_file_name() { return _file; }

  //! @brief Obtain file id
  int32_t Get_file_id() { return _fd; }

  //! @brief Obtain file size
  off_t Get_file_size() { return _file_size; }

  //! @brief Set file id
  void Set_file_id(int32_t id) { _fd = id; }

  //! @brief Set file size
  void Set_file_size(uint32_t size) { _file_size = size; }
};

}  // namespace util
}  // namespace air

#endif  // AIR_UTIL_FILE_MAP_H
