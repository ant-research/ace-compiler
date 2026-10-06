//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

// Phase 1 verification: hand-built-IR *execution* test for the ckks IR ->
// Python (non-ant) backend. Complements the Phase 1.0-1.4 *emit-gate* tests
// (which only `py_compile`): this test EMITS a module exercising the CKKS ops
// the ONNX frontend cannot produce (`sub_*`, `neg`, `mod_switch`, `upscale`,
// `mul_mono`, `mul_scalar`, `ldp/stp`, ...), then EXECUTES it under the
// `fake_rt` plaintext runtime (via `run_py_exec.py`) and checks the plaintext
// output against a hand-computed reference -- while `fake_rt` asserts the real
// CKKS level/scale discipline on every op.
//
// The executable chain (provider OPENFHE; output = input * input):
//   z   = zero_ciph()                         [zero_ciph; unfixed-scale zero]
//   a   = add_ciph(input, z)                  [add_ciph; zero-operand waiver]
//   b   = sub_ciph(input, input)              [sub_ciph; level+scale match]
//   n   = neg_ciph(b)                         [neg]
//   c   = conjugate_ciph(a)                   [conjugate]
//   d   = copy_ciph(c)                        [copy via ST+LD]
//   e   = add_ciph(d, n)                      [add_ciph; 1==1]
//   r   = rotate_ciph(e, 0)                   [rotate]
//   p   = encode_float(2.0, 1, 1, 0)          [encode_float; Plain(0,1)]
//   msc = mul_scalar(e, 3.0)                  [mul_scalar; sd+1]   (dead-end)
//   mm  = mul_mono_ciph(e, p)                 [mul_mono; sd+sd_p]  (dead-end)
//   mp  = mul_plain(e, p)                     [mul_plain]          (dead-end)
//   up  = upscale_ciph(e, 60)                 [upscale; sd+1]      (dead-end)
//   ms  = mod_switch(e)                       [mod_switch; L-1]    (dead-end)
//   rm  = raise_mod(e, 1)                     [raise_mod; lev=1]   (dead-end)
//   lv  = level(e); sc = sc_degree(e)         [level / sc_degree] (dead-end)
//   tN  = copy_ciph(e)  via STP(preg)          [stp + copy_ciph-on-preg]
//   x   = add_ciph(tN, n)                     [ldp + add_ciph]     (dead-end)
//   m2  = mul_ciph(e, e)                      [mul_ciph; ->cipher3 sd2]
//   rn  = relin(m2)                           [relin; cipher3->cipher]
//   output = rescale_ciph(rn)                 [rescale; sd>1, L-1] -> program
//   set_output_data("output", 0, output); return True
//
// Plaintext: e = input (the additive gap-op chain reduces to v + 0 = v), so
// output = e * e = v * v. With input slots [2.0, 2.0], expected output [4,4].
// fake_rt also confirms no level/scale invariant is violated (strict).
//
// CTX_PARAM_ANA acceptance of the "no-producer" ops (neg/upscale/mul_mono) in
// the program entry is verified empirically at build time, per the plan's
// "empirically verify CTX_PARAM_ANA acceptance at implementation time" note.
// If any asserts, this test is the
// detector and the op is moved to emit-only coverage + fake_rt unit semantics
// (every op's plaintext/state semantics are pinned in test_fake_rt.py).

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "air/core/opcode.h"
#include "fhe/core/ctx_param_ana.h"
#include "fhe/ckks/ckks_opcode.h"  // CKKS_DOMAIN::ID, CKKS_OPERATOR::*
#include "fhe/ckks/ckks2py_config.h"
#include "fhe/ckks/ckks2py_driver.h"
#include "fhe/test/gen_ckks_ir.h"
#include "nn/core/attr.h"  // nn::core::ATTR::RNUM (rotate-index attr)

#ifndef FHE_PY_VERIFY_DIR
#define FHE_PY_VERIFY_DIR "./py_verify"
#endif
// Python interpreter: baked by CMake to the same ${PYTHON_EXECUTABLE} the
// other phase1_verify tests use (so the C++ std::system invocations, the .sh
// wrapper, and the pytest add_test all agree -- avoiding interpreter divergence
// where one path has onnx/pytest and another does not).
#ifndef FHE_PYTHON
#define FHE_PYTHON "python3"
#endif

using namespace air::base;
using namespace fhe::core;
using namespace fhe::ckks;
using namespace fhe::poly::test;

static bool Contains(const std::string& hay, const std::string& needle) {
  return hay.find(needle) != std::string::npos;
}

// Build a 1-kid CKKS custom node `dst = name(visit(child0))` stored via ST.
// (Rotate is built separately via Gen_rotate, which also sets the RNUM attr
// ctx_param_ana requires; the ops used here do not carry that attr.)
static STMT_PTR Gen_cust_una(CKKS_IR_GEN& ir_gen, CONTAINER* cntr,
                             ADDR_DATUM_PTR dst, ADDR_DATUM_PTR arg,
                             air::base::OPCODE opc, TYPE_PTR rtype) {
  SPOS spos = ir_gen.Spos();
  STMT_LIST sl = cntr->Stmt_list();
  NODE_PTR node = cntr->New_cust_node(opc, rtype, spos);
  node->Set_child(0, cntr->New_ld(arg, spos));
  STMT_PTR st = cntr->New_st(node, dst, spos);
  sl.Append(st);
  return st;
}

int main(int argc, char** argv) {
  LOWER_CTX   fhe_ctx;
  CKKS_IR_GEN ir_gen(fhe_ctx);

  CONTAINER*  cntr   = ir_gen.Main_container();
  SPOS        spos   = ir_gen.Spos();
  GLOB_SCOPE* glob   = ir_gen.Glob();
  TYPE_PTR    s32_ty = glob->Prim_type(PRIMITIVE_TYPE::INT_S32);
  TYPE_PTR    u32_ty = glob->Prim_type(PRIMITIVE_TYPE::INT_U32);
  TYPE_PTR    f32_ty = glob->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
  STMT_LIST   sl     = cntr->Stmt_list();

  ADDR_DATUM_PTR v_input  = ir_gen.Input_var();
  ADDR_DATUM_PTR v_output = ir_gen.Output_var();

  // z = zero_ciph()  (cipher ST of a ZERO node -> Handle_st -> zero_ciph)
  ADDR_DATUM_PTR v_z = ir_gen.Gen_ciph_var("z");
  sl.Append(cntr->New_st(cntr->New_zero(ir_gen.Ciph_ty(), spos), v_z, spos));

  // a = add_ciph(input, z)
  ADDR_DATUM_PTR v_a = ir_gen.Gen_ciph_var("a");
  ir_gen.Gen_add(cntr, v_a, v_input, v_z);

  // b = sub_ciph(input, input)  (hand-built CKKS SUB; no Gen_sub helper)
  ADDR_DATUM_PTR v_b = ir_gen.Gen_ciph_var("b");
  {
    NODE_PTR lhs = cntr->New_ld(v_input, spos);
    NODE_PTR rhs = cntr->New_ld(v_input, spos);
    NODE_PTR sub = cntr->New_bin_arith(
        air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID, fhe::ckks::CKKS_OPERATOR::SUB),
        lhs->Rtype(), lhs, rhs, spos);
    sl.Append(cntr->New_st(sub, v_b, spos));
  }

  // n = neg_ciph(b)  (NEG, no-producer op provided per the MVP table)
  ADDR_DATUM_PTR v_n = ir_gen.Gen_ciph_var("n");
  Gen_cust_una(ir_gen, cntr, v_n, v_b,
               air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID, fhe::ckks::CKKS_OPERATOR::NEG),
               ir_gen.Ciph_ty());

  // c = conjugate_ciph(a)
  ADDR_DATUM_PTR v_c = ir_gen.Gen_ciph_var("c");
  Gen_cust_una(ir_gen, cntr, v_c, v_a,
               air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                      fhe::ckks::CKKS_OPERATOR::CONJUGATE),
               ir_gen.Ciph_ty());

  // d = copy_ciph(c)  (cipher ST of an LD -> Handle_st -> copy_ciph)
  ADDR_DATUM_PTR v_d = ir_gen.Gen_ciph_var("d");
  sl.Append(cntr->New_st(cntr->New_ld(v_c, spos), v_d, spos));

  // e = add_ciph(d, n)
  ADDR_DATUM_PTR v_e = ir_gen.Gen_ciph_var("e");
  ir_gen.Gen_add(cntr, v_e, v_d, v_n);

  // r = rotate_ciph(e, 0)  (rotate-index attr required by ctx_param_ana)
  ADDR_DATUM_PTR v_r = ir_gen.Gen_ciph_var("r");
  {
    int32_t rnum = 0;
    STMT_PTR rot = ir_gen.Gen_rotate(cntr, v_r, v_e, 0);
    rot->Node()->Child(0)->Set_attr(nn::core::ATTR::RNUM, &rnum, 1);
  }

  // p = encode_float(2.0, 1, 1, 0)  (scale=1 -> sd 1; level=0)
  ADDR_DATUM_PTR v_p = ir_gen.Gen_plain_var("p");
  {
    CONSTANT_PTR cst = glob->New_const(CONSTANT_KIND::FLOAT, f32_ty,
                                       (long double)2.0);
    NODE_PTR ldc  = cntr->New_ldc(cst, spos);
    NODE_PTR enc  = cntr->New_cust_node(
        air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID, fhe::ckks::CKKS_OPERATOR::ENCODE),
        ir_gen.Plain_ty(), spos);
    enc->Set_child(0, ldc);
    enc->Set_child(1, cntr->New_intconst(u32_ty, 1, spos));  // len
    enc->Set_child(2, cntr->New_intconst(u32_ty, 1, spos));  // scale (sd)
    enc->Set_child(3, cntr->New_intconst(u32_ty, 0, spos));  // level
    sl.Append(cntr->New_st(enc, v_p, spos));
  }

  // msc = mul_scalar(e, 3.0)  (MUL with prim FLOAT 2nd operand -> mul_scalar)
  ADDR_DATUM_PTR v_msc = ir_gen.Gen_ciph_var("msc");
  {
    CONSTANT_PTR cst = glob->New_const(CONSTANT_KIND::FLOAT, f32_ty,
                                       (long double)3.0);
    NODE_PTR ldc  = cntr->New_ldc(cst, spos);
    NODE_PTR ld_e = cntr->New_ld(v_e, spos);
    NODE_PTR mul  = cntr->New_bin_arith(
        air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID, fhe::ckks::CKKS_OPERATOR::MUL),
        ld_e->Rtype(), ld_e, ldc, spos);
    sl.Append(cntr->New_st(mul, v_msc, spos));
  }

  // mm = mul_mono_ciph(e, p)  (MUL_MONO, no-producer op)
  ADDR_DATUM_PTR v_mm = ir_gen.Gen_ciph_var("mm");
  {
    NODE_PTR node = cntr->New_cust_node(
        air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
               fhe::ckks::CKKS_OPERATOR::MUL_MONO),
        ir_gen.Ciph_ty(), spos);
    node->Set_child(0, cntr->New_ld(v_e, spos));
    node->Set_child(1, cntr->New_ld(v_p, spos));
    sl.Append(cntr->New_st(node, v_mm, spos));
  }

  // mp = mul_plain(e, p)  (MUL with plain 2nd operand; Gen_mul_node -> ciph)
  ADDR_DATUM_PTR v_mp = ir_gen.Gen_ciph_var("mp");
  ir_gen.Gen_mul(cntr, v_mp, v_e, v_p);

  // up = upscale_ciph(e, 60)  (UPSCALE, no-producer op; 2 kids)
  ADDR_DATUM_PTR v_up = ir_gen.Gen_ciph_var("up");
  {
    NODE_PTR node = cntr->New_cust_node(
        air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
               fhe::ckks::CKKS_OPERATOR::UPSCALE),
        ir_gen.Ciph_ty(), spos);
    node->Set_child(0, cntr->New_ld(v_e, spos));
    node->Set_child(1, cntr->New_intconst(s32_ty, 60, spos));  // bits
    sl.Append(cntr->New_st(node, v_up, spos));
  }

  // ms = mod_switch(e)  (MODSWITCH, 1 kid)
  ADDR_DATUM_PTR v_ms = ir_gen.Gen_ciph_var("ms");
  Gen_cust_una(ir_gen, cntr, v_ms, v_e,
               air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                      fhe::ckks::CKKS_OPERATOR::MODSWITCH),
               ir_gen.Ciph_ty());

  // rm = raise_mod(e, 1)
  ADDR_DATUM_PTR v_rm = ir_gen.Gen_ciph_var("rm");
  ir_gen.Gen_raise_mod(cntr, v_rm, v_e, 1);

  // lv = level(e); sc = sc_degree(e)  (LEVEL/SCALE -> s32 scalar)
  ADDR_DATUM_PTR v_lv = ir_gen.Gen_int_var("lv");
  Gen_cust_una(ir_gen, cntr, v_lv, v_e,
               air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID, fhe::ckks::CKKS_OPERATOR::LEVEL),
               s32_ty);
  ADDR_DATUM_PTR v_sc = ir_gen.Gen_int_var("sc");
  Gen_cust_una(ir_gen, cntr, v_sc, v_e,
               air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID, fhe::ckks::CKKS_OPERATOR::SCALE),
               s32_ty);

  // tN = copy_ciph(e) via STP(preg); x = add_ciph(tN, n) via LDP  (CORE stp/ldp)
  ADDR_DATUM_PTR v_x = ir_gen.Gen_ciph_var("x");
  {
    PREG_PTR preg_t = ir_gen.Main_scope()->New_preg(ir_gen.Ciph_ty());
    sl.Append(cntr->New_stp(cntr->New_ld(v_e, spos), preg_t, spos));
    NODE_PTR ldp_t = cntr->New_ldp(preg_t, spos);
    NODE_PTR ld_n  = cntr->New_ld(v_n, spos);
    NODE_PTR add_x = cntr->New_bin_arith(
        air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID, fhe::ckks::CKKS_OPERATOR::ADD),
        ldp_t->Rtype(), ldp_t, ld_n, spos);
    sl.Append(cntr->New_st(add_x, v_x, spos));
  }

  // m2 = mul_ciph(e, e)  (-> cipher3); rn = relin(m2); output = rescale_ciph(rn)
  ADDR_DATUM_PTR v_m2 = ir_gen.Gen_ciph3_var("m2");
  ir_gen.Gen_mul(cntr, v_m2, v_e, v_e);
  ADDR_DATUM_PTR v_rn = ir_gen.Gen_ciph_var("rn");
  ir_gen.Gen_relin(cntr, v_rn, v_m2);
  ir_gen.Gen_rescale(cntr, v_output, v_rn);  // output = rescale_ciph(rn)

  // retv(output) + CKKS context-param analysis (CTX_PARAM_ANA on program entry)
  ir_gen.Append_output();

  // Drive the Python backend (skip POLY_DRIVER lowering; Flatten first, as the
  // non-ant CKKS2PY_PASS::Run does).
  std::ofstream  of("test_allops.py");
  CKKS2PY_CONFIG cfg;
  cfg._provider = fhe::core::PROVIDER::OPENFHE;  // emitted 'provider' -> "LIB_OPENFHE"
  CKKS2PY_DRIVER poly2py(of, fhe_ctx, cfg);
  GLOB_SCOPE*    out_glob = ir_gen.Glob();
  out_glob = poly2py.Flatten(out_glob);
  CKKS2PY_VISITOR visitor(poly2py.Ctx());
  poly2py.Run(out_glob, visitor);
  of.close();

  std::ifstream ifs("test_allops.py");
  std::stringstream ss;
  ss << ifs.rdbuf();
  std::string py = ss.str();
  std::cout << "---- test_allops.py ----\n" << py << "----\n";

  // Substring assertions: every gap CKKS op the ONNX frontend cannot produce.
  bool ok = true;
  ok &= Contains(py, "def Main_graph():");
  ok &= Contains(py, "zero_ciph()");
  ok &= Contains(py, "= add_ciph(");
  ok &= Contains(py, "= sub_ciph(");
  ok &= Contains(py, "= neg_ciph(");
  ok &= Contains(py, "= conjugate_ciph(");
  ok &= Contains(py, "= copy_ciph(");
  ok &= Contains(py, "= rotate_ciph(");
  ok &= Contains(py, "= encode_float(");
  ok &= Contains(py, "= mul_scalar(");
  ok &= Contains(py, "= mul_mono_ciph(");
  ok &= Contains(py, "= mul_plain(");
  ok &= Contains(py, "= upscale_ciph(");
  ok &= Contains(py, "= mod_switch(");
  ok &= Contains(py, "= raise_mod(");
  ok &= Contains(py, "= level(");
  ok &= Contains(py, "= sc_degree(");
  ok &= Contains(py, "= mul_ciph(");     // cipher3-producing mul
  ok &= Contains(py, "= relin(");
  ok &= Contains(py, "= rescale_ciph(");
  // Body-specific: the Emit_global_include preamble COMMENT also contains
  // `set_output_data(` (it lists the API), so a bare substring would be
  // satisfied by the preamble and false-pass a body-store regression. Anchor
  // to the body call, which uses the program output name ("output"), not the
  // preamble's placeholder `name`.
  ok &= Contains(py, "set_output_data(\"output\"");
  ok &= Contains(py, "return True");
  // No C-only syntax in the Main_graph body.
  {
    std::string line;
    std::istringstream iss(py);
    bool in_main = false;
    while (std::getline(iss, line)) {
      if (line.find("def Main_graph():") != std::string::npos) {
        in_main = true;
        continue;
      }
      if (in_main && line.find("def ") != std::string::npos) in_main = false;
      if (!in_main) continue;
      size_t i = line.find_first_not_of(" \t");
      if (i == std::string::npos) continue;
      if (line[i] == '#') continue;
      if (line.find_first_of(";&{}") != std::string::npos) {
        ok = false;
        std::cerr << "FAIL: C syntax leaked in Main_graph body line: " << line
                  << "\n";
        break;
      }
    }
  }
  if (!ok) {
    std::cerr << "FAIL: generated Python missing expected substrings or leaked "
                 "C syntax\n";
    return 1;
  }

  // Authoritative syntax gate (FHE_PYTHON: the CMake-resolved interpreter,
  // same one the .sh wrapper + pytest add_test use).
  const char* fhe_py = FHE_PYTHON;
  {
    std::string guard = std::string("command -v \"") + fhe_py +
                        "\" >/dev/null 2>&1";
    if (std::system(guard.c_str()) != 0) {
      std::cerr << "FAIL: " << fhe_py << " not found (needed for py_compile + "
                   "run_py_exec)\n";
      return 1;
    }
  }
  int rc = std::system((std::string(fhe_py) + " -m py_compile test_allops.py").c_str());
  if (rc != 0) {
    std::cerr << "FAIL: " << fhe_py << " -m py_compile test_allops.py rc=" << rc
              << "\n";
    return 1;
  }

  // Execution gate: inject fake_rt and run Main_graph under run_py_exec.py.
  // Spec: input [2.0, 2.0] -> e = input -> output = e*e = [4.0, 4.0].
  //
  // run_py_exec.py (+ the rtlib_py it imports) lives in the SOURCE tree
  // (FHE_PY_VERIFY_DIR) and is NOT installed, so this gate only runs where
  // the source checkout is present -- i.e. `ctest` in the build container.
  // cti's unit gate runs the installed /opt/avhc/test binary in an
  // install-only container (no source tree); there the harness is absent, so
  // skip the execution gate (emit + py_compile above already passed) and
  // return success. The full level/scale execution gate is exercised by
  // `ctest` in the build container.
  {
    std::string run_py_exec = std::string(FHE_PY_VERIFY_DIR) + "/run_py_exec.py";
    std::ifstream harness(run_py_exec);
    if (!harness) {
      std::cout << "SKIP: " << run_py_exec << " not found (install-only env; "
                << "execution gate runs under ctest in the build container). "
                << "Emit + py_compile already passed.\n";
      return 0;
    }
  }
  {
    std::ofstream spec("test_allops.spec.json");
    spec << "{\"inputs\":{\"input\":[2.0,2.0]},"
         << "\"expected\":{\"output\":[4.0,4.0]},\"strict\":true}";
  }
  std::string cmd = std::string(fhe_py) + " \"" FHE_PY_VERIFY_DIR
                    "/run_py_exec.py\" test_allops.py test_allops.spec.json";
  rc = std::system(cmd.c_str());
  if (rc != 0) {
    std::cerr << "FAIL: run_py_exec rc=" << rc
              << " (emitted .py did not execute cleanly under fake_rt -- "
              << "a level/scale violation or output mismatch)\n";
    return 1;
  }

  std::cout << "PASS: test_allops.py emits + py_compiles + executes under "
               "fake_rt (output [4,4] = input*input, no level/scale "
               "violations)\n";

  // Remaining CORE ops (ist/ild cipher-array store/load, logical
  // and/or/bnot, ZERO/ONE literal nodes, call) are NOT separately
  // exercised here: a second CKKS_IR_GEN re-registers domains (asserts),
  // and New_zero/New_one reject int rtypes (!Is_int()). Their emitter
  // paths exist in ir2py_core/ir2py_handler and are covered by code
  // inspection + ctrlflow py_compile (for/if/comparisons); adding them is
  // a follow-up. The CKKS ops that matter for level/scale discipline are
  // all executed above + unit-pinned in test_fake_rt.py.
  return 0;
}
