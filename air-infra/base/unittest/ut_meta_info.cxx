//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <string.h>

#include "air/base/meta_info.h"
#include "gtest/gtest.h"

using namespace air::base;

namespace {

enum OPERATOR { FUNC_ENTRY, PRAGMA, DO_LOOP, LDID, LABEL, ADD, CALL, INVALID };

struct OPR_INFO Opr_info[] = {
    {"FUNC_ENTRY", OPR_CAT::ENTRY,  OPR_KIDS::FLEXIBLE, 0, PROP_SCF | PROP_STMT                        },
    {"PRAGMA",     OPR_CAT::PRAGMA, 0,                  0,
     PROP_LEAF | PROP_STMT | PROP_SYM | PROP_VALUE                                                     },
    {"DO_LOOP",    OPR_CAT::CFLOW,  4,                  0, PROP_SCF | PROP_STMT                        },
    {"LDID",       OPR_CAT::LDST,   0,                  0, PROP_LEAF | PROP_LOAD | PROP_SYM | PROP_TYPE},
    {"LABEL",      OPR_CAT::STMT,   0,                  0, PROP_LEAF | PROP_LABEL                      },
    {"ADD",        OPR_CAT::EXPR,   2,                  0, PROP_EXPR | PROP_TYPE                       },
    {"CALL",       OPR_CAT::CALL,   OPR_KIDS::FLEXIBLE, 0,
     PROP_CALL | PROP_STMT | PROP_FLAGS | PROP_RET_VAR                                                 }
};

constexpr uint32_t NUM_OPR_INFO = sizeof(Opr_info) / sizeof(Opr_info[0]);

constexpr uint32_t DOM0_ID = 0;

struct DOMAIN_INFO Dom0_info = {"DOM0", DOM0_ID, NUM_OPR_INFO, Opr_info};

constexpr uint32_t DOM1_ID = 1;

struct DOMAIN_INFO Dom1_info = {"DOM1", DOM1_ID, NUM_OPR_INFO, Opr_info};

TEST(Meta_info, Domains) {
  META_INFO::Remove_all();
  bool ret;
  ret = META_INFO::Register_domain(&Dom0_info);
  EXPECT_TRUE(ret);
  EXPECT_EQ(META_INFO::Num_domain(), 1);
  ret = META_INFO::Register_domain(&Dom1_info);
  EXPECT_TRUE(ret);
  EXPECT_EQ(META_INFO::Num_domain(), 2);
  ret = META_INFO::Register_domain(&Dom0_info);
  EXPECT_FALSE(ret);
  EXPECT_EQ(META_INFO::Num_domain(), 2);
  ret = META_INFO::Register_domain(&Dom1_info);
  EXPECT_FALSE(ret);
  EXPECT_EQ(META_INFO::Num_domain(), 2);

  EXPECT_TRUE(META_INFO::Valid_domain(DOM0_ID));
  EXPECT_TRUE(META_INFO::Valid_domain(DOM1_ID));
  EXPECT_FALSE(META_INFO::Valid_domain(DOM0_ID - 1));
  EXPECT_FALSE(META_INFO::Valid_domain(DOM1_ID + 1));

  EXPECT_EQ(strcmp(META_INFO::Domain_name(DOM0_ID), "DOM0"), 0);
  EXPECT_EQ(strcmp(META_INFO::Domain_name(DOM1_ID), "DOM1"), 0);

  EXPECT_EQ(META_INFO::Num_domain_op(DOM0_ID), NUM_OPR_INFO);
  EXPECT_EQ(META_INFO::Num_domain_op(DOM1_ID), NUM_OPR_INFO);
}

TEST(Meta_info, Operators) {
  META_INFO::Remove_all();
  META_INFO::Register_domain(&Dom0_info);
  META_INFO::Register_domain(&Dom1_info);

  EXPECT_TRUE(META_INFO::Valid_operator(DOM0_ID, FUNC_ENTRY));
  EXPECT_TRUE(META_INFO::Valid_operator(DOM1_ID, CALL));
  EXPECT_FALSE(META_INFO::Valid_operator(DOM0_ID - 1, FUNC_ENTRY));
  EXPECT_FALSE(META_INFO::Valid_operator(DOM1_ID + 1, CALL));
  EXPECT_FALSE(META_INFO::Valid_operator(DOM0_ID, FUNC_ENTRY - 1));
  EXPECT_FALSE(META_INFO::Valid_operator(DOM1_ID, INVALID));

  EXPECT_EQ(strcmp(META_INFO::Op_name(DOM0_ID, FUNC_ENTRY), "FUNC_ENTRY"), 0);
  EXPECT_EQ(strcmp(META_INFO::Op_name(DOM1_ID, FUNC_ENTRY), "FUNC_ENTRY"), 0);

  EXPECT_EQ(META_INFO::Op_num_child(DOM0_ID, FUNC_ENTRY), OPR_KIDS::FLEXIBLE);
  EXPECT_EQ(META_INFO::Op_num_child(DOM1_ID, FUNC_ENTRY), OPR_KIDS::FLEXIBLE);
  EXPECT_EQ(META_INFO::Op_num_child(DOM0_ID, ADD), 2);
  EXPECT_EQ(META_INFO::Op_num_child(DOM1_ID, ADD), 2);

  EXPECT_EQ(META_INFO::Op_category(DOM0_ID, PRAGMA), OPR_CAT::PRAGMA);
  EXPECT_EQ(META_INFO::Op_category(DOM1_ID, PRAGMA), OPR_CAT::PRAGMA);
  EXPECT_EQ(META_INFO::Op_category(DOM0_ID, CALL), OPR_CAT::CALL);
  EXPECT_EQ(META_INFO::Op_category(DOM1_ID, CALL), OPR_CAT::CALL);

  EXPECT_EQ(META_INFO::Op_properties(DOM0_ID, DO_LOOP),
            Opr_info[DO_LOOP]._prop);
  EXPECT_EQ(META_INFO::Op_properties(DOM1_ID, DO_LOOP),
            Opr_info[DO_LOOP]._prop);
  EXPECT_EQ(META_INFO::Op_properties(DOM0_ID, LDID), Opr_info[LDID]._prop);
  EXPECT_EQ(META_INFO::Op_properties(DOM1_ID, LDID), Opr_info[LDID]._prop);

  EXPECT_TRUE(META_INFO::Op_has_prop(DOM0_ID, LABEL, OPR_PROP::LEAF));
  EXPECT_TRUE(META_INFO::Op_has_prop(DOM1_ID, LABEL, OPR_PROP::LEAF));
  EXPECT_TRUE(META_INFO::Op_has_prop(DOM0_ID, CALL, OPR_PROP::FLAGS));
  EXPECT_TRUE(META_INFO::Op_has_prop(DOM1_ID, CALL, OPR_PROP::FLAGS));

  EXPECT_TRUE(META_INFO::Has_prop<OPR_PROP::LEAF>(DOM0_ID, LABEL));
  EXPECT_TRUE(META_INFO::Has_prop<OPR_PROP::LEAF>(DOM1_ID, LABEL));
  EXPECT_TRUE(META_INFO::Has_prop<OPR_PROP::FLAGS>(DOM0_ID, CALL));
  EXPECT_TRUE(META_INFO::Has_prop<OPR_PROP::FLAGS>(DOM1_ID, CALL));
}

}  // anonymous namespace
