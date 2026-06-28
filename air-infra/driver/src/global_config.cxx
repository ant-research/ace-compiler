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
#include "air/util/binary/elf_info.h"
#include "air/util/option.h"

using namespace air::util;

namespace air {
namespace driver {

static GLOBAL_CONFIG Global_config;

static OPTION_DESC Global_option[] = {
    {"help",       "h",  "Print option info",                &Global_config._help,       K_NONE, 0, V_NONE },
    {"show",       "s",  "Show the compilation progress",    &Global_config._show,       K_NONE,
     0,                                                                                             V_NONE },
    {"trace",      "t",  "Enable trace in compiler",         &Global_config._trace,      K_NONE, 0,
     V_NONE                                                                                                },
    {"trace_mp",   "tm", "Enable mempool trace in compiler",
     &Global_config._trace_mp,                                                           K_NONE, 0, V_NONE },
    {"keep",       "k",  "Keep intermediate files",          &Global_config._keep,       K_NONE, 0,
     V_NONE                                                                                                },
    {"verify",     "vr", "Verify intermediate files",        &Global_config._verify,
     K_NONE,                                                                                     0, V_NONE },
    {"print-pass", "",   "Print all pass options",           &Global_config._print_pass,
     K_NONE,                                                                                     0, V_NONE },
    {"print-meta", "",   "Print all meta information",       &Global_config._print_meta,
     K_NONE,                                                                                     0, V_NONE },
    {"O0",         "",   "optimization level 0",             &Global_config._opt_level,  K_NONE, 0,
     V_NONE                                                                                                },
    {"O1",         "",   "optimization level 1",             &Global_config._opt_level,  K_NONE, 0,
     V_NONE                                                                                                },
    {"O2",         "",   "optimization level 2",             &Global_config._opt_level,  K_NONE, 0,
     V_NONE                                                                                                },
    {"O3",         "",   "optimization level 3",             &Global_config._opt_level,  K_NONE, 0,
     V_NONE                                                                                                },
    {"o",          "",   "Set output file name",             &Global_config._ofile,      K_STR,  0, V_SPACE},
    {"dump",       "d",  "Dump IR after specified phase",    &Global_config._dump,       K_STR,
     0,                                                                                             V_EQUAL},
};

static OPTION_DESC_HANDLE Global_option_handle = {
    sizeof(Global_option) / sizeof(Global_option[0]), Global_option};

void GLOBAL_CONFIG::Register_options(DRIVER_CTX* ctx) {
  ctx->Register_top_level_option(&Global_option_handle);
}

//! @brief Normalize dump phase name from abbreviation to full name
//! @param phase User-provided phase name (can be abbreviation or full name)
//! @return Full pass name, or original string if not recognized
static std::string Normalize_dump_phase(const std::string& phase) {
  // Try to find by abbreviation first (case-insensitive, e.g., "o2a" ->
  // "ONNX2AIR")
  const char* full_name = air::util::Get_phase_name_nocase(phase.c_str());
  if (full_name != nullptr) {
    return full_name;
  }
  // Not recognized, return as-is (will be validated later)
  return phase;
}

void GLOBAL_CONFIG::Update_options(const char* ifile) {
  *this = Global_config;
  // Normalize dump phase name (support abbreviations like "o2a" -> "ONNX2AIR")
  if (!_dump.empty()) {
    _dump = Normalize_dump_phase(_dump);
  }
}

void GLOBAL_CONFIG::Print(std::ostream& os) const {
  os << "  Help: " << (_help ? "Yes" : "No") << std::endl;
  os << "  Show: " << (_show ? "Yes" : "No") << std::endl;
  os << "  Trace: " << (_trace ? "Yes" : "No") << std::endl;
  os << "  Trace mempool: " << (_trace_mp ? "Yes" : "No") << std::endl;
  os << "  Keep: " << (_keep ? "Yes" : "No") << std::endl;
  os << "  Verify: " << (_verify ? "Yes" : "No") << std::endl;
  os << "  Print pass: " << (_print_pass ? "Yes" : "No") << std::endl;
  os << "  Print meta: " << (_print_meta ? "Yes" : "No") << std::endl;
  os << "  Output: " << _ofile << std::endl;
  os << "  Dump: " << _dump << std::endl;
}

}  // namespace driver
}  // namespace air
