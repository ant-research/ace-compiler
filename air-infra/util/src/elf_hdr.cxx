//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/util/binary/elf_hdr.h"

#include <iostream>

namespace air {
namespace util {

SH_META Sh_meta[] = {
    {"NULL",          {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},   ENTRY_TYPE::FIXED},
    {".strtab",
     {0, SHT_STRTAB, SHF_AIR_STR, 0, 0, 0, 0, 0, 0, 0},
     ENTRY_TYPE::BYTE_VAR                                                },
    {".AIR.type",
     {0, SHT_STRTAB, SHF_EXCLUDE, 0, 0, 0, 0, 0, 0, 0},
     ENTRY_TYPE::FIXED                                                   },
    {".AIR.arb",
     {0, SHT_NOTE, SHF_EXCLUDE, 0, 0, 0, 0, 0, 0, 0},
     ENTRY_TYPE::FIXED                                                   },
    {".AIR.field",
     {0, SHT_NOTE, SHF_EXCLUDE, 0, 0, 0, 0, 0, 0, 0},
     ENTRY_TYPE::FIXED                                                   },
    {".AIR.param",
     {0, SHT_NOTE, SHF_EXCLUDE, 0, 0, 0, 0, 0, 0, 0},
     ENTRY_TYPE::FIXED                                                   },
    {".AIR.main",
     {0, SHT_NOTE, SHF_EXCLUDE, 0, 0, 0, 0, 0, 0, 0},
     ENTRY_TYPE::FIXED                                                   },
    {".AIR.aux",
     {0, SHT_NOTE, SHF_EXCLUDE, 0, 0, 0, 0, 0, 0, 0},
     ENTRY_TYPE::FIXED                                                   },
    {".AIR.cons",
     {0, SHT_NOTE, SHF_EXCLUDE, 0, 0, 0, 0, 0, 0, 0},
     ENTRY_TYPE::FIXED                                                   },
    {".AIR.attr",
     {0, SHT_NOTE, SHF_EXCLUDE, 0, 0, 0, 0, 0, 0, 0},
     ENTRY_TYPE::FIXED                                                   },
    {".AIR.file",
     {0, SHT_NOTE, SHF_EXCLUDE, 0, 0, 0, 0, 0, 0, 0},
     ENTRY_TYPE::FIXED                                                   },
    {".AIR.func_def",
     {0, SHT_NOTE, SHF_EXCLUDE, 0, 0, 0, 0, 0, 0, 0},
     ENTRY_TYPE::FIXED                                                   },
    {".AIR.blk",
     {0, SHT_NOTE, SHF_EXCLUDE, 0, 0, 0, 0, 0, 0, 0},
     ENTRY_TYPE::FIXED                                                   },
    {".AIR.fnhdr",
     {0, SHT_NULL, SHF_AIR_LCL, 0, 0, 0, 0, 0, 0, 0},
     ENTRY_TYPE::BYTE_VAR                                                },
    {".shstrtab",
     {0, SHT_STRTAB, SHT_NULL, 0, 0, 0, 0, 0, 0, 1},
     ENTRY_TYPE::BYTE_VAR                                                }
};

}  // namespace util
}  // namespace air