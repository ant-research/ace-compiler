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

namespace air {
namespace util {

#define AIR_MAGIC     "Ant IR,"
#define AIR_MAGIC_LEN (8)
#define AIR_VER       "0.01"
#define AIR_VER_LEN   (5)
#define AIR_PHASE_LEN (8)  // onnx, vect, sihe, ckks, poly, be

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
  STR_TAB      = 0x1,
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
  SHSTRTAB     = 0xe,
  MAX          = 0xf
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

}  // namespace util
}  // namespace air

#endif  //  AIR_UTIL_BINARY_ELF_INFO_H
