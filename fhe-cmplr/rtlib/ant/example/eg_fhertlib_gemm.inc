// external header files
#include "rt_ant/rt_ant.h"

typedef float float32_t;

// global constants definition
float32_t _cst_0[6] = {
  0, 1, 2, 3, 4, 5
};
float32_t _cst_1[2] = {
  1, 1
};
float32_t _cst_2[8] = {
  0, 4, 2, 0, 1, 5, 0, 3
};

bool Main_graph() {
  int32_t index0;
  int32_t index1;
  CIPHERTEXT input;
  CIPHERTEXT tmp_result;
  CIPHERTEXT input_dup;
  CIPHERTEXT output;
  CIPHERTEXT _ckks_gen_tmp_32;
  uint32_t _pgen_rns_idx;
  CIPHERTEXT _pgen_tmp_0;
  POLY _pgen_swk_c0;
  POLY _pgen_swk_c1;
  POLY _pgen_ext;
  POLY _pgen_tmp;
  POLY _pgen_mod_down_c0;
  POLY _pgen_mod_down_c1;
  POLY _pgen_decomp;
  SWITCH_KEY* _pgen_swk;
  uint32_t _pgen_part_idx;
  POLY _pgen_key0;
  POLY _pgen_key1;
  MODULUS* _pgen_modulus;
  uint32_t _pgen_num_q;
  uint32_t _pgen_p_ofst;
  uint32_t _pgen_p_idx;
  uint32_t _pgen_num_p;
  int64_t* _pgen_order;
  PLAINTEXT _pgen_tmp_1;
  CIPHERTEXT _pgen_tmp_2;
  CIPHERTEXT _pgen_tmp_3;
  PLAINTEXT _pgen_tmp_4;
  uint32_t  degree = Degree();
  input = Get_input_data("input", 0);
  memset(&output, 0, sizeof(output));
  memset(&tmp_result, 0, sizeof(tmp_result));
  memset(&input_dup, 0, sizeof(input_dup));
  memset(&_pgen_tmp_0, 0, sizeof(_pgen_tmp_0));
  memset(&_ckks_gen_tmp_32, 0, sizeof(_ckks_gen_tmp_32));
  memset(&_pgen_tmp_1, 0, sizeof(_pgen_tmp_1));
  memset(&_pgen_tmp_2, 0, sizeof(_pgen_tmp_2));
  memset(&_pgen_tmp_3, 0, sizeof(_pgen_tmp_3));
  memset(&_pgen_tmp_4, 0, sizeof(_pgen_tmp_4));
  Init_ciph_same_scale(&_pgen_tmp_0, &input, 0);
  {
    _pgen_swk_c0 = Alloc_poly(degree, Poly_level(&input._c1_poly), 1);
    _pgen_swk_c1 = Alloc_poly(degree, Poly_level(&input._c1_poly), 1);
    _pgen_ext = Alloc_poly(degree, Poly_level(&input._c1_poly), 1);
    _pgen_tmp = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&input._c0_poly), 0);
    _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&input._c1_poly), 0);
    _pgen_decomp = Alloc_poly(degree, Poly_level(&input._c0_poly), 0);
    _pgen_swk = Swk(1, -4);
    for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&input._c1_poly); _pgen_part_idx = _pgen_part_idx + 1) {
      Decomp(_pgen_decomp, &input._c1_poly, _pgen_part_idx);
      Mod_up(_pgen_ext, _pgen_decomp, _pgen_part_idx);
      _pgen_key0 = Pk0_at(_pgen_swk, _pgen_part_idx);
      _pgen_key1 = Pk1_at(_pgen_swk, _pgen_part_idx);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(_pgen_ext);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(_pgen_tmp, 0, degree), Coeffs(_pgen_key0, _pgen_rns_idx, degree), Coeffs(_pgen_ext, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(_pgen_swk_c0, _pgen_rns_idx, degree), Coeffs(_pgen_swk_c0, _pgen_rns_idx, degree), Coeffs(_pgen_tmp, 0, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(_pgen_tmp, 0, degree), Coeffs(_pgen_key1, _pgen_rns_idx, degree), Coeffs(_pgen_ext, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(_pgen_swk_c1, _pgen_rns_idx, degree), Coeffs(_pgen_swk_c1, _pgen_rns_idx, degree), Coeffs(_pgen_tmp, 0, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
      {
        _pgen_modulus = P_modulus();
        _pgen_num_p = Num_p(_pgen_ext);
        _pgen_p_ofst = Num_alloc(_pgen_ext) - _pgen_num_p;
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_p; _pgen_rns_idx = _pgen_rns_idx + 1) {
          _pgen_p_idx = _pgen_rns_idx + _pgen_p_ofst;
          Hw_modmul(Coeffs(_pgen_tmp, 0, degree), Coeffs(_pgen_key0, _pgen_p_idx, degree), Coeffs(_pgen_ext, _pgen_p_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(_pgen_swk_c0, _pgen_p_idx, degree), Coeffs(_pgen_swk_c0, _pgen_p_idx, degree), Coeffs(_pgen_tmp, 0, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(_pgen_tmp, 0, degree), Coeffs(_pgen_key1, _pgen_p_idx, degree), Coeffs(_pgen_ext, _pgen_p_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(_pgen_swk_c1, _pgen_p_idx, degree), Coeffs(_pgen_swk_c1, _pgen_p_idx, degree), Coeffs(_pgen_tmp, 0, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    Mod_down(_pgen_mod_down_c0, _pgen_swk_c0);
    Mod_down(_pgen_mod_down_c1, _pgen_swk_c1);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(_pgen_mod_down_c0);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(&input._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _pgen_order = Auto_order(-4);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(_pgen_mod_down_c0);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_rotate(Coeffs(&_pgen_tmp_0._c0_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), _pgen_order, _pgen_modulus, degree);
        Hw_rotate(Coeffs(&_pgen_tmp_0._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree), _pgen_order, _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly(_pgen_swk_c0);
    Free_poly(_pgen_swk_c1);
    Free_poly(_pgen_ext);
    Free_poly(_pgen_tmp);
    Free_poly(_pgen_decomp);
  }
  Init_ciph_same_scale(&input_dup, &input, &_pgen_tmp_0);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&input_dup._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&input_dup._c0_poly, _pgen_rns_idx, degree), Coeffs(&input._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_0._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&input_dup._c1_poly, _pgen_rns_idx, degree), Coeffs(&input._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_0._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  for (index0 = 0; index0 < 2; index0 = index0 + 1) {
    Init_ciph_same_scale(&_ckks_gen_tmp_32, &input_dup, 0);
    {
      _pgen_swk_c0 = Alloc_poly(degree, Poly_level(&input_dup._c1_poly), 1);
      _pgen_swk_c1 = Alloc_poly(degree, Poly_level(&input_dup._c1_poly), 1);
      _pgen_ext = Alloc_poly(degree, Poly_level(&input_dup._c1_poly), 1);
      _pgen_tmp = Alloc_poly(degree, 1, 0);
      _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&input_dup._c0_poly), 0);
      _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&input_dup._c1_poly), 0);
      _pgen_decomp = Alloc_poly(degree, Poly_level(&input_dup._c0_poly), 0);
      _pgen_swk = Swk(1, index0);
      for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&input_dup._c1_poly); _pgen_part_idx = _pgen_part_idx + 1) {
        Decomp(_pgen_decomp, &input_dup._c1_poly, _pgen_part_idx);
        Mod_up(_pgen_ext, _pgen_decomp, _pgen_part_idx);
        _pgen_key0 = Pk0_at(_pgen_swk, _pgen_part_idx);
        _pgen_key1 = Pk1_at(_pgen_swk, _pgen_part_idx);
        {
          _pgen_modulus = Q_modulus();
          _pgen_num_q = Poly_level(_pgen_ext);
          for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
            Hw_modmul(Coeffs(_pgen_tmp, 0, degree), Coeffs(_pgen_key0, _pgen_rns_idx, degree), Coeffs(_pgen_ext, _pgen_rns_idx, degree), _pgen_modulus, degree);
            Hw_modadd(Coeffs(_pgen_swk_c0, _pgen_rns_idx, degree), Coeffs(_pgen_swk_c0, _pgen_rns_idx, degree), Coeffs(_pgen_tmp, 0, degree), _pgen_modulus, degree);
            Hw_modmul(Coeffs(_pgen_tmp, 0, degree), Coeffs(_pgen_key1, _pgen_rns_idx, degree), Coeffs(_pgen_ext, _pgen_rns_idx, degree), _pgen_modulus, degree);
            Hw_modadd(Coeffs(_pgen_swk_c1, _pgen_rns_idx, degree), Coeffs(_pgen_swk_c1, _pgen_rns_idx, degree), Coeffs(_pgen_tmp, 0, degree), _pgen_modulus, degree);
            _pgen_modulus = _pgen_modulus + 1;
          }
        }
        {
          _pgen_modulus = P_modulus();
          _pgen_num_p = Num_p(_pgen_ext);
          _pgen_p_ofst = Num_alloc(_pgen_ext) - _pgen_num_p;
          for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_p; _pgen_rns_idx = _pgen_rns_idx + 1) {
            _pgen_p_idx = _pgen_rns_idx + _pgen_p_ofst;
            Hw_modmul(Coeffs(_pgen_tmp, 0, degree), Coeffs(_pgen_key0, _pgen_p_idx, degree), Coeffs(_pgen_ext, _pgen_p_idx, degree), _pgen_modulus, degree);
            Hw_modadd(Coeffs(_pgen_swk_c0, _pgen_p_idx, degree), Coeffs(_pgen_swk_c0, _pgen_p_idx, degree), Coeffs(_pgen_tmp, 0, degree), _pgen_modulus, degree);
            Hw_modmul(Coeffs(_pgen_tmp, 0, degree), Coeffs(_pgen_key1, _pgen_p_idx, degree), Coeffs(_pgen_ext, _pgen_p_idx, degree), _pgen_modulus, degree);
            Hw_modadd(Coeffs(_pgen_swk_c1, _pgen_p_idx, degree), Coeffs(_pgen_swk_c1, _pgen_p_idx, degree), Coeffs(_pgen_tmp, 0, degree), _pgen_modulus, degree);
            _pgen_modulus = _pgen_modulus + 1;
          }
        }
      }
      Mod_down(_pgen_mod_down_c0, _pgen_swk_c0);
      Mod_down(_pgen_mod_down_c1, _pgen_swk_c1);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(_pgen_mod_down_c0);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modadd(Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(&input_dup._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
      _pgen_order = Auto_order(index0);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(_pgen_mod_down_c0);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_rotate(Coeffs(&_ckks_gen_tmp_32._c0_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), _pgen_order, _pgen_modulus, degree);
          Hw_rotate(Coeffs(&_ckks_gen_tmp_32._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree), _pgen_order, _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
      Free_poly(_pgen_swk_c0);
      Free_poly(_pgen_swk_c1);
      Free_poly(_pgen_ext);
      Free_poly(_pgen_tmp);
      Free_poly(_pgen_decomp);
    }
    Encode_plain_from_float(&_pgen_tmp_1, Slice(_cst_2, index0, 4), 4, Sc_degree(&_ckks_gen_tmp_32), Level(&_ckks_gen_tmp_32));
    Init_ciph_up_scale_plain(&_pgen_tmp_2, &_ckks_gen_tmp_32, &_pgen_tmp_1);
    Init_ciph_same_scale(&tmp_result, &tmp_result, &_pgen_tmp_2);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&tmp_result._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_2._c0_poly, _pgen_rns_idx, degree), Coeffs(&_ckks_gen_tmp_32._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_1._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_2._c1_poly, _pgen_rns_idx, degree), Coeffs(&_ckks_gen_tmp_32._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_1._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&tmp_result._c0_poly, _pgen_rns_idx, degree), Coeffs(&tmp_result._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_2._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&tmp_result._c1_poly, _pgen_rns_idx, degree), Coeffs(&tmp_result._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_2._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
  }
  for (index1 = 0; index1 < 1; index1 = index1 + 1) {
    Init_ciph_same_scale(&_pgen_tmp_3, &tmp_result, 0);
    {
      _pgen_swk_c0 = Alloc_poly(degree, Poly_level(&tmp_result._c1_poly), 1);
      _pgen_swk_c1 = Alloc_poly(degree, Poly_level(&tmp_result._c1_poly), 1);
      _pgen_ext = Alloc_poly(degree, Poly_level(&tmp_result._c1_poly), 1);
      _pgen_tmp = Alloc_poly(degree, 1, 0);
      _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&tmp_result._c0_poly), 0);
      _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&tmp_result._c1_poly), 0);
      _pgen_decomp = Alloc_poly(degree, Poly_level(&tmp_result._c0_poly), 0);
      _pgen_swk = Swk(1, (1 << index1) * 2);
      for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&tmp_result._c1_poly); _pgen_part_idx = _pgen_part_idx + 1) {
        Decomp(_pgen_decomp, &tmp_result._c1_poly, _pgen_part_idx);
        Mod_up(_pgen_ext, _pgen_decomp, _pgen_part_idx);
        _pgen_key0 = Pk0_at(_pgen_swk, _pgen_part_idx);
        _pgen_key1 = Pk1_at(_pgen_swk, _pgen_part_idx);
        {
          _pgen_modulus = Q_modulus();
          _pgen_num_q = Poly_level(_pgen_ext);
          for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
            Hw_modmul(Coeffs(_pgen_tmp, 0, degree), Coeffs(_pgen_key0, _pgen_rns_idx, degree), Coeffs(_pgen_ext, _pgen_rns_idx, degree), _pgen_modulus, degree);
            Hw_modadd(Coeffs(_pgen_swk_c0, _pgen_rns_idx, degree), Coeffs(_pgen_swk_c0, _pgen_rns_idx, degree), Coeffs(_pgen_tmp, 0, degree), _pgen_modulus, degree);
            Hw_modmul(Coeffs(_pgen_tmp, 0, degree), Coeffs(_pgen_key1, _pgen_rns_idx, degree), Coeffs(_pgen_ext, _pgen_rns_idx, degree), _pgen_modulus, degree);
            Hw_modadd(Coeffs(_pgen_swk_c1, _pgen_rns_idx, degree), Coeffs(_pgen_swk_c1, _pgen_rns_idx, degree), Coeffs(_pgen_tmp, 0, degree), _pgen_modulus, degree);
            _pgen_modulus = _pgen_modulus + 1;
          }
        }
        {
          _pgen_modulus = P_modulus();
          _pgen_num_p = Num_p(_pgen_ext);
          _pgen_p_ofst = Num_alloc(_pgen_ext) - _pgen_num_p;
          for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_p; _pgen_rns_idx = _pgen_rns_idx + 1) {
            _pgen_p_idx = _pgen_rns_idx + _pgen_p_ofst;
            Hw_modmul(Coeffs(_pgen_tmp, 0, degree), Coeffs(_pgen_key0, _pgen_p_idx, degree), Coeffs(_pgen_ext, _pgen_p_idx, degree), _pgen_modulus, degree);
            Hw_modadd(Coeffs(_pgen_swk_c0, _pgen_p_idx, degree), Coeffs(_pgen_swk_c0, _pgen_p_idx, degree), Coeffs(_pgen_tmp, 0, degree), _pgen_modulus, degree);
            Hw_modmul(Coeffs(_pgen_tmp, 0, degree), Coeffs(_pgen_key1, _pgen_p_idx, degree), Coeffs(_pgen_ext, _pgen_p_idx, degree), _pgen_modulus, degree);
            Hw_modadd(Coeffs(_pgen_swk_c1, _pgen_p_idx, degree), Coeffs(_pgen_swk_c1, _pgen_p_idx, degree), Coeffs(_pgen_tmp, 0, degree), _pgen_modulus, degree);
            _pgen_modulus = _pgen_modulus + 1;
          }
        }
      }
      Mod_down(_pgen_mod_down_c0, _pgen_swk_c0);
      Mod_down(_pgen_mod_down_c1, _pgen_swk_c1);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(_pgen_mod_down_c0);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modadd(Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(&tmp_result._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
      _pgen_order = Auto_order((1 << index1) * 2);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(_pgen_mod_down_c0);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_rotate(Coeffs(&_pgen_tmp_3._c0_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), _pgen_order, _pgen_modulus, degree);
          Hw_rotate(Coeffs(&_pgen_tmp_3._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree), _pgen_order, _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
      Free_poly(_pgen_swk_c0);
      Free_poly(_pgen_swk_c1);
      Free_poly(_pgen_ext);
      Free_poly(_pgen_tmp);
      Free_poly(_pgen_decomp);
    }
    Init_ciph_same_scale(&tmp_result, &tmp_result, &_pgen_tmp_3);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&tmp_result._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&tmp_result._c0_poly, _pgen_rns_idx, degree), Coeffs(&tmp_result._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_3._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&tmp_result._c1_poly, _pgen_rns_idx, degree), Coeffs(&tmp_result._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_3._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
  }
  Encode_plain_from_float(&_pgen_tmp_4, _cst_1, 2, Sc_degree(&tmp_result), Level(&tmp_result));
  Init_ciph_same_scale_plain(&tmp_result, &tmp_result, &_pgen_tmp_4);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&tmp_result._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&tmp_result._c0_poly, _pgen_rns_idx, degree), Coeffs(&tmp_result._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_4._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&tmp_result._c1_poly, _pgen_rns_idx, degree, Coeffs(&tmp_result._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_same_scale(&output, &tmp_result, 0);
  Copy_poly(&output._c0_poly, &tmp_result._c0_poly);
  Copy_poly(&output._c1_poly, &tmp_result._c1_poly);
  Set_output_data("output", 0, &output);
  return true;
}
CKKS_PARAMS* Get_context_params() {
  static CKKS_PARAMS parm = {
    LIB_ANT, 16, 0, 1, 33, 30, 0, 0, 4,
    { -4, 0, 1, 2 }
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