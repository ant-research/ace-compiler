# B.3 Call Graph: HSSA Analysis / Handler / Visitor + Builder / Func

## Overview

B.3 adds two subsystems to the HSSA framework:

1. **Build pipeline** — Converts AIR IR + SSA into HSSA IR (HEXPR/HSTMT in HCONTAINER) + CFG
2. **Emit pipeline** — Converts HSSA IR + CFG back into AIR IR

Both use a template-based visitor/handler pattern for opcode dispatch.

---

## 1. Build Pipeline

Transforms AIR nodes into HSSA representation (HEXPR, HSTMT, HPHI) and constructs the CFG.

```
HSSA_FUNC::Build(visitor)
│
├── SSA_BUILDER::Perform()                    // Build SSA (pre-existing)
│
├── HSSA_BUILDER::Run(visitor)
│   ├── visitor.Context().Init(cfg, hssa_cont, ssa_cont, cprop)
│   └── visitor.Visit<HEXPR_PTR>(entry_node)
│       │
│       ▼
│   HSSA_VISITOR::Visit(node)
│   ├── GUARD(_ctx, node)                     // Push node onto context stack
│   ├── Pre_visit_node(node)
│   │   ├── ctx.Pre_handle_expr(node)         // if HEXPR_PTR
│   │   └── ctx.Pre_handle_stmt(node)         // if HSTMT_PTR
│   │
│   ├── Forward<0>(domain, node)              // Domain-based handler dispatch
│   │   ├── HSSA_CORE_HANDLER::Handle_*(node) // If domain matches air::core
│   │   └── ctx.Handle_unknown_domain(node)   // Fallback
│   │
│   └── Post_visit_node(node)
│       ├── ctx.Post_handle_expr(node)
│       └── ctx.Post_handle_stmt(node)
│
└── CFG::Build_dom_info()                     // Build dominator tree
```

### HSSA_CORE_HANDLER Dispatch

Each `Handle_*` method processes one AIR opcode, creates HSSA nodes, and recurses via `visitor->Visit()`:

```
HSSA_CORE_HANDLER
│
├── Handle_func_entry(node)
│   ├── cfg.New_bb(BB_ENTRY)
│   ├── hssa_cont.New_entry_stmt(node)
│   ├── visitor->Visit(child[0..n-2])         // Formal parameters
│   ├── visitor->Visit(child[n-1])            // Function body block
│   └── cfg.New_bb(BB_EXIT)
│
├── Handle_st / Handle_stf / Handle_stp / Handle_stpf (node)
│   ├── hssa_cont.New_assign_stmt(node)
│   ├── visitor->Visit(child[0])              // RHS expression → HEXPR_PTR
│   ├── hssa_cont.Find_or_new_var_expr(ver)   // LHS variable
│   ├── ctx.Build_chi_list(stmt, chi_id)      // Attach CHI nodes
│   │   └── hssa_cont.New_chi(stmt)
│   │       ├── hssa_cont.Find_or_new_var_expr(result)
│   │       └── hssa_cont.Find_or_new_var_expr(opnd)
│   └── ctx.Append_stmt(stmt)
│
├── Handle_ld / Handle_ldf / Handle_ldp / Handle_ldpf (node)
│   └── hssa_cont.Find_or_new_var_expr(ver)   // Lookup/create VAR_DATA HEXPR
│
├── Handle_lda(node)
│   ├── ssa_cont.Node_mu(node)                // Walk MU list for SSA version
│   ├── hssa_cont.Find_or_new_var_expr(ver)   // Symbol expression
│   ├── OP_DATA::Alloc() + new OP_DATA(node)  // Create op with extra kid
│   └── hssa_cont.Find_or_new_expr(...)       // Hash-cons the expression
│
├── Handle_intconst / Handle_ldc / Handle_ldca (node)
│   ├── CST_DATA(node)                        // Create constant data
│   └── hssa_cont.Find_or_new_expr(...)       // Hash-cons
│
├── Handle_call(node)
│   ├── hssa_cont.New_call(node)
│   ├── hssa_cont.Find_or_new_var_expr(ver)   // Return value (if any)
│   ├── visitor->Visit(arg[i])                // Each argument → HEXPR_PTR
│   └── ctx.Append_stmt(stmt)
│
├── Handle_do_loop(node)
│   ├── cfg.New_bb(BB_LOOP_INIT / BB_LOOP_PHI / BB_LOOP_BODY / BB_COND / BB_LOOP_EXIT)
│   ├── Create_phis(ctx, phi_bb, node)
│   │   └── hssa_cont.New_phi(bb, size)
│   ├── visitor->Visit(child[0])              // Init expression
│   ├── hssa_cont.New_assign_stmt(init)       // Init statement
│   ├── Handle_phi_opnd(ctx, hphi, phi_id, 0) // PHI operand from init
│   ├── visitor->Visit(child[3])              // Loop body
│   ├── visitor->Visit(child[2])              // Step expression
│   ├── hssa_cont.New_assign_stmt(incr)       // Increment statement
│   ├── Handle_phi_opnd(ctx, hphi, phi_id, 1) // PHI operand from step
│   ├── Handle_phi_res(ctx, hphi, phi_id)     // PHI result
│   │   ├── hssa_cont.Find_or_new_var_expr(result)
│   │   └── expr->Set_flag(EF_DEF_BY_PHI)
│   ├── visitor->Visit(child[1])              // Condition expression
│   ├── hssa_cont.New_if(cond_node, cond)     // Condition statement
│   └── loop_info->Init(init, phi, body, cond, exit, ...)
│
└── Handle_if(node)
    ├── cfg.New_bb(BB_COND / BB_TRUE / BB_FALSE / BB_IF_PHI)
    ├── Create_phis(ctx, phi_bb, node)
    ├── visitor->Visit(child[0])              // Condition
    ├── hssa_cont.New_if(cond_node, cond)
    ├── visitor->Visit(child[1])              // True branch
    ├── Handle_phi_opnd(ctx, hphi, ..., 0)
    ├── visitor->Visit(child[2])              // False branch
    ├── Handle_phi_opnd(ctx, hphi, ..., 1)
    └── Handle_phi_res(ctx, hphi, ...)
```

### HSSA_BUILDER_CTX Fallback Handlers

When no domain-specific handler matches (or for block/generic nodes):

```
HSSA_BUILDER_CTX
│
├── Handle_block(node)                        // Statement block
│   ├── cfg.New_bb(BB_DEF, spos)
│   ├── cfg.Append_bb(bb)
│   ├── cfg.Connect_with_succ(cur_bb, bb)
│   ├── bb->Set_loop_info(cur_loop_info)      // If inside loop
│   └── visitor->Visit(stmt_child[i])         // Each statement in block
│
├── Handle_node(node)                         // Generic operator
│   ├── [root node] hssa_cont.New_op_stmt()   // → HSTMT with NARY_DATA
│   │   ├── visitor->Visit(child[i])          // Each operand
│   │   └── ctx.Append_stmt(stmt)
│   └── [non-root] OP_DATA::Alloc() + new     // → HEXPR with OP_DATA
│       ├── visitor->Visit(child[i])          // Each operand
│       └── hssa_cont.Find_or_new_expr(...)   // Hash-cons
│
└── Handle_unknown_domain(node)
    └── Handle_node(node)                     // Delegates to generic
```

---

## 2. Emit Pipeline

Converts HSSA IR + CFG back into AIR IR (CONTAINER nodes/statements).

```
HSSA_FUNC::Emit(glob)
│
├── glob->New_func_scope()                    // Create output function
├── output_fscope->Clone(*input_fscope)
├── Output_cont()->Clone_stmt(entry)          // Clone entry statement
├── Output_cont()->New_stmt_block(spos)       // Block for function body
│
└── BB_LIST::For_each(emit_bb)                // Traverse CFG in order
    │
    └── BB::Emit(hssa_func, cur_blk, visited)
        │
        ├── [BB_LOOP_INIT] → LOOP_INFO::Emit(hssa_func, blk, visited)
        │   ├── init_stmt->Emit(cont)         // Non-IV init statements
        │   ├── Init_stmt()->Rhs()->Emit(cont)    // Loop init value
        │   ├── Cond_expr()->Emit(cont)            // Loop condition
        │   ├── Incr_stmt()->Rhs()->Emit(cont)    // Loop increment
        │   ├── Loop_body()->Emit_loop_body(hssa_func, ..., visited)
        │   │   ├── BB::Emit(hssa_func, body_blk, visited)
        │   │   └── next_bb->Emit(...)        // Remaining body BBs
        │   ├── cont->New_do_loop(iv, init, cond, incr, body, spos)
        │   └── Exit()->Emit(hssa_func, blk, visited)
        │
        └── [other BB kinds] → HSTMT_LIST::For_each(emit_stmt)
            │
            └── HSTMT::Emit(cont)
                │
                ├── [SK_NARY]
                │   ├── cont->New_cust_stmt(opcode, spos)
                │   └── kid->Emit(cont)       // Each operand → HEXPR::Emit
                │
                ├── [SK_CALL]
                │   ├── cont->New_call(entry, retv, arg_cnt, spos)
                │   └── kid->Emit(cont)       // Each argument
                │
                └── [SK_ASSIGN]
                    ├── rhs->Emit(cont)        // RHS expression
                    └── lhs_var->Emit_lhs(cont, rhs_node, spos)
```

### Expression Emission

```
HEXPR::Emit(cont)
│
├── [EK_OP] → OP_DATA::Emit(cont, hssa_cont)
│   ├── [ARRAY + LDCA] cont->New_array(cont->New_ldca(...), dim, spos)
│   ├── [ARRAY + LDA]  cont->New_array(cont->New_lda(...), dim, spos)
│   │   └── cont->Set_array_idx(node, 0, kid[1]->Emit(cont))
│   └── [other]        cont->New_cust_node(opcode, rtype, spos)
│       └── kid[i]->Emit(cont)               // Recursive child emission
│
├── [EK_VAR] → VAR_DATA::Emit_rhs(cont, spos)
│   ├── [VK_ADDR_DATUM]
│   │   ├── cont->New_ld(datum, spos)         // No sub-field
│   │   └── cont->New_ldf(datum, field, spos) // With sub-field
│   └── [VK_PREG]
│       ├── cont->New_ldp(preg, spos)
│       └── cont->New_ldpf(preg, field, spos)
│
└── [EK_CONST] → CST_DATA::Emit(cont)
    ├── [CK_INT] cont->New_intconst(rtype, val, spos)
    └── [CK_ID]
        ├── [LDC]  cont->New_ldc(cst, spos)
        └── [LDCA] cont->New_ldca(cst, FLAT32, spos)
```

### LHS Emission (Store)

```
VAR_DATA::Emit_lhs(cont, rhs, spos)
│
├── [VK_ADDR_DATUM]
│   ├── cont->New_st(rhs, datum, spos)        // No sub-field
│   └── cont->New_stf(rhs, datum, field, spos) // With sub-field
└── [VK_PREG]
    ├── cont->New_stp(rhs, preg, spos)
    └── cont->New_stpf(rhs, preg, field, spos)
```

---

## 3. Use-Def Traversal

Walks use-def chains backwards from an expression to its definitions.

```
HSSA_UD_TRAV::Start(root_expr)
│
└── Visit(node: HEXPR_PTR)
    ├── GUARD(_ctx, node)                     // Push onto visiting stack
    ├── Pre_handle_expr(node)
    │
    ├── Forward<0>(domain, node)              // Handler dispatch
    │   └── ctx.Handle_expr(node)             // Fallback
    │       │
    │       ├── [EK_VAR] → Handle_var(node)
    │       │   ├── [def_by_stmt]
    │       │   │   └── Visit(def_stmt)       // → Handle_stmt
    │       │   │       └── [SK_ASSIGN] Visit(rhs_expr)
    │       │   │       └── [SK_NARY]  Visit(kid[i])
    │       │   │       └── [SK_CALL]  Visit(arg[i])
    │       │   │
    │       │   ├── [def_by_phi]
    │       │   │   └── Visit(phi.opnd[i])    // Each PHI operand (with cycle detection)
    │       │   │
    │       │   └── [def_by_chi]
    │       │       └── Visit(chi.stmt)       // Statement containing CHI
    │       │
    │       └── [EK_OP] → Handle_op(node)
    │           └── Visit(kid[i])             // Each operand
    │
    └── Post_handle_expr(node)
```

---

## 4. Analysis Visitor

General-purpose HSSA traversal for analysis passes. Walks CFG basic blocks and visits all statements/expressions.

```
HSSA_VISITOR::Trav(entry_bb)
│
├── Pre_handle_bb(bb)
├── HSTMT_LIST::For_each(trav_lambda)
│   └── Visit(stmt: HSTMT_PTR)
│       ├── GUARD(_ctx, stmt)
│       ├── Pre_handle_stmt(stmt)
│       │
│       ├── Forward<0>(domain, stmt)          // Handler dispatch
│       │   └── ctx.Handle_stmt(stmt)         // Fallback
│       │       ├── [SK_ASSIGN] Visit(rhs)
│       │       ├── [SK_NARY]   Visit(kid[i])
│       │       ├── [SK_CALL]   Visit(arg[i])
│       │       └── [SK_IF]     Visit(cond)
│       │
│       │       └── Visit(expr: HEXPR_PTR)
│       │           ├── GUARD(_ctx, expr)
│       │           ├── Pre_handle_expr(expr)
│       │           ├── ctx.Handle_expr(expr)
│       │           │   └── [EK_OP] Visit(kid[i])  // Recursive
│       │           └── Post_handle_expr(expr)
│       │
│       └── Post_handle_stmt(stmt)
│
├── Trav(succ_bb[i])                          // Recurse into successors
└── Post_handle_bb(bb)
```

---

## 5. Class Dependency Summary

```
                        ┌──────────────┐
                        │  HSSA_FUNC   │
                        │  Build/Emit  │
                        └──────┬───────┘
                 ┌─────────────┼─────────────┐
                 ▼             ▼             ▼
          ┌─────────────┐ ┌───────┐  ┌──────────────┐
          │HSSA_BUILDER │ │  CFG  │  │  HCONTAINER  │
          │   Run()     │ │       │  │              │
          └──────┬──────┘ └───────┘  └──────────────┘
                 │                          ▲
                 ▼                          │ (creates HEXPR/HSTMT/HPHI)
    ┌────────────────────────┐              │
    │  HSSA_VISITOR          │              │
    │  <CTX, HANDLERS...>   │──────────────┘
    │  Trav / Visit          │
    └───────┬────────────────┘
            │
     ┌──────┴──────┐
     ▼             ▼
┌──────────┐ ┌──────────────────┐
│ CONTEXT  │ │ HANDLERS (tuple) │
│          │ │                  │
│ Builder: │ │ HSSA_CORE_HANDLER│
│  CTX     │ │ or per-domain    │
│ Analyze: │ │ handlers         │
│  CTX     │ │                  │
│ UD_Trav: │ │ HSSA_DEFAULT_    │
│  CTX     │ │ HANDLER (fallback│
└──────────┘ └──────────────────┘

Emit pipeline (non-template, in hssa_func.cxx):

  HSSA_FUNC::Emit → BB::Emit → HSTMT::Emit → HEXPR::Emit
                     │                         ├── OP_DATA::Emit
                     │                         ├── VAR_DATA::Emit_rhs
                     │                         └── CST_DATA::Emit
                     │
                     └── LOOP_INFO::Emit → BB::Emit_loop_body
                                           └── BB::Emit (body BBs)
```

---

## 6. Files

| File | Role |
|------|------|
| `hssa_func.h` | HSSA_FUNC: entry points Build() and Emit() |
| `hssa_func.cxx` | Emit implementations for BB, LOOP_INFO, HEXPR, HSTMT, VAR_DATA, OP_DATA, CST_DATA |
| `hssa_builder.h` | HSSA_BUILDER: wires visitor to SSA/CFG and calls Visit() |
| `hssa_build_ctx.h` | HSSA_BUILDER_CTX: context for Build — Handle_block, Handle_node, Build_chi_list |
| `hssa_analyze_ctx.h` | HSSA_ANALYZE_CTX: context for analysis passes — Handle_expr, Handle_stmt, GUARD |
| `hssa_core_handler.h` | HSSA_CORE_HANDLER: air::core opcode handlers — st, ld, call, do_loop, if, etc. |
| `hssa_default_handler.h` | HSSA_DEFAULT_HANDLER: macro-generated fallback handlers |
| `hssa_visitor.h` | HSSA_VISITOR: template visitor — BB traversal, domain-based handler dispatch |
| `hssa_ud_trav.h` | HSSA_UD_TRAV: use-def chain traversal visitor |
| `hssa_ud_trav_ctx.h` | HSSA_UD_TRAV_CTX: context for UD traversal — Handle_var, Handle_op, cycle detection |
| `du_info.h` | DU_INFO / USE_INFO: def-use tracking (stub) |
