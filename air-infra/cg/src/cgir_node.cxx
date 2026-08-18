//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/cg/cgir_node.h"

#include <iomanip>
#include <sstream>

#include "air/cg/cgir_container.h"
#include "air/cg/targ_info.h"

namespace air {

namespace cg {

void OPND::Print(std::ostream& os, uint32_t indent, const TARG_INFO* ti) const {
  os << std::string(indent * 2, ' ');
  switch (Kind()) {
    case OPND_KIND::REGISTER:
      os << "reg" << Id().Value();
      if (Reg_num() != REG_UNKNOWN) {
        if (ti != nullptr) {
          os << ":" << ti->Reg_name(Reg_class(), Reg_num());
        } else {
          os << ":c" << (uint32_t)Reg_class() << "n" << (uint32_t)Reg_num();
        }
      }
      break;
    case OPND_KIND::IMMEDIATE:
      os << "$" << Immediate();
      break;
    case OPND_KIND::CONSTANT:
      os << "cst" << Constant().Value();
      if (Offset() != 0) {
        if (Offset() > 0) {
          os << "+";
        }
        os << Offset();
      }
      break;
    case OPND_KIND::SYMBOL:
      os << "sym" << Symbol().Value();
      if (Offset() != 0) {
        if (Offset() > 0) {
          os << "+";
        }
        os << Offset();
      }
      break;
    case OPND_KIND::LABEL:
      os << "lab" << Label().Value();
      break;
    default:
      os << "*unk*";
  }
}

void OPND::Print() const { Print(std::cout, 0); }

std::string OPND::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

INST_PTR INST::Prev() const { return _cont->Inst(Prev_id()); }

INST_PTR INST::Next() const { return _cont->Inst(Next_id()); }

OPND_PTR INST::Opnd(uint32_t idx) const { return _cont->Opnd(Opnd_id(idx)); }

OPND_PTR INST::Res(uint32_t idx) const { return _cont->Opnd(Res_id(idx)); }

void INST::Set_opnd(uint32_t idx, OPND_ID id) {
  AIR_ASSERT(idx < Opnd_count());
  _data->_res_opnd[Res_count() + idx] = id;
}

void INST::Set_res(uint32_t idx, OPND_ID id) {
  AIR_ASSERT(idx < Res_count());
  _data->_res_opnd[idx] = id;
}

void INST::Print(std::ostream& os, uint32_t indent) const {
  os << std::string(indent * 2, ' ');
  os << std::left << std::setw(12) << Spos().To_str();
  const TARG_INFO* ti = TARG_INFO_MGR::Targ_info(Isa());
  if (ti != nullptr) {
    os << std::left << std::setw(10) << ti->Op_name(Opcode());
  } else {
    std::string op_str = std::to_string(Isa()) + "." + std::to_string(Opcode());
    os << std::left << std::setw(10) << op_str;
  }
  for (uint32_t i = 0; i < Res_count(); ++i) {
    if (i > 0) {
      os << ", ";
    }
    Res(i)->Print(os, 0, ti);
  }
  if (Res_count() > 0) {
    os << ", ";
  }
  for (uint32_t i = 0; i < Opnd_count(); ++i) {
    if (i > 0) {
      os << ", ";
    }
    Opnd(i)->Print(os, 0, ti);
  }
  os << std::endl;
}

void INST::Print() const { Print(std::cout, 0); }

std::string INST::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

INST_PTR NODE::First() const { return _cont->Inst(_data->_first_inst); }

INST_PTR NODE::Last() const { return _cont->Inst(_data->_last_inst); }

void NODE::Prepend(INST_PTR inst) {
  inst->Set_prev(air::base::Null_id);
  inst->Set_next(First_id());
  if (First_id() != air::base::Null_id) {
    First()->Set_prev(inst->Id());
  }
  Set_first(inst->Id());
}

void NODE::Prepend(INST_PTR pos, INST_PTR inst) {
  if (pos->Id() == First_id()) {
    Prepend(inst);
  } else {
    inst->Set_prev(pos->Prev_id());
    inst->Set_next(pos->Id());
    pos->Prev()->Set_next(inst->Id());
    pos->Set_prev(inst->Id());
  }
}

void NODE::Append(INST_PTR inst) {
  inst->Set_prev(Last_id());
  inst->Set_next(air::base::Null_id);
  if (Last_id() != air::base::Null_id) {
    Last()->Set_next(inst->Id());
  }
  Set_last(inst->Id());
}

void NODE::Append(INST_PTR pos, INST_PTR inst) {
  if (pos->Id() == Last_id()) {
    Append(inst);
  } else {
    inst->Set_prev(pos->Id());
    inst->Set_next(pos->Next_id());
    pos->Next()->Set_prev(inst->Id());
    pos->Set_next(inst->Id());
  }
}

void NODE::Print(std::ostream& os, uint32_t indent) const {
  PARENT::Print(os, indent);
  INST_PTR inst = First();
  while (inst != air::base::Null_ptr) {
    inst->Print(os, indent + 1);
    inst = inst->Next();
  }
}

void NODE::Print() const { Print(std::cout, 0); }

std::string NODE::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

}  // namespace cg

}  // namespace air
