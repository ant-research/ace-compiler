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

  //! @brief Set phase name in e_ident[EI_AIR_PHASE]
  //! @param phase Phase name (full name, will be abbreviated to 4 chars)
  void Set_phase(const char* phase) {
    const char* abbr = Get_phase_abbr(phase);
    if (abbr == nullptr) {
      abbr = phase;  // Use as-is if not found (already an abbreviation)
    }
    strncpy((char*)_ehdr.e_ident + EI_AIR_PHASE, abbr, AIR_PHASE_LEN);
  }

  //! @brief Get phase name from e_ident
  const char* Get_phase() const {
    return (const char*)_ehdr.e_ident + EI_AIR_PHASE;
  }

  //! @brief Set metadata version in e_flags
  void Set_metadata_version(uint32_t ver) { _ehdr.e_flags = ver; }

  //! @brief Get metadata version from e_flags
  uint32_t Get_metadata_version() const { return _ehdr.e_flags; }

  void Print(std::ostream& os) {
    os << "ELF FILE: " << std::endl;
    os << "  ehdr: " << std::endl;
    os << "    e_ident: " << _ehdr.e_ident << std::endl;
    os << "    e_type: " << _ehdr.e_type << std::endl;
    os << "    e_machine: " << _ehdr.e_machine << std::endl;
    os << "    e_version: " << _ehdr.e_version << std::endl;
    os << "    e_shoff: " << _ehdr.e_shoff << std::endl;
    os << "    e_flags: " << _ehdr.e_flags << std::endl;
    os << "    e_ehsize: " << _ehdr.e_ehsize << std::endl;
    os << "    e_shentsize: " << _ehdr.e_shentsize << std::endl;
    os << "    e_shnum: " << _ehdr.e_shnum << std::endl;
    os << "    e_shstrndx: " << _ehdr.e_shstrndx << std::endl;

    os << "  shdr: " << std::endl;
    for (uint32_t id = 0; id < static_cast<uint32_t>(SHDR::MAX); id++) {
      _s[id].Print(os, id);
    }
  }

private:
  std::ostream&                                             _os;
  ELF_EHDR                                                  _ehdr;
  std::array<SECTION_HDR, static_cast<uint32_t>(SHDR::MAX)> _s = {
      SECTION_HDR(SHDR::INVALID),   SECTION_HDR(SHDR::LIT_TAB),
      SECTION_HDR(SHDR::TYPE_TAB),  SECTION_HDR(SHDR::ARB_TAB),
      SECTION_HDR(SHDR::FIELD_TAB), SECTION_HDR(SHDR::PARAM_TAB),
      SECTION_HDR(SHDR::MAIN_TAB),  SECTION_HDR(SHDR::AUX_TAB),
      SECTION_HDR(SHDR::CONS_TTAB), SECTION_HDR(SHDR::ATTR_TAB),
      SECTION_HDR(SHDR::FILE_TAB),  SECTION_HDR(SHDR::FUNC_DEF_TAB),
      SECTION_HDR(SHDR::BLK_TAB),   SECTION_HDR(SHDR::FUNC_DATA),
      SECTION_HDR(SHDR::COMMENT),   SECTION_HDR(SHDR::STR_TAB),
      SECTION_HDR(SHDR::SHSTRTAB)};

  //! @brief Init elf file header
  void Set_ehdr() {
    // Clear the entire header first to ensure all fields are zeroed
    memset(&_ehdr, 0, sizeof(ELF_EHDR));

    // ELF magic and identification
    strcpy((char*)_ehdr.e_ident, ELFMAG);
    _ehdr.e_ident[EI_CLASS]   = ELFCLASS64;   // 64-bit
    _ehdr.e_ident[EI_DATA]    = ELFDATA2LSB;  // Little endian
    _ehdr.e_ident[EI_VERSION] = EV_CURRENT;
    _ehdr.e_ident[EI_OSABI]   = ELFOSABI_NONE;  // No specific OS ABI

    // AIR IR magic at e_ident[EI_AIR_MAGIC] (bytes 9-11)
    strncpy((char*)_ehdr.e_ident + EI_AIR_MAGIC, AIR_MAGIC, AIR_MAGIC_LEN);

    // Use ET_NONE for intermediate representation files (like Open64 .B files)
    // This indicates the file is not an executable or traditional object file
    _ehdr.e_type      = ET_NONE;           // No file type (IR file)
    _ehdr.e_machine   = EM_NONE;           // No specific machine architecture
    _ehdr.e_version   = EV_CURRENT;        // ELF version
    _ehdr.e_entry     = 0;                 // No entry point
    _ehdr.e_phoff     = 0;                 // No program headers
    _ehdr.e_shoff     = sizeof(ELF_EHDR);  // Section header table offset
    _ehdr.e_flags     = 0;                 // No architecture-specific flags
    _ehdr.e_ehsize    = sizeof(ELF_EHDR);
    _ehdr.e_phentsize = 0;  // No program headers
    _ehdr.e_phnum     = 0;  // No program headers
    _ehdr.e_shentsize = sizeof(ELF_SHDR);
    _ehdr.e_shnum     = static_cast<uint32_t>(SHDR::MAX);
    _ehdr.e_shstrndx =
        static_cast<uint32_t>(SHDR::SHSTRTAB);  // Index of .shstrtab
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
    // offset will be set by ELF_WRITE::Write_shstrtab
  }
};

}  // namespace util
}  // namespace air

#endif  //  AIR_UTIL_BINARY_ELF_HDR_H
