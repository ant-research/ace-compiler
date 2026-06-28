//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/opt/hssa_func.h"

#include <stack>

#include "air/base/container.h"
#include "air/opt/bb.h"

using namespace air::base;
namespace air {
namespace opt {

void HSSA_FUNC::Emit(GLOB_SCOPE* glob) {
  AIR_ASSERT(glob != nullptr);
  CFG*        cfg           = Cfg_ptr();
  FUNC_SCOPE* output_fscope = &glob->New_func_scope(Input_fscope()->Id());
  output_fscope->Clone(*Input_fscope());
  Set_output_fscope(output_fscope);
  STMT_PTR entry     = Input_cont()->Entry_stmt();
  STMT_PTR new_entry = Output_cont()->Clone_stmt(entry);
  Output_fscope()->Set_entry_stmt(new_entry);
  NODE_PTR node     = entry->Node();
  NODE_PTR new_node = new_entry->Node();
  for (uint32_t i = 0; i < node->Num_child() - 1; ++i) {
    NODE_PTR n_child = Output_cont()->Clone_node(node->Child(i));
    new_node->Set_child(i, n_child);
  }
  // Set up block node for statements of the function
  NODE_PTR blk_node = Output_cont()->New_stmt_block(node->Spos());
  blk_node->Set_parent_stmt(new_entry);
  node->Set_child(node->Num_child() - 1, blk_node->Id());

  auto emit_bb = [](BB_PTR bb, HSSA_FUNC* hssa_func, NODE_PTR cur_blk,
                    BBID_SET& visited) {
    bb->Emit(hssa_func, cur_blk, visited);
    // std::cout << "Emit bb: " << bb->Id().Value() << std::endl;
  };

  BB_PTR   entry_bb = cfg->Entry_bb();
  BBID_SET visited;
  visited.insert(entry_bb->Id());
  BB_LIST bb_list(cfg, entry_bb->Id());
  bb_list.For_each(emit_bb, this, Output_cont()->Stmt_list().Block_node(),
                   visited);
}

NODE_PTR BB::Emit(HSSA_FUNC* hssa_func, NODE_PTR cur_blk, BBID_SET& visited) {
  if (visited.find(Id()) != visited.end()) {
    return Null_ptr;
  }
  visited.insert(Id());
  CONTAINER*  cont      = hssa_func->Output_cont();
  HCONTAINER* hssa_cont = hssa_func->Hssa_cont();
  if (Kind() == BB_LOOP_INIT) {
    Loop_info()->Emit(hssa_func, cur_blk, visited);
    return Null_ptr;
  } else if (Kind() == BB_COND && Loop_info() != Null_ptr) {
    // Skip for loop condition
    return Null_ptr;
  }

  auto emit_stmt = [](HSTMT_PTR hstmt, CONTAINER* cont, STMT_LIST& stmt_list) {
    STMT_PTR stmt = hstmt->Emit(cont);
    stmt_list.Append(stmt);
  };
  AIR_ASSERT(cur_blk != Null_ptr);
#if 0
  NODE_PTR   block = cont->New_stmt_block(Spos());
  STMT_LIST  stmt_list(block);
  HSTMT_LIST htmt_list(hssa_cont, Begin_stmt_id());
  htmt_list.For_each(emit_stmt, cont, stmt_list);
  if (cur_blk != Null_ptr) {
    STMT_LIST cur_stmt_list(cur_blk);
    cur_stmt_list.Append(block->Stmt());
  }
#else
  STMT_LIST  stmt_list(cur_blk);
  HSTMT_LIST htmt_list(hssa_cont, Begin_stmt_id());
  htmt_list.For_each(emit_stmt, cont, stmt_list);
#endif
  return cur_blk;
}

NODE_PTR BB::Emit_loop_body(HSSA_FUNC* hssa_func, NODE_PTR& cur_blk,
                            BBID_SET& visited) {
  NODE_PTR  body_blk = hssa_func->Output_cont()->New_stmt_block(Spos());
  STMT_LIST stmt_list(body_blk);
  Emit(hssa_func, body_blk, visited);

  BB_PTR next_bb = Next();
  while (next_bb != Null_ptr) {
    // std::cout << "BODY " << Id().Value() << "->" << next_bb->Id().Value()
    //          << std::endl;
    if (visited.find(next_bb->Id()) != visited.end()) {
      next_bb = next_bb->Next();
      continue;
    }
    if (next_bb->Kind() == BB_LOOP_EXIT &&
        next_bb->Loop_info() == Loop_info()) {
      break;
    }
    next_bb->Emit(hssa_func, body_blk, visited);
    next_bb = next_bb->Next();
  }
  return body_blk;
}

void LOOP_INFO::Emit(HSSA_FUNC* hssa_func, NODE_PTR blk, BBID_SET& visited) {
  AIR_ASSERT(visited.find(Loop_body()->Id()) == visited.end());
  AIR_ASSERT(visited.find(Cond()->Id()) == visited.end());
  CONTAINER*  cont      = hssa_func->Output_cont();
  HCONTAINER* hssa_cont = hssa_func->Hssa_cont();
  STMT_LIST   stmt_list(blk);
  BB_PTR      init_bb = Init();
  HSTMT_ID    init_id = init_bb->Begin_stmt_id();
  // emit init statements except loop induction variable init
  while (init_id != HSTMT_ID()) {
    HSTMT_PTR init_stmt = hssa_cont->Stmt_ptr(init_id);
    if (init_stmt != Init_stmt()) {
      STMT_PTR stmt = init_stmt->Emit(cont);
      stmt_list.Append(stmt);
    }
    init_id = init_stmt->Next_id();
  }
  NODE_PTR init_node = Init_stmt()->Rhs()->Emit(cont);
  NODE_PTR cond_node = Cond_expr()->Emit(cont);
  NODE_PTR incr_node = Incr_stmt()->Rhs()->Emit(cont);
  NODE_PTR null_node = Null_ptr;
  NODE_PTR body_node =
      Loop_body()->Emit_loop_body(hssa_func, null_node, visited);
  VAR_DATA_PTR iv = Ind_expr()->Cast_to_var_expr();
  AIR_ASSERT(iv->Var_kind() == VAR_KIND::VK_ADDR_DATUM);
  ADDR_DATUM_PTR iv_sym =
      cont->Parent_func_scope()->Addr_datum(ADDR_DATUM_ID(iv->Var_id()));
  STMT_PTR do_loop = cont->New_do_loop(iv_sym, init_node, cond_node, incr_node,
                                       body_node, init_node->Spos());
  stmt_list.Append(do_loop);
  Exit()->Emit(hssa_func, blk, visited);
  // stmt_list.Append(exit_node->Stmt());

  visited.insert(Init()->Id());
  visited.insert(Loop_body()->Id());
  visited.insert(Cond()->Id());
}

NODE_PTR OP_DATA::Emit(CONTAINER* cont, HCONTAINER* hssa_cont) const {
  TYPE_PTR rtype = cont->Glob_scope()->Type(Rtype());
  NODE_PTR node;
  // special handler for ARRAY
  if (Opcode() == air::core::ARRAY) {
    HEXPR_PTR kid0 = hssa_cont->Expr_ptr(Kid(0));
    if (kid0->Opcode() == air::core::LDCA) {
      NODE_PTR base = kid0->Emit(cont);
      node          = cont->New_array(base,
                                      1,  // base->Const()->Type()->Cast_to_arr()->Dim(),
                                      Spos());
    } else if (kid0->Opcode() == air::core::LDA) {
      HEXPR_PTR      addr_expr = kid0->Kid(0);
      uint32_t       addr_id   = addr_expr->Cast_to_var_expr()->Var_id();
      ADDR_DATUM_PTR addr_sym =
          cont->Parent_func_scope()->Addr_datum(ADDR_DATUM_ID(addr_id));
      node = cont->New_array(
          cont->New_lda(addr_sym, POINTER_KIND::FLAT32, Spos()), 1, Spos());
    }
    cont->Set_array_idx(node, 0, hssa_cont->Expr_ptr(Kid(1))->Emit(cont));
  } else {
    node = cont->New_cust_node(Opcode(), rtype, Spos());
    for (uint32_t idx = 0; idx < Kid_cnt(); idx++) {
      NODE_PTR child = hssa_cont->Expr_ptr(Kid(idx))->Emit(cont);
      node->Set_child(idx, child->Id());
    }
  }
  return node;
}

NODE_PTR CST_DATA::Emit(CONTAINER* cont) const {
  NODE_PTR ret   = Null_ptr;
  TYPE_PTR rtype = cont->Glob_scope()->Type(Rtype());
  switch (Cst_kind()) {
    case CK_INT:
      ret = cont->New_intconst(rtype, Cst_val(), Spos());
      break;
    case CK_ID: {
      CONSTANT_PTR cst = cont->Glob_scope()->Constant(Cst_id());
      if (Opcode() == air::core::LDC) {
        ret = cont->New_ldc(cst, Spos());
      } else if (Opcode() == air::core::LDCA) {
        ret = cont->New_ldca(cst, POINTER_KIND::FLAT32, Spos());
      } else {
        AIR_ASSERT_MSG(false, "Invalid opcode");
      }
    } break;
    default:
      AIR_ASSERT(false);
  }
  return ret;
}

NODE_PTR HEXPR::Emit(CONTAINER* cont) const {
  NODE_PTR ret = Null_ptr;
  switch (Kind()) {
    case EK_OP:
      ret = Cast_to_op_expr()->Emit(cont, Hssa_cont());
      break;
    case EK_VAR: {
      ret = Cast_to_var_expr()->Emit_rhs(cont, Spos());
      break;
    }
    case EK_CONST:
      ret = Cast_to_cst_expr()->Emit(cont);
      break;
    case EK_IVAR:
    case EK_RCONST:
    case EK_LDA:
    default:
      AIR_ASSERT(false);
  }
  AIR_ASSERT(ret != Null_ptr);
  // TODO: copy HEXPR attr to emitted NODE when Set_attr_id API is available
  return ret;
}

STMT_PTR VAR_DATA::Emit_lhs(CONTAINER* cont, NODE_PTR rhs,
                            const SPOS& spos) const {
  STMT_PTR ret = Null_ptr;
  if (Var_kind() == VAR_KIND::VK_ADDR_DATUM) {
    // TODO: var is global or local?
    ADDR_DATUM_PTR datum =
        cont->Parent_func_scope()->Addr_datum(ADDR_DATUM_ID(Var_id()));
    if (Sub_idx() == SSA_SYM::NO_INDEX) {
      ret = cont->New_st(rhs, datum, spos);
    } else {
      FIELD_ID fld_id(Sub_idx());
      ret = cont->New_stf(rhs, datum, cont->Glob_scope()->Field(fld_id), spos);
    }
  } else if (Var_kind() == VAR_KIND::VK_PREG) {
    PREG_PTR preg = cont->Parent_func_scope()->Preg(PREG_ID(Var_id()));
    if (Sub_idx() == SSA_SYM::NO_INDEX) {
      ret = cont->New_stp(rhs, preg, spos);
    } else {
      FIELD_ID fld_id(Sub_idx());
      ret = cont->New_stpf(rhs, preg, cont->Glob_scope()->Field(fld_id), spos);
    }
  } else {
    AIR_ASSERT(false);
  }
  return ret;
}

NODE_PTR VAR_DATA::Emit_rhs(CONTAINER* cont, const SPOS& spos) const {
  NODE_PTR ret = Null_ptr;
  if (Var_kind() == VAR_KIND::VK_ADDR_DATUM) {
    // TODO: var is global or local?
    ADDR_DATUM_PTR datum =
        cont->Parent_func_scope()->Addr_datum(ADDR_DATUM_ID(Var_id()));
    if (Sub_idx() == SSA_SYM::NO_INDEX) {
      ret = cont->New_ld(datum, spos);
    } else {
      FIELD_ID fld_id(Sub_idx());
      ret = cont->New_ldf(datum, cont->Glob_scope()->Field(fld_id), spos);
    }
  } else if (Var_kind() == VAR_KIND::VK_PREG) {
    PREG_PTR preg = cont->Parent_func_scope()->Preg(PREG_ID(Var_id()));
    if (Sub_idx() == SSA_SYM::NO_INDEX) {
      ret = cont->New_ldp(preg, spos);
    } else {
      FIELD_ID fld_id(Sub_idx());
      ret = cont->New_ldpf(preg, cont->Glob_scope()->Field(fld_id), spos);
    }
  } else {
    AIR_ASSERT(false);
  }
  return ret;
}

STMT_PTR HSTMT::Emit(CONTAINER* cont) const {
  STMT_PTR    ret       = Null_ptr;
  HCONTAINER* hssa_cont = Hssa_cont();
  switch (Kind()) {
    case SK_NARY: {
      NARY_DATA_PTR nary_data = Cast_to_nary();
      ret                     = cont->New_cust_stmt(Opcode(), Spos());
      for (uint32_t idx = 0; idx < nary_data->Kid_cnt(); idx++) {
        HEXPR_ID  kid_id = nary_data->Kid(idx);
        HEXPR_PTR kid    = _cont->Expr_ptr(kid_id);
        NODE_PTR  child  = kid->Emit(cont);
        ret->Node()->Set_child(idx, child);
      }
      // special handle for IST, need to abstract it in HSTMT data structure
      if (Opcode() == air::core::IST) {
        ret->Node()->Set_access_type(ret->Node()
                                         ->Child(0)
                                         ->Rtype()
                                         ->Cast_to_ptr()
                                         ->Domain_type()
                                         ->Base_type_id());
      }
      break;
    }
    case SK_CALL: {
      CALL_DATA_PTR call_data = Cast_to_call();
      ENTRY_PTR     entry = cont->Glob_scope()->Entry_point(call_data->Entry());
      PREG_PTR      retv  = air::base::Null_ptr;
      HEXPR_ID      retv_id = call_data->Retv();
      if (retv_id != HEXPR_ID()) {
        VAR_DATA_PTR retv_var =
            hssa_cont->Expr_ptr(retv_id)->Cast_to_var_expr();
        AIR_ASSERT(retv_var->Var_kind() == VAR_KIND::VK_PREG);
        retv = cont->Parent_func_scope()->Preg(PREG_ID(retv_var->Var_id()));
      }
      ret = cont->New_call(entry, retv, call_data->Arg_cnt(), Spos());
      for (uint32_t idx = 0; idx < call_data->Arg_cnt(); idx++) {
        HEXPR_ID  kid_id = call_data->Arg(idx);
        HEXPR_PTR kid    = _cont->Expr_ptr(kid_id);
        NODE_PTR  child  = kid->Emit(cont);
        ret->Node()->Set_arg(idx, child);
      }
      break;
    }
    case SK_ASSIGN: {
      ASSIGN_DATA_PTR assign_data = Cast_to_assign();
      HEXPR_PTR       rhs         = hssa_cont->Expr_ptr(assign_data->Rhs());
      NODE_PTR        rhs_node    = rhs->Emit(cont);

      HEXPR_PTR lhs = hssa_cont->Expr_ptr(assign_data->Lhs());
      AIR_ASSERT(lhs->Kind() == EK_VAR);
      VAR_DATA_PTR lhs_var = lhs->Cast_to_var_expr();
      ret                  = lhs_var->Emit_lhs(cont, rhs_node, Spos());
      break;
    }
    default:
      AIR_ASSERT_MSG(false, "HSTMT::Emit unsupported kind");
  }
  return ret;
}

}  // namespace opt
}  // namespace air
