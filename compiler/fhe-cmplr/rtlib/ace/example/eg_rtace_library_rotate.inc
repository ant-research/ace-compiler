// external header files
#include "rt_ace/rt_ace.h"

typedef float float32_t;

typedef float float32_t;

// global constants definition
float32_t _cst_0[4] = {
  1, 1, 1, 1
};


bool Main_graph() {
  CIPHERTEXT input;
  CIPHERTEXT output;
  PLAINTEXT _Constant_output_0;
  input = Get_input_data("input", 0);
  Encode_float(&_Constant_output_0, _cst_0, 4, Sc_degree(&input), Level(&input));
  Mul_plain(&output, &input, &_Constant_output_0);
  Rotate_ciph(&output, &output, 1);
  Rotate_ciph(&output, &output, -1);
  // Rotate_ciph(&output, &output, -4096);
  Set_output_data("output", 0, &output);
  return true;
}

int Get_input_count() {
  return 1;
}

DATA_SCHEME* Get_encode_scheme(int idx) {
  static MAP_DESC desc_0[] = {{NORMAL, 0, 0, 0, 0}};
  static DATA_SCHEME scheme_0 = {
    "input", {0, 0, 0, 0}, 1, desc_0
  };
  static DATA_SCHEME* scheme[] = { &scheme_0 };
  return scheme[idx];
}

int Get_output_count() {
  return 1;
}

DATA_SCHEME* Get_decode_scheme(int idx) {
  static MAP_DESC desc[] = {{NORMAL, 0, 0, 0, 0}};
  static DATA_SCHEME scheme = {
    "output", {0, 0, 0, 0}, 1, desc
  };
  return &scheme;
}

CKKS_PARAMS* Get_context_params() {
  static CKKS_PARAMS parm = {

    // LIB_ACE, 4096, 0, 8, 9, 57, 53, 0, 0,
    // LIB_ACE, 4096, 0, 10, 11, 57, 53, 0, 0,
    // 2, {-1, 1 }
    LIB_ACE, 65536, 0, 8, 9, 57, 53, 0, 0, 3, {-1, 1, -4096}
  };
  return &parm;
}

bool Need_bts() { return false; }
