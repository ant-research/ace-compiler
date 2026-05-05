// external header files
#include "rt_phantom/rt_phantom.h"

typedef double float64_t;
typedef float float32_t;

#ifdef __cplusplus
extern "C" {
#endif

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

bool jit_test() {
  CIPHERTEXT input_0;
  CIPHERTEXT onnx__Add_1_1;
  CIPHERTEXT output_2;

  input_0 = Get_input_data("input0", 0);
  onnx__Add_1_1 = Get_input_data("input1", 0);
  memset(&output_2, 0, sizeof(output_2));

 output_2 = add(input_0, onnx__Add_1_1);

  Set_output_data("output", 0, &output_2);
  return true;
}
#if 0
CKKS_PARAMS* Get_context_params() {
  static CKKS_PARAMS parm = {
    LIB_ANT, 16384, 128, 9, 0, 60, 40, 3, 192, 0, 
    {  }
  };
  return &parm;
}

// In offline encoding scenery, data file type will be changed. Below impl need to be changed also? 
RT_DATA_INFO* Get_rt_data_info() {
  return NULL;
}
#endif

#ifdef __cplusplus
}
#endif
