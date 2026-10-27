//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_IR2C_HANDLER_H
#define FHE_CKKS_IR2C_HANDLER_H

#include <cstdint>
#include <string>

#include "air/base/container_decl.h"
#include "air/base/st_decl.h"
#include "air/base/st_type.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/ckks/invalid_handler.h"
#include "fhe/ckks/ir2c_ctx.h"
#include "nn/core/attr.h"
#include "nn/vector/vector_opcode.h"

namespace fhe {
namespace ckks {

class IR2C_HANDLER : public INVALID_HANDLER {
public:
  template <typename RETV, typename VISITOR>
  void Handle_add(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX&           ctx    = visitor->Context();
    air::base::NODE_PTR parent = ctx.Parent(1);

    AIR_ASSERT(parent != air::base::Null_ptr && parent->Is_st());
    if (ctx.Is_plain_type(node->Child(1)->Rtype_id())) {
      ctx << "Add_plain(&";
    } else if (ctx.Is_cipher_type(node->Child(1)->Rtype_id())) {
      ctx << "Add_ciph(&";
    } else {
      AIR_ASSERT(node->Child(1)->Rtype()->Is_prim());
      ctx << "Add_scalar(&";
    }
    ctx.Emit_st_var(parent);
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(1));
    ctx << ")";
  }

  template <typename RETV, typename VISITOR>
  void Handle_sub(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX&           ctx    = visitor->Context();
    air::base::NODE_PTR parent = ctx.Parent(1);

    AIR_ASSERT(parent != air::base::Null_ptr && parent->Is_st());
    if (ctx.Is_plain_type(node->Child(1)->Rtype_id())) {
      ctx << "Sub_plain(&";
    } else if (ctx.Is_cipher_type(node->Child(1)->Rtype_id()) ||
               ctx.Is_cipher3_type(node->Child(1)->Rtype_id())) {
      ctx << "Sub_ciph(&";
    } else if (node->Child(1)->Rtype()->Is_prim()) {
      ctx << "Sub_scalar(&";
    } else {
      // For other types (e.g., cipher3), treat as cipher sub
      ctx << "Sub_ciph(&";
    }
    ctx.Emit_st_var(parent);
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(1));
    ctx << ")";
  }

  template <typename RETV, typename VISITOR>
  void Handle_mul(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX&           ctx    = visitor->Context();
    air::base::NODE_PTR parent = ctx.Parent(1);

    AIR_ASSERT(parent != air::base::Null_ptr && parent->Is_st());
    if (ctx.Is_plain_type(node->Child(1)->Rtype_id())) {
      ctx << "Mul_plain(&";
    } else if (ctx.Is_cipher3_type(node->Child(1)->Rtype_id())) {
      ctx << "Mul_ciph(&";
    } else if (ctx.Is_cipher_type(node->Child(1)->Rtype_id())) {
      ctx << "Mul_ciph(&";
    } else {
      AIR_ASSERT(node->Child(1)->Rtype()->Is_prim());
      ctx << "Mul_scalar(&";
    }

    ctx.Emit_st_var(parent);
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(1));
    ctx << ")";
  }

  template <typename RETV, typename VISITOR>
  void Handle_rotate(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX&           ctx    = visitor->Context();
    air::base::NODE_PTR parent = ctx.Parent(1);

    AIR_ASSERT(parent != air::base::Null_ptr && parent->Is_st());
    // Stage-2/3: galois-key load/free around the rotate, driven by
    // -P2C:gkfm (Gk_free_mode):
    //   OFF        — neither load nor free is emitted (the runtime still lazily
    //                loads inside Rotate_ciph's Ensure_galois_key; slots stay
    //                resident for the whole session; release_session reclaims).
    //   USE_AND_FREE — legacy F1: emit Load_galois_key(step) before EVERY rotate
    //                + Free_galois_key(step) after each NON-constant rotate.
    //                Safe but reloads when the same step recurs.
    //   LAST_USE   — program-level liveness: each distinct static step (taken
    //                from the ROTATE node's RNUM attribute) gets ONE Load at
    //                its first user and ONE Free at its last user across the
    //                whole emitted function; all intermediate rotates hit the
    //                runtime resident early-return (key_mgr.cu ensure_one_slot).
    //                resnet20 steps are all statically known via RNUM, so no
    //                air def-use / IR resource-node modeling is required.
    // The runtime Load_galois_key/Free_galois_key are no-ops outside
    // KeyMode::ON_DEMAND(_FULL), so emitting them unconditionally is harmless.
    // Rotate_ciph's step arg is always Child(1) (runtime value) — unchanged.
    const fhe::poly::Gk_free_mode mode = ctx.Gk_free_mode();

    // Static step values this rotate dereferences (RNUM = expanded loop value
    // set; a contract invariant — see sihe2ckks_impl.h / vector_gen.cxx).
    uint32_t   rnum_cnt = 0;
    const int* rnum     = node->Attr<int>(nn::core::ATTR::RNUM, &rnum_cnt);
    const bool has_rnum = (rnum != nullptr && rnum_cnt > 0);
    const IR2C_CTX::Gk_node_id node_id = node->Id().Value();

    // --- Pre-rotate Loads ---
    if (mode == fhe::poly::Gk_free_mode::USE_AND_FREE) {
      // Legacy: emit a Load before every rotate (each rotate reloads).
      ctx << "Load_galois_key(";
      visitor->template Visit<RETV>(node->Child(1));
      ctx << ");\n  ";
    } else if (mode == fhe::poly::Gk_free_mode::LAST_USE && has_rnum) {
      // LAST_USE: emit a Load ONLY for those step values whose first use is
      // this node. Dedup within this node in case RNUM repeats a value.
      bool emitted_any = false;
      for (uint32_t i = 0; i < rnum_cnt; ++i) {
        const int step = rnum[i];
        if (step == 0) continue;
        // skip duplicates already emitted for this node
        bool dup = false;
        for (uint32_t j = 0; j < i; ++j) {
          if (rnum[j] == step) { dup = true; break; }
        }
        if (dup) continue;
        bool is_first = false, is_last = false;
        if (ctx.Gk_use_pos(node_id, step, is_first, is_last) && is_first) {
          if (!emitted_any) { ctx << ""; emitted_any = true; }
          ctx << "Load_galois_key(" << step << ");\n  ";
        }
      }
    }
    // OFF: no load. LAST_USE without RNUM: rely on Rotate_ciph's internal lazy
    // Ensure_galois_key (no explicit Load); correctness preserved, just not
    // hoisted — acceptable fallback for a hypothetical RNUM-less path.

    // --- The rotate itself (unchanged step arg = Child(1)) ---
    ctx << "Rotate_ciph(&";
    ctx.Emit_st_var(parent);
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(1));
    ctx << ")";

    // --- Post-rotate Frees ---
    if (mode == fhe::poly::Gk_free_mode::USE_AND_FREE &&
        node->Child(1)->Opcode() != air::core::OPC_INTCONST) {
      // Legacy F1: free this (non-constant) step right after each use.
      ctx << ";\n  Free_galois_key(";
      visitor->template Visit<RETV>(node->Child(1));
      ctx << ")";
    } else if (mode == fhe::poly::Gk_free_mode::LAST_USE && has_rnum) {
      // LAST_USE: emit a Free ONLY for those step values whose last use is
      // this node. Dedup within this node.
      for (uint32_t i = 0; i < rnum_cnt; ++i) {
        const int step = rnum[i];
        if (step == 0) continue;
        bool dup = false;
        for (uint32_t j = 0; j < i; ++j) {
          if (rnum[j] == step) { dup = true; break; }
        }
        if (dup) continue;
        bool is_first = false, is_last = false;
        if (ctx.Gk_use_pos(node_id, step, is_first, is_last) && is_last) {
          ctx << ";\n  Free_galois_key(" << step << ")";
        }
      }
    }
    // OFF: no free. LAST_USE-but-is_first (a one-shot step, first==last): the
    // Free branch above still fires because is_last is also true → load+free
    // at the same site, matching use-and-free semantics for never-recurring
    // steps (correct, minimal HBM).
  }

  template <typename RETV, typename VISITOR>
  void Handle_relin(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX&           ctx    = visitor->Context();
    air::base::NODE_PTR parent = ctx.Parent(1);

    AIR_ASSERT(parent != air::base::Null_ptr && parent->Is_st());
    ctx << "Relin(&";
    ctx.Emit_st_var(parent);
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ")";
  }

  template <typename RETV, typename VISITOR>
  void Handle_rescale(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX&           ctx    = visitor->Context();
    air::base::NODE_PTR parent = ctx.Parent(1);

    AIR_ASSERT(parent != air::base::Null_ptr && parent->Is_st());
    ctx << "Rescale_ciph(&";
    ctx.Emit_st_var(parent);
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ")";
  }

  template <typename RETV, typename VISITOR>
  void Handle_mod_switch(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX&           ctx    = visitor->Context();
    air::base::NODE_PTR parent = ctx.Parent(1);

    AIR_ASSERT(parent != air::base::Null_ptr && parent->Is_st());
    ctx << "Mod_switch(&";
    ctx.Emit_st_var(parent);
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ")";
  }

  template <typename RETV, typename VISITOR>
  void Handle_raise_mod(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX&           ctx    = visitor->Context();
    air::base::NODE_PTR parent = ctx.Parent(1);

    AIR_ASSERT(parent != air::base::Null_ptr && parent->Is_st());
    ctx << "Raise_mod(&";
    ctx.Emit_st_var(parent);
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(1));
    ctx << ")";
  }

  template <typename RETV, typename VISITOR>
  void Handle_conjugate(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX&           ctx    = visitor->Context();
    air::base::NODE_PTR parent = ctx.Parent(1);

    AIR_ASSERT(parent != air::base::Null_ptr && parent->Is_st());
    ctx << "Conjugate_ciph(&";
    ctx.Emit_st_var(parent);
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ")";
  }

  template <typename RETV, typename VISITOR>
  void Handle_mul_mono(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX&           ctx    = visitor->Context();
    air::base::NODE_PTR parent = ctx.Parent(1);

    AIR_ASSERT(parent != air::base::Null_ptr && parent->Is_st());
    ctx << "Mul_mono_ciph(&";
    ctx.Emit_st_var(parent);
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(1));
    ctx << ")";
  }

  template <typename RETV, typename VISITOR>
  void Handle_bootstrap(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX&           ctx    = visitor->Context();
    air::base::NODE_PTR parent = ctx.Parent(1);

    AIR_ASSERT(parent != air::base::Null_ptr && parent->Is_st());
    const uint32_t* mul_lev =
        node->Attr<uint32_t>(fhe::core::FHE_ATTR_KIND::LEVEL);
    const uint32_t* slot = node->Attr<uint32_t>(nn::core::ATTR::SLOT);
    if ((ctx.Provider() == core::PROVIDER::ANT) && ctx.Bts_explicit_slot()) {
      // ANT full-math path with explicit slot: emit Eval_bootstrap_ciph
      // which internally performs precom checks then calls Eval_bootstrap.
      ctx << "Eval_bootstrap_ciph(&";
      ctx.Emit_st_var(parent);
      ctx << ", ";
      visitor->template Visit<RETV>(node->Child(0));
      ctx << ", " << (mul_lev == nullptr ? 0 : *mul_lev);
      ctx << ", " << (slot == nullptr ? 0 : *slot) << ")";
    } else {
      // TODO Handle bts with target level.
      ctx << "Bootstrap(&";
      ctx.Emit_st_var(parent);
      ctx << ", ";
      visitor->template Visit<RETV>(node->Child(0));
      ctx << ", " << (mul_lev == nullptr ? 0 : *mul_lev);
      ctx << ", " << (slot == nullptr ? 0 : *slot) << ")";
    }
    if (!ctx._need_bts) {
      ctx._need_bts = true;
    }
  }

  template <typename RETV, typename VISITOR>
  void Handle_bootstrap_coeffs_to_slots(VISITOR*            visitor,
                                        air::base::NODE_PTR node) {
    IR2C_CTX&           ctx    = visitor->Context();
    air::base::NODE_PTR parent = ctx.Parent(1);

    AIR_ASSERT(parent != air::base::Null_ptr && parent->Is_st());
    const uint32_t* slot = node->Attr<uint32_t>(nn::core::ATTR::SLOT);
    if (ctx.Provider() == core::PROVIDER::ANT) {
      ctx << "Eval_bootstrap_coeffs_to_slots_ciph(&";
      ctx.Emit_st_var(parent);
      ctx << ", ";
      visitor->template Visit<RETV>(node->Child(0));
      ctx << ", " << (slot == nullptr ? 0 : *slot) << ")";
    } else {
      ctx << "Bootstrap(&";
      ctx.Emit_st_var(parent);
      ctx << ", ";
      visitor->template Visit<RETV>(node->Child(0));
      ctx << ", 0)";
    }
    if (!ctx._need_bts) {
      ctx._need_bts = true;
    }
  }

  template <typename RETV, typename VISITOR>
  void Handle_bootstrap_eval_mod(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX&           ctx    = visitor->Context();
    air::base::NODE_PTR parent = ctx.Parent(1);

    AIR_ASSERT(parent != air::base::Null_ptr && parent->Is_st());
    const uint32_t* slot = node->Attr<uint32_t>(nn::core::ATTR::SLOT);
    if (ctx.Provider() == core::PROVIDER::ANT) {
      ctx << "Eval_bootstrap_eval_mod_ciph(&";
      ctx.Emit_st_var(parent);
      ctx << ", ";
      visitor->template Visit<RETV>(node->Child(0));
      ctx << ", " << (slot == nullptr ? 0 : *slot) << ")";
    } else {
      ctx << "Bootstrap(&";
      ctx.Emit_st_var(parent);
      ctx << ", ";
      visitor->template Visit<RETV>(node->Child(0));
      ctx << ", 0)";
    }
    if (!ctx._need_bts) {
      ctx._need_bts = true;
    }
  }

  template <typename RETV, typename VISITOR>
  void Handle_bootstrap_slots_to_coeffs(VISITOR*            visitor,
                                        air::base::NODE_PTR node) {
    IR2C_CTX&           ctx    = visitor->Context();
    air::base::NODE_PTR parent = ctx.Parent(1);

    AIR_ASSERT(parent != air::base::Null_ptr && parent->Is_st());
    const uint32_t* slot = node->Attr<uint32_t>(nn::core::ATTR::SLOT);
    if (ctx.Provider() == core::PROVIDER::ANT) {
      ctx << "Eval_bootstrap_slots_to_coeffs_ciph(&";
      ctx.Emit_st_var(parent);
      ctx << ", ";
      visitor->template Visit<RETV>(node->Child(0));
      ctx << ", " << (slot == nullptr ? 0 : *slot) << ")";
    } else {
      ctx << "Bootstrap(&";
      ctx.Emit_st_var(parent);
      ctx << ", ";
      visitor->template Visit<RETV>(node->Child(0));
      ctx << ", 0)";
    }
    if (!ctx._need_bts) {
      ctx._need_bts = true;
    }
  }

  template <typename RETV, typename VISITOR>
  void Handle_free(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX& ctx      = visitor->Context();
    uint64_t  elem_cnt = 0;
    if (node->Child(0)->Opcode() == air::core::OPC_LD &&
        node->Child(0)->Addr_datum()->Type()->Is_array()) {
      elem_cnt =
          node->Child(0)->Addr_datum()->Type()->Cast_to_arr()->Elem_count();
    }
    if (ctx.Is_cipher_type(node->Child(0)->Rtype_id()) ||
        ctx.Is_cipher3_type(node->Child(0)->Rtype_id())) {
      ctx << "Free_cipher(";
    } else if (ctx.Is_plain_type(node->Child(0)->Rtype_id())) {
      ctx << "Free_plain(";
    } else {
      ctx << "Free_ciph_poly(";
    }
    visitor->template Visit<RETV>(node->Child(0));
    if (elem_cnt > 0) {
      ctx << ", " << elem_cnt;
    }
    ctx << ")";
  }

  template <typename RETV, typename VISITOR>
  void Handle_encode(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX&           ctx    = visitor->Context();
    air::base::NODE_PTR parent = ctx.Parent(1);

    AIR_ASSERT(parent != air::base::Null_ptr && parent->Is_st());
    if (ctx.Emit_data_file()) {
      ctx.template Emit_encode<RETV, VISITOR>(visitor, parent, node);
    } else {
      ctx.template Emit_runtime_encode<RETV, VISITOR>(visitor, parent, node);
      for (int i = 1; i < node->Num_child(); ++i) {
        ctx << ", ";
        visitor->template Visit<RETV>(node->Child(i));
      }
      ctx << ")";
    }
  }

  template <typename RETV, typename VISITOR>
  void Handle_level(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX&           ctx    = visitor->Context();
    air::base::NODE_PTR parent = ctx.Parent(1);

    if (parent->Is_st()) {
      ctx.Emit_st_var(parent);
      ctx << " = Level(";
    } else {
      ctx << "Level(";
    }
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ")";
  }

  template <typename RETV, typename VISITOR>
  void Handle_scale(VISITOR* visitor, air::base::NODE_PTR node) {
    IR2C_CTX&           ctx    = visitor->Context();
    air::base::NODE_PTR parent = ctx.Parent(1);

    if (parent->Is_st()) {
      ctx.Emit_st_var(parent);
      ctx << " = Sc_degree(";
    } else {
      ctx << "Sc_degree(";
    }
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ")";
  }
};
}  // namespace ckks
}  // namespace fhe

#endif
