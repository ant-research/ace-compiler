//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_BASE_OP_STATS_H
#define AIR_BASE_OP_STATS_H

#include <unordered_map>
#include <vector>

#include "air/base/analyze_ctx.h"
#include "air/base/st.h"
#include "air/core/opcode.h"

namespace air {
namespace base {

//! @brief counter for each operator
//  contains both static counter (code size) and dynamic counter (performance)
class OP_STATS {
public:
  //! @brief construct OP_STATS with all counters set to 0
  OP_STATS(const GLOB_SCOPE& glob) : _glob(glob) {}

  //! @brief handle node to increase counter
  //  static counter increased by 1. dynamic counter increased by freq
  void Handle_node(NODE_PTR node, uint32_t freq, uint32_t est_freq) {
    uint32_t domain = node->Domain();
    uint32_t oper   = node->Operator();
    if (domain >= _dom_stats.size()) {
      _dom_stats.resize(domain + 1);
    }
    OP_COUNTER& op_stats = _dom_stats[domain];
    if (oper >= op_stats.size()) {
      op_stats.resize(oper + 1);
    }
    op_stats[oper]._stc_cnt += 1;
    op_stats[oper]._dyn_cnt += freq;
    op_stats[oper]._est_dyn_cnt += est_freq;
  }

  //! @brief increase counter for CALL
  //  static counter increased by 1. dynamic counter increased by freq
  void Handle_call(NODE_PTR node, uint32_t freq, uint32_t est_freq) {
    AIR_ASSERT(node->Opcode() == air::core::OPC_CALL);
    COUNTER& call_cnt = _call_stats[node->Entry()->Name_id().Value()];
    call_cnt._stc_cnt += 1;
    call_cnt._dyn_cnt += freq;
    call_cnt._est_dyn_cnt += est_freq;
  }

  void        Print(std::ostream& os, uint32_t indent = 0) const;
  void        Print() const;
  std::string To_str() const;

private:
  // disable copy constructor and assign operator
  OP_STATS(const OP_STATS&)            = delete;
  OP_STATS& operator=(const OP_STATS&) = delete;

  struct COUNTER {
    uint32_t _stc_cnt;
    uint32_t _dyn_cnt;
    uint32_t _est_dyn_cnt;
    COUNTER() : _stc_cnt(0), _dyn_cnt(0), _est_dyn_cnt(0) {}
  };

  typedef std::vector<COUNTER>                  OP_COUNTER;
  typedef std::vector<OP_COUNTER>               DOMAIN_OP_COUNTER;
  typedef std::unordered_map<uint32_t, COUNTER> CALL_COUNTER;
  DOMAIN_OP_COUNTER                             _dom_stats;
  CALL_COUNTER                                  _call_stats;
  const GLOB_SCOPE&                             _glob;

};  // OP_STATS

//! @brief context to analyze IR to collect OP stats
//  traverse IR, maintain freq and increase OP counters
template <typename OP_STATS>
class OP_STATS_CTX : public ANALYZE_CTX {
public:
  //! @brief construct OP_STATS_CTX
  OP_STATS_CTX(OP_STATS& stats) : _stats(stats), _freq(1), _est_freq(1) {}

  template <typename RETV, typename VISITOR>
  RETV Handle_node(VISITOR* visitor, NODE_PTR node) {
    _stats.Handle_node(node, _freq, _est_freq);
    if (node->Opcode() == air::core::OPC_BLOCK) {
      return ANALYZE_CTX::Handle_block<RETV, VISITOR>(visitor, node);
    } else if (node->Opcode() == air::core::OPC_CALL) {
      return Handle_call<RETV, VISITOR>(visitor, node);
    } else if (node->Opcode() == air::core::OPC_DO_LOOP) {
      return Handle_do_loop<RETV, VISITOR>(visitor, node);
    } else {
      for (uint32_t i = 0; i < node->Num_child(); ++i) {
        visitor->template Visit<RETV>(node->Child(i));
      }
    }
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_call(VISITOR* visitor, NODE_PTR node) {
    _stats.Handle_call(node, _freq, _est_freq);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_do_loop(VISITOR* visitor, NODE_PTR node) {
    // handle init
    visitor->template Visit<RETV>(node->Child(0));
    // update freq
    uint32_t orig_freq     = _freq;
    uint32_t orig_est_freq = _est_freq;
    AIR_ASSERT(node->Child(0)->Opcode() == air::core::OPC_INTCONST);
    AIR_ASSERT(node->Child(1)->Opcode() == air::core::OPC_LT);
    AIR_ASSERT(node->Child(1)->Child(0)->Opcode() == air::core::OPC_LD);
    AIR_ASSERT(node->Child(1)->Child(0)->Addr_datum_id() == node->Iv_id());
    AIR_ASSERT(node->Child(2)->Opcode() == air::core::OPC_ADD);
    AIR_ASSERT(node->Child(2)->Child(0)->Opcode() == air::core::OPC_LD);
    AIR_ASSERT(node->Child(2)->Child(0)->Addr_datum_id() == node->Iv_id());
    AIR_ASSERT(node->Child(2)->Child(1)->Opcode() == air::core::OPC_INTCONST);
    AIR_ASSERT(node->Child(2)->Child(1)->Intconst() == 1);
    if (node->Child(1)->Child(1)->Opcode() == air::core::OPC_INTCONST) {
      uint32_t trip_cnt = node->Child(1)->Child(1)->Intconst() - node->Child(0)->Intconst();
      _freq *= trip_cnt;
      _est_freq *= trip_cnt;
    } else {
      _est_freq *= 10;
    }

    // handle compare, body and incr
    visitor->template Visit<RETV>(node->Child(1));
    visitor->template Visit<RETV>(node->Child(3));
    visitor->template Visit<RETV>(node->Child(2));
    // restore freq
    _freq     = orig_freq;
    _est_freq = orig_est_freq;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_unknown_domain(VISITOR* visitor, NODE_PTR node) {
    return Handle_node<RETV, VISITOR>(visitor, node);
  }

private:
  OP_STATS& _stats;
  uint32_t  _freq;
  uint32_t  _est_freq;
};  // OP_STATS_CTX

}  // namespace base

}  // namespace air

#endif  // AIR_BASE_OP_STATS_H
