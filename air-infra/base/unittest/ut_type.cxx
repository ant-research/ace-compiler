//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/base/container.h"
#include "air/base/meta_info.h"
#include "air/base/opcode.h"
#include "air/base/spos.h"
#include "air/base/st.h"
#include "air/base/st_decl.h"
#include "air/base/st_enum.h"
#include "air/core/opcode.h"
#include "gtest/gtest.h"

using namespace air::base;
using namespace air::core;
using namespace air::util;
using namespace testing;

class TEST_TYPE : public ::testing::Test {
protected:
  void        SetUp() override { _glob = new GLOB_SCOPE(0, true); }
  void        TearDown() override { delete _glob; }
  void        Test_array_type_equality();
  void        Test_record_type_equality();
  void        Test_ptr_type_equality();
  GLOB_SCOPE* _glob;
};

void TEST_TYPE::Test_array_type_equality() {
  SPOS     spos      = _glob->Unknown_simple_spos();
  TYPE_PTR f32       = _glob->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
  TYPE_PTR arr_type0 = _glob->New_arr_type("array_type", f32, {64}, spos);
  TYPE_PTR arr_type1 = _glob->New_arr_type("array_type", f32, {64}, spos);
  ASSERT_TRUE(arr_type0->TYPE::Is_compatible_type(arr_type1));

  TYPE_PTR arr_type2 = _glob->New_arr_type("array_type", f32, {32, 2}, spos);
  ASSERT_FALSE(arr_type2->TYPE::Is_compatible_type(arr_type0));

  TYPE_PTR f64       = _glob->Prim_type(PRIMITIVE_TYPE::FLOAT_64);
  TYPE_PTR arr_type3 = _glob->New_arr_type("array_type", f64, {64}, spos);
  ASSERT_FALSE(arr_type3->TYPE::Is_compatible_type(arr_type0));
}

void TEST_TYPE::Test_record_type_equality() {
  SPOS            spos = _glob->Unknown_simple_spos();
  TYPE_PTR        f32  = _glob->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
  RECORD_TYPE_PTR struct0 =
      _glob->New_rec_type(RECORD_KIND::STRUCT, "record", spos);
  FIELD_PTR fld0 = _glob->New_fld("f0", f32, struct0, spos);
  FIELD_PTR fld1 = _glob->New_fld("f1", f32, struct0, spos);
  struct0->Add_fld(fld0->Id());
  struct0->Add_fld(fld1->Id());
  struct0->Set_complete();

  RECORD_TYPE_PTR struct1 =
      _glob->New_rec_type(RECORD_KIND::STRUCT, "record", spos);
  fld0 = _glob->New_fld("f0", f32, struct0, spos);
  fld1 = _glob->New_fld("f1", f32, struct0, spos);
  struct1->Add_fld(fld0->Id());
  struct1->Add_fld(fld1->Id());
  ASSERT_FALSE(struct1->TYPE::Is_compatible_type(struct0));
  struct1->Set_complete();
  ASSERT_TRUE(struct1->TYPE::Is_compatible_type(struct0));

  RECORD_TYPE_PTR union0 =
      _glob->New_rec_type(RECORD_KIND::UNION, "record", spos);
  union0->Add_fld(fld0->Id());
  union0->Add_fld(fld1->Id());
  union0->Set_complete();
  ASSERT_FALSE(union0->TYPE::Is_compatible_type(struct0));
}

void TEST_TYPE::Test_ptr_type_equality() {
  SPOS           spos   = _glob->Unknown_simple_spos();
  TYPE_PTR       f32    = _glob->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
  ARRAY_TYPE_PTR array0 = _glob->New_arr_type("float32_4", f32, {4}, spos);
  ARRAY_TYPE_PTR array1 = _glob->New_arr_type("float32_4", f32, {4}, spos);
  ARRAY_TYPE_PTR array2 = _glob->New_arr_type("float32_4", f32, {8}, spos);
  EXPECT_TRUE(array0->TYPE::Is_compatible_type(array1));

  POINTER_TYPE_PTR ptr0 =
      _glob->New_ptr_type(array0->Id(), POINTER_KIND::FLAT32);
  EXPECT_TRUE(ptr0->TYPE::Is_compatible_type(ptr0));
  POINTER_TYPE_PTR ptr1 =
      _glob->New_ptr_type(array1->Id(), POINTER_KIND::FLAT32);
  EXPECT_TRUE(ptr0->TYPE::Is_compatible_type(ptr1));

  POINTER_TYPE_PTR ptr2 =
      _glob->New_ptr_type(array2->Id(), POINTER_KIND::FLAT32);
  EXPECT_FALSE(ptr0->TYPE::Is_compatible_type(ptr2));
}

TEST_F(TEST_TYPE, array_type_equality) { Test_array_type_equality(); }
TEST_F(TEST_TYPE, record_type_equality) { Test_record_type_equality(); }
TEST_F(TEST_TYPE, ptr_type_equality) { Test_ptr_type_equality(); }