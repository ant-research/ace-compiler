//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/base/op_stats.h"

#include <iomanip>
#include <sstream>

#include "air/base/meta_info.h"

namespace air {

namespace base {

#define OPC_COL_NUM 32
#define STC_COL_NUM 10
#define DYN_COL_NUM 10

void OP_STATS::Print(std::ostream& os, uint32_t indent) const {
  // print operator table
  if (indent) {
    os << std::string(indent * INDENT_SPACE, ' ');
  }
  os << std::left << std::setw(OPC_COL_NUM) << "OPERATOR";
  os << std::right << std::setw(STC_COL_NUM) << "STATIC";
  os << std::right << std::setw(DYN_COL_NUM) << "DYNAMIC" << std::endl;
  os << std::left << std::setw(OPC_COL_NUM) << "----------------";
  os << std::right << std::setw(STC_COL_NUM) << "--------";
  os << std::right << std::setw(DYN_COL_NUM) << "--------" << std::endl;

  for (uint32_t dom = 0; dom < _dom_stats.size(); ++dom) {
    const OP_COUNTER& op_stats   = _dom_stats[dom];
    bool              prt_domain = true;
    for (uint32_t op = 0; op < op_stats.size(); ++op) {
      if (op_stats[op]._stc_cnt == 0) {
        continue;
      }
      if (indent) {
        os << std::string(indent * INDENT_SPACE, ' ');
      }
      std::string op_name;
      if (prt_domain) {
        op_name.append(META_INFO::Domain_name(dom)).append(2, ':');
        prt_domain = false;
      } else {
        op_name.append(strlen(META_INFO::Domain_name(dom)) + 2, ' ');
      }
      op_name.append(META_INFO::Op_name(dom, op));
      os << std::left << std::setw(OPC_COL_NUM) << op_name;
      os << std::right << std::setw(STC_COL_NUM) << op_stats[op]._stc_cnt;
      os << std::right << std::setw(DYN_COL_NUM) << op_stats[op]._est_dyn_cnt;
      if (op_stats[op]._est_dyn_cnt != op_stats[op]._dyn_cnt) {
        os << " (" << op_stats[op]._dyn_cnt << ")";
      }
      os << std::endl;
    }
  }
  // print call tablea
  if (!_call_stats.empty()) {
    if (indent) {
      os << std::string(indent * INDENT_SPACE, ' ');
    }
    os << std::left << std::setw(OPC_COL_NUM) << "CALL TARGET";
    os << std::right << std::setw(STC_COL_NUM) << "STATIC";
    os << std::right << std::setw(DYN_COL_NUM) << "DYNAMIC" << std::endl;
    os << std::left << std::setw(OPC_COL_NUM) << "----------------";
    os << std::right << std::setw(STC_COL_NUM) << "--------";
    os << std::right << std::setw(DYN_COL_NUM) << "--------" << std::endl;

    for (CALL_COUNTER::const_iterator it = _call_stats.begin();
         it != _call_stats.end(); ++it) {
      os << std::left << std::setw(OPC_COL_NUM)
         << _glob.String(STR_ID(it->first))->Char_str();
      os << std::right << std::setw(STC_COL_NUM) << it->second._stc_cnt;
      os << std::right << std::setw(DYN_COL_NUM) << it->second._est_dyn_cnt;
      if (it->second._est_dyn_cnt != it->second._dyn_cnt) {
        os << " (" << it->second._dyn_cnt << ")";
      }
      os << std::endl;
    }
  }
}

void OP_STATS::Print() const { Print(std::cout, 0); }

std::string OP_STATS::To_str() const {
  std::stringbuf buf;
  std::iostream  os(&buf);
  Print(os);
  return buf.str();
}

}  // namespace base

}  // namespace air
