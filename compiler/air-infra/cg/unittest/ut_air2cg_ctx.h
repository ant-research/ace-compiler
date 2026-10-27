//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_CG_UNITTEST_UT_AIR2CGIR_CTX_H
#define AIR_CG_UNITTEST_UT_AIR2CGIR_CTX_H

#include <unordered_map>

#include "air/base/visitor.h"
#include "air/cg/air2cgir_ctx.h"
#include "air/cg/cgir_container.h"
#include "air/cg/cgir_util.h"
#include "air/core/default_handler.h"
#include "air/core/handler.h"
#include "ut_air2cg_ti.h"

using namespace air::base;
using namespace air::core;
using namespace air::cg;

namespace {

class UT_AIR2CGIR_CTX : public AIR2CGIR_CTX {
public:
  UT_AIR2CGIR_CTX(CGIR_CONTAINER* cg_cntr, bool while_do = false)
      : AIR2CGIR_CTX(cg_cntr, while_do) {}

  template <typename RETV, typename VISITOR>
  RETV Handle_scalar_store(VISITOR* visitor, air::base::ADDR_DATUM_PTR var,
                           air::base::NODE_PTR val) {
    int64_t ival = 0;
    if (val->Opcode() == OPC_INTCONST) {
      ival = val->Intconst();
    } else if (val->Opcode() == OPC_ADD) {
      AIR_ASSERT(val->Child(1)->Opcode() == OPC_INTCONST);
      ival = val->Child(1)->Intconst();
    } else {
      AIR_ASSERT(false);
    }
    Record_bb(ival);
    UT_TARG_INFO ti(Container());
    Prepend(ti.Gen_st(var->Id(), ival));
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_jmp(VISITOR* visitor, air::cg::BB_PTR target) {
    UT_TARG_INFO ti(Container());
    Prepend(ti.Gen_jmp(target->Id()));
    return RETV();
  }

  template <typename RETV, typename VISITOR, bool TRUE_BR>
  RETV Handle_cond_jmp(VISITOR* visitor, air::base::NODE_PTR cmp,
                       air::cg::BB_PTR target) {
    AIR_ASSERT(cmp->Child(1)->Opcode() == OPC_INTCONST);
    Record_bb(cmp->Child(1)->Intconst());
    UT_TARG_INFO ti(Container());
    Prepend(ti.Gen_cond_jmp<TRUE_BR>(cmp->Child(0)->Addr_datum_id(),
                                     cmp->Child(1)->Intconst(), target->Id()));
    return RETV();
  }

  void Record_bb(int val) {
    AIR_ASSERT(_val_bb_map.find(val) == _val_bb_map.end());
    _val_bb_map[val] = Top_bb()->Id().Value();
  }

  int Val_bb(int val) {
    AIR_ASSERT_MSG(_val_bb_map.find(val) != _val_bb_map.end(), "val=%d", val);
    return _val_bb_map[val];
  }

private:
  std::unordered_map<int, int> _val_bb_map;
};

class UT_CORE_HANDLER : public air::core::DEFAULT_HANDLER {
public:
  template <typename RETV, typename VISITOR>
  RETV Handle_func_entry(VISITOR* visitor, air::base::NODE_PTR node) {
    return visitor->Context().template Handle_func_entry<RETV>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_lt(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(node->Child(1)->Opcode() == OPC_INTCONST);
    UT_AIR2CGIR_CTX& ctx = visitor->Context();
    ctx.Record_bb(node->Child(1)->Intconst());
    UT_TARG_INFO ti(ctx.Container());
    ctx.Prepend(
        ti.Gen_st(node->Child(0)->Addr_datum_id(), node->Child(1)->Intconst()));
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_st(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(node->Child(0)->Opcode() == OPC_INTCONST);
    UT_AIR2CGIR_CTX& ctx = visitor->Context();
    ctx.Record_bb(node->Child(0)->Intconst());
    UT_TARG_INFO ti(ctx.Container());
    ctx.Prepend(ti.Gen_st(node->Addr_datum_id(), node->Child(0)->Intconst()));
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_retv(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(node->Child(0)->Opcode() == OPC_INTCONST);
    UT_AIR2CGIR_CTX& ctx = visitor->Context();
    ctx.Record_bb(node->Child(0)->Intconst());
    UT_TARG_INFO ti(ctx.Container());
    ctx.Prepend(ti.Gen_retv(node->Child(0)->Intconst()));
    return RETV();
  }
};

}  // namespace

#endif  // AIR_CG_UNITTEST_UT_AIR2CGIR_CTX_H
