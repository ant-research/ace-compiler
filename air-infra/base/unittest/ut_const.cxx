//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "air/base/st.h"
#include "gtest/gtest.h"

using namespace air::base;
using namespace air::util;

TEST(Constant, new_array_const) {
  GLOB_SCOPE* glob      = GLOB_SCOPE::Get();
  STR_PTR     type_name = glob->New_str("tensor_array");
  TYPE_PTR    etype     = glob->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
  ARB_PTR     arb       = glob->New_arb(0, 0, 6, 1);
  SPOS        spos      = glob->Unknown_simple_spos();

  TYPE_PTR     var_type = glob->New_arr_type(type_name, etype, arb, spos);
  float        data[6]  = {1.0, 2.0, 3.0, 4.0, 5.0, 6.0};
  CONSTANT_PTR cst =
      glob->New_const(CONSTANT_KIND::ARRAY, var_type, data, sizeof(data));
  EXPECT_EQ(cst->Array_byte_len(), sizeof(data));
  const float* cptr = cst->Array_ptr<float>();
  EXPECT_EQ(cptr[0], 1.0);
  EXPECT_EQ(cptr[1], 2.0);
  EXPECT_EQ(cptr[2], 3.0);
  EXPECT_EQ(cptr[3], 4.0);
  EXPECT_EQ(cptr[4], 5.0);
  EXPECT_EQ(cptr[5], 6.0);
  EXPECT_EQ(cptr[0], cst->Array_elem<float>(0));
  EXPECT_EQ(cptr[1], cst->Array_elem<float>(1));
  EXPECT_EQ(cptr[2], cst->Array_elem<float>(2));
  EXPECT_EQ(cptr[3], cst->Array_elem<float>(3));
  EXPECT_EQ(cptr[4], cst->Array_elem<float>(4));
  EXPECT_EQ(cptr[5], cst->Array_elem<float>(5));
}

TEST(Constant, muti_dimension_array_const) {
  // ARRAY of type W * H * C [4 * 3 * 2]
  GLOB_SCOPE* glob     = GLOB_SCOPE::Get();
  STR_PTR     type_str = glob->New_str("multi_demension_array");
  TYPE_PTR    etype    = glob->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
  SPOS        spos     = glob->Unknown_simple_spos();
  ARB_PTR     arb_c    = glob->New_arb(0, 0, 2, 1);
  ARB_PTR     arb_h    = glob->New_arb(1, 0, 3, 1);
  ARB_PTR     arb_w    = glob->New_arb(2, 0, 4, 1);
  arb_c->Set_next(arb_h->Id());
  arb_h->Set_next(arb_w->Id());
  TYPE_PTR w_type        = glob->New_arr_type(type_str, etype, arb_c, spos);
  float    data[2][3][4] = {
      {
       {1.0, 2.0, 3.0, 4.0},
       {5.0, 6.0, 7.0, 8.0},
       {9.0, 10.0, 11.0, 12.0},
       },
      {
       {13.0, 14.0, 15.0, 16.0},
       {17.0, 18.0, 19.0, 20.0},
       {21.0, 22.0, 23.0, 24.0},
       },
  };
  CONSTANT_PTR cst =
      glob->New_const(CONSTANT_KIND::ARRAY, w_type, data, sizeof(data));
  EXPECT_EQ(cst->Array_byte_len(), sizeof(data));
  const float* cptr = cst->Array_ptr<float>();
  const float* dref = (const float*)data;
  for (int i = 0; i < 2 * 3 * 4; ++i) {
    EXPECT_EQ(cptr[i], dref[i]);
    EXPECT_EQ(cptr[i], cst->Array_elem<float>(i));
  }
}

TEST(Constant, new_array_type_dim_const) {
  // ARRAY of type W * H * C [28 * 28 * 10]
  GLOB_SCOPE*          glob      = GLOB_SCOPE::Get();
  STR_PTR              type_name = glob->New_str("multi_array_vec");
  TYPE_PTR             etype     = glob->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
  SPOS                 spos      = glob->Unknown_simple_spos();
  int64_t              uppers[]  = {10, 28, 28};
  std::vector<int64_t> dims(uppers, uppers + 3);
  TYPE_PTR w_type = glob->New_arr_type(type_name, etype, dims, spos);
  float    data[28 * 28 * 10];
  for (int i = 0; i < 28 * 28 * 10; ++i) {
    data[i] = 1.0 + i;
  }
  CONSTANT_PTR cst =
      glob->New_const(CONSTANT_KIND::ARRAY, w_type, data, sizeof(data));
  EXPECT_EQ(cst->Array_byte_len(), sizeof(data));
  const float* cptr = cst->Array_ptr<float>();
  for (int i = 0; i < 28 * 28 * 10; ++i) {
    EXPECT_EQ(cptr[i], data[i]);
    EXPECT_EQ(cptr[i], cst->Array_elem<float>(i));
  }
}

TEST(Constant, large_const_array) {
  // ARRAY of type W * H * C [75 * 6144]
  GLOB_SCOPE* glob = GLOB_SCOPE::Get();

  // clone
  GLOB_SCOPE* new_glob = new GLOB_SCOPE(glob->Id(), true);
  ASSERT_TRUE(new_glob != nullptr);
  new_glob->Clone(*glob);

  STR_PTR              type_name = new_glob->New_str("lenet_array_vec");
  TYPE_PTR             etype    = new_glob->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
  SPOS                 spos     = new_glob->Unknown_simple_spos();
  int64_t              uppers[] = {75, 6144};
  std::vector<int64_t> dims(uppers, uppers + 2);
  TYPE_PTR w_type = new_glob->New_arr_type(type_name, etype, dims, spos);
  float    data[75 * 6144];
  for (int i = 0; i < 75 * 6144; ++i) {
    data[i] = 1.0 + i % 100 * 0.1;
  }
  CONSTANT_PTR cst =
      new_glob->New_const(CONSTANT_KIND::ARRAY, w_type, data, sizeof(data));

  ASSERT_EQ(cst->Array_byte_len(), sizeof(data));
  const float* cptr = cst->Array_ptr<float>();
  for (int i = 0; i < 75 * 6144; ++i) {
    ASSERT_EQ(cptr[i], data[i]);
    ASSERT_EQ(cptr[i], cst->Array_elem<float>(i));
  }

  // clone
  GLOB_SCOPE* new_glob1 = new GLOB_SCOPE(new_glob->Id(), true);
  ASSERT_TRUE(new_glob1 != nullptr);
  new_glob1->Clone(*new_glob);
  CONSTANT_PTR cst1 = new_glob1->Constant(cst->Id());

  ASSERT_EQ(cst1->Array_byte_len(), sizeof(data));
  const float* cptr1 = cst1->Array_ptr<float>();
  ASSERT_NE(cptr, cptr1);
  for (int i = 0; i < 75 * 6144; ++i) {
    ASSERT_EQ(cptr1[i], data[i]);
    ASSERT_EQ(cptr1[i], cst1->Array_elem<float>(i));
  }
}

TEST(Constant, ext_const_file) {
  char                 cst1[]  = "Constant1";
  char                 cst2[]  = "Constant2";
  char                 fname[] = "ext_const.txt";
  size_t               len     = strlen(cst1);
  GLOB_SCOPE*          glob    = new GLOB_SCOPE(5, true);
  FILE_PTR             fout    = glob->New_file(fname, LANG::WO_CONST);
  TYPE_PTR             etype   = glob->Prim_type(PRIMITIVE_TYPE::INT_S8);
  std::vector<int64_t> dim(1, len);
  TYPE_PTR             ctype =
      glob->New_arr_type("write_type", etype, dim, glob->Unknown_simple_spos());
  CONSTANT_PTR cst_ptr1 =
      glob->New_const(CONSTANT_KIND::EXT_FILE, ctype, fout, cst1, len);
  CONSTANT_PTR cst_ptr2 =
      glob->New_const(CONSTANT_KIND::EXT_FILE, ctype, fout, cst2, len);
  EXPECT_FALSE(strcmp(cst_ptr1->Ext_file()->File_name()->Char_str(), fname));
  EXPECT_EQ(cst_ptr1->Ext_ofst(), 0);
  EXPECT_EQ(cst_ptr1->Ext_size(), len);
  EXPECT_FALSE(strcmp(cst_ptr2->Ext_file()->File_name()->Char_str(), fname));
  EXPECT_EQ(cst_ptr2->Ext_ofst(), len);
  EXPECT_EQ(cst_ptr2->Ext_size(), len);
  delete glob;
  // done write, now read
  glob         = new GLOB_SCOPE(6, true);
  FILE_PTR fin = glob->New_file(fname, LANG::RO_CONST);
  etype        = glob->Prim_type(PRIMITIVE_TYPE::INT_S8);
  ctype =
      glob->New_arr_type("read_type", etype, dim, glob->Unknown_simple_spos());
  cst_ptr1 =
      glob->New_const(CONSTANT_KIND::EXT_FILE, ctype, fin, (uint64_t)0, len);
  cst_ptr2 = glob->New_const(CONSTANT_KIND::EXT_FILE, ctype, fin, len, len);
  EXPECT_FALSE(strcmp(cst_ptr1->Ext_file()->File_name()->Char_str(), fname));
  EXPECT_EQ(cst_ptr1->Ext_ofst(), 0);
  EXPECT_EQ(cst_ptr1->Ext_size(), len);
  EXPECT_FALSE(strcmp(cst_ptr2->Ext_file()->File_name()->Char_str(), fname));
  EXPECT_EQ(cst_ptr2->Ext_ofst(), len);
  EXPECT_EQ(cst_ptr2->Ext_size(), len);
  delete glob;
  std::remove(fname);
}
