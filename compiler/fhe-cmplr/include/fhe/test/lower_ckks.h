//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================
#ifndef FHE_TEST_LOWER_CKKS_H
#define FHE_TEST_LOWER_CKKS_H

#include "air/base/meta_info.h"
#include "air/base/st.h"
#include "air/base/transform_ctx.h"
#include "air/core/handler.h"
#include "air/core/opcode.h"
#include "fhe/ckks/ckks_gen.h"
#include "fhe/sihe/sihe_gen.h"
#include "fhe/test/gen_ckks_ir.h"
#include "fhe/util/util.h"
#include "gtest/gtest.h"

namespace fhe {
namespace poly {
namespace test {

class TEST_CONFIG {
public:
  TEST_CONFIG(const char* fname_suffix, fhe::poly::POLY_LAYER tgt_layer) {
    _fname = fname_suffix;
    switch (tgt_layer) {
      case fhe::poly::SPOLY:
        _config._lower_to_hpoly = false;
        _config._lower_to_lpoly = false;
        break;
      case fhe::poly::HPOLY:
        _config._lower_to_hpoly = true;
        _config._lower_to_lpoly = false;
        break;
      case fhe::poly::LPOLY:
        _config._lower_to_hpoly = true;
        _config._lower_to_lpoly = true;
        break;
      case fhe::poly::HPOLY_P2:
        _config._lower_to_hpoly  = true;
        _config._lower_to_hpoly2 = true;
        _config._lower_to_lpoly  = false;
        _config._prop_attr       = true;
        break;
      default:
        AIR_ASSERT_MSG(false, "invalid layer");
    }
  }
  fhe::poly::POLY_CONFIG&       Config() { return _config; }
  const fhe::poly::POLY_CONFIG& Config() const { return _config; }
  const char*                   Fname() const { return _fname; }
  uint32_t                      Input_level() const { return _input_level; }
  uint32_t                      Mul_level() const { return _mul_level; }
  void Set_input_level(uint32_t l) { _input_level = l; }
  void Set_mul_level(uint32_t l) { _mul_level = l; }

private:
  fhe::poly::POLY_CONFIG _config;
  const char*            _fname;
  uint32_t               _mul_level   = 21;
  uint32_t               _input_level = 3;
};

template <typename CONFIG>
class TEST_LOWER_CKKS : public ::testing::TestWithParam<CONFIG> {
protected:
  TEST_LOWER_CKKS() : _ckks_ir_gen(_fhe_ctx) {
    Fhe_ctx().Get_ctx_param().Set_poly_degree(16, false);
    Fhe_ctx().Get_ctx_param().Set_mul_level(this->GetParam().Mul_level(), true);
    Fhe_ctx().Get_ctx_param().Set_q_part_num(2);
  }

  void Set_input_level() {
    air::base::NODE_PTR func_entry = Container()->Entry_node();
    AIR_ASSERT(func_entry->Num_child() > 0);
    // set input level for idname
    for (uint32_t id = 0; id < func_entry->Num_child() - 1; ++id) {
      air::base::NODE_PTR child = func_entry->Child(id);
      AIR_ASSERT(child->Opcode() == air::core::OPC_IDNAME);
      Set_level_attr(child, this->GetParam().Input_level());
      Set_sf_degree(child, 1);
    }
  }

  void SetUp() override {
    _cntr = Create_ckks_ir(_fhe_ctx);

    std::string fname = ".";
    fname.append(
        ::testing::UnitTest::GetInstance()->current_test_info()->name());
    std::replace(fname.begin(), fname.end(), '/', '.');
    const CONFIG& param = this->GetParam();
    fname.append(param.Fname());
    _ofile.open(fname, std::ios::out);
    _config = param.Config();
  }

  void TearDown() override {
    if (_ofile.is_open()) {
      _ofile.close();
    }
    air::base::META_INFO::Remove_all();
    delete _cntr->Glob_scope();
  }

public:
  air::base::CONTAINER* Lower() {
    // set input level, after CKKS analyze
    Set_input_level();
    fhe::poly::POLY_DRIVER poly_driver;
    Ofile() << "CKKS Lower Before:" << std::endl;
    Ofile() << "====================================" << std::endl;
    Container()->Glob_scope()->Print(Ofile());
    air::base::GLOB_SCOPE* new_glob = poly_driver.Run(
        Config(), Container()->Glob_scope(), _fhe_ctx, &_driver_ctx);

    Ofile() << "CKKS Lower After:" << std::endl;
    Ofile() << "====================================" << std::endl;
    new_glob->Print_ir(Ofile());

    _cntr = &((*new_glob->Begin_func_scope()).Container());
    EXPECT_EQ(new_glob->Verify_ir(), true);
    return _cntr;
  }

  air::base::SPOS Spos() {
    return air::base::GLOB_SCOPE::Get()->Unknown_simple_spos();
  }
  fhe::poly::test::CKKS_IR_GEN& Ckks_ir_gen() { return _ckks_ir_gen; }
  fhe::poly::POLY_CONFIG&       Config() { return _config; }
  air::base::CONTAINER*         Container() { return _cntr; }
  air::driver::DRIVER_CTX*      Driver_ctx() { return &_driver_ctx; }
  fhe::core::LOWER_CTX&         Fhe_ctx() { return _fhe_ctx; };
  void Update_container(air::base::CONTAINER* cntr) { _cntr = cntr; }

  air::base::ADDR_DATUM_PTR Var_x() { return _var_x; }
  air::base::ADDR_DATUM_PTR Var_y() { return _var_y; }
  air::base::ADDR_DATUM_PTR Var_z() { return _var_z; }
  air::base::ADDR_DATUM_PTR Var_p() { return _var_p; }
  air::base::ADDR_DATUM_PTR Var_ciph3() { return _var_ciph3; }
  air::base::TYPE_PTR       Ciph_ty() { return Ckks_ir_gen().Ciph_ty(); }
  air::base::TYPE_PTR       Ciph3_ty() { return Ckks_ir_gen().Ciph3_ty(); }
  air::base::TYPE_PTR       Plain_ty() { return Ckks_ir_gen().Plain_ty(); }
  std::ofstream&            Ofile() { return _ofile; }

  air::base::CONTAINER* Create_ckks_ir(fhe::core::LOWER_CTX& lower_ctx) {
    _var_x     = Ckks_ir_gen().Gen_ciph_var("ciph_x");
    _var_y     = Ckks_ir_gen().Gen_ciph_var("ciph_y");
    _var_z     = Ckks_ir_gen().Gen_ciph_var("ciph_z");
    _var_p     = Ckks_ir_gen().Gen_plain_var("plain");
    _var_ciph3 = Ckks_ir_gen().Gen_ciph3_var("ciph3");
    return Ckks_ir_gen().Main_container();
  }

  void Set_level_attr(air::base::NODE_PTR node, uint32_t l) {
    node->Set_attr<uint32_t>(fhe::core::FHE_ATTR_KIND::LEVEL, &l, 1);
  }

  void Set_sf_degree(air::base::NODE_PTR node, uint32_t l) {
    node->Set_attr<uint32_t>(fhe::core::FHE_ATTR_KIND::SCALE, &l, 1);
  }

  void Set_rot_idx_attr(air::base::NODE_PTR  node,
                        std::vector<int32_t> rot_idxs) {
    int32_t* data = rot_idxs.data();
    node->Set_attr("nums", data, rot_idxs.size());
  }

  void Set_rescale_level(air::base::NODE_PTR node, uint32_t l) {
    node->Set_attr<uint32_t>(fhe::core::FHE_ATTR_KIND::RESCALE_LEVEL, &l, 1);
  }

private:
  air::driver::DRIVER_CTX      _driver_ctx;
  fhe::core::LOWER_CTX         _fhe_ctx;
  fhe::poly::test::CKKS_IR_GEN _ckks_ir_gen;
  fhe::poly::POLY_CONFIG       _config;
  air::base::CONTAINER*        _cntr = nullptr;
  std::ofstream                _ofile;
  air::base::ADDR_DATUM_PTR    _var_x;
  air::base::ADDR_DATUM_PTR    _var_y;
  air::base::ADDR_DATUM_PTR    _var_z;
  air::base::ADDR_DATUM_PTR    _var_p;
  air::base::ADDR_DATUM_PTR    _var_ciph3;
};

}  // namespace test
}  // namespace poly
}  // namespace fhe
#endif