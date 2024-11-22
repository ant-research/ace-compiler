//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/opt/ssa_node.h"

#include <sstream>

#include "air/opt/ssa_container.h"

namespace air {

namespace opt {

void MU_NODE::Print(std::ostream& os, uint32_t indent) const {
  _cont->Print(Sym_id(), os, indent);
  os << ":mu(";
  _cont->Print_ver(Opnd_id(), os);
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

void CHI_NODE::Print(std::ostream& os, uint32_t indent) const {
  _cont->Print(Sym_id(), os, indent);
  os << ":";
  _cont->Print_ver(Result_id(), os);
  os << "=chi(";
  _cont->Print_ver(Opnd_id(), os);
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

void PHI_NODE::Print(std::ostream& os, uint32_t indent) const {
  _cont->Print(Sym_id(), os, indent);
  os << ":";
  _cont->Print_ver(Result_id(), os);
  os << "=phi(";
  for (uint32_t i = 0; i < Size(); ++i) {
    if (i > 0) {
      os << ", ";
    }
    _cont->Print_ver(Opnd_id(i), os);
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
