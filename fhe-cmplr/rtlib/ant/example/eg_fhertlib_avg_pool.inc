// external header files
#include "rt_ant/rt_ant.h"

typedef float float32_t;

// global constants definition
int64_t   _cst_0[2]  = {0, 0};
int64_t   _cst_1[2]  = {4, 4};
int64_t   _cst_2[2]  = {2, 2};
float32_t _cst_3[16] = {0.25, 0, 0.25, 0, 0, 0, 0, 0,
                        0.25, 0, 0.25, 0, 0, 0, 0, 0};
float32_t _cst_4[32] = {1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0,
                        0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0};
float32_t _cst_5[32] = {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                        0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
float32_t _cst_6[16] = {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

bool Main_graph() {
  CIPHERTEXT  input;
  CIPHERTEXT  output;
  CIPHERTEXT  tmp_row_add;
  CIPHERTEXT  orig_result;
  CIPHERTEXT  comb_row_result;
  int32_t     combine_row_index_n0;
  CIPHERTEXT  comb_rc_result;
  int32_t     combine_rc_index_n0;
  CIPHERTEXT  final_result;
  int32_t     combine_cc_index_n0;
  CIPHERTEXT  _ckks_gen_tmp_13;
  CIPHERTEXT  _ckks_gen_tmp_38;
  CIPHERTEXT  _ckks_gen_tmp_69;
  CIPHERTEXT  _ckks_gen_tmp_100;
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
  uint32_t    _pgen_key_p_ofst;
  uint32_t    _pgen_key_p_idx;
  int64_t*    _pgen_order;
  CIPHERTEXT  _pgen_tmp_1;
  PLAINTEXT   _pgen_tmp_2;
  PLAINTEXT   _pgen_tmp_3;
  CIPHERTEXT  _pgen_tmp_4;
  PLAINTEXT   _pgen_tmp_5;
  CIPHERTEXT  _pgen_tmp_6;
  PLAINTEXT   _pgen_tmp_7;
  CIPHERTEXT  _pgen_tmp_8;
  uint32_t    degree = Degree();
  input              = Get_input_data("input", 0);
  memset(&output, 0, sizeof(output));
  memset(&tmp_row_add, 0, sizeof(tmp_row_add));
  memset(&orig_result, 0, sizeof(orig_result));
  memset(&comb_row_result, 0, sizeof(comb_row_result));
  memset(&comb_rc_result, 0, sizeof(comb_rc_result));
  memset(&final_result, 0, sizeof(final_result));
  memset(&_ckks_gen_tmp_13, 0, sizeof(_ckks_gen_tmp_13));
  memset(&_ckks_gen_tmp_38, 0, sizeof(_ckks_gen_tmp_38));
  memset(&_ckks_gen_tmp_69, 0, sizeof(_ckks_gen_tmp_69));
  memset(&_ckks_gen_tmp_100, 0, sizeof(_ckks_gen_tmp_100));
  memset(&_pgen_tmp_0, 0, sizeof(_pgen_tmp_0));
  memset(&_pgen_tmp_1, 0, sizeof(_pgen_tmp_1));
  memset(&_pgen_tmp_2, 0, sizeof(_pgen_tmp_2));
  memset(&_pgen_tmp_3, 0, sizeof(_pgen_tmp_3));
  memset(&_pgen_tmp_4, 0, sizeof(_pgen_tmp_4));
  memset(&_pgen_tmp_5, 0, sizeof(_pgen_tmp_5));
  memset(&_pgen_tmp_6, 0, sizeof(_pgen_tmp_6));
  memset(&_pgen_tmp_7, 0, sizeof(_pgen_tmp_7));
  memset(&_pgen_tmp_8, 0, sizeof(_pgen_tmp_8));
  Init_ciph_same_scale(&_pgen_tmp_0, &input, 0);
  {
    _pgen_swk_c0      = Alloc_poly(degree, Poly_level(&input._c1_poly), 1);
    _pgen_swk_c1      = Alloc_poly(degree, Poly_level(&input._c1_poly), 1);
    _pgen_ext         = Alloc_poly(degree, Poly_level(&input._c1_poly), 1);
    _pgen_tmp_poly    = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&input._c0_poly), 0);
    _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&input._c1_poly), 0);
    _pgen_decomp      = Alloc_poly(degree, Poly_level(&input._c0_poly), 0);
    _pgen_swk         = Swk(1, 4);
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
        _pgen_key_p_ofst = Poly_level(_pgen_key0);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_p;
             _pgen_rns_idx = _pgen_rns_idx + 1) {
          _pgen_p_idx = _pgen_rns_idx + _pgen_p_ofst;
          _pgen_key_p_idx = _pgen_rns_idx + _pgen_key_p_ofst;
          Hw_modmul(Coeffs(_pgen_tmp_poly, 0, degree),
                    Coeffs(_pgen_key0, _pgen_key_p_idx, degree),
                    Coeffs(_pgen_ext, _pgen_p_idx, degree), _pgen_modulus,
                    degree);
          Hw_modadd(Coeffs(_pgen_swk_c0, _pgen_p_idx, degree),
                    Coeffs(_pgen_swk_c0, _pgen_p_idx, degree),
                    Coeffs(_pgen_tmp_poly, 0, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(_pgen_tmp_poly, 0, degree),
                    Coeffs(_pgen_key1, _pgen_key_p_idx, degree),
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
    _pgen_order = Auto_order(4);
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
  Init_ciph_same_scale(&tmp_row_add, &input, &_pgen_tmp_0);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q   = Poly_level(&tmp_row_add._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q;
         _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&tmp_row_add._c0_poly, _pgen_rns_idx, degree),
                Coeffs(&input._c0_poly, _pgen_rns_idx, degree),
                Coeffs(&_pgen_tmp_0._c0_poly, _pgen_rns_idx, degree),
                _pgen_modulus, degree);
      Hw_modadd(Coeffs(&tmp_row_add._c1_poly, _pgen_rns_idx, degree),
                Coeffs(&input._c1_poly, _pgen_rns_idx, degree),
                Coeffs(&_pgen_tmp_0._c1_poly, _pgen_rns_idx, degree),
                _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_same_scale(&_pgen_tmp_1, &tmp_row_add, 0);
  {
    _pgen_swk_c0   = Alloc_poly(degree, Poly_level(&tmp_row_add._c1_poly), 1);
    _pgen_swk_c1   = Alloc_poly(degree, Poly_level(&tmp_row_add._c1_poly), 1);
    _pgen_ext      = Alloc_poly(degree, Poly_level(&tmp_row_add._c1_poly), 1);
    _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 =
        Alloc_poly(degree, Poly_level(&tmp_row_add._c0_poly), 0);
    _pgen_mod_down_c1 =
        Alloc_poly(degree, Poly_level(&tmp_row_add._c1_poly), 0);
    _pgen_decomp = Alloc_poly(degree, Poly_level(&tmp_row_add._c0_poly), 0);
    _pgen_swk    = Swk(1, 1);
    for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&tmp_row_add._c1_poly);
         _pgen_part_idx = _pgen_part_idx + 1) {
      Decomp(_pgen_decomp, &tmp_row_add._c1_poly, _pgen_part_idx);
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
        _pgen_key_p_ofst = Poly_level(_pgen_key0);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_p;
             _pgen_rns_idx = _pgen_rns_idx + 1) {
          _pgen_p_idx = _pgen_rns_idx + _pgen_p_ofst;
          _pgen_key_p_idx = _pgen_rns_idx + _pgen_key_p_ofst;
          Hw_modmul(Coeffs(_pgen_tmp_poly, 0, degree),
                    Coeffs(_pgen_key0, _pgen_key_p_idx, degree),
                    Coeffs(_pgen_ext, _pgen_p_idx, degree), _pgen_modulus,
                    degree);
          Hw_modadd(Coeffs(_pgen_swk_c0, _pgen_p_idx, degree),
                    Coeffs(_pgen_swk_c0, _pgen_p_idx, degree),
                    Coeffs(_pgen_tmp_poly, 0, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(_pgen_tmp_poly, 0, degree),
                    Coeffs(_pgen_key1, _pgen_key_p_idx, degree),
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
                  Coeffs(&tmp_row_add._c0_poly, _pgen_rns_idx, degree),
                  _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _pgen_order = Auto_order(1);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q   = Poly_level(_pgen_mod_down_c0);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q;
           _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_rotate(Coeffs(&_pgen_tmp_1._c0_poly, _pgen_rns_idx, degree),
                  Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), _pgen_order,
                  _pgen_modulus, degree);
        Hw_rotate(Coeffs(&_pgen_tmp_1._c1_poly, _pgen_rns_idx, degree),
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
  Init_ciph_same_scale(&_ckks_gen_tmp_13, &tmp_row_add, &_pgen_tmp_1);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q   = Poly_level(&_ckks_gen_tmp_13._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q;
         _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_ckks_gen_tmp_13._c0_poly, _pgen_rns_idx, degree),
                Coeffs(&tmp_row_add._c0_poly, _pgen_rns_idx, degree),
                Coeffs(&_pgen_tmp_1._c0_poly, _pgen_rns_idx, degree),
                _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_ckks_gen_tmp_13._c1_poly, _pgen_rns_idx, degree),
                Coeffs(&tmp_row_add._c1_poly, _pgen_rns_idx, degree),
                Coeffs(&_pgen_tmp_1._c1_poly, _pgen_rns_idx, degree),
                _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Encode_plain_from_float(&_pgen_tmp_2, _cst_3, 16,
                          Sc_degree(&_ckks_gen_tmp_13),
                          Level(&_ckks_gen_tmp_13));

  Init_ciph_up_scale_plain(&orig_result, &_ckks_gen_tmp_13, &_pgen_tmp_2);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q   = Poly_level(&orig_result._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q;
         _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&orig_result._c0_poly, _pgen_rns_idx, degree),
                Coeffs(&_ckks_gen_tmp_13._c0_poly, _pgen_rns_idx, degree),
                Coeffs(&_pgen_tmp_2._poly, _pgen_rns_idx, degree),
                _pgen_modulus, degree);
      Hw_modmul(Coeffs(&orig_result._c1_poly, _pgen_rns_idx, degree),
                Coeffs(&_ckks_gen_tmp_13._c1_poly, _pgen_rns_idx, degree),
                Coeffs(&_pgen_tmp_2._poly, _pgen_rns_idx, degree),
                _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_down_scale(&orig_result, &orig_result);
  Rescale(&orig_result._c0_poly, &orig_result._c0_poly);
  Rescale(&orig_result._c1_poly, &orig_result._c1_poly);
  for (combine_row_index_n0 = 0; combine_row_index_n0 < 2;
       combine_row_index_n0 = combine_row_index_n0 + 1) {
    Init_ciph_same_scale(&_ckks_gen_tmp_38, &orig_result, 0);
    {
      _pgen_swk_c0   = Alloc_poly(degree, Poly_level(&orig_result._c1_poly), 1);
      _pgen_swk_c1   = Alloc_poly(degree, Poly_level(&orig_result._c1_poly), 1);
      _pgen_ext      = Alloc_poly(degree, Poly_level(&orig_result._c1_poly), 1);
      _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
      _pgen_mod_down_c0 =
          Alloc_poly(degree, Poly_level(&orig_result._c0_poly), 0);
      _pgen_mod_down_c1 =
          Alloc_poly(degree, Poly_level(&orig_result._c1_poly), 0);
      _pgen_decomp = Alloc_poly(degree, Poly_level(&orig_result._c0_poly), 0);
      _pgen_swk    = Swk(1, combine_row_index_n0);
      for (_pgen_part_idx = 0;
           _pgen_part_idx < Num_decomp(&orig_result._c1_poly);
           _pgen_part_idx = _pgen_part_idx + 1) {
        Decomp(_pgen_decomp, &orig_result._c1_poly, _pgen_part_idx);
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
          _pgen_key_p_ofst = Poly_level(_pgen_key0);
          for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_p;
               _pgen_rns_idx = _pgen_rns_idx + 1) {
            _pgen_p_idx = _pgen_rns_idx + _pgen_p_ofst;
            _pgen_key_p_idx = _pgen_rns_idx + _pgen_key_p_ofst;
            Hw_modmul(Coeffs(_pgen_tmp_poly, 0, degree),
                      Coeffs(_pgen_key0, _pgen_key_p_idx, degree),
                      Coeffs(_pgen_ext, _pgen_p_idx, degree), _pgen_modulus,
                      degree);
            Hw_modadd(Coeffs(_pgen_swk_c0, _pgen_p_idx, degree),
                      Coeffs(_pgen_swk_c0, _pgen_p_idx, degree),
                      Coeffs(_pgen_tmp_poly, 0, degree), _pgen_modulus, degree);
            Hw_modmul(Coeffs(_pgen_tmp_poly, 0, degree),
                      Coeffs(_pgen_key1, _pgen_key_p_idx, degree),
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
                    Coeffs(&orig_result._c0_poly, _pgen_rns_idx, degree),
                    _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
      _pgen_order = Auto_order(combine_row_index_n0);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q   = Poly_level(_pgen_mod_down_c0);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q;
             _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_rotate(Coeffs(&_ckks_gen_tmp_38._c0_poly, _pgen_rns_idx, degree),
                    Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree),
                    _pgen_order, _pgen_modulus, degree);
          Hw_rotate(Coeffs(&_ckks_gen_tmp_38._c1_poly, _pgen_rns_idx, degree),
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
    Encode_plain_from_float(
        &_pgen_tmp_3, Slice(_cst_4, combine_row_index_n0, 16), 16,
        Sc_degree(&_ckks_gen_tmp_38), Level(&_ckks_gen_tmp_38));
    Init_ciph_up_scale_plain(&_pgen_tmp_4, &_ckks_gen_tmp_38, &_pgen_tmp_3);
    Init_ciph_same_scale(&comb_row_result, &comb_row_result, &_pgen_tmp_4);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q   = Poly_level(&comb_row_result._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q;
           _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_4._c0_poly, _pgen_rns_idx, degree),
                  Coeffs(&_ckks_gen_tmp_38._c0_poly, _pgen_rns_idx, degree),
                  Coeffs(&_pgen_tmp_3._poly, _pgen_rns_idx, degree),
                  _pgen_modulus, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_4._c1_poly, _pgen_rns_idx, degree),
                  Coeffs(&_ckks_gen_tmp_38._c1_poly, _pgen_rns_idx, degree),
                  Coeffs(&_pgen_tmp_3._poly, _pgen_rns_idx, degree),
                  _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_row_result._c0_poly, _pgen_rns_idx, degree),
                  Coeffs(&comb_row_result._c0_poly, _pgen_rns_idx, degree),
                  Coeffs(&_pgen_tmp_4._c0_poly, _pgen_rns_idx, degree),
                  _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_row_result._c1_poly, _pgen_rns_idx, degree),
                  Coeffs(&comb_row_result._c1_poly, _pgen_rns_idx, degree),
                  Coeffs(&_pgen_tmp_4._c1_poly, _pgen_rns_idx, degree),
                  _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
  };
  Init_ciph_down_scale(&comb_row_result, &comb_row_result);
  Rescale(&comb_row_result._c0_poly, &comb_row_result._c0_poly);
  Rescale(&comb_row_result._c1_poly, &comb_row_result._c1_poly);
  for (combine_rc_index_n0 = 0; combine_rc_index_n0 < 2;
       combine_rc_index_n0 = combine_rc_index_n0 + 1) {
    Init_ciph_same_scale(&_ckks_gen_tmp_69, &comb_row_result, 0);
    {
      _pgen_swk_c0 =
          Alloc_poly(degree, Poly_level(&comb_row_result._c1_poly), 1);
      _pgen_swk_c1 =
          Alloc_poly(degree, Poly_level(&comb_row_result._c1_poly), 1);
      _pgen_ext = Alloc_poly(degree, Poly_level(&comb_row_result._c1_poly), 1);
      _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
      _pgen_mod_down_c0 =
          Alloc_poly(degree, Poly_level(&comb_row_result._c0_poly), 0);
      _pgen_mod_down_c1 =
          Alloc_poly(degree, Poly_level(&comb_row_result._c1_poly), 0);
      _pgen_decomp =
          Alloc_poly(degree, Poly_level(&comb_row_result._c0_poly), 0);
      _pgen_swk = Swk(1, (combine_rc_index_n0 * 6));
      for (_pgen_part_idx = 0;
           _pgen_part_idx < Num_decomp(&comb_row_result._c1_poly);
           _pgen_part_idx = _pgen_part_idx + 1) {
        Decomp(_pgen_decomp, &comb_row_result._c1_poly, _pgen_part_idx);
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
          _pgen_key_p_ofst = Poly_level(_pgen_key0);
          for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_p;
               _pgen_rns_idx = _pgen_rns_idx + 1) {
            _pgen_p_idx = _pgen_rns_idx + _pgen_p_ofst;
            _pgen_key_p_idx = _pgen_rns_idx + _pgen_key_p_ofst;
            Hw_modmul(Coeffs(_pgen_tmp_poly, 0, degree),
                      Coeffs(_pgen_key0, _pgen_key_p_idx, degree),
                      Coeffs(_pgen_ext, _pgen_p_idx, degree), _pgen_modulus,
                      degree);
            Hw_modadd(Coeffs(_pgen_swk_c0, _pgen_p_idx, degree),
                      Coeffs(_pgen_swk_c0, _pgen_p_idx, degree),
                      Coeffs(_pgen_tmp_poly, 0, degree), _pgen_modulus, degree);
            Hw_modmul(Coeffs(_pgen_tmp_poly, 0, degree),
                      Coeffs(_pgen_key1, _pgen_key_p_idx, degree),
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
                    Coeffs(&comb_row_result._c0_poly, _pgen_rns_idx, degree),
                    _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
      _pgen_order = Auto_order((combine_rc_index_n0 * 6));
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q   = Poly_level(_pgen_mod_down_c0);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q;
             _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_rotate(Coeffs(&_ckks_gen_tmp_69._c0_poly, _pgen_rns_idx, degree),
                    Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree),
                    _pgen_order, _pgen_modulus, degree);
          Hw_rotate(Coeffs(&_ckks_gen_tmp_69._c1_poly, _pgen_rns_idx, degree),
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
    Encode_plain_from_float(
        &_pgen_tmp_5, Slice(_cst_5, combine_rc_index_n0, 16), 16,
        Sc_degree(&_ckks_gen_tmp_69), Level(&_ckks_gen_tmp_69));
    Init_ciph_up_scale_plain(&_pgen_tmp_6, &_ckks_gen_tmp_69, &_pgen_tmp_5);
    Init_ciph_same_scale(&comb_rc_result, &comb_rc_result, &_pgen_tmp_6);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q   = Poly_level(&comb_rc_result._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q;
           _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_6._c0_poly, _pgen_rns_idx, degree),
                  Coeffs(&_ckks_gen_tmp_69._c0_poly, _pgen_rns_idx, degree),
                  Coeffs(&_pgen_tmp_5._poly, _pgen_rns_idx, degree),
                  _pgen_modulus, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_6._c1_poly, _pgen_rns_idx, degree),
                  Coeffs(&_ckks_gen_tmp_69._c1_poly, _pgen_rns_idx, degree),
                  Coeffs(&_pgen_tmp_5._poly, _pgen_rns_idx, degree),
                  _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_rc_result._c0_poly, _pgen_rns_idx, degree),
                  Coeffs(&comb_rc_result._c0_poly, _pgen_rns_idx, degree),
                  Coeffs(&_pgen_tmp_6._c0_poly, _pgen_rns_idx, degree),
                  _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_rc_result._c1_poly, _pgen_rns_idx, degree),
                  Coeffs(&comb_rc_result._c1_poly, _pgen_rns_idx, degree),
                  Coeffs(&_pgen_tmp_6._c1_poly, _pgen_rns_idx, degree),
                  _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
  };
  Init_ciph_down_scale(&comb_rc_result, &comb_rc_result);
  Rescale(&comb_rc_result._c0_poly, &comb_rc_result._c0_poly);
  Rescale(&comb_rc_result._c1_poly, &comb_rc_result._c1_poly);
  for (combine_cc_index_n0 = 0; combine_cc_index_n0 < 1;
       combine_cc_index_n0 = combine_cc_index_n0 + 1) {
    Init_ciph_same_scale(&_ckks_gen_tmp_100, &comb_rc_result, 0);
    {
      _pgen_swk_c0 =
          Alloc_poly(degree, Poly_level(&comb_rc_result._c1_poly), 1);
      _pgen_swk_c1 =
          Alloc_poly(degree, Poly_level(&comb_rc_result._c1_poly), 1);
      _pgen_ext = Alloc_poly(degree, Poly_level(&comb_rc_result._c1_poly), 1);
      _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
      _pgen_mod_down_c0 =
          Alloc_poly(degree, Poly_level(&comb_rc_result._c0_poly), 0);
      _pgen_mod_down_c1 =
          Alloc_poly(degree, Poly_level(&comb_rc_result._c1_poly), 0);
      _pgen_decomp =
          Alloc_poly(degree, Poly_level(&comb_rc_result._c0_poly), 0);
      _pgen_swk = Swk(1, (combine_cc_index_n0 * 12));
      for (_pgen_part_idx = 0;
           _pgen_part_idx < Num_decomp(&comb_rc_result._c1_poly);
           _pgen_part_idx = _pgen_part_idx + 1) {
        Decomp(_pgen_decomp, &comb_rc_result._c1_poly, _pgen_part_idx);
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
          _pgen_key_p_ofst = Poly_level(_pgen_key0);
          for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_p;
               _pgen_rns_idx = _pgen_rns_idx + 1) {
            _pgen_p_idx = _pgen_rns_idx + _pgen_p_ofst;
            _pgen_key_p_idx = _pgen_rns_idx + _pgen_key_p_ofst;
            Hw_modmul(Coeffs(_pgen_tmp_poly, 0, degree),
                      Coeffs(_pgen_key0, _pgen_key_p_idx, degree),
                      Coeffs(_pgen_ext, _pgen_p_idx, degree), _pgen_modulus,
                      degree);
            Hw_modadd(Coeffs(_pgen_swk_c0, _pgen_p_idx, degree),
                      Coeffs(_pgen_swk_c0, _pgen_p_idx, degree),
                      Coeffs(_pgen_tmp_poly, 0, degree), _pgen_modulus, degree);
            Hw_modmul(Coeffs(_pgen_tmp_poly, 0, degree),
                      Coeffs(_pgen_key1, _pgen_key_p_idx, degree),
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
                    Coeffs(&comb_rc_result._c0_poly, _pgen_rns_idx, degree),
                    _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
      _pgen_order = Auto_order((combine_cc_index_n0 * 12));
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q   = Poly_level(_pgen_mod_down_c0);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q;
             _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_rotate(Coeffs(&_ckks_gen_tmp_100._c0_poly, _pgen_rns_idx, degree),
                    Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree),
                    _pgen_order, _pgen_modulus, degree);
          Hw_rotate(Coeffs(&_ckks_gen_tmp_100._c1_poly, _pgen_rns_idx, degree),
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
    Encode_plain_from_float(
        &_pgen_tmp_7, Slice(_cst_6, combine_cc_index_n0, 16), 16,
        Sc_degree(&_ckks_gen_tmp_100), Level(&_ckks_gen_tmp_100));
    Init_ciph_up_scale_plain(&_pgen_tmp_8, &_ckks_gen_tmp_100, &_pgen_tmp_7);
    Init_ciph_same_scale(&final_result, &final_result, &_pgen_tmp_8);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q   = Poly_level(&final_result._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q;
           _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_8._c0_poly, _pgen_rns_idx, degree),
                  Coeffs(&_ckks_gen_tmp_100._c0_poly, _pgen_rns_idx, degree),
                  Coeffs(&_pgen_tmp_7._poly, _pgen_rns_idx, degree),
                  _pgen_modulus, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_8._c1_poly, _pgen_rns_idx, degree),
                  Coeffs(&_ckks_gen_tmp_100._c1_poly, _pgen_rns_idx, degree),
                  Coeffs(&_pgen_tmp_7._poly, _pgen_rns_idx, degree),
                  _pgen_modulus, degree);
        Hw_modadd(Coeffs(&final_result._c0_poly, _pgen_rns_idx, degree),
                  Coeffs(&final_result._c0_poly, _pgen_rns_idx, degree),
                  Coeffs(&_pgen_tmp_8._c0_poly, _pgen_rns_idx, degree),
                  _pgen_modulus, degree);
        Hw_modadd(Coeffs(&final_result._c1_poly, _pgen_rns_idx, degree),
                  Coeffs(&final_result._c1_poly, _pgen_rns_idx, degree),
                  Coeffs(&_pgen_tmp_8._c1_poly, _pgen_rns_idx, degree),
                  _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
  }
  Init_ciph_same_scale(&output, &final_result, 0);
  Copy_poly(&output._c0_poly, &final_result._c0_poly);
  Copy_poly(&output._c1_poly, &final_result._c1_poly);
  Set_output_data("output", 0, &output);
  return true;
}
CKKS_PARAMS* Get_context_params() {
  static CKKS_PARAMS parm = {
      LIB_ANT, 128, 0, 4, 33, 30, 0, 0, 4, {0, 1, 4, 6}
  };
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