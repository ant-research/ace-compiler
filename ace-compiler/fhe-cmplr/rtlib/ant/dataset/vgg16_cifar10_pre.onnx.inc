// external header files
#include "rt_ant/rt_ant.h"

typedef double float64_t;
typedef float float32_t;
typedef size_t LEVEL_T;
typedef double SCALE_T;

extern float32_t _cst_0[10][512];
extern float32_t _cst_1[10];
extern float32_t _cst_2[64][3][3][3];
extern float32_t _cst_3[64];
extern float32_t _cst_4[64][64][3][3];
extern float32_t _cst_5[64];
extern float32_t _cst_6[128][64][3][3];
extern float32_t _cst_7[128];
extern float32_t _cst_8[128][128][3][3];
extern float32_t _cst_9[128];
extern float32_t _cst_10[256][128][3][3];
extern float32_t _cst_11[256];
extern float32_t _cst_12[256][256][3][3];
extern float32_t _cst_13[256];
extern float32_t _cst_14[256][256][3][3];
extern float32_t _cst_15[256];
extern float32_t _cst_16[512][256][3][3];
extern float32_t _cst_17[512];
extern float32_t _cst_18[512][512][3][3];
extern float32_t _cst_19[512];
extern float32_t _cst_20[512][512][3][3];
extern float32_t _cst_21[512];
extern float32_t _cst_22[512][512][3][3];
extern float32_t _cst_23[512];
extern float32_t _cst_24[512][512][3][3];
extern float32_t _cst_25[512];
extern float32_t _cst_26[512][512][3][3];
extern float32_t _cst_27[512];
extern int64_t _cst_28[2];
extern int64_t _cst_29[2];
extern int64_t _cst_30[2];
extern int64_t _cst_31[2];
extern int64_t _cst_32[2];
extern int64_t _cst_33[2];
extern int64_t _cst_34[2];
extern int64_t _cst_35[2];
extern int64_t _cst_36[2];
extern int64_t _cst_37[2];
extern int64_t _cst_38[2];
extern int64_t _cst_39[2];
extern int64_t _cst_40[2];
extern int64_t _cst_41[2];
extern int64_t _cst_42[2];
extern int64_t _cst_43[2];
extern int32_t _cst_44[1];
extern float32_t _cst_45[36][65536];
extern float32_t _cst_46[65536];
extern int32_t _cst_47[9];
extern float32_t _cst_48[65536];
extern int32_t _cst_49[1];
extern float32_t _cst_50[576][65536];
extern float32_t _cst_51[65536];
extern int32_t _cst_52[9];
extern float32_t _cst_53[65536];
extern float32_t _cst_54[65536];
extern float32_t _cst_55[16][65536];
extern float32_t _cst_56[16][65536];
extern float32_t _cst_57[64][65536];
extern int32_t _cst_58[1];
extern float32_t _cst_59[576][32768];
extern float32_t _cst_60[32768];
extern int32_t _cst_61[9];
extern float32_t _cst_62[32768];
extern int32_t _cst_63[1];
extern float32_t _cst_64[1152][32768];
extern float32_t _cst_65[32768];
extern int32_t _cst_66[9];
extern float32_t _cst_67[32768];
extern float32_t _cst_68[32768];
extern float32_t _cst_69[8][32768];
extern float32_t _cst_70[8][32768];
extern float32_t _cst_71[128][32768];
extern int32_t _cst_72[1];
extern float32_t _cst_73[1152][16384];
extern float32_t _cst_74[16384];
extern int32_t _cst_75[9];
extern float32_t _cst_76[16384];
extern int32_t _cst_77[1];
extern float32_t _cst_78[2304][16384];
extern float32_t _cst_79[16384];
extern int32_t _cst_80[9];
extern float32_t _cst_81[16384];
extern int32_t _cst_82[1];
extern float32_t _cst_83[2304][16384];
extern float32_t _cst_84[16384];
extern int32_t _cst_85[9];
extern float32_t _cst_86[16384];
extern float32_t _cst_87[16384];
extern float32_t _cst_88[4][16384];
extern float32_t _cst_89[4][16384];
extern float32_t _cst_90[256][16384];
extern int32_t _cst_91[1];
extern float32_t _cst_92[2304][8192];
extern float32_t _cst_93[8192];
extern int32_t _cst_94[9];
extern float32_t _cst_95[8192];
extern int32_t _cst_96[1];
extern float32_t _cst_97[4608][8192];
extern float32_t _cst_98[8192];
extern int32_t _cst_99[9];
extern float32_t _cst_100[8192];
extern int32_t _cst_101[1];
extern float32_t _cst_102[4608][8192];
extern float32_t _cst_103[8192];
extern int32_t _cst_104[9];
extern float32_t _cst_105[8192];
extern float32_t _cst_106[8192];
extern float32_t _cst_107[2][8192];
extern float32_t _cst_108[2][8192];
extern float32_t _cst_109[512][8192];
extern int32_t _cst_110[1];
extern float32_t _cst_111[4608][2048];
extern float32_t _cst_112[2048];
extern int32_t _cst_113[9];
extern float32_t _cst_114[2048];
extern int32_t _cst_115[1];
extern float32_t _cst_116[4608][2048];
extern float32_t _cst_117[2048];
extern int32_t _cst_118[9];
extern float32_t _cst_119[2048];
extern int32_t _cst_120[1];
extern float32_t _cst_121[4608][2048];
extern float32_t _cst_122[2048];
extern int32_t _cst_123[9];
extern float32_t _cst_124[2048];
extern float32_t _cst_125[2048];
extern float32_t _cst_126[2048];
extern float32_t _cst_127[512][2048];
extern int32_t _cst_128[1];
extern float32_t _cst_129[8][1040];
extern int32_t _cst_130[2];
extern float32_t _cst_131[10];
float32_t _cst_132 = -1;
float32_t _cst_133 = -1;
float64_t _cst_134 = -3.809199432496630510769;
float32_t _cst_135 = -3.809199432496630510769;
float64_t _cst_136 = 2.460940868770910228136;
float32_t _cst_137 = 2.460940868770910228136;
float64_t _cst_138 = -1.430304794543664215212;
float32_t _cst_139 = -1.430304794543664215212;
float64_t _cst_140 = 2.944661935353764636858;
float32_t _cst_141 = 2.944661935353764636858;
float32_t _cst_142 = -1;
float32_t _cst_143 = -1;
float32_t _cst_144 = -1;
float32_t _cst_145 = -1;
float64_t _cst_146 = -0.0001600280537250875771867;
float32_t _cst_147 = -0.0001600280537250875771867;
float64_t _cst_148 = 0.003032478474127557870749;
float32_t _cst_149 = 0.003032478474127557870749;
float64_t _cst_150 = 0.1365828361194337536855;
float32_t _cst_151 = 0.1365828361194337536855;
float64_t _cst_152 = -0.02219735858216702675816;
float64_t _cst_153 = 0.5685231134608953462717;
float32_t _cst_154 = 0.5685231134608953462717;
float64_t _cst_155 = -0.5558785316215136829143;
float64_t _cst_156 = 1.614771079042867985009;
float32_t _cst_157 = 1.614771079042867985009;
float64_t _cst_158 = -0.6164083583294512891371;
float32_t _cst_159 = -1;
float32_t _cst_160 = -1;
float32_t _cst_161 = -1;
float32_t _cst_162 = -1;
float64_t _cst_163 = 0.0001111119162030077897477;
float32_t _cst_164 = 0.0001111119162030077897477;
float64_t _cst_165 = 0.006507995190210730772351;
float32_t _cst_166 = 0.006507995190210730772351;
float64_t _cst_167 = -0.0009371069270347270278171;
float32_t _cst_168 = -0.0009371069270347270278171;
float64_t _cst_169 = 0.1056744718173614761003;
float32_t _cst_170 = 0.1056744718173614761003;
float64_t _cst_171 = -0.03691063940349948585551;
float32_t _cst_172 = -0.03691063940349948585551;
float64_t _cst_173 = 0.6334139843709288841822;
float32_t _cst_174 = 0.6334139843709288841822;
float64_t _cst_175 = -0.2078597158810965850329;
float32_t _cst_176 = -0.2078597158810965850329;
float32_t _cst_177 = 0.5;
float32_t _cst_178 = 0.2000000000000000111022;
float32_t _cst_179 = 0.2000000000000000111022;
float32_t _cst_180 = 0.25;
float32_t _cst_181 = 0.25;
float32_t _cst_182 = 0.25;
float32_t _cst_183 = 0.25;
float32_t _cst_184 = 0.25;
float32_t _cst_185 = 0.25;
float32_t _cst_186 = 0.25;
float32_t _cst_187 = 0.25;
float32_t _cst_188 = 0.25;
float32_t _cst_189 = 0.25;
float32_t _cst_190 = 0.25;
CIPHERTEXT App_relu(CIPHERTEXT input_ct0, CIPHERTEXT input_ct1, uint32_t mask_len);
CIPHERTEXT App_relu_1(CIPHERTEXT input_ct0, CIPHERTEXT input_ct1, uint32_t mask_len);
CIPHERTEXT App_relu_2(CIPHERTEXT input_ct0, CIPHERTEXT input_ct1, uint32_t mask_len);
CIPHERTEXT Rotate(CIPHERTEXT ciph, int32_t rot_idx);
CIPHERTEXT Relinearize(CIPHERTEXT3 ciph);

#ifdef __cplusplus
extern "C"
#endif
bool Main_graph() {
  CIPHERTEXT input_0;
  CIPHERTEXT input_dup_n0_1;
  int32_t bs_input_n0_2;
  CIPHERTEXT input_block_n0_3[9];
  int32_t bs_input_n0_rs2_4;
  CIPHERTEXT result_n1_5;
  int32_t num_grid_n1_6;
  CIPHERTEXT result_grid_n1_7;
  int32_t cap_block_n1_8;
  CIPHERTEXT input_dup_n1_9;
  int32_t bs_input_n1_10;
  CIPHERTEXT input_block_n1_11[9];
  int32_t bs_input_n1_rs2_12;
  CIPHERTEXT result_n2_13;
  int32_t num_grid_n2_14;
  CIPHERTEXT result_grid_n2_15;
  int32_t cap_block_n2_16;
  CIPHERTEXT tmp_row_add2_17;
  CIPHERTEXT orig_result2_18;
  CIPHERTEXT comb_row_result3_19;
  int32_t comb_row_index_n3_20;
  CIPHERTEXT comb_rc_result3_21;
  int32_t comb_rc_index_n3_22;
  CIPHERTEXT ss_result3_23;
  int32_t comb_cc_index_n3_24;
  CIPHERTEXT input_dup_n3_25;
  int32_t bs_input_n3_26;
  CIPHERTEXT input_block_n3_27[9];
  int32_t bs_input_n3_rs2_28;
  CIPHERTEXT result_n4_29;
  int32_t num_grid_n4_30;
  CIPHERTEXT result_grid_n4_31;
  int32_t cap_block_n4_32;
  CIPHERTEXT input_dup_n4_33;
  int32_t bs_input_n4_34;
  CIPHERTEXT input_block_n4_35[9];
  int32_t bs_input_n4_rs2_36;
  CIPHERTEXT result_n5_37;
  int32_t num_grid_n5_38;
  CIPHERTEXT result_grid_n5_39;
  int32_t cap_block_n5_40;
  CIPHERTEXT tmp_row_add5_41;
  CIPHERTEXT orig_result5_42;
  CIPHERTEXT comb_row_result6_43;
  int32_t comb_row_index_n6_44;
  CIPHERTEXT comb_rc_result6_45;
  int32_t comb_rc_index_n6_46;
  CIPHERTEXT ss_result6_47;
  int32_t comb_cc_index_n6_48;
  CIPHERTEXT input_dup_n6_49;
  int32_t bs_input_n6_50;
  CIPHERTEXT input_block_n6_51[9];
  int32_t bs_input_n6_rs2_52;
  CIPHERTEXT result_n7_53;
  int32_t num_grid_n7_54;
  CIPHERTEXT result_grid_n7_55;
  int32_t cap_block_n7_56;
  CIPHERTEXT input_dup_n7_57;
  int32_t bs_input_n7_58;
  CIPHERTEXT input_block_n7_59[9];
  int32_t bs_input_n7_rs2_60;
  CIPHERTEXT result_n8_61;
  int32_t num_grid_n8_62;
  CIPHERTEXT result_grid_n8_63;
  int32_t cap_block_n8_64;
  CIPHERTEXT input_dup_n8_65;
  int32_t bs_input_n8_66;
  CIPHERTEXT input_block_n8_67[9];
  int32_t bs_input_n8_rs2_68;
  CIPHERTEXT result_n9_69;
  int32_t num_grid_n9_70;
  CIPHERTEXT result_grid_n9_71;
  int32_t cap_block_n9_72;
  CIPHERTEXT tmp_row_add9_73;
  CIPHERTEXT orig_result9_74;
  CIPHERTEXT comb_row_result10_75;
  int32_t comb_row_index_n10_76;
  CIPHERTEXT comb_rc_result10_77;
  int32_t comb_rc_index_n10_78;
  CIPHERTEXT ss_result10_79;
  int32_t comb_cc_index_n10_80;
  CIPHERTEXT input_dup_n10_81;
  int32_t bs_input_n10_82;
  CIPHERTEXT input_block_n10_83[9];
  int32_t bs_input_n10_rs2_84;
  CIPHERTEXT result_n11_85;
  int32_t num_grid_n11_86;
  CIPHERTEXT result_grid_n11_87;
  int32_t cap_block_n11_88;
  CIPHERTEXT input_dup_n11_89;
  int32_t bs_input_n11_90;
  CIPHERTEXT input_block_n11_91[9];
  int32_t bs_input_n11_rs2_92;
  CIPHERTEXT result_n12_93;
  int32_t num_grid_n12_94;
  CIPHERTEXT result_grid_n12_95;
  int32_t cap_block_n12_96;
  CIPHERTEXT input_dup_n12_97;
  int32_t bs_input_n12_98;
  CIPHERTEXT input_block_n12_99[9];
  int32_t bs_input_n12_rs2_100;
  CIPHERTEXT result_n13_101;
  int32_t num_grid_n13_102;
  CIPHERTEXT result_grid_n13_103;
  int32_t cap_block_n13_104;
  CIPHERTEXT tmp_row_add13_105;
  CIPHERTEXT orig_result13_106;
  CIPHERTEXT comb_row_result14_107;
  int32_t comb_row_index_n14_108;
  CIPHERTEXT comb_rc_result14_109;
  int32_t comb_rc_index_n14_110;
  CIPHERTEXT ss_result14_111;
  int32_t comb_cc_index_n14_112;
  CIPHERTEXT input_dup_n14_113;
  int32_t bs_input_n14_114;
  CIPHERTEXT input_block_n14_115[9];
  int32_t bs_input_n14_rs2_116;
  CIPHERTEXT result_n15_117;
  int32_t num_grid_n15_118;
  CIPHERTEXT result_grid_n15_119;
  int32_t cap_block_n15_120;
  CIPHERTEXT input_dup_n15_121;
  int32_t bs_input_n15_122;
  CIPHERTEXT input_block_n15_123[9];
  int32_t bs_input_n15_rs2_124;
  CIPHERTEXT result_n16_125;
  int32_t num_grid_n16_126;
  CIPHERTEXT result_grid_n16_127;
  int32_t cap_block_n16_128;
  CIPHERTEXT input_dup_n16_129;
  int32_t bs_input_n16_130;
  CIPHERTEXT input_block_n16_131[9];
  int32_t bs_input_n16_rs2_132;
  CIPHERTEXT result_n17_133;
  int32_t num_grid_n17_134;
  CIPHERTEXT result_grid_n17_135;
  int32_t cap_block_n17_136;
  CIPHERTEXT tmp_row_add17_137;
  CIPHERTEXT orig_result17_138;
  CIPHERTEXT ss_result18_139;
  int32_t comb_cc_index_n18_140;
  CIPHERTEXT input_dup_n18_141;
  CIPHERTEXT input_block_n18_142[2];
  int32_t bs_input_n18_143;
  CIPHERTEXT gemm_result_n19_144;
  int32_t gs_n19_145;
  CIPHERTEXT block_result_146;
  int32_t bs_n19_147;
  CIPHERTEXT reduce_intra_148;
  int32_t ri_n19_149;
  CIPHERTEXT reduce_intra_150;
  int32_t ri_n19_151;
  CIPHERTEXT output_152;
  CIPHERTEXT _ckks_gen_tmp_148_153;
  CIPHERTEXT _ckks_gen_tmp_313_154;
  CIPHERTEXT _ckks_gen_tmp_412_155;
  CIPHERTEXT _ckks_gen_tmp_444_156;
  CIPHERTEXT _ckks_gen_tmp_478_157;
  CIPHERTEXT _ckks_gen_tmp_512_158;
  CIPHERTEXT _ckks_gen_tmp_622_159;
  CIPHERTEXT _ckks_gen_tmp_658_160;
  CIPHERTEXT _ckks_gen_tmp_793_161;
  CIPHERTEXT _ckks_gen_tmp_829_162;
  CIPHERTEXT _ckks_gen_tmp_898_163;
  CIPHERTEXT _ckks_gen_tmp_930_164;
  CIPHERTEXT _ckks_gen_tmp_964_165;
  CIPHERTEXT _ckks_gen_tmp_998_166;
  CIPHERTEXT _ckks_gen_tmp_1108_167;
  CIPHERTEXT _ckks_gen_tmp_1144_168;
  CIPHERTEXT _ckks_gen_tmp_1279_169;
  CIPHERTEXT _ckks_gen_tmp_1315_170;
  CIPHERTEXT _ckks_gen_tmp_1450_171;
  CIPHERTEXT _ckks_gen_tmp_1486_172;
  CIPHERTEXT _ckks_gen_tmp_1555_173;
  CIPHERTEXT _ckks_gen_tmp_1587_174;
  CIPHERTEXT _ckks_gen_tmp_1621_175;
  CIPHERTEXT _ckks_gen_tmp_1655_176;
  CIPHERTEXT _ckks_gen_tmp_1765_177;
  CIPHERTEXT _ckks_gen_tmp_1801_178;
  CIPHERTEXT _ckks_gen_tmp_1936_179;
  CIPHERTEXT _ckks_gen_tmp_1972_180;
  CIPHERTEXT _ckks_gen_tmp_2107_181;
  CIPHERTEXT _ckks_gen_tmp_2143_182;
  CIPHERTEXT _ckks_gen_tmp_2212_183;
  CIPHERTEXT _ckks_gen_tmp_2244_184;
  CIPHERTEXT _ckks_gen_tmp_2278_185;
  CIPHERTEXT _ckks_gen_tmp_2312_186;
  CIPHERTEXT _ckks_gen_tmp_2418_187;
  CIPHERTEXT _ckks_gen_tmp_2454_188;
  CIPHERTEXT _ckks_gen_tmp_2589_189;
  CIPHERTEXT _ckks_gen_tmp_2625_190;
  CIPHERTEXT _ckks_gen_tmp_2760_191;
  CIPHERTEXT _ckks_gen_tmp_2796_192;
  CIPHERTEXT _ckks_gen_tmp_2865_193;
  CIPHERTEXT _ckks_gen_tmp_2908_194;
  CIPHERTEXT _ckks_gen_tmp_3012_195;
  MODULUS* _pgen_modulus_196;
  uint32_t _pgen_num_q_197;
  uint32_t _pgen_rns_idx_198;
  CIPHERTEXT _pgen_tmp_0_199;
  CIPHERTEXT _pgen_tmp_1_200;
  CIPHERTEXT _pgen_tmp_2_201;
  CIPHERTEXT _pgen_tmp_3_202;
  CIPHERTEXT _pgen_tmp_4_203;
  CIPHERTEXT _pgen_tmp_5_204;
  CIPHERTEXT _pgen_tmp_6_205;
  CIPHERTEXT _pgen_tmp_7_206;
  CIPHERTEXT _pgen_tmp_8_207;
  CIPHERTEXT _pgen_tmp_9_208;
  CIPHERTEXT _pgen_tmp_10_209;
  CIPHERTEXT _pgen_tmp_11_210;
  CIPHERTEXT _pgen_tmp_12_211;
  CIPHERTEXT _pgen_tmp_13_212;
  PLAINTEXT _pgen_tmp_14_213;
  CIPHERTEXT _pgen_tmp_15_214;
  CIPHERTEXT _pgen_tmp_16_215;
  PLAINTEXT _pgen_tmp_17_216;
  PLAINTEXT _pgen_tmp_18_217;
  CIPHERTEXT _pgen_tmp_19_218;
  PLAINTEXT _pgen_tmp_20_219;
  PLAINTEXT _pgen_tmp_21_220;
  CIPHERTEXT _pgen_tmp_22_221;
  CIPHERTEXT _pgen_tmp_23_222;
  CIPHERTEXT _pgen_tmp_24_223;
  PLAINTEXT _pgen_tmp_25_224;
  PLAINTEXT _pgen_tmp_26_225;
  CIPHERTEXT _pgen_tmp_27_226;
  PLAINTEXT _pgen_tmp_28_227;
  CIPHERTEXT _pgen_tmp_29_228;
  PLAINTEXT _pgen_tmp_30_229;
  PLAINTEXT _pgen_tmp_31_230;
  CIPHERTEXT _pgen_tmp_32_231;
  PLAINTEXT _pgen_tmp_33_232;
  CIPHERTEXT _pgen_tmp_34_233;
  PLAINTEXT _pgen_tmp_35_234;
  CIPHERTEXT _pgen_tmp_36_235;
  PLAINTEXT _pgen_tmp_37_236;
  CIPHERTEXT _pgen_tmp_38_237;
  CIPHERTEXT _pgen_tmp_39_238;
  PLAINTEXT _pgen_tmp_40_239;
  PLAINTEXT _pgen_tmp_41_240;
  CIPHERTEXT _pgen_tmp_42_241;
  PLAINTEXT _pgen_tmp_43_242;
  PLAINTEXT _pgen_tmp_44_243;
  CIPHERTEXT _pgen_tmp_45_244;
  CIPHERTEXT _pgen_tmp_46_245;
  PLAINTEXT _pgen_tmp_47_246;
  PLAINTEXT _pgen_tmp_48_247;
  CIPHERTEXT _pgen_tmp_49_248;
  PLAINTEXT _pgen_tmp_50_249;
  CIPHERTEXT _pgen_tmp_51_250;
  PLAINTEXT _pgen_tmp_52_251;
  PLAINTEXT _pgen_tmp_53_252;
  CIPHERTEXT _pgen_tmp_54_253;
  PLAINTEXT _pgen_tmp_55_254;
  CIPHERTEXT _pgen_tmp_56_255;
  PLAINTEXT _pgen_tmp_57_256;
  CIPHERTEXT _pgen_tmp_58_257;
  PLAINTEXT _pgen_tmp_59_258;
  CIPHERTEXT _pgen_tmp_60_259;
  CIPHERTEXT _pgen_tmp_61_260;
  PLAINTEXT _pgen_tmp_62_261;
  PLAINTEXT _pgen_tmp_63_262;
  CIPHERTEXT _pgen_tmp_64_263;
  PLAINTEXT _pgen_tmp_65_264;
  PLAINTEXT _pgen_tmp_66_265;
  CIPHERTEXT _pgen_tmp_67_266;
  CIPHERTEXT _pgen_tmp_68_267;
  PLAINTEXT _pgen_tmp_69_268;
  PLAINTEXT _pgen_tmp_70_269;
  CIPHERTEXT _pgen_tmp_71_270;
  PLAINTEXT _pgen_tmp_72_271;
  PLAINTEXT _pgen_tmp_73_272;
  CIPHERTEXT _pgen_tmp_74_273;
  CIPHERTEXT _pgen_tmp_75_274;
  PLAINTEXT _pgen_tmp_76_275;
  PLAINTEXT _pgen_tmp_77_276;
  CIPHERTEXT _pgen_tmp_78_277;
  PLAINTEXT _pgen_tmp_79_278;
  CIPHERTEXT _pgen_tmp_80_279;
  PLAINTEXT _pgen_tmp_81_280;
  PLAINTEXT _pgen_tmp_82_281;
  CIPHERTEXT _pgen_tmp_83_282;
  PLAINTEXT _pgen_tmp_84_283;
  CIPHERTEXT _pgen_tmp_85_284;
  PLAINTEXT _pgen_tmp_86_285;
  CIPHERTEXT _pgen_tmp_87_286;
  PLAINTEXT _pgen_tmp_88_287;
  CIPHERTEXT _pgen_tmp_89_288;
  CIPHERTEXT _pgen_tmp_90_289;
  PLAINTEXT _pgen_tmp_91_290;
  PLAINTEXT _pgen_tmp_92_291;
  CIPHERTEXT _pgen_tmp_93_292;
  PLAINTEXT _pgen_tmp_94_293;
  PLAINTEXT _pgen_tmp_95_294;
  CIPHERTEXT _pgen_tmp_96_295;
  CIPHERTEXT _pgen_tmp_97_296;
  PLAINTEXT _pgen_tmp_98_297;
  PLAINTEXT _pgen_tmp_99_298;
  CIPHERTEXT _pgen_tmp_100_299;
  PLAINTEXT _pgen_tmp_101_300;
  PLAINTEXT _pgen_tmp_102_301;
  CIPHERTEXT _pgen_tmp_103_302;
  CIPHERTEXT _pgen_tmp_104_303;
  PLAINTEXT _pgen_tmp_105_304;
  PLAINTEXT _pgen_tmp_106_305;
  CIPHERTEXT _pgen_tmp_107_306;
  PLAINTEXT _pgen_tmp_108_307;
  CIPHERTEXT _pgen_tmp_109_308;
  PLAINTEXT _pgen_tmp_110_309;
  PLAINTEXT _pgen_tmp_111_310;
  CIPHERTEXT _pgen_tmp_112_311;
  PLAINTEXT _pgen_tmp_113_312;
  CIPHERTEXT _pgen_tmp_114_313;
  PLAINTEXT _pgen_tmp_115_314;
  CIPHERTEXT _pgen_tmp_116_315;
  PLAINTEXT _pgen_tmp_117_316;
  CIPHERTEXT _pgen_tmp_118_317;
  CIPHERTEXT _pgen_tmp_119_318;
  PLAINTEXT _pgen_tmp_120_319;
  PLAINTEXT _pgen_tmp_121_320;
  CIPHERTEXT _pgen_tmp_122_321;
  PLAINTEXT _pgen_tmp_123_322;
  PLAINTEXT _pgen_tmp_124_323;
  CIPHERTEXT _pgen_tmp_125_324;
  CIPHERTEXT _pgen_tmp_126_325;
  PLAINTEXT _pgen_tmp_127_326;
  PLAINTEXT _pgen_tmp_128_327;
  CIPHERTEXT _pgen_tmp_129_328;
  PLAINTEXT _pgen_tmp_130_329;
  PLAINTEXT _pgen_tmp_131_330;
  CIPHERTEXT _pgen_tmp_132_331;
  CIPHERTEXT _pgen_tmp_133_332;
  PLAINTEXT _pgen_tmp_134_333;
  PLAINTEXT _pgen_tmp_135_334;
  CIPHERTEXT _pgen_tmp_136_335;
  PLAINTEXT _pgen_tmp_137_336;
  CIPHERTEXT _pgen_tmp_138_337;
  PLAINTEXT _pgen_tmp_139_338;
  CIPHERTEXT _pgen_tmp_140_339;
  PLAINTEXT _pgen_tmp_141_340;
  PLAINTEXT _pgen_tmp_142_341;
  CIPHERTEXT _pgen_tmp_143_342;
  CIPHERTEXT _pgen_tmp_144_343;
  PLAINTEXT _pgen_tmp_145_344;
  CIPHERTEXT _pgen_tmp_146_345;
  PLAINTEXT _pgen_tmp_147_346;
  CIPHERTEXT _pgen_tmp_148_347;
  PLAINTEXT _pgen_tmp_149_348;
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
  CIPHERTEXT _preg_268435467;
  CIPHERTEXT _preg_268435468;
  CIPHERTEXT _preg_268435469;
  CIPHERTEXT _preg_268435470;
  CIPHERTEXT _preg_268435471;
  CIPHERTEXT _preg_268435472;
  CIPHERTEXT _preg_268435473;
  CIPHERTEXT _preg_268435474;
  CIPHERTEXT _preg_268435475;
  CIPHERTEXT _preg_268435476;
  CIPHERTEXT _preg_268435477;
  CIPHERTEXT _preg_268435478;
  CIPHERTEXT _preg_268435479;
  CIPHERTEXT _preg_268435480;
  CIPHERTEXT _preg_268435481;
  CIPHERTEXT _preg_268435482;
  CIPHERTEXT _preg_268435483;
  CIPHERTEXT _preg_268435484;
  CIPHERTEXT _preg_268435485;
  CIPHERTEXT _preg_268435486;
  CIPHERTEXT _preg_268435487;
  CIPHERTEXT _preg_268435488;
  CIPHERTEXT _preg_268435489;
  CIPHERTEXT _preg_268435490;
  CIPHERTEXT _preg_268435491;
  CIPHERTEXT _preg_268435492;
  CIPHERTEXT _preg_268435493;
  CIPHERTEXT _preg_268435494;
  CIPHERTEXT _preg_268435495;
  CIPHERTEXT _preg_268435496;
  CIPHERTEXT _preg_268435497;
  CIPHERTEXT _preg_268435498;
  CIPHERTEXT _preg_268435499;
  CIPHERTEXT _preg_268435500;
  CIPHERTEXT _preg_268435501;
  CIPHERTEXT _preg_268435502;
  CIPHERTEXT _preg_268435503;
  CIPHERTEXT _preg_268435504;
  CIPHERTEXT _preg_268435505;
  CIPHERTEXT _preg_268435506;
  CIPHERTEXT _preg_268435507;
  CIPHERTEXT _preg_268435508;
  CIPHERTEXT _preg_268435509;
  CIPHERTEXT _preg_268435510;
  CIPHERTEXT _preg_268435511;
  CIPHERTEXT _preg_268435512;
  CIPHERTEXT _preg_268435513;
  CIPHERTEXT _preg_268435514;
  CIPHERTEXT _preg_268435515;
  CIPHERTEXT _preg_268435516;
  CIPHERTEXT _preg_268435517;
  CIPHERTEXT _preg_268435518;
  CIPHERTEXT _preg_268435519;
  CIPHERTEXT _preg_268435520;
  CIPHERTEXT _preg_268435521;
  CIPHERTEXT _preg_268435522;
  CIPHERTEXT _preg_268435523;
  CIPHERTEXT _preg_268435524;
  CIPHERTEXT _preg_268435525;
  CIPHERTEXT _preg_268435526;
  CIPHERTEXT _preg_268435527;
  CIPHERTEXT _preg_268435528;
  CIPHERTEXT _preg_268435529;
  CIPHERTEXT _preg_268435530;
  CIPHERTEXT _preg_268435531;
  CIPHERTEXT _preg_268435532;
  CIPHERTEXT _preg_268435533;
  CIPHERTEXT _preg_268435534;
  CIPHERTEXT _preg_268435535;
  CIPHERTEXT _preg_268435536;
  CIPHERTEXT _preg_268435537;
  CIPHERTEXT _preg_268435538;
  CIPHERTEXT _preg_268435539;
  CIPHERTEXT _preg_268435540;
  CIPHERTEXT _preg_268435541;
  CIPHERTEXT _preg_268435542;
  CIPHERTEXT _preg_268435543;
  CIPHERTEXT _preg_268435544;
  CIPHERTEXT _preg_268435545;
  CIPHERTEXT _preg_268435546;
  CIPHERTEXT _preg_268435547;
  CIPHERTEXT _preg_268435548;
  CIPHERTEXT _preg_268435549;
  CIPHERTEXT _preg_268435550;
  CIPHERTEXT _preg_268435551;
  CIPHERTEXT _preg_268435552;
  CIPHERTEXT _preg_268435553;
  CIPHERTEXT _preg_268435554;
  CIPHERTEXT _preg_268435555;
  CIPHERTEXT _preg_268435556;
  CIPHERTEXT _preg_268435557;
  CIPHERTEXT _preg_268435558;
  CIPHERTEXT _preg_268435559;
  CIPHERTEXT _preg_268435560;
  CIPHERTEXT _preg_268435561;
  CIPHERTEXT _preg_268435562;
  CIPHERTEXT _preg_268435563;
  CIPHERTEXT _preg_268435564;
  CIPHERTEXT _preg_268435565;
  CIPHERTEXT _preg_268435566;
  CIPHERTEXT _preg_268435567;
  CIPHERTEXT _preg_268435568;
  CIPHERTEXT _preg_268435569;
  CIPHERTEXT _preg_268435570;
  CIPHERTEXT _preg_268435571;
  CIPHERTEXT _preg_268435572;
  CIPHERTEXT _preg_268435573;
  CIPHERTEXT _preg_268435574;
  CIPHERTEXT _preg_268435575;
  CIPHERTEXT _preg_268435576;
  CIPHERTEXT _preg_268435577;
  CIPHERTEXT _preg_268435578;
  CIPHERTEXT _preg_268435579;
  CIPHERTEXT _preg_268435580;
  CIPHERTEXT _preg_268435581;
  CIPHERTEXT _preg_268435582;
  CIPHERTEXT _preg_268435583;
  CIPHERTEXT _preg_268435584;
  CIPHERTEXT _preg_268435585;
  CIPHERTEXT _preg_268435586;
  CIPHERTEXT _preg_268435587;
  CIPHERTEXT _preg_268435588;
  CIPHERTEXT _preg_268435589;
  CIPHERTEXT _preg_268435590;
  CIPHERTEXT _preg_268435591;
  CIPHERTEXT _preg_268435592;
  CIPHERTEXT _preg_268435593;
  CIPHERTEXT _preg_268435594;
  CIPHERTEXT _preg_268435595;
  CIPHERTEXT _preg_268435596;
  CIPHERTEXT _preg_268435597;
  CIPHERTEXT _preg_268435598;
  CIPHERTEXT _preg_268435599;
  CIPHERTEXT _preg_268435600;
  CIPHERTEXT _preg_268435601;
  CIPHERTEXT _preg_268435602;
  CIPHERTEXT _preg_268435603;
  CIPHERTEXT _preg_268435604;
  CIPHERTEXT _preg_268435605;
  CIPHERTEXT _preg_268435606;
  CIPHERTEXT _preg_268435607;
  CIPHERTEXT _preg_268435608;
  CIPHERTEXT _preg_268435609;
  CIPHERTEXT _preg_268435610;
  CIPHERTEXT _preg_268435611;
  CIPHERTEXT _preg_268435612;
  CIPHERTEXT _preg_268435613;
  CIPHERTEXT _preg_268435614;
  CIPHERTEXT _preg_268435615;
  CIPHERTEXT _preg_268435616;
  CIPHERTEXT _preg_268435617;
  CIPHERTEXT _preg_268435618;
  CIPHERTEXT _preg_268435619;
  CIPHERTEXT _preg_268435620;
  CIPHERTEXT _preg_268435621;
  CIPHERTEXT _preg_268435622;
  CIPHERTEXT _preg_268435623;
  CIPHERTEXT _preg_268435624;
  CIPHERTEXT _preg_268435625;
  CIPHERTEXT _preg_268435626;
  CIPHERTEXT _preg_268435627;
  CIPHERTEXT _preg_268435628;
  CIPHERTEXT _preg_268435629;
  CIPHERTEXT _preg_268435630;
  CIPHERTEXT _preg_268435631;
  CIPHERTEXT _preg_268435632;
  CIPHERTEXT _preg_268435633;
  CIPHERTEXT _preg_268435634;
  CIPHERTEXT _preg_268435635;
  CIPHERTEXT _preg_268435636;
  CIPHERTEXT _preg_268435637;
  CIPHERTEXT _preg_268435638;
  CIPHERTEXT _preg_268435639;
  CIPHERTEXT _preg_268435640;
  CIPHERTEXT _preg_268435641;
  CIPHERTEXT _preg_268435642;
  CIPHERTEXT _preg_268435643;
  CIPHERTEXT _preg_268435644;
  CIPHERTEXT _preg_268435645;
  CIPHERTEXT _preg_268435646;
  CIPHERTEXT _preg_268435647;
  CIPHERTEXT _preg_268435648;
  CIPHERTEXT _preg_268435649;
  CIPHERTEXT _preg_268435650;
  CIPHERTEXT _preg_268435651;
  CIPHERTEXT _preg_268435652;
  CIPHERTEXT _preg_268435653;
  CIPHERTEXT _preg_268435654;
  CIPHERTEXT _preg_268435655;
  CIPHERTEXT _preg_268435656;
  CIPHERTEXT _preg_268435657;
  CIPHERTEXT _preg_268435658;
  CIPHERTEXT _preg_268435659;
  CIPHERTEXT _preg_268435660;
  CIPHERTEXT _preg_268435661;
  CIPHERTEXT _preg_268435662;
  CIPHERTEXT _preg_268435663;
  CIPHERTEXT _preg_268435664;
  CIPHERTEXT _preg_268435665;
  CIPHERTEXT _preg_268435666;
  CIPHERTEXT _preg_268435667;
  CIPHERTEXT _preg_268435668;
  CIPHERTEXT _preg_268435669;
  CIPHERTEXT _preg_268435670;
  CIPHERTEXT _preg_268435671;
  CIPHERTEXT _preg_268435672;
  CIPHERTEXT _preg_268435673;
  CIPHERTEXT _preg_268435674;
  CIPHERTEXT _preg_268435675;
  CIPHERTEXT _preg_268435676;
  CIPHERTEXT _preg_268435677;
  CIPHERTEXT _preg_268435678;
  CIPHERTEXT _preg_268435679;
  CIPHERTEXT _preg_268435680;
  CIPHERTEXT _preg_268435681;
  CIPHERTEXT _preg_268435682;
  CIPHERTEXT _preg_268435683;
  CIPHERTEXT _preg_268435684;
  CIPHERTEXT _preg_268435685;
  CIPHERTEXT _preg_268435686;
  CIPHERTEXT _preg_268435687;
  CIPHERTEXT _preg_268435688;
  CIPHERTEXT _preg_268435689;
  uint32_t  degree = Degree();
  input_0 = Get_input_data("input", 0);
  memset(&input_dup_n0_1, 0, sizeof(input_dup_n0_1));
  memset(&input_block_n0_3, 0, sizeof(input_block_n0_3));
  memset(&result_n1_5, 0, sizeof(result_n1_5));
  memset(&result_grid_n1_7, 0, sizeof(result_grid_n1_7));
  memset(&input_dup_n1_9, 0, sizeof(input_dup_n1_9));
  memset(&input_block_n1_11, 0, sizeof(input_block_n1_11));
  memset(&result_n2_13, 0, sizeof(result_n2_13));
  memset(&result_grid_n2_15, 0, sizeof(result_grid_n2_15));
  memset(&tmp_row_add2_17, 0, sizeof(tmp_row_add2_17));
  memset(&orig_result2_18, 0, sizeof(orig_result2_18));
  memset(&comb_row_result3_19, 0, sizeof(comb_row_result3_19));
  memset(&comb_rc_result3_21, 0, sizeof(comb_rc_result3_21));
  memset(&ss_result3_23, 0, sizeof(ss_result3_23));
  memset(&input_dup_n3_25, 0, sizeof(input_dup_n3_25));
  memset(&input_block_n3_27, 0, sizeof(input_block_n3_27));
  memset(&result_n4_29, 0, sizeof(result_n4_29));
  memset(&result_grid_n4_31, 0, sizeof(result_grid_n4_31));
  memset(&input_dup_n4_33, 0, sizeof(input_dup_n4_33));
  memset(&input_block_n4_35, 0, sizeof(input_block_n4_35));
  memset(&result_n5_37, 0, sizeof(result_n5_37));
  memset(&result_grid_n5_39, 0, sizeof(result_grid_n5_39));
  memset(&tmp_row_add5_41, 0, sizeof(tmp_row_add5_41));
  memset(&orig_result5_42, 0, sizeof(orig_result5_42));
  memset(&comb_row_result6_43, 0, sizeof(comb_row_result6_43));
  memset(&comb_rc_result6_45, 0, sizeof(comb_rc_result6_45));
  memset(&ss_result6_47, 0, sizeof(ss_result6_47));
  memset(&input_dup_n6_49, 0, sizeof(input_dup_n6_49));
  memset(&input_block_n6_51, 0, sizeof(input_block_n6_51));
  memset(&result_n7_53, 0, sizeof(result_n7_53));
  memset(&result_grid_n7_55, 0, sizeof(result_grid_n7_55));
  memset(&input_dup_n7_57, 0, sizeof(input_dup_n7_57));
  memset(&input_block_n7_59, 0, sizeof(input_block_n7_59));
  memset(&result_n8_61, 0, sizeof(result_n8_61));
  memset(&result_grid_n8_63, 0, sizeof(result_grid_n8_63));
  memset(&input_dup_n8_65, 0, sizeof(input_dup_n8_65));
  memset(&input_block_n8_67, 0, sizeof(input_block_n8_67));
  memset(&result_n9_69, 0, sizeof(result_n9_69));
  memset(&result_grid_n9_71, 0, sizeof(result_grid_n9_71));
  memset(&tmp_row_add9_73, 0, sizeof(tmp_row_add9_73));
  memset(&orig_result9_74, 0, sizeof(orig_result9_74));
  memset(&comb_row_result10_75, 0, sizeof(comb_row_result10_75));
  memset(&comb_rc_result10_77, 0, sizeof(comb_rc_result10_77));
  memset(&ss_result10_79, 0, sizeof(ss_result10_79));
  memset(&input_dup_n10_81, 0, sizeof(input_dup_n10_81));
  memset(&input_block_n10_83, 0, sizeof(input_block_n10_83));
  memset(&result_n11_85, 0, sizeof(result_n11_85));
  memset(&result_grid_n11_87, 0, sizeof(result_grid_n11_87));
  memset(&input_dup_n11_89, 0, sizeof(input_dup_n11_89));
  memset(&input_block_n11_91, 0, sizeof(input_block_n11_91));
  memset(&result_n12_93, 0, sizeof(result_n12_93));
  memset(&result_grid_n12_95, 0, sizeof(result_grid_n12_95));
  memset(&input_dup_n12_97, 0, sizeof(input_dup_n12_97));
  memset(&input_block_n12_99, 0, sizeof(input_block_n12_99));
  memset(&result_n13_101, 0, sizeof(result_n13_101));
  memset(&result_grid_n13_103, 0, sizeof(result_grid_n13_103));
  memset(&tmp_row_add13_105, 0, sizeof(tmp_row_add13_105));
  memset(&orig_result13_106, 0, sizeof(orig_result13_106));
  memset(&comb_row_result14_107, 0, sizeof(comb_row_result14_107));
  memset(&comb_rc_result14_109, 0, sizeof(comb_rc_result14_109));
  memset(&ss_result14_111, 0, sizeof(ss_result14_111));
  memset(&input_dup_n14_113, 0, sizeof(input_dup_n14_113));
  memset(&input_block_n14_115, 0, sizeof(input_block_n14_115));
  memset(&result_n15_117, 0, sizeof(result_n15_117));
  memset(&result_grid_n15_119, 0, sizeof(result_grid_n15_119));
  memset(&input_dup_n15_121, 0, sizeof(input_dup_n15_121));
  memset(&input_block_n15_123, 0, sizeof(input_block_n15_123));
  memset(&result_n16_125, 0, sizeof(result_n16_125));
  memset(&result_grid_n16_127, 0, sizeof(result_grid_n16_127));
  memset(&input_dup_n16_129, 0, sizeof(input_dup_n16_129));
  memset(&input_block_n16_131, 0, sizeof(input_block_n16_131));
  memset(&result_n17_133, 0, sizeof(result_n17_133));
  memset(&result_grid_n17_135, 0, sizeof(result_grid_n17_135));
  memset(&tmp_row_add17_137, 0, sizeof(tmp_row_add17_137));
  memset(&orig_result17_138, 0, sizeof(orig_result17_138));
  memset(&ss_result18_139, 0, sizeof(ss_result18_139));
  memset(&input_dup_n18_141, 0, sizeof(input_dup_n18_141));
  memset(&input_block_n18_142, 0, sizeof(input_block_n18_142));
  memset(&gemm_result_n19_144, 0, sizeof(gemm_result_n19_144));
  memset(&block_result_146, 0, sizeof(block_result_146));
  memset(&reduce_intra_148, 0, sizeof(reduce_intra_148));
  memset(&reduce_intra_150, 0, sizeof(reduce_intra_150));
  memset(&output_152, 0, sizeof(output_152));
  memset(&_ckks_gen_tmp_148_153, 0, sizeof(_ckks_gen_tmp_148_153));
  memset(&_ckks_gen_tmp_313_154, 0, sizeof(_ckks_gen_tmp_313_154));
  memset(&_ckks_gen_tmp_412_155, 0, sizeof(_ckks_gen_tmp_412_155));
  memset(&_ckks_gen_tmp_444_156, 0, sizeof(_ckks_gen_tmp_444_156));
  memset(&_ckks_gen_tmp_478_157, 0, sizeof(_ckks_gen_tmp_478_157));
  memset(&_ckks_gen_tmp_512_158, 0, sizeof(_ckks_gen_tmp_512_158));
  memset(&_ckks_gen_tmp_622_159, 0, sizeof(_ckks_gen_tmp_622_159));
  memset(&_ckks_gen_tmp_658_160, 0, sizeof(_ckks_gen_tmp_658_160));
  memset(&_ckks_gen_tmp_793_161, 0, sizeof(_ckks_gen_tmp_793_161));
  memset(&_ckks_gen_tmp_829_162, 0, sizeof(_ckks_gen_tmp_829_162));
  memset(&_ckks_gen_tmp_898_163, 0, sizeof(_ckks_gen_tmp_898_163));
  memset(&_ckks_gen_tmp_930_164, 0, sizeof(_ckks_gen_tmp_930_164));
  memset(&_ckks_gen_tmp_964_165, 0, sizeof(_ckks_gen_tmp_964_165));
  memset(&_ckks_gen_tmp_998_166, 0, sizeof(_ckks_gen_tmp_998_166));
  memset(&_ckks_gen_tmp_1108_167, 0, sizeof(_ckks_gen_tmp_1108_167));
  memset(&_ckks_gen_tmp_1144_168, 0, sizeof(_ckks_gen_tmp_1144_168));
  memset(&_ckks_gen_tmp_1279_169, 0, sizeof(_ckks_gen_tmp_1279_169));
  memset(&_ckks_gen_tmp_1315_170, 0, sizeof(_ckks_gen_tmp_1315_170));
  memset(&_ckks_gen_tmp_1450_171, 0, sizeof(_ckks_gen_tmp_1450_171));
  memset(&_ckks_gen_tmp_1486_172, 0, sizeof(_ckks_gen_tmp_1486_172));
  memset(&_ckks_gen_tmp_1555_173, 0, sizeof(_ckks_gen_tmp_1555_173));
  memset(&_ckks_gen_tmp_1587_174, 0, sizeof(_ckks_gen_tmp_1587_174));
  memset(&_ckks_gen_tmp_1621_175, 0, sizeof(_ckks_gen_tmp_1621_175));
  memset(&_ckks_gen_tmp_1655_176, 0, sizeof(_ckks_gen_tmp_1655_176));
  memset(&_ckks_gen_tmp_1765_177, 0, sizeof(_ckks_gen_tmp_1765_177));
  memset(&_ckks_gen_tmp_1801_178, 0, sizeof(_ckks_gen_tmp_1801_178));
  memset(&_ckks_gen_tmp_1936_179, 0, sizeof(_ckks_gen_tmp_1936_179));
  memset(&_ckks_gen_tmp_1972_180, 0, sizeof(_ckks_gen_tmp_1972_180));
  memset(&_ckks_gen_tmp_2107_181, 0, sizeof(_ckks_gen_tmp_2107_181));
  memset(&_ckks_gen_tmp_2143_182, 0, sizeof(_ckks_gen_tmp_2143_182));
  memset(&_ckks_gen_tmp_2212_183, 0, sizeof(_ckks_gen_tmp_2212_183));
  memset(&_ckks_gen_tmp_2244_184, 0, sizeof(_ckks_gen_tmp_2244_184));
  memset(&_ckks_gen_tmp_2278_185, 0, sizeof(_ckks_gen_tmp_2278_185));
  memset(&_ckks_gen_tmp_2312_186, 0, sizeof(_ckks_gen_tmp_2312_186));
  memset(&_ckks_gen_tmp_2418_187, 0, sizeof(_ckks_gen_tmp_2418_187));
  memset(&_ckks_gen_tmp_2454_188, 0, sizeof(_ckks_gen_tmp_2454_188));
  memset(&_ckks_gen_tmp_2589_189, 0, sizeof(_ckks_gen_tmp_2589_189));
  memset(&_ckks_gen_tmp_2625_190, 0, sizeof(_ckks_gen_tmp_2625_190));
  memset(&_ckks_gen_tmp_2760_191, 0, sizeof(_ckks_gen_tmp_2760_191));
  memset(&_ckks_gen_tmp_2796_192, 0, sizeof(_ckks_gen_tmp_2796_192));
  memset(&_ckks_gen_tmp_2865_193, 0, sizeof(_ckks_gen_tmp_2865_193));
  memset(&_ckks_gen_tmp_2908_194, 0, sizeof(_ckks_gen_tmp_2908_194));
  memset(&_ckks_gen_tmp_3012_195, 0, sizeof(_ckks_gen_tmp_3012_195));
  memset(&_pgen_tmp_0_199, 0, sizeof(_pgen_tmp_0_199));
  memset(&_pgen_tmp_1_200, 0, sizeof(_pgen_tmp_1_200));
  memset(&_pgen_tmp_2_201, 0, sizeof(_pgen_tmp_2_201));
  memset(&_pgen_tmp_3_202, 0, sizeof(_pgen_tmp_3_202));
  memset(&_pgen_tmp_4_203, 0, sizeof(_pgen_tmp_4_203));
  memset(&_pgen_tmp_5_204, 0, sizeof(_pgen_tmp_5_204));
  memset(&_pgen_tmp_6_205, 0, sizeof(_pgen_tmp_6_205));
  memset(&_pgen_tmp_7_206, 0, sizeof(_pgen_tmp_7_206));
  memset(&_pgen_tmp_8_207, 0, sizeof(_pgen_tmp_8_207));
  memset(&_pgen_tmp_9_208, 0, sizeof(_pgen_tmp_9_208));
  memset(&_pgen_tmp_10_209, 0, sizeof(_pgen_tmp_10_209));
  memset(&_pgen_tmp_11_210, 0, sizeof(_pgen_tmp_11_210));
  memset(&_pgen_tmp_12_211, 0, sizeof(_pgen_tmp_12_211));
  memset(&_pgen_tmp_13_212, 0, sizeof(_pgen_tmp_13_212));
  memset(&_pgen_tmp_14_213, 0, sizeof(_pgen_tmp_14_213));
  memset(&_pgen_tmp_15_214, 0, sizeof(_pgen_tmp_15_214));
  memset(&_pgen_tmp_16_215, 0, sizeof(_pgen_tmp_16_215));
  memset(&_pgen_tmp_17_216, 0, sizeof(_pgen_tmp_17_216));
  memset(&_pgen_tmp_18_217, 0, sizeof(_pgen_tmp_18_217));
  memset(&_pgen_tmp_19_218, 0, sizeof(_pgen_tmp_19_218));
  memset(&_pgen_tmp_20_219, 0, sizeof(_pgen_tmp_20_219));
  memset(&_pgen_tmp_21_220, 0, sizeof(_pgen_tmp_21_220));
  memset(&_pgen_tmp_22_221, 0, sizeof(_pgen_tmp_22_221));
  memset(&_pgen_tmp_23_222, 0, sizeof(_pgen_tmp_23_222));
  memset(&_pgen_tmp_24_223, 0, sizeof(_pgen_tmp_24_223));
  memset(&_pgen_tmp_25_224, 0, sizeof(_pgen_tmp_25_224));
  memset(&_pgen_tmp_26_225, 0, sizeof(_pgen_tmp_26_225));
  memset(&_pgen_tmp_27_226, 0, sizeof(_pgen_tmp_27_226));
  memset(&_pgen_tmp_28_227, 0, sizeof(_pgen_tmp_28_227));
  memset(&_pgen_tmp_29_228, 0, sizeof(_pgen_tmp_29_228));
  memset(&_pgen_tmp_30_229, 0, sizeof(_pgen_tmp_30_229));
  memset(&_pgen_tmp_31_230, 0, sizeof(_pgen_tmp_31_230));
  memset(&_pgen_tmp_32_231, 0, sizeof(_pgen_tmp_32_231));
  memset(&_pgen_tmp_33_232, 0, sizeof(_pgen_tmp_33_232));
  memset(&_pgen_tmp_34_233, 0, sizeof(_pgen_tmp_34_233));
  memset(&_pgen_tmp_35_234, 0, sizeof(_pgen_tmp_35_234));
  memset(&_pgen_tmp_36_235, 0, sizeof(_pgen_tmp_36_235));
  memset(&_pgen_tmp_37_236, 0, sizeof(_pgen_tmp_37_236));
  memset(&_pgen_tmp_38_237, 0, sizeof(_pgen_tmp_38_237));
  memset(&_pgen_tmp_39_238, 0, sizeof(_pgen_tmp_39_238));
  memset(&_pgen_tmp_40_239, 0, sizeof(_pgen_tmp_40_239));
  memset(&_pgen_tmp_41_240, 0, sizeof(_pgen_tmp_41_240));
  memset(&_pgen_tmp_42_241, 0, sizeof(_pgen_tmp_42_241));
  memset(&_pgen_tmp_43_242, 0, sizeof(_pgen_tmp_43_242));
  memset(&_pgen_tmp_44_243, 0, sizeof(_pgen_tmp_44_243));
  memset(&_pgen_tmp_45_244, 0, sizeof(_pgen_tmp_45_244));
  memset(&_pgen_tmp_46_245, 0, sizeof(_pgen_tmp_46_245));
  memset(&_pgen_tmp_47_246, 0, sizeof(_pgen_tmp_47_246));
  memset(&_pgen_tmp_48_247, 0, sizeof(_pgen_tmp_48_247));
  memset(&_pgen_tmp_49_248, 0, sizeof(_pgen_tmp_49_248));
  memset(&_pgen_tmp_50_249, 0, sizeof(_pgen_tmp_50_249));
  memset(&_pgen_tmp_51_250, 0, sizeof(_pgen_tmp_51_250));
  memset(&_pgen_tmp_52_251, 0, sizeof(_pgen_tmp_52_251));
  memset(&_pgen_tmp_53_252, 0, sizeof(_pgen_tmp_53_252));
  memset(&_pgen_tmp_54_253, 0, sizeof(_pgen_tmp_54_253));
  memset(&_pgen_tmp_55_254, 0, sizeof(_pgen_tmp_55_254));
  memset(&_pgen_tmp_56_255, 0, sizeof(_pgen_tmp_56_255));
  memset(&_pgen_tmp_57_256, 0, sizeof(_pgen_tmp_57_256));
  memset(&_pgen_tmp_58_257, 0, sizeof(_pgen_tmp_58_257));
  memset(&_pgen_tmp_59_258, 0, sizeof(_pgen_tmp_59_258));
  memset(&_pgen_tmp_60_259, 0, sizeof(_pgen_tmp_60_259));
  memset(&_pgen_tmp_61_260, 0, sizeof(_pgen_tmp_61_260));
  memset(&_pgen_tmp_62_261, 0, sizeof(_pgen_tmp_62_261));
  memset(&_pgen_tmp_63_262, 0, sizeof(_pgen_tmp_63_262));
  memset(&_pgen_tmp_64_263, 0, sizeof(_pgen_tmp_64_263));
  memset(&_pgen_tmp_65_264, 0, sizeof(_pgen_tmp_65_264));
  memset(&_pgen_tmp_66_265, 0, sizeof(_pgen_tmp_66_265));
  memset(&_pgen_tmp_67_266, 0, sizeof(_pgen_tmp_67_266));
  memset(&_pgen_tmp_68_267, 0, sizeof(_pgen_tmp_68_267));
  memset(&_pgen_tmp_69_268, 0, sizeof(_pgen_tmp_69_268));
  memset(&_pgen_tmp_70_269, 0, sizeof(_pgen_tmp_70_269));
  memset(&_pgen_tmp_71_270, 0, sizeof(_pgen_tmp_71_270));
  memset(&_pgen_tmp_72_271, 0, sizeof(_pgen_tmp_72_271));
  memset(&_pgen_tmp_73_272, 0, sizeof(_pgen_tmp_73_272));
  memset(&_pgen_tmp_74_273, 0, sizeof(_pgen_tmp_74_273));
  memset(&_pgen_tmp_75_274, 0, sizeof(_pgen_tmp_75_274));
  memset(&_pgen_tmp_76_275, 0, sizeof(_pgen_tmp_76_275));
  memset(&_pgen_tmp_77_276, 0, sizeof(_pgen_tmp_77_276));
  memset(&_pgen_tmp_78_277, 0, sizeof(_pgen_tmp_78_277));
  memset(&_pgen_tmp_79_278, 0, sizeof(_pgen_tmp_79_278));
  memset(&_pgen_tmp_80_279, 0, sizeof(_pgen_tmp_80_279));
  memset(&_pgen_tmp_81_280, 0, sizeof(_pgen_tmp_81_280));
  memset(&_pgen_tmp_82_281, 0, sizeof(_pgen_tmp_82_281));
  memset(&_pgen_tmp_83_282, 0, sizeof(_pgen_tmp_83_282));
  memset(&_pgen_tmp_84_283, 0, sizeof(_pgen_tmp_84_283));
  memset(&_pgen_tmp_85_284, 0, sizeof(_pgen_tmp_85_284));
  memset(&_pgen_tmp_86_285, 0, sizeof(_pgen_tmp_86_285));
  memset(&_pgen_tmp_87_286, 0, sizeof(_pgen_tmp_87_286));
  memset(&_pgen_tmp_88_287, 0, sizeof(_pgen_tmp_88_287));
  memset(&_pgen_tmp_89_288, 0, sizeof(_pgen_tmp_89_288));
  memset(&_pgen_tmp_90_289, 0, sizeof(_pgen_tmp_90_289));
  memset(&_pgen_tmp_91_290, 0, sizeof(_pgen_tmp_91_290));
  memset(&_pgen_tmp_92_291, 0, sizeof(_pgen_tmp_92_291));
  memset(&_pgen_tmp_93_292, 0, sizeof(_pgen_tmp_93_292));
  memset(&_pgen_tmp_94_293, 0, sizeof(_pgen_tmp_94_293));
  memset(&_pgen_tmp_95_294, 0, sizeof(_pgen_tmp_95_294));
  memset(&_pgen_tmp_96_295, 0, sizeof(_pgen_tmp_96_295));
  memset(&_pgen_tmp_97_296, 0, sizeof(_pgen_tmp_97_296));
  memset(&_pgen_tmp_98_297, 0, sizeof(_pgen_tmp_98_297));
  memset(&_pgen_tmp_99_298, 0, sizeof(_pgen_tmp_99_298));
  memset(&_pgen_tmp_100_299, 0, sizeof(_pgen_tmp_100_299));
  memset(&_pgen_tmp_101_300, 0, sizeof(_pgen_tmp_101_300));
  memset(&_pgen_tmp_102_301, 0, sizeof(_pgen_tmp_102_301));
  memset(&_pgen_tmp_103_302, 0, sizeof(_pgen_tmp_103_302));
  memset(&_pgen_tmp_104_303, 0, sizeof(_pgen_tmp_104_303));
  memset(&_pgen_tmp_105_304, 0, sizeof(_pgen_tmp_105_304));
  memset(&_pgen_tmp_106_305, 0, sizeof(_pgen_tmp_106_305));
  memset(&_pgen_tmp_107_306, 0, sizeof(_pgen_tmp_107_306));
  memset(&_pgen_tmp_108_307, 0, sizeof(_pgen_tmp_108_307));
  memset(&_pgen_tmp_109_308, 0, sizeof(_pgen_tmp_109_308));
  memset(&_pgen_tmp_110_309, 0, sizeof(_pgen_tmp_110_309));
  memset(&_pgen_tmp_111_310, 0, sizeof(_pgen_tmp_111_310));
  memset(&_pgen_tmp_112_311, 0, sizeof(_pgen_tmp_112_311));
  memset(&_pgen_tmp_113_312, 0, sizeof(_pgen_tmp_113_312));
  memset(&_pgen_tmp_114_313, 0, sizeof(_pgen_tmp_114_313));
  memset(&_pgen_tmp_115_314, 0, sizeof(_pgen_tmp_115_314));
  memset(&_pgen_tmp_116_315, 0, sizeof(_pgen_tmp_116_315));
  memset(&_pgen_tmp_117_316, 0, sizeof(_pgen_tmp_117_316));
  memset(&_pgen_tmp_118_317, 0, sizeof(_pgen_tmp_118_317));
  memset(&_pgen_tmp_119_318, 0, sizeof(_pgen_tmp_119_318));
  memset(&_pgen_tmp_120_319, 0, sizeof(_pgen_tmp_120_319));
  memset(&_pgen_tmp_121_320, 0, sizeof(_pgen_tmp_121_320));
  memset(&_pgen_tmp_122_321, 0, sizeof(_pgen_tmp_122_321));
  memset(&_pgen_tmp_123_322, 0, sizeof(_pgen_tmp_123_322));
  memset(&_pgen_tmp_124_323, 0, sizeof(_pgen_tmp_124_323));
  memset(&_pgen_tmp_125_324, 0, sizeof(_pgen_tmp_125_324));
  memset(&_pgen_tmp_126_325, 0, sizeof(_pgen_tmp_126_325));
  memset(&_pgen_tmp_127_326, 0, sizeof(_pgen_tmp_127_326));
  memset(&_pgen_tmp_128_327, 0, sizeof(_pgen_tmp_128_327));
  memset(&_pgen_tmp_129_328, 0, sizeof(_pgen_tmp_129_328));
  memset(&_pgen_tmp_130_329, 0, sizeof(_pgen_tmp_130_329));
  memset(&_pgen_tmp_131_330, 0, sizeof(_pgen_tmp_131_330));
  memset(&_pgen_tmp_132_331, 0, sizeof(_pgen_tmp_132_331));
  memset(&_pgen_tmp_133_332, 0, sizeof(_pgen_tmp_133_332));
  memset(&_pgen_tmp_134_333, 0, sizeof(_pgen_tmp_134_333));
  memset(&_pgen_tmp_135_334, 0, sizeof(_pgen_tmp_135_334));
  memset(&_pgen_tmp_136_335, 0, sizeof(_pgen_tmp_136_335));
  memset(&_pgen_tmp_137_336, 0, sizeof(_pgen_tmp_137_336));
  memset(&_pgen_tmp_138_337, 0, sizeof(_pgen_tmp_138_337));
  memset(&_pgen_tmp_139_338, 0, sizeof(_pgen_tmp_139_338));
  memset(&_pgen_tmp_140_339, 0, sizeof(_pgen_tmp_140_339));
  memset(&_pgen_tmp_141_340, 0, sizeof(_pgen_tmp_141_340));
  memset(&_pgen_tmp_142_341, 0, sizeof(_pgen_tmp_142_341));
  memset(&_pgen_tmp_143_342, 0, sizeof(_pgen_tmp_143_342));
  memset(&_pgen_tmp_144_343, 0, sizeof(_pgen_tmp_144_343));
  memset(&_pgen_tmp_145_344, 0, sizeof(_pgen_tmp_145_344));
  memset(&_pgen_tmp_146_345, 0, sizeof(_pgen_tmp_146_345));
  memset(&_pgen_tmp_147_346, 0, sizeof(_pgen_tmp_147_346));
  memset(&_pgen_tmp_148_347, 0, sizeof(_pgen_tmp_148_347));
  memset(&_pgen_tmp_149_348, 0, sizeof(_pgen_tmp_149_348));
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
  memset(&_preg_268435467, 0, sizeof(_preg_268435467));
  memset(&_preg_268435468, 0, sizeof(_preg_268435468));
  memset(&_preg_268435469, 0, sizeof(_preg_268435469));
  memset(&_preg_268435470, 0, sizeof(_preg_268435470));
  memset(&_preg_268435471, 0, sizeof(_preg_268435471));
  memset(&_preg_268435472, 0, sizeof(_preg_268435472));
  memset(&_preg_268435473, 0, sizeof(_preg_268435473));
  memset(&_preg_268435474, 0, sizeof(_preg_268435474));
  memset(&_preg_268435475, 0, sizeof(_preg_268435475));
  memset(&_preg_268435476, 0, sizeof(_preg_268435476));
  memset(&_preg_268435477, 0, sizeof(_preg_268435477));
  memset(&_preg_268435478, 0, sizeof(_preg_268435478));
  memset(&_preg_268435479, 0, sizeof(_preg_268435479));
  memset(&_preg_268435480, 0, sizeof(_preg_268435480));
  memset(&_preg_268435481, 0, sizeof(_preg_268435481));
  memset(&_preg_268435482, 0, sizeof(_preg_268435482));
  memset(&_preg_268435483, 0, sizeof(_preg_268435483));
  memset(&_preg_268435484, 0, sizeof(_preg_268435484));
  memset(&_preg_268435485, 0, sizeof(_preg_268435485));
  memset(&_preg_268435486, 0, sizeof(_preg_268435486));
  memset(&_preg_268435487, 0, sizeof(_preg_268435487));
  memset(&_preg_268435488, 0, sizeof(_preg_268435488));
  memset(&_preg_268435489, 0, sizeof(_preg_268435489));
  memset(&_preg_268435490, 0, sizeof(_preg_268435490));
  memset(&_preg_268435491, 0, sizeof(_preg_268435491));
  memset(&_preg_268435492, 0, sizeof(_preg_268435492));
  memset(&_preg_268435493, 0, sizeof(_preg_268435493));
  memset(&_preg_268435494, 0, sizeof(_preg_268435494));
  memset(&_preg_268435495, 0, sizeof(_preg_268435495));
  memset(&_preg_268435496, 0, sizeof(_preg_268435496));
  memset(&_preg_268435497, 0, sizeof(_preg_268435497));
  memset(&_preg_268435498, 0, sizeof(_preg_268435498));
  memset(&_preg_268435499, 0, sizeof(_preg_268435499));
  memset(&_preg_268435500, 0, sizeof(_preg_268435500));
  memset(&_preg_268435501, 0, sizeof(_preg_268435501));
  memset(&_preg_268435502, 0, sizeof(_preg_268435502));
  memset(&_preg_268435503, 0, sizeof(_preg_268435503));
  memset(&_preg_268435504, 0, sizeof(_preg_268435504));
  memset(&_preg_268435505, 0, sizeof(_preg_268435505));
  memset(&_preg_268435506, 0, sizeof(_preg_268435506));
  memset(&_preg_268435507, 0, sizeof(_preg_268435507));
  memset(&_preg_268435508, 0, sizeof(_preg_268435508));
  memset(&_preg_268435509, 0, sizeof(_preg_268435509));
  memset(&_preg_268435510, 0, sizeof(_preg_268435510));
  memset(&_preg_268435511, 0, sizeof(_preg_268435511));
  memset(&_preg_268435512, 0, sizeof(_preg_268435512));
  memset(&_preg_268435513, 0, sizeof(_preg_268435513));
  memset(&_preg_268435514, 0, sizeof(_preg_268435514));
  memset(&_preg_268435515, 0, sizeof(_preg_268435515));
  memset(&_preg_268435516, 0, sizeof(_preg_268435516));
  memset(&_preg_268435517, 0, sizeof(_preg_268435517));
  memset(&_preg_268435518, 0, sizeof(_preg_268435518));
  memset(&_preg_268435519, 0, sizeof(_preg_268435519));
  memset(&_preg_268435520, 0, sizeof(_preg_268435520));
  memset(&_preg_268435521, 0, sizeof(_preg_268435521));
  memset(&_preg_268435522, 0, sizeof(_preg_268435522));
  memset(&_preg_268435523, 0, sizeof(_preg_268435523));
  memset(&_preg_268435524, 0, sizeof(_preg_268435524));
  memset(&_preg_268435525, 0, sizeof(_preg_268435525));
  memset(&_preg_268435526, 0, sizeof(_preg_268435526));
  memset(&_preg_268435527, 0, sizeof(_preg_268435527));
  memset(&_preg_268435528, 0, sizeof(_preg_268435528));
  memset(&_preg_268435529, 0, sizeof(_preg_268435529));
  memset(&_preg_268435530, 0, sizeof(_preg_268435530));
  memset(&_preg_268435531, 0, sizeof(_preg_268435531));
  memset(&_preg_268435532, 0, sizeof(_preg_268435532));
  memset(&_preg_268435533, 0, sizeof(_preg_268435533));
  memset(&_preg_268435534, 0, sizeof(_preg_268435534));
  memset(&_preg_268435535, 0, sizeof(_preg_268435535));
  memset(&_preg_268435536, 0, sizeof(_preg_268435536));
  memset(&_preg_268435537, 0, sizeof(_preg_268435537));
  memset(&_preg_268435538, 0, sizeof(_preg_268435538));
  memset(&_preg_268435539, 0, sizeof(_preg_268435539));
  memset(&_preg_268435540, 0, sizeof(_preg_268435540));
  memset(&_preg_268435541, 0, sizeof(_preg_268435541));
  memset(&_preg_268435542, 0, sizeof(_preg_268435542));
  memset(&_preg_268435543, 0, sizeof(_preg_268435543));
  memset(&_preg_268435544, 0, sizeof(_preg_268435544));
  memset(&_preg_268435545, 0, sizeof(_preg_268435545));
  memset(&_preg_268435546, 0, sizeof(_preg_268435546));
  memset(&_preg_268435547, 0, sizeof(_preg_268435547));
  memset(&_preg_268435548, 0, sizeof(_preg_268435548));
  memset(&_preg_268435549, 0, sizeof(_preg_268435549));
  memset(&_preg_268435550, 0, sizeof(_preg_268435550));
  memset(&_preg_268435551, 0, sizeof(_preg_268435551));
  memset(&_preg_268435552, 0, sizeof(_preg_268435552));
  memset(&_preg_268435553, 0, sizeof(_preg_268435553));
  memset(&_preg_268435554, 0, sizeof(_preg_268435554));
  memset(&_preg_268435555, 0, sizeof(_preg_268435555));
  memset(&_preg_268435556, 0, sizeof(_preg_268435556));
  memset(&_preg_268435557, 0, sizeof(_preg_268435557));
  memset(&_preg_268435558, 0, sizeof(_preg_268435558));
  memset(&_preg_268435559, 0, sizeof(_preg_268435559));
  memset(&_preg_268435560, 0, sizeof(_preg_268435560));
  memset(&_preg_268435561, 0, sizeof(_preg_268435561));
  memset(&_preg_268435562, 0, sizeof(_preg_268435562));
  memset(&_preg_268435563, 0, sizeof(_preg_268435563));
  memset(&_preg_268435564, 0, sizeof(_preg_268435564));
  memset(&_preg_268435565, 0, sizeof(_preg_268435565));
  memset(&_preg_268435566, 0, sizeof(_preg_268435566));
  memset(&_preg_268435567, 0, sizeof(_preg_268435567));
  memset(&_preg_268435568, 0, sizeof(_preg_268435568));
  memset(&_preg_268435569, 0, sizeof(_preg_268435569));
  memset(&_preg_268435570, 0, sizeof(_preg_268435570));
  memset(&_preg_268435571, 0, sizeof(_preg_268435571));
  memset(&_preg_268435572, 0, sizeof(_preg_268435572));
  memset(&_preg_268435573, 0, sizeof(_preg_268435573));
  memset(&_preg_268435574, 0, sizeof(_preg_268435574));
  memset(&_preg_268435575, 0, sizeof(_preg_268435575));
  memset(&_preg_268435576, 0, sizeof(_preg_268435576));
  memset(&_preg_268435577, 0, sizeof(_preg_268435577));
  memset(&_preg_268435578, 0, sizeof(_preg_268435578));
  memset(&_preg_268435579, 0, sizeof(_preg_268435579));
  memset(&_preg_268435580, 0, sizeof(_preg_268435580));
  memset(&_preg_268435581, 0, sizeof(_preg_268435581));
  memset(&_preg_268435582, 0, sizeof(_preg_268435582));
  memset(&_preg_268435583, 0, sizeof(_preg_268435583));
  memset(&_preg_268435584, 0, sizeof(_preg_268435584));
  memset(&_preg_268435585, 0, sizeof(_preg_268435585));
  memset(&_preg_268435586, 0, sizeof(_preg_268435586));
  memset(&_preg_268435587, 0, sizeof(_preg_268435587));
  memset(&_preg_268435588, 0, sizeof(_preg_268435588));
  memset(&_preg_268435589, 0, sizeof(_preg_268435589));
  memset(&_preg_268435590, 0, sizeof(_preg_268435590));
  memset(&_preg_268435591, 0, sizeof(_preg_268435591));
  memset(&_preg_268435592, 0, sizeof(_preg_268435592));
  memset(&_preg_268435593, 0, sizeof(_preg_268435593));
  memset(&_preg_268435594, 0, sizeof(_preg_268435594));
  memset(&_preg_268435595, 0, sizeof(_preg_268435595));
  memset(&_preg_268435596, 0, sizeof(_preg_268435596));
  memset(&_preg_268435597, 0, sizeof(_preg_268435597));
  memset(&_preg_268435598, 0, sizeof(_preg_268435598));
  memset(&_preg_268435599, 0, sizeof(_preg_268435599));
  memset(&_preg_268435600, 0, sizeof(_preg_268435600));
  memset(&_preg_268435601, 0, sizeof(_preg_268435601));
  memset(&_preg_268435602, 0, sizeof(_preg_268435602));
  memset(&_preg_268435603, 0, sizeof(_preg_268435603));
  memset(&_preg_268435604, 0, sizeof(_preg_268435604));
  memset(&_preg_268435605, 0, sizeof(_preg_268435605));
  memset(&_preg_268435606, 0, sizeof(_preg_268435606));
  memset(&_preg_268435607, 0, sizeof(_preg_268435607));
  memset(&_preg_268435608, 0, sizeof(_preg_268435608));
  memset(&_preg_268435609, 0, sizeof(_preg_268435609));
  memset(&_preg_268435610, 0, sizeof(_preg_268435610));
  memset(&_preg_268435611, 0, sizeof(_preg_268435611));
  memset(&_preg_268435612, 0, sizeof(_preg_268435612));
  memset(&_preg_268435613, 0, sizeof(_preg_268435613));
  memset(&_preg_268435614, 0, sizeof(_preg_268435614));
  memset(&_preg_268435615, 0, sizeof(_preg_268435615));
  memset(&_preg_268435616, 0, sizeof(_preg_268435616));
  memset(&_preg_268435617, 0, sizeof(_preg_268435617));
  memset(&_preg_268435618, 0, sizeof(_preg_268435618));
  memset(&_preg_268435619, 0, sizeof(_preg_268435619));
  memset(&_preg_268435620, 0, sizeof(_preg_268435620));
  memset(&_preg_268435621, 0, sizeof(_preg_268435621));
  memset(&_preg_268435622, 0, sizeof(_preg_268435622));
  memset(&_preg_268435623, 0, sizeof(_preg_268435623));
  memset(&_preg_268435624, 0, sizeof(_preg_268435624));
  memset(&_preg_268435625, 0, sizeof(_preg_268435625));
  memset(&_preg_268435626, 0, sizeof(_preg_268435626));
  memset(&_preg_268435627, 0, sizeof(_preg_268435627));
  memset(&_preg_268435628, 0, sizeof(_preg_268435628));
  memset(&_preg_268435629, 0, sizeof(_preg_268435629));
  memset(&_preg_268435630, 0, sizeof(_preg_268435630));
  memset(&_preg_268435631, 0, sizeof(_preg_268435631));
  memset(&_preg_268435632, 0, sizeof(_preg_268435632));
  memset(&_preg_268435633, 0, sizeof(_preg_268435633));
  memset(&_preg_268435634, 0, sizeof(_preg_268435634));
  memset(&_preg_268435635, 0, sizeof(_preg_268435635));
  memset(&_preg_268435636, 0, sizeof(_preg_268435636));
  memset(&_preg_268435637, 0, sizeof(_preg_268435637));
  memset(&_preg_268435638, 0, sizeof(_preg_268435638));
  memset(&_preg_268435639, 0, sizeof(_preg_268435639));
  memset(&_preg_268435640, 0, sizeof(_preg_268435640));
  memset(&_preg_268435641, 0, sizeof(_preg_268435641));
  memset(&_preg_268435642, 0, sizeof(_preg_268435642));
  memset(&_preg_268435643, 0, sizeof(_preg_268435643));
  memset(&_preg_268435644, 0, sizeof(_preg_268435644));
  memset(&_preg_268435645, 0, sizeof(_preg_268435645));
  memset(&_preg_268435646, 0, sizeof(_preg_268435646));
  memset(&_preg_268435647, 0, sizeof(_preg_268435647));
  memset(&_preg_268435648, 0, sizeof(_preg_268435648));
  memset(&_preg_268435649, 0, sizeof(_preg_268435649));
  memset(&_preg_268435650, 0, sizeof(_preg_268435650));
  memset(&_preg_268435651, 0, sizeof(_preg_268435651));
  memset(&_preg_268435652, 0, sizeof(_preg_268435652));
  memset(&_preg_268435653, 0, sizeof(_preg_268435653));
  memset(&_preg_268435654, 0, sizeof(_preg_268435654));
  memset(&_preg_268435655, 0, sizeof(_preg_268435655));
  memset(&_preg_268435656, 0, sizeof(_preg_268435656));
  memset(&_preg_268435657, 0, sizeof(_preg_268435657));
  memset(&_preg_268435658, 0, sizeof(_preg_268435658));
  memset(&_preg_268435659, 0, sizeof(_preg_268435659));
  memset(&_preg_268435660, 0, sizeof(_preg_268435660));
  memset(&_preg_268435661, 0, sizeof(_preg_268435661));
  memset(&_preg_268435662, 0, sizeof(_preg_268435662));
  memset(&_preg_268435663, 0, sizeof(_preg_268435663));
  memset(&_preg_268435664, 0, sizeof(_preg_268435664));
  memset(&_preg_268435665, 0, sizeof(_preg_268435665));
  memset(&_preg_268435666, 0, sizeof(_preg_268435666));
  memset(&_preg_268435667, 0, sizeof(_preg_268435667));
  memset(&_preg_268435668, 0, sizeof(_preg_268435668));
  memset(&_preg_268435669, 0, sizeof(_preg_268435669));
  memset(&_preg_268435670, 0, sizeof(_preg_268435670));
  memset(&_preg_268435671, 0, sizeof(_preg_268435671));
  memset(&_preg_268435672, 0, sizeof(_preg_268435672));
  memset(&_preg_268435673, 0, sizeof(_preg_268435673));
  memset(&_preg_268435674, 0, sizeof(_preg_268435674));
  memset(&_preg_268435675, 0, sizeof(_preg_268435675));
  memset(&_preg_268435676, 0, sizeof(_preg_268435676));
  memset(&_preg_268435677, 0, sizeof(_preg_268435677));
  memset(&_preg_268435678, 0, sizeof(_preg_268435678));
  memset(&_preg_268435679, 0, sizeof(_preg_268435679));
  memset(&_preg_268435680, 0, sizeof(_preg_268435680));
  memset(&_preg_268435681, 0, sizeof(_preg_268435681));
  memset(&_preg_268435682, 0, sizeof(_preg_268435682));
  memset(&_preg_268435683, 0, sizeof(_preg_268435683));
  memset(&_preg_268435684, 0, sizeof(_preg_268435684));
  memset(&_preg_268435685, 0, sizeof(_preg_268435685));
  memset(&_preg_268435686, 0, sizeof(_preg_268435686));
  memset(&_preg_268435687, 0, sizeof(_preg_268435687));
  memset(&_preg_268435688, 0, sizeof(_preg_268435688));
  memset(&_preg_268435689, 0, sizeof(_preg_268435689));
  // /features/features.0/Conv
  // pragma: 65536, 1027, 33
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435456, &input_0);
  Free_data(&input_0._c1_poly);
  Free_data(&input_0._c0_poly);
  // BlockingRot replicate=16
  _preg_268435579 = Rotate(_preg_268435456, -4096);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_0_199, &_preg_268435456, &_preg_268435579);
  CMPLR_TM_END(12, 0);
  _preg_268435580 = Rotate(_preg_268435456, -8192);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_1_200, &_pgen_tmp_0_199, &_preg_268435580);
  CMPLR_TM_END(12, 0);
  _preg_268435581 = Rotate(_preg_268435456, -12288);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_2_201, &_pgen_tmp_1_200, &_preg_268435581);
  CMPLR_TM_END(12, 0);
  _preg_268435582 = Rotate(_preg_268435456, -16384);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_3_202, &_pgen_tmp_2_201, &_preg_268435582);
  CMPLR_TM_END(12, 0);
  _preg_268435583 = Rotate(_preg_268435456, -20480);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_4_203, &_pgen_tmp_3_202, &_preg_268435583);
  CMPLR_TM_END(12, 0);
  _preg_268435584 = Rotate(_preg_268435456, -24576);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_5_204, &_pgen_tmp_4_203, &_preg_268435584);
  CMPLR_TM_END(12, 0);
  _preg_268435585 = Rotate(_preg_268435456, -28672);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_6_205, &_pgen_tmp_5_204, &_preg_268435585);
  CMPLR_TM_END(12, 0);
  _preg_268435586 = Rotate(_preg_268435456, -32768);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_7_206, &_pgen_tmp_6_205, &_preg_268435586);
  CMPLR_TM_END(12, 0);
  _preg_268435587 = Rotate(_preg_268435456, -36864);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_8_207, &_pgen_tmp_7_206, &_preg_268435587);
  CMPLR_TM_END(12, 0);
  _preg_268435588 = Rotate(_preg_268435456, -40960);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_9_208, &_pgen_tmp_8_207, &_preg_268435588);
  CMPLR_TM_END(12, 0);
  _preg_268435589 = Rotate(_preg_268435456, -45056);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_10_209, &_pgen_tmp_9_208, &_preg_268435589);
  CMPLR_TM_END(12, 0);
  _preg_268435590 = Rotate(_preg_268435456, -49152);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_11_210, &_pgen_tmp_10_209, &_preg_268435590);
  CMPLR_TM_END(12, 0);
  _preg_268435591 = Rotate(_preg_268435456, -53248);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_12_211, &_pgen_tmp_11_210, &_preg_268435591);
  CMPLR_TM_END(12, 0);
  _preg_268435592 = Rotate(_preg_268435456, -57344);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_13_212, &_pgen_tmp_12_211, &_preg_268435592);
  CMPLR_TM_END(12, 0);
  _preg_268435593 = Rotate(_preg_268435456, -61440);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&input_dup_n0_1, &_pgen_tmp_13_212, &_preg_268435593);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&input_dup_n0_1._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_0_199._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435456._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435579._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_0_199._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435456._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435579._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_1_200._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_0_199._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435580._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_1_200._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_0_199._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435580._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_2_201._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_1_200._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435581._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_2_201._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_1_200._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435581._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_3_202._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_2_201._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435582._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_3_202._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_2_201._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435582._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_4_203._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_3_202._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435583._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_4_203._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_3_202._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435583._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_5_204._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_4_203._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435584._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_5_204._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_4_203._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435584._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_6_205._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_5_204._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435585._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_6_205._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_5_204._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435585._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_7_206._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_6_205._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435586._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_7_206._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_6_205._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435586._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_8_207._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_7_206._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435587._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_8_207._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_7_206._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435587._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_9_208._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_8_207._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435588._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_9_208._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_8_207._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435588._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_10_209._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_9_208._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435589._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_10_209._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_9_208._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435589._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_11_210._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_10_209._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435590._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_11_210._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_10_209._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435590._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_12_211._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_11_210._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435591._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_12_211._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_11_210._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435591._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_13_212._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_12_211._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435592._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_13_212._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_12_211._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435592._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&input_dup_n0_1._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_13_212._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435593._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&input_dup_n0_1._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_13_212._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435593._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_preg_268435579._c1_poly);
  Free_data(&_preg_268435579._c0_poly);
  Free_data(&_preg_268435456._c1_poly);
  Free_data(&_preg_268435456._c0_poly);
  Free_data(&_preg_268435580._c1_poly);
  Free_data(&_preg_268435580._c0_poly);
  Free_data(&_pgen_tmp_0_199._c1_poly);
  Free_data(&_pgen_tmp_0_199._c0_poly);
  Free_data(&_preg_268435581._c1_poly);
  Free_data(&_preg_268435581._c0_poly);
  Free_data(&_pgen_tmp_1_200._c1_poly);
  Free_data(&_pgen_tmp_1_200._c0_poly);
  Free_data(&_preg_268435582._c1_poly);
  Free_data(&_preg_268435582._c0_poly);
  Free_data(&_pgen_tmp_2_201._c1_poly);
  Free_data(&_pgen_tmp_2_201._c0_poly);
  Free_data(&_preg_268435583._c1_poly);
  Free_data(&_preg_268435583._c0_poly);
  Free_data(&_pgen_tmp_3_202._c1_poly);
  Free_data(&_pgen_tmp_3_202._c0_poly);
  Free_data(&_preg_268435584._c1_poly);
  Free_data(&_preg_268435584._c0_poly);
  Free_data(&_pgen_tmp_4_203._c1_poly);
  Free_data(&_pgen_tmp_4_203._c0_poly);
  Free_data(&_preg_268435585._c1_poly);
  Free_data(&_preg_268435585._c0_poly);
  Free_data(&_pgen_tmp_5_204._c1_poly);
  Free_data(&_pgen_tmp_5_204._c0_poly);
  Free_data(&_preg_268435586._c1_poly);
  Free_data(&_preg_268435586._c0_poly);
  Free_data(&_pgen_tmp_6_205._c1_poly);
  Free_data(&_pgen_tmp_6_205._c0_poly);
  Free_data(&_preg_268435587._c1_poly);
  Free_data(&_preg_268435587._c0_poly);
  Free_data(&_pgen_tmp_7_206._c1_poly);
  Free_data(&_pgen_tmp_7_206._c0_poly);
  Free_data(&_preg_268435588._c1_poly);
  Free_data(&_preg_268435588._c0_poly);
  Free_data(&_pgen_tmp_8_207._c1_poly);
  Free_data(&_pgen_tmp_8_207._c0_poly);
  Free_data(&_preg_268435589._c1_poly);
  Free_data(&_preg_268435589._c0_poly);
  Free_data(&_pgen_tmp_9_208._c1_poly);
  Free_data(&_pgen_tmp_9_208._c0_poly);
  Free_data(&_preg_268435590._c1_poly);
  Free_data(&_preg_268435590._c0_poly);
  Free_data(&_pgen_tmp_10_209._c1_poly);
  Free_data(&_pgen_tmp_10_209._c0_poly);
  Free_data(&_preg_268435591._c1_poly);
  Free_data(&_preg_268435591._c0_poly);
  Free_data(&_pgen_tmp_11_210._c1_poly);
  Free_data(&_pgen_tmp_11_210._c0_poly);
  Free_data(&_preg_268435592._c1_poly);
  Free_data(&_preg_268435592._c0_poly);
  Free_data(&_pgen_tmp_12_211._c1_poly);
  Free_data(&_pgen_tmp_12_211._c0_poly);
  Free_data(&_preg_268435593._c1_poly);
  Free_data(&_preg_268435593._c0_poly);
  Free_data(&_pgen_tmp_13_212._c1_poly);
  Free_data(&_pgen_tmp_13_212._c0_poly);
  CMPLR_TM_END(12, 0);
  // BlockingRot HRot=bs=9
  for (bs_input_n0_2 = 0; bs_input_n0_2 < 4; bs_input_n0_2 = bs_input_n0_2 + 1) {
    _preg_268435594 = Rotate(input_dup_n0_1, _cst_47[bs_input_n0_2]);
    Copy_ciph(&input_block_n0_3[bs_input_n0_2], &_preg_268435594);
    Free_data(&_preg_268435594._c1_poly);
    Free_data(&_preg_268435594._c0_poly);
  }
  _preg_268435595 = Rotate(input_dup_n0_1, 0);
  Copy_ciph(&input_block_n0_3[4], &_preg_268435595);
  Free_data(&_preg_268435595._c1_poly);
  Free_data(&_preg_268435595._c0_poly);
  for (bs_input_n0_rs2_4 = 5; bs_input_n0_rs2_4 < 9; bs_input_n0_rs2_4 = bs_input_n0_rs2_4 + 1) {
    _preg_268435596 = Rotate(input_dup_n0_1, _cst_47[bs_input_n0_rs2_4]);
    Copy_ciph(&input_block_n0_3[bs_input_n0_rs2_4], &_preg_268435596);
    Free_data(&_preg_268435596._c1_poly);
    Free_data(&_preg_268435596._c0_poly);
  }
  Free_data(&input_dup_n0_1._c1_poly);
  Free_data(&input_dup_n0_1._c0_poly);
  Zero_ciph(&result_n1_5);
  for (num_grid_n1_6 = 0; num_grid_n1_6 < 4; num_grid_n1_6 = num_grid_n1_6 + 1) {
    Zero_ciph(&result_grid_n1_7);
    for (cap_block_n1_8 = 0; cap_block_n1_8 < 9; cap_block_n1_8 = cap_block_n1_8 + 1) {
      _ckks_gen_tmp_148_153 = input_block_n0_3[cap_block_n1_8];
      _pgen_tmp_14_213 = *(PLAIN)Pt_from_msg(&_pgen_tmp_14_213, (cap_block_n1_8 + (num_grid_n1_6 * 9)) + 0 /* cst_45_0 */, 65536, 1, 16, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_15_214, &_ckks_gen_tmp_148_153, &_pgen_tmp_14_213);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n1_7, &result_grid_n1_7, &_pgen_tmp_15_214);
      _pgen_modulus_196 = Q_modulus();
      _pgen_num_q_197 = Poly_level(&result_grid_n1_7._c0_poly);
      for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_15_214._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_148_153._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_14_213._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_15_214._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_148_153._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_14_213._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modadd(Coeffs(&result_grid_n1_7._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_grid_n1_7._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_15_214._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modadd(Coeffs(&result_grid_n1_7._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_grid_n1_7._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_15_214._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        _pgen_modulus_196 = _pgen_modulus_196 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435597 = Rotate(result_grid_n1_7, num_grid_n1_6 * 1024);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n1_5, &result_n1_5, &_preg_268435597);
    _pgen_modulus_196 = Q_modulus();
    _pgen_num_q_197 = Poly_level(&result_n1_5._c0_poly);
    for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
      Hw_modadd(Coeffs(&result_n1_5._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n1_5._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435597._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&result_n1_5._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n1_5._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435597._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      _pgen_modulus_196 = _pgen_modulus_196 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_data(&_preg_268435597._c1_poly);
    Free_data(&_preg_268435597._c0_poly);
  }
  Free_data(&_pgen_tmp_14_213._poly);
  Free_data(&_ckks_gen_tmp_148_153._c1_poly);
  Free_data(&_ckks_gen_tmp_148_153._c0_poly);
  Free_data(&_pgen_tmp_15_214._c1_poly);
  Free_data(&_pgen_tmp_15_214._c0_poly);
  Free_data(&result_grid_n1_7._c1_poly);
  Free_data(&result_grid_n1_7._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n1_5, &result_n1_5);
  Rescale(&result_n1_5._c0_poly, &result_n1_5._c0_poly);
  Rescale(&result_n1_5._c1_poly, &result_n1_5._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_17_216 = *(PLAIN)Pt_from_msg(&_pgen_tmp_17_216, 36 /* cst_48 */, 65536, 1, 15, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_16_215, &result_n1_5, &_pgen_tmp_17_216);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_16_215._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_16_215._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n1_5._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_17_216._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_16_215._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n1_5._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_17_216._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_17_216._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435457, &_pgen_tmp_16_215);
  Rescale(&_preg_268435457._c0_poly, &_pgen_tmp_16_215._c0_poly);
  Rescale(&_preg_268435457._c1_poly, &_pgen_tmp_16_215._c1_poly);
  Free_data(&_pgen_tmp_16_215._c1_poly);
  Free_data(&_pgen_tmp_16_215._c0_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_18_217 = *(PLAIN)Pt_from_msg(&_pgen_tmp_18_217, 37 /* cst_46 */, 65536, 1, 14, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n1_5, &_preg_268435457, &_pgen_tmp_18_217);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&result_n1_5._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&result_n1_5._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435457._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_18_217._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Set_coeffs(&result_n1_5._c1_poly, _pgen_rns_idx_198, degree, Coeffs(&_preg_268435457._c1_poly, _pgen_rns_idx_198, degree));
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_18_217._poly);
  Free_data(&_preg_268435457._c1_poly);
  Free_data(&_preg_268435457._c0_poly);
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435458, &result_n1_5);
  Free_data(&result_n1_5._c1_poly);
  Free_data(&result_n1_5._c0_poly);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 33
  // /features/features.2/Relu
  // pragma: 65536, 1033, 39
  CMPLR_TM_START(10, 0);
  Copy_ciph(&_preg_268435459, &_preg_268435458);
  Free_data(&_preg_268435458._c1_poly);
  Free_data(&_preg_268435458._c0_poly);
  Encode_float(&_pgen_tmp_20_219, &_cst_178, 1, 1, Level(&_preg_268435459), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_19_218, &_preg_268435459, &_pgen_tmp_20_219);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_19_218._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_19_218._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435459._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_20_219._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_19_218._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435459._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_20_219._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_20_219._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435461, &_pgen_tmp_19_218);
  Rescale(&_preg_268435461._c0_poly, &_pgen_tmp_19_218._c0_poly);
  Rescale(&_preg_268435461._c1_poly, &_pgen_tmp_19_218._c1_poly);
  Free_data(&_pgen_tmp_19_218._c1_poly);
  Free_data(&_pgen_tmp_19_218._c0_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435460 = App_relu_1(_preg_268435459, _preg_268435461, 65536);
  Free_data(&_preg_268435461._c1_poly);
  Free_data(&_preg_268435461._c0_poly);
  Free_data(&_preg_268435459._c1_poly);
  Free_data(&_preg_268435459._c0_poly);
  Copy_ciph(&_preg_268435462, &_preg_268435460);
  Free_data(&_preg_268435460._c1_poly);
  Free_data(&_preg_268435460._c0_poly);
  CMPLR_TM_END(10, 0);
  // pragma: 65537, 1033, 39
  // /features/features.3/Conv
  // pragma: 65536, 1027, 40
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435463, &_preg_268435462);
  Free_data(&_preg_268435462._c1_poly);
  Free_data(&_preg_268435462._c0_poly);
  // BlockingRot replicate=1
  Copy_ciph(&input_dup_n1_9, &_preg_268435463);
  Free_data(&_preg_268435463._c1_poly);
  Free_data(&_preg_268435463._c0_poly);
  // BlockingRot HRot=bs=9
  for (bs_input_n1_10 = 0; bs_input_n1_10 < 4; bs_input_n1_10 = bs_input_n1_10 + 1) {
    _preg_268435598 = Rotate(input_dup_n1_9, _cst_52[bs_input_n1_10]);
    Copy_ciph(&input_block_n1_11[bs_input_n1_10], &_preg_268435598);
    Free_data(&_preg_268435598._c1_poly);
    Free_data(&_preg_268435598._c0_poly);
  }
  _preg_268435599 = Rotate(input_dup_n1_9, 0);
  Copy_ciph(&input_block_n1_11[4], &_preg_268435599);
  Free_data(&_preg_268435599._c1_poly);
  Free_data(&_preg_268435599._c0_poly);
  for (bs_input_n1_rs2_12 = 5; bs_input_n1_rs2_12 < 9; bs_input_n1_rs2_12 = bs_input_n1_rs2_12 + 1) {
    _preg_268435600 = Rotate(input_dup_n1_9, _cst_52[bs_input_n1_rs2_12]);
    Copy_ciph(&input_block_n1_11[bs_input_n1_rs2_12], &_preg_268435600);
    Free_data(&_preg_268435600._c1_poly);
    Free_data(&_preg_268435600._c0_poly);
  }
  Free_data(&input_dup_n1_9._c1_poly);
  Free_data(&input_dup_n1_9._c0_poly);
  Zero_ciph(&result_n2_13);
  for (num_grid_n2_14 = 0; num_grid_n2_14 < 64; num_grid_n2_14 = num_grid_n2_14 + 1) {
    Zero_ciph(&result_grid_n2_15);
    for (cap_block_n2_16 = 0; cap_block_n2_16 < 9; cap_block_n2_16 = cap_block_n2_16 + 1) {
      _ckks_gen_tmp_313_154 = input_block_n1_11[cap_block_n2_16];
      _pgen_tmp_21_220 = *(PLAIN)Pt_from_msg(&_pgen_tmp_21_220, (cap_block_n2_16 + (num_grid_n2_14 * 9)) + 38 /* cst_50_0 */, 65536, 1, 2, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_22_221, &_ckks_gen_tmp_313_154, &_pgen_tmp_21_220);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n2_15, &result_grid_n2_15, &_pgen_tmp_22_221);
      _pgen_modulus_196 = Q_modulus();
      _pgen_num_q_197 = Poly_level(&result_grid_n2_15._c0_poly);
      for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_22_221._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_313_154._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_21_220._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_22_221._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_313_154._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_21_220._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modadd(Coeffs(&result_grid_n2_15._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_grid_n2_15._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_22_221._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modadd(Coeffs(&result_grid_n2_15._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_grid_n2_15._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_22_221._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        _pgen_modulus_196 = _pgen_modulus_196 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435601 = Rotate(result_grid_n2_15, num_grid_n2_14 * 1024);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n2_13, &result_n2_13, &_preg_268435601);
    _pgen_modulus_196 = Q_modulus();
    _pgen_num_q_197 = Poly_level(&result_n2_13._c0_poly);
    for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
      Hw_modadd(Coeffs(&result_n2_13._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n2_13._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435601._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&result_n2_13._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n2_13._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435601._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      _pgen_modulus_196 = _pgen_modulus_196 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_data(&_preg_268435601._c1_poly);
    Free_data(&_preg_268435601._c0_poly);
  }
  Free_data(&_pgen_tmp_21_220._poly);
  Free_data(&_ckks_gen_tmp_313_154._c1_poly);
  Free_data(&_ckks_gen_tmp_313_154._c0_poly);
  Free_data(&_pgen_tmp_22_221._c1_poly);
  Free_data(&_pgen_tmp_22_221._c0_poly);
  Free_data(&result_grid_n2_15._c1_poly);
  Free_data(&result_grid_n2_15._c0_poly);
  CMPLR_TM_START(14, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_23_222, &result_n2_13);
  Rescale(&_pgen_tmp_23_222._c0_poly, &result_n2_13._c0_poly);
  Rescale(&_pgen_tmp_23_222._c1_poly, &result_n2_13._c1_poly);
  CMPLR_TM_END(19, 0);
  Bootstrap(&result_n2_13, &_pgen_tmp_23_222, 14);
  Free_data(&_pgen_tmp_23_222._c1_poly);
  Free_data(&_pgen_tmp_23_222._c0_poly);
  CMPLR_TM_END(14, 0);
  _pgen_tmp_25_224 = *(PLAIN)Pt_from_msg(&_pgen_tmp_25_224, 614 /* cst_53 */, 65536, 1, 14, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_24_223, &result_n2_13, &_pgen_tmp_25_224);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_24_223._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_24_223._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n2_13._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_25_224._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_24_223._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n2_13._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_25_224._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_25_224._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435464, &_pgen_tmp_24_223);
  Rescale(&_preg_268435464._c0_poly, &_pgen_tmp_24_223._c0_poly);
  Rescale(&_preg_268435464._c1_poly, &_pgen_tmp_24_223._c1_poly);
  Free_data(&_pgen_tmp_24_223._c1_poly);
  Free_data(&_pgen_tmp_24_223._c0_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_26_225 = *(PLAIN)Pt_from_msg(&_pgen_tmp_26_225, 615 /* cst_51 */, 65536, 1, 13, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n2_13, &_preg_268435464, &_pgen_tmp_26_225);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&result_n2_13._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&result_n2_13._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435464._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_26_225._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Set_coeffs(&result_n2_13._c1_poly, _pgen_rns_idx_198, degree, Coeffs(&_preg_268435464._c1_poly, _pgen_rns_idx_198, degree));
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_26_225._poly);
  Free_data(&_preg_268435464._c1_poly);
  Free_data(&_preg_268435464._c0_poly);
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435465, &result_n2_13);
  Free_data(&result_n2_13._c1_poly);
  Free_data(&result_n2_13._c0_poly);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 40
  // /features/features.5/Relu
  // pragma: 65536, 1033, 41
  CMPLR_TM_START(10, 0);
  Copy_ciph(&_preg_268435466, &_preg_268435465);
  Free_data(&_preg_268435465._c1_poly);
  Free_data(&_preg_268435465._c0_poly);
  Encode_float(&_pgen_tmp_28_227, &_cst_179, 1, 1, Level(&_preg_268435466), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_27_226, &_preg_268435466, &_pgen_tmp_28_227);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_27_226._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_27_226._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435466._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_28_227._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_27_226._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435466._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_28_227._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_28_227._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435468, &_pgen_tmp_27_226);
  Rescale(&_preg_268435468._c0_poly, &_pgen_tmp_27_226._c0_poly);
  Rescale(&_preg_268435468._c1_poly, &_pgen_tmp_27_226._c1_poly);
  Free_data(&_pgen_tmp_27_226._c1_poly);
  Free_data(&_pgen_tmp_27_226._c0_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435467 = App_relu_2(_preg_268435466, _preg_268435468, 65536);
  Free_data(&_preg_268435468._c1_poly);
  Free_data(&_preg_268435468._c0_poly);
  Free_data(&_preg_268435466._c1_poly);
  Free_data(&_preg_268435466._c0_poly);
  Copy_ciph(&_preg_268435469, &_preg_268435467);
  Free_data(&_preg_268435467._c1_poly);
  Free_data(&_preg_268435467._c0_poly);
  CMPLR_TM_END(10, 0);
  // pragma: 65537, 1033, 41
  // /features/features.6/AveragePool
  // pragma: 65536, 1026, 42
  CMPLR_TM_START(3, 0);
  Copy_ciph(&_preg_268435470, &_preg_268435469);
  Free_data(&_preg_268435469._c1_poly);
  Free_data(&_preg_268435469._c0_poly);
  _preg_268435602 = Rotate(_preg_268435470, 32);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&tmp_row_add2_17, &_preg_268435470, &_preg_268435602);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&tmp_row_add2_17._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&tmp_row_add2_17._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435470._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435602._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&tmp_row_add2_17._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435470._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435602._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_preg_268435602._c1_poly);
  Free_data(&_preg_268435602._c0_poly);
  Free_data(&_preg_268435470._c1_poly);
  Free_data(&_preg_268435470._c0_poly);
  CMPLR_TM_END(12, 0);
  _preg_268435603 = Rotate(tmp_row_add2_17, 1);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_preg_268435563, &tmp_row_add2_17, &_preg_268435603);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_preg_268435563._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&_preg_268435563._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&tmp_row_add2_17._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435603._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_preg_268435563._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&tmp_row_add2_17._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435603._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_preg_268435603._c1_poly);
  Free_data(&_preg_268435603._c0_poly);
  Free_data(&tmp_row_add2_17._c1_poly);
  Free_data(&tmp_row_add2_17._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(14, 0);
  Bootstrap(&_ckks_gen_tmp_412_155, &_preg_268435563, 6);
  Free_data(&_preg_268435563._c1_poly);
  Free_data(&_preg_268435563._c0_poly);
  CMPLR_TM_END(14, 0);
  _pgen_tmp_30_229 = *(PLAIN)Pt_from_msg(&_pgen_tmp_30_229, 616 /* cst_54 */, 65536, 1, 6, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_29_228, &_ckks_gen_tmp_412_155, &_pgen_tmp_30_229);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_29_228._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_29_228._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_412_155._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_30_229._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_29_228._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_412_155._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_30_229._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_30_229._poly);
  Free_data(&_ckks_gen_tmp_412_155._c1_poly);
  Free_data(&_ckks_gen_tmp_412_155._c0_poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435471, &_pgen_tmp_29_228);
  Rescale(&_preg_268435471._c0_poly, &_pgen_tmp_29_228._c0_poly);
  Rescale(&_preg_268435471._c1_poly, &_pgen_tmp_29_228._c1_poly);
  Free_data(&_pgen_tmp_29_228._c1_poly);
  Free_data(&_pgen_tmp_29_228._c0_poly);
  CMPLR_TM_END(19, 0);
  Copy_ciph(&orig_result2_18, &_preg_268435471);
  Free_data(&_preg_268435471._c1_poly);
  Free_data(&_preg_268435471._c0_poly);
  Zero_ciph(&comb_row_result3_19);
  for (comb_row_index_n3_20 = 0; comb_row_index_n3_20 < 16; comb_row_index_n3_20 = comb_row_index_n3_20 + 1) {
    _preg_268435604 = Rotate(orig_result2_18, comb_row_index_n3_20 * 1);
    Copy_ciph(&_ckks_gen_tmp_444_156, &_preg_268435604);
    _pgen_tmp_31_230 = *(PLAIN)Pt_from_msg(&_pgen_tmp_31_230, comb_row_index_n3_20 + 617 /* cst_55_0 */, 65536, 1, 5, 0);
    CMPLR_TM_START(17, 0);
    Init_ciph_up_scale_plain(&_pgen_tmp_32_231, &_ckks_gen_tmp_444_156, &_pgen_tmp_31_230);
    CMPLR_TM_END(17, 0);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&comb_row_result3_19, &comb_row_result3_19, &_pgen_tmp_32_231);
    _pgen_modulus_196 = Q_modulus();
    _pgen_num_q_197 = Poly_level(&comb_row_result3_19._c0_poly);
    for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_32_231._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_444_156._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_31_230._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_32_231._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_444_156._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_31_230._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&comb_row_result3_19._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&comb_row_result3_19._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_32_231._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&comb_row_result3_19._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&comb_row_result3_19._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_32_231._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      _pgen_modulus_196 = _pgen_modulus_196 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_data(&_preg_268435604._c1_poly);
    Free_data(&_preg_268435604._c0_poly);
  }
  Free_data(&orig_result2_18._c1_poly);
  Free_data(&orig_result2_18._c0_poly);
  Free_data(&_pgen_tmp_31_230._poly);
  Free_data(&_ckks_gen_tmp_444_156._c1_poly);
  Free_data(&_ckks_gen_tmp_444_156._c0_poly);
  Free_data(&_pgen_tmp_32_231._c1_poly);
  Free_data(&_pgen_tmp_32_231._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&comb_row_result3_19, &comb_row_result3_19);
  Rescale(&comb_row_result3_19._c0_poly, &comb_row_result3_19._c0_poly);
  Rescale(&comb_row_result3_19._c1_poly, &comb_row_result3_19._c1_poly);
  CMPLR_TM_END(19, 0);
  Zero_ciph(&comb_rc_result3_21);
  for (comb_rc_index_n3_22 = 0; comb_rc_index_n3_22 < 16; comb_rc_index_n3_22 = comb_rc_index_n3_22 + 1) {
    _preg_268435605 = Rotate(comb_row_result3_19, comb_rc_index_n3_22 * 48);
    Copy_ciph(&_ckks_gen_tmp_478_157, &_preg_268435605);
    _pgen_tmp_33_232 = *(PLAIN)Pt_from_msg(&_pgen_tmp_33_232, comb_rc_index_n3_22 + 633 /* cst_56_0 */, 65536, 1, 4, 0);
    CMPLR_TM_START(17, 0);
    Init_ciph_up_scale_plain(&_pgen_tmp_34_233, &_ckks_gen_tmp_478_157, &_pgen_tmp_33_232);
    CMPLR_TM_END(17, 0);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&comb_rc_result3_21, &comb_rc_result3_21, &_pgen_tmp_34_233);
    _pgen_modulus_196 = Q_modulus();
    _pgen_num_q_197 = Poly_level(&comb_rc_result3_21._c0_poly);
    for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_34_233._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_478_157._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_33_232._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_34_233._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_478_157._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_33_232._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&comb_rc_result3_21._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&comb_rc_result3_21._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_34_233._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&comb_rc_result3_21._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&comb_rc_result3_21._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_34_233._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      _pgen_modulus_196 = _pgen_modulus_196 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_data(&_preg_268435605._c1_poly);
    Free_data(&_preg_268435605._c0_poly);
  }
  Free_data(&comb_row_result3_19._c1_poly);
  Free_data(&comb_row_result3_19._c0_poly);
  Free_data(&_pgen_tmp_33_232._poly);
  Free_data(&_ckks_gen_tmp_478_157._c1_poly);
  Free_data(&_ckks_gen_tmp_478_157._c0_poly);
  Free_data(&_pgen_tmp_34_233._c1_poly);
  Free_data(&_pgen_tmp_34_233._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&comb_rc_result3_21, &comb_rc_result3_21);
  Rescale(&comb_rc_result3_21._c0_poly, &comb_rc_result3_21._c0_poly);
  Rescale(&comb_rc_result3_21._c1_poly, &comb_rc_result3_21._c1_poly);
  CMPLR_TM_END(19, 0);
  Zero_ciph(&ss_result3_23);
  for (comb_cc_index_n3_24 = 0; comb_cc_index_n3_24 < 64; comb_cc_index_n3_24 = comb_cc_index_n3_24 + 1) {
    _preg_268435606 = Rotate(comb_rc_result3_21, comb_cc_index_n3_24 * 768);
    Copy_ciph(&_ckks_gen_tmp_512_158, &_preg_268435606);
    _pgen_tmp_35_234 = *(PLAIN)Pt_from_msg(&_pgen_tmp_35_234, comb_cc_index_n3_24 + 649 /* cst_57_0 */, 65536, 1, 3, 0);
    CMPLR_TM_START(17, 0);
    Init_ciph_up_scale_plain(&_pgen_tmp_36_235, &_ckks_gen_tmp_512_158, &_pgen_tmp_35_234);
    CMPLR_TM_END(17, 0);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&ss_result3_23, &ss_result3_23, &_pgen_tmp_36_235);
    _pgen_modulus_196 = Q_modulus();
    _pgen_num_q_197 = Poly_level(&ss_result3_23._c0_poly);
    for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_36_235._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_512_158._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_35_234._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_36_235._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_512_158._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_35_234._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&ss_result3_23._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&ss_result3_23._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_36_235._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&ss_result3_23._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&ss_result3_23._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_36_235._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      _pgen_modulus_196 = _pgen_modulus_196 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_data(&_preg_268435606._c1_poly);
    Free_data(&_preg_268435606._c0_poly);
  }
  Free_data(&comb_rc_result3_21._c1_poly);
  Free_data(&comb_rc_result3_21._c0_poly);
  Free_data(&_pgen_tmp_35_234._poly);
  Free_data(&_ckks_gen_tmp_512_158._c1_poly);
  Free_data(&_ckks_gen_tmp_512_158._c0_poly);
  Free_data(&_pgen_tmp_36_235._c1_poly);
  Free_data(&_pgen_tmp_36_235._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&ss_result3_23, &ss_result3_23);
  Rescale(&ss_result3_23._c0_poly, &ss_result3_23._c0_poly);
  Rescale(&ss_result3_23._c1_poly, &ss_result3_23._c1_poly);
  CMPLR_TM_END(19, 0);
  Copy_ciph(&_preg_268435472, &ss_result3_23);
  Free_data(&ss_result3_23._c1_poly);
  Free_data(&ss_result3_23._c0_poly);
  CMPLR_TM_END(3, 0);
  // pragma: 65537, 1026, 42
  // /features/features.7/Conv
  // pragma: 65536, 1027, 44
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435473, &_preg_268435472);
  Free_data(&_preg_268435472._c1_poly);
  Free_data(&_preg_268435472._c0_poly);
  // BlockingRot replicate=2
  _preg_268435607 = Rotate(_preg_268435473, -16384);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&input_dup_n3_25, &_preg_268435473, &_preg_268435607);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&input_dup_n3_25._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&input_dup_n3_25._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435473._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435607._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&input_dup_n3_25._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435473._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435607._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_preg_268435607._c1_poly);
  Free_data(&_preg_268435607._c0_poly);
  Free_data(&_preg_268435473._c1_poly);
  Free_data(&_preg_268435473._c0_poly);
  CMPLR_TM_END(12, 0);
  // BlockingRot HRot=bs=9
  for (bs_input_n3_26 = 0; bs_input_n3_26 < 4; bs_input_n3_26 = bs_input_n3_26 + 1) {
    _preg_268435608 = Rotate(input_dup_n3_25, _cst_61[bs_input_n3_26]);
    Copy_ciph(&input_block_n3_27[bs_input_n3_26], &_preg_268435608);
    Free_data(&_preg_268435608._c1_poly);
    Free_data(&_preg_268435608._c0_poly);
  }
  _preg_268435609 = Rotate(input_dup_n3_25, 0);
  Copy_ciph(&input_block_n3_27[4], &_preg_268435609);
  Free_data(&_preg_268435609._c1_poly);
  Free_data(&_preg_268435609._c0_poly);
  for (bs_input_n3_rs2_28 = 5; bs_input_n3_rs2_28 < 9; bs_input_n3_rs2_28 = bs_input_n3_rs2_28 + 1) {
    _preg_268435610 = Rotate(input_dup_n3_25, _cst_61[bs_input_n3_rs2_28]);
    Copy_ciph(&input_block_n3_27[bs_input_n3_rs2_28], &_preg_268435610);
    Free_data(&_preg_268435610._c1_poly);
    Free_data(&_preg_268435610._c0_poly);
  }
  Free_data(&input_dup_n3_25._c1_poly);
  Free_data(&input_dup_n3_25._c0_poly);
  Zero_ciph(&result_n4_29);
  for (num_grid_n4_30 = 0; num_grid_n4_30 < 64; num_grid_n4_30 = num_grid_n4_30 + 1) {
    Zero_ciph(&result_grid_n4_31);
    for (cap_block_n4_32 = 0; cap_block_n4_32 < 9; cap_block_n4_32 = cap_block_n4_32 + 1) {
      _ckks_gen_tmp_622_159 = input_block_n3_27[cap_block_n4_32];
      _pgen_tmp_37_236 = *(PLAIN)Pt_from_msg(&_pgen_tmp_37_236, (cap_block_n4_32 + (num_grid_n4_30 * 9)) + 713 /* cst_59_0 */, 32768, 1, 2, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_38_237, &_ckks_gen_tmp_622_159, &_pgen_tmp_37_236);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n4_31, &result_grid_n4_31, &_pgen_tmp_38_237);
      _pgen_modulus_196 = Q_modulus();
      _pgen_num_q_197 = Poly_level(&result_grid_n4_31._c0_poly);
      for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_38_237._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_622_159._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_37_236._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_38_237._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_622_159._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_37_236._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modadd(Coeffs(&result_grid_n4_31._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_grid_n4_31._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_38_237._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modadd(Coeffs(&result_grid_n4_31._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_grid_n4_31._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_38_237._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        _pgen_modulus_196 = _pgen_modulus_196 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435611 = Rotate(result_grid_n4_31, num_grid_n4_30 * 256);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n4_29, &result_n4_29, &_preg_268435611);
    _pgen_modulus_196 = Q_modulus();
    _pgen_num_q_197 = Poly_level(&result_n4_29._c0_poly);
    for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
      Hw_modadd(Coeffs(&result_n4_29._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n4_29._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435611._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&result_n4_29._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n4_29._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435611._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      _pgen_modulus_196 = _pgen_modulus_196 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_data(&_preg_268435611._c1_poly);
    Free_data(&_preg_268435611._c0_poly);
  }
  Free_data(&_pgen_tmp_37_236._poly);
  Free_data(&_ckks_gen_tmp_622_159._c1_poly);
  Free_data(&_ckks_gen_tmp_622_159._c0_poly);
  Free_data(&_pgen_tmp_38_237._c1_poly);
  Free_data(&_pgen_tmp_38_237._c0_poly);
  Free_data(&result_grid_n4_31._c1_poly);
  Free_data(&result_grid_n4_31._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n4_29, &result_n4_29);
  Rescale(&result_n4_29._c0_poly, &result_n4_29._c0_poly);
  Rescale(&result_n4_29._c1_poly, &result_n4_29._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435612 = Rotate(result_n4_29, -32768);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_preg_268435564, &result_n4_29, &_preg_268435612);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_preg_268435564._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&_preg_268435564._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n4_29._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435612._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_preg_268435564._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n4_29._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435612._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_preg_268435612._c1_poly);
  Free_data(&_preg_268435612._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(14, 0);
  Bootstrap(&_ckks_gen_tmp_658_160, &_preg_268435564, 15);
  Free_data(&_preg_268435564._c1_poly);
  Free_data(&_preg_268435564._c0_poly);
  CMPLR_TM_END(14, 0);
  _pgen_tmp_40_239 = *(PLAIN)Pt_from_msg(&_pgen_tmp_40_239, 1289 /* cst_62 */, 32768, 1, 15, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_39_238, &_ckks_gen_tmp_658_160, &_pgen_tmp_40_239);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_39_238._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_39_238._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_658_160._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_40_239._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_39_238._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_658_160._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_40_239._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_40_239._poly);
  Free_data(&_ckks_gen_tmp_658_160._c1_poly);
  Free_data(&_ckks_gen_tmp_658_160._c0_poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435474, &_pgen_tmp_39_238);
  Rescale(&_preg_268435474._c0_poly, &_pgen_tmp_39_238._c0_poly);
  Rescale(&_preg_268435474._c1_poly, &_pgen_tmp_39_238._c1_poly);
  Free_data(&_pgen_tmp_39_238._c1_poly);
  Free_data(&_pgen_tmp_39_238._c0_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_41_240 = *(PLAIN)Pt_from_msg(&_pgen_tmp_41_240, 1290 /* cst_60 */, 32768, 1, 14, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n4_29, &_preg_268435474, &_pgen_tmp_41_240);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&result_n4_29._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&result_n4_29._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435474._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_41_240._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Set_coeffs(&result_n4_29._c1_poly, _pgen_rns_idx_198, degree, Coeffs(&_preg_268435474._c1_poly, _pgen_rns_idx_198, degree));
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_41_240._poly);
  Free_data(&_preg_268435474._c1_poly);
  Free_data(&_preg_268435474._c0_poly);
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435475, &result_n4_29);
  Free_data(&result_n4_29._c1_poly);
  Free_data(&result_n4_29._c0_poly);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 44
  // /features/features.9/Relu
  // pragma: 65536, 1033, 45
  CMPLR_TM_START(10, 0);
  Copy_ciph(&_preg_268435476, &_preg_268435475);
  Free_data(&_preg_268435475._c1_poly);
  Free_data(&_preg_268435475._c0_poly);
  Encode_float(&_pgen_tmp_43_242, &_cst_180, 1, 1, Level(&_preg_268435476), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_42_241, &_preg_268435476, &_pgen_tmp_43_242);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_42_241._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_42_241._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435476._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_43_242._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_42_241._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435476._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_43_242._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_43_242._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435478, &_pgen_tmp_42_241);
  Rescale(&_preg_268435478._c0_poly, &_pgen_tmp_42_241._c0_poly);
  Rescale(&_preg_268435478._c1_poly, &_pgen_tmp_42_241._c1_poly);
  Free_data(&_pgen_tmp_42_241._c1_poly);
  Free_data(&_pgen_tmp_42_241._c0_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435477 = App_relu_1(_preg_268435476, _preg_268435478, 32768);
  Free_data(&_preg_268435478._c1_poly);
  Free_data(&_preg_268435478._c0_poly);
  Free_data(&_preg_268435476._c1_poly);
  Free_data(&_preg_268435476._c0_poly);
  Copy_ciph(&_preg_268435479, &_preg_268435477);
  Free_data(&_preg_268435477._c1_poly);
  Free_data(&_preg_268435477._c0_poly);
  CMPLR_TM_END(10, 0);
  // pragma: 65537, 1033, 45
  // /features/features.10/Conv
  // pragma: 65536, 1027, 46
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435480, &_preg_268435479);
  Free_data(&_preg_268435479._c1_poly);
  Free_data(&_preg_268435479._c0_poly);
  // BlockingRot replicate=1
  Copy_ciph(&input_dup_n4_33, &_preg_268435480);
  Free_data(&_preg_268435480._c1_poly);
  Free_data(&_preg_268435480._c0_poly);
  // BlockingRot HRot=bs=9
  for (bs_input_n4_34 = 0; bs_input_n4_34 < 4; bs_input_n4_34 = bs_input_n4_34 + 1) {
    _preg_268435613 = Rotate(input_dup_n4_33, _cst_66[bs_input_n4_34]);
    Copy_ciph(&input_block_n4_35[bs_input_n4_34], &_preg_268435613);
    Free_data(&_preg_268435613._c1_poly);
    Free_data(&_preg_268435613._c0_poly);
  }
  _preg_268435614 = Rotate(input_dup_n4_33, 0);
  Copy_ciph(&input_block_n4_35[4], &_preg_268435614);
  Free_data(&_preg_268435614._c1_poly);
  Free_data(&_preg_268435614._c0_poly);
  for (bs_input_n4_rs2_36 = 5; bs_input_n4_rs2_36 < 9; bs_input_n4_rs2_36 = bs_input_n4_rs2_36 + 1) {
    _preg_268435615 = Rotate(input_dup_n4_33, _cst_66[bs_input_n4_rs2_36]);
    Copy_ciph(&input_block_n4_35[bs_input_n4_rs2_36], &_preg_268435615);
    Free_data(&_preg_268435615._c1_poly);
    Free_data(&_preg_268435615._c0_poly);
  }
  Free_data(&input_dup_n4_33._c1_poly);
  Free_data(&input_dup_n4_33._c0_poly);
  Zero_ciph(&result_n5_37);
  for (num_grid_n5_38 = 0; num_grid_n5_38 < 128; num_grid_n5_38 = num_grid_n5_38 + 1) {
    Zero_ciph(&result_grid_n5_39);
    for (cap_block_n5_40 = 0; cap_block_n5_40 < 9; cap_block_n5_40 = cap_block_n5_40 + 1) {
      _ckks_gen_tmp_793_161 = input_block_n4_35[cap_block_n5_40];
      _pgen_tmp_44_243 = *(PLAIN)Pt_from_msg(&_pgen_tmp_44_243, (cap_block_n5_40 + (num_grid_n5_38 * 9)) + 1291 /* cst_64_0 */, 32768, 1, 2, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_45_244, &_ckks_gen_tmp_793_161, &_pgen_tmp_44_243);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n5_39, &result_grid_n5_39, &_pgen_tmp_45_244);
      _pgen_modulus_196 = Q_modulus();
      _pgen_num_q_197 = Poly_level(&result_grid_n5_39._c0_poly);
      for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_45_244._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_793_161._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_44_243._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_45_244._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_793_161._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_44_243._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modadd(Coeffs(&result_grid_n5_39._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_grid_n5_39._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_45_244._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modadd(Coeffs(&result_grid_n5_39._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_grid_n5_39._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_45_244._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        _pgen_modulus_196 = _pgen_modulus_196 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435616 = Rotate(result_grid_n5_39, num_grid_n5_38 * 256);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n5_37, &result_n5_37, &_preg_268435616);
    _pgen_modulus_196 = Q_modulus();
    _pgen_num_q_197 = Poly_level(&result_n5_37._c0_poly);
    for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
      Hw_modadd(Coeffs(&result_n5_37._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n5_37._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435616._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&result_n5_37._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n5_37._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435616._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      _pgen_modulus_196 = _pgen_modulus_196 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_data(&_preg_268435616._c1_poly);
    Free_data(&_preg_268435616._c0_poly);
  }
  Free_data(&_pgen_tmp_44_243._poly);
  Free_data(&_ckks_gen_tmp_793_161._c1_poly);
  Free_data(&_ckks_gen_tmp_793_161._c0_poly);
  Free_data(&_pgen_tmp_45_244._c1_poly);
  Free_data(&_pgen_tmp_45_244._c0_poly);
  Free_data(&result_grid_n5_39._c1_poly);
  Free_data(&result_grid_n5_39._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n5_37, &result_n5_37);
  Rescale(&result_n5_37._c0_poly, &result_n5_37._c0_poly);
  Rescale(&result_n5_37._c1_poly, &result_n5_37._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435617 = Rotate(result_n5_37, -32768);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_preg_268435565, &result_n5_37, &_preg_268435617);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_preg_268435565._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&_preg_268435565._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n5_37._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435617._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_preg_268435565._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n5_37._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435617._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_preg_268435617._c1_poly);
  Free_data(&_preg_268435617._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(14, 0);
  Bootstrap(&_ckks_gen_tmp_829_162, &_preg_268435565, 14);
  Free_data(&_preg_268435565._c1_poly);
  Free_data(&_preg_268435565._c0_poly);
  CMPLR_TM_END(14, 0);
  _pgen_tmp_47_246 = *(PLAIN)Pt_from_msg(&_pgen_tmp_47_246, 2443 /* cst_67 */, 32768, 1, 14, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_46_245, &_ckks_gen_tmp_829_162, &_pgen_tmp_47_246);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_46_245._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_46_245._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_829_162._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_47_246._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_46_245._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_829_162._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_47_246._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_47_246._poly);
  Free_data(&_ckks_gen_tmp_829_162._c1_poly);
  Free_data(&_ckks_gen_tmp_829_162._c0_poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435481, &_pgen_tmp_46_245);
  Rescale(&_preg_268435481._c0_poly, &_pgen_tmp_46_245._c0_poly);
  Rescale(&_preg_268435481._c1_poly, &_pgen_tmp_46_245._c1_poly);
  Free_data(&_pgen_tmp_46_245._c1_poly);
  Free_data(&_pgen_tmp_46_245._c0_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_48_247 = *(PLAIN)Pt_from_msg(&_pgen_tmp_48_247, 2444 /* cst_65 */, 32768, 1, 13, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n5_37, &_preg_268435481, &_pgen_tmp_48_247);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&result_n5_37._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&result_n5_37._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435481._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_48_247._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Set_coeffs(&result_n5_37._c1_poly, _pgen_rns_idx_198, degree, Coeffs(&_preg_268435481._c1_poly, _pgen_rns_idx_198, degree));
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_48_247._poly);
  Free_data(&_preg_268435481._c1_poly);
  Free_data(&_preg_268435481._c0_poly);
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435482, &result_n5_37);
  Free_data(&result_n5_37._c1_poly);
  Free_data(&result_n5_37._c0_poly);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 46
  // /features/features.12/Relu
  // pragma: 65536, 1033, 47
  CMPLR_TM_START(10, 0);
  Copy_ciph(&_preg_268435483, &_preg_268435482);
  Free_data(&_preg_268435482._c1_poly);
  Free_data(&_preg_268435482._c0_poly);
  Encode_float(&_pgen_tmp_50_249, &_cst_181, 1, 1, Level(&_preg_268435483), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_49_248, &_preg_268435483, &_pgen_tmp_50_249);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_49_248._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_49_248._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435483._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_50_249._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_49_248._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435483._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_50_249._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_50_249._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435485, &_pgen_tmp_49_248);
  Rescale(&_preg_268435485._c0_poly, &_pgen_tmp_49_248._c0_poly);
  Rescale(&_preg_268435485._c1_poly, &_pgen_tmp_49_248._c1_poly);
  Free_data(&_pgen_tmp_49_248._c1_poly);
  Free_data(&_pgen_tmp_49_248._c0_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435484 = App_relu_2(_preg_268435483, _preg_268435485, 32768);
  Free_data(&_preg_268435485._c1_poly);
  Free_data(&_preg_268435485._c0_poly);
  Free_data(&_preg_268435483._c1_poly);
  Free_data(&_preg_268435483._c0_poly);
  Copy_ciph(&_preg_268435486, &_preg_268435484);
  Free_data(&_preg_268435484._c1_poly);
  Free_data(&_preg_268435484._c0_poly);
  CMPLR_TM_END(10, 0);
  // pragma: 65537, 1033, 47
  // /features/features.13/AveragePool
  // pragma: 65536, 1026, 48
  CMPLR_TM_START(3, 0);
  Copy_ciph(&_preg_268435487, &_preg_268435486);
  Free_data(&_preg_268435486._c1_poly);
  Free_data(&_preg_268435486._c0_poly);
  _preg_268435618 = Rotate(_preg_268435487, 16);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&tmp_row_add5_41, &_preg_268435487, &_preg_268435618);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&tmp_row_add5_41._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&tmp_row_add5_41._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435487._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435618._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&tmp_row_add5_41._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435487._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435618._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_preg_268435618._c1_poly);
  Free_data(&_preg_268435618._c0_poly);
  Free_data(&_preg_268435487._c1_poly);
  Free_data(&_preg_268435487._c0_poly);
  CMPLR_TM_END(12, 0);
  _preg_268435619 = Rotate(tmp_row_add5_41, 1);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_preg_268435566, &tmp_row_add5_41, &_preg_268435619);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_preg_268435566._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&_preg_268435566._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&tmp_row_add5_41._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435619._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_preg_268435566._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&tmp_row_add5_41._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435619._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_preg_268435619._c1_poly);
  Free_data(&_preg_268435619._c0_poly);
  Free_data(&tmp_row_add5_41._c1_poly);
  Free_data(&tmp_row_add5_41._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(14, 0);
  Bootstrap(&_ckks_gen_tmp_898_163, &_preg_268435566, 6);
  Free_data(&_preg_268435566._c1_poly);
  Free_data(&_preg_268435566._c0_poly);
  CMPLR_TM_END(14, 0);
  _pgen_tmp_52_251 = *(PLAIN)Pt_from_msg(&_pgen_tmp_52_251, 2445 /* cst_68 */, 32768, 1, 6, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_51_250, &_ckks_gen_tmp_898_163, &_pgen_tmp_52_251);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_51_250._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_51_250._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_898_163._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_52_251._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_51_250._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_898_163._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_52_251._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_52_251._poly);
  Free_data(&_ckks_gen_tmp_898_163._c1_poly);
  Free_data(&_ckks_gen_tmp_898_163._c0_poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435488, &_pgen_tmp_51_250);
  Rescale(&_preg_268435488._c0_poly, &_pgen_tmp_51_250._c0_poly);
  Rescale(&_preg_268435488._c1_poly, &_pgen_tmp_51_250._c1_poly);
  Free_data(&_pgen_tmp_51_250._c1_poly);
  Free_data(&_pgen_tmp_51_250._c0_poly);
  CMPLR_TM_END(19, 0);
  Copy_ciph(&orig_result5_42, &_preg_268435488);
  Free_data(&_preg_268435488._c1_poly);
  Free_data(&_preg_268435488._c0_poly);
  Zero_ciph(&comb_row_result6_43);
  for (comb_row_index_n6_44 = 0; comb_row_index_n6_44 < 8; comb_row_index_n6_44 = comb_row_index_n6_44 + 1) {
    _preg_268435620 = Rotate(orig_result5_42, comb_row_index_n6_44 * 1);
    Copy_ciph(&_ckks_gen_tmp_930_164, &_preg_268435620);
    _pgen_tmp_53_252 = *(PLAIN)Pt_from_msg(&_pgen_tmp_53_252, comb_row_index_n6_44 + 2446 /* cst_69_0 */, 32768, 1, 5, 0);
    CMPLR_TM_START(17, 0);
    Init_ciph_up_scale_plain(&_pgen_tmp_54_253, &_ckks_gen_tmp_930_164, &_pgen_tmp_53_252);
    CMPLR_TM_END(17, 0);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&comb_row_result6_43, &comb_row_result6_43, &_pgen_tmp_54_253);
    _pgen_modulus_196 = Q_modulus();
    _pgen_num_q_197 = Poly_level(&comb_row_result6_43._c0_poly);
    for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_54_253._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_930_164._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_53_252._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_54_253._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_930_164._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_53_252._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&comb_row_result6_43._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&comb_row_result6_43._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_54_253._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&comb_row_result6_43._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&comb_row_result6_43._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_54_253._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      _pgen_modulus_196 = _pgen_modulus_196 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_data(&_preg_268435620._c1_poly);
    Free_data(&_preg_268435620._c0_poly);
  }
  Free_data(&orig_result5_42._c1_poly);
  Free_data(&orig_result5_42._c0_poly);
  Free_data(&_pgen_tmp_53_252._poly);
  Free_data(&_ckks_gen_tmp_930_164._c1_poly);
  Free_data(&_ckks_gen_tmp_930_164._c0_poly);
  Free_data(&_pgen_tmp_54_253._c1_poly);
  Free_data(&_pgen_tmp_54_253._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&comb_row_result6_43, &comb_row_result6_43);
  Rescale(&comb_row_result6_43._c0_poly, &comb_row_result6_43._c0_poly);
  Rescale(&comb_row_result6_43._c1_poly, &comb_row_result6_43._c1_poly);
  CMPLR_TM_END(19, 0);
  Zero_ciph(&comb_rc_result6_45);
  for (comb_rc_index_n6_46 = 0; comb_rc_index_n6_46 < 8; comb_rc_index_n6_46 = comb_rc_index_n6_46 + 1) {
    _preg_268435621 = Rotate(comb_row_result6_43, comb_rc_index_n6_46 * 24);
    Copy_ciph(&_ckks_gen_tmp_964_165, &_preg_268435621);
    _pgen_tmp_55_254 = *(PLAIN)Pt_from_msg(&_pgen_tmp_55_254, comb_rc_index_n6_46 + 2454 /* cst_70_0 */, 32768, 1, 4, 0);
    CMPLR_TM_START(17, 0);
    Init_ciph_up_scale_plain(&_pgen_tmp_56_255, &_ckks_gen_tmp_964_165, &_pgen_tmp_55_254);
    CMPLR_TM_END(17, 0);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&comb_rc_result6_45, &comb_rc_result6_45, &_pgen_tmp_56_255);
    _pgen_modulus_196 = Q_modulus();
    _pgen_num_q_197 = Poly_level(&comb_rc_result6_45._c0_poly);
    for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_56_255._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_964_165._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_55_254._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_56_255._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_964_165._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_55_254._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&comb_rc_result6_45._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&comb_rc_result6_45._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_56_255._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&comb_rc_result6_45._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&comb_rc_result6_45._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_56_255._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      _pgen_modulus_196 = _pgen_modulus_196 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_data(&_preg_268435621._c1_poly);
    Free_data(&_preg_268435621._c0_poly);
  }
  Free_data(&comb_row_result6_43._c1_poly);
  Free_data(&comb_row_result6_43._c0_poly);
  Free_data(&_pgen_tmp_55_254._poly);
  Free_data(&_ckks_gen_tmp_964_165._c1_poly);
  Free_data(&_ckks_gen_tmp_964_165._c0_poly);
  Free_data(&_pgen_tmp_56_255._c1_poly);
  Free_data(&_pgen_tmp_56_255._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&comb_rc_result6_45, &comb_rc_result6_45);
  Rescale(&comb_rc_result6_45._c0_poly, &comb_rc_result6_45._c0_poly);
  Rescale(&comb_rc_result6_45._c1_poly, &comb_rc_result6_45._c1_poly);
  CMPLR_TM_END(19, 0);
  Zero_ciph(&ss_result6_47);
  for (comb_cc_index_n6_48 = 0; comb_cc_index_n6_48 < 128; comb_cc_index_n6_48 = comb_cc_index_n6_48 + 1) {
    _preg_268435622 = Rotate(comb_rc_result6_45, comb_cc_index_n6_48 * 192);
    Copy_ciph(&_ckks_gen_tmp_998_166, &_preg_268435622);
    _pgen_tmp_57_256 = *(PLAIN)Pt_from_msg(&_pgen_tmp_57_256, comb_cc_index_n6_48 + 2462 /* cst_71_0 */, 32768, 1, 3, 0);
    CMPLR_TM_START(17, 0);
    Init_ciph_up_scale_plain(&_pgen_tmp_58_257, &_ckks_gen_tmp_998_166, &_pgen_tmp_57_256);
    CMPLR_TM_END(17, 0);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&ss_result6_47, &ss_result6_47, &_pgen_tmp_58_257);
    _pgen_modulus_196 = Q_modulus();
    _pgen_num_q_197 = Poly_level(&ss_result6_47._c0_poly);
    for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_58_257._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_998_166._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_57_256._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_58_257._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_998_166._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_57_256._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&ss_result6_47._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&ss_result6_47._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_58_257._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&ss_result6_47._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&ss_result6_47._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_58_257._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      _pgen_modulus_196 = _pgen_modulus_196 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_data(&_preg_268435622._c1_poly);
    Free_data(&_preg_268435622._c0_poly);
  }
  Free_data(&comb_rc_result6_45._c1_poly);
  Free_data(&comb_rc_result6_45._c0_poly);
  Free_data(&_pgen_tmp_57_256._poly);
  Free_data(&_ckks_gen_tmp_998_166._c1_poly);
  Free_data(&_ckks_gen_tmp_998_166._c0_poly);
  Free_data(&_pgen_tmp_58_257._c1_poly);
  Free_data(&_pgen_tmp_58_257._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&ss_result6_47, &ss_result6_47);
  Rescale(&ss_result6_47._c0_poly, &ss_result6_47._c0_poly);
  Rescale(&ss_result6_47._c1_poly, &ss_result6_47._c1_poly);
  CMPLR_TM_END(19, 0);
  Copy_ciph(&_preg_268435489, &ss_result6_47);
  Free_data(&ss_result6_47._c1_poly);
  Free_data(&ss_result6_47._c0_poly);
  CMPLR_TM_END(3, 0);
  // pragma: 65537, 1026, 48
  // /features/features.14/Conv
  // pragma: 65536, 1027, 49
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435490, &_preg_268435489);
  Free_data(&_preg_268435489._c1_poly);
  Free_data(&_preg_268435489._c0_poly);
  // BlockingRot replicate=2
  _preg_268435623 = Rotate(_preg_268435490, -8192);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&input_dup_n6_49, &_preg_268435490, &_preg_268435623);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&input_dup_n6_49._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&input_dup_n6_49._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435490._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435623._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&input_dup_n6_49._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435490._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435623._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_preg_268435623._c1_poly);
  Free_data(&_preg_268435623._c0_poly);
  Free_data(&_preg_268435490._c1_poly);
  Free_data(&_preg_268435490._c0_poly);
  CMPLR_TM_END(12, 0);
  // BlockingRot HRot=bs=9
  for (bs_input_n6_50 = 0; bs_input_n6_50 < 4; bs_input_n6_50 = bs_input_n6_50 + 1) {
    _preg_268435624 = Rotate(input_dup_n6_49, _cst_75[bs_input_n6_50]);
    Copy_ciph(&input_block_n6_51[bs_input_n6_50], &_preg_268435624);
    Free_data(&_preg_268435624._c1_poly);
    Free_data(&_preg_268435624._c0_poly);
  }
  _preg_268435625 = Rotate(input_dup_n6_49, 0);
  Copy_ciph(&input_block_n6_51[4], &_preg_268435625);
  Free_data(&_preg_268435625._c1_poly);
  Free_data(&_preg_268435625._c0_poly);
  for (bs_input_n6_rs2_52 = 5; bs_input_n6_rs2_52 < 9; bs_input_n6_rs2_52 = bs_input_n6_rs2_52 + 1) {
    _preg_268435626 = Rotate(input_dup_n6_49, _cst_75[bs_input_n6_rs2_52]);
    Copy_ciph(&input_block_n6_51[bs_input_n6_rs2_52], &_preg_268435626);
    Free_data(&_preg_268435626._c1_poly);
    Free_data(&_preg_268435626._c0_poly);
  }
  Free_data(&input_dup_n6_49._c1_poly);
  Free_data(&input_dup_n6_49._c0_poly);
  Zero_ciph(&result_n7_53);
  for (num_grid_n7_54 = 0; num_grid_n7_54 < 128; num_grid_n7_54 = num_grid_n7_54 + 1) {
    Zero_ciph(&result_grid_n7_55);
    for (cap_block_n7_56 = 0; cap_block_n7_56 < 9; cap_block_n7_56 = cap_block_n7_56 + 1) {
      _ckks_gen_tmp_1108_167 = input_block_n6_51[cap_block_n7_56];
      _pgen_tmp_59_258 = *(PLAIN)Pt_from_msg(&_pgen_tmp_59_258, (cap_block_n7_56 + (num_grid_n7_54 * 9)) + 2590 /* cst_73_0 */, 16384, 1, 2, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_60_259, &_ckks_gen_tmp_1108_167, &_pgen_tmp_59_258);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n7_55, &result_grid_n7_55, &_pgen_tmp_60_259);
      _pgen_modulus_196 = Q_modulus();
      _pgen_num_q_197 = Poly_level(&result_grid_n7_55._c0_poly);
      for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_60_259._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_1108_167._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_59_258._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_60_259._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_1108_167._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_59_258._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modadd(Coeffs(&result_grid_n7_55._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_grid_n7_55._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_60_259._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modadd(Coeffs(&result_grid_n7_55._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_grid_n7_55._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_60_259._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        _pgen_modulus_196 = _pgen_modulus_196 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435627 = Rotate(result_grid_n7_55, num_grid_n7_54 * 64);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n7_53, &result_n7_53, &_preg_268435627);
    _pgen_modulus_196 = Q_modulus();
    _pgen_num_q_197 = Poly_level(&result_n7_53._c0_poly);
    for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
      Hw_modadd(Coeffs(&result_n7_53._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n7_53._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435627._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&result_n7_53._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n7_53._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435627._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      _pgen_modulus_196 = _pgen_modulus_196 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_data(&_preg_268435627._c1_poly);
    Free_data(&_preg_268435627._c0_poly);
  }
  Free_data(&_pgen_tmp_59_258._poly);
  Free_data(&_ckks_gen_tmp_1108_167._c1_poly);
  Free_data(&_ckks_gen_tmp_1108_167._c0_poly);
  Free_data(&_pgen_tmp_60_259._c1_poly);
  Free_data(&_pgen_tmp_60_259._c0_poly);
  Free_data(&result_grid_n7_55._c1_poly);
  Free_data(&result_grid_n7_55._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n7_53, &result_n7_53);
  Rescale(&result_n7_53._c0_poly, &result_n7_53._c0_poly);
  Rescale(&result_n7_53._c1_poly, &result_n7_53._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435628 = Rotate(result_n7_53, -16384);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_preg_268435567, &result_n7_53, &_preg_268435628);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_preg_268435567._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&_preg_268435567._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n7_53._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435628._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_preg_268435567._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n7_53._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435628._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_preg_268435628._c1_poly);
  Free_data(&_preg_268435628._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(14, 0);
  Bootstrap(&_ckks_gen_tmp_1144_168, &_preg_268435567, 15);
  Free_data(&_preg_268435567._c1_poly);
  Free_data(&_preg_268435567._c0_poly);
  CMPLR_TM_END(14, 0);
  _pgen_tmp_62_261 = *(PLAIN)Pt_from_msg(&_pgen_tmp_62_261, 3742 /* cst_76 */, 16384, 1, 15, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_61_260, &_ckks_gen_tmp_1144_168, &_pgen_tmp_62_261);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_61_260._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_61_260._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_1144_168._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_62_261._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_61_260._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_1144_168._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_62_261._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_62_261._poly);
  Free_data(&_ckks_gen_tmp_1144_168._c1_poly);
  Free_data(&_ckks_gen_tmp_1144_168._c0_poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435491, &_pgen_tmp_61_260);
  Rescale(&_preg_268435491._c0_poly, &_pgen_tmp_61_260._c0_poly);
  Rescale(&_preg_268435491._c1_poly, &_pgen_tmp_61_260._c1_poly);
  Free_data(&_pgen_tmp_61_260._c1_poly);
  Free_data(&_pgen_tmp_61_260._c0_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_63_262 = *(PLAIN)Pt_from_msg(&_pgen_tmp_63_262, 3743 /* cst_74 */, 16384, 1, 14, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n7_53, &_preg_268435491, &_pgen_tmp_63_262);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&result_n7_53._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&result_n7_53._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435491._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_63_262._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Set_coeffs(&result_n7_53._c1_poly, _pgen_rns_idx_198, degree, Coeffs(&_preg_268435491._c1_poly, _pgen_rns_idx_198, degree));
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_63_262._poly);
  Free_data(&_preg_268435491._c1_poly);
  Free_data(&_preg_268435491._c0_poly);
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435492, &result_n7_53);
  Free_data(&result_n7_53._c1_poly);
  Free_data(&result_n7_53._c0_poly);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 49
  // /features/features.16/Relu
  // pragma: 65536, 1033, 50
  CMPLR_TM_START(10, 0);
  Copy_ciph(&_preg_268435493, &_preg_268435492);
  Free_data(&_preg_268435492._c1_poly);
  Free_data(&_preg_268435492._c0_poly);
  Encode_float(&_pgen_tmp_65_264, &_cst_182, 1, 1, Level(&_preg_268435493), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_64_263, &_preg_268435493, &_pgen_tmp_65_264);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_64_263._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_64_263._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435493._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_65_264._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_64_263._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435493._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_65_264._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_65_264._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435495, &_pgen_tmp_64_263);
  Rescale(&_preg_268435495._c0_poly, &_pgen_tmp_64_263._c0_poly);
  Rescale(&_preg_268435495._c1_poly, &_pgen_tmp_64_263._c1_poly);
  Free_data(&_pgen_tmp_64_263._c1_poly);
  Free_data(&_pgen_tmp_64_263._c0_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435494 = App_relu_1(_preg_268435493, _preg_268435495, 16384);
  Free_data(&_preg_268435495._c1_poly);
  Free_data(&_preg_268435495._c0_poly);
  Free_data(&_preg_268435493._c1_poly);
  Free_data(&_preg_268435493._c0_poly);
  Copy_ciph(&_preg_268435496, &_preg_268435494);
  Free_data(&_preg_268435494._c1_poly);
  Free_data(&_preg_268435494._c0_poly);
  CMPLR_TM_END(10, 0);
  // pragma: 65537, 1033, 50
  // /features/features.17/Conv
  // pragma: 65536, 1027, 51
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435497, &_preg_268435496);
  Free_data(&_preg_268435496._c1_poly);
  Free_data(&_preg_268435496._c0_poly);
  // BlockingRot replicate=1
  Copy_ciph(&input_dup_n7_57, &_preg_268435497);
  Free_data(&_preg_268435497._c1_poly);
  Free_data(&_preg_268435497._c0_poly);
  // BlockingRot HRot=bs=9
  for (bs_input_n7_58 = 0; bs_input_n7_58 < 4; bs_input_n7_58 = bs_input_n7_58 + 1) {
    _preg_268435629 = Rotate(input_dup_n7_57, _cst_80[bs_input_n7_58]);
    Copy_ciph(&input_block_n7_59[bs_input_n7_58], &_preg_268435629);
    Free_data(&_preg_268435629._c1_poly);
    Free_data(&_preg_268435629._c0_poly);
  }
  _preg_268435630 = Rotate(input_dup_n7_57, 0);
  Copy_ciph(&input_block_n7_59[4], &_preg_268435630);
  Free_data(&_preg_268435630._c1_poly);
  Free_data(&_preg_268435630._c0_poly);
  for (bs_input_n7_rs2_60 = 5; bs_input_n7_rs2_60 < 9; bs_input_n7_rs2_60 = bs_input_n7_rs2_60 + 1) {
    _preg_268435631 = Rotate(input_dup_n7_57, _cst_80[bs_input_n7_rs2_60]);
    Copy_ciph(&input_block_n7_59[bs_input_n7_rs2_60], &_preg_268435631);
    Free_data(&_preg_268435631._c1_poly);
    Free_data(&_preg_268435631._c0_poly);
  }
  Free_data(&input_dup_n7_57._c1_poly);
  Free_data(&input_dup_n7_57._c0_poly);
  Zero_ciph(&result_n8_61);
  for (num_grid_n8_62 = 0; num_grid_n8_62 < 256; num_grid_n8_62 = num_grid_n8_62 + 1) {
    Zero_ciph(&result_grid_n8_63);
    for (cap_block_n8_64 = 0; cap_block_n8_64 < 9; cap_block_n8_64 = cap_block_n8_64 + 1) {
      _ckks_gen_tmp_1279_169 = input_block_n7_59[cap_block_n8_64];
      _pgen_tmp_66_265 = *(PLAIN)Pt_from_msg(&_pgen_tmp_66_265, (cap_block_n8_64 + (num_grid_n8_62 * 9)) + 3744 /* cst_78_0 */, 16384, 1, 2, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_67_266, &_ckks_gen_tmp_1279_169, &_pgen_tmp_66_265);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n8_63, &result_grid_n8_63, &_pgen_tmp_67_266);
      _pgen_modulus_196 = Q_modulus();
      _pgen_num_q_197 = Poly_level(&result_grid_n8_63._c0_poly);
      for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_67_266._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_1279_169._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_66_265._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_67_266._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_1279_169._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_66_265._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modadd(Coeffs(&result_grid_n8_63._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_grid_n8_63._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_67_266._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modadd(Coeffs(&result_grid_n8_63._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_grid_n8_63._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_67_266._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        _pgen_modulus_196 = _pgen_modulus_196 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435632 = Rotate(result_grid_n8_63, num_grid_n8_62 * 64);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n8_61, &result_n8_61, &_preg_268435632);
    _pgen_modulus_196 = Q_modulus();
    _pgen_num_q_197 = Poly_level(&result_n8_61._c0_poly);
    for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
      Hw_modadd(Coeffs(&result_n8_61._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n8_61._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435632._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&result_n8_61._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n8_61._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435632._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      _pgen_modulus_196 = _pgen_modulus_196 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_data(&_preg_268435632._c1_poly);
    Free_data(&_preg_268435632._c0_poly);
  }
  Free_data(&_pgen_tmp_66_265._poly);
  Free_data(&_ckks_gen_tmp_1279_169._c1_poly);
  Free_data(&_ckks_gen_tmp_1279_169._c0_poly);
  Free_data(&_pgen_tmp_67_266._c1_poly);
  Free_data(&_pgen_tmp_67_266._c0_poly);
  Free_data(&result_grid_n8_63._c1_poly);
  Free_data(&result_grid_n8_63._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n8_61, &result_n8_61);
  Rescale(&result_n8_61._c0_poly, &result_n8_61._c0_poly);
  Rescale(&result_n8_61._c1_poly, &result_n8_61._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435633 = Rotate(result_n8_61, -16384);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_preg_268435568, &result_n8_61, &_preg_268435633);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_preg_268435568._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&_preg_268435568._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n8_61._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435633._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_preg_268435568._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n8_61._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435633._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_preg_268435633._c1_poly);
  Free_data(&_preg_268435633._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(14, 0);
  Bootstrap(&_ckks_gen_tmp_1315_170, &_preg_268435568, 15);
  Free_data(&_preg_268435568._c1_poly);
  Free_data(&_preg_268435568._c0_poly);
  CMPLR_TM_END(14, 0);
  _pgen_tmp_69_268 = *(PLAIN)Pt_from_msg(&_pgen_tmp_69_268, 6048 /* cst_81 */, 16384, 1, 15, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_68_267, &_ckks_gen_tmp_1315_170, &_pgen_tmp_69_268);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_68_267._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_68_267._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_1315_170._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_69_268._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_68_267._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_1315_170._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_69_268._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_69_268._poly);
  Free_data(&_ckks_gen_tmp_1315_170._c1_poly);
  Free_data(&_ckks_gen_tmp_1315_170._c0_poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435498, &_pgen_tmp_68_267);
  Rescale(&_preg_268435498._c0_poly, &_pgen_tmp_68_267._c0_poly);
  Rescale(&_preg_268435498._c1_poly, &_pgen_tmp_68_267._c1_poly);
  Free_data(&_pgen_tmp_68_267._c1_poly);
  Free_data(&_pgen_tmp_68_267._c0_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_70_269 = *(PLAIN)Pt_from_msg(&_pgen_tmp_70_269, 6049 /* cst_79 */, 16384, 1, 14, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n8_61, &_preg_268435498, &_pgen_tmp_70_269);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&result_n8_61._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&result_n8_61._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435498._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_70_269._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Set_coeffs(&result_n8_61._c1_poly, _pgen_rns_idx_198, degree, Coeffs(&_preg_268435498._c1_poly, _pgen_rns_idx_198, degree));
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_70_269._poly);
  Free_data(&_preg_268435498._c1_poly);
  Free_data(&_preg_268435498._c0_poly);
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435499, &result_n8_61);
  Free_data(&result_n8_61._c1_poly);
  Free_data(&result_n8_61._c0_poly);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 51
  // /features/features.19/Relu
  // pragma: 65536, 1033, 52
  CMPLR_TM_START(10, 0);
  Copy_ciph(&_preg_268435500, &_preg_268435499);
  Free_data(&_preg_268435499._c1_poly);
  Free_data(&_preg_268435499._c0_poly);
  Encode_float(&_pgen_tmp_72_271, &_cst_183, 1, 1, Level(&_preg_268435500), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_71_270, &_preg_268435500, &_pgen_tmp_72_271);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_71_270._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_71_270._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435500._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_72_271._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_71_270._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435500._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_72_271._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_72_271._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435502, &_pgen_tmp_71_270);
  Rescale(&_preg_268435502._c0_poly, &_pgen_tmp_71_270._c0_poly);
  Rescale(&_preg_268435502._c1_poly, &_pgen_tmp_71_270._c1_poly);
  Free_data(&_pgen_tmp_71_270._c1_poly);
  Free_data(&_pgen_tmp_71_270._c0_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435501 = App_relu_1(_preg_268435500, _preg_268435502, 16384);
  Free_data(&_preg_268435502._c1_poly);
  Free_data(&_preg_268435502._c0_poly);
  Free_data(&_preg_268435500._c1_poly);
  Free_data(&_preg_268435500._c0_poly);
  Copy_ciph(&_preg_268435503, &_preg_268435501);
  Free_data(&_preg_268435501._c1_poly);
  Free_data(&_preg_268435501._c0_poly);
  CMPLR_TM_END(10, 0);
  // pragma: 65537, 1033, 52
  // /features/features.20/Conv
  // pragma: 65536, 1027, 53
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435504, &_preg_268435503);
  Free_data(&_preg_268435503._c1_poly);
  Free_data(&_preg_268435503._c0_poly);
  // BlockingRot replicate=1
  Copy_ciph(&input_dup_n8_65, &_preg_268435504);
  Free_data(&_preg_268435504._c1_poly);
  Free_data(&_preg_268435504._c0_poly);
  // BlockingRot HRot=bs=9
  for (bs_input_n8_66 = 0; bs_input_n8_66 < 4; bs_input_n8_66 = bs_input_n8_66 + 1) {
    _preg_268435634 = Rotate(input_dup_n8_65, _cst_85[bs_input_n8_66]);
    Copy_ciph(&input_block_n8_67[bs_input_n8_66], &_preg_268435634);
    Free_data(&_preg_268435634._c1_poly);
    Free_data(&_preg_268435634._c0_poly);
  }
  _preg_268435635 = Rotate(input_dup_n8_65, 0);
  Copy_ciph(&input_block_n8_67[4], &_preg_268435635);
  Free_data(&_preg_268435635._c1_poly);
  Free_data(&_preg_268435635._c0_poly);
  for (bs_input_n8_rs2_68 = 5; bs_input_n8_rs2_68 < 9; bs_input_n8_rs2_68 = bs_input_n8_rs2_68 + 1) {
    _preg_268435636 = Rotate(input_dup_n8_65, _cst_85[bs_input_n8_rs2_68]);
    Copy_ciph(&input_block_n8_67[bs_input_n8_rs2_68], &_preg_268435636);
    Free_data(&_preg_268435636._c1_poly);
    Free_data(&_preg_268435636._c0_poly);
  }
  Free_data(&input_dup_n8_65._c1_poly);
  Free_data(&input_dup_n8_65._c0_poly);
  Zero_ciph(&result_n9_69);
  for (num_grid_n9_70 = 0; num_grid_n9_70 < 256; num_grid_n9_70 = num_grid_n9_70 + 1) {
    Zero_ciph(&result_grid_n9_71);
    for (cap_block_n9_72 = 0; cap_block_n9_72 < 9; cap_block_n9_72 = cap_block_n9_72 + 1) {
      _ckks_gen_tmp_1450_171 = input_block_n8_67[cap_block_n9_72];
      _pgen_tmp_73_272 = *(PLAIN)Pt_from_msg(&_pgen_tmp_73_272, (cap_block_n9_72 + (num_grid_n9_70 * 9)) + 6050 /* cst_83_0 */, 16384, 1, 2, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_74_273, &_ckks_gen_tmp_1450_171, &_pgen_tmp_73_272);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n9_71, &result_grid_n9_71, &_pgen_tmp_74_273);
      _pgen_modulus_196 = Q_modulus();
      _pgen_num_q_197 = Poly_level(&result_grid_n9_71._c0_poly);
      for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_74_273._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_1450_171._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_73_272._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_74_273._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_1450_171._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_73_272._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modadd(Coeffs(&result_grid_n9_71._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_grid_n9_71._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_74_273._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modadd(Coeffs(&result_grid_n9_71._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_grid_n9_71._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_74_273._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        _pgen_modulus_196 = _pgen_modulus_196 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435637 = Rotate(result_grid_n9_71, num_grid_n9_70 * 64);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n9_69, &result_n9_69, &_preg_268435637);
    _pgen_modulus_196 = Q_modulus();
    _pgen_num_q_197 = Poly_level(&result_n9_69._c0_poly);
    for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
      Hw_modadd(Coeffs(&result_n9_69._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n9_69._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435637._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&result_n9_69._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n9_69._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435637._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      _pgen_modulus_196 = _pgen_modulus_196 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_data(&_preg_268435637._c1_poly);
    Free_data(&_preg_268435637._c0_poly);
  }
  Free_data(&_pgen_tmp_73_272._poly);
  Free_data(&_ckks_gen_tmp_1450_171._c1_poly);
  Free_data(&_ckks_gen_tmp_1450_171._c0_poly);
  Free_data(&_pgen_tmp_74_273._c1_poly);
  Free_data(&_pgen_tmp_74_273._c0_poly);
  Free_data(&result_grid_n9_71._c1_poly);
  Free_data(&result_grid_n9_71._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n9_69, &result_n9_69);
  Rescale(&result_n9_69._c0_poly, &result_n9_69._c0_poly);
  Rescale(&result_n9_69._c1_poly, &result_n9_69._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435638 = Rotate(result_n9_69, -16384);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_preg_268435569, &result_n9_69, &_preg_268435638);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_preg_268435569._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&_preg_268435569._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n9_69._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435638._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_preg_268435569._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n9_69._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435638._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_preg_268435638._c1_poly);
  Free_data(&_preg_268435638._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(14, 0);
  Bootstrap(&_ckks_gen_tmp_1486_172, &_preg_268435569, 14);
  Free_data(&_preg_268435569._c1_poly);
  Free_data(&_preg_268435569._c0_poly);
  CMPLR_TM_END(14, 0);
  _pgen_tmp_76_275 = *(PLAIN)Pt_from_msg(&_pgen_tmp_76_275, 8354 /* cst_86 */, 16384, 1, 14, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_75_274, &_ckks_gen_tmp_1486_172, &_pgen_tmp_76_275);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_75_274._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_75_274._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_1486_172._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_76_275._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_75_274._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_1486_172._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_76_275._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_76_275._poly);
  Free_data(&_ckks_gen_tmp_1486_172._c1_poly);
  Free_data(&_ckks_gen_tmp_1486_172._c0_poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435505, &_pgen_tmp_75_274);
  Rescale(&_preg_268435505._c0_poly, &_pgen_tmp_75_274._c0_poly);
  Rescale(&_preg_268435505._c1_poly, &_pgen_tmp_75_274._c1_poly);
  Free_data(&_pgen_tmp_75_274._c1_poly);
  Free_data(&_pgen_tmp_75_274._c0_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_77_276 = *(PLAIN)Pt_from_msg(&_pgen_tmp_77_276, 8355 /* cst_84 */, 16384, 1, 13, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n9_69, &_preg_268435505, &_pgen_tmp_77_276);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&result_n9_69._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&result_n9_69._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435505._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_77_276._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Set_coeffs(&result_n9_69._c1_poly, _pgen_rns_idx_198, degree, Coeffs(&_preg_268435505._c1_poly, _pgen_rns_idx_198, degree));
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_77_276._poly);
  Free_data(&_preg_268435505._c1_poly);
  Free_data(&_preg_268435505._c0_poly);
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435506, &result_n9_69);
  Free_data(&result_n9_69._c1_poly);
  Free_data(&result_n9_69._c0_poly);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 53
  // /features/features.22/Relu
  // pragma: 65536, 1033, 54
  CMPLR_TM_START(10, 0);
  Copy_ciph(&_preg_268435507, &_preg_268435506);
  Free_data(&_preg_268435506._c1_poly);
  Free_data(&_preg_268435506._c0_poly);
  Encode_float(&_pgen_tmp_79_278, &_cst_184, 1, 1, Level(&_preg_268435507), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_78_277, &_preg_268435507, &_pgen_tmp_79_278);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_78_277._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_78_277._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435507._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_79_278._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_78_277._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435507._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_79_278._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_79_278._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435509, &_pgen_tmp_78_277);
  Rescale(&_preg_268435509._c0_poly, &_pgen_tmp_78_277._c0_poly);
  Rescale(&_preg_268435509._c1_poly, &_pgen_tmp_78_277._c1_poly);
  Free_data(&_pgen_tmp_78_277._c1_poly);
  Free_data(&_pgen_tmp_78_277._c0_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435508 = App_relu_2(_preg_268435507, _preg_268435509, 16384);
  Free_data(&_preg_268435509._c1_poly);
  Free_data(&_preg_268435509._c0_poly);
  Free_data(&_preg_268435507._c1_poly);
  Free_data(&_preg_268435507._c0_poly);
  Copy_ciph(&_preg_268435510, &_preg_268435508);
  Free_data(&_preg_268435508._c1_poly);
  Free_data(&_preg_268435508._c0_poly);
  CMPLR_TM_END(10, 0);
  // pragma: 65537, 1033, 54
  // /features/features.23/AveragePool
  // pragma: 65536, 1026, 55
  CMPLR_TM_START(3, 0);
  Copy_ciph(&_preg_268435511, &_preg_268435510);
  Free_data(&_preg_268435510._c1_poly);
  Free_data(&_preg_268435510._c0_poly);
  _preg_268435639 = Rotate(_preg_268435511, 8);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&tmp_row_add9_73, &_preg_268435511, &_preg_268435639);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&tmp_row_add9_73._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&tmp_row_add9_73._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435511._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435639._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&tmp_row_add9_73._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435511._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435639._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_preg_268435639._c1_poly);
  Free_data(&_preg_268435639._c0_poly);
  Free_data(&_preg_268435511._c1_poly);
  Free_data(&_preg_268435511._c0_poly);
  CMPLR_TM_END(12, 0);
  _preg_268435640 = Rotate(tmp_row_add9_73, 1);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_preg_268435570, &tmp_row_add9_73, &_preg_268435640);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_preg_268435570._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&_preg_268435570._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&tmp_row_add9_73._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435640._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_preg_268435570._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&tmp_row_add9_73._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435640._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_preg_268435640._c1_poly);
  Free_data(&_preg_268435640._c0_poly);
  Free_data(&tmp_row_add9_73._c1_poly);
  Free_data(&tmp_row_add9_73._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(14, 0);
  Bootstrap(&_ckks_gen_tmp_1555_173, &_preg_268435570, 6);
  Free_data(&_preg_268435570._c1_poly);
  Free_data(&_preg_268435570._c0_poly);
  CMPLR_TM_END(14, 0);
  _pgen_tmp_81_280 = *(PLAIN)Pt_from_msg(&_pgen_tmp_81_280, 8356 /* cst_87 */, 16384, 1, 6, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_80_279, &_ckks_gen_tmp_1555_173, &_pgen_tmp_81_280);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_80_279._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_80_279._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_1555_173._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_81_280._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_80_279._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_1555_173._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_81_280._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_81_280._poly);
  Free_data(&_ckks_gen_tmp_1555_173._c1_poly);
  Free_data(&_ckks_gen_tmp_1555_173._c0_poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435512, &_pgen_tmp_80_279);
  Rescale(&_preg_268435512._c0_poly, &_pgen_tmp_80_279._c0_poly);
  Rescale(&_preg_268435512._c1_poly, &_pgen_tmp_80_279._c1_poly);
  Free_data(&_pgen_tmp_80_279._c1_poly);
  Free_data(&_pgen_tmp_80_279._c0_poly);
  CMPLR_TM_END(19, 0);
  Copy_ciph(&orig_result9_74, &_preg_268435512);
  Free_data(&_preg_268435512._c1_poly);
  Free_data(&_preg_268435512._c0_poly);
  Zero_ciph(&comb_row_result10_75);
  for (comb_row_index_n10_76 = 0; comb_row_index_n10_76 < 4; comb_row_index_n10_76 = comb_row_index_n10_76 + 1) {
    _preg_268435641 = Rotate(orig_result9_74, comb_row_index_n10_76 * 1);
    Copy_ciph(&_ckks_gen_tmp_1587_174, &_preg_268435641);
    _pgen_tmp_82_281 = *(PLAIN)Pt_from_msg(&_pgen_tmp_82_281, comb_row_index_n10_76 + 8357 /* cst_88_0 */, 16384, 1, 5, 0);
    CMPLR_TM_START(17, 0);
    Init_ciph_up_scale_plain(&_pgen_tmp_83_282, &_ckks_gen_tmp_1587_174, &_pgen_tmp_82_281);
    CMPLR_TM_END(17, 0);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&comb_row_result10_75, &comb_row_result10_75, &_pgen_tmp_83_282);
    _pgen_modulus_196 = Q_modulus();
    _pgen_num_q_197 = Poly_level(&comb_row_result10_75._c0_poly);
    for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_83_282._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_1587_174._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_82_281._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_83_282._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_1587_174._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_82_281._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&comb_row_result10_75._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&comb_row_result10_75._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_83_282._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&comb_row_result10_75._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&comb_row_result10_75._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_83_282._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      _pgen_modulus_196 = _pgen_modulus_196 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_data(&_preg_268435641._c1_poly);
    Free_data(&_preg_268435641._c0_poly);
  }
  Free_data(&orig_result9_74._c1_poly);
  Free_data(&orig_result9_74._c0_poly);
  Free_data(&_pgen_tmp_82_281._poly);
  Free_data(&_ckks_gen_tmp_1587_174._c1_poly);
  Free_data(&_ckks_gen_tmp_1587_174._c0_poly);
  Free_data(&_pgen_tmp_83_282._c1_poly);
  Free_data(&_pgen_tmp_83_282._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&comb_row_result10_75, &comb_row_result10_75);
  Rescale(&comb_row_result10_75._c0_poly, &comb_row_result10_75._c0_poly);
  Rescale(&comb_row_result10_75._c1_poly, &comb_row_result10_75._c1_poly);
  CMPLR_TM_END(19, 0);
  Zero_ciph(&comb_rc_result10_77);
  for (comb_rc_index_n10_78 = 0; comb_rc_index_n10_78 < 4; comb_rc_index_n10_78 = comb_rc_index_n10_78 + 1) {
    _preg_268435642 = Rotate(comb_row_result10_75, comb_rc_index_n10_78 * 12);
    Copy_ciph(&_ckks_gen_tmp_1621_175, &_preg_268435642);
    _pgen_tmp_84_283 = *(PLAIN)Pt_from_msg(&_pgen_tmp_84_283, comb_rc_index_n10_78 + 8361 /* cst_89_0 */, 16384, 1, 4, 0);
    CMPLR_TM_START(17, 0);
    Init_ciph_up_scale_plain(&_pgen_tmp_85_284, &_ckks_gen_tmp_1621_175, &_pgen_tmp_84_283);
    CMPLR_TM_END(17, 0);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&comb_rc_result10_77, &comb_rc_result10_77, &_pgen_tmp_85_284);
    _pgen_modulus_196 = Q_modulus();
    _pgen_num_q_197 = Poly_level(&comb_rc_result10_77._c0_poly);
    for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_85_284._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_1621_175._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_84_283._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_85_284._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_1621_175._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_84_283._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&comb_rc_result10_77._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&comb_rc_result10_77._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_85_284._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&comb_rc_result10_77._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&comb_rc_result10_77._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_85_284._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      _pgen_modulus_196 = _pgen_modulus_196 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_data(&_preg_268435642._c1_poly);
    Free_data(&_preg_268435642._c0_poly);
  }
  Free_data(&comb_row_result10_75._c1_poly);
  Free_data(&comb_row_result10_75._c0_poly);
  Free_data(&_pgen_tmp_84_283._poly);
  Free_data(&_ckks_gen_tmp_1621_175._c1_poly);
  Free_data(&_ckks_gen_tmp_1621_175._c0_poly);
  Free_data(&_pgen_tmp_85_284._c1_poly);
  Free_data(&_pgen_tmp_85_284._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&comb_rc_result10_77, &comb_rc_result10_77);
  Rescale(&comb_rc_result10_77._c0_poly, &comb_rc_result10_77._c0_poly);
  Rescale(&comb_rc_result10_77._c1_poly, &comb_rc_result10_77._c1_poly);
  CMPLR_TM_END(19, 0);
  Zero_ciph(&ss_result10_79);
  for (comb_cc_index_n10_80 = 0; comb_cc_index_n10_80 < 256; comb_cc_index_n10_80 = comb_cc_index_n10_80 + 1) {
    _preg_268435643 = Rotate(comb_rc_result10_77, comb_cc_index_n10_80 * 48);
    Copy_ciph(&_ckks_gen_tmp_1655_176, &_preg_268435643);
    _pgen_tmp_86_285 = *(PLAIN)Pt_from_msg(&_pgen_tmp_86_285, comb_cc_index_n10_80 + 8365 /* cst_90_0 */, 16384, 1, 3, 0);
    CMPLR_TM_START(17, 0);
    Init_ciph_up_scale_plain(&_pgen_tmp_87_286, &_ckks_gen_tmp_1655_176, &_pgen_tmp_86_285);
    CMPLR_TM_END(17, 0);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&ss_result10_79, &ss_result10_79, &_pgen_tmp_87_286);
    _pgen_modulus_196 = Q_modulus();
    _pgen_num_q_197 = Poly_level(&ss_result10_79._c0_poly);
    for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_87_286._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_1655_176._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_86_285._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_87_286._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_1655_176._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_86_285._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&ss_result10_79._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&ss_result10_79._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_87_286._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&ss_result10_79._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&ss_result10_79._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_87_286._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      _pgen_modulus_196 = _pgen_modulus_196 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_data(&_preg_268435643._c1_poly);
    Free_data(&_preg_268435643._c0_poly);
  }
  Free_data(&comb_rc_result10_77._c1_poly);
  Free_data(&comb_rc_result10_77._c0_poly);
  Free_data(&_pgen_tmp_86_285._poly);
  Free_data(&_ckks_gen_tmp_1655_176._c1_poly);
  Free_data(&_ckks_gen_tmp_1655_176._c0_poly);
  Free_data(&_pgen_tmp_87_286._c1_poly);
  Free_data(&_pgen_tmp_87_286._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&ss_result10_79, &ss_result10_79);
  Rescale(&ss_result10_79._c0_poly, &ss_result10_79._c0_poly);
  Rescale(&ss_result10_79._c1_poly, &ss_result10_79._c1_poly);
  CMPLR_TM_END(19, 0);
  Copy_ciph(&_preg_268435513, &ss_result10_79);
  Free_data(&ss_result10_79._c1_poly);
  Free_data(&ss_result10_79._c0_poly);
  CMPLR_TM_END(3, 0);
  // pragma: 65537, 1026, 55
  // /features/features.24/Conv
  // pragma: 65536, 1027, 56
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435514, &_preg_268435513);
  Free_data(&_preg_268435513._c1_poly);
  Free_data(&_preg_268435513._c0_poly);
  // BlockingRot replicate=2
  _preg_268435644 = Rotate(_preg_268435514, -4096);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&input_dup_n10_81, &_preg_268435514, &_preg_268435644);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&input_dup_n10_81._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&input_dup_n10_81._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435514._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435644._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&input_dup_n10_81._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435514._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435644._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_preg_268435644._c1_poly);
  Free_data(&_preg_268435644._c0_poly);
  Free_data(&_preg_268435514._c1_poly);
  Free_data(&_preg_268435514._c0_poly);
  CMPLR_TM_END(12, 0);
  // BlockingRot HRot=bs=9
  for (bs_input_n10_82 = 0; bs_input_n10_82 < 4; bs_input_n10_82 = bs_input_n10_82 + 1) {
    _preg_268435645 = Rotate(input_dup_n10_81, _cst_94[bs_input_n10_82]);
    Copy_ciph(&input_block_n10_83[bs_input_n10_82], &_preg_268435645);
    Free_data(&_preg_268435645._c1_poly);
    Free_data(&_preg_268435645._c0_poly);
  }
  _preg_268435646 = Rotate(input_dup_n10_81, 0);
  Copy_ciph(&input_block_n10_83[4], &_preg_268435646);
  Free_data(&_preg_268435646._c1_poly);
  Free_data(&_preg_268435646._c0_poly);
  for (bs_input_n10_rs2_84 = 5; bs_input_n10_rs2_84 < 9; bs_input_n10_rs2_84 = bs_input_n10_rs2_84 + 1) {
    _preg_268435647 = Rotate(input_dup_n10_81, _cst_94[bs_input_n10_rs2_84]);
    Copy_ciph(&input_block_n10_83[bs_input_n10_rs2_84], &_preg_268435647);
    Free_data(&_preg_268435647._c1_poly);
    Free_data(&_preg_268435647._c0_poly);
  }
  Free_data(&input_dup_n10_81._c1_poly);
  Free_data(&input_dup_n10_81._c0_poly);
  Zero_ciph(&result_n11_85);
  for (num_grid_n11_86 = 0; num_grid_n11_86 < 256; num_grid_n11_86 = num_grid_n11_86 + 1) {
    Zero_ciph(&result_grid_n11_87);
    for (cap_block_n11_88 = 0; cap_block_n11_88 < 9; cap_block_n11_88 = cap_block_n11_88 + 1) {
      _ckks_gen_tmp_1765_177 = input_block_n10_83[cap_block_n11_88];
      _pgen_tmp_88_287 = *(PLAIN)Pt_from_msg(&_pgen_tmp_88_287, (cap_block_n11_88 + (num_grid_n11_86 * 9)) + 8621 /* cst_92_0 */, 8192, 1, 2, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_89_288, &_ckks_gen_tmp_1765_177, &_pgen_tmp_88_287);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n11_87, &result_grid_n11_87, &_pgen_tmp_89_288);
      _pgen_modulus_196 = Q_modulus();
      _pgen_num_q_197 = Poly_level(&result_grid_n11_87._c0_poly);
      for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_89_288._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_1765_177._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_88_287._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_89_288._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_1765_177._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_88_287._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modadd(Coeffs(&result_grid_n11_87._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_grid_n11_87._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_89_288._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modadd(Coeffs(&result_grid_n11_87._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_grid_n11_87._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_89_288._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        _pgen_modulus_196 = _pgen_modulus_196 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435648 = Rotate(result_grid_n11_87, num_grid_n11_86 * 16);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n11_85, &result_n11_85, &_preg_268435648);
    _pgen_modulus_196 = Q_modulus();
    _pgen_num_q_197 = Poly_level(&result_n11_85._c0_poly);
    for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
      Hw_modadd(Coeffs(&result_n11_85._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n11_85._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435648._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&result_n11_85._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n11_85._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435648._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      _pgen_modulus_196 = _pgen_modulus_196 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_data(&_preg_268435648._c1_poly);
    Free_data(&_preg_268435648._c0_poly);
  }
  Free_data(&_pgen_tmp_88_287._poly);
  Free_data(&_ckks_gen_tmp_1765_177._c1_poly);
  Free_data(&_ckks_gen_tmp_1765_177._c0_poly);
  Free_data(&_pgen_tmp_89_288._c1_poly);
  Free_data(&_pgen_tmp_89_288._c0_poly);
  Free_data(&result_grid_n11_87._c1_poly);
  Free_data(&result_grid_n11_87._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n11_85, &result_n11_85);
  Rescale(&result_n11_85._c0_poly, &result_n11_85._c0_poly);
  Rescale(&result_n11_85._c1_poly, &result_n11_85._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435649 = Rotate(result_n11_85, -8192);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_preg_268435571, &result_n11_85, &_preg_268435649);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_preg_268435571._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&_preg_268435571._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n11_85._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435649._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_preg_268435571._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n11_85._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435649._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_preg_268435649._c1_poly);
  Free_data(&_preg_268435649._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(14, 0);
  Bootstrap(&_ckks_gen_tmp_1801_178, &_preg_268435571, 15);
  Free_data(&_preg_268435571._c1_poly);
  Free_data(&_preg_268435571._c0_poly);
  CMPLR_TM_END(14, 0);
  _pgen_tmp_91_290 = *(PLAIN)Pt_from_msg(&_pgen_tmp_91_290, 10925 /* cst_95 */, 8192, 1, 15, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_90_289, &_ckks_gen_tmp_1801_178, &_pgen_tmp_91_290);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_90_289._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_90_289._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_1801_178._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_91_290._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_90_289._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_1801_178._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_91_290._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_91_290._poly);
  Free_data(&_ckks_gen_tmp_1801_178._c1_poly);
  Free_data(&_ckks_gen_tmp_1801_178._c0_poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435515, &_pgen_tmp_90_289);
  Rescale(&_preg_268435515._c0_poly, &_pgen_tmp_90_289._c0_poly);
  Rescale(&_preg_268435515._c1_poly, &_pgen_tmp_90_289._c1_poly);
  Free_data(&_pgen_tmp_90_289._c1_poly);
  Free_data(&_pgen_tmp_90_289._c0_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_92_291 = *(PLAIN)Pt_from_msg(&_pgen_tmp_92_291, 10926 /* cst_93 */, 8192, 1, 14, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n11_85, &_preg_268435515, &_pgen_tmp_92_291);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&result_n11_85._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&result_n11_85._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435515._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_92_291._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Set_coeffs(&result_n11_85._c1_poly, _pgen_rns_idx_198, degree, Coeffs(&_preg_268435515._c1_poly, _pgen_rns_idx_198, degree));
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_92_291._poly);
  Free_data(&_preg_268435515._c1_poly);
  Free_data(&_preg_268435515._c0_poly);
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435516, &result_n11_85);
  Free_data(&result_n11_85._c1_poly);
  Free_data(&result_n11_85._c0_poly);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 56
  // /features/features.26/Relu
  // pragma: 65536, 1033, 57
  CMPLR_TM_START(10, 0);
  Copy_ciph(&_preg_268435517, &_preg_268435516);
  Free_data(&_preg_268435516._c1_poly);
  Free_data(&_preg_268435516._c0_poly);
  Encode_float(&_pgen_tmp_94_293, &_cst_185, 1, 1, Level(&_preg_268435517), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_93_292, &_preg_268435517, &_pgen_tmp_94_293);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_93_292._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_93_292._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435517._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_94_293._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_93_292._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435517._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_94_293._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_94_293._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435519, &_pgen_tmp_93_292);
  Rescale(&_preg_268435519._c0_poly, &_pgen_tmp_93_292._c0_poly);
  Rescale(&_preg_268435519._c1_poly, &_pgen_tmp_93_292._c1_poly);
  Free_data(&_pgen_tmp_93_292._c1_poly);
  Free_data(&_pgen_tmp_93_292._c0_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435518 = App_relu_1(_preg_268435517, _preg_268435519, 8192);
  Free_data(&_preg_268435519._c1_poly);
  Free_data(&_preg_268435519._c0_poly);
  Free_data(&_preg_268435517._c1_poly);
  Free_data(&_preg_268435517._c0_poly);
  Copy_ciph(&_preg_268435520, &_preg_268435518);
  Free_data(&_preg_268435518._c1_poly);
  Free_data(&_preg_268435518._c0_poly);
  CMPLR_TM_END(10, 0);
  // pragma: 65537, 1033, 57
  // /features/features.27/Conv
  // pragma: 65536, 1027, 58
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435521, &_preg_268435520);
  Free_data(&_preg_268435520._c1_poly);
  Free_data(&_preg_268435520._c0_poly);
  // BlockingRot replicate=1
  Copy_ciph(&input_dup_n11_89, &_preg_268435521);
  Free_data(&_preg_268435521._c1_poly);
  Free_data(&_preg_268435521._c0_poly);
  // BlockingRot HRot=bs=9
  for (bs_input_n11_90 = 0; bs_input_n11_90 < 4; bs_input_n11_90 = bs_input_n11_90 + 1) {
    _preg_268435650 = Rotate(input_dup_n11_89, _cst_99[bs_input_n11_90]);
    Copy_ciph(&input_block_n11_91[bs_input_n11_90], &_preg_268435650);
    Free_data(&_preg_268435650._c1_poly);
    Free_data(&_preg_268435650._c0_poly);
  }
  _preg_268435651 = Rotate(input_dup_n11_89, 0);
  Copy_ciph(&input_block_n11_91[4], &_preg_268435651);
  Free_data(&_preg_268435651._c1_poly);
  Free_data(&_preg_268435651._c0_poly);
  for (bs_input_n11_rs2_92 = 5; bs_input_n11_rs2_92 < 9; bs_input_n11_rs2_92 = bs_input_n11_rs2_92 + 1) {
    _preg_268435652 = Rotate(input_dup_n11_89, _cst_99[bs_input_n11_rs2_92]);
    Copy_ciph(&input_block_n11_91[bs_input_n11_rs2_92], &_preg_268435652);
    Free_data(&_preg_268435652._c1_poly);
    Free_data(&_preg_268435652._c0_poly);
  }
  Free_data(&input_dup_n11_89._c1_poly);
  Free_data(&input_dup_n11_89._c0_poly);
  Zero_ciph(&result_n12_93);
  for (num_grid_n12_94 = 0; num_grid_n12_94 < 512; num_grid_n12_94 = num_grid_n12_94 + 1) {
    Zero_ciph(&result_grid_n12_95);
    for (cap_block_n12_96 = 0; cap_block_n12_96 < 9; cap_block_n12_96 = cap_block_n12_96 + 1) {
      _ckks_gen_tmp_1936_179 = input_block_n11_91[cap_block_n12_96];
      _pgen_tmp_95_294 = *(PLAIN)Pt_from_msg(&_pgen_tmp_95_294, (cap_block_n12_96 + (num_grid_n12_94 * 9)) + 10927 /* cst_97_0 */, 8192, 1, 2, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_96_295, &_ckks_gen_tmp_1936_179, &_pgen_tmp_95_294);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n12_95, &result_grid_n12_95, &_pgen_tmp_96_295);
      _pgen_modulus_196 = Q_modulus();
      _pgen_num_q_197 = Poly_level(&result_grid_n12_95._c0_poly);
      for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_96_295._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_1936_179._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_95_294._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_96_295._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_1936_179._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_95_294._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modadd(Coeffs(&result_grid_n12_95._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_grid_n12_95._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_96_295._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modadd(Coeffs(&result_grid_n12_95._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_grid_n12_95._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_96_295._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        _pgen_modulus_196 = _pgen_modulus_196 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435653 = Rotate(result_grid_n12_95, num_grid_n12_94 * 16);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n12_93, &result_n12_93, &_preg_268435653);
    _pgen_modulus_196 = Q_modulus();
    _pgen_num_q_197 = Poly_level(&result_n12_93._c0_poly);
    for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
      Hw_modadd(Coeffs(&result_n12_93._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n12_93._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435653._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&result_n12_93._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n12_93._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435653._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      _pgen_modulus_196 = _pgen_modulus_196 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_data(&_preg_268435653._c1_poly);
    Free_data(&_preg_268435653._c0_poly);
  }
  Free_data(&_pgen_tmp_95_294._poly);
  Free_data(&_ckks_gen_tmp_1936_179._c1_poly);
  Free_data(&_ckks_gen_tmp_1936_179._c0_poly);
  Free_data(&_pgen_tmp_96_295._c1_poly);
  Free_data(&_pgen_tmp_96_295._c0_poly);
  Free_data(&result_grid_n12_95._c1_poly);
  Free_data(&result_grid_n12_95._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n12_93, &result_n12_93);
  Rescale(&result_n12_93._c0_poly, &result_n12_93._c0_poly);
  Rescale(&result_n12_93._c1_poly, &result_n12_93._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435654 = Rotate(result_n12_93, -8192);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_preg_268435572, &result_n12_93, &_preg_268435654);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_preg_268435572._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&_preg_268435572._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n12_93._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435654._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_preg_268435572._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n12_93._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435654._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_preg_268435654._c1_poly);
  Free_data(&_preg_268435654._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(14, 0);
  Bootstrap(&_ckks_gen_tmp_1972_180, &_preg_268435572, 15);
  Free_data(&_preg_268435572._c1_poly);
  Free_data(&_preg_268435572._c0_poly);
  CMPLR_TM_END(14, 0);
  _pgen_tmp_98_297 = *(PLAIN)Pt_from_msg(&_pgen_tmp_98_297, 15535 /* cst_100 */, 8192, 1, 15, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_97_296, &_ckks_gen_tmp_1972_180, &_pgen_tmp_98_297);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_97_296._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_97_296._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_1972_180._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_98_297._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_97_296._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_1972_180._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_98_297._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_98_297._poly);
  Free_data(&_ckks_gen_tmp_1972_180._c1_poly);
  Free_data(&_ckks_gen_tmp_1972_180._c0_poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435522, &_pgen_tmp_97_296);
  Rescale(&_preg_268435522._c0_poly, &_pgen_tmp_97_296._c0_poly);
  Rescale(&_preg_268435522._c1_poly, &_pgen_tmp_97_296._c1_poly);
  Free_data(&_pgen_tmp_97_296._c1_poly);
  Free_data(&_pgen_tmp_97_296._c0_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_99_298 = *(PLAIN)Pt_from_msg(&_pgen_tmp_99_298, 15536 /* cst_98 */, 8192, 1, 14, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n12_93, &_preg_268435522, &_pgen_tmp_99_298);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&result_n12_93._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&result_n12_93._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435522._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_99_298._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Set_coeffs(&result_n12_93._c1_poly, _pgen_rns_idx_198, degree, Coeffs(&_preg_268435522._c1_poly, _pgen_rns_idx_198, degree));
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_99_298._poly);
  Free_data(&_preg_268435522._c1_poly);
  Free_data(&_preg_268435522._c0_poly);
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435523, &result_n12_93);
  Free_data(&result_n12_93._c1_poly);
  Free_data(&result_n12_93._c0_poly);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 58
  // /features/features.29/Relu
  // pragma: 65536, 1033, 59
  CMPLR_TM_START(10, 0);
  Copy_ciph(&_preg_268435524, &_preg_268435523);
  Free_data(&_preg_268435523._c1_poly);
  Free_data(&_preg_268435523._c0_poly);
  Encode_float(&_pgen_tmp_101_300, &_cst_186, 1, 1, Level(&_preg_268435524), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_100_299, &_preg_268435524, &_pgen_tmp_101_300);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_100_299._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_100_299._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435524._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_101_300._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_100_299._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435524._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_101_300._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_101_300._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435526, &_pgen_tmp_100_299);
  Rescale(&_preg_268435526._c0_poly, &_pgen_tmp_100_299._c0_poly);
  Rescale(&_preg_268435526._c1_poly, &_pgen_tmp_100_299._c1_poly);
  Free_data(&_pgen_tmp_100_299._c1_poly);
  Free_data(&_pgen_tmp_100_299._c0_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435525 = App_relu_1(_preg_268435524, _preg_268435526, 8192);
  Free_data(&_preg_268435526._c1_poly);
  Free_data(&_preg_268435526._c0_poly);
  Free_data(&_preg_268435524._c1_poly);
  Free_data(&_preg_268435524._c0_poly);
  Copy_ciph(&_preg_268435527, &_preg_268435525);
  Free_data(&_preg_268435525._c1_poly);
  Free_data(&_preg_268435525._c0_poly);
  CMPLR_TM_END(10, 0);
  // pragma: 65537, 1033, 59
  // /features/features.30/Conv
  // pragma: 65536, 1027, 60
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435528, &_preg_268435527);
  Free_data(&_preg_268435527._c1_poly);
  Free_data(&_preg_268435527._c0_poly);
  // BlockingRot replicate=1
  Copy_ciph(&input_dup_n12_97, &_preg_268435528);
  Free_data(&_preg_268435528._c1_poly);
  Free_data(&_preg_268435528._c0_poly);
  // BlockingRot HRot=bs=9
  for (bs_input_n12_98 = 0; bs_input_n12_98 < 4; bs_input_n12_98 = bs_input_n12_98 + 1) {
    _preg_268435655 = Rotate(input_dup_n12_97, _cst_104[bs_input_n12_98]);
    Copy_ciph(&input_block_n12_99[bs_input_n12_98], &_preg_268435655);
    Free_data(&_preg_268435655._c1_poly);
    Free_data(&_preg_268435655._c0_poly);
  }
  _preg_268435656 = Rotate(input_dup_n12_97, 0);
  Copy_ciph(&input_block_n12_99[4], &_preg_268435656);
  Free_data(&_preg_268435656._c1_poly);
  Free_data(&_preg_268435656._c0_poly);
  for (bs_input_n12_rs2_100 = 5; bs_input_n12_rs2_100 < 9; bs_input_n12_rs2_100 = bs_input_n12_rs2_100 + 1) {
    _preg_268435657 = Rotate(input_dup_n12_97, _cst_104[bs_input_n12_rs2_100]);
    Copy_ciph(&input_block_n12_99[bs_input_n12_rs2_100], &_preg_268435657);
    Free_data(&_preg_268435657._c1_poly);
    Free_data(&_preg_268435657._c0_poly);
  }
  Free_data(&input_dup_n12_97._c1_poly);
  Free_data(&input_dup_n12_97._c0_poly);
  Zero_ciph(&result_n13_101);
  for (num_grid_n13_102 = 0; num_grid_n13_102 < 512; num_grid_n13_102 = num_grid_n13_102 + 1) {
    Zero_ciph(&result_grid_n13_103);
    for (cap_block_n13_104 = 0; cap_block_n13_104 < 9; cap_block_n13_104 = cap_block_n13_104 + 1) {
      _ckks_gen_tmp_2107_181 = input_block_n12_99[cap_block_n13_104];
      _pgen_tmp_102_301 = *(PLAIN)Pt_from_msg(&_pgen_tmp_102_301, (cap_block_n13_104 + (num_grid_n13_102 * 9)) + 15537 /* cst_102_0 */, 8192, 1, 2, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_103_302, &_ckks_gen_tmp_2107_181, &_pgen_tmp_102_301);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n13_103, &result_grid_n13_103, &_pgen_tmp_103_302);
      _pgen_modulus_196 = Q_modulus();
      _pgen_num_q_197 = Poly_level(&result_grid_n13_103._c0_poly);
      for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_103_302._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_2107_181._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_102_301._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_103_302._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_2107_181._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_102_301._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modadd(Coeffs(&result_grid_n13_103._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_grid_n13_103._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_103_302._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modadd(Coeffs(&result_grid_n13_103._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_grid_n13_103._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_103_302._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        _pgen_modulus_196 = _pgen_modulus_196 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435658 = Rotate(result_grid_n13_103, num_grid_n13_102 * 16);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n13_101, &result_n13_101, &_preg_268435658);
    _pgen_modulus_196 = Q_modulus();
    _pgen_num_q_197 = Poly_level(&result_n13_101._c0_poly);
    for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
      Hw_modadd(Coeffs(&result_n13_101._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n13_101._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435658._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&result_n13_101._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n13_101._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435658._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      _pgen_modulus_196 = _pgen_modulus_196 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_data(&_preg_268435658._c1_poly);
    Free_data(&_preg_268435658._c0_poly);
  }
  Free_data(&_pgen_tmp_102_301._poly);
  Free_data(&_ckks_gen_tmp_2107_181._c1_poly);
  Free_data(&_ckks_gen_tmp_2107_181._c0_poly);
  Free_data(&_pgen_tmp_103_302._c1_poly);
  Free_data(&_pgen_tmp_103_302._c0_poly);
  Free_data(&result_grid_n13_103._c1_poly);
  Free_data(&result_grid_n13_103._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n13_101, &result_n13_101);
  Rescale(&result_n13_101._c0_poly, &result_n13_101._c0_poly);
  Rescale(&result_n13_101._c1_poly, &result_n13_101._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435659 = Rotate(result_n13_101, -8192);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_preg_268435573, &result_n13_101, &_preg_268435659);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_preg_268435573._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&_preg_268435573._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n13_101._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435659._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_preg_268435573._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n13_101._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435659._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_preg_268435659._c1_poly);
  Free_data(&_preg_268435659._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(14, 0);
  Bootstrap(&_ckks_gen_tmp_2143_182, &_preg_268435573, 14);
  Free_data(&_preg_268435573._c1_poly);
  Free_data(&_preg_268435573._c0_poly);
  CMPLR_TM_END(14, 0);
  _pgen_tmp_105_304 = *(PLAIN)Pt_from_msg(&_pgen_tmp_105_304, 20145 /* cst_105 */, 8192, 1, 14, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_104_303, &_ckks_gen_tmp_2143_182, &_pgen_tmp_105_304);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_104_303._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_104_303._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_2143_182._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_105_304._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_104_303._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_2143_182._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_105_304._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_105_304._poly);
  Free_data(&_ckks_gen_tmp_2143_182._c1_poly);
  Free_data(&_ckks_gen_tmp_2143_182._c0_poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435529, &_pgen_tmp_104_303);
  Rescale(&_preg_268435529._c0_poly, &_pgen_tmp_104_303._c0_poly);
  Rescale(&_preg_268435529._c1_poly, &_pgen_tmp_104_303._c1_poly);
  Free_data(&_pgen_tmp_104_303._c1_poly);
  Free_data(&_pgen_tmp_104_303._c0_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_106_305 = *(PLAIN)Pt_from_msg(&_pgen_tmp_106_305, 20146 /* cst_103 */, 8192, 1, 13, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n13_101, &_preg_268435529, &_pgen_tmp_106_305);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&result_n13_101._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&result_n13_101._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435529._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_106_305._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Set_coeffs(&result_n13_101._c1_poly, _pgen_rns_idx_198, degree, Coeffs(&_preg_268435529._c1_poly, _pgen_rns_idx_198, degree));
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_106_305._poly);
  Free_data(&_preg_268435529._c1_poly);
  Free_data(&_preg_268435529._c0_poly);
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435530, &result_n13_101);
  Free_data(&result_n13_101._c1_poly);
  Free_data(&result_n13_101._c0_poly);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 60
  // /features/features.32/Relu
  // pragma: 65536, 1033, 61
  CMPLR_TM_START(10, 0);
  Copy_ciph(&_preg_268435531, &_preg_268435530);
  Free_data(&_preg_268435530._c1_poly);
  Free_data(&_preg_268435530._c0_poly);
  Encode_float(&_pgen_tmp_108_307, &_cst_187, 1, 1, Level(&_preg_268435531), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_107_306, &_preg_268435531, &_pgen_tmp_108_307);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_107_306._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_107_306._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435531._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_108_307._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_107_306._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435531._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_108_307._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_108_307._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435533, &_pgen_tmp_107_306);
  Rescale(&_preg_268435533._c0_poly, &_pgen_tmp_107_306._c0_poly);
  Rescale(&_preg_268435533._c1_poly, &_pgen_tmp_107_306._c1_poly);
  Free_data(&_pgen_tmp_107_306._c1_poly);
  Free_data(&_pgen_tmp_107_306._c0_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435532 = App_relu_2(_preg_268435531, _preg_268435533, 8192);
  Free_data(&_preg_268435533._c1_poly);
  Free_data(&_preg_268435533._c0_poly);
  Free_data(&_preg_268435531._c1_poly);
  Free_data(&_preg_268435531._c0_poly);
  Copy_ciph(&_preg_268435534, &_preg_268435532);
  Free_data(&_preg_268435532._c1_poly);
  Free_data(&_preg_268435532._c0_poly);
  CMPLR_TM_END(10, 0);
  // pragma: 65537, 1033, 61
  // /features/features.33/AveragePool
  // pragma: 65536, 1026, 62
  CMPLR_TM_START(3, 0);
  Copy_ciph(&_preg_268435535, &_preg_268435534);
  Free_data(&_preg_268435534._c1_poly);
  Free_data(&_preg_268435534._c0_poly);
  _preg_268435660 = Rotate(_preg_268435535, 4);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&tmp_row_add13_105, &_preg_268435535, &_preg_268435660);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&tmp_row_add13_105._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&tmp_row_add13_105._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435535._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435660._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&tmp_row_add13_105._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435535._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435660._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_preg_268435660._c1_poly);
  Free_data(&_preg_268435660._c0_poly);
  Free_data(&_preg_268435535._c1_poly);
  Free_data(&_preg_268435535._c0_poly);
  CMPLR_TM_END(12, 0);
  _preg_268435661 = Rotate(tmp_row_add13_105, 1);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_preg_268435574, &tmp_row_add13_105, &_preg_268435661);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_preg_268435574._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&_preg_268435574._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&tmp_row_add13_105._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435661._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_preg_268435574._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&tmp_row_add13_105._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435661._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_preg_268435661._c1_poly);
  Free_data(&_preg_268435661._c0_poly);
  Free_data(&tmp_row_add13_105._c1_poly);
  Free_data(&tmp_row_add13_105._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(14, 0);
  Bootstrap(&_ckks_gen_tmp_2212_183, &_preg_268435574, 6);
  Free_data(&_preg_268435574._c1_poly);
  Free_data(&_preg_268435574._c0_poly);
  CMPLR_TM_END(14, 0);
  _pgen_tmp_110_309 = *(PLAIN)Pt_from_msg(&_pgen_tmp_110_309, 20147 /* cst_106 */, 8192, 1, 6, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_109_308, &_ckks_gen_tmp_2212_183, &_pgen_tmp_110_309);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_109_308._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_109_308._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_2212_183._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_110_309._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_109_308._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_2212_183._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_110_309._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_110_309._poly);
  Free_data(&_ckks_gen_tmp_2212_183._c1_poly);
  Free_data(&_ckks_gen_tmp_2212_183._c0_poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435536, &_pgen_tmp_109_308);
  Rescale(&_preg_268435536._c0_poly, &_pgen_tmp_109_308._c0_poly);
  Rescale(&_preg_268435536._c1_poly, &_pgen_tmp_109_308._c1_poly);
  Free_data(&_pgen_tmp_109_308._c1_poly);
  Free_data(&_pgen_tmp_109_308._c0_poly);
  CMPLR_TM_END(19, 0);
  Copy_ciph(&orig_result13_106, &_preg_268435536);
  Free_data(&_preg_268435536._c1_poly);
  Free_data(&_preg_268435536._c0_poly);
  Zero_ciph(&comb_row_result14_107);
  for (comb_row_index_n14_108 = 0; comb_row_index_n14_108 < 2; comb_row_index_n14_108 = comb_row_index_n14_108 + 1) {
    _preg_268435662 = Rotate(orig_result13_106, comb_row_index_n14_108 * 1);
    Copy_ciph(&_ckks_gen_tmp_2244_184, &_preg_268435662);
    _pgen_tmp_111_310 = *(PLAIN)Pt_from_msg(&_pgen_tmp_111_310, comb_row_index_n14_108 + 20148 /* cst_107_0 */, 8192, 1, 5, 0);
    CMPLR_TM_START(17, 0);
    Init_ciph_up_scale_plain(&_pgen_tmp_112_311, &_ckks_gen_tmp_2244_184, &_pgen_tmp_111_310);
    CMPLR_TM_END(17, 0);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&comb_row_result14_107, &comb_row_result14_107, &_pgen_tmp_112_311);
    _pgen_modulus_196 = Q_modulus();
    _pgen_num_q_197 = Poly_level(&comb_row_result14_107._c0_poly);
    for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_112_311._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_2244_184._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_111_310._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_112_311._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_2244_184._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_111_310._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&comb_row_result14_107._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&comb_row_result14_107._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_112_311._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&comb_row_result14_107._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&comb_row_result14_107._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_112_311._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      _pgen_modulus_196 = _pgen_modulus_196 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_data(&_preg_268435662._c1_poly);
    Free_data(&_preg_268435662._c0_poly);
  }
  Free_data(&orig_result13_106._c1_poly);
  Free_data(&orig_result13_106._c0_poly);
  Free_data(&_pgen_tmp_111_310._poly);
  Free_data(&_ckks_gen_tmp_2244_184._c1_poly);
  Free_data(&_ckks_gen_tmp_2244_184._c0_poly);
  Free_data(&_pgen_tmp_112_311._c1_poly);
  Free_data(&_pgen_tmp_112_311._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&comb_row_result14_107, &comb_row_result14_107);
  Rescale(&comb_row_result14_107._c0_poly, &comb_row_result14_107._c0_poly);
  Rescale(&comb_row_result14_107._c1_poly, &comb_row_result14_107._c1_poly);
  CMPLR_TM_END(19, 0);
  Zero_ciph(&comb_rc_result14_109);
  for (comb_rc_index_n14_110 = 0; comb_rc_index_n14_110 < 2; comb_rc_index_n14_110 = comb_rc_index_n14_110 + 1) {
    _preg_268435663 = Rotate(comb_row_result14_107, comb_rc_index_n14_110 * 6);
    Copy_ciph(&_ckks_gen_tmp_2278_185, &_preg_268435663);
    _pgen_tmp_113_312 = *(PLAIN)Pt_from_msg(&_pgen_tmp_113_312, comb_rc_index_n14_110 + 20150 /* cst_108_0 */, 8192, 1, 4, 0);
    CMPLR_TM_START(17, 0);
    Init_ciph_up_scale_plain(&_pgen_tmp_114_313, &_ckks_gen_tmp_2278_185, &_pgen_tmp_113_312);
    CMPLR_TM_END(17, 0);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&comb_rc_result14_109, &comb_rc_result14_109, &_pgen_tmp_114_313);
    _pgen_modulus_196 = Q_modulus();
    _pgen_num_q_197 = Poly_level(&comb_rc_result14_109._c0_poly);
    for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_114_313._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_2278_185._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_113_312._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_114_313._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_2278_185._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_113_312._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&comb_rc_result14_109._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&comb_rc_result14_109._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_114_313._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&comb_rc_result14_109._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&comb_rc_result14_109._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_114_313._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      _pgen_modulus_196 = _pgen_modulus_196 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_data(&_preg_268435663._c1_poly);
    Free_data(&_preg_268435663._c0_poly);
  }
  Free_data(&comb_row_result14_107._c1_poly);
  Free_data(&comb_row_result14_107._c0_poly);
  Free_data(&_pgen_tmp_113_312._poly);
  Free_data(&_ckks_gen_tmp_2278_185._c1_poly);
  Free_data(&_ckks_gen_tmp_2278_185._c0_poly);
  Free_data(&_pgen_tmp_114_313._c1_poly);
  Free_data(&_pgen_tmp_114_313._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&comb_rc_result14_109, &comb_rc_result14_109);
  Rescale(&comb_rc_result14_109._c0_poly, &comb_rc_result14_109._c0_poly);
  Rescale(&comb_rc_result14_109._c1_poly, &comb_rc_result14_109._c1_poly);
  CMPLR_TM_END(19, 0);
  Zero_ciph(&ss_result14_111);
  for (comb_cc_index_n14_112 = 0; comb_cc_index_n14_112 < 512; comb_cc_index_n14_112 = comb_cc_index_n14_112 + 1) {
    _preg_268435664 = Rotate(comb_rc_result14_109, comb_cc_index_n14_112 * 12);
    Copy_ciph(&_ckks_gen_tmp_2312_186, &_preg_268435664);
    _pgen_tmp_115_314 = *(PLAIN)Pt_from_msg(&_pgen_tmp_115_314, comb_cc_index_n14_112 + 20152 /* cst_109_0 */, 8192, 1, 3, 0);
    CMPLR_TM_START(17, 0);
    Init_ciph_up_scale_plain(&_pgen_tmp_116_315, &_ckks_gen_tmp_2312_186, &_pgen_tmp_115_314);
    CMPLR_TM_END(17, 0);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&ss_result14_111, &ss_result14_111, &_pgen_tmp_116_315);
    _pgen_modulus_196 = Q_modulus();
    _pgen_num_q_197 = Poly_level(&ss_result14_111._c0_poly);
    for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_116_315._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_2312_186._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_115_314._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_116_315._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_2312_186._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_115_314._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&ss_result14_111._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&ss_result14_111._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_116_315._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&ss_result14_111._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&ss_result14_111._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_116_315._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      _pgen_modulus_196 = _pgen_modulus_196 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_data(&_preg_268435664._c1_poly);
    Free_data(&_preg_268435664._c0_poly);
  }
  Free_data(&comb_rc_result14_109._c1_poly);
  Free_data(&comb_rc_result14_109._c0_poly);
  Free_data(&_pgen_tmp_115_314._poly);
  Free_data(&_ckks_gen_tmp_2312_186._c1_poly);
  Free_data(&_ckks_gen_tmp_2312_186._c0_poly);
  Free_data(&_pgen_tmp_116_315._c1_poly);
  Free_data(&_pgen_tmp_116_315._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&ss_result14_111, &ss_result14_111);
  Rescale(&ss_result14_111._c0_poly, &ss_result14_111._c0_poly);
  Rescale(&ss_result14_111._c1_poly, &ss_result14_111._c1_poly);
  CMPLR_TM_END(19, 0);
  Copy_ciph(&_preg_268435537, &ss_result14_111);
  Free_data(&ss_result14_111._c1_poly);
  Free_data(&ss_result14_111._c0_poly);
  CMPLR_TM_END(3, 0);
  // pragma: 65537, 1026, 62
  // /features/features.34/Conv
  // pragma: 65536, 1027, 63
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435538, &_preg_268435537);
  Free_data(&_preg_268435537._c1_poly);
  Free_data(&_preg_268435537._c0_poly);
  // BlockingRot replicate=1
  Copy_ciph(&input_dup_n14_113, &_preg_268435538);
  Free_data(&_preg_268435538._c1_poly);
  Free_data(&_preg_268435538._c0_poly);
  // BlockingRot HRot=bs=9
  for (bs_input_n14_114 = 0; bs_input_n14_114 < 4; bs_input_n14_114 = bs_input_n14_114 + 1) {
    _preg_268435665 = Rotate(input_dup_n14_113, _cst_113[bs_input_n14_114]);
    Copy_ciph(&input_block_n14_115[bs_input_n14_114], &_preg_268435665);
    Free_data(&_preg_268435665._c1_poly);
    Free_data(&_preg_268435665._c0_poly);
  }
  _preg_268435666 = Rotate(input_dup_n14_113, 0);
  Copy_ciph(&input_block_n14_115[4], &_preg_268435666);
  Free_data(&_preg_268435666._c1_poly);
  Free_data(&_preg_268435666._c0_poly);
  for (bs_input_n14_rs2_116 = 5; bs_input_n14_rs2_116 < 9; bs_input_n14_rs2_116 = bs_input_n14_rs2_116 + 1) {
    _preg_268435667 = Rotate(input_dup_n14_113, _cst_113[bs_input_n14_rs2_116]);
    Copy_ciph(&input_block_n14_115[bs_input_n14_rs2_116], &_preg_268435667);
    Free_data(&_preg_268435667._c1_poly);
    Free_data(&_preg_268435667._c0_poly);
  }
  Free_data(&input_dup_n14_113._c1_poly);
  Free_data(&input_dup_n14_113._c0_poly);
  Zero_ciph(&result_n15_117);
  for (num_grid_n15_118 = 0; num_grid_n15_118 < 512; num_grid_n15_118 = num_grid_n15_118 + 1) {
    Zero_ciph(&result_grid_n15_119);
    for (cap_block_n15_120 = 0; cap_block_n15_120 < 9; cap_block_n15_120 = cap_block_n15_120 + 1) {
      _ckks_gen_tmp_2418_187 = input_block_n14_115[cap_block_n15_120];
      _pgen_tmp_117_316 = *(PLAIN)Pt_from_msg(&_pgen_tmp_117_316, (cap_block_n15_120 + (num_grid_n15_118 * 9)) + 20664 /* cst_111_0 */, 2048, 1, 2, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_118_317, &_ckks_gen_tmp_2418_187, &_pgen_tmp_117_316);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n15_119, &result_grid_n15_119, &_pgen_tmp_118_317);
      _pgen_modulus_196 = Q_modulus();
      _pgen_num_q_197 = Poly_level(&result_grid_n15_119._c0_poly);
      for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_118_317._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_2418_187._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_117_316._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_118_317._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_2418_187._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_117_316._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modadd(Coeffs(&result_grid_n15_119._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_grid_n15_119._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_118_317._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modadd(Coeffs(&result_grid_n15_119._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_grid_n15_119._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_118_317._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        _pgen_modulus_196 = _pgen_modulus_196 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435668 = Rotate(result_grid_n15_119, num_grid_n15_118 * 4);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n15_117, &result_n15_117, &_preg_268435668);
    _pgen_modulus_196 = Q_modulus();
    _pgen_num_q_197 = Poly_level(&result_n15_117._c0_poly);
    for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
      Hw_modadd(Coeffs(&result_n15_117._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n15_117._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435668._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&result_n15_117._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n15_117._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435668._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      _pgen_modulus_196 = _pgen_modulus_196 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_data(&_preg_268435668._c1_poly);
    Free_data(&_preg_268435668._c0_poly);
  }
  Free_data(&_pgen_tmp_117_316._poly);
  Free_data(&_ckks_gen_tmp_2418_187._c1_poly);
  Free_data(&_ckks_gen_tmp_2418_187._c0_poly);
  Free_data(&_pgen_tmp_118_317._c1_poly);
  Free_data(&_pgen_tmp_118_317._c0_poly);
  Free_data(&result_grid_n15_119._c1_poly);
  Free_data(&result_grid_n15_119._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n15_117, &result_n15_117);
  Rescale(&result_n15_117._c0_poly, &result_n15_117._c0_poly);
  Rescale(&result_n15_117._c1_poly, &result_n15_117._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435669 = Rotate(result_n15_117, -2048);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_preg_268435575, &result_n15_117, &_preg_268435669);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_preg_268435575._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&_preg_268435575._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n15_117._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435669._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_preg_268435575._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n15_117._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435669._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_preg_268435669._c1_poly);
  Free_data(&_preg_268435669._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(14, 0);
  Bootstrap(&_ckks_gen_tmp_2454_188, &_preg_268435575, 15);
  Free_data(&_preg_268435575._c1_poly);
  Free_data(&_preg_268435575._c0_poly);
  CMPLR_TM_END(14, 0);
  _pgen_tmp_120_319 = *(PLAIN)Pt_from_msg(&_pgen_tmp_120_319, 25272 /* cst_114 */, 2048, 1, 15, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_119_318, &_ckks_gen_tmp_2454_188, &_pgen_tmp_120_319);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_119_318._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_119_318._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_2454_188._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_120_319._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_119_318._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_2454_188._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_120_319._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_120_319._poly);
  Free_data(&_ckks_gen_tmp_2454_188._c1_poly);
  Free_data(&_ckks_gen_tmp_2454_188._c0_poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435539, &_pgen_tmp_119_318);
  Rescale(&_preg_268435539._c0_poly, &_pgen_tmp_119_318._c0_poly);
  Rescale(&_preg_268435539._c1_poly, &_pgen_tmp_119_318._c1_poly);
  Free_data(&_pgen_tmp_119_318._c1_poly);
  Free_data(&_pgen_tmp_119_318._c0_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_121_320 = *(PLAIN)Pt_from_msg(&_pgen_tmp_121_320, 25273 /* cst_112 */, 2048, 1, 14, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n15_117, &_preg_268435539, &_pgen_tmp_121_320);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&result_n15_117._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&result_n15_117._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435539._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_121_320._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Set_coeffs(&result_n15_117._c1_poly, _pgen_rns_idx_198, degree, Coeffs(&_preg_268435539._c1_poly, _pgen_rns_idx_198, degree));
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_121_320._poly);
  Free_data(&_preg_268435539._c1_poly);
  Free_data(&_preg_268435539._c0_poly);
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435540, &result_n15_117);
  Free_data(&result_n15_117._c1_poly);
  Free_data(&result_n15_117._c0_poly);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 63
  // /features/features.36/Relu
  // pragma: 65536, 1033, 64
  CMPLR_TM_START(10, 0);
  Copy_ciph(&_preg_268435541, &_preg_268435540);
  Free_data(&_preg_268435540._c1_poly);
  Free_data(&_preg_268435540._c0_poly);
  Encode_float(&_pgen_tmp_123_322, &_cst_188, 1, 1, Level(&_preg_268435541), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_122_321, &_preg_268435541, &_pgen_tmp_123_322);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_122_321._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_122_321._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435541._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_123_322._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_122_321._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435541._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_123_322._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_123_322._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435543, &_pgen_tmp_122_321);
  Rescale(&_preg_268435543._c0_poly, &_pgen_tmp_122_321._c0_poly);
  Rescale(&_preg_268435543._c1_poly, &_pgen_tmp_122_321._c1_poly);
  Free_data(&_pgen_tmp_122_321._c1_poly);
  Free_data(&_pgen_tmp_122_321._c0_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435542 = App_relu_1(_preg_268435541, _preg_268435543, 2048);
  Free_data(&_preg_268435543._c1_poly);
  Free_data(&_preg_268435543._c0_poly);
  Free_data(&_preg_268435541._c1_poly);
  Free_data(&_preg_268435541._c0_poly);
  Copy_ciph(&_preg_268435544, &_preg_268435542);
  Free_data(&_preg_268435542._c1_poly);
  Free_data(&_preg_268435542._c0_poly);
  CMPLR_TM_END(10, 0);
  // pragma: 65537, 1033, 64
  // /features/features.37/Conv
  // pragma: 65536, 1027, 65
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435545, &_preg_268435544);
  Free_data(&_preg_268435544._c1_poly);
  Free_data(&_preg_268435544._c0_poly);
  // BlockingRot replicate=1
  Copy_ciph(&input_dup_n15_121, &_preg_268435545);
  Free_data(&_preg_268435545._c1_poly);
  Free_data(&_preg_268435545._c0_poly);
  // BlockingRot HRot=bs=9
  for (bs_input_n15_122 = 0; bs_input_n15_122 < 4; bs_input_n15_122 = bs_input_n15_122 + 1) {
    _preg_268435670 = Rotate(input_dup_n15_121, _cst_118[bs_input_n15_122]);
    Copy_ciph(&input_block_n15_123[bs_input_n15_122], &_preg_268435670);
    Free_data(&_preg_268435670._c1_poly);
    Free_data(&_preg_268435670._c0_poly);
  }
  _preg_268435671 = Rotate(input_dup_n15_121, 0);
  Copy_ciph(&input_block_n15_123[4], &_preg_268435671);
  Free_data(&_preg_268435671._c1_poly);
  Free_data(&_preg_268435671._c0_poly);
  for (bs_input_n15_rs2_124 = 5; bs_input_n15_rs2_124 < 9; bs_input_n15_rs2_124 = bs_input_n15_rs2_124 + 1) {
    _preg_268435672 = Rotate(input_dup_n15_121, _cst_118[bs_input_n15_rs2_124]);
    Copy_ciph(&input_block_n15_123[bs_input_n15_rs2_124], &_preg_268435672);
    Free_data(&_preg_268435672._c1_poly);
    Free_data(&_preg_268435672._c0_poly);
  }
  Free_data(&input_dup_n15_121._c1_poly);
  Free_data(&input_dup_n15_121._c0_poly);
  Zero_ciph(&result_n16_125);
  for (num_grid_n16_126 = 0; num_grid_n16_126 < 512; num_grid_n16_126 = num_grid_n16_126 + 1) {
    Zero_ciph(&result_grid_n16_127);
    for (cap_block_n16_128 = 0; cap_block_n16_128 < 9; cap_block_n16_128 = cap_block_n16_128 + 1) {
      _ckks_gen_tmp_2589_189 = input_block_n15_123[cap_block_n16_128];
      _pgen_tmp_124_323 = *(PLAIN)Pt_from_msg(&_pgen_tmp_124_323, (cap_block_n16_128 + (num_grid_n16_126 * 9)) + 25274 /* cst_116_0 */, 2048, 1, 2, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_125_324, &_ckks_gen_tmp_2589_189, &_pgen_tmp_124_323);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n16_127, &result_grid_n16_127, &_pgen_tmp_125_324);
      _pgen_modulus_196 = Q_modulus();
      _pgen_num_q_197 = Poly_level(&result_grid_n16_127._c0_poly);
      for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_125_324._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_2589_189._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_124_323._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_125_324._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_2589_189._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_124_323._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modadd(Coeffs(&result_grid_n16_127._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_grid_n16_127._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_125_324._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modadd(Coeffs(&result_grid_n16_127._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_grid_n16_127._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_125_324._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        _pgen_modulus_196 = _pgen_modulus_196 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435673 = Rotate(result_grid_n16_127, num_grid_n16_126 * 4);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n16_125, &result_n16_125, &_preg_268435673);
    _pgen_modulus_196 = Q_modulus();
    _pgen_num_q_197 = Poly_level(&result_n16_125._c0_poly);
    for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
      Hw_modadd(Coeffs(&result_n16_125._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n16_125._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435673._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&result_n16_125._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n16_125._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435673._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      _pgen_modulus_196 = _pgen_modulus_196 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_data(&_preg_268435673._c1_poly);
    Free_data(&_preg_268435673._c0_poly);
  }
  Free_data(&_pgen_tmp_124_323._poly);
  Free_data(&_ckks_gen_tmp_2589_189._c1_poly);
  Free_data(&_ckks_gen_tmp_2589_189._c0_poly);
  Free_data(&_pgen_tmp_125_324._c1_poly);
  Free_data(&_pgen_tmp_125_324._c0_poly);
  Free_data(&result_grid_n16_127._c1_poly);
  Free_data(&result_grid_n16_127._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n16_125, &result_n16_125);
  Rescale(&result_n16_125._c0_poly, &result_n16_125._c0_poly);
  Rescale(&result_n16_125._c1_poly, &result_n16_125._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435674 = Rotate(result_n16_125, -2048);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_preg_268435576, &result_n16_125, &_preg_268435674);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_preg_268435576._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&_preg_268435576._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n16_125._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435674._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_preg_268435576._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n16_125._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435674._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_preg_268435674._c1_poly);
  Free_data(&_preg_268435674._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(14, 0);
  Bootstrap(&_ckks_gen_tmp_2625_190, &_preg_268435576, 15);
  Free_data(&_preg_268435576._c1_poly);
  Free_data(&_preg_268435576._c0_poly);
  CMPLR_TM_END(14, 0);
  _pgen_tmp_127_326 = *(PLAIN)Pt_from_msg(&_pgen_tmp_127_326, 29882 /* cst_119 */, 2048, 1, 15, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_126_325, &_ckks_gen_tmp_2625_190, &_pgen_tmp_127_326);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_126_325._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_126_325._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_2625_190._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_127_326._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_126_325._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_2625_190._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_127_326._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_127_326._poly);
  Free_data(&_ckks_gen_tmp_2625_190._c1_poly);
  Free_data(&_ckks_gen_tmp_2625_190._c0_poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435546, &_pgen_tmp_126_325);
  Rescale(&_preg_268435546._c0_poly, &_pgen_tmp_126_325._c0_poly);
  Rescale(&_preg_268435546._c1_poly, &_pgen_tmp_126_325._c1_poly);
  Free_data(&_pgen_tmp_126_325._c1_poly);
  Free_data(&_pgen_tmp_126_325._c0_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_128_327 = *(PLAIN)Pt_from_msg(&_pgen_tmp_128_327, 29883 /* cst_117 */, 2048, 1, 14, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n16_125, &_preg_268435546, &_pgen_tmp_128_327);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&result_n16_125._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&result_n16_125._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435546._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_128_327._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Set_coeffs(&result_n16_125._c1_poly, _pgen_rns_idx_198, degree, Coeffs(&_preg_268435546._c1_poly, _pgen_rns_idx_198, degree));
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_128_327._poly);
  Free_data(&_preg_268435546._c1_poly);
  Free_data(&_preg_268435546._c0_poly);
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435547, &result_n16_125);
  Free_data(&result_n16_125._c1_poly);
  Free_data(&result_n16_125._c0_poly);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 65
  // /features/features.39/Relu
  // pragma: 65536, 1033, 66
  CMPLR_TM_START(10, 0);
  Copy_ciph(&_preg_268435548, &_preg_268435547);
  Free_data(&_preg_268435547._c1_poly);
  Free_data(&_preg_268435547._c0_poly);
  Encode_float(&_pgen_tmp_130_329, &_cst_189, 1, 1, Level(&_preg_268435548), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_129_328, &_preg_268435548, &_pgen_tmp_130_329);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_129_328._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_129_328._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435548._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_130_329._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_129_328._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435548._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_130_329._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_130_329._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435550, &_pgen_tmp_129_328);
  Rescale(&_preg_268435550._c0_poly, &_pgen_tmp_129_328._c0_poly);
  Rescale(&_preg_268435550._c1_poly, &_pgen_tmp_129_328._c1_poly);
  Free_data(&_pgen_tmp_129_328._c1_poly);
  Free_data(&_pgen_tmp_129_328._c0_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435549 = App_relu_1(_preg_268435548, _preg_268435550, 2048);
  Free_data(&_preg_268435550._c1_poly);
  Free_data(&_preg_268435550._c0_poly);
  Free_data(&_preg_268435548._c1_poly);
  Free_data(&_preg_268435548._c0_poly);
  Copy_ciph(&_preg_268435551, &_preg_268435549);
  Free_data(&_preg_268435549._c1_poly);
  Free_data(&_preg_268435549._c0_poly);
  CMPLR_TM_END(10, 0);
  // pragma: 65537, 1033, 66
  // /features/features.40/Conv
  // pragma: 65536, 1027, 67
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435552, &_preg_268435551);
  Free_data(&_preg_268435551._c1_poly);
  Free_data(&_preg_268435551._c0_poly);
  // BlockingRot replicate=1
  Copy_ciph(&input_dup_n16_129, &_preg_268435552);
  Free_data(&_preg_268435552._c1_poly);
  Free_data(&_preg_268435552._c0_poly);
  // BlockingRot HRot=bs=9
  for (bs_input_n16_130 = 0; bs_input_n16_130 < 4; bs_input_n16_130 = bs_input_n16_130 + 1) {
    _preg_268435675 = Rotate(input_dup_n16_129, _cst_123[bs_input_n16_130]);
    Copy_ciph(&input_block_n16_131[bs_input_n16_130], &_preg_268435675);
    Free_data(&_preg_268435675._c1_poly);
    Free_data(&_preg_268435675._c0_poly);
  }
  _preg_268435676 = Rotate(input_dup_n16_129, 0);
  Copy_ciph(&input_block_n16_131[4], &_preg_268435676);
  Free_data(&_preg_268435676._c1_poly);
  Free_data(&_preg_268435676._c0_poly);
  for (bs_input_n16_rs2_132 = 5; bs_input_n16_rs2_132 < 9; bs_input_n16_rs2_132 = bs_input_n16_rs2_132 + 1) {
    _preg_268435677 = Rotate(input_dup_n16_129, _cst_123[bs_input_n16_rs2_132]);
    Copy_ciph(&input_block_n16_131[bs_input_n16_rs2_132], &_preg_268435677);
    Free_data(&_preg_268435677._c1_poly);
    Free_data(&_preg_268435677._c0_poly);
  }
  Free_data(&input_dup_n16_129._c1_poly);
  Free_data(&input_dup_n16_129._c0_poly);
  Zero_ciph(&result_n17_133);
  for (num_grid_n17_134 = 0; num_grid_n17_134 < 512; num_grid_n17_134 = num_grid_n17_134 + 1) {
    Zero_ciph(&result_grid_n17_135);
    for (cap_block_n17_136 = 0; cap_block_n17_136 < 9; cap_block_n17_136 = cap_block_n17_136 + 1) {
      _ckks_gen_tmp_2760_191 = input_block_n16_131[cap_block_n17_136];
      _pgen_tmp_131_330 = *(PLAIN)Pt_from_msg(&_pgen_tmp_131_330, (cap_block_n17_136 + (num_grid_n17_134 * 9)) + 29884 /* cst_121_0 */, 2048, 1, 2, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_132_331, &_ckks_gen_tmp_2760_191, &_pgen_tmp_131_330);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n17_135, &result_grid_n17_135, &_pgen_tmp_132_331);
      _pgen_modulus_196 = Q_modulus();
      _pgen_num_q_197 = Poly_level(&result_grid_n17_135._c0_poly);
      for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_132_331._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_2760_191._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_131_330._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_132_331._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_2760_191._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_131_330._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modadd(Coeffs(&result_grid_n17_135._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_grid_n17_135._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_132_331._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modadd(Coeffs(&result_grid_n17_135._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_grid_n17_135._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_132_331._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        _pgen_modulus_196 = _pgen_modulus_196 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435678 = Rotate(result_grid_n17_135, num_grid_n17_134 * 4);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n17_133, &result_n17_133, &_preg_268435678);
    _pgen_modulus_196 = Q_modulus();
    _pgen_num_q_197 = Poly_level(&result_n17_133._c0_poly);
    for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
      Hw_modadd(Coeffs(&result_n17_133._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n17_133._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435678._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&result_n17_133._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n17_133._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435678._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      _pgen_modulus_196 = _pgen_modulus_196 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_data(&_preg_268435678._c1_poly);
    Free_data(&_preg_268435678._c0_poly);
  }
  Free_data(&_pgen_tmp_131_330._poly);
  Free_data(&_ckks_gen_tmp_2760_191._c1_poly);
  Free_data(&_ckks_gen_tmp_2760_191._c0_poly);
  Free_data(&_pgen_tmp_132_331._c1_poly);
  Free_data(&_pgen_tmp_132_331._c0_poly);
  Free_data(&result_grid_n17_135._c1_poly);
  Free_data(&result_grid_n17_135._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n17_133, &result_n17_133);
  Rescale(&result_n17_133._c0_poly, &result_n17_133._c0_poly);
  Rescale(&result_n17_133._c1_poly, &result_n17_133._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435679 = Rotate(result_n17_133, -2048);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_preg_268435577, &result_n17_133, &_preg_268435679);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_preg_268435577._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&_preg_268435577._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n17_133._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435679._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_preg_268435577._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&result_n17_133._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435679._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_preg_268435679._c1_poly);
  Free_data(&_preg_268435679._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(14, 0);
  Bootstrap(&_ckks_gen_tmp_2796_192, &_preg_268435577, 14);
  Free_data(&_preg_268435577._c1_poly);
  Free_data(&_preg_268435577._c0_poly);
  CMPLR_TM_END(14, 0);
  _pgen_tmp_134_333 = *(PLAIN)Pt_from_msg(&_pgen_tmp_134_333, 34492 /* cst_124 */, 2048, 1, 14, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_133_332, &_ckks_gen_tmp_2796_192, &_pgen_tmp_134_333);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_133_332._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_133_332._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_2796_192._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_134_333._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_133_332._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_2796_192._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_134_333._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_134_333._poly);
  Free_data(&_ckks_gen_tmp_2796_192._c1_poly);
  Free_data(&_ckks_gen_tmp_2796_192._c0_poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435553, &_pgen_tmp_133_332);
  Rescale(&_preg_268435553._c0_poly, &_pgen_tmp_133_332._c0_poly);
  Rescale(&_preg_268435553._c1_poly, &_pgen_tmp_133_332._c1_poly);
  Free_data(&_pgen_tmp_133_332._c1_poly);
  Free_data(&_pgen_tmp_133_332._c0_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_135_334 = *(PLAIN)Pt_from_msg(&_pgen_tmp_135_334, 34493 /* cst_122 */, 2048, 1, 13, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n17_133, &_preg_268435553, &_pgen_tmp_135_334);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&result_n17_133._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&result_n17_133._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435553._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_135_334._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Set_coeffs(&result_n17_133._c1_poly, _pgen_rns_idx_198, degree, Coeffs(&_preg_268435553._c1_poly, _pgen_rns_idx_198, degree));
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_135_334._poly);
  Free_data(&_preg_268435553._c1_poly);
  Free_data(&_preg_268435553._c0_poly);
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435554, &result_n17_133);
  Free_data(&result_n17_133._c1_poly);
  Free_data(&result_n17_133._c0_poly);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 67
  // /features/features.42/Relu
  // pragma: 65536, 1033, 68
  CMPLR_TM_START(10, 0);
  Copy_ciph(&_preg_268435555, &_preg_268435554);
  Free_data(&_preg_268435554._c1_poly);
  Free_data(&_preg_268435554._c0_poly);
  Encode_float(&_pgen_tmp_137_336, &_cst_190, 1, 1, Level(&_preg_268435555), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_136_335, &_preg_268435555, &_pgen_tmp_137_336);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_136_335._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_136_335._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435555._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_137_336._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_136_335._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435555._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_137_336._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_137_336._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435557, &_pgen_tmp_136_335);
  Rescale(&_preg_268435557._c0_poly, &_pgen_tmp_136_335._c0_poly);
  Rescale(&_preg_268435557._c1_poly, &_pgen_tmp_136_335._c1_poly);
  Free_data(&_pgen_tmp_136_335._c1_poly);
  Free_data(&_pgen_tmp_136_335._c0_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435556 = App_relu_2(_preg_268435555, _preg_268435557, 2048);
  Free_data(&_preg_268435557._c1_poly);
  Free_data(&_preg_268435557._c0_poly);
  Free_data(&_preg_268435555._c1_poly);
  Free_data(&_preg_268435555._c0_poly);
  Copy_ciph(&_preg_268435558, &_preg_268435556);
  Free_data(&_preg_268435556._c1_poly);
  Free_data(&_preg_268435556._c0_poly);
  CMPLR_TM_END(10, 0);
  // pragma: 65537, 1033, 68
  // /features/features.43/AveragePool
  // pragma: 65536, 1026, 69
  CMPLR_TM_START(3, 0);
  Copy_ciph(&_preg_268435559, &_preg_268435558);
  Free_data(&_preg_268435558._c1_poly);
  Free_data(&_preg_268435558._c0_poly);
  _preg_268435680 = Rotate(_preg_268435559, 2);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&tmp_row_add17_137, &_preg_268435559, &_preg_268435680);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&tmp_row_add17_137._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&tmp_row_add17_137._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435559._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435680._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&tmp_row_add17_137._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435559._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435680._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_preg_268435680._c1_poly);
  Free_data(&_preg_268435680._c0_poly);
  Free_data(&_preg_268435559._c1_poly);
  Free_data(&_preg_268435559._c0_poly);
  CMPLR_TM_END(12, 0);
  _preg_268435681 = Rotate(tmp_row_add17_137, 1);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_preg_268435578, &tmp_row_add17_137, &_preg_268435681);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_preg_268435578._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&_preg_268435578._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&tmp_row_add17_137._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435681._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_preg_268435578._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&tmp_row_add17_137._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435681._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_preg_268435681._c1_poly);
  Free_data(&_preg_268435681._c0_poly);
  Free_data(&tmp_row_add17_137._c1_poly);
  Free_data(&tmp_row_add17_137._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(14, 0);
  Bootstrap(&_ckks_gen_tmp_2865_193, &_preg_268435578, 6);
  Free_data(&_preg_268435578._c1_poly);
  Free_data(&_preg_268435578._c0_poly);
  CMPLR_TM_END(14, 0);
  _pgen_tmp_139_338 = *(PLAIN)Pt_from_msg(&_pgen_tmp_139_338, 34494 /* cst_125 */, 2048, 1, 6, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_138_337, &_ckks_gen_tmp_2865_193, &_pgen_tmp_139_338);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_138_337._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_138_337._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_2865_193._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_139_338._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_138_337._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_2865_193._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_139_338._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_139_338._poly);
  Free_data(&_ckks_gen_tmp_2865_193._c1_poly);
  Free_data(&_ckks_gen_tmp_2865_193._c0_poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435560, &_pgen_tmp_138_337);
  Rescale(&_preg_268435560._c0_poly, &_pgen_tmp_138_337._c0_poly);
  Rescale(&_preg_268435560._c1_poly, &_pgen_tmp_138_337._c1_poly);
  Free_data(&_pgen_tmp_138_337._c1_poly);
  Free_data(&_pgen_tmp_138_337._c0_poly);
  CMPLR_TM_END(19, 0);
  Copy_ciph(&orig_result17_138, &_preg_268435560);
  Free_data(&_preg_268435560._c1_poly);
  Free_data(&_preg_268435560._c0_poly);
  _pgen_tmp_141_340 = *(PLAIN)Pt_from_msg(&_pgen_tmp_141_340, 34495 /* cst_126 */, 2048, 1, 5, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_140_339, &orig_result17_138, &_pgen_tmp_141_340);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_140_339._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_140_339._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&orig_result17_138._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_141_340._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_140_339._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&orig_result17_138._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_141_340._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_141_340._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&orig_result17_138, &_pgen_tmp_140_339);
  Rescale(&orig_result17_138._c0_poly, &_pgen_tmp_140_339._c0_poly);
  Rescale(&orig_result17_138._c1_poly, &_pgen_tmp_140_339._c1_poly);
  Free_data(&_pgen_tmp_140_339._c1_poly);
  Free_data(&_pgen_tmp_140_339._c0_poly);
  CMPLR_TM_END(19, 0);
  Zero_ciph(&ss_result18_139);
  for (comb_cc_index_n18_140 = 0; comb_cc_index_n18_140 < 512; comb_cc_index_n18_140 = comb_cc_index_n18_140 + 1) {
    _preg_268435682 = Rotate(orig_result17_138, comb_cc_index_n18_140 * 3);
    Copy_ciph(&_ckks_gen_tmp_2908_194, &_preg_268435682);
    _pgen_tmp_142_341 = *(PLAIN)Pt_from_msg(&_pgen_tmp_142_341, comb_cc_index_n18_140 + 34496 /* cst_127_0 */, 2048, 1, 4, 0);
    CMPLR_TM_START(17, 0);
    Init_ciph_up_scale_plain(&_pgen_tmp_143_342, &_ckks_gen_tmp_2908_194, &_pgen_tmp_142_341);
    CMPLR_TM_END(17, 0);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&ss_result18_139, &ss_result18_139, &_pgen_tmp_143_342);
    _pgen_modulus_196 = Q_modulus();
    _pgen_num_q_197 = Poly_level(&ss_result18_139._c0_poly);
    for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_143_342._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_2908_194._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_142_341._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_143_342._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_2908_194._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_142_341._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&ss_result18_139._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&ss_result18_139._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_143_342._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&ss_result18_139._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&ss_result18_139._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_143_342._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      _pgen_modulus_196 = _pgen_modulus_196 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_data(&_preg_268435682._c1_poly);
    Free_data(&_preg_268435682._c0_poly);
  }
  Free_data(&orig_result17_138._c1_poly);
  Free_data(&orig_result17_138._c0_poly);
  Free_data(&_pgen_tmp_142_341._poly);
  Free_data(&_ckks_gen_tmp_2908_194._c1_poly);
  Free_data(&_ckks_gen_tmp_2908_194._c0_poly);
  Free_data(&_pgen_tmp_143_342._c1_poly);
  Free_data(&_pgen_tmp_143_342._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&ss_result18_139, &ss_result18_139);
  Rescale(&ss_result18_139._c0_poly, &ss_result18_139._c0_poly);
  Rescale(&ss_result18_139._c1_poly, &ss_result18_139._c1_poly);
  CMPLR_TM_END(19, 0);
  Copy_ciph(&_preg_268435561, &ss_result18_139);
  Free_data(&ss_result18_139._c1_poly);
  Free_data(&ss_result18_139._c0_poly);
  CMPLR_TM_END(3, 0);
  // pragma: 65537, 1026, 69
  // /Reshape
  // pragma: 65536, 1034, 70
  CMPLR_TM_START(6, 0);
  Copy_ciph(&_preg_268435562, &_preg_268435561);
  Free_data(&_preg_268435561._c1_poly);
  Free_data(&_preg_268435561._c0_poly);
  CMPLR_TM_END(6, 0);
  // pragma: 65537, 1034, 70
  // /classifier/Gemm
  // pragma: 65536, 1029, 71
  CMPLR_TM_START(7, 0);
  // BlockingRot replicate=3
  _preg_268435683 = Rotate(_preg_268435562, -512);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_144_343, &_preg_268435562, &_preg_268435683);
  CMPLR_TM_END(12, 0);
  _preg_268435684 = Rotate(_preg_268435562, -1024);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&input_dup_n18_141, &_pgen_tmp_144_343, &_preg_268435684);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&input_dup_n18_141._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_144_343._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435562._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435683._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_144_343._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435562._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435683._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&input_dup_n18_141._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_144_343._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435684._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modadd(Coeffs(&input_dup_n18_141._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_144_343._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435684._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_preg_268435683._c1_poly);
  Free_data(&_preg_268435683._c0_poly);
  Free_data(&_preg_268435562._c1_poly);
  Free_data(&_preg_268435562._c0_poly);
  Free_data(&_preg_268435684._c1_poly);
  Free_data(&_preg_268435684._c0_poly);
  Free_data(&_pgen_tmp_144_343._c1_poly);
  Free_data(&_pgen_tmp_144_343._c0_poly);
  CMPLR_TM_END(12, 0);
  // BlockingRot HRot=bs=2
  _preg_268435685 = Rotate(input_dup_n18_141, 0);
  Copy_ciph(&input_block_n18_142[0], &_preg_268435685);
  Free_data(&_preg_268435685._c1_poly);
  Free_data(&_preg_268435685._c0_poly);
  for (bs_input_n18_143 = 1; bs_input_n18_143 < 2; bs_input_n18_143 = bs_input_n18_143 + 1) {
    _preg_268435686 = Rotate(input_dup_n18_141, _cst_130[bs_input_n18_143]);
    Copy_ciph(&input_block_n18_142[bs_input_n18_143], &_preg_268435686);
    Free_data(&_preg_268435686._c1_poly);
    Free_data(&_preg_268435686._c0_poly);
  }
  Free_data(&input_dup_n18_141._c1_poly);
  Free_data(&input_dup_n18_141._c0_poly);
  Zero_ciph(&gemm_result_n19_144);
  // IMRA Metakernel: gs=4
  for (gs_n19_145 = 0; gs_n19_145 < 4; gs_n19_145 = gs_n19_145 + 1) {
    Zero_ciph(&block_result_146);
    for (bs_n19_147 = 0; bs_n19_147 < 2; bs_n19_147 = bs_n19_147 + 1) {
      _ckks_gen_tmp_3012_195 = input_block_n18_142[bs_n19_147];
      _pgen_tmp_145_344 = *(PLAIN)Pt_from_msg(&_pgen_tmp_145_344, (bs_n19_147 + (gs_n19_145 * 2)) + 35008 /* cst_129_0 */, 1040, 1, 3, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_146_345, &_ckks_gen_tmp_3012_195, &_pgen_tmp_145_344);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&block_result_146, &block_result_146, &_pgen_tmp_146_345);
      _pgen_modulus_196 = Q_modulus();
      _pgen_num_q_197 = Poly_level(&block_result_146._c0_poly);
      for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_146_345._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_3012_195._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_145_344._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_146_345._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_ckks_gen_tmp_3012_195._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_145_344._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modadd(Coeffs(&block_result_146._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&block_result_146._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_146_345._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        Hw_modadd(Coeffs(&block_result_146._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&block_result_146._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_146_345._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
        _pgen_modulus_196 = _pgen_modulus_196 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    CMPLR_TM_START(19, 0);
    Init_ciph_down_scale(&block_result_146, &block_result_146);
    Rescale(&block_result_146._c0_poly, &block_result_146._c0_poly);
    Rescale(&block_result_146._c1_poly, &block_result_146._c1_poly);
    CMPLR_TM_END(19, 0);
    _preg_268435687 = Rotate(block_result_146, gs_n19_145 * 2);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&gemm_result_n19_144, &gemm_result_n19_144, &_preg_268435687);
    _pgen_modulus_196 = Q_modulus();
    _pgen_num_q_197 = Poly_level(&gemm_result_n19_144._c0_poly);
    for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
      Hw_modadd(Coeffs(&gemm_result_n19_144._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&gemm_result_n19_144._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435687._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&gemm_result_n19_144._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&gemm_result_n19_144._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435687._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      _pgen_modulus_196 = _pgen_modulus_196 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_data(&_preg_268435687._c1_poly);
    Free_data(&_preg_268435687._c0_poly);
  }
  Free_data(&_pgen_tmp_145_344._poly);
  Free_data(&_ckks_gen_tmp_3012_195._c1_poly);
  Free_data(&_ckks_gen_tmp_3012_195._c0_poly);
  Free_data(&_pgen_tmp_146_345._c1_poly);
  Free_data(&_pgen_tmp_146_345._c0_poly);
  Free_data(&block_result_146._c1_poly);
  Free_data(&block_result_146._c0_poly);
  // gemm result reduce->Ps=2
  Copy_ciph(&reduce_intra_148, &gemm_result_n19_144);
  Free_data(&gemm_result_n19_144._c1_poly);
  Free_data(&gemm_result_n19_144._c0_poly);
  for (ri_n19_149 = 0; ri_n19_149 < 1; ri_n19_149 = ri_n19_149 + 1) {
    _preg_268435688 = Rotate(reduce_intra_148, (1 << ri_n19_149) * 520);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&reduce_intra_148, &reduce_intra_148, &_preg_268435688);
    _pgen_modulus_196 = Q_modulus();
    _pgen_num_q_197 = Poly_level(&reduce_intra_148._c0_poly);
    for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
      Hw_modadd(Coeffs(&reduce_intra_148._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&reduce_intra_148._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435688._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&reduce_intra_148._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&reduce_intra_148._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435688._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      _pgen_modulus_196 = _pgen_modulus_196 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_data(&_preg_268435688._c1_poly);
    Free_data(&_preg_268435688._c0_poly);
  }
  // gemm result reduce->(kp/np)=32
  Copy_ciph(&reduce_intra_150, &reduce_intra_148);
  Free_data(&reduce_intra_148._c1_poly);
  Free_data(&reduce_intra_148._c0_poly);
  for (ri_n19_151 = 0; ri_n19_151 < 5; ri_n19_151 = ri_n19_151 + 1) {
    _preg_268435689 = Rotate(reduce_intra_150, (1 << ri_n19_151) * 16);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&reduce_intra_150, &reduce_intra_150, &_preg_268435689);
    _pgen_modulus_196 = Q_modulus();
    _pgen_num_q_197 = Poly_level(&reduce_intra_150._c0_poly);
    for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
      Hw_modadd(Coeffs(&reduce_intra_150._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&reduce_intra_150._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435689._c0_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      Hw_modadd(Coeffs(&reduce_intra_150._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&reduce_intra_150._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_preg_268435689._c1_poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
      _pgen_modulus_196 = _pgen_modulus_196 + 1;
    }
    CMPLR_TM_END(12, 0);
    Free_data(&_preg_268435689._c1_poly);
    Free_data(&_preg_268435689._c0_poly);
  }
  // gemm add bias
  _pgen_tmp_147_346 = *(PLAIN)Pt_from_msg(&_pgen_tmp_147_346, 35016 /* cst_1 */, 10, 1, 2, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&reduce_intra_150, &reduce_intra_150, &_pgen_tmp_147_346);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&reduce_intra_150._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modadd(Coeffs(&reduce_intra_150._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&reduce_intra_150._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_147_346._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Set_coeffs(&reduce_intra_150._c1_poly, _pgen_rns_idx_198, degree, Coeffs(&reduce_intra_150._c1_poly, _pgen_rns_idx_198, degree));
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_147_346._poly);
  CMPLR_TM_END(13, 0);
  _pgen_tmp_149_348 = *(PLAIN)Pt_from_msg(&_pgen_tmp_149_348, 35017 /* cst_131 */, 10, 1, 2, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_148_347, &reduce_intra_150, &_pgen_tmp_149_348);
  _pgen_modulus_196 = Q_modulus();
  _pgen_num_q_197 = Poly_level(&_pgen_tmp_148_347._c0_poly);
  for (_pgen_rns_idx_198 = 0; _pgen_rns_idx_198 < _pgen_num_q_197; _pgen_rns_idx_198 = _pgen_rns_idx_198 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_148_347._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&reduce_intra_150._c0_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_149_348._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_148_347._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&reduce_intra_150._c1_poly, _pgen_rns_idx_198, degree), Coeffs(&_pgen_tmp_149_348._poly, _pgen_rns_idx_198, degree), _pgen_modulus_196, degree);
    _pgen_modulus_196 = _pgen_modulus_196 + 1;
  }
  Free_data(&_pgen_tmp_149_348._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&reduce_intra_150, &_pgen_tmp_148_347);
  Rescale(&reduce_intra_150._c0_poly, &_pgen_tmp_148_347._c0_poly);
  Rescale(&reduce_intra_150._c1_poly, &_pgen_tmp_148_347._c1_poly);
  Free_data(&_pgen_tmp_148_347._c1_poly);
  Free_data(&_pgen_tmp_148_347._c0_poly);
  CMPLR_TM_END(19, 0);
  Copy_ciph(&output_152, &reduce_intra_150);
  Free_data(&reduce_intra_150._c1_poly);
  Free_data(&reduce_intra_150._c0_poly);
  CMPLR_TM_END(7, 0);
  // pragma: 65537, 1029, 71
  Set_output_data("output", 0, &output_152);
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

CIPHERTEXT App_relu(CIPHERTEXT input_ct0_0, CIPHERTEXT input_ct1_1, uint32_t mask_len_2) {
  CIPHERTEXT _relu_tmp8_3;
  CIPHERTEXT _relu_tmp14_4;
  CIPHERTEXT _relu_tmp18_5;
  CIPHERTEXT _relu_tmp24_6;
  CIPHERTEXT _relu_tmp58_7;
  CIPHERTEXT _relu_tmp62_8;
  CIPHERTEXT _relu_tmp68_9;
  CIPHERTEXT _relu_tmp72_10;
  CIPHERTEXT _relu_tmp80_11;
  CIPHERTEXT _relu_tmp84_12;
  CIPHERTEXT _relu_tmp90_13;
  CIPHERTEXT _relu_tmp94_14;
  CIPHERTEXT _relu_tmp100_15;
  CIPHERTEXT _relu_tmp155_16;
  CIPHERTEXT _relu_tmp159_17;
  CIPHERTEXT _relu_tmp165_18;
  CIPHERTEXT _relu_tmp169_19;
  CIPHERTEXT _relu_tmp177_20;
  CIPHERTEXT _relu_tmp181_21;
  CIPHERTEXT _relu_tmp187_22;
  CIPHERTEXT _relu_tmp191_23;
  CIPHERTEXT _relu_tmp197_24;
  CIPHERTEXT _relu_tmp266_25;
  CIPHERTEXT _relu_tmp274_26;
  CIPHERTEXT3 _pgen_tmp_0_27;
  MODULUS* _pgen_modulus_28;
  uint32_t _pgen_num_q_29;
  uint32_t _pgen_rns_idx_30;
  POLYNOMIAL _pgen_tmp_1_31;
  POLYNOMIAL _pgen_tmp_2_32;
  CIPHERTEXT _pgen_tmp_3_33;
  PLAINTEXT _pgen_tmp_4_34;
  CIPHERTEXT3 _pgen_tmp_5_35;
  POLYNOMIAL _pgen_tmp_6_36;
  POLYNOMIAL _pgen_tmp_7_37;
  CIPHERTEXT _pgen_tmp_8_38;
  PLAINTEXT _pgen_tmp_9_39;
  CIPHERTEXT3 _pgen_tmp_10_40;
  CIPHERTEXT3 _pgen_tmp_11_41;
  PLAINTEXT _pgen_tmp_12_42;
  CIPHERTEXT _pgen_tmp_13_43;
  POLYNOMIAL _pgen_tmp_14_44;
  POLYNOMIAL _pgen_tmp_15_45;
  PLAINTEXT _pgen_tmp_16_46;
  CIPHERTEXT _pgen_tmp_17_47;
  CIPHERTEXT _pgen_tmp_18_48;
  POLYNOMIAL _pgen_tmp_19_49;
  POLYNOMIAL _pgen_tmp_20_50;
  CIPHERTEXT3 _pgen_tmp_21_51;
  PLAINTEXT _pgen_tmp_22_52;
  CIPHERTEXT _pgen_tmp_23_53;
  POLYNOMIAL _pgen_tmp_24_54;
  POLYNOMIAL _pgen_tmp_25_55;
  PLAINTEXT _pgen_tmp_26_56;
  CIPHERTEXT _pgen_tmp_27_57;
  CIPHERTEXT _pgen_tmp_28_58;
  CIPHERTEXT3 _pgen_tmp_29_59;
  POLYNOMIAL _pgen_tmp_30_60;
  POLYNOMIAL _pgen_tmp_31_61;
  CIPHERTEXT _pgen_tmp_32_62;
  PLAINTEXT _pgen_tmp_33_63;
  CIPHERTEXT3 _pgen_tmp_34_64;
  POLYNOMIAL _pgen_tmp_35_65;
  POLYNOMIAL _pgen_tmp_36_66;
  CIPHERTEXT _pgen_tmp_37_67;
  PLAINTEXT _pgen_tmp_38_68;
  CIPHERTEXT _pgen_tmp_39_69;
  CIPHERTEXT3 _pgen_tmp_40_70;
  POLYNOMIAL _pgen_tmp_41_71;
  POLYNOMIAL _pgen_tmp_42_72;
  CIPHERTEXT _pgen_tmp_43_73;
  PLAINTEXT _pgen_tmp_44_74;
  CIPHERTEXT3 _pgen_tmp_45_75;
  POLYNOMIAL _pgen_tmp_46_76;
  POLYNOMIAL _pgen_tmp_47_77;
  CIPHERTEXT _pgen_tmp_48_78;
  PLAINTEXT _pgen_tmp_49_79;
  CIPHERTEXT3 _pgen_tmp_50_80;
  CIPHERTEXT3 _pgen_tmp_51_81;
  CIPHERTEXT3 _pgen_tmp_52_82;
  PLAINTEXT _pgen_tmp_53_83;
  CIPHERTEXT _pgen_tmp_54_84;
  POLYNOMIAL _pgen_tmp_55_85;
  POLYNOMIAL _pgen_tmp_56_86;
  PLAINTEXT _pgen_tmp_57_87;
  CIPHERTEXT _pgen_tmp_58_88;
  CIPHERTEXT _pgen_tmp_59_89;
  POLYNOMIAL _pgen_tmp_60_90;
  POLYNOMIAL _pgen_tmp_61_91;
  PLAINTEXT _pgen_tmp_62_92;
  CIPHERTEXT _pgen_tmp_63_93;
  PLAINTEXT _pgen_tmp_64_94;
  CIPHERTEXT _pgen_tmp_65_95;
  CIPHERTEXT _pgen_tmp_66_96;
  CIPHERTEXT _pgen_tmp_67_97;
  POLYNOMIAL _pgen_tmp_68_98;
  POLYNOMIAL _pgen_tmp_69_99;
  CIPHERTEXT3 _pgen_tmp_70_100;
  PLAINTEXT _pgen_tmp_71_101;
  CIPHERTEXT _pgen_tmp_72_102;
  PLAINTEXT _pgen_tmp_73_103;
  CIPHERTEXT _pgen_tmp_74_104;
  CIPHERTEXT _pgen_tmp_75_105;
  POLYNOMIAL _pgen_tmp_76_106;
  POLYNOMIAL _pgen_tmp_77_107;
  PLAINTEXT _pgen_tmp_78_108;
  CIPHERTEXT _pgen_tmp_79_109;
  PLAINTEXT _pgen_tmp_80_110;
  CIPHERTEXT _pgen_tmp_81_111;
  CIPHERTEXT _pgen_tmp_82_112;
  CIPHERTEXT _pgen_tmp_83_113;
  CIPHERTEXT3 _pgen_tmp_84_114;
  POLYNOMIAL _pgen_tmp_85_115;
  POLYNOMIAL _pgen_tmp_86_116;
  CIPHERTEXT _pgen_tmp_87_117;
  PLAINTEXT _pgen_tmp_88_118;
  CIPHERTEXT3 _pgen_tmp_89_119;
  POLYNOMIAL _pgen_tmp_90_120;
  POLYNOMIAL _pgen_tmp_91_121;
  CIPHERTEXT _pgen_tmp_92_122;
  PLAINTEXT _pgen_tmp_93_123;
  CIPHERTEXT _pgen_tmp_94_124;
  CIPHERTEXT3 _pgen_tmp_95_125;
  POLYNOMIAL _pgen_tmp_96_126;
  POLYNOMIAL _pgen_tmp_97_127;
  CIPHERTEXT _pgen_tmp_98_128;
  PLAINTEXT _pgen_tmp_99_129;
  CIPHERTEXT3 _pgen_tmp_100_130;
  POLYNOMIAL _pgen_tmp_101_131;
  POLYNOMIAL _pgen_tmp_102_132;
  CIPHERTEXT _pgen_tmp_103_133;
  PLAINTEXT _pgen_tmp_104_134;
  CIPHERTEXT3 _pgen_tmp_105_135;
  CIPHERTEXT3 _pgen_tmp_106_136;
  CIPHERTEXT3 _pgen_tmp_107_137;
  PLAINTEXT _pgen_tmp_108_138;
  CIPHERTEXT _pgen_tmp_109_139;
  POLYNOMIAL _pgen_tmp_110_140;
  POLYNOMIAL _pgen_tmp_111_141;
  POLYNOMIAL _pgen_tmp_112_142;
  POLYNOMIAL _pgen_tmp_113_143;
  CIPHERTEXT3 _pgen_tmp_114_144;
  PLAINTEXT _pgen_tmp_115_145;
  CIPHERTEXT _pgen_tmp_116_146;
  POLYNOMIAL _pgen_tmp_117_147;
  POLYNOMIAL _pgen_tmp_118_148;
  CIPHERTEXT3 _pgen_tmp_119_149;
  PLAINTEXT _pgen_tmp_120_150;
  CIPHERTEXT _pgen_tmp_121_151;
  POLYNOMIAL _pgen_tmp_122_152;
  POLYNOMIAL _pgen_tmp_123_153;
  CIPHERTEXT _pgen_tmp_124_154;
  CIPHERTEXT _pgen_tmp_125_155;
  POLYNOMIAL _pgen_tmp_126_156;
  POLYNOMIAL _pgen_tmp_127_157;
  CIPHERTEXT3 _pgen_tmp_128_158;
  CIPHERTEXT3 _pgen_tmp_129_159;
  PLAINTEXT _pgen_tmp_130_160;
  CIPHERTEXT _pgen_tmp_131_161;
  POLYNOMIAL _pgen_tmp_132_162;
  POLYNOMIAL _pgen_tmp_133_163;
  CIPHERTEXT3 _pgen_tmp_134_164;
  PLAINTEXT _pgen_tmp_135_165;
  CIPHERTEXT _pgen_tmp_136_166;
  POLYNOMIAL _pgen_tmp_137_167;
  POLYNOMIAL _pgen_tmp_138_168;
  CIPHERTEXT _pgen_tmp_139_169;
  POLYNOMIAL _pgen_tmp_140_170;
  POLYNOMIAL _pgen_tmp_141_171;
  CIPHERTEXT3 _pgen_tmp_142_172;
  PLAINTEXT _pgen_tmp_143_173;
  CIPHERTEXT _pgen_tmp_144_174;
  POLYNOMIAL _pgen_tmp_145_175;
  POLYNOMIAL _pgen_tmp_146_176;
  CIPHERTEXT3 _pgen_tmp_147_177;
  PLAINTEXT _pgen_tmp_148_178;
  CIPHERTEXT _pgen_tmp_149_179;
  POLYNOMIAL _pgen_tmp_150_180;
  POLYNOMIAL _pgen_tmp_151_181;
  CIPHERTEXT _pgen_tmp_152_182;
  CIPHERTEXT _pgen_tmp_153_183;
  PLAINTEXT _pgen_tmp_154_184;
  CIPHERTEXT _pgen_tmp_155_185;
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
  CIPHERTEXT _preg_268435467;
  CIPHERTEXT _preg_268435468;
  CIPHERTEXT _preg_268435469;
  CIPHERTEXT _preg_268435470;
  CIPHERTEXT _preg_268435471;
  CIPHERTEXT _preg_268435472;
  CIPHERTEXT _preg_268435473;
  CIPHERTEXT _preg_268435474;
  CIPHERTEXT _preg_268435475;
  CIPHERTEXT _preg_268435476;
  CIPHERTEXT _preg_268435477;
  CIPHERTEXT _preg_268435478;
  CIPHERTEXT _preg_268435479;
  CIPHERTEXT _preg_268435480;
  CIPHERTEXT _preg_268435481;
  CIPHERTEXT _preg_268435482;
  CIPHERTEXT _preg_268435483;
  uint32_t  degree = Degree();
  memset(&_relu_tmp8_3, 0, sizeof(_relu_tmp8_3));
  memset(&_relu_tmp14_4, 0, sizeof(_relu_tmp14_4));
  memset(&_relu_tmp18_5, 0, sizeof(_relu_tmp18_5));
  memset(&_relu_tmp24_6, 0, sizeof(_relu_tmp24_6));
  memset(&_relu_tmp58_7, 0, sizeof(_relu_tmp58_7));
  memset(&_relu_tmp62_8, 0, sizeof(_relu_tmp62_8));
  memset(&_relu_tmp68_9, 0, sizeof(_relu_tmp68_9));
  memset(&_relu_tmp72_10, 0, sizeof(_relu_tmp72_10));
  memset(&_relu_tmp80_11, 0, sizeof(_relu_tmp80_11));
  memset(&_relu_tmp84_12, 0, sizeof(_relu_tmp84_12));
  memset(&_relu_tmp90_13, 0, sizeof(_relu_tmp90_13));
  memset(&_relu_tmp94_14, 0, sizeof(_relu_tmp94_14));
  memset(&_relu_tmp100_15, 0, sizeof(_relu_tmp100_15));
  memset(&_relu_tmp155_16, 0, sizeof(_relu_tmp155_16));
  memset(&_relu_tmp159_17, 0, sizeof(_relu_tmp159_17));
  memset(&_relu_tmp165_18, 0, sizeof(_relu_tmp165_18));
  memset(&_relu_tmp169_19, 0, sizeof(_relu_tmp169_19));
  memset(&_relu_tmp177_20, 0, sizeof(_relu_tmp177_20));
  memset(&_relu_tmp181_21, 0, sizeof(_relu_tmp181_21));
  memset(&_relu_tmp187_22, 0, sizeof(_relu_tmp187_22));
  memset(&_relu_tmp191_23, 0, sizeof(_relu_tmp191_23));
  memset(&_relu_tmp197_24, 0, sizeof(_relu_tmp197_24));
  memset(&_relu_tmp266_25, 0, sizeof(_relu_tmp266_25));
  memset(&_relu_tmp274_26, 0, sizeof(_relu_tmp274_26));
  memset(&_pgen_tmp_0_27, 0, sizeof(_pgen_tmp_0_27));
  memset(&_pgen_tmp_1_31, 0, sizeof(_pgen_tmp_1_31));
  memset(&_pgen_tmp_2_32, 0, sizeof(_pgen_tmp_2_32));
  memset(&_pgen_tmp_3_33, 0, sizeof(_pgen_tmp_3_33));
  memset(&_pgen_tmp_4_34, 0, sizeof(_pgen_tmp_4_34));
  memset(&_pgen_tmp_5_35, 0, sizeof(_pgen_tmp_5_35));
  memset(&_pgen_tmp_6_36, 0, sizeof(_pgen_tmp_6_36));
  memset(&_pgen_tmp_7_37, 0, sizeof(_pgen_tmp_7_37));
  memset(&_pgen_tmp_8_38, 0, sizeof(_pgen_tmp_8_38));
  memset(&_pgen_tmp_9_39, 0, sizeof(_pgen_tmp_9_39));
  memset(&_pgen_tmp_10_40, 0, sizeof(_pgen_tmp_10_40));
  memset(&_pgen_tmp_11_41, 0, sizeof(_pgen_tmp_11_41));
  memset(&_pgen_tmp_12_42, 0, sizeof(_pgen_tmp_12_42));
  memset(&_pgen_tmp_13_43, 0, sizeof(_pgen_tmp_13_43));
  memset(&_pgen_tmp_14_44, 0, sizeof(_pgen_tmp_14_44));
  memset(&_pgen_tmp_15_45, 0, sizeof(_pgen_tmp_15_45));
  memset(&_pgen_tmp_16_46, 0, sizeof(_pgen_tmp_16_46));
  memset(&_pgen_tmp_17_47, 0, sizeof(_pgen_tmp_17_47));
  memset(&_pgen_tmp_18_48, 0, sizeof(_pgen_tmp_18_48));
  memset(&_pgen_tmp_19_49, 0, sizeof(_pgen_tmp_19_49));
  memset(&_pgen_tmp_20_50, 0, sizeof(_pgen_tmp_20_50));
  memset(&_pgen_tmp_21_51, 0, sizeof(_pgen_tmp_21_51));
  memset(&_pgen_tmp_22_52, 0, sizeof(_pgen_tmp_22_52));
  memset(&_pgen_tmp_23_53, 0, sizeof(_pgen_tmp_23_53));
  memset(&_pgen_tmp_24_54, 0, sizeof(_pgen_tmp_24_54));
  memset(&_pgen_tmp_25_55, 0, sizeof(_pgen_tmp_25_55));
  memset(&_pgen_tmp_26_56, 0, sizeof(_pgen_tmp_26_56));
  memset(&_pgen_tmp_27_57, 0, sizeof(_pgen_tmp_27_57));
  memset(&_pgen_tmp_28_58, 0, sizeof(_pgen_tmp_28_58));
  memset(&_pgen_tmp_29_59, 0, sizeof(_pgen_tmp_29_59));
  memset(&_pgen_tmp_30_60, 0, sizeof(_pgen_tmp_30_60));
  memset(&_pgen_tmp_31_61, 0, sizeof(_pgen_tmp_31_61));
  memset(&_pgen_tmp_32_62, 0, sizeof(_pgen_tmp_32_62));
  memset(&_pgen_tmp_33_63, 0, sizeof(_pgen_tmp_33_63));
  memset(&_pgen_tmp_34_64, 0, sizeof(_pgen_tmp_34_64));
  memset(&_pgen_tmp_35_65, 0, sizeof(_pgen_tmp_35_65));
  memset(&_pgen_tmp_36_66, 0, sizeof(_pgen_tmp_36_66));
  memset(&_pgen_tmp_37_67, 0, sizeof(_pgen_tmp_37_67));
  memset(&_pgen_tmp_38_68, 0, sizeof(_pgen_tmp_38_68));
  memset(&_pgen_tmp_39_69, 0, sizeof(_pgen_tmp_39_69));
  memset(&_pgen_tmp_40_70, 0, sizeof(_pgen_tmp_40_70));
  memset(&_pgen_tmp_41_71, 0, sizeof(_pgen_tmp_41_71));
  memset(&_pgen_tmp_42_72, 0, sizeof(_pgen_tmp_42_72));
  memset(&_pgen_tmp_43_73, 0, sizeof(_pgen_tmp_43_73));
  memset(&_pgen_tmp_44_74, 0, sizeof(_pgen_tmp_44_74));
  memset(&_pgen_tmp_45_75, 0, sizeof(_pgen_tmp_45_75));
  memset(&_pgen_tmp_46_76, 0, sizeof(_pgen_tmp_46_76));
  memset(&_pgen_tmp_47_77, 0, sizeof(_pgen_tmp_47_77));
  memset(&_pgen_tmp_48_78, 0, sizeof(_pgen_tmp_48_78));
  memset(&_pgen_tmp_49_79, 0, sizeof(_pgen_tmp_49_79));
  memset(&_pgen_tmp_50_80, 0, sizeof(_pgen_tmp_50_80));
  memset(&_pgen_tmp_51_81, 0, sizeof(_pgen_tmp_51_81));
  memset(&_pgen_tmp_52_82, 0, sizeof(_pgen_tmp_52_82));
  memset(&_pgen_tmp_53_83, 0, sizeof(_pgen_tmp_53_83));
  memset(&_pgen_tmp_54_84, 0, sizeof(_pgen_tmp_54_84));
  memset(&_pgen_tmp_55_85, 0, sizeof(_pgen_tmp_55_85));
  memset(&_pgen_tmp_56_86, 0, sizeof(_pgen_tmp_56_86));
  memset(&_pgen_tmp_57_87, 0, sizeof(_pgen_tmp_57_87));
  memset(&_pgen_tmp_58_88, 0, sizeof(_pgen_tmp_58_88));
  memset(&_pgen_tmp_59_89, 0, sizeof(_pgen_tmp_59_89));
  memset(&_pgen_tmp_60_90, 0, sizeof(_pgen_tmp_60_90));
  memset(&_pgen_tmp_61_91, 0, sizeof(_pgen_tmp_61_91));
  memset(&_pgen_tmp_62_92, 0, sizeof(_pgen_tmp_62_92));
  memset(&_pgen_tmp_63_93, 0, sizeof(_pgen_tmp_63_93));
  memset(&_pgen_tmp_64_94, 0, sizeof(_pgen_tmp_64_94));
  memset(&_pgen_tmp_65_95, 0, sizeof(_pgen_tmp_65_95));
  memset(&_pgen_tmp_66_96, 0, sizeof(_pgen_tmp_66_96));
  memset(&_pgen_tmp_67_97, 0, sizeof(_pgen_tmp_67_97));
  memset(&_pgen_tmp_68_98, 0, sizeof(_pgen_tmp_68_98));
  memset(&_pgen_tmp_69_99, 0, sizeof(_pgen_tmp_69_99));
  memset(&_pgen_tmp_70_100, 0, sizeof(_pgen_tmp_70_100));
  memset(&_pgen_tmp_71_101, 0, sizeof(_pgen_tmp_71_101));
  memset(&_pgen_tmp_72_102, 0, sizeof(_pgen_tmp_72_102));
  memset(&_pgen_tmp_73_103, 0, sizeof(_pgen_tmp_73_103));
  memset(&_pgen_tmp_74_104, 0, sizeof(_pgen_tmp_74_104));
  memset(&_pgen_tmp_75_105, 0, sizeof(_pgen_tmp_75_105));
  memset(&_pgen_tmp_76_106, 0, sizeof(_pgen_tmp_76_106));
  memset(&_pgen_tmp_77_107, 0, sizeof(_pgen_tmp_77_107));
  memset(&_pgen_tmp_78_108, 0, sizeof(_pgen_tmp_78_108));
  memset(&_pgen_tmp_79_109, 0, sizeof(_pgen_tmp_79_109));
  memset(&_pgen_tmp_80_110, 0, sizeof(_pgen_tmp_80_110));
  memset(&_pgen_tmp_81_111, 0, sizeof(_pgen_tmp_81_111));
  memset(&_pgen_tmp_82_112, 0, sizeof(_pgen_tmp_82_112));
  memset(&_pgen_tmp_83_113, 0, sizeof(_pgen_tmp_83_113));
  memset(&_pgen_tmp_84_114, 0, sizeof(_pgen_tmp_84_114));
  memset(&_pgen_tmp_85_115, 0, sizeof(_pgen_tmp_85_115));
  memset(&_pgen_tmp_86_116, 0, sizeof(_pgen_tmp_86_116));
  memset(&_pgen_tmp_87_117, 0, sizeof(_pgen_tmp_87_117));
  memset(&_pgen_tmp_88_118, 0, sizeof(_pgen_tmp_88_118));
  memset(&_pgen_tmp_89_119, 0, sizeof(_pgen_tmp_89_119));
  memset(&_pgen_tmp_90_120, 0, sizeof(_pgen_tmp_90_120));
  memset(&_pgen_tmp_91_121, 0, sizeof(_pgen_tmp_91_121));
  memset(&_pgen_tmp_92_122, 0, sizeof(_pgen_tmp_92_122));
  memset(&_pgen_tmp_93_123, 0, sizeof(_pgen_tmp_93_123));
  memset(&_pgen_tmp_94_124, 0, sizeof(_pgen_tmp_94_124));
  memset(&_pgen_tmp_95_125, 0, sizeof(_pgen_tmp_95_125));
  memset(&_pgen_tmp_96_126, 0, sizeof(_pgen_tmp_96_126));
  memset(&_pgen_tmp_97_127, 0, sizeof(_pgen_tmp_97_127));
  memset(&_pgen_tmp_98_128, 0, sizeof(_pgen_tmp_98_128));
  memset(&_pgen_tmp_99_129, 0, sizeof(_pgen_tmp_99_129));
  memset(&_pgen_tmp_100_130, 0, sizeof(_pgen_tmp_100_130));
  memset(&_pgen_tmp_101_131, 0, sizeof(_pgen_tmp_101_131));
  memset(&_pgen_tmp_102_132, 0, sizeof(_pgen_tmp_102_132));
  memset(&_pgen_tmp_103_133, 0, sizeof(_pgen_tmp_103_133));
  memset(&_pgen_tmp_104_134, 0, sizeof(_pgen_tmp_104_134));
  memset(&_pgen_tmp_105_135, 0, sizeof(_pgen_tmp_105_135));
  memset(&_pgen_tmp_106_136, 0, sizeof(_pgen_tmp_106_136));
  memset(&_pgen_tmp_107_137, 0, sizeof(_pgen_tmp_107_137));
  memset(&_pgen_tmp_108_138, 0, sizeof(_pgen_tmp_108_138));
  memset(&_pgen_tmp_109_139, 0, sizeof(_pgen_tmp_109_139));
  memset(&_pgen_tmp_110_140, 0, sizeof(_pgen_tmp_110_140));
  memset(&_pgen_tmp_111_141, 0, sizeof(_pgen_tmp_111_141));
  memset(&_pgen_tmp_112_142, 0, sizeof(_pgen_tmp_112_142));
  memset(&_pgen_tmp_113_143, 0, sizeof(_pgen_tmp_113_143));
  memset(&_pgen_tmp_114_144, 0, sizeof(_pgen_tmp_114_144));
  memset(&_pgen_tmp_115_145, 0, sizeof(_pgen_tmp_115_145));
  memset(&_pgen_tmp_116_146, 0, sizeof(_pgen_tmp_116_146));
  memset(&_pgen_tmp_117_147, 0, sizeof(_pgen_tmp_117_147));
  memset(&_pgen_tmp_118_148, 0, sizeof(_pgen_tmp_118_148));
  memset(&_pgen_tmp_119_149, 0, sizeof(_pgen_tmp_119_149));
  memset(&_pgen_tmp_120_150, 0, sizeof(_pgen_tmp_120_150));
  memset(&_pgen_tmp_121_151, 0, sizeof(_pgen_tmp_121_151));
  memset(&_pgen_tmp_122_152, 0, sizeof(_pgen_tmp_122_152));
  memset(&_pgen_tmp_123_153, 0, sizeof(_pgen_tmp_123_153));
  memset(&_pgen_tmp_124_154, 0, sizeof(_pgen_tmp_124_154));
  memset(&_pgen_tmp_125_155, 0, sizeof(_pgen_tmp_125_155));
  memset(&_pgen_tmp_126_156, 0, sizeof(_pgen_tmp_126_156));
  memset(&_pgen_tmp_127_157, 0, sizeof(_pgen_tmp_127_157));
  memset(&_pgen_tmp_128_158, 0, sizeof(_pgen_tmp_128_158));
  memset(&_pgen_tmp_129_159, 0, sizeof(_pgen_tmp_129_159));
  memset(&_pgen_tmp_130_160, 0, sizeof(_pgen_tmp_130_160));
  memset(&_pgen_tmp_131_161, 0, sizeof(_pgen_tmp_131_161));
  memset(&_pgen_tmp_132_162, 0, sizeof(_pgen_tmp_132_162));
  memset(&_pgen_tmp_133_163, 0, sizeof(_pgen_tmp_133_163));
  memset(&_pgen_tmp_134_164, 0, sizeof(_pgen_tmp_134_164));
  memset(&_pgen_tmp_135_165, 0, sizeof(_pgen_tmp_135_165));
  memset(&_pgen_tmp_136_166, 0, sizeof(_pgen_tmp_136_166));
  memset(&_pgen_tmp_137_167, 0, sizeof(_pgen_tmp_137_167));
  memset(&_pgen_tmp_138_168, 0, sizeof(_pgen_tmp_138_168));
  memset(&_pgen_tmp_139_169, 0, sizeof(_pgen_tmp_139_169));
  memset(&_pgen_tmp_140_170, 0, sizeof(_pgen_tmp_140_170));
  memset(&_pgen_tmp_141_171, 0, sizeof(_pgen_tmp_141_171));
  memset(&_pgen_tmp_142_172, 0, sizeof(_pgen_tmp_142_172));
  memset(&_pgen_tmp_143_173, 0, sizeof(_pgen_tmp_143_173));
  memset(&_pgen_tmp_144_174, 0, sizeof(_pgen_tmp_144_174));
  memset(&_pgen_tmp_145_175, 0, sizeof(_pgen_tmp_145_175));
  memset(&_pgen_tmp_146_176, 0, sizeof(_pgen_tmp_146_176));
  memset(&_pgen_tmp_147_177, 0, sizeof(_pgen_tmp_147_177));
  memset(&_pgen_tmp_148_178, 0, sizeof(_pgen_tmp_148_178));
  memset(&_pgen_tmp_149_179, 0, sizeof(_pgen_tmp_149_179));
  memset(&_pgen_tmp_150_180, 0, sizeof(_pgen_tmp_150_180));
  memset(&_pgen_tmp_151_181, 0, sizeof(_pgen_tmp_151_181));
  memset(&_pgen_tmp_152_182, 0, sizeof(_pgen_tmp_152_182));
  memset(&_pgen_tmp_153_183, 0, sizeof(_pgen_tmp_153_183));
  memset(&_pgen_tmp_154_184, 0, sizeof(_pgen_tmp_154_184));
  memset(&_pgen_tmp_155_185, 0, sizeof(_pgen_tmp_155_185));
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
  memset(&_preg_268435467, 0, sizeof(_preg_268435467));
  memset(&_preg_268435468, 0, sizeof(_preg_268435468));
  memset(&_preg_268435469, 0, sizeof(_preg_268435469));
  memset(&_preg_268435470, 0, sizeof(_preg_268435470));
  memset(&_preg_268435471, 0, sizeof(_preg_268435471));
  memset(&_preg_268435472, 0, sizeof(_preg_268435472));
  memset(&_preg_268435473, 0, sizeof(_preg_268435473));
  memset(&_preg_268435474, 0, sizeof(_preg_268435474));
  memset(&_preg_268435475, 0, sizeof(_preg_268435475));
  memset(&_preg_268435476, 0, sizeof(_preg_268435476));
  memset(&_preg_268435477, 0, sizeof(_preg_268435477));
  memset(&_preg_268435478, 0, sizeof(_preg_268435478));
  memset(&_preg_268435479, 0, sizeof(_preg_268435479));
  memset(&_preg_268435480, 0, sizeof(_preg_268435480));
  memset(&_preg_268435481, 0, sizeof(_preg_268435481));
  memset(&_preg_268435482, 0, sizeof(_preg_268435482));
  memset(&_preg_268435483, 0, sizeof(_preg_268435483));
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_0_27, &input_ct1_1, &input_ct1_1);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_0_27._c0_poly);
  Init_poly_by_size(&_pgen_tmp_1_31, 1, 0);
  Init_poly_by_size(&_pgen_tmp_2_32, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_1_31, 0, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_2_32, 0, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_0_27._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_0_27._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_1_31, 0, degree), Coeffs(&_pgen_tmp_2_32, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_0_27._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_1_31);
  Free_data(&_pgen_tmp_1_31);
  Free_data(&_pgen_tmp_2_32);
  Free_data(&_pgen_tmp_2_32);
  CMPLR_TM_END(16, 0);
  _preg_268435457 = Relinearize(_pgen_tmp_0_27);
  Free_data(&_pgen_tmp_0_27._c2_poly);
  Free_data(&_pgen_tmp_0_27._c1_poly);
  Free_data(&_pgen_tmp_0_27._c0_poly);
  Copy_ciph(&_relu_tmp8_3, &_preg_268435457);
  Free_data(&_preg_268435457._c1_poly);
  Free_data(&_preg_268435457._c0_poly);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_3_33, &_relu_tmp8_3, &_relu_tmp8_3);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_4_34, &_cst_132, 1, Sc_degree(&_pgen_tmp_3_33), Level(&_pgen_tmp_3_33), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp14_4, &_pgen_tmp_3_33, &_pgen_tmp_4_34);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_relu_tmp14_4._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_3_33._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp8_3._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp8_3._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_3_33._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp8_3._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp8_3._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_3_33._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_4_34._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Set_coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_30, degree, Coeffs(&_pgen_tmp_3_33._c1_poly, _pgen_rns_idx_30, degree));
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_relu_tmp8_3._c1_poly);
  Free_data(&_relu_tmp8_3._c0_poly);
  Free_data(&_pgen_tmp_4_34._poly);
  Free_data(&_pgen_tmp_3_33._c1_poly);
  Free_data(&_pgen_tmp_3_33._c0_poly);
  CMPLR_TM_END(13, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_5_35, &_relu_tmp14_4, &_relu_tmp14_4);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_5_35._c0_poly);
  Init_poly_by_size(&_pgen_tmp_6_36, 1, 0);
  Init_poly_by_size(&_pgen_tmp_7_37, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_6_36, 0, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_7_37, 0, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_5_35._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_5_35._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_6_36, 0, degree), Coeffs(&_pgen_tmp_7_37, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_5_35._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_6_36);
  Free_data(&_pgen_tmp_6_36);
  Free_data(&_pgen_tmp_7_37);
  Free_data(&_pgen_tmp_7_37);
  CMPLR_TM_END(16, 0);
  _preg_268435458 = Relinearize(_pgen_tmp_5_35);
  Free_data(&_pgen_tmp_5_35._c2_poly);
  Free_data(&_pgen_tmp_5_35._c1_poly);
  Free_data(&_pgen_tmp_5_35._c0_poly);
  Copy_ciph(&_relu_tmp18_5, &_preg_268435458);
  Free_data(&_preg_268435458._c1_poly);
  Free_data(&_preg_268435458._c0_poly);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_8_38, &_relu_tmp18_5, &_relu_tmp18_5);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_9_39, &_cst_133, 1, Sc_degree(&_pgen_tmp_8_38), Level(&_pgen_tmp_8_38), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp24_6, &_pgen_tmp_8_38, &_pgen_tmp_9_39);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_relu_tmp24_6._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_8_38._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp18_5._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp18_5._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_8_38._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp18_5._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp18_5._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp24_6._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_8_38._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_9_39._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Set_coeffs(&_relu_tmp24_6._c1_poly, _pgen_rns_idx_30, degree, Coeffs(&_pgen_tmp_8_38._c1_poly, _pgen_rns_idx_30, degree));
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_relu_tmp18_5._c1_poly);
  Free_data(&_relu_tmp18_5._c0_poly);
  Free_data(&_pgen_tmp_9_39._poly);
  Free_data(&_pgen_tmp_8_38._c1_poly);
  Free_data(&_pgen_tmp_8_38._c0_poly);
  CMPLR_TM_END(13, 0);
  Encode_float_mask(&_pgen_tmp_12_42, -3.809199432496630510769, mask_len_2, 1, Level(&input_ct1_1), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_13_43, &input_ct1_1, &_pgen_tmp_12_42);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_11_41, &_relu_tmp14_4, &_pgen_tmp_13_43);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_11_41._c0_poly);
  Init_poly_by_size(&_pgen_tmp_14_44, 1, 0);
  Init_poly_by_size(&_pgen_tmp_15_45, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_13_43._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_12_42._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_13_43._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_12_42._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_14_44, 0, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_13_43._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_15_45, 0, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_13_43._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_11_41._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_13_43._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_11_41._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_14_44, 0, degree), Coeffs(&_pgen_tmp_15_45, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_11_41._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_13_43._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_12_42._poly);
  Free_data(&_pgen_tmp_13_43._c1_poly);
  Free_data(&_pgen_tmp_13_43._c0_poly);
  Free_data(&_pgen_tmp_14_44);
  Free_data(&_pgen_tmp_14_44);
  Free_data(&_pgen_tmp_15_45);
  Free_data(&_pgen_tmp_15_45);
  CMPLR_TM_END(16, 0);
  _preg_268435459 = Relinearize(_pgen_tmp_11_41);
  Free_data(&_pgen_tmp_11_41._c2_poly);
  Free_data(&_pgen_tmp_11_41._c1_poly);
  Free_data(&_pgen_tmp_11_41._c0_poly);
  Encode_float_mask(&_pgen_tmp_16_46, 2.460940868770910228136, mask_len_2, 1, Level(&input_ct1_1), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_17_47, &input_ct1_1, &_pgen_tmp_16_46);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_18_48, &_preg_268435459, &_pgen_tmp_17_47);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_10_40, &_relu_tmp24_6, &_pgen_tmp_18_48);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_10_40._c0_poly);
  Init_poly_by_size(&_pgen_tmp_19_49, 1, 0);
  Init_poly_by_size(&_pgen_tmp_20_50, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_17_47._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_16_46._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_17_47._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_16_46._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_18_48._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435459._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_17_47._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_18_48._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435459._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_17_47._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_19_49, 0, degree), Coeffs(&_relu_tmp24_6._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_18_48._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_20_50, 0, degree), Coeffs(&_relu_tmp24_6._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_18_48._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_10_40._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp24_6._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_18_48._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_10_40._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_19_49, 0, degree), Coeffs(&_pgen_tmp_20_50, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_10_40._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp24_6._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_18_48._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_16_46._poly);
  Free_data(&_pgen_tmp_17_47._c1_poly);
  Free_data(&_pgen_tmp_17_47._c0_poly);
  Free_data(&_preg_268435459._c1_poly);
  Free_data(&_preg_268435459._c0_poly);
  Free_data(&_pgen_tmp_18_48._c1_poly);
  Free_data(&_pgen_tmp_18_48._c0_poly);
  Free_data(&_relu_tmp24_6._c1_poly);
  Free_data(&_relu_tmp24_6._c0_poly);
  Free_data(&_pgen_tmp_19_49);
  Free_data(&_pgen_tmp_19_49);
  Free_data(&_pgen_tmp_20_50);
  Free_data(&_pgen_tmp_20_50);
  CMPLR_TM_END(16, 0);
  _preg_268435460 = Relinearize(_pgen_tmp_10_40);
  Free_data(&_pgen_tmp_10_40._c2_poly);
  Free_data(&_pgen_tmp_10_40._c1_poly);
  Free_data(&_pgen_tmp_10_40._c0_poly);
  Encode_float_mask(&_pgen_tmp_22_52, -1.430304794543664215212, mask_len_2, 1, Level(&input_ct1_1), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_23_53, &input_ct1_1, &_pgen_tmp_22_52);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_21_51, &_relu_tmp14_4, &_pgen_tmp_23_53);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_21_51._c0_poly);
  Init_poly_by_size(&_pgen_tmp_24_54, 1, 0);
  Init_poly_by_size(&_pgen_tmp_25_55, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_23_53._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_22_52._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_23_53._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_22_52._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_24_54, 0, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_23_53._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_25_55, 0, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_23_53._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_21_51._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_23_53._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_21_51._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_24_54, 0, degree), Coeffs(&_pgen_tmp_25_55, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_21_51._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_23_53._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_22_52._poly);
  Free_data(&_pgen_tmp_23_53._c1_poly);
  Free_data(&_pgen_tmp_23_53._c0_poly);
  Free_data(&_relu_tmp14_4._c1_poly);
  Free_data(&_relu_tmp14_4._c0_poly);
  Free_data(&_pgen_tmp_24_54);
  Free_data(&_pgen_tmp_24_54);
  Free_data(&_pgen_tmp_25_55);
  Free_data(&_pgen_tmp_25_55);
  CMPLR_TM_END(16, 0);
  _preg_268435461 = Relinearize(_pgen_tmp_21_51);
  Free_data(&_pgen_tmp_21_51._c2_poly);
  Free_data(&_pgen_tmp_21_51._c1_poly);
  Free_data(&_pgen_tmp_21_51._c0_poly);
  Encode_float_mask(&_pgen_tmp_26_56, 2.944661935353764636858, mask_len_2, 1, Level(&input_ct1_1), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_27_57, &input_ct1_1, &_pgen_tmp_26_56);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_28_58, &_preg_268435461, &_pgen_tmp_27_57);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_relu_tmp58_7, &_preg_268435460, &_pgen_tmp_28_58);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_relu_tmp58_7._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_27_57._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_26_56._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_27_57._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_26_56._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_28_58._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435461._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_27_57._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_28_58._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435461._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_27_57._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435460._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_28_58._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435460._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_28_58._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_26_56._poly);
  Free_data(&_pgen_tmp_27_57._c1_poly);
  Free_data(&_pgen_tmp_27_57._c0_poly);
  Free_data(&_preg_268435461._c1_poly);
  Free_data(&_preg_268435461._c0_poly);
  Free_data(&_pgen_tmp_28_58._c1_poly);
  Free_data(&_pgen_tmp_28_58._c0_poly);
  Free_data(&_preg_268435460._c1_poly);
  Free_data(&_preg_268435460._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_29_59, &_relu_tmp58_7, &_relu_tmp58_7);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_29_59._c0_poly);
  Init_poly_by_size(&_pgen_tmp_30_60, 1, 0);
  Init_poly_by_size(&_pgen_tmp_31_61, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_30_60, 0, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_31_61, 0, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_29_59._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_29_59._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_30_60, 0, degree), Coeffs(&_pgen_tmp_31_61, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_29_59._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_30_60);
  Free_data(&_pgen_tmp_30_60);
  Free_data(&_pgen_tmp_31_61);
  Free_data(&_pgen_tmp_31_61);
  CMPLR_TM_END(16, 0);
  _preg_268435462 = Relinearize(_pgen_tmp_29_59);
  Free_data(&_pgen_tmp_29_59._c2_poly);
  Free_data(&_pgen_tmp_29_59._c1_poly);
  Free_data(&_pgen_tmp_29_59._c0_poly);
  Copy_ciph(&_relu_tmp62_8, &_preg_268435462);
  Free_data(&_preg_268435462._c1_poly);
  Free_data(&_preg_268435462._c0_poly);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_32_62, &_relu_tmp62_8, &_relu_tmp62_8);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_33_63, &_cst_142, 1, Sc_degree(&_pgen_tmp_32_62), Level(&_pgen_tmp_32_62), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp68_9, &_pgen_tmp_32_62, &_pgen_tmp_33_63);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_relu_tmp68_9._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_32_62._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp62_8._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp62_8._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_32_62._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp62_8._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp62_8._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_32_62._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_33_63._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Set_coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_30, degree, Coeffs(&_pgen_tmp_32_62._c1_poly, _pgen_rns_idx_30, degree));
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_relu_tmp62_8._c1_poly);
  Free_data(&_relu_tmp62_8._c0_poly);
  Free_data(&_pgen_tmp_33_63._poly);
  Free_data(&_pgen_tmp_32_62._c1_poly);
  Free_data(&_pgen_tmp_32_62._c0_poly);
  CMPLR_TM_END(13, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_34_64, &_relu_tmp58_7, &_relu_tmp68_9);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_34_64._c0_poly);
  Init_poly_by_size(&_pgen_tmp_35_65, 1, 0);
  Init_poly_by_size(&_pgen_tmp_36_66, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_35_65, 0, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_36_66, 0, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_34_64._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_34_64._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_35_65, 0, degree), Coeffs(&_pgen_tmp_36_66, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_34_64._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_35_65);
  Free_data(&_pgen_tmp_35_65);
  Free_data(&_pgen_tmp_36_66);
  Free_data(&_pgen_tmp_36_66);
  CMPLR_TM_END(16, 0);
  _preg_268435463 = Relinearize(_pgen_tmp_34_64);
  Free_data(&_pgen_tmp_34_64._c2_poly);
  Free_data(&_pgen_tmp_34_64._c1_poly);
  Free_data(&_pgen_tmp_34_64._c0_poly);
  Copy_ciph(&_relu_tmp72_10, &_preg_268435463);
  Free_data(&_preg_268435463._c1_poly);
  Free_data(&_preg_268435463._c0_poly);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_37_67, &_relu_tmp72_10, &_relu_tmp72_10);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_38_68, &_cst_143, 1, 1, Level(&_relu_tmp58_7), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_39_69, &_relu_tmp58_7, &_pgen_tmp_38_68);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_relu_tmp80_11, &_pgen_tmp_37_67, &_pgen_tmp_39_69);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_relu_tmp80_11._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_37_67._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp72_10._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp72_10._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_37_67._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp72_10._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp72_10._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_39_69._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_38_68._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_39_69._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_38_68._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp80_11._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_37_67._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_39_69._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp80_11._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_37_67._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_39_69._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_relu_tmp72_10._c1_poly);
  Free_data(&_relu_tmp72_10._c0_poly);
  Free_data(&_pgen_tmp_38_68._poly);
  Free_data(&_pgen_tmp_39_69._c1_poly);
  Free_data(&_pgen_tmp_39_69._c0_poly);
  Free_data(&_pgen_tmp_37_67._c1_poly);
  Free_data(&_pgen_tmp_37_67._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_40_70, &_relu_tmp68_9, &_relu_tmp68_9);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_40_70._c0_poly);
  Init_poly_by_size(&_pgen_tmp_41_71, 1, 0);
  Init_poly_by_size(&_pgen_tmp_42_72, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_41_71, 0, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_42_72, 0, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_40_70._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_40_70._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_41_71, 0, degree), Coeffs(&_pgen_tmp_42_72, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_40_70._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_41_71);
  Free_data(&_pgen_tmp_41_71);
  Free_data(&_pgen_tmp_42_72);
  Free_data(&_pgen_tmp_42_72);
  CMPLR_TM_END(16, 0);
  _preg_268435464 = Relinearize(_pgen_tmp_40_70);
  Free_data(&_pgen_tmp_40_70._c2_poly);
  Free_data(&_pgen_tmp_40_70._c1_poly);
  Free_data(&_pgen_tmp_40_70._c0_poly);
  Copy_ciph(&_relu_tmp84_12, &_preg_268435464);
  Free_data(&_preg_268435464._c1_poly);
  Free_data(&_preg_268435464._c0_poly);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_43_73, &_relu_tmp84_12, &_relu_tmp84_12);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_44_74, &_cst_144, 1, Sc_degree(&_pgen_tmp_43_73), Level(&_pgen_tmp_43_73), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp90_13, &_pgen_tmp_43_73, &_pgen_tmp_44_74);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_relu_tmp90_13._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_43_73._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp84_12._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp84_12._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_43_73._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp84_12._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp84_12._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_43_73._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_44_74._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Set_coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_30, degree, Coeffs(&_pgen_tmp_43_73._c1_poly, _pgen_rns_idx_30, degree));
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_relu_tmp84_12._c1_poly);
  Free_data(&_relu_tmp84_12._c0_poly);
  Free_data(&_pgen_tmp_44_74._poly);
  Free_data(&_pgen_tmp_43_73._c1_poly);
  Free_data(&_pgen_tmp_43_73._c0_poly);
  CMPLR_TM_END(13, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_45_75, &_relu_tmp90_13, &_relu_tmp90_13);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_45_75._c0_poly);
  Init_poly_by_size(&_pgen_tmp_46_76, 1, 0);
  Init_poly_by_size(&_pgen_tmp_47_77, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_46_76, 0, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_47_77, 0, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_45_75._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_45_75._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_46_76, 0, degree), Coeffs(&_pgen_tmp_47_77, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_45_75._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_46_76);
  Free_data(&_pgen_tmp_46_76);
  Free_data(&_pgen_tmp_47_77);
  Free_data(&_pgen_tmp_47_77);
  CMPLR_TM_END(16, 0);
  _preg_268435465 = Relinearize(_pgen_tmp_45_75);
  Free_data(&_pgen_tmp_45_75._c2_poly);
  Free_data(&_pgen_tmp_45_75._c1_poly);
  Free_data(&_pgen_tmp_45_75._c0_poly);
  Copy_ciph(&_relu_tmp94_14, &_preg_268435465);
  Free_data(&_preg_268435465._c1_poly);
  Free_data(&_preg_268435465._c0_poly);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_48_78, &_relu_tmp94_14, &_relu_tmp94_14);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_49_79, &_cst_145, 1, Sc_degree(&_pgen_tmp_48_78), Level(&_pgen_tmp_48_78), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp100_15, &_pgen_tmp_48_78, &_pgen_tmp_49_79);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_relu_tmp100_15._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_48_78._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp94_14._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp94_14._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_48_78._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp94_14._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp94_14._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp100_15._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_48_78._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_49_79._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Set_coeffs(&_relu_tmp100_15._c1_poly, _pgen_rns_idx_30, degree, Coeffs(&_pgen_tmp_48_78._c1_poly, _pgen_rns_idx_30, degree));
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_relu_tmp94_14._c1_poly);
  Free_data(&_relu_tmp94_14._c0_poly);
  Free_data(&_pgen_tmp_49_79._poly);
  Free_data(&_pgen_tmp_48_78._c1_poly);
  Free_data(&_pgen_tmp_48_78._c0_poly);
  CMPLR_TM_END(13, 0);
  Encode_float_mask(&_pgen_tmp_53_83, -0.0001600280537250875771867, mask_len_2, 1, Level(&_relu_tmp58_7), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_54_84, &_relu_tmp58_7, &_pgen_tmp_53_83);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_52_82, &_relu_tmp68_9, &_pgen_tmp_54_84);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_52_82._c0_poly);
  Init_poly_by_size(&_pgen_tmp_55_85, 1, 0);
  Init_poly_by_size(&_pgen_tmp_56_86, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_54_84._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_53_83._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_54_84._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_53_83._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_55_85, 0, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_54_84._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_56_86, 0, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_54_84._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_52_82._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_54_84._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_52_82._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_55_85, 0, degree), Coeffs(&_pgen_tmp_56_86, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_52_82._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_54_84._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_53_83._poly);
  Free_data(&_pgen_tmp_54_84._c1_poly);
  Free_data(&_pgen_tmp_54_84._c0_poly);
  Free_data(&_relu_tmp68_9._c1_poly);
  Free_data(&_relu_tmp68_9._c0_poly);
  Free_data(&_pgen_tmp_55_85);
  Free_data(&_pgen_tmp_55_85);
  Free_data(&_pgen_tmp_56_86);
  Free_data(&_pgen_tmp_56_86);
  CMPLR_TM_END(16, 0);
  _preg_268435466 = Relinearize(_pgen_tmp_52_82);
  Free_data(&_pgen_tmp_52_82._c2_poly);
  Free_data(&_pgen_tmp_52_82._c1_poly);
  Free_data(&_pgen_tmp_52_82._c0_poly);
  Encode_float_mask(&_pgen_tmp_57_87, 0.003032478474127557870749, mask_len_2, 1, Level(&_relu_tmp58_7), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_58_88, &_relu_tmp58_7, &_pgen_tmp_57_87);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_59_89, &_preg_268435466, &_pgen_tmp_58_88);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_51_81, &_relu_tmp90_13, &_pgen_tmp_59_89);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_51_81._c0_poly);
  Init_poly_by_size(&_pgen_tmp_60_90, 1, 0);
  Init_poly_by_size(&_pgen_tmp_61_91, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_58_88._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_57_87._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_58_88._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_57_87._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_59_89._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435466._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_58_88._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_59_89._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435466._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_58_88._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_60_90, 0, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_59_89._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_61_91, 0, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_59_89._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_51_81._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_59_89._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_51_81._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_60_90, 0, degree), Coeffs(&_pgen_tmp_61_91, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_51_81._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_59_89._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_57_87._poly);
  Free_data(&_pgen_tmp_58_88._c1_poly);
  Free_data(&_pgen_tmp_58_88._c0_poly);
  Free_data(&_preg_268435466._c1_poly);
  Free_data(&_preg_268435466._c0_poly);
  Free_data(&_pgen_tmp_59_89._c1_poly);
  Free_data(&_pgen_tmp_59_89._c0_poly);
  Free_data(&_pgen_tmp_60_90);
  Free_data(&_pgen_tmp_60_90);
  Free_data(&_pgen_tmp_61_91);
  Free_data(&_pgen_tmp_61_91);
  CMPLR_TM_END(16, 0);
  _preg_268435467 = Relinearize(_pgen_tmp_51_81);
  Free_data(&_pgen_tmp_51_81._c2_poly);
  Free_data(&_pgen_tmp_51_81._c1_poly);
  Free_data(&_pgen_tmp_51_81._c0_poly);
  Encode_double(&_pgen_tmp_62_92, &_cst_152, 1, 1, Level(&_relu_tmp80_11), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_63_93, &_relu_tmp80_11, &_pgen_tmp_62_92);
  CMPLR_TM_END(17, 0);
  Encode_float_mask(&_pgen_tmp_64_94, 0.1365828361194337536855, mask_len_2, 1, Level(&_relu_tmp58_7), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_65_95, &_relu_tmp58_7, &_pgen_tmp_64_94);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_66_96, &_pgen_tmp_63_93, &_pgen_tmp_65_95);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_67_97, &_preg_268435467, &_pgen_tmp_66_96);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_50_80, &_relu_tmp100_15, &_pgen_tmp_67_97);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_50_80._c0_poly);
  Init_poly_by_size(&_pgen_tmp_68_98, 1, 0);
  Init_poly_by_size(&_pgen_tmp_69_99, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_63_93._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp80_11._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_62_92._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_63_93._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp80_11._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_62_92._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_65_95._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_64_94._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_65_95._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_64_94._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_66_96._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_63_93._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_65_95._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_66_96._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_63_93._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_65_95._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_67_97._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435467._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_66_96._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_67_97._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435467._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_66_96._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_68_98, 0, degree), Coeffs(&_relu_tmp100_15._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_67_97._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_69_99, 0, degree), Coeffs(&_relu_tmp100_15._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_67_97._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_50_80._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp100_15._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_67_97._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_50_80._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_68_98, 0, degree), Coeffs(&_pgen_tmp_69_99, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_50_80._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp100_15._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_67_97._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_62_92._poly);
  Free_data(&_pgen_tmp_64_94._poly);
  Free_data(&_pgen_tmp_65_95._c1_poly);
  Free_data(&_pgen_tmp_65_95._c0_poly);
  Free_data(&_pgen_tmp_63_93._c1_poly);
  Free_data(&_pgen_tmp_63_93._c0_poly);
  Free_data(&_pgen_tmp_66_96._c1_poly);
  Free_data(&_pgen_tmp_66_96._c0_poly);
  Free_data(&_preg_268435467._c1_poly);
  Free_data(&_preg_268435467._c0_poly);
  Free_data(&_pgen_tmp_67_97._c1_poly);
  Free_data(&_pgen_tmp_67_97._c0_poly);
  Free_data(&_relu_tmp100_15._c1_poly);
  Free_data(&_relu_tmp100_15._c0_poly);
  Free_data(&_pgen_tmp_68_98);
  Free_data(&_pgen_tmp_68_98);
  Free_data(&_pgen_tmp_69_99);
  Free_data(&_pgen_tmp_69_99);
  CMPLR_TM_END(16, 0);
  _preg_268435468 = Relinearize(_pgen_tmp_50_80);
  Free_data(&_pgen_tmp_50_80._c2_poly);
  Free_data(&_pgen_tmp_50_80._c1_poly);
  Free_data(&_pgen_tmp_50_80._c0_poly);
  Encode_double(&_pgen_tmp_71_101, &_cst_155, 1, 1, Level(&_relu_tmp80_11), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_72_102, &_relu_tmp80_11, &_pgen_tmp_71_101);
  CMPLR_TM_END(17, 0);
  Encode_float_mask(&_pgen_tmp_73_103, 0.5685231134608953462717, mask_len_2, 1, Level(&_relu_tmp58_7), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_74_104, &_relu_tmp58_7, &_pgen_tmp_73_103);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_75_105, &_pgen_tmp_72_102, &_pgen_tmp_74_104);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_70_100, &_relu_tmp90_13, &_pgen_tmp_75_105);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_70_100._c0_poly);
  Init_poly_by_size(&_pgen_tmp_76_106, 1, 0);
  Init_poly_by_size(&_pgen_tmp_77_107, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_72_102._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp80_11._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_71_101._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_72_102._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp80_11._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_71_101._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_74_104._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_73_103._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_74_104._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_73_103._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_75_105._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_72_102._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_74_104._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_75_105._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_72_102._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_74_104._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_76_106, 0, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_75_105._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_77_107, 0, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_75_105._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_70_100._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_75_105._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_70_100._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_76_106, 0, degree), Coeffs(&_pgen_tmp_77_107, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_70_100._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_75_105._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_71_101._poly);
  Free_data(&_pgen_tmp_73_103._poly);
  Free_data(&_pgen_tmp_74_104._c1_poly);
  Free_data(&_pgen_tmp_74_104._c0_poly);
  Free_data(&_pgen_tmp_72_102._c1_poly);
  Free_data(&_pgen_tmp_72_102._c0_poly);
  Free_data(&_pgen_tmp_75_105._c1_poly);
  Free_data(&_pgen_tmp_75_105._c0_poly);
  Free_data(&_relu_tmp90_13._c1_poly);
  Free_data(&_relu_tmp90_13._c0_poly);
  Free_data(&_pgen_tmp_76_106);
  Free_data(&_pgen_tmp_76_106);
  Free_data(&_pgen_tmp_77_107);
  Free_data(&_pgen_tmp_77_107);
  CMPLR_TM_END(16, 0);
  _preg_268435469 = Relinearize(_pgen_tmp_70_100);
  Free_data(&_pgen_tmp_70_100._c2_poly);
  Free_data(&_pgen_tmp_70_100._c1_poly);
  Free_data(&_pgen_tmp_70_100._c0_poly);
  Encode_double(&_pgen_tmp_78_108, &_cst_158, 1, 1, Level(&_relu_tmp80_11), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_79_109, &_relu_tmp80_11, &_pgen_tmp_78_108);
  CMPLR_TM_END(17, 0);
  Encode_float_mask(&_pgen_tmp_80_110, 1.614771079042867985009, mask_len_2, 1, Level(&_relu_tmp58_7), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_81_111, &_relu_tmp58_7, &_pgen_tmp_80_110);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_82_112, &_pgen_tmp_79_109, &_pgen_tmp_81_111);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_83_113, &_preg_268435469, &_pgen_tmp_82_112);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_relu_tmp155_16, &_preg_268435468, &_pgen_tmp_83_113);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_relu_tmp155_16._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_79_109._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp80_11._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_78_108._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_79_109._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp80_11._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_78_108._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_81_111._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_80_110._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_81_111._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_80_110._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_82_112._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_79_109._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_81_111._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_82_112._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_79_109._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_81_111._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_83_113._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435469._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_82_112._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_83_113._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435469._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_82_112._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435468._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_83_113._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435468._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_83_113._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_78_108._poly);
  Free_data(&_relu_tmp80_11._c1_poly);
  Free_data(&_relu_tmp80_11._c0_poly);
  Free_data(&_pgen_tmp_80_110._poly);
  Free_data(&_relu_tmp58_7._c1_poly);
  Free_data(&_relu_tmp58_7._c0_poly);
  Free_data(&_pgen_tmp_81_111._c1_poly);
  Free_data(&_pgen_tmp_81_111._c0_poly);
  Free_data(&_pgen_tmp_79_109._c1_poly);
  Free_data(&_pgen_tmp_79_109._c0_poly);
  Free_data(&_pgen_tmp_82_112._c1_poly);
  Free_data(&_pgen_tmp_82_112._c0_poly);
  Free_data(&_preg_268435469._c1_poly);
  Free_data(&_preg_268435469._c0_poly);
  Free_data(&_pgen_tmp_83_113._c1_poly);
  Free_data(&_pgen_tmp_83_113._c0_poly);
  Free_data(&_preg_268435468._c1_poly);
  Free_data(&_preg_268435468._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_84_114, &_relu_tmp155_16, &_relu_tmp155_16);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_84_114._c0_poly);
  Init_poly_by_size(&_pgen_tmp_85_115, 1, 0);
  Init_poly_by_size(&_pgen_tmp_86_116, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_85_115, 0, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_86_116, 0, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_84_114._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_84_114._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_85_115, 0, degree), Coeffs(&_pgen_tmp_86_116, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_84_114._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_85_115);
  Free_data(&_pgen_tmp_85_115);
  Free_data(&_pgen_tmp_86_116);
  Free_data(&_pgen_tmp_86_116);
  CMPLR_TM_END(16, 0);
  _preg_268435470 = Relinearize(_pgen_tmp_84_114);
  Free_data(&_pgen_tmp_84_114._c2_poly);
  Free_data(&_pgen_tmp_84_114._c1_poly);
  Free_data(&_pgen_tmp_84_114._c0_poly);
  Copy_ciph(&_relu_tmp159_17, &_preg_268435470);
  Free_data(&_preg_268435470._c1_poly);
  Free_data(&_preg_268435470._c0_poly);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_87_117, &_relu_tmp159_17, &_relu_tmp159_17);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_88_118, &_cst_159, 1, Sc_degree(&_pgen_tmp_87_117), Level(&_pgen_tmp_87_117), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp165_18, &_pgen_tmp_87_117, &_pgen_tmp_88_118);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_relu_tmp165_18._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_87_117._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp159_17._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp159_17._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_87_117._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp159_17._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp159_17._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp165_18._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_87_117._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_88_118._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Set_coeffs(&_relu_tmp165_18._c1_poly, _pgen_rns_idx_30, degree, Coeffs(&_pgen_tmp_87_117._c1_poly, _pgen_rns_idx_30, degree));
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_relu_tmp159_17._c1_poly);
  Free_data(&_relu_tmp159_17._c0_poly);
  Free_data(&_pgen_tmp_88_118._poly);
  Free_data(&_pgen_tmp_87_117._c1_poly);
  Free_data(&_pgen_tmp_87_117._c0_poly);
  CMPLR_TM_END(13, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_89_119, &_relu_tmp155_16, &_relu_tmp165_18);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_89_119._c0_poly);
  Init_poly_by_size(&_pgen_tmp_90_120, 1, 0);
  Init_poly_by_size(&_pgen_tmp_91_121, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_90_120, 0, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp165_18._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_91_121, 0, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp165_18._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_89_119._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp165_18._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_89_119._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_90_120, 0, degree), Coeffs(&_pgen_tmp_91_121, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_89_119._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp165_18._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_90_120);
  Free_data(&_pgen_tmp_90_120);
  Free_data(&_pgen_tmp_91_121);
  Free_data(&_pgen_tmp_91_121);
  CMPLR_TM_END(16, 0);
  _preg_268435471 = Relinearize(_pgen_tmp_89_119);
  Free_data(&_pgen_tmp_89_119._c2_poly);
  Free_data(&_pgen_tmp_89_119._c1_poly);
  Free_data(&_pgen_tmp_89_119._c0_poly);
  Copy_ciph(&_relu_tmp169_19, &_preg_268435471);
  Free_data(&_preg_268435471._c1_poly);
  Free_data(&_preg_268435471._c0_poly);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_92_122, &_relu_tmp169_19, &_relu_tmp169_19);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_93_123, &_cst_160, 1, 1, Level(&_relu_tmp155_16), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_94_124, &_relu_tmp155_16, &_pgen_tmp_93_123);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_relu_tmp177_20, &_pgen_tmp_92_122, &_pgen_tmp_94_124);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_relu_tmp177_20._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_92_122._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp169_19._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp169_19._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_92_122._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp169_19._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp169_19._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_94_124._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_93_123._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_94_124._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_93_123._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp177_20._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_92_122._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_94_124._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp177_20._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_92_122._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_94_124._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_relu_tmp169_19._c1_poly);
  Free_data(&_relu_tmp169_19._c0_poly);
  Free_data(&_pgen_tmp_93_123._poly);
  Free_data(&_pgen_tmp_94_124._c1_poly);
  Free_data(&_pgen_tmp_94_124._c0_poly);
  Free_data(&_pgen_tmp_92_122._c1_poly);
  Free_data(&_pgen_tmp_92_122._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_95_125, &_relu_tmp165_18, &_relu_tmp165_18);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_95_125._c0_poly);
  Init_poly_by_size(&_pgen_tmp_96_126, 1, 0);
  Init_poly_by_size(&_pgen_tmp_97_127, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_96_126, 0, degree), Coeffs(&_relu_tmp165_18._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp165_18._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_97_127, 0, degree), Coeffs(&_relu_tmp165_18._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp165_18._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_95_125._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp165_18._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp165_18._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_95_125._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_96_126, 0, degree), Coeffs(&_pgen_tmp_97_127, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_95_125._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp165_18._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp165_18._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_relu_tmp165_18._c1_poly);
  Free_data(&_relu_tmp165_18._c0_poly);
  Free_data(&_pgen_tmp_96_126);
  Free_data(&_pgen_tmp_96_126);
  Free_data(&_pgen_tmp_97_127);
  Free_data(&_pgen_tmp_97_127);
  CMPLR_TM_END(16, 0);
  _preg_268435472 = Relinearize(_pgen_tmp_95_125);
  Free_data(&_pgen_tmp_95_125._c2_poly);
  Free_data(&_pgen_tmp_95_125._c1_poly);
  Free_data(&_pgen_tmp_95_125._c0_poly);
  Copy_ciph(&_relu_tmp181_21, &_preg_268435472);
  Free_data(&_preg_268435472._c1_poly);
  Free_data(&_preg_268435472._c0_poly);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_98_128, &_relu_tmp181_21, &_relu_tmp181_21);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_99_129, &_cst_161, 1, Sc_degree(&_pgen_tmp_98_128), Level(&_pgen_tmp_98_128), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp187_22, &_pgen_tmp_98_128, &_pgen_tmp_99_129);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_relu_tmp187_22._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_98_128._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp181_21._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp181_21._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_98_128._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp181_21._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp181_21._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_98_128._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_99_129._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Set_coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_30, degree, Coeffs(&_pgen_tmp_98_128._c1_poly, _pgen_rns_idx_30, degree));
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_relu_tmp181_21._c1_poly);
  Free_data(&_relu_tmp181_21._c0_poly);
  Free_data(&_pgen_tmp_99_129._poly);
  Free_data(&_pgen_tmp_98_128._c1_poly);
  Free_data(&_pgen_tmp_98_128._c0_poly);
  CMPLR_TM_END(13, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_100_130, &_relu_tmp187_22, &_relu_tmp187_22);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_100_130._c0_poly);
  Init_poly_by_size(&_pgen_tmp_101_131, 1, 0);
  Init_poly_by_size(&_pgen_tmp_102_132, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_101_131, 0, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_102_132, 0, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_100_130._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_100_130._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_101_131, 0, degree), Coeffs(&_pgen_tmp_102_132, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_100_130._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_101_131);
  Free_data(&_pgen_tmp_101_131);
  Free_data(&_pgen_tmp_102_132);
  Free_data(&_pgen_tmp_102_132);
  CMPLR_TM_END(16, 0);
  _preg_268435473 = Relinearize(_pgen_tmp_100_130);
  Free_data(&_pgen_tmp_100_130._c2_poly);
  Free_data(&_pgen_tmp_100_130._c1_poly);
  Free_data(&_pgen_tmp_100_130._c0_poly);
  Copy_ciph(&_relu_tmp191_23, &_preg_268435473);
  Free_data(&_preg_268435473._c1_poly);
  Free_data(&_preg_268435473._c0_poly);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_103_133, &_relu_tmp191_23, &_relu_tmp191_23);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_104_134, &_cst_162, 1, Sc_degree(&_pgen_tmp_103_133), Level(&_pgen_tmp_103_133), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp197_24, &_pgen_tmp_103_133, &_pgen_tmp_104_134);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_relu_tmp197_24._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_103_133._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp191_23._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp191_23._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_103_133._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp191_23._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp191_23._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp197_24._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_103_133._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_104_134._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Set_coeffs(&_relu_tmp197_24._c1_poly, _pgen_rns_idx_30, degree, Coeffs(&_pgen_tmp_103_133._c1_poly, _pgen_rns_idx_30, degree));
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_relu_tmp191_23._c1_poly);
  Free_data(&_relu_tmp191_23._c0_poly);
  Free_data(&_pgen_tmp_104_134._poly);
  Free_data(&_pgen_tmp_103_133._c1_poly);
  Free_data(&_pgen_tmp_103_133._c0_poly);
  CMPLR_TM_END(13, 0);
  Encode_float_mask(&_pgen_tmp_108_138, 0.0001111119162030077897477, mask_len_2, 1, Level(&_relu_tmp155_16), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_109_139, &_relu_tmp155_16, &_pgen_tmp_108_138);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_107_137, &input_ct0_0, &_pgen_tmp_109_139);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_107_137._c0_poly);
  Init_poly_by_size(&_pgen_tmp_110_140, 1, 0);
  Init_poly_by_size(&_pgen_tmp_111_141, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_109_139._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_108_138._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_109_139._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_108_138._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_110_140, 0, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_109_139._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_111_141, 0, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_109_139._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_107_137._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_109_139._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_107_137._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_110_140, 0, degree), Coeffs(&_pgen_tmp_111_141, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_107_137._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_109_139._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_108_138._poly);
  Free_data(&_pgen_tmp_109_139._c1_poly);
  Free_data(&_pgen_tmp_109_139._c0_poly);
  Free_data(&_pgen_tmp_110_140);
  Free_data(&_pgen_tmp_110_140);
  Free_data(&_pgen_tmp_111_141);
  Free_data(&_pgen_tmp_111_141);
  CMPLR_TM_END(16, 0);
  _preg_268435474 = Relinearize(_pgen_tmp_107_137);
  Free_data(&_pgen_tmp_107_137._c2_poly);
  Free_data(&_pgen_tmp_107_137._c1_poly);
  Free_data(&_pgen_tmp_107_137._c0_poly);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_106_136, &_relu_tmp187_22, &_preg_268435474);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_106_136._c0_poly);
  Init_poly_by_size(&_pgen_tmp_112_142, 1, 0);
  Init_poly_by_size(&_pgen_tmp_113_143, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_112_142, 0, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435474._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_113_143, 0, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435474._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_106_136._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435474._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_106_136._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_112_142, 0, degree), Coeffs(&_pgen_tmp_113_143, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_106_136._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435474._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_preg_268435474._c1_poly);
  Free_data(&_preg_268435474._c0_poly);
  Free_data(&_pgen_tmp_112_142);
  Free_data(&_pgen_tmp_112_142);
  Free_data(&_pgen_tmp_113_143);
  Free_data(&_pgen_tmp_113_143);
  CMPLR_TM_END(16, 0);
  _preg_268435475 = Relinearize(_pgen_tmp_106_136);
  Free_data(&_pgen_tmp_106_136._c2_poly);
  Free_data(&_pgen_tmp_106_136._c1_poly);
  Free_data(&_pgen_tmp_106_136._c0_poly);
  Encode_float_mask(&_pgen_tmp_115_145, -0.0009371069270347270278171, mask_len_2, 1, Level(&input_ct0_0), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_116_146, &input_ct0_0, &_pgen_tmp_115_145);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_114_144, &_relu_tmp177_20, &_pgen_tmp_116_146);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_114_144._c0_poly);
  Init_poly_by_size(&_pgen_tmp_117_147, 1, 0);
  Init_poly_by_size(&_pgen_tmp_118_148, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_116_146._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_115_145._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_116_146._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_115_145._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_117_147, 0, degree), Coeffs(&_relu_tmp177_20._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_116_146._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_118_148, 0, degree), Coeffs(&_relu_tmp177_20._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_116_146._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_114_144._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp177_20._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_116_146._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_114_144._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_117_147, 0, degree), Coeffs(&_pgen_tmp_118_148, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_114_144._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp177_20._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_116_146._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_115_145._poly);
  Free_data(&_pgen_tmp_116_146._c1_poly);
  Free_data(&_pgen_tmp_116_146._c0_poly);
  Free_data(&_pgen_tmp_117_147);
  Free_data(&_pgen_tmp_117_147);
  Free_data(&_pgen_tmp_118_148);
  Free_data(&_pgen_tmp_118_148);
  CMPLR_TM_END(16, 0);
  _preg_268435476 = Relinearize(_pgen_tmp_114_144);
  Free_data(&_pgen_tmp_114_144._c2_poly);
  Free_data(&_pgen_tmp_114_144._c1_poly);
  Free_data(&_pgen_tmp_114_144._c0_poly);
  Encode_float_mask(&_pgen_tmp_120_150, 0.006507995190210730772351, mask_len_2, 1, Level(&_relu_tmp155_16), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_121_151, &_relu_tmp155_16, &_pgen_tmp_120_150);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_119_149, &input_ct0_0, &_pgen_tmp_121_151);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_119_149._c0_poly);
  Init_poly_by_size(&_pgen_tmp_122_152, 1, 0);
  Init_poly_by_size(&_pgen_tmp_123_153, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_121_151._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_120_150._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_121_151._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_120_150._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_122_152, 0, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_121_151._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_123_153, 0, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_121_151._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_119_149._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_121_151._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_119_149._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_122_152, 0, degree), Coeffs(&_pgen_tmp_123_153, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_119_149._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_121_151._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_120_150._poly);
  Free_data(&_pgen_tmp_121_151._c1_poly);
  Free_data(&_pgen_tmp_121_151._c0_poly);
  Free_data(&_pgen_tmp_122_152);
  Free_data(&_pgen_tmp_122_152);
  Free_data(&_pgen_tmp_123_153);
  Free_data(&_pgen_tmp_123_153);
  CMPLR_TM_END(16, 0);
  _preg_268435477 = Relinearize(_pgen_tmp_119_149);
  Free_data(&_pgen_tmp_119_149._c2_poly);
  Free_data(&_pgen_tmp_119_149._c1_poly);
  Free_data(&_pgen_tmp_119_149._c0_poly);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_124_154, &_preg_268435476, &_preg_268435477);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_125_155, &_preg_268435475, &_pgen_tmp_124_154);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_105_135, &_relu_tmp197_24, &_pgen_tmp_125_155);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_105_135._c0_poly);
  Init_poly_by_size(&_pgen_tmp_126_156, 1, 0);
  Init_poly_by_size(&_pgen_tmp_127_157, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_124_154._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435476._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435477._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_124_154._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435476._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435477._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_125_155._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435475._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_124_154._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_125_155._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435475._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_124_154._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_126_156, 0, degree), Coeffs(&_relu_tmp197_24._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_125_155._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_127_157, 0, degree), Coeffs(&_relu_tmp197_24._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_125_155._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_105_135._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp197_24._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_125_155._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_105_135._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_126_156, 0, degree), Coeffs(&_pgen_tmp_127_157, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_105_135._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp197_24._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_125_155._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_preg_268435477._c1_poly);
  Free_data(&_preg_268435477._c0_poly);
  Free_data(&_preg_268435476._c1_poly);
  Free_data(&_preg_268435476._c0_poly);
  Free_data(&_pgen_tmp_124_154._c1_poly);
  Free_data(&_pgen_tmp_124_154._c0_poly);
  Free_data(&_preg_268435475._c1_poly);
  Free_data(&_preg_268435475._c0_poly);
  Free_data(&_pgen_tmp_125_155._c1_poly);
  Free_data(&_pgen_tmp_125_155._c0_poly);
  Free_data(&_relu_tmp197_24._c1_poly);
  Free_data(&_relu_tmp197_24._c0_poly);
  Free_data(&_pgen_tmp_126_156);
  Free_data(&_pgen_tmp_126_156);
  Free_data(&_pgen_tmp_127_157);
  Free_data(&_pgen_tmp_127_157);
  CMPLR_TM_END(16, 0);
  _preg_268435478 = Relinearize(_pgen_tmp_105_135);
  Free_data(&_pgen_tmp_105_135._c2_poly);
  Free_data(&_pgen_tmp_105_135._c1_poly);
  Free_data(&_pgen_tmp_105_135._c0_poly);
  Encode_float_mask(&_pgen_tmp_130_160, -0.03691063940349948585551, mask_len_2, 1, Level(&input_ct0_0), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_131_161, &input_ct0_0, &_pgen_tmp_130_160);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_129_159, &_relu_tmp177_20, &_pgen_tmp_131_161);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_129_159._c0_poly);
  Init_poly_by_size(&_pgen_tmp_132_162, 1, 0);
  Init_poly_by_size(&_pgen_tmp_133_163, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_131_161._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_130_160._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_131_161._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_130_160._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_132_162, 0, degree), Coeffs(&_relu_tmp177_20._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_131_161._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_133_163, 0, degree), Coeffs(&_relu_tmp177_20._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_131_161._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_129_159._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp177_20._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_131_161._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_129_159._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_132_162, 0, degree), Coeffs(&_pgen_tmp_133_163, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_129_159._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp177_20._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_131_161._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_130_160._poly);
  Free_data(&_pgen_tmp_131_161._c1_poly);
  Free_data(&_pgen_tmp_131_161._c0_poly);
  Free_data(&_pgen_tmp_132_162);
  Free_data(&_pgen_tmp_132_162);
  Free_data(&_pgen_tmp_133_163);
  Free_data(&_pgen_tmp_133_163);
  CMPLR_TM_END(16, 0);
  _preg_268435479 = Relinearize(_pgen_tmp_129_159);
  Free_data(&_pgen_tmp_129_159._c2_poly);
  Free_data(&_pgen_tmp_129_159._c1_poly);
  Free_data(&_pgen_tmp_129_159._c0_poly);
  Encode_float_mask(&_pgen_tmp_135_165, 0.1056744718173614761003, mask_len_2, 1, Level(&_relu_tmp155_16), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_136_166, &_relu_tmp155_16, &_pgen_tmp_135_165);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_134_164, &input_ct0_0, &_pgen_tmp_136_166);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_134_164._c0_poly);
  Init_poly_by_size(&_pgen_tmp_137_167, 1, 0);
  Init_poly_by_size(&_pgen_tmp_138_168, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_136_166._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_135_165._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_136_166._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_135_165._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_137_167, 0, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_136_166._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_138_168, 0, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_136_166._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_134_164._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_136_166._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_134_164._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_137_167, 0, degree), Coeffs(&_pgen_tmp_138_168, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_134_164._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_136_166._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_135_165._poly);
  Free_data(&_pgen_tmp_136_166._c1_poly);
  Free_data(&_pgen_tmp_136_166._c0_poly);
  Free_data(&_pgen_tmp_137_167);
  Free_data(&_pgen_tmp_137_167);
  Free_data(&_pgen_tmp_138_168);
  Free_data(&_pgen_tmp_138_168);
  CMPLR_TM_END(16, 0);
  _preg_268435480 = Relinearize(_pgen_tmp_134_164);
  Free_data(&_pgen_tmp_134_164._c2_poly);
  Free_data(&_pgen_tmp_134_164._c1_poly);
  Free_data(&_pgen_tmp_134_164._c0_poly);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_139_169, &_preg_268435479, &_preg_268435480);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_128_158, &_relu_tmp187_22, &_pgen_tmp_139_169);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_128_158._c0_poly);
  Init_poly_by_size(&_pgen_tmp_140_170, 1, 0);
  Init_poly_by_size(&_pgen_tmp_141_171, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_139_169._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435479._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435480._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_139_169._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435479._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435480._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_140_170, 0, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_139_169._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_141_171, 0, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_139_169._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_128_158._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_139_169._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_128_158._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_140_170, 0, degree), Coeffs(&_pgen_tmp_141_171, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_128_158._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_139_169._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_preg_268435480._c1_poly);
  Free_data(&_preg_268435480._c0_poly);
  Free_data(&_preg_268435479._c1_poly);
  Free_data(&_preg_268435479._c0_poly);
  Free_data(&_pgen_tmp_139_169._c1_poly);
  Free_data(&_pgen_tmp_139_169._c0_poly);
  Free_data(&_relu_tmp187_22._c1_poly);
  Free_data(&_relu_tmp187_22._c0_poly);
  Free_data(&_pgen_tmp_140_170);
  Free_data(&_pgen_tmp_140_170);
  Free_data(&_pgen_tmp_141_171);
  Free_data(&_pgen_tmp_141_171);
  CMPLR_TM_END(16, 0);
  _preg_268435481 = Relinearize(_pgen_tmp_128_158);
  Free_data(&_pgen_tmp_128_158._c2_poly);
  Free_data(&_pgen_tmp_128_158._c1_poly);
  Free_data(&_pgen_tmp_128_158._c0_poly);
  Encode_float_mask(&_pgen_tmp_143_173, -0.2078597158810965850329, mask_len_2, 1, Level(&input_ct0_0), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_144_174, &input_ct0_0, &_pgen_tmp_143_173);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_142_172, &_relu_tmp177_20, &_pgen_tmp_144_174);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_142_172._c0_poly);
  Init_poly_by_size(&_pgen_tmp_145_175, 1, 0);
  Init_poly_by_size(&_pgen_tmp_146_176, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_144_174._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_143_173._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_144_174._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_143_173._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_145_175, 0, degree), Coeffs(&_relu_tmp177_20._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_144_174._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_146_176, 0, degree), Coeffs(&_relu_tmp177_20._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_144_174._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_142_172._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp177_20._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_144_174._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_142_172._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_145_175, 0, degree), Coeffs(&_pgen_tmp_146_176, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_142_172._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp177_20._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_144_174._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_143_173._poly);
  Free_data(&_pgen_tmp_144_174._c1_poly);
  Free_data(&_pgen_tmp_144_174._c0_poly);
  Free_data(&_relu_tmp177_20._c1_poly);
  Free_data(&_relu_tmp177_20._c0_poly);
  Free_data(&_pgen_tmp_145_175);
  Free_data(&_pgen_tmp_145_175);
  Free_data(&_pgen_tmp_146_176);
  Free_data(&_pgen_tmp_146_176);
  CMPLR_TM_END(16, 0);
  _preg_268435482 = Relinearize(_pgen_tmp_142_172);
  Free_data(&_pgen_tmp_142_172._c2_poly);
  Free_data(&_pgen_tmp_142_172._c1_poly);
  Free_data(&_pgen_tmp_142_172._c0_poly);
  Encode_float_mask(&_pgen_tmp_148_178, 0.6334139843709288841822, mask_len_2, 1, Level(&_relu_tmp155_16), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_149_179, &_relu_tmp155_16, &_pgen_tmp_148_178);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_147_177, &input_ct0_0, &_pgen_tmp_149_179);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_147_177._c0_poly);
  Init_poly_by_size(&_pgen_tmp_150_180, 1, 0);
  Init_poly_by_size(&_pgen_tmp_151_181, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_149_179._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_148_178._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_149_179._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_148_178._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_150_180, 0, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_149_179._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_151_181, 0, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_149_179._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_147_177._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_149_179._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_147_177._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_150_180, 0, degree), Coeffs(&_pgen_tmp_151_181, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_147_177._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_149_179._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_148_178._poly);
  Free_data(&_relu_tmp155_16._c1_poly);
  Free_data(&_relu_tmp155_16._c0_poly);
  Free_data(&_pgen_tmp_149_179._c1_poly);
  Free_data(&_pgen_tmp_149_179._c0_poly);
  Free_data(&_pgen_tmp_150_180);
  Free_data(&_pgen_tmp_150_180);
  Free_data(&_pgen_tmp_151_181);
  Free_data(&_pgen_tmp_151_181);
  CMPLR_TM_END(16, 0);
  _preg_268435483 = Relinearize(_pgen_tmp_147_177);
  Free_data(&_pgen_tmp_147_177._c2_poly);
  Free_data(&_pgen_tmp_147_177._c1_poly);
  Free_data(&_pgen_tmp_147_177._c0_poly);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_152_182, &_preg_268435482, &_preg_268435483);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_153_183, &_preg_268435481, &_pgen_tmp_152_182);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_relu_tmp266_25, &_preg_268435478, &_pgen_tmp_153_183);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_relu_tmp266_25._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_152_182._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435482._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435483._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_152_182._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435482._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435483._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_153_183._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435481._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_152_182._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_153_183._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435481._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_152_182._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp266_25._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435478._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_153_183._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp266_25._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435478._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_153_183._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_preg_268435483._c1_poly);
  Free_data(&_preg_268435483._c0_poly);
  Free_data(&_preg_268435482._c1_poly);
  Free_data(&_preg_268435482._c0_poly);
  Free_data(&_pgen_tmp_152_182._c1_poly);
  Free_data(&_pgen_tmp_152_182._c0_poly);
  Free_data(&_preg_268435481._c1_poly);
  Free_data(&_preg_268435481._c0_poly);
  Free_data(&_pgen_tmp_153_183._c1_poly);
  Free_data(&_pgen_tmp_153_183._c0_poly);
  Free_data(&_preg_268435478._c1_poly);
  Free_data(&_preg_268435478._c0_poly);
  CMPLR_TM_END(12, 0);
  Encode_float_mask(&_pgen_tmp_154_184, 0.5, mask_len_2, 1, Level(&input_ct0_0), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_155_185, &input_ct0_0, &_pgen_tmp_154_184);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_relu_tmp274_26, &_relu_tmp266_25, &_pgen_tmp_155_185);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_relu_tmp274_26._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_155_185._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_154_184._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_155_185._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_154_184._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp274_26._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp266_25._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_155_185._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp274_26._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp266_25._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_155_185._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_154_184._poly);
  Free_data(&_pgen_tmp_155_185._c1_poly);
  Free_data(&_pgen_tmp_155_185._c0_poly);
  Free_data(&_relu_tmp266_25._c1_poly);
  Free_data(&_relu_tmp266_25._c0_poly);
  CMPLR_TM_END(12, 0);
  Copy_ciph(&_preg_268435456, &_relu_tmp274_26);
  Free_data(&_relu_tmp274_26._c1_poly);
  Free_data(&_relu_tmp274_26._c0_poly);
  return _preg_268435456;
  Free_data(&_preg_268435456._c1_poly);
  Free_data(&_preg_268435456._c0_poly);
}

CIPHERTEXT App_relu_1(CIPHERTEXT input_ct0_0, CIPHERTEXT input_ct1_1, uint32_t mask_len_2) {
  CIPHERTEXT _relu_tmp8_3;
  CIPHERTEXT _relu_tmp14_4;
  CIPHERTEXT _relu_tmp18_5;
  CIPHERTEXT _relu_tmp24_6;
  CIPHERTEXT _relu_tmp58_7;
  CIPHERTEXT _relu_tmp62_8;
  CIPHERTEXT _relu_tmp68_9;
  CIPHERTEXT _relu_tmp72_10;
  CIPHERTEXT _relu_tmp80_11;
  CIPHERTEXT _relu_tmp84_12;
  CIPHERTEXT _relu_tmp90_13;
  CIPHERTEXT _relu_tmp94_14;
  CIPHERTEXT _relu_tmp100_15;
  CIPHERTEXT _relu_tmp155_16;
  CIPHERTEXT _relu_tmp159_17;
  CIPHERTEXT _relu_tmp165_18;
  CIPHERTEXT _relu_tmp169_19;
  CIPHERTEXT _relu_tmp177_20;
  CIPHERTEXT _relu_tmp181_21;
  CIPHERTEXT _relu_tmp187_22;
  CIPHERTEXT _relu_tmp191_23;
  CIPHERTEXT _relu_tmp197_24;
  CIPHERTEXT _relu_tmp266_25;
  CIPHERTEXT _relu_tmp274_26;
  CIPHERTEXT3 _pgen_tmp_0_30;
  MODULUS* _pgen_modulus_31;
  uint32_t _pgen_num_q_32;
  uint32_t _pgen_rns_idx_33;
  POLYNOMIAL _pgen_tmp_1_34;
  POLYNOMIAL _pgen_tmp_2_35;
  CIPHERTEXT _pgen_tmp_3_36;
  PLAINTEXT _pgen_tmp_4_37;
  CIPHERTEXT3 _pgen_tmp_5_38;
  POLYNOMIAL _pgen_tmp_6_39;
  POLYNOMIAL _pgen_tmp_7_40;
  CIPHERTEXT3 _pgen_tmp_8_41;
  CIPHERTEXT _pgen_tmp_9_42;
  PLAINTEXT _pgen_tmp_10_43;
  CIPHERTEXT _pgen_tmp_11_44;
  CIPHERTEXT3 _pgen_tmp_12_45;
  CIPHERTEXT3 _pgen_tmp_13_46;
  CIPHERTEXT _pgen_tmp_14_47;
  PLAINTEXT _pgen_tmp_15_48;
  CIPHERTEXT _pgen_tmp_16_49;
  POLYNOMIAL _pgen_tmp_17_50;
  POLYNOMIAL _pgen_tmp_18_51;
  CIPHERTEXT3 _pgen_tmp_19_52;
  CIPHERTEXT _pgen_tmp_20_53;
  PLAINTEXT _pgen_tmp_21_54;
  CIPHERTEXT _pgen_tmp_22_55;
  CIPHERTEXT _pgen_tmp_23_56;
  POLYNOMIAL _pgen_tmp_24_57;
  POLYNOMIAL _pgen_tmp_25_58;
  CIPHERTEXT3 _pgen_tmp_26_59;
  CIPHERTEXT _pgen_tmp_27_60;
  PLAINTEXT _pgen_tmp_28_61;
  CIPHERTEXT _pgen_tmp_29_62;
  POLYNOMIAL _pgen_tmp_30_63;
  POLYNOMIAL _pgen_tmp_31_64;
  PLAINTEXT _pgen_tmp_32_65;
  CIPHERTEXT _pgen_tmp_33_66;
  CIPHERTEXT _pgen_tmp_34_67;
  CIPHERTEXT3 _pgen_tmp_35_68;
  POLYNOMIAL _pgen_tmp_36_69;
  POLYNOMIAL _pgen_tmp_37_70;
  CIPHERTEXT _pgen_tmp_38_71;
  PLAINTEXT _pgen_tmp_39_72;
  CIPHERTEXT3 _pgen_tmp_40_73;
  POLYNOMIAL _pgen_tmp_41_74;
  POLYNOMIAL _pgen_tmp_42_75;
  CIPHERTEXT _pgen_tmp_43_76;
  CIPHERTEXT _pgen_tmp_44_77;
  PLAINTEXT _pgen_tmp_45_78;
  CIPHERTEXT _pgen_tmp_46_79;
  CIPHERTEXT3 _pgen_tmp_47_80;
  POLYNOMIAL _pgen_tmp_48_81;
  POLYNOMIAL _pgen_tmp_49_82;
  CIPHERTEXT _pgen_tmp_50_83;
  PLAINTEXT _pgen_tmp_51_84;
  CIPHERTEXT3 _pgen_tmp_52_85;
  POLYNOMIAL _pgen_tmp_53_86;
  POLYNOMIAL _pgen_tmp_54_87;
  CIPHERTEXT _pgen_tmp_55_88;
  PLAINTEXT _pgen_tmp_56_89;
  CIPHERTEXT _pgen_tmp_57_90;
  CIPHERTEXT3 _pgen_tmp_58_91;
  CIPHERTEXT _pgen_tmp_59_92;
  CIPHERTEXT3 _pgen_tmp_60_93;
  CIPHERTEXT _pgen_tmp_61_94;
  CIPHERTEXT3 _pgen_tmp_62_95;
  CIPHERTEXT _pgen_tmp_63_96;
  PLAINTEXT _pgen_tmp_64_97;
  CIPHERTEXT _pgen_tmp_65_98;
  POLYNOMIAL _pgen_tmp_66_99;
  POLYNOMIAL _pgen_tmp_67_100;
  PLAINTEXT _pgen_tmp_68_101;
  CIPHERTEXT _pgen_tmp_69_102;
  CIPHERTEXT _pgen_tmp_70_103;
  POLYNOMIAL _pgen_tmp_71_104;
  POLYNOMIAL _pgen_tmp_72_105;
  PLAINTEXT _pgen_tmp_73_106;
  CIPHERTEXT _pgen_tmp_74_107;
  PLAINTEXT _pgen_tmp_75_108;
  CIPHERTEXT _pgen_tmp_76_109;
  CIPHERTEXT _pgen_tmp_77_110;
  CIPHERTEXT _pgen_tmp_78_111;
  POLYNOMIAL _pgen_tmp_79_112;
  POLYNOMIAL _pgen_tmp_80_113;
  CIPHERTEXT3 _pgen_tmp_81_114;
  CIPHERTEXT _pgen_tmp_82_115;
  PLAINTEXT _pgen_tmp_83_116;
  CIPHERTEXT _pgen_tmp_84_117;
  PLAINTEXT _pgen_tmp_85_118;
  CIPHERTEXT _pgen_tmp_86_119;
  CIPHERTEXT _pgen_tmp_87_120;
  POLYNOMIAL _pgen_tmp_88_121;
  POLYNOMIAL _pgen_tmp_89_122;
  PLAINTEXT _pgen_tmp_90_123;
  CIPHERTEXT _pgen_tmp_91_124;
  PLAINTEXT _pgen_tmp_92_125;
  CIPHERTEXT _pgen_tmp_93_126;
  CIPHERTEXT _pgen_tmp_94_127;
  CIPHERTEXT _pgen_tmp_95_128;
  CIPHERTEXT3 _pgen_tmp_96_129;
  POLYNOMIAL _pgen_tmp_97_130;
  POLYNOMIAL _pgen_tmp_98_131;
  CIPHERTEXT3 _pgen_tmp_99_132;
  CIPHERTEXT _pgen_tmp_100_133;
  PLAINTEXT _pgen_tmp_101_134;
  CIPHERTEXT3 _pgen_tmp_102_135;
  POLYNOMIAL _pgen_tmp_103_136;
  POLYNOMIAL _pgen_tmp_104_137;
  CIPHERTEXT _pgen_tmp_105_138;
  CIPHERTEXT _pgen_tmp_106_139;
  PLAINTEXT _pgen_tmp_107_140;
  CIPHERTEXT _pgen_tmp_108_141;
  CIPHERTEXT3 _pgen_tmp_109_142;
  POLYNOMIAL _pgen_tmp_110_143;
  POLYNOMIAL _pgen_tmp_111_144;
  CIPHERTEXT _pgen_tmp_112_145;
  PLAINTEXT _pgen_tmp_113_146;
  CIPHERTEXT3 _pgen_tmp_114_147;
  POLYNOMIAL _pgen_tmp_115_148;
  POLYNOMIAL _pgen_tmp_116_149;
  CIPHERTEXT _pgen_tmp_117_150;
  PLAINTEXT _pgen_tmp_118_151;
  CIPHERTEXT _pgen_tmp_119_152;
  CIPHERTEXT3 _pgen_tmp_120_153;
  CIPHERTEXT _pgen_tmp_121_154;
  CIPHERTEXT3 _pgen_tmp_122_155;
  CIPHERTEXT3 _pgen_tmp_123_156;
  CIPHERTEXT _pgen_tmp_124_157;
  PLAINTEXT _pgen_tmp_125_158;
  CIPHERTEXT _pgen_tmp_126_159;
  POLYNOMIAL _pgen_tmp_127_160;
  POLYNOMIAL _pgen_tmp_128_161;
  CIPHERTEXT _pgen_tmp_129_162;
  POLYNOMIAL _pgen_tmp_130_163;
  POLYNOMIAL _pgen_tmp_131_164;
  CIPHERTEXT3 _pgen_tmp_132_165;
  CIPHERTEXT _pgen_tmp_133_166;
  PLAINTEXT _pgen_tmp_134_167;
  CIPHERTEXT _pgen_tmp_135_168;
  POLYNOMIAL _pgen_tmp_136_169;
  POLYNOMIAL _pgen_tmp_137_170;
  CIPHERTEXT3 _pgen_tmp_138_171;
  CIPHERTEXT _pgen_tmp_139_172;
  PLAINTEXT _pgen_tmp_140_173;
  CIPHERTEXT _pgen_tmp_141_174;
  POLYNOMIAL _pgen_tmp_142_175;
  POLYNOMIAL _pgen_tmp_143_176;
  CIPHERTEXT _pgen_tmp_144_177;
  CIPHERTEXT _pgen_tmp_145_178;
  POLYNOMIAL _pgen_tmp_146_179;
  POLYNOMIAL _pgen_tmp_147_180;
  CIPHERTEXT3 _pgen_tmp_148_181;
  CIPHERTEXT _pgen_tmp_149_182;
  CIPHERTEXT3 _pgen_tmp_150_183;
  CIPHERTEXT _pgen_tmp_151_184;
  PLAINTEXT _pgen_tmp_152_185;
  CIPHERTEXT _pgen_tmp_153_186;
  POLYNOMIAL _pgen_tmp_154_187;
  POLYNOMIAL _pgen_tmp_155_188;
  CIPHERTEXT3 _pgen_tmp_156_189;
  CIPHERTEXT _pgen_tmp_157_190;
  PLAINTEXT _pgen_tmp_158_191;
  CIPHERTEXT _pgen_tmp_159_192;
  POLYNOMIAL _pgen_tmp_160_193;
  POLYNOMIAL _pgen_tmp_161_194;
  CIPHERTEXT _pgen_tmp_162_195;
  POLYNOMIAL _pgen_tmp_163_196;
  POLYNOMIAL _pgen_tmp_164_197;
  CIPHERTEXT3 _pgen_tmp_165_198;
  CIPHERTEXT _pgen_tmp_166_199;
  PLAINTEXT _pgen_tmp_167_200;
  CIPHERTEXT _pgen_tmp_168_201;
  POLYNOMIAL _pgen_tmp_169_202;
  POLYNOMIAL _pgen_tmp_170_203;
  CIPHERTEXT3 _pgen_tmp_171_204;
  CIPHERTEXT _pgen_tmp_172_205;
  PLAINTEXT _pgen_tmp_173_206;
  CIPHERTEXT _pgen_tmp_174_207;
  POLYNOMIAL _pgen_tmp_175_208;
  POLYNOMIAL _pgen_tmp_176_209;
  CIPHERTEXT _pgen_tmp_177_210;
  CIPHERTEXT _pgen_tmp_178_211;
  CIPHERTEXT _pgen_tmp_179_212;
  PLAINTEXT _pgen_tmp_180_213;
  CIPHERTEXT _pgen_tmp_181_214;
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
  CIPHERTEXT _preg_268435467;
  CIPHERTEXT _preg_268435468;
  CIPHERTEXT _preg_268435469;
  CIPHERTEXT _preg_268435470;
  CIPHERTEXT _preg_268435471;
  CIPHERTEXT _preg_268435472;
  CIPHERTEXT _preg_268435473;
  CIPHERTEXT _preg_268435474;
  CIPHERTEXT _preg_268435475;
  CIPHERTEXT _preg_268435476;
  CIPHERTEXT _preg_268435477;
  CIPHERTEXT _preg_268435478;
  CIPHERTEXT _preg_268435479;
  CIPHERTEXT _preg_268435480;
  CIPHERTEXT _preg_268435481;
  CIPHERTEXT _preg_268435482;
  uint32_t  degree = Degree();
  memset(&_relu_tmp8_3, 0, sizeof(_relu_tmp8_3));
  memset(&_relu_tmp14_4, 0, sizeof(_relu_tmp14_4));
  memset(&_relu_tmp18_5, 0, sizeof(_relu_tmp18_5));
  memset(&_relu_tmp24_6, 0, sizeof(_relu_tmp24_6));
  memset(&_relu_tmp58_7, 0, sizeof(_relu_tmp58_7));
  memset(&_relu_tmp62_8, 0, sizeof(_relu_tmp62_8));
  memset(&_relu_tmp68_9, 0, sizeof(_relu_tmp68_9));
  memset(&_relu_tmp72_10, 0, sizeof(_relu_tmp72_10));
  memset(&_relu_tmp80_11, 0, sizeof(_relu_tmp80_11));
  memset(&_relu_tmp84_12, 0, sizeof(_relu_tmp84_12));
  memset(&_relu_tmp90_13, 0, sizeof(_relu_tmp90_13));
  memset(&_relu_tmp94_14, 0, sizeof(_relu_tmp94_14));
  memset(&_relu_tmp100_15, 0, sizeof(_relu_tmp100_15));
  memset(&_relu_tmp155_16, 0, sizeof(_relu_tmp155_16));
  memset(&_relu_tmp159_17, 0, sizeof(_relu_tmp159_17));
  memset(&_relu_tmp165_18, 0, sizeof(_relu_tmp165_18));
  memset(&_relu_tmp169_19, 0, sizeof(_relu_tmp169_19));
  memset(&_relu_tmp177_20, 0, sizeof(_relu_tmp177_20));
  memset(&_relu_tmp181_21, 0, sizeof(_relu_tmp181_21));
  memset(&_relu_tmp187_22, 0, sizeof(_relu_tmp187_22));
  memset(&_relu_tmp191_23, 0, sizeof(_relu_tmp191_23));
  memset(&_relu_tmp197_24, 0, sizeof(_relu_tmp197_24));
  memset(&_relu_tmp266_25, 0, sizeof(_relu_tmp266_25));
  memset(&_relu_tmp274_26, 0, sizeof(_relu_tmp274_26));
  memset(&_pgen_tmp_0_30, 0, sizeof(_pgen_tmp_0_30));
  memset(&_pgen_tmp_1_34, 0, sizeof(_pgen_tmp_1_34));
  memset(&_pgen_tmp_2_35, 0, sizeof(_pgen_tmp_2_35));
  memset(&_pgen_tmp_3_36, 0, sizeof(_pgen_tmp_3_36));
  memset(&_pgen_tmp_4_37, 0, sizeof(_pgen_tmp_4_37));
  memset(&_pgen_tmp_5_38, 0, sizeof(_pgen_tmp_5_38));
  memset(&_pgen_tmp_6_39, 0, sizeof(_pgen_tmp_6_39));
  memset(&_pgen_tmp_7_40, 0, sizeof(_pgen_tmp_7_40));
  memset(&_pgen_tmp_8_41, 0, sizeof(_pgen_tmp_8_41));
  memset(&_pgen_tmp_9_42, 0, sizeof(_pgen_tmp_9_42));
  memset(&_pgen_tmp_10_43, 0, sizeof(_pgen_tmp_10_43));
  memset(&_pgen_tmp_11_44, 0, sizeof(_pgen_tmp_11_44));
  memset(&_pgen_tmp_12_45, 0, sizeof(_pgen_tmp_12_45));
  memset(&_pgen_tmp_13_46, 0, sizeof(_pgen_tmp_13_46));
  memset(&_pgen_tmp_14_47, 0, sizeof(_pgen_tmp_14_47));
  memset(&_pgen_tmp_15_48, 0, sizeof(_pgen_tmp_15_48));
  memset(&_pgen_tmp_16_49, 0, sizeof(_pgen_tmp_16_49));
  memset(&_pgen_tmp_17_50, 0, sizeof(_pgen_tmp_17_50));
  memset(&_pgen_tmp_18_51, 0, sizeof(_pgen_tmp_18_51));
  memset(&_pgen_tmp_19_52, 0, sizeof(_pgen_tmp_19_52));
  memset(&_pgen_tmp_20_53, 0, sizeof(_pgen_tmp_20_53));
  memset(&_pgen_tmp_21_54, 0, sizeof(_pgen_tmp_21_54));
  memset(&_pgen_tmp_22_55, 0, sizeof(_pgen_tmp_22_55));
  memset(&_pgen_tmp_23_56, 0, sizeof(_pgen_tmp_23_56));
  memset(&_pgen_tmp_24_57, 0, sizeof(_pgen_tmp_24_57));
  memset(&_pgen_tmp_25_58, 0, sizeof(_pgen_tmp_25_58));
  memset(&_pgen_tmp_26_59, 0, sizeof(_pgen_tmp_26_59));
  memset(&_pgen_tmp_27_60, 0, sizeof(_pgen_tmp_27_60));
  memset(&_pgen_tmp_28_61, 0, sizeof(_pgen_tmp_28_61));
  memset(&_pgen_tmp_29_62, 0, sizeof(_pgen_tmp_29_62));
  memset(&_pgen_tmp_30_63, 0, sizeof(_pgen_tmp_30_63));
  memset(&_pgen_tmp_31_64, 0, sizeof(_pgen_tmp_31_64));
  memset(&_pgen_tmp_32_65, 0, sizeof(_pgen_tmp_32_65));
  memset(&_pgen_tmp_33_66, 0, sizeof(_pgen_tmp_33_66));
  memset(&_pgen_tmp_34_67, 0, sizeof(_pgen_tmp_34_67));
  memset(&_pgen_tmp_35_68, 0, sizeof(_pgen_tmp_35_68));
  memset(&_pgen_tmp_36_69, 0, sizeof(_pgen_tmp_36_69));
  memset(&_pgen_tmp_37_70, 0, sizeof(_pgen_tmp_37_70));
  memset(&_pgen_tmp_38_71, 0, sizeof(_pgen_tmp_38_71));
  memset(&_pgen_tmp_39_72, 0, sizeof(_pgen_tmp_39_72));
  memset(&_pgen_tmp_40_73, 0, sizeof(_pgen_tmp_40_73));
  memset(&_pgen_tmp_41_74, 0, sizeof(_pgen_tmp_41_74));
  memset(&_pgen_tmp_42_75, 0, sizeof(_pgen_tmp_42_75));
  memset(&_pgen_tmp_43_76, 0, sizeof(_pgen_tmp_43_76));
  memset(&_pgen_tmp_44_77, 0, sizeof(_pgen_tmp_44_77));
  memset(&_pgen_tmp_45_78, 0, sizeof(_pgen_tmp_45_78));
  memset(&_pgen_tmp_46_79, 0, sizeof(_pgen_tmp_46_79));
  memset(&_pgen_tmp_47_80, 0, sizeof(_pgen_tmp_47_80));
  memset(&_pgen_tmp_48_81, 0, sizeof(_pgen_tmp_48_81));
  memset(&_pgen_tmp_49_82, 0, sizeof(_pgen_tmp_49_82));
  memset(&_pgen_tmp_50_83, 0, sizeof(_pgen_tmp_50_83));
  memset(&_pgen_tmp_51_84, 0, sizeof(_pgen_tmp_51_84));
  memset(&_pgen_tmp_52_85, 0, sizeof(_pgen_tmp_52_85));
  memset(&_pgen_tmp_53_86, 0, sizeof(_pgen_tmp_53_86));
  memset(&_pgen_tmp_54_87, 0, sizeof(_pgen_tmp_54_87));
  memset(&_pgen_tmp_55_88, 0, sizeof(_pgen_tmp_55_88));
  memset(&_pgen_tmp_56_89, 0, sizeof(_pgen_tmp_56_89));
  memset(&_pgen_tmp_57_90, 0, sizeof(_pgen_tmp_57_90));
  memset(&_pgen_tmp_58_91, 0, sizeof(_pgen_tmp_58_91));
  memset(&_pgen_tmp_59_92, 0, sizeof(_pgen_tmp_59_92));
  memset(&_pgen_tmp_60_93, 0, sizeof(_pgen_tmp_60_93));
  memset(&_pgen_tmp_61_94, 0, sizeof(_pgen_tmp_61_94));
  memset(&_pgen_tmp_62_95, 0, sizeof(_pgen_tmp_62_95));
  memset(&_pgen_tmp_63_96, 0, sizeof(_pgen_tmp_63_96));
  memset(&_pgen_tmp_64_97, 0, sizeof(_pgen_tmp_64_97));
  memset(&_pgen_tmp_65_98, 0, sizeof(_pgen_tmp_65_98));
  memset(&_pgen_tmp_66_99, 0, sizeof(_pgen_tmp_66_99));
  memset(&_pgen_tmp_67_100, 0, sizeof(_pgen_tmp_67_100));
  memset(&_pgen_tmp_68_101, 0, sizeof(_pgen_tmp_68_101));
  memset(&_pgen_tmp_69_102, 0, sizeof(_pgen_tmp_69_102));
  memset(&_pgen_tmp_70_103, 0, sizeof(_pgen_tmp_70_103));
  memset(&_pgen_tmp_71_104, 0, sizeof(_pgen_tmp_71_104));
  memset(&_pgen_tmp_72_105, 0, sizeof(_pgen_tmp_72_105));
  memset(&_pgen_tmp_73_106, 0, sizeof(_pgen_tmp_73_106));
  memset(&_pgen_tmp_74_107, 0, sizeof(_pgen_tmp_74_107));
  memset(&_pgen_tmp_75_108, 0, sizeof(_pgen_tmp_75_108));
  memset(&_pgen_tmp_76_109, 0, sizeof(_pgen_tmp_76_109));
  memset(&_pgen_tmp_77_110, 0, sizeof(_pgen_tmp_77_110));
  memset(&_pgen_tmp_78_111, 0, sizeof(_pgen_tmp_78_111));
  memset(&_pgen_tmp_79_112, 0, sizeof(_pgen_tmp_79_112));
  memset(&_pgen_tmp_80_113, 0, sizeof(_pgen_tmp_80_113));
  memset(&_pgen_tmp_81_114, 0, sizeof(_pgen_tmp_81_114));
  memset(&_pgen_tmp_82_115, 0, sizeof(_pgen_tmp_82_115));
  memset(&_pgen_tmp_83_116, 0, sizeof(_pgen_tmp_83_116));
  memset(&_pgen_tmp_84_117, 0, sizeof(_pgen_tmp_84_117));
  memset(&_pgen_tmp_85_118, 0, sizeof(_pgen_tmp_85_118));
  memset(&_pgen_tmp_86_119, 0, sizeof(_pgen_tmp_86_119));
  memset(&_pgen_tmp_87_120, 0, sizeof(_pgen_tmp_87_120));
  memset(&_pgen_tmp_88_121, 0, sizeof(_pgen_tmp_88_121));
  memset(&_pgen_tmp_89_122, 0, sizeof(_pgen_tmp_89_122));
  memset(&_pgen_tmp_90_123, 0, sizeof(_pgen_tmp_90_123));
  memset(&_pgen_tmp_91_124, 0, sizeof(_pgen_tmp_91_124));
  memset(&_pgen_tmp_92_125, 0, sizeof(_pgen_tmp_92_125));
  memset(&_pgen_tmp_93_126, 0, sizeof(_pgen_tmp_93_126));
  memset(&_pgen_tmp_94_127, 0, sizeof(_pgen_tmp_94_127));
  memset(&_pgen_tmp_95_128, 0, sizeof(_pgen_tmp_95_128));
  memset(&_pgen_tmp_96_129, 0, sizeof(_pgen_tmp_96_129));
  memset(&_pgen_tmp_97_130, 0, sizeof(_pgen_tmp_97_130));
  memset(&_pgen_tmp_98_131, 0, sizeof(_pgen_tmp_98_131));
  memset(&_pgen_tmp_99_132, 0, sizeof(_pgen_tmp_99_132));
  memset(&_pgen_tmp_100_133, 0, sizeof(_pgen_tmp_100_133));
  memset(&_pgen_tmp_101_134, 0, sizeof(_pgen_tmp_101_134));
  memset(&_pgen_tmp_102_135, 0, sizeof(_pgen_tmp_102_135));
  memset(&_pgen_tmp_103_136, 0, sizeof(_pgen_tmp_103_136));
  memset(&_pgen_tmp_104_137, 0, sizeof(_pgen_tmp_104_137));
  memset(&_pgen_tmp_105_138, 0, sizeof(_pgen_tmp_105_138));
  memset(&_pgen_tmp_106_139, 0, sizeof(_pgen_tmp_106_139));
  memset(&_pgen_tmp_107_140, 0, sizeof(_pgen_tmp_107_140));
  memset(&_pgen_tmp_108_141, 0, sizeof(_pgen_tmp_108_141));
  memset(&_pgen_tmp_109_142, 0, sizeof(_pgen_tmp_109_142));
  memset(&_pgen_tmp_110_143, 0, sizeof(_pgen_tmp_110_143));
  memset(&_pgen_tmp_111_144, 0, sizeof(_pgen_tmp_111_144));
  memset(&_pgen_tmp_112_145, 0, sizeof(_pgen_tmp_112_145));
  memset(&_pgen_tmp_113_146, 0, sizeof(_pgen_tmp_113_146));
  memset(&_pgen_tmp_114_147, 0, sizeof(_pgen_tmp_114_147));
  memset(&_pgen_tmp_115_148, 0, sizeof(_pgen_tmp_115_148));
  memset(&_pgen_tmp_116_149, 0, sizeof(_pgen_tmp_116_149));
  memset(&_pgen_tmp_117_150, 0, sizeof(_pgen_tmp_117_150));
  memset(&_pgen_tmp_118_151, 0, sizeof(_pgen_tmp_118_151));
  memset(&_pgen_tmp_119_152, 0, sizeof(_pgen_tmp_119_152));
  memset(&_pgen_tmp_120_153, 0, sizeof(_pgen_tmp_120_153));
  memset(&_pgen_tmp_121_154, 0, sizeof(_pgen_tmp_121_154));
  memset(&_pgen_tmp_122_155, 0, sizeof(_pgen_tmp_122_155));
  memset(&_pgen_tmp_123_156, 0, sizeof(_pgen_tmp_123_156));
  memset(&_pgen_tmp_124_157, 0, sizeof(_pgen_tmp_124_157));
  memset(&_pgen_tmp_125_158, 0, sizeof(_pgen_tmp_125_158));
  memset(&_pgen_tmp_126_159, 0, sizeof(_pgen_tmp_126_159));
  memset(&_pgen_tmp_127_160, 0, sizeof(_pgen_tmp_127_160));
  memset(&_pgen_tmp_128_161, 0, sizeof(_pgen_tmp_128_161));
  memset(&_pgen_tmp_129_162, 0, sizeof(_pgen_tmp_129_162));
  memset(&_pgen_tmp_130_163, 0, sizeof(_pgen_tmp_130_163));
  memset(&_pgen_tmp_131_164, 0, sizeof(_pgen_tmp_131_164));
  memset(&_pgen_tmp_132_165, 0, sizeof(_pgen_tmp_132_165));
  memset(&_pgen_tmp_133_166, 0, sizeof(_pgen_tmp_133_166));
  memset(&_pgen_tmp_134_167, 0, sizeof(_pgen_tmp_134_167));
  memset(&_pgen_tmp_135_168, 0, sizeof(_pgen_tmp_135_168));
  memset(&_pgen_tmp_136_169, 0, sizeof(_pgen_tmp_136_169));
  memset(&_pgen_tmp_137_170, 0, sizeof(_pgen_tmp_137_170));
  memset(&_pgen_tmp_138_171, 0, sizeof(_pgen_tmp_138_171));
  memset(&_pgen_tmp_139_172, 0, sizeof(_pgen_tmp_139_172));
  memset(&_pgen_tmp_140_173, 0, sizeof(_pgen_tmp_140_173));
  memset(&_pgen_tmp_141_174, 0, sizeof(_pgen_tmp_141_174));
  memset(&_pgen_tmp_142_175, 0, sizeof(_pgen_tmp_142_175));
  memset(&_pgen_tmp_143_176, 0, sizeof(_pgen_tmp_143_176));
  memset(&_pgen_tmp_144_177, 0, sizeof(_pgen_tmp_144_177));
  memset(&_pgen_tmp_145_178, 0, sizeof(_pgen_tmp_145_178));
  memset(&_pgen_tmp_146_179, 0, sizeof(_pgen_tmp_146_179));
  memset(&_pgen_tmp_147_180, 0, sizeof(_pgen_tmp_147_180));
  memset(&_pgen_tmp_148_181, 0, sizeof(_pgen_tmp_148_181));
  memset(&_pgen_tmp_149_182, 0, sizeof(_pgen_tmp_149_182));
  memset(&_pgen_tmp_150_183, 0, sizeof(_pgen_tmp_150_183));
  memset(&_pgen_tmp_151_184, 0, sizeof(_pgen_tmp_151_184));
  memset(&_pgen_tmp_152_185, 0, sizeof(_pgen_tmp_152_185));
  memset(&_pgen_tmp_153_186, 0, sizeof(_pgen_tmp_153_186));
  memset(&_pgen_tmp_154_187, 0, sizeof(_pgen_tmp_154_187));
  memset(&_pgen_tmp_155_188, 0, sizeof(_pgen_tmp_155_188));
  memset(&_pgen_tmp_156_189, 0, sizeof(_pgen_tmp_156_189));
  memset(&_pgen_tmp_157_190, 0, sizeof(_pgen_tmp_157_190));
  memset(&_pgen_tmp_158_191, 0, sizeof(_pgen_tmp_158_191));
  memset(&_pgen_tmp_159_192, 0, sizeof(_pgen_tmp_159_192));
  memset(&_pgen_tmp_160_193, 0, sizeof(_pgen_tmp_160_193));
  memset(&_pgen_tmp_161_194, 0, sizeof(_pgen_tmp_161_194));
  memset(&_pgen_tmp_162_195, 0, sizeof(_pgen_tmp_162_195));
  memset(&_pgen_tmp_163_196, 0, sizeof(_pgen_tmp_163_196));
  memset(&_pgen_tmp_164_197, 0, sizeof(_pgen_tmp_164_197));
  memset(&_pgen_tmp_165_198, 0, sizeof(_pgen_tmp_165_198));
  memset(&_pgen_tmp_166_199, 0, sizeof(_pgen_tmp_166_199));
  memset(&_pgen_tmp_167_200, 0, sizeof(_pgen_tmp_167_200));
  memset(&_pgen_tmp_168_201, 0, sizeof(_pgen_tmp_168_201));
  memset(&_pgen_tmp_169_202, 0, sizeof(_pgen_tmp_169_202));
  memset(&_pgen_tmp_170_203, 0, sizeof(_pgen_tmp_170_203));
  memset(&_pgen_tmp_171_204, 0, sizeof(_pgen_tmp_171_204));
  memset(&_pgen_tmp_172_205, 0, sizeof(_pgen_tmp_172_205));
  memset(&_pgen_tmp_173_206, 0, sizeof(_pgen_tmp_173_206));
  memset(&_pgen_tmp_174_207, 0, sizeof(_pgen_tmp_174_207));
  memset(&_pgen_tmp_175_208, 0, sizeof(_pgen_tmp_175_208));
  memset(&_pgen_tmp_176_209, 0, sizeof(_pgen_tmp_176_209));
  memset(&_pgen_tmp_177_210, 0, sizeof(_pgen_tmp_177_210));
  memset(&_pgen_tmp_178_211, 0, sizeof(_pgen_tmp_178_211));
  memset(&_pgen_tmp_179_212, 0, sizeof(_pgen_tmp_179_212));
  memset(&_pgen_tmp_180_213, 0, sizeof(_pgen_tmp_180_213));
  memset(&_pgen_tmp_181_214, 0, sizeof(_pgen_tmp_181_214));
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
  memset(&_preg_268435467, 0, sizeof(_preg_268435467));
  memset(&_preg_268435468, 0, sizeof(_preg_268435468));
  memset(&_preg_268435469, 0, sizeof(_preg_268435469));
  memset(&_preg_268435470, 0, sizeof(_preg_268435470));
  memset(&_preg_268435471, 0, sizeof(_preg_268435471));
  memset(&_preg_268435472, 0, sizeof(_preg_268435472));
  memset(&_preg_268435473, 0, sizeof(_preg_268435473));
  memset(&_preg_268435474, 0, sizeof(_preg_268435474));
  memset(&_preg_268435475, 0, sizeof(_preg_268435475));
  memset(&_preg_268435476, 0, sizeof(_preg_268435476));
  memset(&_preg_268435477, 0, sizeof(_preg_268435477));
  memset(&_preg_268435478, 0, sizeof(_preg_268435478));
  memset(&_preg_268435479, 0, sizeof(_preg_268435479));
  memset(&_preg_268435480, 0, sizeof(_preg_268435480));
  memset(&_preg_268435481, 0, sizeof(_preg_268435481));
  memset(&_preg_268435482, 0, sizeof(_preg_268435482));
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_0_30, &input_ct1_1, &input_ct1_1);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_0_30._c0_poly);
  Init_poly_by_size(&_pgen_tmp_1_34, 1, 0);
  Init_poly_by_size(&_pgen_tmp_2_35, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_1_34, 0, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_2_35, 0, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_0_30._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_0_30._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_1_34, 0, degree), Coeffs(&_pgen_tmp_2_35, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_0_30._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_1_34);
  Free_data(&_pgen_tmp_1_34);
  Free_data(&_pgen_tmp_2_35);
  Free_data(&_pgen_tmp_2_35);
  CMPLR_TM_END(16, 0);
  _preg_268435456 = Relinearize(_pgen_tmp_0_30);
  Free_data(&_pgen_tmp_0_30._c2_poly);
  Free_data(&_pgen_tmp_0_30._c1_poly);
  Free_data(&_pgen_tmp_0_30._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_relu_tmp8_3, &_preg_268435456);
  Rescale(&_relu_tmp8_3._c0_poly, &_preg_268435456._c0_poly);
  Rescale(&_relu_tmp8_3._c1_poly, &_preg_268435456._c1_poly);
  Free_data(&_preg_268435456._c1_poly);
  Free_data(&_preg_268435456._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_3_36, &_relu_tmp8_3, &_relu_tmp8_3);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_4_37, &_cst_132, 1, Sc_degree(&_pgen_tmp_3_36), Level(&_pgen_tmp_3_36), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp14_4, &_pgen_tmp_3_36, &_pgen_tmp_4_37);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_relu_tmp14_4._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_3_36._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp8_3._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp8_3._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_3_36._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp8_3._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp8_3._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_3_36._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_4_37._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Set_coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_33, degree, Coeffs(&_pgen_tmp_3_36._c1_poly, _pgen_rns_idx_33, degree));
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_relu_tmp8_3._c1_poly);
  Free_data(&_relu_tmp8_3._c0_poly);
  Free_data(&_pgen_tmp_4_37._poly);
  Free_data(&_pgen_tmp_3_36._c1_poly);
  Free_data(&_pgen_tmp_3_36._c0_poly);
  CMPLR_TM_END(13, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_5_38, &_relu_tmp14_4, &_relu_tmp14_4);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_5_38._c0_poly);
  Init_poly_by_size(&_pgen_tmp_6_39, 1, 0);
  Init_poly_by_size(&_pgen_tmp_7_40, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_6_39, 0, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_7_40, 0, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_5_38._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_5_38._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_6_39, 0, degree), Coeffs(&_pgen_tmp_7_40, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_5_38._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_6_39);
  Free_data(&_pgen_tmp_6_39);
  Free_data(&_pgen_tmp_7_40);
  Free_data(&_pgen_tmp_7_40);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_8_41, &_pgen_tmp_5_38);
  Rescale(&_pgen_tmp_8_41._c0_poly, &_pgen_tmp_5_38._c0_poly);
  Rescale(&_pgen_tmp_8_41._c1_poly, &_pgen_tmp_5_38._c1_poly);
  Rescale(&_pgen_tmp_8_41._c2_poly, &_pgen_tmp_5_38._c2_poly);
  Free_data(&_pgen_tmp_5_38._c2_poly);
  Free_data(&_pgen_tmp_5_38._c1_poly);
  Free_data(&_pgen_tmp_5_38._c0_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435457 = Relinearize(_pgen_tmp_8_41);
  Free_data(&_pgen_tmp_8_41._c2_poly);
  Free_data(&_pgen_tmp_8_41._c1_poly);
  Free_data(&_pgen_tmp_8_41._c0_poly);
  Copy_ciph(&_relu_tmp18_5, &_preg_268435457);
  Free_data(&_preg_268435457._c1_poly);
  Free_data(&_preg_268435457._c0_poly);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_9_42, &_relu_tmp18_5, &_relu_tmp18_5);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_10_43, &_cst_133, 1, Sc_degree(&_pgen_tmp_9_42), Level(&_pgen_tmp_9_42), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp24_6, &_pgen_tmp_9_42, &_pgen_tmp_10_43);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_relu_tmp24_6._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_9_42._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp18_5._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp18_5._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_9_42._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp18_5._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp18_5._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_relu_tmp24_6._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_9_42._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_10_43._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Set_coeffs(&_relu_tmp24_6._c1_poly, _pgen_rns_idx_33, degree, Coeffs(&_pgen_tmp_9_42._c1_poly, _pgen_rns_idx_33, degree));
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_relu_tmp18_5._c1_poly);
  Free_data(&_relu_tmp18_5._c0_poly);
  Free_data(&_pgen_tmp_10_43._poly);
  Free_data(&_pgen_tmp_9_42._c1_poly);
  Free_data(&_pgen_tmp_9_42._c0_poly);
  CMPLR_TM_END(13, 0);
  Encode_float_mask(&_pgen_tmp_15_48, -3.809199432496630510769, mask_len_2, 1, Level(&input_ct1_1), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_14_47, &input_ct1_1, &_pgen_tmp_15_48);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_14_47._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_14_47._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_15_48._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_14_47._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_15_48._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_15_48._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_16_49, &_pgen_tmp_14_47);
  Rescale(&_pgen_tmp_16_49._c0_poly, &_pgen_tmp_14_47._c0_poly);
  Rescale(&_pgen_tmp_16_49._c1_poly, &_pgen_tmp_14_47._c1_poly);
  Free_data(&_pgen_tmp_14_47._c1_poly);
  Free_data(&_pgen_tmp_14_47._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_13_46, &_relu_tmp14_4, &_pgen_tmp_16_49);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_13_46._c0_poly);
  Init_poly_by_size(&_pgen_tmp_17_50, 1, 0);
  Init_poly_by_size(&_pgen_tmp_18_51, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_17_50, 0, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_16_49._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_18_51, 0, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_16_49._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_13_46._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_16_49._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_13_46._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_17_50, 0, degree), Coeffs(&_pgen_tmp_18_51, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_13_46._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_16_49._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_16_49._c1_poly);
  Free_data(&_pgen_tmp_16_49._c0_poly);
  Free_data(&_pgen_tmp_17_50);
  Free_data(&_pgen_tmp_17_50);
  Free_data(&_pgen_tmp_18_51);
  Free_data(&_pgen_tmp_18_51);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_19_52, &_pgen_tmp_13_46);
  Rescale(&_pgen_tmp_19_52._c0_poly, &_pgen_tmp_13_46._c0_poly);
  Rescale(&_pgen_tmp_19_52._c1_poly, &_pgen_tmp_13_46._c1_poly);
  Rescale(&_pgen_tmp_19_52._c2_poly, &_pgen_tmp_13_46._c2_poly);
  Free_data(&_pgen_tmp_13_46._c2_poly);
  Free_data(&_pgen_tmp_13_46._c1_poly);
  Free_data(&_pgen_tmp_13_46._c0_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435458 = Relinearize(_pgen_tmp_19_52);
  Free_data(&_pgen_tmp_19_52._c2_poly);
  Free_data(&_pgen_tmp_19_52._c1_poly);
  Free_data(&_pgen_tmp_19_52._c0_poly);
  Encode_float_mask(&_pgen_tmp_21_54, 2.460940868770910228136, mask_len_2, 1, Level(&input_ct1_1), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_20_53, &input_ct1_1, &_pgen_tmp_21_54);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_20_53._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_20_53._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_21_54._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_20_53._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_21_54._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_21_54._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_22_55, &_pgen_tmp_20_53);
  Rescale(&_pgen_tmp_22_55._c0_poly, &_pgen_tmp_20_53._c0_poly);
  Rescale(&_pgen_tmp_22_55._c1_poly, &_pgen_tmp_20_53._c1_poly);
  Free_data(&_pgen_tmp_20_53._c1_poly);
  Free_data(&_pgen_tmp_20_53._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_23_56, &_preg_268435458, &_pgen_tmp_22_55);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_12_45, &_relu_tmp24_6, &_pgen_tmp_23_56);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_12_45._c0_poly);
  Init_poly_by_size(&_pgen_tmp_24_57, 1, 0);
  Init_poly_by_size(&_pgen_tmp_25_58, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_23_56._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435458._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_22_55._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_23_56._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435458._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_22_55._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_24_57, 0, degree), Coeffs(&_relu_tmp24_6._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_23_56._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_25_58, 0, degree), Coeffs(&_relu_tmp24_6._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_23_56._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_12_45._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp24_6._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_23_56._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_12_45._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_24_57, 0, degree), Coeffs(&_pgen_tmp_25_58, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_12_45._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp24_6._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_23_56._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_22_55._c1_poly);
  Free_data(&_pgen_tmp_22_55._c0_poly);
  Free_data(&_preg_268435458._c1_poly);
  Free_data(&_preg_268435458._c0_poly);
  Free_data(&_pgen_tmp_23_56._c1_poly);
  Free_data(&_pgen_tmp_23_56._c0_poly);
  Free_data(&_relu_tmp24_6._c1_poly);
  Free_data(&_relu_tmp24_6._c0_poly);
  Free_data(&_pgen_tmp_24_57);
  Free_data(&_pgen_tmp_24_57);
  Free_data(&_pgen_tmp_25_58);
  Free_data(&_pgen_tmp_25_58);
  CMPLR_TM_END(16, 0);
  _preg_268435459 = Relinearize(_pgen_tmp_12_45);
  Free_data(&_pgen_tmp_12_45._c2_poly);
  Free_data(&_pgen_tmp_12_45._c1_poly);
  Free_data(&_pgen_tmp_12_45._c0_poly);
  Encode_float_mask(&_pgen_tmp_28_61, -1.430304794543664215212, mask_len_2, 1, Level(&input_ct1_1), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_27_60, &input_ct1_1, &_pgen_tmp_28_61);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_27_60._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_27_60._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_28_61._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_27_60._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_28_61._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_28_61._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_29_62, &_pgen_tmp_27_60);
  Rescale(&_pgen_tmp_29_62._c0_poly, &_pgen_tmp_27_60._c0_poly);
  Rescale(&_pgen_tmp_29_62._c1_poly, &_pgen_tmp_27_60._c1_poly);
  Free_data(&_pgen_tmp_27_60._c1_poly);
  Free_data(&_pgen_tmp_27_60._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_26_59, &_relu_tmp14_4, &_pgen_tmp_29_62);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_26_59._c0_poly);
  Init_poly_by_size(&_pgen_tmp_30_63, 1, 0);
  Init_poly_by_size(&_pgen_tmp_31_64, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_30_63, 0, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_29_62._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_31_64, 0, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_29_62._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_26_59._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_29_62._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_26_59._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_30_63, 0, degree), Coeffs(&_pgen_tmp_31_64, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_26_59._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_29_62._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_29_62._c1_poly);
  Free_data(&_pgen_tmp_29_62._c0_poly);
  Free_data(&_relu_tmp14_4._c1_poly);
  Free_data(&_relu_tmp14_4._c0_poly);
  Free_data(&_pgen_tmp_30_63);
  Free_data(&_pgen_tmp_30_63);
  Free_data(&_pgen_tmp_31_64);
  Free_data(&_pgen_tmp_31_64);
  CMPLR_TM_END(16, 0);
  _preg_268435460 = Relinearize(_pgen_tmp_26_59);
  Free_data(&_pgen_tmp_26_59._c2_poly);
  Free_data(&_pgen_tmp_26_59._c1_poly);
  Free_data(&_pgen_tmp_26_59._c0_poly);
  Encode_float_mask(&_pgen_tmp_32_65, 2.944661935353764636858, mask_len_2, 1, Level(&input_ct1_1), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_33_66, &input_ct1_1, &_pgen_tmp_32_65);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_34_67, &_preg_268435460, &_pgen_tmp_33_66);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_11_44, &_preg_268435459, &_pgen_tmp_34_67);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_11_44._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_33_66._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_32_65._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_33_66._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_32_65._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_34_67._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435460._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_33_66._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_34_67._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435460._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_33_66._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_11_44._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435459._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_34_67._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_11_44._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435459._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_34_67._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_32_65._poly);
  Free_data(&_pgen_tmp_33_66._c1_poly);
  Free_data(&_pgen_tmp_33_66._c0_poly);
  Free_data(&_preg_268435460._c1_poly);
  Free_data(&_preg_268435460._c0_poly);
  Free_data(&_pgen_tmp_34_67._c1_poly);
  Free_data(&_pgen_tmp_34_67._c0_poly);
  Free_data(&_preg_268435459._c1_poly);
  Free_data(&_preg_268435459._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_relu_tmp58_7, &_pgen_tmp_11_44);
  Rescale(&_relu_tmp58_7._c0_poly, &_pgen_tmp_11_44._c0_poly);
  Rescale(&_relu_tmp58_7._c1_poly, &_pgen_tmp_11_44._c1_poly);
  Free_data(&_pgen_tmp_11_44._c1_poly);
  Free_data(&_pgen_tmp_11_44._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_35_68, &_relu_tmp58_7, &_relu_tmp58_7);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_35_68._c0_poly);
  Init_poly_by_size(&_pgen_tmp_36_69, 1, 0);
  Init_poly_by_size(&_pgen_tmp_37_70, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_36_69, 0, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_37_70, 0, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_35_68._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_35_68._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_36_69, 0, degree), Coeffs(&_pgen_tmp_37_70, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_35_68._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_36_69);
  Free_data(&_pgen_tmp_36_69);
  Free_data(&_pgen_tmp_37_70);
  Free_data(&_pgen_tmp_37_70);
  CMPLR_TM_END(16, 0);
  _preg_268435461 = Relinearize(_pgen_tmp_35_68);
  Free_data(&_pgen_tmp_35_68._c2_poly);
  Free_data(&_pgen_tmp_35_68._c1_poly);
  Free_data(&_pgen_tmp_35_68._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_relu_tmp62_8, &_preg_268435461);
  Rescale(&_relu_tmp62_8._c0_poly, &_preg_268435461._c0_poly);
  Rescale(&_relu_tmp62_8._c1_poly, &_preg_268435461._c1_poly);
  Free_data(&_preg_268435461._c1_poly);
  Free_data(&_preg_268435461._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_38_71, &_relu_tmp62_8, &_relu_tmp62_8);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_39_72, &_cst_142, 1, Sc_degree(&_pgen_tmp_38_71), Level(&_pgen_tmp_38_71), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp68_9, &_pgen_tmp_38_71, &_pgen_tmp_39_72);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_relu_tmp68_9._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_38_71._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp62_8._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp62_8._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_38_71._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp62_8._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp62_8._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_38_71._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_39_72._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Set_coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_33, degree, Coeffs(&_pgen_tmp_38_71._c1_poly, _pgen_rns_idx_33, degree));
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_relu_tmp62_8._c1_poly);
  Free_data(&_relu_tmp62_8._c0_poly);
  Free_data(&_pgen_tmp_39_72._poly);
  Free_data(&_pgen_tmp_38_71._c1_poly);
  Free_data(&_pgen_tmp_38_71._c0_poly);
  CMPLR_TM_END(13, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_40_73, &_relu_tmp58_7, &_relu_tmp68_9);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_40_73._c0_poly);
  Init_poly_by_size(&_pgen_tmp_41_74, 1, 0);
  Init_poly_by_size(&_pgen_tmp_42_75, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_41_74, 0, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_42_75, 0, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_40_73._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_40_73._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_41_74, 0, degree), Coeffs(&_pgen_tmp_42_75, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_40_73._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_41_74);
  Free_data(&_pgen_tmp_41_74);
  Free_data(&_pgen_tmp_42_75);
  Free_data(&_pgen_tmp_42_75);
  CMPLR_TM_END(16, 0);
  _preg_268435462 = Relinearize(_pgen_tmp_40_73);
  Free_data(&_pgen_tmp_40_73._c2_poly);
  Free_data(&_pgen_tmp_40_73._c1_poly);
  Free_data(&_pgen_tmp_40_73._c0_poly);
  Copy_ciph(&_relu_tmp72_10, &_preg_268435462);
  Free_data(&_preg_268435462._c1_poly);
  Free_data(&_preg_268435462._c0_poly);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_44_77, &_relu_tmp72_10, &_relu_tmp72_10);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_45_78, &_cst_143, 1, 1, Level(&_relu_tmp58_7), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_46_79, &_relu_tmp58_7, &_pgen_tmp_45_78);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_43_76, &_pgen_tmp_44_77, &_pgen_tmp_46_79);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_43_76._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_44_77._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp72_10._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp72_10._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_44_77._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp72_10._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp72_10._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_46_79._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_45_78._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_46_79._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_45_78._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_43_76._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_44_77._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_46_79._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_43_76._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_44_77._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_46_79._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_relu_tmp72_10._c1_poly);
  Free_data(&_relu_tmp72_10._c0_poly);
  Free_data(&_pgen_tmp_45_78._poly);
  Free_data(&_pgen_tmp_46_79._c1_poly);
  Free_data(&_pgen_tmp_46_79._c0_poly);
  Free_data(&_pgen_tmp_44_77._c1_poly);
  Free_data(&_pgen_tmp_44_77._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_relu_tmp80_11, &_pgen_tmp_43_76);
  Rescale(&_relu_tmp80_11._c0_poly, &_pgen_tmp_43_76._c0_poly);
  Rescale(&_relu_tmp80_11._c1_poly, &_pgen_tmp_43_76._c1_poly);
  Free_data(&_pgen_tmp_43_76._c1_poly);
  Free_data(&_pgen_tmp_43_76._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_47_80, &_relu_tmp68_9, &_relu_tmp68_9);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_47_80._c0_poly);
  Init_poly_by_size(&_pgen_tmp_48_81, 1, 0);
  Init_poly_by_size(&_pgen_tmp_49_82, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_48_81, 0, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_49_82, 0, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_47_80._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_47_80._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_48_81, 0, degree), Coeffs(&_pgen_tmp_49_82, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_47_80._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_48_81);
  Free_data(&_pgen_tmp_48_81);
  Free_data(&_pgen_tmp_49_82);
  Free_data(&_pgen_tmp_49_82);
  CMPLR_TM_END(16, 0);
  _preg_268435463 = Relinearize(_pgen_tmp_47_80);
  Free_data(&_pgen_tmp_47_80._c2_poly);
  Free_data(&_pgen_tmp_47_80._c1_poly);
  Free_data(&_pgen_tmp_47_80._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_relu_tmp84_12, &_preg_268435463);
  Rescale(&_relu_tmp84_12._c0_poly, &_preg_268435463._c0_poly);
  Rescale(&_relu_tmp84_12._c1_poly, &_preg_268435463._c1_poly);
  Free_data(&_preg_268435463._c1_poly);
  Free_data(&_preg_268435463._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_50_83, &_relu_tmp84_12, &_relu_tmp84_12);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_51_84, &_cst_144, 1, Sc_degree(&_pgen_tmp_50_83), Level(&_pgen_tmp_50_83), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp90_13, &_pgen_tmp_50_83, &_pgen_tmp_51_84);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_relu_tmp90_13._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_50_83._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp84_12._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp84_12._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_50_83._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp84_12._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp84_12._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_50_83._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_51_84._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Set_coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_33, degree, Coeffs(&_pgen_tmp_50_83._c1_poly, _pgen_rns_idx_33, degree));
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_relu_tmp84_12._c1_poly);
  Free_data(&_relu_tmp84_12._c0_poly);
  Free_data(&_pgen_tmp_51_84._poly);
  Free_data(&_pgen_tmp_50_83._c1_poly);
  Free_data(&_pgen_tmp_50_83._c0_poly);
  CMPLR_TM_END(13, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_52_85, &_relu_tmp90_13, &_relu_tmp90_13);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_52_85._c0_poly);
  Init_poly_by_size(&_pgen_tmp_53_86, 1, 0);
  Init_poly_by_size(&_pgen_tmp_54_87, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_53_86, 0, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_54_87, 0, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_52_85._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_52_85._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_53_86, 0, degree), Coeffs(&_pgen_tmp_54_87, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_52_85._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_53_86);
  Free_data(&_pgen_tmp_53_86);
  Free_data(&_pgen_tmp_54_87);
  Free_data(&_pgen_tmp_54_87);
  CMPLR_TM_END(16, 0);
  _preg_268435464 = Relinearize(_pgen_tmp_52_85);
  Free_data(&_pgen_tmp_52_85._c2_poly);
  Free_data(&_pgen_tmp_52_85._c1_poly);
  Free_data(&_pgen_tmp_52_85._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_relu_tmp94_14, &_preg_268435464);
  Rescale(&_relu_tmp94_14._c0_poly, &_preg_268435464._c0_poly);
  Rescale(&_relu_tmp94_14._c1_poly, &_preg_268435464._c1_poly);
  Free_data(&_preg_268435464._c1_poly);
  Free_data(&_preg_268435464._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_55_88, &_relu_tmp94_14, &_relu_tmp94_14);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_56_89, &_cst_145, 1, Sc_degree(&_pgen_tmp_55_88), Level(&_pgen_tmp_55_88), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp100_15, &_pgen_tmp_55_88, &_pgen_tmp_56_89);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_relu_tmp100_15._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_55_88._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp94_14._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp94_14._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_55_88._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp94_14._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp94_14._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_relu_tmp100_15._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_55_88._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_56_89._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Set_coeffs(&_relu_tmp100_15._c1_poly, _pgen_rns_idx_33, degree, Coeffs(&_pgen_tmp_55_88._c1_poly, _pgen_rns_idx_33, degree));
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_relu_tmp94_14._c1_poly);
  Free_data(&_relu_tmp94_14._c0_poly);
  Free_data(&_pgen_tmp_56_89._poly);
  Free_data(&_pgen_tmp_55_88._c1_poly);
  Free_data(&_pgen_tmp_55_88._c0_poly);
  CMPLR_TM_END(13, 0);
  Encode_float_mask(&_pgen_tmp_64_97, -0.0001600280537250875771867, mask_len_2, 1, Level(&_relu_tmp58_7), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_63_96, &_relu_tmp58_7, &_pgen_tmp_64_97);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_63_96._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_63_96._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_64_97._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_63_96._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_64_97._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_64_97._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_65_98, &_pgen_tmp_63_96);
  Rescale(&_pgen_tmp_65_98._c0_poly, &_pgen_tmp_63_96._c0_poly);
  Rescale(&_pgen_tmp_65_98._c1_poly, &_pgen_tmp_63_96._c1_poly);
  Free_data(&_pgen_tmp_63_96._c1_poly);
  Free_data(&_pgen_tmp_63_96._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_62_95, &_relu_tmp68_9, &_pgen_tmp_65_98);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_62_95._c0_poly);
  Init_poly_by_size(&_pgen_tmp_66_99, 1, 0);
  Init_poly_by_size(&_pgen_tmp_67_100, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_66_99, 0, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_65_98._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_67_100, 0, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_65_98._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_62_95._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_65_98._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_62_95._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_66_99, 0, degree), Coeffs(&_pgen_tmp_67_100, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_62_95._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_65_98._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_65_98._c1_poly);
  Free_data(&_pgen_tmp_65_98._c0_poly);
  Free_data(&_relu_tmp68_9._c1_poly);
  Free_data(&_relu_tmp68_9._c0_poly);
  Free_data(&_pgen_tmp_66_99);
  Free_data(&_pgen_tmp_66_99);
  Free_data(&_pgen_tmp_67_100);
  Free_data(&_pgen_tmp_67_100);
  CMPLR_TM_END(16, 0);
  _preg_268435465 = Relinearize(_pgen_tmp_62_95);
  Free_data(&_pgen_tmp_62_95._c2_poly);
  Free_data(&_pgen_tmp_62_95._c1_poly);
  Free_data(&_pgen_tmp_62_95._c0_poly);
  Encode_float_mask(&_pgen_tmp_68_101, 0.003032478474127557870749, mask_len_2, 1, Level(&_relu_tmp58_7), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_69_102, &_relu_tmp58_7, &_pgen_tmp_68_101);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_61_94, &_preg_268435465, &_pgen_tmp_69_102);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_61_94._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_69_102._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_68_101._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_69_102._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_68_101._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_61_94._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435465._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_69_102._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_61_94._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435465._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_69_102._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_68_101._poly);
  Free_data(&_pgen_tmp_69_102._c1_poly);
  Free_data(&_pgen_tmp_69_102._c0_poly);
  Free_data(&_preg_268435465._c1_poly);
  Free_data(&_preg_268435465._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_70_103, &_pgen_tmp_61_94);
  Rescale(&_pgen_tmp_70_103._c0_poly, &_pgen_tmp_61_94._c0_poly);
  Rescale(&_pgen_tmp_70_103._c1_poly, &_pgen_tmp_61_94._c1_poly);
  Free_data(&_pgen_tmp_61_94._c1_poly);
  Free_data(&_pgen_tmp_61_94._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_60_93, &_relu_tmp90_13, &_pgen_tmp_70_103);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_60_93._c0_poly);
  Init_poly_by_size(&_pgen_tmp_71_104, 1, 0);
  Init_poly_by_size(&_pgen_tmp_72_105, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_71_104, 0, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_70_103._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_72_105, 0, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_70_103._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_60_93._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_70_103._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_60_93._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_71_104, 0, degree), Coeffs(&_pgen_tmp_72_105, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_60_93._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_70_103._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_70_103._c1_poly);
  Free_data(&_pgen_tmp_70_103._c0_poly);
  Free_data(&_pgen_tmp_71_104);
  Free_data(&_pgen_tmp_71_104);
  Free_data(&_pgen_tmp_72_105);
  Free_data(&_pgen_tmp_72_105);
  CMPLR_TM_END(16, 0);
  _preg_268435466 = Relinearize(_pgen_tmp_60_93);
  Free_data(&_pgen_tmp_60_93._c2_poly);
  Free_data(&_pgen_tmp_60_93._c1_poly);
  Free_data(&_pgen_tmp_60_93._c0_poly);
  Encode_double(&_pgen_tmp_73_106, &_cst_152, 1, 1, Level(&_relu_tmp80_11), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_74_107, &_relu_tmp80_11, &_pgen_tmp_73_106);
  CMPLR_TM_END(17, 0);
  Encode_float_mask(&_pgen_tmp_75_108, 0.1365828361194337536855, mask_len_2, 1, Level(&_relu_tmp58_7), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_76_109, &_relu_tmp58_7, &_pgen_tmp_75_108);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_77_110, &_pgen_tmp_74_107, &_pgen_tmp_76_109);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_59_92, &_preg_268435466, &_pgen_tmp_77_110);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_59_92._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_74_107._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp80_11._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_73_106._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_74_107._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp80_11._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_73_106._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_76_109._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_75_108._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_76_109._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_75_108._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_77_110._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_74_107._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_76_109._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_77_110._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_74_107._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_76_109._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_59_92._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435466._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_77_110._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_59_92._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435466._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_77_110._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_73_106._poly);
  Free_data(&_pgen_tmp_75_108._poly);
  Free_data(&_pgen_tmp_76_109._c1_poly);
  Free_data(&_pgen_tmp_76_109._c0_poly);
  Free_data(&_pgen_tmp_74_107._c1_poly);
  Free_data(&_pgen_tmp_74_107._c0_poly);
  Free_data(&_pgen_tmp_77_110._c1_poly);
  Free_data(&_pgen_tmp_77_110._c0_poly);
  Free_data(&_preg_268435466._c1_poly);
  Free_data(&_preg_268435466._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_78_111, &_pgen_tmp_59_92);
  Rescale(&_pgen_tmp_78_111._c0_poly, &_pgen_tmp_59_92._c0_poly);
  Rescale(&_pgen_tmp_78_111._c1_poly, &_pgen_tmp_59_92._c1_poly);
  Free_data(&_pgen_tmp_59_92._c1_poly);
  Free_data(&_pgen_tmp_59_92._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_58_91, &_relu_tmp100_15, &_pgen_tmp_78_111);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_58_91._c0_poly);
  Init_poly_by_size(&_pgen_tmp_79_112, 1, 0);
  Init_poly_by_size(&_pgen_tmp_80_113, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_79_112, 0, degree), Coeffs(&_relu_tmp100_15._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_78_111._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_80_113, 0, degree), Coeffs(&_relu_tmp100_15._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_78_111._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_58_91._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp100_15._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_78_111._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_58_91._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_79_112, 0, degree), Coeffs(&_pgen_tmp_80_113, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_58_91._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp100_15._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_78_111._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_78_111._c1_poly);
  Free_data(&_pgen_tmp_78_111._c0_poly);
  Free_data(&_relu_tmp100_15._c1_poly);
  Free_data(&_relu_tmp100_15._c0_poly);
  Free_data(&_pgen_tmp_79_112);
  Free_data(&_pgen_tmp_79_112);
  Free_data(&_pgen_tmp_80_113);
  Free_data(&_pgen_tmp_80_113);
  CMPLR_TM_END(16, 0);
  _preg_268435467 = Relinearize(_pgen_tmp_58_91);
  Free_data(&_pgen_tmp_58_91._c2_poly);
  Free_data(&_pgen_tmp_58_91._c1_poly);
  Free_data(&_pgen_tmp_58_91._c0_poly);
  Encode_double(&_pgen_tmp_83_116, &_cst_155, 1, 1, Level(&_relu_tmp80_11), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_84_117, &_relu_tmp80_11, &_pgen_tmp_83_116);
  CMPLR_TM_END(17, 0);
  Encode_float_mask(&_pgen_tmp_85_118, 0.5685231134608953462717, mask_len_2, 1, Level(&_relu_tmp58_7), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_86_119, &_relu_tmp58_7, &_pgen_tmp_85_118);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_82_115, &_pgen_tmp_84_117, &_pgen_tmp_86_119);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_82_115._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_84_117._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp80_11._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_83_116._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_84_117._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp80_11._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_83_116._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_86_119._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_85_118._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_86_119._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_85_118._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_82_115._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_84_117._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_86_119._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_82_115._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_84_117._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_86_119._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_83_116._poly);
  Free_data(&_pgen_tmp_85_118._poly);
  Free_data(&_pgen_tmp_86_119._c1_poly);
  Free_data(&_pgen_tmp_86_119._c0_poly);
  Free_data(&_pgen_tmp_84_117._c1_poly);
  Free_data(&_pgen_tmp_84_117._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_87_120, &_pgen_tmp_82_115);
  Rescale(&_pgen_tmp_87_120._c0_poly, &_pgen_tmp_82_115._c0_poly);
  Rescale(&_pgen_tmp_87_120._c1_poly, &_pgen_tmp_82_115._c1_poly);
  Free_data(&_pgen_tmp_82_115._c1_poly);
  Free_data(&_pgen_tmp_82_115._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_81_114, &_relu_tmp90_13, &_pgen_tmp_87_120);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_81_114._c0_poly);
  Init_poly_by_size(&_pgen_tmp_88_121, 1, 0);
  Init_poly_by_size(&_pgen_tmp_89_122, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_88_121, 0, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_87_120._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_89_122, 0, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_87_120._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_81_114._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_87_120._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_81_114._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_88_121, 0, degree), Coeffs(&_pgen_tmp_89_122, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_81_114._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_87_120._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_87_120._c1_poly);
  Free_data(&_pgen_tmp_87_120._c0_poly);
  Free_data(&_relu_tmp90_13._c1_poly);
  Free_data(&_relu_tmp90_13._c0_poly);
  Free_data(&_pgen_tmp_88_121);
  Free_data(&_pgen_tmp_88_121);
  Free_data(&_pgen_tmp_89_122);
  Free_data(&_pgen_tmp_89_122);
  CMPLR_TM_END(16, 0);
  _preg_268435468 = Relinearize(_pgen_tmp_81_114);
  Free_data(&_pgen_tmp_81_114._c2_poly);
  Free_data(&_pgen_tmp_81_114._c1_poly);
  Free_data(&_pgen_tmp_81_114._c0_poly);
  Encode_double(&_pgen_tmp_90_123, &_cst_158, 1, 1, Level(&_relu_tmp80_11), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_91_124, &_relu_tmp80_11, &_pgen_tmp_90_123);
  CMPLR_TM_END(17, 0);
  Encode_float_mask(&_pgen_tmp_92_125, 1.614771079042867985009, mask_len_2, 1, Level(&_relu_tmp58_7), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_93_126, &_relu_tmp58_7, &_pgen_tmp_92_125);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_94_127, &_pgen_tmp_91_124, &_pgen_tmp_93_126);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_95_128, &_preg_268435468, &_pgen_tmp_94_127);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_57_90, &_preg_268435467, &_pgen_tmp_95_128);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_57_90._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_91_124._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp80_11._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_90_123._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_91_124._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp80_11._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_90_123._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_93_126._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_92_125._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_93_126._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_92_125._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_94_127._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_91_124._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_93_126._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_94_127._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_91_124._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_93_126._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_95_128._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435468._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_94_127._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_95_128._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435468._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_94_127._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_57_90._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435467._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_95_128._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_57_90._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435467._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_95_128._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_90_123._poly);
  Free_data(&_relu_tmp80_11._c1_poly);
  Free_data(&_relu_tmp80_11._c0_poly);
  Free_data(&_pgen_tmp_92_125._poly);
  Free_data(&_relu_tmp58_7._c1_poly);
  Free_data(&_relu_tmp58_7._c0_poly);
  Free_data(&_pgen_tmp_93_126._c1_poly);
  Free_data(&_pgen_tmp_93_126._c0_poly);
  Free_data(&_pgen_tmp_91_124._c1_poly);
  Free_data(&_pgen_tmp_91_124._c0_poly);
  Free_data(&_pgen_tmp_94_127._c1_poly);
  Free_data(&_pgen_tmp_94_127._c0_poly);
  Free_data(&_preg_268435468._c1_poly);
  Free_data(&_preg_268435468._c0_poly);
  Free_data(&_pgen_tmp_95_128._c1_poly);
  Free_data(&_pgen_tmp_95_128._c0_poly);
  Free_data(&_preg_268435467._c1_poly);
  Free_data(&_preg_268435467._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_relu_tmp155_16, &_pgen_tmp_57_90);
  Rescale(&_relu_tmp155_16._c0_poly, &_pgen_tmp_57_90._c0_poly);
  Rescale(&_relu_tmp155_16._c1_poly, &_pgen_tmp_57_90._c1_poly);
  Free_data(&_pgen_tmp_57_90._c1_poly);
  Free_data(&_pgen_tmp_57_90._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_96_129, &_relu_tmp155_16, &_relu_tmp155_16);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_96_129._c0_poly);
  Init_poly_by_size(&_pgen_tmp_97_130, 1, 0);
  Init_poly_by_size(&_pgen_tmp_98_131, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_97_130, 0, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_98_131, 0, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_96_129._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_96_129._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_97_130, 0, degree), Coeffs(&_pgen_tmp_98_131, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_96_129._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_97_130);
  Free_data(&_pgen_tmp_97_130);
  Free_data(&_pgen_tmp_98_131);
  Free_data(&_pgen_tmp_98_131);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_99_132, &_pgen_tmp_96_129);
  Rescale(&_pgen_tmp_99_132._c0_poly, &_pgen_tmp_96_129._c0_poly);
  Rescale(&_pgen_tmp_99_132._c1_poly, &_pgen_tmp_96_129._c1_poly);
  Rescale(&_pgen_tmp_99_132._c2_poly, &_pgen_tmp_96_129._c2_poly);
  Free_data(&_pgen_tmp_96_129._c2_poly);
  Free_data(&_pgen_tmp_96_129._c1_poly);
  Free_data(&_pgen_tmp_96_129._c0_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435469 = Relinearize(_pgen_tmp_99_132);
  Free_data(&_pgen_tmp_99_132._c2_poly);
  Free_data(&_pgen_tmp_99_132._c1_poly);
  Free_data(&_pgen_tmp_99_132._c0_poly);
  Copy_ciph(&_relu_tmp159_17, &_preg_268435469);
  Free_data(&_preg_268435469._c1_poly);
  Free_data(&_preg_268435469._c0_poly);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_100_133, &_relu_tmp159_17, &_relu_tmp159_17);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_101_134, &_cst_159, 1, Sc_degree(&_pgen_tmp_100_133), Level(&_pgen_tmp_100_133), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp165_18, &_pgen_tmp_100_133, &_pgen_tmp_101_134);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_relu_tmp165_18._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_100_133._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp159_17._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp159_17._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_100_133._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp159_17._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp159_17._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_relu_tmp165_18._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_100_133._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_101_134._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Set_coeffs(&_relu_tmp165_18._c1_poly, _pgen_rns_idx_33, degree, Coeffs(&_pgen_tmp_100_133._c1_poly, _pgen_rns_idx_33, degree));
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_relu_tmp159_17._c1_poly);
  Free_data(&_relu_tmp159_17._c0_poly);
  Free_data(&_pgen_tmp_101_134._poly);
  Free_data(&_pgen_tmp_100_133._c1_poly);
  Free_data(&_pgen_tmp_100_133._c0_poly);
  CMPLR_TM_END(13, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_102_135, &_relu_tmp155_16, &_relu_tmp165_18);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_102_135._c0_poly);
  Init_poly_by_size(&_pgen_tmp_103_136, 1, 0);
  Init_poly_by_size(&_pgen_tmp_104_137, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_103_136, 0, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp165_18._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_104_137, 0, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp165_18._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_102_135._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp165_18._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_102_135._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_103_136, 0, degree), Coeffs(&_pgen_tmp_104_137, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_102_135._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp165_18._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_103_136);
  Free_data(&_pgen_tmp_103_136);
  Free_data(&_pgen_tmp_104_137);
  Free_data(&_pgen_tmp_104_137);
  CMPLR_TM_END(16, 0);
  _preg_268435470 = Relinearize(_pgen_tmp_102_135);
  Free_data(&_pgen_tmp_102_135._c2_poly);
  Free_data(&_pgen_tmp_102_135._c1_poly);
  Free_data(&_pgen_tmp_102_135._c0_poly);
  Copy_ciph(&_relu_tmp169_19, &_preg_268435470);
  Free_data(&_preg_268435470._c1_poly);
  Free_data(&_preg_268435470._c0_poly);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_106_139, &_relu_tmp169_19, &_relu_tmp169_19);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_107_140, &_cst_160, 1, 1, Level(&_relu_tmp155_16), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_108_141, &_relu_tmp155_16, &_pgen_tmp_107_140);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_105_138, &_pgen_tmp_106_139, &_pgen_tmp_108_141);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_105_138._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_106_139._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp169_19._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp169_19._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_106_139._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp169_19._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp169_19._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_108_141._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_107_140._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_108_141._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_107_140._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_105_138._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_106_139._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_108_141._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_105_138._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_106_139._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_108_141._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_relu_tmp169_19._c1_poly);
  Free_data(&_relu_tmp169_19._c0_poly);
  Free_data(&_pgen_tmp_107_140._poly);
  Free_data(&_pgen_tmp_108_141._c1_poly);
  Free_data(&_pgen_tmp_108_141._c0_poly);
  Free_data(&_pgen_tmp_106_139._c1_poly);
  Free_data(&_pgen_tmp_106_139._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_relu_tmp177_20, &_pgen_tmp_105_138);
  Rescale(&_relu_tmp177_20._c0_poly, &_pgen_tmp_105_138._c0_poly);
  Rescale(&_relu_tmp177_20._c1_poly, &_pgen_tmp_105_138._c1_poly);
  Free_data(&_pgen_tmp_105_138._c1_poly);
  Free_data(&_pgen_tmp_105_138._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_109_142, &_relu_tmp165_18, &_relu_tmp165_18);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_109_142._c0_poly);
  Init_poly_by_size(&_pgen_tmp_110_143, 1, 0);
  Init_poly_by_size(&_pgen_tmp_111_144, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_110_143, 0, degree), Coeffs(&_relu_tmp165_18._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp165_18._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_111_144, 0, degree), Coeffs(&_relu_tmp165_18._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp165_18._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_109_142._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp165_18._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp165_18._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_109_142._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_110_143, 0, degree), Coeffs(&_pgen_tmp_111_144, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_109_142._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp165_18._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp165_18._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_relu_tmp165_18._c1_poly);
  Free_data(&_relu_tmp165_18._c0_poly);
  Free_data(&_pgen_tmp_110_143);
  Free_data(&_pgen_tmp_110_143);
  Free_data(&_pgen_tmp_111_144);
  Free_data(&_pgen_tmp_111_144);
  CMPLR_TM_END(16, 0);
  _preg_268435471 = Relinearize(_pgen_tmp_109_142);
  Free_data(&_pgen_tmp_109_142._c2_poly);
  Free_data(&_pgen_tmp_109_142._c1_poly);
  Free_data(&_pgen_tmp_109_142._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_relu_tmp181_21, &_preg_268435471);
  Rescale(&_relu_tmp181_21._c0_poly, &_preg_268435471._c0_poly);
  Rescale(&_relu_tmp181_21._c1_poly, &_preg_268435471._c1_poly);
  Free_data(&_preg_268435471._c1_poly);
  Free_data(&_preg_268435471._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_112_145, &_relu_tmp181_21, &_relu_tmp181_21);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_113_146, &_cst_161, 1, Sc_degree(&_pgen_tmp_112_145), Level(&_pgen_tmp_112_145), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp187_22, &_pgen_tmp_112_145, &_pgen_tmp_113_146);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_relu_tmp187_22._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_112_145._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp181_21._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp181_21._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_112_145._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp181_21._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp181_21._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_112_145._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_113_146._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Set_coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_33, degree, Coeffs(&_pgen_tmp_112_145._c1_poly, _pgen_rns_idx_33, degree));
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_relu_tmp181_21._c1_poly);
  Free_data(&_relu_tmp181_21._c0_poly);
  Free_data(&_pgen_tmp_113_146._poly);
  Free_data(&_pgen_tmp_112_145._c1_poly);
  Free_data(&_pgen_tmp_112_145._c0_poly);
  CMPLR_TM_END(13, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_114_147, &_relu_tmp187_22, &_relu_tmp187_22);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_114_147._c0_poly);
  Init_poly_by_size(&_pgen_tmp_115_148, 1, 0);
  Init_poly_by_size(&_pgen_tmp_116_149, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_115_148, 0, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_116_149, 0, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_114_147._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_114_147._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_115_148, 0, degree), Coeffs(&_pgen_tmp_116_149, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_114_147._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_115_148);
  Free_data(&_pgen_tmp_115_148);
  Free_data(&_pgen_tmp_116_149);
  Free_data(&_pgen_tmp_116_149);
  CMPLR_TM_END(16, 0);
  _preg_268435472 = Relinearize(_pgen_tmp_114_147);
  Free_data(&_pgen_tmp_114_147._c2_poly);
  Free_data(&_pgen_tmp_114_147._c1_poly);
  Free_data(&_pgen_tmp_114_147._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_relu_tmp191_23, &_preg_268435472);
  Rescale(&_relu_tmp191_23._c0_poly, &_preg_268435472._c0_poly);
  Rescale(&_relu_tmp191_23._c1_poly, &_preg_268435472._c1_poly);
  Free_data(&_preg_268435472._c1_poly);
  Free_data(&_preg_268435472._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_117_150, &_relu_tmp191_23, &_relu_tmp191_23);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_118_151, &_cst_162, 1, Sc_degree(&_pgen_tmp_117_150), Level(&_pgen_tmp_117_150), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp197_24, &_pgen_tmp_117_150, &_pgen_tmp_118_151);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_relu_tmp197_24._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_117_150._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp191_23._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp191_23._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_117_150._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp191_23._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp191_23._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_relu_tmp197_24._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_117_150._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_118_151._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Set_coeffs(&_relu_tmp197_24._c1_poly, _pgen_rns_idx_33, degree, Coeffs(&_pgen_tmp_117_150._c1_poly, _pgen_rns_idx_33, degree));
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_relu_tmp191_23._c1_poly);
  Free_data(&_relu_tmp191_23._c0_poly);
  Free_data(&_pgen_tmp_118_151._poly);
  Free_data(&_pgen_tmp_117_150._c1_poly);
  Free_data(&_pgen_tmp_117_150._c0_poly);
  CMPLR_TM_END(13, 0);
  Encode_float_mask(&_pgen_tmp_125_158, 0.0001111119162030077897477, mask_len_2, 1, Level(&_relu_tmp155_16), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_124_157, &_relu_tmp155_16, &_pgen_tmp_125_158);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_124_157._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_124_157._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_125_158._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_124_157._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_125_158._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_125_158._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_126_159, &_pgen_tmp_124_157);
  Rescale(&_pgen_tmp_126_159._c0_poly, &_pgen_tmp_124_157._c0_poly);
  Rescale(&_pgen_tmp_126_159._c1_poly, &_pgen_tmp_124_157._c1_poly);
  Free_data(&_pgen_tmp_124_157._c1_poly);
  Free_data(&_pgen_tmp_124_157._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_123_156, &input_ct0_0, &_pgen_tmp_126_159);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_123_156._c0_poly);
  Init_poly_by_size(&_pgen_tmp_127_160, 1, 0);
  Init_poly_by_size(&_pgen_tmp_128_161, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_127_160, 0, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_126_159._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_128_161, 0, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_126_159._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_123_156._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_126_159._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_123_156._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_127_160, 0, degree), Coeffs(&_pgen_tmp_128_161, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_123_156._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_126_159._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_126_159._c1_poly);
  Free_data(&_pgen_tmp_126_159._c0_poly);
  Free_data(&_pgen_tmp_127_160);
  Free_data(&_pgen_tmp_127_160);
  Free_data(&_pgen_tmp_128_161);
  Free_data(&_pgen_tmp_128_161);
  CMPLR_TM_END(16, 0);
  _preg_268435473 = Relinearize(_pgen_tmp_123_156);
  Free_data(&_pgen_tmp_123_156._c2_poly);
  Free_data(&_pgen_tmp_123_156._c1_poly);
  Free_data(&_pgen_tmp_123_156._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_129_162, &_preg_268435473);
  Rescale(&_pgen_tmp_129_162._c0_poly, &_preg_268435473._c0_poly);
  Rescale(&_pgen_tmp_129_162._c1_poly, &_preg_268435473._c1_poly);
  Free_data(&_preg_268435473._c1_poly);
  Free_data(&_preg_268435473._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_122_155, &_relu_tmp187_22, &_pgen_tmp_129_162);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_122_155._c0_poly);
  Init_poly_by_size(&_pgen_tmp_130_163, 1, 0);
  Init_poly_by_size(&_pgen_tmp_131_164, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_130_163, 0, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_129_162._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_131_164, 0, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_129_162._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_122_155._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_129_162._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_122_155._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_130_163, 0, degree), Coeffs(&_pgen_tmp_131_164, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_122_155._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_129_162._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_129_162._c1_poly);
  Free_data(&_pgen_tmp_129_162._c0_poly);
  Free_data(&_pgen_tmp_130_163);
  Free_data(&_pgen_tmp_130_163);
  Free_data(&_pgen_tmp_131_164);
  Free_data(&_pgen_tmp_131_164);
  CMPLR_TM_END(16, 0);
  _preg_268435474 = Relinearize(_pgen_tmp_122_155);
  Free_data(&_pgen_tmp_122_155._c2_poly);
  Free_data(&_pgen_tmp_122_155._c1_poly);
  Free_data(&_pgen_tmp_122_155._c0_poly);
  Encode_float_mask(&_pgen_tmp_134_167, -0.0009371069270347270278171, mask_len_2, 1, Level(&input_ct0_0), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_133_166, &input_ct0_0, &_pgen_tmp_134_167);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_133_166._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_133_166._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_134_167._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_133_166._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_134_167._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_134_167._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_135_168, &_pgen_tmp_133_166);
  Rescale(&_pgen_tmp_135_168._c0_poly, &_pgen_tmp_133_166._c0_poly);
  Rescale(&_pgen_tmp_135_168._c1_poly, &_pgen_tmp_133_166._c1_poly);
  Free_data(&_pgen_tmp_133_166._c1_poly);
  Free_data(&_pgen_tmp_133_166._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_132_165, &_relu_tmp177_20, &_pgen_tmp_135_168);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_132_165._c0_poly);
  Init_poly_by_size(&_pgen_tmp_136_169, 1, 0);
  Init_poly_by_size(&_pgen_tmp_137_170, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_136_169, 0, degree), Coeffs(&_relu_tmp177_20._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_135_168._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_137_170, 0, degree), Coeffs(&_relu_tmp177_20._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_135_168._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_132_165._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp177_20._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_135_168._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_132_165._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_136_169, 0, degree), Coeffs(&_pgen_tmp_137_170, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_132_165._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp177_20._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_135_168._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_135_168._c1_poly);
  Free_data(&_pgen_tmp_135_168._c0_poly);
  Free_data(&_pgen_tmp_136_169);
  Free_data(&_pgen_tmp_136_169);
  Free_data(&_pgen_tmp_137_170);
  Free_data(&_pgen_tmp_137_170);
  CMPLR_TM_END(16, 0);
  _preg_268435475 = Relinearize(_pgen_tmp_132_165);
  Free_data(&_pgen_tmp_132_165._c2_poly);
  Free_data(&_pgen_tmp_132_165._c1_poly);
  Free_data(&_pgen_tmp_132_165._c0_poly);
  Encode_float_mask(&_pgen_tmp_140_173, 0.006507995190210730772351, mask_len_2, 1, Level(&_relu_tmp155_16), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_139_172, &_relu_tmp155_16, &_pgen_tmp_140_173);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_139_172._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_139_172._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_140_173._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_139_172._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_140_173._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_140_173._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_141_174, &_pgen_tmp_139_172);
  Rescale(&_pgen_tmp_141_174._c0_poly, &_pgen_tmp_139_172._c0_poly);
  Rescale(&_pgen_tmp_141_174._c1_poly, &_pgen_tmp_139_172._c1_poly);
  Free_data(&_pgen_tmp_139_172._c1_poly);
  Free_data(&_pgen_tmp_139_172._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_138_171, &input_ct0_0, &_pgen_tmp_141_174);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_138_171._c0_poly);
  Init_poly_by_size(&_pgen_tmp_142_175, 1, 0);
  Init_poly_by_size(&_pgen_tmp_143_176, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_142_175, 0, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_141_174._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_143_176, 0, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_141_174._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_138_171._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_141_174._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_138_171._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_142_175, 0, degree), Coeffs(&_pgen_tmp_143_176, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_138_171._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_141_174._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_141_174._c1_poly);
  Free_data(&_pgen_tmp_141_174._c0_poly);
  Free_data(&_pgen_tmp_142_175);
  Free_data(&_pgen_tmp_142_175);
  Free_data(&_pgen_tmp_143_176);
  Free_data(&_pgen_tmp_143_176);
  CMPLR_TM_END(16, 0);
  _preg_268435476 = Relinearize(_pgen_tmp_138_171);
  Free_data(&_pgen_tmp_138_171._c2_poly);
  Free_data(&_pgen_tmp_138_171._c1_poly);
  Free_data(&_pgen_tmp_138_171._c0_poly);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_144_177, &_preg_268435475, &_preg_268435476);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_121_154, &_preg_268435474, &_pgen_tmp_144_177);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_121_154._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_144_177._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435475._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435476._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_144_177._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435475._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435476._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_121_154._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435474._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_144_177._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_121_154._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435474._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_144_177._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_preg_268435476._c1_poly);
  Free_data(&_preg_268435476._c0_poly);
  Free_data(&_preg_268435475._c1_poly);
  Free_data(&_preg_268435475._c0_poly);
  Free_data(&_pgen_tmp_144_177._c1_poly);
  Free_data(&_pgen_tmp_144_177._c0_poly);
  Free_data(&_preg_268435474._c1_poly);
  Free_data(&_preg_268435474._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_145_178, &_pgen_tmp_121_154);
  Rescale(&_pgen_tmp_145_178._c0_poly, &_pgen_tmp_121_154._c0_poly);
  Rescale(&_pgen_tmp_145_178._c1_poly, &_pgen_tmp_121_154._c1_poly);
  Free_data(&_pgen_tmp_121_154._c1_poly);
  Free_data(&_pgen_tmp_121_154._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_120_153, &_relu_tmp197_24, &_pgen_tmp_145_178);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_120_153._c0_poly);
  Init_poly_by_size(&_pgen_tmp_146_179, 1, 0);
  Init_poly_by_size(&_pgen_tmp_147_180, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_146_179, 0, degree), Coeffs(&_relu_tmp197_24._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_145_178._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_147_180, 0, degree), Coeffs(&_relu_tmp197_24._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_145_178._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_120_153._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp197_24._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_145_178._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_120_153._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_146_179, 0, degree), Coeffs(&_pgen_tmp_147_180, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_120_153._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp197_24._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_145_178._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_145_178._c1_poly);
  Free_data(&_pgen_tmp_145_178._c0_poly);
  Free_data(&_relu_tmp197_24._c1_poly);
  Free_data(&_relu_tmp197_24._c0_poly);
  Free_data(&_pgen_tmp_146_179);
  Free_data(&_pgen_tmp_146_179);
  Free_data(&_pgen_tmp_147_180);
  Free_data(&_pgen_tmp_147_180);
  CMPLR_TM_END(16, 0);
  _preg_268435477 = Relinearize(_pgen_tmp_120_153);
  Free_data(&_pgen_tmp_120_153._c2_poly);
  Free_data(&_pgen_tmp_120_153._c1_poly);
  Free_data(&_pgen_tmp_120_153._c0_poly);
  Encode_float_mask(&_pgen_tmp_152_185, -0.03691063940349948585551, mask_len_2, 1, Level(&input_ct0_0), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_151_184, &input_ct0_0, &_pgen_tmp_152_185);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_151_184._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_151_184._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_152_185._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_151_184._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_152_185._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_152_185._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_153_186, &_pgen_tmp_151_184);
  Rescale(&_pgen_tmp_153_186._c0_poly, &_pgen_tmp_151_184._c0_poly);
  Rescale(&_pgen_tmp_153_186._c1_poly, &_pgen_tmp_151_184._c1_poly);
  Free_data(&_pgen_tmp_151_184._c1_poly);
  Free_data(&_pgen_tmp_151_184._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_150_183, &_relu_tmp177_20, &_pgen_tmp_153_186);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_150_183._c0_poly);
  Init_poly_by_size(&_pgen_tmp_154_187, 1, 0);
  Init_poly_by_size(&_pgen_tmp_155_188, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_154_187, 0, degree), Coeffs(&_relu_tmp177_20._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_153_186._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_155_188, 0, degree), Coeffs(&_relu_tmp177_20._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_153_186._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_150_183._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp177_20._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_153_186._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_150_183._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_154_187, 0, degree), Coeffs(&_pgen_tmp_155_188, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_150_183._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp177_20._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_153_186._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_153_186._c1_poly);
  Free_data(&_pgen_tmp_153_186._c0_poly);
  Free_data(&_pgen_tmp_154_187);
  Free_data(&_pgen_tmp_154_187);
  Free_data(&_pgen_tmp_155_188);
  Free_data(&_pgen_tmp_155_188);
  CMPLR_TM_END(16, 0);
  _preg_268435478 = Relinearize(_pgen_tmp_150_183);
  Free_data(&_pgen_tmp_150_183._c2_poly);
  Free_data(&_pgen_tmp_150_183._c1_poly);
  Free_data(&_pgen_tmp_150_183._c0_poly);
  Encode_float_mask(&_pgen_tmp_158_191, 0.1056744718173614761003, mask_len_2, 1, Level(&_relu_tmp155_16), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_157_190, &_relu_tmp155_16, &_pgen_tmp_158_191);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_157_190._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_157_190._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_158_191._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_157_190._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_158_191._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_158_191._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_159_192, &_pgen_tmp_157_190);
  Rescale(&_pgen_tmp_159_192._c0_poly, &_pgen_tmp_157_190._c0_poly);
  Rescale(&_pgen_tmp_159_192._c1_poly, &_pgen_tmp_157_190._c1_poly);
  Free_data(&_pgen_tmp_157_190._c1_poly);
  Free_data(&_pgen_tmp_157_190._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_156_189, &input_ct0_0, &_pgen_tmp_159_192);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_156_189._c0_poly);
  Init_poly_by_size(&_pgen_tmp_160_193, 1, 0);
  Init_poly_by_size(&_pgen_tmp_161_194, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_160_193, 0, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_159_192._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_161_194, 0, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_159_192._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_156_189._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_159_192._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_156_189._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_160_193, 0, degree), Coeffs(&_pgen_tmp_161_194, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_156_189._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_159_192._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_159_192._c1_poly);
  Free_data(&_pgen_tmp_159_192._c0_poly);
  Free_data(&_pgen_tmp_160_193);
  Free_data(&_pgen_tmp_160_193);
  Free_data(&_pgen_tmp_161_194);
  Free_data(&_pgen_tmp_161_194);
  CMPLR_TM_END(16, 0);
  _preg_268435479 = Relinearize(_pgen_tmp_156_189);
  Free_data(&_pgen_tmp_156_189._c2_poly);
  Free_data(&_pgen_tmp_156_189._c1_poly);
  Free_data(&_pgen_tmp_156_189._c0_poly);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_149_182, &_preg_268435478, &_preg_268435479);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_149_182._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_149_182._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435478._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435479._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_149_182._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435478._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435479._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_preg_268435479._c1_poly);
  Free_data(&_preg_268435479._c0_poly);
  Free_data(&_preg_268435478._c1_poly);
  Free_data(&_preg_268435478._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_162_195, &_pgen_tmp_149_182);
  Rescale(&_pgen_tmp_162_195._c0_poly, &_pgen_tmp_149_182._c0_poly);
  Rescale(&_pgen_tmp_162_195._c1_poly, &_pgen_tmp_149_182._c1_poly);
  Free_data(&_pgen_tmp_149_182._c1_poly);
  Free_data(&_pgen_tmp_149_182._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_148_181, &_relu_tmp187_22, &_pgen_tmp_162_195);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_148_181._c0_poly);
  Init_poly_by_size(&_pgen_tmp_163_196, 1, 0);
  Init_poly_by_size(&_pgen_tmp_164_197, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_163_196, 0, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_162_195._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_164_197, 0, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_162_195._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_148_181._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_162_195._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_148_181._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_163_196, 0, degree), Coeffs(&_pgen_tmp_164_197, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_148_181._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_162_195._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_162_195._c1_poly);
  Free_data(&_pgen_tmp_162_195._c0_poly);
  Free_data(&_relu_tmp187_22._c1_poly);
  Free_data(&_relu_tmp187_22._c0_poly);
  Free_data(&_pgen_tmp_163_196);
  Free_data(&_pgen_tmp_163_196);
  Free_data(&_pgen_tmp_164_197);
  Free_data(&_pgen_tmp_164_197);
  CMPLR_TM_END(16, 0);
  _preg_268435480 = Relinearize(_pgen_tmp_148_181);
  Free_data(&_pgen_tmp_148_181._c2_poly);
  Free_data(&_pgen_tmp_148_181._c1_poly);
  Free_data(&_pgen_tmp_148_181._c0_poly);
  Encode_float_mask(&_pgen_tmp_167_200, -0.2078597158810965850329, mask_len_2, 1, Level(&input_ct0_0), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_166_199, &input_ct0_0, &_pgen_tmp_167_200);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_166_199._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_166_199._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_167_200._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_166_199._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_167_200._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_167_200._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_168_201, &_pgen_tmp_166_199);
  Rescale(&_pgen_tmp_168_201._c0_poly, &_pgen_tmp_166_199._c0_poly);
  Rescale(&_pgen_tmp_168_201._c1_poly, &_pgen_tmp_166_199._c1_poly);
  Free_data(&_pgen_tmp_166_199._c1_poly);
  Free_data(&_pgen_tmp_166_199._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_165_198, &_relu_tmp177_20, &_pgen_tmp_168_201);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_165_198._c0_poly);
  Init_poly_by_size(&_pgen_tmp_169_202, 1, 0);
  Init_poly_by_size(&_pgen_tmp_170_203, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_169_202, 0, degree), Coeffs(&_relu_tmp177_20._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_168_201._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_170_203, 0, degree), Coeffs(&_relu_tmp177_20._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_168_201._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_165_198._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp177_20._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_168_201._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_165_198._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_169_202, 0, degree), Coeffs(&_pgen_tmp_170_203, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_165_198._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp177_20._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_168_201._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_168_201._c1_poly);
  Free_data(&_pgen_tmp_168_201._c0_poly);
  Free_data(&_relu_tmp177_20._c1_poly);
  Free_data(&_relu_tmp177_20._c0_poly);
  Free_data(&_pgen_tmp_169_202);
  Free_data(&_pgen_tmp_169_202);
  Free_data(&_pgen_tmp_170_203);
  Free_data(&_pgen_tmp_170_203);
  CMPLR_TM_END(16, 0);
  _preg_268435481 = Relinearize(_pgen_tmp_165_198);
  Free_data(&_pgen_tmp_165_198._c2_poly);
  Free_data(&_pgen_tmp_165_198._c1_poly);
  Free_data(&_pgen_tmp_165_198._c0_poly);
  Encode_float_mask(&_pgen_tmp_173_206, 0.6334139843709288841822, mask_len_2, 1, Level(&_relu_tmp155_16), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_172_205, &_relu_tmp155_16, &_pgen_tmp_173_206);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_172_205._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_172_205._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_173_206._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_172_205._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_173_206._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_173_206._poly);
  Free_data(&_relu_tmp155_16._c1_poly);
  Free_data(&_relu_tmp155_16._c0_poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_174_207, &_pgen_tmp_172_205);
  Rescale(&_pgen_tmp_174_207._c0_poly, &_pgen_tmp_172_205._c0_poly);
  Rescale(&_pgen_tmp_174_207._c1_poly, &_pgen_tmp_172_205._c1_poly);
  Free_data(&_pgen_tmp_172_205._c1_poly);
  Free_data(&_pgen_tmp_172_205._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_171_204, &input_ct0_0, &_pgen_tmp_174_207);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_171_204._c0_poly);
  Init_poly_by_size(&_pgen_tmp_175_208, 1, 0);
  Init_poly_by_size(&_pgen_tmp_176_209, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_175_208, 0, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_174_207._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_176_209, 0, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_174_207._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_171_204._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_174_207._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_171_204._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_175_208, 0, degree), Coeffs(&_pgen_tmp_176_209, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_171_204._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_174_207._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_174_207._c1_poly);
  Free_data(&_pgen_tmp_174_207._c0_poly);
  Free_data(&_pgen_tmp_175_208);
  Free_data(&_pgen_tmp_175_208);
  Free_data(&_pgen_tmp_176_209);
  Free_data(&_pgen_tmp_176_209);
  CMPLR_TM_END(16, 0);
  _preg_268435482 = Relinearize(_pgen_tmp_171_204);
  Free_data(&_pgen_tmp_171_204._c2_poly);
  Free_data(&_pgen_tmp_171_204._c1_poly);
  Free_data(&_pgen_tmp_171_204._c0_poly);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_177_210, &_preg_268435481, &_preg_268435482);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_178_211, &_preg_268435480, &_pgen_tmp_177_210);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_119_152, &_preg_268435477, &_pgen_tmp_178_211);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_119_152._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_177_210._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435481._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435482._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_177_210._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435481._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435482._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_178_211._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435480._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_177_210._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_178_211._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435480._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_177_210._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_119_152._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435477._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_178_211._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_119_152._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435477._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_178_211._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_preg_268435482._c1_poly);
  Free_data(&_preg_268435482._c0_poly);
  Free_data(&_preg_268435481._c1_poly);
  Free_data(&_preg_268435481._c0_poly);
  Free_data(&_pgen_tmp_177_210._c1_poly);
  Free_data(&_pgen_tmp_177_210._c0_poly);
  Free_data(&_preg_268435480._c1_poly);
  Free_data(&_preg_268435480._c0_poly);
  Free_data(&_pgen_tmp_178_211._c1_poly);
  Free_data(&_pgen_tmp_178_211._c0_poly);
  Free_data(&_preg_268435477._c1_poly);
  Free_data(&_preg_268435477._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_relu_tmp266_25, &_pgen_tmp_119_152);
  Rescale(&_relu_tmp266_25._c0_poly, &_pgen_tmp_119_152._c0_poly);
  Rescale(&_relu_tmp266_25._c1_poly, &_pgen_tmp_119_152._c1_poly);
  Free_data(&_pgen_tmp_119_152._c1_poly);
  Free_data(&_pgen_tmp_119_152._c0_poly);
  CMPLR_TM_END(19, 0);
  Encode_float_mask(&_pgen_tmp_180_213, 0.5, mask_len_2, 1, Level(&input_ct0_0), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_179_212, &input_ct0_0, &_pgen_tmp_180_213);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_179_212._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_179_212._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_180_213._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_179_212._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_180_213._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_180_213._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_181_214, &_pgen_tmp_179_212);
  Rescale(&_pgen_tmp_181_214._c0_poly, &_pgen_tmp_179_212._c0_poly);
  Rescale(&_pgen_tmp_181_214._c1_poly, &_pgen_tmp_179_212._c1_poly);
  Free_data(&_pgen_tmp_179_212._c1_poly);
  Free_data(&_pgen_tmp_179_212._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_relu_tmp274_26, &_relu_tmp266_25, &_pgen_tmp_181_214);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_relu_tmp274_26._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modadd(Coeffs(&_relu_tmp274_26._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp266_25._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_181_214._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_relu_tmp274_26._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp266_25._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_181_214._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_181_214._c1_poly);
  Free_data(&_pgen_tmp_181_214._c0_poly);
  Free_data(&_relu_tmp266_25._c1_poly);
  Free_data(&_relu_tmp266_25._c0_poly);
  CMPLR_TM_END(12, 0);
  return _relu_tmp274_26;
}

CIPHERTEXT App_relu_2(CIPHERTEXT input_ct0_0, CIPHERTEXT input_ct1_1, uint32_t mask_len_2) {
  CIPHERTEXT _relu_tmp8_3;
  CIPHERTEXT _relu_tmp14_4;
  CIPHERTEXT _relu_tmp18_5;
  CIPHERTEXT _relu_tmp24_6;
  CIPHERTEXT _relu_tmp58_7;
  CIPHERTEXT _relu_tmp62_8;
  CIPHERTEXT _relu_tmp68_9;
  CIPHERTEXT _relu_tmp72_10;
  CIPHERTEXT _relu_tmp80_11;
  CIPHERTEXT _relu_tmp84_12;
  CIPHERTEXT _relu_tmp90_13;
  CIPHERTEXT _relu_tmp94_14;
  CIPHERTEXT _relu_tmp100_15;
  CIPHERTEXT _relu_tmp155_16;
  CIPHERTEXT _relu_tmp159_17;
  CIPHERTEXT _relu_tmp165_18;
  CIPHERTEXT _relu_tmp169_19;
  CIPHERTEXT _relu_tmp177_20;
  CIPHERTEXT _relu_tmp181_21;
  CIPHERTEXT _relu_tmp187_22;
  CIPHERTEXT _relu_tmp191_23;
  CIPHERTEXT _relu_tmp197_24;
  CIPHERTEXT _relu_tmp266_25;
  CIPHERTEXT _relu_tmp274_26;
  CIPHERTEXT3 _pgen_tmp_0_30;
  MODULUS* _pgen_modulus_31;
  uint32_t _pgen_num_q_32;
  uint32_t _pgen_rns_idx_33;
  POLYNOMIAL _pgen_tmp_1_34;
  POLYNOMIAL _pgen_tmp_2_35;
  CIPHERTEXT3 _pgen_tmp_3_36;
  CIPHERTEXT _pgen_tmp_4_37;
  PLAINTEXT _pgen_tmp_5_38;
  CIPHERTEXT3 _pgen_tmp_6_39;
  POLYNOMIAL _pgen_tmp_7_40;
  POLYNOMIAL _pgen_tmp_8_41;
  CIPHERTEXT _pgen_tmp_9_42;
  PLAINTEXT _pgen_tmp_10_43;
  CIPHERTEXT _pgen_tmp_11_44;
  CIPHERTEXT3 _pgen_tmp_12_45;
  CIPHERTEXT _pgen_tmp_13_46;
  CIPHERTEXT3 _pgen_tmp_14_47;
  CIPHERTEXT _pgen_tmp_15_48;
  PLAINTEXT _pgen_tmp_16_49;
  CIPHERTEXT _pgen_tmp_17_50;
  POLYNOMIAL _pgen_tmp_18_51;
  POLYNOMIAL _pgen_tmp_19_52;
  PLAINTEXT _pgen_tmp_20_53;
  CIPHERTEXT _pgen_tmp_21_54;
  CIPHERTEXT _pgen_tmp_22_55;
  POLYNOMIAL _pgen_tmp_23_56;
  POLYNOMIAL _pgen_tmp_24_57;
  CIPHERTEXT3 _pgen_tmp_25_58;
  CIPHERTEXT _pgen_tmp_26_59;
  PLAINTEXT _pgen_tmp_27_60;
  CIPHERTEXT _pgen_tmp_28_61;
  POLYNOMIAL _pgen_tmp_29_62;
  POLYNOMIAL _pgen_tmp_30_63;
  PLAINTEXT _pgen_tmp_31_64;
  CIPHERTEXT _pgen_tmp_32_65;
  CIPHERTEXT _pgen_tmp_33_66;
  CIPHERTEXT3 _pgen_tmp_34_67;
  POLYNOMIAL _pgen_tmp_35_68;
  POLYNOMIAL _pgen_tmp_36_69;
  CIPHERTEXT _pgen_tmp_37_70;
  PLAINTEXT _pgen_tmp_38_71;
  CIPHERTEXT3 _pgen_tmp_39_72;
  POLYNOMIAL _pgen_tmp_40_73;
  POLYNOMIAL _pgen_tmp_41_74;
  CIPHERTEXT _pgen_tmp_42_75;
  CIPHERTEXT _pgen_tmp_43_76;
  PLAINTEXT _pgen_tmp_44_77;
  CIPHERTEXT _pgen_tmp_45_78;
  CIPHERTEXT3 _pgen_tmp_46_79;
  POLYNOMIAL _pgen_tmp_47_80;
  POLYNOMIAL _pgen_tmp_48_81;
  CIPHERTEXT _pgen_tmp_49_82;
  PLAINTEXT _pgen_tmp_50_83;
  CIPHERTEXT3 _pgen_tmp_51_84;
  POLYNOMIAL _pgen_tmp_52_85;
  POLYNOMIAL _pgen_tmp_53_86;
  CIPHERTEXT _pgen_tmp_54_87;
  PLAINTEXT _pgen_tmp_55_88;
  CIPHERTEXT _pgen_tmp_56_89;
  CIPHERTEXT3 _pgen_tmp_57_90;
  CIPHERTEXT _pgen_tmp_58_91;
  CIPHERTEXT3 _pgen_tmp_59_92;
  CIPHERTEXT _pgen_tmp_60_93;
  CIPHERTEXT3 _pgen_tmp_61_94;
  CIPHERTEXT _pgen_tmp_62_95;
  PLAINTEXT _pgen_tmp_63_96;
  CIPHERTEXT _pgen_tmp_64_97;
  POLYNOMIAL _pgen_tmp_65_98;
  POLYNOMIAL _pgen_tmp_66_99;
  PLAINTEXT _pgen_tmp_67_100;
  CIPHERTEXT _pgen_tmp_68_101;
  CIPHERTEXT _pgen_tmp_69_102;
  POLYNOMIAL _pgen_tmp_70_103;
  POLYNOMIAL _pgen_tmp_71_104;
  PLAINTEXT _pgen_tmp_72_105;
  CIPHERTEXT _pgen_tmp_73_106;
  PLAINTEXT _pgen_tmp_74_107;
  CIPHERTEXT _pgen_tmp_75_108;
  CIPHERTEXT _pgen_tmp_76_109;
  CIPHERTEXT _pgen_tmp_77_110;
  POLYNOMIAL _pgen_tmp_78_111;
  POLYNOMIAL _pgen_tmp_79_112;
  CIPHERTEXT3 _pgen_tmp_80_113;
  CIPHERTEXT _pgen_tmp_81_114;
  PLAINTEXT _pgen_tmp_82_115;
  CIPHERTEXT _pgen_tmp_83_116;
  PLAINTEXT _pgen_tmp_84_117;
  CIPHERTEXT _pgen_tmp_85_118;
  CIPHERTEXT _pgen_tmp_86_119;
  POLYNOMIAL _pgen_tmp_87_120;
  POLYNOMIAL _pgen_tmp_88_121;
  PLAINTEXT _pgen_tmp_89_122;
  CIPHERTEXT _pgen_tmp_90_123;
  PLAINTEXT _pgen_tmp_91_124;
  CIPHERTEXT _pgen_tmp_92_125;
  CIPHERTEXT _pgen_tmp_93_126;
  CIPHERTEXT _pgen_tmp_94_127;
  CIPHERTEXT3 _pgen_tmp_95_128;
  POLYNOMIAL _pgen_tmp_96_129;
  POLYNOMIAL _pgen_tmp_97_130;
  CIPHERTEXT3 _pgen_tmp_98_131;
  CIPHERTEXT _pgen_tmp_99_132;
  PLAINTEXT _pgen_tmp_100_133;
  CIPHERTEXT3 _pgen_tmp_101_134;
  POLYNOMIAL _pgen_tmp_102_135;
  POLYNOMIAL _pgen_tmp_103_136;
  CIPHERTEXT _pgen_tmp_104_137;
  CIPHERTEXT _pgen_tmp_105_138;
  PLAINTEXT _pgen_tmp_106_139;
  CIPHERTEXT _pgen_tmp_107_140;
  CIPHERTEXT3 _pgen_tmp_108_141;
  POLYNOMIAL _pgen_tmp_109_142;
  POLYNOMIAL _pgen_tmp_110_143;
  CIPHERTEXT _pgen_tmp_111_144;
  PLAINTEXT _pgen_tmp_112_145;
  CIPHERTEXT3 _pgen_tmp_113_146;
  POLYNOMIAL _pgen_tmp_114_147;
  POLYNOMIAL _pgen_tmp_115_148;
  CIPHERTEXT _pgen_tmp_116_149;
  PLAINTEXT _pgen_tmp_117_150;
  CIPHERTEXT3 _pgen_tmp_118_151;
  CIPHERTEXT _pgen_tmp_119_152;
  CIPHERTEXT3 _pgen_tmp_120_153;
  CIPHERTEXT3 _pgen_tmp_121_154;
  CIPHERTEXT _pgen_tmp_122_155;
  PLAINTEXT _pgen_tmp_123_156;
  CIPHERTEXT _pgen_tmp_124_157;
  POLYNOMIAL _pgen_tmp_125_158;
  POLYNOMIAL _pgen_tmp_126_159;
  CIPHERTEXT _pgen_tmp_127_160;
  POLYNOMIAL _pgen_tmp_128_161;
  POLYNOMIAL _pgen_tmp_129_162;
  CIPHERTEXT3 _pgen_tmp_130_163;
  CIPHERTEXT _pgen_tmp_131_164;
  PLAINTEXT _pgen_tmp_132_165;
  CIPHERTEXT _pgen_tmp_133_166;
  POLYNOMIAL _pgen_tmp_134_167;
  POLYNOMIAL _pgen_tmp_135_168;
  CIPHERTEXT3 _pgen_tmp_136_169;
  CIPHERTEXT _pgen_tmp_137_170;
  PLAINTEXT _pgen_tmp_138_171;
  CIPHERTEXT _pgen_tmp_139_172;
  POLYNOMIAL _pgen_tmp_140_173;
  POLYNOMIAL _pgen_tmp_141_174;
  CIPHERTEXT _pgen_tmp_142_175;
  CIPHERTEXT _pgen_tmp_143_176;
  POLYNOMIAL _pgen_tmp_144_177;
  POLYNOMIAL _pgen_tmp_145_178;
  CIPHERTEXT3 _pgen_tmp_146_179;
  CIPHERTEXT3 _pgen_tmp_147_180;
  CIPHERTEXT _pgen_tmp_148_181;
  CIPHERTEXT3 _pgen_tmp_149_182;
  CIPHERTEXT _pgen_tmp_150_183;
  PLAINTEXT _pgen_tmp_151_184;
  CIPHERTEXT _pgen_tmp_152_185;
  POLYNOMIAL _pgen_tmp_153_186;
  POLYNOMIAL _pgen_tmp_154_187;
  CIPHERTEXT3 _pgen_tmp_155_188;
  CIPHERTEXT _pgen_tmp_156_189;
  PLAINTEXT _pgen_tmp_157_190;
  CIPHERTEXT _pgen_tmp_158_191;
  POLYNOMIAL _pgen_tmp_159_192;
  POLYNOMIAL _pgen_tmp_160_193;
  CIPHERTEXT _pgen_tmp_161_194;
  POLYNOMIAL _pgen_tmp_162_195;
  POLYNOMIAL _pgen_tmp_163_196;
  CIPHERTEXT3 _pgen_tmp_164_197;
  CIPHERTEXT3 _pgen_tmp_165_198;
  CIPHERTEXT _pgen_tmp_166_199;
  PLAINTEXT _pgen_tmp_167_200;
  CIPHERTEXT _pgen_tmp_168_201;
  POLYNOMIAL _pgen_tmp_169_202;
  POLYNOMIAL _pgen_tmp_170_203;
  CIPHERTEXT3 _pgen_tmp_171_204;
  CIPHERTEXT3 _pgen_tmp_172_205;
  CIPHERTEXT _pgen_tmp_173_206;
  PLAINTEXT _pgen_tmp_174_207;
  CIPHERTEXT _pgen_tmp_175_208;
  POLYNOMIAL _pgen_tmp_176_209;
  POLYNOMIAL _pgen_tmp_177_210;
  CIPHERTEXT3 _pgen_tmp_178_211;
  CIPHERTEXT _pgen_tmp_179_212;
  CIPHERTEXT _pgen_tmp_180_213;
  CIPHERTEXT _pgen_tmp_181_214;
  PLAINTEXT _pgen_tmp_182_215;
  CIPHERTEXT _pgen_tmp_183_216;
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
  CIPHERTEXT _preg_268435467;
  CIPHERTEXT _preg_268435468;
  CIPHERTEXT _preg_268435469;
  CIPHERTEXT _preg_268435470;
  CIPHERTEXT _preg_268435471;
  CIPHERTEXT _preg_268435472;
  CIPHERTEXT _preg_268435473;
  CIPHERTEXT _preg_268435474;
  CIPHERTEXT _preg_268435475;
  CIPHERTEXT _preg_268435476;
  CIPHERTEXT _preg_268435477;
  CIPHERTEXT _preg_268435478;
  CIPHERTEXT _preg_268435479;
  CIPHERTEXT _preg_268435480;
  CIPHERTEXT _preg_268435481;
  CIPHERTEXT _preg_268435482;
  uint32_t  degree = Degree();
  memset(&_relu_tmp8_3, 0, sizeof(_relu_tmp8_3));
  memset(&_relu_tmp14_4, 0, sizeof(_relu_tmp14_4));
  memset(&_relu_tmp18_5, 0, sizeof(_relu_tmp18_5));
  memset(&_relu_tmp24_6, 0, sizeof(_relu_tmp24_6));
  memset(&_relu_tmp58_7, 0, sizeof(_relu_tmp58_7));
  memset(&_relu_tmp62_8, 0, sizeof(_relu_tmp62_8));
  memset(&_relu_tmp68_9, 0, sizeof(_relu_tmp68_9));
  memset(&_relu_tmp72_10, 0, sizeof(_relu_tmp72_10));
  memset(&_relu_tmp80_11, 0, sizeof(_relu_tmp80_11));
  memset(&_relu_tmp84_12, 0, sizeof(_relu_tmp84_12));
  memset(&_relu_tmp90_13, 0, sizeof(_relu_tmp90_13));
  memset(&_relu_tmp94_14, 0, sizeof(_relu_tmp94_14));
  memset(&_relu_tmp100_15, 0, sizeof(_relu_tmp100_15));
  memset(&_relu_tmp155_16, 0, sizeof(_relu_tmp155_16));
  memset(&_relu_tmp159_17, 0, sizeof(_relu_tmp159_17));
  memset(&_relu_tmp165_18, 0, sizeof(_relu_tmp165_18));
  memset(&_relu_tmp169_19, 0, sizeof(_relu_tmp169_19));
  memset(&_relu_tmp177_20, 0, sizeof(_relu_tmp177_20));
  memset(&_relu_tmp181_21, 0, sizeof(_relu_tmp181_21));
  memset(&_relu_tmp187_22, 0, sizeof(_relu_tmp187_22));
  memset(&_relu_tmp191_23, 0, sizeof(_relu_tmp191_23));
  memset(&_relu_tmp197_24, 0, sizeof(_relu_tmp197_24));
  memset(&_relu_tmp266_25, 0, sizeof(_relu_tmp266_25));
  memset(&_relu_tmp274_26, 0, sizeof(_relu_tmp274_26));
  memset(&_pgen_tmp_0_30, 0, sizeof(_pgen_tmp_0_30));
  memset(&_pgen_tmp_1_34, 0, sizeof(_pgen_tmp_1_34));
  memset(&_pgen_tmp_2_35, 0, sizeof(_pgen_tmp_2_35));
  memset(&_pgen_tmp_3_36, 0, sizeof(_pgen_tmp_3_36));
  memset(&_pgen_tmp_4_37, 0, sizeof(_pgen_tmp_4_37));
  memset(&_pgen_tmp_5_38, 0, sizeof(_pgen_tmp_5_38));
  memset(&_pgen_tmp_6_39, 0, sizeof(_pgen_tmp_6_39));
  memset(&_pgen_tmp_7_40, 0, sizeof(_pgen_tmp_7_40));
  memset(&_pgen_tmp_8_41, 0, sizeof(_pgen_tmp_8_41));
  memset(&_pgen_tmp_9_42, 0, sizeof(_pgen_tmp_9_42));
  memset(&_pgen_tmp_10_43, 0, sizeof(_pgen_tmp_10_43));
  memset(&_pgen_tmp_11_44, 0, sizeof(_pgen_tmp_11_44));
  memset(&_pgen_tmp_12_45, 0, sizeof(_pgen_tmp_12_45));
  memset(&_pgen_tmp_13_46, 0, sizeof(_pgen_tmp_13_46));
  memset(&_pgen_tmp_14_47, 0, sizeof(_pgen_tmp_14_47));
  memset(&_pgen_tmp_15_48, 0, sizeof(_pgen_tmp_15_48));
  memset(&_pgen_tmp_16_49, 0, sizeof(_pgen_tmp_16_49));
  memset(&_pgen_tmp_17_50, 0, sizeof(_pgen_tmp_17_50));
  memset(&_pgen_tmp_18_51, 0, sizeof(_pgen_tmp_18_51));
  memset(&_pgen_tmp_19_52, 0, sizeof(_pgen_tmp_19_52));
  memset(&_pgen_tmp_20_53, 0, sizeof(_pgen_tmp_20_53));
  memset(&_pgen_tmp_21_54, 0, sizeof(_pgen_tmp_21_54));
  memset(&_pgen_tmp_22_55, 0, sizeof(_pgen_tmp_22_55));
  memset(&_pgen_tmp_23_56, 0, sizeof(_pgen_tmp_23_56));
  memset(&_pgen_tmp_24_57, 0, sizeof(_pgen_tmp_24_57));
  memset(&_pgen_tmp_25_58, 0, sizeof(_pgen_tmp_25_58));
  memset(&_pgen_tmp_26_59, 0, sizeof(_pgen_tmp_26_59));
  memset(&_pgen_tmp_27_60, 0, sizeof(_pgen_tmp_27_60));
  memset(&_pgen_tmp_28_61, 0, sizeof(_pgen_tmp_28_61));
  memset(&_pgen_tmp_29_62, 0, sizeof(_pgen_tmp_29_62));
  memset(&_pgen_tmp_30_63, 0, sizeof(_pgen_tmp_30_63));
  memset(&_pgen_tmp_31_64, 0, sizeof(_pgen_tmp_31_64));
  memset(&_pgen_tmp_32_65, 0, sizeof(_pgen_tmp_32_65));
  memset(&_pgen_tmp_33_66, 0, sizeof(_pgen_tmp_33_66));
  memset(&_pgen_tmp_34_67, 0, sizeof(_pgen_tmp_34_67));
  memset(&_pgen_tmp_35_68, 0, sizeof(_pgen_tmp_35_68));
  memset(&_pgen_tmp_36_69, 0, sizeof(_pgen_tmp_36_69));
  memset(&_pgen_tmp_37_70, 0, sizeof(_pgen_tmp_37_70));
  memset(&_pgen_tmp_38_71, 0, sizeof(_pgen_tmp_38_71));
  memset(&_pgen_tmp_39_72, 0, sizeof(_pgen_tmp_39_72));
  memset(&_pgen_tmp_40_73, 0, sizeof(_pgen_tmp_40_73));
  memset(&_pgen_tmp_41_74, 0, sizeof(_pgen_tmp_41_74));
  memset(&_pgen_tmp_42_75, 0, sizeof(_pgen_tmp_42_75));
  memset(&_pgen_tmp_43_76, 0, sizeof(_pgen_tmp_43_76));
  memset(&_pgen_tmp_44_77, 0, sizeof(_pgen_tmp_44_77));
  memset(&_pgen_tmp_45_78, 0, sizeof(_pgen_tmp_45_78));
  memset(&_pgen_tmp_46_79, 0, sizeof(_pgen_tmp_46_79));
  memset(&_pgen_tmp_47_80, 0, sizeof(_pgen_tmp_47_80));
  memset(&_pgen_tmp_48_81, 0, sizeof(_pgen_tmp_48_81));
  memset(&_pgen_tmp_49_82, 0, sizeof(_pgen_tmp_49_82));
  memset(&_pgen_tmp_50_83, 0, sizeof(_pgen_tmp_50_83));
  memset(&_pgen_tmp_51_84, 0, sizeof(_pgen_tmp_51_84));
  memset(&_pgen_tmp_52_85, 0, sizeof(_pgen_tmp_52_85));
  memset(&_pgen_tmp_53_86, 0, sizeof(_pgen_tmp_53_86));
  memset(&_pgen_tmp_54_87, 0, sizeof(_pgen_tmp_54_87));
  memset(&_pgen_tmp_55_88, 0, sizeof(_pgen_tmp_55_88));
  memset(&_pgen_tmp_56_89, 0, sizeof(_pgen_tmp_56_89));
  memset(&_pgen_tmp_57_90, 0, sizeof(_pgen_tmp_57_90));
  memset(&_pgen_tmp_58_91, 0, sizeof(_pgen_tmp_58_91));
  memset(&_pgen_tmp_59_92, 0, sizeof(_pgen_tmp_59_92));
  memset(&_pgen_tmp_60_93, 0, sizeof(_pgen_tmp_60_93));
  memset(&_pgen_tmp_61_94, 0, sizeof(_pgen_tmp_61_94));
  memset(&_pgen_tmp_62_95, 0, sizeof(_pgen_tmp_62_95));
  memset(&_pgen_tmp_63_96, 0, sizeof(_pgen_tmp_63_96));
  memset(&_pgen_tmp_64_97, 0, sizeof(_pgen_tmp_64_97));
  memset(&_pgen_tmp_65_98, 0, sizeof(_pgen_tmp_65_98));
  memset(&_pgen_tmp_66_99, 0, sizeof(_pgen_tmp_66_99));
  memset(&_pgen_tmp_67_100, 0, sizeof(_pgen_tmp_67_100));
  memset(&_pgen_tmp_68_101, 0, sizeof(_pgen_tmp_68_101));
  memset(&_pgen_tmp_69_102, 0, sizeof(_pgen_tmp_69_102));
  memset(&_pgen_tmp_70_103, 0, sizeof(_pgen_tmp_70_103));
  memset(&_pgen_tmp_71_104, 0, sizeof(_pgen_tmp_71_104));
  memset(&_pgen_tmp_72_105, 0, sizeof(_pgen_tmp_72_105));
  memset(&_pgen_tmp_73_106, 0, sizeof(_pgen_tmp_73_106));
  memset(&_pgen_tmp_74_107, 0, sizeof(_pgen_tmp_74_107));
  memset(&_pgen_tmp_75_108, 0, sizeof(_pgen_tmp_75_108));
  memset(&_pgen_tmp_76_109, 0, sizeof(_pgen_tmp_76_109));
  memset(&_pgen_tmp_77_110, 0, sizeof(_pgen_tmp_77_110));
  memset(&_pgen_tmp_78_111, 0, sizeof(_pgen_tmp_78_111));
  memset(&_pgen_tmp_79_112, 0, sizeof(_pgen_tmp_79_112));
  memset(&_pgen_tmp_80_113, 0, sizeof(_pgen_tmp_80_113));
  memset(&_pgen_tmp_81_114, 0, sizeof(_pgen_tmp_81_114));
  memset(&_pgen_tmp_82_115, 0, sizeof(_pgen_tmp_82_115));
  memset(&_pgen_tmp_83_116, 0, sizeof(_pgen_tmp_83_116));
  memset(&_pgen_tmp_84_117, 0, sizeof(_pgen_tmp_84_117));
  memset(&_pgen_tmp_85_118, 0, sizeof(_pgen_tmp_85_118));
  memset(&_pgen_tmp_86_119, 0, sizeof(_pgen_tmp_86_119));
  memset(&_pgen_tmp_87_120, 0, sizeof(_pgen_tmp_87_120));
  memset(&_pgen_tmp_88_121, 0, sizeof(_pgen_tmp_88_121));
  memset(&_pgen_tmp_89_122, 0, sizeof(_pgen_tmp_89_122));
  memset(&_pgen_tmp_90_123, 0, sizeof(_pgen_tmp_90_123));
  memset(&_pgen_tmp_91_124, 0, sizeof(_pgen_tmp_91_124));
  memset(&_pgen_tmp_92_125, 0, sizeof(_pgen_tmp_92_125));
  memset(&_pgen_tmp_93_126, 0, sizeof(_pgen_tmp_93_126));
  memset(&_pgen_tmp_94_127, 0, sizeof(_pgen_tmp_94_127));
  memset(&_pgen_tmp_95_128, 0, sizeof(_pgen_tmp_95_128));
  memset(&_pgen_tmp_96_129, 0, sizeof(_pgen_tmp_96_129));
  memset(&_pgen_tmp_97_130, 0, sizeof(_pgen_tmp_97_130));
  memset(&_pgen_tmp_98_131, 0, sizeof(_pgen_tmp_98_131));
  memset(&_pgen_tmp_99_132, 0, sizeof(_pgen_tmp_99_132));
  memset(&_pgen_tmp_100_133, 0, sizeof(_pgen_tmp_100_133));
  memset(&_pgen_tmp_101_134, 0, sizeof(_pgen_tmp_101_134));
  memset(&_pgen_tmp_102_135, 0, sizeof(_pgen_tmp_102_135));
  memset(&_pgen_tmp_103_136, 0, sizeof(_pgen_tmp_103_136));
  memset(&_pgen_tmp_104_137, 0, sizeof(_pgen_tmp_104_137));
  memset(&_pgen_tmp_105_138, 0, sizeof(_pgen_tmp_105_138));
  memset(&_pgen_tmp_106_139, 0, sizeof(_pgen_tmp_106_139));
  memset(&_pgen_tmp_107_140, 0, sizeof(_pgen_tmp_107_140));
  memset(&_pgen_tmp_108_141, 0, sizeof(_pgen_tmp_108_141));
  memset(&_pgen_tmp_109_142, 0, sizeof(_pgen_tmp_109_142));
  memset(&_pgen_tmp_110_143, 0, sizeof(_pgen_tmp_110_143));
  memset(&_pgen_tmp_111_144, 0, sizeof(_pgen_tmp_111_144));
  memset(&_pgen_tmp_112_145, 0, sizeof(_pgen_tmp_112_145));
  memset(&_pgen_tmp_113_146, 0, sizeof(_pgen_tmp_113_146));
  memset(&_pgen_tmp_114_147, 0, sizeof(_pgen_tmp_114_147));
  memset(&_pgen_tmp_115_148, 0, sizeof(_pgen_tmp_115_148));
  memset(&_pgen_tmp_116_149, 0, sizeof(_pgen_tmp_116_149));
  memset(&_pgen_tmp_117_150, 0, sizeof(_pgen_tmp_117_150));
  memset(&_pgen_tmp_118_151, 0, sizeof(_pgen_tmp_118_151));
  memset(&_pgen_tmp_119_152, 0, sizeof(_pgen_tmp_119_152));
  memset(&_pgen_tmp_120_153, 0, sizeof(_pgen_tmp_120_153));
  memset(&_pgen_tmp_121_154, 0, sizeof(_pgen_tmp_121_154));
  memset(&_pgen_tmp_122_155, 0, sizeof(_pgen_tmp_122_155));
  memset(&_pgen_tmp_123_156, 0, sizeof(_pgen_tmp_123_156));
  memset(&_pgen_tmp_124_157, 0, sizeof(_pgen_tmp_124_157));
  memset(&_pgen_tmp_125_158, 0, sizeof(_pgen_tmp_125_158));
  memset(&_pgen_tmp_126_159, 0, sizeof(_pgen_tmp_126_159));
  memset(&_pgen_tmp_127_160, 0, sizeof(_pgen_tmp_127_160));
  memset(&_pgen_tmp_128_161, 0, sizeof(_pgen_tmp_128_161));
  memset(&_pgen_tmp_129_162, 0, sizeof(_pgen_tmp_129_162));
  memset(&_pgen_tmp_130_163, 0, sizeof(_pgen_tmp_130_163));
  memset(&_pgen_tmp_131_164, 0, sizeof(_pgen_tmp_131_164));
  memset(&_pgen_tmp_132_165, 0, sizeof(_pgen_tmp_132_165));
  memset(&_pgen_tmp_133_166, 0, sizeof(_pgen_tmp_133_166));
  memset(&_pgen_tmp_134_167, 0, sizeof(_pgen_tmp_134_167));
  memset(&_pgen_tmp_135_168, 0, sizeof(_pgen_tmp_135_168));
  memset(&_pgen_tmp_136_169, 0, sizeof(_pgen_tmp_136_169));
  memset(&_pgen_tmp_137_170, 0, sizeof(_pgen_tmp_137_170));
  memset(&_pgen_tmp_138_171, 0, sizeof(_pgen_tmp_138_171));
  memset(&_pgen_tmp_139_172, 0, sizeof(_pgen_tmp_139_172));
  memset(&_pgen_tmp_140_173, 0, sizeof(_pgen_tmp_140_173));
  memset(&_pgen_tmp_141_174, 0, sizeof(_pgen_tmp_141_174));
  memset(&_pgen_tmp_142_175, 0, sizeof(_pgen_tmp_142_175));
  memset(&_pgen_tmp_143_176, 0, sizeof(_pgen_tmp_143_176));
  memset(&_pgen_tmp_144_177, 0, sizeof(_pgen_tmp_144_177));
  memset(&_pgen_tmp_145_178, 0, sizeof(_pgen_tmp_145_178));
  memset(&_pgen_tmp_146_179, 0, sizeof(_pgen_tmp_146_179));
  memset(&_pgen_tmp_147_180, 0, sizeof(_pgen_tmp_147_180));
  memset(&_pgen_tmp_148_181, 0, sizeof(_pgen_tmp_148_181));
  memset(&_pgen_tmp_149_182, 0, sizeof(_pgen_tmp_149_182));
  memset(&_pgen_tmp_150_183, 0, sizeof(_pgen_tmp_150_183));
  memset(&_pgen_tmp_151_184, 0, sizeof(_pgen_tmp_151_184));
  memset(&_pgen_tmp_152_185, 0, sizeof(_pgen_tmp_152_185));
  memset(&_pgen_tmp_153_186, 0, sizeof(_pgen_tmp_153_186));
  memset(&_pgen_tmp_154_187, 0, sizeof(_pgen_tmp_154_187));
  memset(&_pgen_tmp_155_188, 0, sizeof(_pgen_tmp_155_188));
  memset(&_pgen_tmp_156_189, 0, sizeof(_pgen_tmp_156_189));
  memset(&_pgen_tmp_157_190, 0, sizeof(_pgen_tmp_157_190));
  memset(&_pgen_tmp_158_191, 0, sizeof(_pgen_tmp_158_191));
  memset(&_pgen_tmp_159_192, 0, sizeof(_pgen_tmp_159_192));
  memset(&_pgen_tmp_160_193, 0, sizeof(_pgen_tmp_160_193));
  memset(&_pgen_tmp_161_194, 0, sizeof(_pgen_tmp_161_194));
  memset(&_pgen_tmp_162_195, 0, sizeof(_pgen_tmp_162_195));
  memset(&_pgen_tmp_163_196, 0, sizeof(_pgen_tmp_163_196));
  memset(&_pgen_tmp_164_197, 0, sizeof(_pgen_tmp_164_197));
  memset(&_pgen_tmp_165_198, 0, sizeof(_pgen_tmp_165_198));
  memset(&_pgen_tmp_166_199, 0, sizeof(_pgen_tmp_166_199));
  memset(&_pgen_tmp_167_200, 0, sizeof(_pgen_tmp_167_200));
  memset(&_pgen_tmp_168_201, 0, sizeof(_pgen_tmp_168_201));
  memset(&_pgen_tmp_169_202, 0, sizeof(_pgen_tmp_169_202));
  memset(&_pgen_tmp_170_203, 0, sizeof(_pgen_tmp_170_203));
  memset(&_pgen_tmp_171_204, 0, sizeof(_pgen_tmp_171_204));
  memset(&_pgen_tmp_172_205, 0, sizeof(_pgen_tmp_172_205));
  memset(&_pgen_tmp_173_206, 0, sizeof(_pgen_tmp_173_206));
  memset(&_pgen_tmp_174_207, 0, sizeof(_pgen_tmp_174_207));
  memset(&_pgen_tmp_175_208, 0, sizeof(_pgen_tmp_175_208));
  memset(&_pgen_tmp_176_209, 0, sizeof(_pgen_tmp_176_209));
  memset(&_pgen_tmp_177_210, 0, sizeof(_pgen_tmp_177_210));
  memset(&_pgen_tmp_178_211, 0, sizeof(_pgen_tmp_178_211));
  memset(&_pgen_tmp_179_212, 0, sizeof(_pgen_tmp_179_212));
  memset(&_pgen_tmp_180_213, 0, sizeof(_pgen_tmp_180_213));
  memset(&_pgen_tmp_181_214, 0, sizeof(_pgen_tmp_181_214));
  memset(&_pgen_tmp_182_215, 0, sizeof(_pgen_tmp_182_215));
  memset(&_pgen_tmp_183_216, 0, sizeof(_pgen_tmp_183_216));
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
  memset(&_preg_268435467, 0, sizeof(_preg_268435467));
  memset(&_preg_268435468, 0, sizeof(_preg_268435468));
  memset(&_preg_268435469, 0, sizeof(_preg_268435469));
  memset(&_preg_268435470, 0, sizeof(_preg_268435470));
  memset(&_preg_268435471, 0, sizeof(_preg_268435471));
  memset(&_preg_268435472, 0, sizeof(_preg_268435472));
  memset(&_preg_268435473, 0, sizeof(_preg_268435473));
  memset(&_preg_268435474, 0, sizeof(_preg_268435474));
  memset(&_preg_268435475, 0, sizeof(_preg_268435475));
  memset(&_preg_268435476, 0, sizeof(_preg_268435476));
  memset(&_preg_268435477, 0, sizeof(_preg_268435477));
  memset(&_preg_268435478, 0, sizeof(_preg_268435478));
  memset(&_preg_268435479, 0, sizeof(_preg_268435479));
  memset(&_preg_268435480, 0, sizeof(_preg_268435480));
  memset(&_preg_268435481, 0, sizeof(_preg_268435481));
  memset(&_preg_268435482, 0, sizeof(_preg_268435482));
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_0_30, &input_ct1_1, &input_ct1_1);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_0_30._c0_poly);
  Init_poly_by_size(&_pgen_tmp_1_34, 1, 0);
  Init_poly_by_size(&_pgen_tmp_2_35, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_1_34, 0, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_2_35, 0, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_0_30._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_0_30._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_1_34, 0, degree), Coeffs(&_pgen_tmp_2_35, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_0_30._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_1_34);
  Free_data(&_pgen_tmp_1_34);
  Free_data(&_pgen_tmp_2_35);
  Free_data(&_pgen_tmp_2_35);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_3_36, &_pgen_tmp_0_30);
  Rescale(&_pgen_tmp_3_36._c0_poly, &_pgen_tmp_0_30._c0_poly);
  Rescale(&_pgen_tmp_3_36._c1_poly, &_pgen_tmp_0_30._c1_poly);
  Rescale(&_pgen_tmp_3_36._c2_poly, &_pgen_tmp_0_30._c2_poly);
  Free_data(&_pgen_tmp_0_30._c2_poly);
  Free_data(&_pgen_tmp_0_30._c1_poly);
  Free_data(&_pgen_tmp_0_30._c0_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435456 = Relinearize(_pgen_tmp_3_36);
  Free_data(&_pgen_tmp_3_36._c2_poly);
  Free_data(&_pgen_tmp_3_36._c1_poly);
  Free_data(&_pgen_tmp_3_36._c0_poly);
  Copy_ciph(&_relu_tmp8_3, &_preg_268435456);
  Free_data(&_preg_268435456._c1_poly);
  Free_data(&_preg_268435456._c0_poly);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_4_37, &_relu_tmp8_3, &_relu_tmp8_3);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_5_38, &_cst_132, 1, Sc_degree(&_pgen_tmp_4_37), Level(&_pgen_tmp_4_37), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp14_4, &_pgen_tmp_4_37, &_pgen_tmp_5_38);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_relu_tmp14_4._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_4_37._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp8_3._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp8_3._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_4_37._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp8_3._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp8_3._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_4_37._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_5_38._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Set_coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_33, degree, Coeffs(&_pgen_tmp_4_37._c1_poly, _pgen_rns_idx_33, degree));
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_relu_tmp8_3._c1_poly);
  Free_data(&_relu_tmp8_3._c0_poly);
  Free_data(&_pgen_tmp_5_38._poly);
  Free_data(&_pgen_tmp_4_37._c1_poly);
  Free_data(&_pgen_tmp_4_37._c0_poly);
  CMPLR_TM_END(13, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_6_39, &_relu_tmp14_4, &_relu_tmp14_4);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_6_39._c0_poly);
  Init_poly_by_size(&_pgen_tmp_7_40, 1, 0);
  Init_poly_by_size(&_pgen_tmp_8_41, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_7_40, 0, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_8_41, 0, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_6_39._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_6_39._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_7_40, 0, degree), Coeffs(&_pgen_tmp_8_41, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_6_39._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_7_40);
  Free_data(&_pgen_tmp_7_40);
  Free_data(&_pgen_tmp_8_41);
  Free_data(&_pgen_tmp_8_41);
  CMPLR_TM_END(16, 0);
  _preg_268435457 = Relinearize(_pgen_tmp_6_39);
  Free_data(&_pgen_tmp_6_39._c2_poly);
  Free_data(&_pgen_tmp_6_39._c1_poly);
  Free_data(&_pgen_tmp_6_39._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_relu_tmp18_5, &_preg_268435457);
  Rescale(&_relu_tmp18_5._c0_poly, &_preg_268435457._c0_poly);
  Rescale(&_relu_tmp18_5._c1_poly, &_preg_268435457._c1_poly);
  Free_data(&_preg_268435457._c1_poly);
  Free_data(&_preg_268435457._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_9_42, &_relu_tmp18_5, &_relu_tmp18_5);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_10_43, &_cst_133, 1, Sc_degree(&_pgen_tmp_9_42), Level(&_pgen_tmp_9_42), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp24_6, &_pgen_tmp_9_42, &_pgen_tmp_10_43);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_relu_tmp24_6._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_9_42._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp18_5._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp18_5._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_9_42._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp18_5._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp18_5._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_relu_tmp24_6._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_9_42._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_10_43._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Set_coeffs(&_relu_tmp24_6._c1_poly, _pgen_rns_idx_33, degree, Coeffs(&_pgen_tmp_9_42._c1_poly, _pgen_rns_idx_33, degree));
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_relu_tmp18_5._c1_poly);
  Free_data(&_relu_tmp18_5._c0_poly);
  Free_data(&_pgen_tmp_10_43._poly);
  Free_data(&_pgen_tmp_9_42._c1_poly);
  Free_data(&_pgen_tmp_9_42._c0_poly);
  CMPLR_TM_END(13, 0);
  Encode_float_mask(&_pgen_tmp_16_49, -3.809199432496630510769, mask_len_2, 1, Level(&input_ct1_1), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_15_48, &input_ct1_1, &_pgen_tmp_16_49);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_15_48._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_15_48._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_16_49._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_15_48._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_16_49._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_16_49._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_17_50, &_pgen_tmp_15_48);
  Rescale(&_pgen_tmp_17_50._c0_poly, &_pgen_tmp_15_48._c0_poly);
  Rescale(&_pgen_tmp_17_50._c1_poly, &_pgen_tmp_15_48._c1_poly);
  Free_data(&_pgen_tmp_15_48._c1_poly);
  Free_data(&_pgen_tmp_15_48._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_14_47, &_relu_tmp14_4, &_pgen_tmp_17_50);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_14_47._c0_poly);
  Init_poly_by_size(&_pgen_tmp_18_51, 1, 0);
  Init_poly_by_size(&_pgen_tmp_19_52, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_18_51, 0, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_17_50._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_19_52, 0, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_17_50._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_14_47._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_17_50._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_14_47._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_18_51, 0, degree), Coeffs(&_pgen_tmp_19_52, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_14_47._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_17_50._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_17_50._c1_poly);
  Free_data(&_pgen_tmp_17_50._c0_poly);
  Free_data(&_pgen_tmp_18_51);
  Free_data(&_pgen_tmp_18_51);
  Free_data(&_pgen_tmp_19_52);
  Free_data(&_pgen_tmp_19_52);
  CMPLR_TM_END(16, 0);
  _preg_268435458 = Relinearize(_pgen_tmp_14_47);
  Free_data(&_pgen_tmp_14_47._c2_poly);
  Free_data(&_pgen_tmp_14_47._c1_poly);
  Free_data(&_pgen_tmp_14_47._c0_poly);
  Encode_float_mask(&_pgen_tmp_20_53, 2.460940868770910228136, mask_len_2, 1, Level(&input_ct1_1), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_21_54, &input_ct1_1, &_pgen_tmp_20_53);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_13_46, &_preg_268435458, &_pgen_tmp_21_54);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_13_46._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_21_54._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_20_53._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_21_54._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_20_53._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_13_46._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435458._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_21_54._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_13_46._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435458._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_21_54._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_20_53._poly);
  Free_data(&_pgen_tmp_21_54._c1_poly);
  Free_data(&_pgen_tmp_21_54._c0_poly);
  Free_data(&_preg_268435458._c1_poly);
  Free_data(&_preg_268435458._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_22_55, &_pgen_tmp_13_46);
  Rescale(&_pgen_tmp_22_55._c0_poly, &_pgen_tmp_13_46._c0_poly);
  Rescale(&_pgen_tmp_22_55._c1_poly, &_pgen_tmp_13_46._c1_poly);
  Free_data(&_pgen_tmp_13_46._c1_poly);
  Free_data(&_pgen_tmp_13_46._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_12_45, &_relu_tmp24_6, &_pgen_tmp_22_55);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_12_45._c0_poly);
  Init_poly_by_size(&_pgen_tmp_23_56, 1, 0);
  Init_poly_by_size(&_pgen_tmp_24_57, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_23_56, 0, degree), Coeffs(&_relu_tmp24_6._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_22_55._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_24_57, 0, degree), Coeffs(&_relu_tmp24_6._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_22_55._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_12_45._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp24_6._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_22_55._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_12_45._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_23_56, 0, degree), Coeffs(&_pgen_tmp_24_57, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_12_45._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp24_6._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_22_55._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_22_55._c1_poly);
  Free_data(&_pgen_tmp_22_55._c0_poly);
  Free_data(&_relu_tmp24_6._c1_poly);
  Free_data(&_relu_tmp24_6._c0_poly);
  Free_data(&_pgen_tmp_23_56);
  Free_data(&_pgen_tmp_23_56);
  Free_data(&_pgen_tmp_24_57);
  Free_data(&_pgen_tmp_24_57);
  CMPLR_TM_END(16, 0);
  _preg_268435459 = Relinearize(_pgen_tmp_12_45);
  Free_data(&_pgen_tmp_12_45._c2_poly);
  Free_data(&_pgen_tmp_12_45._c1_poly);
  Free_data(&_pgen_tmp_12_45._c0_poly);
  Encode_float_mask(&_pgen_tmp_27_60, -1.430304794543664215212, mask_len_2, 1, Level(&input_ct1_1), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_26_59, &input_ct1_1, &_pgen_tmp_27_60);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_26_59._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_26_59._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_27_60._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_26_59._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_27_60._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_27_60._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_28_61, &_pgen_tmp_26_59);
  Rescale(&_pgen_tmp_28_61._c0_poly, &_pgen_tmp_26_59._c0_poly);
  Rescale(&_pgen_tmp_28_61._c1_poly, &_pgen_tmp_26_59._c1_poly);
  Free_data(&_pgen_tmp_26_59._c1_poly);
  Free_data(&_pgen_tmp_26_59._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_25_58, &_relu_tmp14_4, &_pgen_tmp_28_61);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_25_58._c0_poly);
  Init_poly_by_size(&_pgen_tmp_29_62, 1, 0);
  Init_poly_by_size(&_pgen_tmp_30_63, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_29_62, 0, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_28_61._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_30_63, 0, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_28_61._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_25_58._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_28_61._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_25_58._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_29_62, 0, degree), Coeffs(&_pgen_tmp_30_63, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_25_58._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_28_61._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_28_61._c1_poly);
  Free_data(&_pgen_tmp_28_61._c0_poly);
  Free_data(&_relu_tmp14_4._c1_poly);
  Free_data(&_relu_tmp14_4._c0_poly);
  Free_data(&_pgen_tmp_29_62);
  Free_data(&_pgen_tmp_29_62);
  Free_data(&_pgen_tmp_30_63);
  Free_data(&_pgen_tmp_30_63);
  CMPLR_TM_END(16, 0);
  _preg_268435460 = Relinearize(_pgen_tmp_25_58);
  Free_data(&_pgen_tmp_25_58._c2_poly);
  Free_data(&_pgen_tmp_25_58._c1_poly);
  Free_data(&_pgen_tmp_25_58._c0_poly);
  Encode_float_mask(&_pgen_tmp_31_64, 2.944661935353764636858, mask_len_2, 1, Level(&input_ct1_1), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_32_65, &input_ct1_1, &_pgen_tmp_31_64);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_33_66, &_preg_268435460, &_pgen_tmp_32_65);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_11_44, &_preg_268435459, &_pgen_tmp_33_66);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_11_44._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_32_65._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_31_64._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_32_65._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_31_64._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_33_66._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435460._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_32_65._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_33_66._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435460._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_32_65._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_11_44._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435459._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_33_66._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_11_44._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435459._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_33_66._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_31_64._poly);
  Free_data(&_pgen_tmp_32_65._c1_poly);
  Free_data(&_pgen_tmp_32_65._c0_poly);
  Free_data(&_preg_268435460._c1_poly);
  Free_data(&_preg_268435460._c0_poly);
  Free_data(&_pgen_tmp_33_66._c1_poly);
  Free_data(&_pgen_tmp_33_66._c0_poly);
  Free_data(&_preg_268435459._c1_poly);
  Free_data(&_preg_268435459._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_relu_tmp58_7, &_pgen_tmp_11_44);
  Rescale(&_relu_tmp58_7._c0_poly, &_pgen_tmp_11_44._c0_poly);
  Rescale(&_relu_tmp58_7._c1_poly, &_pgen_tmp_11_44._c1_poly);
  Free_data(&_pgen_tmp_11_44._c1_poly);
  Free_data(&_pgen_tmp_11_44._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_34_67, &_relu_tmp58_7, &_relu_tmp58_7);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_34_67._c0_poly);
  Init_poly_by_size(&_pgen_tmp_35_68, 1, 0);
  Init_poly_by_size(&_pgen_tmp_36_69, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_35_68, 0, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_36_69, 0, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_34_67._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_34_67._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_35_68, 0, degree), Coeffs(&_pgen_tmp_36_69, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_34_67._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_35_68);
  Free_data(&_pgen_tmp_35_68);
  Free_data(&_pgen_tmp_36_69);
  Free_data(&_pgen_tmp_36_69);
  CMPLR_TM_END(16, 0);
  _preg_268435461 = Relinearize(_pgen_tmp_34_67);
  Free_data(&_pgen_tmp_34_67._c2_poly);
  Free_data(&_pgen_tmp_34_67._c1_poly);
  Free_data(&_pgen_tmp_34_67._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_relu_tmp62_8, &_preg_268435461);
  Rescale(&_relu_tmp62_8._c0_poly, &_preg_268435461._c0_poly);
  Rescale(&_relu_tmp62_8._c1_poly, &_preg_268435461._c1_poly);
  Free_data(&_preg_268435461._c1_poly);
  Free_data(&_preg_268435461._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_37_70, &_relu_tmp62_8, &_relu_tmp62_8);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_38_71, &_cst_142, 1, Sc_degree(&_pgen_tmp_37_70), Level(&_pgen_tmp_37_70), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp68_9, &_pgen_tmp_37_70, &_pgen_tmp_38_71);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_relu_tmp68_9._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_37_70._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp62_8._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp62_8._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_37_70._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp62_8._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp62_8._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_37_70._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_38_71._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Set_coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_33, degree, Coeffs(&_pgen_tmp_37_70._c1_poly, _pgen_rns_idx_33, degree));
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_relu_tmp62_8._c1_poly);
  Free_data(&_relu_tmp62_8._c0_poly);
  Free_data(&_pgen_tmp_38_71._poly);
  Free_data(&_pgen_tmp_37_70._c1_poly);
  Free_data(&_pgen_tmp_37_70._c0_poly);
  CMPLR_TM_END(13, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_39_72, &_relu_tmp58_7, &_relu_tmp68_9);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_39_72._c0_poly);
  Init_poly_by_size(&_pgen_tmp_40_73, 1, 0);
  Init_poly_by_size(&_pgen_tmp_41_74, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_40_73, 0, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_41_74, 0, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_39_72._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_39_72._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_40_73, 0, degree), Coeffs(&_pgen_tmp_41_74, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_39_72._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_40_73);
  Free_data(&_pgen_tmp_40_73);
  Free_data(&_pgen_tmp_41_74);
  Free_data(&_pgen_tmp_41_74);
  CMPLR_TM_END(16, 0);
  _preg_268435462 = Relinearize(_pgen_tmp_39_72);
  Free_data(&_pgen_tmp_39_72._c2_poly);
  Free_data(&_pgen_tmp_39_72._c1_poly);
  Free_data(&_pgen_tmp_39_72._c0_poly);
  Copy_ciph(&_relu_tmp72_10, &_preg_268435462);
  Free_data(&_preg_268435462._c1_poly);
  Free_data(&_preg_268435462._c0_poly);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_43_76, &_relu_tmp72_10, &_relu_tmp72_10);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_44_77, &_cst_143, 1, 1, Level(&_relu_tmp58_7), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_45_78, &_relu_tmp58_7, &_pgen_tmp_44_77);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_42_75, &_pgen_tmp_43_76, &_pgen_tmp_45_78);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_42_75._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_43_76._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp72_10._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp72_10._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_43_76._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp72_10._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp72_10._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_45_78._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_44_77._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_45_78._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_44_77._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_42_75._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_43_76._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_45_78._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_42_75._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_43_76._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_45_78._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_relu_tmp72_10._c1_poly);
  Free_data(&_relu_tmp72_10._c0_poly);
  Free_data(&_pgen_tmp_44_77._poly);
  Free_data(&_pgen_tmp_45_78._c1_poly);
  Free_data(&_pgen_tmp_45_78._c0_poly);
  Free_data(&_pgen_tmp_43_76._c1_poly);
  Free_data(&_pgen_tmp_43_76._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_relu_tmp80_11, &_pgen_tmp_42_75);
  Rescale(&_relu_tmp80_11._c0_poly, &_pgen_tmp_42_75._c0_poly);
  Rescale(&_relu_tmp80_11._c1_poly, &_pgen_tmp_42_75._c1_poly);
  Free_data(&_pgen_tmp_42_75._c1_poly);
  Free_data(&_pgen_tmp_42_75._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_46_79, &_relu_tmp68_9, &_relu_tmp68_9);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_46_79._c0_poly);
  Init_poly_by_size(&_pgen_tmp_47_80, 1, 0);
  Init_poly_by_size(&_pgen_tmp_48_81, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_47_80, 0, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_48_81, 0, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_46_79._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_46_79._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_47_80, 0, degree), Coeffs(&_pgen_tmp_48_81, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_46_79._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_47_80);
  Free_data(&_pgen_tmp_47_80);
  Free_data(&_pgen_tmp_48_81);
  Free_data(&_pgen_tmp_48_81);
  CMPLR_TM_END(16, 0);
  _preg_268435463 = Relinearize(_pgen_tmp_46_79);
  Free_data(&_pgen_tmp_46_79._c2_poly);
  Free_data(&_pgen_tmp_46_79._c1_poly);
  Free_data(&_pgen_tmp_46_79._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_relu_tmp84_12, &_preg_268435463);
  Rescale(&_relu_tmp84_12._c0_poly, &_preg_268435463._c0_poly);
  Rescale(&_relu_tmp84_12._c1_poly, &_preg_268435463._c1_poly);
  Free_data(&_preg_268435463._c1_poly);
  Free_data(&_preg_268435463._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_49_82, &_relu_tmp84_12, &_relu_tmp84_12);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_50_83, &_cst_144, 1, Sc_degree(&_pgen_tmp_49_82), Level(&_pgen_tmp_49_82), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp90_13, &_pgen_tmp_49_82, &_pgen_tmp_50_83);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_relu_tmp90_13._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_49_82._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp84_12._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp84_12._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_49_82._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp84_12._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp84_12._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_49_82._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_50_83._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Set_coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_33, degree, Coeffs(&_pgen_tmp_49_82._c1_poly, _pgen_rns_idx_33, degree));
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_relu_tmp84_12._c1_poly);
  Free_data(&_relu_tmp84_12._c0_poly);
  Free_data(&_pgen_tmp_50_83._poly);
  Free_data(&_pgen_tmp_49_82._c1_poly);
  Free_data(&_pgen_tmp_49_82._c0_poly);
  CMPLR_TM_END(13, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_51_84, &_relu_tmp90_13, &_relu_tmp90_13);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_51_84._c0_poly);
  Init_poly_by_size(&_pgen_tmp_52_85, 1, 0);
  Init_poly_by_size(&_pgen_tmp_53_86, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_52_85, 0, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_53_86, 0, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_51_84._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_51_84._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_52_85, 0, degree), Coeffs(&_pgen_tmp_53_86, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_51_84._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_52_85);
  Free_data(&_pgen_tmp_52_85);
  Free_data(&_pgen_tmp_53_86);
  Free_data(&_pgen_tmp_53_86);
  CMPLR_TM_END(16, 0);
  _preg_268435464 = Relinearize(_pgen_tmp_51_84);
  Free_data(&_pgen_tmp_51_84._c2_poly);
  Free_data(&_pgen_tmp_51_84._c1_poly);
  Free_data(&_pgen_tmp_51_84._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_relu_tmp94_14, &_preg_268435464);
  Rescale(&_relu_tmp94_14._c0_poly, &_preg_268435464._c0_poly);
  Rescale(&_relu_tmp94_14._c1_poly, &_preg_268435464._c1_poly);
  Free_data(&_preg_268435464._c1_poly);
  Free_data(&_preg_268435464._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_54_87, &_relu_tmp94_14, &_relu_tmp94_14);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_55_88, &_cst_145, 1, Sc_degree(&_pgen_tmp_54_87), Level(&_pgen_tmp_54_87), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp100_15, &_pgen_tmp_54_87, &_pgen_tmp_55_88);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_relu_tmp100_15._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_54_87._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp94_14._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp94_14._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_54_87._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp94_14._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp94_14._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_relu_tmp100_15._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_54_87._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_55_88._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Set_coeffs(&_relu_tmp100_15._c1_poly, _pgen_rns_idx_33, degree, Coeffs(&_pgen_tmp_54_87._c1_poly, _pgen_rns_idx_33, degree));
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_relu_tmp94_14._c1_poly);
  Free_data(&_relu_tmp94_14._c0_poly);
  Free_data(&_pgen_tmp_55_88._poly);
  Free_data(&_pgen_tmp_54_87._c1_poly);
  Free_data(&_pgen_tmp_54_87._c0_poly);
  CMPLR_TM_END(13, 0);
  Encode_float_mask(&_pgen_tmp_63_96, -0.0001600280537250875771867, mask_len_2, 1, Level(&_relu_tmp58_7), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_62_95, &_relu_tmp58_7, &_pgen_tmp_63_96);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_62_95._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_62_95._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_63_96._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_62_95._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_63_96._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_63_96._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_64_97, &_pgen_tmp_62_95);
  Rescale(&_pgen_tmp_64_97._c0_poly, &_pgen_tmp_62_95._c0_poly);
  Rescale(&_pgen_tmp_64_97._c1_poly, &_pgen_tmp_62_95._c1_poly);
  Free_data(&_pgen_tmp_62_95._c1_poly);
  Free_data(&_pgen_tmp_62_95._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_61_94, &_relu_tmp68_9, &_pgen_tmp_64_97);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_61_94._c0_poly);
  Init_poly_by_size(&_pgen_tmp_65_98, 1, 0);
  Init_poly_by_size(&_pgen_tmp_66_99, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_65_98, 0, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_64_97._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_66_99, 0, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_64_97._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_61_94._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_64_97._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_61_94._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_65_98, 0, degree), Coeffs(&_pgen_tmp_66_99, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_61_94._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_64_97._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_64_97._c1_poly);
  Free_data(&_pgen_tmp_64_97._c0_poly);
  Free_data(&_relu_tmp68_9._c1_poly);
  Free_data(&_relu_tmp68_9._c0_poly);
  Free_data(&_pgen_tmp_65_98);
  Free_data(&_pgen_tmp_65_98);
  Free_data(&_pgen_tmp_66_99);
  Free_data(&_pgen_tmp_66_99);
  CMPLR_TM_END(16, 0);
  _preg_268435465 = Relinearize(_pgen_tmp_61_94);
  Free_data(&_pgen_tmp_61_94._c2_poly);
  Free_data(&_pgen_tmp_61_94._c1_poly);
  Free_data(&_pgen_tmp_61_94._c0_poly);
  Encode_float_mask(&_pgen_tmp_67_100, 0.003032478474127557870749, mask_len_2, 1, Level(&_relu_tmp58_7), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_68_101, &_relu_tmp58_7, &_pgen_tmp_67_100);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_60_93, &_preg_268435465, &_pgen_tmp_68_101);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_60_93._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_68_101._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_67_100._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_68_101._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_67_100._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_60_93._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435465._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_68_101._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_60_93._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435465._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_68_101._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_67_100._poly);
  Free_data(&_pgen_tmp_68_101._c1_poly);
  Free_data(&_pgen_tmp_68_101._c0_poly);
  Free_data(&_preg_268435465._c1_poly);
  Free_data(&_preg_268435465._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_69_102, &_pgen_tmp_60_93);
  Rescale(&_pgen_tmp_69_102._c0_poly, &_pgen_tmp_60_93._c0_poly);
  Rescale(&_pgen_tmp_69_102._c1_poly, &_pgen_tmp_60_93._c1_poly);
  Free_data(&_pgen_tmp_60_93._c1_poly);
  Free_data(&_pgen_tmp_60_93._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_59_92, &_relu_tmp90_13, &_pgen_tmp_69_102);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_59_92._c0_poly);
  Init_poly_by_size(&_pgen_tmp_70_103, 1, 0);
  Init_poly_by_size(&_pgen_tmp_71_104, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_70_103, 0, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_69_102._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_71_104, 0, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_69_102._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_59_92._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_69_102._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_59_92._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_70_103, 0, degree), Coeffs(&_pgen_tmp_71_104, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_59_92._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_69_102._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_69_102._c1_poly);
  Free_data(&_pgen_tmp_69_102._c0_poly);
  Free_data(&_pgen_tmp_70_103);
  Free_data(&_pgen_tmp_70_103);
  Free_data(&_pgen_tmp_71_104);
  Free_data(&_pgen_tmp_71_104);
  CMPLR_TM_END(16, 0);
  _preg_268435466 = Relinearize(_pgen_tmp_59_92);
  Free_data(&_pgen_tmp_59_92._c2_poly);
  Free_data(&_pgen_tmp_59_92._c1_poly);
  Free_data(&_pgen_tmp_59_92._c0_poly);
  Encode_double(&_pgen_tmp_72_105, &_cst_152, 1, 1, Level(&_relu_tmp80_11), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_73_106, &_relu_tmp80_11, &_pgen_tmp_72_105);
  CMPLR_TM_END(17, 0);
  Encode_float_mask(&_pgen_tmp_74_107, 0.1365828361194337536855, mask_len_2, 1, Level(&_relu_tmp58_7), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_75_108, &_relu_tmp58_7, &_pgen_tmp_74_107);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_76_109, &_pgen_tmp_73_106, &_pgen_tmp_75_108);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_58_91, &_preg_268435466, &_pgen_tmp_76_109);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_58_91._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_73_106._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp80_11._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_72_105._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_73_106._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp80_11._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_72_105._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_75_108._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_74_107._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_75_108._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_74_107._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_76_109._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_73_106._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_75_108._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_76_109._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_73_106._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_75_108._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_58_91._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435466._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_76_109._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_58_91._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435466._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_76_109._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_72_105._poly);
  Free_data(&_pgen_tmp_74_107._poly);
  Free_data(&_pgen_tmp_75_108._c1_poly);
  Free_data(&_pgen_tmp_75_108._c0_poly);
  Free_data(&_pgen_tmp_73_106._c1_poly);
  Free_data(&_pgen_tmp_73_106._c0_poly);
  Free_data(&_pgen_tmp_76_109._c1_poly);
  Free_data(&_pgen_tmp_76_109._c0_poly);
  Free_data(&_preg_268435466._c1_poly);
  Free_data(&_preg_268435466._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_77_110, &_pgen_tmp_58_91);
  Rescale(&_pgen_tmp_77_110._c0_poly, &_pgen_tmp_58_91._c0_poly);
  Rescale(&_pgen_tmp_77_110._c1_poly, &_pgen_tmp_58_91._c1_poly);
  Free_data(&_pgen_tmp_58_91._c1_poly);
  Free_data(&_pgen_tmp_58_91._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_57_90, &_relu_tmp100_15, &_pgen_tmp_77_110);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_57_90._c0_poly);
  Init_poly_by_size(&_pgen_tmp_78_111, 1, 0);
  Init_poly_by_size(&_pgen_tmp_79_112, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_78_111, 0, degree), Coeffs(&_relu_tmp100_15._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_77_110._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_79_112, 0, degree), Coeffs(&_relu_tmp100_15._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_77_110._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_57_90._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp100_15._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_77_110._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_57_90._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_78_111, 0, degree), Coeffs(&_pgen_tmp_79_112, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_57_90._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp100_15._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_77_110._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_77_110._c1_poly);
  Free_data(&_pgen_tmp_77_110._c0_poly);
  Free_data(&_relu_tmp100_15._c1_poly);
  Free_data(&_relu_tmp100_15._c0_poly);
  Free_data(&_pgen_tmp_78_111);
  Free_data(&_pgen_tmp_78_111);
  Free_data(&_pgen_tmp_79_112);
  Free_data(&_pgen_tmp_79_112);
  CMPLR_TM_END(16, 0);
  _preg_268435467 = Relinearize(_pgen_tmp_57_90);
  Free_data(&_pgen_tmp_57_90._c2_poly);
  Free_data(&_pgen_tmp_57_90._c1_poly);
  Free_data(&_pgen_tmp_57_90._c0_poly);
  Encode_double(&_pgen_tmp_82_115, &_cst_155, 1, 1, Level(&_relu_tmp80_11), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_83_116, &_relu_tmp80_11, &_pgen_tmp_82_115);
  CMPLR_TM_END(17, 0);
  Encode_float_mask(&_pgen_tmp_84_117, 0.5685231134608953462717, mask_len_2, 1, Level(&_relu_tmp58_7), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_85_118, &_relu_tmp58_7, &_pgen_tmp_84_117);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_81_114, &_pgen_tmp_83_116, &_pgen_tmp_85_118);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_81_114._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_83_116._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp80_11._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_82_115._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_83_116._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp80_11._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_82_115._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_85_118._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_84_117._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_85_118._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_84_117._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_81_114._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_83_116._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_85_118._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_81_114._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_83_116._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_85_118._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_82_115._poly);
  Free_data(&_pgen_tmp_84_117._poly);
  Free_data(&_pgen_tmp_85_118._c1_poly);
  Free_data(&_pgen_tmp_85_118._c0_poly);
  Free_data(&_pgen_tmp_83_116._c1_poly);
  Free_data(&_pgen_tmp_83_116._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_86_119, &_pgen_tmp_81_114);
  Rescale(&_pgen_tmp_86_119._c0_poly, &_pgen_tmp_81_114._c0_poly);
  Rescale(&_pgen_tmp_86_119._c1_poly, &_pgen_tmp_81_114._c1_poly);
  Free_data(&_pgen_tmp_81_114._c1_poly);
  Free_data(&_pgen_tmp_81_114._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_80_113, &_relu_tmp90_13, &_pgen_tmp_86_119);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_80_113._c0_poly);
  Init_poly_by_size(&_pgen_tmp_87_120, 1, 0);
  Init_poly_by_size(&_pgen_tmp_88_121, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_87_120, 0, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_86_119._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_88_121, 0, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_86_119._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_80_113._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_86_119._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_80_113._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_87_120, 0, degree), Coeffs(&_pgen_tmp_88_121, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_80_113._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_86_119._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_86_119._c1_poly);
  Free_data(&_pgen_tmp_86_119._c0_poly);
  Free_data(&_relu_tmp90_13._c1_poly);
  Free_data(&_relu_tmp90_13._c0_poly);
  Free_data(&_pgen_tmp_87_120);
  Free_data(&_pgen_tmp_87_120);
  Free_data(&_pgen_tmp_88_121);
  Free_data(&_pgen_tmp_88_121);
  CMPLR_TM_END(16, 0);
  _preg_268435468 = Relinearize(_pgen_tmp_80_113);
  Free_data(&_pgen_tmp_80_113._c2_poly);
  Free_data(&_pgen_tmp_80_113._c1_poly);
  Free_data(&_pgen_tmp_80_113._c0_poly);
  Encode_double(&_pgen_tmp_89_122, &_cst_158, 1, 1, Level(&_relu_tmp80_11), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_90_123, &_relu_tmp80_11, &_pgen_tmp_89_122);
  CMPLR_TM_END(17, 0);
  Encode_float_mask(&_pgen_tmp_91_124, 1.614771079042867985009, mask_len_2, 1, Level(&_relu_tmp58_7), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_92_125, &_relu_tmp58_7, &_pgen_tmp_91_124);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_93_126, &_pgen_tmp_90_123, &_pgen_tmp_92_125);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_94_127, &_preg_268435468, &_pgen_tmp_93_126);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_56_89, &_preg_268435467, &_pgen_tmp_94_127);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_56_89._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_90_123._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp80_11._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_89_122._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_90_123._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp80_11._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_89_122._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_92_125._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_91_124._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_92_125._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_91_124._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_93_126._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_90_123._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_92_125._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_93_126._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_90_123._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_92_125._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_94_127._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435468._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_93_126._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_94_127._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435468._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_93_126._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_56_89._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435467._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_94_127._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_56_89._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435467._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_94_127._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_89_122._poly);
  Free_data(&_relu_tmp80_11._c1_poly);
  Free_data(&_relu_tmp80_11._c0_poly);
  Free_data(&_pgen_tmp_91_124._poly);
  Free_data(&_relu_tmp58_7._c1_poly);
  Free_data(&_relu_tmp58_7._c0_poly);
  Free_data(&_pgen_tmp_92_125._c1_poly);
  Free_data(&_pgen_tmp_92_125._c0_poly);
  Free_data(&_pgen_tmp_90_123._c1_poly);
  Free_data(&_pgen_tmp_90_123._c0_poly);
  Free_data(&_pgen_tmp_93_126._c1_poly);
  Free_data(&_pgen_tmp_93_126._c0_poly);
  Free_data(&_preg_268435468._c1_poly);
  Free_data(&_preg_268435468._c0_poly);
  Free_data(&_pgen_tmp_94_127._c1_poly);
  Free_data(&_pgen_tmp_94_127._c0_poly);
  Free_data(&_preg_268435467._c1_poly);
  Free_data(&_preg_268435467._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_relu_tmp155_16, &_pgen_tmp_56_89);
  Rescale(&_relu_tmp155_16._c0_poly, &_pgen_tmp_56_89._c0_poly);
  Rescale(&_relu_tmp155_16._c1_poly, &_pgen_tmp_56_89._c1_poly);
  Free_data(&_pgen_tmp_56_89._c1_poly);
  Free_data(&_pgen_tmp_56_89._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_95_128, &_relu_tmp155_16, &_relu_tmp155_16);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_95_128._c0_poly);
  Init_poly_by_size(&_pgen_tmp_96_129, 1, 0);
  Init_poly_by_size(&_pgen_tmp_97_130, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_96_129, 0, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_97_130, 0, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_95_128._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_95_128._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_96_129, 0, degree), Coeffs(&_pgen_tmp_97_130, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_95_128._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_96_129);
  Free_data(&_pgen_tmp_96_129);
  Free_data(&_pgen_tmp_97_130);
  Free_data(&_pgen_tmp_97_130);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_98_131, &_pgen_tmp_95_128);
  Rescale(&_pgen_tmp_98_131._c0_poly, &_pgen_tmp_95_128._c0_poly);
  Rescale(&_pgen_tmp_98_131._c1_poly, &_pgen_tmp_95_128._c1_poly);
  Rescale(&_pgen_tmp_98_131._c2_poly, &_pgen_tmp_95_128._c2_poly);
  Free_data(&_pgen_tmp_95_128._c2_poly);
  Free_data(&_pgen_tmp_95_128._c1_poly);
  Free_data(&_pgen_tmp_95_128._c0_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435469 = Relinearize(_pgen_tmp_98_131);
  Free_data(&_pgen_tmp_98_131._c2_poly);
  Free_data(&_pgen_tmp_98_131._c1_poly);
  Free_data(&_pgen_tmp_98_131._c0_poly);
  Copy_ciph(&_relu_tmp159_17, &_preg_268435469);
  Free_data(&_preg_268435469._c1_poly);
  Free_data(&_preg_268435469._c0_poly);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_99_132, &_relu_tmp159_17, &_relu_tmp159_17);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_100_133, &_cst_159, 1, Sc_degree(&_pgen_tmp_99_132), Level(&_pgen_tmp_99_132), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp165_18, &_pgen_tmp_99_132, &_pgen_tmp_100_133);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_relu_tmp165_18._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_99_132._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp159_17._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp159_17._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_99_132._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp159_17._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp159_17._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_relu_tmp165_18._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_99_132._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_100_133._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Set_coeffs(&_relu_tmp165_18._c1_poly, _pgen_rns_idx_33, degree, Coeffs(&_pgen_tmp_99_132._c1_poly, _pgen_rns_idx_33, degree));
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_relu_tmp159_17._c1_poly);
  Free_data(&_relu_tmp159_17._c0_poly);
  Free_data(&_pgen_tmp_100_133._poly);
  Free_data(&_pgen_tmp_99_132._c1_poly);
  Free_data(&_pgen_tmp_99_132._c0_poly);
  CMPLR_TM_END(13, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_101_134, &_relu_tmp155_16, &_relu_tmp165_18);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_101_134._c0_poly);
  Init_poly_by_size(&_pgen_tmp_102_135, 1, 0);
  Init_poly_by_size(&_pgen_tmp_103_136, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_102_135, 0, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp165_18._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_103_136, 0, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp165_18._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_101_134._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp165_18._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_101_134._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_102_135, 0, degree), Coeffs(&_pgen_tmp_103_136, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_101_134._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp165_18._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_102_135);
  Free_data(&_pgen_tmp_102_135);
  Free_data(&_pgen_tmp_103_136);
  Free_data(&_pgen_tmp_103_136);
  CMPLR_TM_END(16, 0);
  _preg_268435470 = Relinearize(_pgen_tmp_101_134);
  Free_data(&_pgen_tmp_101_134._c2_poly);
  Free_data(&_pgen_tmp_101_134._c1_poly);
  Free_data(&_pgen_tmp_101_134._c0_poly);
  Copy_ciph(&_relu_tmp169_19, &_preg_268435470);
  Free_data(&_preg_268435470._c1_poly);
  Free_data(&_preg_268435470._c0_poly);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_105_138, &_relu_tmp169_19, &_relu_tmp169_19);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_106_139, &_cst_160, 1, 1, Level(&_relu_tmp155_16), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_107_140, &_relu_tmp155_16, &_pgen_tmp_106_139);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_104_137, &_pgen_tmp_105_138, &_pgen_tmp_107_140);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_104_137._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_105_138._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp169_19._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp169_19._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_105_138._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp169_19._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp169_19._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_107_140._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_106_139._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_107_140._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_106_139._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_104_137._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_105_138._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_107_140._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_104_137._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_105_138._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_107_140._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_relu_tmp169_19._c1_poly);
  Free_data(&_relu_tmp169_19._c0_poly);
  Free_data(&_pgen_tmp_106_139._poly);
  Free_data(&_pgen_tmp_107_140._c1_poly);
  Free_data(&_pgen_tmp_107_140._c0_poly);
  Free_data(&_pgen_tmp_105_138._c1_poly);
  Free_data(&_pgen_tmp_105_138._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_relu_tmp177_20, &_pgen_tmp_104_137);
  Rescale(&_relu_tmp177_20._c0_poly, &_pgen_tmp_104_137._c0_poly);
  Rescale(&_relu_tmp177_20._c1_poly, &_pgen_tmp_104_137._c1_poly);
  Free_data(&_pgen_tmp_104_137._c1_poly);
  Free_data(&_pgen_tmp_104_137._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_108_141, &_relu_tmp165_18, &_relu_tmp165_18);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_108_141._c0_poly);
  Init_poly_by_size(&_pgen_tmp_109_142, 1, 0);
  Init_poly_by_size(&_pgen_tmp_110_143, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_109_142, 0, degree), Coeffs(&_relu_tmp165_18._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp165_18._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_110_143, 0, degree), Coeffs(&_relu_tmp165_18._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp165_18._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_108_141._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp165_18._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp165_18._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_108_141._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_109_142, 0, degree), Coeffs(&_pgen_tmp_110_143, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_108_141._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp165_18._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp165_18._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_relu_tmp165_18._c1_poly);
  Free_data(&_relu_tmp165_18._c0_poly);
  Free_data(&_pgen_tmp_109_142);
  Free_data(&_pgen_tmp_109_142);
  Free_data(&_pgen_tmp_110_143);
  Free_data(&_pgen_tmp_110_143);
  CMPLR_TM_END(16, 0);
  _preg_268435471 = Relinearize(_pgen_tmp_108_141);
  Free_data(&_pgen_tmp_108_141._c2_poly);
  Free_data(&_pgen_tmp_108_141._c1_poly);
  Free_data(&_pgen_tmp_108_141._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_relu_tmp181_21, &_preg_268435471);
  Rescale(&_relu_tmp181_21._c0_poly, &_preg_268435471._c0_poly);
  Rescale(&_relu_tmp181_21._c1_poly, &_preg_268435471._c1_poly);
  Free_data(&_preg_268435471._c1_poly);
  Free_data(&_preg_268435471._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_111_144, &_relu_tmp181_21, &_relu_tmp181_21);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_112_145, &_cst_161, 1, Sc_degree(&_pgen_tmp_111_144), Level(&_pgen_tmp_111_144), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp187_22, &_pgen_tmp_111_144, &_pgen_tmp_112_145);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_relu_tmp187_22._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_111_144._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp181_21._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp181_21._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_111_144._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp181_21._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp181_21._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_111_144._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_112_145._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Set_coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_33, degree, Coeffs(&_pgen_tmp_111_144._c1_poly, _pgen_rns_idx_33, degree));
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_relu_tmp181_21._c1_poly);
  Free_data(&_relu_tmp181_21._c0_poly);
  Free_data(&_pgen_tmp_112_145._poly);
  Free_data(&_pgen_tmp_111_144._c1_poly);
  Free_data(&_pgen_tmp_111_144._c0_poly);
  CMPLR_TM_END(13, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_113_146, &_relu_tmp187_22, &_relu_tmp187_22);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_113_146._c0_poly);
  Init_poly_by_size(&_pgen_tmp_114_147, 1, 0);
  Init_poly_by_size(&_pgen_tmp_115_148, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_114_147, 0, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_115_148, 0, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_113_146._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_113_146._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_114_147, 0, degree), Coeffs(&_pgen_tmp_115_148, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_113_146._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_114_147);
  Free_data(&_pgen_tmp_114_147);
  Free_data(&_pgen_tmp_115_148);
  Free_data(&_pgen_tmp_115_148);
  CMPLR_TM_END(16, 0);
  _preg_268435472 = Relinearize(_pgen_tmp_113_146);
  Free_data(&_pgen_tmp_113_146._c2_poly);
  Free_data(&_pgen_tmp_113_146._c1_poly);
  Free_data(&_pgen_tmp_113_146._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_relu_tmp191_23, &_preg_268435472);
  Rescale(&_relu_tmp191_23._c0_poly, &_preg_268435472._c0_poly);
  Rescale(&_relu_tmp191_23._c1_poly, &_preg_268435472._c1_poly);
  Free_data(&_preg_268435472._c1_poly);
  Free_data(&_preg_268435472._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_116_149, &_relu_tmp191_23, &_relu_tmp191_23);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_117_150, &_cst_162, 1, Sc_degree(&_pgen_tmp_116_149), Level(&_pgen_tmp_116_149), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp197_24, &_pgen_tmp_116_149, &_pgen_tmp_117_150);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_relu_tmp197_24._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_116_149._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp191_23._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp191_23._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_116_149._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp191_23._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp191_23._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_relu_tmp197_24._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_116_149._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_117_150._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Set_coeffs(&_relu_tmp197_24._c1_poly, _pgen_rns_idx_33, degree, Coeffs(&_pgen_tmp_116_149._c1_poly, _pgen_rns_idx_33, degree));
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_relu_tmp191_23._c1_poly);
  Free_data(&_relu_tmp191_23._c0_poly);
  Free_data(&_pgen_tmp_117_150._poly);
  Free_data(&_pgen_tmp_116_149._c1_poly);
  Free_data(&_pgen_tmp_116_149._c0_poly);
  CMPLR_TM_END(13, 0);
  Encode_float_mask(&_pgen_tmp_123_156, 0.0001111119162030077897477, mask_len_2, 1, Level(&_relu_tmp155_16), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_122_155, &_relu_tmp155_16, &_pgen_tmp_123_156);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_122_155._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_122_155._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_123_156._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_122_155._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_123_156._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_123_156._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_124_157, &_pgen_tmp_122_155);
  Rescale(&_pgen_tmp_124_157._c0_poly, &_pgen_tmp_122_155._c0_poly);
  Rescale(&_pgen_tmp_124_157._c1_poly, &_pgen_tmp_122_155._c1_poly);
  Free_data(&_pgen_tmp_122_155._c1_poly);
  Free_data(&_pgen_tmp_122_155._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_121_154, &input_ct0_0, &_pgen_tmp_124_157);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_121_154._c0_poly);
  Init_poly_by_size(&_pgen_tmp_125_158, 1, 0);
  Init_poly_by_size(&_pgen_tmp_126_159, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_125_158, 0, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_124_157._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_126_159, 0, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_124_157._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_121_154._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_124_157._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_121_154._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_125_158, 0, degree), Coeffs(&_pgen_tmp_126_159, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_121_154._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_124_157._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_124_157._c1_poly);
  Free_data(&_pgen_tmp_124_157._c0_poly);
  Free_data(&_pgen_tmp_125_158);
  Free_data(&_pgen_tmp_125_158);
  Free_data(&_pgen_tmp_126_159);
  Free_data(&_pgen_tmp_126_159);
  CMPLR_TM_END(16, 0);
  _preg_268435473 = Relinearize(_pgen_tmp_121_154);
  Free_data(&_pgen_tmp_121_154._c2_poly);
  Free_data(&_pgen_tmp_121_154._c1_poly);
  Free_data(&_pgen_tmp_121_154._c0_poly);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_127_160, &_preg_268435473);
  Rescale(&_pgen_tmp_127_160._c0_poly, &_preg_268435473._c0_poly);
  Rescale(&_pgen_tmp_127_160._c1_poly, &_preg_268435473._c1_poly);
  Free_data(&_preg_268435473._c1_poly);
  Free_data(&_preg_268435473._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_120_153, &_relu_tmp187_22, &_pgen_tmp_127_160);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_120_153._c0_poly);
  Init_poly_by_size(&_pgen_tmp_128_161, 1, 0);
  Init_poly_by_size(&_pgen_tmp_129_162, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_128_161, 0, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_127_160._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_129_162, 0, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_127_160._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_120_153._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_127_160._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_120_153._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_128_161, 0, degree), Coeffs(&_pgen_tmp_129_162, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_120_153._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_127_160._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_127_160._c1_poly);
  Free_data(&_pgen_tmp_127_160._c0_poly);
  Free_data(&_pgen_tmp_128_161);
  Free_data(&_pgen_tmp_128_161);
  Free_data(&_pgen_tmp_129_162);
  Free_data(&_pgen_tmp_129_162);
  CMPLR_TM_END(16, 0);
  _preg_268435474 = Relinearize(_pgen_tmp_120_153);
  Free_data(&_pgen_tmp_120_153._c2_poly);
  Free_data(&_pgen_tmp_120_153._c1_poly);
  Free_data(&_pgen_tmp_120_153._c0_poly);
  Encode_float_mask(&_pgen_tmp_132_165, -0.0009371069270347270278171, mask_len_2, 1, Level(&input_ct0_0), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_131_164, &input_ct0_0, &_pgen_tmp_132_165);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_131_164._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_131_164._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_132_165._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_131_164._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_132_165._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_132_165._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_133_166, &_pgen_tmp_131_164);
  Rescale(&_pgen_tmp_133_166._c0_poly, &_pgen_tmp_131_164._c0_poly);
  Rescale(&_pgen_tmp_133_166._c1_poly, &_pgen_tmp_131_164._c1_poly);
  Free_data(&_pgen_tmp_131_164._c1_poly);
  Free_data(&_pgen_tmp_131_164._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_130_163, &_relu_tmp177_20, &_pgen_tmp_133_166);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_130_163._c0_poly);
  Init_poly_by_size(&_pgen_tmp_134_167, 1, 0);
  Init_poly_by_size(&_pgen_tmp_135_168, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_134_167, 0, degree), Coeffs(&_relu_tmp177_20._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_133_166._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_135_168, 0, degree), Coeffs(&_relu_tmp177_20._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_133_166._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_130_163._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp177_20._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_133_166._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_130_163._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_134_167, 0, degree), Coeffs(&_pgen_tmp_135_168, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_130_163._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp177_20._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_133_166._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_133_166._c1_poly);
  Free_data(&_pgen_tmp_133_166._c0_poly);
  Free_data(&_pgen_tmp_134_167);
  Free_data(&_pgen_tmp_134_167);
  Free_data(&_pgen_tmp_135_168);
  Free_data(&_pgen_tmp_135_168);
  CMPLR_TM_END(16, 0);
  _preg_268435475 = Relinearize(_pgen_tmp_130_163);
  Free_data(&_pgen_tmp_130_163._c2_poly);
  Free_data(&_pgen_tmp_130_163._c1_poly);
  Free_data(&_pgen_tmp_130_163._c0_poly);
  Encode_float_mask(&_pgen_tmp_138_171, 0.006507995190210730772351, mask_len_2, 1, Level(&_relu_tmp155_16), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_137_170, &_relu_tmp155_16, &_pgen_tmp_138_171);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_137_170._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_137_170._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_138_171._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_137_170._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_138_171._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_138_171._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_139_172, &_pgen_tmp_137_170);
  Rescale(&_pgen_tmp_139_172._c0_poly, &_pgen_tmp_137_170._c0_poly);
  Rescale(&_pgen_tmp_139_172._c1_poly, &_pgen_tmp_137_170._c1_poly);
  Free_data(&_pgen_tmp_137_170._c1_poly);
  Free_data(&_pgen_tmp_137_170._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_136_169, &input_ct0_0, &_pgen_tmp_139_172);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_136_169._c0_poly);
  Init_poly_by_size(&_pgen_tmp_140_173, 1, 0);
  Init_poly_by_size(&_pgen_tmp_141_174, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_140_173, 0, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_139_172._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_141_174, 0, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_139_172._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_136_169._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_139_172._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_136_169._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_140_173, 0, degree), Coeffs(&_pgen_tmp_141_174, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_136_169._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_139_172._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_139_172._c1_poly);
  Free_data(&_pgen_tmp_139_172._c0_poly);
  Free_data(&_pgen_tmp_140_173);
  Free_data(&_pgen_tmp_140_173);
  Free_data(&_pgen_tmp_141_174);
  Free_data(&_pgen_tmp_141_174);
  CMPLR_TM_END(16, 0);
  _preg_268435476 = Relinearize(_pgen_tmp_136_169);
  Free_data(&_pgen_tmp_136_169._c2_poly);
  Free_data(&_pgen_tmp_136_169._c1_poly);
  Free_data(&_pgen_tmp_136_169._c0_poly);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_142_175, &_preg_268435475, &_preg_268435476);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_119_152, &_preg_268435474, &_pgen_tmp_142_175);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_119_152._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_142_175._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435475._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435476._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_142_175._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435475._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435476._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_119_152._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435474._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_142_175._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_119_152._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435474._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_142_175._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_preg_268435476._c1_poly);
  Free_data(&_preg_268435476._c0_poly);
  Free_data(&_preg_268435475._c1_poly);
  Free_data(&_preg_268435475._c0_poly);
  Free_data(&_pgen_tmp_142_175._c1_poly);
  Free_data(&_pgen_tmp_142_175._c0_poly);
  Free_data(&_preg_268435474._c1_poly);
  Free_data(&_preg_268435474._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_143_176, &_pgen_tmp_119_152);
  Rescale(&_pgen_tmp_143_176._c0_poly, &_pgen_tmp_119_152._c0_poly);
  Rescale(&_pgen_tmp_143_176._c1_poly, &_pgen_tmp_119_152._c1_poly);
  Free_data(&_pgen_tmp_119_152._c1_poly);
  Free_data(&_pgen_tmp_119_152._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_118_151, &_relu_tmp197_24, &_pgen_tmp_143_176);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_118_151._c0_poly);
  Init_poly_by_size(&_pgen_tmp_144_177, 1, 0);
  Init_poly_by_size(&_pgen_tmp_145_178, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_144_177, 0, degree), Coeffs(&_relu_tmp197_24._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_143_176._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_145_178, 0, degree), Coeffs(&_relu_tmp197_24._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_143_176._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_118_151._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp197_24._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_143_176._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_118_151._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_144_177, 0, degree), Coeffs(&_pgen_tmp_145_178, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_118_151._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp197_24._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_143_176._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_143_176._c1_poly);
  Free_data(&_pgen_tmp_143_176._c0_poly);
  Free_data(&_relu_tmp197_24._c1_poly);
  Free_data(&_relu_tmp197_24._c0_poly);
  Free_data(&_pgen_tmp_144_177);
  Free_data(&_pgen_tmp_144_177);
  Free_data(&_pgen_tmp_145_178);
  Free_data(&_pgen_tmp_145_178);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_146_179, &_pgen_tmp_118_151);
  Rescale(&_pgen_tmp_146_179._c0_poly, &_pgen_tmp_118_151._c0_poly);
  Rescale(&_pgen_tmp_146_179._c1_poly, &_pgen_tmp_118_151._c1_poly);
  Rescale(&_pgen_tmp_146_179._c2_poly, &_pgen_tmp_118_151._c2_poly);
  Free_data(&_pgen_tmp_118_151._c2_poly);
  Free_data(&_pgen_tmp_118_151._c1_poly);
  Free_data(&_pgen_tmp_118_151._c0_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435477 = Relinearize(_pgen_tmp_146_179);
  Free_data(&_pgen_tmp_146_179._c2_poly);
  Free_data(&_pgen_tmp_146_179._c1_poly);
  Free_data(&_pgen_tmp_146_179._c0_poly);
  Encode_float_mask(&_pgen_tmp_151_184, -0.03691063940349948585551, mask_len_2, 1, Level(&input_ct0_0), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_150_183, &input_ct0_0, &_pgen_tmp_151_184);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_150_183._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_150_183._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_151_184._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_150_183._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_151_184._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_151_184._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_152_185, &_pgen_tmp_150_183);
  Rescale(&_pgen_tmp_152_185._c0_poly, &_pgen_tmp_150_183._c0_poly);
  Rescale(&_pgen_tmp_152_185._c1_poly, &_pgen_tmp_150_183._c1_poly);
  Free_data(&_pgen_tmp_150_183._c1_poly);
  Free_data(&_pgen_tmp_150_183._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_149_182, &_relu_tmp177_20, &_pgen_tmp_152_185);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_149_182._c0_poly);
  Init_poly_by_size(&_pgen_tmp_153_186, 1, 0);
  Init_poly_by_size(&_pgen_tmp_154_187, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_153_186, 0, degree), Coeffs(&_relu_tmp177_20._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_152_185._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_154_187, 0, degree), Coeffs(&_relu_tmp177_20._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_152_185._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_149_182._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp177_20._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_152_185._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_149_182._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_153_186, 0, degree), Coeffs(&_pgen_tmp_154_187, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_149_182._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp177_20._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_152_185._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_152_185._c1_poly);
  Free_data(&_pgen_tmp_152_185._c0_poly);
  Free_data(&_pgen_tmp_153_186);
  Free_data(&_pgen_tmp_153_186);
  Free_data(&_pgen_tmp_154_187);
  Free_data(&_pgen_tmp_154_187);
  CMPLR_TM_END(16, 0);
  _preg_268435478 = Relinearize(_pgen_tmp_149_182);
  Free_data(&_pgen_tmp_149_182._c2_poly);
  Free_data(&_pgen_tmp_149_182._c1_poly);
  Free_data(&_pgen_tmp_149_182._c0_poly);
  Encode_float_mask(&_pgen_tmp_157_190, 0.1056744718173614761003, mask_len_2, 1, Level(&_relu_tmp155_16), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_156_189, &_relu_tmp155_16, &_pgen_tmp_157_190);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_156_189._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_156_189._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_157_190._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_156_189._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_157_190._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_157_190._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_158_191, &_pgen_tmp_156_189);
  Rescale(&_pgen_tmp_158_191._c0_poly, &_pgen_tmp_156_189._c0_poly);
  Rescale(&_pgen_tmp_158_191._c1_poly, &_pgen_tmp_156_189._c1_poly);
  Free_data(&_pgen_tmp_156_189._c1_poly);
  Free_data(&_pgen_tmp_156_189._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_155_188, &input_ct0_0, &_pgen_tmp_158_191);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_155_188._c0_poly);
  Init_poly_by_size(&_pgen_tmp_159_192, 1, 0);
  Init_poly_by_size(&_pgen_tmp_160_193, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_159_192, 0, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_158_191._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_160_193, 0, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_158_191._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_155_188._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_158_191._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_155_188._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_159_192, 0, degree), Coeffs(&_pgen_tmp_160_193, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_155_188._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_158_191._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_158_191._c1_poly);
  Free_data(&_pgen_tmp_158_191._c0_poly);
  Free_data(&_pgen_tmp_159_192);
  Free_data(&_pgen_tmp_159_192);
  Free_data(&_pgen_tmp_160_193);
  Free_data(&_pgen_tmp_160_193);
  CMPLR_TM_END(16, 0);
  _preg_268435479 = Relinearize(_pgen_tmp_155_188);
  Free_data(&_pgen_tmp_155_188._c2_poly);
  Free_data(&_pgen_tmp_155_188._c1_poly);
  Free_data(&_pgen_tmp_155_188._c0_poly);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_148_181, &_preg_268435478, &_preg_268435479);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_148_181._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_148_181._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435478._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435479._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_148_181._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435478._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435479._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_preg_268435479._c1_poly);
  Free_data(&_preg_268435479._c0_poly);
  Free_data(&_preg_268435478._c1_poly);
  Free_data(&_preg_268435478._c0_poly);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_161_194, &_pgen_tmp_148_181);
  Rescale(&_pgen_tmp_161_194._c0_poly, &_pgen_tmp_148_181._c0_poly);
  Rescale(&_pgen_tmp_161_194._c1_poly, &_pgen_tmp_148_181._c1_poly);
  Free_data(&_pgen_tmp_148_181._c1_poly);
  Free_data(&_pgen_tmp_148_181._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_147_180, &_relu_tmp187_22, &_pgen_tmp_161_194);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_147_180._c0_poly);
  Init_poly_by_size(&_pgen_tmp_162_195, 1, 0);
  Init_poly_by_size(&_pgen_tmp_163_196, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_162_195, 0, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_161_194._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_163_196, 0, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_161_194._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_147_180._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_161_194._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_147_180._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_162_195, 0, degree), Coeffs(&_pgen_tmp_163_196, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_147_180._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_161_194._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_161_194._c1_poly);
  Free_data(&_pgen_tmp_161_194._c0_poly);
  Free_data(&_relu_tmp187_22._c1_poly);
  Free_data(&_relu_tmp187_22._c0_poly);
  Free_data(&_pgen_tmp_162_195);
  Free_data(&_pgen_tmp_162_195);
  Free_data(&_pgen_tmp_163_196);
  Free_data(&_pgen_tmp_163_196);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_164_197, &_pgen_tmp_147_180);
  Rescale(&_pgen_tmp_164_197._c0_poly, &_pgen_tmp_147_180._c0_poly);
  Rescale(&_pgen_tmp_164_197._c1_poly, &_pgen_tmp_147_180._c1_poly);
  Rescale(&_pgen_tmp_164_197._c2_poly, &_pgen_tmp_147_180._c2_poly);
  Free_data(&_pgen_tmp_147_180._c2_poly);
  Free_data(&_pgen_tmp_147_180._c1_poly);
  Free_data(&_pgen_tmp_147_180._c0_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435480 = Relinearize(_pgen_tmp_164_197);
  Free_data(&_pgen_tmp_164_197._c2_poly);
  Free_data(&_pgen_tmp_164_197._c1_poly);
  Free_data(&_pgen_tmp_164_197._c0_poly);
  Encode_float_mask(&_pgen_tmp_167_200, -0.2078597158810965850329, mask_len_2, 1, Level(&input_ct0_0), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_166_199, &input_ct0_0, &_pgen_tmp_167_200);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_166_199._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_166_199._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_167_200._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_166_199._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_167_200._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_167_200._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_168_201, &_pgen_tmp_166_199);
  Rescale(&_pgen_tmp_168_201._c0_poly, &_pgen_tmp_166_199._c0_poly);
  Rescale(&_pgen_tmp_168_201._c1_poly, &_pgen_tmp_166_199._c1_poly);
  Free_data(&_pgen_tmp_166_199._c1_poly);
  Free_data(&_pgen_tmp_166_199._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_165_198, &_relu_tmp177_20, &_pgen_tmp_168_201);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_165_198._c0_poly);
  Init_poly_by_size(&_pgen_tmp_169_202, 1, 0);
  Init_poly_by_size(&_pgen_tmp_170_203, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_169_202, 0, degree), Coeffs(&_relu_tmp177_20._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_168_201._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_170_203, 0, degree), Coeffs(&_relu_tmp177_20._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_168_201._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_165_198._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp177_20._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_168_201._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_165_198._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_169_202, 0, degree), Coeffs(&_pgen_tmp_170_203, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_165_198._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp177_20._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_168_201._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_168_201._c1_poly);
  Free_data(&_pgen_tmp_168_201._c0_poly);
  Free_data(&_relu_tmp177_20._c1_poly);
  Free_data(&_relu_tmp177_20._c0_poly);
  Free_data(&_pgen_tmp_169_202);
  Free_data(&_pgen_tmp_169_202);
  Free_data(&_pgen_tmp_170_203);
  Free_data(&_pgen_tmp_170_203);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_171_204, &_pgen_tmp_165_198);
  Rescale(&_pgen_tmp_171_204._c0_poly, &_pgen_tmp_165_198._c0_poly);
  Rescale(&_pgen_tmp_171_204._c1_poly, &_pgen_tmp_165_198._c1_poly);
  Rescale(&_pgen_tmp_171_204._c2_poly, &_pgen_tmp_165_198._c2_poly);
  Free_data(&_pgen_tmp_165_198._c2_poly);
  Free_data(&_pgen_tmp_165_198._c1_poly);
  Free_data(&_pgen_tmp_165_198._c0_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435481 = Relinearize(_pgen_tmp_171_204);
  Free_data(&_pgen_tmp_171_204._c2_poly);
  Free_data(&_pgen_tmp_171_204._c1_poly);
  Free_data(&_pgen_tmp_171_204._c0_poly);
  Encode_float_mask(&_pgen_tmp_174_207, 0.6334139843709288841822, mask_len_2, 1, Level(&_relu_tmp155_16), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_173_206, &_relu_tmp155_16, &_pgen_tmp_174_207);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_173_206._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_173_206._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_174_207._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_173_206._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_174_207._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_174_207._poly);
  Free_data(&_relu_tmp155_16._c1_poly);
  Free_data(&_relu_tmp155_16._c0_poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_175_208, &_pgen_tmp_173_206);
  Rescale(&_pgen_tmp_175_208._c0_poly, &_pgen_tmp_173_206._c0_poly);
  Rescale(&_pgen_tmp_175_208._c1_poly, &_pgen_tmp_173_206._c1_poly);
  Free_data(&_pgen_tmp_173_206._c1_poly);
  Free_data(&_pgen_tmp_173_206._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_172_205, &input_ct0_0, &_pgen_tmp_175_208);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_172_205._c0_poly);
  Init_poly_by_size(&_pgen_tmp_176_209, 1, 0);
  Init_poly_by_size(&_pgen_tmp_177_210, 1, 0);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_176_209, 0, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_175_208._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_177_210, 0, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_175_208._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_172_205._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_175_208._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_172_205._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_176_209, 0, degree), Coeffs(&_pgen_tmp_177_210, 0, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_172_205._c2_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_175_208._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_175_208._c1_poly);
  Free_data(&_pgen_tmp_175_208._c0_poly);
  Free_data(&_pgen_tmp_176_209);
  Free_data(&_pgen_tmp_176_209);
  Free_data(&_pgen_tmp_177_210);
  Free_data(&_pgen_tmp_177_210);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_178_211, &_pgen_tmp_172_205);
  Rescale(&_pgen_tmp_178_211._c0_poly, &_pgen_tmp_172_205._c0_poly);
  Rescale(&_pgen_tmp_178_211._c1_poly, &_pgen_tmp_172_205._c1_poly);
  Rescale(&_pgen_tmp_178_211._c2_poly, &_pgen_tmp_172_205._c2_poly);
  Free_data(&_pgen_tmp_172_205._c2_poly);
  Free_data(&_pgen_tmp_172_205._c1_poly);
  Free_data(&_pgen_tmp_172_205._c0_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435482 = Relinearize(_pgen_tmp_178_211);
  Free_data(&_pgen_tmp_178_211._c2_poly);
  Free_data(&_pgen_tmp_178_211._c1_poly);
  Free_data(&_pgen_tmp_178_211._c0_poly);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_179_212, &_preg_268435481, &_preg_268435482);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_180_213, &_preg_268435480, &_pgen_tmp_179_212);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_relu_tmp266_25, &_preg_268435477, &_pgen_tmp_180_213);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_relu_tmp266_25._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_179_212._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435481._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435482._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_179_212._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435481._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435482._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_180_213._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435480._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_179_212._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_180_213._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435480._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_179_212._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_relu_tmp266_25._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435477._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_180_213._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_relu_tmp266_25._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_preg_268435477._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_180_213._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_preg_268435482._c1_poly);
  Free_data(&_preg_268435482._c0_poly);
  Free_data(&_preg_268435481._c1_poly);
  Free_data(&_preg_268435481._c0_poly);
  Free_data(&_pgen_tmp_179_212._c1_poly);
  Free_data(&_pgen_tmp_179_212._c0_poly);
  Free_data(&_preg_268435480._c1_poly);
  Free_data(&_preg_268435480._c0_poly);
  Free_data(&_pgen_tmp_180_213._c1_poly);
  Free_data(&_pgen_tmp_180_213._c0_poly);
  Free_data(&_preg_268435477._c1_poly);
  Free_data(&_preg_268435477._c0_poly);
  CMPLR_TM_END(12, 0);
  Encode_float_mask(&_pgen_tmp_182_215, 0.5, mask_len_2, 1, Level(&input_ct0_0), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_181_214, &input_ct0_0, &_pgen_tmp_182_215);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_pgen_tmp_181_214._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_181_214._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_182_215._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_181_214._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_182_215._poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_182_215._poly);
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_183_216, &_pgen_tmp_181_214);
  Rescale(&_pgen_tmp_183_216._c0_poly, &_pgen_tmp_181_214._c0_poly);
  Rescale(&_pgen_tmp_183_216._c1_poly, &_pgen_tmp_181_214._c1_poly);
  Free_data(&_pgen_tmp_181_214._c1_poly);
  Free_data(&_pgen_tmp_181_214._c0_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_relu_tmp274_26, &_relu_tmp266_25, &_pgen_tmp_183_216);
  _pgen_modulus_31 = Q_modulus();
  _pgen_num_q_32 = Poly_level(&_relu_tmp274_26._c0_poly);
  for (_pgen_rns_idx_33 = 0; _pgen_rns_idx_33 < _pgen_num_q_32; _pgen_rns_idx_33 = _pgen_rns_idx_33 + 1) {
    Hw_modadd(Coeffs(&_relu_tmp274_26._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp266_25._c0_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_183_216._c0_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    Hw_modadd(Coeffs(&_relu_tmp274_26._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_relu_tmp266_25._c1_poly, _pgen_rns_idx_33, degree), Coeffs(&_pgen_tmp_183_216._c1_poly, _pgen_rns_idx_33, degree), _pgen_modulus_31, degree);
    _pgen_modulus_31 = _pgen_modulus_31 + 1;
  }
  Free_data(&_pgen_tmp_183_216._c1_poly);
  Free_data(&_pgen_tmp_183_216._c0_poly);
  Free_data(&_relu_tmp266_25._c1_poly);
  Free_data(&_relu_tmp266_25._c0_poly);
  CMPLR_TM_END(12, 0);
  return _relu_tmp274_26;
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
  if (rot_idx_1 == 0) {
    Init_ciph_same_scale(&_pgen_rot_res_2, &ciph_0, 0);
    Copy_ciphertext(&_pgen_rot_res_2, &ciph_0);
    RTLIB_TM_END(20, rtm);
    return _pgen_rot_res_2;
  }
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
  Free_data(&_pgen_swk_c0_3);
  Free_data(&_pgen_swk_c0_3);
  Free_data(&_pgen_swk_c1_4);
  Free_data(&_pgen_swk_c1_4);
  Free_data(&_pgen_ext_5);
  Free_data(&_pgen_ext_5);
  Free_data(&_pgen_tmp_poly_6);
  Free_data(&_pgen_tmp_poly_6);
  Free_data(&_pgen_mod_down_c0_7);
  Free_data(&_pgen_mod_down_c0_7);
  Free_data(&_pgen_mod_down_c1_8);
  Free_data(&_pgen_mod_down_c1_8);
  RTLIB_TM_END(20, rtm);
  return _pgen_rot_res_2;
}

CIPHERTEXT Relinearize(CIPHERTEXT3 ciph_0) {
  RTLIB_TM_START(18, rtm);
  CIPHERTEXT _pgen_relin_res_1;
  POLYNOMIAL _pgen_swk_c0_2;
  POLYNOMIAL _pgen_swk_c1_3;
  POLYNOMIAL _pgen_ext_4;
  POLYNOMIAL _pgen_tmp_poly_5;
  POLYNOMIAL _pgen_mod_down_c0_6;
  POLYNOMIAL _pgen_mod_down_c1_7;
  SWITCH_KEY* _pgen_swk_8;
  uint32_t _pgen_part_idx_9;
  POLYNOMIAL _pgen_key0_10;
  POLYNOMIAL _pgen_key1_11;
  MODULUS* _pgen_modulus_12;
  uint32_t _pgen_num_q_13;
  uint32_t _pgen_rns_idx_14;
  uint32_t _pgen_num_p_15;
  uint32_t _pgen_p_ofst_16;
  uint32_t _pgen_p_idx_17;
  uint32_t _pgen_key_p_ofst_18;
  uint32_t _pgen_key_p_idx_19;
  uint32_t  degree = Degree();
  memset(&_pgen_relin_res_1, 0, sizeof(_pgen_relin_res_1));
  memset(&_pgen_swk_c0_2, 0, sizeof(_pgen_swk_c0_2));
  memset(&_pgen_swk_c1_3, 0, sizeof(_pgen_swk_c1_3));
  memset(&_pgen_ext_4, 0, sizeof(_pgen_ext_4));
  memset(&_pgen_tmp_poly_5, 0, sizeof(_pgen_tmp_poly_5));
  memset(&_pgen_mod_down_c0_6, 0, sizeof(_pgen_mod_down_c0_6));
  memset(&_pgen_mod_down_c1_7, 0, sizeof(_pgen_mod_down_c1_7));
  memset(&_pgen_key0_10, 0, sizeof(_pgen_key0_10));
  memset(&_pgen_key1_11, 0, sizeof(_pgen_key1_11));
  Init_ciph_same_scale_ciph3(&_pgen_relin_res_1, &ciph_0);
  Init_poly_by_size(&_pgen_swk_c0_2, Poly_level(&ciph_0._c2_poly), 1);
  Init_poly_by_size(&_pgen_swk_c1_3, Poly_level(&ciph_0._c2_poly), 1);
  Init_poly_by_size(&_pgen_ext_4, Poly_level(&ciph_0._c2_poly), 1);
  Init_poly_by_size(&_pgen_tmp_poly_5, 1, 0);
  Init_poly_by_size(&_pgen_mod_down_c0_6, Poly_level(&ciph_0._c0_poly), 0);
  Init_poly_by_size(&_pgen_mod_down_c1_7, Poly_level(&ciph_0._c2_poly), 0);
  _pgen_swk_8 = Swk(0, 0);
  for (_pgen_part_idx_9 = 0; _pgen_part_idx_9 < Num_decomp(&ciph_0._c2_poly); _pgen_part_idx_9 = _pgen_part_idx_9 + 1) {
    Decomp_modup(&_pgen_ext_4, &ciph_0._c2_poly, _pgen_part_idx_9);
    Set_pk0(&_pgen_key0_10, _pgen_swk_8, _pgen_part_idx_9);
    Set_pk1(&_pgen_key1_11, _pgen_swk_8, _pgen_part_idx_9);
    _pgen_modulus_12 = Q_modulus();
    _pgen_num_q_13 = Poly_level(&_pgen_ext_4);
    for (_pgen_rns_idx_14 = 0; _pgen_rns_idx_14 < _pgen_num_q_13; _pgen_rns_idx_14 = _pgen_rns_idx_14 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_poly_5, 0, degree), Coeffs(&_pgen_key0_10, _pgen_rns_idx_14, degree), Coeffs(&_pgen_ext_4, _pgen_rns_idx_14, degree), _pgen_modulus_12, degree);
      Hw_modadd(Coeffs(&_pgen_swk_c0_2, _pgen_rns_idx_14, degree), Coeffs(&_pgen_swk_c0_2, _pgen_rns_idx_14, degree), Coeffs(&_pgen_tmp_poly_5, 0, degree), _pgen_modulus_12, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_poly_5, 0, degree), Coeffs(&_pgen_key1_11, _pgen_rns_idx_14, degree), Coeffs(&_pgen_ext_4, _pgen_rns_idx_14, degree), _pgen_modulus_12, degree);
      Hw_modadd(Coeffs(&_pgen_swk_c1_3, _pgen_rns_idx_14, degree), Coeffs(&_pgen_swk_c1_3, _pgen_rns_idx_14, degree), Coeffs(&_pgen_tmp_poly_5, 0, degree), _pgen_modulus_12, degree);
      _pgen_modulus_12 = _pgen_modulus_12 + 1;
    }
    _pgen_modulus_12 = P_modulus();
    _pgen_num_p_15 = Num_p(&_pgen_ext_4);
    _pgen_p_ofst_16 = Num_alloc(&_pgen_ext_4) - _pgen_num_p_15;
    _pgen_key_p_ofst_18 = Poly_level(&_pgen_key0_10);
    for (_pgen_rns_idx_14 = 0; _pgen_rns_idx_14 < _pgen_num_p_15; _pgen_rns_idx_14 = _pgen_rns_idx_14 + 1) {
      _pgen_p_idx_17 = _pgen_rns_idx_14 + _pgen_p_ofst_16;
      _pgen_key_p_idx_19 = _pgen_rns_idx_14 + _pgen_key_p_ofst_18;
      Hw_modmul(Coeffs(&_pgen_tmp_poly_5, 0, degree), Coeffs(&_pgen_key0_10, _pgen_key_p_idx_19, degree), Coeffs(&_pgen_ext_4, _pgen_p_idx_17, degree), _pgen_modulus_12, degree);
      Hw_modadd(Coeffs(&_pgen_swk_c0_2, _pgen_p_idx_17, degree), Coeffs(&_pgen_swk_c0_2, _pgen_p_idx_17, degree), Coeffs(&_pgen_tmp_poly_5, 0, degree), _pgen_modulus_12, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_poly_5, 0, degree), Coeffs(&_pgen_key1_11, _pgen_key_p_idx_19, degree), Coeffs(&_pgen_ext_4, _pgen_p_idx_17, degree), _pgen_modulus_12, degree);
      Hw_modadd(Coeffs(&_pgen_swk_c1_3, _pgen_p_idx_17, degree), Coeffs(&_pgen_swk_c1_3, _pgen_p_idx_17, degree), Coeffs(&_pgen_tmp_poly_5, 0, degree), _pgen_modulus_12, degree);
      _pgen_modulus_12 = _pgen_modulus_12 + 1;
    }
  }
  Mod_down(&_pgen_mod_down_c0_6, &_pgen_swk_c0_2);
  Mod_down(&_pgen_mod_down_c1_7, &_pgen_swk_c1_3);
  _pgen_modulus_12 = Q_modulus();
  _pgen_num_q_13 = Poly_level(&ciph_0._c0_poly);
  for (_pgen_rns_idx_14 = 0; _pgen_rns_idx_14 < _pgen_num_q_13; _pgen_rns_idx_14 = _pgen_rns_idx_14 + 1) {
    Hw_modadd(Coeffs(&_pgen_relin_res_1._c0_poly, _pgen_rns_idx_14, degree), Coeffs(&_pgen_mod_down_c0_6, _pgen_rns_idx_14, degree), Coeffs(&ciph_0._c0_poly, _pgen_rns_idx_14, degree), _pgen_modulus_12, degree);
    Hw_modadd(Coeffs(&_pgen_relin_res_1._c1_poly, _pgen_rns_idx_14, degree), Coeffs(&_pgen_mod_down_c1_7, _pgen_rns_idx_14, degree), Coeffs(&ciph_0._c1_poly, _pgen_rns_idx_14, degree), _pgen_modulus_12, degree);
    _pgen_modulus_12 = _pgen_modulus_12 + 1;
  }
  Free_data(&_pgen_swk_c0_2);
  Free_data(&_pgen_swk_c0_2);
  Free_data(&_pgen_swk_c1_3);
  Free_data(&_pgen_swk_c1_3);
  Free_data(&_pgen_ext_4);
  Free_data(&_pgen_ext_4);
  Free_data(&_pgen_tmp_poly_5);
  Free_data(&_pgen_tmp_poly_5);
  Free_data(&_pgen_mod_down_c0_6);
  Free_data(&_pgen_mod_down_c0_6);
  Free_data(&_pgen_mod_down_c1_7);
  Free_data(&_pgen_mod_down_c1_7);
  RTLIB_TM_END(18, rtm);
  return _pgen_relin_res_1;
}

CKKS_PARAMS* Get_context_params() {
  static CKKS_PARAMS parm = {
    LIB_ANT, 131072, 0, 29, 16, 60, 57, 3, 192, 1914, 
    { -61440, -57344, -53248, -49152, -45056, -40960, -36864, -32768,
      -28672, -24576, -20480, -16384, -12288, -8192, -4096, -2048,
      -1024, -512, -33, -32, -31, -17, -16, -15,
      -9, -8, -7, -5, -4, -3, -2, -1,
      0, 1, 2, 3, 4, 5, 6, 7,
      8, 9, 10, 11, 12, 13, 14, 15,
      16, 17, 18, 20, 21, 24, 27, 28,
      30, 31, 32, 33, 36, 39, 40, 42,
      44, 45, 48, 51, 52, 54, 56, 57,
      60, 63, 64, 66, 68, 69, 72, 75,
      76, 78, 80, 81, 84, 87, 88, 90,
      92, 93, 96, 99, 100, 102, 104, 105,
      108, 111, 112, 114, 116, 117, 120, 123,
      124, 126, 128, 129, 132, 135, 136, 138,
      140, 141, 144, 147, 148, 150, 152, 153,
      156, 159, 160, 162, 164, 165, 168, 171,
      172, 174, 176, 177, 180, 183, 184, 186,
      188, 189, 192, 195, 196, 198, 200, 201,
      204, 207, 208, 210, 212, 213, 216, 219,
      220, 222, 224, 225, 228, 231, 232, 234,
      236, 237, 240, 243, 244, 246, 248, 249,
      252, 255, 256, 258, 260, 261, 264, 267,
      268, 270, 272, 273, 276, 279, 280, 282,
      284, 285, 288, 291, 292, 294, 296, 297,
      300, 303, 304, 306, 308, 309, 312, 315,
      316, 318, 320, 321, 324, 327, 328, 330,
      332, 333, 336, 339, 340, 342, 344, 345,
      348, 351, 352, 354, 356, 357, 360, 363,
      364, 366, 368, 369, 372, 375, 376, 378,
      380, 381, 384, 387, 388, 390, 392, 393,
      396, 399, 400, 402, 404, 405, 408, 411,
      412, 414, 416, 417, 420, 423, 424, 426,
      428, 429, 432, 435, 436, 438, 440, 441,
      444, 447, 448, 450, 452, 453, 456, 459,
      460, 462, 464, 465, 468, 471, 472, 474,
      476, 477, 480, 483, 484, 486, 488, 489,
      492, 495, 496, 498, 500, 501, 504, 507,
      508, 510, 512, 513, 516, 519, 520, 522,
      524, 525, 528, 531, 532, 534, 536, 537,
      540, 543, 544, 546, 548, 549, 552, 555,
      556, 558, 560, 561, 564, 567, 568, 570,
      572, 573, 576, 579, 580, 582, 584, 585,
      588, 591, 592, 594, 596, 597, 600, 603,
      604, 606, 608, 609, 612, 615, 616, 618,
      620, 621, 624, 627, 628, 630, 632, 633,
      636, 639, 640, 642, 644, 645, 648, 651,
      652, 654, 656, 657, 660, 663, 664, 666,
      668, 669, 672, 675, 676, 678, 680, 681,
      684, 687, 688, 690, 692, 693, 696, 699,
      700, 702, 704, 705, 708, 711, 712, 714,
      716, 717, 720, 723, 724, 726, 728, 729,
      732, 735, 736, 738, 740, 741, 744, 747,
      748, 750, 752, 753, 756, 759, 760, 762,
      764, 765, 768, 771, 772, 774, 776, 777,
      780, 783, 784, 786, 788, 789, 792, 795,
      796, 798, 800, 801, 804, 807, 808, 810,
      812, 813, 816, 819, 820, 822, 824, 825,
      828, 831, 832, 834, 836, 837, 840, 843,
      844, 846, 848, 849, 852, 855, 856, 858,
      860, 861, 864, 867, 868, 870, 872, 873,
      876, 879, 880, 882, 884, 885, 888, 891,
      892, 894, 896, 897, 900, 903, 904, 906,
      908, 909, 912, 915, 916, 918, 920, 921,
      924, 927, 928, 930, 932, 933, 936, 939,
      940, 942, 944, 945, 948, 951, 952, 954,
      956, 957, 960, 963, 964, 966, 968, 969,
      972, 975, 976, 978, 980, 981, 984, 987,
      988, 990, 992, 993, 996, 999, 1000, 1002,
      1004, 1005, 1008, 1011, 1012, 1014, 1016, 1017,
      1020, 1023, 1024, 1026, 1028, 1029, 1032, 1035,
      1036, 1038, 1040, 1041, 1044, 1047, 1048, 1050,
      1052, 1053, 1056, 1059, 1060, 1062, 1064, 1065,
      1068, 1071, 1072, 1074, 1076, 1077, 1080, 1083,
      1084, 1086, 1088, 1089, 1092, 1095, 1096, 1098,
      1100, 1101, 1104, 1107, 1108, 1110, 1112, 1113,
      1116, 1119, 1120, 1122, 1124, 1125, 1128, 1131,
      1132, 1134, 1136, 1137, 1140, 1143, 1144, 1146,
      1148, 1149, 1152, 1155, 1156, 1158, 1160, 1161,
      1164, 1167, 1168, 1170, 1172, 1173, 1176, 1179,
      1180, 1182, 1184, 1185, 1188, 1191, 1192, 1194,
      1196, 1197, 1200, 1203, 1204, 1206, 1208, 1209,
      1212, 1215, 1216, 1218, 1220, 1221, 1224, 1227,
      1228, 1230, 1232, 1233, 1236, 1239, 1240, 1242,
      1244, 1245, 1248, 1251, 1252, 1254, 1256, 1257,
      1260, 1263, 1264, 1266, 1268, 1269, 1272, 1275,
      1276, 1278, 1280, 1281, 1284, 1287, 1288, 1290,
      1292, 1293, 1296, 1299, 1300, 1302, 1304, 1305,
      1308, 1311, 1312, 1314, 1316, 1317, 1320, 1323,
      1324, 1326, 1328, 1329, 1332, 1335, 1336, 1338,
      1340, 1341, 1344, 1347, 1348, 1350, 1352, 1353,
      1356, 1359, 1360, 1362, 1364, 1365, 1368, 1371,
      1372, 1374, 1376, 1377, 1380, 1383, 1384, 1386,
      1388, 1389, 1392, 1395, 1396, 1398, 1400, 1401,
      1404, 1407, 1408, 1410, 1412, 1413, 1416, 1419,
      1420, 1422, 1424, 1425, 1428, 1431, 1432, 1434,
      1436, 1437, 1440, 1443, 1444, 1446, 1448, 1449,
      1452, 1455, 1456, 1458, 1460, 1461, 1464, 1467,
      1468, 1470, 1472, 1473, 1476, 1479, 1480, 1482,
      1484, 1485, 1488, 1491, 1492, 1494, 1496, 1497,
      1500, 1503, 1504, 1506, 1508, 1509, 1512, 1515,
      1516, 1518, 1520, 1521, 1524, 1527, 1528, 1530,
      1532, 1533, 1536, 1540, 1544, 1548, 1552, 1556,
      1560, 1564, 1568, 1572, 1576, 1580, 1584, 1588,
      1592, 1596, 1600, 1604, 1608, 1612, 1616, 1620,
      1624, 1628, 1632, 1636, 1640, 1644, 1648, 1652,
      1656, 1660, 1664, 1668, 1672, 1676, 1680, 1684,
      1688, 1692, 1696, 1700, 1704, 1708, 1712, 1716,
      1720, 1724, 1728, 1732, 1736, 1740, 1744, 1748,
      1752, 1756, 1760, 1764, 1768, 1772, 1776, 1780,
      1784, 1788, 1792, 1796, 1800, 1804, 1808, 1812,
      1816, 1820, 1824, 1828, 1832, 1836, 1840, 1844,
      1848, 1852, 1856, 1860, 1864, 1868, 1872, 1876,
      1880, 1884, 1888, 1892, 1896, 1900, 1904, 1908,
      1912, 1916, 1920, 1924, 1928, 1932, 1936, 1940,
      1944, 1948, 1952, 1956, 1960, 1964, 1968, 1972,
      1976, 1980, 1984, 1988, 1992, 1996, 2000, 2004,
      2008, 2012, 2016, 2020, 2024, 2028, 2032, 2036,
      2040, 2044, 2048, 2052, 2064, 2076, 2080, 2088,
      2096, 2100, 2112, 2124, 2128, 2136, 2144, 2148,
      2160, 2172, 2176, 2184, 2192, 2196, 2208, 2220,
      2224, 2232, 2240, 2244, 2256, 2268, 2272, 2280,
      2288, 2292, 2304, 2316, 2320, 2328, 2336, 2340,
      2352, 2364, 2368, 2376, 2384, 2388, 2400, 2412,
      2416, 2424, 2432, 2436, 2448, 2460, 2464, 2472,
      2480, 2484, 2496, 2508, 2512, 2520, 2528, 2532,
      2544, 2556, 2560, 2568, 2576, 2580, 2592, 2604,
      2608, 2616, 2624, 2628, 2640, 2652, 2656, 2664,
      2672, 2676, 2688, 2700, 2704, 2712, 2720, 2724,
      2736, 2748, 2752, 2760, 2768, 2772, 2784, 2796,
      2800, 2808, 2816, 2820, 2832, 2844, 2848, 2856,
      2864, 2868, 2880, 2892, 2896, 2904, 2912, 2916,
      2928, 2940, 2944, 2952, 2960, 2964, 2976, 2988,
      2992, 3000, 3008, 3012, 3024, 3036, 3040, 3048,
      3056, 3060, 3072, 3084, 3088, 3096, 3104, 3108,
      3120, 3132, 3136, 3144, 3152, 3156, 3168, 3180,
      3184, 3192, 3200, 3204, 3216, 3228, 3232, 3240,
      3248, 3252, 3264, 3276, 3280, 3288, 3296, 3300,
      3312, 3324, 3328, 3336, 3344, 3348, 3360, 3372,
      3376, 3384, 3392, 3396, 3408, 3420, 3424, 3432,
      3440, 3444, 3456, 3468, 3472, 3480, 3488, 3492,
      3504, 3516, 3520, 3528, 3536, 3540, 3552, 3564,
      3568, 3576, 3584, 3588, 3600, 3612, 3616, 3624,
      3632, 3636, 3648, 3660, 3664, 3672, 3680, 3684,
      3696, 3708, 3712, 3720, 3728, 3732, 3744, 3756,
      3760, 3768, 3776, 3780, 3792, 3804, 3808, 3816,
      3824, 3828, 3840, 3852, 3856, 3864, 3872, 3876,
      3888, 3900, 3904, 3912, 3920, 3924, 3936, 3948,
      3952, 3960, 3968, 3972, 3984, 3996, 4000, 4008,
      4016, 4020, 4032, 4044, 4048, 4056, 4064, 4068,
      4080, 4092, 4096, 4104, 4112, 4116, 4128, 4140,
      4144, 4152, 4160, 4164, 4176, 4188, 4192, 4200,
      4208, 4212, 4224, 4236, 4240, 4248, 4256, 4260,
      4272, 4284, 4288, 4296, 4304, 4308, 4320, 4332,
      4336, 4344, 4352, 4356, 4368, 4380, 4384, 4392,
      4400, 4404, 4416, 4428, 4432, 4440, 4448, 4452,
      4464, 4476, 4480, 4488, 4496, 4500, 4512, 4524,
      4528, 4536, 4544, 4548, 4560, 4572, 4576, 4584,
      4592, 4596, 4608, 4620, 4624, 4632, 4640, 4644,
      4656, 4668, 4672, 4680, 4688, 4692, 4704, 4716,
      4720, 4728, 4736, 4740, 4752, 4764, 4768, 4776,
      4784, 4788, 4800, 4812, 4816, 4824, 4832, 4836,
      4848, 4860, 4864, 4872, 4880, 4884, 4896, 4908,
      4912, 4920, 4928, 4932, 4944, 4956, 4960, 4968,
      4976, 4980, 4992, 5004, 5008, 5016, 5024, 5028,
      5040, 5052, 5056, 5064, 5072, 5076, 5088, 5100,
      5104, 5112, 5120, 5124, 5136, 5148, 5152, 5160,
      5168, 5172, 5184, 5196, 5200, 5208, 5216, 5220,
      5232, 5244, 5248, 5256, 5264, 5268, 5280, 5292,
      5296, 5304, 5312, 5316, 5328, 5340, 5344, 5352,
      5360, 5364, 5376, 5388, 5392, 5400, 5408, 5412,
      5424, 5436, 5440, 5448, 5456, 5460, 5472, 5484,
      5488, 5496, 5504, 5508, 5520, 5532, 5536, 5544,
      5552, 5556, 5568, 5580, 5584, 5592, 5600, 5604,
      5616, 5628, 5632, 5640, 5648, 5652, 5664, 5676,
      5680, 5688, 5696, 5700, 5712, 5724, 5728, 5736,
      5744, 5748, 5760, 5772, 5776, 5784, 5792, 5796,
      5808, 5820, 5824, 5832, 5840, 5844, 5856, 5868,
      5872, 5880, 5888, 5892, 5904, 5916, 5920, 5928,
      5936, 5940, 5952, 5964, 5968, 5976, 5984, 5988,
      6000, 6012, 6016, 6024, 6032, 6036, 6048, 6060,
      6064, 6072, 6080, 6084, 6096, 6108, 6112, 6120,
      6128, 6132, 6144, 6160, 6176, 6192, 6208, 6224,
      6240, 6256, 6272, 6288, 6304, 6320, 6336, 6352,
      6368, 6384, 6400, 6416, 6432, 6448, 6464, 6480,
      6496, 6512, 6528, 6544, 6560, 6576, 6592, 6608,
      6624, 6640, 6656, 6672, 6688, 6704, 6720, 6736,
      6752, 6768, 6784, 6800, 6816, 6832, 6848, 6864,
      6880, 6896, 6912, 6928, 6944, 6960, 6976, 6992,
      7008, 7024, 7040, 7056, 7072, 7088, 7104, 7120,
      7136, 7152, 7168, 7184, 7200, 7216, 7232, 7248,
      7264, 7280, 7296, 7312, 7328, 7344, 7360, 7376,
      7392, 7408, 7424, 7440, 7456, 7472, 7488, 7504,
      7520, 7536, 7552, 7568, 7584, 7600, 7616, 7632,
      7648, 7664, 7680, 7696, 7712, 7728, 7744, 7760,
      7776, 7792, 7808, 7824, 7840, 7856, 7872, 7888,
      7904, 7920, 7936, 7952, 7968, 7984, 8000, 8016,
      8032, 8048, 8064, 8080, 8096, 8112, 8128, 8144,
      8160, 8176, 8192, 8208, 8256, 8304, 8320, 8352,
      8384, 8400, 8448, 8496, 8512, 8544, 8576, 8592,
      8640, 8688, 8704, 8736, 8768, 8784, 8832, 8880,
      8896, 8928, 8960, 8976, 9024, 9072, 9088, 9120,
      9152, 9168, 9216, 9264, 9280, 9312, 9344, 9360,
      9408, 9456, 9472, 9504, 9536, 9552, 9600, 9648,
      9664, 9696, 9728, 9744, 9792, 9840, 9856, 9888,
      9920, 9936, 9984, 10032, 10048, 10080, 10112, 10128,
      10176, 10224, 10240, 10272, 10304, 10320, 10368, 10416,
      10432, 10464, 10496, 10512, 10560, 10608, 10624, 10656,
      10688, 10704, 10752, 10800, 10816, 10848, 10880, 10896,
      10944, 10992, 11008, 11040, 11072, 11088, 11136, 11184,
      11200, 11232, 11264, 11280, 11328, 11376, 11392, 11424,
      11456, 11472, 11520, 11568, 11584, 11616, 11648, 11664,
      11712, 11760, 11776, 11808, 11840, 11856, 11904, 11952,
      11968, 12000, 12032, 12048, 12096, 12144, 12160, 12192,
      12224, 12240, 12288, 12352, 12416, 12480, 12544, 12608,
      12672, 12736, 12800, 12864, 12928, 12992, 13056, 13120,
      13184, 13248, 13312, 13376, 13440, 13504, 13568, 13632,
      13696, 13760, 13824, 13888, 13952, 14016, 14080, 14144,
      14208, 14272, 14336, 14400, 14464, 14528, 14592, 14656,
      14720, 14784, 14848, 14912, 14976, 15040, 15104, 15168,
      15232, 15296, 15360, 15424, 15488, 15552, 15616, 15680,
      15744, 15808, 15872, 15936, 16000, 16064, 16128, 16192,
      16256, 16320, 16384, 16512, 16640, 16704, 16896, 17088,
      17152, 17280, 17408, 17472, 17664, 17856, 17920, 18048,
      18176, 18240, 18432, 18624, 18688, 18816, 18944, 19008,
      19200, 19392, 19456, 19584, 19712, 19776, 19968, 20160,
      20224, 20352, 20480, 20544, 20736, 20928, 20992, 21120,
      21248, 21312, 21504, 21696, 21760, 21888, 22016, 22080,
      22272, 22464, 22528, 22656, 22784, 22848, 23040, 23232,
      23296, 23424, 23552, 23616, 23808, 24000, 24064, 24192,
      24320, 24384, 24576, 24832, 25088, 25344, 25600, 25856,
      26112, 26368, 26624, 26880, 27136, 27392, 27648, 27904,
      28160, 28416, 28672, 28928, 29184, 29440, 29696, 29952,
      30208, 30464, 30720, 30976, 31232, 31488, 31744, 32000,
      32256, 32512, 32768, 33024, 33792, 34560, 34816, 35328,
      35840, 36096, 36864, 37632, 37888, 38400, 38912, 39168,
      39936, 40704, 40960, 41472, 41984, 42240, 43008, 43776,
      44032, 44544, 45056, 45312, 46080, 46848, 47104, 47616,
      48128, 48384, 49152, 50176, 51200, 52224, 53248, 54272,
      55296, 56320, 57344, 58368, 59392, 60416, 61440, 62464,
      63488, 64512 }
  };
  return &parm;
}

// In offline encoding scenery, data file type will be changed. Below impl need to be changed also? 
RT_DATA_INFO* Get_rt_data_info() {
  static RT_DATA_INFO info = {
    "vgg16_cifar10.weight",
    "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX",
    DE_MSG_F32
  };
  return &info;
}

int32_t _cst_47[9] = {
  -33, -32, -31, -1, 0, 1, 31, 32,
  33
};
int32_t _cst_52[9] = {
  -33, -32, -31, -1, 0, 1, 31, 32,
  33
};
int32_t _cst_61[9] = {
  -17, -16, -15, -1, 0, 1, 15, 16,
  17
};
int32_t _cst_66[9] = {
  -17, -16, -15, -1, 0, 1, 15, 16,
  17
};
int32_t _cst_75[9] = {
  -9, -8, -7, -1, 0, 1, 7, 8,
  9
};
int32_t _cst_80[9] = {
  -9, -8, -7, -1, 0, 1, 7, 8,
  9
};
int32_t _cst_85[9] = {
  -9, -8, -7, -1, 0, 1, 7, 8,
  9
};
int32_t _cst_94[9] = {
  -5, -4, -3, -1, 0, 1, 3, 4,
  5
};
int32_t _cst_99[9] = {
  -5, -4, -3, -1, 0, 1, 3, 4,
  5
};
int32_t _cst_104[9] = {
  -5, -4, -3, -1, 0, 1, 3, 4,
  5
};
int32_t _cst_113[9] = {
  -3, -2, -1, -1, 0, 1, 1, 2,
  3
};
int32_t _cst_118[9] = {
  -3, -2, -1, -1, 0, 1, 1, 2,
  3
};
int32_t _cst_123[9] = {
  -3, -2, -1, -1, 0, 1, 1, 2,
  3
};
int32_t _cst_130[2] = {
  0, 1
};
