//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/driver/global_config.h"

#include <filesystem>

#include "air/driver/driver_ctx.h"
#include "air/util/option.h"

using namespace air::util;

namespace air {
namespace driver {

static GLOBAL_CONFIG Global_config;

static OPTION_DESC Global_option[] = {
    {"help",       "h", "Print option info",              &Global_config._help,       K_NONE, 0, V_NONE },
    {"show",       "s", "Show the compilation progress",  &Global_config._show,       K_NONE,
     0,                                                                                          V_NONE },
    {"trace",      "t", "Enable trace in compiler",       &Global_config._trace,      K_NONE, 0,
     V_NONE                                                                                             },
    {"perf",       "p", "Enable performance in compiler", &Global_config._perf,
     K_NONE,                                                                                  0, V_NONE },
    {"keep",       "k", "Keep intermediate files",        &Global_config._keep,       K_NONE, 0,
     V_NONE                                                                                             },
    {"print-pass", "",  "Print all pass options",         &Global_config._print_pass,
     K_NONE,                                                                                  0, V_NONE },
    {"print-meta", "",  "Print all meta information",     &Global_config._print_meta,
     K_NONE,                                                                                  0, V_NONE },
    {"o",          "",  "Set output file name",           &Global_config._ofile,      K_STR,  0, V_SPACE},
};

static OPTION_DESC_HANDLE Global_option_handle = {
    sizeof(Global_option) / sizeof(Global_option[0]), Global_option};

void GLOBAL_CONFIG::Register_options(DRIVER_CTX* ctx) {
  ctx->Register_top_level_option(&Global_option_handle);
}

void GLOBAL_CONFIG::Update_options(const char* ifile) {
  *this = Global_config;
  // update _ofile according to ifile if it's not set
  if ((ifile != nullptr) && (_ofile.size() == 0)) {
    _ofile = std::filesystem::path(ifile).filename().string() + CFILE_SUFFIX;
  }
}

void GLOBAL_CONFIG::Print(std::ostream& os) const {
  os << "  Help: " << (_help ? "Yes" : "No") << std::endl;
  os << "  Show: " << (_show ? "Yes" : "No") << std::endl;
  os << "  Trace: " << (_trace ? "Yes" : "No") << std::endl;
  os << "  Perf: " << (_perf ? "Yes" : "No") << std::endl;
  os << "  Keep: " << (_keep ? "Yes" : "No") << std::endl;
  os << "  Print pass: " << (_print_pass ? "Yes" : "No") << std::endl;
  os << "  Print meta: " << (_print_meta ? "Yes" : "No") << std::endl;
  os << "  Output: " << _ofile << std::endl;
}

}  // namespace driver
}  // namespace air
