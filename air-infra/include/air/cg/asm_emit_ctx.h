//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================
#ifndef AIR_CG_ASM_EMIT_CTX_H
#define AIR_CG_ASM_EMIT_CTX_H

#include <fstream>
#include <ostream>
#include <set>

#include "air/base/st.h"
#include "air/cg/analyze_ctx.h"
#include "air/cg/cgir_container.h"
#include "air/cg/cgir_decl.h"
#include "air/cg/cgir_util.h"
#include "air/cg/config.h"
#include "air/cg/isa_core.h"
#include "air/cg/targ_info.h"
#include "air/driver/driver_ctx.h"
#include "air/util/error.h"
namespace air {
namespace cg {

//! @brief kind of section.
enum SECTION_KIND : uint8_t {
  BSS     = 0,
  DATA    = 1,
  RODATA  = 2,
  TEXT    = 3,
  COMMENT = 4,
  UNKNOWN = 5,
};

//! @brief Context of the ASM emitter providing trace APIs and recording
//! constants used in the current GLOB_SCOPE.
class ASM_EMIT_CTX : public ANALYZE_CTX {
  using GLOB_SCOPE = base::GLOB_SCOPE;

public:
  ASM_EMIT_CTX(const driver::DRIVER_CTX* driver_ctx, const CODE_GEN_CONFIG* cfg,
               GLOB_SCOPE* glob, std::ostream& ofile)
      : _driver_ctx(driver_ctx), _cfg(cfg), _glob(glob), _ofile(ofile) {}

  //! @brief Emit opcode
  R_CODE Emit_opc(INST_PTR inst);
  //! @brief Emit an operand.
  R_CODE Handle_opnd(OPND_PTR opnd, uint8_t isa);

  //! @brief Default instruction handler.
  template <typename VISITOR>
  void Handle_inst(VISITOR* visitor, INST_PTR inst);
  //! @brief Default basic block handler.
  template <typename VISITOR>
  void Handle_bb(VISITOR* visitor, BB_PTR bb);
  //! @brief Emit the function header, including its type and start label.
  void Emit_func_header(CGIR_CONTAINER* cont);
  //! @brief Emit the function end label and its type size.
  void Emit_func_end(CGIR_CONTAINER* cont);
  //! @brief Emit the function.
  template <typename VISITOR>
  void Handle_func(VISITOR* visitor, CGIR_CONTAINER* cont);

  std::string Bb_labl_name(BB_PTR bb) const;
  std::string Cst_sym_name(base::CONSTANT_PTR cst) const;
  void        Emit_sec_title(SECTION_KIND kind);

  //! @brief Emit a constant.
  R_CODE Emit_cst(base::CONSTANT_PTR cst);
  //! @brief Emit constants into .rodata.
  R_CODE Emit_cst();

  //! @brief Return a default indent for instructions.
  std::string Default_indent(void) const {
    return std::string(_cfg->Inst_indent(), ' ');
  }

  //! @brief Return the default opcode width.
  uint32_t Opc_width(void) const { return _cfg->Opc_width(); }

  void Set_container(CGIR_CONTAINER* cont) { _cont = cont; }

  template <typename T>
  ASM_EMIT_CTX& operator<<(const T& str) {
    _ofile << str;
    return *this;
  }

  DECLARE_TRACE_DETAIL_API((*_cfg), _driver_ctx)
private:
  std::ostream&   Ofile(void) { return _ofile; }
  GLOB_SCOPE*     Glob(void) const { return _glob; }
  CGIR_CONTAINER* Container(void) const { return _cont; }

  const driver::DRIVER_CTX* _driver_ctx = nullptr;
  const CODE_GEN_CONFIG*    _cfg        = nullptr;
  GLOB_SCOPE*               _glob       = nullptr;
  CGIR_CONTAINER*           _cont       = nullptr;
  std::ostream&             _ofile;
};

//! @brief Default instruction handler.
template <typename VISITOR>
void ASM_EMIT_CTX::Handle_inst(VISITOR* visitor, INST_PTR inst) {
  uint8_t              isa      = inst->Isa();
  const ISA_INFO_META* isa_info = TARG_INFO_MGR::Isa_info(isa);
  // skip CORE ISA
  if (isa == ISA_CORE) {
    AIR_ASSERT(inst->Opcode() == CORE_CHI);
    return;
  }

  Emit_opc(inst);

  uint32_t res_num  = isa_info->Res_num(inst->Operator());
  uint32_t opnd_num = isa_info->Opnd_num(inst->Operator());
  for (uint32_t idx = 0; idx < res_num; ++idx) {
    OPND_PTR res = inst->Res(idx);
    R_CODE   r   = Handle_opnd(res, isa);
    if (r != R_CODE::NORMAL) return;

    bool is_last_opnd = (opnd_num == 0);
    // print comma to sperate operands
    if (!is_last_opnd) Ofile() << ", ";
  }

  for (uint32_t idx = 0; idx < opnd_num; ++idx) {
    OPND_PTR opnd = inst->Opnd(idx);
    R_CODE   r    = Handle_opnd(opnd, isa);
    if (r != R_CODE::NORMAL) return;

    bool is_last_opnd = ((idx + 1) == opnd_num);
    // print comma to sperate operands
    if (!is_last_opnd) Ofile() << ", ";
  }
  Ofile() << std::endl;
}

template <typename VISITOR>
void ASM_EMIT_CTX::Handle_bb(VISITOR* visitor, BB_PTR bb) {
  if (bb == BB_PTR()) return;
  // skip empty basic blocks with no predecessor or successor
  if (bb->Empty() && (bb->Pred_cnt() == 0 || bb->Succ_cnt() == 0)) {
    return;
  }

  // 1. emit BB label.
  Ofile() << Bb_labl_name(bb) << ":\n";

  // 2. emit instruction.
  INST_PTR inst = bb->First();
  while (inst != INST_PTR()) {
    Handle_inst(visitor, inst);

    if (inst->Next_id() == INST_ID()) break;
    inst = inst->Next();
  }
}

template <typename VISITOR>
void ASM_EMIT_CTX::Handle_func(VISITOR* visitor, CGIR_CONTAINER* cont) {
  Set_container(cont);
  // 1. emit func header
  Emit_func_header(cont);

  // 2. emit IR according LAYOUT
  visitor->template Visit<air::cg::LAYOUT_ITER, air::cg::INST_ITER>(
      air::cg::LAYOUT_ITER(cont));

  // 3. emit end label and size info
  Emit_func_end(cont);
}

}  // namespace cg
}  // namespace air

#endif
