//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_MIN_CUT_REGION_H
#define FHE_CKKS_MIN_CUT_REGION_H

#include "air/driver/driver_ctx.h"
#include "air/opt/dfg_container.h"
#include "air/opt/scc_container.h"
#include "air/opt/scc_node.h"
#include "dfg_region.h"
#include "dfg_region_container.h"
#include "fhe/ckks/config.h"
#include "fhe/core/lower_ctx.h"
namespace fhe {
namespace ckks {

//! @brief Kind of min cut.
enum MIN_CUT_KIND : uint8_t {
  MIN_CUT_BTS     = 0,  // min cut for bootstrapping
  MIN_CUT_RESCALE = 1,  // min cut for rescaling
  MIN_CUT_END     = 2,
};

//! @brief CUT_TYPE contains REGION_ELEMs for inserting rescaling/bootstrapping
//! operations and the cost increase of the current cut. Provides APIs to access
//! and update REGION_ELEMs and cost increase.
class CUT_TYPE {
public:
  using REGION_ELEM_ID_SET = std::set<REGION_ELEM_ID>;
  using ELEM_ITER          = REGION_ELEM_ID_SET::iterator;
  CUT_TYPE(const REGION_CONTAINER* cntr, const std::set<REGION_ELEM_ID>& cut,
           double val)
      : _region_cntr(cntr), _cut_elem(cut), _cost_incr(val) {}
  explicit CUT_TYPE(const REGION_CONTAINER* cntr) : _region_cntr(cntr) {}
  CUT_TYPE(const CUT_TYPE& o)
      : _region_cntr(o.Region_cntr()),
        _cut_elem(o.Cut_elem()),
        _cost_incr(o.Cost_incr()) {}
  CUT_TYPE& operator=(const CUT_TYPE& o) {
    _region_cntr = o.Region_cntr();
    _cut_elem    = o.Cut_elem();
    _cost_incr   = o.Cost_incr();
    return *this;
  }
  ~CUT_TYPE() {}

  const std::set<REGION_ELEM_ID>& Cut_elem(void) const { return _cut_elem; }
  std::set<REGION_ELEM_ID>&       Cut_elem(void) { return _cut_elem; }
  void      Set_cost_incr(double val) { _cost_incr = val; }
  double    Cost_incr(void) const { return _cost_incr; }
  void      Add_elem(REGION_ELEM_ID id) { _cut_elem.insert(id); }
  void      Erase_elem(REGION_ELEM_ID id) { _cut_elem.erase(id); }
  bool      Empty(void) const { return _cut_elem.empty(); }
  ELEM_ITER Find(REGION_ELEM_ID id) const { return _cut_elem.find(id); }
  ELEM_ITER End(void) const { return _cut_elem.end(); }
  uint32_t  Size(void) const { return _cut_elem.size(); }
  void      Print(std::ostream& os, uint32_t indent = 4);
  void      Print(void) { Print(std::cout, 4); }

private:
  // REQUIRED UNDEFINED UNWANTED methods
  CUT_TYPE(void);

  const REGION_CONTAINER*  Region_cntr(void) const { return _region_cntr; }
  const REGION_CONTAINER*  _region_cntr = nullptr;
  std::set<REGION_ELEM_ID> _cut_elem;
  double                   _cost_incr = 0.;
};

//! @brief Perform min cut for a region to select inserting locations of
//! rescaling/bootstrapping. Take rescaling/bootstrapping as cut operation,
//! value of a cut is set as the sum of the cost of cut operations inserted
//! at the cut and the cost increase of operations ahead of the cut.
//! Current implementation is based on article:
//! Stoer M, Frank W. A simple min-cut algorithm. JACM, 1997, 44(4): 585-591.
//! The original algorithm is as follows:
//!   // G: a graph; V: all vertex; w: the weight of each edge; a: src node.
//!   Min_cut(G, w, a)
//!     while |V| > 1
//!       Min_cut_phase(G, w, a)
//!       if the cut-of-the-phase is lighter than the current minimum cut
//!         then store the cut-of-the-phase as the current minimum cut
//!   Min_cut_phase(G, w, a)
//!     A = {a}
//!     while A != V
//!       add to A the most tightly connected vertex
//!     store the cut-of-the-phase and shrink G by merging the two vertices
//!     added last
//!
//! In context of min_cut for rescaling/bootstrapping, the nodes originating
//! from upper regions are taken as the initial source node. In selecting the
//! most tightly connected vertex, the node resulting in the highest cost
//! increase is selected.
class MIN_CUT_REGION {
public:
  using DRIVER_CTX    = air::driver::DRIVER_CTX;
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

  MIN_CUT_REGION(const CKKS_CONFIG* cfg, const DRIVER_CTX* driver_ctx,
                 const REGION_CONTAINER* reg_cntr, REGION_ID region,
                 uint32_t level, MIN_CUT_KIND kind)
      : _config(cfg),
        _driver_ctx(driver_ctx),
        _reg_cntr(reg_cntr),
        _min_cut(reg_cntr),
        _region(region),
        _level(level),
        _kind(kind) {}
  ~MIN_CUT_REGION() {}

  void            Perform();
  const CUT_TYPE& Min_cut(void) const { return _min_cut; }
  CUT_TYPE&       Min_cut(void) { return _min_cut; }

  DECLARE_CKKS_OPTION_CONFIG_ACCESS_API((*_config))
  DECLARE_TRACE_DETAIL_API((*_config), _driver_ctx)

private:
  // REQUIRED UNDEFINED UNWANTED methods
  MIN_CUT_REGION(void);
  MIN_CUT_REGION(const MIN_CUT_REGION&);
  MIN_CUT_REGION& operator=(const MIN_CUT_REGION&);

  //! @brief Return cost increase if cut operation is inserted after the node.
  double Node_op_cost(const DFG_NODE_PTR& node) const;
  //! @brief Return cost of cut operations inserted after scc_node.
  double Scc_cut_op_cost(const SCC_NODE_PTR& scc_node) const;
  //! @brief Return total cost increase of operations in SCC_NODE if cut
  //! operation is inserted after the node.
  double Scc_node_op_cost(const SCC_NODE_PTR& scc_node) const;
  //! @brief Set the weight of each edge within or originating from scc_node.
  void Set_weight(const SCC_NODE_PTR& scc_node, double val);
  //! @brief Return the node originating from current cut with the highest cost
  //! increase.
  SCC_NODE_ID Next_src_node(const CUT_TYPE& cur_cut);
  //! @brief Return cost of added FHE operation for a cut.
  double Cut_op_cost(void) const;
  //! @brief Return total frequency of the edges to insert cut operation.
  uint32_t Cut_edge_freq(void) const;
  //! @brief Return the cost of the current cut, which is the sum of the
  //! cost of all bootstrap/rescale operations required for the current cut.
  double Cut_cost(const CUT_TYPE& cut);
  //! @brief Merge node, and update cur_cut.
  void Update_cut(const SCC_NODE_PTR& node, CUT_TYPE& cur_cut);
  //! @brief Collect all the nodes originating from upper region to use as
  //! source nodes for the min-cut.
  double Init_src_node(void);
  void   Min_cut_phase(void);

  const SCC_NODE_SET&     Src(void) const { return _src_node; }
  SCC_NODE_SET&           Src(void) { return _src_node; }
  uint32_t                Level(void) const { return _level; }
  MIN_CUT_KIND            Kind(void) const { return _kind; }
  const REGION_CONTAINER* Reg_cntr(void) const { return _reg_cntr; }
  REGION_ID               Region_id(void) const { return _region; }
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

  const REGION_CONTAINER* _reg_cntr   = nullptr;
  const CKKS_CONFIG*      _config     = nullptr;
  const DRIVER_CTX*       _driver_ctx = nullptr;
  CUT_TYPE                _min_cut;
  SCC_NODE_SET            _src_node;
  REGION_ID               _region;
  uint32_t                _level;
  MIN_CUT_KIND            _kind;
};

}  // namespace ckks
}  // namespace fhe
#endif  // FHE_CKKS_MIN_CUT_REGION_H