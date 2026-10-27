//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_OPT_DEAD_STORE_ELIM_H
#define AIR_OPT_DEAD_STORE_ELIM_H

#include <unordered_map>
#include <vector>

#include "air/base/container.h"
#include "air/base/st.h"
#include "air/driver/driver_ctx.h"
#include "air/opt/ssa_build.h"
#include "air/opt/ssa_container.h"

namespace air {
namespace opt {

//! @brief Trace detail flags for DSE pass
enum DSE_TRACE_DETAIL : uint64_t {
  TD_DSE = 1ULL << 0,  //!< General DSE trace
};

//! @brief Config for DSE pass
class DSE_CONFIG {
public:
  DSE_CONFIG(uint64_t td) : _trace_detail(td) {}
  bool Is_trace(uint64_t flag) const { return (_trace_detail & flag) != 0; }

private:
  DSE_CONFIG(void);
  DSE_CONFIG(const DSE_CONFIG&);
  DSE_CONFIG operator=(const DSE_CONFIG&);

  uint64_t _trace_detail = 0;
};

/**
 * @brief SSA-based Dead Store Elimination (Chow et al. 1996).
 *
 * Uses HSSA use-count analysis to identify and remove stores whose defined
 * versions have no uses. Handles cascading: removing a dead store decrements
 * its RHS uses, potentially making more stores dead.
 *
 * Architecture & Data-Flow Diagram
 * ================================
 *
 *  EXTERNAL (read-only inputs)        INTERNAL (owned by DSE)
 *  ~~~~~~~~~~~~~~~~~~~~~~~~~~~        ~~~~~~~~~~~~~~~~~~~~~~~
 *  FUNC_SCOPE -> CONTAINER            VER_USE_MAP {id -> use_count}
 *  DRIVER_CTX                         VER_USE_COUNT {_real_use, _any_use}
 *  SSA_CONTAINER + SSA_BUILDER
 *  NODE, STMT, STMT_LIST
 *  CHI_NODE, MU_NODE, PHI_NODE
 *  SSA_VER, META_INFO, OPC_IST
 *
 *  +--- Run() -------------------------------------------------+
 *  | SSA_BUILDER::Perform()  [constructs SSA, one-time]        |
 *  |                                                           |
 *  | Phase 1: BUILD USE-COUNTS         Reads     Writes        |
 *  |  Build_use_counts()               IR,SSA -> USE_MAP       |
 *  |  +- Scan_stmt_list()    -------------------------+        |
 *  |  +- Scan_node()         traverses NODE tree      |        |
 *  |  +- Increment_real/any  (static helpers)    -----+        |
 *  |                              |                            |
 *  | Phase 2: REVERSE REMOVE       v         Reads    Writes   |
 *  |  Remove_dead_stores()       USE_MAP --> USE_MAP            |
 *  |  +- Reverse_remove_dead()   IR,SSA  --> STMT_LIST          |
 *  |  |   (reverse stmt traversal,            ::Remove()       |
 *  |  |    inline cascading via               CHI.Set_dead()   |
 *  |  |    Decrement_rhs/node_uses)           PHI.Set_dead()   |
 *  |  +- Cascade_chi_phi()       (cross-block worklist)        |
 *  |  +- Is_safe_to_eliminate()                                |
 *  |  +- Is_store_target_removable()                           |
 *  |  +- Is_identity_assignment()                              |
 *  |  +- Has_side_effect()                                     |
 *  +-----------------------------------------------------------+
 *
 *  MUTATION BOUNDARIES:
 *    - SSA mutated ONLY via CHI_NODE::Set_dead(), PHI_NODE::Set_dead()
 *    - IR  mutated ONLY via STMT_LIST::Remove()  (Phase 2 only)
 *    - NODE, CONTAINER: never mutated by DSE
 *    - VER_USE_MAP: internal, never exposed outside DSE
 */
class DEAD_STORE_ELIM {
public:
  DEAD_STORE_ELIM(air::base::FUNC_SCOPE* fs, const air::driver::DRIVER_CTX* ctx)
      : _fs(fs), _ctx(ctx), _cntr(&fs->Container()), _config(0) {}

  //! Build SSA and apply dead store elimination to the entire function.
  //! Returns the number of stores eliminated.
  int Run();

  //! Per-version use counts: real_use for direct LD/MU reads,
  //! any_use for all uses including transitive PHI/CHI operands.
  //! Distinction per Open64 Real_use/Any_use (opt_dse.cxx).
  struct VER_USE_COUNT {
    uint32_t _real_use = 0;  //!< Direct LD reads + MU operands
    uint32_t _any_use  = 0;  //!< All uses (LD, MU, CHI opnd, PHI opnd)
  };
  using VER_USE_MAP = std::unordered_map<uint32_t, VER_USE_COUNT>;

private:
  DECLARE_TRACE_DETAIL_API(_config, _ctx)

  //! Phase 1: Build use-counts for all SSA versions
  void Build_use_counts(SSA_CONTAINER& ssa, VER_USE_MAP& use_map);
  void Scan_stmt_list(air::base::STMT_LIST sl, SSA_CONTAINER& ssa,
                      VER_USE_MAP& use_map);
  void Scan_node(air::base::NODE_PTR node, SSA_CONTAINER& ssa,
                 VER_USE_MAP& use_map);

  //! Phase 2: Reverse-traversal removal with inline cascading
  //! Returns the number of stores eliminated.
  int  Remove_dead_stores(SSA_CONTAINER& ssa, VER_USE_MAP& use_map);
  int  Reverse_remove_dead(air::base::STMT_LIST sl, SSA_CONTAINER& ssa,
                           VER_USE_MAP&           use_map,
                           std::vector<uint32_t>& worklist);
  void Cascade_chi_phi(air::base::NODE_PTR node, SSA_CONTAINER& ssa,
                       VER_USE_MAP& use_map, std::vector<uint32_t>& worklist);
  void Decrement_rhs_uses(air::base::STMT_PTR stmt, SSA_CONTAINER& ssa,
                          VER_USE_MAP&           use_map,
                          std::vector<uint32_t>& worklist);
  void Decrement_node_uses(air::base::NODE_PTR node, SSA_CONTAINER& ssa,
                           VER_USE_MAP&           use_map,
                           std::vector<uint32_t>& worklist);
  bool Is_safe_to_eliminate(air::base::STMT_PTR stmt, SSA_CONTAINER& ssa,
                            const VER_USE_MAP& use_map) const;
  bool Is_store_target_removable(air::base::NODE_PTR node) const;
  bool Is_identity_assignment(air::base::NODE_PTR node) const;
  bool Has_side_effect(air::base::NODE_PTR node) const;

  air::base::FUNC_SCOPE*         _fs;
  const air::driver::DRIVER_CTX* _ctx;
  air::base::CONTAINER*          _cntr;
  DSE_CONFIG                     _config;
};

//! Convenience entry point: construct, run, return number of stores eliminated.
int Run_dead_store_elim(air::base::FUNC_SCOPE*         fs,
                        const air::driver::DRIVER_CTX* ctx);

}  // namespace opt
}  // namespace air

#endif  // AIR_OPT_DEAD_STORE_ELIM_H
