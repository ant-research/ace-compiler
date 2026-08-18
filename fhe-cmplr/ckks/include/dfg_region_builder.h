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

#include "../opt/include/scc_builder.h"
#include "air/base/container_decl.h"
#include "air/base/id_wrapper.h"
#include "air/base/node.h"
#include "air/base/st_decl.h"
#include "air/base/st_enum.h"
#include "air/driver/driver_ctx.h"
#include "air/opt/dfg_data.h"
#include "air/opt/scc_container.h"
#include "air/opt/scc_node.h"
#include "air/util/debug.h"
#include "dfg_region.h"
#include "dfg_region_container.h"
#include "fhe/core/lower_ctx.h"

namespace fhe {
namespace ckks {

class REGION_BUILDER {
public:
  using GLOB_SCOPE         = air::base::GLOB_SCOPE;
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

  explicit REGION_BUILDER(REGION_CONTAINER*              region_cntr,
                          const air::driver::DRIVER_CTX* driver_ctx)
      : _region_cntr(region_cntr), _driver_ctx(driver_ctx) {}

  void Perform();

private:
  // REQUIRED UNDEFINED UNWANTED methods
  REGION_BUILDER(void);
  REGION_BUILDER(const REGION_BUILDER&);
  REGION_BUILDER& operator=(const REGION_BUILDER&);

  void Link_param(const CALLSITE_INFO& callsite);
  void Handle_callsite(const CALLSITE_INFO& callsite);
  void Build_dfg();
  void Build_scc();
  void Downward_merge();
  void Collect_region_node();

  //! @brief Check if SCC node contains DFG node of CKKS::mul.
  bool Has_mul(CONST_SCC_NODE_PTR scc_node);

  //! @brief Return consumed mul_depth of DFG node
  //! Return 1 if the DFG node contains CKKS::mul.
  uint32_t Consume_mul_depth(CONST_DFG_NODE_PTR dfg_node);
  //! @brief Return consumed mul_depth of SCC node.
  uint32_t Consume_mul_depth(CONST_SCC_NODE_PTR scc_node);

  //! @brief Check if SCC node is retv
  bool Is_retv(CONST_SCC_NODE_PTR scc_node);
  //! @brief Check if SCC node is call
  bool Is_call(CONST_SCC_NODE_PTR scc_node);
  //! @brief Check if SCC node contains ciphertext value
  bool Is_cipher(CONST_REGION_ELEM_PTR elem) const;
  bool Is_cipher(CONST_SCC_NODE_PTR scc_node) const;
  bool Is_cipher(FUNC_ID func, CONST_DFG_NODE_PTR dfg_node) const;
  bool Is_cipher(FUNC_ID func, CONST_SCC_NODE_PTR scc_node) const;
  bool Is_cipher(FUNC_ID func, SCC_NODE_ID scc_node) const {
    return Is_cipher(func, Region_cntr()->Scc_cntr()->Node(scc_node));
  }

  //! @brief Current impl of RESBM is based on the assumption that scale and
  //! level of each node is statically determined. This method checks if any
  //! CKKS::mul occurs in a circle of data flow graph, which violates previous
  //! assumption.
  R_CODE Validate_scc_node(FUNC_ID func);

  //! @brief
  void Downward_merge(FUNC_ID func);

  //!
  void Handle_call(const air::opt::DFG_NODE_PTR& call, FUNC_ID caller);

  //! @brief Return mul_depth of
  uint32_t Param_mul_depth(const CALLSITE_INFO& call_info, uint32_t param_id);
  uint32_t Mul_depth(SCC_NODE_ID scc_node) {
    AIR_ASSERT(scc_node.Value() < _scc_mul_depth.size());
    return _scc_mul_depth[scc_node.Value()];
  }
  void Set_mul_depth(SCC_NODE_ID scc_node, uint32_t mul_depth) {
    _scc_mul_depth[scc_node.Value()] = mul_depth;
    _max_mul_depth                   = std::max(_max_mul_depth, mul_depth);
  }
  uint32_t Max_mul_depth(void) const { return _max_mul_depth; }
  //! @brief Calculate mul_depth of each node. Region of DFG is created
  //! according to mul_depth of the DFG nodes. The mul_depth is recorded as the
  //! REGION ID.
  void Cal_mul_depth(FUNC_ID func, const CALLSITE_INFO& call_info);
  void Cal_mul_depth();

  // void Set_node_region(air::opt::SCC_NODE_ID id, REGION_ID region) {
  //   _region_cntr->Set_region(id, region);
  // }

  void Collect_dfg_node(FUNC_ID callee, FUNC_ID caller, DFG_NODE_ID call_site);

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
  // CALL_INFO_MAP                  _call_info;
  std::vector<uint32_t> _scc_mul_depth;
  uint32_t              _max_mul_depth = 0;
};

}  // namespace ckks
}  // namespace fhe
#endif  // FHE_CKKS_DFG_REGION_BUILDER