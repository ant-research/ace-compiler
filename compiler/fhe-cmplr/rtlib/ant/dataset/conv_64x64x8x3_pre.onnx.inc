// external header files
#include "rt_ant/rt_ant.h"

typedef double float64_t;
typedef float float32_t;

extern float32_t _cst_0[36864];
extern float32_t _cst_1[64];
extern int32_t _cst_2[1];
extern float32_t _cst_3[2949120];
extern float32_t _cst_4[4096];
extern int32_t _cst_5[9];
extern float32_t _cst_6[4096];
extern float32_t _cst_7[4096];
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
  int32_t num_block_collecitve_data_n2_8;
  int32_t num_block_collecitve_gap_n2_9;
  CIPHERTEXT output_10;
  CIPHERTEXT _ckks_gen_tmp_77_11;
  CIPHERTEXT _ckks_gen_tmp_133_12;
  CIPHERTEXT _ckks_gen_tmp_163_13;
  CIPHERTEXT _ckks_gen_tmp_179_14;
  MODULUS* _pgen_modulus_15;
  uint32_t _pgen_num_q_16;
  uint32_t _pgen_rns_idx_17;
  CIPHERTEXT _pgen_tmp_0_18;
  CIPHERTEXT _pgen_tmp_1_19;
  CIPHERTEXT _pgen_tmp_2_20;
  PLAINTEXT _pgen_tmp_3_21;
  CIPHERTEXT _pgen_tmp_4_22;
  CIPHERTEXT _pgen_tmp_5_23;
  PLAINTEXT _pgen_tmp_6_24;
  CIPHERTEXT _pgen_tmp_7_25;
  PLAINTEXT _pgen_tmp_8_26;
  CIPHERTEXT _pgen_tmp_9_27;
  CIPHERTEXT _pgen_tmp_10_28;
  PLAINTEXT _pgen_tmp_11_29;
  CIPHERTEXT _pgen_tmp_12_30;
  CIPHERTEXT _pgen_tmp_13_31;
  PLAINTEXT _pgen_tmp_14_32;
  CIPHERTEXT _pgen_tmp_15_33;
  PLAINTEXT _pgen_tmp_16_34;
  CIPHERTEXT _preg_268435456;
  CIPHERTEXT _preg_268435457;
  CIPHERTEXT _preg_268435458;
  CIPHERTEXT _preg_268435459;
  CIPHERTEXT _preg_268435460;
  CIPHERTEXT _preg_268435461;
  CIPHERTEXT _preg_268435462;
  CIPHERTEXT _preg_268435463;
  CIPHERTEXT _preg_268435464;
  CIPHERTEXT _preg_268435465;
  CIPHERTEXT _preg_268435466;
  uint32_t  degree = Degree();
  input_0 = Get_input_data("input", 0);
  memset(&input_dup_n1_1, 0, sizeof(input_dup_n1_1));
  memset(&input_block_n1_3, 0, sizeof(input_block_n1_3));
  memset(&result_n2_4, 0, sizeof(result_n2_4));
  memset(&result_grid_n2_6, 0, sizeof(result_grid_n2_6));
  memset(&output_10, 0, sizeof(output_10));
  memset(&_ckks_gen_tmp_77_11, 0, sizeof(_ckks_gen_tmp_77_11));
  memset(&_ckks_gen_tmp_133_12, 0, sizeof(_ckks_gen_tmp_133_12));
  memset(&_ckks_gen_tmp_163_13, 0, sizeof(_ckks_gen_tmp_163_13));
  memset(&_ckks_gen_tmp_179_14, 0, sizeof(_ckks_gen_tmp_179_14));
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
  memset(&_pgen_tmp_14_32, 0, sizeof(_pgen_tmp_14_32));
  memset(&_pgen_tmp_15_33, 0, sizeof(_pgen_tmp_15_33));
  memset(&_pgen_tmp_16_34, 0, sizeof(_pgen_tmp_16_34));
  memset(&_preg_268435456, 0, sizeof(_preg_268435456));
  memset(&_preg_268435457, 0, sizeof(_preg_268435457));
  memset(&_preg_268435458, 0, sizeof(_preg_268435458));
  memset(&_preg_268435459, 0, sizeof(_preg_268435459));
  memset(&_preg_268435460, 0, sizeof(_preg_268435460));
  memset(&_preg_268435461, 0, sizeof(_preg_268435461));
  memset(&_preg_268435462, 0, sizeof(_preg_268435462));
  memset(&_preg_268435463, 0, sizeof(_preg_268435463));
  memset(&_preg_268435464, 0, sizeof(_preg_268435464));
  memset(&_preg_268435465, 0, sizeof(_preg_268435465));
  memset(&_preg_268435466, 0, sizeof(_preg_268435466));
  // /conv/Conv
  // pragma: 65536, 1027, 33
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435456, &input_0);
  Free_poly_data(&input_0._c1_poly);
  Free_poly_data(&input_0._c0_poly);
  // BlockingRot replicate=5
  _preg_268435458 = Rotate(_preg_268435456, -4096);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_0_18, &_preg_268435456, &_preg_268435458);
  CMPLR_TM_END(12, 0);
  _preg_268435459 = Rotate(_preg_268435456, -8192);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_1_19, &_pgen_tmp_0_18, &_preg_268435459);
  CMPLR_TM_END(12, 0);
  _preg_268435460 = Rotate(_preg_268435456, -12288);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_2_20, &_pgen_tmp_1_19, &_preg_268435460);
  CMPLR_TM_END(12, 0);
  _preg_268435461 = Rotate(_preg_268435456, -16384);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&input_dup_n1_1, &_pgen_tmp_2_20, &_preg_268435461);
  _pgen_modulus_15 = Q_modulus();
  _pgen_num_q_16 = Poly_level(&input_dup_n1_1._c0_poly);
  for (_pgen_rns_idx_17 = 0; _pgen_rns_idx_17 < _pgen_num_q_16; _pgen_rns_idx_17 = _pgen_rns_idx_17 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_0_18._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_preg_268435456._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_preg_268435458._c0_poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_0_18._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_preg_268435456._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_preg_268435458._c1_poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_1_19._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_0_18._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_preg_268435459._c0_poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_1_19._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_0_18._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_preg_268435459._c1_poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_2_20._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_1_19._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_preg_268435460._c0_poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_2_20._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_1_19._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_preg_268435460._c1_poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
    Hw_modadd(Coeffs(&input_dup_n1_1._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_2_20._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_preg_268435461._c0_poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
    Hw_modadd(Coeffs(&input_dup_n1_1._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_2_20._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_preg_268435461._c1_poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
    _pgen_modulus_15 = _pgen_modulus_15 + 1;
  }
  Free_poly_data(&_preg_268435458._c1_poly);
  Free_poly_data(&_preg_268435458._c0_poly);
  Free_poly_data(&_preg_268435456._c1_poly);
  Free_poly_data(&_preg_268435456._c0_poly);
  Free_poly_data(&_preg_268435459._c1_poly);
  Free_poly_data(&_preg_268435459._c0_poly);
  Free_poly_data(&_pgen_tmp_0_18._c1_poly);
  Free_poly_data(&_pgen_tmp_0_18._c0_poly);
  Free_poly_data(&_preg_268435460._c1_poly);
  Free_poly_data(&_preg_268435460._c0_poly);
  Free_poly_data(&_pgen_tmp_1_19._c1_poly);
  Free_poly_data(&_pgen_tmp_1_19._c0_poly);
  Free_poly_data(&_preg_268435461._c1_poly);
  Free_poly_data(&_preg_268435461._c0_poly);
  Free_poly_data(&_pgen_tmp_2_20._c1_poly);
  Free_poly_data(&_pgen_tmp_2_20._c0_poly);
  CMPLR_TM_END(12, 0);
  // BlockingRot HRot=bs=9
  for (bs_input_n1_2 = 0; bs_input_n1_2 < 9; bs_input_n1_2 = bs_input_n1_2 + 1) {
    _preg_268435462 = Rotate(input_dup_n1_1, _cst_5[bs_input_n1_2]);
    Copy_ciph(&input_block_n1_3[bs_input_n1_2], &_preg_268435462);
    Free_poly_data(&_preg_268435462._c1_poly);
    Free_poly_data(&_preg_268435462._c0_poly);
  }
  Free_poly_data(&input_dup_n1_1._c1_poly);
  Free_poly_data(&input_dup_n1_1._c0_poly);
  Zero_ciph(&result_n2_4);
  for (num_grid_n2_5 = 0; num_grid_n2_5 < 16; num_grid_n2_5 = num_grid_n2_5 + 1) {
    Zero_ciph(&result_grid_n2_6);
    for (cap_block_n2_7 = 0; cap_block_n2_7 < 9; cap_block_n2_7 = cap_block_n2_7 + 1) {
      _ckks_gen_tmp_77_11 = input_block_n1_3[cap_block_n2_7];
      Pt_from_msg_ofst(&_pgen_tmp_3_21, 0/* cst_3 */, ((cap_block_n2_7 + (num_grid_n2_5 * 9))) * (20480), 20480, 1, 3);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_4_22, &_ckks_gen_tmp_77_11, &_pgen_tmp_3_21);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n2_6, &result_grid_n2_6, &_pgen_tmp_4_22);
      _pgen_modulus_15 = Q_modulus();
      _pgen_num_q_16 = Poly_level(&result_grid_n2_6._c0_poly);
      for (_pgen_rns_idx_17 = 0; _pgen_rns_idx_17 < _pgen_num_q_16; _pgen_rns_idx_17 = _pgen_rns_idx_17 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_4_22._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_ckks_gen_tmp_77_11._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_3_21._poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_4_22._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_ckks_gen_tmp_77_11._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_3_21._poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
        Hw_modadd(Coeffs(&result_grid_n2_6._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&result_grid_n2_6._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_4_22._c0_poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
        Hw_modadd(Coeffs(&result_grid_n2_6._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&result_grid_n2_6._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_4_22._c1_poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
        _pgen_modulus_15 = _pgen_modulus_15 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435463 = Rotate(result_grid_n2_6, num_grid_n2_5 * 64);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n2_4, &result_n2_4, &_preg_268435463);
    _pgen_modulus_15 = Q_modulus();
    _pgen_num_q_16 = Poly_level(&result_n2_4._c0_poly);
    for (_pgen_rns_idx_17 = 0; _pgen_rns_idx_17 < _pgen_num_q_16; _pgen_rns_idx_17 = _pgen_rns_idx_17 + 1) {
      Hw_modadd(Coeffs(&result_n2_4._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&result_n2_4._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_preg_268435463._c0_poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
      Hw_modadd(Coeffs(&result_n2_4._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&result_n2_4._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_preg_268435463._c1_poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
      _pgen_modulus_15 = _pgen_modulus_15 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_poly_data(&_preg_268435463._c1_poly);
    Free_poly_data(&_preg_268435463._c0_poly);
  }
  Free_ciph_poly(input_block_n1_3, 9);
  Free_poly_data(&_pgen_tmp_3_21._poly);
  Free_poly_data(&_pgen_tmp_4_22._c1_poly);
  Free_poly_data(&_pgen_tmp_4_22._c0_poly);
  Free_poly_data(&result_grid_n2_6._c1_poly);
  Free_poly_data(&result_grid_n2_6._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n2_4, &result_n2_4);
  Rescale(&result_n2_4._c0_poly, &result_n2_4._c0_poly);
  Rescale(&result_n2_4._c1_poly, &result_n2_4._c1_poly);
  CMPLR_TM_END(19, 0);
  Pt_from_msg(&_pgen_tmp_6_24, 1 /* cst_6 */, 4096, 1, 2);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_5_23, &result_n2_4, &_pgen_tmp_6_24);
  _pgen_modulus_15 = Q_modulus();
  _pgen_num_q_16 = Poly_level(&_pgen_tmp_5_23._c0_poly);
  for (_pgen_rns_idx_17 = 0; _pgen_rns_idx_17 < _pgen_num_q_16; _pgen_rns_idx_17 = _pgen_rns_idx_17 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_5_23._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&result_n2_4._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_6_24._poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_5_23._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&result_n2_4._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_6_24._poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
    _pgen_modulus_15 = _pgen_modulus_15 + 1;
  }
  Free_poly_data(&_pgen_tmp_6_24._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435457, &_pgen_tmp_5_23);
  Rescale(&_preg_268435457._c0_poly, &_pgen_tmp_5_23._c0_poly);
  Rescale(&_preg_268435457._c1_poly, &_pgen_tmp_5_23._c1_poly);
  Free_poly_data(&_pgen_tmp_5_23._c1_poly);
  Free_poly_data(&_pgen_tmp_5_23._c0_poly);
  CMPLR_TM_END(19, 0);
  for (num_block_collecitve_data_n2_8 = 1; num_block_collecitve_data_n2_8 < 4; num_block_collecitve_data_n2_8 = num_block_collecitve_data_n2_8 + 1) {
    _preg_268435464 = Rotate(result_n2_4, num_block_collecitve_data_n2_8 * 5120);
    Copy_ciph(&_ckks_gen_tmp_133_12, &_preg_268435464);
    Pt_from_msg(&_pgen_tmp_8_26, 2 /* cst_6 */, 4096, 1, 2);
    CMPLR_TM_START(17, 0);
    Init_ciph_up_scale_plain(&_pgen_tmp_7_25, &_ckks_gen_tmp_133_12, &_pgen_tmp_8_26);
    _pgen_modulus_15 = Q_modulus();
    _pgen_num_q_16 = Poly_level(&_pgen_tmp_7_25._c0_poly);
    for (_pgen_rns_idx_17 = 0; _pgen_rns_idx_17 < _pgen_num_q_16; _pgen_rns_idx_17 = _pgen_rns_idx_17 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_7_25._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_ckks_gen_tmp_133_12._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_8_26._poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_7_25._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_ckks_gen_tmp_133_12._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_8_26._poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
      _pgen_modulus_15 = _pgen_modulus_15 + 1;
    }
    CMPLR_TM_END(17, 0);
    CMPLR_TM_START(19, 0);
    Init_ciph_down_scale(&_pgen_tmp_9_27, &_pgen_tmp_7_25);
    Rescale(&_pgen_tmp_9_27._c0_poly, &_pgen_tmp_7_25._c0_poly);
    Rescale(&_pgen_tmp_9_27._c1_poly, &_pgen_tmp_7_25._c1_poly);
    CMPLR_TM_END(19, 0);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&_preg_268435457, &_preg_268435457, &_pgen_tmp_9_27);
    _pgen_modulus_15 = Q_modulus();
    _pgen_num_q_16 = Poly_level(&_preg_268435457._c0_poly);
    for (_pgen_rns_idx_17 = 0; _pgen_rns_idx_17 < _pgen_num_q_16; _pgen_rns_idx_17 = _pgen_rns_idx_17 + 1) {
      Hw_modadd(Coeffs(&_preg_268435457._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_preg_268435457._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_9_27._c0_poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
      Hw_modadd(Coeffs(&_preg_268435457._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_preg_268435457._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_9_27._c1_poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
      _pgen_modulus_15 = _pgen_modulus_15 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_poly_data(&_preg_268435464._c1_poly);
    Free_poly_data(&_preg_268435464._c0_poly);
  }
  Free_poly_data(&_pgen_tmp_8_26._poly);
  Free_poly_data(&_ckks_gen_tmp_133_12._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_133_12._c0_poly);
  Free_poly_data(&_pgen_tmp_7_25._c1_poly);
  Free_poly_data(&_pgen_tmp_7_25._c0_poly);
  Free_poly_data(&_pgen_tmp_9_27._c1_poly);
  Free_poly_data(&_pgen_tmp_9_27._c0_poly);
  for (num_block_collecitve_gap_n2_9 = 0; num_block_collecitve_gap_n2_9 < 3; num_block_collecitve_gap_n2_9 = num_block_collecitve_gap_n2_9 + 1) {
    _preg_268435465 = Rotate(result_n2_4, (num_block_collecitve_gap_n2_9 * 5120) + 1024);
    Copy_ciph(&_ckks_gen_tmp_163_13, &_preg_268435465);
    Pt_from_msg(&_pgen_tmp_11_29, 3 /* cst_7 */, 4096, 1, 2);
    CMPLR_TM_START(17, 0);
    Init_ciph_up_scale_plain(&_pgen_tmp_10_28, &_ckks_gen_tmp_163_13, &_pgen_tmp_11_29);
    _pgen_modulus_15 = Q_modulus();
    _pgen_num_q_16 = Poly_level(&_pgen_tmp_10_28._c0_poly);
    for (_pgen_rns_idx_17 = 0; _pgen_rns_idx_17 < _pgen_num_q_16; _pgen_rns_idx_17 = _pgen_rns_idx_17 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_10_28._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_ckks_gen_tmp_163_13._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_11_29._poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_10_28._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_ckks_gen_tmp_163_13._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_11_29._poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
      _pgen_modulus_15 = _pgen_modulus_15 + 1;
    }
    CMPLR_TM_END(17, 0);
    CMPLR_TM_START(19, 0);
    Init_ciph_down_scale(&_pgen_tmp_12_30, &_pgen_tmp_10_28);
    Rescale(&_pgen_tmp_12_30._c0_poly, &_pgen_tmp_10_28._c0_poly);
    Rescale(&_pgen_tmp_12_30._c1_poly, &_pgen_tmp_10_28._c1_poly);
    CMPLR_TM_END(19, 0);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&_preg_268435457, &_preg_268435457, &_pgen_tmp_12_30);
    _pgen_modulus_15 = Q_modulus();
    _pgen_num_q_16 = Poly_level(&_preg_268435457._c0_poly);
    for (_pgen_rns_idx_17 = 0; _pgen_rns_idx_17 < _pgen_num_q_16; _pgen_rns_idx_17 = _pgen_rns_idx_17 + 1) {
      Hw_modadd(Coeffs(&_preg_268435457._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_preg_268435457._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_12_30._c0_poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
      Hw_modadd(Coeffs(&_preg_268435457._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_preg_268435457._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_12_30._c1_poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
      _pgen_modulus_15 = _pgen_modulus_15 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_poly_data(&_preg_268435465._c1_poly);
    Free_poly_data(&_preg_268435465._c0_poly);
  }
  Free_poly_data(&_pgen_tmp_11_29._poly);
  Free_poly_data(&_ckks_gen_tmp_163_13._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_163_13._c0_poly);
  Free_poly_data(&_pgen_tmp_10_28._c1_poly);
  Free_poly_data(&_pgen_tmp_10_28._c0_poly);
  Free_poly_data(&_pgen_tmp_12_30._c1_poly);
  Free_poly_data(&_pgen_tmp_12_30._c0_poly);
  _preg_268435466 = Rotate(result_n2_4, -4096);
  Copy_ciph(&_ckks_gen_tmp_179_14, &_preg_268435466);
  Free_poly_data(&_preg_268435466._c1_poly);
  Free_poly_data(&_preg_268435466._c0_poly);
  Pt_from_msg(&_pgen_tmp_14_32, 4 /* cst_7 */, 4096, 1, 2);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_13_31, &_ckks_gen_tmp_179_14, &_pgen_tmp_14_32);
  _pgen_modulus_15 = Q_modulus();
  _pgen_num_q_16 = Poly_level(&_pgen_tmp_13_31._c0_poly);
  for (_pgen_rns_idx_17 = 0; _pgen_rns_idx_17 < _pgen_num_q_16; _pgen_rns_idx_17 = _pgen_rns_idx_17 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_13_31._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_ckks_gen_tmp_179_14._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_14_32._poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_13_31._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_ckks_gen_tmp_179_14._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_14_32._poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
    _pgen_modulus_15 = _pgen_modulus_15 + 1;
  }
  Free_poly_data(&_pgen_tmp_14_32._poly);
  Free_poly_data(&_ckks_gen_tmp_179_14._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_179_14._c0_poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_15_33, &_pgen_tmp_13_31);
  Rescale(&_pgen_tmp_15_33._c0_poly, &_pgen_tmp_13_31._c0_poly);
  Rescale(&_pgen_tmp_15_33._c1_poly, &_pgen_tmp_13_31._c1_poly);
  Free_poly_data(&_pgen_tmp_13_31._c1_poly);
  Free_poly_data(&_pgen_tmp_13_31._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_preg_268435457, &_preg_268435457, &_pgen_tmp_15_33);
  _pgen_modulus_15 = Q_modulus();
  _pgen_num_q_16 = Poly_level(&_preg_268435457._c0_poly);
  for (_pgen_rns_idx_17 = 0; _pgen_rns_idx_17 < _pgen_num_q_16; _pgen_rns_idx_17 = _pgen_rns_idx_17 + 1) {
    Hw_modadd(Coeffs(&_preg_268435457._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_preg_268435457._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_15_33._c0_poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
    Hw_modadd(Coeffs(&_preg_268435457._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_preg_268435457._c1_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_15_33._c1_poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
    _pgen_modulus_15 = _pgen_modulus_15 + 1;
  }
  Free_poly_data(&_pgen_tmp_15_33._c1_poly);
  Free_poly_data(&_pgen_tmp_15_33._c0_poly);
  CMPLR_TM_END(12, 0);
  Pt_from_msg(&_pgen_tmp_16_34, 5 /* cst_4 */, 4096, 1, 1);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n2_4, &_preg_268435457, &_pgen_tmp_16_34);
  _pgen_modulus_15 = Q_modulus();
  _pgen_num_q_16 = Poly_level(&result_n2_4._c0_poly);
  for (_pgen_rns_idx_17 = 0; _pgen_rns_idx_17 < _pgen_num_q_16; _pgen_rns_idx_17 = _pgen_rns_idx_17 + 1) {
    Hw_modadd(Coeffs(&result_n2_4._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_preg_268435457._c0_poly, _pgen_rns_idx_17, degree), Coeffs(&_pgen_tmp_16_34._poly, _pgen_rns_idx_17, degree), _pgen_modulus_15, degree);
    Set_coeffs(&result_n2_4._c1_poly, _pgen_rns_idx_17, degree, Coeffs(&_preg_268435457._c1_poly, _pgen_rns_idx_17, degree));
    _pgen_modulus_15 = _pgen_modulus_15 + 1;
  }
  Free_poly_data(&_pgen_tmp_16_34._poly);
  Free_poly_data(&_preg_268435457._c1_poly);
  Free_poly_data(&_preg_268435457._c0_poly);
  CMPLR_TM_END(13, 0);
  Copy_ciph(&output_10, &result_n2_4);
  Free_poly_data(&result_n2_4._c1_poly);
  Free_poly_data(&result_n2_4._c0_poly);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 33
  Set_output_data("output", 0, &output_10);
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
    LIB_ANT, 65536, 0, 2, 3, 60, 56, 2, 192, 34, 
    { -16384, -12288, -8192, -4096, -9, -8, -7, -1,
      0, 1, 7, 8, 9, 64, 128, 192,
      256, 320, 384, 448, 512, 576, 640, 704,
      768, 832, 896, 960, 1024, 5120, 6144, 10240,
      11264, 15360 }
  };
  return &parm;
}

RT_DATA_INFO* Get_rt_data_info() {
  static RT_DATA_INFO info = {
    "conv_64x64x8x3.weight",
    "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX",
    DE_MSG_F32
  };
  return &info;
}

int32_t _cst_5[9] = {
  -9, -8, -7, -1, 0, 1, 7, 8,
  9
};
