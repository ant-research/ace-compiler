//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_BINARY_ELF_WRITE_H
#define AIR_UTIL_BINARY_ELF_WRITE_H

#include "air/util/binary/elf_hdr.h"
#include "air/util/debug.h"
#include "air/util/file_map.h"

namespace air {
namespace util {

class FILE_MAP;

//! @brief Write AIR to ELF file
class ELF_WRITE {
public:
  //! @brief Construct a new object for write elf file
  ELF_WRITE(const std::string& ofile, std::ostream& os) : _os(os), _elf(os) {
    _map = new FILE_MAP(ofile.c_str(), true);  // O_WRITE

    // Write ELF file basic format
    Write_ehdr();
    Write_shdr();
    Write_shstrtab();
  }

  //! @brief Write ir to file & Destruct irwriter object
  ~ELF_WRITE(void) {
    // Calculate file size
    _map->Remap(Get_pos() - Get_map_addr());

    // Re-write ELF EHDR and SHDR after update
    Write_ehdr();
    Write_shdr();

    AIR_ASSERT(_map != nullptr);
    delete _map;

    _elf.Print(_os);
  }

  //! @brief Get offset position to opened file
  BYTE_PTR Get_pos() { return _pos; }

  //! @brief Get offset position to opened file
  uint32_t Get_offset() { return _pos - Get_map_addr(); }

  //! @brief Set offset position to opened file
  void Set_pos(BYTE_PTR pos) { _pos = pos; }

  //! @brief Address alignment according to the system bits
  uint32_t Align_pos(BYTE_PTR pos, uint32_t align) {
    uint32_t offset = (reinterpret_cast<uintptr_t>(pos) % align);
    pos = reinterpret_cast<BYTE_PTR>(reinterpret_cast<uintptr_t>(pos) + align -
                                     offset);
    return offset;
  }

  //! @brief Update section header
  template <typename T>
  ELF_SHDR* Get_shdr(T t) {
    return _elf.Get_shdr(t);
  }

  //! @brief Update section header
  template <typename T>
  void Update_shdr(T id, BYTE_PTR pos, uint32_t sz, size_t align,
                   size_t entsize) {
    CMPLR_ASSERT(pos != nullptr, "Error: position should not be 0");

    uint32_t offset = pos - Get_map_addr();

    _elf.Set_size(id, sz);
    _elf.Set_offset(id, offset);
    _elf.Set_addralign(id, align);
    _elf.Set_entsize(id, entsize);

    _os << "ELF_WRITE::Update_shdr() id: " << static_cast<uint32_t>(id)
        << " offset: " << offset << " size: " << sz << "  align: " << align
        << "  entysize: " << entsize << std::endl;
  }

private:
  //! @brief Get mmap address to opened file
  BYTE_PTR Get_map_addr() { return _map->Get_map_addr(); }

  //! @brief Set mmap address to opened file
  void Set_map_addr(BYTE_PTR map) { _map->Set_map_addr(map); }

  //! @brief Written elf header
  void Write_ehdr() {
    _pos = Get_map_addr();
    _os << " ELF_WRITE::Write_ehdr:" << std::endl;
    _os << "  ehdr: " << std::hex << "  offset: " << _pos - Get_map_addr()
        << "  size: " << sizeof(ELF_EHDR) << "  e_shoff: " << _elf.Get_shoff()
        << std::endl;

    memcpy(_pos, _elf.Get_ehdr(), sizeof(ELF_EHDR));
    _pos += sizeof(ELF_EHDR);
  }

  //! @brief Written section header
  void Write_shdr() {
    _os << " ELF_WRITE::Write_shdr:" << std::endl;
    for (uint32_t id = 0; id < static_cast<uint32_t>(SHDR::MAX); ++id) {
      _os << std::hex << "  shdr: " << id
          << "  offset: " << _pos - Get_map_addr()
          << "  sh_offset: " << _elf.Get_offset(id)
          << "  sh_size: " << _elf.Get_size(id) << std::endl;

      memcpy(_pos, _elf.Get_shdr(id), sizeof(ELF_SHDR));
      _pos += sizeof(ELF_SHDR);
    }
  }

  //! @brief Written section .shstrtab
  void Write_shstrtab() {
    BYTE_PTR offset = _pos;
    _pos++;

    for (uint32_t id = 1; id < static_cast<uint32_t>(SHDR::MAX); id++) {
      strcpy(_pos, _elf.Get_name(id));
      _pos += strlen(_pos) + 1;
      //_os << "  section: " << id << "  index: " << _elf.Get_index(id)
      //    << "  name: " << _elf.Get_name(id) << std::endl;
    }

    Update_shdr(SHDR::SHSTRTAB, offset, _pos - offset, 1, 0);
  }

private:
  std::ostream& _os;
  FILE_MAP*     _map;  // ELF file
  ELF_HDR       _elf;  // ELF file meta data
  BYTE_PTR      _pos;  // Write file position

};  // class ELF_WRITE

}  // namespace util
}  // namespace air

#endif  // AIR_UTIL_BINARY_ELF_WRITE_H
