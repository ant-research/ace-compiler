//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_PLUGIN_PYRUN_H
#define AIR_PLUGIN_PYRUN_H

#include "air/util/debug.h"

namespace air {
namespace plugin {

typedef const char* CONST_BYTE_PTR;

//! @brief Run python code
class PYRUN {
public:
  PYRUN();

  ~PYRUN();

  void Run(CONST_BYTE_PTR pystr);
  void Run(std::string& pyfile);
};

}  // namespace plugin
}  // namespace air

#endif  // AIR_PLUGIN_PYRUN_H