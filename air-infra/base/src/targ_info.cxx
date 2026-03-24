//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <cstring>
#include <sstream>

#include "air/base/st.h"

namespace air {
namespace base {

//=============================================================================
// class TARG_INFO member functions
//=============================================================================

void TARG_INFO::Print(std::ostream& os, uint32_t indent) const {
  if (indent) {
    os << std::string(indent * INDENT_SPACE, ' ');
  }
  os << "TARG_INFO: ENDIAN=" << (_endian == ENDIANNESS::BIG ? "BIG" : "LITTLE")
     << ",ARCH=" << static_cast<uint32_t>(_arch);
  ATTR_LIST attrs(_attr, _scope);
  attrs.Print(os);
  os << std::endl;
}

void TARG_INFO::Print() const { Print(std::cout, 0); }

std::string TARG_INFO::To_str() const {
  std::stringbuf buf;
  std::iostream  os(&buf);
  Print(os);
  return buf.str();
}

}  // namespace base
}  // namespace air
