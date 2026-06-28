//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================
#include "fhe/opt/mdown_hoist_opt.h"

#include "air/opt/bb.h"
#include "air/opt/hssa_analyze_ctx.h"
#include "air/opt/hssa_container.h"
#include "air/opt/hssa_ud_trav.h"
#include "air/opt/hssa_ud_trav_ctx.h"
#include "air/util/mem_allocator.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/poly/config.h"
#include "fhe/poly/opcode.h"

using namespace air::base;
using namespace air::opt;
using fhe::poly::IR_AFTER_MD;
using fhe::poly::IR_BEFORE_MD;
using fhe::poly::MD_FLOW;
using fhe::poly::MD_STATS;

namespace fhe {
namespace poly {

class MD_WL_BUILDER_CTX : public HSSA_ANALYZE_CTX {
public:
  MD_WL_BUILDER_CTX(MDOWN_HOIST_OPT& opt_driver)
      : HSSA_ANALYZE_CTX(opt_driver.Cfg()), _opt_driver(opt_driver) {}

  template <typename RETV, typename VISITOR>
  RETV Pre_handle_bb(VISITOR* visitor, BB_PTR bb) {
    if (bb->Is_phi()) {
      auto prop_flag = [](HPHI_PTR phi, MD_WL_BUILDER_CTX* ctx) {
        MDOWN_FLAG f = NONE;
        for (uint32_t idx = 0; idx < phi->Size(); idx++) {
          HEXPR_PTR opnd    = phi->Opnd(idx);
          HPHI_PTR  def_phi = opnd->Def_phi();
          // disable flag prop among different phi bb
          if (!(def_phi != Null_ptr && def_phi->Bb_id() != phi->Bb_id())) {
            ctx->_opt_driver.Prop_flag(phi->Result(), phi->Opnd(idx));
          }
        }
      };
      HCONTAINER* cont = Cfg().Hssa_cont();
      HPHI_LIST   phi_list(cont, bb->Begin_phi_id());
      phi_list.For_each(prop_flag, this);
    }
  }

  template <typename RETV, typename VISITOR>
  RETV Post_handle_expr(VISITOR* visitor, HEXPR_PTR expr) {
    if (expr->Kind() == EK_OP) {
      OP_DATA_PTR op_expr    = expr->Cast_to_op_expr();
      uint32_t    kid_md_cnt = 0;
      MDOWN_FLAG  f          = NONE;
      for (size_t idx = 0; idx < op_expr->Kid_cnt(); idx++) {
        HEXPR_PTR kid = expr->Kid(idx);
        _opt_driver.Prop_flag(expr, kid);
        if (_opt_driver.Has_expr_flag(kid, IS_MDOWN) ||
            _opt_driver.Has_expr_flag(kid, KID_HAS_MDOWN)) {
          kid_md_cnt++;
        }
      }
      if (_opt_driver.Is_mdown_check_point(expr) &&
          kid_md_cnt == op_expr->Kid_cnt() &&
          !_opt_driver.Has_expr_flag(expr, IS_CAND) &&
          !_opt_driver.Has_expr_flag(expr, CANNOT_EXTEND)) {
        const NODE_INFO& parent = visitor->Parent(1);
        if (_opt_driver.Iteration() == 0 || Is_inplaced_add(expr, parent)) {
          _opt_driver.Set_expr_flag(expr, IS_CAND);
          _opt_driver.Add_cand(expr);
          _opt_driver.Add_cand_parent(parent);
        }
      }
    }

    if (Is_mdown(expr)) {
      _opt_driver.Set_expr_flag(expr, IS_MDOWN);
    }
    Set_cannot_extend(expr);
  }

  template <typename RETV, typename VISITOR>
  RETV Post_handle_stmt(VISITOR* visitor, HSTMT_PTR stmt) {
    if (stmt->Kind() == SK_ASSIGN) {
      HEXPR_PTR rhs = stmt->Rhs();
      _opt_driver.Prop_flag(stmt->Lhs(), rhs);
    }
  }

  bool Is_inplaced_add(HEXPR_PTR expr, const NODE_INFO& parent) {
    HSTMT_PTR par_stmt = parent.second;
    if (parent.first != Null_ptr) return false;
    if (par_stmt->Kind() != SK_ASSIGN) return false;
    HEXPR_PTR   lhs      = par_stmt->Lhs();
    OP_DATA_PTR add_expr = expr->Cast_to_op_expr();
    AIR_ASSERT(_opt_driver.Is_mdown_check_point(expr));
    for (uint32_t idx = 0; idx < add_expr->Kid_cnt(); idx++) {
      HEXPR_PTR kid = expr->Kid(idx);
      if (lhs->Match_lex(kid)) {
        return true;
      }
    }
    return false;
  }

  bool Is_linear(HEXPR_PTR expr) {
    if (expr->Kind() == EK_OP) {
      air::base::OPCODE opcode = expr->Opcode();
      if (opcode == air::base::OPCODE(POLYNOMIAL_DID, fhe::poly::OPCODE::ADD) ||
          opcode ==
              air::base::OPCODE(POLYNOMIAL_DID, fhe::poly::OPCODE::ROTATE) ||
          opcode == air::base::OPCODE(POLYNOMIAL_DID, fhe::poly::OPCODE::MUL) ||
          opcode ==
              air::base::OPCODE(POLYNOMIAL_DID, fhe::poly::OPCODE::MOD_DOWN)) {
        return true;
      }
      return false;
    }
    return true;
  }

  void Set_cannot_extend(HEXPR_PTR expr) {
    // if moddown used in precompute, mark kid0 canot extend
    if (Is_precomp(expr) &&
        (_opt_driver.Has_expr_flag(expr->Kid(0), KID_HAS_MDOWN) ||
         _opt_driver.Has_expr_flag(expr->Kid(0), IS_MDOWN))) {
      _opt_driver.Set_expr_flag(expr->Kid(0), CANNOT_EXTEND);
      return;
    }

    if (expr->Opcode() ==
        air::base::OPCODE(POLYNOMIAL_DID, fhe::poly::OPCODE::MUL)) {
      const uint32_t* is_mul_ciph =
          expr->Attr<uint32_t>(fhe::core::FHE_ATTR_KIND::MUL_CIPH);
      if (is_mul_ciph != nullptr && *is_mul_ciph != 0) {
        _opt_driver.Set_expr_flag(expr, CANNOT_EXTEND);
        return;
      }
    }

    if (expr->Opcode() == OPC_RESCALE &&
        (_opt_driver.Has_expr_flag(expr->Kid(0), KID_HAS_MDOWN) ||
         _opt_driver.Has_expr_flag(expr->Kid(0), IS_MDOWN))) {
      _opt_driver.Clear_expr_flag(expr, KID_HAS_MDOWN);
    }
  }

  bool Is_mdown(HEXPR_PTR expr) {
    if (expr->Opcode() ==
        air::base::OPCODE(POLYNOMIAL_DID, fhe::poly::OPCODE::MOD_DOWN)) {
      return true;
    }
    return false;
  }

  bool Is_precomp(HEXPR_PTR expr) {
    if (expr->Opcode() ==
        air::base::OPCODE(POLYNOMIAL_DID, fhe::poly::OPCODE::PRECOMP)) {
      return true;
    }
    return false;
  }

private:
  MDOWN_HOIST_OPT& _opt_driver;
};

class MD_COMMUTE_CTX : public HSSA_UD_TRAV_CTX {
public:
  MD_COMMUTE_CTX(MDOWN_HOIST_OPT& opt_driver, HEXPR_PTR expr)
      : HSSA_UD_TRAV_CTX(opt_driver.Cfg()),
        _opt_driver(opt_driver),
        _root_expr(expr),
        _root_kid_idx(-1) {}

  template <typename RETV, typename VISITOR>
  RETV Handle_expr(VISITOR* visitor, HEXPR_PTR node) {
    if (Is_stop()) return node;
    if (!_opt_driver._lower_ctx->Is_poly_type(node->Rtype_id()) &&
        !_opt_driver._lower_ctx->Is_plain_type(node->Rtype_id()))
      return node;
    if (node == _root_expr && _root_kid_idx != -1) {
      _opt_driver.Trace(MD_FLOW, "Root node already visited\n");
      return node;
    }
    if (_root_kid_idx < (int32_t)(_root_expr->Kid_cnt() - 1) &&
        node == _root_expr->Kid(_root_kid_idx + 1)) {
      _root_kid_idx++;
      // add empty candidate for recusion mod_down
      if (_opt_driver._worklist2.size() < _root_kid_idx) {
        _md_kids.push_back(Null_ptr);
      }
    }
    HEXPR_PTR retv = node;
    switch (node->Kind()) {
      case EK_VAR:
        retv = Handle_var<RETV>(visitor, node);
        break;
      case EK_OP:
        retv = Handle_op<RETV>(visitor, node);
        break;
      default:
        CMPLR_ASSERT(false, "node not handled");
    }
    if (node == _root_expr) {
      uint32_t idx = 0;
      // AIR_ASSERT(_md_kids.size() <= _root_expr->Kid_cnt());
      for (auto md : _md_kids) {
        HEXPR_PTR kid = _root_expr->Kid(idx);
        if (kid != Null_ptr) {
          _root_expr->Set_kid(idx, md);
          md->Set_kid(0, kid);
        }
        if (++idx >= _root_expr->Kid_cnt()) break;
      }
    }
    _opt_driver.Trace(MD_FLOW, "\nAfter commutation: expr", node->Id().Value(),
                      "\n");
    _opt_driver.Trace_obj(MD_FLOW, retv);
    return retv;
  }

  template <typename RETV, typename VISITOR>
  RETV Pre_handle_stmt(VISITOR* visitor, HSTMT_PTR stmt) {
    _opt_driver.Trace(MD_FLOW, "expr", stmt->Lhs()->Id().Value(),
                      "--def--> expr", stmt->Rhs()->Id().Value(), "\n");
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Post_handle_stmt(VISITOR* visitor, HSTMT_PTR stmt) {
    if (Is_stop()) return RETV();
    if (stmt->Kind() == SK_ASSIGN &&
        _opt_driver._lower_ctx->Is_poly_type(stmt->Rhs()->Rtype_id())) {
      HEXPR_PTR ext_var = _opt_driver.Get_ext_var_with_ver(stmt->Lhs());
      if ((_opt_driver.Is_modswitch(stmt->Rhs()) ||
           stmt->Rhs()->Opcode() == OPC_RESCALE) &&
          ext_var->Def_stmt() == Null_ptr) {
        // special handle for modswitch to make sure the level change for input
        // still valid for input = modswitch(input) gen: input =
        // modswitch(input) input.ext = extend(input)
        air::base::OPCODE opcode = OPC_EXTEND;
        TYPE_ID   poly_type      = _opt_driver._lower_ctx->Get_poly_type_id();
        HEXPR_PTR ext_op         = _opt_driver.Hssa_cont()->New_op_expr(
            opcode, poly_type, poly_type, stmt->Spos());
        ext_op->Set_kid(0, stmt->Lhs());
        HSTMT_PTR ext_assign =
            _opt_driver.Hssa_cont()->New_assign_stmt(ext_var, ext_op);
        _opt_driver.Cfg()
            .Bb_ptr(stmt->Bb_id())
            ->Insert_stmt_after(ext_assign, stmt);
        ext_var->Cast_to_var_expr()->Set_def_stmt(ext_assign->Id());
      } else if (stmt->Rhs() != _root_expr &&
                 stmt->Rhs()->Kid_idx(_root_expr) == -1) {
        // stmt is not root parent
        // replace lhs var with extend symbol
        // TODO: check DU, make sure ref cnt is only one
        // update new lhs's def stmt
        ext_var->Cast_to_var_expr()->Set_def_stmt(stmt->Id());
        stmt->Set_lhs(ext_var->Id());
        air::base::OPCODE opcode =
            air::base::OPCODE(air::core::CORE, air::core::OPCODE::ST);
        stmt->Set_opcode(opcode);
      }
    }
    _opt_driver.Trace(MD_FLOW, "After commutation stmt: \n");
    _opt_driver.Trace_obj(MD_FLOW, stmt);
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_var(VISITOR* visitor, HEXPR_PTR expr) {
    AIR_ASSERT(expr->Kind() == EK_VAR);
    HEXPR_PTR ret        = expr;
    HPHI_PTR  last_phi   = Null_ptr;
    bool      cross_loop = Is_cross_loop(expr, last_phi);
    if (cross_loop) {
      _opt_driver.Trace(MD_FLOW, "Cross loop, no need to follow phi UD\n");
    } else if (_opt_driver.Has_ext_var(expr)) {
      _opt_driver.Trace(MD_FLOW,
                        "Has extended var, no need to follow var UD\n");
    } else {
      HSSA_UD_TRAV_CTX::Handle_var<RETV, VISITOR>(visitor, expr);
    }

    if (_opt_driver.Need_extend(expr)) {
      if (expr->Cast_to_var_expr()->Def_by_phi()) {
        if (cross_loop) {  // && !expr->Is_extend()
          HEXPR_PTR ext_var = _opt_driver.Get_ext_var_with_ver(expr);
          if (ext_var->Def_stmt() == Null_ptr &&
              ext_var->Def_phi() == Null_ptr) {  // &&

            // last_phi->Opnd_idx(expr) == -1) {
            air::base::OPCODE opcode =
                air::base::OPCODE(POLYNOMIAL_DID, fhe::poly::OPCODE::EXTEND);
            TYPE_ID   poly_type = _opt_driver._lower_ctx->Get_poly_type_id();
            HEXPR_PTR ext_op    = _opt_driver.Hssa_cont()->New_op_expr(
                opcode, poly_type, poly_type, expr->Spos());
            ext_op->Set_kid(0, expr);
            HSTMT_PTR ext_assign =
                _opt_driver.Hssa_cont()->New_assign_stmt(ext_var, ext_op);
            // find extend insertion place
            // insert to last_phi's init bb
            BB_PTR        def_bb    = expr->Def_phi()->Bb(&(_opt_driver.Cfg()));
            BB_PTR        phi_bb    = last_phi->Bb(&(_opt_driver.Cfg()));
            LOOP_INFO_PTR loop_info = phi_bb->Loop_info();
            AIR_ASSERT(loop_info != Null_ptr);
            // if the crossing loop is inner loop, prepend stmt at loop exit
            if (def_bb->Loop_info()->Parent_id() == loop_info->Id()) {
              def_bb->Loop_info()->Exit()->Prepend_stmt(ext_assign);
              _opt_driver.Trace(MD_FLOW, "Gen extend stmt in BB",
                                def_bb->Loop_info()->Exit()->Id().Value(),
                                "\n");
            } else {
              loop_info->Init()->Append_stmt(ext_assign);
              _opt_driver.Trace(MD_FLOW, "Gen extend stmt in BB",
                                loop_info->Init()->Id().Value(), "\n");
            }
            ext_var->Cast_to_var_expr()->Set_def_stmt(ext_assign->Id());
          }
        }
        ret = Handle_phi<RETV>(visitor, expr);
      } else if (expr->Def_stmt() == Null_ptr) {
        // expr has no def, ex:input variable with zero version,
        // create a extend op to make the change
        air::base::OPCODE opcode =
            air::base::OPCODE(POLYNOMIAL_DID, fhe::poly::OPCODE::EXTEND);
        TYPE_ID   poly_type = _opt_driver._lower_ctx->Get_poly_type_id();
        HEXPR_PTR ext_op    = _opt_driver.Hssa_cont()->New_op_expr(
            opcode, poly_type, poly_type, expr->Spos());
        ext_op->Set_kid(0, expr);
        ret = ext_op;

        _opt_driver.Trace(MD_FLOW, "Gen extend op:");
        _opt_driver.Trace_obj(MD_FLOW, ext_op);
      } else if (expr->Cast_to_var_expr()->Def_by_chi()) {
        // Gen extend stmt and append after chi
        HSTMT_PTR def_stmt = expr->Def_stmt();
        HEXPR_PTR ext_var  = _opt_driver.Get_ext_var_with_ver(expr);
        // if ext_var already generated extend statement, no need to gen extend
        // assign
        if (ext_var->Def_stmt() == Null_ptr) {
          air::base::OPCODE opcode =
              air::base::OPCODE(POLYNOMIAL_DID, fhe::poly::OPCODE::EXTEND);
          TYPE_ID   poly_type = _opt_driver._lower_ctx->Get_poly_type_id();
          HEXPR_PTR ext_op    = _opt_driver.Hssa_cont()->New_op_expr(
              opcode, poly_type, poly_type, expr->Spos());
          ext_op->Set_kid(0, expr);
          HSTMT_PTR ext_assign =
              _opt_driver.Hssa_cont()->New_assign_stmt(ext_var, ext_op);
          _opt_driver.Cfg()
              .Bb_ptr(def_stmt->Bb_id())
              ->Insert_stmt_after(ext_assign, def_stmt);

          _opt_driver.Trace(MD_FLOW, "Gen extend stmt after chi\n");
          _opt_driver.Trace_obj(MD_FLOW, ext_assign);
          ext_var->Cast_to_var_expr()->Set_def_stmt(ext_assign->Id());
        }
        ret = ext_var;
      } else {
        // expr has def, follow UD will adjust def stmt to extended assign
        HEXPR_PTR ext_var = _opt_driver.Get_ext_var_with_ver(expr);
        _opt_driver._ext_expr_map[expr->Id().Value()] = ext_var;
        _opt_driver.Trace(MD_FLOW, "New/Find ext preg for expr",
                          expr->Id().Value(), "->");
        _opt_driver.Trace_obj(MD_FLOW, ext_var);
        ret = ext_var;
        _opt_driver.Du_info().Add_use(ext_var, visitor->Parent_stmt());
        _opt_driver.Du_info().Remove_use(expr, visitor->Parent_stmt());
      }
    }

    return ret;
  }

  bool Is_cross_loop(HEXPR_PTR expr, HPHI_PTR& last_phi) {
    if (expr->Kind() != EK_VAR) return false;
    HPHI_PTR phi = expr->Def_phi();
    if (phi == Null_ptr) return false;

    BB_PTR       bb        = _opt_driver.Cfg().Bb_ptr(phi->Bb_id());
    LOOP_INFO_ID loop_info = bb->Loop_info_id();
    if (loop_info == LOOP_INFO_ID()) return false;

    std::set<HPHI_ID>& visited_phi = Visited_phi();
    for (auto phi : visited_phi) {
      HPHI_PTR phi_ptr = _opt_driver.Hssa_cont()->Phi_ptr(phi);
      BB_PTR   bb      = _opt_driver.Cfg().Bb_ptr(phi_ptr->Bb_id());
      if (bb->Loop_info_id() != LOOP_INFO_ID()) {
        if (loop_info != bb->Loop_info_id()) {
          last_phi = phi_ptr;
          return true;
        }
      }
    }
    return false;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_phi(VISITOR* visitor, HEXPR_PTR expr) {
    HPHI_PTR phi = expr->Def_phi();
    AIR_ASSERT(phi != Null_ptr);
    _opt_driver.Trace(MD_FLOW, "!!!Enter phi bb", phi->Bb_id().Value(), "\n");
    HPHI_PTR ext_phi = _opt_driver.Find_ext_phi(phi);
    if (ext_phi == Null_ptr) {
      ext_phi           = _opt_driver.New_ext_phi(phi);
      HEXPR_PTR ext_res = _opt_driver.Get_ext_var_with_ver(phi->Result());
      ext_phi->Set_result(ext_res->Id());
    }
    for (uint32_t idx = 0; idx < phi->Size(); idx++) {
      HEXPR_PTR opnd     = phi->Opnd(idx);
      HEXPR_PTR ext_opnd = _opt_driver.Get_ext_var_with_ver(opnd);
      ext_phi->Set_opnd(idx, ext_opnd->Id());
    }

    HEXPR_PTR ext_expr = _opt_driver.Get_ext_var_with_ver(expr);
    ext_expr->Set_defphi(ext_phi);
    _opt_driver.Trace(MD_FLOW, "Create hphi:\n");
    _opt_driver.Trace_obj(MD_FLOW, phi);
    _opt_driver.Trace(MD_FLOW, "\n");
    _opt_driver.Trace_obj(MD_FLOW, ext_phi);

    _opt_driver.Du_info().Add_use(ext_expr, ext_phi);
    _opt_driver.Du_info().Remove_use(expr, phi);
    return ext_expr;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_op(VISITOR* visitor, HEXPR_PTR expr) {
    AIR_ASSERT(expr->Kind() == EK_OP);
    HEXPR_PTR retv = expr;

    TYPE_ID poly_type = _opt_driver._lower_ctx->Get_poly_type_id();
    if (_opt_driver.Has_expr_flag(expr, IS_MDOWN)) {
      // link MOD_DOWN kid0 to its parent
      HEXPR_PTR par_expr = visitor->Parent(1);
      AIR_ASSERT(par_expr != Null_ptr);
      if (par_expr->Kind() == EK_OP) {
        uint32_t kid_idx = par_expr->Kid_idx(expr);
        par_expr->Set_kid(kid_idx, expr->Kid(0));
      } else if (par_expr->Kind() == EK_VAR) {
        HSTMT_PTR par_stmt = par_expr->Def_stmt();
        par_stmt->Set_rhs(expr->Kid(0)->Id());
      } else {
        AIR_ASSERT_MSG(false, "unexpected op kind");
      }

      retv = expr->Kid(0);
      _md_kids.push_back(expr);
    } else if (_opt_driver.Is_modswitch(expr)) {
      // skip extend for modswitch
      retv = expr;
    } else if (expr->Opcode() == OPC_RESCALE) {
      // skip ud for rescale
      retv = expr;
    } else {
      OP_DATA_PTR op_expr = expr->Cast_to_op_expr();
      for (size_t idx = 0; idx < op_expr->Kid_cnt(); idx++) {
        HEXPR_PTR kid = expr->Kid(idx);
        _opt_driver.Trace(MD_FLOW, "expr", expr->Id().Value(), "--");
        // expr->Print_opcode(expr->Opcode(), std::cout);
        _opt_driver.Trace(MD_FLOW, "-->expr", kid->Id().Value(), "\n");
        retv->Set_kid(idx, visitor->template Visit<RETV>(kid));
      }
      _opt_driver.Mark_extend(retv);
    }
    return retv;
  }

private:
  MDOWN_HOIST_OPT&       _opt_driver;
  HEXPR_PTR              _root_expr;
  int32_t                _root_kid_idx;
  std::vector<HEXPR_PTR> _md_kids;
};

void MDOWN_HOIST_OPT::Run(HSSA_FUNC* func) {
  Set_cur_func(func);
  Trace(IR_BEFORE_MD, "\n=========XIR before MODDOWN======\n");
  Trace_obj(IR_BEFORE_MD, &Cfg());
  // Step1: Create worklist: candidate selection
  Create_worklist();
  if (_worklist2.size() > 0) {
    Hssa_cont()->Build_du_info(&Du_info());
  }

  uint32_t idx = 0;
  for (auto item : _worklist2) {
    // if (idx < 1)
    {
      Trace(MD_FLOW, "Processing candidate ", idx, ":\n");
      if (Has_ext_attr(item)) {
        Trace(MD_FLOW, "Already processed, skipping\n");
        idx++;
        continue;
      }
      // Step2: Operator Commutation
      Mdown_commutation(item);
      // Step3: Moddown Factoring
      Mdown_factor(item, _cand_parent[idx]);
      // Step4: Moddown Loop sinking
      Mdown_sinking(item, _cand_parent[idx]);
    }
    idx++;
  }
  Trace(MD_STATS, "Statictics: Moddown IR reduce cnt:", _factor_cnt,
        " Sink cnt:", _sink_cnt);
  Trace(IR_AFTER_MD, "\n=========XIR before MODDOWN======\n");
  Trace_obj(IR_AFTER_MD, &Cfg());
}

bool MDOWN_HOIST_OPT::Has_ext_attr(HEXPR_PTR expr) {
  bool            is_ext = false;
  const uint32_t* is_ext_ptr =
      expr->Attr<uint32_t>(fhe::core::FHE_ATTR_KIND::EXTENDED);
  if (is_ext_ptr != nullptr && *is_ext_ptr != 0) {
    is_ext = true;
  }
  return is_ext;
}

bool MDOWN_CAND::Is_valid() {
  size_t                       ok_cnt  = 0;
  std::vector<KID_DEF_STATUS>& kid_sts = Status();
  for (size_t idx = 0; idx < kid_sts.size(); idx++) {
    if (kid_sts[idx] == DEF_BY_MDOWN || kid_sts[idx] == DEF_BY_RECUR) {
      ok_cnt++;
    }
  }
  if (ok_cnt == kid_sts.size()) {
    return true;
  } else {
    return false;
  }
}

void MDOWN_HOIST_OPT::Add_cand(MDOWN_CAND& cand) {
  air::util::CXX_MEM_ALLOCATOR<MDOWN_CAND, MDOWN_OPT_MEM_POOL> alloc(&_mpool);
  MDOWN_CAND* new_cand = alloc.Allocate(cand);
  _worklist.push_back(new_cand);
}

void MDOWN_HOIST_OPT::Add_cand(air::opt::HEXPR_PTR expr) {
  _worklist2.push_back(expr);
}

void MDOWN_HOIST_OPT::Add_cand_parent(const NODE_INFO& node_info) {
  _cand_parent.push_back(node_info);
}

void MDOWN_HOIST_OPT::Create_worklist() {
  CFG&              cfg = Cfg();
  MD_WL_BUILDER_CTX ctx(*this);
  do {
    Set_flag_changed(false);
    HSSA_VISITOR<MD_WL_BUILDER_CTX> wk_builder(ctx, TOR_DOM);  // TOR_PDOM
    wk_builder.Trav<void>(cfg.Entry_bb());
    _iteration++;
  } while (Is_flag_changed() && Iteration() < MAX_ITERATION);

  // reorder candidates
  for (uint32_t i = 0; i < _worklist2.size(); i++) {
    bool swapped = false;
    for (uint32_t j = 0; j < _worklist2.size() - i - 1; j++) {
      HSTMT_PTR stmt1    = _cand_parent[j].second;
      HSTMT_PTR stmt2    = _cand_parent[j + 1].second;
      BB_ID     bb_id1   = stmt1->Bb_id();
      BB_ID     bb_id2   = stmt2->Bb_id();
      int32_t   dom_idx1 = Cfg().Dom_info().Get_dom_tree_pre_idx(bb_id1);
      int32_t   dom_idx2 = Cfg().Dom_info().Get_dom_tree_pre_idx(bb_id2);
      // in same BB, follow reverse stmt order
      // in different BB, follow dom order
      if (dom_idx1 > dom_idx2 || (bb_id1.Value() == bb_id2.Value() &&
                                  stmt1->Id().Value() < stmt2->Id().Value())) {
        HEXPR_PTR tmp     = _worklist2[j];
        _worklist2[j]     = _worklist2[j + 1];
        _worklist2[j + 1] = tmp;

        NODE_INFO tmp_info  = _cand_parent[j];
        _cand_parent[j]     = _cand_parent[j + 1];
        _cand_parent[j + 1] = tmp_info;
        swapped             = true;
      }
    }
  }
  if (_config.Is_trace(MD_FLOW)) {
    Print_worklist(_driver_ctx->Tstream());
  }
}

void MDOWN_HOIST_OPT::Mdown_commutation(HEXPR_PTR cand) {
  MD_COMMUTE_CTX               ctx(*this, cand);
  HSSA_UD_TRAV<MD_COMMUTE_CTX> commutor(ctx);
  commutor.Start<HEXPR_PTR>(cand);
#if 0
  // processing new phis
  std::map<uint32_t, air::opt::HPHI_PTR>::iterator it;
  air::base::OPCODE                                opcode =
      air::base::OPCODE(POLYNOMIAL_DID, fhe::poly::OPCODE::EXTEND);
  TYPE_ID poly_type = _lower_ctx->Get_poly_type_id();
  for (it = _ext_phi_map.begin(); it != _ext_phi_map.end(); it++) {
    HPHI_PTR new_phi = it->second;
    HPHI_PTR old_phi = Hssa_cont()->Phi_ptr(HPHI_ID(it->first));
    HEXPR_PTR  old_res = Hssa_cont()->Expr_ptr(old_phi->Result_id());
    HEXPR_PTR  new_res = Hssa_cont()->Expr_ptr(new_phi->Result_id());
    BB_PTR   phi_bb  = Cfg().Bb_ptr(new_phi->Bb_id());
    BB_PTR   idom    = Cfg().Bb_ptr(Cfg().Dom_info().Get_imm_dom(phi_bb));

    HEXPR_PTR ext_op =
        Hssa_cont()->New_op_expr(opcode, poly_type, poly_type, phi_bb->Spos());
    ext_op->Set_kid(0, old_res);
    HSTMT_PTR ext_assign = Hssa_cont()->New_assign_stmt(new_res, ext_op);
    idom->Append_stmt(ext_assign);
  }
#endif
}

void MDOWN_HOIST_OPT::Mark_extend(HEXPR_PTR op) {
  uint32_t is_extended = 1;
  op->Set_attr(fhe::core::FHE_ATTR_KIND::EXTENDED, &is_extended, 1);
  if (Is_encode(op)) {
    HEXPR_PTR      is_ext      = Hssa_cont()->New_cst_expr(1);
    const uint32_t ext_kid_idx = 4;
    op->Cast_to_op_expr()->Set_kid(ext_kid_idx, is_ext->Id());
  }
}

void MDOWN_HOIST_OPT::Extend_op(HEXPR_PTR op) {
  air::base::OPCODE opcode =
      air::base::OPCODE(POLYNOMIAL_DID, fhe::poly::OPCODE::EXTEND);
  TYPE_ID poly_type = _lower_ctx->Get_poly_type_id();
  if (op->Kind() == EK_OP) {
    for (uint32_t idx = 0; idx < op->Kid_cnt(); idx++) {
      HEXPR_PTR kid = op->Kid(idx);
      if (Need_extend(kid)) {
        HEXPR_PTR ext_op =
            Hssa_cont()->New_op_expr(opcode, poly_type, poly_type, op->Spos());
        ext_op->Set_kid(0, kid);
        op->Set_kid(idx, ext_op->Id());
      }
    }
    Mark_extend(op);
  } else if (op->Kind() == EK_VAR && Need_extend(op)) {
    HSTMT_PTR def_stmt = op->Def_stmt();
    AIR_ASSERT(def_stmt != Null_ptr);
    HEXPR_PTR ext_var = Hssa_cont()->New_preg_expr(op);
    def_stmt->Set_lhs(ext_var->Id());
    _ext_expr_map[op->Id().Value()] = ext_var;
  }
}

bool MDOWN_HOIST_OPT::Need_extend(HEXPR_PTR op) {
  if (!_lower_ctx->Is_poly_type(op->Rtype_id())) return false;
  if (op->Kind() == EK_VAR) {
    HSTMT_PTR def_stmt = op->Def_stmt();
    if (def_stmt != Null_ptr && Is_encode(def_stmt->Rhs())) {
      // plaintext defined by encode do not need to extend
      // encode with extended flag generate extended poly
      return false;
    } else {
      return true;
    }
  }

  return false;
}

void MDOWN_HOIST_OPT::Mdown_factor(HEXPR_PTR cand, NODE_INFO& cand_parent) {
  // step1 : remove modown from cand's kids
  for (uint32_t idx = 0; idx < cand->Kid_cnt(); idx++) {
    HEXPR_PTR kid = cand->Kid(idx);
    if (Is_mdown(kid)) {
      cand->Set_kid(idx, kid->Kid(0));
    }
  }

  air::base::OPCODE md_opcode =
      air::base::OPCODE(POLYNOMIAL_DID, fhe::poly::OPCODE::MOD_DOWN);
  TYPE_ID   poly_type = _lower_ctx->Get_poly_type_id();
  HEXPR_PTR new_md_op =
      Hssa_cont()->New_op_expr(md_opcode, poly_type, poly_type, cand->Spos());
  Set_expr_flag(new_md_op, IS_MDOWN);
  new_md_op->Set_kid(0, cand->Id());
  if (cand_parent.first == Null_ptr) {
    // candidate is under a statment
    HSTMT_PTR par_stmt   = cand_parent.second;
    HEXPR_PTR lhs        = par_stmt->Lhs();
    HEXPR_PTR ext_lhs    = Get_ext_var_with_ver(lhs);
    HSTMT_PTR ext_assign = Hssa_cont()->New_assign_stmt(ext_lhs, cand);
    new_md_op->Set_kid(0, ext_lhs->Id());
    par_stmt->Cast_to_assign()->Set_rhs(new_md_op->Id());
    Cfg().Bb_ptr(par_stmt->Bb_id())->Insert_stmt_before(ext_assign, par_stmt);
    lhs->Cast_to_var_expr()->Set_def_stmt(par_stmt->Id());
    Trace(MD_FLOW, "After factoring\n");
    Trace_obj(MD_FLOW, ext_assign);
    Trace_obj(MD_FLOW, par_stmt);
  } else {
    HEXPR_PTR par_expr = cand_parent.first;
    uint32_t  cand_idx = par_expr->Kid_idx(cand);
    par_expr->Set_kid(cand_idx, new_md_op->Id());
    Trace(MD_FLOW, "After factoring\n");
    Trace_obj(MD_FLOW, par_expr);
  }
  _factor_cnt++;
}

HEXPR_PTR MDOWN_HOIST_OPT::New_ext_var(HEXPR_PTR var_expr) {
  FUNC_SCOPE* fscope    = Hssa_cont()->Air_cont()->Parent_func_scope();
  TYPE_PTR    poly_type = _lower_ctx->Get_poly_type(&fscope->Glob_scope());
  std::string var_name  = var_expr->Cast_to_var_expr()->Name(Hssa_cont());
  std::string ext_str(var_name);
  ext_str.append(".ext");
  ADDR_DATUM_PTR ext_var =
      fscope->New_var(poly_type, ext_str.c_str(), var_expr->Spos());
  HEXPR_PTR ext_expr = Hssa_cont()->New_var_expr(ext_var);
  return ext_expr;
}

bool MDOWN_HOIST_OPT::Has_ext_var(HEXPR_PTR var_expr) {
  std::map<uint32_t, HEXPR_PTR>::iterator it =
      _ext_expr_map.find(var_expr->Id().Value());
  if (it != _ext_expr_map.end()) {
    return true;
  }
  return false;
}

HEXPR_PTR MDOWN_HOIST_OPT::Get_ext_var(HEXPR_PTR var_expr) {
  uint32_t var_id  = var_expr->Cast_to_var_expr()->Var_id();
  uint32_t sub_idx = var_expr->Cast_to_var_expr()->Sub_idx();
  uint32_t is_preg =
      var_expr->Cast_to_var_expr()->Var_kind() == air::opt::VAR_KIND::VK_PREG
          ? 1
          : 0;
  uint64_t key = (uint64_t)(((uint64_t)var_id << 32) + sub_idx << 1 + is_preg);
  std::map<uint64_t, HEXPR_PTR>::iterator it;
  it = _ext_sym_map.find(key);
  if (it == _ext_sym_map.end()) {
    HEXPR_PTR ext_var = New_ext_var(var_expr);
    _ext_sym_map[key] = ext_var;
    return ext_var;
  } else {
    return it->second;
  }
}

HEXPR_PTR MDOWN_HOIST_OPT::Get_ext_var_with_ver(HEXPR_PTR expr) {
  std::map<uint32_t, HEXPR_PTR>::iterator it =
      _ext_expr_map.find(expr->Id().Value());
  if (it != _ext_expr_map.end()) {
    return it->second;
  } else {
    HEXPR_PTR ext_var = Get_ext_var(expr);
    HEXPR_PTR ext_var_at_ver =
        Hssa_cont()->New_var_with_ver(ext_var, expr->Cast_to_var_expr()->Ver());
    _ext_expr_map[expr->Id().Value()] = ext_var_at_ver;
    return ext_var_at_ver;
  }
}

HPHI_PTR MDOWN_HOIST_OPT::Find_ext_phi(HPHI_PTR phi) {
  std::map<uint32_t, HPHI_PTR>::iterator it =
      _ext_phi_map.find(phi->Id().Value());
  if (it != _ext_phi_map.end()) {
    return it->second;
  } else {
    return Null_ptr;
  }
}

HPHI_PTR MDOWN_HOIST_OPT::New_ext_phi(HPHI_PTR phi) {
  AIR_ASSERT(_ext_phi_map.find(phi->Id().Value()) == _ext_phi_map.end());
  BB_PTR   bb      = Cfg().Bb_ptr(phi->Bb_id());
  HPHI_PTR ext_phi = Hssa_cont()->New_phi(bb, phi->Size());
  bb->Append_phi(ext_phi);
  _ext_phi_map[phi->Id().Value()] = ext_phi;
  return ext_phi;
}

bool MDOWN_HOIST_OPT::Is_mdown(HEXPR_PTR expr) {
  if (expr->Opcode() == air::base::OPCODE(fhe::poly::POLYNOMIAL_DID,
                                          fhe::poly::OPCODE::MOD_DOWN)) {
    return true;
  }
  return false;
}

bool MDOWN_HOIST_OPT::Is_encode(HEXPR_PTR expr) {
  if (expr->Opcode() == air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                          fhe::ckks::CKKS_OPERATOR::ENCODE)) {
    return true;
  }
  return false;
}

bool MDOWN_HOIST_OPT::Is_modswitch(HEXPR_PTR expr) {
  if (expr->Opcode() ==
      air::base::OPCODE(POLYNOMIAL_DID, fhe::poly::OPCODE::MODSWITCH)) {
    return true;
  }
  return false;
}

bool MDOWN_HOIST_OPT::Is_mdown_check_point(HEXPR_PTR expr) {
  if (expr->Opcode() !=
          air::base::OPCODE(POLYNOMIAL_DID, fhe::poly::OPCODE::ADD) &&
      expr->Opcode() !=
          air::base::OPCODE(POLYNOMIAL_DID, fhe::poly::OPCODE::SUB)) {
    return false;
  }
  return true;
}

MDOWN_FLAG MDOWN_HOIST_OPT::Get_expr_flag(air::opt::HEXPR_PTR expr) {
  std::map<uint32_t, MDOWN_FLAG>::iterator it;
  it = _expr_flags.find(expr->Id().Value());
  if (it != _expr_flags.end())
    return it->second;
  else
    return NONE;
}

void MDOWN_HOIST_OPT::Set_expr_flag(air::opt::HEXPR_PTR expr, MDOWN_FLAG f) {
  std::map<uint32_t, MDOWN_FLAG>::iterator it;
  it = _expr_flags.find(expr->Id().Value());
  if (it != _expr_flags.end()) {
    MDOWN_FLAG old_f                = it->second;
    _expr_flags[expr->Id().Value()] = (MDOWN_FLAG)(old_f | f);
  } else {
    _expr_flags[expr->Id().Value()] = f;
  }
}

void MDOWN_HOIST_OPT::Clear_expr_flag(air::opt::HEXPR_PTR expr, MDOWN_FLAG f) {
  std::map<uint32_t, MDOWN_FLAG>::iterator it;
  it = _expr_flags.find(expr->Id().Value());
  if (it != _expr_flags.end()) {
    MDOWN_FLAG old_f                = it->second;
    _expr_flags[expr->Id().Value()] = (MDOWN_FLAG)(old_f & (~f));
  }
}

bool MDOWN_HOIST_OPT::Has_expr_flag(air::opt::HEXPR_PTR expr, MDOWN_FLAG f) {
  std::map<uint32_t, MDOWN_FLAG>::iterator it;
  it = _expr_flags.find(expr->Id().Value());
  if (it != _expr_flags.end()) {
    MDOWN_FLAG exp_flag = it->second;
    return exp_flag & f;
  }
  return false;
}

void MDOWN_HOIST_OPT::Prop_flag(HEXPR_PTR expr1, HEXPR_PTR expr2) {
  if (!_lower_ctx->Is_poly_type(expr2->Rtype_id()) ||
      !_lower_ctx->Is_poly_type(expr1->Rtype_id()))
    return;
  MDOWN_FLAG old_f = Get_expr_flag(expr1);
  if (Has_expr_flag(expr2, IS_MDOWN) || Has_expr_flag(expr2, KID_HAS_MDOWN)) {
    Set_expr_flag(expr1, KID_HAS_MDOWN);
  }

  if (Has_expr_flag(expr2, CANNOT_EXTEND)) {
    Set_expr_flag(expr1, CANNOT_EXTEND);
  }
  if (old_f != Get_expr_flag(expr1)) {
    Set_flag_changed(true);
  }
}

void MDOWN_HOIST_OPT::Mdown_sinking(HEXPR_PTR cand, NODE_INFO& cand_parent) {
  Trace(MD_FLOW, "@@ Step 4: Sinking\n");
  if (cand_parent.first == Null_ptr) {
    HSTMT_PTR     par_stmt  = cand_parent.second;
    BB_PTR        bb        = Cfg().Bb_ptr(par_stmt->Bb_id());
    LOOP_INFO_PTR loop_info = bb->Loop_info();
    if (loop_info != Null_ptr) {
      if (bb->Kind() == BB_LOOP_BODY) {
        HEXPR_PTR lhs = par_stmt->Lhs();
        if (!Is_expr_used_in_loop(lhs, loop_info)) {
          BB_PTR exit_bb = loop_info->Exit();
          bb->Remove_stmt(par_stmt);
          exit_bb->Prepend_stmt(par_stmt);
          Trace(MD_FLOW, "  move stmt to loop exit: \n");
          Trace_obj(MD_FLOW, par_stmt);
          _sink_cnt++;
        }
      } else if (bb->Kind() == BB_LOOP_EXIT &&
                 loop_info->Parent() != Null_ptr) {
        LOOP_INFO_PTR parent = loop_info->Parent();
        HEXPR_PTR     lhs    = par_stmt->Lhs();
        if (!Is_expr_used_in_loop(lhs, parent)) {
          BB_PTR exit_bb = parent->Exit();
          bb->Remove_stmt(par_stmt);
          exit_bb->Prepend_stmt(par_stmt);
          Trace(MD_FLOW, "  move stmt to loop exit: \n");
          Trace_obj(MD_FLOW, par_stmt);
          _sink_cnt++;
        }
      }
    }
  }
}

bool MDOWN_HOIST_OPT::Is_expr_used_in_loop(HEXPR_PTR     expr,
                                           LOOP_INFO_PTR loop_info) {
  HCONTAINER* cont = Hssa_cont();
  USE_LIST*   uses = Du_info().Uses(expr);
  if (uses == nullptr) {
    CMPLR_ASSERT(false, "candidate for dce");
    return false;
  }
  bool is_used = false;
  for (uint32_t idx = 0; idx < uses->size() && !is_used; idx++) {
    USE_INFO& use = uses->at(idx);
    if (use.Is_stmt()) {
      HSTMT_PTR use_stmt = use.Stmt(cont);
      AIR_ASSERT(use_stmt != Null_ptr);
      BB_PTR use_bb = Cfg().Bb_ptr(use_stmt->Bb_id());
      // use bb is not in the same loop of cand statment
      if (use_bb->Loop_info_id() == loop_info->Id()) {
        is_used = true;
      }
    } else if (use.Is_phi()) {
      HPHI_PTR use_phi = use.Phi(cont);
      AIR_ASSERT(use_phi != Null_ptr);
      HEXPR_PTR phi_res = Hssa_cont()->Expr_ptr(use_phi->Result_id());
      BB_PTR    phi_bb  = Cfg().Bb_ptr(use_phi->Bb_id());
      if (Is_expr_used_in_loop(phi_res, phi_bb->Loop_info())) {
        is_used = true;
      }
    } else {
      AIR_ASSERT_MSG(false, "unexpected use");
    }
  }
  return is_used;
}

void MDOWN_HOIST_OPT::Print_worklist(std::ostream& os) const {
  os << "Flags after propagation: \n";
  for (auto it : _expr_flags) {
    os << "expr" << it.first << ": |";
    if (it.second & CANNOT_EXTEND) os << "cannot_extend|";
    if (it.second & IS_MDOWN) os << "mdown|";
    if (it.second & KID_HAS_MDOWN) os << "kid has mdown |";
    if (it.second & IS_CAND) os << "cand";
    os << "|\n";
  }
  os << "MDOWN candidates:\n";
  uint32_t idx = 0;
  for (auto item : _worklist2) {
    os << "[" << idx << "]: ";
    os << "expr[" << item->Id().Value() << "]\n";
    idx++;
  }
}

void MDOWN_CAND::Print(std::ostream& os) const {
  os << "root:" << Root()->Id().Value() << std::endl;
  uint32_t idx = 0;
  for (auto ud : Ud_chain()) {
    os << "  kid[", idx, "]:";
    for (auto expr : ud) {
      os << expr->Id().Value() << " ";
    }
    idx++;
    os << std::endl;
  }
}

}  // namespace poly
}  // namespace fhe
