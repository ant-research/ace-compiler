//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_BINARY_ELF_READ_H
#define AIR_UTIL_BINARY_ELF_READ_H

#include "air/util/binary/elf_hdr.h"
#include "air/util/debug.h"
#include "air/util/file_map.h"

namespace air {
namespace util {

class FILE_MAP;

//! @brief Read AIR from ELF file
class ELF_READ {
public:
  //! @brief Construct a new elf read object
  ELF_READ(const std::string& ifile, std::ostream& os) : _elf(os), _os(os) {
    _map = new FILE_MAP(ifile.c_str(), false);  // O_READ

    Read_ehdr();
    Read_shdr();
  }

  //! @brief Destruct the elf read object
  ~ELF_READ(void) {
    AIR_ASSERT(_map != nullptr);
    delete _map;

    _elf.Print(_os);
  }

  //! @brief Obtain section offset
  template <typename T>
  uint32_t Get_offset(T t) {
    return _elf.Get_offset(t);
  }

  //! @brief Obtain section size
  template <typename T>
  uint32_t Get_size(T t) {
    return _elf.Get_size(t);
  }

  //! @brief Obtain section postion
  template <typename T>
  BYTE_PTR Get_pos(T t) {
    return Get_map_addr() + Get_offset(t);
  }

  //! @brief Obtain section size
  template <typename T>
  uint32_t Get_addralign(T t) {
    return _elf.Get_addralign(t);
  }

  //! @brief Obtain section size
  template <typename T>
  uint32_t Get_entsize(T t) {
    return _elf.Get_entsize(t);
  }

private:
  std::ostream& _os;
  FILE_MAP*     _map;
  ELF_HDR       _elf;

  //! @brief Get mmap address to opened file
  BYTE_PTR Get_map_addr() { return _map->Get_map_addr(); }

  //! @brief Set mmap address to opened file
  void Set_map_addr(BYTE_PTR map) { _map->Set_map_addr(map); }

  //! @brief Get Elf64_Ehdr
  void Read_ehdr() {
    CMPLR_ASSERT(_map->Get_map_addr() != nullptr, "Error: Get_map_addr() addr");

    ELF_EHDR* ehdr = (ELF_EHDR*)_map->Get_map_addr();
    memcpy(_elf.Get_ehdr(), ehdr, sizeof(ELF_EHDR));
  }

  //! @brief Get Elf64_Shdr
  void Read_shdr() {
    CMPLR_ASSERT(_map->Get_map_addr() != nullptr, "Error: Get_map_addr() addr");

    ELF_SHDR* shdr = (ELF_SHDR*)(_map->Get_map_addr() + _elf.Get_shoff());

    _os << "Sections : " << std::endl;
    for (uint32_t id = 0; id < static_cast<uint32_t>(SHDR::MAX); ++id) {
      memcpy(_elf.Get_shdr(id), shdr, sizeof(ELF_SHDR));
      shdr++;
    }
  }

};  // class ELF_READ

}  // namespace util
}  // namespace air

#endif  // AIR_UTIL_BINARY_ELF_READ_H
