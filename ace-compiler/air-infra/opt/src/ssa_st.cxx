//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/opt/ssa_st.h"

#include <sstream>

#include "air/opt/ssa_container.h"

namespace air {

namespace opt {

void SSA_SYM::Print(std::ostream& os, uint32_t indent) const {
  os << std::string(indent * air::base::INDENT_SPACE, ' ');
  if (_data->Is_preg()) {
    os << "_preg." << _data->Var_id();
  } else if (_data->Is_addr_datum()) {
    air::base::SYM_ID  sym_id(_data->Var_id());
    air::base::SYM_PTR sym = _cont->Func_scope()->Find_sym(sym_id);
    AIR_ASSERT(sym != air::base::Null_ptr);
    os << sym->Name()->Char_str();
  } else {
    AIR_ASSERT(false);
  }
  uint32_t index = _data->Index();
  if (index != NO_INDEX) {
    os << ".f" << std::dec << index;
  }
}

void SSA_SYM::Print() const {
  Print(std::cout, 0);
  std::cout << std::endl;
}

std::string SSA_SYM::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

SSA_SYM_PTR SSA_VER::Sym() const { return _cont->Sym(Sym_id()); }

air::base::STMT_PTR SSA_VER::Def_stmt() const {
  return _cont->Stmt(Def_stmt_id());
}

PHI_NODE_PTR SSA_VER::Def_phi() const { return _cont->Phi_node(Def_phi_id()); }

CHI_NODE_PTR SSA_VER::Def_chi() const { return _cont->Chi_node(Def_chi_id()); }

void SSA_VER::Print_ver(std::ostream& os) const {
  uint32_t ver = _data->Version();
  if (ver != NO_VER) {
    os << "v" << std::dec << ver;
  } else {
    os << "zero";
  }
}
void SSA_VER::Print(std::ostream& os, uint32_t indent) const {
  _cont->Print(Sym_id(), os, indent);
  os << ".";
  Print_ver(os);
}

void SSA_VER::Print() const {
  Print(std::cout, 0);
  std::cout << std::endl;
}

std::string SSA_VER::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

}  // namespace opt

}  // namespace air
