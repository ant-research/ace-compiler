//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/opt/ssa_node.h"

#include <sstream>

#include "air/base/container_decl.h"
#include "air/base/id_wrapper.h"
#include "air/opt/ssa_container.h"
#include "air/opt/ssa_decl.h"

namespace air {

namespace opt {

SSA_SYM_PTR MU_NODE::Sym(void) const { return Ssa_cont()->Sym(Sym_id()); }
SSA_VER_PTR MU_NODE::Opnd(void) const { return Ssa_cont()->Ver(Opnd_id()); }

MU_NODE_PTR MU_NODE::Next(void) const {
  return Next_id() != air::base::Null_id ? Ssa_cont()->Mu_node(Next_id())
                                         : MU_NODE_PTR();
}

void MU_NODE::Print(std::ostream& os, uint32_t indent) const {
  os << "MU(";
  Ssa_cont()->Print(Sym_id(), os, indent);
  os << ".";
  Ssa_cont()->Print_version(Opnd_id(), os);
  os << ")";
}

void MU_NODE::Print() const {
  Print(std::cout, 0);
  std::cout << std::endl;
}

std::string MU_NODE::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

CHI_NODE_PTR CHI_NODE::Next(void) const {
  return Next_id() != air::base::Null_id ? Ssa_cont()->Chi_node(Next_id())
                                         : CHI_NODE_PTR();
}
SSA_SYM_PTR CHI_NODE::Sym(void) const { return Ssa_cont()->Sym(Sym_id()); }
SSA_VER_PTR CHI_NODE::Result(void) const {
  return Ssa_cont()->Ver(Result_id());
}
SSA_VER_PTR    CHI_NODE::Opnd(void) const { return Ssa_cont()->Ver(Opnd_id()); }
base::STMT_PTR CHI_NODE::Def_stmt(void) const {
  return Ssa_cont()->Container()->Stmt(Def_stmt_id());
}

void CHI_NODE::Print(std::ostream& os, uint32_t indent) const {
  _cont->Print(Sym_id(), os, indent);
  os << ".";
  Ssa_cont()->Print_version(Result_id(), os);

  os << "=CHI(";
  Ssa_cont()->Print_version(Opnd_id(), os);
  os << ")";
  if (Is_dead()) {
    os << " dead";
  }
}

void CHI_NODE::Print() const {
  Print(std::cout, 0);
  std::cout << std::endl;
}

std::string CHI_NODE::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

PHI_NODE_PTR PHI_NODE::Next(void) const {
  return Ssa_cont()->Phi_node(Next_id());
}
SSA_SYM_PTR PHI_NODE::Sym(void) const { return Ssa_cont()->Sym(Sym_id()); }
SSA_VER_PTR PHI_NODE::Result(void) const {
  return Ssa_cont()->Ver(Result_id());
}
SSA_VER_PTR PHI_NODE::Opnd(uint32_t idx) const {
  return Ssa_cont()->Ver(Opnd_id(idx));
}
base::STMT_PTR PHI_NODE::Def_stmt(void) const {
  return Ssa_cont()->Container()->Stmt(Def_stmt_id());
}

void PHI_NODE::Print(std::ostream& os, uint32_t indent) const {
  Ssa_cont()->Print(Sym_id(), os, indent);
  os << ".";
  Ssa_cont()->Print_version(Result_id(), os);
  os << "=PHI(";
  for (uint32_t i = 0; i < Size(); ++i) {
    if (i > 0) {
      os << ", ";
    }
    Ssa_cont()->Print_version(Opnd_id(i), os);
  }
  os << ")";
  if (Is_dead()) {
    os << " dead";
  }
}

void PHI_NODE::Print() const {
  Print(std::cout, 0);
  std::cout << std::endl;
}

std::string PHI_NODE::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

}  // namespace opt

}  // namespace air
