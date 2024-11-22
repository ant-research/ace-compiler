//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_ST_ENUM_H
#define AIR_BASE_ST_ENUM_H

#include "air/base/arena.h"
#include "air/base/ptr_wrapper.h"

namespace air {
namespace base {

enum class LANG { C, CPP, RO_CONST, WO_CONST };

enum class ENDIANNESS { LITTLE, BIG };

//! All enumerators are place holders for now
enum class ARCHITECTURE { ARM, X86, RISCV, NV, FPGA };

enum class SYMBOL_CLASS {
  UNKNOWN    = 0x0,
  FUNC       = 0x1,
  ENTRY      = 0x2,
  THUNK      = 0x3,
  ADDR_DATUM = 0x4,
  VAR        = 0x5,
  FORMAL     = 0x6,
  PACKET     = 0x7,
  END        = 0x9
};

enum class STORAGE_CLASS {
  INVALID   = 0x0,
  STATIC    = 0x1,
  IMPORT    = 0x2,
  EXPORT    = 0x3,
  CONCAT    = 0x4,
  OVERLAY   = 0x5,
  OPAQUE    = 0x6,
  DYNAMIC   = 0x7,
  FUNC_LOC  = 0x8,
  INNER_BLK = 0x9
};

enum class BLOCK_KIND {
  INVALID       = 0x0,
  COMP_ENV      = 0x1,
  MODULE        = 0x2,
  FUNC          = 0x3,
  INLINED_FUNC  = 0x4,
  INNER_BLK     = 0x5,
  NAMED_SCOPE   = 0x6,
  UNNAMED_SCOPE = 0x7
};

enum class AUX_KIND {
  UNKNOWN        = 0x0,
  STORAGE        = 0x1,
  FIRST          = STORAGE,
  MODULE         = 0x2,
  SYM            = 0x3,
  INLINED_FUNC   = 0x4,
  PACKET_DATUM   = 0x5,
  LINK_ID        = 0x6,
  SECOND_LINK_ID = 0x7,
  SYM_ID         = 0x8,
  INIT_ENTRY     = 0x9,
  FINI_ENTRY     = 0xa,
  LAST           = FINI_ENTRY
  // TODO: not complete yet
};

enum class TYPE_TRAIT {
  UNKNOWN   = 0b0000,
  PRIMITIVE = 0b0001,
  VA_LIST   = 0b0010,
  POINTER   = 0b0011,
  ARRAY     = 0b0100,
  RECORD    = 0b0101,
  SIGNATURE = 0b0110,
  SUBTYPE   = 0b0111,
  END       = 0b1000
};

enum class PRIMITIVE_TYPE {
  INT_S8      = 0x0,
  INT_S16     = 0x1,
  INT_S32     = 0x2,
  INT_S64     = 0x3,
  INT_U8      = 0x4,
  INT_U16     = 0x5,
  INT_U32     = 0x6,
  INT_U64     = 0x7,
  FLOAT_32    = 0x8,
  FLOAT_64    = 0x9,
  FLOAT_80    = 0xa,
  FLOAT_128   = 0xb,
  COMPLEX_32  = 0xc,
  COMPLEX_64  = 0xd,
  COMPLEX_80  = 0xe,
  COMPLEX_128 = 0xf,
  VOID        = 0x10,
  BOOL        = 0x11,
  END         = 0x12
};

enum class SUBTYPE_KIND {
  UNKNOWN    = 0b0000,
  TYPEDEF    = 0b0001,
  ALIGN      = 0b0010,
  MODIFIER   = 0b0011,
  DESC_ARRAY = 0b0100,
  DESC_PTR   = 0b0101,
  IMAG       = 0b0110,
  PTR_TO_MEM = 0b0111,
  REF        = 0b1000,
  FIXED      = 0b1001,
  ADDR       = 0b1010,
  END        = 0b1011
};

enum class CONSTANT_KIND {
  UNKNOWN           = 0x0,
  SIGNED_INT        = 0x1,
  UNSIGNED_INT      = 0x2,
  PTR_INT           = 0x3,
  FLOAT             = 0x4,
  COMPLEX           = 0x5,
  NULL_PTR          = 0x6,
  VAR_PTR           = 0x7,
  ENTRY_PTR         = 0x8,
  THUNK_PTR         = 0x9,
  ARRAY_ELEM_PTR    = 0xa,
  FIELD_PTR         = 0xb,
  PTR_OFST          = 0xc,
  PTR_FROM_UNSIGNED = 0xd,
  PTR_CAST          = 0xe,
  STR_ARRAY         = 0xf,
  ARRAY             = 0x10,
  ARRAY_ELEM        = 0x11,
  STRUCT            = 0x12,
  STRUCT_FIELD      = 0x13,
  UNION             = 0x14,
  ENTRY_FUNC_DESC   = 0x15,
  THUNK_FUNC_DESC   = 0x16,
  NAMED             = 0x17,
  BOOLEAN           = 0x18,
  LABEL_PTR         = 0x19,
  DECIMAL           = 0x1a,
  EXT_FILE          = 0x1b,
  END               = 0x1c
};

enum ARENA_KIND {
  AK_GLOB_SYM = 0x1,
  AK_GLOB_AUX = 0x2,
  AK_FUNC_DEF = 0x3,
  AK_FUNC_SYM = 0x4,
  AK_FUNC_AUX = 0x5,
  AK_TYPE     = 0x6,
  AK_PARAM    = 0x7,
  AK_FIELD    = 0x8,
  AK_CONSTANT = 0x9,
  AK_BLOCK    = 0xa,
  AK_FILE     = 0xc,
  AK_STRING   = 0xd,
  AK_LABEL    = 0xe,
  AK_CODE     = 0xf,
  AK_ARB      = 0x10,
  AK_ATTR     = 0x11,
  AK_PREG     = 0x12
};

enum class PARAM_KIND { MANDATORY = 0x0, RETURN = 0x1, ELLIPSIS = 0x2 };

enum class FIELD_KIND {
  INVALID          = 0x0,
  REGULAR          = 0x1,
  VTABLE_PTR       = 0x2,
  NON_VIRT_INH_INS = 0x3,
  VIRT_INH_INS     = 0x4,
  VIRT_INH         = 0x5,
  STATIC           = 0x6
};

/**
 * @brief ARB entry flag indicating if bounds are constant
 * and if the entry is the first or last dimension entry
 *
 */
enum class ARB_FLAG {
  LB_CONST     = 0x01,
  UB_CONST     = 0x02,
  STRIDE_CONST = 0x04,
  FIRST_DIM    = 0x08,
  LAST_DIM     = 0x10,
};

enum class REGION_INFO_KIND {
  INVALID       = 0x0,
  PARAL_TASK    = 0x1,
  PARAL_REPL    = 0x2,
  SINGLE_THREAD = 0x3,
  OMP_TASK      = 0x4
};

enum class DATA_ALIGN {
  BAD    = 0xf,
  BIT1   = 0,
  BIT2   = 1,
  BIT4   = 2,
  BIT8   = 3,
  BIT16  = 4,
  BIT32  = 5,
  BIT64  = 6,
  BIT128 = 7,
  BYTE1  = BIT8,
  BYTE2  = BIT16,
  BYTE4  = BIT32,
  BYTE8  = BIT64,
  BYTE16 = BIT128,
  FIRST  = BIT1,
  LAST   = BIT128
};

enum class RECORD_KIND { CLASS = 0x0, STRUCT = 0x1, UNION = 0x2 };

enum class STRIDE_KIND { BIT = 0x0, BYTE = 0x1 };

/**
 * @brief FLAT32 refers to pointer of size 32 and alignment 32,
 *        FLAT64 refers to pointer of size 64 and alignment 64
 *
 */
enum class POINTER_KIND { FLAT32 = 0x0, FLAT64 = 0x1, END = 0x2 };

}  // namespace base
}  // namespace air

#endif
