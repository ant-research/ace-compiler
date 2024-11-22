//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/ckks/ckks_gen.h"

#include <vector>

#include "air/base/container_decl.h"
#include "air/base/meta_info.h"
#include "air/base/opcode.h"
#include "air/base/st.h"
#include "air/base/st_decl.h"
#include "air/base/st_enum.h"
#include "air/base/st_misc.h"
#include "air/core/opcode.h"
#include "air/util/debug.h"
#include "air/util/messg.h"
#include "fhe/ckks/ckks_opcode.h"
#include "fhe/sihe/sihe_gen.h"

namespace fhe {

namespace ckks {
using namespace air::base;

enum class POLY_TYPE_FIELD {
  POLY        = 0,  // field 0 denotes whole aggregate
  RING_DEGREE = 1,
  DATA        = 2,
  LAST        = 3
};

static const char*
    Poly_type_field_name[static_cast<uint32_t>(POLY_TYPE_FIELD::LAST)] = {
        "POLY",
        "_ring_degree",
        "_data",
};

enum class PLAIN_TYPE_FIELD {
  PLAIN = 0,  // field 0 denotes whole aggregate
  POLY  = 1,
  LAST  = 2,
};

static const char*
    Plain_type_field_name[static_cast<uint32_t>(PLAIN_TYPE_FIELD::LAST)] = {
        "PLAINTEXT",
        "_poly",
};

enum class CIPHER_TYPE_FIELD {
  CIPHER  = 0,  // field 0 denotes whole aggregate
  C0_POLY = 1,
  C1_POLY = 2,
  LAST    = 3,
};

static const char*
    Cipher_type_field_name[static_cast<uint32_t>(CIPHER_TYPE_FIELD::LAST)] = {
        "CIPHERTEXT",
        "_c0_poly",
        "_c1_poly",
};

// CIPHER3 denotes result of (CIPHER * CIPHER) in CKKS which contains 3
// polynomials.
enum class CIPHER3_TYPE_FIELD {
  CIPHER3 = 0,  // field 0 denotes whole aggregate
  C0_POLY = 1,
  C1_POLY = 2,
  C2_POLY = 3,
  LAST    = 4,
};

static const char*
    Cipher3_type_field_name[static_cast<uint32_t>(CIPHER3_TYPE_FIELD::LAST)] = {
        "CIPHERTEXT3",
        "_c0_poly",
        "_c1_poly",
        "_c2_poly",
};

TYPE_ID CKKS_GEN::Gen_poly_type() {
  SPOS            spos           = _glob_scope->Unknown_simple_spos();
  const char*     poly_type_name = Poly_type_field_name[0];
  STR_PTR         poly_str       = _glob_scope->New_str(poly_type_name);
  RECORD_TYPE_PTR poly_type =
      _glob_scope->New_rec_type(RECORD_KIND::STRUCT, poly_str, spos);

  // create field 1(_ring_degree) type: uint64_t
  const char* ring_degree_name =
      Poly_type_field_name[static_cast<uint32_t>(POLY_TYPE_FIELD::RING_DEGREE)];
  STR_PTR   fld1_str = _glob_scope->New_str(ring_degree_name);
  TYPE_PTR  fld1_ty  = _glob_scope->Prim_type(PRIMITIVE_TYPE::INT_U64);
  FIELD_PTR fld1     = _glob_scope->New_fld(fld1_str, fld1_ty, poly_type, spos);
  poly_type->Add_fld(fld1->Id());

  // create field 2(_data) type: uint64_t*
  TYPE_PTR u64_type = _glob_scope->Prim_type(PRIMITIVE_TYPE::INT_U64);
  TYPE_PTR ptr_u64 =
      _glob_scope->New_ptr_type(u64_type->Id(), POINTER_KIND::FLAT64);
  const char* data_name =
      Poly_type_field_name[static_cast<uint32_t>(POLY_TYPE_FIELD::DATA)];
  STR_PTR   fld2_str = _glob_scope->New_str(data_name);
  FIELD_PTR fld2     = _glob_scope->New_fld(fld2_str, ptr_u64, poly_type, spos);
  poly_type->Add_fld(fld2->Id());
  poly_type->Set_complete();
  return poly_type->Id();
}

TYPE_ID CKKS_GEN::Gen_cipher3_type() {
  SPOS            spos      = _glob_scope->Unknown_simple_spos();
  STR_PTR         type_name = _glob_scope->New_str(Cipher3_type_field_name[0]);
  RECORD_TYPE_PTR cipher3_type =
      _glob_scope->New_rec_type(RECORD_KIND::STRUCT, type_name, spos);

  TYPE_PTR poly_type = _lower_ctx->Get_poly_type(_glob_scope);
  // create fields
  for (uint32_t id = 1; id < static_cast<uint32_t>(CIPHER3_TYPE_FIELD::LAST);
       ++id) {
    STR_PTR   fld_str = _glob_scope->New_str(Cipher3_type_field_name[id]);
    FIELD_PTR fld =
        _glob_scope->New_fld(fld_str, poly_type, cipher3_type, spos);
    cipher3_type->Add_fld(fld->Id());
  }
  cipher3_type->Set_complete();
  return cipher3_type->Id();
}

TYPE_ID CKKS_GEN::Update_plain_type() {
  TYPE_ID         plain_type_id = _lower_ctx->Get_plain_type_id();
  RECORD_TYPE_PTR plain_type = _glob_scope->Type(plain_type_id)->Cast_to_rec();

  TYPE_PTR    poly_type = _lower_ctx->Get_poly_type(_glob_scope);
  const char* name_of_ploy =
      Plain_type_field_name[static_cast<uint32_t>(PLAIN_TYPE_FIELD::POLY)];
  STR_PTR   ploy_str = _glob_scope->New_str(name_of_ploy);
  FIELD_PTR field =
      _glob_scope->New_fld(ploy_str, poly_type, plain_type, plain_type->Spos());
  plain_type->Add_fld(field->Id());
  plain_type->Set_complete();
  return plain_type_id;
}

TYPE_ID CKKS_GEN::Update_cipher_type() {
  TYPE_ID         cipher_type_id = _lower_ctx->Get_cipher_type_id();
  RECORD_TYPE_PTR cipher_type =
      _glob_scope->Type(cipher_type_id)->Cast_to_rec();

  TYPE_PTR poly_type = _lower_ctx->Get_poly_type(_glob_scope);
  // create fields
  for (uint32_t id = 1; id < static_cast<uint32_t>(CIPHER_TYPE_FIELD::LAST);
       ++id) {
    STR_PTR   fld_str = _glob_scope->New_str(Cipher_type_field_name[id]);
    FIELD_PTR fld     = _glob_scope->New_fld(fld_str, poly_type, cipher_type,
                                             cipher_type->Spos());
    cipher_type->Add_fld(fld->Id());
  }
  cipher_type->Set_complete();
  return cipher_type_id;
}

void CKKS_GEN::Register_ckks_types() {
  TYPE_ID poly_type_id = Gen_poly_type();
  _lower_ctx->Set_poly_type_id(poly_type_id);

  TYPE_ID cipher3_type_id = Gen_cipher3_type();
  _lower_ctx->Set_cipher3_type_id(cipher3_type_id);

  Update_plain_type();
  Update_cipher_type();
}
}  // namespace ckks
}  // namespace fhe
