//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_CG_AIR2CGIR_CTX_H
#define AIR_CG_AIR2CGIR_CTX_H

#include <tuple>
#include <unordered_map>
#include <vector>

#include "air/base/analyze_ctx.h"
#include "air/cg/cgir_container.h"
#include "air/core/opcode.h"

namespace air {
namespace cg {

//! @brief Context for air2cgir pass used by VISITOR
class AIR2CGIR_CTX : public air::base::ANALYZE_CTX {
public:
  //! @brief Construct a new AIR2CGIR_CTX object with container for new CGIR
  //! @param cntr CGIR Container for new CGIR
  AIR2CGIR_CTX(CGIR_CONTAINER* cntr, bool while_do = false)
      : _cntr(cntr), _force_while_do(while_do) {}

  //! @brief Destruct AIR2CGIR_CTX object
  ~AIR2CGIR_CTX() { AIR_ASSERT(_cg_blk.empty()); }

  //! @brief Get container for new NODE
  CGIR_CONTAINER* Container() const { return _cntr; }

  //! @brief Prepend stmt in new container before current stmt
  void Prepend(INST_PTR inst) {
    AIR_ASSERT(!_cg_blk.empty());
    _cg_blk.back()->Append(inst);
  }

  //! @brief Append stmt in new container after current stmt
  void Append(INST_PTR inst) { _post_inst.push_back(inst); }

  //! @brief Begin translating a air BLOCK to cgir
  void Push(const air::base::NODE_PTR& air_blk, const BB_PTR& cg_blk) {
    ANALYZE_CTX::Push(air_blk);
    _cg_blk.push_back(cg_blk);
  }

  //! @brief End translating a air BLOCK to cgir
  void Pop(const air::base::NODE_PTR& air_blk, const BB_PTR& cg_blk) {
    ANALYZE_CTX::Pop(air_blk);
    AIR_ASSERT(!_cg_blk.empty() && _cg_blk.back() == cg_blk);
    _cg_blk.pop_back();
  }

  //! @brief get top CGIR BLOCK
  const BB_PTR Top_bb() const {
    AIR_ASSERT(!_cg_blk.empty());
    return _cg_blk.back();
  }

  //! @brief Begin translating a air STMT to cgir
  void Begin_stmt(const air::base::NODE_PTR& node) {
    AIR_ASSERT(!_cg_blk.empty());
    AIR_ASSERT(_post_inst.empty());
  }

  //! @brief End translating a air STMT to cgir
  void End_stmt(const air::base::NODE_PTR& node) {
    AIR_ASSERT(!_cg_blk.empty());
    if (!_post_inst.empty()) {
      BB_PTR bb = Top_bb();
      for (auto&& it = _post_inst.begin(); it != _post_inst.end(); ++it) {
        bb->Append(*it);
      }
      _post_inst.clear();
    }
  }

  //! @brief Default BLOCK handler to traverse original stmt list and
  //  construct new cgir inst list
  template <typename RETV, typename VISITOR>
  RETV Handle_block(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(air::base::ANALYZE_CTX::Top() == node);
    for (air::base::STMT_PTR stmt       = node->Begin_stmt();
         stmt != node->End_stmt(); stmt = stmt->Next()) {
      air::base::NODE_PTR s_node = stmt->Node();
      Begin_stmt(s_node);
      if (s_node->Opcode() == air::core::DO_LOOP) {
        Handle_do_loop<RETV>(visitor, s_node);
      } else if (s_node->Opcode() == air::core::IF) {
        Handle_if<RETV>(visitor, s_node);
      } else {
        visitor->template Visit<RETV>(s_node);
      }
      End_stmt(s_node);
    }
    // Pop(node, cg_node);
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_node(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT_MSG(false, "no default implementation for Handle_node: %s",
                   node->Name());
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_retv(VISITOR* visitor, base::NODE_PTR node) {
    return RETV();
  }
  template <typename RETV, typename VISITOR>
  RETV Handle_formal(VISITOR* visitor, base::NODE_PTR node) {
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_func_entry(VISITOR* visitor, air::base::NODE_PTR node) {
    air::base::NODE_PTR body_blk = node->Body_blk();
    Push(body_blk, _cntr->Entry());
    air::cg::BB_PTR first_bb = _cntr->New_bb();
    _cntr->Set_bb_order(_cntr->Entry(), first_bb);
    // connect entry with first_bb
    _cntr->Set_entry(first_bb);
    Push(body_blk, first_bb);

    // handle returned value and parameters
    visitor->Context().template Handle_retv<RETV>(visitor, node);
    visitor->Context().template Handle_formal<RETV>(visitor, node);
    // handle function body
    visitor->template Visit<RETV>(body_blk);
    air::cg::BB_PTR last_bb = Top_bb();
    // connect last bb with exit
    _cntr->Set_exit(last_bb);
    Pop(body_blk, last_bb);
    Pop(body_blk, _cntr->Entry());
    _cntr->Set_bb_order(last_bb, _cntr->Exit());
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_do_loop(VISITOR* visitor, air::base::NODE_PTR node) {
    // AIR IR:      CGIR CFG(while-do):          CGIR CFG(do-while):
    // DO_LOOP         init_bb (init)               init_bb (init)
    //   init              |                            |
    //   cmp         +- cmp_bb <-+ (cmp)            body_bb <-+ (body, incr)
    //   incr        |     |     |                      |     |
    //   BLOCK       | body_bb --+ (body, incr)      cmp_bb --+
    //     body      V                                  |
    //            tail_bb                             tail_bb

    bool while_do = _force_while_do || !Safe_do_while(node);

    // save current air and cgit top block
    air::base::NODE_PTR air_blk = air::base::ANALYZE_CTX::Top();
    air::cg::BB_PTR     init_bb = Top_bb();
    // handle init
    (void)visitor->Context().Get_preg(node->Iv());
    visitor->Context().template Handle_scalar_store<RETV>(visitor, node->Iv(),
                                                          node->Loop_init());
    Pop(air_blk, init_bb);
    // handle cmp
    air::cg::BB_PTR cmp_bb_begin;
    if (while_do) {
      // create a new comparison bb for while-do
      cmp_bb_begin = _cntr->New_bb();
      // set cmp_bb next to init_bb for text layout
      _cntr->Set_bb_order(init_bb, cmp_bb_begin);
    }

    // handle body
    air::cg::BB_PTR     body_bb_begin = _cntr->New_bb();
    air::base::NODE_PTR body_blk      = node->Body_blk();
    Push(body_blk, body_bb_begin);
    visitor->template Visit<RETV>(body_blk);
    // handle incr
    visitor->Context().template Handle_scalar_store<RETV>(visitor, node->Iv(),
                                                          node->Loop_incr());
    if (while_do) {
      // jump from incr to cmp
      visitor->Context().template Handle_jmp<RETV>(visitor, cmp_bb_begin);
      // connect body_bb_end to cmp_bb_begin
      _cntr->Connect(Top_bb(), cmp_bb_begin);
      // connect init_bb to cmp_bb_begin
      _cntr->Connect(init_bb, cmp_bb_begin);
      // set body_bb_begin next to cmp_bb for text layout
      _cntr->Set_bb_order(cmp_bb_begin, body_bb_begin);
    } else {
      // handle cmp and conditional jump to body
      visitor->Context().template Handle_cond_jmp<RETV, VISITOR, true>(
          visitor, node->Compare(), body_bb_begin);
      // connect body_bb_end to body_bb_begin
      _cntr->Connect(Top_bb(), body_bb_begin);
      // connect init_bb to body_bb_begin
      _cntr->Connect(init_bb, body_bb_begin);
      // set body_bb_begin next to init_bb for text layout
      _cntr->Set_bb_order(init_bb, body_bb_begin);
    }
    air::cg::BB_PTR body_bb_end = Top_bb();
    Pop(body_blk, body_bb_end);

    // create tail_bb
    air::cg::BB_PTR tail_bb = _cntr->New_bb();
    // handle cmp for while-do
    if (while_do) {
      Push(body_blk, cmp_bb_begin);
      visitor->Context().template Handle_cond_jmp<RETV, VISITOR, false>(
          visitor, node->Compare(), tail_bb);
      air::cg::BB_PTR cmp_bb_end = Top_bb();
      Pop(body_blk, cmp_bb_end);
      // connect cmp_bb_end to tail_bb
      _cntr->Connect(cmp_bb_end, tail_bb);
      // connect cmp_bb_end to body_bb_begin
      _cntr->Connect(cmp_bb_end, body_bb_begin);
    } else {
      // connect body_bb_end to tail_bb
      _cntr->Connect(body_bb_end, tail_bb);
    }
    // set tail bb next to body_bb_end
    _cntr->Set_bb_order(body_bb_end, tail_bb);
    // restore air and cgir block
    Push(air_blk, tail_bb);
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_if(VISITOR* visitor, air::base::NODE_PTR node) {
    // AIR IR:          CGIR CFG:
    // IF                    if_bb (cmp)
    //   cmp                 /    \
    //   BLOCK          then_bb else_bb
    //     then              \    /
    //   BLOCK              merge_bb
    //     else

    // save current air and cgit top block
    air::base::NODE_PTR air_blk = air::base::ANALYZE_CTX::Top();
    air::cg::BB_PTR     if_bb   = Top_bb();
    // handle compare
    visitor->template Visit<RETV>(node->Compare());
    Pop(air_blk, if_bb);
    // handle then
    air::cg::BB_PTR then_bb_begin = _cntr->New_bb();
    // set then_bb_begin next to if_bb
    _cntr->Set_bb_order(if_bb, then_bb_begin);
    air::base::NODE_PTR then_blk = node->Then_blk();
    Push(then_blk, then_bb_begin);
    visitor->template Visit<RETV>(then_blk);
    air::cg::BB_PTR then_bb_end = Top_bb();
    Pop(then_blk, then_bb_end);
    // handle else
    air::cg::BB_PTR else_bb_begin = _cntr->New_bb();
    // set else_bb_begin next to then_bb_end
    _cntr->Set_bb_order(then_bb_end, else_bb_begin);
    air::base::NODE_PTR else_blk = node->Else_blk();
    Push(else_blk, else_bb_begin);
    visitor->template Visit<RETV>(else_blk);
    air::cg::BB_PTR else_bb_end = Top_bb();
    Pop(else_blk, else_bb_end);
    // connect if to else/then block
    _cntr->Connect(if_bb, else_bb_begin);
    _cntr->Connect(if_bb, then_bb_begin);
    // connect else/then to merge block
    air::cg::BB_PTR merge_bb = _cntr->New_bb();
    _cntr->Connect(else_bb_end, merge_bb);
    _cntr->Connect(then_bb_end, merge_bb);
    // set merge_bb next to else_bb_end
    _cntr->Set_bb_order(else_bb_end, merge_bb);
    // restore air and cgir block
    Push(air_blk, merge_bb);
    return RETV();
  }

  //! @brief Set the preg used to substitute the symbol in lowering AIR.
  bool Set_preg(base::ADDR_DATUM_PTR sym, base::PREG_PTR preg) {
    if (sym->Type()->Is_prim()) {
      AIR_ASSERT(sym->Type_id() == preg->Type_id());
    } else {
      base::TYPE_PTR preg_type = preg->Type();
      AIR_ASSERT(preg_type->Is_ptr());
      AIR_ASSERT(preg_type->Cast_to_ptr()->Domain_type_id() == sym->Type_id());
    }
    std::pair<std::unordered_map<uint32_t, base::PREG_ID>::iterator, bool> res =
        _sym2preg.insert({sym->Id().Value(), preg->Id()});
    AIR_ASSERT(res.second);
    return res.second;
  }

  //! @brief Return the preg used to substitute the addr_datum in inst_select.
  //! If none exists, create and return a new one.
  base::PREG_PTR Get_preg(base::ADDR_DATUM_PTR sym) {
    std::pair<std::unordered_map<uint32_t, base::PREG_ID>::iterator, bool> res =
        _sym2preg.insert({sym->Id().Value(), base::PREG_ID()});
    if (!res.second) {
      base::PREG_ID preg_id = res.first->second;
      AIR_ASSERT(preg_id != base::PREG_ID());
      return _cntr->Func_scope()->Preg(preg_id);
    }
    base::TYPE_PTR type = sym->Type();
    if (!type->Is_prim()) {
      AIR_ASSERT(type->Is_record());
      type = _cntr->Func_scope()->Glob_scope().New_ptr_type(
          type, base::POINTER_KIND::FLAT32);
    }
    base::PREG_PTR preg = _cntr->Func_scope()->New_preg(type);
    res.first->second   = preg->Id();
    return preg;
  }
  //! @brief Return the preg used to substitute the addr_datum in
  //! inst_selection. If none exists, return null.
  base::PREG_ID Preg_id(base::ADDR_DATUM_PTR sym) const {
    std::unordered_map<uint32_t, base::PREG_ID>::const_iterator iter =
        _sym2preg.find(sym->Id().Value());
    if (iter == _sym2preg.end()) {
      return base::PREG_ID();
    }
    AIR_ASSERT(iter->second != base::PREG_ID());
    return iter->second;
  }
  base::PREG_PTR Preg(base::ADDR_DATUM_PTR sym) const {
    base::PREG_ID id = Preg_id(sym);
    if (id == base::PREG_ID()) return base::PREG_PTR();
    return _cntr->Func_scope()->Preg(id);
  }

  //! @brief Return the preg used to substitute the preg in inst_select.
  //! If none exists, create and return a new one.
  base::PREG_PTR Get_preg(base::PREG_PTR preg_bcg) {
    base::TYPE_PTR type = preg_bcg->Type();
    // not replace primitive/pointer type PREGs.
    if (type->Is_prim() || type->Is_ptr()) return preg_bcg;

    // return the existing PREG
    std::pair<std::unordered_map<uint32_t, base::PREG_ID>::iterator, bool> res =
        _preg2preg.insert({preg_bcg->Id().Value(), base::PREG_ID()});
    if (!res.second) {
      base::PREG_ID preg_id = res.first->second;
      AIR_ASSERT(preg_id != base::PREG_ID());
      return _cntr->Func_scope()->Preg(preg_id);
    }

    // create a new pointer-type PREG in CG to replace the one created
    // before CG.
    AIR_ASSERT(type->Is_record());
    type = _cntr->Func_scope()->Glob_scope().New_ptr_type(
        type, base::POINTER_KIND::FLAT32);
    base::PREG_PTR preg = _cntr->Func_scope()->New_preg(type);
    res.first->second   = preg->Id();
    return preg;
  }

  //! @brief Set the PREG used to replace a PREG created before CodeGen in
  //! inst_selection. In the current implementation, the PREGs to be replaced
  //! are all of RECORD_TYPE. The new PREGs used for replacement are pointers
  //! to the RECORD_TYPE values.
  void Set_preg(base::PREG_PTR preg_bcg, base::PREG_PTR preg_cg) {
    AIR_ASSERT(!preg_bcg->Type()->Is_prim());
    AIR_ASSERT(preg_cg->Type()->Is_ptr());
    AIR_ASSERT(preg_cg->Type()->Cast_to_ptr()->Domain_type_id() ==
               preg_bcg->Type_id());
    std::pair<std::unordered_map<uint32_t, base::PREG_ID>::iterator, bool> res =
        _preg2preg.insert({preg_bcg->Id().Value(), preg_cg->Id()});
    AIR_ASSERT_MSG(res.first->second == preg_cg->Id(),
                   "ERROR: replace one PREG with different ptr PREG");
  }
  //! @brief Return the PREG used to replace the PREG created before CodeGen in
  //! inst_selection. Return invalid PREG_ID() if no PREG exists.
  base::PREG_ID Preg(base::PREG_ID preg_bcg) const {
    std::unordered_map<uint32_t, base::PREG_ID>::const_iterator iter =
        _preg2preg.find(preg_bcg.Value());
    if (iter == _preg2preg.end()) return base::PREG_ID();
    return iter->second;
  }

  //! @brief Set the zero version opnd of PREG.
  void Set_zero_ver_opnd(OPND_ID opnd, base::PREG_ID preg) {
    std::pair<std::unordered_map<uint32_t, OPND_ID>::iterator, bool> res =
        _preg2opnd.insert({preg.Value(), opnd});
    AIR_ASSERT_MSG(res.first->second == opnd,
                   "ERROR: creating redundant zero version opnd of PREG_",
                   preg.Index());
  }
  //! @brief Return the existing zero version OPND of PREG. Return invalid
  //! OPND_ID() if no opnd exists.
  OPND_ID Zero_ver_opnd(base::PREG_ID preg) const {
    std::unordered_map<uint32_t, OPND_ID>::const_iterator iter =
        _preg2opnd.find(preg.Value());
    if (iter == _preg2opnd.end()) return OPND_ID();
    return iter->second;
  }

protected:
  // can do_loop be convert to do-while safely
  bool Safe_do_while(air::base::NODE_PTR node) const {
    AIR_ASSERT(node->Opcode() == air::core::OPC_DO_LOOP);
    air::base::NODE_PTR init = node->Loop_init();
    if (init->Opcode() != air::core::OPC_INTCONST) {
      return false;
    }
    air::base::NODE_PTR cmp = node->Compare();
    if (cmp->Num_child() != 2 ||
        cmp->Child(1)->Opcode() != air::core::OPC_INTCONST) {
      return false;
    }
    air::base::NODE_PTR inc = node->Loop_incr();
    if (inc->Num_child() != 2 ||
        inc->Child(1)->Opcode() != air::core::OPC_INTCONST) {
      return false;
    }
    int64_t init_val  = init->Intconst();
    int64_t cmpr_val  = cmp->Child(1)->Intconst();
    int64_t incr_val  = inc->Child(1)->Intconst();
    bool    increment = (inc->Opcode() == air::core::OPC_ADD && incr_val > 0);
    if (increment) {
      if ((cmp->Opcode() == air::core::OPC_LT && init_val < cmpr_val) ||
          (cmp->Opcode() == air::core::OPC_LE && init_val <= cmpr_val)) {
        return true;
      }
    } else {
      if ((cmp->Opcode() == air::core::OPC_GT && init_val > cmpr_val) ||
          (cmp->Opcode() == air::core::OPC_GE && init_val >= cmpr_val)) {
        return true;
      }
    }
    return false;
  }

  // new container
  CGIR_CONTAINER* _cntr;

  // new node stack
  std::vector<BB_PTR> _cg_blk;

  // new stmts to be appended after current stmt
  std::vector<INST_PTR> _post_inst;

  // PREG used to replace a symbol in lowering AIR.
  std::unordered_map<uint32_t, base::PREG_ID> _sym2preg;

  // PREG used to replace a non-prime type PREG in lowering AIR.
  std::unordered_map<uint32_t, base::PREG_ID> _preg2preg;

  // zero version OPND of each PREG
  std::unordered_map<uint32_t, OPND_ID> _preg2opnd;

  // force treating do_loop to while_do
  bool _force_while_do;

};  // AIR2CGIR_CTX

}  // namespace cg
}  // namespace air

#endif  // AIR_CG_AIR2CGIR_CTX_H
