//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/driver/driver_ctx.h"

#include "air/base/meta_info.h"

namespace air {

namespace driver {

void DRIVER_CTX::Handle_global_options() {
  if (_config.Print_meta()) {
    air::base::META_INFO::Print(std::cout);
    Teardown(R_CODE::NORMAL);
  } else if (_config.Print_pass() || _config.Help()) {
    _option_mgr.Print();
    Teardown(R_CODE::NORMAL);
  }
}

void DRIVER_CTX::Teardown(R_CODE rc) { exit((int)rc); }

air::util::TFILE& DRIVER_CTX::Register_tfile(uint32_t id, const char* suffix) {
  auto it = _tfiles.find(id);
  if (it != _tfiles.end()) {
    return *(it->second);
  }
  air::util::TFILE* file = (suffix != nullptr)
                               ? new air::util::TFILE(_option_mgr.Tfile(suffix))
                               : new air::util::TFILE();
  _tfiles.emplace_hint(it, id, file);
  return *file;
}

void DRIVER_CTX::Destroy_tfile() {
  for (auto&& it : _tfiles) {
    delete it.second;
  }
  _tfiles.clear();
}

}  // namespace driver

}  // namespace air
