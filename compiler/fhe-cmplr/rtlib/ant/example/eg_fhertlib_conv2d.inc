// conv2d_keep_shape.onnx
// external header files
#include "rt_ant/rt_ant.h"

typedef float float32_t;

// global constants definition
float32_t _cst_0[27] = {
    -0.144314, 0.0386629, 0.0462259,  -0.128857, -0.0913122, 0.0656208,
    0.0344722, -0.081863, -0.0582577, 0.176262,  -0.0356074, 0.108502,
    0.0833315, -0.124401, -0.163665,  0.184731,  0.0100491,  0.131916,
    0.0398817, 0.0618963, 0.143752,   0.182484,  -0.127706,  0.0240618,
    0.143617,  0.139418,  0.119557};
float32_t _cst_1[1]   = {-0.13928};
int32_t   _cst_2[1]   = {75};
int32_t   _cst_3[9]   = {-6, -5, -4, -1, 0, 1, 4, 5, 6};
float32_t _cst_4[675] = {
    -0,         -0,         -0,         -0,         -0,         -0,
    -0.144314,  -0.144314,  -0.144314,  -0.144314,  -0,         -0.144314,
    -0.144314,  -0.144314,  -0.144314,  -0,         -0.144314,  -0.144314,
    -0.144314,  -0.144314,  -0,         -0.144314,  -0.144314,  -0.144314,
    -0.144314,  0,          0,          0,          0,          0,
    0.0386629,  0.0386629,  0.0386629,  0.0386629,  0.0386629,  0.0386629,
    0.0386629,  0.0386629,  0.0386629,  0.0386629,  0.0386629,  0.0386629,
    0.0386629,  0.0386629,  0.0386629,  0.0386629,  0.0386629,  0.0386629,
    0.0386629,  0.0386629,  0,          0,          0,          0,
    0,          0.0462259,  0.0462259,  0.0462259,  0.0462259,  0,
    0.0462259,  0.0462259,  0.0462259,  0.0462259,  0,          0.0462259,
    0.0462259,  0.0462259,  0.0462259,  0,          0.0462259,  0.0462259,
    0.0462259,  0.0462259,  0,          -0,         -0.128857,  -0.128857,
    -0.128857,  -0.128857,  -0,         -0.128857,  -0.128857,  -0.128857,
    -0.128857,  -0,         -0.128857,  -0.128857,  -0.128857,  -0.128857,
    -0,         -0.128857,  -0.128857,  -0.128857,  -0.128857,  -0,
    -0.128857,  -0.128857,  -0.128857,  -0.128857,  -0.0913122, -0.0913122,
    -0.0913122, -0.0913122, -0.0913122, -0.0913122, -0.0913122, -0.0913122,
    -0.0913122, -0.0913122, -0.0913122, -0.0913122, -0.0913122, -0.0913122,
    -0.0913122, -0.0913122, -0.0913122, -0.0913122, -0.0913122, -0.0913122,
    -0.0913122, -0.0913122, -0.0913122, -0.0913122, -0.0913122, 0.0656208,
    0.0656208,  0.0656208,  0.0656208,  0,          0.0656208,  0.0656208,
    0.0656208,  0.0656208,  0,          0.0656208,  0.0656208,  0.0656208,
    0.0656208,  0,          0.0656208,  0.0656208,  0.0656208,  0.0656208,
    0,          0.0656208,  0.0656208,  0.0656208,  0.0656208,  0,
    0,          0.0344722,  0.0344722,  0.0344722,  0.0344722,  0,
    0.0344722,  0.0344722,  0.0344722,  0.0344722,  0,          0.0344722,
    0.0344722,  0.0344722,  0.0344722,  0,          0.0344722,  0.0344722,
    0.0344722,  0.0344722,  0,          0,          0,          0,
    0,          -0.081863,  -0.081863,  -0.081863,  -0.081863,  -0.081863,
    -0.081863,  -0.081863,  -0.081863,  -0.081863,  -0.081863,  -0.081863,
    -0.081863,  -0.081863,  -0.081863,  -0.081863,  -0.081863,  -0.081863,
    -0.081863,  -0.081863,  -0.081863,  -0,         -0,         -0,
    -0,         -0,         -0.0582577, -0.0582577, -0.0582577, -0.0582577,
    -0,         -0.0582577, -0.0582577, -0.0582577, -0.0582577, -0,
    -0.0582577, -0.0582577, -0.0582577, -0.0582577, -0,         -0.0582577,
    -0.0582577, -0.0582577, -0.0582577, -0,         -0,         -0,
    -0,         -0,         -0,         0,          0,          0,
    0,          0,          0,          0.176262,   0.176262,   0.176262,
    0.176262,   0,          0.176262,   0.176262,   0.176262,   0.176262,
    0,          0.176262,   0.176262,   0.176262,   0.176262,   0,
    0.176262,   0.176262,   0.176262,   0.176262,   -0,         -0,
    -0,         -0,         -0,         -0.0356074, -0.0356074, -0.0356074,
    -0.0356074, -0.0356074, -0.0356074, -0.0356074, -0.0356074, -0.0356074,
    -0.0356074, -0.0356074, -0.0356074, -0.0356074, -0.0356074, -0.0356074,
    -0.0356074, -0.0356074, -0.0356074, -0.0356074, -0.0356074, 0,
    0,          0,          0,          0,          0.108502,   0.108502,
    0.108502,   0.108502,   0,          0.108502,   0.108502,   0.108502,
    0.108502,   0,          0.108502,   0.108502,   0.108502,   0.108502,
    0,          0.108502,   0.108502,   0.108502,   0.108502,   0,
    0,          0.0833315,  0.0833315,  0.0833315,  0.0833315,  0,
    0.0833315,  0.0833315,  0.0833315,  0.0833315,  0,          0.0833315,
    0.0833315,  0.0833315,  0.0833315,  0,          0.0833315,  0.0833315,
    0.0833315,  0.0833315,  0,          0.0833315,  0.0833315,  0.0833315,
    0.0833315,  -0.124401,  -0.124401,  -0.124401,  -0.124401,  -0.124401,
    -0.124401,  -0.124401,  -0.124401,  -0.124401,  -0.124401,  -0.124401,
    -0.124401,  -0.124401,  -0.124401,  -0.124401,  -0.124401,  -0.124401,
    -0.124401,  -0.124401,  -0.124401,  -0.124401,  -0.124401,  -0.124401,
    -0.124401,  -0.124401,  -0.163665,  -0.163665,  -0.163665,  -0.163665,
    -0,         -0.163665,  -0.163665,  -0.163665,  -0.163665,  -0,
    -0.163665,  -0.163665,  -0.163665,  -0.163665,  -0,         -0.163665,
    -0.163665,  -0.163665,  -0.163665,  -0,         -0.163665,  -0.163665,
    -0.163665,  -0.163665,  -0,         0,          0.184731,   0.184731,
    0.184731,   0.184731,   0,          0.184731,   0.184731,   0.184731,
    0.184731,   0,          0.184731,   0.184731,   0.184731,   0.184731,
    0,          0.184731,   0.184731,   0.184731,   0.184731,   0,
    0,          0,          0,          0,          0.0100491,  0.0100491,
    0.0100491,  0.0100491,  0.0100491,  0.0100491,  0.0100491,  0.0100491,
    0.0100491,  0.0100491,  0.0100491,  0.0100491,  0.0100491,  0.0100491,
    0.0100491,  0.0100491,  0.0100491,  0.0100491,  0.0100491,  0.0100491,
    0,          0,          0,          0,          0,          0.131916,
    0.131916,   0.131916,   0.131916,   0,          0.131916,   0.131916,
    0.131916,   0.131916,   0,          0.131916,   0.131916,   0.131916,
    0.131916,   0,          0.131916,   0.131916,   0.131916,   0.131916,
    0,          0,          0,          0,          0,          0,
    0,          0,          0,          0,          0,          0,
    0.0398817,  0.0398817,  0.0398817,  0.0398817,  0,          0.0398817,
    0.0398817,  0.0398817,  0.0398817,  0,          0.0398817,  0.0398817,
    0.0398817,  0.0398817,  0,          0.0398817,  0.0398817,  0.0398817,
    0.0398817,  0,          0,          0,          0,          0,
    0.0618963,  0.0618963,  0.0618963,  0.0618963,  0.0618963,  0.0618963,
    0.0618963,  0.0618963,  0.0618963,  0.0618963,  0.0618963,  0.0618963,
    0.0618963,  0.0618963,  0.0618963,  0.0618963,  0.0618963,  0.0618963,
    0.0618963,  0.0618963,  0,          0,          0,          0,
    0,          0.143752,   0.143752,   0.143752,   0.143752,   0,
    0.143752,   0.143752,   0.143752,   0.143752,   0,          0.143752,
    0.143752,   0.143752,   0.143752,   0,          0.143752,   0.143752,
    0.143752,   0.143752,   0,          0,          0.182484,   0.182484,
    0.182484,   0.182484,   0,          0.182484,   0.182484,   0.182484,
    0.182484,   0,          0.182484,   0.182484,   0.182484,   0.182484,
    0,          0.182484,   0.182484,   0.182484,   0.182484,   0,
    0.182484,   0.182484,   0.182484,   0.182484,   -0.127706,  -0.127706,
    -0.127706,  -0.127706,  -0.127706,  -0.127706,  -0.127706,  -0.127706,
    -0.127706,  -0.127706,  -0.127706,  -0.127706,  -0.127706,  -0.127706,
    -0.127706,  -0.127706,  -0.127706,  -0.127706,  -0.127706,  -0.127706,
    -0.127706,  -0.127706,  -0.127706,  -0.127706,  -0.127706,  0.0240618,
    0.0240618,  0.0240618,  0.0240618,  0,          0.0240618,  0.0240618,
    0.0240618,  0.0240618,  0,          0.0240618,  0.0240618,  0.0240618,
    0.0240618,  0,          0.0240618,  0.0240618,  0.0240618,  0.0240618,
    0,          0.0240618,  0.0240618,  0.0240618,  0.0240618,  0,
    0,          0.143617,   0.143617,   0.143617,   0.143617,   0,
    0.143617,   0.143617,   0.143617,   0.143617,   0,          0.143617,
    0.143617,   0.143617,   0.143617,   0,          0.143617,   0.143617,
    0.143617,   0.143617,   0,          0,          0,          0,
    0,          0.139418,   0.139418,   0.139418,   0.139418,   0.139418,
    0.139418,   0.139418,   0.139418,   0.139418,   0.139418,   0.139418,
    0.139418,   0.139418,   0.139418,   0.139418,   0.139418,   0.139418,
    0.139418,   0.139418,   0.139418,   0,          0,          0,
    0,          0,          0.119557,   0.119557,   0.119557,   0.119557,
    0,          0.119557,   0.119557,   0.119557,   0.119557,   0,
    0.119557,   0.119557,   0.119557,   0.119557,   0,          0.119557,
    0.119557,   0.119557,   0.119557,   0,          0,          0,
    0,          0,          0};
float32_t _cst_5[25] = {-0.13928, -0.13928, -0.13928, -0.13928, -0.13928,
                        -0.13928, -0.13928, -0.13928, -0.13928, -0.13928,
                        -0.13928, -0.13928, -0.13928, -0.13928, -0.13928,
                        -0.13928, -0.13928, -0.13928, -0.13928, -0.13928,
                        -0.13928, -0.13928, -0.13928, -0.13928, -0.13928};
int32_t   _cst_6[9]  = {-6, -5, -4, -1, 0, 1, 4, 5, 6};

bool Main_graph() {
  CIPHERTEXT  input;
  CIPHERTEXT  output;
  CIPHERTEXT  tmp_result_n1;
  CIPHERTEXT  input_dup_n1;
  int32_t     index_cin_n1;
  int32_t     index_khw_n1;
  CIPHERTEXT  _ckks_gen_tmp_41;
  MODULUS*    _pgen_modulus;
  uint32_t    _pgen_num_q;
  uint32_t    _pgen_rns_idx;
  CIPHERTEXT  _pgen_tmp_0;
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
  int64_t*    _pgen_order;
  CIPHERTEXT  _pgen_tmp_1;
  CIPHERTEXT  _pgen_tmp_2;
  PLAINTEXT   _pgen_tmp_3;
  CIPHERTEXT  _pgen_tmp_4;
  PLAINTEXT   _pgen_tmp_5;
  uint32_t    degree = Degree();
  input              = Get_input_data("input", 0);
  memset(&output, 0, sizeof(output));
  memset(&tmp_result_n1, 0, sizeof(tmp_result_n1));
  memset(&input_dup_n1, 0, sizeof(input_dup_n1));
  memset(&_ckks_gen_tmp_41, 0, sizeof(_ckks_gen_tmp_41));
  memset(&_pgen_tmp_0, 0, sizeof(_pgen_tmp_0));
  memset(&_pgen_tmp_1, 0, sizeof(_pgen_tmp_1));
  memset(&_pgen_tmp_2, 0, sizeof(_pgen_tmp_2));
  memset(&_pgen_tmp_3, 0, sizeof(_pgen_tmp_3));
  memset(&_pgen_tmp_4, 0, sizeof(_pgen_tmp_4));
  memset(&_pgen_tmp_5, 0, sizeof(_pgen_tmp_5));
  ;
  Init_ciph_same_scale(&_pgen_tmp_0, &input, 0);
  {
    _pgen_swk_c0      = Alloc_poly(degree, Poly_level(&input._c1_poly), 1);
    _pgen_swk_c1      = Alloc_poly(degree, Poly_level(&input._c1_poly), 1);
    _pgen_ext         = Alloc_poly(degree, Poly_level(&input._c1_poly), 1);
    _pgen_tmp_poly    = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&input._c0_poly), 0);
    _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&input._c1_poly), 0);
    _pgen_decomp      = Alloc_poly(degree, Poly_level(&input._c0_poly), 0);
    _pgen_swk         = Swk(1, -75);
    for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&input._c1_poly);
         _pgen_part_idx = _pgen_part_idx + 1) {
      Decomp(_pgen_decomp, &input._c1_poly, _pgen_part_idx);
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
      _pgen_num_q   = Poly_level(_pgen_mod_down_c0);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q;
           _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree),
                  Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree),
                  Coeffs(&input._c0_poly, _pgen_rns_idx, degree), _pgen_modulus,
                  degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _pgen_order = Auto_order(-75);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q   = Poly_level(_pgen_mod_down_c0);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q;
           _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_rotate(Coeffs(&_pgen_tmp_0._c0_poly, _pgen_rns_idx, degree),
                  Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), _pgen_order,
                  _pgen_modulus, degree);
        Hw_rotate(Coeffs(&_pgen_tmp_0._c1_poly, _pgen_rns_idx, degree),
                  Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree), _pgen_order,
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
  Init_ciph_same_scale(&_pgen_tmp_1, &input, &_pgen_tmp_0);
  Init_ciph_same_scale(&_pgen_tmp_2, &input, 0);
  {
    _pgen_swk_c0      = Alloc_poly(degree, Poly_level(&input._c1_poly), 1);
    _pgen_swk_c1      = Alloc_poly(degree, Poly_level(&input._c1_poly), 1);
    _pgen_ext         = Alloc_poly(degree, Poly_level(&input._c1_poly), 1);
    _pgen_tmp_poly    = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&input._c0_poly), 0);
    _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&input._c1_poly), 0);
    _pgen_decomp      = Alloc_poly(degree, Poly_level(&input._c0_poly), 0);
    _pgen_swk         = Swk(1, -150);
    for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&input._c1_poly);
         _pgen_part_idx = _pgen_part_idx + 1) {
      Decomp(_pgen_decomp, &input._c1_poly, _pgen_part_idx);
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
      _pgen_num_q   = Poly_level(_pgen_mod_down_c0);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q;
           _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree),
                  Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree),
                  Coeffs(&input._c0_poly, _pgen_rns_idx, degree), _pgen_modulus,
                  degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _pgen_order = Auto_order(-150);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q   = Poly_level(_pgen_mod_down_c0);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q;
           _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_rotate(Coeffs(&_pgen_tmp_2._c0_poly, _pgen_rns_idx, degree),
                  Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), _pgen_order,
                  _pgen_modulus, degree);
        Hw_rotate(Coeffs(&_pgen_tmp_2._c1_poly, _pgen_rns_idx, degree),
                  Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree), _pgen_order,
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
  Init_ciph_same_scale(&input_dup_n1, &_pgen_tmp_1, &_pgen_tmp_2);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q   = Poly_level(&input_dup_n1._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q;
         _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_1._c0_poly, _pgen_rns_idx, degree),
                Coeffs(&input._c0_poly, _pgen_rns_idx, degree),
                Coeffs(&_pgen_tmp_0._c0_poly, _pgen_rns_idx, degree),
                _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_1._c1_poly, _pgen_rns_idx, degree),
                Coeffs(&input._c1_poly, _pgen_rns_idx, degree),
                Coeffs(&_pgen_tmp_0._c1_poly, _pgen_rns_idx, degree),
                _pgen_modulus, degree);
      Hw_modadd(Coeffs(&input_dup_n1._c0_poly, _pgen_rns_idx, degree),
                Coeffs(&_pgen_tmp_1._c0_poly, _pgen_rns_idx, degree),
                Coeffs(&_pgen_tmp_2._c0_poly, _pgen_rns_idx, degree),
                _pgen_modulus, degree);
      Hw_modadd(Coeffs(&input_dup_n1._c1_poly, _pgen_rns_idx, degree),
                Coeffs(&_pgen_tmp_1._c1_poly, _pgen_rns_idx, degree),
                Coeffs(&_pgen_tmp_2._c1_poly, _pgen_rns_idx, degree),
                _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  for (index_cin_n1 = 0; index_cin_n1 < 3; index_cin_n1 = index_cin_n1 + 1) {
    for (index_khw_n1 = 0; index_khw_n1 < 9; index_khw_n1 = index_khw_n1 + 1) {
      Init_ciph_same_scale(&_ckks_gen_tmp_41, &input_dup_n1, 0);
      {
        _pgen_swk_c0 =
            Alloc_poly(degree, Poly_level(&input_dup_n1._c1_poly), 1);
        _pgen_swk_c1 =
            Alloc_poly(degree, Poly_level(&input_dup_n1._c1_poly), 1);
        _pgen_ext = Alloc_poly(degree, Poly_level(&input_dup_n1._c1_poly), 1);
        _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
        _pgen_mod_down_c0 =
            Alloc_poly(degree, Poly_level(&input_dup_n1._c0_poly), 0);
        _pgen_mod_down_c1 =
            Alloc_poly(degree, Poly_level(&input_dup_n1._c1_poly), 0);
        _pgen_decomp =
            Alloc_poly(degree, Poly_level(&input_dup_n1._c0_poly), 0);
        _pgen_swk = Swk(1, _cst_6[index_khw_n1]);
        for (_pgen_part_idx = 0;
             _pgen_part_idx < Num_decomp(&input_dup_n1._c1_poly);
             _pgen_part_idx = _pgen_part_idx + 1) {
          Decomp(_pgen_decomp, &input_dup_n1._c1_poly, _pgen_part_idx);
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
                        Coeffs(_pgen_tmp_poly, 0, degree), _pgen_modulus,
                        degree);
              Hw_modmul(Coeffs(_pgen_tmp_poly, 0, degree),
                        Coeffs(_pgen_key1, _pgen_rns_idx, degree),
                        Coeffs(_pgen_ext, _pgen_rns_idx, degree), _pgen_modulus,
                        degree);
              Hw_modadd(Coeffs(_pgen_swk_c1, _pgen_rns_idx, degree),
                        Coeffs(_pgen_swk_c1, _pgen_rns_idx, degree),
                        Coeffs(_pgen_tmp_poly, 0, degree), _pgen_modulus,
                        degree);
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
                        Coeffs(_pgen_tmp_poly, 0, degree), _pgen_modulus,
                        degree);
              Hw_modmul(Coeffs(_pgen_tmp_poly, 0, degree),
                        Coeffs(_pgen_key1, _pgen_p_idx, degree),
                        Coeffs(_pgen_ext, _pgen_p_idx, degree), _pgen_modulus,
                        degree);
              Hw_modadd(Coeffs(_pgen_swk_c1, _pgen_p_idx, degree),
                        Coeffs(_pgen_swk_c1, _pgen_p_idx, degree),
                        Coeffs(_pgen_tmp_poly, 0, degree), _pgen_modulus,
                        degree);
              _pgen_modulus = _pgen_modulus + 1;
            }
          }
        }
        Mod_down(_pgen_mod_down_c0, _pgen_swk_c0);
        Mod_down(_pgen_mod_down_c1, _pgen_swk_c1);
        {
          _pgen_modulus = Q_modulus();
          _pgen_num_q   = Poly_level(_pgen_mod_down_c0);
          for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q;
               _pgen_rns_idx = _pgen_rns_idx + 1) {
            Hw_modadd(Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree),
                      Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree),
                      Coeffs(&input_dup_n1._c0_poly, _pgen_rns_idx, degree),
                      _pgen_modulus, degree);
            _pgen_modulus = _pgen_modulus + 1;
          }
        }
        _pgen_order = Auto_order(_cst_6[index_khw_n1]);
        {
          _pgen_modulus = Q_modulus();
          _pgen_num_q   = Poly_level(_pgen_mod_down_c0);
          for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q;
               _pgen_rns_idx = _pgen_rns_idx + 1) {
            Hw_rotate(Coeffs(&_ckks_gen_tmp_41._c0_poly, _pgen_rns_idx, degree),
                      Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree),
                      _pgen_order, _pgen_modulus, degree);
            Hw_rotate(Coeffs(&_ckks_gen_tmp_41._c1_poly, _pgen_rns_idx, degree),
                      Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree),
                      _pgen_order, _pgen_modulus, degree);
            _pgen_modulus = _pgen_modulus + 1;
          }
        }
        Free_poly(_pgen_swk_c0);
        Free_poly(_pgen_swk_c1);
        Free_poly(_pgen_ext);
        Free_poly(_pgen_tmp_poly);
        Free_poly(_pgen_decomp);
      }
      Encode_float(
          &_pgen_tmp_3, Slice(_cst_4, (index_khw_n1 + (index_cin_n1 * 9)), 25),
          25, Sc_degree(&_ckks_gen_tmp_41), Level(&_ckks_gen_tmp_41));
      Init_ciph_up_scale_plain(&_pgen_tmp_4, &_ckks_gen_tmp_41, &_pgen_tmp_3);
      Init_ciph_same_scale(&tmp_result_n1, &tmp_result_n1, &_pgen_tmp_4);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q   = Poly_level(&tmp_result_n1._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q;
             _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_4._c0_poly, _pgen_rns_idx, degree),
                    Coeffs(&_ckks_gen_tmp_41._c0_poly, _pgen_rns_idx, degree),
                    Coeffs(&_pgen_tmp_3._poly, _pgen_rns_idx, degree),
                    _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_4._c1_poly, _pgen_rns_idx, degree),
                    Coeffs(&_ckks_gen_tmp_41._c1_poly, _pgen_rns_idx, degree),
                    Coeffs(&_pgen_tmp_3._poly, _pgen_rns_idx, degree),
                    _pgen_modulus, degree);
          Hw_modadd(Coeffs(&tmp_result_n1._c0_poly, _pgen_rns_idx, degree),
                    Coeffs(&tmp_result_n1._c0_poly, _pgen_rns_idx, degree),
                    Coeffs(&_pgen_tmp_4._c0_poly, _pgen_rns_idx, degree),
                    _pgen_modulus, degree);
          Hw_modadd(Coeffs(&tmp_result_n1._c1_poly, _pgen_rns_idx, degree),
                    Coeffs(&tmp_result_n1._c1_poly, _pgen_rns_idx, degree),
                    Coeffs(&_pgen_tmp_4._c1_poly, _pgen_rns_idx, degree),
                    _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    {
      _pgen_swk_c0 = Alloc_poly(degree, Poly_level(&input_dup_n1._c1_poly), 1);
      _pgen_swk_c1 = Alloc_poly(degree, Poly_level(&input_dup_n1._c1_poly), 1);
      _pgen_ext    = Alloc_poly(degree, Poly_level(&input_dup_n1._c1_poly), 1);
      _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
      _pgen_mod_down_c0 =
          Alloc_poly(degree, Poly_level(&input_dup_n1._c0_poly), 0);
      _pgen_mod_down_c1 =
          Alloc_poly(degree, Poly_level(&input_dup_n1._c1_poly), 0);
      _pgen_decomp = Alloc_poly(degree, Poly_level(&input_dup_n1._c0_poly), 0);
      _pgen_swk    = Swk(1, 25);
      for (_pgen_part_idx = 0;
           _pgen_part_idx < Num_decomp(&input_dup_n1._c1_poly);
           _pgen_part_idx = _pgen_part_idx + 1) {
        Decomp(_pgen_decomp, &input_dup_n1._c1_poly, _pgen_part_idx);
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
        _pgen_num_q   = Poly_level(_pgen_mod_down_c0);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q;
             _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modadd(Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree),
                    Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree),
                    Coeffs(&input_dup_n1._c0_poly, _pgen_rns_idx, degree),
                    _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
      _pgen_order = Auto_order(25);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q   = Poly_level(_pgen_mod_down_c0);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q;
             _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_rotate(Coeffs(&input_dup_n1._c0_poly, _pgen_rns_idx, degree),
                    Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree),
                    _pgen_order, _pgen_modulus, degree);
          Hw_rotate(Coeffs(&input_dup_n1._c1_poly, _pgen_rns_idx, degree),
                    Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree),
                    _pgen_order, _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
      Free_poly(_pgen_swk_c0);
      Free_poly(_pgen_swk_c1);
      Free_poly(_pgen_ext);
      Free_poly(_pgen_tmp_poly);
      Free_poly(_pgen_decomp);
    }
  }
  Encode_float(&_pgen_tmp_5, _cst_5, 25, Sc_degree(&tmp_result_n1),
                          Level(&tmp_result_n1));
  Init_ciph_same_scale_plain(&tmp_result_n1, &tmp_result_n1, &_pgen_tmp_5);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q   = Poly_level(&tmp_result_n1._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q;
         _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&tmp_result_n1._c0_poly, _pgen_rns_idx, degree),
                Coeffs(&tmp_result_n1._c0_poly, _pgen_rns_idx, degree),
                Coeffs(&_pgen_tmp_5._poly, _pgen_rns_idx, degree),
                _pgen_modulus, degree);
      Set_coeffs(&tmp_result_n1._c1_poly, _pgen_rns_idx, degree,
                 Coeffs(&tmp_result_n1._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_same_scale(&output, &tmp_result_n1, 0);
  Copy_poly(&output._c0_poly, &tmp_result_n1._c0_poly);
  Copy_poly(&output._c1_poly, &tmp_result_n1._c1_poly);
  Set_output_data("output", 0, &output);
  return true;
}
CKKS_PARAMS* Get_context_params() {
  static CKKS_PARAMS parm = {
      LIB_ANT, 1024, 0, 1, 0, 33,
      30,   0, 0, 12, {-150, -75, -6, -5, -4, -1, 0, 1, 4, 5, 6, 25}
  };
  return &parm;
}

DATA_SCHEME* Get_encode_scheme(int idx) {
  static MAP_DESC desc_0[] = {{NORMAL, 0, 0, 0, 0}};
  static DATA_SCHEME scheme_0 = {
    "input", {0, 0, 0, 0}, 1, desc_0
  };
  static DATA_SCHEME* scheme[] = { &scheme_0 };
  return scheme[idx];
}

DATA_SCHEME* Get_decode_scheme(int idx) {
  static MAP_DESC desc[] = {{NORMAL, 0, 0, 0, 0}};
  static DATA_SCHEME scheme = {
    "output", {0, 0, 0, 0}, 1, desc
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
