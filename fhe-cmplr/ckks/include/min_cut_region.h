//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_MIN_CUT_REGION_H
#define FHE_CKKS_MIN_CUT_REGION_H
#include <list>

#include "air/base/st.h"
#include "air/base/st_decl.h"
#include "air/opt/dfg_container.h"
#include "air/opt/dfg_data.h"
#include "air/opt/scc_container.h"
#include "air/opt/scc_node.h"
#include "dfg_region.h"
#include "dfg_region_container.h"
#include "fhe/core/lower_ctx.h"
namespace fhe {
namespace ckks {

enum MIN_CUT_KIND : uint8_t {
  MIN_CUT_START   = 0,
  MIN_CUT_BTS     = 1,
  MIN_CUT_RESCALE = 2,
  MIN_CUT_END     = 3,
};

class MIN_CUT {
public:
  using FUNC_ID       = air::base::FUNC_ID;
  using GLOB_SCOPE    = air::base::GLOB_SCOPE;
  using TYPE_ID       = air::base::TYPE_ID;
  using TYPE_PTR      = air::base::TYPE_PTR;
  using DFG_NODE_ID   = air::opt::DFG_NODE_ID;
  using DFG_NODE_PTR  = air::opt::DFG_NODE_PTR;
  using SCC_NODE_ID   = air::opt::SCC_NODE_ID;
  using SCC_NODE_PTR  = air::opt::SCC_NODE_PTR;
  using DFG_CONTAINER = air::opt::DFG_CONTAINER;
  using SCC_CONTAINER = air::opt::SCC_CONTAINER;
  using SCC_NODE_SET  = std::set<SCC_NODE_ID>;
  using CUT_TYPE      = std::set<REGION_ELEM_ID>;

  class NODE_INFO {
  public:
    NODE_INFO(SCC_NODE_ID node, double weight) : _node(node), _weight(weight) {}
    ~NODE_INFO() {}
    SCC_NODE_ID Node(void) { return _node; }
    double      Weight(void) { return _weight; }

  private:
    // REQUIRED UNDEFINED UNWANTED methods
    NODE_INFO(void);
    NODE_INFO(const NODE_INFO&);
    NODE_INFO operator=(const NODE_INFO&);

    SCC_NODE_ID _node;
    double      _weight;
  };

  MIN_CUT(const REGION_CONTAINER* reg_cntr, REGION_ID region, uint32_t level,
          MIN_CUT_KIND kind)
      : _reg_cntr(reg_cntr), _region(region), _level(level), _kind(kind) {}

  ~MIN_CUT() {}

  void            Perform();
  const CUT_TYPE& Min_cut(void) const { return _min_cut; }
  CUT_TYPE&       Min_cut(void) { return _min_cut; }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  MIN_CUT(void);
  MIN_CUT(const MIN_CUT&);
  MIN_CUT& operator=(const MIN_CUT&);

  double    Node_op_cost(const DFG_NODE_PTR& node) const;
  double    Scc_cut_op_cost(const SCC_NODE_PTR& scc_node) const;
  double    Scc_node_op_cost(const SCC_NODE_PTR& scc_node) const;
  NODE_INFO Next_src_node(const CUT_TYPE& cur_cut) const;
  NODE_INFO Next_sink_node(void) const;
  //! @brief Return cost of added FHE operation for a cut.
  double   Cut_op_cost(void) const;
  uint32_t Cut_edge_freq(void) const;
  //! @brief Return the cost of the current cut, which is equal to the sum of
  //! cost of all bootstrap/rescale operations required for the current cut.
  double Cut_value(const CUT_TYPE& cut);
  void   Update_cut(const SCC_NODE_PTR& node, CUT_TYPE& cut);
  double Init_src_node(void);
  void   Min_cut_phase(void);

  const SCC_NODE_SET&     Src(void) const { return _src_node; }
  SCC_NODE_SET&           Src(void) { return _src_node; }
  const SCC_NODE_SET&     Snk(void) const { return _snk_node; }
  SCC_NODE_SET&           Snk(void) { return _snk_node; }
  double                  Total_cost(void) const { return _tot_cost; }
  uint32_t                Level(void) const { return _level; }
  MIN_CUT_KIND            Kind(void) const { return _kind; }
  const REGION_CONTAINER* Reg_cntr(void) const { return _reg_cntr; }
  REGION_PTR Region(void) const { return _reg_cntr->Region(_region); }
  const REGION_CONTAINER* Region_cntr(void) const { return _reg_cntr; }
  const DFG_CONTAINER*    Dfg_cntr(FUNC_ID func) const {
    return _reg_cntr->Dfg_cntr(func);
  }
  const SCC_CONTAINER*   Scc_cntr() const { return _reg_cntr->Scc_cntr(); }
  const core::LOWER_CTX* Lower_ctx(void) const {
    return _reg_cntr->Lower_ctx();
  }
  const GLOB_SCOPE* Glob_scope(void) const { return _reg_cntr->Glob_scope(); }

  const REGION_CONTAINER* _reg_cntr;
  double                  _tot_cost;
  CUT_TYPE                _min_cut;
  SCC_NODE_SET            _src_node;
  SCC_NODE_SET            _snk_node;
  REGION_ID               _region;
  uint32_t                _level;
  MIN_CUT_KIND            _kind;
};

}  // namespace ckks
}  // namespace fhe
#endif  // FHE_CKKS_MIN_CUT_REGION_H