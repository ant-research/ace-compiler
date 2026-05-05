//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================
#ifndef AIR_OPT_DFG_BUILD_CTX_H
#define AIR_OPT_DFG_BUILD_CTX_H

#include "air/base/analyze_ctx.h"
#include "air/opt/ssa_container.h"
#include "air/util/debug.h"
#include "dfg_container.h"
#include "dfg_node.h"

namespace air {
namespace opt {

//! @brief DFG_BUILD_CTX is the context used in building DFG.
//! _dfg points to the DFG to create. Current DFG is built with SSA form IR,
//! _ssa_cntr is container of the SSA form IR. To record frequency of each node,
//! _freq_stack is used to record frequency of handling blocks.
class DFG_BUILD_CTX : public air::base::ANALYZE_CTX {
public:
  using NODE_ID   = air::base::NODE_ID;
  using NODE_PTR  = air::base::NODE_PTR;
  using STMT_PTR  = air::base::STMT_PTR;
  using CONTAINER = air::base::CONTAINER;
  using DFG       = DFG_CONTAINER::DFG;

  static constexpr uint32_t INVALID_FREQ = 0;

  DFG_BUILD_CTX(DFG* dfg, const SSA_CONTAINER* ssa_cntr)
      : _dfg(dfg), _ssa_cntr(ssa_cntr) {
    AIR_ASSERT(dfg != nullptr);
    AIR_ASSERT(ssa_cntr != nullptr);
  }

  ~DFG_BUILD_CTX() {}

  //! @brief Return the existing DFG node of ver,
  //! or create and return a new DFG node for ver.
  DFG_NODE_PTR Get_dfg_node(CONST_SSA_VER_PTR ver, air::base::TYPE_ID type,
                            uint32_t freq = INVALID_FREQ) {
    uint32_t     opnd_cnt = (ver->Kind() == air::opt::VER_DEF_KIND::PHI)
                                ? Ssa_cntr()->Phi_node(ver->Def_phi_id())->Size()
                                : 1;
    DFG_NODE_PTR dfg_node =
        _dfg->Container().Get_node(ver->Id(), type, opnd_cnt);
    if (dfg_node->Freq() == INVALID_FREQ && freq != INVALID_FREQ) {
      dfg_node->Set_freq(freq);
    }
    AIR_ASSERT_MSG(freq == INVALID_FREQ || dfg_node->Freq() == freq,
                   "Mismatch node freqency.");
    return dfg_node;
  }

  //! @brief Return the existing DFG node of expr,
  //! or create and return a new DFG node for expr.
  DFG_NODE_PTR Get_dfg_node(base::CONST_NODE_PTR expr, air::base::TYPE_ID type,
                            uint32_t freq) {
    uint32_t     opnd_cnt = expr->Num_child();
    DFG_NODE_PTR dfg_node =
        _dfg->Container().Get_node(expr->Id(), type, opnd_cnt);
    if (dfg_node->Freq() == INVALID_FREQ && freq != INVALID_FREQ) {
      dfg_node->Set_freq(freq);
    }
    AIR_ASSERT_MSG(freq == INVALID_FREQ || dfg_node->Freq() == freq,
                   "Mismatch node freqency.");
    return dfg_node;
  }

  void     Push_freq(uint32_t iter_cnt) { _freq_stack.push_back(iter_cnt); }
  void     Pop_freq() { _freq_stack.pop_back(); }
  uint32_t Top_freq() const { return _freq_stack.back(); }
  //! @brief Return the cumulative frequency of nested loops.
  uint32_t Freq() const {
    uint32_t freq = 1;
    for (uint32_t val : _freq_stack) freq *= val;
    return freq;
  }
  bool Freq_empty() const { return _freq_stack.empty(); }

  const DFG& Dfg() const { return *_dfg; }
  DFG&       Dfg() { return *_dfg; }

  const SSA_CONTAINER* Ssa_cntr() const { return _ssa_cntr; }
  const CONTAINER*     Container(void) const { return _ssa_cntr->Container(); }

  //! @brief unknown DOMAIN handler
  template <typename RETV, typename VISITOR>
  RETV Handle_unknown_domain(VISITOR* visitor, NODE_PTR node) {
    uint32_t     freq     = Freq();
    DFG_NODE_PTR dfg_node = Get_dfg_node(node, node->Rtype_id(), freq);
    for (uint32_t id = 0; id < node->Num_child(); ++id) {
      DFG_NODE_PTR child_dfg_node =
          visitor->template Visit<RETV>(node->Child(id)).Node();
      dfg_node->Set_opnd(id, child_dfg_node);
    }
    return RETV(dfg_node);
  }

  //! @brief default NODE handler
  template <typename RETV, typename VISITOR>
  RETV Handle_node(VISITOR* visitor, NODE_PTR node) {
    uint32_t     freq     = Freq();
    DFG_NODE_PTR dfg_node = Get_dfg_node(node, node->Rtype_id(), freq);
    for (uint32_t id = 0; id < node->Num_child(); ++id) {
      DFG_NODE_PTR child_node =
          visitor->template Visit<RETV>(node->Child(id)).Node();
      dfg_node->Set_opnd(id, child_node->Id());
    }
    return RETV(dfg_node);
  }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  DFG_BUILD_CTX(void);
  DFG_BUILD_CTX(const DFG_BUILD_CTX&);
  DFG_BUILD_CTX operator=(const DFG_BUILD_CTX&);

  DFG*                  _dfg;
  const SSA_CONTAINER*  _ssa_cntr;
  std::vector<uint32_t> _freq_stack;
};

}  // namespace opt
}  // namespace air

#endif  // AIR_OPT_DFG_BUILD_CTX_H