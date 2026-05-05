#include "rt_phantom/rt_phantom.h"

typedef double float64_t;
typedef float float32_t;

#ifdef __cplusplus
extern "C" {
#endif

CIPHERTEXT add(CIPHERTEXT p0_0, CIPHERTEXT p1_1);

CIPHERTEXT add(CIPHERTEXT p0_0, CIPHERTEXT p1_1) {
  CIPHERTEXT output_2;
  // input_0 = Get_input_data("input", 0);
  // onnx__Add_1_1 = Get_input_data("onnx::Add_1", 0);
  // /Add
  // pragma: 65536, 1025, 34
  Add_ciph(&output_2, &p0_0, &p1_1);
  // pragma: 65537, 1025, 34
  // Set_output_data("output", 0, &output_2);
  return output_2;
}

bool Main_graph() {
  CIPHERTEXT input_0;
  CIPHERTEXT onnx__Add_1_1;
  CIPHERTEXT output_2;
  input_0 = Get_input_data("input", 0);
  onnx__Add_1_1 = Get_input_data("onnx::Add_1", 0);
  // /Add
  // pragma: 65536, 1025, 34
  // Add_ciph(&output_2, &input_0, &onnx__Add_1_1);
  output_2 = add(input_0, onnx__Add_1_1);
  // pragma: 65537, 1025, 34
  Set_output_data("output", 0, &output_2);
  return true;
}

int Get_input_count() {
  return 2;
}

DATA_SCHEME* Get_encode_scheme(int idx) {
  static MAP_DESC desc_0[] = {
    {NORMAL, 0, 0, 0, 0}
  };
  static DATA_SCHEME scheme_0 = {
    "input", {0, 0, 0, 0}, 1, desc_0
  };
  static MAP_DESC desc_1[] = {
    {NORMAL, 0, 0, 0, 0}
  };
  static DATA_SCHEME scheme_1 = {
    "onnx::Add_1", {0, 0, 0, 0}, 1, desc_1
  };
  static DATA_SCHEME* scheme[] = { &scheme_0, &scheme_1 };
  return scheme[idx];
}

int Get_output_count() {
  return 1;
}

DATA_SCHEME* Get_decode_scheme(int idx) {
  static MAP_DESC desc_0[] = {
    {NORMAL, 0, 0, 0, 0}
  };
  static DATA_SCHEME scheme = {
    "output", {0, 0, 0, 0}, 1, desc_0
  };
  return &scheme;
}

CKKS_PARAMS* Get_context_params() {
  static CKKS_PARAMS parm = {
    LIB_PHANTOM, 65536, 0, 0, 1, 60, 56, 1, 192, 0, 
    {  }
  };
  return &parm;
}

RT_DATA_INFO* Get_rt_data_info() {
  return NULL;
}

// bool Need_bts() { return false; }

#ifdef __cplusplus
}
#endif

bool Need_bts() { return false; }
