//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/cg/cgir_container.h"
#include "air/util/debug.h"
#include "test_aircg_demo_isa.h"

namespace air {

namespace cg {

namespace demo {

static CGIR_CONTAINER& Init_container(CGIR_CONTAINER& cont) {
  // TEST PROGRAM:
  //   typedef struct {
  //     long coeff[16];
  //   } poly;
  //   typedef struct {
  //     struct poly c0;
  //     struct poly c1;
  //   } cipher;
  //   cipher* cadd(cipher* res, cipher* op0, cipher* op1) {
  //     for (uint32_t i = 0; i < 16; ++i) {
  //       res->c0.coeff[i] = op0->c0.coeff[i] + op1->c0.coeff[i];
  //       res->c1.coeff[i] = op0->c1.coeff[i] + op1->c1.coeff[i];
  //     }
  //     return res;
  //   }

  // BUILD CFG
  //   fake-entry
  //        |
  //        n0
  //     e0 |
  //        n1 <----+
  //     e1 |    e3 |
  //        n2 -----+
  //     e2 |
  //        n3
  //        |
  //   fake-exit

  BB_PTR n0 = cont.New_bb();
  BB_PTR n1 = cont.New_bb();
  BB_PTR n2 = cont.New_bb();
  BB_PTR n3 = cont.New_bb();

  EDGE_PTR e0 = cont.Connect(n0, n1);
  EDGE_PTR e1 = cont.Connect(n1, n2);
  EDGE_PTR e2 = cont.Connect(n1, n3);
  EDGE_PTR e3 = cont.Connect(n2, n1);

  cont.Set_entry(n0);
  cont.Set_exit(n3);

  // create operands
  OPND_PTR x0           = cont.New_opnd(REG_CLASS::GPR, demo::X0);
  OPND_PTR x10          = cont.New_opnd(REG_CLASS::GPR, demo::X10);
  OPND_PTR x11          = cont.New_opnd(REG_CLASS::GPR, demo::X11);
  OPND_PTR x12          = cont.New_opnd(REG_CLASS::GPR, demo::X12);
  OPND_PTR r_i          = cont.New_preg(REG_CLASS::GPR);
  OPND_PTR r_res_c0_ptr = cont.New_preg(REG_CLASS::GPR);
  OPND_PTR r_res_c1_ptr = cont.New_preg(REG_CLASS::GPR);
  OPND_PTR r_op0_c0_ptr = cont.New_preg(REG_CLASS::GPR);
  OPND_PTR r_op0_c1_ptr = cont.New_preg(REG_CLASS::GPR);
  OPND_PTR r_op1_c0_ptr = cont.New_preg(REG_CLASS::GPR);
  OPND_PTR r_op1_c1_ptr = cont.New_preg(REG_CLASS::GPR);
  OPND_PTR r_res_c0_val = cont.New_preg(REG_CLASS::GPR);
  OPND_PTR r_res_c1_val = cont.New_preg(REG_CLASS::GPR);
  OPND_PTR r_op0_c0_val = cont.New_preg(REG_CLASS::GPR);
  OPND_PTR r_op0_c1_val = cont.New_preg(REG_CLASS::GPR);
  OPND_PTR r_op1_c0_val = cont.New_preg(REG_CLASS::GPR);
  OPND_PTR r_op1_c1_val = cont.New_preg(REG_CLASS::GPR);
  OPND_PTR i_1          = cont.New_opnd(1);
  OPND_PTR i_8          = cont.New_opnd(8);
  OPND_PTR i_16         = cont.New_opnd(16);
  OPND_PTR i_128        = cont.New_opnd(128);
  OPND_PTR i_neg_1      = cont.New_opnd(-1);
  OPND_PTR l_bb1        = cont.New_opnd(n1->Id());

  air::base::SPOS spos;
  INST_PTR        inst;

  // create instructions for fake entry
  // r_res_c0_ptr = x10
  // r_op0_c0_ptr = x11
  // r_op1_c0_ptr = x12
  BB_PTR entry = cont.Entry();
  inst = cont.New_inst(spos, demo::ISA_ID, demo::OPC_MV, r_res_c0_ptr->Id(),
                       x10->Id());
  AIR_ASSERT(inst->Res_id(0) == r_res_c0_ptr->Id());
  AIR_ASSERT(inst->Opnd_id(0) == x10->Id());
  entry->Append(inst);
  inst = cont.New_inst(spos, demo::ISA_ID, demo::OPC_MV, r_op0_c0_ptr->Id(),
                       x11->Id());
  AIR_ASSERT(inst->Res_id(0) == r_op0_c0_ptr->Id());
  AIR_ASSERT(inst->Opnd_id(0) == x11->Id());
  entry->Append(inst);
  inst = cont.New_inst(spos, demo::ISA_ID, demo::OPC_MV, r_op1_c0_ptr->Id(),
                       x12->Id());
  AIR_ASSERT(inst->Res_id(0) == r_op1_c0_ptr->Id());
  AIR_ASSERT(inst->Opnd_id(0) == x12->Id());
  entry->Append(inst);

  // create instructions for n0
  // r_res_c1_ptr = r_res_c0_ptr + 128
  // r_op0_c1_ptr = r_op0_c0_ptr + 128
  // r_op1_c1_ptr = r_op1_c0_ptr + 128
  // r_i      = 16
  inst = cont.New_inst(spos, demo::ISA_ID, demo::OPC_ADDI, r_res_c1_ptr->Id(),
                       r_res_c0_ptr->Id(), i_128->Id());
  AIR_ASSERT(inst->Res_id(0) == r_res_c1_ptr->Id());
  AIR_ASSERT(inst->Opnd_id(0) == r_res_c0_ptr->Id());
  AIR_ASSERT(inst->Opnd_id(1) == i_128->Id());
  n0->Append(inst);
  inst = cont.New_inst(spos, demo::ISA_ID, demo::OPC_ADDI, r_op0_c1_ptr->Id(),
                       r_op0_c0_ptr->Id(), i_128->Id());
  AIR_ASSERT(inst->Res_id(0) == r_op0_c1_ptr->Id());
  AIR_ASSERT(inst->Opnd_id(0) == r_op0_c0_ptr->Id());
  AIR_ASSERT(inst->Opnd_id(1) == i_128->Id());
  n0->Append(inst);
  inst = cont.New_inst(spos, demo::ISA_ID, demo::OPC_ADDI, r_op1_c1_ptr->Id(),
                       r_op1_c0_ptr->Id(), i_128->Id());
  AIR_ASSERT(inst->Res_id(0) == r_op1_c1_ptr->Id());
  AIR_ASSERT(inst->Opnd_id(0) == r_op1_c0_ptr->Id());
  AIR_ASSERT(inst->Opnd_id(1) == i_128->Id());
  n0->Append(inst);
  inst = cont.New_inst(spos, demo::ISA_ID, demo::OPC_ADDI, r_i->Id(), x0->Id(),
                       i_16->Id());
  AIR_ASSERT(inst->Res_id(0) == r_i->Id());
  AIR_ASSERT(inst->Opnd_id(0) == x0->Id());
  AIR_ASSERT(inst->Opnd_id(1) == i_16->Id());
  n0->Append(inst);

  // create instruction for n1
  // *r_res_c0_ptr = *r_op0_c0_ptr + *r_op1_c0_ptr
  // *r_res_c1_ptr = *r_op0_c1_ptr + *r_op1_c1_ptr
  inst = cont.New_inst(spos, demo::ISA_ID, demo::OPC_LD, r_op0_c0_val->Id(),
                       r_op0_c0_ptr->Id());
  AIR_ASSERT(inst->Res_id(0) == r_op0_c0_val->Id());
  AIR_ASSERT(inst->Opnd_id(0) == r_op0_c0_ptr->Id());
  n1->Append(inst);
  inst = cont.New_inst(spos, demo::ISA_ID, demo::OPC_LD, r_op1_c0_val->Id(),
                       r_op1_c0_ptr->Id());
  AIR_ASSERT(inst->Res_id(0) == r_op1_c0_val->Id());
  AIR_ASSERT(inst->Opnd_id(0) == r_op1_c0_ptr->Id());
  n1->Append(inst);
  inst = cont.New_inst(spos, demo::ISA_ID, demo::OPC_ADD, r_res_c0_val->Id(),
                       r_op0_c0_val->Id(), r_op1_c0_val->Id());
  AIR_ASSERT(inst->Res_id(0) == r_res_c0_val->Id());
  AIR_ASSERT(inst->Opnd_id(0) == r_op0_c0_val->Id());
  AIR_ASSERT(inst->Opnd_id(1) == r_op1_c0_val->Id());
  n1->Append(inst);
  inst = cont.New_inst(spos, demo::ISA_ID, demo::OPC_SD, r_res_c0_val->Id(),
                       r_res_c0_ptr->Id());
  AIR_ASSERT(inst->Opnd_id(0) == r_res_c0_val->Id());
  AIR_ASSERT(inst->Opnd_id(1) == r_res_c0_ptr->Id());
  n1->Append(inst);
  inst = cont.New_inst(spos, demo::ISA_ID, demo::OPC_LD, r_op0_c1_val->Id(),
                       r_op0_c1_ptr->Id());
  AIR_ASSERT(inst->Res_id(0) == r_op0_c1_val->Id());
  AIR_ASSERT(inst->Opnd_id(0) == r_op0_c1_ptr->Id());
  n1->Append(inst);
  inst = cont.New_inst(spos, demo::ISA_ID, demo::OPC_LD, r_op1_c1_val->Id(),
                       r_op1_c1_ptr->Id());
  AIR_ASSERT(inst->Res_id(0) == r_op1_c1_val->Id());
  AIR_ASSERT(inst->Opnd_id(0) == r_op1_c1_ptr->Id());
  n1->Append(inst);
  inst = cont.New_inst(spos, demo::ISA_ID, demo::OPC_ADD, r_res_c1_val->Id(),
                       r_op0_c1_val->Id(), r_op1_c1_val->Id());
  AIR_ASSERT(inst->Res_id(0) == r_res_c1_val->Id());
  AIR_ASSERT(inst->Opnd_id(0) == r_op0_c1_val->Id());
  AIR_ASSERT(inst->Opnd_id(1) == r_op1_c1_val->Id());
  n1->Append(inst);
  inst = cont.New_inst(spos, demo::ISA_ID, demo::OPC_SD, r_res_c1_val->Id(),
                       r_res_c1_ptr->Id());
  AIR_ASSERT(inst->Opnd_id(0) == r_res_c1_val->Id());
  AIR_ASSERT(inst->Opnd_id(1) == r_res_c1_ptr->Id());
  n1->Append(inst);

  // create instruction for n2
  // r_res_c0_ptr = r_res_c0_ptr + 128
  // r_op0_c0_ptr = r_op0_c0_ptr + 128
  // r_op1_c0_ptr = r_op1_c0_ptr + 128
  // r_res_c1_ptr = r_res_c1_ptr + 128
  // r_op0_c1_ptr = r_op0_c1_ptr + 128
  // r_op1_c1_ptr = r_op1_c1_ptr + 128
  // r_i = r_i - 1
  // bnez r_i, n1
  inst = cont.New_inst(spos, demo::ISA_ID, demo::OPC_ADDI, r_res_c0_ptr->Id(),
                       r_res_c0_ptr->Id(), i_128->Id());
  AIR_ASSERT(inst->Res_id(0) == r_res_c0_ptr->Id());
  AIR_ASSERT(inst->Opnd_id(0) == r_res_c0_ptr->Id());
  AIR_ASSERT(inst->Opnd_id(1) == i_128->Id());
  n2->Append(inst);
  inst = cont.New_inst(spos, demo::ISA_ID, demo::OPC_ADDI, r_op0_c0_ptr->Id(),
                       r_op0_c0_ptr->Id(), i_128->Id());
  AIR_ASSERT(inst->Res_id(0) == r_op0_c0_ptr->Id());
  AIR_ASSERT(inst->Opnd_id(0) == r_op0_c0_ptr->Id());
  AIR_ASSERT(inst->Opnd_id(1) == i_128->Id());
  n2->Append(inst);
  inst = cont.New_inst(spos, demo::ISA_ID, demo::OPC_ADDI, r_op1_c0_ptr->Id(),
                       r_op1_c0_ptr->Id(), i_128->Id());
  AIR_ASSERT(inst->Res_id(0) == r_op1_c0_ptr->Id());
  AIR_ASSERT(inst->Opnd_id(0) == r_op1_c0_ptr->Id());
  AIR_ASSERT(inst->Opnd_id(1) == i_128->Id());
  n2->Append(inst);
  inst = cont.New_inst(spos, demo::ISA_ID, demo::OPC_ADDI, r_res_c1_ptr->Id(),
                       r_res_c1_ptr->Id(), i_128->Id());
  AIR_ASSERT(inst->Res_id(0) == r_res_c1_ptr->Id());
  AIR_ASSERT(inst->Opnd_id(0) == r_res_c1_ptr->Id());
  AIR_ASSERT(inst->Opnd_id(1) == i_128->Id());
  n2->Append(inst);
  inst = cont.New_inst(spos, demo::ISA_ID, demo::OPC_ADDI, r_op0_c1_ptr->Id(),
                       r_op0_c1_ptr->Id(), i_128->Id());
  AIR_ASSERT(inst->Res_id(0) == r_op0_c1_ptr->Id());
  AIR_ASSERT(inst->Opnd_id(0) == r_op0_c1_ptr->Id());
  AIR_ASSERT(inst->Opnd_id(1) == i_128->Id());
  n2->Append(inst);
  inst = cont.New_inst(spos, demo::ISA_ID, demo::OPC_ADDI, r_op1_c1_ptr->Id(),
                       r_op1_c1_ptr->Id(), i_128->Id());
  AIR_ASSERT(inst->Res_id(0) == r_op1_c1_ptr->Id());
  AIR_ASSERT(inst->Opnd_id(0) == r_op1_c1_ptr->Id());
  AIR_ASSERT(inst->Opnd_id(1) == i_128->Id());
  n2->Append(inst);
  inst = cont.New_inst(spos, demo::ISA_ID, demo::OPC_ADDI, r_i->Id(), r_i->Id(),
                       i_neg_1->Id());
  AIR_ASSERT(inst->Res_id(0) == r_i->Id());
  AIR_ASSERT(inst->Opnd_id(0) == r_i->Id());
  AIR_ASSERT(inst->Opnd_id(1) == i_neg_1->Id());
  n2->Append(inst);
  inst =
      cont.New_inst(spos, demo::ISA_ID, demo::OPC_BNEZ, r_i->Id(), l_bb1->Id());
  AIR_ASSERT(inst->Opnd_id(0) == r_i->Id());
  AIR_ASSERT(inst->Opnd_id(1) == l_bb1->Id());

  // create instruction for n3
  // x10 = r_res_c0_ptr
  inst = cont.New_inst(spos, demo::ISA_ID, demo::OPC_MV, x10->Id(),
                       r_res_c0_ptr->Id());
  AIR_ASSERT(inst->Res_id(0) == x10->Id());
  AIR_ASSERT(inst->Opnd_id(0) == r_res_c0_ptr->Id());
  n3->Append(inst);

  // create instruction for fake-exit
  // ret
  BB_PTR exit = cont.Exit();
  inst          = cont.New_inst(spos, demo::ISA_ID, demo::OPC_RET);
  exit->Append(inst);

  return cont;
}  // Init_container

}  // namespace demo

}  // namespace cg

}  // namespace air
