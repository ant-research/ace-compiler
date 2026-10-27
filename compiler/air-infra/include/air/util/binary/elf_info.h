//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_BINARY_ELF_INFO_H
#define AIR_UTIL_BINARY_ELF_INFO_H

#include <elf.h>

#include <string>

namespace air {
namespace util {

#define AIR_MAGIC            "AIR"  // AIR IR magic
#define AIR_MAGIC_LEN        (3)    // "AIR" length
#define AIR_PHASE_LEN        (4)    // Max phase abbreviation length
#define AIR_METADATA_VERSION (1)    // Metadata version stored in e_flags

// EI_PAD starts at index 9, we have 7 bytes (9-15) for custom use
// Layout: [9-11] = "AIR" magic, [12-15] = phase abbreviation
#define EI_AIR_MAGIC (9)   // AIR IR magic offset in e_ident
#define EI_AIR_PHASE (12)  // Phase abbreviation offset in e_ident

//
// TODO!! : MUST CHNAGE THIS BEFORE SOURCE GO PUBLIC
// temporarily borrow MIPS_WHIRL for AIR_IR dump
//
#define EF_RISCV_64BIT_AIR EF_MIPS_64BIT_WHIRL
#define SHT_RISCV_AIR      SHT_MIPS_WHIRL

// define all AIR IR related sh_type fields of Elf64_Shdr
// We now made that the same (whenever applicable) with that of
// MIPS extensions for IR in elf.h (check all SHT_MIPS_* in elf.h)
// we will need to set up something similar when go outside
#define SHT_AIR_IR SHT_MIPS_WHIRL

#define SHF_AIR_LCL SHF_MIPS_LOCAL
#define SHF_AIR_STR SHF_MIPS_STRINGS  // SHF_MIPS_STRING

//! @brief AIR file is a 64-bit elf file
#define ET_AIR (ET_LOPROC + 1)

typedef Elf64_Off  ELF_OFF;
typedef Elf64_Ehdr ELF_EHDR;
typedef Elf64_Shdr ELF_SHDR;
typedef Elf64_Sym  ELF_SYM;

//! @brief Section header index of AIR sections
enum class SHDR {
  INVALID      = 0x0,
  LIT_TAB      = 0x1,
  TYPE_TAB     = 0x2,
  ARB_TAB      = 0x3,
  FIELD_TAB    = 0x4,
  PARAM_TAB    = 0x5,
  MAIN_TAB     = 0x6,
  AUX_TAB      = 0x7,
  CONS_TTAB    = 0x8,
  ATTR_TAB     = 0x9,
  FILE_TAB     = 0xa,
  FUNC_DEF_TAB = 0xb,
  BLK_TAB      = 0xc,
  FUNC_DATA    = 0xd,
  COMMENT      = 0xe,  // .comment section for metadata
  STR_TAB      = 0xf,  // string table, right before .shstrtab
  SHSTRTAB     = 0x10,
  MAX          = 0x11
};

enum class SCOPE { GLOB, LOCAL };

//! @brief Glob symbol table index
enum class SYMBOL {
  INVALID      = 0x0,
  TYPE_TAB     = 0x1,
  ARB_TAB      = 0x2,
  FIELD_TAB    = 0x3,
  PARAM_TAB    = 0x4,
  MAIN_TAB     = 0x5,
  AUX_TAB      = 0x6,
  CONS_TTAB    = 0x7,
  ATTR_TAB     = 0x8,
  FILE_TAB     = 0x9,
  FUNC_DEF_TAB = 0xa,
  BLK_TAB      = 0xb,
  MAX          = 0x13
};

//! @brief Local symbol table index
enum class LSYMBOL {
  FUNC_MAIN = 0x0,
  FUNC_AUX  = 0x1,
  FUNC_ATTR = 0x2,
  FUNC_PREG = 0x3,
  FUNC_NODE = 0x4,
  MAX       = 0x5
};

//! @brief Size type of entry in a specific section
//! @enum: FIXED: Entry size is always the same, e.g. symtab for linkage
//! @enum: CLASS_VAR: Variable size due to variations within the type of entry
//! @enum: BYTE_VAR: Variable size with each entry a combination of byte sizes
enum class ENTRY_TYPE { FIXED, BYTE_VAR, CLASS_VAR };

typedef struct {
  const char* _name;  // Section name
  Elf64_Shdr  _shdr;  // Section header
  ENTRY_TYPE  _type;  // Entry size type
} SH_META;

// ！ @brief total size of items
typedef struct {
  uint32_t _unit_sz;  // unit size
  uint32_t _align;    // data align
  uint32_t _size;     // item size
  uint32_t _num;      // item number
  uint32_t _offset;   // file offset
} TABLE_INFO;

typedef char* BYTE_PTR;

//! @brief Read phase abbreviation from ELF file (lightweight, only reads
//! e_ident)
//! @param filepath Path to the ELF file
//! @return Phase abbreviation string, empty string if not a valid AIR IR file
//! @note This function only reads the first 16 bytes of the file (e_ident),
//!       making it efficient for quick phase detection without full ELF parsing
std::string Read_ir_phase_abbr(const std::string& filepath);

//! @brief Get phase abbreviation from full name
//! @param name Full pass name (e.g., "ONNX2AIR")
//! @return Abbreviation string, or nullptr if not found
const char* Get_phase_abbr(const char* name);

//! @brief Get phase index from name or abbreviation
//! @param name_or_abbr Full name or abbreviation
//! @return Phase index, or -1 if not found
int Get_phase_index(const char* name_or_abbr);

//! @brief Get phase full name from abbreviation (case-insensitive)
//! @param abbr Abbreviation (case-insensitive, e.g., "o2a" or "O2A")
//! @return Full name string, or nullptr if not found
const char* Get_phase_name_nocase(const char* abbr);

}  // namespace util
}  // namespace air

#endif  //  AIR_UTIL_BINARY_ELF_INFO_H
