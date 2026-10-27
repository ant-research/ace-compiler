//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef NN_VECTOR_ROLL_SIMP_H
#define NN_VECTOR_ROLL_SIMP_H

#include <vector>

#include "air/base/analyze_ctx.h"
#include "air/base/container.h"
#include "air/base/node.h"
#include "air/base/st.h"
#include "air/base/transform_ctx.h"
#include "air/core/default_handler.h"
#include "air/core/opcode.h"
#include "nn/core/attr.h"
#include "nn/core/default_handler.h"
#include "nn/vector/config.h"
#include "nn/vector/core_handler.h"
#include "nn/vector/default_handler.h"
#include "nn/vector/tensor2vector_ctx.h"
#include "nn/vector/vector_ctx.h"
#include "nn/vector/vector_gen.h"
#include "nn/vector/vector_opcode.h"
#include "nn/vector/vector_utils.h"

namespace nn {
namespace vector {

using namespace air::base;

//! @brief Context and Handler for Vector Roll Simplification Optimization
//!
//! TRACKING: The downstream SIHE/CKKS scale & level analysis is specialized
//! to the AST loop shapes and alias-free SSA topology produced by CNN
//! lowering; it is NOT a general CFG/MemorySSA dataflow engine. Any
//! loop-level transform in this pass must therefore preserve the SSA dataflow
//! isomorphism of the original IR (fresh SSA version per grid tap, no bare
//! copies aliasing a producer version into an array) until that analysis is
//! rebuilt as a proper SSA dataflow solver. Failure to do so drains the RNS
//! level budget on level-tight configs (runtime "Level of rescale opnd is too
//! small").
class ROLL_SIMP_HANDLER;

class ROLL_SIMP_CTX : public air::base::TRANSFORM_CTX {
public:
  ROLL_SIMP_CTX(air::base::CONTAINER* cont, VECTOR_CTX& ctx,
                const air::driver::DRIVER_CTX* driver_ctx,
                const VECTOR_CONFIG&           cfg)
      : air::base::TRANSFORM_CTX(cont),
        _ctx(ctx),
        _driver_ctx(driver_ctx),
        _config(cfg),
        _cur_func_scope(nullptr) {}

  DECLARE_VECTOR_CTX_ACCESS_API(_ctx)
  DECLARE_VECTOR_CONFIG_ACCESS_API(_config)
  DECLARE_TRACE_DETAIL_API(_config, _driver_ctx)

  const VECTOR_CONFIG& Config() const { return _config; }
  FUNC_SCOPE*          Cur_func_scope() const { return _cur_func_scope; }
  void Set_cur_func_scope(FUNC_SCOPE* fs) { _cur_func_scope = fs; }

  //! Statements produced by a statement-level transform (e.g. the split loops
  //! and zero-tap rolls replacing a Blocking_rot loop). When non-empty after a
  //! statement is visited, these replace the visited statement.
  std::vector<STMT_PTR>& Replacement_stmts() { return _replacement_stmts; }

  template <typename RETV, typename VISITOR>
  RETV Handle_block(VISITOR* visitor, air::base::NODE_PTR node) {
    NODE_PTR block = _cntr->New_stmt_block(node->Spos());
    Push(node, block);
    STMT_LIST list(block);
    for (STMT_PTR stmt = node->Begin_stmt(); stmt != node->End_stmt();
         stmt          = stmt->Next()) {
      NODE_PTR s_node = stmt->Node();
      Begin_stmt(s_node);
      _replacement_stmts.clear();
      RETV     retv   = visitor->template Visit<RETV>(s_node);
      NODE_PTR n_node = Get_node_ptr(retv);
      if (!_replacement_stmts.empty()) {
        for (auto st : _replacement_stmts) {
          list.Append(st);
        }
        _replacement_stmts.clear();
      } else if (n_node != Null_ptr) {
        AIR_ASSERT(n_node->Is_root());
        list.Append(n_node->Stmt());
      }
      End_stmt(s_node);
    }
    Pop(node, block);
    return RETV(block);
  }

private:
  VECTOR_CTX&                    _ctx;
  const air::driver::DRIVER_CTX* _driver_ctx;
  const VECTOR_CONFIG&           _config;
  FUNC_SCOPE*                    _cur_func_scope;
  std::vector<STMT_PTR>          _replacement_stmts;
};

class ROLL_SIMP_CORE_HANDLER : public air::core::DEFAULT_HANDLER {
private:
  template <typename RETV>
  NODE_PTR Node(RETV retv) {
    AIR_ASSERT(retv.Num_node() == 1);
    return retv.Node();
  }
  NODE_PTR Node(NODE_PTR node) { return node; }

public:
  template <typename RETV, typename VISITOR>
  RETV Handle_block(VISITOR* visitor, air::base::NODE_PTR node) {
    return visitor->Context().template Handle_block<RETV, VISITOR>(visitor, node);
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_ld(VISITOR* visitor, air::base::NODE_PTR node) {
    CONTAINER*     cntr = visitor->Context().Container();
    ADDR_DATUM_PTR data =
        cntr->Parent_func_scope()->Addr_datum(node->Addr_datum_id());
    NODE_PTR new_load = cntr->New_ld(data, node->Spos());
    new_load->Copy_attr(node);
    return RETV(new_load);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_st(VISITOR* visitor, air::base::NODE_PTR node) {
    CONTAINER* cntr = visitor->Context().Container();
    NODE_PTR   child = Node(visitor->template Visit<RETV>(node->Child(0)));
    ADDR_DATUM_PTR data =
        cntr->Parent_func_scope()->Addr_datum(node->Addr_datum_id());
    data->Set_type(child->Rtype());
    STMT_PTR new_store = cntr->New_st(child, data, node->Spos());
    new_store->Node()->Copy_attr(node);
    return RETV(new_store->Node());
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ldp(VISITOR* visitor, air::base::NODE_PTR node) {
    CONTAINER* cntr     = visitor->Context().Container();
    PREG_PTR   preg     = cntr->Parent_func_scope()->Preg(node->Preg_id());
    NODE_PTR   new_load = cntr->New_ldp(preg, node->Spos());
    new_load->Copy_attr(node);
    return RETV(new_load);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_stp(VISITOR* visitor, air::base::NODE_PTR node) {
    CONTAINER* cntr = visitor->Context().Container();
    NODE_PTR   child = Node(visitor->template Visit<RETV>(node->Child(0)));
    PREG_PTR   preg  = cntr->Parent_func_scope()->Preg(node->Preg_id());
    STMT_PTR   new_store = cntr->New_stp(child, preg, node->Spos());
    new_store->Node()->Copy_attr(node);
    return RETV(new_store->Node());
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ist(VISITOR* visitor, air::base::NODE_PTR node) {
    CONTAINER* cntr = visitor->Context().Container();
    NODE_PTR   base_idx = Node(visitor->template Visit<RETV>(node->Child(0)));
    NODE_PTR   val      = Node(visitor->template Visit<RETV>(node->Child(1)));
    STMT_PTR   new_ist  = cntr->New_ist(base_idx, val, node->Spos());
    new_ist->Node()->Copy_attr(node);
    return RETV(new_ist->Node());
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_lda(VISITOR* visitor, air::base::NODE_PTR node) {
    CONTAINER*     cntr = visitor->Context().Container();
    ADDR_DATUM_PTR data =
        cntr->Parent_func_scope()->Addr_datum(node->Addr_datum_id());
    return RETV(cntr->New_lda(data, POINTER_KIND::FLAT32, node->Spos()));
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ldca(VISITOR* visitor, air::base::NODE_PTR node) {
    CONTAINER* cntr = visitor->Context().Container();
    return RETV(cntr->New_ldca(node->Const(), POINTER_KIND::FLAT32, node->Spos()));
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_array(VISITOR* visitor, air::base::NODE_PTR node) {
    CONTAINER* cntr = visitor->Context().Container();
    NODE_PTR base = Node(visitor->template Visit<RETV>(node->Child(0)));
    NODE_PTR new_array = cntr->New_array(base, node->Array_dim(), node->Spos());
    for (uint32_t i = 0; i < node->Array_dim(); ++i) {
      NODE_PTR idx = Node(visitor->template Visit<RETV>(node->Array_idx(i)));
      cntr->Set_array_idx(new_array, i, idx);
    }
    return RETV(new_array);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ild(VISITOR* visitor, air::base::NODE_PTR node) {
    CONTAINER* cntr = visitor->Context().Container();
    NODE_PTR   addr = Node(visitor->template Visit<RETV>(node->Child(0)));
    return RETV(cntr->New_ild(addr, node->Spos()));
  }

  //! @brief Inspect Blocking_rot loops to eliminate zero rotations (identity
  //! taps) while preserving the loop structure that downstream scale/level
  //! analysis depends on.
  template <typename RETV, typename VISITOR>
  RETV Handle_do_loop(VISITOR* visitor, air::base::NODE_PTR node) {
    CONTAINER*     cntr   = visitor->Context().Container();
    GLOB_SCOPE*    gscope = cntr->Glob_scope();
    CONST_TYPE_PTR s32_type =
        gscope->Prim_type(air::base::PRIMITIVE_TYPE::INT_S32);

    // Inspect if loop is of the form:
    // for (iv = 0; iv < bs; iv++) {
    //   input_grid[iv] = roll(input_dup, ra[iv], {ra})
    // }
    NODE_PTR init_node = node->Child(0);
    NODE_PTR comp_node = node->Child(1);
    NODE_PTR incr_node = node->Child(2);
    NODE_PTR body_node = node->Child(3);

    bool is_standard_form =
        (init_node->Opcode() == air::core::OPC_INTCONST &&
         init_node->Intconst() == 0 &&
         comp_node->Opcode() == air::core::OPC_LT &&
         comp_node->Child(1)->Opcode() == air::core::OPC_INTCONST &&
         incr_node->Opcode() == air::core::OPC_ADD && body_node->Is_block());

    if (is_standard_form) {
      int64_t ub = comp_node->Child(1)->Intconst();
      // The loop body must contain exactly one statement:
      //   ist (array(lda(input_grid), iv),
      //        roll(ld(input_dup), ild(array(ldca(ra_const), iv)), {ra}))
      STMT_PTR only_stmt = body_node->Begin_stmt();
      if (only_stmt != body_node->End_stmt() &&
          only_stmt->Next() == body_node->End_stmt()) {
        NODE_PTR s_node = only_stmt->Node();
        if (s_node->Opcode() == air::core::OPC_IST &&
            s_node->Child(0)->Opcode() == air::core::OPC_ARRAY &&
            s_node->Child(1)->Opcode() ==
                OPCODE(nn::vector::VECTOR, nn::vector::VECTOR_OPCODE::ROLL)) {
          NODE_PTR array_lhs = s_node->Child(0);
          NODE_PTR roll_rhs  = s_node->Child(1);
          NODE_PTR base_lhs  = array_lhs->Child(0);

          if (base_lhs->Opcode() == air::core::OPC_LDA &&
              Has_attr<int>(roll_rhs, nn::core::ATTR::RNUM)) {
            uint32_t   rnum_cnt = 0;
            const int* rnums    = roll_rhs->Attr<int>(nn::core::ATTR::RNUM, &rnum_cnt);
            // The roll shift must be ra[iv] loaded from a constant array
            // (ild(array(ldca(ra_const), iv))) so the split loops can be
            // rebuilt with fresh induction variables.
            NODE_PTR shift_node = roll_rhs->Child(1);
            bool     shift_ok   =
                shift_node->Opcode() == air::core::OPC_ILD &&
                shift_node->Child(0)->Opcode() == air::core::OPC_ARRAY &&
                shift_node->Child(0)->Child(0)->Opcode() == air::core::OPC_LDCA;
            if (rnums != nullptr && rnum_cnt == (uint32_t)ub && ub <= 9 &&
                shift_ok) {
              CONSTANT_PTR ra_const =
                  shift_node->Child(0)->Child(0)->Const();
              // Count zero rotations (identity taps).
              uint32_t zero_cnt = 0;
              for (uint32_t i = 0; i < rnum_cnt; ++i) {
                if (rnums[i] == 0) ++zero_cnt;
              }
              // Only transform loops that actually contain a zero rotation.
              // Transforming zero-free loops would change the IR structure
              // (which downstream scale/level analysis depends on) for no
              // keyswitch benefit.
              if (zero_cnt > 0) {
                return Split_rot_loop<RETV, VISITOR>(visitor, node, s_node,
                                                     ra_const, rnums,
                                                     rnum_cnt, ub, zero_cnt);
              }
            }
          }
        }
      }
    }

    return visitor->Context().template Handle_node<RETV, VISITOR>(visitor, node);
  }

private:
  //! @brief Transform a matched Blocking_rot loop to eliminate zero
  //! rotations (identity taps pay a full keyswitch downstream otherwise).
  //!
  //! Rule 2 (all offsets zero, e.g. 1x1 convs with bs=1, ra={0}):
  //!   keep the loop (identical bounds) but rewrite the roll's shift to a
  //!   literal 0, so its keyswitch is eliminated downstream by
  //!   CKKS2HPOLY::Rotate_identity at the POLY stage:
  //!     for (iv in [0, bs))  input_grid[iv] = roll(input_dup, 0, {0})
  //! Rule 3 (mixed zeros, e.g. 3x3 convs with a center tap 0 in ra):
  //!   split the loop at the zero taps so that the surrounding loop
  //!   structure is preserved for downstream scale/level analysis:
  //!     for (iv in [0, z))      input_grid[iv] = roll(input_dup, ra[iv])
  //!     input_grid[z] = roll(input_dup, 0, {0})   (identity -> copy at POLY)
  //!     for (iv in (z, bs))     input_grid[iv] = roll(input_dup, ra[iv])
  template <typename RETV, typename VISITOR>
  RETV Split_rot_loop(VISITOR* visitor, air::base::NODE_PTR loop_node,
                      air::base::NODE_PTR ist_node, CONSTANT_PTR ra_const,
                      const int* rnums, uint32_t rnum_cnt, int64_t ub,
                      uint32_t zero_cnt) {
    CONTAINER*     cntr   = visitor->Context().Container();
    GLOB_SCOPE*    gscope = cntr->Glob_scope();
    CONST_TYPE_PTR s32_type =
        gscope->Prim_type(air::base::PRIMITIVE_TYPE::INT_S32);
    SPOS spos = loop_node->Spos();

    NODE_PTR array_lhs = ist_node->Child(0);
    NODE_PTR roll_rhs  = ist_node->Child(1);
    ADDR_DATUM_PTR input_grid_var =
        cntr->Parent_func_scope()->Addr_datum(
            array_lhs->Child(0)->Addr_datum_id());
    ADDR_DATUM_PTR iv_var =
        cntr->Parent_func_scope()->Addr_datum(loop_node->Iv_id());
    // Clone of the roll's input expression in the new container.
    NODE_PTR input_dup = Node(visitor->template Visit<RETV>(roll_rhs->Child(0)));

    // Emit `input_grid[k] = roll(input_dup, intconst 0, {0})` for an identity
    // (zero) rotation. A literal-zero roll is used instead of a bare copy
    // `input_grid[k] = input_dup` on purpose: the roll yields a fresh SSA
    // version (uniform with the rolled neighbors), so the loop-structured
    // scale/level analysis is undisturbed, whereas a bare copy aliases
    // input_dup's version into the array and drains one RNS level from the
    // level plan (observed as a runtime "Level of rescale opnd is too small"
    // crash on level-tight models). The keyswitch itself is eliminated later
    // by CKKS2HPOLY::Rotate_identity at the POLY stage, after all scale/level
    // accounting is done. This node is never re-visited by this pass, so
    // Rule 1's literal-zero fold does not fire on it.
    auto emit_zero_roll = [&](int64_t k) {
      NODE_PTR input_grid_k =
          cntr->New_array(cntr->New_lda(input_grid_var, POINTER_KIND::FLAT32,
                                        spos),
                          1, spos);
      cntr->Set_array_idx(input_grid_k, 0,
                          cntr->New_intconst(s32_type, k, spos));
      int        zero_shift = 0;
      NODE_PTR   roll_node  = cntr->New_cust_node(
          OPCODE(nn::vector::VECTOR, nn::vector::VECTOR_OPCODE::ROLL),
          input_dup->Rtype(), spos);
      roll_node->Set_child(0, cntr->Clone_node_tree(input_dup));
      roll_node->Set_child(1, cntr->New_intconst(s32_type, 0, spos));
      roll_node->Set_attr(nn::core::ATTR::RNUM, &zero_shift, 1);
      STMT_PTR store_stmt = cntr->New_ist(input_grid_k, roll_node, spos);
      visitor->Context().Replacement_stmts().push_back(store_stmt);
    };

    // Emit a loop over iv in [lo, hi) whose body is the original
    // `input_grid[iv] = roll(input_dup, ra[iv], {ra})` store.
    // Each split loop needs its own induction variable: the SIHE lowering
    // registers every loop IV exactly once (Get_or_gen_sihe_addr_datum with
    // the vector-domain type), so sharing one IV across two split loops
    // trips its type-compatibility check.
    uint32_t split_idx   = 0;
    auto emit_split_loop = [&](int64_t lo, int64_t hi) {
      ADDR_DATUM_PTR loop_iv = iv_var;
      if (split_idx++ > 0) {
        std::string iv_name = std::string(iv_var->Name()->Char_str()) +
                              "_rs" + std::to_string(split_idx);
        loop_iv =
            cntr->Parent_func_scope()->New_var(s32_type, iv_name.c_str(), spos);
      }
      NODE_PTR init = cntr->New_intconst(s32_type, lo, spos);
      NODE_PTR comp = cntr->New_bin_arith(
          air::core::OPC_LT, s32_type, cntr->New_ld(loop_iv, spos),
          cntr->New_intconst(s32_type, hi, spos), spos);
      NODE_PTR incr = cntr->New_bin_arith(
          air::core::OPC_ADD, s32_type, cntr->New_ld(loop_iv, spos),
          cntr->New_intconst(s32_type, 1, spos), spos);

      // input_grid[iv] = roll(input_dup, ra[iv], {ra})
      NODE_PTR grid_i =
          cntr->New_array(cntr->New_lda(input_grid_var, POINTER_KIND::FLAT32,
                                        spos),
                          1, spos);
      cntr->Set_array_idx(grid_i, 0, cntr->New_ld(loop_iv, spos));
      NODE_PTR ra_array = cntr->New_array(
          cntr->New_ldca(ra_const, POINTER_KIND::FLAT32, spos), 1, spos);
      cntr->Set_array_idx(ra_array, 0, cntr->New_ld(loop_iv, spos));
      NODE_PTR roll_node = cntr->New_cust_node(
          OPCODE(nn::vector::VECTOR, nn::vector::VECTOR_OPCODE::ROLL),
          input_dup->Rtype(), spos);
      roll_node->Set_child(0, cntr->Clone_node_tree(input_dup));
      roll_node->Set_child(1, cntr->New_ild(ra_array, spos));
      roll_node->Set_attr(nn::core::ATTR::RNUM, rnums, rnum_cnt);
      STMT_PTR ist = cntr->New_ist(grid_i, roll_node, spos);

      NODE_PTR body_blk = cntr->New_stmt_block(spos);
      STMT_LIST(body_blk).Append(ist);
      STMT_PTR split_loop =
          cntr->New_do_loop(loop_iv, init, comp, incr, body_blk, spos);
      visitor->Context().Replacement_stmts().push_back(split_loop);
    };

    if (zero_cnt == (uint32_t)ub) {
      // Rule 2: all offsets are zero. Keep the loop (identical bounds) and
      // rewrite the roll's shift to a literal 0: the loop structure and the
      // SSA version structure are untouched, and the identity rotation's
      // keyswitch is eliminated later by CKKS2HPOLY::Rotate_identity at the
      // POLY stage (after scale/level accounting). See emit_zero_roll.
      ADDR_DATUM_PTR loop_iv = iv_var;
      NODE_PTR init = cntr->New_intconst(s32_type, 0, spos);
      NODE_PTR comp = cntr->New_bin_arith(
          air::core::OPC_LT, s32_type, cntr->New_ld(loop_iv, spos),
          cntr->New_intconst(s32_type, ub, spos), spos);
      NODE_PTR incr = cntr->New_bin_arith(
          air::core::OPC_ADD, s32_type, cntr->New_ld(loop_iv, spos),
          cntr->New_intconst(s32_type, 1, spos), spos);

      NODE_PTR grid_i =
          cntr->New_array(cntr->New_lda(input_grid_var, POINTER_KIND::FLAT32,
                                        spos),
                          1, spos);
      cntr->Set_array_idx(grid_i, 0, cntr->New_ld(loop_iv, spos));
      int      zero_shift = 0;
      NODE_PTR roll_node  = cntr->New_cust_node(
          OPCODE(nn::vector::VECTOR, nn::vector::VECTOR_OPCODE::ROLL),
          input_dup->Rtype(), spos);
      roll_node->Set_child(0, cntr->Clone_node_tree(input_dup));
      roll_node->Set_child(1, cntr->New_intconst(s32_type, 0, spos));
      roll_node->Set_attr(nn::core::ATTR::RNUM, &zero_shift, 1);
      STMT_PTR zero_roll_ist = cntr->New_ist(grid_i, roll_node, spos);

      NODE_PTR body_blk = cntr->New_stmt_block(spos);
      STMT_LIST(body_blk).Append(zero_roll_ist);
      STMT_PTR copy_loop =
          cntr->New_do_loop(loop_iv, init, comp, incr, body_blk, spos);
      visitor->Context().Replacement_stmts().push_back(copy_loop);
    } else {
      // Rule 3: split at zero taps, keeping loops over non-zero runs.
      int64_t run_lo = -1;
      for (int64_t k = 0; k < ub; ++k) {
        if (rnums[k] == 0) {
          if (run_lo >= 0) {
            emit_split_loop(run_lo, k);
            run_lo = -1;
          }
          emit_zero_roll(k);
        } else if (run_lo < 0) {
          run_lo = k;
        }
      }
      if (run_lo >= 0) {
        emit_split_loop(run_lo, ub);
      }
    }
    return RETV(Null_ptr);
  }
};

class ROLL_SIMP_VECTOR_HANDLER : public nn::vector::DEFAULT_HANDLER {
private:
  template <typename RETV>
  NODE_PTR Node(RETV retv) {
    AIR_ASSERT(retv.Num_node() == 1);
    return retv.Node();
  }
  NODE_PTR Node(NODE_PTR node) { return node; }

public:
  //! @brief Simplify vector.roll nodes
  //!
  //! NOTE (architecture review): Rule 1 below is an algebraic identity fold
  //! (`roll(v, 0) -> v`, `roll(v, mul(x, 0)) -> v`, `RNUM={0} -> v`). In a
  //! mature compiler architecture, this rule belongs in a generalized
  //! algebraic canonicalization pass (alongside `add(x, 0) -> x`,
  //! `mul(x, 1) -> x`, `mul(x, 0) -> 0`, etc.) rather than being specialized
  //! inside this pass.
  //!
  //! It lives here today because:
  //! 1) The existing `SIMP_CTX` / `Constant_folding_opt` pass is specialized to
  //!    folding scalar multipliers into conv/gemm weights and does not perform
  //!    algebraic identity folding (its core handler inherits NULL_HANDLER);
  //! 2) `roll` is a custom VECTOR-domain opcode (domain 2) with domain-specific
  //!    rotation semantics (`shift == 0`) and `RNUM` attribute metadata;
  //! 3) In `Vector_driver`, `Constant_folding_opt` runs before `Lower_to_vector`
  //!    and `Mv2v_opt`, while `roll` nodes are only materialized during those
  //!    two lowering steps.
  //!
  //! Future refactor: if an general canonicalization pass is built, 
  //! extract Rule 1 into it as the `roll` identity entry.
  //! Rules 2 and 3 (the loop-idiom matching, splitting, and IV-renaming logic)
  //! must remain a dedicated structural pass.
  template <typename RETV, typename VISITOR>
  RETV Handle_roll(VISITOR* visitor, air::base::NODE_PTR node) {
    CONTAINER* cntr = visitor->Context().Container();
    SPOS       spos = node->Spos();

    // 1. Visit operand (input vector)
    NODE_PTR input = Node(visitor->template Visit<RETV>(node->Child(0)));
    NODE_PTR shift = Node(visitor->template Visit<RETV>(node->Child(1)));

    // Rule 1: Literal constant zero fold: roll(v, 0) -> v
    if (shift->Opcode() == air::core::OPC_INTCONST && shift->Intconst() == 0) {
      return RETV(input);
    }

    // Check if shift is a multiply by 0: mul(x, 0) or mul(0, x)
    if (shift->Opcode() == air::core::OPC_MUL) {
      NODE_PTR c0 = shift->Child(0);
      NODE_PTR c1 = shift->Child(1);
      if ((c0->Opcode() == air::core::OPC_INTCONST && c0->Intconst() == 0) ||
          (c1->Opcode() == air::core::OPC_INTCONST && c1->Intconst() == 0)) {
        return RETV(input);
      }
    }

    // Check RNUM attribute: if single element and equals 0
    if (Has_attr<int>(node, nn::core::ATTR::RNUM)) {
      uint32_t   cnt   = 0;
      const int* rnums = node->Attr<int>(nn::core::ATTR::RNUM, &cnt);
      if (rnums != nullptr && cnt == 1 && rnums[0] == 0) {
        return RETV(input);
      }
    }

    NODE_PTR new_roll = cntr->New_cust_node(
        OPCODE(nn::vector::VECTOR, nn::vector::VECTOR_OPCODE::ROLL),
        node->Rtype(), spos);
    new_roll->Set_child(0, input);
    new_roll->Set_child(1, shift);
    new_roll->Copy_attr(node);
    return RETV(new_roll);
  }
};

GLOB_SCOPE* Roll_simp_opt(GLOB_SCOPE* glob, VECTOR_CTX& ctx,
                          const air::driver::DRIVER_CTX* driver_ctx,
                          const VECTOR_CONFIG&           cfg);

}  // namespace vector
}  // namespace nn

#endif  // NN_VECTOR_ROLL_SIMP_H
