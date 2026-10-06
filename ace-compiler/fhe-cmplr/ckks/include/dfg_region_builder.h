//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_DFG_REGION_BUILDER
#define FHE_CKKS_DFG_REGION_BUILDER

#include <vector>

#include "air/base/st.h"
#include "air/driver/driver_ctx.h"
#include "air/opt/scc_builder.h"
#include "air/opt/scc_node.h"
#include "air/util/error.h"
#include "dfg_region_container.h"
#include "fhe/core/lower_ctx.h"

namespace fhe {
namespace ckks {

//! @brief Implementation of the REGION builder. A REGION is a subgraph of the
//! DFG. Each region groups elements with the same mul_depth.
class REGION_BUILDER {
public:
  using GLOB_SCOPE         = air::base::GLOB_SCOPE;
  using FUNC_SCOPE         = air::base::FUNC_SCOPE;
  using FUNC_ID            = air::base::FUNC_ID;
  using DFG_NODE_ID        = air::opt::DFG_NODE_ID;
  using DFG_NODE_PTR       = air::opt::DFG_NODE_PTR;
  using CONST_DFG_NODE_PTR = const DFG_NODE_PTR&;
  using SCC_NODE_ID        = air::opt::SCC_NODE_ID;
  using SCC_NODE_PTR       = air::opt::SCC_NODE_PTR;
  using CONST_SCC_NODE_PTR = const SCC_NODE_PTR&;
  using SCC_CONTAINER      = air::opt::SCC_CONTAINER;
  using DFG_CONTAINER      = air::opt::DFG_CONTAINER;
  using DFG                = DFG_CONTAINER::DFG;
  using SCC_GRAPH          = SCC_CONTAINER::SCC_GRAPH;
  using SCC_BUILDER        = air::opt::SCC_BUILDER<REGION_CONTAINER::GRAPH>;
  using CALL_INFO_MAP      = std::map<CALLSITE_INFO, std::vector<uint32_t>>;

  REGION_BUILDER(REGION_CONTAINER*              region_cntr,
                 const air::driver::DRIVER_CTX* driver_ctx)
      : _region_cntr(region_cntr), _driver_ctx(driver_ctx) {}

  R_CODE Perform(void);

private:
  // REQUIRED UNDEFINED UNWANTED methods
  REGION_BUILDER(void);
  REGION_BUILDER(const REGION_BUILDER&);
  REGION_BUILDER& operator=(const REGION_BUILDER&);

  //! @brief Build DFG of each function.
  void Build_dfg();

  //! @brief Link the parameters of a call node with the corresponding formal
  //! parameters of the called function. Also, link the return node of the
  //! called function with the return register of the call node.
  void Link_formal(const CALLSITE_INFO& callsite);

  //! @brief Handle the callsite by creating a REGION_ELEM for each cipher-type
  //! data node of the called function within the context of the callsite.
  void Handle_callsite(const CALLSITE_INFO& callsite);

  //! @brief Current impl of RESBM is based on the assumption that the scale and
  //! level of each node is statically determined. This method checks if any
  //! CKKS::mul occurs in a circle of data flow graph, which violates previous
  //! assumption.
  R_CODE Validate_scc_node();

  //! @brief Build SCC for the graph of REGION_ELEM.
  R_CODE Build_scc();

  //! @brief Compute mul_depth of each node. The mul_depth of a node is set to
  //! the count of multiplications from the program entry.
  void Cal_mul_depth();

  //! @brief To ensure each region starts with MulCP/MulCC, maintain the
  //! mul_depth of nodes consistent with their source MulCP/MulCC nodes within
  //! each region.
  void Unify_mul_depth(const std::list<air::opt::SCC_NODE_ID>& scc_node_list);
  //! @brief Increase the mul_depth of each node to the lowest value among its
  //! successors. If the successor is a multiplication node, set the mul_depth
  //! to one less than that of the successor.
  void Opt_mul_depth();

  //! @brief Create regions. REGION_ELEMs with the same mul_depth are grouped
  //! into the same region.
  void Create_region();

  //! @brief Verify 1. each region starts with MulCP/MulCC;
  //! 2. each MulCP/MulCC locates at the beginning of a region.
  void Verify_region();

  //! @brief Return consumed mul_depth of SCC node.
  uint32_t Consume_mul_lev(CONST_SCC_NODE_PTR scc_node);
  //! @brief Check if SCC node is retv
  bool Is_retv(CONST_SCC_NODE_PTR scc_node);
  //! @brief Check if SCC node contains ciphertext value
  bool Is_cipher(CONST_REGION_ELEM_PTR elem) const;
  bool Is_cipher(CONST_SCC_NODE_PTR scc_node) const;
  bool Is_cipher(CONST_DFG_NODE_PTR dfg_node) const;

  void Init_scc_mul_depth(void) {
    _scc_mul_depth.clear();
    _scc_mul_depth.resize(Scc_cntr()->Node_cnt(), INVALID_MUL_DEPTH);
  }
  uint32_t Mul_depth(SCC_NODE_ID scc_node) {
    AIR_ASSERT(scc_node.Value() < _scc_mul_depth.size());
    return _scc_mul_depth[scc_node.Value()];
  }
  void Set_mul_depth(SCC_NODE_ID scc_node, uint32_t mul_depth) {
    _scc_mul_depth[scc_node.Value()] = mul_depth;
    _max_mul_depth                   = std::max(_max_mul_depth, mul_depth);
  }

  uint32_t Max_mul_depth(void) const { return _max_mul_depth; }

  REGION_CONTAINER*    Region_cntr(void) const { return _region_cntr; }
  SCC_CONTAINER*       Scc_cntr() { return _region_cntr->Scc_cntr(); }
  const DFG_CONTAINER* Dfg_cntr(FUNC_ID func) const {
    return _region_cntr->Dfg_cntr(func);
  }

  const GLOB_SCOPE* Glob_scope(void) const {
    return _region_cntr->Glob_scope();
  }
  const core::LOWER_CTX* Lower_ctx(void) const {
    return _region_cntr->Lower_ctx();
  }
  const air::driver::DRIVER_CTX* Driver_ctx(void) const { return _driver_ctx; }

  REGION_CONTAINER*              _region_cntr;
  const air::driver::DRIVER_CTX* _driver_ctx;
  std::vector<uint32_t>          _scc_mul_depth;
  uint32_t                       _max_mul_depth = 0;
};

}  // namespace ckks
}  // namespace fhe
#endif  // FHE_CKKS_DFG_REGION_BUILDER