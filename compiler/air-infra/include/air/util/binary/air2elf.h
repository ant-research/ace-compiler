//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_UTIL_BINARY_AIR2ELF_H
#define AIR_UTIL_BINARY_AIR2ELF_H

#include <ctime>

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

  //! @brief Archive IR data with metadata to Binary file
  //! @param glob Global scope containing IR
  //! @param phase_name Name of the pass that generated this IR (e.g., "O2A")
  void Run(air::base::GLOB_SCOPE* glob, const std::string& phase_name) {
    AIR_ASSERT(glob != nullptr);

    // Set phase in ELF header e_ident[EI_PAD]
    _ir.Set_phase(phase_name);

    _ir.Write_glob(glob);
    _ir.Write_func(glob, SHDR::FUNC_DATA);

    // Write .comment section with full metadata string
    Write_comment(phase_name);
  }

private:
  //! @brief Write .comment section with metadata string
  void Write_comment(const std::string& phase_name) {
    // Format: "AIR IR v<version>, phase: <phase>, time: <timestamp>"
    std::time_t now = std::time(nullptr);
    char        time_str[32];
    std::strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S",
                  std::localtime(&now));

    std::string comment = "AIR IR v" + std::to_string(AIR_METADATA_VERSION) +
                          ", phase: " + phase_name + ", time: " + time_str;

    _ir.Write_comment(comment);
  }

  air::base::IR_WRITE _ir;
};

}  // namespace util
}  // namespace air

#endif  // AIR_UTIL_BINARY_AIR2ELF_H