// external header files
#include "rt_ant/rt_ant.h"

typedef double float64_t;
typedef float float32_t;

extern float32_t _cst_0[2304];
extern float32_t _cst_1[16];
extern int32_t _cst_2[1];
extern float32_t _cst_3[2359296];
extern float32_t _cst_4[16384];
extern int32_t _cst_5[9];
extern float32_t _cst_6[16384];
extern float32_t _cst_7[16384];
CIPHERTEXT Rotate(CIPHERTEXT ciph, int32_t rot_idx);

bool Main_graph() {
  CIPHERTEXT input_0;
  CIPHERTEXT input_dup_n1_1;
  int32_t bs_input_n1_2;
  CIPHERTEXT input_block_n1_3[9];
  CIPHERTEXT result_n2_4;
  int32_t num_grid_n2_5;
  CIPHERTEXT result_grid_n2_6;
  int32_t cap_block_n2_7;
  CIPHERTEXT output_8;
  CIPHERTEXT _ckks_gen_tmp_61_9;
  CIPHERTEXT _ckks_gen_tmp_105_10;
  MODULUS* _pgen_modulus_11;
  uint32_t _pgen_num_q_12;
  uint32_t _pgen_rns_idx_13;
  PLAINTEXT _pgen_tmp_0_14;
  CIPHERTEXT _pgen_tmp_1_15;
  CIPHERTEXT _pgen_tmp_2_16;
  PLAINTEXT _pgen_tmp_3_17;
  CIPHERTEXT _pgen_tmp_4_18;
  PLAINTEXT _pgen_tmp_5_19;
  CIPHERTEXT _pgen_tmp_6_20;
  PLAINTEXT _pgen_tmp_7_21;
  CIPHERTEXT _preg_268435456;
  CIPHERTEXT _preg_268435457;
  CIPHERTEXT _preg_268435458;
  CIPHERTEXT _preg_268435459;
  CIPHERTEXT _preg_268435460;
  uint32_t  degree = Degree();
  input_0 = Get_input_data("input", 0);
  memset(&input_dup_n1_1, 0, sizeof(input_dup_n1_1));
  memset(&input_block_n1_3, 0, sizeof(input_block_n1_3));
  memset(&result_n2_4, 0, sizeof(result_n2_4));
  memset(&result_grid_n2_6, 0, sizeof(result_grid_n2_6));
  memset(&output_8, 0, sizeof(output_8));
  memset(&_ckks_gen_tmp_61_9, 0, sizeof(_ckks_gen_tmp_61_9));
  memset(&_ckks_gen_tmp_105_10, 0, sizeof(_ckks_gen_tmp_105_10));
  memset(&_pgen_tmp_0_14, 0, sizeof(_pgen_tmp_0_14));
  memset(&_pgen_tmp_1_15, 0, sizeof(_pgen_tmp_1_15));
  memset(&_pgen_tmp_2_16, 0, sizeof(_pgen_tmp_2_16));
  memset(&_pgen_tmp_3_17, 0, sizeof(_pgen_tmp_3_17));
  memset(&_pgen_tmp_4_18, 0, sizeof(_pgen_tmp_4_18));
  memset(&_pgen_tmp_5_19, 0, sizeof(_pgen_tmp_5_19));
  memset(&_pgen_tmp_6_20, 0, sizeof(_pgen_tmp_6_20));
  memset(&_pgen_tmp_7_21, 0, sizeof(_pgen_tmp_7_21));
  memset(&_preg_268435456, 0, sizeof(_preg_268435456));
  memset(&_preg_268435457, 0, sizeof(_preg_268435457));
  memset(&_preg_268435458, 0, sizeof(_preg_268435458));
  memset(&_preg_268435459, 0, sizeof(_preg_268435459));
  memset(&_preg_268435460, 0, sizeof(_preg_268435460));
  // /conv/Conv
  // pragma: 65536, 1027, 33
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435456, &input_0);
  Free_poly_data(&input_0._c1_poly);
  Free_poly_data(&input_0._c0_poly);
  // BlockingRot replicate=1
  Copy_ciph(&input_dup_n1_1, &_preg_268435456);
  Free_poly_data(&_preg_268435456._c1_poly);
  Free_poly_data(&_preg_268435456._c0_poly);
  // BlockingRot HRot=bs=9
  for (bs_input_n1_2 = 0; bs_input_n1_2 < 9; bs_input_n1_2 = bs_input_n1_2 + 1) {
    _preg_268435458 = Rotate(input_dup_n1_1, _cst_5[bs_input_n1_2]);
    Copy_ciph(&input_block_n1_3[bs_input_n1_2], &_preg_268435458);
    Free_poly_data(&_preg_268435458._c1_poly);
    Free_poly_data(&_preg_268435458._c0_poly);
  }
  Free_poly_data(&input_dup_n1_1._c1_poly);
  Free_poly_data(&input_dup_n1_1._c0_poly);
  Zero_ciph(&result_n2_4);
  for (num_grid_n2_5 = 0; num_grid_n2_5 < 16; num_grid_n2_5 = num_grid_n2_5 + 1) {
    Zero_ciph(&result_grid_n2_6);
    for (cap_block_n2_7 = 0; cap_block_n2_7 < 9; cap_block_n2_7 = cap_block_n2_7 + 1) {
      _ckks_gen_tmp_61_9 = input_block_n1_3[cap_block_n2_7];
      Pt_from_msg_ofst(&_pgen_tmp_0_14, 0/* cst_3 */, ((cap_block_n2_7 + (num_grid_n2_5 * 9))) * (16384), 16384, 1, 3);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_1_15, &_ckks_gen_tmp_61_9, &_pgen_tmp_0_14);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n2_6, &result_grid_n2_6, &_pgen_tmp_1_15);
      _pgen_modulus_11 = Q_modulus();
      _pgen_num_q_12 = Poly_level(&result_grid_n2_6._c0_poly);
      for (_pgen_rns_idx_13 = 0; _pgen_rns_idx_13 < _pgen_num_q_12; _pgen_rns_idx_13 = _pgen_rns_idx_13 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_1_15._c0_poly, _pgen_rns_idx_13, degree), Coeffs(&_ckks_gen_tmp_61_9._c0_poly, _pgen_rns_idx_13, degree), Coeffs(&_pgen_tmp_0_14._poly, _pgen_rns_idx_13, degree), _pgen_modulus_11, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_1_15._c1_poly, _pgen_rns_idx_13, degree), Coeffs(&_ckks_gen_tmp_61_9._c1_poly, _pgen_rns_idx_13, degree), Coeffs(&_pgen_tmp_0_14._poly, _pgen_rns_idx_13, degree), _pgen_modulus_11, degree);
        Hw_modadd(Coeffs(&result_grid_n2_6._c0_poly, _pgen_rns_idx_13, degree), Coeffs(&result_grid_n2_6._c0_poly, _pgen_rns_idx_13, degree), Coeffs(&_pgen_tmp_1_15._c0_poly, _pgen_rns_idx_13, degree), _pgen_modulus_11, degree);
        Hw_modadd(Coeffs(&result_grid_n2_6._c1_poly, _pgen_rns_idx_13, degree), Coeffs(&result_grid_n2_6._c1_poly, _pgen_rns_idx_13, degree), Coeffs(&_pgen_tmp_1_15._c1_poly, _pgen_rns_idx_13, degree), _pgen_modulus_11, degree);
        _pgen_modulus_11 = _pgen_modulus_11 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435459 = Rotate(result_grid_n2_6, num_grid_n2_5 * 1024);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n2_4, &result_n2_4, &_preg_268435459);
    _pgen_modulus_11 = Q_modulus();
    _pgen_num_q_12 = Poly_level(&result_n2_4._c0_poly);
    for (_pgen_rns_idx_13 = 0; _pgen_rns_idx_13 < _pgen_num_q_12; _pgen_rns_idx_13 = _pgen_rns_idx_13 + 1) {
      Hw_modadd(Coeffs(&result_n2_4._c0_poly, _pgen_rns_idx_13, degree), Coeffs(&result_n2_4._c0_poly, _pgen_rns_idx_13, degree), Coeffs(&_preg_268435459._c0_poly, _pgen_rns_idx_13, degree), _pgen_modulus_11, degree);
      Hw_modadd(Coeffs(&result_n2_4._c1_poly, _pgen_rns_idx_13, degree), Coeffs(&result_n2_4._c1_poly, _pgen_rns_idx_13, degree), Coeffs(&_preg_268435459._c1_poly, _pgen_rns_idx_13, degree), _pgen_modulus_11, degree);
      _pgen_modulus_11 = _pgen_modulus_11 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_poly_data(&_preg_268435459._c1_poly);
    Free_poly_data(&_preg_268435459._c0_poly);
  }
  Free_ciph_poly(input_block_n1_3, 9);
  Free_poly_data(&_pgen_tmp_0_14._poly);
  Free_poly_data(&_pgen_tmp_1_15._c1_poly);
  Free_poly_data(&_pgen_tmp_1_15._c0_poly);
  Free_poly_data(&result_grid_n2_6._c1_poly);
  Free_poly_data(&result_grid_n2_6._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n2_4, &result_n2_4);
  Rescale(&result_n2_4._c0_poly, &result_n2_4._c0_poly);
  Rescale(&result_n2_4._c1_poly, &result_n2_4._c1_poly);
  CMPLR_TM_END(19, 0);
  Pt_from_msg(&_pgen_tmp_3_17, 1 /* cst_6 */, 16384, 1, 2);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_2_16, &result_n2_4, &_pgen_tmp_3_17);
  _pgen_modulus_11 = Q_modulus();
  _pgen_num_q_12 = Poly_level(&_pgen_tmp_2_16._c0_poly);
  for (_pgen_rns_idx_13 = 0; _pgen_rns_idx_13 < _pgen_num_q_12; _pgen_rns_idx_13 = _pgen_rns_idx_13 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_2_16._c0_poly, _pgen_rns_idx_13, degree), Coeffs(&result_n2_4._c0_poly, _pgen_rns_idx_13, degree), Coeffs(&_pgen_tmp_3_17._poly, _pgen_rns_idx_13, degree), _pgen_modulus_11, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_2_16._c1_poly, _pgen_rns_idx_13, degree), Coeffs(&result_n2_4._c1_poly, _pgen_rns_idx_13, degree), Coeffs(&_pgen_tmp_3_17._poly, _pgen_rns_idx_13, degree), _pgen_modulus_11, degree);
    _pgen_modulus_11 = _pgen_modulus_11 + 1;
  }
  Free_poly_data(&_pgen_tmp_3_17._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435457, &_pgen_tmp_2_16);
  Rescale(&_preg_268435457._c0_poly, &_pgen_tmp_2_16._c0_poly);
  Rescale(&_preg_268435457._c1_poly, &_pgen_tmp_2_16._c1_poly);
  Free_poly_data(&_pgen_tmp_2_16._c1_poly);
  Free_poly_data(&_pgen_tmp_2_16._c0_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435460 = Rotate(result_n2_4, -16384);
  Copy_ciph(&_ckks_gen_tmp_105_10, &_preg_268435460);
  Free_poly_data(&_preg_268435460._c1_poly);
  Free_poly_data(&_preg_268435460._c0_poly);
  Pt_from_msg(&_pgen_tmp_5_19, 2 /* cst_7 */, 16384, 1, 2);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_4_18, &_ckks_gen_tmp_105_10, &_pgen_tmp_5_19);
  _pgen_modulus_11 = Q_modulus();
  _pgen_num_q_12 = Poly_level(&_pgen_tmp_4_18._c0_poly);
  for (_pgen_rns_idx_13 = 0; _pgen_rns_idx_13 < _pgen_num_q_12; _pgen_rns_idx_13 = _pgen_rns_idx_13 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_4_18._c0_poly, _pgen_rns_idx_13, degree), Coeffs(&_ckks_gen_tmp_105_10._c0_poly, _pgen_rns_idx_13, degree), Coeffs(&_pgen_tmp_5_19._poly, _pgen_rns_idx_13, degree), _pgen_modulus_11, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_4_18._c1_poly, _pgen_rns_idx_13, degree), Coeffs(&_ckks_gen_tmp_105_10._c1_poly, _pgen_rns_idx_13, degree), Coeffs(&_pgen_tmp_5_19._poly, _pgen_rns_idx_13, degree), _pgen_modulus_11, degree);
    _pgen_modulus_11 = _pgen_modulus_11 + 1;
  }
  Free_poly_data(&_pgen_tmp_5_19._poly);
  Free_poly_data(&_ckks_gen_tmp_105_10._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_105_10._c0_poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_6_20, &_pgen_tmp_4_18);
  Rescale(&_pgen_tmp_6_20._c0_poly, &_pgen_tmp_4_18._c0_poly);
  Rescale(&_pgen_tmp_6_20._c1_poly, &_pgen_tmp_4_18._c1_poly);
  Free_poly_data(&_pgen_tmp_4_18._c1_poly);
  Free_poly_data(&_pgen_tmp_4_18._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_preg_268435457, &_preg_268435457, &_pgen_tmp_6_20);
  _pgen_modulus_11 = Q_modulus();
  _pgen_num_q_12 = Poly_level(&_preg_268435457._c0_poly);
  for (_pgen_rns_idx_13 = 0; _pgen_rns_idx_13 < _pgen_num_q_12; _pgen_rns_idx_13 = _pgen_rns_idx_13 + 1) {
    Hw_modadd(Coeffs(&_preg_268435457._c0_poly, _pgen_rns_idx_13, degree), Coeffs(&_preg_268435457._c0_poly, _pgen_rns_idx_13, degree), Coeffs(&_pgen_tmp_6_20._c0_poly, _pgen_rns_idx_13, degree), _pgen_modulus_11, degree);
    Hw_modadd(Coeffs(&_preg_268435457._c1_poly, _pgen_rns_idx_13, degree), Coeffs(&_preg_268435457._c1_poly, _pgen_rns_idx_13, degree), Coeffs(&_pgen_tmp_6_20._c1_poly, _pgen_rns_idx_13, degree), _pgen_modulus_11, degree);
    _pgen_modulus_11 = _pgen_modulus_11 + 1;
  }
  Free_poly_data(&_pgen_tmp_6_20._c1_poly);
  Free_poly_data(&_pgen_tmp_6_20._c0_poly);
  CMPLR_TM_END(12, 0);
  Pt_from_msg(&_pgen_tmp_7_21, 3 /* cst_4 */, 16384, 1, 1);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n2_4, &_preg_268435457, &_pgen_tmp_7_21);
  _pgen_modulus_11 = Q_modulus();
  _pgen_num_q_12 = Poly_level(&result_n2_4._c0_poly);
  for (_pgen_rns_idx_13 = 0; _pgen_rns_idx_13 < _pgen_num_q_12; _pgen_rns_idx_13 = _pgen_rns_idx_13 + 1) {
    Hw_modadd(Coeffs(&result_n2_4._c0_poly, _pgen_rns_idx_13, degree), Coeffs(&_preg_268435457._c0_poly, _pgen_rns_idx_13, degree), Coeffs(&_pgen_tmp_7_21._poly, _pgen_rns_idx_13, degree), _pgen_modulus_11, degree);
    Set_coeffs(&result_n2_4._c1_poly, _pgen_rns_idx_13, degree, Coeffs(&_preg_268435457._c1_poly, _pgen_rns_idx_13, degree));
    _pgen_modulus_11 = _pgen_modulus_11 + 1;
  }
  Free_poly_data(&_pgen_tmp_7_21._poly);
  Free_poly_data(&_preg_268435457._c1_poly);
  Free_poly_data(&_preg_268435457._c0_poly);
  CMPLR_TM_END(13, 0);
  Copy_ciph(&output_8, &result_n2_4);
  Free_poly_data(&result_n2_4._c1_poly);
  Free_poly_data(&result_n2_4._c0_poly);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 33
  Set_output_data("output", 0, &output_8);
  return true;
}

int Get_input_count() {
  return 1;
}

DATA_SCHEME* Get_encode_scheme(int idx) {
  static MAP_DESC desc_0[] = {
    {NORMAL, 0, 0, 0, 0}
  };
  static DATA_SCHEME scheme_0 = {
    "input", {0, 0, 0, 0}, 1, desc_0
  };
  static DATA_SCHEME* scheme[] = { &scheme_0 };
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

CIPHERTEXT Rotate(CIPHERTEXT ciph_0, int32_t rot_idx_1) {
  RTLIB_TM_START(20, rtm);
  CIPHERTEXT _pgen_rot_res_2;
  POLYNOMIAL _pgen_swk_c0_3;
  POLYNOMIAL _pgen_swk_c1_4;
  POLYNOMIAL _pgen_ext_5;
  POLYNOMIAL _pgen_tmp_poly_6;
  POLYNOMIAL _pgen_mod_down_c0_7;
  POLYNOMIAL _pgen_mod_down_c1_8;
  SWITCH_KEY* _pgen_swk_9;
  uint32_t _pgen_part_idx_10;
  POLYNOMIAL _pgen_key0_11;
  POLYNOMIAL _pgen_key1_12;
  MODULUS* _pgen_modulus_13;
  uint32_t _pgen_num_q_14;
  uint32_t _pgen_rns_idx_15;
  uint32_t _pgen_num_p_16;
  uint32_t _pgen_p_ofst_17;
  uint32_t _pgen_p_idx_18;
  uint32_t _pgen_key_p_ofst_19;
  uint32_t _pgen_key_p_idx_20;
  int64_t* _pgen_order_21;
  uint32_t  degree = Degree();
  memset(&_pgen_rot_res_2, 0, sizeof(_pgen_rot_res_2));
  memset(&_pgen_swk_c0_3, 0, sizeof(_pgen_swk_c0_3));
  memset(&_pgen_swk_c1_4, 0, sizeof(_pgen_swk_c1_4));
  memset(&_pgen_ext_5, 0, sizeof(_pgen_ext_5));
  memset(&_pgen_tmp_poly_6, 0, sizeof(_pgen_tmp_poly_6));
  memset(&_pgen_mod_down_c0_7, 0, sizeof(_pgen_mod_down_c0_7));
  memset(&_pgen_mod_down_c1_8, 0, sizeof(_pgen_mod_down_c1_8));
  memset(&_pgen_key0_11, 0, sizeof(_pgen_key0_11));
  memset(&_pgen_key1_12, 0, sizeof(_pgen_key1_12));
  Init_ciph_same_scale(&_pgen_rot_res_2, &ciph_0, 0);
  Init_poly_by_size(&_pgen_swk_c0_3, Poly_level(&ciph_0._c1_poly), 1);
  Init_poly_by_size(&_pgen_swk_c1_4, Poly_level(&ciph_0._c1_poly), 1);
  Init_poly_by_size(&_pgen_ext_5, Poly_level(&ciph_0._c1_poly), 1);
  Init_poly_by_size(&_pgen_tmp_poly_6, 1, 0);
  Init_poly_by_size(&_pgen_mod_down_c0_7, Poly_level(&ciph_0._c0_poly), 0);
  Init_poly_by_size(&_pgen_mod_down_c1_8, Poly_level(&ciph_0._c1_poly), 0);
  _pgen_swk_9 = Swk(1, rot_idx_1);
  for (_pgen_part_idx_10 = 0; _pgen_part_idx_10 < Num_decomp(&ciph_0._c1_poly); _pgen_part_idx_10 = _pgen_part_idx_10 + 1) {
    Decomp_modup(&_pgen_ext_5, &ciph_0._c1_poly, _pgen_part_idx_10);
    Set_pk0(&_pgen_key0_11, _pgen_swk_9, _pgen_part_idx_10);
    Set_pk1(&_pgen_key1_12, _pgen_swk_9, _pgen_part_idx_10);
    _pgen_modulus_13 = Q_modulus();
    _pgen_num_q_14 = Poly_level(&_pgen_ext_5);
    for (_pgen_rns_idx_15 = 0; _pgen_rns_idx_15 < _pgen_num_q_14; _pgen_rns_idx_15 = _pgen_rns_idx_15 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_poly_6, 0, degree), Coeffs(&_pgen_key0_11, _pgen_rns_idx_15, degree), Coeffs(&_pgen_ext_5, _pgen_rns_idx_15, degree), _pgen_modulus_13, degree);
      Hw_modadd(Coeffs(&_pgen_swk_c0_3, _pgen_rns_idx_15, degree), Coeffs(&_pgen_swk_c0_3, _pgen_rns_idx_15, degree), Coeffs(&_pgen_tmp_poly_6, 0, degree), _pgen_modulus_13, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_poly_6, 0, degree), Coeffs(&_pgen_key1_12, _pgen_rns_idx_15, degree), Coeffs(&_pgen_ext_5, _pgen_rns_idx_15, degree), _pgen_modulus_13, degree);
      Hw_modadd(Coeffs(&_pgen_swk_c1_4, _pgen_rns_idx_15, degree), Coeffs(&_pgen_swk_c1_4, _pgen_rns_idx_15, degree), Coeffs(&_pgen_tmp_poly_6, 0, degree), _pgen_modulus_13, degree);
      _pgen_modulus_13 = _pgen_modulus_13 + 1;
    }
    _pgen_modulus_13 = P_modulus();
    _pgen_num_p_16 = Num_p(&_pgen_ext_5);
    _pgen_p_ofst_17 = Num_alloc(&_pgen_ext_5) - _pgen_num_p_16;
    _pgen_key_p_ofst_19 = Poly_level(&_pgen_key0_11);
    for (_pgen_rns_idx_15 = 0; _pgen_rns_idx_15 < _pgen_num_p_16; _pgen_rns_idx_15 = _pgen_rns_idx_15 + 1) {
      _pgen_p_idx_18 = _pgen_rns_idx_15 + _pgen_p_ofst_17;
      _pgen_key_p_idx_20 = _pgen_rns_idx_15 + _pgen_key_p_ofst_19;
      Hw_modmul(Coeffs(&_pgen_tmp_poly_6, 0, degree), Coeffs(&_pgen_key0_11, _pgen_key_p_idx_20, degree), Coeffs(&_pgen_ext_5, _pgen_p_idx_18, degree), _pgen_modulus_13, degree);
      Hw_modadd(Coeffs(&_pgen_swk_c0_3, _pgen_p_idx_18, degree), Coeffs(&_pgen_swk_c0_3, _pgen_p_idx_18, degree), Coeffs(&_pgen_tmp_poly_6, 0, degree), _pgen_modulus_13, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_poly_6, 0, degree), Coeffs(&_pgen_key1_12, _pgen_key_p_idx_20, degree), Coeffs(&_pgen_ext_5, _pgen_p_idx_18, degree), _pgen_modulus_13, degree);
      Hw_modadd(Coeffs(&_pgen_swk_c1_4, _pgen_p_idx_18, degree), Coeffs(&_pgen_swk_c1_4, _pgen_p_idx_18, degree), Coeffs(&_pgen_tmp_poly_6, 0, degree), _pgen_modulus_13, degree);
      _pgen_modulus_13 = _pgen_modulus_13 + 1;
    }
  }
  Mod_down(&_pgen_mod_down_c0_7, &_pgen_swk_c0_3);
  Mod_down(&_pgen_mod_down_c1_8, &_pgen_swk_c1_4);
  _pgen_modulus_13 = Q_modulus();
  _pgen_num_q_14 = Poly_level(&_pgen_mod_down_c0_7);
  for (_pgen_rns_idx_15 = 0; _pgen_rns_idx_15 < _pgen_num_q_14; _pgen_rns_idx_15 = _pgen_rns_idx_15 + 1) {
    Hw_modadd(Coeffs(&_pgen_mod_down_c0_7, _pgen_rns_idx_15, degree), Coeffs(&_pgen_mod_down_c0_7, _pgen_rns_idx_15, degree), Coeffs(&ciph_0._c0_poly, _pgen_rns_idx_15, degree), _pgen_modulus_13, degree);
    _pgen_modulus_13 = _pgen_modulus_13 + 1;
  }
  _pgen_order_21 = Auto_order(rot_idx_1);
  _pgen_modulus_13 = Q_modulus();
  _pgen_num_q_14 = Poly_level(&_pgen_mod_down_c0_7);
  for (_pgen_rns_idx_15 = 0; _pgen_rns_idx_15 < _pgen_num_q_14; _pgen_rns_idx_15 = _pgen_rns_idx_15 + 1) {
    Hw_rotate(Coeffs(&_pgen_rot_res_2._c0_poly, _pgen_rns_idx_15, degree), Coeffs(&_pgen_mod_down_c0_7, _pgen_rns_idx_15, degree), _pgen_order_21, _pgen_modulus_13, degree);
    Hw_rotate(Coeffs(&_pgen_rot_res_2._c1_poly, _pgen_rns_idx_15, degree), Coeffs(&_pgen_mod_down_c1_8, _pgen_rns_idx_15, degree), _pgen_order_21, _pgen_modulus_13, degree);
    _pgen_modulus_13 = _pgen_modulus_13 + 1;
  }
  Free_poly_data(&_pgen_swk_c0_3);
  Free_poly_data(&_pgen_swk_c1_4);
  Free_poly_data(&_pgen_ext_5);
  Free_poly_data(&_pgen_tmp_poly_6);
  Free_poly_data(&_pgen_mod_down_c0_7);
  Free_poly_data(&_pgen_mod_down_c1_8);
  RTLIB_TM_END(20, rtm);
  return _pgen_rot_res_2;
}

CKKS_PARAMS* Get_context_params() {
  static CKKS_PARAMS parm = {
    LIB_ANT, 65536, 0, 2, 3, 60, 56, 2, 192, 25, 
    { -16384, -33, -32, -31, -1, 0, 1, 31,
      32, 33, 1024, 2048, 3072, 4096, 5120, 6144,
      7168, 8192, 9216, 10240, 11264, 12288, 13312, 14336,
      15360 }
  };
  return &parm;
}

RT_DATA_INFO* Get_rt_data_info() {
  static RT_DATA_INFO info = {
    "conv_16x16x32x3.weight",
    "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX",
    DE_MSG_F32
  };
  return &info;
}

int32_t _cst_5[9] = {
  -33, -32, -31, -1, 0, 1, 31, 32,
  33
};
