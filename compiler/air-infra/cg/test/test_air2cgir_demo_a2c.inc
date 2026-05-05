//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/base/visitor.h"
#include "air/core/handler.h"
#include "air/core/default_handler.h"
#include "air/cg/cgir_container.h"
#include "air/cg/air2cgir_ctx.h"
#include "test_aircg_demo_isa.h"

namespace air {

namespace cg {

namespace demo {

class DEMO_AIR2CGIR_CTX : public air::cg::AIR2CGIR_CTX {
public:
  DEMO_AIR2CGIR_CTX(air::cg::CGIR_CONTAINER* cntr) : air::cg::AIR2CGIR_CTX(cntr) {}

  template <typename RETV, typename VISITOR>
  RETV Handle_scalar_store(VISITOR* visitor, air::base::ADDR_DATUM_PTR var, air::base::NODE_PTR val) {
    RETV ret = visitor->template Visit<RETV>(val);
    AIR_ASSERT(ret != air::base::Null_ptr);
    air::cg::CGIR_CONTAINER* cntr = visitor->Context().Container();
    air::cg::OPND_PTR addr = cntr->New_opnd(var->Id(), 0);
    air::cg::INST_PTR st_ptr = cntr->New_inst(val->Spos(), demo::ISA_ID, demo::OPC_SD, ret->Id(), addr->Id());
    visitor->Context().Prepend(st_ptr);
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_jmp(VISITOR* visitor, air::cg::BB_PTR target) {
    air::cg::CGIR_CONTAINER* cntr = visitor->Context().Container();
    air::cg::OPND_PTR tgt = cntr->New_opnd(target->Id());
    air::cg::INST_PTR inst = cntr->New_inst(air::base::SPOS(), demo::ISA_ID, demo::OPC_J, tgt->Id());
    visitor->Context().Prepend(inst);
    return RETV();
  }

  template <typename RETV, typename VISITOR, bool TRUE_BR>
  RETV Handle_cond_jmp(VISITOR* visitor, air::base::NODE_PTR cmp, air::cg::BB_PTR target) {
    air::cg::CGIR_CONTAINER* cntr = visitor->Context().Container();
    air::cg::OPND_PTR tgt = cntr->New_opnd(target->Id());
    RETV op0 = visitor->template Visit<RETV>(cmp->Child(0));
    AIR_ASSERT(op0 != air::base::Null_ptr);
    RETV op1 = visitor->template Visit<RETV>(cmp->Child(1));
    AIR_ASSERT(op1 != air::base::Null_ptr);
    uint32_t opc = demo::OPC_UNDEF;
    switch (cmp->Opcode()) {
    case air::core::OPC_LT:
      opc = TRUE_BR ? demo::OPC_BGE /* should be OPC_BLT */ : demo::OPC_BGE;
      break;
    default:
      AIR_ASSERT_MSG(false, "TODO: support %s", cmp->Name());
      break;
    }
    air::cg::INST_PTR inst = cntr->New_inst(cmp->Spos(), demo::ISA_ID, opc, op0->Id(), op1->Id(), tgt->Id());
    visitor->Context().Prepend(inst);
    return RETV();
  }
};

class DEMO_AIR2CGIR_HANDLER : public air::core::DEFAULT_HANDLER {
public:
  template <typename RETV, typename VISITOR>
  RETV Handle_add(VISITOR* visitor, air::base::NODE_PTR node) {
    RETV op0 = visitor->template Visit<RETV>(node->Child(0));
    AIR_ASSERT(op0 != air::base::Null_ptr);
    RETV op1 = visitor->template Visit<RETV>(node->Child(1));
    AIR_ASSERT(op1 != air::base::Null_ptr);
    air::cg::CGIR_CONTAINER* cntr = visitor->Context().Container();
    air::cg::OPND_PTR res = cntr->New_preg(REG_CLASS::GPR);
    air::cg::INST_PTR inst = cntr->New_inst(node->Spos(), demo::ISA_ID, demo::OPC_ADD, res->Id(), op0->Id(), op1->Id());
    visitor->Context().Prepend(inst);
    return res;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_array(VISITOR* visitor, air::base::NODE_PTR node) {
    AIR_ASSERT(node->Child(0)->Opcode() == air::core::OPC_LDA);
    air::base::ADDR_DATUM_PTR sym = node->Child(0)->Addr_datum();
    AIR_ASSERT(sym->Type()->Is_array());
    air::base::ARRAY_TYPE_PTR type = sym->Type()->Cast_to_arr();
    AIR_ASSERT(type->Dim() == node->Num_child() - 1);
    AIR_ASSERT_MSG(type->Dim() == 1, "TODO: multi-dim array");
    RETV ofst = visitor->template Visit<RETV>(node->Child(1));
    AIR_ASSERT(ofst != air::base::Null_ptr);
    RETV base = visitor->template Visit<RETV>(node->Child(0));
    AIR_ASSERT(base != air::base::Null_ptr);
    air::cg::CGIR_CONTAINER* cntr = visitor->Context().Container();
    air::cg::OPND_PTR addr = cntr->New_preg(REG_CLASS::GPR);
    air::cg::INST_PTR addr_add = cntr->New_inst(node->Spos(), demo::ISA_ID, demo::OPC_ADD, addr->Id(), base->Id(), ofst->Id());
    visitor->Context().Prepend(addr_add);
    return addr;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_func_entry(VISITOR* visitor, air::base::NODE_PTR node) {
    return visitor->Context().template Handle_func_entry<RETV>(visitor, node);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_intconst(VISITOR* visitor, air::base::NODE_PTR node) {
    air::cg::CGIR_CONTAINER* cntr = visitor->Context().Container();
    return cntr->New_opnd(node->Intconst());
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ild(VISITOR* visitor, air::base::NODE_PTR node) {
    RETV addr = visitor->template Visit<RETV>(node->Child(0));
    AIR_ASSERT(addr != air::base::Null_ptr);
    air::cg::CGIR_CONTAINER* cntr = visitor->Context().Container();
    air::cg::OPND_PTR res = cntr->New_preg(REG_CLASS::GPR);
    air::cg::INST_PTR inst = cntr->New_inst(node->Spos(), demo::ISA_ID, demo::OPC_LD, res->Id(), addr->Id());
    visitor->Context().Prepend(inst);
    return res;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ist(VISITOR* visitor, air::base::NODE_PTR node) {
    RETV addr = visitor->template Visit<RETV>(node->Child(0));
    AIR_ASSERT(addr != air::base::Null_ptr);
    RETV val = visitor->template Visit<RETV>(node->Child(1));
    AIR_ASSERT(val != air::base::Null_ptr);
    air::cg::CGIR_CONTAINER* cntr = visitor->Context().Container();
    air::cg::INST_PTR st_ptr = cntr->New_inst(node->Spos(), demo::ISA_ID, demo::OPC_SD, val->Id(), addr->Id());
    visitor->Context().Prepend(st_ptr);
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_ld(VISITOR* visitor, air::base::NODE_PTR node) {
    air::cg::CGIR_CONTAINER* cntr = visitor->Context().Container();
    air::cg::OPND_PTR res = cntr->New_preg(REG_CLASS::GPR);
    air::cg::OPND_PTR addr = cntr->New_opnd(node->Addr_datum_id(), 0);
    air::cg::INST_PTR ld = cntr->New_inst(node->Spos(), demo::ISA_ID, demo::OPC_LD, res->Id(), addr->Id());
    visitor->Context().Prepend(ld);
    return res;
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_lda(VISITOR* visitor, air::base::NODE_PTR node) {
    air::cg::CGIR_CONTAINER* cntr = visitor->Context().Container();
    return cntr->New_opnd(node->Addr_datum_id(), 0);
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_lt(VISITOR* visitor, air::base::NODE_PTR node) {
    RETV op0 = visitor->template Visit<RETV>(node->Child(0));
    AIR_ASSERT(op0 != air::base::Null_ptr);
    RETV op1 = visitor->template Visit<RETV>(node->Child(1));
    AIR_ASSERT(op1 != air::base::Null_ptr);
    air::cg::CGIR_CONTAINER* cntr = visitor->Context().Container();
    air::cg::INST_PTR inst = cntr->New_inst(node->Spos(), demo::ISA_ID, demo::OPC_BNEZ, op0->Id(), op1->Id() /* TODO label */);
    visitor->Context().Prepend(inst);
    return RETV();
  }

  template <typename RETV, typename VISITOR>
  RETV Handle_retv(VISITOR* visitor, air::base::NODE_PTR node) {
    RETV op0 = visitor->template Visit<RETV>(node->Child(0));
    AIR_ASSERT(op0 != air::base::Null_ptr);
    air::cg::CGIR_CONTAINER* cntr = visitor->Context().Container();
    air::cg::OPND_PTR res = cntr->New_opnd(REG_CLASS::GPR, demo::X10);
    air::cg::INST_PTR inst = cntr->New_inst(node->Spos(), demo::ISA_ID, demo::OPC_MV, op0->Id(), res->Id());
    visitor->Context().Prepend(inst);
    return RETV();
  }
};

static void Air_to_cgir(const air::base::CONTAINER* air_cont, air::cg::CGIR_CONTAINER* cg_cont) {
  DEMO_AIR2CGIR_CTX ctx(cg_cont);
  air::base::VISITOR<DEMO_AIR2CGIR_CTX,
                     air::core::HANDLER<DEMO_AIR2CGIR_HANDLER> >
                      trav(ctx);
  air::base::NODE_PTR body = air_cont->Entry_node();
  trav.template Visit<air::cg::OPND_PTR>(body);
}  // Air_to_cgir

}  // namespace demo

}  // namespace cg

}  // namespace air
