// external header files
#include "rt_ant/rt_ant.h"

typedef double float64_t;
typedef float float32_t;
typedef size_t LEVEL_T;
typedef double SCALE_T;


extern "C" bool Main_graph() {
  CIPHERTEXT x_0;
  CIPHERTEXT y_1;
  CIPHERTEXT output_2;
  MODULUS* _pgen_modulus_3;
  uint32_t _pgen_num_q_4;
  uint32_t _pgen_rns_idx_5;
  uint32_t  degree = Degree();
  x_0 = Get_input_data("x", 0);
  y_1 = Get_input_data("y", 0);
  memset(&output_2, 0, sizeof(output_2));
  // /Add
  // pragma: 65536, 1025, 34
  Init_ciph_same_scale(&output_2, &x_0, &y_1);
  _pgen_modulus_3 = Q_modulus();
  _pgen_num_q_4 = Poly_level(&output_2._c0_poly);
  for (_pgen_rns_idx_5 = 0; _pgen_rns_idx_5 < _pgen_num_q_4; _pgen_rns_idx_5 = _pgen_rns_idx_5 + 1) {
    Hw_modadd(Coeffs(&output_2._c0_poly, _pgen_rns_idx_5, degree), Coeffs(&x_0._c0_poly, _pgen_rns_idx_5, degree), Coeffs(&y_1._c0_poly, _pgen_rns_idx_5, degree), _pgen_modulus_3, degree);
    Hw_modadd(Coeffs(&output_2._c1_poly, _pgen_rns_idx_5, degree), Coeffs(&x_0._c1_poly, _pgen_rns_idx_5, degree), Coeffs(&y_1._c1_poly, _pgen_rns_idx_5, degree), _pgen_modulus_3, degree);
    _pgen_modulus_3 = _pgen_modulus_3 + 1;
  }
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
    "x", {0, 0, 0, 0}, 1, desc_0
  };
  static MAP_DESC desc_1[] = {
    {NORMAL, 0, 0, 0, 0}
  };
  static DATA_SCHEME scheme_1 = {
    "y", {0, 0, 0, 0}, 1, desc_1
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
    LIB_ANT, 32, 0, 0, 1, 60, 56, 1, 192, 0, 
    {  }
  };
  return &parm;
}

// In offline encoding scenery, data file type will be changed. Below impl need to be changed also? 
RT_DATA_INFO* Get_rt_data_info() {
  return NULL;
}

