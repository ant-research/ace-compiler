//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_GALOIS_LIVENESS_H
#define FHE_CKKS_GALOIS_LIVENESS_H

#include <map>

#include "air/base/analyze_ctx.h"
#include "air/base/container.h"
#include "air/base/node.h"
#include "air/core/opcode.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/ckks/ir2c_ctx.h"
#include "nn/core/attr.h"

namespace fhe {
namespace ckks {

//! @brief Program-level galois-key liveness collector (RNUM-driven).
//!
//! Problem it solves: Handle_rotate's legacy USE_AND_FREE emits Free_galois_key
//! after EACH non-constant rotate, so a step that recurs (every frame, or the
//! same {-33..33} set across 38 rotate nodes) reloads from NVMe on each use.
//!
//! This pass walks a function body in forward order, finds every ckks
//! OPC_ROTATE node, reads its RNUM attribute (int[] = all static step values
//! this rotate dereferences after loop expansion — a contract invariant, see
//! sihe2ckks_impl.h:265 / vector_gen.cxx:194), and records per distinct step
//! integer the FIRST and LAST rotate NODE_ID that uses it. Handle_rotate then
//! emits one Load_galois_key(step) at the first user and one Free at the last;
//! all intermediate rotates hit the runtime resident early-return
//! (key_mgr.cu ensure_one_slot). resnet20's steps are provably all static
//! (RNUM), so no air def-use / IR resource-node modeling is needed here.
//!
//! Scoped per function body. resnet20/phantom inlines all rotates into
//! Main_graph (poly2c_pass.cxx disables ckks2poly for non-ANT), so for that
//! path there is exactly one function of interest.
class GALOIS_LIVENESS_PASS {
public:
  using Gk_liveness_map = fhe::ckks::IR2C_CTX::Gk_liveness_map;
  using Gk_node_id      = fhe::ckks::IR2C_CTX::Gk_node_id;

  explicit GALOIS_LIVENESS_PASS(Gk_liveness_map& out) : _map(out) {}

  //! @brief Collect first/last use of every distinct rotate step in `body`.
  void Perform(air::base::NODE_PTR body) {
    _order_counter = 0;
    Collect(body);
  }

private:
  //! Forward pre-order walk: visit children, and when a ckks ROTATE is hit,
  //! register every step value in its RNUM against the running first/last map.
  //! A monotonic _order_counter preserves source order so "first" = earliest
  //! occurrence and "last" = latest occurrence across the whole body (loops too,
  //! since a loop-body rotate node is visited once and its RNUM already holds
  //! the full expanded value set).
  void Collect(air::base::NODE_PTR node) {
    if (node == air::base::Null_ptr) return;

    if (node->Opcode() == fhe::ckks::OPC_ROTATE) {
      Register_rotate(node);
      // Still recurse into children (Child(0) cipher, Child(1) step) — they are
      // expression subtrees with no nested ROTATE, but walk for completeness.
    }
    for (uint32_t i = 0; i < node->Num_child(); ++i) {
      Collect(node->Child(i));
    }
  }

  void Register_rotate(air::base::NODE_PTR rot_node) {
    const char* key    = nn::core::ATTR::RNUM;
    uint32_t    cnt    = 0;
    const int*  rnum   = rot_node->Attr<int>(key, &cnt);
    if (rnum == nullptr || cnt == 0) {
      // RNUM existence is a contract invariant (AIR_ASSERT at lowering); if a
      // future path produces a RNUM-less rotate, skip it: Handle_rotate falls
      // back to per-call Load/Free-free rotate (the runtime Ensure_galois_key
      // inside Rotate still loads lazily). Do not crash the compile.
      return;
    }
    const Gk_node_id node_id    = rot_node->Id().Value();
    const uint32_t   this_order = _order_counter++;
    for (uint32_t i = 0; i < cnt; ++i) {
      const int step = rnum[i];
      if (step == 0) continue;  // step 0 is a no-op/conjugate sentinel; never load/free
      auto& span = _map[step];
      if (!span.seen || this_order < span.first_order) {
        span.first_node  = node_id;
        span.first_order = this_order;
      }
      if (!span.seen || this_order > span.last_order) {
        span.last_node  = node_id;
        span.last_order = this_order;
      }
      span.seen = true;
    }
  }

  Gk_liveness_map& _map;
  uint32_t         _order_counter = 0;
};

}  // namespace ckks
}  // namespace fhe

#endif  // FHE_CKKS_GALOIS_LIVENESS_H