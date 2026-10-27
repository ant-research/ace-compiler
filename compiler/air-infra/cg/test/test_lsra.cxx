//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <vector>

#include "air/base/container.h"
#include "air/base/spos.h"
#include "air/base/st.h"
#include "air/base/st_decl.h"
#include "air/cg/cgir_container.h"
#include "air/cg/cgir_decl.h"
#include "air/cg/cgir_enum.h"
#include "air/cg/config.h"
#include "air/cg/isa_core.h"
#include "air/cg/lsra.h"
#include "air/cg/targ_info.h"
#include "air/driver/driver_ctx.h"
#include "test_aircg_demo_isa.h"
#include "test_aircg_demo_isa.inc"

using namespace air::base;
using namespace air::cg;
using namespace air::cg::demo;
using CGIR_CONTAINER = air::cg::CGIR_CONTAINER;
//! @brief Demo IR generator
class DEMO_IR_GEN {
public:
  explicit DEMO_IR_GEN(CGIR_CONTAINER* cont) : _cont(cont) {}
  ~DEMO_IR_GEN() {}

  INST_PTR Gen_mv(OPND_PTR dst, OPND_PTR src, const SPOS& spos) {
    INST_PTR mv =
        _cont->New_inst(spos, air::cg::demo::ISA_ID, air::cg::demo::OPC_ADDI);
    mv->Set_res(0, dst);
    mv->Set_opnd(0, src);
    mv->Set_opnd(1, _cont->New_opnd(0));
    return mv;
  }

private:
  CGIR_CONTAINER* _cont;
};

class TEST_LSRA {
public:
  TEST_LSRA() {
    _glob = new GLOB_SCOPE(0, true);
    _isa  = air::cg::demo::ISA_ID;
    Register_core_isa();
    Create_func_scope();
  }
  ~TEST_LSRA() {
    _glob->Delete_func_scope(_func_scope);
    _func_scope = nullptr;
    _glob->~GLOB_SCOPE();
    _glob = nullptr;
  }
  void Run_test_while_loop_func();
  void Run_test_do_loop_func();
  void Run_fall_through_func();

private:
  void                          Create_func_scope();
  INST_PTR                      Create_phi(CGIR_CONTAINER& cont, PREG_PTR preg,
                                           std::vector<OPND_ID> opnd_def_inst);
  FUNC_SCOPE*                   Func_scope(void) const { return _func_scope; }
  const air::cg::REG_INFO_META* Gpr_info(void) const {
    return TARG_INFO_MGR::Reg_info(REG_CLASS::GPR);
  }
  const ABI_INFO_META* Abi_info(void) const {
    return TARG_INFO_MGR::Abi_info(ABI_ID);
  }
  SPOS Spos() { return _glob->Unknown_simple_spos(); }

  FUNC_SCOPE* _func_scope = nullptr;
  GLOB_SCOPE* _glob       = nullptr;
  TYPE_PTR    _i32;
  uint32_t    _isa;
};

INST_PTR TEST_LSRA::Create_phi(CGIR_CONTAINER& cont, PREG_PTR preg,
                                  std::vector<OPND_ID> opnd) {
  OPND_PTR res = cont.New_preg(GPR);
  res->Set_preg(preg);
  INST_PTR phi = cont.New_phi(Spos(), res->Id(), opnd.size());
  // res->Set_def_inst(phi->Id());
  phi->Set_res(0, res);
  for (uint32_t id = 0; id < opnd.size(); ++id) {
    phi->Set_opnd(id, opnd[id]);
  }
  return phi;
}

void TEST_LSRA::Create_func_scope() {
  _i32          = _glob->Prim_type(PRIMITIVE_TYPE::INT_S32);
  SPOS     spos = _glob->Unknown_simple_spos();
  FUNC_PTR func = _glob->New_func("foo", spos);
  func->Set_parent(_glob->Comp_env_id());
  SIGNATURE_TYPE_PTR sig = _glob->New_sig_type();
  _glob->New_ret_param(_i32, sig);
  PARAM_PTR param0 = _glob->New_param("x", _i32, sig, spos);
  PARAM_PTR param1 = _glob->New_param("y", _i32, sig, spos);
  sig->Set_complete();
  ENTRY_PTR fn_entry = _glob->New_entry_point(sig, func, "foo", spos);
  _func_scope        = &_glob->New_func_scope(func);
}

void TEST_LSRA::Run_test_while_loop_func() {
  CGIR_CONTAINER cont(_func_scope);
  //         |-----------|
  // bb0 -> bb1 -> bb2   bb3 ->
  //         ^------|
  BB_PTR bb0 = cont.New_bb();
  BB_PTR bb1 = cont.New_bb();
  BB_PTR bb2 = cont.New_bb();
  BB_PTR bb3 = cont.New_bb();
  // setup loop info
  air::cg::LOOP_INFO_MGR loop_mgr(&cont);
  LOOP_INFO_PTR          loop_info = loop_mgr.New_loop();
  loop_mgr.Set_header(loop_info, bb1);
  loop_mgr.Set_back(loop_info, bb1);
  loop_mgr.Set_preheader(loop_info, bb0);
  loop_mgr.Set_tail(loop_info, bb3);

  cont.Connect(bb2, bb1);
  cont.Connect(bb0, bb1);
  cont.Connect(bb1, bb2);
  cont.Connect(bb1, bb3);

  cont.Create_fake_entry_exit();
  cont.Set_entry(bb0);
  cont.Set_exit(bb3);
  cont.Set_bb_order(cont.Entry(), bb0);
  cont.Set_bb_order(bb0, bb1);
  cont.Set_bb_order(bb1, bb2);
  cont.Set_bb_order(bb2, bb3);
  cont.Set_bb_order(bb3, cont.Exit());
  // BB0: addi %0  a0  0
  //      addi %1  a1  0
  //      li   %2  1

  // addi  %0.v1  a0  0
  OPND_PTR res   = cont.New_preg(GPR);
  PREG_PTR preg0 = _func_scope->New_preg(_i32);
  res->Set_preg(preg0);
  OPND_PTR opnd = cont.New_opnd(GPR, Abi_info()->Parm_reg(GPR, 0));

  INST_PTR inst0 = cont.New_inst(Spos(), _isa, OPC_ADDI, res->Id(), opnd->Id(),
                                 cont.New_opnd(0)->Id());
  // res->Set_def_inst(inst0->Id());
  bb0->Append(inst0);
  // addi  %1.v1  a1  0
  res            = cont.New_preg(GPR);
  PREG_PTR preg1 = _func_scope->New_preg(_i32);
  res->Set_preg(preg1);
  opnd           = cont.New_opnd(GPR, Abi_info()->Parm_reg(GPR, 1));
  INST_PTR inst1 = cont.New_inst(Spos(), _isa, OPC_ADDI, res->Id(), opnd->Id(),
                                 cont.New_opnd(0)->Id());
  // res->Set_def_inst(inst1->Id());
  bb0->Append(inst1);
  // li   %2.v1  1
  res            = cont.New_preg(GPR);
  PREG_PTR preg2 = _func_scope->New_preg(_i32);
  res->Set_preg(preg2);
  INST_PTR inst2 =
      cont.New_inst(Spos(), _isa, OPC_LI, res->Id(), cont.New_opnd(1)->Id());
  // res->Set_def_inst(inst2->Id());
  bb0->Append(inst2);

  // BB1: %1.v2 = phi(%1.v1, %1.v3)
  //      %2.v2 = phi(%2.v1, %2.v3)
  //      blt  %1.v2  1  BB3
  INST_PTR phi_preg1 = Create_phi(cont, preg1, {inst1->Res_id(0), OPND_ID()});
  INST_PTR phi_preg2 = Create_phi(cont, preg2, {inst2->Res_id(0), OPND_ID()});
  INST_PTR blt       = cont.New_inst(Spos(), _isa, OPC_BLT);
  blt->Set_opnd(0, phi_preg2->Res(0));
  blt->Set_opnd(1, cont.New_opnd(1));
  blt->Set_opnd(2, cont.New_opnd(bb3->Id()));
  bb1->Append(phi_preg1);
  bb1->Append(phi_preg2);
  bb1->Append(blt);

  // BB2: mul  %2.v3  %2.v2  %1.v2
  //      sub  %1.v3  %1.v2  1
  //      j    BB1
  INST_PTR mul = cont.New_inst(Spos(), _isa, OPC_MUL);
  mul->Set_opnd(0, phi_preg2->Res(0));
  mul->Set_opnd(1, phi_preg1->Res(0));
  res = cont.New_preg(GPR);
  res->Set_preg(preg2);
  // res->Set_def_inst(mul->Id());
  mul->Set_res(0, res);
  bb2->Append(mul);
  phi_preg2->Set_opnd(1, res->Id());

  // sub  %1.v3  %1.v2  1
  INST_PTR sub = cont.New_inst(Spos(), _isa, OPC_SUBI);
  sub->Set_opnd(0, phi_preg1->Res(0));
  sub->Set_opnd(1, cont.New_opnd(1));
  res = cont.New_preg(GPR);
  res->Set_preg(preg1);
  // res->Set_def_inst(sub->Id());
  sub->Set_res(0, res);
  bb2->Append(sub);
  phi_preg1->Set_opnd(1, res->Id());

  // j    BB1
  INST_PTR j = cont.New_inst(Spos(), _isa, OPC_J);
  j->Set_opnd(0, cont.New_opnd(bb1->Id()));
  bb2->Append(j);

  // BB3: add  %2.v4  %2.v2  %0.v1
  //      addi a0     %2.v4  0
  INST_PTR add = cont.New_inst(Spos(), _isa, OPC_ADD);
  add->Set_opnd(0, phi_preg2->Res(0));
  add->Set_opnd(1, inst0->Res(0));
  res = cont.New_preg(GPR);
  res->Set_preg(preg2);
  // res->Set_def_inst(add->Id());
  add->Set_res(0, res);
  bb3->Append(add);

  INST_PTR ret = cont.New_inst(Spos(), _isa, OPC_ADDI);
  ret->Set_opnd(0, add->Res(0));
  ret->Set_opnd(1, cont.New_opnd(0));
  res = cont.New_opnd(GPR, Abi_info()->Retv_reg(GPR, 0));
  ret->Set_res(0, res);
  bb3->Append(ret);

  CODE_GEN_CONFIG cfg;
  cfg._trace_detail |= TRACE_LI_BUILD;
  cfg._trace_detail |= TRACE_LSRA;
  air::driver::DRIVER_CTX ctx;
  DEMO_IR_GEN             ir_gen(&cont);
  LSRA                    lsra(&ctx, &cfg, &cont, ISA_ID);
  lsra.Run(&ir_gen);
}

void TEST_LSRA::Run_test_do_loop_func() {
  CGIR_CONTAINER cont(_func_scope, true);
  // bb0 ->  bb1  -> bb2 ->
  //      ^------|
  BB_PTR bb0 = cont.New_bb();
  BB_PTR bb1 = cont.New_bb();
  BB_PTR bb2 = cont.New_bb();
  // setup loop info
  air::cg::LOOP_INFO_MGR loop_mgr(&cont);
  LOOP_INFO_PTR          loop_info = loop_mgr.New_loop();
  loop_mgr.Set_header(loop_info, bb1);
  loop_mgr.Set_back(loop_info, bb1);
  loop_mgr.Set_preheader(loop_info, bb0);
  loop_mgr.Set_tail(loop_info, bb2);

  cont.Connect(bb1, bb1);
  cont.Connect(bb0, bb1);
  cont.Connect(bb1, bb2);

  cont.Set_entry(bb0);
  cont.Set_exit(bb2);
  cont.Set_bb_order(cont.Entry(), bb0);
  cont.Set_bb_order(bb0, bb1);
  cont.Set_bb_order(bb1, bb2);
  cont.Set_bb_order(bb2, cont.Exit());
  // BB0: addi %0.v1  a0  0
  //      addi %1.v1  a1  0
  //      li   %2.v1  1

  // addi  %0.v1  a0  0
  OPND_PTR res   = cont.New_preg(GPR);
  PREG_PTR preg0 = _func_scope->New_preg(_i32);
  res->Set_preg(preg0);
  OPND_PTR opnd = cont.New_opnd(GPR, Abi_info()->Parm_reg(GPR, 0));

  INST_PTR inst0 = cont.New_inst(Spos(), _isa, OPC_ADDI, res->Id(), opnd->Id(),
                                 cont.New_opnd(0)->Id());
  // res->Set_def_inst(inst0->Id());
  bb0->Append(inst0);
  // addi  %1.v1  a1  0
  res            = cont.New_preg(GPR);
  PREG_PTR preg1 = _func_scope->New_preg(_i32);
  res->Set_preg(preg1);
  opnd           = cont.New_opnd(GPR, Abi_info()->Parm_reg(GPR, 1));
  INST_PTR inst1 = cont.New_inst(Spos(), _isa, OPC_ADDI, res->Id(), opnd->Id(),
                                 cont.New_opnd(0)->Id());
  // res->Set_def_inst(inst1->Id());
  bb0->Append(inst1);
  // li   %2.v1  1
  res            = cont.New_preg(GPR);
  PREG_PTR preg2 = _func_scope->New_preg(_i32);
  res->Set_preg(preg2);
  INST_PTR inst2 =
      cont.New_inst(Spos(), _isa, OPC_LI, res->Id(), cont.New_opnd(1)->Id());
  // res->Set_def_inst(inst2->Id());
  bb0->Append(inst2);

  // BB1: %1.v2 = phi(%1.v1, %1.v3)
  //      %2.v2 = phi(%2.v1, %2.v3)
  //      mul  %2.v3  %2.v2  %1.v2
  //      sub  %1.v3  %1.v2  1
  //      blt  %1.v3  1      BB1
  INST_PTR phi_preg1 = Create_phi(cont, preg1, {inst1->Res_id(0), OPND_ID()});
  INST_PTR phi_preg2 = Create_phi(cont, preg2, {inst2->Res_id(0), OPND_ID()});
  bb1->Append(phi_preg1);
  bb1->Append(phi_preg2);
  // mul  %2.v3  %2.v2  %1.v2
  INST_PTR mul = cont.New_inst(Spos(), _isa, OPC_MUL);
  mul->Set_opnd(0, phi_preg2->Res(0));
  mul->Set_opnd(1, phi_preg1->Res(0));
  res = cont.New_preg(GPR);
  res->Set_preg(preg2);
  // res->Set_def_inst(mul->Id());
  mul->Set_res(0, res);
  bb1->Append(mul);
  phi_preg2->Set_opnd(1, res->Id());

  // sub  %1.v3  %1.v2  1
  INST_PTR sub = cont.New_inst(Spos(), _isa, OPC_SUBI);
  sub->Set_opnd(0, phi_preg1->Res(0));
  sub->Set_opnd(1, cont.New_opnd(1));
  res = cont.New_preg(GPR);
  res->Set_preg(preg1);
  // res->Set_def_inst(sub->Id());
  sub->Set_res(0, res);
  bb1->Append(sub);
  phi_preg1->Set_opnd(1, res->Id());

  // blt  %1.v3  1  BB1
  INST_PTR blt = cont.New_inst(Spos(), _isa, OPC_BLT);
  blt->Set_opnd(0, sub->Res(0));
  blt->Set_opnd(1, cont.New_opnd(1));
  blt->Set_opnd(2, cont.New_opnd(bb1->Id()));
  bb1->Append(blt);

  // BB2: add  %2.v4  %2.v3  %0.v1
  //      addi a0     %2.v4  0
  INST_PTR add = cont.New_inst(Spos(), _isa, OPC_ADD);
  add->Set_opnd(0, mul->Res(0));
  add->Set_opnd(1, inst0->Res(0));
  res = cont.New_preg(GPR);
  res->Set_preg(preg2);
  // res->Set_def_inst(add->Id());
  add->Set_res(0, res);
  bb2->Append(add);

  INST_PTR ret = cont.New_inst(Spos(), _isa, OPC_ADDI);
  ret->Set_opnd(0, add->Res(0));
  ret->Set_opnd(1, cont.New_opnd(0));
  res = cont.New_opnd(GPR, Abi_info()->Retv_reg(GPR, 0));
  ret->Set_res(0, res);
  bb2->Append(ret);

  CODE_GEN_CONFIG cfg;
  cfg._trace_detail |= TRACE_LI_BUILD;
  cfg._trace_detail |= TRACE_LSRA;
  air::driver::DRIVER_CTX ctx;
  DEMO_IR_GEN             ir_gen(&cont);
  LSRA                    lsra(&ctx, &cfg, &cont, ISA_ID);
  lsra.Run(&ir_gen);
}

int main(void) {
  TEST_LSRA lsra;
  lsra.Run_test_while_loop_func();
  lsra.Run_test_do_loop_func();
  return 0;
}