//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/base/meta_info.h"

#include <iomanip>

namespace air {
namespace base {

// vector to keep all domain info pointers
std::vector<const DOMAIN_INFO*> META_INFO::Domains;

// get operator category name
const char* META_INFO::Op_cate_name(OPR_CAT cat) {
  static constexpr const char* NAMES[] = {"ENTRY", "PRAGMA", "CFLOW", "LDST",
                                          "STMT",  "EXPR",   "CALL"};
  AIR_STATIC_ASSERT(sizeof(NAMES) / sizeof(NAMES[0]) == (int)OPR_CAT::LAST_CAT);
  return cat < OPR_CAT::LAST_CAT ? NAMES[(int)cat] : "ERROR";
}

// get operator property name
const char* META_INFO::Op_prop_name(OPR_PROP prop) {
  static constexpr const char* NAMES[] = {
      "EX_CHILD", "EX_FIELD", "RET_VAR",  "SCF",      "NON_SCF",  "END_BB",
      "LEAF",     "STMT",     "EXPR",     "STORE",    "LOAD",     "CALL",
      "COMPARE",  "BOOLEAN",  "NOT_EXEC", "SYM",      "TYPE",     "LABEL",
      "OFFSET",   "VALUE",    "FLAGS",    "FIELD_ID", "CONST_ID", "BARRIER",
      "PREFETCH", "ATTR",     "ACC_TYPE", "PREG",     "LIB_CALL"};
  AIR_STATIC_ASSERT(sizeof(NAMES) / sizeof(NAMES[0]) ==
                    (int)OPR_PROP::LAST_PROP);
  return prop < OPR_PROP::LAST_PROP ? NAMES[(int)prop] : "ERROR";
}

// register domain information to global registry
bool META_INFO::Register_domain(const DOMAIN_INFO* info) {
  if (info->_id >= Domains.size()) {
    Domains.resize(info->_id + 1);
  }
  if (Domains[info->_id] != nullptr) {
    return false;
  }
  Domains[info->_id] = info;
  return true;
}

void META_INFO::Print(std::ostream& os) {
  for (uint32_t i = 0; i < Domains.size(); i++) {
    DOMAIN_INFO const* info = Domains[i];
    if (info == nullptr) continue;
    uint32_t did = info->_id;
    AIR_ASSERT(did < Domains.size());
    os << "Domain " << did << ": " << Domains[did]->_name << std::endl;
    for (uint32_t o = 0; o < Domains[did]->_nopr; o++) {
      Print(os, &Domains[did]->_opr_info[o]);
    }
    os << std::endl;
  }
}

void META_INFO::Print(std::ostream& os, const OPR_INFO* opr) {
  os << "  " << std::left << std::setw(16) << opr->_name << "  " << std::setw(6)
     << Op_cate_name(opr->_cate) << " kids: " << std::setw(4) << opr->_nkids;
  bool comma = false;
  for (uint32_t i = 0; i < (uint32_t)OPR_PROP::LAST_PROP; ++i) {
    if ((opr->_prop & (PROP_TO_INT(i))) != 0) {
      if (comma == false) {
        comma = true;
      } else {
        os << ",";
      }
      os << Op_prop_name((OPR_PROP)i);
    }
  }
  os << std::endl;
}

}  // namespace base
}  // namespace air
