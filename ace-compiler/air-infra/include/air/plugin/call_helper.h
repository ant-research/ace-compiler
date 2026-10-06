//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_PLUGIN_HELPER_H
#define AIR_PLUGIN_HELPER_H

#include <any>
#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace air {
namespace plugin {

//! @brief Handles Type conversions for Extension
class CALL_HELPER {
public:
  //! @brief Convert to std::any vectors
  template <typename... Args>
  static std::vector<std::any> To_vec(Args&&... args) {
    return {std::forward<Args>(args)...};
  }

  //! @brief Convert vector to void* ptr
  static void* To_ptr(const std::vector<std::any>& vec) {
    return reinterpret_cast<void*>(const_cast<std::vector<std::any>*>(&vec));
  }

private:
  CALL_HELPER() = delete;
};

}  // namespace plugin
}  // namespace air

#endif  // AIR_PLUGIN_HELPER_H