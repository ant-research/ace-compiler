//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/util/binary/elf_hdr.h"

#include <strings.h>  // for strcasecmp

#include <cstdio>
#include <cstring>
#include <iostream>

namespace air {
namespace util {

//! @brief Phase information table: {full name, abbreviation}
static const struct {
  const char* _name;  //!< Full pass name (e.g., "ONNX2AIR")
  const char* _abbr;  //!< 4-char abbreviation (e.g., "O2A")
} Phase_info_table[] = {
    {"ONNX2AIR",            "O2A"},
    {"SCHEME_INFO_ANALYZE", "SCH"},
    {"VECTOR",              "VEC"},
    {"SIHE",                "SIH"},
    {"CKKS",                "CKK"},
    {"POLY",                "POL"},
    {"POLY2C",              "P2C"},
};

static constexpr int PHASE_INFO_COUNT =
    sizeof(Phase_info_table) / sizeof(Phase_info_table[0]);

SH_META Sh_meta[] = {
    {"NULL",          {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},     ENTRY_TYPE::FIXED},
    {".AIR.lit",
     {0, SHT_PROGBITS, SHF_AIR_STR, 0, 0, 0, 0, 0, 0, 0},
     ENTRY_TYPE::BYTE_VAR                                                  },
    {".AIR.type",
     {0, SHT_PROGBITS, SHF_EXCLUDE, 0, 0, 0, 0, 0, 0, 0},
     ENTRY_TYPE::FIXED                                                     },
    {".AIR.arb",
     {0, SHT_PROGBITS, SHF_EXCLUDE, 0, 0, 0, 0, 0, 0, 0},
     ENTRY_TYPE::FIXED                                                     },
    {".AIR.field",
     {0, SHT_PROGBITS, SHF_EXCLUDE, 0, 0, 0, 0, 0, 0, 0},
     ENTRY_TYPE::FIXED                                                     },
    {".AIR.param",
     {0, SHT_PROGBITS, SHF_EXCLUDE, 0, 0, 0, 0, 0, 0, 0},
     ENTRY_TYPE::FIXED                                                     },
    {".AIR.main",
     {0, SHT_PROGBITS, SHF_EXCLUDE, 0, 0, 0, 0, 0, 0, 0},
     ENTRY_TYPE::FIXED                                                     },
    {".AIR.aux",
     {0, SHT_PROGBITS, SHF_EXCLUDE, 0, 0, 0, 0, 0, 0, 0},
     ENTRY_TYPE::FIXED                                                     },
    {".AIR.cons",
     {0, SHT_PROGBITS, SHF_EXCLUDE, 0, 0, 0, 0, 0, 0, 0},
     ENTRY_TYPE::FIXED                                                     },
    {".AIR.attr",
     {0, SHT_PROGBITS, SHF_EXCLUDE, 0, 0, 0, 0, 0, 0, 0},
     ENTRY_TYPE::FIXED                                                     },
    {".AIR.file",
     {0, SHT_PROGBITS, SHF_EXCLUDE, 0, 0, 0, 0, 0, 0, 0},
     ENTRY_TYPE::FIXED                                                     },
    {".AIR.func_def",
     {0, SHT_PROGBITS, SHF_EXCLUDE, 0, 0, 0, 0, 0, 0, 0},
     ENTRY_TYPE::FIXED                                                     },
    {".AIR.blk",
     {0, SHT_PROGBITS, SHF_EXCLUDE, 0, 0, 0, 0, 0, 0, 0},
     ENTRY_TYPE::FIXED                                                     },
    {".AIR.fnhdr",
     {0, SHT_PROGBITS, SHF_AIR_LCL, 0, 0, 0, 0, 0, 0, 0},
     ENTRY_TYPE::BYTE_VAR                                                  },
    {".comment",
     {0, SHT_PROGBITS, 0, 0, 0, 0, 0, 0, 0, 1},
     ENTRY_TYPE::BYTE_VAR                                                  },
    {".strtab",
     {0, SHT_STRTAB, SHF_AIR_STR, 0, 0, 0, 0, 0, 0, 0},
     ENTRY_TYPE::BYTE_VAR                                                  },
    {".shstrtab",
     {0, SHT_STRTAB, 0, 0, 0, 0, 0, 0, 0, 1},
     ENTRY_TYPE::BYTE_VAR                                                  }
};

std::string Read_ir_phase_abbr(const std::string& filepath) {
  if (filepath.empty()) return "";

  FILE* fp = fopen(filepath.c_str(), "rb");
  if (fp == nullptr) return "";

  // Read ELF header e_ident bytes (16 bytes)
  unsigned char e_ident[16];
  if (fread(e_ident, 1, 16, fp) != 16) {
    fclose(fp);
    return "";
  }
  fclose(fp);

  // Check ELF magic number
  if (strncmp((const char*)e_ident, ELFMAG, SELFMAG) != 0) {
    return "";
  }

  // Check AIR IR magic at e_ident[EI_AIR_MAGIC]
  if (strncmp((const char*)e_ident + EI_AIR_MAGIC, AIR_MAGIC, AIR_MAGIC_LEN) !=
      0) {
    return "";
  }

  // Phase is stored at e_ident[EI_AIR_PHASE] (4 bytes)
  if (e_ident[EI_AIR_PHASE] != 0) {
    char phase[5] = {0};
    phase[0]      = e_ident[EI_AIR_PHASE];
    phase[1]      = e_ident[EI_AIR_PHASE + 1];
    phase[2]      = e_ident[EI_AIR_PHASE + 2];
    phase[3]      = e_ident[EI_AIR_PHASE + 3];
    return std::string(phase);
  }
  return "";
}

const char* Get_phase_abbr(const char* name) {
  if (name == nullptr) return nullptr;
  for (int i = 0; i < PHASE_INFO_COUNT; i++) {
    if (strcmp(name, Phase_info_table[i]._name) == 0) {
      return Phase_info_table[i]._abbr;
    }
  }
  return nullptr;
}

int Get_phase_index(const char* name_or_abbr) {
  if (name_or_abbr == nullptr) return -1;
  for (int i = 0; i < PHASE_INFO_COUNT; i++) {
    if (strcmp(name_or_abbr, Phase_info_table[i]._name) == 0 ||
        strcmp(name_or_abbr, Phase_info_table[i]._abbr) == 0) {
      return i;
    }
  }
  return -1;
}

const char* Get_phase_name_nocase(const char* abbr) {
  if (abbr == nullptr) return nullptr;
  for (int i = 0; i < PHASE_INFO_COUNT; i++) {
    // Case-insensitive comparison for abbreviation
    if (strcasecmp(abbr, Phase_info_table[i]._abbr) == 0) {
      return Phase_info_table[i]._name;
    }
    // Also check full name (case-insensitive)
    if (strcasecmp(abbr, Phase_info_table[i]._name) == 0) {
      return Phase_info_table[i]._name;
    }
  }
  return nullptr;
}

}  // namespace util
}  // namespace air