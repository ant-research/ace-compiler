//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "ckks_cost_model.h"

#include <cmath>

#include "air/util/debug.h"
#include "fhe/ckks/ckks_opcode.h"

namespace fhe {
namespace ckks {
#define INVALID_COST MAXFLOAT
// latency(ms) of rotation at level [0, 18]
const static CKKS_OP_COST Rotate = CKKS_OP_COST(
    OPC_ROTATE, {58.422,  67.133,  77.521,  85.767,  93.799,  /*L0-L4*/
                 102.637, 111.902, 120.673, 130.940, 140.105, /*L5-L9*/
                 150.321, 227.607, 241.560, 255.688, 243.323, /*L10-L14*/
                 253.616, 290.575, 305.379, 325.038, 339.922, /*L15-L19*/
                 352.791, 366.287, 453.043, 470.903, 488.770, /*L20-L24*/
                 506.045, 523.305, 531.071, 548.304, 564.881, /*L25-L29*/
                 582.596, 598.225, /*L30-L31*/});
// latency(ms) of CKKS::add at level [0, 18]
const static CKKS_OP_COST Add =
    CKKS_OP_COST(OPC_ADD, {0.164, 0.319, 0.397, 0.541, 0.683, /*L0-L4*/
                           0.807, 1.084, 1.094, 1.402, 1.382, /*L5-L9*/
                           1.681, 1.905, 2.259, 2.420, 2.208, /*L10-L14*/
                           2.836, 3.088, 3.356, 3.685, 3.998, /*L15-L19*/
                           4.301, 4.539, 4.826, 5.101, 5.448, /*L20-L24*/
                           5.748, 6.024, 6.336, 6.559, 6.803, /*L25-L29*/
                           7.129, 7.678 /*L30-L31*/});

const static CKKS_OP_COST Sub = CKKS_OP_COST(OPC_SUB, {});

const static CKKS_OP_COST Mul_cost = CKKS_OP_COST(
    OPC_MUL, {INVALID_COST, 1.680,  2.509,  3.476,  4.237,  /*L0-L4*/
              5.170,        6.021,  6.773,  7.750,  8.513,  /*L5-L9*/
              9.280,        10.153, 11.129, 12.019, 13.053, /*L10-L14*/
              14.300,       15.638, 16.333, 17.534, 18.916, /*L15-L19*/
              20.072,       21.209, 22.467, 23.556, 24.654, /*L20-L24*/
              25.950,       26.891, 27.957, 29.299, 30.118, /*L25-L29*/
              31.076,       32.204 /*L30-L31*/});

const static CKKS_OP_COST Neg_cost = CKKS_OP_COST(OPC_NEG, {});

const static CKKS_OP_COST Encode = CKKS_OP_COST(
    OPC_ENCODE, {1.981,  3.267,  4.552,  5.840,  7.124,  /*L0-L4*/
                 8.407,  9.699,  10.981, 12.278, 13.603, /*L5-L9*/
                 14.860, 19.636, 20.987, 22.317, 23.694, /*L10-L14*/
                 25.029, 26.349, 27.709, 29.058, 30.425, /*L15-L19*/
                 31.771, 33.201, 34.547, 35.950, 37.312, /*L20-L24*/
                 38.612, 39.966, 41.370, 42.730, 44.145, /*L25-L29*/
                 45.504, 47.323, /*L30-L31*/});

const static CKKS_OP_COST Rescale = CKKS_OP_COST(
    OPC_RESCALE, {INVALID_COST, 5.547,   9.085,  11.927, 15.107, /*L0-L4*/
                  17.977,       21.333,  23.950, 27.535, 30.436, /*L5-L9*/
                  33.792,       36.779,  40.068, 43.022, 46.372, /*L10-L14*/
                  50.455,       53.958,  57.117, 60.514, 63.512, /*L15-L19*/
                  67.100,       70.041,  73.705, 76.704, 80.280, /*L20-L24*/
                  83.290,       86.983,  89.966, 93.482, 96.499, /*L25-L29*/
                  100.066,      103.812, /*L30-L31*/});

// const static CKKS_OP_COST Downscale = CKKS_OP_COST(OPC_DOWNSCALE, {});

const static CKKS_OP_COST Upscale = CKKS_OP_COST(OPC_UPSCALE, {});

const static CKKS_OP_COST Modswitch =
    CKKS_OP_COST(OPC_MODSWITCH, {
                                    0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
                                    0., 0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
                                    0., 0., 0., 0., 0., 0., 0., 0., 0., 0.,
                                });

const static CKKS_OP_COST Relin = CKKS_OP_COST(
    OPC_RELIN, {56.917,  66.917,  76.947,  85.529,  93.617,  /*L0-L4*/
                103.379, 111.819, 119.513, 130.493, 139.943, /*L5-L9*/
                149.586, 203.871, 215.768, 228.876, 242.031, /*L10-L14*/
                255.061, 262.308, 300.575, 314.451, 328.734, /*L15-L19*/
                334.446, 347.487, 414.455, 431.404, 448.205, /*L20-L24*/
                463.557, 480.168, 523.351, 540.551, 555.581, /*L25-L29*/
                573.083, 589.458, /*L30-L31*/});

const static CKKS_OP_COST Bootstrap =
    CKKS_OP_COST(OPC_BOOTSTRAP, {
                                    INVALID_COST,
                                    19316.356,
                                    21004.843,
                                    22464.008,
                                    23737.735,
                                    25133.126,
                                    26228.991,
                                    28077.301,
                                    30413.233,
                                    32810.135,
                                    34556.051,
                                    36115.279,
                                    37843.620,
                                    39522.544,
                                    41581.551,
                                    43554.812,
                                    44719.032,
                                    46257.028,
                                });

const static std::vector<const CKKS_OP_COST*> Fhe_op_cost = {
    &Rotate,  &Add,     &Sub,       &Mul_cost, &Neg_cost, &Encode,
    &Rescale, &Upscale, &Modswitch, &Relin,    &Bootstrap};  //&Downscale,

double Operation_cost(air::base::OPCODE opc, uint32_t level) {
  AIR_ASSERT_MSG(opc.Domain() == CKKS_DOMAIN::ID,
                 "currently only support cost of CKKS operation");
  const CKKS_OP_COST* opc_cost = Fhe_op_cost[opc.Operator()];
  AIR_ASSERT_MSG(opc_cost->Opcode() == opc,
                 "opcode inconsistent in OPERATION_COST");
  return opc_cost->Cost(level);
}

double Rescale_cost(uint32_t level, uint32_t poly_num) {
  AIR_ASSERT(poly_num >= 2);
  double rescale_cost = Operation_cost(OPC_RESCALE, level);
  // default rescale cost is the value of CIPHER2,
  // in case of ciphertext contains more polynomial,
  // need enlarge it with polynomial number.
  return (rescale_cost * poly_num / 2.);
}

}  // namespace ckks
}  // namespace fhe
