// external header files
#include "rt_seal/rt_seal.h"

typedef float float32_t;

// global constants definition


bool Main_graph() {
  CIPHERTEXT input;
  CIPHERTEXT onnx__Add_1;
  CIPHERTEXT output;
  input = Get_input_data("input", 0);
  onnx__Add_1 = Get_input_data("onnx::Add_1", 0);
  Add_ciph(&output, &input, &onnx__Add_1);
  Set_output_data("output", 0, &output);
  return true;
}

int Get_input_count() {
  return 2;
}

DATA_SCHEME* Get_encode_scheme(int idx) {
  static DATA_SCHEME scheme_0 = {
    "input", {0, 0, 0, 0}, 1, {NORMAL, 0, 0, 0, 0}
  };
  static DATA_SCHEME scheme_1 = {
    "onnx::Add_1", {0, 0, 0, 0}, 1, {NORMAL, 0, 0, 0, 0}
  };
  static DATA_SCHEME* scheme[] = { &scheme_0, &scheme_1 };
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
    LIB_SEAL, 128, 0, 0, 33, 30, 0, 0, 
    {  }
  };
  return &parm;
}

