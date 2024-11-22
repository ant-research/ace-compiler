// external header files
#include "rt_openfhe/rt_openfhe.h"

typedef float float32_t;

extern float32_t _cst_0[27];
extern float32_t _cst_1[1];
extern int32_t _cst_2[1];
extern int32_t _cst_3[9];
extern float32_t _cst_4[675];
extern float32_t _cst_5[25];
extern int32_t _cst_6[9];

bool Main_graph() {
  CIPHERTEXT input;
  CIPHERTEXT tmp_result_n1;
  CIPHERTEXT input_dup_n1;
  int32_t index_cin_n1;
  int32_t index_khw_n1;
  CIPHERTEXT output;
  CIPHERTEXT _ckks_gen_tmp_39;
  CIPHERTEXT _preg_268435456;
  uint64_t _preg_268435457;
  uint64_t _preg_268435458;
  PLAINTEXT _preg_268435459;
  CIPHERTEXT _preg_268435460;
  uint64_t _preg_268435461;
  uint64_t _preg_268435462;
  PLAINTEXT _preg_268435463;
  input = Get_input_data("input", 0);
  Zero_ciph(&tmp_result_n1);
  Copy_ciph(&_preg_268435456, Rotate_ciph(&_preg_268435456, &input, -75));
  Add_ciph(&input_dup_n1, &input, &_preg_268435456);
  for (index_cin_n1 = 0; index_cin_n1 < 3; index_cin_n1 = index_cin_n1 + 1) {
    for (index_khw_n1 = 0; index_khw_n1 < 9; index_khw_n1 = index_khw_n1 + 1) {
      Rotate_ciph(&_ckks_gen_tmp_39, &input_dup_n1, _cst_6[index_khw_n1]);
      _preg_268435457 = Sc_degree(&_ckks_gen_tmp_39);
      _preg_268435458 = Level(&_ckks_gen_tmp_39);
      Encode_plain_from_float(&_preg_268435459, Slice(_cst_4, (index_khw_n1 + (index_cin_n1 * 9)), 25), 25, _preg_268435457, _preg_268435458);
      Copy_ciph(&_preg_268435460, Mul_plain(&_preg_268435460, &_ckks_gen_tmp_39, &_preg_268435459));
      Add_ciph(&tmp_result_n1, &tmp_result_n1, &_preg_268435460);
    }
    Rotate_ciph(&input_dup_n1, &input_dup_n1, 25);
  }
  _preg_268435461 = Sc_degree(&tmp_result_n1);
  _preg_268435462 = Level(&tmp_result_n1);
  Encode_plain_from_float(&_preg_268435463, _cst_5, 25, _preg_268435461, _preg_268435462);
  Add_plain(&tmp_result_n1, &tmp_result_n1, &_preg_268435463);
  Copy_ciph(&output, &tmp_result_n1);
  Set_output_data("output", 0, &output);
  return true;
}

int Get_input_count() {
  return 1;
}

DATA_SCHEME* Get_encode_scheme(int idx) {
  static DATA_SCHEME scheme_0 = {
    "input", {0, 0, 0, 0}, 1, {NORMAL, 0, 0, 0, 0}
  };
  static DATA_SCHEME* scheme[] = { &scheme_0 };
  return scheme[idx];
}

int Get_output_count() {
  return 1;
}

DATA_SCHEME* Get_decode_scheme(int idx) {
  static DATA_SCHEME scheme = {
    "output", {0, 0, 0, 0}, 1, {NORMAL, 0, 0, 0, 0}
  };
  return &scheme;
}

CKKS_PARAMS* Get_context_params() {
  static CKKS_PARAMS parm = {
    LIB_OPENFHE, 1024, 0, 1, 33, 30, 2, 0, 11, 
    { -75, -6, -5, -4, -1, 0, 1, 4,
      5, 6, 25 }
  };
  return &parm;
}

RT_DATA_INFO* Get_rt_data_info() {
  return NULL;
}

float32_t _cst_4[675] = {
  -0, -0, -0, -0, -0, -0, -0.144314, -0.144314,
  -0.144314, -0.144314, -0, -0.144314, -0.144314, -0.144314, -0.144314, -0,
  -0.144314, -0.144314, -0.144314, -0.144314, -0, -0.144314, -0.144314, -0.144314,
  -0.144314, 0, 0, 0, 0, 0, 0.0386629, 0.0386629,
  0.0386629, 0.0386629, 0.0386629, 0.0386629, 0.0386629, 0.0386629, 0.0386629, 0.0386629,
  0.0386629, 0.0386629, 0.0386629, 0.0386629, 0.0386629, 0.0386629, 0.0386629, 0.0386629,
  0.0386629, 0.0386629, 0, 0, 0, 0, 0, 0.0462259,
  0.0462259, 0.0462259, 0.0462259, 0, 0.0462259, 0.0462259, 0.0462259, 0.0462259,
  0, 0.0462259, 0.0462259, 0.0462259, 0.0462259, 0, 0.0462259, 0.0462259,
  0.0462259, 0.0462259, 0, -0, -0.128857, -0.128857, -0.128857, -0.128857,
  -0, -0.128857, -0.128857, -0.128857, -0.128857, -0, -0.128857, -0.128857,
  -0.128857, -0.128857, -0, -0.128857, -0.128857, -0.128857, -0.128857, -0,
  -0.128857, -0.128857, -0.128857, -0.128857, -0.0913122, -0.0913122, -0.0913122, -0.0913122,
  -0.0913122, -0.0913122, -0.0913122, -0.0913122, -0.0913122, -0.0913122, -0.0913122, -0.0913122,
  -0.0913122, -0.0913122, -0.0913122, -0.0913122, -0.0913122, -0.0913122, -0.0913122, -0.0913122,
  -0.0913122, -0.0913122, -0.0913122, -0.0913122, -0.0913122, 0.0656208, 0.0656208, 0.0656208,
  0.0656208, 0, 0.0656208, 0.0656208, 0.0656208, 0.0656208, 0, 0.0656208,
  0.0656208, 0.0656208, 0.0656208, 0, 0.0656208, 0.0656208, 0.0656208, 0.0656208,
  0, 0.0656208, 0.0656208, 0.0656208, 0.0656208, 0, 0, 0.0344722,
  0.0344722, 0.0344722, 0.0344722, 0, 0.0344722, 0.0344722, 0.0344722, 0.0344722,
  0, 0.0344722, 0.0344722, 0.0344722, 0.0344722, 0, 0.0344722, 0.0344722,
  0.0344722, 0.0344722, 0, 0, 0, 0, 0, -0.081863,
  -0.081863, -0.081863, -0.081863, -0.081863, -0.081863, -0.081863, -0.081863, -0.081863,
  -0.081863, -0.081863, -0.081863, -0.081863, -0.081863, -0.081863, -0.081863, -0.081863,
  -0.081863, -0.081863, -0.081863, -0, -0, -0, -0, -0,
  -0.0582577, -0.0582577, -0.0582577, -0.0582577, -0, -0.0582577, -0.0582577, -0.0582577,
  -0.0582577, -0, -0.0582577, -0.0582577, -0.0582577, -0.0582577, -0, -0.0582577,
  -0.0582577, -0.0582577, -0.0582577, -0, -0, -0, -0, -0,
  -0, 0, 0, 0, 0, 0, 0, 0.176262,
  0.176262, 0.176262, 0.176262, 0, 0.176262, 0.176262, 0.176262, 0.176262,
  0, 0.176262, 0.176262, 0.176262, 0.176262, 0, 0.176262, 0.176262,
  0.176262, 0.176262, -0, -0, -0, -0, -0, -0.0356074,
  -0.0356074, -0.0356074, -0.0356074, -0.0356074, -0.0356074, -0.0356074, -0.0356074, -0.0356074,
  -0.0356074, -0.0356074, -0.0356074, -0.0356074, -0.0356074, -0.0356074, -0.0356074, -0.0356074,
  -0.0356074, -0.0356074, -0.0356074, 0, 0, 0, 0, 0,
  0.108502, 0.108502, 0.108502, 0.108502, 0, 0.108502, 0.108502, 0.108502,
  0.108502, 0, 0.108502, 0.108502, 0.108502, 0.108502, 0, 0.108502,
  0.108502, 0.108502, 0.108502, 0, 0, 0.0833315, 0.0833315, 0.0833315,
  0.0833315, 0, 0.0833315, 0.0833315, 0.0833315, 0.0833315, 0, 0.0833315,
  0.0833315, 0.0833315, 0.0833315, 0, 0.0833315, 0.0833315, 0.0833315, 0.0833315,
  0, 0.0833315, 0.0833315, 0.0833315, 0.0833315, -0.124401, -0.124401, -0.124401,
  -0.124401, -0.124401, -0.124401, -0.124401, -0.124401, -0.124401, -0.124401, -0.124401,
  -0.124401, -0.124401, -0.124401, -0.124401, -0.124401, -0.124401, -0.124401, -0.124401,
  -0.124401, -0.124401, -0.124401, -0.124401, -0.124401, -0.124401, -0.163665, -0.163665,
  -0.163665, -0.163665, -0, -0.163665, -0.163665, -0.163665, -0.163665, -0,
  -0.163665, -0.163665, -0.163665, -0.163665, -0, -0.163665, -0.163665, -0.163665,
  -0.163665, -0, -0.163665, -0.163665, -0.163665, -0.163665, -0, 0,
  0.184731, 0.184731, 0.184731, 0.184731, 0, 0.184731, 0.184731, 0.184731,
  0.184731, 0, 0.184731, 0.184731, 0.184731, 0.184731, 0, 0.184731,
  0.184731, 0.184731, 0.184731, 0, 0, 0, 0, 0,
  0.0100491, 0.0100491, 0.0100491, 0.0100491, 0.0100491, 0.0100491, 0.0100491, 0.0100491,
  0.0100491, 0.0100491, 0.0100491, 0.0100491, 0.0100491, 0.0100491, 0.0100491, 0.0100491,
  0.0100491, 0.0100491, 0.0100491, 0.0100491, 0, 0, 0, 0,
  0, 0.131916, 0.131916, 0.131916, 0.131916, 0, 0.131916, 0.131916,
  0.131916, 0.131916, 0, 0.131916, 0.131916, 0.131916, 0.131916, 0,
  0.131916, 0.131916, 0.131916, 0.131916, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0.0398817, 0.0398817, 0.0398817, 0.0398817, 0, 0.0398817, 0.0398817, 0.0398817,
  0.0398817, 0, 0.0398817, 0.0398817, 0.0398817, 0.0398817, 0, 0.0398817,
  0.0398817, 0.0398817, 0.0398817, 0, 0, 0, 0, 0,
  0.0618963, 0.0618963, 0.0618963, 0.0618963, 0.0618963, 0.0618963, 0.0618963, 0.0618963,
  0.0618963, 0.0618963, 0.0618963, 0.0618963, 0.0618963, 0.0618963, 0.0618963, 0.0618963,
  0.0618963, 0.0618963, 0.0618963, 0.0618963, 0, 0, 0, 0,
  0, 0.143752, 0.143752, 0.143752, 0.143752, 0, 0.143752, 0.143752,
  0.143752, 0.143752, 0, 0.143752, 0.143752, 0.143752, 0.143752, 0,
  0.143752, 0.143752, 0.143752, 0.143752, 0, 0, 0.182484, 0.182484,
  0.182484, 0.182484, 0, 0.182484, 0.182484, 0.182484, 0.182484, 0,
  0.182484, 0.182484, 0.182484, 0.182484, 0, 0.182484, 0.182484, 0.182484,
  0.182484, 0, 0.182484, 0.182484, 0.182484, 0.182484, -0.127706, -0.127706,
  -0.127706, -0.127706, -0.127706, -0.127706, -0.127706, -0.127706, -0.127706, -0.127706,
  -0.127706, -0.127706, -0.127706, -0.127706, -0.127706, -0.127706, -0.127706, -0.127706,
  -0.127706, -0.127706, -0.127706, -0.127706, -0.127706, -0.127706, -0.127706, 0.0240618,
  0.0240618, 0.0240618, 0.0240618, 0, 0.0240618, 0.0240618, 0.0240618, 0.0240618,
  0, 0.0240618, 0.0240618, 0.0240618, 0.0240618, 0, 0.0240618, 0.0240618,
  0.0240618, 0.0240618, 0, 0.0240618, 0.0240618, 0.0240618, 0.0240618, 0,
  0, 0.143617, 0.143617, 0.143617, 0.143617, 0, 0.143617, 0.143617,
  0.143617, 0.143617, 0, 0.143617, 0.143617, 0.143617, 0.143617, 0,
  0.143617, 0.143617, 0.143617, 0.143617, 0, 0, 0, 0,
  0, 0.139418, 0.139418, 0.139418, 0.139418, 0.139418, 0.139418, 0.139418,
  0.139418, 0.139418, 0.139418, 0.139418, 0.139418, 0.139418, 0.139418, 0.139418,
  0.139418, 0.139418, 0.139418, 0.139418, 0.139418, 0, 0, 0,
  0, 0, 0.119557, 0.119557, 0.119557, 0.119557, 0, 0.119557,
  0.119557, 0.119557, 0.119557, 0, 0.119557, 0.119557, 0.119557, 0.119557,
  0, 0.119557, 0.119557, 0.119557, 0.119557, 0, 0, 0,
  0, 0, 0
};
float32_t _cst_5[25] = {
  -0.13928, -0.13928, -0.13928, -0.13928, -0.13928, -0.13928, -0.13928, -0.13928,
  -0.13928, -0.13928, -0.13928, -0.13928, -0.13928, -0.13928, -0.13928, -0.13928,
  -0.13928, -0.13928, -0.13928, -0.13928, -0.13928, -0.13928, -0.13928, -0.13928,
  -0.13928
};
int32_t _cst_6[9] = {
  -6, -5, -4, -1, 0, 1, 4, 5,
  6
};