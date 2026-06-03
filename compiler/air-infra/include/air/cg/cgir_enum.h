//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_CG_CGIR_ENUM_H
#define AIR_CG_CGIR_ENUM_H

#include <string>

#include "air/base/spos.h"
#include "air/util/debug.h"

namespace air {

namespace cg {

//! @brief Operand kind for CGIR instructions
enum class OPND_KIND {
  REGISTER,   //!< Operand is a pseudo or physical register
  IMMEDIATE,  //!< Operand is an integer immediate
  CONSTANT,   //!< Operand references an entry in constant table
  SYMBOL,     //!< Operand references an entry in symbol table
  BB,         //!< Operand is an BB for local branch
  LAST        //!< Last unused operand kind
};

//! @brief Operand kind description
static inline const char* Opnd_kind_desc(OPND_KIND kind) {
  AIR_ASSERT(kind < OPND_KIND::LAST);
  static const char* name[] = {"reg", "imm", "cst", "sym", "lab"};
  return name[(uint8_t)kind];
}

//! @brief Special register number for unknown register
static constexpr uint8_t REG_UNKNOWN = 255;

//! @brief Register flag
enum REG_FLAG : uint32_t {
  REG_ALLOCATABLE = 0x0001,  //!< allocatable
  REG_PARAMETER   = 0x0002,  //!< parameter passing
  REG_RET_VALUE   = 0x0004,  //!< return value
  REG_SCRATCH     = 0x0008,  //!< temporary register
  REG_PRESERVED   = 0x0010,  //!< callee-saved register
  REG_READONLY    = 0x0020,  //!< readonly, writes are ignored
  REG_RET_ADDR    = 0x0040,  //!< return address
  REG_GLOBAL_PTR  = 0x0080,  //!< global pointer
  REG_STACK_PTR   = 0x0100,  //!< stack pointer
  REG_THREAD_PTR  = 0x0200,  //!< thread pointer
  REG_ZERO        = 0x0400,  //!< fixed value of zero
};

//! @brief Instruction flag
enum INST_FLAG : uint64_t {
  INST_CALL        = 0x0001,
  INST_RETURN      = 0x0002,
  INST_BRANCH      = 0x0004,
  INST_CMP         = 0x0008,
  INST_COMMUTABLE  = 0x0010,
  INST_TERMINATOR  = 0x0020,
};

//! @brief Register flag description
static inline std::string Reg_flag_desc(uint32_t flags) {
  static const char* name[] = {"allocatable", "parameter",  "ret_value",
                               "scratch",     "preserved",  "readonly",
                               "ret_addr",    "global_ptr", "stack_addr",
                               "thread_addr", "zero"};
  std::string        desc;
  uint32_t           pos = 0;
  bool               sep = false;
  while (flags) {
    if ((flags & 1) == 1) {
      if (sep) {
        desc += " ";
      } else {
        sep = true;
      }
      AIR_ASSERT(pos < sizeof(name) / sizeof(name[0]));
      desc += name[pos];
    }
    ++pos;
    flags >>= 1;
  }
  return desc;
}

}  // namespace cg

}  // namespace air

#endif  // AIR_CG_CGIR_ENUM_H
