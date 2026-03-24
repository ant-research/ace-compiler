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

namespace fhe {
namespace ckks {

#define INVALID_COST MAXFLOAT
// latency(ms) of CKKS::rotation at N= 2^16 and level [0, 31]
const static CKKS_OP_COST Rotate_deg65536 = CKKS_OP_COST(
    OPC_ROTATE, {58.422,  67.133,  77.521,  85.767,  93.799,  /*L0-L4*/
                 102.637, 111.902, 120.673, 130.940, 140.105, /*L5-L9*/
                 150.321, 227.607, 241.560, 255.688, 243.323, /*L10-L14*/
                 253.616, 290.575, 305.379, 325.038, 339.922, /*L15-L19*/
                 352.791, 366.287, 453.043, 470.903, 488.770, /*L20-L24*/
                 506.045, 523.305, 531.071, 548.304, 564.881, /*L25-L29*/
                 582.596, 598.225, /*L30-L31*/});

// latency(ms) of CKKS::add at N= 2^16 and level [0, 31]
const static CKKS_OP_COST Add_deg65536 =
    CKKS_OP_COST(OPC_ADD, {0.164, 0.319, 0.397, 0.541, 0.683, /*L0-L4*/
                           0.807, 1.084, 1.094, 1.402, 1.382, /*L5-L9*/
                           1.681, 1.905, 2.208, 2.259, 2.420, /*L10-L14*/
                           2.836, 3.088, 3.356, 3.685, 3.998, /*L15-L19*/
                           4.301, 4.539, 4.826, 5.101, 5.448, /*L20-L24*/
                           5.748, 6.024, 6.336, 6.559, 6.803, /*L25-L29*/
                           7.129, 7.678 /*L30-L31*/});

// latency(ms) of CKKS::mulcc at N= 2^16 and level [0, 31]
const static CKKS_OP_COST Mul_deg65536 = CKKS_OP_COST(
    OPC_MUL, {INVALID_COST, 1.680,  2.509,  3.476,  4.237,  /*L0-L4*/
              5.170,        6.021,  6.773,  7.750,  8.513,  /*L5-L9*/
              9.280,        10.153, 11.129, 12.019, 13.053, /*L10-L14*/
              14.300,       15.638, 16.333, 17.534, 18.916, /*L15-L19*/
              20.072,       21.209, 22.467, 23.556, 24.654, /*L20-L24*/
              25.950,       26.891, 27.957, 29.299, 30.118, /*L25-L29*/
              31.076,       32.204 /*L30-L31*/});

// latency(ms) of CKKS::encode at N= 2^16 and level [0, 31]
const static CKKS_OP_COST Encode_deg65536 = CKKS_OP_COST(
    OPC_ENCODE, {1.981,  3.267,  4.552,  5.840,  7.124,  /*L0-L4*/
                 8.407,  9.699,  10.981, 12.278, 13.603, /*L5-L9*/
                 14.860, 19.636, 20.987, 22.317, 23.694, /*L10-L14*/
                 25.029, 26.349, 27.709, 29.058, 30.425, /*L15-L19*/
                 31.771, 33.201, 34.547, 35.950, 37.312, /*L20-L24*/
                 38.612, 39.966, 41.370, 42.730, 44.145, /*L25-L29*/
                 45.504, 47.323, /*L30-L31*/});

// latency(ms) of CKKS::rescale at N= 2^16 and level [0, 31]
const static CKKS_OP_COST Rescale_deg65536 = CKKS_OP_COST(
    OPC_RESCALE, {INVALID_COST, 5.547,   9.085,  11.927, 15.107, /*L0-L4*/
                  17.977,       21.333,  23.950, 27.535, 30.436, /*L5-L9*/
                  33.792,       36.779,  40.068, 43.022, 46.372, /*L10-L14*/
                  50.455,       53.958,  57.117, 60.514, 63.512, /*L15-L19*/
                  67.100,       70.041,  73.705, 76.704, 80.280, /*L20-L24*/
                  83.290,       86.983,  89.966, 93.482, 96.499, /*L25-L29*/
                  100.066,      103.812, /*L30-L31*/});

// latency(ms) of CKKS::relin at N= 2^16 and level [0, 31]
const static CKKS_OP_COST Relin_deg65536 = CKKS_OP_COST(
    OPC_RELIN, {56.887,  66.917,  76.947,  85.529,  93.617,  /*L0-L4*/
                103.379, 111.819, 119.513, 130.493, 139.943, /*L5-L9*/
                149.586, 203.871, 215.768, 228.876, 242.031, /*L10-L14*/
                255.061, 262.308, 300.575, 314.451, 328.734, /*L15-L19*/
                334.446, 347.487, 414.455, 431.404, 448.205, /*L20-L24*/
                463.557, 480.168, 523.351, 540.551, 555.581, /*L25-L29*/
                573.083, 589.458, /*L30-L31*/});

// latency(ms) of CKKS::bootstrap at N= 2^16 and level [0, 17]
const static CKKS_OP_COST Bootstrap_deg65536 = CKKS_OP_COST(
    OPC_BOOTSTRAP,
    {INVALID_COST, 19316.356, 21004.843, 22464.008, 23737.735, /*L0-L4*/
     25133.126, 26228.991, 28077.301, 30413.233, 32810.135,    /*L5-L9*/
     34556.051, 36115.279, 37843.620, 39522.544, 41581.551,    /*L10-L14*/
     43554.812, 44719.032, 46257.028,
     /*L15-L17*/});

const static CKKS_OP_COST Modswitch = CKKS_OP_COST(OPC_MODSWITCH, 0., 31);
// currently not used opcode
const static CKKS_OP_COST Sub     = CKKS_OP_COST(OPC_SUB, {});
const static CKKS_OP_COST Neg     = CKKS_OP_COST(OPC_NEG, {});
const static CKKS_OP_COST Upscale = CKKS_OP_COST(OPC_UPSCALE, {});

//! @brief Fhe_op_cost_deg65536 provides a summary of the cost of FHE operations
//! at polynomial degree N= 2^16.
const static std::vector<const CKKS_OP_COST*> Fhe_op_cost_deg65536 = {
    &Rotate_deg65536,
    &Add_deg65536,
    &Sub,
    &Mul_deg65536,
    &Neg,
    &Encode_deg65536,
    &Rescale_deg65536,
    &Upscale,
    &Modswitch,
    &Relin_deg65536,
    &Bootstrap_deg65536,
};

// latency(ms) of rotation at poly_degree= 2^17 and level [0, 31]
const static CKKS_OP_COST Rotate_deg131072 = CKKS_OP_COST(
    OPC_ROTATE, {142.724,  146.293,  161.355,  210.270,  230.666,  /*L0-L4*/
                 249.794,  269.995,  286.087,  306.715,  326.405,  /*L5-L9*/
                 348.204,  472.378,  504.645,  529.753,  549.684,  /*L10-L14*/
                 578.744,  607.641,  635.111,  653.817,  682.185,  /*L15-L19*/
                 704.693,  733.281,  909.868,  948.561,  983.870,  /*L20-L24*/
                 1040.362, 1071.283, 1110.485, 1144.498, 1178.817, /*L25-L29*/
                 1223.600, 1230.847, /*L30-L31*/});

// latency(ms) of CKKS::add at poly_degree= 2^17 and level [0, 31]
const static CKKS_OP_COST Add_deg131072 =
    CKKS_OP_COST(OPC_ADD, {0.308,  0.587,  0.883,  1.175,  1.893,  /*L0-L4*/
                           2.085,  2.597,  3.152,  3.817,  4.301,  /*L5-L9*/
                           4.762,  5.405,  5.837,  6.378,  6.771,  /*L10-L14*/
                           7.349,  7.958,  8.545,  9.684,  10.648, /*L15-L19*/
                           10.725, 11.351, 12.053, 13.027, 13.710, /*L20-L24*/
                           13.915, 14.187, 15.426, 15.690, 16.131, /*L25-L29*/
                           16.561, 17.046, /*L30-L31*/});

// latency(ms) of CKKS::mulcc at poly_degree= 2^17 and level [0, 31]
const static CKKS_OP_COST Mul_deg131072 = CKKS_OP_COST(
    OPC_MUL, {
                 INVALID_COST, 3.343,  5.047,  6.740,  8.596,  /*L0-L4*/
                 10.684,       12.648, 15.462, 17.452, 19.642, /*L5-L9*/
                 22.038,       24.257, 25.412, 27.744, 30.506, /*L10-L14*/
                 32.403,       34.642, 36.833, 39.259, 41.432, /*L15-L19*/
                 43.630,       45.953, 48.484, 50.197, 51.951, /*L20-L24*/
                 55.187,       55.702, 58.361, 60.698, 62.709, /*L25-L29*/
                 65.265,       72.253,                         /*L30-L31*/
             });

// latency(ms) of CKKS::encode at poly_degree= 2^17 and level [0, 31]
const static CKKS_OP_COST Encode_deg131072 = CKKS_OP_COST(
    OPC_ENCODE, {
                    11.297, 13.651,  16.534, 19.381, 24.216, /*L0-L4*/
                    26.454, 29.426,  32.306, 35.293, 38.371, /*L5-L9*/
                    41.406, 44.278,  45.437, 48.541, 52.136, /*L10-L14*/
                    54.177, 57.442,  59.986, 64.592, 67.503, /*L15-L19*/
                    70.618, 73.539,  76.102, 78.951, 82.356, /*L20-L24*/
                    84.625, 88.039,  90.524, 93.257, 96.633, /*L25-L29*/
                    99.229, 101.843,                         /*L30-L31*/
                });

// latency(ms) of CKKS::rescale at poly_degree= 2^17 and level [0, 31]
const static CKKS_OP_COST Rescale_deg131072 = CKKS_OP_COST(
    OPC_RESCALE, {INVALID_COST, 11.633,  19.011,  25.177,  32.457,  /*L0-L4*/
                  39.529,       46.718,  52.952,  60.491,  66.780,  /*L5-L9*/
                  74.214,       80.577,  87.608,  93.361,  100.980, /*L10-L14*/
                  107.217,      114.613, 120.846, 128.628, 135.234, /*L15-L19*/
                  142.615,      148.987, 156.320, 163.031, 170.437, /*L20-L24*/
                  176.400,      184.103, 189.896, 197.207, 203.816, /*L25-L29*/
                  211.294,      212.714, /*L30-L31*/});

// latency(ms) of CKKS::relin at poly_degree= 2^17 and level [0, 31]
const static CKKS_OP_COST Relin_deg131072 = CKKS_OP_COST(
    OPC_RELIN, {125.146,  143.041,  160.936,  178.893,  199.502,  /*L0-L4*/
                243.180,  261.962,  281.200,  300.278,  320.011,  /*L5-L9*/
                339.831,  449.913,  476.223,  508.681,  541.858,  /*L10-L14*/
                567.953,  594.505,  622.482,  642.904,  668.792,  /*L15-L19*/
                697.864,  727.126,  903.809,  935.409,  973.747,  /*L20-L24*/
                1006.919, 1041.947, 1071.073, 1115.276, 1144.581, /*L25-L29*/
                1180.513, 1199.452 /*L30-L31*/});

// latency(ms) of CKKS::bootstrap at poly_degree= 2^17 and level [0, 31]
const static CKKS_OP_COST Bootstrap_deg131072 = CKKS_OP_COST(
    OPC_BOOTSTRAP,
    {INVALID_COST, 52065.553, 56130.527, 59976.537, 62360.028, /*L0-L4*/
     66107.424, 69902.344, 74689.894, 80639.685, 85906.506,    /*L5-L9*/
     90376.269, 94190.792, 99224.003, 103165.413, 107099.946,  /*L10-L14*/
     111336.525, 116486.584, 121013.544,
     /*L15-L17*/});

//! @brief Fhe_op_cost_deg131072 provides a summary of the cost of FHE
//! operations at polynomial degree N= 2^17.
const static std::vector<const CKKS_OP_COST*> Fhe_op_cost_deg131072 = {
    &Rotate_deg131072,
    &Add_deg131072,
    &Sub,
    &Mul_deg131072,
    &Neg,
    &Encode_deg131072,
    &Rescale_deg131072,
    &Upscale,
    &Modswitch,
    &Relin_deg131072,
    &Bootstrap_deg131072,
};

double Operation_cost(air::base::OPCODE opc, uint32_t level,
                      uint32_t poly_deg) {
  AIR_ASSERT_MSG(opc.Domain() == CKKS_DOMAIN::ID,
                 "currently only support cost of CKKS operation");
  const CKKS_OP_COST* opc_cost = (poly_deg >= 131072)
                                     ? Fhe_op_cost_deg131072[opc.Operator()]
                                     : Fhe_op_cost_deg65536[opc.Operator()];
  AIR_ASSERT_MSG(opc_cost->Opcode() == opc,
                 "opcode inconsistent with CKKS_OP_COST");
  return opc_cost->Cost(level);
}

double Rescale_cost(uint32_t level, uint32_t poly_num, uint32_t poly_deg) {
  AIR_ASSERT_MSG(poly_num >= 2,
                 "polynomial number of a cipher must be at least 2");
  double rescale_cost = Operation_cost(OPC_RESCALE, level, poly_deg);
  // Default rescaling cost is the value of CIPHER2 of which poly_num is 2.
  // In case of the ciphertext contains more polynomials, adjust the cost by
  // a factor of poly_num/2.
  return (rescale_cost * poly_num / 2.);
}

}  // namespace ckks
}  // namespace fhe
