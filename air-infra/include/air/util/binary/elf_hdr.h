//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_BINARY_ELF_HDR_H
#define AIR_UTIL_BINARY_ELF_HDR_H

#include <array>
#include <cstring>
#include <ostream>

#include "air/util/binary/elf_info.h"

namespace air {
namespace util {

extern SH_META Sh_meta[];

//! @brief Define the information for section header
class SECTION_HDR {
public:
  SECTION_HDR(SHDR s) : _s(Sh_meta[static_cast<uint32_t>(s)]) {}

  //! @brief Get section name
  const char* Get_name(SHDR s) { return _s._name; }

  //! @brief Get section name (string tbl index)
  Elf64_Word Get_index() { return _s._shdr.sh_name; }

  //! @brief Get section size
  Elf64_Xword Get_size() { return _s._shdr.sh_size; }

  //! @brief Get section offset
  Elf64_Off Get_offset() { return _s._shdr.sh_offset; }

  //! @brief Get section information
  Elf64_Word Get_info() { return _s._shdr.sh_info; }

  //! @brief Get section alignment
  Elf64_Xword Get_addralign() { return _s._shdr.sh_addralign; }

  //! @brief Get section entry size
  Elf64_Xword Get_entsize() { return _s._shdr.sh_entsize; }

  //! @brief Get the section header
  Elf64_Shdr* Get_shdr() { return &_s._shdr; }

  //! @brief Set section name (string tbl index)
  void Set_index(Elf64_Word idx) { _s._shdr.sh_name = idx; }

  //! @brief Set section size
  void Set_size(Elf64_Xword sz) { _s._shdr.sh_size = sz; }

  //! @brief Set section offset
  void Set_offset(Elf64_Off offset) { _s._shdr.sh_offset = offset; }

  //! @brief Set section information
  void Set_info(Elf64_Word info) { _s._shdr.sh_info = info; }

  //! @brief Set section alignment
  void Set_addralign(Elf64_Xword align) { _s._shdr.sh_addralign = align; }

  //! @brief Set section entry size
  void Set_entsize(Elf64_Xword sz) { _s._shdr.sh_entsize = sz; }

  //! @brief Print information of the section
  void Print(std::ostream& os, uint32_t id) {
    os << std::hex << "    id: " << id << " idx: " << _s._shdr.sh_name
       << "  offset: " << _s._shdr.sh_offset << "  size: " << _s._shdr.sh_size
       << "  name: " << _s._name << std::endl;
  }

private:
  SH_META& _s;  // section meta data
};

//! @brief Define the information for elf header
class ELF_HDR {
public:
  ELF_HDR(std::ostream& os) : _os(os) {
    Set_ehdr();

    Elf64_Word idx = Set_index();
    Set_shstrtab(idx);
  }

  //! @brief Get elf file header
  ELF_EHDR* Get_ehdr() { return &_ehdr; }

  //! @brief Get file offset of section header table
  Elf64_Off Get_shoff() { return _ehdr.e_shoff; }

  //! @brief Get the section header
  Elf64_Shdr* Get_shdr(uint32_t id) { return _s[id].Get_shdr(); }

  //! @brief Get name of the section recorded in .shstrtab
  const char* Get_name(uint32_t id) {
    return _s[id].Get_name(static_cast<SHDR>(id));
  }

  //! @brief Get file offset of the section
  template <typename T>
  Elf64_Off Get_index(T id) {
    return _s[static_cast<uint32_t>(id)].Get_index();
  }

  //! @brief Get size of the section
  template <typename T>
  Elf64_Xword Get_size(T id) {
    return _s[static_cast<uint32_t>(id)].Get_size();
  }

  //! @brief Get file offset of the section
  template <typename T>
  Elf64_Off Get_offset(T id) {
    return _s[static_cast<uint32_t>(id)].Get_offset();
  }

  //! @brief Get information of the section
  template <typename T>
  Elf64_Off Get_info(T id) {
    return _s[static_cast<uint32_t>(id)].Get_info();
  }

  //! @brief Get alignment of the section
  template <typename T>
  Elf64_Xword Get_addralign(T id) {
    return _s[static_cast<uint32_t>(id)].Get_addralign();
  }

  //! @brief Get enty size of the section
  template <typename T>
  Elf64_Xword Get_entsize(T id) {
    return _s[static_cast<uint32_t>(id)].Get_entsize();
  }

  //! @brief Set size of the section
  template <typename T>
  void Set_size(T id, Elf64_Word sz) {
    _s[static_cast<uint32_t>(id)].Set_size(sz);
  }

  //! @brief Set file offset of the section
  template <typename T>
  void Set_offset(T id, Elf64_Word offset) {
    _s[static_cast<uint32_t>(id)].Set_offset(offset);
  }

  //! @brief Set information of the section
  template <typename T>
  void Set_info(T id, Elf64_Word info) {
    _s[static_cast<uint32_t>(id)].Set_info(info);
  }

  //! @brief Set alignment of the section
  template <typename T>
  void Set_addralign(T id, Elf64_Xword align) {
    _s[static_cast<uint32_t>(id)].Set_addralign(align);
  }

  //! @brief Set alignment of the section
  template <typename T>
  void Set_entsize(T id, Elf64_Xword sz) {
    _s[static_cast<uint32_t>(id)].Set_entsize(sz);
  }

  //! @brief Set alignment of the section
  template <typename T>
  void Set_shdr(T id, Elf64_Shdr shdr) {
    memcpy(&_s[static_cast<uint32_t>(id)], &shdr, sizeof(Elf64_Shdr));
  }

  //! @brief Set alignment of the section
  template <typename T>
  void Set_sym(T id, Elf64_Sym& sym) {}

  void Print(std::ostream& os) {
    os << "ELF FILE: " << std::endl;
    os << "  ehdr: " << std::endl;
    os << "    e_ident: " << _ehdr.e_ident << std::endl;
    os << "    e_type: " << _ehdr.e_type << std::endl;
    os << "    e_ident: " << _ehdr.e_ident << std::endl;
    os << "    e_machine: " << _ehdr.e_machine << std::endl;
    os << "    e_version: " << _ehdr.e_version << std::endl;
    os << "    e_shoff: " << _ehdr.e_shoff << std::endl;
    os << "    e_flags: " << _ehdr.e_shoff << std::endl;
    os << "    e_ehsize: " << _ehdr.e_shoff << std::endl;
    os << "    e_shentsize: " << _ehdr.e_shoff << std::endl;
    os << "    e_shoff: " << _ehdr.e_shoff << std::endl;
    os << "    e_shnum: " << _ehdr.e_shoff << std::endl;
    os << "    e_shstrndx: " << _ehdr.e_shoff << std::endl;

    os << "  shdr: " << std::endl;
    for (uint32_t id = 0; id < static_cast<uint32_t>(SHDR::MAX); id++) {
      _s[id].Print(os, id);
    }
  }

private:
  std::ostream&                                             _os;
  ELF_EHDR                                                  _ehdr;
  std::array<SECTION_HDR, static_cast<uint32_t>(SHDR::MAX)> _s = {
      SECTION_HDR(SHDR::INVALID),   SECTION_HDR(SHDR::STR_TAB),
      SECTION_HDR(SHDR::TYPE_TAB),  SECTION_HDR(SHDR::ARB_TAB),
      SECTION_HDR(SHDR::FIELD_TAB), SECTION_HDR(SHDR::PARAM_TAB),
      SECTION_HDR(SHDR::MAIN_TAB),  SECTION_HDR(SHDR::AUX_TAB),
      SECTION_HDR(SHDR::CONS_TTAB), SECTION_HDR(SHDR::ATTR_TAB),
      SECTION_HDR(SHDR::FILE_TAB),  SECTION_HDR(SHDR::FUNC_DEF_TAB),
      SECTION_HDR(SHDR::BLK_TAB),   SECTION_HDR(SHDR::FUNC_DATA),
      SECTION_HDR(SHDR::SHSTRTAB)};

  //! @brief Init elf file header
  void Set_ehdr() {
    strcpy((char*)_ehdr.e_ident, ELFMAG);
    _ehdr.e_ident[EI_CLASS]   = ELFCLASS64;
    _ehdr.e_ident[EI_DATA]    = ELFDATA2LSB;
    _ehdr.e_ident[EI_VERSION] = EV_CURRENT;
    _ehdr.e_type              = ET_AIR;      // AIR elf file
    _ehdr.e_machine           = EM_RISCV;    // RISC-V
    _ehdr.e_version           = EV_CURRENT;  // Default Elf64 Current version
    _ehdr.e_shoff     = sizeof(ELF_EHDR);    // Section header table offset
    _ehdr.e_flags     = EF_RISCV_RVC;        // TODO: define
    _ehdr.e_ehsize    = sizeof(ELF_EHDR);
    _ehdr.e_shentsize = sizeof(ELF_SHDR);
    _ehdr.e_shnum     = static_cast<uint32_t>(SHDR::MAX);
    _ehdr.e_shstrndx  = static_cast<uint32_t>(SHDR::MAX) - 1;
  }

  //! @brief Set index of section
  Elf64_Word Set_index() {
    // section header name
    Elf64_Word idx = 1;
    // section header
    for (uint32_t id = 1; id < static_cast<uint32_t>(SHDR::MAX); id++) {
      _s[id].Set_index(idx);
      idx += strlen(_s[id].Get_name(static_cast<SHDR>(id))) + 1;
    }

    return idx;
  }

  //! @brief Set .shstrtab section information
  void Set_shstrtab(Elf64_Word idx) {
    _s[static_cast<uint32_t>(SHDR::SHSTRTAB)].Set_size(idx);

    Elf64_Word offset =
        sizeof(ELF_EHDR) +
        sizeof(ELF_SHDR) * (static_cast<uint32_t>(SHDR::MAX) - 1);
    _s[static_cast<uint32_t>(SHDR::SHSTRTAB)].Set_offset(offset);
  }
};

}  // namespace util
}  // namespace air

#endif  //  AIR_UTIL_BINARY_ELF_HDR_H
