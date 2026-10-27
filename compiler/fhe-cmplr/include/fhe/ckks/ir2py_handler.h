//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_IR2PY_HANDLER_H
#define FHE_CKKS_IR2PY_HANDLER_H

// IR2PY_HANDLER mirrors fhe::ckks::IR2C_HANDLER (the CKKS-domain emitter) but
// emits Python runtime calls: the C `&dst` first out-parameter is dropped and
// turned into the assignment LHS, and the call name is lowercased to the
// functional Python runtime API (plan section 7). For example
//   C:   Add_ciph(&dst, a, b)
//   Py:  dst = add_ciph(a, b)
//
// Dispatch (fhe::ckks::HANDLER<IMPL>) resolves each CKKS opcode to
// `_impl.Handle_<name>` by *static* type, and _impl is IR2PY_HANDLER, so an
// override here shadows the inherited IR2C_HANDLER member of the same name --
// the same non-virtual mechanism IR2PY_CORE/IR2PY_CTX rely on. No `auto& ctx`
// trick is needed for *dispatch* (unlike IR2PY_CORE::Handle_block, which has to
// route non-virtual CTX helpers through the derived type); here the CTX helpers
// we call (Emit_st_var / Is_*_type / Provider / operator<<) are not overridden
// by IR2PY_CTX, so binding the derived type via `auto& ctx` is purely for
// consistency and forward-safety.
//
// Interaction with IR2PY_CORE::Handle_st / Handle_stp: when a store's value is
// a CKKS-domain op (ADD/SUB/MUL/...), the CORE handler does NOT emit `dst = `;
// it visits the CKKS op, which owns the leading `dst = ` (the ST/STP parent is
// reached via ctx.Parent(1), and Emit_st_var emits its destination name). This
// mirrors the C split where IR2C_CORE::Handle_st visits the CKKS op and
// IR2C_HANDLER::Handle_add emits `Add_ciph(&dst, ...)`.
//
// Opcode naming note (verified against opcode_def.inc + ckks_handler.h): the
// dispatch name is the 2nd macro field. In particular MODSWITCH dispatches to
// `Handle_modswitch`, but IR2C_HANDLER defines `Handle_mod_switch` (underscore)
// -- which is therefore *dead code in the C backend* (never dispatched). The
// C backend has no handler at all for NEG/UPSCALE (INVALID_HANDLER would
// assert). The Python backend provides correctly-named handlers for the full
// MVP op set per the plan's mapping table, so these fire if ever produced:
//   Handle_neg       (NEG,      kid_num=1)  -> dst = neg_ciph(a)
//   Handle_modswitch (MODSWITCH, kid_num=1) -> dst = mod_switch(a)
//   Handle_upscale   (UPSCALE,  kid_num=2)  -> dst = upscale_ciph(a, bits)
// (NEG/UPSCALE have no producer in the un-lowered non-ant CKKS IR today; they
//  are implemented for completeness so a future frontend change yields Python,
//  not an INVALID_HANDLER assert.)
//
// Bootstrap + its 3 sub-stage variants are handled below (Phase 2.4): they
// collapse to a single `dst = bootstrap_ciph(a, level, slot)` (rt_ref plaintext
// near-identity), mirroring the C non-ANT collapse to `Bootstrap(&dst, a, ...)`
// (ir2c_handler.h:199-305). The non-ant `-CKKS2PY` path never lowers to poly/bts.
// CTX_PARAM_ANA *does* run on -CKKS2PY: POLY2PY_PASS::Pre_run disables only the
// POLY/POLY2C passes (poly2py_pass.cxx:57-58), not CKKS_PASS, whose Ckks_driver
// runs ctx_param_ana.Run() (ckks.cxx:58-73) before POLY2PY_PASS emits. So a
// bootstrap node reaches Handle_bootstrap *after* CTX_PARAM_ANA's Handle_bootstrap
// (ctx_param_ana.h:1066-1140): the operand must carry RESCALE_LEVEL (:1076
// assert), the bootstrap must survive the redundancy deletion (:1078), and its
// LEVEL is normalized by Set_node_mul_level (:1120). The region-branch modswitch
// rewrite (:1096-1114) needs Rgn_bts_mng (off for the default non-region config)
// and is not hit. A -CKKS2PY bootstrap is not produced by the simple-ONNX green gate;
// this handler is exercised by the hand-built test_fhepoly_py_bootstrap (whose IR
// goes through Append_output's CTX_PARAM_ANA the same way). batch_size /
// rotate_batch are still not produced in the MVP and remain inherited from
// IR2C_HANDLER (unused).

#include <string>

#include "fhe/ckks/ir2c_handler.h"  // IR2C_HANDLER (base), NODE_PTR, Is_*_type, ...
#include "nn/vector/vector_opcode.h"  // nn::vector::OPC_SLICE (slice offline arm)

namespace fhe {
namespace ckks {

class IR2PY_HANDLER : public fhe::ckks::IR2C_HANDLER {
public:
  //! @brief add: `dst = add_ciph/add_plain/add_scalar(a, b)` (variant by the
  //! 2nd operand's type, mirroring IR2C_HANDLER::Handle_add).
  template <typename RETV, typename VISITOR>
  void Handle_add(VISITOR* visitor, air::base::NODE_PTR node) {
    auto&               ctx    = visitor->Context();
    air::base::NODE_PTR parent = ctx.Parent(1);
    AIR_ASSERT(parent != air::base::Null_ptr && parent->Is_st());
    ctx.Emit_st_var(parent);
    ctx << " = ";
    if (ctx.Is_plain_type(node->Child(1)->Rtype_id())) {
      ctx << "add_plain(";
    } else if (ctx.Is_cipher_type(node->Child(1)->Rtype_id())) {
      ctx << "add_ciph(";
    } else {
      AIR_ASSERT(node->Child(1)->Rtype()->Is_prim());
      ctx << "add_scalar(";
    }
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(1));
    ctx << ")";
  }

  //! @brief sub: `dst = sub_ciph/sub_plain/sub_scalar(a, b)` (variant by the
  //! 2nd operand's type, mirroring IR2C_HANDLER::Handle_sub).
  template <typename RETV, typename VISITOR>
  void Handle_sub(VISITOR* visitor, air::base::NODE_PTR node) {
    auto&               ctx    = visitor->Context();
    air::base::NODE_PTR parent = ctx.Parent(1);
    AIR_ASSERT(parent != air::base::Null_ptr && parent->Is_st());
    ctx.Emit_st_var(parent);
    ctx << " = ";
    if (ctx.Is_plain_type(node->Child(1)->Rtype_id())) {
      ctx << "sub_plain(";
    } else if (ctx.Is_cipher_type(node->Child(1)->Rtype_id()) ||
               ctx.Is_cipher3_type(node->Child(1)->Rtype_id())) {
      ctx << "sub_ciph(";
    } else if (node->Child(1)->Rtype()->Is_prim()) {
      ctx << "sub_scalar(";
    } else {
      // Other types (e.g. cipher3 without a cipher3 predicate hit) -> sub_ciph,
      // mirroring IR2C_HANDLER::Handle_sub's final fallback.
      ctx << "sub_ciph(";
    }
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(1));
    ctx << ")";
  }

  //! @brief mul: `dst = mul_ciph/mul_plain/mul_scalar(a, b)` (variant by the
  //! 2nd operand's type, mirroring IR2C_HANDLER::Handle_mul).
  template <typename RETV, typename VISITOR>
  void Handle_mul(VISITOR* visitor, air::base::NODE_PTR node) {
    auto&               ctx    = visitor->Context();
    air::base::NODE_PTR parent = ctx.Parent(1);
    AIR_ASSERT(parent != air::base::Null_ptr && parent->Is_st());
    ctx.Emit_st_var(parent);
    ctx << " = ";
    if (ctx.Is_plain_type(node->Child(1)->Rtype_id())) {
      ctx << "mul_plain(";
    } else if (ctx.Is_cipher3_type(node->Child(1)->Rtype_id()) ||
               ctx.Is_cipher_type(node->Child(1)->Rtype_id())) {
      ctx << "mul_ciph(";
    } else {
      AIR_ASSERT(node->Child(1)->Rtype()->Is_prim());
      ctx << "mul_scalar(";
    }
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(1));
    ctx << ")";
  }

  //! @brief neg (kid_num=1): `dst = neg_ciph(a)`. No C handler (INVALID would
  //! assert); provided per the plan's MVP table.
  template <typename RETV, typename VISITOR>
  void Handle_neg(VISITOR* visitor, air::base::NODE_PTR node) {
    Emit_una_op<RETV, VISITOR>(visitor, node, "neg_ciph");
  }

  //! @brief rotate (kid_num=2): `dst = rotate_ciph(a, idx)`.
  template <typename RETV, typename VISITOR>
  void Handle_rotate(VISITOR* visitor, air::base::NODE_PTR node) {
    Emit_bin_op<RETV, VISITOR>(visitor, node, "rotate_ciph");
  }

  //! @brief relin (kid_num=1): `dst = relin(a)`.
  template <typename RETV, typename VISITOR>
  void Handle_relin(VISITOR* visitor, air::base::NODE_PTR node) {
    Emit_una_op<RETV, VISITOR>(visitor, node, "relin");
  }

  //! @brief rescale (kid_num=1): `dst = rescale_ciph(a)`.
  template <typename RETV, typename VISITOR>
  void Handle_rescale(VISITOR* visitor, air::base::NODE_PTR node) {
    Emit_una_op<RETV, VISITOR>(visitor, node, "rescale_ciph");
  }

  //! @brief modswitch (kid_num=1): `dst = mod_switch(a)`. Dispatch name is
  //! Handle_modswitch (the C Handle_mod_switch is dead -- never dispatched).
  template <typename RETV, typename VISITOR>
  void Handle_modswitch(VISITOR* visitor, air::base::NODE_PTR node) {
    Emit_una_op<RETV, VISITOR>(visitor, node, "mod_switch");
  }

  //! @brief upscale (kid_num=2): `dst = upscale_ciph(a, bits)`. No C handler;
  //! provided per the plan's MVP table.
  template <typename RETV, typename VISITOR>
  void Handle_upscale(VISITOR* visitor, air::base::NODE_PTR node) {
    Emit_bin_op<RETV, VISITOR>(visitor, node, "upscale_ciph");
  }

  //! @brief raise_mod (kid_num=2): `dst = raise_mod(a, lev)`.
  template <typename RETV, typename VISITOR>
  void Handle_raise_mod(VISITOR* visitor, air::base::NODE_PTR node) {
    Emit_bin_op<RETV, VISITOR>(visitor, node, "raise_mod");
  }

  //! @brief conjugate (kid_num=1): `dst = conjugate_ciph(a)`.
  template <typename RETV, typename VISITOR>
  void Handle_conjugate(VISITOR* visitor, air::base::NODE_PTR node) {
    Emit_una_op<RETV, VISITOR>(visitor, node, "conjugate_ciph");
  }

  //! @brief mul_mono (kid_num=2): `dst = mul_mono_ciph(a, p)`.
  template <typename RETV, typename VISITOR>
  void Handle_mul_mono(VISITOR* visitor, air::base::NODE_PTR node) {
    Emit_bin_op<RETV, VISITOR>(visitor, node, "mul_mono_ciph");
  }

  //! @brief free (PROP_STMT, kid_num=1): `free_ciph(a)` / `free_plain(a)`
  //! (optionally `(a, cnt)` for an array). Mirrors IR2C_HANDLER::Handle_free:
  //! no `&dst` (it is a statement, not a store), lowercase name, drop nothing
  //! else (the C arg was already a bare visit of Child(0)).
  template <typename RETV, typename VISITOR>
  void Handle_free(VISITOR* visitor, air::base::NODE_PTR node) {
    auto& ctx      = visitor->Context();
    uint64_t elem_cnt = 0;
    if (node->Child(0)->Opcode() == air::core::OPC_LD &&
        node->Child(0)->Addr_datum()->Type()->Is_array()) {
      elem_cnt =
          node->Child(0)->Addr_datum()->Type()->Cast_to_arr()->Elem_count();
    }
    if (ctx.Is_cipher_type(node->Child(0)->Rtype_id()) ||
        ctx.Is_cipher3_type(node->Child(0)->Rtype_id())) {
      ctx << "free_ciph(";
    } else if (ctx.Is_plain_type(node->Child(0)->Rtype_id())) {
      ctx << "free_plain(";
    } else {
      // Mirrors C `Free_ciph_poly(...)`. Dead in the non-ant path (no poly
      // types in un-lowered CKKS IR), but kept for a faithful 1:1 mirror.
      ctx << "free_ciph_poly(";
    }
    visitor->template Visit<RETV>(node->Child(0));
    if (elem_cnt > 0) {
      ctx << ", " << elem_cnt;
    }
    ctx << ")";
  }

  //! @brief encode (kid_num=4): runtime path -> `dst = encode_float/encode_double
  //! /(encode_dcmplx)(buf, len, scale, level)`; offline data-file path (`-CKKS2PY:
  //! df=`) -> `dst = encode_float(load_data_entry(idx, len), len, scale, level)`
  //! reading the weights back from the C binary side data file (Phase 2.3).
  //!
  //! Mirrors IR2C_HANDLER::Handle_encode / IR2C_CTX::Emit_encode (ckks/
  //! ir2c_ctx.h:75-289). The runtime branch emits the functional form directly
  //! (`dst = ` + lowercased name + buffer/len/scale/level children), dropping
  //! `&dst`, the `(DCMPLX*)`/`(double*)` casts and the `&buf[0][0]` address-of
  //! (Child(0) is visited, yielding the constant name `cst_N` -- declared as a
  //! Python list by Emit_global_constants since Phase 2.2 -- or a scalar
  //! literal). The offline branch (Try_emit_offline_encode) appends the constant
  //! to the inherited RT_DATA_WRITER and references it by index, mirroring the C
  //! `Pt_from_msg(idx)` shape; child(0) is NOT visited there, so the constant is
  //! NOT emitted as a Python list (it lives in the data file).
  //!
  //! Type selection mirrors Emit_runtime_encode's domain_type derivation. The
  //! SEAL `_cst_lvl` / mask variants collapse to the plain functional name (the
  //! binding hides level-constant specialization); dcmplx is emitted as
  //! `encode_dcmplx` (its rt_ref consumer landed in Phase 2.3).
  template <typename RETV, typename VISITOR>
  void Handle_encode(VISITOR* visitor, air::base::NODE_PTR node) {
    auto&               ctx    = visitor->Context();
    air::base::NODE_PTR parent = ctx.Parent(1);
    AIR_ASSERT(parent != air::base::Null_ptr && parent->Is_st());
    // Offline data-file encode (`-CKKS2PY:df=`): Try_emit_offline_encode handles
    // the plain float32-array case (append to the C binary data file + reference
    // by index, mirroring ckks/ir2c_ctx.h:90-128); mask/dcmplx/non-float32/
    // non-LDC/slice encodes fall through to the runtime branch below (embedded
    // _cst_N list), matching the C path's handling of the non-offline cases.
    if (ctx.Emit_data_file() &&
        Try_emit_offline_encode<RETV, VISITOR>(visitor, node, parent)) {
      return;
    }
    air::base::NODE_PTR cst      = node->Child(0);
    air::base::TYPE_PTR cst_type = cst->Rtype();
    air::base::TYPE_PTR domain_type;
    const double*       mask_attr = node->Attr<double>(nn::core::ATTR::MASK);
    bool                encoding_mask = (mask_attr != nullptr);
    const uint32_t*     complex_attr =
        node->Attr<uint32_t>(fhe::core::FHE_ATTR_KIND::ENCODE_DCMPLX);
    bool encoding_dcmplx = (complex_attr != nullptr) && (*complex_attr != 0);
    if (cst_type->Is_ptr()) {
      domain_type = cst_type->Cast_to_ptr()->Domain_type();
    } else if (cst_type->Is_prim()) {
      domain_type = cst_type;
    } else if (encoding_mask) {
      domain_type = cst_type;
    } else {
      AIR_ASSERT(cst_type->Is_array());
      domain_type = cst_type->Cast_to_arr()->Elem_type();
    }
    AIR_ASSERT(domain_type->Is_prim());
    ctx.Emit_st_var(parent);
    ctx << " = ";
    if (encoding_dcmplx) {
      ctx << "encode_dcmplx(";  // Phase 2 binding; functional form emitted now
    } else {
      switch (domain_type->Cast_to_prim()->Encoding()) {
        case air::base::PRIMITIVE_TYPE::FLOAT_32:
          ctx << "encode_float(";
          break;
        case air::base::PRIMITIVE_TYPE::FLOAT_64:
        case air::base::PRIMITIVE_TYPE::INT_S32:
        case air::base::PRIMITIVE_TYPE::INT_U32:
        case air::base::PRIMITIVE_TYPE::INT_S64:
        case air::base::PRIMITIVE_TYPE::INT_U64:
          ctx << "encode_double(";
          break;
        default:
          AIR_ASSERT_MSG(false, "not supported primitive type for encoding");
          break;
      }
    }
    visitor->template Visit<RETV>(node->Child(0));  // buffer
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(1));  // element count
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(2));  // scale
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(3));  // level
    ctx << ")";
  }

  //! @brief level (kid_num=1): `dst = level(a)` when stored, else inline
  //! `level(a)`. Mirrors IR2C_HANDLER::Handle_level (Sc_degree/Level -> level).
  template <typename RETV, typename VISITOR>
  void Handle_level(VISITOR* visitor, air::base::NODE_PTR node) {
    auto&               ctx    = visitor->Context();
    air::base::NODE_PTR parent = ctx.Parent(1);
    if (parent->Is_st()) {
      ctx.Emit_st_var(parent);
      ctx << " = level(";
    } else {
      ctx << "level(";
    }
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ")";
  }

  //! @brief scale (kid_num=1): `dst = sc_degree(a)` when stored, else inline
  //! `sc_degree(a)`. Mirrors IR2C_HANDLER::Handle_scale.
  template <typename RETV, typename VISITOR>
  void Handle_scale(VISITOR* visitor, air::base::NODE_PTR node) {
    auto&               ctx    = visitor->Context();
    air::base::NODE_PTR parent = ctx.Parent(1);
    if (parent->Is_st()) {
      ctx.Emit_st_var(parent);
      ctx << " = sc_degree(";
    } else {
      ctx << "sc_degree(";
    }
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ")";
  }

  //! @brief bootstrap (kid_num=1): `dst = bootstrap_ciph(a, level, slot)`. Top-
  //! level opcode: `level` from FHE_ATTR_KIND::LEVEL, `slot` from
  //! nn::core::ATTR::SLOT (both default 0 when absent). Mirrors
  //! IR2C_HANDLER::Handle_bootstrap's non-ANT branch (ir2c_handler.h:199-228),
  //! which emits `Bootstrap(&dst, a, mul_lev, slot)`; the Python path collapses
  //! the C `&dst` out-param to the assignment LHS and the provider-branched
  //! `Bootstrap`/`Eval_bootstrap_ciph` name to one runtime name `bootstrap_ciph`
  //! (plan 2.4) so Phase 3 can swap a real openfhe-python bootstrap behind it.
  template <typename RETV, typename VISITOR>
  void Handle_bootstrap(VISITOR* visitor, air::base::NODE_PTR node) {
    Emit_bootstrap_op<RETV, VISITOR>(visitor, node, /*read_level=*/true);
  }

  //! @brief bootstrap_coeffs_to_slots (kid_num=1): collapses to
  //! `dst = bootstrap_ciph(a, 0, slot)`. Mirrors IR2C_HANDLER's non-ANT branch
  //! (ir2c_handler.h:230-254), which emits `Bootstrap(&dst, a, 0)` -- a literal
  //! 0 for the level position (no FHE_ATTR_KIND::LEVEL on the sub-stages); slot
  //! is read from nn::core::ATTR::SLOT (default 0), per the plan's "sub-stages
  //! read SLOT". C drops slot from the non-ANT output; Python keeps the uniform
  //! 3-arg shape and passes it faithfully.
  template <typename RETV, typename VISITOR>
  void Handle_bootstrap_coeffs_to_slots(VISITOR* visitor,
                                        air::base::NODE_PTR node) {
    Emit_bootstrap_op<RETV, VISITOR>(visitor, node, /*read_level=*/false);
  }

  //! @brief bootstrap_eval_mod (kid_num=1): `dst = bootstrap_ciph(a, 0, slot)`.
  //! Mirrors ir2c_handler.h:256-279 (non-ANT `Bootstrap(&dst, a, 0)`).
  template <typename RETV, typename VISITOR>
  void Handle_bootstrap_eval_mod(VISITOR* visitor, air::base::NODE_PTR node) {
    Emit_bootstrap_op<RETV, VISITOR>(visitor, node, /*read_level=*/false);
  }

  //! @brief bootstrap_slots_to_coeffs (kid_num=1): `dst = bootstrap_ciph(a, 0,
  //! slot)`. Mirrors ir2c_handler.h:281-305 (non-ANT `Bootstrap(&dst, a, 0)`).
  template <typename RETV, typename VISITOR>
  void Handle_bootstrap_slots_to_coeffs(VISITOR* visitor,
                                        air::base::NODE_PTR node) {
    Emit_bootstrap_op<RETV, VISITOR>(visitor, node, /*read_level=*/false);
  }

private:
  //! @brief Emit `dst = <name>(<child0>)` for a 1-kid CKKS op stored via ST/STP.
  //! `dst` is the store's destination (ctx.Parent(1)); `Emit_st_var` covers both
  //! ST (addr-datum name) and STP (preg id). `Is_st()` is true for any STORE
  //! opcode (ST/STP/IST); a CKKS op is never the direct RHS of an IST (the
  //! cipher IST path wraps the source in copy_ciph), so the ST/STP assert is
  //! exhaustive -- mirroring IR2C_HANDLER.
  template <typename RETV, typename VISITOR>
  void Emit_una_op(VISITOR* visitor, air::base::NODE_PTR node, const char* name) {
    auto&               ctx    = visitor->Context();
    air::base::NODE_PTR parent = ctx.Parent(1);
    AIR_ASSERT(parent != air::base::Null_ptr && parent->Is_st());
    ctx.Emit_st_var(parent);
    ctx << " = " << name << "(";
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ")";
  }

  //! @brief Emit `dst = <name>(<child0>, <child1>)` for a 2-kid CKKS op.
  template <typename RETV, typename VISITOR>
  void Emit_bin_op(VISITOR* visitor, air::base::NODE_PTR node, const char* name) {
    auto&               ctx    = visitor->Context();
    air::base::NODE_PTR parent = ctx.Parent(1);
    AIR_ASSERT(parent != air::base::Null_ptr && parent->Is_st());
    ctx.Emit_st_var(parent);
    ctx << " = " << name << "(";
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(1));
    ctx << ")";
  }

  //! @brief Emit `dst = bootstrap_ciph(a, level, slot)` for the 4 bootstrap
  //! opcodes (BOOTSTRAP + the 3 sub-stages) -- the non-ANT Python collapse of
  //! C's `Bootstrap`/`Eval_bootstrap_*` (ir2c_handler.h:199-305). `read_level`
  //! true: level = FHE_ATTR_KIND::LEVEL attr (top-level BOOTSTRAP, mirrors C
  //! reading mul_lev, default 0); false: level = 0 literal (sub-stages, mirrors
  //! C non-ANT `Bootstrap(&dst, a, 0)` -- the sub-stages carry no LEVEL attr).
  //! slot = nn::core::ATTR::SLOT attr (default 0) in both, per the plan's
  //! "top-level reads LEVEL, sub-stages read SLOT". rt_ref.bootstrap_ciph is a
  //! plaintext near-identity (message preserved, level reset to `level`); real
  //! HE bootstrap is Phase 3. Need_bts: IR2PY_CTX::Emit_need_bts stays a no-op
  //! `{}` (the need_bts signal flows as a `need_bts` key in get_context_params,
  //! see poly2py_driver.cxx), but -- mirroring IR2C_HANDLER::Handle_bootstrap
  //! (ir2c_handler.h:225-227) -- Emit_bootstrap_op sets `ctx._need_bts = true`
  //! so that key is emitted, gating rt_openfhe's _build_bootstrap_context
  //! (Phase 3 wiring; only fires when ReSBM inserts OPC_BOOTSTRAP, i.e. -CKKS:sbm).
  template <typename RETV, typename VISITOR>
  void Emit_bootstrap_op(VISITOR* visitor, air::base::NODE_PTR node,
                         bool read_level) {
    auto&               ctx    = visitor->Context();
    air::base::NODE_PTR parent = ctx.Parent(1);
    AIR_ASSERT(parent != air::base::Null_ptr && parent->Is_st());
    const uint32_t* slot     = node->Attr<uint32_t>(nn::core::ATTR::SLOT);
    uint32_t        slot_val = (slot == nullptr) ? 0 : *slot;
    uint32_t        lev_val  = 0;
    if (read_level) {
      const uint32_t* mul_lev =
          node->Attr<uint32_t>(fhe::core::FHE_ATTR_KIND::LEVEL);
      lev_val = (mul_lev == nullptr) ? 0 : *mul_lev;
    }
    ctx.Emit_st_var(parent);
    ctx << " = bootstrap_ciph(";
    visitor->template Visit<RETV>(node->Child(0));
    ctx << ", " << lev_val << ", " << slot_val << ")";
    // Mirror IR2C_HANDLER::Handle_bootstrap (ir2c_handler.h:225-227): flag that
    // this module needs a bootstrap context, so Emit_get_context_params emits
    // 'need_bts': True and rt_openfhe builds the bootstrap context (Phase 3).
    if (!ctx._need_bts) {
      ctx._need_bts = true;
    }
  }

  //! @brief Offline data-file encode (Phase 2.3, `-CKKS2PY:df=`). Mirrors the C
  //! IR2C_CTX::Emit_encode offline branch (ckks/ir2c_ctx.h:90-128): when
  //! child(0) is an LDC of a FLOAT_32 ARRAY constant and children 1..3 are
  //! INTCONSTs (the form real ONNX float32 initializers lower to), append the
  //! constant's bytes to the inherited `RT_DATA_WRITER` (the C binary side data
  //! file opened by the base ckks::IR2C_CTX ctor on `-df=`) and reference the
  //! weights by index -- rt_ref reads them back via `load_data_entry(idx, n)`
  //! and `encode_float`s them. Returns true when it handled the node (so
  //! Handle_encode returns); returns false to let the runtime branch handle
  //! mask / dcmplx / non-float32 / non-LDC encodes -- runtime-encoded with the
  //! constant embedded as a Python list. PARITY NOTE: this matches the C path
  //! for mask/dcmplx/non-float32 (C also runtime-encodes those). Slice encodes
  //! ARE offlined -- the SLICE arm above mirrors ckks/ir2c_ctx.h:180-248,
  //! closing the former -df= parity gap that bloated F-family .py to GB; only
  //! mask/dcmplx/non-float32/non-LDC slice-encodes fall through to the runtime
  //! branch (C also runtime-encodes those), which still computes the correct
  //! plaintext under rt_ref.
  //!
  //! The constant is NOT visited (only its data is read for the append); we
  //! additionally record its id via Mark_offlined_cst so Emit_global_constants
  //! skips re-emitting it as a Python list -- its bytes now live in the data
  //! file and the body references them by index. Mirrors the C path, which emits
  //! only the index. The writer is a public member of the base ckks::IR2C_CTX
  //! (reused, not wrapped): the Python backend never runs the C Emit_encode (its
  //! own Handle_encode is dispatched), so there is no double-append; default-C
  //! (no `-CKKS2PY`) never creates the writer, so this is reached only on the
  //! `-CKKS2PY:df=` path.
  template <typename RETV, typename VISITOR>
  bool Try_emit_offline_encode(VISITOR* visitor, air::base::NODE_PTR node,
                               air::base::NODE_PTR parent) {
    auto& ctx = visitor->Context();
    // dcmplx is never offline (ckks/ir2c_ctx.h:90 guards `!encoding_dcmplx`).
    const uint32_t* complex_attr =
        node->Attr<uint32_t>(fhe::core::FHE_ATTR_KIND::ENCODE_DCMPLX);
    if (complex_attr != nullptr && *complex_attr != 0) return false;
    if (ctx._rt_data_writer == nullptr) return false;  // no data file
    // -----------------------------------------------------------------------
    // SLICE offline arm (rtpy-df, Design A-minimal). Mirrors the C slice branch
    // (ckks/ir2c_ctx.h:180-248, the live `#if 1` multi-entry path) so `-CKKS2PY:df=`
    // also offlines OPC_SLICE-formed per-slot weight encodes. Without this arm
    // the Python path devolves slice to the embedded-list runtime branch (the
    // -df= parity gap formerly noted in the PARITY CAVEAT below), bloating the
    // emitted .py to GB for F-family conv models (resnet44 2.65 GB, vgg up to
    // 13 GB). Inherits Parse_subscript_expr / Parse_do_loop / _rt_data_writer
    // (public members of base ckks::IR2C_CTX); ckks/ir2c_ctx.h is NOT modified
    // (no shared-method extraction, per PLAN section 3/section 11.1). The loop body mirrors
    // ir2c_ctx.h:192-228: each slice Append writes `count` floats per LUT entry
    // `cst_<id>_<i>` (loop_cnt consecutive entries); the body references them by
    // `load_data_entry(Visit(start) + base_idx, count)` -- rt_ref reads the LUT
    // entry by runtime IV index, deterministically equivalent to C's
    // Pt_from_msg(start + idx). The Stage-0.5 gate (doc/rtpy-df/GATE_RESULTS.md)
    // confirmed all F-family slice subscripts are parseable and no offlined
    // array is also referenced inline (no NameError), so this arm is safe across
    // the collapsible F subset. Parse / DO_LOOP-walk failure -> AIR_ASSERT
    // (faithful mirror of C :194/:208/:211, hard crash = C-same behavior on
    // gate-uncovered edge cases). Non-FLOAT_32 / non-INTCONST-span / non-ARRAY /
    // unresolvable scale-level -> return false (graceful runtime inline, correct
    // output; mirrors the existing ARRAY-LDC arm's lenient fallback below).
    // -----------------------------------------------------------------------
    if (node->Child(0)->Opcode() == nn::vector::OPC_SLICE &&
        node->Child(1)->Opcode() == air::core::OPC_INTCONST) {
      air::base::NODE_PTR slice   = node->Child(0);
      air::base::NODE_PTR cst_ldc = slice->Child(0);  // LDC of the ARRAY weight
      air::base::NODE_PTR start   = slice->Child(1);  // runtime IV subscript
      if (cst_ldc->Opcode() != air::core::OPC_LDC) return false;
      if (slice->Child(2)->Opcode() != air::core::OPC_INTCONST) return false;  // span
      air::base::CONSTANT_PTR cst = cst_ldc->Const();
      if (cst == air::base::Null_ptr ||
          cst->Kind() != air::base::CONSTANT_KIND::ARRAY ||
          !cst->Type()->Is_array()) {
        return false;
      }
      air::base::TYPE_PTR elem = cst->Type()->Cast_to_arr()->Elem_type();
      if (elem->Is_array()) elem = elem->Cast_to_arr()->Elem_type();  // flatten
      if (!elem->Is_prim() ||
          elem->Cast_to_prim()->Encoding() !=
              air::base::PRIMITIVE_TYPE::FLOAT_32) {
        return false;  // runtime inline handles non-f32 (mirrors ARRAY-LDC arm)
      }
      // --- mirror ir2c_ctx.h:192-214: parse the subscript, then walk enclosing
      // DO_LOOPs (by Iv_id up the visitor parent stack) for loop_cnt = prod(ub).
      std::vector<std::pair<int64_t, int64_t>> subscript;
      bool ok = ctx.Parse_subscript_expr(start, subscript);
      AIR_ASSERT(ok && subscript.size() > 0);  // faithful mirror of C :194
      uint64_t loop_cnt = 1;
      for (uint64_t i = 0; i < subscript.size(); ++i) {
        air::base::NODE_PTR loop = air::base::Null_ptr;
        for (size_t depth = 1;; ++depth) {
          air::base::NODE_PTR cand = visitor->Parent(depth);
          if (cand == air::base::Null_ptr) break;
          if (cand->Opcode() == air::core::DO_LOOP &&
              cand->Iv_id().Value() == subscript[i].first) {
            loop = cand;
            break;
          }
        }
        AIR_ASSERT(loop != air::base::Null_ptr);  // mirror C :208
        int64_t lb, ub, stride;
        ok = ctx.Parse_do_loop(loop, lb, ub, stride);
        AIR_ASSERT(ok && lb == 0 && stride == 1);  // mirror C :211
        loop_cnt *= static_cast<uint64_t>(ub);
      }
      const float* data  = cst->Array_ptr<float>();
      uint64_t     total = cst->Array_byte_len() / sizeof(float);
      uint64_t     span  = slice->Child(2)->Intconst();
      uint64_t     count = node->Child(1)->Intconst();
      // scale/level attrs-first (DESIGN section 6.5: the LUT-entry sc/lv are unused
      // metadata; decode uses these emit-passed values). Non-INTCONST child
      // fallback -> return false (runtime inline handles it).
      const uint32_t* scale_attr =
          node->Attr<uint32_t>(fhe::core::FHE_ATTR_KIND::SCALE);
      const uint32_t* level_attr =
          node->Attr<uint32_t>(fhe::core::FHE_ATTR_KIND::LEVEL);
      uint32_t sc, lv;
      if (scale_attr != nullptr) {
        sc = *scale_attr;
      } else if (node->Child(2)->Opcode() == air::core::OPC_INTCONST) {
        sc = static_cast<uint32_t>(node->Child(2)->Intconst());
      } else {
        return false;
      }
      if (level_attr != nullptr) {
        lv = *level_attr;
      } else if (node->Child(3)->Opcode() == air::core::OPC_INTCONST) {
        lv = static_cast<uint32_t>(node->Child(3)->Intconst());
      } else {
        return false;
      }
      // --- mirror ir2c_ctx.h:220-228: append loop_cnt consecutive LUT entries,
      // each `count` floats starting at data + i*span; base_idx = i==0 index.
      uint64_t base_idx = 0;
      for (uint64_t i = 0; i < loop_cnt; ++i) {
        AIR_ASSERT(total >= i * span + count);  // mirror C :221
        std::string name = "cst_" + std::to_string(cst->Id().Value()) + "_" +
                           std::to_string(i);
        uint64_t idx = ctx._rt_data_writer->Append(
            name.c_str(), data + i * span, static_cast<uint32_t>(count),
            static_cast<int>(sc), static_cast<int>(lv));
        if (i == 0) base_idx = idx;
      }
      // Skip re-emitting the underlying ARRAY as a Python list (its bytes now
      // live in the data file; the body references them by index). cst->Id() is
      // the underlying ARRAY id (slice->Child(0)->Const()->Id()), so
      // Emit_global_constants skips _cst_<id>. The G2 gate (GATE_RESULTS.md)
      // confirmed no offlined array is also referenced inline -> no NameError.
      ctx.Mark_offlined_cst(cst->Id());
      // `dst = encode_float(load_data_entry(Visit(start) + base_idx, count),
      // count, sc, lv)` -- rt_ref.load_data_entry(int(idx), n) reads the slice's
      // LUT entry by runtime IV index (Visit(start) emits the IV subscript expr,
      // e.g. `cap_block_n1_7 + num_grid_n1_5 * 9`), then encode_float encodes it.
      ctx.Emit_st_var(parent);
      ctx << " = encode_float(load_data_entry(";
      visitor->template Visit<RETV>(start);
      ctx << " + " << base_idx << ", " << count << "), " << count << ", " << sc
          << ", " << lv << ")";
      return true;
    }
    air::base::NODE_PTR cst_node = node->Child(0);
    if (cst_node->Opcode() != air::core::OPC_LDC) return false;
    if (node->Child(1)->Opcode() != air::core::OPC_INTCONST ||
        node->Child(2)->Opcode() != air::core::OPC_INTCONST ||
        node->Child(3)->Opcode() != air::core::OPC_INTCONST) {
      return false;
    }
    air::base::CONSTANT_PTR cst = cst_node->Const();
    if (cst == air::base::Null_ptr ||
        cst->Kind() != air::base::CONSTANT_KIND::ARRAY) {
      return false;
    }
    air::base::TYPE_PTR elem = cst->Type()->Cast_to_arr()->Elem_type();
    // Flatten a multi-dim array to its base prim. C's offline branch instead
    // ASSERTs Elem_type()->Is_prim() (ir2c_ctx.h:100, i.e. 1-D only) and routes
    // 2-D through the separate slice branch; this ARRAY-LDC arm does not take the
    // slice path, so it leniently flattens. Byte-level Array_ptr<T>/Array_byte_len
    // keep the emitted data/count correct either way, so this is a
    // strictness/faithfulness gap, not a wrong-output bug.
    if (elem->Is_array()) elem = elem->Cast_to_arr()->Elem_type();  // flatten
    if (!elem->Is_prim() ||
        elem->Cast_to_prim()->Encoding() !=
            air::base::PRIMITIVE_TYPE::FLOAT_32) {
      return false;
    }
    const float* data  = cst->Array_ptr<float>();
    uint64_t     count = cst->Array_byte_len() / sizeof(float);
    uint32_t     n     = static_cast<uint32_t>(node->Child(1)->Intconst());
    AIR_ASSERT_MSG(count >= n, "offline encode: array shorter than encode len");
    // scale/level: prefer the FHE_ATTR_KIND::SCALE/LEVEL attrs (the C path
    // reads these), else the INTCONST children.
    const uint32_t* scale_attr =
        node->Attr<uint32_t>(fhe::core::FHE_ATTR_KIND::SCALE);
    const uint32_t* level_attr =
        node->Attr<uint32_t>(fhe::core::FHE_ATTR_KIND::LEVEL);
    uint32_t sc = (scale_attr != nullptr)
                      ? *scale_attr
                      : static_cast<uint32_t>(node->Child(2)->Intconst());
    uint32_t lv = (level_attr != nullptr)
                      ? *level_attr
                      : static_cast<uint32_t>(node->Child(3)->Intconst());
    std::string name = "cst_" + std::to_string(cst->Id().Value());
    uint64_t     idx = ctx._rt_data_writer->Append(
        name.c_str(), data, static_cast<uint32_t>(count), static_cast<int>(sc),
        static_cast<int>(lv));
    // Record the offlined array id so Emit_global_constants skips re-emitting it
    // as a Python list (its bytes now live in the data file; the body references
    // them by index via load_data_entry). df= off short-circuits above at the
    // `_rt_data_writer == nullptr` guard, so this mark only fires on the -CKKS2PY:df=
    // path -- never marking an array that was not actually offlined.
    ctx.Mark_offlined_cst(cst->Id());
    // `dst = encode_float(load_data_entry(idx, n), n, sc, lv)` -- rt_ref reads
    // the weights back from the data file via load_data_entry, then encodes.
    ctx.Emit_st_var(parent);
    ctx << " = encode_float(load_data_entry(" << idx << ", " << n << "), " << n
        << ", " << sc << ", " << lv << ")";
    return true;
  }
};

}  // namespace ckks
}  // namespace fhe

#endif  // FHE_CKKS_IR2PY_HANDLER_H
