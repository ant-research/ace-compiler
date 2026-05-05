// external header files
#include "rt_ant/rt_ant.h"

typedef double float64_t;
typedef float float32_t;

extern float32_t _cst_0[2359296];
extern float32_t _cst_1[512];
extern int32_t _cst_2[1];
extern float32_t _cst_3[115605504];
extern float32_t _cst_4[25088];
extern int32_t _cst_5[9];
extern float32_t _cst_6[12845056];
extern float32_t _cst_7[12845056];
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
  MODULUS* _pgen_modulus_10;
  uint32_t _pgen_num_q_11;
  uint32_t _pgen_rns_idx_12;
  PLAINTEXT _pgen_tmp_0_13;
  CIPHERTEXT _pgen_tmp_1_14;
  CIPHERTEXT _pgen_tmp_2_15;
  CIPHERTEXT _pgen_tmp_3_16;
  PLAINTEXT _pgen_tmp_4_17;
  CIPHERTEXT _pgen_tmp_5_18;
  CIPHERTEXT _pgen_tmp_6_19;
  PLAINTEXT _pgen_tmp_7_20;
  CIPHERTEXT _pgen_tmp_8_21;
  PLAINTEXT _pgen_tmp_9_22;
  CIPHERTEXT _preg_268435456;
  CIPHERTEXT _preg_268435457;
  CIPHERTEXT _preg_268435458;
  CIPHERTEXT _preg_268435459;
  uint32_t  degree = Degree();
  input_0 = Get_input_data("input", 0);
  memset(&input_dup_n1_1, 0, sizeof(input_dup_n1_1));
  memset(&input_block_n1_3, 0, sizeof(input_block_n1_3));
  memset(&result_n2_4, 0, sizeof(result_n2_4));
  memset(&result_grid_n2_6, 0, sizeof(result_grid_n2_6));
  memset(&output_8, 0, sizeof(output_8));
  memset(&_ckks_gen_tmp_61_9, 0, sizeof(_ckks_gen_tmp_61_9));
  memset(&_pgen_tmp_0_13, 0, sizeof(_pgen_tmp_0_13));
  memset(&_pgen_tmp_1_14, 0, sizeof(_pgen_tmp_1_14));
  memset(&_pgen_tmp_2_15, 0, sizeof(_pgen_tmp_2_15));
  memset(&_pgen_tmp_3_16, 0, sizeof(_pgen_tmp_3_16));
  memset(&_pgen_tmp_4_17, 0, sizeof(_pgen_tmp_4_17));
  memset(&_pgen_tmp_5_18, 0, sizeof(_pgen_tmp_5_18));
  memset(&_pgen_tmp_6_19, 0, sizeof(_pgen_tmp_6_19));
  memset(&_pgen_tmp_7_20, 0, sizeof(_pgen_tmp_7_20));
  memset(&_pgen_tmp_8_21, 0, sizeof(_pgen_tmp_8_21));
  memset(&_pgen_tmp_9_22, 0, sizeof(_pgen_tmp_9_22));
  memset(&_preg_268435456, 0, sizeof(_preg_268435456));
  memset(&_preg_268435457, 0, sizeof(_preg_268435457));
  memset(&_preg_268435458, 0, sizeof(_preg_268435458));
  memset(&_preg_268435459, 0, sizeof(_preg_268435459));
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
    _preg_268435457 = Rotate(input_dup_n1_1, _cst_5[bs_input_n1_2]);
    Copy_ciph(&input_block_n1_3[bs_input_n1_2], &_preg_268435457);
    Free_poly_data(&_preg_268435457._c1_poly);
    Free_poly_data(&_preg_268435457._c0_poly);
  }
  Free_poly_data(&input_dup_n1_1._c1_poly);
  Free_poly_data(&input_dup_n1_1._c0_poly);
  Zero_ciph(&result_n2_4);
  for (num_grid_n2_5 = 0; num_grid_n2_5 < 512; num_grid_n2_5 = num_grid_n2_5 + 1) {
    Zero_ciph(&result_grid_n2_6);
    for (cap_block_n2_7 = 0; cap_block_n2_7 < 9; cap_block_n2_7 = cap_block_n2_7 + 1) {
      _ckks_gen_tmp_61_9 = input_block_n1_3[cap_block_n2_7];
      Pt_from_msg_ofst(&_pgen_tmp_0_13, 0/* cst_3 */, ((cap_block_n2_7 + (num_grid_n2_5 * 9))) * (25088), 25088, 1, 3);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_1_14, &_ckks_gen_tmp_61_9, &_pgen_tmp_0_13);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n2_6, &result_grid_n2_6, &_pgen_tmp_1_14);
      _pgen_modulus_10 = Q_modulus();
      _pgen_num_q_11 = Poly_level(&result_grid_n2_6._c0_poly);
      for (_pgen_rns_idx_12 = 0; _pgen_rns_idx_12 < _pgen_num_q_11; _pgen_rns_idx_12 = _pgen_rns_idx_12 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_1_14._c0_poly, _pgen_rns_idx_12, degree), Coeffs(&_ckks_gen_tmp_61_9._c0_poly, _pgen_rns_idx_12, degree), Coeffs(&_pgen_tmp_0_13._poly, _pgen_rns_idx_12, degree), _pgen_modulus_10, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_1_14._c1_poly, _pgen_rns_idx_12, degree), Coeffs(&_ckks_gen_tmp_61_9._c1_poly, _pgen_rns_idx_12, degree), Coeffs(&_pgen_tmp_0_13._poly, _pgen_rns_idx_12, degree), _pgen_modulus_10, degree);
        Hw_modadd(Coeffs(&result_grid_n2_6._c0_poly, _pgen_rns_idx_12, degree), Coeffs(&result_grid_n2_6._c0_poly, _pgen_rns_idx_12, degree), Coeffs(&_pgen_tmp_1_14._c0_poly, _pgen_rns_idx_12, degree), _pgen_modulus_10, degree);
        Hw_modadd(Coeffs(&result_grid_n2_6._c1_poly, _pgen_rns_idx_12, degree), Coeffs(&result_grid_n2_6._c1_poly, _pgen_rns_idx_12, degree), Coeffs(&_pgen_tmp_1_14._c1_poly, _pgen_rns_idx_12, degree), _pgen_modulus_10, degree);
        _pgen_modulus_10 = _pgen_modulus_10 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    CMPLR_TM_START(19, 0);
    Init_ciph_down_scale(&_pgen_tmp_3_16, &result_grid_n2_6);
    Rescale(&_pgen_tmp_3_16._c0_poly, &result_grid_n2_6._c0_poly);
    Rescale(&_pgen_tmp_3_16._c1_poly, &result_grid_n2_6._c1_poly);
    CMPLR_TM_END(19, 0);
    Pt_from_msg_ofst(&_pgen_tmp_4_17, 1/* cst_6 */, (num_grid_n2_5) * (25088), 25088, 1, 2);
    CMPLR_TM_START(17, 0);
    Init_ciph_up_scale_plain(&_pgen_tmp_2_15, &_pgen_tmp_3_16, &_pgen_tmp_4_17);
    _pgen_modulus_10 = Q_modulus();
    _pgen_num_q_11 = Poly_level(&_pgen_tmp_2_15._c0_poly);
    for (_pgen_rns_idx_12 = 0; _pgen_rns_idx_12 < _pgen_num_q_11; _pgen_rns_idx_12 = _pgen_rns_idx_12 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_2_15._c0_poly, _pgen_rns_idx_12, degree), Coeffs(&_pgen_tmp_3_16._c0_poly, _pgen_rns_idx_12, degree), Coeffs(&_pgen_tmp_4_17._poly, _pgen_rns_idx_12, degree), _pgen_modulus_10, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_2_15._c1_poly, _pgen_rns_idx_12, degree), Coeffs(&_pgen_tmp_3_16._c1_poly, _pgen_rns_idx_12, degree), Coeffs(&_pgen_tmp_4_17._poly, _pgen_rns_idx_12, degree), _pgen_modulus_10, degree);
      _pgen_modulus_10 = _pgen_modulus_10 + 1;
    }
    CMPLR_TM_END(17, 0);
    _preg_268435458 = Rotate(_pgen_tmp_2_15, -25088 + (num_grid_n2_5 * 49));
    CMPLR_TM_START(19, 0);
    Init_ciph_down_scale(&_pgen_tmp_6_19, &result_grid_n2_6);
    Rescale(&_pgen_tmp_6_19._c0_poly, &result_grid_n2_6._c0_poly);
    Rescale(&_pgen_tmp_6_19._c1_poly, &result_grid_n2_6._c1_poly);
    CMPLR_TM_END(19, 0);
    Pt_from_msg_ofst(&_pgen_tmp_7_20, 2/* cst_7 */, (num_grid_n2_5) * (25088), 25088, 1, 2);
    CMPLR_TM_START(17, 0);
    Init_ciph_up_scale_plain(&_pgen_tmp_5_18, &_pgen_tmp_6_19, &_pgen_tmp_7_20);
    _pgen_modulus_10 = Q_modulus();
    _pgen_num_q_11 = Poly_level(&_pgen_tmp_5_18._c0_poly);
    for (_pgen_rns_idx_12 = 0; _pgen_rns_idx_12 < _pgen_num_q_11; _pgen_rns_idx_12 = _pgen_rns_idx_12 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_5_18._c0_poly, _pgen_rns_idx_12, degree), Coeffs(&_pgen_tmp_6_19._c0_poly, _pgen_rns_idx_12, degree), Coeffs(&_pgen_tmp_7_20._poly, _pgen_rns_idx_12, degree), _pgen_modulus_10, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_5_18._c1_poly, _pgen_rns_idx_12, degree), Coeffs(&_pgen_tmp_6_19._c1_poly, _pgen_rns_idx_12, degree), Coeffs(&_pgen_tmp_7_20._poly, _pgen_rns_idx_12, degree), _pgen_modulus_10, degree);
      _pgen_modulus_10 = _pgen_modulus_10 + 1;
    }
    CMPLR_TM_END(17, 0);
    _preg_268435459 = Rotate(_pgen_tmp_5_18, num_grid_n2_5 * 49);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&_pgen_tmp_8_21, &_preg_268435458, &_preg_268435459);
    CMPLR_TM_END(12, 0);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n2_4, &result_n2_4, &_pgen_tmp_8_21);
    _pgen_modulus_10 = Q_modulus();
    _pgen_num_q_11 = Poly_level(&result_n2_4._c0_poly);
    for (_pgen_rns_idx_12 = 0; _pgen_rns_idx_12 < _pgen_num_q_11; _pgen_rns_idx_12 = _pgen_rns_idx_12 + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_8_21._c0_poly, _pgen_rns_idx_12, degree), Coeffs(&_preg_268435458._c0_poly, _pgen_rns_idx_12, degree), Coeffs(&_preg_268435459._c0_poly, _pgen_rns_idx_12, degree), _pgen_modulus_10, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_8_21._c1_poly, _pgen_rns_idx_12, degree), Coeffs(&_preg_268435458._c1_poly, _pgen_rns_idx_12, degree), Coeffs(&_preg_268435459._c1_poly, _pgen_rns_idx_12, degree), _pgen_modulus_10, degree);
      Hw_modadd(Coeffs(&result_n2_4._c0_poly, _pgen_rns_idx_12, degree), Coeffs(&result_n2_4._c0_poly, _pgen_rns_idx_12, degree), Coeffs(&_pgen_tmp_8_21._c0_poly, _pgen_rns_idx_12, degree), _pgen_modulus_10, degree);
      Hw_modadd(Coeffs(&result_n2_4._c1_poly, _pgen_rns_idx_12, degree), Coeffs(&result_n2_4._c1_poly, _pgen_rns_idx_12, degree), Coeffs(&_pgen_tmp_8_21._c1_poly, _pgen_rns_idx_12, degree), _pgen_modulus_10, degree);
      _pgen_modulus_10 = _pgen_modulus_10 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_poly_data(&_preg_268435458._c1_poly);
    Free_poly_data(&_preg_268435458._c0_poly);
    Free_poly_data(&_preg_268435459._c1_poly);
    Free_poly_data(&_preg_268435459._c0_poly);
  }
  Free_ciph_poly(input_block_n1_3, 9);
  Free_poly_data(&_pgen_tmp_0_13._poly);
  Free_poly_data(&_pgen_tmp_1_14._c1_poly);
  Free_poly_data(&_pgen_tmp_1_14._c0_poly);
  Free_poly_data(&_pgen_tmp_4_17._poly);
  Free_poly_data(&_pgen_tmp_3_16._c1_poly);
  Free_poly_data(&_pgen_tmp_3_16._c0_poly);
  Free_poly_data(&_pgen_tmp_2_15._c1_poly);
  Free_poly_data(&_pgen_tmp_2_15._c0_poly);
  Free_poly_data(&result_grid_n2_6._c1_poly);
  Free_poly_data(&result_grid_n2_6._c0_poly);
  Free_poly_data(&_pgen_tmp_7_20._poly);
  Free_poly_data(&_pgen_tmp_6_19._c1_poly);
  Free_poly_data(&_pgen_tmp_6_19._c0_poly);
  Free_poly_data(&_pgen_tmp_5_18._c1_poly);
  Free_poly_data(&_pgen_tmp_5_18._c0_poly);
  Free_poly_data(&_pgen_tmp_8_21._c1_poly);
  Free_poly_data(&_pgen_tmp_8_21._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n2_4, &result_n2_4);
  Rescale(&result_n2_4._c0_poly, &result_n2_4._c0_poly);
  Rescale(&result_n2_4._c1_poly, &result_n2_4._c1_poly);
  CMPLR_TM_END(19, 0);
  Pt_from_msg(&_pgen_tmp_9_22, 3 /* cst_4 */, 25088, 1, 1);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n2_4, &result_n2_4, &_pgen_tmp_9_22);
  _pgen_modulus_10 = Q_modulus();
  _pgen_num_q_11 = Poly_level(&result_n2_4._c0_poly);
  for (_pgen_rns_idx_12 = 0; _pgen_rns_idx_12 < _pgen_num_q_11; _pgen_rns_idx_12 = _pgen_rns_idx_12 + 1) {
    Hw_modadd(Coeffs(&result_n2_4._c0_poly, _pgen_rns_idx_12, degree), Coeffs(&result_n2_4._c0_poly, _pgen_rns_idx_12, degree), Coeffs(&_pgen_tmp_9_22._poly, _pgen_rns_idx_12, degree), _pgen_modulus_10, degree);
    Set_coeffs(&result_n2_4._c1_poly, _pgen_rns_idx_12, degree, Coeffs(&result_n2_4._c1_poly, _pgen_rns_idx_12, degree));
    _pgen_modulus_10 = _pgen_modulus_10 + 1;
  }
  Free_poly_data(&_pgen_tmp_9_22._poly);
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
    LIB_ANT, 65536, 0, 2, 3, 60, 56, 2, 192, 1032, 
    { -25088, -25039, -24990, -24941, -24892, -24843, -24794, -24745,
      -24696, -24647, -24598, -24549, -24500, -24451, -24402, -24353,
      -24304, -24255, -24206, -24157, -24108, -24059, -24010, -23961,
      -23912, -23863, -23814, -23765, -23716, -23667, -23618, -23569,
      -23520, -23471, -23422, -23373, -23324, -23275, -23226, -23177,
      -23128, -23079, -23030, -22981, -22932, -22883, -22834, -22785,
      -22736, -22687, -22638, -22589, -22540, -22491, -22442, -22393,
      -22344, -22295, -22246, -22197, -22148, -22099, -22050, -22001,
      -21952, -21903, -21854, -21805, -21756, -21707, -21658, -21609,
      -21560, -21511, -21462, -21413, -21364, -21315, -21266, -21217,
      -21168, -21119, -21070, -21021, -20972, -20923, -20874, -20825,
      -20776, -20727, -20678, -20629, -20580, -20531, -20482, -20433,
      -20384, -20335, -20286, -20237, -20188, -20139, -20090, -20041,
      -19992, -19943, -19894, -19845, -19796, -19747, -19698, -19649,
      -19600, -19551, -19502, -19453, -19404, -19355, -19306, -19257,
      -19208, -19159, -19110, -19061, -19012, -18963, -18914, -18865,
      -18816, -18767, -18718, -18669, -18620, -18571, -18522, -18473,
      -18424, -18375, -18326, -18277, -18228, -18179, -18130, -18081,
      -18032, -17983, -17934, -17885, -17836, -17787, -17738, -17689,
      -17640, -17591, -17542, -17493, -17444, -17395, -17346, -17297,
      -17248, -17199, -17150, -17101, -17052, -17003, -16954, -16905,
      -16856, -16807, -16758, -16709, -16660, -16611, -16562, -16513,
      -16464, -16415, -16366, -16317, -16268, -16219, -16170, -16121,
      -16072, -16023, -15974, -15925, -15876, -15827, -15778, -15729,
      -15680, -15631, -15582, -15533, -15484, -15435, -15386, -15337,
      -15288, -15239, -15190, -15141, -15092, -15043, -14994, -14945,
      -14896, -14847, -14798, -14749, -14700, -14651, -14602, -14553,
      -14504, -14455, -14406, -14357, -14308, -14259, -14210, -14161,
      -14112, -14063, -14014, -13965, -13916, -13867, -13818, -13769,
      -13720, -13671, -13622, -13573, -13524, -13475, -13426, -13377,
      -13328, -13279, -13230, -13181, -13132, -13083, -13034, -12985,
      -12936, -12887, -12838, -12789, -12740, -12691, -12642, -12593,
      -12544, -12495, -12446, -12397, -12348, -12299, -12250, -12201,
      -12152, -12103, -12054, -12005, -11956, -11907, -11858, -11809,
      -11760, -11711, -11662, -11613, -11564, -11515, -11466, -11417,
      -11368, -11319, -11270, -11221, -11172, -11123, -11074, -11025,
      -10976, -10927, -10878, -10829, -10780, -10731, -10682, -10633,
      -10584, -10535, -10486, -10437, -10388, -10339, -10290, -10241,
      -10192, -10143, -10094, -10045, -9996, -9947, -9898, -9849,
      -9800, -9751, -9702, -9653, -9604, -9555, -9506, -9457,
      -9408, -9359, -9310, -9261, -9212, -9163, -9114, -9065,
      -9016, -8967, -8918, -8869, -8820, -8771, -8722, -8673,
      -8624, -8575, -8526, -8477, -8428, -8379, -8330, -8281,
      -8232, -8183, -8134, -8085, -8036, -7987, -7938, -7889,
      -7840, -7791, -7742, -7693, -7644, -7595, -7546, -7497,
      -7448, -7399, -7350, -7301, -7252, -7203, -7154, -7105,
      -7056, -7007, -6958, -6909, -6860, -6811, -6762, -6713,
      -6664, -6615, -6566, -6517, -6468, -6419, -6370, -6321,
      -6272, -6223, -6174, -6125, -6076, -6027, -5978, -5929,
      -5880, -5831, -5782, -5733, -5684, -5635, -5586, -5537,
      -5488, -5439, -5390, -5341, -5292, -5243, -5194, -5145,
      -5096, -5047, -4998, -4949, -4900, -4851, -4802, -4753,
      -4704, -4655, -4606, -4557, -4508, -4459, -4410, -4361,
      -4312, -4263, -4214, -4165, -4116, -4067, -4018, -3969,
      -3920, -3871, -3822, -3773, -3724, -3675, -3626, -3577,
      -3528, -3479, -3430, -3381, -3332, -3283, -3234, -3185,
      -3136, -3087, -3038, -2989, -2940, -2891, -2842, -2793,
      -2744, -2695, -2646, -2597, -2548, -2499, -2450, -2401,
      -2352, -2303, -2254, -2205, -2156, -2107, -2058, -2009,
      -1960, -1911, -1862, -1813, -1764, -1715, -1666, -1617,
      -1568, -1519, -1470, -1421, -1372, -1323, -1274, -1225,
      -1176, -1127, -1078, -1029, -980, -931, -882, -833,
      -784, -735, -686, -637, -588, -539, -490, -441,
      -392, -343, -294, -245, -196, -147, -98, -49,
      -8, -7, -6, -1, 0, 1, 6, 7,
      8, 49, 98, 147, 196, 245, 294, 343,
      392, 441, 490, 539, 588, 637, 686, 735,
      784, 833, 882, 931, 980, 1029, 1078, 1127,
      1176, 1225, 1274, 1323, 1372, 1421, 1470, 1519,
      1568, 1617, 1666, 1715, 1764, 1813, 1862, 1911,
      1960, 2009, 2058, 2107, 2156, 2205, 2254, 2303,
      2352, 2401, 2450, 2499, 2548, 2597, 2646, 2695,
      2744, 2793, 2842, 2891, 2940, 2989, 3038, 3087,
      3136, 3185, 3234, 3283, 3332, 3381, 3430, 3479,
      3528, 3577, 3626, 3675, 3724, 3773, 3822, 3871,
      3920, 3969, 4018, 4067, 4116, 4165, 4214, 4263,
      4312, 4361, 4410, 4459, 4508, 4557, 4606, 4655,
      4704, 4753, 4802, 4851, 4900, 4949, 4998, 5047,
      5096, 5145, 5194, 5243, 5292, 5341, 5390, 5439,
      5488, 5537, 5586, 5635, 5684, 5733, 5782, 5831,
      5880, 5929, 5978, 6027, 6076, 6125, 6174, 6223,
      6272, 6321, 6370, 6419, 6468, 6517, 6566, 6615,
      6664, 6713, 6762, 6811, 6860, 6909, 6958, 7007,
      7056, 7105, 7154, 7203, 7252, 7301, 7350, 7399,
      7448, 7497, 7546, 7595, 7644, 7693, 7742, 7791,
      7840, 7889, 7938, 7987, 8036, 8085, 8134, 8183,
      8232, 8281, 8330, 8379, 8428, 8477, 8526, 8575,
      8624, 8673, 8722, 8771, 8820, 8869, 8918, 8967,
      9016, 9065, 9114, 9163, 9212, 9261, 9310, 9359,
      9408, 9457, 9506, 9555, 9604, 9653, 9702, 9751,
      9800, 9849, 9898, 9947, 9996, 10045, 10094, 10143,
      10192, 10241, 10290, 10339, 10388, 10437, 10486, 10535,
      10584, 10633, 10682, 10731, 10780, 10829, 10878, 10927,
      10976, 11025, 11074, 11123, 11172, 11221, 11270, 11319,
      11368, 11417, 11466, 11515, 11564, 11613, 11662, 11711,
      11760, 11809, 11858, 11907, 11956, 12005, 12054, 12103,
      12152, 12201, 12250, 12299, 12348, 12397, 12446, 12495,
      12544, 12593, 12642, 12691, 12740, 12789, 12838, 12887,
      12936, 12985, 13034, 13083, 13132, 13181, 13230, 13279,
      13328, 13377, 13426, 13475, 13524, 13573, 13622, 13671,
      13720, 13769, 13818, 13867, 13916, 13965, 14014, 14063,
      14112, 14161, 14210, 14259, 14308, 14357, 14406, 14455,
      14504, 14553, 14602, 14651, 14700, 14749, 14798, 14847,
      14896, 14945, 14994, 15043, 15092, 15141, 15190, 15239,
      15288, 15337, 15386, 15435, 15484, 15533, 15582, 15631,
      15680, 15729, 15778, 15827, 15876, 15925, 15974, 16023,
      16072, 16121, 16170, 16219, 16268, 16317, 16366, 16415,
      16464, 16513, 16562, 16611, 16660, 16709, 16758, 16807,
      16856, 16905, 16954, 17003, 17052, 17101, 17150, 17199,
      17248, 17297, 17346, 17395, 17444, 17493, 17542, 17591,
      17640, 17689, 17738, 17787, 17836, 17885, 17934, 17983,
      18032, 18081, 18130, 18179, 18228, 18277, 18326, 18375,
      18424, 18473, 18522, 18571, 18620, 18669, 18718, 18767,
      18816, 18865, 18914, 18963, 19012, 19061, 19110, 19159,
      19208, 19257, 19306, 19355, 19404, 19453, 19502, 19551,
      19600, 19649, 19698, 19747, 19796, 19845, 19894, 19943,
      19992, 20041, 20090, 20139, 20188, 20237, 20286, 20335,
      20384, 20433, 20482, 20531, 20580, 20629, 20678, 20727,
      20776, 20825, 20874, 20923, 20972, 21021, 21070, 21119,
      21168, 21217, 21266, 21315, 21364, 21413, 21462, 21511,
      21560, 21609, 21658, 21707, 21756, 21805, 21854, 21903,
      21952, 22001, 22050, 22099, 22148, 22197, 22246, 22295,
      22344, 22393, 22442, 22491, 22540, 22589, 22638, 22687,
      22736, 22785, 22834, 22883, 22932, 22981, 23030, 23079,
      23128, 23177, 23226, 23275, 23324, 23373, 23422, 23471,
      23520, 23569, 23618, 23667, 23716, 23765, 23814, 23863,
      23912, 23961, 24010, 24059, 24108, 24157, 24206, 24255,
      24304, 24353, 24402, 24451, 24500, 24549, 24598, 24647,
      24696, 24745, 24794, 24843, 24892, 24941, 24990, 25039 }
  };
  return &parm;
}

RT_DATA_INFO* Get_rt_data_info() {
  static RT_DATA_INFO info = {
    "conv_512x512x7x3.weight",
    "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX",
    DE_MSG_F32
  };
  return &info;
}

int32_t _cst_5[9] = {
  -8, -7, -6, -1, 0, 1, 6, 7,
  8
};
