//-*-c-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_INCLUDE_UTIL_CKKS_BOOTSTRAP_CONTEXT_H
#define RTLIB_INCLUDE_UTIL_CKKS_BOOTSTRAP_CONTEXT_H

#include "util/ciphertext.h"
#include "util/ckks_encoder.h"
#include "util/ckks_evaluator.h"
#include "util/ckks_key_generator.h"
#include "util/ckks_parameters.h"
#include "util/fhe_types.h"
#include "util/matrix_operations.h"
#include "util/plaintext.h"

#ifdef __cplusplus
extern "C" {
#endif

// TODO: move these macros to config
#define FIXED_MANUAL    1
#define AUTO_SCALE      0
#define AUTO_SCALE_EXT  0
#define SPARSE_TERNARY  0
#define UNIFORM_TERNARY 1

// 1. info of polynomial in sparse secret case
#define K_SPARSE \
  14  //!< upper bound for the number of overflows in the sparse secret case

#define R_SPARSE \
  3  //!< number of double-angle iterations in CKKS bootstrapping.  Must be
     //!< static because it is used in a static function.

//! @brief Chebyshev series coefficients for the SPARSE case
#define SPARSE_COEFF_SIZE 45
static const double G_coefficients_sparse[SPARSE_COEFF_SIZE] = {
    -0.18646470117093214,    0.036680543700430925,     -0.20323558926782626,
    0.029327390306199311,    -0.24346234149506416,     0.011710240188138248,
    -0.27023281815251715,    -0.017621188001030602,    -0.21383614034992021,
    -0.048567932060728937,   -0.013982336571484519,    -0.051097367628344978,
    0.24300487324019346,     0.0016547743046161035,    0.23316923792642233,
    0.060707936480887646,    -0.18317928363421143,     0.0076878773048247966,
    -0.24293447776635235,    -0.071417413140564698,    0.37747441314067182,
    0.065154496937795681,    -0.24810721693607704,     -0.033588418808958603,
    0.10510660697380972,     0.012045222815124426,     -0.032574751830745423,
    -0.0032761730196023873,  0.0078689491066424744,    0.00070965574480802061,
    -0.0015405394287521192,  -0.00012640521062948649,  0.00025108496615830787,
    0.000018944629154033562, -0.000034753284216308228, -2.4309868106111825e-6,
    4.1486274737866247e-6,   2.7079833113674568e-7,    -4.3245388569898879e-7,
    -2.6482744214856919e-8,  3.9770028771436554e-8,    2.2951153557906580e-9,
    -3.2556026220554990e-9,  -1.7691071323926939e-10,  2.5459052150406730e-10};

// 2. info of polynomial with hamming_weight <= 192
#define HAMMING_WEIGHT_THRESHOLD 192
#define R_UNIFORM_HW_192 \
  3  //!< number of double-angle iterations in CKKS bootstrapping.  Must be
     //!< static because it is used in a static function.

#define K_UNIFORM_HW_192 \
  32  //!< upper bound for the number of overflows in the uniform secret case

//!< Chebyshev series coefficients for the OPTIMIZED/uniform case
#define UNIFORM_COEFF_SIZE_HW_192 55
static const double G_coefficients_uniform_hw_192[UNIFORM_COEFF_SIZE_HW_192] = {
    // deg= 54, K= 32, R= 3
    1.74551960283504837e-01,  -3.43838095837535329e-02,
    1.88307649106864788e-01,  -2.84223873992535993e-02,
    2.22419882865789564e-01,  -1.43397005803286518e-02,
    2.51103798550390944e-01,  9.50854609032555226e-03,
    2.24475678532524398e-01,  3.79342483118012136e-02,
    8.78908877085935597e-02,  5.18464470537667449e-02,
    -1.40269389175310705e-01, 2.52026526332414826e-02,
    -2.71343812500084935e-01, -3.49285487170959558e-02,
    -6.17395308539803664e-02, -5.05648932050318592e-02,
    2.82155868186952818e-01,  2.98272328751879069e-02,
    5.54332147538673034e-02,  4.73762170911353267e-02,
    -3.42589653109854397e-01, -7.19260908452365733e-02,
    3.19234546310780576e-01,  4.93494016031356467e-02,
    -1.74337152324168188e-01, -2.23994935740034137e-02,
    6.76154588798445894e-02,  7.56838175610476029e-03,
    -2.01915893273537893e-02, -2.01996389480041394e-03,
    4.85990579019698801e-03,  4.41705640530539389e-04,
    -9.71526466295980677e-04, -8.11544278739113802e-05,
    1.64814371135792263e-04,  1.27637159472312703e-05,
    -2.41183607585707303e-05, -1.74347427937465971e-06,
    3.08411936249047440e-06,  2.09259735883450997e-07,
    -3.48280526734833634e-07, -2.22825972864890841e-08,
    3.50404774489712212e-08,  2.12216680463557985e-09,
    -3.16453692971713038e-09, -1.82031853692548044e-10,
    2.58203419199988530e-10,  1.41483617957390541e-11,
    -1.91412743082734574e-11, -1.00089939783634691e-12,
    1.29702147256041809e-12,  6.67556346626149772e-14,
    -7.81869621069283006e-14,
};

// 3. info of even polynomial with hamming_weight <= 192
#define R_UNIFORM_EVEN_HW_192 \
  3  //!< number of double-angle iterations in CKKS bootstrapping.  Must be
     //!< static because it is used in a static function.

#define K_UNIFORM_EVEN_HW_192 \
  32  //!< upper bound for the number of overflows in the uniform secret case
//!< Chebyshev series coefficients for the OPTIMIZED/uniform case

#define UNIFORM_EVEN_COEFF_SIZE_HW_192 55
static const double
    G_even_coefficients_uniform_hw_192[UNIFORM_EVEN_COEFF_SIZE_HW_192] = {
        // deg= 54, K= 32, R= 3
        1.77971635352991098e-01,  0., 1.91996814052353887e-01,  0.,
        2.26777345979688877e-01,  0., 2.56023212794501354e-01,  0.,
        2.28873417064574675e-01,  0., 8.96127719947621554e-02,  0.,
        -1.43017428970428406e-01, 0., -2.76659752059622344e-01, 0.,
        -6.29490797706480504e-02, 0., 2.87683628440008832e-01,  0.,
        5.65192156341060015e-02,  0., -3.49301381204462469e-01, 0.,
        3.25488720813115806e-01,  0., -1.77752619056938649e-01, 0.,
        6.89401240320876713e-02,  0., -2.05871659483433701e-02, 0.,
        4.95511697341025851e-03,  0., -9.90559795000754261e-04, 0.,
        1.68043275555716744e-04,  0., -2.45908673799202791e-05, 0.,
        3.14454083284708929e-06,  0., -3.55103745652075532e-07, 0.,
        3.57269609461536428e-08,  0., -3.22653396792257366e-09, 0.,
        2.63261700897943539e-10,  0., -1.95167968786489595e-11, 0.,
        1.32120352716508172e-12,  0., -8.22849593050680629e-14,
};

// 4. info of polynomial with hamming_weight > 192
#define R_UNIFORM \
  6  //!< number of double-angle iterations in CKKS bootstrapping.  Must be
     //!< static because it is used in a static function.

#define K_UNIFORM \
  512  //!< upper bound for the number of overflows in the uniform secret case

//!< Chebyshev series coefficients for the OPTIMIZED/uniform case
#define UNIFORM_COEFF_SIZE 89
static const double G_coefficients_uniform[UNIFORM_COEFF_SIZE] = {
    0.15421426400235561,      -0.0037671538417132409,  0.16032011744533031,
    -0.0034539657223742453,   0.17711481926851286,     -0.0027619720033372291,
    0.19949802549604084,      -0.0015928034845171929,  0.21756948616367638,
    0.00010729951647566607,   0.21600427371240055,     0.0022171399198851363,
    0.17647500259573556,      0.0042856217194480991,   0.086174491919472254,
    0.0054640252312780444,    -0.046667988130649173,   0.0047346914623733714,
    -0.17712686172280406,     0.0016205080004247200,   -0.22703114241338604,
    -0.0028145845916205865,   -0.13123089730288540,    -0.0056345646688793190,
    0.078818395388692147,     -0.0037868875028868542,  0.23226434602675575,
    0.0021116338645426574,    0.13985510526186795,     0.0059365649669377071,
    -0.13918475289368595,     0.0018580676740836374,   -0.23254376365752788,
    -0.0054103844866927788,   0.056840618403875359,    -0.0035227192748552472,
    0.25667909012207590,      0.0055029673963982112,   -0.073334392714092062,
    0.0027810273357488265,    -0.24912792167850559,    -0.0069524866497120566,
    0.21288810409948347,      0.0017810057298691725,   0.088760951809475269,
    0.0055957188940032095,    -0.31937177676259115,    -0.0087539416335935556,
    0.34748800245527145,      0.0075378299617709235,   -0.25116537379803394,
    -0.0047285674679876204,   0.13970502851683486,     0.0023672533925155220,
    -0.063649401080083698,    -0.00098993213448982727, 0.024597838934816905,
    0.00035553235917057483,   -0.0082485030307578155,  -0.00011176184313622549,
    0.0024390574829093264,    0.000031180384864488629, -0.00064373524734389861,
    -7.8036008952377965e-6,   0.00015310015145922058,  1.7670804180220134e-6,
    -0.000033066844379476900, -3.6460909134279425e-7,  6.5276969021754105e-6,
    6.8957843666189918e-8,    -1.1842811187642386e-6,  -1.2015133285307312e-8,
    1.9839339947648331e-7,    1.9372045971100854e-9,   -3.0815418032523593e-8,
    -2.9013806338735810e-10,  4.4540904298173700e-9,   4.0505136697916078e-11,
    -6.0104912807134771e-10,  -5.2873323696828491e-12, 7.5943206779351725e-11,
    6.4679566322060472e-13,   -9.0081200925539902e-12, -7.4396949275292252e-14,
    1.0057423059167244e-12,   8.1701187638005194e-15,  -1.0611736208855373e-13,
    -8.9597492970451533e-16,  1.1421575296031385e-14};

// 5. info of even polynomial with hamming_weight > 192
#define R_UNIFORM_EVEN \
  7  //!< number of double-angle iterations in CKKS bootstrapping.  Must be
     //!< static because it is used in a static function.

#define K_UNIFORM_EVEN \
  512  //!< upper bound for the number of overflows in the uniform secret case

//!< Chebyshev series coefficients for the OPTIMIZED/uniform case
#define UNIFORM_EVEN_COEFF_SIZE 55
static const double G_even_coefficients_uniform[UNIFORM_EVEN_COEFF_SIZE] = {
    // deg= 54, K= 512, R= 7
    2.20743281549609704e-01,  0., 2.38139109622304862e-01,  0.,
    2.81278392668466215e-01,  0., 3.17552873147672721e-01,  0.,
    2.83878209255650049e-01,  0., 1.11149270048825827e-01,  0.,
    -1.77388585136637050e-01, 0., -3.43148959783472818e-01, 0.,
    -7.80775341617799268e-02, 0., 3.56822187220964315e-01,  0.,
    7.01023907823122711e-02,  0., -4.33248438628725141e-01, 0.,
    4.03712918618547689e-01,  0., -2.20471629407921144e-01, 0.,
    8.55083967683751417e-02,  0., -2.55348475066272465e-02, 0.,
    6.14597252535931983e-03,  0., -1.22861948920053167e-03, 0.,
    2.08428854491001886e-04,  0., -3.05007522734078458e-05, 0.,
    3.90026343823076681e-06,  0., -4.40445276295426794e-07, 0.,
    4.43131658784083885e-08,  0., -4.00196185587582648e-09, 0.,
    3.26530975833769012e-10,  0., -2.42072383039309157e-11, 0.,
    1.63872631502701690e-12,  0., -1.02060375537654724e-13,
};

typedef enum {
  SPARSE_KIND               = 0,
  UNIFORM_HW_UNDER_192      = 1,
  UNIFORM_HW_ABOVE_192      = 2,
  UNIFORM_EVEN_HW_UNDER_192 = 3,
  UNIFORM_EVEN_HW_ABOVE_192 = 4,
  LAST_KIND                 = 5,
} EVAL_SIN_POLY_KIND;

//! Infomation of polynomial which approximates sin(ct) in CKKS bootstrapping.
typedef struct {
  EVAL_SIN_POLY_KIND _poly_kind;     // kind of polynomial
  uint32_t           _upper_bound;   // infinite norm K of I(X).
  uint32_t           _double_angle;  // number of double-angle iterations.
  uint32_t           _coeff_size;    // number of polynomial coefficients.
  const double*      _coeff;         // value of polynomial coefficients.
} EVAL_SIN_POLY_INFO;

const EVAL_SIN_POLY_INFO* Get_eval_sin_poly_info(uint32_t hamming_weight);
static inline uint32_t    Get_eval_sin_upper_bound_k(uint32_t hamming_weight) {
  return Get_eval_sin_poly_info(hamming_weight)->_upper_bound;
}

static inline uint32_t Get_eval_sin_double_angle_num(int32_t hamming_weight) {
  return Get_eval_sin_poly_info(hamming_weight)->_double_angle;
}

static inline uint32_t Get_eval_sin_poly_coeff_size(int32_t hamming_weight) {
  return Get_eval_sin_poly_info(hamming_weight)->_coeff_size;
}

static inline const double* Get_eval_sin_poly_coeff(int32_t hamming_weight) {
  return Get_eval_sin_poly_info(hamming_weight)->_coeff;
}

//! @brief Populate the conversion table Degree-to-Multiplicative Depth
enum {
  LOWER_BOUND_DEGREE = 5,
  UPPER_BOUND_DEGREE = 2031,
};

//! @brief Generate a table for multiply depth and degree
const uint32_t* Gen_depth_by_degree_table();

//! @brief Get multiply depth by degree
static inline uint32_t Get_depth_by_degree(size_t degree) {
  IS_TRUE(degree >= LOWER_BOUND_DEGREE && degree <= UPPER_BOUND_DEGREE,
          "Polynomial degree is supported from 5 to 2031 inclusive.");
  const uint32_t* depth_table = Gen_depth_by_degree_table();
  return depth_table[degree];
}

//! @brief Get the depth for a given vector of coefficients for the
//! Paterson-Stockmeyer algorithm. The functions is based on the table described
//! in openFHE: src/pke/examples/FUNCTION_EVALUATION.md
//! @param is_normalized true if the vector normalized. false is the default
//! value
static inline uint32_t Get_mul_depth_by_coeff_vector(size_t vec_len,
                                                     bool   is_normalized) {
  IS_TRUE(vec_len, "Cannot perform operation on empty vector");
  size_t   degree     = vec_len - 1;
  uint32_t mult_depth = Get_depth_by_degree(degree);
  return (is_normalized) ? (mult_depth - 1) : mult_depth;
}

//! @brief Get internal multiply depth
static inline uint32_t Get_mul_depth_internal(uint32_t hamming_weight) {
  uint32_t coeff_size       = Get_eval_sin_poly_coeff_size(hamming_weight);
  uint32_t double_angle_num = Get_eval_sin_double_angle_num(hamming_weight);
  return Get_mul_depth_by_coeff_vector(coeff_size, TRUE) + double_angle_num;
}

//! @brief Get the numbers of levels that bootstrapping procedure itself needs
//! to consume
static inline uint32_t Get_bootstrap_depth(VALUE_LIST* level_budget,
                                           uint32_t    num_iteration,
                                           uint32_t    hamming_weight) {
  IS_TRUE(num_iteration > 0, "invalid number of iterations for bootstrapping");
  uint32_t approx_depth  = Get_mul_depth_internal(hamming_weight);
  uint32_t encode_budget = UI32_VALUE_AT(level_budget, 0);
  uint32_t decode_budget = UI32_VALUE_AT(level_budget, 1);
  return approx_depth + encode_budget + decode_budget + (num_iteration - 1);
}

typedef VL_PTR    VL_PLAIN;     //<! VALUE_LIST<PLAINTEXT*>
typedef VL_PTR    VL_CIPH;      //<! VALUE_LIST<CIPHERTEXT*>
typedef VL_VL_PTR VL_VL_PLAIN;  //<! VALUE_LIST<VALUE_LIST<PLAINTEXT*>>

typedef enum {
  LEVEL_BUDGET,  //<! the level budget
  LAYERS_COLL,   //<! the number of layers to collapse in one level
  LAYERS_REM,  //<! the number of layers remaining to be collapsed in one level
               // to have exactly the number of levels specified in the level
               // budget
  NUM_ROTATIONS,      //<! the number of rotations in one level
  BABY_STEP,          //<! the baby step in the baby-step giant-step strategy
  GIANT_STEP,         //<! the giant step in the baby-step giant-step strategy
  NUM_ROTATIONS_REM,  //<! the number of rotations in the remaining level
  BABY_STEP_REM,      //<! the baby step in the baby-step giant-step strategy
                      //<! for the remaining level
  GIANT_STEP_REM,     //<! the giant step in the baby-step giant-step strategy
                      //<! for the remaining level
  TOTAL_PARAMS        //<! total number of params
} CKKS_BOOT_PARAM;

//! @brief CKKS Bootstrapping precompute for slots
typedef struct {
  uint32_t  _dim1;           //<! inner dimension in the baby-step gaint-step
  uint32_t  _slots;          //<! number of slots for bootstrapping
  VL_I32*   _encode_params;  //<! parameters for encoding, size is TOTAL_PARAMS
  VL_I32*   _decode_params;  //<! parameters for decoding, size is TOTAL_PARAMS
  VL_PLAIN* _u0_pre;         //<! Linear map U0; used in decoding
  VL_PLAIN* _u0hatt_pre;     //<! Conj(U0^T); used in encoding
  VL_VL_PLAIN* _u0_pre_fft;  //<! coeffs corresponding to U0; used in decoding
  VL_VL_PLAIN* _u0hatt_pre_fft;  //<! coeffs corresponding to conj(U0^T); used
                                 // in encoding
  bool _keygen_status;  //<! Set this to true if keys were generated, default is
                        // false.
} CKKS_BTS_PRECOM;

//! @brief Allocate CKKS_BTS_PRECOM
static inline CKKS_BTS_PRECOM* Alloc_ckks_bts_precom() {
  CKKS_BTS_PRECOM* ctx = (CKKS_BTS_PRECOM*)malloc(sizeof(CKKS_BTS_PRECOM));
  ctx->_encode_params  = NULL;
  ctx->_decode_params  = NULL;
  ctx->_u0_pre         = NULL;
  ctx->_u0hatt_pre     = NULL;
  ctx->_u0_pre_fft     = NULL;
  ctx->_u0hatt_pre_fft = NULL;
  ctx->_keygen_status  = false;
  return ctx;
}

//! @brief Cleanup CKKS_BTS_PRECOM
void Free_ckks_bts_precom(CKKS_BTS_PRECOM* ctx);

//! @brief Get inner dimension in the baby-step gaint-step
static inline uint32_t Get_inner_dim(CKKS_BTS_PRECOM* precom) {
  return precom->_dim1;
}

//! @brief Get number of slots
static inline uint32_t Get_slots_num(CKKS_BTS_PRECOM* precom) {
  return precom->_slots;
}

//! @brief Get parameters for encoding
static inline VL_I32* Get_encode_params(CKKS_BTS_PRECOM* precom) {
  return precom->_encode_params;
}

//! @brief Get parameters for decoding
static inline VL_I32* Get_decode_params(CKKS_BTS_PRECOM* precom) {
  return precom->_decode_params;
}

//! @brief Get linear map U0
static inline VL_PTR* Get_u0_pre(CKKS_BTS_PRECOM* precom) {
  return precom->_u0_pre;
}

//! @brief Get conj(U0^T)
static inline VL_PTR* Get_u0hatt_pre(CKKS_BTS_PRECOM* precom) {
  return precom->_u0hatt_pre;
}

//! @brief Get coeffs corresponding to U0
static inline VL_VL_PTR* Get_u0_pre_fft(CKKS_BTS_PRECOM* precom) {
  return precom->_u0_pre_fft;
}

//! @brief Get coeffs corresponding to conj(U0^T)
static inline VL_VL_PTR* Get_u0hatt_pre_fft(CKKS_BTS_PRECOM* precom) {
  return precom->_u0hatt_pre_fft;
}

//! @brief Get keygen status
static inline bool Get_keygen_status(CKKS_BTS_PRECOM* precom) {
  return precom->_keygen_status;
}

//! @brief Set inner dimension in the baby-step gaint-step
static inline void Set_inner_dim(CKKS_BTS_PRECOM* precom, uint32_t dim) {
  precom->_dim1 = dim;
}

//! @brief Set number of slots
static inline void Set_slots_num(CKKS_BTS_PRECOM* precom, uint32_t slots) {
  precom->_slots = slots;
}

//! @brief Set parameters for encoding
static inline void Set_encode_params(CKKS_BTS_PRECOM* precom, VL_I32* vals) {
  precom->_encode_params = vals;
}

//! @brief Set parameters for decoding
static inline void Set_decode_params(CKKS_BTS_PRECOM* precom, VL_I32* vals) {
  precom->_decode_params = vals;
}

//! @brief Set linear map U0
static inline void Set_u0_pre(CKKS_BTS_PRECOM* precom, VL_PTR* vl) {
  precom->_u0_pre = vl;
}

//! @brief Set conj(U0^T)
static inline void Set_u0hatt_pre(CKKS_BTS_PRECOM* precom, VL_PTR* vl) {
  precom->_u0hatt_pre = vl;
}

//! @brief Set coeffs corresponding to U0
static inline void Set_u0_pre_fft(CKKS_BTS_PRECOM* precom, VL_VL_PTR* vl) {
  precom->_u0_pre_fft = vl;
}

//! @brief Set coeffs corresponding to conj(U0^T)
static inline void Set_u0hatt_pre_fft(CKKS_BTS_PRECOM* precom, VL_VL_PTR* vl) {
  precom->_u0hatt_pre_fft = vl;
}

//! @brief Set keygen status
static inline void Set_keygen_status(CKKS_BTS_PRECOM* precom, bool keygen) {
  precom->_keygen_status = keygen;
}

//! @brief Print CKKS_BTS_PRECOM
void Print_ckks_bts_precom(FILE* fp, CKKS_BTS_PRECOM* precom);

//! @brief A map for bootstrap pre-computations
typedef struct {
  uint32_t         _slots;
  CKKS_BTS_PRECOM* _bts_precom;
  UT_hash_handle   HH;
} BTS_PRECOM_MAP;

//! @brief clean up BTS_PRECOM_MAP
static inline void Free_bts_precom_map(BTS_PRECOM_MAP* bts_precomp_map) {
  BTS_PRECOM_MAP* current;
  BTS_PRECOM_MAP* tmp;
  HASH_ITER(HH, bts_precomp_map, current, tmp) {
    HASH_DEL(bts_precomp_map, current);
    free(current);
  }
}

//! @brief Context for bootstrap pre-computations
typedef struct CKKS_BOOTSTRAP_CONTEXT {
  BTS_PRECOM_MAP* _precomp;
  CKKS_EVALUATOR* _eval;
} CKKS_BTS_CTX;

//! @brief CKKS_BTS_CTX allocator
static inline CKKS_BTS_CTX* Alloc_ckks_bts_ctx(CKKS_EVALUATOR* eval) {
  CKKS_BTS_CTX* ctx = (CKKS_BTS_CTX*)malloc(sizeof(CKKS_BTS_CTX));
  memset(ctx, 0, sizeof(CKKS_BTS_CTX));
  ctx->_precomp = NULL;
  ctx->_eval    = eval;
  return ctx;
}

//! @brief CKKS_BTS_CTX de-allocator
static inline void Free_ckks_bts_ctx(CKKS_BTS_CTX* ctx) {
  if (ctx == NULL) return;
  if (ctx->_precomp) {
    BTS_PRECOM_MAP *iter, *tmp;
    HASH_ITER(HH, ctx->_precomp, iter, tmp) {
      Free_ckks_bts_precom(iter->_bts_precom);
    }
    Free_bts_precom_map(ctx->_precomp);
    ctx->_precomp = NULL;
  }
  free(ctx);
}

//! @brief Add bootstrap pre-computations
static inline void Add_bts_precom(CKKS_BTS_CTX* ctx, CKKS_BTS_PRECOM* precom) {
  uint32_t        slots = Get_slots_num(precom);
  BTS_PRECOM_MAP* map;
  HASH_FIND_INT(ctx->_precomp, &slots, map);
  if (map == NULL) {
    map         = (BTS_PRECOM_MAP*)malloc(sizeof(BTS_PRECOM_MAP));
    map->_slots = slots;
    HASH_ADD_INT(ctx->_precomp, _slots, map);
  }
  map->_bts_precom = precom;
}

//! @brief Get bootstrap pre-computations
static inline BTS_PRECOM_MAP* Get_bts_precomps(CKKS_BTS_CTX* ctx) {
  return ctx->_precomp;
}

//! @brief Get key generator
static inline CKKS_KEY_GENERATOR* Get_bts_gen(CKKS_BTS_CTX* ctx) {
  return ctx->_eval->_keygen;
}

//! @brief Get encoder
static inline CKKS_ENCODER* Get_bts_encoder(CKKS_BTS_CTX* ctx) {
  return ctx->_eval->_encoder;
}

//! @brief Get ckks parameters
static inline CKKS_PARAMETER* Get_bts_params(CKKS_BTS_CTX* ctx) {
  return ctx->_eval->_params;
}

//! @brief Get evaluator
static inline CKKS_EVALUATOR* Get_bts_eval(CKKS_BTS_CTX* ctx) {
  return ctx->_eval;
}

//! @brief Get bootstrap pre-computation for input slots
static inline CKKS_BTS_PRECOM* Get_bts_precom(CKKS_BTS_CTX* ctx,
                                              uint32_t      slots) {
  BTS_PRECOM_MAP* result;
  HASH_FIND_INT(ctx->_precomp, &slots, result);
  if (result) {
    return result->_bts_precom;
  } else {
    return NULL;
  }
}

//! @brief Precompute for Coeffs2Slots
VL_VL_PLAIN* Coeffs2slots_precomp(CKKS_BTS_CTX* bts_ctx, VL_DCMPLX* ksipows,
                                  VL_UI32* rot_group, bool flag, double scale,
                                  uint32_t level);

//! @brief Precompute for Slots2coeffs
VL_VL_PLAIN* Slots2coeffs_precomp(CKKS_BTS_CTX* bts_ctx, VL_DCMPLX* ksipows,
                                  VL_UI32* rot_group, bool flag, double scale,
                                  uint32_t level);

//! @brief Precompute for linear transformations
VL_PLAIN* Linear_trans_precomp(CKKS_BTS_CTX* bts_ctx, MATRIX* matrix,
                               double scale, uint32_t level);

//! @brief Precompute for linear transformations of sparse bootstrapping
VL_PLAIN* Linear_trans_precomp_sparse(CKKS_BTS_CTX* bts_ctx, MATRIX* matrix_a,
                                      MATRIX* matrix_b, uint32_t orientation,
                                      double scale, uint32_t level);

//! @brief Bootstrap parameter setup
void Bootstrap_setup(CKKS_BTS_CTX* bts_ctx, VL_UI32* level_budget,
                     VL_UI32* dim1, uint32_t num_slots);

//! @brief Key generation for bootstrap
void Bootstrap_keygen(CKKS_BTS_CTX* bts_ctx, uint32_t slots);

//! @brief Evaluate coeffs to slots
CIPHERTEXT* Coeffs_to_slots(CIPHERTEXT* result, CIPHERTEXT* ciph,
                            VL_VL_PLAIN* conj_pre, CKKS_BTS_CTX* bts_ctx);

//! @brief Evaluate slots to coeffs
CIPHERTEXT* Slots_to_coeffs(CIPHERTEXT* result, CIPHERTEXT* ciph,
                            VL_VL_PLAIN* conj_pre, CKKS_BTS_CTX* bts_ctx);

//! @brief Evaluate linear transformations
CIPHERTEXT* Linear_transform(CIPHERTEXT* result, CIPHERTEXT* ciph,
                             VL_PLAIN* conj_pre, CKKS_BTS_CTX* bts_ctx);

//! @brief Evaluate Chebyshev
void Eval_chebyshev(CKKS_BTS_CTX* bts_ctx, CIPHERTEXT* out, CIPHERTEXT* in,
                    VL_DBL* coeffs, double a, double b);

//! @brief Eval chebyshev with paterson-stockmeyer algorithm
//! it's used for the degree of the polynomial is more than 5.
void Eval_chebyshev_ps(CKKS_BTS_CTX* bts_ctx, CIPHERTEXT* out, CIPHERTEXT* in,
                       VL_DBL* coeffs, double a, double b);

//! @brief Evaluate bootstrap
//! @param raise_level The level of raising the modulus, when raise_level is set
//! to 0, ciph will be raised to q_cnt
CIPHERTEXT* Eval_bootstrap(CIPHERTEXT* res, CIPHERTEXT* ciph,
                           uint32_t num_iterations, uint32_t precision,
                           uint32_t raise_level, CKKS_BTS_CTX* bts_ctx);

//! @brief Print CKKS_BTS_CTX
void Print_ckks_bts_ctx(FILE* fp, CKKS_BTS_CTX* bts_ctx);

//! @brief Compute how many layers are collapsed in each of the level from the
//! budget
VL_I32* Get_colls_fft_params(uint32_t slots, uint32_t level_budget,
                             uint32_t dim1);
#ifdef __cplusplus
}
#endif

#endif  // RTLIB_INCLUDE_UTIL_CKKS_BOOTSTRAP_CONTEXT_H