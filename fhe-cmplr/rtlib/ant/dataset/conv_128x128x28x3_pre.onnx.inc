// external header files
#include "rt_ant/rt_ant.h"

typedef double float64_t;
typedef float float32_t;

extern float32_t _cst_0[147456];
extern float32_t _cst_1[128];
extern float32_t _cst_2[147456];
extern float32_t _cst_3[100352];
extern float32_t _cst_4[100352];
extern float32_t _cst_5[32];
extern float32_t _cst_6[115605504];
extern int32_t _cst_7[1];
extern float32_t _cst_8[7225344];
extern float32_t _cst_9[25088];
extern int32_t _cst_10[9];
extern float32_t _cst_11[802816];
extern float32_t _cst_12[802816];
CIPHERTEXT Rotate(CIPHERTEXT ciph, int32_t rot_idx);

bool Main_graph() {
  CIPHERTEXT input_0[4];
  CIPHERTEXT VS_out_conv1_1[4];
  CIPHERTEXT VS_out_conv_halo1_2[4];
  int32_t mesh_y_n1_3;
  int32_t mesh_z_n1_4;
  CIPHERTEXT input_dup_n1_5;
  int32_t bs_input_n1_6;
  CIPHERTEXT input_block_n1_7[9];
  int32_t mesh_x_n1_8;
  CIPHERTEXT result_n2_9;
  int32_t num_grid_n2_10;
  CIPHERTEXT result_grid_n2_11;
  int32_t cap_block_n2_12;
  CIPHERTEXT _ckks_gen_tmp_102_13;
  int32_t rescale_iv_14;
  MODULUS* _pgen_modulus_15;
  uint32_t _pgen_num_q_16;
  uint32_t _pgen_rns_idx_17;
  PLAINTEXT _pgen_tmp_0_18;
  CIPHERTEXT _pgen_tmp_1_19;
  CIPHERTEXT _pgen_tmp_2_20;
  CIPHERTEXT _pgen_tmp_3_21;
  PLAINTEXT _pgen_tmp_4_22;
  CIPHERTEXT _pgen_tmp_5_23;
  CIPHERTEXT _pgen_tmp_6_24;
  PLAINTEXT _pgen_tmp_7_25;
  CIPHERTEXT _pgen_tmp_8_26;
  PLAINTEXT _pgen_tmp_9_27;
  CIPHERTEXT _pgen_tmp_10_28;
  CIPHERTEXT _pgen_tmp_11_29;
  CIPHERTEXT _pgen_tmp_12_30;
  CIPHERTEXT _pgen_tmp_13_31;
  CIPHERTEXT _preg_268435456;
  CIPHERTEXT _preg_268435457;
  CIPHERTEXT _preg_268435458;
  CIPHERTEXT _preg_268435459;
  uint32_t  degree = Degree();
  for (int input_idx = 0; input_idx < 4; ++input_idx) {
    input_0[input_idx] = Get_input_data("input", input_idx);
  }
  memset(&VS_out_conv1_1, 0, sizeof(VS_out_conv1_1));
  memset(&VS_out_conv_halo1_2, 0, sizeof(VS_out_conv_halo1_2));
  memset(&input_dup_n1_5, 0, sizeof(input_dup_n1_5));
  memset(&input_block_n1_7, 0, sizeof(input_block_n1_7));
  memset(&result_n2_9, 0, sizeof(result_n2_9));
  memset(&result_grid_n2_11, 0, sizeof(result_grid_n2_11));
  memset(&_ckks_gen_tmp_102_13, 0, sizeof(_ckks_gen_tmp_102_13));
  memset(&_pgen_tmp_0_18, 0, sizeof(_pgen_tmp_0_18));
  memset(&_pgen_tmp_1_19, 0, sizeof(_pgen_tmp_1_19));
  memset(&_pgen_tmp_2_20, 0, sizeof(_pgen_tmp_2_20));
  memset(&_pgen_tmp_3_21, 0, sizeof(_pgen_tmp_3_21));
  memset(&_pgen_tmp_4_22, 0, sizeof(_pgen_tmp_4_22));
  memset(&_pgen_tmp_5_23, 0, sizeof(_pgen_tmp_5_23));
  memset(&_pgen_tmp_6_24, 0, sizeof(_pgen_tmp_6_24));
  memset(&_pgen_tmp_7_25, 0, sizeof(_pgen_tmp_7_25));
  memset(&_pgen_tmp_8_26, 0, sizeof(_pgen_tmp_8_26));
  memset(&_pgen_tmp_9_27, 0, sizeof(_pgen_tmp_9_27));
  memset(&_pgen_tmp_10_28, 0, sizeof(_pgen_tmp_10_28));
  memset(&_pgen_tmp_11_29, 0, sizeof(_pgen_tmp_11_29));
  memset(&_pgen_tmp_12_30, 0, sizeof(_pgen_tmp_12_30));
  memset(&_pgen_tmp_13_31, 0, sizeof(_pgen_tmp_13_31));
  memset(&_preg_268435456, 0, sizeof(_preg_268435456));
  memset(&_preg_268435457, 0, sizeof(_preg_268435457));
  memset(&_preg_268435458, 0, sizeof(_preg_268435458));
  memset(&_preg_268435459, 0, sizeof(_preg_268435459));
  // /conv/Conv
  // pragma: 65536, 1027, 33
  CMPLR_TM_START(5, 0);
  memset(&VS_out_conv1_1, 0, sizeof(VS_out_conv1_1));
  memset(&VS_out_conv_halo1_2, 0, sizeof(VS_out_conv_halo1_2));
  Free_ciph_poly(VS_out_conv_halo1_2, 4);
  for (mesh_y_n1_3 = 0; mesh_y_n1_3 < 4; mesh_y_n1_3 = mesh_y_n1_3 + 1) {
    for (mesh_z_n1_4 = 0; mesh_z_n1_4 < 1; mesh_z_n1_4 = mesh_z_n1_4 + 1) {
      Copy_ciph(&_preg_268435456, &input_0[((mesh_y_n1_3 * 1) + mesh_z_n1_4)]);
      Copy_ciph(&input_dup_n1_5, &_preg_268435456);
      for (bs_input_n1_6 = 0; bs_input_n1_6 < 9; bs_input_n1_6 = bs_input_n1_6 + 1) {
        _preg_268435457 = Rotate(input_dup_n1_5, _cst_10[bs_input_n1_6]);
        Copy_ciph(&input_block_n1_7[bs_input_n1_6], &_preg_268435457);
        Free_poly_data(&_preg_268435457._c1_poly);
        Free_poly_data(&_preg_268435457._c0_poly);
      }
      for (mesh_x_n1_8 = 0; mesh_x_n1_8 < 4; mesh_x_n1_8 = mesh_x_n1_8 + 1) {
        // BlockingRot replicate=1
        // BlockingRot HRot=bs=9
        Zero_ciph(&result_n2_9);
        for (num_grid_n2_10 = 0; num_grid_n2_10 < 32; num_grid_n2_10 = num_grid_n2_10 + 1) {
          Zero_ciph(&result_grid_n2_11);
          for (cap_block_n2_12 = 0; cap_block_n2_12 < 9; cap_block_n2_12 = cap_block_n2_12 + 1) {
            _ckks_gen_tmp_102_13 = input_block_n1_7[cap_block_n2_12];
            Pt_from_msg_ofst(&_pgen_tmp_0_18, 0/* cst_6 */, (((((mesh_x_n1_8 * 4) + mesh_y_n1_3) * 288) + (cap_block_n2_12 + (num_grid_n2_10 * 9)))) * (25088), 25088, 1, 3);
            CMPLR_TM_START(17, 0);
            Init_ciph_up_scale_plain(&_pgen_tmp_1_19, &_ckks_gen_tmp_102_13, &_pgen_tmp_0_18);
            CMPLR_TM_END(17, 0);
            CMPLR_TM_START(12, 0);
            Init_ciph_same_scale(&result_grid_n2_11, &result_grid_n2_11, &_pgen_tmp_1_19);
            _pgen_modulus_15 = Q_modulus();
            _pgen_num_q_16 = Poly_level(&result_grid_n2_11._c0_poly);
            for (_pgen_rns_idx_17 = 0; _pgen_rns_idx_17 < _pgen_num_q_16; _pgen_rns_idx_17 = _pgen_rns_idx_17 + 1) {
              Hw_modmul(Coeffs(&_pgen_tmp_1_19._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_ckks_gen_tmp_102_13._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_0_18._poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
              Hw_modmul(Coeffs(&_pgen_tmp_1_19._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_ckks_gen_tmp_102_13._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_0_18._poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
              Hw_modadd(Coeffs(&result_grid_n2_11._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&result_grid_n2_11._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_1_19._c0_poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
              Hw_modadd(Coeffs(&result_grid_n2_11._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&result_grid_n2_11._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_1_19._c1_poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
              _pgen_modulus_15 = _pgen_modulus_15 + 1;
            }
            CMPLR_TM_END(12, 0);
          }
          CMPLR_TM_START(19, 0);
          Init_ciph_down_scale(&_pgen_tmp_3_21, &result_grid_n2_11);
          Rescale(&_pgen_tmp_3_21._c0_poly, &result_grid_n2_11._c0_poly);
          Rescale(&_pgen_tmp_3_21._c1_poly, &result_grid_n2_11._c1_poly);
          CMPLR_TM_END(19, 0);
          Pt_from_msg_ofst(&_pgen_tmp_4_22, 1/* cst_11 */, (num_grid_n2_10) * (25088), 25088, 1, 2);
          CMPLR_TM_START(17, 0);
          Init_ciph_up_scale_plain(&_pgen_tmp_2_20, &_pgen_tmp_3_21, &_pgen_tmp_4_22);
          _pgen_modulus_15 = Q_modulus();
          _pgen_num_q_16 = Poly_level(&_pgen_tmp_2_20._c0_poly);
          for (_pgen_rns_idx_17 = 0; _pgen_rns_idx_17 < _pgen_num_q_16; _pgen_rns_idx_17 = _pgen_rns_idx_17 + 1) {
            Hw_modmul(Coeffs(&_pgen_tmp_2_20._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_3_21._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_4_22._poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
            Hw_modmul(Coeffs(&_pgen_tmp_2_20._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_3_21._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_4_22._poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
            _pgen_modulus_15 = _pgen_modulus_15 + 1;
          }
          CMPLR_TM_END(17, 0);
          _preg_268435458 = Rotate(_pgen_tmp_2_20, -25088 + (num_grid_n2_10 * 784));
          CMPLR_TM_START(19, 0);
          Init_ciph_down_scale(&_pgen_tmp_6_24, &result_grid_n2_11);
          Rescale(&_pgen_tmp_6_24._c0_poly, &result_grid_n2_11._c0_poly);
          Rescale(&_pgen_tmp_6_24._c1_poly, &result_grid_n2_11._c1_poly);
          CMPLR_TM_END(19, 0);
          Pt_from_msg_ofst(&_pgen_tmp_7_25, 2/* cst_12 */, (num_grid_n2_10) * (25088), 25088, 1, 2);
          CMPLR_TM_START(17, 0);
          Init_ciph_up_scale_plain(&_pgen_tmp_5_23, &_pgen_tmp_6_24, &_pgen_tmp_7_25);
          _pgen_modulus_15 = Q_modulus();
          _pgen_num_q_16 = Poly_level(&_pgen_tmp_5_23._c0_poly);
          for (_pgen_rns_idx_17 = 0; _pgen_rns_idx_17 < _pgen_num_q_16; _pgen_rns_idx_17 = _pgen_rns_idx_17 + 1) {
            Hw_modmul(Coeffs(&_pgen_tmp_5_23._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_6_24._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_7_25._poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
            Hw_modmul(Coeffs(&_pgen_tmp_5_23._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_6_24._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_7_25._poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
            _pgen_modulus_15 = _pgen_modulus_15 + 1;
          }
          CMPLR_TM_END(17, 0);
          _preg_268435459 = Rotate(_pgen_tmp_5_23, num_grid_n2_10 * 784);
          CMPLR_TM_START(12, 0);
          Init_ciph_same_scale(&_pgen_tmp_8_26, &_preg_268435458, &_preg_268435459);
          CMPLR_TM_END(12, 0);
          CMPLR_TM_START(12, 0);
          Init_ciph_same_scale(&result_n2_9, &result_n2_9, &_pgen_tmp_8_26);
          _pgen_modulus_15 = Q_modulus();
          _pgen_num_q_16 = Poly_level(&result_n2_9._c0_poly);
          for (_pgen_rns_idx_17 = 0; _pgen_rns_idx_17 < _pgen_num_q_16; _pgen_rns_idx_17 = _pgen_rns_idx_17 + 1) {
            Hw_modadd(Coeffs(&_pgen_tmp_8_26._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_preg_268435458._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_preg_268435459._c0_poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
            Hw_modadd(Coeffs(&_pgen_tmp_8_26._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_preg_268435458._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_preg_268435459._c1_poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
            Hw_modadd(Coeffs(&result_n2_9._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&result_n2_9._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_8_26._c0_poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
            Hw_modadd(Coeffs(&result_n2_9._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&result_n2_9._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_8_26._c1_poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
            _pgen_modulus_15 = _pgen_modulus_15 + 1;
          }
          CMPLR_TM_END(12, 0);
          Free_poly_data(&_preg_268435458._c1_poly);
          Free_poly_data(&_preg_268435458._c0_poly);
          Free_poly_data(&_preg_268435459._c1_poly);
          Free_poly_data(&_preg_268435459._c0_poly);
        }
        Pt_from_msg(&_pgen_tmp_9_27, 3 /* cst_9 */, 25088, 2, 2);
        CMPLR_TM_START(13, 0);
        Init_ciph_same_scale_plain(&result_n2_9, &result_n2_9, &_pgen_tmp_9_27);
        _pgen_modulus_15 = Q_modulus();
        _pgen_num_q_16 = Poly_level(&result_n2_9._c0_poly);
        for (_pgen_rns_idx_17 = 0; _pgen_rns_idx_17 < _pgen_num_q_16; _pgen_rns_idx_17 = _pgen_rns_idx_17 + 1) {
          Hw_modadd(Coeffs(&result_n2_9._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&result_n2_9._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_9_27._poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
          Set_coeffs(&result_n2_9._c1_poly, _pgen_rns_idx_17, degree, Coeffs(&result_n2_9._c1_poly, _pgen_rns_idx_17, degree));
          _pgen_modulus_15 = _pgen_modulus_15 + 1;
        }
        CMPLR_TM_END(13, 0);
        _pgen_tmp_11_29 = VS_out_conv1_1[((mesh_x_n1_8 * 1) + mesh_z_n1_4)];
        CMPLR_TM_START(12, 0);
        Init_ciph_same_scale(&_pgen_tmp_10_28, &_pgen_tmp_11_29, &result_n2_9);
        _pgen_modulus_15 = Q_modulus();
        _pgen_num_q_16 = Poly_level(&_pgen_tmp_10_28._c0_poly);
        for (_pgen_rns_idx_17 = 0; _pgen_rns_idx_17 < _pgen_num_q_16; _pgen_rns_idx_17 = _pgen_rns_idx_17 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_10_28._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_11_29._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&result_n2_9._c0_poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_10_28._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_11_29._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&result_n2_9._c1_poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
          _pgen_modulus_15 = _pgen_modulus_15 + 1;
        }
        CMPLR_TM_END(12, 0);
        Copy_ciph(&VS_out_conv1_1[((mesh_x_n1_8 * 1) + mesh_z_n1_4)], &_pgen_tmp_10_28);
      }
    }
  }
  Free_ciph_poly(input_0, 4);
  Free_poly_data(&_preg_268435456._c1_poly);
  Free_poly_data(&_preg_268435456._c0_poly);
  Free_poly_data(&input_dup_n1_5._c1_poly);
  Free_poly_data(&input_dup_n1_5._c0_poly);
  Free_ciph_poly(input_block_n1_7, 9);
  Free_poly_data(&_pgen_tmp_0_18._poly);
  Free_poly_data(&_pgen_tmp_1_19._c1_poly);
  Free_poly_data(&_pgen_tmp_1_19._c0_poly);
  Free_poly_data(&_pgen_tmp_4_22._poly);
  Free_poly_data(&_pgen_tmp_3_21._c1_poly);
  Free_poly_data(&_pgen_tmp_3_21._c0_poly);
  Free_poly_data(&_pgen_tmp_2_20._c1_poly);
  Free_poly_data(&_pgen_tmp_2_20._c0_poly);
  Free_poly_data(&result_grid_n2_11._c1_poly);
  Free_poly_data(&result_grid_n2_11._c0_poly);
  Free_poly_data(&_pgen_tmp_7_25._poly);
  Free_poly_data(&_pgen_tmp_6_24._c1_poly);
  Free_poly_data(&_pgen_tmp_6_24._c0_poly);
  Free_poly_data(&_pgen_tmp_5_23._c1_poly);
  Free_poly_data(&_pgen_tmp_5_23._c0_poly);
  Free_poly_data(&_pgen_tmp_8_26._c1_poly);
  Free_poly_data(&_pgen_tmp_8_26._c0_poly);
  Free_poly_data(&_pgen_tmp_9_27._poly);
  Free_poly_data(&result_n2_9._c1_poly);
  Free_poly_data(&result_n2_9._c0_poly);
  Free_poly_data(&_pgen_tmp_10_28._c1_poly);
  Free_poly_data(&_pgen_tmp_10_28._c0_poly);
  for (rescale_iv_14 = 0; rescale_iv_14 < 4; rescale_iv_14 = rescale_iv_14 + 1) {
    _pgen_tmp_12_30 = VS_out_conv1_1[rescale_iv_14];
    CMPLR_TM_START(19, 0);
    Init_ciph_down_scale(&_pgen_tmp_13_31, &_pgen_tmp_12_30);
    Rescale(&_pgen_tmp_13_31._c0_poly, &_pgen_tmp_12_30._c0_poly);
    Rescale(&_pgen_tmp_13_31._c1_poly, &_pgen_tmp_12_30._c1_poly);
    CMPLR_TM_END(19, 0);
    Copy_ciph(&VS_out_conv1_1[rescale_iv_14], &_pgen_tmp_13_31);
  }
  Free_poly_data(&_pgen_tmp_13_31._c1_poly);
  Free_poly_data(&_pgen_tmp_13_31._c0_poly);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 33
  for (int output_idx = 0; output_idx < 4; ++output_idx) {
    Set_output_data("VS_out_conv1", output_idx, &VS_out_conv1_1[output_idx]);
  };
  return true;
}

int Get_input_count() {
  return 1;
}

DATA_SCHEME* Get_encode_scheme(int idx) {
  static MAP_DESC desc_0[] = {
    { NORMAL, 25088, 0, 0, 1},
    { NORMAL, 25088, 25088, 0, 1},
    { NORMAL, 25088, 50176, 0, 1},
    { NORMAL, 25088, 75264, 0, 1},
  };
  static DATA_SCHEME scheme_0 = {
    "input", {1, 128, 28, 28}, 4, desc_0
  };
  static DATA_SCHEME* scheme[] = { &scheme_0 };
  return scheme[idx];
}

int Get_output_count() {
  return 1;
}

DATA_SCHEME* Get_decode_scheme(int idx) {
  static MAP_DESC desc_0[] = {
    { NORMAL, 25088, 0, 0, 1},
    { NORMAL, 25088, 25088, 0, 1},
    { NORMAL, 25088, 50176, 0, 1},
    { NORMAL, 25088, 75264, 0, 1},
  };
  static DATA_SCHEME scheme = {
    "VS_out_conv1", {1, 128, 28, 28}, 4, desc_0
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
    LIB_ANT, 65536, 0, 2, 3, 60, 56, 2, 192, 72, 
    { -25088, -24304, -23520, -22736, -21952, -21168, -20384, -19600,
      -18816, -18032, -17248, -16464, -15680, -14896, -14112, -13328,
      -12544, -11760, -10976, -10192, -9408, -8624, -7840, -7056,
      -6272, -5488, -4704, -3920, -3136, -2352, -1568, -784,
      -29, -28, -27, -1, 0, 1, 27, 28,
      29, 784, 1568, 2352, 3136, 3920, 4704, 5488,
      6272, 7056, 7840, 8624, 9408, 10192, 10976, 11760,
      12544, 13328, 14112, 14896, 15680, 16464, 17248, 18032,
      18816, 19600, 20384, 21168, 21952, 22736, 23520, 24304 }
  };
  return &parm;
}

RT_DATA_INFO* Get_rt_data_info() {
  static RT_DATA_INFO info = {
    "conv_128x128x28x3.weight",
    "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX",
    DE_MSG_F32
  };
  return &info;
}

int32_t _cst_10[9] = {
  -29, -28, -27, -1, 0, 1, 27, 28,
  29
};
