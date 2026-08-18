//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_TEST_GEN_CKKS_IR_H
#define FHE_TEST_GEN_CKKS_IR_H

#include <limits.h>

#include "air/base/container_decl.h"
#include "air/base/meta_info.h"
#include "air/base/node.h"
#include "air/base/st.h"
#include "air/core/handler.h"
#include "air/core/opcode.h"
#include "air/driver/driver_ctx.h"
#include "fhe/ckks/ckks_gen.h"
#include "fhe/ckks/config.h"
#include "fhe/core/ctx_param_ana.h"
#include "fhe/poly/opcode.h"
#include "fhe/poly/poly2c_driver.h"
#include "fhe/poly/poly_driver.h"
#include "fhe/sihe/sihe_gen.h"
#include "fhe/util/util.h"

namespace fhe {
namespace poly {
namespace test {

class CKKS_IR_GEN {
public:
  CKKS_IR_GEN(fhe::core::LOWER_CTX& lower_ctx) : _lower_ctx(lower_ctx) {
    bool ret = air::core::Register_core();
    CMPLR_ASSERT(ret, "core register failed");
    ret = fhe::ckks::Register_ckks_domain();
    CMPLR_ASSERT(ret, "ckks register failed");
    ret = fhe::poly::Register_polynomial();
    CMPLR_ASSERT(ret, "polynomial register failed");

    _glob = new air::base::GLOB_SCOPE(0, true);
    fhe::sihe::SIHE_GEN(Glob(), &_lower_ctx).Register_sihe_types();
    fhe::ckks::CKKS_GEN(Glob(), &_lower_ctx).Register_ckks_types();
    _ciph_ty    = Glob()->Type(_lower_ctx.Get_cipher_type_id());
    _ciph3_ty   = Glob()->Type(_lower_ctx.Get_cipher3_type_id());
    _plain_ty   = Glob()->Type(_lower_ctx.Get_plain_type_id());
    _spos       = Glob()->Unknown_simple_spos();
    _main_graph = Gen_main_graph();
  }

  ~CKKS_IR_GEN() {
    air::base::META_INFO::Remove_all();
    _glob->~GLOB_SCOPE();
  }

  air::base::SPOS        Spos() { return _spos; }
  air::base::FUNC_SCOPE* Main_scope() { return _main_graph; }
  air::base::CONTAINER*  Main_container() { return &_main_graph->Container(); }
  air::base::GLOB_SCOPE* Glob() { return _glob; }
  air::base::TYPE_PTR    Ciph_ty() { return _ciph_ty; }
  air::base::TYPE_PTR    Ciph3_ty() { return _ciph3_ty; }
  air::base::TYPE_PTR    Plain_ty() { return _plain_ty; }
  air::base::ADDR_DATUM_PTR Input_var() { return _v_input; }
  air::base::ADDR_DATUM_PTR Output_var() { return _v_output; }

  air::base::FUNC_SCOPE* Gen_main_graph() {
    // name of main function
    air::base::STR_PTR name_str = Glob()->New_str("Main_graph");
    // main function
    air::base::FUNC_PTR main_func = Glob()->New_func(name_str, Spos());
    main_func->Set_parent(Glob()->Comp_env_id());
    // signature of main function
    air::base::SIGNATURE_TYPE_PTR sig = Glob()->New_sig_type();
    // return type of main function
    air::base::TYPE_PTR main_rtype =
        Glob()->Prim_type(air::base::PRIMITIVE_TYPE::INT_S32);
    Glob()->New_ret_param(main_rtype, sig);
    // parameter argc of function main
    air::base::TYPE_PTR argc_type =
        Glob()->Prim_type(air::base::PRIMITIVE_TYPE::INT_S32);
    air::base::STR_PTR argc_str = Glob()->New_str("input");
    Glob()->New_param(argc_str, _ciph_ty, sig, Spos());
    sig->Set_complete();
    // global entry for main
    air::base::ENTRY_PTR entry =
        Glob()->New_global_entry_point(sig, main_func, name_str, Spos());
    // set define before create a new scope
    air::base::FUNC_SCOPE* main_scope = &(Glob()->New_func_scope(main_func));
    air::base::STMT_PTR    ent_stmt =
        main_scope->Container().New_func_entry(Spos());
    _v_input  = main_scope->Formal(0);
    _v_output = main_scope->New_var(_ciph_ty, "output", Spos());

    main_scope->Owning_func()->Entry_point()->Set_program_entry();

    return main_scope;
  }

  void Append_output() {
    air::base::CONTAINER* cntr = &_main_graph->Container();
    air::base::STMT_LIST  sl   = cntr->Stmt_list();

    // return
    air::base::NODE_PTR ld_output_var = cntr->New_ld(Output_var(), Spos());
    air::base::STMT_PTR ret_stmt      = cntr->New_retv(ld_output_var, Spos());
    sl.Append(ret_stmt);

    // analyze CKKS parameters
    air::driver::DRIVER_CTX driver_ctx;
    fhe::ckks::CKKS_CONFIG  config;
    _lower_ctx.Get_ctx_param().Set_poly_degree(16);
    fhe::core::CTX_PARAM_ANA ctx_param_ana(cntr->Parent_func_scope(),
                                           &_lower_ctx, &driver_ctx, &config);
    ctx_param_ana.Run();
  }

  air::base::ADDR_DATUM_PTR Gen_int_var(const char* var_name) {
    air::base::STR_PTR  str = Glob()->New_str(var_name);
    air::base::TYPE_PTR ty =
        Glob()->Prim_type(air::base::PRIMITIVE_TYPE::INT_S32);
    return Main_scope()->New_var(ty, str, Spos());
  }

  air::base::ADDR_DATUM_PTR Gen_ciph_var(const char* var_name) {
    air::base::STR_PTR str = Glob()->New_str(var_name);
    return Main_scope()->New_var(Ciph_ty(), str, Spos());
  }

  air::base::ADDR_DATUM_PTR Gen_ciph3_var(const char* var_name) {
    air::base::STR_PTR str = Glob()->New_str(var_name);
    return Main_scope()->New_var(Ciph3_ty(), str, Spos());
  }

  air::base::ADDR_DATUM_PTR Gen_plain_var(const char* var_name) {
    air::base::STR_PTR str = Glob()->New_str(var_name);
    return Main_scope()->New_var(Plain_ty(), str, Spos());
  }

  // z = x + y
  air::base::STMT_PTR Gen_add(air::base::CONTAINER*     cntr,
                              air::base::ADDR_DATUM_PTR var_z,
                              air::base::ADDR_DATUM_PTR var_x,
                              air::base::ADDR_DATUM_PTR var_y,
                              air::base::STMT_LIST*     sl = NULL) {
    air::base::STMT_LIST stmt_list = cntr->Stmt_list();
    if (sl == NULL) {
      sl = &stmt_list;
    }
    air::base::NODE_PTR x_node = cntr->New_ld(var_x, Spos());
    air::base::NODE_PTR y_node = cntr->New_ld(var_y, Spos());
    air::base::NODE_PTR add_node =
        cntr->New_bin_arith(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                              fhe::ckks::CKKS_OPERATOR::ADD),
                            x_node, y_node, Spos());
    add_node->Set_rtype(Ciph_ty());
    air::base::STMT_PTR add_stmt = cntr->New_st(add_node, var_z, Spos());

    sl->Append(add_stmt);
    return add_stmt;
  }

  // var_z = var_x * var_p
  air::base::STMT_PTR Gen_mul(air::base::CONTAINER*     cntr,
                              air::base::ADDR_DATUM_PTR var_z,
                              air::base::ADDR_DATUM_PTR var_x,
                              air::base::ADDR_DATUM_PTR var_y,
                              air::base::STMT_LIST*     sl = NULL) {
    air::base::STMT_LIST stmt_list = cntr->Stmt_list();
    if (sl == NULL) {
      sl = &stmt_list;
    }
    air::base::NODE_PTR x_node = cntr->New_ld(var_x, Spos());
    air::base::NODE_PTR y_node = cntr->New_ld(var_y, Spos());
    air::base::NODE_PTR mul_node =
        cntr->New_bin_arith(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                              fhe::ckks::CKKS_OPERATOR::MUL),
                            x_node, y_node, Spos());
    if (var_y->Type() == Ciph_ty()) {
      mul_node->Set_rtype(Ciph3_ty());
    } else {
      mul_node->Set_rtype(Ciph_ty());
    }
    air::base::STMT_PTR mul_stmt = cntr->New_st(mul_node, var_z, Spos());
    sl->Append(mul_stmt);
    return mul_stmt;
  }

  air::base::STMT_PTR Gen_mul(air::base::CONTAINER*     cntr,
                              air::base::PREG_PTR       preg_z,
                              air::base::ADDR_DATUM_PTR var_x,
                              air::base::ADDR_DATUM_PTR var_y,
                              air::base::STMT_LIST*     sl = NULL) {
    air::base::STMT_LIST stmt_list = cntr->Stmt_list();
    if (sl == NULL) {
      sl = &stmt_list;
    }
    air::base::NODE_PTR x_node = cntr->New_ld(var_x, Spos());
    air::base::NODE_PTR y_node = cntr->New_ld(var_y, Spos());
    air::base::NODE_PTR mul_node =
        cntr->New_bin_arith(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                              fhe::ckks::CKKS_OPERATOR::MUL),
                            x_node, y_node, Spos());
    if (var_y->Type() == Ciph_ty()) {
      mul_node->Set_rtype(Ciph3_ty());
    } else {
      mul_node->Set_rtype(Ciph_ty());
    }
    air::base::STMT_PTR mul_stmt = cntr->New_stp(mul_node, preg_z, Spos());
    sl->Append(mul_stmt);
    return mul_stmt;
  }

  // var_z = rotate(var_x, 5)
  air::base::STMT_PTR Gen_rotate(air::base::CONTAINER*     cntr,
                                 air::base::ADDR_DATUM_PTR var_z,
                                 air::base::ADDR_DATUM_PTR var_x,
                                 int32_t                   rot_idx,
                                 air::base::STMT_LIST*     sl = NULL) {
    air::base::STMT_LIST stmt_list = cntr->Stmt_list();
    if (sl == NULL) {
      sl = &stmt_list;
    }
    air::base::NODE_PTR x_node    = cntr->New_ld(var_x, Spos());
    air::base::NODE_PTR n_rot_idx = cntr->New_intconst(
        cntr->Glob_scope()->Prim_type(air::base::PRIMITIVE_TYPE::INT_S32),
        rot_idx, Spos());
    air::base::NODE_PTR rotate_node =
        cntr->New_cust_node(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                              fhe::ckks::CKKS_OPERATOR::ROTATE),
                            Ciph_ty(), Spos());
    rotate_node->Set_child(0, x_node);
    rotate_node->Set_child(1, n_rot_idx);
    air::base::STMT_PTR rotate_stmt = cntr->New_st(rotate_node, var_z, Spos());
    sl->Append(rotate_stmt);
    return rotate_stmt;
  }

  // var_z = rotate(var_x, var_idx)
  air::base::STMT_PTR Gen_rotate(air::base::CONTAINER*     cntr,
                                 air::base::ADDR_DATUM_PTR var_z,
                                 air::base::ADDR_DATUM_PTR var_x,
                                 air::base::ADDR_DATUM_PTR var_idx,
                                 air::base::STMT_LIST*     sl = NULL) {
    air::base::STMT_LIST stmt_list = cntr->Stmt_list();
    if (sl == NULL) {
      sl = &stmt_list;
    }
    air::base::NODE_PTR x_node    = cntr->New_ld(var_x, Spos());
    air::base::NODE_PTR n_rot_idx = cntr->New_ld(var_idx, Spos());
    air::base::NODE_PTR rotate_node =
        cntr->New_cust_node(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                              fhe::ckks::CKKS_OPERATOR::ROTATE),
                            Ciph_ty(), Spos());
    rotate_node->Set_child(0, x_node);
    rotate_node->Set_child(1, n_rot_idx);
    air::base::STMT_PTR rotate_stmt = cntr->New_st(rotate_node, var_z, Spos());

    sl->Append(rotate_stmt);
    return rotate_stmt;
  }

  // preg_z = rotate(preg_x, rot_idx)
  air::base::STMT_PTR Gen_rotate(air::base::CONTAINER* cntr,
                                 air::base::PREG_PTR   preg_z,
                                 air::base::PREG_PTR preg_x, int32_t rot_idx) {
    air::base::STMT_LIST sl        = cntr->Stmt_list();
    air::base::NODE_PTR  x_node    = cntr->New_ldp(preg_x, Spos());
    air::base::NODE_PTR  n_rot_idx = cntr->New_intconst(
        cntr->Glob_scope()->Prim_type(air::base::PRIMITIVE_TYPE::INT_S32),
        rot_idx, Spos());
    air::base::NODE_PTR rotate_node =
        cntr->New_cust_node(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                              fhe::ckks::CKKS_OPERATOR::ROTATE),
                            Ciph_ty(), Spos());
    rotate_node->Set_child(0, x_node);
    rotate_node->Set_child(1, n_rot_idx);
    air::base::STMT_PTR rotate_stmt =
        cntr->New_stp(rotate_node, preg_z, Spos());
    sl.Append(rotate_stmt);
    return rotate_stmt;
  }

  // var_z = rescale(var_x)
  air::base::STMT_PTR Gen_rescale(air::base::CONTAINER*     cntr,
                                  air::base::ADDR_DATUM_PTR var_z,
                                  air::base::ADDR_DATUM_PTR var_x) {
    air::base::STMT_LIST sl           = cntr->Stmt_list();
    air::base::NODE_PTR  x_node       = cntr->New_ld(var_x, Spos());
    air::base::NODE_PTR  n_x          = cntr->New_ld(var_x, Spos());
    air::base::NODE_PTR  rescale_node = cntr->New_cust_node(
        air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                           fhe::ckks::CKKS_OPERATOR::RESCALE),
        Ciph_ty(), Spos());
    rescale_node->Set_child(0, n_x);
    air::base::STMT_PTR rescale_stmt =
        cntr->New_st(rescale_node, var_z, Spos());
    sl.Append(rescale_stmt);
    return rescale_stmt;
  }

  // var_z = relin(var_x)
  air::base::STMT_PTR Gen_relin(air::base::CONTAINER*     cntr,
                                air::base::ADDR_DATUM_PTR var_z,
                                air::base::ADDR_DATUM_PTR var_x) {
    air::base::STMT_LIST sl  = cntr->Stmt_list();
    air::base::NODE_PTR  n_x = cntr->New_ld(var_x, Spos());
    air::base::NODE_PTR  relin_node =
        cntr->New_una_arith(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                              fhe::ckks::CKKS_OPERATOR::RELIN),
                            n_x, Spos());
    relin_node->Set_rtype(Ciph_ty());
    air::base::STMT_PTR relin_stmt = cntr->New_st(relin_node, var_z, Spos());
    sl.Append(relin_stmt);
    return relin_stmt;
  }

  // preg_z = relin(var_x)
  air::base::STMT_PTR Gen_relin(air::base::CONTAINER*     cntr,
                                air::base::PREG_PTR       preg_z,
                                air::base::ADDR_DATUM_PTR var_x) {
    air::base::STMT_LIST sl  = cntr->Stmt_list();
    air::base::NODE_PTR  n_x = cntr->New_ld(var_x, Spos());
    air::base::NODE_PTR  relin_node =
        cntr->New_una_arith(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                              fhe::ckks::CKKS_OPERATOR::RELIN),
                            n_x, Spos());
    relin_node->Set_rtype(Ciph_ty());
    air::base::STMT_PTR relin_stmt = cntr->New_stp(relin_node, preg_z, Spos());
    sl.Append(relin_stmt);
    return relin_stmt;
  }

  // var_z = mul_float(x, f)
  air::base::STMT_PTR Gen_mul_float(air::base::CONTAINER*     cntr,
                                    air::base::ADDR_DATUM_PTR var_z,
                                    air::base::ADDR_DATUM_PTR var_x,
                                    long double               f) {
    air::base::STMT_LIST    sl        = cntr->Stmt_list();
    air::base::CONSTANT_PTR cst_float = cntr->Glob_scope()->New_const(
        air::base::CONSTANT_KIND::FLOAT,
        cntr->Glob_scope()->Prim_type(air::base::PRIMITIVE_TYPE::FLOAT_64), f);
    air::base::NODE_PTR n_x    = cntr->New_ld(var_x, Spos());
    air::base::NODE_PTR f_node = cntr->New_ldc(cst_float, Spos());
    air::base::NODE_PTR mul_float_node =
        cntr->New_bin_arith(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                              fhe::ckks::CKKS_OPERATOR::MUL),
                            n_x, f_node, Spos());
    air::base::STMT_PTR mul_float_stmt =
        cntr->New_st(mul_float_node, var_z, Spos());
    sl.Append(mul_float_stmt);
    return mul_float_stmt;
  }

  // var_z = encode()
  air::base::STMT_PTR Gen_encode(air::base::CONTAINER*     cntr,
                                 air::base::ADDR_DATUM_PTR var_p) {
    air::base::STMT_LIST     sl   = cntr->Stmt_list();
    air::base::GLOB_SCOPE*   glob = cntr->Glob_scope();
    air::base::PRIM_TYPE_PTR u32_type =
        glob->Prim_type(air::base::PRIMITIVE_TYPE::INT_U32);
    air::base::STR_PTR  type_name = glob->Undefined_name();
    air::base::ARB_PTR  arb       = glob->New_arb(1, 0, 1, 1);
    air::base::TYPE_PTR arr_type =
        glob->New_arr_type(type_name, u32_type, arb, Spos());
    air::base::NODE_PTR encode_node =
        cntr->New_cust_node(air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                                              fhe::ckks::CKKS_OPERATOR::ENCODE),
                            Plain_ty(), Spos());
    void*                   data_buffer = malloc(sizeof(uint32_t));
    air::base::CONSTANT_PTR cst =
        glob->New_const(air::base::CONSTANT_KIND::ARRAY, arr_type, data_buffer,
                        sizeof(uint32_t));

    encode_node->Set_child(0, cntr->New_ldc(cst, Spos()));
    encode_node->Set_child(1, cntr->New_intconst(u32_type, 4, Spos()));
    encode_node->Set_child(2, cntr->New_intconst(u32_type, 0, Spos()));
    encode_node->Set_child(3, cntr->New_intconst(u32_type, 0, Spos()));
    encode_node->Set_child(4, cntr->New_intconst(u32_type, 0, Spos()));
    air::base::STMT_PTR encode_stmt = cntr->New_st(encode_node, var_p, Spos());
    sl.Append(encode_stmt);
    return encode_stmt;
  }

  // tmp = bootstrap(var_x)
  air::base::STMT_PTR Gen_bootstrap(air::base::CONTAINER*     cntr,
                                    air::base::ADDR_DATUM_PTR var_x) {
    air::base::STMT_LIST sl       = cntr->Stmt_list();
    air::base::NODE_PTR  bts_node = cntr->New_cust_node(
        air::base::OPCODE(fhe::ckks::CKKS_DOMAIN::ID,
                           fhe::ckks::CKKS_OPERATOR::BOOTSTRAP),
        Ciph_ty(), Spos());
    bts_node->Set_child(0, cntr->New_ld(var_x, Spos()));
    air::base::PREG_PTR tmp = cntr->Parent_func_scope()->New_preg(Ciph_ty());
    air::base::STMT_PTR bts_stmt = cntr->New_stp(bts_node, tmp, Spos());
    sl.Append(bts_stmt);
    return bts_stmt;
  }

  // return var_z
  air::base::STMT_PTR Gen_ret(air::base::CONTAINER*     cntr,
                              air::base::ADDR_DATUM_PTR var_z) {
    air::base::STMT_LIST sl       = cntr->Stmt_list();
    air::base::NODE_PTR  ld_z     = cntr->New_ld(var_z, Spos());
    air::base::STMT_PTR  ret_stmt = cntr->New_retv(ld_z, Spos());
    sl.Append(ret_stmt);
    return ret_stmt;
  }

private:
  fhe::core::LOWER_CTX&     _lower_ctx;
  air::base::FUNC_SCOPE*    _main_graph;
  air::base::GLOB_SCOPE*    _glob;
  air::base::TYPE_PTR       _ciph_ty;
  air::base::TYPE_PTR       _ciph3_ty;
  air::base::TYPE_PTR       _plain_ty;
  air::base::ADDR_DATUM_PTR _v_input;
  air::base::ADDR_DATUM_PTR _v_output;
  air::base::SPOS           _spos;
};

}  // namespace test
}  // namespace poly
}  // namespace fhe

#endif
