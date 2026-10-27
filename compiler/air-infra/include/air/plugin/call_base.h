//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_PLUGIN_CALL_BASE_H
#define AIR_PLUGIN_CALL_BASE_H

#include "air/base/container.h"
#include "air/base/opcode.h"
#include "air/base/st.h"
#include "air/core/opcode.h"
#include "air/plugin/call_helper.h"
#include "air/util/debug.h"

using namespace air::base;

namespace air {
namespace plugin {

typedef const char* CONST_BYTE_PTR;

//! @brief BASE Class, Call Python
class CALL_BASE {
public:
  virtual ~CALL_BASE() = default;

  //! @brief Pass the main information of Lowering, including Domain, Phase, etc
  virtual bool Pass_lowering_info(
      std::unordered_map<std::string, std::string> lowering) = 0;

  //! @brief Call Python, Trace, convert IR, and import IR into AIR
  virtual NODE_PTR Callback(const std::vector<std::any>&,
                            const std::string&           action,
                            const std::vector<std::any>& params) = 0;
};

//! @brief Using PYBIND11 Call Python
class CALL_PYBIND11 : public CALL_BASE {
public:
  CALL_PYBIND11()
      : _extension("ace_test"),
        _entry("domain.import_entry"),
        _func("import_node") {}

  bool Pass_lowering_info(
      std::unordered_map<std::string, std::string> lowering) override;
  NODE_PTR Callback(const std::vector<std::any>& cntr,
                    const std::string&           action,
                    const std::vector<std::any>& params) override;

private:
  std::unordered_map<std::string, std::string> _info;
  std::string                                  _extension;
  std::string                                  _entry;
  std::string                                  _func;
};

//! @brief Using Python C API Call Python
class CALL_PYCAPI : public CALL_BASE {
public:
  CALL_PYCAPI()
      : _extension("ace_test"),
        _entry("domain.import_entry"),
        _func("import_node") {}

  void Run(CONST_BYTE_PTR pystr);
  void Run(std::string& pyfile);

  bool Pass_lowering_info(
      std::unordered_map<std::string, std::string> info) override {
    _info = info;
    return false;
  }

  NODE_PTR Callback(const std::vector<std::any>&, const std::string& action,
                    const std::vector<std::any>& params) override;

private:
  std::unordered_map<std::string, std::string> _info;
  std::string                                  _extension;
  std::string                                  _entry;
  std::string                                  _func;
};

}  // namespace plugin
}  // namespace air

#endif