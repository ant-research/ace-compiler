//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/cg/cgir_container.h"
#include "gtest/gtest.h"

using namespace air::cg;

namespace {

TEST(CGIR, CONTAINER) {
  // create a container
  CGIR_CONTAINER cont;
  // create a basic block node
  NODE_PTR node = cont.New_node();

  // create operands
  OPND_PTR opnd0 = cont.New_opnd(REG_CLASS::GPR);
  OPND_PTR opnd1 = cont.New_opnd(REG_CLASS::GPR);
  OPND_PTR opnd2 = cont.New_opnd(1);
  OPND_PTR opnd3 = cont.New_opnd(REG_CLASS::FPR, 0);
  OPND_PTR opnd4 = cont.New_opnd(REG_CLASS::FPR, 1);
  OPND_PTR opnd5 = cont.New_opnd(air::base::SYM_ID(2), -4);
  OPND_PTR opnd6 = cont.New_opnd(air::base::CONSTANT_ID(3), 4);
  OPND_PTR opnd7 = cont.New_opnd(air::base::LABEL_ID(4));

  // create instructions
  air::base::SPOS spos;
  INST_PTR        inst0 = cont.New_inst(spos, 1, 2, 1, 2);
  INST_PTR        inst1 = cont.New_inst(spos, 2, 3, 1, 2);
  INST_PTR        inst2 = cont.New_inst(spos, 3, 4, 1, 2);
  INST_PTR        inst3 = cont.New_inst(spos, 4, 5, 1, 2);
  INST_PTR        inst4 = cont.New_inst(spos, 5, 6, 1, 2);
  INST_PTR        inst5 = cont.New_inst(spos, 6, 7, 1, 2);

  // set instructions' operands and results
  inst0->Set_res(0, opnd0);
  inst0->Set_opnd(0, opnd1);
  inst0->Set_opnd(1, opnd2);
  inst1->Set_res(0, opnd3);
  inst1->Set_opnd(0, opnd4);
  inst1->Set_opnd(1, opnd5);
  inst2->Set_res(0, opnd1);
  inst2->Set_opnd(0, opnd0);
  inst2->Set_opnd(1, opnd6);
  inst3->Set_res(0, opnd4);
  inst3->Set_opnd(0, opnd3);
  inst3->Set_opnd(1, opnd7);
  inst4->Set_res(0, opnd0);
  inst4->Set_opnd(0, opnd1);
  inst4->Set_opnd(1, opnd5);
  inst5->Set_res(0, opnd3);
  inst5->Set_opnd(0, opnd4);
  inst5->Set_opnd(1, opnd6);

  // validate operands
  EXPECT_TRUE(opnd0->Is_register());
  EXPECT_EQ(opnd0->Reg_class(), REG_CLASS::GPR);
  EXPECT_EQ(opnd0->Reg_num(), REG_UNKNOWN);
  EXPECT_TRUE(opnd1->Is_register());
  EXPECT_EQ(opnd1->Reg_class(), REG_CLASS::GPR);
  EXPECT_EQ(opnd1->Reg_num(), REG_UNKNOWN);
  EXPECT_TRUE(opnd2->Is_immediate());
  EXPECT_EQ(opnd2->Immediate(), 1);
  EXPECT_TRUE(opnd3->Is_register());
  EXPECT_EQ(opnd3->Reg_class(), REG_CLASS::FPR);
  EXPECT_EQ(opnd3->Reg_num(), 0);
  EXPECT_TRUE(opnd4->Is_register());
  EXPECT_EQ(opnd4->Reg_class(), REG_CLASS::FPR);
  EXPECT_EQ(opnd4->Reg_num(), 1);
  EXPECT_TRUE(opnd5->Is_symbol());
  EXPECT_EQ(opnd5->Symbol(), air::base::SYM_ID(2));
  EXPECT_EQ(opnd5->Offset(), -4);
  EXPECT_TRUE(opnd6->Is_constant());
  EXPECT_EQ(opnd6->Constant(), air::base::CONSTANT_ID(3));
  EXPECT_EQ(opnd6->Offset(), 4);
  EXPECT_TRUE(opnd7->Is_label());
  EXPECT_EQ(opnd7->Label(), air::base::LABEL_ID(4));

  // validate instructions
  EXPECT_EQ(inst0->Isa(), 1);
  EXPECT_EQ(inst0->Opcode(), 2);
  EXPECT_EQ(inst0->Res_id(0), opnd0->Id());
  EXPECT_EQ(inst0->Opnd_id(0), opnd1->Id());
  EXPECT_EQ(inst0->Opnd_id(1), opnd2->Id());
  EXPECT_EQ(inst1->Isa(), 2);
  EXPECT_EQ(inst1->Opcode(), 3);
  EXPECT_EQ(inst1->Res_id(0), opnd3->Id());
  EXPECT_EQ(inst1->Opnd_id(0), opnd4->Id());
  EXPECT_EQ(inst1->Opnd_id(1), opnd5->Id());
  EXPECT_EQ(inst2->Isa(), 3);
  EXPECT_EQ(inst2->Opcode(), 4);
  EXPECT_EQ(inst2->Res_id(0), opnd1->Id());
  EXPECT_EQ(inst2->Opnd_id(0), opnd0->Id());
  EXPECT_EQ(inst2->Opnd_id(1), opnd6->Id());
  EXPECT_EQ(inst3->Isa(), 4);
  EXPECT_EQ(inst3->Opcode(), 5);
  EXPECT_EQ(inst3->Res_id(0), opnd4->Id());
  EXPECT_EQ(inst3->Opnd_id(0), opnd3->Id());
  EXPECT_EQ(inst3->Opnd_id(1), opnd7->Id());
  EXPECT_EQ(inst4->Isa(), 5);
  EXPECT_EQ(inst4->Opcode(), 6);
  EXPECT_EQ(inst4->Res_id(0), opnd0->Id());
  EXPECT_EQ(inst4->Opnd_id(0), opnd1->Id());
  EXPECT_EQ(inst4->Opnd_id(1), opnd5->Id());
  EXPECT_EQ(inst5->Isa(), 6);
  EXPECT_EQ(inst5->Opcode(), 7);
  EXPECT_EQ(inst5->Res_id(0), opnd3->Id());
  EXPECT_EQ(inst5->Opnd_id(0), opnd4->Id());
  EXPECT_EQ(inst5->Opnd_id(1), opnd6->Id());

  // add instructions to basic block node and validate list head and tail
  node->Append(inst4);
  EXPECT_EQ(node->First_id(), inst4->Id());
  EXPECT_EQ(node->Last_id(), inst4->Id());

  node->Prepend(inst1);
  EXPECT_EQ(node->First_id(), inst1->Id());
  EXPECT_EQ(node->Last_id(), inst4->Id());

  node->Append(inst4, inst5);
  EXPECT_EQ(node->First_id(), inst1->Id());
  EXPECT_EQ(node->Last_id(), inst5->Id());

  node->Prepend(inst1, inst0);
  EXPECT_EQ(node->First_id(), inst0->Id());
  EXPECT_EQ(node->Last_id(), inst5->Id());

  node->Append(inst1, inst2);
  EXPECT_EQ(node->First_id(), inst0->Id());
  EXPECT_EQ(node->Last_id(), inst5->Id());

  node->Prepend(inst4, inst3);
  EXPECT_EQ(node->First_id(), inst0->Id());
  EXPECT_EQ(node->Last_id(), inst5->Id());

  cont.Print();

  // validate list next and prev
  EXPECT_EQ(node->First_id(), inst0->Id());
  EXPECT_EQ(node->First()->Next_id(), inst1->Id());
  EXPECT_EQ(node->First()->Next()->Next_id(), inst2->Id());
  EXPECT_EQ(node->First()->Next()->Next()->Next_id(), inst3->Id());
  EXPECT_EQ(node->First()->Next()->Next()->Next()->Next_id(), inst4->Id());
  EXPECT_EQ(node->First()->Next()->Next()->Next()->Next()->Next_id(),
            inst5->Id());
  EXPECT_EQ(node->First()->Next()->Next()->Next()->Next()->Next()->Next_id(),
            INST_ID());

  EXPECT_EQ(node->Last_id(), inst5->Id());
  EXPECT_EQ(node->Last()->Prev_id(), inst4->Id());
  EXPECT_EQ(node->Last()->Prev()->Prev_id(), inst3->Id());
  EXPECT_EQ(node->Last()->Prev()->Prev()->Prev_id(), inst2->Id());
  EXPECT_EQ(node->Last()->Prev()->Prev()->Prev()->Prev_id(), inst1->Id());
  EXPECT_EQ(node->Last()->Prev()->Prev()->Prev()->Prev()->Prev_id(),
            inst0->Id());
  EXPECT_EQ(node->Last()->Prev()->Prev()->Prev()->Prev()->Prev()->Prev_id(),
            INST_ID());
}

}  // namespace
