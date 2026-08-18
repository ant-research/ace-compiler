//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_TARG_INFO_H
#define AIR_BASE_TARG_INFO_H

#include "air/base/st_attr.h"
#include "air/base/st_decl.h"

namespace air {
namespace base {

class TARG_INFO {
public:
  TARG_INFO(SCOPE_BASE* s, ENDIANNESS e, ARCHITECTURE a)
      : _scope(s), _endian(e), _arch(a) {
    AIR_ASSERT(s != nullptr);
  }
  ~TARG_INFO() {}

  ENDIANNESS   Endian() const { return _endian; }
  ARCHITECTURE Arch() const { return _arch; }

  DECLATR_ATTR_ACCESS_API(_attr, _scope)

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

private:
  SCOPE_BASE*  _scope;
  ENDIANNESS   _endian;
  ARCHITECTURE _arch;
  ATTR_ID      _attr;
};

}  // namespace base
}  // namespace air

#endif
