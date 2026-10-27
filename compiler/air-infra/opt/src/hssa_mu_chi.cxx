//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/opt/hssa_mu_chi.h"

#include <sstream>

#include "air/opt/cfg.h"
#include "air/opt/hssa_container.h"

using namespace air::base;
namespace air {

namespace opt {
void HMU::Print(std::ostream& os, uint32_t indent) const {
  os << "mu(expr" << Opnd_id().Value() << ")";
}

void HMU::Print(void) const {
  Print(std::cout, 0);
  std::cout << std::endl;
}

std::string HMU::To_str(void) const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

void HCHI::Print(std::ostream& os, uint32_t indent) const {
  os << "chi<expr" << Result_id().Value();
  os << "/expr" << Opnd_id().Value();
  if (Is_dead()) {
    os << " dead";
  }
  os << ">";
}

void HCHI::Print(void) const {
  Print(std::cout, 0);
  std::cout << std::endl;
}

std::string HCHI::To_str(void) const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

BB_PTR    HPHI::Bb(CFG* cfg) const { return cfg->Bb_ptr(Bb_id()); }
HEXPR_PTR HPHI::Result(void) const { return _cont->Expr_ptr(Result_id()); }

HEXPR_PTR HPHI::Opnd(uint32_t idx) const {
  return _cont->Expr_ptr(Opnd_id(idx));
}

int32_t HPHI::Opnd_idx(HEXPR_PTR opnd) const {
  int32_t idx = -1;
  for (int32_t i = 0; i < (int32_t)Size(); ++i) {
    if (Opnd(i) == opnd) {
      idx = i;
      break;
    }
  }
  return idx;
}

void HPHI::Print(std::ostream& os, uint32_t indent) const {
  os << std::string((indent)*INDENT_SPACE, ' ');
  os << "expr" << Result_id().Value();
  os << "=phi<";

  for (uint32_t i = 0; i < Size(); ++i) {
    if (i > 0) {
      os << ", ";
    }
    if (Opnd_id(i) != HEXPR_ID()) {
      _cont->Expr_ptr(Opnd_id(i))->Print(os);
    } else {
      // Phi operand not yet set — print placeholder with null ID value
      os << "expr" << Opnd_id(i).Value();
    }
  }
  os << ">";

  if (Is_dead()) {
    os << " dead";
  }
}

void HPHI::Print(void) const {
  Print(std::cout, 0);
  std::cout << std::endl;
}

std::string HPHI::To_str(void) const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

}  // namespace opt
}  // namespace air
