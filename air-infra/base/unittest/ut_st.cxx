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
#include "air/base/st.h"
#include "air/core/opcode.h"
#include "gtest/gtest.h"

using namespace air::base;
using namespace air::core;
using namespace air::util;
using namespace testing;

class TEST_GLOB_SCOPE : public ::testing::Test {
protected:
  void SetUp() override { _glob = new GLOB_SCOPE(0, true); }
  void TearDown() override { delete _glob; }
  void Run_test_new_pointer_type();
  void Run_test_new_array_type();
  void Run_test_new_record_type();
  void Run_test_record_size1();
  void Run_test_record_size2();
  void Run_test_record_size3();
  void Run_test_record_size4();
  void Run_test_record_size5();
  void Run_test_record_size6();
  void Run_test_record_size7();
  void Run_test_record_size8();
  void Run_test_union_size();
  void Run_test_record_union_size();
  void Run_test_new_signature_type();
  void Run_test_new_multi_dimen_array_type();
  void Run_test_new_multi_dimen_array_type_vec();
  void Run_test_new_var_sym();
  void Run_test_new_func_sym();
  void Run_test_new_entry_sym();
  void Run_test_clone();
  void Run_test_init_targ_info();

  GLOB_SCOPE* _glob;
};

void TEST_GLOB_SCOPE::Run_test_new_pointer_type() {
  PRIM_TYPE_PTR    ptr_to = _glob->Prim_type(PRIMITIVE_TYPE::INT_S8);
  POINTER_TYPE_PTR ptr =
      _glob->New_ptr_type(ptr_to->Id(), POINTER_KIND::FLAT64);

  std::string expected("TYP[0x12] pointer(64flat), domain(TYP[0])\n");
  std::string result(ptr->To_str());
  EXPECT_EQ(result, expected);
}

void TEST_GLOB_SCOPE::Run_test_new_array_type() {
  TYPE_PTR etype    = _glob->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
  ARB_PTR  arb      = _glob->New_arb(0, 0, 5, 1);
  SPOS     spos     = _glob->Unknown_simple_spos();
  TYPE_PTR var_type = _glob->New_arr_type("tensor_array", etype, arb, spos);

  std::string expected(
      "TYP[0x12] array(\"tensor_array\"), element(TYP[0x8])\n"
      "  ARB[0] lower(const:0), upper(const:0x5), stride(const:0x1)\n"
      "    dimension(0), first(yes), last(yes), next(ARB[0xffffffff])\n");
  std::string result(var_type->To_str());
  EXPECT_EQ(result, expected);
}

void TEST_GLOB_SCOPE::Run_test_new_record_type() {
  SPOS            spos = _glob->Unknown_simple_spos();
  RECORD_TYPE_PTR rec_type =
      _glob->New_rec_type(RECORD_KIND::STRUCT, "tensor_record", spos);
  STR_PTR   name_fld1 = _glob->New_str("fld1");
  TYPE_PTR  fld1_type = _glob->Prim_type(PRIMITIVE_TYPE::INT_U64);
  FIELD_PTR fld1      = _glob->New_fld(name_fld1, fld1_type, rec_type, spos);
  rec_type->Add_fld(fld1->Id());
  STR_PTR   name_fld2 = _glob->New_str("fld2");
  TYPE_PTR  fld2_type = _glob->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
  FIELD_PTR fld2      = _glob->New_fld(name_fld2, fld2_type, rec_type, spos);
  rec_type->Add_fld(fld2->Id());
  rec_type->Set_complete();

  std::string expected(
      "TYP[0x12] record(\"tensor_record\"), complete(yes)\n"
      "  FLD[0] \"fld1\", TYP[0x7](primitive,\"uint64_t\")\n"
      "  FLD[0x1] \"fld2\", TYP[0x8](primitive,\"float32_t\")\n");
  std::string result(rec_type->To_str());
  EXPECT_EQ(result, expected);
}

void TEST_GLOB_SCOPE::Run_test_record_size1() {
  SPOS spos = _glob->Unknown_simple_spos();
  /*
    struct {
      int32_t a;
      int64_t b;
    };
  */
  RECORD_TYPE_PTR rec_type =
      _glob->New_rec_type(RECORD_KIND::STRUCT, "record", spos);
  TYPE_PTR  int32_type = _glob->Prim_type(PRIMITIVE_TYPE::INT_S32);
  TYPE_PTR  int64_type = _glob->Prim_type(PRIMITIVE_TYPE::INT_S64);
  FIELD_PTR fld1       = _glob->New_fld("a", int32_type, rec_type, spos);
  FIELD_PTR fld2       = _glob->New_fld("b", int64_type, rec_type, spos);
  rec_type->Add_fld(fld1->Id());
  rec_type->Add_fld(fld2->Id());
  rec_type->Set_complete();
  EXPECT_EQ(rec_type->Byte_size(), 16);
  EXPECT_EQ(rec_type->Alignment(), DATA_ALIGN::BIT64);
  EXPECT_EQ(fld1->Ofst(), 0);
  EXPECT_EQ(fld1->Bit_size(), 32);
  EXPECT_EQ(fld2->Ofst(), 64);
  EXPECT_EQ(fld2->Bit_size(), 64);
}

void TEST_GLOB_SCOPE::Run_test_record_size2() {
  SPOS spos = _glob->Unknown_simple_spos();
  /*
    struct {
      int64_t a;
      int32_t b;
    };
  */
  RECORD_TYPE_PTR rec_type =
      _glob->New_rec_type(RECORD_KIND::STRUCT, "record", spos);
  TYPE_PTR  int32_type = _glob->Prim_type(PRIMITIVE_TYPE::INT_S32);
  TYPE_PTR  int64_type = _glob->Prim_type(PRIMITIVE_TYPE::INT_S64);
  FIELD_PTR fld1       = _glob->New_fld("a", int64_type, rec_type, spos);
  FIELD_PTR fld2       = _glob->New_fld("b", int32_type, rec_type, spos);
  rec_type->Add_fld(fld1->Id());
  rec_type->Add_fld(fld2->Id());
  rec_type->Set_complete();
  EXPECT_EQ(rec_type->Byte_size(), 16);
  EXPECT_EQ(rec_type->Alignment(), DATA_ALIGN::BIT64);
  EXPECT_EQ(fld1->Ofst(), 0);
  EXPECT_EQ(fld1->Bit_size(), 64);
  EXPECT_EQ(fld2->Ofst(), 64);
  EXPECT_EQ(fld2->Bit_size(), 32);
}

void TEST_GLOB_SCOPE::Run_test_record_size3() {
  SPOS spos = _glob->Unknown_simple_spos();
  /*
    struct {
      int32_t a;
      int32_t b;
      int64_t c;
    };
  */
  RECORD_TYPE_PTR rec_type =
      _glob->New_rec_type(RECORD_KIND::STRUCT, "record", spos);
  TYPE_PTR  int32_type = _glob->Prim_type(PRIMITIVE_TYPE::INT_S32);
  TYPE_PTR  int64_type = _glob->Prim_type(PRIMITIVE_TYPE::INT_S64);
  FIELD_PTR fld1       = _glob->New_fld("a", int32_type, rec_type, spos);
  FIELD_PTR fld2       = _glob->New_fld("b", int32_type, rec_type, spos);
  FIELD_PTR fld3       = _glob->New_fld("c", int64_type, rec_type, spos);
  rec_type->Add_fld(fld1->Id());
  rec_type->Add_fld(fld2->Id());
  rec_type->Add_fld(fld3->Id());
  rec_type->Set_complete();
  EXPECT_EQ(rec_type->Byte_size(), 16);
  EXPECT_EQ(rec_type->Alignment(), DATA_ALIGN::BIT64);
  EXPECT_EQ(fld1->Ofst(), 0);
  EXPECT_EQ(fld1->Bit_size(), 32);
  EXPECT_EQ(fld2->Ofst(), 32);
  EXPECT_EQ(fld2->Bit_size(), 32);
  EXPECT_EQ(fld3->Ofst(), 64);
  EXPECT_EQ(fld3->Bit_size(), 64);
}

void TEST_GLOB_SCOPE::Run_test_record_size4() {
  SPOS spos = _glob->Unknown_simple_spos();
  /*
    struct {
      int32_t a;
      int64_t b;
      int32_t c;
    };
  */
  RECORD_TYPE_PTR rec_type =
      _glob->New_rec_type(RECORD_KIND::STRUCT, "record", spos);
  TYPE_PTR  int32_type = _glob->Prim_type(PRIMITIVE_TYPE::INT_S32);
  TYPE_PTR  int64_type = _glob->Prim_type(PRIMITIVE_TYPE::INT_S64);
  FIELD_PTR fld1       = _glob->New_fld("a", int32_type, rec_type, spos);
  FIELD_PTR fld2       = _glob->New_fld("b", int64_type, rec_type, spos);
  FIELD_PTR fld3       = _glob->New_fld("c", int32_type, rec_type, spos);
  rec_type->Add_fld(fld1->Id());
  rec_type->Add_fld(fld2->Id());
  rec_type->Add_fld(fld3->Id());
  rec_type->Set_complete();
  EXPECT_EQ(rec_type->Byte_size(), 24);
  EXPECT_EQ(rec_type->Alignment(), DATA_ALIGN::BIT64);
  EXPECT_EQ(fld1->Ofst(), 0);
  EXPECT_EQ(fld1->Bit_size(), 32);
  EXPECT_EQ(fld2->Ofst(), 64);
  EXPECT_EQ(fld2->Bit_size(), 64);
  EXPECT_EQ(fld3->Ofst(), 128);
  EXPECT_EQ(fld3->Bit_size(), 32);
}

void TEST_GLOB_SCOPE::Run_test_record_size5() {
  SPOS spos = _glob->Unknown_simple_spos();
  /*
    struct {
      int64_t a;
      int32_t b;
      int32_t c;
    };
  */
  RECORD_TYPE_PTR rec_type =
      _glob->New_rec_type(RECORD_KIND::STRUCT, "record", spos);
  TYPE_PTR  int32_type = _glob->Prim_type(PRIMITIVE_TYPE::INT_S32);
  TYPE_PTR  int64_type = _glob->Prim_type(PRIMITIVE_TYPE::INT_S64);
  FIELD_PTR fld1       = _glob->New_fld("a", int64_type, rec_type, spos);
  FIELD_PTR fld2       = _glob->New_fld("b", int32_type, rec_type, spos);
  FIELD_PTR fld3       = _glob->New_fld("c", int32_type, rec_type, spos);
  rec_type->Add_fld(fld1->Id());
  rec_type->Add_fld(fld2->Id());
  rec_type->Add_fld(fld3->Id());
  rec_type->Set_complete();
  EXPECT_EQ(rec_type->Byte_size(), 16);
  EXPECT_EQ(rec_type->Alignment(), DATA_ALIGN::BIT64);
  EXPECT_EQ(fld1->Ofst(), 0);
  EXPECT_EQ(fld1->Bit_size(), 64);
  EXPECT_EQ(fld2->Ofst(), 64);
  EXPECT_EQ(fld2->Bit_size(), 32);
  EXPECT_EQ(fld3->Ofst(), 96);
  EXPECT_EQ(fld3->Bit_size(), 32);
}

void TEST_GLOB_SCOPE::Run_test_record_size6() {
  SPOS spos = _glob->Unknown_simple_spos();
  /*
    struct {
      int32_t a : 1;
      int32_t b : 1;
      int64_t c;
    };
  */
  RECORD_TYPE_PTR rec_type =
      _glob->New_rec_type(RECORD_KIND::STRUCT, "record", spos);
  TYPE_PTR  int32_type = _glob->Prim_type(PRIMITIVE_TYPE::INT_S32);
  TYPE_PTR  int64_type = _glob->Prim_type(PRIMITIVE_TYPE::INT_S64);
  FIELD_PTR fld1 =
      _glob->New_fld("a", int32_type, rec_type, 1, DATA_ALIGN::BIT1, spos);
  FIELD_PTR fld2 =
      _glob->New_fld("b", int32_type, rec_type, 1, DATA_ALIGN::BIT1, spos);
  FIELD_PTR fld3 = _glob->New_fld("c", int64_type, rec_type, spos);
  rec_type->Add_fld(fld1->Id());
  rec_type->Add_fld(fld2->Id());
  rec_type->Add_fld(fld3->Id());
  rec_type->Set_complete();
  EXPECT_EQ(rec_type->Byte_size(), 16);
  EXPECT_EQ(rec_type->Alignment(), DATA_ALIGN::BIT64);
  EXPECT_EQ(fld1->Ofst(), 0);
  EXPECT_EQ(fld1->Bit_size(), 1);
  EXPECT_EQ(fld2->Ofst(), 1);
  EXPECT_EQ(fld2->Bit_size(), 1);
  EXPECT_EQ(fld3->Ofst(), 64);
  EXPECT_EQ(fld3->Bit_size(), 64);
}

void TEST_GLOB_SCOPE::Run_test_record_size7() {
  SPOS spos = _glob->Unknown_simple_spos();
  /*
    struct {
      int32_t a : 1;
      int64_t b;
      int32_t c : 1;
    };
  */
  RECORD_TYPE_PTR rec_type =
      _glob->New_rec_type(RECORD_KIND::STRUCT, "record", spos);
  TYPE_PTR  int32_type = _glob->Prim_type(PRIMITIVE_TYPE::INT_S32);
  TYPE_PTR  int64_type = _glob->Prim_type(PRIMITIVE_TYPE::INT_S64);
  FIELD_PTR fld1 =
      _glob->New_fld("a", int32_type, rec_type, 1, DATA_ALIGN::BIT1, spos);
  FIELD_PTR fld2 = _glob->New_fld("b", int64_type, rec_type, spos);
  FIELD_PTR fld3 =
      _glob->New_fld("c", int32_type, rec_type, 1, DATA_ALIGN::BIT1, spos);
  rec_type->Add_fld(fld1->Id());
  rec_type->Add_fld(fld2->Id());
  rec_type->Add_fld(fld3->Id());
  rec_type->Set_complete();
  EXPECT_EQ(rec_type->Byte_size(), 24);
  EXPECT_EQ(rec_type->Alignment(), DATA_ALIGN::BIT64);
  EXPECT_EQ(fld1->Ofst(), 0);
  EXPECT_EQ(fld1->Bit_size(), 1);
  EXPECT_EQ(fld2->Ofst(), 64);
  EXPECT_EQ(fld2->Bit_size(), 64);
  EXPECT_EQ(fld3->Ofst(), 128);
  EXPECT_EQ(fld3->Bit_size(), 1);
}

void TEST_GLOB_SCOPE::Run_test_record_size8() {
  SPOS spos = _glob->Unknown_simple_spos();
  /*
    struct {
      int64_t a;
      int32_t b : 1;
      int32_t c : 1;
    };
  */
  RECORD_TYPE_PTR rec_type =
      _glob->New_rec_type(RECORD_KIND::STRUCT, "record", spos);
  TYPE_PTR  int32_type = _glob->Prim_type(PRIMITIVE_TYPE::INT_S32);
  TYPE_PTR  int64_type = _glob->Prim_type(PRIMITIVE_TYPE::INT_S64);
  FIELD_PTR fld1       = _glob->New_fld("a", int64_type, rec_type, spos);
  FIELD_PTR fld2 =
      _glob->New_fld("b", int32_type, rec_type, 1, DATA_ALIGN::BIT1, spos);
  FIELD_PTR fld3 =
      _glob->New_fld("c", int32_type, rec_type, 1, DATA_ALIGN::BIT1, spos);
  rec_type->Add_fld(fld1->Id());
  rec_type->Add_fld(fld2->Id());
  rec_type->Add_fld(fld3->Id());
  rec_type->Set_complete();
  EXPECT_EQ(rec_type->Byte_size(), 16);
  EXPECT_EQ(rec_type->Alignment(), DATA_ALIGN::BIT64);
  EXPECT_EQ(fld1->Ofst(), 0);
  EXPECT_EQ(fld1->Bit_size(), 64);
  EXPECT_EQ(fld2->Ofst(), 64);
  EXPECT_EQ(fld2->Bit_size(), 1);
  EXPECT_EQ(fld3->Ofst(), 65);
  EXPECT_EQ(fld3->Bit_size(), 1);
}

void TEST_GLOB_SCOPE::Run_test_union_size() {
  SPOS spos = _glob->Unknown_simple_spos();
  /*
    union {
      int32_t a;
      int32_t b;
      int64_t c;
    };
  */
  RECORD_TYPE_PTR rec_type =
      _glob->New_rec_type(RECORD_KIND::UNION, "union", spos);
  TYPE_PTR  int32_type = _glob->Prim_type(PRIMITIVE_TYPE::INT_S32);
  TYPE_PTR  int64_type = _glob->Prim_type(PRIMITIVE_TYPE::INT_S64);
  FIELD_PTR fld1       = _glob->New_fld("a", int32_type, rec_type, spos);
  FIELD_PTR fld2       = _glob->New_fld("b", int32_type, rec_type, spos);
  FIELD_PTR fld3       = _glob->New_fld("c", int64_type, rec_type, spos);
  rec_type->Add_fld(fld1->Id());
  rec_type->Add_fld(fld2->Id());
  rec_type->Add_fld(fld3->Id());
  rec_type->Set_complete();
  EXPECT_EQ(rec_type->Byte_size(), 8);
  EXPECT_EQ(rec_type->Alignment(), DATA_ALIGN::BIT64);
  EXPECT_EQ(fld1->Ofst(), 0);
  EXPECT_EQ(fld1->Bit_size(), 32);
  EXPECT_EQ(fld2->Ofst(), 0);
  EXPECT_EQ(fld2->Bit_size(), 32);
  EXPECT_EQ(fld3->Ofst(), 0);
  EXPECT_EQ(fld3->Bit_size(), 64);
}

void TEST_GLOB_SCOPE::Run_test_record_union_size() {
  SPOS spos = _glob->Unknown_simple_spos();
  /*
    struct {
      int32_t a;
      union {
        int32_t b;
        int64_t c;
      } d;
      int32_t e;
    };
  */
  RECORD_TYPE_PTR uni_type =
      _glob->New_rec_type(RECORD_KIND::UNION, "union", spos);
  TYPE_PTR  int32_type = _glob->Prim_type(PRIMITIVE_TYPE::INT_S32);
  TYPE_PTR  int64_type = _glob->Prim_type(PRIMITIVE_TYPE::INT_S64);
  FIELD_PTR ufld1      = _glob->New_fld("b", int32_type, uni_type, spos);
  FIELD_PTR ufld2      = _glob->New_fld("c", int64_type, uni_type, spos);
  uni_type->Add_fld(ufld1->Id());
  uni_type->Add_fld(ufld2->Id());
  uni_type->Set_complete();
  RECORD_TYPE_PTR rec_type =
      _glob->New_rec_type(RECORD_KIND::STRUCT, "record", spos);
  FIELD_PTR fld1 = _glob->New_fld("a", int32_type, rec_type, spos);
  FIELD_PTR fld2 = _glob->New_fld("d", uni_type, rec_type, spos);
  FIELD_PTR fld3 = _glob->New_fld("e", int32_type, rec_type, spos);
  rec_type->Add_fld(fld1->Id());
  rec_type->Add_fld(fld2->Id());
  rec_type->Add_fld(fld3->Id());
  rec_type->Set_complete();
  EXPECT_EQ(rec_type->Byte_size(), 24);
  EXPECT_EQ(rec_type->Alignment(), DATA_ALIGN::BIT64);
  EXPECT_EQ(fld1->Ofst(), 0);
  EXPECT_EQ(fld1->Bit_size(), 32);
  EXPECT_EQ(fld2->Ofst(), 64);
  EXPECT_EQ(fld2->Bit_size(), 64);
  EXPECT_EQ(fld3->Ofst(), 128);
  EXPECT_EQ(fld3->Bit_size(), 32);
}

void TEST_GLOB_SCOPE::Run_test_new_signature_type() {
  SIGNATURE_TYPE_PTR sig   = _glob->New_sig_type();
  TYPE_PTR           rtype = _glob->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
  _glob->New_ret_param(rtype, sig);
  STR_PTR str_x = _glob->New_str("x");
  STR_PTR str_y = _glob->New_str("y");
  SPOS    spos  = _glob->Unknown_simple_spos();
  _glob->New_param(str_x, rtype, sig, spos);
  _glob->New_param(str_y, rtype, sig, spos);
  sig->Set_complete();

  std::string expected(
      "TYP[0x12] signature(\"_noname\"), dwarf_ret(TYP[0xffffffff]), "
      "complete(yes)\n"
      "  PAR[0] return, \"_noname\", TYP[0x8](primitive, \"float32_t\")\n"
      "  PAR[0x1] regular, \"x\", TYP[0x8](primitive, \"float32_t\")\n"
      "  PAR[0x2] regular, \"y\", TYP[0x8](primitive, \"float32_t\")\n");
  std::string result(sig->To_str());
  EXPECT_EQ(result, expected);
}

void TEST_GLOB_SCOPE::Run_test_new_multi_dimen_array_type() {
  // ARRAY of type W * H * C [28 * 28 * 10]
  TYPE_PTR etype = _glob->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
  SPOS     spos  = _glob->Unknown_simple_spos();
  ARB_PTR  arb_c = _glob->New_arb(0, 0, 10, 1);
  ARB_PTR  arb_h = _glob->New_arb(1, 0, 28, 1);
  ARB_PTR  arb_w = _glob->New_arb(2, 0, 28, 1);
  arb_c->Set_next(arb_h->Id());
  arb_h->Set_next(arb_w->Id());
  TYPE_PTR w_type =
      _glob->New_arr_type("multi_demension_array", etype, arb_c, spos);

  std::string expected(
      "TYP[0x12] array(\"multi_demension_array\"), element(TYP[0x8])\n"
      "  ARB[0] lower(const:0), upper(const:0xa), stride(const:0x1)\n"
      "    dimension(0), first(yes), last(no), next(ARB[0x1])\n"
      "  ARB[0x1] lower(const:0), upper(const:0x1c), stride(const:0x1)\n"
      "    dimension(0x1), first(no), last(no), next(ARB[0x2])\n"
      "  ARB[0x2] lower(const:0), upper(const:0x1c), stride(const:0x1)\n"
      "    dimension(0x2), first(no), last(yes), next(ARB[0xffffffff])\n");
  std::string result(w_type->To_str());
  EXPECT_EQ(result, expected);
  EXPECT_EQ(w_type->Byte_size(), 4 * 10 * 28 * 28);
}

void TEST_GLOB_SCOPE::Run_test_new_multi_dimen_array_type_vec() {
  // ARRAY of type W * H * C [28 * 28 * 10]
  TYPE_PTR             etype    = _glob->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
  SPOS                 spos     = _glob->Unknown_simple_spos();
  int64_t              uppers[] = {10, 28, 28};
  std::vector<int64_t> dims(uppers, uppers + 3);
  TYPE_PTR w_type = _glob->New_arr_type("multi_array_vec", etype, dims, spos);

  std::string expected(
      "TYP[0x12] array(\"multi_array_vec\"), element(TYP[0x8])\n"
      "  ARB[0] lower(const:0), upper(const:0xa), stride(const:0x1)\n"
      "    dimension(0), first(yes), last(no), next(ARB[0x1])\n"
      "  ARB[0x1] lower(const:0), upper(const:0x1c), stride(const:0x1)\n"
      "    dimension(0x1), first(no), last(no), next(ARB[0x2])\n"
      "  ARB[0x2] lower(const:0), upper(const:0x1c), stride(const:0x1)\n"
      "    dimension(0x2), first(no), last(yes), next(ARB[0xffffffff])\n");
  std::string result(w_type->To_str());
  EXPECT_EQ(result, expected);
}

void TEST_GLOB_SCOPE::Run_test_new_var_sym() {
  TYPE_PTR       type = _glob->Prim_type(PRIMITIVE_TYPE::INT_U64);
  ADDR_DATUM_PTR var  = _glob->New_var(type, "my_var", SPOS());

  std::string expected(
      "VAR[0] \"my_var\"\n"
      "  scope_level[0], TYP[0x7](primitive,\"uint64_t\")\n");
  std::string result(var->To_str());
  EXPECT_EQ(result, expected);
}

void TEST_GLOB_SCOPE::Run_test_new_func_sym() {
  SPOS     spos = _glob->Unknown_simple_spos();
  FUNC_PTR func = _glob->New_func("My_func", spos);
  func->Set_parent(_glob->Comp_env_id());

  std::string expected(
      "FUN[0] \"My_func\"\n"
      "  parentBLK[0], defined(no)\n");
  std::string result(func->To_str());
  EXPECT_EQ(result, expected);
}

void TEST_GLOB_SCOPE::Run_test_new_entry_sym() {
  STR_PTR  name_str = _glob->New_str("My_func_entry");
  SPOS     spos     = _glob->Unknown_simple_spos();
  FUNC_PTR func     = _glob->New_func(name_str, spos);
  func->Set_parent(_glob->Comp_env_id());
  SIGNATURE_TYPE_PTR sig = _glob->New_sig_type();
  ENTRY_PTR entry = _glob->New_global_entry_point(sig, func, name_str, spos);

  std::string expected(
      "ENT[0x1] \"My_func_entry\"\n"
      "  FUN[0], TYP[0x12](signature,\"_noname\")\n");
  std::string result(entry->To_str());
  EXPECT_EQ(result, expected);
}

void TEST_GLOB_SCOPE::Run_test_clone() {
  // Define a pointer type
  SPOS             spos  = _glob->Unknown_simple_spos();
  TYPE_PTR         etype = _glob->Prim_type(PRIMITIVE_TYPE::FLOAT_32);
  POINTER_TYPE_PTR ptr = _glob->New_ptr_type(etype->Id(), POINTER_KIND::FLAT64);

  // Define a record type
  RECORD_TYPE_PTR rec_type =
      _glob->New_rec_type(RECORD_KIND::STRUCT, "tensor_record", spos);
  STR_PTR   name_fld1 = _glob->New_str("fld1");
  TYPE_PTR  fld1_type = _glob->Prim_type(PRIMITIVE_TYPE::INT_U64);
  FIELD_PTR fld1      = _glob->New_fld(name_fld1, fld1_type, rec_type, spos);
  rec_type->Add_fld(fld1->Id());
  STR_PTR   name_fld2 = _glob->New_str("fld2");
  FIELD_PTR fld2      = _glob->New_fld(name_fld2, etype, rec_type, spos);
  rec_type->Add_fld(fld2->Id());
  rec_type->Set_complete();

  // Define a function signature type
  SIGNATURE_TYPE_PTR sig = _glob->New_sig_type();
  _glob->New_ret_param(etype, sig);
  STR_PTR str_x = _glob->New_str("x");
  STR_PTR str_y = _glob->New_str("y");
  _glob->New_param(str_x, etype, sig, spos);
  _glob->New_param(str_y, etype, sig, spos);
  sig->Set_complete();

  // Define a multi-demension array type
  int64_t              uppers[] = {10, 28, 28};
  std::vector<int64_t> dims(uppers, uppers + 3);
  TYPE_PTR w_type = _glob->New_arr_type("multi_array_vec", etype, dims, spos);

  // Define a function
  META_INFO::Remove_all();
  air::core::Register_core();
  STR_PTR   func_name  = _glob->New_str("My_float_add");
  FUNC_PTR  func_ptr   = _glob->New_func(func_name, spos);
  ENTRY_PTR func_entry = _glob->New_entry_point(sig, func_ptr, func_name, spos);
  FUNC_SCOPE*    func_scope = &_glob->New_func_scope(func_ptr);
  CONTAINER*     func_cntr  = &func_scope->Container();
  STMT_PTR       estmt      = func_cntr->New_func_entry(spos);
  ADDR_DATUM_PTR formal_x   = func_scope->Formal(0);
  ADDR_DATUM_PTR formal_y   = func_scope->Formal(1);
  ADDR_DATUM_PTR var_z      = func_scope->New_var(etype, "z", spos);
  NODE_PTR       node_x     = func_cntr->New_ld(formal_x, spos);
  NODE_PTR       node_y     = func_cntr->New_ld(formal_y, spos);
  NODE_PTR       node_add =
      func_cntr->New_bin_arith(air::core::OPC_ADD, node_x, node_y, spos);
  STMT_PTR stmt_st = func_cntr->New_st(node_add, var_z, spos);
  func_cntr->Stmt_list().Append(stmt_st);
  NODE_PTR node_z   = func_cntr->New_ld(var_z, spos);
  STMT_PTR stmt_ret = func_cntr->New_retv(node_z, spos);
  func_cntr->Stmt_list().Append(stmt_ret);

  GLOB_SCOPE* cloned_glob = new GLOB_SCOPE(1, true);
  cloned_glob->Clone(*_glob);

  // Type table, implying arb table, param table and field table
  TYPE_ITER   ori_t_iter = _glob->Begin_type();
  TYPE_ITER   ori_t_end  = _glob->End_type();
  std::string type_ori;
  for (; ori_t_iter != ori_t_end; ++ori_t_iter) {
    type_ori.append((*ori_t_iter)->To_str());
  }
  TYPE_ITER   clo_t_iter = cloned_glob->Begin_type();
  TYPE_ITER   clo_t_end  = cloned_glob->End_type();
  std::string type_clo;
  for (; clo_t_iter != clo_t_end; ++clo_t_iter) {
    type_clo.append((*clo_t_iter)->To_str());
  }
  EXPECT_EQ(type_ori, type_clo);

  // String table
  STR_ITER    ori_s_iter = _glob->Begin_str();
  STR_ITER    ori_s_end  = _glob->End_str();
  std::string str_ori;
  for (; ori_s_iter != ori_s_end; ++ori_s_iter) {
    str_ori.append((*ori_s_iter)->Char_str());
  }
  STR_ITER    clo_s_iter = cloned_glob->Begin_str();
  STR_ITER    clo_s_end  = cloned_glob->End_str();
  std::string str_clo;
  for (; clo_s_iter != clo_s_end; ++clo_s_iter) {
    str_clo.append((*clo_s_iter)->Char_str());
  }
  EXPECT_EQ(str_ori, str_clo);

  // Function table, implying func_def table
  FUNC_ITER   ori_f_iter = _glob->Begin_func();
  FUNC_ITER   ori_f_end  = _glob->End_func();
  std::string func_ori;
  for (; ori_f_iter != ori_f_end; ++ori_f_iter) {
    func_ori.append((*ori_f_iter)->To_str());
  }
  std::string expected_ori(
      "FUN[0] \"My_float_add\"\n"
      "  parentBLK[0], defined(yes)\n");
  EXPECT_EQ(func_ori, expected_ori);
  FUNC_ITER   clo_f_iter = cloned_glob->Begin_func();
  FUNC_ITER   clo_f_end  = cloned_glob->End_func();
  std::string func_clo;
  for (; clo_f_iter != clo_f_end; ++clo_f_iter) {
    func_clo.append((*clo_f_iter)->To_str());
  }
  std::string expected_clo(
      "FUN[0] \"My_float_add\"\n"
      "  parentBLK[0], defined(no)\n");
  EXPECT_EQ(func_clo, expected_clo);

  delete cloned_glob;
}

void TEST_GLOB_SCOPE::Run_test_init_targ_info() {
  _glob->Init_targ_info(ENDIANNESS::BIG, ARCHITECTURE::RISCV);
  std::string res = _glob->Targ_info()->To_str();
  std::string exp("TARG_INFO: ENDIAN=BIG,ARCH=2\n");
  EXPECT_EQ(res, exp);
  _glob->Targ_info()->Set_attr("attr", "good");
  _glob->Targ_info()->Set_attr("rtta", "bad");
  std::string_view attr = _glob->Targ_info()->Attr("attr");
  EXPECT_EQ(attr, "good");
  std::string_view rtta = _glob->Targ_info()->Attr("rtta");
  EXPECT_EQ(rtta, "bad");
}

TEST_F(TEST_GLOB_SCOPE, new_pointer_type) { Run_test_new_pointer_type(); }
TEST_F(TEST_GLOB_SCOPE, new_array_type) { Run_test_new_array_type(); }
TEST_F(TEST_GLOB_SCOPE, new_record_type) { Run_test_new_record_type(); }
TEST_F(TEST_GLOB_SCOPE, record_size1) { Run_test_record_size1(); }
TEST_F(TEST_GLOB_SCOPE, record_size2) { Run_test_record_size2(); }
TEST_F(TEST_GLOB_SCOPE, record_size3) { Run_test_record_size3(); }
TEST_F(TEST_GLOB_SCOPE, record_size4) { Run_test_record_size4(); }
TEST_F(TEST_GLOB_SCOPE, record_size5) { Run_test_record_size5(); }
TEST_F(TEST_GLOB_SCOPE, record_size6) { Run_test_record_size6(); }
TEST_F(TEST_GLOB_SCOPE, record_size7) { Run_test_record_size7(); }
TEST_F(TEST_GLOB_SCOPE, record_size8) { Run_test_record_size8(); }
TEST_F(TEST_GLOB_SCOPE, union_size) { Run_test_union_size(); }
TEST_F(TEST_GLOB_SCOPE, record_union_size) { Run_test_record_union_size(); }
TEST_F(TEST_GLOB_SCOPE, new_signature_type) { Run_test_new_signature_type(); }
TEST_F(TEST_GLOB_SCOPE, new_multi_dimen_array_type) {
  Run_test_new_multi_dimen_array_type();
}
TEST_F(TEST_GLOB_SCOPE, new_multi_dimen_array_type_vec) {
  Run_test_new_multi_dimen_array_type_vec();
}
TEST_F(TEST_GLOB_SCOPE, new_var_sym) { Run_test_new_var_sym(); }
TEST_F(TEST_GLOB_SCOPE, new_func_sym) { Run_test_new_func_sym(); }
TEST_F(TEST_GLOB_SCOPE, new_entry_sym) { Run_test_new_entry_sym(); }
TEST_F(TEST_GLOB_SCOPE, clone) { Run_test_clone(); }
TEST_F(TEST_GLOB_SCOPE, init_targ_info) { Run_test_init_targ_info(); }

class TEST_FUNC_SCOPE : public ::testing::Test {
protected:
  void SetUp() override {
    META_INFO::Remove_all();
    air::core::Register_core();
    _glob             = new GLOB_SCOPE(0, true);
    STR_PTR  name_str = _glob->New_str("My_func");
    SPOS     spos     = _glob->Unknown_simple_spos();
    FUNC_PTR func     = _glob->New_func(name_str, spos);
    func->Set_parent(_glob->Comp_env_id());
    SIGNATURE_TYPE_PTR sig  = _glob->New_sig_type();
    TYPE_PTR           type = _glob->Prim_type(PRIMITIVE_TYPE::INT_S32);
    _glob->New_param("x", type, sig, spos);
    _glob->New_param("y", type, sig, spos);
    sig->Set_complete();
    ENTRY_PTR entry  = _glob->New_entry_point(sig, func, name_str, spos);
    _func            = &_glob->New_func_scope(func);
    CONTAINER* cntr  = &_func->Container();
    STMT_PTR   estmt = cntr->New_func_entry(SPOS());
  }
  void TearDown() override { delete _glob; }
  void Run_test_new_formal_sym();
  void Run_test_new_var_sym();
  void Run_test_new_preg_sym();
  void Run_test_clone();

  GLOB_SCOPE* _glob;
  FUNC_SCOPE* _func;
};

void TEST_FUNC_SCOPE::Run_test_new_formal_sym() {
  ADDR_DATUM_PTR formal_x = _func->Formal(0);
  ADDR_DATUM_PTR formal_y = _func->Formal(1);

  std::string expected_x(
      "FML[0x10000000] \"x\", TYP[0x2](primitive,\"int32_t\")\n");
  std::string result_x(formal_x->To_str());
  EXPECT_EQ(result_x, expected_x);
  std::string expected_y(
      "FML[0x10000001] \"y\", TYP[0x2](primitive,\"int32_t\")\n");
  std::string result_y(formal_y->To_str());
  EXPECT_EQ(result_y, expected_y);
}

void TEST_FUNC_SCOPE::Run_test_new_var_sym() {
  TYPE_PTR       type = _glob->Prim_type(PRIMITIVE_TYPE::INT_U64);
  ADDR_DATUM_PTR var  = _func->New_var(type, "a", SPOS());

  std::string expected(
      "VAR[0x10000002] \"a\"\n"
      "  scope_level[0x1], TYP[0x7](primitive,\"uint64_t\")\n");
  std::string result(var->To_str());
  EXPECT_EQ(result, expected);
}

void TEST_FUNC_SCOPE::Run_test_new_preg_sym() {
  TYPE_PTR type = _glob->Prim_type(PRIMITIVE_TYPE::INT_U64);
  PREG_PTR preg = _func->New_preg(type);

  std::string expected(
      "PREG[0x10000000] TYP[0x7](primitive,\"uint64_t\",size:8), "
      "Home[0xffffffff]\n");
  std::string result(preg->To_str());
  EXPECT_EQ(result, expected);
}

void TEST_FUNC_SCOPE::Run_test_clone() {
  TYPE_PTR       type = _glob->Prim_type(PRIMITIVE_TYPE::INT_S32);
  ADDR_DATUM_PTR var  = _func->New_var(type, "a", SPOS());
  PREG_PTR       preg = _func->New_preg(type);

  GLOB_SCOPE* cloned_glob = new GLOB_SCOPE(1, true);
  cloned_glob->Clone(*_glob);
  FUNC_SCOPE* cloned_func = &cloned_glob->New_func_scope(_func->Owning_func());
  cloned_func->Clone(*_func);

  FORMAL_ITER ori_f_iter = _func->Begin_formal();
  FORMAL_ITER ori_f_end  = _func->End_formal();
  std::string formal_ori;
  for (; ori_f_iter != ori_f_end; ++ori_f_iter) {
    formal_ori.append((*ori_f_iter)->To_str());
  }
  FORMAL_ITER clo_f_iter = cloned_func->Begin_formal();
  FORMAL_ITER clo_f_end  = cloned_func->End_formal();
  std::string formal_clo;
  for (; clo_f_iter != clo_f_end; ++clo_f_iter) {
    formal_clo.append((*clo_f_iter)->To_str());
  }
  EXPECT_EQ(formal_ori, formal_clo);

  VAR_ITER    ori_v_iter = _func->Begin_var();
  VAR_ITER    ori_v_end  = _func->End_var();
  std::string var_ori;
  for (; ori_v_iter != ori_v_end; ++ori_v_iter) {
    var_ori.append((*ori_v_iter)->To_str());
  }
  VAR_ITER    clo_v_iter = cloned_func->Begin_var();
  VAR_ITER    clo_v_end  = cloned_func->End_var();
  std::string var_clo;
  for (; clo_v_iter != clo_v_end; ++clo_v_iter) {
    var_clo.append((*clo_v_iter)->To_str());
  }
  EXPECT_EQ(var_ori, var_clo);

  PREG_ITER   ori_p_iter = _func->Begin_preg();
  PREG_ITER   ori_p_end  = _func->End_preg();
  std::string preg_ori;
  for (; ori_p_iter != ori_p_end; ++ori_p_iter) {
    preg_ori.append((*ori_p_iter)->To_str());
  }
  PREG_ITER   clo_p_iter = cloned_func->Begin_preg();
  PREG_ITER   clo_p_end  = cloned_func->End_preg();
  std::string preg_clo;
  for (; clo_p_iter != clo_p_end; ++clo_p_iter) {
    preg_clo.append((*clo_p_iter)->To_str());
  }
  EXPECT_EQ(preg_ori, preg_clo);

  delete cloned_glob;
}

TEST_F(TEST_FUNC_SCOPE, new_formal_sym) { Run_test_new_formal_sym(); }
TEST_F(TEST_FUNC_SCOPE, new_var_sym) { Run_test_new_var_sym(); }
TEST_F(TEST_FUNC_SCOPE, new_preg_sym) { Run_test_new_preg_sym(); }
TEST_F(TEST_FUNC_SCOPE, clone) { Run_test_clone(); }
