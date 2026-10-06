//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/base/container.h"
#include "air/base/meta_info.h"
#include "air/base/st.h"
#include "air/base/st_enum.h"
#include "air/core/opcode.h"
#include "air/driver/driver_ctx.h"
#include "air/opt/dead_store_elim.h"
#include "gtest/gtest.h"

using namespace air::base;
using namespace air::core;
using namespace air::opt;

class TEST_DEAD_STORE_ELIM : public ::testing::Test {
protected:
  void SetUp() override {
    META_INFO::Remove_all();
    air::core::Register_core();
    _glob     = GLOB_SCOPE::Get();
    _int_type = _glob->Prim_type(PRIMITIVE_TYPE::INT_S32);
    SPOS     spos(0, 1, 1, 0);
    STR_PTR  name = _glob->New_str("dse_test_func");
    FUNC_PTR func = _glob->New_func(name, spos);
    func->Set_parent(_glob->Comp_env_id());
    SIGNATURE_TYPE_PTR sig = _glob->New_sig_type();
    _glob->New_ret_param(_glob->Prim_type(PRIMITIVE_TYPE::VOID), sig);
    sig->Set_complete();
    _glob->New_entry_point(sig, func, name, spos);
    _fs   = &_glob->New_func_scope(func);
    _cntr = &_fs->Container();
    _cntr->New_func_entry(spos);
  }

  void TearDown() override { META_INFO::Remove_all(); }

  //! Count the number of store statements in the statement list (recursive)
  uint32_t Count_stores(STMT_LIST sl) {
    uint32_t count = 0;
    for (STMT_PTR s = sl.Begin_stmt(); s != sl.End_stmt(); s = s->Next()) {
      NODE_PTR node = s->Node();
      if (node == Null_ptr) continue;
      if (node->Is_st()) count++;
      for (uint32_t i = 0; i < node->Num_child(); ++i) {
        NODE_PTR child = node->Child(i);
        if (child != Null_ptr && child->Is_block()) {
          count += Count_stores(STMT_LIST(child));
        }
      }
    }
    return count;
  }

  //! Create a "result" variable that is addr_passed so DSE cannot remove
  //! stores to it. This simulates an externally observable output.
  ADDR_DATUM_PTR New_live_var(const char* name) {
    SPOS           spos(0, 1, 1, 0);
    ADDR_DATUM_PTR v = _fs->New_var(_int_type, name, spos);
    v->Set_addr_passed();
    return v;
  }

  GLOB_SCOPE* _glob;
  FUNC_SCOPE* _fs;
  CONTAINER*  _cntr;
  TYPE_PTR    _int_type;
};

// DSE-1: st x=3; st x=5; st r=ld x — x_v1 has no uses, first store removed
TEST_F(TEST_DEAD_STORE_ELIM, overwritten_store_removed) {
  SPOS           spos(0, 2, 1, 0);
  ADDR_DATUM_PTR x      = _fs->New_var(_int_type, "x", spos);
  ADDR_DATUM_PTR result = New_live_var("result");
  STMT_LIST      sl     = _cntr->Stmt_list();

  // st x = 3
  sl.Append(_cntr->New_st(_cntr->New_intconst(_int_type, 3, spos), x, spos));
  // st x = 5
  sl.Append(_cntr->New_st(_cntr->New_intconst(_int_type, 5, spos), x, spos));
  // st result = ld x  (result is addr_passed so it stays)
  sl.Append(_cntr->New_st(_cntr->New_ld(x, spos), result, spos));

  EXPECT_EQ(Count_stores(sl), 3u);

  air::driver::DRIVER_CTX ctx;
  EXPECT_EQ(Run_dead_store_elim(_fs, &ctx), 1);

  // First store to x should be removed; 2 stores remain
  EXPECT_EQ(Count_stores(sl), 2u);
}

// DSE-2: st x=3; st r=ld x — x_v1 used by LD, no change
TEST_F(TEST_DEAD_STORE_ELIM, used_store_preserved) {
  SPOS           spos(0, 2, 1, 0);
  ADDR_DATUM_PTR x      = _fs->New_var(_int_type, "x", spos);
  ADDR_DATUM_PTR result = New_live_var("result");
  STMT_LIST      sl     = _cntr->Stmt_list();

  // st x = 3
  sl.Append(_cntr->New_st(_cntr->New_intconst(_int_type, 3, spos), x, spos));
  // st result = ld x
  sl.Append(_cntr->New_st(_cntr->New_ld(x, spos), result, spos));

  air::driver::DRIVER_CTX ctx;
  EXPECT_EQ(Run_dead_store_elim(_fs, &ctx), 0);
  EXPECT_EQ(Count_stores(sl), 2u);
}

// DSE-3: st x=3 (local, never read) — store removed
TEST_F(TEST_DEAD_STORE_ELIM, never_read_local_removed) {
  SPOS           spos(0, 2, 1, 0);
  ADDR_DATUM_PTR x  = _fs->New_var(_int_type, "x", spos);
  STMT_LIST      sl = _cntr->Stmt_list();

  // st x = 3
  sl.Append(_cntr->New_st(_cntr->New_intconst(_int_type, 3, spos), x, spos));

  EXPECT_EQ(Count_stores(sl), 1u);

  air::driver::DRIVER_CTX ctx;
  EXPECT_EQ(Run_dead_store_elim(_fs, &ctx), 1);

  EXPECT_EQ(Count_stores(sl), 0u);
}

// DSE-4: st x=3 where x is addr_passed — store preserved (safety)
TEST_F(TEST_DEAD_STORE_ELIM, addr_passed_preserved) {
  SPOS           spos(0, 2, 1, 0);
  ADDR_DATUM_PTR x = _fs->New_var(_int_type, "x", spos);
  x->Set_addr_passed();
  STMT_LIST sl = _cntr->Stmt_list();

  // st x = 3
  sl.Append(_cntr->New_st(_cntr->New_intconst(_int_type, 3, spos), x, spos));

  air::driver::DRIVER_CTX ctx;
  EXPECT_EQ(Run_dead_store_elim(_fs, &ctx), 0);
  EXPECT_EQ(Count_stores(sl), 1u);
}

// DSE-5: Dead PREG store: stp p=3; stp p=5; st r=ldp p — first STP removed
TEST_F(TEST_DEAD_STORE_ELIM, dead_preg_store_removed) {
  SPOS           spos(0, 2, 1, 0);
  PREG_PTR       p      = _fs->New_preg(_int_type);
  ADDR_DATUM_PTR result = New_live_var("result");
  STMT_LIST      sl     = _cntr->Stmt_list();

  // stp p = 3
  sl.Append(_cntr->New_stp(_cntr->New_intconst(_int_type, 3, spos), p, spos));
  // stp p = 5
  sl.Append(_cntr->New_stp(_cntr->New_intconst(_int_type, 5, spos), p, spos));
  // st result = ldp p
  sl.Append(_cntr->New_st(_cntr->New_ldp(p, spos), result, spos));

  EXPECT_EQ(Count_stores(sl), 3u);

  air::driver::DRIVER_CTX ctx;
  EXPECT_EQ(Run_dead_store_elim(_fs, &ctx), 1);

  // First STP should be removed; 2 stores remain
  EXPECT_EQ(Count_stores(sl), 2u);
}

// DSE-6: Multiple dead stores in sequence — all dead removed
TEST_F(TEST_DEAD_STORE_ELIM, multiple_dead_stores) {
  SPOS           spos(0, 2, 1, 0);
  ADDR_DATUM_PTR x      = _fs->New_var(_int_type, "x", spos);
  ADDR_DATUM_PTR y      = _fs->New_var(_int_type, "y", spos);
  ADDR_DATUM_PTR result = New_live_var("result");
  STMT_LIST      sl     = _cntr->Stmt_list();

  // st x = 1 (dead)
  sl.Append(_cntr->New_st(_cntr->New_intconst(_int_type, 1, spos), x, spos));
  // st x = 2 (dead)
  sl.Append(_cntr->New_st(_cntr->New_intconst(_int_type, 2, spos), x, spos));
  // st y = 3 (dead)
  sl.Append(_cntr->New_st(_cntr->New_intconst(_int_type, 3, spos), y, spos));
  // st y = 4
  sl.Append(_cntr->New_st(_cntr->New_intconst(_int_type, 4, spos), y, spos));
  // st x = 5
  sl.Append(_cntr->New_st(_cntr->New_intconst(_int_type, 5, spos), x, spos));
  // st result = ld x + ld y
  sl.Append(_cntr->New_st(
      _cntr->New_bin_arith(OPC_ADD, _int_type, _cntr->New_ld(x, spos),
                           _cntr->New_ld(y, spos), spos),
      result, spos));

  EXPECT_EQ(Count_stores(sl), 6u);

  air::driver::DRIVER_CTX ctx;
  EXPECT_EQ(Run_dead_store_elim(_fs, &ctx), 3);

  // st x=1, st x=2, st y=3 should be removed; 3 stores remain
  EXPECT_EQ(Count_stores(sl), 3u);
}

// DSE-7: st x=3 where x is addr_saved — store preserved
TEST_F(TEST_DEAD_STORE_ELIM, addr_saved_preserved) {
  SPOS           spos(0, 2, 1, 0);
  ADDR_DATUM_PTR x = _fs->New_var(_int_type, "x", spos);
  x->Set_addr_saved();
  STMT_LIST sl = _cntr->Stmt_list();

  // st x = 3
  sl.Append(_cntr->New_st(_cntr->New_intconst(_int_type, 3, spos), x, spos));

  air::driver::DRIVER_CTX ctx;
  EXPECT_EQ(Run_dead_store_elim(_fs, &ctx), 0);
  EXPECT_EQ(Count_stores(sl), 1u);
}

// DSE-8: Cascading: st y=3; st x=ld y; st x=5; st r=ld x
// Both st y=3 and st x=ld y should be removed
TEST_F(TEST_DEAD_STORE_ELIM, cascading_dead_stores) {
  SPOS           spos(0, 2, 1, 0);
  ADDR_DATUM_PTR x      = _fs->New_var(_int_type, "x", spos);
  ADDR_DATUM_PTR y      = _fs->New_var(_int_type, "y", spos);
  ADDR_DATUM_PTR result = New_live_var("result");
  STMT_LIST      sl     = _cntr->Stmt_list();

  // st y = 3
  sl.Append(_cntr->New_st(_cntr->New_intconst(_int_type, 3, spos), y, spos));
  // st x = ld y
  sl.Append(_cntr->New_st(_cntr->New_ld(y, spos), x, spos));
  // st x = 5
  sl.Append(_cntr->New_st(_cntr->New_intconst(_int_type, 5, spos), x, spos));
  // st result = ld x
  sl.Append(_cntr->New_st(_cntr->New_ld(x, spos), result, spos));

  EXPECT_EQ(Count_stores(sl), 4u);

  air::driver::DRIVER_CTX ctx;
  EXPECT_EQ(Run_dead_store_elim(_fs, &ctx), 2);

  // st y=3 and st x=ld y should be removed; 2 stores remain
  EXPECT_EQ(Count_stores(sl), 2u);
}

// DSE-10: st formal_param = 3 where param is a formal — store preserved
// (Open64 Required_stid: formal parameters are visible to callers)
TEST_F(TEST_DEAD_STORE_ELIM, formal_param_preserved) {
  SPOS           spos(0, 2, 1, 0);
  ADDR_DATUM_PTR param = _fs->New_formal(_int_type->Id(), "formal_param", spos);
  STMT_LIST      sl    = _cntr->Stmt_list();

  // st formal_param = 3
  sl.Append(
      _cntr->New_st(_cntr->New_intconst(_int_type, 3, spos), param, spos));

  EXPECT_EQ(Count_stores(sl), 1u);

  air::driver::DRIVER_CTX ctx;
  EXPECT_EQ(Run_dead_store_elim(_fs, &ctx), 0);

  // Store to formal must be preserved
  EXPECT_EQ(Count_stores(sl), 1u);
}

// DSE-11: st x = ld x where x has no other uses — store preserved (identity)
// (Open64 Required_stid: identity assignments maintain non-zero versions)
TEST_F(TEST_DEAD_STORE_ELIM, identity_assignment_preserved) {
  SPOS           spos(0, 2, 1, 0);
  ADDR_DATUM_PTR x  = _fs->New_var(_int_type, "x", spos);
  STMT_LIST      sl = _cntr->Stmt_list();

  // st x = ld x (identity assignment)
  sl.Append(_cntr->New_st(_cntr->New_ld(x, spos), x, spos));

  EXPECT_EQ(Count_stores(sl), 1u);

  air::driver::DRIVER_CTX ctx;
  EXPECT_EQ(Run_dead_store_elim(_fs, &ctx), 0);

  // Identity assignment must be preserved
  EXPECT_EQ(Count_stores(sl), 1u);
}

// DSE-12: st x=3; st y=ld x; st z=ld y — cascading with real/any split
// x_v1 used by LD (real), y_v1 used by LD (real) — all preserved
TEST_F(TEST_DEAD_STORE_ELIM, real_use_preserves_chain) {
  SPOS           spos(0, 2, 1, 0);
  ADDR_DATUM_PTR x      = _fs->New_var(_int_type, "x", spos);
  ADDR_DATUM_PTR y      = _fs->New_var(_int_type, "y", spos);
  ADDR_DATUM_PTR result = New_live_var("result");
  STMT_LIST      sl     = _cntr->Stmt_list();

  // st x = 3
  sl.Append(_cntr->New_st(_cntr->New_intconst(_int_type, 3, spos), x, spos));
  // st y = ld x
  sl.Append(_cntr->New_st(_cntr->New_ld(x, spos), y, spos));
  // st result = ld y
  sl.Append(_cntr->New_st(_cntr->New_ld(y, spos), result, spos));

  EXPECT_EQ(Count_stores(sl), 3u);

  air::driver::DRIVER_CTX ctx;
  EXPECT_EQ(Run_dead_store_elim(_fs, &ctx), 0);

  // All stores have real uses — all preserved
  EXPECT_EQ(Count_stores(sl), 3u);
}
