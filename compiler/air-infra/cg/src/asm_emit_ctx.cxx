//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/cg/asm_emit_ctx.h"

#include <cmath>
#include <iomanip>

namespace air {
namespace cg {
enum DIREC_KIND : uint16_t {
  D_FILE     = 0,
  D_GLOB     = 1,
  D_P2A      = 2,
  D_TYPE     = 3,
  D_FUNC_END = 4,
  D_SIZE     = 5,
  D_BB       = 6,
  D_SEC      = 7,
  D_CST      = 8,
};

static const char* Directive_name[] = {
    ".file", ".globl", ".p2align", ".type",     ".Lfunc_end",
    ".size", ".LBB",   ".section", "AIRCG_CST",
};

static const char* Section_name[] = {".bss",  ".data",    ".rodata",
                                     ".text", ".comment", ""};

enum SEC_ATTR : uint8_t {
  ALLOC    = 0,
  EXEC     = 1,
  WRITE    = 2,
  PROGBITS = 3,
  ALIGN    = 4,
};
static const char* Sec_attr_name[] = {"@alloc", "@exec", "@write", "@progbits",
                                      "@align"};

enum SYM_KIND : uint8_t {
  FUNC = 0,
  OBJ  = 1,
};
static const char* Symbol_kind_name[] = {"@function", "@object"};

enum PRIM_TYPE_KIND : uint8_t {
  WORD = 0,
  QUAD = 1,
};
static const char* Prim_type_name[] = {".word", ".quad"};

std::string ASM_EMIT_CTX::Bb_labl_name(BB_PTR bb) const {
  std::string labl_name(Directive_name[D_BB]);
  labl_name += std::to_string(bb->Id().Value());
  return labl_name;
}

std::string ASM_EMIT_CTX::Cst_sym_name(base::CONSTANT_PTR cst) const {
  std::string cst_name(Directive_name[D_CST]);
  cst_name += "_" + std::to_string(cst->Id().Index());
  return cst_name;
}

//! @brief Emit opcode
R_CODE ASM_EMIT_CTX::Emit_opc(INST_PTR inst) {
  uint8_t              isa      = inst->Isa();
  const ISA_INFO_META* isa_info = TARG_INFO_MGR::Isa_info(isa);
  Ofile() << Default_indent() << std::left << std::setw(Opc_width())
          << isa_info->Op_name(inst->Operator());
  return R_CODE::NORMAL;
}

R_CODE ASM_EMIT_CTX::Handle_opnd(OPND_PTR opnd, uint8_t isa) {
  switch (opnd->Kind()) {
    case OPND_KIND::REGISTER: {
      AIR_ASSERT_MSG(!opnd->Is_preg(), "Illegal PREG opnd");
      const char* reg_name =
          TARG_INFO_MGR::Reg_name(opnd->Reg_class(), opnd->Reg_num());
      Ofile() << reg_name;
      return R_CODE::NORMAL;
    }
    case OPND_KIND::BB: {
      BB_PTR targ_bb = Container()->Bb(opnd->Bb());
      Ofile() << Bb_labl_name(targ_bb);
      return R_CODE::NORMAL;
    }
    case OPND_KIND::IMMEDIATE: {
      int32_t val = opnd->Immediate();
      Ofile() << val;
      return R_CODE::NORMAL;
    }
    case OPND_KIND::CONSTANT: {
      bool has_opnd_flag = (opnd->Opnd_flag() > 0);
      if (has_opnd_flag) {
        Ofile() << "%"
                << TARG_INFO_MGR::Isa_info(isa)->Opnd_flag(opnd->Opnd_flag())
                << "(";
      }

      base::CONSTANT_PTR cst = Glob()->Constant(opnd->Constant());
      Ofile() << Cst_sym_name(cst);
      if (has_opnd_flag) {
        Ofile() << ")";
      }
      return R_CODE::NORMAL;
    }
    case OPND_KIND::SYMBOL: {
      base::SYM_ID id = opnd->Symbol();
      AIR_ASSERT(Container() != nullptr);
      base::SYM_PTR sym = Container()->Func_scope()->Sym(id);
      AIR_ASSERT(sym->Is_func());
      Ofile() << sym->Name()->Char_str();
      return R_CODE::NORMAL;
    }
    default: {
      AIR_ASSERT_MSG(false, "Not supported opnd kind");
      return R_CODE::INTERNAL;
    }
  }
  return R_CODE::INTERNAL;
}

void ASM_EMIT_CTX::Emit_func_header(CGIR_CONTAINER* cont) {
  const std::string& indent = Default_indent();
  const char* name = cont->Func_scope()->Owning_func()->Name()->Char_str();
  Ofile() << indent << Directive_name[D_GLOB] << "  " << name
          << "\t# Begin function\n";
  Ofile() << indent << Directive_name[D_P2A] << "  1\n";
  Ofile() << indent << Directive_name[D_TYPE] << " " << name << ","
          << Symbol_kind_name[FUNC] << "\n";
  Ofile() << name << ":\t\t"
          << "# @" << name << "\n";
}

void ASM_EMIT_CTX::Emit_func_end(CGIR_CONTAINER* cont) {
  const std::string& indent = Default_indent();
  std::string        end_label(Directive_name[D_FUNC_END]);
  const char* name = cont->Func_scope()->Owning_func()->Name()->Char_str();
  end_label += "_";
  end_label += name;
  Ofile() << end_label << ":\n";
  Ofile() << indent << Directive_name[D_SIZE] << "  " << name << ", "
          << end_label << "-" << name << "\n";
  Ofile() << "\t\t# End function\n";
}

void ASM_EMIT_CTX::Emit_sec_title(SECTION_KIND kind) {
  Ofile() << Default_indent() << Section_name[kind] << "\n";
}

//! @brief Emit a constant.
R_CODE ASM_EMIT_CTX::Emit_cst(base::CONSTANT_PTR cst) {
  base::TYPE_PTR cst_type = cst->Type();
  AIR_ASSERT(cst_type->Is_array());
  // 1. emit header
  const std::string& indent   = Default_indent();
  const std::string  cst_name = Cst_sym_name(cst);
  Ofile() << indent << Directive_name[D_TYPE] << "  " << cst_name << ","
          << Symbol_kind_name[OBJ] << "\n";

  Ofile() << indent << Directive_name[D_GLOB] << "  " << cst_name << "\n";

  // 2. emit align info
  base::ARRAY_TYPE_PTR arr_type  = cst_type->Cast_to_arr();
  base::TYPE_PTR       elem_type = arr_type->Elem_type();
  AIR_ASSERT(elem_type->Is_int());
  uint32_t size      = elem_type->Byte_size();
  uint32_t size_log2 = std::log2(size);
  AIR_ASSERT(size_log2 == 3);
  Ofile() << indent << Directive_name[D_P2A] << "  " << size_log2 << "\n";

  // 3. emit cst label
  Ofile() << cst_name << ":\n";

  // 4. emit data in constant
  uint32_t elem_cnt = arr_type->Elem_count();
  for (uint32_t idx = 0; idx < elem_cnt; ++idx) {
    uint64_t val = cst->Array_elem<uint64_t>(idx);
    Ofile() << indent << Prim_type_name[QUAD] << "  " << val << "\n";
  }

  // 5. emit cst size
  uint32_t tot_size = size * elem_cnt;
  Ofile() << indent << Directive_name[D_SIZE] << "  " << cst_name << ", "
          << tot_size << "\n";
  return R_CODE::NORMAL;
}

//! @brief Emit constants into .rodata.
R_CODE ASM_EMIT_CTX::Emit_cst() {
  // 1. emit section name
  const std::string& indent = Default_indent();
  Ofile() << indent << Directive_name[D_SEC] << " " << Section_name[RODATA]
          << ",\"a\"," << Sec_attr_name[PROGBITS] << "\n";

  // 2. emit constants in GLOB_SCOPE
  for (base::CONSTANT_ITER cst_itr = Glob()->Begin_const();
       cst_itr != Glob()->End_const(); ++cst_itr) {
    base::CONSTANT_PTR cst = *cst_itr;
    R_CODE             r   = Emit_cst(cst);
    if (r != R_CODE::NORMAL) return r;
  }
  return R_CODE::NORMAL;
}

}  // namespace cg
}  // namespace air