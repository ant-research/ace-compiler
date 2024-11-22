//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/util/app_composite_poly.h"

#include <cmath>
#include <iostream>
#include <memory>
#include <ostream>
#include <string>

#include "air/base/st_decl.h"
#include "air/util/debug.h"
#include "air/util/messg.h"

namespace fhe {
namespace util {

//! approximate composite polynomial of sign at alfa = 5 and mul_depth = 9
static const APP_SIGN_COMP_POLY::POLY_ARRAY App_sign_comp_poly_pow_alfa5_depth9{
    {
     {POLY_BASIS_TYPE::POWER,
         15,
         {0., 16.991912801003051892923261, 0., -394.30462944608592454353314, 0.,
          3732.9438341250469929346644, 0., -16694.033300999042855833984, 0.,
          39329.431040775967515596684, 0., -50248.794119518568012111608, 0.,
          32926.27463357162420222774, 0., -8667.9902964915960941020499}},
     {POLY_BASIS_TYPE::POWER,
         29,
         {0., 4.9658644770032308652625025, 0., -29.448884583925814589495991,
          0., 139.78371088903676355969164, 0., -465.9336214215280025199331,
          0., 1115.8611769977060195196749, 0., -1965.3906607101910315340303,
          0., 2585.8860916290664168457066, 0., -2562.0785113031476588750824,
          0., 1913.2925184471942343973977, 0., -1069.5452820334728267866371,
          0., 440.19390573542399694605194, 0., -129.32576670397024386464789,
          0., 25.645293406943162690472632, 0., -3.0739777273880675957413278,
          0., 0.16814265087412611753805143}},
     }
};

//! approximate composite polynomial of sign at alfa = 6 and mul_depth = 11
static const APP_SIGN_COMP_POLY::POLY_ARRAY
    App_sign_comp_poly_pow_alfa6_depth11{
        {
         {POLY_BASIS_TYPE::POWER,
             7,
             {0., 10.645107031164969866340373, 0., -60.640130864220915398757457,
              0., 111.10831333008641314686476, 0.,
              -60.947190919946087201028259}},
         {POLY_BASIS_TYPE::POWER,
             15,
             {0., 5.9065222807504452489467449, 0., -29.296824157667488408800074,
              0., 72.393055487666408825264757, 0., -89.863875429625467634015956,
              0., 60.343327011788152012290846, 0., -22.278541554980650296657923,
              0., 4.2523390466080470408814646, 0.,
              -0.3277374540289793557993192}},
         {POLY_BASIS_TYPE::POWER,
             13,
             {0., 2.96565989985919302305495, 0., -6.0334871135992003605057485,
              0., 9.1410865513515385934302672, 0., -8.7314926734132325042445658,
              0., 5.0729454400189282207173291, 0., -1.6422691064348403158912445,
              0., 0.22755720438375995864878361}},
         }
};

//! approximate composite polynomial of sign at alfa = 6 and mul_depth = 11
static const APP_SIGN_COMP_POLY::POLY_ARRAY
    App_sign_comp_poly_chebyshev_alfa6_depth11{
        {
         {POLY_BASIS_TYPE::CHEBYSHEV,
             7,
             {
                 0.,
                 1.277209679957775013e+00,
                 0.,
                 -4.369818210105346212e-01,
                 0.,
                 2.781705762612975419e-01,
                 0.,
                 -9.522998581241576277e-01,
             }},
         {POLY_BASIS_TYPE::CHEBYSHEV,
             15,
             {
                 0.,
                 1.336811809725395372e+00,
                 0.,
                 -3.314086854871873267e-01,
                 0.,
                 2.739009935511804161e-01,
                 0.,
                 -2.096678512577555831e-01,
                 0.,
                 6.827141455300124451e-02,
                 0.,
                 -1.036056317926726048e-02,
                 0.,
                 7.381161118162535544e-04,
                 0.,
                 -2.000350671563594715e-05,
             }},
         {POLY_BASIS_TYPE::CHEBYSHEV,
             13,
             {
                 0.,
                 1.229917329338358289e+00,
                 0.,
                 -3.099894039867301943e-01,
                 0.,
                 1.047929208484282559e-01,
                 0.,
                 -3.040264421328875422e-02,
                 0.,
                 6.507995190210730772e-03,
                 0.,
                 -8.815509689332230855e-04,
                 0.,
                 5.555595810150389487e-05,
             }},
         }
};

//! approximate composite polynomial of sign at alfa = 9 and mul_depth = 13
static const APP_SIGN_COMP_POLY::POLY_ARRAY
    App_sign_comp_poly_chebyshev_alfa9_depth13{
        {POLY_BASIS_TYPE::CHEBYSHEV,
         15, {
             0.,
             1.274244441439567055e+00,
             0.,
             -4.274610154279958607e-01,
             0.,
             2.598417608934820988e-01,
             0.,
             -1.894160321998888952e-01,
             0.,
             1.516157904980795224e-01,
             0.,
             -1.289471808964555988e-01,
             0.,
             1.148389592593351827e-01,
             0.,
             -1.006755030034787834e+00,
         }},

        {POLY_BASIS_TYPE::CHEBYSHEV,
         15, {
             0.,
             1.504797731281392936e+00,
             0.,
             -1.262993831946355172e-01,
             0.,
             5.310374803122150933e-01,
             0.,
             -4.763164287058726520e-01,
             0.,
             1.404090303951424090e-01,
             0.,
             -1.856485351687612792e-02,
             0.,
             1.142402954164560992e-03,
             0.,
             -2.667926441648920576e-05,
         }},

        {POLY_BASIS_TYPE::CHEBYSHEV,
         27, {
             0., 1.258870573407572691e+00, 0., -3.830661449095234539e-01,
             0., 1.909371044429533648e-01, 0., -1.025700865042690896e-01,
             0., 5.364833181833868897e-02, 0., -2.602904444646918572e-02,
             0., 1.119529495100999271e-02, 0., -3.976394146723259693e-03,
             0., 1.080475747158062428e-03, 0., -2.115428631766840754e-04,
             0., 2.840163212584644305e-05, 0., -2.461531419370990484e-06,
             0., 1.235599278444410819e-07, 0., -2.723078631019510824e-09,
         }}
};

//! return the greatest power of 2 that is le deg
static uint32_t Floor_pow2(uint32_t deg) {
  uint32_t res = 1;
  while (res <= deg) {
    res <<= 1;
  }
  return (res >> 1);
}

//! return the least power of 2 that is ge deg
static uint32_t Ceil_pow2(uint32_t deg) {
  uint32_t res = 1;
  while (res < deg) {
    res <<= 1;
  }
  return res;
}

uint32_t POLY_DATA::Get_full_decompose_deg() const {
  uint32_t poly_deg  = Get_deg();
  uint32_t ceil_pow2 = Ceil_pow2(poly_deg + 1);
  uint32_t low_pow   = floor(log2(ceil_pow2) / 2);
  uint32_t high_deg  = ceil_pow2 - std::pow(2, low_pow) / 2;
  return high_deg;
}

uint32_t POLY_DATA::Get_precompute_deg() const {
  uint32_t poly_deg  = Get_deg();
  uint32_t ceil_pow2 = Ceil_pow2(poly_deg + 1);
  uint32_t low_pow2  = floor(log2(ceil_pow2) / 2);
  uint32_t low_deg   = pow(2, low_pow2);
  return low_deg;
}

void POLY_DATA::Print(std::ostream& out, uint32_t indent) const {
  std::string indent_str0(indent, ' ');
  std::string indent_str1(indent + 4U, ' ');
  out << indent_str0 << "POLY_DATA {" << std::endl;

  out << indent_str1 << "Poly_base_type: ";
  if (Get_basis_type() == POLY_BASIS_TYPE::POWER) {
    out << "power" << std::endl;
  } else if (Get_basis_type() == POLY_BASIS_TYPE::CHEBYSHEV) {
    out << "chebyshev" << std::endl;
  } else {
    Templ_print(out, "not support poly base type.");
    AIR_ASSERT(false);
  }
  out << indent_str1 << "Poly deg= " << Get_deg() << std::endl;

  uint32_t deg        = 0;
  uint32_t deg_stride = Deg_stride();
  out << indent_str1 << "Poly_coeffs {" << std::endl;
  std::string indent_str2(indent + 8U, ' ');
  for (float coeff : Get_coeff()) {
    out << indent_str2 << coeff << ", // coeff of item at deg= " << deg
        << std::endl;
    deg += deg_stride;
  }
  out << indent_str1 << "}" << std::endl;
  out << indent_str0 << "}" << std::endl;
}

void POLY_TREE_NODE::Decompose_pow_basis_poly(uint32_t full_decompose_deg,
                                              uint32_t precompute_deg) {
  const POLY_DATA& cur_poly = Get_poly_data();
  AIR_ASSERT(cur_poly.Get_basis_type() == POLY_BASIS_TYPE::POWER);

  uint32_t deg = cur_poly.Get_deg();
  if (deg <= cur_poly.Get_min_deg() ||
      (deg < precompute_deg && Get_actual_deg() < full_decompose_deg)) {
    return;
  }

  // 1. cal pow of divisor item
  uint32_t floor_pow2 = Floor_pow2(deg);
  Set_divisor_pow(std::log2(floor_pow2));

  POLY_BASIS_TYPE basis_type = cur_poly.Get_basis_type();
  uint32_t        deg_stride = cur_poly.Deg_stride();

  // 2. cal remainder polynomial tree node
  const POLY_DATA::POLY_COEFF&     cur_poly_coeff = cur_poly.Get_coeff();
  POLY_DATA::POLY_COEFF_CONST_ITER begin_iter     = cur_poly_coeff.begin();
  uint32_t coeff_num_r = (floor_pow2 + (deg_stride - 1)) / deg_stride;
  POLY_DATA::POLY_COEFF coeff_r(begin_iter, begin_iter + coeff_num_r);
  uint32_t              deg_r = (coeff_num_r - 1) * deg_stride;
  POLY_DATA             poly_r(basis_type, deg_r, std::move(coeff_r));
  uint32_t              actual_deg_r =
      Get_actual_deg() - (cur_poly_coeff.size() - coeff_r.size()) * deg_stride;

  POLY_TREE_NODE* tree_node_r =
      new POLY_TREE_NODE(std::move(poly_r), actual_deg_r);
  Set_poly_r(tree_node_r);
  Get_poly_r()->Decompose_pow_basis_poly(full_decompose_deg, precompute_deg);

  // 3. cal quotient polynomial tree node
  POLY_DATA::POLY_COEFF_CONST_ITER end_iter = cur_poly.Get_coeff().end();
  POLY_DATA::POLY_COEFF            coeff_q(begin_iter + coeff_num_r, end_iter);
  uint32_t                         deg_q = deg - floor_pow2;
  POLY_DATA poly_q(basis_type, deg_q, std::move(coeff_q));

  POLY_TREE_NODE* tree_node_q =
      new POLY_TREE_NODE(std::move(poly_q), Get_actual_deg());
  Set_poly_q(tree_node_q);
  Get_poly_q()->Decompose_pow_basis_poly(full_decompose_deg, precompute_deg);
}

void POLY_TREE_NODE::Decompose_chebyshev_basis_poly(uint32_t full_decompose_deg,
                                                    uint32_t precompute_deg) {
  const POLY_DATA& cur_poly = Get_poly_data();
  AIR_ASSERT(cur_poly.Get_basis_type() == POLY_BASIS_TYPE::CHEBYSHEV);

  uint32_t deg = cur_poly.Get_deg();
  if (deg <= cur_poly.Get_min_deg() ||
      (deg < precompute_deg && Get_actual_deg() < full_decompose_deg)) {
    return;
  }

  // 1. cal pow of divisor item
  uint32_t divisor_deg = Floor_pow2(deg);
  Set_divisor_pow(std::log2(divisor_deg));

  POLY_BASIS_TYPE basis_type  = cur_poly.Get_basis_type();
  uint32_t        deg_stride  = cur_poly.Deg_stride();
  uint32_t        coeff_num_r = divisor_deg;

  const POLY_DATA::POLY_COEFF&     cur_poly_coeff = cur_poly.Get_coeff();
  POLY_DATA::POLY_COEFF_CONST_ITER begin_iter     = cur_poly_coeff.begin();
  POLY_DATA::POLY_COEFF_CONST_ITER end_iter       = cur_poly.Get_coeff().end();

  // 2. cal coefficient of quotient and remainder polynomial
  POLY_DATA::POLY_COEFF coeff_q(begin_iter + divisor_deg, end_iter);
  POLY_DATA::POLY_COEFF coeff_r(begin_iter, begin_iter + divisor_deg);
  AIR_ASSERT(divisor_deg >= coeff_q.size());
  for (uint32_t term_deg = 1; term_deg < coeff_q.size(); ++term_deg) {
    coeff_r[divisor_deg - term_deg] -= coeff_q[term_deg];
  }
  for (uint32_t term_deg = 1; term_deg < coeff_q.size(); ++term_deg) {
    coeff_q[term_deg] *= 2.;
  }

  // 3. gen tree node for quotient and remainder polynomial
  uint32_t        deg_q = coeff_q.size() - 1;
  POLY_DATA       poly_q(basis_type, deg_q, std::move(coeff_q));
  POLY_TREE_NODE* tree_node_q =
      new POLY_TREE_NODE(std::move(poly_q), Get_actual_deg());

  uint32_t  deg_r = coeff_r.size() - 1;
  POLY_DATA poly_r(basis_type, deg_r, std::move(coeff_r));
  uint32_t  actual_deg_r =
      Get_actual_deg() - (cur_poly_coeff.size() - coeff_r.size()) * deg_stride;
  POLY_TREE_NODE* tree_node_r =
      new POLY_TREE_NODE(std::move(poly_r), actual_deg_r);

  Set_poly_r(tree_node_r);
  Get_poly_r()->Decompose_chebyshev_basis_poly(full_decompose_deg,
                                               precompute_deg);

  Set_poly_q(tree_node_q);
  Get_poly_q()->Decompose_chebyshev_basis_poly(full_decompose_deg,
                                               precompute_deg);
}

void APP_SIGN_COMP_POLY::Register_app_sign_comp_poly() {
  uint32_t mul_depth = 9;
  _app_sign_comp_poly.insert({
      POLY_INFO{POLY_BASIS_TYPE::POWER, mul_depth},
      &App_sign_comp_poly_pow_alfa5_depth9
  });

  mul_depth = 11;
  _app_sign_comp_poly.insert({
      POLY_INFO{POLY_BASIS_TYPE::POWER, mul_depth},
      &App_sign_comp_poly_pow_alfa6_depth11
  });

  _app_sign_comp_poly.insert({
      POLY_INFO{POLY_BASIS_TYPE::CHEBYSHEV, mul_depth},
      &App_sign_comp_poly_chebyshev_alfa6_depth11
  });

  mul_depth = 13;
  _app_sign_comp_poly.insert({
      POLY_INFO{POLY_BASIS_TYPE::CHEBYSHEV, mul_depth},
      &App_sign_comp_poly_chebyshev_alfa9_depth13
  });
}

void APP_SIGN_COMP_POLY::Print(std::ostream& out, uint32_t indent) {
  std::string indent_str0(indent, ' ');
  std::string indent_str1(indent + 4U, ' ');
  out << indent_str0 << "APP_SIGN_COMP_POLY {" << std::endl;
  for (const POLY_DATA& poly : *_app_sign_comp_poly.begin()->second) {
    poly.Print(out, indent + 4U);
  }
  out << indent_str0 << "}" << std::endl;
}

}  // namespace util
}  // namespace fhe