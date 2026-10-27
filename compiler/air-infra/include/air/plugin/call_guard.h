//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_PLUGIN_CALL_GUARD_H
#define AIR_PLUGIN_CALL_GUARD_H

#include "air/plugin/call_base.h"

namespace air {
namespace plugin {

//! @brief FE GUARD, Used to run Pytorch code
class FE_GUARD {
public:
  FE_GUARD(const std::string& domain, const std::string& phase,
           const std::string& body = "PYBIND11")
      : _domain(domain), _phase(phase), _instance(Get_instance(body)) {
    if (_instance) {
      std::unordered_map<std::string, std::string> info;
      info["domain"] = _domain;
      info["phase"]  = _phase;
      _instance->Pass_lowering_info(info);
    } else {
      AIR_ASSERT_MSG(false, "Transformer() Invalid _instance!");
    }
  }

  //! @brief Call Callback instance to process Torch program
  NODE_PTR Transformer(const std::vector<std::any>& cntr,
                       const std::string&           action,
                       const std::vector<std::any>& params) {
    NODE_PTR new_node;
    if (_instance) {
      new_node = _instance->Callback(cntr, action, params);
    } else {
      AIR_ASSERT_MSG(false, "Transformer() Invalid _instance!");
    }
    return new_node;
  }

  //! @brief The instance program that gets the call response
  std::unique_ptr<CALL_BASE> Get_instance(const std::string& type) {
    if (type == "PYCAPI") {
      return std::make_unique<CALL_PYCAPI>();
    } else if (type == "PYBIND11") {
      return std::make_unique<CALL_PYBIND11>();
    } else {
      return nullptr;
    }
  }

private:
  std::unique_ptr<CALL_BASE> _instance;
  const std::string          _domain;
  const std::string          _phase;
};

}  // namespace plugin
}  // namespace air

#endif  // AIR_PLUGIN_CALL_GUARD_H