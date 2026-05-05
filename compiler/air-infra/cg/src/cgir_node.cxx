//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/cg/cgir_node.h"

#include <iomanip>
#include <ios>
#include <sstream>

#include "air/base/id_wrapper.h"
#include "air/base/st.h"
#include "air/base/st_decl.h"
#include "air/cg/cgir_container.h"
#include "air/cg/cgir_decl.h"
#include "air/cg/targ_info.h"
#include "air/util/debug.h"

namespace air {

namespace cg {

INST_PTR OPND::Def_inst() const { return _cont->Inst(Def_inst_id()); }

void OPND::Print(std::ostream& os, uint32_t indent,
                 const ISA_INFO_META* ti) const {
  std::ios::fmtflags init_fmt = os.flags();
  os.setf(std::ios_base::dec);
  os << std::string(indent * 2, ' ');
  if (ti != nullptr && Opnd_flag() != 0) {
    const char* flag = ti->Opnd_flag(Opnd_flag());
    AIR_ASSERT(flag != nullptr);
    os << "(" << ti->_name << ":" << flag << ") ";
  }
  switch (Kind()) {
    case OPND_KIND::REGISTER:
      if (Is_preg()) {
        os << "%" << Preg().Index() << ":"
           << TARG_INFO_MGR::Reg_class_name(Reg_class());
      } else {
        os << REG_DESC(Reg_class(), Reg_num());
      }
      os << ".v" << _data->_rel_ver;
      break;
    case OPND_KIND::IMMEDIATE:
      os << "$" << Immediate();
      break;
    case OPND_KIND::CONSTANT:
      os << "cst" << Constant().Index();
      if (Offset() != 0) {
        if (Offset() > 0) {
          os << "+";
        }
        os << Offset();
      }
      break;
    case OPND_KIND::SYMBOL: {
      if (_cont->Func_scope() != nullptr) {
        base::SYM_PTR sym = _cont->Func_scope()->Sym(Symbol());
        os << (sym->Id().Is_local() ? "%" : "@") << sym->Name()->Char_str();
      } else {
        os << "sym" << Symbol().Index();
      }
      if (Offset() != 0) {
        if (Offset() > 0) {
          os << "+";
        }
        os << Offset();
      }
      break;
    }
    case OPND_KIND::BB:
      os << "bb" << Bb().Value();
      break;
    default:
      os << "*unk*";
  }
  // restore os fmt flag.
  os.setf(init_fmt);
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

OPND_PTR INST::Res_opnd(uint32_t idx) const {
  return _cont->Opnd(_data->_res_opnd[idx]);
}

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
  const ISA_INFO_META* ti = TARG_INFO_MGR::Isa_info(Isa());
  if (ti != nullptr) {
    os << std::left << std::setw(16) << ti->Op_name(Operator());
  } else {
    std::string op_str =
        std::to_string(Isa()) + "." + std::to_string(Operator());
    os << std::left << std::setw(16) << op_str;
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

INST_PTR BB::First() const { return _cont->Inst(_data->_first_inst); }

INST_PTR BB::Last() const { return _cont->Inst(_data->_last_inst); }

void BB::Prepend(INST_PTR inst) {
  inst->Set_prev(air::base::Null_id);
  inst->Set_next(First_id());
  if (First_id() != air::base::Null_id) {
    First()->Set_prev(inst->Id());
  }
  Set_first(inst->Id());
}

void BB::Prepend(INST_PTR pos, INST_PTR inst) {
  if (pos->Id() == First_id()) {
    Prepend(inst);
  } else {
    inst->Set_prev(pos->Prev_id());
    inst->Set_next(pos->Id());
    pos->Prev()->Set_next(inst->Id());
    pos->Set_prev(inst->Id());
  }
}

void BB::Append(INST_PTR inst) {
  inst->Set_prev(Last_id());
  inst->Set_next(air::base::Null_id);
  if (Last_id() != air::base::Null_id) {
    Last()->Set_next(inst->Id());
  }
  Set_last(inst->Id());
}

void BB::Append(INST_PTR pos, INST_PTR inst) {
  if (pos->Id() == Last_id()) {
    Append(inst);
  } else {
    inst->Set_prev(pos->Id());
    inst->Set_next(pos->Next_id());
    pos->Next()->Set_prev(inst->Id());
    pos->Set_next(inst->Id());
  }
}

void BB::Remove(INST_PTR inst) {
  if (inst == INST_PTR()) return;
  INST_ID prev = inst->Prev_id();
  INST_ID next = inst->Next_id();

  if (prev != INST_ID()) {
    _cont->Inst(prev)->Set_next(next);
  } else {
    // removing the first inst of current basic block
    // {inst, next, ...} -> {next, ...}
    _data->_first_inst = next;
  }
  if (next != INST_ID()) {
    _cont->Inst(next)->Set_prev(prev);
  } else {
    // removing the last inst of current basic block
    // {..., prev, inst} -> {..., prev}
    _data->_last_inst = prev;
  }
}

BB_PTR BB::Next() const { return _cont->Bb(Next_id()); }

BB_PTR BB::Prev() const { return _cont->Bb(Prev_id()); }

void BB::Print(std::ostream& os, uint32_t indent) const {
  PARENT::Print(os, indent);
  INST_PTR inst = First();
  while (inst != air::base::Null_ptr) {
    inst->Print(os, indent + 1);
    inst = inst->Next();
  }
}

void BB::Print() const { Print(std::cout, 0); }

std::string BB::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

}  // namespace cg

}  // namespace air
