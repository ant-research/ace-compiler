// external header files
#include "rt_ant/rt_ant.h"

typedef float float32_t;

// global constants definition
float32_t _cst_0[147] = {
    0.073609,    -0.063054,  -0.0276945,  0.0670527,  -0.134491,  0.0856739,
    -0.0293893,  0.0726777,  0.0198596,   -0.017492,  0.039623,   0.0070474,
    0.0521755,   -0.0556716, -0.0104155,  -0.012861,  0.0207063,  -0.000570685,
    0.124881,    0.0444557,  -0.0532009,  -0.0862802, -0.0239451, -0.0616201,
    -0.0457784,  0.00684023, 0.0851611,   0.0776481,  -0.13965,   0.0885601,
    0.0399091,   0.135508,   0.0942943,   -0.130163,  -0.13583,   -0.0689046,
    0.125445,    -0.0237956, 0.0611371,   -0.0663873, 0.140174,   -0.0604426,
    0.107132,    0.00169167, -0.0752596,  0.0734307,  -0.0758315, 0.042015,
    -0.041251,   -0.0156621, -0.137341,   -0.0681117, 0.0775191,  -0.0347253,
    0.142293,    0.114513,   -0.00668909, -0.0953547, 0.0869946,  0.0443367,
    -0.0923457,  0.0927921,  0.0867288,   0.126699,   -0.08008,   -0.0235151,
    -0.00276741, 0.0208654,  -0.108417,   -0.101375,  0.0777149,  -0.0334989,
    0.0697819,   0.00814427, 0.0469064,   0.0314127,  0.0519428,  0.0708158,
    -0.132302,   0.0719074,  -0.100446,   -0.107787,  0.00868776, -0.0243439,
    0.0839034,   -0.0827337, -0.126997,   0.103967,   -0.0211831, 0.080351,
    0.0459264,   -0.107125,  0.0286998,   0.0343138,  -0.0956512, -0.0677818,
    0.0487108,   0.0255889,  -0.0607675,  -0.0432451, 0.13084,    -0.0264316,
    0.0805417,   0.0618576,  -0.0923439,  -0.12149,   0.137127,   0.00745954,
    0.0979226,   0.0296045,  0.0459461,   0.106708,   0.135459,   -0.0947973,
    0.0178613,   0.106608,   0.103491,    0.0887484,  -0.103388,  -0.102872,
    -0.0863959,  0.0179469,  0.142381,    -0.0902394, 0.0761258,  -0.0790697,
    -0.134317,   -0.0303628, 0.0823101,   0.132625,   -0.0887198, 0.0310016,
    0.123266,    0.0946648,  0.0890319,   0.101519,   0.0903595,  0.0368995,
    -0.0976886,  -0.119966,  -0.0654601,  -0.0166351, -0.0875646, 0.0522648,
    0.0442125,   -0.0323479, 0.0549058};
float32_t _cst_1[3]   = {0.0461693, 0.0872198, 0.0962062};
float32_t _cst_2[153] = {
    0.073609,   -0.137341,  0.13084,      0.0670527,  -0.0347253,
    0.0618576,  -0.0293893, -0.00668909,  0.137127,   -0.017492,
    0.0443367,  0.0296045,  0.0521755,    0.0867288,  0.135459,
    -0.012861,  -0.0235151, 0.106608,     0.124881,   -0.108417,
    -0.103388,  -0.0862802, -0.0334989,   0.0179469,  -0.0457784,
    0.0469064,  0.0761258,  0.0776481,    0.0708158,  -0.0303628,
    0.0399091,  -0.100446,  -0.0887198,   -0.130163,  -0.0243439,
    0.0946648,  0.125445,   -0.126997,    0.0903595,  -0.0663873,
    0.080351,   -0.119966,  0.107132,     0.0286998,  -0.0875646,
    0.0734307,  -0.0677818, -0.0323479,   -0.041251,  0,
    0,          -0.063054,  -0.0681117,   -0.0264316, -0.134491,
    0.142293,   -0.0923439, 0.0726777,    -0.0953547, 0.00745954,
    0.039623,   -0.0923457, 0.0459461,    -0.0556716, 0.126699,
    -0.0947973, 0.0207063,  -0.00276741,  0.103491,   0.0444557,
    -0.101375,  -0.102872,  -0.0239451,   0.0697819,  0.142381,
    0.00684023, 0.0314127,  -0.0790697,   -0.13965,   -0.132302,
    0.0823101,  0.135508,   -0.107787,    0.0310016,  -0.13583,
    0.0839034,  0.0890319,  -0.0237956,   0.103967,   0.0368995,
    0.140174,   0.0459264,  -0.0654601,   0.00169167, 0.0343138,
    0.0522648,  -0.0758315, 0.0487108,    0.0549058,  0,
    0,          -0.0607675, -0.0276945,   0.0775191,  0.0805417,
    0.0856739,  0.114513,   -0.12149,     0.0198596,  0.0869946,
    0.0979226,  0.0070474,  0.0927921,    0.106708,   -0.0104155,
    -0.08008,   0.0178613,  -0.000570685, 0.0208654,  0.0887484,
    -0.0532009, 0.0777149,  -0.0863959,   -0.0616201, 0.00814427,
    -0.0902394, 0.0851611,  0.0519428,    -0.134317,  0.0885601,
    0.0719074,  0.132625,   0.0942943,    0.00868776, 0.123266,
    -0.0689046, -0.0827337, 0.101519,     0.0611371,  -0.0211831,
    -0.0976886, -0.0604426, -0.107125,    -0.0166351, -0.0752596,
    -0.0956512, 0.0442125,  0.042015,     0.0255889,  0,
    0,          -0.0156621, -0.0432451};

bool Main_graph() {
  CIPHERTEXT  input;
  CIPHERTEXT  output;
  CIPHERTEXT  tmp_result_n1;
  CIPHERTEXT  input_dup_n1;
  int32_t     index_gemm_n1;
  int32_t     index_add_n1;
  CIPHERTEXT  _ckks_gen_tmp_24;
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
  PLAINTEXT   _pgen_tmp_1;
  CIPHERTEXT  _pgen_tmp_2;
  CIPHERTEXT  _pgen_tmp_3;
  PLAINTEXT   _pgen_tmp_4;
  uint32_t    degree = Degree();
  input              = Get_input_data("input", 0);
  memset(&output, 0, sizeof(output));
  memset(&tmp_result_n1, 0, sizeof(tmp_result_n1));
  memset(&input_dup_n1, 0, sizeof(input_dup_n1));
  memset(&_ckks_gen_tmp_24, 0, sizeof(_ckks_gen_tmp_24));
  memset(&_pgen_tmp_0, 0, sizeof(_pgen_tmp_0));
  memset(&_pgen_tmp_1, 0, sizeof(_pgen_tmp_1));
  memset(&_pgen_tmp_2, 0, sizeof(_pgen_tmp_2));
  memset(&_pgen_tmp_3, 0, sizeof(_pgen_tmp_3));
  memset(&_pgen_tmp_4, 0, sizeof(_pgen_tmp_4));
  Init_ciph_same_scale(&_pgen_tmp_0, &input, 0);
  {
    _pgen_swk_c0      = Alloc_poly(degree, Poly_level(&input._c1_poly), 1);
    _pgen_swk_c1      = Alloc_poly(degree, Poly_level(&input._c1_poly), 1);
    _pgen_ext         = Alloc_poly(degree, Poly_level(&input._c1_poly), 1);
    _pgen_tmp_poly    = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&input._c0_poly), 0);
    _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&input._c1_poly), 0);
    _pgen_decomp      = Alloc_poly(degree, Poly_level(&input._c0_poly), 0);
    _pgen_swk         = Swk(1, -51);
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
    _pgen_order = Auto_order(-51);
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
  Init_ciph_same_scale(&input_dup_n1, &input, &_pgen_tmp_0);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q   = Poly_level(&input_dup_n1._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q;
         _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&input_dup_n1._c0_poly, _pgen_rns_idx, degree),
                Coeffs(&input._c0_poly, _pgen_rns_idx, degree),
                Coeffs(&_pgen_tmp_0._c0_poly, _pgen_rns_idx, degree),
                _pgen_modulus, degree);
      Hw_modadd(Coeffs(&input_dup_n1._c1_poly, _pgen_rns_idx, degree),
                Coeffs(&input._c1_poly, _pgen_rns_idx, degree),
                Coeffs(&_pgen_tmp_0._c1_poly, _pgen_rns_idx, degree),
                _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  for (index_gemm_n1 = 0; index_gemm_n1 < 3;
       index_gemm_n1 = index_gemm_n1 + 1) {
    Init_ciph_same_scale(&_ckks_gen_tmp_24, &input_dup_n1, 0);
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
      _pgen_swk    = Swk(1, index_gemm_n1);
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
      _pgen_order = Auto_order(index_gemm_n1);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q   = Poly_level(_pgen_mod_down_c0);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q;
             _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_rotate(Coeffs(&_ckks_gen_tmp_24._c0_poly, _pgen_rns_idx, degree),
                    Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree),
                    _pgen_order, _pgen_modulus, degree);
          Hw_rotate(Coeffs(&_ckks_gen_tmp_24._c1_poly, _pgen_rns_idx, degree),
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
    Encode_plain_from_float(&_pgen_tmp_1, Slice(_cst_2, index_gemm_n1, 51), 51,
                            Sc_degree(&_ckks_gen_tmp_24),
                            Level(&_ckks_gen_tmp_24));
    Init_ciph_up_scale_plain(&_pgen_tmp_2, &_ckks_gen_tmp_24, &_pgen_tmp_1);
    Init_ciph_same_scale(&tmp_result_n1, &tmp_result_n1, &_pgen_tmp_2);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q   = Poly_level(&tmp_result_n1._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q;
           _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_2._c0_poly, _pgen_rns_idx, degree),
                  Coeffs(&_ckks_gen_tmp_24._c0_poly, _pgen_rns_idx, degree),
                  Coeffs(&_pgen_tmp_1._poly, _pgen_rns_idx, degree),
                  _pgen_modulus, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_2._c1_poly, _pgen_rns_idx, degree),
                  Coeffs(&_ckks_gen_tmp_24._c1_poly, _pgen_rns_idx, degree),
                  Coeffs(&_pgen_tmp_1._poly, _pgen_rns_idx, degree),
                  _pgen_modulus, degree);
        Hw_modadd(Coeffs(&tmp_result_n1._c0_poly, _pgen_rns_idx, degree),
                  Coeffs(&tmp_result_n1._c0_poly, _pgen_rns_idx, degree),
                  Coeffs(&_pgen_tmp_2._c0_poly, _pgen_rns_idx, degree),
                  _pgen_modulus, degree);
        Hw_modadd(Coeffs(&tmp_result_n1._c1_poly, _pgen_rns_idx, degree),
                  Coeffs(&tmp_result_n1._c1_poly, _pgen_rns_idx, degree),
                  Coeffs(&_pgen_tmp_2._c1_poly, _pgen_rns_idx, degree),
                  _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
  }
  for (index_add_n1 = 0; index_add_n1 < 5; index_add_n1 = index_add_n1 + 1) {
    Init_ciph_same_scale(&_pgen_tmp_3, &tmp_result_n1, 0);
    {
      _pgen_swk_c0 = Alloc_poly(degree, Poly_level(&tmp_result_n1._c1_poly), 1);
      _pgen_swk_c1 = Alloc_poly(degree, Poly_level(&tmp_result_n1._c1_poly), 1);
      _pgen_ext    = Alloc_poly(degree, Poly_level(&tmp_result_n1._c1_poly), 1);
      _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
      _pgen_mod_down_c0 =
          Alloc_poly(degree, Poly_level(&tmp_result_n1._c0_poly), 0);
      _pgen_mod_down_c1 =
          Alloc_poly(degree, Poly_level(&tmp_result_n1._c1_poly), 0);
      _pgen_decomp = Alloc_poly(degree, Poly_level(&tmp_result_n1._c0_poly), 0);
      _pgen_swk    = Swk(1, ((1 << index_add_n1) * 3));
      for (_pgen_part_idx = 0;
           _pgen_part_idx < Num_decomp(&tmp_result_n1._c1_poly);
           _pgen_part_idx = _pgen_part_idx + 1) {
        Decomp(_pgen_decomp, &tmp_result_n1._c1_poly, _pgen_part_idx);
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
                    Coeffs(&tmp_result_n1._c0_poly, _pgen_rns_idx, degree),
                    _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
      _pgen_order = Auto_order(((1 << index_add_n1) * 3));
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q   = Poly_level(_pgen_mod_down_c0);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q;
             _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_rotate(Coeffs(&_pgen_tmp_3._c0_poly, _pgen_rns_idx, degree),
                    Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree),
                    _pgen_order, _pgen_modulus, degree);
          Hw_rotate(Coeffs(&_pgen_tmp_3._c1_poly, _pgen_rns_idx, degree),
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
    Init_ciph_same_scale(&tmp_result_n1, &tmp_result_n1, &_pgen_tmp_3);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q   = Poly_level(&tmp_result_n1._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q;
           _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&tmp_result_n1._c0_poly, _pgen_rns_idx, degree),
                  Coeffs(&tmp_result_n1._c0_poly, _pgen_rns_idx, degree),
                  Coeffs(&_pgen_tmp_3._c0_poly, _pgen_rns_idx, degree),
                  _pgen_modulus, degree);
        Hw_modadd(Coeffs(&tmp_result_n1._c1_poly, _pgen_rns_idx, degree),
                  Coeffs(&tmp_result_n1._c1_poly, _pgen_rns_idx, degree),
                  Coeffs(&_pgen_tmp_3._c1_poly, _pgen_rns_idx, degree),
                  _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
  }
  Encode_plain_from_float(&_pgen_tmp_4, _cst_1, 3, Sc_degree(&tmp_result_n1),
                          Level(&tmp_result_n1));
  Init_ciph_same_scale_plain(&tmp_result_n1, &tmp_result_n1, &_pgen_tmp_4);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q   = Poly_level(&tmp_result_n1._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q;
         _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&tmp_result_n1._c0_poly, _pgen_rns_idx, degree),
                Coeffs(&tmp_result_n1._c0_poly, _pgen_rns_idx, degree),
                Coeffs(&_pgen_tmp_4._poly, _pgen_rns_idx, degree),
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
      LIB_ANT, 512, 0, 1, 33, 30, 0, 0, 9, {-51, 0, 1, 2, 3, 6, 12, 24, 48}
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