//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_CKKS_STATS_H
#define FHE_CKKS_CKKS_STATS_H

#include <vector>

#include "air/base/op_stats.h"
#include "air/core/opcode.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/core/lower_ctx.h"
#include "nn/core/attr.h"
#include "nn/core/pragma.h"

namespace fhe {

namespace ckks {

//! @brief counter for each CKKS operator with level info
//  contains code counter (code size), execution counter (performance), dram
//  counter (storage size) and load counter (data transfer size)
class CKKS_STATS {
public:
  enum OP_KIND {
    ADD_CC,
    ADD_CP,
    MUL_CC,
    MUL_CP,
    ROTATE,
    RELIN,
    RESCALE,
    MODSWITCH,
    BOOTSTRAP,
    CALL,
    OP_MAX
  };

  static inline const char* Kind_name(OP_KIND kind) {
    static const char* name[OP_MAX] = {"add_cc",  "add_cp",    "mul_cc",
                                       "mul_cp",  "rotate",    "relin",
                                       "rescale", "modswitch", "bootstrap"};
    AIR_ASSERT(kind >= ADD_CC && kind < OP_MAX);
    return name[kind];
  }

public:
  //! @brief construct CKKS_STATS with all counters set to 0
  CKKS_STATS(const air::base::GLOB_SCOPE& glob, const core::LOWER_CTX& ctx,
             bool per_op_stats)
      : _glob(glob),
        _ctx(ctx),
        _func_stat(nullptr),
        _op_stat(nullptr),
        _per_op_stats(per_op_stats) {}

  //! @brief handle node to increase counter
  //  code counter increased by 1. exec counter increased by freq
  //  dram counter increased by 1. load counter increased by freq
  void Handle_node(air::base::NODE_PTR node, uint32_t freq, uint32_t est_freq) {
    OP_KIND           kind;
    air::base::OPCODE opc = node->Opcode();
    switch (opc) {
      case OPC_ADD:
        AIR_ASSERT(_ctx.Is_cipher_type(node->Child(0)->Rtype_id()));
        kind =
            _ctx.Is_cipher_type(node->Child(1)->Rtype_id()) ? ADD_CC : ADD_CP;
        break;
      case OPC_MUL:
        AIR_ASSERT(_ctx.Is_cipher_type(node->Child(0)->Rtype_id()));
        kind =
            _ctx.Is_cipher_type(node->Child(1)->Rtype_id()) ? MUL_CC : MUL_CP;
        break;
      case OPC_RELIN:
        kind = RELIN;
        break;
      case OPC_ROTATE:
        kind = ROTATE;
        break;
      case OPC_RESCALE:
        kind = RESCALE;
        break;
      case OPC_MODSWITCH:
        kind = MODSWITCH;
        break;
      case OPC_BOOTSTRAP:
        kind = BOOTSTRAP;
        break;
      case air::core::OPC_PRAGMA:
        if (_per_op_stats) {
          if (node->Pragma_id() == nn::core::PRAGMA_OP_START) {
            AIR_ASSERT(_op_stat == nullptr);
            _op_stat = New_op_stat(air::base::STR_ID(node->Pragma_arg1()));
          } else if (node->Pragma_id() == nn::core::PRAGMA_OP_END) {
            AIR_ASSERT(_op_stat != nullptr);
            AIR_ASSERT(_op_stat->_name.Value() == node->Pragma_arg1());
            _op_stat = nullptr;
          }
        }
        return;
      case air::core::OPC_FUNC_ENTRY:
        _func_stat = New_func_stat(node->Func_scope()->Owning_func_id());
        return;
      default:
        return;
    }
    AIR_ASSERT((int)kind < OP_MAX);
    AIR_ASSERT(_func_stat != nullptr);
    const uint32_t* level_ptr =
        node->Attr<uint32_t>(core::FHE_ATTR_KIND::LEVEL);
    AIR_ASSERT(level_ptr != nullptr);
    uint32_t level = *level_ptr;
    Update_ckks_stats(_func_stat->_ckks_stats, kind, level, est_freq);
    if (_op_stat != nullptr) {
      Update_ckks_stats(_op_stat->_ckks_stats, kind, level, est_freq);
    }

    if (kind == ROTATE) {
      uint32_t   rot_cnt = 0;
      const int* rot_idx = node->Attr<int>(nn::core::ATTR::RNUM, &rot_cnt);
      AIR_ASSERT(rot_idx != nullptr && rot_cnt > 0);
      Update_rot_map(_func_stat->_rot_stats, rot_idx, rot_cnt, level, est_freq);
      if (_op_stat != nullptr) {
        Update_rot_map(_op_stat->_rot_stats, rot_idx, rot_cnt, level, est_freq);
      }
    }
  }

  //! @brief increase counter for CALL
  //  static counter increased by 1. dynamic counter increased by freq
  void Handle_call(air::base::NODE_PTR node, uint32_t freq, uint32_t est_freq) {
    AIR_ASSERT(node->Opcode() == air::core::OPC_CALL);
    const uint32_t* level_ptr =
        node->Attr<uint32_t>(core::FHE_ATTR_KIND::LEVEL);
    AIR_ASSERT(level_ptr != nullptr);
    uint32_t level = *level_ptr;
    Update_call_stats(_func_stat->_call_stats,
                      node->Entry()->Owning_func_id().Value(), level, est_freq);
    if (_op_stat != nullptr) {
      Update_call_stats(_op_stat->_call_stats,
                        node->Entry()->Owning_func_id().Value(), level,
                        est_freq);
    }
  }

  //! @brief fix up call stats to integrate callee's stats to caller
  void Fixup_call_stats();

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

private:
  // disable copy constructor and assign operator
  CKKS_STATS(const CKKS_STATS&)            = delete;
  CKKS_STATS& operator=(const CKKS_STATS&) = delete;

  // counter
  struct COUNTER {
    uint32_t _oper_cnt;  // operator counter (code size)
    uint32_t _exec_cnt;  // execution counter (performance)
    uint32_t _dram_cnt;  // dram counter (memory usage)
    uint32_t _load_cnt;  // load counter (data transfer)
    COUNTER() : _oper_cnt(0), _exec_cnt(0), _dram_cnt(0), _load_cnt(0) {}
  };

  // per-level counter. array index is level
  typedef std::vector<COUNTER> LEVEL_COUNTER;
  // per-level counter for each op. array index is OP_KIND
  typedef std::vector<LEVEL_COUNTER> CKKS_COUNTER;
  // per-level counter for each call. map key is entry id
  typedef std::unordered_map<uint32_t, LEVEL_COUNTER> CALL_COUNTER;
  // per-level counter for each rotate. map key is rotate step
  typedef std::unordered_map<int32_t, LEVEL_COUNTER> ROT_COUNTER;

  // counter for each operator
  struct OP_COUNTER {
    air::base::STR_ID _name;        // NN op name
    CKKS_COUNTER      _ckks_stats;  // operator stats for NN op
    CALL_COUNTER      _call_stats;  // call stats for NN op
    ROT_COUNTER       _rot_stats;   // rotate stats for NN op

    OP_COUNTER(air::base::STR_ID name) : _name(name) {}
  };

  // per-op counter
  typedef std::vector<OP_COUNTER> PER_OP_COUNTER;

  // counter for whole FUNC
  struct FUNC_COUNTER {
    air::base::FUNC_ID _func;        // function id
    PER_OP_COUNTER     _op_stats;    // stats for each NN op
    CKKS_COUNTER       _ckks_stats;  // operator stats for whole func
    CALL_COUNTER       _call_stats;  // call stats for whole func
    ROT_COUNTER        _rot_stats;   // rotate stats for whole func

    FUNC_COUNTER(air::base::FUNC_ID func) : _func(func) {}

    OP_COUNTER* New_op_stat(air::base::STR_ID name) {
      _op_stats.emplace_back(name);
      return &_op_stats.back();
    }
  };

  // per-func counter
  typedef std::vector<FUNC_COUNTER> PER_FUNC_COUNTER;

  FUNC_COUNTER* New_func_stat(air::base::FUNC_ID func) {
    _stats.emplace_back(func);
    return &_stats.back();
  }

  OP_COUNTER* New_op_stat(air::base::STR_ID name) {
    AIR_ASSERT(_func_stat != nullptr);
    return _func_stat->New_op_stat(name);
  }

  FUNC_COUNTER* Find_func_stat(air::base::FUNC_ID func) {
    for (PER_FUNC_COUNTER::iterator it = _stats.begin(); it != _stats.end();
         ++it) {
      if (it->_func == func) {
        return &(*it);
      }
    }
    AIR_ASSERT(false);
    return nullptr;
  }

  void Update_level_counter(LEVEL_COUNTER& lc, uint32_t level, uint32_t freq,
                            bool same_data) {
    if (level >= lc.size()) {
      lc.resize(level + 1);
    }
    lc[level]._oper_cnt += 1;
    lc[level]._exec_cnt += freq;
    lc[level]._dram_cnt += same_data ? 1 : freq;
    lc[level]._load_cnt += freq;
  }

  void Merge_level_counter(LEVEL_COUNTER& dst, const LEVEL_COUNTER& src,
                           uint32_t level, uint32_t freq) {
    if (src.size() + level > dst.size()) {
      dst.resize(src.size() + level);
    }
    for (uint32_t i = 0; i < src.size(); ++i) {
      if (src[i]._oper_cnt > 0) {
        dst[i + level]._oper_cnt += src[i]._oper_cnt;
        dst[i + level]._exec_cnt += src[i]._exec_cnt * freq;
        dst[i + level]._dram_cnt += src[i]._dram_cnt;
        dst[i + level]._load_cnt += src[i]._load_cnt * freq;
      }
    }
  }

  void Update_ckks_stats(CKKS_COUNTER& sts, OP_KIND kind, uint32_t level,
                         uint32_t freq) {
    if (sts.size() == 0) {
      sts.resize(OP_MAX);
    }
    // assume ADD_CP/MUL_CP constant operand always different
    bool same_data = (kind != ADD_CP && kind != MUL_CP);
    Update_level_counter(sts[(int)kind], level, freq, same_data);
  }

  void Update_call_stats(CALL_COUNTER& sts, uint32_t id, uint32_t level,
                         uint32_t freq) {
    Update_level_counter(sts[id], level, freq, true);
  }

  void Update_rot_map(ROT_COUNTER& sts, const int* rot_idx, uint32_t cnt,
                      uint32_t level, uint32_t freq) {
    for (uint32_t i = 0; i < cnt; ++i) {
      // can't use freq because rot_idx already counts all iterations
      int step = rot_idx[i];
      Update_level_counter(sts[step], level, 1, true);
    }
  }

  void Fixup_caller_stats(CKKS_COUNTER& ckks_sts, ROT_COUNTER& rot_sts,
                          const CALL_COUNTER& cs_sts);

  double Op_cost(OP_KIND kind, const core::CTX_PARAM& param,
                 uint32_t level) const;

  double Key_size(const core::CTX_PARAM& param) const;

  void Summary_stats(const CKKS_COUNTER& sts, OP_KIND op, uint64_t& oper,
                     uint64_t& exec, double& load, double& dram) const;

  void Print_mem_stats(const CKKS_COUNTER& ckks_sts, const ROT_COUNTER& rot_sts,
                       std::ostream& os, uint32_t indent) const;
  void Print_ckks_stats(const CKKS_COUNTER& sts, std::ostream& os,
                        uint32_t indent) const;
  void Print_call_stats(const CALL_COUNTER& sts, std::ostream& os,
                        uint32_t indent) const;
  void Print_rot_stats(const ROT_COUNTER& sts, std::ostream& os,
                       uint32_t indent) const;

  const air::base::GLOB_SCOPE& _glob;          // global scope
  const core::LOWER_CTX&       _ctx;           // lower context
  PER_FUNC_COUNTER             _stats;         // stats
  FUNC_COUNTER*                _func_stat;     // pointer to current func stat
  OP_COUNTER*                  _op_stat;       // pointer to current per-op stat
  bool                         _per_op_stats;  // true for per-op stats
};

}  // namespace ckks

}  // namespace fhe

#endif  // FHE_CKKS_CKKS_STATS_H
