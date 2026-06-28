//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_BINARY_ELF2AIR_H
#define AIR_UTIL_BINARY_ELF2AIR_H

#include "air/base/ir_read.h"

namespace air {
namespace util {

//! @brief Read AIR from ELF binary file
class ELF2AIR {
public:
  ELF2AIR(const std::string& ifile, std::ostream& os) : _ir(ifile, os) {
    _glob = new air::base::GLOB_SCOPE(/*glob->Id()*/ 0, true);
    AIR_ASSERT(_glob != nullptr);
  }

  //! @brief Recovery IR data from Binary file
  air::base::GLOB_SCOPE* Run() {
    _ir.Read_glob(_glob);
    _ir.Read_func(_glob, SHDR::FUNC_DATA);

    return _glob;
  }

  //! @brief Get phase name from ELF header
  const char* Get_phase() const { return _ir.Get_phase(); }

  //! @brief Get metadata version from ELF header
  uint32_t Get_metadata_version() const { return _ir.Get_metadata_version(); }

private:
  air::base::IR_READ     _ir;
  air::base::GLOB_SCOPE* _glob;
};

}  // namespace util
}  // namespace air

#endif  // AIR_UTIL_BINARY_ELF2AIR_H