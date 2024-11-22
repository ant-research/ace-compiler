// external header files
#include "rt_seal/rt_seal.h"

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
  Encode_plain_from_float(&_Constant_output_0, _cst_0, 4, 1, 0);
  Add_plain(&output, &input, &_Constant_output_0);
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
    LIB_SEAL, 32, 0, 0, 33, 30, 0, 0, 
    {  }
  };
  return &parm;
}

