//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/opt/dead_store_elim.h"

#include "air/base/meta_info.h"
#include "air/base/node.h"
#include "air/core/opcode.h"
#include "air/opt/ssa_st.h"

namespace air {
namespace opt {

int Run_dead_store_elim(air::base::FUNC_SCOPE*         fs,
                        const air::driver::DRIVER_CTX* ctx) {
  DEAD_STORE_ELIM dse(fs, ctx);
  return dse.Run();
}

int DEAD_STORE_ELIM::Run() {
  SSA_CONTAINER ssa(_cntr);
  SSA_BUILDER   builder(_fs, &ssa, _ctx);
  builder.Perform();

  // Phase 1: Build use-counts for all SSA versions
  VER_USE_MAP use_map;
  Build_use_counts(ssa, use_map);
  Trace(TD_DSE, "DSE Phase1: ", ssa.Num_ver(), " versions tracked\n");

  // Phase 2: Reverse-traversal removal with inline cascading
  int num_eliminated = Remove_dead_stores(ssa, use_map);
  Trace(TD_DSE, "DSE summary: ", num_eliminated, " stores eliminated\n");
  return num_eliminated;
}

// ============================================================
// Phase 1: Build use-counts
// ============================================================

//! Increment both real_use and any_use — for direct LD reads and MU operands
static void Increment_real(DEAD_STORE_ELIM::VER_USE_MAP& use_map,
                           SSA_VER_ID                    id) {
  if (id == air::base::Null_id) return;
  auto& cnt = use_map[id.Value()];
  cnt._real_use++;
  cnt._any_use++;
}

//! Increment only any_use — for transitive CHI/PHI operands
static void Increment_any(DEAD_STORE_ELIM::VER_USE_MAP& use_map,
                          SSA_VER_ID                    id) {
  if (id == air::base::Null_id) return;
  use_map[id.Value()]._any_use++;
}

void DEAD_STORE_ELIM::Build_use_counts(SSA_CONTAINER& ssa,
                                       VER_USE_MAP&   use_map) {
  // Initialize all version use-counts to zero
  for (uint32_t i = 0; i < ssa.Num_ver(); ++i) {
    use_map[i] = {0, 0};
  }
  Scan_stmt_list(_cntr->Stmt_list(), ssa, use_map);
}

void DEAD_STORE_ELIM::Scan_stmt_list(air::base::STMT_LIST sl,
                                     SSA_CONTAINER& ssa, VER_USE_MAP& use_map) {
  for (air::base::STMT_PTR s = sl.Begin_stmt(); s != sl.End_stmt();
       s                     = s->Next()) {
    air::base::NODE_PTR node = s->Node();
    if (node == air::base::Null_ptr) continue;

    // Walk expression tree for direct LD uses
    Scan_node(node, ssa, use_map);

    // CHI operands on ST, CALL, ENTRY (MayDef inputs) — transitive only
    if (SSA_CONTAINER::Has_chi(node)) {
      CHI_NODE_ID chi_id = ssa.Node_chi(node->Id());
      while (chi_id != air::base::Null_id) {
        CHI_NODE_PTR chi = ssa.Chi_node(chi_id);
        Increment_any(use_map, chi->Opnd_id());
        chi_id = chi->Next_id();
      }
    }

    // MU operands on LD, CALL, RET (MayUse) — real uses
    // Note: MU on LD nodes is handled in Scan_node when we encounter LD
    if (node->Is_call() || node->Is_ret()) {
      if (SSA_CONTAINER::Has_mu(node)) {
        MU_NODE_ID mu_id = ssa.Node_mu(node->Id());
        while (mu_id != air::base::Null_id) {
          MU_NODE_PTR mu = ssa.Mu_node(mu_id);
          Increment_real(use_map, mu->Opnd_id());
          mu_id = mu->Next_id();
        }
      }
    }

    // PHI operands on IF, DO_LOOP (merge inputs) — transitive,
    // except loop preheader operands which are real uses (Open64
    // Set_Required_PHI: preserves IV definitions for downstream IVR)
    if (SSA_CONTAINER::Has_phi(node)) {
      bool        is_loop = node->Is_do_loop();
      PHI_NODE_ID phi_id  = ssa.Node_phi(node->Id());
      while (phi_id != air::base::Null_id) {
        PHI_NODE_PTR phi = ssa.Phi_node(phi_id);
        for (uint32_t i = 0; i < phi->Size(); ++i) {
          if (is_loop && i == PREHEADER_PHI_OPND_ID) {
            // Preheader operand is a real use — preserves IV definitions
            Increment_real(use_map, phi->Opnd_id(i));
          } else {
            Increment_any(use_map, phi->Opnd_id(i));
          }
        }
        phi_id = phi->Next_id();
      }
    }
  }
}

void DEAD_STORE_ELIM::Scan_node(air::base::NODE_PTR node, SSA_CONTAINER& ssa,
                                VER_USE_MAP& use_map) {
  if (node == air::base::Null_ptr) return;

  // For LD/LDP/LDF/LDO/LDPF nodes: the direct version is a real use
  if (node->Is_ld()) {
    SSA_VER_ID ver_id = ssa.Node_ver_id(node->Id());
    if (ver_id != air::base::Null_id) {
      Increment_real(use_map, ver_id);
    }
    // Also scan MU list for aliased may-uses on LD nodes — real uses
    if (SSA_CONTAINER::Has_mu(node)) {
      MU_NODE_ID mu_id = ssa.Node_mu(node->Id());
      while (mu_id != air::base::Null_id) {
        MU_NODE_PTR mu = ssa.Mu_node(mu_id);
        Increment_real(use_map, mu->Opnd_id());
        mu_id = mu->Next_id();
      }
    }
    return;  // LD is a leaf; no further recursion needed
  }

  // BLOCK nodes: iterate stmt list
  if (node->Is_block()) {
    Scan_stmt_list(air::base::STMT_LIST(node), ssa, use_map);
    return;
  }

  // Other nodes: recurse into children
  for (uint32_t i = 0; i < node->Num_child(); ++i) {
    Scan_node(node->Child(i), ssa, use_map);
  }
}

// ============================================================
// Phase 2: Reverse-traversal removal with inline cascading
// ============================================================

//! Decrement any_use only (for CHI/PHI operand cascading)
static void Decrement_and_enqueue(DEAD_STORE_ELIM::VER_USE_MAP& use_map,
                                  std::vector<uint32_t>&        worklist,
                                  SSA_VER_ID                    ver_id) {
  if (ver_id == air::base::Null_id) return;
  uint32_t id = ver_id.Value();
  auto     it = use_map.find(id);
  if (it == use_map.end()) return;
  if (it->second._any_use > 0) {
    it->second._any_use--;
    if (it->second._any_use == 0) {
      worklist.push_back(id);
    }
  }
}

//! Decrement both real_use and any_use (for LD/MU cascading)
static void Decrement_real_and_enqueue(DEAD_STORE_ELIM::VER_USE_MAP& use_map,
                                       std::vector<uint32_t>&        worklist,
                                       SSA_VER_ID                    ver_id) {
  if (ver_id == air::base::Null_id) return;
  uint32_t id = ver_id.Value();
  auto     it = use_map.find(id);
  if (it == use_map.end()) return;
  if (it->second._real_use > 0) {
    it->second._real_use--;
  }
  if (it->second._any_use > 0) {
    it->second._any_use--;
    if (it->second._any_use == 0) {
      worklist.push_back(id);
    }
  }
}

void DEAD_STORE_ELIM::Decrement_rhs_uses(air::base::STMT_PTR    stmt,
                                         SSA_CONTAINER&         ssa,
                                         VER_USE_MAP&           use_map,
                                         std::vector<uint32_t>& worklist) {
  air::base::NODE_PTR node = stmt->Node();
  if (node == air::base::Null_ptr) return;

  // For store statements, child(0) is the RHS expression tree
  if (node->Is_st() && node->Num_child() > 0) {
    Decrement_node_uses(node->Child(0), ssa, use_map, worklist);
  }
}

void DEAD_STORE_ELIM::Decrement_node_uses(air::base::NODE_PTR    node,
                                          SSA_CONTAINER&         ssa,
                                          VER_USE_MAP&           use_map,
                                          std::vector<uint32_t>& worklist) {
  if (node == air::base::Null_ptr) return;

  // For LD nodes: decrement both real_use and any_use (LD is a real use)
  if (node->Is_ld()) {
    SSA_VER_ID ver_id = ssa.Node_ver_id(node->Id());
    if (ver_id != air::base::Null_id) {
      Decrement_real_and_enqueue(use_map, worklist, ver_id);
    }
    // Also decrement MU operands on LD (MU is a real use)
    if (SSA_CONTAINER::Has_mu(node)) {
      MU_NODE_ID mu_id = ssa.Node_mu(node->Id());
      while (mu_id != air::base::Null_id) {
        MU_NODE_PTR mu = ssa.Mu_node(mu_id);
        Decrement_real_and_enqueue(use_map, worklist, mu->Opnd_id());
        mu_id = mu->Next_id();
      }
    }
    return;
  }

  // Recurse into children
  for (uint32_t i = 0; i < node->Num_child(); ++i) {
    Decrement_node_uses(node->Child(i), ssa, use_map, worklist);
  }
}

// ============================================================
// Phase 3: Remove dead stores
// ============================================================

bool DEAD_STORE_ELIM::Has_side_effect(air::base::NODE_PTR node) const {
  if (node == air::base::Null_ptr) return false;
  if (air::base::META_INFO::Has_prop<air::base::OPR_PROP::CALL>(
          node->Opcode())) {
    return true;
  }
  for (uint32_t i = 0; i < node->Num_child(); ++i) {
    if (Has_side_effect(node->Child(i))) return true;
  }
  return false;
}

bool DEAD_STORE_ELIM::Is_store_target_removable(
    air::base::NODE_PTR node) const {
  if (node == air::base::Null_ptr) return false;
  if (!node->Is_st()) return false;

  // Never eliminate stores on FUNC_ENTRY
  if (node->Is_entry()) return false;

  // IST (indirect store) — conservatively skip
  if (node->Opcode() == air::core::OPC_IST) return false;

  // For PREG stores (STP/STPF): always safe (PREGs cannot escape)
  if (node->Is_preg_op()) {
    return !Has_side_effect(node->Child(0));
  }

  // For direct stores (ST/STF/STO): target ADDR_DATUM must be safe
  if (node->Has_sym()) {
    air::base::ADDR_DATUM_PTR addr = node->Addr_datum();
    if (addr->Defining_func_scope() == nullptr) return false;
    if (addr->Is_addr_saved() || addr->Is_addr_passed()) return false;
    if (addr->Is_static_fld()) return false;
    // Formal parameters are visible to callers (Open64 Required_stid)
    if (addr->Is_formal()) return false;
  }

  // RHS must be side-effect-free
  if (node->Num_child() > 0 && Has_side_effect(node->Child(0))) {
    return false;
  }

  return true;
}

bool DEAD_STORE_ELIM::Is_safe_to_eliminate(air::base::STMT_PTR stmt,
                                           SSA_CONTAINER&      ssa,
                                           const VER_USE_MAP&  use_map) const {
  air::base::NODE_PTR node = stmt->Node();

  // Target-based safety checks (addr_passed, IST, side-effects, etc.)
  if (!Is_store_target_removable(node)) return false;

  // Preserve identity assignments (st x = ld x) — they maintain
  // non-zero versions at merge points (Open64 Required_stid)
  if (Is_identity_assignment(node)) return false;

  // Check 1: Direct MustDef version must have any_use == 0
  SSA_VER_ID ver_id = ssa.Node_ver_id(node->Id());
  if (ver_id == air::base::Null_id) return false;
  {
    auto it = use_map.find(ver_id.Value());
    if (it == use_map.end() || it->second._any_use != 0) return false;
  }

  // Check 2: ALL CHI result versions must have any_use == 0
  // Check 3: No zero versions on CHI results (paper's rule)
  if (SSA_CONTAINER::Has_chi(node)) {
    CHI_NODE_ID chi_id = ssa.Node_chi(node->Id());
    while (chi_id != air::base::Null_id) {
      CHI_NODE_PTR chi    = ssa.Chi_node(chi_id);
      SSA_VER_ID   res_id = chi->Result_id();
      if (res_id != air::base::Null_id) {
        SSA_VER_PTR res_ver = ssa.Ver(res_id);
        if (res_ver->Version() == 0) return false;

        auto it = use_map.find(res_id.Value());
        if (it == use_map.end() || it->second._any_use != 0) return false;
      }
      chi_id = chi->Next_id();
    }
  }

  return true;
}

void DEAD_STORE_ELIM::Cascade_chi_phi(air::base::NODE_PTR node,
                                      SSA_CONTAINER& ssa, VER_USE_MAP& use_map,
                                      std::vector<uint32_t>& worklist) {
  // Mark all CHI nodes on this statement dead and decrement their operands
  if (SSA_CONTAINER::Has_chi(node)) {
    CHI_NODE_ID chi_id = ssa.Node_chi(node->Id());
    while (chi_id != air::base::Null_id) {
      CHI_NODE_PTR chi = ssa.Chi_node(chi_id);
      chi->Set_dead();
      Decrement_and_enqueue(use_map, worklist, chi->Opnd_id());
      chi_id = chi->Next_id();
    }
  }
}

int DEAD_STORE_ELIM::Remove_dead_stores(SSA_CONTAINER& ssa,
                                        VER_USE_MAP&   use_map) {
  std::vector<uint32_t> worklist;
  int changed = Reverse_remove_dead(_cntr->Stmt_list(), ssa, use_map, worklist);

  // Drain worklist for cross-block PHI/CHI cascading.
  // Reverse traversal handles same-block STMT cascading implicitly;
  // this worklist only processes PHI/CHI-defined versions whose any_use
  // dropped to 0 during removal (much smaller than seeding all versions).
  while (!worklist.empty()) {
    uint32_t vid = worklist.back();
    worklist.pop_back();

    auto it = use_map.find(vid);
    if (it != use_map.end() && it->second._any_use != 0) continue;

    SSA_VER_PTR ver = ssa.Ver(SSA_VER_ID(vid));
    if (ver->Kind() == VER_DEF_KIND::UNKNOWN) continue;
    if (ver->Version() == 0) continue;

    switch (ver->Kind()) {
      case VER_DEF_KIND::CHI: {
        CHI_NODE_PTR chi = ssa.Chi_node(ver->Def_chi_id());
        chi->Set_dead();
        Trace(TD_DSE, "DSE Phase2: mark CHI dead, ver_", vid, "\n");
        Decrement_and_enqueue(use_map, worklist, chi->Opnd_id());
        break;
      }
      case VER_DEF_KIND::PHI: {
        PHI_NODE_PTR phi = ssa.Phi_node(ver->Def_phi_id());
        phi->Set_dead();
        Trace(TD_DSE, "DSE Phase2: mark PHI dead, ver_", vid, "\n");
        for (uint32_t i = 0; i < phi->Size(); ++i) {
          Decrement_and_enqueue(use_map, worklist, phi->Opnd_id(i));
        }
        break;
      }
      case VER_DEF_KIND::STMT: {
        // Cross-block cascading made a STMT-defined version dead.
        // Guard: skip if already removed during reverse traversal.
        air::base::STMT_PTR stmt = ssa.Stmt(ver->Def_stmt_id());
        if (!stmt->Has_parent_node()) break;
        air::base::NODE_PTR node = stmt->Node();
        if (node != air::base::Null_ptr && it->second._real_use == 0 &&
            Is_store_target_removable(node)) {
          Trace(TD_DSE, "DSE Phase2: cascade remove stmt_",
                ver->Def_stmt_id().Value(), "\n");
          Decrement_rhs_uses(stmt, ssa, use_map, worklist);
          Cascade_chi_phi(node, ssa, use_map, worklist);
          air::base::STMT_LIST encl =
              air::base::STMT_LIST::Enclosing_list(stmt);
          encl.Remove(stmt);
          changed++;
        }
        break;
      }
      default:
        AIR_ASSERT_MSG(false, "DSE: unexpected VER_DEF_KIND in worklist");
        break;
    }
  }

  return changed;
}

int DEAD_STORE_ELIM::Reverse_remove_dead(air::base::STMT_LIST   sl,
                                         SSA_CONTAINER&         ssa,
                                         VER_USE_MAP&           use_map,
                                         std::vector<uint32_t>& worklist) {
  if (sl.Is_empty()) return 0;

  int                 changed = 0;
  air::base::STMT_PTR stmt    = sl.Last_stmt();
  while (stmt != air::base::Null_ptr) {
    air::base::STMT_PTR prev = stmt->Prev();  // cache before potential removal
    air::base::NODE_PTR node = stmt->Node();

    if (node != air::base::Null_ptr) {
      // Recurse into nested blocks (depth-first: inner blocks first so
      // their dead-store removals update use-counts before we inspect
      // versions in the enclosing scope)
      for (uint32_t i = 0; i < node->Num_child(); ++i) {
        air::base::NODE_PTR child = node->Child(i);
        if (child != air::base::Null_ptr && child->Is_block()) {
          changed += Reverse_remove_dead(air::base::STMT_LIST(child), ssa,
                                         use_map, worklist);
        }
      }

      if (node->Is_st() && Is_safe_to_eliminate(stmt, ssa, use_map)) {
        Trace(TD_DSE, "DSE Phase2: remove stmt_", stmt->Id().Value(), "\n");
        // Inline cascading: decrement RHS uses so earlier stmts (visited
        // later in reverse) see updated counts
        Decrement_rhs_uses(stmt, ssa, use_map, worklist);
        Cascade_chi_phi(node, ssa, use_map, worklist);
        sl.Remove(stmt);
        changed++;
      }
    }

    stmt = prev;
  }
  return changed;
}

bool DEAD_STORE_ELIM::Is_identity_assignment(air::base::NODE_PTR node) const {
  if (node == air::base::Null_ptr) return false;
  if (node->Num_child() == 0) return false;
  air::base::NODE_PTR rhs = node->Child(0);
  if (rhs == air::base::Null_ptr) return false;

  // ST x = LD x (same ADDR_DATUM)
  if (node->Has_sym() && rhs->Is_ld() && rhs->Has_sym()) {
    return node->Addr_datum_id() == rhs->Addr_datum_id();
  }
  // STP p = LDP p (same PREG)
  if (node->Is_preg_op() && rhs->Is_ld() && rhs->Is_preg_op()) {
    return node->Preg_id() == rhs->Preg_id();
  }
  return false;
}

}  // namespace opt
}  // namespace air
