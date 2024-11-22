//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_BINARY_AIR2ELF_H
#define AIR_UTIL_BINARY_AIR2ELF_H

#include "air/base/ir_write.h"

namespace air {
namespace util {

//! @brief Write IR to Binary file
class AIR2ELF {
public:
  AIR2ELF(const std::string& ofile, std::ostream& os) : _ir(ofile, os) {}

  //! @brief Archive IR data to Binary file
  void Run(air::base::GLOB_SCOPE* glob) {
    AIR_ASSERT(glob != nullptr);

    _ir.Write_glob(glob);
    _ir.Write_func(glob, SHDR::FUNC_DATA);
  }

private:
  air::base::IR_WRITE _ir;
};

}  // namespace util
}  // namespace air

#endif  // AIR_UTIL_BINARY_AIR2ELF_H