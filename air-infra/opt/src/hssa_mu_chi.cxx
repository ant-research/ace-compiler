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
  os << "mu(hcr" << Opnd_id().Value() << ")";
}

void HMU::Print() const {
  Print(std::cout, 0);
  std::cout << std::endl;
}

std::string HMU::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

void HCHI::Print(std::ostream& os, uint32_t indent) const {
  os << "chi<cr" << Result_id().Value();
  os << "/cr" << Opnd_id().Value();
  if (Is_dead()) {
    os << " dead";
  }
  os << ">";
}

void HCHI::Print() const {
  Print(std::cout, 0);
  std::cout << std::endl;
}

std::string HCHI::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

BB_PTR  HPHI::Bb(CFG* cfg) const { return cfg->Bb_ptr(Bb_id()); }
HCR_PTR HPHI::Result() const { return _cont->Cr_ptr(Result_id()); }

HCR_PTR HPHI::Opnd(uint32_t idx) const { return _cont->Cr_ptr(Opnd_id(idx)); }

int32_t HPHI::Opnd_idx(HCR_PTR opnd) const {
  int32_t idx = -1;
  for (int32_t i = 0; i < Size(); ++i) {
    if (Opnd(i) == opnd) {
      idx = i;
      break;
    }
  }
  return idx;
}

void HPHI::Print(std::ostream& os, uint32_t indent) const {
  os << std::string((indent)*INDENT_SPACE, ' ');
  os << "cr" << Result_id().Value();
  os << "=phi<";

  for (uint32_t i = 0; i < Size(); ++i) {
    if (i > 0) {
      os << ", ";
    }
    if (Opnd_id(i) != HCR_ID()) {
      _cont->Cr_ptr(Opnd_id(i))->Print(os);
    } else {
      os << "cr" << Opnd_id(i).Value();
    }
  }
  os << ">";

  if (Is_dead()) {
    os << " dead";
  }
}

void HPHI::Print() const {
  Print(std::cout, 0);
  std::cout << std::endl;
}

std::string HPHI::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

}  // namespace opt
}  // namespace air