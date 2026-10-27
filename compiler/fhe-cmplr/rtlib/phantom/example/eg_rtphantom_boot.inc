// external header files
#include "rt_phantom/rt_phantom.h"

typedef double float64_t;
typedef float float32_t;


bool Main_graph() {
  CIPHERTEXT input_0;
  CIPHERTEXT onnx__Add_1_1;
  CIPHERTEXT onnx__Add_1_2;
  CIPHERTEXT onnx__Add_1_3;
  CIPHERTEXT boot_1;
  CIPHERTEXT boot_2;
  CIPHERTEXT boot_3;
  CIPHERTEXT boot_4;
  CIPHERTEXT output_2;
  input_0 = Get_input_data("input", 0);
  onnx__Add_1_1 = Get_input_data("onnx::Add_1", 0);

  // /Add
  // pragma: 65536, 1025, 34
  Bootstrap(&boot_1, &input_0, 15, 8192);
  Bootstrap(&boot_2, &onnx__Add_1_1, 15, 16384);

  
  
  Add_ciph(&output_2, &boot_1, &boot_2);
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
    LIB_PHANTOM, 65536, 0, 32, 31, 60, 56, 3, 192, 0, 
    { }
  };
  return &parm;
}
RT_DATA_INFO* Get_rt_data_info() {
  return NULL;
}

bool Need_bts() { return true; }