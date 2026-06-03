//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <algorithm>
#include <string>

#include "air/base/container.h"
#include "air/base/st.h"
#include "air/core/opcode.h"
#include "air/driver/driver_ctx.h"
#include "air/opt/dfg_builder.h"
#include "air/opt/scc_builder.h"
#include "air/opt/scc_container.h"
#include "air/opt/scc_node.h"
#include "air/opt/ssa_build.h"
#include "air/opt/ssa_container.h"
#include "air/util/debug.h"
#include "air/util/option.h"
#include "gtest/gtest.h"

class DFG_TEST : public testing::Test {
public:
  using FUNC_SCOPE    = air::base::FUNC_SCOPE;
  using FUNC_PTR      = air::base::FUNC_PTR;
  using GLOB_SCOPE    = air::base::GLOB_SCOPE;
  using META_INFO     = air::base::META_INFO;
  using TYPE_PTR      = air::base::TYPE_PTR;
  using TYPE_ID       = air::base::TYPE_ID;
  using ARRAY_TYPE    = air::base::ARRAY_TYPE_PTR;
  using RECORD_TYPE   = air::base::RECORD_TYPE_PTR;
  using FIELD         = air::base::FIELD_PTR;
  using SPOS          = air::base::SPOS;
  using DFG_CONTAINER = air::opt::DFG_CONTAINER;
  using DFG           = DFG_CONTAINER::DFG;

  FUNC_SCOPE* Gen_poly_func(const char* func_name);
  FUNC_SCOPE* Gen_loop_func(const char* func_name);

protected:
  void SetUp() override {
    _glob_scope = GLOB_SCOPE::Get();
    Register_domains();
    Gen_poly_func("Poly_func");
    Gen_loop_func("Loop_func");
  }

  FUNC_SCOPE* Gen_func(const char* func_name);
  void        TearDown() override { META_INFO::Remove_all(); }

  void        Register_domains();
  GLOB_SCOPE* Glob_scope(void) const { return _glob_scope; }
  FUNC_SCOPE* Poly_func(void) const { return _poly_func; }
  FUNC_SCOPE* Loop_func(void) const { return _loop_func; }

  GLOB_SCOPE* _glob_scope;
  FUNC_SCOPE* _poly_func;
  FUNC_SCOPE* _loop_func;
};

void DFG_TEST::Register_domains() {
  META_INFO::Remove_all();
  air::core::Register_core();
}

air::base::FUNC_SCOPE* DFG_TEST::Gen_func(const char* func_name) {
  // gen function
  air::base::SPOS spos       = Glob_scope()->Unknown_simple_spos();
  FUNC_PTR        func       = Glob_scope()->New_func(func_name, spos);
  FUNC_SCOPE*     func_scope = &Glob_scope()->New_func_scope(func);

  func->Set_parent(Glob_scope()->Comp_env_id());

  // signature of function
  air::base::SIGNATURE_TYPE_PTR sig_type = Glob_scope()->New_sig_type();
  // return type of function
  TYPE_PTR u32 = Glob_scope()->Prim_type(air::base::PRIMITIVE_TYPE::INT_U32);
  Glob_scope()->New_ret_param(u32, sig_type);
  Glob_scope()->New_param("x", u32, sig_type, spos);
  sig_type->Set_complete();
  Glob_scope()->New_entry_point(sig_type, func, func_name, spos);
  func_scope->Container().New_func_entry(spos);
  return func_scope;
}

//! @brief gen DFG of function
//! uint32_t Poly_func(uint32_t x) {
//!   uint32_t sum = x;
//!   uint32_t x2 = x * x;
//!   sum = sum + x2;
//!   return sum;
//! }
air::base::FUNC_SCOPE* DFG_TEST::Gen_poly_func(const char* func_name) {
  // gen function
  _poly_func                       = Gen_func(func_name);
  FUNC_SCOPE*           func_scope = _poly_func;
  air::base::CONTAINER& cntr       = func_scope->Container();
  TYPE_PTR u32 = Glob_scope()->Prim_type(air::base::PRIMITIVE_TYPE::INT_U32);
  air::base::SPOS spos = Glob_scope()->Unknown_simple_spos();

  cntr.New_func_entry(spos);
  air::base::STMT_LIST      sl     = cntr.Stmt_list();
  air::base::ADDR_DATUM_PTR formal = func_scope->Formal(0);
  // sum = x;
  air::base::PREG_PTR sum       = func_scope->New_preg(u32);
  air::base::NODE_PTR ld_formal = cntr.New_ld(formal, spos);
  air::base::STMT_PTR stp_sum   = cntr.New_stp(ld_formal, sum, spos);
  sl.Append(stp_sum);

  // gen x^2
  air::base::PREG_PTR x2         = func_scope->New_preg(u32);
  air::base::NODE_PTR ld_formal0 = cntr.New_ld(formal, spos);
  air::base::NODE_PTR ld_formal1 = cntr.New_ld(formal, spos);
  air::base::NODE_PTR square =
      cntr.New_bin_arith(air::core::OPC_MUL, u32, ld_formal0, ld_formal1, spos);
  air::base::STMT_PTR st_x2 = cntr.New_stp(square, x2, spos);
  sl.Append(st_x2);

  air::base::NODE_PTR add_x2 =
      cntr.New_bin_arith(air::core::OPC_ADD, u32, cntr.New_ldp(sum, spos),
                         cntr.New_ldp(x2, spos), spos);
  stp_sum = cntr.New_stp(add_x2, sum, spos);
  sl.Append(stp_sum);

  // return sum
  air::base::STMT_PTR retv = cntr.New_retv(cntr.New_ldp(sum, spos), spos);
  sl.Append(retv);
  return func_scope;
}

//! @brief gen DFG of function
//! uint32_t Poly_func(uint32_t x) {
//!   uint32_t sum = 0;
//!   for (int i = 0; i < 10; ++i) sum += i;
//!   sum = sum * x;
//!   return sum;
//! }
air::base::FUNC_SCOPE* DFG_TEST::Gen_loop_func(const char* func_name) {
  _loop_func                       = Gen_func(func_name);
  FUNC_SCOPE*           func_scope = _loop_func;
  air::base::CONTAINER& cntr       = func_scope->Container();
  TYPE_PTR u32 = Glob_scope()->Prim_type(air::base::PRIMITIVE_TYPE::INT_U32);
  air::base::SPOS      spos = Glob_scope()->Unknown_simple_spos();
  air::base::STMT_LIST sl   = cntr.Stmt_list();

  // sum = 0;
  air::base::NODE_PTR intcst = cntr.New_intconst(u32, 0, spos);
  air::base::PREG_PTR sum    = func_scope->New_preg(u32);
  air::base::STMT_PTR stp    = cntr.New_stp(intcst, sum, spos);
  sl.Append(stp);

  air::base::ADDR_DATUM_PTR iv    = func_scope->New_var(u32, "i", spos);
  air::base::NODE_PTR       init  = cntr.New_intconst(u32, 0, spos);
  air::base::NODE_PTR       ld_iv = cntr.New_ld(iv, spos);
  air::base::NODE_PTR       upper = cntr.New_intconst(u32, 10, spos);
  air::base::NODE_PTR       comp =
      cntr.New_bin_arith(air::core::OPC_LT, u32, ld_iv, upper, spos);
  ld_iv                    = cntr.New_ld(iv, spos);
  air::base::NODE_PTR step = cntr.New_intconst(u32, 1, spos);
  air::base::NODE_PTR incr =
      cntr.New_bin_arith(air::core::OPC_ADD, u32, ld_iv, step, spos);
  air::base::NODE_PTR  block = cntr.New_stmt_block(spos);
  air::base::STMT_LIST loop_body(block);
  air::base::NODE_PTR  ld_sum = cntr.New_ldp(sum, spos);
  ld_iv                       = cntr.New_ld(iv, spos);
  air::base::NODE_PTR add =
      cntr.New_bin_arith(air::core::OPC_ADD, u32, ld_sum, ld_iv, spos);
  air::base::STMT_PTR st_sum = cntr.New_stp(add, sum, spos);
  loop_body.Append(st_sum);
  air::base::STMT_PTR do_loop =
      cntr.New_do_loop(iv, init, comp, incr, block, spos);
  sl.Append(do_loop);

  // sum = sum * x
  ld_sum                   = cntr.New_ldp(sum, spos);
  air::base::NODE_PTR ld_x = cntr.New_ld(func_scope->Formal(0), spos);
  air::base::NODE_PTR mul =
      cntr.New_bin_arith(air::core::OPC_MUL, u32, ld_sum, ld_x, spos);
  st_sum = cntr.New_stp(mul, sum, spos);
  sl.Append(st_sum);

  // ret sum
  ld_sum                   = cntr.New_ldp(sum, spos);
  air::base::STMT_PTR retv = cntr.New_retv(ld_sum, spos);
  sl.Append(retv);

  return func_scope;
}

void Gen_dfg_node(air::base::CONST_NODE_PTR node,
                  air::opt::DFG_CONTAINER*  dfg_cntr,
                  air::opt::DFG_NODE_ID&    dfg_node_id) {
  const air::opt::SSA_CONTAINER* ssa_cntr = dfg_cntr->Ssa_cntr();
  // 1. gen DFG node
  air::opt::DFG_NODE_PTR dfg_node;
  switch (node->Opcode()) {
    case air::core::OPC_ST:
    case air::core::OPC_STP: {
      air::opt::SSA_VER_PTR ver = ssa_cntr->Node_ver(node->Id());
      air::opt::SSA_SYM_PTR sym = ssa_cntr->Node_sym(node->Id());
      dfg_node = dfg_cntr->Get_node(ver->Id(), sym->Type_id(), 1);
      break;
    }
    case air::core::OPC_RETV: {
      air::base::TYPE_ID type = node->Child(0)->Rtype_id();
      dfg_node                = dfg_cntr->Get_node(node->Id(), type, 1);
      dfg_cntr->Add_exit(dfg_node->Id());
      break;
    }
    case air::core::OPC_IDNAME:
    case air::core::OPC_LDP:
    case air::core::OPC_LD: {
      air::opt::SSA_VER_ID ver = ssa_cntr->Node_ver_id(node->Id());
      dfg_node                 = dfg_cntr->Get_node(ver, node->Rtype_id(), 0);
      break;
    }
    default: {
      AIR_ASSERT(node->Has_rtype());
      dfg_node =
          dfg_cntr->Get_node(node->Id(), node->Rtype_id(), node->Num_child());
      break;
    }
  }
  dfg_node_id = dfg_node->Id();

  // 2. handle child nodes
  for (uint32_t id = 0; id < node->Num_child(); ++id) {
    air::base::NODE_PTR   child = node->Child(id);
    air::opt::DFG_NODE_ID child_node_id(air::base::Null_id);
    Gen_dfg_node(child, dfg_cntr, child_node_id);
    dfg_node->Set_opnd(id, child_node_id);
    air::opt::DFG_NODE_PTR child_node = dfg_cntr->Node(child_node_id);
    // check if operand is set successfully
    ASSERT_TRUE(child_node->Is_succ(dfg_node_id)) << "Failed in add succ node";
    ASSERT_TRUE(dfg_node->Is_opnd(id, child_node_id)) << "Failed in set opnd";
  }
}

TEST_F(DFG_TEST, poly) {
  FUNC_SCOPE*           func_scope = Poly_func();
  air::base::CONTAINER& cntr       = func_scope->Container();
  // 1. build ssa
  air::opt::SSA_CONTAINER ssa_cntr(&cntr);
  air::driver::DRIVER_CTX driver_ctx;
  air::opt::SSA_BUILDER   ssa_builder(func_scope, &ssa_cntr, &driver_ctx);
  ssa_builder.Perform();

  // 2. build dfg
  DFG_CONTAINER       dfg_cntr(&ssa_cntr);
  DFG                 dfg(&dfg_cntr);
  air::base::NODE_PTR idname = func_scope->Container().Entry_node()->Child(0);
  AIR_ASSERT(idname->Opcode() == air::core::OPC_IDNAME);
  air::opt::DFG_NODE_ID formal_dfg_node;
  Gen_dfg_node(idname, &dfg_cntr, formal_dfg_node);
  dfg.Container().Set_entry(0, formal_dfg_node);

  air::base::STMT_LIST sl(cntr.Entry_node()->Last_child());
  for (air::base::STMT_PTR stmt = sl.Begin_stmt(); stmt != sl.End_stmt();
       stmt                     = stmt->Next()) {
    air::opt::DFG_NODE_ID stmt_dfg_node;
    Gen_dfg_node(stmt->Node(), &dfg_cntr, stmt_dfg_node);
    ASSERT_NE(stmt_dfg_node, air::base::Null_id) << "invalid DFG node";
  }

  // 3. check DFG node/edge relation
  for (DFG_CONTAINER::NODE_ITER iter = dfg_cntr.Begin_node();
       iter != dfg_cntr.End_node(); ++iter) {
    for (uint32_t id = 0; id < iter->Opnd_cnt(); ++id) {
      air::opt::DFG_NODE_PTR opnd = iter->Opnd(id);
      if (opnd == air::opt::DFG_NODE_PTR()) continue;
      ASSERT_TRUE(opnd->Is_succ(iter->Id()));
    }
  }

  // 4. print dfg
  std::string dot_file_name(Poly_func()->Owning_func()->Name()->Char_str());
  dot_file_name += ".dot";
  dfg.Print_dot(dot_file_name.c_str());

  // 5. check DFG node and edge number
  uint32_t node_size = dfg_cntr.Node_cnt();
  ASSERT_EQ(node_size, 7) << "node number is incorrect";
  uint32_t edge_size = dfg_cntr.Edge_cnt();
  ASSERT_EQ(edge_size, 7) << "edge number is incorrect";
}

TEST_F(DFG_TEST, dfg_builder_poly) {
  FUNC_SCOPE*           func_scope = Poly_func();
  air::base::CONTAINER& cntr       = func_scope->Container();
  // 1. build ssa
  air::opt::SSA_CONTAINER ssa_cntr(&cntr);
  air::driver::DRIVER_CTX driver_ctx;
  air::opt::SSA_BUILDER   ssa_builder(func_scope, &ssa_cntr, &driver_ctx);
  ssa_builder.Perform();

  // 2. build dfg
  DFG_CONTAINER           dfg_cntr(&ssa_cntr);
  DFG                     dfg(&dfg_cntr);
  air::opt::DFG_BUILDER<> dfg_builder(&dfg, func_scope, &ssa_cntr, {});
  dfg_builder.Perform();

  // 3. check DFG node/edge relation
  for (DFG_CONTAINER::NODE_ITER iter = dfg_cntr.Begin_node();
       iter != dfg_cntr.End_node(); ++iter) {
    for (uint32_t id = 0; id < iter->Opnd_cnt(); ++id) {
      air::opt::DFG_NODE_PTR opnd = iter->Opnd(id);
      if (opnd == air::opt::DFG_NODE_PTR()) continue;
      ASSERT_TRUE(opnd->Is_succ(iter->Id()));
    }
  }

  // 4. output DFG to dot file
  std::string dot_file_name(func_scope->Owning_func()->Name()->Char_str());
  dot_file_name += ".dot";
  dfg.Print_dot(dot_file_name.c_str());

  // 5. check DFG node and edge number
  uint32_t node_size = dfg_cntr.Node_cnt();
  ASSERT_EQ(node_size, 7) << "node number is incorrect";
  uint32_t edge_size = dfg_cntr.Edge_cnt();
  ASSERT_EQ(edge_size, 7) << "edge number is incorrect";
}

TEST_F(DFG_TEST, dfg_builder_loop) {
  FUNC_SCOPE*           func_scope = Loop_func();
  air::base::CONTAINER& cntr       = func_scope->Container();
  // 1. build ssa
  air::opt::SSA_CONTAINER ssa_cntr(&cntr);
  air::driver::DRIVER_CTX driver_ctx;
  air::opt::SSA_BUILDER   ssa_builder(func_scope, &ssa_cntr, &driver_ctx);
  ssa_builder.Perform();

  // 2. build dfg
  DFG_CONTAINER           dfg_cntr(&ssa_cntr);
  DFG                     dfg(&dfg_cntr);
  air::opt::DFG_BUILDER<> dfg_builder(&dfg, func_scope, &ssa_cntr, {});
  dfg_builder.Perform();

  // 3. check DFG node/edge relation
  for (DFG_CONTAINER::NODE_ITER iter = dfg_cntr.Begin_node();
       iter != dfg_cntr.End_node(); ++iter) {
    for (uint32_t id = 0; id < iter->Opnd_cnt(); ++id) {
      air::opt::DFG_NODE_PTR opnd = iter->Opnd(id);
      if (opnd == air::opt::DFG_NODE_PTR()) continue;
      ASSERT_TRUE(opnd->Is_succ(iter->Id()));
    }
  }

  // 4. output DFG to dot file
  std::string dot_file_name(func_scope->Owning_func()->Name()->Char_str());
  dot_file_name += ".dot";
  dfg.Print_dot(dot_file_name.c_str());

  // 5. check DFG node and edge number
  uint32_t node_size = dfg_cntr.Node_cnt();
  ASSERT_EQ(node_size, 11) << "node number is incorrect";
  uint32_t edge_size = dfg_cntr.Edge_cnt();
  ASSERT_EQ(edge_size, 11) << "edge number is incorrect";
}

TEST_F(DFG_TEST, scc_of_loop_dfg) {
  FUNC_SCOPE*           func_scope = Loop_func();
  air::base::CONTAINER& cntr       = func_scope->Container();
  // 1. build ssa
  air::opt::SSA_CONTAINER ssa_cntr(&cntr);
  air::driver::DRIVER_CTX driver_ctx;
  air::opt::SSA_BUILDER   ssa_builder(func_scope, &ssa_cntr, &driver_ctx);
  ssa_builder.Perform();

  // 2. build dfg
  DFG_CONTAINER           dfg_cntr(&ssa_cntr);
  DFG                     dfg(&dfg_cntr);
  air::opt::DFG_BUILDER<> dfg_builder(&dfg, func_scope, &ssa_cntr, {});
  dfg_builder.Perform();

  // 3. build scc of dfg
  air::opt::SCC_CONTAINER            scc_cntr;
  air::opt::SCC_CONTAINER::SCC_GRAPH scc_graph(&scc_cntr);
  air::opt::SCC_BUILDER<DFG>         scc_builder(&dfg, &scc_graph);
  scc_builder.Perform();

  // 4. output GRAPH of scc nodes to dot file
  scc_cntr.Print_dot(&dfg_cntr, "Loop_func_scc.dot");

  // 5. check SCC node/edge relation, and max element count
  uint32_t max_elem_num = 0;
  for (air::opt::SCC_CONTAINER::NODE_ITER iter = scc_cntr.Begin_node();
       iter != scc_cntr.End_node(); ++iter) {
    for (air::opt::SCC_NODE::SCC_NODE_ITER pred_iter = iter->Begin_pred();
         pred_iter != iter->End_pred(); ++pred_iter) {
      ASSERT_TRUE(pred_iter->Has_succ(iter->Id()));
    }
    max_elem_num = std::max(max_elem_num, iter->Elem_cnt());
  }
  ASSERT_EQ(max_elem_num, 3) << "the max element count for SCC is incorrect";

  // 6. check DFG node and edge number
  uint32_t node_size = scc_cntr.Node_cnt();
  ASSERT_EQ(node_size, 9) << "node number is incorrect";
  uint32_t edge_size = scc_cntr.Edge_cnt();
  ASSERT_EQ(edge_size, 8) << "edge number is incorrect";
}