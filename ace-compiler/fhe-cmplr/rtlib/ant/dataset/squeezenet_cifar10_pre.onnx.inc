// external header files
#include "rt_ant/rt_ant.h"

typedef double float64_t;
typedef float float32_t;
typedef size_t LEVEL_T;
typedef double SCALE_T;

extern float32_t _cst_0[512][512][1][1];
extern float32_t _cst_1[512];
extern float32_t _cst_2[10][512];
extern float32_t _cst_3[10];
extern float32_t _cst_4[64][3][1][1];
extern float32_t _cst_5[64];
extern float32_t _cst_6[16][64][1][1];
extern float32_t _cst_7[16];
extern float32_t _cst_8[32][16][1][1];
extern float32_t _cst_9[32];
extern float32_t _cst_10[32][16][3][3];
extern float32_t _cst_11[32];
extern float32_t _cst_12[16][64][1][1];
extern float32_t _cst_13[16];
extern float32_t _cst_14[32][16][1][1];
extern float32_t _cst_15[32];
extern float32_t _cst_16[32][16][3][3];
extern float32_t _cst_17[32];
extern float32_t _cst_18[32][64][1][1];
extern float32_t _cst_19[32];
extern float32_t _cst_20[64][32][1][1];
extern float32_t _cst_21[64];
extern float32_t _cst_22[64][32][3][3];
extern float32_t _cst_23[64];
extern float32_t _cst_24[32][128][1][1];
extern float32_t _cst_25[32];
extern float32_t _cst_26[128][32][1][1];
extern float32_t _cst_27[128];
extern float32_t _cst_28[128][32][3][3];
extern float32_t _cst_29[128];
extern float32_t _cst_30[32][256][1][1];
extern float32_t _cst_31[32];
extern float32_t _cst_32[128][32][1][1];
extern float32_t _cst_33[128];
extern float32_t _cst_34[128][32][3][3];
extern float32_t _cst_35[128];
extern float32_t _cst_36[32][256][1][1];
extern float32_t _cst_37[32];
extern float32_t _cst_38[128][32][1][1];
extern float32_t _cst_39[128];
extern float32_t _cst_40[128][32][3][3];
extern float32_t _cst_41[128];
extern float32_t _cst_42[64][256][1][1];
extern float32_t _cst_43[64];
extern float32_t _cst_44[256][64][1][1];
extern float32_t _cst_45[256];
extern float32_t _cst_46[256][64][3][3];
extern float32_t _cst_47[256];
extern float32_t _cst_48[64][512][1][1];
extern float32_t _cst_49[64];
extern float32_t _cst_50[256][64][1][1];
extern float32_t _cst_51[256];
extern float32_t _cst_52[256][64][3][3];
extern float32_t _cst_53[256];
extern int64_t _cst_54[2];
extern int64_t _cst_55[2];
extern int64_t _cst_56[2];
extern int64_t _cst_57[2];
extern int64_t _cst_58[2];
extern int64_t _cst_59[2];
extern int64_t _cst_60[2];
extern int64_t _cst_61[2];
extern int64_t _cst_62[2];
extern int64_t _cst_63[2];
extern int64_t _cst_64[2];
extern int64_t _cst_65[2];
extern int64_t _cst_66[2];
extern int64_t _cst_67[2];
extern int64_t _cst_68[2];
extern int64_t _cst_69[2];
extern int32_t _cst_70[1];
extern float32_t _cst_71[4][65536];
extern float32_t _cst_72[65536];
extern int32_t _cst_73[1];
extern float32_t _cst_74[65536];
extern int32_t _cst_75[1];
extern float32_t _cst_76[64][16384];
extern float32_t _cst_77[16384];
extern int32_t _cst_78[1];
extern int32_t _cst_79[1];
extern float32_t _cst_80[16][32768];
extern float32_t _cst_81[32768];
extern int32_t _cst_82[1];
extern float32_t _cst_83[32768];
extern int32_t _cst_84[1];
extern float32_t _cst_85[144][32768];
extern float32_t _cst_86[32768];
extern int32_t _cst_87[9];
extern float32_t _cst_88[32768];
extern int32_t _cst_89[1];
extern float32_t _cst_90[64][16384];
extern float32_t _cst_91[16384];
extern int32_t _cst_92[1];
extern int32_t _cst_93[1];
extern float32_t _cst_94[16][32768];
extern float32_t _cst_95[32768];
extern int32_t _cst_96[1];
extern float32_t _cst_97[32768];
extern int32_t _cst_98[1];
extern float32_t _cst_99[144][32768];
extern float32_t _cst_100[32768];
extern int32_t _cst_101[9];
extern float32_t _cst_102[32768];
extern int32_t _cst_103[1];
extern float32_t _cst_104[64][32768];
extern float32_t _cst_105[32768];
extern int32_t _cst_106[1];
extern int32_t _cst_107[1];
extern float32_t _cst_108[32][65536];
extern float32_t _cst_109[65536];
extern int32_t _cst_110[4];
extern float32_t _cst_111[65536];
extern float32_t _cst_112[65536];
extern float32_t _cst_113[16][65536];
extern float32_t _cst_114[16][65536];
extern float32_t _cst_115[64][65536];
extern int32_t _cst_116[1];
extern float32_t _cst_117[288][65536];
extern float32_t _cst_118[65536];
extern int32_t _cst_119[9];
extern float32_t _cst_120[65536];
extern float32_t _cst_121[65536];
extern float32_t _cst_122[16][65536];
extern float32_t _cst_123[16][65536];
extern float32_t _cst_124[64][65536];
extern int32_t _cst_125[1];
extern float32_t _cst_126[128][8192];
extern float32_t _cst_127[8192];
extern int32_t _cst_128[1];
extern int32_t _cst_129[1];
extern float32_t _cst_130[32][32768];
extern float32_t _cst_131[32768];
extern int32_t _cst_132[4];
extern float32_t _cst_133[32768];
extern int32_t _cst_134[1];
extern float32_t _cst_135[288][32768];
extern float32_t _cst_136[32768];
extern int32_t _cst_137[9];
extern float32_t _cst_138[32768];
extern int32_t _cst_139[1];
extern float32_t _cst_140[256][8192];
extern float32_t _cst_141[8192];
extern int32_t _cst_142[1];
extern int32_t _cst_143[1];
extern float32_t _cst_144[32][32768];
extern float32_t _cst_145[32768];
extern int32_t _cst_146[4];
extern float32_t _cst_147[32768];
extern int32_t _cst_148[1];
extern float32_t _cst_149[288][32768];
extern float32_t _cst_150[32768];
extern int32_t _cst_151[9];
extern float32_t _cst_152[32768];
extern int32_t _cst_153[1];
extern float32_t _cst_154[256][8192];
extern float32_t _cst_155[8192];
extern int32_t _cst_156[1];
extern int32_t _cst_157[1];
extern float32_t _cst_158[32][32768];
extern float32_t _cst_159[32768];
extern int32_t _cst_160[4];
extern float32_t _cst_161[32768];
extern int32_t _cst_162[1];
extern float32_t _cst_163[288][32768];
extern float32_t _cst_164[32768];
extern int32_t _cst_165[9];
extern float32_t _cst_166[32768];
extern int32_t _cst_167[1];
extern float32_t _cst_168[256][16384];
extern float32_t _cst_169[16384];
extern int32_t _cst_170[1];
extern int32_t _cst_171[1];
extern float32_t _cst_172[64][65536];
extern float32_t _cst_173[65536];
extern int32_t _cst_174[8];
extern float32_t _cst_175[65536];
extern float32_t _cst_176[65536];
extern float32_t _cst_177[8][65536];
extern float32_t _cst_178[8][65536];
extern float32_t _cst_179[256][65536];
extern int32_t _cst_180[1];
extern float32_t _cst_181[576][65536];
extern float32_t _cst_182[65536];
extern int32_t _cst_183[9];
extern float32_t _cst_184[65536];
extern float32_t _cst_185[65536];
extern float32_t _cst_186[8][65536];
extern float32_t _cst_187[8][65536];
extern float32_t _cst_188[256][65536];
extern int32_t _cst_189[1];
extern float32_t _cst_190[512][4096];
extern float32_t _cst_191[4096];
extern int32_t _cst_192[1];
extern int32_t _cst_193[1];
extern float32_t _cst_194[64][16384];
extern float32_t _cst_195[16384];
extern int32_t _cst_196[8];
extern float32_t _cst_197[16384];
extern int32_t _cst_198[1];
extern float32_t _cst_199[576][16384];
extern float32_t _cst_200[16384];
extern int32_t _cst_201[9];
extern float32_t _cst_202[16384];
extern int32_t _cst_203[1];
extern float32_t _cst_204[512][32768];
extern float32_t _cst_205[32768];
extern int32_t _cst_206[16];
extern float32_t _cst_207[32768];
float32_t _cst_208 = 0.015625;
extern float32_t _cst_209[32768];
extern float32_t _cst_210[512][32768];
extern int32_t _cst_211[1];
extern float32_t _cst_212[8][1040];
extern int32_t _cst_213[2];
extern float32_t _cst_214[10];
float32_t _cst_215 = -1;
float32_t _cst_216 = -1;
float64_t _cst_217 = -3.809199432496630510769;
float32_t _cst_218 = -3.809199432496630510769;
float64_t _cst_219 = 2.460940868770910228136;
float32_t _cst_220 = 2.460940868770910228136;
float64_t _cst_221 = -1.430304794543664215212;
float32_t _cst_222 = -1.430304794543664215212;
float64_t _cst_223 = 2.944661935353764636858;
float32_t _cst_224 = 2.944661935353764636858;
float32_t _cst_225 = -1;
float32_t _cst_226 = -1;
float32_t _cst_227 = -1;
float32_t _cst_228 = -1;
float64_t _cst_229 = -0.0001600280537250875771867;
float32_t _cst_230 = -0.0001600280537250875771867;
float64_t _cst_231 = 0.003032478474127557870749;
float32_t _cst_232 = 0.003032478474127557870749;
float64_t _cst_233 = 0.1365828361194337536855;
float32_t _cst_234 = 0.1365828361194337536855;
float64_t _cst_235 = -0.02219735858216702675816;
float64_t _cst_236 = 0.5685231134608953462717;
float32_t _cst_237 = 0.5685231134608953462717;
float64_t _cst_238 = -0.5558785316215136829143;
float64_t _cst_239 = 1.614771079042867985009;
float32_t _cst_240 = 1.614771079042867985009;
float64_t _cst_241 = -0.6164083583294512891371;
float32_t _cst_242 = -1;
float32_t _cst_243 = -1;
float32_t _cst_244 = -1;
float32_t _cst_245 = -1;
float64_t _cst_246 = 0.0001111119162030077897477;
float32_t _cst_247 = 0.0001111119162030077897477;
float64_t _cst_248 = 0.006507995190210730772351;
float32_t _cst_249 = 0.006507995190210730772351;
float64_t _cst_250 = -0.0009371069270347270278171;
float32_t _cst_251 = -0.0009371069270347270278171;
float64_t _cst_252 = 0.1056744718173614761003;
float32_t _cst_253 = 0.1056744718173614761003;
float64_t _cst_254 = -0.03691063940349948585551;
float32_t _cst_255 = -0.03691063940349948585551;
float64_t _cst_256 = 0.6334139843709288841822;
float32_t _cst_257 = 0.6334139843709288841822;
float64_t _cst_258 = -0.2078597158810965850329;
float32_t _cst_259 = -0.2078597158810965850329;
float32_t _cst_260 = 0.5;
float32_t _cst_261 = 0.5;
float32_t _cst_262 = 0.5;
float32_t _cst_263 = 0.3333333333333333148296;
float32_t _cst_264 = 0.3333333333333333148296;
float32_t _cst_265 = 0.25;
float32_t _cst_266 = 0.25;
float32_t _cst_267 = 0.3333333333333333148296;
float32_t _cst_268 = 0.25;
float32_t _cst_269 = 0.3333333333333333148296;
float32_t _cst_270 = 0.25;
float32_t _cst_271 = 0.3333333333333333148296;
float32_t _cst_272 = 0.25;
float32_t _cst_273 = 0.3333333333333333148296;
float32_t _cst_274 = 0.2000000000000000111022;
float32_t _cst_275 = 0.3333333333333333148296;
float32_t _cst_276 = 0.2000000000000000111022;
float32_t _cst_277 = 0.07692307692307692734701;
float32_t _cst_278 = 0.09090909090909091161414;
CIPHERTEXT App_relu(CIPHERTEXT input_ct0, CIPHERTEXT input_ct1, uint32_t mask_len);
CIPHERTEXT Rotate(CIPHERTEXT ciph, int32_t rot_idx);
CIPHERTEXT Relinearize(CIPHERTEXT3 ciph);

#ifdef __cplusplus
extern "C"
#endif
bool Main_graph() {
  CIPHERTEXT input_0;
  CIPHERTEXT input_dup_n0_1;
  int32_t bs_input_n0_2;
  CIPHERTEXT input_block_n0_3[1];
  CIPHERTEXT result_n1_4;
  int32_t num_grid_n1_5;
  CIPHERTEXT result_grid_n1_6;
  int32_t cap_block_n1_7;
  CIPHERTEXT tmp_result_n2_8;
  CIPHERTEXT input_dup_n2_9;
  int32_t index_cin_n2_10;
  int32_t index_khw_n2_11;
  CIPHERTEXT input_dup_n2_12;
  int32_t bs_input_n2_13;
  CIPHERTEXT input_block_n2_14[1];
  CIPHERTEXT result_n3_15;
  int32_t num_grid_n3_16;
  CIPHERTEXT result_grid_n3_17;
  int32_t cap_block_n3_18;
  CIPHERTEXT input_dup_n3_19;
  int32_t bs_input_n3_20;
  CIPHERTEXT input_block_n3_21[9];
  int32_t bs_input_n3_rs2_22;
  CIPHERTEXT result_n4_23;
  int32_t num_grid_n4_24;
  CIPHERTEXT result_grid_n4_25;
  int32_t cap_block_n4_26;
  CIPHERTEXT tmp_result_n5_27;
  CIPHERTEXT input_dup_n5_28;
  int32_t index_cin_n5_29;
  int32_t index_khw_n5_30;
  CIPHERTEXT input_dup_n5_31;
  int32_t bs_input_n5_32;
  CIPHERTEXT input_block_n5_33[1];
  CIPHERTEXT result_n6_34;
  int32_t num_grid_n6_35;
  CIPHERTEXT result_grid_n6_36;
  int32_t cap_block_n6_37;
  CIPHERTEXT input_dup_n6_38;
  int32_t bs_input_n6_39;
  CIPHERTEXT input_block_n6_40[9];
  int32_t bs_input_n6_rs2_41;
  CIPHERTEXT result_n7_42;
  int32_t num_grid_n7_43;
  CIPHERTEXT result_grid_n7_44;
  int32_t cap_block_n7_45;
  CIPHERTEXT tmp_result_n8_46;
  CIPHERTEXT input_dup_n8_47;
  int32_t index_cin_n8_48;
  int32_t index_khw_n8_49;
  CIPHERTEXT input_dup_n8_50;
  CIPHERTEXT input_block_n8_51[4];
  int32_t bs_input_n8_52;
  CIPHERTEXT result_n9_53;
  int32_t num_grid_n9_54;
  CIPHERTEXT result_grid_n9_55;
  int32_t cap_block_n9_56;
  CIPHERTEXT tmp_row_add9_57;
  CIPHERTEXT orig_result9_58;
  CIPHERTEXT comb_row_result10_59;
  int32_t comb_row_index_n10_60;
  CIPHERTEXT comb_rc_result10_61;
  int32_t comb_rc_index_n10_62;
  CIPHERTEXT ss_result10_63;
  int32_t comb_cc_index_n10_64;
  CIPHERTEXT input_dup_n10_65;
  int32_t bs_input_n10_66;
  CIPHERTEXT input_block_n10_67[9];
  int32_t bs_input_n10_rs2_68;
  CIPHERTEXT result_n11_69;
  int32_t num_grid_n11_70;
  CIPHERTEXT result_grid_n11_71;
  int32_t cap_block_n11_72;
  CIPHERTEXT tmp_row_add11_73;
  CIPHERTEXT orig_result11_74;
  CIPHERTEXT comb_row_result12_75;
  int32_t comb_row_index_n12_76;
  CIPHERTEXT comb_rc_result12_77;
  int32_t comb_rc_index_n12_78;
  CIPHERTEXT ss_result12_79;
  int32_t comb_cc_index_n12_80;
  CIPHERTEXT tmp_result_n13_81;
  CIPHERTEXT input_dup_n13_82;
  int32_t index_cin_n13_83;
  int32_t index_khw_n13_84;
  CIPHERTEXT input_dup_n13_85;
  CIPHERTEXT input_block_n13_86[4];
  int32_t bs_input_n13_87;
  CIPHERTEXT result_n14_88;
  int32_t num_grid_n14_89;
  CIPHERTEXT result_grid_n14_90;
  int32_t cap_block_n14_91;
  CIPHERTEXT input_dup_n14_92;
  int32_t bs_input_n14_93;
  CIPHERTEXT input_block_n14_94[9];
  int32_t bs_input_n14_rs2_95;
  CIPHERTEXT result_n15_96;
  int32_t num_grid_n15_97;
  CIPHERTEXT result_grid_n15_98;
  int32_t cap_block_n15_99;
  CIPHERTEXT tmp_result_n16_100;
  CIPHERTEXT input_dup_n16_101;
  int32_t index_cin_n16_102;
  int32_t index_khw_n16_103;
  CIPHERTEXT input_dup_n16_104;
  CIPHERTEXT input_block_n16_105[4];
  int32_t bs_input_n16_106;
  CIPHERTEXT result_n17_107;
  int32_t num_grid_n17_108;
  CIPHERTEXT result_grid_n17_109;
  int32_t cap_block_n17_110;
  CIPHERTEXT input_dup_n17_111;
  int32_t bs_input_n17_112;
  CIPHERTEXT input_block_n17_113[9];
  int32_t bs_input_n17_rs2_114;
  CIPHERTEXT result_n18_115;
  int32_t num_grid_n18_116;
  CIPHERTEXT result_grid_n18_117;
  int32_t cap_block_n18_118;
  CIPHERTEXT tmp_result_n19_119;
  CIPHERTEXT input_dup_n19_120;
  int32_t index_cin_n19_121;
  int32_t index_khw_n19_122;
  CIPHERTEXT input_dup_n19_123;
  CIPHERTEXT input_block_n19_124[4];
  int32_t bs_input_n19_125;
  CIPHERTEXT result_n20_126;
  int32_t num_grid_n20_127;
  CIPHERTEXT result_grid_n20_128;
  int32_t cap_block_n20_129;
  CIPHERTEXT input_dup_n20_130;
  int32_t bs_input_n20_131;
  CIPHERTEXT input_block_n20_132[9];
  int32_t bs_input_n20_rs2_133;
  CIPHERTEXT result_n21_134;
  int32_t num_grid_n21_135;
  CIPHERTEXT result_grid_n21_136;
  int32_t cap_block_n21_137;
  CIPHERTEXT tmp_result_n22_138;
  CIPHERTEXT input_dup_n22_139;
  int32_t index_cin_n22_140;
  int32_t index_khw_n22_141;
  CIPHERTEXT input_dup_n22_142;
  CIPHERTEXT input_block_n22_143[8];
  int32_t bs_input_n22_144;
  CIPHERTEXT result_n23_145;
  int32_t num_grid_n23_146;
  CIPHERTEXT result_grid_n23_147;
  int32_t cap_block_n23_148;
  CIPHERTEXT tmp_row_add23_149;
  CIPHERTEXT orig_result23_150;
  CIPHERTEXT comb_row_result24_151;
  int32_t comb_row_index_n24_152;
  CIPHERTEXT comb_rc_result24_153;
  int32_t comb_rc_index_n24_154;
  CIPHERTEXT ss_result24_155;
  int32_t comb_cc_index_n24_156;
  CIPHERTEXT input_dup_n24_157;
  int32_t bs_input_n24_158;
  CIPHERTEXT input_block_n24_159[9];
  int32_t bs_input_n24_rs2_160;
  CIPHERTEXT result_n25_161;
  int32_t num_grid_n25_162;
  CIPHERTEXT result_grid_n25_163;
  int32_t cap_block_n25_164;
  CIPHERTEXT tmp_row_add25_165;
  CIPHERTEXT orig_result25_166;
  CIPHERTEXT comb_row_result26_167;
  int32_t comb_row_index_n26_168;
  CIPHERTEXT comb_rc_result26_169;
  int32_t comb_rc_index_n26_170;
  CIPHERTEXT ss_result26_171;
  int32_t comb_cc_index_n26_172;
  CIPHERTEXT tmp_result_n27_173;
  CIPHERTEXT input_dup_n27_174;
  int32_t index_cin_n27_175;
  int32_t index_khw_n27_176;
  CIPHERTEXT input_dup_n27_177;
  CIPHERTEXT input_block_n27_178[8];
  int32_t bs_input_n27_179;
  CIPHERTEXT result_n28_180;
  int32_t num_grid_n28_181;
  CIPHERTEXT result_grid_n28_182;
  int32_t cap_block_n28_183;
  CIPHERTEXT input_dup_n28_184;
  int32_t bs_input_n28_185;
  CIPHERTEXT input_block_n28_186[9];
  int32_t bs_input_n28_rs2_187;
  CIPHERTEXT result_n29_188;
  int32_t num_grid_n29_189;
  CIPHERTEXT result_grid_n29_190;
  int32_t cap_block_n29_191;
  CIPHERTEXT input_dup_n29_192;
  int32_t bs_input_n29_193;
  CIPHERTEXT input_block_n29_194[16];
  CIPHERTEXT result_n30_195;
  int32_t num_grid_n30_196;
  CIPHERTEXT result_grid_n30_197;
  int32_t cap_block_n30_198;
  int32_t gap_sum_index_n30_199;
  CIPHERTEXT gap_result30_200;
  CIPHERTEXT orig_result30_201;
  CIPHERTEXT ss_result31_202;
  int32_t comb_cc_index_n31_203;
  CIPHERTEXT input_dup_n31_204;
  CIPHERTEXT input_block_n31_205[2];
  int32_t bs_input_n31_206;
  CIPHERTEXT gemm_result_n32_207;
  int32_t gs_n32_208;
  CIPHERTEXT block_result_209;
  int32_t bs_n32_210;
  CIPHERTEXT reduce_intra_211;
  int32_t ri_n32_212;
  CIPHERTEXT reduce_intra_213;
  int32_t ri_n32_214;
  CIPHERTEXT output_215;
  CIPHERTEXT _ckks_gen_tmp_118_216;
  CIPHERTEXT _ckks_gen_tmp_351_217;
  CIPHERTEXT _ckks_gen_tmp_387_218;
  CIPHERTEXT _ckks_gen_tmp_504_219;
  CIPHERTEXT _ckks_gen_tmp_540_220;
  CIPHERTEXT _ckks_gen_tmp_758_221;
  CIPHERTEXT _ckks_gen_tmp_794_222;
  CIPHERTEXT _ckks_gen_tmp_911_223;
  CIPHERTEXT _ckks_gen_tmp_947_224;
  CIPHERTEXT _ckks_gen_tmp_1179_225;
  CIPHERTEXT _ckks_gen_tmp_1256_226;
  CIPHERTEXT _ckks_gen_tmp_1288_227;
  CIPHERTEXT _ckks_gen_tmp_1322_228;
  CIPHERTEXT _ckks_gen_tmp_1356_229;
  CIPHERTEXT _ckks_gen_tmp_1466_230;
  CIPHERTEXT _ckks_gen_tmp_1543_231;
  CIPHERTEXT _ckks_gen_tmp_1575_232;
  CIPHERTEXT _ckks_gen_tmp_1609_233;
  CIPHERTEXT _ckks_gen_tmp_1643_234;
  CIPHERTEXT _ckks_gen_tmp_1880_235;
  CIPHERTEXT _ckks_gen_tmp_1916_236;
  CIPHERTEXT _ckks_gen_tmp_2041_237;
  CIPHERTEXT _ckks_gen_tmp_2077_238;
  CIPHERTEXT _ckks_gen_tmp_2317_239;
  CIPHERTEXT _ckks_gen_tmp_2353_240;
  CIPHERTEXT _ckks_gen_tmp_2478_241;
  CIPHERTEXT _ckks_gen_tmp_2514_242;
  CIPHERTEXT _ckks_gen_tmp_2754_243;
  CIPHERTEXT _ckks_gen_tmp_2790_244;
  CIPHERTEXT _ckks_gen_tmp_2915_245;
  CIPHERTEXT _ckks_gen_tmp_2951_246;
  CIPHERTEXT _ckks_gen_tmp_3191_247;
  CIPHERTEXT _ckks_gen_tmp_3268_248;
  CIPHERTEXT _ckks_gen_tmp_3300_249;
  CIPHERTEXT _ckks_gen_tmp_3334_250;
  CIPHERTEXT _ckks_gen_tmp_3368_251;
  CIPHERTEXT _ckks_gen_tmp_3486_252;
  CIPHERTEXT _ckks_gen_tmp_3563_253;
  CIPHERTEXT _ckks_gen_tmp_3595_254;
  CIPHERTEXT _ckks_gen_tmp_3629_255;
  CIPHERTEXT _ckks_gen_tmp_3663_256;
  CIPHERTEXT _ckks_gen_tmp_3900_257;
  CIPHERTEXT _ckks_gen_tmp_3936_258;
  CIPHERTEXT _ckks_gen_tmp_4061_259;
  CIPHERTEXT _ckks_gen_tmp_4097_260;
  CIPHERTEXT _ckks_gen_tmp_4224_261;
  CIPHERTEXT _ckks_gen_tmp_4260_262;
  CIPHERTEXT _ckks_gen_tmp_4373_263;
  CIPHERTEXT _ckks_gen_tmp_4477_264;
  MODULUS* _pgen_modulus_265;
  uint32_t _pgen_num_q_266;
  uint32_t _pgen_rns_idx_267;
  CIPHERTEXT _pgen_tmp_0_268;
  CIPHERTEXT _pgen_tmp_1_269;
  CIPHERTEXT _pgen_tmp_2_270;
  CIPHERTEXT _pgen_tmp_3_271;
  CIPHERTEXT _pgen_tmp_4_272;
  CIPHERTEXT _pgen_tmp_5_273;
  CIPHERTEXT _pgen_tmp_6_274;
  CIPHERTEXT _pgen_tmp_7_275;
  CIPHERTEXT _pgen_tmp_8_276;
  CIPHERTEXT _pgen_tmp_9_277;
  CIPHERTEXT _pgen_tmp_10_278;
  CIPHERTEXT _pgen_tmp_11_279;
  CIPHERTEXT _pgen_tmp_12_280;
  CIPHERTEXT _pgen_tmp_13_281;
  PLAINTEXT _pgen_tmp_14_282;
  CIPHERTEXT _pgen_tmp_15_283;
  CIPHERTEXT _pgen_tmp_16_284;
  PLAINTEXT _pgen_tmp_17_285;
  PLAINTEXT _pgen_tmp_18_286;
  CIPHERTEXT _pgen_tmp_19_287;
  PLAINTEXT _pgen_tmp_20_288;
  PLAINTEXT _pgen_tmp_21_289;
  CIPHERTEXT _pgen_tmp_22_290;
  PLAINTEXT _pgen_tmp_23_291;
  CIPHERTEXT _pgen_tmp_24_292;
  PLAINTEXT _pgen_tmp_25_293;
  PLAINTEXT _pgen_tmp_26_294;
  CIPHERTEXT _pgen_tmp_27_295;
  CIPHERTEXT _pgen_tmp_28_296;
  PLAINTEXT _pgen_tmp_29_297;
  PLAINTEXT _pgen_tmp_30_298;
  PLAINTEXT _pgen_tmp_31_299;
  CIPHERTEXT _pgen_tmp_32_300;
  CIPHERTEXT _pgen_tmp_33_301;
  PLAINTEXT _pgen_tmp_34_302;
  PLAINTEXT _pgen_tmp_35_303;
  CIPHERTEXT _pgen_tmp_36_304;
  PLAINTEXT _pgen_tmp_37_305;
  PLAINTEXT _pgen_tmp_38_306;
  CIPHERTEXT _pgen_tmp_39_307;
  PLAINTEXT _pgen_tmp_40_308;
  CIPHERTEXT _pgen_tmp_41_309;
  PLAINTEXT _pgen_tmp_42_310;
  PLAINTEXT _pgen_tmp_43_311;
  CIPHERTEXT _pgen_tmp_44_312;
  CIPHERTEXT _pgen_tmp_45_313;
  PLAINTEXT _pgen_tmp_46_314;
  PLAINTEXT _pgen_tmp_47_315;
  PLAINTEXT _pgen_tmp_48_316;
  CIPHERTEXT _pgen_tmp_49_317;
  CIPHERTEXT _pgen_tmp_50_318;
  PLAINTEXT _pgen_tmp_51_319;
  PLAINTEXT _pgen_tmp_52_320;
  CIPHERTEXT _pgen_tmp_53_321;
  PLAINTEXT _pgen_tmp_54_322;
  PLAINTEXT _pgen_tmp_55_323;
  CIPHERTEXT _pgen_tmp_56_324;
  PLAINTEXT _pgen_tmp_57_325;
  CIPHERTEXT _pgen_tmp_58_326;
  PLAINTEXT _pgen_tmp_59_327;
  CIPHERTEXT _pgen_tmp_60_328;
  PLAINTEXT _pgen_tmp_61_329;
  CIPHERTEXT _pgen_tmp_62_330;
  CIPHERTEXT _pgen_tmp_63_331;
  PLAINTEXT _pgen_tmp_64_332;
  PLAINTEXT _pgen_tmp_65_333;
  CIPHERTEXT _pgen_tmp_66_334;
  PLAINTEXT _pgen_tmp_67_335;
  PLAINTEXT _pgen_tmp_68_336;
  CIPHERTEXT _pgen_tmp_69_337;
  PLAINTEXT _pgen_tmp_70_338;
  CIPHERTEXT _pgen_tmp_71_339;
  PLAINTEXT _pgen_tmp_72_340;
  CIPHERTEXT _pgen_tmp_73_341;
  PLAINTEXT _pgen_tmp_74_342;
  CIPHERTEXT _pgen_tmp_75_343;
  CIPHERTEXT _pgen_tmp_76_344;
  PLAINTEXT _pgen_tmp_77_345;
  PLAINTEXT _pgen_tmp_78_346;
  CIPHERTEXT _pgen_tmp_79_347;
  PLAINTEXT _pgen_tmp_80_348;
  PLAINTEXT _pgen_tmp_81_349;
  CIPHERTEXT _pgen_tmp_82_350;
  PLAINTEXT _pgen_tmp_83_351;
  CIPHERTEXT _pgen_tmp_84_352;
  PLAINTEXT _pgen_tmp_85_353;
  CIPHERTEXT _pgen_tmp_86_354;
  CIPHERTEXT _pgen_tmp_87_355;
  PLAINTEXT _pgen_tmp_88_356;
  PLAINTEXT _pgen_tmp_89_357;
  CIPHERTEXT _pgen_tmp_90_358;
  PLAINTEXT _pgen_tmp_91_359;
  CIPHERTEXT _pgen_tmp_92_360;
  PLAINTEXT _pgen_tmp_93_361;
  CIPHERTEXT _pgen_tmp_94_362;
  CIPHERTEXT _pgen_tmp_95_363;
  CIPHERTEXT _pgen_tmp_96_364;
  PLAINTEXT _pgen_tmp_97_365;
  CIPHERTEXT _pgen_tmp_98_366;
  CIPHERTEXT _pgen_tmp_99_367;
  PLAINTEXT _pgen_tmp_100_368;
  PLAINTEXT _pgen_tmp_101_369;
  CIPHERTEXT _pgen_tmp_102_370;
  CIPHERTEXT _pgen_tmp_103_371;
  PLAINTEXT _pgen_tmp_104_372;
  CIPHERTEXT _pgen_tmp_105_373;
  CIPHERTEXT _pgen_tmp_106_374;
  PLAINTEXT _pgen_tmp_107_375;
  PLAINTEXT _pgen_tmp_108_376;
  CIPHERTEXT _pgen_tmp_109_377;
  PLAINTEXT _pgen_tmp_110_378;
  PLAINTEXT _pgen_tmp_111_379;
  CIPHERTEXT _pgen_tmp_112_380;
  PLAINTEXT _pgen_tmp_113_381;
  CIPHERTEXT _pgen_tmp_114_382;
  PLAINTEXT _pgen_tmp_115_383;
  CIPHERTEXT _pgen_tmp_116_384;
  CIPHERTEXT _pgen_tmp_117_385;
  CIPHERTEXT _pgen_tmp_118_386;
  PLAINTEXT _pgen_tmp_119_387;
  CIPHERTEXT _pgen_tmp_120_388;
  CIPHERTEXT _pgen_tmp_121_389;
  PLAINTEXT _pgen_tmp_122_390;
  PLAINTEXT _pgen_tmp_123_391;
  CIPHERTEXT _pgen_tmp_124_392;
  CIPHERTEXT _pgen_tmp_125_393;
  PLAINTEXT _pgen_tmp_126_394;
  CIPHERTEXT _pgen_tmp_127_395;
  CIPHERTEXT _pgen_tmp_128_396;
  PLAINTEXT _pgen_tmp_129_397;
  PLAINTEXT _pgen_tmp_130_398;
  CIPHERTEXT _pgen_tmp_131_399;
  PLAINTEXT _pgen_tmp_132_400;
  PLAINTEXT _pgen_tmp_133_401;
  CIPHERTEXT _pgen_tmp_134_402;
  PLAINTEXT _pgen_tmp_135_403;
  CIPHERTEXT _pgen_tmp_136_404;
  PLAINTEXT _pgen_tmp_137_405;
  CIPHERTEXT _pgen_tmp_138_406;
  CIPHERTEXT _pgen_tmp_139_407;
  CIPHERTEXT _pgen_tmp_140_408;
  PLAINTEXT _pgen_tmp_141_409;
  CIPHERTEXT _pgen_tmp_142_410;
  CIPHERTEXT _pgen_tmp_143_411;
  PLAINTEXT _pgen_tmp_144_412;
  PLAINTEXT _pgen_tmp_145_413;
  CIPHERTEXT _pgen_tmp_146_414;
  CIPHERTEXT _pgen_tmp_147_415;
  PLAINTEXT _pgen_tmp_148_416;
  CIPHERTEXT _pgen_tmp_149_417;
  CIPHERTEXT _pgen_tmp_150_418;
  PLAINTEXT _pgen_tmp_151_419;
  PLAINTEXT _pgen_tmp_152_420;
  CIPHERTEXT _pgen_tmp_153_421;
  PLAINTEXT _pgen_tmp_154_422;
  PLAINTEXT _pgen_tmp_155_423;
  CIPHERTEXT _pgen_tmp_156_424;
  PLAINTEXT _pgen_tmp_157_425;
  CIPHERTEXT _pgen_tmp_158_426;
  PLAINTEXT _pgen_tmp_159_427;
  CIPHERTEXT _pgen_tmp_160_428;
  CIPHERTEXT _pgen_tmp_161_429;
  CIPHERTEXT _pgen_tmp_162_430;
  PLAINTEXT _pgen_tmp_163_431;
  CIPHERTEXT _pgen_tmp_164_432;
  CIPHERTEXT _pgen_tmp_165_433;
  PLAINTEXT _pgen_tmp_166_434;
  PLAINTEXT _pgen_tmp_167_435;
  CIPHERTEXT _pgen_tmp_168_436;
  PLAINTEXT _pgen_tmp_169_437;
  PLAINTEXT _pgen_tmp_170_438;
  CIPHERTEXT _pgen_tmp_171_439;
  PLAINTEXT _pgen_tmp_172_440;
  CIPHERTEXT _pgen_tmp_173_441;
  PLAINTEXT _pgen_tmp_174_442;
  CIPHERTEXT _pgen_tmp_175_443;
  CIPHERTEXT _pgen_tmp_176_444;
  CIPHERTEXT _pgen_tmp_177_445;
  PLAINTEXT _pgen_tmp_178_446;
  CIPHERTEXT _pgen_tmp_179_447;
  CIPHERTEXT _pgen_tmp_180_448;
  PLAINTEXT _pgen_tmp_181_449;
  PLAINTEXT _pgen_tmp_182_450;
  CIPHERTEXT _pgen_tmp_183_451;
  PLAINTEXT _pgen_tmp_184_452;
  PLAINTEXT _pgen_tmp_185_453;
  CIPHERTEXT _pgen_tmp_186_454;
  PLAINTEXT _pgen_tmp_187_455;
  CIPHERTEXT _pgen_tmp_188_456;
  PLAINTEXT _pgen_tmp_189_457;
  CIPHERTEXT _pgen_tmp_190_458;
  CIPHERTEXT _pgen_tmp_191_459;
  PLAINTEXT _pgen_tmp_192_460;
  PLAINTEXT _pgen_tmp_193_461;
  CIPHERTEXT _pgen_tmp_194_462;
  PLAINTEXT _pgen_tmp_195_463;
  CIPHERTEXT _pgen_tmp_196_464;
  PLAINTEXT _pgen_tmp_197_465;
  CIPHERTEXT _pgen_tmp_198_466;
  CIPHERTEXT _pgen_tmp_199_467;
  CIPHERTEXT _pgen_tmp_200_468;
  PLAINTEXT _pgen_tmp_201_469;
  CIPHERTEXT _pgen_tmp_202_470;
  CIPHERTEXT _pgen_tmp_203_471;
  PLAINTEXT _pgen_tmp_204_472;
  PLAINTEXT _pgen_tmp_205_473;
  CIPHERTEXT _pgen_tmp_206_474;
  CIPHERTEXT _pgen_tmp_207_475;
  PLAINTEXT _pgen_tmp_208_476;
  CIPHERTEXT _pgen_tmp_209_477;
  CIPHERTEXT _pgen_tmp_210_478;
  PLAINTEXT _pgen_tmp_211_479;
  PLAINTEXT _pgen_tmp_212_480;
  CIPHERTEXT _pgen_tmp_213_481;
  PLAINTEXT _pgen_tmp_214_482;
  PLAINTEXT _pgen_tmp_215_483;
  CIPHERTEXT _pgen_tmp_216_484;
  CIPHERTEXT _pgen_tmp_217_485;
  PLAINTEXT _pgen_tmp_218_486;
  PLAINTEXT _pgen_tmp_219_487;
  CIPHERTEXT _pgen_tmp_220_488;
  PLAINTEXT _pgen_tmp_221_489;
  CIPHERTEXT _pgen_tmp_222_490;
  PLAINTEXT _pgen_tmp_223_491;
  CIPHERTEXT _pgen_tmp_224_492;
  PLAINTEXT _pgen_tmp_225_493;
  PLAINTEXT _pgen_tmp_226_494;
  CIPHERTEXT _pgen_tmp_227_495;
  CIPHERTEXT _pgen_tmp_228_496;
  PLAINTEXT _pgen_tmp_229_497;
  CIPHERTEXT _pgen_tmp_230_498;
  PLAINTEXT _pgen_tmp_231_499;
  CIPHERTEXT _pgen_tmp_232_500;
  PLAINTEXT _pgen_tmp_233_501;
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
  CIPHERTEXT _preg_268435690;
  CIPHERTEXT _preg_268435691;
  CIPHERTEXT _preg_268435692;
  CIPHERTEXT _preg_268435693;
  CIPHERTEXT _preg_268435694;
  CIPHERTEXT _preg_268435695;
  CIPHERTEXT _preg_268435696;
  CIPHERTEXT _preg_268435697;
  CIPHERTEXT _preg_268435698;
  CIPHERTEXT _preg_268435699;
  CIPHERTEXT _preg_268435700;
  CIPHERTEXT _preg_268435701;
  CIPHERTEXT _preg_268435702;
  CIPHERTEXT _preg_268435703;
  CIPHERTEXT _preg_268435704;
  CIPHERTEXT _preg_268435705;
  CIPHERTEXT _preg_268435706;
  CIPHERTEXT _preg_268435707;
  CIPHERTEXT _preg_268435708;
  CIPHERTEXT _preg_268435709;
  CIPHERTEXT _preg_268435710;
  CIPHERTEXT _preg_268435711;
  CIPHERTEXT _preg_268435712;
  CIPHERTEXT _preg_268435713;
  CIPHERTEXT _preg_268435714;
  CIPHERTEXT _preg_268435715;
  CIPHERTEXT _preg_268435716;
  CIPHERTEXT _preg_268435717;
  CIPHERTEXT _preg_268435718;
  CIPHERTEXT _preg_268435719;
  CIPHERTEXT _preg_268435720;
  CIPHERTEXT _preg_268435721;
  CIPHERTEXT _preg_268435722;
  CIPHERTEXT _preg_268435723;
  CIPHERTEXT _preg_268435724;
  CIPHERTEXT _preg_268435725;
  CIPHERTEXT _preg_268435726;
  CIPHERTEXT _preg_268435727;
  CIPHERTEXT _preg_268435728;
  CIPHERTEXT _preg_268435729;
  CIPHERTEXT _preg_268435730;
  CIPHERTEXT _preg_268435731;
  CIPHERTEXT _preg_268435732;
  CIPHERTEXT _preg_268435733;
  CIPHERTEXT _preg_268435734;
  CIPHERTEXT _preg_268435735;
  CIPHERTEXT _preg_268435736;
  CIPHERTEXT _preg_268435737;
  CIPHERTEXT _preg_268435738;
  CIPHERTEXT _preg_268435739;
  CIPHERTEXT _preg_268435740;
  CIPHERTEXT _preg_268435741;
  CIPHERTEXT _preg_268435742;
  CIPHERTEXT _preg_268435743;
  CIPHERTEXT _preg_268435744;
  CIPHERTEXT _preg_268435745;
  CIPHERTEXT _preg_268435746;
  CIPHERTEXT _preg_268435747;
  CIPHERTEXT _preg_268435748;
  CIPHERTEXT _preg_268435749;
  CIPHERTEXT _preg_268435750;
  CIPHERTEXT _preg_268435751;
  CIPHERTEXT _preg_268435752;
  CIPHERTEXT _preg_268435753;
  CIPHERTEXT _preg_268435754;
  CIPHERTEXT _preg_268435755;
  CIPHERTEXT _preg_268435756;
  CIPHERTEXT _preg_268435757;
  CIPHERTEXT _preg_268435758;
  CIPHERTEXT _preg_268435759;
  CIPHERTEXT _preg_268435760;
  CIPHERTEXT _preg_268435761;
  CIPHERTEXT _preg_268435762;
  CIPHERTEXT _preg_268435763;
  CIPHERTEXT _preg_268435764;
  CIPHERTEXT _preg_268435765;
  CIPHERTEXT _preg_268435766;
  CIPHERTEXT _preg_268435767;
  CIPHERTEXT _preg_268435768;
  CIPHERTEXT _preg_268435769;
  CIPHERTEXT _preg_268435770;
  CIPHERTEXT _preg_268435771;
  CIPHERTEXT _preg_268435772;
  CIPHERTEXT _preg_268435773;
  CIPHERTEXT _preg_268435774;
  CIPHERTEXT _preg_268435775;
  CIPHERTEXT _preg_268435776;
  CIPHERTEXT _preg_268435777;
  CIPHERTEXT _preg_268435778;
  CIPHERTEXT _preg_268435779;
  CIPHERTEXT _preg_268435780;
  CIPHERTEXT _preg_268435781;
  CIPHERTEXT _preg_268435782;
  CIPHERTEXT _preg_268435783;
  CIPHERTEXT _preg_268435784;
  CIPHERTEXT _preg_268435785;
  CIPHERTEXT _preg_268435786;
  CIPHERTEXT _preg_268435787;
  CIPHERTEXT _preg_268435788;
  CIPHERTEXT _preg_268435789;
  CIPHERTEXT _preg_268435790;
  CIPHERTEXT _preg_268435791;
  CIPHERTEXT _preg_268435792;
  CIPHERTEXT _preg_268435793;
  CIPHERTEXT _preg_268435794;
  CIPHERTEXT _preg_268435795;
  CIPHERTEXT _preg_268435796;
  CIPHERTEXT _preg_268435797;
  uint32_t  degree = Degree();
  input_0 = Get_input_data("input", 0);
  memset(&input_dup_n0_1, 0, sizeof(input_dup_n0_1));
  memset(&input_block_n0_3, 0, sizeof(input_block_n0_3));
  memset(&result_n1_4, 0, sizeof(result_n1_4));
  memset(&result_grid_n1_6, 0, sizeof(result_grid_n1_6));
  memset(&tmp_result_n2_8, 0, sizeof(tmp_result_n2_8));
  memset(&input_dup_n2_9, 0, sizeof(input_dup_n2_9));
  memset(&input_dup_n2_12, 0, sizeof(input_dup_n2_12));
  memset(&input_block_n2_14, 0, sizeof(input_block_n2_14));
  memset(&result_n3_15, 0, sizeof(result_n3_15));
  memset(&result_grid_n3_17, 0, sizeof(result_grid_n3_17));
  memset(&input_dup_n3_19, 0, sizeof(input_dup_n3_19));
  memset(&input_block_n3_21, 0, sizeof(input_block_n3_21));
  memset(&result_n4_23, 0, sizeof(result_n4_23));
  memset(&result_grid_n4_25, 0, sizeof(result_grid_n4_25));
  memset(&tmp_result_n5_27, 0, sizeof(tmp_result_n5_27));
  memset(&input_dup_n5_28, 0, sizeof(input_dup_n5_28));
  memset(&input_dup_n5_31, 0, sizeof(input_dup_n5_31));
  memset(&input_block_n5_33, 0, sizeof(input_block_n5_33));
  memset(&result_n6_34, 0, sizeof(result_n6_34));
  memset(&result_grid_n6_36, 0, sizeof(result_grid_n6_36));
  memset(&input_dup_n6_38, 0, sizeof(input_dup_n6_38));
  memset(&input_block_n6_40, 0, sizeof(input_block_n6_40));
  memset(&result_n7_42, 0, sizeof(result_n7_42));
  memset(&result_grid_n7_44, 0, sizeof(result_grid_n7_44));
  memset(&tmp_result_n8_46, 0, sizeof(tmp_result_n8_46));
  memset(&input_dup_n8_47, 0, sizeof(input_dup_n8_47));
  memset(&input_dup_n8_50, 0, sizeof(input_dup_n8_50));
  memset(&input_block_n8_51, 0, sizeof(input_block_n8_51));
  memset(&result_n9_53, 0, sizeof(result_n9_53));
  memset(&result_grid_n9_55, 0, sizeof(result_grid_n9_55));
  memset(&tmp_row_add9_57, 0, sizeof(tmp_row_add9_57));
  memset(&orig_result9_58, 0, sizeof(orig_result9_58));
  memset(&comb_row_result10_59, 0, sizeof(comb_row_result10_59));
  memset(&comb_rc_result10_61, 0, sizeof(comb_rc_result10_61));
  memset(&ss_result10_63, 0, sizeof(ss_result10_63));
  memset(&input_dup_n10_65, 0, sizeof(input_dup_n10_65));
  memset(&input_block_n10_67, 0, sizeof(input_block_n10_67));
  memset(&result_n11_69, 0, sizeof(result_n11_69));
  memset(&result_grid_n11_71, 0, sizeof(result_grid_n11_71));
  memset(&tmp_row_add11_73, 0, sizeof(tmp_row_add11_73));
  memset(&orig_result11_74, 0, sizeof(orig_result11_74));
  memset(&comb_row_result12_75, 0, sizeof(comb_row_result12_75));
  memset(&comb_rc_result12_77, 0, sizeof(comb_rc_result12_77));
  memset(&ss_result12_79, 0, sizeof(ss_result12_79));
  memset(&tmp_result_n13_81, 0, sizeof(tmp_result_n13_81));
  memset(&input_dup_n13_82, 0, sizeof(input_dup_n13_82));
  memset(&input_dup_n13_85, 0, sizeof(input_dup_n13_85));
  memset(&input_block_n13_86, 0, sizeof(input_block_n13_86));
  memset(&result_n14_88, 0, sizeof(result_n14_88));
  memset(&result_grid_n14_90, 0, sizeof(result_grid_n14_90));
  memset(&input_dup_n14_92, 0, sizeof(input_dup_n14_92));
  memset(&input_block_n14_94, 0, sizeof(input_block_n14_94));
  memset(&result_n15_96, 0, sizeof(result_n15_96));
  memset(&result_grid_n15_98, 0, sizeof(result_grid_n15_98));
  memset(&tmp_result_n16_100, 0, sizeof(tmp_result_n16_100));
  memset(&input_dup_n16_101, 0, sizeof(input_dup_n16_101));
  memset(&input_dup_n16_104, 0, sizeof(input_dup_n16_104));
  memset(&input_block_n16_105, 0, sizeof(input_block_n16_105));
  memset(&result_n17_107, 0, sizeof(result_n17_107));
  memset(&result_grid_n17_109, 0, sizeof(result_grid_n17_109));
  memset(&input_dup_n17_111, 0, sizeof(input_dup_n17_111));
  memset(&input_block_n17_113, 0, sizeof(input_block_n17_113));
  memset(&result_n18_115, 0, sizeof(result_n18_115));
  memset(&result_grid_n18_117, 0, sizeof(result_grid_n18_117));
  memset(&tmp_result_n19_119, 0, sizeof(tmp_result_n19_119));
  memset(&input_dup_n19_120, 0, sizeof(input_dup_n19_120));
  memset(&input_dup_n19_123, 0, sizeof(input_dup_n19_123));
  memset(&input_block_n19_124, 0, sizeof(input_block_n19_124));
  memset(&result_n20_126, 0, sizeof(result_n20_126));
  memset(&result_grid_n20_128, 0, sizeof(result_grid_n20_128));
  memset(&input_dup_n20_130, 0, sizeof(input_dup_n20_130));
  memset(&input_block_n20_132, 0, sizeof(input_block_n20_132));
  memset(&result_n21_134, 0, sizeof(result_n21_134));
  memset(&result_grid_n21_136, 0, sizeof(result_grid_n21_136));
  memset(&tmp_result_n22_138, 0, sizeof(tmp_result_n22_138));
  memset(&input_dup_n22_139, 0, sizeof(input_dup_n22_139));
  memset(&input_dup_n22_142, 0, sizeof(input_dup_n22_142));
  memset(&input_block_n22_143, 0, sizeof(input_block_n22_143));
  memset(&result_n23_145, 0, sizeof(result_n23_145));
  memset(&result_grid_n23_147, 0, sizeof(result_grid_n23_147));
  memset(&tmp_row_add23_149, 0, sizeof(tmp_row_add23_149));
  memset(&orig_result23_150, 0, sizeof(orig_result23_150));
  memset(&comb_row_result24_151, 0, sizeof(comb_row_result24_151));
  memset(&comb_rc_result24_153, 0, sizeof(comb_rc_result24_153));
  memset(&ss_result24_155, 0, sizeof(ss_result24_155));
  memset(&input_dup_n24_157, 0, sizeof(input_dup_n24_157));
  memset(&input_block_n24_159, 0, sizeof(input_block_n24_159));
  memset(&result_n25_161, 0, sizeof(result_n25_161));
  memset(&result_grid_n25_163, 0, sizeof(result_grid_n25_163));
  memset(&tmp_row_add25_165, 0, sizeof(tmp_row_add25_165));
  memset(&orig_result25_166, 0, sizeof(orig_result25_166));
  memset(&comb_row_result26_167, 0, sizeof(comb_row_result26_167));
  memset(&comb_rc_result26_169, 0, sizeof(comb_rc_result26_169));
  memset(&ss_result26_171, 0, sizeof(ss_result26_171));
  memset(&tmp_result_n27_173, 0, sizeof(tmp_result_n27_173));
  memset(&input_dup_n27_174, 0, sizeof(input_dup_n27_174));
  memset(&input_dup_n27_177, 0, sizeof(input_dup_n27_177));
  memset(&input_block_n27_178, 0, sizeof(input_block_n27_178));
  memset(&result_n28_180, 0, sizeof(result_n28_180));
  memset(&result_grid_n28_182, 0, sizeof(result_grid_n28_182));
  memset(&input_dup_n28_184, 0, sizeof(input_dup_n28_184));
  memset(&input_block_n28_186, 0, sizeof(input_block_n28_186));
  memset(&result_n29_188, 0, sizeof(result_n29_188));
  memset(&result_grid_n29_190, 0, sizeof(result_grid_n29_190));
  memset(&input_dup_n29_192, 0, sizeof(input_dup_n29_192));
  memset(&input_block_n29_194, 0, sizeof(input_block_n29_194));
  memset(&result_n30_195, 0, sizeof(result_n30_195));
  memset(&result_grid_n30_197, 0, sizeof(result_grid_n30_197));
  memset(&gap_result30_200, 0, sizeof(gap_result30_200));
  memset(&orig_result30_201, 0, sizeof(orig_result30_201));
  memset(&ss_result31_202, 0, sizeof(ss_result31_202));
  memset(&input_dup_n31_204, 0, sizeof(input_dup_n31_204));
  memset(&input_block_n31_205, 0, sizeof(input_block_n31_205));
  memset(&gemm_result_n32_207, 0, sizeof(gemm_result_n32_207));
  memset(&block_result_209, 0, sizeof(block_result_209));
  memset(&reduce_intra_211, 0, sizeof(reduce_intra_211));
  memset(&reduce_intra_213, 0, sizeof(reduce_intra_213));
  memset(&output_215, 0, sizeof(output_215));
  memset(&_ckks_gen_tmp_118_216, 0, sizeof(_ckks_gen_tmp_118_216));
  memset(&_ckks_gen_tmp_351_217, 0, sizeof(_ckks_gen_tmp_351_217));
  memset(&_ckks_gen_tmp_387_218, 0, sizeof(_ckks_gen_tmp_387_218));
  memset(&_ckks_gen_tmp_504_219, 0, sizeof(_ckks_gen_tmp_504_219));
  memset(&_ckks_gen_tmp_540_220, 0, sizeof(_ckks_gen_tmp_540_220));
  memset(&_ckks_gen_tmp_758_221, 0, sizeof(_ckks_gen_tmp_758_221));
  memset(&_ckks_gen_tmp_794_222, 0, sizeof(_ckks_gen_tmp_794_222));
  memset(&_ckks_gen_tmp_911_223, 0, sizeof(_ckks_gen_tmp_911_223));
  memset(&_ckks_gen_tmp_947_224, 0, sizeof(_ckks_gen_tmp_947_224));
  memset(&_ckks_gen_tmp_1179_225, 0, sizeof(_ckks_gen_tmp_1179_225));
  memset(&_ckks_gen_tmp_1256_226, 0, sizeof(_ckks_gen_tmp_1256_226));
  memset(&_ckks_gen_tmp_1288_227, 0, sizeof(_ckks_gen_tmp_1288_227));
  memset(&_ckks_gen_tmp_1322_228, 0, sizeof(_ckks_gen_tmp_1322_228));
  memset(&_ckks_gen_tmp_1356_229, 0, sizeof(_ckks_gen_tmp_1356_229));
  memset(&_ckks_gen_tmp_1466_230, 0, sizeof(_ckks_gen_tmp_1466_230));
  memset(&_ckks_gen_tmp_1543_231, 0, sizeof(_ckks_gen_tmp_1543_231));
  memset(&_ckks_gen_tmp_1575_232, 0, sizeof(_ckks_gen_tmp_1575_232));
  memset(&_ckks_gen_tmp_1609_233, 0, sizeof(_ckks_gen_tmp_1609_233));
  memset(&_ckks_gen_tmp_1643_234, 0, sizeof(_ckks_gen_tmp_1643_234));
  memset(&_ckks_gen_tmp_1880_235, 0, sizeof(_ckks_gen_tmp_1880_235));
  memset(&_ckks_gen_tmp_1916_236, 0, sizeof(_ckks_gen_tmp_1916_236));
  memset(&_ckks_gen_tmp_2041_237, 0, sizeof(_ckks_gen_tmp_2041_237));
  memset(&_ckks_gen_tmp_2077_238, 0, sizeof(_ckks_gen_tmp_2077_238));
  memset(&_ckks_gen_tmp_2317_239, 0, sizeof(_ckks_gen_tmp_2317_239));
  memset(&_ckks_gen_tmp_2353_240, 0, sizeof(_ckks_gen_tmp_2353_240));
  memset(&_ckks_gen_tmp_2478_241, 0, sizeof(_ckks_gen_tmp_2478_241));
  memset(&_ckks_gen_tmp_2514_242, 0, sizeof(_ckks_gen_tmp_2514_242));
  memset(&_ckks_gen_tmp_2754_243, 0, sizeof(_ckks_gen_tmp_2754_243));
  memset(&_ckks_gen_tmp_2790_244, 0, sizeof(_ckks_gen_tmp_2790_244));
  memset(&_ckks_gen_tmp_2915_245, 0, sizeof(_ckks_gen_tmp_2915_245));
  memset(&_ckks_gen_tmp_2951_246, 0, sizeof(_ckks_gen_tmp_2951_246));
  memset(&_ckks_gen_tmp_3191_247, 0, sizeof(_ckks_gen_tmp_3191_247));
  memset(&_ckks_gen_tmp_3268_248, 0, sizeof(_ckks_gen_tmp_3268_248));
  memset(&_ckks_gen_tmp_3300_249, 0, sizeof(_ckks_gen_tmp_3300_249));
  memset(&_ckks_gen_tmp_3334_250, 0, sizeof(_ckks_gen_tmp_3334_250));
  memset(&_ckks_gen_tmp_3368_251, 0, sizeof(_ckks_gen_tmp_3368_251));
  memset(&_ckks_gen_tmp_3486_252, 0, sizeof(_ckks_gen_tmp_3486_252));
  memset(&_ckks_gen_tmp_3563_253, 0, sizeof(_ckks_gen_tmp_3563_253));
  memset(&_ckks_gen_tmp_3595_254, 0, sizeof(_ckks_gen_tmp_3595_254));
  memset(&_ckks_gen_tmp_3629_255, 0, sizeof(_ckks_gen_tmp_3629_255));
  memset(&_ckks_gen_tmp_3663_256, 0, sizeof(_ckks_gen_tmp_3663_256));
  memset(&_ckks_gen_tmp_3900_257, 0, sizeof(_ckks_gen_tmp_3900_257));
  memset(&_ckks_gen_tmp_3936_258, 0, sizeof(_ckks_gen_tmp_3936_258));
  memset(&_ckks_gen_tmp_4061_259, 0, sizeof(_ckks_gen_tmp_4061_259));
  memset(&_ckks_gen_tmp_4097_260, 0, sizeof(_ckks_gen_tmp_4097_260));
  memset(&_ckks_gen_tmp_4224_261, 0, sizeof(_ckks_gen_tmp_4224_261));
  memset(&_ckks_gen_tmp_4260_262, 0, sizeof(_ckks_gen_tmp_4260_262));
  memset(&_ckks_gen_tmp_4373_263, 0, sizeof(_ckks_gen_tmp_4373_263));
  memset(&_ckks_gen_tmp_4477_264, 0, sizeof(_ckks_gen_tmp_4477_264));
  memset(&_pgen_tmp_0_268, 0, sizeof(_pgen_tmp_0_268));
  memset(&_pgen_tmp_1_269, 0, sizeof(_pgen_tmp_1_269));
  memset(&_pgen_tmp_2_270, 0, sizeof(_pgen_tmp_2_270));
  memset(&_pgen_tmp_3_271, 0, sizeof(_pgen_tmp_3_271));
  memset(&_pgen_tmp_4_272, 0, sizeof(_pgen_tmp_4_272));
  memset(&_pgen_tmp_5_273, 0, sizeof(_pgen_tmp_5_273));
  memset(&_pgen_tmp_6_274, 0, sizeof(_pgen_tmp_6_274));
  memset(&_pgen_tmp_7_275, 0, sizeof(_pgen_tmp_7_275));
  memset(&_pgen_tmp_8_276, 0, sizeof(_pgen_tmp_8_276));
  memset(&_pgen_tmp_9_277, 0, sizeof(_pgen_tmp_9_277));
  memset(&_pgen_tmp_10_278, 0, sizeof(_pgen_tmp_10_278));
  memset(&_pgen_tmp_11_279, 0, sizeof(_pgen_tmp_11_279));
  memset(&_pgen_tmp_12_280, 0, sizeof(_pgen_tmp_12_280));
  memset(&_pgen_tmp_13_281, 0, sizeof(_pgen_tmp_13_281));
  memset(&_pgen_tmp_14_282, 0, sizeof(_pgen_tmp_14_282));
  memset(&_pgen_tmp_15_283, 0, sizeof(_pgen_tmp_15_283));
  memset(&_pgen_tmp_16_284, 0, sizeof(_pgen_tmp_16_284));
  memset(&_pgen_tmp_17_285, 0, sizeof(_pgen_tmp_17_285));
  memset(&_pgen_tmp_18_286, 0, sizeof(_pgen_tmp_18_286));
  memset(&_pgen_tmp_19_287, 0, sizeof(_pgen_tmp_19_287));
  memset(&_pgen_tmp_20_288, 0, sizeof(_pgen_tmp_20_288));
  memset(&_pgen_tmp_21_289, 0, sizeof(_pgen_tmp_21_289));
  memset(&_pgen_tmp_22_290, 0, sizeof(_pgen_tmp_22_290));
  memset(&_pgen_tmp_23_291, 0, sizeof(_pgen_tmp_23_291));
  memset(&_pgen_tmp_24_292, 0, sizeof(_pgen_tmp_24_292));
  memset(&_pgen_tmp_25_293, 0, sizeof(_pgen_tmp_25_293));
  memset(&_pgen_tmp_26_294, 0, sizeof(_pgen_tmp_26_294));
  memset(&_pgen_tmp_27_295, 0, sizeof(_pgen_tmp_27_295));
  memset(&_pgen_tmp_28_296, 0, sizeof(_pgen_tmp_28_296));
  memset(&_pgen_tmp_29_297, 0, sizeof(_pgen_tmp_29_297));
  memset(&_pgen_tmp_30_298, 0, sizeof(_pgen_tmp_30_298));
  memset(&_pgen_tmp_31_299, 0, sizeof(_pgen_tmp_31_299));
  memset(&_pgen_tmp_32_300, 0, sizeof(_pgen_tmp_32_300));
  memset(&_pgen_tmp_33_301, 0, sizeof(_pgen_tmp_33_301));
  memset(&_pgen_tmp_34_302, 0, sizeof(_pgen_tmp_34_302));
  memset(&_pgen_tmp_35_303, 0, sizeof(_pgen_tmp_35_303));
  memset(&_pgen_tmp_36_304, 0, sizeof(_pgen_tmp_36_304));
  memset(&_pgen_tmp_37_305, 0, sizeof(_pgen_tmp_37_305));
  memset(&_pgen_tmp_38_306, 0, sizeof(_pgen_tmp_38_306));
  memset(&_pgen_tmp_39_307, 0, sizeof(_pgen_tmp_39_307));
  memset(&_pgen_tmp_40_308, 0, sizeof(_pgen_tmp_40_308));
  memset(&_pgen_tmp_41_309, 0, sizeof(_pgen_tmp_41_309));
  memset(&_pgen_tmp_42_310, 0, sizeof(_pgen_tmp_42_310));
  memset(&_pgen_tmp_43_311, 0, sizeof(_pgen_tmp_43_311));
  memset(&_pgen_tmp_44_312, 0, sizeof(_pgen_tmp_44_312));
  memset(&_pgen_tmp_45_313, 0, sizeof(_pgen_tmp_45_313));
  memset(&_pgen_tmp_46_314, 0, sizeof(_pgen_tmp_46_314));
  memset(&_pgen_tmp_47_315, 0, sizeof(_pgen_tmp_47_315));
  memset(&_pgen_tmp_48_316, 0, sizeof(_pgen_tmp_48_316));
  memset(&_pgen_tmp_49_317, 0, sizeof(_pgen_tmp_49_317));
  memset(&_pgen_tmp_50_318, 0, sizeof(_pgen_tmp_50_318));
  memset(&_pgen_tmp_51_319, 0, sizeof(_pgen_tmp_51_319));
  memset(&_pgen_tmp_52_320, 0, sizeof(_pgen_tmp_52_320));
  memset(&_pgen_tmp_53_321, 0, sizeof(_pgen_tmp_53_321));
  memset(&_pgen_tmp_54_322, 0, sizeof(_pgen_tmp_54_322));
  memset(&_pgen_tmp_55_323, 0, sizeof(_pgen_tmp_55_323));
  memset(&_pgen_tmp_56_324, 0, sizeof(_pgen_tmp_56_324));
  memset(&_pgen_tmp_57_325, 0, sizeof(_pgen_tmp_57_325));
  memset(&_pgen_tmp_58_326, 0, sizeof(_pgen_tmp_58_326));
  memset(&_pgen_tmp_59_327, 0, sizeof(_pgen_tmp_59_327));
  memset(&_pgen_tmp_60_328, 0, sizeof(_pgen_tmp_60_328));
  memset(&_pgen_tmp_61_329, 0, sizeof(_pgen_tmp_61_329));
  memset(&_pgen_tmp_62_330, 0, sizeof(_pgen_tmp_62_330));
  memset(&_pgen_tmp_63_331, 0, sizeof(_pgen_tmp_63_331));
  memset(&_pgen_tmp_64_332, 0, sizeof(_pgen_tmp_64_332));
  memset(&_pgen_tmp_65_333, 0, sizeof(_pgen_tmp_65_333));
  memset(&_pgen_tmp_66_334, 0, sizeof(_pgen_tmp_66_334));
  memset(&_pgen_tmp_67_335, 0, sizeof(_pgen_tmp_67_335));
  memset(&_pgen_tmp_68_336, 0, sizeof(_pgen_tmp_68_336));
  memset(&_pgen_tmp_69_337, 0, sizeof(_pgen_tmp_69_337));
  memset(&_pgen_tmp_70_338, 0, sizeof(_pgen_tmp_70_338));
  memset(&_pgen_tmp_71_339, 0, sizeof(_pgen_tmp_71_339));
  memset(&_pgen_tmp_72_340, 0, sizeof(_pgen_tmp_72_340));
  memset(&_pgen_tmp_73_341, 0, sizeof(_pgen_tmp_73_341));
  memset(&_pgen_tmp_74_342, 0, sizeof(_pgen_tmp_74_342));
  memset(&_pgen_tmp_75_343, 0, sizeof(_pgen_tmp_75_343));
  memset(&_pgen_tmp_76_344, 0, sizeof(_pgen_tmp_76_344));
  memset(&_pgen_tmp_77_345, 0, sizeof(_pgen_tmp_77_345));
  memset(&_pgen_tmp_78_346, 0, sizeof(_pgen_tmp_78_346));
  memset(&_pgen_tmp_79_347, 0, sizeof(_pgen_tmp_79_347));
  memset(&_pgen_tmp_80_348, 0, sizeof(_pgen_tmp_80_348));
  memset(&_pgen_tmp_81_349, 0, sizeof(_pgen_tmp_81_349));
  memset(&_pgen_tmp_82_350, 0, sizeof(_pgen_tmp_82_350));
  memset(&_pgen_tmp_83_351, 0, sizeof(_pgen_tmp_83_351));
  memset(&_pgen_tmp_84_352, 0, sizeof(_pgen_tmp_84_352));
  memset(&_pgen_tmp_85_353, 0, sizeof(_pgen_tmp_85_353));
  memset(&_pgen_tmp_86_354, 0, sizeof(_pgen_tmp_86_354));
  memset(&_pgen_tmp_87_355, 0, sizeof(_pgen_tmp_87_355));
  memset(&_pgen_tmp_88_356, 0, sizeof(_pgen_tmp_88_356));
  memset(&_pgen_tmp_89_357, 0, sizeof(_pgen_tmp_89_357));
  memset(&_pgen_tmp_90_358, 0, sizeof(_pgen_tmp_90_358));
  memset(&_pgen_tmp_91_359, 0, sizeof(_pgen_tmp_91_359));
  memset(&_pgen_tmp_92_360, 0, sizeof(_pgen_tmp_92_360));
  memset(&_pgen_tmp_93_361, 0, sizeof(_pgen_tmp_93_361));
  memset(&_pgen_tmp_94_362, 0, sizeof(_pgen_tmp_94_362));
  memset(&_pgen_tmp_95_363, 0, sizeof(_pgen_tmp_95_363));
  memset(&_pgen_tmp_96_364, 0, sizeof(_pgen_tmp_96_364));
  memset(&_pgen_tmp_97_365, 0, sizeof(_pgen_tmp_97_365));
  memset(&_pgen_tmp_98_366, 0, sizeof(_pgen_tmp_98_366));
  memset(&_pgen_tmp_99_367, 0, sizeof(_pgen_tmp_99_367));
  memset(&_pgen_tmp_100_368, 0, sizeof(_pgen_tmp_100_368));
  memset(&_pgen_tmp_101_369, 0, sizeof(_pgen_tmp_101_369));
  memset(&_pgen_tmp_102_370, 0, sizeof(_pgen_tmp_102_370));
  memset(&_pgen_tmp_103_371, 0, sizeof(_pgen_tmp_103_371));
  memset(&_pgen_tmp_104_372, 0, sizeof(_pgen_tmp_104_372));
  memset(&_pgen_tmp_105_373, 0, sizeof(_pgen_tmp_105_373));
  memset(&_pgen_tmp_106_374, 0, sizeof(_pgen_tmp_106_374));
  memset(&_pgen_tmp_107_375, 0, sizeof(_pgen_tmp_107_375));
  memset(&_pgen_tmp_108_376, 0, sizeof(_pgen_tmp_108_376));
  memset(&_pgen_tmp_109_377, 0, sizeof(_pgen_tmp_109_377));
  memset(&_pgen_tmp_110_378, 0, sizeof(_pgen_tmp_110_378));
  memset(&_pgen_tmp_111_379, 0, sizeof(_pgen_tmp_111_379));
  memset(&_pgen_tmp_112_380, 0, sizeof(_pgen_tmp_112_380));
  memset(&_pgen_tmp_113_381, 0, sizeof(_pgen_tmp_113_381));
  memset(&_pgen_tmp_114_382, 0, sizeof(_pgen_tmp_114_382));
  memset(&_pgen_tmp_115_383, 0, sizeof(_pgen_tmp_115_383));
  memset(&_pgen_tmp_116_384, 0, sizeof(_pgen_tmp_116_384));
  memset(&_pgen_tmp_117_385, 0, sizeof(_pgen_tmp_117_385));
  memset(&_pgen_tmp_118_386, 0, sizeof(_pgen_tmp_118_386));
  memset(&_pgen_tmp_119_387, 0, sizeof(_pgen_tmp_119_387));
  memset(&_pgen_tmp_120_388, 0, sizeof(_pgen_tmp_120_388));
  memset(&_pgen_tmp_121_389, 0, sizeof(_pgen_tmp_121_389));
  memset(&_pgen_tmp_122_390, 0, sizeof(_pgen_tmp_122_390));
  memset(&_pgen_tmp_123_391, 0, sizeof(_pgen_tmp_123_391));
  memset(&_pgen_tmp_124_392, 0, sizeof(_pgen_tmp_124_392));
  memset(&_pgen_tmp_125_393, 0, sizeof(_pgen_tmp_125_393));
  memset(&_pgen_tmp_126_394, 0, sizeof(_pgen_tmp_126_394));
  memset(&_pgen_tmp_127_395, 0, sizeof(_pgen_tmp_127_395));
  memset(&_pgen_tmp_128_396, 0, sizeof(_pgen_tmp_128_396));
  memset(&_pgen_tmp_129_397, 0, sizeof(_pgen_tmp_129_397));
  memset(&_pgen_tmp_130_398, 0, sizeof(_pgen_tmp_130_398));
  memset(&_pgen_tmp_131_399, 0, sizeof(_pgen_tmp_131_399));
  memset(&_pgen_tmp_132_400, 0, sizeof(_pgen_tmp_132_400));
  memset(&_pgen_tmp_133_401, 0, sizeof(_pgen_tmp_133_401));
  memset(&_pgen_tmp_134_402, 0, sizeof(_pgen_tmp_134_402));
  memset(&_pgen_tmp_135_403, 0, sizeof(_pgen_tmp_135_403));
  memset(&_pgen_tmp_136_404, 0, sizeof(_pgen_tmp_136_404));
  memset(&_pgen_tmp_137_405, 0, sizeof(_pgen_tmp_137_405));
  memset(&_pgen_tmp_138_406, 0, sizeof(_pgen_tmp_138_406));
  memset(&_pgen_tmp_139_407, 0, sizeof(_pgen_tmp_139_407));
  memset(&_pgen_tmp_140_408, 0, sizeof(_pgen_tmp_140_408));
  memset(&_pgen_tmp_141_409, 0, sizeof(_pgen_tmp_141_409));
  memset(&_pgen_tmp_142_410, 0, sizeof(_pgen_tmp_142_410));
  memset(&_pgen_tmp_143_411, 0, sizeof(_pgen_tmp_143_411));
  memset(&_pgen_tmp_144_412, 0, sizeof(_pgen_tmp_144_412));
  memset(&_pgen_tmp_145_413, 0, sizeof(_pgen_tmp_145_413));
  memset(&_pgen_tmp_146_414, 0, sizeof(_pgen_tmp_146_414));
  memset(&_pgen_tmp_147_415, 0, sizeof(_pgen_tmp_147_415));
  memset(&_pgen_tmp_148_416, 0, sizeof(_pgen_tmp_148_416));
  memset(&_pgen_tmp_149_417, 0, sizeof(_pgen_tmp_149_417));
  memset(&_pgen_tmp_150_418, 0, sizeof(_pgen_tmp_150_418));
  memset(&_pgen_tmp_151_419, 0, sizeof(_pgen_tmp_151_419));
  memset(&_pgen_tmp_152_420, 0, sizeof(_pgen_tmp_152_420));
  memset(&_pgen_tmp_153_421, 0, sizeof(_pgen_tmp_153_421));
  memset(&_pgen_tmp_154_422, 0, sizeof(_pgen_tmp_154_422));
  memset(&_pgen_tmp_155_423, 0, sizeof(_pgen_tmp_155_423));
  memset(&_pgen_tmp_156_424, 0, sizeof(_pgen_tmp_156_424));
  memset(&_pgen_tmp_157_425, 0, sizeof(_pgen_tmp_157_425));
  memset(&_pgen_tmp_158_426, 0, sizeof(_pgen_tmp_158_426));
  memset(&_pgen_tmp_159_427, 0, sizeof(_pgen_tmp_159_427));
  memset(&_pgen_tmp_160_428, 0, sizeof(_pgen_tmp_160_428));
  memset(&_pgen_tmp_161_429, 0, sizeof(_pgen_tmp_161_429));
  memset(&_pgen_tmp_162_430, 0, sizeof(_pgen_tmp_162_430));
  memset(&_pgen_tmp_163_431, 0, sizeof(_pgen_tmp_163_431));
  memset(&_pgen_tmp_164_432, 0, sizeof(_pgen_tmp_164_432));
  memset(&_pgen_tmp_165_433, 0, sizeof(_pgen_tmp_165_433));
  memset(&_pgen_tmp_166_434, 0, sizeof(_pgen_tmp_166_434));
  memset(&_pgen_tmp_167_435, 0, sizeof(_pgen_tmp_167_435));
  memset(&_pgen_tmp_168_436, 0, sizeof(_pgen_tmp_168_436));
  memset(&_pgen_tmp_169_437, 0, sizeof(_pgen_tmp_169_437));
  memset(&_pgen_tmp_170_438, 0, sizeof(_pgen_tmp_170_438));
  memset(&_pgen_tmp_171_439, 0, sizeof(_pgen_tmp_171_439));
  memset(&_pgen_tmp_172_440, 0, sizeof(_pgen_tmp_172_440));
  memset(&_pgen_tmp_173_441, 0, sizeof(_pgen_tmp_173_441));
  memset(&_pgen_tmp_174_442, 0, sizeof(_pgen_tmp_174_442));
  memset(&_pgen_tmp_175_443, 0, sizeof(_pgen_tmp_175_443));
  memset(&_pgen_tmp_176_444, 0, sizeof(_pgen_tmp_176_444));
  memset(&_pgen_tmp_177_445, 0, sizeof(_pgen_tmp_177_445));
  memset(&_pgen_tmp_178_446, 0, sizeof(_pgen_tmp_178_446));
  memset(&_pgen_tmp_179_447, 0, sizeof(_pgen_tmp_179_447));
  memset(&_pgen_tmp_180_448, 0, sizeof(_pgen_tmp_180_448));
  memset(&_pgen_tmp_181_449, 0, sizeof(_pgen_tmp_181_449));
  memset(&_pgen_tmp_182_450, 0, sizeof(_pgen_tmp_182_450));
  memset(&_pgen_tmp_183_451, 0, sizeof(_pgen_tmp_183_451));
  memset(&_pgen_tmp_184_452, 0, sizeof(_pgen_tmp_184_452));
  memset(&_pgen_tmp_185_453, 0, sizeof(_pgen_tmp_185_453));
  memset(&_pgen_tmp_186_454, 0, sizeof(_pgen_tmp_186_454));
  memset(&_pgen_tmp_187_455, 0, sizeof(_pgen_tmp_187_455));
  memset(&_pgen_tmp_188_456, 0, sizeof(_pgen_tmp_188_456));
  memset(&_pgen_tmp_189_457, 0, sizeof(_pgen_tmp_189_457));
  memset(&_pgen_tmp_190_458, 0, sizeof(_pgen_tmp_190_458));
  memset(&_pgen_tmp_191_459, 0, sizeof(_pgen_tmp_191_459));
  memset(&_pgen_tmp_192_460, 0, sizeof(_pgen_tmp_192_460));
  memset(&_pgen_tmp_193_461, 0, sizeof(_pgen_tmp_193_461));
  memset(&_pgen_tmp_194_462, 0, sizeof(_pgen_tmp_194_462));
  memset(&_pgen_tmp_195_463, 0, sizeof(_pgen_tmp_195_463));
  memset(&_pgen_tmp_196_464, 0, sizeof(_pgen_tmp_196_464));
  memset(&_pgen_tmp_197_465, 0, sizeof(_pgen_tmp_197_465));
  memset(&_pgen_tmp_198_466, 0, sizeof(_pgen_tmp_198_466));
  memset(&_pgen_tmp_199_467, 0, sizeof(_pgen_tmp_199_467));
  memset(&_pgen_tmp_200_468, 0, sizeof(_pgen_tmp_200_468));
  memset(&_pgen_tmp_201_469, 0, sizeof(_pgen_tmp_201_469));
  memset(&_pgen_tmp_202_470, 0, sizeof(_pgen_tmp_202_470));
  memset(&_pgen_tmp_203_471, 0, sizeof(_pgen_tmp_203_471));
  memset(&_pgen_tmp_204_472, 0, sizeof(_pgen_tmp_204_472));
  memset(&_pgen_tmp_205_473, 0, sizeof(_pgen_tmp_205_473));
  memset(&_pgen_tmp_206_474, 0, sizeof(_pgen_tmp_206_474));
  memset(&_pgen_tmp_207_475, 0, sizeof(_pgen_tmp_207_475));
  memset(&_pgen_tmp_208_476, 0, sizeof(_pgen_tmp_208_476));
  memset(&_pgen_tmp_209_477, 0, sizeof(_pgen_tmp_209_477));
  memset(&_pgen_tmp_210_478, 0, sizeof(_pgen_tmp_210_478));
  memset(&_pgen_tmp_211_479, 0, sizeof(_pgen_tmp_211_479));
  memset(&_pgen_tmp_212_480, 0, sizeof(_pgen_tmp_212_480));
  memset(&_pgen_tmp_213_481, 0, sizeof(_pgen_tmp_213_481));
  memset(&_pgen_tmp_214_482, 0, sizeof(_pgen_tmp_214_482));
  memset(&_pgen_tmp_215_483, 0, sizeof(_pgen_tmp_215_483));
  memset(&_pgen_tmp_216_484, 0, sizeof(_pgen_tmp_216_484));
  memset(&_pgen_tmp_217_485, 0, sizeof(_pgen_tmp_217_485));
  memset(&_pgen_tmp_218_486, 0, sizeof(_pgen_tmp_218_486));
  memset(&_pgen_tmp_219_487, 0, sizeof(_pgen_tmp_219_487));
  memset(&_pgen_tmp_220_488, 0, sizeof(_pgen_tmp_220_488));
  memset(&_pgen_tmp_221_489, 0, sizeof(_pgen_tmp_221_489));
  memset(&_pgen_tmp_222_490, 0, sizeof(_pgen_tmp_222_490));
  memset(&_pgen_tmp_223_491, 0, sizeof(_pgen_tmp_223_491));
  memset(&_pgen_tmp_224_492, 0, sizeof(_pgen_tmp_224_492));
  memset(&_pgen_tmp_225_493, 0, sizeof(_pgen_tmp_225_493));
  memset(&_pgen_tmp_226_494, 0, sizeof(_pgen_tmp_226_494));
  memset(&_pgen_tmp_227_495, 0, sizeof(_pgen_tmp_227_495));
  memset(&_pgen_tmp_228_496, 0, sizeof(_pgen_tmp_228_496));
  memset(&_pgen_tmp_229_497, 0, sizeof(_pgen_tmp_229_497));
  memset(&_pgen_tmp_230_498, 0, sizeof(_pgen_tmp_230_498));
  memset(&_pgen_tmp_231_499, 0, sizeof(_pgen_tmp_231_499));
  memset(&_pgen_tmp_232_500, 0, sizeof(_pgen_tmp_232_500));
  memset(&_pgen_tmp_233_501, 0, sizeof(_pgen_tmp_233_501));
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
  memset(&_preg_268435690, 0, sizeof(_preg_268435690));
  memset(&_preg_268435691, 0, sizeof(_preg_268435691));
  memset(&_preg_268435692, 0, sizeof(_preg_268435692));
  memset(&_preg_268435693, 0, sizeof(_preg_268435693));
  memset(&_preg_268435694, 0, sizeof(_preg_268435694));
  memset(&_preg_268435695, 0, sizeof(_preg_268435695));
  memset(&_preg_268435696, 0, sizeof(_preg_268435696));
  memset(&_preg_268435697, 0, sizeof(_preg_268435697));
  memset(&_preg_268435698, 0, sizeof(_preg_268435698));
  memset(&_preg_268435699, 0, sizeof(_preg_268435699));
  memset(&_preg_268435700, 0, sizeof(_preg_268435700));
  memset(&_preg_268435701, 0, sizeof(_preg_268435701));
  memset(&_preg_268435702, 0, sizeof(_preg_268435702));
  memset(&_preg_268435703, 0, sizeof(_preg_268435703));
  memset(&_preg_268435704, 0, sizeof(_preg_268435704));
  memset(&_preg_268435705, 0, sizeof(_preg_268435705));
  memset(&_preg_268435706, 0, sizeof(_preg_268435706));
  memset(&_preg_268435707, 0, sizeof(_preg_268435707));
  memset(&_preg_268435708, 0, sizeof(_preg_268435708));
  memset(&_preg_268435709, 0, sizeof(_preg_268435709));
  memset(&_preg_268435710, 0, sizeof(_preg_268435710));
  memset(&_preg_268435711, 0, sizeof(_preg_268435711));
  memset(&_preg_268435712, 0, sizeof(_preg_268435712));
  memset(&_preg_268435713, 0, sizeof(_preg_268435713));
  memset(&_preg_268435714, 0, sizeof(_preg_268435714));
  memset(&_preg_268435715, 0, sizeof(_preg_268435715));
  memset(&_preg_268435716, 0, sizeof(_preg_268435716));
  memset(&_preg_268435717, 0, sizeof(_preg_268435717));
  memset(&_preg_268435718, 0, sizeof(_preg_268435718));
  memset(&_preg_268435719, 0, sizeof(_preg_268435719));
  memset(&_preg_268435720, 0, sizeof(_preg_268435720));
  memset(&_preg_268435721, 0, sizeof(_preg_268435721));
  memset(&_preg_268435722, 0, sizeof(_preg_268435722));
  memset(&_preg_268435723, 0, sizeof(_preg_268435723));
  memset(&_preg_268435724, 0, sizeof(_preg_268435724));
  memset(&_preg_268435725, 0, sizeof(_preg_268435725));
  memset(&_preg_268435726, 0, sizeof(_preg_268435726));
  memset(&_preg_268435727, 0, sizeof(_preg_268435727));
  memset(&_preg_268435728, 0, sizeof(_preg_268435728));
  memset(&_preg_268435729, 0, sizeof(_preg_268435729));
  memset(&_preg_268435730, 0, sizeof(_preg_268435730));
  memset(&_preg_268435731, 0, sizeof(_preg_268435731));
  memset(&_preg_268435732, 0, sizeof(_preg_268435732));
  memset(&_preg_268435733, 0, sizeof(_preg_268435733));
  memset(&_preg_268435734, 0, sizeof(_preg_268435734));
  memset(&_preg_268435735, 0, sizeof(_preg_268435735));
  memset(&_preg_268435736, 0, sizeof(_preg_268435736));
  memset(&_preg_268435737, 0, sizeof(_preg_268435737));
  memset(&_preg_268435738, 0, sizeof(_preg_268435738));
  memset(&_preg_268435739, 0, sizeof(_preg_268435739));
  memset(&_preg_268435740, 0, sizeof(_preg_268435740));
  memset(&_preg_268435741, 0, sizeof(_preg_268435741));
  memset(&_preg_268435742, 0, sizeof(_preg_268435742));
  memset(&_preg_268435743, 0, sizeof(_preg_268435743));
  memset(&_preg_268435744, 0, sizeof(_preg_268435744));
  memset(&_preg_268435745, 0, sizeof(_preg_268435745));
  memset(&_preg_268435746, 0, sizeof(_preg_268435746));
  memset(&_preg_268435747, 0, sizeof(_preg_268435747));
  memset(&_preg_268435748, 0, sizeof(_preg_268435748));
  memset(&_preg_268435749, 0, sizeof(_preg_268435749));
  memset(&_preg_268435750, 0, sizeof(_preg_268435750));
  memset(&_preg_268435751, 0, sizeof(_preg_268435751));
  memset(&_preg_268435752, 0, sizeof(_preg_268435752));
  memset(&_preg_268435753, 0, sizeof(_preg_268435753));
  memset(&_preg_268435754, 0, sizeof(_preg_268435754));
  memset(&_preg_268435755, 0, sizeof(_preg_268435755));
  memset(&_preg_268435756, 0, sizeof(_preg_268435756));
  memset(&_preg_268435757, 0, sizeof(_preg_268435757));
  memset(&_preg_268435758, 0, sizeof(_preg_268435758));
  memset(&_preg_268435759, 0, sizeof(_preg_268435759));
  memset(&_preg_268435760, 0, sizeof(_preg_268435760));
  memset(&_preg_268435761, 0, sizeof(_preg_268435761));
  memset(&_preg_268435762, 0, sizeof(_preg_268435762));
  memset(&_preg_268435763, 0, sizeof(_preg_268435763));
  memset(&_preg_268435764, 0, sizeof(_preg_268435764));
  memset(&_preg_268435765, 0, sizeof(_preg_268435765));
  memset(&_preg_268435766, 0, sizeof(_preg_268435766));
  memset(&_preg_268435767, 0, sizeof(_preg_268435767));
  memset(&_preg_268435768, 0, sizeof(_preg_268435768));
  memset(&_preg_268435769, 0, sizeof(_preg_268435769));
  memset(&_preg_268435770, 0, sizeof(_preg_268435770));
  memset(&_preg_268435771, 0, sizeof(_preg_268435771));
  memset(&_preg_268435772, 0, sizeof(_preg_268435772));
  memset(&_preg_268435773, 0, sizeof(_preg_268435773));
  memset(&_preg_268435774, 0, sizeof(_preg_268435774));
  memset(&_preg_268435775, 0, sizeof(_preg_268435775));
  memset(&_preg_268435776, 0, sizeof(_preg_268435776));
  memset(&_preg_268435777, 0, sizeof(_preg_268435777));
  memset(&_preg_268435778, 0, sizeof(_preg_268435778));
  memset(&_preg_268435779, 0, sizeof(_preg_268435779));
  memset(&_preg_268435780, 0, sizeof(_preg_268435780));
  memset(&_preg_268435781, 0, sizeof(_preg_268435781));
  memset(&_preg_268435782, 0, sizeof(_preg_268435782));
  memset(&_preg_268435783, 0, sizeof(_preg_268435783));
  memset(&_preg_268435784, 0, sizeof(_preg_268435784));
  memset(&_preg_268435785, 0, sizeof(_preg_268435785));
  memset(&_preg_268435786, 0, sizeof(_preg_268435786));
  memset(&_preg_268435787, 0, sizeof(_preg_268435787));
  memset(&_preg_268435788, 0, sizeof(_preg_268435788));
  memset(&_preg_268435789, 0, sizeof(_preg_268435789));
  memset(&_preg_268435790, 0, sizeof(_preg_268435790));
  memset(&_preg_268435791, 0, sizeof(_preg_268435791));
  memset(&_preg_268435792, 0, sizeof(_preg_268435792));
  memset(&_preg_268435793, 0, sizeof(_preg_268435793));
  memset(&_preg_268435794, 0, sizeof(_preg_268435794));
  memset(&_preg_268435795, 0, sizeof(_preg_268435795));
  memset(&_preg_268435796, 0, sizeof(_preg_268435796));
  memset(&_preg_268435797, 0, sizeof(_preg_268435797));
  // /conv1/conv1.0/Conv
  // pragma: 65536, 1027, 33
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435456, &input_0);
  // BlockingRot replicate=16
  _preg_268435622 = Rotate(_preg_268435456, -4096);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_0_268, &_preg_268435456, &_preg_268435622);
  CMPLR_TM_END(12, 0);
  _preg_268435623 = Rotate(_preg_268435456, -8192);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_1_269, &_pgen_tmp_0_268, &_preg_268435623);
  CMPLR_TM_END(12, 0);
  _preg_268435624 = Rotate(_preg_268435456, -12288);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_2_270, &_pgen_tmp_1_269, &_preg_268435624);
  CMPLR_TM_END(12, 0);
  _preg_268435625 = Rotate(_preg_268435456, -16384);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_3_271, &_pgen_tmp_2_270, &_preg_268435625);
  CMPLR_TM_END(12, 0);
  _preg_268435626 = Rotate(_preg_268435456, -20480);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_4_272, &_pgen_tmp_3_271, &_preg_268435626);
  CMPLR_TM_END(12, 0);
  _preg_268435627 = Rotate(_preg_268435456, -24576);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_5_273, &_pgen_tmp_4_272, &_preg_268435627);
  CMPLR_TM_END(12, 0);
  _preg_268435628 = Rotate(_preg_268435456, -28672);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_6_274, &_pgen_tmp_5_273, &_preg_268435628);
  CMPLR_TM_END(12, 0);
  _preg_268435629 = Rotate(_preg_268435456, -32768);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_7_275, &_pgen_tmp_6_274, &_preg_268435629);
  CMPLR_TM_END(12, 0);
  _preg_268435630 = Rotate(_preg_268435456, -36864);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_8_276, &_pgen_tmp_7_275, &_preg_268435630);
  CMPLR_TM_END(12, 0);
  _preg_268435631 = Rotate(_preg_268435456, -40960);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_9_277, &_pgen_tmp_8_276, &_preg_268435631);
  CMPLR_TM_END(12, 0);
  _preg_268435632 = Rotate(_preg_268435456, -45056);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_10_278, &_pgen_tmp_9_277, &_preg_268435632);
  CMPLR_TM_END(12, 0);
  _preg_268435633 = Rotate(_preg_268435456, -49152);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_11_279, &_pgen_tmp_10_278, &_preg_268435633);
  CMPLR_TM_END(12, 0);
  _preg_268435634 = Rotate(_preg_268435456, -53248);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_12_280, &_pgen_tmp_11_279, &_preg_268435634);
  CMPLR_TM_END(12, 0);
  _preg_268435635 = Rotate(_preg_268435456, -57344);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_13_281, &_pgen_tmp_12_280, &_preg_268435635);
  CMPLR_TM_END(12, 0);
  _preg_268435636 = Rotate(_preg_268435456, -61440);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&input_dup_n0_1, &_pgen_tmp_13_281, &_preg_268435636);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&input_dup_n0_1._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_0_268._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435456._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435622._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_0_268._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435456._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435622._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_1_269._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_0_268._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435623._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_1_269._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_0_268._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435623._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_2_270._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_1_269._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435624._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_2_270._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_1_269._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435624._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_3_271._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_2_270._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435625._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_3_271._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_2_270._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435625._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_4_272._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_3_271._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435626._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_4_272._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_3_271._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435626._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_5_273._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_4_272._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435627._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_5_273._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_4_272._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435627._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_6_274._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_5_273._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435628._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_6_274._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_5_273._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435628._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_7_275._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_6_274._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435629._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_7_275._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_6_274._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435629._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_8_276._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_7_275._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435630._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_8_276._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_7_275._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435630._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_9_277._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_8_276._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435631._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_9_277._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_8_276._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435631._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_10_278._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_9_277._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435632._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_10_278._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_9_277._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435632._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_11_279._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_10_278._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435633._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_11_279._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_10_278._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435633._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_12_280._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_11_279._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435634._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_12_280._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_11_279._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435634._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_13_281._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_12_280._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435635._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_13_281._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_12_280._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435635._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n0_1._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_13_281._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435636._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n0_1._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_13_281._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435636._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  // BlockingRot HRot=bs=1
  for (bs_input_n0_2 = 0; bs_input_n0_2 < 1; bs_input_n0_2 = bs_input_n0_2 + 1) {
    _preg_268435637 = Rotate(input_dup_n0_1, 0);
    Copy_ciph(&input_block_n0_3[bs_input_n0_2], &_preg_268435637);
  }
  Zero_ciph(&result_n1_4);
  for (num_grid_n1_5 = 0; num_grid_n1_5 < 4; num_grid_n1_5 = num_grid_n1_5 + 1) {
    Zero_ciph(&result_grid_n1_6);
    for (cap_block_n1_7 = 0; cap_block_n1_7 < 1; cap_block_n1_7 = cap_block_n1_7 + 1) {
      _ckks_gen_tmp_118_216 = input_block_n0_3[cap_block_n1_7];
      _pgen_tmp_14_282 = *(PLAIN)Pt_from_msg(&_pgen_tmp_14_282, (cap_block_n1_7 + (num_grid_n1_5 * 1)) + 0 /* cst_71_0 */, 65536, 1, 30, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_15_283, &_ckks_gen_tmp_118_216, &_pgen_tmp_14_282);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n1_6, &result_grid_n1_6, &_pgen_tmp_15_283);
      _pgen_modulus_265 = Q_modulus();
      _pgen_num_q_266 = Poly_level(&result_grid_n1_6._c0_poly);
      for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_15_283._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_118_216._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_14_282._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_15_283._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_118_216._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_14_282._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n1_6._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n1_6._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_15_283._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n1_6._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n1_6._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_15_283._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        _pgen_modulus_265 = _pgen_modulus_265 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435638 = Rotate(result_grid_n1_6, num_grid_n1_5 * 1024);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n1_4, &result_n1_4, &_preg_268435638);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&result_n1_4._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modadd(Coeffs(&result_n1_4._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n1_4._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435638._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&result_n1_4._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n1_4._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435638._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n1_4, &result_n1_4);
  Rescale(&result_n1_4._c0_poly, &result_n1_4._c0_poly);
  Rescale(&result_n1_4._c1_poly, &result_n1_4._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_17_285 = *(PLAIN)Pt_from_msg(&_pgen_tmp_17_285, 4 /* cst_74 */, 65536, 1, 29, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_16_284, &result_n1_4, &_pgen_tmp_17_285);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_16_284._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_16_284._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n1_4._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_17_285._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_16_284._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n1_4._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_17_285._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435457, &_pgen_tmp_16_284);
  Rescale(&_preg_268435457._c0_poly, &_pgen_tmp_16_284._c0_poly);
  Rescale(&_preg_268435457._c1_poly, &_pgen_tmp_16_284._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_18_286 = *(PLAIN)Pt_from_msg(&_pgen_tmp_18_286, 5 /* cst_72 */, 65536, 1, 28, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n1_4, &_preg_268435457, &_pgen_tmp_18_286);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&result_n1_4._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&result_n1_4._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435457._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_18_286._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Set_coeffs(&result_n1_4._c1_poly, _pgen_rns_idx_267, degree, Coeffs(&_preg_268435457._c1_poly, _pgen_rns_idx_267, degree));
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435458, &result_n1_4);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 33
  // /conv1/conv1.2/Relu
  // pragma: 65536, 1033, 39
  CMPLR_TM_START(10, 0);
  Copy_ciph(&_preg_268435459, &_preg_268435458);
  Encode_float(&_pgen_tmp_20_288, &_cst_261, 1, 1, Level(&_preg_268435459), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_19_287, &_preg_268435459, &_pgen_tmp_20_288);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_19_287._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_19_287._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435459._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_20_288._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_19_287._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435459._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_20_288._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435461, &_pgen_tmp_19_287);
  Rescale(&_preg_268435461._c0_poly, &_pgen_tmp_19_287._c0_poly);
  Rescale(&_preg_268435461._c1_poly, &_pgen_tmp_19_287._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435460 = App_relu(_preg_268435459, _preg_268435461, 65536);
  Copy_ciph(&_preg_268435462, &_preg_268435460);
  CMPLR_TM_END(10, 0);
  // pragma: 65537, 1033, 39
  // /fire2/squeeze/squeeze.0/Conv
  // pragma: 65536, 1027, 40
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435463, &_preg_268435462);
  Zero_ciph(&tmp_result_n2_8);
  Copy_ciph(&input_dup_n2_9, &_preg_268435463);
  for (index_cin_n2_10 = 0; index_cin_n2_10 < 64; index_cin_n2_10 = index_cin_n2_10 + 1) {
    for (index_khw_n2_11 = 0; index_khw_n2_11 < 1; index_khw_n2_11 = index_khw_n2_11 + 1) {
      _pgen_tmp_21_289 = *(PLAIN)Pt_from_msg(&_pgen_tmp_21_289, (index_khw_n2_11 + (index_cin_n2_10 * 1)) + 6 /* cst_76_0 */, 16384, 1, 16, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_22_290, &input_dup_n2_9, &_pgen_tmp_21_289);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&tmp_result_n2_8, &tmp_result_n2_8, &_pgen_tmp_22_290);
      _pgen_modulus_265 = Q_modulus();
      _pgen_num_q_266 = Poly_level(&tmp_result_n2_8._c0_poly);
      for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_22_290._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&input_dup_n2_9._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_21_289._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_22_290._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&input_dup_n2_9._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_21_289._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&tmp_result_n2_8._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_result_n2_8._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_22_290._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&tmp_result_n2_8._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_result_n2_8._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_22_290._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        _pgen_modulus_265 = _pgen_modulus_265 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435639 = Rotate(input_dup_n2_9, 1024);
    Copy_ciph(&input_dup_n2_9, &_preg_268435639);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&tmp_result_n2_8, &tmp_result_n2_8);
  Rescale(&tmp_result_n2_8._c0_poly, &tmp_result_n2_8._c0_poly);
  Rescale(&tmp_result_n2_8._c1_poly, &tmp_result_n2_8._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_23_291 = *(PLAIN)Pt_from_msg(&_pgen_tmp_23_291, 70 /* cst_77 */, 16384, 1, 15, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&tmp_result_n2_8, &tmp_result_n2_8, &_pgen_tmp_23_291);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&tmp_result_n2_8._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&tmp_result_n2_8._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_result_n2_8._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_23_291._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Set_coeffs(&tmp_result_n2_8._c1_poly, _pgen_rns_idx_267, degree, Coeffs(&tmp_result_n2_8._c1_poly, _pgen_rns_idx_267, degree));
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435464, &tmp_result_n2_8);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 40
  // /fire2/squeeze/squeeze.2/Relu
  // pragma: 65536, 1033, 41
  CMPLR_TM_START(10, 0);
  Copy_ciph(&_preg_268435465, &_preg_268435464);
  Encode_float(&_pgen_tmp_25_293, &_cst_262, 1, 1, Level(&_preg_268435465), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_24_292, &_preg_268435465, &_pgen_tmp_25_293);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_24_292._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_24_292._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435465._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_25_293._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_24_292._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435465._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_25_293._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435467, &_pgen_tmp_24_292);
  Rescale(&_preg_268435467._c0_poly, &_pgen_tmp_24_292._c0_poly);
  Rescale(&_preg_268435467._c1_poly, &_pgen_tmp_24_292._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435466 = App_relu(_preg_268435465, _preg_268435467, 16384);
  Copy_ciph(&_preg_268435468, &_preg_268435466);
  CMPLR_TM_END(10, 0);
  // pragma: 65537, 1033, 41
  // /fire2/expand1/expand1.0/Conv
  // pragma: 65536, 1027, 42
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435469, &_preg_268435468);
  // BlockingRot replicate=2
  _preg_268435640 = Rotate(_preg_268435469, -16384);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&input_dup_n2_12, &_preg_268435469, &_preg_268435640);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&input_dup_n2_12._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&input_dup_n2_12._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435469._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435640._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n2_12._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435469._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435640._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  // BlockingRot HRot=bs=1
  for (bs_input_n2_13 = 0; bs_input_n2_13 < 1; bs_input_n2_13 = bs_input_n2_13 + 1) {
    _preg_268435641 = Rotate(input_dup_n2_12, 0);
    Copy_ciph(&input_block_n2_14[bs_input_n2_13], &_preg_268435641);
  }
  Zero_ciph(&result_n3_15);
  for (num_grid_n3_16 = 0; num_grid_n3_16 < 16; num_grid_n3_16 = num_grid_n3_16 + 1) {
    Zero_ciph(&result_grid_n3_17);
    for (cap_block_n3_18 = 0; cap_block_n3_18 < 1; cap_block_n3_18 = cap_block_n3_18 + 1) {
      _ckks_gen_tmp_351_217 = input_block_n2_14[cap_block_n3_18];
      _pgen_tmp_26_294 = *(PLAIN)Pt_from_msg(&_pgen_tmp_26_294, (cap_block_n3_18 + (num_grid_n3_16 * 1)) + 71 /* cst_80_0 */, 32768, 1, 3, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_27_295, &_ckks_gen_tmp_351_217, &_pgen_tmp_26_294);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n3_17, &result_grid_n3_17, &_pgen_tmp_27_295);
      _pgen_modulus_265 = Q_modulus();
      _pgen_num_q_266 = Poly_level(&result_grid_n3_17._c0_poly);
      for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_27_295._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_351_217._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_26_294._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_27_295._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_351_217._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_26_294._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n3_17._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n3_17._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_27_295._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n3_17._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n3_17._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_27_295._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        _pgen_modulus_265 = _pgen_modulus_265 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435642 = Rotate(result_grid_n3_17, num_grid_n3_16 * 1024);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n3_15, &result_n3_15, &_preg_268435642);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&result_n3_15._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modadd(Coeffs(&result_n3_15._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n3_15._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435642._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&result_n3_15._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n3_15._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435642._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n3_15, &result_n3_15);
  Rescale(&result_n3_15._c0_poly, &result_n3_15._c0_poly);
  Rescale(&result_n3_15._c1_poly, &result_n3_15._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435643 = Rotate(result_n3_15, -32768);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_ckks_gen_tmp_387_218, &result_n3_15, &_preg_268435643);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_ckks_gen_tmp_387_218._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_ckks_gen_tmp_387_218._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n3_15._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435643._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_ckks_gen_tmp_387_218._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n3_15._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435643._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  _pgen_tmp_29_297 = *(PLAIN)Pt_from_msg(&_pgen_tmp_29_297, 87 /* cst_83 */, 32768, 1, 2, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_28_296, &_ckks_gen_tmp_387_218, &_pgen_tmp_29_297);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_28_296._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_28_296._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_387_218._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_29_297._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_28_296._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_387_218._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_29_297._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435470, &_pgen_tmp_28_296);
  Rescale(&_preg_268435470._c0_poly, &_pgen_tmp_28_296._c0_poly);
  Rescale(&_preg_268435470._c1_poly, &_pgen_tmp_28_296._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_30_298 = *(PLAIN)Pt_from_msg(&_pgen_tmp_30_298, 88 /* cst_81 */, 32768, 1, 1, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n3_15, &_preg_268435470, &_pgen_tmp_30_298);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&result_n3_15._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&result_n3_15._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435470._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_30_298._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Set_coeffs(&result_n3_15._c1_poly, _pgen_rns_idx_267, degree, Coeffs(&_preg_268435470._c1_poly, _pgen_rns_idx_267, degree));
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435471, &result_n3_15);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 42
  // /fire2/expand2/expand2.0/Conv
  // pragma: 65536, 1027, 43
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435472, &_preg_268435468);
  // BlockingRot replicate=2
  _preg_268435644 = Rotate(_preg_268435472, -16384);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&input_dup_n3_19, &_preg_268435472, &_preg_268435644);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&input_dup_n3_19._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&input_dup_n3_19._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435472._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435644._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n3_19._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435472._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435644._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  // BlockingRot HRot=bs=9
  for (bs_input_n3_20 = 0; bs_input_n3_20 < 4; bs_input_n3_20 = bs_input_n3_20 + 1) {
    _preg_268435645 = Rotate(input_dup_n3_19, _cst_87[bs_input_n3_20]);
    Copy_ciph(&input_block_n3_21[bs_input_n3_20], &_preg_268435645);
  }
  _preg_268435646 = Rotate(input_dup_n3_19, 0);
  Copy_ciph(&input_block_n3_21[4], &_preg_268435646);
  for (bs_input_n3_rs2_22 = 5; bs_input_n3_rs2_22 < 9; bs_input_n3_rs2_22 = bs_input_n3_rs2_22 + 1) {
    _preg_268435647 = Rotate(input_dup_n3_19, _cst_87[bs_input_n3_rs2_22]);
    Copy_ciph(&input_block_n3_21[bs_input_n3_rs2_22], &_preg_268435647);
  }
  Zero_ciph(&result_n4_23);
  for (num_grid_n4_24 = 0; num_grid_n4_24 < 16; num_grid_n4_24 = num_grid_n4_24 + 1) {
    Zero_ciph(&result_grid_n4_25);
    for (cap_block_n4_26 = 0; cap_block_n4_26 < 9; cap_block_n4_26 = cap_block_n4_26 + 1) {
      _ckks_gen_tmp_504_219 = input_block_n3_21[cap_block_n4_26];
      _pgen_tmp_31_299 = *(PLAIN)Pt_from_msg(&_pgen_tmp_31_299, (cap_block_n4_26 + (num_grid_n4_24 * 9)) + 89 /* cst_85_0 */, 32768, 1, 3, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_32_300, &_ckks_gen_tmp_504_219, &_pgen_tmp_31_299);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n4_25, &result_grid_n4_25, &_pgen_tmp_32_300);
      _pgen_modulus_265 = Q_modulus();
      _pgen_num_q_266 = Poly_level(&result_grid_n4_25._c0_poly);
      for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_32_300._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_504_219._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_31_299._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_32_300._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_504_219._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_31_299._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n4_25._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n4_25._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_32_300._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n4_25._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n4_25._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_32_300._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        _pgen_modulus_265 = _pgen_modulus_265 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435648 = Rotate(result_grid_n4_25, num_grid_n4_24 * 1024);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n4_23, &result_n4_23, &_preg_268435648);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&result_n4_23._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modadd(Coeffs(&result_n4_23._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n4_23._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435648._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&result_n4_23._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n4_23._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435648._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n4_23, &result_n4_23);
  Rescale(&result_n4_23._c0_poly, &result_n4_23._c0_poly);
  Rescale(&result_n4_23._c1_poly, &result_n4_23._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435649 = Rotate(result_n4_23, -32768);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_ckks_gen_tmp_540_220, &result_n4_23, &_preg_268435649);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_ckks_gen_tmp_540_220._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_ckks_gen_tmp_540_220._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n4_23._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435649._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_ckks_gen_tmp_540_220._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n4_23._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435649._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  _pgen_tmp_34_302 = *(PLAIN)Pt_from_msg(&_pgen_tmp_34_302, 233 /* cst_88 */, 32768, 1, 2, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_33_301, &_ckks_gen_tmp_540_220, &_pgen_tmp_34_302);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_33_301._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_33_301._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_540_220._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_34_302._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_33_301._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_540_220._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_34_302._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435473, &_pgen_tmp_33_301);
  Rescale(&_preg_268435473._c0_poly, &_pgen_tmp_33_301._c0_poly);
  Rescale(&_preg_268435473._c1_poly, &_pgen_tmp_33_301._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_35_303 = *(PLAIN)Pt_from_msg(&_pgen_tmp_35_303, 234 /* cst_86 */, 32768, 1, 1, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n4_23, &_preg_268435473, &_pgen_tmp_35_303);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&result_n4_23._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&result_n4_23._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435473._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_35_303._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Set_coeffs(&result_n4_23._c1_poly, _pgen_rns_idx_267, degree, Coeffs(&_preg_268435473._c1_poly, _pgen_rns_idx_267, degree));
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435474, &result_n4_23);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 43
  // /fire2/Concat
  // pragma: 65536, 1045, 44
  CMPLR_TM_START(4, 0);
  _preg_268435650 = Rotate(_preg_268435474, -32768);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_preg_268435475, &_preg_268435471, &_preg_268435650);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_preg_268435475._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_preg_268435475._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435471._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435650._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_preg_268435475._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435471._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435650._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  CMPLR_TM_END(4, 0);
  // pragma: 65537, 1045, 44
  // /fire2/Relu
  // pragma: 65536, 1033, 46
  CMPLR_TM_START(10, 0);
  Bootstrap(&_preg_268435476, &_preg_268435475, 14);
  Encode_float(&_pgen_tmp_37_305, &_cst_263, 1, 1, Level(&_preg_268435476), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_36_304, &_preg_268435476, &_pgen_tmp_37_305);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_36_304._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_36_304._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435476._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_37_305._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_36_304._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435476._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_37_305._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435478, &_pgen_tmp_36_304);
  Rescale(&_preg_268435478._c0_poly, &_pgen_tmp_36_304._c0_poly);
  Rescale(&_preg_268435478._c1_poly, &_pgen_tmp_36_304._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435477 = App_relu(_preg_268435476, _preg_268435478, 65536);
  Copy_ciph(&_preg_268435479, &_preg_268435477);
  CMPLR_TM_END(10, 0);
  // pragma: 65537, 1033, 46
  // /fire3/squeeze/squeeze.0/Conv
  // pragma: 65536, 1027, 47
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435480, &_preg_268435479);
  Zero_ciph(&tmp_result_n5_27);
  Copy_ciph(&input_dup_n5_28, &_preg_268435480);
  for (index_cin_n5_29 = 0; index_cin_n5_29 < 64; index_cin_n5_29 = index_cin_n5_29 + 1) {
    for (index_khw_n5_30 = 0; index_khw_n5_30 < 1; index_khw_n5_30 = index_khw_n5_30 + 1) {
      _pgen_tmp_38_306 = *(PLAIN)Pt_from_msg(&_pgen_tmp_38_306, (index_khw_n5_30 + (index_cin_n5_29 * 1)) + 235 /* cst_90_0 */, 16384, 1, 2, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_39_307, &input_dup_n5_28, &_pgen_tmp_38_306);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&tmp_result_n5_27, &tmp_result_n5_27, &_pgen_tmp_39_307);
      _pgen_modulus_265 = Q_modulus();
      _pgen_num_q_266 = Poly_level(&tmp_result_n5_27._c0_poly);
      for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_39_307._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&input_dup_n5_28._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_38_306._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_39_307._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&input_dup_n5_28._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_38_306._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&tmp_result_n5_27._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_result_n5_27._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_39_307._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&tmp_result_n5_27._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_result_n5_27._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_39_307._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        _pgen_modulus_265 = _pgen_modulus_265 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435651 = Rotate(input_dup_n5_28, 1024);
    Copy_ciph(&input_dup_n5_28, &_preg_268435651);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&tmp_result_n5_27, &tmp_result_n5_27);
  Rescale(&tmp_result_n5_27._c0_poly, &tmp_result_n5_27._c0_poly);
  Rescale(&tmp_result_n5_27._c1_poly, &tmp_result_n5_27._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_40_308 = *(PLAIN)Pt_from_msg(&_pgen_tmp_40_308, 299 /* cst_91 */, 16384, 1, 1, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&tmp_result_n5_27, &tmp_result_n5_27, &_pgen_tmp_40_308);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&tmp_result_n5_27._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&tmp_result_n5_27._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_result_n5_27._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_40_308._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Set_coeffs(&tmp_result_n5_27._c1_poly, _pgen_rns_idx_267, degree, Coeffs(&tmp_result_n5_27._c1_poly, _pgen_rns_idx_267, degree));
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435481, &tmp_result_n5_27);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 47
  // /fire3/squeeze/squeeze.2/Relu
  // pragma: 65536, 1033, 48
  CMPLR_TM_START(10, 0);
  Bootstrap(&_preg_268435482, &_preg_268435481, 15);
  Encode_float(&_pgen_tmp_42_310, &_cst_264, 1, 1, Level(&_preg_268435482), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_41_309, &_preg_268435482, &_pgen_tmp_42_310);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_41_309._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_41_309._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435482._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_42_310._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_41_309._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435482._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_42_310._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435484, &_pgen_tmp_41_309);
  Rescale(&_preg_268435484._c0_poly, &_pgen_tmp_41_309._c0_poly);
  Rescale(&_preg_268435484._c1_poly, &_pgen_tmp_41_309._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435483 = App_relu(_preg_268435482, _preg_268435484, 16384);
  Copy_ciph(&_preg_268435485, &_preg_268435483);
  CMPLR_TM_END(10, 0);
  // pragma: 65537, 1033, 48
  // /fire3/expand1/expand1.0/Conv
  // pragma: 65536, 1027, 49
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435486, &_preg_268435485);
  // BlockingRot replicate=2
  _preg_268435652 = Rotate(_preg_268435486, -16384);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&input_dup_n5_31, &_preg_268435486, &_preg_268435652);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&input_dup_n5_31._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&input_dup_n5_31._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435486._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435652._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n5_31._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435486._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435652._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  // BlockingRot HRot=bs=1
  for (bs_input_n5_32 = 0; bs_input_n5_32 < 1; bs_input_n5_32 = bs_input_n5_32 + 1) {
    _preg_268435653 = Rotate(input_dup_n5_31, 0);
    Copy_ciph(&input_block_n5_33[bs_input_n5_32], &_preg_268435653);
  }
  Zero_ciph(&result_n6_34);
  for (num_grid_n6_35 = 0; num_grid_n6_35 < 16; num_grid_n6_35 = num_grid_n6_35 + 1) {
    Zero_ciph(&result_grid_n6_36);
    for (cap_block_n6_37 = 0; cap_block_n6_37 < 1; cap_block_n6_37 = cap_block_n6_37 + 1) {
      _ckks_gen_tmp_758_221 = input_block_n5_33[cap_block_n6_37];
      _pgen_tmp_43_311 = *(PLAIN)Pt_from_msg(&_pgen_tmp_43_311, (cap_block_n6_37 + (num_grid_n6_35 * 1)) + 300 /* cst_94_0 */, 32768, 1, 3, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_44_312, &_ckks_gen_tmp_758_221, &_pgen_tmp_43_311);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n6_36, &result_grid_n6_36, &_pgen_tmp_44_312);
      _pgen_modulus_265 = Q_modulus();
      _pgen_num_q_266 = Poly_level(&result_grid_n6_36._c0_poly);
      for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_44_312._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_758_221._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_43_311._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_44_312._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_758_221._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_43_311._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n6_36._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n6_36._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_44_312._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n6_36._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n6_36._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_44_312._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        _pgen_modulus_265 = _pgen_modulus_265 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435654 = Rotate(result_grid_n6_36, num_grid_n6_35 * 1024);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n6_34, &result_n6_34, &_preg_268435654);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&result_n6_34._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modadd(Coeffs(&result_n6_34._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n6_34._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435654._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&result_n6_34._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n6_34._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435654._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n6_34, &result_n6_34);
  Rescale(&result_n6_34._c0_poly, &result_n6_34._c0_poly);
  Rescale(&result_n6_34._c1_poly, &result_n6_34._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435655 = Rotate(result_n6_34, -32768);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_ckks_gen_tmp_794_222, &result_n6_34, &_preg_268435655);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_ckks_gen_tmp_794_222._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_ckks_gen_tmp_794_222._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n6_34._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435655._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_ckks_gen_tmp_794_222._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n6_34._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435655._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  _pgen_tmp_46_314 = *(PLAIN)Pt_from_msg(&_pgen_tmp_46_314, 316 /* cst_97 */, 32768, 1, 2, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_45_313, &_ckks_gen_tmp_794_222, &_pgen_tmp_46_314);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_45_313._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_45_313._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_794_222._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_46_314._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_45_313._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_794_222._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_46_314._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435487, &_pgen_tmp_45_313);
  Rescale(&_preg_268435487._c0_poly, &_pgen_tmp_45_313._c0_poly);
  Rescale(&_preg_268435487._c1_poly, &_pgen_tmp_45_313._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_47_315 = *(PLAIN)Pt_from_msg(&_pgen_tmp_47_315, 317 /* cst_95 */, 32768, 1, 1, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n6_34, &_preg_268435487, &_pgen_tmp_47_315);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&result_n6_34._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&result_n6_34._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435487._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_47_315._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Set_coeffs(&result_n6_34._c1_poly, _pgen_rns_idx_267, degree, Coeffs(&_preg_268435487._c1_poly, _pgen_rns_idx_267, degree));
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435488, &result_n6_34);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 49
  // /fire3/expand2/expand2.0/Conv
  // pragma: 65536, 1027, 50
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435489, &_preg_268435485);
  // BlockingRot replicate=2
  _preg_268435656 = Rotate(_preg_268435489, -16384);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&input_dup_n6_38, &_preg_268435489, &_preg_268435656);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&input_dup_n6_38._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&input_dup_n6_38._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435489._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435656._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n6_38._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435489._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435656._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  // BlockingRot HRot=bs=9
  for (bs_input_n6_39 = 0; bs_input_n6_39 < 4; bs_input_n6_39 = bs_input_n6_39 + 1) {
    _preg_268435657 = Rotate(input_dup_n6_38, _cst_101[bs_input_n6_39]);
    Copy_ciph(&input_block_n6_40[bs_input_n6_39], &_preg_268435657);
  }
  _preg_268435658 = Rotate(input_dup_n6_38, 0);
  Copy_ciph(&input_block_n6_40[4], &_preg_268435658);
  for (bs_input_n6_rs2_41 = 5; bs_input_n6_rs2_41 < 9; bs_input_n6_rs2_41 = bs_input_n6_rs2_41 + 1) {
    _preg_268435659 = Rotate(input_dup_n6_38, _cst_101[bs_input_n6_rs2_41]);
    Copy_ciph(&input_block_n6_40[bs_input_n6_rs2_41], &_preg_268435659);
  }
  Zero_ciph(&result_n7_42);
  for (num_grid_n7_43 = 0; num_grid_n7_43 < 16; num_grid_n7_43 = num_grid_n7_43 + 1) {
    Zero_ciph(&result_grid_n7_44);
    for (cap_block_n7_45 = 0; cap_block_n7_45 < 9; cap_block_n7_45 = cap_block_n7_45 + 1) {
      _ckks_gen_tmp_911_223 = input_block_n6_40[cap_block_n7_45];
      _pgen_tmp_48_316 = *(PLAIN)Pt_from_msg(&_pgen_tmp_48_316, (cap_block_n7_45 + (num_grid_n7_43 * 9)) + 318 /* cst_99_0 */, 32768, 1, 3, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_49_317, &_ckks_gen_tmp_911_223, &_pgen_tmp_48_316);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n7_44, &result_grid_n7_44, &_pgen_tmp_49_317);
      _pgen_modulus_265 = Q_modulus();
      _pgen_num_q_266 = Poly_level(&result_grid_n7_44._c0_poly);
      for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_49_317._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_911_223._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_48_316._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_49_317._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_911_223._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_48_316._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n7_44._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n7_44._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_49_317._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n7_44._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n7_44._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_49_317._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        _pgen_modulus_265 = _pgen_modulus_265 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435660 = Rotate(result_grid_n7_44, num_grid_n7_43 * 1024);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n7_42, &result_n7_42, &_preg_268435660);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&result_n7_42._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modadd(Coeffs(&result_n7_42._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n7_42._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435660._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&result_n7_42._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n7_42._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435660._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n7_42, &result_n7_42);
  Rescale(&result_n7_42._c0_poly, &result_n7_42._c0_poly);
  Rescale(&result_n7_42._c1_poly, &result_n7_42._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435661 = Rotate(result_n7_42, -32768);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_ckks_gen_tmp_947_224, &result_n7_42, &_preg_268435661);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_ckks_gen_tmp_947_224._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_ckks_gen_tmp_947_224._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n7_42._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435661._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_ckks_gen_tmp_947_224._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n7_42._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435661._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  _pgen_tmp_51_319 = *(PLAIN)Pt_from_msg(&_pgen_tmp_51_319, 462 /* cst_102 */, 32768, 1, 2, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_50_318, &_ckks_gen_tmp_947_224, &_pgen_tmp_51_319);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_50_318._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_50_318._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_947_224._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_51_319._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_50_318._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_947_224._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_51_319._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435490, &_pgen_tmp_50_318);
  Rescale(&_preg_268435490._c0_poly, &_pgen_tmp_50_318._c0_poly);
  Rescale(&_preg_268435490._c1_poly, &_pgen_tmp_50_318._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_52_320 = *(PLAIN)Pt_from_msg(&_pgen_tmp_52_320, 463 /* cst_100 */, 32768, 1, 1, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n7_42, &_preg_268435490, &_pgen_tmp_52_320);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&result_n7_42._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&result_n7_42._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435490._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_52_320._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Set_coeffs(&result_n7_42._c1_poly, _pgen_rns_idx_267, degree, Coeffs(&_preg_268435490._c1_poly, _pgen_rns_idx_267, degree));
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435491, &result_n7_42);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 50
  // /fire3/Concat
  // pragma: 65536, 1045, 51
  CMPLR_TM_START(4, 0);
  _preg_268435662 = Rotate(_preg_268435491, -32768);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_preg_268435492, &_preg_268435488, &_preg_268435662);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_preg_268435492._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_preg_268435492._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435488._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435662._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_preg_268435492._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435488._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435662._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  CMPLR_TM_END(4, 0);
  // pragma: 65537, 1045, 51
  // /fire3/Relu
  // pragma: 65536, 1033, 52
  CMPLR_TM_START(10, 0);
  Bootstrap(&_preg_268435493, &_preg_268435492, 14);
  Encode_float(&_pgen_tmp_54_322, &_cst_265, 1, 1, Level(&_preg_268435493), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_53_321, &_preg_268435493, &_pgen_tmp_54_322);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_53_321._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_53_321._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435493._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_54_322._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_53_321._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435493._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_54_322._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435495, &_pgen_tmp_53_321);
  Rescale(&_preg_268435495._c0_poly, &_pgen_tmp_53_321._c0_poly);
  Rescale(&_preg_268435495._c1_poly, &_pgen_tmp_53_321._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435494 = App_relu(_preg_268435493, _preg_268435495, 65536);
  Copy_ciph(&_preg_268435496, &_preg_268435494);
  CMPLR_TM_END(10, 0);
  // pragma: 65537, 1033, 52
  // /fire4/squeeze/squeeze.0/Conv
  // pragma: 65536, 1027, 53
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435497, &_preg_268435496);
  Zero_ciph(&tmp_result_n8_46);
  Copy_ciph(&input_dup_n8_47, &_preg_268435497);
  for (index_cin_n8_48 = 0; index_cin_n8_48 < 64; index_cin_n8_48 = index_cin_n8_48 + 1) {
    for (index_khw_n8_49 = 0; index_khw_n8_49 < 1; index_khw_n8_49 = index_khw_n8_49 + 1) {
      _pgen_tmp_55_323 = *(PLAIN)Pt_from_msg(&_pgen_tmp_55_323, (index_khw_n8_49 + (index_cin_n8_48 * 1)) + 464 /* cst_104_0 */, 32768, 1, 2, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_56_324, &input_dup_n8_47, &_pgen_tmp_55_323);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&tmp_result_n8_46, &tmp_result_n8_46, &_pgen_tmp_56_324);
      _pgen_modulus_265 = Q_modulus();
      _pgen_num_q_266 = Poly_level(&tmp_result_n8_46._c0_poly);
      for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_56_324._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&input_dup_n8_47._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_55_323._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_56_324._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&input_dup_n8_47._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_55_323._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&tmp_result_n8_46._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_result_n8_46._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_56_324._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&tmp_result_n8_46._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_result_n8_46._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_56_324._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        _pgen_modulus_265 = _pgen_modulus_265 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435663 = Rotate(input_dup_n8_47, 1024);
    Copy_ciph(&input_dup_n8_47, &_preg_268435663);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&tmp_result_n8_46, &tmp_result_n8_46);
  Rescale(&tmp_result_n8_46._c0_poly, &tmp_result_n8_46._c0_poly);
  Rescale(&tmp_result_n8_46._c1_poly, &tmp_result_n8_46._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_57_325 = *(PLAIN)Pt_from_msg(&_pgen_tmp_57_325, 528 /* cst_105 */, 32768, 1, 1, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&tmp_result_n8_46, &tmp_result_n8_46, &_pgen_tmp_57_325);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&tmp_result_n8_46._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&tmp_result_n8_46._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_result_n8_46._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_57_325._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Set_coeffs(&tmp_result_n8_46._c1_poly, _pgen_rns_idx_267, degree, Coeffs(&tmp_result_n8_46._c1_poly, _pgen_rns_idx_267, degree));
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435498, &tmp_result_n8_46);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 53
  // /fire4/squeeze/squeeze.2/Relu
  // pragma: 65536, 1033, 54
  CMPLR_TM_START(10, 0);
  Bootstrap(&_preg_268435499, &_preg_268435498, 19);
  Encode_float(&_pgen_tmp_59_327, &_cst_266, 1, 1, Level(&_preg_268435499), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_58_326, &_preg_268435499, &_pgen_tmp_59_327);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_58_326._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_58_326._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435499._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_59_327._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_58_326._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435499._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_59_327._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435501, &_pgen_tmp_58_326);
  Rescale(&_preg_268435501._c0_poly, &_pgen_tmp_58_326._c0_poly);
  Rescale(&_preg_268435501._c1_poly, &_pgen_tmp_58_326._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435500 = App_relu(_preg_268435499, _preg_268435501, 32768);
  Copy_ciph(&_preg_268435502, &_preg_268435500);
  CMPLR_TM_END(10, 0);
  // pragma: 65537, 1033, 54
  // /fire4/expand1/expand1.0/Conv
  // pragma: 65536, 1027, 55
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435503, &_preg_268435502);
  // BlockingRot replicate=3
  _preg_268435664 = Rotate(_preg_268435503, -32768);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_60_328, &_preg_268435503, &_preg_268435664);
  CMPLR_TM_END(12, 0);
  _preg_268435665 = Rotate(_preg_268435503, -65536);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&input_dup_n8_50, &_pgen_tmp_60_328, &_preg_268435665);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&input_dup_n8_50._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_60_328._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435503._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435664._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_60_328._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435503._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435664._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n8_50._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_60_328._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435665._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n8_50._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_60_328._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435665._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  // BlockingRot HRot=bs=4
  _preg_268435666 = Rotate(input_dup_n8_50, 0);
  Copy_ciph(&input_block_n8_51[0], &_preg_268435666);
  for (bs_input_n8_52 = 1; bs_input_n8_52 < 4; bs_input_n8_52 = bs_input_n8_52 + 1) {
    _preg_268435667 = Rotate(input_dup_n8_50, _cst_110[bs_input_n8_52]);
    Copy_ciph(&input_block_n8_51[bs_input_n8_52], &_preg_268435667);
  }
  Zero_ciph(&result_n9_53);
  for (num_grid_n9_54 = 0; num_grid_n9_54 < 8; num_grid_n9_54 = num_grid_n9_54 + 1) {
    Zero_ciph(&result_grid_n9_55);
    for (cap_block_n9_56 = 0; cap_block_n9_56 < 4; cap_block_n9_56 = cap_block_n9_56 + 1) {
      _ckks_gen_tmp_1179_225 = input_block_n8_51[cap_block_n9_56];
      _pgen_tmp_61_329 = *(PLAIN)Pt_from_msg(&_pgen_tmp_61_329, (cap_block_n9_56 + (num_grid_n9_54 * 4)) + 529 /* cst_108_0 */, 65536, 1, 7, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_62_330, &_ckks_gen_tmp_1179_225, &_pgen_tmp_61_329);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n9_55, &result_grid_n9_55, &_pgen_tmp_62_330);
      _pgen_modulus_265 = Q_modulus();
      _pgen_num_q_266 = Poly_level(&result_grid_n9_55._c0_poly);
      for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_62_330._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_1179_225._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_61_329._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_62_330._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_1179_225._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_61_329._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n9_55._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n9_55._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_62_330._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n9_55._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n9_55._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_62_330._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        _pgen_modulus_265 = _pgen_modulus_265 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435668 = Rotate(result_grid_n9_55, num_grid_n9_54 * 4096);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n9_53, &result_n9_53, &_preg_268435668);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&result_n9_53._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modadd(Coeffs(&result_n9_53._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n9_53._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435668._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&result_n9_53._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n9_53._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435668._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n9_53, &result_n9_53);
  Rescale(&result_n9_53._c0_poly, &result_n9_53._c0_poly);
  Rescale(&result_n9_53._c1_poly, &result_n9_53._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_64_332 = *(PLAIN)Pt_from_msg(&_pgen_tmp_64_332, 561 /* cst_111 */, 65536, 1, 6, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_63_331, &result_n9_53, &_pgen_tmp_64_332);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_63_331._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_63_331._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n9_53._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_64_332._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_63_331._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n9_53._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_64_332._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435504, &_pgen_tmp_63_331);
  Rescale(&_preg_268435504._c0_poly, &_pgen_tmp_63_331._c0_poly);
  Rescale(&_preg_268435504._c1_poly, &_pgen_tmp_63_331._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_65_333 = *(PLAIN)Pt_from_msg(&_pgen_tmp_65_333, 562 /* cst_109 */, 65536, 1, 5, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n9_53, &_preg_268435504, &_pgen_tmp_65_333);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&result_n9_53._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&result_n9_53._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435504._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_65_333._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Set_coeffs(&result_n9_53._c1_poly, _pgen_rns_idx_267, degree, Coeffs(&_preg_268435504._c1_poly, _pgen_rns_idx_267, degree));
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435505, &result_n9_53);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 55
  // /fire4/expand1/expand1.2/AveragePool
  // pragma: 65536, 1026, 56
  CMPLR_TM_START(3, 0);
  Copy_ciph(&_preg_268435506, &_preg_268435505);
  _preg_268435669 = Rotate(_preg_268435506, 32);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&tmp_row_add9_57, &_preg_268435506, &_preg_268435669);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&tmp_row_add9_57._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&tmp_row_add9_57._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435506._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435669._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&tmp_row_add9_57._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435506._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435669._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  _preg_268435670 = Rotate(tmp_row_add9_57, 1);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_ckks_gen_tmp_1256_226, &tmp_row_add9_57, &_preg_268435670);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_ckks_gen_tmp_1256_226._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_ckks_gen_tmp_1256_226._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_row_add9_57._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435670._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_ckks_gen_tmp_1256_226._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_row_add9_57._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435670._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  _pgen_tmp_67_335 = *(PLAIN)Pt_from_msg(&_pgen_tmp_67_335, 563 /* cst_112 */, 65536, 1, 5, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_66_334, &_ckks_gen_tmp_1256_226, &_pgen_tmp_67_335);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_66_334._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_66_334._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_1256_226._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_67_335._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_66_334._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_1256_226._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_67_335._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435507, &_pgen_tmp_66_334);
  Rescale(&_preg_268435507._c0_poly, &_pgen_tmp_66_334._c0_poly);
  Rescale(&_preg_268435507._c1_poly, &_pgen_tmp_66_334._c1_poly);
  CMPLR_TM_END(19, 0);
  Copy_ciph(&orig_result9_58, &_preg_268435507);
  Zero_ciph(&comb_row_result10_59);
  for (comb_row_index_n10_60 = 0; comb_row_index_n10_60 < 16; comb_row_index_n10_60 = comb_row_index_n10_60 + 1) {
    _preg_268435671 = Rotate(orig_result9_58, comb_row_index_n10_60 * 1);
    Copy_ciph(&_ckks_gen_tmp_1288_227, &_preg_268435671);
    _pgen_tmp_68_336 = *(PLAIN)Pt_from_msg(&_pgen_tmp_68_336, comb_row_index_n10_60 + 564 /* cst_113_0 */, 65536, 1, 4, 0);
    CMPLR_TM_START(17, 0);
    Init_ciph_up_scale_plain(&_pgen_tmp_69_337, &_ckks_gen_tmp_1288_227, &_pgen_tmp_68_336);
    CMPLR_TM_END(17, 0);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&comb_row_result10_59, &comb_row_result10_59, &_pgen_tmp_69_337);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&comb_row_result10_59._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_69_337._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_1288_227._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_68_336._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_69_337._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_1288_227._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_68_336._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&comb_row_result10_59._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&comb_row_result10_59._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_69_337._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&comb_row_result10_59._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&comb_row_result10_59._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_69_337._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&comb_row_result10_59, &comb_row_result10_59);
  Rescale(&comb_row_result10_59._c0_poly, &comb_row_result10_59._c0_poly);
  Rescale(&comb_row_result10_59._c1_poly, &comb_row_result10_59._c1_poly);
  CMPLR_TM_END(19, 0);
  Zero_ciph(&comb_rc_result10_61);
  for (comb_rc_index_n10_62 = 0; comb_rc_index_n10_62 < 16; comb_rc_index_n10_62 = comb_rc_index_n10_62 + 1) {
    _preg_268435672 = Rotate(comb_row_result10_59, comb_rc_index_n10_62 * 48);
    Copy_ciph(&_ckks_gen_tmp_1322_228, &_preg_268435672);
    _pgen_tmp_70_338 = *(PLAIN)Pt_from_msg(&_pgen_tmp_70_338, comb_rc_index_n10_62 + 580 /* cst_114_0 */, 65536, 1, 3, 0);
    CMPLR_TM_START(17, 0);
    Init_ciph_up_scale_plain(&_pgen_tmp_71_339, &_ckks_gen_tmp_1322_228, &_pgen_tmp_70_338);
    CMPLR_TM_END(17, 0);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&comb_rc_result10_61, &comb_rc_result10_61, &_pgen_tmp_71_339);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&comb_rc_result10_61._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_71_339._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_1322_228._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_70_338._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_71_339._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_1322_228._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_70_338._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&comb_rc_result10_61._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&comb_rc_result10_61._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_71_339._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&comb_rc_result10_61._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&comb_rc_result10_61._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_71_339._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&comb_rc_result10_61, &comb_rc_result10_61);
  Rescale(&comb_rc_result10_61._c0_poly, &comb_rc_result10_61._c0_poly);
  Rescale(&comb_rc_result10_61._c1_poly, &comb_rc_result10_61._c1_poly);
  CMPLR_TM_END(19, 0);
  Zero_ciph(&ss_result10_63);
  for (comb_cc_index_n10_64 = 0; comb_cc_index_n10_64 < 64; comb_cc_index_n10_64 = comb_cc_index_n10_64 + 1) {
    _preg_268435673 = Rotate(comb_rc_result10_61, comb_cc_index_n10_64 * 768);
    Copy_ciph(&_ckks_gen_tmp_1356_229, &_preg_268435673);
    _pgen_tmp_72_340 = *(PLAIN)Pt_from_msg(&_pgen_tmp_72_340, comb_cc_index_n10_64 + 596 /* cst_115_0 */, 65536, 1, 2, 0);
    CMPLR_TM_START(17, 0);
    Init_ciph_up_scale_plain(&_pgen_tmp_73_341, &_ckks_gen_tmp_1356_229, &_pgen_tmp_72_340);
    CMPLR_TM_END(17, 0);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&ss_result10_63, &ss_result10_63, &_pgen_tmp_73_341);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&ss_result10_63._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_73_341._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_1356_229._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_72_340._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_73_341._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_1356_229._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_72_340._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&ss_result10_63._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&ss_result10_63._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_73_341._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&ss_result10_63._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&ss_result10_63._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_73_341._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&ss_result10_63, &ss_result10_63);
  Rescale(&ss_result10_63._c0_poly, &ss_result10_63._c0_poly);
  Rescale(&ss_result10_63._c1_poly, &ss_result10_63._c1_poly);
  CMPLR_TM_END(19, 0);
  Copy_ciph(&_preg_268435508, &ss_result10_63);
  CMPLR_TM_END(3, 0);
  // pragma: 65537, 1026, 56
  // /fire4/expand2/expand2.0/Conv
  // pragma: 65536, 1027, 58
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435509, &_preg_268435502);
  // BlockingRot replicate=2
  _preg_268435674 = Rotate(_preg_268435509, -32768);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&input_dup_n10_65, &_preg_268435509, &_preg_268435674);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&input_dup_n10_65._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&input_dup_n10_65._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435509._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435674._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n10_65._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435509._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435674._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  // BlockingRot HRot=bs=9
  for (bs_input_n10_66 = 0; bs_input_n10_66 < 4; bs_input_n10_66 = bs_input_n10_66 + 1) {
    _preg_268435675 = Rotate(input_dup_n10_65, _cst_119[bs_input_n10_66]);
    Copy_ciph(&input_block_n10_67[bs_input_n10_66], &_preg_268435675);
  }
  _preg_268435676 = Rotate(input_dup_n10_65, 0);
  Copy_ciph(&input_block_n10_67[4], &_preg_268435676);
  for (bs_input_n10_rs2_68 = 5; bs_input_n10_rs2_68 < 9; bs_input_n10_rs2_68 = bs_input_n10_rs2_68 + 1) {
    _preg_268435677 = Rotate(input_dup_n10_65, _cst_119[bs_input_n10_rs2_68]);
    Copy_ciph(&input_block_n10_67[bs_input_n10_rs2_68], &_preg_268435677);
  }
  Zero_ciph(&result_n11_69);
  for (num_grid_n11_70 = 0; num_grid_n11_70 < 32; num_grid_n11_70 = num_grid_n11_70 + 1) {
    Zero_ciph(&result_grid_n11_71);
    for (cap_block_n11_72 = 0; cap_block_n11_72 < 9; cap_block_n11_72 = cap_block_n11_72 + 1) {
      _ckks_gen_tmp_1466_230 = input_block_n10_67[cap_block_n11_72];
      _pgen_tmp_74_342 = *(PLAIN)Pt_from_msg(&_pgen_tmp_74_342, (cap_block_n11_72 + (num_grid_n11_70 * 9)) + 660 /* cst_117_0 */, 65536, 1, 7, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_75_343, &_ckks_gen_tmp_1466_230, &_pgen_tmp_74_342);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n11_71, &result_grid_n11_71, &_pgen_tmp_75_343);
      _pgen_modulus_265 = Q_modulus();
      _pgen_num_q_266 = Poly_level(&result_grid_n11_71._c0_poly);
      for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_75_343._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_1466_230._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_74_342._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_75_343._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_1466_230._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_74_342._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n11_71._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n11_71._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_75_343._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n11_71._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n11_71._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_75_343._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        _pgen_modulus_265 = _pgen_modulus_265 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435678 = Rotate(result_grid_n11_71, num_grid_n11_70 * 1024);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n11_69, &result_n11_69, &_preg_268435678);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&result_n11_69._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modadd(Coeffs(&result_n11_69._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n11_69._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435678._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&result_n11_69._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n11_69._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435678._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n11_69, &result_n11_69);
  Rescale(&result_n11_69._c0_poly, &result_n11_69._c0_poly);
  Rescale(&result_n11_69._c1_poly, &result_n11_69._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_77_345 = *(PLAIN)Pt_from_msg(&_pgen_tmp_77_345, 948 /* cst_120 */, 65536, 1, 6, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_76_344, &result_n11_69, &_pgen_tmp_77_345);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_76_344._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_76_344._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n11_69._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_77_345._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_76_344._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n11_69._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_77_345._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435510, &_pgen_tmp_76_344);
  Rescale(&_preg_268435510._c0_poly, &_pgen_tmp_76_344._c0_poly);
  Rescale(&_preg_268435510._c1_poly, &_pgen_tmp_76_344._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_78_346 = *(PLAIN)Pt_from_msg(&_pgen_tmp_78_346, 949 /* cst_118 */, 65536, 1, 5, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n11_69, &_preg_268435510, &_pgen_tmp_78_346);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&result_n11_69._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&result_n11_69._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435510._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_78_346._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Set_coeffs(&result_n11_69._c1_poly, _pgen_rns_idx_267, degree, Coeffs(&_preg_268435510._c1_poly, _pgen_rns_idx_267, degree));
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435511, &result_n11_69);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 58
  // /fire4/expand2/expand2.2/AveragePool
  // pragma: 65536, 1026, 59
  CMPLR_TM_START(3, 0);
  Copy_ciph(&_preg_268435512, &_preg_268435511);
  _preg_268435679 = Rotate(_preg_268435512, 32);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&tmp_row_add11_73, &_preg_268435512, &_preg_268435679);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&tmp_row_add11_73._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&tmp_row_add11_73._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435512._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435679._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&tmp_row_add11_73._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435512._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435679._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  _preg_268435680 = Rotate(tmp_row_add11_73, 1);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_ckks_gen_tmp_1543_231, &tmp_row_add11_73, &_preg_268435680);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_ckks_gen_tmp_1543_231._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_ckks_gen_tmp_1543_231._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_row_add11_73._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435680._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_ckks_gen_tmp_1543_231._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_row_add11_73._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435680._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  _pgen_tmp_80_348 = *(PLAIN)Pt_from_msg(&_pgen_tmp_80_348, 950 /* cst_121 */, 65536, 1, 5, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_79_347, &_ckks_gen_tmp_1543_231, &_pgen_tmp_80_348);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_79_347._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_79_347._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_1543_231._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_80_348._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_79_347._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_1543_231._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_80_348._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435513, &_pgen_tmp_79_347);
  Rescale(&_preg_268435513._c0_poly, &_pgen_tmp_79_347._c0_poly);
  Rescale(&_preg_268435513._c1_poly, &_pgen_tmp_79_347._c1_poly);
  CMPLR_TM_END(19, 0);
  Copy_ciph(&orig_result11_74, &_preg_268435513);
  Zero_ciph(&comb_row_result12_75);
  for (comb_row_index_n12_76 = 0; comb_row_index_n12_76 < 16; comb_row_index_n12_76 = comb_row_index_n12_76 + 1) {
    _preg_268435681 = Rotate(orig_result11_74, comb_row_index_n12_76 * 1);
    Copy_ciph(&_ckks_gen_tmp_1575_232, &_preg_268435681);
    _pgen_tmp_81_349 = *(PLAIN)Pt_from_msg(&_pgen_tmp_81_349, comb_row_index_n12_76 + 951 /* cst_122_0 */, 65536, 1, 4, 0);
    CMPLR_TM_START(17, 0);
    Init_ciph_up_scale_plain(&_pgen_tmp_82_350, &_ckks_gen_tmp_1575_232, &_pgen_tmp_81_349);
    CMPLR_TM_END(17, 0);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&comb_row_result12_75, &comb_row_result12_75, &_pgen_tmp_82_350);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&comb_row_result12_75._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_82_350._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_1575_232._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_81_349._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_82_350._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_1575_232._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_81_349._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&comb_row_result12_75._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&comb_row_result12_75._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_82_350._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&comb_row_result12_75._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&comb_row_result12_75._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_82_350._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&comb_row_result12_75, &comb_row_result12_75);
  Rescale(&comb_row_result12_75._c0_poly, &comb_row_result12_75._c0_poly);
  Rescale(&comb_row_result12_75._c1_poly, &comb_row_result12_75._c1_poly);
  CMPLR_TM_END(19, 0);
  Zero_ciph(&comb_rc_result12_77);
  for (comb_rc_index_n12_78 = 0; comb_rc_index_n12_78 < 16; comb_rc_index_n12_78 = comb_rc_index_n12_78 + 1) {
    _preg_268435682 = Rotate(comb_row_result12_75, comb_rc_index_n12_78 * 48);
    Copy_ciph(&_ckks_gen_tmp_1609_233, &_preg_268435682);
    _pgen_tmp_83_351 = *(PLAIN)Pt_from_msg(&_pgen_tmp_83_351, comb_rc_index_n12_78 + 967 /* cst_123_0 */, 65536, 1, 3, 0);
    CMPLR_TM_START(17, 0);
    Init_ciph_up_scale_plain(&_pgen_tmp_84_352, &_ckks_gen_tmp_1609_233, &_pgen_tmp_83_351);
    CMPLR_TM_END(17, 0);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&comb_rc_result12_77, &comb_rc_result12_77, &_pgen_tmp_84_352);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&comb_rc_result12_77._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_84_352._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_1609_233._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_83_351._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_84_352._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_1609_233._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_83_351._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&comb_rc_result12_77._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&comb_rc_result12_77._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_84_352._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&comb_rc_result12_77._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&comb_rc_result12_77._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_84_352._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&comb_rc_result12_77, &comb_rc_result12_77);
  Rescale(&comb_rc_result12_77._c0_poly, &comb_rc_result12_77._c0_poly);
  Rescale(&comb_rc_result12_77._c1_poly, &comb_rc_result12_77._c1_poly);
  CMPLR_TM_END(19, 0);
  Zero_ciph(&ss_result12_79);
  for (comb_cc_index_n12_80 = 0; comb_cc_index_n12_80 < 64; comb_cc_index_n12_80 = comb_cc_index_n12_80 + 1) {
    _preg_268435683 = Rotate(comb_rc_result12_77, comb_cc_index_n12_80 * 768);
    Copy_ciph(&_ckks_gen_tmp_1643_234, &_preg_268435683);
    _pgen_tmp_85_353 = *(PLAIN)Pt_from_msg(&_pgen_tmp_85_353, comb_cc_index_n12_80 + 983 /* cst_124_0 */, 65536, 1, 2, 0);
    CMPLR_TM_START(17, 0);
    Init_ciph_up_scale_plain(&_pgen_tmp_86_354, &_ckks_gen_tmp_1643_234, &_pgen_tmp_85_353);
    CMPLR_TM_END(17, 0);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&ss_result12_79, &ss_result12_79, &_pgen_tmp_86_354);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&ss_result12_79._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_86_354._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_1643_234._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_85_353._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_86_354._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_1643_234._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_85_353._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&ss_result12_79._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&ss_result12_79._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_86_354._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&ss_result12_79._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&ss_result12_79._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_86_354._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&ss_result12_79, &ss_result12_79);
  Rescale(&ss_result12_79._c0_poly, &ss_result12_79._c0_poly);
  Rescale(&ss_result12_79._c1_poly, &ss_result12_79._c1_poly);
  CMPLR_TM_END(19, 0);
  Copy_ciph(&_preg_268435514, &ss_result12_79);
  CMPLR_TM_END(3, 0);
  // pragma: 65537, 1026, 59
  // /fire4/Concat
  // pragma: 65536, 1045, 60
  CMPLR_TM_START(4, 0);
  _preg_268435684 = Rotate(_preg_268435514, -16384);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_preg_268435515, &_preg_268435508, &_preg_268435684);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_preg_268435515._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_preg_268435515._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435508._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435684._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_preg_268435515._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435508._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435684._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  CMPLR_TM_END(4, 0);
  // pragma: 65537, 1045, 60
  // /fire4/Relu
  // pragma: 65536, 1033, 61
  CMPLR_TM_START(10, 0);
  Bootstrap(&_preg_268435516, &_preg_268435515, 14);
  Encode_float(&_pgen_tmp_88_356, &_cst_267, 1, 1, Level(&_preg_268435516), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_87_355, &_preg_268435516, &_pgen_tmp_88_356);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_87_355._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_87_355._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435516._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_88_356._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_87_355._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435516._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_88_356._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435518, &_pgen_tmp_87_355);
  Rescale(&_preg_268435518._c0_poly, &_pgen_tmp_87_355._c0_poly);
  Rescale(&_preg_268435518._c1_poly, &_pgen_tmp_87_355._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435517 = App_relu(_preg_268435516, _preg_268435518, 32768);
  Copy_ciph(&_preg_268435519, &_preg_268435517);
  CMPLR_TM_END(10, 0);
  // pragma: 65537, 1033, 61
  // /fire5/squeeze/squeeze.0/Conv
  // pragma: 65536, 1027, 62
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435520, &_preg_268435519);
  Zero_ciph(&tmp_result_n13_81);
  _preg_268435685 = Rotate(_preg_268435520, -32768);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&input_dup_n13_82, &_preg_268435520, &_preg_268435685);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&input_dup_n13_82._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&input_dup_n13_82._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435520._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435685._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n13_82._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435520._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435685._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  for (index_cin_n13_83 = 0; index_cin_n13_83 < 128; index_cin_n13_83 = index_cin_n13_83 + 1) {
    for (index_khw_n13_84 = 0; index_khw_n13_84 < 1; index_khw_n13_84 = index_khw_n13_84 + 1) {
      _pgen_tmp_89_357 = *(PLAIN)Pt_from_msg(&_pgen_tmp_89_357, (index_khw_n13_84 + (index_cin_n13_83 * 1)) + 1047 /* cst_126_0 */, 8192, 1, 2, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_90_358, &input_dup_n13_82, &_pgen_tmp_89_357);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&tmp_result_n13_81, &tmp_result_n13_81, &_pgen_tmp_90_358);
      _pgen_modulus_265 = Q_modulus();
      _pgen_num_q_266 = Poly_level(&tmp_result_n13_81._c0_poly);
      for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_90_358._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&input_dup_n13_82._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_89_357._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_90_358._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&input_dup_n13_82._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_89_357._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&tmp_result_n13_81._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_result_n13_81._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_90_358._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&tmp_result_n13_81._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_result_n13_81._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_90_358._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        _pgen_modulus_265 = _pgen_modulus_265 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435686 = Rotate(input_dup_n13_82, 256);
    Copy_ciph(&input_dup_n13_82, &_preg_268435686);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&tmp_result_n13_81, &tmp_result_n13_81);
  Rescale(&tmp_result_n13_81._c0_poly, &tmp_result_n13_81._c0_poly);
  Rescale(&tmp_result_n13_81._c1_poly, &tmp_result_n13_81._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_91_359 = *(PLAIN)Pt_from_msg(&_pgen_tmp_91_359, 1175 /* cst_127 */, 8192, 1, 1, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&tmp_result_n13_81, &tmp_result_n13_81, &_pgen_tmp_91_359);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&tmp_result_n13_81._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&tmp_result_n13_81._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_result_n13_81._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_91_359._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Set_coeffs(&tmp_result_n13_81._c1_poly, _pgen_rns_idx_267, degree, Coeffs(&tmp_result_n13_81._c1_poly, _pgen_rns_idx_267, degree));
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435521, &tmp_result_n13_81);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 62
  // /fire5/squeeze/squeeze.2/Relu
  // pragma: 65536, 1033, 63
  CMPLR_TM_START(10, 0);
  Bootstrap(&_preg_268435522, &_preg_268435521, 15);
  Encode_float(&_pgen_tmp_93_361, &_cst_268, 1, 1, Level(&_preg_268435522), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_92_360, &_preg_268435522, &_pgen_tmp_93_361);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_92_360._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_92_360._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435522._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_93_361._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_92_360._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435522._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_93_361._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435524, &_pgen_tmp_92_360);
  Rescale(&_preg_268435524._c0_poly, &_pgen_tmp_92_360._c0_poly);
  Rescale(&_preg_268435524._c1_poly, &_pgen_tmp_92_360._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435523 = App_relu(_preg_268435522, _preg_268435524, 8192);
  Copy_ciph(&_preg_268435525, &_preg_268435523);
  CMPLR_TM_END(10, 0);
  // pragma: 65537, 1033, 63
  // /fire5/expand1/expand1.0/Conv
  // pragma: 65536, 1027, 64
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435526, &_preg_268435525);
  // BlockingRot replicate=5
  _preg_268435687 = Rotate(_preg_268435526, -8192);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_94_362, &_preg_268435526, &_preg_268435687);
  CMPLR_TM_END(12, 0);
  _preg_268435688 = Rotate(_preg_268435526, -16384);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_95_363, &_pgen_tmp_94_362, &_preg_268435688);
  CMPLR_TM_END(12, 0);
  _preg_268435689 = Rotate(_preg_268435526, -24576);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_96_364, &_pgen_tmp_95_363, &_preg_268435689);
  CMPLR_TM_END(12, 0);
  _preg_268435690 = Rotate(_preg_268435526, -32768);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&input_dup_n13_85, &_pgen_tmp_96_364, &_preg_268435690);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&input_dup_n13_85._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_94_362._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435526._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435687._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_94_362._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435526._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435687._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_95_363._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_94_362._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435688._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_95_363._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_94_362._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435688._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_96_364._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_95_363._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435689._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_96_364._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_95_363._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435689._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n13_85._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_96_364._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435690._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n13_85._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_96_364._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435690._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  // BlockingRot HRot=bs=4
  _preg_268435691 = Rotate(input_dup_n13_85, 0);
  Copy_ciph(&input_block_n13_86[0], &_preg_268435691);
  for (bs_input_n13_87 = 1; bs_input_n13_87 < 4; bs_input_n13_87 = bs_input_n13_87 + 1) {
    _preg_268435692 = Rotate(input_dup_n13_85, _cst_132[bs_input_n13_87]);
    Copy_ciph(&input_block_n13_86[bs_input_n13_87], &_preg_268435692);
  }
  Zero_ciph(&result_n14_88);
  for (num_grid_n14_89 = 0; num_grid_n14_89 < 8; num_grid_n14_89 = num_grid_n14_89 + 1) {
    Zero_ciph(&result_grid_n14_90);
    for (cap_block_n14_91 = 0; cap_block_n14_91 < 4; cap_block_n14_91 = cap_block_n14_91 + 1) {
      _ckks_gen_tmp_1880_235 = input_block_n13_86[cap_block_n14_91];
      _pgen_tmp_97_365 = *(PLAIN)Pt_from_msg(&_pgen_tmp_97_365, (cap_block_n14_91 + (num_grid_n14_89 * 4)) + 1176 /* cst_130_0 */, 32768, 1, 3, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_98_366, &_ckks_gen_tmp_1880_235, &_pgen_tmp_97_365);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n14_90, &result_grid_n14_90, &_pgen_tmp_98_366);
      _pgen_modulus_265 = Q_modulus();
      _pgen_num_q_266 = Poly_level(&result_grid_n14_90._c0_poly);
      for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_98_366._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_1880_235._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_97_365._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_98_366._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_1880_235._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_97_365._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n14_90._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n14_90._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_98_366._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n14_90._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n14_90._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_98_366._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        _pgen_modulus_265 = _pgen_modulus_265 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435693 = Rotate(result_grid_n14_90, num_grid_n14_89 * 1024);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n14_88, &result_n14_88, &_preg_268435693);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&result_n14_88._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modadd(Coeffs(&result_n14_88._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n14_88._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435693._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&result_n14_88._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n14_88._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435693._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n14_88, &result_n14_88);
  Rescale(&result_n14_88._c0_poly, &result_n14_88._c0_poly);
  Rescale(&result_n14_88._c1_poly, &result_n14_88._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435694 = Rotate(result_n14_88, -32768);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_ckks_gen_tmp_1916_236, &result_n14_88, &_preg_268435694);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_ckks_gen_tmp_1916_236._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_ckks_gen_tmp_1916_236._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n14_88._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435694._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_ckks_gen_tmp_1916_236._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n14_88._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435694._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  _pgen_tmp_100_368 = *(PLAIN)Pt_from_msg(&_pgen_tmp_100_368, 1208 /* cst_133 */, 32768, 1, 2, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_99_367, &_ckks_gen_tmp_1916_236, &_pgen_tmp_100_368);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_99_367._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_99_367._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_1916_236._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_100_368._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_99_367._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_1916_236._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_100_368._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435527, &_pgen_tmp_99_367);
  Rescale(&_preg_268435527._c0_poly, &_pgen_tmp_99_367._c0_poly);
  Rescale(&_preg_268435527._c1_poly, &_pgen_tmp_99_367._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_101_369 = *(PLAIN)Pt_from_msg(&_pgen_tmp_101_369, 1209 /* cst_131 */, 32768, 1, 1, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n14_88, &_preg_268435527, &_pgen_tmp_101_369);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&result_n14_88._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&result_n14_88._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435527._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_101_369._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Set_coeffs(&result_n14_88._c1_poly, _pgen_rns_idx_267, degree, Coeffs(&_preg_268435527._c1_poly, _pgen_rns_idx_267, degree));
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435528, &result_n14_88);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 64
  // /fire5/expand2/expand2.0/Conv
  // pragma: 65536, 1027, 65
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435529, &_preg_268435525);
  // BlockingRot replicate=4
  _preg_268435695 = Rotate(_preg_268435529, -8192);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_102_370, &_preg_268435529, &_preg_268435695);
  CMPLR_TM_END(12, 0);
  _preg_268435696 = Rotate(_preg_268435529, -16384);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_103_371, &_pgen_tmp_102_370, &_preg_268435696);
  CMPLR_TM_END(12, 0);
  _preg_268435697 = Rotate(_preg_268435529, -24576);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&input_dup_n14_92, &_pgen_tmp_103_371, &_preg_268435697);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&input_dup_n14_92._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_102_370._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435529._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435695._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_102_370._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435529._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435695._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_103_371._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_102_370._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435696._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_103_371._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_102_370._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435696._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n14_92._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_103_371._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435697._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n14_92._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_103_371._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435697._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  // BlockingRot HRot=bs=9
  for (bs_input_n14_93 = 0; bs_input_n14_93 < 4; bs_input_n14_93 = bs_input_n14_93 + 1) {
    _preg_268435698 = Rotate(input_dup_n14_92, _cst_137[bs_input_n14_93]);
    Copy_ciph(&input_block_n14_94[bs_input_n14_93], &_preg_268435698);
  }
  _preg_268435699 = Rotate(input_dup_n14_92, 0);
  Copy_ciph(&input_block_n14_94[4], &_preg_268435699);
  for (bs_input_n14_rs2_95 = 5; bs_input_n14_rs2_95 < 9; bs_input_n14_rs2_95 = bs_input_n14_rs2_95 + 1) {
    _preg_268435700 = Rotate(input_dup_n14_92, _cst_137[bs_input_n14_rs2_95]);
    Copy_ciph(&input_block_n14_94[bs_input_n14_rs2_95], &_preg_268435700);
  }
  Zero_ciph(&result_n15_96);
  for (num_grid_n15_97 = 0; num_grid_n15_97 < 32; num_grid_n15_97 = num_grid_n15_97 + 1) {
    Zero_ciph(&result_grid_n15_98);
    for (cap_block_n15_99 = 0; cap_block_n15_99 < 9; cap_block_n15_99 = cap_block_n15_99 + 1) {
      _ckks_gen_tmp_2041_237 = input_block_n14_94[cap_block_n15_99];
      _pgen_tmp_104_372 = *(PLAIN)Pt_from_msg(&_pgen_tmp_104_372, (cap_block_n15_99 + (num_grid_n15_97 * 9)) + 1210 /* cst_135_0 */, 32768, 1, 3, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_105_373, &_ckks_gen_tmp_2041_237, &_pgen_tmp_104_372);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n15_98, &result_grid_n15_98, &_pgen_tmp_105_373);
      _pgen_modulus_265 = Q_modulus();
      _pgen_num_q_266 = Poly_level(&result_grid_n15_98._c0_poly);
      for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_105_373._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_2041_237._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_104_372._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_105_373._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_2041_237._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_104_372._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n15_98._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n15_98._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_105_373._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n15_98._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n15_98._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_105_373._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        _pgen_modulus_265 = _pgen_modulus_265 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435701 = Rotate(result_grid_n15_98, num_grid_n15_97 * 256);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n15_96, &result_n15_96, &_preg_268435701);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&result_n15_96._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modadd(Coeffs(&result_n15_96._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n15_96._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435701._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&result_n15_96._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n15_96._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435701._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n15_96, &result_n15_96);
  Rescale(&result_n15_96._c0_poly, &result_n15_96._c0_poly);
  Rescale(&result_n15_96._c1_poly, &result_n15_96._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435702 = Rotate(result_n15_96, -32768);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_ckks_gen_tmp_2077_238, &result_n15_96, &_preg_268435702);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_ckks_gen_tmp_2077_238._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_ckks_gen_tmp_2077_238._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n15_96._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435702._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_ckks_gen_tmp_2077_238._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n15_96._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435702._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  _pgen_tmp_107_375 = *(PLAIN)Pt_from_msg(&_pgen_tmp_107_375, 1498 /* cst_138 */, 32768, 1, 2, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_106_374, &_ckks_gen_tmp_2077_238, &_pgen_tmp_107_375);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_106_374._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_106_374._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_2077_238._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_107_375._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_106_374._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_2077_238._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_107_375._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435530, &_pgen_tmp_106_374);
  Rescale(&_preg_268435530._c0_poly, &_pgen_tmp_106_374._c0_poly);
  Rescale(&_preg_268435530._c1_poly, &_pgen_tmp_106_374._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_108_376 = *(PLAIN)Pt_from_msg(&_pgen_tmp_108_376, 1499 /* cst_136 */, 32768, 1, 1, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n15_96, &_preg_268435530, &_pgen_tmp_108_376);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&result_n15_96._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&result_n15_96._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435530._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_108_376._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Set_coeffs(&result_n15_96._c1_poly, _pgen_rns_idx_267, degree, Coeffs(&_preg_268435530._c1_poly, _pgen_rns_idx_267, degree));
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435531, &result_n15_96);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 65
  // /fire5/Concat
  // pragma: 65536, 1045, 66
  CMPLR_TM_START(4, 0);
  _preg_268435703 = Rotate(_preg_268435531, -32768);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_preg_268435532, &_preg_268435528, &_preg_268435703);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_preg_268435532._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_preg_268435532._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435528._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435703._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_preg_268435532._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435528._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435703._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  CMPLR_TM_END(4, 0);
  // pragma: 65537, 1045, 66
  // /fire5/Relu
  // pragma: 65536, 1033, 67
  CMPLR_TM_START(10, 0);
  Bootstrap(&_preg_268435533, &_preg_268435532, 14);
  Encode_float(&_pgen_tmp_110_378, &_cst_269, 1, 1, Level(&_preg_268435533), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_109_377, &_preg_268435533, &_pgen_tmp_110_378);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_109_377._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_109_377._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435533._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_110_378._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_109_377._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435533._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_110_378._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435535, &_pgen_tmp_109_377);
  Rescale(&_preg_268435535._c0_poly, &_pgen_tmp_109_377._c0_poly);
  Rescale(&_preg_268435535._c1_poly, &_pgen_tmp_109_377._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435534 = App_relu(_preg_268435533, _preg_268435535, 65536);
  Copy_ciph(&_preg_268435536, &_preg_268435534);
  CMPLR_TM_END(10, 0);
  // pragma: 65537, 1033, 67
  // /fire6/squeeze/squeeze.0/Conv
  // pragma: 65536, 1027, 68
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435537, &_preg_268435536);
  Zero_ciph(&tmp_result_n16_100);
  Copy_ciph(&input_dup_n16_101, &_preg_268435537);
  for (index_cin_n16_102 = 0; index_cin_n16_102 < 256; index_cin_n16_102 = index_cin_n16_102 + 1) {
    for (index_khw_n16_103 = 0; index_khw_n16_103 < 1; index_khw_n16_103 = index_khw_n16_103 + 1) {
      _pgen_tmp_111_379 = *(PLAIN)Pt_from_msg(&_pgen_tmp_111_379, (index_khw_n16_103 + (index_cin_n16_102 * 1)) + 1500 /* cst_140_0 */, 8192, 1, 2, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_112_380, &input_dup_n16_101, &_pgen_tmp_111_379);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&tmp_result_n16_100, &tmp_result_n16_100, &_pgen_tmp_112_380);
      _pgen_modulus_265 = Q_modulus();
      _pgen_num_q_266 = Poly_level(&tmp_result_n16_100._c0_poly);
      for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_112_380._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&input_dup_n16_101._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_111_379._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_112_380._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&input_dup_n16_101._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_111_379._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&tmp_result_n16_100._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_result_n16_100._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_112_380._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&tmp_result_n16_100._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_result_n16_100._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_112_380._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        _pgen_modulus_265 = _pgen_modulus_265 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435704 = Rotate(input_dup_n16_101, 256);
    Copy_ciph(&input_dup_n16_101, &_preg_268435704);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&tmp_result_n16_100, &tmp_result_n16_100);
  Rescale(&tmp_result_n16_100._c0_poly, &tmp_result_n16_100._c0_poly);
  Rescale(&tmp_result_n16_100._c1_poly, &tmp_result_n16_100._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_113_381 = *(PLAIN)Pt_from_msg(&_pgen_tmp_113_381, 1756 /* cst_141 */, 8192, 1, 1, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&tmp_result_n16_100, &tmp_result_n16_100, &_pgen_tmp_113_381);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&tmp_result_n16_100._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&tmp_result_n16_100._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_result_n16_100._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_113_381._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Set_coeffs(&tmp_result_n16_100._c1_poly, _pgen_rns_idx_267, degree, Coeffs(&tmp_result_n16_100._c1_poly, _pgen_rns_idx_267, degree));
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435538, &tmp_result_n16_100);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 68
  // /fire6/squeeze/squeeze.2/Relu
  // pragma: 65536, 1033, 69
  CMPLR_TM_START(10, 0);
  Bootstrap(&_preg_268435539, &_preg_268435538, 15);
  Encode_float(&_pgen_tmp_115_383, &_cst_270, 1, 1, Level(&_preg_268435539), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_114_382, &_preg_268435539, &_pgen_tmp_115_383);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_114_382._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_114_382._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435539._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_115_383._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_114_382._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435539._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_115_383._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435541, &_pgen_tmp_114_382);
  Rescale(&_preg_268435541._c0_poly, &_pgen_tmp_114_382._c0_poly);
  Rescale(&_preg_268435541._c1_poly, &_pgen_tmp_114_382._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435540 = App_relu(_preg_268435539, _preg_268435541, 8192);
  Copy_ciph(&_preg_268435542, &_preg_268435540);
  CMPLR_TM_END(10, 0);
  // pragma: 65537, 1033, 69
  // /fire6/expand1/expand1.0/Conv
  // pragma: 65536, 1027, 70
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435543, &_preg_268435542);
  // BlockingRot replicate=5
  _preg_268435705 = Rotate(_preg_268435543, -8192);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_116_384, &_preg_268435543, &_preg_268435705);
  CMPLR_TM_END(12, 0);
  _preg_268435706 = Rotate(_preg_268435543, -16384);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_117_385, &_pgen_tmp_116_384, &_preg_268435706);
  CMPLR_TM_END(12, 0);
  _preg_268435707 = Rotate(_preg_268435543, -24576);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_118_386, &_pgen_tmp_117_385, &_preg_268435707);
  CMPLR_TM_END(12, 0);
  _preg_268435708 = Rotate(_preg_268435543, -32768);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&input_dup_n16_104, &_pgen_tmp_118_386, &_preg_268435708);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&input_dup_n16_104._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_116_384._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435543._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435705._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_116_384._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435543._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435705._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_117_385._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_116_384._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435706._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_117_385._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_116_384._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435706._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_118_386._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_117_385._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435707._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_118_386._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_117_385._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435707._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n16_104._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_118_386._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435708._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n16_104._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_118_386._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435708._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  // BlockingRot HRot=bs=4
  _preg_268435709 = Rotate(input_dup_n16_104, 0);
  Copy_ciph(&input_block_n16_105[0], &_preg_268435709);
  for (bs_input_n16_106 = 1; bs_input_n16_106 < 4; bs_input_n16_106 = bs_input_n16_106 + 1) {
    _preg_268435710 = Rotate(input_dup_n16_104, _cst_146[bs_input_n16_106]);
    Copy_ciph(&input_block_n16_105[bs_input_n16_106], &_preg_268435710);
  }
  Zero_ciph(&result_n17_107);
  for (num_grid_n17_108 = 0; num_grid_n17_108 < 8; num_grid_n17_108 = num_grid_n17_108 + 1) {
    Zero_ciph(&result_grid_n17_109);
    for (cap_block_n17_110 = 0; cap_block_n17_110 < 4; cap_block_n17_110 = cap_block_n17_110 + 1) {
      _ckks_gen_tmp_2317_239 = input_block_n16_105[cap_block_n17_110];
      _pgen_tmp_119_387 = *(PLAIN)Pt_from_msg(&_pgen_tmp_119_387, (cap_block_n17_110 + (num_grid_n17_108 * 4)) + 1757 /* cst_144_0 */, 32768, 1, 3, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_120_388, &_ckks_gen_tmp_2317_239, &_pgen_tmp_119_387);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n17_109, &result_grid_n17_109, &_pgen_tmp_120_388);
      _pgen_modulus_265 = Q_modulus();
      _pgen_num_q_266 = Poly_level(&result_grid_n17_109._c0_poly);
      for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_120_388._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_2317_239._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_119_387._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_120_388._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_2317_239._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_119_387._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n17_109._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n17_109._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_120_388._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n17_109._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n17_109._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_120_388._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        _pgen_modulus_265 = _pgen_modulus_265 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435711 = Rotate(result_grid_n17_109, num_grid_n17_108 * 1024);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n17_107, &result_n17_107, &_preg_268435711);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&result_n17_107._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modadd(Coeffs(&result_n17_107._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n17_107._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435711._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&result_n17_107._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n17_107._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435711._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n17_107, &result_n17_107);
  Rescale(&result_n17_107._c0_poly, &result_n17_107._c0_poly);
  Rescale(&result_n17_107._c1_poly, &result_n17_107._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435712 = Rotate(result_n17_107, -32768);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_ckks_gen_tmp_2353_240, &result_n17_107, &_preg_268435712);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_ckks_gen_tmp_2353_240._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_ckks_gen_tmp_2353_240._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n17_107._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435712._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_ckks_gen_tmp_2353_240._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n17_107._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435712._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  _pgen_tmp_122_390 = *(PLAIN)Pt_from_msg(&_pgen_tmp_122_390, 1789 /* cst_147 */, 32768, 1, 2, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_121_389, &_ckks_gen_tmp_2353_240, &_pgen_tmp_122_390);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_121_389._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_121_389._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_2353_240._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_122_390._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_121_389._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_2353_240._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_122_390._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435544, &_pgen_tmp_121_389);
  Rescale(&_preg_268435544._c0_poly, &_pgen_tmp_121_389._c0_poly);
  Rescale(&_preg_268435544._c1_poly, &_pgen_tmp_121_389._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_123_391 = *(PLAIN)Pt_from_msg(&_pgen_tmp_123_391, 1790 /* cst_145 */, 32768, 1, 1, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n17_107, &_preg_268435544, &_pgen_tmp_123_391);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&result_n17_107._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&result_n17_107._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435544._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_123_391._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Set_coeffs(&result_n17_107._c1_poly, _pgen_rns_idx_267, degree, Coeffs(&_preg_268435544._c1_poly, _pgen_rns_idx_267, degree));
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435545, &result_n17_107);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 70
  // /fire6/expand2/expand2.0/Conv
  // pragma: 65536, 1027, 71
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435546, &_preg_268435542);
  // BlockingRot replicate=4
  _preg_268435713 = Rotate(_preg_268435546, -8192);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_124_392, &_preg_268435546, &_preg_268435713);
  CMPLR_TM_END(12, 0);
  _preg_268435714 = Rotate(_preg_268435546, -16384);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_125_393, &_pgen_tmp_124_392, &_preg_268435714);
  CMPLR_TM_END(12, 0);
  _preg_268435715 = Rotate(_preg_268435546, -24576);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&input_dup_n17_111, &_pgen_tmp_125_393, &_preg_268435715);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&input_dup_n17_111._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_124_392._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435546._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435713._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_124_392._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435546._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435713._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_125_393._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_124_392._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435714._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_125_393._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_124_392._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435714._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n17_111._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_125_393._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435715._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n17_111._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_125_393._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435715._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  // BlockingRot HRot=bs=9
  for (bs_input_n17_112 = 0; bs_input_n17_112 < 4; bs_input_n17_112 = bs_input_n17_112 + 1) {
    _preg_268435716 = Rotate(input_dup_n17_111, _cst_151[bs_input_n17_112]);
    Copy_ciph(&input_block_n17_113[bs_input_n17_112], &_preg_268435716);
  }
  _preg_268435717 = Rotate(input_dup_n17_111, 0);
  Copy_ciph(&input_block_n17_113[4], &_preg_268435717);
  for (bs_input_n17_rs2_114 = 5; bs_input_n17_rs2_114 < 9; bs_input_n17_rs2_114 = bs_input_n17_rs2_114 + 1) {
    _preg_268435718 = Rotate(input_dup_n17_111, _cst_151[bs_input_n17_rs2_114]);
    Copy_ciph(&input_block_n17_113[bs_input_n17_rs2_114], &_preg_268435718);
  }
  Zero_ciph(&result_n18_115);
  for (num_grid_n18_116 = 0; num_grid_n18_116 < 32; num_grid_n18_116 = num_grid_n18_116 + 1) {
    Zero_ciph(&result_grid_n18_117);
    for (cap_block_n18_118 = 0; cap_block_n18_118 < 9; cap_block_n18_118 = cap_block_n18_118 + 1) {
      _ckks_gen_tmp_2478_241 = input_block_n17_113[cap_block_n18_118];
      _pgen_tmp_126_394 = *(PLAIN)Pt_from_msg(&_pgen_tmp_126_394, (cap_block_n18_118 + (num_grid_n18_116 * 9)) + 1791 /* cst_149_0 */, 32768, 1, 3, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_127_395, &_ckks_gen_tmp_2478_241, &_pgen_tmp_126_394);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n18_117, &result_grid_n18_117, &_pgen_tmp_127_395);
      _pgen_modulus_265 = Q_modulus();
      _pgen_num_q_266 = Poly_level(&result_grid_n18_117._c0_poly);
      for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_127_395._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_2478_241._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_126_394._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_127_395._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_2478_241._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_126_394._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n18_117._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n18_117._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_127_395._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n18_117._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n18_117._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_127_395._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        _pgen_modulus_265 = _pgen_modulus_265 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435719 = Rotate(result_grid_n18_117, num_grid_n18_116 * 256);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n18_115, &result_n18_115, &_preg_268435719);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&result_n18_115._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modadd(Coeffs(&result_n18_115._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n18_115._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435719._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&result_n18_115._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n18_115._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435719._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n18_115, &result_n18_115);
  Rescale(&result_n18_115._c0_poly, &result_n18_115._c0_poly);
  Rescale(&result_n18_115._c1_poly, &result_n18_115._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435720 = Rotate(result_n18_115, -32768);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_ckks_gen_tmp_2514_242, &result_n18_115, &_preg_268435720);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_ckks_gen_tmp_2514_242._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_ckks_gen_tmp_2514_242._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n18_115._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435720._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_ckks_gen_tmp_2514_242._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n18_115._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435720._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  _pgen_tmp_129_397 = *(PLAIN)Pt_from_msg(&_pgen_tmp_129_397, 2079 /* cst_152 */, 32768, 1, 2, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_128_396, &_ckks_gen_tmp_2514_242, &_pgen_tmp_129_397);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_128_396._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_128_396._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_2514_242._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_129_397._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_128_396._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_2514_242._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_129_397._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435547, &_pgen_tmp_128_396);
  Rescale(&_preg_268435547._c0_poly, &_pgen_tmp_128_396._c0_poly);
  Rescale(&_preg_268435547._c1_poly, &_pgen_tmp_128_396._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_130_398 = *(PLAIN)Pt_from_msg(&_pgen_tmp_130_398, 2080 /* cst_150 */, 32768, 1, 1, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n18_115, &_preg_268435547, &_pgen_tmp_130_398);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&result_n18_115._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&result_n18_115._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435547._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_130_398._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Set_coeffs(&result_n18_115._c1_poly, _pgen_rns_idx_267, degree, Coeffs(&_preg_268435547._c1_poly, _pgen_rns_idx_267, degree));
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435548, &result_n18_115);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 71
  // /fire6/Concat
  // pragma: 65536, 1045, 72
  CMPLR_TM_START(4, 0);
  _preg_268435721 = Rotate(_preg_268435548, -32768);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_preg_268435549, &_preg_268435545, &_preg_268435721);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_preg_268435549._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_preg_268435549._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435545._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435721._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_preg_268435549._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435545._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435721._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  CMPLR_TM_END(4, 0);
  // pragma: 65537, 1045, 72
  // /fire6/Relu
  // pragma: 65536, 1033, 73
  CMPLR_TM_START(10, 0);
  Bootstrap(&_preg_268435550, &_preg_268435549, 14);
  Encode_float(&_pgen_tmp_132_400, &_cst_271, 1, 1, Level(&_preg_268435550), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_131_399, &_preg_268435550, &_pgen_tmp_132_400);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_131_399._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_131_399._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435550._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_132_400._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_131_399._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435550._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_132_400._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435552, &_pgen_tmp_131_399);
  Rescale(&_preg_268435552._c0_poly, &_pgen_tmp_131_399._c0_poly);
  Rescale(&_preg_268435552._c1_poly, &_pgen_tmp_131_399._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435551 = App_relu(_preg_268435550, _preg_268435552, 65536);
  Copy_ciph(&_preg_268435553, &_preg_268435551);
  CMPLR_TM_END(10, 0);
  // pragma: 65537, 1033, 73
  // /fire7/squeeze/squeeze.0/Conv
  // pragma: 65536, 1027, 74
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435554, &_preg_268435553);
  Zero_ciph(&tmp_result_n19_119);
  Copy_ciph(&input_dup_n19_120, &_preg_268435554);
  for (index_cin_n19_121 = 0; index_cin_n19_121 < 256; index_cin_n19_121 = index_cin_n19_121 + 1) {
    for (index_khw_n19_122 = 0; index_khw_n19_122 < 1; index_khw_n19_122 = index_khw_n19_122 + 1) {
      _pgen_tmp_133_401 = *(PLAIN)Pt_from_msg(&_pgen_tmp_133_401, (index_khw_n19_122 + (index_cin_n19_121 * 1)) + 2081 /* cst_154_0 */, 8192, 1, 2, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_134_402, &input_dup_n19_120, &_pgen_tmp_133_401);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&tmp_result_n19_119, &tmp_result_n19_119, &_pgen_tmp_134_402);
      _pgen_modulus_265 = Q_modulus();
      _pgen_num_q_266 = Poly_level(&tmp_result_n19_119._c0_poly);
      for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_134_402._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&input_dup_n19_120._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_133_401._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_134_402._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&input_dup_n19_120._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_133_401._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&tmp_result_n19_119._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_result_n19_119._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_134_402._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&tmp_result_n19_119._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_result_n19_119._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_134_402._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        _pgen_modulus_265 = _pgen_modulus_265 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435722 = Rotate(input_dup_n19_120, 256);
    Copy_ciph(&input_dup_n19_120, &_preg_268435722);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&tmp_result_n19_119, &tmp_result_n19_119);
  Rescale(&tmp_result_n19_119._c0_poly, &tmp_result_n19_119._c0_poly);
  Rescale(&tmp_result_n19_119._c1_poly, &tmp_result_n19_119._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_135_403 = *(PLAIN)Pt_from_msg(&_pgen_tmp_135_403, 2337 /* cst_155 */, 8192, 1, 1, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&tmp_result_n19_119, &tmp_result_n19_119, &_pgen_tmp_135_403);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&tmp_result_n19_119._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&tmp_result_n19_119._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_result_n19_119._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_135_403._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Set_coeffs(&tmp_result_n19_119._c1_poly, _pgen_rns_idx_267, degree, Coeffs(&tmp_result_n19_119._c1_poly, _pgen_rns_idx_267, degree));
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435555, &tmp_result_n19_119);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 74
  // /fire7/squeeze/squeeze.2/Relu
  // pragma: 65536, 1033, 75
  CMPLR_TM_START(10, 0);
  Bootstrap(&_preg_268435556, &_preg_268435555, 15);
  Encode_float(&_pgen_tmp_137_405, &_cst_272, 1, 1, Level(&_preg_268435556), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_136_404, &_preg_268435556, &_pgen_tmp_137_405);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_136_404._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_136_404._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435556._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_137_405._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_136_404._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435556._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_137_405._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435558, &_pgen_tmp_136_404);
  Rescale(&_preg_268435558._c0_poly, &_pgen_tmp_136_404._c0_poly);
  Rescale(&_preg_268435558._c1_poly, &_pgen_tmp_136_404._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435557 = App_relu(_preg_268435556, _preg_268435558, 8192);
  Copy_ciph(&_preg_268435559, &_preg_268435557);
  CMPLR_TM_END(10, 0);
  // pragma: 65537, 1033, 75
  // /fire7/expand1/expand1.0/Conv
  // pragma: 65536, 1027, 76
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435560, &_preg_268435559);
  // BlockingRot replicate=5
  _preg_268435723 = Rotate(_preg_268435560, -8192);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_138_406, &_preg_268435560, &_preg_268435723);
  CMPLR_TM_END(12, 0);
  _preg_268435724 = Rotate(_preg_268435560, -16384);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_139_407, &_pgen_tmp_138_406, &_preg_268435724);
  CMPLR_TM_END(12, 0);
  _preg_268435725 = Rotate(_preg_268435560, -24576);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_140_408, &_pgen_tmp_139_407, &_preg_268435725);
  CMPLR_TM_END(12, 0);
  _preg_268435726 = Rotate(_preg_268435560, -32768);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&input_dup_n19_123, &_pgen_tmp_140_408, &_preg_268435726);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&input_dup_n19_123._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_138_406._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435560._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435723._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_138_406._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435560._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435723._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_139_407._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_138_406._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435724._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_139_407._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_138_406._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435724._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_140_408._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_139_407._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435725._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_140_408._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_139_407._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435725._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n19_123._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_140_408._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435726._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n19_123._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_140_408._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435726._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  // BlockingRot HRot=bs=4
  _preg_268435727 = Rotate(input_dup_n19_123, 0);
  Copy_ciph(&input_block_n19_124[0], &_preg_268435727);
  for (bs_input_n19_125 = 1; bs_input_n19_125 < 4; bs_input_n19_125 = bs_input_n19_125 + 1) {
    _preg_268435728 = Rotate(input_dup_n19_123, _cst_160[bs_input_n19_125]);
    Copy_ciph(&input_block_n19_124[bs_input_n19_125], &_preg_268435728);
  }
  Zero_ciph(&result_n20_126);
  for (num_grid_n20_127 = 0; num_grid_n20_127 < 8; num_grid_n20_127 = num_grid_n20_127 + 1) {
    Zero_ciph(&result_grid_n20_128);
    for (cap_block_n20_129 = 0; cap_block_n20_129 < 4; cap_block_n20_129 = cap_block_n20_129 + 1) {
      _ckks_gen_tmp_2754_243 = input_block_n19_124[cap_block_n20_129];
      _pgen_tmp_141_409 = *(PLAIN)Pt_from_msg(&_pgen_tmp_141_409, (cap_block_n20_129 + (num_grid_n20_127 * 4)) + 2338 /* cst_158_0 */, 32768, 1, 3, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_142_410, &_ckks_gen_tmp_2754_243, &_pgen_tmp_141_409);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n20_128, &result_grid_n20_128, &_pgen_tmp_142_410);
      _pgen_modulus_265 = Q_modulus();
      _pgen_num_q_266 = Poly_level(&result_grid_n20_128._c0_poly);
      for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_142_410._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_2754_243._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_141_409._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_142_410._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_2754_243._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_141_409._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n20_128._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n20_128._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_142_410._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n20_128._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n20_128._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_142_410._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        _pgen_modulus_265 = _pgen_modulus_265 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435729 = Rotate(result_grid_n20_128, num_grid_n20_127 * 1024);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n20_126, &result_n20_126, &_preg_268435729);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&result_n20_126._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modadd(Coeffs(&result_n20_126._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n20_126._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435729._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&result_n20_126._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n20_126._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435729._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n20_126, &result_n20_126);
  Rescale(&result_n20_126._c0_poly, &result_n20_126._c0_poly);
  Rescale(&result_n20_126._c1_poly, &result_n20_126._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435730 = Rotate(result_n20_126, -32768);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_ckks_gen_tmp_2790_244, &result_n20_126, &_preg_268435730);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_ckks_gen_tmp_2790_244._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_ckks_gen_tmp_2790_244._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n20_126._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435730._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_ckks_gen_tmp_2790_244._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n20_126._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435730._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  _pgen_tmp_144_412 = *(PLAIN)Pt_from_msg(&_pgen_tmp_144_412, 2370 /* cst_161 */, 32768, 1, 2, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_143_411, &_ckks_gen_tmp_2790_244, &_pgen_tmp_144_412);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_143_411._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_143_411._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_2790_244._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_144_412._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_143_411._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_2790_244._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_144_412._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435561, &_pgen_tmp_143_411);
  Rescale(&_preg_268435561._c0_poly, &_pgen_tmp_143_411._c0_poly);
  Rescale(&_preg_268435561._c1_poly, &_pgen_tmp_143_411._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_145_413 = *(PLAIN)Pt_from_msg(&_pgen_tmp_145_413, 2371 /* cst_159 */, 32768, 1, 1, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n20_126, &_preg_268435561, &_pgen_tmp_145_413);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&result_n20_126._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&result_n20_126._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435561._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_145_413._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Set_coeffs(&result_n20_126._c1_poly, _pgen_rns_idx_267, degree, Coeffs(&_preg_268435561._c1_poly, _pgen_rns_idx_267, degree));
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435562, &result_n20_126);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 76
  // /fire7/expand2/expand2.0/Conv
  // pragma: 65536, 1027, 77
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435563, &_preg_268435559);
  // BlockingRot replicate=4
  _preg_268435731 = Rotate(_preg_268435563, -8192);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_146_414, &_preg_268435563, &_preg_268435731);
  CMPLR_TM_END(12, 0);
  _preg_268435732 = Rotate(_preg_268435563, -16384);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_147_415, &_pgen_tmp_146_414, &_preg_268435732);
  CMPLR_TM_END(12, 0);
  _preg_268435733 = Rotate(_preg_268435563, -24576);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&input_dup_n20_130, &_pgen_tmp_147_415, &_preg_268435733);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&input_dup_n20_130._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_146_414._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435563._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435731._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_146_414._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435563._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435731._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_147_415._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_146_414._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435732._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_147_415._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_146_414._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435732._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n20_130._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_147_415._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435733._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n20_130._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_147_415._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435733._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  // BlockingRot HRot=bs=9
  for (bs_input_n20_131 = 0; bs_input_n20_131 < 4; bs_input_n20_131 = bs_input_n20_131 + 1) {
    _preg_268435734 = Rotate(input_dup_n20_130, _cst_165[bs_input_n20_131]);
    Copy_ciph(&input_block_n20_132[bs_input_n20_131], &_preg_268435734);
  }
  _preg_268435735 = Rotate(input_dup_n20_130, 0);
  Copy_ciph(&input_block_n20_132[4], &_preg_268435735);
  for (bs_input_n20_rs2_133 = 5; bs_input_n20_rs2_133 < 9; bs_input_n20_rs2_133 = bs_input_n20_rs2_133 + 1) {
    _preg_268435736 = Rotate(input_dup_n20_130, _cst_165[bs_input_n20_rs2_133]);
    Copy_ciph(&input_block_n20_132[bs_input_n20_rs2_133], &_preg_268435736);
  }
  Zero_ciph(&result_n21_134);
  for (num_grid_n21_135 = 0; num_grid_n21_135 < 32; num_grid_n21_135 = num_grid_n21_135 + 1) {
    Zero_ciph(&result_grid_n21_136);
    for (cap_block_n21_137 = 0; cap_block_n21_137 < 9; cap_block_n21_137 = cap_block_n21_137 + 1) {
      _ckks_gen_tmp_2915_245 = input_block_n20_132[cap_block_n21_137];
      _pgen_tmp_148_416 = *(PLAIN)Pt_from_msg(&_pgen_tmp_148_416, (cap_block_n21_137 + (num_grid_n21_135 * 9)) + 2372 /* cst_163_0 */, 32768, 1, 3, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_149_417, &_ckks_gen_tmp_2915_245, &_pgen_tmp_148_416);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n21_136, &result_grid_n21_136, &_pgen_tmp_149_417);
      _pgen_modulus_265 = Q_modulus();
      _pgen_num_q_266 = Poly_level(&result_grid_n21_136._c0_poly);
      for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_149_417._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_2915_245._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_148_416._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_149_417._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_2915_245._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_148_416._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n21_136._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n21_136._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_149_417._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n21_136._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n21_136._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_149_417._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        _pgen_modulus_265 = _pgen_modulus_265 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435737 = Rotate(result_grid_n21_136, num_grid_n21_135 * 256);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n21_134, &result_n21_134, &_preg_268435737);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&result_n21_134._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modadd(Coeffs(&result_n21_134._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n21_134._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435737._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&result_n21_134._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n21_134._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435737._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n21_134, &result_n21_134);
  Rescale(&result_n21_134._c0_poly, &result_n21_134._c0_poly);
  Rescale(&result_n21_134._c1_poly, &result_n21_134._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435738 = Rotate(result_n21_134, -32768);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_ckks_gen_tmp_2951_246, &result_n21_134, &_preg_268435738);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_ckks_gen_tmp_2951_246._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_ckks_gen_tmp_2951_246._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n21_134._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435738._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_ckks_gen_tmp_2951_246._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n21_134._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435738._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  _pgen_tmp_151_419 = *(PLAIN)Pt_from_msg(&_pgen_tmp_151_419, 2660 /* cst_166 */, 32768, 1, 2, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_150_418, &_ckks_gen_tmp_2951_246, &_pgen_tmp_151_419);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_150_418._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_150_418._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_2951_246._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_151_419._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_150_418._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_2951_246._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_151_419._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435564, &_pgen_tmp_150_418);
  Rescale(&_preg_268435564._c0_poly, &_pgen_tmp_150_418._c0_poly);
  Rescale(&_preg_268435564._c1_poly, &_pgen_tmp_150_418._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_152_420 = *(PLAIN)Pt_from_msg(&_pgen_tmp_152_420, 2661 /* cst_164 */, 32768, 1, 1, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n21_134, &_preg_268435564, &_pgen_tmp_152_420);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&result_n21_134._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&result_n21_134._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435564._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_152_420._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Set_coeffs(&result_n21_134._c1_poly, _pgen_rns_idx_267, degree, Coeffs(&_preg_268435564._c1_poly, _pgen_rns_idx_267, degree));
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435565, &result_n21_134);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 77
  // /fire7/Concat
  // pragma: 65536, 1045, 78
  CMPLR_TM_START(4, 0);
  _preg_268435739 = Rotate(_preg_268435565, -32768);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_preg_268435566, &_preg_268435562, &_preg_268435739);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_preg_268435566._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_preg_268435566._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435562._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435739._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_preg_268435566._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435562._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435739._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  CMPLR_TM_END(4, 0);
  // pragma: 65537, 1045, 78
  // /fire7/Relu
  // pragma: 65536, 1033, 79
  CMPLR_TM_START(10, 0);
  Bootstrap(&_preg_268435567, &_preg_268435566, 14);
  Encode_float(&_pgen_tmp_154_422, &_cst_273, 1, 1, Level(&_preg_268435567), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_153_421, &_preg_268435567, &_pgen_tmp_154_422);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_153_421._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_153_421._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435567._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_154_422._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_153_421._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435567._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_154_422._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435569, &_pgen_tmp_153_421);
  Rescale(&_preg_268435569._c0_poly, &_pgen_tmp_153_421._c0_poly);
  Rescale(&_preg_268435569._c1_poly, &_pgen_tmp_153_421._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435568 = App_relu(_preg_268435567, _preg_268435569, 65536);
  Copy_ciph(&_preg_268435570, &_preg_268435568);
  CMPLR_TM_END(10, 0);
  // pragma: 65537, 1033, 79
  // /fire8/squeeze/squeeze.0/Conv
  // pragma: 65536, 1027, 80
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435571, &_preg_268435570);
  Zero_ciph(&tmp_result_n22_138);
  Copy_ciph(&input_dup_n22_139, &_preg_268435571);
  for (index_cin_n22_140 = 0; index_cin_n22_140 < 256; index_cin_n22_140 = index_cin_n22_140 + 1) {
    for (index_khw_n22_141 = 0; index_khw_n22_141 < 1; index_khw_n22_141 = index_khw_n22_141 + 1) {
      _pgen_tmp_155_423 = *(PLAIN)Pt_from_msg(&_pgen_tmp_155_423, (index_khw_n22_141 + (index_cin_n22_140 * 1)) + 2662 /* cst_168_0 */, 16384, 1, 2, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_156_424, &input_dup_n22_139, &_pgen_tmp_155_423);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&tmp_result_n22_138, &tmp_result_n22_138, &_pgen_tmp_156_424);
      _pgen_modulus_265 = Q_modulus();
      _pgen_num_q_266 = Poly_level(&tmp_result_n22_138._c0_poly);
      for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_156_424._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&input_dup_n22_139._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_155_423._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_156_424._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&input_dup_n22_139._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_155_423._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&tmp_result_n22_138._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_result_n22_138._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_156_424._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&tmp_result_n22_138._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_result_n22_138._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_156_424._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        _pgen_modulus_265 = _pgen_modulus_265 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435740 = Rotate(input_dup_n22_139, 256);
    Copy_ciph(&input_dup_n22_139, &_preg_268435740);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&tmp_result_n22_138, &tmp_result_n22_138);
  Rescale(&tmp_result_n22_138._c0_poly, &tmp_result_n22_138._c0_poly);
  Rescale(&tmp_result_n22_138._c1_poly, &tmp_result_n22_138._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_157_425 = *(PLAIN)Pt_from_msg(&_pgen_tmp_157_425, 2918 /* cst_169 */, 16384, 1, 1, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&tmp_result_n22_138, &tmp_result_n22_138, &_pgen_tmp_157_425);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&tmp_result_n22_138._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&tmp_result_n22_138._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_result_n22_138._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_157_425._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Set_coeffs(&tmp_result_n22_138._c1_poly, _pgen_rns_idx_267, degree, Coeffs(&tmp_result_n22_138._c1_poly, _pgen_rns_idx_267, degree));
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435572, &tmp_result_n22_138);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 80
  // /fire8/squeeze/squeeze.2/Relu
  // pragma: 65536, 1033, 81
  CMPLR_TM_START(10, 0);
  Bootstrap(&_preg_268435573, &_preg_268435572, 19);
  Encode_float(&_pgen_tmp_159_427, &_cst_274, 1, 1, Level(&_preg_268435573), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_158_426, &_preg_268435573, &_pgen_tmp_159_427);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_158_426._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_158_426._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435573._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_159_427._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_158_426._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435573._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_159_427._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435575, &_pgen_tmp_158_426);
  Rescale(&_preg_268435575._c0_poly, &_pgen_tmp_158_426._c0_poly);
  Rescale(&_preg_268435575._c1_poly, &_pgen_tmp_158_426._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435574 = App_relu(_preg_268435573, _preg_268435575, 16384);
  Copy_ciph(&_preg_268435576, &_preg_268435574);
  CMPLR_TM_END(10, 0);
  // pragma: 65537, 1033, 81
  // /fire8/expand1/expand1.0/Conv
  // pragma: 65536, 1027, 82
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435577, &_preg_268435576);
  // BlockingRot replicate=5
  _preg_268435741 = Rotate(_preg_268435577, -16384);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_160_428, &_preg_268435577, &_preg_268435741);
  CMPLR_TM_END(12, 0);
  _preg_268435742 = Rotate(_preg_268435577, -32768);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_161_429, &_pgen_tmp_160_428, &_preg_268435742);
  CMPLR_TM_END(12, 0);
  _preg_268435743 = Rotate(_preg_268435577, -49152);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_162_430, &_pgen_tmp_161_429, &_preg_268435743);
  CMPLR_TM_END(12, 0);
  _preg_268435744 = Rotate(_preg_268435577, -65536);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&input_dup_n22_142, &_pgen_tmp_162_430, &_preg_268435744);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&input_dup_n22_142._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_160_428._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435577._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435741._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_160_428._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435577._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435741._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_161_429._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_160_428._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435742._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_161_429._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_160_428._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435742._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_162_430._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_161_429._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435743._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_162_430._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_161_429._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435743._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n22_142._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_162_430._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435744._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n22_142._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_162_430._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435744._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  // BlockingRot HRot=bs=8
  _preg_268435745 = Rotate(input_dup_n22_142, 0);
  Copy_ciph(&input_block_n22_143[0], &_preg_268435745);
  for (bs_input_n22_144 = 1; bs_input_n22_144 < 8; bs_input_n22_144 = bs_input_n22_144 + 1) {
    _preg_268435746 = Rotate(input_dup_n22_142, _cst_174[bs_input_n22_144]);
    Copy_ciph(&input_block_n22_143[bs_input_n22_144], &_preg_268435746);
  }
  Zero_ciph(&result_n23_145);
  for (num_grid_n23_146 = 0; num_grid_n23_146 < 8; num_grid_n23_146 = num_grid_n23_146 + 1) {
    Zero_ciph(&result_grid_n23_147);
    for (cap_block_n23_148 = 0; cap_block_n23_148 < 8; cap_block_n23_148 = cap_block_n23_148 + 1) {
      _ckks_gen_tmp_3191_247 = input_block_n22_143[cap_block_n23_148];
      _pgen_tmp_163_431 = *(PLAIN)Pt_from_msg(&_pgen_tmp_163_431, (cap_block_n23_148 + (num_grid_n23_146 * 8)) + 2919 /* cst_172_0 */, 65536, 1, 7, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_164_432, &_ckks_gen_tmp_3191_247, &_pgen_tmp_163_431);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n23_147, &result_grid_n23_147, &_pgen_tmp_164_432);
      _pgen_modulus_265 = Q_modulus();
      _pgen_num_q_266 = Poly_level(&result_grid_n23_147._c0_poly);
      for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_164_432._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_3191_247._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_163_431._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_164_432._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_3191_247._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_163_431._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n23_147._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n23_147._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_164_432._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n23_147._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n23_147._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_164_432._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        _pgen_modulus_265 = _pgen_modulus_265 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435747 = Rotate(result_grid_n23_147, num_grid_n23_146 * 2048);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n23_145, &result_n23_145, &_preg_268435747);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&result_n23_145._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modadd(Coeffs(&result_n23_145._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n23_145._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435747._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&result_n23_145._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n23_145._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435747._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n23_145, &result_n23_145);
  Rescale(&result_n23_145._c0_poly, &result_n23_145._c0_poly);
  Rescale(&result_n23_145._c1_poly, &result_n23_145._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_166_434 = *(PLAIN)Pt_from_msg(&_pgen_tmp_166_434, 2983 /* cst_175 */, 65536, 1, 6, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_165_433, &result_n23_145, &_pgen_tmp_166_434);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_165_433._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_165_433._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n23_145._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_166_434._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_165_433._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n23_145._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_166_434._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435578, &_pgen_tmp_165_433);
  Rescale(&_preg_268435578._c0_poly, &_pgen_tmp_165_433._c0_poly);
  Rescale(&_preg_268435578._c1_poly, &_pgen_tmp_165_433._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_167_435 = *(PLAIN)Pt_from_msg(&_pgen_tmp_167_435, 2984 /* cst_173 */, 65536, 1, 5, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n23_145, &_preg_268435578, &_pgen_tmp_167_435);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&result_n23_145._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&result_n23_145._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435578._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_167_435._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Set_coeffs(&result_n23_145._c1_poly, _pgen_rns_idx_267, degree, Coeffs(&_preg_268435578._c1_poly, _pgen_rns_idx_267, degree));
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435579, &result_n23_145);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 82
  // /fire8/expand1/expand1.2/AveragePool
  // pragma: 65536, 1026, 83
  CMPLR_TM_START(3, 0);
  Copy_ciph(&_preg_268435580, &_preg_268435579);
  _preg_268435748 = Rotate(_preg_268435580, 16);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&tmp_row_add23_149, &_preg_268435580, &_preg_268435748);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&tmp_row_add23_149._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&tmp_row_add23_149._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435580._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435748._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&tmp_row_add23_149._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435580._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435748._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  _preg_268435749 = Rotate(tmp_row_add23_149, 1);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_ckks_gen_tmp_3268_248, &tmp_row_add23_149, &_preg_268435749);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_ckks_gen_tmp_3268_248._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_ckks_gen_tmp_3268_248._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_row_add23_149._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435749._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_ckks_gen_tmp_3268_248._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_row_add23_149._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435749._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  _pgen_tmp_169_437 = *(PLAIN)Pt_from_msg(&_pgen_tmp_169_437, 2985 /* cst_176 */, 65536, 1, 5, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_168_436, &_ckks_gen_tmp_3268_248, &_pgen_tmp_169_437);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_168_436._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_168_436._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_3268_248._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_169_437._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_168_436._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_3268_248._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_169_437._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435581, &_pgen_tmp_168_436);
  Rescale(&_preg_268435581._c0_poly, &_pgen_tmp_168_436._c0_poly);
  Rescale(&_preg_268435581._c1_poly, &_pgen_tmp_168_436._c1_poly);
  CMPLR_TM_END(19, 0);
  Copy_ciph(&orig_result23_150, &_preg_268435581);
  Zero_ciph(&comb_row_result24_151);
  for (comb_row_index_n24_152 = 0; comb_row_index_n24_152 < 8; comb_row_index_n24_152 = comb_row_index_n24_152 + 1) {
    _preg_268435750 = Rotate(orig_result23_150, comb_row_index_n24_152 * 1);
    Copy_ciph(&_ckks_gen_tmp_3300_249, &_preg_268435750);
    _pgen_tmp_170_438 = *(PLAIN)Pt_from_msg(&_pgen_tmp_170_438, comb_row_index_n24_152 + 2986 /* cst_177_0 */, 65536, 1, 4, 0);
    CMPLR_TM_START(17, 0);
    Init_ciph_up_scale_plain(&_pgen_tmp_171_439, &_ckks_gen_tmp_3300_249, &_pgen_tmp_170_438);
    CMPLR_TM_END(17, 0);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&comb_row_result24_151, &comb_row_result24_151, &_pgen_tmp_171_439);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&comb_row_result24_151._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_171_439._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_3300_249._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_170_438._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_171_439._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_3300_249._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_170_438._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&comb_row_result24_151._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&comb_row_result24_151._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_171_439._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&comb_row_result24_151._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&comb_row_result24_151._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_171_439._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&comb_row_result24_151, &comb_row_result24_151);
  Rescale(&comb_row_result24_151._c0_poly, &comb_row_result24_151._c0_poly);
  Rescale(&comb_row_result24_151._c1_poly, &comb_row_result24_151._c1_poly);
  CMPLR_TM_END(19, 0);
  Zero_ciph(&comb_rc_result24_153);
  for (comb_rc_index_n24_154 = 0; comb_rc_index_n24_154 < 8; comb_rc_index_n24_154 = comb_rc_index_n24_154 + 1) {
    _preg_268435751 = Rotate(comb_row_result24_151, comb_rc_index_n24_154 * 24);
    Copy_ciph(&_ckks_gen_tmp_3334_250, &_preg_268435751);
    _pgen_tmp_172_440 = *(PLAIN)Pt_from_msg(&_pgen_tmp_172_440, comb_rc_index_n24_154 + 2994 /* cst_178_0 */, 65536, 1, 3, 0);
    CMPLR_TM_START(17, 0);
    Init_ciph_up_scale_plain(&_pgen_tmp_173_441, &_ckks_gen_tmp_3334_250, &_pgen_tmp_172_440);
    CMPLR_TM_END(17, 0);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&comb_rc_result24_153, &comb_rc_result24_153, &_pgen_tmp_173_441);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&comb_rc_result24_153._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_173_441._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_3334_250._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_172_440._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_173_441._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_3334_250._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_172_440._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&comb_rc_result24_153._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&comb_rc_result24_153._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_173_441._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&comb_rc_result24_153._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&comb_rc_result24_153._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_173_441._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&comb_rc_result24_153, &comb_rc_result24_153);
  Rescale(&comb_rc_result24_153._c0_poly, &comb_rc_result24_153._c0_poly);
  Rescale(&comb_rc_result24_153._c1_poly, &comb_rc_result24_153._c1_poly);
  CMPLR_TM_END(19, 0);
  Zero_ciph(&ss_result24_155);
  for (comb_cc_index_n24_156 = 0; comb_cc_index_n24_156 < 256; comb_cc_index_n24_156 = comb_cc_index_n24_156 + 1) {
    _preg_268435752 = Rotate(comb_rc_result24_153, comb_cc_index_n24_156 * 192);
    Copy_ciph(&_ckks_gen_tmp_3368_251, &_preg_268435752);
    _pgen_tmp_174_442 = *(PLAIN)Pt_from_msg(&_pgen_tmp_174_442, comb_cc_index_n24_156 + 3002 /* cst_179_0 */, 65536, 1, 2, 0);
    CMPLR_TM_START(17, 0);
    Init_ciph_up_scale_plain(&_pgen_tmp_175_443, &_ckks_gen_tmp_3368_251, &_pgen_tmp_174_442);
    CMPLR_TM_END(17, 0);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&ss_result24_155, &ss_result24_155, &_pgen_tmp_175_443);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&ss_result24_155._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_175_443._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_3368_251._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_174_442._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_175_443._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_3368_251._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_174_442._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&ss_result24_155._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&ss_result24_155._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_175_443._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&ss_result24_155._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&ss_result24_155._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_175_443._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&ss_result24_155, &ss_result24_155);
  Rescale(&ss_result24_155._c0_poly, &ss_result24_155._c0_poly);
  Rescale(&ss_result24_155._c1_poly, &ss_result24_155._c1_poly);
  CMPLR_TM_END(19, 0);
  Copy_ciph(&_preg_268435582, &ss_result24_155);
  CMPLR_TM_END(3, 0);
  // pragma: 65537, 1026, 83
  // /fire8/expand2/expand2.0/Conv
  // pragma: 65536, 1027, 84
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435583, &_preg_268435576);
  // BlockingRot replicate=4
  _preg_268435753 = Rotate(_preg_268435583, -16384);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_176_444, &_preg_268435583, &_preg_268435753);
  CMPLR_TM_END(12, 0);
  _preg_268435754 = Rotate(_preg_268435583, -32768);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_177_445, &_pgen_tmp_176_444, &_preg_268435754);
  CMPLR_TM_END(12, 0);
  _preg_268435755 = Rotate(_preg_268435583, -49152);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&input_dup_n24_157, &_pgen_tmp_177_445, &_preg_268435755);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&input_dup_n24_157._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_176_444._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435583._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435753._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_176_444._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435583._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435753._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_177_445._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_176_444._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435754._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_177_445._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_176_444._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435754._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n24_157._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_177_445._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435755._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n24_157._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_177_445._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435755._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  // BlockingRot HRot=bs=9
  for (bs_input_n24_158 = 0; bs_input_n24_158 < 4; bs_input_n24_158 = bs_input_n24_158 + 1) {
    _preg_268435756 = Rotate(input_dup_n24_157, _cst_183[bs_input_n24_158]);
    Copy_ciph(&input_block_n24_159[bs_input_n24_158], &_preg_268435756);
  }
  _preg_268435757 = Rotate(input_dup_n24_157, 0);
  Copy_ciph(&input_block_n24_159[4], &_preg_268435757);
  for (bs_input_n24_rs2_160 = 5; bs_input_n24_rs2_160 < 9; bs_input_n24_rs2_160 = bs_input_n24_rs2_160 + 1) {
    _preg_268435758 = Rotate(input_dup_n24_157, _cst_183[bs_input_n24_rs2_160]);
    Copy_ciph(&input_block_n24_159[bs_input_n24_rs2_160], &_preg_268435758);
  }
  Zero_ciph(&result_n25_161);
  for (num_grid_n25_162 = 0; num_grid_n25_162 < 64; num_grid_n25_162 = num_grid_n25_162 + 1) {
    Zero_ciph(&result_grid_n25_163);
    for (cap_block_n25_164 = 0; cap_block_n25_164 < 9; cap_block_n25_164 = cap_block_n25_164 + 1) {
      _ckks_gen_tmp_3486_252 = input_block_n24_159[cap_block_n25_164];
      _pgen_tmp_178_446 = *(PLAIN)Pt_from_msg(&_pgen_tmp_178_446, (cap_block_n25_164 + (num_grid_n25_162 * 9)) + 3258 /* cst_181_0 */, 65536, 1, 7, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_179_447, &_ckks_gen_tmp_3486_252, &_pgen_tmp_178_446);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n25_163, &result_grid_n25_163, &_pgen_tmp_179_447);
      _pgen_modulus_265 = Q_modulus();
      _pgen_num_q_266 = Poly_level(&result_grid_n25_163._c0_poly);
      for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_179_447._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_3486_252._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_178_446._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_179_447._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_3486_252._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_178_446._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n25_163._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n25_163._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_179_447._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n25_163._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n25_163._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_179_447._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        _pgen_modulus_265 = _pgen_modulus_265 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435759 = Rotate(result_grid_n25_163, num_grid_n25_162 * 256);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n25_161, &result_n25_161, &_preg_268435759);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&result_n25_161._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modadd(Coeffs(&result_n25_161._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n25_161._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435759._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&result_n25_161._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n25_161._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435759._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n25_161, &result_n25_161);
  Rescale(&result_n25_161._c0_poly, &result_n25_161._c0_poly);
  Rescale(&result_n25_161._c1_poly, &result_n25_161._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_181_449 = *(PLAIN)Pt_from_msg(&_pgen_tmp_181_449, 3834 /* cst_184 */, 65536, 1, 6, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_180_448, &result_n25_161, &_pgen_tmp_181_449);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_180_448._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_180_448._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n25_161._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_181_449._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_180_448._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n25_161._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_181_449._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435584, &_pgen_tmp_180_448);
  Rescale(&_preg_268435584._c0_poly, &_pgen_tmp_180_448._c0_poly);
  Rescale(&_preg_268435584._c1_poly, &_pgen_tmp_180_448._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_182_450 = *(PLAIN)Pt_from_msg(&_pgen_tmp_182_450, 3835 /* cst_182 */, 65536, 1, 5, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n25_161, &_preg_268435584, &_pgen_tmp_182_450);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&result_n25_161._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&result_n25_161._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435584._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_182_450._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Set_coeffs(&result_n25_161._c1_poly, _pgen_rns_idx_267, degree, Coeffs(&_preg_268435584._c1_poly, _pgen_rns_idx_267, degree));
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435585, &result_n25_161);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 84
  // /fire8/expand2/expand2.2/AveragePool
  // pragma: 65536, 1026, 85
  CMPLR_TM_START(3, 0);
  Copy_ciph(&_preg_268435586, &_preg_268435585);
  _preg_268435760 = Rotate(_preg_268435586, 16);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&tmp_row_add25_165, &_preg_268435586, &_preg_268435760);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&tmp_row_add25_165._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&tmp_row_add25_165._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435586._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435760._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&tmp_row_add25_165._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435586._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435760._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  _preg_268435761 = Rotate(tmp_row_add25_165, 1);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_ckks_gen_tmp_3563_253, &tmp_row_add25_165, &_preg_268435761);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_ckks_gen_tmp_3563_253._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_ckks_gen_tmp_3563_253._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_row_add25_165._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435761._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_ckks_gen_tmp_3563_253._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_row_add25_165._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435761._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  _pgen_tmp_184_452 = *(PLAIN)Pt_from_msg(&_pgen_tmp_184_452, 3836 /* cst_185 */, 65536, 1, 5, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_183_451, &_ckks_gen_tmp_3563_253, &_pgen_tmp_184_452);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_183_451._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_183_451._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_3563_253._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_184_452._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_183_451._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_3563_253._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_184_452._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435587, &_pgen_tmp_183_451);
  Rescale(&_preg_268435587._c0_poly, &_pgen_tmp_183_451._c0_poly);
  Rescale(&_preg_268435587._c1_poly, &_pgen_tmp_183_451._c1_poly);
  CMPLR_TM_END(19, 0);
  Copy_ciph(&orig_result25_166, &_preg_268435587);
  Zero_ciph(&comb_row_result26_167);
  for (comb_row_index_n26_168 = 0; comb_row_index_n26_168 < 8; comb_row_index_n26_168 = comb_row_index_n26_168 + 1) {
    _preg_268435762 = Rotate(orig_result25_166, comb_row_index_n26_168 * 1);
    Copy_ciph(&_ckks_gen_tmp_3595_254, &_preg_268435762);
    _pgen_tmp_185_453 = *(PLAIN)Pt_from_msg(&_pgen_tmp_185_453, comb_row_index_n26_168 + 3837 /* cst_186_0 */, 65536, 1, 4, 0);
    CMPLR_TM_START(17, 0);
    Init_ciph_up_scale_plain(&_pgen_tmp_186_454, &_ckks_gen_tmp_3595_254, &_pgen_tmp_185_453);
    CMPLR_TM_END(17, 0);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&comb_row_result26_167, &comb_row_result26_167, &_pgen_tmp_186_454);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&comb_row_result26_167._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_186_454._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_3595_254._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_185_453._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_186_454._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_3595_254._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_185_453._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&comb_row_result26_167._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&comb_row_result26_167._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_186_454._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&comb_row_result26_167._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&comb_row_result26_167._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_186_454._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&comb_row_result26_167, &comb_row_result26_167);
  Rescale(&comb_row_result26_167._c0_poly, &comb_row_result26_167._c0_poly);
  Rescale(&comb_row_result26_167._c1_poly, &comb_row_result26_167._c1_poly);
  CMPLR_TM_END(19, 0);
  Zero_ciph(&comb_rc_result26_169);
  for (comb_rc_index_n26_170 = 0; comb_rc_index_n26_170 < 8; comb_rc_index_n26_170 = comb_rc_index_n26_170 + 1) {
    _preg_268435763 = Rotate(comb_row_result26_167, comb_rc_index_n26_170 * 24);
    Copy_ciph(&_ckks_gen_tmp_3629_255, &_preg_268435763);
    _pgen_tmp_187_455 = *(PLAIN)Pt_from_msg(&_pgen_tmp_187_455, comb_rc_index_n26_170 + 3845 /* cst_187_0 */, 65536, 1, 3, 0);
    CMPLR_TM_START(17, 0);
    Init_ciph_up_scale_plain(&_pgen_tmp_188_456, &_ckks_gen_tmp_3629_255, &_pgen_tmp_187_455);
    CMPLR_TM_END(17, 0);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&comb_rc_result26_169, &comb_rc_result26_169, &_pgen_tmp_188_456);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&comb_rc_result26_169._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_188_456._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_3629_255._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_187_455._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_188_456._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_3629_255._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_187_455._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&comb_rc_result26_169._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&comb_rc_result26_169._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_188_456._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&comb_rc_result26_169._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&comb_rc_result26_169._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_188_456._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&comb_rc_result26_169, &comb_rc_result26_169);
  Rescale(&comb_rc_result26_169._c0_poly, &comb_rc_result26_169._c0_poly);
  Rescale(&comb_rc_result26_169._c1_poly, &comb_rc_result26_169._c1_poly);
  CMPLR_TM_END(19, 0);
  Zero_ciph(&ss_result26_171);
  for (comb_cc_index_n26_172 = 0; comb_cc_index_n26_172 < 256; comb_cc_index_n26_172 = comb_cc_index_n26_172 + 1) {
    _preg_268435764 = Rotate(comb_rc_result26_169, comb_cc_index_n26_172 * 192);
    Copy_ciph(&_ckks_gen_tmp_3663_256, &_preg_268435764);
    _pgen_tmp_189_457 = *(PLAIN)Pt_from_msg(&_pgen_tmp_189_457, comb_cc_index_n26_172 + 3853 /* cst_188_0 */, 65536, 1, 2, 0);
    CMPLR_TM_START(17, 0);
    Init_ciph_up_scale_plain(&_pgen_tmp_190_458, &_ckks_gen_tmp_3663_256, &_pgen_tmp_189_457);
    CMPLR_TM_END(17, 0);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&ss_result26_171, &ss_result26_171, &_pgen_tmp_190_458);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&ss_result26_171._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_190_458._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_3663_256._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_189_457._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_190_458._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_3663_256._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_189_457._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&ss_result26_171._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&ss_result26_171._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_190_458._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&ss_result26_171._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&ss_result26_171._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_190_458._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&ss_result26_171, &ss_result26_171);
  Rescale(&ss_result26_171._c0_poly, &ss_result26_171._c0_poly);
  Rescale(&ss_result26_171._c1_poly, &ss_result26_171._c1_poly);
  CMPLR_TM_END(19, 0);
  Copy_ciph(&_preg_268435588, &ss_result26_171);
  CMPLR_TM_END(3, 0);
  // pragma: 65537, 1026, 85
  // /fire8/Concat
  // pragma: 65536, 1045, 86
  CMPLR_TM_START(4, 0);
  _preg_268435765 = Rotate(_preg_268435588, -16384);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_preg_268435589, &_preg_268435582, &_preg_268435765);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_preg_268435589._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_preg_268435589._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435582._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435765._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_preg_268435589._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435582._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435765._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  CMPLR_TM_END(4, 0);
  // pragma: 65537, 1045, 86
  // /fire8/Relu
  // pragma: 65536, 1033, 87
  CMPLR_TM_START(10, 0);
  Bootstrap(&_preg_268435590, &_preg_268435589, 14);
  Encode_float(&_pgen_tmp_192_460, &_cst_275, 1, 1, Level(&_preg_268435590), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_191_459, &_preg_268435590, &_pgen_tmp_192_460);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_191_459._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_191_459._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435590._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_192_460._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_191_459._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435590._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_192_460._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435592, &_pgen_tmp_191_459);
  Rescale(&_preg_268435592._c0_poly, &_pgen_tmp_191_459._c0_poly);
  Rescale(&_preg_268435592._c1_poly, &_pgen_tmp_191_459._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435591 = App_relu(_preg_268435590, _preg_268435592, 32768);
  Copy_ciph(&_preg_268435593, &_preg_268435591);
  CMPLR_TM_END(10, 0);
  // pragma: 65537, 1033, 87
  // /fire9/squeeze/squeeze.0/Conv
  // pragma: 65536, 1027, 88
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435594, &_preg_268435593);
  Zero_ciph(&tmp_result_n27_173);
  _preg_268435766 = Rotate(_preg_268435594, -32768);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&input_dup_n27_174, &_preg_268435594, &_preg_268435766);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&input_dup_n27_174._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&input_dup_n27_174._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435594._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435766._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n27_174._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435594._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435766._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  for (index_cin_n27_175 = 0; index_cin_n27_175 < 512; index_cin_n27_175 = index_cin_n27_175 + 1) {
    for (index_khw_n27_176 = 0; index_khw_n27_176 < 1; index_khw_n27_176 = index_khw_n27_176 + 1) {
      _pgen_tmp_193_461 = *(PLAIN)Pt_from_msg(&_pgen_tmp_193_461, (index_khw_n27_176 + (index_cin_n27_175 * 1)) + 4109 /* cst_190_0 */, 4096, 1, 2, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_194_462, &input_dup_n27_174, &_pgen_tmp_193_461);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&tmp_result_n27_173, &tmp_result_n27_173, &_pgen_tmp_194_462);
      _pgen_modulus_265 = Q_modulus();
      _pgen_num_q_266 = Poly_level(&tmp_result_n27_173._c0_poly);
      for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_194_462._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&input_dup_n27_174._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_193_461._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_194_462._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&input_dup_n27_174._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_193_461._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&tmp_result_n27_173._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_result_n27_173._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_194_462._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&tmp_result_n27_173._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_result_n27_173._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_194_462._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        _pgen_modulus_265 = _pgen_modulus_265 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435767 = Rotate(input_dup_n27_174, 64);
    Copy_ciph(&input_dup_n27_174, &_preg_268435767);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&tmp_result_n27_173, &tmp_result_n27_173);
  Rescale(&tmp_result_n27_173._c0_poly, &tmp_result_n27_173._c0_poly);
  Rescale(&tmp_result_n27_173._c1_poly, &tmp_result_n27_173._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_195_463 = *(PLAIN)Pt_from_msg(&_pgen_tmp_195_463, 4621 /* cst_191 */, 4096, 1, 1, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&tmp_result_n27_173, &tmp_result_n27_173, &_pgen_tmp_195_463);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&tmp_result_n27_173._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&tmp_result_n27_173._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&tmp_result_n27_173._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_195_463._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Set_coeffs(&tmp_result_n27_173._c1_poly, _pgen_rns_idx_267, degree, Coeffs(&tmp_result_n27_173._c1_poly, _pgen_rns_idx_267, degree));
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435595, &tmp_result_n27_173);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 88
  // /fire9/squeeze/squeeze.2/Relu
  // pragma: 65536, 1033, 89
  CMPLR_TM_START(10, 0);
  Bootstrap(&_preg_268435596, &_preg_268435595, 15);
  Encode_float(&_pgen_tmp_197_465, &_cst_276, 1, 1, Level(&_preg_268435596), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_196_464, &_preg_268435596, &_pgen_tmp_197_465);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_196_464._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_196_464._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435596._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_197_465._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_196_464._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435596._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_197_465._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435598, &_pgen_tmp_196_464);
  Rescale(&_preg_268435598._c0_poly, &_pgen_tmp_196_464._c0_poly);
  Rescale(&_preg_268435598._c1_poly, &_pgen_tmp_196_464._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435597 = App_relu(_preg_268435596, _preg_268435598, 4096);
  Copy_ciph(&_preg_268435599, &_preg_268435597);
  CMPLR_TM_END(10, 0);
  // pragma: 65537, 1033, 89
  // /fire9/expand1/expand1.0/Conv
  // pragma: 65536, 1027, 90
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435600, &_preg_268435599);
  // BlockingRot replicate=5
  _preg_268435768 = Rotate(_preg_268435600, -4096);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_198_466, &_preg_268435600, &_preg_268435768);
  CMPLR_TM_END(12, 0);
  _preg_268435769 = Rotate(_preg_268435600, -8192);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_199_467, &_pgen_tmp_198_466, &_preg_268435769);
  CMPLR_TM_END(12, 0);
  _preg_268435770 = Rotate(_preg_268435600, -12288);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_200_468, &_pgen_tmp_199_467, &_preg_268435770);
  CMPLR_TM_END(12, 0);
  _preg_268435771 = Rotate(_preg_268435600, -16384);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&input_dup_n27_177, &_pgen_tmp_200_468, &_preg_268435771);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&input_dup_n27_177._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_198_466._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435600._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435768._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_198_466._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435600._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435768._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_199_467._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_198_466._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435769._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_199_467._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_198_466._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435769._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_200_468._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_199_467._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435770._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_200_468._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_199_467._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435770._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n27_177._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_200_468._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435771._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n27_177._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_200_468._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435771._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  // BlockingRot HRot=bs=8
  _preg_268435772 = Rotate(input_dup_n27_177, 0);
  Copy_ciph(&input_block_n27_178[0], &_preg_268435772);
  for (bs_input_n27_179 = 1; bs_input_n27_179 < 8; bs_input_n27_179 = bs_input_n27_179 + 1) {
    _preg_268435773 = Rotate(input_dup_n27_177, _cst_196[bs_input_n27_179]);
    Copy_ciph(&input_block_n27_178[bs_input_n27_179], &_preg_268435773);
  }
  Zero_ciph(&result_n28_180);
  for (num_grid_n28_181 = 0; num_grid_n28_181 < 8; num_grid_n28_181 = num_grid_n28_181 + 1) {
    Zero_ciph(&result_grid_n28_182);
    for (cap_block_n28_183 = 0; cap_block_n28_183 < 8; cap_block_n28_183 = cap_block_n28_183 + 1) {
      _ckks_gen_tmp_3900_257 = input_block_n27_178[cap_block_n28_183];
      _pgen_tmp_201_469 = *(PLAIN)Pt_from_msg(&_pgen_tmp_201_469, (cap_block_n28_183 + (num_grid_n28_181 * 8)) + 4622 /* cst_194_0 */, 16384, 1, 3, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_202_470, &_ckks_gen_tmp_3900_257, &_pgen_tmp_201_469);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n28_182, &result_grid_n28_182, &_pgen_tmp_202_470);
      _pgen_modulus_265 = Q_modulus();
      _pgen_num_q_266 = Poly_level(&result_grid_n28_182._c0_poly);
      for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_202_470._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_3900_257._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_201_469._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_202_470._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_3900_257._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_201_469._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n28_182._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n28_182._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_202_470._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n28_182._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n28_182._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_202_470._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        _pgen_modulus_265 = _pgen_modulus_265 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435774 = Rotate(result_grid_n28_182, num_grid_n28_181 * 512);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n28_180, &result_n28_180, &_preg_268435774);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&result_n28_180._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modadd(Coeffs(&result_n28_180._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n28_180._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435774._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&result_n28_180._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n28_180._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435774._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n28_180, &result_n28_180);
  Rescale(&result_n28_180._c0_poly, &result_n28_180._c0_poly);
  Rescale(&result_n28_180._c1_poly, &result_n28_180._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435775 = Rotate(result_n28_180, -16384);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_ckks_gen_tmp_3936_258, &result_n28_180, &_preg_268435775);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_ckks_gen_tmp_3936_258._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_ckks_gen_tmp_3936_258._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n28_180._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435775._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_ckks_gen_tmp_3936_258._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n28_180._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435775._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  _pgen_tmp_204_472 = *(PLAIN)Pt_from_msg(&_pgen_tmp_204_472, 4686 /* cst_197 */, 16384, 1, 2, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_203_471, &_ckks_gen_tmp_3936_258, &_pgen_tmp_204_472);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_203_471._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_203_471._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_3936_258._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_204_472._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_203_471._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_3936_258._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_204_472._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435601, &_pgen_tmp_203_471);
  Rescale(&_preg_268435601._c0_poly, &_pgen_tmp_203_471._c0_poly);
  Rescale(&_preg_268435601._c1_poly, &_pgen_tmp_203_471._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_205_473 = *(PLAIN)Pt_from_msg(&_pgen_tmp_205_473, 4687 /* cst_195 */, 16384, 1, 1, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n28_180, &_preg_268435601, &_pgen_tmp_205_473);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&result_n28_180._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&result_n28_180._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435601._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_205_473._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Set_coeffs(&result_n28_180._c1_poly, _pgen_rns_idx_267, degree, Coeffs(&_preg_268435601._c1_poly, _pgen_rns_idx_267, degree));
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435602, &result_n28_180);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 90
  // /fire9/expand2/expand2.0/Conv
  // pragma: 65536, 1027, 91
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435603, &_preg_268435599);
  // BlockingRot replicate=4
  _preg_268435776 = Rotate(_preg_268435603, -4096);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_206_474, &_preg_268435603, &_preg_268435776);
  CMPLR_TM_END(12, 0);
  _preg_268435777 = Rotate(_preg_268435603, -8192);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_207_475, &_pgen_tmp_206_474, &_preg_268435777);
  CMPLR_TM_END(12, 0);
  _preg_268435778 = Rotate(_preg_268435603, -12288);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&input_dup_n28_184, &_pgen_tmp_207_475, &_preg_268435778);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&input_dup_n28_184._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_206_474._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435603._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435776._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_206_474._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435603._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435776._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_207_475._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_206_474._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435777._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_207_475._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_206_474._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435777._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n28_184._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_207_475._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435778._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n28_184._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_207_475._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435778._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  // BlockingRot HRot=bs=9
  for (bs_input_n28_185 = 0; bs_input_n28_185 < 4; bs_input_n28_185 = bs_input_n28_185 + 1) {
    _preg_268435779 = Rotate(input_dup_n28_184, _cst_201[bs_input_n28_185]);
    Copy_ciph(&input_block_n28_186[bs_input_n28_185], &_preg_268435779);
  }
  _preg_268435780 = Rotate(input_dup_n28_184, 0);
  Copy_ciph(&input_block_n28_186[4], &_preg_268435780);
  for (bs_input_n28_rs2_187 = 5; bs_input_n28_rs2_187 < 9; bs_input_n28_rs2_187 = bs_input_n28_rs2_187 + 1) {
    _preg_268435781 = Rotate(input_dup_n28_184, _cst_201[bs_input_n28_rs2_187]);
    Copy_ciph(&input_block_n28_186[bs_input_n28_rs2_187], &_preg_268435781);
  }
  Zero_ciph(&result_n29_188);
  for (num_grid_n29_189 = 0; num_grid_n29_189 < 64; num_grid_n29_189 = num_grid_n29_189 + 1) {
    Zero_ciph(&result_grid_n29_190);
    for (cap_block_n29_191 = 0; cap_block_n29_191 < 9; cap_block_n29_191 = cap_block_n29_191 + 1) {
      _ckks_gen_tmp_4061_259 = input_block_n28_186[cap_block_n29_191];
      _pgen_tmp_208_476 = *(PLAIN)Pt_from_msg(&_pgen_tmp_208_476, (cap_block_n29_191 + (num_grid_n29_189 * 9)) + 4688 /* cst_199_0 */, 16384, 1, 3, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_209_477, &_ckks_gen_tmp_4061_259, &_pgen_tmp_208_476);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n29_190, &result_grid_n29_190, &_pgen_tmp_209_477);
      _pgen_modulus_265 = Q_modulus();
      _pgen_num_q_266 = Poly_level(&result_grid_n29_190._c0_poly);
      for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_209_477._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_4061_259._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_208_476._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_209_477._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_4061_259._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_208_476._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n29_190._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n29_190._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_209_477._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n29_190._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n29_190._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_209_477._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        _pgen_modulus_265 = _pgen_modulus_265 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435782 = Rotate(result_grid_n29_190, num_grid_n29_189 * 64);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n29_188, &result_n29_188, &_preg_268435782);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&result_n29_188._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modadd(Coeffs(&result_n29_188._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n29_188._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435782._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&result_n29_188._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n29_188._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435782._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n29_188, &result_n29_188);
  Rescale(&result_n29_188._c0_poly, &result_n29_188._c0_poly);
  Rescale(&result_n29_188._c1_poly, &result_n29_188._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435783 = Rotate(result_n29_188, -16384);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_ckks_gen_tmp_4097_260, &result_n29_188, &_preg_268435783);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_ckks_gen_tmp_4097_260._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_ckks_gen_tmp_4097_260._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n29_188._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435783._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_ckks_gen_tmp_4097_260._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n29_188._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435783._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  _pgen_tmp_211_479 = *(PLAIN)Pt_from_msg(&_pgen_tmp_211_479, 5264 /* cst_202 */, 16384, 1, 2, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_210_478, &_ckks_gen_tmp_4097_260, &_pgen_tmp_211_479);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_210_478._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_210_478._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_4097_260._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_211_479._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_210_478._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_4097_260._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_211_479._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435604, &_pgen_tmp_210_478);
  Rescale(&_preg_268435604._c0_poly, &_pgen_tmp_210_478._c0_poly);
  Rescale(&_preg_268435604._c1_poly, &_pgen_tmp_210_478._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_212_480 = *(PLAIN)Pt_from_msg(&_pgen_tmp_212_480, 5265 /* cst_200 */, 16384, 1, 1, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n29_188, &_preg_268435604, &_pgen_tmp_212_480);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&result_n29_188._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&result_n29_188._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435604._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_212_480._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Set_coeffs(&result_n29_188._c1_poly, _pgen_rns_idx_267, degree, Coeffs(&_preg_268435604._c1_poly, _pgen_rns_idx_267, degree));
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435605, &result_n29_188);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 91
  // /fire9/Concat
  // pragma: 65536, 1045, 92
  CMPLR_TM_START(4, 0);
  _preg_268435784 = Rotate(_preg_268435605, -16384);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_preg_268435606, &_preg_268435602, &_preg_268435784);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_preg_268435606._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_preg_268435606._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435602._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435784._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_preg_268435606._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435602._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435784._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  CMPLR_TM_END(4, 0);
  // pragma: 65537, 1045, 92
  // /fire9/Relu
  // pragma: 65536, 1033, 93
  CMPLR_TM_START(10, 0);
  Bootstrap(&_preg_268435607, &_preg_268435606, 15);
  Encode_float(&_pgen_tmp_214_482, &_cst_277, 1, 1, Level(&_preg_268435607), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_213_481, &_preg_268435607, &_pgen_tmp_214_482);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_213_481._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_213_481._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435607._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_214_482._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_213_481._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435607._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_214_482._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435609, &_pgen_tmp_213_481);
  Rescale(&_preg_268435609._c0_poly, &_pgen_tmp_213_481._c0_poly);
  Rescale(&_preg_268435609._c1_poly, &_pgen_tmp_213_481._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435608 = App_relu(_preg_268435607, _preg_268435609, 32768);
  Copy_ciph(&_preg_268435610, &_preg_268435608);
  CMPLR_TM_END(10, 0);
  // pragma: 65537, 1033, 93
  // /conv10/conv10.0/Conv
  // pragma: 65536, 1027, 94
  CMPLR_TM_START(5, 0);
  Copy_ciph(&_preg_268435611, &_preg_268435610);
  // BlockingRot replicate=2
  _preg_268435785 = Rotate(_preg_268435611, -32768);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&input_dup_n29_192, &_preg_268435611, &_preg_268435785);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&input_dup_n29_192._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&input_dup_n29_192._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435611._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435785._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n29_192._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435611._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435785._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  // BlockingRot HRot=bs=16
  for (bs_input_n29_193 = 0; bs_input_n29_193 < 16; bs_input_n29_193 = bs_input_n29_193 + 1) {
    _preg_268435786 = Rotate(input_dup_n29_192, _cst_206[bs_input_n29_193]);
    Copy_ciph(&input_block_n29_194[bs_input_n29_193], &_preg_268435786);
  }
  Zero_ciph(&result_n30_195);
  for (num_grid_n30_196 = 0; num_grid_n30_196 < 32; num_grid_n30_196 = num_grid_n30_196 + 1) {
    Zero_ciph(&result_grid_n30_197);
    for (cap_block_n30_198 = 0; cap_block_n30_198 < 16; cap_block_n30_198 = cap_block_n30_198 + 1) {
      _ckks_gen_tmp_4224_261 = input_block_n29_194[cap_block_n30_198];
      _pgen_tmp_215_483 = *(PLAIN)Pt_from_msg(&_pgen_tmp_215_483, (cap_block_n30_198 + (num_grid_n30_196 * 16)) + 5266 /* cst_204_0 */, 32768, 1, 3, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_216_484, &_ckks_gen_tmp_4224_261, &_pgen_tmp_215_483);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&result_grid_n30_197, &result_grid_n30_197, &_pgen_tmp_216_484);
      _pgen_modulus_265 = Q_modulus();
      _pgen_num_q_266 = Poly_level(&result_grid_n30_197._c0_poly);
      for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_216_484._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_4224_261._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_215_483._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_216_484._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_4224_261._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_215_483._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n30_197._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n30_197._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_216_484._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&result_grid_n30_197._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_grid_n30_197._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_216_484._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        _pgen_modulus_265 = _pgen_modulus_265 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435787 = Rotate(result_grid_n30_197, num_grid_n30_196 * 1024);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&result_n30_195, &result_n30_195, &_preg_268435787);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&result_n30_195._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modadd(Coeffs(&result_n30_195._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n30_195._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435787._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&result_n30_195._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n30_195._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435787._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&result_n30_195, &result_n30_195);
  Rescale(&result_n30_195._c0_poly, &result_n30_195._c0_poly);
  Rescale(&result_n30_195._c1_poly, &result_n30_195._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435788 = Rotate(result_n30_195, -32768);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_ckks_gen_tmp_4260_262, &result_n30_195, &_preg_268435788);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_ckks_gen_tmp_4260_262._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_ckks_gen_tmp_4260_262._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n30_195._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435788._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_ckks_gen_tmp_4260_262._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&result_n30_195._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435788._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  _pgen_tmp_218_486 = *(PLAIN)Pt_from_msg(&_pgen_tmp_218_486, 5778 /* cst_207 */, 32768, 1, 2, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_217_485, &_ckks_gen_tmp_4260_262, &_pgen_tmp_218_486);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_217_485._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_217_485._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_4260_262._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_218_486._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_217_485._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_4260_262._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_218_486._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435612, &_pgen_tmp_217_485);
  Rescale(&_preg_268435612._c0_poly, &_pgen_tmp_217_485._c0_poly);
  Rescale(&_preg_268435612._c1_poly, &_pgen_tmp_217_485._c1_poly);
  CMPLR_TM_END(19, 0);
  _pgen_tmp_219_487 = *(PLAIN)Pt_from_msg(&_pgen_tmp_219_487, 5779 /* cst_205 */, 32768, 1, 1, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&result_n30_195, &_preg_268435612, &_pgen_tmp_219_487);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&result_n30_195._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&result_n30_195._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435612._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_219_487._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Set_coeffs(&result_n30_195._c1_poly, _pgen_rns_idx_267, degree, Coeffs(&_preg_268435612._c1_poly, _pgen_rns_idx_267, degree));
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(13, 0);
  Copy_ciph(&_preg_268435613, &result_n30_195);
  CMPLR_TM_END(5, 0);
  // pragma: 65537, 1027, 94
  // /conv10/conv10.2/Relu
  // pragma: 65536, 1033, 95
  CMPLR_TM_START(10, 0);
  Bootstrap(&_preg_268435614, &_preg_268435613, 18);
  Encode_float(&_pgen_tmp_221_489, &_cst_278, 1, 1, Level(&_preg_268435614), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_220_488, &_preg_268435614, &_pgen_tmp_221_489);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_220_488._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_220_488._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435614._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_221_489._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_220_488._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435614._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_221_489._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_preg_268435616, &_pgen_tmp_220_488);
  Rescale(&_preg_268435616._c0_poly, &_pgen_tmp_220_488._c0_poly);
  Rescale(&_preg_268435616._c1_poly, &_pgen_tmp_220_488._c1_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435615 = App_relu(_preg_268435614, _preg_268435616, 32768);
  Copy_ciph(&_preg_268435617, &_preg_268435615);
  CMPLR_TM_END(10, 0);
  // pragma: 65537, 1033, 95
  // /avg_pool/GlobalAveragePool
  // pragma: 65536, 1030, 96
  CMPLR_TM_START(8, 0);
  Copy_ciph(&_preg_268435618, &_preg_268435617);
  for (gap_sum_index_n30_199 = 0; gap_sum_index_n30_199 < 6; gap_sum_index_n30_199 = gap_sum_index_n30_199 + 1) {
    _preg_268435789 = Rotate(_preg_268435618, 1 << gap_sum_index_n30_199);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&_preg_268435618, &_preg_268435618, &_preg_268435789);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&_preg_268435618._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modadd(Coeffs(&_preg_268435618._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435618._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435789._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&_preg_268435618._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435618._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435789._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  Encode_float(&_pgen_tmp_223_491, &_cst_208, 1, 1, Level(&_preg_268435618), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_222_490, &_preg_268435618, &_pgen_tmp_223_491);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_222_490._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_222_490._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435618._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_223_491._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_222_490._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435618._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_223_491._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&gap_result30_200, &_pgen_tmp_222_490);
  Rescale(&gap_result30_200._c0_poly, &_pgen_tmp_222_490._c0_poly);
  Rescale(&gap_result30_200._c1_poly, &_pgen_tmp_222_490._c1_poly);
  CMPLR_TM_END(19, 0);
  Copy_ciph(&_preg_268435619, &gap_result30_200);
  Copy_ciph(&orig_result30_201, &_preg_268435619);
  _pgen_tmp_225_493 = *(PLAIN)Pt_from_msg(&_pgen_tmp_225_493, 5780 /* cst_209 */, 32768, 1, 5, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_224_492, &orig_result30_201, &_pgen_tmp_225_493);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_224_492._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_224_492._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&orig_result30_201._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_225_493._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_224_492._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&orig_result30_201._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_225_493._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&orig_result30_201, &_pgen_tmp_224_492);
  Rescale(&orig_result30_201._c0_poly, &_pgen_tmp_224_492._c0_poly);
  Rescale(&orig_result30_201._c1_poly, &_pgen_tmp_224_492._c1_poly);
  CMPLR_TM_END(19, 0);
  Zero_ciph(&ss_result31_202);
  for (comb_cc_index_n31_203 = 0; comb_cc_index_n31_203 < 512; comb_cc_index_n31_203 = comb_cc_index_n31_203 + 1) {
    _preg_268435790 = Rotate(orig_result30_201, comb_cc_index_n31_203 * 63);
    Copy_ciph(&_ckks_gen_tmp_4373_263, &_preg_268435790);
    _pgen_tmp_226_494 = *(PLAIN)Pt_from_msg(&_pgen_tmp_226_494, comb_cc_index_n31_203 + 5781 /* cst_210_0 */, 32768, 1, 4, 0);
    CMPLR_TM_START(17, 0);
    Init_ciph_up_scale_plain(&_pgen_tmp_227_495, &_ckks_gen_tmp_4373_263, &_pgen_tmp_226_494);
    CMPLR_TM_END(17, 0);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&ss_result31_202, &ss_result31_202, &_pgen_tmp_227_495);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&ss_result31_202._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_227_495._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_4373_263._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_226_494._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_227_495._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_4373_263._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_226_494._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&ss_result31_202._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&ss_result31_202._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_227_495._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&ss_result31_202._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&ss_result31_202._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_227_495._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&ss_result31_202, &ss_result31_202);
  Rescale(&ss_result31_202._c0_poly, &ss_result31_202._c0_poly);
  Rescale(&ss_result31_202._c1_poly, &ss_result31_202._c1_poly);
  CMPLR_TM_END(19, 0);
  Copy_ciph(&_preg_268435620, &ss_result31_202);
  CMPLR_TM_END(8, 0);
  // pragma: 65537, 1030, 96
  // /Reshape
  // pragma: 65536, 1034, 97
  CMPLR_TM_START(6, 0);
  Copy_ciph(&_preg_268435621, &_preg_268435620);
  CMPLR_TM_END(6, 0);
  // pragma: 65537, 1034, 97
  // /fc/Gemm
  // pragma: 65536, 1029, 98
  CMPLR_TM_START(7, 0);
  // BlockingRot replicate=3
  _preg_268435791 = Rotate(_preg_268435621, -512);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_228_496, &_preg_268435621, &_preg_268435791);
  CMPLR_TM_END(12, 0);
  _preg_268435792 = Rotate(_preg_268435621, -1024);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&input_dup_n31_204, &_pgen_tmp_228_496, &_preg_268435792);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&input_dup_n31_204._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_228_496._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435621._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435791._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_228_496._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435621._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435791._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n31_204._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_228_496._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435792._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modadd(Coeffs(&input_dup_n31_204._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_228_496._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435792._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(12, 0);
  // BlockingRot HRot=bs=2
  _preg_268435793 = Rotate(input_dup_n31_204, 0);
  Copy_ciph(&input_block_n31_205[0], &_preg_268435793);
  for (bs_input_n31_206 = 1; bs_input_n31_206 < 2; bs_input_n31_206 = bs_input_n31_206 + 1) {
    _preg_268435794 = Rotate(input_dup_n31_204, _cst_213[bs_input_n31_206]);
    Copy_ciph(&input_block_n31_205[bs_input_n31_206], &_preg_268435794);
  }
  Zero_ciph(&gemm_result_n32_207);
  // IMRA Metakernel: gs=4
  for (gs_n32_208 = 0; gs_n32_208 < 4; gs_n32_208 = gs_n32_208 + 1) {
    Zero_ciph(&block_result_209);
    for (bs_n32_210 = 0; bs_n32_210 < 2; bs_n32_210 = bs_n32_210 + 1) {
      _ckks_gen_tmp_4477_264 = input_block_n31_205[bs_n32_210];
      _pgen_tmp_229_497 = *(PLAIN)Pt_from_msg(&_pgen_tmp_229_497, (bs_n32_210 + (gs_n32_208 * 2)) + 6293 /* cst_212_0 */, 1040, 1, 3, 0);
      CMPLR_TM_START(17, 0);
      Init_ciph_up_scale_plain(&_pgen_tmp_230_498, &_ckks_gen_tmp_4477_264, &_pgen_tmp_229_497);
      CMPLR_TM_END(17, 0);
      CMPLR_TM_START(12, 0);
      Init_ciph_same_scale(&block_result_209, &block_result_209, &_pgen_tmp_230_498);
      _pgen_modulus_265 = Q_modulus();
      _pgen_num_q_266 = Poly_level(&block_result_209._c0_poly);
      for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_230_498._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_4477_264._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_229_497._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_230_498._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_ckks_gen_tmp_4477_264._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_229_497._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&block_result_209._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&block_result_209._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_230_498._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        Hw_modadd(Coeffs(&block_result_209._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&block_result_209._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_230_498._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
        _pgen_modulus_265 = _pgen_modulus_265 + 1;
      }
      CMPLR_TM_END(12, 0);
    }
    _preg_268435795 = Rotate(block_result_209, gs_n32_208 * 2);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&gemm_result_n32_207, &gemm_result_n32_207, &_preg_268435795);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&gemm_result_n32_207._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modadd(Coeffs(&gemm_result_n32_207._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&gemm_result_n32_207._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435795._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&gemm_result_n32_207._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&gemm_result_n32_207._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435795._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&gemm_result_n32_207, &gemm_result_n32_207);
  Rescale(&gemm_result_n32_207._c0_poly, &gemm_result_n32_207._c0_poly);
  Rescale(&gemm_result_n32_207._c1_poly, &gemm_result_n32_207._c1_poly);
  CMPLR_TM_END(19, 0);
  // gemm result reduce->Ps=2
  Copy_ciph(&reduce_intra_211, &gemm_result_n32_207);
  for (ri_n32_212 = 0; ri_n32_212 < 1; ri_n32_212 = ri_n32_212 + 1) {
    _preg_268435796 = Rotate(reduce_intra_211, (1 << ri_n32_212) * 520);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&reduce_intra_211, &reduce_intra_211, &_preg_268435796);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&reduce_intra_211._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modadd(Coeffs(&reduce_intra_211._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&reduce_intra_211._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435796._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&reduce_intra_211._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&reduce_intra_211._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435796._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  // gemm result reduce->(kp/np)=32
  Copy_ciph(&reduce_intra_213, &reduce_intra_211);
  for (ri_n32_214 = 0; ri_n32_214 < 5; ri_n32_214 = ri_n32_214 + 1) {
    _preg_268435797 = Rotate(reduce_intra_213, (1 << ri_n32_214) * 16);
    CMPLR_TM_START(12, 0);
    Init_ciph_same_scale(&reduce_intra_213, &reduce_intra_213, &_preg_268435797);
    _pgen_modulus_265 = Q_modulus();
    _pgen_num_q_266 = Poly_level(&reduce_intra_213._c0_poly);
    for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
      Hw_modadd(Coeffs(&reduce_intra_213._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&reduce_intra_213._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435797._c0_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      Hw_modadd(Coeffs(&reduce_intra_213._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&reduce_intra_213._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_preg_268435797._c1_poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
      _pgen_modulus_265 = _pgen_modulus_265 + 1;
    }
    CMPLR_TM_END(12, 0);
  }
  // gemm add bias
  _pgen_tmp_231_499 = *(PLAIN)Pt_from_msg(&_pgen_tmp_231_499, 6301 /* cst_3 */, 10, 1, 2, 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&reduce_intra_213, &reduce_intra_213, &_pgen_tmp_231_499);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&reduce_intra_213._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modadd(Coeffs(&reduce_intra_213._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&reduce_intra_213._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_231_499._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Set_coeffs(&reduce_intra_213._c1_poly, _pgen_rns_idx_267, degree, Coeffs(&reduce_intra_213._c1_poly, _pgen_rns_idx_267, degree));
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(13, 0);
  _pgen_tmp_233_501 = *(PLAIN)Pt_from_msg(&_pgen_tmp_233_501, 6302 /* cst_214 */, 10, 1, 2, 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_232_500, &reduce_intra_213, &_pgen_tmp_233_501);
  _pgen_modulus_265 = Q_modulus();
  _pgen_num_q_266 = Poly_level(&_pgen_tmp_232_500._c0_poly);
  for (_pgen_rns_idx_267 = 0; _pgen_rns_idx_267 < _pgen_num_q_266; _pgen_rns_idx_267 = _pgen_rns_idx_267 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_232_500._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&reduce_intra_213._c0_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_233_501._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_232_500._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&reduce_intra_213._c1_poly, _pgen_rns_idx_267, degree), Coeffs(&_pgen_tmp_233_501._poly, _pgen_rns_idx_267, degree), _pgen_modulus_265, degree);
    _pgen_modulus_265 = _pgen_modulus_265 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&reduce_intra_213, &_pgen_tmp_232_500);
  Rescale(&reduce_intra_213._c0_poly, &_pgen_tmp_232_500._c0_poly);
  Rescale(&reduce_intra_213._c1_poly, &_pgen_tmp_232_500._c1_poly);
  CMPLR_TM_END(19, 0);
  Copy_ciph(&output_215, &reduce_intra_213);
  CMPLR_TM_END(7, 0);
  // pragma: 65537, 1029, 98
  Set_output_data("output", 0, &output_215);
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
  CIPHERTEXT3 _pgen_tmp_3_33;
  CIPHERTEXT _pgen_tmp_4_34;
  PLAINTEXT _pgen_tmp_5_35;
  CIPHERTEXT3 _pgen_tmp_6_36;
  POLYNOMIAL _pgen_tmp_7_37;
  POLYNOMIAL _pgen_tmp_8_38;
  CIPHERTEXT3 _pgen_tmp_9_39;
  CIPHERTEXT _pgen_tmp_10_40;
  PLAINTEXT _pgen_tmp_11_41;
  CIPHERTEXT3 _pgen_tmp_12_42;
  CIPHERTEXT3 _pgen_tmp_13_43;
  CIPHERTEXT _pgen_tmp_14_44;
  PLAINTEXT _pgen_tmp_15_45;
  CIPHERTEXT _pgen_tmp_16_46;
  POLYNOMIAL _pgen_tmp_17_47;
  POLYNOMIAL _pgen_tmp_18_48;
  CIPHERTEXT3 _pgen_tmp_19_49;
  CIPHERTEXT _pgen_tmp_20_50;
  PLAINTEXT _pgen_tmp_21_51;
  CIPHERTEXT _pgen_tmp_22_52;
  CIPHERTEXT _pgen_tmp_23_53;
  POLYNOMIAL _pgen_tmp_24_54;
  POLYNOMIAL _pgen_tmp_25_55;
  CIPHERTEXT3 _pgen_tmp_26_56;
  CIPHERTEXT3 _pgen_tmp_27_57;
  CIPHERTEXT _pgen_tmp_28_58;
  PLAINTEXT _pgen_tmp_29_59;
  CIPHERTEXT _pgen_tmp_30_60;
  POLYNOMIAL _pgen_tmp_31_61;
  POLYNOMIAL _pgen_tmp_32_62;
  CIPHERTEXT3 _pgen_tmp_33_63;
  CIPHERTEXT _pgen_tmp_34_64;
  PLAINTEXT _pgen_tmp_35_65;
  CIPHERTEXT _pgen_tmp_36_66;
  CIPHERTEXT _pgen_tmp_37_67;
  CIPHERTEXT3 _pgen_tmp_38_68;
  POLYNOMIAL _pgen_tmp_39_69;
  POLYNOMIAL _pgen_tmp_40_70;
  CIPHERTEXT3 _pgen_tmp_41_71;
  CIPHERTEXT _pgen_tmp_42_72;
  PLAINTEXT _pgen_tmp_43_73;
  CIPHERTEXT3 _pgen_tmp_44_74;
  POLYNOMIAL _pgen_tmp_45_75;
  POLYNOMIAL _pgen_tmp_46_76;
  CIPHERTEXT3 _pgen_tmp_47_77;
  CIPHERTEXT _pgen_tmp_48_78;
  CIPHERTEXT _pgen_tmp_49_79;
  PLAINTEXT _pgen_tmp_50_80;
  CIPHERTEXT _pgen_tmp_51_81;
  CIPHERTEXT3 _pgen_tmp_52_82;
  POLYNOMIAL _pgen_tmp_53_83;
  POLYNOMIAL _pgen_tmp_54_84;
  CIPHERTEXT3 _pgen_tmp_55_85;
  CIPHERTEXT _pgen_tmp_56_86;
  PLAINTEXT _pgen_tmp_57_87;
  CIPHERTEXT3 _pgen_tmp_58_88;
  POLYNOMIAL _pgen_tmp_59_89;
  POLYNOMIAL _pgen_tmp_60_90;
  CIPHERTEXT3 _pgen_tmp_61_91;
  CIPHERTEXT _pgen_tmp_62_92;
  PLAINTEXT _pgen_tmp_63_93;
  CIPHERTEXT3 _pgen_tmp_64_94;
  CIPHERTEXT3 _pgen_tmp_65_95;
  CIPHERTEXT3 _pgen_tmp_66_96;
  CIPHERTEXT _pgen_tmp_67_97;
  PLAINTEXT _pgen_tmp_68_98;
  CIPHERTEXT _pgen_tmp_69_99;
  POLYNOMIAL _pgen_tmp_70_100;
  POLYNOMIAL _pgen_tmp_71_101;
  CIPHERTEXT3 _pgen_tmp_72_102;
  CIPHERTEXT _pgen_tmp_73_103;
  PLAINTEXT _pgen_tmp_74_104;
  CIPHERTEXT _pgen_tmp_75_105;
  CIPHERTEXT _pgen_tmp_76_106;
  POLYNOMIAL _pgen_tmp_77_107;
  POLYNOMIAL _pgen_tmp_78_108;
  CIPHERTEXT3 _pgen_tmp_79_109;
  CIPHERTEXT _pgen_tmp_80_110;
  PLAINTEXT _pgen_tmp_81_111;
  CIPHERTEXT _pgen_tmp_82_112;
  CIPHERTEXT _pgen_tmp_83_113;
  PLAINTEXT _pgen_tmp_84_114;
  CIPHERTEXT _pgen_tmp_85_115;
  CIPHERTEXT _pgen_tmp_86_116;
  CIPHERTEXT _pgen_tmp_87_117;
  POLYNOMIAL _pgen_tmp_88_118;
  POLYNOMIAL _pgen_tmp_89_119;
  CIPHERTEXT3 _pgen_tmp_90_120;
  CIPHERTEXT3 _pgen_tmp_91_121;
  CIPHERTEXT _pgen_tmp_92_122;
  PLAINTEXT _pgen_tmp_93_123;
  CIPHERTEXT _pgen_tmp_94_124;
  CIPHERTEXT _pgen_tmp_95_125;
  PLAINTEXT _pgen_tmp_96_126;
  CIPHERTEXT _pgen_tmp_97_127;
  CIPHERTEXT _pgen_tmp_98_128;
  POLYNOMIAL _pgen_tmp_99_129;
  POLYNOMIAL _pgen_tmp_100_130;
  CIPHERTEXT3 _pgen_tmp_101_131;
  CIPHERTEXT _pgen_tmp_102_132;
  PLAINTEXT _pgen_tmp_103_133;
  CIPHERTEXT _pgen_tmp_104_134;
  CIPHERTEXT _pgen_tmp_105_135;
  PLAINTEXT _pgen_tmp_106_136;
  CIPHERTEXT _pgen_tmp_107_137;
  CIPHERTEXT _pgen_tmp_108_138;
  CIPHERTEXT _pgen_tmp_109_139;
  CIPHERTEXT3 _pgen_tmp_110_140;
  POLYNOMIAL _pgen_tmp_111_141;
  POLYNOMIAL _pgen_tmp_112_142;
  CIPHERTEXT3 _pgen_tmp_113_143;
  CIPHERTEXT _pgen_tmp_114_144;
  PLAINTEXT _pgen_tmp_115_145;
  CIPHERTEXT3 _pgen_tmp_116_146;
  POLYNOMIAL _pgen_tmp_117_147;
  POLYNOMIAL _pgen_tmp_118_148;
  CIPHERTEXT3 _pgen_tmp_119_149;
  CIPHERTEXT _pgen_tmp_120_150;
  CIPHERTEXT _pgen_tmp_121_151;
  PLAINTEXT _pgen_tmp_122_152;
  CIPHERTEXT _pgen_tmp_123_153;
  CIPHERTEXT3 _pgen_tmp_124_154;
  POLYNOMIAL _pgen_tmp_125_155;
  POLYNOMIAL _pgen_tmp_126_156;
  CIPHERTEXT3 _pgen_tmp_127_157;
  CIPHERTEXT _pgen_tmp_128_158;
  PLAINTEXT _pgen_tmp_129_159;
  CIPHERTEXT3 _pgen_tmp_130_160;
  POLYNOMIAL _pgen_tmp_131_161;
  POLYNOMIAL _pgen_tmp_132_162;
  CIPHERTEXT3 _pgen_tmp_133_163;
  CIPHERTEXT _pgen_tmp_134_164;
  PLAINTEXT _pgen_tmp_135_165;
  CIPHERTEXT3 _pgen_tmp_136_166;
  CIPHERTEXT3 _pgen_tmp_137_167;
  CIPHERTEXT3 _pgen_tmp_138_168;
  CIPHERTEXT _pgen_tmp_139_169;
  PLAINTEXT _pgen_tmp_140_170;
  CIPHERTEXT _pgen_tmp_141_171;
  POLYNOMIAL _pgen_tmp_142_172;
  POLYNOMIAL _pgen_tmp_143_173;
  CIPHERTEXT3 _pgen_tmp_144_174;
  POLYNOMIAL _pgen_tmp_145_175;
  POLYNOMIAL _pgen_tmp_146_176;
  CIPHERTEXT3 _pgen_tmp_147_177;
  CIPHERTEXT3 _pgen_tmp_148_178;
  CIPHERTEXT _pgen_tmp_149_179;
  PLAINTEXT _pgen_tmp_150_180;
  CIPHERTEXT _pgen_tmp_151_181;
  POLYNOMIAL _pgen_tmp_152_182;
  POLYNOMIAL _pgen_tmp_153_183;
  CIPHERTEXT3 _pgen_tmp_154_184;
  CIPHERTEXT3 _pgen_tmp_155_185;
  CIPHERTEXT _pgen_tmp_156_186;
  PLAINTEXT _pgen_tmp_157_187;
  CIPHERTEXT _pgen_tmp_158_188;
  POLYNOMIAL _pgen_tmp_159_189;
  POLYNOMIAL _pgen_tmp_160_190;
  CIPHERTEXT3 _pgen_tmp_161_191;
  CIPHERTEXT _pgen_tmp_162_192;
  CIPHERTEXT _pgen_tmp_163_193;
  POLYNOMIAL _pgen_tmp_164_194;
  POLYNOMIAL _pgen_tmp_165_195;
  CIPHERTEXT3 _pgen_tmp_166_196;
  CIPHERTEXT3 _pgen_tmp_167_197;
  CIPHERTEXT3 _pgen_tmp_168_198;
  CIPHERTEXT _pgen_tmp_169_199;
  PLAINTEXT _pgen_tmp_170_200;
  CIPHERTEXT _pgen_tmp_171_201;
  POLYNOMIAL _pgen_tmp_172_202;
  POLYNOMIAL _pgen_tmp_173_203;
  CIPHERTEXT3 _pgen_tmp_174_204;
  CIPHERTEXT3 _pgen_tmp_175_205;
  CIPHERTEXT _pgen_tmp_176_206;
  PLAINTEXT _pgen_tmp_177_207;
  CIPHERTEXT _pgen_tmp_178_208;
  POLYNOMIAL _pgen_tmp_179_209;
  POLYNOMIAL _pgen_tmp_180_210;
  CIPHERTEXT3 _pgen_tmp_181_211;
  CIPHERTEXT _pgen_tmp_182_212;
  POLYNOMIAL _pgen_tmp_183_213;
  POLYNOMIAL _pgen_tmp_184_214;
  CIPHERTEXT3 _pgen_tmp_185_215;
  CIPHERTEXT3 _pgen_tmp_186_216;
  CIPHERTEXT _pgen_tmp_187_217;
  PLAINTEXT _pgen_tmp_188_218;
  CIPHERTEXT _pgen_tmp_189_219;
  POLYNOMIAL _pgen_tmp_190_220;
  POLYNOMIAL _pgen_tmp_191_221;
  CIPHERTEXT3 _pgen_tmp_192_222;
  CIPHERTEXT3 _pgen_tmp_193_223;
  CIPHERTEXT _pgen_tmp_194_224;
  PLAINTEXT _pgen_tmp_195_225;
  CIPHERTEXT _pgen_tmp_196_226;
  POLYNOMIAL _pgen_tmp_197_227;
  POLYNOMIAL _pgen_tmp_198_228;
  CIPHERTEXT3 _pgen_tmp_199_229;
  CIPHERTEXT _pgen_tmp_200_230;
  CIPHERTEXT _pgen_tmp_201_231;
  CIPHERTEXT _pgen_tmp_202_232;
  PLAINTEXT _pgen_tmp_203_233;
  CIPHERTEXT _pgen_tmp_204_234;
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
  memset(&_pgen_tmp_156_186, 0, sizeof(_pgen_tmp_156_186));
  memset(&_pgen_tmp_157_187, 0, sizeof(_pgen_tmp_157_187));
  memset(&_pgen_tmp_158_188, 0, sizeof(_pgen_tmp_158_188));
  memset(&_pgen_tmp_159_189, 0, sizeof(_pgen_tmp_159_189));
  memset(&_pgen_tmp_160_190, 0, sizeof(_pgen_tmp_160_190));
  memset(&_pgen_tmp_161_191, 0, sizeof(_pgen_tmp_161_191));
  memset(&_pgen_tmp_162_192, 0, sizeof(_pgen_tmp_162_192));
  memset(&_pgen_tmp_163_193, 0, sizeof(_pgen_tmp_163_193));
  memset(&_pgen_tmp_164_194, 0, sizeof(_pgen_tmp_164_194));
  memset(&_pgen_tmp_165_195, 0, sizeof(_pgen_tmp_165_195));
  memset(&_pgen_tmp_166_196, 0, sizeof(_pgen_tmp_166_196));
  memset(&_pgen_tmp_167_197, 0, sizeof(_pgen_tmp_167_197));
  memset(&_pgen_tmp_168_198, 0, sizeof(_pgen_tmp_168_198));
  memset(&_pgen_tmp_169_199, 0, sizeof(_pgen_tmp_169_199));
  memset(&_pgen_tmp_170_200, 0, sizeof(_pgen_tmp_170_200));
  memset(&_pgen_tmp_171_201, 0, sizeof(_pgen_tmp_171_201));
  memset(&_pgen_tmp_172_202, 0, sizeof(_pgen_tmp_172_202));
  memset(&_pgen_tmp_173_203, 0, sizeof(_pgen_tmp_173_203));
  memset(&_pgen_tmp_174_204, 0, sizeof(_pgen_tmp_174_204));
  memset(&_pgen_tmp_175_205, 0, sizeof(_pgen_tmp_175_205));
  memset(&_pgen_tmp_176_206, 0, sizeof(_pgen_tmp_176_206));
  memset(&_pgen_tmp_177_207, 0, sizeof(_pgen_tmp_177_207));
  memset(&_pgen_tmp_178_208, 0, sizeof(_pgen_tmp_178_208));
  memset(&_pgen_tmp_179_209, 0, sizeof(_pgen_tmp_179_209));
  memset(&_pgen_tmp_180_210, 0, sizeof(_pgen_tmp_180_210));
  memset(&_pgen_tmp_181_211, 0, sizeof(_pgen_tmp_181_211));
  memset(&_pgen_tmp_182_212, 0, sizeof(_pgen_tmp_182_212));
  memset(&_pgen_tmp_183_213, 0, sizeof(_pgen_tmp_183_213));
  memset(&_pgen_tmp_184_214, 0, sizeof(_pgen_tmp_184_214));
  memset(&_pgen_tmp_185_215, 0, sizeof(_pgen_tmp_185_215));
  memset(&_pgen_tmp_186_216, 0, sizeof(_pgen_tmp_186_216));
  memset(&_pgen_tmp_187_217, 0, sizeof(_pgen_tmp_187_217));
  memset(&_pgen_tmp_188_218, 0, sizeof(_pgen_tmp_188_218));
  memset(&_pgen_tmp_189_219, 0, sizeof(_pgen_tmp_189_219));
  memset(&_pgen_tmp_190_220, 0, sizeof(_pgen_tmp_190_220));
  memset(&_pgen_tmp_191_221, 0, sizeof(_pgen_tmp_191_221));
  memset(&_pgen_tmp_192_222, 0, sizeof(_pgen_tmp_192_222));
  memset(&_pgen_tmp_193_223, 0, sizeof(_pgen_tmp_193_223));
  memset(&_pgen_tmp_194_224, 0, sizeof(_pgen_tmp_194_224));
  memset(&_pgen_tmp_195_225, 0, sizeof(_pgen_tmp_195_225));
  memset(&_pgen_tmp_196_226, 0, sizeof(_pgen_tmp_196_226));
  memset(&_pgen_tmp_197_227, 0, sizeof(_pgen_tmp_197_227));
  memset(&_pgen_tmp_198_228, 0, sizeof(_pgen_tmp_198_228));
  memset(&_pgen_tmp_199_229, 0, sizeof(_pgen_tmp_199_229));
  memset(&_pgen_tmp_200_230, 0, sizeof(_pgen_tmp_200_230));
  memset(&_pgen_tmp_201_231, 0, sizeof(_pgen_tmp_201_231));
  memset(&_pgen_tmp_202_232, 0, sizeof(_pgen_tmp_202_232));
  memset(&_pgen_tmp_203_233, 0, sizeof(_pgen_tmp_203_233));
  memset(&_pgen_tmp_204_234, 0, sizeof(_pgen_tmp_204_234));
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
  Free_data(&_pgen_tmp_2_32);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_3_33, &_pgen_tmp_0_27);
  Rescale(&_pgen_tmp_3_33._c0_poly, &_pgen_tmp_0_27._c0_poly);
  Rescale(&_pgen_tmp_3_33._c1_poly, &_pgen_tmp_0_27._c1_poly);
  Rescale(&_pgen_tmp_3_33._c2_poly, &_pgen_tmp_0_27._c2_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435456 = Relinearize(_pgen_tmp_3_33);
  Copy_ciph(&_relu_tmp8_3, &_preg_268435456);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_4_34, &_relu_tmp8_3, &_relu_tmp8_3);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_5_35, &_cst_215, 1, Sc_degree(&_pgen_tmp_4_34), Level(&_pgen_tmp_4_34), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp14_4, &_pgen_tmp_4_34, &_pgen_tmp_5_35);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_relu_tmp14_4._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_4_34._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp8_3._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp8_3._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_4_34._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp8_3._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp8_3._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_4_34._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_5_35._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Set_coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_30, degree, Coeffs(&_pgen_tmp_4_34._c1_poly, _pgen_rns_idx_30, degree));
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(13, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_6_36, &_relu_tmp14_4, &_relu_tmp14_4);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_6_36._c0_poly);
  Init_poly_by_size(&_pgen_tmp_7_37, 1, 0);
  Init_poly_by_size(&_pgen_tmp_8_38, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_7_37, 0, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_8_38, 0, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_6_36._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_6_36._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_7_37, 0, degree), Coeffs(&_pgen_tmp_8_38, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_6_36._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_7_37);
  Free_data(&_pgen_tmp_8_38);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_9_39, &_pgen_tmp_6_36);
  Rescale(&_pgen_tmp_9_39._c0_poly, &_pgen_tmp_6_36._c0_poly);
  Rescale(&_pgen_tmp_9_39._c1_poly, &_pgen_tmp_6_36._c1_poly);
  Rescale(&_pgen_tmp_9_39._c2_poly, &_pgen_tmp_6_36._c2_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435457 = Relinearize(_pgen_tmp_9_39);
  Copy_ciph(&_relu_tmp18_5, &_preg_268435457);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_10_40, &_relu_tmp18_5, &_relu_tmp18_5);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_11_41, &_cst_216, 1, Sc_degree(&_pgen_tmp_10_40), Level(&_pgen_tmp_10_40), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp24_6, &_pgen_tmp_10_40, &_pgen_tmp_11_41);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_relu_tmp24_6._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_10_40._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp18_5._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp18_5._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_10_40._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp18_5._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp18_5._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp24_6._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_10_40._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_11_41._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Set_coeffs(&_relu_tmp24_6._c1_poly, _pgen_rns_idx_30, degree, Coeffs(&_pgen_tmp_10_40._c1_poly, _pgen_rns_idx_30, degree));
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(13, 0);
  Encode_float_mask(&_pgen_tmp_15_45, -3.809199432496630510769, mask_len_2, 1, Level(&input_ct1_1), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_14_44, &input_ct1_1, &_pgen_tmp_15_45);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_14_44._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_14_44._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_15_45._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_14_44._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_15_45._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_16_46, &_pgen_tmp_14_44);
  Rescale(&_pgen_tmp_16_46._c0_poly, &_pgen_tmp_14_44._c0_poly);
  Rescale(&_pgen_tmp_16_46._c1_poly, &_pgen_tmp_14_44._c1_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_13_43, &_relu_tmp14_4, &_pgen_tmp_16_46);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_13_43._c0_poly);
  Init_poly_by_size(&_pgen_tmp_17_47, 1, 0);
  Init_poly_by_size(&_pgen_tmp_18_48, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_17_47, 0, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_16_46._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_18_48, 0, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_16_46._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_13_43._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_16_46._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_13_43._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_17_47, 0, degree), Coeffs(&_pgen_tmp_18_48, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_13_43._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_16_46._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_17_47);
  Free_data(&_pgen_tmp_18_48);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_19_49, &_pgen_tmp_13_43);
  Rescale(&_pgen_tmp_19_49._c0_poly, &_pgen_tmp_13_43._c0_poly);
  Rescale(&_pgen_tmp_19_49._c1_poly, &_pgen_tmp_13_43._c1_poly);
  Rescale(&_pgen_tmp_19_49._c2_poly, &_pgen_tmp_13_43._c2_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435458 = Relinearize(_pgen_tmp_19_49);
  Encode_float_mask(&_pgen_tmp_21_51, 2.460940868770910228136, mask_len_2, 1, Level(&input_ct1_1), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_20_50, &input_ct1_1, &_pgen_tmp_21_51);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_20_50._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_20_50._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_21_51._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_20_50._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_21_51._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_22_52, &_pgen_tmp_20_50);
  Rescale(&_pgen_tmp_22_52._c0_poly, &_pgen_tmp_20_50._c0_poly);
  Rescale(&_pgen_tmp_22_52._c1_poly, &_pgen_tmp_20_50._c1_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_23_53, &_preg_268435458, &_pgen_tmp_22_52);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_12_42, &_relu_tmp24_6, &_pgen_tmp_23_53);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_12_42._c0_poly);
  Init_poly_by_size(&_pgen_tmp_24_54, 1, 0);
  Init_poly_by_size(&_pgen_tmp_25_55, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_23_53._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435458._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_22_52._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_23_53._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435458._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_22_52._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_24_54, 0, degree), Coeffs(&_relu_tmp24_6._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_23_53._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_25_55, 0, degree), Coeffs(&_relu_tmp24_6._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_23_53._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_12_42._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp24_6._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_23_53._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_12_42._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_24_54, 0, degree), Coeffs(&_pgen_tmp_25_55, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_12_42._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp24_6._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_23_53._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_24_54);
  Free_data(&_pgen_tmp_25_55);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_26_56, &_pgen_tmp_12_42);
  Rescale(&_pgen_tmp_26_56._c0_poly, &_pgen_tmp_12_42._c0_poly);
  Rescale(&_pgen_tmp_26_56._c1_poly, &_pgen_tmp_12_42._c1_poly);
  Rescale(&_pgen_tmp_26_56._c2_poly, &_pgen_tmp_12_42._c2_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435459 = Relinearize(_pgen_tmp_26_56);
  Encode_float_mask(&_pgen_tmp_29_59, -1.430304794543664215212, mask_len_2, 1, Level(&input_ct1_1), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_28_58, &input_ct1_1, &_pgen_tmp_29_59);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_28_58._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_28_58._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_29_59._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_28_58._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_29_59._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_30_60, &_pgen_tmp_28_58);
  Rescale(&_pgen_tmp_30_60._c0_poly, &_pgen_tmp_28_58._c0_poly);
  Rescale(&_pgen_tmp_30_60._c1_poly, &_pgen_tmp_28_58._c1_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_27_57, &_relu_tmp14_4, &_pgen_tmp_30_60);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_27_57._c0_poly);
  Init_poly_by_size(&_pgen_tmp_31_61, 1, 0);
  Init_poly_by_size(&_pgen_tmp_32_62, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_31_61, 0, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_30_60._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_32_62, 0, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_30_60._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_27_57._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_30_60._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_27_57._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_31_61, 0, degree), Coeffs(&_pgen_tmp_32_62, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_27_57._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_30_60._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_31_61);
  Free_data(&_pgen_tmp_32_62);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_33_63, &_pgen_tmp_27_57);
  Rescale(&_pgen_tmp_33_63._c0_poly, &_pgen_tmp_27_57._c0_poly);
  Rescale(&_pgen_tmp_33_63._c1_poly, &_pgen_tmp_27_57._c1_poly);
  Rescale(&_pgen_tmp_33_63._c2_poly, &_pgen_tmp_27_57._c2_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435460 = Relinearize(_pgen_tmp_33_63);
  Encode_float_mask(&_pgen_tmp_35_65, 2.944661935353764636858, mask_len_2, 1, Level(&input_ct1_1), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_34_64, &input_ct1_1, &_pgen_tmp_35_65);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_34_64._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_34_64._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_35_65._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_34_64._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_35_65._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_36_66, &_pgen_tmp_34_64);
  Rescale(&_pgen_tmp_36_66._c0_poly, &_pgen_tmp_34_64._c0_poly);
  Rescale(&_pgen_tmp_36_66._c1_poly, &_pgen_tmp_34_64._c1_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_37_67, &_preg_268435460, &_pgen_tmp_36_66);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_relu_tmp58_7, &_preg_268435459, &_pgen_tmp_37_67);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_relu_tmp58_7._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_37_67._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435460._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_36_66._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_37_67._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435460._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_36_66._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435459._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_37_67._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435459._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_37_67._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_38_68, &_relu_tmp58_7, &_relu_tmp58_7);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_38_68._c0_poly);
  Init_poly_by_size(&_pgen_tmp_39_69, 1, 0);
  Init_poly_by_size(&_pgen_tmp_40_70, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_39_69, 0, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_40_70, 0, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_38_68._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_38_68._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_39_69, 0, degree), Coeffs(&_pgen_tmp_40_70, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_38_68._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_39_69);
  Free_data(&_pgen_tmp_40_70);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_41_71, &_pgen_tmp_38_68);
  Rescale(&_pgen_tmp_41_71._c0_poly, &_pgen_tmp_38_68._c0_poly);
  Rescale(&_pgen_tmp_41_71._c1_poly, &_pgen_tmp_38_68._c1_poly);
  Rescale(&_pgen_tmp_41_71._c2_poly, &_pgen_tmp_38_68._c2_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435461 = Relinearize(_pgen_tmp_41_71);
  Copy_ciph(&_relu_tmp62_8, &_preg_268435461);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_42_72, &_relu_tmp62_8, &_relu_tmp62_8);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_43_73, &_cst_225, 1, Sc_degree(&_pgen_tmp_42_72), Level(&_pgen_tmp_42_72), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp68_9, &_pgen_tmp_42_72, &_pgen_tmp_43_73);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_relu_tmp68_9._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_42_72._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp62_8._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp62_8._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_42_72._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp62_8._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp62_8._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_42_72._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_43_73._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Set_coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_30, degree, Coeffs(&_pgen_tmp_42_72._c1_poly, _pgen_rns_idx_30, degree));
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(13, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_44_74, &_relu_tmp58_7, &_relu_tmp68_9);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_44_74._c0_poly);
  Init_poly_by_size(&_pgen_tmp_45_75, 1, 0);
  Init_poly_by_size(&_pgen_tmp_46_76, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_45_75, 0, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_46_76, 0, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_44_74._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_44_74._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_45_75, 0, degree), Coeffs(&_pgen_tmp_46_76, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_44_74._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_45_75);
  Free_data(&_pgen_tmp_46_76);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_47_77, &_pgen_tmp_44_74);
  Rescale(&_pgen_tmp_47_77._c0_poly, &_pgen_tmp_44_74._c0_poly);
  Rescale(&_pgen_tmp_47_77._c1_poly, &_pgen_tmp_44_74._c1_poly);
  Rescale(&_pgen_tmp_47_77._c2_poly, &_pgen_tmp_44_74._c2_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435462 = Relinearize(_pgen_tmp_47_77);
  Copy_ciph(&_relu_tmp72_10, &_preg_268435462);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_48_78, &_relu_tmp72_10, &_relu_tmp72_10);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_50_80, &_cst_226, 1, 1, Level(&_relu_tmp58_7), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_49_79, &_relu_tmp58_7, &_pgen_tmp_50_80);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_49_79._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_49_79._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_50_80._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_49_79._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_50_80._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_51_81, &_pgen_tmp_49_79);
  Rescale(&_pgen_tmp_51_81._c0_poly, &_pgen_tmp_49_79._c0_poly);
  Rescale(&_pgen_tmp_51_81._c1_poly, &_pgen_tmp_49_79._c1_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_relu_tmp80_11, &_pgen_tmp_48_78, &_pgen_tmp_51_81);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_relu_tmp80_11._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_48_78._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp72_10._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp72_10._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_48_78._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp72_10._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp72_10._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp80_11._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_48_78._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_51_81._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp80_11._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_48_78._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_51_81._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_52_82, &_relu_tmp68_9, &_relu_tmp68_9);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_52_82._c0_poly);
  Init_poly_by_size(&_pgen_tmp_53_83, 1, 0);
  Init_poly_by_size(&_pgen_tmp_54_84, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_53_83, 0, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_54_84, 0, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_52_82._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_52_82._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_53_83, 0, degree), Coeffs(&_pgen_tmp_54_84, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_52_82._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_53_83);
  Free_data(&_pgen_tmp_54_84);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_55_85, &_pgen_tmp_52_82);
  Rescale(&_pgen_tmp_55_85._c0_poly, &_pgen_tmp_52_82._c0_poly);
  Rescale(&_pgen_tmp_55_85._c1_poly, &_pgen_tmp_52_82._c1_poly);
  Rescale(&_pgen_tmp_55_85._c2_poly, &_pgen_tmp_52_82._c2_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435463 = Relinearize(_pgen_tmp_55_85);
  Copy_ciph(&_relu_tmp84_12, &_preg_268435463);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_56_86, &_relu_tmp84_12, &_relu_tmp84_12);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_57_87, &_cst_227, 1, Sc_degree(&_pgen_tmp_56_86), Level(&_pgen_tmp_56_86), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp90_13, &_pgen_tmp_56_86, &_pgen_tmp_57_87);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_relu_tmp90_13._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_56_86._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp84_12._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp84_12._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_56_86._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp84_12._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp84_12._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_56_86._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_57_87._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Set_coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_30, degree, Coeffs(&_pgen_tmp_56_86._c1_poly, _pgen_rns_idx_30, degree));
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(13, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_58_88, &_relu_tmp90_13, &_relu_tmp90_13);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_58_88._c0_poly);
  Init_poly_by_size(&_pgen_tmp_59_89, 1, 0);
  Init_poly_by_size(&_pgen_tmp_60_90, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_59_89, 0, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_60_90, 0, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_58_88._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_58_88._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_59_89, 0, degree), Coeffs(&_pgen_tmp_60_90, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_58_88._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_59_89);
  Free_data(&_pgen_tmp_60_90);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_61_91, &_pgen_tmp_58_88);
  Rescale(&_pgen_tmp_61_91._c0_poly, &_pgen_tmp_58_88._c0_poly);
  Rescale(&_pgen_tmp_61_91._c1_poly, &_pgen_tmp_58_88._c1_poly);
  Rescale(&_pgen_tmp_61_91._c2_poly, &_pgen_tmp_58_88._c2_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435464 = Relinearize(_pgen_tmp_61_91);
  Copy_ciph(&_relu_tmp94_14, &_preg_268435464);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_62_92, &_relu_tmp94_14, &_relu_tmp94_14);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_63_93, &_cst_228, 1, Sc_degree(&_pgen_tmp_62_92), Level(&_pgen_tmp_62_92), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp100_15, &_pgen_tmp_62_92, &_pgen_tmp_63_93);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_relu_tmp100_15._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_62_92._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp94_14._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp94_14._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_62_92._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp94_14._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp94_14._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp100_15._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_62_92._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_63_93._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Set_coeffs(&_relu_tmp100_15._c1_poly, _pgen_rns_idx_30, degree, Coeffs(&_pgen_tmp_62_92._c1_poly, _pgen_rns_idx_30, degree));
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(13, 0);
  Encode_float_mask(&_pgen_tmp_68_98, -0.0001600280537250875771867, mask_len_2, 1, Level(&_relu_tmp58_7), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_67_97, &_relu_tmp58_7, &_pgen_tmp_68_98);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_67_97._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_67_97._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_68_98._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_67_97._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_68_98._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_69_99, &_pgen_tmp_67_97);
  Rescale(&_pgen_tmp_69_99._c0_poly, &_pgen_tmp_67_97._c0_poly);
  Rescale(&_pgen_tmp_69_99._c1_poly, &_pgen_tmp_67_97._c1_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_66_96, &_relu_tmp68_9, &_pgen_tmp_69_99);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_66_96._c0_poly);
  Init_poly_by_size(&_pgen_tmp_70_100, 1, 0);
  Init_poly_by_size(&_pgen_tmp_71_101, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_70_100, 0, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_69_99._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_71_101, 0, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_69_99._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_66_96._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp68_9._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_69_99._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_66_96._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_70_100, 0, degree), Coeffs(&_pgen_tmp_71_101, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_66_96._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp68_9._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_69_99._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_70_100);
  Free_data(&_pgen_tmp_71_101);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_72_102, &_pgen_tmp_66_96);
  Rescale(&_pgen_tmp_72_102._c0_poly, &_pgen_tmp_66_96._c0_poly);
  Rescale(&_pgen_tmp_72_102._c1_poly, &_pgen_tmp_66_96._c1_poly);
  Rescale(&_pgen_tmp_72_102._c2_poly, &_pgen_tmp_66_96._c2_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435465 = Relinearize(_pgen_tmp_72_102);
  Encode_float_mask(&_pgen_tmp_74_104, 0.003032478474127557870749, mask_len_2, 1, Level(&_relu_tmp58_7), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_73_103, &_relu_tmp58_7, &_pgen_tmp_74_104);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_73_103._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_73_103._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_74_104._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_73_103._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_74_104._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_75_105, &_pgen_tmp_73_103);
  Rescale(&_pgen_tmp_75_105._c0_poly, &_pgen_tmp_73_103._c0_poly);
  Rescale(&_pgen_tmp_75_105._c1_poly, &_pgen_tmp_73_103._c1_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_76_106, &_preg_268435465, &_pgen_tmp_75_105);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_65_95, &_relu_tmp90_13, &_pgen_tmp_76_106);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_65_95._c0_poly);
  Init_poly_by_size(&_pgen_tmp_77_107, 1, 0);
  Init_poly_by_size(&_pgen_tmp_78_108, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_76_106._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435465._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_75_105._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_76_106._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435465._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_75_105._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_77_107, 0, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_76_106._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_78_108, 0, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_76_106._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_65_95._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_76_106._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_65_95._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_77_107, 0, degree), Coeffs(&_pgen_tmp_78_108, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_65_95._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_76_106._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_77_107);
  Free_data(&_pgen_tmp_78_108);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_79_109, &_pgen_tmp_65_95);
  Rescale(&_pgen_tmp_79_109._c0_poly, &_pgen_tmp_65_95._c0_poly);
  Rescale(&_pgen_tmp_79_109._c1_poly, &_pgen_tmp_65_95._c1_poly);
  Rescale(&_pgen_tmp_79_109._c2_poly, &_pgen_tmp_65_95._c2_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435466 = Relinearize(_pgen_tmp_79_109);
  Encode_double(&_pgen_tmp_81_111, &_cst_235, 1, 1, Level(&_relu_tmp80_11), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_80_110, &_relu_tmp80_11, &_pgen_tmp_81_111);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_80_110._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_80_110._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp80_11._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_81_111._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_80_110._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp80_11._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_81_111._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_82_112, &_pgen_tmp_80_110);
  Rescale(&_pgen_tmp_82_112._c0_poly, &_pgen_tmp_80_110._c0_poly);
  Rescale(&_pgen_tmp_82_112._c1_poly, &_pgen_tmp_80_110._c1_poly);
  CMPLR_TM_END(19, 0);
  Encode_float_mask(&_pgen_tmp_84_114, 0.1365828361194337536855, mask_len_2, 1, Level(&_relu_tmp58_7), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_83_113, &_relu_tmp58_7, &_pgen_tmp_84_114);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_83_113._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_83_113._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_84_114._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_83_113._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_84_114._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_85_115, &_pgen_tmp_83_113);
  Rescale(&_pgen_tmp_85_115._c0_poly, &_pgen_tmp_83_113._c0_poly);
  Rescale(&_pgen_tmp_85_115._c1_poly, &_pgen_tmp_83_113._c1_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_86_116, &_pgen_tmp_82_112, &_pgen_tmp_85_115);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_87_117, &_preg_268435466, &_pgen_tmp_86_116);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_64_94, &_relu_tmp100_15, &_pgen_tmp_87_117);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_64_94._c0_poly);
  Init_poly_by_size(&_pgen_tmp_88_118, 1, 0);
  Init_poly_by_size(&_pgen_tmp_89_119, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_86_116._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_82_112._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_85_115._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_86_116._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_82_112._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_85_115._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_87_117._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435466._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_86_116._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_87_117._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435466._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_86_116._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_88_118, 0, degree), Coeffs(&_relu_tmp100_15._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_87_117._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_89_119, 0, degree), Coeffs(&_relu_tmp100_15._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_87_117._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_64_94._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp100_15._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_87_117._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_64_94._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_88_118, 0, degree), Coeffs(&_pgen_tmp_89_119, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_64_94._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp100_15._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_87_117._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_88_118);
  Free_data(&_pgen_tmp_89_119);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_90_120, &_pgen_tmp_64_94);
  Rescale(&_pgen_tmp_90_120._c0_poly, &_pgen_tmp_64_94._c0_poly);
  Rescale(&_pgen_tmp_90_120._c1_poly, &_pgen_tmp_64_94._c1_poly);
  Rescale(&_pgen_tmp_90_120._c2_poly, &_pgen_tmp_64_94._c2_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435467 = Relinearize(_pgen_tmp_90_120);
  Encode_double(&_pgen_tmp_93_123, &_cst_238, 1, 1, Level(&_relu_tmp80_11), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_92_122, &_relu_tmp80_11, &_pgen_tmp_93_123);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_92_122._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_92_122._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp80_11._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_93_123._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_92_122._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp80_11._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_93_123._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_94_124, &_pgen_tmp_92_122);
  Rescale(&_pgen_tmp_94_124._c0_poly, &_pgen_tmp_92_122._c0_poly);
  Rescale(&_pgen_tmp_94_124._c1_poly, &_pgen_tmp_92_122._c1_poly);
  CMPLR_TM_END(19, 0);
  Encode_float_mask(&_pgen_tmp_96_126, 0.5685231134608953462717, mask_len_2, 1, Level(&_relu_tmp58_7), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_95_125, &_relu_tmp58_7, &_pgen_tmp_96_126);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_95_125._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_95_125._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_96_126._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_95_125._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_96_126._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_97_127, &_pgen_tmp_95_125);
  Rescale(&_pgen_tmp_97_127._c0_poly, &_pgen_tmp_95_125._c0_poly);
  Rescale(&_pgen_tmp_97_127._c1_poly, &_pgen_tmp_95_125._c1_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_98_128, &_pgen_tmp_94_124, &_pgen_tmp_97_127);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_91_121, &_relu_tmp90_13, &_pgen_tmp_98_128);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_91_121._c0_poly);
  Init_poly_by_size(&_pgen_tmp_99_129, 1, 0);
  Init_poly_by_size(&_pgen_tmp_100_130, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_98_128._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_94_124._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_97_127._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_98_128._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_94_124._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_97_127._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_99_129, 0, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_98_128._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_100_130, 0, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_98_128._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_91_121._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp90_13._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_98_128._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_91_121._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_99_129, 0, degree), Coeffs(&_pgen_tmp_100_130, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_91_121._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp90_13._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_98_128._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_99_129);
  Free_data(&_pgen_tmp_100_130);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_101_131, &_pgen_tmp_91_121);
  Rescale(&_pgen_tmp_101_131._c0_poly, &_pgen_tmp_91_121._c0_poly);
  Rescale(&_pgen_tmp_101_131._c1_poly, &_pgen_tmp_91_121._c1_poly);
  Rescale(&_pgen_tmp_101_131._c2_poly, &_pgen_tmp_91_121._c2_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435468 = Relinearize(_pgen_tmp_101_131);
  Encode_double(&_pgen_tmp_103_133, &_cst_241, 1, 1, Level(&_relu_tmp80_11), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_102_132, &_relu_tmp80_11, &_pgen_tmp_103_133);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_102_132._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_102_132._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp80_11._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_103_133._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_102_132._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp80_11._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_103_133._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_104_134, &_pgen_tmp_102_132);
  Rescale(&_pgen_tmp_104_134._c0_poly, &_pgen_tmp_102_132._c0_poly);
  Rescale(&_pgen_tmp_104_134._c1_poly, &_pgen_tmp_102_132._c1_poly);
  CMPLR_TM_END(19, 0);
  Encode_float_mask(&_pgen_tmp_106_136, 1.614771079042867985009, mask_len_2, 1, Level(&_relu_tmp58_7), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_105_135, &_relu_tmp58_7, &_pgen_tmp_106_136);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_105_135._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_105_135._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_106_136._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_105_135._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp58_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_106_136._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_107_137, &_pgen_tmp_105_135);
  Rescale(&_pgen_tmp_107_137._c0_poly, &_pgen_tmp_105_135._c0_poly);
  Rescale(&_pgen_tmp_107_137._c1_poly, &_pgen_tmp_105_135._c1_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_108_138, &_pgen_tmp_104_134, &_pgen_tmp_107_137);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_109_139, &_preg_268435468, &_pgen_tmp_108_138);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_relu_tmp155_16, &_preg_268435467, &_pgen_tmp_109_139);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_relu_tmp155_16._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_108_138._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_104_134._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_107_137._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_108_138._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_104_134._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_107_137._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_109_139._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435468._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_108_138._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_109_139._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435468._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_108_138._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435467._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_109_139._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435467._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_109_139._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_110_140, &_relu_tmp155_16, &_relu_tmp155_16);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_110_140._c0_poly);
  Init_poly_by_size(&_pgen_tmp_111_141, 1, 0);
  Init_poly_by_size(&_pgen_tmp_112_142, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_111_141, 0, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_112_142, 0, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_110_140._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_110_140._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_111_141, 0, degree), Coeffs(&_pgen_tmp_112_142, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_110_140._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_111_141);
  Free_data(&_pgen_tmp_112_142);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_113_143, &_pgen_tmp_110_140);
  Rescale(&_pgen_tmp_113_143._c0_poly, &_pgen_tmp_110_140._c0_poly);
  Rescale(&_pgen_tmp_113_143._c1_poly, &_pgen_tmp_110_140._c1_poly);
  Rescale(&_pgen_tmp_113_143._c2_poly, &_pgen_tmp_110_140._c2_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435469 = Relinearize(_pgen_tmp_113_143);
  Copy_ciph(&_relu_tmp159_17, &_preg_268435469);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_114_144, &_relu_tmp159_17, &_relu_tmp159_17);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_115_145, &_cst_242, 1, Sc_degree(&_pgen_tmp_114_144), Level(&_pgen_tmp_114_144), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp165_18, &_pgen_tmp_114_144, &_pgen_tmp_115_145);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_relu_tmp165_18._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_114_144._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp159_17._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp159_17._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_114_144._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp159_17._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp159_17._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp165_18._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_114_144._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_115_145._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Set_coeffs(&_relu_tmp165_18._c1_poly, _pgen_rns_idx_30, degree, Coeffs(&_pgen_tmp_114_144._c1_poly, _pgen_rns_idx_30, degree));
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(13, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_116_146, &_relu_tmp155_16, &_relu_tmp165_18);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_116_146._c0_poly);
  Init_poly_by_size(&_pgen_tmp_117_147, 1, 0);
  Init_poly_by_size(&_pgen_tmp_118_148, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_117_147, 0, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp165_18._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_118_148, 0, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp165_18._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_116_146._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp165_18._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_116_146._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_117_147, 0, degree), Coeffs(&_pgen_tmp_118_148, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_116_146._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp165_18._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_117_147);
  Free_data(&_pgen_tmp_118_148);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_119_149, &_pgen_tmp_116_146);
  Rescale(&_pgen_tmp_119_149._c0_poly, &_pgen_tmp_116_146._c0_poly);
  Rescale(&_pgen_tmp_119_149._c1_poly, &_pgen_tmp_116_146._c1_poly);
  Rescale(&_pgen_tmp_119_149._c2_poly, &_pgen_tmp_116_146._c2_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435470 = Relinearize(_pgen_tmp_119_149);
  Copy_ciph(&_relu_tmp169_19, &_preg_268435470);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_120_150, &_relu_tmp169_19, &_relu_tmp169_19);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_122_152, &_cst_243, 1, 1, Level(&_relu_tmp155_16), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_121_151, &_relu_tmp155_16, &_pgen_tmp_122_152);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_121_151._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_121_151._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_122_152._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_121_151._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_122_152._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_123_153, &_pgen_tmp_121_151);
  Rescale(&_pgen_tmp_123_153._c0_poly, &_pgen_tmp_121_151._c0_poly);
  Rescale(&_pgen_tmp_123_153._c1_poly, &_pgen_tmp_121_151._c1_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_relu_tmp177_20, &_pgen_tmp_120_150, &_pgen_tmp_123_153);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_relu_tmp177_20._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_120_150._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp169_19._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp169_19._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_120_150._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp169_19._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp169_19._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp177_20._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_120_150._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_123_153._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp177_20._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_120_150._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_123_153._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_124_154, &_relu_tmp165_18, &_relu_tmp165_18);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_124_154._c0_poly);
  Init_poly_by_size(&_pgen_tmp_125_155, 1, 0);
  Init_poly_by_size(&_pgen_tmp_126_156, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_125_155, 0, degree), Coeffs(&_relu_tmp165_18._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp165_18._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_126_156, 0, degree), Coeffs(&_relu_tmp165_18._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp165_18._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_124_154._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp165_18._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp165_18._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_124_154._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_125_155, 0, degree), Coeffs(&_pgen_tmp_126_156, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_124_154._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp165_18._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp165_18._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_125_155);
  Free_data(&_pgen_tmp_126_156);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_127_157, &_pgen_tmp_124_154);
  Rescale(&_pgen_tmp_127_157._c0_poly, &_pgen_tmp_124_154._c0_poly);
  Rescale(&_pgen_tmp_127_157._c1_poly, &_pgen_tmp_124_154._c1_poly);
  Rescale(&_pgen_tmp_127_157._c2_poly, &_pgen_tmp_124_154._c2_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435471 = Relinearize(_pgen_tmp_127_157);
  Copy_ciph(&_relu_tmp181_21, &_preg_268435471);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_128_158, &_relu_tmp181_21, &_relu_tmp181_21);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_129_159, &_cst_244, 1, Sc_degree(&_pgen_tmp_128_158), Level(&_pgen_tmp_128_158), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp187_22, &_pgen_tmp_128_158, &_pgen_tmp_129_159);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_relu_tmp187_22._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_128_158._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp181_21._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp181_21._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_128_158._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp181_21._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp181_21._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_128_158._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_129_159._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Set_coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_30, degree, Coeffs(&_pgen_tmp_128_158._c1_poly, _pgen_rns_idx_30, degree));
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(13, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_130_160, &_relu_tmp187_22, &_relu_tmp187_22);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_130_160._c0_poly);
  Init_poly_by_size(&_pgen_tmp_131_161, 1, 0);
  Init_poly_by_size(&_pgen_tmp_132_162, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_131_161, 0, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_132_162, 0, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_130_160._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_130_160._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_131_161, 0, degree), Coeffs(&_pgen_tmp_132_162, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_130_160._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_131_161);
  Free_data(&_pgen_tmp_132_162);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_133_163, &_pgen_tmp_130_160);
  Rescale(&_pgen_tmp_133_163._c0_poly, &_pgen_tmp_130_160._c0_poly);
  Rescale(&_pgen_tmp_133_163._c1_poly, &_pgen_tmp_130_160._c1_poly);
  Rescale(&_pgen_tmp_133_163._c2_poly, &_pgen_tmp_130_160._c2_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435472 = Relinearize(_pgen_tmp_133_163);
  Copy_ciph(&_relu_tmp191_23, &_preg_268435472);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_134_164, &_relu_tmp191_23, &_relu_tmp191_23);
  CMPLR_TM_END(12, 0);
  Encode_float(&_pgen_tmp_135_165, &_cst_245, 1, Sc_degree(&_pgen_tmp_134_164), Level(&_pgen_tmp_134_164), 0);
  CMPLR_TM_START(13, 0);
  Init_ciph_same_scale_plain(&_relu_tmp197_24, &_pgen_tmp_134_164, &_pgen_tmp_135_165);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_relu_tmp197_24._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_134_164._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp191_23._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp191_23._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_134_164._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp191_23._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp191_23._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp197_24._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_134_164._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_135_165._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Set_coeffs(&_relu_tmp197_24._c1_poly, _pgen_rns_idx_30, degree, Coeffs(&_pgen_tmp_134_164._c1_poly, _pgen_rns_idx_30, degree));
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(13, 0);
  Encode_float_mask(&_pgen_tmp_140_170, 0.0001111119162030077897477, mask_len_2, 1, Level(&_relu_tmp155_16), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_139_169, &_relu_tmp155_16, &_pgen_tmp_140_170);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_139_169._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_139_169._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_140_170._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_139_169._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_140_170._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_141_171, &_pgen_tmp_139_169);
  Rescale(&_pgen_tmp_141_171._c0_poly, &_pgen_tmp_139_169._c0_poly);
  Rescale(&_pgen_tmp_141_171._c1_poly, &_pgen_tmp_139_169._c1_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_138_168, &input_ct0_0, &_pgen_tmp_141_171);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_138_168._c0_poly);
  Init_poly_by_size(&_pgen_tmp_142_172, 1, 0);
  Init_poly_by_size(&_pgen_tmp_143_173, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_142_172, 0, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_141_171._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_143_173, 0, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_141_171._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_138_168._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_141_171._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_138_168._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_142_172, 0, degree), Coeffs(&_pgen_tmp_143_173, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_138_168._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_141_171._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_142_172);
  Free_data(&_pgen_tmp_143_173);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_144_174, &_pgen_tmp_138_168);
  Rescale(&_pgen_tmp_144_174._c0_poly, &_pgen_tmp_138_168._c0_poly);
  Rescale(&_pgen_tmp_144_174._c1_poly, &_pgen_tmp_138_168._c1_poly);
  Rescale(&_pgen_tmp_144_174._c2_poly, &_pgen_tmp_138_168._c2_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435473 = Relinearize(_pgen_tmp_144_174);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_137_167, &_relu_tmp187_22, &_preg_268435473);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_137_167._c0_poly);
  Init_poly_by_size(&_pgen_tmp_145_175, 1, 0);
  Init_poly_by_size(&_pgen_tmp_146_176, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_145_175, 0, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435473._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_146_176, 0, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435473._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_137_167._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435473._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_137_167._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_145_175, 0, degree), Coeffs(&_pgen_tmp_146_176, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_137_167._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435473._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_145_175);
  Free_data(&_pgen_tmp_146_176);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_147_177, &_pgen_tmp_137_167);
  Rescale(&_pgen_tmp_147_177._c0_poly, &_pgen_tmp_137_167._c0_poly);
  Rescale(&_pgen_tmp_147_177._c1_poly, &_pgen_tmp_137_167._c1_poly);
  Rescale(&_pgen_tmp_147_177._c2_poly, &_pgen_tmp_137_167._c2_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435474 = Relinearize(_pgen_tmp_147_177);
  Encode_float_mask(&_pgen_tmp_150_180, -0.0009371069270347270278171, mask_len_2, 1, Level(&input_ct0_0), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_149_179, &input_ct0_0, &_pgen_tmp_150_180);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_149_179._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_149_179._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_150_180._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_149_179._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_150_180._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_151_181, &_pgen_tmp_149_179);
  Rescale(&_pgen_tmp_151_181._c0_poly, &_pgen_tmp_149_179._c0_poly);
  Rescale(&_pgen_tmp_151_181._c1_poly, &_pgen_tmp_149_179._c1_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_148_178, &_relu_tmp177_20, &_pgen_tmp_151_181);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_148_178._c0_poly);
  Init_poly_by_size(&_pgen_tmp_152_182, 1, 0);
  Init_poly_by_size(&_pgen_tmp_153_183, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_152_182, 0, degree), Coeffs(&_relu_tmp177_20._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_151_181._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_153_183, 0, degree), Coeffs(&_relu_tmp177_20._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_151_181._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_148_178._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp177_20._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_151_181._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_148_178._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_152_182, 0, degree), Coeffs(&_pgen_tmp_153_183, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_148_178._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp177_20._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_151_181._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_152_182);
  Free_data(&_pgen_tmp_153_183);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_154_184, &_pgen_tmp_148_178);
  Rescale(&_pgen_tmp_154_184._c0_poly, &_pgen_tmp_148_178._c0_poly);
  Rescale(&_pgen_tmp_154_184._c1_poly, &_pgen_tmp_148_178._c1_poly);
  Rescale(&_pgen_tmp_154_184._c2_poly, &_pgen_tmp_148_178._c2_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435475 = Relinearize(_pgen_tmp_154_184);
  Encode_float_mask(&_pgen_tmp_157_187, 0.006507995190210730772351, mask_len_2, 1, Level(&_relu_tmp155_16), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_156_186, &_relu_tmp155_16, &_pgen_tmp_157_187);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_156_186._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_156_186._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_157_187._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_156_186._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_157_187._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_158_188, &_pgen_tmp_156_186);
  Rescale(&_pgen_tmp_158_188._c0_poly, &_pgen_tmp_156_186._c0_poly);
  Rescale(&_pgen_tmp_158_188._c1_poly, &_pgen_tmp_156_186._c1_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_155_185, &input_ct0_0, &_pgen_tmp_158_188);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_155_185._c0_poly);
  Init_poly_by_size(&_pgen_tmp_159_189, 1, 0);
  Init_poly_by_size(&_pgen_tmp_160_190, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_159_189, 0, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_158_188._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_160_190, 0, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_158_188._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_155_185._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_158_188._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_155_185._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_159_189, 0, degree), Coeffs(&_pgen_tmp_160_190, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_155_185._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_158_188._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_159_189);
  Free_data(&_pgen_tmp_160_190);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_161_191, &_pgen_tmp_155_185);
  Rescale(&_pgen_tmp_161_191._c0_poly, &_pgen_tmp_155_185._c0_poly);
  Rescale(&_pgen_tmp_161_191._c1_poly, &_pgen_tmp_155_185._c1_poly);
  Rescale(&_pgen_tmp_161_191._c2_poly, &_pgen_tmp_155_185._c2_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435476 = Relinearize(_pgen_tmp_161_191);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_162_192, &_preg_268435475, &_preg_268435476);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_163_193, &_preg_268435474, &_pgen_tmp_162_192);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_136_166, &_relu_tmp197_24, &_pgen_tmp_163_193);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_136_166._c0_poly);
  Init_poly_by_size(&_pgen_tmp_164_194, 1, 0);
  Init_poly_by_size(&_pgen_tmp_165_195, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_162_192._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435475._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435476._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_162_192._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435475._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435476._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_163_193._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435474._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_162_192._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_163_193._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435474._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_162_192._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_164_194, 0, degree), Coeffs(&_relu_tmp197_24._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_163_193._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_165_195, 0, degree), Coeffs(&_relu_tmp197_24._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_163_193._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_136_166._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp197_24._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_163_193._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_136_166._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_164_194, 0, degree), Coeffs(&_pgen_tmp_165_195, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_136_166._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp197_24._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_163_193._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_164_194);
  Free_data(&_pgen_tmp_165_195);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_166_196, &_pgen_tmp_136_166);
  Rescale(&_pgen_tmp_166_196._c0_poly, &_pgen_tmp_136_166._c0_poly);
  Rescale(&_pgen_tmp_166_196._c1_poly, &_pgen_tmp_136_166._c1_poly);
  Rescale(&_pgen_tmp_166_196._c2_poly, &_pgen_tmp_136_166._c2_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435477 = Relinearize(_pgen_tmp_166_196);
  Encode_float_mask(&_pgen_tmp_170_200, -0.03691063940349948585551, mask_len_2, 1, Level(&input_ct0_0), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_169_199, &input_ct0_0, &_pgen_tmp_170_200);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_169_199._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_169_199._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_170_200._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_169_199._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_170_200._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_171_201, &_pgen_tmp_169_199);
  Rescale(&_pgen_tmp_171_201._c0_poly, &_pgen_tmp_169_199._c0_poly);
  Rescale(&_pgen_tmp_171_201._c1_poly, &_pgen_tmp_169_199._c1_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_168_198, &_relu_tmp177_20, &_pgen_tmp_171_201);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_168_198._c0_poly);
  Init_poly_by_size(&_pgen_tmp_172_202, 1, 0);
  Init_poly_by_size(&_pgen_tmp_173_203, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_172_202, 0, degree), Coeffs(&_relu_tmp177_20._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_171_201._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_173_203, 0, degree), Coeffs(&_relu_tmp177_20._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_171_201._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_168_198._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp177_20._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_171_201._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_168_198._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_172_202, 0, degree), Coeffs(&_pgen_tmp_173_203, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_168_198._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp177_20._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_171_201._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_172_202);
  Free_data(&_pgen_tmp_173_203);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_174_204, &_pgen_tmp_168_198);
  Rescale(&_pgen_tmp_174_204._c0_poly, &_pgen_tmp_168_198._c0_poly);
  Rescale(&_pgen_tmp_174_204._c1_poly, &_pgen_tmp_168_198._c1_poly);
  Rescale(&_pgen_tmp_174_204._c2_poly, &_pgen_tmp_168_198._c2_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435478 = Relinearize(_pgen_tmp_174_204);
  Encode_float_mask(&_pgen_tmp_177_207, 0.1056744718173614761003, mask_len_2, 1, Level(&_relu_tmp155_16), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_176_206, &_relu_tmp155_16, &_pgen_tmp_177_207);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_176_206._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_176_206._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_177_207._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_176_206._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_177_207._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_178_208, &_pgen_tmp_176_206);
  Rescale(&_pgen_tmp_178_208._c0_poly, &_pgen_tmp_176_206._c0_poly);
  Rescale(&_pgen_tmp_178_208._c1_poly, &_pgen_tmp_176_206._c1_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_175_205, &input_ct0_0, &_pgen_tmp_178_208);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_175_205._c0_poly);
  Init_poly_by_size(&_pgen_tmp_179_209, 1, 0);
  Init_poly_by_size(&_pgen_tmp_180_210, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_179_209, 0, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_178_208._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_180_210, 0, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_178_208._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_175_205._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_178_208._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_175_205._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_179_209, 0, degree), Coeffs(&_pgen_tmp_180_210, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_175_205._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_178_208._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_179_209);
  Free_data(&_pgen_tmp_180_210);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_181_211, &_pgen_tmp_175_205);
  Rescale(&_pgen_tmp_181_211._c0_poly, &_pgen_tmp_175_205._c0_poly);
  Rescale(&_pgen_tmp_181_211._c1_poly, &_pgen_tmp_175_205._c1_poly);
  Rescale(&_pgen_tmp_181_211._c2_poly, &_pgen_tmp_175_205._c2_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435479 = Relinearize(_pgen_tmp_181_211);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_182_212, &_preg_268435478, &_preg_268435479);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_167_197, &_relu_tmp187_22, &_pgen_tmp_182_212);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_167_197._c0_poly);
  Init_poly_by_size(&_pgen_tmp_183_213, 1, 0);
  Init_poly_by_size(&_pgen_tmp_184_214, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_182_212._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435478._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435479._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_182_212._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435478._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435479._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_183_213, 0, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_182_212._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_184_214, 0, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_182_212._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_167_197._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp187_22._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_182_212._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_167_197._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_183_213, 0, degree), Coeffs(&_pgen_tmp_184_214, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_167_197._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp187_22._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_182_212._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_183_213);
  Free_data(&_pgen_tmp_184_214);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_185_215, &_pgen_tmp_167_197);
  Rescale(&_pgen_tmp_185_215._c0_poly, &_pgen_tmp_167_197._c0_poly);
  Rescale(&_pgen_tmp_185_215._c1_poly, &_pgen_tmp_167_197._c1_poly);
  Rescale(&_pgen_tmp_185_215._c2_poly, &_pgen_tmp_167_197._c2_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435480 = Relinearize(_pgen_tmp_185_215);
  Encode_float_mask(&_pgen_tmp_188_218, -0.2078597158810965850329, mask_len_2, 1, Level(&input_ct0_0), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_187_217, &input_ct0_0, &_pgen_tmp_188_218);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_187_217._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_187_217._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_188_218._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_187_217._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_188_218._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_189_219, &_pgen_tmp_187_217);
  Rescale(&_pgen_tmp_189_219._c0_poly, &_pgen_tmp_187_217._c0_poly);
  Rescale(&_pgen_tmp_189_219._c1_poly, &_pgen_tmp_187_217._c1_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_186_216, &_relu_tmp177_20, &_pgen_tmp_189_219);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_186_216._c0_poly);
  Init_poly_by_size(&_pgen_tmp_190_220, 1, 0);
  Init_poly_by_size(&_pgen_tmp_191_221, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_190_220, 0, degree), Coeffs(&_relu_tmp177_20._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_189_219._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_191_221, 0, degree), Coeffs(&_relu_tmp177_20._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_189_219._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_186_216._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp177_20._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_189_219._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_186_216._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_190_220, 0, degree), Coeffs(&_pgen_tmp_191_221, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_186_216._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp177_20._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_189_219._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_190_220);
  Free_data(&_pgen_tmp_191_221);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_192_222, &_pgen_tmp_186_216);
  Rescale(&_pgen_tmp_192_222._c0_poly, &_pgen_tmp_186_216._c0_poly);
  Rescale(&_pgen_tmp_192_222._c1_poly, &_pgen_tmp_186_216._c1_poly);
  Rescale(&_pgen_tmp_192_222._c2_poly, &_pgen_tmp_186_216._c2_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435481 = Relinearize(_pgen_tmp_192_222);
  Encode_float_mask(&_pgen_tmp_195_225, 0.6334139843709288841822, mask_len_2, 1, Level(&_relu_tmp155_16), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_194_224, &_relu_tmp155_16, &_pgen_tmp_195_225);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_194_224._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_194_224._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_195_225._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_194_224._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp155_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_195_225._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_196_226, &_pgen_tmp_194_224);
  Rescale(&_pgen_tmp_196_226._c0_poly, &_pgen_tmp_194_224._c0_poly);
  Rescale(&_pgen_tmp_196_226._c1_poly, &_pgen_tmp_194_224._c1_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(16, 0);
  Init_ciph3_up_scale(&_pgen_tmp_193_223, &input_ct0_0, &_pgen_tmp_196_226);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_193_223._c0_poly);
  Init_poly_by_size(&_pgen_tmp_197_227, 1, 0);
  Init_poly_by_size(&_pgen_tmp_198_228, 1, 0);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_197_227, 0, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_196_226._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_198_228, 0, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_196_226._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_193_223._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_196_226._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_193_223._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_197_227, 0, degree), Coeffs(&_pgen_tmp_198_228, 0, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_193_223._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_196_226._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  Free_data(&_pgen_tmp_197_227);
  Free_data(&_pgen_tmp_198_228);
  CMPLR_TM_END(16, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph3_down_scale(&_pgen_tmp_199_229, &_pgen_tmp_193_223);
  Rescale(&_pgen_tmp_199_229._c0_poly, &_pgen_tmp_193_223._c0_poly);
  Rescale(&_pgen_tmp_199_229._c1_poly, &_pgen_tmp_193_223._c1_poly);
  Rescale(&_pgen_tmp_199_229._c2_poly, &_pgen_tmp_193_223._c2_poly);
  CMPLR_TM_END(19, 0);
  _preg_268435482 = Relinearize(_pgen_tmp_199_229);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_200_230, &_preg_268435481, &_preg_268435482);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_pgen_tmp_201_231, &_preg_268435480, &_pgen_tmp_200_230);
  CMPLR_TM_END(12, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_relu_tmp266_25, &_preg_268435477, &_pgen_tmp_201_231);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_relu_tmp266_25._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_pgen_tmp_200_230._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435481._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435482._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_200_230._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435481._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435482._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_201_231._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435480._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_200_230._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_pgen_tmp_201_231._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435480._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_200_230._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp266_25._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435477._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_201_231._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp266_25._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435477._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_201_231._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(12, 0);
  Encode_float_mask(&_pgen_tmp_203_233, 0.5, mask_len_2, 1, Level(&input_ct0_0), 0);
  CMPLR_TM_START(17, 0);
  Init_ciph_up_scale_plain(&_pgen_tmp_202_232, &input_ct0_0, &_pgen_tmp_203_233);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_pgen_tmp_202_232._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modmul(Coeffs(&_pgen_tmp_202_232._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_203_233._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modmul(Coeffs(&_pgen_tmp_202_232._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_203_233._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
  CMPLR_TM_END(17, 0);
  CMPLR_TM_START(19, 0);
  Init_ciph_down_scale(&_pgen_tmp_204_234, &_pgen_tmp_202_232);
  Rescale(&_pgen_tmp_204_234._c0_poly, &_pgen_tmp_202_232._c0_poly);
  Rescale(&_pgen_tmp_204_234._c1_poly, &_pgen_tmp_202_232._c1_poly);
  CMPLR_TM_END(19, 0);
  CMPLR_TM_START(12, 0);
  Init_ciph_same_scale(&_relu_tmp274_26, &_relu_tmp266_25, &_pgen_tmp_204_234);
  _pgen_modulus_28 = Q_modulus();
  _pgen_num_q_29 = Poly_level(&_relu_tmp274_26._c0_poly);
  for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
    Hw_modadd(Coeffs(&_relu_tmp274_26._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp266_25._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_204_234._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    Hw_modadd(Coeffs(&_relu_tmp274_26._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp266_25._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_204_234._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
    _pgen_modulus_28 = _pgen_modulus_28 + 1;
  }
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
  Free_data(&_pgen_swk_c1_4);
  Free_data(&_pgen_ext_5);
  Free_data(&_pgen_tmp_poly_6);
  Free_data(&_pgen_mod_down_c0_7);
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
  Free_data(&_pgen_swk_c1_3);
  Free_data(&_pgen_ext_4);
  Free_data(&_pgen_tmp_poly_5);
  Free_data(&_pgen_mod_down_c0_6);
  Free_data(&_pgen_mod_down_c1_7);
  RTLIB_TM_END(18, rtm);
  return _pgen_relin_res_1;
}

CKKS_PARAMS* Get_context_params() {
  static CKKS_PARAMS parm = {
    LIB_ANT, 131072, 0, 33, 30, 60, 56, 3, 192, 910, 
    { -65536, -61440, -57344, -53248, -49152, -45056, -40960, -36864,
      -32768, -28672, -24576, -20480, -16384, -12288, -8192, -4096,
      -1024, -512, -33, -32, -31, -17, -16, -15,
      -9, -8, -7, -1, 0, 1, 2, 3,
      4, 5, 6, 7, 8, 9, 10, 11,
      12, 13, 14, 15, 16, 17, 24, 31,
      32, 33, 48, 63, 64, 72, 96, 120,
      126, 128, 144, 168, 189, 192, 240, 252,
      256, 288, 315, 320, 336, 378, 384, 432,
      441, 448, 480, 504, 512, 520, 528, 567,
      576, 624, 630, 640, 672, 693, 704, 720,
      756, 768, 819, 832, 882, 896, 945, 960,
      1008, 1024, 1071, 1088, 1134, 1152, 1197, 1216,
      1260, 1280, 1323, 1344, 1386, 1408, 1449, 1472,
      1512, 1536, 1575, 1600, 1638, 1664, 1701, 1728,
      1764, 1792, 1827, 1856, 1890, 1920, 1953, 1984,
      2016, 2048, 2079, 2112, 2142, 2176, 2205, 2240,
      2268, 2304, 2331, 2368, 2394, 2432, 2457, 2496,
      2520, 2560, 2583, 2624, 2646, 2688, 2709, 2752,
      2772, 2816, 2835, 2880, 2898, 2944, 2961, 3008,
      3024, 3072, 3087, 3136, 3150, 3200, 3213, 3264,
      3276, 3328, 3339, 3392, 3402, 3456, 3465, 3520,
      3528, 3584, 3591, 3648, 3654, 3712, 3717, 3776,
      3780, 3840, 3843, 3904, 3906, 3968, 3969, 4032,
      4095, 4096, 4158, 4221, 4224, 4284, 4347, 4352,
      4410, 4416, 4473, 4536, 4599, 4608, 4662, 4725,
      4788, 4800, 4851, 4864, 4914, 4977, 4992, 5040,
      5103, 5120, 5166, 5184, 5229, 5292, 5355, 5376,
      5418, 5481, 5544, 5568, 5607, 5632, 5670, 5733,
      5760, 5796, 5859, 5888, 5922, 5952, 5985, 6048,
      6111, 6144, 6174, 6237, 6300, 6336, 6363, 6400,
      6426, 6489, 6528, 6552, 6615, 6656, 6678, 6720,
      6741, 6804, 6867, 6912, 6930, 6993, 7056, 7104,
      7119, 7168, 7182, 7245, 7296, 7308, 7371, 7424,
      7434, 7488, 7497, 7560, 7623, 7680, 7686, 7749,
      7812, 7872, 7875, 7936, 7938, 8001, 8064, 8127,
      8190, 8192, 8253, 8256, 8316, 8379, 8442, 8448,
      8505, 8568, 8631, 8640, 8694, 8704, 8757, 8820,
      8832, 8883, 8946, 8960, 9009, 9024, 9072, 9135,
      9198, 9216, 9261, 9324, 9387, 9408, 9450, 9472,
      9513, 9576, 9600, 9639, 9702, 9728, 9765, 9792,
      9828, 9891, 9954, 9984, 10017, 10080, 10143, 10176,
      10206, 10240, 10269, 10332, 10368, 10395, 10458, 10496,
      10521, 10560, 10584, 10647, 10710, 10752, 10773, 10836,
      10899, 10944, 10962, 11008, 11025, 11088, 11136, 11151,
      11214, 11264, 11277, 11328, 11340, 11403, 11466, 11520,
      11529, 11592, 11655, 11712, 11718, 11776, 11781, 11844,
      11904, 11907, 11970, 12032, 12033, 12096, 12159, 12222,
      12285, 12288, 12348, 12411, 12474, 12480, 12537, 12544,
      12600, 12663, 12672, 12726, 12789, 12800, 12852, 12864,
      12915, 12978, 13041, 13056, 13104, 13167, 13230, 13248,
      13293, 13312, 13356, 13419, 13440, 13482, 13545, 13568,
      13608, 13632, 13671, 13734, 13797, 13824, 13860, 13923,
      13986, 14016, 14049, 14080, 14112, 14175, 14208, 14238,
      14301, 14336, 14364, 14400, 14427, 14490, 14553, 14592,
      14616, 14679, 14742, 14784, 14805, 14848, 14868, 14931,
      14976, 14994, 15057, 15104, 15120, 15168, 15183, 15246,
      15309, 15360, 15372, 15435, 15498, 15552, 15561, 15616,
      15624, 15687, 15744, 15750, 15813, 15872, 15876, 15936,
      15939, 16002, 16065, 16128, 16191, 16254, 16317, 16320,
      16380, 16384, 16443, 16506, 16512, 16569, 16632, 16695,
      16704, 16758, 16821, 16884, 16896, 16947, 17010, 17073,
      17088, 17136, 17199, 17262, 17280, 17325, 17388, 17408,
      17451, 17472, 17514, 17577, 17640, 17664, 17703, 17766,
      17829, 17856, 17892, 17955, 18018, 18048, 18081, 18144,
      18207, 18240, 18270, 18333, 18396, 18432, 18459, 18522,
      18585, 18624, 18648, 18711, 18774, 18816, 18837, 18900,
      18963, 19008, 19026, 19089, 19152, 19200, 19215, 19278,
      19341, 19392, 19404, 19456, 19467, 19530, 19584, 19593,
      19656, 19719, 19776, 19782, 19845, 19908, 19968, 19971,
      20034, 20097, 20160, 20223, 20286, 20349, 20352, 20412,
      20475, 20480, 20538, 20544, 20601, 20664, 20727, 20736,
      20790, 20853, 20916, 20928, 20979, 21042, 21105, 21120,
      21168, 21231, 21294, 21312, 21357, 21420, 21483, 21504,
      21546, 21609, 21672, 21696, 21735, 21798, 21861, 21888,
      21924, 21987, 22050, 22080, 22113, 22176, 22239, 22272,
      22302, 22365, 22428, 22464, 22491, 22528, 22554, 22617,
      22656, 22680, 22743, 22806, 22848, 22869, 22932, 22995,
      23040, 23058, 23121, 23184, 23232, 23247, 23310, 23373,
      23424, 23436, 23499, 23552, 23562, 23616, 23625, 23688,
      23751, 23808, 23814, 23877, 23940, 24000, 24003, 24066,
      24129, 24192, 24255, 24318, 24381, 24384, 24444, 24507,
      24570, 24576, 24633, 24696, 24759, 24768, 24822, 24885,
      24948, 24960, 25011, 25074, 25137, 25152, 25200, 25263,
      25326, 25344, 25389, 25452, 25515, 25536, 25578, 25600,
      25641, 25704, 25728, 25767, 25830, 25893, 25920, 25956,
      26019, 26082, 26112, 26145, 26208, 26271, 26304, 26334,
      26397, 26460, 26496, 26523, 26586, 26624, 26649, 26688,
      26712, 26775, 26838, 26880, 26901, 26964, 27027, 27072,
      27090, 27153, 27216, 27264, 27279, 27342, 27405, 27456,
      27468, 27531, 27594, 27648, 27657, 27720, 27783, 27840,
      27846, 27909, 27972, 28032, 28035, 28098, 28161, 28224,
      28287, 28350, 28413, 28416, 28476, 28539, 28602, 28608,
      28665, 28672, 28728, 28791, 28800, 28854, 28917, 28980,
      28992, 29043, 29106, 29169, 29184, 29232, 29295, 29358,
      29376, 29421, 29484, 29547, 29568, 29610, 29673, 29696,
      29736, 29760, 29799, 29862, 29925, 29952, 29988, 30051,
      30114, 30144, 30177, 30240, 30303, 30336, 30366, 30429,
      30492, 30528, 30555, 30618, 30681, 30720, 30744, 30807,
      30870, 30912, 30933, 30996, 31059, 31104, 31122, 31185,
      31248, 31296, 31311, 31374, 31437, 31488, 31500, 31563,
      31626, 31680, 31689, 31744, 31752, 31815, 31872, 31878,
      31941, 32004, 32064, 32067, 32130, 32193, 32256, 32448,
      32640, 32832, 33024, 33216, 33408, 33600, 33792, 33984,
      34176, 34368, 34560, 34752, 34944, 35136, 35328, 35520,
      35712, 35904, 36096, 36288, 36480, 36672, 36864, 37056,
      37248, 37440, 37632, 37824, 38016, 38208, 38400, 38592,
      38784, 38976, 39168, 39360, 39552, 39744, 39936, 40128,
      40320, 40512, 40704, 40896, 41088, 41280, 41472, 41664,
      41856, 42048, 42240, 42432, 42624, 42816, 43008, 43200,
      43392, 43584, 43776, 43968, 44160, 44352, 44544, 44736,
      44928, 45120, 45312, 45504, 45696, 45888, 46080, 46272,
      46464, 46656, 46848, 47040, 47232, 47424, 47616, 47808,
      48000, 48192, 48384, 48576, 48768, 48960 }
  };
  return &parm;
}

// In offline encoding scenery, data file type will be changed. Below impl need to be changed also? 
RT_DATA_INFO* Get_rt_data_info() {
  static RT_DATA_INFO info = {
    "squeezenet_cifar10.weight",
    "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX",
    DE_MSG_F32
  };
  return &info;
}

int32_t _cst_87[9] = {
  -33, -32, -31, -1, 0, 1, 31, 32,
  33
};
int32_t _cst_101[9] = {
  -33, -32, -31, -1, 0, 1, 31, 32,
  33
};
int32_t _cst_110[4] = {
  0, 1024, 2048, 3072
};
int32_t _cst_119[9] = {
  -33, -32, -31, -1, 0, 1, 31, 32,
  33
};
int32_t _cst_132[4] = {
  0, 256, 512, 768
};
int32_t _cst_137[9] = {
  -17, -16, -15, -1, 0, 1, 15, 16,
  17
};
int32_t _cst_146[4] = {
  0, 256, 512, 768
};
int32_t _cst_151[9] = {
  -17, -16, -15, -1, 0, 1, 15, 16,
  17
};
int32_t _cst_160[4] = {
  0, 256, 512, 768
};
int32_t _cst_165[9] = {
  -17, -16, -15, -1, 0, 1, 15, 16,
  17
};
int32_t _cst_174[8] = {
  0, 256, 512, 768, 1024, 1280, 1536, 1792
};
int32_t _cst_183[9] = {
  -17, -16, -15, -1, 0, 1, 15, 16,
  17
};
int32_t _cst_196[8] = {
  0, 64, 128, 192, 256, 320, 384, 448
};
int32_t _cst_201[9] = {
  -9, -8, -7, -1, 0, 1, 7, 8,
  9
};
int32_t _cst_206[16] = {
  0, 64, 128, 192, 256, 320, 384, 448,
  512, 576, 640, 704, 768, 832, 896, 960
};
int32_t _cst_213[2] = {
  0, 1
};
