// external header files
#include "rt_ant/rt_ant.h"

typedef double float64_t;
typedef float float32_t;

extern float32_t _cst_0[640];
extern float32_t _cst_1[10];
extern float32_t _cst_2[432];
extern float32_t _cst_3[16];
extern float32_t _cst_4[2304];
extern float32_t _cst_5[16];
extern float32_t _cst_6[2304];
extern float32_t _cst_7[16];
extern float32_t _cst_8[2304];
extern float32_t _cst_9[16];
extern float32_t _cst_10[2304];
extern float32_t _cst_11[16];
extern float32_t _cst_12[2304];
extern float32_t _cst_13[16];
extern float32_t _cst_14[2304];
extern float32_t _cst_15[16];
extern float32_t _cst_16[4608];
extern float32_t _cst_17[32];
extern float32_t _cst_18[9216];
extern float32_t _cst_19[32];
extern float32_t _cst_20[512];
extern float32_t _cst_21[32];
extern float32_t _cst_22[9216];
extern float32_t _cst_23[32];
extern float32_t _cst_24[9216];
extern float32_t _cst_25[32];
extern float32_t _cst_26[9216];
extern float32_t _cst_27[32];
extern float32_t _cst_28[9216];
extern float32_t _cst_29[32];
extern float32_t _cst_30[18432];
extern float32_t _cst_31[64];
extern float32_t _cst_32[36864];
extern float32_t _cst_33[64];
extern float32_t _cst_34[2048];
extern float32_t _cst_35[64];
extern float32_t _cst_36[36864];
extern float32_t _cst_37[64];
extern float32_t _cst_38[36864];
extern float32_t _cst_39[64];
extern float32_t _cst_40[36864];
extern float32_t _cst_41[64];
extern float32_t _cst_42[36864];
extern float32_t _cst_43[64];
extern int64_t _cst_44[2];
extern int64_t _cst_45[2];
extern int64_t _cst_46[2];
extern int64_t _cst_47[2];
extern int64_t _cst_48[2];
extern int64_t _cst_49[2];
extern int64_t _cst_50[2];
extern int64_t _cst_51[2];
extern int64_t _cst_52[2];
extern int64_t _cst_53[2];
extern int64_t _cst_54[2];
extern int64_t _cst_55[2];
extern int64_t _cst_56[2];
extern int32_t _cst_58[1];
extern float32_t _cst_59[589824];
extern float32_t _cst_60[16384];
extern int32_t _cst_61[9];
extern float32_t _cst_62[16384];
extern int32_t _cst_64[1];
extern float32_t _cst_65[2359296];
extern float32_t _cst_66[16384];
extern int32_t _cst_67[9];
extern float32_t _cst_68[16384];
extern int32_t _cst_70[1];
extern float32_t _cst_71[2359296];
extern float32_t _cst_72[16384];
extern int32_t _cst_73[9];
extern float32_t _cst_74[16384];
extern int32_t _cst_77[1];
extern float32_t _cst_78[2359296];
extern float32_t _cst_79[16384];
extern int32_t _cst_80[9];
extern float32_t _cst_81[16384];
extern int32_t _cst_83[1];
extern float32_t _cst_84[2359296];
extern float32_t _cst_85[16384];
extern int32_t _cst_86[9];
extern float32_t _cst_87[16384];
extern int32_t _cst_90[1];
extern float32_t _cst_91[2359296];
extern float32_t _cst_92[16384];
extern int32_t _cst_93[9];
extern float32_t _cst_94[16384];
extern int32_t _cst_96[1];
extern float32_t _cst_97[2359296];
extern float32_t _cst_98[16384];
extern int32_t _cst_99[9];
extern float32_t _cst_100[16384];
extern int32_t _cst_103[1];
extern float32_t _cst_104[4718592];
extern float32_t _cst_105[32768];
extern int32_t _cst_106[9];
extern float32_t _cst_107[32768];
extern float32_t _cst_108[524288];
extern float32_t _cst_109[524288];
extern float32_t _cst_110[1048576];
extern int32_t _cst_112[1];
extern float32_t _cst_113[2359296];
extern float32_t _cst_114[8192];
extern int32_t _cst_115[9];
extern float32_t _cst_116[8192];
extern int32_t _cst_118[1];
extern float32_t _cst_119[524288];
extern float32_t _cst_120[32768];
extern int32_t _cst_121[1];
extern float32_t _cst_122[32768];
extern float32_t _cst_123[524288];
extern float32_t _cst_124[524288];
extern float32_t _cst_125[1048576];
extern int32_t _cst_128[1];
extern float32_t _cst_129[2359296];
extern float32_t _cst_130[8192];
extern int32_t _cst_131[9];
extern float32_t _cst_132[8192];
extern int32_t _cst_134[1];
extern float32_t _cst_135[2359296];
extern float32_t _cst_136[8192];
extern int32_t _cst_137[9];
extern float32_t _cst_138[8192];
extern int32_t _cst_141[1];
extern float32_t _cst_142[2359296];
extern float32_t _cst_143[8192];
extern int32_t _cst_144[9];
extern float32_t _cst_145[8192];
extern int32_t _cst_147[1];
extern float32_t _cst_148[2359296];
extern float32_t _cst_149[8192];
extern int32_t _cst_150[9];
extern float32_t _cst_151[8192];
extern int32_t _cst_154[1];
extern float32_t _cst_155[4718592];
extern float32_t _cst_156[16384];
extern int32_t _cst_157[9];
extern float32_t _cst_158[16384];
extern float32_t _cst_159[131072];
extern float32_t _cst_160[131072];
extern float32_t _cst_161[1048576];
extern int32_t _cst_163[1];
extern float32_t _cst_164[2359296];
extern float32_t _cst_165[4096];
extern int32_t _cst_166[9];
extern float32_t _cst_167[4096];
extern int32_t _cst_169[1];
extern float32_t _cst_170[524288];
extern float32_t _cst_171[16384];
extern int32_t _cst_172[1];
extern float32_t _cst_173[16384];
extern float32_t _cst_174[131072];
extern float32_t _cst_175[131072];
extern float32_t _cst_176[1048576];
extern int32_t _cst_179[1];
extern float32_t _cst_180[2359296];
extern float32_t _cst_181[4096];
extern int32_t _cst_182[9];
extern float32_t _cst_183[4096];
extern int32_t _cst_185[1];
extern float32_t _cst_186[2359296];
extern float32_t _cst_187[4096];
extern int32_t _cst_188[9];
extern float32_t _cst_189[4096];
extern int32_t _cst_192[1];
extern float32_t _cst_193[2359296];
extern float32_t _cst_194[4096];
extern int32_t _cst_195[9];
extern float32_t _cst_196[4096];
extern int32_t _cst_198[1];
extern float32_t _cst_199[2359296];
extern float32_t _cst_200[4096];
extern int32_t _cst_201[9];
extern float32_t _cst_202[4096];
extern float32_t _cst_205[4096];
extern float32_t _cst_206[64];
extern float32_t _cst_209[700];
extern float32_t _cst_210[10];
float32_t _cst_211 = -1;
float32_t _cst_212 = -1;
float64_t _cst_213 = -3.809199432496630510769;
float64_t _cst_214 = 2.460940868770910228136;
float64_t _cst_215 = -1.430304794543664215212;
float64_t _cst_216 = 2.944661935353764636858;
float32_t _cst_217 = -1;
float32_t _cst_218 = -1;
float32_t _cst_219 = -1;
float32_t _cst_220 = -1;
float64_t _cst_221 = -0.0001600280537250875771867;
float64_t _cst_222 = 0.003032478474127557870749;
float64_t _cst_223 = 0.1365828361194337536855;
float64_t _cst_224 = -0.02219735858216702675816;
float64_t _cst_225 = 0.5685231134608953462717;
float64_t _cst_226 = -0.5558785316215136829143;
float64_t _cst_227 = 1.614771079042867985009;
float64_t _cst_228 = -0.6164083583294512891371;
float32_t _cst_229 = -1;
float32_t _cst_230 = -1;
float32_t _cst_231 = -1;
float32_t _cst_232 = -1;
float64_t _cst_233 = 0.0001111119162030077897477;
float64_t _cst_234 = 0.006507995190210730772351;
float64_t _cst_235 = -0.0009371069270347270278171;
float64_t _cst_236 = 0.1056744718173614761003;
float64_t _cst_237 = -0.03691063940349948585551;
float64_t _cst_238 = 0.6334139843709288841822;
float64_t _cst_239 = -0.2078597158810965850329;
float32_t _cst_240 = 0.5;
float32_t _cst_241 = 0.25;
float32_t _cst_242 = 0.3333333333333333148296;
float32_t _cst_243 = 0.25;
float32_t _cst_244 = 0.25;
float32_t _cst_245 = 0.2000000000000000111022;
float32_t _cst_246 = 0.3333333333333333148296;
float32_t _cst_247 = 0.2000000000000000111022;
float32_t _cst_248 = 0.3333333333333333148296;
float32_t _cst_249 = 0.2000000000000000111022;
float32_t _cst_250 = 0.3333333333333333148296;
float32_t _cst_251 = 0.2000000000000000111022;
float32_t _cst_252 = 0.3333333333333333148296;
float32_t _cst_253 = 0.1428571428571428492127;
float32_t _cst_254 = 0.3333333333333333148296;
float32_t _cst_255 = 0.25;
float32_t _cst_256 = 0.3333333333333333148296;
float32_t _cst_257 = 0.1666666666666666574148;
float32_t _cst_258 = 0.25;
float32_t _cst_259 = 0.05000000000000000277556;
CIPHERTEXT App_relu(CIPHERTEXT input_ct0, CIPHERTEXT input_ct1);
CIPHERTEXT Rotate(CIPHERTEXT ciph, int32_t rot_idx);
CIPHERTEXT Relinearize(CIPHERTEXT3 ciph3);

bool Main_graph() {
  CIPHERTEXT input;
  CIPHERTEXT result_n1;
  CIPHERTEXT input_dup_n1;
  int32_t index_khw1_n1;
  CIPHERTEXT input_roll_n1[9];
  int32_t index_cin_n1;
  CIPHERTEXT result_cin_n1;
  int32_t index_khw2_n1;
  CIPHERTEXT result_n2;
  CIPHERTEXT input_dup_n2;
  int32_t index_khw1_n2;
  CIPHERTEXT input_roll_n2[9];
  int32_t index_cin_n2;
  CIPHERTEXT result_cin_n2;
  int32_t index_khw2_n2;
  CIPHERTEXT result_n3;
  CIPHERTEXT input_dup_n3;
  int32_t index_khw1_n3;
  CIPHERTEXT input_roll_n3[9];
  int32_t index_cin_n3;
  CIPHERTEXT result_cin_n3;
  int32_t index_khw2_n3;
  CIPHERTEXT result_n4;
  CIPHERTEXT input_dup_n4;
  int32_t index_khw1_n4;
  CIPHERTEXT input_roll_n4[9];
  int32_t index_cin_n4;
  CIPHERTEXT result_cin_n4;
  int32_t index_khw2_n4;
  CIPHERTEXT result_n5;
  CIPHERTEXT input_dup_n5;
  int32_t index_khw1_n5;
  CIPHERTEXT input_roll_n5[9];
  int32_t index_cin_n5;
  CIPHERTEXT result_cin_n5;
  int32_t index_khw2_n5;
  CIPHERTEXT result_n6;
  CIPHERTEXT input_dup_n6;
  int32_t index_khw1_n6;
  CIPHERTEXT input_roll_n6[9];
  int32_t index_cin_n6;
  CIPHERTEXT result_cin_n6;
  int32_t index_khw2_n6;
  CIPHERTEXT result_n7;
  CIPHERTEXT input_dup_n7;
  int32_t index_khw1_n7;
  CIPHERTEXT input_roll_n7[9];
  int32_t index_cin_n7;
  CIPHERTEXT result_cin_n7;
  int32_t index_khw2_n7;
  CIPHERTEXT result_n8;
  CIPHERTEXT input_dup_n8;
  int32_t index_khw1_n8;
  CIPHERTEXT input_roll_n8[9];
  int32_t index_cin_n8;
  CIPHERTEXT result_cin_n8;
  int32_t index_khw2_n8;
  CIPHERTEXT orig_result8;
  CIPHERTEXT comb_row_result9;
  int32_t combine_row_index_n9;
  CIPHERTEXT comb_rc_result9;
  int32_t combine_rc_index_n9;
  CIPHERTEXT final_result9;
  int32_t combine_cc_index_n9;
  CIPHERTEXT result_n10;
  CIPHERTEXT input_dup_n10;
  int32_t index_khw1_n10;
  CIPHERTEXT input_roll_n10[9];
  int32_t index_cin_n10;
  CIPHERTEXT result_cin_n10;
  int32_t index_khw2_n10;
  CIPHERTEXT result_n11;
  CIPHERTEXT input_dup_n11;
  int32_t index_khw1_n11;
  CIPHERTEXT input_roll_n11[1];
  int32_t index_cin_n11;
  CIPHERTEXT result_cin_n11;
  int32_t index_khw2_n11;
  CIPHERTEXT orig_result11;
  CIPHERTEXT comb_row_result12;
  int32_t combine_row_index_n12;
  CIPHERTEXT comb_rc_result12;
  int32_t combine_rc_index_n12;
  CIPHERTEXT final_result12;
  int32_t combine_cc_index_n12;
  CIPHERTEXT result_n13;
  CIPHERTEXT input_dup_n13;
  int32_t index_khw1_n13;
  CIPHERTEXT input_roll_n13[9];
  int32_t index_cin_n13;
  CIPHERTEXT result_cin_n13;
  int32_t index_khw2_n13;
  CIPHERTEXT result_n14;
  CIPHERTEXT input_dup_n14;
  int32_t index_khw1_n14;
  CIPHERTEXT input_roll_n14[9];
  int32_t index_cin_n14;
  CIPHERTEXT result_cin_n14;
  int32_t index_khw2_n14;
  CIPHERTEXT result_n15;
  CIPHERTEXT input_dup_n15;
  int32_t index_khw1_n15;
  CIPHERTEXT input_roll_n15[9];
  int32_t index_cin_n15;
  CIPHERTEXT result_cin_n15;
  int32_t index_khw2_n15;
  CIPHERTEXT result_n16;
  CIPHERTEXT input_dup_n16;
  int32_t index_khw1_n16;
  CIPHERTEXT input_roll_n16[9];
  int32_t index_cin_n16;
  CIPHERTEXT result_cin_n16;
  int32_t index_khw2_n16;
  CIPHERTEXT result_n17;
  CIPHERTEXT input_dup_n17;
  int32_t index_khw1_n17;
  CIPHERTEXT input_roll_n17[9];
  int32_t index_cin_n17;
  CIPHERTEXT result_cin_n17;
  int32_t index_khw2_n17;
  CIPHERTEXT orig_result17;
  CIPHERTEXT comb_row_result18;
  int32_t combine_row_index_n18;
  CIPHERTEXT comb_rc_result18;
  int32_t combine_rc_index_n18;
  CIPHERTEXT final_result18;
  int32_t combine_cc_index_n18;
  CIPHERTEXT result_n19;
  CIPHERTEXT input_dup_n19;
  int32_t index_khw1_n19;
  CIPHERTEXT input_roll_n19[9];
  int32_t index_cin_n19;
  CIPHERTEXT result_cin_n19;
  int32_t index_khw2_n19;
  CIPHERTEXT result_n20;
  CIPHERTEXT input_dup_n20;
  int32_t index_khw1_n20;
  CIPHERTEXT input_roll_n20[1];
  int32_t index_cin_n20;
  CIPHERTEXT result_cin_n20;
  int32_t index_khw2_n20;
  CIPHERTEXT orig_result20;
  CIPHERTEXT comb_row_result21;
  int32_t combine_row_index_n21;
  CIPHERTEXT comb_rc_result21;
  int32_t combine_rc_index_n21;
  CIPHERTEXT final_result21;
  int32_t combine_cc_index_n21;
  CIPHERTEXT result_n22;
  CIPHERTEXT input_dup_n22;
  int32_t index_khw1_n22;
  CIPHERTEXT input_roll_n22[9];
  int32_t index_cin_n22;
  CIPHERTEXT result_cin_n22;
  int32_t index_khw2_n22;
  CIPHERTEXT result_n23;
  CIPHERTEXT input_dup_n23;
  int32_t index_khw1_n23;
  CIPHERTEXT input_roll_n23[9];
  int32_t index_cin_n23;
  CIPHERTEXT result_cin_n23;
  int32_t index_khw2_n23;
  CIPHERTEXT result_n24;
  CIPHERTEXT input_dup_n24;
  int32_t index_khw1_n24;
  CIPHERTEXT input_roll_n24[9];
  int32_t index_cin_n24;
  CIPHERTEXT result_cin_n24;
  int32_t index_khw2_n24;
  CIPHERTEXT result_n25;
  CIPHERTEXT input_dup_n25;
  int32_t index_khw1_n25;
  CIPHERTEXT input_roll_n25[9];
  int32_t index_cin_n25;
  CIPHERTEXT result_cin_n25;
  int32_t index_khw2_n25;
  int32_t sum_index_n25;
  CIPHERTEXT gap_result25;
  int32_t combine_cc_index_n25;
  CIPHERTEXT tmp_result_n26;
  CIPHERTEXT input_dup_n26;
  int32_t index_gemm_n26;
  int32_t index_add_n26;
  CIPHERTEXT output;
  CIPHERTEXT _ckks_gen_tmp_905;
  CIPHERTEXT _ckks_gen_tmp_936;
  CIPHERTEXT _ckks_gen_tmp_967;
  CIPHERTEXT _ckks_gen_tmp_1206;
  CIPHERTEXT _ckks_gen_tmp_1237;
  CIPHERTEXT _ckks_gen_tmp_1268;
  CIPHERTEXT _ckks_gen_tmp_1863;
  CIPHERTEXT _ckks_gen_tmp_1894;
  CIPHERTEXT _ckks_gen_tmp_1925;
  CIPHERTEXT _ckks_gen_tmp_2168;
  CIPHERTEXT _ckks_gen_tmp_2199;
  CIPHERTEXT _ckks_gen_tmp_2230;
  CIPHERTEXT _ckks_gen_tmp_2791;
  CIPHERTEXT _rescale_tmp_65;
  CIPHERTEXT _rescale_tmp_2850;
  CIPHERTEXT _rescale_tmp_173;
  CIPHERTEXT _rescale_tmp_2863;
  CIPHERTEXT _rescale_tmp_281;
  CIPHERTEXT _rescale_tmp_2876;
  CIPHERTEXT _rescale_tmp_395;
  CIPHERTEXT _rescale_tmp_2889;
  CIPHERTEXT _rescale_tmp_503;
  CIPHERTEXT _rescale_tmp_2902;
  CIPHERTEXT _rescale_tmp_617;
  CIPHERTEXT _rescale_tmp_2915;
  CIPHERTEXT _rescale_tmp_725;
  CIPHERTEXT _rescale_tmp_2928;
  CIPHERTEXT _rescale_tmp_843;
  CIPHERTEXT _rescale_tmp_2941;
  CIPHERTEXT _rescale_tmp_2947;
  CIPHERTEXT _rescale_tmp_2953;
  CIPHERTEXT _rescale_tmp_2959;
  CIPHERTEXT _rescale_tmp_1042;
  CIPHERTEXT _rescale_tmp_2972;
  CIPHERTEXT _rescale_tmp_1144;
  CIPHERTEXT _rescale_tmp_2983;
  CIPHERTEXT _rescale_tmp_2989;
  CIPHERTEXT _rescale_tmp_2995;
  CIPHERTEXT _rescale_tmp_3001;
  CIPHERTEXT _rescale_tmp_1349;
  CIPHERTEXT _rescale_tmp_3014;
  CIPHERTEXT _rescale_tmp_1457;
  CIPHERTEXT _rescale_tmp_3027;
  CIPHERTEXT _rescale_tmp_1571;
  CIPHERTEXT _rescale_tmp_3040;
  CIPHERTEXT _rescale_tmp_1679;
  CIPHERTEXT _rescale_tmp_3053;
  CIPHERTEXT _rescale_tmp_1797;
  CIPHERTEXT _rescale_tmp_3066;
  CIPHERTEXT _rescale_tmp_3072;
  CIPHERTEXT _rescale_tmp_3078;
  CIPHERTEXT _rescale_tmp_3084;
  CIPHERTEXT _rescale_tmp_2000;
  CIPHERTEXT _rescale_tmp_3097;
  CIPHERTEXT _rescale_tmp_2102;
  CIPHERTEXT _rescale_tmp_3108;
  CIPHERTEXT _rescale_tmp_3114;
  CIPHERTEXT _rescale_tmp_3120;
  CIPHERTEXT _rescale_tmp_3126;
  CIPHERTEXT _rescale_tmp_2311;
  CIPHERTEXT _rescale_tmp_3139;
  CIPHERTEXT _rescale_tmp_2419;
  CIPHERTEXT _rescale_tmp_3152;
  CIPHERTEXT _rescale_tmp_2533;
  CIPHERTEXT _rescale_tmp_3165;
  CIPHERTEXT _rescale_tmp_2641;
  CIPHERTEXT _rescale_tmp_3178;
  CIPHERTEXT _rescale_tmp_2726;
  CIPHERTEXT _rescale_tmp_3191;
  CIPHERTEXT _rescale_tmp_3197;
  CIPHERTEXT _rescale_tmp_3203;
  MODULUS* _pgen_modulus;
  uint32_t _pgen_num_q;
  uint32_t _pgen_rns_idx;
  CIPHERTEXT _pgen_tmp_0;
  CIPHERTEXT _pgen_tmp_1;
  PLAINTEXT _pgen_tmp_2;
  CIPHERTEXT _pgen_tmp_3;
  CIPHERTEXT _pgen_tmp_4;
  PLAINTEXT _pgen_tmp_5;
  PLAINTEXT _pgen_tmp_6;
  CIPHERTEXT _pgen_tmp_7;
  CIPHERTEXT _pgen_tmp_8;
  PLAINTEXT _pgen_tmp_9;
  CIPHERTEXT _pgen_tmp_10;
  PLAINTEXT _pgen_tmp_11;
  CIPHERTEXT _pgen_tmp_12;
  CIPHERTEXT _pgen_tmp_13;
  PLAINTEXT _pgen_tmp_14;
  PLAINTEXT _pgen_tmp_15;
  CIPHERTEXT _pgen_tmp_16;
  CIPHERTEXT _pgen_tmp_17;
  PLAINTEXT _pgen_tmp_18;
  CIPHERTEXT _pgen_tmp_19;
  PLAINTEXT _pgen_tmp_20;
  CIPHERTEXT _pgen_tmp_21;
  CIPHERTEXT _pgen_tmp_22;
  PLAINTEXT _pgen_tmp_23;
  PLAINTEXT _pgen_tmp_24;
  CIPHERTEXT _pgen_tmp_25;
  CIPHERTEXT _pgen_tmp_26;
  PLAINTEXT _pgen_tmp_27;
  CIPHERTEXT _pgen_tmp_28;
  PLAINTEXT _pgen_tmp_29;
  CIPHERTEXT _pgen_tmp_30;
  CIPHERTEXT _pgen_tmp_31;
  PLAINTEXT _pgen_tmp_32;
  PLAINTEXT _pgen_tmp_33;
  CIPHERTEXT _pgen_tmp_34;
  CIPHERTEXT _pgen_tmp_35;
  PLAINTEXT _pgen_tmp_36;
  CIPHERTEXT _pgen_tmp_37;
  PLAINTEXT _pgen_tmp_38;
  CIPHERTEXT _pgen_tmp_39;
  CIPHERTEXT _pgen_tmp_40;
  PLAINTEXT _pgen_tmp_41;
  PLAINTEXT _pgen_tmp_42;
  CIPHERTEXT _pgen_tmp_43;
  CIPHERTEXT _pgen_tmp_44;
  PLAINTEXT _pgen_tmp_45;
  CIPHERTEXT _pgen_tmp_46;
  PLAINTEXT _pgen_tmp_47;
  CIPHERTEXT _pgen_tmp_48;
  CIPHERTEXT _pgen_tmp_49;
  PLAINTEXT _pgen_tmp_50;
  PLAINTEXT _pgen_tmp_51;
  CIPHERTEXT _pgen_tmp_52;
  CIPHERTEXT _pgen_tmp_53;
  PLAINTEXT _pgen_tmp_54;
  CIPHERTEXT _pgen_tmp_55;
  PLAINTEXT _pgen_tmp_56;
  CIPHERTEXT _pgen_tmp_57;
  CIPHERTEXT _pgen_tmp_58;
  PLAINTEXT _pgen_tmp_59;
  PLAINTEXT _pgen_tmp_60;
  CIPHERTEXT _pgen_tmp_61;
  CIPHERTEXT _pgen_tmp_62;
  PLAINTEXT _pgen_tmp_63;
  CIPHERTEXT _pgen_tmp_64;
  PLAINTEXT _pgen_tmp_65;
  CIPHERTEXT _pgen_tmp_66;
  PLAINTEXT _pgen_tmp_67;
  PLAINTEXT _pgen_tmp_68;
  PLAINTEXT _pgen_tmp_69;
  CIPHERTEXT _pgen_tmp_70;
  PLAINTEXT _pgen_tmp_71;
  CIPHERTEXT _pgen_tmp_72;
  PLAINTEXT _pgen_tmp_73;
  CIPHERTEXT _pgen_tmp_74;
  CIPHERTEXT _pgen_tmp_75;
  CIPHERTEXT _pgen_tmp_76;
  PLAINTEXT _pgen_tmp_77;
  CIPHERTEXT _pgen_tmp_78;
  PLAINTEXT _pgen_tmp_79;
  CIPHERTEXT _pgen_tmp_80;
  CIPHERTEXT _pgen_tmp_81;
  PLAINTEXT _pgen_tmp_82;
  PLAINTEXT _pgen_tmp_83;
  PLAINTEXT _pgen_tmp_84;
  CIPHERTEXT _pgen_tmp_85;
  PLAINTEXT _pgen_tmp_86;
  PLAINTEXT _pgen_tmp_87;
  PLAINTEXT _pgen_tmp_88;
  CIPHERTEXT _pgen_tmp_89;
  PLAINTEXT _pgen_tmp_90;
  CIPHERTEXT _pgen_tmp_91;
  PLAINTEXT _pgen_tmp_92;
  CIPHERTEXT _pgen_tmp_93;
  CIPHERTEXT _pgen_tmp_94;
  CIPHERTEXT _pgen_tmp_95;
  PLAINTEXT _pgen_tmp_96;
  CIPHERTEXT _pgen_tmp_97;
  PLAINTEXT _pgen_tmp_98;
  CIPHERTEXT _pgen_tmp_99;
  CIPHERTEXT _pgen_tmp_100;
  PLAINTEXT _pgen_tmp_101;
  PLAINTEXT _pgen_tmp_102;
  CIPHERTEXT _pgen_tmp_103;
  CIPHERTEXT _pgen_tmp_104;
  PLAINTEXT _pgen_tmp_105;
  CIPHERTEXT _pgen_tmp_106;
  PLAINTEXT _pgen_tmp_107;
  CIPHERTEXT _pgen_tmp_108;
  CIPHERTEXT _pgen_tmp_109;
  PLAINTEXT _pgen_tmp_110;
  PLAINTEXT _pgen_tmp_111;
  CIPHERTEXT _pgen_tmp_112;
  CIPHERTEXT _pgen_tmp_113;
  PLAINTEXT _pgen_tmp_114;
  CIPHERTEXT _pgen_tmp_115;
  PLAINTEXT _pgen_tmp_116;
  CIPHERTEXT _pgen_tmp_117;
  CIPHERTEXT _pgen_tmp_118;
  PLAINTEXT _pgen_tmp_119;
  PLAINTEXT _pgen_tmp_120;
  CIPHERTEXT _pgen_tmp_121;
  CIPHERTEXT _pgen_tmp_122;
  PLAINTEXT _pgen_tmp_123;
  CIPHERTEXT _pgen_tmp_124;
  PLAINTEXT _pgen_tmp_125;
  CIPHERTEXT _pgen_tmp_126;
  CIPHERTEXT _pgen_tmp_127;
  PLAINTEXT _pgen_tmp_128;
  PLAINTEXT _pgen_tmp_129;
  CIPHERTEXT _pgen_tmp_130;
  CIPHERTEXT _pgen_tmp_131;
  PLAINTEXT _pgen_tmp_132;
  CIPHERTEXT _pgen_tmp_133;
  PLAINTEXT _pgen_tmp_134;
  CIPHERTEXT _pgen_tmp_135;
  CIPHERTEXT _pgen_tmp_136;
  PLAINTEXT _pgen_tmp_137;
  PLAINTEXT _pgen_tmp_138;
  PLAINTEXT _pgen_tmp_139;
  CIPHERTEXT _pgen_tmp_140;
  PLAINTEXT _pgen_tmp_141;
  CIPHERTEXT _pgen_tmp_142;
  PLAINTEXT _pgen_tmp_143;
  CIPHERTEXT _pgen_tmp_144;
  CIPHERTEXT _pgen_tmp_145;
  CIPHERTEXT _pgen_tmp_146;
  PLAINTEXT _pgen_tmp_147;
  CIPHERTEXT _pgen_tmp_148;
  PLAINTEXT _pgen_tmp_149;
  CIPHERTEXT _pgen_tmp_150;
  CIPHERTEXT _pgen_tmp_151;
  PLAINTEXT _pgen_tmp_152;
  PLAINTEXT _pgen_tmp_153;
  PLAINTEXT _pgen_tmp_154;
  CIPHERTEXT _pgen_tmp_155;
  CIPHERTEXT _pgen_tmp_156;
  PLAINTEXT _pgen_tmp_157;
  PLAINTEXT _pgen_tmp_158;
  PLAINTEXT _pgen_tmp_159;
  CIPHERTEXT _pgen_tmp_160;
  PLAINTEXT _pgen_tmp_161;
  CIPHERTEXT _pgen_tmp_162;
  PLAINTEXT _pgen_tmp_163;
  CIPHERTEXT _pgen_tmp_164;
  CIPHERTEXT _pgen_tmp_165;
  CIPHERTEXT _pgen_tmp_166;
  PLAINTEXT _pgen_tmp_167;
  CIPHERTEXT _pgen_tmp_168;
  PLAINTEXT _pgen_tmp_169;
  CIPHERTEXT _pgen_tmp_170;
  CIPHERTEXT _pgen_tmp_171;
  PLAINTEXT _pgen_tmp_172;
  PLAINTEXT _pgen_tmp_173;
  CIPHERTEXT _pgen_tmp_174;
  CIPHERTEXT _pgen_tmp_175;
  PLAINTEXT _pgen_tmp_176;
  CIPHERTEXT _pgen_tmp_177;
  PLAINTEXT _pgen_tmp_178;
  CIPHERTEXT _pgen_tmp_179;
  CIPHERTEXT _pgen_tmp_180;
  PLAINTEXT _pgen_tmp_181;
  PLAINTEXT _pgen_tmp_182;
  CIPHERTEXT _pgen_tmp_183;
  CIPHERTEXT _pgen_tmp_184;
  PLAINTEXT _pgen_tmp_185;
  CIPHERTEXT _pgen_tmp_186;
  PLAINTEXT _pgen_tmp_187;
  CIPHERTEXT _pgen_tmp_188;
  CIPHERTEXT _pgen_tmp_189;
  PLAINTEXT _pgen_tmp_190;
  PLAINTEXT _pgen_tmp_191;
  CIPHERTEXT _pgen_tmp_192;
  CIPHERTEXT _pgen_tmp_193;
  PLAINTEXT _pgen_tmp_194;
  CIPHERTEXT _pgen_tmp_195;
  PLAINTEXT _pgen_tmp_196;
  CIPHERTEXT _pgen_tmp_197;
  CIPHERTEXT _pgen_tmp_198;
  PLAINTEXT _pgen_tmp_199;
  PLAINTEXT _pgen_tmp_200;
  CIPHERTEXT _pgen_tmp_201;
  CIPHERTEXT _pgen_tmp_202;
  PLAINTEXT _pgen_tmp_203;
  CIPHERTEXT _pgen_tmp_204;
  PLAINTEXT _pgen_tmp_205;
  PLAINTEXT _pgen_tmp_206;
  PLAINTEXT _pgen_tmp_207;
  CIPHERTEXT _pgen_tmp_208;
  PLAINTEXT _pgen_tmp_209;
  PLAINTEXT _pgen_tmp_210;
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
  uint32_t  degree = Degree();
  input = Get_input_data("input", 0);
  memset(&result_n1, 0, sizeof(result_n1));
  memset(&input_dup_n1, 0, sizeof(input_dup_n1));
  memset(&input_roll_n1, 0, sizeof(input_roll_n1));
  memset(&result_cin_n1, 0, sizeof(result_cin_n1));
  memset(&result_n2, 0, sizeof(result_n2));
  memset(&input_dup_n2, 0, sizeof(input_dup_n2));
  memset(&input_roll_n2, 0, sizeof(input_roll_n2));
  memset(&result_cin_n2, 0, sizeof(result_cin_n2));
  memset(&result_n3, 0, sizeof(result_n3));
  memset(&input_dup_n3, 0, sizeof(input_dup_n3));
  memset(&input_roll_n3, 0, sizeof(input_roll_n3));
  memset(&result_cin_n3, 0, sizeof(result_cin_n3));
  memset(&result_n4, 0, sizeof(result_n4));
  memset(&input_dup_n4, 0, sizeof(input_dup_n4));
  memset(&input_roll_n4, 0, sizeof(input_roll_n4));
  memset(&result_cin_n4, 0, sizeof(result_cin_n4));
  memset(&result_n5, 0, sizeof(result_n5));
  memset(&input_dup_n5, 0, sizeof(input_dup_n5));
  memset(&input_roll_n5, 0, sizeof(input_roll_n5));
  memset(&result_cin_n5, 0, sizeof(result_cin_n5));
  memset(&result_n6, 0, sizeof(result_n6));
  memset(&input_dup_n6, 0, sizeof(input_dup_n6));
  memset(&input_roll_n6, 0, sizeof(input_roll_n6));
  memset(&result_cin_n6, 0, sizeof(result_cin_n6));
  memset(&result_n7, 0, sizeof(result_n7));
  memset(&input_dup_n7, 0, sizeof(input_dup_n7));
  memset(&input_roll_n7, 0, sizeof(input_roll_n7));
  memset(&result_cin_n7, 0, sizeof(result_cin_n7));
  memset(&result_n8, 0, sizeof(result_n8));
  memset(&input_dup_n8, 0, sizeof(input_dup_n8));
  memset(&input_roll_n8, 0, sizeof(input_roll_n8));
  memset(&result_cin_n8, 0, sizeof(result_cin_n8));
  memset(&orig_result8, 0, sizeof(orig_result8));
  memset(&comb_row_result9, 0, sizeof(comb_row_result9));
  memset(&comb_rc_result9, 0, sizeof(comb_rc_result9));
  memset(&final_result9, 0, sizeof(final_result9));
  memset(&result_n10, 0, sizeof(result_n10));
  memset(&input_dup_n10, 0, sizeof(input_dup_n10));
  memset(&input_roll_n10, 0, sizeof(input_roll_n10));
  memset(&result_cin_n10, 0, sizeof(result_cin_n10));
  memset(&result_n11, 0, sizeof(result_n11));
  memset(&input_dup_n11, 0, sizeof(input_dup_n11));
  memset(&input_roll_n11, 0, sizeof(input_roll_n11));
  memset(&result_cin_n11, 0, sizeof(result_cin_n11));
  memset(&orig_result11, 0, sizeof(orig_result11));
  memset(&comb_row_result12, 0, sizeof(comb_row_result12));
  memset(&comb_rc_result12, 0, sizeof(comb_rc_result12));
  memset(&final_result12, 0, sizeof(final_result12));
  memset(&result_n13, 0, sizeof(result_n13));
  memset(&input_dup_n13, 0, sizeof(input_dup_n13));
  memset(&input_roll_n13, 0, sizeof(input_roll_n13));
  memset(&result_cin_n13, 0, sizeof(result_cin_n13));
  memset(&result_n14, 0, sizeof(result_n14));
  memset(&input_dup_n14, 0, sizeof(input_dup_n14));
  memset(&input_roll_n14, 0, sizeof(input_roll_n14));
  memset(&result_cin_n14, 0, sizeof(result_cin_n14));
  memset(&result_n15, 0, sizeof(result_n15));
  memset(&input_dup_n15, 0, sizeof(input_dup_n15));
  memset(&input_roll_n15, 0, sizeof(input_roll_n15));
  memset(&result_cin_n15, 0, sizeof(result_cin_n15));
  memset(&result_n16, 0, sizeof(result_n16));
  memset(&input_dup_n16, 0, sizeof(input_dup_n16));
  memset(&input_roll_n16, 0, sizeof(input_roll_n16));
  memset(&result_cin_n16, 0, sizeof(result_cin_n16));
  memset(&result_n17, 0, sizeof(result_n17));
  memset(&input_dup_n17, 0, sizeof(input_dup_n17));
  memset(&input_roll_n17, 0, sizeof(input_roll_n17));
  memset(&result_cin_n17, 0, sizeof(result_cin_n17));
  memset(&orig_result17, 0, sizeof(orig_result17));
  memset(&comb_row_result18, 0, sizeof(comb_row_result18));
  memset(&comb_rc_result18, 0, sizeof(comb_rc_result18));
  memset(&final_result18, 0, sizeof(final_result18));
  memset(&result_n19, 0, sizeof(result_n19));
  memset(&input_dup_n19, 0, sizeof(input_dup_n19));
  memset(&input_roll_n19, 0, sizeof(input_roll_n19));
  memset(&result_cin_n19, 0, sizeof(result_cin_n19));
  memset(&result_n20, 0, sizeof(result_n20));
  memset(&input_dup_n20, 0, sizeof(input_dup_n20));
  memset(&input_roll_n20, 0, sizeof(input_roll_n20));
  memset(&result_cin_n20, 0, sizeof(result_cin_n20));
  memset(&orig_result20, 0, sizeof(orig_result20));
  memset(&comb_row_result21, 0, sizeof(comb_row_result21));
  memset(&comb_rc_result21, 0, sizeof(comb_rc_result21));
  memset(&final_result21, 0, sizeof(final_result21));
  memset(&result_n22, 0, sizeof(result_n22));
  memset(&input_dup_n22, 0, sizeof(input_dup_n22));
  memset(&input_roll_n22, 0, sizeof(input_roll_n22));
  memset(&result_cin_n22, 0, sizeof(result_cin_n22));
  memset(&result_n23, 0, sizeof(result_n23));
  memset(&input_dup_n23, 0, sizeof(input_dup_n23));
  memset(&input_roll_n23, 0, sizeof(input_roll_n23));
  memset(&result_cin_n23, 0, sizeof(result_cin_n23));
  memset(&result_n24, 0, sizeof(result_n24));
  memset(&input_dup_n24, 0, sizeof(input_dup_n24));
  memset(&input_roll_n24, 0, sizeof(input_roll_n24));
  memset(&result_cin_n24, 0, sizeof(result_cin_n24));
  memset(&result_n25, 0, sizeof(result_n25));
  memset(&input_dup_n25, 0, sizeof(input_dup_n25));
  memset(&input_roll_n25, 0, sizeof(input_roll_n25));
  memset(&result_cin_n25, 0, sizeof(result_cin_n25));
  memset(&gap_result25, 0, sizeof(gap_result25));
  memset(&tmp_result_n26, 0, sizeof(tmp_result_n26));
  memset(&input_dup_n26, 0, sizeof(input_dup_n26));
  memset(&output, 0, sizeof(output));
  memset(&_ckks_gen_tmp_905, 0, sizeof(_ckks_gen_tmp_905));
  memset(&_ckks_gen_tmp_936, 0, sizeof(_ckks_gen_tmp_936));
  memset(&_ckks_gen_tmp_967, 0, sizeof(_ckks_gen_tmp_967));
  memset(&_ckks_gen_tmp_1206, 0, sizeof(_ckks_gen_tmp_1206));
  memset(&_ckks_gen_tmp_1237, 0, sizeof(_ckks_gen_tmp_1237));
  memset(&_ckks_gen_tmp_1268, 0, sizeof(_ckks_gen_tmp_1268));
  memset(&_ckks_gen_tmp_1863, 0, sizeof(_ckks_gen_tmp_1863));
  memset(&_ckks_gen_tmp_1894, 0, sizeof(_ckks_gen_tmp_1894));
  memset(&_ckks_gen_tmp_1925, 0, sizeof(_ckks_gen_tmp_1925));
  memset(&_ckks_gen_tmp_2168, 0, sizeof(_ckks_gen_tmp_2168));
  memset(&_ckks_gen_tmp_2199, 0, sizeof(_ckks_gen_tmp_2199));
  memset(&_ckks_gen_tmp_2230, 0, sizeof(_ckks_gen_tmp_2230));
  memset(&_ckks_gen_tmp_2791, 0, sizeof(_ckks_gen_tmp_2791));
  memset(&_rescale_tmp_65, 0, sizeof(_rescale_tmp_65));
  memset(&_rescale_tmp_2850, 0, sizeof(_rescale_tmp_2850));
  memset(&_rescale_tmp_173, 0, sizeof(_rescale_tmp_173));
  memset(&_rescale_tmp_2863, 0, sizeof(_rescale_tmp_2863));
  memset(&_rescale_tmp_281, 0, sizeof(_rescale_tmp_281));
  memset(&_rescale_tmp_2876, 0, sizeof(_rescale_tmp_2876));
  memset(&_rescale_tmp_395, 0, sizeof(_rescale_tmp_395));
  memset(&_rescale_tmp_2889, 0, sizeof(_rescale_tmp_2889));
  memset(&_rescale_tmp_503, 0, sizeof(_rescale_tmp_503));
  memset(&_rescale_tmp_2902, 0, sizeof(_rescale_tmp_2902));
  memset(&_rescale_tmp_617, 0, sizeof(_rescale_tmp_617));
  memset(&_rescale_tmp_2915, 0, sizeof(_rescale_tmp_2915));
  memset(&_rescale_tmp_725, 0, sizeof(_rescale_tmp_725));
  memset(&_rescale_tmp_2928, 0, sizeof(_rescale_tmp_2928));
  memset(&_rescale_tmp_843, 0, sizeof(_rescale_tmp_843));
  memset(&_rescale_tmp_2941, 0, sizeof(_rescale_tmp_2941));
  memset(&_rescale_tmp_2947, 0, sizeof(_rescale_tmp_2947));
  memset(&_rescale_tmp_2953, 0, sizeof(_rescale_tmp_2953));
  memset(&_rescale_tmp_2959, 0, sizeof(_rescale_tmp_2959));
  memset(&_rescale_tmp_1042, 0, sizeof(_rescale_tmp_1042));
  memset(&_rescale_tmp_2972, 0, sizeof(_rescale_tmp_2972));
  memset(&_rescale_tmp_1144, 0, sizeof(_rescale_tmp_1144));
  memset(&_rescale_tmp_2983, 0, sizeof(_rescale_tmp_2983));
  memset(&_rescale_tmp_2989, 0, sizeof(_rescale_tmp_2989));
  memset(&_rescale_tmp_2995, 0, sizeof(_rescale_tmp_2995));
  memset(&_rescale_tmp_3001, 0, sizeof(_rescale_tmp_3001));
  memset(&_rescale_tmp_1349, 0, sizeof(_rescale_tmp_1349));
  memset(&_rescale_tmp_3014, 0, sizeof(_rescale_tmp_3014));
  memset(&_rescale_tmp_1457, 0, sizeof(_rescale_tmp_1457));
  memset(&_rescale_tmp_3027, 0, sizeof(_rescale_tmp_3027));
  memset(&_rescale_tmp_1571, 0, sizeof(_rescale_tmp_1571));
  memset(&_rescale_tmp_3040, 0, sizeof(_rescale_tmp_3040));
  memset(&_rescale_tmp_1679, 0, sizeof(_rescale_tmp_1679));
  memset(&_rescale_tmp_3053, 0, sizeof(_rescale_tmp_3053));
  memset(&_rescale_tmp_1797, 0, sizeof(_rescale_tmp_1797));
  memset(&_rescale_tmp_3066, 0, sizeof(_rescale_tmp_3066));
  memset(&_rescale_tmp_3072, 0, sizeof(_rescale_tmp_3072));
  memset(&_rescale_tmp_3078, 0, sizeof(_rescale_tmp_3078));
  memset(&_rescale_tmp_3084, 0, sizeof(_rescale_tmp_3084));
  memset(&_rescale_tmp_2000, 0, sizeof(_rescale_tmp_2000));
  memset(&_rescale_tmp_3097, 0, sizeof(_rescale_tmp_3097));
  memset(&_rescale_tmp_2102, 0, sizeof(_rescale_tmp_2102));
  memset(&_rescale_tmp_3108, 0, sizeof(_rescale_tmp_3108));
  memset(&_rescale_tmp_3114, 0, sizeof(_rescale_tmp_3114));
  memset(&_rescale_tmp_3120, 0, sizeof(_rescale_tmp_3120));
  memset(&_rescale_tmp_3126, 0, sizeof(_rescale_tmp_3126));
  memset(&_rescale_tmp_2311, 0, sizeof(_rescale_tmp_2311));
  memset(&_rescale_tmp_3139, 0, sizeof(_rescale_tmp_3139));
  memset(&_rescale_tmp_2419, 0, sizeof(_rescale_tmp_2419));
  memset(&_rescale_tmp_3152, 0, sizeof(_rescale_tmp_3152));
  memset(&_rescale_tmp_2533, 0, sizeof(_rescale_tmp_2533));
  memset(&_rescale_tmp_3165, 0, sizeof(_rescale_tmp_3165));
  memset(&_rescale_tmp_2641, 0, sizeof(_rescale_tmp_2641));
  memset(&_rescale_tmp_3178, 0, sizeof(_rescale_tmp_3178));
  memset(&_rescale_tmp_2726, 0, sizeof(_rescale_tmp_2726));
  memset(&_rescale_tmp_3191, 0, sizeof(_rescale_tmp_3191));
  memset(&_rescale_tmp_3197, 0, sizeof(_rescale_tmp_3197));
  memset(&_rescale_tmp_3203, 0, sizeof(_rescale_tmp_3203));
  memset(&_pgen_tmp_0, 0, sizeof(_pgen_tmp_0));
  memset(&_pgen_tmp_1, 0, sizeof(_pgen_tmp_1));
  memset(&_pgen_tmp_2, 0, sizeof(_pgen_tmp_2));
  memset(&_pgen_tmp_3, 0, sizeof(_pgen_tmp_3));
  memset(&_pgen_tmp_4, 0, sizeof(_pgen_tmp_4));
  memset(&_pgen_tmp_5, 0, sizeof(_pgen_tmp_5));
  memset(&_pgen_tmp_6, 0, sizeof(_pgen_tmp_6));
  memset(&_pgen_tmp_7, 0, sizeof(_pgen_tmp_7));
  memset(&_pgen_tmp_8, 0, sizeof(_pgen_tmp_8));
  memset(&_pgen_tmp_9, 0, sizeof(_pgen_tmp_9));
  memset(&_pgen_tmp_10, 0, sizeof(_pgen_tmp_10));
  memset(&_pgen_tmp_11, 0, sizeof(_pgen_tmp_11));
  memset(&_pgen_tmp_12, 0, sizeof(_pgen_tmp_12));
  memset(&_pgen_tmp_13, 0, sizeof(_pgen_tmp_13));
  memset(&_pgen_tmp_14, 0, sizeof(_pgen_tmp_14));
  memset(&_pgen_tmp_15, 0, sizeof(_pgen_tmp_15));
  memset(&_pgen_tmp_16, 0, sizeof(_pgen_tmp_16));
  memset(&_pgen_tmp_17, 0, sizeof(_pgen_tmp_17));
  memset(&_pgen_tmp_18, 0, sizeof(_pgen_tmp_18));
  memset(&_pgen_tmp_19, 0, sizeof(_pgen_tmp_19));
  memset(&_pgen_tmp_20, 0, sizeof(_pgen_tmp_20));
  memset(&_pgen_tmp_21, 0, sizeof(_pgen_tmp_21));
  memset(&_pgen_tmp_22, 0, sizeof(_pgen_tmp_22));
  memset(&_pgen_tmp_23, 0, sizeof(_pgen_tmp_23));
  memset(&_pgen_tmp_24, 0, sizeof(_pgen_tmp_24));
  memset(&_pgen_tmp_25, 0, sizeof(_pgen_tmp_25));
  memset(&_pgen_tmp_26, 0, sizeof(_pgen_tmp_26));
  memset(&_pgen_tmp_27, 0, sizeof(_pgen_tmp_27));
  memset(&_pgen_tmp_28, 0, sizeof(_pgen_tmp_28));
  memset(&_pgen_tmp_29, 0, sizeof(_pgen_tmp_29));
  memset(&_pgen_tmp_30, 0, sizeof(_pgen_tmp_30));
  memset(&_pgen_tmp_31, 0, sizeof(_pgen_tmp_31));
  memset(&_pgen_tmp_32, 0, sizeof(_pgen_tmp_32));
  memset(&_pgen_tmp_33, 0, sizeof(_pgen_tmp_33));
  memset(&_pgen_tmp_34, 0, sizeof(_pgen_tmp_34));
  memset(&_pgen_tmp_35, 0, sizeof(_pgen_tmp_35));
  memset(&_pgen_tmp_36, 0, sizeof(_pgen_tmp_36));
  memset(&_pgen_tmp_37, 0, sizeof(_pgen_tmp_37));
  memset(&_pgen_tmp_38, 0, sizeof(_pgen_tmp_38));
  memset(&_pgen_tmp_39, 0, sizeof(_pgen_tmp_39));
  memset(&_pgen_tmp_40, 0, sizeof(_pgen_tmp_40));
  memset(&_pgen_tmp_41, 0, sizeof(_pgen_tmp_41));
  memset(&_pgen_tmp_42, 0, sizeof(_pgen_tmp_42));
  memset(&_pgen_tmp_43, 0, sizeof(_pgen_tmp_43));
  memset(&_pgen_tmp_44, 0, sizeof(_pgen_tmp_44));
  memset(&_pgen_tmp_45, 0, sizeof(_pgen_tmp_45));
  memset(&_pgen_tmp_46, 0, sizeof(_pgen_tmp_46));
  memset(&_pgen_tmp_47, 0, sizeof(_pgen_tmp_47));
  memset(&_pgen_tmp_48, 0, sizeof(_pgen_tmp_48));
  memset(&_pgen_tmp_49, 0, sizeof(_pgen_tmp_49));
  memset(&_pgen_tmp_50, 0, sizeof(_pgen_tmp_50));
  memset(&_pgen_tmp_51, 0, sizeof(_pgen_tmp_51));
  memset(&_pgen_tmp_52, 0, sizeof(_pgen_tmp_52));
  memset(&_pgen_tmp_53, 0, sizeof(_pgen_tmp_53));
  memset(&_pgen_tmp_54, 0, sizeof(_pgen_tmp_54));
  memset(&_pgen_tmp_55, 0, sizeof(_pgen_tmp_55));
  memset(&_pgen_tmp_56, 0, sizeof(_pgen_tmp_56));
  memset(&_pgen_tmp_57, 0, sizeof(_pgen_tmp_57));
  memset(&_pgen_tmp_58, 0, sizeof(_pgen_tmp_58));
  memset(&_pgen_tmp_59, 0, sizeof(_pgen_tmp_59));
  memset(&_pgen_tmp_60, 0, sizeof(_pgen_tmp_60));
  memset(&_pgen_tmp_61, 0, sizeof(_pgen_tmp_61));
  memset(&_pgen_tmp_62, 0, sizeof(_pgen_tmp_62));
  memset(&_pgen_tmp_63, 0, sizeof(_pgen_tmp_63));
  memset(&_pgen_tmp_64, 0, sizeof(_pgen_tmp_64));
  memset(&_pgen_tmp_65, 0, sizeof(_pgen_tmp_65));
  memset(&_pgen_tmp_66, 0, sizeof(_pgen_tmp_66));
  memset(&_pgen_tmp_67, 0, sizeof(_pgen_tmp_67));
  memset(&_pgen_tmp_68, 0, sizeof(_pgen_tmp_68));
  memset(&_pgen_tmp_69, 0, sizeof(_pgen_tmp_69));
  memset(&_pgen_tmp_70, 0, sizeof(_pgen_tmp_70));
  memset(&_pgen_tmp_71, 0, sizeof(_pgen_tmp_71));
  memset(&_pgen_tmp_72, 0, sizeof(_pgen_tmp_72));
  memset(&_pgen_tmp_73, 0, sizeof(_pgen_tmp_73));
  memset(&_pgen_tmp_74, 0, sizeof(_pgen_tmp_74));
  memset(&_pgen_tmp_75, 0, sizeof(_pgen_tmp_75));
  memset(&_pgen_tmp_76, 0, sizeof(_pgen_tmp_76));
  memset(&_pgen_tmp_77, 0, sizeof(_pgen_tmp_77));
  memset(&_pgen_tmp_78, 0, sizeof(_pgen_tmp_78));
  memset(&_pgen_tmp_79, 0, sizeof(_pgen_tmp_79));
  memset(&_pgen_tmp_80, 0, sizeof(_pgen_tmp_80));
  memset(&_pgen_tmp_81, 0, sizeof(_pgen_tmp_81));
  memset(&_pgen_tmp_82, 0, sizeof(_pgen_tmp_82));
  memset(&_pgen_tmp_83, 0, sizeof(_pgen_tmp_83));
  memset(&_pgen_tmp_84, 0, sizeof(_pgen_tmp_84));
  memset(&_pgen_tmp_85, 0, sizeof(_pgen_tmp_85));
  memset(&_pgen_tmp_86, 0, sizeof(_pgen_tmp_86));
  memset(&_pgen_tmp_87, 0, sizeof(_pgen_tmp_87));
  memset(&_pgen_tmp_88, 0, sizeof(_pgen_tmp_88));
  memset(&_pgen_tmp_89, 0, sizeof(_pgen_tmp_89));
  memset(&_pgen_tmp_90, 0, sizeof(_pgen_tmp_90));
  memset(&_pgen_tmp_91, 0, sizeof(_pgen_tmp_91));
  memset(&_pgen_tmp_92, 0, sizeof(_pgen_tmp_92));
  memset(&_pgen_tmp_93, 0, sizeof(_pgen_tmp_93));
  memset(&_pgen_tmp_94, 0, sizeof(_pgen_tmp_94));
  memset(&_pgen_tmp_95, 0, sizeof(_pgen_tmp_95));
  memset(&_pgen_tmp_96, 0, sizeof(_pgen_tmp_96));
  memset(&_pgen_tmp_97, 0, sizeof(_pgen_tmp_97));
  memset(&_pgen_tmp_98, 0, sizeof(_pgen_tmp_98));
  memset(&_pgen_tmp_99, 0, sizeof(_pgen_tmp_99));
  memset(&_pgen_tmp_100, 0, sizeof(_pgen_tmp_100));
  memset(&_pgen_tmp_101, 0, sizeof(_pgen_tmp_101));
  memset(&_pgen_tmp_102, 0, sizeof(_pgen_tmp_102));
  memset(&_pgen_tmp_103, 0, sizeof(_pgen_tmp_103));
  memset(&_pgen_tmp_104, 0, sizeof(_pgen_tmp_104));
  memset(&_pgen_tmp_105, 0, sizeof(_pgen_tmp_105));
  memset(&_pgen_tmp_106, 0, sizeof(_pgen_tmp_106));
  memset(&_pgen_tmp_107, 0, sizeof(_pgen_tmp_107));
  memset(&_pgen_tmp_108, 0, sizeof(_pgen_tmp_108));
  memset(&_pgen_tmp_109, 0, sizeof(_pgen_tmp_109));
  memset(&_pgen_tmp_110, 0, sizeof(_pgen_tmp_110));
  memset(&_pgen_tmp_111, 0, sizeof(_pgen_tmp_111));
  memset(&_pgen_tmp_112, 0, sizeof(_pgen_tmp_112));
  memset(&_pgen_tmp_113, 0, sizeof(_pgen_tmp_113));
  memset(&_pgen_tmp_114, 0, sizeof(_pgen_tmp_114));
  memset(&_pgen_tmp_115, 0, sizeof(_pgen_tmp_115));
  memset(&_pgen_tmp_116, 0, sizeof(_pgen_tmp_116));
  memset(&_pgen_tmp_117, 0, sizeof(_pgen_tmp_117));
  memset(&_pgen_tmp_118, 0, sizeof(_pgen_tmp_118));
  memset(&_pgen_tmp_119, 0, sizeof(_pgen_tmp_119));
  memset(&_pgen_tmp_120, 0, sizeof(_pgen_tmp_120));
  memset(&_pgen_tmp_121, 0, sizeof(_pgen_tmp_121));
  memset(&_pgen_tmp_122, 0, sizeof(_pgen_tmp_122));
  memset(&_pgen_tmp_123, 0, sizeof(_pgen_tmp_123));
  memset(&_pgen_tmp_124, 0, sizeof(_pgen_tmp_124));
  memset(&_pgen_tmp_125, 0, sizeof(_pgen_tmp_125));
  memset(&_pgen_tmp_126, 0, sizeof(_pgen_tmp_126));
  memset(&_pgen_tmp_127, 0, sizeof(_pgen_tmp_127));
  memset(&_pgen_tmp_128, 0, sizeof(_pgen_tmp_128));
  memset(&_pgen_tmp_129, 0, sizeof(_pgen_tmp_129));
  memset(&_pgen_tmp_130, 0, sizeof(_pgen_tmp_130));
  memset(&_pgen_tmp_131, 0, sizeof(_pgen_tmp_131));
  memset(&_pgen_tmp_132, 0, sizeof(_pgen_tmp_132));
  memset(&_pgen_tmp_133, 0, sizeof(_pgen_tmp_133));
  memset(&_pgen_tmp_134, 0, sizeof(_pgen_tmp_134));
  memset(&_pgen_tmp_135, 0, sizeof(_pgen_tmp_135));
  memset(&_pgen_tmp_136, 0, sizeof(_pgen_tmp_136));
  memset(&_pgen_tmp_137, 0, sizeof(_pgen_tmp_137));
  memset(&_pgen_tmp_138, 0, sizeof(_pgen_tmp_138));
  memset(&_pgen_tmp_139, 0, sizeof(_pgen_tmp_139));
  memset(&_pgen_tmp_140, 0, sizeof(_pgen_tmp_140));
  memset(&_pgen_tmp_141, 0, sizeof(_pgen_tmp_141));
  memset(&_pgen_tmp_142, 0, sizeof(_pgen_tmp_142));
  memset(&_pgen_tmp_143, 0, sizeof(_pgen_tmp_143));
  memset(&_pgen_tmp_144, 0, sizeof(_pgen_tmp_144));
  memset(&_pgen_tmp_145, 0, sizeof(_pgen_tmp_145));
  memset(&_pgen_tmp_146, 0, sizeof(_pgen_tmp_146));
  memset(&_pgen_tmp_147, 0, sizeof(_pgen_tmp_147));
  memset(&_pgen_tmp_148, 0, sizeof(_pgen_tmp_148));
  memset(&_pgen_tmp_149, 0, sizeof(_pgen_tmp_149));
  memset(&_pgen_tmp_150, 0, sizeof(_pgen_tmp_150));
  memset(&_pgen_tmp_151, 0, sizeof(_pgen_tmp_151));
  memset(&_pgen_tmp_152, 0, sizeof(_pgen_tmp_152));
  memset(&_pgen_tmp_153, 0, sizeof(_pgen_tmp_153));
  memset(&_pgen_tmp_154, 0, sizeof(_pgen_tmp_154));
  memset(&_pgen_tmp_155, 0, sizeof(_pgen_tmp_155));
  memset(&_pgen_tmp_156, 0, sizeof(_pgen_tmp_156));
  memset(&_pgen_tmp_157, 0, sizeof(_pgen_tmp_157));
  memset(&_pgen_tmp_158, 0, sizeof(_pgen_tmp_158));
  memset(&_pgen_tmp_159, 0, sizeof(_pgen_tmp_159));
  memset(&_pgen_tmp_160, 0, sizeof(_pgen_tmp_160));
  memset(&_pgen_tmp_161, 0, sizeof(_pgen_tmp_161));
  memset(&_pgen_tmp_162, 0, sizeof(_pgen_tmp_162));
  memset(&_pgen_tmp_163, 0, sizeof(_pgen_tmp_163));
  memset(&_pgen_tmp_164, 0, sizeof(_pgen_tmp_164));
  memset(&_pgen_tmp_165, 0, sizeof(_pgen_tmp_165));
  memset(&_pgen_tmp_166, 0, sizeof(_pgen_tmp_166));
  memset(&_pgen_tmp_167, 0, sizeof(_pgen_tmp_167));
  memset(&_pgen_tmp_168, 0, sizeof(_pgen_tmp_168));
  memset(&_pgen_tmp_169, 0, sizeof(_pgen_tmp_169));
  memset(&_pgen_tmp_170, 0, sizeof(_pgen_tmp_170));
  memset(&_pgen_tmp_171, 0, sizeof(_pgen_tmp_171));
  memset(&_pgen_tmp_172, 0, sizeof(_pgen_tmp_172));
  memset(&_pgen_tmp_173, 0, sizeof(_pgen_tmp_173));
  memset(&_pgen_tmp_174, 0, sizeof(_pgen_tmp_174));
  memset(&_pgen_tmp_175, 0, sizeof(_pgen_tmp_175));
  memset(&_pgen_tmp_176, 0, sizeof(_pgen_tmp_176));
  memset(&_pgen_tmp_177, 0, sizeof(_pgen_tmp_177));
  memset(&_pgen_tmp_178, 0, sizeof(_pgen_tmp_178));
  memset(&_pgen_tmp_179, 0, sizeof(_pgen_tmp_179));
  memset(&_pgen_tmp_180, 0, sizeof(_pgen_tmp_180));
  memset(&_pgen_tmp_181, 0, sizeof(_pgen_tmp_181));
  memset(&_pgen_tmp_182, 0, sizeof(_pgen_tmp_182));
  memset(&_pgen_tmp_183, 0, sizeof(_pgen_tmp_183));
  memset(&_pgen_tmp_184, 0, sizeof(_pgen_tmp_184));
  memset(&_pgen_tmp_185, 0, sizeof(_pgen_tmp_185));
  memset(&_pgen_tmp_186, 0, sizeof(_pgen_tmp_186));
  memset(&_pgen_tmp_187, 0, sizeof(_pgen_tmp_187));
  memset(&_pgen_tmp_188, 0, sizeof(_pgen_tmp_188));
  memset(&_pgen_tmp_189, 0, sizeof(_pgen_tmp_189));
  memset(&_pgen_tmp_190, 0, sizeof(_pgen_tmp_190));
  memset(&_pgen_tmp_191, 0, sizeof(_pgen_tmp_191));
  memset(&_pgen_tmp_192, 0, sizeof(_pgen_tmp_192));
  memset(&_pgen_tmp_193, 0, sizeof(_pgen_tmp_193));
  memset(&_pgen_tmp_194, 0, sizeof(_pgen_tmp_194));
  memset(&_pgen_tmp_195, 0, sizeof(_pgen_tmp_195));
  memset(&_pgen_tmp_196, 0, sizeof(_pgen_tmp_196));
  memset(&_pgen_tmp_197, 0, sizeof(_pgen_tmp_197));
  memset(&_pgen_tmp_198, 0, sizeof(_pgen_tmp_198));
  memset(&_pgen_tmp_199, 0, sizeof(_pgen_tmp_199));
  memset(&_pgen_tmp_200, 0, sizeof(_pgen_tmp_200));
  memset(&_pgen_tmp_201, 0, sizeof(_pgen_tmp_201));
  memset(&_pgen_tmp_202, 0, sizeof(_pgen_tmp_202));
  memset(&_pgen_tmp_203, 0, sizeof(_pgen_tmp_203));
  memset(&_pgen_tmp_204, 0, sizeof(_pgen_tmp_204));
  memset(&_pgen_tmp_205, 0, sizeof(_pgen_tmp_205));
  memset(&_pgen_tmp_206, 0, sizeof(_pgen_tmp_206));
  memset(&_pgen_tmp_207, 0, sizeof(_pgen_tmp_207));
  memset(&_pgen_tmp_208, 0, sizeof(_pgen_tmp_208));
  memset(&_pgen_tmp_209, 0, sizeof(_pgen_tmp_209));
  memset(&_pgen_tmp_210, 0, sizeof(_pgen_tmp_210));
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
  Tm_start("Tensor::conv");
  Zero_ciph(&result_n1);
  _preg_268435545 = Rotate(input, -4096);
  Init_ciph_same_scale(&_pgen_tmp_0, &input, &_preg_268435545);
  _preg_268435546 = Rotate(input, -8192);
  Init_ciph_same_scale(&_pgen_tmp_1, &_pgen_tmp_0, &_preg_268435546);
  _preg_268435547 = Rotate(input, -12288);
  Init_ciph_same_scale(&input_dup_n1, &_pgen_tmp_1, &_preg_268435547);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&input_dup_n1._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_0._c0_poly, _pgen_rns_idx, degree), Coeffs(&input._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435545._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_0._c1_poly, _pgen_rns_idx, degree), Coeffs(&input._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435545._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_1._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_0._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435546._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_1._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_0._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435546._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&input_dup_n1._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_1._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435547._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&input_dup_n1._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_1._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435547._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435545._c1_poly);
    Free_poly_data(&_preg_268435545._c0_poly);
    Free_poly_data(&input._c1_poly);
    Free_poly_data(&input._c0_poly);
    Free_poly_data(&_preg_268435546._c1_poly);
    Free_poly_data(&_preg_268435546._c0_poly);
    Free_poly_data(&_pgen_tmp_0._c1_poly);
    Free_poly_data(&_pgen_tmp_0._c0_poly);
    Free_poly_data(&_preg_268435547._c1_poly);
    Free_poly_data(&_preg_268435547._c0_poly);
    Free_poly_data(&_pgen_tmp_1._c1_poly);
    Free_poly_data(&_pgen_tmp_1._c0_poly);
  }
  for (index_khw1_n1 = 0; index_khw1_n1 < 9; index_khw1_n1 = index_khw1_n1 + 1) {
    _preg_268435548 = Rotate(input_dup_n1, _cst_61[index_khw1_n1]);
    Copy_ciph(&input_roll_n1[index_khw1_n1], &_preg_268435548);
    Free_poly_data(&_preg_268435548._c1_poly);
    Free_poly_data(&_preg_268435548._c0_poly);
  }
  Free_poly_data(&input_dup_n1._c1_poly);
  Free_poly_data(&input_dup_n1._c0_poly);
  for (index_cin_n1 = 0; index_cin_n1 < 4; index_cin_n1 = index_cin_n1 + 1) {
    Zero_ciph(&result_cin_n1);
    for (index_khw2_n1 = 0; index_khw2_n1 < 9; index_khw2_n1 = index_khw2_n1 + 1) {
      _rescale_tmp_65 = input_roll_n1[index_khw2_n1];
      Pt_from_msg(&_pgen_tmp_2, index_khw2_n1 + (index_cin_n1 * 9) + 0 /* cst_59_0 */, 16384, Sc_degree(&_rescale_tmp_65), Level(&_rescale_tmp_65));
      Init_ciph_up_scale_plain(&_pgen_tmp_3, &_rescale_tmp_65, &_pgen_tmp_2);
      Init_ciph_same_scale(&result_cin_n1, &result_cin_n1, &_pgen_tmp_3);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n1._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_3._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_65._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_2._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_3._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_65._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_2._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n1._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n1._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_3._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n1._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n1._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_3._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435549 = Rotate(result_cin_n1, -16384);
    Init_ciph_same_scale(&_pgen_tmp_4, &result_cin_n1, &_preg_268435549);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_4._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_4._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n1._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435549._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_4._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n1._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435549._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435550 = Rotate(_pgen_tmp_4, index_cin_n1 * 1024);
    Init_ciph_same_scale(&result_n1, &result_n1, &_preg_268435550);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n1._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n1._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n1._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435550._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n1._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n1._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435550._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435549._c1_poly);
    Free_poly_data(&_preg_268435549._c0_poly);
    Free_poly_data(&_preg_268435550._c1_poly);
    Free_poly_data(&_preg_268435550._c0_poly);
  }
  Free_ciph_poly(input_roll_n1, 9);
  Free_poly_data(&_pgen_tmp_2._poly);
  Free_poly_data(&_pgen_tmp_3._c1_poly);
  Free_poly_data(&_pgen_tmp_3._c0_poly);
  Free_poly_data(&result_cin_n1._c1_poly);
  Free_poly_data(&result_cin_n1._c0_poly);
  Free_poly_data(&_pgen_tmp_4._c1_poly);
  Free_poly_data(&_pgen_tmp_4._c0_poly);
  Pt_from_msg(&_pgen_tmp_5, 36 /* cst_60 */, 16384, Sc_degree(&result_n1), Level(&result_n1));
  Init_ciph_same_scale_plain(&result_n1, &result_n1, &_pgen_tmp_5);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n1._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n1._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n1._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_5._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n1._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n1._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_5._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_2850, &result_n1);
  Rescale(&_rescale_tmp_2850._c0_poly, &result_n1._c0_poly);
  Rescale(&_rescale_tmp_2850._c1_poly, &result_n1._c1_poly);
  Pt_from_msg(&_pgen_tmp_6, 37 /* cst_62 */, 16384, Sc_degree(&_rescale_tmp_2850), Level(&_rescale_tmp_2850));
  Init_ciph_up_scale_plain(&result_n1, &_rescale_tmp_2850, &_pgen_tmp_6);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n1._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n1._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2850._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_6._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n1._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2850._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_6._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_6._poly);
    Free_poly_data(&_rescale_tmp_2850._c1_poly);
    Free_poly_data(&_rescale_tmp_2850._c0_poly);
  }
  Copy_ciph(&_preg_268435456, &result_n1);
  Free_poly_data(&result_n1._c1_poly);
  Free_poly_data(&result_n1._c0_poly);
  Tm_taken("Tensor::conv");
  Init_ciph_down_scale(&_pgen_tmp_7, &_preg_268435456);
  Rescale(&_pgen_tmp_7._c0_poly, &_preg_268435456._c0_poly);
  Rescale(&_pgen_tmp_7._c1_poly, &_preg_268435456._c1_poly);
  Free_poly_data(&_preg_268435456._c1_poly);
  Free_poly_data(&_preg_268435456._c0_poly);
  Bootstrap(&_preg_268435457, &_pgen_tmp_7, 15);
  Free_poly_data(&_pgen_tmp_7._c1_poly);
  Free_poly_data(&_pgen_tmp_7._c0_poly);
  Encode_plain_from_float(&_pgen_tmp_9, &_cst_241, 1, 1, Level(&_preg_268435457));
  Init_ciph_up_scale_plain(&_pgen_tmp_8, &_preg_268435457, &_pgen_tmp_9);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_8._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_8._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435457._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_9._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_8._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435457._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_9._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_9._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_10, &_pgen_tmp_8);
  Rescale(&_pgen_tmp_10._c0_poly, &_pgen_tmp_8._c0_poly);
  Rescale(&_pgen_tmp_10._c1_poly, &_pgen_tmp_8._c1_poly);
  Free_poly_data(&_pgen_tmp_8._c1_poly);
  Free_poly_data(&_pgen_tmp_8._c0_poly);
  _preg_268435458 = App_relu(_preg_268435457, _pgen_tmp_10);
  Free_poly_data(&_pgen_tmp_10._c1_poly);
  Free_poly_data(&_pgen_tmp_10._c0_poly);
  Free_poly_data(&_preg_268435457._c1_poly);
  Free_poly_data(&_preg_268435457._c0_poly);
  Copy_ciph(&_preg_268435459, &_preg_268435458);
  Free_poly_data(&_preg_268435458._c1_poly);
  Free_poly_data(&_preg_268435458._c0_poly);
  Tm_start("Tensor::conv");
  Zero_ciph(&result_n2);
  Copy_ciph(&input_dup_n2, &_preg_268435459);
  for (index_khw1_n2 = 0; index_khw1_n2 < 9; index_khw1_n2 = index_khw1_n2 + 1) {
    _preg_268435551 = Rotate(input_dup_n2, _cst_67[index_khw1_n2]);
    Copy_ciph(&input_roll_n2[index_khw1_n2], &_preg_268435551);
    Free_poly_data(&_preg_268435551._c1_poly);
    Free_poly_data(&_preg_268435551._c0_poly);
  }
  Free_poly_data(&input_dup_n2._c1_poly);
  Free_poly_data(&input_dup_n2._c0_poly);
  for (index_cin_n2 = 0; index_cin_n2 < 16; index_cin_n2 = index_cin_n2 + 1) {
    Zero_ciph(&result_cin_n2);
    for (index_khw2_n2 = 0; index_khw2_n2 < 9; index_khw2_n2 = index_khw2_n2 + 1) {
      _rescale_tmp_173 = input_roll_n2[index_khw2_n2];
      Pt_from_msg(&_pgen_tmp_11, index_khw2_n2 + (index_cin_n2 * 9) + 38 /* cst_65_0 */, 16384, Sc_degree(&_rescale_tmp_173), Level(&_rescale_tmp_173));
      Init_ciph_up_scale_plain(&_pgen_tmp_12, &_rescale_tmp_173, &_pgen_tmp_11);
      Init_ciph_same_scale(&result_cin_n2, &result_cin_n2, &_pgen_tmp_12);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n2._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_12._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_173._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_11._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_12._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_173._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_11._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n2._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n2._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_12._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n2._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n2._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_12._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435552 = Rotate(result_cin_n2, -16384);
    Init_ciph_same_scale(&_pgen_tmp_13, &result_cin_n2, &_preg_268435552);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_13._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_13._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n2._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435552._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_13._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n2._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435552._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435553 = Rotate(_pgen_tmp_13, index_cin_n2 * 1024);
    Init_ciph_same_scale(&result_n2, &result_n2, &_preg_268435553);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n2._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n2._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n2._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435553._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n2._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n2._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435553._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435552._c1_poly);
    Free_poly_data(&_preg_268435552._c0_poly);
    Free_poly_data(&_preg_268435553._c1_poly);
    Free_poly_data(&_preg_268435553._c0_poly);
  }
  Free_ciph_poly(input_roll_n2, 9);
  Free_poly_data(&_pgen_tmp_11._poly);
  Free_poly_data(&_pgen_tmp_12._c1_poly);
  Free_poly_data(&_pgen_tmp_12._c0_poly);
  Free_poly_data(&result_cin_n2._c1_poly);
  Free_poly_data(&result_cin_n2._c0_poly);
  Free_poly_data(&_pgen_tmp_13._c1_poly);
  Free_poly_data(&_pgen_tmp_13._c0_poly);
  Pt_from_msg(&_pgen_tmp_14, 182 /* cst_66 */, 16384, Sc_degree(&result_n2), Level(&result_n2));
  Init_ciph_same_scale_plain(&result_n2, &result_n2, &_pgen_tmp_14);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n2._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n2._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n2._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_14._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n2._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n2._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_14._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_2863, &result_n2);
  Rescale(&_rescale_tmp_2863._c0_poly, &result_n2._c0_poly);
  Rescale(&_rescale_tmp_2863._c1_poly, &result_n2._c1_poly);
  Pt_from_msg(&_pgen_tmp_15, 183 /* cst_68 */, 16384, Sc_degree(&_rescale_tmp_2863), Level(&_rescale_tmp_2863));
  Init_ciph_up_scale_plain(&result_n2, &_rescale_tmp_2863, &_pgen_tmp_15);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n2._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n2._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2863._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_15._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n2._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2863._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_15._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_15._poly);
    Free_poly_data(&_rescale_tmp_2863._c1_poly);
    Free_poly_data(&_rescale_tmp_2863._c0_poly);
  }
  Copy_ciph(&_preg_268435460, &result_n2);
  Free_poly_data(&result_n2._c1_poly);
  Free_poly_data(&result_n2._c0_poly);
  Tm_taken("Tensor::conv");
  Init_ciph_down_scale(&_pgen_tmp_16, &_preg_268435460);
  Rescale(&_pgen_tmp_16._c0_poly, &_preg_268435460._c0_poly);
  Rescale(&_pgen_tmp_16._c1_poly, &_preg_268435460._c1_poly);
  Free_poly_data(&_preg_268435460._c1_poly);
  Free_poly_data(&_preg_268435460._c0_poly);
  Bootstrap(&_preg_268435461, &_pgen_tmp_16, 15);
  Free_poly_data(&_pgen_tmp_16._c1_poly);
  Free_poly_data(&_pgen_tmp_16._c0_poly);
  Encode_plain_from_float(&_pgen_tmp_18, &_cst_242, 1, 1, Level(&_preg_268435461));
  Init_ciph_up_scale_plain(&_pgen_tmp_17, &_preg_268435461, &_pgen_tmp_18);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_17._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_17._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435461._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_18._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_17._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435461._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_18._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_18._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_19, &_pgen_tmp_17);
  Rescale(&_pgen_tmp_19._c0_poly, &_pgen_tmp_17._c0_poly);
  Rescale(&_pgen_tmp_19._c1_poly, &_pgen_tmp_17._c1_poly);
  Free_poly_data(&_pgen_tmp_17._c1_poly);
  Free_poly_data(&_pgen_tmp_17._c0_poly);
  _preg_268435462 = App_relu(_preg_268435461, _pgen_tmp_19);
  Free_poly_data(&_pgen_tmp_19._c1_poly);
  Free_poly_data(&_pgen_tmp_19._c0_poly);
  Free_poly_data(&_preg_268435461._c1_poly);
  Free_poly_data(&_preg_268435461._c0_poly);
  Copy_ciph(&_preg_268435463, &_preg_268435462);
  Free_poly_data(&_preg_268435462._c1_poly);
  Free_poly_data(&_preg_268435462._c0_poly);
  Tm_start("Tensor::conv");
  Zero_ciph(&result_n3);
  Copy_ciph(&input_dup_n3, &_preg_268435463);
  Free_poly_data(&_preg_268435463._c1_poly);
  Free_poly_data(&_preg_268435463._c0_poly);
  for (index_khw1_n3 = 0; index_khw1_n3 < 9; index_khw1_n3 = index_khw1_n3 + 1) {
    _preg_268435554 = Rotate(input_dup_n3, _cst_73[index_khw1_n3]);
    Copy_ciph(&input_roll_n3[index_khw1_n3], &_preg_268435554);
    Free_poly_data(&_preg_268435554._c1_poly);
    Free_poly_data(&_preg_268435554._c0_poly);
  }
  Free_poly_data(&input_dup_n3._c1_poly);
  Free_poly_data(&input_dup_n3._c0_poly);
  for (index_cin_n3 = 0; index_cin_n3 < 16; index_cin_n3 = index_cin_n3 + 1) {
    Zero_ciph(&result_cin_n3);
    for (index_khw2_n3 = 0; index_khw2_n3 < 9; index_khw2_n3 = index_khw2_n3 + 1) {
      _rescale_tmp_281 = input_roll_n3[index_khw2_n3];
      Pt_from_msg(&_pgen_tmp_20, index_khw2_n3 + (index_cin_n3 * 9) + 184 /* cst_71_0 */, 16384, Sc_degree(&_rescale_tmp_281), Level(&_rescale_tmp_281));
      Init_ciph_up_scale_plain(&_pgen_tmp_21, &_rescale_tmp_281, &_pgen_tmp_20);
      Init_ciph_same_scale(&result_cin_n3, &result_cin_n3, &_pgen_tmp_21);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n3._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_21._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_281._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_20._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_21._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_281._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_20._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n3._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n3._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_21._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n3._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n3._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_21._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435555 = Rotate(result_cin_n3, -16384);
    Init_ciph_same_scale(&_pgen_tmp_22, &result_cin_n3, &_preg_268435555);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_22._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_22._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n3._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435555._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_22._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n3._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435555._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435556 = Rotate(_pgen_tmp_22, index_cin_n3 * 1024);
    Init_ciph_same_scale(&result_n3, &result_n3, &_preg_268435556);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n3._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n3._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n3._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435556._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n3._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n3._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435556._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435555._c1_poly);
    Free_poly_data(&_preg_268435555._c0_poly);
    Free_poly_data(&_preg_268435556._c1_poly);
    Free_poly_data(&_preg_268435556._c0_poly);
  }
  Free_ciph_poly(input_roll_n3, 9);
  Free_poly_data(&_pgen_tmp_20._poly);
  Free_poly_data(&_pgen_tmp_21._c1_poly);
  Free_poly_data(&_pgen_tmp_21._c0_poly);
  Free_poly_data(&result_cin_n3._c1_poly);
  Free_poly_data(&result_cin_n3._c0_poly);
  Free_poly_data(&_pgen_tmp_22._c1_poly);
  Free_poly_data(&_pgen_tmp_22._c0_poly);
  Pt_from_msg(&_pgen_tmp_23, 328 /* cst_72 */, 16384, Sc_degree(&result_n3), Level(&result_n3));
  Init_ciph_same_scale_plain(&result_n3, &result_n3, &_pgen_tmp_23);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n3._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n3._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n3._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_23._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n3._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n3._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_23._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_2876, &result_n3);
  Rescale(&_rescale_tmp_2876._c0_poly, &result_n3._c0_poly);
  Rescale(&_rescale_tmp_2876._c1_poly, &result_n3._c1_poly);
  Pt_from_msg(&_pgen_tmp_24, 329 /* cst_74 */, 16384, Sc_degree(&_rescale_tmp_2876), Level(&_rescale_tmp_2876));
  Init_ciph_up_scale_plain(&result_n3, &_rescale_tmp_2876, &_pgen_tmp_24);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n3._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n3._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2876._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_24._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n3._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2876._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_24._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_24._poly);
    Free_poly_data(&_rescale_tmp_2876._c1_poly);
    Free_poly_data(&_rescale_tmp_2876._c0_poly);
  }
  Copy_ciph(&_preg_268435464, &result_n3);
  Free_poly_data(&result_n3._c1_poly);
  Free_poly_data(&result_n3._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::add");
  Init_ciph_down_scale(&_pgen_tmp_25, &_preg_268435464);
  Rescale(&_pgen_tmp_25._c0_poly, &_preg_268435464._c0_poly);
  Rescale(&_pgen_tmp_25._c1_poly, &_preg_268435464._c1_poly);
  Free_poly_data(&_preg_268435464._c1_poly);
  Free_poly_data(&_preg_268435464._c0_poly);
  Init_ciph_same_scale(&_preg_268435465, &_pgen_tmp_25, &_preg_268435459);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_preg_268435465._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_preg_268435465._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_25._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435459._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_preg_268435465._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_25._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435459._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435459._c1_poly);
    Free_poly_data(&_preg_268435459._c0_poly);
    Free_poly_data(&_pgen_tmp_25._c1_poly);
    Free_poly_data(&_pgen_tmp_25._c0_poly);
  }
  Tm_taken("Tensor::add");
  Bootstrap(&_preg_268435466, &_preg_268435465, 15);
  Free_poly_data(&_preg_268435465._c1_poly);
  Free_poly_data(&_preg_268435465._c0_poly);
  Encode_plain_from_float(&_pgen_tmp_27, &_cst_243, 1, 1, Level(&_preg_268435466));
  Init_ciph_up_scale_plain(&_pgen_tmp_26, &_preg_268435466, &_pgen_tmp_27);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_26._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_26._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435466._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_27._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_26._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435466._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_27._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_27._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_28, &_pgen_tmp_26);
  Rescale(&_pgen_tmp_28._c0_poly, &_pgen_tmp_26._c0_poly);
  Rescale(&_pgen_tmp_28._c1_poly, &_pgen_tmp_26._c1_poly);
  Free_poly_data(&_pgen_tmp_26._c1_poly);
  Free_poly_data(&_pgen_tmp_26._c0_poly);
  _preg_268435467 = App_relu(_preg_268435466, _pgen_tmp_28);
  Free_poly_data(&_pgen_tmp_28._c1_poly);
  Free_poly_data(&_pgen_tmp_28._c0_poly);
  Free_poly_data(&_preg_268435466._c1_poly);
  Free_poly_data(&_preg_268435466._c0_poly);
  Copy_ciph(&_preg_268435468, &_preg_268435467);
  Free_poly_data(&_preg_268435467._c1_poly);
  Free_poly_data(&_preg_268435467._c0_poly);
  Tm_start("Tensor::conv");
  Zero_ciph(&result_n4);
  Copy_ciph(&input_dup_n4, &_preg_268435468);
  for (index_khw1_n4 = 0; index_khw1_n4 < 9; index_khw1_n4 = index_khw1_n4 + 1) {
    _preg_268435557 = Rotate(input_dup_n4, _cst_80[index_khw1_n4]);
    Copy_ciph(&input_roll_n4[index_khw1_n4], &_preg_268435557);
    Free_poly_data(&_preg_268435557._c1_poly);
    Free_poly_data(&_preg_268435557._c0_poly);
  }
  Free_poly_data(&input_dup_n4._c1_poly);
  Free_poly_data(&input_dup_n4._c0_poly);
  for (index_cin_n4 = 0; index_cin_n4 < 16; index_cin_n4 = index_cin_n4 + 1) {
    Zero_ciph(&result_cin_n4);
    for (index_khw2_n4 = 0; index_khw2_n4 < 9; index_khw2_n4 = index_khw2_n4 + 1) {
      _rescale_tmp_395 = input_roll_n4[index_khw2_n4];
      Pt_from_msg(&_pgen_tmp_29, index_khw2_n4 + (index_cin_n4 * 9) + 330 /* cst_78_0 */, 16384, Sc_degree(&_rescale_tmp_395), Level(&_rescale_tmp_395));
      Init_ciph_up_scale_plain(&_pgen_tmp_30, &_rescale_tmp_395, &_pgen_tmp_29);
      Init_ciph_same_scale(&result_cin_n4, &result_cin_n4, &_pgen_tmp_30);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n4._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_30._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_395._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_29._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_30._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_395._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_29._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n4._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n4._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_30._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n4._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n4._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_30._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435558 = Rotate(result_cin_n4, -16384);
    Init_ciph_same_scale(&_pgen_tmp_31, &result_cin_n4, &_preg_268435558);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_31._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_31._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n4._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435558._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_31._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n4._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435558._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435559 = Rotate(_pgen_tmp_31, index_cin_n4 * 1024);
    Init_ciph_same_scale(&result_n4, &result_n4, &_preg_268435559);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n4._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n4._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n4._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435559._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n4._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n4._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435559._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435558._c1_poly);
    Free_poly_data(&_preg_268435558._c0_poly);
    Free_poly_data(&_preg_268435559._c1_poly);
    Free_poly_data(&_preg_268435559._c0_poly);
  }
  Free_ciph_poly(input_roll_n4, 9);
  Free_poly_data(&_pgen_tmp_29._poly);
  Free_poly_data(&_pgen_tmp_30._c1_poly);
  Free_poly_data(&_pgen_tmp_30._c0_poly);
  Free_poly_data(&result_cin_n4._c1_poly);
  Free_poly_data(&result_cin_n4._c0_poly);
  Free_poly_data(&_pgen_tmp_31._c1_poly);
  Free_poly_data(&_pgen_tmp_31._c0_poly);
  Pt_from_msg(&_pgen_tmp_32, 474 /* cst_79 */, 16384, Sc_degree(&result_n4), Level(&result_n4));
  Init_ciph_same_scale_plain(&result_n4, &result_n4, &_pgen_tmp_32);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n4._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n4._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n4._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_32._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n4._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n4._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_32._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_2889, &result_n4);
  Rescale(&_rescale_tmp_2889._c0_poly, &result_n4._c0_poly);
  Rescale(&_rescale_tmp_2889._c1_poly, &result_n4._c1_poly);
  Pt_from_msg(&_pgen_tmp_33, 475 /* cst_81 */, 16384, Sc_degree(&_rescale_tmp_2889), Level(&_rescale_tmp_2889));
  Init_ciph_up_scale_plain(&result_n4, &_rescale_tmp_2889, &_pgen_tmp_33);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n4._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n4._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2889._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_33._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n4._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2889._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_33._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_33._poly);
    Free_poly_data(&_rescale_tmp_2889._c1_poly);
    Free_poly_data(&_rescale_tmp_2889._c0_poly);
  }
  Copy_ciph(&_preg_268435469, &result_n4);
  Free_poly_data(&result_n4._c1_poly);
  Free_poly_data(&result_n4._c0_poly);
  Tm_taken("Tensor::conv");
  Init_ciph_down_scale(&_pgen_tmp_34, &_preg_268435469);
  Rescale(&_pgen_tmp_34._c0_poly, &_preg_268435469._c0_poly);
  Rescale(&_pgen_tmp_34._c1_poly, &_preg_268435469._c1_poly);
  Free_poly_data(&_preg_268435469._c1_poly);
  Free_poly_data(&_preg_268435469._c0_poly);
  Bootstrap(&_preg_268435470, &_pgen_tmp_34, 15);
  Free_poly_data(&_pgen_tmp_34._c1_poly);
  Free_poly_data(&_pgen_tmp_34._c0_poly);
  Encode_plain_from_float(&_pgen_tmp_36, &_cst_244, 1, 1, Level(&_preg_268435470));
  Init_ciph_up_scale_plain(&_pgen_tmp_35, &_preg_268435470, &_pgen_tmp_36);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_35._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_35._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435470._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_36._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_35._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435470._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_36._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_36._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_37, &_pgen_tmp_35);
  Rescale(&_pgen_tmp_37._c0_poly, &_pgen_tmp_35._c0_poly);
  Rescale(&_pgen_tmp_37._c1_poly, &_pgen_tmp_35._c1_poly);
  Free_poly_data(&_pgen_tmp_35._c1_poly);
  Free_poly_data(&_pgen_tmp_35._c0_poly);
  _preg_268435471 = App_relu(_preg_268435470, _pgen_tmp_37);
  Free_poly_data(&_pgen_tmp_37._c1_poly);
  Free_poly_data(&_pgen_tmp_37._c0_poly);
  Free_poly_data(&_preg_268435470._c1_poly);
  Free_poly_data(&_preg_268435470._c0_poly);
  Copy_ciph(&_preg_268435472, &_preg_268435471);
  Free_poly_data(&_preg_268435471._c1_poly);
  Free_poly_data(&_preg_268435471._c0_poly);
  Tm_start("Tensor::conv");
  Zero_ciph(&result_n5);
  Copy_ciph(&input_dup_n5, &_preg_268435472);
  Free_poly_data(&_preg_268435472._c1_poly);
  Free_poly_data(&_preg_268435472._c0_poly);
  for (index_khw1_n5 = 0; index_khw1_n5 < 9; index_khw1_n5 = index_khw1_n5 + 1) {
    _preg_268435560 = Rotate(input_dup_n5, _cst_86[index_khw1_n5]);
    Copy_ciph(&input_roll_n5[index_khw1_n5], &_preg_268435560);
    Free_poly_data(&_preg_268435560._c1_poly);
    Free_poly_data(&_preg_268435560._c0_poly);
  }
  Free_poly_data(&input_dup_n5._c1_poly);
  Free_poly_data(&input_dup_n5._c0_poly);
  for (index_cin_n5 = 0; index_cin_n5 < 16; index_cin_n5 = index_cin_n5 + 1) {
    Zero_ciph(&result_cin_n5);
    for (index_khw2_n5 = 0; index_khw2_n5 < 9; index_khw2_n5 = index_khw2_n5 + 1) {
      _rescale_tmp_503 = input_roll_n5[index_khw2_n5];
      Pt_from_msg(&_pgen_tmp_38, index_khw2_n5 + (index_cin_n5 * 9) + 476 /* cst_84_0 */, 16384, Sc_degree(&_rescale_tmp_503), Level(&_rescale_tmp_503));
      Init_ciph_up_scale_plain(&_pgen_tmp_39, &_rescale_tmp_503, &_pgen_tmp_38);
      Init_ciph_same_scale(&result_cin_n5, &result_cin_n5, &_pgen_tmp_39);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n5._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_39._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_503._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_38._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_39._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_503._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_38._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n5._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n5._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_39._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n5._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n5._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_39._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435561 = Rotate(result_cin_n5, -16384);
    Init_ciph_same_scale(&_pgen_tmp_40, &result_cin_n5, &_preg_268435561);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_40._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_40._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n5._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435561._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_40._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n5._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435561._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435562 = Rotate(_pgen_tmp_40, index_cin_n5 * 1024);
    Init_ciph_same_scale(&result_n5, &result_n5, &_preg_268435562);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n5._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n5._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n5._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435562._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n5._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n5._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435562._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435561._c1_poly);
    Free_poly_data(&_preg_268435561._c0_poly);
    Free_poly_data(&_preg_268435562._c1_poly);
    Free_poly_data(&_preg_268435562._c0_poly);
  }
  Free_ciph_poly(input_roll_n5, 9);
  Free_poly_data(&_pgen_tmp_38._poly);
  Free_poly_data(&_pgen_tmp_39._c1_poly);
  Free_poly_data(&_pgen_tmp_39._c0_poly);
  Free_poly_data(&result_cin_n5._c1_poly);
  Free_poly_data(&result_cin_n5._c0_poly);
  Free_poly_data(&_pgen_tmp_40._c1_poly);
  Free_poly_data(&_pgen_tmp_40._c0_poly);
  Pt_from_msg(&_pgen_tmp_41, 620 /* cst_85 */, 16384, Sc_degree(&result_n5), Level(&result_n5));
  Init_ciph_same_scale_plain(&result_n5, &result_n5, &_pgen_tmp_41);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n5._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n5._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n5._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_41._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n5._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n5._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_41._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_2902, &result_n5);
  Rescale(&_rescale_tmp_2902._c0_poly, &result_n5._c0_poly);
  Rescale(&_rescale_tmp_2902._c1_poly, &result_n5._c1_poly);
  Pt_from_msg(&_pgen_tmp_42, 621 /* cst_87 */, 16384, Sc_degree(&_rescale_tmp_2902), Level(&_rescale_tmp_2902));
  Init_ciph_up_scale_plain(&result_n5, &_rescale_tmp_2902, &_pgen_tmp_42);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n5._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n5._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2902._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_42._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n5._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2902._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_42._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_42._poly);
    Free_poly_data(&_rescale_tmp_2902._c1_poly);
    Free_poly_data(&_rescale_tmp_2902._c0_poly);
  }
  Copy_ciph(&_preg_268435473, &result_n5);
  Free_poly_data(&result_n5._c1_poly);
  Free_poly_data(&result_n5._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::add");
  Init_ciph_down_scale(&_pgen_tmp_43, &_preg_268435473);
  Rescale(&_pgen_tmp_43._c0_poly, &_preg_268435473._c0_poly);
  Rescale(&_pgen_tmp_43._c1_poly, &_preg_268435473._c1_poly);
  Free_poly_data(&_preg_268435473._c1_poly);
  Free_poly_data(&_preg_268435473._c0_poly);
  Init_ciph_same_scale(&_preg_268435474, &_pgen_tmp_43, &_preg_268435468);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_preg_268435474._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_preg_268435474._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_43._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435468._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_preg_268435474._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_43._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435468._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435468._c1_poly);
    Free_poly_data(&_preg_268435468._c0_poly);
    Free_poly_data(&_pgen_tmp_43._c1_poly);
    Free_poly_data(&_pgen_tmp_43._c0_poly);
  }
  Tm_taken("Tensor::add");
  Bootstrap(&_preg_268435475, &_preg_268435474, 15);
  Free_poly_data(&_preg_268435474._c1_poly);
  Free_poly_data(&_preg_268435474._c0_poly);
  Encode_plain_from_float(&_pgen_tmp_45, &_cst_245, 1, 1, Level(&_preg_268435475));
  Init_ciph_up_scale_plain(&_pgen_tmp_44, &_preg_268435475, &_pgen_tmp_45);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_44._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_44._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435475._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_45._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_44._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435475._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_45._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_45._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_46, &_pgen_tmp_44);
  Rescale(&_pgen_tmp_46._c0_poly, &_pgen_tmp_44._c0_poly);
  Rescale(&_pgen_tmp_46._c1_poly, &_pgen_tmp_44._c1_poly);
  Free_poly_data(&_pgen_tmp_44._c1_poly);
  Free_poly_data(&_pgen_tmp_44._c0_poly);
  _preg_268435476 = App_relu(_preg_268435475, _pgen_tmp_46);
  Free_poly_data(&_pgen_tmp_46._c1_poly);
  Free_poly_data(&_pgen_tmp_46._c0_poly);
  Free_poly_data(&_preg_268435475._c1_poly);
  Free_poly_data(&_preg_268435475._c0_poly);
  Copy_ciph(&_preg_268435477, &_preg_268435476);
  Free_poly_data(&_preg_268435476._c1_poly);
  Free_poly_data(&_preg_268435476._c0_poly);
  Tm_start("Tensor::conv");
  Zero_ciph(&result_n6);
  Copy_ciph(&input_dup_n6, &_preg_268435477);
  for (index_khw1_n6 = 0; index_khw1_n6 < 9; index_khw1_n6 = index_khw1_n6 + 1) {
    _preg_268435563 = Rotate(input_dup_n6, _cst_93[index_khw1_n6]);
    Copy_ciph(&input_roll_n6[index_khw1_n6], &_preg_268435563);
    Free_poly_data(&_preg_268435563._c1_poly);
    Free_poly_data(&_preg_268435563._c0_poly);
  }
  Free_poly_data(&input_dup_n6._c1_poly);
  Free_poly_data(&input_dup_n6._c0_poly);
  for (index_cin_n6 = 0; index_cin_n6 < 16; index_cin_n6 = index_cin_n6 + 1) {
    Zero_ciph(&result_cin_n6);
    for (index_khw2_n6 = 0; index_khw2_n6 < 9; index_khw2_n6 = index_khw2_n6 + 1) {
      _rescale_tmp_617 = input_roll_n6[index_khw2_n6];
      Pt_from_msg(&_pgen_tmp_47, index_khw2_n6 + (index_cin_n6 * 9) + 622 /* cst_91_0 */, 16384, Sc_degree(&_rescale_tmp_617), Level(&_rescale_tmp_617));
      Init_ciph_up_scale_plain(&_pgen_tmp_48, &_rescale_tmp_617, &_pgen_tmp_47);
      Init_ciph_same_scale(&result_cin_n6, &result_cin_n6, &_pgen_tmp_48);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n6._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_48._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_617._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_47._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_48._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_617._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_47._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n6._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n6._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_48._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n6._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n6._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_48._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435564 = Rotate(result_cin_n6, -16384);
    Init_ciph_same_scale(&_pgen_tmp_49, &result_cin_n6, &_preg_268435564);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_49._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_49._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n6._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435564._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_49._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n6._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435564._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435565 = Rotate(_pgen_tmp_49, index_cin_n6 * 1024);
    Init_ciph_same_scale(&result_n6, &result_n6, &_preg_268435565);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n6._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n6._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n6._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435565._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n6._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n6._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435565._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435564._c1_poly);
    Free_poly_data(&_preg_268435564._c0_poly);
    Free_poly_data(&_preg_268435565._c1_poly);
    Free_poly_data(&_preg_268435565._c0_poly);
  }
  Free_ciph_poly(input_roll_n6, 9);
  Free_poly_data(&_pgen_tmp_47._poly);
  Free_poly_data(&_pgen_tmp_48._c1_poly);
  Free_poly_data(&_pgen_tmp_48._c0_poly);
  Free_poly_data(&result_cin_n6._c1_poly);
  Free_poly_data(&result_cin_n6._c0_poly);
  Free_poly_data(&_pgen_tmp_49._c1_poly);
  Free_poly_data(&_pgen_tmp_49._c0_poly);
  Pt_from_msg(&_pgen_tmp_50, 766 /* cst_92 */, 16384, Sc_degree(&result_n6), Level(&result_n6));
  Init_ciph_same_scale_plain(&result_n6, &result_n6, &_pgen_tmp_50);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n6._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n6._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n6._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_50._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n6._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n6._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_50._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_2915, &result_n6);
  Rescale(&_rescale_tmp_2915._c0_poly, &result_n6._c0_poly);
  Rescale(&_rescale_tmp_2915._c1_poly, &result_n6._c1_poly);
  Pt_from_msg(&_pgen_tmp_51, 767 /* cst_94 */, 16384, Sc_degree(&_rescale_tmp_2915), Level(&_rescale_tmp_2915));
  Init_ciph_up_scale_plain(&result_n6, &_rescale_tmp_2915, &_pgen_tmp_51);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n6._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n6._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2915._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_51._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n6._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2915._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_51._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_51._poly);
    Free_poly_data(&_rescale_tmp_2915._c1_poly);
    Free_poly_data(&_rescale_tmp_2915._c0_poly);
  }
  Copy_ciph(&_preg_268435478, &result_n6);
  Free_poly_data(&result_n6._c1_poly);
  Free_poly_data(&result_n6._c0_poly);
  Tm_taken("Tensor::conv");
  Init_ciph_down_scale(&_pgen_tmp_52, &_preg_268435478);
  Rescale(&_pgen_tmp_52._c0_poly, &_preg_268435478._c0_poly);
  Rescale(&_pgen_tmp_52._c1_poly, &_preg_268435478._c1_poly);
  Free_poly_data(&_preg_268435478._c1_poly);
  Free_poly_data(&_preg_268435478._c0_poly);
  Bootstrap(&_preg_268435479, &_pgen_tmp_52, 15);
  Free_poly_data(&_pgen_tmp_52._c1_poly);
  Free_poly_data(&_pgen_tmp_52._c0_poly);
  Encode_plain_from_float(&_pgen_tmp_54, &_cst_246, 1, 1, Level(&_preg_268435479));
  Init_ciph_up_scale_plain(&_pgen_tmp_53, &_preg_268435479, &_pgen_tmp_54);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_53._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_53._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435479._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_54._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_53._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435479._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_54._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_54._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_55, &_pgen_tmp_53);
  Rescale(&_pgen_tmp_55._c0_poly, &_pgen_tmp_53._c0_poly);
  Rescale(&_pgen_tmp_55._c1_poly, &_pgen_tmp_53._c1_poly);
  Free_poly_data(&_pgen_tmp_53._c1_poly);
  Free_poly_data(&_pgen_tmp_53._c0_poly);
  _preg_268435480 = App_relu(_preg_268435479, _pgen_tmp_55);
  Free_poly_data(&_pgen_tmp_55._c1_poly);
  Free_poly_data(&_pgen_tmp_55._c0_poly);
  Free_poly_data(&_preg_268435479._c1_poly);
  Free_poly_data(&_preg_268435479._c0_poly);
  Copy_ciph(&_preg_268435481, &_preg_268435480);
  Free_poly_data(&_preg_268435480._c1_poly);
  Free_poly_data(&_preg_268435480._c0_poly);
  Tm_start("Tensor::conv");
  Zero_ciph(&result_n7);
  Copy_ciph(&input_dup_n7, &_preg_268435481);
  Free_poly_data(&_preg_268435481._c1_poly);
  Free_poly_data(&_preg_268435481._c0_poly);
  for (index_khw1_n7 = 0; index_khw1_n7 < 9; index_khw1_n7 = index_khw1_n7 + 1) {
    _preg_268435566 = Rotate(input_dup_n7, _cst_99[index_khw1_n7]);
    Copy_ciph(&input_roll_n7[index_khw1_n7], &_preg_268435566);
    Free_poly_data(&_preg_268435566._c1_poly);
    Free_poly_data(&_preg_268435566._c0_poly);
  }
  Free_poly_data(&input_dup_n7._c1_poly);
  Free_poly_data(&input_dup_n7._c0_poly);
  for (index_cin_n7 = 0; index_cin_n7 < 16; index_cin_n7 = index_cin_n7 + 1) {
    Zero_ciph(&result_cin_n7);
    for (index_khw2_n7 = 0; index_khw2_n7 < 9; index_khw2_n7 = index_khw2_n7 + 1) {
      _rescale_tmp_725 = input_roll_n7[index_khw2_n7];
      Pt_from_msg(&_pgen_tmp_56, index_khw2_n7 + (index_cin_n7 * 9) + 768 /* cst_97_0 */, 16384, Sc_degree(&_rescale_tmp_725), Level(&_rescale_tmp_725));
      Init_ciph_up_scale_plain(&_pgen_tmp_57, &_rescale_tmp_725, &_pgen_tmp_56);
      Init_ciph_same_scale(&result_cin_n7, &result_cin_n7, &_pgen_tmp_57);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n7._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_57._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_725._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_56._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_57._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_725._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_56._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n7._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n7._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_57._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n7._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n7._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_57._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435567 = Rotate(result_cin_n7, -16384);
    Init_ciph_same_scale(&_pgen_tmp_58, &result_cin_n7, &_preg_268435567);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_58._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_58._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n7._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435567._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_58._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n7._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435567._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435568 = Rotate(_pgen_tmp_58, index_cin_n7 * 1024);
    Init_ciph_same_scale(&result_n7, &result_n7, &_preg_268435568);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n7._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n7._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n7._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435568._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n7._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n7._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435568._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435567._c1_poly);
    Free_poly_data(&_preg_268435567._c0_poly);
    Free_poly_data(&_preg_268435568._c1_poly);
    Free_poly_data(&_preg_268435568._c0_poly);
  }
  Free_ciph_poly(input_roll_n7, 9);
  Free_poly_data(&_pgen_tmp_56._poly);
  Free_poly_data(&_pgen_tmp_57._c1_poly);
  Free_poly_data(&_pgen_tmp_57._c0_poly);
  Free_poly_data(&result_cin_n7._c1_poly);
  Free_poly_data(&result_cin_n7._c0_poly);
  Free_poly_data(&_pgen_tmp_58._c1_poly);
  Free_poly_data(&_pgen_tmp_58._c0_poly);
  Pt_from_msg(&_pgen_tmp_59, 912 /* cst_98 */, 16384, Sc_degree(&result_n7), Level(&result_n7));
  Init_ciph_same_scale_plain(&result_n7, &result_n7, &_pgen_tmp_59);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n7._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n7._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n7._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_59._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n7._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n7._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_59._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_2928, &result_n7);
  Rescale(&_rescale_tmp_2928._c0_poly, &result_n7._c0_poly);
  Rescale(&_rescale_tmp_2928._c1_poly, &result_n7._c1_poly);
  Pt_from_msg(&_pgen_tmp_60, 913 /* cst_100 */, 16384, Sc_degree(&_rescale_tmp_2928), Level(&_rescale_tmp_2928));
  Init_ciph_up_scale_plain(&result_n7, &_rescale_tmp_2928, &_pgen_tmp_60);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n7._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n7._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2928._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_60._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n7._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2928._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_60._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_60._poly);
    Free_poly_data(&_rescale_tmp_2928._c1_poly);
    Free_poly_data(&_rescale_tmp_2928._c0_poly);
  }
  Copy_ciph(&_preg_268435482, &result_n7);
  Free_poly_data(&result_n7._c1_poly);
  Free_poly_data(&result_n7._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::add");
  Init_ciph_down_scale(&_pgen_tmp_61, &_preg_268435482);
  Rescale(&_pgen_tmp_61._c0_poly, &_preg_268435482._c0_poly);
  Rescale(&_pgen_tmp_61._c1_poly, &_preg_268435482._c1_poly);
  Free_poly_data(&_preg_268435482._c1_poly);
  Free_poly_data(&_preg_268435482._c0_poly);
  Init_ciph_same_scale(&_preg_268435483, &_pgen_tmp_61, &_preg_268435477);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_preg_268435483._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_preg_268435483._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_61._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435477._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_preg_268435483._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_61._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435477._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435477._c1_poly);
    Free_poly_data(&_preg_268435477._c0_poly);
    Free_poly_data(&_pgen_tmp_61._c1_poly);
    Free_poly_data(&_pgen_tmp_61._c0_poly);
  }
  Tm_taken("Tensor::add");
  Bootstrap(&_preg_268435484, &_preg_268435483, 18);
  Free_poly_data(&_preg_268435483._c1_poly);
  Free_poly_data(&_preg_268435483._c0_poly);
  Encode_plain_from_float(&_pgen_tmp_63, &_cst_247, 1, 1, Level(&_preg_268435484));
  Init_ciph_up_scale_plain(&_pgen_tmp_62, &_preg_268435484, &_pgen_tmp_63);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_62._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_62._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435484._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_63._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_62._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435484._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_63._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_63._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_64, &_pgen_tmp_62);
  Rescale(&_pgen_tmp_64._c0_poly, &_pgen_tmp_62._c0_poly);
  Rescale(&_pgen_tmp_64._c1_poly, &_pgen_tmp_62._c1_poly);
  Free_poly_data(&_pgen_tmp_62._c1_poly);
  Free_poly_data(&_pgen_tmp_62._c0_poly);
  _preg_268435485 = App_relu(_preg_268435484, _pgen_tmp_64);
  Free_poly_data(&_pgen_tmp_64._c1_poly);
  Free_poly_data(&_pgen_tmp_64._c0_poly);
  Free_poly_data(&_preg_268435484._c1_poly);
  Free_poly_data(&_preg_268435484._c0_poly);
  Copy_ciph(&_preg_268435486, &_preg_268435485);
  Free_poly_data(&_preg_268435485._c1_poly);
  Free_poly_data(&_preg_268435485._c0_poly);
  Tm_start("Tensor::conv");
  Zero_ciph(&result_n8);
  _preg_268435569 = Rotate(_preg_268435486, -16384);
  Init_ciph_same_scale(&input_dup_n8, &_preg_268435486, &_preg_268435569);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&input_dup_n8._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&input_dup_n8._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435486._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435569._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&input_dup_n8._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435486._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435569._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435569._c1_poly);
    Free_poly_data(&_preg_268435569._c0_poly);
  }
  for (index_khw1_n8 = 0; index_khw1_n8 < 9; index_khw1_n8 = index_khw1_n8 + 1) {
    _preg_268435570 = Rotate(input_dup_n8, _cst_106[index_khw1_n8]);
    Copy_ciph(&input_roll_n8[index_khw1_n8], &_preg_268435570);
    Free_poly_data(&_preg_268435570._c1_poly);
    Free_poly_data(&_preg_268435570._c0_poly);
  }
  Free_poly_data(&input_dup_n8._c1_poly);
  Free_poly_data(&input_dup_n8._c0_poly);
  for (index_cin_n8 = 0; index_cin_n8 < 16; index_cin_n8 = index_cin_n8 + 1) {
    Zero_ciph(&result_cin_n8);
    for (index_khw2_n8 = 0; index_khw2_n8 < 9; index_khw2_n8 = index_khw2_n8 + 1) {
      _rescale_tmp_843 = input_roll_n8[index_khw2_n8];
      Pt_from_msg(&_pgen_tmp_65, index_khw2_n8 + (index_cin_n8 * 9) + 914 /* cst_104_0 */, 32768, Sc_degree(&_rescale_tmp_843), Level(&_rescale_tmp_843));
      Init_ciph_up_scale_plain(&_pgen_tmp_66, &_rescale_tmp_843, &_pgen_tmp_65);
      Init_ciph_same_scale(&result_cin_n8, &result_cin_n8, &_pgen_tmp_66);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n8._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_66._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_843._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_65._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_66._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_843._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_65._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n8._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n8._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_66._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n8._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n8._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_66._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435571 = Rotate(result_cin_n8, index_cin_n8 * 1024);
    Init_ciph_same_scale(&result_n8, &result_n8, &_preg_268435571);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n8._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n8._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n8._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435571._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n8._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n8._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435571._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435571._c1_poly);
    Free_poly_data(&_preg_268435571._c0_poly);
  }
  Free_ciph_poly(input_roll_n8, 9);
  Free_poly_data(&_pgen_tmp_65._poly);
  Free_poly_data(&_pgen_tmp_66._c1_poly);
  Free_poly_data(&_pgen_tmp_66._c0_poly);
  Free_poly_data(&result_cin_n8._c1_poly);
  Free_poly_data(&result_cin_n8._c0_poly);
  Pt_from_msg(&_pgen_tmp_67, 1058 /* cst_105 */, 32768, Sc_degree(&result_n8), Level(&result_n8));
  Init_ciph_same_scale_plain(&result_n8, &result_n8, &_pgen_tmp_67);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n8._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n8._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n8._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_67._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n8._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n8._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_67._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_2941, &result_n8);
  Rescale(&_rescale_tmp_2941._c0_poly, &result_n8._c0_poly);
  Rescale(&_rescale_tmp_2941._c1_poly, &result_n8._c1_poly);
  Pt_from_msg(&_pgen_tmp_68, 1059 /* cst_107 */, 32768, Sc_degree(&_rescale_tmp_2941), Level(&_rescale_tmp_2941));
  Init_ciph_up_scale_plain(&result_n8, &_rescale_tmp_2941, &_pgen_tmp_68);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n8._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n8._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2941._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_68._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n8._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2941._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_68._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_68._poly);
    Free_poly_data(&_rescale_tmp_2941._c1_poly);
    Free_poly_data(&_rescale_tmp_2941._c0_poly);
  }
  Copy_ciph(&orig_result8, &result_n8);
  Free_poly_data(&result_n8._c1_poly);
  Free_poly_data(&result_n8._c0_poly);
  Zero_ciph(&comb_row_result9);
  for (combine_row_index_n9 = 0; combine_row_index_n9 < 16; combine_row_index_n9 = combine_row_index_n9 + 1) {
    _preg_268435572 = Rotate(orig_result8, combine_row_index_n9 * 1);
    Copy_ciph(&_ckks_gen_tmp_905, &_preg_268435572);
    Init_ciph_down_scale(&_rescale_tmp_2947, &_ckks_gen_tmp_905);
    Rescale(&_rescale_tmp_2947._c0_poly, &_ckks_gen_tmp_905._c0_poly);
    Rescale(&_rescale_tmp_2947._c1_poly, &_ckks_gen_tmp_905._c1_poly);
    Pt_from_msg(&_pgen_tmp_69, combine_row_index_n9 + 1060 /* cst_108_0 */, 32768, Sc_degree(&_rescale_tmp_2947), Level(&_rescale_tmp_2947));
    Init_ciph_up_scale_plain(&_pgen_tmp_70, &_rescale_tmp_2947, &_pgen_tmp_69);
    Init_ciph_same_scale(&comb_row_result9, &comb_row_result9, &_pgen_tmp_70);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&comb_row_result9._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_70._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2947._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_69._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_70._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2947._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_69._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_row_result9._c0_poly, _pgen_rns_idx, degree), Coeffs(&comb_row_result9._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_70._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_row_result9._c1_poly, _pgen_rns_idx, degree), Coeffs(&comb_row_result9._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_70._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435572._c1_poly);
    Free_poly_data(&_preg_268435572._c0_poly);
  }
  Free_poly_data(&orig_result8._c1_poly);
  Free_poly_data(&orig_result8._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_905._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_905._c0_poly);
  Free_poly_data(&_pgen_tmp_69._poly);
  Free_poly_data(&_rescale_tmp_2947._c1_poly);
  Free_poly_data(&_rescale_tmp_2947._c0_poly);
  Free_poly_data(&_pgen_tmp_70._c1_poly);
  Free_poly_data(&_pgen_tmp_70._c0_poly);
  Zero_ciph(&comb_rc_result9);
  for (combine_rc_index_n9 = 0; combine_rc_index_n9 < 16; combine_rc_index_n9 = combine_rc_index_n9 + 1) {
    _preg_268435573 = Rotate(comb_row_result9, combine_rc_index_n9 * 48);
    Copy_ciph(&_ckks_gen_tmp_936, &_preg_268435573);
    Init_ciph_down_scale(&_rescale_tmp_2953, &_ckks_gen_tmp_936);
    Rescale(&_rescale_tmp_2953._c0_poly, &_ckks_gen_tmp_936._c0_poly);
    Rescale(&_rescale_tmp_2953._c1_poly, &_ckks_gen_tmp_936._c1_poly);
    Pt_from_msg(&_pgen_tmp_71, combine_rc_index_n9 + 1076 /* cst_109_0 */, 32768, Sc_degree(&_rescale_tmp_2953), Level(&_rescale_tmp_2953));
    Init_ciph_up_scale_plain(&_pgen_tmp_72, &_rescale_tmp_2953, &_pgen_tmp_71);
    Init_ciph_same_scale(&comb_rc_result9, &comb_rc_result9, &_pgen_tmp_72);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&comb_rc_result9._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_72._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2953._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_71._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_72._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2953._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_71._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_rc_result9._c0_poly, _pgen_rns_idx, degree), Coeffs(&comb_rc_result9._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_72._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_rc_result9._c1_poly, _pgen_rns_idx, degree), Coeffs(&comb_rc_result9._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_72._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435573._c1_poly);
    Free_poly_data(&_preg_268435573._c0_poly);
  }
  Free_poly_data(&comb_row_result9._c1_poly);
  Free_poly_data(&comb_row_result9._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_936._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_936._c0_poly);
  Free_poly_data(&_pgen_tmp_71._poly);
  Free_poly_data(&_rescale_tmp_2953._c1_poly);
  Free_poly_data(&_rescale_tmp_2953._c0_poly);
  Free_poly_data(&_pgen_tmp_72._c1_poly);
  Free_poly_data(&_pgen_tmp_72._c0_poly);
  Zero_ciph(&final_result9);
  for (combine_cc_index_n9 = 0; combine_cc_index_n9 < 32; combine_cc_index_n9 = combine_cc_index_n9 + 1) {
    _preg_268435574 = Rotate(comb_rc_result9, combine_cc_index_n9 * 768);
    Copy_ciph(&_ckks_gen_tmp_967, &_preg_268435574);
    Init_ciph_down_scale(&_rescale_tmp_2959, &_ckks_gen_tmp_967);
    Rescale(&_rescale_tmp_2959._c0_poly, &_ckks_gen_tmp_967._c0_poly);
    Rescale(&_rescale_tmp_2959._c1_poly, &_ckks_gen_tmp_967._c1_poly);
    Pt_from_msg(&_pgen_tmp_73, combine_cc_index_n9 + 1092 /* cst_110_0 */, 32768, Sc_degree(&_rescale_tmp_2959), Level(&_rescale_tmp_2959));
    Init_ciph_up_scale_plain(&_pgen_tmp_74, &_rescale_tmp_2959, &_pgen_tmp_73);
    Init_ciph_same_scale(&final_result9, &final_result9, &_pgen_tmp_74);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&final_result9._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_74._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2959._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_73._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_74._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2959._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_73._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&final_result9._c0_poly, _pgen_rns_idx, degree), Coeffs(&final_result9._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_74._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&final_result9._c1_poly, _pgen_rns_idx, degree), Coeffs(&final_result9._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_74._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435574._c1_poly);
    Free_poly_data(&_preg_268435574._c0_poly);
  }
  Free_poly_data(&comb_rc_result9._c1_poly);
  Free_poly_data(&comb_rc_result9._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_967._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_967._c0_poly);
  Free_poly_data(&_pgen_tmp_73._poly);
  Free_poly_data(&_rescale_tmp_2959._c1_poly);
  Free_poly_data(&_rescale_tmp_2959._c0_poly);
  Free_poly_data(&_pgen_tmp_74._c1_poly);
  Free_poly_data(&_pgen_tmp_74._c0_poly);
  Copy_ciph(&_preg_268435487, &final_result9);
  Free_poly_data(&final_result9._c1_poly);
  Free_poly_data(&final_result9._c0_poly);
  Tm_taken("Tensor::conv");
  Init_ciph_down_scale(&_pgen_tmp_75, &_preg_268435487);
  Rescale(&_pgen_tmp_75._c0_poly, &_preg_268435487._c0_poly);
  Rescale(&_pgen_tmp_75._c1_poly, &_preg_268435487._c1_poly);
  Free_poly_data(&_preg_268435487._c1_poly);
  Free_poly_data(&_preg_268435487._c0_poly);
  Bootstrap(&_preg_268435488, &_pgen_tmp_75, 15);
  Free_poly_data(&_pgen_tmp_75._c1_poly);
  Free_poly_data(&_pgen_tmp_75._c0_poly);
  Encode_plain_from_float(&_pgen_tmp_77, &_cst_248, 1, 1, Level(&_preg_268435488));
  Init_ciph_up_scale_plain(&_pgen_tmp_76, &_preg_268435488, &_pgen_tmp_77);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_76._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_76._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435488._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_77._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_76._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435488._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_77._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_77._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_78, &_pgen_tmp_76);
  Rescale(&_pgen_tmp_78._c0_poly, &_pgen_tmp_76._c0_poly);
  Rescale(&_pgen_tmp_78._c1_poly, &_pgen_tmp_76._c1_poly);
  Free_poly_data(&_pgen_tmp_76._c1_poly);
  Free_poly_data(&_pgen_tmp_76._c0_poly);
  _preg_268435489 = App_relu(_preg_268435488, _pgen_tmp_78);
  Free_poly_data(&_pgen_tmp_78._c1_poly);
  Free_poly_data(&_pgen_tmp_78._c0_poly);
  Free_poly_data(&_preg_268435488._c1_poly);
  Free_poly_data(&_preg_268435488._c0_poly);
  Copy_ciph(&_preg_268435490, &_preg_268435489);
  Free_poly_data(&_preg_268435489._c1_poly);
  Free_poly_data(&_preg_268435489._c0_poly);
  Tm_start("Tensor::conv");
  Zero_ciph(&result_n10);
  Copy_ciph(&input_dup_n10, &_preg_268435490);
  Free_poly_data(&_preg_268435490._c1_poly);
  Free_poly_data(&_preg_268435490._c0_poly);
  for (index_khw1_n10 = 0; index_khw1_n10 < 9; index_khw1_n10 = index_khw1_n10 + 1) {
    _preg_268435575 = Rotate(input_dup_n10, _cst_115[index_khw1_n10]);
    Copy_ciph(&input_roll_n10[index_khw1_n10], &_preg_268435575);
    Free_poly_data(&_preg_268435575._c1_poly);
    Free_poly_data(&_preg_268435575._c0_poly);
  }
  Free_poly_data(&input_dup_n10._c1_poly);
  Free_poly_data(&input_dup_n10._c0_poly);
  for (index_cin_n10 = 0; index_cin_n10 < 32; index_cin_n10 = index_cin_n10 + 1) {
    Zero_ciph(&result_cin_n10);
    for (index_khw2_n10 = 0; index_khw2_n10 < 9; index_khw2_n10 = index_khw2_n10 + 1) {
      _rescale_tmp_1042 = input_roll_n10[index_khw2_n10];
      Pt_from_msg(&_pgen_tmp_79, index_khw2_n10 + (index_cin_n10 * 9) + 1124 /* cst_113_0 */, 8192, Sc_degree(&_rescale_tmp_1042), Level(&_rescale_tmp_1042));
      Init_ciph_up_scale_plain(&_pgen_tmp_80, &_rescale_tmp_1042, &_pgen_tmp_79);
      Init_ciph_same_scale(&result_cin_n10, &result_cin_n10, &_pgen_tmp_80);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n10._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_80._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_1042._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_79._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_80._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_1042._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_79._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n10._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n10._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_80._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n10._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n10._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_80._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435576 = Rotate(result_cin_n10, -8192);
    Init_ciph_same_scale(&_pgen_tmp_81, &result_cin_n10, &_preg_268435576);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_81._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_81._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n10._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435576._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_81._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n10._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435576._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435577 = Rotate(_pgen_tmp_81, index_cin_n10 * 256);
    Init_ciph_same_scale(&result_n10, &result_n10, &_preg_268435577);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n10._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n10._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n10._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435577._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n10._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n10._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435577._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435576._c1_poly);
    Free_poly_data(&_preg_268435576._c0_poly);
    Free_poly_data(&_preg_268435577._c1_poly);
    Free_poly_data(&_preg_268435577._c0_poly);
  }
  Free_ciph_poly(input_roll_n10, 9);
  Free_poly_data(&_pgen_tmp_79._poly);
  Free_poly_data(&_pgen_tmp_80._c1_poly);
  Free_poly_data(&_pgen_tmp_80._c0_poly);
  Free_poly_data(&result_cin_n10._c1_poly);
  Free_poly_data(&result_cin_n10._c0_poly);
  Free_poly_data(&_pgen_tmp_81._c1_poly);
  Free_poly_data(&_pgen_tmp_81._c0_poly);
  Pt_from_msg(&_pgen_tmp_82, 1412 /* cst_114 */, 8192, Sc_degree(&result_n10), Level(&result_n10));
  Init_ciph_same_scale_plain(&result_n10, &result_n10, &_pgen_tmp_82);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n10._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n10._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n10._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_82._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n10._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n10._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_82._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_2972, &result_n10);
  Rescale(&_rescale_tmp_2972._c0_poly, &result_n10._c0_poly);
  Rescale(&_rescale_tmp_2972._c1_poly, &result_n10._c1_poly);
  Pt_from_msg(&_pgen_tmp_83, 1413 /* cst_116 */, 8192, Sc_degree(&_rescale_tmp_2972), Level(&_rescale_tmp_2972));
  Init_ciph_up_scale_plain(&result_n10, &_rescale_tmp_2972, &_pgen_tmp_83);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n10._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n10._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2972._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_83._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n10._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2972._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_83._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_83._poly);
    Free_poly_data(&_rescale_tmp_2972._c1_poly);
    Free_poly_data(&_rescale_tmp_2972._c0_poly);
  }
  Copy_ciph(&_preg_268435491, &result_n10);
  Free_poly_data(&result_n10._c1_poly);
  Free_poly_data(&result_n10._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::conv");
  Zero_ciph(&result_n11);
  _preg_268435578 = Rotate(_preg_268435486, -16384);
  Init_ciph_same_scale(&input_dup_n11, &_preg_268435486, &_preg_268435578);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&input_dup_n11._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&input_dup_n11._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435486._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435578._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&input_dup_n11._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435486._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435578._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435578._c1_poly);
    Free_poly_data(&_preg_268435578._c0_poly);
    Free_poly_data(&_preg_268435486._c1_poly);
    Free_poly_data(&_preg_268435486._c0_poly);
  }
  for (index_khw1_n11 = 0; index_khw1_n11 < 1; index_khw1_n11 = index_khw1_n11 + 1) {
    _preg_268435579 = Rotate(input_dup_n11, _cst_121[index_khw1_n11]);
    Copy_ciph(&input_roll_n11[index_khw1_n11], &_preg_268435579);
    Free_poly_data(&_preg_268435579._c1_poly);
    Free_poly_data(&_preg_268435579._c0_poly);
  }
  Free_poly_data(&input_dup_n11._c1_poly);
  Free_poly_data(&input_dup_n11._c0_poly);
  for (index_cin_n11 = 0; index_cin_n11 < 16; index_cin_n11 = index_cin_n11 + 1) {
    Zero_ciph(&result_cin_n11);
    for (index_khw2_n11 = 0; index_khw2_n11 < 1; index_khw2_n11 = index_khw2_n11 + 1) {
      _rescale_tmp_1144 = input_roll_n11[index_khw2_n11];
      Pt_from_msg(&_pgen_tmp_84, index_khw2_n11 + (index_cin_n11 * 1) + 1414 /* cst_119_0 */, 32768, Sc_degree(&_rescale_tmp_1144), Level(&_rescale_tmp_1144));
      Init_ciph_up_scale_plain(&_pgen_tmp_85, &_rescale_tmp_1144, &_pgen_tmp_84);
      Init_ciph_same_scale(&result_cin_n11, &result_cin_n11, &_pgen_tmp_85);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n11._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_85._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_1144._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_84._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_85._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_1144._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_84._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n11._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n11._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_85._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n11._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n11._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_85._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435580 = Rotate(result_cin_n11, index_cin_n11 * 1024);
    Init_ciph_same_scale(&result_n11, &result_n11, &_preg_268435580);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n11._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n11._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n11._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435580._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n11._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n11._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435580._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435580._c1_poly);
    Free_poly_data(&_preg_268435580._c0_poly);
  }
  Free_ciph_poly(input_roll_n11, 1);
  Free_poly_data(&_pgen_tmp_84._poly);
  Free_poly_data(&_pgen_tmp_85._c1_poly);
  Free_poly_data(&_pgen_tmp_85._c0_poly);
  Free_poly_data(&result_cin_n11._c1_poly);
  Free_poly_data(&result_cin_n11._c0_poly);
  Pt_from_msg(&_pgen_tmp_86, 1430 /* cst_120 */, 32768, Sc_degree(&result_n11), Level(&result_n11));
  Init_ciph_same_scale_plain(&result_n11, &result_n11, &_pgen_tmp_86);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n11._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n11._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n11._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_86._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n11._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n11._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_86._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_2983, &result_n11);
  Rescale(&_rescale_tmp_2983._c0_poly, &result_n11._c0_poly);
  Rescale(&_rescale_tmp_2983._c1_poly, &result_n11._c1_poly);
  Pt_from_msg(&_pgen_tmp_87, 1431 /* cst_122 */, 32768, Sc_degree(&_rescale_tmp_2983), Level(&_rescale_tmp_2983));
  Init_ciph_up_scale_plain(&result_n11, &_rescale_tmp_2983, &_pgen_tmp_87);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n11._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n11._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2983._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_87._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n11._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2983._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_87._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_87._poly);
    Free_poly_data(&_rescale_tmp_2983._c1_poly);
    Free_poly_data(&_rescale_tmp_2983._c0_poly);
  }
  Copy_ciph(&orig_result11, &result_n11);
  Free_poly_data(&result_n11._c1_poly);
  Free_poly_data(&result_n11._c0_poly);
  Zero_ciph(&comb_row_result12);
  for (combine_row_index_n12 = 0; combine_row_index_n12 < 16; combine_row_index_n12 = combine_row_index_n12 + 1) {
    _preg_268435581 = Rotate(orig_result11, combine_row_index_n12 * 1);
    Copy_ciph(&_ckks_gen_tmp_1206, &_preg_268435581);
    Init_ciph_down_scale(&_rescale_tmp_2989, &_ckks_gen_tmp_1206);
    Rescale(&_rescale_tmp_2989._c0_poly, &_ckks_gen_tmp_1206._c0_poly);
    Rescale(&_rescale_tmp_2989._c1_poly, &_ckks_gen_tmp_1206._c1_poly);
    Pt_from_msg(&_pgen_tmp_88, combine_row_index_n12 + 1432 /* cst_123_0 */, 32768, Sc_degree(&_rescale_tmp_2989), Level(&_rescale_tmp_2989));
    Init_ciph_up_scale_plain(&_pgen_tmp_89, &_rescale_tmp_2989, &_pgen_tmp_88);
    Init_ciph_same_scale(&comb_row_result12, &comb_row_result12, &_pgen_tmp_89);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&comb_row_result12._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_89._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2989._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_88._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_89._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2989._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_88._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_row_result12._c0_poly, _pgen_rns_idx, degree), Coeffs(&comb_row_result12._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_89._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_row_result12._c1_poly, _pgen_rns_idx, degree), Coeffs(&comb_row_result12._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_89._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435581._c1_poly);
    Free_poly_data(&_preg_268435581._c0_poly);
  }
  Free_poly_data(&orig_result11._c1_poly);
  Free_poly_data(&orig_result11._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_1206._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_1206._c0_poly);
  Free_poly_data(&_pgen_tmp_88._poly);
  Free_poly_data(&_rescale_tmp_2989._c1_poly);
  Free_poly_data(&_rescale_tmp_2989._c0_poly);
  Free_poly_data(&_pgen_tmp_89._c1_poly);
  Free_poly_data(&_pgen_tmp_89._c0_poly);
  Zero_ciph(&comb_rc_result12);
  for (combine_rc_index_n12 = 0; combine_rc_index_n12 < 16; combine_rc_index_n12 = combine_rc_index_n12 + 1) {
    _preg_268435582 = Rotate(comb_row_result12, combine_rc_index_n12 * 48);
    Copy_ciph(&_ckks_gen_tmp_1237, &_preg_268435582);
    Init_ciph_down_scale(&_rescale_tmp_2995, &_ckks_gen_tmp_1237);
    Rescale(&_rescale_tmp_2995._c0_poly, &_ckks_gen_tmp_1237._c0_poly);
    Rescale(&_rescale_tmp_2995._c1_poly, &_ckks_gen_tmp_1237._c1_poly);
    Pt_from_msg(&_pgen_tmp_90, combine_rc_index_n12 + 1448 /* cst_124_0 */, 32768, Sc_degree(&_rescale_tmp_2995), Level(&_rescale_tmp_2995));
    Init_ciph_up_scale_plain(&_pgen_tmp_91, &_rescale_tmp_2995, &_pgen_tmp_90);
    Init_ciph_same_scale(&comb_rc_result12, &comb_rc_result12, &_pgen_tmp_91);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&comb_rc_result12._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_91._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2995._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_90._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_91._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2995._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_90._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_rc_result12._c0_poly, _pgen_rns_idx, degree), Coeffs(&comb_rc_result12._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_91._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_rc_result12._c1_poly, _pgen_rns_idx, degree), Coeffs(&comb_rc_result12._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_91._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435582._c1_poly);
    Free_poly_data(&_preg_268435582._c0_poly);
  }
  Free_poly_data(&comb_row_result12._c1_poly);
  Free_poly_data(&comb_row_result12._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_1237._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_1237._c0_poly);
  Free_poly_data(&_pgen_tmp_90._poly);
  Free_poly_data(&_rescale_tmp_2995._c1_poly);
  Free_poly_data(&_rescale_tmp_2995._c0_poly);
  Free_poly_data(&_pgen_tmp_91._c1_poly);
  Free_poly_data(&_pgen_tmp_91._c0_poly);
  Zero_ciph(&final_result12);
  for (combine_cc_index_n12 = 0; combine_cc_index_n12 < 32; combine_cc_index_n12 = combine_cc_index_n12 + 1) {
    _preg_268435583 = Rotate(comb_rc_result12, combine_cc_index_n12 * 768);
    Copy_ciph(&_ckks_gen_tmp_1268, &_preg_268435583);
    Init_ciph_down_scale(&_rescale_tmp_3001, &_ckks_gen_tmp_1268);
    Rescale(&_rescale_tmp_3001._c0_poly, &_ckks_gen_tmp_1268._c0_poly);
    Rescale(&_rescale_tmp_3001._c1_poly, &_ckks_gen_tmp_1268._c1_poly);
    Pt_from_msg(&_pgen_tmp_92, combine_cc_index_n12 + 1464 /* cst_125_0 */, 32768, Sc_degree(&_rescale_tmp_3001), Level(&_rescale_tmp_3001));
    Init_ciph_up_scale_plain(&_pgen_tmp_93, &_rescale_tmp_3001, &_pgen_tmp_92);
    Init_ciph_same_scale(&final_result12, &final_result12, &_pgen_tmp_93);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&final_result12._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_93._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3001._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_92._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_93._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3001._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_92._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&final_result12._c0_poly, _pgen_rns_idx, degree), Coeffs(&final_result12._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_93._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&final_result12._c1_poly, _pgen_rns_idx, degree), Coeffs(&final_result12._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_93._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435583._c1_poly);
    Free_poly_data(&_preg_268435583._c0_poly);
  }
  Free_poly_data(&comb_rc_result12._c1_poly);
  Free_poly_data(&comb_rc_result12._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_1268._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_1268._c0_poly);
  Free_poly_data(&_pgen_tmp_92._poly);
  Free_poly_data(&_rescale_tmp_3001._c1_poly);
  Free_poly_data(&_rescale_tmp_3001._c0_poly);
  Free_poly_data(&_pgen_tmp_93._c1_poly);
  Free_poly_data(&_pgen_tmp_93._c0_poly);
  Copy_ciph(&_preg_268435492, &final_result12);
  Free_poly_data(&final_result12._c1_poly);
  Free_poly_data(&final_result12._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::add");
  Init_ciph_same_scale(&_preg_268435493, &_preg_268435491, &_preg_268435492);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_preg_268435493._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_preg_268435493._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435491._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435492._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_preg_268435493._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435491._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435492._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435492._c1_poly);
    Free_poly_data(&_preg_268435492._c0_poly);
    Free_poly_data(&_preg_268435491._c1_poly);
    Free_poly_data(&_preg_268435491._c0_poly);
  }
  Tm_taken("Tensor::add");
  Init_ciph_down_scale(&_pgen_tmp_94, &_preg_268435493);
  Rescale(&_pgen_tmp_94._c0_poly, &_preg_268435493._c0_poly);
  Rescale(&_pgen_tmp_94._c1_poly, &_preg_268435493._c1_poly);
  Free_poly_data(&_preg_268435493._c1_poly);
  Free_poly_data(&_preg_268435493._c0_poly);
  Bootstrap(&_preg_268435494, &_pgen_tmp_94, 15);
  Free_poly_data(&_pgen_tmp_94._c1_poly);
  Free_poly_data(&_pgen_tmp_94._c0_poly);
  Encode_plain_from_float(&_pgen_tmp_96, &_cst_249, 1, 1, Level(&_preg_268435494));
  Init_ciph_up_scale_plain(&_pgen_tmp_95, &_preg_268435494, &_pgen_tmp_96);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_95._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_95._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435494._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_96._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_95._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435494._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_96._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_96._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_97, &_pgen_tmp_95);
  Rescale(&_pgen_tmp_97._c0_poly, &_pgen_tmp_95._c0_poly);
  Rescale(&_pgen_tmp_97._c1_poly, &_pgen_tmp_95._c1_poly);
  Free_poly_data(&_pgen_tmp_95._c1_poly);
  Free_poly_data(&_pgen_tmp_95._c0_poly);
  _preg_268435495 = App_relu(_preg_268435494, _pgen_tmp_97);
  Free_poly_data(&_pgen_tmp_97._c1_poly);
  Free_poly_data(&_pgen_tmp_97._c0_poly);
  Free_poly_data(&_preg_268435494._c1_poly);
  Free_poly_data(&_preg_268435494._c0_poly);
  Copy_ciph(&_preg_268435496, &_preg_268435495);
  Free_poly_data(&_preg_268435495._c1_poly);
  Free_poly_data(&_preg_268435495._c0_poly);
  Tm_start("Tensor::conv");
  Zero_ciph(&result_n13);
  Copy_ciph(&input_dup_n13, &_preg_268435496);
  for (index_khw1_n13 = 0; index_khw1_n13 < 9; index_khw1_n13 = index_khw1_n13 + 1) {
    _preg_268435584 = Rotate(input_dup_n13, _cst_131[index_khw1_n13]);
    Copy_ciph(&input_roll_n13[index_khw1_n13], &_preg_268435584);
    Free_poly_data(&_preg_268435584._c1_poly);
    Free_poly_data(&_preg_268435584._c0_poly);
  }
  Free_poly_data(&input_dup_n13._c1_poly);
  Free_poly_data(&input_dup_n13._c0_poly);
  for (index_cin_n13 = 0; index_cin_n13 < 32; index_cin_n13 = index_cin_n13 + 1) {
    Zero_ciph(&result_cin_n13);
    for (index_khw2_n13 = 0; index_khw2_n13 < 9; index_khw2_n13 = index_khw2_n13 + 1) {
      _rescale_tmp_1349 = input_roll_n13[index_khw2_n13];
      Pt_from_msg(&_pgen_tmp_98, index_khw2_n13 + (index_cin_n13 * 9) + 1496 /* cst_129_0 */, 8192, Sc_degree(&_rescale_tmp_1349), Level(&_rescale_tmp_1349));
      Init_ciph_up_scale_plain(&_pgen_tmp_99, &_rescale_tmp_1349, &_pgen_tmp_98);
      Init_ciph_same_scale(&result_cin_n13, &result_cin_n13, &_pgen_tmp_99);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n13._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_99._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_1349._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_98._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_99._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_1349._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_98._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n13._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n13._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_99._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n13._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n13._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_99._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435585 = Rotate(result_cin_n13, -8192);
    Init_ciph_same_scale(&_pgen_tmp_100, &result_cin_n13, &_preg_268435585);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_100._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_100._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n13._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435585._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_100._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n13._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435585._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435586 = Rotate(_pgen_tmp_100, index_cin_n13 * 256);
    Init_ciph_same_scale(&result_n13, &result_n13, &_preg_268435586);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n13._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n13._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n13._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435586._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n13._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n13._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435586._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435585._c1_poly);
    Free_poly_data(&_preg_268435585._c0_poly);
    Free_poly_data(&_preg_268435586._c1_poly);
    Free_poly_data(&_preg_268435586._c0_poly);
  }
  Free_ciph_poly(input_roll_n13, 9);
  Free_poly_data(&_pgen_tmp_98._poly);
  Free_poly_data(&_pgen_tmp_99._c1_poly);
  Free_poly_data(&_pgen_tmp_99._c0_poly);
  Free_poly_data(&result_cin_n13._c1_poly);
  Free_poly_data(&result_cin_n13._c0_poly);
  Free_poly_data(&_pgen_tmp_100._c1_poly);
  Free_poly_data(&_pgen_tmp_100._c0_poly);
  Pt_from_msg(&_pgen_tmp_101, 1784 /* cst_130 */, 8192, Sc_degree(&result_n13), Level(&result_n13));
  Init_ciph_same_scale_plain(&result_n13, &result_n13, &_pgen_tmp_101);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n13._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n13._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n13._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_101._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n13._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n13._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_101._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_3014, &result_n13);
  Rescale(&_rescale_tmp_3014._c0_poly, &result_n13._c0_poly);
  Rescale(&_rescale_tmp_3014._c1_poly, &result_n13._c1_poly);
  Pt_from_msg(&_pgen_tmp_102, 1785 /* cst_132 */, 8192, Sc_degree(&_rescale_tmp_3014), Level(&_rescale_tmp_3014));
  Init_ciph_up_scale_plain(&result_n13, &_rescale_tmp_3014, &_pgen_tmp_102);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n13._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n13._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3014._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_102._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n13._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3014._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_102._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_102._poly);
    Free_poly_data(&_rescale_tmp_3014._c1_poly);
    Free_poly_data(&_rescale_tmp_3014._c0_poly);
  }
  Copy_ciph(&_preg_268435497, &result_n13);
  Free_poly_data(&result_n13._c1_poly);
  Free_poly_data(&result_n13._c0_poly);
  Tm_taken("Tensor::conv");
  Init_ciph_down_scale(&_pgen_tmp_103, &_preg_268435497);
  Rescale(&_pgen_tmp_103._c0_poly, &_preg_268435497._c0_poly);
  Rescale(&_pgen_tmp_103._c1_poly, &_preg_268435497._c1_poly);
  Free_poly_data(&_preg_268435497._c1_poly);
  Free_poly_data(&_preg_268435497._c0_poly);
  Bootstrap(&_preg_268435498, &_pgen_tmp_103, 15);
  Free_poly_data(&_pgen_tmp_103._c1_poly);
  Free_poly_data(&_pgen_tmp_103._c0_poly);
  Encode_plain_from_float(&_pgen_tmp_105, &_cst_250, 1, 1, Level(&_preg_268435498));
  Init_ciph_up_scale_plain(&_pgen_tmp_104, &_preg_268435498, &_pgen_tmp_105);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_104._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_104._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435498._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_105._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_104._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435498._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_105._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_105._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_106, &_pgen_tmp_104);
  Rescale(&_pgen_tmp_106._c0_poly, &_pgen_tmp_104._c0_poly);
  Rescale(&_pgen_tmp_106._c1_poly, &_pgen_tmp_104._c1_poly);
  Free_poly_data(&_pgen_tmp_104._c1_poly);
  Free_poly_data(&_pgen_tmp_104._c0_poly);
  _preg_268435499 = App_relu(_preg_268435498, _pgen_tmp_106);
  Free_poly_data(&_pgen_tmp_106._c1_poly);
  Free_poly_data(&_pgen_tmp_106._c0_poly);
  Free_poly_data(&_preg_268435498._c1_poly);
  Free_poly_data(&_preg_268435498._c0_poly);
  Copy_ciph(&_preg_268435500, &_preg_268435499);
  Free_poly_data(&_preg_268435499._c1_poly);
  Free_poly_data(&_preg_268435499._c0_poly);
  Tm_start("Tensor::conv");
  Zero_ciph(&result_n14);
  Copy_ciph(&input_dup_n14, &_preg_268435500);
  Free_poly_data(&_preg_268435500._c1_poly);
  Free_poly_data(&_preg_268435500._c0_poly);
  for (index_khw1_n14 = 0; index_khw1_n14 < 9; index_khw1_n14 = index_khw1_n14 + 1) {
    _preg_268435587 = Rotate(input_dup_n14, _cst_137[index_khw1_n14]);
    Copy_ciph(&input_roll_n14[index_khw1_n14], &_preg_268435587);
    Free_poly_data(&_preg_268435587._c1_poly);
    Free_poly_data(&_preg_268435587._c0_poly);
  }
  Free_poly_data(&input_dup_n14._c1_poly);
  Free_poly_data(&input_dup_n14._c0_poly);
  for (index_cin_n14 = 0; index_cin_n14 < 32; index_cin_n14 = index_cin_n14 + 1) {
    Zero_ciph(&result_cin_n14);
    for (index_khw2_n14 = 0; index_khw2_n14 < 9; index_khw2_n14 = index_khw2_n14 + 1) {
      _rescale_tmp_1457 = input_roll_n14[index_khw2_n14];
      Pt_from_msg(&_pgen_tmp_107, index_khw2_n14 + (index_cin_n14 * 9) + 1786 /* cst_135_0 */, 8192, Sc_degree(&_rescale_tmp_1457), Level(&_rescale_tmp_1457));
      Init_ciph_up_scale_plain(&_pgen_tmp_108, &_rescale_tmp_1457, &_pgen_tmp_107);
      Init_ciph_same_scale(&result_cin_n14, &result_cin_n14, &_pgen_tmp_108);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n14._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_108._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_1457._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_107._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_108._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_1457._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_107._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n14._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n14._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_108._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n14._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n14._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_108._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435588 = Rotate(result_cin_n14, -8192);
    Init_ciph_same_scale(&_pgen_tmp_109, &result_cin_n14, &_preg_268435588);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_109._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_109._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n14._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435588._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_109._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n14._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435588._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435589 = Rotate(_pgen_tmp_109, index_cin_n14 * 256);
    Init_ciph_same_scale(&result_n14, &result_n14, &_preg_268435589);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n14._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n14._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n14._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435589._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n14._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n14._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435589._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435588._c1_poly);
    Free_poly_data(&_preg_268435588._c0_poly);
    Free_poly_data(&_preg_268435589._c1_poly);
    Free_poly_data(&_preg_268435589._c0_poly);
  }
  Free_ciph_poly(input_roll_n14, 9);
  Free_poly_data(&_pgen_tmp_107._poly);
  Free_poly_data(&_pgen_tmp_108._c1_poly);
  Free_poly_data(&_pgen_tmp_108._c0_poly);
  Free_poly_data(&result_cin_n14._c1_poly);
  Free_poly_data(&result_cin_n14._c0_poly);
  Free_poly_data(&_pgen_tmp_109._c1_poly);
  Free_poly_data(&_pgen_tmp_109._c0_poly);
  Pt_from_msg(&_pgen_tmp_110, 2074 /* cst_136 */, 8192, Sc_degree(&result_n14), Level(&result_n14));
  Init_ciph_same_scale_plain(&result_n14, &result_n14, &_pgen_tmp_110);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n14._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n14._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n14._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_110._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n14._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n14._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_110._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_3027, &result_n14);
  Rescale(&_rescale_tmp_3027._c0_poly, &result_n14._c0_poly);
  Rescale(&_rescale_tmp_3027._c1_poly, &result_n14._c1_poly);
  Pt_from_msg(&_pgen_tmp_111, 2075 /* cst_138 */, 8192, Sc_degree(&_rescale_tmp_3027), Level(&_rescale_tmp_3027));
  Init_ciph_up_scale_plain(&result_n14, &_rescale_tmp_3027, &_pgen_tmp_111);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n14._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n14._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3027._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_111._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n14._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3027._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_111._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_111._poly);
    Free_poly_data(&_rescale_tmp_3027._c1_poly);
    Free_poly_data(&_rescale_tmp_3027._c0_poly);
  }
  Copy_ciph(&_preg_268435501, &result_n14);
  Free_poly_data(&result_n14._c1_poly);
  Free_poly_data(&result_n14._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::add");
  Init_ciph_down_scale(&_pgen_tmp_112, &_preg_268435501);
  Rescale(&_pgen_tmp_112._c0_poly, &_preg_268435501._c0_poly);
  Rescale(&_pgen_tmp_112._c1_poly, &_preg_268435501._c1_poly);
  Free_poly_data(&_preg_268435501._c1_poly);
  Free_poly_data(&_preg_268435501._c0_poly);
  Init_ciph_same_scale(&_preg_268435502, &_pgen_tmp_112, &_preg_268435496);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_preg_268435502._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_preg_268435502._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_112._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435496._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_preg_268435502._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_112._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435496._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435496._c1_poly);
    Free_poly_data(&_preg_268435496._c0_poly);
    Free_poly_data(&_pgen_tmp_112._c1_poly);
    Free_poly_data(&_pgen_tmp_112._c0_poly);
  }
  Tm_taken("Tensor::add");
  Bootstrap(&_preg_268435503, &_preg_268435502, 15);
  Free_poly_data(&_preg_268435502._c1_poly);
  Free_poly_data(&_preg_268435502._c0_poly);
  Encode_plain_from_float(&_pgen_tmp_114, &_cst_251, 1, 1, Level(&_preg_268435503));
  Init_ciph_up_scale_plain(&_pgen_tmp_113, &_preg_268435503, &_pgen_tmp_114);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_113._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_113._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435503._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_114._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_113._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435503._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_114._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_114._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_115, &_pgen_tmp_113);
  Rescale(&_pgen_tmp_115._c0_poly, &_pgen_tmp_113._c0_poly);
  Rescale(&_pgen_tmp_115._c1_poly, &_pgen_tmp_113._c1_poly);
  Free_poly_data(&_pgen_tmp_113._c1_poly);
  Free_poly_data(&_pgen_tmp_113._c0_poly);
  _preg_268435504 = App_relu(_preg_268435503, _pgen_tmp_115);
  Free_poly_data(&_pgen_tmp_115._c1_poly);
  Free_poly_data(&_pgen_tmp_115._c0_poly);
  Free_poly_data(&_preg_268435503._c1_poly);
  Free_poly_data(&_preg_268435503._c0_poly);
  Copy_ciph(&_preg_268435505, &_preg_268435504);
  Free_poly_data(&_preg_268435504._c1_poly);
  Free_poly_data(&_preg_268435504._c0_poly);
  Tm_start("Tensor::conv");
  Zero_ciph(&result_n15);
  Copy_ciph(&input_dup_n15, &_preg_268435505);
  for (index_khw1_n15 = 0; index_khw1_n15 < 9; index_khw1_n15 = index_khw1_n15 + 1) {
    _preg_268435590 = Rotate(input_dup_n15, _cst_144[index_khw1_n15]);
    Copy_ciph(&input_roll_n15[index_khw1_n15], &_preg_268435590);
    Free_poly_data(&_preg_268435590._c1_poly);
    Free_poly_data(&_preg_268435590._c0_poly);
  }
  Free_poly_data(&input_dup_n15._c1_poly);
  Free_poly_data(&input_dup_n15._c0_poly);
  for (index_cin_n15 = 0; index_cin_n15 < 32; index_cin_n15 = index_cin_n15 + 1) {
    Zero_ciph(&result_cin_n15);
    for (index_khw2_n15 = 0; index_khw2_n15 < 9; index_khw2_n15 = index_khw2_n15 + 1) {
      _rescale_tmp_1571 = input_roll_n15[index_khw2_n15];
      Pt_from_msg(&_pgen_tmp_116, index_khw2_n15 + (index_cin_n15 * 9) + 2076 /* cst_142_0 */, 8192, Sc_degree(&_rescale_tmp_1571), Level(&_rescale_tmp_1571));
      Init_ciph_up_scale_plain(&_pgen_tmp_117, &_rescale_tmp_1571, &_pgen_tmp_116);
      Init_ciph_same_scale(&result_cin_n15, &result_cin_n15, &_pgen_tmp_117);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n15._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_117._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_1571._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_116._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_117._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_1571._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_116._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n15._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n15._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_117._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n15._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n15._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_117._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435591 = Rotate(result_cin_n15, -8192);
    Init_ciph_same_scale(&_pgen_tmp_118, &result_cin_n15, &_preg_268435591);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_118._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_118._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n15._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435591._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_118._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n15._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435591._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435592 = Rotate(_pgen_tmp_118, index_cin_n15 * 256);
    Init_ciph_same_scale(&result_n15, &result_n15, &_preg_268435592);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n15._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n15._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n15._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435592._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n15._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n15._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435592._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435591._c1_poly);
    Free_poly_data(&_preg_268435591._c0_poly);
    Free_poly_data(&_preg_268435592._c1_poly);
    Free_poly_data(&_preg_268435592._c0_poly);
  }
  Free_ciph_poly(input_roll_n15, 9);
  Free_poly_data(&_pgen_tmp_116._poly);
  Free_poly_data(&_pgen_tmp_117._c1_poly);
  Free_poly_data(&_pgen_tmp_117._c0_poly);
  Free_poly_data(&result_cin_n15._c1_poly);
  Free_poly_data(&result_cin_n15._c0_poly);
  Free_poly_data(&_pgen_tmp_118._c1_poly);
  Free_poly_data(&_pgen_tmp_118._c0_poly);
  Pt_from_msg(&_pgen_tmp_119, 2364 /* cst_143 */, 8192, Sc_degree(&result_n15), Level(&result_n15));
  Init_ciph_same_scale_plain(&result_n15, &result_n15, &_pgen_tmp_119);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n15._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n15._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n15._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_119._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n15._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n15._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_119._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_3040, &result_n15);
  Rescale(&_rescale_tmp_3040._c0_poly, &result_n15._c0_poly);
  Rescale(&_rescale_tmp_3040._c1_poly, &result_n15._c1_poly);
  Pt_from_msg(&_pgen_tmp_120, 2365 /* cst_145 */, 8192, Sc_degree(&_rescale_tmp_3040), Level(&_rescale_tmp_3040));
  Init_ciph_up_scale_plain(&result_n15, &_rescale_tmp_3040, &_pgen_tmp_120);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n15._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n15._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3040._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_120._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n15._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3040._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_120._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_120._poly);
    Free_poly_data(&_rescale_tmp_3040._c1_poly);
    Free_poly_data(&_rescale_tmp_3040._c0_poly);
  }
  Copy_ciph(&_preg_268435506, &result_n15);
  Free_poly_data(&result_n15._c1_poly);
  Free_poly_data(&result_n15._c0_poly);
  Tm_taken("Tensor::conv");
  Init_ciph_down_scale(&_pgen_tmp_121, &_preg_268435506);
  Rescale(&_pgen_tmp_121._c0_poly, &_preg_268435506._c0_poly);
  Rescale(&_pgen_tmp_121._c1_poly, &_preg_268435506._c1_poly);
  Free_poly_data(&_preg_268435506._c1_poly);
  Free_poly_data(&_preg_268435506._c0_poly);
  Bootstrap(&_preg_268435507, &_pgen_tmp_121, 15);
  Free_poly_data(&_pgen_tmp_121._c1_poly);
  Free_poly_data(&_pgen_tmp_121._c0_poly);
  Encode_plain_from_float(&_pgen_tmp_123, &_cst_252, 1, 1, Level(&_preg_268435507));
  Init_ciph_up_scale_plain(&_pgen_tmp_122, &_preg_268435507, &_pgen_tmp_123);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_122._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_122._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435507._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_123._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_122._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435507._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_123._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_123._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_124, &_pgen_tmp_122);
  Rescale(&_pgen_tmp_124._c0_poly, &_pgen_tmp_122._c0_poly);
  Rescale(&_pgen_tmp_124._c1_poly, &_pgen_tmp_122._c1_poly);
  Free_poly_data(&_pgen_tmp_122._c1_poly);
  Free_poly_data(&_pgen_tmp_122._c0_poly);
  _preg_268435508 = App_relu(_preg_268435507, _pgen_tmp_124);
  Free_poly_data(&_pgen_tmp_124._c1_poly);
  Free_poly_data(&_pgen_tmp_124._c0_poly);
  Free_poly_data(&_preg_268435507._c1_poly);
  Free_poly_data(&_preg_268435507._c0_poly);
  Copy_ciph(&_preg_268435509, &_preg_268435508);
  Free_poly_data(&_preg_268435508._c1_poly);
  Free_poly_data(&_preg_268435508._c0_poly);
  Tm_start("Tensor::conv");
  Zero_ciph(&result_n16);
  Copy_ciph(&input_dup_n16, &_preg_268435509);
  Free_poly_data(&_preg_268435509._c1_poly);
  Free_poly_data(&_preg_268435509._c0_poly);
  for (index_khw1_n16 = 0; index_khw1_n16 < 9; index_khw1_n16 = index_khw1_n16 + 1) {
    _preg_268435593 = Rotate(input_dup_n16, _cst_150[index_khw1_n16]);
    Copy_ciph(&input_roll_n16[index_khw1_n16], &_preg_268435593);
    Free_poly_data(&_preg_268435593._c1_poly);
    Free_poly_data(&_preg_268435593._c0_poly);
  }
  Free_poly_data(&input_dup_n16._c1_poly);
  Free_poly_data(&input_dup_n16._c0_poly);
  for (index_cin_n16 = 0; index_cin_n16 < 32; index_cin_n16 = index_cin_n16 + 1) {
    Zero_ciph(&result_cin_n16);
    for (index_khw2_n16 = 0; index_khw2_n16 < 9; index_khw2_n16 = index_khw2_n16 + 1) {
      _rescale_tmp_1679 = input_roll_n16[index_khw2_n16];
      Pt_from_msg(&_pgen_tmp_125, index_khw2_n16 + (index_cin_n16 * 9) + 2366 /* cst_148_0 */, 8192, Sc_degree(&_rescale_tmp_1679), Level(&_rescale_tmp_1679));
      Init_ciph_up_scale_plain(&_pgen_tmp_126, &_rescale_tmp_1679, &_pgen_tmp_125);
      Init_ciph_same_scale(&result_cin_n16, &result_cin_n16, &_pgen_tmp_126);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n16._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_126._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_1679._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_125._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_126._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_1679._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_125._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n16._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n16._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_126._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n16._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n16._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_126._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435594 = Rotate(result_cin_n16, -8192);
    Init_ciph_same_scale(&_pgen_tmp_127, &result_cin_n16, &_preg_268435594);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_127._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_127._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n16._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435594._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_127._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n16._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435594._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435595 = Rotate(_pgen_tmp_127, index_cin_n16 * 256);
    Init_ciph_same_scale(&result_n16, &result_n16, &_preg_268435595);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n16._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n16._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n16._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435595._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n16._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n16._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435595._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435594._c1_poly);
    Free_poly_data(&_preg_268435594._c0_poly);
    Free_poly_data(&_preg_268435595._c1_poly);
    Free_poly_data(&_preg_268435595._c0_poly);
  }
  Free_ciph_poly(input_roll_n16, 9);
  Free_poly_data(&_pgen_tmp_125._poly);
  Free_poly_data(&_pgen_tmp_126._c1_poly);
  Free_poly_data(&_pgen_tmp_126._c0_poly);
  Free_poly_data(&result_cin_n16._c1_poly);
  Free_poly_data(&result_cin_n16._c0_poly);
  Free_poly_data(&_pgen_tmp_127._c1_poly);
  Free_poly_data(&_pgen_tmp_127._c0_poly);
  Pt_from_msg(&_pgen_tmp_128, 2654 /* cst_149 */, 8192, Sc_degree(&result_n16), Level(&result_n16));
  Init_ciph_same_scale_plain(&result_n16, &result_n16, &_pgen_tmp_128);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n16._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n16._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n16._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_128._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n16._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n16._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_128._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_3053, &result_n16);
  Rescale(&_rescale_tmp_3053._c0_poly, &result_n16._c0_poly);
  Rescale(&_rescale_tmp_3053._c1_poly, &result_n16._c1_poly);
  Pt_from_msg(&_pgen_tmp_129, 2655 /* cst_151 */, 8192, Sc_degree(&_rescale_tmp_3053), Level(&_rescale_tmp_3053));
  Init_ciph_up_scale_plain(&result_n16, &_rescale_tmp_3053, &_pgen_tmp_129);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n16._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n16._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3053._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_129._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n16._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3053._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_129._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_129._poly);
    Free_poly_data(&_rescale_tmp_3053._c1_poly);
    Free_poly_data(&_rescale_tmp_3053._c0_poly);
  }
  Copy_ciph(&_preg_268435510, &result_n16);
  Free_poly_data(&result_n16._c1_poly);
  Free_poly_data(&result_n16._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::add");
  Init_ciph_down_scale(&_pgen_tmp_130, &_preg_268435510);
  Rescale(&_pgen_tmp_130._c0_poly, &_preg_268435510._c0_poly);
  Rescale(&_pgen_tmp_130._c1_poly, &_preg_268435510._c1_poly);
  Free_poly_data(&_preg_268435510._c1_poly);
  Free_poly_data(&_preg_268435510._c0_poly);
  Init_ciph_same_scale(&_preg_268435511, &_pgen_tmp_130, &_preg_268435505);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_preg_268435511._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_preg_268435511._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_130._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435505._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_preg_268435511._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_130._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435505._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435505._c1_poly);
    Free_poly_data(&_preg_268435505._c0_poly);
    Free_poly_data(&_pgen_tmp_130._c1_poly);
    Free_poly_data(&_pgen_tmp_130._c0_poly);
  }
  Tm_taken("Tensor::add");
  Bootstrap(&_preg_268435512, &_preg_268435511, 18);
  Free_poly_data(&_preg_268435511._c1_poly);
  Free_poly_data(&_preg_268435511._c0_poly);
  Encode_plain_from_float(&_pgen_tmp_132, &_cst_253, 1, 1, Level(&_preg_268435512));
  Init_ciph_up_scale_plain(&_pgen_tmp_131, &_preg_268435512, &_pgen_tmp_132);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_131._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_131._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435512._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_132._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_131._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435512._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_132._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_132._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_133, &_pgen_tmp_131);
  Rescale(&_pgen_tmp_133._c0_poly, &_pgen_tmp_131._c0_poly);
  Rescale(&_pgen_tmp_133._c1_poly, &_pgen_tmp_131._c1_poly);
  Free_poly_data(&_pgen_tmp_131._c1_poly);
  Free_poly_data(&_pgen_tmp_131._c0_poly);
  _preg_268435513 = App_relu(_preg_268435512, _pgen_tmp_133);
  Free_poly_data(&_pgen_tmp_133._c1_poly);
  Free_poly_data(&_pgen_tmp_133._c0_poly);
  Free_poly_data(&_preg_268435512._c1_poly);
  Free_poly_data(&_preg_268435512._c0_poly);
  Copy_ciph(&_preg_268435514, &_preg_268435513);
  Free_poly_data(&_preg_268435513._c1_poly);
  Free_poly_data(&_preg_268435513._c0_poly);
  Tm_start("Tensor::conv");
  Zero_ciph(&result_n17);
  _preg_268435596 = Rotate(_preg_268435514, -8192);
  Init_ciph_same_scale(&input_dup_n17, &_preg_268435514, &_preg_268435596);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&input_dup_n17._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&input_dup_n17._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435514._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435596._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&input_dup_n17._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435514._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435596._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435596._c1_poly);
    Free_poly_data(&_preg_268435596._c0_poly);
  }
  for (index_khw1_n17 = 0; index_khw1_n17 < 9; index_khw1_n17 = index_khw1_n17 + 1) {
    _preg_268435597 = Rotate(input_dup_n17, _cst_157[index_khw1_n17]);
    Copy_ciph(&input_roll_n17[index_khw1_n17], &_preg_268435597);
    Free_poly_data(&_preg_268435597._c1_poly);
    Free_poly_data(&_preg_268435597._c0_poly);
  }
  Free_poly_data(&input_dup_n17._c1_poly);
  Free_poly_data(&input_dup_n17._c0_poly);
  for (index_cin_n17 = 0; index_cin_n17 < 32; index_cin_n17 = index_cin_n17 + 1) {
    Zero_ciph(&result_cin_n17);
    for (index_khw2_n17 = 0; index_khw2_n17 < 9; index_khw2_n17 = index_khw2_n17 + 1) {
      _rescale_tmp_1797 = input_roll_n17[index_khw2_n17];
      Pt_from_msg(&_pgen_tmp_134, index_khw2_n17 + (index_cin_n17 * 9) + 2656 /* cst_155_0 */, 16384, Sc_degree(&_rescale_tmp_1797), Level(&_rescale_tmp_1797));
      Init_ciph_up_scale_plain(&_pgen_tmp_135, &_rescale_tmp_1797, &_pgen_tmp_134);
      Init_ciph_same_scale(&result_cin_n17, &result_cin_n17, &_pgen_tmp_135);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n17._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_135._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_1797._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_134._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_135._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_1797._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_134._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n17._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n17._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_135._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n17._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n17._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_135._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435598 = Rotate(result_cin_n17, -16384);
    Init_ciph_same_scale(&_pgen_tmp_136, &result_cin_n17, &_preg_268435598);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_136._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_136._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n17._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435598._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_136._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n17._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435598._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435599 = Rotate(_pgen_tmp_136, index_cin_n17 * 256);
    Init_ciph_same_scale(&result_n17, &result_n17, &_preg_268435599);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n17._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n17._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n17._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435599._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n17._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n17._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435599._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435598._c1_poly);
    Free_poly_data(&_preg_268435598._c0_poly);
    Free_poly_data(&_preg_268435599._c1_poly);
    Free_poly_data(&_preg_268435599._c0_poly);
  }
  Free_ciph_poly(input_roll_n17, 9);
  Free_poly_data(&_pgen_tmp_134._poly);
  Free_poly_data(&_pgen_tmp_135._c1_poly);
  Free_poly_data(&_pgen_tmp_135._c0_poly);
  Free_poly_data(&result_cin_n17._c1_poly);
  Free_poly_data(&result_cin_n17._c0_poly);
  Free_poly_data(&_pgen_tmp_136._c1_poly);
  Free_poly_data(&_pgen_tmp_136._c0_poly);
  Pt_from_msg(&_pgen_tmp_137, 2944 /* cst_156 */, 16384, Sc_degree(&result_n17), Level(&result_n17));
  Init_ciph_same_scale_plain(&result_n17, &result_n17, &_pgen_tmp_137);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n17._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n17._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n17._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_137._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n17._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n17._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_137._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_3066, &result_n17);
  Rescale(&_rescale_tmp_3066._c0_poly, &result_n17._c0_poly);
  Rescale(&_rescale_tmp_3066._c1_poly, &result_n17._c1_poly);
  Pt_from_msg(&_pgen_tmp_138, 2945 /* cst_158 */, 16384, Sc_degree(&_rescale_tmp_3066), Level(&_rescale_tmp_3066));
  Init_ciph_up_scale_plain(&result_n17, &_rescale_tmp_3066, &_pgen_tmp_138);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n17._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n17._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3066._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_138._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n17._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3066._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_138._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_138._poly);
    Free_poly_data(&_rescale_tmp_3066._c1_poly);
    Free_poly_data(&_rescale_tmp_3066._c0_poly);
  }
  Copy_ciph(&orig_result17, &result_n17);
  Free_poly_data(&result_n17._c1_poly);
  Free_poly_data(&result_n17._c0_poly);
  Zero_ciph(&comb_row_result18);
  for (combine_row_index_n18 = 0; combine_row_index_n18 < 8; combine_row_index_n18 = combine_row_index_n18 + 1) {
    _preg_268435600 = Rotate(orig_result17, combine_row_index_n18 * 1);
    Copy_ciph(&_ckks_gen_tmp_1863, &_preg_268435600);
    Init_ciph_down_scale(&_rescale_tmp_3072, &_ckks_gen_tmp_1863);
    Rescale(&_rescale_tmp_3072._c0_poly, &_ckks_gen_tmp_1863._c0_poly);
    Rescale(&_rescale_tmp_3072._c1_poly, &_ckks_gen_tmp_1863._c1_poly);
    Pt_from_msg(&_pgen_tmp_139, combine_row_index_n18 + 2946 /* cst_159_0 */, 16384, Sc_degree(&_rescale_tmp_3072), Level(&_rescale_tmp_3072));
    Init_ciph_up_scale_plain(&_pgen_tmp_140, &_rescale_tmp_3072, &_pgen_tmp_139);
    Init_ciph_same_scale(&comb_row_result18, &comb_row_result18, &_pgen_tmp_140);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&comb_row_result18._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_140._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3072._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_139._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_140._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3072._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_139._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_row_result18._c0_poly, _pgen_rns_idx, degree), Coeffs(&comb_row_result18._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_140._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_row_result18._c1_poly, _pgen_rns_idx, degree), Coeffs(&comb_row_result18._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_140._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435600._c1_poly);
    Free_poly_data(&_preg_268435600._c0_poly);
  }
  Free_poly_data(&orig_result17._c1_poly);
  Free_poly_data(&orig_result17._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_1863._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_1863._c0_poly);
  Free_poly_data(&_pgen_tmp_139._poly);
  Free_poly_data(&_rescale_tmp_3072._c1_poly);
  Free_poly_data(&_rescale_tmp_3072._c0_poly);
  Free_poly_data(&_pgen_tmp_140._c1_poly);
  Free_poly_data(&_pgen_tmp_140._c0_poly);
  Zero_ciph(&comb_rc_result18);
  for (combine_rc_index_n18 = 0; combine_rc_index_n18 < 8; combine_rc_index_n18 = combine_rc_index_n18 + 1) {
    _preg_268435601 = Rotate(comb_row_result18, combine_rc_index_n18 * 24);
    Copy_ciph(&_ckks_gen_tmp_1894, &_preg_268435601);
    Init_ciph_down_scale(&_rescale_tmp_3078, &_ckks_gen_tmp_1894);
    Rescale(&_rescale_tmp_3078._c0_poly, &_ckks_gen_tmp_1894._c0_poly);
    Rescale(&_rescale_tmp_3078._c1_poly, &_ckks_gen_tmp_1894._c1_poly);
    Pt_from_msg(&_pgen_tmp_141, combine_rc_index_n18 + 2954 /* cst_160_0 */, 16384, Sc_degree(&_rescale_tmp_3078), Level(&_rescale_tmp_3078));
    Init_ciph_up_scale_plain(&_pgen_tmp_142, &_rescale_tmp_3078, &_pgen_tmp_141);
    Init_ciph_same_scale(&comb_rc_result18, &comb_rc_result18, &_pgen_tmp_142);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&comb_rc_result18._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_142._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3078._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_141._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_142._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3078._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_141._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_rc_result18._c0_poly, _pgen_rns_idx, degree), Coeffs(&comb_rc_result18._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_142._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_rc_result18._c1_poly, _pgen_rns_idx, degree), Coeffs(&comb_rc_result18._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_142._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435601._c1_poly);
    Free_poly_data(&_preg_268435601._c0_poly);
  }
  Free_poly_data(&comb_row_result18._c1_poly);
  Free_poly_data(&comb_row_result18._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_1894._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_1894._c0_poly);
  Free_poly_data(&_pgen_tmp_141._poly);
  Free_poly_data(&_rescale_tmp_3078._c1_poly);
  Free_poly_data(&_rescale_tmp_3078._c0_poly);
  Free_poly_data(&_pgen_tmp_142._c1_poly);
  Free_poly_data(&_pgen_tmp_142._c0_poly);
  Zero_ciph(&final_result18);
  for (combine_cc_index_n18 = 0; combine_cc_index_n18 < 64; combine_cc_index_n18 = combine_cc_index_n18 + 1) {
    _preg_268435602 = Rotate(comb_rc_result18, combine_cc_index_n18 * 192);
    Copy_ciph(&_ckks_gen_tmp_1925, &_preg_268435602);
    Init_ciph_down_scale(&_rescale_tmp_3084, &_ckks_gen_tmp_1925);
    Rescale(&_rescale_tmp_3084._c0_poly, &_ckks_gen_tmp_1925._c0_poly);
    Rescale(&_rescale_tmp_3084._c1_poly, &_ckks_gen_tmp_1925._c1_poly);
    Pt_from_msg(&_pgen_tmp_143, combine_cc_index_n18 + 2962 /* cst_161_0 */, 16384, Sc_degree(&_rescale_tmp_3084), Level(&_rescale_tmp_3084));
    Init_ciph_up_scale_plain(&_pgen_tmp_144, &_rescale_tmp_3084, &_pgen_tmp_143);
    Init_ciph_same_scale(&final_result18, &final_result18, &_pgen_tmp_144);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&final_result18._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_144._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3084._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_143._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_144._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3084._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_143._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&final_result18._c0_poly, _pgen_rns_idx, degree), Coeffs(&final_result18._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_144._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&final_result18._c1_poly, _pgen_rns_idx, degree), Coeffs(&final_result18._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_144._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435602._c1_poly);
    Free_poly_data(&_preg_268435602._c0_poly);
  }
  Free_poly_data(&comb_rc_result18._c1_poly);
  Free_poly_data(&comb_rc_result18._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_1925._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_1925._c0_poly);
  Free_poly_data(&_pgen_tmp_143._poly);
  Free_poly_data(&_rescale_tmp_3084._c1_poly);
  Free_poly_data(&_rescale_tmp_3084._c0_poly);
  Free_poly_data(&_pgen_tmp_144._c1_poly);
  Free_poly_data(&_pgen_tmp_144._c0_poly);
  Copy_ciph(&_preg_268435515, &final_result18);
  Free_poly_data(&final_result18._c1_poly);
  Free_poly_data(&final_result18._c0_poly);
  Tm_taken("Tensor::conv");
  Init_ciph_down_scale(&_pgen_tmp_145, &_preg_268435515);
  Rescale(&_pgen_tmp_145._c0_poly, &_preg_268435515._c0_poly);
  Rescale(&_pgen_tmp_145._c1_poly, &_preg_268435515._c1_poly);
  Free_poly_data(&_preg_268435515._c1_poly);
  Free_poly_data(&_preg_268435515._c0_poly);
  Bootstrap(&_preg_268435516, &_pgen_tmp_145, 15);
  Free_poly_data(&_pgen_tmp_145._c1_poly);
  Free_poly_data(&_pgen_tmp_145._c0_poly);
  Encode_plain_from_float(&_pgen_tmp_147, &_cst_254, 1, 1, Level(&_preg_268435516));
  Init_ciph_up_scale_plain(&_pgen_tmp_146, &_preg_268435516, &_pgen_tmp_147);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_146._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_146._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435516._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_147._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_146._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435516._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_147._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_147._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_148, &_pgen_tmp_146);
  Rescale(&_pgen_tmp_148._c0_poly, &_pgen_tmp_146._c0_poly);
  Rescale(&_pgen_tmp_148._c1_poly, &_pgen_tmp_146._c1_poly);
  Free_poly_data(&_pgen_tmp_146._c1_poly);
  Free_poly_data(&_pgen_tmp_146._c0_poly);
  _preg_268435517 = App_relu(_preg_268435516, _pgen_tmp_148);
  Free_poly_data(&_pgen_tmp_148._c1_poly);
  Free_poly_data(&_pgen_tmp_148._c0_poly);
  Free_poly_data(&_preg_268435516._c1_poly);
  Free_poly_data(&_preg_268435516._c0_poly);
  Copy_ciph(&_preg_268435518, &_preg_268435517);
  Free_poly_data(&_preg_268435517._c1_poly);
  Free_poly_data(&_preg_268435517._c0_poly);
  Tm_start("Tensor::conv");
  Zero_ciph(&result_n19);
  Copy_ciph(&input_dup_n19, &_preg_268435518);
  Free_poly_data(&_preg_268435518._c1_poly);
  Free_poly_data(&_preg_268435518._c0_poly);
  for (index_khw1_n19 = 0; index_khw1_n19 < 9; index_khw1_n19 = index_khw1_n19 + 1) {
    _preg_268435603 = Rotate(input_dup_n19, _cst_166[index_khw1_n19]);
    Copy_ciph(&input_roll_n19[index_khw1_n19], &_preg_268435603);
    Free_poly_data(&_preg_268435603._c1_poly);
    Free_poly_data(&_preg_268435603._c0_poly);
  }
  Free_poly_data(&input_dup_n19._c1_poly);
  Free_poly_data(&input_dup_n19._c0_poly);
  for (index_cin_n19 = 0; index_cin_n19 < 64; index_cin_n19 = index_cin_n19 + 1) {
    Zero_ciph(&result_cin_n19);
    for (index_khw2_n19 = 0; index_khw2_n19 < 9; index_khw2_n19 = index_khw2_n19 + 1) {
      _rescale_tmp_2000 = input_roll_n19[index_khw2_n19];
      Pt_from_msg(&_pgen_tmp_149, index_khw2_n19 + (index_cin_n19 * 9) + 3026 /* cst_164_0 */, 4096, Sc_degree(&_rescale_tmp_2000), Level(&_rescale_tmp_2000));
      Init_ciph_up_scale_plain(&_pgen_tmp_150, &_rescale_tmp_2000, &_pgen_tmp_149);
      Init_ciph_same_scale(&result_cin_n19, &result_cin_n19, &_pgen_tmp_150);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n19._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_150._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2000._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_149._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_150._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2000._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_149._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n19._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n19._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_150._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n19._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n19._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_150._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435604 = Rotate(result_cin_n19, -4096);
    Init_ciph_same_scale(&_pgen_tmp_151, &result_cin_n19, &_preg_268435604);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_151._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_151._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n19._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435604._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_151._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n19._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435604._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435605 = Rotate(_pgen_tmp_151, index_cin_n19 * 64);
    Init_ciph_same_scale(&result_n19, &result_n19, &_preg_268435605);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n19._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n19._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n19._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435605._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n19._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n19._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435605._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435604._c1_poly);
    Free_poly_data(&_preg_268435604._c0_poly);
    Free_poly_data(&_preg_268435605._c1_poly);
    Free_poly_data(&_preg_268435605._c0_poly);
  }
  Free_ciph_poly(input_roll_n19, 9);
  Free_poly_data(&_pgen_tmp_149._poly);
  Free_poly_data(&_pgen_tmp_150._c1_poly);
  Free_poly_data(&_pgen_tmp_150._c0_poly);
  Free_poly_data(&result_cin_n19._c1_poly);
  Free_poly_data(&result_cin_n19._c0_poly);
  Free_poly_data(&_pgen_tmp_151._c1_poly);
  Free_poly_data(&_pgen_tmp_151._c0_poly);
  Pt_from_msg(&_pgen_tmp_152, 3602 /* cst_165 */, 4096, Sc_degree(&result_n19), Level(&result_n19));
  Init_ciph_same_scale_plain(&result_n19, &result_n19, &_pgen_tmp_152);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n19._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n19._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n19._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_152._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n19._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n19._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_152._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_3097, &result_n19);
  Rescale(&_rescale_tmp_3097._c0_poly, &result_n19._c0_poly);
  Rescale(&_rescale_tmp_3097._c1_poly, &result_n19._c1_poly);
  Pt_from_msg(&_pgen_tmp_153, 3603 /* cst_167 */, 4096, Sc_degree(&_rescale_tmp_3097), Level(&_rescale_tmp_3097));
  Init_ciph_up_scale_plain(&result_n19, &_rescale_tmp_3097, &_pgen_tmp_153);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n19._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n19._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3097._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_153._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n19._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3097._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_153._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_153._poly);
    Free_poly_data(&_rescale_tmp_3097._c1_poly);
    Free_poly_data(&_rescale_tmp_3097._c0_poly);
  }
  Copy_ciph(&_preg_268435519, &result_n19);
  Free_poly_data(&result_n19._c1_poly);
  Free_poly_data(&result_n19._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::conv");
  Zero_ciph(&result_n20);
  _preg_268435606 = Rotate(_preg_268435514, -8192);
  Init_ciph_same_scale(&input_dup_n20, &_preg_268435514, &_preg_268435606);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&input_dup_n20._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&input_dup_n20._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435514._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435606._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&input_dup_n20._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435514._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435606._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435606._c1_poly);
    Free_poly_data(&_preg_268435606._c0_poly);
    Free_poly_data(&_preg_268435514._c1_poly);
    Free_poly_data(&_preg_268435514._c0_poly);
  }
  for (index_khw1_n20 = 0; index_khw1_n20 < 1; index_khw1_n20 = index_khw1_n20 + 1) {
    _preg_268435607 = Rotate(input_dup_n20, _cst_172[index_khw1_n20]);
    Copy_ciph(&input_roll_n20[index_khw1_n20], &_preg_268435607);
    Free_poly_data(&_preg_268435607._c1_poly);
    Free_poly_data(&_preg_268435607._c0_poly);
  }
  Free_poly_data(&input_dup_n20._c1_poly);
  Free_poly_data(&input_dup_n20._c0_poly);
  for (index_cin_n20 = 0; index_cin_n20 < 32; index_cin_n20 = index_cin_n20 + 1) {
    Zero_ciph(&result_cin_n20);
    for (index_khw2_n20 = 0; index_khw2_n20 < 1; index_khw2_n20 = index_khw2_n20 + 1) {
      _rescale_tmp_2102 = input_roll_n20[index_khw2_n20];
      Pt_from_msg(&_pgen_tmp_154, index_khw2_n20 + (index_cin_n20 * 1) + 3604 /* cst_170_0 */, 16384, Sc_degree(&_rescale_tmp_2102), Level(&_rescale_tmp_2102));
      Init_ciph_up_scale_plain(&_pgen_tmp_155, &_rescale_tmp_2102, &_pgen_tmp_154);
      Init_ciph_same_scale(&result_cin_n20, &result_cin_n20, &_pgen_tmp_155);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n20._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_155._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2102._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_154._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_155._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2102._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_154._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n20._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n20._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_155._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n20._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n20._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_155._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435608 = Rotate(result_cin_n20, -16384);
    Init_ciph_same_scale(&_pgen_tmp_156, &result_cin_n20, &_preg_268435608);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_156._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_156._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n20._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435608._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_156._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n20._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435608._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435609 = Rotate(_pgen_tmp_156, index_cin_n20 * 256);
    Init_ciph_same_scale(&result_n20, &result_n20, &_preg_268435609);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n20._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n20._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n20._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435609._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n20._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n20._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435609._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435608._c1_poly);
    Free_poly_data(&_preg_268435608._c0_poly);
    Free_poly_data(&_preg_268435609._c1_poly);
    Free_poly_data(&_preg_268435609._c0_poly);
  }
  Free_ciph_poly(input_roll_n20, 1);
  Free_poly_data(&_pgen_tmp_154._poly);
  Free_poly_data(&_pgen_tmp_155._c1_poly);
  Free_poly_data(&_pgen_tmp_155._c0_poly);
  Free_poly_data(&result_cin_n20._c1_poly);
  Free_poly_data(&result_cin_n20._c0_poly);
  Free_poly_data(&_pgen_tmp_156._c1_poly);
  Free_poly_data(&_pgen_tmp_156._c0_poly);
  Pt_from_msg(&_pgen_tmp_157, 3636 /* cst_171 */, 16384, Sc_degree(&result_n20), Level(&result_n20));
  Init_ciph_same_scale_plain(&result_n20, &result_n20, &_pgen_tmp_157);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n20._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n20._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n20._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_157._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n20._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n20._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_157._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_3108, &result_n20);
  Rescale(&_rescale_tmp_3108._c0_poly, &result_n20._c0_poly);
  Rescale(&_rescale_tmp_3108._c1_poly, &result_n20._c1_poly);
  Pt_from_msg(&_pgen_tmp_158, 3637 /* cst_173 */, 16384, Sc_degree(&_rescale_tmp_3108), Level(&_rescale_tmp_3108));
  Init_ciph_up_scale_plain(&result_n20, &_rescale_tmp_3108, &_pgen_tmp_158);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n20._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n20._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3108._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_158._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n20._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3108._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_158._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_158._poly);
    Free_poly_data(&_rescale_tmp_3108._c1_poly);
    Free_poly_data(&_rescale_tmp_3108._c0_poly);
  }
  Copy_ciph(&orig_result20, &result_n20);
  Free_poly_data(&result_n20._c1_poly);
  Free_poly_data(&result_n20._c0_poly);
  Zero_ciph(&comb_row_result21);
  for (combine_row_index_n21 = 0; combine_row_index_n21 < 8; combine_row_index_n21 = combine_row_index_n21 + 1) {
    _preg_268435610 = Rotate(orig_result20, combine_row_index_n21 * 1);
    Copy_ciph(&_ckks_gen_tmp_2168, &_preg_268435610);
    Init_ciph_down_scale(&_rescale_tmp_3114, &_ckks_gen_tmp_2168);
    Rescale(&_rescale_tmp_3114._c0_poly, &_ckks_gen_tmp_2168._c0_poly);
    Rescale(&_rescale_tmp_3114._c1_poly, &_ckks_gen_tmp_2168._c1_poly);
    Pt_from_msg(&_pgen_tmp_159, combine_row_index_n21 + 3638 /* cst_174_0 */, 16384, Sc_degree(&_rescale_tmp_3114), Level(&_rescale_tmp_3114));
    Init_ciph_up_scale_plain(&_pgen_tmp_160, &_rescale_tmp_3114, &_pgen_tmp_159);
    Init_ciph_same_scale(&comb_row_result21, &comb_row_result21, &_pgen_tmp_160);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&comb_row_result21._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_160._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3114._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_159._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_160._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3114._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_159._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_row_result21._c0_poly, _pgen_rns_idx, degree), Coeffs(&comb_row_result21._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_160._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_row_result21._c1_poly, _pgen_rns_idx, degree), Coeffs(&comb_row_result21._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_160._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435610._c1_poly);
    Free_poly_data(&_preg_268435610._c0_poly);
  }
  Free_poly_data(&orig_result20._c1_poly);
  Free_poly_data(&orig_result20._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_2168._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_2168._c0_poly);
  Free_poly_data(&_pgen_tmp_159._poly);
  Free_poly_data(&_rescale_tmp_3114._c1_poly);
  Free_poly_data(&_rescale_tmp_3114._c0_poly);
  Free_poly_data(&_pgen_tmp_160._c1_poly);
  Free_poly_data(&_pgen_tmp_160._c0_poly);
  Zero_ciph(&comb_rc_result21);
  for (combine_rc_index_n21 = 0; combine_rc_index_n21 < 8; combine_rc_index_n21 = combine_rc_index_n21 + 1) {
    _preg_268435611 = Rotate(comb_row_result21, combine_rc_index_n21 * 24);
    Copy_ciph(&_ckks_gen_tmp_2199, &_preg_268435611);
    Init_ciph_down_scale(&_rescale_tmp_3120, &_ckks_gen_tmp_2199);
    Rescale(&_rescale_tmp_3120._c0_poly, &_ckks_gen_tmp_2199._c0_poly);
    Rescale(&_rescale_tmp_3120._c1_poly, &_ckks_gen_tmp_2199._c1_poly);
    Pt_from_msg(&_pgen_tmp_161, combine_rc_index_n21 + 3646 /* cst_175_0 */, 16384, Sc_degree(&_rescale_tmp_3120), Level(&_rescale_tmp_3120));
    Init_ciph_up_scale_plain(&_pgen_tmp_162, &_rescale_tmp_3120, &_pgen_tmp_161);
    Init_ciph_same_scale(&comb_rc_result21, &comb_rc_result21, &_pgen_tmp_162);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&comb_rc_result21._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_162._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3120._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_161._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_162._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3120._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_161._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_rc_result21._c0_poly, _pgen_rns_idx, degree), Coeffs(&comb_rc_result21._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_162._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_rc_result21._c1_poly, _pgen_rns_idx, degree), Coeffs(&comb_rc_result21._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_162._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435611._c1_poly);
    Free_poly_data(&_preg_268435611._c0_poly);
  }
  Free_poly_data(&comb_row_result21._c1_poly);
  Free_poly_data(&comb_row_result21._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_2199._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_2199._c0_poly);
  Free_poly_data(&_pgen_tmp_161._poly);
  Free_poly_data(&_rescale_tmp_3120._c1_poly);
  Free_poly_data(&_rescale_tmp_3120._c0_poly);
  Free_poly_data(&_pgen_tmp_162._c1_poly);
  Free_poly_data(&_pgen_tmp_162._c0_poly);
  Zero_ciph(&final_result21);
  for (combine_cc_index_n21 = 0; combine_cc_index_n21 < 64; combine_cc_index_n21 = combine_cc_index_n21 + 1) {
    _preg_268435612 = Rotate(comb_rc_result21, combine_cc_index_n21 * 192);
    Copy_ciph(&_ckks_gen_tmp_2230, &_preg_268435612);
    Init_ciph_down_scale(&_rescale_tmp_3126, &_ckks_gen_tmp_2230);
    Rescale(&_rescale_tmp_3126._c0_poly, &_ckks_gen_tmp_2230._c0_poly);
    Rescale(&_rescale_tmp_3126._c1_poly, &_ckks_gen_tmp_2230._c1_poly);
    Pt_from_msg(&_pgen_tmp_163, combine_cc_index_n21 + 3654 /* cst_176_0 */, 16384, Sc_degree(&_rescale_tmp_3126), Level(&_rescale_tmp_3126));
    Init_ciph_up_scale_plain(&_pgen_tmp_164, &_rescale_tmp_3126, &_pgen_tmp_163);
    Init_ciph_same_scale(&final_result21, &final_result21, &_pgen_tmp_164);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&final_result21._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_164._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3126._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_163._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_164._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3126._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_163._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&final_result21._c0_poly, _pgen_rns_idx, degree), Coeffs(&final_result21._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_164._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&final_result21._c1_poly, _pgen_rns_idx, degree), Coeffs(&final_result21._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_164._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435612._c1_poly);
    Free_poly_data(&_preg_268435612._c0_poly);
  }
  Free_poly_data(&comb_rc_result21._c1_poly);
  Free_poly_data(&comb_rc_result21._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_2230._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_2230._c0_poly);
  Free_poly_data(&_pgen_tmp_163._poly);
  Free_poly_data(&_rescale_tmp_3126._c1_poly);
  Free_poly_data(&_rescale_tmp_3126._c0_poly);
  Free_poly_data(&_pgen_tmp_164._c1_poly);
  Free_poly_data(&_pgen_tmp_164._c0_poly);
  Copy_ciph(&_preg_268435520, &final_result21);
  Free_poly_data(&final_result21._c1_poly);
  Free_poly_data(&final_result21._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::add");
  Init_ciph_same_scale(&_preg_268435521, &_preg_268435519, &_preg_268435520);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_preg_268435521._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_preg_268435521._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435519._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435520._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_preg_268435521._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435519._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435520._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435520._c1_poly);
    Free_poly_data(&_preg_268435520._c0_poly);
    Free_poly_data(&_preg_268435519._c1_poly);
    Free_poly_data(&_preg_268435519._c0_poly);
  }
  Tm_taken("Tensor::add");
  Init_ciph_down_scale(&_pgen_tmp_165, &_preg_268435521);
  Rescale(&_pgen_tmp_165._c0_poly, &_preg_268435521._c0_poly);
  Rescale(&_pgen_tmp_165._c1_poly, &_preg_268435521._c1_poly);
  Free_poly_data(&_preg_268435521._c1_poly);
  Free_poly_data(&_preg_268435521._c0_poly);
  Bootstrap(&_preg_268435522, &_pgen_tmp_165, 15);
  Free_poly_data(&_pgen_tmp_165._c1_poly);
  Free_poly_data(&_pgen_tmp_165._c0_poly);
  Encode_plain_from_float(&_pgen_tmp_167, &_cst_255, 1, 1, Level(&_preg_268435522));
  Init_ciph_up_scale_plain(&_pgen_tmp_166, &_preg_268435522, &_pgen_tmp_167);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_166._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_166._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435522._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_167._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_166._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435522._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_167._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_167._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_168, &_pgen_tmp_166);
  Rescale(&_pgen_tmp_168._c0_poly, &_pgen_tmp_166._c0_poly);
  Rescale(&_pgen_tmp_168._c1_poly, &_pgen_tmp_166._c1_poly);
  Free_poly_data(&_pgen_tmp_166._c1_poly);
  Free_poly_data(&_pgen_tmp_166._c0_poly);
  _preg_268435523 = App_relu(_preg_268435522, _pgen_tmp_168);
  Free_poly_data(&_pgen_tmp_168._c1_poly);
  Free_poly_data(&_pgen_tmp_168._c0_poly);
  Free_poly_data(&_preg_268435522._c1_poly);
  Free_poly_data(&_preg_268435522._c0_poly);
  Copy_ciph(&_preg_268435524, &_preg_268435523);
  Free_poly_data(&_preg_268435523._c1_poly);
  Free_poly_data(&_preg_268435523._c0_poly);
  Tm_start("Tensor::conv");
  Zero_ciph(&result_n22);
  Copy_ciph(&input_dup_n22, &_preg_268435524);
  for (index_khw1_n22 = 0; index_khw1_n22 < 9; index_khw1_n22 = index_khw1_n22 + 1) {
    _preg_268435613 = Rotate(input_dup_n22, _cst_182[index_khw1_n22]);
    Copy_ciph(&input_roll_n22[index_khw1_n22], &_preg_268435613);
    Free_poly_data(&_preg_268435613._c1_poly);
    Free_poly_data(&_preg_268435613._c0_poly);
  }
  Free_poly_data(&input_dup_n22._c1_poly);
  Free_poly_data(&input_dup_n22._c0_poly);
  for (index_cin_n22 = 0; index_cin_n22 < 64; index_cin_n22 = index_cin_n22 + 1) {
    Zero_ciph(&result_cin_n22);
    for (index_khw2_n22 = 0; index_khw2_n22 < 9; index_khw2_n22 = index_khw2_n22 + 1) {
      _rescale_tmp_2311 = input_roll_n22[index_khw2_n22];
      Pt_from_msg(&_pgen_tmp_169, index_khw2_n22 + (index_cin_n22 * 9) + 3718 /* cst_180_0 */, 4096, Sc_degree(&_rescale_tmp_2311), Level(&_rescale_tmp_2311));
      Init_ciph_up_scale_plain(&_pgen_tmp_170, &_rescale_tmp_2311, &_pgen_tmp_169);
      Init_ciph_same_scale(&result_cin_n22, &result_cin_n22, &_pgen_tmp_170);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n22._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_170._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2311._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_169._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_170._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2311._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_169._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n22._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n22._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_170._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n22._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n22._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_170._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435614 = Rotate(result_cin_n22, -4096);
    Init_ciph_same_scale(&_pgen_tmp_171, &result_cin_n22, &_preg_268435614);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_171._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_171._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n22._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435614._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_171._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n22._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435614._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435615 = Rotate(_pgen_tmp_171, index_cin_n22 * 64);
    Init_ciph_same_scale(&result_n22, &result_n22, &_preg_268435615);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n22._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n22._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n22._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435615._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n22._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n22._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435615._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435614._c1_poly);
    Free_poly_data(&_preg_268435614._c0_poly);
    Free_poly_data(&_preg_268435615._c1_poly);
    Free_poly_data(&_preg_268435615._c0_poly);
  }
  Free_ciph_poly(input_roll_n22, 9);
  Free_poly_data(&_pgen_tmp_169._poly);
  Free_poly_data(&_pgen_tmp_170._c1_poly);
  Free_poly_data(&_pgen_tmp_170._c0_poly);
  Free_poly_data(&result_cin_n22._c1_poly);
  Free_poly_data(&result_cin_n22._c0_poly);
  Free_poly_data(&_pgen_tmp_171._c1_poly);
  Free_poly_data(&_pgen_tmp_171._c0_poly);
  Pt_from_msg(&_pgen_tmp_172, 4294 /* cst_181 */, 4096, Sc_degree(&result_n22), Level(&result_n22));
  Init_ciph_same_scale_plain(&result_n22, &result_n22, &_pgen_tmp_172);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n22._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n22._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n22._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_172._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n22._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n22._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_172._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_3139, &result_n22);
  Rescale(&_rescale_tmp_3139._c0_poly, &result_n22._c0_poly);
  Rescale(&_rescale_tmp_3139._c1_poly, &result_n22._c1_poly);
  Pt_from_msg(&_pgen_tmp_173, 4295 /* cst_183 */, 4096, Sc_degree(&_rescale_tmp_3139), Level(&_rescale_tmp_3139));
  Init_ciph_up_scale_plain(&result_n22, &_rescale_tmp_3139, &_pgen_tmp_173);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n22._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n22._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3139._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_173._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n22._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3139._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_173._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_173._poly);
    Free_poly_data(&_rescale_tmp_3139._c1_poly);
    Free_poly_data(&_rescale_tmp_3139._c0_poly);
  }
  Copy_ciph(&_preg_268435525, &result_n22);
  Free_poly_data(&result_n22._c1_poly);
  Free_poly_data(&result_n22._c0_poly);
  Tm_taken("Tensor::conv");
  Init_ciph_down_scale(&_pgen_tmp_174, &_preg_268435525);
  Rescale(&_pgen_tmp_174._c0_poly, &_preg_268435525._c0_poly);
  Rescale(&_pgen_tmp_174._c1_poly, &_preg_268435525._c1_poly);
  Free_poly_data(&_preg_268435525._c1_poly);
  Free_poly_data(&_preg_268435525._c0_poly);
  Bootstrap(&_preg_268435526, &_pgen_tmp_174, 15);
  Free_poly_data(&_pgen_tmp_174._c1_poly);
  Free_poly_data(&_pgen_tmp_174._c0_poly);
  Encode_plain_from_float(&_pgen_tmp_176, &_cst_256, 1, 1, Level(&_preg_268435526));
  Init_ciph_up_scale_plain(&_pgen_tmp_175, &_preg_268435526, &_pgen_tmp_176);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_175._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_175._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435526._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_176._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_175._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435526._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_176._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_176._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_177, &_pgen_tmp_175);
  Rescale(&_pgen_tmp_177._c0_poly, &_pgen_tmp_175._c0_poly);
  Rescale(&_pgen_tmp_177._c1_poly, &_pgen_tmp_175._c1_poly);
  Free_poly_data(&_pgen_tmp_175._c1_poly);
  Free_poly_data(&_pgen_tmp_175._c0_poly);
  _preg_268435527 = App_relu(_preg_268435526, _pgen_tmp_177);
  Free_poly_data(&_pgen_tmp_177._c1_poly);
  Free_poly_data(&_pgen_tmp_177._c0_poly);
  Free_poly_data(&_preg_268435526._c1_poly);
  Free_poly_data(&_preg_268435526._c0_poly);
  Copy_ciph(&_preg_268435528, &_preg_268435527);
  Free_poly_data(&_preg_268435527._c1_poly);
  Free_poly_data(&_preg_268435527._c0_poly);
  Tm_start("Tensor::conv");
  Zero_ciph(&result_n23);
  Copy_ciph(&input_dup_n23, &_preg_268435528);
  Free_poly_data(&_preg_268435528._c1_poly);
  Free_poly_data(&_preg_268435528._c0_poly);
  for (index_khw1_n23 = 0; index_khw1_n23 < 9; index_khw1_n23 = index_khw1_n23 + 1) {
    _preg_268435616 = Rotate(input_dup_n23, _cst_188[index_khw1_n23]);
    Copy_ciph(&input_roll_n23[index_khw1_n23], &_preg_268435616);
    Free_poly_data(&_preg_268435616._c1_poly);
    Free_poly_data(&_preg_268435616._c0_poly);
  }
  Free_poly_data(&input_dup_n23._c1_poly);
  Free_poly_data(&input_dup_n23._c0_poly);
  for (index_cin_n23 = 0; index_cin_n23 < 64; index_cin_n23 = index_cin_n23 + 1) {
    Zero_ciph(&result_cin_n23);
    for (index_khw2_n23 = 0; index_khw2_n23 < 9; index_khw2_n23 = index_khw2_n23 + 1) {
      _rescale_tmp_2419 = input_roll_n23[index_khw2_n23];
      Pt_from_msg(&_pgen_tmp_178, index_khw2_n23 + (index_cin_n23 * 9) + 4296 /* cst_186_0 */, 4096, Sc_degree(&_rescale_tmp_2419), Level(&_rescale_tmp_2419));
      Init_ciph_up_scale_plain(&_pgen_tmp_179, &_rescale_tmp_2419, &_pgen_tmp_178);
      Init_ciph_same_scale(&result_cin_n23, &result_cin_n23, &_pgen_tmp_179);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n23._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_179._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2419._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_178._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_179._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2419._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_178._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n23._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n23._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_179._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n23._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n23._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_179._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435617 = Rotate(result_cin_n23, -4096);
    Init_ciph_same_scale(&_pgen_tmp_180, &result_cin_n23, &_preg_268435617);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_180._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_180._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n23._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435617._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_180._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n23._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435617._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435618 = Rotate(_pgen_tmp_180, index_cin_n23 * 64);
    Init_ciph_same_scale(&result_n23, &result_n23, &_preg_268435618);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n23._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n23._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n23._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435618._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n23._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n23._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435618._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435617._c1_poly);
    Free_poly_data(&_preg_268435617._c0_poly);
    Free_poly_data(&_preg_268435618._c1_poly);
    Free_poly_data(&_preg_268435618._c0_poly);
  }
  Free_ciph_poly(input_roll_n23, 9);
  Free_poly_data(&_pgen_tmp_178._poly);
  Free_poly_data(&_pgen_tmp_179._c1_poly);
  Free_poly_data(&_pgen_tmp_179._c0_poly);
  Free_poly_data(&result_cin_n23._c1_poly);
  Free_poly_data(&result_cin_n23._c0_poly);
  Free_poly_data(&_pgen_tmp_180._c1_poly);
  Free_poly_data(&_pgen_tmp_180._c0_poly);
  Pt_from_msg(&_pgen_tmp_181, 4872 /* cst_187 */, 4096, Sc_degree(&result_n23), Level(&result_n23));
  Init_ciph_same_scale_plain(&result_n23, &result_n23, &_pgen_tmp_181);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n23._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n23._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n23._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_181._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n23._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n23._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_181._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_3152, &result_n23);
  Rescale(&_rescale_tmp_3152._c0_poly, &result_n23._c0_poly);
  Rescale(&_rescale_tmp_3152._c1_poly, &result_n23._c1_poly);
  Pt_from_msg(&_pgen_tmp_182, 4873 /* cst_189 */, 4096, Sc_degree(&_rescale_tmp_3152), Level(&_rescale_tmp_3152));
  Init_ciph_up_scale_plain(&result_n23, &_rescale_tmp_3152, &_pgen_tmp_182);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n23._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n23._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3152._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_182._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n23._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3152._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_182._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_182._poly);
    Free_poly_data(&_rescale_tmp_3152._c1_poly);
    Free_poly_data(&_rescale_tmp_3152._c0_poly);
  }
  Copy_ciph(&_preg_268435529, &result_n23);
  Free_poly_data(&result_n23._c1_poly);
  Free_poly_data(&result_n23._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::add");
  Init_ciph_down_scale(&_pgen_tmp_183, &_preg_268435529);
  Rescale(&_pgen_tmp_183._c0_poly, &_preg_268435529._c0_poly);
  Rescale(&_pgen_tmp_183._c1_poly, &_preg_268435529._c1_poly);
  Free_poly_data(&_preg_268435529._c1_poly);
  Free_poly_data(&_preg_268435529._c0_poly);
  Init_ciph_same_scale(&_preg_268435530, &_pgen_tmp_183, &_preg_268435524);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_preg_268435530._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_preg_268435530._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_183._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435524._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_preg_268435530._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_183._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435524._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435524._c1_poly);
    Free_poly_data(&_preg_268435524._c0_poly);
    Free_poly_data(&_pgen_tmp_183._c1_poly);
    Free_poly_data(&_pgen_tmp_183._c0_poly);
  }
  Tm_taken("Tensor::add");
  Bootstrap(&_preg_268435531, &_preg_268435530, 15);
  Free_poly_data(&_preg_268435530._c1_poly);
  Free_poly_data(&_preg_268435530._c0_poly);
  Encode_plain_from_float(&_pgen_tmp_185, &_cst_257, 1, 1, Level(&_preg_268435531));
  Init_ciph_up_scale_plain(&_pgen_tmp_184, &_preg_268435531, &_pgen_tmp_185);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_184._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_184._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435531._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_185._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_184._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435531._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_185._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_185._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_186, &_pgen_tmp_184);
  Rescale(&_pgen_tmp_186._c0_poly, &_pgen_tmp_184._c0_poly);
  Rescale(&_pgen_tmp_186._c1_poly, &_pgen_tmp_184._c1_poly);
  Free_poly_data(&_pgen_tmp_184._c1_poly);
  Free_poly_data(&_pgen_tmp_184._c0_poly);
  _preg_268435532 = App_relu(_preg_268435531, _pgen_tmp_186);
  Free_poly_data(&_pgen_tmp_186._c1_poly);
  Free_poly_data(&_pgen_tmp_186._c0_poly);
  Free_poly_data(&_preg_268435531._c1_poly);
  Free_poly_data(&_preg_268435531._c0_poly);
  Copy_ciph(&_preg_268435533, &_preg_268435532);
  Free_poly_data(&_preg_268435532._c1_poly);
  Free_poly_data(&_preg_268435532._c0_poly);
  Tm_start("Tensor::conv");
  Zero_ciph(&result_n24);
  Copy_ciph(&input_dup_n24, &_preg_268435533);
  for (index_khw1_n24 = 0; index_khw1_n24 < 9; index_khw1_n24 = index_khw1_n24 + 1) {
    _preg_268435619 = Rotate(input_dup_n24, _cst_195[index_khw1_n24]);
    Copy_ciph(&input_roll_n24[index_khw1_n24], &_preg_268435619);
    Free_poly_data(&_preg_268435619._c1_poly);
    Free_poly_data(&_preg_268435619._c0_poly);
  }
  Free_poly_data(&input_dup_n24._c1_poly);
  Free_poly_data(&input_dup_n24._c0_poly);
  for (index_cin_n24 = 0; index_cin_n24 < 64; index_cin_n24 = index_cin_n24 + 1) {
    Zero_ciph(&result_cin_n24);
    for (index_khw2_n24 = 0; index_khw2_n24 < 9; index_khw2_n24 = index_khw2_n24 + 1) {
      _rescale_tmp_2533 = input_roll_n24[index_khw2_n24];
      Pt_from_msg(&_pgen_tmp_187, index_khw2_n24 + (index_cin_n24 * 9) + 4874 /* cst_193_0 */, 4096, Sc_degree(&_rescale_tmp_2533), Level(&_rescale_tmp_2533));
      Init_ciph_up_scale_plain(&_pgen_tmp_188, &_rescale_tmp_2533, &_pgen_tmp_187);
      Init_ciph_same_scale(&result_cin_n24, &result_cin_n24, &_pgen_tmp_188);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n24._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_188._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2533._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_187._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_188._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2533._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_187._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n24._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n24._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_188._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n24._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n24._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_188._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435620 = Rotate(result_cin_n24, -4096);
    Init_ciph_same_scale(&_pgen_tmp_189, &result_cin_n24, &_preg_268435620);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_189._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_189._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n24._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435620._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_189._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n24._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435620._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435621 = Rotate(_pgen_tmp_189, index_cin_n24 * 64);
    Init_ciph_same_scale(&result_n24, &result_n24, &_preg_268435621);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n24._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n24._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n24._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435621._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n24._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n24._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435621._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435620._c1_poly);
    Free_poly_data(&_preg_268435620._c0_poly);
    Free_poly_data(&_preg_268435621._c1_poly);
    Free_poly_data(&_preg_268435621._c0_poly);
  }
  Free_ciph_poly(input_roll_n24, 9);
  Free_poly_data(&_pgen_tmp_187._poly);
  Free_poly_data(&_pgen_tmp_188._c1_poly);
  Free_poly_data(&_pgen_tmp_188._c0_poly);
  Free_poly_data(&result_cin_n24._c1_poly);
  Free_poly_data(&result_cin_n24._c0_poly);
  Free_poly_data(&_pgen_tmp_189._c1_poly);
  Free_poly_data(&_pgen_tmp_189._c0_poly);
  Pt_from_msg(&_pgen_tmp_190, 5450 /* cst_194 */, 4096, Sc_degree(&result_n24), Level(&result_n24));
  Init_ciph_same_scale_plain(&result_n24, &result_n24, &_pgen_tmp_190);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n24._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n24._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n24._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_190._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n24._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n24._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_190._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_3165, &result_n24);
  Rescale(&_rescale_tmp_3165._c0_poly, &result_n24._c0_poly);
  Rescale(&_rescale_tmp_3165._c1_poly, &result_n24._c1_poly);
  Pt_from_msg(&_pgen_tmp_191, 5451 /* cst_196 */, 4096, Sc_degree(&_rescale_tmp_3165), Level(&_rescale_tmp_3165));
  Init_ciph_up_scale_plain(&result_n24, &_rescale_tmp_3165, &_pgen_tmp_191);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n24._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n24._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3165._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_191._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n24._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3165._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_191._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_191._poly);
    Free_poly_data(&_rescale_tmp_3165._c1_poly);
    Free_poly_data(&_rescale_tmp_3165._c0_poly);
  }
  Copy_ciph(&_preg_268435534, &result_n24);
  Free_poly_data(&result_n24._c1_poly);
  Free_poly_data(&result_n24._c0_poly);
  Tm_taken("Tensor::conv");
  Init_ciph_down_scale(&_pgen_tmp_192, &_preg_268435534);
  Rescale(&_pgen_tmp_192._c0_poly, &_preg_268435534._c0_poly);
  Rescale(&_pgen_tmp_192._c1_poly, &_preg_268435534._c1_poly);
  Free_poly_data(&_preg_268435534._c1_poly);
  Free_poly_data(&_preg_268435534._c0_poly);
  Bootstrap(&_preg_268435535, &_pgen_tmp_192, 15);
  Free_poly_data(&_pgen_tmp_192._c1_poly);
  Free_poly_data(&_pgen_tmp_192._c0_poly);
  Encode_plain_from_float(&_pgen_tmp_194, &_cst_258, 1, 1, Level(&_preg_268435535));
  Init_ciph_up_scale_plain(&_pgen_tmp_193, &_preg_268435535, &_pgen_tmp_194);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_193._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_193._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435535._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_194._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_193._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435535._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_194._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_194._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_195, &_pgen_tmp_193);
  Rescale(&_pgen_tmp_195._c0_poly, &_pgen_tmp_193._c0_poly);
  Rescale(&_pgen_tmp_195._c1_poly, &_pgen_tmp_193._c1_poly);
  Free_poly_data(&_pgen_tmp_193._c1_poly);
  Free_poly_data(&_pgen_tmp_193._c0_poly);
  _preg_268435536 = App_relu(_preg_268435535, _pgen_tmp_195);
  Free_poly_data(&_pgen_tmp_195._c1_poly);
  Free_poly_data(&_pgen_tmp_195._c0_poly);
  Free_poly_data(&_preg_268435535._c1_poly);
  Free_poly_data(&_preg_268435535._c0_poly);
  Copy_ciph(&_preg_268435537, &_preg_268435536);
  Free_poly_data(&_preg_268435536._c1_poly);
  Free_poly_data(&_preg_268435536._c0_poly);
  Tm_start("Tensor::conv");
  Zero_ciph(&result_n25);
  Copy_ciph(&input_dup_n25, &_preg_268435537);
  Free_poly_data(&_preg_268435537._c1_poly);
  Free_poly_data(&_preg_268435537._c0_poly);
  for (index_khw1_n25 = 0; index_khw1_n25 < 9; index_khw1_n25 = index_khw1_n25 + 1) {
    _preg_268435622 = Rotate(input_dup_n25, _cst_201[index_khw1_n25]);
    Copy_ciph(&input_roll_n25[index_khw1_n25], &_preg_268435622);
    Free_poly_data(&_preg_268435622._c1_poly);
    Free_poly_data(&_preg_268435622._c0_poly);
  }
  Free_poly_data(&input_dup_n25._c1_poly);
  Free_poly_data(&input_dup_n25._c0_poly);
  for (index_cin_n25 = 0; index_cin_n25 < 64; index_cin_n25 = index_cin_n25 + 1) {
    Zero_ciph(&result_cin_n25);
    for (index_khw2_n25 = 0; index_khw2_n25 < 9; index_khw2_n25 = index_khw2_n25 + 1) {
      _rescale_tmp_2641 = input_roll_n25[index_khw2_n25];
      Pt_from_msg(&_pgen_tmp_196, index_khw2_n25 + (index_cin_n25 * 9) + 5452 /* cst_199_0 */, 4096, Sc_degree(&_rescale_tmp_2641), Level(&_rescale_tmp_2641));
      Init_ciph_up_scale_plain(&_pgen_tmp_197, &_rescale_tmp_2641, &_pgen_tmp_196);
      Init_ciph_same_scale(&result_cin_n25, &result_cin_n25, &_pgen_tmp_197);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n25._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_197._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2641._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_196._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_197._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2641._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_196._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n25._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n25._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_197._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n25._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n25._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_197._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435623 = Rotate(result_cin_n25, -4096);
    Init_ciph_same_scale(&_pgen_tmp_198, &result_cin_n25, &_preg_268435623);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_198._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_198._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n25._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435623._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_198._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n25._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435623._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435624 = Rotate(_pgen_tmp_198, index_cin_n25 * 64);
    Init_ciph_same_scale(&result_n25, &result_n25, &_preg_268435624);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n25._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n25._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n25._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435624._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n25._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n25._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435624._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435623._c1_poly);
    Free_poly_data(&_preg_268435623._c0_poly);
    Free_poly_data(&_preg_268435624._c1_poly);
    Free_poly_data(&_preg_268435624._c0_poly);
  }
  Free_ciph_poly(input_roll_n25, 9);
  Free_poly_data(&_pgen_tmp_196._poly);
  Free_poly_data(&_pgen_tmp_197._c1_poly);
  Free_poly_data(&_pgen_tmp_197._c0_poly);
  Free_poly_data(&result_cin_n25._c1_poly);
  Free_poly_data(&result_cin_n25._c0_poly);
  Free_poly_data(&_pgen_tmp_198._c1_poly);
  Free_poly_data(&_pgen_tmp_198._c0_poly);
  Pt_from_msg(&_pgen_tmp_199, 6028 /* cst_200 */, 4096, Sc_degree(&result_n25), Level(&result_n25));
  Init_ciph_same_scale_plain(&result_n25, &result_n25, &_pgen_tmp_199);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n25._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n25._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n25._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_199._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n25._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n25._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_199._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_3178, &result_n25);
  Rescale(&_rescale_tmp_3178._c0_poly, &result_n25._c0_poly);
  Rescale(&_rescale_tmp_3178._c1_poly, &result_n25._c1_poly);
  Pt_from_msg(&_pgen_tmp_200, 6029 /* cst_202 */, 4096, Sc_degree(&_rescale_tmp_3178), Level(&_rescale_tmp_3178));
  Init_ciph_up_scale_plain(&result_n25, &_rescale_tmp_3178, &_pgen_tmp_200);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n25._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n25._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3178._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_200._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n25._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3178._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_200._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_200._poly);
    Free_poly_data(&_rescale_tmp_3178._c1_poly);
    Free_poly_data(&_rescale_tmp_3178._c0_poly);
  }
  Copy_ciph(&_preg_268435538, &result_n25);
  Free_poly_data(&result_n25._c1_poly);
  Free_poly_data(&result_n25._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::add");
  Init_ciph_down_scale(&_pgen_tmp_201, &_preg_268435538);
  Rescale(&_pgen_tmp_201._c0_poly, &_preg_268435538._c0_poly);
  Rescale(&_pgen_tmp_201._c1_poly, &_preg_268435538._c1_poly);
  Free_poly_data(&_preg_268435538._c1_poly);
  Free_poly_data(&_preg_268435538._c0_poly);
  Init_ciph_same_scale(&_preg_268435539, &_pgen_tmp_201, &_preg_268435533);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_preg_268435539._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_preg_268435539._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_201._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435533._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_preg_268435539._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_201._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435533._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435533._c1_poly);
    Free_poly_data(&_preg_268435533._c0_poly);
    Free_poly_data(&_pgen_tmp_201._c1_poly);
    Free_poly_data(&_pgen_tmp_201._c0_poly);
  }
  Tm_taken("Tensor::add");
  Bootstrap(&_preg_268435540, &_preg_268435539, 17);
  Free_poly_data(&_preg_268435539._c1_poly);
  Free_poly_data(&_preg_268435539._c0_poly);
  Encode_plain_from_float(&_pgen_tmp_203, &_cst_259, 1, 1, Level(&_preg_268435540));
  Init_ciph_up_scale_plain(&_pgen_tmp_202, &_preg_268435540, &_pgen_tmp_203);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_202._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_202._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435540._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_203._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_202._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435540._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_203._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_203._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_204, &_pgen_tmp_202);
  Rescale(&_pgen_tmp_204._c0_poly, &_pgen_tmp_202._c0_poly);
  Rescale(&_pgen_tmp_204._c1_poly, &_pgen_tmp_202._c1_poly);
  Free_poly_data(&_pgen_tmp_202._c1_poly);
  Free_poly_data(&_pgen_tmp_202._c0_poly);
  _preg_268435541 = App_relu(_preg_268435540, _pgen_tmp_204);
  Free_poly_data(&_pgen_tmp_204._c1_poly);
  Free_poly_data(&_pgen_tmp_204._c0_poly);
  Free_poly_data(&_preg_268435540._c1_poly);
  Free_poly_data(&_preg_268435540._c0_poly);
  Copy_ciph(&_preg_268435542, &_preg_268435541);
  Free_poly_data(&_preg_268435541._c1_poly);
  Free_poly_data(&_preg_268435541._c0_poly);
  Tm_start("Tensor::global_avg_pool");
  for (sum_index_n25 = 0; sum_index_n25 < 6; sum_index_n25 = sum_index_n25 + 1) {
    _preg_268435625 = Rotate(_preg_268435542, 1 << sum_index_n25);
    Init_ciph_same_scale(&_preg_268435542, &_preg_268435542, &_preg_268435625);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_preg_268435542._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_preg_268435542._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435542._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435625._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_preg_268435542._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435542._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435625._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435625._c1_poly);
    Free_poly_data(&_preg_268435625._c0_poly);
  }
  Copy_ciph(&_rescale_tmp_2726, &_preg_268435542);
  Free_poly_data(&_preg_268435542._c1_poly);
  Free_poly_data(&_preg_268435542._c0_poly);
  Pt_from_msg(&_pgen_tmp_205, 6030 /* cst_205 */, 4096, Sc_degree(&_rescale_tmp_2726), Level(&_rescale_tmp_2726));
  Init_ciph_up_scale_plain(&gap_result25, &_rescale_tmp_2726, &_pgen_tmp_205);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&gap_result25._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&gap_result25._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2726._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_205._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&gap_result25._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2726._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_205._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_205._poly);
    Free_poly_data(&_rescale_tmp_2726._c1_poly);
    Free_poly_data(&_rescale_tmp_2726._c0_poly);
  }
  for (combine_cc_index_n25 = 0; combine_cc_index_n25 < 6; combine_cc_index_n25 = combine_cc_index_n25 + 1) {
    _preg_268435626 = Rotate(gap_result25, (1 << combine_cc_index_n25) * 63);
    Init_ciph_same_scale(&gap_result25, &gap_result25, &_preg_268435626);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&gap_result25._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&gap_result25._c0_poly, _pgen_rns_idx, degree), Coeffs(&gap_result25._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435626._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&gap_result25._c1_poly, _pgen_rns_idx, degree), Coeffs(&gap_result25._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435626._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435626._c1_poly);
    Free_poly_data(&_preg_268435626._c0_poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_3191, &gap_result25);
  Rescale(&_rescale_tmp_3191._c0_poly, &gap_result25._c0_poly);
  Rescale(&_rescale_tmp_3191._c1_poly, &gap_result25._c1_poly);
  Pt_from_msg(&_pgen_tmp_206, 6031 /* cst_206 */, 64, Sc_degree(&_rescale_tmp_3191), Level(&_rescale_tmp_3191));
  Init_ciph_up_scale_plain(&gap_result25, &_rescale_tmp_3191, &_pgen_tmp_206);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&gap_result25._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&gap_result25._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3191._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_206._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&gap_result25._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3191._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_206._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_206._poly);
    Free_poly_data(&_rescale_tmp_3191._c1_poly);
    Free_poly_data(&_rescale_tmp_3191._c0_poly);
  }
  Copy_ciph(&_preg_268435543, &gap_result25);
  Free_poly_data(&gap_result25._c1_poly);
  Free_poly_data(&gap_result25._c0_poly);
  Tm_taken("Tensor::global_avg_pool");
  Tm_start("Tensor::flatten");
  Copy_ciph(&_preg_268435544, &_preg_268435543);
  Free_poly_data(&_preg_268435543._c1_poly);
  Free_poly_data(&_preg_268435543._c0_poly);
  Tm_taken("Tensor::flatten");
  Tm_start("Tensor::gemm");
  Zero_ciph(&tmp_result_n26);
  _preg_268435627 = Rotate(_preg_268435544, -70);
  Init_ciph_same_scale(&input_dup_n26, &_preg_268435544, &_preg_268435627);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&input_dup_n26._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&input_dup_n26._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435544._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435627._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&input_dup_n26._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435544._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435627._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435627._c1_poly);
    Free_poly_data(&_preg_268435627._c0_poly);
    Free_poly_data(&_preg_268435544._c1_poly);
    Free_poly_data(&_preg_268435544._c0_poly);
  }
  for (index_gemm_n26 = 0; index_gemm_n26 < 10; index_gemm_n26 = index_gemm_n26 + 1) {
    _preg_268435628 = Rotate(input_dup_n26, index_gemm_n26);
    Copy_ciph(&_ckks_gen_tmp_2791, &_preg_268435628);
    Init_ciph_down_scale(&_rescale_tmp_3197, &_ckks_gen_tmp_2791);
    Rescale(&_rescale_tmp_3197._c0_poly, &_ckks_gen_tmp_2791._c0_poly);
    Rescale(&_rescale_tmp_3197._c1_poly, &_ckks_gen_tmp_2791._c1_poly);
    Pt_from_msg(&_pgen_tmp_207, index_gemm_n26 + 6032 /* cst_209_0 */, 70, Sc_degree(&_rescale_tmp_3197), Level(&_rescale_tmp_3197));
    Init_ciph_up_scale_plain(&_pgen_tmp_208, &_rescale_tmp_3197, &_pgen_tmp_207);
    Init_ciph_same_scale(&tmp_result_n26, &tmp_result_n26, &_pgen_tmp_208);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&tmp_result_n26._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_208._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3197._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_207._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_208._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3197._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_207._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&tmp_result_n26._c0_poly, _pgen_rns_idx, degree), Coeffs(&tmp_result_n26._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_208._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&tmp_result_n26._c1_poly, _pgen_rns_idx, degree), Coeffs(&tmp_result_n26._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_208._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435628._c1_poly);
    Free_poly_data(&_preg_268435628._c0_poly);
  }
  Free_poly_data(&input_dup_n26._c1_poly);
  Free_poly_data(&input_dup_n26._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_2791._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_2791._c0_poly);
  Free_poly_data(&_pgen_tmp_207._poly);
  Free_poly_data(&_rescale_tmp_3197._c1_poly);
  Free_poly_data(&_rescale_tmp_3197._c0_poly);
  Free_poly_data(&_pgen_tmp_208._c1_poly);
  Free_poly_data(&_pgen_tmp_208._c0_poly);
  for (index_add_n26 = 0; index_add_n26 < 3; index_add_n26 = index_add_n26 + 1) {
    _preg_268435629 = Rotate(tmp_result_n26, (1 << index_add_n26) * 10);
    Init_ciph_same_scale(&tmp_result_n26, &tmp_result_n26, &_preg_268435629);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&tmp_result_n26._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&tmp_result_n26._c0_poly, _pgen_rns_idx, degree), Coeffs(&tmp_result_n26._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435629._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&tmp_result_n26._c1_poly, _pgen_rns_idx, degree), Coeffs(&tmp_result_n26._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435629._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435629._c1_poly);
    Free_poly_data(&_preg_268435629._c0_poly);
  }
  Pt_from_msg(&_pgen_tmp_209, 6042 /* cst_1 */, 10, Sc_degree(&tmp_result_n26), Level(&tmp_result_n26));
  Init_ciph_same_scale_plain(&tmp_result_n26, &tmp_result_n26, &_pgen_tmp_209);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&tmp_result_n26._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&tmp_result_n26._c0_poly, _pgen_rns_idx, degree), Coeffs(&tmp_result_n26._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_209._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&tmp_result_n26._c1_poly, _pgen_rns_idx, degree, Coeffs(&tmp_result_n26._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_209._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_3203, &tmp_result_n26);
  Rescale(&_rescale_tmp_3203._c0_poly, &tmp_result_n26._c0_poly);
  Rescale(&_rescale_tmp_3203._c1_poly, &tmp_result_n26._c1_poly);
  Pt_from_msg(&_pgen_tmp_210, 6043 /* cst_210 */, 10, Sc_degree(&_rescale_tmp_3203), Level(&_rescale_tmp_3203));
  Init_ciph_up_scale_plain(&tmp_result_n26, &_rescale_tmp_3203, &_pgen_tmp_210);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&tmp_result_n26._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&tmp_result_n26._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3203._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_210._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&tmp_result_n26._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3203._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_210._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_210._poly);
    Free_poly_data(&_rescale_tmp_3203._c1_poly);
    Free_poly_data(&_rescale_tmp_3203._c0_poly);
  }
  Copy_ciph(&output, &tmp_result_n26);
  Free_poly_data(&tmp_result_n26._c1_poly);
  Free_poly_data(&tmp_result_n26._c0_poly);
  Tm_taken("Tensor::gemm");
  Set_output_data("output", 0, &output);
  return true;
}

int Get_input_count() {
  return 1;
}

DATA_SCHEME* Get_encode_scheme(int idx) {
  static DATA_SCHEME scheme_0 = {
    "input", {0, 0, 0, 0}, 1, {NORMAL, 0, 0, 0, 0}
  };
  static DATA_SCHEME* scheme[] = { &scheme_0 };
  return scheme[idx];
}

int Get_output_count() {
  return 1;
}

DATA_SCHEME* Get_decode_scheme(int idx) {
  static DATA_SCHEME scheme = {
    "output", {0, 0, 0, 0}, 1, {NORMAL, 0, 0, 0, 0}
  };
  return &scheme;
}

CIPHERTEXT App_relu(CIPHERTEXT input_ct0, CIPHERTEXT input_ct1) {
  CIPHERTEXT _relu_tmp7;
  CIPHERTEXT _relu_tmp13;
  CIPHERTEXT _relu_tmp17;
  CIPHERTEXT _relu_tmp23;
  CIPHERTEXT _relu_tmp45;
  CIPHERTEXT _relu_tmp49;
  CIPHERTEXT _relu_tmp55;
  CIPHERTEXT _relu_tmp59;
  CIPHERTEXT _relu_tmp67;
  CIPHERTEXT _relu_tmp71;
  CIPHERTEXT _relu_tmp77;
  CIPHERTEXT _relu_tmp81;
  CIPHERTEXT _relu_tmp87;
  CIPHERTEXT _relu_tmp127;
  CIPHERTEXT _relu_tmp131;
  CIPHERTEXT _relu_tmp137;
  CIPHERTEXT _relu_tmp141;
  CIPHERTEXT _relu_tmp149;
  CIPHERTEXT _relu_tmp153;
  CIPHERTEXT _relu_tmp159;
  CIPHERTEXT _relu_tmp163;
  CIPHERTEXT _relu_tmp169;
  CIPHERTEXT _relu_tmp217;
  CIPHERTEXT _relu_tmp223;
  CIPHERTEXT _rescale_tmp_253;
  CIPHERTEXT3 _pgen_tmp_0;
  MODULUS* _pgen_modulus;
  uint32_t _pgen_num_q;
  uint32_t _pgen_rns_idx;
  POLY _pgen_tmp_1;
  POLY _pgen_tmp_2;
  CIPHERTEXT _pgen_tmp_3;
  PLAINTEXT _pgen_tmp_4;
  CIPHERTEXT3 _pgen_tmp_5;
  CIPHERTEXT _pgen_tmp_6;
  CIPHERTEXT _pgen_tmp_7;
  POLY _pgen_tmp_8;
  POLY _pgen_tmp_9;
  CIPHERTEXT _pgen_tmp_10;
  PLAINTEXT _pgen_tmp_11;
  CIPHERTEXT3 _pgen_tmp_12;
  CIPHERTEXT _pgen_tmp_13;
  CIPHERTEXT _pgen_tmp_14;
  CIPHERTEXT3 _pgen_tmp_15;
  CIPHERTEXT _pgen_tmp_16;
  CIPHERTEXT _pgen_tmp_17;
  PLAINTEXT _pgen_tmp_18;
  CIPHERTEXT _pgen_tmp_19;
  POLY _pgen_tmp_20;
  POLY _pgen_tmp_21;
  PLAINTEXT _pgen_tmp_22;
  CIPHERTEXT _pgen_tmp_23;
  CIPHERTEXT _pgen_tmp_24;
  POLY _pgen_tmp_25;
  POLY _pgen_tmp_26;
  CIPHERTEXT3 _pgen_tmp_27;
  CIPHERTEXT _pgen_tmp_28;
  CIPHERTEXT _pgen_tmp_29;
  PLAINTEXT _pgen_tmp_30;
  CIPHERTEXT _pgen_tmp_31;
  POLY _pgen_tmp_32;
  POLY _pgen_tmp_33;
  PLAINTEXT _pgen_tmp_34;
  CIPHERTEXT _pgen_tmp_35;
  CIPHERTEXT _pgen_tmp_36;
  CIPHERTEXT3 _pgen_tmp_37;
  CIPHERTEXT _pgen_tmp_38;
  CIPHERTEXT _pgen_tmp_39;
  POLY _pgen_tmp_40;
  POLY _pgen_tmp_41;
  CIPHERTEXT _pgen_tmp_42;
  PLAINTEXT _pgen_tmp_43;
  CIPHERTEXT3 _pgen_tmp_44;
  CIPHERTEXT _pgen_tmp_45;
  CIPHERTEXT _pgen_tmp_46;
  POLY _pgen_tmp_47;
  POLY _pgen_tmp_48;
  CIPHERTEXT _pgen_tmp_49;
  CIPHERTEXT _pgen_tmp_50;
  PLAINTEXT _pgen_tmp_51;
  CIPHERTEXT _pgen_tmp_52;
  CIPHERTEXT3 _pgen_tmp_53;
  CIPHERTEXT _pgen_tmp_54;
  CIPHERTEXT _pgen_tmp_55;
  POLY _pgen_tmp_56;
  POLY _pgen_tmp_57;
  CIPHERTEXT _pgen_tmp_58;
  PLAINTEXT _pgen_tmp_59;
  CIPHERTEXT3 _pgen_tmp_60;
  CIPHERTEXT _pgen_tmp_61;
  CIPHERTEXT _pgen_tmp_62;
  POLY _pgen_tmp_63;
  POLY _pgen_tmp_64;
  CIPHERTEXT _pgen_tmp_65;
  PLAINTEXT _pgen_tmp_66;
  CIPHERTEXT3 _pgen_tmp_67;
  CIPHERTEXT _pgen_tmp_68;
  CIPHERTEXT _pgen_tmp_69;
  CIPHERTEXT3 _pgen_tmp_70;
  CIPHERTEXT _pgen_tmp_71;
  CIPHERTEXT _pgen_tmp_72;
  CIPHERTEXT3 _pgen_tmp_73;
  CIPHERTEXT _pgen_tmp_74;
  CIPHERTEXT _pgen_tmp_75;
  CIPHERTEXT _pgen_tmp_76;
  PLAINTEXT _pgen_tmp_77;
  CIPHERTEXT _pgen_tmp_78;
  POLY _pgen_tmp_79;
  POLY _pgen_tmp_80;
  CIPHERTEXT _pgen_tmp_81;
  PLAINTEXT _pgen_tmp_82;
  CIPHERTEXT _pgen_tmp_83;
  CIPHERTEXT _pgen_tmp_84;
  POLY _pgen_tmp_85;
  POLY _pgen_tmp_86;
  CIPHERTEXT _pgen_tmp_87;
  PLAINTEXT _pgen_tmp_88;
  CIPHERTEXT _pgen_tmp_89;
  CIPHERTEXT _pgen_tmp_90;
  PLAINTEXT _pgen_tmp_91;
  CIPHERTEXT _pgen_tmp_92;
  CIPHERTEXT _pgen_tmp_93;
  CIPHERTEXT _pgen_tmp_94;
  POLY _pgen_tmp_95;
  POLY _pgen_tmp_96;
  CIPHERTEXT3 _pgen_tmp_97;
  CIPHERTEXT _pgen_tmp_98;
  CIPHERTEXT _pgen_tmp_99;
  CIPHERTEXT _pgen_tmp_100;
  PLAINTEXT _pgen_tmp_101;
  CIPHERTEXT _pgen_tmp_102;
  CIPHERTEXT _pgen_tmp_103;
  PLAINTEXT _pgen_tmp_104;
  CIPHERTEXT _pgen_tmp_105;
  CIPHERTEXT _pgen_tmp_106;
  POLY _pgen_tmp_107;
  POLY _pgen_tmp_108;
  CIPHERTEXT _pgen_tmp_109;
  PLAINTEXT _pgen_tmp_110;
  CIPHERTEXT _pgen_tmp_111;
  CIPHERTEXT _pgen_tmp_112;
  PLAINTEXT _pgen_tmp_113;
  CIPHERTEXT _pgen_tmp_114;
  CIPHERTEXT _pgen_tmp_115;
  CIPHERTEXT _pgen_tmp_116;
  CIPHERTEXT3 _pgen_tmp_117;
  CIPHERTEXT _pgen_tmp_118;
  CIPHERTEXT _pgen_tmp_119;
  POLY _pgen_tmp_120;
  POLY _pgen_tmp_121;
  CIPHERTEXT _pgen_tmp_122;
  PLAINTEXT _pgen_tmp_123;
  CIPHERTEXT3 _pgen_tmp_124;
  CIPHERTEXT _pgen_tmp_125;
  CIPHERTEXT _pgen_tmp_126;
  POLY _pgen_tmp_127;
  POLY _pgen_tmp_128;
  CIPHERTEXT _pgen_tmp_129;
  CIPHERTEXT _pgen_tmp_130;
  PLAINTEXT _pgen_tmp_131;
  CIPHERTEXT _pgen_tmp_132;
  CIPHERTEXT3 _pgen_tmp_133;
  CIPHERTEXT _pgen_tmp_134;
  CIPHERTEXT _pgen_tmp_135;
  POLY _pgen_tmp_136;
  POLY _pgen_tmp_137;
  CIPHERTEXT _pgen_tmp_138;
  PLAINTEXT _pgen_tmp_139;
  CIPHERTEXT3 _pgen_tmp_140;
  CIPHERTEXT _pgen_tmp_141;
  CIPHERTEXT _pgen_tmp_142;
  POLY _pgen_tmp_143;
  POLY _pgen_tmp_144;
  CIPHERTEXT _pgen_tmp_145;
  PLAINTEXT _pgen_tmp_146;
  CIPHERTEXT3 _pgen_tmp_147;
  CIPHERTEXT _pgen_tmp_148;
  CIPHERTEXT _pgen_tmp_149;
  CIPHERTEXT3 _pgen_tmp_150;
  CIPHERTEXT _pgen_tmp_151;
  CIPHERTEXT3 _pgen_tmp_152;
  CIPHERTEXT _pgen_tmp_153;
  CIPHERTEXT _pgen_tmp_154;
  PLAINTEXT _pgen_tmp_155;
  CIPHERTEXT _pgen_tmp_156;
  POLY _pgen_tmp_157;
  POLY _pgen_tmp_158;
  CIPHERTEXT _pgen_tmp_159;
  POLY _pgen_tmp_160;
  POLY _pgen_tmp_161;
  CIPHERTEXT3 _pgen_tmp_162;
  CIPHERTEXT _pgen_tmp_163;
  CIPHERTEXT _pgen_tmp_164;
  PLAINTEXT _pgen_tmp_165;
  CIPHERTEXT _pgen_tmp_166;
  POLY _pgen_tmp_167;
  POLY _pgen_tmp_168;
  CIPHERTEXT3 _pgen_tmp_169;
  CIPHERTEXT _pgen_tmp_170;
  CIPHERTEXT _pgen_tmp_171;
  PLAINTEXT _pgen_tmp_172;
  CIPHERTEXT _pgen_tmp_173;
  POLY _pgen_tmp_174;
  POLY _pgen_tmp_175;
  CIPHERTEXT _pgen_tmp_176;
  CIPHERTEXT _pgen_tmp_177;
  POLY _pgen_tmp_178;
  POLY _pgen_tmp_179;
  CIPHERTEXT3 _pgen_tmp_180;
  CIPHERTEXT _pgen_tmp_181;
  CIPHERTEXT _pgen_tmp_182;
  CIPHERTEXT3 _pgen_tmp_183;
  CIPHERTEXT _pgen_tmp_184;
  CIPHERTEXT _pgen_tmp_185;
  PLAINTEXT _pgen_tmp_186;
  CIPHERTEXT _pgen_tmp_187;
  POLY _pgen_tmp_188;
  POLY _pgen_tmp_189;
  CIPHERTEXT3 _pgen_tmp_190;
  CIPHERTEXT _pgen_tmp_191;
  CIPHERTEXT _pgen_tmp_192;
  PLAINTEXT _pgen_tmp_193;
  CIPHERTEXT _pgen_tmp_194;
  POLY _pgen_tmp_195;
  POLY _pgen_tmp_196;
  CIPHERTEXT _pgen_tmp_197;
  POLY _pgen_tmp_198;
  POLY _pgen_tmp_199;
  CIPHERTEXT3 _pgen_tmp_200;
  CIPHERTEXT _pgen_tmp_201;
  CIPHERTEXT _pgen_tmp_202;
  PLAINTEXT _pgen_tmp_203;
  CIPHERTEXT _pgen_tmp_204;
  POLY _pgen_tmp_205;
  POLY _pgen_tmp_206;
  CIPHERTEXT3 _pgen_tmp_207;
  CIPHERTEXT _pgen_tmp_208;
  CIPHERTEXT _pgen_tmp_209;
  PLAINTEXT _pgen_tmp_210;
  CIPHERTEXT _pgen_tmp_211;
  POLY _pgen_tmp_212;
  POLY _pgen_tmp_213;
  CIPHERTEXT _pgen_tmp_214;
  CIPHERTEXT _pgen_tmp_215;
  PLAINTEXT _pgen_tmp_216;
  CIPHERTEXT _pgen_tmp_217;
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
  memset(&_relu_tmp7, 0, sizeof(_relu_tmp7));
  memset(&_relu_tmp13, 0, sizeof(_relu_tmp13));
  memset(&_relu_tmp17, 0, sizeof(_relu_tmp17));
  memset(&_relu_tmp23, 0, sizeof(_relu_tmp23));
  memset(&_relu_tmp45, 0, sizeof(_relu_tmp45));
  memset(&_relu_tmp49, 0, sizeof(_relu_tmp49));
  memset(&_relu_tmp55, 0, sizeof(_relu_tmp55));
  memset(&_relu_tmp59, 0, sizeof(_relu_tmp59));
  memset(&_relu_tmp67, 0, sizeof(_relu_tmp67));
  memset(&_relu_tmp71, 0, sizeof(_relu_tmp71));
  memset(&_relu_tmp77, 0, sizeof(_relu_tmp77));
  memset(&_relu_tmp81, 0, sizeof(_relu_tmp81));
  memset(&_relu_tmp87, 0, sizeof(_relu_tmp87));
  memset(&_relu_tmp127, 0, sizeof(_relu_tmp127));
  memset(&_relu_tmp131, 0, sizeof(_relu_tmp131));
  memset(&_relu_tmp137, 0, sizeof(_relu_tmp137));
  memset(&_relu_tmp141, 0, sizeof(_relu_tmp141));
  memset(&_relu_tmp149, 0, sizeof(_relu_tmp149));
  memset(&_relu_tmp153, 0, sizeof(_relu_tmp153));
  memset(&_relu_tmp159, 0, sizeof(_relu_tmp159));
  memset(&_relu_tmp163, 0, sizeof(_relu_tmp163));
  memset(&_relu_tmp169, 0, sizeof(_relu_tmp169));
  memset(&_relu_tmp217, 0, sizeof(_relu_tmp217));
  memset(&_relu_tmp223, 0, sizeof(_relu_tmp223));
  memset(&_rescale_tmp_253, 0, sizeof(_rescale_tmp_253));
  memset(&_pgen_tmp_0, 0, sizeof(_pgen_tmp_0));
  memset(&_pgen_tmp_3, 0, sizeof(_pgen_tmp_3));
  memset(&_pgen_tmp_4, 0, sizeof(_pgen_tmp_4));
  memset(&_pgen_tmp_5, 0, sizeof(_pgen_tmp_5));
  memset(&_pgen_tmp_6, 0, sizeof(_pgen_tmp_6));
  memset(&_pgen_tmp_7, 0, sizeof(_pgen_tmp_7));
  memset(&_pgen_tmp_10, 0, sizeof(_pgen_tmp_10));
  memset(&_pgen_tmp_11, 0, sizeof(_pgen_tmp_11));
  memset(&_pgen_tmp_12, 0, sizeof(_pgen_tmp_12));
  memset(&_pgen_tmp_13, 0, sizeof(_pgen_tmp_13));
  memset(&_pgen_tmp_14, 0, sizeof(_pgen_tmp_14));
  memset(&_pgen_tmp_15, 0, sizeof(_pgen_tmp_15));
  memset(&_pgen_tmp_16, 0, sizeof(_pgen_tmp_16));
  memset(&_pgen_tmp_17, 0, sizeof(_pgen_tmp_17));
  memset(&_pgen_tmp_18, 0, sizeof(_pgen_tmp_18));
  memset(&_pgen_tmp_19, 0, sizeof(_pgen_tmp_19));
  memset(&_pgen_tmp_22, 0, sizeof(_pgen_tmp_22));
  memset(&_pgen_tmp_23, 0, sizeof(_pgen_tmp_23));
  memset(&_pgen_tmp_24, 0, sizeof(_pgen_tmp_24));
  memset(&_pgen_tmp_27, 0, sizeof(_pgen_tmp_27));
  memset(&_pgen_tmp_28, 0, sizeof(_pgen_tmp_28));
  memset(&_pgen_tmp_29, 0, sizeof(_pgen_tmp_29));
  memset(&_pgen_tmp_30, 0, sizeof(_pgen_tmp_30));
  memset(&_pgen_tmp_31, 0, sizeof(_pgen_tmp_31));
  memset(&_pgen_tmp_34, 0, sizeof(_pgen_tmp_34));
  memset(&_pgen_tmp_35, 0, sizeof(_pgen_tmp_35));
  memset(&_pgen_tmp_36, 0, sizeof(_pgen_tmp_36));
  memset(&_pgen_tmp_37, 0, sizeof(_pgen_tmp_37));
  memset(&_pgen_tmp_38, 0, sizeof(_pgen_tmp_38));
  memset(&_pgen_tmp_39, 0, sizeof(_pgen_tmp_39));
  memset(&_pgen_tmp_42, 0, sizeof(_pgen_tmp_42));
  memset(&_pgen_tmp_43, 0, sizeof(_pgen_tmp_43));
  memset(&_pgen_tmp_44, 0, sizeof(_pgen_tmp_44));
  memset(&_pgen_tmp_45, 0, sizeof(_pgen_tmp_45));
  memset(&_pgen_tmp_46, 0, sizeof(_pgen_tmp_46));
  memset(&_pgen_tmp_49, 0, sizeof(_pgen_tmp_49));
  memset(&_pgen_tmp_50, 0, sizeof(_pgen_tmp_50));
  memset(&_pgen_tmp_51, 0, sizeof(_pgen_tmp_51));
  memset(&_pgen_tmp_52, 0, sizeof(_pgen_tmp_52));
  memset(&_pgen_tmp_53, 0, sizeof(_pgen_tmp_53));
  memset(&_pgen_tmp_54, 0, sizeof(_pgen_tmp_54));
  memset(&_pgen_tmp_55, 0, sizeof(_pgen_tmp_55));
  memset(&_pgen_tmp_58, 0, sizeof(_pgen_tmp_58));
  memset(&_pgen_tmp_59, 0, sizeof(_pgen_tmp_59));
  memset(&_pgen_tmp_60, 0, sizeof(_pgen_tmp_60));
  memset(&_pgen_tmp_61, 0, sizeof(_pgen_tmp_61));
  memset(&_pgen_tmp_62, 0, sizeof(_pgen_tmp_62));
  memset(&_pgen_tmp_65, 0, sizeof(_pgen_tmp_65));
  memset(&_pgen_tmp_66, 0, sizeof(_pgen_tmp_66));
  memset(&_pgen_tmp_67, 0, sizeof(_pgen_tmp_67));
  memset(&_pgen_tmp_68, 0, sizeof(_pgen_tmp_68));
  memset(&_pgen_tmp_69, 0, sizeof(_pgen_tmp_69));
  memset(&_pgen_tmp_70, 0, sizeof(_pgen_tmp_70));
  memset(&_pgen_tmp_71, 0, sizeof(_pgen_tmp_71));
  memset(&_pgen_tmp_72, 0, sizeof(_pgen_tmp_72));
  memset(&_pgen_tmp_73, 0, sizeof(_pgen_tmp_73));
  memset(&_pgen_tmp_74, 0, sizeof(_pgen_tmp_74));
  memset(&_pgen_tmp_75, 0, sizeof(_pgen_tmp_75));
  memset(&_pgen_tmp_76, 0, sizeof(_pgen_tmp_76));
  memset(&_pgen_tmp_77, 0, sizeof(_pgen_tmp_77));
  memset(&_pgen_tmp_78, 0, sizeof(_pgen_tmp_78));
  memset(&_pgen_tmp_81, 0, sizeof(_pgen_tmp_81));
  memset(&_pgen_tmp_82, 0, sizeof(_pgen_tmp_82));
  memset(&_pgen_tmp_83, 0, sizeof(_pgen_tmp_83));
  memset(&_pgen_tmp_84, 0, sizeof(_pgen_tmp_84));
  memset(&_pgen_tmp_87, 0, sizeof(_pgen_tmp_87));
  memset(&_pgen_tmp_88, 0, sizeof(_pgen_tmp_88));
  memset(&_pgen_tmp_89, 0, sizeof(_pgen_tmp_89));
  memset(&_pgen_tmp_90, 0, sizeof(_pgen_tmp_90));
  memset(&_pgen_tmp_91, 0, sizeof(_pgen_tmp_91));
  memset(&_pgen_tmp_92, 0, sizeof(_pgen_tmp_92));
  memset(&_pgen_tmp_93, 0, sizeof(_pgen_tmp_93));
  memset(&_pgen_tmp_94, 0, sizeof(_pgen_tmp_94));
  memset(&_pgen_tmp_97, 0, sizeof(_pgen_tmp_97));
  memset(&_pgen_tmp_98, 0, sizeof(_pgen_tmp_98));
  memset(&_pgen_tmp_99, 0, sizeof(_pgen_tmp_99));
  memset(&_pgen_tmp_100, 0, sizeof(_pgen_tmp_100));
  memset(&_pgen_tmp_101, 0, sizeof(_pgen_tmp_101));
  memset(&_pgen_tmp_102, 0, sizeof(_pgen_tmp_102));
  memset(&_pgen_tmp_103, 0, sizeof(_pgen_tmp_103));
  memset(&_pgen_tmp_104, 0, sizeof(_pgen_tmp_104));
  memset(&_pgen_tmp_105, 0, sizeof(_pgen_tmp_105));
  memset(&_pgen_tmp_106, 0, sizeof(_pgen_tmp_106));
  memset(&_pgen_tmp_109, 0, sizeof(_pgen_tmp_109));
  memset(&_pgen_tmp_110, 0, sizeof(_pgen_tmp_110));
  memset(&_pgen_tmp_111, 0, sizeof(_pgen_tmp_111));
  memset(&_pgen_tmp_112, 0, sizeof(_pgen_tmp_112));
  memset(&_pgen_tmp_113, 0, sizeof(_pgen_tmp_113));
  memset(&_pgen_tmp_114, 0, sizeof(_pgen_tmp_114));
  memset(&_pgen_tmp_115, 0, sizeof(_pgen_tmp_115));
  memset(&_pgen_tmp_116, 0, sizeof(_pgen_tmp_116));
  memset(&_pgen_tmp_117, 0, sizeof(_pgen_tmp_117));
  memset(&_pgen_tmp_118, 0, sizeof(_pgen_tmp_118));
  memset(&_pgen_tmp_119, 0, sizeof(_pgen_tmp_119));
  memset(&_pgen_tmp_122, 0, sizeof(_pgen_tmp_122));
  memset(&_pgen_tmp_123, 0, sizeof(_pgen_tmp_123));
  memset(&_pgen_tmp_124, 0, sizeof(_pgen_tmp_124));
  memset(&_pgen_tmp_125, 0, sizeof(_pgen_tmp_125));
  memset(&_pgen_tmp_126, 0, sizeof(_pgen_tmp_126));
  memset(&_pgen_tmp_129, 0, sizeof(_pgen_tmp_129));
  memset(&_pgen_tmp_130, 0, sizeof(_pgen_tmp_130));
  memset(&_pgen_tmp_131, 0, sizeof(_pgen_tmp_131));
  memset(&_pgen_tmp_132, 0, sizeof(_pgen_tmp_132));
  memset(&_pgen_tmp_133, 0, sizeof(_pgen_tmp_133));
  memset(&_pgen_tmp_134, 0, sizeof(_pgen_tmp_134));
  memset(&_pgen_tmp_135, 0, sizeof(_pgen_tmp_135));
  memset(&_pgen_tmp_138, 0, sizeof(_pgen_tmp_138));
  memset(&_pgen_tmp_139, 0, sizeof(_pgen_tmp_139));
  memset(&_pgen_tmp_140, 0, sizeof(_pgen_tmp_140));
  memset(&_pgen_tmp_141, 0, sizeof(_pgen_tmp_141));
  memset(&_pgen_tmp_142, 0, sizeof(_pgen_tmp_142));
  memset(&_pgen_tmp_145, 0, sizeof(_pgen_tmp_145));
  memset(&_pgen_tmp_146, 0, sizeof(_pgen_tmp_146));
  memset(&_pgen_tmp_147, 0, sizeof(_pgen_tmp_147));
  memset(&_pgen_tmp_148, 0, sizeof(_pgen_tmp_148));
  memset(&_pgen_tmp_149, 0, sizeof(_pgen_tmp_149));
  memset(&_pgen_tmp_150, 0, sizeof(_pgen_tmp_150));
  memset(&_pgen_tmp_151, 0, sizeof(_pgen_tmp_151));
  memset(&_pgen_tmp_152, 0, sizeof(_pgen_tmp_152));
  memset(&_pgen_tmp_153, 0, sizeof(_pgen_tmp_153));
  memset(&_pgen_tmp_154, 0, sizeof(_pgen_tmp_154));
  memset(&_pgen_tmp_155, 0, sizeof(_pgen_tmp_155));
  memset(&_pgen_tmp_156, 0, sizeof(_pgen_tmp_156));
  memset(&_pgen_tmp_159, 0, sizeof(_pgen_tmp_159));
  memset(&_pgen_tmp_162, 0, sizeof(_pgen_tmp_162));
  memset(&_pgen_tmp_163, 0, sizeof(_pgen_tmp_163));
  memset(&_pgen_tmp_164, 0, sizeof(_pgen_tmp_164));
  memset(&_pgen_tmp_165, 0, sizeof(_pgen_tmp_165));
  memset(&_pgen_tmp_166, 0, sizeof(_pgen_tmp_166));
  memset(&_pgen_tmp_169, 0, sizeof(_pgen_tmp_169));
  memset(&_pgen_tmp_170, 0, sizeof(_pgen_tmp_170));
  memset(&_pgen_tmp_171, 0, sizeof(_pgen_tmp_171));
  memset(&_pgen_tmp_172, 0, sizeof(_pgen_tmp_172));
  memset(&_pgen_tmp_173, 0, sizeof(_pgen_tmp_173));
  memset(&_pgen_tmp_176, 0, sizeof(_pgen_tmp_176));
  memset(&_pgen_tmp_177, 0, sizeof(_pgen_tmp_177));
  memset(&_pgen_tmp_180, 0, sizeof(_pgen_tmp_180));
  memset(&_pgen_tmp_181, 0, sizeof(_pgen_tmp_181));
  memset(&_pgen_tmp_182, 0, sizeof(_pgen_tmp_182));
  memset(&_pgen_tmp_183, 0, sizeof(_pgen_tmp_183));
  memset(&_pgen_tmp_184, 0, sizeof(_pgen_tmp_184));
  memset(&_pgen_tmp_185, 0, sizeof(_pgen_tmp_185));
  memset(&_pgen_tmp_186, 0, sizeof(_pgen_tmp_186));
  memset(&_pgen_tmp_187, 0, sizeof(_pgen_tmp_187));
  memset(&_pgen_tmp_190, 0, sizeof(_pgen_tmp_190));
  memset(&_pgen_tmp_191, 0, sizeof(_pgen_tmp_191));
  memset(&_pgen_tmp_192, 0, sizeof(_pgen_tmp_192));
  memset(&_pgen_tmp_193, 0, sizeof(_pgen_tmp_193));
  memset(&_pgen_tmp_194, 0, sizeof(_pgen_tmp_194));
  memset(&_pgen_tmp_197, 0, sizeof(_pgen_tmp_197));
  memset(&_pgen_tmp_200, 0, sizeof(_pgen_tmp_200));
  memset(&_pgen_tmp_201, 0, sizeof(_pgen_tmp_201));
  memset(&_pgen_tmp_202, 0, sizeof(_pgen_tmp_202));
  memset(&_pgen_tmp_203, 0, sizeof(_pgen_tmp_203));
  memset(&_pgen_tmp_204, 0, sizeof(_pgen_tmp_204));
  memset(&_pgen_tmp_207, 0, sizeof(_pgen_tmp_207));
  memset(&_pgen_tmp_208, 0, sizeof(_pgen_tmp_208));
  memset(&_pgen_tmp_209, 0, sizeof(_pgen_tmp_209));
  memset(&_pgen_tmp_210, 0, sizeof(_pgen_tmp_210));
  memset(&_pgen_tmp_211, 0, sizeof(_pgen_tmp_211));
  memset(&_pgen_tmp_214, 0, sizeof(_pgen_tmp_214));
  memset(&_pgen_tmp_215, 0, sizeof(_pgen_tmp_215));
  memset(&_pgen_tmp_216, 0, sizeof(_pgen_tmp_216));
  memset(&_pgen_tmp_217, 0, sizeof(_pgen_tmp_217));
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
  Init_ciph3_up_scale(&_pgen_tmp_0, &input_ct1, &input_ct1);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_0._c0_poly);
    _pgen_tmp_1 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_2 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_1, 0, degree), Coeffs(&input_ct1._c0_poly, _pgen_rns_idx, degree), Coeffs(&input_ct1._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_2, 0, degree), Coeffs(&input_ct1._c1_poly, _pgen_rns_idx, degree), Coeffs(&input_ct1._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_0._c0_poly, _pgen_rns_idx, degree), Coeffs(&input_ct1._c0_poly, _pgen_rns_idx, degree), Coeffs(&input_ct1._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_0._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_1, 0, degree), Coeffs(_pgen_tmp_2, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_0._c2_poly, _pgen_rns_idx, degree), Coeffs(&input_ct1._c1_poly, _pgen_rns_idx, degree), Coeffs(&input_ct1._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly(_pgen_tmp_1);
    Free_poly(_pgen_tmp_2);
  }
  _preg_268435456 = Relinearize(_pgen_tmp_0);
  Free_poly_data(&_pgen_tmp_0._c2_poly);
  Free_poly_data(&_pgen_tmp_0._c1_poly);
  Free_poly_data(&_pgen_tmp_0._c0_poly);
  Copy_ciph(&_relu_tmp7, &_preg_268435456);
  Free_poly_data(&_preg_268435456._c1_poly);
  Free_poly_data(&_preg_268435456._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_3, &_relu_tmp7, &_relu_tmp7);
  Encode_plain_from_float(&_pgen_tmp_4, &_cst_211, 1, Sc_degree(&_pgen_tmp_3), Level(&_pgen_tmp_3));
  Init_ciph_same_scale_plain(&_relu_tmp13, &_pgen_tmp_3, &_pgen_tmp_4);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_relu_tmp13._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_3._c0_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp7._c0_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp7._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_3._c1_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp7._c1_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp7._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_relu_tmp13._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_3._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_4._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&_relu_tmp13._c1_poly, _pgen_rns_idx, degree, Coeffs(&_pgen_tmp_3._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_relu_tmp7._c1_poly);
    Free_poly_data(&_relu_tmp7._c0_poly);
    Free_poly_data(&_pgen_tmp_4._poly);
    Free_poly_data(&_pgen_tmp_3._c1_poly);
    Free_poly_data(&_pgen_tmp_3._c0_poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_6, &_relu_tmp13);
  Rescale(&_pgen_tmp_6._c0_poly, &_relu_tmp13._c0_poly);
  Rescale(&_pgen_tmp_6._c1_poly, &_relu_tmp13._c1_poly);
  Init_ciph_down_scale(&_pgen_tmp_7, &_relu_tmp13);
  Rescale(&_pgen_tmp_7._c0_poly, &_relu_tmp13._c0_poly);
  Rescale(&_pgen_tmp_7._c1_poly, &_relu_tmp13._c1_poly);
  Init_ciph3_up_scale(&_pgen_tmp_5, &_pgen_tmp_6, &_pgen_tmp_7);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_5._c0_poly);
    _pgen_tmp_8 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_9 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_8, 0, degree), Coeffs(&_pgen_tmp_6._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_7._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_9, 0, degree), Coeffs(&_pgen_tmp_6._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_7._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_5._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_6._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_7._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_5._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_8, 0, degree), Coeffs(_pgen_tmp_9, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_5._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_6._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_7._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_7._c1_poly);
    Free_poly_data(&_pgen_tmp_7._c0_poly);
    Free_poly_data(&_pgen_tmp_6._c1_poly);
    Free_poly_data(&_pgen_tmp_6._c0_poly);
    Free_poly(_pgen_tmp_8);
    Free_poly(_pgen_tmp_9);
  }
  _preg_268435457 = Relinearize(_pgen_tmp_5);
  Free_poly_data(&_pgen_tmp_5._c2_poly);
  Free_poly_data(&_pgen_tmp_5._c1_poly);
  Free_poly_data(&_pgen_tmp_5._c0_poly);
  Copy_ciph(&_relu_tmp17, &_preg_268435457);
  Free_poly_data(&_preg_268435457._c1_poly);
  Free_poly_data(&_preg_268435457._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_10, &_relu_tmp17, &_relu_tmp17);
  Encode_plain_from_float(&_pgen_tmp_11, &_cst_212, 1, Sc_degree(&_pgen_tmp_10), Level(&_pgen_tmp_10));
  Init_ciph_same_scale_plain(&_relu_tmp23, &_pgen_tmp_10, &_pgen_tmp_11);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_relu_tmp23._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_10._c0_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp17._c0_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp17._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_10._c1_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp17._c1_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp17._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_relu_tmp23._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_10._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_11._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&_relu_tmp23._c1_poly, _pgen_rns_idx, degree, Coeffs(&_pgen_tmp_10._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_relu_tmp17._c1_poly);
    Free_poly_data(&_relu_tmp17._c0_poly);
    Free_poly_data(&_pgen_tmp_11._poly);
    Free_poly_data(&_pgen_tmp_10._c1_poly);
    Free_poly_data(&_pgen_tmp_10._c0_poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_13, &_relu_tmp23);
  Rescale(&_pgen_tmp_13._c0_poly, &_relu_tmp23._c0_poly);
  Rescale(&_pgen_tmp_13._c1_poly, &_relu_tmp23._c1_poly);
  Free_poly_data(&_relu_tmp23._c1_poly);
  Free_poly_data(&_relu_tmp23._c0_poly);
  Init_ciph_down_scale(&_pgen_tmp_16, &_relu_tmp13);
  Rescale(&_pgen_tmp_16._c0_poly, &_relu_tmp13._c0_poly);
  Rescale(&_pgen_tmp_16._c1_poly, &_relu_tmp13._c1_poly);
  Encode_plain_from_double(&_pgen_tmp_18, &_cst_213, 1, 1, Level(&input_ct1));
  Init_ciph_up_scale_plain(&_pgen_tmp_17, &input_ct1, &_pgen_tmp_18);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_17._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_17._c0_poly, _pgen_rns_idx, degree), Coeffs(&input_ct1._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_18._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_17._c1_poly, _pgen_rns_idx, degree), Coeffs(&input_ct1._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_18._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_18._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_19, &_pgen_tmp_17);
  Rescale(&_pgen_tmp_19._c0_poly, &_pgen_tmp_17._c0_poly);
  Rescale(&_pgen_tmp_19._c1_poly, &_pgen_tmp_17._c1_poly);
  Free_poly_data(&_pgen_tmp_17._c1_poly);
  Free_poly_data(&_pgen_tmp_17._c0_poly);
  Init_ciph3_up_scale(&_pgen_tmp_15, &_pgen_tmp_16, &_pgen_tmp_19);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_15._c0_poly);
    _pgen_tmp_20 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_21 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_20, 0, degree), Coeffs(&_pgen_tmp_16._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_19._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_21, 0, degree), Coeffs(&_pgen_tmp_16._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_19._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_15._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_16._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_19._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_15._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_20, 0, degree), Coeffs(_pgen_tmp_21, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_15._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_16._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_19._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_19._c1_poly);
    Free_poly_data(&_pgen_tmp_19._c0_poly);
    Free_poly_data(&_pgen_tmp_16._c1_poly);
    Free_poly_data(&_pgen_tmp_16._c0_poly);
    Free_poly(_pgen_tmp_20);
    Free_poly(_pgen_tmp_21);
  }
  _preg_268435458 = Relinearize(_pgen_tmp_15);
  Free_poly_data(&_pgen_tmp_15._c2_poly);
  Free_poly_data(&_pgen_tmp_15._c1_poly);
  Free_poly_data(&_pgen_tmp_15._c0_poly);
  Encode_plain_from_double(&_pgen_tmp_22, &_cst_214, 1, 1, Level(&input_ct1));
  Init_ciph_up_scale_plain(&_pgen_tmp_23, &input_ct1, &_pgen_tmp_22);
  Init_ciph_same_scale(&_pgen_tmp_14, &_preg_268435458, &_pgen_tmp_23);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_14._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_23._c0_poly, _pgen_rns_idx, degree), Coeffs(&input_ct1._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_22._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_23._c1_poly, _pgen_rns_idx, degree), Coeffs(&input_ct1._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_22._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_14._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435458._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_23._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_14._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435458._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_23._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_22._poly);
    Free_poly_data(&_pgen_tmp_23._c1_poly);
    Free_poly_data(&_pgen_tmp_23._c0_poly);
    Free_poly_data(&_preg_268435458._c1_poly);
    Free_poly_data(&_preg_268435458._c0_poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_24, &_pgen_tmp_14);
  Rescale(&_pgen_tmp_24._c0_poly, &_pgen_tmp_14._c0_poly);
  Rescale(&_pgen_tmp_24._c1_poly, &_pgen_tmp_14._c1_poly);
  Free_poly_data(&_pgen_tmp_14._c1_poly);
  Free_poly_data(&_pgen_tmp_14._c0_poly);
  Init_ciph3_up_scale(&_pgen_tmp_12, &_pgen_tmp_13, &_pgen_tmp_24);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_12._c0_poly);
    _pgen_tmp_25 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_26 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_25, 0, degree), Coeffs(&_pgen_tmp_13._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_24._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_26, 0, degree), Coeffs(&_pgen_tmp_13._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_24._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_12._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_13._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_24._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_12._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_25, 0, degree), Coeffs(_pgen_tmp_26, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_12._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_13._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_24._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_24._c1_poly);
    Free_poly_data(&_pgen_tmp_24._c0_poly);
    Free_poly_data(&_pgen_tmp_13._c1_poly);
    Free_poly_data(&_pgen_tmp_13._c0_poly);
    Free_poly(_pgen_tmp_25);
    Free_poly(_pgen_tmp_26);
  }
  _preg_268435459 = Relinearize(_pgen_tmp_12);
  Free_poly_data(&_pgen_tmp_12._c2_poly);
  Free_poly_data(&_pgen_tmp_12._c1_poly);
  Free_poly_data(&_pgen_tmp_12._c0_poly);
  Init_ciph_down_scale(&_pgen_tmp_28, &_relu_tmp13);
  Rescale(&_pgen_tmp_28._c0_poly, &_relu_tmp13._c0_poly);
  Rescale(&_pgen_tmp_28._c1_poly, &_relu_tmp13._c1_poly);
  Free_poly_data(&_relu_tmp13._c1_poly);
  Free_poly_data(&_relu_tmp13._c0_poly);
  Encode_plain_from_double(&_pgen_tmp_30, &_cst_215, 1, 1, Level(&input_ct1));
  Init_ciph_up_scale_plain(&_pgen_tmp_29, &input_ct1, &_pgen_tmp_30);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_29._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_29._c0_poly, _pgen_rns_idx, degree), Coeffs(&input_ct1._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_30._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_29._c1_poly, _pgen_rns_idx, degree), Coeffs(&input_ct1._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_30._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_30._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_31, &_pgen_tmp_29);
  Rescale(&_pgen_tmp_31._c0_poly, &_pgen_tmp_29._c0_poly);
  Rescale(&_pgen_tmp_31._c1_poly, &_pgen_tmp_29._c1_poly);
  Free_poly_data(&_pgen_tmp_29._c1_poly);
  Free_poly_data(&_pgen_tmp_29._c0_poly);
  Init_ciph3_up_scale(&_pgen_tmp_27, &_pgen_tmp_28, &_pgen_tmp_31);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_27._c0_poly);
    _pgen_tmp_32 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_33 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_32, 0, degree), Coeffs(&_pgen_tmp_28._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_31._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_33, 0, degree), Coeffs(&_pgen_tmp_28._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_31._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_27._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_28._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_31._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_27._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_32, 0, degree), Coeffs(_pgen_tmp_33, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_27._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_28._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_31._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_31._c1_poly);
    Free_poly_data(&_pgen_tmp_31._c0_poly);
    Free_poly_data(&_pgen_tmp_28._c1_poly);
    Free_poly_data(&_pgen_tmp_28._c0_poly);
    Free_poly(_pgen_tmp_32);
    Free_poly(_pgen_tmp_33);
  }
  _preg_268435460 = Relinearize(_pgen_tmp_27);
  Free_poly_data(&_pgen_tmp_27._c2_poly);
  Free_poly_data(&_pgen_tmp_27._c1_poly);
  Free_poly_data(&_pgen_tmp_27._c0_poly);
  Encode_plain_from_double(&_pgen_tmp_34, &_cst_216, 1, 1, Level(&input_ct1));
  Init_ciph_up_scale_plain(&_pgen_tmp_35, &input_ct1, &_pgen_tmp_34);
  Init_ciph_same_scale(&_pgen_tmp_36, &_preg_268435460, &_pgen_tmp_35);
  Init_ciph_same_scale(&_relu_tmp45, &_preg_268435459, &_pgen_tmp_36);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_relu_tmp45._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_35._c0_poly, _pgen_rns_idx, degree), Coeffs(&input_ct1._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_34._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_35._c1_poly, _pgen_rns_idx, degree), Coeffs(&input_ct1._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_34._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_36._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435460._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_35._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_36._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435460._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_35._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_relu_tmp45._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435459._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_36._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_relu_tmp45._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435459._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_36._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_34._poly);
    Free_poly_data(&_pgen_tmp_35._c1_poly);
    Free_poly_data(&_pgen_tmp_35._c0_poly);
    Free_poly_data(&_preg_268435460._c1_poly);
    Free_poly_data(&_preg_268435460._c0_poly);
    Free_poly_data(&_pgen_tmp_36._c1_poly);
    Free_poly_data(&_pgen_tmp_36._c0_poly);
    Free_poly_data(&_preg_268435459._c1_poly);
    Free_poly_data(&_preg_268435459._c0_poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_38, &_relu_tmp45);
  Rescale(&_pgen_tmp_38._c0_poly, &_relu_tmp45._c0_poly);
  Rescale(&_pgen_tmp_38._c1_poly, &_relu_tmp45._c1_poly);
  Init_ciph_down_scale(&_pgen_tmp_39, &_relu_tmp45);
  Rescale(&_pgen_tmp_39._c0_poly, &_relu_tmp45._c0_poly);
  Rescale(&_pgen_tmp_39._c1_poly, &_relu_tmp45._c1_poly);
  Init_ciph3_up_scale(&_pgen_tmp_37, &_pgen_tmp_38, &_pgen_tmp_39);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_37._c0_poly);
    _pgen_tmp_40 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_41 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_40, 0, degree), Coeffs(&_pgen_tmp_38._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_39._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_41, 0, degree), Coeffs(&_pgen_tmp_38._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_39._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_37._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_38._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_39._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_37._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_40, 0, degree), Coeffs(_pgen_tmp_41, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_37._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_38._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_39._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_39._c1_poly);
    Free_poly_data(&_pgen_tmp_39._c0_poly);
    Free_poly_data(&_pgen_tmp_38._c1_poly);
    Free_poly_data(&_pgen_tmp_38._c0_poly);
    Free_poly(_pgen_tmp_40);
    Free_poly(_pgen_tmp_41);
  }
  _preg_268435461 = Relinearize(_pgen_tmp_37);
  Free_poly_data(&_pgen_tmp_37._c2_poly);
  Free_poly_data(&_pgen_tmp_37._c1_poly);
  Free_poly_data(&_pgen_tmp_37._c0_poly);
  Copy_ciph(&_relu_tmp49, &_preg_268435461);
  Free_poly_data(&_preg_268435461._c1_poly);
  Free_poly_data(&_preg_268435461._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_42, &_relu_tmp49, &_relu_tmp49);
  Encode_plain_from_float(&_pgen_tmp_43, &_cst_217, 1, Sc_degree(&_pgen_tmp_42), Level(&_pgen_tmp_42));
  Init_ciph_same_scale_plain(&_relu_tmp55, &_pgen_tmp_42, &_pgen_tmp_43);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_relu_tmp55._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_42._c0_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp49._c0_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp49._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_42._c1_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp49._c1_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp49._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_relu_tmp55._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_42._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_43._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&_relu_tmp55._c1_poly, _pgen_rns_idx, degree, Coeffs(&_pgen_tmp_42._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_relu_tmp49._c1_poly);
    Free_poly_data(&_relu_tmp49._c0_poly);
    Free_poly_data(&_pgen_tmp_43._poly);
    Free_poly_data(&_pgen_tmp_42._c1_poly);
    Free_poly_data(&_pgen_tmp_42._c0_poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_45, &_relu_tmp45);
  Rescale(&_pgen_tmp_45._c0_poly, &_relu_tmp45._c0_poly);
  Rescale(&_pgen_tmp_45._c1_poly, &_relu_tmp45._c1_poly);
  Init_ciph_down_scale(&_pgen_tmp_46, &_relu_tmp55);
  Rescale(&_pgen_tmp_46._c0_poly, &_relu_tmp55._c0_poly);
  Rescale(&_pgen_tmp_46._c1_poly, &_relu_tmp55._c1_poly);
  Init_ciph3_up_scale(&_pgen_tmp_44, &_pgen_tmp_45, &_pgen_tmp_46);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_44._c0_poly);
    _pgen_tmp_47 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_48 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_47, 0, degree), Coeffs(&_pgen_tmp_45._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_46._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_48, 0, degree), Coeffs(&_pgen_tmp_45._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_46._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_44._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_45._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_46._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_44._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_47, 0, degree), Coeffs(_pgen_tmp_48, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_44._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_45._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_46._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_46._c1_poly);
    Free_poly_data(&_pgen_tmp_46._c0_poly);
    Free_poly_data(&_pgen_tmp_45._c1_poly);
    Free_poly_data(&_pgen_tmp_45._c0_poly);
    Free_poly(_pgen_tmp_47);
    Free_poly(_pgen_tmp_48);
  }
  _preg_268435462 = Relinearize(_pgen_tmp_44);
  Free_poly_data(&_pgen_tmp_44._c2_poly);
  Free_poly_data(&_pgen_tmp_44._c1_poly);
  Free_poly_data(&_pgen_tmp_44._c0_poly);
  Copy_ciph(&_relu_tmp59, &_preg_268435462);
  Free_poly_data(&_preg_268435462._c1_poly);
  Free_poly_data(&_preg_268435462._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_49, &_relu_tmp59, &_relu_tmp59);
  Init_ciph_down_scale(&_pgen_tmp_50, &_relu_tmp45);
  Rescale(&_pgen_tmp_50._c0_poly, &_relu_tmp45._c0_poly);
  Rescale(&_pgen_tmp_50._c1_poly, &_relu_tmp45._c1_poly);
  Encode_plain_from_float(&_pgen_tmp_51, &_cst_218, 1, 1, Level(&_pgen_tmp_50));
  Init_ciph_up_scale_plain(&_pgen_tmp_52, &_pgen_tmp_50, &_pgen_tmp_51);
  Init_ciph_same_scale(&_relu_tmp67, &_pgen_tmp_49, &_pgen_tmp_52);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_relu_tmp67._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_49._c0_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp59._c0_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp59._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_49._c1_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp59._c1_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp59._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_52._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_50._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_51._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_52._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_50._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_51._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_relu_tmp67._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_49._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_52._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_relu_tmp67._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_49._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_52._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_relu_tmp59._c1_poly);
    Free_poly_data(&_relu_tmp59._c0_poly);
    Free_poly_data(&_pgen_tmp_51._poly);
    Free_poly_data(&_pgen_tmp_50._c1_poly);
    Free_poly_data(&_pgen_tmp_50._c0_poly);
    Free_poly_data(&_pgen_tmp_52._c1_poly);
    Free_poly_data(&_pgen_tmp_52._c0_poly);
    Free_poly_data(&_pgen_tmp_49._c1_poly);
    Free_poly_data(&_pgen_tmp_49._c0_poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_54, &_relu_tmp55);
  Rescale(&_pgen_tmp_54._c0_poly, &_relu_tmp55._c0_poly);
  Rescale(&_pgen_tmp_54._c1_poly, &_relu_tmp55._c1_poly);
  Init_ciph_down_scale(&_pgen_tmp_55, &_relu_tmp55);
  Rescale(&_pgen_tmp_55._c0_poly, &_relu_tmp55._c0_poly);
  Rescale(&_pgen_tmp_55._c1_poly, &_relu_tmp55._c1_poly);
  Init_ciph3_up_scale(&_pgen_tmp_53, &_pgen_tmp_54, &_pgen_tmp_55);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_53._c0_poly);
    _pgen_tmp_56 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_57 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_56, 0, degree), Coeffs(&_pgen_tmp_54._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_55._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_57, 0, degree), Coeffs(&_pgen_tmp_54._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_55._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_53._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_54._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_55._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_53._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_56, 0, degree), Coeffs(_pgen_tmp_57, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_53._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_54._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_55._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_55._c1_poly);
    Free_poly_data(&_pgen_tmp_55._c0_poly);
    Free_poly_data(&_pgen_tmp_54._c1_poly);
    Free_poly_data(&_pgen_tmp_54._c0_poly);
    Free_poly(_pgen_tmp_56);
    Free_poly(_pgen_tmp_57);
  }
  _preg_268435463 = Relinearize(_pgen_tmp_53);
  Free_poly_data(&_pgen_tmp_53._c2_poly);
  Free_poly_data(&_pgen_tmp_53._c1_poly);
  Free_poly_data(&_pgen_tmp_53._c0_poly);
  Copy_ciph(&_relu_tmp71, &_preg_268435463);
  Free_poly_data(&_preg_268435463._c1_poly);
  Free_poly_data(&_preg_268435463._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_58, &_relu_tmp71, &_relu_tmp71);
  Encode_plain_from_float(&_pgen_tmp_59, &_cst_219, 1, Sc_degree(&_pgen_tmp_58), Level(&_pgen_tmp_58));
  Init_ciph_same_scale_plain(&_relu_tmp77, &_pgen_tmp_58, &_pgen_tmp_59);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_relu_tmp77._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_58._c0_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp71._c0_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp71._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_58._c1_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp71._c1_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp71._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_relu_tmp77._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_58._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_59._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&_relu_tmp77._c1_poly, _pgen_rns_idx, degree, Coeffs(&_pgen_tmp_58._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_relu_tmp71._c1_poly);
    Free_poly_data(&_relu_tmp71._c0_poly);
    Free_poly_data(&_pgen_tmp_59._poly);
    Free_poly_data(&_pgen_tmp_58._c1_poly);
    Free_poly_data(&_pgen_tmp_58._c0_poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_61, &_relu_tmp77);
  Rescale(&_pgen_tmp_61._c0_poly, &_relu_tmp77._c0_poly);
  Rescale(&_pgen_tmp_61._c1_poly, &_relu_tmp77._c1_poly);
  Init_ciph_down_scale(&_pgen_tmp_62, &_relu_tmp77);
  Rescale(&_pgen_tmp_62._c0_poly, &_relu_tmp77._c0_poly);
  Rescale(&_pgen_tmp_62._c1_poly, &_relu_tmp77._c1_poly);
  Init_ciph3_up_scale(&_pgen_tmp_60, &_pgen_tmp_61, &_pgen_tmp_62);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_60._c0_poly);
    _pgen_tmp_63 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_64 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_63, 0, degree), Coeffs(&_pgen_tmp_61._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_62._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_64, 0, degree), Coeffs(&_pgen_tmp_61._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_62._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_60._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_61._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_62._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_60._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_63, 0, degree), Coeffs(_pgen_tmp_64, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_60._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_61._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_62._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_62._c1_poly);
    Free_poly_data(&_pgen_tmp_62._c0_poly);
    Free_poly_data(&_pgen_tmp_61._c1_poly);
    Free_poly_data(&_pgen_tmp_61._c0_poly);
    Free_poly(_pgen_tmp_63);
    Free_poly(_pgen_tmp_64);
  }
  _preg_268435464 = Relinearize(_pgen_tmp_60);
  Free_poly_data(&_pgen_tmp_60._c2_poly);
  Free_poly_data(&_pgen_tmp_60._c1_poly);
  Free_poly_data(&_pgen_tmp_60._c0_poly);
  Copy_ciph(&_relu_tmp81, &_preg_268435464);
  Free_poly_data(&_preg_268435464._c1_poly);
  Free_poly_data(&_preg_268435464._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_65, &_relu_tmp81, &_relu_tmp81);
  Encode_plain_from_float(&_pgen_tmp_66, &_cst_220, 1, Sc_degree(&_pgen_tmp_65), Level(&_pgen_tmp_65));
  Init_ciph_same_scale_plain(&_relu_tmp87, &_pgen_tmp_65, &_pgen_tmp_66);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_relu_tmp87._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_65._c0_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp81._c0_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp81._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_65._c1_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp81._c1_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp81._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_relu_tmp87._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_65._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_66._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&_relu_tmp87._c1_poly, _pgen_rns_idx, degree, Coeffs(&_pgen_tmp_65._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_relu_tmp81._c1_poly);
    Free_poly_data(&_relu_tmp81._c0_poly);
    Free_poly_data(&_pgen_tmp_66._poly);
    Free_poly_data(&_pgen_tmp_65._c1_poly);
    Free_poly_data(&_pgen_tmp_65._c0_poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_68, &_relu_tmp87);
  Rescale(&_pgen_tmp_68._c0_poly, &_relu_tmp87._c0_poly);
  Rescale(&_pgen_tmp_68._c1_poly, &_relu_tmp87._c1_poly);
  Free_poly_data(&_relu_tmp87._c1_poly);
  Free_poly_data(&_relu_tmp87._c0_poly);
  Init_ciph_down_scale(&_pgen_tmp_71, &_relu_tmp77);
  Rescale(&_pgen_tmp_71._c0_poly, &_relu_tmp77._c0_poly);
  Rescale(&_pgen_tmp_71._c1_poly, &_relu_tmp77._c1_poly);
  Init_ciph_down_scale(&_pgen_tmp_74, &_relu_tmp55);
  Rescale(&_pgen_tmp_74._c0_poly, &_relu_tmp55._c0_poly);
  Rescale(&_pgen_tmp_74._c1_poly, &_relu_tmp55._c1_poly);
  Free_poly_data(&_relu_tmp55._c1_poly);
  Free_poly_data(&_relu_tmp55._c0_poly);
  Init_ciph_down_scale(&_pgen_tmp_76, &_relu_tmp45);
  Rescale(&_pgen_tmp_76._c0_poly, &_relu_tmp45._c0_poly);
  Rescale(&_pgen_tmp_76._c1_poly, &_relu_tmp45._c1_poly);
  Encode_plain_from_double(&_pgen_tmp_77, &_cst_221, 1, 1, Level(&_pgen_tmp_76));
  Init_ciph_up_scale_plain(&_pgen_tmp_75, &_pgen_tmp_76, &_pgen_tmp_77);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_75._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_75._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_76._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_77._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_75._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_76._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_77._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_77._poly);
    Free_poly_data(&_pgen_tmp_76._c1_poly);
    Free_poly_data(&_pgen_tmp_76._c0_poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_78, &_pgen_tmp_75);
  Rescale(&_pgen_tmp_78._c0_poly, &_pgen_tmp_75._c0_poly);
  Rescale(&_pgen_tmp_78._c1_poly, &_pgen_tmp_75._c1_poly);
  Free_poly_data(&_pgen_tmp_75._c1_poly);
  Free_poly_data(&_pgen_tmp_75._c0_poly);
  Init_ciph3_up_scale(&_pgen_tmp_73, &_pgen_tmp_74, &_pgen_tmp_78);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_73._c0_poly);
    _pgen_tmp_79 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_80 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_79, 0, degree), Coeffs(&_pgen_tmp_74._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_78._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_80, 0, degree), Coeffs(&_pgen_tmp_74._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_78._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_73._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_74._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_78._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_73._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_79, 0, degree), Coeffs(_pgen_tmp_80, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_73._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_74._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_78._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_78._c1_poly);
    Free_poly_data(&_pgen_tmp_78._c0_poly);
    Free_poly_data(&_pgen_tmp_74._c1_poly);
    Free_poly_data(&_pgen_tmp_74._c0_poly);
    Free_poly(_pgen_tmp_79);
    Free_poly(_pgen_tmp_80);
  }
  _preg_268435465 = Relinearize(_pgen_tmp_73);
  Free_poly_data(&_pgen_tmp_73._c2_poly);
  Free_poly_data(&_pgen_tmp_73._c1_poly);
  Free_poly_data(&_pgen_tmp_73._c0_poly);
  Init_ciph_down_scale(&_pgen_tmp_81, &_relu_tmp45);
  Rescale(&_pgen_tmp_81._c0_poly, &_relu_tmp45._c0_poly);
  Rescale(&_pgen_tmp_81._c1_poly, &_relu_tmp45._c1_poly);
  Encode_plain_from_double(&_pgen_tmp_82, &_cst_222, 1, 1, Level(&_pgen_tmp_81));
  Init_ciph_up_scale_plain(&_pgen_tmp_83, &_pgen_tmp_81, &_pgen_tmp_82);
  Init_ciph_same_scale(&_pgen_tmp_72, &_preg_268435465, &_pgen_tmp_83);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_72._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_83._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_81._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_82._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_83._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_81._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_82._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_72._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435465._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_83._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_72._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435465._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_83._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_82._poly);
    Free_poly_data(&_pgen_tmp_81._c1_poly);
    Free_poly_data(&_pgen_tmp_81._c0_poly);
    Free_poly_data(&_pgen_tmp_83._c1_poly);
    Free_poly_data(&_pgen_tmp_83._c0_poly);
    Free_poly_data(&_preg_268435465._c1_poly);
    Free_poly_data(&_preg_268435465._c0_poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_84, &_pgen_tmp_72);
  Rescale(&_pgen_tmp_84._c0_poly, &_pgen_tmp_72._c0_poly);
  Rescale(&_pgen_tmp_84._c1_poly, &_pgen_tmp_72._c1_poly);
  Free_poly_data(&_pgen_tmp_72._c1_poly);
  Free_poly_data(&_pgen_tmp_72._c0_poly);
  Init_ciph3_up_scale(&_pgen_tmp_70, &_pgen_tmp_71, &_pgen_tmp_84);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_70._c0_poly);
    _pgen_tmp_85 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_86 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_85, 0, degree), Coeffs(&_pgen_tmp_71._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_84._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_86, 0, degree), Coeffs(&_pgen_tmp_71._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_84._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_70._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_71._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_84._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_70._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_85, 0, degree), Coeffs(_pgen_tmp_86, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_70._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_71._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_84._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_84._c1_poly);
    Free_poly_data(&_pgen_tmp_84._c0_poly);
    Free_poly_data(&_pgen_tmp_71._c1_poly);
    Free_poly_data(&_pgen_tmp_71._c0_poly);
    Free_poly(_pgen_tmp_85);
    Free_poly(_pgen_tmp_86);
  }
  _preg_268435466 = Relinearize(_pgen_tmp_70);
  Free_poly_data(&_pgen_tmp_70._c2_poly);
  Free_poly_data(&_pgen_tmp_70._c1_poly);
  Free_poly_data(&_pgen_tmp_70._c0_poly);
  Init_ciph_down_scale(&_pgen_tmp_87, &_relu_tmp67);
  Rescale(&_pgen_tmp_87._c0_poly, &_relu_tmp67._c0_poly);
  Rescale(&_pgen_tmp_87._c1_poly, &_relu_tmp67._c1_poly);
  Encode_plain_from_double(&_pgen_tmp_88, &_cst_224, 1, 1, Level(&_pgen_tmp_87));
  Init_ciph_up_scale_plain(&_pgen_tmp_89, &_pgen_tmp_87, &_pgen_tmp_88);
  Init_ciph_down_scale(&_pgen_tmp_90, &_relu_tmp45);
  Rescale(&_pgen_tmp_90._c0_poly, &_relu_tmp45._c0_poly);
  Rescale(&_pgen_tmp_90._c1_poly, &_relu_tmp45._c1_poly);
  Encode_plain_from_double(&_pgen_tmp_91, &_cst_223, 1, 1, Level(&_pgen_tmp_90));
  Init_ciph_up_scale_plain(&_pgen_tmp_92, &_pgen_tmp_90, &_pgen_tmp_91);
  Init_ciph_same_scale(&_pgen_tmp_93, &_pgen_tmp_89, &_pgen_tmp_92);
  Init_ciph_same_scale(&_pgen_tmp_69, &_preg_268435466, &_pgen_tmp_93);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_69._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_89._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_87._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_88._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_89._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_87._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_88._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_92._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_90._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_91._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_92._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_90._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_91._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_93._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_89._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_92._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_93._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_89._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_92._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_69._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435466._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_93._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_69._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435466._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_93._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_88._poly);
    Free_poly_data(&_pgen_tmp_87._c1_poly);
    Free_poly_data(&_pgen_tmp_87._c0_poly);
    Free_poly_data(&_pgen_tmp_91._poly);
    Free_poly_data(&_pgen_tmp_90._c1_poly);
    Free_poly_data(&_pgen_tmp_90._c0_poly);
    Free_poly_data(&_pgen_tmp_92._c1_poly);
    Free_poly_data(&_pgen_tmp_92._c0_poly);
    Free_poly_data(&_pgen_tmp_89._c1_poly);
    Free_poly_data(&_pgen_tmp_89._c0_poly);
    Free_poly_data(&_pgen_tmp_93._c1_poly);
    Free_poly_data(&_pgen_tmp_93._c0_poly);
    Free_poly_data(&_preg_268435466._c1_poly);
    Free_poly_data(&_preg_268435466._c0_poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_94, &_pgen_tmp_69);
  Rescale(&_pgen_tmp_94._c0_poly, &_pgen_tmp_69._c0_poly);
  Rescale(&_pgen_tmp_94._c1_poly, &_pgen_tmp_69._c1_poly);
  Free_poly_data(&_pgen_tmp_69._c1_poly);
  Free_poly_data(&_pgen_tmp_69._c0_poly);
  Init_ciph3_up_scale(&_pgen_tmp_67, &_pgen_tmp_68, &_pgen_tmp_94);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_67._c0_poly);
    _pgen_tmp_95 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_96 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_95, 0, degree), Coeffs(&_pgen_tmp_68._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_94._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_96, 0, degree), Coeffs(&_pgen_tmp_68._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_94._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_67._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_68._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_94._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_67._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_95, 0, degree), Coeffs(_pgen_tmp_96, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_67._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_68._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_94._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_94._c1_poly);
    Free_poly_data(&_pgen_tmp_94._c0_poly);
    Free_poly_data(&_pgen_tmp_68._c1_poly);
    Free_poly_data(&_pgen_tmp_68._c0_poly);
    Free_poly(_pgen_tmp_95);
    Free_poly(_pgen_tmp_96);
  }
  _preg_268435467 = Relinearize(_pgen_tmp_67);
  Free_poly_data(&_pgen_tmp_67._c2_poly);
  Free_poly_data(&_pgen_tmp_67._c1_poly);
  Free_poly_data(&_pgen_tmp_67._c0_poly);
  Init_ciph_down_scale(&_pgen_tmp_98, &_relu_tmp77);
  Rescale(&_pgen_tmp_98._c0_poly, &_relu_tmp77._c0_poly);
  Rescale(&_pgen_tmp_98._c1_poly, &_relu_tmp77._c1_poly);
  Free_poly_data(&_relu_tmp77._c1_poly);
  Free_poly_data(&_relu_tmp77._c0_poly);
  Init_ciph_down_scale(&_pgen_tmp_100, &_relu_tmp67);
  Rescale(&_pgen_tmp_100._c0_poly, &_relu_tmp67._c0_poly);
  Rescale(&_pgen_tmp_100._c1_poly, &_relu_tmp67._c1_poly);
  Encode_plain_from_double(&_pgen_tmp_101, &_cst_226, 1, 1, Level(&_pgen_tmp_100));
  Init_ciph_up_scale_plain(&_pgen_tmp_102, &_pgen_tmp_100, &_pgen_tmp_101);
  Init_ciph_down_scale(&_pgen_tmp_103, &_relu_tmp45);
  Rescale(&_pgen_tmp_103._c0_poly, &_relu_tmp45._c0_poly);
  Rescale(&_pgen_tmp_103._c1_poly, &_relu_tmp45._c1_poly);
  Encode_plain_from_double(&_pgen_tmp_104, &_cst_225, 1, 1, Level(&_pgen_tmp_103));
  Init_ciph_up_scale_plain(&_pgen_tmp_105, &_pgen_tmp_103, &_pgen_tmp_104);
  Init_ciph_same_scale(&_pgen_tmp_99, &_pgen_tmp_102, &_pgen_tmp_105);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_99._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_102._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_100._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_101._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_102._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_100._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_101._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_105._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_103._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_104._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_105._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_103._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_104._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_99._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_102._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_105._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_99._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_102._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_105._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_101._poly);
    Free_poly_data(&_pgen_tmp_100._c1_poly);
    Free_poly_data(&_pgen_tmp_100._c0_poly);
    Free_poly_data(&_pgen_tmp_104._poly);
    Free_poly_data(&_pgen_tmp_103._c1_poly);
    Free_poly_data(&_pgen_tmp_103._c0_poly);
    Free_poly_data(&_pgen_tmp_105._c1_poly);
    Free_poly_data(&_pgen_tmp_105._c0_poly);
    Free_poly_data(&_pgen_tmp_102._c1_poly);
    Free_poly_data(&_pgen_tmp_102._c0_poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_106, &_pgen_tmp_99);
  Rescale(&_pgen_tmp_106._c0_poly, &_pgen_tmp_99._c0_poly);
  Rescale(&_pgen_tmp_106._c1_poly, &_pgen_tmp_99._c1_poly);
  Free_poly_data(&_pgen_tmp_99._c1_poly);
  Free_poly_data(&_pgen_tmp_99._c0_poly);
  Init_ciph3_up_scale(&_pgen_tmp_97, &_pgen_tmp_98, &_pgen_tmp_106);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_97._c0_poly);
    _pgen_tmp_107 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_108 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_107, 0, degree), Coeffs(&_pgen_tmp_98._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_106._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_108, 0, degree), Coeffs(&_pgen_tmp_98._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_106._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_97._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_98._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_106._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_97._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_107, 0, degree), Coeffs(_pgen_tmp_108, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_97._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_98._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_106._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_106._c1_poly);
    Free_poly_data(&_pgen_tmp_106._c0_poly);
    Free_poly_data(&_pgen_tmp_98._c1_poly);
    Free_poly_data(&_pgen_tmp_98._c0_poly);
    Free_poly(_pgen_tmp_107);
    Free_poly(_pgen_tmp_108);
  }
  _preg_268435468 = Relinearize(_pgen_tmp_97);
  Free_poly_data(&_pgen_tmp_97._c2_poly);
  Free_poly_data(&_pgen_tmp_97._c1_poly);
  Free_poly_data(&_pgen_tmp_97._c0_poly);
  Init_ciph_down_scale(&_pgen_tmp_109, &_relu_tmp67);
  Rescale(&_pgen_tmp_109._c0_poly, &_relu_tmp67._c0_poly);
  Rescale(&_pgen_tmp_109._c1_poly, &_relu_tmp67._c1_poly);
  Free_poly_data(&_relu_tmp67._c1_poly);
  Free_poly_data(&_relu_tmp67._c0_poly);
  Encode_plain_from_double(&_pgen_tmp_110, &_cst_228, 1, 1, Level(&_pgen_tmp_109));
  Init_ciph_up_scale_plain(&_pgen_tmp_111, &_pgen_tmp_109, &_pgen_tmp_110);
  Init_ciph_down_scale(&_pgen_tmp_112, &_relu_tmp45);
  Rescale(&_pgen_tmp_112._c0_poly, &_relu_tmp45._c0_poly);
  Rescale(&_pgen_tmp_112._c1_poly, &_relu_tmp45._c1_poly);
  Free_poly_data(&_relu_tmp45._c1_poly);
  Free_poly_data(&_relu_tmp45._c0_poly);
  Encode_plain_from_double(&_pgen_tmp_113, &_cst_227, 1, 1, Level(&_pgen_tmp_112));
  Init_ciph_up_scale_plain(&_pgen_tmp_114, &_pgen_tmp_112, &_pgen_tmp_113);
  Init_ciph_same_scale(&_pgen_tmp_115, &_pgen_tmp_111, &_pgen_tmp_114);
  Init_ciph_same_scale(&_pgen_tmp_116, &_preg_268435468, &_pgen_tmp_115);
  Init_ciph_same_scale(&_relu_tmp127, &_preg_268435467, &_pgen_tmp_116);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_relu_tmp127._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_111._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_109._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_110._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_111._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_109._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_110._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_114._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_112._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_113._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_114._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_112._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_113._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_115._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_111._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_114._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_115._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_111._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_114._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_116._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435468._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_115._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_116._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435468._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_115._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_relu_tmp127._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435467._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_116._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_relu_tmp127._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435467._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_116._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_110._poly);
    Free_poly_data(&_pgen_tmp_109._c1_poly);
    Free_poly_data(&_pgen_tmp_109._c0_poly);
    Free_poly_data(&_pgen_tmp_113._poly);
    Free_poly_data(&_pgen_tmp_112._c1_poly);
    Free_poly_data(&_pgen_tmp_112._c0_poly);
    Free_poly_data(&_pgen_tmp_114._c1_poly);
    Free_poly_data(&_pgen_tmp_114._c0_poly);
    Free_poly_data(&_pgen_tmp_111._c1_poly);
    Free_poly_data(&_pgen_tmp_111._c0_poly);
    Free_poly_data(&_pgen_tmp_115._c1_poly);
    Free_poly_data(&_pgen_tmp_115._c0_poly);
    Free_poly_data(&_preg_268435468._c1_poly);
    Free_poly_data(&_preg_268435468._c0_poly);
    Free_poly_data(&_pgen_tmp_116._c1_poly);
    Free_poly_data(&_pgen_tmp_116._c0_poly);
    Free_poly_data(&_preg_268435467._c1_poly);
    Free_poly_data(&_preg_268435467._c0_poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_118, &_relu_tmp127);
  Rescale(&_pgen_tmp_118._c0_poly, &_relu_tmp127._c0_poly);
  Rescale(&_pgen_tmp_118._c1_poly, &_relu_tmp127._c1_poly);
  Init_ciph_down_scale(&_pgen_tmp_119, &_relu_tmp127);
  Rescale(&_pgen_tmp_119._c0_poly, &_relu_tmp127._c0_poly);
  Rescale(&_pgen_tmp_119._c1_poly, &_relu_tmp127._c1_poly);
  Init_ciph3_up_scale(&_pgen_tmp_117, &_pgen_tmp_118, &_pgen_tmp_119);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_117._c0_poly);
    _pgen_tmp_120 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_121 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_120, 0, degree), Coeffs(&_pgen_tmp_118._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_119._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_121, 0, degree), Coeffs(&_pgen_tmp_118._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_119._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_117._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_118._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_119._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_117._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_120, 0, degree), Coeffs(_pgen_tmp_121, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_117._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_118._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_119._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_119._c1_poly);
    Free_poly_data(&_pgen_tmp_119._c0_poly);
    Free_poly_data(&_pgen_tmp_118._c1_poly);
    Free_poly_data(&_pgen_tmp_118._c0_poly);
    Free_poly(_pgen_tmp_120);
    Free_poly(_pgen_tmp_121);
  }
  _preg_268435469 = Relinearize(_pgen_tmp_117);
  Free_poly_data(&_pgen_tmp_117._c2_poly);
  Free_poly_data(&_pgen_tmp_117._c1_poly);
  Free_poly_data(&_pgen_tmp_117._c0_poly);
  Copy_ciph(&_relu_tmp131, &_preg_268435469);
  Free_poly_data(&_preg_268435469._c1_poly);
  Free_poly_data(&_preg_268435469._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_122, &_relu_tmp131, &_relu_tmp131);
  Encode_plain_from_float(&_pgen_tmp_123, &_cst_229, 1, Sc_degree(&_pgen_tmp_122), Level(&_pgen_tmp_122));
  Init_ciph_same_scale_plain(&_relu_tmp137, &_pgen_tmp_122, &_pgen_tmp_123);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_relu_tmp137._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_122._c0_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp131._c0_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp131._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_122._c1_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp131._c1_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp131._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_relu_tmp137._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_122._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_123._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&_relu_tmp137._c1_poly, _pgen_rns_idx, degree, Coeffs(&_pgen_tmp_122._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_relu_tmp131._c1_poly);
    Free_poly_data(&_relu_tmp131._c0_poly);
    Free_poly_data(&_pgen_tmp_123._poly);
    Free_poly_data(&_pgen_tmp_122._c1_poly);
    Free_poly_data(&_pgen_tmp_122._c0_poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_125, &_relu_tmp127);
  Rescale(&_pgen_tmp_125._c0_poly, &_relu_tmp127._c0_poly);
  Rescale(&_pgen_tmp_125._c1_poly, &_relu_tmp127._c1_poly);
  Init_ciph_down_scale(&_pgen_tmp_126, &_relu_tmp137);
  Rescale(&_pgen_tmp_126._c0_poly, &_relu_tmp137._c0_poly);
  Rescale(&_pgen_tmp_126._c1_poly, &_relu_tmp137._c1_poly);
  Init_ciph3_up_scale(&_pgen_tmp_124, &_pgen_tmp_125, &_pgen_tmp_126);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_124._c0_poly);
    _pgen_tmp_127 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_128 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_127, 0, degree), Coeffs(&_pgen_tmp_125._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_126._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_128, 0, degree), Coeffs(&_pgen_tmp_125._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_126._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_124._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_125._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_126._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_124._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_127, 0, degree), Coeffs(_pgen_tmp_128, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_124._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_125._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_126._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_126._c1_poly);
    Free_poly_data(&_pgen_tmp_126._c0_poly);
    Free_poly_data(&_pgen_tmp_125._c1_poly);
    Free_poly_data(&_pgen_tmp_125._c0_poly);
    Free_poly(_pgen_tmp_127);
    Free_poly(_pgen_tmp_128);
  }
  _preg_268435470 = Relinearize(_pgen_tmp_124);
  Free_poly_data(&_pgen_tmp_124._c2_poly);
  Free_poly_data(&_pgen_tmp_124._c1_poly);
  Free_poly_data(&_pgen_tmp_124._c0_poly);
  Copy_ciph(&_relu_tmp141, &_preg_268435470);
  Free_poly_data(&_preg_268435470._c1_poly);
  Free_poly_data(&_preg_268435470._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_129, &_relu_tmp141, &_relu_tmp141);
  Init_ciph_down_scale(&_pgen_tmp_130, &_relu_tmp127);
  Rescale(&_pgen_tmp_130._c0_poly, &_relu_tmp127._c0_poly);
  Rescale(&_pgen_tmp_130._c1_poly, &_relu_tmp127._c1_poly);
  Encode_plain_from_float(&_pgen_tmp_131, &_cst_230, 1, 1, Level(&_pgen_tmp_130));
  Init_ciph_up_scale_plain(&_pgen_tmp_132, &_pgen_tmp_130, &_pgen_tmp_131);
  Init_ciph_same_scale(&_relu_tmp149, &_pgen_tmp_129, &_pgen_tmp_132);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_relu_tmp149._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_129._c0_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp141._c0_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp141._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_129._c1_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp141._c1_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp141._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_132._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_130._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_131._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_132._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_130._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_131._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_relu_tmp149._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_129._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_132._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_relu_tmp149._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_129._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_132._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_relu_tmp141._c1_poly);
    Free_poly_data(&_relu_tmp141._c0_poly);
    Free_poly_data(&_pgen_tmp_131._poly);
    Free_poly_data(&_pgen_tmp_130._c1_poly);
    Free_poly_data(&_pgen_tmp_130._c0_poly);
    Free_poly_data(&_pgen_tmp_132._c1_poly);
    Free_poly_data(&_pgen_tmp_132._c0_poly);
    Free_poly_data(&_pgen_tmp_129._c1_poly);
    Free_poly_data(&_pgen_tmp_129._c0_poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_134, &_relu_tmp137);
  Rescale(&_pgen_tmp_134._c0_poly, &_relu_tmp137._c0_poly);
  Rescale(&_pgen_tmp_134._c1_poly, &_relu_tmp137._c1_poly);
  Init_ciph_down_scale(&_pgen_tmp_135, &_relu_tmp137);
  Rescale(&_pgen_tmp_135._c0_poly, &_relu_tmp137._c0_poly);
  Rescale(&_pgen_tmp_135._c1_poly, &_relu_tmp137._c1_poly);
  Free_poly_data(&_relu_tmp137._c1_poly);
  Free_poly_data(&_relu_tmp137._c0_poly);
  Init_ciph3_up_scale(&_pgen_tmp_133, &_pgen_tmp_134, &_pgen_tmp_135);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_133._c0_poly);
    _pgen_tmp_136 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_137 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_136, 0, degree), Coeffs(&_pgen_tmp_134._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_135._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_137, 0, degree), Coeffs(&_pgen_tmp_134._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_135._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_133._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_134._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_135._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_133._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_136, 0, degree), Coeffs(_pgen_tmp_137, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_133._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_134._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_135._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_135._c1_poly);
    Free_poly_data(&_pgen_tmp_135._c0_poly);
    Free_poly_data(&_pgen_tmp_134._c1_poly);
    Free_poly_data(&_pgen_tmp_134._c0_poly);
    Free_poly(_pgen_tmp_136);
    Free_poly(_pgen_tmp_137);
  }
  _preg_268435471 = Relinearize(_pgen_tmp_133);
  Free_poly_data(&_pgen_tmp_133._c2_poly);
  Free_poly_data(&_pgen_tmp_133._c1_poly);
  Free_poly_data(&_pgen_tmp_133._c0_poly);
  Copy_ciph(&_relu_tmp153, &_preg_268435471);
  Free_poly_data(&_preg_268435471._c1_poly);
  Free_poly_data(&_preg_268435471._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_138, &_relu_tmp153, &_relu_tmp153);
  Encode_plain_from_float(&_pgen_tmp_139, &_cst_231, 1, Sc_degree(&_pgen_tmp_138), Level(&_pgen_tmp_138));
  Init_ciph_same_scale_plain(&_relu_tmp159, &_pgen_tmp_138, &_pgen_tmp_139);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_relu_tmp159._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_138._c0_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp153._c0_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp153._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_138._c1_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp153._c1_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp153._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_relu_tmp159._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_138._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_139._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&_relu_tmp159._c1_poly, _pgen_rns_idx, degree, Coeffs(&_pgen_tmp_138._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_relu_tmp153._c1_poly);
    Free_poly_data(&_relu_tmp153._c0_poly);
    Free_poly_data(&_pgen_tmp_139._poly);
    Free_poly_data(&_pgen_tmp_138._c1_poly);
    Free_poly_data(&_pgen_tmp_138._c0_poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_141, &_relu_tmp159);
  Rescale(&_pgen_tmp_141._c0_poly, &_relu_tmp159._c0_poly);
  Rescale(&_pgen_tmp_141._c1_poly, &_relu_tmp159._c1_poly);
  Init_ciph_down_scale(&_pgen_tmp_142, &_relu_tmp159);
  Rescale(&_pgen_tmp_142._c0_poly, &_relu_tmp159._c0_poly);
  Rescale(&_pgen_tmp_142._c1_poly, &_relu_tmp159._c1_poly);
  Init_ciph3_up_scale(&_pgen_tmp_140, &_pgen_tmp_141, &_pgen_tmp_142);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_140._c0_poly);
    _pgen_tmp_143 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_144 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_143, 0, degree), Coeffs(&_pgen_tmp_141._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_142._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_144, 0, degree), Coeffs(&_pgen_tmp_141._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_142._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_140._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_141._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_142._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_140._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_143, 0, degree), Coeffs(_pgen_tmp_144, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_140._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_141._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_142._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_142._c1_poly);
    Free_poly_data(&_pgen_tmp_142._c0_poly);
    Free_poly_data(&_pgen_tmp_141._c1_poly);
    Free_poly_data(&_pgen_tmp_141._c0_poly);
    Free_poly(_pgen_tmp_143);
    Free_poly(_pgen_tmp_144);
  }
  _preg_268435472 = Relinearize(_pgen_tmp_140);
  Free_poly_data(&_pgen_tmp_140._c2_poly);
  Free_poly_data(&_pgen_tmp_140._c1_poly);
  Free_poly_data(&_pgen_tmp_140._c0_poly);
  Copy_ciph(&_relu_tmp163, &_preg_268435472);
  Free_poly_data(&_preg_268435472._c1_poly);
  Free_poly_data(&_preg_268435472._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_145, &_relu_tmp163, &_relu_tmp163);
  Encode_plain_from_float(&_pgen_tmp_146, &_cst_232, 1, Sc_degree(&_pgen_tmp_145), Level(&_pgen_tmp_145));
  Init_ciph_same_scale_plain(&_relu_tmp169, &_pgen_tmp_145, &_pgen_tmp_146);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_relu_tmp169._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_145._c0_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp163._c0_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp163._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_145._c1_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp163._c1_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp163._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_relu_tmp169._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_145._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_146._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&_relu_tmp169._c1_poly, _pgen_rns_idx, degree, Coeffs(&_pgen_tmp_145._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_relu_tmp163._c1_poly);
    Free_poly_data(&_relu_tmp163._c0_poly);
    Free_poly_data(&_pgen_tmp_146._poly);
    Free_poly_data(&_pgen_tmp_145._c1_poly);
    Free_poly_data(&_pgen_tmp_145._c0_poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_148, &_relu_tmp169);
  Rescale(&_pgen_tmp_148._c0_poly, &_relu_tmp169._c0_poly);
  Rescale(&_pgen_tmp_148._c1_poly, &_relu_tmp169._c1_poly);
  Free_poly_data(&_relu_tmp169._c1_poly);
  Free_poly_data(&_relu_tmp169._c0_poly);
  Init_ciph_down_scale(&_pgen_tmp_151, &_relu_tmp159);
  Rescale(&_pgen_tmp_151._c0_poly, &_relu_tmp159._c0_poly);
  Rescale(&_pgen_tmp_151._c1_poly, &_relu_tmp159._c1_poly);
  Init_ciph_down_scale(&_pgen_tmp_153, &_relu_tmp127);
  Rescale(&_pgen_tmp_153._c0_poly, &_relu_tmp127._c0_poly);
  Rescale(&_pgen_tmp_153._c1_poly, &_relu_tmp127._c1_poly);
  Encode_plain_from_double(&_pgen_tmp_155, &_cst_233, 1, 1, Level(&input_ct0));
  Init_ciph_up_scale_plain(&_pgen_tmp_154, &input_ct0, &_pgen_tmp_155);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_154._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_154._c0_poly, _pgen_rns_idx, degree), Coeffs(&input_ct0._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_155._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_154._c1_poly, _pgen_rns_idx, degree), Coeffs(&input_ct0._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_155._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_155._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_156, &_pgen_tmp_154);
  Rescale(&_pgen_tmp_156._c0_poly, &_pgen_tmp_154._c0_poly);
  Rescale(&_pgen_tmp_156._c1_poly, &_pgen_tmp_154._c1_poly);
  Free_poly_data(&_pgen_tmp_154._c1_poly);
  Free_poly_data(&_pgen_tmp_154._c0_poly);
  Init_ciph3_up_scale(&_pgen_tmp_152, &_pgen_tmp_153, &_pgen_tmp_156);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_152._c0_poly);
    _pgen_tmp_157 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_158 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_157, 0, degree), Coeffs(&_pgen_tmp_153._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_156._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_158, 0, degree), Coeffs(&_pgen_tmp_153._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_156._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_152._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_153._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_156._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_152._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_157, 0, degree), Coeffs(_pgen_tmp_158, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_152._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_153._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_156._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_156._c1_poly);
    Free_poly_data(&_pgen_tmp_156._c0_poly);
    Free_poly_data(&_pgen_tmp_153._c1_poly);
    Free_poly_data(&_pgen_tmp_153._c0_poly);
    Free_poly(_pgen_tmp_157);
    Free_poly(_pgen_tmp_158);
  }
  _preg_268435473 = Relinearize(_pgen_tmp_152);
  Free_poly_data(&_pgen_tmp_152._c2_poly);
  Free_poly_data(&_pgen_tmp_152._c1_poly);
  Free_poly_data(&_pgen_tmp_152._c0_poly);
  Init_ciph_down_scale(&_pgen_tmp_159, &_preg_268435473);
  Rescale(&_pgen_tmp_159._c0_poly, &_preg_268435473._c0_poly);
  Rescale(&_pgen_tmp_159._c1_poly, &_preg_268435473._c1_poly);
  Free_poly_data(&_preg_268435473._c1_poly);
  Free_poly_data(&_preg_268435473._c0_poly);
  Init_ciph3_up_scale(&_pgen_tmp_150, &_pgen_tmp_151, &_pgen_tmp_159);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_150._c0_poly);
    _pgen_tmp_160 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_161 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_160, 0, degree), Coeffs(&_pgen_tmp_151._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_159._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_161, 0, degree), Coeffs(&_pgen_tmp_151._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_159._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_150._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_151._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_159._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_150._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_160, 0, degree), Coeffs(_pgen_tmp_161, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_150._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_151._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_159._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_159._c1_poly);
    Free_poly_data(&_pgen_tmp_159._c0_poly);
    Free_poly_data(&_pgen_tmp_151._c1_poly);
    Free_poly_data(&_pgen_tmp_151._c0_poly);
    Free_poly(_pgen_tmp_160);
    Free_poly(_pgen_tmp_161);
  }
  _preg_268435474 = Relinearize(_pgen_tmp_150);
  Free_poly_data(&_pgen_tmp_150._c2_poly);
  Free_poly_data(&_pgen_tmp_150._c1_poly);
  Free_poly_data(&_pgen_tmp_150._c0_poly);
  Init_ciph_down_scale(&_pgen_tmp_163, &_relu_tmp149);
  Rescale(&_pgen_tmp_163._c0_poly, &_relu_tmp149._c0_poly);
  Rescale(&_pgen_tmp_163._c1_poly, &_relu_tmp149._c1_poly);
  Encode_plain_from_double(&_pgen_tmp_165, &_cst_235, 1, 1, Level(&input_ct0));
  Init_ciph_up_scale_plain(&_pgen_tmp_164, &input_ct0, &_pgen_tmp_165);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_164._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_164._c0_poly, _pgen_rns_idx, degree), Coeffs(&input_ct0._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_165._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_164._c1_poly, _pgen_rns_idx, degree), Coeffs(&input_ct0._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_165._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_165._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_166, &_pgen_tmp_164);
  Rescale(&_pgen_tmp_166._c0_poly, &_pgen_tmp_164._c0_poly);
  Rescale(&_pgen_tmp_166._c1_poly, &_pgen_tmp_164._c1_poly);
  Free_poly_data(&_pgen_tmp_164._c1_poly);
  Free_poly_data(&_pgen_tmp_164._c0_poly);
  Init_ciph3_up_scale(&_pgen_tmp_162, &_pgen_tmp_163, &_pgen_tmp_166);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_162._c0_poly);
    _pgen_tmp_167 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_168 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_167, 0, degree), Coeffs(&_pgen_tmp_163._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_166._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_168, 0, degree), Coeffs(&_pgen_tmp_163._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_166._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_162._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_163._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_166._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_162._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_167, 0, degree), Coeffs(_pgen_tmp_168, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_162._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_163._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_166._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_166._c1_poly);
    Free_poly_data(&_pgen_tmp_166._c0_poly);
    Free_poly_data(&_pgen_tmp_163._c1_poly);
    Free_poly_data(&_pgen_tmp_163._c0_poly);
    Free_poly(_pgen_tmp_167);
    Free_poly(_pgen_tmp_168);
  }
  _preg_268435475 = Relinearize(_pgen_tmp_162);
  Free_poly_data(&_pgen_tmp_162._c2_poly);
  Free_poly_data(&_pgen_tmp_162._c1_poly);
  Free_poly_data(&_pgen_tmp_162._c0_poly);
  Init_ciph_down_scale(&_pgen_tmp_170, &_relu_tmp127);
  Rescale(&_pgen_tmp_170._c0_poly, &_relu_tmp127._c0_poly);
  Rescale(&_pgen_tmp_170._c1_poly, &_relu_tmp127._c1_poly);
  Encode_plain_from_double(&_pgen_tmp_172, &_cst_234, 1, 1, Level(&input_ct0));
  Init_ciph_up_scale_plain(&_pgen_tmp_171, &input_ct0, &_pgen_tmp_172);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_171._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_171._c0_poly, _pgen_rns_idx, degree), Coeffs(&input_ct0._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_172._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_171._c1_poly, _pgen_rns_idx, degree), Coeffs(&input_ct0._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_172._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_172._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_173, &_pgen_tmp_171);
  Rescale(&_pgen_tmp_173._c0_poly, &_pgen_tmp_171._c0_poly);
  Rescale(&_pgen_tmp_173._c1_poly, &_pgen_tmp_171._c1_poly);
  Free_poly_data(&_pgen_tmp_171._c1_poly);
  Free_poly_data(&_pgen_tmp_171._c0_poly);
  Init_ciph3_up_scale(&_pgen_tmp_169, &_pgen_tmp_170, &_pgen_tmp_173);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_169._c0_poly);
    _pgen_tmp_174 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_175 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_174, 0, degree), Coeffs(&_pgen_tmp_170._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_173._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_175, 0, degree), Coeffs(&_pgen_tmp_170._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_173._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_169._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_170._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_173._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_169._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_174, 0, degree), Coeffs(_pgen_tmp_175, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_169._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_170._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_173._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_173._c1_poly);
    Free_poly_data(&_pgen_tmp_173._c0_poly);
    Free_poly_data(&_pgen_tmp_170._c1_poly);
    Free_poly_data(&_pgen_tmp_170._c0_poly);
    Free_poly(_pgen_tmp_174);
    Free_poly(_pgen_tmp_175);
  }
  _preg_268435476 = Relinearize(_pgen_tmp_169);
  Free_poly_data(&_pgen_tmp_169._c2_poly);
  Free_poly_data(&_pgen_tmp_169._c1_poly);
  Free_poly_data(&_pgen_tmp_169._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_176, &_preg_268435475, &_preg_268435476);
  Init_ciph_same_scale(&_pgen_tmp_149, &_preg_268435474, &_pgen_tmp_176);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_149._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_176._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435475._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435476._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_176._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435475._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435476._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_149._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435474._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_176._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_149._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435474._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_176._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435476._c1_poly);
    Free_poly_data(&_preg_268435476._c0_poly);
    Free_poly_data(&_preg_268435475._c1_poly);
    Free_poly_data(&_preg_268435475._c0_poly);
    Free_poly_data(&_pgen_tmp_176._c1_poly);
    Free_poly_data(&_pgen_tmp_176._c0_poly);
    Free_poly_data(&_preg_268435474._c1_poly);
    Free_poly_data(&_preg_268435474._c0_poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_177, &_pgen_tmp_149);
  Rescale(&_pgen_tmp_177._c0_poly, &_pgen_tmp_149._c0_poly);
  Rescale(&_pgen_tmp_177._c1_poly, &_pgen_tmp_149._c1_poly);
  Free_poly_data(&_pgen_tmp_149._c1_poly);
  Free_poly_data(&_pgen_tmp_149._c0_poly);
  Init_ciph3_up_scale(&_pgen_tmp_147, &_pgen_tmp_148, &_pgen_tmp_177);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_147._c0_poly);
    _pgen_tmp_178 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_179 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_178, 0, degree), Coeffs(&_pgen_tmp_148._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_177._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_179, 0, degree), Coeffs(&_pgen_tmp_148._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_177._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_147._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_148._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_177._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_147._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_178, 0, degree), Coeffs(_pgen_tmp_179, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_147._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_148._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_177._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_177._c1_poly);
    Free_poly_data(&_pgen_tmp_177._c0_poly);
    Free_poly_data(&_pgen_tmp_148._c1_poly);
    Free_poly_data(&_pgen_tmp_148._c0_poly);
    Free_poly(_pgen_tmp_178);
    Free_poly(_pgen_tmp_179);
  }
  _preg_268435477 = Relinearize(_pgen_tmp_147);
  Free_poly_data(&_pgen_tmp_147._c2_poly);
  Free_poly_data(&_pgen_tmp_147._c1_poly);
  Free_poly_data(&_pgen_tmp_147._c0_poly);
  Init_ciph_down_scale(&_pgen_tmp_181, &_relu_tmp159);
  Rescale(&_pgen_tmp_181._c0_poly, &_relu_tmp159._c0_poly);
  Rescale(&_pgen_tmp_181._c1_poly, &_relu_tmp159._c1_poly);
  Free_poly_data(&_relu_tmp159._c1_poly);
  Free_poly_data(&_relu_tmp159._c0_poly);
  Init_ciph_down_scale(&_pgen_tmp_184, &_relu_tmp149);
  Rescale(&_pgen_tmp_184._c0_poly, &_relu_tmp149._c0_poly);
  Rescale(&_pgen_tmp_184._c1_poly, &_relu_tmp149._c1_poly);
  Encode_plain_from_double(&_pgen_tmp_186, &_cst_237, 1, 1, Level(&input_ct0));
  Init_ciph_up_scale_plain(&_pgen_tmp_185, &input_ct0, &_pgen_tmp_186);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_185._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_185._c0_poly, _pgen_rns_idx, degree), Coeffs(&input_ct0._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_186._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_185._c1_poly, _pgen_rns_idx, degree), Coeffs(&input_ct0._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_186._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_186._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_187, &_pgen_tmp_185);
  Rescale(&_pgen_tmp_187._c0_poly, &_pgen_tmp_185._c0_poly);
  Rescale(&_pgen_tmp_187._c1_poly, &_pgen_tmp_185._c1_poly);
  Free_poly_data(&_pgen_tmp_185._c1_poly);
  Free_poly_data(&_pgen_tmp_185._c0_poly);
  Init_ciph3_up_scale(&_pgen_tmp_183, &_pgen_tmp_184, &_pgen_tmp_187);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_183._c0_poly);
    _pgen_tmp_188 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_189 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_188, 0, degree), Coeffs(&_pgen_tmp_184._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_187._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_189, 0, degree), Coeffs(&_pgen_tmp_184._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_187._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_183._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_184._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_187._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_183._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_188, 0, degree), Coeffs(_pgen_tmp_189, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_183._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_184._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_187._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_187._c1_poly);
    Free_poly_data(&_pgen_tmp_187._c0_poly);
    Free_poly_data(&_pgen_tmp_184._c1_poly);
    Free_poly_data(&_pgen_tmp_184._c0_poly);
    Free_poly(_pgen_tmp_188);
    Free_poly(_pgen_tmp_189);
  }
  _preg_268435478 = Relinearize(_pgen_tmp_183);
  Free_poly_data(&_pgen_tmp_183._c2_poly);
  Free_poly_data(&_pgen_tmp_183._c1_poly);
  Free_poly_data(&_pgen_tmp_183._c0_poly);
  Init_ciph_down_scale(&_pgen_tmp_191, &_relu_tmp127);
  Rescale(&_pgen_tmp_191._c0_poly, &_relu_tmp127._c0_poly);
  Rescale(&_pgen_tmp_191._c1_poly, &_relu_tmp127._c1_poly);
  Encode_plain_from_double(&_pgen_tmp_193, &_cst_236, 1, 1, Level(&input_ct0));
  Init_ciph_up_scale_plain(&_pgen_tmp_192, &input_ct0, &_pgen_tmp_193);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_192._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_192._c0_poly, _pgen_rns_idx, degree), Coeffs(&input_ct0._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_193._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_192._c1_poly, _pgen_rns_idx, degree), Coeffs(&input_ct0._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_193._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_193._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_194, &_pgen_tmp_192);
  Rescale(&_pgen_tmp_194._c0_poly, &_pgen_tmp_192._c0_poly);
  Rescale(&_pgen_tmp_194._c1_poly, &_pgen_tmp_192._c1_poly);
  Free_poly_data(&_pgen_tmp_192._c1_poly);
  Free_poly_data(&_pgen_tmp_192._c0_poly);
  Init_ciph3_up_scale(&_pgen_tmp_190, &_pgen_tmp_191, &_pgen_tmp_194);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_190._c0_poly);
    _pgen_tmp_195 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_196 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_195, 0, degree), Coeffs(&_pgen_tmp_191._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_194._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_196, 0, degree), Coeffs(&_pgen_tmp_191._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_194._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_190._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_191._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_194._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_190._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_195, 0, degree), Coeffs(_pgen_tmp_196, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_190._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_191._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_194._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_194._c1_poly);
    Free_poly_data(&_pgen_tmp_194._c0_poly);
    Free_poly_data(&_pgen_tmp_191._c1_poly);
    Free_poly_data(&_pgen_tmp_191._c0_poly);
    Free_poly(_pgen_tmp_195);
    Free_poly(_pgen_tmp_196);
  }
  _preg_268435479 = Relinearize(_pgen_tmp_190);
  Free_poly_data(&_pgen_tmp_190._c2_poly);
  Free_poly_data(&_pgen_tmp_190._c1_poly);
  Free_poly_data(&_pgen_tmp_190._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_182, &_preg_268435478, &_preg_268435479);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_182._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_182._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435478._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435479._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_182._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435478._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435479._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435479._c1_poly);
    Free_poly_data(&_preg_268435479._c0_poly);
    Free_poly_data(&_preg_268435478._c1_poly);
    Free_poly_data(&_preg_268435478._c0_poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_197, &_pgen_tmp_182);
  Rescale(&_pgen_tmp_197._c0_poly, &_pgen_tmp_182._c0_poly);
  Rescale(&_pgen_tmp_197._c1_poly, &_pgen_tmp_182._c1_poly);
  Free_poly_data(&_pgen_tmp_182._c1_poly);
  Free_poly_data(&_pgen_tmp_182._c0_poly);
  Init_ciph3_up_scale(&_pgen_tmp_180, &_pgen_tmp_181, &_pgen_tmp_197);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_180._c0_poly);
    _pgen_tmp_198 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_199 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_198, 0, degree), Coeffs(&_pgen_tmp_181._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_197._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_199, 0, degree), Coeffs(&_pgen_tmp_181._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_197._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_180._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_181._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_197._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_180._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_198, 0, degree), Coeffs(_pgen_tmp_199, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_180._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_181._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_197._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_197._c1_poly);
    Free_poly_data(&_pgen_tmp_197._c0_poly);
    Free_poly_data(&_pgen_tmp_181._c1_poly);
    Free_poly_data(&_pgen_tmp_181._c0_poly);
    Free_poly(_pgen_tmp_198);
    Free_poly(_pgen_tmp_199);
  }
  _preg_268435480 = Relinearize(_pgen_tmp_180);
  Free_poly_data(&_pgen_tmp_180._c2_poly);
  Free_poly_data(&_pgen_tmp_180._c1_poly);
  Free_poly_data(&_pgen_tmp_180._c0_poly);
  Init_ciph_down_scale(&_pgen_tmp_201, &_relu_tmp149);
  Rescale(&_pgen_tmp_201._c0_poly, &_relu_tmp149._c0_poly);
  Rescale(&_pgen_tmp_201._c1_poly, &_relu_tmp149._c1_poly);
  Free_poly_data(&_relu_tmp149._c1_poly);
  Free_poly_data(&_relu_tmp149._c0_poly);
  Encode_plain_from_double(&_pgen_tmp_203, &_cst_239, 1, 1, Level(&input_ct0));
  Init_ciph_up_scale_plain(&_pgen_tmp_202, &input_ct0, &_pgen_tmp_203);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_202._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_202._c0_poly, _pgen_rns_idx, degree), Coeffs(&input_ct0._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_203._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_202._c1_poly, _pgen_rns_idx, degree), Coeffs(&input_ct0._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_203._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_203._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_204, &_pgen_tmp_202);
  Rescale(&_pgen_tmp_204._c0_poly, &_pgen_tmp_202._c0_poly);
  Rescale(&_pgen_tmp_204._c1_poly, &_pgen_tmp_202._c1_poly);
  Free_poly_data(&_pgen_tmp_202._c1_poly);
  Free_poly_data(&_pgen_tmp_202._c0_poly);
  Init_ciph3_up_scale(&_pgen_tmp_200, &_pgen_tmp_201, &_pgen_tmp_204);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_200._c0_poly);
    _pgen_tmp_205 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_206 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_205, 0, degree), Coeffs(&_pgen_tmp_201._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_204._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_206, 0, degree), Coeffs(&_pgen_tmp_201._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_204._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_200._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_201._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_204._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_200._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_205, 0, degree), Coeffs(_pgen_tmp_206, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_200._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_201._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_204._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_204._c1_poly);
    Free_poly_data(&_pgen_tmp_204._c0_poly);
    Free_poly_data(&_pgen_tmp_201._c1_poly);
    Free_poly_data(&_pgen_tmp_201._c0_poly);
    Free_poly(_pgen_tmp_205);
    Free_poly(_pgen_tmp_206);
  }
  _preg_268435481 = Relinearize(_pgen_tmp_200);
  Free_poly_data(&_pgen_tmp_200._c2_poly);
  Free_poly_data(&_pgen_tmp_200._c1_poly);
  Free_poly_data(&_pgen_tmp_200._c0_poly);
  Init_ciph_down_scale(&_pgen_tmp_208, &_relu_tmp127);
  Rescale(&_pgen_tmp_208._c0_poly, &_relu_tmp127._c0_poly);
  Rescale(&_pgen_tmp_208._c1_poly, &_relu_tmp127._c1_poly);
  Free_poly_data(&_relu_tmp127._c1_poly);
  Free_poly_data(&_relu_tmp127._c0_poly);
  Encode_plain_from_double(&_pgen_tmp_210, &_cst_238, 1, 1, Level(&input_ct0));
  Init_ciph_up_scale_plain(&_pgen_tmp_209, &input_ct0, &_pgen_tmp_210);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_209._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_209._c0_poly, _pgen_rns_idx, degree), Coeffs(&input_ct0._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_210._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_209._c1_poly, _pgen_rns_idx, degree), Coeffs(&input_ct0._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_210._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_210._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_211, &_pgen_tmp_209);
  Rescale(&_pgen_tmp_211._c0_poly, &_pgen_tmp_209._c0_poly);
  Rescale(&_pgen_tmp_211._c1_poly, &_pgen_tmp_209._c1_poly);
  Free_poly_data(&_pgen_tmp_209._c1_poly);
  Free_poly_data(&_pgen_tmp_209._c0_poly);
  Init_ciph3_up_scale(&_pgen_tmp_207, &_pgen_tmp_208, &_pgen_tmp_211);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_207._c0_poly);
    _pgen_tmp_212 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_213 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_212, 0, degree), Coeffs(&_pgen_tmp_208._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_211._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_213, 0, degree), Coeffs(&_pgen_tmp_208._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_211._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_207._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_208._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_211._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_207._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_212, 0, degree), Coeffs(_pgen_tmp_213, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_207._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_208._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_211._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_211._c1_poly);
    Free_poly_data(&_pgen_tmp_211._c0_poly);
    Free_poly_data(&_pgen_tmp_208._c1_poly);
    Free_poly_data(&_pgen_tmp_208._c0_poly);
    Free_poly(_pgen_tmp_212);
    Free_poly(_pgen_tmp_213);
  }
  _preg_268435482 = Relinearize(_pgen_tmp_207);
  Free_poly_data(&_pgen_tmp_207._c2_poly);
  Free_poly_data(&_pgen_tmp_207._c1_poly);
  Free_poly_data(&_pgen_tmp_207._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_214, &_preg_268435481, &_preg_268435482);
  Init_ciph_same_scale(&_pgen_tmp_215, &_preg_268435480, &_pgen_tmp_214);
  Init_ciph_same_scale(&_relu_tmp217, &_preg_268435477, &_pgen_tmp_215);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_relu_tmp217._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_214._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435481._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435482._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_214._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435481._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435482._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_215._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435480._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_214._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_215._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435480._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_214._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_relu_tmp217._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435477._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_215._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_relu_tmp217._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435477._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_215._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435482._c1_poly);
    Free_poly_data(&_preg_268435482._c0_poly);
    Free_poly_data(&_preg_268435481._c1_poly);
    Free_poly_data(&_preg_268435481._c0_poly);
    Free_poly_data(&_pgen_tmp_214._c1_poly);
    Free_poly_data(&_pgen_tmp_214._c0_poly);
    Free_poly_data(&_preg_268435480._c1_poly);
    Free_poly_data(&_preg_268435480._c0_poly);
    Free_poly_data(&_pgen_tmp_215._c1_poly);
    Free_poly_data(&_pgen_tmp_215._c0_poly);
    Free_poly_data(&_preg_268435477._c1_poly);
    Free_poly_data(&_preg_268435477._c0_poly);
  }
  Encode_plain_from_float(&_pgen_tmp_216, &_cst_240, 1, 1, Level(&input_ct0));
  Init_ciph_up_scale_plain(&_pgen_tmp_217, &input_ct0, &_pgen_tmp_216);
  Init_ciph_same_scale(&_relu_tmp223, &_relu_tmp217, &_pgen_tmp_217);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_relu_tmp223._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_217._c0_poly, _pgen_rns_idx, degree), Coeffs(&input_ct0._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_216._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_217._c1_poly, _pgen_rns_idx, degree), Coeffs(&input_ct0._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_216._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_relu_tmp223._c0_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp217._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_217._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_relu_tmp223._c1_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp217._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_217._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_216._poly);
    Free_poly_data(&_pgen_tmp_217._c1_poly);
    Free_poly_data(&_pgen_tmp_217._c0_poly);
    Free_poly_data(&_relu_tmp217._c1_poly);
    Free_poly_data(&_relu_tmp217._c0_poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_253, &_relu_tmp223);
  Rescale(&_rescale_tmp_253._c0_poly, &_relu_tmp223._c0_poly);
  Rescale(&_rescale_tmp_253._c1_poly, &_relu_tmp223._c1_poly);
  Free_poly_data(&_relu_tmp223._c1_poly);
  Free_poly_data(&_relu_tmp223._c0_poly);
  return _rescale_tmp_253;
}

CIPHERTEXT Rotate(CIPHERTEXT ciph, int32_t rot_idx) {
  CIPHERTEXT _pgen_rot_res;
  POLY _pgen_swk_c0;
  POLY _pgen_swk_c1;
  POLY _pgen_ext;
  POLY _pgen_tmp_poly;
  POLY _pgen_mod_down_c0;
  POLY _pgen_mod_down_c1;
  SWITCH_KEY* _pgen_swk;
  uint32_t _pgen_part_idx;
  POLY _pgen_key0;
  POLY _pgen_key1;
  MODULUS* _pgen_modulus;
  uint32_t _pgen_num_q;
  uint32_t _pgen_rns_idx;
  uint32_t _pgen_num_p;
  uint32_t _pgen_p_ofst;
  uint32_t _pgen_p_idx;
  uint32_t _pgen_key_p_ofst;
  uint32_t _pgen_key_p_idx;
  int64_t* _pgen_order;
  uint32_t  degree = Degree();
  memset(&_pgen_rot_res, 0, sizeof(_pgen_rot_res));
  Init_ciph_same_scale(&_pgen_rot_res, &ciph, 0);
  {
    _pgen_swk_c0 = Alloc_poly(degree, Poly_level(&ciph._c1_poly), 1);
    _pgen_swk_c1 = Alloc_poly(degree, Poly_level(&ciph._c1_poly), 1);
    _pgen_ext = Alloc_poly(degree, Poly_level(&ciph._c1_poly), 1);
    _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&ciph._c0_poly), 0);
    _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&ciph._c1_poly), 0);
    _pgen_swk = Swk(1, rot_idx);
    for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&ciph._c1_poly); _pgen_part_idx = _pgen_part_idx + 1) {
      Decomp_modup(_pgen_ext, &ciph._c1_poly, _pgen_part_idx);
      _pgen_key0 = Pk0_at(_pgen_swk, _pgen_part_idx);
      _pgen_key1 = Pk1_at(_pgen_swk, _pgen_part_idx);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(_pgen_ext);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(_pgen_tmp_poly, 0, degree), Coeffs(_pgen_key0, _pgen_rns_idx, degree), Coeffs(_pgen_ext, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(_pgen_swk_c0, _pgen_rns_idx, degree), Coeffs(_pgen_swk_c0, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_poly, 0, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(_pgen_tmp_poly, 0, degree), Coeffs(_pgen_key1, _pgen_rns_idx, degree), Coeffs(_pgen_ext, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(_pgen_swk_c1, _pgen_rns_idx, degree), Coeffs(_pgen_swk_c1, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_poly, 0, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
      {
        _pgen_modulus = P_modulus();
        _pgen_num_p = Num_p(_pgen_ext);
        _pgen_p_ofst = Num_alloc(_pgen_ext) - _pgen_num_p;
        _pgen_key_p_ofst = Poly_level(_pgen_key0);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_p; _pgen_rns_idx = _pgen_rns_idx + 1) {
          _pgen_p_idx = _pgen_rns_idx + _pgen_p_ofst;
          _pgen_key_p_idx = _pgen_rns_idx + _pgen_key_p_ofst;
          Hw_modmul(Coeffs(_pgen_tmp_poly, 0, degree), Coeffs(_pgen_key0, _pgen_key_p_idx, degree), Coeffs(_pgen_ext, _pgen_p_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(_pgen_swk_c0, _pgen_p_idx, degree), Coeffs(_pgen_swk_c0, _pgen_p_idx, degree), Coeffs(_pgen_tmp_poly, 0, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(_pgen_tmp_poly, 0, degree), Coeffs(_pgen_key1, _pgen_key_p_idx, degree), Coeffs(_pgen_ext, _pgen_p_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(_pgen_swk_c1, _pgen_p_idx, degree), Coeffs(_pgen_swk_c1, _pgen_p_idx, degree), Coeffs(_pgen_tmp_poly, 0, degree), _pgen_modulus, degree);
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
        Hw_modadd(Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(&ciph._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _pgen_order = Auto_order(rot_idx);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(_pgen_mod_down_c0);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_rotate(Coeffs(&_pgen_rot_res._c0_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), _pgen_order, _pgen_modulus, degree);
        Hw_rotate(Coeffs(&_pgen_rot_res._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree), _pgen_order, _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly(_pgen_swk_c0);
    Free_poly(_pgen_swk_c1);
    Free_poly(_pgen_ext);
    Free_poly(_pgen_tmp_poly);
    Free_poly(_pgen_mod_down_c0);
    Free_poly(_pgen_mod_down_c1);
    return _pgen_rot_res;
  }
}

CIPHERTEXT Relinearize(CIPHERTEXT3 ciph3) {
  CIPHERTEXT _pgen_relin_res;
  POLY _pgen_swk_c0;
  POLY _pgen_swk_c1;
  POLY _pgen_ext;
  POLY _pgen_tmp_poly;
  POLY _pgen_mod_down_c0;
  POLY _pgen_mod_down_c1;
  SWITCH_KEY* _pgen_swk;
  uint32_t _pgen_part_idx;
  POLY _pgen_key0;
  POLY _pgen_key1;
  MODULUS* _pgen_modulus;
  uint32_t _pgen_num_q;
  uint32_t _pgen_rns_idx;
  uint32_t _pgen_num_p;
  uint32_t _pgen_p_ofst;
  uint32_t _pgen_p_idx;
  uint32_t _pgen_key_p_ofst;
  uint32_t _pgen_key_p_idx;
  uint32_t  degree = Degree();
  memset(&_pgen_relin_res, 0, sizeof(_pgen_relin_res));
  Init_ciph_same_scale_ciph3(&_pgen_relin_res, &ciph3);
  {
    _pgen_swk_c0 = Alloc_poly(degree, Poly_level(&ciph3._c2_poly), 1);
    _pgen_swk_c1 = Alloc_poly(degree, Poly_level(&ciph3._c2_poly), 1);
    _pgen_ext = Alloc_poly(degree, Poly_level(&ciph3._c2_poly), 1);
    _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&ciph3._c0_poly), 0);
    _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&ciph3._c2_poly), 0);
    _pgen_swk = Swk(0, 0);
    for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&ciph3._c2_poly); _pgen_part_idx = _pgen_part_idx + 1) {
      Decomp_modup(_pgen_ext, &ciph3._c2_poly, _pgen_part_idx);
      _pgen_key0 = Pk0_at(_pgen_swk, _pgen_part_idx);
      _pgen_key1 = Pk1_at(_pgen_swk, _pgen_part_idx);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(_pgen_ext);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(_pgen_tmp_poly, 0, degree), Coeffs(_pgen_key0, _pgen_rns_idx, degree), Coeffs(_pgen_ext, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(_pgen_swk_c0, _pgen_rns_idx, degree), Coeffs(_pgen_swk_c0, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_poly, 0, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(_pgen_tmp_poly, 0, degree), Coeffs(_pgen_key1, _pgen_rns_idx, degree), Coeffs(_pgen_ext, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(_pgen_swk_c1, _pgen_rns_idx, degree), Coeffs(_pgen_swk_c1, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_poly, 0, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
      {
        _pgen_modulus = P_modulus();
        _pgen_num_p = Num_p(_pgen_ext);
        _pgen_p_ofst = Num_alloc(_pgen_ext) - _pgen_num_p;
        _pgen_key_p_ofst = Poly_level(_pgen_key0);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_p; _pgen_rns_idx = _pgen_rns_idx + 1) {
          _pgen_p_idx = _pgen_rns_idx + _pgen_p_ofst;
          _pgen_key_p_idx = _pgen_rns_idx + _pgen_key_p_ofst;
          Hw_modmul(Coeffs(_pgen_tmp_poly, 0, degree), Coeffs(_pgen_key0, _pgen_key_p_idx, degree), Coeffs(_pgen_ext, _pgen_p_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(_pgen_swk_c0, _pgen_p_idx, degree), Coeffs(_pgen_swk_c0, _pgen_p_idx, degree), Coeffs(_pgen_tmp_poly, 0, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(_pgen_tmp_poly, 0, degree), Coeffs(_pgen_key1, _pgen_key_p_idx, degree), Coeffs(_pgen_ext, _pgen_p_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(_pgen_swk_c1, _pgen_p_idx, degree), Coeffs(_pgen_swk_c1, _pgen_p_idx, degree), Coeffs(_pgen_tmp_poly, 0, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    Mod_down(_pgen_mod_down_c0, _pgen_swk_c0);
    Mod_down(_pgen_mod_down_c1, _pgen_swk_c1);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&ciph3._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_relin_res._c0_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(&ciph3._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_relin_res._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree), Coeffs(&ciph3._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly(_pgen_swk_c0);
    Free_poly(_pgen_swk_c1);
    Free_poly(_pgen_ext);
    Free_poly(_pgen_tmp_poly);
    Free_poly(_pgen_mod_down_c0);
    Free_poly(_pgen_mod_down_c1);
    return _pgen_relin_res;
  }
}

CKKS_PARAMS* Get_context_params() {
  static CKKS_PARAMS parm = {
    LIB_ANT, 65536, 0, 33, 51, 50, 3, 192, 197, 
    { -16384, -12288, -8192, -4096, -70, -33, -32, -31,
      -17, -16, -15, -9, -8, -7, -1, 0,
      1, 2, 3, 4, 5, 6, 7, 8,
      9, 10, 11, 12, 13, 14, 15, 16,
      17, 20, 24, 31, 32, 33, 40, 48,
      63, 64, 72, 96, 120, 126, 128, 144,
      168, 192, 240, 252, 256, 288, 320, 336,
      384, 432, 448, 480, 504, 512, 528, 576,
      624, 640, 672, 704, 720, 768, 832, 896,
      960, 1008, 1024, 1088, 1152, 1216, 1280, 1344,
      1408, 1472, 1536, 1600, 1664, 1728, 1792, 1856,
      1920, 1984, 2016, 2048, 2112, 2176, 2240, 2304,
      2368, 2432, 2496, 2560, 2624, 2688, 2752, 2816,
      2880, 2944, 3008, 3072, 3136, 3200, 3264, 3328,
      3392, 3456, 3520, 3584, 3648, 3712, 3776, 3840,
      3904, 3968, 4032, 4096, 4224, 4352, 4416, 4608,
      4800, 4864, 4992, 5120, 5184, 5376, 5568, 5632,
      5760, 5888, 5952, 6144, 6336, 6400, 6528, 6656,
      6720, 6912, 7104, 7168, 7296, 7424, 7488, 7680,
      7872, 7936, 8064, 8192, 8256, 8448, 8640, 8832,
      9024, 9216, 9408, 9600, 9792, 9984, 10176, 10240,
      10368, 10560, 10752, 10944, 11136, 11264, 11328, 11520,
      11712, 11904, 12096, 12288, 13056, 13312, 13824, 14336,
      14592, 15360, 16128, 16896, 17664, 18432, 19200, 19968,
      20736, 21504, 22272, 23040, 23808 }
  };
  return &parm;
}

RT_DATA_INFO* Get_rt_data_info() {
  static RT_DATA_INFO info = {
    "/app/release/resnet20_cifar10_pre.onnx.omp.msg",
    "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX",
    DE_MSG_F32
  };
  return &info;
}

int32_t _cst_61[9] = {
  -33, -32, -31, -1, 0, 1, 31, 32,
  33
};
int32_t _cst_67[9] = {
  -33, -32, -31, -1, 0, 1, 31, 32,
  33
};
int32_t _cst_73[9] = {
  -33, -32, -31, -1, 0, 1, 31, 32,
  33
};
int32_t _cst_80[9] = {
  -33, -32, -31, -1, 0, 1, 31, 32,
  33
};
int32_t _cst_86[9] = {
  -33, -32, -31, -1, 0, 1, 31, 32,
  33
};
int32_t _cst_93[9] = {
  -33, -32, -31, -1, 0, 1, 31, 32,
  33
};
int32_t _cst_99[9] = {
  -33, -32, -31, -1, 0, 1, 31, 32,
  33
};
int32_t _cst_106[9] = {
  -33, -32, -31, -1, 0, 1, 31, 32,
  33
};
int32_t _cst_115[9] = {
  -17, -16, -15, -1, 0, 1, 15, 16,
  17
};
int32_t _cst_121[1] = {
  0
};
int32_t _cst_131[9] = {
  -17, -16, -15, -1, 0, 1, 15, 16,
  17
};
int32_t _cst_137[9] = {
  -17, -16, -15, -1, 0, 1, 15, 16,
  17
};
int32_t _cst_144[9] = {
  -17, -16, -15, -1, 0, 1, 15, 16,
  17
};
int32_t _cst_150[9] = {
  -17, -16, -15, -1, 0, 1, 15, 16,
  17
};
int32_t _cst_157[9] = {
  -17, -16, -15, -1, 0, 1, 15, 16,
  17
};
int32_t _cst_166[9] = {
  -9, -8, -7, -1, 0, 1, 7, 8,
  9
};
int32_t _cst_172[1] = {
  0
};
int32_t _cst_182[9] = {
  -9, -8, -7, -1, 0, 1, 7, 8,
  9
};
int32_t _cst_188[9] = {
  -9, -8, -7, -1, 0, 1, 7, 8,
  9
};
int32_t _cst_195[9] = {
  -9, -8, -7, -1, 0, 1, 7, 8,
  9
};
int32_t _cst_201[9] = {
  -9, -8, -7, -1, 0, 1, 7, 8,
  9
};
