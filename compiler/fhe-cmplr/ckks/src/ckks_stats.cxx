//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "ckks_stats.h"

#include <iomanip>
#include <sstream>

namespace fhe {

namespace ckks {

using air::base::INDENT_SPACE;

#define OPC_COL_NUM 12
#define STC_COL_NUM 12
#define OPC_COL_SEP "--------"
#define STC_COL_SEP "------"
#define MKD_COL_SEP "|"

void CKKS_STATS::Fixup_call_stats() {
  for (PER_FUNC_COUNTER::iterator f = _stats.begin(); f != _stats.end(); ++f) {
    if (f->_call_stats.size() == 0) {
      // no call to fixup
      continue;
    }
    // fixup function stats
    if (f->_ckks_stats.size() == 0) {
      f->_ckks_stats.resize(OP_MAX);
    }
    Fixup_caller_stats(f->_ckks_stats, f->_rot_stats, f->_call_stats);

    // fixup per-op stats
    for (PER_OP_COUNTER::iterator o = f->_op_stats.begin();
         o != f->_op_stats.end(); ++o) {
      if (o->_call_stats.size() == 0) {
        // no call to fixup
        continue;
      }
      if (o->_ckks_stats.size() == 0) {
        o->_ckks_stats.resize(OP_MAX);
      }
      Fixup_caller_stats(o->_ckks_stats, o->_rot_stats, o->_call_stats);
    }
  }
}

void CKKS_STATS::Fixup_caller_stats(CKKS_COUNTER&       ckks_sts,
                                    ROT_COUNTER&        rot_sts,
                                    const CALL_COUNTER& cs_sts) {
  for (CALL_COUNTER::const_iterator c = cs_sts.begin(); c != cs_sts.end();
       ++c) {
    FUNC_COUNTER* ce = Find_func_stat(air::base::FUNC_ID(c->first));
    AIR_ASSERT(ce != nullptr);
    AIR_ASSERT_MSG(ce->_call_stats.size() == 0, "TODO: support nested call");
    const LEVEL_COUNTER& lc = c->second;
    for (uint32_t l = 0; l < lc.size(); ++l) {
      if (lc[l]._oper_cnt == 0) {
        continue;
      }
      for (uint32_t op = 0; op < ce->_ckks_stats.size(); ++op) {
        if (ce->_ckks_stats[op].size() == 0) {
          continue;
        }
        // callee output level is 1, so need minus 1 here
        Merge_level_counter(ckks_sts[op], ce->_ckks_stats[op], l - 1,
                            lc[l]._exec_cnt);
      }

      for (ROT_COUNTER::iterator r = ce->_rot_stats.begin();
           r != ce->_rot_stats.end(); ++r) {
        // callee output level is 1, so need minus 1 here
        Merge_level_counter(rot_sts[r->first], r->second, l - 1,
                            lc[l]._exec_cnt);
      }
    }
  }
}

double CKKS_STATS::Op_cost(OP_KIND kind, const core::CTX_PARAM& param,
                           uint32_t level) const {
  switch (kind) {
    case ADD_CC:
    case MUL_CC:
    case RESCALE:
    case MODSWITCH:
    case BOOTSTRAP:
    case CALL:
      break;
    case ADD_CP:
    case MUL_CP:
      // calculate plaintext size
      {
        uint64_t degree = param.Get_poly_degree();
        return (double)(degree * level * sizeof(int64_t)) / (1024 * 1024);
      }
    case ROTATE:
    case RELIN:
      // calculate relin & rotate key
      {
        uint64_t degree  = param.Get_poly_degree();
        uint64_t tot_lev = param.Get_mul_level();
        uint64_t dnum    = param.Get_q_part_num();
        uint64_t pnum    = param.Get_p_prime_num();
        uint64_t load    = dnum * 2 * degree * (level + pnum) * sizeof(int64_t);
        return (double)(load) / (1024 * 1024);
      }
    default:
      break;
  }
  return 0.;
}

double CKKS_STATS::Key_size(const core::CTX_PARAM& param) const {
  uint64_t degree   = param.Get_poly_degree();
  uint64_t tot_lev  = param.Get_mul_level();
  uint64_t dnum     = param.Get_q_part_num();
  uint64_t pnum     = param.Get_p_prime_num();
  uint64_t key_size = dnum * 2 * degree * (tot_lev + pnum) * sizeof(int64_t);
  return (double)(key_size) / (1024 * 1024);
}

void CKKS_STATS::Summary_stats(const CKKS_COUNTER& sts, OP_KIND op,
                               uint64_t& oper, uint64_t& exec, double& load,
                               double& dram) const {
  const core::CTX_PARAM& param = _ctx.Get_ctx_param();
  const LEVEL_COUNTER&   lc    = sts[(int)op];
  for (uint32_t l = 0; l < lc.size(); ++l) {
    if (lc[l]._oper_cnt > 0) {
      oper += lc[l]._oper_cnt;
      exec += lc[l]._exec_cnt;
      double cost = Op_cost(op, param, l);
      load += cost * lc[l]._load_cnt;
      dram += cost * lc[l]._dram_cnt;
    }
  }
}

void CKKS_STATS::Print_mem_stats(const CKKS_COUNTER& ckks_sts,
                                 const ROT_COUNTER& rot_sts, std::ostream& os,
                                 uint32_t indent) const {
  if (ckks_sts.size() == 0) {
    return;
  }
  os << std::string(indent * INDENT_SPACE, ' ') << MKD_COL_SEP;
  os << std::left << std::setw(OPC_COL_NUM) << "DATA" << MKD_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << "COUNT" << MKD_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << "EXEC" << MKD_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << "LOAD(MB)" << MKD_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << "DRAM(MB)" << MKD_COL_SEP
     << std::endl;
  os << std::string(indent * INDENT_SPACE, ' ') << MKD_COL_SEP;
  os << std::left << std::setw(OPC_COL_NUM) << OPC_COL_SEP << MKD_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << STC_COL_SEP << MKD_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << STC_COL_SEP << MKD_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << STC_COL_SEP << MKD_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << STC_COL_SEP << MKD_COL_SEP
     << std::endl;
  // plaintext
  uint64_t ptx_oper = 0, ptx_exec = 0;
  double   ptx_load = 0., ptx_dram = 0.;
  Summary_stats(ckks_sts, ADD_CP, ptx_oper, ptx_exec, ptx_load, ptx_dram);
  Summary_stats(ckks_sts, MUL_CP, ptx_oper, ptx_exec, ptx_load, ptx_dram);
  os << std::string(indent * INDENT_SPACE, ' ') << MKD_COL_SEP;
  os << std::left << std::setw(OPC_COL_NUM) << "PLAINTEXT" << MKD_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << ptx_oper << MKD_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << ptx_exec << MKD_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << ptx_load << MKD_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << ptx_dram << MKD_COL_SEP
     << std::endl;
  // relin key
  uint64_t rel_oper = 0, rel_exec = 0;
  double   rel_load = 0., rel_dram = 0.;
  double   key_size = Key_size(_ctx.Get_ctx_param());
  Summary_stats(ckks_sts, RELIN, rel_oper, rel_exec, rel_load, rel_dram);
  if (rel_oper > 0) {
    rel_dram = key_size;
    os << std::string(indent * INDENT_SPACE, ' ') << MKD_COL_SEP;
    os << std::left << std::setw(OPC_COL_NUM) << "RELIN" << MKD_COL_SEP;
    os << std::right << std::setw(STC_COL_NUM) << rel_oper << MKD_COL_SEP;
    os << std::right << std::setw(STC_COL_NUM) << rel_exec << MKD_COL_SEP;
    os << std::right << std::setw(STC_COL_NUM) << rel_load << MKD_COL_SEP;
    os << std::right << std::setw(STC_COL_NUM) << rel_dram << MKD_COL_SEP
       << std::endl;
  }
  // rotate key
  uint64_t rot_oper = 0, rot_exec = 0;
  double   rot_load = 0., rot_dram = 0.;
  Summary_stats(ckks_sts, ROTATE, rot_oper, rot_exec, rot_load, rot_dram);
  if (rot_oper > 0) {
    rot_dram = key_size * rot_sts.size();
    os << std::string(indent * INDENT_SPACE, ' ') << MKD_COL_SEP;
    os << std::left << std::setw(OPC_COL_NUM) << "ROTATE" << MKD_COL_SEP;
    os << std::right << std::setw(STC_COL_NUM) << rot_oper << MKD_COL_SEP;
    os << std::right << std::setw(STC_COL_NUM) << rot_exec << MKD_COL_SEP;
    os << std::right << std::setw(STC_COL_NUM) << rot_load << MKD_COL_SEP;
    os << std::right << std::setw(STC_COL_NUM) << rot_dram << MKD_COL_SEP
       << std::endl;
  }
  os << std::endl;
}

void CKKS_STATS::Print_ckks_stats(const CKKS_COUNTER& sts, std::ostream& os,
                                  uint32_t indent) const {
  if (sts.size() == 0) {
    return;
  }
  os << std::string(indent * INDENT_SPACE, ' ') << MKD_COL_SEP;
  os << std::left << std::setw(OPC_COL_NUM) << "OP" << MKD_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << "LEVEL" << MKD_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << "COUNT" << MKD_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << "EXEC" << MKD_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << "LOAD(MB)" << MKD_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << "DRAM(MB)" << MKD_COL_SEP
     << std::endl;
  os << std::string(indent * INDENT_SPACE, ' ') << MKD_COL_SEP;
  os << std::left << std::setw(OPC_COL_NUM) << OPC_COL_SEP << MKD_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << STC_COL_SEP << MKD_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << STC_COL_SEP << MKD_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << STC_COL_SEP << MKD_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << STC_COL_SEP << MKD_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << STC_COL_SEP << MKD_COL_SEP
     << std::endl;
  uint64_t tot_oper = 0;
  uint64_t tot_exec = 0;
  double   tot_load = 0.;
  double   tot_dram = 0.;
  for (uint32_t op = 0; op < sts.size(); ++op) {
    const LEVEL_COUNTER& level_stats = sts[op];
    for (uint32_t level = 0; level < level_stats.size(); ++level) {
      if (level_stats[level]._oper_cnt == 0) {
        continue;
      }
      uint32_t oper = level_stats[level]._oper_cnt;
      uint32_t exec = level_stats[level]._exec_cnt;
      uint32_t dram = level_stats[level]._dram_cnt;
      uint32_t load = level_stats[level]._load_cnt;
      double   cost = Op_cost((OP_KIND)op, _ctx.Get_ctx_param(), level);
      os << std::string(indent * INDENT_SPACE, ' ') << MKD_COL_SEP;
      os << std::left << std::setw(OPC_COL_NUM) << Kind_name((OP_KIND)op)
         << MKD_COL_SEP;
      os << std::right << std::setw(STC_COL_NUM) << level << MKD_COL_SEP;
      os << std::right << std::setw(STC_COL_NUM) << oper << MKD_COL_SEP;
      os << std::right << std::setw(STC_COL_NUM) << exec << MKD_COL_SEP;
      os << std::right << std::setw(STC_COL_NUM) << cost * load << MKD_COL_SEP;
      os << std::right << std::setw(STC_COL_NUM) << cost * dram << MKD_COL_SEP
         << std::endl;
      tot_oper += oper;
      tot_exec += exec;
      tot_load += cost * load;
      tot_dram += cost * dram;
    }
  }
  os << std::string(indent * INDENT_SPACE, ' ') << MKD_COL_SEP;
  os << std::left << std::setw(OPC_COL_NUM) << "TOTAL" << MKD_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << "" << MKD_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << tot_oper << MKD_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << tot_exec << MKD_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << tot_load << MKD_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << tot_dram << MKD_COL_SEP
     << std::endl;
  os << std::endl;
}

void CKKS_STATS::Print_call_stats(const CALL_COUNTER& sts, std::ostream& os,
                                  uint32_t indent) const {
  if (sts.size() == 0) {
    return;
  }
  os << std::string(indent * INDENT_SPACE, ' ');
  os << std::left << std::setw(OPC_COL_NUM) << "CALLEE";
  os << std::right << std::setw(STC_COL_NUM) << "LEVEL";
  os << std::right << std::setw(STC_COL_NUM) << "COUNT" << std::endl;
  os << std::string(indent * INDENT_SPACE, ' ');
  os << std::left << std::setw(OPC_COL_NUM) << OPC_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << STC_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << STC_COL_SEP << std::endl;
  for (CALL_COUNTER::const_iterator it = sts.begin(); it != sts.end(); ++it) {
    const LEVEL_COUNTER& level_stats = it->second;
    const char*          callee =
        _glob.Func(air::base::FUNC_ID(it->first))->Name()->Char_str();
    for (uint32_t level = 0; level < level_stats.size(); ++level) {
      if (level_stats[level]._oper_cnt == 0) {
        continue;
      }
      os << std::string(indent * INDENT_SPACE, ' ');
      os << std::left << std::setw(OPC_COL_NUM) << callee;
      os << std::right << std::setw(STC_COL_NUM) << level;
      os << std::right << std::setw(STC_COL_NUM) << level_stats[level]._exec_cnt
         << std::endl;
    }
  }
  os << std::endl;
}

void CKKS_STATS::Print_rot_stats(const ROT_COUNTER& sts, std::ostream& os,
                                 uint32_t indent) const {
  if (sts.size() == 0) {
    return;
  }
  os << std::string(indent * INDENT_SPACE, ' ');
  os << std::left << std::setw(OPC_COL_NUM) << "ROTATE";
  os << std::right << std::setw(STC_COL_NUM) << "LEVEL";
  os << std::right << std::setw(STC_COL_NUM) << "COUNT";
  os << std::right << std::setw(STC_COL_NUM) << "LOAD(MB)" << std::endl;
  os << std::string(indent * INDENT_SPACE, ' ');
  os << std::left << std::setw(OPC_COL_NUM) << OPC_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << STC_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << STC_COL_SEP;
  os << std::right << std::setw(STC_COL_NUM) << STC_COL_SEP << std::endl;
  uint64_t tot_cnt  = 0;
  double   tot_load = 0.;
  for (ROT_COUNTER::const_iterator it = sts.begin(); it != sts.end(); ++it) {
    const LEVEL_COUNTER& level_stats = it->second;
    for (uint32_t level = 0; level < level_stats.size(); ++level) {
      if (level_stats[level]._oper_cnt == 0) {
        continue;
      }
      uint32_t exec = level_stats[level]._exec_cnt;
      uint32_t load = level_stats[level]._load_cnt;
      double   cost = Op_cost(ROTATE, _ctx.Get_ctx_param(), level);
      os << std::string(indent * INDENT_SPACE, ' ');
      os << std::left << std::setw(OPC_COL_NUM) << it->first;
      os << std::right << std::setw(STC_COL_NUM) << level;
      os << std::right << std::setw(STC_COL_NUM) << exec;
      os << std::right << std::setw(STC_COL_NUM) << cost * load << std::endl;
      tot_cnt += exec;
      tot_load += cost * load;
    }
  }
  os << std::string(indent * INDENT_SPACE, ' ');
  os << std::left << std::setw(OPC_COL_NUM) << "TOTAL";
  os << std::right << std::setw(STC_COL_NUM) << "";
  os << std::right << std::setw(STC_COL_NUM) << tot_cnt;
  os << std::right << std::setw(STC_COL_NUM) << tot_load << std::endl;
  os << std::endl;
}

void CKKS_STATS::Print(std::ostream& os, uint32_t indent) const {
  const core::CTX_PARAM& param = _ctx.Get_ctx_param();
  for (PER_FUNC_COUNTER::const_iterator f = _stats.begin(); f != _stats.end();
       ++f) {
    const char* name =
        _glob.String(_glob.Func(f->_func)->Name_id())->Char_str();
    for (PER_OP_COUNTER::const_iterator o = f->_op_stats.begin();
         o != f->_op_stats.end(); ++o) {
      os << "Per-op stats for " << _glob.String(o->_name)->Char_str() << " in "
         << name << ":" << std::endl;
      Print_mem_stats(o->_ckks_stats, o->_rot_stats, os, indent);
      Print_ckks_stats(o->_ckks_stats, os, indent);
      Print_rot_stats(o->_rot_stats, os, indent);
      Print_call_stats(o->_call_stats, os, indent);
    }
  }
  for (PER_FUNC_COUNTER::const_iterator f = _stats.begin(); f != _stats.end();
       ++f) {
    const char* name =
        _glob.String(_glob.Func(f->_func)->Name_id())->Char_str();
    os << "Total stats for " << name << ":" << std::endl;
    Print_mem_stats(f->_ckks_stats, f->_rot_stats, os, indent);
    Print_ckks_stats(f->_ckks_stats, os, indent);
    Print_rot_stats(f->_rot_stats, os, indent);
    Print_call_stats(f->_call_stats, os, indent);
  }
}

void CKKS_STATS::Print() const { Print(std::cout, 0); }

std::string CKKS_STATS::To_str() const {
  std::stringbuf buf;
  std::iostream  os(&buf);
  Print(os);
  return buf.str();
}

}  // namespace ckks

}  // namespace fhe
