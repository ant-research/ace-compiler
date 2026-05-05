// external header files
#include "rt_ant/rt_ant.h"

typedef double float64_t;
typedef float float32_t;

#ifdef __cplusplus
extern "C" {
#endif

#if 0

bool Main_graph2() { 
  CIPHERTEXT x_0;
  CIPHERTEXT y_1;
  CIPHERTEXT add_1_2;
  MODULUS* _pgen_modulus_3;
  uint32_t _pgen_num_q_4;
  uint32_t _pgen_rns_idx_5;
  uint32_t  degree = Degree();
  x_0 = Get_input_data("x", 0);
  y_1 = Get_input_data("y", 0);
  memset(&add_1_2, 0, sizeof(add_1_2));
  Init_ciph_same_scale(&add_1_2, &x_0, &y_1);
  _pgen_modulus_3 = Q_modulus();
  _pgen_num_q_4 = Poly_level(&add_1_2._c0_poly);
  for (_pgen_rns_idx_5 = 0; _pgen_rns_idx_5 < _pgen_num_q_4; _pgen_rns_idx_5 = _pgen_rns_idx_5 + 1) {
    Hw_modadd(Coeffs(&add_1_2._c0_poly, _pgen_rns_idx_5, degree), Coeffs(&x_0._c0_poly, _pgen_rns_idx_5, degree), Coeffs(&y_1._c0_poly, _pgen_rns_idx_5, degree), _pgen_modulus_3, degree);
    Hw_modadd(Coeffs(&add_1_2._c1_poly, _pgen_rns_idx_5, degree), Coeffs(&x_0._c1_poly, _pgen_rns_idx_5, degree), Coeffs(&y_1._c1_poly, _pgen_rns_idx_5, degree), _pgen_modulus_3, degree);
    _pgen_modulus_3 = _pgen_modulus_3 + 1;
  }
  Set_output_data("add_1", 0, &add_1_2);
  return true;
}

#else
// CIPHERTEXT Test_get_input(const char* name, size_t idx) {
//   printf("Received name: '%s', idx: %zu\n", name, idx);
//   return Get_input_data(name, 0);
// }

bool Main_graph2(CIPHERTEXT x_0, CIPHERTEXT y_1) { 
  // CIPHERTEXT x_0;
  // CIPHERTEXT y_1;
  CIPHERTEXT add_1_2;
  MODULUS* _pgen_modulus_3;
  uint32_t _pgen_num_q_4;
  uint32_t _pgen_rns_idx_5;
  uint32_t  degree = Degree();
  // x_0 = Get_input_data("x", 0);
  // y_1 = Get_input_data("y", 0);
  memset(&add_1_2, 0, sizeof(add_1_2));
  Init_ciph_same_scale(&add_1_2, &x_0, &y_1);
  _pgen_modulus_3 = Q_modulus();
  _pgen_num_q_4 = Poly_level(&add_1_2._c0_poly);
  for (_pgen_rns_idx_5 = 0; _pgen_rns_idx_5 < _pgen_num_q_4; _pgen_rns_idx_5 = _pgen_rns_idx_5 + 1) {
    Hw_modadd(Coeffs(&add_1_2._c0_poly, _pgen_rns_idx_5, degree), Coeffs(&x_0._c0_poly, _pgen_rns_idx_5, degree), Coeffs(&y_1._c0_poly, _pgen_rns_idx_5, degree), _pgen_modulus_3, degree);
    Hw_modadd(Coeffs(&add_1_2._c1_poly, _pgen_rns_idx_5, degree), Coeffs(&x_0._c1_poly, _pgen_rns_idx_5, degree), Coeffs(&y_1._c1_poly, _pgen_rns_idx_5, degree), _pgen_modulus_3, degree);
    _pgen_modulus_3 = _pgen_modulus_3 + 1;
  }
  Set_output_data("add_1", 0, &add_1_2);
  return true;
}
#endif

CIPHERTEXT add(CIPHERTEXT p0_0, CIPHERTEXT p1_1) {
  CIPHERTEXT _fhe_tmp_0_2;
  MODULUS* _pgen_modulus_3;
  uint32_t _pgen_num_q_4;
  uint32_t _pgen_rns_idx_5;
  uint32_t  degree = Degree();
  memset(&_fhe_tmp_0_2, 0, sizeof(_fhe_tmp_0_2));
  Init_ciph_same_scale(&_fhe_tmp_0_2, &p0_0, &p1_1);
  _pgen_modulus_3 = Q_modulus();
  _pgen_num_q_4 = Poly_level(&_fhe_tmp_0_2._c0_poly);
  for (_pgen_rns_idx_5 = 0; _pgen_rns_idx_5 < _pgen_num_q_4; _pgen_rns_idx_5 = _pgen_rns_idx_5 + 1) {
    Hw_modadd(Coeffs(&_fhe_tmp_0_2._c0_poly, _pgen_rns_idx_5, degree), Coeffs(&p0_0._c0_poly, _pgen_rns_idx_5, degree), Coeffs(&p1_1._c0_poly, _pgen_rns_idx_5, degree), _pgen_modulus_3, degree);
    Hw_modadd(Coeffs(&_fhe_tmp_0_2._c1_poly, _pgen_rns_idx_5, degree), Coeffs(&p0_0._c1_poly, _pgen_rns_idx_5, degree), Coeffs(&p1_1._c1_poly, _pgen_rns_idx_5, degree), _pgen_modulus_3, degree);
    _pgen_modulus_3 = _pgen_modulus_3 + 1;
  }
  return _fhe_tmp_0_2;
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
