// external header files
#include "rt_ant/rt_ant.h"

typedef double float64_t;
typedef float float32_t;

extern float32_t _cst_0[512000];
extern float32_t _cst_1[1000];
extern float32_t _cst_2[9408];
extern float32_t _cst_3[64];
extern float32_t _cst_4[36864];
extern float32_t _cst_5[64];
extern float32_t _cst_6[36864];
extern float32_t _cst_7[64];
extern float32_t _cst_8[36864];
extern float32_t _cst_9[64];
extern float32_t _cst_10[36864];
extern float32_t _cst_11[64];
extern float32_t _cst_12[8192];
extern float32_t _cst_13[128];
extern float32_t _cst_14[73728];
extern float32_t _cst_15[128];
extern float32_t _cst_16[147456];
extern float32_t _cst_17[128];
extern float32_t _cst_18[147456];
extern float32_t _cst_19[128];
extern float32_t _cst_20[147456];
extern float32_t _cst_21[128];
extern float32_t _cst_22[32768];
extern float32_t _cst_23[256];
extern float32_t _cst_24[294912];
extern float32_t _cst_25[256];
extern float32_t _cst_26[589824];
extern float32_t _cst_27[256];
extern float32_t _cst_28[589824];
extern float32_t _cst_29[256];
extern float32_t _cst_30[589824];
extern float32_t _cst_31[256];
extern float32_t _cst_32[131072];
extern float32_t _cst_33[512];
extern float32_t _cst_34[1179648];
extern float32_t _cst_35[512];
extern float32_t _cst_36[2359296];
extern float32_t _cst_37[512];
extern float32_t _cst_38[2359296];
extern float32_t _cst_39[512];
extern float32_t _cst_40[2359296];
extern float32_t _cst_41[512];
extern float32_t _cst_42[9408];
extern float32_t _cst_43[802816];
extern float32_t _cst_44[802816];
extern float32_t _cst_45[1];
extern int64_t _cst_46[2];
extern int64_t _cst_47[2];
extern int64_t _cst_48[2];
extern float32_t _cst_49[36864];
extern float32_t _cst_50[200704];
extern float32_t _cst_51[200704];
extern float32_t _cst_52[8];
extern float32_t _cst_53[36864];
extern float32_t _cst_54[200704];
extern float32_t _cst_55[200704];
extern float32_t _cst_56[8];
extern float32_t _cst_57[36864];
extern float32_t _cst_58[200704];
extern float32_t _cst_59[200704];
extern float32_t _cst_60[8];
extern float32_t _cst_61[36864];
extern float32_t _cst_62[200704];
extern float32_t _cst_63[200704];
extern float32_t _cst_64[8];
extern float32_t _cst_65[8192];
extern float32_t _cst_66[100352];
extern float32_t _cst_67[100352];
extern float32_t _cst_68[8];
extern int64_t _cst_69[2];
extern int64_t _cst_70[2];
extern int64_t _cst_71[2];
extern float32_t _cst_72[73728];
extern float32_t _cst_73[100352];
extern float32_t _cst_74[100352];
extern float32_t _cst_75[8];
extern int64_t _cst_76[2];
extern int64_t _cst_77[2];
extern int64_t _cst_78[2];
extern float32_t _cst_79[147456];
extern float32_t _cst_80[100352];
extern float32_t _cst_81[100352];
extern float32_t _cst_82[32];
extern float32_t _cst_83[147456];
extern float32_t _cst_84[100352];
extern float32_t _cst_85[100352];
extern float32_t _cst_86[32];
extern float32_t _cst_87[147456];
extern float32_t _cst_88[100352];
extern float32_t _cst_89[100352];
extern float32_t _cst_90[32];
extern float32_t _cst_91[32768];
extern float32_t _cst_92[50176];
extern float32_t _cst_93[50176];
extern float32_t _cst_94[32];
extern int64_t _cst_95[2];
extern int64_t _cst_96[2];
extern int64_t _cst_97[2];
extern float32_t _cst_98[294912];
extern float32_t _cst_99[50176];
extern float32_t _cst_100[50176];
extern float32_t _cst_101[32];
extern int64_t _cst_102[2];
extern int64_t _cst_103[2];
extern int64_t _cst_104[2];
extern float32_t _cst_105[589824];
extern float32_t _cst_106[50176];
extern float32_t _cst_107[50176];
extern float32_t _cst_108[128];
extern float32_t _cst_109[589824];
extern float32_t _cst_110[50176];
extern float32_t _cst_111[50176];
extern float32_t _cst_112[128];
extern float32_t _cst_113[589824];
extern float32_t _cst_114[50176];
extern float32_t _cst_115[50176];
extern float32_t _cst_116[128];
extern float32_t _cst_117[131072];
extern float32_t _cst_118[25088];
extern float32_t _cst_119[128];
extern int64_t _cst_120[2];
extern int64_t _cst_121[2];
extern int64_t _cst_122[2];
extern float32_t _cst_123[1179648];
extern float32_t _cst_124[25088];
extern float32_t _cst_125[128];
extern int64_t _cst_126[2];
extern int64_t _cst_127[2];
extern int64_t _cst_128[2];
extern int64_t _cst_129[2];
extern int64_t _cst_130[2];
extern int64_t _cst_131[2];
extern int64_t _cst_132[2];
extern int64_t _cst_133[2];
extern int64_t _cst_134[2];
extern int32_t _cst_137[1];
extern float32_t _cst_138[248672256];
extern float32_t _cst_139[1295168];
extern float32_t _cst_140[26432];
extern int32_t _cst_141[49];
extern float32_t _cst_142[2809856];
extern float32_t _cst_143[1404928];
extern float32_t _cst_148[802816];
extern float32_t _cst_151[25088];
extern float32_t _cst_153[1404928];
extern float32_t _cst_154[1404928];
extern float32_t _cst_155[6272];
extern int32_t _cst_161[1];
extern float32_t _cst_162[115605504];
extern float32_t _cst_163[1806336];
extern float32_t _cst_164[25088];
extern int32_t _cst_165[9];
extern float32_t _cst_166[25088];
extern float32_t _cst_167[25088];
extern float32_t _cst_169[200704];
extern int32_t _cst_173[1];
extern float32_t _cst_174[115605504];
extern float32_t _cst_175[1806336];
extern float32_t _cst_176[25088];
extern int32_t _cst_177[9];
extern float32_t _cst_178[25088];
extern float32_t _cst_179[25088];
extern float32_t _cst_181[200704];
extern int32_t _cst_187[1];
extern float32_t _cst_188[115605504];
extern float32_t _cst_189[1806336];
extern float32_t _cst_190[25088];
extern int32_t _cst_191[9];
extern float32_t _cst_192[25088];
extern float32_t _cst_193[25088];
extern float32_t _cst_195[200704];
extern int32_t _cst_199[1];
extern float32_t _cst_200[115605504];
extern float32_t _cst_201[1806336];
extern float32_t _cst_202[25088];
extern int32_t _cst_203[9];
extern float32_t _cst_204[25088];
extern float32_t _cst_205[25088];
extern float32_t _cst_207[200704];
extern int32_t _cst_213[1];
extern float32_t _cst_214[25690112];
extern float32_t _cst_215[200704];
extern float32_t _cst_216[25088];
extern int32_t _cst_217[1];
extern float32_t _cst_218[25088];
extern float32_t _cst_219[25088];
extern float32_t _cst_220[702464];
extern float32_t _cst_221[702464];
extern float32_t _cst_222[200704];
extern float32_t _cst_227[100352];
extern int32_t _cst_230[1];
extern float32_t _cst_231[231211008];
extern float32_t _cst_232[1806336];
extern float32_t _cst_233[25088];
extern int32_t _cst_234[9];
extern float32_t _cst_235[25088];
extern float32_t _cst_236[25088];
extern float32_t _cst_237[702464];
extern float32_t _cst_238[702464];
extern float32_t _cst_239[200704];
extern float32_t _cst_244[100352];
extern int32_t _cst_248[1];
extern float32_t _cst_249[115605504];
extern float32_t _cst_250[7225344];
extern float32_t _cst_251[25088];
extern int32_t _cst_252[9];
extern float32_t _cst_253[25088];
extern float32_t _cst_254[25088];
extern float32_t _cst_256[100352];
extern int32_t _cst_262[1];
extern float32_t _cst_263[115605504];
extern float32_t _cst_264[7225344];
extern float32_t _cst_265[25088];
extern int32_t _cst_266[9];
extern float32_t _cst_267[25088];
extern float32_t _cst_268[25088];
extern float32_t _cst_270[100352];
extern int32_t _cst_274[1];
extern float32_t _cst_275[115605504];
extern float32_t _cst_276[7225344];
extern float32_t _cst_277[25088];
extern int32_t _cst_278[9];
extern float32_t _cst_279[25088];
extern float32_t _cst_280[25088];
extern float32_t _cst_282[100352];
extern int32_t _cst_288[1];
extern float32_t _cst_289[25690112];
extern float32_t _cst_290[802816];
extern float32_t _cst_291[25088];
extern int32_t _cst_292[4];
extern float32_t _cst_293[25088];
extern float32_t _cst_294[25088];
extern float32_t _cst_295[351232];
extern float32_t _cst_296[351232];
extern float32_t _cst_297[802816];
extern float32_t _cst_302[50176];
extern int32_t _cst_305[1];
extern float32_t _cst_306[231211008];
extern float32_t _cst_307[7225344];
extern float32_t _cst_308[25088];
extern int32_t _cst_309[9];
extern float32_t _cst_310[25088];
extern float32_t _cst_311[25088];
extern float32_t _cst_312[351232];
extern float32_t _cst_313[351232];
extern float32_t _cst_314[802816];
extern float32_t _cst_319[50176];
extern int32_t _cst_323[1];
extern float32_t _cst_324[115605504];
extern float32_t _cst_325[28901376];
extern float32_t _cst_326[25088];
extern int32_t _cst_327[9];
extern float32_t _cst_328[25088];
extern float32_t _cst_329[25088];
extern float32_t _cst_331[50176];
extern int32_t _cst_337[1];
extern float32_t _cst_338[115605504];
extern float32_t _cst_339[28901376];
extern float32_t _cst_340[25088];
extern int32_t _cst_341[9];
extern float32_t _cst_342[25088];
extern float32_t _cst_343[25088];
extern float32_t _cst_345[50176];
extern int32_t _cst_349[1];
extern float32_t _cst_350[115605504];
extern float32_t _cst_351[28901376];
extern float32_t _cst_352[25088];
extern int32_t _cst_353[9];
extern float32_t _cst_354[25088];
extern float32_t _cst_355[25088];
extern float32_t _cst_357[50176];
extern int32_t _cst_363[1];
extern float32_t _cst_364[25690112];
extern float32_t _cst_365[3211264];
extern float32_t _cst_366[25088];
extern int32_t _cst_367[8];
extern float32_t _cst_368[25088];
extern float32_t _cst_369[25088];
extern float32_t _cst_370[175616];
extern float32_t _cst_371[175616];
extern float32_t _cst_372[3211264];
extern int32_t _cst_379[1];
extern float32_t _cst_380[231211008];
extern float32_t _cst_381[28901376];
extern float32_t _cst_382[25088];
extern int32_t _cst_383[9];
extern float32_t _cst_384[25088];
extern float32_t _cst_385[25088];
extern float32_t _cst_386[175616];
extern float32_t _cst_387[175616];
extern float32_t _cst_388[3211264];
extern int32_t _cst_394[1];
extern float32_t _cst_395[115605504];
extern float32_t _cst_396[25088];
extern int32_t _cst_397[9];
extern float32_t _cst_398[25088];
extern float32_t _cst_399[25088];
extern int32_t _cst_402[1];
extern float32_t _cst_403[115605504];
extern float32_t _cst_404[25088];
extern int32_t _cst_405[9];
extern float32_t _cst_406[25088];
extern float32_t _cst_407[25088];
extern int32_t _cst_409[1];
extern float32_t _cst_410[115605504];
extern float32_t _cst_411[25088];
extern int32_t _cst_412[9];
extern float32_t _cst_413[25088];
extern float32_t _cst_414[25088];
float32_t _cst_417 = 0.02040816326530612244846;
extern float32_t _cst_418[25088];
extern float32_t _cst_419[250880];
extern int32_t _cst_421[1];
extern float32_t _cst_423[1000000];
extern float32_t _cst_424[1000];
float32_t _cst_425 = -1;
float32_t _cst_426 = -1;
float64_t _cst_427 = -3.809199432496630510769;
float64_t _cst_428 = 2.460940868770910228136;
float64_t _cst_429 = -1.430304794543664215212;
float64_t _cst_430 = 2.944661935353764636858;
float32_t _cst_431 = -1;
float32_t _cst_432 = -1;
float32_t _cst_433 = -1;
float32_t _cst_434 = -1;
float64_t _cst_435 = -0.0001600280537250875771867;
float64_t _cst_436 = 0.003032478474127557870749;
float64_t _cst_437 = 0.1365828361194337536855;
float64_t _cst_438 = -0.02219735858216702675816;
float64_t _cst_439 = 0.5685231134608953462717;
float64_t _cst_440 = -0.5558785316215136829143;
float64_t _cst_441 = 1.614771079042867985009;
float64_t _cst_442 = -0.6164083583294512891371;
float32_t _cst_443 = -1;
float32_t _cst_444 = -1;
float32_t _cst_445 = -1;
float32_t _cst_446 = -1;
float64_t _cst_447 = 0.0001111119162030077897477;
float32_t _cst_448 = 0.0001111119162030077897477;
float64_t _cst_449 = 0.006507995190210730772351;
float32_t _cst_450 = 0.006507995190210730772351;
float64_t _cst_451 = -0.0009371069270347270278171;
float32_t _cst_452 = -0.0009371069270347270278171;
float64_t _cst_453 = 0.1056744718173614761003;
float32_t _cst_454 = 0.1056744718173614761003;
float64_t _cst_455 = -0.03691063940349948585551;
float32_t _cst_456 = -0.03691063940349948585551;
float64_t _cst_457 = 0.6334139843709288841822;
float32_t _cst_458 = 0.6334139843709288841822;
float64_t _cst_459 = -0.2078597158810965850329;
float32_t _cst_460 = -0.2078597158810965850329;
float32_t _cst_461 = 0.5;
float32_t _cst_462 = 0.002500000000000000052042;
float32_t _cst_463 = 0.002500000000000000052042;
float32_t _cst_464 = 0.002500000000000000052042;
float32_t _cst_465 = 0.002500000000000000052042;
float32_t _cst_466 = 0.002500000000000000052042;
float32_t _cst_467 = 0.002500000000000000052042;
float32_t _cst_468 = 0.002500000000000000052042;
float32_t _cst_469 = 0.002500000000000000052042;
float32_t _cst_470 = 0.002500000000000000052042;
float32_t _cst_471 = 0.002500000000000000052042;
float32_t _cst_472 = 0.002500000000000000052042;
float32_t _cst_473 = 0.002500000000000000052042;
float32_t _cst_474 = 0.002500000000000000052042;
float32_t _cst_475 = 0.002500000000000000052042;
float32_t _cst_476 = 0.002500000000000000052042;
float32_t _cst_477 = 0.002500000000000000052042;
float32_t _cst_478 = 0.002500000000000000052042;
CIPHERTEXT App_relu(CIPHERTEXT input_ct0, CIPHERTEXT input_ct1, uint32_t mask_len);
CIPHERTEXT Rotate(CIPHERTEXT ciph, int32_t rot_idx);
CIPHERTEXT Relinearize(CIPHERTEXT3 ciph);

bool Main_graph() {
  CIPHERTEXT input_0[6];
  CIPHERTEXT VS_out_conv1_1[128];
  int32_t mesh_x_n1_2;
  int32_t mesh_y_n1_3;
  int32_t mesh_z_n1_4;
  CIPHERTEXT result_n21_5;
  CIPHERTEXT input_dup_n21_6;
  int32_t cap_block_input_grid_n21_7;
  CIPHERTEXT input_grid_n21_8[49];
  int32_t num_grid_n21_9;
  CIPHERTEXT result_grid_n21_10;
  int32_t cap_block_n21_11;
  CIPHERTEXT x_row21_12;
  CIPHERTEXT comb_row_result22_13;
  int32_t combine_row_index_n22_14;
  CIPHERTEXT comb_rc_result22_15;
  int32_t combine_rc_index_n22_16;
  int32_t reshardm_n1_17;
  CIPHERTEXT output_reshard1_18[32];
  int32_t add_bias_n1_19;
  CIPHERTEXT addshard1_20[32];
  CIPHERTEXT VS_out_relu1_21[32];
  int32_t mesh_x_n1_22;
  int32_t mesh_y_n1_23;
  CIPHERTEXT VS_out_avp1_24[32];
  int32_t mesh_x_n1_25;
  int32_t mesh_y_n1_26;
  CIPHERTEXT tmp_row_add22_27;
  CIPHERTEXT orig_result22_28;
  CIPHERTEXT comb_row_result23_29;
  int32_t combine_row_index_n23_30;
  CIPHERTEXT comb_rc_result23_31;
  int32_t combine_rc_index_n23_32;
  CIPHERTEXT final_result23_33;
  int32_t combine_cc_index_n23_34;
  int32_t reshardm_n1_35;
  CIPHERTEXT output_reshard1_36[8];
  CIPHERTEXT VS_out_conv2_37[8];
  int32_t mesh_x_n2_38;
  int32_t mesh_y_n2_39;
  CIPHERTEXT result_n24_40;
  CIPHERTEXT input_dup_n24_41;
  int32_t cap_block_input_grid_n24_42;
  CIPHERTEXT input_grid_n24_43[9];
  int32_t num_grid_n24_44;
  CIPHERTEXT result_grid_n24_45;
  int32_t cap_block_n24_46;
  int32_t add_bias_n2_47;
  CIPHERTEXT addshard2_48[8];
  CIPHERTEXT VS_out_relu2_49[8];
  int32_t mesh_x_n2_50;
  int32_t mesh_y_n2_51;
  CIPHERTEXT VS_out_conv3_52[8];
  int32_t mesh_x_n3_53;
  int32_t mesh_y_n3_54;
  CIPHERTEXT result_n25_55;
  CIPHERTEXT input_dup_n25_56;
  int32_t cap_block_input_grid_n25_57;
  CIPHERTEXT input_grid_n25_58[9];
  int32_t num_grid_n25_59;
  CIPHERTEXT result_grid_n25_60;
  int32_t cap_block_n25_61;
  int32_t add_bias_n3_62;
  CIPHERTEXT addshard3_63[8];
  CIPHERTEXT VS_out_add3_64[8];
  int32_t mesh_x_n3_65;
  int32_t mesh_y_n3_66;
  CIPHERTEXT VS_out_relu3_67[8];
  int32_t mesh_x_n3_68;
  int32_t mesh_y_n3_69;
  CIPHERTEXT VS_out_conv4_70[8];
  int32_t mesh_x_n4_71;
  int32_t mesh_y_n4_72;
  CIPHERTEXT result_n26_73;
  CIPHERTEXT input_dup_n26_74;
  int32_t cap_block_input_grid_n26_75;
  CIPHERTEXT input_grid_n26_76[9];
  int32_t num_grid_n26_77;
  CIPHERTEXT result_grid_n26_78;
  int32_t cap_block_n26_79;
  int32_t add_bias_n4_80;
  CIPHERTEXT addshard4_81[8];
  CIPHERTEXT VS_out_relu4_82[8];
  int32_t mesh_x_n4_83;
  int32_t mesh_y_n4_84;
  CIPHERTEXT VS_out_conv5_85[8];
  int32_t mesh_x_n5_86;
  int32_t mesh_y_n5_87;
  CIPHERTEXT result_n27_88;
  CIPHERTEXT input_dup_n27_89;
  int32_t cap_block_input_grid_n27_90;
  CIPHERTEXT input_grid_n27_91[9];
  int32_t num_grid_n27_92;
  CIPHERTEXT result_grid_n27_93;
  int32_t cap_block_n27_94;
  int32_t add_bias_n5_95;
  CIPHERTEXT addshard5_96[8];
  CIPHERTEXT VS_out_add5_97[8];
  int32_t mesh_x_n5_98;
  int32_t mesh_y_n5_99;
  CIPHERTEXT VS_out_relu5_100[8];
  int32_t mesh_x_n5_101;
  int32_t mesh_y_n5_102;
  CIPHERTEXT VS_out_conv6_103[16];
  int32_t mesh_x_n6_104;
  int32_t mesh_y_n6_105;
  CIPHERTEXT result_n28_106;
  CIPHERTEXT input_dup_n28_107;
  int32_t cap_block_input_grid_n28_108;
  CIPHERTEXT input_grid_n28_109[1];
  int32_t num_grid_n28_110;
  CIPHERTEXT result_grid_n28_111;
  int32_t cap_block_n28_112;
  CIPHERTEXT orig_result28_113;
  CIPHERTEXT comb_row_result29_114;
  int32_t combine_row_index_n29_115;
  CIPHERTEXT comb_rc_result29_116;
  int32_t combine_rc_index_n29_117;
  CIPHERTEXT final_result29_118;
  int32_t combine_cc_index_n29_119;
  int32_t reshardm_n6_120;
  CIPHERTEXT output_reshard6_121[4];
  int32_t add_bias_n6_122;
  CIPHERTEXT addshard6_123[4];
  CIPHERTEXT VS_out_conv7_124[16];
  int32_t mesh_x_n7_125;
  int32_t mesh_y_n7_126;
  CIPHERTEXT result_n30_127;
  CIPHERTEXT input_dup_n30_128;
  int32_t cap_block_input_grid_n30_129;
  CIPHERTEXT input_grid_n30_130[9];
  int32_t num_grid_n30_131;
  CIPHERTEXT result_grid_n30_132;
  int32_t cap_block_n30_133;
  CIPHERTEXT orig_result30_134;
  CIPHERTEXT comb_row_result31_135;
  int32_t combine_row_index_n31_136;
  CIPHERTEXT comb_rc_result31_137;
  int32_t combine_rc_index_n31_138;
  CIPHERTEXT final_result31_139;
  int32_t combine_cc_index_n31_140;
  int32_t reshardm_n7_141;
  CIPHERTEXT output_reshard7_142[4];
  int32_t add_bias_n7_143;
  CIPHERTEXT addshard7_144[4];
  CIPHERTEXT VS_out_relu7_145[4];
  int32_t mesh_x_n7_146;
  int32_t mesh_y_n7_147;
  CIPHERTEXT VS_out_conv8_148[4];
  int32_t mesh_x_n8_149;
  int32_t mesh_y_n8_150;
  CIPHERTEXT result_n32_151;
  CIPHERTEXT input_dup_n32_152;
  int32_t cap_block_input_grid_n32_153;
  CIPHERTEXT input_grid_n32_154[9];
  int32_t num_grid_n32_155;
  CIPHERTEXT result_grid_n32_156;
  int32_t cap_block_n32_157;
  int32_t add_bias_n8_158;
  CIPHERTEXT addshard8_159[4];
  CIPHERTEXT VS_out_add8_160[4];
  int32_t mesh_x_n8_161;
  int32_t mesh_y_n8_162;
  CIPHERTEXT VS_out_relu8_163[4];
  int32_t mesh_x_n8_164;
  int32_t mesh_y_n8_165;
  CIPHERTEXT VS_out_conv9_166[4];
  int32_t mesh_x_n9_167;
  int32_t mesh_y_n9_168;
  CIPHERTEXT result_n33_169;
  CIPHERTEXT input_dup_n33_170;
  int32_t cap_block_input_grid_n33_171;
  CIPHERTEXT input_grid_n33_172[9];
  int32_t num_grid_n33_173;
  CIPHERTEXT result_grid_n33_174;
  int32_t cap_block_n33_175;
  int32_t add_bias_n9_176;
  CIPHERTEXT addshard9_177[4];
  CIPHERTEXT VS_out_relu9_178[4];
  int32_t mesh_x_n9_179;
  int32_t mesh_y_n9_180;
  CIPHERTEXT VS_out_conv10_181[4];
  int32_t mesh_x_n10_182;
  int32_t mesh_y_n10_183;
  CIPHERTEXT result_n34_184;
  CIPHERTEXT input_dup_n34_185;
  int32_t cap_block_input_grid_n34_186;
  CIPHERTEXT input_grid_n34_187[9];
  int32_t num_grid_n34_188;
  CIPHERTEXT result_grid_n34_189;
  int32_t cap_block_n34_190;
  int32_t add_bias_n10_191;
  CIPHERTEXT addshard10_192[4];
  CIPHERTEXT VS_out_add10_193[4];
  int32_t mesh_x_n10_194;
  int32_t mesh_y_n10_195;
  CIPHERTEXT VS_out_relu10_196[4];
  int32_t mesh_x_n10_197;
  int32_t mesh_y_n10_198;
  CIPHERTEXT VS_out_conv11_199[8];
  int32_t mesh_x_n11_200;
  int32_t mesh_y_n11_201;
  CIPHERTEXT result_n35_202;
  CIPHERTEXT input_dup_n35_203;
  int32_t cap_block_input_grid_n35_204;
  CIPHERTEXT input_grid_n35_205[4];
  int32_t num_grid_n35_206;
  CIPHERTEXT result_grid_n35_207;
  int32_t cap_block_n35_208;
  CIPHERTEXT orig_result35_209;
  CIPHERTEXT comb_row_result36_210;
  int32_t combine_row_index_n36_211;
  CIPHERTEXT comb_rc_result36_212;
  int32_t combine_rc_index_n36_213;
  CIPHERTEXT final_result36_214;
  int32_t combine_cc_index_n36_215;
  int32_t reshardm_n11_216;
  CIPHERTEXT output_reshard11_217[2];
  int32_t add_bias_n11_218;
  CIPHERTEXT addshard11_219[2];
  CIPHERTEXT VS_out_conv12_220[8];
  int32_t mesh_x_n12_221;
  int32_t mesh_y_n12_222;
  CIPHERTEXT result_n37_223;
  CIPHERTEXT input_dup_n37_224;
  int32_t cap_block_input_grid_n37_225;
  CIPHERTEXT input_grid_n37_226[9];
  int32_t num_grid_n37_227;
  CIPHERTEXT result_grid_n37_228;
  int32_t cap_block_n37_229;
  CIPHERTEXT orig_result37_230;
  CIPHERTEXT comb_row_result38_231;
  int32_t combine_row_index_n38_232;
  CIPHERTEXT comb_rc_result38_233;
  int32_t combine_rc_index_n38_234;
  CIPHERTEXT final_result38_235;
  int32_t combine_cc_index_n38_236;
  int32_t reshardm_n12_237;
  CIPHERTEXT output_reshard12_238[2];
  int32_t add_bias_n12_239;
  CIPHERTEXT addshard12_240[2];
  CIPHERTEXT VS_out_relu12_241[2];
  int32_t mesh_x_n12_242;
  int32_t mesh_y_n12_243;
  CIPHERTEXT VS_out_conv13_244[2];
  int32_t mesh_x_n13_245;
  int32_t mesh_y_n13_246;
  CIPHERTEXT result_n39_247;
  CIPHERTEXT input_dup_n39_248;
  int32_t cap_block_input_grid_n39_249;
  CIPHERTEXT input_grid_n39_250[9];
  int32_t num_grid_n39_251;
  CIPHERTEXT result_grid_n39_252;
  int32_t cap_block_n39_253;
  int32_t add_bias_n13_254;
  CIPHERTEXT addshard13_255[2];
  CIPHERTEXT VS_out_add13_256[2];
  int32_t mesh_x_n13_257;
  int32_t mesh_y_n13_258;
  CIPHERTEXT VS_out_relu13_259[2];
  int32_t mesh_x_n13_260;
  int32_t mesh_y_n13_261;
  CIPHERTEXT VS_out_conv14_262[2];
  int32_t mesh_x_n14_263;
  int32_t mesh_y_n14_264;
  CIPHERTEXT result_n40_265;
  CIPHERTEXT input_dup_n40_266;
  int32_t cap_block_input_grid_n40_267;
  CIPHERTEXT input_grid_n40_268[9];
  int32_t num_grid_n40_269;
  CIPHERTEXT result_grid_n40_270;
  int32_t cap_block_n40_271;
  int32_t add_bias_n14_272;
  CIPHERTEXT addshard14_273[2];
  CIPHERTEXT VS_out_relu14_274[2];
  int32_t mesh_x_n14_275;
  int32_t mesh_y_n14_276;
  CIPHERTEXT VS_out_conv15_277[2];
  int32_t mesh_x_n15_278;
  int32_t mesh_y_n15_279;
  CIPHERTEXT result_n41_280;
  CIPHERTEXT input_dup_n41_281;
  int32_t cap_block_input_grid_n41_282;
  CIPHERTEXT input_grid_n41_283[9];
  int32_t num_grid_n41_284;
  CIPHERTEXT result_grid_n41_285;
  int32_t cap_block_n41_286;
  int32_t add_bias_n15_287;
  CIPHERTEXT addshard15_288[2];
  CIPHERTEXT VS_out_add15_289[2];
  int32_t mesh_x_n15_290;
  int32_t mesh_y_n15_291;
  CIPHERTEXT VS_out_relu15_292[2];
  int32_t mesh_x_n15_293;
  int32_t mesh_y_n15_294;
  CIPHERTEXT VS_out_conv16_295[4];
  int32_t mesh_x_n16_296;
  int32_t mesh_y_n16_297;
  CIPHERTEXT result_n42_298;
  CIPHERTEXT input_dup_n42_299;
  int32_t cap_block_input_grid_n42_300;
  CIPHERTEXT input_grid_n42_301[8];
  int32_t num_grid_n42_302;
  CIPHERTEXT result_grid_n42_303;
  int32_t cap_block_n42_304;
  CIPHERTEXT orig_result42_305;
  CIPHERTEXT comb_row_result43_306;
  int32_t combine_row_index_n43_307;
  CIPHERTEXT comb_rc_result43_308;
  int32_t combine_rc_index_n43_309;
  CIPHERTEXT final_result43_310;
  int32_t combine_cc_index_n43_311;
  int32_t reshard1_n16_312;
  CIPHERTEXT output_reshard16_313;
  CIPHERTEXT addshard16_314;
  CIPHERTEXT VS_out_conv17_315[4];
  int32_t mesh_x_n17_316;
  int32_t mesh_y_n17_317;
  CIPHERTEXT result_n44_318;
  CIPHERTEXT input_dup_n44_319;
  int32_t cap_block_input_grid_n44_320;
  CIPHERTEXT input_grid_n44_321[9];
  int32_t num_grid_n44_322;
  CIPHERTEXT result_grid_n44_323;
  int32_t cap_block_n44_324;
  CIPHERTEXT orig_result44_325;
  CIPHERTEXT comb_row_result45_326;
  int32_t combine_row_index_n45_327;
  CIPHERTEXT comb_rc_result45_328;
  int32_t combine_rc_index_n45_329;
  CIPHERTEXT final_result45_330;
  int32_t combine_cc_index_n45_331;
  int32_t reshard1_n17_332;
  CIPHERTEXT output_reshard17_333;
  CIPHERTEXT addshard17_334;
  CIPHERTEXT result_n46_335;
  CIPHERTEXT input_dup_n46_336;
  int32_t cap_block_input_grid_n46_337;
  CIPHERTEXT input_grid_n46_338[9];
  int32_t num_grid_n46_339;
  CIPHERTEXT result_grid_n46_340;
  int32_t cap_block_n46_341;
  CIPHERTEXT result_n47_342;
  CIPHERTEXT input_dup_n47_343;
  int32_t cap_block_input_grid_n47_344;
  CIPHERTEXT input_grid_n47_345[9];
  int32_t num_grid_n47_346;
  CIPHERTEXT result_grid_n47_347;
  int32_t cap_block_n47_348;
  CIPHERTEXT result_n48_349;
  CIPHERTEXT input_dup_n48_350;
  int32_t cap_block_input_grid_n48_351;
  CIPHERTEXT input_grid_n48_352[9];
  int32_t num_grid_n48_353;
  CIPHERTEXT result_grid_n48_354;
  int32_t cap_block_n48_355;
  int32_t gap_sum_index_n48_356;
  CIPHERTEXT gap_result48_357;
  CIPHERTEXT orig_result48_358;
  CIPHERTEXT final_result49_359;
  int32_t combine_cc_lf_index_n49_360;
  int32_t combine_cc_index_n49_361;
  CIPHERTEXT tmp_result_n50_362;
  CIPHERTEXT input_dup_n50_363;
  int32_t index_gemm_n50_364;
  CIPHERTEXT output_365;
  CIPHERTEXT _ckks_gen_tmp_95_366;
  CIPHERTEXT _ckks_gen_tmp_165_367;
  CIPHERTEXT _ckks_gen_tmp_198_368;
  CIPHERTEXT _ckks_gen_tmp_307_369;
  CIPHERTEXT _ckks_gen_tmp_426_370;
  CIPHERTEXT _ckks_gen_tmp_459_371;
  CIPHERTEXT _ckks_gen_tmp_492_372;
  CIPHERTEXT _ckks_gen_tmp_696_373;
  CIPHERTEXT _ckks_gen_tmp_748_374;
  CIPHERTEXT _ckks_gen_tmp_797_375;
  CIPHERTEXT _ckks_gen_tmp_950_376;
  CIPHERTEXT _ckks_gen_tmp_1002_377;
  CIPHERTEXT _ckks_gen_tmp_1051_378;
  CIPHERTEXT _ckks_gen_tmp_1264_379;
  CIPHERTEXT _ckks_gen_tmp_1316_380;
  CIPHERTEXT _ckks_gen_tmp_1365_381;
  CIPHERTEXT _ckks_gen_tmp_1518_382;
  CIPHERTEXT _ckks_gen_tmp_1570_383;
  CIPHERTEXT _ckks_gen_tmp_1619_384;
  CIPHERTEXT _ckks_gen_tmp_1832_385;
  CIPHERTEXT _ckks_gen_tmp_1884_386;
  CIPHERTEXT _ckks_gen_tmp_1926_387;
  CIPHERTEXT _ckks_gen_tmp_1959_388;
  CIPHERTEXT _ckks_gen_tmp_1992_389;
  CIPHERTEXT _ckks_gen_tmp_2093_390;
  CIPHERTEXT _ckks_gen_tmp_2188_391;
  CIPHERTEXT _ckks_gen_tmp_2240_392;
  CIPHERTEXT _ckks_gen_tmp_2282_393;
  CIPHERTEXT _ckks_gen_tmp_2315_394;
  CIPHERTEXT _ckks_gen_tmp_2348_395;
  CIPHERTEXT _ckks_gen_tmp_2449_396;
  CIPHERTEXT _ckks_gen_tmp_2602_397;
  CIPHERTEXT _ckks_gen_tmp_2654_398;
  CIPHERTEXT _ckks_gen_tmp_2703_399;
  CIPHERTEXT _ckks_gen_tmp_2916_400;
  CIPHERTEXT _ckks_gen_tmp_2968_401;
  CIPHERTEXT _ckks_gen_tmp_3017_402;
  CIPHERTEXT _ckks_gen_tmp_3170_403;
  CIPHERTEXT _ckks_gen_tmp_3222_404;
  CIPHERTEXT _ckks_gen_tmp_3271_405;
  CIPHERTEXT _ckks_gen_tmp_3488_406;
  CIPHERTEXT _ckks_gen_tmp_3540_407;
  CIPHERTEXT _ckks_gen_tmp_3582_408;
  CIPHERTEXT _ckks_gen_tmp_3615_409;
  CIPHERTEXT _ckks_gen_tmp_3648_410;
  CIPHERTEXT _ckks_gen_tmp_3749_411;
  CIPHERTEXT _ckks_gen_tmp_3844_412;
  CIPHERTEXT _ckks_gen_tmp_3896_413;
  CIPHERTEXT _ckks_gen_tmp_3938_414;
  CIPHERTEXT _ckks_gen_tmp_3971_415;
  CIPHERTEXT _ckks_gen_tmp_4004_416;
  CIPHERTEXT _ckks_gen_tmp_4105_417;
  CIPHERTEXT _ckks_gen_tmp_4258_418;
  CIPHERTEXT _ckks_gen_tmp_4310_419;
  CIPHERTEXT _ckks_gen_tmp_4359_420;
  CIPHERTEXT _ckks_gen_tmp_4572_421;
  CIPHERTEXT _ckks_gen_tmp_4624_422;
  CIPHERTEXT _ckks_gen_tmp_4673_423;
  CIPHERTEXT _ckks_gen_tmp_4826_424;
  CIPHERTEXT _ckks_gen_tmp_4878_425;
  CIPHERTEXT _ckks_gen_tmp_4927_426;
  CIPHERTEXT _ckks_gen_tmp_5144_427;
  CIPHERTEXT _ckks_gen_tmp_5196_428;
  CIPHERTEXT _ckks_gen_tmp_5238_429;
  CIPHERTEXT _ckks_gen_tmp_5271_430;
  CIPHERTEXT _ckks_gen_tmp_5304_431;
  CIPHERTEXT _ckks_gen_tmp_5468_432;
  CIPHERTEXT _ckks_gen_tmp_5520_433;
  CIPHERTEXT _ckks_gen_tmp_5562_434;
  CIPHERTEXT _ckks_gen_tmp_5595_435;
  CIPHERTEXT _ckks_gen_tmp_5628_436;
  CIPHERTEXT _ckks_gen_tmp_5777_437;
  CIPHERTEXT _ckks_gen_tmp_5821_438;
  CIPHERTEXT _ckks_gen_tmp_5914_439;
  CIPHERTEXT _ckks_gen_tmp_5958_440;
  CIPHERTEXT _ckks_gen_tmp_6045_441;
  CIPHERTEXT _ckks_gen_tmp_6089_442;
  CIPHERTEXT _ckks_gen_tmp_6209_443;
  CIPHERTEXT _ckks_gen_tmp_6253_444;
  int32_t rescale_iv_445;
  int32_t rescale_iv_446;
  int32_t rescale_iv_447;
  int32_t rescale_iv_448;
  int32_t rescale_iv_449;
  int32_t rescale_iv_450;
  int32_t rescale_iv_451;
  int32_t rescale_iv_452;
  int32_t rescale_iv_453;
  int32_t rescale_iv_454;
  int32_t rescale_iv_455;
  int32_t rescale_iv_456;
  int32_t rescale_iv_457;
  int32_t rescale_iv_458;
  int32_t rescale_iv_459;
  int32_t rescale_iv_460;
  int32_t rescale_iv_461;
  int32_t rescale_iv_462;
  MODULUS* _pgen_modulus_463;
  uint32_t _pgen_num_q_464;
  uint32_t _pgen_rns_idx_465;
  PLAINTEXT _pgen_tmp_0_466;
  CIPHERTEXT _pgen_tmp_1_467;
  PLAINTEXT _pgen_tmp_2_468;
  CIPHERTEXT _pgen_tmp_3_469;
  PLAINTEXT _pgen_tmp_4_470;
  CIPHERTEXT _pgen_tmp_5_471;
  CIPHERTEXT _pgen_tmp_6_472;
  PLAINTEXT _pgen_tmp_7_473;
  CIPHERTEXT _pgen_tmp_8_474;
  CIPHERTEXT _pgen_tmp_9_475;
  CIPHERTEXT _pgen_tmp_10_476;
  CIPHERTEXT _pgen_tmp_11_477;
  CIPHERTEXT _pgen_tmp_12_478;
  CIPHERTEXT _pgen_tmp_13_479;
  CIPHERTEXT _pgen_tmp_14_480;
  CIPHERTEXT _pgen_tmp_15_481;
  CIPHERTEXT _pgen_tmp_16_482;
  CIPHERTEXT _pgen_tmp_17_483;
  CIPHERTEXT _pgen_tmp_18_484;
  CIPHERTEXT _pgen_tmp_19_485;
  CIPHERTEXT _pgen_tmp_20_486;
  PLAINTEXT _pgen_tmp_21_487;
  CIPHERTEXT _pgen_tmp_22_488;
  CIPHERTEXT _pgen_tmp_23_489;
  PLAINTEXT _pgen_tmp_24_490;
  CIPHERTEXT _pgen_tmp_25_491;
  CIPHERTEXT _pgen_tmp_26_492;
  CIPHERTEXT _pgen_tmp_27_493;
  PLAINTEXT _pgen_tmp_28_494;
  CIPHERTEXT _pgen_tmp_29_495;
  PLAINTEXT _pgen_tmp_30_496;
  CIPHERTEXT _pgen_tmp_31_497;
  CIPHERTEXT _pgen_tmp_32_498;
  PLAINTEXT _pgen_tmp_33_499;
  CIPHERTEXT _pgen_tmp_34_500;
  CIPHERTEXT _pgen_tmp_35_501;
  PLAINTEXT _pgen_tmp_36_502;
  CIPHERTEXT _pgen_tmp_37_503;
  CIPHERTEXT _pgen_tmp_38_504;
  CIPHERTEXT _pgen_tmp_39_505;
  CIPHERTEXT _pgen_tmp_40_506;
  CIPHERTEXT _pgen_tmp_41_507;
  CIPHERTEXT _pgen_tmp_42_508;
  CIPHERTEXT _pgen_tmp_43_509;
  CIPHERTEXT _pgen_tmp_44_510;
  CIPHERTEXT _pgen_tmp_45_511;
  CIPHERTEXT _pgen_tmp_46_512;
  CIPHERTEXT _pgen_tmp_47_513;
  PLAINTEXT _pgen_tmp_48_514;
  CIPHERTEXT _pgen_tmp_49_515;
  CIPHERTEXT _pgen_tmp_50_516;
  PLAINTEXT _pgen_tmp_51_517;
  CIPHERTEXT _pgen_tmp_52_518;
  PLAINTEXT _pgen_tmp_53_519;
  CIPHERTEXT _pgen_tmp_54_520;
  PLAINTEXT _pgen_tmp_55_521;
  CIPHERTEXT _pgen_tmp_56_522;
  CIPHERTEXT _pgen_tmp_57_523;
  CIPHERTEXT _pgen_tmp_58_524;
  CIPHERTEXT _pgen_tmp_59_525;
  CIPHERTEXT _pgen_tmp_60_526;
  PLAINTEXT _pgen_tmp_61_527;
  CIPHERTEXT _pgen_tmp_62_528;
  CIPHERTEXT _pgen_tmp_63_529;
  PLAINTEXT _pgen_tmp_64_530;
  CIPHERTEXT _pgen_tmp_65_531;
  CIPHERTEXT _pgen_tmp_66_532;
  CIPHERTEXT _pgen_tmp_67_533;
  PLAINTEXT _pgen_tmp_68_534;
  CIPHERTEXT _pgen_tmp_69_535;
  CIPHERTEXT _pgen_tmp_70_536;
  PLAINTEXT _pgen_tmp_71_537;
  CIPHERTEXT _pgen_tmp_72_538;
  PLAINTEXT _pgen_tmp_73_539;
  CIPHERTEXT _pgen_tmp_74_540;
  PLAINTEXT _pgen_tmp_75_541;
  CIPHERTEXT _pgen_tmp_76_542;
  CIPHERTEXT _pgen_tmp_77_543;
  CIPHERTEXT _pgen_tmp_78_544;
  CIPHERTEXT _pgen_tmp_79_545;
  CIPHERTEXT _pgen_tmp_80_546;
  PLAINTEXT _pgen_tmp_81_547;
  CIPHERTEXT _pgen_tmp_82_548;
  CIPHERTEXT _pgen_tmp_83_549;
  CIPHERTEXT _pgen_tmp_84_550;
  CIPHERTEXT _pgen_tmp_85_551;
  CIPHERTEXT _pgen_tmp_86_552;
  CIPHERTEXT _pgen_tmp_87_553;
  CIPHERTEXT _pgen_tmp_88_554;
  PLAINTEXT _pgen_tmp_89_555;
  CIPHERTEXT _pgen_tmp_90_556;
  CIPHERTEXT _pgen_tmp_91_557;
  CIPHERTEXT _pgen_tmp_92_558;
  PLAINTEXT _pgen_tmp_93_559;
  CIPHERTEXT _pgen_tmp_94_560;
  CIPHERTEXT _pgen_tmp_95_561;
  PLAINTEXT _pgen_tmp_96_562;
  CIPHERTEXT _pgen_tmp_97_563;
  PLAINTEXT _pgen_tmp_98_564;
  CIPHERTEXT _pgen_tmp_99_565;
  PLAINTEXT _pgen_tmp_100_566;
  CIPHERTEXT _pgen_tmp_101_567;
  CIPHERTEXT _pgen_tmp_102_568;
  CIPHERTEXT _pgen_tmp_103_569;
  CIPHERTEXT _pgen_tmp_104_570;
  CIPHERTEXT _pgen_tmp_105_571;
  PLAINTEXT _pgen_tmp_106_572;
  CIPHERTEXT _pgen_tmp_107_573;
  CIPHERTEXT _pgen_tmp_108_574;
  PLAINTEXT _pgen_tmp_109_575;
  CIPHERTEXT _pgen_tmp_110_576;
  CIPHERTEXT _pgen_tmp_111_577;
  CIPHERTEXT _pgen_tmp_112_578;
  PLAINTEXT _pgen_tmp_113_579;
  CIPHERTEXT _pgen_tmp_114_580;
  CIPHERTEXT _pgen_tmp_115_581;
  PLAINTEXT _pgen_tmp_116_582;
  CIPHERTEXT _pgen_tmp_117_583;
  PLAINTEXT _pgen_tmp_118_584;
  CIPHERTEXT _pgen_tmp_119_585;
  PLAINTEXT _pgen_tmp_120_586;
  CIPHERTEXT _pgen_tmp_121_587;
  CIPHERTEXT _pgen_tmp_122_588;
  CIPHERTEXT _pgen_tmp_123_589;
  CIPHERTEXT _pgen_tmp_124_590;
  CIPHERTEXT _pgen_tmp_125_591;
  PLAINTEXT _pgen_tmp_126_592;
  CIPHERTEXT _pgen_tmp_127_593;
  CIPHERTEXT _pgen_tmp_128_594;
  CIPHERTEXT _pgen_tmp_129_595;
  CIPHERTEXT _pgen_tmp_130_596;
  CIPHERTEXT _pgen_tmp_131_597;
  CIPHERTEXT _pgen_tmp_132_598;
  CIPHERTEXT _pgen_tmp_133_599;
  PLAINTEXT _pgen_tmp_134_600;
  CIPHERTEXT _pgen_tmp_135_601;
  CIPHERTEXT _pgen_tmp_136_602;
  CIPHERTEXT _pgen_tmp_137_603;
  PLAINTEXT _pgen_tmp_138_604;
  CIPHERTEXT _pgen_tmp_139_605;
  CIPHERTEXT _pgen_tmp_140_606;
  PLAINTEXT _pgen_tmp_141_607;
  CIPHERTEXT _pgen_tmp_142_608;
  PLAINTEXT _pgen_tmp_143_609;
  CIPHERTEXT _pgen_tmp_144_610;
  PLAINTEXT _pgen_tmp_145_611;
  CIPHERTEXT _pgen_tmp_146_612;
  PLAINTEXT _pgen_tmp_147_613;
  CIPHERTEXT _pgen_tmp_148_614;
  CIPHERTEXT _pgen_tmp_149_615;
  PLAINTEXT _pgen_tmp_150_616;
  CIPHERTEXT _pgen_tmp_151_617;
  CIPHERTEXT _pgen_tmp_152_618;
  PLAINTEXT _pgen_tmp_153_619;
  CIPHERTEXT _pgen_tmp_154_620;
  CIPHERTEXT _pgen_tmp_155_621;
  CIPHERTEXT _pgen_tmp_156_622;
  CIPHERTEXT _pgen_tmp_157_623;
  CIPHERTEXT _pgen_tmp_158_624;
  CIPHERTEXT _pgen_tmp_159_625;
  CIPHERTEXT _pgen_tmp_160_626;
  CIPHERTEXT _pgen_tmp_161_627;
  CIPHERTEXT _pgen_tmp_162_628;
  CIPHERTEXT _pgen_tmp_163_629;
  CIPHERTEXT _pgen_tmp_164_630;
  CIPHERTEXT _pgen_tmp_165_631;
  CIPHERTEXT _pgen_tmp_166_632;
  PLAINTEXT _pgen_tmp_167_633;
  PLAINTEXT _pgen_tmp_168_634;
  CIPHERTEXT _pgen_tmp_169_635;
  CIPHERTEXT _pgen_tmp_170_636;
  PLAINTEXT _pgen_tmp_171_637;
  CIPHERTEXT _pgen_tmp_172_638;
  PLAINTEXT _pgen_tmp_173_639;
  CIPHERTEXT _pgen_tmp_174_640;
  PLAINTEXT _pgen_tmp_175_641;
  CIPHERTEXT _pgen_tmp_176_642;
  PLAINTEXT _pgen_tmp_177_643;
  CIPHERTEXT _pgen_tmp_178_644;
  CIPHERTEXT _pgen_tmp_179_645;
  PLAINTEXT _pgen_tmp_180_646;
  CIPHERTEXT _pgen_tmp_181_647;
  CIPHERTEXT _pgen_tmp_182_648;
  PLAINTEXT _pgen_tmp_183_649;
  CIPHERTEXT _pgen_tmp_184_650;
  CIPHERTEXT _pgen_tmp_185_651;
  CIPHERTEXT _pgen_tmp_186_652;
  CIPHERTEXT _pgen_tmp_187_653;
  CIPHERTEXT _pgen_tmp_188_654;
  CIPHERTEXT _pgen_tmp_189_655;
  CIPHERTEXT _pgen_tmp_190_656;
  CIPHERTEXT _pgen_tmp_191_657;
  CIPHERTEXT _pgen_tmp_192_658;
  CIPHERTEXT _pgen_tmp_193_659;
  CIPHERTEXT _pgen_tmp_194_660;
  CIPHERTEXT _pgen_tmp_195_661;
  CIPHERTEXT _pgen_tmp_196_662;
  PLAINTEXT _pgen_tmp_197_663;
  CIPHERTEXT _pgen_tmp_198_664;
  CIPHERTEXT _pgen_tmp_199_665;
  PLAINTEXT _pgen_tmp_200_666;
  CIPHERTEXT _pgen_tmp_201_667;
  CIPHERTEXT _pgen_tmp_202_668;
  CIPHERTEXT _pgen_tmp_203_669;
  PLAINTEXT _pgen_tmp_204_670;
  CIPHERTEXT _pgen_tmp_205_671;
  CIPHERTEXT _pgen_tmp_206_672;
  PLAINTEXT _pgen_tmp_207_673;
  CIPHERTEXT _pgen_tmp_208_674;
  PLAINTEXT _pgen_tmp_209_675;
  CIPHERTEXT _pgen_tmp_210_676;
  PLAINTEXT _pgen_tmp_211_677;
  CIPHERTEXT _pgen_tmp_212_678;
  CIPHERTEXT _pgen_tmp_213_679;
  CIPHERTEXT _pgen_tmp_214_680;
  CIPHERTEXT _pgen_tmp_215_681;
  CIPHERTEXT _pgen_tmp_216_682;
  PLAINTEXT _pgen_tmp_217_683;
  CIPHERTEXT _pgen_tmp_218_684;
  CIPHERTEXT _pgen_tmp_219_685;
  CIPHERTEXT _pgen_tmp_220_686;
  CIPHERTEXT _pgen_tmp_221_687;
  CIPHERTEXT _pgen_tmp_222_688;
  CIPHERTEXT _pgen_tmp_223_689;
  CIPHERTEXT _pgen_tmp_224_690;
  PLAINTEXT _pgen_tmp_225_691;
  CIPHERTEXT _pgen_tmp_226_692;
  CIPHERTEXT _pgen_tmp_227_693;
  CIPHERTEXT _pgen_tmp_228_694;
  PLAINTEXT _pgen_tmp_229_695;
  CIPHERTEXT _pgen_tmp_230_696;
  CIPHERTEXT _pgen_tmp_231_697;
  PLAINTEXT _pgen_tmp_232_698;
  CIPHERTEXT _pgen_tmp_233_699;
  PLAINTEXT _pgen_tmp_234_700;
  CIPHERTEXT _pgen_tmp_235_701;
  PLAINTEXT _pgen_tmp_236_702;
  CIPHERTEXT _pgen_tmp_237_703;
  CIPHERTEXT _pgen_tmp_238_704;
  CIPHERTEXT _pgen_tmp_239_705;
  CIPHERTEXT _pgen_tmp_240_706;
  CIPHERTEXT _pgen_tmp_241_707;
  PLAINTEXT _pgen_tmp_242_708;
  CIPHERTEXT _pgen_tmp_243_709;
  CIPHERTEXT _pgen_tmp_244_710;
  PLAINTEXT _pgen_tmp_245_711;
  CIPHERTEXT _pgen_tmp_246_712;
  CIPHERTEXT _pgen_tmp_247_713;
  CIPHERTEXT _pgen_tmp_248_714;
  PLAINTEXT _pgen_tmp_249_715;
  CIPHERTEXT _pgen_tmp_250_716;
  CIPHERTEXT _pgen_tmp_251_717;
  PLAINTEXT _pgen_tmp_252_718;
  CIPHERTEXT _pgen_tmp_253_719;
  PLAINTEXT _pgen_tmp_254_720;
  CIPHERTEXT _pgen_tmp_255_721;
  PLAINTEXT _pgen_tmp_256_722;
  CIPHERTEXT _pgen_tmp_257_723;
  CIPHERTEXT _pgen_tmp_258_724;
  CIPHERTEXT _pgen_tmp_259_725;
  CIPHERTEXT _pgen_tmp_260_726;
  CIPHERTEXT _pgen_tmp_261_727;
  PLAINTEXT _pgen_tmp_262_728;
  CIPHERTEXT _pgen_tmp_263_729;
  CIPHERTEXT _pgen_tmp_264_730;
  CIPHERTEXT _pgen_tmp_265_731;
  CIPHERTEXT _pgen_tmp_266_732;
  CIPHERTEXT _pgen_tmp_267_733;
  CIPHERTEXT _pgen_tmp_268_734;
  CIPHERTEXT _pgen_tmp_269_735;
  PLAINTEXT _pgen_tmp_270_736;
  CIPHERTEXT _pgen_tmp_271_737;
  CIPHERTEXT _pgen_tmp_272_738;
  CIPHERTEXT _pgen_tmp_273_739;
  PLAINTEXT _pgen_tmp_274_740;
  CIPHERTEXT _pgen_tmp_275_741;
  CIPHERTEXT _pgen_tmp_276_742;
  PLAINTEXT _pgen_tmp_277_743;
  CIPHERTEXT _pgen_tmp_278_744;
  PLAINTEXT _pgen_tmp_279_745;
  CIPHERTEXT _pgen_tmp_280_746;
  PLAINTEXT _pgen_tmp_281_747;
  CIPHERTEXT _pgen_tmp_282_748;
  PLAINTEXT _pgen_tmp_283_749;
  CIPHERTEXT _pgen_tmp_284_750;
  CIPHERTEXT _pgen_tmp_285_751;
  PLAINTEXT _pgen_tmp_286_752;
  CIPHERTEXT _pgen_tmp_287_753;
  CIPHERTEXT _pgen_tmp_288_754;
  PLAINTEXT _pgen_tmp_289_755;
  CIPHERTEXT _pgen_tmp_290_756;
  CIPHERTEXT _pgen_tmp_291_757;
  CIPHERTEXT _pgen_tmp_292_758;
  CIPHERTEXT _pgen_tmp_293_759;
  CIPHERTEXT _pgen_tmp_294_760;
  CIPHERTEXT _pgen_tmp_295_761;
  CIPHERTEXT _pgen_tmp_296_762;
  CIPHERTEXT _pgen_tmp_297_763;
  CIPHERTEXT _pgen_tmp_298_764;
  CIPHERTEXT _pgen_tmp_299_765;
  CIPHERTEXT _pgen_tmp_300_766;
  CIPHERTEXT _pgen_tmp_301_767;
  CIPHERTEXT _pgen_tmp_302_768;
  PLAINTEXT _pgen_tmp_303_769;
  PLAINTEXT _pgen_tmp_304_770;
  CIPHERTEXT _pgen_tmp_305_771;
  CIPHERTEXT _pgen_tmp_306_772;
  PLAINTEXT _pgen_tmp_307_773;
  CIPHERTEXT _pgen_tmp_308_774;
  PLAINTEXT _pgen_tmp_309_775;
  CIPHERTEXT _pgen_tmp_310_776;
  PLAINTEXT _pgen_tmp_311_777;
  CIPHERTEXT _pgen_tmp_312_778;
  PLAINTEXT _pgen_tmp_313_779;
  CIPHERTEXT _pgen_tmp_314_780;
  CIPHERTEXT _pgen_tmp_315_781;
  PLAINTEXT _pgen_tmp_316_782;
  CIPHERTEXT _pgen_tmp_317_783;
  CIPHERTEXT _pgen_tmp_318_784;
  PLAINTEXT _pgen_tmp_319_785;
  CIPHERTEXT _pgen_tmp_320_786;
  CIPHERTEXT _pgen_tmp_321_787;
  CIPHERTEXT _pgen_tmp_322_788;
  CIPHERTEXT _pgen_tmp_323_789;
  CIPHERTEXT _pgen_tmp_324_790;
  CIPHERTEXT _pgen_tmp_325_791;
  CIPHERTEXT _pgen_tmp_326_792;
  CIPHERTEXT _pgen_tmp_327_793;
  CIPHERTEXT _pgen_tmp_328_794;
  CIPHERTEXT _pgen_tmp_329_795;
  CIPHERTEXT _pgen_tmp_330_796;
  CIPHERTEXT _pgen_tmp_331_797;
  CIPHERTEXT _pgen_tmp_332_798;
  PLAINTEXT _pgen_tmp_333_799;
  CIPHERTEXT _pgen_tmp_334_800;
  CIPHERTEXT _pgen_tmp_335_801;
  PLAINTEXT _pgen_tmp_336_802;
  CIPHERTEXT _pgen_tmp_337_803;
  CIPHERTEXT _pgen_tmp_338_804;
  CIPHERTEXT _pgen_tmp_339_805;
  PLAINTEXT _pgen_tmp_340_806;
  CIPHERTEXT _pgen_tmp_341_807;
  CIPHERTEXT _pgen_tmp_342_808;
  PLAINTEXT _pgen_tmp_343_809;
  CIPHERTEXT _pgen_tmp_344_810;
  PLAINTEXT _pgen_tmp_345_811;
  CIPHERTEXT _pgen_tmp_346_812;
  PLAINTEXT _pgen_tmp_347_813;
  CIPHERTEXT _pgen_tmp_348_814;
  CIPHERTEXT _pgen_tmp_349_815;
  CIPHERTEXT _pgen_tmp_350_816;
  CIPHERTEXT _pgen_tmp_351_817;
  CIPHERTEXT _pgen_tmp_352_818;
  PLAINTEXT _pgen_tmp_353_819;
  CIPHERTEXT _pgen_tmp_354_820;
  CIPHERTEXT _pgen_tmp_355_821;
  CIPHERTEXT _pgen_tmp_356_822;
  CIPHERTEXT _pgen_tmp_357_823;
  CIPHERTEXT _pgen_tmp_358_824;
  CIPHERTEXT _pgen_tmp_359_825;
  CIPHERTEXT _pgen_tmp_360_826;
  PLAINTEXT _pgen_tmp_361_827;
  CIPHERTEXT _pgen_tmp_362_828;
  CIPHERTEXT _pgen_tmp_363_829;
  CIPHERTEXT _pgen_tmp_364_830;
  PLAINTEXT _pgen_tmp_365_831;
  CIPHERTEXT _pgen_tmp_366_832;
  CIPHERTEXT _pgen_tmp_367_833;
  PLAINTEXT _pgen_tmp_368_834;
  CIPHERTEXT _pgen_tmp_369_835;
  PLAINTEXT _pgen_tmp_370_836;
  CIPHERTEXT _pgen_tmp_371_837;
  PLAINTEXT _pgen_tmp_372_838;
  CIPHERTEXT _pgen_tmp_373_839;
  CIPHERTEXT _pgen_tmp_374_840;
  CIPHERTEXT _pgen_tmp_375_841;
  CIPHERTEXT _pgen_tmp_376_842;
  CIPHERTEXT _pgen_tmp_377_843;
  PLAINTEXT _pgen_tmp_378_844;
  CIPHERTEXT _pgen_tmp_379_845;
  CIPHERTEXT _pgen_tmp_380_846;
  PLAINTEXT _pgen_tmp_381_847;
  CIPHERTEXT _pgen_tmp_382_848;
  CIPHERTEXT _pgen_tmp_383_849;
  CIPHERTEXT _pgen_tmp_384_850;
  PLAINTEXT _pgen_tmp_385_851;
  CIPHERTEXT _pgen_tmp_386_852;
  CIPHERTEXT _pgen_tmp_387_853;
  PLAINTEXT _pgen_tmp_388_854;
  CIPHERTEXT _pgen_tmp_389_855;
  PLAINTEXT _pgen_tmp_390_856;
  CIPHERTEXT _pgen_tmp_391_857;
  PLAINTEXT _pgen_tmp_392_858;
  CIPHERTEXT _pgen_tmp_393_859;
  CIPHERTEXT _pgen_tmp_394_860;
  CIPHERTEXT _pgen_tmp_395_861;
  CIPHERTEXT _pgen_tmp_396_862;
  CIPHERTEXT _pgen_tmp_397_863;
  PLAINTEXT _pgen_tmp_398_864;
  CIPHERTEXT _pgen_tmp_399_865;
  CIPHERTEXT _pgen_tmp_400_866;
  CIPHERTEXT _pgen_tmp_401_867;
  CIPHERTEXT _pgen_tmp_402_868;
  CIPHERTEXT _pgen_tmp_403_869;
  CIPHERTEXT _pgen_tmp_404_870;
  CIPHERTEXT _pgen_tmp_405_871;
  PLAINTEXT _pgen_tmp_406_872;
  CIPHERTEXT _pgen_tmp_407_873;
  CIPHERTEXT _pgen_tmp_408_874;
  CIPHERTEXT _pgen_tmp_409_875;
  PLAINTEXT _pgen_tmp_410_876;
  CIPHERTEXT _pgen_tmp_411_877;
  CIPHERTEXT _pgen_tmp_412_878;
  PLAINTEXT _pgen_tmp_413_879;
  CIPHERTEXT _pgen_tmp_414_880;
  PLAINTEXT _pgen_tmp_415_881;
  CIPHERTEXT _pgen_tmp_416_882;
  PLAINTEXT _pgen_tmp_417_883;
  CIPHERTEXT _pgen_tmp_418_884;
  PLAINTEXT _pgen_tmp_419_885;
  CIPHERTEXT _pgen_tmp_420_886;
  CIPHERTEXT _pgen_tmp_421_887;
  PLAINTEXT _pgen_tmp_422_888;
  CIPHERTEXT _pgen_tmp_423_889;
  CIPHERTEXT _pgen_tmp_424_890;
  PLAINTEXT _pgen_tmp_425_891;
  CIPHERTEXT _pgen_tmp_426_892;
  CIPHERTEXT _pgen_tmp_427_893;
  CIPHERTEXT _pgen_tmp_428_894;
  CIPHERTEXT _pgen_tmp_429_895;
  CIPHERTEXT _pgen_tmp_430_896;
  CIPHERTEXT _pgen_tmp_431_897;
  CIPHERTEXT _pgen_tmp_432_898;
  CIPHERTEXT _pgen_tmp_433_899;
  CIPHERTEXT _pgen_tmp_434_900;
  CIPHERTEXT _pgen_tmp_435_901;
  CIPHERTEXT _pgen_tmp_436_902;
  PLAINTEXT _pgen_tmp_437_903;
  PLAINTEXT _pgen_tmp_438_904;
  CIPHERTEXT _pgen_tmp_439_905;
  CIPHERTEXT _pgen_tmp_440_906;
  PLAINTEXT _pgen_tmp_441_907;
  CIPHERTEXT _pgen_tmp_442_908;
  PLAINTEXT _pgen_tmp_443_909;
  CIPHERTEXT _pgen_tmp_444_910;
  PLAINTEXT _pgen_tmp_445_911;
  CIPHERTEXT _pgen_tmp_446_912;
  PLAINTEXT _pgen_tmp_447_913;
  CIPHERTEXT _pgen_tmp_448_914;
  CIPHERTEXT _pgen_tmp_449_915;
  PLAINTEXT _pgen_tmp_450_916;
  CIPHERTEXT _pgen_tmp_451_917;
  CIPHERTEXT _pgen_tmp_452_918;
  PLAINTEXT _pgen_tmp_453_919;
  CIPHERTEXT _pgen_tmp_454_920;
  CIPHERTEXT _pgen_tmp_455_921;
  CIPHERTEXT _pgen_tmp_456_922;
  CIPHERTEXT _pgen_tmp_457_923;
  CIPHERTEXT _pgen_tmp_458_924;
  CIPHERTEXT _pgen_tmp_459_925;
  CIPHERTEXT _pgen_tmp_460_926;
  CIPHERTEXT _pgen_tmp_461_927;
  CIPHERTEXT _pgen_tmp_462_928;
  CIPHERTEXT _pgen_tmp_463_929;
  CIPHERTEXT _pgen_tmp_464_930;
  PLAINTEXT _pgen_tmp_465_931;
  CIPHERTEXT _pgen_tmp_466_932;
  PLAINTEXT _pgen_tmp_467_933;
  CIPHERTEXT _pgen_tmp_468_934;
  PLAINTEXT _pgen_tmp_469_935;
  CIPHERTEXT _pgen_tmp_470_936;
  CIPHERTEXT _pgen_tmp_471_937;
  PLAINTEXT _pgen_tmp_472_938;
  CIPHERTEXT _pgen_tmp_473_939;
  PLAINTEXT _pgen_tmp_474_940;
  CIPHERTEXT _pgen_tmp_475_941;
  PLAINTEXT _pgen_tmp_476_942;
  CIPHERTEXT _pgen_tmp_477_943;
  PLAINTEXT _pgen_tmp_478_944;
  CIPHERTEXT _pgen_tmp_479_945;
  PLAINTEXT _pgen_tmp_480_946;
  CIPHERTEXT _pgen_tmp_481_947;
  CIPHERTEXT _pgen_tmp_482_948;
  PLAINTEXT _pgen_tmp_483_949;
  CIPHERTEXT _pgen_tmp_484_950;
  PLAINTEXT _pgen_tmp_485_951;
  CIPHERTEXT _pgen_tmp_486_952;
  PLAINTEXT _pgen_tmp_487_953;
  CIPHERTEXT _pgen_tmp_488_954;
  PLAINTEXT _pgen_tmp_489_955;
  CIPHERTEXT _pgen_tmp_490_956;
  PLAINTEXT _pgen_tmp_491_957;
  CIPHERTEXT _pgen_tmp_492_958;
  CIPHERTEXT _pgen_tmp_493_959;
  PLAINTEXT _pgen_tmp_494_960;
  CIPHERTEXT _pgen_tmp_495_961;
  PLAINTEXT _pgen_tmp_496_962;
  CIPHERTEXT _pgen_tmp_497_963;
  PLAINTEXT _pgen_tmp_498_964;
  CIPHERTEXT _pgen_tmp_499_965;
  PLAINTEXT _pgen_tmp_500_966;
  CIPHERTEXT _pgen_tmp_501_967;
  CIPHERTEXT _pgen_tmp_502_968;
  PLAINTEXT _pgen_tmp_503_969;
  CIPHERTEXT _pgen_tmp_504_970;
  PLAINTEXT _pgen_tmp_505_971;
  PLAINTEXT _pgen_tmp_506_972;
  CIPHERTEXT _pgen_tmp_507_973;
  PLAINTEXT _pgen_tmp_508_974;
  CIPHERTEXT _pgen_tmp_509_975;
  PLAINTEXT _pgen_tmp_510_976;
  CIPHERTEXT _pgen_tmp_511_977;
  PLAINTEXT _pgen_tmp_512_978;
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
  uint32_t  degree = Degree();
  for (int input_idx = 0; input_idx < 6; ++input_idx) {
    input_0[input_idx] = Get_input_data("input", input_idx);
  }
  memset(&VS_out_conv1_1, 0, sizeof(VS_out_conv1_1));
  memset(&result_n21_5, 0, sizeof(result_n21_5));
  memset(&input_dup_n21_6, 0, sizeof(input_dup_n21_6));
  memset(&input_grid_n21_8, 0, sizeof(input_grid_n21_8));
  memset(&result_grid_n21_10, 0, sizeof(result_grid_n21_10));
  memset(&x_row21_12, 0, sizeof(x_row21_12));
  memset(&comb_row_result22_13, 0, sizeof(comb_row_result22_13));
  memset(&comb_rc_result22_15, 0, sizeof(comb_rc_result22_15));
  memset(&output_reshard1_18, 0, sizeof(output_reshard1_18));
  memset(&addshard1_20, 0, sizeof(addshard1_20));
  memset(&VS_out_relu1_21, 0, sizeof(VS_out_relu1_21));
  memset(&VS_out_avp1_24, 0, sizeof(VS_out_avp1_24));
  memset(&tmp_row_add22_27, 0, sizeof(tmp_row_add22_27));
  memset(&orig_result22_28, 0, sizeof(orig_result22_28));
  memset(&comb_row_result23_29, 0, sizeof(comb_row_result23_29));
  memset(&comb_rc_result23_31, 0, sizeof(comb_rc_result23_31));
  memset(&final_result23_33, 0, sizeof(final_result23_33));
  memset(&output_reshard1_36, 0, sizeof(output_reshard1_36));
  memset(&VS_out_conv2_37, 0, sizeof(VS_out_conv2_37));
  memset(&result_n24_40, 0, sizeof(result_n24_40));
  memset(&input_dup_n24_41, 0, sizeof(input_dup_n24_41));
  memset(&input_grid_n24_43, 0, sizeof(input_grid_n24_43));
  memset(&result_grid_n24_45, 0, sizeof(result_grid_n24_45));
  memset(&addshard2_48, 0, sizeof(addshard2_48));
  memset(&VS_out_relu2_49, 0, sizeof(VS_out_relu2_49));
  memset(&VS_out_conv3_52, 0, sizeof(VS_out_conv3_52));
  memset(&result_n25_55, 0, sizeof(result_n25_55));
  memset(&input_dup_n25_56, 0, sizeof(input_dup_n25_56));
  memset(&input_grid_n25_58, 0, sizeof(input_grid_n25_58));
  memset(&result_grid_n25_60, 0, sizeof(result_grid_n25_60));
  memset(&addshard3_63, 0, sizeof(addshard3_63));
  memset(&VS_out_add3_64, 0, sizeof(VS_out_add3_64));
  memset(&VS_out_relu3_67, 0, sizeof(VS_out_relu3_67));
  memset(&VS_out_conv4_70, 0, sizeof(VS_out_conv4_70));
  memset(&result_n26_73, 0, sizeof(result_n26_73));
  memset(&input_dup_n26_74, 0, sizeof(input_dup_n26_74));
  memset(&input_grid_n26_76, 0, sizeof(input_grid_n26_76));
  memset(&result_grid_n26_78, 0, sizeof(result_grid_n26_78));
  memset(&addshard4_81, 0, sizeof(addshard4_81));
  memset(&VS_out_relu4_82, 0, sizeof(VS_out_relu4_82));
  memset(&VS_out_conv5_85, 0, sizeof(VS_out_conv5_85));
  memset(&result_n27_88, 0, sizeof(result_n27_88));
  memset(&input_dup_n27_89, 0, sizeof(input_dup_n27_89));
  memset(&input_grid_n27_91, 0, sizeof(input_grid_n27_91));
  memset(&result_grid_n27_93, 0, sizeof(result_grid_n27_93));
  memset(&addshard5_96, 0, sizeof(addshard5_96));
  memset(&VS_out_add5_97, 0, sizeof(VS_out_add5_97));
  memset(&VS_out_relu5_100, 0, sizeof(VS_out_relu5_100));
  memset(&VS_out_conv6_103, 0, sizeof(VS_out_conv6_103));
  memset(&result_n28_106, 0, sizeof(result_n28_106));
  memset(&input_dup_n28_107, 0, sizeof(input_dup_n28_107));
  memset(&input_grid_n28_109, 0, sizeof(input_grid_n28_109));
  memset(&result_grid_n28_111, 0, sizeof(result_grid_n28_111));
  memset(&orig_result28_113, 0, sizeof(orig_result28_113));
  memset(&comb_row_result29_114, 0, sizeof(comb_row_result29_114));
  memset(&comb_rc_result29_116, 0, sizeof(comb_rc_result29_116));
  memset(&final_result29_118, 0, sizeof(final_result29_118));
  memset(&output_reshard6_121, 0, sizeof(output_reshard6_121));
  memset(&addshard6_123, 0, sizeof(addshard6_123));
  memset(&VS_out_conv7_124, 0, sizeof(VS_out_conv7_124));
  memset(&result_n30_127, 0, sizeof(result_n30_127));
  memset(&input_dup_n30_128, 0, sizeof(input_dup_n30_128));
  memset(&input_grid_n30_130, 0, sizeof(input_grid_n30_130));
  memset(&result_grid_n30_132, 0, sizeof(result_grid_n30_132));
  memset(&orig_result30_134, 0, sizeof(orig_result30_134));
  memset(&comb_row_result31_135, 0, sizeof(comb_row_result31_135));
  memset(&comb_rc_result31_137, 0, sizeof(comb_rc_result31_137));
  memset(&final_result31_139, 0, sizeof(final_result31_139));
  memset(&output_reshard7_142, 0, sizeof(output_reshard7_142));
  memset(&addshard7_144, 0, sizeof(addshard7_144));
  memset(&VS_out_relu7_145, 0, sizeof(VS_out_relu7_145));
  memset(&VS_out_conv8_148, 0, sizeof(VS_out_conv8_148));
  memset(&result_n32_151, 0, sizeof(result_n32_151));
  memset(&input_dup_n32_152, 0, sizeof(input_dup_n32_152));
  memset(&input_grid_n32_154, 0, sizeof(input_grid_n32_154));
  memset(&result_grid_n32_156, 0, sizeof(result_grid_n32_156));
  memset(&addshard8_159, 0, sizeof(addshard8_159));
  memset(&VS_out_add8_160, 0, sizeof(VS_out_add8_160));
  memset(&VS_out_relu8_163, 0, sizeof(VS_out_relu8_163));
  memset(&VS_out_conv9_166, 0, sizeof(VS_out_conv9_166));
  memset(&result_n33_169, 0, sizeof(result_n33_169));
  memset(&input_dup_n33_170, 0, sizeof(input_dup_n33_170));
  memset(&input_grid_n33_172, 0, sizeof(input_grid_n33_172));
  memset(&result_grid_n33_174, 0, sizeof(result_grid_n33_174));
  memset(&addshard9_177, 0, sizeof(addshard9_177));
  memset(&VS_out_relu9_178, 0, sizeof(VS_out_relu9_178));
  memset(&VS_out_conv10_181, 0, sizeof(VS_out_conv10_181));
  memset(&result_n34_184, 0, sizeof(result_n34_184));
  memset(&input_dup_n34_185, 0, sizeof(input_dup_n34_185));
  memset(&input_grid_n34_187, 0, sizeof(input_grid_n34_187));
  memset(&result_grid_n34_189, 0, sizeof(result_grid_n34_189));
  memset(&addshard10_192, 0, sizeof(addshard10_192));
  memset(&VS_out_add10_193, 0, sizeof(VS_out_add10_193));
  memset(&VS_out_relu10_196, 0, sizeof(VS_out_relu10_196));
  memset(&VS_out_conv11_199, 0, sizeof(VS_out_conv11_199));
  memset(&result_n35_202, 0, sizeof(result_n35_202));
  memset(&input_dup_n35_203, 0, sizeof(input_dup_n35_203));
  memset(&input_grid_n35_205, 0, sizeof(input_grid_n35_205));
  memset(&result_grid_n35_207, 0, sizeof(result_grid_n35_207));
  memset(&orig_result35_209, 0, sizeof(orig_result35_209));
  memset(&comb_row_result36_210, 0, sizeof(comb_row_result36_210));
  memset(&comb_rc_result36_212, 0, sizeof(comb_rc_result36_212));
  memset(&final_result36_214, 0, sizeof(final_result36_214));
  memset(&output_reshard11_217, 0, sizeof(output_reshard11_217));
  memset(&addshard11_219, 0, sizeof(addshard11_219));
  memset(&VS_out_conv12_220, 0, sizeof(VS_out_conv12_220));
  memset(&result_n37_223, 0, sizeof(result_n37_223));
  memset(&input_dup_n37_224, 0, sizeof(input_dup_n37_224));
  memset(&input_grid_n37_226, 0, sizeof(input_grid_n37_226));
  memset(&result_grid_n37_228, 0, sizeof(result_grid_n37_228));
  memset(&orig_result37_230, 0, sizeof(orig_result37_230));
  memset(&comb_row_result38_231, 0, sizeof(comb_row_result38_231));
  memset(&comb_rc_result38_233, 0, sizeof(comb_rc_result38_233));
  memset(&final_result38_235, 0, sizeof(final_result38_235));
  memset(&output_reshard12_238, 0, sizeof(output_reshard12_238));
  memset(&addshard12_240, 0, sizeof(addshard12_240));
  memset(&VS_out_relu12_241, 0, sizeof(VS_out_relu12_241));
  memset(&VS_out_conv13_244, 0, sizeof(VS_out_conv13_244));
  memset(&result_n39_247, 0, sizeof(result_n39_247));
  memset(&input_dup_n39_248, 0, sizeof(input_dup_n39_248));
  memset(&input_grid_n39_250, 0, sizeof(input_grid_n39_250));
  memset(&result_grid_n39_252, 0, sizeof(result_grid_n39_252));
  memset(&addshard13_255, 0, sizeof(addshard13_255));
  memset(&VS_out_add13_256, 0, sizeof(VS_out_add13_256));
  memset(&VS_out_relu13_259, 0, sizeof(VS_out_relu13_259));
  memset(&VS_out_conv14_262, 0, sizeof(VS_out_conv14_262));
  memset(&result_n40_265, 0, sizeof(result_n40_265));
  memset(&input_dup_n40_266, 0, sizeof(input_dup_n40_266));
  memset(&input_grid_n40_268, 0, sizeof(input_grid_n40_268));
  memset(&result_grid_n40_270, 0, sizeof(result_grid_n40_270));
  memset(&addshard14_273, 0, sizeof(addshard14_273));
  memset(&VS_out_relu14_274, 0, sizeof(VS_out_relu14_274));
  memset(&VS_out_conv15_277, 0, sizeof(VS_out_conv15_277));
  memset(&result_n41_280, 0, sizeof(result_n41_280));
  memset(&input_dup_n41_281, 0, sizeof(input_dup_n41_281));
  memset(&input_grid_n41_283, 0, sizeof(input_grid_n41_283));
  memset(&result_grid_n41_285, 0, sizeof(result_grid_n41_285));
  memset(&addshard15_288, 0, sizeof(addshard15_288));
  memset(&VS_out_add15_289, 0, sizeof(VS_out_add15_289));
  memset(&VS_out_relu15_292, 0, sizeof(VS_out_relu15_292));
  memset(&VS_out_conv16_295, 0, sizeof(VS_out_conv16_295));
  memset(&result_n42_298, 0, sizeof(result_n42_298));
  memset(&input_dup_n42_299, 0, sizeof(input_dup_n42_299));
  memset(&input_grid_n42_301, 0, sizeof(input_grid_n42_301));
  memset(&result_grid_n42_303, 0, sizeof(result_grid_n42_303));
  memset(&orig_result42_305, 0, sizeof(orig_result42_305));
  memset(&comb_row_result43_306, 0, sizeof(comb_row_result43_306));
  memset(&comb_rc_result43_308, 0, sizeof(comb_rc_result43_308));
  memset(&final_result43_310, 0, sizeof(final_result43_310));
  memset(&output_reshard16_313, 0, sizeof(output_reshard16_313));
  memset(&addshard16_314, 0, sizeof(addshard16_314));
  memset(&VS_out_conv17_315, 0, sizeof(VS_out_conv17_315));
  memset(&result_n44_318, 0, sizeof(result_n44_318));
  memset(&input_dup_n44_319, 0, sizeof(input_dup_n44_319));
  memset(&input_grid_n44_321, 0, sizeof(input_grid_n44_321));
  memset(&result_grid_n44_323, 0, sizeof(result_grid_n44_323));
  memset(&orig_result44_325, 0, sizeof(orig_result44_325));
  memset(&comb_row_result45_326, 0, sizeof(comb_row_result45_326));
  memset(&comb_rc_result45_328, 0, sizeof(comb_rc_result45_328));
  memset(&final_result45_330, 0, sizeof(final_result45_330));
  memset(&output_reshard17_333, 0, sizeof(output_reshard17_333));
  memset(&addshard17_334, 0, sizeof(addshard17_334));
  memset(&result_n46_335, 0, sizeof(result_n46_335));
  memset(&input_dup_n46_336, 0, sizeof(input_dup_n46_336));
  memset(&input_grid_n46_338, 0, sizeof(input_grid_n46_338));
  memset(&result_grid_n46_340, 0, sizeof(result_grid_n46_340));
  memset(&result_n47_342, 0, sizeof(result_n47_342));
  memset(&input_dup_n47_343, 0, sizeof(input_dup_n47_343));
  memset(&input_grid_n47_345, 0, sizeof(input_grid_n47_345));
  memset(&result_grid_n47_347, 0, sizeof(result_grid_n47_347));
  memset(&result_n48_349, 0, sizeof(result_n48_349));
  memset(&input_dup_n48_350, 0, sizeof(input_dup_n48_350));
  memset(&input_grid_n48_352, 0, sizeof(input_grid_n48_352));
  memset(&result_grid_n48_354, 0, sizeof(result_grid_n48_354));
  memset(&gap_result48_357, 0, sizeof(gap_result48_357));
  memset(&orig_result48_358, 0, sizeof(orig_result48_358));
  memset(&final_result49_359, 0, sizeof(final_result49_359));
  memset(&tmp_result_n50_362, 0, sizeof(tmp_result_n50_362));
  memset(&input_dup_n50_363, 0, sizeof(input_dup_n50_363));
  memset(&output_365, 0, sizeof(output_365));
  memset(&_ckks_gen_tmp_95_366, 0, sizeof(_ckks_gen_tmp_95_366));
  memset(&_ckks_gen_tmp_165_367, 0, sizeof(_ckks_gen_tmp_165_367));
  memset(&_ckks_gen_tmp_198_368, 0, sizeof(_ckks_gen_tmp_198_368));
  memset(&_ckks_gen_tmp_307_369, 0, sizeof(_ckks_gen_tmp_307_369));
  memset(&_ckks_gen_tmp_426_370, 0, sizeof(_ckks_gen_tmp_426_370));
  memset(&_ckks_gen_tmp_459_371, 0, sizeof(_ckks_gen_tmp_459_371));
  memset(&_ckks_gen_tmp_492_372, 0, sizeof(_ckks_gen_tmp_492_372));
  memset(&_ckks_gen_tmp_696_373, 0, sizeof(_ckks_gen_tmp_696_373));
  memset(&_ckks_gen_tmp_748_374, 0, sizeof(_ckks_gen_tmp_748_374));
  memset(&_ckks_gen_tmp_797_375, 0, sizeof(_ckks_gen_tmp_797_375));
  memset(&_ckks_gen_tmp_950_376, 0, sizeof(_ckks_gen_tmp_950_376));
  memset(&_ckks_gen_tmp_1002_377, 0, sizeof(_ckks_gen_tmp_1002_377));
  memset(&_ckks_gen_tmp_1051_378, 0, sizeof(_ckks_gen_tmp_1051_378));
  memset(&_ckks_gen_tmp_1264_379, 0, sizeof(_ckks_gen_tmp_1264_379));
  memset(&_ckks_gen_tmp_1316_380, 0, sizeof(_ckks_gen_tmp_1316_380));
  memset(&_ckks_gen_tmp_1365_381, 0, sizeof(_ckks_gen_tmp_1365_381));
  memset(&_ckks_gen_tmp_1518_382, 0, sizeof(_ckks_gen_tmp_1518_382));
  memset(&_ckks_gen_tmp_1570_383, 0, sizeof(_ckks_gen_tmp_1570_383));
  memset(&_ckks_gen_tmp_1619_384, 0, sizeof(_ckks_gen_tmp_1619_384));
  memset(&_ckks_gen_tmp_1832_385, 0, sizeof(_ckks_gen_tmp_1832_385));
  memset(&_ckks_gen_tmp_1884_386, 0, sizeof(_ckks_gen_tmp_1884_386));
  memset(&_ckks_gen_tmp_1926_387, 0, sizeof(_ckks_gen_tmp_1926_387));
  memset(&_ckks_gen_tmp_1959_388, 0, sizeof(_ckks_gen_tmp_1959_388));
  memset(&_ckks_gen_tmp_1992_389, 0, sizeof(_ckks_gen_tmp_1992_389));
  memset(&_ckks_gen_tmp_2093_390, 0, sizeof(_ckks_gen_tmp_2093_390));
  memset(&_ckks_gen_tmp_2188_391, 0, sizeof(_ckks_gen_tmp_2188_391));
  memset(&_ckks_gen_tmp_2240_392, 0, sizeof(_ckks_gen_tmp_2240_392));
  memset(&_ckks_gen_tmp_2282_393, 0, sizeof(_ckks_gen_tmp_2282_393));
  memset(&_ckks_gen_tmp_2315_394, 0, sizeof(_ckks_gen_tmp_2315_394));
  memset(&_ckks_gen_tmp_2348_395, 0, sizeof(_ckks_gen_tmp_2348_395));
  memset(&_ckks_gen_tmp_2449_396, 0, sizeof(_ckks_gen_tmp_2449_396));
  memset(&_ckks_gen_tmp_2602_397, 0, sizeof(_ckks_gen_tmp_2602_397));
  memset(&_ckks_gen_tmp_2654_398, 0, sizeof(_ckks_gen_tmp_2654_398));
  memset(&_ckks_gen_tmp_2703_399, 0, sizeof(_ckks_gen_tmp_2703_399));
  memset(&_ckks_gen_tmp_2916_400, 0, sizeof(_ckks_gen_tmp_2916_400));
  memset(&_ckks_gen_tmp_2968_401, 0, sizeof(_ckks_gen_tmp_2968_401));
  memset(&_ckks_gen_tmp_3017_402, 0, sizeof(_ckks_gen_tmp_3017_402));
  memset(&_ckks_gen_tmp_3170_403, 0, sizeof(_ckks_gen_tmp_3170_403));
  memset(&_ckks_gen_tmp_3222_404, 0, sizeof(_ckks_gen_tmp_3222_404));
  memset(&_ckks_gen_tmp_3271_405, 0, sizeof(_ckks_gen_tmp_3271_405));
  memset(&_ckks_gen_tmp_3488_406, 0, sizeof(_ckks_gen_tmp_3488_406));
  memset(&_ckks_gen_tmp_3540_407, 0, sizeof(_ckks_gen_tmp_3540_407));
  memset(&_ckks_gen_tmp_3582_408, 0, sizeof(_ckks_gen_tmp_3582_408));
  memset(&_ckks_gen_tmp_3615_409, 0, sizeof(_ckks_gen_tmp_3615_409));
  memset(&_ckks_gen_tmp_3648_410, 0, sizeof(_ckks_gen_tmp_3648_410));
  memset(&_ckks_gen_tmp_3749_411, 0, sizeof(_ckks_gen_tmp_3749_411));
  memset(&_ckks_gen_tmp_3844_412, 0, sizeof(_ckks_gen_tmp_3844_412));
  memset(&_ckks_gen_tmp_3896_413, 0, sizeof(_ckks_gen_tmp_3896_413));
  memset(&_ckks_gen_tmp_3938_414, 0, sizeof(_ckks_gen_tmp_3938_414));
  memset(&_ckks_gen_tmp_3971_415, 0, sizeof(_ckks_gen_tmp_3971_415));
  memset(&_ckks_gen_tmp_4004_416, 0, sizeof(_ckks_gen_tmp_4004_416));
  memset(&_ckks_gen_tmp_4105_417, 0, sizeof(_ckks_gen_tmp_4105_417));
  memset(&_ckks_gen_tmp_4258_418, 0, sizeof(_ckks_gen_tmp_4258_418));
  memset(&_ckks_gen_tmp_4310_419, 0, sizeof(_ckks_gen_tmp_4310_419));
  memset(&_ckks_gen_tmp_4359_420, 0, sizeof(_ckks_gen_tmp_4359_420));
  memset(&_ckks_gen_tmp_4572_421, 0, sizeof(_ckks_gen_tmp_4572_421));
  memset(&_ckks_gen_tmp_4624_422, 0, sizeof(_ckks_gen_tmp_4624_422));
  memset(&_ckks_gen_tmp_4673_423, 0, sizeof(_ckks_gen_tmp_4673_423));
  memset(&_ckks_gen_tmp_4826_424, 0, sizeof(_ckks_gen_tmp_4826_424));
  memset(&_ckks_gen_tmp_4878_425, 0, sizeof(_ckks_gen_tmp_4878_425));
  memset(&_ckks_gen_tmp_4927_426, 0, sizeof(_ckks_gen_tmp_4927_426));
  memset(&_ckks_gen_tmp_5144_427, 0, sizeof(_ckks_gen_tmp_5144_427));
  memset(&_ckks_gen_tmp_5196_428, 0, sizeof(_ckks_gen_tmp_5196_428));
  memset(&_ckks_gen_tmp_5238_429, 0, sizeof(_ckks_gen_tmp_5238_429));
  memset(&_ckks_gen_tmp_5271_430, 0, sizeof(_ckks_gen_tmp_5271_430));
  memset(&_ckks_gen_tmp_5304_431, 0, sizeof(_ckks_gen_tmp_5304_431));
  memset(&_ckks_gen_tmp_5468_432, 0, sizeof(_ckks_gen_tmp_5468_432));
  memset(&_ckks_gen_tmp_5520_433, 0, sizeof(_ckks_gen_tmp_5520_433));
  memset(&_ckks_gen_tmp_5562_434, 0, sizeof(_ckks_gen_tmp_5562_434));
  memset(&_ckks_gen_tmp_5595_435, 0, sizeof(_ckks_gen_tmp_5595_435));
  memset(&_ckks_gen_tmp_5628_436, 0, sizeof(_ckks_gen_tmp_5628_436));
  memset(&_ckks_gen_tmp_5777_437, 0, sizeof(_ckks_gen_tmp_5777_437));
  memset(&_ckks_gen_tmp_5821_438, 0, sizeof(_ckks_gen_tmp_5821_438));
  memset(&_ckks_gen_tmp_5914_439, 0, sizeof(_ckks_gen_tmp_5914_439));
  memset(&_ckks_gen_tmp_5958_440, 0, sizeof(_ckks_gen_tmp_5958_440));
  memset(&_ckks_gen_tmp_6045_441, 0, sizeof(_ckks_gen_tmp_6045_441));
  memset(&_ckks_gen_tmp_6089_442, 0, sizeof(_ckks_gen_tmp_6089_442));
  memset(&_ckks_gen_tmp_6209_443, 0, sizeof(_ckks_gen_tmp_6209_443));
  memset(&_ckks_gen_tmp_6253_444, 0, sizeof(_ckks_gen_tmp_6253_444));
  memset(&_pgen_tmp_0_466, 0, sizeof(_pgen_tmp_0_466));
  memset(&_pgen_tmp_1_467, 0, sizeof(_pgen_tmp_1_467));
  memset(&_pgen_tmp_2_468, 0, sizeof(_pgen_tmp_2_468));
  memset(&_pgen_tmp_3_469, 0, sizeof(_pgen_tmp_3_469));
  memset(&_pgen_tmp_4_470, 0, sizeof(_pgen_tmp_4_470));
  memset(&_pgen_tmp_5_471, 0, sizeof(_pgen_tmp_5_471));
  memset(&_pgen_tmp_6_472, 0, sizeof(_pgen_tmp_6_472));
  memset(&_pgen_tmp_7_473, 0, sizeof(_pgen_tmp_7_473));
  memset(&_pgen_tmp_8_474, 0, sizeof(_pgen_tmp_8_474));
  memset(&_pgen_tmp_9_475, 0, sizeof(_pgen_tmp_9_475));
  memset(&_pgen_tmp_10_476, 0, sizeof(_pgen_tmp_10_476));
  memset(&_pgen_tmp_11_477, 0, sizeof(_pgen_tmp_11_477));
  memset(&_pgen_tmp_12_478, 0, sizeof(_pgen_tmp_12_478));
  memset(&_pgen_tmp_13_479, 0, sizeof(_pgen_tmp_13_479));
  memset(&_pgen_tmp_14_480, 0, sizeof(_pgen_tmp_14_480));
  memset(&_pgen_tmp_15_481, 0, sizeof(_pgen_tmp_15_481));
  memset(&_pgen_tmp_16_482, 0, sizeof(_pgen_tmp_16_482));
  memset(&_pgen_tmp_17_483, 0, sizeof(_pgen_tmp_17_483));
  memset(&_pgen_tmp_18_484, 0, sizeof(_pgen_tmp_18_484));
  memset(&_pgen_tmp_19_485, 0, sizeof(_pgen_tmp_19_485));
  memset(&_pgen_tmp_20_486, 0, sizeof(_pgen_tmp_20_486));
  memset(&_pgen_tmp_21_487, 0, sizeof(_pgen_tmp_21_487));
  memset(&_pgen_tmp_22_488, 0, sizeof(_pgen_tmp_22_488));
  memset(&_pgen_tmp_23_489, 0, sizeof(_pgen_tmp_23_489));
  memset(&_pgen_tmp_24_490, 0, sizeof(_pgen_tmp_24_490));
  memset(&_pgen_tmp_25_491, 0, sizeof(_pgen_tmp_25_491));
  memset(&_pgen_tmp_26_492, 0, sizeof(_pgen_tmp_26_492));
  memset(&_pgen_tmp_27_493, 0, sizeof(_pgen_tmp_27_493));
  memset(&_pgen_tmp_28_494, 0, sizeof(_pgen_tmp_28_494));
  memset(&_pgen_tmp_29_495, 0, sizeof(_pgen_tmp_29_495));
  memset(&_pgen_tmp_30_496, 0, sizeof(_pgen_tmp_30_496));
  memset(&_pgen_tmp_31_497, 0, sizeof(_pgen_tmp_31_497));
  memset(&_pgen_tmp_32_498, 0, sizeof(_pgen_tmp_32_498));
  memset(&_pgen_tmp_33_499, 0, sizeof(_pgen_tmp_33_499));
  memset(&_pgen_tmp_34_500, 0, sizeof(_pgen_tmp_34_500));
  memset(&_pgen_tmp_35_501, 0, sizeof(_pgen_tmp_35_501));
  memset(&_pgen_tmp_36_502, 0, sizeof(_pgen_tmp_36_502));
  memset(&_pgen_tmp_37_503, 0, sizeof(_pgen_tmp_37_503));
  memset(&_pgen_tmp_38_504, 0, sizeof(_pgen_tmp_38_504));
  memset(&_pgen_tmp_39_505, 0, sizeof(_pgen_tmp_39_505));
  memset(&_pgen_tmp_40_506, 0, sizeof(_pgen_tmp_40_506));
  memset(&_pgen_tmp_41_507, 0, sizeof(_pgen_tmp_41_507));
  memset(&_pgen_tmp_42_508, 0, sizeof(_pgen_tmp_42_508));
  memset(&_pgen_tmp_43_509, 0, sizeof(_pgen_tmp_43_509));
  memset(&_pgen_tmp_44_510, 0, sizeof(_pgen_tmp_44_510));
  memset(&_pgen_tmp_45_511, 0, sizeof(_pgen_tmp_45_511));
  memset(&_pgen_tmp_46_512, 0, sizeof(_pgen_tmp_46_512));
  memset(&_pgen_tmp_47_513, 0, sizeof(_pgen_tmp_47_513));
  memset(&_pgen_tmp_48_514, 0, sizeof(_pgen_tmp_48_514));
  memset(&_pgen_tmp_49_515, 0, sizeof(_pgen_tmp_49_515));
  memset(&_pgen_tmp_50_516, 0, sizeof(_pgen_tmp_50_516));
  memset(&_pgen_tmp_51_517, 0, sizeof(_pgen_tmp_51_517));
  memset(&_pgen_tmp_52_518, 0, sizeof(_pgen_tmp_52_518));
  memset(&_pgen_tmp_53_519, 0, sizeof(_pgen_tmp_53_519));
  memset(&_pgen_tmp_54_520, 0, sizeof(_pgen_tmp_54_520));
  memset(&_pgen_tmp_55_521, 0, sizeof(_pgen_tmp_55_521));
  memset(&_pgen_tmp_56_522, 0, sizeof(_pgen_tmp_56_522));
  memset(&_pgen_tmp_57_523, 0, sizeof(_pgen_tmp_57_523));
  memset(&_pgen_tmp_58_524, 0, sizeof(_pgen_tmp_58_524));
  memset(&_pgen_tmp_59_525, 0, sizeof(_pgen_tmp_59_525));
  memset(&_pgen_tmp_60_526, 0, sizeof(_pgen_tmp_60_526));
  memset(&_pgen_tmp_61_527, 0, sizeof(_pgen_tmp_61_527));
  memset(&_pgen_tmp_62_528, 0, sizeof(_pgen_tmp_62_528));
  memset(&_pgen_tmp_63_529, 0, sizeof(_pgen_tmp_63_529));
  memset(&_pgen_tmp_64_530, 0, sizeof(_pgen_tmp_64_530));
  memset(&_pgen_tmp_65_531, 0, sizeof(_pgen_tmp_65_531));
  memset(&_pgen_tmp_66_532, 0, sizeof(_pgen_tmp_66_532));
  memset(&_pgen_tmp_67_533, 0, sizeof(_pgen_tmp_67_533));
  memset(&_pgen_tmp_68_534, 0, sizeof(_pgen_tmp_68_534));
  memset(&_pgen_tmp_69_535, 0, sizeof(_pgen_tmp_69_535));
  memset(&_pgen_tmp_70_536, 0, sizeof(_pgen_tmp_70_536));
  memset(&_pgen_tmp_71_537, 0, sizeof(_pgen_tmp_71_537));
  memset(&_pgen_tmp_72_538, 0, sizeof(_pgen_tmp_72_538));
  memset(&_pgen_tmp_73_539, 0, sizeof(_pgen_tmp_73_539));
  memset(&_pgen_tmp_74_540, 0, sizeof(_pgen_tmp_74_540));
  memset(&_pgen_tmp_75_541, 0, sizeof(_pgen_tmp_75_541));
  memset(&_pgen_tmp_76_542, 0, sizeof(_pgen_tmp_76_542));
  memset(&_pgen_tmp_77_543, 0, sizeof(_pgen_tmp_77_543));
  memset(&_pgen_tmp_78_544, 0, sizeof(_pgen_tmp_78_544));
  memset(&_pgen_tmp_79_545, 0, sizeof(_pgen_tmp_79_545));
  memset(&_pgen_tmp_80_546, 0, sizeof(_pgen_tmp_80_546));
  memset(&_pgen_tmp_81_547, 0, sizeof(_pgen_tmp_81_547));
  memset(&_pgen_tmp_82_548, 0, sizeof(_pgen_tmp_82_548));
  memset(&_pgen_tmp_83_549, 0, sizeof(_pgen_tmp_83_549));
  memset(&_pgen_tmp_84_550, 0, sizeof(_pgen_tmp_84_550));
  memset(&_pgen_tmp_85_551, 0, sizeof(_pgen_tmp_85_551));
  memset(&_pgen_tmp_86_552, 0, sizeof(_pgen_tmp_86_552));
  memset(&_pgen_tmp_87_553, 0, sizeof(_pgen_tmp_87_553));
  memset(&_pgen_tmp_88_554, 0, sizeof(_pgen_tmp_88_554));
  memset(&_pgen_tmp_89_555, 0, sizeof(_pgen_tmp_89_555));
  memset(&_pgen_tmp_90_556, 0, sizeof(_pgen_tmp_90_556));
  memset(&_pgen_tmp_91_557, 0, sizeof(_pgen_tmp_91_557));
  memset(&_pgen_tmp_92_558, 0, sizeof(_pgen_tmp_92_558));
  memset(&_pgen_tmp_93_559, 0, sizeof(_pgen_tmp_93_559));
  memset(&_pgen_tmp_94_560, 0, sizeof(_pgen_tmp_94_560));
  memset(&_pgen_tmp_95_561, 0, sizeof(_pgen_tmp_95_561));
  memset(&_pgen_tmp_96_562, 0, sizeof(_pgen_tmp_96_562));
  memset(&_pgen_tmp_97_563, 0, sizeof(_pgen_tmp_97_563));
  memset(&_pgen_tmp_98_564, 0, sizeof(_pgen_tmp_98_564));
  memset(&_pgen_tmp_99_565, 0, sizeof(_pgen_tmp_99_565));
  memset(&_pgen_tmp_100_566, 0, sizeof(_pgen_tmp_100_566));
  memset(&_pgen_tmp_101_567, 0, sizeof(_pgen_tmp_101_567));
  memset(&_pgen_tmp_102_568, 0, sizeof(_pgen_tmp_102_568));
  memset(&_pgen_tmp_103_569, 0, sizeof(_pgen_tmp_103_569));
  memset(&_pgen_tmp_104_570, 0, sizeof(_pgen_tmp_104_570));
  memset(&_pgen_tmp_105_571, 0, sizeof(_pgen_tmp_105_571));
  memset(&_pgen_tmp_106_572, 0, sizeof(_pgen_tmp_106_572));
  memset(&_pgen_tmp_107_573, 0, sizeof(_pgen_tmp_107_573));
  memset(&_pgen_tmp_108_574, 0, sizeof(_pgen_tmp_108_574));
  memset(&_pgen_tmp_109_575, 0, sizeof(_pgen_tmp_109_575));
  memset(&_pgen_tmp_110_576, 0, sizeof(_pgen_tmp_110_576));
  memset(&_pgen_tmp_111_577, 0, sizeof(_pgen_tmp_111_577));
  memset(&_pgen_tmp_112_578, 0, sizeof(_pgen_tmp_112_578));
  memset(&_pgen_tmp_113_579, 0, sizeof(_pgen_tmp_113_579));
  memset(&_pgen_tmp_114_580, 0, sizeof(_pgen_tmp_114_580));
  memset(&_pgen_tmp_115_581, 0, sizeof(_pgen_tmp_115_581));
  memset(&_pgen_tmp_116_582, 0, sizeof(_pgen_tmp_116_582));
  memset(&_pgen_tmp_117_583, 0, sizeof(_pgen_tmp_117_583));
  memset(&_pgen_tmp_118_584, 0, sizeof(_pgen_tmp_118_584));
  memset(&_pgen_tmp_119_585, 0, sizeof(_pgen_tmp_119_585));
  memset(&_pgen_tmp_120_586, 0, sizeof(_pgen_tmp_120_586));
  memset(&_pgen_tmp_121_587, 0, sizeof(_pgen_tmp_121_587));
  memset(&_pgen_tmp_122_588, 0, sizeof(_pgen_tmp_122_588));
  memset(&_pgen_tmp_123_589, 0, sizeof(_pgen_tmp_123_589));
  memset(&_pgen_tmp_124_590, 0, sizeof(_pgen_tmp_124_590));
  memset(&_pgen_tmp_125_591, 0, sizeof(_pgen_tmp_125_591));
  memset(&_pgen_tmp_126_592, 0, sizeof(_pgen_tmp_126_592));
  memset(&_pgen_tmp_127_593, 0, sizeof(_pgen_tmp_127_593));
  memset(&_pgen_tmp_128_594, 0, sizeof(_pgen_tmp_128_594));
  memset(&_pgen_tmp_129_595, 0, sizeof(_pgen_tmp_129_595));
  memset(&_pgen_tmp_130_596, 0, sizeof(_pgen_tmp_130_596));
  memset(&_pgen_tmp_131_597, 0, sizeof(_pgen_tmp_131_597));
  memset(&_pgen_tmp_132_598, 0, sizeof(_pgen_tmp_132_598));
  memset(&_pgen_tmp_133_599, 0, sizeof(_pgen_tmp_133_599));
  memset(&_pgen_tmp_134_600, 0, sizeof(_pgen_tmp_134_600));
  memset(&_pgen_tmp_135_601, 0, sizeof(_pgen_tmp_135_601));
  memset(&_pgen_tmp_136_602, 0, sizeof(_pgen_tmp_136_602));
  memset(&_pgen_tmp_137_603, 0, sizeof(_pgen_tmp_137_603));
  memset(&_pgen_tmp_138_604, 0, sizeof(_pgen_tmp_138_604));
  memset(&_pgen_tmp_139_605, 0, sizeof(_pgen_tmp_139_605));
  memset(&_pgen_tmp_140_606, 0, sizeof(_pgen_tmp_140_606));
  memset(&_pgen_tmp_141_607, 0, sizeof(_pgen_tmp_141_607));
  memset(&_pgen_tmp_142_608, 0, sizeof(_pgen_tmp_142_608));
  memset(&_pgen_tmp_143_609, 0, sizeof(_pgen_tmp_143_609));
  memset(&_pgen_tmp_144_610, 0, sizeof(_pgen_tmp_144_610));
  memset(&_pgen_tmp_145_611, 0, sizeof(_pgen_tmp_145_611));
  memset(&_pgen_tmp_146_612, 0, sizeof(_pgen_tmp_146_612));
  memset(&_pgen_tmp_147_613, 0, sizeof(_pgen_tmp_147_613));
  memset(&_pgen_tmp_148_614, 0, sizeof(_pgen_tmp_148_614));
  memset(&_pgen_tmp_149_615, 0, sizeof(_pgen_tmp_149_615));
  memset(&_pgen_tmp_150_616, 0, sizeof(_pgen_tmp_150_616));
  memset(&_pgen_tmp_151_617, 0, sizeof(_pgen_tmp_151_617));
  memset(&_pgen_tmp_152_618, 0, sizeof(_pgen_tmp_152_618));
  memset(&_pgen_tmp_153_619, 0, sizeof(_pgen_tmp_153_619));
  memset(&_pgen_tmp_154_620, 0, sizeof(_pgen_tmp_154_620));
  memset(&_pgen_tmp_155_621, 0, sizeof(_pgen_tmp_155_621));
  memset(&_pgen_tmp_156_622, 0, sizeof(_pgen_tmp_156_622));
  memset(&_pgen_tmp_157_623, 0, sizeof(_pgen_tmp_157_623));
  memset(&_pgen_tmp_158_624, 0, sizeof(_pgen_tmp_158_624));
  memset(&_pgen_tmp_159_625, 0, sizeof(_pgen_tmp_159_625));
  memset(&_pgen_tmp_160_626, 0, sizeof(_pgen_tmp_160_626));
  memset(&_pgen_tmp_161_627, 0, sizeof(_pgen_tmp_161_627));
  memset(&_pgen_tmp_162_628, 0, sizeof(_pgen_tmp_162_628));
  memset(&_pgen_tmp_163_629, 0, sizeof(_pgen_tmp_163_629));
  memset(&_pgen_tmp_164_630, 0, sizeof(_pgen_tmp_164_630));
  memset(&_pgen_tmp_165_631, 0, sizeof(_pgen_tmp_165_631));
  memset(&_pgen_tmp_166_632, 0, sizeof(_pgen_tmp_166_632));
  memset(&_pgen_tmp_167_633, 0, sizeof(_pgen_tmp_167_633));
  memset(&_pgen_tmp_168_634, 0, sizeof(_pgen_tmp_168_634));
  memset(&_pgen_tmp_169_635, 0, sizeof(_pgen_tmp_169_635));
  memset(&_pgen_tmp_170_636, 0, sizeof(_pgen_tmp_170_636));
  memset(&_pgen_tmp_171_637, 0, sizeof(_pgen_tmp_171_637));
  memset(&_pgen_tmp_172_638, 0, sizeof(_pgen_tmp_172_638));
  memset(&_pgen_tmp_173_639, 0, sizeof(_pgen_tmp_173_639));
  memset(&_pgen_tmp_174_640, 0, sizeof(_pgen_tmp_174_640));
  memset(&_pgen_tmp_175_641, 0, sizeof(_pgen_tmp_175_641));
  memset(&_pgen_tmp_176_642, 0, sizeof(_pgen_tmp_176_642));
  memset(&_pgen_tmp_177_643, 0, sizeof(_pgen_tmp_177_643));
  memset(&_pgen_tmp_178_644, 0, sizeof(_pgen_tmp_178_644));
  memset(&_pgen_tmp_179_645, 0, sizeof(_pgen_tmp_179_645));
  memset(&_pgen_tmp_180_646, 0, sizeof(_pgen_tmp_180_646));
  memset(&_pgen_tmp_181_647, 0, sizeof(_pgen_tmp_181_647));
  memset(&_pgen_tmp_182_648, 0, sizeof(_pgen_tmp_182_648));
  memset(&_pgen_tmp_183_649, 0, sizeof(_pgen_tmp_183_649));
  memset(&_pgen_tmp_184_650, 0, sizeof(_pgen_tmp_184_650));
  memset(&_pgen_tmp_185_651, 0, sizeof(_pgen_tmp_185_651));
  memset(&_pgen_tmp_186_652, 0, sizeof(_pgen_tmp_186_652));
  memset(&_pgen_tmp_187_653, 0, sizeof(_pgen_tmp_187_653));
  memset(&_pgen_tmp_188_654, 0, sizeof(_pgen_tmp_188_654));
  memset(&_pgen_tmp_189_655, 0, sizeof(_pgen_tmp_189_655));
  memset(&_pgen_tmp_190_656, 0, sizeof(_pgen_tmp_190_656));
  memset(&_pgen_tmp_191_657, 0, sizeof(_pgen_tmp_191_657));
  memset(&_pgen_tmp_192_658, 0, sizeof(_pgen_tmp_192_658));
  memset(&_pgen_tmp_193_659, 0, sizeof(_pgen_tmp_193_659));
  memset(&_pgen_tmp_194_660, 0, sizeof(_pgen_tmp_194_660));
  memset(&_pgen_tmp_195_661, 0, sizeof(_pgen_tmp_195_661));
  memset(&_pgen_tmp_196_662, 0, sizeof(_pgen_tmp_196_662));
  memset(&_pgen_tmp_197_663, 0, sizeof(_pgen_tmp_197_663));
  memset(&_pgen_tmp_198_664, 0, sizeof(_pgen_tmp_198_664));
  memset(&_pgen_tmp_199_665, 0, sizeof(_pgen_tmp_199_665));
  memset(&_pgen_tmp_200_666, 0, sizeof(_pgen_tmp_200_666));
  memset(&_pgen_tmp_201_667, 0, sizeof(_pgen_tmp_201_667));
  memset(&_pgen_tmp_202_668, 0, sizeof(_pgen_tmp_202_668));
  memset(&_pgen_tmp_203_669, 0, sizeof(_pgen_tmp_203_669));
  memset(&_pgen_tmp_204_670, 0, sizeof(_pgen_tmp_204_670));
  memset(&_pgen_tmp_205_671, 0, sizeof(_pgen_tmp_205_671));
  memset(&_pgen_tmp_206_672, 0, sizeof(_pgen_tmp_206_672));
  memset(&_pgen_tmp_207_673, 0, sizeof(_pgen_tmp_207_673));
  memset(&_pgen_tmp_208_674, 0, sizeof(_pgen_tmp_208_674));
  memset(&_pgen_tmp_209_675, 0, sizeof(_pgen_tmp_209_675));
  memset(&_pgen_tmp_210_676, 0, sizeof(_pgen_tmp_210_676));
  memset(&_pgen_tmp_211_677, 0, sizeof(_pgen_tmp_211_677));
  memset(&_pgen_tmp_212_678, 0, sizeof(_pgen_tmp_212_678));
  memset(&_pgen_tmp_213_679, 0, sizeof(_pgen_tmp_213_679));
  memset(&_pgen_tmp_214_680, 0, sizeof(_pgen_tmp_214_680));
  memset(&_pgen_tmp_215_681, 0, sizeof(_pgen_tmp_215_681));
  memset(&_pgen_tmp_216_682, 0, sizeof(_pgen_tmp_216_682));
  memset(&_pgen_tmp_217_683, 0, sizeof(_pgen_tmp_217_683));
  memset(&_pgen_tmp_218_684, 0, sizeof(_pgen_tmp_218_684));
  memset(&_pgen_tmp_219_685, 0, sizeof(_pgen_tmp_219_685));
  memset(&_pgen_tmp_220_686, 0, sizeof(_pgen_tmp_220_686));
  memset(&_pgen_tmp_221_687, 0, sizeof(_pgen_tmp_221_687));
  memset(&_pgen_tmp_222_688, 0, sizeof(_pgen_tmp_222_688));
  memset(&_pgen_tmp_223_689, 0, sizeof(_pgen_tmp_223_689));
  memset(&_pgen_tmp_224_690, 0, sizeof(_pgen_tmp_224_690));
  memset(&_pgen_tmp_225_691, 0, sizeof(_pgen_tmp_225_691));
  memset(&_pgen_tmp_226_692, 0, sizeof(_pgen_tmp_226_692));
  memset(&_pgen_tmp_227_693, 0, sizeof(_pgen_tmp_227_693));
  memset(&_pgen_tmp_228_694, 0, sizeof(_pgen_tmp_228_694));
  memset(&_pgen_tmp_229_695, 0, sizeof(_pgen_tmp_229_695));
  memset(&_pgen_tmp_230_696, 0, sizeof(_pgen_tmp_230_696));
  memset(&_pgen_tmp_231_697, 0, sizeof(_pgen_tmp_231_697));
  memset(&_pgen_tmp_232_698, 0, sizeof(_pgen_tmp_232_698));
  memset(&_pgen_tmp_233_699, 0, sizeof(_pgen_tmp_233_699));
  memset(&_pgen_tmp_234_700, 0, sizeof(_pgen_tmp_234_700));
  memset(&_pgen_tmp_235_701, 0, sizeof(_pgen_tmp_235_701));
  memset(&_pgen_tmp_236_702, 0, sizeof(_pgen_tmp_236_702));
  memset(&_pgen_tmp_237_703, 0, sizeof(_pgen_tmp_237_703));
  memset(&_pgen_tmp_238_704, 0, sizeof(_pgen_tmp_238_704));
  memset(&_pgen_tmp_239_705, 0, sizeof(_pgen_tmp_239_705));
  memset(&_pgen_tmp_240_706, 0, sizeof(_pgen_tmp_240_706));
  memset(&_pgen_tmp_241_707, 0, sizeof(_pgen_tmp_241_707));
  memset(&_pgen_tmp_242_708, 0, sizeof(_pgen_tmp_242_708));
  memset(&_pgen_tmp_243_709, 0, sizeof(_pgen_tmp_243_709));
  memset(&_pgen_tmp_244_710, 0, sizeof(_pgen_tmp_244_710));
  memset(&_pgen_tmp_245_711, 0, sizeof(_pgen_tmp_245_711));
  memset(&_pgen_tmp_246_712, 0, sizeof(_pgen_tmp_246_712));
  memset(&_pgen_tmp_247_713, 0, sizeof(_pgen_tmp_247_713));
  memset(&_pgen_tmp_248_714, 0, sizeof(_pgen_tmp_248_714));
  memset(&_pgen_tmp_249_715, 0, sizeof(_pgen_tmp_249_715));
  memset(&_pgen_tmp_250_716, 0, sizeof(_pgen_tmp_250_716));
  memset(&_pgen_tmp_251_717, 0, sizeof(_pgen_tmp_251_717));
  memset(&_pgen_tmp_252_718, 0, sizeof(_pgen_tmp_252_718));
  memset(&_pgen_tmp_253_719, 0, sizeof(_pgen_tmp_253_719));
  memset(&_pgen_tmp_254_720, 0, sizeof(_pgen_tmp_254_720));
  memset(&_pgen_tmp_255_721, 0, sizeof(_pgen_tmp_255_721));
  memset(&_pgen_tmp_256_722, 0, sizeof(_pgen_tmp_256_722));
  memset(&_pgen_tmp_257_723, 0, sizeof(_pgen_tmp_257_723));
  memset(&_pgen_tmp_258_724, 0, sizeof(_pgen_tmp_258_724));
  memset(&_pgen_tmp_259_725, 0, sizeof(_pgen_tmp_259_725));
  memset(&_pgen_tmp_260_726, 0, sizeof(_pgen_tmp_260_726));
  memset(&_pgen_tmp_261_727, 0, sizeof(_pgen_tmp_261_727));
  memset(&_pgen_tmp_262_728, 0, sizeof(_pgen_tmp_262_728));
  memset(&_pgen_tmp_263_729, 0, sizeof(_pgen_tmp_263_729));
  memset(&_pgen_tmp_264_730, 0, sizeof(_pgen_tmp_264_730));
  memset(&_pgen_tmp_265_731, 0, sizeof(_pgen_tmp_265_731));
  memset(&_pgen_tmp_266_732, 0, sizeof(_pgen_tmp_266_732));
  memset(&_pgen_tmp_267_733, 0, sizeof(_pgen_tmp_267_733));
  memset(&_pgen_tmp_268_734, 0, sizeof(_pgen_tmp_268_734));
  memset(&_pgen_tmp_269_735, 0, sizeof(_pgen_tmp_269_735));
  memset(&_pgen_tmp_270_736, 0, sizeof(_pgen_tmp_270_736));
  memset(&_pgen_tmp_271_737, 0, sizeof(_pgen_tmp_271_737));
  memset(&_pgen_tmp_272_738, 0, sizeof(_pgen_tmp_272_738));
  memset(&_pgen_tmp_273_739, 0, sizeof(_pgen_tmp_273_739));
  memset(&_pgen_tmp_274_740, 0, sizeof(_pgen_tmp_274_740));
  memset(&_pgen_tmp_275_741, 0, sizeof(_pgen_tmp_275_741));
  memset(&_pgen_tmp_276_742, 0, sizeof(_pgen_tmp_276_742));
  memset(&_pgen_tmp_277_743, 0, sizeof(_pgen_tmp_277_743));
  memset(&_pgen_tmp_278_744, 0, sizeof(_pgen_tmp_278_744));
  memset(&_pgen_tmp_279_745, 0, sizeof(_pgen_tmp_279_745));
  memset(&_pgen_tmp_280_746, 0, sizeof(_pgen_tmp_280_746));
  memset(&_pgen_tmp_281_747, 0, sizeof(_pgen_tmp_281_747));
  memset(&_pgen_tmp_282_748, 0, sizeof(_pgen_tmp_282_748));
  memset(&_pgen_tmp_283_749, 0, sizeof(_pgen_tmp_283_749));
  memset(&_pgen_tmp_284_750, 0, sizeof(_pgen_tmp_284_750));
  memset(&_pgen_tmp_285_751, 0, sizeof(_pgen_tmp_285_751));
  memset(&_pgen_tmp_286_752, 0, sizeof(_pgen_tmp_286_752));
  memset(&_pgen_tmp_287_753, 0, sizeof(_pgen_tmp_287_753));
  memset(&_pgen_tmp_288_754, 0, sizeof(_pgen_tmp_288_754));
  memset(&_pgen_tmp_289_755, 0, sizeof(_pgen_tmp_289_755));
  memset(&_pgen_tmp_290_756, 0, sizeof(_pgen_tmp_290_756));
  memset(&_pgen_tmp_291_757, 0, sizeof(_pgen_tmp_291_757));
  memset(&_pgen_tmp_292_758, 0, sizeof(_pgen_tmp_292_758));
  memset(&_pgen_tmp_293_759, 0, sizeof(_pgen_tmp_293_759));
  memset(&_pgen_tmp_294_760, 0, sizeof(_pgen_tmp_294_760));
  memset(&_pgen_tmp_295_761, 0, sizeof(_pgen_tmp_295_761));
  memset(&_pgen_tmp_296_762, 0, sizeof(_pgen_tmp_296_762));
  memset(&_pgen_tmp_297_763, 0, sizeof(_pgen_tmp_297_763));
  memset(&_pgen_tmp_298_764, 0, sizeof(_pgen_tmp_298_764));
  memset(&_pgen_tmp_299_765, 0, sizeof(_pgen_tmp_299_765));
  memset(&_pgen_tmp_300_766, 0, sizeof(_pgen_tmp_300_766));
  memset(&_pgen_tmp_301_767, 0, sizeof(_pgen_tmp_301_767));
  memset(&_pgen_tmp_302_768, 0, sizeof(_pgen_tmp_302_768));
  memset(&_pgen_tmp_303_769, 0, sizeof(_pgen_tmp_303_769));
  memset(&_pgen_tmp_304_770, 0, sizeof(_pgen_tmp_304_770));
  memset(&_pgen_tmp_305_771, 0, sizeof(_pgen_tmp_305_771));
  memset(&_pgen_tmp_306_772, 0, sizeof(_pgen_tmp_306_772));
  memset(&_pgen_tmp_307_773, 0, sizeof(_pgen_tmp_307_773));
  memset(&_pgen_tmp_308_774, 0, sizeof(_pgen_tmp_308_774));
  memset(&_pgen_tmp_309_775, 0, sizeof(_pgen_tmp_309_775));
  memset(&_pgen_tmp_310_776, 0, sizeof(_pgen_tmp_310_776));
  memset(&_pgen_tmp_311_777, 0, sizeof(_pgen_tmp_311_777));
  memset(&_pgen_tmp_312_778, 0, sizeof(_pgen_tmp_312_778));
  memset(&_pgen_tmp_313_779, 0, sizeof(_pgen_tmp_313_779));
  memset(&_pgen_tmp_314_780, 0, sizeof(_pgen_tmp_314_780));
  memset(&_pgen_tmp_315_781, 0, sizeof(_pgen_tmp_315_781));
  memset(&_pgen_tmp_316_782, 0, sizeof(_pgen_tmp_316_782));
  memset(&_pgen_tmp_317_783, 0, sizeof(_pgen_tmp_317_783));
  memset(&_pgen_tmp_318_784, 0, sizeof(_pgen_tmp_318_784));
  memset(&_pgen_tmp_319_785, 0, sizeof(_pgen_tmp_319_785));
  memset(&_pgen_tmp_320_786, 0, sizeof(_pgen_tmp_320_786));
  memset(&_pgen_tmp_321_787, 0, sizeof(_pgen_tmp_321_787));
  memset(&_pgen_tmp_322_788, 0, sizeof(_pgen_tmp_322_788));
  memset(&_pgen_tmp_323_789, 0, sizeof(_pgen_tmp_323_789));
  memset(&_pgen_tmp_324_790, 0, sizeof(_pgen_tmp_324_790));
  memset(&_pgen_tmp_325_791, 0, sizeof(_pgen_tmp_325_791));
  memset(&_pgen_tmp_326_792, 0, sizeof(_pgen_tmp_326_792));
  memset(&_pgen_tmp_327_793, 0, sizeof(_pgen_tmp_327_793));
  memset(&_pgen_tmp_328_794, 0, sizeof(_pgen_tmp_328_794));
  memset(&_pgen_tmp_329_795, 0, sizeof(_pgen_tmp_329_795));
  memset(&_pgen_tmp_330_796, 0, sizeof(_pgen_tmp_330_796));
  memset(&_pgen_tmp_331_797, 0, sizeof(_pgen_tmp_331_797));
  memset(&_pgen_tmp_332_798, 0, sizeof(_pgen_tmp_332_798));
  memset(&_pgen_tmp_333_799, 0, sizeof(_pgen_tmp_333_799));
  memset(&_pgen_tmp_334_800, 0, sizeof(_pgen_tmp_334_800));
  memset(&_pgen_tmp_335_801, 0, sizeof(_pgen_tmp_335_801));
  memset(&_pgen_tmp_336_802, 0, sizeof(_pgen_tmp_336_802));
  memset(&_pgen_tmp_337_803, 0, sizeof(_pgen_tmp_337_803));
  memset(&_pgen_tmp_338_804, 0, sizeof(_pgen_tmp_338_804));
  memset(&_pgen_tmp_339_805, 0, sizeof(_pgen_tmp_339_805));
  memset(&_pgen_tmp_340_806, 0, sizeof(_pgen_tmp_340_806));
  memset(&_pgen_tmp_341_807, 0, sizeof(_pgen_tmp_341_807));
  memset(&_pgen_tmp_342_808, 0, sizeof(_pgen_tmp_342_808));
  memset(&_pgen_tmp_343_809, 0, sizeof(_pgen_tmp_343_809));
  memset(&_pgen_tmp_344_810, 0, sizeof(_pgen_tmp_344_810));
  memset(&_pgen_tmp_345_811, 0, sizeof(_pgen_tmp_345_811));
  memset(&_pgen_tmp_346_812, 0, sizeof(_pgen_tmp_346_812));
  memset(&_pgen_tmp_347_813, 0, sizeof(_pgen_tmp_347_813));
  memset(&_pgen_tmp_348_814, 0, sizeof(_pgen_tmp_348_814));
  memset(&_pgen_tmp_349_815, 0, sizeof(_pgen_tmp_349_815));
  memset(&_pgen_tmp_350_816, 0, sizeof(_pgen_tmp_350_816));
  memset(&_pgen_tmp_351_817, 0, sizeof(_pgen_tmp_351_817));
  memset(&_pgen_tmp_352_818, 0, sizeof(_pgen_tmp_352_818));
  memset(&_pgen_tmp_353_819, 0, sizeof(_pgen_tmp_353_819));
  memset(&_pgen_tmp_354_820, 0, sizeof(_pgen_tmp_354_820));
  memset(&_pgen_tmp_355_821, 0, sizeof(_pgen_tmp_355_821));
  memset(&_pgen_tmp_356_822, 0, sizeof(_pgen_tmp_356_822));
  memset(&_pgen_tmp_357_823, 0, sizeof(_pgen_tmp_357_823));
  memset(&_pgen_tmp_358_824, 0, sizeof(_pgen_tmp_358_824));
  memset(&_pgen_tmp_359_825, 0, sizeof(_pgen_tmp_359_825));
  memset(&_pgen_tmp_360_826, 0, sizeof(_pgen_tmp_360_826));
  memset(&_pgen_tmp_361_827, 0, sizeof(_pgen_tmp_361_827));
  memset(&_pgen_tmp_362_828, 0, sizeof(_pgen_tmp_362_828));
  memset(&_pgen_tmp_363_829, 0, sizeof(_pgen_tmp_363_829));
  memset(&_pgen_tmp_364_830, 0, sizeof(_pgen_tmp_364_830));
  memset(&_pgen_tmp_365_831, 0, sizeof(_pgen_tmp_365_831));
  memset(&_pgen_tmp_366_832, 0, sizeof(_pgen_tmp_366_832));
  memset(&_pgen_tmp_367_833, 0, sizeof(_pgen_tmp_367_833));
  memset(&_pgen_tmp_368_834, 0, sizeof(_pgen_tmp_368_834));
  memset(&_pgen_tmp_369_835, 0, sizeof(_pgen_tmp_369_835));
  memset(&_pgen_tmp_370_836, 0, sizeof(_pgen_tmp_370_836));
  memset(&_pgen_tmp_371_837, 0, sizeof(_pgen_tmp_371_837));
  memset(&_pgen_tmp_372_838, 0, sizeof(_pgen_tmp_372_838));
  memset(&_pgen_tmp_373_839, 0, sizeof(_pgen_tmp_373_839));
  memset(&_pgen_tmp_374_840, 0, sizeof(_pgen_tmp_374_840));
  memset(&_pgen_tmp_375_841, 0, sizeof(_pgen_tmp_375_841));
  memset(&_pgen_tmp_376_842, 0, sizeof(_pgen_tmp_376_842));
  memset(&_pgen_tmp_377_843, 0, sizeof(_pgen_tmp_377_843));
  memset(&_pgen_tmp_378_844, 0, sizeof(_pgen_tmp_378_844));
  memset(&_pgen_tmp_379_845, 0, sizeof(_pgen_tmp_379_845));
  memset(&_pgen_tmp_380_846, 0, sizeof(_pgen_tmp_380_846));
  memset(&_pgen_tmp_381_847, 0, sizeof(_pgen_tmp_381_847));
  memset(&_pgen_tmp_382_848, 0, sizeof(_pgen_tmp_382_848));
  memset(&_pgen_tmp_383_849, 0, sizeof(_pgen_tmp_383_849));
  memset(&_pgen_tmp_384_850, 0, sizeof(_pgen_tmp_384_850));
  memset(&_pgen_tmp_385_851, 0, sizeof(_pgen_tmp_385_851));
  memset(&_pgen_tmp_386_852, 0, sizeof(_pgen_tmp_386_852));
  memset(&_pgen_tmp_387_853, 0, sizeof(_pgen_tmp_387_853));
  memset(&_pgen_tmp_388_854, 0, sizeof(_pgen_tmp_388_854));
  memset(&_pgen_tmp_389_855, 0, sizeof(_pgen_tmp_389_855));
  memset(&_pgen_tmp_390_856, 0, sizeof(_pgen_tmp_390_856));
  memset(&_pgen_tmp_391_857, 0, sizeof(_pgen_tmp_391_857));
  memset(&_pgen_tmp_392_858, 0, sizeof(_pgen_tmp_392_858));
  memset(&_pgen_tmp_393_859, 0, sizeof(_pgen_tmp_393_859));
  memset(&_pgen_tmp_394_860, 0, sizeof(_pgen_tmp_394_860));
  memset(&_pgen_tmp_395_861, 0, sizeof(_pgen_tmp_395_861));
  memset(&_pgen_tmp_396_862, 0, sizeof(_pgen_tmp_396_862));
  memset(&_pgen_tmp_397_863, 0, sizeof(_pgen_tmp_397_863));
  memset(&_pgen_tmp_398_864, 0, sizeof(_pgen_tmp_398_864));
  memset(&_pgen_tmp_399_865, 0, sizeof(_pgen_tmp_399_865));
  memset(&_pgen_tmp_400_866, 0, sizeof(_pgen_tmp_400_866));
  memset(&_pgen_tmp_401_867, 0, sizeof(_pgen_tmp_401_867));
  memset(&_pgen_tmp_402_868, 0, sizeof(_pgen_tmp_402_868));
  memset(&_pgen_tmp_403_869, 0, sizeof(_pgen_tmp_403_869));
  memset(&_pgen_tmp_404_870, 0, sizeof(_pgen_tmp_404_870));
  memset(&_pgen_tmp_405_871, 0, sizeof(_pgen_tmp_405_871));
  memset(&_pgen_tmp_406_872, 0, sizeof(_pgen_tmp_406_872));
  memset(&_pgen_tmp_407_873, 0, sizeof(_pgen_tmp_407_873));
  memset(&_pgen_tmp_408_874, 0, sizeof(_pgen_tmp_408_874));
  memset(&_pgen_tmp_409_875, 0, sizeof(_pgen_tmp_409_875));
  memset(&_pgen_tmp_410_876, 0, sizeof(_pgen_tmp_410_876));
  memset(&_pgen_tmp_411_877, 0, sizeof(_pgen_tmp_411_877));
  memset(&_pgen_tmp_412_878, 0, sizeof(_pgen_tmp_412_878));
  memset(&_pgen_tmp_413_879, 0, sizeof(_pgen_tmp_413_879));
  memset(&_pgen_tmp_414_880, 0, sizeof(_pgen_tmp_414_880));
  memset(&_pgen_tmp_415_881, 0, sizeof(_pgen_tmp_415_881));
  memset(&_pgen_tmp_416_882, 0, sizeof(_pgen_tmp_416_882));
  memset(&_pgen_tmp_417_883, 0, sizeof(_pgen_tmp_417_883));
  memset(&_pgen_tmp_418_884, 0, sizeof(_pgen_tmp_418_884));
  memset(&_pgen_tmp_419_885, 0, sizeof(_pgen_tmp_419_885));
  memset(&_pgen_tmp_420_886, 0, sizeof(_pgen_tmp_420_886));
  memset(&_pgen_tmp_421_887, 0, sizeof(_pgen_tmp_421_887));
  memset(&_pgen_tmp_422_888, 0, sizeof(_pgen_tmp_422_888));
  memset(&_pgen_tmp_423_889, 0, sizeof(_pgen_tmp_423_889));
  memset(&_pgen_tmp_424_890, 0, sizeof(_pgen_tmp_424_890));
  memset(&_pgen_tmp_425_891, 0, sizeof(_pgen_tmp_425_891));
  memset(&_pgen_tmp_426_892, 0, sizeof(_pgen_tmp_426_892));
  memset(&_pgen_tmp_427_893, 0, sizeof(_pgen_tmp_427_893));
  memset(&_pgen_tmp_428_894, 0, sizeof(_pgen_tmp_428_894));
  memset(&_pgen_tmp_429_895, 0, sizeof(_pgen_tmp_429_895));
  memset(&_pgen_tmp_430_896, 0, sizeof(_pgen_tmp_430_896));
  memset(&_pgen_tmp_431_897, 0, sizeof(_pgen_tmp_431_897));
  memset(&_pgen_tmp_432_898, 0, sizeof(_pgen_tmp_432_898));
  memset(&_pgen_tmp_433_899, 0, sizeof(_pgen_tmp_433_899));
  memset(&_pgen_tmp_434_900, 0, sizeof(_pgen_tmp_434_900));
  memset(&_pgen_tmp_435_901, 0, sizeof(_pgen_tmp_435_901));
  memset(&_pgen_tmp_436_902, 0, sizeof(_pgen_tmp_436_902));
  memset(&_pgen_tmp_437_903, 0, sizeof(_pgen_tmp_437_903));
  memset(&_pgen_tmp_438_904, 0, sizeof(_pgen_tmp_438_904));
  memset(&_pgen_tmp_439_905, 0, sizeof(_pgen_tmp_439_905));
  memset(&_pgen_tmp_440_906, 0, sizeof(_pgen_tmp_440_906));
  memset(&_pgen_tmp_441_907, 0, sizeof(_pgen_tmp_441_907));
  memset(&_pgen_tmp_442_908, 0, sizeof(_pgen_tmp_442_908));
  memset(&_pgen_tmp_443_909, 0, sizeof(_pgen_tmp_443_909));
  memset(&_pgen_tmp_444_910, 0, sizeof(_pgen_tmp_444_910));
  memset(&_pgen_tmp_445_911, 0, sizeof(_pgen_tmp_445_911));
  memset(&_pgen_tmp_446_912, 0, sizeof(_pgen_tmp_446_912));
  memset(&_pgen_tmp_447_913, 0, sizeof(_pgen_tmp_447_913));
  memset(&_pgen_tmp_448_914, 0, sizeof(_pgen_tmp_448_914));
  memset(&_pgen_tmp_449_915, 0, sizeof(_pgen_tmp_449_915));
  memset(&_pgen_tmp_450_916, 0, sizeof(_pgen_tmp_450_916));
  memset(&_pgen_tmp_451_917, 0, sizeof(_pgen_tmp_451_917));
  memset(&_pgen_tmp_452_918, 0, sizeof(_pgen_tmp_452_918));
  memset(&_pgen_tmp_453_919, 0, sizeof(_pgen_tmp_453_919));
  memset(&_pgen_tmp_454_920, 0, sizeof(_pgen_tmp_454_920));
  memset(&_pgen_tmp_455_921, 0, sizeof(_pgen_tmp_455_921));
  memset(&_pgen_tmp_456_922, 0, sizeof(_pgen_tmp_456_922));
  memset(&_pgen_tmp_457_923, 0, sizeof(_pgen_tmp_457_923));
  memset(&_pgen_tmp_458_924, 0, sizeof(_pgen_tmp_458_924));
  memset(&_pgen_tmp_459_925, 0, sizeof(_pgen_tmp_459_925));
  memset(&_pgen_tmp_460_926, 0, sizeof(_pgen_tmp_460_926));
  memset(&_pgen_tmp_461_927, 0, sizeof(_pgen_tmp_461_927));
  memset(&_pgen_tmp_462_928, 0, sizeof(_pgen_tmp_462_928));
  memset(&_pgen_tmp_463_929, 0, sizeof(_pgen_tmp_463_929));
  memset(&_pgen_tmp_464_930, 0, sizeof(_pgen_tmp_464_930));
  memset(&_pgen_tmp_465_931, 0, sizeof(_pgen_tmp_465_931));
  memset(&_pgen_tmp_466_932, 0, sizeof(_pgen_tmp_466_932));
  memset(&_pgen_tmp_467_933, 0, sizeof(_pgen_tmp_467_933));
  memset(&_pgen_tmp_468_934, 0, sizeof(_pgen_tmp_468_934));
  memset(&_pgen_tmp_469_935, 0, sizeof(_pgen_tmp_469_935));
  memset(&_pgen_tmp_470_936, 0, sizeof(_pgen_tmp_470_936));
  memset(&_pgen_tmp_471_937, 0, sizeof(_pgen_tmp_471_937));
  memset(&_pgen_tmp_472_938, 0, sizeof(_pgen_tmp_472_938));
  memset(&_pgen_tmp_473_939, 0, sizeof(_pgen_tmp_473_939));
  memset(&_pgen_tmp_474_940, 0, sizeof(_pgen_tmp_474_940));
  memset(&_pgen_tmp_475_941, 0, sizeof(_pgen_tmp_475_941));
  memset(&_pgen_tmp_476_942, 0, sizeof(_pgen_tmp_476_942));
  memset(&_pgen_tmp_477_943, 0, sizeof(_pgen_tmp_477_943));
  memset(&_pgen_tmp_478_944, 0, sizeof(_pgen_tmp_478_944));
  memset(&_pgen_tmp_479_945, 0, sizeof(_pgen_tmp_479_945));
  memset(&_pgen_tmp_480_946, 0, sizeof(_pgen_tmp_480_946));
  memset(&_pgen_tmp_481_947, 0, sizeof(_pgen_tmp_481_947));
  memset(&_pgen_tmp_482_948, 0, sizeof(_pgen_tmp_482_948));
  memset(&_pgen_tmp_483_949, 0, sizeof(_pgen_tmp_483_949));
  memset(&_pgen_tmp_484_950, 0, sizeof(_pgen_tmp_484_950));
  memset(&_pgen_tmp_485_951, 0, sizeof(_pgen_tmp_485_951));
  memset(&_pgen_tmp_486_952, 0, sizeof(_pgen_tmp_486_952));
  memset(&_pgen_tmp_487_953, 0, sizeof(_pgen_tmp_487_953));
  memset(&_pgen_tmp_488_954, 0, sizeof(_pgen_tmp_488_954));
  memset(&_pgen_tmp_489_955, 0, sizeof(_pgen_tmp_489_955));
  memset(&_pgen_tmp_490_956, 0, sizeof(_pgen_tmp_490_956));
  memset(&_pgen_tmp_491_957, 0, sizeof(_pgen_tmp_491_957));
  memset(&_pgen_tmp_492_958, 0, sizeof(_pgen_tmp_492_958));
  memset(&_pgen_tmp_493_959, 0, sizeof(_pgen_tmp_493_959));
  memset(&_pgen_tmp_494_960, 0, sizeof(_pgen_tmp_494_960));
  memset(&_pgen_tmp_495_961, 0, sizeof(_pgen_tmp_495_961));
  memset(&_pgen_tmp_496_962, 0, sizeof(_pgen_tmp_496_962));
  memset(&_pgen_tmp_497_963, 0, sizeof(_pgen_tmp_497_963));
  memset(&_pgen_tmp_498_964, 0, sizeof(_pgen_tmp_498_964));
  memset(&_pgen_tmp_499_965, 0, sizeof(_pgen_tmp_499_965));
  memset(&_pgen_tmp_500_966, 0, sizeof(_pgen_tmp_500_966));
  memset(&_pgen_tmp_501_967, 0, sizeof(_pgen_tmp_501_967));
  memset(&_pgen_tmp_502_968, 0, sizeof(_pgen_tmp_502_968));
  memset(&_pgen_tmp_503_969, 0, sizeof(_pgen_tmp_503_969));
  memset(&_pgen_tmp_504_970, 0, sizeof(_pgen_tmp_504_970));
  memset(&_pgen_tmp_505_971, 0, sizeof(_pgen_tmp_505_971));
  memset(&_pgen_tmp_506_972, 0, sizeof(_pgen_tmp_506_972));
  memset(&_pgen_tmp_507_973, 0, sizeof(_pgen_tmp_507_973));
  memset(&_pgen_tmp_508_974, 0, sizeof(_pgen_tmp_508_974));
  memset(&_pgen_tmp_509_975, 0, sizeof(_pgen_tmp_509_975));
  memset(&_pgen_tmp_510_976, 0, sizeof(_pgen_tmp_510_976));
  memset(&_pgen_tmp_511_977, 0, sizeof(_pgen_tmp_511_977));
  memset(&_pgen_tmp_512_978, 0, sizeof(_pgen_tmp_512_978));
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
  memset(&VS_out_conv1_1, 0, sizeof(VS_out_conv1_1));
  for (mesh_x_n1_2 = 0; mesh_x_n1_2 < 64; mesh_x_n1_2 = mesh_x_n1_2 + 1) {
    for (mesh_y_n1_3 = 0; mesh_y_n1_3 < 3; mesh_y_n1_3 = mesh_y_n1_3 + 1) {
      for (mesh_z_n1_4 = 0; mesh_z_n1_4 < 2; mesh_z_n1_4 = mesh_z_n1_4 + 1) {
        Tm_start("Tensor::add");
        Tm_start("Tensor::conv");
        Copy_ciph(&_preg_268435456, &input_0[((mesh_y_n1_3 * 2) + mesh_z_n1_4)]);
        Zero_ciph(&result_n21_5);
        Copy_ciph(&input_dup_n21_6, &_preg_268435456);
        for (cap_block_input_grid_n21_7 = 0; cap_block_input_grid_n21_7 < 49; cap_block_input_grid_n21_7 = cap_block_input_grid_n21_7 + 1) {
          _preg_268435544 = Rotate(input_dup_n21_6, _cst_141[cap_block_input_grid_n21_7]);
          Copy_ciph(&input_grid_n21_8[cap_block_input_grid_n21_7], &_preg_268435544);
          Free_poly_data(&_preg_268435544._c1_poly);
          Free_poly_data(&_preg_268435544._c0_poly);
        }
        for (num_grid_n21_9 = 0; num_grid_n21_9 < 1; num_grid_n21_9 = num_grid_n21_9 + 1) {
          Zero_ciph(&result_grid_n21_10);
          for (cap_block_n21_11 = 0; cap_block_n21_11 < 49; cap_block_n21_11 = cap_block_n21_11 + 1) {
            _ckks_gen_tmp_95_366 = input_grid_n21_8[cap_block_n21_11];
            Pt_from_msg_ofst(&_pgen_tmp_0_466, 0/* cst_138 */, ((((mesh_x_n1_2 * 3) + mesh_y_n1_3) * 49) + (cap_block_n21_11 + (num_grid_n21_9 * 49))) * (26432), 26432, 1, 4);
            Init_ciph_up_scale_plain(&_pgen_tmp_1_467, &_ckks_gen_tmp_95_366, &_pgen_tmp_0_466);
            Init_ciph_same_scale(&result_grid_n21_10, &result_grid_n21_10, &_pgen_tmp_1_467);
            {
              _pgen_modulus_463 = Q_modulus();
              _pgen_num_q_464 = Poly_level(&result_grid_n21_10._c0_poly);
              for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
                Hw_modmul(Coeffs(&_pgen_tmp_1_467._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_95_366._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_0_466._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
                Hw_modmul(Coeffs(&_pgen_tmp_1_467._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_95_366._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_0_466._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
                Hw_modadd(Coeffs(&result_grid_n21_10._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n21_10._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_1_467._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
                Hw_modadd(Coeffs(&result_grid_n21_10._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n21_10._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_1_467._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
                _pgen_modulus_463 = _pgen_modulus_463 + 1;
              }
            }
          }
          _preg_268435545 = Rotate(result_grid_n21_10, num_grid_n21_9 * 26432);
          Init_ciph_same_scale(&result_n21_5, &result_n21_5, &_preg_268435545);
          {
            _pgen_modulus_463 = Q_modulus();
            _pgen_num_q_464 = Poly_level(&result_n21_5._c0_poly);
            for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
              Hw_modadd(Coeffs(&result_n21_5._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n21_5._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435545._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_n21_5._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n21_5._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435545._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              _pgen_modulus_463 = _pgen_modulus_463 + 1;
            }
          }
          Free_poly_data(&_preg_268435545._c1_poly);
          Free_poly_data(&_preg_268435545._c0_poly);
        }
        Pt_from_msg(&_pgen_tmp_2_468, 1 /* cst_140 */, 26432, 2, 4);
        Init_ciph_same_scale_plain(&result_n21_5, &result_n21_5, &_pgen_tmp_2_468);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&result_n21_5._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modadd(Coeffs(&result_n21_5._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n21_5._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_2_468._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Set_coeffs(&result_n21_5._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&result_n21_5._c1_poly, _pgen_rns_idx_465, degree));
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        _preg_268435546 = Rotate(result_n21_5, 672);
        Copy_ciph(&x_row21_12, &_preg_268435546);
        Zero_ciph(&comb_row_result22_13);
        for (combine_row_index_n22_14 = 0; combine_row_index_n22_14 < 112; combine_row_index_n22_14 = combine_row_index_n22_14 + 1) {
          _preg_268435547 = Rotate(x_row21_12, combine_row_index_n22_14 * 1);
          Copy_ciph(&_ckks_gen_tmp_165_367, &_preg_268435547);
          Init_ciph_down_scale(&_pgen_tmp_3_469, &_ckks_gen_tmp_165_367);
          Rescale(&_pgen_tmp_3_469._c0_poly, &_ckks_gen_tmp_165_367._c0_poly);
          Rescale(&_pgen_tmp_3_469._c1_poly, &_ckks_gen_tmp_165_367._c1_poly);
          Pt_from_msg_ofst(&_pgen_tmp_4_470, 2/* cst_142 */, (combine_row_index_n22_14) * (25088), 25088, 1, 3);
          Init_ciph_up_scale_plain(&_pgen_tmp_5_471, &_pgen_tmp_3_469, &_pgen_tmp_4_470);
          Init_ciph_same_scale(&comb_row_result22_13, &comb_row_result22_13, &_pgen_tmp_5_471);
          {
            _pgen_modulus_463 = Q_modulus();
            _pgen_num_q_464 = Poly_level(&comb_row_result22_13._c0_poly);
            for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
              Hw_modmul(Coeffs(&_pgen_tmp_5_471._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_3_469._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_4_470._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modmul(Coeffs(&_pgen_tmp_5_471._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_3_469._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_4_470._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&comb_row_result22_13._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_row_result22_13._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_5_471._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&comb_row_result22_13._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_row_result22_13._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_5_471._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              _pgen_modulus_463 = _pgen_modulus_463 + 1;
            }
          }
          Free_poly_data(&_preg_268435547._c1_poly);
          Free_poly_data(&_preg_268435547._c0_poly);
        }
        Zero_ciph(&comb_rc_result22_15);
        for (combine_rc_index_n22_16 = 0; combine_rc_index_n22_16 < 56; combine_rc_index_n22_16 = combine_rc_index_n22_16 + 1) {
          _preg_268435548 = Rotate(comb_row_result22_13, combine_rc_index_n22_16 * 336);
          Copy_ciph(&_ckks_gen_tmp_198_368, &_preg_268435548);
          Init_ciph_down_scale(&_pgen_tmp_6_472, &_ckks_gen_tmp_198_368);
          Rescale(&_pgen_tmp_6_472._c0_poly, &_ckks_gen_tmp_198_368._c0_poly);
          Rescale(&_pgen_tmp_6_472._c1_poly, &_ckks_gen_tmp_198_368._c1_poly);
          Pt_from_msg_ofst(&_pgen_tmp_7_473, 3/* cst_143 */, (combine_rc_index_n22_16) * (25088), 25088, 1, 2);
          Init_ciph_up_scale_plain(&_pgen_tmp_8_474, &_pgen_tmp_6_472, &_pgen_tmp_7_473);
          Init_ciph_same_scale(&comb_rc_result22_15, &comb_rc_result22_15, &_pgen_tmp_8_474);
          {
            _pgen_modulus_463 = Q_modulus();
            _pgen_num_q_464 = Poly_level(&comb_rc_result22_15._c0_poly);
            for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
              Hw_modmul(Coeffs(&_pgen_tmp_8_474._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_6_472._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_7_473._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modmul(Coeffs(&_pgen_tmp_8_474._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_6_472._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_7_473._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&comb_rc_result22_15._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_rc_result22_15._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_8_474._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&comb_rc_result22_15._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_rc_result22_15._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_8_474._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              _pgen_modulus_463 = _pgen_modulus_463 + 1;
            }
          }
          Free_poly_data(&_preg_268435548._c1_poly);
          Free_poly_data(&_preg_268435548._c0_poly);
        }
        _pgen_tmp_10_476 = VS_out_conv1_1[((mesh_x_n1_2 * 2) + mesh_z_n1_4)];
        Init_ciph_same_scale(&_pgen_tmp_9_475, &_pgen_tmp_10_476, &comb_rc_result22_15);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&_pgen_tmp_9_475._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modadd(Coeffs(&_pgen_tmp_9_475._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_10_476._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_rc_result22_15._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&_pgen_tmp_9_475._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_10_476._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_rc_result22_15._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Copy_ciph(&VS_out_conv1_1[((mesh_x_n1_2 * 2) + mesh_z_n1_4)], &_pgen_tmp_9_475);
        Tm_taken("Tensor::conv");
        Tm_taken("Tensor::add");
        Free_poly_data(&_preg_268435546._c1_poly);
        Free_poly_data(&_preg_268435546._c0_poly);
      }
    }
  }
  Free_ciph_poly(input_0, 6);
  Free_poly_data(&_preg_268435456._c1_poly);
  Free_poly_data(&_preg_268435456._c0_poly);
  Free_poly_data(&input_dup_n21_6._c1_poly);
  Free_poly_data(&input_dup_n21_6._c0_poly);
  Free_ciph_poly(input_grid_n21_8, 49);
  Free_poly_data(&_pgen_tmp_0_466._poly);
  Free_poly_data(&_pgen_tmp_1_467._c1_poly);
  Free_poly_data(&_pgen_tmp_1_467._c0_poly);
  Free_poly_data(&result_grid_n21_10._c1_poly);
  Free_poly_data(&result_grid_n21_10._c0_poly);
  Free_poly_data(&_pgen_tmp_2_468._poly);
  Free_poly_data(&result_n21_5._c1_poly);
  Free_poly_data(&result_n21_5._c0_poly);
  Free_poly_data(&x_row21_12._c1_poly);
  Free_poly_data(&x_row21_12._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_165_367._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_165_367._c0_poly);
  Free_poly_data(&_pgen_tmp_4_470._poly);
  Free_poly_data(&_pgen_tmp_3_469._c1_poly);
  Free_poly_data(&_pgen_tmp_3_469._c0_poly);
  Free_poly_data(&_pgen_tmp_5_471._c1_poly);
  Free_poly_data(&_pgen_tmp_5_471._c0_poly);
  Free_poly_data(&comb_row_result22_13._c1_poly);
  Free_poly_data(&comb_row_result22_13._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_198_368._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_198_368._c0_poly);
  Free_poly_data(&_pgen_tmp_7_473._poly);
  Free_poly_data(&_pgen_tmp_6_472._c1_poly);
  Free_poly_data(&_pgen_tmp_6_472._c0_poly);
  Free_poly_data(&_pgen_tmp_8_474._c1_poly);
  Free_poly_data(&_pgen_tmp_8_474._c0_poly);
  Free_poly_data(&comb_rc_result22_15._c1_poly);
  Free_poly_data(&comb_rc_result22_15._c0_poly);
  Free_poly_data(&_pgen_tmp_9_475._c1_poly);
  Free_poly_data(&_pgen_tmp_9_475._c0_poly);
  for (rescale_iv_445 = 0; rescale_iv_445 < 128; rescale_iv_445 = rescale_iv_445 + 1) {
    _pgen_tmp_11_477 = VS_out_conv1_1[rescale_iv_445];
    Init_ciph_down_scale(&_pgen_tmp_12_478, &_pgen_tmp_11_477);
    Rescale(&_pgen_tmp_12_478._c0_poly, &_pgen_tmp_11_477._c0_poly);
    Rescale(&_pgen_tmp_12_478._c1_poly, &_pgen_tmp_11_477._c1_poly);
    Copy_ciph(&VS_out_conv1_1[rescale_iv_445], &_pgen_tmp_12_478);
  }
  Free_poly_data(&_pgen_tmp_12_478._c1_poly);
  Free_poly_data(&_pgen_tmp_12_478._c0_poly);
  for (reshardm_n1_17 = 0; reshardm_n1_17 < 32; reshardm_n1_17 = reshardm_n1_17 + 1) {
    Tm_start("Tensor::concat");
    Tm_start("Tensor::concat");
    Tm_start("Tensor::concat");
    _pgen_tmp_14_480 = VS_out_conv1_1[(reshardm_n1_17 * 4)];
    _pgen_tmp_15_481 = VS_out_conv1_1[((reshardm_n1_17 * 4) + 1)];
    _preg_268435549 = Rotate(_pgen_tmp_15_481, -6272);
    Init_ciph_same_scale(&_pgen_tmp_16_482, &_pgen_tmp_14_480, &_preg_268435549);
    _pgen_tmp_18_484 = VS_out_conv1_1[((reshardm_n1_17 * 4) + 2)];
    _pgen_tmp_19_485 = VS_out_conv1_1[((reshardm_n1_17 * 4) + 3)];
    _preg_268435550 = Rotate(_pgen_tmp_19_485, -6272);
    Init_ciph_same_scale(&_pgen_tmp_17_483, &_pgen_tmp_18_484, &_preg_268435550);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&_pgen_tmp_17_483._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_17_483._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_18_484._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435550._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_17_483._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_18_484._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435550._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    _preg_268435551 = Rotate(_pgen_tmp_17_483, -12544);
    Init_ciph_same_scale(&_pgen_tmp_13_479, &_pgen_tmp_16_482, &_preg_268435551);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&_pgen_tmp_13_479._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_16_482._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_14_480._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435549._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_16_482._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_14_480._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435549._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_13_479._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_16_482._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435551._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_13_479._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_16_482._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435551._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    Copy_ciph(&output_reshard1_18[reshardm_n1_17], &_pgen_tmp_13_479);
    Tm_taken("Tensor::concat");
    Tm_taken("Tensor::concat");
    Tm_taken("Tensor::concat");
    Free_poly_data(&_preg_268435549._c1_poly);
    Free_poly_data(&_preg_268435549._c0_poly);
    Free_poly_data(&_preg_268435550._c1_poly);
    Free_poly_data(&_preg_268435550._c0_poly);
    Free_poly_data(&_preg_268435551._c1_poly);
    Free_poly_data(&_preg_268435551._c0_poly);
  }
  Free_ciph_poly(VS_out_conv1_1, 128);
  Free_poly_data(&_pgen_tmp_17_483._c1_poly);
  Free_poly_data(&_pgen_tmp_17_483._c0_poly);
  Free_poly_data(&_pgen_tmp_16_482._c1_poly);
  Free_poly_data(&_pgen_tmp_16_482._c0_poly);
  Free_poly_data(&_pgen_tmp_13_479._c1_poly);
  Free_poly_data(&_pgen_tmp_13_479._c0_poly);
  for (add_bias_n1_19 = 0; add_bias_n1_19 < 32; add_bias_n1_19 = add_bias_n1_19 + 1) {
    Tm_start("Tensor::add");
    _ckks_gen_tmp_307_369 = output_reshard1_18[add_bias_n1_19];
    Pt_from_msg_ofst(&_pgen_tmp_21_487, 4/* cst_148 */, (add_bias_n1_19) * (25088), 25088, 1, 1);
    Init_ciph_same_scale_plain(&_pgen_tmp_20_486, &_ckks_gen_tmp_307_369, &_pgen_tmp_21_487);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&_pgen_tmp_20_486._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_20_486._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_307_369._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_21_487._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Set_coeffs(&_pgen_tmp_20_486._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_ckks_gen_tmp_307_369._c1_poly, _pgen_rns_idx_465, degree));
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    Copy_ciph(&addshard1_20[add_bias_n1_19], &_pgen_tmp_20_486);
    Tm_taken("Tensor::add");
  }
  Free_ciph_poly(output_reshard1_18, 32);
  Free_poly_data(&_pgen_tmp_21_487._poly);
  Free_poly_data(&_pgen_tmp_20_486._c1_poly);
  Free_poly_data(&_pgen_tmp_20_486._c0_poly);
  memset(&VS_out_relu1_21, 0, sizeof(VS_out_relu1_21));
  for (mesh_x_n1_22 = 0; mesh_x_n1_22 < 32; mesh_x_n1_22 = mesh_x_n1_22 + 1) {
    for (mesh_y_n1_23 = 0; mesh_y_n1_23 < 1; mesh_y_n1_23 = mesh_y_n1_23 + 1) {
      Tm_start("Tensor::add");
      _pgen_tmp_22_488 = addshard1_20[((mesh_x_n1_22 * 1) + mesh_y_n1_23)];
      Bootstrap(&_preg_268435457, &_pgen_tmp_22_488, 19);
      Encode_float(&_pgen_tmp_24_490, &_cst_462, 1, 1, Level(&_preg_268435457));
      Init_ciph_up_scale_plain(&_pgen_tmp_23_489, &_preg_268435457, &_pgen_tmp_24_490);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_23_489._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_23_489._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435457._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_24_490._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_23_489._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435457._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_24_490._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Init_ciph_down_scale(&_pgen_tmp_25_491, &_pgen_tmp_23_489);
      Rescale(&_pgen_tmp_25_491._c0_poly, &_pgen_tmp_23_489._c0_poly);
      Rescale(&_pgen_tmp_25_491._c1_poly, &_pgen_tmp_23_489._c1_poly);
      _preg_268435458 = App_relu(_preg_268435457, _pgen_tmp_25_491, 25088);
      _pgen_tmp_27_493 = VS_out_relu1_21[((mesh_x_n1_22 * 1) + mesh_y_n1_23)];
      Init_ciph_same_scale(&_pgen_tmp_26_492, &_pgen_tmp_27_493, &_preg_268435458);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_26_492._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_26_492._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_27_493._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435458._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_26_492._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_27_493._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435458._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_relu1_21[((mesh_x_n1_22 * 1) + mesh_y_n1_23)], &_pgen_tmp_26_492);
      Tm_taken("Tensor::add");
    }
  }
  Free_ciph_poly(addshard1_20, 32);
  Free_poly_data(&_pgen_tmp_24_490._poly);
  Free_poly_data(&_pgen_tmp_23_489._c1_poly);
  Free_poly_data(&_pgen_tmp_23_489._c0_poly);
  Free_poly_data(&_pgen_tmp_25_491._c1_poly);
  Free_poly_data(&_pgen_tmp_25_491._c0_poly);
  Free_poly_data(&_preg_268435457._c1_poly);
  Free_poly_data(&_preg_268435457._c0_poly);
  Free_poly_data(&_preg_268435458._c1_poly);
  Free_poly_data(&_preg_268435458._c0_poly);
  Free_poly_data(&_pgen_tmp_26_492._c1_poly);
  Free_poly_data(&_pgen_tmp_26_492._c0_poly);
  memset(&VS_out_avp1_24, 0, sizeof(VS_out_avp1_24));
  for (mesh_x_n1_25 = 0; mesh_x_n1_25 < 32; mesh_x_n1_25 = mesh_x_n1_25 + 1) {
    for (mesh_y_n1_26 = 0; mesh_y_n1_26 < 1; mesh_y_n1_26 = mesh_y_n1_26 + 1) {
      Tm_start("Tensor::avg_pool");
      Copy_ciph(&_preg_268435459, &VS_out_relu1_21[((mesh_x_n1_25 * 1) + mesh_y_n1_26)]);
      _preg_268435552 = Rotate(_preg_268435459, 112);
      Init_ciph_same_scale(&tmp_row_add22_27, &_preg_268435459, &_preg_268435552);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&tmp_row_add22_27._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&tmp_row_add22_27._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435459._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435552._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&tmp_row_add22_27._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435459._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435552._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      _preg_268435553 = Rotate(tmp_row_add22_27, 1);
      Init_ciph_same_scale(&_ckks_gen_tmp_426_370, &tmp_row_add22_27, &_preg_268435553);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_ckks_gen_tmp_426_370._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_ckks_gen_tmp_426_370._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&tmp_row_add22_27._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435553._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_ckks_gen_tmp_426_370._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&tmp_row_add22_27._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435553._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Pt_from_msg(&_pgen_tmp_28_494, 5 /* cst_151 */, 25088, 1, 7);
      Init_ciph_up_scale_plain(&_preg_268435460, &_ckks_gen_tmp_426_370, &_pgen_tmp_28_494);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435460._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_preg_268435460._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_426_370._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_28_494._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_preg_268435460._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_426_370._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_28_494._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Tm_taken("Tensor::avg_pool");
      Tm_start("Tensor::add");
      Copy_ciph(&orig_result22_28, &_preg_268435460);
      Zero_ciph(&comb_row_result23_29);
      for (combine_row_index_n23_30 = 0; combine_row_index_n23_30 < 56; combine_row_index_n23_30 = combine_row_index_n23_30 + 1) {
        _preg_268435554 = Rotate(orig_result22_28, combine_row_index_n23_30 * 1);
        Copy_ciph(&_ckks_gen_tmp_459_371, &_preg_268435554);
        Init_ciph_down_scale(&_pgen_tmp_29_495, &_ckks_gen_tmp_459_371);
        Rescale(&_pgen_tmp_29_495._c0_poly, &_ckks_gen_tmp_459_371._c0_poly);
        Rescale(&_pgen_tmp_29_495._c1_poly, &_ckks_gen_tmp_459_371._c1_poly);
        Pt_from_msg_ofst(&_pgen_tmp_30_496, 6/* cst_153 */, (combine_row_index_n23_30) * (25088), 25088, 1, 6);
        Init_ciph_up_scale_plain(&_pgen_tmp_31_497, &_pgen_tmp_29_495, &_pgen_tmp_30_496);
        Init_ciph_same_scale(&comb_row_result23_29, &comb_row_result23_29, &_pgen_tmp_31_497);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&comb_row_result23_29._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modmul(Coeffs(&_pgen_tmp_31_497._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_29_495._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_30_496._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modmul(Coeffs(&_pgen_tmp_31_497._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_29_495._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_30_496._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&comb_row_result23_29._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_row_result23_29._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_31_497._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&comb_row_result23_29._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_row_result23_29._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_31_497._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435554._c1_poly);
        Free_poly_data(&_preg_268435554._c0_poly);
      }
      Zero_ciph(&comb_rc_result23_31);
      for (combine_rc_index_n23_32 = 0; combine_rc_index_n23_32 < 56; combine_rc_index_n23_32 = combine_rc_index_n23_32 + 1) {
        _preg_268435555 = Rotate(comb_row_result23_29, combine_rc_index_n23_32 * 168);
        Copy_ciph(&_ckks_gen_tmp_492_372, &_preg_268435555);
        Init_ciph_down_scale(&_pgen_tmp_32_498, &_ckks_gen_tmp_492_372);
        Rescale(&_pgen_tmp_32_498._c0_poly, &_ckks_gen_tmp_492_372._c0_poly);
        Rescale(&_pgen_tmp_32_498._c1_poly, &_ckks_gen_tmp_492_372._c1_poly);
        Pt_from_msg_ofst(&_pgen_tmp_33_499, 7/* cst_154 */, (combine_rc_index_n23_32) * (25088), 25088, 1, 5);
        Init_ciph_up_scale_plain(&_pgen_tmp_34_500, &_pgen_tmp_32_498, &_pgen_tmp_33_499);
        Init_ciph_same_scale(&comb_rc_result23_31, &comb_rc_result23_31, &_pgen_tmp_34_500);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&comb_rc_result23_31._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modmul(Coeffs(&_pgen_tmp_34_500._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_32_498._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_33_499._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modmul(Coeffs(&_pgen_tmp_34_500._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_32_498._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_33_499._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&comb_rc_result23_31._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_rc_result23_31._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_34_500._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&comb_rc_result23_31._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_rc_result23_31._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_34_500._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435555._c1_poly);
        Free_poly_data(&_preg_268435555._c0_poly);
      }
      Zero_ciph(&final_result23_33);
      for (combine_cc_index_n23_34 = 0; combine_cc_index_n23_34 < 1; combine_cc_index_n23_34 = combine_cc_index_n23_34 + 1) {
        _preg_268435556 = Rotate(comb_rc_result23_31, (1 << combine_cc_index_n23_34) * 9408);
        Init_ciph_same_scale(&comb_rc_result23_31, &comb_rc_result23_31, &_preg_268435556);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&comb_rc_result23_31._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modadd(Coeffs(&comb_rc_result23_31._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_rc_result23_31._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435556._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&comb_rc_result23_31._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_rc_result23_31._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435556._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435556._c1_poly);
        Free_poly_data(&_preg_268435556._c0_poly);
      }
      Init_ciph_down_scale(&_pgen_tmp_35_501, &comb_rc_result23_31);
      Rescale(&_pgen_tmp_35_501._c0_poly, &comb_rc_result23_31._c0_poly);
      Rescale(&_pgen_tmp_35_501._c1_poly, &comb_rc_result23_31._c1_poly);
      Pt_from_msg(&_pgen_tmp_36_502, 8 /* cst_155 */, 6272, 1, 4);
      Init_ciph_up_scale_plain(&comb_rc_result23_31, &_pgen_tmp_35_501, &_pgen_tmp_36_502);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&comb_rc_result23_31._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&comb_rc_result23_31._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_35_501._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_36_502._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&comb_rc_result23_31._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_35_501._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_36_502._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&final_result23_33, &comb_rc_result23_31);
      _pgen_tmp_38_504 = VS_out_avp1_24[((mesh_x_n1_25 * 1) + mesh_y_n1_26)];
      Init_ciph_same_scale(&_pgen_tmp_37_503, &_pgen_tmp_38_504, &final_result23_33);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_37_503._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_37_503._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_38_504._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&final_result23_33._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_37_503._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_38_504._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&final_result23_33._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_avp1_24[((mesh_x_n1_25 * 1) + mesh_y_n1_26)], &_pgen_tmp_37_503);
      Tm_taken("Tensor::add");
      Free_poly_data(&_preg_268435552._c1_poly);
      Free_poly_data(&_preg_268435552._c0_poly);
      Free_poly_data(&_preg_268435553._c1_poly);
      Free_poly_data(&_preg_268435553._c0_poly);
    }
  }
  Free_ciph_poly(VS_out_relu1_21, 32);
  Free_poly_data(&_preg_268435459._c1_poly);
  Free_poly_data(&_preg_268435459._c0_poly);
  Free_poly_data(&tmp_row_add22_27._c1_poly);
  Free_poly_data(&tmp_row_add22_27._c0_poly);
  Free_poly_data(&_pgen_tmp_28_494._poly);
  Free_poly_data(&_ckks_gen_tmp_426_370._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_426_370._c0_poly);
  Free_poly_data(&_preg_268435460._c1_poly);
  Free_poly_data(&_preg_268435460._c0_poly);
  Free_poly_data(&orig_result22_28._c1_poly);
  Free_poly_data(&orig_result22_28._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_459_371._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_459_371._c0_poly);
  Free_poly_data(&_pgen_tmp_30_496._poly);
  Free_poly_data(&_pgen_tmp_29_495._c1_poly);
  Free_poly_data(&_pgen_tmp_29_495._c0_poly);
  Free_poly_data(&_pgen_tmp_31_497._c1_poly);
  Free_poly_data(&_pgen_tmp_31_497._c0_poly);
  Free_poly_data(&comb_row_result23_29._c1_poly);
  Free_poly_data(&comb_row_result23_29._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_492_372._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_492_372._c0_poly);
  Free_poly_data(&_pgen_tmp_33_499._poly);
  Free_poly_data(&_pgen_tmp_32_498._c1_poly);
  Free_poly_data(&_pgen_tmp_32_498._c0_poly);
  Free_poly_data(&_pgen_tmp_34_500._c1_poly);
  Free_poly_data(&_pgen_tmp_34_500._c0_poly);
  Free_poly_data(&_pgen_tmp_36_502._poly);
  Free_poly_data(&_pgen_tmp_35_501._c1_poly);
  Free_poly_data(&_pgen_tmp_35_501._c0_poly);
  Free_poly_data(&comb_rc_result23_31._c1_poly);
  Free_poly_data(&comb_rc_result23_31._c0_poly);
  Free_poly_data(&final_result23_33._c1_poly);
  Free_poly_data(&final_result23_33._c0_poly);
  Free_poly_data(&_pgen_tmp_37_503._c1_poly);
  Free_poly_data(&_pgen_tmp_37_503._c0_poly);
  for (rescale_iv_446 = 0; rescale_iv_446 < 32; rescale_iv_446 = rescale_iv_446 + 1) {
    _pgen_tmp_39_505 = VS_out_avp1_24[rescale_iv_446];
    Init_ciph_down_scale(&_pgen_tmp_40_506, &_pgen_tmp_39_505);
    Rescale(&_pgen_tmp_40_506._c0_poly, &_pgen_tmp_39_505._c0_poly);
    Rescale(&_pgen_tmp_40_506._c1_poly, &_pgen_tmp_39_505._c1_poly);
    Copy_ciph(&VS_out_avp1_24[rescale_iv_446], &_pgen_tmp_40_506);
  }
  Free_poly_data(&_pgen_tmp_40_506._c1_poly);
  Free_poly_data(&_pgen_tmp_40_506._c0_poly);
  for (reshardm_n1_35 = 0; reshardm_n1_35 < 8; reshardm_n1_35 = reshardm_n1_35 + 1) {
    Tm_start("Tensor::concat");
    Tm_start("Tensor::concat");
    Tm_start("Tensor::concat");
    _pgen_tmp_42_508 = VS_out_avp1_24[(reshardm_n1_35 * 4)];
    _pgen_tmp_43_509 = VS_out_avp1_24[((reshardm_n1_35 * 4) + 1)];
    _preg_268435557 = Rotate(_pgen_tmp_43_509, -6272);
    Init_ciph_same_scale(&_pgen_tmp_44_510, &_pgen_tmp_42_508, &_preg_268435557);
    _pgen_tmp_46_512 = VS_out_avp1_24[((reshardm_n1_35 * 4) + 2)];
    _pgen_tmp_47_513 = VS_out_avp1_24[((reshardm_n1_35 * 4) + 3)];
    _preg_268435558 = Rotate(_pgen_tmp_47_513, -6272);
    Init_ciph_same_scale(&_pgen_tmp_45_511, &_pgen_tmp_46_512, &_preg_268435558);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&_pgen_tmp_45_511._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_45_511._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_46_512._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435558._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_45_511._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_46_512._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435558._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    _preg_268435559 = Rotate(_pgen_tmp_45_511, -12544);
    Init_ciph_same_scale(&_pgen_tmp_41_507, &_pgen_tmp_44_510, &_preg_268435559);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&_pgen_tmp_41_507._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_44_510._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_42_508._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435557._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_44_510._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_42_508._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435557._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_41_507._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_44_510._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435559._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_41_507._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_44_510._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435559._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    Copy_ciph(&output_reshard1_36[reshardm_n1_35], &_pgen_tmp_41_507);
    Tm_taken("Tensor::concat");
    Tm_taken("Tensor::concat");
    Tm_taken("Tensor::concat");
    Free_poly_data(&_preg_268435557._c1_poly);
    Free_poly_data(&_preg_268435557._c0_poly);
    Free_poly_data(&_preg_268435558._c1_poly);
    Free_poly_data(&_preg_268435558._c0_poly);
    Free_poly_data(&_preg_268435559._c1_poly);
    Free_poly_data(&_preg_268435559._c0_poly);
  }
  Free_ciph_poly(VS_out_avp1_24, 32);
  Free_poly_data(&_pgen_tmp_45_511._c1_poly);
  Free_poly_data(&_pgen_tmp_45_511._c0_poly);
  Free_poly_data(&_pgen_tmp_44_510._c1_poly);
  Free_poly_data(&_pgen_tmp_44_510._c0_poly);
  Free_poly_data(&_pgen_tmp_41_507._c1_poly);
  Free_poly_data(&_pgen_tmp_41_507._c0_poly);
  memset(&VS_out_conv2_37, 0, sizeof(VS_out_conv2_37));
  for (mesh_x_n2_38 = 0; mesh_x_n2_38 < 8; mesh_x_n2_38 = mesh_x_n2_38 + 1) {
    for (mesh_y_n2_39 = 0; mesh_y_n2_39 < 8; mesh_y_n2_39 = mesh_y_n2_39 + 1) {
      Tm_start("Tensor::add");
      Tm_start("Tensor::conv");
      Copy_ciph(&_preg_268435461, &output_reshard1_36[mesh_y_n2_39]);
      Zero_ciph(&result_n24_40);
      Copy_ciph(&input_dup_n24_41, &_preg_268435461);
      for (cap_block_input_grid_n24_42 = 0; cap_block_input_grid_n24_42 < 9; cap_block_input_grid_n24_42 = cap_block_input_grid_n24_42 + 1) {
        _preg_268435560 = Rotate(input_dup_n24_41, _cst_165[cap_block_input_grid_n24_42]);
        Copy_ciph(&input_grid_n24_43[cap_block_input_grid_n24_42], &_preg_268435560);
        Free_poly_data(&_preg_268435560._c1_poly);
        Free_poly_data(&_preg_268435560._c0_poly);
      }
      for (num_grid_n24_44 = 0; num_grid_n24_44 < 8; num_grid_n24_44 = num_grid_n24_44 + 1) {
        Zero_ciph(&result_grid_n24_45);
        for (cap_block_n24_46 = 0; cap_block_n24_46 < 9; cap_block_n24_46 = cap_block_n24_46 + 1) {
          _ckks_gen_tmp_696_373 = input_grid_n24_43[cap_block_n24_46];
          Pt_from_msg_ofst(&_pgen_tmp_48_514, 9/* cst_162 */, ((((mesh_x_n2_38 * 8) + mesh_y_n2_39) * 72) + (cap_block_n24_46 + (num_grid_n24_44 * 9))) * (25088), 25088, 1, 3);
          Init_ciph_up_scale_plain(&_pgen_tmp_49_515, &_ckks_gen_tmp_696_373, &_pgen_tmp_48_514);
          Init_ciph_same_scale(&result_grid_n24_45, &result_grid_n24_45, &_pgen_tmp_49_515);
          {
            _pgen_modulus_463 = Q_modulus();
            _pgen_num_q_464 = Poly_level(&result_grid_n24_45._c0_poly);
            for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
              Hw_modmul(Coeffs(&_pgen_tmp_49_515._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_696_373._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_48_514._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modmul(Coeffs(&_pgen_tmp_49_515._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_696_373._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_48_514._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n24_45._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n24_45._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_49_515._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n24_45._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n24_45._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_49_515._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              _pgen_modulus_463 = _pgen_modulus_463 + 1;
            }
          }
        }
        _preg_268435561 = Rotate(result_grid_n24_45, num_grid_n24_44 * 3136);
        Init_ciph_same_scale(&result_n24_40, &result_n24_40, &_preg_268435561);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&result_n24_40._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modadd(Coeffs(&result_n24_40._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n24_40._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435561._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&result_n24_40._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n24_40._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435561._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435561._c1_poly);
        Free_poly_data(&_preg_268435561._c0_poly);
      }
      Init_ciph_down_scale(&_pgen_tmp_50_516, &result_n24_40);
      Rescale(&_pgen_tmp_50_516._c0_poly, &result_n24_40._c0_poly);
      Rescale(&_pgen_tmp_50_516._c1_poly, &result_n24_40._c1_poly);
      Pt_from_msg(&_pgen_tmp_51_517, 10 /* cst_166 */, 25088, 1, 2);
      Init_ciph_up_scale_plain(&_preg_268435462, &_pgen_tmp_50_516, &_pgen_tmp_51_517);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435462._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_preg_268435462._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_50_516._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_51_517._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_preg_268435462._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_50_516._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_51_517._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      _preg_268435562 = Rotate(result_n24_40, -25088);
      Copy_ciph(&_ckks_gen_tmp_748_374, &_preg_268435562);
      Init_ciph_down_scale(&_pgen_tmp_52_518, &_ckks_gen_tmp_748_374);
      Rescale(&_pgen_tmp_52_518._c0_poly, &_ckks_gen_tmp_748_374._c0_poly);
      Rescale(&_pgen_tmp_52_518._c1_poly, &_ckks_gen_tmp_748_374._c1_poly);
      Pt_from_msg(&_pgen_tmp_53_519, 11 /* cst_167 */, 25088, 1, 2);
      Init_ciph_up_scale_plain(&_pgen_tmp_54_520, &_pgen_tmp_52_518, &_pgen_tmp_53_519);
      Init_ciph_same_scale(&_preg_268435462, &_preg_268435462, &_pgen_tmp_54_520);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435462._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_54_520._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_52_518._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_53_519._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_54_520._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_52_518._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_53_519._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435462._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435462._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_54_520._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435462._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435462._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_54_520._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Pt_from_msg(&_pgen_tmp_55_521, 12 /* cst_164 */, 25088, 2, 2);
      Init_ciph_same_scale_plain(&result_n24_40, &_preg_268435462, &_pgen_tmp_55_521);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&result_n24_40._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&result_n24_40._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435462._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_55_521._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Set_coeffs(&result_n24_40._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_preg_268435462._c1_poly, _pgen_rns_idx_465, degree));
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      _pgen_tmp_57_523 = VS_out_conv2_37[mesh_x_n2_38];
      Init_ciph_same_scale(&_pgen_tmp_56_522, &_pgen_tmp_57_523, &result_n24_40);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_56_522._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_56_522._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_57_523._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n24_40._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_56_522._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_57_523._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n24_40._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_conv2_37[mesh_x_n2_38], &_pgen_tmp_56_522);
      Tm_taken("Tensor::conv");
      Tm_taken("Tensor::add");
      Free_poly_data(&_preg_268435562._c1_poly);
      Free_poly_data(&_preg_268435562._c0_poly);
    }
  }
  Free_poly_data(&_preg_268435461._c1_poly);
  Free_poly_data(&_preg_268435461._c0_poly);
  Free_poly_data(&input_dup_n24_41._c1_poly);
  Free_poly_data(&input_dup_n24_41._c0_poly);
  Free_ciph_poly(input_grid_n24_43, 9);
  Free_poly_data(&_pgen_tmp_48_514._poly);
  Free_poly_data(&_pgen_tmp_49_515._c1_poly);
  Free_poly_data(&_pgen_tmp_49_515._c0_poly);
  Free_poly_data(&result_grid_n24_45._c1_poly);
  Free_poly_data(&result_grid_n24_45._c0_poly);
  Free_poly_data(&_pgen_tmp_51_517._poly);
  Free_poly_data(&_pgen_tmp_50_516._c1_poly);
  Free_poly_data(&_pgen_tmp_50_516._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_748_374._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_748_374._c0_poly);
  Free_poly_data(&_pgen_tmp_53_519._poly);
  Free_poly_data(&_pgen_tmp_52_518._c1_poly);
  Free_poly_data(&_pgen_tmp_52_518._c0_poly);
  Free_poly_data(&_pgen_tmp_54_520._c1_poly);
  Free_poly_data(&_pgen_tmp_54_520._c0_poly);
  Free_poly_data(&_pgen_tmp_55_521._poly);
  Free_poly_data(&_preg_268435462._c1_poly);
  Free_poly_data(&_preg_268435462._c0_poly);
  Free_poly_data(&result_n24_40._c1_poly);
  Free_poly_data(&result_n24_40._c0_poly);
  Free_poly_data(&_pgen_tmp_56_522._c1_poly);
  Free_poly_data(&_pgen_tmp_56_522._c0_poly);
  for (rescale_iv_447 = 0; rescale_iv_447 < 8; rescale_iv_447 = rescale_iv_447 + 1) {
    _pgen_tmp_58_524 = VS_out_conv2_37[rescale_iv_447];
    Init_ciph_down_scale(&_pgen_tmp_59_525, &_pgen_tmp_58_524);
    Rescale(&_pgen_tmp_59_525._c0_poly, &_pgen_tmp_58_524._c0_poly);
    Rescale(&_pgen_tmp_59_525._c1_poly, &_pgen_tmp_58_524._c1_poly);
    Copy_ciph(&VS_out_conv2_37[rescale_iv_447], &_pgen_tmp_59_525);
  }
  Free_poly_data(&_pgen_tmp_59_525._c1_poly);
  Free_poly_data(&_pgen_tmp_59_525._c0_poly);
  for (add_bias_n2_47 = 0; add_bias_n2_47 < 8; add_bias_n2_47 = add_bias_n2_47 + 1) {
    Tm_start("Tensor::add");
    _ckks_gen_tmp_797_375 = VS_out_conv2_37[add_bias_n2_47];
    Pt_from_msg_ofst(&_pgen_tmp_61_527, 13/* cst_169 */, (add_bias_n2_47) * (25088), 25088, 1, 1);
    Init_ciph_same_scale_plain(&_pgen_tmp_60_526, &_ckks_gen_tmp_797_375, &_pgen_tmp_61_527);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&_pgen_tmp_60_526._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_60_526._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_797_375._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_61_527._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Set_coeffs(&_pgen_tmp_60_526._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_ckks_gen_tmp_797_375._c1_poly, _pgen_rns_idx_465, degree));
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    Copy_ciph(&addshard2_48[add_bias_n2_47], &_pgen_tmp_60_526);
    Tm_taken("Tensor::add");
  }
  Free_ciph_poly(VS_out_conv2_37, 8);
  Free_poly_data(&_pgen_tmp_61_527._poly);
  Free_poly_data(&_pgen_tmp_60_526._c1_poly);
  Free_poly_data(&_pgen_tmp_60_526._c0_poly);
  memset(&VS_out_relu2_49, 0, sizeof(VS_out_relu2_49));
  for (mesh_x_n2_50 = 0; mesh_x_n2_50 < 8; mesh_x_n2_50 = mesh_x_n2_50 + 1) {
    for (mesh_y_n2_51 = 0; mesh_y_n2_51 < 1; mesh_y_n2_51 = mesh_y_n2_51 + 1) {
      Tm_start("Tensor::add");
      _pgen_tmp_62_528 = addshard2_48[((mesh_x_n2_50 * 1) + mesh_y_n2_51)];
      Bootstrap(&_preg_268435463, &_pgen_tmp_62_528, 15);
      Encode_float(&_pgen_tmp_64_530, &_cst_463, 1, 1, Level(&_preg_268435463));
      Init_ciph_up_scale_plain(&_pgen_tmp_63_529, &_preg_268435463, &_pgen_tmp_64_530);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_63_529._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_63_529._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435463._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_64_530._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_63_529._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435463._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_64_530._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Init_ciph_down_scale(&_pgen_tmp_65_531, &_pgen_tmp_63_529);
      Rescale(&_pgen_tmp_65_531._c0_poly, &_pgen_tmp_63_529._c0_poly);
      Rescale(&_pgen_tmp_65_531._c1_poly, &_pgen_tmp_63_529._c1_poly);
      _preg_268435464 = App_relu(_preg_268435463, _pgen_tmp_65_531, 25088);
      _pgen_tmp_67_533 = VS_out_relu2_49[((mesh_x_n2_50 * 1) + mesh_y_n2_51)];
      Init_ciph_same_scale(&_pgen_tmp_66_532, &_pgen_tmp_67_533, &_preg_268435464);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_66_532._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_66_532._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_67_533._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435464._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_66_532._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_67_533._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435464._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_relu2_49[((mesh_x_n2_50 * 1) + mesh_y_n2_51)], &_pgen_tmp_66_532);
      Tm_taken("Tensor::add");
    }
  }
  Free_ciph_poly(addshard2_48, 8);
  Free_poly_data(&_pgen_tmp_64_530._poly);
  Free_poly_data(&_pgen_tmp_63_529._c1_poly);
  Free_poly_data(&_pgen_tmp_63_529._c0_poly);
  Free_poly_data(&_pgen_tmp_65_531._c1_poly);
  Free_poly_data(&_pgen_tmp_65_531._c0_poly);
  Free_poly_data(&_preg_268435463._c1_poly);
  Free_poly_data(&_preg_268435463._c0_poly);
  Free_poly_data(&_preg_268435464._c1_poly);
  Free_poly_data(&_preg_268435464._c0_poly);
  Free_poly_data(&_pgen_tmp_66_532._c1_poly);
  Free_poly_data(&_pgen_tmp_66_532._c0_poly);
  memset(&VS_out_conv3_52, 0, sizeof(VS_out_conv3_52));
  for (mesh_x_n3_53 = 0; mesh_x_n3_53 < 8; mesh_x_n3_53 = mesh_x_n3_53 + 1) {
    for (mesh_y_n3_54 = 0; mesh_y_n3_54 < 8; mesh_y_n3_54 = mesh_y_n3_54 + 1) {
      Tm_start("Tensor::add");
      Tm_start("Tensor::conv");
      Copy_ciph(&_preg_268435465, &VS_out_relu2_49[mesh_y_n3_54]);
      Zero_ciph(&result_n25_55);
      Copy_ciph(&input_dup_n25_56, &_preg_268435465);
      for (cap_block_input_grid_n25_57 = 0; cap_block_input_grid_n25_57 < 9; cap_block_input_grid_n25_57 = cap_block_input_grid_n25_57 + 1) {
        _preg_268435563 = Rotate(input_dup_n25_56, _cst_177[cap_block_input_grid_n25_57]);
        Copy_ciph(&input_grid_n25_58[cap_block_input_grid_n25_57], &_preg_268435563);
        Free_poly_data(&_preg_268435563._c1_poly);
        Free_poly_data(&_preg_268435563._c0_poly);
      }
      for (num_grid_n25_59 = 0; num_grid_n25_59 < 8; num_grid_n25_59 = num_grid_n25_59 + 1) {
        Zero_ciph(&result_grid_n25_60);
        for (cap_block_n25_61 = 0; cap_block_n25_61 < 9; cap_block_n25_61 = cap_block_n25_61 + 1) {
          _ckks_gen_tmp_950_376 = input_grid_n25_58[cap_block_n25_61];
          Pt_from_msg_ofst(&_pgen_tmp_68_534, 14/* cst_174 */, ((((mesh_x_n3_53 * 8) + mesh_y_n3_54) * 72) + (cap_block_n25_61 + (num_grid_n25_59 * 9))) * (25088), 25088, 1, 3);
          Init_ciph_up_scale_plain(&_pgen_tmp_69_535, &_ckks_gen_tmp_950_376, &_pgen_tmp_68_534);
          Init_ciph_same_scale(&result_grid_n25_60, &result_grid_n25_60, &_pgen_tmp_69_535);
          {
            _pgen_modulus_463 = Q_modulus();
            _pgen_num_q_464 = Poly_level(&result_grid_n25_60._c0_poly);
            for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
              Hw_modmul(Coeffs(&_pgen_tmp_69_535._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_950_376._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_68_534._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modmul(Coeffs(&_pgen_tmp_69_535._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_950_376._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_68_534._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n25_60._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n25_60._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_69_535._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n25_60._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n25_60._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_69_535._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              _pgen_modulus_463 = _pgen_modulus_463 + 1;
            }
          }
        }
        _preg_268435564 = Rotate(result_grid_n25_60, num_grid_n25_59 * 3136);
        Init_ciph_same_scale(&result_n25_55, &result_n25_55, &_preg_268435564);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&result_n25_55._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modadd(Coeffs(&result_n25_55._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n25_55._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435564._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&result_n25_55._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n25_55._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435564._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435564._c1_poly);
        Free_poly_data(&_preg_268435564._c0_poly);
      }
      Init_ciph_down_scale(&_pgen_tmp_70_536, &result_n25_55);
      Rescale(&_pgen_tmp_70_536._c0_poly, &result_n25_55._c0_poly);
      Rescale(&_pgen_tmp_70_536._c1_poly, &result_n25_55._c1_poly);
      Pt_from_msg(&_pgen_tmp_71_537, 15 /* cst_178 */, 25088, 1, 2);
      Init_ciph_up_scale_plain(&_preg_268435466, &_pgen_tmp_70_536, &_pgen_tmp_71_537);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435466._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_preg_268435466._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_70_536._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_71_537._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_preg_268435466._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_70_536._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_71_537._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      _preg_268435565 = Rotate(result_n25_55, -25088);
      Copy_ciph(&_ckks_gen_tmp_1002_377, &_preg_268435565);
      Init_ciph_down_scale(&_pgen_tmp_72_538, &_ckks_gen_tmp_1002_377);
      Rescale(&_pgen_tmp_72_538._c0_poly, &_ckks_gen_tmp_1002_377._c0_poly);
      Rescale(&_pgen_tmp_72_538._c1_poly, &_ckks_gen_tmp_1002_377._c1_poly);
      Pt_from_msg(&_pgen_tmp_73_539, 16 /* cst_179 */, 25088, 1, 2);
      Init_ciph_up_scale_plain(&_pgen_tmp_74_540, &_pgen_tmp_72_538, &_pgen_tmp_73_539);
      Init_ciph_same_scale(&_preg_268435466, &_preg_268435466, &_pgen_tmp_74_540);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435466._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_74_540._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_72_538._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_73_539._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_74_540._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_72_538._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_73_539._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435466._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435466._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_74_540._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435466._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435466._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_74_540._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Pt_from_msg(&_pgen_tmp_75_541, 17 /* cst_176 */, 25088, 2, 2);
      Init_ciph_same_scale_plain(&result_n25_55, &_preg_268435466, &_pgen_tmp_75_541);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&result_n25_55._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&result_n25_55._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435466._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_75_541._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Set_coeffs(&result_n25_55._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_preg_268435466._c1_poly, _pgen_rns_idx_465, degree));
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      _pgen_tmp_77_543 = VS_out_conv3_52[mesh_x_n3_53];
      Init_ciph_same_scale(&_pgen_tmp_76_542, &_pgen_tmp_77_543, &result_n25_55);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_76_542._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_76_542._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_77_543._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n25_55._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_76_542._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_77_543._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n25_55._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_conv3_52[mesh_x_n3_53], &_pgen_tmp_76_542);
      Tm_taken("Tensor::conv");
      Tm_taken("Tensor::add");
      Free_poly_data(&_preg_268435565._c1_poly);
      Free_poly_data(&_preg_268435565._c0_poly);
    }
  }
  Free_ciph_poly(VS_out_relu2_49, 8);
  Free_poly_data(&_preg_268435465._c1_poly);
  Free_poly_data(&_preg_268435465._c0_poly);
  Free_poly_data(&input_dup_n25_56._c1_poly);
  Free_poly_data(&input_dup_n25_56._c0_poly);
  Free_ciph_poly(input_grid_n25_58, 9);
  Free_poly_data(&_pgen_tmp_68_534._poly);
  Free_poly_data(&_pgen_tmp_69_535._c1_poly);
  Free_poly_data(&_pgen_tmp_69_535._c0_poly);
  Free_poly_data(&result_grid_n25_60._c1_poly);
  Free_poly_data(&result_grid_n25_60._c0_poly);
  Free_poly_data(&_pgen_tmp_71_537._poly);
  Free_poly_data(&_pgen_tmp_70_536._c1_poly);
  Free_poly_data(&_pgen_tmp_70_536._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_1002_377._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_1002_377._c0_poly);
  Free_poly_data(&_pgen_tmp_73_539._poly);
  Free_poly_data(&_pgen_tmp_72_538._c1_poly);
  Free_poly_data(&_pgen_tmp_72_538._c0_poly);
  Free_poly_data(&_pgen_tmp_74_540._c1_poly);
  Free_poly_data(&_pgen_tmp_74_540._c0_poly);
  Free_poly_data(&_pgen_tmp_75_541._poly);
  Free_poly_data(&_preg_268435466._c1_poly);
  Free_poly_data(&_preg_268435466._c0_poly);
  Free_poly_data(&result_n25_55._c1_poly);
  Free_poly_data(&result_n25_55._c0_poly);
  Free_poly_data(&_pgen_tmp_76_542._c1_poly);
  Free_poly_data(&_pgen_tmp_76_542._c0_poly);
  for (rescale_iv_448 = 0; rescale_iv_448 < 8; rescale_iv_448 = rescale_iv_448 + 1) {
    _pgen_tmp_78_544 = VS_out_conv3_52[rescale_iv_448];
    Init_ciph_down_scale(&_pgen_tmp_79_545, &_pgen_tmp_78_544);
    Rescale(&_pgen_tmp_79_545._c0_poly, &_pgen_tmp_78_544._c0_poly);
    Rescale(&_pgen_tmp_79_545._c1_poly, &_pgen_tmp_78_544._c1_poly);
    Copy_ciph(&VS_out_conv3_52[rescale_iv_448], &_pgen_tmp_79_545);
  }
  Free_poly_data(&_pgen_tmp_79_545._c1_poly);
  Free_poly_data(&_pgen_tmp_79_545._c0_poly);
  for (add_bias_n3_62 = 0; add_bias_n3_62 < 8; add_bias_n3_62 = add_bias_n3_62 + 1) {
    Tm_start("Tensor::add");
    _ckks_gen_tmp_1051_378 = VS_out_conv3_52[add_bias_n3_62];
    Pt_from_msg_ofst(&_pgen_tmp_81_547, 18/* cst_181 */, (add_bias_n3_62) * (25088), 25088, 1, 1);
    Init_ciph_same_scale_plain(&_pgen_tmp_80_546, &_ckks_gen_tmp_1051_378, &_pgen_tmp_81_547);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&_pgen_tmp_80_546._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_80_546._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_1051_378._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_81_547._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Set_coeffs(&_pgen_tmp_80_546._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_ckks_gen_tmp_1051_378._c1_poly, _pgen_rns_idx_465, degree));
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    Copy_ciph(&addshard3_63[add_bias_n3_62], &_pgen_tmp_80_546);
    Tm_taken("Tensor::add");
  }
  Free_ciph_poly(VS_out_conv3_52, 8);
  Free_poly_data(&_pgen_tmp_81_547._poly);
  Free_poly_data(&_pgen_tmp_80_546._c1_poly);
  Free_poly_data(&_pgen_tmp_80_546._c0_poly);
  memset(&VS_out_add3_64, 0, sizeof(VS_out_add3_64));
  for (mesh_x_n3_65 = 0; mesh_x_n3_65 < 8; mesh_x_n3_65 = mesh_x_n3_65 + 1) {
    for (mesh_y_n3_66 = 0; mesh_y_n3_66 < 1; mesh_y_n3_66 = mesh_y_n3_66 + 1) {
      Tm_start("Tensor::add");
      Tm_start("Tensor::add");
      _pgen_tmp_83_549 = VS_out_add3_64[((mesh_x_n3_65 * 1) + mesh_y_n3_66)];
      _pgen_tmp_84_550 = addshard3_63[((mesh_x_n3_65 * 1) + mesh_y_n3_66)];
      _pgen_tmp_85_551 = output_reshard1_36[((mesh_x_n3_65 * 1) + mesh_y_n3_66)];
      Init_ciph_same_scale(&_pgen_tmp_86_552, &_pgen_tmp_84_550, &_pgen_tmp_85_551);
      Init_ciph_same_scale(&_pgen_tmp_82_548, &_pgen_tmp_83_549, &_pgen_tmp_86_552);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_82_548._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_86_552._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_84_550._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_85_551._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_86_552._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_84_550._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_85_551._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_82_548._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_83_549._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_86_552._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_82_548._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_83_549._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_86_552._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_add3_64[((mesh_x_n3_65 * 1) + mesh_y_n3_66)], &_pgen_tmp_82_548);
      Tm_taken("Tensor::add");
      Tm_taken("Tensor::add");
    }
  }
  Free_ciph_poly(addshard3_63, 8);
  Free_ciph_poly(output_reshard1_36, 8);
  Free_poly_data(&_pgen_tmp_86_552._c1_poly);
  Free_poly_data(&_pgen_tmp_86_552._c0_poly);
  Free_poly_data(&_pgen_tmp_82_548._c1_poly);
  Free_poly_data(&_pgen_tmp_82_548._c0_poly);
  memset(&VS_out_relu3_67, 0, sizeof(VS_out_relu3_67));
  for (mesh_x_n3_68 = 0; mesh_x_n3_68 < 8; mesh_x_n3_68 = mesh_x_n3_68 + 1) {
    for (mesh_y_n3_69 = 0; mesh_y_n3_69 < 1; mesh_y_n3_69 = mesh_y_n3_69 + 1) {
      Tm_start("Tensor::add");
      _pgen_tmp_87_553 = VS_out_add3_64[((mesh_x_n3_68 * 1) + mesh_y_n3_69)];
      Bootstrap(&_preg_268435467, &_pgen_tmp_87_553, 15);
      Encode_float(&_pgen_tmp_89_555, &_cst_464, 1, 1, Level(&_preg_268435467));
      Init_ciph_up_scale_plain(&_pgen_tmp_88_554, &_preg_268435467, &_pgen_tmp_89_555);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_88_554._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_88_554._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435467._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_89_555._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_88_554._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435467._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_89_555._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Init_ciph_down_scale(&_pgen_tmp_90_556, &_pgen_tmp_88_554);
      Rescale(&_pgen_tmp_90_556._c0_poly, &_pgen_tmp_88_554._c0_poly);
      Rescale(&_pgen_tmp_90_556._c1_poly, &_pgen_tmp_88_554._c1_poly);
      _preg_268435468 = App_relu(_preg_268435467, _pgen_tmp_90_556, 25088);
      _pgen_tmp_92_558 = VS_out_relu3_67[((mesh_x_n3_68 * 1) + mesh_y_n3_69)];
      Init_ciph_same_scale(&_pgen_tmp_91_557, &_pgen_tmp_92_558, &_preg_268435468);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_91_557._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_91_557._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_92_558._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435468._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_91_557._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_92_558._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435468._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_relu3_67[((mesh_x_n3_68 * 1) + mesh_y_n3_69)], &_pgen_tmp_91_557);
      Tm_taken("Tensor::add");
    }
  }
  Free_ciph_poly(VS_out_add3_64, 8);
  Free_poly_data(&_pgen_tmp_89_555._poly);
  Free_poly_data(&_pgen_tmp_88_554._c1_poly);
  Free_poly_data(&_pgen_tmp_88_554._c0_poly);
  Free_poly_data(&_pgen_tmp_90_556._c1_poly);
  Free_poly_data(&_pgen_tmp_90_556._c0_poly);
  Free_poly_data(&_preg_268435467._c1_poly);
  Free_poly_data(&_preg_268435467._c0_poly);
  Free_poly_data(&_preg_268435468._c1_poly);
  Free_poly_data(&_preg_268435468._c0_poly);
  Free_poly_data(&_pgen_tmp_91_557._c1_poly);
  Free_poly_data(&_pgen_tmp_91_557._c0_poly);
  memset(&VS_out_conv4_70, 0, sizeof(VS_out_conv4_70));
  for (mesh_x_n4_71 = 0; mesh_x_n4_71 < 8; mesh_x_n4_71 = mesh_x_n4_71 + 1) {
    for (mesh_y_n4_72 = 0; mesh_y_n4_72 < 8; mesh_y_n4_72 = mesh_y_n4_72 + 1) {
      Tm_start("Tensor::add");
      Tm_start("Tensor::conv");
      Copy_ciph(&_preg_268435469, &VS_out_relu3_67[mesh_y_n4_72]);
      Zero_ciph(&result_n26_73);
      Copy_ciph(&input_dup_n26_74, &_preg_268435469);
      for (cap_block_input_grid_n26_75 = 0; cap_block_input_grid_n26_75 < 9; cap_block_input_grid_n26_75 = cap_block_input_grid_n26_75 + 1) {
        _preg_268435566 = Rotate(input_dup_n26_74, _cst_191[cap_block_input_grid_n26_75]);
        Copy_ciph(&input_grid_n26_76[cap_block_input_grid_n26_75], &_preg_268435566);
        Free_poly_data(&_preg_268435566._c1_poly);
        Free_poly_data(&_preg_268435566._c0_poly);
      }
      for (num_grid_n26_77 = 0; num_grid_n26_77 < 8; num_grid_n26_77 = num_grid_n26_77 + 1) {
        Zero_ciph(&result_grid_n26_78);
        for (cap_block_n26_79 = 0; cap_block_n26_79 < 9; cap_block_n26_79 = cap_block_n26_79 + 1) {
          _ckks_gen_tmp_1264_379 = input_grid_n26_76[cap_block_n26_79];
          Pt_from_msg_ofst(&_pgen_tmp_93_559, 19/* cst_188 */, ((((mesh_x_n4_71 * 8) + mesh_y_n4_72) * 72) + (cap_block_n26_79 + (num_grid_n26_77 * 9))) * (25088), 25088, 1, 3);
          Init_ciph_up_scale_plain(&_pgen_tmp_94_560, &_ckks_gen_tmp_1264_379, &_pgen_tmp_93_559);
          Init_ciph_same_scale(&result_grid_n26_78, &result_grid_n26_78, &_pgen_tmp_94_560);
          {
            _pgen_modulus_463 = Q_modulus();
            _pgen_num_q_464 = Poly_level(&result_grid_n26_78._c0_poly);
            for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
              Hw_modmul(Coeffs(&_pgen_tmp_94_560._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_1264_379._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_93_559._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modmul(Coeffs(&_pgen_tmp_94_560._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_1264_379._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_93_559._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n26_78._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n26_78._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_94_560._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n26_78._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n26_78._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_94_560._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              _pgen_modulus_463 = _pgen_modulus_463 + 1;
            }
          }
        }
        _preg_268435567 = Rotate(result_grid_n26_78, num_grid_n26_77 * 3136);
        Init_ciph_same_scale(&result_n26_73, &result_n26_73, &_preg_268435567);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&result_n26_73._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modadd(Coeffs(&result_n26_73._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n26_73._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435567._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&result_n26_73._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n26_73._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435567._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435567._c1_poly);
        Free_poly_data(&_preg_268435567._c0_poly);
      }
      Init_ciph_down_scale(&_pgen_tmp_95_561, &result_n26_73);
      Rescale(&_pgen_tmp_95_561._c0_poly, &result_n26_73._c0_poly);
      Rescale(&_pgen_tmp_95_561._c1_poly, &result_n26_73._c1_poly);
      Pt_from_msg(&_pgen_tmp_96_562, 20 /* cst_192 */, 25088, 1, 2);
      Init_ciph_up_scale_plain(&_preg_268435470, &_pgen_tmp_95_561, &_pgen_tmp_96_562);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435470._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_preg_268435470._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_95_561._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_96_562._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_preg_268435470._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_95_561._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_96_562._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      _preg_268435568 = Rotate(result_n26_73, -25088);
      Copy_ciph(&_ckks_gen_tmp_1316_380, &_preg_268435568);
      Init_ciph_down_scale(&_pgen_tmp_97_563, &_ckks_gen_tmp_1316_380);
      Rescale(&_pgen_tmp_97_563._c0_poly, &_ckks_gen_tmp_1316_380._c0_poly);
      Rescale(&_pgen_tmp_97_563._c1_poly, &_ckks_gen_tmp_1316_380._c1_poly);
      Pt_from_msg(&_pgen_tmp_98_564, 21 /* cst_193 */, 25088, 1, 2);
      Init_ciph_up_scale_plain(&_pgen_tmp_99_565, &_pgen_tmp_97_563, &_pgen_tmp_98_564);
      Init_ciph_same_scale(&_preg_268435470, &_preg_268435470, &_pgen_tmp_99_565);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435470._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_99_565._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_97_563._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_98_564._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_99_565._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_97_563._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_98_564._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435470._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435470._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_99_565._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435470._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435470._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_99_565._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Pt_from_msg(&_pgen_tmp_100_566, 22 /* cst_190 */, 25088, 2, 2);
      Init_ciph_same_scale_plain(&result_n26_73, &_preg_268435470, &_pgen_tmp_100_566);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&result_n26_73._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&result_n26_73._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435470._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_100_566._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Set_coeffs(&result_n26_73._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_preg_268435470._c1_poly, _pgen_rns_idx_465, degree));
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      _pgen_tmp_102_568 = VS_out_conv4_70[mesh_x_n4_71];
      Init_ciph_same_scale(&_pgen_tmp_101_567, &_pgen_tmp_102_568, &result_n26_73);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_101_567._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_101_567._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_102_568._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n26_73._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_101_567._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_102_568._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n26_73._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_conv4_70[mesh_x_n4_71], &_pgen_tmp_101_567);
      Tm_taken("Tensor::conv");
      Tm_taken("Tensor::add");
      Free_poly_data(&_preg_268435568._c1_poly);
      Free_poly_data(&_preg_268435568._c0_poly);
    }
  }
  Free_poly_data(&_preg_268435469._c1_poly);
  Free_poly_data(&_preg_268435469._c0_poly);
  Free_poly_data(&input_dup_n26_74._c1_poly);
  Free_poly_data(&input_dup_n26_74._c0_poly);
  Free_ciph_poly(input_grid_n26_76, 9);
  Free_poly_data(&_pgen_tmp_93_559._poly);
  Free_poly_data(&_pgen_tmp_94_560._c1_poly);
  Free_poly_data(&_pgen_tmp_94_560._c0_poly);
  Free_poly_data(&result_grid_n26_78._c1_poly);
  Free_poly_data(&result_grid_n26_78._c0_poly);
  Free_poly_data(&_pgen_tmp_96_562._poly);
  Free_poly_data(&_pgen_tmp_95_561._c1_poly);
  Free_poly_data(&_pgen_tmp_95_561._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_1316_380._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_1316_380._c0_poly);
  Free_poly_data(&_pgen_tmp_98_564._poly);
  Free_poly_data(&_pgen_tmp_97_563._c1_poly);
  Free_poly_data(&_pgen_tmp_97_563._c0_poly);
  Free_poly_data(&_pgen_tmp_99_565._c1_poly);
  Free_poly_data(&_pgen_tmp_99_565._c0_poly);
  Free_poly_data(&_pgen_tmp_100_566._poly);
  Free_poly_data(&_preg_268435470._c1_poly);
  Free_poly_data(&_preg_268435470._c0_poly);
  Free_poly_data(&result_n26_73._c1_poly);
  Free_poly_data(&result_n26_73._c0_poly);
  Free_poly_data(&_pgen_tmp_101_567._c1_poly);
  Free_poly_data(&_pgen_tmp_101_567._c0_poly);
  for (rescale_iv_449 = 0; rescale_iv_449 < 8; rescale_iv_449 = rescale_iv_449 + 1) {
    _pgen_tmp_103_569 = VS_out_conv4_70[rescale_iv_449];
    Init_ciph_down_scale(&_pgen_tmp_104_570, &_pgen_tmp_103_569);
    Rescale(&_pgen_tmp_104_570._c0_poly, &_pgen_tmp_103_569._c0_poly);
    Rescale(&_pgen_tmp_104_570._c1_poly, &_pgen_tmp_103_569._c1_poly);
    Copy_ciph(&VS_out_conv4_70[rescale_iv_449], &_pgen_tmp_104_570);
  }
  Free_poly_data(&_pgen_tmp_104_570._c1_poly);
  Free_poly_data(&_pgen_tmp_104_570._c0_poly);
  for (add_bias_n4_80 = 0; add_bias_n4_80 < 8; add_bias_n4_80 = add_bias_n4_80 + 1) {
    Tm_start("Tensor::add");
    _ckks_gen_tmp_1365_381 = VS_out_conv4_70[add_bias_n4_80];
    Pt_from_msg_ofst(&_pgen_tmp_106_572, 23/* cst_195 */, (add_bias_n4_80) * (25088), 25088, 1, 1);
    Init_ciph_same_scale_plain(&_pgen_tmp_105_571, &_ckks_gen_tmp_1365_381, &_pgen_tmp_106_572);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&_pgen_tmp_105_571._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_105_571._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_1365_381._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_106_572._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Set_coeffs(&_pgen_tmp_105_571._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_ckks_gen_tmp_1365_381._c1_poly, _pgen_rns_idx_465, degree));
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    Copy_ciph(&addshard4_81[add_bias_n4_80], &_pgen_tmp_105_571);
    Tm_taken("Tensor::add");
  }
  Free_ciph_poly(VS_out_conv4_70, 8);
  Free_poly_data(&_pgen_tmp_106_572._poly);
  Free_poly_data(&_pgen_tmp_105_571._c1_poly);
  Free_poly_data(&_pgen_tmp_105_571._c0_poly);
  memset(&VS_out_relu4_82, 0, sizeof(VS_out_relu4_82));
  for (mesh_x_n4_83 = 0; mesh_x_n4_83 < 8; mesh_x_n4_83 = mesh_x_n4_83 + 1) {
    for (mesh_y_n4_84 = 0; mesh_y_n4_84 < 1; mesh_y_n4_84 = mesh_y_n4_84 + 1) {
      Tm_start("Tensor::add");
      _pgen_tmp_107_573 = addshard4_81[((mesh_x_n4_83 * 1) + mesh_y_n4_84)];
      Bootstrap(&_preg_268435471, &_pgen_tmp_107_573, 15);
      Encode_float(&_pgen_tmp_109_575, &_cst_465, 1, 1, Level(&_preg_268435471));
      Init_ciph_up_scale_plain(&_pgen_tmp_108_574, &_preg_268435471, &_pgen_tmp_109_575);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_108_574._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_108_574._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435471._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_109_575._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_108_574._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435471._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_109_575._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Init_ciph_down_scale(&_pgen_tmp_110_576, &_pgen_tmp_108_574);
      Rescale(&_pgen_tmp_110_576._c0_poly, &_pgen_tmp_108_574._c0_poly);
      Rescale(&_pgen_tmp_110_576._c1_poly, &_pgen_tmp_108_574._c1_poly);
      _preg_268435472 = App_relu(_preg_268435471, _pgen_tmp_110_576, 25088);
      _pgen_tmp_112_578 = VS_out_relu4_82[((mesh_x_n4_83 * 1) + mesh_y_n4_84)];
      Init_ciph_same_scale(&_pgen_tmp_111_577, &_pgen_tmp_112_578, &_preg_268435472);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_111_577._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_111_577._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_112_578._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435472._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_111_577._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_112_578._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435472._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_relu4_82[((mesh_x_n4_83 * 1) + mesh_y_n4_84)], &_pgen_tmp_111_577);
      Tm_taken("Tensor::add");
    }
  }
  Free_ciph_poly(addshard4_81, 8);
  Free_poly_data(&_pgen_tmp_109_575._poly);
  Free_poly_data(&_pgen_tmp_108_574._c1_poly);
  Free_poly_data(&_pgen_tmp_108_574._c0_poly);
  Free_poly_data(&_pgen_tmp_110_576._c1_poly);
  Free_poly_data(&_pgen_tmp_110_576._c0_poly);
  Free_poly_data(&_preg_268435471._c1_poly);
  Free_poly_data(&_preg_268435471._c0_poly);
  Free_poly_data(&_preg_268435472._c1_poly);
  Free_poly_data(&_preg_268435472._c0_poly);
  Free_poly_data(&_pgen_tmp_111_577._c1_poly);
  Free_poly_data(&_pgen_tmp_111_577._c0_poly);
  memset(&VS_out_conv5_85, 0, sizeof(VS_out_conv5_85));
  for (mesh_x_n5_86 = 0; mesh_x_n5_86 < 8; mesh_x_n5_86 = mesh_x_n5_86 + 1) {
    for (mesh_y_n5_87 = 0; mesh_y_n5_87 < 8; mesh_y_n5_87 = mesh_y_n5_87 + 1) {
      Tm_start("Tensor::add");
      Tm_start("Tensor::conv");
      Copy_ciph(&_preg_268435473, &VS_out_relu4_82[mesh_y_n5_87]);
      Zero_ciph(&result_n27_88);
      Copy_ciph(&input_dup_n27_89, &_preg_268435473);
      for (cap_block_input_grid_n27_90 = 0; cap_block_input_grid_n27_90 < 9; cap_block_input_grid_n27_90 = cap_block_input_grid_n27_90 + 1) {
        _preg_268435569 = Rotate(input_dup_n27_89, _cst_203[cap_block_input_grid_n27_90]);
        Copy_ciph(&input_grid_n27_91[cap_block_input_grid_n27_90], &_preg_268435569);
        Free_poly_data(&_preg_268435569._c1_poly);
        Free_poly_data(&_preg_268435569._c0_poly);
      }
      for (num_grid_n27_92 = 0; num_grid_n27_92 < 8; num_grid_n27_92 = num_grid_n27_92 + 1) {
        Zero_ciph(&result_grid_n27_93);
        for (cap_block_n27_94 = 0; cap_block_n27_94 < 9; cap_block_n27_94 = cap_block_n27_94 + 1) {
          _ckks_gen_tmp_1518_382 = input_grid_n27_91[cap_block_n27_94];
          Pt_from_msg_ofst(&_pgen_tmp_113_579, 24/* cst_200 */, ((((mesh_x_n5_86 * 8) + mesh_y_n5_87) * 72) + (cap_block_n27_94 + (num_grid_n27_92 * 9))) * (25088), 25088, 1, 3);
          Init_ciph_up_scale_plain(&_pgen_tmp_114_580, &_ckks_gen_tmp_1518_382, &_pgen_tmp_113_579);
          Init_ciph_same_scale(&result_grid_n27_93, &result_grid_n27_93, &_pgen_tmp_114_580);
          {
            _pgen_modulus_463 = Q_modulus();
            _pgen_num_q_464 = Poly_level(&result_grid_n27_93._c0_poly);
            for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
              Hw_modmul(Coeffs(&_pgen_tmp_114_580._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_1518_382._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_113_579._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modmul(Coeffs(&_pgen_tmp_114_580._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_1518_382._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_113_579._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n27_93._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n27_93._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_114_580._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n27_93._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n27_93._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_114_580._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              _pgen_modulus_463 = _pgen_modulus_463 + 1;
            }
          }
        }
        _preg_268435570 = Rotate(result_grid_n27_93, num_grid_n27_92 * 3136);
        Init_ciph_same_scale(&result_n27_88, &result_n27_88, &_preg_268435570);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&result_n27_88._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modadd(Coeffs(&result_n27_88._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n27_88._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435570._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&result_n27_88._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n27_88._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435570._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435570._c1_poly);
        Free_poly_data(&_preg_268435570._c0_poly);
      }
      Init_ciph_down_scale(&_pgen_tmp_115_581, &result_n27_88);
      Rescale(&_pgen_tmp_115_581._c0_poly, &result_n27_88._c0_poly);
      Rescale(&_pgen_tmp_115_581._c1_poly, &result_n27_88._c1_poly);
      Pt_from_msg(&_pgen_tmp_116_582, 25 /* cst_204 */, 25088, 1, 2);
      Init_ciph_up_scale_plain(&_preg_268435474, &_pgen_tmp_115_581, &_pgen_tmp_116_582);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435474._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_preg_268435474._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_115_581._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_116_582._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_preg_268435474._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_115_581._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_116_582._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      _preg_268435571 = Rotate(result_n27_88, -25088);
      Copy_ciph(&_ckks_gen_tmp_1570_383, &_preg_268435571);
      Init_ciph_down_scale(&_pgen_tmp_117_583, &_ckks_gen_tmp_1570_383);
      Rescale(&_pgen_tmp_117_583._c0_poly, &_ckks_gen_tmp_1570_383._c0_poly);
      Rescale(&_pgen_tmp_117_583._c1_poly, &_ckks_gen_tmp_1570_383._c1_poly);
      Pt_from_msg(&_pgen_tmp_118_584, 26 /* cst_205 */, 25088, 1, 2);
      Init_ciph_up_scale_plain(&_pgen_tmp_119_585, &_pgen_tmp_117_583, &_pgen_tmp_118_584);
      Init_ciph_same_scale(&_preg_268435474, &_preg_268435474, &_pgen_tmp_119_585);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435474._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_119_585._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_117_583._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_118_584._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_119_585._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_117_583._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_118_584._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435474._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435474._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_119_585._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435474._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435474._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_119_585._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Pt_from_msg(&_pgen_tmp_120_586, 27 /* cst_202 */, 25088, 2, 2);
      Init_ciph_same_scale_plain(&result_n27_88, &_preg_268435474, &_pgen_tmp_120_586);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&result_n27_88._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&result_n27_88._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435474._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_120_586._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Set_coeffs(&result_n27_88._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_preg_268435474._c1_poly, _pgen_rns_idx_465, degree));
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      _pgen_tmp_122_588 = VS_out_conv5_85[mesh_x_n5_86];
      Init_ciph_same_scale(&_pgen_tmp_121_587, &_pgen_tmp_122_588, &result_n27_88);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_121_587._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_121_587._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_122_588._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n27_88._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_121_587._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_122_588._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n27_88._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_conv5_85[mesh_x_n5_86], &_pgen_tmp_121_587);
      Tm_taken("Tensor::conv");
      Tm_taken("Tensor::add");
      Free_poly_data(&_preg_268435571._c1_poly);
      Free_poly_data(&_preg_268435571._c0_poly);
    }
  }
  Free_ciph_poly(VS_out_relu4_82, 8);
  Free_poly_data(&_preg_268435473._c1_poly);
  Free_poly_data(&_preg_268435473._c0_poly);
  Free_poly_data(&input_dup_n27_89._c1_poly);
  Free_poly_data(&input_dup_n27_89._c0_poly);
  Free_ciph_poly(input_grid_n27_91, 9);
  Free_poly_data(&_pgen_tmp_113_579._poly);
  Free_poly_data(&_pgen_tmp_114_580._c1_poly);
  Free_poly_data(&_pgen_tmp_114_580._c0_poly);
  Free_poly_data(&result_grid_n27_93._c1_poly);
  Free_poly_data(&result_grid_n27_93._c0_poly);
  Free_poly_data(&_pgen_tmp_116_582._poly);
  Free_poly_data(&_pgen_tmp_115_581._c1_poly);
  Free_poly_data(&_pgen_tmp_115_581._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_1570_383._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_1570_383._c0_poly);
  Free_poly_data(&_pgen_tmp_118_584._poly);
  Free_poly_data(&_pgen_tmp_117_583._c1_poly);
  Free_poly_data(&_pgen_tmp_117_583._c0_poly);
  Free_poly_data(&_pgen_tmp_119_585._c1_poly);
  Free_poly_data(&_pgen_tmp_119_585._c0_poly);
  Free_poly_data(&_pgen_tmp_120_586._poly);
  Free_poly_data(&_preg_268435474._c1_poly);
  Free_poly_data(&_preg_268435474._c0_poly);
  Free_poly_data(&result_n27_88._c1_poly);
  Free_poly_data(&result_n27_88._c0_poly);
  Free_poly_data(&_pgen_tmp_121_587._c1_poly);
  Free_poly_data(&_pgen_tmp_121_587._c0_poly);
  for (rescale_iv_450 = 0; rescale_iv_450 < 8; rescale_iv_450 = rescale_iv_450 + 1) {
    _pgen_tmp_123_589 = VS_out_conv5_85[rescale_iv_450];
    Init_ciph_down_scale(&_pgen_tmp_124_590, &_pgen_tmp_123_589);
    Rescale(&_pgen_tmp_124_590._c0_poly, &_pgen_tmp_123_589._c0_poly);
    Rescale(&_pgen_tmp_124_590._c1_poly, &_pgen_tmp_123_589._c1_poly);
    Copy_ciph(&VS_out_conv5_85[rescale_iv_450], &_pgen_tmp_124_590);
  }
  Free_poly_data(&_pgen_tmp_124_590._c1_poly);
  Free_poly_data(&_pgen_tmp_124_590._c0_poly);
  for (add_bias_n5_95 = 0; add_bias_n5_95 < 8; add_bias_n5_95 = add_bias_n5_95 + 1) {
    Tm_start("Tensor::add");
    _ckks_gen_tmp_1619_384 = VS_out_conv5_85[add_bias_n5_95];
    Pt_from_msg_ofst(&_pgen_tmp_126_592, 28/* cst_207 */, (add_bias_n5_95) * (25088), 25088, 1, 1);
    Init_ciph_same_scale_plain(&_pgen_tmp_125_591, &_ckks_gen_tmp_1619_384, &_pgen_tmp_126_592);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&_pgen_tmp_125_591._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_125_591._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_1619_384._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_126_592._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Set_coeffs(&_pgen_tmp_125_591._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_ckks_gen_tmp_1619_384._c1_poly, _pgen_rns_idx_465, degree));
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    Copy_ciph(&addshard5_96[add_bias_n5_95], &_pgen_tmp_125_591);
    Tm_taken("Tensor::add");
  }
  Free_ciph_poly(VS_out_conv5_85, 8);
  Free_poly_data(&_pgen_tmp_126_592._poly);
  Free_poly_data(&_pgen_tmp_125_591._c1_poly);
  Free_poly_data(&_pgen_tmp_125_591._c0_poly);
  memset(&VS_out_add5_97, 0, sizeof(VS_out_add5_97));
  for (mesh_x_n5_98 = 0; mesh_x_n5_98 < 8; mesh_x_n5_98 = mesh_x_n5_98 + 1) {
    for (mesh_y_n5_99 = 0; mesh_y_n5_99 < 1; mesh_y_n5_99 = mesh_y_n5_99 + 1) {
      Tm_start("Tensor::add");
      Tm_start("Tensor::add");
      _pgen_tmp_128_594 = VS_out_add5_97[((mesh_x_n5_98 * 1) + mesh_y_n5_99)];
      _pgen_tmp_129_595 = addshard5_96[((mesh_x_n5_98 * 1) + mesh_y_n5_99)];
      _pgen_tmp_130_596 = VS_out_relu3_67[((mesh_x_n5_98 * 1) + mesh_y_n5_99)];
      Init_ciph_same_scale(&_pgen_tmp_131_597, &_pgen_tmp_129_595, &_pgen_tmp_130_596);
      Init_ciph_same_scale(&_pgen_tmp_127_593, &_pgen_tmp_128_594, &_pgen_tmp_131_597);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_127_593._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_131_597._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_129_595._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_130_596._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_131_597._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_129_595._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_130_596._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_127_593._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_128_594._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_131_597._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_127_593._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_128_594._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_131_597._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_add5_97[((mesh_x_n5_98 * 1) + mesh_y_n5_99)], &_pgen_tmp_127_593);
      Tm_taken("Tensor::add");
      Tm_taken("Tensor::add");
    }
  }
  Free_ciph_poly(addshard5_96, 8);
  Free_ciph_poly(VS_out_relu3_67, 8);
  Free_poly_data(&_pgen_tmp_131_597._c1_poly);
  Free_poly_data(&_pgen_tmp_131_597._c0_poly);
  Free_poly_data(&_pgen_tmp_127_593._c1_poly);
  Free_poly_data(&_pgen_tmp_127_593._c0_poly);
  memset(&VS_out_relu5_100, 0, sizeof(VS_out_relu5_100));
  for (mesh_x_n5_101 = 0; mesh_x_n5_101 < 8; mesh_x_n5_101 = mesh_x_n5_101 + 1) {
    for (mesh_y_n5_102 = 0; mesh_y_n5_102 < 1; mesh_y_n5_102 = mesh_y_n5_102 + 1) {
      Tm_start("Tensor::add");
      _pgen_tmp_132_598 = VS_out_add5_97[((mesh_x_n5_101 * 1) + mesh_y_n5_102)];
      Bootstrap(&_preg_268435475, &_pgen_tmp_132_598, 18);
      Encode_float(&_pgen_tmp_134_600, &_cst_466, 1, 1, Level(&_preg_268435475));
      Init_ciph_up_scale_plain(&_pgen_tmp_133_599, &_preg_268435475, &_pgen_tmp_134_600);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_133_599._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_133_599._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435475._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_134_600._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_133_599._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435475._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_134_600._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Init_ciph_down_scale(&_pgen_tmp_135_601, &_pgen_tmp_133_599);
      Rescale(&_pgen_tmp_135_601._c0_poly, &_pgen_tmp_133_599._c0_poly);
      Rescale(&_pgen_tmp_135_601._c1_poly, &_pgen_tmp_133_599._c1_poly);
      _preg_268435476 = App_relu(_preg_268435475, _pgen_tmp_135_601, 25088);
      _pgen_tmp_137_603 = VS_out_relu5_100[((mesh_x_n5_101 * 1) + mesh_y_n5_102)];
      Init_ciph_same_scale(&_pgen_tmp_136_602, &_pgen_tmp_137_603, &_preg_268435476);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_136_602._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_136_602._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_137_603._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435476._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_136_602._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_137_603._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435476._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_relu5_100[((mesh_x_n5_101 * 1) + mesh_y_n5_102)], &_pgen_tmp_136_602);
      Tm_taken("Tensor::add");
    }
  }
  Free_ciph_poly(VS_out_add5_97, 8);
  Free_poly_data(&_pgen_tmp_134_600._poly);
  Free_poly_data(&_pgen_tmp_133_599._c1_poly);
  Free_poly_data(&_pgen_tmp_133_599._c0_poly);
  Free_poly_data(&_pgen_tmp_135_601._c1_poly);
  Free_poly_data(&_pgen_tmp_135_601._c0_poly);
  Free_poly_data(&_preg_268435475._c1_poly);
  Free_poly_data(&_preg_268435475._c0_poly);
  Free_poly_data(&_preg_268435476._c1_poly);
  Free_poly_data(&_preg_268435476._c0_poly);
  Free_poly_data(&_pgen_tmp_136_602._c1_poly);
  Free_poly_data(&_pgen_tmp_136_602._c0_poly);
  memset(&VS_out_conv6_103, 0, sizeof(VS_out_conv6_103));
  for (mesh_x_n6_104 = 0; mesh_x_n6_104 < 16; mesh_x_n6_104 = mesh_x_n6_104 + 1) {
    for (mesh_y_n6_105 = 0; mesh_y_n6_105 < 8; mesh_y_n6_105 = mesh_y_n6_105 + 1) {
      Tm_start("Tensor::add");
      Tm_start("Tensor::conv");
      Copy_ciph(&_preg_268435477, &VS_out_relu5_100[mesh_y_n6_105]);
      Zero_ciph(&result_n28_106);
      Copy_ciph(&input_dup_n28_107, &_preg_268435477);
      for (cap_block_input_grid_n28_108 = 0; cap_block_input_grid_n28_108 < 1; cap_block_input_grid_n28_108 = cap_block_input_grid_n28_108 + 1) {
        _preg_268435572 = Rotate(input_dup_n28_107, _cst_217[cap_block_input_grid_n28_108]);
        Copy_ciph(&input_grid_n28_109[cap_block_input_grid_n28_108], &_preg_268435572);
        Free_poly_data(&_preg_268435572._c1_poly);
        Free_poly_data(&_preg_268435572._c0_poly);
      }
      for (num_grid_n28_110 = 0; num_grid_n28_110 < 8; num_grid_n28_110 = num_grid_n28_110 + 1) {
        Zero_ciph(&result_grid_n28_111);
        for (cap_block_n28_112 = 0; cap_block_n28_112 < 1; cap_block_n28_112 = cap_block_n28_112 + 1) {
          _ckks_gen_tmp_1832_385 = input_grid_n28_109[cap_block_n28_112];
          Pt_from_msg_ofst(&_pgen_tmp_138_604, 29/* cst_214 */, ((((mesh_x_n6_104 * 8) + mesh_y_n6_105) * 8) + (cap_block_n28_112 + (num_grid_n28_110 * 1))) * (25088), 25088, 1, 6);
          Init_ciph_up_scale_plain(&_pgen_tmp_139_605, &_ckks_gen_tmp_1832_385, &_pgen_tmp_138_604);
          Init_ciph_same_scale(&result_grid_n28_111, &result_grid_n28_111, &_pgen_tmp_139_605);
          {
            _pgen_modulus_463 = Q_modulus();
            _pgen_num_q_464 = Poly_level(&result_grid_n28_111._c0_poly);
            for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
              Hw_modmul(Coeffs(&_pgen_tmp_139_605._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_1832_385._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_138_604._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modmul(Coeffs(&_pgen_tmp_139_605._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_1832_385._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_138_604._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n28_111._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n28_111._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_139_605._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n28_111._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n28_111._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_139_605._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              _pgen_modulus_463 = _pgen_modulus_463 + 1;
            }
          }
        }
        _preg_268435573 = Rotate(result_grid_n28_111, num_grid_n28_110 * 3136);
        Init_ciph_same_scale(&result_n28_106, &result_n28_106, &_preg_268435573);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&result_n28_106._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modadd(Coeffs(&result_n28_106._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n28_106._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435573._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&result_n28_106._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n28_106._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435573._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435573._c1_poly);
        Free_poly_data(&_preg_268435573._c0_poly);
      }
      Init_ciph_down_scale(&_pgen_tmp_140_606, &result_n28_106);
      Rescale(&_pgen_tmp_140_606._c0_poly, &result_n28_106._c0_poly);
      Rescale(&_pgen_tmp_140_606._c1_poly, &result_n28_106._c1_poly);
      Pt_from_msg(&_pgen_tmp_141_607, 30 /* cst_218 */, 25088, 1, 5);
      Init_ciph_up_scale_plain(&_preg_268435478, &_pgen_tmp_140_606, &_pgen_tmp_141_607);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435478._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_preg_268435478._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_140_606._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_141_607._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_preg_268435478._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_140_606._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_141_607._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      _preg_268435574 = Rotate(result_n28_106, -25088);
      Copy_ciph(&_ckks_gen_tmp_1884_386, &_preg_268435574);
      Init_ciph_down_scale(&_pgen_tmp_142_608, &_ckks_gen_tmp_1884_386);
      Rescale(&_pgen_tmp_142_608._c0_poly, &_ckks_gen_tmp_1884_386._c0_poly);
      Rescale(&_pgen_tmp_142_608._c1_poly, &_ckks_gen_tmp_1884_386._c1_poly);
      Pt_from_msg(&_pgen_tmp_143_609, 31 /* cst_219 */, 25088, 1, 5);
      Init_ciph_up_scale_plain(&_pgen_tmp_144_610, &_pgen_tmp_142_608, &_pgen_tmp_143_609);
      Init_ciph_same_scale(&_preg_268435478, &_preg_268435478, &_pgen_tmp_144_610);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435478._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_144_610._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_142_608._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_143_609._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_144_610._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_142_608._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_143_609._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435478._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435478._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_144_610._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435478._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435478._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_144_610._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Pt_from_msg(&_pgen_tmp_145_611, 32 /* cst_216 */, 25088, 2, 5);
      Init_ciph_same_scale_plain(&result_n28_106, &_preg_268435478, &_pgen_tmp_145_611);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&result_n28_106._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&result_n28_106._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435478._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_145_611._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Set_coeffs(&result_n28_106._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_preg_268435478._c1_poly, _pgen_rns_idx_465, degree));
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&orig_result28_113, &result_n28_106);
      Zero_ciph(&comb_row_result29_114);
      for (combine_row_index_n29_115 = 0; combine_row_index_n29_115 < 28; combine_row_index_n29_115 = combine_row_index_n29_115 + 1) {
        _preg_268435575 = Rotate(orig_result28_113, combine_row_index_n29_115 * 1);
        Copy_ciph(&_ckks_gen_tmp_1926_387, &_preg_268435575);
        Init_ciph_down_scale(&_pgen_tmp_146_612, &_ckks_gen_tmp_1926_387);
        Rescale(&_pgen_tmp_146_612._c0_poly, &_ckks_gen_tmp_1926_387._c0_poly);
        Rescale(&_pgen_tmp_146_612._c1_poly, &_ckks_gen_tmp_1926_387._c1_poly);
        Pt_from_msg_ofst(&_pgen_tmp_147_613, 33/* cst_220 */, (combine_row_index_n29_115) * (25088), 25088, 1, 4);
        Init_ciph_up_scale_plain(&_pgen_tmp_148_614, &_pgen_tmp_146_612, &_pgen_tmp_147_613);
        Init_ciph_same_scale(&comb_row_result29_114, &comb_row_result29_114, &_pgen_tmp_148_614);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&comb_row_result29_114._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modmul(Coeffs(&_pgen_tmp_148_614._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_146_612._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_147_613._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modmul(Coeffs(&_pgen_tmp_148_614._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_146_612._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_147_613._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&comb_row_result29_114._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_row_result29_114._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_148_614._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&comb_row_result29_114._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_row_result29_114._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_148_614._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435575._c1_poly);
        Free_poly_data(&_preg_268435575._c0_poly);
      }
      Zero_ciph(&comb_rc_result29_116);
      for (combine_rc_index_n29_117 = 0; combine_rc_index_n29_117 < 28; combine_rc_index_n29_117 = combine_rc_index_n29_117 + 1) {
        _preg_268435576 = Rotate(comb_row_result29_114, combine_rc_index_n29_117 * 84);
        Copy_ciph(&_ckks_gen_tmp_1959_388, &_preg_268435576);
        Init_ciph_down_scale(&_pgen_tmp_149_615, &_ckks_gen_tmp_1959_388);
        Rescale(&_pgen_tmp_149_615._c0_poly, &_ckks_gen_tmp_1959_388._c0_poly);
        Rescale(&_pgen_tmp_149_615._c1_poly, &_ckks_gen_tmp_1959_388._c1_poly);
        Pt_from_msg_ofst(&_pgen_tmp_150_616, 34/* cst_221 */, (combine_rc_index_n29_117) * (25088), 25088, 1, 3);
        Init_ciph_up_scale_plain(&_pgen_tmp_151_617, &_pgen_tmp_149_615, &_pgen_tmp_150_616);
        Init_ciph_same_scale(&comb_rc_result29_116, &comb_rc_result29_116, &_pgen_tmp_151_617);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&comb_rc_result29_116._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modmul(Coeffs(&_pgen_tmp_151_617._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_149_615._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_150_616._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modmul(Coeffs(&_pgen_tmp_151_617._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_149_615._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_150_616._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&comb_rc_result29_116._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_rc_result29_116._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_151_617._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&comb_rc_result29_116._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_rc_result29_116._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_151_617._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435576._c1_poly);
        Free_poly_data(&_preg_268435576._c0_poly);
      }
      Zero_ciph(&final_result29_118);
      for (combine_cc_index_n29_119 = 0; combine_cc_index_n29_119 < 8; combine_cc_index_n29_119 = combine_cc_index_n29_119 + 1) {
        _preg_268435577 = Rotate(comb_rc_result29_116, combine_cc_index_n29_119 * 2352);
        Copy_ciph(&_ckks_gen_tmp_1992_389, &_preg_268435577);
        Init_ciph_down_scale(&_pgen_tmp_152_618, &_ckks_gen_tmp_1992_389);
        Rescale(&_pgen_tmp_152_618._c0_poly, &_ckks_gen_tmp_1992_389._c0_poly);
        Rescale(&_pgen_tmp_152_618._c1_poly, &_ckks_gen_tmp_1992_389._c1_poly);
        Pt_from_msg_ofst(&_pgen_tmp_153_619, 35/* cst_222 */, (combine_cc_index_n29_119) * (25088), 25088, 1, 2);
        Init_ciph_up_scale_plain(&_pgen_tmp_154_620, &_pgen_tmp_152_618, &_pgen_tmp_153_619);
        Init_ciph_same_scale(&final_result29_118, &final_result29_118, &_pgen_tmp_154_620);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&final_result29_118._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modmul(Coeffs(&_pgen_tmp_154_620._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_152_618._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_153_619._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modmul(Coeffs(&_pgen_tmp_154_620._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_152_618._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_153_619._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&final_result29_118._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&final_result29_118._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_154_620._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&final_result29_118._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&final_result29_118._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_154_620._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435577._c1_poly);
        Free_poly_data(&_preg_268435577._c0_poly);
      }
      _pgen_tmp_156_622 = VS_out_conv6_103[mesh_x_n6_104];
      Init_ciph_same_scale(&_pgen_tmp_155_621, &_pgen_tmp_156_622, &final_result29_118);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_155_621._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_155_621._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_156_622._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&final_result29_118._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_155_621._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_156_622._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&final_result29_118._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_conv6_103[mesh_x_n6_104], &_pgen_tmp_155_621);
      Tm_taken("Tensor::conv");
      Tm_taken("Tensor::add");
      Free_poly_data(&_preg_268435574._c1_poly);
      Free_poly_data(&_preg_268435574._c0_poly);
    }
  }
  Free_poly_data(&_preg_268435477._c1_poly);
  Free_poly_data(&_preg_268435477._c0_poly);
  Free_poly_data(&input_dup_n28_107._c1_poly);
  Free_poly_data(&input_dup_n28_107._c0_poly);
  Free_ciph_poly(input_grid_n28_109, 1);
  Free_poly_data(&_pgen_tmp_138_604._poly);
  Free_poly_data(&_pgen_tmp_139_605._c1_poly);
  Free_poly_data(&_pgen_tmp_139_605._c0_poly);
  Free_poly_data(&result_grid_n28_111._c1_poly);
  Free_poly_data(&result_grid_n28_111._c0_poly);
  Free_poly_data(&_pgen_tmp_141_607._poly);
  Free_poly_data(&_pgen_tmp_140_606._c1_poly);
  Free_poly_data(&_pgen_tmp_140_606._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_1884_386._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_1884_386._c0_poly);
  Free_poly_data(&_pgen_tmp_143_609._poly);
  Free_poly_data(&_pgen_tmp_142_608._c1_poly);
  Free_poly_data(&_pgen_tmp_142_608._c0_poly);
  Free_poly_data(&_pgen_tmp_144_610._c1_poly);
  Free_poly_data(&_pgen_tmp_144_610._c0_poly);
  Free_poly_data(&_pgen_tmp_145_611._poly);
  Free_poly_data(&_preg_268435478._c1_poly);
  Free_poly_data(&_preg_268435478._c0_poly);
  Free_poly_data(&result_n28_106._c1_poly);
  Free_poly_data(&result_n28_106._c0_poly);
  Free_poly_data(&orig_result28_113._c1_poly);
  Free_poly_data(&orig_result28_113._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_1926_387._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_1926_387._c0_poly);
  Free_poly_data(&_pgen_tmp_147_613._poly);
  Free_poly_data(&_pgen_tmp_146_612._c1_poly);
  Free_poly_data(&_pgen_tmp_146_612._c0_poly);
  Free_poly_data(&_pgen_tmp_148_614._c1_poly);
  Free_poly_data(&_pgen_tmp_148_614._c0_poly);
  Free_poly_data(&comb_row_result29_114._c1_poly);
  Free_poly_data(&comb_row_result29_114._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_1959_388._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_1959_388._c0_poly);
  Free_poly_data(&_pgen_tmp_150_616._poly);
  Free_poly_data(&_pgen_tmp_149_615._c1_poly);
  Free_poly_data(&_pgen_tmp_149_615._c0_poly);
  Free_poly_data(&_pgen_tmp_151_617._c1_poly);
  Free_poly_data(&_pgen_tmp_151_617._c0_poly);
  Free_poly_data(&comb_rc_result29_116._c1_poly);
  Free_poly_data(&comb_rc_result29_116._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_1992_389._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_1992_389._c0_poly);
  Free_poly_data(&_pgen_tmp_153_619._poly);
  Free_poly_data(&_pgen_tmp_152_618._c1_poly);
  Free_poly_data(&_pgen_tmp_152_618._c0_poly);
  Free_poly_data(&_pgen_tmp_154_620._c1_poly);
  Free_poly_data(&_pgen_tmp_154_620._c0_poly);
  Free_poly_data(&final_result29_118._c1_poly);
  Free_poly_data(&final_result29_118._c0_poly);
  Free_poly_data(&_pgen_tmp_155_621._c1_poly);
  Free_poly_data(&_pgen_tmp_155_621._c0_poly);
  for (rescale_iv_451 = 0; rescale_iv_451 < 16; rescale_iv_451 = rescale_iv_451 + 1) {
    _pgen_tmp_157_623 = VS_out_conv6_103[rescale_iv_451];
    Init_ciph_down_scale(&_pgen_tmp_158_624, &_pgen_tmp_157_623);
    Rescale(&_pgen_tmp_158_624._c0_poly, &_pgen_tmp_157_623._c0_poly);
    Rescale(&_pgen_tmp_158_624._c1_poly, &_pgen_tmp_157_623._c1_poly);
    Copy_ciph(&VS_out_conv6_103[rescale_iv_451], &_pgen_tmp_158_624);
  }
  Free_poly_data(&_pgen_tmp_158_624._c1_poly);
  Free_poly_data(&_pgen_tmp_158_624._c0_poly);
  for (reshardm_n6_120 = 0; reshardm_n6_120 < 4; reshardm_n6_120 = reshardm_n6_120 + 1) {
    Tm_start("Tensor::concat");
    Tm_start("Tensor::concat");
    Tm_start("Tensor::concat");
    _pgen_tmp_160_626 = VS_out_conv6_103[(reshardm_n6_120 * 4)];
    _pgen_tmp_161_627 = VS_out_conv6_103[((reshardm_n6_120 * 4) + 1)];
    _preg_268435578 = Rotate(_pgen_tmp_161_627, -6272);
    Init_ciph_same_scale(&_pgen_tmp_162_628, &_pgen_tmp_160_626, &_preg_268435578);
    _pgen_tmp_164_630 = VS_out_conv6_103[((reshardm_n6_120 * 4) + 2)];
    _pgen_tmp_165_631 = VS_out_conv6_103[((reshardm_n6_120 * 4) + 3)];
    _preg_268435579 = Rotate(_pgen_tmp_165_631, -6272);
    Init_ciph_same_scale(&_pgen_tmp_163_629, &_pgen_tmp_164_630, &_preg_268435579);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&_pgen_tmp_163_629._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_163_629._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_164_630._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435579._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_163_629._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_164_630._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435579._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    _preg_268435580 = Rotate(_pgen_tmp_163_629, -12544);
    Init_ciph_same_scale(&_pgen_tmp_159_625, &_pgen_tmp_162_628, &_preg_268435580);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&_pgen_tmp_159_625._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_162_628._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_160_626._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435578._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_162_628._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_160_626._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435578._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_159_625._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_162_628._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435580._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_159_625._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_162_628._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435580._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    Copy_ciph(&output_reshard6_121[reshardm_n6_120], &_pgen_tmp_159_625);
    Tm_taken("Tensor::concat");
    Tm_taken("Tensor::concat");
    Tm_taken("Tensor::concat");
    Free_poly_data(&_preg_268435578._c1_poly);
    Free_poly_data(&_preg_268435578._c0_poly);
    Free_poly_data(&_preg_268435579._c1_poly);
    Free_poly_data(&_preg_268435579._c0_poly);
    Free_poly_data(&_preg_268435580._c1_poly);
    Free_poly_data(&_preg_268435580._c0_poly);
  }
  Free_ciph_poly(VS_out_conv6_103, 16);
  Free_poly_data(&_pgen_tmp_163_629._c1_poly);
  Free_poly_data(&_pgen_tmp_163_629._c0_poly);
  Free_poly_data(&_pgen_tmp_162_628._c1_poly);
  Free_poly_data(&_pgen_tmp_162_628._c0_poly);
  Free_poly_data(&_pgen_tmp_159_625._c1_poly);
  Free_poly_data(&_pgen_tmp_159_625._c0_poly);
  for (add_bias_n6_122 = 0; add_bias_n6_122 < 4; add_bias_n6_122 = add_bias_n6_122 + 1) {
    Tm_start("Tensor::add");
    _ckks_gen_tmp_2093_390 = output_reshard6_121[add_bias_n6_122];
    Pt_from_msg_ofst(&_pgen_tmp_167_633, 36/* cst_227 */, (add_bias_n6_122) * (25088), 25088, 1, 1);
    Init_ciph_same_scale_plain(&_pgen_tmp_166_632, &_ckks_gen_tmp_2093_390, &_pgen_tmp_167_633);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&_pgen_tmp_166_632._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_166_632._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_2093_390._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_167_633._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Set_coeffs(&_pgen_tmp_166_632._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_ckks_gen_tmp_2093_390._c1_poly, _pgen_rns_idx_465, degree));
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    Copy_ciph(&addshard6_123[add_bias_n6_122], &_pgen_tmp_166_632);
    Tm_taken("Tensor::add");
  }
  Free_ciph_poly(output_reshard6_121, 4);
  Free_poly_data(&_pgen_tmp_167_633._poly);
  Free_poly_data(&_pgen_tmp_166_632._c1_poly);
  Free_poly_data(&_pgen_tmp_166_632._c0_poly);
  memset(&VS_out_conv7_124, 0, sizeof(VS_out_conv7_124));
  for (mesh_x_n7_125 = 0; mesh_x_n7_125 < 16; mesh_x_n7_125 = mesh_x_n7_125 + 1) {
    for (mesh_y_n7_126 = 0; mesh_y_n7_126 < 8; mesh_y_n7_126 = mesh_y_n7_126 + 1) {
      Tm_start("Tensor::add");
      Tm_start("Tensor::conv");
      Copy_ciph(&_preg_268435479, &VS_out_relu5_100[mesh_y_n7_126]);
      Zero_ciph(&result_n30_127);
      Copy_ciph(&input_dup_n30_128, &_preg_268435479);
      for (cap_block_input_grid_n30_129 = 0; cap_block_input_grid_n30_129 < 9; cap_block_input_grid_n30_129 = cap_block_input_grid_n30_129 + 1) {
        _preg_268435581 = Rotate(input_dup_n30_128, _cst_234[cap_block_input_grid_n30_129]);
        Copy_ciph(&input_grid_n30_130[cap_block_input_grid_n30_129], &_preg_268435581);
        Free_poly_data(&_preg_268435581._c1_poly);
        Free_poly_data(&_preg_268435581._c0_poly);
      }
      for (num_grid_n30_131 = 0; num_grid_n30_131 < 8; num_grid_n30_131 = num_grid_n30_131 + 1) {
        Zero_ciph(&result_grid_n30_132);
        for (cap_block_n30_133 = 0; cap_block_n30_133 < 9; cap_block_n30_133 = cap_block_n30_133 + 1) {
          _ckks_gen_tmp_2188_391 = input_grid_n30_130[cap_block_n30_133];
          Pt_from_msg_ofst(&_pgen_tmp_168_634, 37/* cst_231 */, ((((mesh_x_n7_125 * 8) + mesh_y_n7_126) * 72) + (cap_block_n30_133 + (num_grid_n30_131 * 9))) * (25088), 25088, 1, 6);
          Init_ciph_up_scale_plain(&_pgen_tmp_169_635, &_ckks_gen_tmp_2188_391, &_pgen_tmp_168_634);
          Init_ciph_same_scale(&result_grid_n30_132, &result_grid_n30_132, &_pgen_tmp_169_635);
          {
            _pgen_modulus_463 = Q_modulus();
            _pgen_num_q_464 = Poly_level(&result_grid_n30_132._c0_poly);
            for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
              Hw_modmul(Coeffs(&_pgen_tmp_169_635._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_2188_391._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_168_634._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modmul(Coeffs(&_pgen_tmp_169_635._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_2188_391._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_168_634._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n30_132._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n30_132._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_169_635._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n30_132._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n30_132._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_169_635._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              _pgen_modulus_463 = _pgen_modulus_463 + 1;
            }
          }
        }
        _preg_268435582 = Rotate(result_grid_n30_132, num_grid_n30_131 * 3136);
        Init_ciph_same_scale(&result_n30_127, &result_n30_127, &_preg_268435582);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&result_n30_127._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modadd(Coeffs(&result_n30_127._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n30_127._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435582._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&result_n30_127._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n30_127._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435582._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435582._c1_poly);
        Free_poly_data(&_preg_268435582._c0_poly);
      }
      Init_ciph_down_scale(&_pgen_tmp_170_636, &result_n30_127);
      Rescale(&_pgen_tmp_170_636._c0_poly, &result_n30_127._c0_poly);
      Rescale(&_pgen_tmp_170_636._c1_poly, &result_n30_127._c1_poly);
      Pt_from_msg(&_pgen_tmp_171_637, 38 /* cst_235 */, 25088, 1, 5);
      Init_ciph_up_scale_plain(&_preg_268435480, &_pgen_tmp_170_636, &_pgen_tmp_171_637);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435480._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_preg_268435480._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_170_636._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_171_637._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_preg_268435480._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_170_636._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_171_637._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      _preg_268435583 = Rotate(result_n30_127, -25088);
      Copy_ciph(&_ckks_gen_tmp_2240_392, &_preg_268435583);
      Init_ciph_down_scale(&_pgen_tmp_172_638, &_ckks_gen_tmp_2240_392);
      Rescale(&_pgen_tmp_172_638._c0_poly, &_ckks_gen_tmp_2240_392._c0_poly);
      Rescale(&_pgen_tmp_172_638._c1_poly, &_ckks_gen_tmp_2240_392._c1_poly);
      Pt_from_msg(&_pgen_tmp_173_639, 39 /* cst_236 */, 25088, 1, 5);
      Init_ciph_up_scale_plain(&_pgen_tmp_174_640, &_pgen_tmp_172_638, &_pgen_tmp_173_639);
      Init_ciph_same_scale(&_preg_268435480, &_preg_268435480, &_pgen_tmp_174_640);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435480._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_174_640._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_172_638._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_173_639._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_174_640._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_172_638._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_173_639._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435480._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435480._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_174_640._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435480._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435480._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_174_640._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Pt_from_msg(&_pgen_tmp_175_641, 40 /* cst_233 */, 25088, 2, 5);
      Init_ciph_same_scale_plain(&result_n30_127, &_preg_268435480, &_pgen_tmp_175_641);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&result_n30_127._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&result_n30_127._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435480._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_175_641._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Set_coeffs(&result_n30_127._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_preg_268435480._c1_poly, _pgen_rns_idx_465, degree));
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&orig_result30_134, &result_n30_127);
      Zero_ciph(&comb_row_result31_135);
      for (combine_row_index_n31_136 = 0; combine_row_index_n31_136 < 28; combine_row_index_n31_136 = combine_row_index_n31_136 + 1) {
        _preg_268435584 = Rotate(orig_result30_134, combine_row_index_n31_136 * 1);
        Copy_ciph(&_ckks_gen_tmp_2282_393, &_preg_268435584);
        Init_ciph_down_scale(&_pgen_tmp_176_642, &_ckks_gen_tmp_2282_393);
        Rescale(&_pgen_tmp_176_642._c0_poly, &_ckks_gen_tmp_2282_393._c0_poly);
        Rescale(&_pgen_tmp_176_642._c1_poly, &_ckks_gen_tmp_2282_393._c1_poly);
        Pt_from_msg_ofst(&_pgen_tmp_177_643, 41/* cst_237 */, (combine_row_index_n31_136) * (25088), 25088, 1, 4);
        Init_ciph_up_scale_plain(&_pgen_tmp_178_644, &_pgen_tmp_176_642, &_pgen_tmp_177_643);
        Init_ciph_same_scale(&comb_row_result31_135, &comb_row_result31_135, &_pgen_tmp_178_644);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&comb_row_result31_135._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modmul(Coeffs(&_pgen_tmp_178_644._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_176_642._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_177_643._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modmul(Coeffs(&_pgen_tmp_178_644._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_176_642._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_177_643._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&comb_row_result31_135._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_row_result31_135._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_178_644._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&comb_row_result31_135._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_row_result31_135._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_178_644._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435584._c1_poly);
        Free_poly_data(&_preg_268435584._c0_poly);
      }
      Zero_ciph(&comb_rc_result31_137);
      for (combine_rc_index_n31_138 = 0; combine_rc_index_n31_138 < 28; combine_rc_index_n31_138 = combine_rc_index_n31_138 + 1) {
        _preg_268435585 = Rotate(comb_row_result31_135, combine_rc_index_n31_138 * 84);
        Copy_ciph(&_ckks_gen_tmp_2315_394, &_preg_268435585);
        Init_ciph_down_scale(&_pgen_tmp_179_645, &_ckks_gen_tmp_2315_394);
        Rescale(&_pgen_tmp_179_645._c0_poly, &_ckks_gen_tmp_2315_394._c0_poly);
        Rescale(&_pgen_tmp_179_645._c1_poly, &_ckks_gen_tmp_2315_394._c1_poly);
        Pt_from_msg_ofst(&_pgen_tmp_180_646, 42/* cst_238 */, (combine_rc_index_n31_138) * (25088), 25088, 1, 3);
        Init_ciph_up_scale_plain(&_pgen_tmp_181_647, &_pgen_tmp_179_645, &_pgen_tmp_180_646);
        Init_ciph_same_scale(&comb_rc_result31_137, &comb_rc_result31_137, &_pgen_tmp_181_647);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&comb_rc_result31_137._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modmul(Coeffs(&_pgen_tmp_181_647._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_179_645._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_180_646._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modmul(Coeffs(&_pgen_tmp_181_647._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_179_645._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_180_646._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&comb_rc_result31_137._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_rc_result31_137._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_181_647._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&comb_rc_result31_137._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_rc_result31_137._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_181_647._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435585._c1_poly);
        Free_poly_data(&_preg_268435585._c0_poly);
      }
      Zero_ciph(&final_result31_139);
      for (combine_cc_index_n31_140 = 0; combine_cc_index_n31_140 < 8; combine_cc_index_n31_140 = combine_cc_index_n31_140 + 1) {
        _preg_268435586 = Rotate(comb_rc_result31_137, combine_cc_index_n31_140 * 2352);
        Copy_ciph(&_ckks_gen_tmp_2348_395, &_preg_268435586);
        Init_ciph_down_scale(&_pgen_tmp_182_648, &_ckks_gen_tmp_2348_395);
        Rescale(&_pgen_tmp_182_648._c0_poly, &_ckks_gen_tmp_2348_395._c0_poly);
        Rescale(&_pgen_tmp_182_648._c1_poly, &_ckks_gen_tmp_2348_395._c1_poly);
        Pt_from_msg_ofst(&_pgen_tmp_183_649, 43/* cst_239 */, (combine_cc_index_n31_140) * (25088), 25088, 1, 2);
        Init_ciph_up_scale_plain(&_pgen_tmp_184_650, &_pgen_tmp_182_648, &_pgen_tmp_183_649);
        Init_ciph_same_scale(&final_result31_139, &final_result31_139, &_pgen_tmp_184_650);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&final_result31_139._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modmul(Coeffs(&_pgen_tmp_184_650._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_182_648._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_183_649._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modmul(Coeffs(&_pgen_tmp_184_650._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_182_648._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_183_649._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&final_result31_139._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&final_result31_139._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_184_650._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&final_result31_139._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&final_result31_139._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_184_650._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435586._c1_poly);
        Free_poly_data(&_preg_268435586._c0_poly);
      }
      _pgen_tmp_186_652 = VS_out_conv7_124[mesh_x_n7_125];
      Init_ciph_same_scale(&_pgen_tmp_185_651, &_pgen_tmp_186_652, &final_result31_139);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_185_651._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_185_651._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_186_652._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&final_result31_139._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_185_651._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_186_652._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&final_result31_139._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_conv7_124[mesh_x_n7_125], &_pgen_tmp_185_651);
      Tm_taken("Tensor::conv");
      Tm_taken("Tensor::add");
      Free_poly_data(&_preg_268435583._c1_poly);
      Free_poly_data(&_preg_268435583._c0_poly);
    }
  }
  Free_ciph_poly(VS_out_relu5_100, 8);
  Free_poly_data(&_preg_268435479._c1_poly);
  Free_poly_data(&_preg_268435479._c0_poly);
  Free_poly_data(&input_dup_n30_128._c1_poly);
  Free_poly_data(&input_dup_n30_128._c0_poly);
  Free_ciph_poly(input_grid_n30_130, 9);
  Free_poly_data(&_pgen_tmp_168_634._poly);
  Free_poly_data(&_pgen_tmp_169_635._c1_poly);
  Free_poly_data(&_pgen_tmp_169_635._c0_poly);
  Free_poly_data(&result_grid_n30_132._c1_poly);
  Free_poly_data(&result_grid_n30_132._c0_poly);
  Free_poly_data(&_pgen_tmp_171_637._poly);
  Free_poly_data(&_pgen_tmp_170_636._c1_poly);
  Free_poly_data(&_pgen_tmp_170_636._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_2240_392._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_2240_392._c0_poly);
  Free_poly_data(&_pgen_tmp_173_639._poly);
  Free_poly_data(&_pgen_tmp_172_638._c1_poly);
  Free_poly_data(&_pgen_tmp_172_638._c0_poly);
  Free_poly_data(&_pgen_tmp_174_640._c1_poly);
  Free_poly_data(&_pgen_tmp_174_640._c0_poly);
  Free_poly_data(&_pgen_tmp_175_641._poly);
  Free_poly_data(&_preg_268435480._c1_poly);
  Free_poly_data(&_preg_268435480._c0_poly);
  Free_poly_data(&result_n30_127._c1_poly);
  Free_poly_data(&result_n30_127._c0_poly);
  Free_poly_data(&orig_result30_134._c1_poly);
  Free_poly_data(&orig_result30_134._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_2282_393._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_2282_393._c0_poly);
  Free_poly_data(&_pgen_tmp_177_643._poly);
  Free_poly_data(&_pgen_tmp_176_642._c1_poly);
  Free_poly_data(&_pgen_tmp_176_642._c0_poly);
  Free_poly_data(&_pgen_tmp_178_644._c1_poly);
  Free_poly_data(&_pgen_tmp_178_644._c0_poly);
  Free_poly_data(&comb_row_result31_135._c1_poly);
  Free_poly_data(&comb_row_result31_135._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_2315_394._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_2315_394._c0_poly);
  Free_poly_data(&_pgen_tmp_180_646._poly);
  Free_poly_data(&_pgen_tmp_179_645._c1_poly);
  Free_poly_data(&_pgen_tmp_179_645._c0_poly);
  Free_poly_data(&_pgen_tmp_181_647._c1_poly);
  Free_poly_data(&_pgen_tmp_181_647._c0_poly);
  Free_poly_data(&comb_rc_result31_137._c1_poly);
  Free_poly_data(&comb_rc_result31_137._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_2348_395._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_2348_395._c0_poly);
  Free_poly_data(&_pgen_tmp_183_649._poly);
  Free_poly_data(&_pgen_tmp_182_648._c1_poly);
  Free_poly_data(&_pgen_tmp_182_648._c0_poly);
  Free_poly_data(&_pgen_tmp_184_650._c1_poly);
  Free_poly_data(&_pgen_tmp_184_650._c0_poly);
  Free_poly_data(&final_result31_139._c1_poly);
  Free_poly_data(&final_result31_139._c0_poly);
  Free_poly_data(&_pgen_tmp_185_651._c1_poly);
  Free_poly_data(&_pgen_tmp_185_651._c0_poly);
  for (rescale_iv_452 = 0; rescale_iv_452 < 16; rescale_iv_452 = rescale_iv_452 + 1) {
    _pgen_tmp_187_653 = VS_out_conv7_124[rescale_iv_452];
    Init_ciph_down_scale(&_pgen_tmp_188_654, &_pgen_tmp_187_653);
    Rescale(&_pgen_tmp_188_654._c0_poly, &_pgen_tmp_187_653._c0_poly);
    Rescale(&_pgen_tmp_188_654._c1_poly, &_pgen_tmp_187_653._c1_poly);
    Copy_ciph(&VS_out_conv7_124[rescale_iv_452], &_pgen_tmp_188_654);
  }
  Free_poly_data(&_pgen_tmp_188_654._c1_poly);
  Free_poly_data(&_pgen_tmp_188_654._c0_poly);
  for (reshardm_n7_141 = 0; reshardm_n7_141 < 4; reshardm_n7_141 = reshardm_n7_141 + 1) {
    Tm_start("Tensor::concat");
    Tm_start("Tensor::concat");
    Tm_start("Tensor::concat");
    _pgen_tmp_190_656 = VS_out_conv7_124[(reshardm_n7_141 * 4)];
    _pgen_tmp_191_657 = VS_out_conv7_124[((reshardm_n7_141 * 4) + 1)];
    _preg_268435587 = Rotate(_pgen_tmp_191_657, -6272);
    Init_ciph_same_scale(&_pgen_tmp_192_658, &_pgen_tmp_190_656, &_preg_268435587);
    _pgen_tmp_194_660 = VS_out_conv7_124[((reshardm_n7_141 * 4) + 2)];
    _pgen_tmp_195_661 = VS_out_conv7_124[((reshardm_n7_141 * 4) + 3)];
    _preg_268435588 = Rotate(_pgen_tmp_195_661, -6272);
    Init_ciph_same_scale(&_pgen_tmp_193_659, &_pgen_tmp_194_660, &_preg_268435588);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&_pgen_tmp_193_659._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_193_659._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_194_660._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435588._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_193_659._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_194_660._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435588._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    _preg_268435589 = Rotate(_pgen_tmp_193_659, -12544);
    Init_ciph_same_scale(&_pgen_tmp_189_655, &_pgen_tmp_192_658, &_preg_268435589);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&_pgen_tmp_189_655._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_192_658._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_190_656._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435587._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_192_658._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_190_656._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435587._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_189_655._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_192_658._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435589._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_189_655._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_192_658._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435589._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    Copy_ciph(&output_reshard7_142[reshardm_n7_141], &_pgen_tmp_189_655);
    Tm_taken("Tensor::concat");
    Tm_taken("Tensor::concat");
    Tm_taken("Tensor::concat");
    Free_poly_data(&_preg_268435587._c1_poly);
    Free_poly_data(&_preg_268435587._c0_poly);
    Free_poly_data(&_preg_268435588._c1_poly);
    Free_poly_data(&_preg_268435588._c0_poly);
    Free_poly_data(&_preg_268435589._c1_poly);
    Free_poly_data(&_preg_268435589._c0_poly);
  }
  Free_ciph_poly(VS_out_conv7_124, 16);
  Free_poly_data(&_pgen_tmp_193_659._c1_poly);
  Free_poly_data(&_pgen_tmp_193_659._c0_poly);
  Free_poly_data(&_pgen_tmp_192_658._c1_poly);
  Free_poly_data(&_pgen_tmp_192_658._c0_poly);
  Free_poly_data(&_pgen_tmp_189_655._c1_poly);
  Free_poly_data(&_pgen_tmp_189_655._c0_poly);
  for (add_bias_n7_143 = 0; add_bias_n7_143 < 4; add_bias_n7_143 = add_bias_n7_143 + 1) {
    Tm_start("Tensor::add");
    _ckks_gen_tmp_2449_396 = output_reshard7_142[add_bias_n7_143];
    Pt_from_msg_ofst(&_pgen_tmp_197_663, 44/* cst_244 */, (add_bias_n7_143) * (25088), 25088, 1, 1);
    Init_ciph_same_scale_plain(&_pgen_tmp_196_662, &_ckks_gen_tmp_2449_396, &_pgen_tmp_197_663);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&_pgen_tmp_196_662._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_196_662._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_2449_396._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_197_663._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Set_coeffs(&_pgen_tmp_196_662._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_ckks_gen_tmp_2449_396._c1_poly, _pgen_rns_idx_465, degree));
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    Copy_ciph(&addshard7_144[add_bias_n7_143], &_pgen_tmp_196_662);
    Tm_taken("Tensor::add");
  }
  Free_ciph_poly(output_reshard7_142, 4);
  Free_poly_data(&_pgen_tmp_197_663._poly);
  Free_poly_data(&_pgen_tmp_196_662._c1_poly);
  Free_poly_data(&_pgen_tmp_196_662._c0_poly);
  memset(&VS_out_relu7_145, 0, sizeof(VS_out_relu7_145));
  for (mesh_x_n7_146 = 0; mesh_x_n7_146 < 4; mesh_x_n7_146 = mesh_x_n7_146 + 1) {
    for (mesh_y_n7_147 = 0; mesh_y_n7_147 < 1; mesh_y_n7_147 = mesh_y_n7_147 + 1) {
      Tm_start("Tensor::add");
      _pgen_tmp_198_664 = addshard7_144[((mesh_x_n7_146 * 1) + mesh_y_n7_147)];
      Bootstrap(&_preg_268435481, &_pgen_tmp_198_664, 15);
      Encode_float(&_pgen_tmp_200_666, &_cst_467, 1, 1, Level(&_preg_268435481));
      Init_ciph_up_scale_plain(&_pgen_tmp_199_665, &_preg_268435481, &_pgen_tmp_200_666);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_199_665._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_199_665._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435481._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_200_666._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_199_665._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435481._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_200_666._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Init_ciph_down_scale(&_pgen_tmp_201_667, &_pgen_tmp_199_665);
      Rescale(&_pgen_tmp_201_667._c0_poly, &_pgen_tmp_199_665._c0_poly);
      Rescale(&_pgen_tmp_201_667._c1_poly, &_pgen_tmp_199_665._c1_poly);
      _preg_268435482 = App_relu(_preg_268435481, _pgen_tmp_201_667, 25088);
      _pgen_tmp_203_669 = VS_out_relu7_145[((mesh_x_n7_146 * 1) + mesh_y_n7_147)];
      Init_ciph_same_scale(&_pgen_tmp_202_668, &_pgen_tmp_203_669, &_preg_268435482);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_202_668._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_202_668._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_203_669._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435482._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_202_668._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_203_669._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435482._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_relu7_145[((mesh_x_n7_146 * 1) + mesh_y_n7_147)], &_pgen_tmp_202_668);
      Tm_taken("Tensor::add");
    }
  }
  Free_ciph_poly(addshard7_144, 4);
  Free_poly_data(&_pgen_tmp_200_666._poly);
  Free_poly_data(&_pgen_tmp_199_665._c1_poly);
  Free_poly_data(&_pgen_tmp_199_665._c0_poly);
  Free_poly_data(&_pgen_tmp_201_667._c1_poly);
  Free_poly_data(&_pgen_tmp_201_667._c0_poly);
  Free_poly_data(&_preg_268435481._c1_poly);
  Free_poly_data(&_preg_268435481._c0_poly);
  Free_poly_data(&_preg_268435482._c1_poly);
  Free_poly_data(&_preg_268435482._c0_poly);
  Free_poly_data(&_pgen_tmp_202_668._c1_poly);
  Free_poly_data(&_pgen_tmp_202_668._c0_poly);
  memset(&VS_out_conv8_148, 0, sizeof(VS_out_conv8_148));
  for (mesh_x_n8_149 = 0; mesh_x_n8_149 < 4; mesh_x_n8_149 = mesh_x_n8_149 + 1) {
    for (mesh_y_n8_150 = 0; mesh_y_n8_150 < 4; mesh_y_n8_150 = mesh_y_n8_150 + 1) {
      Tm_start("Tensor::add");
      Tm_start("Tensor::conv");
      Copy_ciph(&_preg_268435483, &VS_out_relu7_145[mesh_y_n8_150]);
      Zero_ciph(&result_n32_151);
      Copy_ciph(&input_dup_n32_152, &_preg_268435483);
      for (cap_block_input_grid_n32_153 = 0; cap_block_input_grid_n32_153 < 9; cap_block_input_grid_n32_153 = cap_block_input_grid_n32_153 + 1) {
        _preg_268435590 = Rotate(input_dup_n32_152, _cst_252[cap_block_input_grid_n32_153]);
        Copy_ciph(&input_grid_n32_154[cap_block_input_grid_n32_153], &_preg_268435590);
        Free_poly_data(&_preg_268435590._c1_poly);
        Free_poly_data(&_preg_268435590._c0_poly);
      }
      for (num_grid_n32_155 = 0; num_grid_n32_155 < 32; num_grid_n32_155 = num_grid_n32_155 + 1) {
        Zero_ciph(&result_grid_n32_156);
        for (cap_block_n32_157 = 0; cap_block_n32_157 < 9; cap_block_n32_157 = cap_block_n32_157 + 1) {
          _ckks_gen_tmp_2602_397 = input_grid_n32_154[cap_block_n32_157];
          Pt_from_msg_ofst(&_pgen_tmp_204_670, 45/* cst_249 */, ((((mesh_x_n8_149 * 4) + mesh_y_n8_150) * 288) + (cap_block_n32_157 + (num_grid_n32_155 * 9))) * (25088), 25088, 1, 3);
          Init_ciph_up_scale_plain(&_pgen_tmp_205_671, &_ckks_gen_tmp_2602_397, &_pgen_tmp_204_670);
          Init_ciph_same_scale(&result_grid_n32_156, &result_grid_n32_156, &_pgen_tmp_205_671);
          {
            _pgen_modulus_463 = Q_modulus();
            _pgen_num_q_464 = Poly_level(&result_grid_n32_156._c0_poly);
            for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
              Hw_modmul(Coeffs(&_pgen_tmp_205_671._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_2602_397._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_204_670._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modmul(Coeffs(&_pgen_tmp_205_671._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_2602_397._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_204_670._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n32_156._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n32_156._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_205_671._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n32_156._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n32_156._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_205_671._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              _pgen_modulus_463 = _pgen_modulus_463 + 1;
            }
          }
        }
        _preg_268435591 = Rotate(result_grid_n32_156, num_grid_n32_155 * 784);
        Init_ciph_same_scale(&result_n32_151, &result_n32_151, &_preg_268435591);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&result_n32_151._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modadd(Coeffs(&result_n32_151._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n32_151._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435591._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&result_n32_151._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n32_151._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435591._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435591._c1_poly);
        Free_poly_data(&_preg_268435591._c0_poly);
      }
      Init_ciph_down_scale(&_pgen_tmp_206_672, &result_n32_151);
      Rescale(&_pgen_tmp_206_672._c0_poly, &result_n32_151._c0_poly);
      Rescale(&_pgen_tmp_206_672._c1_poly, &result_n32_151._c1_poly);
      Pt_from_msg(&_pgen_tmp_207_673, 46 /* cst_253 */, 25088, 1, 2);
      Init_ciph_up_scale_plain(&_preg_268435484, &_pgen_tmp_206_672, &_pgen_tmp_207_673);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435484._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_preg_268435484._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_206_672._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_207_673._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_preg_268435484._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_206_672._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_207_673._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      _preg_268435592 = Rotate(result_n32_151, -25088);
      Copy_ciph(&_ckks_gen_tmp_2654_398, &_preg_268435592);
      Init_ciph_down_scale(&_pgen_tmp_208_674, &_ckks_gen_tmp_2654_398);
      Rescale(&_pgen_tmp_208_674._c0_poly, &_ckks_gen_tmp_2654_398._c0_poly);
      Rescale(&_pgen_tmp_208_674._c1_poly, &_ckks_gen_tmp_2654_398._c1_poly);
      Pt_from_msg(&_pgen_tmp_209_675, 47 /* cst_254 */, 25088, 1, 2);
      Init_ciph_up_scale_plain(&_pgen_tmp_210_676, &_pgen_tmp_208_674, &_pgen_tmp_209_675);
      Init_ciph_same_scale(&_preg_268435484, &_preg_268435484, &_pgen_tmp_210_676);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435484._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_210_676._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_208_674._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_209_675._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_210_676._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_208_674._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_209_675._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435484._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435484._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_210_676._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435484._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435484._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_210_676._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Pt_from_msg(&_pgen_tmp_211_677, 48 /* cst_251 */, 25088, 2, 2);
      Init_ciph_same_scale_plain(&result_n32_151, &_preg_268435484, &_pgen_tmp_211_677);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&result_n32_151._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&result_n32_151._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435484._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_211_677._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Set_coeffs(&result_n32_151._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_preg_268435484._c1_poly, _pgen_rns_idx_465, degree));
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      _pgen_tmp_213_679 = VS_out_conv8_148[mesh_x_n8_149];
      Init_ciph_same_scale(&_pgen_tmp_212_678, &_pgen_tmp_213_679, &result_n32_151);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_212_678._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_212_678._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_213_679._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n32_151._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_212_678._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_213_679._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n32_151._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_conv8_148[mesh_x_n8_149], &_pgen_tmp_212_678);
      Tm_taken("Tensor::conv");
      Tm_taken("Tensor::add");
      Free_poly_data(&_preg_268435592._c1_poly);
      Free_poly_data(&_preg_268435592._c0_poly);
    }
  }
  Free_ciph_poly(VS_out_relu7_145, 4);
  Free_poly_data(&_preg_268435483._c1_poly);
  Free_poly_data(&_preg_268435483._c0_poly);
  Free_poly_data(&input_dup_n32_152._c1_poly);
  Free_poly_data(&input_dup_n32_152._c0_poly);
  Free_ciph_poly(input_grid_n32_154, 9);
  Free_poly_data(&_pgen_tmp_204_670._poly);
  Free_poly_data(&_pgen_tmp_205_671._c1_poly);
  Free_poly_data(&_pgen_tmp_205_671._c0_poly);
  Free_poly_data(&result_grid_n32_156._c1_poly);
  Free_poly_data(&result_grid_n32_156._c0_poly);
  Free_poly_data(&_pgen_tmp_207_673._poly);
  Free_poly_data(&_pgen_tmp_206_672._c1_poly);
  Free_poly_data(&_pgen_tmp_206_672._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_2654_398._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_2654_398._c0_poly);
  Free_poly_data(&_pgen_tmp_209_675._poly);
  Free_poly_data(&_pgen_tmp_208_674._c1_poly);
  Free_poly_data(&_pgen_tmp_208_674._c0_poly);
  Free_poly_data(&_pgen_tmp_210_676._c1_poly);
  Free_poly_data(&_pgen_tmp_210_676._c0_poly);
  Free_poly_data(&_pgen_tmp_211_677._poly);
  Free_poly_data(&_preg_268435484._c1_poly);
  Free_poly_data(&_preg_268435484._c0_poly);
  Free_poly_data(&result_n32_151._c1_poly);
  Free_poly_data(&result_n32_151._c0_poly);
  Free_poly_data(&_pgen_tmp_212_678._c1_poly);
  Free_poly_data(&_pgen_tmp_212_678._c0_poly);
  for (rescale_iv_453 = 0; rescale_iv_453 < 4; rescale_iv_453 = rescale_iv_453 + 1) {
    _pgen_tmp_214_680 = VS_out_conv8_148[rescale_iv_453];
    Init_ciph_down_scale(&_pgen_tmp_215_681, &_pgen_tmp_214_680);
    Rescale(&_pgen_tmp_215_681._c0_poly, &_pgen_tmp_214_680._c0_poly);
    Rescale(&_pgen_tmp_215_681._c1_poly, &_pgen_tmp_214_680._c1_poly);
    Copy_ciph(&VS_out_conv8_148[rescale_iv_453], &_pgen_tmp_215_681);
  }
  Free_poly_data(&_pgen_tmp_215_681._c1_poly);
  Free_poly_data(&_pgen_tmp_215_681._c0_poly);
  for (add_bias_n8_158 = 0; add_bias_n8_158 < 4; add_bias_n8_158 = add_bias_n8_158 + 1) {
    Tm_start("Tensor::add");
    _ckks_gen_tmp_2703_399 = VS_out_conv8_148[add_bias_n8_158];
    Pt_from_msg_ofst(&_pgen_tmp_217_683, 49/* cst_256 */, (add_bias_n8_158) * (25088), 25088, 1, 1);
    Init_ciph_same_scale_plain(&_pgen_tmp_216_682, &_ckks_gen_tmp_2703_399, &_pgen_tmp_217_683);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&_pgen_tmp_216_682._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_216_682._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_2703_399._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_217_683._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Set_coeffs(&_pgen_tmp_216_682._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_ckks_gen_tmp_2703_399._c1_poly, _pgen_rns_idx_465, degree));
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    Copy_ciph(&addshard8_159[add_bias_n8_158], &_pgen_tmp_216_682);
    Tm_taken("Tensor::add");
  }
  Free_ciph_poly(VS_out_conv8_148, 4);
  Free_poly_data(&_pgen_tmp_217_683._poly);
  Free_poly_data(&_pgen_tmp_216_682._c1_poly);
  Free_poly_data(&_pgen_tmp_216_682._c0_poly);
  memset(&VS_out_add8_160, 0, sizeof(VS_out_add8_160));
  for (mesh_x_n8_161 = 0; mesh_x_n8_161 < 4; mesh_x_n8_161 = mesh_x_n8_161 + 1) {
    for (mesh_y_n8_162 = 0; mesh_y_n8_162 < 1; mesh_y_n8_162 = mesh_y_n8_162 + 1) {
      Tm_start("Tensor::add");
      Tm_start("Tensor::add");
      _pgen_tmp_219_685 = VS_out_add8_160[((mesh_x_n8_161 * 1) + mesh_y_n8_162)];
      _pgen_tmp_220_686 = addshard8_159[((mesh_x_n8_161 * 1) + mesh_y_n8_162)];
      _pgen_tmp_221_687 = addshard6_123[((mesh_x_n8_161 * 1) + mesh_y_n8_162)];
      Init_ciph_same_scale(&_pgen_tmp_222_688, &_pgen_tmp_220_686, &_pgen_tmp_221_687);
      Init_ciph_same_scale(&_pgen_tmp_218_684, &_pgen_tmp_219_685, &_pgen_tmp_222_688);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_218_684._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_222_688._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_220_686._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_221_687._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_222_688._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_220_686._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_221_687._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_218_684._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_219_685._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_222_688._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_218_684._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_219_685._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_222_688._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_add8_160[((mesh_x_n8_161 * 1) + mesh_y_n8_162)], &_pgen_tmp_218_684);
      Tm_taken("Tensor::add");
      Tm_taken("Tensor::add");
    }
  }
  Free_ciph_poly(addshard8_159, 4);
  Free_ciph_poly(addshard6_123, 4);
  Free_poly_data(&_pgen_tmp_222_688._c1_poly);
  Free_poly_data(&_pgen_tmp_222_688._c0_poly);
  Free_poly_data(&_pgen_tmp_218_684._c1_poly);
  Free_poly_data(&_pgen_tmp_218_684._c0_poly);
  memset(&VS_out_relu8_163, 0, sizeof(VS_out_relu8_163));
  for (mesh_x_n8_164 = 0; mesh_x_n8_164 < 4; mesh_x_n8_164 = mesh_x_n8_164 + 1) {
    for (mesh_y_n8_165 = 0; mesh_y_n8_165 < 1; mesh_y_n8_165 = mesh_y_n8_165 + 1) {
      Tm_start("Tensor::add");
      _pgen_tmp_223_689 = VS_out_add8_160[((mesh_x_n8_164 * 1) + mesh_y_n8_165)];
      Bootstrap(&_preg_268435485, &_pgen_tmp_223_689, 15);
      Encode_float(&_pgen_tmp_225_691, &_cst_468, 1, 1, Level(&_preg_268435485));
      Init_ciph_up_scale_plain(&_pgen_tmp_224_690, &_preg_268435485, &_pgen_tmp_225_691);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_224_690._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_224_690._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435485._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_225_691._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_224_690._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435485._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_225_691._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Init_ciph_down_scale(&_pgen_tmp_226_692, &_pgen_tmp_224_690);
      Rescale(&_pgen_tmp_226_692._c0_poly, &_pgen_tmp_224_690._c0_poly);
      Rescale(&_pgen_tmp_226_692._c1_poly, &_pgen_tmp_224_690._c1_poly);
      _preg_268435486 = App_relu(_preg_268435485, _pgen_tmp_226_692, 25088);
      _pgen_tmp_228_694 = VS_out_relu8_163[((mesh_x_n8_164 * 1) + mesh_y_n8_165)];
      Init_ciph_same_scale(&_pgen_tmp_227_693, &_pgen_tmp_228_694, &_preg_268435486);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_227_693._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_227_693._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_228_694._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435486._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_227_693._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_228_694._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435486._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_relu8_163[((mesh_x_n8_164 * 1) + mesh_y_n8_165)], &_pgen_tmp_227_693);
      Tm_taken("Tensor::add");
    }
  }
  Free_ciph_poly(VS_out_add8_160, 4);
  Free_poly_data(&_pgen_tmp_225_691._poly);
  Free_poly_data(&_pgen_tmp_224_690._c1_poly);
  Free_poly_data(&_pgen_tmp_224_690._c0_poly);
  Free_poly_data(&_pgen_tmp_226_692._c1_poly);
  Free_poly_data(&_pgen_tmp_226_692._c0_poly);
  Free_poly_data(&_preg_268435485._c1_poly);
  Free_poly_data(&_preg_268435485._c0_poly);
  Free_poly_data(&_preg_268435486._c1_poly);
  Free_poly_data(&_preg_268435486._c0_poly);
  Free_poly_data(&_pgen_tmp_227_693._c1_poly);
  Free_poly_data(&_pgen_tmp_227_693._c0_poly);
  memset(&VS_out_conv9_166, 0, sizeof(VS_out_conv9_166));
  for (mesh_x_n9_167 = 0; mesh_x_n9_167 < 4; mesh_x_n9_167 = mesh_x_n9_167 + 1) {
    for (mesh_y_n9_168 = 0; mesh_y_n9_168 < 4; mesh_y_n9_168 = mesh_y_n9_168 + 1) {
      Tm_start("Tensor::add");
      Tm_start("Tensor::conv");
      Copy_ciph(&_preg_268435487, &VS_out_relu8_163[mesh_y_n9_168]);
      Zero_ciph(&result_n33_169);
      Copy_ciph(&input_dup_n33_170, &_preg_268435487);
      for (cap_block_input_grid_n33_171 = 0; cap_block_input_grid_n33_171 < 9; cap_block_input_grid_n33_171 = cap_block_input_grid_n33_171 + 1) {
        _preg_268435593 = Rotate(input_dup_n33_170, _cst_266[cap_block_input_grid_n33_171]);
        Copy_ciph(&input_grid_n33_172[cap_block_input_grid_n33_171], &_preg_268435593);
        Free_poly_data(&_preg_268435593._c1_poly);
        Free_poly_data(&_preg_268435593._c0_poly);
      }
      for (num_grid_n33_173 = 0; num_grid_n33_173 < 32; num_grid_n33_173 = num_grid_n33_173 + 1) {
        Zero_ciph(&result_grid_n33_174);
        for (cap_block_n33_175 = 0; cap_block_n33_175 < 9; cap_block_n33_175 = cap_block_n33_175 + 1) {
          _ckks_gen_tmp_2916_400 = input_grid_n33_172[cap_block_n33_175];
          Pt_from_msg_ofst(&_pgen_tmp_229_695, 50/* cst_263 */, ((((mesh_x_n9_167 * 4) + mesh_y_n9_168) * 288) + (cap_block_n33_175 + (num_grid_n33_173 * 9))) * (25088), 25088, 1, 3);
          Init_ciph_up_scale_plain(&_pgen_tmp_230_696, &_ckks_gen_tmp_2916_400, &_pgen_tmp_229_695);
          Init_ciph_same_scale(&result_grid_n33_174, &result_grid_n33_174, &_pgen_tmp_230_696);
          {
            _pgen_modulus_463 = Q_modulus();
            _pgen_num_q_464 = Poly_level(&result_grid_n33_174._c0_poly);
            for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
              Hw_modmul(Coeffs(&_pgen_tmp_230_696._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_2916_400._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_229_695._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modmul(Coeffs(&_pgen_tmp_230_696._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_2916_400._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_229_695._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n33_174._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n33_174._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_230_696._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n33_174._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n33_174._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_230_696._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              _pgen_modulus_463 = _pgen_modulus_463 + 1;
            }
          }
        }
        _preg_268435594 = Rotate(result_grid_n33_174, num_grid_n33_173 * 784);
        Init_ciph_same_scale(&result_n33_169, &result_n33_169, &_preg_268435594);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&result_n33_169._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modadd(Coeffs(&result_n33_169._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n33_169._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435594._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&result_n33_169._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n33_169._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435594._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435594._c1_poly);
        Free_poly_data(&_preg_268435594._c0_poly);
      }
      Init_ciph_down_scale(&_pgen_tmp_231_697, &result_n33_169);
      Rescale(&_pgen_tmp_231_697._c0_poly, &result_n33_169._c0_poly);
      Rescale(&_pgen_tmp_231_697._c1_poly, &result_n33_169._c1_poly);
      Pt_from_msg(&_pgen_tmp_232_698, 51 /* cst_267 */, 25088, 1, 2);
      Init_ciph_up_scale_plain(&_preg_268435488, &_pgen_tmp_231_697, &_pgen_tmp_232_698);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435488._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_preg_268435488._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_231_697._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_232_698._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_preg_268435488._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_231_697._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_232_698._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      _preg_268435595 = Rotate(result_n33_169, -25088);
      Copy_ciph(&_ckks_gen_tmp_2968_401, &_preg_268435595);
      Init_ciph_down_scale(&_pgen_tmp_233_699, &_ckks_gen_tmp_2968_401);
      Rescale(&_pgen_tmp_233_699._c0_poly, &_ckks_gen_tmp_2968_401._c0_poly);
      Rescale(&_pgen_tmp_233_699._c1_poly, &_ckks_gen_tmp_2968_401._c1_poly);
      Pt_from_msg(&_pgen_tmp_234_700, 52 /* cst_268 */, 25088, 1, 2);
      Init_ciph_up_scale_plain(&_pgen_tmp_235_701, &_pgen_tmp_233_699, &_pgen_tmp_234_700);
      Init_ciph_same_scale(&_preg_268435488, &_preg_268435488, &_pgen_tmp_235_701);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435488._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_235_701._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_233_699._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_234_700._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_235_701._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_233_699._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_234_700._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435488._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435488._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_235_701._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435488._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435488._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_235_701._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Pt_from_msg(&_pgen_tmp_236_702, 53 /* cst_265 */, 25088, 2, 2);
      Init_ciph_same_scale_plain(&result_n33_169, &_preg_268435488, &_pgen_tmp_236_702);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&result_n33_169._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&result_n33_169._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435488._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_236_702._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Set_coeffs(&result_n33_169._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_preg_268435488._c1_poly, _pgen_rns_idx_465, degree));
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      _pgen_tmp_238_704 = VS_out_conv9_166[mesh_x_n9_167];
      Init_ciph_same_scale(&_pgen_tmp_237_703, &_pgen_tmp_238_704, &result_n33_169);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_237_703._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_237_703._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_238_704._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n33_169._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_237_703._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_238_704._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n33_169._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_conv9_166[mesh_x_n9_167], &_pgen_tmp_237_703);
      Tm_taken("Tensor::conv");
      Tm_taken("Tensor::add");
      Free_poly_data(&_preg_268435595._c1_poly);
      Free_poly_data(&_preg_268435595._c0_poly);
    }
  }
  Free_poly_data(&_preg_268435487._c1_poly);
  Free_poly_data(&_preg_268435487._c0_poly);
  Free_poly_data(&input_dup_n33_170._c1_poly);
  Free_poly_data(&input_dup_n33_170._c0_poly);
  Free_ciph_poly(input_grid_n33_172, 9);
  Free_poly_data(&_pgen_tmp_229_695._poly);
  Free_poly_data(&_pgen_tmp_230_696._c1_poly);
  Free_poly_data(&_pgen_tmp_230_696._c0_poly);
  Free_poly_data(&result_grid_n33_174._c1_poly);
  Free_poly_data(&result_grid_n33_174._c0_poly);
  Free_poly_data(&_pgen_tmp_232_698._poly);
  Free_poly_data(&_pgen_tmp_231_697._c1_poly);
  Free_poly_data(&_pgen_tmp_231_697._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_2968_401._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_2968_401._c0_poly);
  Free_poly_data(&_pgen_tmp_234_700._poly);
  Free_poly_data(&_pgen_tmp_233_699._c1_poly);
  Free_poly_data(&_pgen_tmp_233_699._c0_poly);
  Free_poly_data(&_pgen_tmp_235_701._c1_poly);
  Free_poly_data(&_pgen_tmp_235_701._c0_poly);
  Free_poly_data(&_pgen_tmp_236_702._poly);
  Free_poly_data(&_preg_268435488._c1_poly);
  Free_poly_data(&_preg_268435488._c0_poly);
  Free_poly_data(&result_n33_169._c1_poly);
  Free_poly_data(&result_n33_169._c0_poly);
  Free_poly_data(&_pgen_tmp_237_703._c1_poly);
  Free_poly_data(&_pgen_tmp_237_703._c0_poly);
  for (rescale_iv_454 = 0; rescale_iv_454 < 4; rescale_iv_454 = rescale_iv_454 + 1) {
    _pgen_tmp_239_705 = VS_out_conv9_166[rescale_iv_454];
    Init_ciph_down_scale(&_pgen_tmp_240_706, &_pgen_tmp_239_705);
    Rescale(&_pgen_tmp_240_706._c0_poly, &_pgen_tmp_239_705._c0_poly);
    Rescale(&_pgen_tmp_240_706._c1_poly, &_pgen_tmp_239_705._c1_poly);
    Copy_ciph(&VS_out_conv9_166[rescale_iv_454], &_pgen_tmp_240_706);
  }
  Free_poly_data(&_pgen_tmp_240_706._c1_poly);
  Free_poly_data(&_pgen_tmp_240_706._c0_poly);
  for (add_bias_n9_176 = 0; add_bias_n9_176 < 4; add_bias_n9_176 = add_bias_n9_176 + 1) {
    Tm_start("Tensor::add");
    _ckks_gen_tmp_3017_402 = VS_out_conv9_166[add_bias_n9_176];
    Pt_from_msg_ofst(&_pgen_tmp_242_708, 54/* cst_270 */, (add_bias_n9_176) * (25088), 25088, 1, 1);
    Init_ciph_same_scale_plain(&_pgen_tmp_241_707, &_ckks_gen_tmp_3017_402, &_pgen_tmp_242_708);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&_pgen_tmp_241_707._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_241_707._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_3017_402._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_242_708._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Set_coeffs(&_pgen_tmp_241_707._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_ckks_gen_tmp_3017_402._c1_poly, _pgen_rns_idx_465, degree));
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    Copy_ciph(&addshard9_177[add_bias_n9_176], &_pgen_tmp_241_707);
    Tm_taken("Tensor::add");
  }
  Free_ciph_poly(VS_out_conv9_166, 4);
  Free_poly_data(&_pgen_tmp_242_708._poly);
  Free_poly_data(&_pgen_tmp_241_707._c1_poly);
  Free_poly_data(&_pgen_tmp_241_707._c0_poly);
  memset(&VS_out_relu9_178, 0, sizeof(VS_out_relu9_178));
  for (mesh_x_n9_179 = 0; mesh_x_n9_179 < 4; mesh_x_n9_179 = mesh_x_n9_179 + 1) {
    for (mesh_y_n9_180 = 0; mesh_y_n9_180 < 1; mesh_y_n9_180 = mesh_y_n9_180 + 1) {
      Tm_start("Tensor::add");
      _pgen_tmp_243_709 = addshard9_177[((mesh_x_n9_179 * 1) + mesh_y_n9_180)];
      Bootstrap(&_preg_268435489, &_pgen_tmp_243_709, 15);
      Encode_float(&_pgen_tmp_245_711, &_cst_469, 1, 1, Level(&_preg_268435489));
      Init_ciph_up_scale_plain(&_pgen_tmp_244_710, &_preg_268435489, &_pgen_tmp_245_711);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_244_710._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_244_710._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435489._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_245_711._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_244_710._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435489._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_245_711._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Init_ciph_down_scale(&_pgen_tmp_246_712, &_pgen_tmp_244_710);
      Rescale(&_pgen_tmp_246_712._c0_poly, &_pgen_tmp_244_710._c0_poly);
      Rescale(&_pgen_tmp_246_712._c1_poly, &_pgen_tmp_244_710._c1_poly);
      _preg_268435490 = App_relu(_preg_268435489, _pgen_tmp_246_712, 25088);
      _pgen_tmp_248_714 = VS_out_relu9_178[((mesh_x_n9_179 * 1) + mesh_y_n9_180)];
      Init_ciph_same_scale(&_pgen_tmp_247_713, &_pgen_tmp_248_714, &_preg_268435490);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_247_713._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_247_713._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_248_714._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435490._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_247_713._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_248_714._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435490._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_relu9_178[((mesh_x_n9_179 * 1) + mesh_y_n9_180)], &_pgen_tmp_247_713);
      Tm_taken("Tensor::add");
    }
  }
  Free_ciph_poly(addshard9_177, 4);
  Free_poly_data(&_pgen_tmp_245_711._poly);
  Free_poly_data(&_pgen_tmp_244_710._c1_poly);
  Free_poly_data(&_pgen_tmp_244_710._c0_poly);
  Free_poly_data(&_pgen_tmp_246_712._c1_poly);
  Free_poly_data(&_pgen_tmp_246_712._c0_poly);
  Free_poly_data(&_preg_268435489._c1_poly);
  Free_poly_data(&_preg_268435489._c0_poly);
  Free_poly_data(&_preg_268435490._c1_poly);
  Free_poly_data(&_preg_268435490._c0_poly);
  Free_poly_data(&_pgen_tmp_247_713._c1_poly);
  Free_poly_data(&_pgen_tmp_247_713._c0_poly);
  memset(&VS_out_conv10_181, 0, sizeof(VS_out_conv10_181));
  for (mesh_x_n10_182 = 0; mesh_x_n10_182 < 4; mesh_x_n10_182 = mesh_x_n10_182 + 1) {
    for (mesh_y_n10_183 = 0; mesh_y_n10_183 < 4; mesh_y_n10_183 = mesh_y_n10_183 + 1) {
      Tm_start("Tensor::add");
      Tm_start("Tensor::conv");
      Copy_ciph(&_preg_268435491, &VS_out_relu9_178[mesh_y_n10_183]);
      Zero_ciph(&result_n34_184);
      Copy_ciph(&input_dup_n34_185, &_preg_268435491);
      for (cap_block_input_grid_n34_186 = 0; cap_block_input_grid_n34_186 < 9; cap_block_input_grid_n34_186 = cap_block_input_grid_n34_186 + 1) {
        _preg_268435596 = Rotate(input_dup_n34_185, _cst_278[cap_block_input_grid_n34_186]);
        Copy_ciph(&input_grid_n34_187[cap_block_input_grid_n34_186], &_preg_268435596);
        Free_poly_data(&_preg_268435596._c1_poly);
        Free_poly_data(&_preg_268435596._c0_poly);
      }
      for (num_grid_n34_188 = 0; num_grid_n34_188 < 32; num_grid_n34_188 = num_grid_n34_188 + 1) {
        Zero_ciph(&result_grid_n34_189);
        for (cap_block_n34_190 = 0; cap_block_n34_190 < 9; cap_block_n34_190 = cap_block_n34_190 + 1) {
          _ckks_gen_tmp_3170_403 = input_grid_n34_187[cap_block_n34_190];
          Pt_from_msg_ofst(&_pgen_tmp_249_715, 55/* cst_275 */, ((((mesh_x_n10_182 * 4) + mesh_y_n10_183) * 288) + (cap_block_n34_190 + (num_grid_n34_188 * 9))) * (25088), 25088, 1, 3);
          Init_ciph_up_scale_plain(&_pgen_tmp_250_716, &_ckks_gen_tmp_3170_403, &_pgen_tmp_249_715);
          Init_ciph_same_scale(&result_grid_n34_189, &result_grid_n34_189, &_pgen_tmp_250_716);
          {
            _pgen_modulus_463 = Q_modulus();
            _pgen_num_q_464 = Poly_level(&result_grid_n34_189._c0_poly);
            for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
              Hw_modmul(Coeffs(&_pgen_tmp_250_716._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_3170_403._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_249_715._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modmul(Coeffs(&_pgen_tmp_250_716._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_3170_403._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_249_715._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n34_189._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n34_189._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_250_716._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n34_189._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n34_189._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_250_716._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              _pgen_modulus_463 = _pgen_modulus_463 + 1;
            }
          }
        }
        _preg_268435597 = Rotate(result_grid_n34_189, num_grid_n34_188 * 784);
        Init_ciph_same_scale(&result_n34_184, &result_n34_184, &_preg_268435597);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&result_n34_184._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modadd(Coeffs(&result_n34_184._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n34_184._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435597._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&result_n34_184._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n34_184._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435597._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435597._c1_poly);
        Free_poly_data(&_preg_268435597._c0_poly);
      }
      Init_ciph_down_scale(&_pgen_tmp_251_717, &result_n34_184);
      Rescale(&_pgen_tmp_251_717._c0_poly, &result_n34_184._c0_poly);
      Rescale(&_pgen_tmp_251_717._c1_poly, &result_n34_184._c1_poly);
      Pt_from_msg(&_pgen_tmp_252_718, 56 /* cst_279 */, 25088, 1, 2);
      Init_ciph_up_scale_plain(&_preg_268435492, &_pgen_tmp_251_717, &_pgen_tmp_252_718);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435492._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_preg_268435492._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_251_717._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_252_718._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_preg_268435492._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_251_717._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_252_718._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      _preg_268435598 = Rotate(result_n34_184, -25088);
      Copy_ciph(&_ckks_gen_tmp_3222_404, &_preg_268435598);
      Init_ciph_down_scale(&_pgen_tmp_253_719, &_ckks_gen_tmp_3222_404);
      Rescale(&_pgen_tmp_253_719._c0_poly, &_ckks_gen_tmp_3222_404._c0_poly);
      Rescale(&_pgen_tmp_253_719._c1_poly, &_ckks_gen_tmp_3222_404._c1_poly);
      Pt_from_msg(&_pgen_tmp_254_720, 57 /* cst_280 */, 25088, 1, 2);
      Init_ciph_up_scale_plain(&_pgen_tmp_255_721, &_pgen_tmp_253_719, &_pgen_tmp_254_720);
      Init_ciph_same_scale(&_preg_268435492, &_preg_268435492, &_pgen_tmp_255_721);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435492._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_255_721._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_253_719._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_254_720._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_255_721._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_253_719._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_254_720._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435492._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435492._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_255_721._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435492._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435492._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_255_721._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Pt_from_msg(&_pgen_tmp_256_722, 58 /* cst_277 */, 25088, 2, 2);
      Init_ciph_same_scale_plain(&result_n34_184, &_preg_268435492, &_pgen_tmp_256_722);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&result_n34_184._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&result_n34_184._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435492._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_256_722._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Set_coeffs(&result_n34_184._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_preg_268435492._c1_poly, _pgen_rns_idx_465, degree));
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      _pgen_tmp_258_724 = VS_out_conv10_181[mesh_x_n10_182];
      Init_ciph_same_scale(&_pgen_tmp_257_723, &_pgen_tmp_258_724, &result_n34_184);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_257_723._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_257_723._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_258_724._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n34_184._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_257_723._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_258_724._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n34_184._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_conv10_181[mesh_x_n10_182], &_pgen_tmp_257_723);
      Tm_taken("Tensor::conv");
      Tm_taken("Tensor::add");
      Free_poly_data(&_preg_268435598._c1_poly);
      Free_poly_data(&_preg_268435598._c0_poly);
    }
  }
  Free_ciph_poly(VS_out_relu9_178, 4);
  Free_poly_data(&_preg_268435491._c1_poly);
  Free_poly_data(&_preg_268435491._c0_poly);
  Free_poly_data(&input_dup_n34_185._c1_poly);
  Free_poly_data(&input_dup_n34_185._c0_poly);
  Free_ciph_poly(input_grid_n34_187, 9);
  Free_poly_data(&_pgen_tmp_249_715._poly);
  Free_poly_data(&_pgen_tmp_250_716._c1_poly);
  Free_poly_data(&_pgen_tmp_250_716._c0_poly);
  Free_poly_data(&result_grid_n34_189._c1_poly);
  Free_poly_data(&result_grid_n34_189._c0_poly);
  Free_poly_data(&_pgen_tmp_252_718._poly);
  Free_poly_data(&_pgen_tmp_251_717._c1_poly);
  Free_poly_data(&_pgen_tmp_251_717._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_3222_404._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_3222_404._c0_poly);
  Free_poly_data(&_pgen_tmp_254_720._poly);
  Free_poly_data(&_pgen_tmp_253_719._c1_poly);
  Free_poly_data(&_pgen_tmp_253_719._c0_poly);
  Free_poly_data(&_pgen_tmp_255_721._c1_poly);
  Free_poly_data(&_pgen_tmp_255_721._c0_poly);
  Free_poly_data(&_pgen_tmp_256_722._poly);
  Free_poly_data(&_preg_268435492._c1_poly);
  Free_poly_data(&_preg_268435492._c0_poly);
  Free_poly_data(&result_n34_184._c1_poly);
  Free_poly_data(&result_n34_184._c0_poly);
  Free_poly_data(&_pgen_tmp_257_723._c1_poly);
  Free_poly_data(&_pgen_tmp_257_723._c0_poly);
  for (rescale_iv_455 = 0; rescale_iv_455 < 4; rescale_iv_455 = rescale_iv_455 + 1) {
    _pgen_tmp_259_725 = VS_out_conv10_181[rescale_iv_455];
    Init_ciph_down_scale(&_pgen_tmp_260_726, &_pgen_tmp_259_725);
    Rescale(&_pgen_tmp_260_726._c0_poly, &_pgen_tmp_259_725._c0_poly);
    Rescale(&_pgen_tmp_260_726._c1_poly, &_pgen_tmp_259_725._c1_poly);
    Copy_ciph(&VS_out_conv10_181[rescale_iv_455], &_pgen_tmp_260_726);
  }
  Free_poly_data(&_pgen_tmp_260_726._c1_poly);
  Free_poly_data(&_pgen_tmp_260_726._c0_poly);
  for (add_bias_n10_191 = 0; add_bias_n10_191 < 4; add_bias_n10_191 = add_bias_n10_191 + 1) {
    Tm_start("Tensor::add");
    _ckks_gen_tmp_3271_405 = VS_out_conv10_181[add_bias_n10_191];
    Pt_from_msg_ofst(&_pgen_tmp_262_728, 59/* cst_282 */, (add_bias_n10_191) * (25088), 25088, 1, 1);
    Init_ciph_same_scale_plain(&_pgen_tmp_261_727, &_ckks_gen_tmp_3271_405, &_pgen_tmp_262_728);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&_pgen_tmp_261_727._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_261_727._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_3271_405._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_262_728._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Set_coeffs(&_pgen_tmp_261_727._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_ckks_gen_tmp_3271_405._c1_poly, _pgen_rns_idx_465, degree));
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    Copy_ciph(&addshard10_192[add_bias_n10_191], &_pgen_tmp_261_727);
    Tm_taken("Tensor::add");
  }
  Free_ciph_poly(VS_out_conv10_181, 4);
  Free_poly_data(&_pgen_tmp_262_728._poly);
  Free_poly_data(&_pgen_tmp_261_727._c1_poly);
  Free_poly_data(&_pgen_tmp_261_727._c0_poly);
  memset(&VS_out_add10_193, 0, sizeof(VS_out_add10_193));
  for (mesh_x_n10_194 = 0; mesh_x_n10_194 < 4; mesh_x_n10_194 = mesh_x_n10_194 + 1) {
    for (mesh_y_n10_195 = 0; mesh_y_n10_195 < 1; mesh_y_n10_195 = mesh_y_n10_195 + 1) {
      Tm_start("Tensor::add");
      Tm_start("Tensor::add");
      _pgen_tmp_264_730 = VS_out_add10_193[((mesh_x_n10_194 * 1) + mesh_y_n10_195)];
      _pgen_tmp_265_731 = addshard10_192[((mesh_x_n10_194 * 1) + mesh_y_n10_195)];
      _pgen_tmp_266_732 = VS_out_relu8_163[((mesh_x_n10_194 * 1) + mesh_y_n10_195)];
      Init_ciph_same_scale(&_pgen_tmp_267_733, &_pgen_tmp_265_731, &_pgen_tmp_266_732);
      Init_ciph_same_scale(&_pgen_tmp_263_729, &_pgen_tmp_264_730, &_pgen_tmp_267_733);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_263_729._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_267_733._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_265_731._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_266_732._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_267_733._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_265_731._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_266_732._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_263_729._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_264_730._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_267_733._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_263_729._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_264_730._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_267_733._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_add10_193[((mesh_x_n10_194 * 1) + mesh_y_n10_195)], &_pgen_tmp_263_729);
      Tm_taken("Tensor::add");
      Tm_taken("Tensor::add");
    }
  }
  Free_ciph_poly(addshard10_192, 4);
  Free_ciph_poly(VS_out_relu8_163, 4);
  Free_poly_data(&_pgen_tmp_267_733._c1_poly);
  Free_poly_data(&_pgen_tmp_267_733._c0_poly);
  Free_poly_data(&_pgen_tmp_263_729._c1_poly);
  Free_poly_data(&_pgen_tmp_263_729._c0_poly);
  memset(&VS_out_relu10_196, 0, sizeof(VS_out_relu10_196));
  for (mesh_x_n10_197 = 0; mesh_x_n10_197 < 4; mesh_x_n10_197 = mesh_x_n10_197 + 1) {
    for (mesh_y_n10_198 = 0; mesh_y_n10_198 < 1; mesh_y_n10_198 = mesh_y_n10_198 + 1) {
      Tm_start("Tensor::add");
      _pgen_tmp_268_734 = VS_out_add10_193[((mesh_x_n10_197 * 1) + mesh_y_n10_198)];
      Bootstrap(&_preg_268435493, &_pgen_tmp_268_734, 18);
      Encode_float(&_pgen_tmp_270_736, &_cst_470, 1, 1, Level(&_preg_268435493));
      Init_ciph_up_scale_plain(&_pgen_tmp_269_735, &_preg_268435493, &_pgen_tmp_270_736);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_269_735._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_269_735._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435493._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_270_736._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_269_735._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435493._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_270_736._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Init_ciph_down_scale(&_pgen_tmp_271_737, &_pgen_tmp_269_735);
      Rescale(&_pgen_tmp_271_737._c0_poly, &_pgen_tmp_269_735._c0_poly);
      Rescale(&_pgen_tmp_271_737._c1_poly, &_pgen_tmp_269_735._c1_poly);
      _preg_268435494 = App_relu(_preg_268435493, _pgen_tmp_271_737, 25088);
      _pgen_tmp_273_739 = VS_out_relu10_196[((mesh_x_n10_197 * 1) + mesh_y_n10_198)];
      Init_ciph_same_scale(&_pgen_tmp_272_738, &_pgen_tmp_273_739, &_preg_268435494);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_272_738._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_272_738._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_273_739._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435494._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_272_738._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_273_739._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435494._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_relu10_196[((mesh_x_n10_197 * 1) + mesh_y_n10_198)], &_pgen_tmp_272_738);
      Tm_taken("Tensor::add");
    }
  }
  Free_ciph_poly(VS_out_add10_193, 4);
  Free_poly_data(&_pgen_tmp_270_736._poly);
  Free_poly_data(&_pgen_tmp_269_735._c1_poly);
  Free_poly_data(&_pgen_tmp_269_735._c0_poly);
  Free_poly_data(&_pgen_tmp_271_737._c1_poly);
  Free_poly_data(&_pgen_tmp_271_737._c0_poly);
  Free_poly_data(&_preg_268435493._c1_poly);
  Free_poly_data(&_preg_268435493._c0_poly);
  Free_poly_data(&_preg_268435494._c1_poly);
  Free_poly_data(&_preg_268435494._c0_poly);
  Free_poly_data(&_pgen_tmp_272_738._c1_poly);
  Free_poly_data(&_pgen_tmp_272_738._c0_poly);
  memset(&VS_out_conv11_199, 0, sizeof(VS_out_conv11_199));
  for (mesh_x_n11_200 = 0; mesh_x_n11_200 < 8; mesh_x_n11_200 = mesh_x_n11_200 + 1) {
    for (mesh_y_n11_201 = 0; mesh_y_n11_201 < 4; mesh_y_n11_201 = mesh_y_n11_201 + 1) {
      Tm_start("Tensor::add");
      Tm_start("Tensor::conv");
      Copy_ciph(&_preg_268435495, &VS_out_relu10_196[mesh_y_n11_201]);
      Zero_ciph(&result_n35_202);
      _preg_268435599 = Rotate(_preg_268435495, -25088);
      Init_ciph_same_scale(&input_dup_n35_203, &_preg_268435495, &_preg_268435599);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&input_dup_n35_203._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&input_dup_n35_203._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435495._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435599._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&input_dup_n35_203._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435495._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435599._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      for (cap_block_input_grid_n35_204 = 0; cap_block_input_grid_n35_204 < 4; cap_block_input_grid_n35_204 = cap_block_input_grid_n35_204 + 1) {
        _preg_268435600 = Rotate(input_dup_n35_203, _cst_292[cap_block_input_grid_n35_204]);
        Copy_ciph(&input_grid_n35_205[cap_block_input_grid_n35_204], &_preg_268435600);
        Free_poly_data(&_preg_268435600._c1_poly);
        Free_poly_data(&_preg_268435600._c0_poly);
      }
      for (num_grid_n35_206 = 0; num_grid_n35_206 < 8; num_grid_n35_206 = num_grid_n35_206 + 1) {
        Zero_ciph(&result_grid_n35_207);
        for (cap_block_n35_208 = 0; cap_block_n35_208 < 4; cap_block_n35_208 = cap_block_n35_208 + 1) {
          _ckks_gen_tmp_3488_406 = input_grid_n35_205[cap_block_n35_208];
          Pt_from_msg_ofst(&_pgen_tmp_274_740, 60/* cst_289 */, ((((mesh_x_n11_200 * 4) + mesh_y_n11_201) * 32) + (cap_block_n35_208 + (num_grid_n35_206 * 4))) * (25088), 25088, 1, 6);
          Init_ciph_up_scale_plain(&_pgen_tmp_275_741, &_ckks_gen_tmp_3488_406, &_pgen_tmp_274_740);
          Init_ciph_same_scale(&result_grid_n35_207, &result_grid_n35_207, &_pgen_tmp_275_741);
          {
            _pgen_modulus_463 = Q_modulus();
            _pgen_num_q_464 = Poly_level(&result_grid_n35_207._c0_poly);
            for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
              Hw_modmul(Coeffs(&_pgen_tmp_275_741._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_3488_406._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_274_740._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modmul(Coeffs(&_pgen_tmp_275_741._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_3488_406._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_274_740._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n35_207._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n35_207._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_275_741._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n35_207._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n35_207._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_275_741._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              _pgen_modulus_463 = _pgen_modulus_463 + 1;
            }
          }
        }
        _preg_268435601 = Rotate(result_grid_n35_207, num_grid_n35_206 * 3136);
        Init_ciph_same_scale(&result_n35_202, &result_n35_202, &_preg_268435601);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&result_n35_202._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modadd(Coeffs(&result_n35_202._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n35_202._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435601._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&result_n35_202._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n35_202._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435601._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435601._c1_poly);
        Free_poly_data(&_preg_268435601._c0_poly);
      }
      Init_ciph_down_scale(&_pgen_tmp_276_742, &result_n35_202);
      Rescale(&_pgen_tmp_276_742._c0_poly, &result_n35_202._c0_poly);
      Rescale(&_pgen_tmp_276_742._c1_poly, &result_n35_202._c1_poly);
      Pt_from_msg(&_pgen_tmp_277_743, 61 /* cst_293 */, 25088, 1, 5);
      Init_ciph_up_scale_plain(&_preg_268435496, &_pgen_tmp_276_742, &_pgen_tmp_277_743);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435496._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_preg_268435496._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_276_742._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_277_743._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_preg_268435496._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_276_742._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_277_743._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      _preg_268435602 = Rotate(result_n35_202, -25088);
      Copy_ciph(&_ckks_gen_tmp_3540_407, &_preg_268435602);
      Init_ciph_down_scale(&_pgen_tmp_278_744, &_ckks_gen_tmp_3540_407);
      Rescale(&_pgen_tmp_278_744._c0_poly, &_ckks_gen_tmp_3540_407._c0_poly);
      Rescale(&_pgen_tmp_278_744._c1_poly, &_ckks_gen_tmp_3540_407._c1_poly);
      Pt_from_msg(&_pgen_tmp_279_745, 62 /* cst_294 */, 25088, 1, 5);
      Init_ciph_up_scale_plain(&_pgen_tmp_280_746, &_pgen_tmp_278_744, &_pgen_tmp_279_745);
      Init_ciph_same_scale(&_preg_268435496, &_preg_268435496, &_pgen_tmp_280_746);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435496._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_280_746._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_278_744._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_279_745._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_280_746._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_278_744._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_279_745._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435496._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435496._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_280_746._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435496._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435496._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_280_746._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Pt_from_msg(&_pgen_tmp_281_747, 63 /* cst_291 */, 25088, 2, 5);
      Init_ciph_same_scale_plain(&result_n35_202, &_preg_268435496, &_pgen_tmp_281_747);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&result_n35_202._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&result_n35_202._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435496._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_281_747._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Set_coeffs(&result_n35_202._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_preg_268435496._c1_poly, _pgen_rns_idx_465, degree));
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&orig_result35_209, &result_n35_202);
      Zero_ciph(&comb_row_result36_210);
      for (combine_row_index_n36_211 = 0; combine_row_index_n36_211 < 14; combine_row_index_n36_211 = combine_row_index_n36_211 + 1) {
        _preg_268435603 = Rotate(orig_result35_209, combine_row_index_n36_211 * 1);
        Copy_ciph(&_ckks_gen_tmp_3582_408, &_preg_268435603);
        Init_ciph_down_scale(&_pgen_tmp_282_748, &_ckks_gen_tmp_3582_408);
        Rescale(&_pgen_tmp_282_748._c0_poly, &_ckks_gen_tmp_3582_408._c0_poly);
        Rescale(&_pgen_tmp_282_748._c1_poly, &_ckks_gen_tmp_3582_408._c1_poly);
        Pt_from_msg_ofst(&_pgen_tmp_283_749, 64/* cst_295 */, (combine_row_index_n36_211) * (25088), 25088, 1, 4);
        Init_ciph_up_scale_plain(&_pgen_tmp_284_750, &_pgen_tmp_282_748, &_pgen_tmp_283_749);
        Init_ciph_same_scale(&comb_row_result36_210, &comb_row_result36_210, &_pgen_tmp_284_750);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&comb_row_result36_210._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modmul(Coeffs(&_pgen_tmp_284_750._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_282_748._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_283_749._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modmul(Coeffs(&_pgen_tmp_284_750._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_282_748._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_283_749._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&comb_row_result36_210._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_row_result36_210._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_284_750._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&comb_row_result36_210._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_row_result36_210._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_284_750._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435603._c1_poly);
        Free_poly_data(&_preg_268435603._c0_poly);
      }
      Zero_ciph(&comb_rc_result36_212);
      for (combine_rc_index_n36_213 = 0; combine_rc_index_n36_213 < 14; combine_rc_index_n36_213 = combine_rc_index_n36_213 + 1) {
        _preg_268435604 = Rotate(comb_row_result36_210, combine_rc_index_n36_213 * 42);
        Copy_ciph(&_ckks_gen_tmp_3615_409, &_preg_268435604);
        Init_ciph_down_scale(&_pgen_tmp_285_751, &_ckks_gen_tmp_3615_409);
        Rescale(&_pgen_tmp_285_751._c0_poly, &_ckks_gen_tmp_3615_409._c0_poly);
        Rescale(&_pgen_tmp_285_751._c1_poly, &_ckks_gen_tmp_3615_409._c1_poly);
        Pt_from_msg_ofst(&_pgen_tmp_286_752, 65/* cst_296 */, (combine_rc_index_n36_213) * (25088), 25088, 1, 3);
        Init_ciph_up_scale_plain(&_pgen_tmp_287_753, &_pgen_tmp_285_751, &_pgen_tmp_286_752);
        Init_ciph_same_scale(&comb_rc_result36_212, &comb_rc_result36_212, &_pgen_tmp_287_753);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&comb_rc_result36_212._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modmul(Coeffs(&_pgen_tmp_287_753._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_285_751._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_286_752._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modmul(Coeffs(&_pgen_tmp_287_753._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_285_751._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_286_752._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&comb_rc_result36_212._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_rc_result36_212._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_287_753._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&comb_rc_result36_212._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_rc_result36_212._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_287_753._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435604._c1_poly);
        Free_poly_data(&_preg_268435604._c0_poly);
      }
      Zero_ciph(&final_result36_214);
      for (combine_cc_index_n36_215 = 0; combine_cc_index_n36_215 < 32; combine_cc_index_n36_215 = combine_cc_index_n36_215 + 1) {
        _preg_268435605 = Rotate(comb_rc_result36_212, combine_cc_index_n36_215 * 588);
        Copy_ciph(&_ckks_gen_tmp_3648_410, &_preg_268435605);
        Init_ciph_down_scale(&_pgen_tmp_288_754, &_ckks_gen_tmp_3648_410);
        Rescale(&_pgen_tmp_288_754._c0_poly, &_ckks_gen_tmp_3648_410._c0_poly);
        Rescale(&_pgen_tmp_288_754._c1_poly, &_ckks_gen_tmp_3648_410._c1_poly);
        Pt_from_msg_ofst(&_pgen_tmp_289_755, 66/* cst_297 */, (combine_cc_index_n36_215) * (25088), 25088, 1, 2);
        Init_ciph_up_scale_plain(&_pgen_tmp_290_756, &_pgen_tmp_288_754, &_pgen_tmp_289_755);
        Init_ciph_same_scale(&final_result36_214, &final_result36_214, &_pgen_tmp_290_756);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&final_result36_214._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modmul(Coeffs(&_pgen_tmp_290_756._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_288_754._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_289_755._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modmul(Coeffs(&_pgen_tmp_290_756._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_288_754._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_289_755._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&final_result36_214._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&final_result36_214._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_290_756._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&final_result36_214._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&final_result36_214._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_290_756._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435605._c1_poly);
        Free_poly_data(&_preg_268435605._c0_poly);
      }
      _pgen_tmp_292_758 = VS_out_conv11_199[mesh_x_n11_200];
      Init_ciph_same_scale(&_pgen_tmp_291_757, &_pgen_tmp_292_758, &final_result36_214);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_291_757._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_291_757._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_292_758._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&final_result36_214._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_291_757._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_292_758._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&final_result36_214._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_conv11_199[mesh_x_n11_200], &_pgen_tmp_291_757);
      Tm_taken("Tensor::conv");
      Tm_taken("Tensor::add");
      Free_poly_data(&_preg_268435599._c1_poly);
      Free_poly_data(&_preg_268435599._c0_poly);
      Free_poly_data(&_preg_268435602._c1_poly);
      Free_poly_data(&_preg_268435602._c0_poly);
    }
  }
  Free_poly_data(&_preg_268435495._c1_poly);
  Free_poly_data(&_preg_268435495._c0_poly);
  Free_poly_data(&input_dup_n35_203._c1_poly);
  Free_poly_data(&input_dup_n35_203._c0_poly);
  Free_ciph_poly(input_grid_n35_205, 4);
  Free_poly_data(&_pgen_tmp_274_740._poly);
  Free_poly_data(&_pgen_tmp_275_741._c1_poly);
  Free_poly_data(&_pgen_tmp_275_741._c0_poly);
  Free_poly_data(&result_grid_n35_207._c1_poly);
  Free_poly_data(&result_grid_n35_207._c0_poly);
  Free_poly_data(&_pgen_tmp_277_743._poly);
  Free_poly_data(&_pgen_tmp_276_742._c1_poly);
  Free_poly_data(&_pgen_tmp_276_742._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_3540_407._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_3540_407._c0_poly);
  Free_poly_data(&_pgen_tmp_279_745._poly);
  Free_poly_data(&_pgen_tmp_278_744._c1_poly);
  Free_poly_data(&_pgen_tmp_278_744._c0_poly);
  Free_poly_data(&_pgen_tmp_280_746._c1_poly);
  Free_poly_data(&_pgen_tmp_280_746._c0_poly);
  Free_poly_data(&_pgen_tmp_281_747._poly);
  Free_poly_data(&_preg_268435496._c1_poly);
  Free_poly_data(&_preg_268435496._c0_poly);
  Free_poly_data(&result_n35_202._c1_poly);
  Free_poly_data(&result_n35_202._c0_poly);
  Free_poly_data(&orig_result35_209._c1_poly);
  Free_poly_data(&orig_result35_209._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_3582_408._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_3582_408._c0_poly);
  Free_poly_data(&_pgen_tmp_283_749._poly);
  Free_poly_data(&_pgen_tmp_282_748._c1_poly);
  Free_poly_data(&_pgen_tmp_282_748._c0_poly);
  Free_poly_data(&_pgen_tmp_284_750._c1_poly);
  Free_poly_data(&_pgen_tmp_284_750._c0_poly);
  Free_poly_data(&comb_row_result36_210._c1_poly);
  Free_poly_data(&comb_row_result36_210._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_3615_409._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_3615_409._c0_poly);
  Free_poly_data(&_pgen_tmp_286_752._poly);
  Free_poly_data(&_pgen_tmp_285_751._c1_poly);
  Free_poly_data(&_pgen_tmp_285_751._c0_poly);
  Free_poly_data(&_pgen_tmp_287_753._c1_poly);
  Free_poly_data(&_pgen_tmp_287_753._c0_poly);
  Free_poly_data(&comb_rc_result36_212._c1_poly);
  Free_poly_data(&comb_rc_result36_212._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_3648_410._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_3648_410._c0_poly);
  Free_poly_data(&_pgen_tmp_289_755._poly);
  Free_poly_data(&_pgen_tmp_288_754._c1_poly);
  Free_poly_data(&_pgen_tmp_288_754._c0_poly);
  Free_poly_data(&_pgen_tmp_290_756._c1_poly);
  Free_poly_data(&_pgen_tmp_290_756._c0_poly);
  Free_poly_data(&final_result36_214._c1_poly);
  Free_poly_data(&final_result36_214._c0_poly);
  Free_poly_data(&_pgen_tmp_291_757._c1_poly);
  Free_poly_data(&_pgen_tmp_291_757._c0_poly);
  for (rescale_iv_456 = 0; rescale_iv_456 < 8; rescale_iv_456 = rescale_iv_456 + 1) {
    _pgen_tmp_293_759 = VS_out_conv11_199[rescale_iv_456];
    Init_ciph_down_scale(&_pgen_tmp_294_760, &_pgen_tmp_293_759);
    Rescale(&_pgen_tmp_294_760._c0_poly, &_pgen_tmp_293_759._c0_poly);
    Rescale(&_pgen_tmp_294_760._c1_poly, &_pgen_tmp_293_759._c1_poly);
    Copy_ciph(&VS_out_conv11_199[rescale_iv_456], &_pgen_tmp_294_760);
  }
  Free_poly_data(&_pgen_tmp_294_760._c1_poly);
  Free_poly_data(&_pgen_tmp_294_760._c0_poly);
  for (reshardm_n11_216 = 0; reshardm_n11_216 < 2; reshardm_n11_216 = reshardm_n11_216 + 1) {
    Tm_start("Tensor::concat");
    Tm_start("Tensor::concat");
    Tm_start("Tensor::concat");
    _pgen_tmp_296_762 = VS_out_conv11_199[(reshardm_n11_216 * 4)];
    _pgen_tmp_297_763 = VS_out_conv11_199[((reshardm_n11_216 * 4) + 1)];
    _preg_268435606 = Rotate(_pgen_tmp_297_763, -6272);
    Init_ciph_same_scale(&_pgen_tmp_298_764, &_pgen_tmp_296_762, &_preg_268435606);
    _pgen_tmp_300_766 = VS_out_conv11_199[((reshardm_n11_216 * 4) + 2)];
    _pgen_tmp_301_767 = VS_out_conv11_199[((reshardm_n11_216 * 4) + 3)];
    _preg_268435607 = Rotate(_pgen_tmp_301_767, -6272);
    Init_ciph_same_scale(&_pgen_tmp_299_765, &_pgen_tmp_300_766, &_preg_268435607);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&_pgen_tmp_299_765._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_299_765._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_300_766._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435607._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_299_765._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_300_766._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435607._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    _preg_268435608 = Rotate(_pgen_tmp_299_765, -12544);
    Init_ciph_same_scale(&_pgen_tmp_295_761, &_pgen_tmp_298_764, &_preg_268435608);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&_pgen_tmp_295_761._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_298_764._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_296_762._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435606._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_298_764._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_296_762._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435606._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_295_761._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_298_764._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435608._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_295_761._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_298_764._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435608._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    Copy_ciph(&output_reshard11_217[reshardm_n11_216], &_pgen_tmp_295_761);
    Tm_taken("Tensor::concat");
    Tm_taken("Tensor::concat");
    Tm_taken("Tensor::concat");
    Free_poly_data(&_preg_268435606._c1_poly);
    Free_poly_data(&_preg_268435606._c0_poly);
    Free_poly_data(&_preg_268435607._c1_poly);
    Free_poly_data(&_preg_268435607._c0_poly);
    Free_poly_data(&_preg_268435608._c1_poly);
    Free_poly_data(&_preg_268435608._c0_poly);
  }
  Free_ciph_poly(VS_out_conv11_199, 8);
  Free_poly_data(&_pgen_tmp_299_765._c1_poly);
  Free_poly_data(&_pgen_tmp_299_765._c0_poly);
  Free_poly_data(&_pgen_tmp_298_764._c1_poly);
  Free_poly_data(&_pgen_tmp_298_764._c0_poly);
  Free_poly_data(&_pgen_tmp_295_761._c1_poly);
  Free_poly_data(&_pgen_tmp_295_761._c0_poly);
  for (add_bias_n11_218 = 0; add_bias_n11_218 < 2; add_bias_n11_218 = add_bias_n11_218 + 1) {
    Tm_start("Tensor::add");
    _ckks_gen_tmp_3749_411 = output_reshard11_217[add_bias_n11_218];
    Pt_from_msg_ofst(&_pgen_tmp_303_769, 67/* cst_302 */, (add_bias_n11_218) * (25088), 25088, 1, 1);
    Init_ciph_same_scale_plain(&_pgen_tmp_302_768, &_ckks_gen_tmp_3749_411, &_pgen_tmp_303_769);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&_pgen_tmp_302_768._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_302_768._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_3749_411._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_303_769._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Set_coeffs(&_pgen_tmp_302_768._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_ckks_gen_tmp_3749_411._c1_poly, _pgen_rns_idx_465, degree));
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    Copy_ciph(&addshard11_219[add_bias_n11_218], &_pgen_tmp_302_768);
    Tm_taken("Tensor::add");
  }
  Free_ciph_poly(output_reshard11_217, 2);
  Free_poly_data(&_pgen_tmp_303_769._poly);
  Free_poly_data(&_pgen_tmp_302_768._c1_poly);
  Free_poly_data(&_pgen_tmp_302_768._c0_poly);
  memset(&VS_out_conv12_220, 0, sizeof(VS_out_conv12_220));
  for (mesh_x_n12_221 = 0; mesh_x_n12_221 < 8; mesh_x_n12_221 = mesh_x_n12_221 + 1) {
    for (mesh_y_n12_222 = 0; mesh_y_n12_222 < 4; mesh_y_n12_222 = mesh_y_n12_222 + 1) {
      Tm_start("Tensor::add");
      Tm_start("Tensor::conv");
      Copy_ciph(&_preg_268435497, &VS_out_relu10_196[mesh_y_n12_222]);
      Zero_ciph(&result_n37_223);
      Copy_ciph(&input_dup_n37_224, &_preg_268435497);
      for (cap_block_input_grid_n37_225 = 0; cap_block_input_grid_n37_225 < 9; cap_block_input_grid_n37_225 = cap_block_input_grid_n37_225 + 1) {
        _preg_268435609 = Rotate(input_dup_n37_224, _cst_309[cap_block_input_grid_n37_225]);
        Copy_ciph(&input_grid_n37_226[cap_block_input_grid_n37_225], &_preg_268435609);
        Free_poly_data(&_preg_268435609._c1_poly);
        Free_poly_data(&_preg_268435609._c0_poly);
      }
      for (num_grid_n37_227 = 0; num_grid_n37_227 < 32; num_grid_n37_227 = num_grid_n37_227 + 1) {
        Zero_ciph(&result_grid_n37_228);
        for (cap_block_n37_229 = 0; cap_block_n37_229 < 9; cap_block_n37_229 = cap_block_n37_229 + 1) {
          _ckks_gen_tmp_3844_412 = input_grid_n37_226[cap_block_n37_229];
          Pt_from_msg_ofst(&_pgen_tmp_304_770, 68/* cst_306 */, ((((mesh_x_n12_221 * 4) + mesh_y_n12_222) * 288) + (cap_block_n37_229 + (num_grid_n37_227 * 9))) * (25088), 25088, 1, 6);
          Init_ciph_up_scale_plain(&_pgen_tmp_305_771, &_ckks_gen_tmp_3844_412, &_pgen_tmp_304_770);
          Init_ciph_same_scale(&result_grid_n37_228, &result_grid_n37_228, &_pgen_tmp_305_771);
          {
            _pgen_modulus_463 = Q_modulus();
            _pgen_num_q_464 = Poly_level(&result_grid_n37_228._c0_poly);
            for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
              Hw_modmul(Coeffs(&_pgen_tmp_305_771._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_3844_412._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_304_770._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modmul(Coeffs(&_pgen_tmp_305_771._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_3844_412._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_304_770._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n37_228._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n37_228._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_305_771._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n37_228._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n37_228._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_305_771._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              _pgen_modulus_463 = _pgen_modulus_463 + 1;
            }
          }
        }
        _preg_268435610 = Rotate(result_grid_n37_228, num_grid_n37_227 * 784);
        Init_ciph_same_scale(&result_n37_223, &result_n37_223, &_preg_268435610);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&result_n37_223._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modadd(Coeffs(&result_n37_223._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n37_223._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435610._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&result_n37_223._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n37_223._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435610._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435610._c1_poly);
        Free_poly_data(&_preg_268435610._c0_poly);
      }
      Init_ciph_down_scale(&_pgen_tmp_306_772, &result_n37_223);
      Rescale(&_pgen_tmp_306_772._c0_poly, &result_n37_223._c0_poly);
      Rescale(&_pgen_tmp_306_772._c1_poly, &result_n37_223._c1_poly);
      Pt_from_msg(&_pgen_tmp_307_773, 69 /* cst_310 */, 25088, 1, 5);
      Init_ciph_up_scale_plain(&_preg_268435498, &_pgen_tmp_306_772, &_pgen_tmp_307_773);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435498._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_preg_268435498._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_306_772._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_307_773._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_preg_268435498._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_306_772._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_307_773._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      _preg_268435611 = Rotate(result_n37_223, -25088);
      Copy_ciph(&_ckks_gen_tmp_3896_413, &_preg_268435611);
      Init_ciph_down_scale(&_pgen_tmp_308_774, &_ckks_gen_tmp_3896_413);
      Rescale(&_pgen_tmp_308_774._c0_poly, &_ckks_gen_tmp_3896_413._c0_poly);
      Rescale(&_pgen_tmp_308_774._c1_poly, &_ckks_gen_tmp_3896_413._c1_poly);
      Pt_from_msg(&_pgen_tmp_309_775, 70 /* cst_311 */, 25088, 1, 5);
      Init_ciph_up_scale_plain(&_pgen_tmp_310_776, &_pgen_tmp_308_774, &_pgen_tmp_309_775);
      Init_ciph_same_scale(&_preg_268435498, &_preg_268435498, &_pgen_tmp_310_776);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435498._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_310_776._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_308_774._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_309_775._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_310_776._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_308_774._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_309_775._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435498._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435498._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_310_776._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435498._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435498._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_310_776._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Pt_from_msg(&_pgen_tmp_311_777, 71 /* cst_308 */, 25088, 2, 5);
      Init_ciph_same_scale_plain(&result_n37_223, &_preg_268435498, &_pgen_tmp_311_777);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&result_n37_223._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&result_n37_223._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435498._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_311_777._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Set_coeffs(&result_n37_223._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_preg_268435498._c1_poly, _pgen_rns_idx_465, degree));
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&orig_result37_230, &result_n37_223);
      Zero_ciph(&comb_row_result38_231);
      for (combine_row_index_n38_232 = 0; combine_row_index_n38_232 < 14; combine_row_index_n38_232 = combine_row_index_n38_232 + 1) {
        _preg_268435612 = Rotate(orig_result37_230, combine_row_index_n38_232 * 1);
        Copy_ciph(&_ckks_gen_tmp_3938_414, &_preg_268435612);
        Init_ciph_down_scale(&_pgen_tmp_312_778, &_ckks_gen_tmp_3938_414);
        Rescale(&_pgen_tmp_312_778._c0_poly, &_ckks_gen_tmp_3938_414._c0_poly);
        Rescale(&_pgen_tmp_312_778._c1_poly, &_ckks_gen_tmp_3938_414._c1_poly);
        Pt_from_msg_ofst(&_pgen_tmp_313_779, 72/* cst_312 */, (combine_row_index_n38_232) * (25088), 25088, 1, 4);
        Init_ciph_up_scale_plain(&_pgen_tmp_314_780, &_pgen_tmp_312_778, &_pgen_tmp_313_779);
        Init_ciph_same_scale(&comb_row_result38_231, &comb_row_result38_231, &_pgen_tmp_314_780);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&comb_row_result38_231._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modmul(Coeffs(&_pgen_tmp_314_780._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_312_778._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_313_779._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modmul(Coeffs(&_pgen_tmp_314_780._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_312_778._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_313_779._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&comb_row_result38_231._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_row_result38_231._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_314_780._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&comb_row_result38_231._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_row_result38_231._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_314_780._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435612._c1_poly);
        Free_poly_data(&_preg_268435612._c0_poly);
      }
      Zero_ciph(&comb_rc_result38_233);
      for (combine_rc_index_n38_234 = 0; combine_rc_index_n38_234 < 14; combine_rc_index_n38_234 = combine_rc_index_n38_234 + 1) {
        _preg_268435613 = Rotate(comb_row_result38_231, combine_rc_index_n38_234 * 42);
        Copy_ciph(&_ckks_gen_tmp_3971_415, &_preg_268435613);
        Init_ciph_down_scale(&_pgen_tmp_315_781, &_ckks_gen_tmp_3971_415);
        Rescale(&_pgen_tmp_315_781._c0_poly, &_ckks_gen_tmp_3971_415._c0_poly);
        Rescale(&_pgen_tmp_315_781._c1_poly, &_ckks_gen_tmp_3971_415._c1_poly);
        Pt_from_msg_ofst(&_pgen_tmp_316_782, 73/* cst_313 */, (combine_rc_index_n38_234) * (25088), 25088, 1, 3);
        Init_ciph_up_scale_plain(&_pgen_tmp_317_783, &_pgen_tmp_315_781, &_pgen_tmp_316_782);
        Init_ciph_same_scale(&comb_rc_result38_233, &comb_rc_result38_233, &_pgen_tmp_317_783);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&comb_rc_result38_233._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modmul(Coeffs(&_pgen_tmp_317_783._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_315_781._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_316_782._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modmul(Coeffs(&_pgen_tmp_317_783._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_315_781._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_316_782._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&comb_rc_result38_233._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_rc_result38_233._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_317_783._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&comb_rc_result38_233._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_rc_result38_233._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_317_783._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435613._c1_poly);
        Free_poly_data(&_preg_268435613._c0_poly);
      }
      Zero_ciph(&final_result38_235);
      for (combine_cc_index_n38_236 = 0; combine_cc_index_n38_236 < 32; combine_cc_index_n38_236 = combine_cc_index_n38_236 + 1) {
        _preg_268435614 = Rotate(comb_rc_result38_233, combine_cc_index_n38_236 * 588);
        Copy_ciph(&_ckks_gen_tmp_4004_416, &_preg_268435614);
        Init_ciph_down_scale(&_pgen_tmp_318_784, &_ckks_gen_tmp_4004_416);
        Rescale(&_pgen_tmp_318_784._c0_poly, &_ckks_gen_tmp_4004_416._c0_poly);
        Rescale(&_pgen_tmp_318_784._c1_poly, &_ckks_gen_tmp_4004_416._c1_poly);
        Pt_from_msg_ofst(&_pgen_tmp_319_785, 74/* cst_314 */, (combine_cc_index_n38_236) * (25088), 25088, 1, 2);
        Init_ciph_up_scale_plain(&_pgen_tmp_320_786, &_pgen_tmp_318_784, &_pgen_tmp_319_785);
        Init_ciph_same_scale(&final_result38_235, &final_result38_235, &_pgen_tmp_320_786);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&final_result38_235._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modmul(Coeffs(&_pgen_tmp_320_786._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_318_784._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_319_785._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modmul(Coeffs(&_pgen_tmp_320_786._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_318_784._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_319_785._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&final_result38_235._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&final_result38_235._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_320_786._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&final_result38_235._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&final_result38_235._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_320_786._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435614._c1_poly);
        Free_poly_data(&_preg_268435614._c0_poly);
      }
      _pgen_tmp_322_788 = VS_out_conv12_220[mesh_x_n12_221];
      Init_ciph_same_scale(&_pgen_tmp_321_787, &_pgen_tmp_322_788, &final_result38_235);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_321_787._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_321_787._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_322_788._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&final_result38_235._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_321_787._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_322_788._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&final_result38_235._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_conv12_220[mesh_x_n12_221], &_pgen_tmp_321_787);
      Tm_taken("Tensor::conv");
      Tm_taken("Tensor::add");
      Free_poly_data(&_preg_268435611._c1_poly);
      Free_poly_data(&_preg_268435611._c0_poly);
    }
  }
  Free_ciph_poly(VS_out_relu10_196, 4);
  Free_poly_data(&_preg_268435497._c1_poly);
  Free_poly_data(&_preg_268435497._c0_poly);
  Free_poly_data(&input_dup_n37_224._c1_poly);
  Free_poly_data(&input_dup_n37_224._c0_poly);
  Free_ciph_poly(input_grid_n37_226, 9);
  Free_poly_data(&_pgen_tmp_304_770._poly);
  Free_poly_data(&_pgen_tmp_305_771._c1_poly);
  Free_poly_data(&_pgen_tmp_305_771._c0_poly);
  Free_poly_data(&result_grid_n37_228._c1_poly);
  Free_poly_data(&result_grid_n37_228._c0_poly);
  Free_poly_data(&_pgen_tmp_307_773._poly);
  Free_poly_data(&_pgen_tmp_306_772._c1_poly);
  Free_poly_data(&_pgen_tmp_306_772._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_3896_413._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_3896_413._c0_poly);
  Free_poly_data(&_pgen_tmp_309_775._poly);
  Free_poly_data(&_pgen_tmp_308_774._c1_poly);
  Free_poly_data(&_pgen_tmp_308_774._c0_poly);
  Free_poly_data(&_pgen_tmp_310_776._c1_poly);
  Free_poly_data(&_pgen_tmp_310_776._c0_poly);
  Free_poly_data(&_pgen_tmp_311_777._poly);
  Free_poly_data(&_preg_268435498._c1_poly);
  Free_poly_data(&_preg_268435498._c0_poly);
  Free_poly_data(&result_n37_223._c1_poly);
  Free_poly_data(&result_n37_223._c0_poly);
  Free_poly_data(&orig_result37_230._c1_poly);
  Free_poly_data(&orig_result37_230._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_3938_414._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_3938_414._c0_poly);
  Free_poly_data(&_pgen_tmp_313_779._poly);
  Free_poly_data(&_pgen_tmp_312_778._c1_poly);
  Free_poly_data(&_pgen_tmp_312_778._c0_poly);
  Free_poly_data(&_pgen_tmp_314_780._c1_poly);
  Free_poly_data(&_pgen_tmp_314_780._c0_poly);
  Free_poly_data(&comb_row_result38_231._c1_poly);
  Free_poly_data(&comb_row_result38_231._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_3971_415._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_3971_415._c0_poly);
  Free_poly_data(&_pgen_tmp_316_782._poly);
  Free_poly_data(&_pgen_tmp_315_781._c1_poly);
  Free_poly_data(&_pgen_tmp_315_781._c0_poly);
  Free_poly_data(&_pgen_tmp_317_783._c1_poly);
  Free_poly_data(&_pgen_tmp_317_783._c0_poly);
  Free_poly_data(&comb_rc_result38_233._c1_poly);
  Free_poly_data(&comb_rc_result38_233._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_4004_416._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_4004_416._c0_poly);
  Free_poly_data(&_pgen_tmp_319_785._poly);
  Free_poly_data(&_pgen_tmp_318_784._c1_poly);
  Free_poly_data(&_pgen_tmp_318_784._c0_poly);
  Free_poly_data(&_pgen_tmp_320_786._c1_poly);
  Free_poly_data(&_pgen_tmp_320_786._c0_poly);
  Free_poly_data(&final_result38_235._c1_poly);
  Free_poly_data(&final_result38_235._c0_poly);
  Free_poly_data(&_pgen_tmp_321_787._c1_poly);
  Free_poly_data(&_pgen_tmp_321_787._c0_poly);
  for (rescale_iv_457 = 0; rescale_iv_457 < 8; rescale_iv_457 = rescale_iv_457 + 1) {
    _pgen_tmp_323_789 = VS_out_conv12_220[rescale_iv_457];
    Init_ciph_down_scale(&_pgen_tmp_324_790, &_pgen_tmp_323_789);
    Rescale(&_pgen_tmp_324_790._c0_poly, &_pgen_tmp_323_789._c0_poly);
    Rescale(&_pgen_tmp_324_790._c1_poly, &_pgen_tmp_323_789._c1_poly);
    Copy_ciph(&VS_out_conv12_220[rescale_iv_457], &_pgen_tmp_324_790);
  }
  Free_poly_data(&_pgen_tmp_324_790._c1_poly);
  Free_poly_data(&_pgen_tmp_324_790._c0_poly);
  for (reshardm_n12_237 = 0; reshardm_n12_237 < 2; reshardm_n12_237 = reshardm_n12_237 + 1) {
    Tm_start("Tensor::concat");
    Tm_start("Tensor::concat");
    Tm_start("Tensor::concat");
    _pgen_tmp_326_792 = VS_out_conv12_220[(reshardm_n12_237 * 4)];
    _pgen_tmp_327_793 = VS_out_conv12_220[((reshardm_n12_237 * 4) + 1)];
    _preg_268435615 = Rotate(_pgen_tmp_327_793, -6272);
    Init_ciph_same_scale(&_pgen_tmp_328_794, &_pgen_tmp_326_792, &_preg_268435615);
    _pgen_tmp_330_796 = VS_out_conv12_220[((reshardm_n12_237 * 4) + 2)];
    _pgen_tmp_331_797 = VS_out_conv12_220[((reshardm_n12_237 * 4) + 3)];
    _preg_268435616 = Rotate(_pgen_tmp_331_797, -6272);
    Init_ciph_same_scale(&_pgen_tmp_329_795, &_pgen_tmp_330_796, &_preg_268435616);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&_pgen_tmp_329_795._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_329_795._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_330_796._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435616._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_329_795._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_330_796._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435616._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    _preg_268435617 = Rotate(_pgen_tmp_329_795, -12544);
    Init_ciph_same_scale(&_pgen_tmp_325_791, &_pgen_tmp_328_794, &_preg_268435617);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&_pgen_tmp_325_791._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_328_794._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_326_792._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435615._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_328_794._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_326_792._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435615._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_325_791._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_328_794._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435617._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_325_791._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_328_794._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435617._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    Copy_ciph(&output_reshard12_238[reshardm_n12_237], &_pgen_tmp_325_791);
    Tm_taken("Tensor::concat");
    Tm_taken("Tensor::concat");
    Tm_taken("Tensor::concat");
    Free_poly_data(&_preg_268435615._c1_poly);
    Free_poly_data(&_preg_268435615._c0_poly);
    Free_poly_data(&_preg_268435616._c1_poly);
    Free_poly_data(&_preg_268435616._c0_poly);
    Free_poly_data(&_preg_268435617._c1_poly);
    Free_poly_data(&_preg_268435617._c0_poly);
  }
  Free_ciph_poly(VS_out_conv12_220, 8);
  Free_poly_data(&_pgen_tmp_329_795._c1_poly);
  Free_poly_data(&_pgen_tmp_329_795._c0_poly);
  Free_poly_data(&_pgen_tmp_328_794._c1_poly);
  Free_poly_data(&_pgen_tmp_328_794._c0_poly);
  Free_poly_data(&_pgen_tmp_325_791._c1_poly);
  Free_poly_data(&_pgen_tmp_325_791._c0_poly);
  for (add_bias_n12_239 = 0; add_bias_n12_239 < 2; add_bias_n12_239 = add_bias_n12_239 + 1) {
    Tm_start("Tensor::add");
    _ckks_gen_tmp_4105_417 = output_reshard12_238[add_bias_n12_239];
    Pt_from_msg_ofst(&_pgen_tmp_333_799, 75/* cst_319 */, (add_bias_n12_239) * (25088), 25088, 1, 1);
    Init_ciph_same_scale_plain(&_pgen_tmp_332_798, &_ckks_gen_tmp_4105_417, &_pgen_tmp_333_799);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&_pgen_tmp_332_798._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_332_798._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_4105_417._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_333_799._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Set_coeffs(&_pgen_tmp_332_798._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_ckks_gen_tmp_4105_417._c1_poly, _pgen_rns_idx_465, degree));
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    Copy_ciph(&addshard12_240[add_bias_n12_239], &_pgen_tmp_332_798);
    Tm_taken("Tensor::add");
  }
  Free_ciph_poly(output_reshard12_238, 2);
  Free_poly_data(&_pgen_tmp_333_799._poly);
  Free_poly_data(&_pgen_tmp_332_798._c1_poly);
  Free_poly_data(&_pgen_tmp_332_798._c0_poly);
  memset(&VS_out_relu12_241, 0, sizeof(VS_out_relu12_241));
  for (mesh_x_n12_242 = 0; mesh_x_n12_242 < 2; mesh_x_n12_242 = mesh_x_n12_242 + 1) {
    for (mesh_y_n12_243 = 0; mesh_y_n12_243 < 1; mesh_y_n12_243 = mesh_y_n12_243 + 1) {
      Tm_start("Tensor::add");
      _pgen_tmp_334_800 = addshard12_240[((mesh_x_n12_242 * 1) + mesh_y_n12_243)];
      Bootstrap(&_preg_268435499, &_pgen_tmp_334_800, 15);
      Encode_float(&_pgen_tmp_336_802, &_cst_471, 1, 1, Level(&_preg_268435499));
      Init_ciph_up_scale_plain(&_pgen_tmp_335_801, &_preg_268435499, &_pgen_tmp_336_802);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_335_801._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_335_801._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435499._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_336_802._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_335_801._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435499._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_336_802._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Init_ciph_down_scale(&_pgen_tmp_337_803, &_pgen_tmp_335_801);
      Rescale(&_pgen_tmp_337_803._c0_poly, &_pgen_tmp_335_801._c0_poly);
      Rescale(&_pgen_tmp_337_803._c1_poly, &_pgen_tmp_335_801._c1_poly);
      _preg_268435500 = App_relu(_preg_268435499, _pgen_tmp_337_803, 25088);
      _pgen_tmp_339_805 = VS_out_relu12_241[((mesh_x_n12_242 * 1) + mesh_y_n12_243)];
      Init_ciph_same_scale(&_pgen_tmp_338_804, &_pgen_tmp_339_805, &_preg_268435500);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_338_804._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_338_804._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_339_805._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435500._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_338_804._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_339_805._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435500._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_relu12_241[((mesh_x_n12_242 * 1) + mesh_y_n12_243)], &_pgen_tmp_338_804);
      Tm_taken("Tensor::add");
    }
  }
  Free_ciph_poly(addshard12_240, 2);
  Free_poly_data(&_pgen_tmp_336_802._poly);
  Free_poly_data(&_pgen_tmp_335_801._c1_poly);
  Free_poly_data(&_pgen_tmp_335_801._c0_poly);
  Free_poly_data(&_pgen_tmp_337_803._c1_poly);
  Free_poly_data(&_pgen_tmp_337_803._c0_poly);
  Free_poly_data(&_preg_268435499._c1_poly);
  Free_poly_data(&_preg_268435499._c0_poly);
  Free_poly_data(&_preg_268435500._c1_poly);
  Free_poly_data(&_preg_268435500._c0_poly);
  Free_poly_data(&_pgen_tmp_338_804._c1_poly);
  Free_poly_data(&_pgen_tmp_338_804._c0_poly);
  memset(&VS_out_conv13_244, 0, sizeof(VS_out_conv13_244));
  for (mesh_x_n13_245 = 0; mesh_x_n13_245 < 2; mesh_x_n13_245 = mesh_x_n13_245 + 1) {
    for (mesh_y_n13_246 = 0; mesh_y_n13_246 < 2; mesh_y_n13_246 = mesh_y_n13_246 + 1) {
      Tm_start("Tensor::add");
      Tm_start("Tensor::conv");
      Copy_ciph(&_preg_268435501, &VS_out_relu12_241[mesh_y_n13_246]);
      Zero_ciph(&result_n39_247);
      Copy_ciph(&input_dup_n39_248, &_preg_268435501);
      for (cap_block_input_grid_n39_249 = 0; cap_block_input_grid_n39_249 < 9; cap_block_input_grid_n39_249 = cap_block_input_grid_n39_249 + 1) {
        _preg_268435618 = Rotate(input_dup_n39_248, _cst_327[cap_block_input_grid_n39_249]);
        Copy_ciph(&input_grid_n39_250[cap_block_input_grid_n39_249], &_preg_268435618);
        Free_poly_data(&_preg_268435618._c1_poly);
        Free_poly_data(&_preg_268435618._c0_poly);
      }
      for (num_grid_n39_251 = 0; num_grid_n39_251 < 128; num_grid_n39_251 = num_grid_n39_251 + 1) {
        Zero_ciph(&result_grid_n39_252);
        for (cap_block_n39_253 = 0; cap_block_n39_253 < 9; cap_block_n39_253 = cap_block_n39_253 + 1) {
          _ckks_gen_tmp_4258_418 = input_grid_n39_250[cap_block_n39_253];
          Pt_from_msg_ofst(&_pgen_tmp_340_806, 76/* cst_324 */, ((((mesh_x_n13_245 * 2) + mesh_y_n13_246) * 1152) + (cap_block_n39_253 + (num_grid_n39_251 * 9))) * (25088), 25088, 1, 3);
          Init_ciph_up_scale_plain(&_pgen_tmp_341_807, &_ckks_gen_tmp_4258_418, &_pgen_tmp_340_806);
          Init_ciph_same_scale(&result_grid_n39_252, &result_grid_n39_252, &_pgen_tmp_341_807);
          {
            _pgen_modulus_463 = Q_modulus();
            _pgen_num_q_464 = Poly_level(&result_grid_n39_252._c0_poly);
            for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
              Hw_modmul(Coeffs(&_pgen_tmp_341_807._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_4258_418._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_340_806._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modmul(Coeffs(&_pgen_tmp_341_807._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_4258_418._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_340_806._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n39_252._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n39_252._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_341_807._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n39_252._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n39_252._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_341_807._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              _pgen_modulus_463 = _pgen_modulus_463 + 1;
            }
          }
        }
        _preg_268435619 = Rotate(result_grid_n39_252, num_grid_n39_251 * 196);
        Init_ciph_same_scale(&result_n39_247, &result_n39_247, &_preg_268435619);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&result_n39_247._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modadd(Coeffs(&result_n39_247._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n39_247._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435619._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&result_n39_247._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n39_247._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435619._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435619._c1_poly);
        Free_poly_data(&_preg_268435619._c0_poly);
      }
      Init_ciph_down_scale(&_pgen_tmp_342_808, &result_n39_247);
      Rescale(&_pgen_tmp_342_808._c0_poly, &result_n39_247._c0_poly);
      Rescale(&_pgen_tmp_342_808._c1_poly, &result_n39_247._c1_poly);
      Pt_from_msg(&_pgen_tmp_343_809, 77 /* cst_328 */, 25088, 1, 2);
      Init_ciph_up_scale_plain(&_preg_268435502, &_pgen_tmp_342_808, &_pgen_tmp_343_809);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435502._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_preg_268435502._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_342_808._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_343_809._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_preg_268435502._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_342_808._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_343_809._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      _preg_268435620 = Rotate(result_n39_247, -25088);
      Copy_ciph(&_ckks_gen_tmp_4310_419, &_preg_268435620);
      Init_ciph_down_scale(&_pgen_tmp_344_810, &_ckks_gen_tmp_4310_419);
      Rescale(&_pgen_tmp_344_810._c0_poly, &_ckks_gen_tmp_4310_419._c0_poly);
      Rescale(&_pgen_tmp_344_810._c1_poly, &_ckks_gen_tmp_4310_419._c1_poly);
      Pt_from_msg(&_pgen_tmp_345_811, 78 /* cst_329 */, 25088, 1, 2);
      Init_ciph_up_scale_plain(&_pgen_tmp_346_812, &_pgen_tmp_344_810, &_pgen_tmp_345_811);
      Init_ciph_same_scale(&_preg_268435502, &_preg_268435502, &_pgen_tmp_346_812);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435502._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_346_812._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_344_810._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_345_811._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_346_812._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_344_810._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_345_811._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435502._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435502._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_346_812._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435502._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435502._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_346_812._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Pt_from_msg(&_pgen_tmp_347_813, 79 /* cst_326 */, 25088, 2, 2);
      Init_ciph_same_scale_plain(&result_n39_247, &_preg_268435502, &_pgen_tmp_347_813);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&result_n39_247._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&result_n39_247._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435502._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_347_813._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Set_coeffs(&result_n39_247._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_preg_268435502._c1_poly, _pgen_rns_idx_465, degree));
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      _pgen_tmp_349_815 = VS_out_conv13_244[mesh_x_n13_245];
      Init_ciph_same_scale(&_pgen_tmp_348_814, &_pgen_tmp_349_815, &result_n39_247);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_348_814._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_348_814._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_349_815._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n39_247._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_348_814._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_349_815._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n39_247._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_conv13_244[mesh_x_n13_245], &_pgen_tmp_348_814);
      Tm_taken("Tensor::conv");
      Tm_taken("Tensor::add");
      Free_poly_data(&_preg_268435620._c1_poly);
      Free_poly_data(&_preg_268435620._c0_poly);
    }
  }
  Free_ciph_poly(VS_out_relu12_241, 2);
  Free_poly_data(&_preg_268435501._c1_poly);
  Free_poly_data(&_preg_268435501._c0_poly);
  Free_poly_data(&input_dup_n39_248._c1_poly);
  Free_poly_data(&input_dup_n39_248._c0_poly);
  Free_ciph_poly(input_grid_n39_250, 9);
  Free_poly_data(&_pgen_tmp_340_806._poly);
  Free_poly_data(&_pgen_tmp_341_807._c1_poly);
  Free_poly_data(&_pgen_tmp_341_807._c0_poly);
  Free_poly_data(&result_grid_n39_252._c1_poly);
  Free_poly_data(&result_grid_n39_252._c0_poly);
  Free_poly_data(&_pgen_tmp_343_809._poly);
  Free_poly_data(&_pgen_tmp_342_808._c1_poly);
  Free_poly_data(&_pgen_tmp_342_808._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_4310_419._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_4310_419._c0_poly);
  Free_poly_data(&_pgen_tmp_345_811._poly);
  Free_poly_data(&_pgen_tmp_344_810._c1_poly);
  Free_poly_data(&_pgen_tmp_344_810._c0_poly);
  Free_poly_data(&_pgen_tmp_346_812._c1_poly);
  Free_poly_data(&_pgen_tmp_346_812._c0_poly);
  Free_poly_data(&_pgen_tmp_347_813._poly);
  Free_poly_data(&_preg_268435502._c1_poly);
  Free_poly_data(&_preg_268435502._c0_poly);
  Free_poly_data(&result_n39_247._c1_poly);
  Free_poly_data(&result_n39_247._c0_poly);
  Free_poly_data(&_pgen_tmp_348_814._c1_poly);
  Free_poly_data(&_pgen_tmp_348_814._c0_poly);
  for (rescale_iv_458 = 0; rescale_iv_458 < 2; rescale_iv_458 = rescale_iv_458 + 1) {
    _pgen_tmp_350_816 = VS_out_conv13_244[rescale_iv_458];
    Init_ciph_down_scale(&_pgen_tmp_351_817, &_pgen_tmp_350_816);
    Rescale(&_pgen_tmp_351_817._c0_poly, &_pgen_tmp_350_816._c0_poly);
    Rescale(&_pgen_tmp_351_817._c1_poly, &_pgen_tmp_350_816._c1_poly);
    Copy_ciph(&VS_out_conv13_244[rescale_iv_458], &_pgen_tmp_351_817);
  }
  Free_poly_data(&_pgen_tmp_351_817._c1_poly);
  Free_poly_data(&_pgen_tmp_351_817._c0_poly);
  for (add_bias_n13_254 = 0; add_bias_n13_254 < 2; add_bias_n13_254 = add_bias_n13_254 + 1) {
    Tm_start("Tensor::add");
    _ckks_gen_tmp_4359_420 = VS_out_conv13_244[add_bias_n13_254];
    Pt_from_msg_ofst(&_pgen_tmp_353_819, 80/* cst_331 */, (add_bias_n13_254) * (25088), 25088, 1, 1);
    Init_ciph_same_scale_plain(&_pgen_tmp_352_818, &_ckks_gen_tmp_4359_420, &_pgen_tmp_353_819);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&_pgen_tmp_352_818._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_352_818._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_4359_420._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_353_819._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Set_coeffs(&_pgen_tmp_352_818._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_ckks_gen_tmp_4359_420._c1_poly, _pgen_rns_idx_465, degree));
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    Copy_ciph(&addshard13_255[add_bias_n13_254], &_pgen_tmp_352_818);
    Tm_taken("Tensor::add");
  }
  Free_ciph_poly(VS_out_conv13_244, 2);
  Free_poly_data(&_pgen_tmp_353_819._poly);
  Free_poly_data(&_pgen_tmp_352_818._c1_poly);
  Free_poly_data(&_pgen_tmp_352_818._c0_poly);
  memset(&VS_out_add13_256, 0, sizeof(VS_out_add13_256));
  for (mesh_x_n13_257 = 0; mesh_x_n13_257 < 2; mesh_x_n13_257 = mesh_x_n13_257 + 1) {
    for (mesh_y_n13_258 = 0; mesh_y_n13_258 < 1; mesh_y_n13_258 = mesh_y_n13_258 + 1) {
      Tm_start("Tensor::add");
      Tm_start("Tensor::add");
      _pgen_tmp_355_821 = VS_out_add13_256[((mesh_x_n13_257 * 1) + mesh_y_n13_258)];
      _pgen_tmp_356_822 = addshard13_255[((mesh_x_n13_257 * 1) + mesh_y_n13_258)];
      _pgen_tmp_357_823 = addshard11_219[((mesh_x_n13_257 * 1) + mesh_y_n13_258)];
      Init_ciph_same_scale(&_pgen_tmp_358_824, &_pgen_tmp_356_822, &_pgen_tmp_357_823);
      Init_ciph_same_scale(&_pgen_tmp_354_820, &_pgen_tmp_355_821, &_pgen_tmp_358_824);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_354_820._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_358_824._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_356_822._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_357_823._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_358_824._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_356_822._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_357_823._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_354_820._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_355_821._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_358_824._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_354_820._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_355_821._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_358_824._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_add13_256[((mesh_x_n13_257 * 1) + mesh_y_n13_258)], &_pgen_tmp_354_820);
      Tm_taken("Tensor::add");
      Tm_taken("Tensor::add");
    }
  }
  Free_ciph_poly(addshard13_255, 2);
  Free_ciph_poly(addshard11_219, 2);
  Free_poly_data(&_pgen_tmp_358_824._c1_poly);
  Free_poly_data(&_pgen_tmp_358_824._c0_poly);
  Free_poly_data(&_pgen_tmp_354_820._c1_poly);
  Free_poly_data(&_pgen_tmp_354_820._c0_poly);
  memset(&VS_out_relu13_259, 0, sizeof(VS_out_relu13_259));
  for (mesh_x_n13_260 = 0; mesh_x_n13_260 < 2; mesh_x_n13_260 = mesh_x_n13_260 + 1) {
    for (mesh_y_n13_261 = 0; mesh_y_n13_261 < 1; mesh_y_n13_261 = mesh_y_n13_261 + 1) {
      Tm_start("Tensor::add");
      _pgen_tmp_359_825 = VS_out_add13_256[((mesh_x_n13_260 * 1) + mesh_y_n13_261)];
      Bootstrap(&_preg_268435503, &_pgen_tmp_359_825, 15);
      Encode_float(&_pgen_tmp_361_827, &_cst_472, 1, 1, Level(&_preg_268435503));
      Init_ciph_up_scale_plain(&_pgen_tmp_360_826, &_preg_268435503, &_pgen_tmp_361_827);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_360_826._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_360_826._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435503._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_361_827._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_360_826._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435503._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_361_827._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Init_ciph_down_scale(&_pgen_tmp_362_828, &_pgen_tmp_360_826);
      Rescale(&_pgen_tmp_362_828._c0_poly, &_pgen_tmp_360_826._c0_poly);
      Rescale(&_pgen_tmp_362_828._c1_poly, &_pgen_tmp_360_826._c1_poly);
      _preg_268435504 = App_relu(_preg_268435503, _pgen_tmp_362_828, 25088);
      _pgen_tmp_364_830 = VS_out_relu13_259[((mesh_x_n13_260 * 1) + mesh_y_n13_261)];
      Init_ciph_same_scale(&_pgen_tmp_363_829, &_pgen_tmp_364_830, &_preg_268435504);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_363_829._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_363_829._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_364_830._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435504._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_363_829._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_364_830._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435504._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_relu13_259[((mesh_x_n13_260 * 1) + mesh_y_n13_261)], &_pgen_tmp_363_829);
      Tm_taken("Tensor::add");
    }
  }
  Free_ciph_poly(VS_out_add13_256, 2);
  Free_poly_data(&_pgen_tmp_361_827._poly);
  Free_poly_data(&_pgen_tmp_360_826._c1_poly);
  Free_poly_data(&_pgen_tmp_360_826._c0_poly);
  Free_poly_data(&_pgen_tmp_362_828._c1_poly);
  Free_poly_data(&_pgen_tmp_362_828._c0_poly);
  Free_poly_data(&_preg_268435503._c1_poly);
  Free_poly_data(&_preg_268435503._c0_poly);
  Free_poly_data(&_preg_268435504._c1_poly);
  Free_poly_data(&_preg_268435504._c0_poly);
  Free_poly_data(&_pgen_tmp_363_829._c1_poly);
  Free_poly_data(&_pgen_tmp_363_829._c0_poly);
  memset(&VS_out_conv14_262, 0, sizeof(VS_out_conv14_262));
  for (mesh_x_n14_263 = 0; mesh_x_n14_263 < 2; mesh_x_n14_263 = mesh_x_n14_263 + 1) {
    for (mesh_y_n14_264 = 0; mesh_y_n14_264 < 2; mesh_y_n14_264 = mesh_y_n14_264 + 1) {
      Tm_start("Tensor::add");
      Tm_start("Tensor::conv");
      Copy_ciph(&_preg_268435505, &VS_out_relu13_259[mesh_y_n14_264]);
      Zero_ciph(&result_n40_265);
      Copy_ciph(&input_dup_n40_266, &_preg_268435505);
      for (cap_block_input_grid_n40_267 = 0; cap_block_input_grid_n40_267 < 9; cap_block_input_grid_n40_267 = cap_block_input_grid_n40_267 + 1) {
        _preg_268435621 = Rotate(input_dup_n40_266, _cst_341[cap_block_input_grid_n40_267]);
        Copy_ciph(&input_grid_n40_268[cap_block_input_grid_n40_267], &_preg_268435621);
        Free_poly_data(&_preg_268435621._c1_poly);
        Free_poly_data(&_preg_268435621._c0_poly);
      }
      for (num_grid_n40_269 = 0; num_grid_n40_269 < 128; num_grid_n40_269 = num_grid_n40_269 + 1) {
        Zero_ciph(&result_grid_n40_270);
        for (cap_block_n40_271 = 0; cap_block_n40_271 < 9; cap_block_n40_271 = cap_block_n40_271 + 1) {
          _ckks_gen_tmp_4572_421 = input_grid_n40_268[cap_block_n40_271];
          Pt_from_msg_ofst(&_pgen_tmp_365_831, 81/* cst_338 */, ((((mesh_x_n14_263 * 2) + mesh_y_n14_264) * 1152) + (cap_block_n40_271 + (num_grid_n40_269 * 9))) * (25088), 25088, 1, 3);
          Init_ciph_up_scale_plain(&_pgen_tmp_366_832, &_ckks_gen_tmp_4572_421, &_pgen_tmp_365_831);
          Init_ciph_same_scale(&result_grid_n40_270, &result_grid_n40_270, &_pgen_tmp_366_832);
          {
            _pgen_modulus_463 = Q_modulus();
            _pgen_num_q_464 = Poly_level(&result_grid_n40_270._c0_poly);
            for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
              Hw_modmul(Coeffs(&_pgen_tmp_366_832._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_4572_421._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_365_831._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modmul(Coeffs(&_pgen_tmp_366_832._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_4572_421._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_365_831._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n40_270._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n40_270._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_366_832._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n40_270._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n40_270._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_366_832._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              _pgen_modulus_463 = _pgen_modulus_463 + 1;
            }
          }
        }
        _preg_268435622 = Rotate(result_grid_n40_270, num_grid_n40_269 * 196);
        Init_ciph_same_scale(&result_n40_265, &result_n40_265, &_preg_268435622);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&result_n40_265._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modadd(Coeffs(&result_n40_265._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n40_265._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435622._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&result_n40_265._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n40_265._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435622._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435622._c1_poly);
        Free_poly_data(&_preg_268435622._c0_poly);
      }
      Init_ciph_down_scale(&_pgen_tmp_367_833, &result_n40_265);
      Rescale(&_pgen_tmp_367_833._c0_poly, &result_n40_265._c0_poly);
      Rescale(&_pgen_tmp_367_833._c1_poly, &result_n40_265._c1_poly);
      Pt_from_msg(&_pgen_tmp_368_834, 82 /* cst_342 */, 25088, 1, 2);
      Init_ciph_up_scale_plain(&_preg_268435506, &_pgen_tmp_367_833, &_pgen_tmp_368_834);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435506._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_preg_268435506._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_367_833._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_368_834._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_preg_268435506._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_367_833._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_368_834._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      _preg_268435623 = Rotate(result_n40_265, -25088);
      Copy_ciph(&_ckks_gen_tmp_4624_422, &_preg_268435623);
      Init_ciph_down_scale(&_pgen_tmp_369_835, &_ckks_gen_tmp_4624_422);
      Rescale(&_pgen_tmp_369_835._c0_poly, &_ckks_gen_tmp_4624_422._c0_poly);
      Rescale(&_pgen_tmp_369_835._c1_poly, &_ckks_gen_tmp_4624_422._c1_poly);
      Pt_from_msg(&_pgen_tmp_370_836, 83 /* cst_343 */, 25088, 1, 2);
      Init_ciph_up_scale_plain(&_pgen_tmp_371_837, &_pgen_tmp_369_835, &_pgen_tmp_370_836);
      Init_ciph_same_scale(&_preg_268435506, &_preg_268435506, &_pgen_tmp_371_837);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435506._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_371_837._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_369_835._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_370_836._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_371_837._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_369_835._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_370_836._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435506._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435506._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_371_837._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435506._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435506._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_371_837._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Pt_from_msg(&_pgen_tmp_372_838, 84 /* cst_340 */, 25088, 2, 2);
      Init_ciph_same_scale_plain(&result_n40_265, &_preg_268435506, &_pgen_tmp_372_838);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&result_n40_265._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&result_n40_265._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435506._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_372_838._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Set_coeffs(&result_n40_265._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_preg_268435506._c1_poly, _pgen_rns_idx_465, degree));
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      _pgen_tmp_374_840 = VS_out_conv14_262[mesh_x_n14_263];
      Init_ciph_same_scale(&_pgen_tmp_373_839, &_pgen_tmp_374_840, &result_n40_265);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_373_839._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_373_839._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_374_840._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n40_265._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_373_839._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_374_840._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n40_265._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_conv14_262[mesh_x_n14_263], &_pgen_tmp_373_839);
      Tm_taken("Tensor::conv");
      Tm_taken("Tensor::add");
      Free_poly_data(&_preg_268435623._c1_poly);
      Free_poly_data(&_preg_268435623._c0_poly);
    }
  }
  Free_poly_data(&_preg_268435505._c1_poly);
  Free_poly_data(&_preg_268435505._c0_poly);
  Free_poly_data(&input_dup_n40_266._c1_poly);
  Free_poly_data(&input_dup_n40_266._c0_poly);
  Free_ciph_poly(input_grid_n40_268, 9);
  Free_poly_data(&_pgen_tmp_365_831._poly);
  Free_poly_data(&_pgen_tmp_366_832._c1_poly);
  Free_poly_data(&_pgen_tmp_366_832._c0_poly);
  Free_poly_data(&result_grid_n40_270._c1_poly);
  Free_poly_data(&result_grid_n40_270._c0_poly);
  Free_poly_data(&_pgen_tmp_368_834._poly);
  Free_poly_data(&_pgen_tmp_367_833._c1_poly);
  Free_poly_data(&_pgen_tmp_367_833._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_4624_422._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_4624_422._c0_poly);
  Free_poly_data(&_pgen_tmp_370_836._poly);
  Free_poly_data(&_pgen_tmp_369_835._c1_poly);
  Free_poly_data(&_pgen_tmp_369_835._c0_poly);
  Free_poly_data(&_pgen_tmp_371_837._c1_poly);
  Free_poly_data(&_pgen_tmp_371_837._c0_poly);
  Free_poly_data(&_pgen_tmp_372_838._poly);
  Free_poly_data(&_preg_268435506._c1_poly);
  Free_poly_data(&_preg_268435506._c0_poly);
  Free_poly_data(&result_n40_265._c1_poly);
  Free_poly_data(&result_n40_265._c0_poly);
  Free_poly_data(&_pgen_tmp_373_839._c1_poly);
  Free_poly_data(&_pgen_tmp_373_839._c0_poly);
  for (rescale_iv_459 = 0; rescale_iv_459 < 2; rescale_iv_459 = rescale_iv_459 + 1) {
    _pgen_tmp_375_841 = VS_out_conv14_262[rescale_iv_459];
    Init_ciph_down_scale(&_pgen_tmp_376_842, &_pgen_tmp_375_841);
    Rescale(&_pgen_tmp_376_842._c0_poly, &_pgen_tmp_375_841._c0_poly);
    Rescale(&_pgen_tmp_376_842._c1_poly, &_pgen_tmp_375_841._c1_poly);
    Copy_ciph(&VS_out_conv14_262[rescale_iv_459], &_pgen_tmp_376_842);
  }
  Free_poly_data(&_pgen_tmp_376_842._c1_poly);
  Free_poly_data(&_pgen_tmp_376_842._c0_poly);
  for (add_bias_n14_272 = 0; add_bias_n14_272 < 2; add_bias_n14_272 = add_bias_n14_272 + 1) {
    Tm_start("Tensor::add");
    _ckks_gen_tmp_4673_423 = VS_out_conv14_262[add_bias_n14_272];
    Pt_from_msg_ofst(&_pgen_tmp_378_844, 85/* cst_345 */, (add_bias_n14_272) * (25088), 25088, 1, 1);
    Init_ciph_same_scale_plain(&_pgen_tmp_377_843, &_ckks_gen_tmp_4673_423, &_pgen_tmp_378_844);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&_pgen_tmp_377_843._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_377_843._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_4673_423._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_378_844._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Set_coeffs(&_pgen_tmp_377_843._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_ckks_gen_tmp_4673_423._c1_poly, _pgen_rns_idx_465, degree));
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    Copy_ciph(&addshard14_273[add_bias_n14_272], &_pgen_tmp_377_843);
    Tm_taken("Tensor::add");
  }
  Free_ciph_poly(VS_out_conv14_262, 2);
  Free_poly_data(&_pgen_tmp_378_844._poly);
  Free_poly_data(&_pgen_tmp_377_843._c1_poly);
  Free_poly_data(&_pgen_tmp_377_843._c0_poly);
  memset(&VS_out_relu14_274, 0, sizeof(VS_out_relu14_274));
  for (mesh_x_n14_275 = 0; mesh_x_n14_275 < 2; mesh_x_n14_275 = mesh_x_n14_275 + 1) {
    for (mesh_y_n14_276 = 0; mesh_y_n14_276 < 1; mesh_y_n14_276 = mesh_y_n14_276 + 1) {
      Tm_start("Tensor::add");
      _pgen_tmp_379_845 = addshard14_273[((mesh_x_n14_275 * 1) + mesh_y_n14_276)];
      Bootstrap(&_preg_268435507, &_pgen_tmp_379_845, 15);
      Encode_float(&_pgen_tmp_381_847, &_cst_473, 1, 1, Level(&_preg_268435507));
      Init_ciph_up_scale_plain(&_pgen_tmp_380_846, &_preg_268435507, &_pgen_tmp_381_847);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_380_846._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_380_846._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435507._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_381_847._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_380_846._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435507._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_381_847._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Init_ciph_down_scale(&_pgen_tmp_382_848, &_pgen_tmp_380_846);
      Rescale(&_pgen_tmp_382_848._c0_poly, &_pgen_tmp_380_846._c0_poly);
      Rescale(&_pgen_tmp_382_848._c1_poly, &_pgen_tmp_380_846._c1_poly);
      _preg_268435508 = App_relu(_preg_268435507, _pgen_tmp_382_848, 25088);
      _pgen_tmp_384_850 = VS_out_relu14_274[((mesh_x_n14_275 * 1) + mesh_y_n14_276)];
      Init_ciph_same_scale(&_pgen_tmp_383_849, &_pgen_tmp_384_850, &_preg_268435508);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_383_849._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_383_849._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_384_850._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435508._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_383_849._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_384_850._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435508._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_relu14_274[((mesh_x_n14_275 * 1) + mesh_y_n14_276)], &_pgen_tmp_383_849);
      Tm_taken("Tensor::add");
    }
  }
  Free_ciph_poly(addshard14_273, 2);
  Free_poly_data(&_pgen_tmp_381_847._poly);
  Free_poly_data(&_pgen_tmp_380_846._c1_poly);
  Free_poly_data(&_pgen_tmp_380_846._c0_poly);
  Free_poly_data(&_pgen_tmp_382_848._c1_poly);
  Free_poly_data(&_pgen_tmp_382_848._c0_poly);
  Free_poly_data(&_preg_268435507._c1_poly);
  Free_poly_data(&_preg_268435507._c0_poly);
  Free_poly_data(&_preg_268435508._c1_poly);
  Free_poly_data(&_preg_268435508._c0_poly);
  Free_poly_data(&_pgen_tmp_383_849._c1_poly);
  Free_poly_data(&_pgen_tmp_383_849._c0_poly);
  memset(&VS_out_conv15_277, 0, sizeof(VS_out_conv15_277));
  for (mesh_x_n15_278 = 0; mesh_x_n15_278 < 2; mesh_x_n15_278 = mesh_x_n15_278 + 1) {
    for (mesh_y_n15_279 = 0; mesh_y_n15_279 < 2; mesh_y_n15_279 = mesh_y_n15_279 + 1) {
      Tm_start("Tensor::add");
      Tm_start("Tensor::conv");
      Copy_ciph(&_preg_268435509, &VS_out_relu14_274[mesh_y_n15_279]);
      Zero_ciph(&result_n41_280);
      Copy_ciph(&input_dup_n41_281, &_preg_268435509);
      for (cap_block_input_grid_n41_282 = 0; cap_block_input_grid_n41_282 < 9; cap_block_input_grid_n41_282 = cap_block_input_grid_n41_282 + 1) {
        _preg_268435624 = Rotate(input_dup_n41_281, _cst_353[cap_block_input_grid_n41_282]);
        Copy_ciph(&input_grid_n41_283[cap_block_input_grid_n41_282], &_preg_268435624);
        Free_poly_data(&_preg_268435624._c1_poly);
        Free_poly_data(&_preg_268435624._c0_poly);
      }
      for (num_grid_n41_284 = 0; num_grid_n41_284 < 128; num_grid_n41_284 = num_grid_n41_284 + 1) {
        Zero_ciph(&result_grid_n41_285);
        for (cap_block_n41_286 = 0; cap_block_n41_286 < 9; cap_block_n41_286 = cap_block_n41_286 + 1) {
          _ckks_gen_tmp_4826_424 = input_grid_n41_283[cap_block_n41_286];
          Pt_from_msg_ofst(&_pgen_tmp_385_851, 86/* cst_350 */, ((((mesh_x_n15_278 * 2) + mesh_y_n15_279) * 1152) + (cap_block_n41_286 + (num_grid_n41_284 * 9))) * (25088), 25088, 1, 3);
          Init_ciph_up_scale_plain(&_pgen_tmp_386_852, &_ckks_gen_tmp_4826_424, &_pgen_tmp_385_851);
          Init_ciph_same_scale(&result_grid_n41_285, &result_grid_n41_285, &_pgen_tmp_386_852);
          {
            _pgen_modulus_463 = Q_modulus();
            _pgen_num_q_464 = Poly_level(&result_grid_n41_285._c0_poly);
            for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
              Hw_modmul(Coeffs(&_pgen_tmp_386_852._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_4826_424._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_385_851._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modmul(Coeffs(&_pgen_tmp_386_852._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_4826_424._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_385_851._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n41_285._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n41_285._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_386_852._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n41_285._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n41_285._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_386_852._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              _pgen_modulus_463 = _pgen_modulus_463 + 1;
            }
          }
        }
        _preg_268435625 = Rotate(result_grid_n41_285, num_grid_n41_284 * 196);
        Init_ciph_same_scale(&result_n41_280, &result_n41_280, &_preg_268435625);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&result_n41_280._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modadd(Coeffs(&result_n41_280._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n41_280._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435625._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&result_n41_280._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n41_280._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435625._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435625._c1_poly);
        Free_poly_data(&_preg_268435625._c0_poly);
      }
      Init_ciph_down_scale(&_pgen_tmp_387_853, &result_n41_280);
      Rescale(&_pgen_tmp_387_853._c0_poly, &result_n41_280._c0_poly);
      Rescale(&_pgen_tmp_387_853._c1_poly, &result_n41_280._c1_poly);
      Pt_from_msg(&_pgen_tmp_388_854, 87 /* cst_354 */, 25088, 1, 2);
      Init_ciph_up_scale_plain(&_preg_268435510, &_pgen_tmp_387_853, &_pgen_tmp_388_854);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435510._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_preg_268435510._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_387_853._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_388_854._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_preg_268435510._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_387_853._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_388_854._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      _preg_268435626 = Rotate(result_n41_280, -25088);
      Copy_ciph(&_ckks_gen_tmp_4878_425, &_preg_268435626);
      Init_ciph_down_scale(&_pgen_tmp_389_855, &_ckks_gen_tmp_4878_425);
      Rescale(&_pgen_tmp_389_855._c0_poly, &_ckks_gen_tmp_4878_425._c0_poly);
      Rescale(&_pgen_tmp_389_855._c1_poly, &_ckks_gen_tmp_4878_425._c1_poly);
      Pt_from_msg(&_pgen_tmp_390_856, 88 /* cst_355 */, 25088, 1, 2);
      Init_ciph_up_scale_plain(&_pgen_tmp_391_857, &_pgen_tmp_389_855, &_pgen_tmp_390_856);
      Init_ciph_same_scale(&_preg_268435510, &_preg_268435510, &_pgen_tmp_391_857);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435510._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_391_857._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_389_855._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_390_856._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_391_857._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_389_855._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_390_856._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435510._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435510._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_391_857._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435510._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435510._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_391_857._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Pt_from_msg(&_pgen_tmp_392_858, 89 /* cst_352 */, 25088, 2, 2);
      Init_ciph_same_scale_plain(&result_n41_280, &_preg_268435510, &_pgen_tmp_392_858);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&result_n41_280._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&result_n41_280._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435510._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_392_858._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Set_coeffs(&result_n41_280._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_preg_268435510._c1_poly, _pgen_rns_idx_465, degree));
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      _pgen_tmp_394_860 = VS_out_conv15_277[mesh_x_n15_278];
      Init_ciph_same_scale(&_pgen_tmp_393_859, &_pgen_tmp_394_860, &result_n41_280);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_393_859._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_393_859._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_394_860._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n41_280._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_393_859._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_394_860._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n41_280._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_conv15_277[mesh_x_n15_278], &_pgen_tmp_393_859);
      Tm_taken("Tensor::conv");
      Tm_taken("Tensor::add");
      Free_poly_data(&_preg_268435626._c1_poly);
      Free_poly_data(&_preg_268435626._c0_poly);
    }
  }
  Free_ciph_poly(VS_out_relu14_274, 2);
  Free_poly_data(&_preg_268435509._c1_poly);
  Free_poly_data(&_preg_268435509._c0_poly);
  Free_poly_data(&input_dup_n41_281._c1_poly);
  Free_poly_data(&input_dup_n41_281._c0_poly);
  Free_ciph_poly(input_grid_n41_283, 9);
  Free_poly_data(&_pgen_tmp_385_851._poly);
  Free_poly_data(&_pgen_tmp_386_852._c1_poly);
  Free_poly_data(&_pgen_tmp_386_852._c0_poly);
  Free_poly_data(&result_grid_n41_285._c1_poly);
  Free_poly_data(&result_grid_n41_285._c0_poly);
  Free_poly_data(&_pgen_tmp_388_854._poly);
  Free_poly_data(&_pgen_tmp_387_853._c1_poly);
  Free_poly_data(&_pgen_tmp_387_853._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_4878_425._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_4878_425._c0_poly);
  Free_poly_data(&_pgen_tmp_390_856._poly);
  Free_poly_data(&_pgen_tmp_389_855._c1_poly);
  Free_poly_data(&_pgen_tmp_389_855._c0_poly);
  Free_poly_data(&_pgen_tmp_391_857._c1_poly);
  Free_poly_data(&_pgen_tmp_391_857._c0_poly);
  Free_poly_data(&_pgen_tmp_392_858._poly);
  Free_poly_data(&_preg_268435510._c1_poly);
  Free_poly_data(&_preg_268435510._c0_poly);
  Free_poly_data(&result_n41_280._c1_poly);
  Free_poly_data(&result_n41_280._c0_poly);
  Free_poly_data(&_pgen_tmp_393_859._c1_poly);
  Free_poly_data(&_pgen_tmp_393_859._c0_poly);
  for (rescale_iv_460 = 0; rescale_iv_460 < 2; rescale_iv_460 = rescale_iv_460 + 1) {
    _pgen_tmp_395_861 = VS_out_conv15_277[rescale_iv_460];
    Init_ciph_down_scale(&_pgen_tmp_396_862, &_pgen_tmp_395_861);
    Rescale(&_pgen_tmp_396_862._c0_poly, &_pgen_tmp_395_861._c0_poly);
    Rescale(&_pgen_tmp_396_862._c1_poly, &_pgen_tmp_395_861._c1_poly);
    Copy_ciph(&VS_out_conv15_277[rescale_iv_460], &_pgen_tmp_396_862);
  }
  Free_poly_data(&_pgen_tmp_396_862._c1_poly);
  Free_poly_data(&_pgen_tmp_396_862._c0_poly);
  for (add_bias_n15_287 = 0; add_bias_n15_287 < 2; add_bias_n15_287 = add_bias_n15_287 + 1) {
    Tm_start("Tensor::add");
    _ckks_gen_tmp_4927_426 = VS_out_conv15_277[add_bias_n15_287];
    Pt_from_msg_ofst(&_pgen_tmp_398_864, 90/* cst_357 */, (add_bias_n15_287) * (25088), 25088, 1, 1);
    Init_ciph_same_scale_plain(&_pgen_tmp_397_863, &_ckks_gen_tmp_4927_426, &_pgen_tmp_398_864);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&_pgen_tmp_397_863._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_397_863._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_4927_426._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_398_864._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Set_coeffs(&_pgen_tmp_397_863._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_ckks_gen_tmp_4927_426._c1_poly, _pgen_rns_idx_465, degree));
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    Copy_ciph(&addshard15_288[add_bias_n15_287], &_pgen_tmp_397_863);
    Tm_taken("Tensor::add");
  }
  Free_ciph_poly(VS_out_conv15_277, 2);
  Free_poly_data(&_pgen_tmp_398_864._poly);
  Free_poly_data(&_pgen_tmp_397_863._c1_poly);
  Free_poly_data(&_pgen_tmp_397_863._c0_poly);
  memset(&VS_out_add15_289, 0, sizeof(VS_out_add15_289));
  for (mesh_x_n15_290 = 0; mesh_x_n15_290 < 2; mesh_x_n15_290 = mesh_x_n15_290 + 1) {
    for (mesh_y_n15_291 = 0; mesh_y_n15_291 < 1; mesh_y_n15_291 = mesh_y_n15_291 + 1) {
      Tm_start("Tensor::add");
      Tm_start("Tensor::add");
      _pgen_tmp_400_866 = VS_out_add15_289[((mesh_x_n15_290 * 1) + mesh_y_n15_291)];
      _pgen_tmp_401_867 = addshard15_288[((mesh_x_n15_290 * 1) + mesh_y_n15_291)];
      _pgen_tmp_402_868 = VS_out_relu13_259[((mesh_x_n15_290 * 1) + mesh_y_n15_291)];
      Init_ciph_same_scale(&_pgen_tmp_403_869, &_pgen_tmp_401_867, &_pgen_tmp_402_868);
      Init_ciph_same_scale(&_pgen_tmp_399_865, &_pgen_tmp_400_866, &_pgen_tmp_403_869);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_399_865._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_403_869._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_401_867._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_402_868._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_403_869._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_401_867._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_402_868._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_399_865._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_400_866._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_403_869._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_399_865._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_400_866._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_403_869._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_add15_289[((mesh_x_n15_290 * 1) + mesh_y_n15_291)], &_pgen_tmp_399_865);
      Tm_taken("Tensor::add");
      Tm_taken("Tensor::add");
    }
  }
  Free_ciph_poly(addshard15_288, 2);
  Free_ciph_poly(VS_out_relu13_259, 2);
  Free_poly_data(&_pgen_tmp_403_869._c1_poly);
  Free_poly_data(&_pgen_tmp_403_869._c0_poly);
  Free_poly_data(&_pgen_tmp_399_865._c1_poly);
  Free_poly_data(&_pgen_tmp_399_865._c0_poly);
  memset(&VS_out_relu15_292, 0, sizeof(VS_out_relu15_292));
  for (mesh_x_n15_293 = 0; mesh_x_n15_293 < 2; mesh_x_n15_293 = mesh_x_n15_293 + 1) {
    for (mesh_y_n15_294 = 0; mesh_y_n15_294 < 1; mesh_y_n15_294 = mesh_y_n15_294 + 1) {
      Tm_start("Tensor::add");
      _pgen_tmp_404_870 = VS_out_add15_289[((mesh_x_n15_293 * 1) + mesh_y_n15_294)];
      Bootstrap(&_preg_268435511, &_pgen_tmp_404_870, 18);
      Encode_float(&_pgen_tmp_406_872, &_cst_474, 1, 1, Level(&_preg_268435511));
      Init_ciph_up_scale_plain(&_pgen_tmp_405_871, &_preg_268435511, &_pgen_tmp_406_872);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_405_871._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_405_871._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435511._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_406_872._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_405_871._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435511._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_406_872._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Init_ciph_down_scale(&_pgen_tmp_407_873, &_pgen_tmp_405_871);
      Rescale(&_pgen_tmp_407_873._c0_poly, &_pgen_tmp_405_871._c0_poly);
      Rescale(&_pgen_tmp_407_873._c1_poly, &_pgen_tmp_405_871._c1_poly);
      _preg_268435512 = App_relu(_preg_268435511, _pgen_tmp_407_873, 25088);
      _pgen_tmp_409_875 = VS_out_relu15_292[((mesh_x_n15_293 * 1) + mesh_y_n15_294)];
      Init_ciph_same_scale(&_pgen_tmp_408_874, &_pgen_tmp_409_875, &_preg_268435512);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_408_874._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_408_874._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_409_875._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435512._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_408_874._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_409_875._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435512._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_relu15_292[((mesh_x_n15_293 * 1) + mesh_y_n15_294)], &_pgen_tmp_408_874);
      Tm_taken("Tensor::add");
    }
  }
  Free_ciph_poly(VS_out_add15_289, 2);
  Free_poly_data(&_pgen_tmp_406_872._poly);
  Free_poly_data(&_pgen_tmp_405_871._c1_poly);
  Free_poly_data(&_pgen_tmp_405_871._c0_poly);
  Free_poly_data(&_pgen_tmp_407_873._c1_poly);
  Free_poly_data(&_pgen_tmp_407_873._c0_poly);
  Free_poly_data(&_preg_268435511._c1_poly);
  Free_poly_data(&_preg_268435511._c0_poly);
  Free_poly_data(&_preg_268435512._c1_poly);
  Free_poly_data(&_preg_268435512._c0_poly);
  Free_poly_data(&_pgen_tmp_408_874._c1_poly);
  Free_poly_data(&_pgen_tmp_408_874._c0_poly);
  memset(&VS_out_conv16_295, 0, sizeof(VS_out_conv16_295));
  for (mesh_x_n16_296 = 0; mesh_x_n16_296 < 4; mesh_x_n16_296 = mesh_x_n16_296 + 1) {
    for (mesh_y_n16_297 = 0; mesh_y_n16_297 < 2; mesh_y_n16_297 = mesh_y_n16_297 + 1) {
      Tm_start("Tensor::add");
      Tm_start("Tensor::conv");
      Copy_ciph(&_preg_268435513, &VS_out_relu15_292[mesh_y_n16_297]);
      Zero_ciph(&result_n42_298);
      _preg_268435627 = Rotate(_preg_268435513, -25088);
      Init_ciph_same_scale(&input_dup_n42_299, &_preg_268435513, &_preg_268435627);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&input_dup_n42_299._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&input_dup_n42_299._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435513._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435627._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&input_dup_n42_299._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435513._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435627._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      for (cap_block_input_grid_n42_300 = 0; cap_block_input_grid_n42_300 < 8; cap_block_input_grid_n42_300 = cap_block_input_grid_n42_300 + 1) {
        _preg_268435628 = Rotate(input_dup_n42_299, _cst_367[cap_block_input_grid_n42_300]);
        Copy_ciph(&input_grid_n42_301[cap_block_input_grid_n42_300], &_preg_268435628);
        Free_poly_data(&_preg_268435628._c1_poly);
        Free_poly_data(&_preg_268435628._c0_poly);
      }
      for (num_grid_n42_302 = 0; num_grid_n42_302 < 16; num_grid_n42_302 = num_grid_n42_302 + 1) {
        Zero_ciph(&result_grid_n42_303);
        for (cap_block_n42_304 = 0; cap_block_n42_304 < 8; cap_block_n42_304 = cap_block_n42_304 + 1) {
          _ckks_gen_tmp_5144_427 = input_grid_n42_301[cap_block_n42_304];
          Pt_from_msg_ofst(&_pgen_tmp_410_876, 91/* cst_364 */, ((((mesh_x_n16_296 * 2) + mesh_y_n16_297) * 128) + (cap_block_n42_304 + (num_grid_n42_302 * 8))) * (25088), 25088, 1, 6);
          Init_ciph_up_scale_plain(&_pgen_tmp_411_877, &_ckks_gen_tmp_5144_427, &_pgen_tmp_410_876);
          Init_ciph_same_scale(&result_grid_n42_303, &result_grid_n42_303, &_pgen_tmp_411_877);
          {
            _pgen_modulus_463 = Q_modulus();
            _pgen_num_q_464 = Poly_level(&result_grid_n42_303._c0_poly);
            for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
              Hw_modmul(Coeffs(&_pgen_tmp_411_877._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_5144_427._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_410_876._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modmul(Coeffs(&_pgen_tmp_411_877._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_5144_427._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_410_876._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n42_303._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n42_303._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_411_877._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n42_303._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n42_303._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_411_877._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              _pgen_modulus_463 = _pgen_modulus_463 + 1;
            }
          }
        }
        _preg_268435629 = Rotate(result_grid_n42_303, num_grid_n42_302 * 1568);
        Init_ciph_same_scale(&result_n42_298, &result_n42_298, &_preg_268435629);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&result_n42_298._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modadd(Coeffs(&result_n42_298._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n42_298._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435629._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&result_n42_298._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n42_298._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435629._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435629._c1_poly);
        Free_poly_data(&_preg_268435629._c0_poly);
      }
      Init_ciph_down_scale(&_pgen_tmp_412_878, &result_n42_298);
      Rescale(&_pgen_tmp_412_878._c0_poly, &result_n42_298._c0_poly);
      Rescale(&_pgen_tmp_412_878._c1_poly, &result_n42_298._c1_poly);
      Pt_from_msg(&_pgen_tmp_413_879, 92 /* cst_368 */, 25088, 1, 5);
      Init_ciph_up_scale_plain(&_preg_268435514, &_pgen_tmp_412_878, &_pgen_tmp_413_879);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435514._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_preg_268435514._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_412_878._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_413_879._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_preg_268435514._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_412_878._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_413_879._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      _preg_268435630 = Rotate(result_n42_298, -25088);
      Copy_ciph(&_ckks_gen_tmp_5196_428, &_preg_268435630);
      Init_ciph_down_scale(&_pgen_tmp_414_880, &_ckks_gen_tmp_5196_428);
      Rescale(&_pgen_tmp_414_880._c0_poly, &_ckks_gen_tmp_5196_428._c0_poly);
      Rescale(&_pgen_tmp_414_880._c1_poly, &_ckks_gen_tmp_5196_428._c1_poly);
      Pt_from_msg(&_pgen_tmp_415_881, 93 /* cst_369 */, 25088, 1, 5);
      Init_ciph_up_scale_plain(&_pgen_tmp_416_882, &_pgen_tmp_414_880, &_pgen_tmp_415_881);
      Init_ciph_same_scale(&_preg_268435514, &_preg_268435514, &_pgen_tmp_416_882);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435514._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_416_882._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_414_880._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_415_881._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_416_882._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_414_880._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_415_881._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435514._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435514._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_416_882._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435514._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435514._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_416_882._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Pt_from_msg(&_pgen_tmp_417_883, 94 /* cst_366 */, 25088, 2, 5);
      Init_ciph_same_scale_plain(&result_n42_298, &_preg_268435514, &_pgen_tmp_417_883);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&result_n42_298._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&result_n42_298._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435514._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_417_883._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Set_coeffs(&result_n42_298._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_preg_268435514._c1_poly, _pgen_rns_idx_465, degree));
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&orig_result42_305, &result_n42_298);
      Zero_ciph(&comb_row_result43_306);
      for (combine_row_index_n43_307 = 0; combine_row_index_n43_307 < 7; combine_row_index_n43_307 = combine_row_index_n43_307 + 1) {
        _preg_268435631 = Rotate(orig_result42_305, combine_row_index_n43_307 * 1);
        Copy_ciph(&_ckks_gen_tmp_5238_429, &_preg_268435631);
        Init_ciph_down_scale(&_pgen_tmp_418_884, &_ckks_gen_tmp_5238_429);
        Rescale(&_pgen_tmp_418_884._c0_poly, &_ckks_gen_tmp_5238_429._c0_poly);
        Rescale(&_pgen_tmp_418_884._c1_poly, &_ckks_gen_tmp_5238_429._c1_poly);
        Pt_from_msg_ofst(&_pgen_tmp_419_885, 95/* cst_370 */, (combine_row_index_n43_307) * (25088), 25088, 1, 4);
        Init_ciph_up_scale_plain(&_pgen_tmp_420_886, &_pgen_tmp_418_884, &_pgen_tmp_419_885);
        Init_ciph_same_scale(&comb_row_result43_306, &comb_row_result43_306, &_pgen_tmp_420_886);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&comb_row_result43_306._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modmul(Coeffs(&_pgen_tmp_420_886._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_418_884._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_419_885._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modmul(Coeffs(&_pgen_tmp_420_886._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_418_884._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_419_885._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&comb_row_result43_306._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_row_result43_306._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_420_886._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&comb_row_result43_306._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_row_result43_306._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_420_886._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435631._c1_poly);
        Free_poly_data(&_preg_268435631._c0_poly);
      }
      Zero_ciph(&comb_rc_result43_308);
      for (combine_rc_index_n43_309 = 0; combine_rc_index_n43_309 < 7; combine_rc_index_n43_309 = combine_rc_index_n43_309 + 1) {
        _preg_268435632 = Rotate(comb_row_result43_306, combine_rc_index_n43_309 * 21);
        Copy_ciph(&_ckks_gen_tmp_5271_430, &_preg_268435632);
        Init_ciph_down_scale(&_pgen_tmp_421_887, &_ckks_gen_tmp_5271_430);
        Rescale(&_pgen_tmp_421_887._c0_poly, &_ckks_gen_tmp_5271_430._c0_poly);
        Rescale(&_pgen_tmp_421_887._c1_poly, &_ckks_gen_tmp_5271_430._c1_poly);
        Pt_from_msg_ofst(&_pgen_tmp_422_888, 96/* cst_371 */, (combine_rc_index_n43_309) * (25088), 25088, 1, 3);
        Init_ciph_up_scale_plain(&_pgen_tmp_423_889, &_pgen_tmp_421_887, &_pgen_tmp_422_888);
        Init_ciph_same_scale(&comb_rc_result43_308, &comb_rc_result43_308, &_pgen_tmp_423_889);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&comb_rc_result43_308._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modmul(Coeffs(&_pgen_tmp_423_889._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_421_887._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_422_888._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modmul(Coeffs(&_pgen_tmp_423_889._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_421_887._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_422_888._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&comb_rc_result43_308._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_rc_result43_308._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_423_889._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&comb_rc_result43_308._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_rc_result43_308._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_423_889._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435632._c1_poly);
        Free_poly_data(&_preg_268435632._c0_poly);
      }
      Zero_ciph(&final_result43_310);
      for (combine_cc_index_n43_311 = 0; combine_cc_index_n43_311 < 128; combine_cc_index_n43_311 = combine_cc_index_n43_311 + 1) {
        _preg_268435633 = Rotate(comb_rc_result43_308, combine_cc_index_n43_311 * 147);
        Copy_ciph(&_ckks_gen_tmp_5304_431, &_preg_268435633);
        Init_ciph_down_scale(&_pgen_tmp_424_890, &_ckks_gen_tmp_5304_431);
        Rescale(&_pgen_tmp_424_890._c0_poly, &_ckks_gen_tmp_5304_431._c0_poly);
        Rescale(&_pgen_tmp_424_890._c1_poly, &_ckks_gen_tmp_5304_431._c1_poly);
        Pt_from_msg_ofst(&_pgen_tmp_425_891, 97/* cst_372 */, (combine_cc_index_n43_311) * (25088), 25088, 1, 2);
        Init_ciph_up_scale_plain(&_pgen_tmp_426_892, &_pgen_tmp_424_890, &_pgen_tmp_425_891);
        Init_ciph_same_scale(&final_result43_310, &final_result43_310, &_pgen_tmp_426_892);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&final_result43_310._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modmul(Coeffs(&_pgen_tmp_426_892._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_424_890._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_425_891._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modmul(Coeffs(&_pgen_tmp_426_892._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_424_890._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_425_891._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&final_result43_310._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&final_result43_310._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_426_892._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&final_result43_310._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&final_result43_310._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_426_892._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435633._c1_poly);
        Free_poly_data(&_preg_268435633._c0_poly);
      }
      _pgen_tmp_428_894 = VS_out_conv16_295[mesh_x_n16_296];
      Init_ciph_same_scale(&_pgen_tmp_427_893, &_pgen_tmp_428_894, &final_result43_310);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_427_893._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_427_893._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_428_894._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&final_result43_310._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_427_893._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_428_894._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&final_result43_310._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_conv16_295[mesh_x_n16_296], &_pgen_tmp_427_893);
      Tm_taken("Tensor::conv");
      Tm_taken("Tensor::add");
      Free_poly_data(&_preg_268435627._c1_poly);
      Free_poly_data(&_preg_268435627._c0_poly);
      Free_poly_data(&_preg_268435630._c1_poly);
      Free_poly_data(&_preg_268435630._c0_poly);
    }
  }
  Free_poly_data(&_preg_268435513._c1_poly);
  Free_poly_data(&_preg_268435513._c0_poly);
  Free_poly_data(&input_dup_n42_299._c1_poly);
  Free_poly_data(&input_dup_n42_299._c0_poly);
  Free_ciph_poly(input_grid_n42_301, 8);
  Free_poly_data(&_pgen_tmp_410_876._poly);
  Free_poly_data(&_pgen_tmp_411_877._c1_poly);
  Free_poly_data(&_pgen_tmp_411_877._c0_poly);
  Free_poly_data(&result_grid_n42_303._c1_poly);
  Free_poly_data(&result_grid_n42_303._c0_poly);
  Free_poly_data(&_pgen_tmp_413_879._poly);
  Free_poly_data(&_pgen_tmp_412_878._c1_poly);
  Free_poly_data(&_pgen_tmp_412_878._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_5196_428._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_5196_428._c0_poly);
  Free_poly_data(&_pgen_tmp_415_881._poly);
  Free_poly_data(&_pgen_tmp_414_880._c1_poly);
  Free_poly_data(&_pgen_tmp_414_880._c0_poly);
  Free_poly_data(&_pgen_tmp_416_882._c1_poly);
  Free_poly_data(&_pgen_tmp_416_882._c0_poly);
  Free_poly_data(&_pgen_tmp_417_883._poly);
  Free_poly_data(&_preg_268435514._c1_poly);
  Free_poly_data(&_preg_268435514._c0_poly);
  Free_poly_data(&result_n42_298._c1_poly);
  Free_poly_data(&result_n42_298._c0_poly);
  Free_poly_data(&orig_result42_305._c1_poly);
  Free_poly_data(&orig_result42_305._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_5238_429._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_5238_429._c0_poly);
  Free_poly_data(&_pgen_tmp_419_885._poly);
  Free_poly_data(&_pgen_tmp_418_884._c1_poly);
  Free_poly_data(&_pgen_tmp_418_884._c0_poly);
  Free_poly_data(&_pgen_tmp_420_886._c1_poly);
  Free_poly_data(&_pgen_tmp_420_886._c0_poly);
  Free_poly_data(&comb_row_result43_306._c1_poly);
  Free_poly_data(&comb_row_result43_306._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_5271_430._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_5271_430._c0_poly);
  Free_poly_data(&_pgen_tmp_422_888._poly);
  Free_poly_data(&_pgen_tmp_421_887._c1_poly);
  Free_poly_data(&_pgen_tmp_421_887._c0_poly);
  Free_poly_data(&_pgen_tmp_423_889._c1_poly);
  Free_poly_data(&_pgen_tmp_423_889._c0_poly);
  Free_poly_data(&comb_rc_result43_308._c1_poly);
  Free_poly_data(&comb_rc_result43_308._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_5304_431._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_5304_431._c0_poly);
  Free_poly_data(&_pgen_tmp_425_891._poly);
  Free_poly_data(&_pgen_tmp_424_890._c1_poly);
  Free_poly_data(&_pgen_tmp_424_890._c0_poly);
  Free_poly_data(&_pgen_tmp_426_892._c1_poly);
  Free_poly_data(&_pgen_tmp_426_892._c0_poly);
  Free_poly_data(&final_result43_310._c1_poly);
  Free_poly_data(&final_result43_310._c0_poly);
  Free_poly_data(&_pgen_tmp_427_893._c1_poly);
  Free_poly_data(&_pgen_tmp_427_893._c0_poly);
  for (rescale_iv_461 = 0; rescale_iv_461 < 4; rescale_iv_461 = rescale_iv_461 + 1) {
    _pgen_tmp_429_895 = VS_out_conv16_295[rescale_iv_461];
    Init_ciph_down_scale(&_pgen_tmp_430_896, &_pgen_tmp_429_895);
    Rescale(&_pgen_tmp_430_896._c0_poly, &_pgen_tmp_429_895._c0_poly);
    Rescale(&_pgen_tmp_430_896._c1_poly, &_pgen_tmp_429_895._c1_poly);
    Copy_ciph(&VS_out_conv16_295[rescale_iv_461], &_pgen_tmp_430_896);
  }
  Free_poly_data(&_pgen_tmp_430_896._c1_poly);
  Free_poly_data(&_pgen_tmp_430_896._c0_poly);
  for (reshard1_n16_312 = 0; reshard1_n16_312 < 1; reshard1_n16_312 = reshard1_n16_312 + 1) {
    Tm_start("Tensor::concat");
    Tm_start("Tensor::concat");
    Tm_start("Tensor::concat");
    _pgen_tmp_431_897 = VS_out_conv16_295[reshard1_n16_312];
    _pgen_tmp_432_898 = VS_out_conv16_295[(reshard1_n16_312 + 1)];
    _preg_268435634 = Rotate(_pgen_tmp_432_898, -6272);
    Init_ciph_same_scale(&_pgen_tmp_433_899, &_pgen_tmp_431_897, &_preg_268435634);
    _pgen_tmp_435_901 = VS_out_conv16_295[(reshard1_n16_312 + 2)];
    _pgen_tmp_436_902 = VS_out_conv16_295[(reshard1_n16_312 + 3)];
    _preg_268435635 = Rotate(_pgen_tmp_436_902, -6272);
    Init_ciph_same_scale(&_pgen_tmp_434_900, &_pgen_tmp_435_901, &_preg_268435635);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&_pgen_tmp_434_900._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_434_900._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_435_901._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435635._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_434_900._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_435_901._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435635._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    _preg_268435636 = Rotate(_pgen_tmp_434_900, -12544);
    Init_ciph_same_scale(&output_reshard16_313, &_pgen_tmp_433_899, &_preg_268435636);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&output_reshard16_313._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_433_899._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_431_897._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435634._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_433_899._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_431_897._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435634._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&output_reshard16_313._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_433_899._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435636._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&output_reshard16_313._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_433_899._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435636._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    Tm_taken("Tensor::concat");
    Tm_taken("Tensor::concat");
    Tm_taken("Tensor::concat");
    Free_poly_data(&_preg_268435634._c1_poly);
    Free_poly_data(&_preg_268435634._c0_poly);
    Free_poly_data(&_preg_268435635._c1_poly);
    Free_poly_data(&_preg_268435635._c0_poly);
    Free_poly_data(&_preg_268435636._c1_poly);
    Free_poly_data(&_preg_268435636._c0_poly);
  }
  Free_ciph_poly(VS_out_conv16_295, 4);
  Free_poly_data(&_pgen_tmp_434_900._c1_poly);
  Free_poly_data(&_pgen_tmp_434_900._c0_poly);
  Free_poly_data(&_pgen_tmp_433_899._c1_poly);
  Free_poly_data(&_pgen_tmp_433_899._c0_poly);
  Tm_start("Tensor::add");
  Pt_from_msg(&_pgen_tmp_437_903, 98 /* cst_118 */, 25088, 1, 1);
  Init_ciph_same_scale_plain(&addshard16_314, &output_reshard16_313, &_pgen_tmp_437_903);
  {
    _pgen_modulus_463 = Q_modulus();
    _pgen_num_q_464 = Poly_level(&addshard16_314._c0_poly);
    for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
      Hw_modadd(Coeffs(&addshard16_314._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&output_reshard16_313._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_437_903._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      Set_coeffs(&addshard16_314._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&output_reshard16_313._c1_poly, _pgen_rns_idx_465, degree));
      _pgen_modulus_463 = _pgen_modulus_463 + 1;
    }
    Free_poly_data(&_pgen_tmp_437_903._poly);
    Free_poly_data(&output_reshard16_313._c1_poly);
    Free_poly_data(&output_reshard16_313._c0_poly);
  }
  Tm_taken("Tensor::add");
  memset(&VS_out_conv17_315, 0, sizeof(VS_out_conv17_315));
  for (mesh_x_n17_316 = 0; mesh_x_n17_316 < 4; mesh_x_n17_316 = mesh_x_n17_316 + 1) {
    for (mesh_y_n17_317 = 0; mesh_y_n17_317 < 2; mesh_y_n17_317 = mesh_y_n17_317 + 1) {
      Tm_start("Tensor::add");
      Tm_start("Tensor::conv");
      Copy_ciph(&_preg_268435515, &VS_out_relu15_292[mesh_y_n17_317]);
      Zero_ciph(&result_n44_318);
      Copy_ciph(&input_dup_n44_319, &_preg_268435515);
      for (cap_block_input_grid_n44_320 = 0; cap_block_input_grid_n44_320 < 9; cap_block_input_grid_n44_320 = cap_block_input_grid_n44_320 + 1) {
        _preg_268435637 = Rotate(input_dup_n44_319, _cst_383[cap_block_input_grid_n44_320]);
        Copy_ciph(&input_grid_n44_321[cap_block_input_grid_n44_320], &_preg_268435637);
        Free_poly_data(&_preg_268435637._c1_poly);
        Free_poly_data(&_preg_268435637._c0_poly);
      }
      for (num_grid_n44_322 = 0; num_grid_n44_322 < 128; num_grid_n44_322 = num_grid_n44_322 + 1) {
        Zero_ciph(&result_grid_n44_323);
        for (cap_block_n44_324 = 0; cap_block_n44_324 < 9; cap_block_n44_324 = cap_block_n44_324 + 1) {
          _ckks_gen_tmp_5468_432 = input_grid_n44_321[cap_block_n44_324];
          Pt_from_msg_ofst(&_pgen_tmp_438_904, 99/* cst_380 */, ((((mesh_x_n17_316 * 2) + mesh_y_n17_317) * 1152) + (cap_block_n44_324 + (num_grid_n44_322 * 9))) * (25088), 25088, 1, 6);
          Init_ciph_up_scale_plain(&_pgen_tmp_439_905, &_ckks_gen_tmp_5468_432, &_pgen_tmp_438_904);
          Init_ciph_same_scale(&result_grid_n44_323, &result_grid_n44_323, &_pgen_tmp_439_905);
          {
            _pgen_modulus_463 = Q_modulus();
            _pgen_num_q_464 = Poly_level(&result_grid_n44_323._c0_poly);
            for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
              Hw_modmul(Coeffs(&_pgen_tmp_439_905._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_5468_432._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_438_904._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modmul(Coeffs(&_pgen_tmp_439_905._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_5468_432._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_438_904._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n44_323._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n44_323._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_439_905._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              Hw_modadd(Coeffs(&result_grid_n44_323._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n44_323._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_439_905._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
              _pgen_modulus_463 = _pgen_modulus_463 + 1;
            }
          }
        }
        _preg_268435638 = Rotate(result_grid_n44_323, num_grid_n44_322 * 196);
        Init_ciph_same_scale(&result_n44_318, &result_n44_318, &_preg_268435638);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&result_n44_318._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modadd(Coeffs(&result_n44_318._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n44_318._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435638._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&result_n44_318._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n44_318._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435638._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435638._c1_poly);
        Free_poly_data(&_preg_268435638._c0_poly);
      }
      Init_ciph_down_scale(&_pgen_tmp_440_906, &result_n44_318);
      Rescale(&_pgen_tmp_440_906._c0_poly, &result_n44_318._c0_poly);
      Rescale(&_pgen_tmp_440_906._c1_poly, &result_n44_318._c1_poly);
      Pt_from_msg(&_pgen_tmp_441_907, 100 /* cst_384 */, 25088, 1, 5);
      Init_ciph_up_scale_plain(&_preg_268435516, &_pgen_tmp_440_906, &_pgen_tmp_441_907);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435516._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_preg_268435516._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_440_906._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_441_907._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_preg_268435516._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_440_906._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_441_907._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      _preg_268435639 = Rotate(result_n44_318, -25088);
      Copy_ciph(&_ckks_gen_tmp_5520_433, &_preg_268435639);
      Init_ciph_down_scale(&_pgen_tmp_442_908, &_ckks_gen_tmp_5520_433);
      Rescale(&_pgen_tmp_442_908._c0_poly, &_ckks_gen_tmp_5520_433._c0_poly);
      Rescale(&_pgen_tmp_442_908._c1_poly, &_ckks_gen_tmp_5520_433._c1_poly);
      Pt_from_msg(&_pgen_tmp_443_909, 101 /* cst_385 */, 25088, 1, 5);
      Init_ciph_up_scale_plain(&_pgen_tmp_444_910, &_pgen_tmp_442_908, &_pgen_tmp_443_909);
      Init_ciph_same_scale(&_preg_268435516, &_preg_268435516, &_pgen_tmp_444_910);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_preg_268435516._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_444_910._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_442_908._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_443_909._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_444_910._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_442_908._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_443_909._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435516._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435516._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_444_910._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_preg_268435516._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435516._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_444_910._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Pt_from_msg(&_pgen_tmp_445_911, 102 /* cst_382 */, 25088, 2, 5);
      Init_ciph_same_scale_plain(&result_n44_318, &_preg_268435516, &_pgen_tmp_445_911);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&result_n44_318._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&result_n44_318._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435516._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_445_911._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Set_coeffs(&result_n44_318._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_preg_268435516._c1_poly, _pgen_rns_idx_465, degree));
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&orig_result44_325, &result_n44_318);
      Zero_ciph(&comb_row_result45_326);
      for (combine_row_index_n45_327 = 0; combine_row_index_n45_327 < 7; combine_row_index_n45_327 = combine_row_index_n45_327 + 1) {
        _preg_268435640 = Rotate(orig_result44_325, combine_row_index_n45_327 * 1);
        Copy_ciph(&_ckks_gen_tmp_5562_434, &_preg_268435640);
        Init_ciph_down_scale(&_pgen_tmp_446_912, &_ckks_gen_tmp_5562_434);
        Rescale(&_pgen_tmp_446_912._c0_poly, &_ckks_gen_tmp_5562_434._c0_poly);
        Rescale(&_pgen_tmp_446_912._c1_poly, &_ckks_gen_tmp_5562_434._c1_poly);
        Pt_from_msg_ofst(&_pgen_tmp_447_913, 103/* cst_386 */, (combine_row_index_n45_327) * (25088), 25088, 1, 4);
        Init_ciph_up_scale_plain(&_pgen_tmp_448_914, &_pgen_tmp_446_912, &_pgen_tmp_447_913);
        Init_ciph_same_scale(&comb_row_result45_326, &comb_row_result45_326, &_pgen_tmp_448_914);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&comb_row_result45_326._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modmul(Coeffs(&_pgen_tmp_448_914._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_446_912._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_447_913._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modmul(Coeffs(&_pgen_tmp_448_914._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_446_912._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_447_913._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&comb_row_result45_326._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_row_result45_326._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_448_914._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&comb_row_result45_326._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_row_result45_326._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_448_914._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435640._c1_poly);
        Free_poly_data(&_preg_268435640._c0_poly);
      }
      Zero_ciph(&comb_rc_result45_328);
      for (combine_rc_index_n45_329 = 0; combine_rc_index_n45_329 < 7; combine_rc_index_n45_329 = combine_rc_index_n45_329 + 1) {
        _preg_268435641 = Rotate(comb_row_result45_326, combine_rc_index_n45_329 * 21);
        Copy_ciph(&_ckks_gen_tmp_5595_435, &_preg_268435641);
        Init_ciph_down_scale(&_pgen_tmp_449_915, &_ckks_gen_tmp_5595_435);
        Rescale(&_pgen_tmp_449_915._c0_poly, &_ckks_gen_tmp_5595_435._c0_poly);
        Rescale(&_pgen_tmp_449_915._c1_poly, &_ckks_gen_tmp_5595_435._c1_poly);
        Pt_from_msg_ofst(&_pgen_tmp_450_916, 104/* cst_387 */, (combine_rc_index_n45_329) * (25088), 25088, 1, 3);
        Init_ciph_up_scale_plain(&_pgen_tmp_451_917, &_pgen_tmp_449_915, &_pgen_tmp_450_916);
        Init_ciph_same_scale(&comb_rc_result45_328, &comb_rc_result45_328, &_pgen_tmp_451_917);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&comb_rc_result45_328._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modmul(Coeffs(&_pgen_tmp_451_917._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_449_915._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_450_916._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modmul(Coeffs(&_pgen_tmp_451_917._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_449_915._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_450_916._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&comb_rc_result45_328._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_rc_result45_328._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_451_917._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&comb_rc_result45_328._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&comb_rc_result45_328._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_451_917._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435641._c1_poly);
        Free_poly_data(&_preg_268435641._c0_poly);
      }
      Zero_ciph(&final_result45_330);
      for (combine_cc_index_n45_331 = 0; combine_cc_index_n45_331 < 128; combine_cc_index_n45_331 = combine_cc_index_n45_331 + 1) {
        _preg_268435642 = Rotate(comb_rc_result45_328, combine_cc_index_n45_331 * 147);
        Copy_ciph(&_ckks_gen_tmp_5628_436, &_preg_268435642);
        Init_ciph_down_scale(&_pgen_tmp_452_918, &_ckks_gen_tmp_5628_436);
        Rescale(&_pgen_tmp_452_918._c0_poly, &_ckks_gen_tmp_5628_436._c0_poly);
        Rescale(&_pgen_tmp_452_918._c1_poly, &_ckks_gen_tmp_5628_436._c1_poly);
        Pt_from_msg_ofst(&_pgen_tmp_453_919, 105/* cst_388 */, (combine_cc_index_n45_331) * (25088), 25088, 1, 2);
        Init_ciph_up_scale_plain(&_pgen_tmp_454_920, &_pgen_tmp_452_918, &_pgen_tmp_453_919);
        Init_ciph_same_scale(&final_result45_330, &final_result45_330, &_pgen_tmp_454_920);
        {
          _pgen_modulus_463 = Q_modulus();
          _pgen_num_q_464 = Poly_level(&final_result45_330._c0_poly);
          for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
            Hw_modmul(Coeffs(&_pgen_tmp_454_920._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_452_918._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_453_919._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modmul(Coeffs(&_pgen_tmp_454_920._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_452_918._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_453_919._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&final_result45_330._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&final_result45_330._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_454_920._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            Hw_modadd(Coeffs(&final_result45_330._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&final_result45_330._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_454_920._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
            _pgen_modulus_463 = _pgen_modulus_463 + 1;
          }
        }
        Free_poly_data(&_preg_268435642._c1_poly);
        Free_poly_data(&_preg_268435642._c0_poly);
      }
      _pgen_tmp_456_922 = VS_out_conv17_315[mesh_x_n17_316];
      Init_ciph_same_scale(&_pgen_tmp_455_921, &_pgen_tmp_456_922, &final_result45_330);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&_pgen_tmp_455_921._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modadd(Coeffs(&_pgen_tmp_455_921._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_456_922._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&final_result45_330._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&_pgen_tmp_455_921._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_456_922._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&final_result45_330._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
      Copy_ciph(&VS_out_conv17_315[mesh_x_n17_316], &_pgen_tmp_455_921);
      Tm_taken("Tensor::conv");
      Tm_taken("Tensor::add");
      Free_poly_data(&_preg_268435639._c1_poly);
      Free_poly_data(&_preg_268435639._c0_poly);
    }
  }
  Free_ciph_poly(VS_out_relu15_292, 2);
  Free_poly_data(&_preg_268435515._c1_poly);
  Free_poly_data(&_preg_268435515._c0_poly);
  Free_poly_data(&input_dup_n44_319._c1_poly);
  Free_poly_data(&input_dup_n44_319._c0_poly);
  Free_ciph_poly(input_grid_n44_321, 9);
  Free_poly_data(&_pgen_tmp_438_904._poly);
  Free_poly_data(&_pgen_tmp_439_905._c1_poly);
  Free_poly_data(&_pgen_tmp_439_905._c0_poly);
  Free_poly_data(&result_grid_n44_323._c1_poly);
  Free_poly_data(&result_grid_n44_323._c0_poly);
  Free_poly_data(&_pgen_tmp_441_907._poly);
  Free_poly_data(&_pgen_tmp_440_906._c1_poly);
  Free_poly_data(&_pgen_tmp_440_906._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_5520_433._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_5520_433._c0_poly);
  Free_poly_data(&_pgen_tmp_443_909._poly);
  Free_poly_data(&_pgen_tmp_442_908._c1_poly);
  Free_poly_data(&_pgen_tmp_442_908._c0_poly);
  Free_poly_data(&_pgen_tmp_444_910._c1_poly);
  Free_poly_data(&_pgen_tmp_444_910._c0_poly);
  Free_poly_data(&_pgen_tmp_445_911._poly);
  Free_poly_data(&_preg_268435516._c1_poly);
  Free_poly_data(&_preg_268435516._c0_poly);
  Free_poly_data(&result_n44_318._c1_poly);
  Free_poly_data(&result_n44_318._c0_poly);
  Free_poly_data(&orig_result44_325._c1_poly);
  Free_poly_data(&orig_result44_325._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_5562_434._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_5562_434._c0_poly);
  Free_poly_data(&_pgen_tmp_447_913._poly);
  Free_poly_data(&_pgen_tmp_446_912._c1_poly);
  Free_poly_data(&_pgen_tmp_446_912._c0_poly);
  Free_poly_data(&_pgen_tmp_448_914._c1_poly);
  Free_poly_data(&_pgen_tmp_448_914._c0_poly);
  Free_poly_data(&comb_row_result45_326._c1_poly);
  Free_poly_data(&comb_row_result45_326._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_5595_435._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_5595_435._c0_poly);
  Free_poly_data(&_pgen_tmp_450_916._poly);
  Free_poly_data(&_pgen_tmp_449_915._c1_poly);
  Free_poly_data(&_pgen_tmp_449_915._c0_poly);
  Free_poly_data(&_pgen_tmp_451_917._c1_poly);
  Free_poly_data(&_pgen_tmp_451_917._c0_poly);
  Free_poly_data(&comb_rc_result45_328._c1_poly);
  Free_poly_data(&comb_rc_result45_328._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_5628_436._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_5628_436._c0_poly);
  Free_poly_data(&_pgen_tmp_453_919._poly);
  Free_poly_data(&_pgen_tmp_452_918._c1_poly);
  Free_poly_data(&_pgen_tmp_452_918._c0_poly);
  Free_poly_data(&_pgen_tmp_454_920._c1_poly);
  Free_poly_data(&_pgen_tmp_454_920._c0_poly);
  Free_poly_data(&final_result45_330._c1_poly);
  Free_poly_data(&final_result45_330._c0_poly);
  Free_poly_data(&_pgen_tmp_455_921._c1_poly);
  Free_poly_data(&_pgen_tmp_455_921._c0_poly);
  for (rescale_iv_462 = 0; rescale_iv_462 < 4; rescale_iv_462 = rescale_iv_462 + 1) {
    _pgen_tmp_457_923 = VS_out_conv17_315[rescale_iv_462];
    Init_ciph_down_scale(&_pgen_tmp_458_924, &_pgen_tmp_457_923);
    Rescale(&_pgen_tmp_458_924._c0_poly, &_pgen_tmp_457_923._c0_poly);
    Rescale(&_pgen_tmp_458_924._c1_poly, &_pgen_tmp_457_923._c1_poly);
    Copy_ciph(&VS_out_conv17_315[rescale_iv_462], &_pgen_tmp_458_924);
  }
  Free_poly_data(&_pgen_tmp_458_924._c1_poly);
  Free_poly_data(&_pgen_tmp_458_924._c0_poly);
  for (reshard1_n17_332 = 0; reshard1_n17_332 < 1; reshard1_n17_332 = reshard1_n17_332 + 1) {
    Tm_start("Tensor::concat");
    Tm_start("Tensor::concat");
    Tm_start("Tensor::concat");
    _pgen_tmp_459_925 = VS_out_conv17_315[reshard1_n17_332];
    _pgen_tmp_460_926 = VS_out_conv17_315[(reshard1_n17_332 + 1)];
    _preg_268435643 = Rotate(_pgen_tmp_460_926, -6272);
    Init_ciph_same_scale(&_pgen_tmp_461_927, &_pgen_tmp_459_925, &_preg_268435643);
    _pgen_tmp_463_929 = VS_out_conv17_315[(reshard1_n17_332 + 2)];
    _pgen_tmp_464_930 = VS_out_conv17_315[(reshard1_n17_332 + 3)];
    _preg_268435644 = Rotate(_pgen_tmp_464_930, -6272);
    Init_ciph_same_scale(&_pgen_tmp_462_928, &_pgen_tmp_463_929, &_preg_268435644);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&_pgen_tmp_462_928._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_462_928._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_463_929._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435644._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_462_928._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_463_929._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435644._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    _preg_268435645 = Rotate(_pgen_tmp_462_928, -12544);
    Init_ciph_same_scale(&output_reshard17_333, &_pgen_tmp_461_927, &_preg_268435645);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&output_reshard17_333._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_461_927._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_459_925._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435643._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_461_927._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_459_925._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435643._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&output_reshard17_333._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_461_927._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435645._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&output_reshard17_333._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_461_927._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435645._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    Tm_taken("Tensor::concat");
    Tm_taken("Tensor::concat");
    Tm_taken("Tensor::concat");
    Free_poly_data(&_preg_268435643._c1_poly);
    Free_poly_data(&_preg_268435643._c0_poly);
    Free_poly_data(&_preg_268435644._c1_poly);
    Free_poly_data(&_preg_268435644._c0_poly);
    Free_poly_data(&_preg_268435645._c1_poly);
    Free_poly_data(&_preg_268435645._c0_poly);
  }
  Free_ciph_poly(VS_out_conv17_315, 4);
  Free_poly_data(&_pgen_tmp_462_928._c1_poly);
  Free_poly_data(&_pgen_tmp_462_928._c0_poly);
  Free_poly_data(&_pgen_tmp_461_927._c1_poly);
  Free_poly_data(&_pgen_tmp_461_927._c0_poly);
  Tm_start("Tensor::add");
  Pt_from_msg(&_pgen_tmp_465_931, 106 /* cst_124 */, 25088, 1, 1);
  Init_ciph_same_scale_plain(&addshard17_334, &output_reshard17_333, &_pgen_tmp_465_931);
  {
    _pgen_modulus_463 = Q_modulus();
    _pgen_num_q_464 = Poly_level(&addshard17_334._c0_poly);
    for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
      Hw_modadd(Coeffs(&addshard17_334._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&output_reshard17_333._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_465_931._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      Set_coeffs(&addshard17_334._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&output_reshard17_333._c1_poly, _pgen_rns_idx_465, degree));
      _pgen_modulus_463 = _pgen_modulus_463 + 1;
    }
    Free_poly_data(&_pgen_tmp_465_931._poly);
    Free_poly_data(&output_reshard17_333._c1_poly);
    Free_poly_data(&output_reshard17_333._c0_poly);
  }
  Tm_taken("Tensor::add");
  Bootstrap(&_preg_268435517, &addshard17_334, 15);
  Free_poly_data(&addshard17_334._c1_poly);
  Free_poly_data(&addshard17_334._c0_poly);
  Encode_float(&_pgen_tmp_467_933, &_cst_475, 1, 1, Level(&_preg_268435517));
  Init_ciph_up_scale_plain(&_pgen_tmp_466_932, &_preg_268435517, &_pgen_tmp_467_933);
  {
    _pgen_modulus_463 = Q_modulus();
    _pgen_num_q_464 = Poly_level(&_pgen_tmp_466_932._c0_poly);
    for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_466_932._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435517._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_467_933._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_466_932._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435517._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_467_933._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      _pgen_modulus_463 = _pgen_modulus_463 + 1;
    }
    Free_poly_data(&_pgen_tmp_467_933._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_468_934, &_pgen_tmp_466_932);
  Rescale(&_pgen_tmp_468_934._c0_poly, &_pgen_tmp_466_932._c0_poly);
  Rescale(&_pgen_tmp_468_934._c1_poly, &_pgen_tmp_466_932._c1_poly);
  Free_poly_data(&_pgen_tmp_466_932._c1_poly);
  Free_poly_data(&_pgen_tmp_466_932._c0_poly);
  _preg_268435518 = App_relu(_preg_268435517, _pgen_tmp_468_934, 25088);
  Free_poly_data(&_pgen_tmp_468_934._c1_poly);
  Free_poly_data(&_pgen_tmp_468_934._c0_poly);
  Free_poly_data(&_preg_268435517._c1_poly);
  Free_poly_data(&_preg_268435517._c0_poly);
  Copy_ciph(&_preg_268435519, &_preg_268435518);
  Free_poly_data(&_preg_268435518._c1_poly);
  Free_poly_data(&_preg_268435518._c0_poly);
  Tm_start("Tensor::conv");
  Copy_ciph(&_preg_268435520, &_preg_268435519);
  Free_poly_data(&_preg_268435519._c1_poly);
  Free_poly_data(&_preg_268435519._c0_poly);
  Zero_ciph(&result_n46_335);
  Copy_ciph(&input_dup_n46_336, &_preg_268435520);
  Free_poly_data(&_preg_268435520._c1_poly);
  Free_poly_data(&_preg_268435520._c0_poly);
  for (cap_block_input_grid_n46_337 = 0; cap_block_input_grid_n46_337 < 9; cap_block_input_grid_n46_337 = cap_block_input_grid_n46_337 + 1) {
    _preg_268435646 = Rotate(input_dup_n46_336, _cst_397[cap_block_input_grid_n46_337]);
    Copy_ciph(&input_grid_n46_338[cap_block_input_grid_n46_337], &_preg_268435646);
    Free_poly_data(&_preg_268435646._c1_poly);
    Free_poly_data(&_preg_268435646._c0_poly);
  }
  Free_poly_data(&input_dup_n46_336._c1_poly);
  Free_poly_data(&input_dup_n46_336._c0_poly);
  for (num_grid_n46_339 = 0; num_grid_n46_339 < 512; num_grid_n46_339 = num_grid_n46_339 + 1) {
    Zero_ciph(&result_grid_n46_340);
    for (cap_block_n46_341 = 0; cap_block_n46_341 < 9; cap_block_n46_341 = cap_block_n46_341 + 1) {
      _ckks_gen_tmp_5777_437 = input_grid_n46_338[cap_block_n46_341];
      Pt_from_msg_ofst(&_pgen_tmp_469_935, 107/* cst_395 */, (cap_block_n46_341 + (num_grid_n46_339 * 9)) * (25088), 25088, 1, 3);
      Init_ciph_up_scale_plain(&_pgen_tmp_470_936, &_ckks_gen_tmp_5777_437, &_pgen_tmp_469_935);
      Init_ciph_same_scale(&result_grid_n46_340, &result_grid_n46_340, &_pgen_tmp_470_936);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&result_grid_n46_340._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_470_936._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_5777_437._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_469_935._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_470_936._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_5777_437._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_469_935._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&result_grid_n46_340._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n46_340._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_470_936._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&result_grid_n46_340._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n46_340._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_470_936._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
    }
    _preg_268435647 = Rotate(result_grid_n46_340, num_grid_n46_339 * 49);
    Init_ciph_same_scale(&result_n46_335, &result_n46_335, &_preg_268435647);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&result_n46_335._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&result_n46_335._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n46_335._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435647._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&result_n46_335._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n46_335._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435647._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    Free_poly_data(&_preg_268435647._c1_poly);
    Free_poly_data(&_preg_268435647._c0_poly);
  }
  Free_ciph_poly(input_grid_n46_338, 9);
  Free_poly_data(&_pgen_tmp_469_935._poly);
  Free_poly_data(&_pgen_tmp_470_936._c1_poly);
  Free_poly_data(&_pgen_tmp_470_936._c0_poly);
  Free_poly_data(&result_grid_n46_340._c1_poly);
  Free_poly_data(&result_grid_n46_340._c0_poly);
  Init_ciph_down_scale(&result_n46_335, &result_n46_335);
  Rescale(&result_n46_335._c0_poly, &result_n46_335._c0_poly);
  Rescale(&result_n46_335._c1_poly, &result_n46_335._c1_poly);
  Pt_from_msg(&_pgen_tmp_472_938, 108 /* cst_398 */, 25088, 1, 2);
  Init_ciph_up_scale_plain(&_pgen_tmp_471_937, &result_n46_335, &_pgen_tmp_472_938);
  {
    _pgen_modulus_463 = Q_modulus();
    _pgen_num_q_464 = Poly_level(&_pgen_tmp_471_937._c0_poly);
    for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_471_937._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n46_335._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_472_938._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_471_937._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n46_335._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_472_938._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      _pgen_modulus_463 = _pgen_modulus_463 + 1;
    }
    Free_poly_data(&_pgen_tmp_472_938._poly);
  }
  Init_ciph_down_scale(&_preg_268435521, &_pgen_tmp_471_937);
  Rescale(&_preg_268435521._c0_poly, &_pgen_tmp_471_937._c0_poly);
  Rescale(&_preg_268435521._c1_poly, &_pgen_tmp_471_937._c1_poly);
  Free_poly_data(&_pgen_tmp_471_937._c1_poly);
  Free_poly_data(&_pgen_tmp_471_937._c0_poly);
  _preg_268435648 = Rotate(result_n46_335, -25088);
  Copy_ciph(&_ckks_gen_tmp_5821_438, &_preg_268435648);
  Free_poly_data(&_preg_268435648._c1_poly);
  Free_poly_data(&_preg_268435648._c0_poly);
  Pt_from_msg(&_pgen_tmp_474_940, 109 /* cst_399 */, 25088, 1, 2);
  Init_ciph_up_scale_plain(&_pgen_tmp_473_939, &_ckks_gen_tmp_5821_438, &_pgen_tmp_474_940);
  {
    _pgen_modulus_463 = Q_modulus();
    _pgen_num_q_464 = Poly_level(&_pgen_tmp_473_939._c0_poly);
    for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_473_939._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_5821_438._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_474_940._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_473_939._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_5821_438._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_474_940._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      _pgen_modulus_463 = _pgen_modulus_463 + 1;
    }
    Free_poly_data(&_pgen_tmp_474_940._poly);
    Free_poly_data(&_ckks_gen_tmp_5821_438._c1_poly);
    Free_poly_data(&_ckks_gen_tmp_5821_438._c0_poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_475_941, &_pgen_tmp_473_939);
  Rescale(&_pgen_tmp_475_941._c0_poly, &_pgen_tmp_473_939._c0_poly);
  Rescale(&_pgen_tmp_475_941._c1_poly, &_pgen_tmp_473_939._c1_poly);
  Free_poly_data(&_pgen_tmp_473_939._c1_poly);
  Free_poly_data(&_pgen_tmp_473_939._c0_poly);
  Init_ciph_same_scale(&_preg_268435521, &_preg_268435521, &_pgen_tmp_475_941);
  {
    _pgen_modulus_463 = Q_modulus();
    _pgen_num_q_464 = Poly_level(&_preg_268435521._c0_poly);
    for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
      Hw_modadd(Coeffs(&_preg_268435521._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435521._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_475_941._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      Hw_modadd(Coeffs(&_preg_268435521._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435521._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_475_941._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      _pgen_modulus_463 = _pgen_modulus_463 + 1;
    }
    Free_poly_data(&_pgen_tmp_475_941._c1_poly);
    Free_poly_data(&_pgen_tmp_475_941._c0_poly);
  }
  Pt_from_msg(&_pgen_tmp_476_942, 110 /* cst_396 */, 25088, 1, 1);
  Init_ciph_same_scale_plain(&result_n46_335, &_preg_268435521, &_pgen_tmp_476_942);
  {
    _pgen_modulus_463 = Q_modulus();
    _pgen_num_q_464 = Poly_level(&result_n46_335._c0_poly);
    for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
      Hw_modadd(Coeffs(&result_n46_335._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435521._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_476_942._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      Set_coeffs(&result_n46_335._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_preg_268435521._c1_poly, _pgen_rns_idx_465, degree));
      _pgen_modulus_463 = _pgen_modulus_463 + 1;
    }
    Free_poly_data(&_pgen_tmp_476_942._poly);
    Free_poly_data(&_preg_268435521._c1_poly);
    Free_poly_data(&_preg_268435521._c0_poly);
  }
  Copy_ciph(&_preg_268435522, &result_n46_335);
  Free_poly_data(&result_n46_335._c1_poly);
  Free_poly_data(&result_n46_335._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::add");
  Init_ciph_same_scale(&_preg_268435523, &_preg_268435522, &addshard16_314);
  {
    _pgen_modulus_463 = Q_modulus();
    _pgen_num_q_464 = Poly_level(&_preg_268435523._c0_poly);
    for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
      Hw_modadd(Coeffs(&_preg_268435523._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435522._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&addshard16_314._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      Hw_modadd(Coeffs(&_preg_268435523._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435522._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&addshard16_314._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      _pgen_modulus_463 = _pgen_modulus_463 + 1;
    }
    Free_poly_data(&addshard16_314._c1_poly);
    Free_poly_data(&addshard16_314._c0_poly);
    Free_poly_data(&_preg_268435522._c1_poly);
    Free_poly_data(&_preg_268435522._c0_poly);
  }
  Tm_taken("Tensor::add");
  Bootstrap(&_preg_268435524, &_preg_268435523, 15);
  Free_poly_data(&_preg_268435523._c1_poly);
  Free_poly_data(&_preg_268435523._c0_poly);
  Encode_float(&_pgen_tmp_478_944, &_cst_476, 1, 1, Level(&_preg_268435524));
  Init_ciph_up_scale_plain(&_pgen_tmp_477_943, &_preg_268435524, &_pgen_tmp_478_944);
  {
    _pgen_modulus_463 = Q_modulus();
    _pgen_num_q_464 = Poly_level(&_pgen_tmp_477_943._c0_poly);
    for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_477_943._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435524._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_478_944._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_477_943._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435524._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_478_944._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      _pgen_modulus_463 = _pgen_modulus_463 + 1;
    }
    Free_poly_data(&_pgen_tmp_478_944._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_479_945, &_pgen_tmp_477_943);
  Rescale(&_pgen_tmp_479_945._c0_poly, &_pgen_tmp_477_943._c0_poly);
  Rescale(&_pgen_tmp_479_945._c1_poly, &_pgen_tmp_477_943._c1_poly);
  Free_poly_data(&_pgen_tmp_477_943._c1_poly);
  Free_poly_data(&_pgen_tmp_477_943._c0_poly);
  _preg_268435525 = App_relu(_preg_268435524, _pgen_tmp_479_945, 25088);
  Free_poly_data(&_pgen_tmp_479_945._c1_poly);
  Free_poly_data(&_pgen_tmp_479_945._c0_poly);
  Free_poly_data(&_preg_268435524._c1_poly);
  Free_poly_data(&_preg_268435524._c0_poly);
  Copy_ciph(&_preg_268435526, &_preg_268435525);
  Free_poly_data(&_preg_268435525._c1_poly);
  Free_poly_data(&_preg_268435525._c0_poly);
  Tm_start("Tensor::conv");
  Copy_ciph(&_preg_268435527, &_preg_268435526);
  Zero_ciph(&result_n47_342);
  Copy_ciph(&input_dup_n47_343, &_preg_268435527);
  Free_poly_data(&_preg_268435527._c1_poly);
  Free_poly_data(&_preg_268435527._c0_poly);
  for (cap_block_input_grid_n47_344 = 0; cap_block_input_grid_n47_344 < 9; cap_block_input_grid_n47_344 = cap_block_input_grid_n47_344 + 1) {
    _preg_268435649 = Rotate(input_dup_n47_343, _cst_405[cap_block_input_grid_n47_344]);
    Copy_ciph(&input_grid_n47_345[cap_block_input_grid_n47_344], &_preg_268435649);
    Free_poly_data(&_preg_268435649._c1_poly);
    Free_poly_data(&_preg_268435649._c0_poly);
  }
  Free_poly_data(&input_dup_n47_343._c1_poly);
  Free_poly_data(&input_dup_n47_343._c0_poly);
  for (num_grid_n47_346 = 0; num_grid_n47_346 < 512; num_grid_n47_346 = num_grid_n47_346 + 1) {
    Zero_ciph(&result_grid_n47_347);
    for (cap_block_n47_348 = 0; cap_block_n47_348 < 9; cap_block_n47_348 = cap_block_n47_348 + 1) {
      _ckks_gen_tmp_5914_439 = input_grid_n47_345[cap_block_n47_348];
      Pt_from_msg_ofst(&_pgen_tmp_480_946, 111/* cst_403 */, (cap_block_n47_348 + (num_grid_n47_346 * 9)) * (25088), 25088, 1, 3);
      Init_ciph_up_scale_plain(&_pgen_tmp_481_947, &_ckks_gen_tmp_5914_439, &_pgen_tmp_480_946);
      Init_ciph_same_scale(&result_grid_n47_347, &result_grid_n47_347, &_pgen_tmp_481_947);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&result_grid_n47_347._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_481_947._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_5914_439._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_480_946._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_481_947._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_5914_439._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_480_946._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&result_grid_n47_347._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n47_347._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_481_947._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&result_grid_n47_347._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n47_347._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_481_947._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
    }
    _preg_268435650 = Rotate(result_grid_n47_347, num_grid_n47_346 * 49);
    Init_ciph_same_scale(&result_n47_342, &result_n47_342, &_preg_268435650);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&result_n47_342._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&result_n47_342._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n47_342._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435650._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&result_n47_342._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n47_342._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435650._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    Free_poly_data(&_preg_268435650._c1_poly);
    Free_poly_data(&_preg_268435650._c0_poly);
  }
  Free_ciph_poly(input_grid_n47_345, 9);
  Free_poly_data(&_pgen_tmp_480_946._poly);
  Free_poly_data(&_pgen_tmp_481_947._c1_poly);
  Free_poly_data(&_pgen_tmp_481_947._c0_poly);
  Free_poly_data(&result_grid_n47_347._c1_poly);
  Free_poly_data(&result_grid_n47_347._c0_poly);
  Init_ciph_down_scale(&result_n47_342, &result_n47_342);
  Rescale(&result_n47_342._c0_poly, &result_n47_342._c0_poly);
  Rescale(&result_n47_342._c1_poly, &result_n47_342._c1_poly);
  Pt_from_msg(&_pgen_tmp_483_949, 112 /* cst_406 */, 25088, 1, 2);
  Init_ciph_up_scale_plain(&_pgen_tmp_482_948, &result_n47_342, &_pgen_tmp_483_949);
  {
    _pgen_modulus_463 = Q_modulus();
    _pgen_num_q_464 = Poly_level(&_pgen_tmp_482_948._c0_poly);
    for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_482_948._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n47_342._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_483_949._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_482_948._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n47_342._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_483_949._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      _pgen_modulus_463 = _pgen_modulus_463 + 1;
    }
    Free_poly_data(&_pgen_tmp_483_949._poly);
  }
  Init_ciph_down_scale(&_preg_268435528, &_pgen_tmp_482_948);
  Rescale(&_preg_268435528._c0_poly, &_pgen_tmp_482_948._c0_poly);
  Rescale(&_preg_268435528._c1_poly, &_pgen_tmp_482_948._c1_poly);
  Free_poly_data(&_pgen_tmp_482_948._c1_poly);
  Free_poly_data(&_pgen_tmp_482_948._c0_poly);
  _preg_268435651 = Rotate(result_n47_342, -25088);
  Copy_ciph(&_ckks_gen_tmp_5958_440, &_preg_268435651);
  Free_poly_data(&_preg_268435651._c1_poly);
  Free_poly_data(&_preg_268435651._c0_poly);
  Pt_from_msg(&_pgen_tmp_485_951, 113 /* cst_407 */, 25088, 1, 2);
  Init_ciph_up_scale_plain(&_pgen_tmp_484_950, &_ckks_gen_tmp_5958_440, &_pgen_tmp_485_951);
  {
    _pgen_modulus_463 = Q_modulus();
    _pgen_num_q_464 = Poly_level(&_pgen_tmp_484_950._c0_poly);
    for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_484_950._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_5958_440._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_485_951._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_484_950._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_5958_440._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_485_951._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      _pgen_modulus_463 = _pgen_modulus_463 + 1;
    }
    Free_poly_data(&_pgen_tmp_485_951._poly);
    Free_poly_data(&_ckks_gen_tmp_5958_440._c1_poly);
    Free_poly_data(&_ckks_gen_tmp_5958_440._c0_poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_486_952, &_pgen_tmp_484_950);
  Rescale(&_pgen_tmp_486_952._c0_poly, &_pgen_tmp_484_950._c0_poly);
  Rescale(&_pgen_tmp_486_952._c1_poly, &_pgen_tmp_484_950._c1_poly);
  Free_poly_data(&_pgen_tmp_484_950._c1_poly);
  Free_poly_data(&_pgen_tmp_484_950._c0_poly);
  Init_ciph_same_scale(&_preg_268435528, &_preg_268435528, &_pgen_tmp_486_952);
  {
    _pgen_modulus_463 = Q_modulus();
    _pgen_num_q_464 = Poly_level(&_preg_268435528._c0_poly);
    for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
      Hw_modadd(Coeffs(&_preg_268435528._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435528._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_486_952._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      Hw_modadd(Coeffs(&_preg_268435528._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435528._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_486_952._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      _pgen_modulus_463 = _pgen_modulus_463 + 1;
    }
    Free_poly_data(&_pgen_tmp_486_952._c1_poly);
    Free_poly_data(&_pgen_tmp_486_952._c0_poly);
  }
  Pt_from_msg(&_pgen_tmp_487_953, 114 /* cst_404 */, 25088, 1, 1);
  Init_ciph_same_scale_plain(&result_n47_342, &_preg_268435528, &_pgen_tmp_487_953);
  {
    _pgen_modulus_463 = Q_modulus();
    _pgen_num_q_464 = Poly_level(&result_n47_342._c0_poly);
    for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
      Hw_modadd(Coeffs(&result_n47_342._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435528._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_487_953._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      Set_coeffs(&result_n47_342._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_preg_268435528._c1_poly, _pgen_rns_idx_465, degree));
      _pgen_modulus_463 = _pgen_modulus_463 + 1;
    }
    Free_poly_data(&_pgen_tmp_487_953._poly);
    Free_poly_data(&_preg_268435528._c1_poly);
    Free_poly_data(&_preg_268435528._c0_poly);
  }
  Copy_ciph(&_preg_268435529, &result_n47_342);
  Free_poly_data(&result_n47_342._c1_poly);
  Free_poly_data(&result_n47_342._c0_poly);
  Tm_taken("Tensor::conv");
  Bootstrap(&_preg_268435530, &_preg_268435529, 15);
  Free_poly_data(&_preg_268435529._c1_poly);
  Free_poly_data(&_preg_268435529._c0_poly);
  Encode_float(&_pgen_tmp_489_955, &_cst_477, 1, 1, Level(&_preg_268435530));
  Init_ciph_up_scale_plain(&_pgen_tmp_488_954, &_preg_268435530, &_pgen_tmp_489_955);
  {
    _pgen_modulus_463 = Q_modulus();
    _pgen_num_q_464 = Poly_level(&_pgen_tmp_488_954._c0_poly);
    for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_488_954._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435530._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_489_955._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_488_954._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435530._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_489_955._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      _pgen_modulus_463 = _pgen_modulus_463 + 1;
    }
    Free_poly_data(&_pgen_tmp_489_955._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_490_956, &_pgen_tmp_488_954);
  Rescale(&_pgen_tmp_490_956._c0_poly, &_pgen_tmp_488_954._c0_poly);
  Rescale(&_pgen_tmp_490_956._c1_poly, &_pgen_tmp_488_954._c1_poly);
  Free_poly_data(&_pgen_tmp_488_954._c1_poly);
  Free_poly_data(&_pgen_tmp_488_954._c0_poly);
  _preg_268435531 = App_relu(_preg_268435530, _pgen_tmp_490_956, 25088);
  Free_poly_data(&_pgen_tmp_490_956._c1_poly);
  Free_poly_data(&_pgen_tmp_490_956._c0_poly);
  Free_poly_data(&_preg_268435530._c1_poly);
  Free_poly_data(&_preg_268435530._c0_poly);
  Copy_ciph(&_preg_268435532, &_preg_268435531);
  Free_poly_data(&_preg_268435531._c1_poly);
  Free_poly_data(&_preg_268435531._c0_poly);
  Tm_start("Tensor::conv");
  Copy_ciph(&_preg_268435533, &_preg_268435532);
  Free_poly_data(&_preg_268435532._c1_poly);
  Free_poly_data(&_preg_268435532._c0_poly);
  Zero_ciph(&result_n48_349);
  Copy_ciph(&input_dup_n48_350, &_preg_268435533);
  Free_poly_data(&_preg_268435533._c1_poly);
  Free_poly_data(&_preg_268435533._c0_poly);
  for (cap_block_input_grid_n48_351 = 0; cap_block_input_grid_n48_351 < 9; cap_block_input_grid_n48_351 = cap_block_input_grid_n48_351 + 1) {
    _preg_268435652 = Rotate(input_dup_n48_350, _cst_412[cap_block_input_grid_n48_351]);
    Copy_ciph(&input_grid_n48_352[cap_block_input_grid_n48_351], &_preg_268435652);
    Free_poly_data(&_preg_268435652._c1_poly);
    Free_poly_data(&_preg_268435652._c0_poly);
  }
  Free_poly_data(&input_dup_n48_350._c1_poly);
  Free_poly_data(&input_dup_n48_350._c0_poly);
  for (num_grid_n48_353 = 0; num_grid_n48_353 < 512; num_grid_n48_353 = num_grid_n48_353 + 1) {
    Zero_ciph(&result_grid_n48_354);
    for (cap_block_n48_355 = 0; cap_block_n48_355 < 9; cap_block_n48_355 = cap_block_n48_355 + 1) {
      _ckks_gen_tmp_6045_441 = input_grid_n48_352[cap_block_n48_355];
      Pt_from_msg_ofst(&_pgen_tmp_491_957, 115/* cst_410 */, (cap_block_n48_355 + (num_grid_n48_353 * 9)) * (25088), 25088, 1, 3);
      Init_ciph_up_scale_plain(&_pgen_tmp_492_958, &_ckks_gen_tmp_6045_441, &_pgen_tmp_491_957);
      Init_ciph_same_scale(&result_grid_n48_354, &result_grid_n48_354, &_pgen_tmp_492_958);
      {
        _pgen_modulus_463 = Q_modulus();
        _pgen_num_q_464 = Poly_level(&result_grid_n48_354._c0_poly);
        for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_492_958._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_6045_441._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_491_957._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_492_958._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_6045_441._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_491_957._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&result_grid_n48_354._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n48_354._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_492_958._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          Hw_modadd(Coeffs(&result_grid_n48_354._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_grid_n48_354._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_492_958._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
          _pgen_modulus_463 = _pgen_modulus_463 + 1;
        }
      }
    }
    _preg_268435653 = Rotate(result_grid_n48_354, num_grid_n48_353 * 49);
    Init_ciph_same_scale(&result_n48_349, &result_n48_349, &_preg_268435653);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&result_n48_349._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&result_n48_349._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n48_349._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435653._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&result_n48_349._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n48_349._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435653._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    Free_poly_data(&_preg_268435653._c1_poly);
    Free_poly_data(&_preg_268435653._c0_poly);
  }
  Free_ciph_poly(input_grid_n48_352, 9);
  Free_poly_data(&_pgen_tmp_491_957._poly);
  Free_poly_data(&_pgen_tmp_492_958._c1_poly);
  Free_poly_data(&_pgen_tmp_492_958._c0_poly);
  Free_poly_data(&result_grid_n48_354._c1_poly);
  Free_poly_data(&result_grid_n48_354._c0_poly);
  Init_ciph_down_scale(&result_n48_349, &result_n48_349);
  Rescale(&result_n48_349._c0_poly, &result_n48_349._c0_poly);
  Rescale(&result_n48_349._c1_poly, &result_n48_349._c1_poly);
  Pt_from_msg(&_pgen_tmp_494_960, 116 /* cst_413 */, 25088, 1, 2);
  Init_ciph_up_scale_plain(&_pgen_tmp_493_959, &result_n48_349, &_pgen_tmp_494_960);
  {
    _pgen_modulus_463 = Q_modulus();
    _pgen_num_q_464 = Poly_level(&_pgen_tmp_493_959._c0_poly);
    for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_493_959._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n48_349._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_494_960._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_493_959._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&result_n48_349._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_494_960._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      _pgen_modulus_463 = _pgen_modulus_463 + 1;
    }
    Free_poly_data(&_pgen_tmp_494_960._poly);
  }
  Init_ciph_down_scale(&_preg_268435534, &_pgen_tmp_493_959);
  Rescale(&_preg_268435534._c0_poly, &_pgen_tmp_493_959._c0_poly);
  Rescale(&_preg_268435534._c1_poly, &_pgen_tmp_493_959._c1_poly);
  Free_poly_data(&_pgen_tmp_493_959._c1_poly);
  Free_poly_data(&_pgen_tmp_493_959._c0_poly);
  _preg_268435654 = Rotate(result_n48_349, -25088);
  Copy_ciph(&_ckks_gen_tmp_6089_442, &_preg_268435654);
  Free_poly_data(&_preg_268435654._c1_poly);
  Free_poly_data(&_preg_268435654._c0_poly);
  Pt_from_msg(&_pgen_tmp_496_962, 117 /* cst_414 */, 25088, 1, 2);
  Init_ciph_up_scale_plain(&_pgen_tmp_495_961, &_ckks_gen_tmp_6089_442, &_pgen_tmp_496_962);
  {
    _pgen_modulus_463 = Q_modulus();
    _pgen_num_q_464 = Poly_level(&_pgen_tmp_495_961._c0_poly);
    for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_495_961._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_6089_442._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_496_962._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_495_961._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_6089_442._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_496_962._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      _pgen_modulus_463 = _pgen_modulus_463 + 1;
    }
    Free_poly_data(&_pgen_tmp_496_962._poly);
    Free_poly_data(&_ckks_gen_tmp_6089_442._c1_poly);
    Free_poly_data(&_ckks_gen_tmp_6089_442._c0_poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_497_963, &_pgen_tmp_495_961);
  Rescale(&_pgen_tmp_497_963._c0_poly, &_pgen_tmp_495_961._c0_poly);
  Rescale(&_pgen_tmp_497_963._c1_poly, &_pgen_tmp_495_961._c1_poly);
  Free_poly_data(&_pgen_tmp_495_961._c1_poly);
  Free_poly_data(&_pgen_tmp_495_961._c0_poly);
  Init_ciph_same_scale(&_preg_268435534, &_preg_268435534, &_pgen_tmp_497_963);
  {
    _pgen_modulus_463 = Q_modulus();
    _pgen_num_q_464 = Poly_level(&_preg_268435534._c0_poly);
    for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
      Hw_modadd(Coeffs(&_preg_268435534._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435534._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_497_963._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      Hw_modadd(Coeffs(&_preg_268435534._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435534._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_497_963._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      _pgen_modulus_463 = _pgen_modulus_463 + 1;
    }
    Free_poly_data(&_pgen_tmp_497_963._c1_poly);
    Free_poly_data(&_pgen_tmp_497_963._c0_poly);
  }
  Pt_from_msg(&_pgen_tmp_498_964, 118 /* cst_411 */, 25088, 1, 1);
  Init_ciph_same_scale_plain(&result_n48_349, &_preg_268435534, &_pgen_tmp_498_964);
  {
    _pgen_modulus_463 = Q_modulus();
    _pgen_num_q_464 = Poly_level(&result_n48_349._c0_poly);
    for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
      Hw_modadd(Coeffs(&result_n48_349._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435534._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_498_964._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      Set_coeffs(&result_n48_349._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&_preg_268435534._c1_poly, _pgen_rns_idx_465, degree));
      _pgen_modulus_463 = _pgen_modulus_463 + 1;
    }
    Free_poly_data(&_pgen_tmp_498_964._poly);
    Free_poly_data(&_preg_268435534._c1_poly);
    Free_poly_data(&_preg_268435534._c0_poly);
  }
  Copy_ciph(&_preg_268435535, &result_n48_349);
  Free_poly_data(&result_n48_349._c1_poly);
  Free_poly_data(&result_n48_349._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::add");
  Init_ciph_same_scale(&_preg_268435536, &_preg_268435535, &_preg_268435526);
  {
    _pgen_modulus_463 = Q_modulus();
    _pgen_num_q_464 = Poly_level(&_preg_268435536._c0_poly);
    for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
      Hw_modadd(Coeffs(&_preg_268435536._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435535._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435526._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      Hw_modadd(Coeffs(&_preg_268435536._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435535._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435526._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      _pgen_modulus_463 = _pgen_modulus_463 + 1;
    }
    Free_poly_data(&_preg_268435526._c1_poly);
    Free_poly_data(&_preg_268435526._c0_poly);
    Free_poly_data(&_preg_268435535._c1_poly);
    Free_poly_data(&_preg_268435535._c0_poly);
  }
  Tm_taken("Tensor::add");
  Bootstrap(&_preg_268435537, &_preg_268435536, 18);
  Free_poly_data(&_preg_268435536._c1_poly);
  Free_poly_data(&_preg_268435536._c0_poly);
  Encode_float(&_pgen_tmp_500_966, &_cst_478, 1, 1, Level(&_preg_268435537));
  Init_ciph_up_scale_plain(&_pgen_tmp_499_965, &_preg_268435537, &_pgen_tmp_500_966);
  {
    _pgen_modulus_463 = Q_modulus();
    _pgen_num_q_464 = Poly_level(&_pgen_tmp_499_965._c0_poly);
    for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_499_965._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435537._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_500_966._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_499_965._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435537._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_500_966._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      _pgen_modulus_463 = _pgen_modulus_463 + 1;
    }
    Free_poly_data(&_pgen_tmp_500_966._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_501_967, &_pgen_tmp_499_965);
  Rescale(&_pgen_tmp_501_967._c0_poly, &_pgen_tmp_499_965._c0_poly);
  Rescale(&_pgen_tmp_501_967._c1_poly, &_pgen_tmp_499_965._c1_poly);
  Free_poly_data(&_pgen_tmp_499_965._c1_poly);
  Free_poly_data(&_pgen_tmp_499_965._c0_poly);
  _preg_268435538 = App_relu(_preg_268435537, _pgen_tmp_501_967, 25088);
  Free_poly_data(&_pgen_tmp_501_967._c1_poly);
  Free_poly_data(&_pgen_tmp_501_967._c0_poly);
  Free_poly_data(&_preg_268435537._c1_poly);
  Free_poly_data(&_preg_268435537._c0_poly);
  Copy_ciph(&_preg_268435539, &_preg_268435538);
  Free_poly_data(&_preg_268435538._c1_poly);
  Free_poly_data(&_preg_268435538._c0_poly);
  Tm_start("Tensor::global_avg_pool");
  Copy_ciph(&_preg_268435540, &_preg_268435539);
  Free_poly_data(&_preg_268435539._c1_poly);
  Free_poly_data(&_preg_268435539._c0_poly);
  for (gap_sum_index_n48_356 = 0; gap_sum_index_n48_356 < 5; gap_sum_index_n48_356 = gap_sum_index_n48_356 + 1) {
    _preg_268435655 = Rotate(_preg_268435540, 1 << gap_sum_index_n48_356);
    Init_ciph_same_scale(&_preg_268435540, &_preg_268435540, &_preg_268435655);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&_preg_268435540._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&_preg_268435540._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435540._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435655._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&_preg_268435540._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435540._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435655._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    Free_poly_data(&_preg_268435655._c1_poly);
    Free_poly_data(&_preg_268435655._c0_poly);
  }
  Encode_float(&_pgen_tmp_503_969, &_cst_417, 1, 1, Level(&_preg_268435540));
  Init_ciph_up_scale_plain(&_pgen_tmp_502_968, &_preg_268435540, &_pgen_tmp_503_969);
  {
    _pgen_modulus_463 = Q_modulus();
    _pgen_num_q_464 = Poly_level(&_pgen_tmp_502_968._c0_poly);
    for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_502_968._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435540._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_503_969._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_502_968._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435540._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_503_969._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      _pgen_modulus_463 = _pgen_modulus_463 + 1;
    }
    Free_poly_data(&_pgen_tmp_503_969._poly);
    Free_poly_data(&_preg_268435540._c1_poly);
    Free_poly_data(&_preg_268435540._c0_poly);
  }
  Init_ciph_down_scale(&gap_result48_357, &_pgen_tmp_502_968);
  Rescale(&gap_result48_357._c0_poly, &_pgen_tmp_502_968._c0_poly);
  Rescale(&gap_result48_357._c1_poly, &_pgen_tmp_502_968._c1_poly);
  Free_poly_data(&_pgen_tmp_502_968._c1_poly);
  Free_poly_data(&_pgen_tmp_502_968._c0_poly);
  Copy_ciph(&_preg_268435541, &gap_result48_357);
  Free_poly_data(&gap_result48_357._c1_poly);
  Free_poly_data(&gap_result48_357._c0_poly);
  Tm_taken("Tensor::global_avg_pool");
  Copy_ciph(&orig_result48_358, &_preg_268435541);
  Free_poly_data(&_preg_268435541._c1_poly);
  Free_poly_data(&_preg_268435541._c0_poly);
  Pt_from_msg(&_pgen_tmp_505_971, 119 /* cst_418 */, 25088, 1, 5);
  Init_ciph_up_scale_plain(&_pgen_tmp_504_970, &orig_result48_358, &_pgen_tmp_505_971);
  {
    _pgen_modulus_463 = Q_modulus();
    _pgen_num_q_464 = Poly_level(&_pgen_tmp_504_970._c0_poly);
    for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_504_970._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&orig_result48_358._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_505_971._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_504_970._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&orig_result48_358._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_505_971._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      _pgen_modulus_463 = _pgen_modulus_463 + 1;
    }
    Free_poly_data(&_pgen_tmp_505_971._poly);
  }
  Init_ciph_down_scale(&orig_result48_358, &_pgen_tmp_504_970);
  Rescale(&orig_result48_358._c0_poly, &_pgen_tmp_504_970._c0_poly);
  Rescale(&orig_result48_358._c1_poly, &_pgen_tmp_504_970._c1_poly);
  Free_poly_data(&_pgen_tmp_504_970._c1_poly);
  Free_poly_data(&_pgen_tmp_504_970._c0_poly);
  Zero_ciph(&final_result49_359);
  for (combine_cc_lf_index_n49_360 = 0; combine_cc_lf_index_n49_360 < 7; combine_cc_lf_index_n49_360 = combine_cc_lf_index_n49_360 + 1) {
    _preg_268435656 = Rotate(orig_result48_358, (1 << combine_cc_lf_index_n49_360) * 48);
    Init_ciph_same_scale(&orig_result48_358, &orig_result48_358, &_preg_268435656);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&orig_result48_358._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modadd(Coeffs(&orig_result48_358._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&orig_result48_358._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435656._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&orig_result48_358._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&orig_result48_358._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435656._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    Free_poly_data(&_preg_268435656._c1_poly);
    Free_poly_data(&_preg_268435656._c0_poly);
  }
  for (combine_cc_index_n49_361 = 0; combine_cc_index_n49_361 < 10; combine_cc_index_n49_361 = combine_cc_index_n49_361 + 1) {
    _preg_268435657 = Rotate(orig_result48_358, combine_cc_index_n49_361 * 2450);
    Copy_ciph(&_ckks_gen_tmp_6209_443, &_preg_268435657);
    Pt_from_msg_ofst(&_pgen_tmp_506_972, 120/* cst_419 */, (combine_cc_index_n49_361) * (25088), 25088, 1, 4);
    Init_ciph_up_scale_plain(&_pgen_tmp_507_973, &_ckks_gen_tmp_6209_443, &_pgen_tmp_506_972);
    Init_ciph_same_scale(&final_result49_359, &final_result49_359, &_pgen_tmp_507_973);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&final_result49_359._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_507_973._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_6209_443._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_506_972._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_507_973._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_6209_443._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_506_972._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&final_result49_359._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&final_result49_359._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_507_973._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&final_result49_359._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&final_result49_359._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_507_973._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    Free_poly_data(&_preg_268435657._c1_poly);
    Free_poly_data(&_preg_268435657._c0_poly);
  }
  Free_poly_data(&orig_result48_358._c1_poly);
  Free_poly_data(&orig_result48_358._c0_poly);
  Free_poly_data(&_pgen_tmp_506_972._poly);
  Free_poly_data(&_ckks_gen_tmp_6209_443._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_6209_443._c0_poly);
  Free_poly_data(&_pgen_tmp_507_973._c1_poly);
  Free_poly_data(&_pgen_tmp_507_973._c0_poly);
  Init_ciph_down_scale(&final_result49_359, &final_result49_359);
  Rescale(&final_result49_359._c0_poly, &final_result49_359._c0_poly);
  Rescale(&final_result49_359._c1_poly, &final_result49_359._c1_poly);
  Copy_ciph(&_preg_268435542, &final_result49_359);
  Free_poly_data(&final_result49_359._c1_poly);
  Free_poly_data(&final_result49_359._c0_poly);
  Tm_start("Tensor::flatten");
  Copy_ciph(&_preg_268435543, &_preg_268435542);
  Free_poly_data(&_preg_268435542._c1_poly);
  Free_poly_data(&_preg_268435542._c0_poly);
  Tm_taken("Tensor::flatten");
  Tm_start("Tensor::gemm");
  Zero_ciph(&tmp_result_n50_362);
  _preg_268435658 = Rotate(_preg_268435543, -1000);
  Init_ciph_same_scale(&input_dup_n50_363, &_preg_268435543, &_preg_268435658);
  {
    _pgen_modulus_463 = Q_modulus();
    _pgen_num_q_464 = Poly_level(&input_dup_n50_363._c0_poly);
    for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
      Hw_modadd(Coeffs(&input_dup_n50_363._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435543._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435658._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      Hw_modadd(Coeffs(&input_dup_n50_363._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435543._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_preg_268435658._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      _pgen_modulus_463 = _pgen_modulus_463 + 1;
    }
    Free_poly_data(&_preg_268435658._c1_poly);
    Free_poly_data(&_preg_268435658._c0_poly);
    Free_poly_data(&_preg_268435543._c1_poly);
    Free_poly_data(&_preg_268435543._c0_poly);
  }
  for (index_gemm_n50_364 = 0; index_gemm_n50_364 < 1000; index_gemm_n50_364 = index_gemm_n50_364 + 1) {
    _preg_268435659 = Rotate(input_dup_n50_363, index_gemm_n50_364);
    Copy_ciph(&_ckks_gen_tmp_6253_444, &_preg_268435659);
    Pt_from_msg_ofst(&_pgen_tmp_508_974, 121/* cst_423 */, (index_gemm_n50_364) * (1000), 1000, 1, 3);
    Init_ciph_up_scale_plain(&_pgen_tmp_509_975, &_ckks_gen_tmp_6253_444, &_pgen_tmp_508_974);
    Init_ciph_same_scale(&tmp_result_n50_362, &tmp_result_n50_362, &_pgen_tmp_509_975);
    {
      _pgen_modulus_463 = Q_modulus();
      _pgen_num_q_464 = Poly_level(&tmp_result_n50_362._c0_poly);
      for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_509_975._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_6253_444._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_508_974._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_509_975._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_ckks_gen_tmp_6253_444._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_508_974._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&tmp_result_n50_362._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&tmp_result_n50_362._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_509_975._c0_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        Hw_modadd(Coeffs(&tmp_result_n50_362._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&tmp_result_n50_362._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_509_975._c1_poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
        _pgen_modulus_463 = _pgen_modulus_463 + 1;
      }
    }
    Free_poly_data(&_preg_268435659._c1_poly);
    Free_poly_data(&_preg_268435659._c0_poly);
  }
  Free_poly_data(&input_dup_n50_363._c1_poly);
  Free_poly_data(&input_dup_n50_363._c0_poly);
  Free_poly_data(&_pgen_tmp_508_974._poly);
  Free_poly_data(&_ckks_gen_tmp_6253_444._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_6253_444._c0_poly);
  Free_poly_data(&_pgen_tmp_509_975._c1_poly);
  Free_poly_data(&_pgen_tmp_509_975._c0_poly);
  Init_ciph_down_scale(&tmp_result_n50_362, &tmp_result_n50_362);
  Rescale(&tmp_result_n50_362._c0_poly, &tmp_result_n50_362._c0_poly);
  Rescale(&tmp_result_n50_362._c1_poly, &tmp_result_n50_362._c1_poly);
  Pt_from_msg(&_pgen_tmp_510_976, 122 /* cst_1 */, 1000, 1, 2);
  Init_ciph_same_scale_plain(&tmp_result_n50_362, &tmp_result_n50_362, &_pgen_tmp_510_976);
  {
    _pgen_modulus_463 = Q_modulus();
    _pgen_num_q_464 = Poly_level(&tmp_result_n50_362._c0_poly);
    for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
      Hw_modadd(Coeffs(&tmp_result_n50_362._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&tmp_result_n50_362._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_510_976._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      Set_coeffs(&tmp_result_n50_362._c1_poly, _pgen_rns_idx_465, degree, Coeffs(&tmp_result_n50_362._c1_poly, _pgen_rns_idx_465, degree));
      _pgen_modulus_463 = _pgen_modulus_463 + 1;
    }
    Free_poly_data(&_pgen_tmp_510_976._poly);
  }
  Pt_from_msg(&_pgen_tmp_512_978, 123 /* cst_424 */, 1000, 1, 2);
  Init_ciph_up_scale_plain(&_pgen_tmp_511_977, &tmp_result_n50_362, &_pgen_tmp_512_978);
  {
    _pgen_modulus_463 = Q_modulus();
    _pgen_num_q_464 = Poly_level(&_pgen_tmp_511_977._c0_poly);
    for (_pgen_rns_idx_465 = 0; _pgen_rns_idx_465 < _pgen_num_q_464; _pgen_rns_idx_465 = _pgen_rns_idx_465 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_511_977._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&tmp_result_n50_362._c0_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_512_978._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_511_977._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&tmp_result_n50_362._c1_poly, _pgen_rns_idx_465, degree), Coeffs(&_pgen_tmp_512_978._poly, _pgen_rns_idx_465, degree), _pgen_modulus_463, degree);
      _pgen_modulus_463 = _pgen_modulus_463 + 1;
    }
    Free_poly_data(&_pgen_tmp_512_978._poly);
  }
  Init_ciph_down_scale(&tmp_result_n50_362, &_pgen_tmp_511_977);
  Rescale(&tmp_result_n50_362._c0_poly, &_pgen_tmp_511_977._c0_poly);
  Rescale(&tmp_result_n50_362._c1_poly, &_pgen_tmp_511_977._c1_poly);
  Free_poly_data(&_pgen_tmp_511_977._c1_poly);
  Free_poly_data(&_pgen_tmp_511_977._c0_poly);
  Copy_ciph(&output_365, &tmp_result_n50_362);
  Free_poly_data(&tmp_result_n50_362._c1_poly);
  Free_poly_data(&tmp_result_n50_362._c0_poly);
  Tm_taken("Tensor::gemm");
  Set_output_data("output", 0, &output_365);
  return true;
}

int Get_input_count() {
  return 1;
}

DATA_SCHEME* Get_encode_scheme(int idx) {
  static MAP_DESC desc_0[] = {
    { NORMAL, 25088, 0, 672, 1},
    { NORMAL, 25088, 25088, 672, 1},
    { NORMAL, 25088, 50176, 672, 1},
    { NORMAL, 25088, 75264, 672, 1},
    { NORMAL, 25088, 100352, 672, 1},
    { NORMAL, 25088, 125440, 672, 1},
  };
  static DATA_SCHEME scheme_0 = {
    "input", {1, 3, 224, 224}, 6, desc_0
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
  CIPHERTEXT _relu_tmp46_7;
  CIPHERTEXT _relu_tmp50_8;
  CIPHERTEXT _relu_tmp56_9;
  CIPHERTEXT _relu_tmp60_10;
  CIPHERTEXT _relu_tmp68_11;
  CIPHERTEXT _relu_tmp72_12;
  CIPHERTEXT _relu_tmp78_13;
  CIPHERTEXT _relu_tmp82_14;
  CIPHERTEXT _relu_tmp88_15;
  CIPHERTEXT _relu_tmp128_16;
  CIPHERTEXT _relu_tmp132_17;
  CIPHERTEXT _relu_tmp138_18;
  CIPHERTEXT _relu_tmp142_19;
  CIPHERTEXT _relu_tmp150_20;
  CIPHERTEXT _relu_tmp154_21;
  CIPHERTEXT _relu_tmp160_22;
  CIPHERTEXT _relu_tmp164_23;
  CIPHERTEXT _relu_tmp170_24;
  CIPHERTEXT _relu_tmp239_25;
  CIPHERTEXT _relu_tmp247_26;
  CIPHERTEXT3 _pgen_tmp_0_27;
  MODULUS* _pgen_modulus_28;
  uint32_t _pgen_num_q_29;
  uint32_t _pgen_rns_idx_30;
  POLY _pgen_tmp_1_31;
  POLY _pgen_tmp_2_32;
  CIPHERTEXT3 _pgen_tmp_3_33;
  CIPHERTEXT _pgen_tmp_4_34;
  PLAINTEXT _pgen_tmp_5_35;
  CIPHERTEXT3 _pgen_tmp_6_36;
  POLY _pgen_tmp_7_37;
  POLY _pgen_tmp_8_38;
  CIPHERTEXT3 _pgen_tmp_9_39;
  CIPHERTEXT _pgen_tmp_10_40;
  PLAINTEXT _pgen_tmp_11_41;
  CIPHERTEXT3 _pgen_tmp_12_42;
  CIPHERTEXT3 _pgen_tmp_13_43;
  CIPHERTEXT _pgen_tmp_14_44;
  PLAINTEXT _pgen_tmp_15_45;
  CIPHERTEXT _pgen_tmp_16_46;
  POLY _pgen_tmp_17_47;
  POLY _pgen_tmp_18_48;
  CIPHERTEXT3 _pgen_tmp_19_49;
  CIPHERTEXT _pgen_tmp_20_50;
  PLAINTEXT _pgen_tmp_21_51;
  CIPHERTEXT _pgen_tmp_22_52;
  CIPHERTEXT _pgen_tmp_23_53;
  POLY _pgen_tmp_24_54;
  POLY _pgen_tmp_25_55;
  CIPHERTEXT3 _pgen_tmp_26_56;
  CIPHERTEXT3 _pgen_tmp_27_57;
  CIPHERTEXT _pgen_tmp_28_58;
  PLAINTEXT _pgen_tmp_29_59;
  CIPHERTEXT _pgen_tmp_30_60;
  POLY _pgen_tmp_31_61;
  POLY _pgen_tmp_32_62;
  CIPHERTEXT3 _pgen_tmp_33_63;
  CIPHERTEXT _pgen_tmp_34_64;
  PLAINTEXT _pgen_tmp_35_65;
  CIPHERTEXT _pgen_tmp_36_66;
  CIPHERTEXT _pgen_tmp_37_67;
  CIPHERTEXT3 _pgen_tmp_38_68;
  POLY _pgen_tmp_39_69;
  POLY _pgen_tmp_40_70;
  CIPHERTEXT3 _pgen_tmp_41_71;
  CIPHERTEXT _pgen_tmp_42_72;
  PLAINTEXT _pgen_tmp_43_73;
  CIPHERTEXT3 _pgen_tmp_44_74;
  POLY _pgen_tmp_45_75;
  POLY _pgen_tmp_46_76;
  CIPHERTEXT3 _pgen_tmp_47_77;
  CIPHERTEXT _pgen_tmp_48_78;
  CIPHERTEXT _pgen_tmp_49_79;
  PLAINTEXT _pgen_tmp_50_80;
  CIPHERTEXT _pgen_tmp_51_81;
  CIPHERTEXT3 _pgen_tmp_52_82;
  POLY _pgen_tmp_53_83;
  POLY _pgen_tmp_54_84;
  CIPHERTEXT3 _pgen_tmp_55_85;
  CIPHERTEXT _pgen_tmp_56_86;
  PLAINTEXT _pgen_tmp_57_87;
  CIPHERTEXT3 _pgen_tmp_58_88;
  POLY _pgen_tmp_59_89;
  POLY _pgen_tmp_60_90;
  CIPHERTEXT3 _pgen_tmp_61_91;
  CIPHERTEXT _pgen_tmp_62_92;
  PLAINTEXT _pgen_tmp_63_93;
  CIPHERTEXT3 _pgen_tmp_64_94;
  CIPHERTEXT3 _pgen_tmp_65_95;
  CIPHERTEXT3 _pgen_tmp_66_96;
  CIPHERTEXT _pgen_tmp_67_97;
  PLAINTEXT _pgen_tmp_68_98;
  CIPHERTEXT _pgen_tmp_69_99;
  POLY _pgen_tmp_70_100;
  POLY _pgen_tmp_71_101;
  CIPHERTEXT3 _pgen_tmp_72_102;
  CIPHERTEXT _pgen_tmp_73_103;
  PLAINTEXT _pgen_tmp_74_104;
  CIPHERTEXT _pgen_tmp_75_105;
  CIPHERTEXT _pgen_tmp_76_106;
  POLY _pgen_tmp_77_107;
  POLY _pgen_tmp_78_108;
  CIPHERTEXT3 _pgen_tmp_79_109;
  CIPHERTEXT _pgen_tmp_80_110;
  PLAINTEXT _pgen_tmp_81_111;
  CIPHERTEXT _pgen_tmp_82_112;
  CIPHERTEXT _pgen_tmp_83_113;
  PLAINTEXT _pgen_tmp_84_114;
  CIPHERTEXT _pgen_tmp_85_115;
  CIPHERTEXT _pgen_tmp_86_116;
  CIPHERTEXT _pgen_tmp_87_117;
  POLY _pgen_tmp_88_118;
  POLY _pgen_tmp_89_119;
  CIPHERTEXT3 _pgen_tmp_90_120;
  CIPHERTEXT3 _pgen_tmp_91_121;
  CIPHERTEXT _pgen_tmp_92_122;
  PLAINTEXT _pgen_tmp_93_123;
  CIPHERTEXT _pgen_tmp_94_124;
  CIPHERTEXT _pgen_tmp_95_125;
  PLAINTEXT _pgen_tmp_96_126;
  CIPHERTEXT _pgen_tmp_97_127;
  CIPHERTEXT _pgen_tmp_98_128;
  POLY _pgen_tmp_99_129;
  POLY _pgen_tmp_100_130;
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
  POLY _pgen_tmp_111_141;
  POLY _pgen_tmp_112_142;
  CIPHERTEXT3 _pgen_tmp_113_143;
  CIPHERTEXT _pgen_tmp_114_144;
  PLAINTEXT _pgen_tmp_115_145;
  CIPHERTEXT3 _pgen_tmp_116_146;
  POLY _pgen_tmp_117_147;
  POLY _pgen_tmp_118_148;
  CIPHERTEXT3 _pgen_tmp_119_149;
  CIPHERTEXT _pgen_tmp_120_150;
  CIPHERTEXT _pgen_tmp_121_151;
  PLAINTEXT _pgen_tmp_122_152;
  CIPHERTEXT _pgen_tmp_123_153;
  CIPHERTEXT3 _pgen_tmp_124_154;
  POLY _pgen_tmp_125_155;
  POLY _pgen_tmp_126_156;
  CIPHERTEXT3 _pgen_tmp_127_157;
  CIPHERTEXT _pgen_tmp_128_158;
  PLAINTEXT _pgen_tmp_129_159;
  CIPHERTEXT3 _pgen_tmp_130_160;
  POLY _pgen_tmp_131_161;
  POLY _pgen_tmp_132_162;
  CIPHERTEXT3 _pgen_tmp_133_163;
  CIPHERTEXT _pgen_tmp_134_164;
  PLAINTEXT _pgen_tmp_135_165;
  CIPHERTEXT3 _pgen_tmp_136_166;
  CIPHERTEXT3 _pgen_tmp_137_167;
  CIPHERTEXT3 _pgen_tmp_138_168;
  CIPHERTEXT _pgen_tmp_139_169;
  PLAINTEXT _pgen_tmp_140_170;
  CIPHERTEXT _pgen_tmp_141_171;
  POLY _pgen_tmp_142_172;
  POLY _pgen_tmp_143_173;
  CIPHERTEXT3 _pgen_tmp_144_174;
  POLY _pgen_tmp_145_175;
  POLY _pgen_tmp_146_176;
  CIPHERTEXT3 _pgen_tmp_147_177;
  CIPHERTEXT3 _pgen_tmp_148_178;
  CIPHERTEXT _pgen_tmp_149_179;
  PLAINTEXT _pgen_tmp_150_180;
  CIPHERTEXT _pgen_tmp_151_181;
  POLY _pgen_tmp_152_182;
  POLY _pgen_tmp_153_183;
  CIPHERTEXT3 _pgen_tmp_154_184;
  CIPHERTEXT3 _pgen_tmp_155_185;
  CIPHERTEXT _pgen_tmp_156_186;
  PLAINTEXT _pgen_tmp_157_187;
  CIPHERTEXT _pgen_tmp_158_188;
  POLY _pgen_tmp_159_189;
  POLY _pgen_tmp_160_190;
  CIPHERTEXT3 _pgen_tmp_161_191;
  CIPHERTEXT _pgen_tmp_162_192;
  CIPHERTEXT _pgen_tmp_163_193;
  POLY _pgen_tmp_164_194;
  POLY _pgen_tmp_165_195;
  CIPHERTEXT3 _pgen_tmp_166_196;
  CIPHERTEXT3 _pgen_tmp_167_197;
  CIPHERTEXT3 _pgen_tmp_168_198;
  CIPHERTEXT _pgen_tmp_169_199;
  PLAINTEXT _pgen_tmp_170_200;
  CIPHERTEXT _pgen_tmp_171_201;
  POLY _pgen_tmp_172_202;
  POLY _pgen_tmp_173_203;
  CIPHERTEXT3 _pgen_tmp_174_204;
  CIPHERTEXT3 _pgen_tmp_175_205;
  CIPHERTEXT _pgen_tmp_176_206;
  PLAINTEXT _pgen_tmp_177_207;
  CIPHERTEXT _pgen_tmp_178_208;
  POLY _pgen_tmp_179_209;
  POLY _pgen_tmp_180_210;
  CIPHERTEXT3 _pgen_tmp_181_211;
  CIPHERTEXT _pgen_tmp_182_212;
  POLY _pgen_tmp_183_213;
  POLY _pgen_tmp_184_214;
  CIPHERTEXT3 _pgen_tmp_185_215;
  CIPHERTEXT3 _pgen_tmp_186_216;
  CIPHERTEXT _pgen_tmp_187_217;
  PLAINTEXT _pgen_tmp_188_218;
  CIPHERTEXT _pgen_tmp_189_219;
  POLY _pgen_tmp_190_220;
  POLY _pgen_tmp_191_221;
  CIPHERTEXT3 _pgen_tmp_192_222;
  CIPHERTEXT3 _pgen_tmp_193_223;
  CIPHERTEXT _pgen_tmp_194_224;
  PLAINTEXT _pgen_tmp_195_225;
  CIPHERTEXT _pgen_tmp_196_226;
  POLY _pgen_tmp_197_227;
  POLY _pgen_tmp_198_228;
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
  memset(&_relu_tmp46_7, 0, sizeof(_relu_tmp46_7));
  memset(&_relu_tmp50_8, 0, sizeof(_relu_tmp50_8));
  memset(&_relu_tmp56_9, 0, sizeof(_relu_tmp56_9));
  memset(&_relu_tmp60_10, 0, sizeof(_relu_tmp60_10));
  memset(&_relu_tmp68_11, 0, sizeof(_relu_tmp68_11));
  memset(&_relu_tmp72_12, 0, sizeof(_relu_tmp72_12));
  memset(&_relu_tmp78_13, 0, sizeof(_relu_tmp78_13));
  memset(&_relu_tmp82_14, 0, sizeof(_relu_tmp82_14));
  memset(&_relu_tmp88_15, 0, sizeof(_relu_tmp88_15));
  memset(&_relu_tmp128_16, 0, sizeof(_relu_tmp128_16));
  memset(&_relu_tmp132_17, 0, sizeof(_relu_tmp132_17));
  memset(&_relu_tmp138_18, 0, sizeof(_relu_tmp138_18));
  memset(&_relu_tmp142_19, 0, sizeof(_relu_tmp142_19));
  memset(&_relu_tmp150_20, 0, sizeof(_relu_tmp150_20));
  memset(&_relu_tmp154_21, 0, sizeof(_relu_tmp154_21));
  memset(&_relu_tmp160_22, 0, sizeof(_relu_tmp160_22));
  memset(&_relu_tmp164_23, 0, sizeof(_relu_tmp164_23));
  memset(&_relu_tmp170_24, 0, sizeof(_relu_tmp170_24));
  memset(&_relu_tmp239_25, 0, sizeof(_relu_tmp239_25));
  memset(&_relu_tmp247_26, 0, sizeof(_relu_tmp247_26));
  memset(&_pgen_tmp_0_27, 0, sizeof(_pgen_tmp_0_27));
  memset(&_pgen_tmp_3_33, 0, sizeof(_pgen_tmp_3_33));
  memset(&_pgen_tmp_4_34, 0, sizeof(_pgen_tmp_4_34));
  memset(&_pgen_tmp_5_35, 0, sizeof(_pgen_tmp_5_35));
  memset(&_pgen_tmp_6_36, 0, sizeof(_pgen_tmp_6_36));
  memset(&_pgen_tmp_9_39, 0, sizeof(_pgen_tmp_9_39));
  memset(&_pgen_tmp_10_40, 0, sizeof(_pgen_tmp_10_40));
  memset(&_pgen_tmp_11_41, 0, sizeof(_pgen_tmp_11_41));
  memset(&_pgen_tmp_12_42, 0, sizeof(_pgen_tmp_12_42));
  memset(&_pgen_tmp_13_43, 0, sizeof(_pgen_tmp_13_43));
  memset(&_pgen_tmp_14_44, 0, sizeof(_pgen_tmp_14_44));
  memset(&_pgen_tmp_15_45, 0, sizeof(_pgen_tmp_15_45));
  memset(&_pgen_tmp_16_46, 0, sizeof(_pgen_tmp_16_46));
  memset(&_pgen_tmp_19_49, 0, sizeof(_pgen_tmp_19_49));
  memset(&_pgen_tmp_20_50, 0, sizeof(_pgen_tmp_20_50));
  memset(&_pgen_tmp_21_51, 0, sizeof(_pgen_tmp_21_51));
  memset(&_pgen_tmp_22_52, 0, sizeof(_pgen_tmp_22_52));
  memset(&_pgen_tmp_23_53, 0, sizeof(_pgen_tmp_23_53));
  memset(&_pgen_tmp_26_56, 0, sizeof(_pgen_tmp_26_56));
  memset(&_pgen_tmp_27_57, 0, sizeof(_pgen_tmp_27_57));
  memset(&_pgen_tmp_28_58, 0, sizeof(_pgen_tmp_28_58));
  memset(&_pgen_tmp_29_59, 0, sizeof(_pgen_tmp_29_59));
  memset(&_pgen_tmp_30_60, 0, sizeof(_pgen_tmp_30_60));
  memset(&_pgen_tmp_33_63, 0, sizeof(_pgen_tmp_33_63));
  memset(&_pgen_tmp_34_64, 0, sizeof(_pgen_tmp_34_64));
  memset(&_pgen_tmp_35_65, 0, sizeof(_pgen_tmp_35_65));
  memset(&_pgen_tmp_36_66, 0, sizeof(_pgen_tmp_36_66));
  memset(&_pgen_tmp_37_67, 0, sizeof(_pgen_tmp_37_67));
  memset(&_pgen_tmp_38_68, 0, sizeof(_pgen_tmp_38_68));
  memset(&_pgen_tmp_41_71, 0, sizeof(_pgen_tmp_41_71));
  memset(&_pgen_tmp_42_72, 0, sizeof(_pgen_tmp_42_72));
  memset(&_pgen_tmp_43_73, 0, sizeof(_pgen_tmp_43_73));
  memset(&_pgen_tmp_44_74, 0, sizeof(_pgen_tmp_44_74));
  memset(&_pgen_tmp_47_77, 0, sizeof(_pgen_tmp_47_77));
  memset(&_pgen_tmp_48_78, 0, sizeof(_pgen_tmp_48_78));
  memset(&_pgen_tmp_49_79, 0, sizeof(_pgen_tmp_49_79));
  memset(&_pgen_tmp_50_80, 0, sizeof(_pgen_tmp_50_80));
  memset(&_pgen_tmp_51_81, 0, sizeof(_pgen_tmp_51_81));
  memset(&_pgen_tmp_52_82, 0, sizeof(_pgen_tmp_52_82));
  memset(&_pgen_tmp_55_85, 0, sizeof(_pgen_tmp_55_85));
  memset(&_pgen_tmp_56_86, 0, sizeof(_pgen_tmp_56_86));
  memset(&_pgen_tmp_57_87, 0, sizeof(_pgen_tmp_57_87));
  memset(&_pgen_tmp_58_88, 0, sizeof(_pgen_tmp_58_88));
  memset(&_pgen_tmp_61_91, 0, sizeof(_pgen_tmp_61_91));
  memset(&_pgen_tmp_62_92, 0, sizeof(_pgen_tmp_62_92));
  memset(&_pgen_tmp_63_93, 0, sizeof(_pgen_tmp_63_93));
  memset(&_pgen_tmp_64_94, 0, sizeof(_pgen_tmp_64_94));
  memset(&_pgen_tmp_65_95, 0, sizeof(_pgen_tmp_65_95));
  memset(&_pgen_tmp_66_96, 0, sizeof(_pgen_tmp_66_96));
  memset(&_pgen_tmp_67_97, 0, sizeof(_pgen_tmp_67_97));
  memset(&_pgen_tmp_68_98, 0, sizeof(_pgen_tmp_68_98));
  memset(&_pgen_tmp_69_99, 0, sizeof(_pgen_tmp_69_99));
  memset(&_pgen_tmp_72_102, 0, sizeof(_pgen_tmp_72_102));
  memset(&_pgen_tmp_73_103, 0, sizeof(_pgen_tmp_73_103));
  memset(&_pgen_tmp_74_104, 0, sizeof(_pgen_tmp_74_104));
  memset(&_pgen_tmp_75_105, 0, sizeof(_pgen_tmp_75_105));
  memset(&_pgen_tmp_76_106, 0, sizeof(_pgen_tmp_76_106));
  memset(&_pgen_tmp_79_109, 0, sizeof(_pgen_tmp_79_109));
  memset(&_pgen_tmp_80_110, 0, sizeof(_pgen_tmp_80_110));
  memset(&_pgen_tmp_81_111, 0, sizeof(_pgen_tmp_81_111));
  memset(&_pgen_tmp_82_112, 0, sizeof(_pgen_tmp_82_112));
  memset(&_pgen_tmp_83_113, 0, sizeof(_pgen_tmp_83_113));
  memset(&_pgen_tmp_84_114, 0, sizeof(_pgen_tmp_84_114));
  memset(&_pgen_tmp_85_115, 0, sizeof(_pgen_tmp_85_115));
  memset(&_pgen_tmp_86_116, 0, sizeof(_pgen_tmp_86_116));
  memset(&_pgen_tmp_87_117, 0, sizeof(_pgen_tmp_87_117));
  memset(&_pgen_tmp_90_120, 0, sizeof(_pgen_tmp_90_120));
  memset(&_pgen_tmp_91_121, 0, sizeof(_pgen_tmp_91_121));
  memset(&_pgen_tmp_92_122, 0, sizeof(_pgen_tmp_92_122));
  memset(&_pgen_tmp_93_123, 0, sizeof(_pgen_tmp_93_123));
  memset(&_pgen_tmp_94_124, 0, sizeof(_pgen_tmp_94_124));
  memset(&_pgen_tmp_95_125, 0, sizeof(_pgen_tmp_95_125));
  memset(&_pgen_tmp_96_126, 0, sizeof(_pgen_tmp_96_126));
  memset(&_pgen_tmp_97_127, 0, sizeof(_pgen_tmp_97_127));
  memset(&_pgen_tmp_98_128, 0, sizeof(_pgen_tmp_98_128));
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
  memset(&_pgen_tmp_113_143, 0, sizeof(_pgen_tmp_113_143));
  memset(&_pgen_tmp_114_144, 0, sizeof(_pgen_tmp_114_144));
  memset(&_pgen_tmp_115_145, 0, sizeof(_pgen_tmp_115_145));
  memset(&_pgen_tmp_116_146, 0, sizeof(_pgen_tmp_116_146));
  memset(&_pgen_tmp_119_149, 0, sizeof(_pgen_tmp_119_149));
  memset(&_pgen_tmp_120_150, 0, sizeof(_pgen_tmp_120_150));
  memset(&_pgen_tmp_121_151, 0, sizeof(_pgen_tmp_121_151));
  memset(&_pgen_tmp_122_152, 0, sizeof(_pgen_tmp_122_152));
  memset(&_pgen_tmp_123_153, 0, sizeof(_pgen_tmp_123_153));
  memset(&_pgen_tmp_124_154, 0, sizeof(_pgen_tmp_124_154));
  memset(&_pgen_tmp_127_157, 0, sizeof(_pgen_tmp_127_157));
  memset(&_pgen_tmp_128_158, 0, sizeof(_pgen_tmp_128_158));
  memset(&_pgen_tmp_129_159, 0, sizeof(_pgen_tmp_129_159));
  memset(&_pgen_tmp_130_160, 0, sizeof(_pgen_tmp_130_160));
  memset(&_pgen_tmp_133_163, 0, sizeof(_pgen_tmp_133_163));
  memset(&_pgen_tmp_134_164, 0, sizeof(_pgen_tmp_134_164));
  memset(&_pgen_tmp_135_165, 0, sizeof(_pgen_tmp_135_165));
  memset(&_pgen_tmp_136_166, 0, sizeof(_pgen_tmp_136_166));
  memset(&_pgen_tmp_137_167, 0, sizeof(_pgen_tmp_137_167));
  memset(&_pgen_tmp_138_168, 0, sizeof(_pgen_tmp_138_168));
  memset(&_pgen_tmp_139_169, 0, sizeof(_pgen_tmp_139_169));
  memset(&_pgen_tmp_140_170, 0, sizeof(_pgen_tmp_140_170));
  memset(&_pgen_tmp_141_171, 0, sizeof(_pgen_tmp_141_171));
  memset(&_pgen_tmp_144_174, 0, sizeof(_pgen_tmp_144_174));
  memset(&_pgen_tmp_147_177, 0, sizeof(_pgen_tmp_147_177));
  memset(&_pgen_tmp_148_178, 0, sizeof(_pgen_tmp_148_178));
  memset(&_pgen_tmp_149_179, 0, sizeof(_pgen_tmp_149_179));
  memset(&_pgen_tmp_150_180, 0, sizeof(_pgen_tmp_150_180));
  memset(&_pgen_tmp_151_181, 0, sizeof(_pgen_tmp_151_181));
  memset(&_pgen_tmp_154_184, 0, sizeof(_pgen_tmp_154_184));
  memset(&_pgen_tmp_155_185, 0, sizeof(_pgen_tmp_155_185));
  memset(&_pgen_tmp_156_186, 0, sizeof(_pgen_tmp_156_186));
  memset(&_pgen_tmp_157_187, 0, sizeof(_pgen_tmp_157_187));
  memset(&_pgen_tmp_158_188, 0, sizeof(_pgen_tmp_158_188));
  memset(&_pgen_tmp_161_191, 0, sizeof(_pgen_tmp_161_191));
  memset(&_pgen_tmp_162_192, 0, sizeof(_pgen_tmp_162_192));
  memset(&_pgen_tmp_163_193, 0, sizeof(_pgen_tmp_163_193));
  memset(&_pgen_tmp_166_196, 0, sizeof(_pgen_tmp_166_196));
  memset(&_pgen_tmp_167_197, 0, sizeof(_pgen_tmp_167_197));
  memset(&_pgen_tmp_168_198, 0, sizeof(_pgen_tmp_168_198));
  memset(&_pgen_tmp_169_199, 0, sizeof(_pgen_tmp_169_199));
  memset(&_pgen_tmp_170_200, 0, sizeof(_pgen_tmp_170_200));
  memset(&_pgen_tmp_171_201, 0, sizeof(_pgen_tmp_171_201));
  memset(&_pgen_tmp_174_204, 0, sizeof(_pgen_tmp_174_204));
  memset(&_pgen_tmp_175_205, 0, sizeof(_pgen_tmp_175_205));
  memset(&_pgen_tmp_176_206, 0, sizeof(_pgen_tmp_176_206));
  memset(&_pgen_tmp_177_207, 0, sizeof(_pgen_tmp_177_207));
  memset(&_pgen_tmp_178_208, 0, sizeof(_pgen_tmp_178_208));
  memset(&_pgen_tmp_181_211, 0, sizeof(_pgen_tmp_181_211));
  memset(&_pgen_tmp_182_212, 0, sizeof(_pgen_tmp_182_212));
  memset(&_pgen_tmp_185_215, 0, sizeof(_pgen_tmp_185_215));
  memset(&_pgen_tmp_186_216, 0, sizeof(_pgen_tmp_186_216));
  memset(&_pgen_tmp_187_217, 0, sizeof(_pgen_tmp_187_217));
  memset(&_pgen_tmp_188_218, 0, sizeof(_pgen_tmp_188_218));
  memset(&_pgen_tmp_189_219, 0, sizeof(_pgen_tmp_189_219));
  memset(&_pgen_tmp_192_222, 0, sizeof(_pgen_tmp_192_222));
  memset(&_pgen_tmp_193_223, 0, sizeof(_pgen_tmp_193_223));
  memset(&_pgen_tmp_194_224, 0, sizeof(_pgen_tmp_194_224));
  memset(&_pgen_tmp_195_225, 0, sizeof(_pgen_tmp_195_225));
  memset(&_pgen_tmp_196_226, 0, sizeof(_pgen_tmp_196_226));
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
  Copy_ciph(&input_ct0_0, &input_ct0_0);
  Copy_ciph(&input_ct1_1, &input_ct1_1);
  Init_ciph3_up_scale(&_pgen_tmp_0_27, &input_ct1_1, &input_ct1_1);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_0_27._c0_poly);
    _pgen_tmp_1_31 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_2_32 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_1_31, 0, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_2_32, 0, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_0_27._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_0_27._c1_poly, _pgen_rns_idx_30, degree), Coeffs(_pgen_tmp_1_31, 0, degree), Coeffs(_pgen_tmp_2_32, 0, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_0_27._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly(_pgen_tmp_1_31);
    Free_poly(_pgen_tmp_2_32);
  }
  Init_ciph3_down_scale(&_pgen_tmp_3_33, &_pgen_tmp_0_27);
  Rescale(&_pgen_tmp_3_33._c0_poly, &_pgen_tmp_0_27._c0_poly);
  Rescale(&_pgen_tmp_3_33._c1_poly, &_pgen_tmp_0_27._c1_poly);
  Rescale(&_pgen_tmp_3_33._c2_poly, &_pgen_tmp_0_27._c2_poly);
  Free_poly_data(&_pgen_tmp_0_27._c2_poly);
  Free_poly_data(&_pgen_tmp_0_27._c1_poly);
  Free_poly_data(&_pgen_tmp_0_27._c0_poly);
  _preg_268435456 = Relinearize(_pgen_tmp_3_33);
  Free_poly_data(&_pgen_tmp_3_33._c2_poly);
  Free_poly_data(&_pgen_tmp_3_33._c1_poly);
  Free_poly_data(&_pgen_tmp_3_33._c0_poly);
  Copy_ciph(&_relu_tmp8_3, &_preg_268435456);
  Free_poly_data(&_preg_268435456._c1_poly);
  Free_poly_data(&_preg_268435456._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_4_34, &_relu_tmp8_3, &_relu_tmp8_3);
  Encode_float(&_pgen_tmp_5_35, &_cst_425, 1, Sc_degree(&_pgen_tmp_4_34), Level(&_pgen_tmp_4_34));
  Init_ciph_same_scale_plain(&_relu_tmp14_4, &_pgen_tmp_4_34, &_pgen_tmp_5_35);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_relu_tmp14_4._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_4_34._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp8_3._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp8_3._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_4_34._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp8_3._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp8_3._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_4_34._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_5_35._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Set_coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_30, degree, Coeffs(&_pgen_tmp_4_34._c1_poly, _pgen_rns_idx_30, degree));
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_relu_tmp8_3._c1_poly);
    Free_poly_data(&_relu_tmp8_3._c0_poly);
    Free_poly_data(&_pgen_tmp_5_35._poly);
    Free_poly_data(&_pgen_tmp_4_34._c1_poly);
    Free_poly_data(&_pgen_tmp_4_34._c0_poly);
  }
  Init_ciph3_up_scale(&_pgen_tmp_6_36, &_relu_tmp14_4, &_relu_tmp14_4);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_6_36._c0_poly);
    _pgen_tmp_7_37 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_8_38 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_7_37, 0, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_8_38, 0, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_6_36._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_6_36._c1_poly, _pgen_rns_idx_30, degree), Coeffs(_pgen_tmp_7_37, 0, degree), Coeffs(_pgen_tmp_8_38, 0, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_6_36._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly(_pgen_tmp_7_37);
    Free_poly(_pgen_tmp_8_38);
  }
  Init_ciph3_down_scale(&_pgen_tmp_9_39, &_pgen_tmp_6_36);
  Rescale(&_pgen_tmp_9_39._c0_poly, &_pgen_tmp_6_36._c0_poly);
  Rescale(&_pgen_tmp_9_39._c1_poly, &_pgen_tmp_6_36._c1_poly);
  Rescale(&_pgen_tmp_9_39._c2_poly, &_pgen_tmp_6_36._c2_poly);
  Free_poly_data(&_pgen_tmp_6_36._c2_poly);
  Free_poly_data(&_pgen_tmp_6_36._c1_poly);
  Free_poly_data(&_pgen_tmp_6_36._c0_poly);
  _preg_268435457 = Relinearize(_pgen_tmp_9_39);
  Free_poly_data(&_pgen_tmp_9_39._c2_poly);
  Free_poly_data(&_pgen_tmp_9_39._c1_poly);
  Free_poly_data(&_pgen_tmp_9_39._c0_poly);
  Copy_ciph(&_relu_tmp18_5, &_preg_268435457);
  Free_poly_data(&_preg_268435457._c1_poly);
  Free_poly_data(&_preg_268435457._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_10_40, &_relu_tmp18_5, &_relu_tmp18_5);
  Encode_float(&_pgen_tmp_11_41, &_cst_426, 1, Sc_degree(&_pgen_tmp_10_40), Level(&_pgen_tmp_10_40));
  Init_ciph_same_scale_plain(&_relu_tmp24_6, &_pgen_tmp_10_40, &_pgen_tmp_11_41);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_relu_tmp24_6._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_10_40._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp18_5._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp18_5._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_10_40._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp18_5._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp18_5._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_relu_tmp24_6._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_10_40._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_11_41._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Set_coeffs(&_relu_tmp24_6._c1_poly, _pgen_rns_idx_30, degree, Coeffs(&_pgen_tmp_10_40._c1_poly, _pgen_rns_idx_30, degree));
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_relu_tmp18_5._c1_poly);
    Free_poly_data(&_relu_tmp18_5._c0_poly);
    Free_poly_data(&_pgen_tmp_11_41._poly);
    Free_poly_data(&_pgen_tmp_10_40._c1_poly);
    Free_poly_data(&_pgen_tmp_10_40._c0_poly);
  }
  Encode_double(&_pgen_tmp_15_45, &_cst_427, 1, 1, Level(&input_ct1_1));
  Init_ciph_up_scale_plain(&_pgen_tmp_14_44, &input_ct1_1, &_pgen_tmp_15_45);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_14_44._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_14_44._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_15_45._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_14_44._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_15_45._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_15_45._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_16_46, &_pgen_tmp_14_44);
  Rescale(&_pgen_tmp_16_46._c0_poly, &_pgen_tmp_14_44._c0_poly);
  Rescale(&_pgen_tmp_16_46._c1_poly, &_pgen_tmp_14_44._c1_poly);
  Free_poly_data(&_pgen_tmp_14_44._c1_poly);
  Free_poly_data(&_pgen_tmp_14_44._c0_poly);
  Init_ciph3_up_scale(&_pgen_tmp_13_43, &_relu_tmp14_4, &_pgen_tmp_16_46);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_13_43._c0_poly);
    _pgen_tmp_17_47 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_18_48 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_17_47, 0, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_16_46._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_18_48, 0, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_16_46._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_13_43._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_16_46._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_13_43._c1_poly, _pgen_rns_idx_30, degree), Coeffs(_pgen_tmp_17_47, 0, degree), Coeffs(_pgen_tmp_18_48, 0, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_13_43._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_16_46._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_16_46._c1_poly);
    Free_poly_data(&_pgen_tmp_16_46._c0_poly);
    Free_poly(_pgen_tmp_17_47);
    Free_poly(_pgen_tmp_18_48);
  }
  Init_ciph3_down_scale(&_pgen_tmp_19_49, &_pgen_tmp_13_43);
  Rescale(&_pgen_tmp_19_49._c0_poly, &_pgen_tmp_13_43._c0_poly);
  Rescale(&_pgen_tmp_19_49._c1_poly, &_pgen_tmp_13_43._c1_poly);
  Rescale(&_pgen_tmp_19_49._c2_poly, &_pgen_tmp_13_43._c2_poly);
  Free_poly_data(&_pgen_tmp_13_43._c2_poly);
  Free_poly_data(&_pgen_tmp_13_43._c1_poly);
  Free_poly_data(&_pgen_tmp_13_43._c0_poly);
  _preg_268435458 = Relinearize(_pgen_tmp_19_49);
  Free_poly_data(&_pgen_tmp_19_49._c2_poly);
  Free_poly_data(&_pgen_tmp_19_49._c1_poly);
  Free_poly_data(&_pgen_tmp_19_49._c0_poly);
  Encode_double(&_pgen_tmp_21_51, &_cst_428, 1, 1, Level(&input_ct1_1));
  Init_ciph_up_scale_plain(&_pgen_tmp_20_50, &input_ct1_1, &_pgen_tmp_21_51);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_20_50._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_20_50._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_21_51._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_20_50._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_21_51._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_21_51._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_22_52, &_pgen_tmp_20_50);
  Rescale(&_pgen_tmp_22_52._c0_poly, &_pgen_tmp_20_50._c0_poly);
  Rescale(&_pgen_tmp_22_52._c1_poly, &_pgen_tmp_20_50._c1_poly);
  Free_poly_data(&_pgen_tmp_20_50._c1_poly);
  Free_poly_data(&_pgen_tmp_20_50._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_23_53, &_preg_268435458, &_pgen_tmp_22_52);
  Init_ciph3_up_scale(&_pgen_tmp_12_42, &_relu_tmp24_6, &_pgen_tmp_23_53);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_12_42._c0_poly);
    _pgen_tmp_24_54 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_25_55 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_23_53._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435458._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_22_52._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_23_53._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435458._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_22_52._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_24_54, 0, degree), Coeffs(&_relu_tmp24_6._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_23_53._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_25_55, 0, degree), Coeffs(&_relu_tmp24_6._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_23_53._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_12_42._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp24_6._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_23_53._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_12_42._c1_poly, _pgen_rns_idx_30, degree), Coeffs(_pgen_tmp_24_54, 0, degree), Coeffs(_pgen_tmp_25_55, 0, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_12_42._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp24_6._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_23_53._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_22_52._c1_poly);
    Free_poly_data(&_pgen_tmp_22_52._c0_poly);
    Free_poly_data(&_preg_268435458._c1_poly);
    Free_poly_data(&_preg_268435458._c0_poly);
    Free_poly_data(&_pgen_tmp_23_53._c1_poly);
    Free_poly_data(&_pgen_tmp_23_53._c0_poly);
    Free_poly_data(&_relu_tmp24_6._c1_poly);
    Free_poly_data(&_relu_tmp24_6._c0_poly);
    Free_poly(_pgen_tmp_24_54);
    Free_poly(_pgen_tmp_25_55);
  }
  Init_ciph3_down_scale(&_pgen_tmp_26_56, &_pgen_tmp_12_42);
  Rescale(&_pgen_tmp_26_56._c0_poly, &_pgen_tmp_12_42._c0_poly);
  Rescale(&_pgen_tmp_26_56._c1_poly, &_pgen_tmp_12_42._c1_poly);
  Rescale(&_pgen_tmp_26_56._c2_poly, &_pgen_tmp_12_42._c2_poly);
  Free_poly_data(&_pgen_tmp_12_42._c2_poly);
  Free_poly_data(&_pgen_tmp_12_42._c1_poly);
  Free_poly_data(&_pgen_tmp_12_42._c0_poly);
  _preg_268435459 = Relinearize(_pgen_tmp_26_56);
  Free_poly_data(&_pgen_tmp_26_56._c2_poly);
  Free_poly_data(&_pgen_tmp_26_56._c1_poly);
  Free_poly_data(&_pgen_tmp_26_56._c0_poly);
  Encode_double(&_pgen_tmp_29_59, &_cst_429, 1, 1, Level(&input_ct1_1));
  Init_ciph_up_scale_plain(&_pgen_tmp_28_58, &input_ct1_1, &_pgen_tmp_29_59);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_28_58._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_28_58._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_29_59._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_28_58._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_29_59._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_29_59._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_30_60, &_pgen_tmp_28_58);
  Rescale(&_pgen_tmp_30_60._c0_poly, &_pgen_tmp_28_58._c0_poly);
  Rescale(&_pgen_tmp_30_60._c1_poly, &_pgen_tmp_28_58._c1_poly);
  Free_poly_data(&_pgen_tmp_28_58._c1_poly);
  Free_poly_data(&_pgen_tmp_28_58._c0_poly);
  Init_ciph3_up_scale(&_pgen_tmp_27_57, &_relu_tmp14_4, &_pgen_tmp_30_60);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_27_57._c0_poly);
    _pgen_tmp_31_61 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_32_62 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_31_61, 0, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_30_60._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_32_62, 0, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_30_60._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_27_57._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp14_4._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_30_60._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_27_57._c1_poly, _pgen_rns_idx_30, degree), Coeffs(_pgen_tmp_31_61, 0, degree), Coeffs(_pgen_tmp_32_62, 0, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_27_57._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp14_4._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_30_60._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_30_60._c1_poly);
    Free_poly_data(&_pgen_tmp_30_60._c0_poly);
    Free_poly_data(&_relu_tmp14_4._c1_poly);
    Free_poly_data(&_relu_tmp14_4._c0_poly);
    Free_poly(_pgen_tmp_31_61);
    Free_poly(_pgen_tmp_32_62);
  }
  Init_ciph3_down_scale(&_pgen_tmp_33_63, &_pgen_tmp_27_57);
  Rescale(&_pgen_tmp_33_63._c0_poly, &_pgen_tmp_27_57._c0_poly);
  Rescale(&_pgen_tmp_33_63._c1_poly, &_pgen_tmp_27_57._c1_poly);
  Rescale(&_pgen_tmp_33_63._c2_poly, &_pgen_tmp_27_57._c2_poly);
  Free_poly_data(&_pgen_tmp_27_57._c2_poly);
  Free_poly_data(&_pgen_tmp_27_57._c1_poly);
  Free_poly_data(&_pgen_tmp_27_57._c0_poly);
  _preg_268435460 = Relinearize(_pgen_tmp_33_63);
  Free_poly_data(&_pgen_tmp_33_63._c2_poly);
  Free_poly_data(&_pgen_tmp_33_63._c1_poly);
  Free_poly_data(&_pgen_tmp_33_63._c0_poly);
  Encode_double(&_pgen_tmp_35_65, &_cst_430, 1, 1, Level(&input_ct1_1));
  Init_ciph_up_scale_plain(&_pgen_tmp_34_64, &input_ct1_1, &_pgen_tmp_35_65);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_34_64._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_34_64._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_35_65._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_34_64._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct1_1._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_35_65._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_35_65._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_36_66, &_pgen_tmp_34_64);
  Rescale(&_pgen_tmp_36_66._c0_poly, &_pgen_tmp_34_64._c0_poly);
  Rescale(&_pgen_tmp_36_66._c1_poly, &_pgen_tmp_34_64._c1_poly);
  Free_poly_data(&_pgen_tmp_34_64._c1_poly);
  Free_poly_data(&_pgen_tmp_34_64._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_37_67, &_preg_268435460, &_pgen_tmp_36_66);
  Init_ciph_same_scale(&_relu_tmp46_7, &_preg_268435459, &_pgen_tmp_37_67);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_relu_tmp46_7._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_37_67._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435460._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_36_66._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_37_67._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435460._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_36_66._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_relu_tmp46_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435459._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_37_67._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_relu_tmp46_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435459._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_37_67._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_36_66._c1_poly);
    Free_poly_data(&_pgen_tmp_36_66._c0_poly);
    Free_poly_data(&_preg_268435460._c1_poly);
    Free_poly_data(&_preg_268435460._c0_poly);
    Free_poly_data(&_pgen_tmp_37_67._c1_poly);
    Free_poly_data(&_pgen_tmp_37_67._c0_poly);
    Free_poly_data(&_preg_268435459._c1_poly);
    Free_poly_data(&_preg_268435459._c0_poly);
  }
  Init_ciph3_up_scale(&_pgen_tmp_38_68, &_relu_tmp46_7, &_relu_tmp46_7);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_38_68._c0_poly);
    _pgen_tmp_39_69 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_40_70 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_39_69, 0, degree), Coeffs(&_relu_tmp46_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp46_7._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_40_70, 0, degree), Coeffs(&_relu_tmp46_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp46_7._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_38_68._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp46_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp46_7._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_38_68._c1_poly, _pgen_rns_idx_30, degree), Coeffs(_pgen_tmp_39_69, 0, degree), Coeffs(_pgen_tmp_40_70, 0, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_38_68._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp46_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp46_7._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly(_pgen_tmp_39_69);
    Free_poly(_pgen_tmp_40_70);
  }
  Init_ciph3_down_scale(&_pgen_tmp_41_71, &_pgen_tmp_38_68);
  Rescale(&_pgen_tmp_41_71._c0_poly, &_pgen_tmp_38_68._c0_poly);
  Rescale(&_pgen_tmp_41_71._c1_poly, &_pgen_tmp_38_68._c1_poly);
  Rescale(&_pgen_tmp_41_71._c2_poly, &_pgen_tmp_38_68._c2_poly);
  Free_poly_data(&_pgen_tmp_38_68._c2_poly);
  Free_poly_data(&_pgen_tmp_38_68._c1_poly);
  Free_poly_data(&_pgen_tmp_38_68._c0_poly);
  _preg_268435461 = Relinearize(_pgen_tmp_41_71);
  Free_poly_data(&_pgen_tmp_41_71._c2_poly);
  Free_poly_data(&_pgen_tmp_41_71._c1_poly);
  Free_poly_data(&_pgen_tmp_41_71._c0_poly);
  Copy_ciph(&_relu_tmp50_8, &_preg_268435461);
  Free_poly_data(&_preg_268435461._c1_poly);
  Free_poly_data(&_preg_268435461._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_42_72, &_relu_tmp50_8, &_relu_tmp50_8);
  Encode_float(&_pgen_tmp_43_73, &_cst_431, 1, Sc_degree(&_pgen_tmp_42_72), Level(&_pgen_tmp_42_72));
  Init_ciph_same_scale_plain(&_relu_tmp56_9, &_pgen_tmp_42_72, &_pgen_tmp_43_73);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_relu_tmp56_9._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_42_72._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp50_8._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp50_8._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_42_72._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp50_8._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp50_8._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_relu_tmp56_9._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_42_72._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_43_73._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Set_coeffs(&_relu_tmp56_9._c1_poly, _pgen_rns_idx_30, degree, Coeffs(&_pgen_tmp_42_72._c1_poly, _pgen_rns_idx_30, degree));
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_relu_tmp50_8._c1_poly);
    Free_poly_data(&_relu_tmp50_8._c0_poly);
    Free_poly_data(&_pgen_tmp_43_73._poly);
    Free_poly_data(&_pgen_tmp_42_72._c1_poly);
    Free_poly_data(&_pgen_tmp_42_72._c0_poly);
  }
  Init_ciph3_up_scale(&_pgen_tmp_44_74, &_relu_tmp46_7, &_relu_tmp56_9);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_44_74._c0_poly);
    _pgen_tmp_45_75 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_46_76 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_45_75, 0, degree), Coeffs(&_relu_tmp46_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp56_9._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_46_76, 0, degree), Coeffs(&_relu_tmp46_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp56_9._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_44_74._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp46_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp56_9._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_44_74._c1_poly, _pgen_rns_idx_30, degree), Coeffs(_pgen_tmp_45_75, 0, degree), Coeffs(_pgen_tmp_46_76, 0, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_44_74._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp46_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp56_9._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly(_pgen_tmp_45_75);
    Free_poly(_pgen_tmp_46_76);
  }
  Init_ciph3_down_scale(&_pgen_tmp_47_77, &_pgen_tmp_44_74);
  Rescale(&_pgen_tmp_47_77._c0_poly, &_pgen_tmp_44_74._c0_poly);
  Rescale(&_pgen_tmp_47_77._c1_poly, &_pgen_tmp_44_74._c1_poly);
  Rescale(&_pgen_tmp_47_77._c2_poly, &_pgen_tmp_44_74._c2_poly);
  Free_poly_data(&_pgen_tmp_44_74._c2_poly);
  Free_poly_data(&_pgen_tmp_44_74._c1_poly);
  Free_poly_data(&_pgen_tmp_44_74._c0_poly);
  _preg_268435462 = Relinearize(_pgen_tmp_47_77);
  Free_poly_data(&_pgen_tmp_47_77._c2_poly);
  Free_poly_data(&_pgen_tmp_47_77._c1_poly);
  Free_poly_data(&_pgen_tmp_47_77._c0_poly);
  Copy_ciph(&_relu_tmp60_10, &_preg_268435462);
  Free_poly_data(&_preg_268435462._c1_poly);
  Free_poly_data(&_preg_268435462._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_48_78, &_relu_tmp60_10, &_relu_tmp60_10);
  Encode_float(&_pgen_tmp_50_80, &_cst_432, 1, 1, Level(&_relu_tmp46_7));
  Init_ciph_up_scale_plain(&_pgen_tmp_49_79, &_relu_tmp46_7, &_pgen_tmp_50_80);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_49_79._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_49_79._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp46_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_50_80._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_49_79._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp46_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_50_80._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_50_80._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_51_81, &_pgen_tmp_49_79);
  Rescale(&_pgen_tmp_51_81._c0_poly, &_pgen_tmp_49_79._c0_poly);
  Rescale(&_pgen_tmp_51_81._c1_poly, &_pgen_tmp_49_79._c1_poly);
  Free_poly_data(&_pgen_tmp_49_79._c1_poly);
  Free_poly_data(&_pgen_tmp_49_79._c0_poly);
  Init_ciph_same_scale(&_relu_tmp68_11, &_pgen_tmp_48_78, &_pgen_tmp_51_81);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_relu_tmp68_11._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_48_78._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp60_10._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp60_10._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_48_78._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp60_10._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp60_10._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_relu_tmp68_11._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_48_78._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_51_81._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_relu_tmp68_11._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_48_78._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_51_81._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_relu_tmp60_10._c1_poly);
    Free_poly_data(&_relu_tmp60_10._c0_poly);
    Free_poly_data(&_pgen_tmp_51_81._c1_poly);
    Free_poly_data(&_pgen_tmp_51_81._c0_poly);
    Free_poly_data(&_pgen_tmp_48_78._c1_poly);
    Free_poly_data(&_pgen_tmp_48_78._c0_poly);
  }
  Init_ciph3_up_scale(&_pgen_tmp_52_82, &_relu_tmp56_9, &_relu_tmp56_9);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_52_82._c0_poly);
    _pgen_tmp_53_83 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_54_84 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_53_83, 0, degree), Coeffs(&_relu_tmp56_9._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp56_9._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_54_84, 0, degree), Coeffs(&_relu_tmp56_9._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp56_9._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_52_82._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp56_9._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp56_9._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_52_82._c1_poly, _pgen_rns_idx_30, degree), Coeffs(_pgen_tmp_53_83, 0, degree), Coeffs(_pgen_tmp_54_84, 0, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_52_82._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp56_9._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp56_9._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly(_pgen_tmp_53_83);
    Free_poly(_pgen_tmp_54_84);
  }
  Init_ciph3_down_scale(&_pgen_tmp_55_85, &_pgen_tmp_52_82);
  Rescale(&_pgen_tmp_55_85._c0_poly, &_pgen_tmp_52_82._c0_poly);
  Rescale(&_pgen_tmp_55_85._c1_poly, &_pgen_tmp_52_82._c1_poly);
  Rescale(&_pgen_tmp_55_85._c2_poly, &_pgen_tmp_52_82._c2_poly);
  Free_poly_data(&_pgen_tmp_52_82._c2_poly);
  Free_poly_data(&_pgen_tmp_52_82._c1_poly);
  Free_poly_data(&_pgen_tmp_52_82._c0_poly);
  _preg_268435463 = Relinearize(_pgen_tmp_55_85);
  Free_poly_data(&_pgen_tmp_55_85._c2_poly);
  Free_poly_data(&_pgen_tmp_55_85._c1_poly);
  Free_poly_data(&_pgen_tmp_55_85._c0_poly);
  Copy_ciph(&_relu_tmp72_12, &_preg_268435463);
  Free_poly_data(&_preg_268435463._c1_poly);
  Free_poly_data(&_preg_268435463._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_56_86, &_relu_tmp72_12, &_relu_tmp72_12);
  Encode_float(&_pgen_tmp_57_87, &_cst_433, 1, Sc_degree(&_pgen_tmp_56_86), Level(&_pgen_tmp_56_86));
  Init_ciph_same_scale_plain(&_relu_tmp78_13, &_pgen_tmp_56_86, &_pgen_tmp_57_87);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_relu_tmp78_13._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_56_86._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp72_12._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp72_12._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_56_86._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp72_12._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp72_12._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_relu_tmp78_13._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_56_86._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_57_87._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Set_coeffs(&_relu_tmp78_13._c1_poly, _pgen_rns_idx_30, degree, Coeffs(&_pgen_tmp_56_86._c1_poly, _pgen_rns_idx_30, degree));
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_relu_tmp72_12._c1_poly);
    Free_poly_data(&_relu_tmp72_12._c0_poly);
    Free_poly_data(&_pgen_tmp_57_87._poly);
    Free_poly_data(&_pgen_tmp_56_86._c1_poly);
    Free_poly_data(&_pgen_tmp_56_86._c0_poly);
  }
  Init_ciph3_up_scale(&_pgen_tmp_58_88, &_relu_tmp78_13, &_relu_tmp78_13);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_58_88._c0_poly);
    _pgen_tmp_59_89 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_60_90 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_59_89, 0, degree), Coeffs(&_relu_tmp78_13._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp78_13._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_60_90, 0, degree), Coeffs(&_relu_tmp78_13._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp78_13._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_58_88._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp78_13._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp78_13._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_58_88._c1_poly, _pgen_rns_idx_30, degree), Coeffs(_pgen_tmp_59_89, 0, degree), Coeffs(_pgen_tmp_60_90, 0, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_58_88._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp78_13._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp78_13._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly(_pgen_tmp_59_89);
    Free_poly(_pgen_tmp_60_90);
  }
  Init_ciph3_down_scale(&_pgen_tmp_61_91, &_pgen_tmp_58_88);
  Rescale(&_pgen_tmp_61_91._c0_poly, &_pgen_tmp_58_88._c0_poly);
  Rescale(&_pgen_tmp_61_91._c1_poly, &_pgen_tmp_58_88._c1_poly);
  Rescale(&_pgen_tmp_61_91._c2_poly, &_pgen_tmp_58_88._c2_poly);
  Free_poly_data(&_pgen_tmp_58_88._c2_poly);
  Free_poly_data(&_pgen_tmp_58_88._c1_poly);
  Free_poly_data(&_pgen_tmp_58_88._c0_poly);
  _preg_268435464 = Relinearize(_pgen_tmp_61_91);
  Free_poly_data(&_pgen_tmp_61_91._c2_poly);
  Free_poly_data(&_pgen_tmp_61_91._c1_poly);
  Free_poly_data(&_pgen_tmp_61_91._c0_poly);
  Copy_ciph(&_relu_tmp82_14, &_preg_268435464);
  Free_poly_data(&_preg_268435464._c1_poly);
  Free_poly_data(&_preg_268435464._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_62_92, &_relu_tmp82_14, &_relu_tmp82_14);
  Encode_float(&_pgen_tmp_63_93, &_cst_434, 1, Sc_degree(&_pgen_tmp_62_92), Level(&_pgen_tmp_62_92));
  Init_ciph_same_scale_plain(&_relu_tmp88_15, &_pgen_tmp_62_92, &_pgen_tmp_63_93);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_relu_tmp88_15._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_62_92._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp82_14._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp82_14._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_62_92._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp82_14._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp82_14._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_relu_tmp88_15._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_62_92._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_63_93._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Set_coeffs(&_relu_tmp88_15._c1_poly, _pgen_rns_idx_30, degree, Coeffs(&_pgen_tmp_62_92._c1_poly, _pgen_rns_idx_30, degree));
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_relu_tmp82_14._c1_poly);
    Free_poly_data(&_relu_tmp82_14._c0_poly);
    Free_poly_data(&_pgen_tmp_63_93._poly);
    Free_poly_data(&_pgen_tmp_62_92._c1_poly);
    Free_poly_data(&_pgen_tmp_62_92._c0_poly);
  }
  Encode_double(&_pgen_tmp_68_98, &_cst_435, 1, 1, Level(&_relu_tmp46_7));
  Init_ciph_up_scale_plain(&_pgen_tmp_67_97, &_relu_tmp46_7, &_pgen_tmp_68_98);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_67_97._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_67_97._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp46_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_68_98._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_67_97._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp46_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_68_98._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_68_98._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_69_99, &_pgen_tmp_67_97);
  Rescale(&_pgen_tmp_69_99._c0_poly, &_pgen_tmp_67_97._c0_poly);
  Rescale(&_pgen_tmp_69_99._c1_poly, &_pgen_tmp_67_97._c1_poly);
  Free_poly_data(&_pgen_tmp_67_97._c1_poly);
  Free_poly_data(&_pgen_tmp_67_97._c0_poly);
  Init_ciph3_up_scale(&_pgen_tmp_66_96, &_relu_tmp56_9, &_pgen_tmp_69_99);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_66_96._c0_poly);
    _pgen_tmp_70_100 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_71_101 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_70_100, 0, degree), Coeffs(&_relu_tmp56_9._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_69_99._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_71_101, 0, degree), Coeffs(&_relu_tmp56_9._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_69_99._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_66_96._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp56_9._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_69_99._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_66_96._c1_poly, _pgen_rns_idx_30, degree), Coeffs(_pgen_tmp_70_100, 0, degree), Coeffs(_pgen_tmp_71_101, 0, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_66_96._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp56_9._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_69_99._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_69_99._c1_poly);
    Free_poly_data(&_pgen_tmp_69_99._c0_poly);
    Free_poly_data(&_relu_tmp56_9._c1_poly);
    Free_poly_data(&_relu_tmp56_9._c0_poly);
    Free_poly(_pgen_tmp_70_100);
    Free_poly(_pgen_tmp_71_101);
  }
  Init_ciph3_down_scale(&_pgen_tmp_72_102, &_pgen_tmp_66_96);
  Rescale(&_pgen_tmp_72_102._c0_poly, &_pgen_tmp_66_96._c0_poly);
  Rescale(&_pgen_tmp_72_102._c1_poly, &_pgen_tmp_66_96._c1_poly);
  Rescale(&_pgen_tmp_72_102._c2_poly, &_pgen_tmp_66_96._c2_poly);
  Free_poly_data(&_pgen_tmp_66_96._c2_poly);
  Free_poly_data(&_pgen_tmp_66_96._c1_poly);
  Free_poly_data(&_pgen_tmp_66_96._c0_poly);
  _preg_268435465 = Relinearize(_pgen_tmp_72_102);
  Free_poly_data(&_pgen_tmp_72_102._c2_poly);
  Free_poly_data(&_pgen_tmp_72_102._c1_poly);
  Free_poly_data(&_pgen_tmp_72_102._c0_poly);
  Encode_double(&_pgen_tmp_74_104, &_cst_436, 1, 1, Level(&_relu_tmp46_7));
  Init_ciph_up_scale_plain(&_pgen_tmp_73_103, &_relu_tmp46_7, &_pgen_tmp_74_104);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_73_103._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_73_103._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp46_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_74_104._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_73_103._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp46_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_74_104._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_74_104._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_75_105, &_pgen_tmp_73_103);
  Rescale(&_pgen_tmp_75_105._c0_poly, &_pgen_tmp_73_103._c0_poly);
  Rescale(&_pgen_tmp_75_105._c1_poly, &_pgen_tmp_73_103._c1_poly);
  Free_poly_data(&_pgen_tmp_73_103._c1_poly);
  Free_poly_data(&_pgen_tmp_73_103._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_76_106, &_preg_268435465, &_pgen_tmp_75_105);
  Init_ciph3_up_scale(&_pgen_tmp_65_95, &_relu_tmp78_13, &_pgen_tmp_76_106);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_65_95._c0_poly);
    _pgen_tmp_77_107 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_78_108 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_76_106._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435465._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_75_105._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_76_106._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435465._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_75_105._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_77_107, 0, degree), Coeffs(&_relu_tmp78_13._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_76_106._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_78_108, 0, degree), Coeffs(&_relu_tmp78_13._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_76_106._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_65_95._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp78_13._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_76_106._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_65_95._c1_poly, _pgen_rns_idx_30, degree), Coeffs(_pgen_tmp_77_107, 0, degree), Coeffs(_pgen_tmp_78_108, 0, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_65_95._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp78_13._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_76_106._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_75_105._c1_poly);
    Free_poly_data(&_pgen_tmp_75_105._c0_poly);
    Free_poly_data(&_preg_268435465._c1_poly);
    Free_poly_data(&_preg_268435465._c0_poly);
    Free_poly_data(&_pgen_tmp_76_106._c1_poly);
    Free_poly_data(&_pgen_tmp_76_106._c0_poly);
    Free_poly(_pgen_tmp_77_107);
    Free_poly(_pgen_tmp_78_108);
  }
  Init_ciph3_down_scale(&_pgen_tmp_79_109, &_pgen_tmp_65_95);
  Rescale(&_pgen_tmp_79_109._c0_poly, &_pgen_tmp_65_95._c0_poly);
  Rescale(&_pgen_tmp_79_109._c1_poly, &_pgen_tmp_65_95._c1_poly);
  Rescale(&_pgen_tmp_79_109._c2_poly, &_pgen_tmp_65_95._c2_poly);
  Free_poly_data(&_pgen_tmp_65_95._c2_poly);
  Free_poly_data(&_pgen_tmp_65_95._c1_poly);
  Free_poly_data(&_pgen_tmp_65_95._c0_poly);
  _preg_268435466 = Relinearize(_pgen_tmp_79_109);
  Free_poly_data(&_pgen_tmp_79_109._c2_poly);
  Free_poly_data(&_pgen_tmp_79_109._c1_poly);
  Free_poly_data(&_pgen_tmp_79_109._c0_poly);
  Encode_double(&_pgen_tmp_81_111, &_cst_438, 1, 1, Level(&_relu_tmp68_11));
  Init_ciph_up_scale_plain(&_pgen_tmp_80_110, &_relu_tmp68_11, &_pgen_tmp_81_111);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_80_110._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_80_110._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp68_11._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_81_111._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_80_110._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp68_11._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_81_111._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_81_111._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_82_112, &_pgen_tmp_80_110);
  Rescale(&_pgen_tmp_82_112._c0_poly, &_pgen_tmp_80_110._c0_poly);
  Rescale(&_pgen_tmp_82_112._c1_poly, &_pgen_tmp_80_110._c1_poly);
  Free_poly_data(&_pgen_tmp_80_110._c1_poly);
  Free_poly_data(&_pgen_tmp_80_110._c0_poly);
  Encode_double(&_pgen_tmp_84_114, &_cst_437, 1, 1, Level(&_relu_tmp46_7));
  Init_ciph_up_scale_plain(&_pgen_tmp_83_113, &_relu_tmp46_7, &_pgen_tmp_84_114);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_83_113._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_83_113._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp46_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_84_114._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_83_113._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp46_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_84_114._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_84_114._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_85_115, &_pgen_tmp_83_113);
  Rescale(&_pgen_tmp_85_115._c0_poly, &_pgen_tmp_83_113._c0_poly);
  Rescale(&_pgen_tmp_85_115._c1_poly, &_pgen_tmp_83_113._c1_poly);
  Free_poly_data(&_pgen_tmp_83_113._c1_poly);
  Free_poly_data(&_pgen_tmp_83_113._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_86_116, &_pgen_tmp_82_112, &_pgen_tmp_85_115);
  Init_ciph_same_scale(&_pgen_tmp_87_117, &_preg_268435466, &_pgen_tmp_86_116);
  Init_ciph3_up_scale(&_pgen_tmp_64_94, &_relu_tmp88_15, &_pgen_tmp_87_117);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_64_94._c0_poly);
    _pgen_tmp_88_118 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_89_119 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_86_116._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_82_112._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_85_115._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_86_116._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_82_112._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_85_115._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_87_117._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435466._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_86_116._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_87_117._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435466._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_86_116._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_88_118, 0, degree), Coeffs(&_relu_tmp88_15._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_87_117._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_89_119, 0, degree), Coeffs(&_relu_tmp88_15._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_87_117._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_64_94._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp88_15._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_87_117._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_64_94._c1_poly, _pgen_rns_idx_30, degree), Coeffs(_pgen_tmp_88_118, 0, degree), Coeffs(_pgen_tmp_89_119, 0, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_64_94._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp88_15._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_87_117._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_85_115._c1_poly);
    Free_poly_data(&_pgen_tmp_85_115._c0_poly);
    Free_poly_data(&_pgen_tmp_82_112._c1_poly);
    Free_poly_data(&_pgen_tmp_82_112._c0_poly);
    Free_poly_data(&_pgen_tmp_86_116._c1_poly);
    Free_poly_data(&_pgen_tmp_86_116._c0_poly);
    Free_poly_data(&_preg_268435466._c1_poly);
    Free_poly_data(&_preg_268435466._c0_poly);
    Free_poly_data(&_pgen_tmp_87_117._c1_poly);
    Free_poly_data(&_pgen_tmp_87_117._c0_poly);
    Free_poly_data(&_relu_tmp88_15._c1_poly);
    Free_poly_data(&_relu_tmp88_15._c0_poly);
    Free_poly(_pgen_tmp_88_118);
    Free_poly(_pgen_tmp_89_119);
  }
  Init_ciph3_down_scale(&_pgen_tmp_90_120, &_pgen_tmp_64_94);
  Rescale(&_pgen_tmp_90_120._c0_poly, &_pgen_tmp_64_94._c0_poly);
  Rescale(&_pgen_tmp_90_120._c1_poly, &_pgen_tmp_64_94._c1_poly);
  Rescale(&_pgen_tmp_90_120._c2_poly, &_pgen_tmp_64_94._c2_poly);
  Free_poly_data(&_pgen_tmp_64_94._c2_poly);
  Free_poly_data(&_pgen_tmp_64_94._c1_poly);
  Free_poly_data(&_pgen_tmp_64_94._c0_poly);
  _preg_268435467 = Relinearize(_pgen_tmp_90_120);
  Free_poly_data(&_pgen_tmp_90_120._c2_poly);
  Free_poly_data(&_pgen_tmp_90_120._c1_poly);
  Free_poly_data(&_pgen_tmp_90_120._c0_poly);
  Encode_double(&_pgen_tmp_93_123, &_cst_440, 1, 1, Level(&_relu_tmp68_11));
  Init_ciph_up_scale_plain(&_pgen_tmp_92_122, &_relu_tmp68_11, &_pgen_tmp_93_123);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_92_122._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_92_122._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp68_11._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_93_123._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_92_122._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp68_11._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_93_123._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_93_123._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_94_124, &_pgen_tmp_92_122);
  Rescale(&_pgen_tmp_94_124._c0_poly, &_pgen_tmp_92_122._c0_poly);
  Rescale(&_pgen_tmp_94_124._c1_poly, &_pgen_tmp_92_122._c1_poly);
  Free_poly_data(&_pgen_tmp_92_122._c1_poly);
  Free_poly_data(&_pgen_tmp_92_122._c0_poly);
  Encode_double(&_pgen_tmp_96_126, &_cst_439, 1, 1, Level(&_relu_tmp46_7));
  Init_ciph_up_scale_plain(&_pgen_tmp_95_125, &_relu_tmp46_7, &_pgen_tmp_96_126);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_95_125._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_95_125._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp46_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_96_126._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_95_125._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp46_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_96_126._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_96_126._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_97_127, &_pgen_tmp_95_125);
  Rescale(&_pgen_tmp_97_127._c0_poly, &_pgen_tmp_95_125._c0_poly);
  Rescale(&_pgen_tmp_97_127._c1_poly, &_pgen_tmp_95_125._c1_poly);
  Free_poly_data(&_pgen_tmp_95_125._c1_poly);
  Free_poly_data(&_pgen_tmp_95_125._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_98_128, &_pgen_tmp_94_124, &_pgen_tmp_97_127);
  Init_ciph3_up_scale(&_pgen_tmp_91_121, &_relu_tmp78_13, &_pgen_tmp_98_128);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_91_121._c0_poly);
    _pgen_tmp_99_129 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_100_130 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_98_128._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_94_124._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_97_127._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_98_128._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_94_124._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_97_127._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_99_129, 0, degree), Coeffs(&_relu_tmp78_13._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_98_128._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_100_130, 0, degree), Coeffs(&_relu_tmp78_13._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_98_128._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_91_121._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp78_13._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_98_128._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_91_121._c1_poly, _pgen_rns_idx_30, degree), Coeffs(_pgen_tmp_99_129, 0, degree), Coeffs(_pgen_tmp_100_130, 0, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_91_121._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp78_13._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_98_128._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_97_127._c1_poly);
    Free_poly_data(&_pgen_tmp_97_127._c0_poly);
    Free_poly_data(&_pgen_tmp_94_124._c1_poly);
    Free_poly_data(&_pgen_tmp_94_124._c0_poly);
    Free_poly_data(&_pgen_tmp_98_128._c1_poly);
    Free_poly_data(&_pgen_tmp_98_128._c0_poly);
    Free_poly_data(&_relu_tmp78_13._c1_poly);
    Free_poly_data(&_relu_tmp78_13._c0_poly);
    Free_poly(_pgen_tmp_99_129);
    Free_poly(_pgen_tmp_100_130);
  }
  Init_ciph3_down_scale(&_pgen_tmp_101_131, &_pgen_tmp_91_121);
  Rescale(&_pgen_tmp_101_131._c0_poly, &_pgen_tmp_91_121._c0_poly);
  Rescale(&_pgen_tmp_101_131._c1_poly, &_pgen_tmp_91_121._c1_poly);
  Rescale(&_pgen_tmp_101_131._c2_poly, &_pgen_tmp_91_121._c2_poly);
  Free_poly_data(&_pgen_tmp_91_121._c2_poly);
  Free_poly_data(&_pgen_tmp_91_121._c1_poly);
  Free_poly_data(&_pgen_tmp_91_121._c0_poly);
  _preg_268435468 = Relinearize(_pgen_tmp_101_131);
  Free_poly_data(&_pgen_tmp_101_131._c2_poly);
  Free_poly_data(&_pgen_tmp_101_131._c1_poly);
  Free_poly_data(&_pgen_tmp_101_131._c0_poly);
  Encode_double(&_pgen_tmp_103_133, &_cst_442, 1, 1, Level(&_relu_tmp68_11));
  Init_ciph_up_scale_plain(&_pgen_tmp_102_132, &_relu_tmp68_11, &_pgen_tmp_103_133);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_102_132._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_102_132._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp68_11._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_103_133._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_102_132._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp68_11._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_103_133._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_103_133._poly);
    Free_poly_data(&_relu_tmp68_11._c1_poly);
    Free_poly_data(&_relu_tmp68_11._c0_poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_104_134, &_pgen_tmp_102_132);
  Rescale(&_pgen_tmp_104_134._c0_poly, &_pgen_tmp_102_132._c0_poly);
  Rescale(&_pgen_tmp_104_134._c1_poly, &_pgen_tmp_102_132._c1_poly);
  Free_poly_data(&_pgen_tmp_102_132._c1_poly);
  Free_poly_data(&_pgen_tmp_102_132._c0_poly);
  Encode_double(&_pgen_tmp_106_136, &_cst_441, 1, 1, Level(&_relu_tmp46_7));
  Init_ciph_up_scale_plain(&_pgen_tmp_105_135, &_relu_tmp46_7, &_pgen_tmp_106_136);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_105_135._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_105_135._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp46_7._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_106_136._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_105_135._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp46_7._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_106_136._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_106_136._poly);
    Free_poly_data(&_relu_tmp46_7._c1_poly);
    Free_poly_data(&_relu_tmp46_7._c0_poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_107_137, &_pgen_tmp_105_135);
  Rescale(&_pgen_tmp_107_137._c0_poly, &_pgen_tmp_105_135._c0_poly);
  Rescale(&_pgen_tmp_107_137._c1_poly, &_pgen_tmp_105_135._c1_poly);
  Free_poly_data(&_pgen_tmp_105_135._c1_poly);
  Free_poly_data(&_pgen_tmp_105_135._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_108_138, &_pgen_tmp_104_134, &_pgen_tmp_107_137);
  Init_ciph_same_scale(&_pgen_tmp_109_139, &_preg_268435468, &_pgen_tmp_108_138);
  Init_ciph_same_scale(&_relu_tmp128_16, &_preg_268435467, &_pgen_tmp_109_139);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_relu_tmp128_16._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_108_138._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_104_134._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_107_137._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_108_138._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_104_134._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_107_137._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_109_139._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435468._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_108_138._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_109_139._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435468._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_108_138._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_relu_tmp128_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435467._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_109_139._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_relu_tmp128_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435467._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_109_139._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_107_137._c1_poly);
    Free_poly_data(&_pgen_tmp_107_137._c0_poly);
    Free_poly_data(&_pgen_tmp_104_134._c1_poly);
    Free_poly_data(&_pgen_tmp_104_134._c0_poly);
    Free_poly_data(&_pgen_tmp_108_138._c1_poly);
    Free_poly_data(&_pgen_tmp_108_138._c0_poly);
    Free_poly_data(&_preg_268435468._c1_poly);
    Free_poly_data(&_preg_268435468._c0_poly);
    Free_poly_data(&_pgen_tmp_109_139._c1_poly);
    Free_poly_data(&_pgen_tmp_109_139._c0_poly);
    Free_poly_data(&_preg_268435467._c1_poly);
    Free_poly_data(&_preg_268435467._c0_poly);
  }
  Init_ciph3_up_scale(&_pgen_tmp_110_140, &_relu_tmp128_16, &_relu_tmp128_16);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_110_140._c0_poly);
    _pgen_tmp_111_141 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_112_142 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_111_141, 0, degree), Coeffs(&_relu_tmp128_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp128_16._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_112_142, 0, degree), Coeffs(&_relu_tmp128_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp128_16._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_110_140._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp128_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp128_16._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_110_140._c1_poly, _pgen_rns_idx_30, degree), Coeffs(_pgen_tmp_111_141, 0, degree), Coeffs(_pgen_tmp_112_142, 0, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_110_140._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp128_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp128_16._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly(_pgen_tmp_111_141);
    Free_poly(_pgen_tmp_112_142);
  }
  Init_ciph3_down_scale(&_pgen_tmp_113_143, &_pgen_tmp_110_140);
  Rescale(&_pgen_tmp_113_143._c0_poly, &_pgen_tmp_110_140._c0_poly);
  Rescale(&_pgen_tmp_113_143._c1_poly, &_pgen_tmp_110_140._c1_poly);
  Rescale(&_pgen_tmp_113_143._c2_poly, &_pgen_tmp_110_140._c2_poly);
  Free_poly_data(&_pgen_tmp_110_140._c2_poly);
  Free_poly_data(&_pgen_tmp_110_140._c1_poly);
  Free_poly_data(&_pgen_tmp_110_140._c0_poly);
  _preg_268435469 = Relinearize(_pgen_tmp_113_143);
  Free_poly_data(&_pgen_tmp_113_143._c2_poly);
  Free_poly_data(&_pgen_tmp_113_143._c1_poly);
  Free_poly_data(&_pgen_tmp_113_143._c0_poly);
  Copy_ciph(&_relu_tmp132_17, &_preg_268435469);
  Free_poly_data(&_preg_268435469._c1_poly);
  Free_poly_data(&_preg_268435469._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_114_144, &_relu_tmp132_17, &_relu_tmp132_17);
  Encode_float(&_pgen_tmp_115_145, &_cst_443, 1, Sc_degree(&_pgen_tmp_114_144), Level(&_pgen_tmp_114_144));
  Init_ciph_same_scale_plain(&_relu_tmp138_18, &_pgen_tmp_114_144, &_pgen_tmp_115_145);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_relu_tmp138_18._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_114_144._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp132_17._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp132_17._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_114_144._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp132_17._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp132_17._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_relu_tmp138_18._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_114_144._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_115_145._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Set_coeffs(&_relu_tmp138_18._c1_poly, _pgen_rns_idx_30, degree, Coeffs(&_pgen_tmp_114_144._c1_poly, _pgen_rns_idx_30, degree));
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_relu_tmp132_17._c1_poly);
    Free_poly_data(&_relu_tmp132_17._c0_poly);
    Free_poly_data(&_pgen_tmp_115_145._poly);
    Free_poly_data(&_pgen_tmp_114_144._c1_poly);
    Free_poly_data(&_pgen_tmp_114_144._c0_poly);
  }
  Init_ciph3_up_scale(&_pgen_tmp_116_146, &_relu_tmp128_16, &_relu_tmp138_18);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_116_146._c0_poly);
    _pgen_tmp_117_147 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_118_148 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_117_147, 0, degree), Coeffs(&_relu_tmp128_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp138_18._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_118_148, 0, degree), Coeffs(&_relu_tmp128_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp138_18._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_116_146._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp128_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp138_18._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_116_146._c1_poly, _pgen_rns_idx_30, degree), Coeffs(_pgen_tmp_117_147, 0, degree), Coeffs(_pgen_tmp_118_148, 0, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_116_146._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp128_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp138_18._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly(_pgen_tmp_117_147);
    Free_poly(_pgen_tmp_118_148);
  }
  Init_ciph3_down_scale(&_pgen_tmp_119_149, &_pgen_tmp_116_146);
  Rescale(&_pgen_tmp_119_149._c0_poly, &_pgen_tmp_116_146._c0_poly);
  Rescale(&_pgen_tmp_119_149._c1_poly, &_pgen_tmp_116_146._c1_poly);
  Rescale(&_pgen_tmp_119_149._c2_poly, &_pgen_tmp_116_146._c2_poly);
  Free_poly_data(&_pgen_tmp_116_146._c2_poly);
  Free_poly_data(&_pgen_tmp_116_146._c1_poly);
  Free_poly_data(&_pgen_tmp_116_146._c0_poly);
  _preg_268435470 = Relinearize(_pgen_tmp_119_149);
  Free_poly_data(&_pgen_tmp_119_149._c2_poly);
  Free_poly_data(&_pgen_tmp_119_149._c1_poly);
  Free_poly_data(&_pgen_tmp_119_149._c0_poly);
  Copy_ciph(&_relu_tmp142_19, &_preg_268435470);
  Free_poly_data(&_preg_268435470._c1_poly);
  Free_poly_data(&_preg_268435470._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_120_150, &_relu_tmp142_19, &_relu_tmp142_19);
  Encode_float(&_pgen_tmp_122_152, &_cst_444, 1, 1, Level(&_relu_tmp128_16));
  Init_ciph_up_scale_plain(&_pgen_tmp_121_151, &_relu_tmp128_16, &_pgen_tmp_122_152);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_121_151._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_121_151._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp128_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_122_152._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_121_151._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp128_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_122_152._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_122_152._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_123_153, &_pgen_tmp_121_151);
  Rescale(&_pgen_tmp_123_153._c0_poly, &_pgen_tmp_121_151._c0_poly);
  Rescale(&_pgen_tmp_123_153._c1_poly, &_pgen_tmp_121_151._c1_poly);
  Free_poly_data(&_pgen_tmp_121_151._c1_poly);
  Free_poly_data(&_pgen_tmp_121_151._c0_poly);
  Init_ciph_same_scale(&_relu_tmp150_20, &_pgen_tmp_120_150, &_pgen_tmp_123_153);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_relu_tmp150_20._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_120_150._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp142_19._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp142_19._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_120_150._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp142_19._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp142_19._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_relu_tmp150_20._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_120_150._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_123_153._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_relu_tmp150_20._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_120_150._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_123_153._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_relu_tmp142_19._c1_poly);
    Free_poly_data(&_relu_tmp142_19._c0_poly);
    Free_poly_data(&_pgen_tmp_123_153._c1_poly);
    Free_poly_data(&_pgen_tmp_123_153._c0_poly);
    Free_poly_data(&_pgen_tmp_120_150._c1_poly);
    Free_poly_data(&_pgen_tmp_120_150._c0_poly);
  }
  Init_ciph3_up_scale(&_pgen_tmp_124_154, &_relu_tmp138_18, &_relu_tmp138_18);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_124_154._c0_poly);
    _pgen_tmp_125_155 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_126_156 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_125_155, 0, degree), Coeffs(&_relu_tmp138_18._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp138_18._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_126_156, 0, degree), Coeffs(&_relu_tmp138_18._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp138_18._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_124_154._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp138_18._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp138_18._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_124_154._c1_poly, _pgen_rns_idx_30, degree), Coeffs(_pgen_tmp_125_155, 0, degree), Coeffs(_pgen_tmp_126_156, 0, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_124_154._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp138_18._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp138_18._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_relu_tmp138_18._c1_poly);
    Free_poly_data(&_relu_tmp138_18._c0_poly);
    Free_poly(_pgen_tmp_125_155);
    Free_poly(_pgen_tmp_126_156);
  }
  Init_ciph3_down_scale(&_pgen_tmp_127_157, &_pgen_tmp_124_154);
  Rescale(&_pgen_tmp_127_157._c0_poly, &_pgen_tmp_124_154._c0_poly);
  Rescale(&_pgen_tmp_127_157._c1_poly, &_pgen_tmp_124_154._c1_poly);
  Rescale(&_pgen_tmp_127_157._c2_poly, &_pgen_tmp_124_154._c2_poly);
  Free_poly_data(&_pgen_tmp_124_154._c2_poly);
  Free_poly_data(&_pgen_tmp_124_154._c1_poly);
  Free_poly_data(&_pgen_tmp_124_154._c0_poly);
  _preg_268435471 = Relinearize(_pgen_tmp_127_157);
  Free_poly_data(&_pgen_tmp_127_157._c2_poly);
  Free_poly_data(&_pgen_tmp_127_157._c1_poly);
  Free_poly_data(&_pgen_tmp_127_157._c0_poly);
  Copy_ciph(&_relu_tmp154_21, &_preg_268435471);
  Free_poly_data(&_preg_268435471._c1_poly);
  Free_poly_data(&_preg_268435471._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_128_158, &_relu_tmp154_21, &_relu_tmp154_21);
  Encode_float(&_pgen_tmp_129_159, &_cst_445, 1, Sc_degree(&_pgen_tmp_128_158), Level(&_pgen_tmp_128_158));
  Init_ciph_same_scale_plain(&_relu_tmp160_22, &_pgen_tmp_128_158, &_pgen_tmp_129_159);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_relu_tmp160_22._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_128_158._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp154_21._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp154_21._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_128_158._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp154_21._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp154_21._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_relu_tmp160_22._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_128_158._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_129_159._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Set_coeffs(&_relu_tmp160_22._c1_poly, _pgen_rns_idx_30, degree, Coeffs(&_pgen_tmp_128_158._c1_poly, _pgen_rns_idx_30, degree));
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_relu_tmp154_21._c1_poly);
    Free_poly_data(&_relu_tmp154_21._c0_poly);
    Free_poly_data(&_pgen_tmp_129_159._poly);
    Free_poly_data(&_pgen_tmp_128_158._c1_poly);
    Free_poly_data(&_pgen_tmp_128_158._c0_poly);
  }
  Init_ciph3_up_scale(&_pgen_tmp_130_160, &_relu_tmp160_22, &_relu_tmp160_22);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_130_160._c0_poly);
    _pgen_tmp_131_161 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_132_162 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_131_161, 0, degree), Coeffs(&_relu_tmp160_22._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp160_22._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_132_162, 0, degree), Coeffs(&_relu_tmp160_22._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp160_22._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_130_160._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp160_22._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp160_22._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_130_160._c1_poly, _pgen_rns_idx_30, degree), Coeffs(_pgen_tmp_131_161, 0, degree), Coeffs(_pgen_tmp_132_162, 0, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_130_160._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp160_22._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp160_22._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly(_pgen_tmp_131_161);
    Free_poly(_pgen_tmp_132_162);
  }
  Init_ciph3_down_scale(&_pgen_tmp_133_163, &_pgen_tmp_130_160);
  Rescale(&_pgen_tmp_133_163._c0_poly, &_pgen_tmp_130_160._c0_poly);
  Rescale(&_pgen_tmp_133_163._c1_poly, &_pgen_tmp_130_160._c1_poly);
  Rescale(&_pgen_tmp_133_163._c2_poly, &_pgen_tmp_130_160._c2_poly);
  Free_poly_data(&_pgen_tmp_130_160._c2_poly);
  Free_poly_data(&_pgen_tmp_130_160._c1_poly);
  Free_poly_data(&_pgen_tmp_130_160._c0_poly);
  _preg_268435472 = Relinearize(_pgen_tmp_133_163);
  Free_poly_data(&_pgen_tmp_133_163._c2_poly);
  Free_poly_data(&_pgen_tmp_133_163._c1_poly);
  Free_poly_data(&_pgen_tmp_133_163._c0_poly);
  Copy_ciph(&_relu_tmp164_23, &_preg_268435472);
  Free_poly_data(&_preg_268435472._c1_poly);
  Free_poly_data(&_preg_268435472._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_134_164, &_relu_tmp164_23, &_relu_tmp164_23);
  Encode_float(&_pgen_tmp_135_165, &_cst_446, 1, Sc_degree(&_pgen_tmp_134_164), Level(&_pgen_tmp_134_164));
  Init_ciph_same_scale_plain(&_relu_tmp170_24, &_pgen_tmp_134_164, &_pgen_tmp_135_165);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_relu_tmp170_24._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_134_164._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp164_23._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp164_23._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_134_164._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp164_23._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp164_23._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_relu_tmp170_24._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_134_164._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_135_165._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Set_coeffs(&_relu_tmp170_24._c1_poly, _pgen_rns_idx_30, degree, Coeffs(&_pgen_tmp_134_164._c1_poly, _pgen_rns_idx_30, degree));
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_relu_tmp164_23._c1_poly);
    Free_poly_data(&_relu_tmp164_23._c0_poly);
    Free_poly_data(&_pgen_tmp_135_165._poly);
    Free_poly_data(&_pgen_tmp_134_164._c1_poly);
    Free_poly_data(&_pgen_tmp_134_164._c0_poly);
  }
  Encode_float_mask(&_pgen_tmp_140_170, 0.0001111119162030077897477, mask_len_2, 1, Level(&input_ct0_0));
  Init_ciph_up_scale_plain(&_pgen_tmp_139_169, &input_ct0_0, &_pgen_tmp_140_170);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_139_169._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_139_169._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_140_170._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_139_169._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_140_170._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_140_170._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_141_171, &_pgen_tmp_139_169);
  Rescale(&_pgen_tmp_141_171._c0_poly, &_pgen_tmp_139_169._c0_poly);
  Rescale(&_pgen_tmp_141_171._c1_poly, &_pgen_tmp_139_169._c1_poly);
  Free_poly_data(&_pgen_tmp_139_169._c1_poly);
  Free_poly_data(&_pgen_tmp_139_169._c0_poly);
  Init_ciph3_up_scale(&_pgen_tmp_138_168, &_relu_tmp128_16, &_pgen_tmp_141_171);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_138_168._c0_poly);
    _pgen_tmp_142_172 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_143_173 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_142_172, 0, degree), Coeffs(&_relu_tmp128_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_141_171._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_143_173, 0, degree), Coeffs(&_relu_tmp128_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_141_171._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_138_168._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp128_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_141_171._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_138_168._c1_poly, _pgen_rns_idx_30, degree), Coeffs(_pgen_tmp_142_172, 0, degree), Coeffs(_pgen_tmp_143_173, 0, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_138_168._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp128_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_141_171._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_141_171._c1_poly);
    Free_poly_data(&_pgen_tmp_141_171._c0_poly);
    Free_poly(_pgen_tmp_142_172);
    Free_poly(_pgen_tmp_143_173);
  }
  Init_ciph3_down_scale(&_pgen_tmp_144_174, &_pgen_tmp_138_168);
  Rescale(&_pgen_tmp_144_174._c0_poly, &_pgen_tmp_138_168._c0_poly);
  Rescale(&_pgen_tmp_144_174._c1_poly, &_pgen_tmp_138_168._c1_poly);
  Rescale(&_pgen_tmp_144_174._c2_poly, &_pgen_tmp_138_168._c2_poly);
  Free_poly_data(&_pgen_tmp_138_168._c2_poly);
  Free_poly_data(&_pgen_tmp_138_168._c1_poly);
  Free_poly_data(&_pgen_tmp_138_168._c0_poly);
  _preg_268435473 = Relinearize(_pgen_tmp_144_174);
  Free_poly_data(&_pgen_tmp_144_174._c2_poly);
  Free_poly_data(&_pgen_tmp_144_174._c1_poly);
  Free_poly_data(&_pgen_tmp_144_174._c0_poly);
  Init_ciph3_up_scale(&_pgen_tmp_137_167, &_relu_tmp160_22, &_preg_268435473);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_137_167._c0_poly);
    _pgen_tmp_145_175 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_146_176 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_145_175, 0, degree), Coeffs(&_relu_tmp160_22._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435473._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_146_176, 0, degree), Coeffs(&_relu_tmp160_22._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435473._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_137_167._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp160_22._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435473._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_137_167._c1_poly, _pgen_rns_idx_30, degree), Coeffs(_pgen_tmp_145_175, 0, degree), Coeffs(_pgen_tmp_146_176, 0, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_137_167._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp160_22._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435473._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_preg_268435473._c1_poly);
    Free_poly_data(&_preg_268435473._c0_poly);
    Free_poly(_pgen_tmp_145_175);
    Free_poly(_pgen_tmp_146_176);
  }
  Init_ciph3_down_scale(&_pgen_tmp_147_177, &_pgen_tmp_137_167);
  Rescale(&_pgen_tmp_147_177._c0_poly, &_pgen_tmp_137_167._c0_poly);
  Rescale(&_pgen_tmp_147_177._c1_poly, &_pgen_tmp_137_167._c1_poly);
  Rescale(&_pgen_tmp_147_177._c2_poly, &_pgen_tmp_137_167._c2_poly);
  Free_poly_data(&_pgen_tmp_137_167._c2_poly);
  Free_poly_data(&_pgen_tmp_137_167._c1_poly);
  Free_poly_data(&_pgen_tmp_137_167._c0_poly);
  _preg_268435474 = Relinearize(_pgen_tmp_147_177);
  Free_poly_data(&_pgen_tmp_147_177._c2_poly);
  Free_poly_data(&_pgen_tmp_147_177._c1_poly);
  Free_poly_data(&_pgen_tmp_147_177._c0_poly);
  Encode_float_mask(&_pgen_tmp_150_180, -0.0009371069270347270278171, mask_len_2, 1, Level(&input_ct0_0));
  Init_ciph_up_scale_plain(&_pgen_tmp_149_179, &input_ct0_0, &_pgen_tmp_150_180);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_149_179._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_149_179._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_150_180._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_149_179._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_150_180._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_150_180._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_151_181, &_pgen_tmp_149_179);
  Rescale(&_pgen_tmp_151_181._c0_poly, &_pgen_tmp_149_179._c0_poly);
  Rescale(&_pgen_tmp_151_181._c1_poly, &_pgen_tmp_149_179._c1_poly);
  Free_poly_data(&_pgen_tmp_149_179._c1_poly);
  Free_poly_data(&_pgen_tmp_149_179._c0_poly);
  Init_ciph3_up_scale(&_pgen_tmp_148_178, &_relu_tmp150_20, &_pgen_tmp_151_181);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_148_178._c0_poly);
    _pgen_tmp_152_182 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_153_183 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_152_182, 0, degree), Coeffs(&_relu_tmp150_20._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_151_181._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_153_183, 0, degree), Coeffs(&_relu_tmp150_20._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_151_181._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_148_178._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp150_20._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_151_181._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_148_178._c1_poly, _pgen_rns_idx_30, degree), Coeffs(_pgen_tmp_152_182, 0, degree), Coeffs(_pgen_tmp_153_183, 0, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_148_178._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp150_20._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_151_181._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_151_181._c1_poly);
    Free_poly_data(&_pgen_tmp_151_181._c0_poly);
    Free_poly(_pgen_tmp_152_182);
    Free_poly(_pgen_tmp_153_183);
  }
  Init_ciph3_down_scale(&_pgen_tmp_154_184, &_pgen_tmp_148_178);
  Rescale(&_pgen_tmp_154_184._c0_poly, &_pgen_tmp_148_178._c0_poly);
  Rescale(&_pgen_tmp_154_184._c1_poly, &_pgen_tmp_148_178._c1_poly);
  Rescale(&_pgen_tmp_154_184._c2_poly, &_pgen_tmp_148_178._c2_poly);
  Free_poly_data(&_pgen_tmp_148_178._c2_poly);
  Free_poly_data(&_pgen_tmp_148_178._c1_poly);
  Free_poly_data(&_pgen_tmp_148_178._c0_poly);
  _preg_268435475 = Relinearize(_pgen_tmp_154_184);
  Free_poly_data(&_pgen_tmp_154_184._c2_poly);
  Free_poly_data(&_pgen_tmp_154_184._c1_poly);
  Free_poly_data(&_pgen_tmp_154_184._c0_poly);
  Encode_float_mask(&_pgen_tmp_157_187, 0.006507995190210730772351, mask_len_2, 1, Level(&input_ct0_0));
  Init_ciph_up_scale_plain(&_pgen_tmp_156_186, &input_ct0_0, &_pgen_tmp_157_187);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_156_186._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_156_186._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_157_187._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_156_186._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_157_187._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_157_187._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_158_188, &_pgen_tmp_156_186);
  Rescale(&_pgen_tmp_158_188._c0_poly, &_pgen_tmp_156_186._c0_poly);
  Rescale(&_pgen_tmp_158_188._c1_poly, &_pgen_tmp_156_186._c1_poly);
  Free_poly_data(&_pgen_tmp_156_186._c1_poly);
  Free_poly_data(&_pgen_tmp_156_186._c0_poly);
  Init_ciph3_up_scale(&_pgen_tmp_155_185, &_relu_tmp128_16, &_pgen_tmp_158_188);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_155_185._c0_poly);
    _pgen_tmp_159_189 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_160_190 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_159_189, 0, degree), Coeffs(&_relu_tmp128_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_158_188._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_160_190, 0, degree), Coeffs(&_relu_tmp128_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_158_188._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_155_185._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp128_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_158_188._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_155_185._c1_poly, _pgen_rns_idx_30, degree), Coeffs(_pgen_tmp_159_189, 0, degree), Coeffs(_pgen_tmp_160_190, 0, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_155_185._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp128_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_158_188._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_158_188._c1_poly);
    Free_poly_data(&_pgen_tmp_158_188._c0_poly);
    Free_poly(_pgen_tmp_159_189);
    Free_poly(_pgen_tmp_160_190);
  }
  Init_ciph3_down_scale(&_pgen_tmp_161_191, &_pgen_tmp_155_185);
  Rescale(&_pgen_tmp_161_191._c0_poly, &_pgen_tmp_155_185._c0_poly);
  Rescale(&_pgen_tmp_161_191._c1_poly, &_pgen_tmp_155_185._c1_poly);
  Rescale(&_pgen_tmp_161_191._c2_poly, &_pgen_tmp_155_185._c2_poly);
  Free_poly_data(&_pgen_tmp_155_185._c2_poly);
  Free_poly_data(&_pgen_tmp_155_185._c1_poly);
  Free_poly_data(&_pgen_tmp_155_185._c0_poly);
  _preg_268435476 = Relinearize(_pgen_tmp_161_191);
  Free_poly_data(&_pgen_tmp_161_191._c2_poly);
  Free_poly_data(&_pgen_tmp_161_191._c1_poly);
  Free_poly_data(&_pgen_tmp_161_191._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_162_192, &_preg_268435475, &_preg_268435476);
  Init_ciph_same_scale(&_pgen_tmp_163_193, &_preg_268435474, &_pgen_tmp_162_192);
  Init_ciph3_up_scale(&_pgen_tmp_136_166, &_relu_tmp170_24, &_pgen_tmp_163_193);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_136_166._c0_poly);
    _pgen_tmp_164_194 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_165_195 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_162_192._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435475._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435476._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_162_192._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435475._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435476._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_163_193._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435474._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_162_192._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_163_193._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435474._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_162_192._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_164_194, 0, degree), Coeffs(&_relu_tmp170_24._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_163_193._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_165_195, 0, degree), Coeffs(&_relu_tmp170_24._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_163_193._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_136_166._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp170_24._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_163_193._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_136_166._c1_poly, _pgen_rns_idx_30, degree), Coeffs(_pgen_tmp_164_194, 0, degree), Coeffs(_pgen_tmp_165_195, 0, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_136_166._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp170_24._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_163_193._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_preg_268435476._c1_poly);
    Free_poly_data(&_preg_268435476._c0_poly);
    Free_poly_data(&_preg_268435475._c1_poly);
    Free_poly_data(&_preg_268435475._c0_poly);
    Free_poly_data(&_pgen_tmp_162_192._c1_poly);
    Free_poly_data(&_pgen_tmp_162_192._c0_poly);
    Free_poly_data(&_preg_268435474._c1_poly);
    Free_poly_data(&_preg_268435474._c0_poly);
    Free_poly_data(&_pgen_tmp_163_193._c1_poly);
    Free_poly_data(&_pgen_tmp_163_193._c0_poly);
    Free_poly_data(&_relu_tmp170_24._c1_poly);
    Free_poly_data(&_relu_tmp170_24._c0_poly);
    Free_poly(_pgen_tmp_164_194);
    Free_poly(_pgen_tmp_165_195);
  }
  Init_ciph3_down_scale(&_pgen_tmp_166_196, &_pgen_tmp_136_166);
  Rescale(&_pgen_tmp_166_196._c0_poly, &_pgen_tmp_136_166._c0_poly);
  Rescale(&_pgen_tmp_166_196._c1_poly, &_pgen_tmp_136_166._c1_poly);
  Rescale(&_pgen_tmp_166_196._c2_poly, &_pgen_tmp_136_166._c2_poly);
  Free_poly_data(&_pgen_tmp_136_166._c2_poly);
  Free_poly_data(&_pgen_tmp_136_166._c1_poly);
  Free_poly_data(&_pgen_tmp_136_166._c0_poly);
  _preg_268435477 = Relinearize(_pgen_tmp_166_196);
  Free_poly_data(&_pgen_tmp_166_196._c2_poly);
  Free_poly_data(&_pgen_tmp_166_196._c1_poly);
  Free_poly_data(&_pgen_tmp_166_196._c0_poly);
  Encode_float_mask(&_pgen_tmp_170_200, -0.03691063940349948585551, mask_len_2, 1, Level(&input_ct0_0));
  Init_ciph_up_scale_plain(&_pgen_tmp_169_199, &input_ct0_0, &_pgen_tmp_170_200);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_169_199._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_169_199._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_170_200._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_169_199._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_170_200._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_170_200._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_171_201, &_pgen_tmp_169_199);
  Rescale(&_pgen_tmp_171_201._c0_poly, &_pgen_tmp_169_199._c0_poly);
  Rescale(&_pgen_tmp_171_201._c1_poly, &_pgen_tmp_169_199._c1_poly);
  Free_poly_data(&_pgen_tmp_169_199._c1_poly);
  Free_poly_data(&_pgen_tmp_169_199._c0_poly);
  Init_ciph3_up_scale(&_pgen_tmp_168_198, &_relu_tmp150_20, &_pgen_tmp_171_201);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_168_198._c0_poly);
    _pgen_tmp_172_202 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_173_203 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_172_202, 0, degree), Coeffs(&_relu_tmp150_20._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_171_201._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_173_203, 0, degree), Coeffs(&_relu_tmp150_20._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_171_201._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_168_198._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp150_20._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_171_201._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_168_198._c1_poly, _pgen_rns_idx_30, degree), Coeffs(_pgen_tmp_172_202, 0, degree), Coeffs(_pgen_tmp_173_203, 0, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_168_198._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp150_20._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_171_201._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_171_201._c1_poly);
    Free_poly_data(&_pgen_tmp_171_201._c0_poly);
    Free_poly(_pgen_tmp_172_202);
    Free_poly(_pgen_tmp_173_203);
  }
  Init_ciph3_down_scale(&_pgen_tmp_174_204, &_pgen_tmp_168_198);
  Rescale(&_pgen_tmp_174_204._c0_poly, &_pgen_tmp_168_198._c0_poly);
  Rescale(&_pgen_tmp_174_204._c1_poly, &_pgen_tmp_168_198._c1_poly);
  Rescale(&_pgen_tmp_174_204._c2_poly, &_pgen_tmp_168_198._c2_poly);
  Free_poly_data(&_pgen_tmp_168_198._c2_poly);
  Free_poly_data(&_pgen_tmp_168_198._c1_poly);
  Free_poly_data(&_pgen_tmp_168_198._c0_poly);
  _preg_268435478 = Relinearize(_pgen_tmp_174_204);
  Free_poly_data(&_pgen_tmp_174_204._c2_poly);
  Free_poly_data(&_pgen_tmp_174_204._c1_poly);
  Free_poly_data(&_pgen_tmp_174_204._c0_poly);
  Encode_float_mask(&_pgen_tmp_177_207, 0.1056744718173614761003, mask_len_2, 1, Level(&input_ct0_0));
  Init_ciph_up_scale_plain(&_pgen_tmp_176_206, &input_ct0_0, &_pgen_tmp_177_207);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_176_206._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_176_206._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_177_207._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_176_206._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_177_207._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_177_207._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_178_208, &_pgen_tmp_176_206);
  Rescale(&_pgen_tmp_178_208._c0_poly, &_pgen_tmp_176_206._c0_poly);
  Rescale(&_pgen_tmp_178_208._c1_poly, &_pgen_tmp_176_206._c1_poly);
  Free_poly_data(&_pgen_tmp_176_206._c1_poly);
  Free_poly_data(&_pgen_tmp_176_206._c0_poly);
  Init_ciph3_up_scale(&_pgen_tmp_175_205, &_relu_tmp128_16, &_pgen_tmp_178_208);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_175_205._c0_poly);
    _pgen_tmp_179_209 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_180_210 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_179_209, 0, degree), Coeffs(&_relu_tmp128_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_178_208._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_180_210, 0, degree), Coeffs(&_relu_tmp128_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_178_208._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_175_205._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp128_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_178_208._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_175_205._c1_poly, _pgen_rns_idx_30, degree), Coeffs(_pgen_tmp_179_209, 0, degree), Coeffs(_pgen_tmp_180_210, 0, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_175_205._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp128_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_178_208._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_178_208._c1_poly);
    Free_poly_data(&_pgen_tmp_178_208._c0_poly);
    Free_poly(_pgen_tmp_179_209);
    Free_poly(_pgen_tmp_180_210);
  }
  Init_ciph3_down_scale(&_pgen_tmp_181_211, &_pgen_tmp_175_205);
  Rescale(&_pgen_tmp_181_211._c0_poly, &_pgen_tmp_175_205._c0_poly);
  Rescale(&_pgen_tmp_181_211._c1_poly, &_pgen_tmp_175_205._c1_poly);
  Rescale(&_pgen_tmp_181_211._c2_poly, &_pgen_tmp_175_205._c2_poly);
  Free_poly_data(&_pgen_tmp_175_205._c2_poly);
  Free_poly_data(&_pgen_tmp_175_205._c1_poly);
  Free_poly_data(&_pgen_tmp_175_205._c0_poly);
  _preg_268435479 = Relinearize(_pgen_tmp_181_211);
  Free_poly_data(&_pgen_tmp_181_211._c2_poly);
  Free_poly_data(&_pgen_tmp_181_211._c1_poly);
  Free_poly_data(&_pgen_tmp_181_211._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_182_212, &_preg_268435478, &_preg_268435479);
  Init_ciph3_up_scale(&_pgen_tmp_167_197, &_relu_tmp160_22, &_pgen_tmp_182_212);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_167_197._c0_poly);
    _pgen_tmp_183_213 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_184_214 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_182_212._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435478._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435479._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_182_212._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435478._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435479._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_183_213, 0, degree), Coeffs(&_relu_tmp160_22._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_182_212._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_184_214, 0, degree), Coeffs(&_relu_tmp160_22._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_182_212._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_167_197._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp160_22._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_182_212._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_167_197._c1_poly, _pgen_rns_idx_30, degree), Coeffs(_pgen_tmp_183_213, 0, degree), Coeffs(_pgen_tmp_184_214, 0, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_167_197._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp160_22._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_182_212._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_preg_268435479._c1_poly);
    Free_poly_data(&_preg_268435479._c0_poly);
    Free_poly_data(&_preg_268435478._c1_poly);
    Free_poly_data(&_preg_268435478._c0_poly);
    Free_poly_data(&_pgen_tmp_182_212._c1_poly);
    Free_poly_data(&_pgen_tmp_182_212._c0_poly);
    Free_poly_data(&_relu_tmp160_22._c1_poly);
    Free_poly_data(&_relu_tmp160_22._c0_poly);
    Free_poly(_pgen_tmp_183_213);
    Free_poly(_pgen_tmp_184_214);
  }
  Init_ciph3_down_scale(&_pgen_tmp_185_215, &_pgen_tmp_167_197);
  Rescale(&_pgen_tmp_185_215._c0_poly, &_pgen_tmp_167_197._c0_poly);
  Rescale(&_pgen_tmp_185_215._c1_poly, &_pgen_tmp_167_197._c1_poly);
  Rescale(&_pgen_tmp_185_215._c2_poly, &_pgen_tmp_167_197._c2_poly);
  Free_poly_data(&_pgen_tmp_167_197._c2_poly);
  Free_poly_data(&_pgen_tmp_167_197._c1_poly);
  Free_poly_data(&_pgen_tmp_167_197._c0_poly);
  _preg_268435480 = Relinearize(_pgen_tmp_185_215);
  Free_poly_data(&_pgen_tmp_185_215._c2_poly);
  Free_poly_data(&_pgen_tmp_185_215._c1_poly);
  Free_poly_data(&_pgen_tmp_185_215._c0_poly);
  Encode_float_mask(&_pgen_tmp_188_218, -0.2078597158810965850329, mask_len_2, 1, Level(&input_ct0_0));
  Init_ciph_up_scale_plain(&_pgen_tmp_187_217, &input_ct0_0, &_pgen_tmp_188_218);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_187_217._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_187_217._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_188_218._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_187_217._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_188_218._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_188_218._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_189_219, &_pgen_tmp_187_217);
  Rescale(&_pgen_tmp_189_219._c0_poly, &_pgen_tmp_187_217._c0_poly);
  Rescale(&_pgen_tmp_189_219._c1_poly, &_pgen_tmp_187_217._c1_poly);
  Free_poly_data(&_pgen_tmp_187_217._c1_poly);
  Free_poly_data(&_pgen_tmp_187_217._c0_poly);
  Init_ciph3_up_scale(&_pgen_tmp_186_216, &_relu_tmp150_20, &_pgen_tmp_189_219);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_186_216._c0_poly);
    _pgen_tmp_190_220 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_191_221 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_190_220, 0, degree), Coeffs(&_relu_tmp150_20._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_189_219._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_191_221, 0, degree), Coeffs(&_relu_tmp150_20._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_189_219._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_186_216._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp150_20._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_189_219._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_186_216._c1_poly, _pgen_rns_idx_30, degree), Coeffs(_pgen_tmp_190_220, 0, degree), Coeffs(_pgen_tmp_191_221, 0, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_186_216._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp150_20._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_189_219._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_189_219._c1_poly);
    Free_poly_data(&_pgen_tmp_189_219._c0_poly);
    Free_poly_data(&_relu_tmp150_20._c1_poly);
    Free_poly_data(&_relu_tmp150_20._c0_poly);
    Free_poly(_pgen_tmp_190_220);
    Free_poly(_pgen_tmp_191_221);
  }
  Init_ciph3_down_scale(&_pgen_tmp_192_222, &_pgen_tmp_186_216);
  Rescale(&_pgen_tmp_192_222._c0_poly, &_pgen_tmp_186_216._c0_poly);
  Rescale(&_pgen_tmp_192_222._c1_poly, &_pgen_tmp_186_216._c1_poly);
  Rescale(&_pgen_tmp_192_222._c2_poly, &_pgen_tmp_186_216._c2_poly);
  Free_poly_data(&_pgen_tmp_186_216._c2_poly);
  Free_poly_data(&_pgen_tmp_186_216._c1_poly);
  Free_poly_data(&_pgen_tmp_186_216._c0_poly);
  _preg_268435481 = Relinearize(_pgen_tmp_192_222);
  Free_poly_data(&_pgen_tmp_192_222._c2_poly);
  Free_poly_data(&_pgen_tmp_192_222._c1_poly);
  Free_poly_data(&_pgen_tmp_192_222._c0_poly);
  Encode_float_mask(&_pgen_tmp_195_225, 0.6334139843709288841822, mask_len_2, 1, Level(&input_ct0_0));
  Init_ciph_up_scale_plain(&_pgen_tmp_194_224, &input_ct0_0, &_pgen_tmp_195_225);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_194_224._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_194_224._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_195_225._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_194_224._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_195_225._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_195_225._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_196_226, &_pgen_tmp_194_224);
  Rescale(&_pgen_tmp_196_226._c0_poly, &_pgen_tmp_194_224._c0_poly);
  Rescale(&_pgen_tmp_196_226._c1_poly, &_pgen_tmp_194_224._c1_poly);
  Free_poly_data(&_pgen_tmp_194_224._c1_poly);
  Free_poly_data(&_pgen_tmp_194_224._c0_poly);
  Init_ciph3_up_scale(&_pgen_tmp_193_223, &_relu_tmp128_16, &_pgen_tmp_196_226);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_193_223._c0_poly);
    _pgen_tmp_197_227 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_198_228 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_197_227, 0, degree), Coeffs(&_relu_tmp128_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_196_226._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(_pgen_tmp_198_228, 0, degree), Coeffs(&_relu_tmp128_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_196_226._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_193_223._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp128_16._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_196_226._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_193_223._c1_poly, _pgen_rns_idx_30, degree), Coeffs(_pgen_tmp_197_227, 0, degree), Coeffs(_pgen_tmp_198_228, 0, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_193_223._c2_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp128_16._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_196_226._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_196_226._c1_poly);
    Free_poly_data(&_pgen_tmp_196_226._c0_poly);
    Free_poly_data(&_relu_tmp128_16._c1_poly);
    Free_poly_data(&_relu_tmp128_16._c0_poly);
    Free_poly(_pgen_tmp_197_227);
    Free_poly(_pgen_tmp_198_228);
  }
  Init_ciph3_down_scale(&_pgen_tmp_199_229, &_pgen_tmp_193_223);
  Rescale(&_pgen_tmp_199_229._c0_poly, &_pgen_tmp_193_223._c0_poly);
  Rescale(&_pgen_tmp_199_229._c1_poly, &_pgen_tmp_193_223._c1_poly);
  Rescale(&_pgen_tmp_199_229._c2_poly, &_pgen_tmp_193_223._c2_poly);
  Free_poly_data(&_pgen_tmp_193_223._c2_poly);
  Free_poly_data(&_pgen_tmp_193_223._c1_poly);
  Free_poly_data(&_pgen_tmp_193_223._c0_poly);
  _preg_268435482 = Relinearize(_pgen_tmp_199_229);
  Free_poly_data(&_pgen_tmp_199_229._c2_poly);
  Free_poly_data(&_pgen_tmp_199_229._c1_poly);
  Free_poly_data(&_pgen_tmp_199_229._c0_poly);
  Init_ciph_same_scale(&_pgen_tmp_200_230, &_preg_268435481, &_preg_268435482);
  Init_ciph_same_scale(&_pgen_tmp_201_231, &_preg_268435480, &_pgen_tmp_200_230);
  Init_ciph_same_scale(&_relu_tmp239_25, &_preg_268435477, &_pgen_tmp_201_231);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_relu_tmp239_25._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_200_230._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435481._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435482._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_200_230._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435481._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435482._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_201_231._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435480._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_200_230._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_201_231._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435480._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_200_230._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_relu_tmp239_25._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435477._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_201_231._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_relu_tmp239_25._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_preg_268435477._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_201_231._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_preg_268435482._c1_poly);
    Free_poly_data(&_preg_268435482._c0_poly);
    Free_poly_data(&_preg_268435481._c1_poly);
    Free_poly_data(&_preg_268435481._c0_poly);
    Free_poly_data(&_pgen_tmp_200_230._c1_poly);
    Free_poly_data(&_pgen_tmp_200_230._c0_poly);
    Free_poly_data(&_preg_268435480._c1_poly);
    Free_poly_data(&_preg_268435480._c0_poly);
    Free_poly_data(&_pgen_tmp_201_231._c1_poly);
    Free_poly_data(&_pgen_tmp_201_231._c0_poly);
    Free_poly_data(&_preg_268435477._c1_poly);
    Free_poly_data(&_preg_268435477._c0_poly);
  }
  Encode_float_mask(&_pgen_tmp_203_233, 0.5, mask_len_2, 1, Level(&input_ct0_0));
  Init_ciph_up_scale_plain(&_pgen_tmp_202_232, &input_ct0_0, &_pgen_tmp_203_233);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_pgen_tmp_202_232._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_202_232._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_203_233._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_202_232._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&input_ct0_0._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_203_233._poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_203_233._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_204_234, &_pgen_tmp_202_232);
  Rescale(&_pgen_tmp_204_234._c0_poly, &_pgen_tmp_202_232._c0_poly);
  Rescale(&_pgen_tmp_204_234._c1_poly, &_pgen_tmp_202_232._c1_poly);
  Free_poly_data(&_pgen_tmp_202_232._c1_poly);
  Free_poly_data(&_pgen_tmp_202_232._c0_poly);
  Init_ciph_same_scale(&_relu_tmp247_26, &_relu_tmp239_25, &_pgen_tmp_204_234);
  {
    _pgen_modulus_28 = Q_modulus();
    _pgen_num_q_29 = Poly_level(&_relu_tmp247_26._c0_poly);
    for (_pgen_rns_idx_30 = 0; _pgen_rns_idx_30 < _pgen_num_q_29; _pgen_rns_idx_30 = _pgen_rns_idx_30 + 1) {
      Hw_modadd(Coeffs(&_relu_tmp247_26._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp239_25._c0_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_204_234._c0_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      Hw_modadd(Coeffs(&_relu_tmp247_26._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_relu_tmp239_25._c1_poly, _pgen_rns_idx_30, degree), Coeffs(&_pgen_tmp_204_234._c1_poly, _pgen_rns_idx_30, degree), _pgen_modulus_28, degree);
      _pgen_modulus_28 = _pgen_modulus_28 + 1;
    }
    Free_poly_data(&_pgen_tmp_204_234._c1_poly);
    Free_poly_data(&_pgen_tmp_204_234._c0_poly);
    Free_poly_data(&_relu_tmp239_25._c1_poly);
    Free_poly_data(&_relu_tmp239_25._c0_poly);
  }
  return _relu_tmp247_26;
}

CIPHERTEXT Rotate(CIPHERTEXT ciph_0, int32_t rot_idx_1) {
  CIPHERTEXT _pgen_rot_res_2;
  POLY _pgen_swk_c0_3;
  POLY _pgen_swk_c1_4;
  POLY _pgen_ext_5;
  POLY _pgen_tmp_poly_6;
  POLY _pgen_mod_down_c0_7;
  POLY _pgen_mod_down_c1_8;
  SWITCH_KEY* _pgen_swk_9;
  uint32_t _pgen_part_idx_10;
  POLY _pgen_key0_11;
  POLY _pgen_key1_12;
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
  Init_ciph_same_scale(&_pgen_rot_res_2, &ciph_0, 0);
  {
    _pgen_swk_c0_3 = Alloc_poly(degree, Poly_level(&ciph_0._c1_poly), 1);
    _pgen_swk_c1_4 = Alloc_poly(degree, Poly_level(&ciph_0._c1_poly), 1);
    _pgen_ext_5 = Alloc_poly(degree, Poly_level(&ciph_0._c1_poly), 1);
    _pgen_tmp_poly_6 = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0_7 = Alloc_poly(degree, Poly_level(&ciph_0._c0_poly), 0);
    _pgen_mod_down_c1_8 = Alloc_poly(degree, Poly_level(&ciph_0._c1_poly), 0);
    _pgen_swk_9 = Swk(1, rot_idx_1);
    for (_pgen_part_idx_10 = 0; _pgen_part_idx_10 < Num_decomp(&ciph_0._c1_poly); _pgen_part_idx_10 = _pgen_part_idx_10 + 1) {
      Decomp_modup(_pgen_ext_5, &ciph_0._c1_poly, _pgen_part_idx_10);
      _pgen_key0_11 = Pk0_at(_pgen_swk_9, _pgen_part_idx_10);
      _pgen_key1_12 = Pk1_at(_pgen_swk_9, _pgen_part_idx_10);
      {
        _pgen_modulus_13 = Q_modulus();
        _pgen_num_q_14 = Poly_level(_pgen_ext_5);
        for (_pgen_rns_idx_15 = 0; _pgen_rns_idx_15 < _pgen_num_q_14; _pgen_rns_idx_15 = _pgen_rns_idx_15 + 1) {
          Hw_modmul(Coeffs(_pgen_tmp_poly_6, 0, degree), Coeffs(_pgen_key0_11, _pgen_rns_idx_15, degree), Coeffs(_pgen_ext_5, _pgen_rns_idx_15, degree), _pgen_modulus_13, degree);
          Hw_modadd(Coeffs(_pgen_swk_c0_3, _pgen_rns_idx_15, degree), Coeffs(_pgen_swk_c0_3, _pgen_rns_idx_15, degree), Coeffs(_pgen_tmp_poly_6, 0, degree), _pgen_modulus_13, degree);
          Hw_modmul(Coeffs(_pgen_tmp_poly_6, 0, degree), Coeffs(_pgen_key1_12, _pgen_rns_idx_15, degree), Coeffs(_pgen_ext_5, _pgen_rns_idx_15, degree), _pgen_modulus_13, degree);
          Hw_modadd(Coeffs(_pgen_swk_c1_4, _pgen_rns_idx_15, degree), Coeffs(_pgen_swk_c1_4, _pgen_rns_idx_15, degree), Coeffs(_pgen_tmp_poly_6, 0, degree), _pgen_modulus_13, degree);
          _pgen_modulus_13 = _pgen_modulus_13 + 1;
        }
      }
      {
        _pgen_modulus_13 = P_modulus();
        _pgen_num_p_16 = Num_p(_pgen_ext_5);
        _pgen_p_ofst_17 = Num_alloc(_pgen_ext_5) - _pgen_num_p_16;
        _pgen_key_p_ofst_19 = Poly_level(_pgen_key0_11);
        for (_pgen_rns_idx_15 = 0; _pgen_rns_idx_15 < _pgen_num_p_16; _pgen_rns_idx_15 = _pgen_rns_idx_15 + 1) {
          _pgen_p_idx_18 = _pgen_rns_idx_15 + _pgen_p_ofst_17;
          _pgen_key_p_idx_20 = _pgen_rns_idx_15 + _pgen_key_p_ofst_19;
          Hw_modmul(Coeffs(_pgen_tmp_poly_6, 0, degree), Coeffs(_pgen_key0_11, _pgen_key_p_idx_20, degree), Coeffs(_pgen_ext_5, _pgen_p_idx_18, degree), _pgen_modulus_13, degree);
          Hw_modadd(Coeffs(_pgen_swk_c0_3, _pgen_p_idx_18, degree), Coeffs(_pgen_swk_c0_3, _pgen_p_idx_18, degree), Coeffs(_pgen_tmp_poly_6, 0, degree), _pgen_modulus_13, degree);
          Hw_modmul(Coeffs(_pgen_tmp_poly_6, 0, degree), Coeffs(_pgen_key1_12, _pgen_key_p_idx_20, degree), Coeffs(_pgen_ext_5, _pgen_p_idx_18, degree), _pgen_modulus_13, degree);
          Hw_modadd(Coeffs(_pgen_swk_c1_4, _pgen_p_idx_18, degree), Coeffs(_pgen_swk_c1_4, _pgen_p_idx_18, degree), Coeffs(_pgen_tmp_poly_6, 0, degree), _pgen_modulus_13, degree);
          _pgen_modulus_13 = _pgen_modulus_13 + 1;
        }
      }
    }
    Mod_down(_pgen_mod_down_c0_7, _pgen_swk_c0_3);
    Mod_down(_pgen_mod_down_c1_8, _pgen_swk_c1_4);
    {
      _pgen_modulus_13 = Q_modulus();
      _pgen_num_q_14 = Poly_level(_pgen_mod_down_c0_7);
      for (_pgen_rns_idx_15 = 0; _pgen_rns_idx_15 < _pgen_num_q_14; _pgen_rns_idx_15 = _pgen_rns_idx_15 + 1) {
        Hw_modadd(Coeffs(_pgen_mod_down_c0_7, _pgen_rns_idx_15, degree), Coeffs(_pgen_mod_down_c0_7, _pgen_rns_idx_15, degree), Coeffs(&ciph_0._c0_poly, _pgen_rns_idx_15, degree), _pgen_modulus_13, degree);
        _pgen_modulus_13 = _pgen_modulus_13 + 1;
      }
    }
    _pgen_order_21 = Auto_order(rot_idx_1);
    {
      _pgen_modulus_13 = Q_modulus();
      _pgen_num_q_14 = Poly_level(_pgen_mod_down_c0_7);
      for (_pgen_rns_idx_15 = 0; _pgen_rns_idx_15 < _pgen_num_q_14; _pgen_rns_idx_15 = _pgen_rns_idx_15 + 1) {
        Hw_rotate(Coeffs(&_pgen_rot_res_2._c0_poly, _pgen_rns_idx_15, degree), Coeffs(_pgen_mod_down_c0_7, _pgen_rns_idx_15, degree), _pgen_order_21, _pgen_modulus_13, degree);
        Hw_rotate(Coeffs(&_pgen_rot_res_2._c1_poly, _pgen_rns_idx_15, degree), Coeffs(_pgen_mod_down_c1_8, _pgen_rns_idx_15, degree), _pgen_order_21, _pgen_modulus_13, degree);
        _pgen_modulus_13 = _pgen_modulus_13 + 1;
      }
    }
    Free_poly(_pgen_swk_c0_3);
    Free_poly(_pgen_swk_c1_4);
    Free_poly(_pgen_ext_5);
    Free_poly(_pgen_tmp_poly_6);
    Free_poly(_pgen_mod_down_c0_7);
    Free_poly(_pgen_mod_down_c1_8);
    return _pgen_rot_res_2;
  }
}

CIPHERTEXT Relinearize(CIPHERTEXT3 ciph_0) {
  CIPHERTEXT _pgen_relin_res_1;
  POLY _pgen_swk_c0_2;
  POLY _pgen_swk_c1_3;
  POLY _pgen_ext_4;
  POLY _pgen_tmp_poly_5;
  POLY _pgen_mod_down_c0_6;
  POLY _pgen_mod_down_c1_7;
  SWITCH_KEY* _pgen_swk_8;
  uint32_t _pgen_part_idx_9;
  POLY _pgen_key0_10;
  POLY _pgen_key1_11;
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
  Init_ciph_same_scale_ciph3(&_pgen_relin_res_1, &ciph_0);
  {
    _pgen_swk_c0_2 = Alloc_poly(degree, Poly_level(&ciph_0._c2_poly), 1);
    _pgen_swk_c1_3 = Alloc_poly(degree, Poly_level(&ciph_0._c2_poly), 1);
    _pgen_ext_4 = Alloc_poly(degree, Poly_level(&ciph_0._c2_poly), 1);
    _pgen_tmp_poly_5 = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0_6 = Alloc_poly(degree, Poly_level(&ciph_0._c0_poly), 0);
    _pgen_mod_down_c1_7 = Alloc_poly(degree, Poly_level(&ciph_0._c2_poly), 0);
    _pgen_swk_8 = Swk(0, 0);
    for (_pgen_part_idx_9 = 0; _pgen_part_idx_9 < Num_decomp(&ciph_0._c2_poly); _pgen_part_idx_9 = _pgen_part_idx_9 + 1) {
      Decomp_modup(_pgen_ext_4, &ciph_0._c2_poly, _pgen_part_idx_9);
      _pgen_key0_10 = Pk0_at(_pgen_swk_8, _pgen_part_idx_9);
      _pgen_key1_11 = Pk1_at(_pgen_swk_8, _pgen_part_idx_9);
      {
        _pgen_modulus_12 = Q_modulus();
        _pgen_num_q_13 = Poly_level(_pgen_ext_4);
        for (_pgen_rns_idx_14 = 0; _pgen_rns_idx_14 < _pgen_num_q_13; _pgen_rns_idx_14 = _pgen_rns_idx_14 + 1) {
          Hw_modmul(Coeffs(_pgen_tmp_poly_5, 0, degree), Coeffs(_pgen_key0_10, _pgen_rns_idx_14, degree), Coeffs(_pgen_ext_4, _pgen_rns_idx_14, degree), _pgen_modulus_12, degree);
          Hw_modadd(Coeffs(_pgen_swk_c0_2, _pgen_rns_idx_14, degree), Coeffs(_pgen_swk_c0_2, _pgen_rns_idx_14, degree), Coeffs(_pgen_tmp_poly_5, 0, degree), _pgen_modulus_12, degree);
          Hw_modmul(Coeffs(_pgen_tmp_poly_5, 0, degree), Coeffs(_pgen_key1_11, _pgen_rns_idx_14, degree), Coeffs(_pgen_ext_4, _pgen_rns_idx_14, degree), _pgen_modulus_12, degree);
          Hw_modadd(Coeffs(_pgen_swk_c1_3, _pgen_rns_idx_14, degree), Coeffs(_pgen_swk_c1_3, _pgen_rns_idx_14, degree), Coeffs(_pgen_tmp_poly_5, 0, degree), _pgen_modulus_12, degree);
          _pgen_modulus_12 = _pgen_modulus_12 + 1;
        }
      }
      {
        _pgen_modulus_12 = P_modulus();
        _pgen_num_p_15 = Num_p(_pgen_ext_4);
        _pgen_p_ofst_16 = Num_alloc(_pgen_ext_4) - _pgen_num_p_15;
        _pgen_key_p_ofst_18 = Poly_level(_pgen_key0_10);
        for (_pgen_rns_idx_14 = 0; _pgen_rns_idx_14 < _pgen_num_p_15; _pgen_rns_idx_14 = _pgen_rns_idx_14 + 1) {
          _pgen_p_idx_17 = _pgen_rns_idx_14 + _pgen_p_ofst_16;
          _pgen_key_p_idx_19 = _pgen_rns_idx_14 + _pgen_key_p_ofst_18;
          Hw_modmul(Coeffs(_pgen_tmp_poly_5, 0, degree), Coeffs(_pgen_key0_10, _pgen_key_p_idx_19, degree), Coeffs(_pgen_ext_4, _pgen_p_idx_17, degree), _pgen_modulus_12, degree);
          Hw_modadd(Coeffs(_pgen_swk_c0_2, _pgen_p_idx_17, degree), Coeffs(_pgen_swk_c0_2, _pgen_p_idx_17, degree), Coeffs(_pgen_tmp_poly_5, 0, degree), _pgen_modulus_12, degree);
          Hw_modmul(Coeffs(_pgen_tmp_poly_5, 0, degree), Coeffs(_pgen_key1_11, _pgen_key_p_idx_19, degree), Coeffs(_pgen_ext_4, _pgen_p_idx_17, degree), _pgen_modulus_12, degree);
          Hw_modadd(Coeffs(_pgen_swk_c1_3, _pgen_p_idx_17, degree), Coeffs(_pgen_swk_c1_3, _pgen_p_idx_17, degree), Coeffs(_pgen_tmp_poly_5, 0, degree), _pgen_modulus_12, degree);
          _pgen_modulus_12 = _pgen_modulus_12 + 1;
        }
      }
    }
    Mod_down(_pgen_mod_down_c0_6, _pgen_swk_c0_2);
    Mod_down(_pgen_mod_down_c1_7, _pgen_swk_c1_3);
    {
      _pgen_modulus_12 = Q_modulus();
      _pgen_num_q_13 = Poly_level(&ciph_0._c0_poly);
      for (_pgen_rns_idx_14 = 0; _pgen_rns_idx_14 < _pgen_num_q_13; _pgen_rns_idx_14 = _pgen_rns_idx_14 + 1) {
        Hw_modadd(Coeffs(&_pgen_relin_res_1._c0_poly, _pgen_rns_idx_14, degree), Coeffs(_pgen_mod_down_c0_6, _pgen_rns_idx_14, degree), Coeffs(&ciph_0._c0_poly, _pgen_rns_idx_14, degree), _pgen_modulus_12, degree);
        Hw_modadd(Coeffs(&_pgen_relin_res_1._c1_poly, _pgen_rns_idx_14, degree), Coeffs(_pgen_mod_down_c1_7, _pgen_rns_idx_14, degree), Coeffs(&ciph_0._c1_poly, _pgen_rns_idx_14, degree), _pgen_modulus_12, degree);
        _pgen_modulus_12 = _pgen_modulus_12 + 1;
      }
    }
    Free_poly(_pgen_swk_c0_2);
    Free_poly(_pgen_swk_c1_3);
    Free_poly(_pgen_ext_4);
    Free_poly(_pgen_tmp_poly_5);
    Free_poly(_pgen_mod_down_c0_6);
    Free_poly(_pgen_mod_down_c1_7);
    return _pgen_relin_res_1;
  }
}

CKKS_PARAMS* Get_context_params() {
  static CKKS_PARAMS parm = {
    LIB_ANT, 65536, 0, 33, 4, 60, 56, 3, 192, 1607, 
    { -25088, -12544, -6272, -1000, -675, -674, -673, -672,
      -671, -670, -669, -451, -450, -449, -448, -447,
      -446, -445, -227, -226, -225, -224, -223, -222,
      -221, -57, -56, -55, -29, -28, -27, -15,
      -14, -13, -8, -7, -6, -3, -2, -1,
      0, 1, 2, 3, 4, 5, 6, 7,
      8, 9, 10, 11, 12, 13, 14, 15,
      16, 17, 18, 19, 20, 21, 22, 23,
      24, 25, 26, 27, 28, 29, 30, 31,
      32, 33, 34, 35, 36, 37, 38, 39,
      40, 41, 42, 43, 44, 45, 46, 47,
      48, 49, 50, 51, 52, 53, 54, 55,
      56, 57, 58, 59, 60, 61, 62, 63,
      64, 65, 66, 67, 68, 69, 70, 71,
      72, 73, 74, 75, 76, 77, 78, 79,
      80, 81, 82, 83, 84, 85, 86, 87,
      88, 89, 90, 91, 92, 93, 94, 95,
      96, 97, 98, 99, 100, 101, 102, 103,
      104, 105, 106, 107, 108, 109, 110, 111,
      112, 113, 114, 115, 116, 117, 118, 119,
      120, 121, 122, 123, 124, 125, 126, 127,
      128, 129, 130, 131, 132, 133, 134, 135,
      136, 137, 138, 139, 140, 141, 142, 143,
      144, 145, 146, 147, 148, 149, 150, 151,
      152, 153, 154, 155, 156, 157, 158, 159,
      160, 161, 162, 163, 164, 165, 166, 167,
      168, 169, 170, 171, 172, 173, 174, 175,
      176, 177, 178, 179, 180, 181, 182, 183,
      184, 185, 186, 187, 188, 189, 190, 191,
      192, 193, 194, 195, 196, 197, 198, 199,
      200, 201, 202, 203, 204, 205, 206, 207,
      208, 209, 210, 211, 212, 213, 214, 215,
      216, 217, 218, 219, 220, 221, 222, 223,
      224, 225, 226, 227, 228, 229, 230, 231,
      232, 233, 234, 235, 236, 237, 238, 239,
      240, 241, 242, 243, 244, 245, 246, 247,
      248, 249, 250, 251, 252, 253, 254, 255,
      256, 257, 258, 259, 260, 261, 262, 263,
      264, 265, 266, 267, 268, 269, 270, 271,
      272, 273, 274, 275, 276, 277, 278, 279,
      280, 281, 282, 283, 284, 285, 286, 287,
      288, 289, 290, 291, 292, 293, 294, 295,
      296, 297, 298, 299, 300, 301, 302, 303,
      304, 305, 306, 307, 308, 309, 310, 311,
      312, 313, 314, 315, 316, 317, 318, 319,
      320, 321, 322, 323, 324, 325, 326, 327,
      328, 329, 330, 331, 332, 333, 334, 335,
      336, 337, 338, 339, 340, 341, 342, 343,
      344, 345, 346, 347, 348, 349, 350, 351,
      352, 353, 354, 355, 356, 357, 358, 359,
      360, 361, 362, 363, 364, 365, 366, 367,
      368, 369, 370, 371, 372, 373, 374, 375,
      376, 377, 378, 379, 380, 381, 382, 383,
      384, 385, 386, 387, 388, 389, 390, 391,
      392, 393, 394, 395, 396, 397, 398, 399,
      400, 401, 402, 403, 404, 405, 406, 407,
      408, 409, 410, 411, 412, 413, 414, 415,
      416, 417, 418, 419, 420, 421, 422, 423,
      424, 425, 426, 427, 428, 429, 430, 431,
      432, 433, 434, 435, 436, 437, 438, 439,
      440, 441, 442, 443, 444, 445, 446, 447,
      448, 449, 450, 451, 452, 453, 454, 455,
      456, 457, 458, 459, 460, 461, 462, 463,
      464, 465, 466, 467, 468, 469, 470, 471,
      472, 473, 474, 475, 476, 477, 478, 479,
      480, 481, 482, 483, 484, 485, 486, 487,
      488, 489, 490, 491, 492, 493, 494, 495,
      496, 497, 498, 499, 500, 501, 502, 503,
      504, 505, 506, 507, 508, 509, 510, 511,
      512, 513, 514, 515, 516, 517, 518, 519,
      520, 521, 522, 523, 524, 525, 526, 527,
      528, 529, 530, 531, 532, 533, 534, 535,
      536, 537, 538, 539, 540, 541, 542, 543,
      544, 545, 546, 547, 548, 549, 550, 551,
      552, 553, 554, 555, 556, 557, 558, 559,
      560, 561, 562, 563, 564, 565, 566, 567,
      568, 569, 570, 571, 572, 573, 574, 575,
      576, 577, 578, 579, 580, 581, 582, 583,
      584, 585, 586, 587, 588, 589, 590, 591,
      592, 593, 594, 595, 596, 597, 598, 599,
      600, 601, 602, 603, 604, 605, 606, 607,
      608, 609, 610, 611, 612, 613, 614, 615,
      616, 617, 618, 619, 620, 621, 622, 623,
      624, 625, 626, 627, 628, 629, 630, 631,
      632, 633, 634, 635, 636, 637, 638, 639,
      640, 641, 642, 643, 644, 645, 646, 647,
      648, 649, 650, 651, 652, 653, 654, 655,
      656, 657, 658, 659, 660, 661, 662, 663,
      664, 665, 666, 667, 668, 669, 670, 671,
      672, 673, 674, 675, 676, 677, 678, 679,
      680, 681, 682, 683, 684, 685, 686, 687,
      688, 689, 690, 691, 692, 693, 694, 695,
      696, 697, 698, 699, 700, 701, 702, 703,
      704, 705, 706, 707, 708, 709, 710, 711,
      712, 713, 714, 715, 716, 717, 718, 719,
      720, 721, 722, 723, 724, 725, 726, 727,
      728, 729, 730, 731, 732, 733, 734, 735,
      736, 737, 738, 739, 740, 741, 742, 743,
      744, 745, 746, 747, 748, 749, 750, 751,
      752, 753, 754, 755, 756, 757, 758, 759,
      760, 761, 762, 763, 764, 765, 766, 767,
      768, 769, 770, 771, 772, 773, 774, 775,
      776, 777, 778, 779, 780, 781, 782, 783,
      784, 785, 786, 787, 788, 789, 790, 791,
      792, 793, 794, 795, 796, 797, 798, 799,
      800, 801, 802, 803, 804, 805, 806, 807,
      808, 809, 810, 811, 812, 813, 814, 815,
      816, 817, 818, 819, 820, 821, 822, 823,
      824, 825, 826, 827, 828, 829, 830, 831,
      832, 833, 834, 835, 836, 837, 838, 839,
      840, 841, 842, 843, 844, 845, 846, 847,
      848, 849, 850, 851, 852, 853, 854, 855,
      856, 857, 858, 859, 860, 861, 862, 863,
      864, 865, 866, 867, 868, 869, 870, 871,
      872, 873, 874, 875, 876, 877, 878, 879,
      880, 881, 882, 883, 884, 885, 886, 887,
      888, 889, 890, 891, 892, 893, 894, 895,
      896, 897, 898, 899, 900, 901, 902, 903,
      904, 905, 906, 907, 908, 909, 910, 911,
      912, 913, 914, 915, 916, 917, 918, 919,
      920, 921, 922, 923, 924, 925, 926, 927,
      928, 929, 930, 931, 932, 933, 934, 935,
      936, 937, 938, 939, 940, 941, 942, 943,
      944, 945, 946, 947, 948, 949, 950, 951,
      952, 953, 954, 955, 956, 957, 958, 959,
      960, 961, 962, 963, 964, 965, 966, 967,
      968, 969, 970, 971, 972, 973, 974, 975,
      976, 977, 978, 979, 980, 981, 982, 983,
      984, 985, 986, 987, 988, 989, 990, 991,
      992, 993, 994, 995, 996, 997, 998, 999,
      1008, 1029, 1078, 1092, 1127, 1176, 1225, 1260,
      1274, 1323, 1344, 1372, 1421, 1428, 1470, 1512,
      1519, 1536, 1568, 1596, 1617, 1666, 1680, 1715,
      1764, 1813, 1848, 1862, 1911, 1932, 1960, 2009,
      2016, 2058, 2100, 2107, 2156, 2184, 2205, 2254,
      2268, 2303, 2352, 2401, 2450, 2499, 2520, 2548,
      2597, 2646, 2688, 2695, 2744, 2793, 2842, 2856,
      2891, 2940, 2989, 3024, 3038, 3072, 3087, 3136,
      3185, 3192, 3234, 3283, 3332, 3360, 3381, 3430,
      3479, 3528, 3577, 3626, 3675, 3696, 3724, 3773,
      3822, 3864, 3871, 3920, 3969, 4018, 4032, 4067,
      4116, 4165, 4200, 4214, 4263, 4312, 4361, 4368,
      4410, 4459, 4508, 4536, 4557, 4606, 4655, 4704,
      4753, 4802, 4851, 4872, 4900, 4949, 4998, 5040,
      5047, 5096, 5145, 5194, 5208, 5243, 5292, 5341,
      5376, 5390, 5439, 5488, 5537, 5544, 5586, 5635,
      5684, 5712, 5733, 5782, 5831, 5880, 5929, 5978,
      6027, 6048, 6076, 6125, 6174, 6216, 6223, 6272,
      6321, 6370, 6384, 6419, 6468, 6517, 6552, 6566,
      6615, 6664, 6713, 6720, 6762, 6811, 6860, 6888,
      6909, 6958, 7007, 7056, 7105, 7154, 7203, 7224,
      7252, 7301, 7350, 7392, 7399, 7448, 7497, 7546,
      7560, 7595, 7644, 7693, 7728, 7742, 7791, 7840,
      7889, 7896, 7938, 7987, 8036, 8064, 8085, 8134,
      8183, 8232, 8281, 8330, 8379, 8400, 8428, 8477,
      8526, 8568, 8575, 8624, 8673, 8722, 8736, 8771,
      8820, 8869, 8904, 8918, 8967, 9016, 9065, 9072,
      9114, 9163, 9212, 9240, 9261, 9310, 9359, 9408,
      9457, 9506, 9555, 9604, 9653, 9702, 9744, 9751,
      9800, 9849, 9898, 9947, 9996, 10045, 10080, 10094,
      10143, 10192, 10241, 10290, 10339, 10388, 10416, 10437,
      10486, 10535, 10584, 10633, 10682, 10731, 10752, 10780,
      10829, 10878, 10927, 10976, 11025, 11074, 11088, 11123,
      11172, 11221, 11270, 11319, 11368, 11417, 11424, 11466,
      11515, 11564, 11613, 11662, 11711, 11760, 11809, 11858,
      11907, 11956, 12005, 12054, 12096, 12103, 12152, 12201,
      12250, 12299, 12348, 12397, 12432, 12446, 12495, 12544,
      12593, 12642, 12691, 12740, 12768, 12789, 12838, 12887,
      12936, 12985, 13034, 13083, 13104, 13132, 13181, 13230,
      13279, 13328, 13377, 13426, 13440, 13475, 13524, 13573,
      13622, 13671, 13720, 13769, 13776, 13818, 13867, 13916,
      13965, 14014, 14063, 14112, 14161, 14210, 14259, 14308,
      14357, 14406, 14448, 14455, 14504, 14553, 14602, 14651,
      14700, 14749, 14784, 14798, 14847, 14896, 14945, 14994,
      15043, 15092, 15120, 15141, 15190, 15239, 15288, 15337,
      15386, 15435, 15456, 15484, 15533, 15582, 15631, 15680,
      15729, 15778, 15792, 15827, 15876, 15925, 15974, 16023,
      16072, 16121, 16128, 16170, 16219, 16268, 16317, 16366,
      16415, 16464, 16513, 16562, 16611, 16660, 16709, 16758,
      16800, 16807, 16856, 16905, 16954, 17003, 17052, 17101,
      17136, 17150, 17199, 17248, 17297, 17346, 17395, 17444,
      17472, 17493, 17542, 17591, 17640, 17689, 17738, 17787,
      17808, 17836, 17885, 17934, 17983, 18032, 18081, 18130,
      18144, 18179, 18228, 18277, 18326, 18375, 18424, 18473,
      18480, 18522, 18571, 18620, 18669, 18718, 18767, 18816,
      18865, 18914, 18963, 19012, 19061, 19110, 19159, 19208,
      19257, 19306, 19355, 19404, 19453, 19502, 19551, 19600,
      19649, 19698, 19747, 19796, 19845, 19894, 19943, 19992,
      20041, 20090, 20139, 20188, 20237, 20286, 20335, 20384,
      20433, 20482, 20531, 20580, 20629, 20678, 20727, 20776,
      20825, 20874, 20923, 20972, 21021, 21070, 21119, 21168,
      21217, 21266, 21315, 21364, 21413, 21462, 21511, 21560,
      21609, 21658, 21707, 21756, 21805, 21854, 21903, 21952,
      22001, 22050, 22099, 22148, 22197, 22246, 22295, 22344,
      22393, 22442, 22491, 22540, 22589, 22638, 22687, 22736,
      22785, 22834, 22883, 22932, 22981, 23030, 23079, 23128,
      23177, 23226, 23275, 23324, 23373, 23422, 23471, 23520,
      23569, 23618, 23667, 23716, 23765, 23814, 23863, 23912,
      23961, 24010, 24059, 24108, 24157, 24206, 24255, 24304,
      24353, 24402, 24451, 24500, 24549, 24598, 24647, 24696,
      24745, 24794, 24843, 24892, 24941, 24990, 25039 }
  };
  return &parm;
}

RT_DATA_INFO* Get_rt_data_info() {
  static RT_DATA_INFO info = {
    "/app/release/resnet18.bin",
    "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX",
    DE_MSG_F32
  };
  return &info;
}

int32_t _cst_141[49] = {
  -675, -674, -673, -672, -671, -670, -669, -451,
  -450, -449, -448, -447, -446, -445, -227, -226,
  -225, -224, -223, -222, -221, -3, -2, -1,
  0, 1, 2, 3, 221, 222, 223, 224,
  225, 226, 227, 445, 446, 447, 448, 449,
  450, 451, 669, 670, 671, 672, 673, 674,
  675
};
int32_t _cst_165[9] = {
  -57, -56, -55, -1, 0, 1, 55, 56,
  57
};
int32_t _cst_177[9] = {
  -57, -56, -55, -1, 0, 1, 55, 56,
  57
};
int32_t _cst_191[9] = {
  -57, -56, -55, -1, 0, 1, 55, 56,
  57
};
int32_t _cst_203[9] = {
  -57, -56, -55, -1, 0, 1, 55, 56,
  57
};
int32_t _cst_217[1] = {
  0
};
int32_t _cst_234[9] = {
  -57, -56, -55, -1, 0, 1, 55, 56,
  57
};
int32_t _cst_252[9] = {
  -29, -28, -27, -1, 0, 1, 27, 28,
  29
};
int32_t _cst_266[9] = {
  -29, -28, -27, -1, 0, 1, 27, 28,
  29
};
int32_t _cst_278[9] = {
  -29, -28, -27, -1, 0, 1, 27, 28,
  29
};
int32_t _cst_292[4] = {
  0, 784, 1568, 2352
};
int32_t _cst_309[9] = {
  -29, -28, -27, -1, 0, 1, 27, 28,
  29
};
int32_t _cst_327[9] = {
  -15, -14, -13, -1, 0, 1, 13, 14,
  15
};
int32_t _cst_341[9] = {
  -15, -14, -13, -1, 0, 1, 13, 14,
  15
};
int32_t _cst_353[9] = {
  -15, -14, -13, -1, 0, 1, 13, 14,
  15
};
int32_t _cst_367[8] = {
  0, 196, 392, 588, 784, 980, 1176, 1372
};
int32_t _cst_383[9] = {
  -15, -14, -13, -1, 0, 1, 13, 14,
  15
};
int32_t _cst_397[9] = {
  -8, -7, -6, -1, 0, 1, 6, 7,
  8
};
int32_t _cst_405[9] = {
  -8, -7, -6, -1, 0, 1, 6, 7,
  8
};
int32_t _cst_412[9] = {
  -8, -7, -6, -1, 0, 1, 6, 7,
  8
};
