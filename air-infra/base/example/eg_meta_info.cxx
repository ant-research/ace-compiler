//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <stdio.h>

#include "air/base/meta_info.h"

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

constexpr uint32_t ID = 0;

struct DOMAIN_INFO Domain_info = {
    "DOM0", ID, sizeof(Opr_info) / sizeof(Opr_info[0]), Opr_info};

}  // anonymous namespace

int main() {
  bool ret = META_INFO::Register_domain(&Domain_info);
  AIR_ASSERT(ret == true);
  printf("Register domain [%s:%d] %s.\n", META_INFO::Domain_name(ID), ID,
         ret ? "succ" : "fail");

  for (uint32_t i = 0; i < META_INFO::Num_domain_op(ID); ++i) {
    printf("  [%d] %s %d %s %x\n", i, META_INFO::Op_name(ID, i),
           META_INFO::Op_num_child(ID, i), META_INFO::Op_category_name(ID, i),
           META_INFO::Op_properties(ID, i));
    for (uint32_t j = 0; j < (uint32_t)OPR_PROP::LAST_PROP; ++j) {
      printf("    %s: %s\n", META_INFO::Op_prop_name((OPR_PROP)j),
             META_INFO::Op_has_prop(ID, i, (OPR_PROP)j) ? "yes" : "no");
    }
  }

  return 0;
}
