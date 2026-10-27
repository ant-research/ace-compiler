//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CKKS_IR2PY_CORE_H
#define FHE_CKKS_IR2PY_CORE_H

// IR2PY_CORE mirrors fhe::ckks::IR2C_CORE (the CORE-domain emitter) but emits
// Python. It is the IMPL used by air::core::HANDLER<IR2PY_CORE> inside
// CKKS2PY_VISITOR (see ckks2py_driver.h).
//
// It inherits air::core::IR2C_HANDLER (NOT IR2C_CORE), so the CORE ops whose
// base emission is already Python-safe are reused verbatim:
//   - arithmetic + - * / % , comparisons < <= > >= == != , band/bor `&`/`|`,
//     bnot `~`, intconst, lda, ret, intrn_call/op, ldc, array, ldca, ld/ldp
//     (base Handle_ld/ldp only Emit_var the name -- no C `&`; the C `&` lives
//     only in IR2C_CORE, which we do NOT inherit), call (base Handle_call emits
//     `ret = sym(args)` with no `&`/`;`).
// Only the ops whose base/C emission is C-only are overridden here (Phase 1.2):
//   - Handle_do_loop     -> `for iv in range(lb, ub, stride):` (canonical, via
//                           Parse_do_loop) / `while` fallback (non-canonical).
//   - Handle_if          -> `if <cond>:` / `elif <cond>:` / `else:` (collapses
//                           `else { if ... }` into `elif`, plan rule 4).
//   - Handle_st          -> `dst = zero_ciph()` / `dst = copy_ciph(src)` /
//                           scalar `dst = <val>` (drops C `&dst` out-param).
//   - Handle_stp         -> `<preg> = copy_ciph(<src>)` / `<preg> = <val>`.
//   - Handle_ist         -> `<addr> = copy_ciph(<src>)` / `<addr> = <val>`
//                           (drops C `*` dereference).
//   - Handle_zero/one    -> `0` / `1` (IR2C_CORE adds these; not in our base).
//   - Handle_and/Handle_or-> ` and ` / ` or ` (C logical `&&`/`||`; the IR
//                           opcodes are AND/OR -> Handle_and/Handle_or. There
//                           is no LAND/LOR/LNOT opcode; bnot `~` is reused.)
//   - Handle_ild         -> bare address / `var[idx]` (drops C `*` dereference).
//
// Phase 1.1 overrides kept here:
//   - Handle_block       : route statement openers/terminators through the
//                          *derived* IR2PY_CTX (End_stmt with no `;`). The base
//                          handler binds the CTX through an air::base::IR2C_CTX&
//                          reference, which (non-virtual) would resolve End_stmt
//                          to the C base and re-introduce `;`; binding the
//                          derived type here fixes that.
//   - Handle_retv        : program entry -> `set_output_data("name", 0, var)`
//                          + `return True`; other functions delegate to base.
//   - Handle_comment     : `# ...` instead of C `// ...`.
//   - Handle_pragma      : `# pragma: ...` instead of C `// pragma: ...`.
//
// Phase 2.1 overrides (observability / metadata ops; mirror IR2C_CORE
// :204-249 but Python-ized -- lowercase call names, no C `&`/`;`):
//   - Handle_validate    -> `validate(ciph, msg, len, eps)` (4 kids: cipher
//                           result, message-op result, length, epsilon).
//   - Handle_tm_start    -> `tm_start("label")` ; Handle_tm_taken ->
//                           `tm_taken("label")` (Emit_constant_str_init emits a
//                           valid Python "str"). NOTE: tm_start/tm_taken are
//                           NOT dispatched CORE opcodes today (ckks timing is
//                           INTRN_CALL-based + raw driver text in POLY2C), so
//                           these are parity handlers for the day such an
//                           opcode flows into -CKKS2PY; rt_ref's tm_* are no-ops
//                           (plan decision #4). They are exercised at the
//                           runtime layer (rt_ref unit tests), not via an
//                           emitted opcode (tm_start is not constructable).
//   - Handle_dump_var    -> `dump_cipher_msg(name, ciph, len)` (3 kids: name
//                           str ldc, cipher, size).
//
// Handle_func_entry is provided for completeness (faithful to the plan) but is
// NOT reached by CKKS2PY_DRIVER::Run, which emits `def`/body directly through
// the concrete IR2PY_CTX member (so Emit_func_def resolves to the Python `def`,
// not the C base). Should a FUNC_ENTRY ever be visited, this override routes
// the structure helpers through the derived CTX so it emits valid Python too.

#include "air/core/ir2c_handler.h"   // air::core::IR2C_HANDLER (base), Null_ptr/NODE_PTR/...
#include "air/core/opcode.h"         // air::core::OPC_*, CORE
#include "fhe/ckks/ckks_opcode.h"    // fhe::ckks::CKKS_DOMAIN::ID
#include "fhe/ckks/ir2py_ctx.h"      // IR2PY_CTX (derived CTX type for Handle_block)

namespace fhe {
namespace ckks {

class IR2PY_CORE : public air::core::IR2C_HANDLER {
public:
  //! @brief Iterate a BLOCK routing statement openers/terminators through the
  //! derived IR2PY_CTX (so End_stmt emits a bare newline, no C `;`). Otherwise
  //! identical to air::core::IR2C_HANDLER::Handle_block.
  template <typename RETV, typename VISITOR>
  RETV Handle_block(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(node->Is_block());
    auto& ctx = visitor->Context();  // fhe::ckks::IR2PY_CTX& (derived)
    ctx.Begin_block(node);
    for (air::base::STMT_PTR stmt = node->Begin_stmt();
         stmt != node->End_stmt(); stmt = stmt->Next()) {
      air::base::NODE_PTR snode = stmt->Node();
      ctx.Begin_stmt(snode);
      visitor->template Visit<RETV>(snode);
      ctx.End_stmt(snode);
    }
    ctx.End_block(node);
    return RETV();
  }

  //! @brief Emit `def name(params):` + body for a FUNC_ENTRY (routes the
  //! structure helpers through the derived CTX). Not reached by the Python
  //! driver (it emits `def`/body directly); provided for completeness.
  template <typename RETV, typename VISITOR>
  RETV Handle_func_entry(VISITOR* visitor, air::base::NODE_PTR node) {
    auto&               ctx    = visitor->Context();
    air::base::FUNC_SCOPE* fscope = node->Func_scope();
    ctx.Emit_func_def(fscope);
    ctx.Begin_func_body(node);
    ctx.Emit_local_var(fscope);
    visitor->template Visit<RETV>(node->Last_child());  // body block
    ctx.End_func_body(node);
    ctx << "\n";
    return RETV();
  }

  //! @brief Canonical affine DO_LOOP `for (iv=lb; iv<ub; iv+=stride) body` ->
  //! Python `for iv in range(lb, ub, stride):` (range excludes ub, matching
  //! `iv < ub`). Non-canonical loops fall back to a `while` (init; while cond:
  //! body; iv = incr). The head `:` + newline is emitted here because the body
  //! block's Begin_block (IR2PY_CTX override) only adjusts indent, unlike the C
  //! base which emits `{` + newline.
  template <typename RETV, typename VISITOR>
  RETV Handle_do_loop(VISITOR* visitor, air::base::NODE_PTR node) {
    auto&    ctx = visitor->Context();
    int64_t  lb = 0, ub = 0, stride = 0;
    if (ctx.Parse_do_loop(node, lb, ub, stride)) {
      ctx << "for ";
      ctx.Emit_var(node->Iv());
      ctx << " in range(" << lb << ", " << ub << ", " << stride << "):"
          << std::endl;
      Emit_suite<RETV, VISITOR>(visitor, node->Child(3));  // body (pass if empty)
      return RETV();
    }
    // Non-canonical -> while fallback (edge case, plan risk #5). Emit init,
    // then a synthesized body block holding the original body statements plus
    // the increment, all at one indent level (Python requires a consistent
    // suite indent).
    ctx.Emit_var(node->Iv());
    ctx << " = ";
    visitor->template Visit<RETV>(node->Child(0));  // init
    ctx << "\n";
    ctx << std::string(ctx.Level() * 2, ' ') << "while ";
    visitor->template Visit<RETV>(node->Child(1));  // cond
    ctx << ":" << std::endl;
    ctx.Begin_block(node);                          // Level L -> L+1
    air::base::NODE_PTR body_blk = node->Child(3);
    if (body_blk != air::base::Null_ptr && body_blk->Is_block()) {
      for (air::base::STMT_PTR s = body_blk->Begin_stmt();
           s != body_blk->End_stmt(); s = s->Next()) {
        air::base::NODE_PTR snode = s->Node();
        ctx.Begin_stmt(snode);
        visitor->template Visit<RETV>(snode);
        ctx.End_stmt(snode);
      }
    } else {
      // Non-block body: indent it like a statement (Begin_stmt emits the
      // Level()*2 indent that the per-opcode handlers do not emit themselves).
      ctx.Begin_stmt(body_blk);
      visitor->template Visit<RETV>(body_blk);
      ctx.End_stmt(body_blk);
    }
    ctx.Begin_stmt(node);  // iv = <incr>
    ctx.Emit_var(node->Iv());
    ctx << " = ";
    visitor->template Visit<RETV>(node->Child(2));  // incr
    ctx.End_stmt(node);
    ctx.End_block(node);  // Level L+1 -> L
    return RETV();
  }

  //! @brief `if <cond>:` / `elif <cond>:` / `else:`. An else block whose only
  //! statement is an IF collapses to `elif` (plan rule 4). The head `:` +
  //! newline is emitted here (Begin_block only adjusts indent).
  template <typename RETV, typename VISITOR>
  RETV Handle_if(VISITOR* visitor, air::base::NODE_PTR node) {
    Emit_if_chain<RETV, VISITOR>(visitor, node, "if ");
    return RETV();
  }

  //! @brief CORE LD: base Handle_ld only Emit_var's the name (no `&`). The C
  //! `&` prefix lives in IR2C_CORE::Handle_ld, which we do NOT inherit, so no
  //! override is needed -- the inherited base is already Python-correct.
  // (Handle_ld deliberately not overridden.)

  //! @brief CORE LDP: same as LD -- base Handle_ldp only Emit_preg_id (no `&`).
  // (Handle_ldp deliberately not overridden.)

  //! @brief CORE LDC: the base `air::core::IR2C_HANDLER::Handle_ldc` is already
  //! Python-correct (ARRAY kind -> `Emit_constant_name` -> `_cst_N`; scalars ->
  //! literals), so emission is NOT overridden -- we delegate to the base. The
  //! ONLY addition is recording the array constant id as used-by-body, so
  //! `IR2PY_CTX::Emit_global_constants` can use-prune dead arrays (rtpy-df B):
  //! air-infra's `_cst_used` (populated by `Emit_constant_name` in the C path)
  //! is private in `IR2C_UTIL` with no public accessor and air-infra is
  //! read-only, so Python must self-track. Only ARRAY-kind constants are emitted
  //! as `_cst_N` names (scalars are inline literals and never bloat the .py), so
  //! only those are recorded. See `poly/ir2py_ctx.h` Emit_global_constants.
  template <typename RETV, typename VISITOR>
  RETV Handle_ldc(VISITOR* visitor, air::base::NODE_PTR node) {
    air::base::CONSTANT_PTR cst = node->Const();
    if (cst != air::base::Null_ptr &&
        cst->Kind() == air::base::CONSTANT_KIND::ARRAY) {
      visitor->Context().Mark_used_cst(cst->Id());
    }
    return air::core::IR2C_HANDLER::template Handle_ldc<RETV, VISITOR>(
        visitor, node);
  }

  //! @brief CORE LDCA: mirror of Handle_ldc -- the base emits the array name
  //! (`Emit_constant_name` -> `_cst_N`); we additionally record the array id as
  //! used-by-body for the same use-prune (rtpy-df B). F-family models reference
  //! arrays via LDC (pre-verify confirmed), but `Emit_global_constants` is a
  //! global change affecting all -CKKS2PY models, so LDCA is covered too for safety
  //! (a non-F model referencing an array via LDCA must not be false-pruned).
  template <typename RETV, typename VISITOR>
  RETV Handle_ldca(VISITOR* visitor, air::base::NODE_PTR node) {
    air::base::CONSTANT_PTR cst = node->Const();
    if (cst != air::base::Null_ptr &&
        cst->Kind() == air::base::CONSTANT_KIND::ARRAY) {
      visitor->Context().Mark_used_cst(cst->Id());
    }
    return air::core::IR2C_HANDLER::template Handle_ldca<RETV, VISITOR>(
        visitor, node);
  }

  //! @brief CORE ST -> Python assignment. Cipher targets become runtime calls
  //! (zero_ciph / copy_ciph); CKKS-domain RHS is visited (the CKKS handler, in
  //! Phase 1.3, emits `dst = op(...)` itself, mirroring C's `Add_ciph(&dst,...)`
  //! where the handler owns the `&dst`); scalars become `dst = <val>`. Mirrors
  //! IR2C_CORE::Handle_st, dropping the C `&` out-param.
  template <typename RETV, typename VISITOR>
  RETV Handle_st(VISITOR* visitor, air::base::NODE_PTR node) {
    auto&               ctx = visitor->Context();
    air::base::NODE_PTR val = node->Child(0);
    air::base::TYPE_ID  type = node->Addr_datum()->Type_id();
    air::base::OPCODE   val_opc = val->Opcode();
    if (ctx.Is_cipher_type(type) && val_opc == air::core::OPC_ZERO) {
      ctx.Emit_var(node);
      ctx << " = zero_ciph()";
      return RETV();
    }
    // E (2026-08-12): array-of-cipher zero-init -> [zero_ciph() for _ in range(N)].
    // The scalar Is_cipher_type branch above does not fire for an ARRAY type, so
    // a ST of ZERO into a cipher-array accumulator (e.g. sharded conv VS_out,
    // read-before-write via add_ciph(VS_out[idx], ...)) would fall through to
    // `dst = <visit ZERO>` = `dst = 0`, destroying the `[None]*N` decl and
    // crashing the later subscript (`TypeError: 'int' not subscriptable`). Emit
    // a per-element zero_ciph list instead -- the Python analogue of the C
    // `CIPHERTEXT arr[N]` the conv accumulator needs (each slot a fresh zero so
    // add_ciph reads a cipher, not None/int).
    {
      air::base::TYPE_PTR tp = node->Addr_datum()->Type();
      if (tp->Is_array() && val_opc == air::core::OPC_ZERO &&
          ctx.Is_cipher_type(tp->Cast_to_arr()->Elem_type()->Id())) {
        uint64_t n = tp->Cast_to_arr()->Elem_count();
        ctx.Emit_var(node);
        ctx << " = [zero_ciph() for _ in range(" << n << ")]";
        return RETV();
      }
    }
    if (ctx.Is_cipher_type(type) &&
        (val_opc == air::core::OPC_LD || val_opc == air::core::OPC_LDP)) {
      ctx.Emit_var(node);
      ctx << " = copy_ciph(";
      visitor->template Visit<RETV>(val);
      ctx << ")";
      return RETV();
    }
    if (val_opc.Domain() == fhe::ckks::CKKS_DOMAIN::ID) {
      // CKKS-domain op: its handler emits the leading `dst = ` (Phase 1.3).
      visitor->template Visit<RETV>(val);
    } else {
      ctx.Emit_var(node);
      ctx << " = ";
      visitor->template Visit<RETV>(val);
    }
    return RETV();
  }

  //! @brief CORE STP -> `<preg> = copy_ciph(<src>)` / `<preg> = <val>`.
  //! Mirrors IR2C_CORE::Handle_stp, dropping the C `&` (and the `&` prefix it
  //! adds for an ILD source -- Python has no address-of).
  template <typename RETV, typename VISITOR>
  RETV Handle_stp(VISITOR* visitor, air::base::NODE_PTR node) {
    auto&               ctx = visitor->Context();
    air::base::NODE_PTR val = node->Child(0);
    if (val->Opcode().Domain() == fhe::ckks::CKKS_DOMAIN::ID) {
      visitor->template Visit<RETV>(val);
      return RETV();
    }
    if (ctx.Is_cipher_type(val->Rtype_id())) {
      ctx.Emit_preg_id(node->Preg_id());
      ctx << " = copy_ciph(";
      visitor->template Visit<RETV>(val);
      ctx << ")";
      return RETV();
    }
    ctx.Emit_preg_id(node->Preg_id());
    ctx << " = ";
    visitor->template Visit<RETV>(val);
    return RETV();
  }

  //! @brief CORE IST -> `<addr> = copy_ciph(<src>)` (cipher) / `<addr> = <val>`
  //! (scalar). Mirrors IR2C_CORE::Handle_ist, dropping the C `&`/`*` (the addr
  //! is visited, yielding `var[idx]` for ARRAY forms via Handle_array).
  template <typename RETV, typename VISITOR>
  RETV Handle_ist(VISITOR* visitor, air::base::NODE_PTR node) {
    auto&               ctx = visitor->Context();
    air::base::NODE_PTR addr = node->Child(0);
    air::base::NODE_PTR rhs  = node->Child(1);
    if (ctx.Is_cipher_type(node->Access_type_id())) {
      visitor->template Visit<RETV>(addr);
      ctx << " = copy_ciph(";
      visitor->template Visit<RETV>(rhs);
      ctx << ")";
      return RETV();
    }
    visitor->template Visit<RETV>(addr);
    ctx << " = ";
    visitor->template Visit<RETV>(rhs);
    return RETV();
  }

  //! @brief Emit `0` for ZERO (IR2C_CORE adds this; not in our base, which
  //! would assert "Unexpected ZERO").
  template <typename RETV, typename VISITOR>
  RETV Handle_zero(VISITOR* visitor, air::base::NODE_PTR node) {
    (void)node;
    visitor->Context() << "0";
    return RETV();
  }

  //! @brief Emit `1` for ONE (mirrors IR2C_CORE::Handle_one).
  template <typename RETV, typename VISITOR>
  RETV Handle_one(VISITOR* visitor, air::base::NODE_PTR node) {
    (void)node;
    visitor->Context() << "1";
    return RETV();
  }

  //! @brief Logical AND -> Python `and` (C `&&`). The IR opcode AND dispatches
  //! to Handle_and (there is no LAND opcode; base Handle_land is dead code).
  template <typename RETV, typename VISITOR>
  RETV Handle_and(VISITOR* visitor, air::base::NODE_PTR node) {
    return air::core::IR2C_HANDLER::template Handle_binop<RETV>(visitor, node,
                                                                " and ");
  }

  //! @brief Logical OR -> Python `or` (C `||`).
  template <typename RETV, typename VISITOR>
  RETV Handle_or(VISITOR* visitor, air::base::NODE_PTR node) {
    return air::core::IR2C_HANDLER::template Handle_binop<RETV>(visitor, node,
                                                                " or ");
  }

  //! @brief ILOAD -> bare address / `var[idx]`. Drops the C `*` dereference
  //! (Python has no pointers; ARRAY address forms are already subscript-correct
  //! via Handle_array).
  template <typename RETV, typename VISITOR>
  RETV Handle_ild(VISITOR* visitor, air::base::NODE_PTR node) {
    visitor->template Visit<RETV>(node->Child(0));  // address (no `*`)
    return RETV();
  }

  //! @brief Emit `set_output_data("name", 0, var)` + `return True` for the
  //! program entry (mirrors IR2C_CORE::Handle_retv, functional Python style,
  //! no `&`/`;`). Other functions delegate to the base `return <child>`.
  template <typename RETV, typename VISITOR>
  void Handle_retv(VISITOR* visitor, air::base::NODE_PTR node) {
    auto&               ctx = visitor->Context();
    air::base::NODE_PTR val = node->Child(0);
    if (node->Func_scope()->Owning_func()->Entry_point()->Is_program_entry()) {
      AIR_ASSERT(val->Opcode() == air::core::OPC_LD);
      const char* name = val->Addr_datum()->Base_sym()->Name()->Char_str();
      ctx.Set_output_name(name);
      ctx << "set_output_data(\"" << name << "\", 0, ";
      ctx.Emit_var(val);
      ctx << ")";
      ctx.End_stmt(node);
      ctx.Begin_stmt(node);
      ctx << "return True";
    } else {
      air::core::IR2C_HANDLER::template Handle_retv<RETV, VISITOR>(visitor,
                                                                   node);
    }
  }

  //! @brief Python line comment instead of C `// `.
  template <typename RETV, typename VISITOR>
  RETV Handle_comment(VISITOR* visitor, air::base::NODE_PTR node) {
    auto& ctx = visitor->Context();
    ctx << "# " << node->Comment() << "\n";
    return RETV();
  }

  //! @brief Python line comment instead of C `// pragma: `.
  template <typename RETV, typename VISITOR>
  RETV Handle_pragma(VISITOR* visitor, air::base::NODE_PTR node) {
    auto& ctx = visitor->Context();
    ctx << "# pragma: " << node->Pragma_id() << ", " << node->Pragma_arg0()
        << ", " << node->Pragma_arg1() << "\n";
    return RETV();
  }

  // -- Phase 2.1: observability / metadata ops ------------------------------
  // Mirror IR2C_CORE::Handle_validate/tm_start/tm_taken/dump_var
  // (ir2c_core.h:204-249) but Python-ized: lowercase runtime call names, no C
  // `&`/`;`. The kids are visited in the same order as the C path so the arg
  // layout matches C Validate/Tm_*/Dump_cipher_msg exactly; the rt_ref runtime
  // (rtlib_py.rt_ref) implements validate/tm_start/tm_taken/dump_cipher_msg.

  //! @brief CORE VALIDATE -> `validate(ciph, msg, len, eps)`. Mirrors
  //! IR2C_CORE::Handle_validate (4 kids: cipher result, message-op result,
  //! length, epsilon) without the C `&`/`;`.
  template <typename RETV, typename VISITOR>
  RETV Handle_validate(VISITOR* visitor, air::base::NODE_PTR node) {
    auto& ctx = visitor->Context();
    ctx << "validate(";
    visitor->template Visit<RETV>(node->Child(0));  // cipher result
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(1));  // message-op result
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(2));  // length
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(3));  // epsilon
    ctx << ")";
    return RETV();
  }

  //! @brief CORE TM_START -> `tm_start("label")`. Mirrors IR2C_CORE::Handle_tm
  //! _start: the node carries a STR_ARRAY constant label, emitted via
  //! Emit_constant_str_init as a valid Python double-quoted string. tm_start is
  //! not a constructable/dispatched CORE opcode today (see class doc), so this
  //! is a parity handler; rt_ref.tm_start is a no-op (plan decision #4).
  template <typename RETV, typename VISITOR>
  RETV Handle_tm_start(VISITOR* visitor, air::base::NODE_PTR node) {
    auto& ctx = visitor->Context();
    ctx << "tm_start(";
    ctx.Emit_constant_str_init(node->Const());
    ctx << ")";
    return RETV();
  }

  //! @brief CORE TM_TAKEN -> `tm_taken("label")`. Mirrors IR2C_CORE::Handle_tm
  //! _taken; rt_ref.tm_taken is a no-op (plan decision #4).
  template <typename RETV, typename VISITOR>
  RETV Handle_tm_taken(VISITOR* visitor, air::base::NODE_PTR node) {
    auto& ctx = visitor->Context();
    ctx << "tm_taken(";
    ctx.Emit_constant_str_init(node->Const());
    ctx << ")";
    return RETV();
  }

  //! @brief CORE DUMP_VAR -> `dump_cipher_msg(name, ciph, len)`. Mirrors
  //! IR2C_CORE::Handle_dump_var (3 kids: name str ldc, cipher, size) without
  //! the C `&`/`;`. dump_cipher_msg writes an observation to stderr in rt_ref
  //! (never raises; execution-correctness neutral).
  template <typename RETV, typename VISITOR>
  RETV Handle_dump_var(VISITOR* visitor, air::base::NODE_PTR node) {
    auto& ctx = visitor->Context();
    ctx << "dump_cipher_msg(";
    visitor->template Visit<RETV>(node->Child(0));  // name (str ldc -> "...")
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(1));  // cipher
    ctx << ", ";
    visitor->template Visit<RETV>(node->Child(2));  // size
    ctx << ")";
    return RETV();
  }

private:
  //! @brief True if `blk` is a BLOCK holding at least one non-comment,
  //! non-pragma statement. Python requires a suite (a real statement) after a
  //! `:` header; comments alone do not count, so a comment-only block is
  //! treated as empty.
  static bool Has_real_stmt(air::base::NODE_PTR blk) {
    if (blk == air::base::Null_ptr || !blk->Is_block()) return false;
    for (air::base::STMT_PTR s = blk->Begin_stmt(); s != blk->End_stmt();
         s = s->Next()) {
      air::base::NODE_PTR n = s->Node();
      if (!n->Is_comment() && !n->Is_pragma()) return true;
    }
    return false;
  }

  //! @brief Emit a Python suite for a BLOCK body (an `if`/`elif`/`else`/`for`
  //! body). Non-empty blocks are visited normally (Handle_block iterates the
  //! statements). An empty or comment/pragma-only block would leave the header
  //! with no statement -> IndentationError, so a `pass` is emitted (after any
  //! comments), indented one level beyond the header.
  template <typename RETV, typename VISITOR>
  void Emit_suite(VISITOR* visitor, air::base::NODE_PTR blk) {
    // Non-block body (a single statement, e.g. a raw DO_LOOP body built without
    // New_stmt_block): emit it at one indent level rather than silently dropping
    // it. In-scope front-ends always wrap bodies in a block, so this is
    // defensive -- symmetric with the while-fallback non-block branch.
    if (blk != air::base::Null_ptr && !blk->Is_block()) {
      auto& ctx = visitor->Context();
      ctx.Begin_block(blk);  // Level L -> L+1
      ctx.Begin_stmt(blk);
      visitor->template Visit<RETV>(blk);
      ctx.End_stmt(blk);
      ctx.End_block(blk);  // Level L+1 -> L
      return;
    }
    if (Has_real_stmt(blk)) {
      visitor->template Visit<RETV>(blk);  // Handle_block: Begin_block/iter/End_block
      return;
    }
    auto& ctx = visitor->Context();
    ctx.Begin_block(blk);  // Level L -> L+1
    if (blk != air::base::Null_ptr && blk->Is_block()) {
      // Emit any comment/pragma lines first (they carry their own trailing
      // newline; End_stmt is a no-op for them).
      for (air::base::STMT_PTR s = blk->Begin_stmt(); s != blk->End_stmt();
           s = s->Next()) {
        air::base::NODE_PTR n = s->Node();
        ctx.Begin_stmt(n);
        visitor->template Visit<RETV>(n);
        ctx.End_stmt(n);
      }
    }
    ctx.Begin_stmt(blk);
    ctx << "pass";
    ctx.End_stmt(blk);
    ctx.End_block(blk);  // Level L+1 -> L
  }

  //! @brief Emit an `if`/`elif` chain. `kw` is `"if "` for the head and
  //! `"elif "` for collapsed else-if. After the then-block (indent returns to
  //! the current level), `else:` / `elif ` is re-indented at that level.
  template <typename RETV, typename VISITOR>
  void Emit_if_chain(VISITOR* visitor, air::base::NODE_PTR node,
                     const char* kw) {
    auto& ctx = visitor->Context();
    ctx << kw;
    visitor->template Visit<RETV>(node->Child(0));  // condition
    ctx << ":" << std::endl;
    Emit_suite<RETV, VISITOR>(visitor, node->Child(1));  // then block (pass if empty)
    if (node->Num_child() > 2 &&
        node->Child(2) != air::base::Null_ptr) {
      air::base::NODE_PTR else_blk = node->Child(2);
      // Collapse `else: { if ... }` into `elif ...` when the else block's only
      // statement is an IF (plan rule 4).
      bool                 elif = false;
      air::base::NODE_PTR  inner = air::base::Null_ptr;
      if (else_blk->Is_block()) {
        air::base::STMT_PTR first = else_blk->Begin_stmt();
        if (first != else_blk->End_stmt() &&
            first->Next() == else_blk->End_stmt()) {
          air::base::NODE_PTR n = first->Node();
          if (n->Opcode() == air::core::OPC_IF) {
            elif  = true;
            inner = n;
          }
        }
      }
      ctx << std::string(ctx.Level() * 2, ' ');  // re-indent at current level
      if (elif) {
        Emit_if_chain<RETV, VISITOR>(visitor, inner, "elif ");
      } else {
        ctx << "else:" << std::endl;
        Emit_suite<RETV, VISITOR>(visitor, else_blk);  // pass if empty
      }
    }
  }
};

}  // namespace ckks
}  // namespace fhe

#endif  // FHE_CKKS_IR2PY_CORE_H