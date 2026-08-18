//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================
#include "fhe/opt/op_fusion.h"

#include <stack>

#include "air/opt/bb.h"
#include "air/opt/hssa_analyze_ctx.h"
#include "air/opt/hssa_container.h"
#include "air/opt/hssa_ud_trav.h"
#include "air/opt/hssa_ud_trav_ctx.h"
#include "air/util/mem_allocator.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/poly/opcode.h"
using namespace air::base;
using namespace air::opt;
namespace fhe {
namespace poly {
class OP_FUSION_FILTER : public HSSA_ANALYZE_CTX {
public:
  OP_FUSION_FILTER(OP_FUSION_OPT& opt_driver)
      : HSSA_ANALYZE_CTX(opt_driver.Cfg()), _opt_driver(opt_driver) {}

  template <typename RETV, typename VISITOR>
  RETV Pre_handle_cr(VISITOR* visitor, HCR_PTR expr) {
    if (expr->Kind() == CK_OP) {
      for (auto cand : _opt_driver.Rules()) {
        if (expr->Opcode() == cand.Start_op()) {
          if (_opt_driver.Fuse_rule(cand, expr, visitor->Parent(1))) {
            // if rule matched, skip traverse
            break;
          }
        }
      }
    }
  }

private:
  OP_FUSION_OPT& _opt_driver;
};

class OP_FUSION_MATCHER : public HSSA_UD_TRAV_CTX {
public:
  OP_FUSION_MATCHER(OP_FUSION_OPT& opt_driver, OP_FUSION_RULE& rule,
                    HCR_PTR expr, NODE_INFO parent)
      : HSSA_UD_TRAV_CTX(opt_driver.Cfg()),
        _opt_driver(opt_driver),
        _root_expr(expr),
        _root_parent(parent),
        _rule(rule),
        _match_idx(0) {}

  template <typename RETV, typename VISITOR>
  RETV Handle_cr(VISITOR* visitor, HCR_PTR expr) {
    if (Is_stop()) return RETV();
    switch (expr->Kind()) {
      case CK_VAR:
        // var has no def, entry chi, match any
        if (expr->Cast_to_var_cr()->Def_by_none() &&
            _rule.Op(_match_idx) == air::core::OPC_INVALID) {
          // std::cout << "match any\n";
          NODE_INFO info(expr, Parent_stmt());
          _matched_expr.push(info);
          _match_idx++;
        }
        return Handle_var<RETV>(visitor, expr);
      case CK_OP:
        return Handle_op<RETV>(visitor, expr);
      default:
        ;
        // CMPLR_ASSERT(false, "expr not handled");
    }
    return RETV(expr);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_op(VISITOR* visitor, HCR_PTR expr) {
    AIR_ASSERT(expr->Kind() == CK_OP);
    OP_HCR_DATA_PTR op_cr = expr->Cast_to_op_cr();

    if (_match_idx < _rule.Op_cnt() &&
        (expr->Opcode() == _rule.Op(_match_idx))) {
      _match_idx++;
      HSTMT_PTR par_stmt = Parent_stmt();
      // TODO: add use site checking
      // skip first match, as there is not parent stmt in the UD chain
      if (_match_idx != 1 &&
          (par_stmt == Null_ptr ||
           par_stmt->Kind() != SK_ASSIGN)) {  // && par_stmt->Lhs()->Use)
        // std::cout << "Skip, parent is not assign or referenced by other node"
        // << std::endl;
        Set_stop(true);
        return RETV(expr);
      }
      HCR_PTR   prop_expr = _opt_driver.Hssa_cont()->New_cr(expr);
      NODE_INFO info(prop_expr, Parent_stmt());
      if (expr != _root_expr) {
        _matched_expr.push(info);
      }
      if (_match_idx == _rule.Op_cnt()) {
        return prop_expr;
      }
      for (size_t idx = 0; idx < op_cr->Kid_cnt(); idx++) {
        HCR_PTR kid =
            visitor->template Visit<RETV>(Hssa_cont().Cr_ptr(op_cr->Kid(idx)));
        prop_expr->Set_kid(idx, kid);
      }
      if (_match_idx == _rule.Op_cnt() && expr == _root_expr) {
        // std::cout << "Matched rule\n";
        // prop_expr->Print(std::cout);
        Apply_rule(prop_expr);
      }
      return prop_expr;
    } else if (_rule.Op(_match_idx) == air::core::OPC_INVALID) {
      // std::cout << "match any\n";
      NODE_INFO info(expr, Parent_stmt());
      _matched_expr.push(info);
      _match_idx++;
    } else {
      // std::cout << "not match, set stop\n";
      Set_stop(true);
      return RETV(expr);
    }
    return RETV(expr);
  }

  void Apply_rule(HCR_PTR prop_expr) {
    CFG*    cfg        = &(_opt_driver.Cfg());
    HCR_PTR fused_expr = Fuse_expr();
    if (_root_parent.first != Null_ptr) {
      // parent is expression
      HCR_PTR par_expr = _root_parent.first;
      int32_t kid_idx  = par_expr->Kid_idx(_root_expr);
      AIR_ASSERT(kid_idx != -1);
      par_expr->Set_kid(kid_idx, fused_expr);
    } else {
      // parent is statment
      HSTMT_PTR par_stmt = _root_parent.second;
      if (par_stmt->Kind() == SK_ASSIGN) {
        HSTMT_PTR new_assign = _opt_driver.Hssa_cont()->New_assign_stmt(
            par_stmt->Lhs(), fused_expr);
        // remove of the stmt may change filter traverse, better collect the
        // stmt and call the remove at the end of optimization phase
        par_stmt->Bb(cfg)->Insert_stmt_before(new_assign, par_stmt);
        _opt_driver.Add_dead_stmt(par_stmt);
        // std::cout << "Replace with fused stmt" << std::endl;
        // new_assign->Print();
      } else {
        CMPLR_ASSERT(false, "not supported parent");
      }
    }
    _opt_driver.Inc_opt_cnt();
    // std::cout << "\nFused expr:" << std::endl;
    // fused_expr->Print(std::cout);
  }

  HCR_PTR Fuse_expr() {
    AIR_ASSERT(_matched_expr.size() == _rule.Op_cnt() - 1);
    // special fuse rule for MULP_FAST
    if (_rule.Opcode() == OPC_MULP_FAST) {
      return Fuse_mulp_fast();
    }
#if 0
    if (_rule.Opcode() == OPC_MULADD_NOMOD) {
      return Fuse_dotprod();
    }
#endif
    HCR_PTR fused_expr = _opt_driver.Hssa_cont()->New_op_cr(
        _rule.Opcode(), _root_expr->Rtype_id(), _root_expr->Dsctype_id(),
        _root_expr->Spos());

    uint32_t fused_idx = 0;
    // std::cout << "\nmatched cnt: " << _matched_expr.size() << std::endl;
    uint32_t match_idx = _matched_expr.size() - 1;
    while (!_matched_expr.empty()) {
      NODE_INFO match_info = _matched_expr.top();
      HCR_PTR   expr       = match_info.first;
      HSTMT_PTR stmt       = match_info.second;
      _matched_expr.pop();
      // std::cout << "Processing matched Expr:\n";
      // expr->Print();
      if (_rule.Op(match_idx) != air::core::INVALID) {
        for (size_t idx = 0; idx < expr->Cast_to_op_cr()->Kid_cnt(); idx++) {
          if (_matched_expr.empty()) {
            fused_expr->Set_kid(fused_idx++, expr->Kid(idx));
          } else if (expr->Kid(idx) != _matched_expr.top().first) {
            fused_expr->Set_kid(fused_idx++, expr->Kid(idx));
          }
        }
      } else {
        // for any maching, do not expand expr kid to fused expression
        fused_expr->Set_kid(fused_idx++, expr);
      }
      if (stmt != Null_ptr) {
        AIR_ASSERT(stmt->Kind() == SK_ASSIGN);
        // stmt->Bb(cfg)->Remove_stmt(stmt);
        _opt_driver.Add_dead_stmt(stmt);
        // std::cout << "Mark stmt dead:" << std::endl;
        // stmt->Print();
      }
      match_idx--;
    }
    return fused_expr;
  }
#if 0
  HCR_PTR Fuse_dotprod() {
    NODE_INFO m1 = _matched_expr.top();
    HCR_PTR   m1_expr = m1.first;
    if (m1_expr->Opcode() == fhe::ckks::OPC_ADD) {
      if (m1_expr->Opnd(0) == m1_expr->Opnd(1)) {
        
      }
    }

    
  }
#endif
  HCR_PTR Fuse_mulp_fast() {
    HCR_PTR fused_expr = _opt_driver.Hssa_cont()->New_op_cr(
        _rule.Opcode(), _root_expr->Rtype_id(), _root_expr->Dsctype_id(),
        _root_expr->Spos());

    NODE_INFO match1 = _matched_expr.top();
    _matched_expr.pop();
    NODE_INFO match2      = _matched_expr.top();
    HCR_PTR   encode_expr = match2.first;
    HSTMT_PTR encode_stmt = match2.second;
    AIR_ASSERT(encode_expr->Opcode() == fhe::ckks::OPC_ENCODE);

    uint32_t prec = 1;
    encode_stmt->Rhs()->Set_attr(
        _opt_driver.Lower_ctx()->Attr_name(core::FHE_ATTR_KIND::PRECOMPUTE),
        &prec, 1);

    // gen precompute poly
    HCR_PTR prec_plain_var = _opt_driver.Prec_var(encode_stmt->Lhs());
    if (prec_plain_var == Null_ptr) {
      HCR_PTR prec_expr = _opt_driver.Hssa_cont()->New_op_cr(
          fhe::poly::OPC_PREC_PLAIN, encode_expr->Rtype_id(),
          encode_expr->Dsctype_id(), _root_expr->Spos());
      for (uint32_t idx = 0; idx < encode_expr->Cast_to_op_cr()->Kid_cnt();
           idx++) {
        prec_expr->Set_kid(idx, encode_expr->Kid(idx));
      }

      prec_plain_var = _opt_driver.Find_or_new_prec_var(encode_stmt->Lhs());

      HSTMT_PTR s_plain_prec =
          _opt_driver.Hssa_cont()->New_assign_stmt(prec_plain_var, prec_expr);
      encode_stmt->Bb(&(_opt_driver.Cfg()))
          ->Insert_stmt_after(s_plain_prec, encode_stmt);

      prec_expr->Set_attr(
          _opt_driver.Lower_ctx()->Attr_name(core::FHE_ATTR_KIND::PRECOMPUTE),
          &prec, 1);
    }

    ADDR_DATUM_PTR prec_sym =
        _opt_driver.Hssa_cont()->Air_cont()->Parent_func_scope()->Addr_datum(
            ADDR_DATUM_ID(prec_plain_var->Cast_to_var_cr()->Var_id()));
    HCR_PTR prec_poly_var = _opt_driver.Hssa_cont()->New_var_cr(
        prec_sym, _root_expr->Kid(1)->Cast_to_var_cr()->Sub_idx());

    fused_expr->Set_kid(0, _root_expr->Kid(0));
    fused_expr->Set_kid(1, _root_expr->Kid(1));
    fused_expr->Set_kid(2, prec_poly_var);
    return fused_expr;
  }

private:
  OP_FUSION_OPT&        _opt_driver;
  OP_FUSION_RULE&       _rule;
  HCR_PTR               _root_expr;
  NODE_INFO             _root_parent;
  uint32_t              _match_idx;
  std::stack<NODE_INFO> _matched_expr;
};

void OP_FUSION_OPT::Register_rules(uint32_t          priority,
                                   air::base::OPCODE fused_op, OPLIST& list) {
  OP_FUSION_RULE cand(priority, fused_op, list);
  auto           it = std::lower_bound(Rules().begin(), Rules().end(), cand,
                                       OP_FUSION_RULE::Less);
  Rules().insert(it, cand);
}

void OP_FUSION_OPT::Run(air::opt::HSSA_FUNC* func) {
  // std::cout << "============OP_FUSION_OPT for ";
  // std::cout << func->Input_fscope()->Owning_func()->Name()->Char_str();
  // std::cout << "=============\n";
  Set_cur_func(func);
  // func->Cfg().Print();

  CFG&                           cfg = Cfg();
  OP_FUSION_FILTER               ctx(*this);
  HSSA_VISITOR<OP_FUSION_FILTER> op_fuser(ctx, TOR_PRE);
  op_fuser.Trav<void>(cfg.Entry_bb());
  // std::cout << "Statistics for op fusion: " << Opt_cnt() << std::endl;

  Remove_dead_stmts();
}

void OP_FUSION_OPT::Remove_dead_stmts() {
  CFG* cfg = &(Cfg());
  for (auto dead_stmt : _dead_stmts) {
    AIR_ASSERT(dead_stmt->Is_dead());
    dead_stmt->Bb(cfg)->Remove_stmt(dead_stmt);
  }
}
bool OP_FUSION_OPT::Fuse_rule(OP_FUSION_RULE& cand, HCR_PTR expr,
                              const NODE_INFO& parent) {
  // std::cout << "Processing candidates:" << Cand_idx() << "\n";
  Inc_cand_idx();
  uint32_t                        opt_cnt = Opt_cnt();
  CFG&                            cfg     = Cfg();
  OP_FUSION_MATCHER               ctx(*this, cand, expr, parent);
  HSSA_UD_TRAV<OP_FUSION_MATCHER> matcher(ctx);
  matcher.Start<HCR_PTR>(expr);
  // if rule applied opt cnt will be increased, return true, otherwise return
  // false
  return opt_cnt == Opt_cnt() ? false : true;
}

bool OP_FUSION_RULE::Less(const OP_FUSION_RULE& c1, const OP_FUSION_RULE& c2) {
  if (c1.Priority() < c2.Priority()) return true;
  return false;
}

uint64_t OP_FUSION_OPT::Prec_key(HCR_PTR encode_var) {
  uint32_t var_id  = encode_var->Cast_to_var_cr()->Var_id();
  uint32_t sub_idx = encode_var->Cast_to_var_cr()->Sub_idx();
  uint32_t is_preg =
      encode_var->Cast_to_var_cr()->Var_kind() == air::opt::VAR_KIND::PREG ? 1
                                                                           : 0;
  uint64_t key = (uint64_t)(((uint64_t)var_id << 32) + sub_idx << 1 + is_preg);
  return key;
}
HCR_PTR OP_FUSION_OPT::Prec_var(HCR_PTR encode_var) {
  uint64_t                              key = Prec_key(encode_var);
  std::map<uint64_t, HCR_PTR>::iterator it;
  it = _prec_sym_map.find(key);
  if (it == _prec_sym_map.end()) {
    return Null_ptr;
  } else {
    return it->second;
  }
}

HCR_PTR OP_FUSION_OPT::New_prec_var(HCR_PTR encode_var) {
  FUNC_SCOPE* fscope     = Hssa_cont()->Air_cont()->Parent_func_scope();
  TYPE_PTR    plain_type = _lower_ctx->Get_plain_type(&fscope->Glob_scope());
  std::string var_name   = encode_var->Cast_to_var_cr()->Name(Hssa_cont());
  std::string prec_str(var_name);
  prec_str.append(".prec");
  ADDR_DATUM_PTR prec_var =
      fscope->New_var(plain_type, prec_str.c_str(), encode_var->Spos());
  HCR_PTR prec_expr = Hssa_cont()->New_var_cr(prec_var);
  return prec_expr;
}

HCR_PTR OP_FUSION_OPT::Find_or_new_prec_var(HCR_PTR encode_var) {
  HCR_PTR prec_var = Prec_var(encode_var);
  if (prec_var == Null_ptr) {
    HCR_PTR prec_var                    = New_prec_var(encode_var);
    _prec_sym_map[Prec_key(encode_var)] = prec_var;
    return prec_var;
  } else {
    return prec_var;
  }
}

}  // namespace poly
}  // namespace fhe