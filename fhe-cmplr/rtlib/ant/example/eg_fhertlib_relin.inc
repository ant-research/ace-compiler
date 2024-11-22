// external header files
#include "rt_ant/rt_ant.h"

typedef float float32_t;

// global constants definition

bool Main_graph() {
  CIPHERTEXT  input;
  CIPHERTEXT  output;
  CIPHERTEXT3 _pgen_tmp_0;
  MODULUS*    _pgen_modulus;
  uint32_t    _pgen_num_q;
  uint32_t    _pgen_rns_idx;
  POLY        _pgen_tmp_1;
  POLY        _pgen_tmp_2;
  POLY        _pgen_swk_c0;
  POLY        _pgen_swk_c1;
  POLY        _pgen_ext;
  POLY        _pgen_tmp_poly;
  POLY        _pgen_mod_down_c0;
  POLY        _pgen_mod_down_c1;
  POLY        _pgen_decomp;
  SWITCH_KEY* _pgen_swk;
  uint32_t    _pgen_part_idx;
  POLY        _pgen_key0;
  POLY        _pgen_key1;
  uint32_t    _pgen_num_p;
  uint32_t    _pgen_p_ofst;
  uint32_t    _pgen_p_idx;
  uint32_t    degree = Degree();
  input              = Get_input_data("input", 0);
  memset(&output, 0, sizeof(output));
  memset(&_pgen_tmp_0, 0, sizeof(_pgen_tmp_0));
  Init_ciph3_up_scale(&_pgen_tmp_0, &input, &input);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q   = Poly_level(&_pgen_tmp_0._c0_poly);
    _pgen_tmp_1   = Alloc_poly(degree, 1, 0);
    _pgen_tmp_2   = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q;
         _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_1, 0, degree),
                Coeffs(&input._c0_poly, _pgen_rns_idx, degree),
                Coeffs(&input._c1_poly, _pgen_rns_idx, degree), _pgen_modulus,
                degree);
      Hw_modmul(Coeffs(_pgen_tmp_2, 0, degree),
                Coeffs(&input._c1_poly, _pgen_rns_idx, degree),
                Coeffs(&input._c0_poly, _pgen_rns_idx, degree), _pgen_modulus,
                degree);
      Hw_modmul(Coeffs(&_pgen_tmp_0._c0_poly, _pgen_rns_idx, degree),
                Coeffs(&input._c0_poly, _pgen_rns_idx, degree),
                Coeffs(&input._c0_poly, _pgen_rns_idx, degree), _pgen_modulus,
                degree);
      Hw_modadd(Coeffs(&_pgen_tmp_0._c1_poly, _pgen_rns_idx, degree),
                Coeffs(_pgen_tmp_1, 0, degree), Coeffs(_pgen_tmp_2, 0, degree),
                _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_0._c2_poly, _pgen_rns_idx, degree),
                Coeffs(&input._c1_poly, _pgen_rns_idx, degree),
                Coeffs(&input._c1_poly, _pgen_rns_idx, degree), _pgen_modulus,
                degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_same_scale_ciph3(&output, &_pgen_tmp_0);
  {
    _pgen_swk_c0   = Alloc_poly(degree, Poly_level(&_pgen_tmp_0._c2_poly), 1);
    _pgen_swk_c1   = Alloc_poly(degree, Poly_level(&_pgen_tmp_0._c2_poly), 1);
    _pgen_ext      = Alloc_poly(degree, Poly_level(&_pgen_tmp_0._c2_poly), 1);
    _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 =
        Alloc_poly(degree, Poly_level(&_pgen_tmp_0._c0_poly), 0);
    _pgen_mod_down_c1 =
        Alloc_poly(degree, Poly_level(&_pgen_tmp_0._c2_poly), 0);
    _pgen_decomp = Alloc_poly(degree, Poly_level(&_pgen_tmp_0._c0_poly), 0);
    _pgen_swk    = Swk(0, 0);
    for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&_pgen_tmp_0._c2_poly);
         _pgen_part_idx = _pgen_part_idx + 1) {
      Decomp(_pgen_decomp, &_pgen_tmp_0._c2_poly, _pgen_part_idx);
      Mod_up(_pgen_ext, _pgen_decomp, _pgen_part_idx);
      _pgen_key0 = Pk0_at(_pgen_swk, _pgen_part_idx);
      _pgen_key1 = Pk1_at(_pgen_swk, _pgen_part_idx);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q   = Poly_level(_pgen_ext);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q;
             _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(_pgen_tmp_poly, 0, degree),
                    Coeffs(_pgen_key0, _pgen_rns_idx, degree),
                    Coeffs(_pgen_ext, _pgen_rns_idx, degree), _pgen_modulus,
                    degree);
          Hw_modadd(Coeffs(_pgen_swk_c0, _pgen_rns_idx, degree),
                    Coeffs(_pgen_swk_c0, _pgen_rns_idx, degree),
                    Coeffs(_pgen_tmp_poly, 0, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(_pgen_tmp_poly, 0, degree),
                    Coeffs(_pgen_key1, _pgen_rns_idx, degree),
                    Coeffs(_pgen_ext, _pgen_rns_idx, degree), _pgen_modulus,
                    degree);
          Hw_modadd(Coeffs(_pgen_swk_c1, _pgen_rns_idx, degree),
                    Coeffs(_pgen_swk_c1, _pgen_rns_idx, degree),
                    Coeffs(_pgen_tmp_poly, 0, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
      {
        _pgen_modulus = P_modulus();
        _pgen_num_p   = Num_p(_pgen_ext);
        _pgen_p_ofst  = Num_alloc(_pgen_ext) - _pgen_num_p;
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_p;
             _pgen_rns_idx = _pgen_rns_idx + 1) {
          _pgen_p_idx = _pgen_rns_idx + _pgen_p_ofst;
          Hw_modmul(Coeffs(_pgen_tmp_poly, 0, degree),
                    Coeffs(_pgen_key0, _pgen_p_idx, degree),
                    Coeffs(_pgen_ext, _pgen_p_idx, degree), _pgen_modulus,
                    degree);
          Hw_modadd(Coeffs(_pgen_swk_c0, _pgen_p_idx, degree),
                    Coeffs(_pgen_swk_c0, _pgen_p_idx, degree),
                    Coeffs(_pgen_tmp_poly, 0, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(_pgen_tmp_poly, 0, degree),
                    Coeffs(_pgen_key1, _pgen_p_idx, degree),
                    Coeffs(_pgen_ext, _pgen_p_idx, degree), _pgen_modulus,
                    degree);
          Hw_modadd(Coeffs(_pgen_swk_c1, _pgen_p_idx, degree),
                    Coeffs(_pgen_swk_c1, _pgen_p_idx, degree),
                    Coeffs(_pgen_tmp_poly, 0, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    Mod_down(_pgen_mod_down_c0, _pgen_swk_c0);
    Mod_down(_pgen_mod_down_c1, _pgen_swk_c1);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q   = Poly_level(&_pgen_tmp_0._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q;
           _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&output._c0_poly, _pgen_rns_idx, degree),
                  Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree),
                  Coeffs(&_pgen_tmp_0._c0_poly, _pgen_rns_idx, degree),
                  _pgen_modulus, degree);
        Hw_modadd(Coeffs(&output._c1_poly, _pgen_rns_idx, degree),
                  Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree),
                  Coeffs(&_pgen_tmp_0._c1_poly, _pgen_rns_idx, degree),
                  _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly(_pgen_swk_c0);
    Free_poly(_pgen_swk_c1);
    Free_poly(_pgen_ext);
    Free_poly(_pgen_tmp_poly);
    Free_poly(_pgen_decomp);
  }
  Set_output_data("output", 0, &output);
  return true;
}
CKKS_PARAMS* Get_context_params() {
  static CKKS_PARAMS parm = {LIB_ANT, 16, 0, 1, 33, 30, 0, 0, 0, {}};
  return &parm;
}

DATA_SCHEME* Get_encode_scheme(int idx) {
  static DATA_SCHEME scheme_0 = {
    "input", {0, 0, 0, 0}, 1, {NORMAL, 0, 0, 0, 0}
  };
  static DATA_SCHEME* scheme[] = { &scheme_0 };
  return scheme[idx];
}

DATA_SCHEME* Get_decode_scheme(int idx) {
  static DATA_SCHEME scheme = {
    "output", {0, 0, 0, 0}, 1, {NORMAL, 0, 0, 0, 0}
  };
  return &scheme;
}

RT_DATA_INFO* Get_rt_data_info() {
  return NULL;
}

int Get_output_count() {
  return 1;
}

int Get_input_count() {
  return 1;
}