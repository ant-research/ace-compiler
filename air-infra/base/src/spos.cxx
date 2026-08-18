//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/base/spos.h"

#include <sstream>

namespace air {
namespace base {

void SPOS::Print(std::ostream& os, uint32_t indent) const {
  os << std::string(indent * 2, ' ');
  os << '[' << File() << ':' << Line() << ':' << Col() << "] ";
}

void SPOS::Print() const { Print(std::cout, 0); }

std::string SPOS::To_str() const {
  std::stringbuf buf;
  std::ostream   os(&buf);
  Print(os, 0);
  return buf.str();
}

}  // namespace base
}  // namespace air
