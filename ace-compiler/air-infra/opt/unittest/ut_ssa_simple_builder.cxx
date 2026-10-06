//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/base/container.h"
#include "air/base/container_decl.h"
#include "air/base/meta_info.h"
#include "air/base/spos.h"
#include "air/base/st.h"
#include "air/base/st_decl.h"
#include "air/base/st_enum.h"
#include "air/core/opcode.h"
#include "air/driver/driver_ctx.h"
#include "air/opt/opt.h"
#include "air/opt/ssa_build.h"
#include "air/opt/ssa_decl.h"
#include "air/util/debug.h"
#include "gtest/gtest.h"
#include "ssa_simple_builder.h"

#define RECORD_FLD_NUM 4
using namespace air::opt;

class SSA_BUILDER_TEST : public testing::Test {
public:
  using FUNC_SCOPE  = air::base::FUNC_SCOPE;
  using FUNC_PTR    = air::base::FUNC_PTR;
  using GLOB_SCOPE  = air::base::GLOB_SCOPE;
  using META_INFO   = air::base::META_INFO;
  using TYPE_PTR    = air::base::TYPE_PTR;
  using ARRAY_TYPE  = air::base::ARRAY_TYPE_PTR;
  using RECORD_TYPE = air::base::RECORD_TYPE_PTR;
  using FIELD       = air::base::FIELD_PTR;
  using SPOS        = air::base::SPOS;

  FUNC_SCOPE* Gen_bin_formal_func(const char* func_name);

  RECORD_TYPE Record_type() const { return _record_type; }
  ARRAY_TYPE  Array_type() const { return _array_type; }

protected:
  void SetUp() override {
    _glob_scope = GLOB_SCOPE::Get();
    Register_domains();
    Register_types();
  }

  void TearDown() override { META_INFO::Remove_all(); }

  void        Register_domains();
  void        Register_types();
  ARRAY_TYPE  Gen_array_type(const std::vector<int64_t>& dim);
  RECORD_TYPE Gen_record_type();
  GLOB_SCOPE* Glob_scope(void) const { return _glob_scope; }
  FIELD       Field(uint32_t id) const {
    AIR_ASSERT_MSG(id < RECORD_FLD_NUM, "field id out of range");
    return _field[id];
  }
  void Set_field(uint32_t id, FIELD fld) {
    AIR_ASSERT_MSG(id < RECORD_FLD_NUM, "field id out of range");
    _field[id] = fld;
  }

  GLOB_SCOPE* _glob_scope;
  ARRAY_TYPE  _array_type;
  RECORD_TYPE _record_type;
  FIELD       _field[RECORD_FLD_NUM];
};

void SSA_BUILDER_TEST::Register_domains() {
  META_INFO::Remove_all();
  air::core::Register_core();
}

void SSA_BUILDER_TEST::Register_types() {
  _array_type  = Gen_array_type({20});
  _record_type = Gen_record_type();
}

air::base::ARRAY_TYPE_PTR SSA_BUILDER_TEST::Gen_array_type(
    const std::vector<int64_t>& dim) {
  TYPE_PTR f32_type =
      Glob_scope()->Prim_type(air::base::PRIMITIVE_TYPE::FLOAT_32);
  const char*     arr_name = "f32_10";
  air::base::SPOS spos     = Glob_scope()->Unknown_simple_spos();
  ARRAY_TYPE      array_type =
      Glob_scope()->New_arr_type(arr_name, f32_type, dim, spos);
  return array_type;
}

air::base::RECORD_TYPE_PTR SSA_BUILDER_TEST::Gen_record_type() {
  air::base::SPOS spos   = Glob_scope()->Unknown_simple_spos();
  RECORD_TYPE     record = Glob_scope()->New_rec_type(
      air::base::RECORD_KIND::STRUCT, "test_rec", spos);
  TYPE_PTR s32_type =
      Glob_scope()->Prim_type(air::base::PRIMITIVE_TYPE::INT_S32);
  TYPE_PTR s64_type =
      Glob_scope()->Prim_type(air::base::PRIMITIVE_TYPE::INT_S64);
  TYPE_PTR f32_type =
      Glob_scope()->Prim_type(air::base::PRIMITIVE_TYPE::FLOAT_32);
  TYPE_PTR f64_type =
      Glob_scope()->Prim_type(air::base::PRIMITIVE_TYPE::FLOAT_64);
  FIELD fld = Glob_scope()->New_fld("_int32", s32_type, record, spos);
  Set_field(0, fld);
  fld = Glob_scope()->New_fld("_int64", s64_type, record, spos);
  Set_field(1, fld);
  fld = Glob_scope()->New_fld("_f32", f32_type, record, spos);
  Set_field(2, fld);
  fld = Glob_scope()->New_fld("_f64", f64_type, record, spos);
  Set_field(3, fld);
  record->Add_fld(Field(0)->Id());
  record->Add_fld(Field(1)->Id());
  record->Add_fld(Field(2)->Id());
  record->Add_fld(Field(3)->Id());
  record->Set_complete();
  AIR_ASSERT_MSG(record->Num_fld() == RECORD_FLD_NUM, "Field number incorrect");
  return record;
}

air::base::FUNC_SCOPE* SSA_BUILDER_TEST::Gen_bin_formal_func(
    const char* func_name) {
  air::base::STR_PTR name_ptr = Glob_scope()->New_str(func_name);
  air::base::SPOS    spos     = Glob_scope()->Unknown_simple_spos();

  // gen function
  FUNC_PTR    bin_formal_func = Glob_scope()->New_func(name_ptr, spos);
  FUNC_SCOPE* func_scope      = &Glob_scope()->New_func_scope(bin_formal_func);

  bin_formal_func->Set_parent(Glob_scope()->Comp_env_id());
  // signature of function
  air::base::SIGNATURE_TYPE_PTR sig_type = Glob_scope()->New_sig_type();
  // return type of function
  Glob_scope()->New_ret_param(Array_type(), sig_type);
  Glob_scope()->New_param("arr", Array_type(), sig_type, spos);
  Glob_scope()->New_param("rec", Record_type(), sig_type, spos);
  sig_type->Set_complete();
  Glob_scope()->New_entry_point(sig_type, bin_formal_func, name_ptr, spos);

  return func_scope;
}

TEST_F(SSA_BUILDER_TEST, add_func) {
  FUNC_SCOPE*           func_scope = Gen_bin_formal_func("add_func");
  air::base::FUNC_PTR   func       = func_scope->Owning_func();
  air::base::CONTAINER* cont       = &func_scope->Container();
  air::base::SPOS       spos       = Glob_scope()->Unknown_simple_spos();
  cont->New_func_entry(spos);
  SSA_CONTAINER           ssa_cont(cont);
  air::driver::DRIVER_CTX driver_ctx;
  SSA_BUILDER             ssa_builder(func_scope, &ssa_cont, &driver_ctx);

  // ist arr[0] = 3.14
  TYPE_PTR f32_type = Array_type()->Elem_type();
  TYPE_PTR u32_type =
      Glob_scope()->Prim_type(air::base::PRIMITIVE_TYPE::INT_U32);
  air::base::CONSTANT_PTR cst = Glob_scope()->New_const(
      air::base::CONSTANT_KIND::FLOAT, f32_type, (long double)3.14);
  air::base::NODE_PTR       ldc     = cont->New_ldc(cst, spos);
  air::base::ADDR_DATUM_PTR formal0 = func_scope->Formal(0);
  air::base::NODE_PTR       lda =
      cont->New_lda(formal0, air::base::POINTER_KIND::FLAT32, spos);
  air::base::NODE_PTR arr = cont->New_array(lda, 1, spos);
  air::base::NODE_PTR idx = cont->New_intconst(u32_type, 0, spos);
  cont->Set_array_idx(arr, 0, idx);
  air::base::STMT_PTR  ist_arr0  = cont->New_ist(arr, ldc, spos);
  air::base::STMT_LIST stmt_list = cont->Stmt_list();
  stmt_list.Append(ist_arr0);

  // ild arr[0]
  lda = cont->Clone_node(lda);
  arr = cont->New_array(lda, 1, spos);
  idx = cont->New_intconst(u32_type, 0, spos);
  cont->Set_array_idx(arr, 0, idx);
  air::base::NODE_PTR ild_arr0 = cont->New_ild(arr, spos);
  // preg1 = ild arr[0]
  air::base::PREG_PTR preg1        = func_scope->New_preg(f32_type);
  air::base::STMT_PTR stp1_f32_arr = cont->New_stp(ild_arr0, preg1, spos);
  stmt_list.Append(stp1_f32_arr);

  // res = 0
  air::base::ADDR_DATUM_PTR formal1 = func_scope->Formal(1);
  air::base::NODE_PTR       zero    = cont->New_zero(formal1->Type(), spos);
  air::base::STMT_PTR       st_res  = cont->New_st(zero, formal1, spos);
  cont->Stmt_list().Append(st_res);

  // res._f32 = 3.14;
  ldc                          = cont->New_ldc(cst, spos);
  air::base::STMT_PTR stf_fld3 = cont->New_stf(ldc, formal1, Field(2), spos);
  cont->Stmt_list().Append(stf_fld3);

  // preg2 = res._f32
  air::base::NODE_PTR ldf_fld3     = cont->New_ldf(formal1, Field(2), spos);
  air::base::PREG_PTR preg2        = func_scope->New_preg(f32_type);
  air::base::STMT_PTR stp2_f32_rec = cont->New_stp(ldf_fld3, preg2, spos);
  stmt_list.Append(stp2_f32_rec);

  // preg3 = preg1 + preg2
  air::base::PREG_PTR preg3 = func_scope->New_preg(f32_type);
  air::base::NODE_PTR ldp1  = cont->New_ldp(preg1, spos);
  air::base::NODE_PTR ldp2  = cont->New_ldp(preg2, spos);
  air::base::NODE_PTR add =
      cont->New_bin_arith(air::core::OPC_ADD, f32_type, ldp1, ldp2, spos);
  air::base::STMT_PTR stp3_f32 = cont->New_stp(add, preg3, spos);
  stmt_list.Append(stp3_f32);

  // retv preg3
  air::base::NODE_PTR ldp3 = cont->New_ldp(preg3, spos);
  air::base::STMT_PTR retv = cont->New_retv(ldp3, spos);
  stmt_list.Append(retv);

  ssa_builder.Perform();

  // verify ild & ist
  SSA_SYM_PTR sym = ssa_cont.Node_sym(ist_arr0->Node()->Id());
  ASSERT_TRUE(sym->Is_addr_datum());
  ASSERT_EQ(sym->Type_id(), f32_type->Id());
  ASSERT_EQ(sym->Var_id(), formal0->Id().Value());
  ASSERT_EQ(sym->Id(), ssa_cont.Node_sym_id(ild_arr0->Id()));
  sym = ssa_cont.Node_sym(ild_arr0->Id());
  ASSERT_EQ(sym->Type_id(), f32_type->Id());
  SSA_VER_PTR ver = ssa_cont.Node_ver(ild_arr0->Id());
  ASSERT_EQ(ver->Kind(), VER_DEF_KIND::STMT);
  ASSERT_EQ(ver->Def_stmt_id(), ist_arr0->Id());

  // verify ldf & stf
  CHI_NODE_ID  chi_id = ssa_cont.Node_chi(st_res->Node()->Id());
  CHI_NODE_PTR chi    = ssa_cont.Chi_node(chi_id);
  sym                 = ssa_cont.Node_sym(st_res->Node()->Id());
  ASSERT_EQ(sym->Var_id(), formal1->Id().Value());
  ASSERT_EQ(sym->Index(), SSA_SYM::NO_INDEX);
  ASSERT_EQ(sym->Type_id(), Record_type()->Id());
  sym = ssa_cont.Node_sym(stf_fld3->Node()->Id());
  ASSERT_EQ(chi->Next_id(), air::base::Null_id);
  ASSERT_EQ(chi->Sym_id(), sym->Id());
  ASSERT_EQ(sym->Var_id(), formal1->Id().Value());
  chi_id            = ssa_cont.Node_chi(stf_fld3->Node()->Id());
  SSA_SYM_ID sym_id = ssa_cont.Chi_node(chi_id)->Sym_id();
  sym               = ssa_cont.Sym(sym_id);
  ASSERT_EQ(sym->Type_id(), Record_type()->Id());

  // verify stf_fld3
  sym = ssa_cont.Node_sym(stf_fld3->Node()->Id());
  ASSERT_EQ(sym->Type_id(), f32_type->Id());
  ver = ssa_cont.Node_ver(stf_fld3->Node()->Id());
  ASSERT_EQ(ver->Kind(), VER_DEF_KIND::STMT);
  ASSERT_EQ(ver->Def_stmt_id(), stf_fld3->Id());

  // verify ldf_fld3
  sym = ssa_cont.Node_sym(ldf_fld3->Id());
  ASSERT_EQ(sym->Type_id(), f32_type->Id());
  ver = ssa_cont.Node_ver(ldf_fld3->Id());
  ASSERT_EQ(ver->Kind(), VER_DEF_KIND::STMT);
  ASSERT_EQ(ver->Def_stmt_id(), stf_fld3->Id());

  // verify add operands ldp1 & ldp2
  sym = ssa_cont.Node_sym(ldp1->Id());
  ASSERT_EQ(sym->Type_id(), f32_type->Id());
  ver = ssa_cont.Node_ver(ldp1->Id());
  ASSERT_EQ(ver->Kind(), VER_DEF_KIND::STMT);
  ASSERT_EQ(ver->Def_stmt_id(), stp1_f32_arr->Id());
  sym = ssa_cont.Node_sym(ldp2->Id());
  ASSERT_EQ(sym->Type_id(), f32_type->Id());
  ver = ssa_cont.Node_ver(ldp2->Id());
  ASSERT_EQ(ver->Kind(), VER_DEF_KIND::STMT);
  ASSERT_EQ(ver->Def_stmt_id(), stp2_f32_rec->Id());

  // verify retv
  sym = ssa_cont.Node_sym(ldp3->Id());
  ASSERT_EQ(sym->Type_id(), f32_type->Id());
  ver = ssa_cont.Node_ver(ldp3->Id());
  ASSERT_EQ(ver->Kind(), VER_DEF_KIND::STMT);
  ASSERT_EQ(ver->Def_stmt_id(), stp3_f32->Id());
}
