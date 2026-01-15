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
extern float32_t _cst_16[2304];
extern float32_t _cst_17[16];
extern float32_t _cst_18[2304];
extern float32_t _cst_19[16];
extern float32_t _cst_20[2304];
extern float32_t _cst_21[16];
extern float32_t _cst_22[2304];
extern float32_t _cst_23[16];
extern float32_t _cst_24[2304];
extern float32_t _cst_25[16];
extern float32_t _cst_26[2304];
extern float32_t _cst_27[16];
extern float32_t _cst_28[2304];
extern float32_t _cst_29[16];
extern float32_t _cst_30[2304];
extern float32_t _cst_31[16];
extern float32_t _cst_32[4608];
extern float32_t _cst_33[32];
extern float32_t _cst_34[9216];
extern float32_t _cst_35[32];
extern float32_t _cst_36[512];
extern float32_t _cst_37[32];
extern float32_t _cst_38[9216];
extern float32_t _cst_39[32];
extern float32_t _cst_40[9216];
extern float32_t _cst_41[32];
extern float32_t _cst_42[9216];
extern float32_t _cst_43[32];
extern float32_t _cst_44[9216];
extern float32_t _cst_45[32];
extern float32_t _cst_46[9216];
extern float32_t _cst_47[32];
extern float32_t _cst_48[9216];
extern float32_t _cst_49[32];
extern float32_t _cst_50[9216];
extern float32_t _cst_51[32];
extern float32_t _cst_52[9216];
extern float32_t _cst_53[32];
extern float32_t _cst_54[9216];
extern float32_t _cst_55[32];
extern float32_t _cst_56[9216];
extern float32_t _cst_57[32];
extern float32_t _cst_58[9216];
extern float32_t _cst_59[32];
extern float32_t _cst_60[9216];
extern float32_t _cst_61[32];
extern float32_t _cst_62[18432];
extern float32_t _cst_63[64];
extern float32_t _cst_64[36864];
extern float32_t _cst_65[64];
extern float32_t _cst_66[2048];
extern float32_t _cst_67[64];
extern float32_t _cst_68[36864];
extern float32_t _cst_69[64];
extern float32_t _cst_70[36864];
extern float32_t _cst_71[64];
extern float32_t _cst_72[36864];
extern float32_t _cst_73[64];
extern float32_t _cst_74[36864];
extern float32_t _cst_75[64];
extern float32_t _cst_76[36864];
extern float32_t _cst_77[64];
extern float32_t _cst_78[36864];
extern float32_t _cst_79[64];
extern float32_t _cst_80[36864];
extern float32_t _cst_81[64];
extern float32_t _cst_82[36864];
extern float32_t _cst_83[64];
extern float32_t _cst_84[36864];
extern float32_t _cst_85[64];
extern float32_t _cst_86[36864];
extern float32_t _cst_87[64];
extern float32_t _cst_88[36864];
extern float32_t _cst_89[64];
extern float32_t _cst_90[36864];
extern float32_t _cst_91[64];
extern int64_t _cst_92[2];
extern int64_t _cst_93[2];
extern int64_t _cst_94[2];
extern int64_t _cst_95[2];
extern int64_t _cst_96[2];
extern int64_t _cst_97[2];
extern int64_t _cst_98[2];
extern int64_t _cst_99[2];
extern int64_t _cst_100[2];
extern int64_t _cst_101[2];
extern int64_t _cst_102[2];
extern int64_t _cst_103[2];
extern int64_t _cst_104[2];
extern int32_t _cst_106[1];
extern float32_t _cst_107[589824];
extern float32_t _cst_108[16384];
extern int32_t _cst_109[9];
extern float32_t _cst_110[16384];
extern int32_t _cst_112[1];
extern float32_t _cst_113[2359296];
extern float32_t _cst_114[16384];
extern int32_t _cst_115[9];
extern float32_t _cst_116[16384];
extern int32_t _cst_118[1];
extern float32_t _cst_119[2359296];
extern float32_t _cst_120[16384];
extern int32_t _cst_121[9];
extern float32_t _cst_122[16384];
extern int32_t _cst_125[1];
extern float32_t _cst_126[2359296];
extern float32_t _cst_127[16384];
extern int32_t _cst_128[9];
extern float32_t _cst_129[16384];
extern int32_t _cst_131[1];
extern float32_t _cst_132[2359296];
extern float32_t _cst_133[16384];
extern int32_t _cst_134[9];
extern float32_t _cst_135[16384];
extern int32_t _cst_138[1];
extern float32_t _cst_139[2359296];
extern float32_t _cst_140[16384];
extern int32_t _cst_141[9];
extern float32_t _cst_142[16384];
extern int32_t _cst_144[1];
extern float32_t _cst_145[2359296];
extern float32_t _cst_146[16384];
extern int32_t _cst_147[9];
extern float32_t _cst_148[16384];
extern int32_t _cst_151[1];
extern float32_t _cst_152[2359296];
extern float32_t _cst_153[16384];
extern int32_t _cst_154[9];
extern float32_t _cst_155[16384];
extern int32_t _cst_157[1];
extern float32_t _cst_158[2359296];
extern float32_t _cst_159[16384];
extern int32_t _cst_160[9];
extern float32_t _cst_161[16384];
extern int32_t _cst_164[1];
extern float32_t _cst_165[2359296];
extern float32_t _cst_166[16384];
extern int32_t _cst_167[9];
extern float32_t _cst_168[16384];
extern int32_t _cst_170[1];
extern float32_t _cst_171[2359296];
extern float32_t _cst_172[16384];
extern int32_t _cst_173[9];
extern float32_t _cst_174[16384];
extern int32_t _cst_177[1];
extern float32_t _cst_178[2359296];
extern float32_t _cst_179[16384];
extern int32_t _cst_180[9];
extern float32_t _cst_181[16384];
extern int32_t _cst_183[1];
extern float32_t _cst_184[2359296];
extern float32_t _cst_185[16384];
extern int32_t _cst_186[9];
extern float32_t _cst_187[16384];
extern int32_t _cst_190[1];
extern float32_t _cst_191[2359296];
extern float32_t _cst_192[16384];
extern int32_t _cst_193[9];
extern float32_t _cst_194[16384];
extern int32_t _cst_196[1];
extern float32_t _cst_197[2359296];
extern float32_t _cst_198[16384];
extern int32_t _cst_199[9];
extern float32_t _cst_200[16384];
extern int32_t _cst_203[1];
extern float32_t _cst_204[4718592];
extern float32_t _cst_205[32768];
extern int32_t _cst_206[9];
extern float32_t _cst_207[32768];
extern float32_t _cst_208[524288];
extern float32_t _cst_209[524288];
extern float32_t _cst_210[1048576];
extern int32_t _cst_212[1];
extern float32_t _cst_213[2359296];
extern float32_t _cst_214[8192];
extern int32_t _cst_215[9];
extern float32_t _cst_216[8192];
extern int32_t _cst_218[1];
extern float32_t _cst_219[524288];
extern float32_t _cst_220[32768];
extern int32_t _cst_221[1];
extern float32_t _cst_222[32768];
extern float32_t _cst_223[524288];
extern float32_t _cst_224[524288];
extern float32_t _cst_225[1048576];
extern int32_t _cst_228[1];
extern float32_t _cst_229[2359296];
extern float32_t _cst_230[8192];
extern int32_t _cst_231[9];
extern float32_t _cst_232[8192];
extern int32_t _cst_234[1];
extern float32_t _cst_235[2359296];
extern float32_t _cst_236[8192];
extern int32_t _cst_237[9];
extern float32_t _cst_238[8192];
extern int32_t _cst_241[1];
extern float32_t _cst_242[2359296];
extern float32_t _cst_243[8192];
extern int32_t _cst_244[9];
extern float32_t _cst_245[8192];
extern int32_t _cst_247[1];
extern float32_t _cst_248[2359296];
extern float32_t _cst_249[8192];
extern int32_t _cst_250[9];
extern float32_t _cst_251[8192];
extern int32_t _cst_254[1];
extern float32_t _cst_255[2359296];
extern float32_t _cst_256[8192];
extern int32_t _cst_257[9];
extern float32_t _cst_258[8192];
extern int32_t _cst_260[1];
extern float32_t _cst_261[2359296];
extern float32_t _cst_262[8192];
extern int32_t _cst_263[9];
extern float32_t _cst_264[8192];
extern int32_t _cst_267[1];
extern float32_t _cst_268[2359296];
extern float32_t _cst_269[8192];
extern int32_t _cst_270[9];
extern float32_t _cst_271[8192];
extern int32_t _cst_273[1];
extern float32_t _cst_274[2359296];
extern float32_t _cst_275[8192];
extern int32_t _cst_276[9];
extern float32_t _cst_277[8192];
extern int32_t _cst_280[1];
extern float32_t _cst_281[2359296];
extern float32_t _cst_282[8192];
extern int32_t _cst_283[9];
extern float32_t _cst_284[8192];
extern int32_t _cst_286[1];
extern float32_t _cst_287[2359296];
extern float32_t _cst_288[8192];
extern int32_t _cst_289[9];
extern float32_t _cst_290[8192];
extern int32_t _cst_293[1];
extern float32_t _cst_294[2359296];
extern float32_t _cst_295[8192];
extern int32_t _cst_296[9];
extern float32_t _cst_297[8192];
extern int32_t _cst_299[1];
extern float32_t _cst_300[2359296];
extern float32_t _cst_301[8192];
extern int32_t _cst_302[9];
extern float32_t _cst_303[8192];
extern int32_t _cst_306[1];
extern float32_t _cst_307[4718592];
extern float32_t _cst_308[16384];
extern int32_t _cst_309[9];
extern float32_t _cst_310[16384];
extern float32_t _cst_311[131072];
extern float32_t _cst_312[131072];
extern float32_t _cst_313[1048576];
extern int32_t _cst_315[1];
extern float32_t _cst_316[2359296];
extern float32_t _cst_317[4096];
extern int32_t _cst_318[9];
extern float32_t _cst_319[4096];
extern int32_t _cst_321[1];
extern float32_t _cst_322[524288];
extern float32_t _cst_323[16384];
extern int32_t _cst_324[1];
extern float32_t _cst_325[16384];
extern float32_t _cst_326[131072];
extern float32_t _cst_327[131072];
extern float32_t _cst_328[1048576];
extern int32_t _cst_331[1];
extern float32_t _cst_332[2359296];
extern float32_t _cst_333[4096];
extern int32_t _cst_334[9];
extern float32_t _cst_335[4096];
extern int32_t _cst_337[1];
extern float32_t _cst_338[2359296];
extern float32_t _cst_339[4096];
extern int32_t _cst_340[9];
extern float32_t _cst_341[4096];
extern int32_t _cst_344[1];
extern float32_t _cst_345[2359296];
extern float32_t _cst_346[4096];
extern int32_t _cst_347[9];
extern float32_t _cst_348[4096];
extern int32_t _cst_350[1];
extern float32_t _cst_351[2359296];
extern float32_t _cst_352[4096];
extern int32_t _cst_353[9];
extern float32_t _cst_354[4096];
extern int32_t _cst_357[1];
extern float32_t _cst_358[2359296];
extern float32_t _cst_359[4096];
extern int32_t _cst_360[9];
extern float32_t _cst_361[4096];
extern int32_t _cst_363[1];
extern float32_t _cst_364[2359296];
extern float32_t _cst_365[4096];
extern int32_t _cst_366[9];
extern float32_t _cst_367[4096];
extern int32_t _cst_370[1];
extern float32_t _cst_371[2359296];
extern float32_t _cst_372[4096];
extern int32_t _cst_373[9];
extern float32_t _cst_374[4096];
extern int32_t _cst_376[1];
extern float32_t _cst_377[2359296];
extern float32_t _cst_378[4096];
extern int32_t _cst_379[9];
extern float32_t _cst_380[4096];
extern int32_t _cst_383[1];
extern float32_t _cst_384[2359296];
extern float32_t _cst_385[4096];
extern int32_t _cst_386[9];
extern float32_t _cst_387[4096];
extern int32_t _cst_389[1];
extern float32_t _cst_390[2359296];
extern float32_t _cst_391[4096];
extern int32_t _cst_392[9];
extern float32_t _cst_393[4096];
extern int32_t _cst_396[1];
extern float32_t _cst_397[2359296];
extern float32_t _cst_398[4096];
extern int32_t _cst_399[9];
extern float32_t _cst_400[4096];
extern int32_t _cst_402[1];
extern float32_t _cst_403[2359296];
extern float32_t _cst_404[4096];
extern int32_t _cst_405[9];
extern float32_t _cst_406[4096];
extern float32_t _cst_409[4096];
extern float32_t _cst_410[64];
extern float32_t _cst_413[700];
extern float32_t _cst_414[10];
float32_t _cst_415 = -1;
float32_t _cst_416 = -1;
float64_t _cst_417 = -3.809199432496630510769;
float64_t _cst_418 = 2.460940868770910228136;
float64_t _cst_419 = -1.430304794543664215212;
float64_t _cst_420 = 2.944661935353764636858;
float32_t _cst_421 = -1;
float32_t _cst_422 = -1;
float32_t _cst_423 = -1;
float32_t _cst_424 = -1;
float64_t _cst_425 = -0.0001600280537250875771867;
float64_t _cst_426 = 0.003032478474127557870749;
float64_t _cst_427 = 0.1365828361194337536855;
float64_t _cst_428 = -0.02219735858216702675816;
float64_t _cst_429 = 0.5685231134608953462717;
float64_t _cst_430 = -0.5558785316215136829143;
float64_t _cst_431 = 1.614771079042867985009;
float64_t _cst_432 = -0.6164083583294512891371;
float32_t _cst_433 = -1;
float32_t _cst_434 = -1;
float32_t _cst_435 = -1;
float32_t _cst_436 = -1;
float64_t _cst_437 = 0.0001111119162030077897477;
float64_t _cst_438 = 0.006507995190210730772351;
float64_t _cst_439 = -0.0009371069270347270278171;
float64_t _cst_440 = 0.1056744718173614761003;
float64_t _cst_441 = -0.03691063940349948585551;
float64_t _cst_442 = 0.6334139843709288841822;
float64_t _cst_443 = -0.2078597158810965850329;
float32_t _cst_444 = 0.5;
float32_t _cst_445 = 0.25;
float32_t _cst_446 = 0.5;
float32_t _cst_447 = 0.2000000000000000111022;
float32_t _cst_448 = 0.5;
float32_t _cst_449 = 0.2000000000000000111022;
float32_t _cst_450 = 0.5;
float32_t _cst_451 = 0.2000000000000000111022;
float32_t _cst_452 = 0.5;
float32_t _cst_453 = 0.1666666666666666574148;
float32_t _cst_454 = 0.5;
float32_t _cst_455 = 0.1666666666666666574148;
float32_t _cst_456 = 0.5;
float32_t _cst_457 = 0.1428571428571428492127;
float32_t _cst_458 = 0.5;
float32_t _cst_459 = 0.1666666666666666574148;
float32_t _cst_460 = 0.5;
float32_t _cst_461 = 0.2000000000000000111022;
float32_t _cst_462 = 0.5;
float32_t _cst_463 = 0.2000000000000000111022;
float32_t _cst_464 = 0.5;
float32_t _cst_465 = 0.2000000000000000111022;
float32_t _cst_466 = 0.5;
float32_t _cst_467 = 0.2000000000000000111022;
float32_t _cst_468 = 0.5;
float32_t _cst_469 = 0.2000000000000000111022;
float32_t _cst_470 = 0.5;
float32_t _cst_471 = 0.1666666666666666574148;
float32_t _cst_472 = 0.5;
float32_t _cst_473 = 0.1428571428571428492127;
float32_t _cst_474 = 0.5;
float32_t _cst_475 = 0.2000000000000000111022;
float32_t _cst_476 = 0.5;
float32_t _cst_477 = 0.2000000000000000111022;
float32_t _cst_478 = 0.5;
float32_t _cst_479 = 0.1666666666666666574148;
float32_t _cst_480 = 0.5;
float32_t _cst_481 = 0.1428571428571428492127;
float32_t _cst_482 = 0.5;
float32_t _cst_483 = 0.1111111111111111049432;
float32_t _cst_484 = 0.5;
float32_t _cst_485 = 0.06666666666666666574148;
float32_t _cst_486 = 0.5;
float32_t _cst_487 = 0.0625;
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
  CIPHERTEXT result_n9;
  CIPHERTEXT input_dup_n9;
  int32_t index_khw1_n9;
  CIPHERTEXT input_roll_n9[9];
  int32_t index_cin_n9;
  CIPHERTEXT result_cin_n9;
  int32_t index_khw2_n9;
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
  CIPHERTEXT input_roll_n11[9];
  int32_t index_cin_n11;
  CIPHERTEXT result_cin_n11;
  int32_t index_khw2_n11;
  CIPHERTEXT result_n12;
  CIPHERTEXT input_dup_n12;
  int32_t index_khw1_n12;
  CIPHERTEXT input_roll_n12[9];
  int32_t index_cin_n12;
  CIPHERTEXT result_cin_n12;
  int32_t index_khw2_n12;
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
  CIPHERTEXT orig_result16;
  CIPHERTEXT comb_row_result17;
  int32_t combine_row_index_n17;
  CIPHERTEXT comb_rc_result17;
  int32_t combine_rc_index_n17;
  CIPHERTEXT final_result17;
  int32_t combine_cc_index_n17;
  CIPHERTEXT result_n18;
  CIPHERTEXT input_dup_n18;
  int32_t index_khw1_n18;
  CIPHERTEXT input_roll_n18[9];
  int32_t index_cin_n18;
  CIPHERTEXT result_cin_n18;
  int32_t index_khw2_n18;
  CIPHERTEXT result_n19;
  CIPHERTEXT input_dup_n19;
  int32_t index_khw1_n19;
  CIPHERTEXT input_roll_n19[1];
  int32_t index_cin_n19;
  CIPHERTEXT result_cin_n19;
  int32_t index_khw2_n19;
  CIPHERTEXT orig_result19;
  CIPHERTEXT comb_row_result20;
  int32_t combine_row_index_n20;
  CIPHERTEXT comb_rc_result20;
  int32_t combine_rc_index_n20;
  CIPHERTEXT final_result20;
  int32_t combine_cc_index_n20;
  CIPHERTEXT result_n21;
  CIPHERTEXT input_dup_n21;
  int32_t index_khw1_n21;
  CIPHERTEXT input_roll_n21[9];
  int32_t index_cin_n21;
  CIPHERTEXT result_cin_n21;
  int32_t index_khw2_n21;
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
  CIPHERTEXT result_n26;
  CIPHERTEXT input_dup_n26;
  int32_t index_khw1_n26;
  CIPHERTEXT input_roll_n26[9];
  int32_t index_cin_n26;
  CIPHERTEXT result_cin_n26;
  int32_t index_khw2_n26;
  CIPHERTEXT result_n27;
  CIPHERTEXT input_dup_n27;
  int32_t index_khw1_n27;
  CIPHERTEXT input_roll_n27[9];
  int32_t index_cin_n27;
  CIPHERTEXT result_cin_n27;
  int32_t index_khw2_n27;
  CIPHERTEXT result_n28;
  CIPHERTEXT input_dup_n28;
  int32_t index_khw1_n28;
  CIPHERTEXT input_roll_n28[9];
  int32_t index_cin_n28;
  CIPHERTEXT result_cin_n28;
  int32_t index_khw2_n28;
  CIPHERTEXT result_n29;
  CIPHERTEXT input_dup_n29;
  int32_t index_khw1_n29;
  CIPHERTEXT input_roll_n29[9];
  int32_t index_cin_n29;
  CIPHERTEXT result_cin_n29;
  int32_t index_khw2_n29;
  CIPHERTEXT result_n30;
  CIPHERTEXT input_dup_n30;
  int32_t index_khw1_n30;
  CIPHERTEXT input_roll_n30[9];
  int32_t index_cin_n30;
  CIPHERTEXT result_cin_n30;
  int32_t index_khw2_n30;
  CIPHERTEXT result_n31;
  CIPHERTEXT input_dup_n31;
  int32_t index_khw1_n31;
  CIPHERTEXT input_roll_n31[9];
  int32_t index_cin_n31;
  CIPHERTEXT result_cin_n31;
  int32_t index_khw2_n31;
  CIPHERTEXT result_n32;
  CIPHERTEXT input_dup_n32;
  int32_t index_khw1_n32;
  CIPHERTEXT input_roll_n32[9];
  int32_t index_cin_n32;
  CIPHERTEXT result_cin_n32;
  int32_t index_khw2_n32;
  CIPHERTEXT result_n33;
  CIPHERTEXT input_dup_n33;
  int32_t index_khw1_n33;
  CIPHERTEXT input_roll_n33[9];
  int32_t index_cin_n33;
  CIPHERTEXT result_cin_n33;
  int32_t index_khw2_n33;
  CIPHERTEXT orig_result33;
  CIPHERTEXT comb_row_result34;
  int32_t combine_row_index_n34;
  CIPHERTEXT comb_rc_result34;
  int32_t combine_rc_index_n34;
  CIPHERTEXT final_result34;
  int32_t combine_cc_index_n34;
  CIPHERTEXT result_n35;
  CIPHERTEXT input_dup_n35;
  int32_t index_khw1_n35;
  CIPHERTEXT input_roll_n35[9];
  int32_t index_cin_n35;
  CIPHERTEXT result_cin_n35;
  int32_t index_khw2_n35;
  CIPHERTEXT result_n36;
  CIPHERTEXT input_dup_n36;
  int32_t index_khw1_n36;
  CIPHERTEXT input_roll_n36[1];
  int32_t index_cin_n36;
  CIPHERTEXT result_cin_n36;
  int32_t index_khw2_n36;
  CIPHERTEXT orig_result36;
  CIPHERTEXT comb_row_result37;
  int32_t combine_row_index_n37;
  CIPHERTEXT comb_rc_result37;
  int32_t combine_rc_index_n37;
  CIPHERTEXT final_result37;
  int32_t combine_cc_index_n37;
  CIPHERTEXT result_n38;
  CIPHERTEXT input_dup_n38;
  int32_t index_khw1_n38;
  CIPHERTEXT input_roll_n38[9];
  int32_t index_cin_n38;
  CIPHERTEXT result_cin_n38;
  int32_t index_khw2_n38;
  CIPHERTEXT result_n39;
  CIPHERTEXT input_dup_n39;
  int32_t index_khw1_n39;
  CIPHERTEXT input_roll_n39[9];
  int32_t index_cin_n39;
  CIPHERTEXT result_cin_n39;
  int32_t index_khw2_n39;
  CIPHERTEXT result_n40;
  CIPHERTEXT input_dup_n40;
  int32_t index_khw1_n40;
  CIPHERTEXT input_roll_n40[9];
  int32_t index_cin_n40;
  CIPHERTEXT result_cin_n40;
  int32_t index_khw2_n40;
  CIPHERTEXT result_n41;
  CIPHERTEXT input_dup_n41;
  int32_t index_khw1_n41;
  CIPHERTEXT input_roll_n41[9];
  int32_t index_cin_n41;
  CIPHERTEXT result_cin_n41;
  int32_t index_khw2_n41;
  CIPHERTEXT result_n42;
  CIPHERTEXT input_dup_n42;
  int32_t index_khw1_n42;
  CIPHERTEXT input_roll_n42[9];
  int32_t index_cin_n42;
  CIPHERTEXT result_cin_n42;
  int32_t index_khw2_n42;
  CIPHERTEXT result_n43;
  CIPHERTEXT input_dup_n43;
  int32_t index_khw1_n43;
  CIPHERTEXT input_roll_n43[9];
  int32_t index_cin_n43;
  CIPHERTEXT result_cin_n43;
  int32_t index_khw2_n43;
  CIPHERTEXT result_n44;
  CIPHERTEXT input_dup_n44;
  int32_t index_khw1_n44;
  CIPHERTEXT input_roll_n44[9];
  int32_t index_cin_n44;
  CIPHERTEXT result_cin_n44;
  int32_t index_khw2_n44;
  CIPHERTEXT result_n45;
  CIPHERTEXT input_dup_n45;
  int32_t index_khw1_n45;
  CIPHERTEXT input_roll_n45[9];
  int32_t index_cin_n45;
  CIPHERTEXT result_cin_n45;
  int32_t index_khw2_n45;
  CIPHERTEXT result_n46;
  CIPHERTEXT input_dup_n46;
  int32_t index_khw1_n46;
  CIPHERTEXT input_roll_n46[9];
  int32_t index_cin_n46;
  CIPHERTEXT result_cin_n46;
  int32_t index_khw2_n46;
  CIPHERTEXT result_n47;
  CIPHERTEXT input_dup_n47;
  int32_t index_khw1_n47;
  CIPHERTEXT input_roll_n47[9];
  int32_t index_cin_n47;
  CIPHERTEXT result_cin_n47;
  int32_t index_khw2_n47;
  CIPHERTEXT result_n48;
  CIPHERTEXT input_dup_n48;
  int32_t index_khw1_n48;
  CIPHERTEXT input_roll_n48[9];
  int32_t index_cin_n48;
  CIPHERTEXT result_cin_n48;
  int32_t index_khw2_n48;
  CIPHERTEXT result_n49;
  CIPHERTEXT input_dup_n49;
  int32_t index_khw1_n49;
  CIPHERTEXT input_roll_n49[9];
  int32_t index_cin_n49;
  CIPHERTEXT result_cin_n49;
  int32_t index_khw2_n49;
  int32_t sum_index_n49;
  CIPHERTEXT gap_result49;
  int32_t combine_cc_index_n49;
  CIPHERTEXT tmp_result_n50;
  CIPHERTEXT input_dup_n50;
  int32_t index_gemm_n50;
  int32_t index_add_n50;
  CIPHERTEXT output;
  CIPHERTEXT _ckks_gen_tmp_1793;
  CIPHERTEXT _ckks_gen_tmp_1824;
  CIPHERTEXT _ckks_gen_tmp_1855;
  CIPHERTEXT _ckks_gen_tmp_2094;
  CIPHERTEXT _ckks_gen_tmp_2125;
  CIPHERTEXT _ckks_gen_tmp_2156;
  CIPHERTEXT _ckks_gen_tmp_3639;
  CIPHERTEXT _ckks_gen_tmp_3670;
  CIPHERTEXT _ckks_gen_tmp_3701;
  CIPHERTEXT _ckks_gen_tmp_3944;
  CIPHERTEXT _ckks_gen_tmp_3975;
  CIPHERTEXT _ckks_gen_tmp_4006;
  CIPHERTEXT _ckks_gen_tmp_5455;
  CIPHERTEXT _rescale_tmp_65;
  CIPHERTEXT _rescale_tmp_5514;
  CIPHERTEXT _rescale_tmp_173;
  CIPHERTEXT _rescale_tmp_5527;
  CIPHERTEXT _rescale_tmp_281;
  CIPHERTEXT _rescale_tmp_5540;
  CIPHERTEXT _rescale_tmp_395;
  CIPHERTEXT _rescale_tmp_5553;
  CIPHERTEXT _rescale_tmp_503;
  CIPHERTEXT _rescale_tmp_5566;
  CIPHERTEXT _rescale_tmp_617;
  CIPHERTEXT _rescale_tmp_5579;
  CIPHERTEXT _rescale_tmp_725;
  CIPHERTEXT _rescale_tmp_5592;
  CIPHERTEXT _rescale_tmp_839;
  CIPHERTEXT _rescale_tmp_5605;
  CIPHERTEXT _rescale_tmp_947;
  CIPHERTEXT _rescale_tmp_5618;
  CIPHERTEXT _rescale_tmp_1061;
  CIPHERTEXT _rescale_tmp_5631;
  CIPHERTEXT _rescale_tmp_1169;
  CIPHERTEXT _rescale_tmp_5644;
  CIPHERTEXT _rescale_tmp_1283;
  CIPHERTEXT _rescale_tmp_5657;
  CIPHERTEXT _rescale_tmp_1391;
  CIPHERTEXT _rescale_tmp_5670;
  CIPHERTEXT _rescale_tmp_1505;
  CIPHERTEXT _rescale_tmp_5683;
  CIPHERTEXT _rescale_tmp_1613;
  CIPHERTEXT _rescale_tmp_5696;
  CIPHERTEXT _rescale_tmp_1731;
  CIPHERTEXT _rescale_tmp_5709;
  CIPHERTEXT _rescale_tmp_5715;
  CIPHERTEXT _rescale_tmp_5721;
  CIPHERTEXT _rescale_tmp_5727;
  CIPHERTEXT _rescale_tmp_1930;
  CIPHERTEXT _rescale_tmp_5740;
  CIPHERTEXT _rescale_tmp_2032;
  CIPHERTEXT _rescale_tmp_5751;
  CIPHERTEXT _rescale_tmp_5757;
  CIPHERTEXT _rescale_tmp_5763;
  CIPHERTEXT _rescale_tmp_5769;
  CIPHERTEXT _rescale_tmp_2237;
  CIPHERTEXT _rescale_tmp_5782;
  CIPHERTEXT _rescale_tmp_2345;
  CIPHERTEXT _rescale_tmp_5795;
  CIPHERTEXT _rescale_tmp_2459;
  CIPHERTEXT _rescale_tmp_5808;
  CIPHERTEXT _rescale_tmp_2567;
  CIPHERTEXT _rescale_tmp_5821;
  CIPHERTEXT _rescale_tmp_2681;
  CIPHERTEXT _rescale_tmp_5834;
  CIPHERTEXT _rescale_tmp_2789;
  CIPHERTEXT _rescale_tmp_5847;
  CIPHERTEXT _rescale_tmp_2903;
  CIPHERTEXT _rescale_tmp_5860;
  CIPHERTEXT _rescale_tmp_3011;
  CIPHERTEXT _rescale_tmp_5873;
  CIPHERTEXT _rescale_tmp_3125;
  CIPHERTEXT _rescale_tmp_5886;
  CIPHERTEXT _rescale_tmp_3233;
  CIPHERTEXT _rescale_tmp_5899;
  CIPHERTEXT _rescale_tmp_3347;
  CIPHERTEXT _rescale_tmp_5912;
  CIPHERTEXT _rescale_tmp_3455;
  CIPHERTEXT _rescale_tmp_5925;
  CIPHERTEXT _rescale_tmp_3573;
  CIPHERTEXT _rescale_tmp_5938;
  CIPHERTEXT _rescale_tmp_5944;
  CIPHERTEXT _rescale_tmp_5950;
  CIPHERTEXT _rescale_tmp_5956;
  CIPHERTEXT _rescale_tmp_3776;
  CIPHERTEXT _rescale_tmp_5969;
  CIPHERTEXT _rescale_tmp_3878;
  CIPHERTEXT _rescale_tmp_5980;
  CIPHERTEXT _rescale_tmp_5986;
  CIPHERTEXT _rescale_tmp_5992;
  CIPHERTEXT _rescale_tmp_5998;
  CIPHERTEXT _rescale_tmp_4087;
  CIPHERTEXT _rescale_tmp_6011;
  CIPHERTEXT _rescale_tmp_4195;
  CIPHERTEXT _rescale_tmp_6024;
  CIPHERTEXT _rescale_tmp_4309;
  CIPHERTEXT _rescale_tmp_6037;
  CIPHERTEXT _rescale_tmp_4417;
  CIPHERTEXT _rescale_tmp_6050;
  CIPHERTEXT _rescale_tmp_4531;
  CIPHERTEXT _rescale_tmp_6063;
  CIPHERTEXT _rescale_tmp_4639;
  CIPHERTEXT _rescale_tmp_6076;
  CIPHERTEXT _rescale_tmp_4753;
  CIPHERTEXT _rescale_tmp_6089;
  CIPHERTEXT _rescale_tmp_4861;
  CIPHERTEXT _rescale_tmp_6102;
  CIPHERTEXT _rescale_tmp_4975;
  CIPHERTEXT _rescale_tmp_6115;
  CIPHERTEXT _rescale_tmp_5083;
  CIPHERTEXT _rescale_tmp_6128;
  CIPHERTEXT _rescale_tmp_5197;
  CIPHERTEXT _rescale_tmp_6141;
  CIPHERTEXT _rescale_tmp_5305;
  CIPHERTEXT _rescale_tmp_6154;
  CIPHERTEXT _rescale_tmp_5390;
  CIPHERTEXT _rescale_tmp_6167;
  CIPHERTEXT _rescale_tmp_6173;
  CIPHERTEXT _rescale_tmp_6179;
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
  CIPHERTEXT _pgen_tmp_67;
  PLAINTEXT _pgen_tmp_68;
  PLAINTEXT _pgen_tmp_69;
  CIPHERTEXT _pgen_tmp_70;
  CIPHERTEXT _pgen_tmp_71;
  PLAINTEXT _pgen_tmp_72;
  CIPHERTEXT _pgen_tmp_73;
  PLAINTEXT _pgen_tmp_74;
  CIPHERTEXT _pgen_tmp_75;
  CIPHERTEXT _pgen_tmp_76;
  PLAINTEXT _pgen_tmp_77;
  PLAINTEXT _pgen_tmp_78;
  CIPHERTEXT _pgen_tmp_79;
  CIPHERTEXT _pgen_tmp_80;
  PLAINTEXT _pgen_tmp_81;
  CIPHERTEXT _pgen_tmp_82;
  PLAINTEXT _pgen_tmp_83;
  CIPHERTEXT _pgen_tmp_84;
  CIPHERTEXT _pgen_tmp_85;
  PLAINTEXT _pgen_tmp_86;
  PLAINTEXT _pgen_tmp_87;
  CIPHERTEXT _pgen_tmp_88;
  CIPHERTEXT _pgen_tmp_89;
  PLAINTEXT _pgen_tmp_90;
  CIPHERTEXT _pgen_tmp_91;
  PLAINTEXT _pgen_tmp_92;
  CIPHERTEXT _pgen_tmp_93;
  CIPHERTEXT _pgen_tmp_94;
  PLAINTEXT _pgen_tmp_95;
  PLAINTEXT _pgen_tmp_96;
  CIPHERTEXT _pgen_tmp_97;
  CIPHERTEXT _pgen_tmp_98;
  PLAINTEXT _pgen_tmp_99;
  CIPHERTEXT _pgen_tmp_100;
  PLAINTEXT _pgen_tmp_101;
  CIPHERTEXT _pgen_tmp_102;
  CIPHERTEXT _pgen_tmp_103;
  PLAINTEXT _pgen_tmp_104;
  PLAINTEXT _pgen_tmp_105;
  CIPHERTEXT _pgen_tmp_106;
  CIPHERTEXT _pgen_tmp_107;
  PLAINTEXT _pgen_tmp_108;
  CIPHERTEXT _pgen_tmp_109;
  PLAINTEXT _pgen_tmp_110;
  CIPHERTEXT _pgen_tmp_111;
  CIPHERTEXT _pgen_tmp_112;
  PLAINTEXT _pgen_tmp_113;
  PLAINTEXT _pgen_tmp_114;
  CIPHERTEXT _pgen_tmp_115;
  CIPHERTEXT _pgen_tmp_116;
  PLAINTEXT _pgen_tmp_117;
  CIPHERTEXT _pgen_tmp_118;
  PLAINTEXT _pgen_tmp_119;
  CIPHERTEXT _pgen_tmp_120;
  CIPHERTEXT _pgen_tmp_121;
  PLAINTEXT _pgen_tmp_122;
  PLAINTEXT _pgen_tmp_123;
  CIPHERTEXT _pgen_tmp_124;
  CIPHERTEXT _pgen_tmp_125;
  PLAINTEXT _pgen_tmp_126;
  CIPHERTEXT _pgen_tmp_127;
  PLAINTEXT _pgen_tmp_128;
  CIPHERTEXT _pgen_tmp_129;
  CIPHERTEXT _pgen_tmp_130;
  PLAINTEXT _pgen_tmp_131;
  PLAINTEXT _pgen_tmp_132;
  CIPHERTEXT _pgen_tmp_133;
  CIPHERTEXT _pgen_tmp_134;
  PLAINTEXT _pgen_tmp_135;
  CIPHERTEXT _pgen_tmp_136;
  PLAINTEXT _pgen_tmp_137;
  CIPHERTEXT _pgen_tmp_138;
  PLAINTEXT _pgen_tmp_139;
  PLAINTEXT _pgen_tmp_140;
  PLAINTEXT _pgen_tmp_141;
  CIPHERTEXT _pgen_tmp_142;
  PLAINTEXT _pgen_tmp_143;
  CIPHERTEXT _pgen_tmp_144;
  PLAINTEXT _pgen_tmp_145;
  CIPHERTEXT _pgen_tmp_146;
  CIPHERTEXT _pgen_tmp_147;
  CIPHERTEXT _pgen_tmp_148;
  PLAINTEXT _pgen_tmp_149;
  CIPHERTEXT _pgen_tmp_150;
  PLAINTEXT _pgen_tmp_151;
  CIPHERTEXT _pgen_tmp_152;
  CIPHERTEXT _pgen_tmp_153;
  PLAINTEXT _pgen_tmp_154;
  PLAINTEXT _pgen_tmp_155;
  PLAINTEXT _pgen_tmp_156;
  CIPHERTEXT _pgen_tmp_157;
  PLAINTEXT _pgen_tmp_158;
  PLAINTEXT _pgen_tmp_159;
  PLAINTEXT _pgen_tmp_160;
  CIPHERTEXT _pgen_tmp_161;
  PLAINTEXT _pgen_tmp_162;
  CIPHERTEXT _pgen_tmp_163;
  PLAINTEXT _pgen_tmp_164;
  CIPHERTEXT _pgen_tmp_165;
  CIPHERTEXT _pgen_tmp_166;
  CIPHERTEXT _pgen_tmp_167;
  PLAINTEXT _pgen_tmp_168;
  CIPHERTEXT _pgen_tmp_169;
  PLAINTEXT _pgen_tmp_170;
  CIPHERTEXT _pgen_tmp_171;
  CIPHERTEXT _pgen_tmp_172;
  PLAINTEXT _pgen_tmp_173;
  PLAINTEXT _pgen_tmp_174;
  CIPHERTEXT _pgen_tmp_175;
  CIPHERTEXT _pgen_tmp_176;
  PLAINTEXT _pgen_tmp_177;
  CIPHERTEXT _pgen_tmp_178;
  PLAINTEXT _pgen_tmp_179;
  CIPHERTEXT _pgen_tmp_180;
  CIPHERTEXT _pgen_tmp_181;
  PLAINTEXT _pgen_tmp_182;
  PLAINTEXT _pgen_tmp_183;
  CIPHERTEXT _pgen_tmp_184;
  CIPHERTEXT _pgen_tmp_185;
  PLAINTEXT _pgen_tmp_186;
  CIPHERTEXT _pgen_tmp_187;
  PLAINTEXT _pgen_tmp_188;
  CIPHERTEXT _pgen_tmp_189;
  CIPHERTEXT _pgen_tmp_190;
  PLAINTEXT _pgen_tmp_191;
  PLAINTEXT _pgen_tmp_192;
  CIPHERTEXT _pgen_tmp_193;
  CIPHERTEXT _pgen_tmp_194;
  PLAINTEXT _pgen_tmp_195;
  CIPHERTEXT _pgen_tmp_196;
  PLAINTEXT _pgen_tmp_197;
  CIPHERTEXT _pgen_tmp_198;
  CIPHERTEXT _pgen_tmp_199;
  PLAINTEXT _pgen_tmp_200;
  PLAINTEXT _pgen_tmp_201;
  CIPHERTEXT _pgen_tmp_202;
  CIPHERTEXT _pgen_tmp_203;
  PLAINTEXT _pgen_tmp_204;
  CIPHERTEXT _pgen_tmp_205;
  PLAINTEXT _pgen_tmp_206;
  CIPHERTEXT _pgen_tmp_207;
  CIPHERTEXT _pgen_tmp_208;
  PLAINTEXT _pgen_tmp_209;
  PLAINTEXT _pgen_tmp_210;
  CIPHERTEXT _pgen_tmp_211;
  CIPHERTEXT _pgen_tmp_212;
  PLAINTEXT _pgen_tmp_213;
  CIPHERTEXT _pgen_tmp_214;
  PLAINTEXT _pgen_tmp_215;
  CIPHERTEXT _pgen_tmp_216;
  CIPHERTEXT _pgen_tmp_217;
  PLAINTEXT _pgen_tmp_218;
  PLAINTEXT _pgen_tmp_219;
  CIPHERTEXT _pgen_tmp_220;
  CIPHERTEXT _pgen_tmp_221;
  PLAINTEXT _pgen_tmp_222;
  CIPHERTEXT _pgen_tmp_223;
  PLAINTEXT _pgen_tmp_224;
  CIPHERTEXT _pgen_tmp_225;
  CIPHERTEXT _pgen_tmp_226;
  PLAINTEXT _pgen_tmp_227;
  PLAINTEXT _pgen_tmp_228;
  CIPHERTEXT _pgen_tmp_229;
  CIPHERTEXT _pgen_tmp_230;
  PLAINTEXT _pgen_tmp_231;
  CIPHERTEXT _pgen_tmp_232;
  PLAINTEXT _pgen_tmp_233;
  CIPHERTEXT _pgen_tmp_234;
  CIPHERTEXT _pgen_tmp_235;
  PLAINTEXT _pgen_tmp_236;
  PLAINTEXT _pgen_tmp_237;
  CIPHERTEXT _pgen_tmp_238;
  CIPHERTEXT _pgen_tmp_239;
  PLAINTEXT _pgen_tmp_240;
  CIPHERTEXT _pgen_tmp_241;
  PLAINTEXT _pgen_tmp_242;
  CIPHERTEXT _pgen_tmp_243;
  CIPHERTEXT _pgen_tmp_244;
  PLAINTEXT _pgen_tmp_245;
  PLAINTEXT _pgen_tmp_246;
  CIPHERTEXT _pgen_tmp_247;
  CIPHERTEXT _pgen_tmp_248;
  PLAINTEXT _pgen_tmp_249;
  CIPHERTEXT _pgen_tmp_250;
  PLAINTEXT _pgen_tmp_251;
  CIPHERTEXT _pgen_tmp_252;
  CIPHERTEXT _pgen_tmp_253;
  PLAINTEXT _pgen_tmp_254;
  PLAINTEXT _pgen_tmp_255;
  CIPHERTEXT _pgen_tmp_256;
  CIPHERTEXT _pgen_tmp_257;
  PLAINTEXT _pgen_tmp_258;
  CIPHERTEXT _pgen_tmp_259;
  PLAINTEXT _pgen_tmp_260;
  CIPHERTEXT _pgen_tmp_261;
  CIPHERTEXT _pgen_tmp_262;
  PLAINTEXT _pgen_tmp_263;
  PLAINTEXT _pgen_tmp_264;
  CIPHERTEXT _pgen_tmp_265;
  CIPHERTEXT _pgen_tmp_266;
  PLAINTEXT _pgen_tmp_267;
  CIPHERTEXT _pgen_tmp_268;
  PLAINTEXT _pgen_tmp_269;
  CIPHERTEXT _pgen_tmp_270;
  CIPHERTEXT _pgen_tmp_271;
  PLAINTEXT _pgen_tmp_272;
  PLAINTEXT _pgen_tmp_273;
  CIPHERTEXT _pgen_tmp_274;
  CIPHERTEXT _pgen_tmp_275;
  PLAINTEXT _pgen_tmp_276;
  CIPHERTEXT _pgen_tmp_277;
  PLAINTEXT _pgen_tmp_278;
  CIPHERTEXT _pgen_tmp_279;
  CIPHERTEXT _pgen_tmp_280;
  PLAINTEXT _pgen_tmp_281;
  PLAINTEXT _pgen_tmp_282;
  PLAINTEXT _pgen_tmp_283;
  CIPHERTEXT _pgen_tmp_284;
  PLAINTEXT _pgen_tmp_285;
  CIPHERTEXT _pgen_tmp_286;
  PLAINTEXT _pgen_tmp_287;
  CIPHERTEXT _pgen_tmp_288;
  CIPHERTEXT _pgen_tmp_289;
  CIPHERTEXT _pgen_tmp_290;
  PLAINTEXT _pgen_tmp_291;
  CIPHERTEXT _pgen_tmp_292;
  PLAINTEXT _pgen_tmp_293;
  CIPHERTEXT _pgen_tmp_294;
  CIPHERTEXT _pgen_tmp_295;
  PLAINTEXT _pgen_tmp_296;
  PLAINTEXT _pgen_tmp_297;
  PLAINTEXT _pgen_tmp_298;
  CIPHERTEXT _pgen_tmp_299;
  CIPHERTEXT _pgen_tmp_300;
  PLAINTEXT _pgen_tmp_301;
  PLAINTEXT _pgen_tmp_302;
  PLAINTEXT _pgen_tmp_303;
  CIPHERTEXT _pgen_tmp_304;
  PLAINTEXT _pgen_tmp_305;
  CIPHERTEXT _pgen_tmp_306;
  PLAINTEXT _pgen_tmp_307;
  CIPHERTEXT _pgen_tmp_308;
  CIPHERTEXT _pgen_tmp_309;
  CIPHERTEXT _pgen_tmp_310;
  PLAINTEXT _pgen_tmp_311;
  CIPHERTEXT _pgen_tmp_312;
  PLAINTEXT _pgen_tmp_313;
  CIPHERTEXT _pgen_tmp_314;
  CIPHERTEXT _pgen_tmp_315;
  PLAINTEXT _pgen_tmp_316;
  PLAINTEXT _pgen_tmp_317;
  CIPHERTEXT _pgen_tmp_318;
  CIPHERTEXT _pgen_tmp_319;
  PLAINTEXT _pgen_tmp_320;
  CIPHERTEXT _pgen_tmp_321;
  PLAINTEXT _pgen_tmp_322;
  CIPHERTEXT _pgen_tmp_323;
  CIPHERTEXT _pgen_tmp_324;
  PLAINTEXT _pgen_tmp_325;
  PLAINTEXT _pgen_tmp_326;
  CIPHERTEXT _pgen_tmp_327;
  CIPHERTEXT _pgen_tmp_328;
  PLAINTEXT _pgen_tmp_329;
  CIPHERTEXT _pgen_tmp_330;
  PLAINTEXT _pgen_tmp_331;
  CIPHERTEXT _pgen_tmp_332;
  CIPHERTEXT _pgen_tmp_333;
  PLAINTEXT _pgen_tmp_334;
  PLAINTEXT _pgen_tmp_335;
  CIPHERTEXT _pgen_tmp_336;
  CIPHERTEXT _pgen_tmp_337;
  PLAINTEXT _pgen_tmp_338;
  CIPHERTEXT _pgen_tmp_339;
  PLAINTEXT _pgen_tmp_340;
  CIPHERTEXT _pgen_tmp_341;
  CIPHERTEXT _pgen_tmp_342;
  PLAINTEXT _pgen_tmp_343;
  PLAINTEXT _pgen_tmp_344;
  CIPHERTEXT _pgen_tmp_345;
  CIPHERTEXT _pgen_tmp_346;
  PLAINTEXT _pgen_tmp_347;
  CIPHERTEXT _pgen_tmp_348;
  PLAINTEXT _pgen_tmp_349;
  CIPHERTEXT _pgen_tmp_350;
  CIPHERTEXT _pgen_tmp_351;
  PLAINTEXT _pgen_tmp_352;
  PLAINTEXT _pgen_tmp_353;
  CIPHERTEXT _pgen_tmp_354;
  CIPHERTEXT _pgen_tmp_355;
  PLAINTEXT _pgen_tmp_356;
  CIPHERTEXT _pgen_tmp_357;
  PLAINTEXT _pgen_tmp_358;
  CIPHERTEXT _pgen_tmp_359;
  CIPHERTEXT _pgen_tmp_360;
  PLAINTEXT _pgen_tmp_361;
  PLAINTEXT _pgen_tmp_362;
  CIPHERTEXT _pgen_tmp_363;
  CIPHERTEXT _pgen_tmp_364;
  PLAINTEXT _pgen_tmp_365;
  CIPHERTEXT _pgen_tmp_366;
  PLAINTEXT _pgen_tmp_367;
  CIPHERTEXT _pgen_tmp_368;
  CIPHERTEXT _pgen_tmp_369;
  PLAINTEXT _pgen_tmp_370;
  PLAINTEXT _pgen_tmp_371;
  CIPHERTEXT _pgen_tmp_372;
  CIPHERTEXT _pgen_tmp_373;
  PLAINTEXT _pgen_tmp_374;
  CIPHERTEXT _pgen_tmp_375;
  PLAINTEXT _pgen_tmp_376;
  CIPHERTEXT _pgen_tmp_377;
  CIPHERTEXT _pgen_tmp_378;
  PLAINTEXT _pgen_tmp_379;
  PLAINTEXT _pgen_tmp_380;
  CIPHERTEXT _pgen_tmp_381;
  CIPHERTEXT _pgen_tmp_382;
  PLAINTEXT _pgen_tmp_383;
  CIPHERTEXT _pgen_tmp_384;
  PLAINTEXT _pgen_tmp_385;
  CIPHERTEXT _pgen_tmp_386;
  CIPHERTEXT _pgen_tmp_387;
  PLAINTEXT _pgen_tmp_388;
  PLAINTEXT _pgen_tmp_389;
  CIPHERTEXT _pgen_tmp_390;
  CIPHERTEXT _pgen_tmp_391;
  PLAINTEXT _pgen_tmp_392;
  CIPHERTEXT _pgen_tmp_393;
  PLAINTEXT _pgen_tmp_394;
  CIPHERTEXT _pgen_tmp_395;
  CIPHERTEXT _pgen_tmp_396;
  PLAINTEXT _pgen_tmp_397;
  PLAINTEXT _pgen_tmp_398;
  CIPHERTEXT _pgen_tmp_399;
  CIPHERTEXT _pgen_tmp_400;
  PLAINTEXT _pgen_tmp_401;
  CIPHERTEXT _pgen_tmp_402;
  PLAINTEXT _pgen_tmp_403;
  CIPHERTEXT _pgen_tmp_404;
  CIPHERTEXT _pgen_tmp_405;
  PLAINTEXT _pgen_tmp_406;
  PLAINTEXT _pgen_tmp_407;
  CIPHERTEXT _pgen_tmp_408;
  CIPHERTEXT _pgen_tmp_409;
  PLAINTEXT _pgen_tmp_410;
  CIPHERTEXT _pgen_tmp_411;
  PLAINTEXT _pgen_tmp_412;
  CIPHERTEXT _pgen_tmp_413;
  CIPHERTEXT _pgen_tmp_414;
  PLAINTEXT _pgen_tmp_415;
  PLAINTEXT _pgen_tmp_416;
  CIPHERTEXT _pgen_tmp_417;
  CIPHERTEXT _pgen_tmp_418;
  PLAINTEXT _pgen_tmp_419;
  CIPHERTEXT _pgen_tmp_420;
  PLAINTEXT _pgen_tmp_421;
  PLAINTEXT _pgen_tmp_422;
  PLAINTEXT _pgen_tmp_423;
  CIPHERTEXT _pgen_tmp_424;
  PLAINTEXT _pgen_tmp_425;
  PLAINTEXT _pgen_tmp_426;
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
  CIPHERTEXT _preg_268435798;
  CIPHERTEXT _preg_268435799;
  CIPHERTEXT _preg_268435800;
  CIPHERTEXT _preg_268435801;
  CIPHERTEXT _preg_268435802;
  CIPHERTEXT _preg_268435803;
  CIPHERTEXT _preg_268435804;
  CIPHERTEXT _preg_268435805;
  CIPHERTEXT _preg_268435806;
  CIPHERTEXT _preg_268435807;
  CIPHERTEXT _preg_268435808;
  CIPHERTEXT _preg_268435809;
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
  memset(&result_n9, 0, sizeof(result_n9));
  memset(&input_dup_n9, 0, sizeof(input_dup_n9));
  memset(&input_roll_n9, 0, sizeof(input_roll_n9));
  memset(&result_cin_n9, 0, sizeof(result_cin_n9));
  memset(&result_n10, 0, sizeof(result_n10));
  memset(&input_dup_n10, 0, sizeof(input_dup_n10));
  memset(&input_roll_n10, 0, sizeof(input_roll_n10));
  memset(&result_cin_n10, 0, sizeof(result_cin_n10));
  memset(&result_n11, 0, sizeof(result_n11));
  memset(&input_dup_n11, 0, sizeof(input_dup_n11));
  memset(&input_roll_n11, 0, sizeof(input_roll_n11));
  memset(&result_cin_n11, 0, sizeof(result_cin_n11));
  memset(&result_n12, 0, sizeof(result_n12));
  memset(&input_dup_n12, 0, sizeof(input_dup_n12));
  memset(&input_roll_n12, 0, sizeof(input_roll_n12));
  memset(&result_cin_n12, 0, sizeof(result_cin_n12));
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
  memset(&orig_result16, 0, sizeof(orig_result16));
  memset(&comb_row_result17, 0, sizeof(comb_row_result17));
  memset(&comb_rc_result17, 0, sizeof(comb_rc_result17));
  memset(&final_result17, 0, sizeof(final_result17));
  memset(&result_n18, 0, sizeof(result_n18));
  memset(&input_dup_n18, 0, sizeof(input_dup_n18));
  memset(&input_roll_n18, 0, sizeof(input_roll_n18));
  memset(&result_cin_n18, 0, sizeof(result_cin_n18));
  memset(&result_n19, 0, sizeof(result_n19));
  memset(&input_dup_n19, 0, sizeof(input_dup_n19));
  memset(&input_roll_n19, 0, sizeof(input_roll_n19));
  memset(&result_cin_n19, 0, sizeof(result_cin_n19));
  memset(&orig_result19, 0, sizeof(orig_result19));
  memset(&comb_row_result20, 0, sizeof(comb_row_result20));
  memset(&comb_rc_result20, 0, sizeof(comb_rc_result20));
  memset(&final_result20, 0, sizeof(final_result20));
  memset(&result_n21, 0, sizeof(result_n21));
  memset(&input_dup_n21, 0, sizeof(input_dup_n21));
  memset(&input_roll_n21, 0, sizeof(input_roll_n21));
  memset(&result_cin_n21, 0, sizeof(result_cin_n21));
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
  memset(&result_n26, 0, sizeof(result_n26));
  memset(&input_dup_n26, 0, sizeof(input_dup_n26));
  memset(&input_roll_n26, 0, sizeof(input_roll_n26));
  memset(&result_cin_n26, 0, sizeof(result_cin_n26));
  memset(&result_n27, 0, sizeof(result_n27));
  memset(&input_dup_n27, 0, sizeof(input_dup_n27));
  memset(&input_roll_n27, 0, sizeof(input_roll_n27));
  memset(&result_cin_n27, 0, sizeof(result_cin_n27));
  memset(&result_n28, 0, sizeof(result_n28));
  memset(&input_dup_n28, 0, sizeof(input_dup_n28));
  memset(&input_roll_n28, 0, sizeof(input_roll_n28));
  memset(&result_cin_n28, 0, sizeof(result_cin_n28));
  memset(&result_n29, 0, sizeof(result_n29));
  memset(&input_dup_n29, 0, sizeof(input_dup_n29));
  memset(&input_roll_n29, 0, sizeof(input_roll_n29));
  memset(&result_cin_n29, 0, sizeof(result_cin_n29));
  memset(&result_n30, 0, sizeof(result_n30));
  memset(&input_dup_n30, 0, sizeof(input_dup_n30));
  memset(&input_roll_n30, 0, sizeof(input_roll_n30));
  memset(&result_cin_n30, 0, sizeof(result_cin_n30));
  memset(&result_n31, 0, sizeof(result_n31));
  memset(&input_dup_n31, 0, sizeof(input_dup_n31));
  memset(&input_roll_n31, 0, sizeof(input_roll_n31));
  memset(&result_cin_n31, 0, sizeof(result_cin_n31));
  memset(&result_n32, 0, sizeof(result_n32));
  memset(&input_dup_n32, 0, sizeof(input_dup_n32));
  memset(&input_roll_n32, 0, sizeof(input_roll_n32));
  memset(&result_cin_n32, 0, sizeof(result_cin_n32));
  memset(&result_n33, 0, sizeof(result_n33));
  memset(&input_dup_n33, 0, sizeof(input_dup_n33));
  memset(&input_roll_n33, 0, sizeof(input_roll_n33));
  memset(&result_cin_n33, 0, sizeof(result_cin_n33));
  memset(&orig_result33, 0, sizeof(orig_result33));
  memset(&comb_row_result34, 0, sizeof(comb_row_result34));
  memset(&comb_rc_result34, 0, sizeof(comb_rc_result34));
  memset(&final_result34, 0, sizeof(final_result34));
  memset(&result_n35, 0, sizeof(result_n35));
  memset(&input_dup_n35, 0, sizeof(input_dup_n35));
  memset(&input_roll_n35, 0, sizeof(input_roll_n35));
  memset(&result_cin_n35, 0, sizeof(result_cin_n35));
  memset(&result_n36, 0, sizeof(result_n36));
  memset(&input_dup_n36, 0, sizeof(input_dup_n36));
  memset(&input_roll_n36, 0, sizeof(input_roll_n36));
  memset(&result_cin_n36, 0, sizeof(result_cin_n36));
  memset(&orig_result36, 0, sizeof(orig_result36));
  memset(&comb_row_result37, 0, sizeof(comb_row_result37));
  memset(&comb_rc_result37, 0, sizeof(comb_rc_result37));
  memset(&final_result37, 0, sizeof(final_result37));
  memset(&result_n38, 0, sizeof(result_n38));
  memset(&input_dup_n38, 0, sizeof(input_dup_n38));
  memset(&input_roll_n38, 0, sizeof(input_roll_n38));
  memset(&result_cin_n38, 0, sizeof(result_cin_n38));
  memset(&result_n39, 0, sizeof(result_n39));
  memset(&input_dup_n39, 0, sizeof(input_dup_n39));
  memset(&input_roll_n39, 0, sizeof(input_roll_n39));
  memset(&result_cin_n39, 0, sizeof(result_cin_n39));
  memset(&result_n40, 0, sizeof(result_n40));
  memset(&input_dup_n40, 0, sizeof(input_dup_n40));
  memset(&input_roll_n40, 0, sizeof(input_roll_n40));
  memset(&result_cin_n40, 0, sizeof(result_cin_n40));
  memset(&result_n41, 0, sizeof(result_n41));
  memset(&input_dup_n41, 0, sizeof(input_dup_n41));
  memset(&input_roll_n41, 0, sizeof(input_roll_n41));
  memset(&result_cin_n41, 0, sizeof(result_cin_n41));
  memset(&result_n42, 0, sizeof(result_n42));
  memset(&input_dup_n42, 0, sizeof(input_dup_n42));
  memset(&input_roll_n42, 0, sizeof(input_roll_n42));
  memset(&result_cin_n42, 0, sizeof(result_cin_n42));
  memset(&result_n43, 0, sizeof(result_n43));
  memset(&input_dup_n43, 0, sizeof(input_dup_n43));
  memset(&input_roll_n43, 0, sizeof(input_roll_n43));
  memset(&result_cin_n43, 0, sizeof(result_cin_n43));
  memset(&result_n44, 0, sizeof(result_n44));
  memset(&input_dup_n44, 0, sizeof(input_dup_n44));
  memset(&input_roll_n44, 0, sizeof(input_roll_n44));
  memset(&result_cin_n44, 0, sizeof(result_cin_n44));
  memset(&result_n45, 0, sizeof(result_n45));
  memset(&input_dup_n45, 0, sizeof(input_dup_n45));
  memset(&input_roll_n45, 0, sizeof(input_roll_n45));
  memset(&result_cin_n45, 0, sizeof(result_cin_n45));
  memset(&result_n46, 0, sizeof(result_n46));
  memset(&input_dup_n46, 0, sizeof(input_dup_n46));
  memset(&input_roll_n46, 0, sizeof(input_roll_n46));
  memset(&result_cin_n46, 0, sizeof(result_cin_n46));
  memset(&result_n47, 0, sizeof(result_n47));
  memset(&input_dup_n47, 0, sizeof(input_dup_n47));
  memset(&input_roll_n47, 0, sizeof(input_roll_n47));
  memset(&result_cin_n47, 0, sizeof(result_cin_n47));
  memset(&result_n48, 0, sizeof(result_n48));
  memset(&input_dup_n48, 0, sizeof(input_dup_n48));
  memset(&input_roll_n48, 0, sizeof(input_roll_n48));
  memset(&result_cin_n48, 0, sizeof(result_cin_n48));
  memset(&result_n49, 0, sizeof(result_n49));
  memset(&input_dup_n49, 0, sizeof(input_dup_n49));
  memset(&input_roll_n49, 0, sizeof(input_roll_n49));
  memset(&result_cin_n49, 0, sizeof(result_cin_n49));
  memset(&gap_result49, 0, sizeof(gap_result49));
  memset(&tmp_result_n50, 0, sizeof(tmp_result_n50));
  memset(&input_dup_n50, 0, sizeof(input_dup_n50));
  memset(&output, 0, sizeof(output));
  memset(&_ckks_gen_tmp_1793, 0, sizeof(_ckks_gen_tmp_1793));
  memset(&_ckks_gen_tmp_1824, 0, sizeof(_ckks_gen_tmp_1824));
  memset(&_ckks_gen_tmp_1855, 0, sizeof(_ckks_gen_tmp_1855));
  memset(&_ckks_gen_tmp_2094, 0, sizeof(_ckks_gen_tmp_2094));
  memset(&_ckks_gen_tmp_2125, 0, sizeof(_ckks_gen_tmp_2125));
  memset(&_ckks_gen_tmp_2156, 0, sizeof(_ckks_gen_tmp_2156));
  memset(&_ckks_gen_tmp_3639, 0, sizeof(_ckks_gen_tmp_3639));
  memset(&_ckks_gen_tmp_3670, 0, sizeof(_ckks_gen_tmp_3670));
  memset(&_ckks_gen_tmp_3701, 0, sizeof(_ckks_gen_tmp_3701));
  memset(&_ckks_gen_tmp_3944, 0, sizeof(_ckks_gen_tmp_3944));
  memset(&_ckks_gen_tmp_3975, 0, sizeof(_ckks_gen_tmp_3975));
  memset(&_ckks_gen_tmp_4006, 0, sizeof(_ckks_gen_tmp_4006));
  memset(&_ckks_gen_tmp_5455, 0, sizeof(_ckks_gen_tmp_5455));
  memset(&_rescale_tmp_65, 0, sizeof(_rescale_tmp_65));
  memset(&_rescale_tmp_5514, 0, sizeof(_rescale_tmp_5514));
  memset(&_rescale_tmp_173, 0, sizeof(_rescale_tmp_173));
  memset(&_rescale_tmp_5527, 0, sizeof(_rescale_tmp_5527));
  memset(&_rescale_tmp_281, 0, sizeof(_rescale_tmp_281));
  memset(&_rescale_tmp_5540, 0, sizeof(_rescale_tmp_5540));
  memset(&_rescale_tmp_395, 0, sizeof(_rescale_tmp_395));
  memset(&_rescale_tmp_5553, 0, sizeof(_rescale_tmp_5553));
  memset(&_rescale_tmp_503, 0, sizeof(_rescale_tmp_503));
  memset(&_rescale_tmp_5566, 0, sizeof(_rescale_tmp_5566));
  memset(&_rescale_tmp_617, 0, sizeof(_rescale_tmp_617));
  memset(&_rescale_tmp_5579, 0, sizeof(_rescale_tmp_5579));
  memset(&_rescale_tmp_725, 0, sizeof(_rescale_tmp_725));
  memset(&_rescale_tmp_5592, 0, sizeof(_rescale_tmp_5592));
  memset(&_rescale_tmp_839, 0, sizeof(_rescale_tmp_839));
  memset(&_rescale_tmp_5605, 0, sizeof(_rescale_tmp_5605));
  memset(&_rescale_tmp_947, 0, sizeof(_rescale_tmp_947));
  memset(&_rescale_tmp_5618, 0, sizeof(_rescale_tmp_5618));
  memset(&_rescale_tmp_1061, 0, sizeof(_rescale_tmp_1061));
  memset(&_rescale_tmp_5631, 0, sizeof(_rescale_tmp_5631));
  memset(&_rescale_tmp_1169, 0, sizeof(_rescale_tmp_1169));
  memset(&_rescale_tmp_5644, 0, sizeof(_rescale_tmp_5644));
  memset(&_rescale_tmp_1283, 0, sizeof(_rescale_tmp_1283));
  memset(&_rescale_tmp_5657, 0, sizeof(_rescale_tmp_5657));
  memset(&_rescale_tmp_1391, 0, sizeof(_rescale_tmp_1391));
  memset(&_rescale_tmp_5670, 0, sizeof(_rescale_tmp_5670));
  memset(&_rescale_tmp_1505, 0, sizeof(_rescale_tmp_1505));
  memset(&_rescale_tmp_5683, 0, sizeof(_rescale_tmp_5683));
  memset(&_rescale_tmp_1613, 0, sizeof(_rescale_tmp_1613));
  memset(&_rescale_tmp_5696, 0, sizeof(_rescale_tmp_5696));
  memset(&_rescale_tmp_1731, 0, sizeof(_rescale_tmp_1731));
  memset(&_rescale_tmp_5709, 0, sizeof(_rescale_tmp_5709));
  memset(&_rescale_tmp_5715, 0, sizeof(_rescale_tmp_5715));
  memset(&_rescale_tmp_5721, 0, sizeof(_rescale_tmp_5721));
  memset(&_rescale_tmp_5727, 0, sizeof(_rescale_tmp_5727));
  memset(&_rescale_tmp_1930, 0, sizeof(_rescale_tmp_1930));
  memset(&_rescale_tmp_5740, 0, sizeof(_rescale_tmp_5740));
  memset(&_rescale_tmp_2032, 0, sizeof(_rescale_tmp_2032));
  memset(&_rescale_tmp_5751, 0, sizeof(_rescale_tmp_5751));
  memset(&_rescale_tmp_5757, 0, sizeof(_rescale_tmp_5757));
  memset(&_rescale_tmp_5763, 0, sizeof(_rescale_tmp_5763));
  memset(&_rescale_tmp_5769, 0, sizeof(_rescale_tmp_5769));
  memset(&_rescale_tmp_2237, 0, sizeof(_rescale_tmp_2237));
  memset(&_rescale_tmp_5782, 0, sizeof(_rescale_tmp_5782));
  memset(&_rescale_tmp_2345, 0, sizeof(_rescale_tmp_2345));
  memset(&_rescale_tmp_5795, 0, sizeof(_rescale_tmp_5795));
  memset(&_rescale_tmp_2459, 0, sizeof(_rescale_tmp_2459));
  memset(&_rescale_tmp_5808, 0, sizeof(_rescale_tmp_5808));
  memset(&_rescale_tmp_2567, 0, sizeof(_rescale_tmp_2567));
  memset(&_rescale_tmp_5821, 0, sizeof(_rescale_tmp_5821));
  memset(&_rescale_tmp_2681, 0, sizeof(_rescale_tmp_2681));
  memset(&_rescale_tmp_5834, 0, sizeof(_rescale_tmp_5834));
  memset(&_rescale_tmp_2789, 0, sizeof(_rescale_tmp_2789));
  memset(&_rescale_tmp_5847, 0, sizeof(_rescale_tmp_5847));
  memset(&_rescale_tmp_2903, 0, sizeof(_rescale_tmp_2903));
  memset(&_rescale_tmp_5860, 0, sizeof(_rescale_tmp_5860));
  memset(&_rescale_tmp_3011, 0, sizeof(_rescale_tmp_3011));
  memset(&_rescale_tmp_5873, 0, sizeof(_rescale_tmp_5873));
  memset(&_rescale_tmp_3125, 0, sizeof(_rescale_tmp_3125));
  memset(&_rescale_tmp_5886, 0, sizeof(_rescale_tmp_5886));
  memset(&_rescale_tmp_3233, 0, sizeof(_rescale_tmp_3233));
  memset(&_rescale_tmp_5899, 0, sizeof(_rescale_tmp_5899));
  memset(&_rescale_tmp_3347, 0, sizeof(_rescale_tmp_3347));
  memset(&_rescale_tmp_5912, 0, sizeof(_rescale_tmp_5912));
  memset(&_rescale_tmp_3455, 0, sizeof(_rescale_tmp_3455));
  memset(&_rescale_tmp_5925, 0, sizeof(_rescale_tmp_5925));
  memset(&_rescale_tmp_3573, 0, sizeof(_rescale_tmp_3573));
  memset(&_rescale_tmp_5938, 0, sizeof(_rescale_tmp_5938));
  memset(&_rescale_tmp_5944, 0, sizeof(_rescale_tmp_5944));
  memset(&_rescale_tmp_5950, 0, sizeof(_rescale_tmp_5950));
  memset(&_rescale_tmp_5956, 0, sizeof(_rescale_tmp_5956));
  memset(&_rescale_tmp_3776, 0, sizeof(_rescale_tmp_3776));
  memset(&_rescale_tmp_5969, 0, sizeof(_rescale_tmp_5969));
  memset(&_rescale_tmp_3878, 0, sizeof(_rescale_tmp_3878));
  memset(&_rescale_tmp_5980, 0, sizeof(_rescale_tmp_5980));
  memset(&_rescale_tmp_5986, 0, sizeof(_rescale_tmp_5986));
  memset(&_rescale_tmp_5992, 0, sizeof(_rescale_tmp_5992));
  memset(&_rescale_tmp_5998, 0, sizeof(_rescale_tmp_5998));
  memset(&_rescale_tmp_4087, 0, sizeof(_rescale_tmp_4087));
  memset(&_rescale_tmp_6011, 0, sizeof(_rescale_tmp_6011));
  memset(&_rescale_tmp_4195, 0, sizeof(_rescale_tmp_4195));
  memset(&_rescale_tmp_6024, 0, sizeof(_rescale_tmp_6024));
  memset(&_rescale_tmp_4309, 0, sizeof(_rescale_tmp_4309));
  memset(&_rescale_tmp_6037, 0, sizeof(_rescale_tmp_6037));
  memset(&_rescale_tmp_4417, 0, sizeof(_rescale_tmp_4417));
  memset(&_rescale_tmp_6050, 0, sizeof(_rescale_tmp_6050));
  memset(&_rescale_tmp_4531, 0, sizeof(_rescale_tmp_4531));
  memset(&_rescale_tmp_6063, 0, sizeof(_rescale_tmp_6063));
  memset(&_rescale_tmp_4639, 0, sizeof(_rescale_tmp_4639));
  memset(&_rescale_tmp_6076, 0, sizeof(_rescale_tmp_6076));
  memset(&_rescale_tmp_4753, 0, sizeof(_rescale_tmp_4753));
  memset(&_rescale_tmp_6089, 0, sizeof(_rescale_tmp_6089));
  memset(&_rescale_tmp_4861, 0, sizeof(_rescale_tmp_4861));
  memset(&_rescale_tmp_6102, 0, sizeof(_rescale_tmp_6102));
  memset(&_rescale_tmp_4975, 0, sizeof(_rescale_tmp_4975));
  memset(&_rescale_tmp_6115, 0, sizeof(_rescale_tmp_6115));
  memset(&_rescale_tmp_5083, 0, sizeof(_rescale_tmp_5083));
  memset(&_rescale_tmp_6128, 0, sizeof(_rescale_tmp_6128));
  memset(&_rescale_tmp_5197, 0, sizeof(_rescale_tmp_5197));
  memset(&_rescale_tmp_6141, 0, sizeof(_rescale_tmp_6141));
  memset(&_rescale_tmp_5305, 0, sizeof(_rescale_tmp_5305));
  memset(&_rescale_tmp_6154, 0, sizeof(_rescale_tmp_6154));
  memset(&_rescale_tmp_5390, 0, sizeof(_rescale_tmp_5390));
  memset(&_rescale_tmp_6167, 0, sizeof(_rescale_tmp_6167));
  memset(&_rescale_tmp_6173, 0, sizeof(_rescale_tmp_6173));
  memset(&_rescale_tmp_6179, 0, sizeof(_rescale_tmp_6179));
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
  memset(&_pgen_tmp_211, 0, sizeof(_pgen_tmp_211));
  memset(&_pgen_tmp_212, 0, sizeof(_pgen_tmp_212));
  memset(&_pgen_tmp_213, 0, sizeof(_pgen_tmp_213));
  memset(&_pgen_tmp_214, 0, sizeof(_pgen_tmp_214));
  memset(&_pgen_tmp_215, 0, sizeof(_pgen_tmp_215));
  memset(&_pgen_tmp_216, 0, sizeof(_pgen_tmp_216));
  memset(&_pgen_tmp_217, 0, sizeof(_pgen_tmp_217));
  memset(&_pgen_tmp_218, 0, sizeof(_pgen_tmp_218));
  memset(&_pgen_tmp_219, 0, sizeof(_pgen_tmp_219));
  memset(&_pgen_tmp_220, 0, sizeof(_pgen_tmp_220));
  memset(&_pgen_tmp_221, 0, sizeof(_pgen_tmp_221));
  memset(&_pgen_tmp_222, 0, sizeof(_pgen_tmp_222));
  memset(&_pgen_tmp_223, 0, sizeof(_pgen_tmp_223));
  memset(&_pgen_tmp_224, 0, sizeof(_pgen_tmp_224));
  memset(&_pgen_tmp_225, 0, sizeof(_pgen_tmp_225));
  memset(&_pgen_tmp_226, 0, sizeof(_pgen_tmp_226));
  memset(&_pgen_tmp_227, 0, sizeof(_pgen_tmp_227));
  memset(&_pgen_tmp_228, 0, sizeof(_pgen_tmp_228));
  memset(&_pgen_tmp_229, 0, sizeof(_pgen_tmp_229));
  memset(&_pgen_tmp_230, 0, sizeof(_pgen_tmp_230));
  memset(&_pgen_tmp_231, 0, sizeof(_pgen_tmp_231));
  memset(&_pgen_tmp_232, 0, sizeof(_pgen_tmp_232));
  memset(&_pgen_tmp_233, 0, sizeof(_pgen_tmp_233));
  memset(&_pgen_tmp_234, 0, sizeof(_pgen_tmp_234));
  memset(&_pgen_tmp_235, 0, sizeof(_pgen_tmp_235));
  memset(&_pgen_tmp_236, 0, sizeof(_pgen_tmp_236));
  memset(&_pgen_tmp_237, 0, sizeof(_pgen_tmp_237));
  memset(&_pgen_tmp_238, 0, sizeof(_pgen_tmp_238));
  memset(&_pgen_tmp_239, 0, sizeof(_pgen_tmp_239));
  memset(&_pgen_tmp_240, 0, sizeof(_pgen_tmp_240));
  memset(&_pgen_tmp_241, 0, sizeof(_pgen_tmp_241));
  memset(&_pgen_tmp_242, 0, sizeof(_pgen_tmp_242));
  memset(&_pgen_tmp_243, 0, sizeof(_pgen_tmp_243));
  memset(&_pgen_tmp_244, 0, sizeof(_pgen_tmp_244));
  memset(&_pgen_tmp_245, 0, sizeof(_pgen_tmp_245));
  memset(&_pgen_tmp_246, 0, sizeof(_pgen_tmp_246));
  memset(&_pgen_tmp_247, 0, sizeof(_pgen_tmp_247));
  memset(&_pgen_tmp_248, 0, sizeof(_pgen_tmp_248));
  memset(&_pgen_tmp_249, 0, sizeof(_pgen_tmp_249));
  memset(&_pgen_tmp_250, 0, sizeof(_pgen_tmp_250));
  memset(&_pgen_tmp_251, 0, sizeof(_pgen_tmp_251));
  memset(&_pgen_tmp_252, 0, sizeof(_pgen_tmp_252));
  memset(&_pgen_tmp_253, 0, sizeof(_pgen_tmp_253));
  memset(&_pgen_tmp_254, 0, sizeof(_pgen_tmp_254));
  memset(&_pgen_tmp_255, 0, sizeof(_pgen_tmp_255));
  memset(&_pgen_tmp_256, 0, sizeof(_pgen_tmp_256));
  memset(&_pgen_tmp_257, 0, sizeof(_pgen_tmp_257));
  memset(&_pgen_tmp_258, 0, sizeof(_pgen_tmp_258));
  memset(&_pgen_tmp_259, 0, sizeof(_pgen_tmp_259));
  memset(&_pgen_tmp_260, 0, sizeof(_pgen_tmp_260));
  memset(&_pgen_tmp_261, 0, sizeof(_pgen_tmp_261));
  memset(&_pgen_tmp_262, 0, sizeof(_pgen_tmp_262));
  memset(&_pgen_tmp_263, 0, sizeof(_pgen_tmp_263));
  memset(&_pgen_tmp_264, 0, sizeof(_pgen_tmp_264));
  memset(&_pgen_tmp_265, 0, sizeof(_pgen_tmp_265));
  memset(&_pgen_tmp_266, 0, sizeof(_pgen_tmp_266));
  memset(&_pgen_tmp_267, 0, sizeof(_pgen_tmp_267));
  memset(&_pgen_tmp_268, 0, sizeof(_pgen_tmp_268));
  memset(&_pgen_tmp_269, 0, sizeof(_pgen_tmp_269));
  memset(&_pgen_tmp_270, 0, sizeof(_pgen_tmp_270));
  memset(&_pgen_tmp_271, 0, sizeof(_pgen_tmp_271));
  memset(&_pgen_tmp_272, 0, sizeof(_pgen_tmp_272));
  memset(&_pgen_tmp_273, 0, sizeof(_pgen_tmp_273));
  memset(&_pgen_tmp_274, 0, sizeof(_pgen_tmp_274));
  memset(&_pgen_tmp_275, 0, sizeof(_pgen_tmp_275));
  memset(&_pgen_tmp_276, 0, sizeof(_pgen_tmp_276));
  memset(&_pgen_tmp_277, 0, sizeof(_pgen_tmp_277));
  memset(&_pgen_tmp_278, 0, sizeof(_pgen_tmp_278));
  memset(&_pgen_tmp_279, 0, sizeof(_pgen_tmp_279));
  memset(&_pgen_tmp_280, 0, sizeof(_pgen_tmp_280));
  memset(&_pgen_tmp_281, 0, sizeof(_pgen_tmp_281));
  memset(&_pgen_tmp_282, 0, sizeof(_pgen_tmp_282));
  memset(&_pgen_tmp_283, 0, sizeof(_pgen_tmp_283));
  memset(&_pgen_tmp_284, 0, sizeof(_pgen_tmp_284));
  memset(&_pgen_tmp_285, 0, sizeof(_pgen_tmp_285));
  memset(&_pgen_tmp_286, 0, sizeof(_pgen_tmp_286));
  memset(&_pgen_tmp_287, 0, sizeof(_pgen_tmp_287));
  memset(&_pgen_tmp_288, 0, sizeof(_pgen_tmp_288));
  memset(&_pgen_tmp_289, 0, sizeof(_pgen_tmp_289));
  memset(&_pgen_tmp_290, 0, sizeof(_pgen_tmp_290));
  memset(&_pgen_tmp_291, 0, sizeof(_pgen_tmp_291));
  memset(&_pgen_tmp_292, 0, sizeof(_pgen_tmp_292));
  memset(&_pgen_tmp_293, 0, sizeof(_pgen_tmp_293));
  memset(&_pgen_tmp_294, 0, sizeof(_pgen_tmp_294));
  memset(&_pgen_tmp_295, 0, sizeof(_pgen_tmp_295));
  memset(&_pgen_tmp_296, 0, sizeof(_pgen_tmp_296));
  memset(&_pgen_tmp_297, 0, sizeof(_pgen_tmp_297));
  memset(&_pgen_tmp_298, 0, sizeof(_pgen_tmp_298));
  memset(&_pgen_tmp_299, 0, sizeof(_pgen_tmp_299));
  memset(&_pgen_tmp_300, 0, sizeof(_pgen_tmp_300));
  memset(&_pgen_tmp_301, 0, sizeof(_pgen_tmp_301));
  memset(&_pgen_tmp_302, 0, sizeof(_pgen_tmp_302));
  memset(&_pgen_tmp_303, 0, sizeof(_pgen_tmp_303));
  memset(&_pgen_tmp_304, 0, sizeof(_pgen_tmp_304));
  memset(&_pgen_tmp_305, 0, sizeof(_pgen_tmp_305));
  memset(&_pgen_tmp_306, 0, sizeof(_pgen_tmp_306));
  memset(&_pgen_tmp_307, 0, sizeof(_pgen_tmp_307));
  memset(&_pgen_tmp_308, 0, sizeof(_pgen_tmp_308));
  memset(&_pgen_tmp_309, 0, sizeof(_pgen_tmp_309));
  memset(&_pgen_tmp_310, 0, sizeof(_pgen_tmp_310));
  memset(&_pgen_tmp_311, 0, sizeof(_pgen_tmp_311));
  memset(&_pgen_tmp_312, 0, sizeof(_pgen_tmp_312));
  memset(&_pgen_tmp_313, 0, sizeof(_pgen_tmp_313));
  memset(&_pgen_tmp_314, 0, sizeof(_pgen_tmp_314));
  memset(&_pgen_tmp_315, 0, sizeof(_pgen_tmp_315));
  memset(&_pgen_tmp_316, 0, sizeof(_pgen_tmp_316));
  memset(&_pgen_tmp_317, 0, sizeof(_pgen_tmp_317));
  memset(&_pgen_tmp_318, 0, sizeof(_pgen_tmp_318));
  memset(&_pgen_tmp_319, 0, sizeof(_pgen_tmp_319));
  memset(&_pgen_tmp_320, 0, sizeof(_pgen_tmp_320));
  memset(&_pgen_tmp_321, 0, sizeof(_pgen_tmp_321));
  memset(&_pgen_tmp_322, 0, sizeof(_pgen_tmp_322));
  memset(&_pgen_tmp_323, 0, sizeof(_pgen_tmp_323));
  memset(&_pgen_tmp_324, 0, sizeof(_pgen_tmp_324));
  memset(&_pgen_tmp_325, 0, sizeof(_pgen_tmp_325));
  memset(&_pgen_tmp_326, 0, sizeof(_pgen_tmp_326));
  memset(&_pgen_tmp_327, 0, sizeof(_pgen_tmp_327));
  memset(&_pgen_tmp_328, 0, sizeof(_pgen_tmp_328));
  memset(&_pgen_tmp_329, 0, sizeof(_pgen_tmp_329));
  memset(&_pgen_tmp_330, 0, sizeof(_pgen_tmp_330));
  memset(&_pgen_tmp_331, 0, sizeof(_pgen_tmp_331));
  memset(&_pgen_tmp_332, 0, sizeof(_pgen_tmp_332));
  memset(&_pgen_tmp_333, 0, sizeof(_pgen_tmp_333));
  memset(&_pgen_tmp_334, 0, sizeof(_pgen_tmp_334));
  memset(&_pgen_tmp_335, 0, sizeof(_pgen_tmp_335));
  memset(&_pgen_tmp_336, 0, sizeof(_pgen_tmp_336));
  memset(&_pgen_tmp_337, 0, sizeof(_pgen_tmp_337));
  memset(&_pgen_tmp_338, 0, sizeof(_pgen_tmp_338));
  memset(&_pgen_tmp_339, 0, sizeof(_pgen_tmp_339));
  memset(&_pgen_tmp_340, 0, sizeof(_pgen_tmp_340));
  memset(&_pgen_tmp_341, 0, sizeof(_pgen_tmp_341));
  memset(&_pgen_tmp_342, 0, sizeof(_pgen_tmp_342));
  memset(&_pgen_tmp_343, 0, sizeof(_pgen_tmp_343));
  memset(&_pgen_tmp_344, 0, sizeof(_pgen_tmp_344));
  memset(&_pgen_tmp_345, 0, sizeof(_pgen_tmp_345));
  memset(&_pgen_tmp_346, 0, sizeof(_pgen_tmp_346));
  memset(&_pgen_tmp_347, 0, sizeof(_pgen_tmp_347));
  memset(&_pgen_tmp_348, 0, sizeof(_pgen_tmp_348));
  memset(&_pgen_tmp_349, 0, sizeof(_pgen_tmp_349));
  memset(&_pgen_tmp_350, 0, sizeof(_pgen_tmp_350));
  memset(&_pgen_tmp_351, 0, sizeof(_pgen_tmp_351));
  memset(&_pgen_tmp_352, 0, sizeof(_pgen_tmp_352));
  memset(&_pgen_tmp_353, 0, sizeof(_pgen_tmp_353));
  memset(&_pgen_tmp_354, 0, sizeof(_pgen_tmp_354));
  memset(&_pgen_tmp_355, 0, sizeof(_pgen_tmp_355));
  memset(&_pgen_tmp_356, 0, sizeof(_pgen_tmp_356));
  memset(&_pgen_tmp_357, 0, sizeof(_pgen_tmp_357));
  memset(&_pgen_tmp_358, 0, sizeof(_pgen_tmp_358));
  memset(&_pgen_tmp_359, 0, sizeof(_pgen_tmp_359));
  memset(&_pgen_tmp_360, 0, sizeof(_pgen_tmp_360));
  memset(&_pgen_tmp_361, 0, sizeof(_pgen_tmp_361));
  memset(&_pgen_tmp_362, 0, sizeof(_pgen_tmp_362));
  memset(&_pgen_tmp_363, 0, sizeof(_pgen_tmp_363));
  memset(&_pgen_tmp_364, 0, sizeof(_pgen_tmp_364));
  memset(&_pgen_tmp_365, 0, sizeof(_pgen_tmp_365));
  memset(&_pgen_tmp_366, 0, sizeof(_pgen_tmp_366));
  memset(&_pgen_tmp_367, 0, sizeof(_pgen_tmp_367));
  memset(&_pgen_tmp_368, 0, sizeof(_pgen_tmp_368));
  memset(&_pgen_tmp_369, 0, sizeof(_pgen_tmp_369));
  memset(&_pgen_tmp_370, 0, sizeof(_pgen_tmp_370));
  memset(&_pgen_tmp_371, 0, sizeof(_pgen_tmp_371));
  memset(&_pgen_tmp_372, 0, sizeof(_pgen_tmp_372));
  memset(&_pgen_tmp_373, 0, sizeof(_pgen_tmp_373));
  memset(&_pgen_tmp_374, 0, sizeof(_pgen_tmp_374));
  memset(&_pgen_tmp_375, 0, sizeof(_pgen_tmp_375));
  memset(&_pgen_tmp_376, 0, sizeof(_pgen_tmp_376));
  memset(&_pgen_tmp_377, 0, sizeof(_pgen_tmp_377));
  memset(&_pgen_tmp_378, 0, sizeof(_pgen_tmp_378));
  memset(&_pgen_tmp_379, 0, sizeof(_pgen_tmp_379));
  memset(&_pgen_tmp_380, 0, sizeof(_pgen_tmp_380));
  memset(&_pgen_tmp_381, 0, sizeof(_pgen_tmp_381));
  memset(&_pgen_tmp_382, 0, sizeof(_pgen_tmp_382));
  memset(&_pgen_tmp_383, 0, sizeof(_pgen_tmp_383));
  memset(&_pgen_tmp_384, 0, sizeof(_pgen_tmp_384));
  memset(&_pgen_tmp_385, 0, sizeof(_pgen_tmp_385));
  memset(&_pgen_tmp_386, 0, sizeof(_pgen_tmp_386));
  memset(&_pgen_tmp_387, 0, sizeof(_pgen_tmp_387));
  memset(&_pgen_tmp_388, 0, sizeof(_pgen_tmp_388));
  memset(&_pgen_tmp_389, 0, sizeof(_pgen_tmp_389));
  memset(&_pgen_tmp_390, 0, sizeof(_pgen_tmp_390));
  memset(&_pgen_tmp_391, 0, sizeof(_pgen_tmp_391));
  memset(&_pgen_tmp_392, 0, sizeof(_pgen_tmp_392));
  memset(&_pgen_tmp_393, 0, sizeof(_pgen_tmp_393));
  memset(&_pgen_tmp_394, 0, sizeof(_pgen_tmp_394));
  memset(&_pgen_tmp_395, 0, sizeof(_pgen_tmp_395));
  memset(&_pgen_tmp_396, 0, sizeof(_pgen_tmp_396));
  memset(&_pgen_tmp_397, 0, sizeof(_pgen_tmp_397));
  memset(&_pgen_tmp_398, 0, sizeof(_pgen_tmp_398));
  memset(&_pgen_tmp_399, 0, sizeof(_pgen_tmp_399));
  memset(&_pgen_tmp_400, 0, sizeof(_pgen_tmp_400));
  memset(&_pgen_tmp_401, 0, sizeof(_pgen_tmp_401));
  memset(&_pgen_tmp_402, 0, sizeof(_pgen_tmp_402));
  memset(&_pgen_tmp_403, 0, sizeof(_pgen_tmp_403));
  memset(&_pgen_tmp_404, 0, sizeof(_pgen_tmp_404));
  memset(&_pgen_tmp_405, 0, sizeof(_pgen_tmp_405));
  memset(&_pgen_tmp_406, 0, sizeof(_pgen_tmp_406));
  memset(&_pgen_tmp_407, 0, sizeof(_pgen_tmp_407));
  memset(&_pgen_tmp_408, 0, sizeof(_pgen_tmp_408));
  memset(&_pgen_tmp_409, 0, sizeof(_pgen_tmp_409));
  memset(&_pgen_tmp_410, 0, sizeof(_pgen_tmp_410));
  memset(&_pgen_tmp_411, 0, sizeof(_pgen_tmp_411));
  memset(&_pgen_tmp_412, 0, sizeof(_pgen_tmp_412));
  memset(&_pgen_tmp_413, 0, sizeof(_pgen_tmp_413));
  memset(&_pgen_tmp_414, 0, sizeof(_pgen_tmp_414));
  memset(&_pgen_tmp_415, 0, sizeof(_pgen_tmp_415));
  memset(&_pgen_tmp_416, 0, sizeof(_pgen_tmp_416));
  memset(&_pgen_tmp_417, 0, sizeof(_pgen_tmp_417));
  memset(&_pgen_tmp_418, 0, sizeof(_pgen_tmp_418));
  memset(&_pgen_tmp_419, 0, sizeof(_pgen_tmp_419));
  memset(&_pgen_tmp_420, 0, sizeof(_pgen_tmp_420));
  memset(&_pgen_tmp_421, 0, sizeof(_pgen_tmp_421));
  memset(&_pgen_tmp_422, 0, sizeof(_pgen_tmp_422));
  memset(&_pgen_tmp_423, 0, sizeof(_pgen_tmp_423));
  memset(&_pgen_tmp_424, 0, sizeof(_pgen_tmp_424));
  memset(&_pgen_tmp_425, 0, sizeof(_pgen_tmp_425));
  memset(&_pgen_tmp_426, 0, sizeof(_pgen_tmp_426));
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
  memset(&_preg_268435798, 0, sizeof(_preg_268435798));
  memset(&_preg_268435799, 0, sizeof(_preg_268435799));
  memset(&_preg_268435800, 0, sizeof(_preg_268435800));
  memset(&_preg_268435801, 0, sizeof(_preg_268435801));
  memset(&_preg_268435802, 0, sizeof(_preg_268435802));
  memset(&_preg_268435803, 0, sizeof(_preg_268435803));
  memset(&_preg_268435804, 0, sizeof(_preg_268435804));
  memset(&_preg_268435805, 0, sizeof(_preg_268435805));
  memset(&_preg_268435806, 0, sizeof(_preg_268435806));
  memset(&_preg_268435807, 0, sizeof(_preg_268435807));
  memset(&_preg_268435808, 0, sizeof(_preg_268435808));
  memset(&_preg_268435809, 0, sizeof(_preg_268435809));
  Tm_start("Tensor::conv");
  memset(&result_n1, 0, sizeof(result_n1));
  _preg_268435653 = Rotate(input, -4096);
  Init_ciph_same_scale(&_pgen_tmp_0, &input, &_preg_268435653);
  _preg_268435654 = Rotate(input, -8192);
  Init_ciph_same_scale(&_pgen_tmp_1, &_pgen_tmp_0, &_preg_268435654);
  _preg_268435655 = Rotate(input, -12288);
  Init_ciph_same_scale(&input_dup_n1, &_pgen_tmp_1, &_preg_268435655);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&input_dup_n1._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_pgen_tmp_0._c0_poly, _pgen_rns_idx, degree), Coeffs(&input._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435653._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_0._c1_poly, _pgen_rns_idx, degree), Coeffs(&input._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435653._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_1._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_0._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435654._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_1._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_0._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435654._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&input_dup_n1._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_1._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435655._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&input_dup_n1._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_1._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435655._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435653._c1_poly);
    Free_poly_data(&_preg_268435653._c0_poly);
    Free_poly_data(&input._c1_poly);
    Free_poly_data(&input._c0_poly);
    Free_poly_data(&_preg_268435654._c1_poly);
    Free_poly_data(&_preg_268435654._c0_poly);
    Free_poly_data(&_pgen_tmp_0._c1_poly);
    Free_poly_data(&_pgen_tmp_0._c0_poly);
    Free_poly_data(&_preg_268435655._c1_poly);
    Free_poly_data(&_preg_268435655._c0_poly);
    Free_poly_data(&_pgen_tmp_1._c1_poly);
    Free_poly_data(&_pgen_tmp_1._c0_poly);
  }
  for (index_khw1_n1 = 0; index_khw1_n1 < 9; index_khw1_n1 = index_khw1_n1 + 1) {
    _preg_268435656 = Rotate(input_dup_n1, _cst_109[index_khw1_n1]);
    Copy_ciph(&input_roll_n1[index_khw1_n1], &_preg_268435656);
    Free_poly_data(&_preg_268435656._c1_poly);
    Free_poly_data(&_preg_268435656._c0_poly);
  }
  Free_poly_data(&input_dup_n1._c1_poly);
  Free_poly_data(&input_dup_n1._c0_poly);
  for (index_cin_n1 = 0; index_cin_n1 < 4; index_cin_n1 = index_cin_n1 + 1) {
    memset(&result_cin_n1, 0, sizeof(result_cin_n1));
    for (index_khw2_n1 = 0; index_khw2_n1 < 9; index_khw2_n1 = index_khw2_n1 + 1) {
      _rescale_tmp_65 = input_roll_n1[index_khw2_n1];
      Pt_from_msg(&_pgen_tmp_2, index_khw2_n1 + (index_cin_n1 * 9) + 0 /* cst_107_0 */, 16384, Sc_degree(&_rescale_tmp_65), Level(&_rescale_tmp_65));
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
    _preg_268435657 = Rotate(result_cin_n1, -16384);
    Init_ciph_same_scale(&_pgen_tmp_4, &result_cin_n1, &_preg_268435657);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_4._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_4._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n1._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435657._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_4._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n1._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435657._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435658 = Rotate(_pgen_tmp_4, index_cin_n1 * 1024);
    Init_ciph_same_scale(&result_n1, &result_n1, &_preg_268435658);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n1._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n1._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n1._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435658._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n1._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n1._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435658._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435657._c1_poly);
    Free_poly_data(&_preg_268435657._c0_poly);
    Free_poly_data(&_preg_268435658._c1_poly);
    Free_poly_data(&_preg_268435658._c0_poly);
  }
  Free_ciph_poly(input_roll_n1, 9);
  Free_poly_data(&_pgen_tmp_2._poly);
  Free_poly_data(&_pgen_tmp_3._c1_poly);
  Free_poly_data(&_pgen_tmp_3._c0_poly);
  Free_poly_data(&result_cin_n1._c1_poly);
  Free_poly_data(&result_cin_n1._c0_poly);
  Free_poly_data(&_pgen_tmp_4._c1_poly);
  Free_poly_data(&_pgen_tmp_4._c0_poly);
  Pt_from_msg(&_pgen_tmp_5, 36 /* cst_108 */, 16384, Sc_degree(&result_n1), Level(&result_n1));
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
  Init_ciph_down_scale(&_rescale_tmp_5514, &result_n1);
  Rescale(&_rescale_tmp_5514._c0_poly, &result_n1._c0_poly);
  Rescale(&_rescale_tmp_5514._c1_poly, &result_n1._c1_poly);
  Pt_from_msg(&_pgen_tmp_6, 37 /* cst_110 */, 16384, Sc_degree(&_rescale_tmp_5514), Level(&_rescale_tmp_5514));
  Init_ciph_up_scale_plain(&result_n1, &_rescale_tmp_5514, &_pgen_tmp_6);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n1._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n1._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5514._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_6._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n1._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5514._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_6._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_6._poly);
    Free_poly_data(&_rescale_tmp_5514._c1_poly);
    Free_poly_data(&_rescale_tmp_5514._c0_poly);
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
  Encode_float(&_pgen_tmp_9, &_cst_445, 1, 1, Level(&_preg_268435457));
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
  memset(&result_n2, 0, sizeof(result_n2));
  Copy_ciph(&input_dup_n2, &_preg_268435459);
  for (index_khw1_n2 = 0; index_khw1_n2 < 9; index_khw1_n2 = index_khw1_n2 + 1) {
    _preg_268435659 = Rotate(input_dup_n2, _cst_115[index_khw1_n2]);
    Copy_ciph(&input_roll_n2[index_khw1_n2], &_preg_268435659);
    Free_poly_data(&_preg_268435659._c1_poly);
    Free_poly_data(&_preg_268435659._c0_poly);
  }
  Free_poly_data(&input_dup_n2._c1_poly);
  Free_poly_data(&input_dup_n2._c0_poly);
  for (index_cin_n2 = 0; index_cin_n2 < 16; index_cin_n2 = index_cin_n2 + 1) {
    memset(&result_cin_n2, 0, sizeof(result_cin_n2));
    for (index_khw2_n2 = 0; index_khw2_n2 < 9; index_khw2_n2 = index_khw2_n2 + 1) {
      _rescale_tmp_173 = input_roll_n2[index_khw2_n2];
      Pt_from_msg(&_pgen_tmp_11, index_khw2_n2 + (index_cin_n2 * 9) + 38 /* cst_113_0 */, 16384, Sc_degree(&_rescale_tmp_173), Level(&_rescale_tmp_173));
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
    _preg_268435660 = Rotate(result_cin_n2, -16384);
    Init_ciph_same_scale(&_pgen_tmp_13, &result_cin_n2, &_preg_268435660);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_13._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_13._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n2._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435660._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_13._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n2._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435660._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435661 = Rotate(_pgen_tmp_13, index_cin_n2 * 1024);
    Init_ciph_same_scale(&result_n2, &result_n2, &_preg_268435661);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n2._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n2._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n2._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435661._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n2._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n2._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435661._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435660._c1_poly);
    Free_poly_data(&_preg_268435660._c0_poly);
    Free_poly_data(&_preg_268435661._c1_poly);
    Free_poly_data(&_preg_268435661._c0_poly);
  }
  Free_ciph_poly(input_roll_n2, 9);
  Free_poly_data(&_pgen_tmp_11._poly);
  Free_poly_data(&_pgen_tmp_12._c1_poly);
  Free_poly_data(&_pgen_tmp_12._c0_poly);
  Free_poly_data(&result_cin_n2._c1_poly);
  Free_poly_data(&result_cin_n2._c0_poly);
  Free_poly_data(&_pgen_tmp_13._c1_poly);
  Free_poly_data(&_pgen_tmp_13._c0_poly);
  Pt_from_msg(&_pgen_tmp_14, 182 /* cst_114 */, 16384, Sc_degree(&result_n2), Level(&result_n2));
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
  Init_ciph_down_scale(&_rescale_tmp_5527, &result_n2);
  Rescale(&_rescale_tmp_5527._c0_poly, &result_n2._c0_poly);
  Rescale(&_rescale_tmp_5527._c1_poly, &result_n2._c1_poly);
  Pt_from_msg(&_pgen_tmp_15, 183 /* cst_116 */, 16384, Sc_degree(&_rescale_tmp_5527), Level(&_rescale_tmp_5527));
  Init_ciph_up_scale_plain(&result_n2, &_rescale_tmp_5527, &_pgen_tmp_15);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n2._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n2._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5527._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_15._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n2._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5527._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_15._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_15._poly);
    Free_poly_data(&_rescale_tmp_5527._c1_poly);
    Free_poly_data(&_rescale_tmp_5527._c0_poly);
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
  Encode_float(&_pgen_tmp_18, &_cst_446, 1, 1, Level(&_preg_268435461));
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
  memset(&result_n3, 0, sizeof(result_n3));
  Copy_ciph(&input_dup_n3, &_preg_268435463);
  Free_poly_data(&_preg_268435463._c1_poly);
  Free_poly_data(&_preg_268435463._c0_poly);
  for (index_khw1_n3 = 0; index_khw1_n3 < 9; index_khw1_n3 = index_khw1_n3 + 1) {
    _preg_268435662 = Rotate(input_dup_n3, _cst_121[index_khw1_n3]);
    Copy_ciph(&input_roll_n3[index_khw1_n3], &_preg_268435662);
    Free_poly_data(&_preg_268435662._c1_poly);
    Free_poly_data(&_preg_268435662._c0_poly);
  }
  Free_poly_data(&input_dup_n3._c1_poly);
  Free_poly_data(&input_dup_n3._c0_poly);
  for (index_cin_n3 = 0; index_cin_n3 < 16; index_cin_n3 = index_cin_n3 + 1) {
    memset(&result_cin_n3, 0, sizeof(result_cin_n3));
    for (index_khw2_n3 = 0; index_khw2_n3 < 9; index_khw2_n3 = index_khw2_n3 + 1) {
      _rescale_tmp_281 = input_roll_n3[index_khw2_n3];
      Pt_from_msg(&_pgen_tmp_20, index_khw2_n3 + (index_cin_n3 * 9) + 184 /* cst_119_0 */, 16384, Sc_degree(&_rescale_tmp_281), Level(&_rescale_tmp_281));
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
    _preg_268435663 = Rotate(result_cin_n3, -16384);
    Init_ciph_same_scale(&_pgen_tmp_22, &result_cin_n3, &_preg_268435663);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_22._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_22._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n3._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435663._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_22._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n3._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435663._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435664 = Rotate(_pgen_tmp_22, index_cin_n3 * 1024);
    Init_ciph_same_scale(&result_n3, &result_n3, &_preg_268435664);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n3._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n3._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n3._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435664._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n3._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n3._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435664._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435663._c1_poly);
    Free_poly_data(&_preg_268435663._c0_poly);
    Free_poly_data(&_preg_268435664._c1_poly);
    Free_poly_data(&_preg_268435664._c0_poly);
  }
  Free_ciph_poly(input_roll_n3, 9);
  Free_poly_data(&_pgen_tmp_20._poly);
  Free_poly_data(&_pgen_tmp_21._c1_poly);
  Free_poly_data(&_pgen_tmp_21._c0_poly);
  Free_poly_data(&result_cin_n3._c1_poly);
  Free_poly_data(&result_cin_n3._c0_poly);
  Free_poly_data(&_pgen_tmp_22._c1_poly);
  Free_poly_data(&_pgen_tmp_22._c0_poly);
  Pt_from_msg(&_pgen_tmp_23, 328 /* cst_120 */, 16384, Sc_degree(&result_n3), Level(&result_n3));
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
  Init_ciph_down_scale(&_rescale_tmp_5540, &result_n3);
  Rescale(&_rescale_tmp_5540._c0_poly, &result_n3._c0_poly);
  Rescale(&_rescale_tmp_5540._c1_poly, &result_n3._c1_poly);
  Pt_from_msg(&_pgen_tmp_24, 329 /* cst_122 */, 16384, Sc_degree(&_rescale_tmp_5540), Level(&_rescale_tmp_5540));
  Init_ciph_up_scale_plain(&result_n3, &_rescale_tmp_5540, &_pgen_tmp_24);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n3._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n3._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5540._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_24._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n3._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5540._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_24._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_24._poly);
    Free_poly_data(&_rescale_tmp_5540._c1_poly);
    Free_poly_data(&_rescale_tmp_5540._c0_poly);
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
  Encode_float(&_pgen_tmp_27, &_cst_447, 1, 1, Level(&_preg_268435466));
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
  memset(&result_n4, 0, sizeof(result_n4));
  Copy_ciph(&input_dup_n4, &_preg_268435468);
  for (index_khw1_n4 = 0; index_khw1_n4 < 9; index_khw1_n4 = index_khw1_n4 + 1) {
    _preg_268435665 = Rotate(input_dup_n4, _cst_128[index_khw1_n4]);
    Copy_ciph(&input_roll_n4[index_khw1_n4], &_preg_268435665);
    Free_poly_data(&_preg_268435665._c1_poly);
    Free_poly_data(&_preg_268435665._c0_poly);
  }
  Free_poly_data(&input_dup_n4._c1_poly);
  Free_poly_data(&input_dup_n4._c0_poly);
  for (index_cin_n4 = 0; index_cin_n4 < 16; index_cin_n4 = index_cin_n4 + 1) {
    memset(&result_cin_n4, 0, sizeof(result_cin_n4));
    for (index_khw2_n4 = 0; index_khw2_n4 < 9; index_khw2_n4 = index_khw2_n4 + 1) {
      _rescale_tmp_395 = input_roll_n4[index_khw2_n4];
      Pt_from_msg(&_pgen_tmp_29, index_khw2_n4 + (index_cin_n4 * 9) + 330 /* cst_126_0 */, 16384, Sc_degree(&_rescale_tmp_395), Level(&_rescale_tmp_395));
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
    _preg_268435666 = Rotate(result_cin_n4, -16384);
    Init_ciph_same_scale(&_pgen_tmp_31, &result_cin_n4, &_preg_268435666);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_31._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_31._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n4._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435666._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_31._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n4._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435666._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435667 = Rotate(_pgen_tmp_31, index_cin_n4 * 1024);
    Init_ciph_same_scale(&result_n4, &result_n4, &_preg_268435667);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n4._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n4._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n4._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435667._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n4._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n4._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435667._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435666._c1_poly);
    Free_poly_data(&_preg_268435666._c0_poly);
    Free_poly_data(&_preg_268435667._c1_poly);
    Free_poly_data(&_preg_268435667._c0_poly);
  }
  Free_ciph_poly(input_roll_n4, 9);
  Free_poly_data(&_pgen_tmp_29._poly);
  Free_poly_data(&_pgen_tmp_30._c1_poly);
  Free_poly_data(&_pgen_tmp_30._c0_poly);
  Free_poly_data(&result_cin_n4._c1_poly);
  Free_poly_data(&result_cin_n4._c0_poly);
  Free_poly_data(&_pgen_tmp_31._c1_poly);
  Free_poly_data(&_pgen_tmp_31._c0_poly);
  Pt_from_msg(&_pgen_tmp_32, 474 /* cst_127 */, 16384, Sc_degree(&result_n4), Level(&result_n4));
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
  Init_ciph_down_scale(&_rescale_tmp_5553, &result_n4);
  Rescale(&_rescale_tmp_5553._c0_poly, &result_n4._c0_poly);
  Rescale(&_rescale_tmp_5553._c1_poly, &result_n4._c1_poly);
  Pt_from_msg(&_pgen_tmp_33, 475 /* cst_129 */, 16384, Sc_degree(&_rescale_tmp_5553), Level(&_rescale_tmp_5553));
  Init_ciph_up_scale_plain(&result_n4, &_rescale_tmp_5553, &_pgen_tmp_33);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n4._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n4._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5553._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_33._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n4._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5553._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_33._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_33._poly);
    Free_poly_data(&_rescale_tmp_5553._c1_poly);
    Free_poly_data(&_rescale_tmp_5553._c0_poly);
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
  Encode_float(&_pgen_tmp_36, &_cst_448, 1, 1, Level(&_preg_268435470));
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
  memset(&result_n5, 0, sizeof(result_n5));
  Copy_ciph(&input_dup_n5, &_preg_268435472);
  Free_poly_data(&_preg_268435472._c1_poly);
  Free_poly_data(&_preg_268435472._c0_poly);
  for (index_khw1_n5 = 0; index_khw1_n5 < 9; index_khw1_n5 = index_khw1_n5 + 1) {
    _preg_268435668 = Rotate(input_dup_n5, _cst_134[index_khw1_n5]);
    Copy_ciph(&input_roll_n5[index_khw1_n5], &_preg_268435668);
    Free_poly_data(&_preg_268435668._c1_poly);
    Free_poly_data(&_preg_268435668._c0_poly);
  }
  Free_poly_data(&input_dup_n5._c1_poly);
  Free_poly_data(&input_dup_n5._c0_poly);
  for (index_cin_n5 = 0; index_cin_n5 < 16; index_cin_n5 = index_cin_n5 + 1) {
    memset(&result_cin_n5, 0, sizeof(result_cin_n5));
    for (index_khw2_n5 = 0; index_khw2_n5 < 9; index_khw2_n5 = index_khw2_n5 + 1) {
      _rescale_tmp_503 = input_roll_n5[index_khw2_n5];
      Pt_from_msg(&_pgen_tmp_38, index_khw2_n5 + (index_cin_n5 * 9) + 476 /* cst_132_0 */, 16384, Sc_degree(&_rescale_tmp_503), Level(&_rescale_tmp_503));
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
    _preg_268435669 = Rotate(result_cin_n5, -16384);
    Init_ciph_same_scale(&_pgen_tmp_40, &result_cin_n5, &_preg_268435669);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_40._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_40._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n5._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435669._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_40._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n5._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435669._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435670 = Rotate(_pgen_tmp_40, index_cin_n5 * 1024);
    Init_ciph_same_scale(&result_n5, &result_n5, &_preg_268435670);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n5._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n5._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n5._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435670._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n5._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n5._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435670._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435669._c1_poly);
    Free_poly_data(&_preg_268435669._c0_poly);
    Free_poly_data(&_preg_268435670._c1_poly);
    Free_poly_data(&_preg_268435670._c0_poly);
  }
  Free_ciph_poly(input_roll_n5, 9);
  Free_poly_data(&_pgen_tmp_38._poly);
  Free_poly_data(&_pgen_tmp_39._c1_poly);
  Free_poly_data(&_pgen_tmp_39._c0_poly);
  Free_poly_data(&result_cin_n5._c1_poly);
  Free_poly_data(&result_cin_n5._c0_poly);
  Free_poly_data(&_pgen_tmp_40._c1_poly);
  Free_poly_data(&_pgen_tmp_40._c0_poly);
  Pt_from_msg(&_pgen_tmp_41, 620 /* cst_133 */, 16384, Sc_degree(&result_n5), Level(&result_n5));
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
  Init_ciph_down_scale(&_rescale_tmp_5566, &result_n5);
  Rescale(&_rescale_tmp_5566._c0_poly, &result_n5._c0_poly);
  Rescale(&_rescale_tmp_5566._c1_poly, &result_n5._c1_poly);
  Pt_from_msg(&_pgen_tmp_42, 621 /* cst_135 */, 16384, Sc_degree(&_rescale_tmp_5566), Level(&_rescale_tmp_5566));
  Init_ciph_up_scale_plain(&result_n5, &_rescale_tmp_5566, &_pgen_tmp_42);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n5._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n5._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5566._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_42._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n5._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5566._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_42._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_42._poly);
    Free_poly_data(&_rescale_tmp_5566._c1_poly);
    Free_poly_data(&_rescale_tmp_5566._c0_poly);
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
  Encode_float(&_pgen_tmp_45, &_cst_449, 1, 1, Level(&_preg_268435475));
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
  memset(&result_n6, 0, sizeof(result_n6));
  Copy_ciph(&input_dup_n6, &_preg_268435477);
  for (index_khw1_n6 = 0; index_khw1_n6 < 9; index_khw1_n6 = index_khw1_n6 + 1) {
    _preg_268435671 = Rotate(input_dup_n6, _cst_141[index_khw1_n6]);
    Copy_ciph(&input_roll_n6[index_khw1_n6], &_preg_268435671);
    Free_poly_data(&_preg_268435671._c1_poly);
    Free_poly_data(&_preg_268435671._c0_poly);
  }
  Free_poly_data(&input_dup_n6._c1_poly);
  Free_poly_data(&input_dup_n6._c0_poly);
  for (index_cin_n6 = 0; index_cin_n6 < 16; index_cin_n6 = index_cin_n6 + 1) {
    memset(&result_cin_n6, 0, sizeof(result_cin_n6));
    for (index_khw2_n6 = 0; index_khw2_n6 < 9; index_khw2_n6 = index_khw2_n6 + 1) {
      _rescale_tmp_617 = input_roll_n6[index_khw2_n6];
      Pt_from_msg(&_pgen_tmp_47, index_khw2_n6 + (index_cin_n6 * 9) + 622 /* cst_139_0 */, 16384, Sc_degree(&_rescale_tmp_617), Level(&_rescale_tmp_617));
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
    _preg_268435672 = Rotate(result_cin_n6, -16384);
    Init_ciph_same_scale(&_pgen_tmp_49, &result_cin_n6, &_preg_268435672);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_49._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_49._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n6._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435672._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_49._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n6._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435672._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435673 = Rotate(_pgen_tmp_49, index_cin_n6 * 1024);
    Init_ciph_same_scale(&result_n6, &result_n6, &_preg_268435673);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n6._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n6._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n6._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435673._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n6._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n6._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435673._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435672._c1_poly);
    Free_poly_data(&_preg_268435672._c0_poly);
    Free_poly_data(&_preg_268435673._c1_poly);
    Free_poly_data(&_preg_268435673._c0_poly);
  }
  Free_ciph_poly(input_roll_n6, 9);
  Free_poly_data(&_pgen_tmp_47._poly);
  Free_poly_data(&_pgen_tmp_48._c1_poly);
  Free_poly_data(&_pgen_tmp_48._c0_poly);
  Free_poly_data(&result_cin_n6._c1_poly);
  Free_poly_data(&result_cin_n6._c0_poly);
  Free_poly_data(&_pgen_tmp_49._c1_poly);
  Free_poly_data(&_pgen_tmp_49._c0_poly);
  Pt_from_msg(&_pgen_tmp_50, 766 /* cst_140 */, 16384, Sc_degree(&result_n6), Level(&result_n6));
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
  Init_ciph_down_scale(&_rescale_tmp_5579, &result_n6);
  Rescale(&_rescale_tmp_5579._c0_poly, &result_n6._c0_poly);
  Rescale(&_rescale_tmp_5579._c1_poly, &result_n6._c1_poly);
  Pt_from_msg(&_pgen_tmp_51, 767 /* cst_142 */, 16384, Sc_degree(&_rescale_tmp_5579), Level(&_rescale_tmp_5579));
  Init_ciph_up_scale_plain(&result_n6, &_rescale_tmp_5579, &_pgen_tmp_51);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n6._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n6._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5579._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_51._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n6._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5579._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_51._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_51._poly);
    Free_poly_data(&_rescale_tmp_5579._c1_poly);
    Free_poly_data(&_rescale_tmp_5579._c0_poly);
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
  Encode_float(&_pgen_tmp_54, &_cst_450, 1, 1, Level(&_preg_268435479));
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
  memset(&result_n7, 0, sizeof(result_n7));
  Copy_ciph(&input_dup_n7, &_preg_268435481);
  Free_poly_data(&_preg_268435481._c1_poly);
  Free_poly_data(&_preg_268435481._c0_poly);
  for (index_khw1_n7 = 0; index_khw1_n7 < 9; index_khw1_n7 = index_khw1_n7 + 1) {
    _preg_268435674 = Rotate(input_dup_n7, _cst_147[index_khw1_n7]);
    Copy_ciph(&input_roll_n7[index_khw1_n7], &_preg_268435674);
    Free_poly_data(&_preg_268435674._c1_poly);
    Free_poly_data(&_preg_268435674._c0_poly);
  }
  Free_poly_data(&input_dup_n7._c1_poly);
  Free_poly_data(&input_dup_n7._c0_poly);
  for (index_cin_n7 = 0; index_cin_n7 < 16; index_cin_n7 = index_cin_n7 + 1) {
    memset(&result_cin_n7, 0, sizeof(result_cin_n7));
    for (index_khw2_n7 = 0; index_khw2_n7 < 9; index_khw2_n7 = index_khw2_n7 + 1) {
      _rescale_tmp_725 = input_roll_n7[index_khw2_n7];
      Pt_from_msg(&_pgen_tmp_56, index_khw2_n7 + (index_cin_n7 * 9) + 768 /* cst_145_0 */, 16384, Sc_degree(&_rescale_tmp_725), Level(&_rescale_tmp_725));
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
    _preg_268435675 = Rotate(result_cin_n7, -16384);
    Init_ciph_same_scale(&_pgen_tmp_58, &result_cin_n7, &_preg_268435675);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_58._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_58._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n7._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435675._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_58._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n7._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435675._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435676 = Rotate(_pgen_tmp_58, index_cin_n7 * 1024);
    Init_ciph_same_scale(&result_n7, &result_n7, &_preg_268435676);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n7._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n7._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n7._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435676._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n7._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n7._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435676._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435675._c1_poly);
    Free_poly_data(&_preg_268435675._c0_poly);
    Free_poly_data(&_preg_268435676._c1_poly);
    Free_poly_data(&_preg_268435676._c0_poly);
  }
  Free_ciph_poly(input_roll_n7, 9);
  Free_poly_data(&_pgen_tmp_56._poly);
  Free_poly_data(&_pgen_tmp_57._c1_poly);
  Free_poly_data(&_pgen_tmp_57._c0_poly);
  Free_poly_data(&result_cin_n7._c1_poly);
  Free_poly_data(&result_cin_n7._c0_poly);
  Free_poly_data(&_pgen_tmp_58._c1_poly);
  Free_poly_data(&_pgen_tmp_58._c0_poly);
  Pt_from_msg(&_pgen_tmp_59, 912 /* cst_146 */, 16384, Sc_degree(&result_n7), Level(&result_n7));
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
  Init_ciph_down_scale(&_rescale_tmp_5592, &result_n7);
  Rescale(&_rescale_tmp_5592._c0_poly, &result_n7._c0_poly);
  Rescale(&_rescale_tmp_5592._c1_poly, &result_n7._c1_poly);
  Pt_from_msg(&_pgen_tmp_60, 913 /* cst_148 */, 16384, Sc_degree(&_rescale_tmp_5592), Level(&_rescale_tmp_5592));
  Init_ciph_up_scale_plain(&result_n7, &_rescale_tmp_5592, &_pgen_tmp_60);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n7._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n7._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5592._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_60._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n7._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5592._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_60._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_60._poly);
    Free_poly_data(&_rescale_tmp_5592._c1_poly);
    Free_poly_data(&_rescale_tmp_5592._c0_poly);
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
  Bootstrap(&_preg_268435484, &_preg_268435483, 15);
  Free_poly_data(&_preg_268435483._c1_poly);
  Free_poly_data(&_preg_268435483._c0_poly);
  Encode_float(&_pgen_tmp_63, &_cst_451, 1, 1, Level(&_preg_268435484));
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
  memset(&result_n8, 0, sizeof(result_n8));
  Copy_ciph(&input_dup_n8, &_preg_268435486);
  for (index_khw1_n8 = 0; index_khw1_n8 < 9; index_khw1_n8 = index_khw1_n8 + 1) {
    _preg_268435677 = Rotate(input_dup_n8, _cst_154[index_khw1_n8]);
    Copy_ciph(&input_roll_n8[index_khw1_n8], &_preg_268435677);
    Free_poly_data(&_preg_268435677._c1_poly);
    Free_poly_data(&_preg_268435677._c0_poly);
  }
  Free_poly_data(&input_dup_n8._c1_poly);
  Free_poly_data(&input_dup_n8._c0_poly);
  for (index_cin_n8 = 0; index_cin_n8 < 16; index_cin_n8 = index_cin_n8 + 1) {
    memset(&result_cin_n8, 0, sizeof(result_cin_n8));
    for (index_khw2_n8 = 0; index_khw2_n8 < 9; index_khw2_n8 = index_khw2_n8 + 1) {
      _rescale_tmp_839 = input_roll_n8[index_khw2_n8];
      Pt_from_msg(&_pgen_tmp_65, index_khw2_n8 + (index_cin_n8 * 9) + 914 /* cst_152_0 */, 16384, Sc_degree(&_rescale_tmp_839), Level(&_rescale_tmp_839));
      Init_ciph_up_scale_plain(&_pgen_tmp_66, &_rescale_tmp_839, &_pgen_tmp_65);
      Init_ciph_same_scale(&result_cin_n8, &result_cin_n8, &_pgen_tmp_66);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n8._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_66._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_839._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_65._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_66._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_839._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_65._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n8._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n8._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_66._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n8._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n8._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_66._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435678 = Rotate(result_cin_n8, -16384);
    Init_ciph_same_scale(&_pgen_tmp_67, &result_cin_n8, &_preg_268435678);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_67._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_67._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n8._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435678._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_67._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n8._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435678._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435679 = Rotate(_pgen_tmp_67, index_cin_n8 * 1024);
    Init_ciph_same_scale(&result_n8, &result_n8, &_preg_268435679);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n8._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n8._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n8._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435679._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n8._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n8._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435679._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435678._c1_poly);
    Free_poly_data(&_preg_268435678._c0_poly);
    Free_poly_data(&_preg_268435679._c1_poly);
    Free_poly_data(&_preg_268435679._c0_poly);
  }
  Free_ciph_poly(input_roll_n8, 9);
  Free_poly_data(&_pgen_tmp_65._poly);
  Free_poly_data(&_pgen_tmp_66._c1_poly);
  Free_poly_data(&_pgen_tmp_66._c0_poly);
  Free_poly_data(&result_cin_n8._c1_poly);
  Free_poly_data(&result_cin_n8._c0_poly);
  Free_poly_data(&_pgen_tmp_67._c1_poly);
  Free_poly_data(&_pgen_tmp_67._c0_poly);
  Pt_from_msg(&_pgen_tmp_68, 1058 /* cst_153 */, 16384, Sc_degree(&result_n8), Level(&result_n8));
  Init_ciph_same_scale_plain(&result_n8, &result_n8, &_pgen_tmp_68);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n8._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n8._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n8._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_68._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n8._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n8._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_68._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_5605, &result_n8);
  Rescale(&_rescale_tmp_5605._c0_poly, &result_n8._c0_poly);
  Rescale(&_rescale_tmp_5605._c1_poly, &result_n8._c1_poly);
  Pt_from_msg(&_pgen_tmp_69, 1059 /* cst_155 */, 16384, Sc_degree(&_rescale_tmp_5605), Level(&_rescale_tmp_5605));
  Init_ciph_up_scale_plain(&result_n8, &_rescale_tmp_5605, &_pgen_tmp_69);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n8._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n8._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5605._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_69._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n8._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5605._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_69._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_69._poly);
    Free_poly_data(&_rescale_tmp_5605._c1_poly);
    Free_poly_data(&_rescale_tmp_5605._c0_poly);
  }
  Copy_ciph(&_preg_268435487, &result_n8);
  Free_poly_data(&result_n8._c1_poly);
  Free_poly_data(&result_n8._c0_poly);
  Tm_taken("Tensor::conv");
  Init_ciph_down_scale(&_pgen_tmp_70, &_preg_268435487);
  Rescale(&_pgen_tmp_70._c0_poly, &_preg_268435487._c0_poly);
  Rescale(&_pgen_tmp_70._c1_poly, &_preg_268435487._c1_poly);
  Free_poly_data(&_preg_268435487._c1_poly);
  Free_poly_data(&_preg_268435487._c0_poly);
  Bootstrap(&_preg_268435488, &_pgen_tmp_70, 15);
  Free_poly_data(&_pgen_tmp_70._c1_poly);
  Free_poly_data(&_pgen_tmp_70._c0_poly);
  Encode_float(&_pgen_tmp_72, &_cst_452, 1, 1, Level(&_preg_268435488));
  Init_ciph_up_scale_plain(&_pgen_tmp_71, &_preg_268435488, &_pgen_tmp_72);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_71._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_71._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435488._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_72._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_71._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435488._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_72._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_72._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_73, &_pgen_tmp_71);
  Rescale(&_pgen_tmp_73._c0_poly, &_pgen_tmp_71._c0_poly);
  Rescale(&_pgen_tmp_73._c1_poly, &_pgen_tmp_71._c1_poly);
  Free_poly_data(&_pgen_tmp_71._c1_poly);
  Free_poly_data(&_pgen_tmp_71._c0_poly);
  _preg_268435489 = App_relu(_preg_268435488, _pgen_tmp_73);
  Free_poly_data(&_pgen_tmp_73._c1_poly);
  Free_poly_data(&_pgen_tmp_73._c0_poly);
  Free_poly_data(&_preg_268435488._c1_poly);
  Free_poly_data(&_preg_268435488._c0_poly);
  Copy_ciph(&_preg_268435490, &_preg_268435489);
  Free_poly_data(&_preg_268435489._c1_poly);
  Free_poly_data(&_preg_268435489._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n9, 0, sizeof(result_n9));
  Copy_ciph(&input_dup_n9, &_preg_268435490);
  Free_poly_data(&_preg_268435490._c1_poly);
  Free_poly_data(&_preg_268435490._c0_poly);
  for (index_khw1_n9 = 0; index_khw1_n9 < 9; index_khw1_n9 = index_khw1_n9 + 1) {
    _preg_268435680 = Rotate(input_dup_n9, _cst_160[index_khw1_n9]);
    Copy_ciph(&input_roll_n9[index_khw1_n9], &_preg_268435680);
    Free_poly_data(&_preg_268435680._c1_poly);
    Free_poly_data(&_preg_268435680._c0_poly);
  }
  Free_poly_data(&input_dup_n9._c1_poly);
  Free_poly_data(&input_dup_n9._c0_poly);
  for (index_cin_n9 = 0; index_cin_n9 < 16; index_cin_n9 = index_cin_n9 + 1) {
    memset(&result_cin_n9, 0, sizeof(result_cin_n9));
    for (index_khw2_n9 = 0; index_khw2_n9 < 9; index_khw2_n9 = index_khw2_n9 + 1) {
      _rescale_tmp_947 = input_roll_n9[index_khw2_n9];
      Pt_from_msg(&_pgen_tmp_74, index_khw2_n9 + (index_cin_n9 * 9) + 1060 /* cst_158_0 */, 16384, Sc_degree(&_rescale_tmp_947), Level(&_rescale_tmp_947));
      Init_ciph_up_scale_plain(&_pgen_tmp_75, &_rescale_tmp_947, &_pgen_tmp_74);
      Init_ciph_same_scale(&result_cin_n9, &result_cin_n9, &_pgen_tmp_75);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n9._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_75._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_947._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_74._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_75._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_947._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_74._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n9._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n9._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_75._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n9._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n9._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_75._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435681 = Rotate(result_cin_n9, -16384);
    Init_ciph_same_scale(&_pgen_tmp_76, &result_cin_n9, &_preg_268435681);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_76._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_76._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n9._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435681._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_76._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n9._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435681._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435682 = Rotate(_pgen_tmp_76, index_cin_n9 * 1024);
    Init_ciph_same_scale(&result_n9, &result_n9, &_preg_268435682);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n9._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n9._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n9._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435682._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n9._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n9._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435682._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435681._c1_poly);
    Free_poly_data(&_preg_268435681._c0_poly);
    Free_poly_data(&_preg_268435682._c1_poly);
    Free_poly_data(&_preg_268435682._c0_poly);
  }
  Free_ciph_poly(input_roll_n9, 9);
  Free_poly_data(&_pgen_tmp_74._poly);
  Free_poly_data(&_pgen_tmp_75._c1_poly);
  Free_poly_data(&_pgen_tmp_75._c0_poly);
  Free_poly_data(&result_cin_n9._c1_poly);
  Free_poly_data(&result_cin_n9._c0_poly);
  Free_poly_data(&_pgen_tmp_76._c1_poly);
  Free_poly_data(&_pgen_tmp_76._c0_poly);
  Pt_from_msg(&_pgen_tmp_77, 1204 /* cst_159 */, 16384, Sc_degree(&result_n9), Level(&result_n9));
  Init_ciph_same_scale_plain(&result_n9, &result_n9, &_pgen_tmp_77);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n9._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n9._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n9._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_77._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n9._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n9._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_77._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_5618, &result_n9);
  Rescale(&_rescale_tmp_5618._c0_poly, &result_n9._c0_poly);
  Rescale(&_rescale_tmp_5618._c1_poly, &result_n9._c1_poly);
  Pt_from_msg(&_pgen_tmp_78, 1205 /* cst_161 */, 16384, Sc_degree(&_rescale_tmp_5618), Level(&_rescale_tmp_5618));
  Init_ciph_up_scale_plain(&result_n9, &_rescale_tmp_5618, &_pgen_tmp_78);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n9._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n9._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5618._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_78._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n9._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5618._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_78._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_78._poly);
    Free_poly_data(&_rescale_tmp_5618._c1_poly);
    Free_poly_data(&_rescale_tmp_5618._c0_poly);
  }
  Copy_ciph(&_preg_268435491, &result_n9);
  Free_poly_data(&result_n9._c1_poly);
  Free_poly_data(&result_n9._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::add");
  Init_ciph_down_scale(&_pgen_tmp_79, &_preg_268435491);
  Rescale(&_pgen_tmp_79._c0_poly, &_preg_268435491._c0_poly);
  Rescale(&_pgen_tmp_79._c1_poly, &_preg_268435491._c1_poly);
  Free_poly_data(&_preg_268435491._c1_poly);
  Free_poly_data(&_preg_268435491._c0_poly);
  Init_ciph_same_scale(&_preg_268435492, &_pgen_tmp_79, &_preg_268435486);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_preg_268435492._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_preg_268435492._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_79._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435486._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_preg_268435492._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_79._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435486._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435486._c1_poly);
    Free_poly_data(&_preg_268435486._c0_poly);
    Free_poly_data(&_pgen_tmp_79._c1_poly);
    Free_poly_data(&_pgen_tmp_79._c0_poly);
  }
  Tm_taken("Tensor::add");
  Bootstrap(&_preg_268435493, &_preg_268435492, 15);
  Free_poly_data(&_preg_268435492._c1_poly);
  Free_poly_data(&_preg_268435492._c0_poly);
  Encode_float(&_pgen_tmp_81, &_cst_453, 1, 1, Level(&_preg_268435493));
  Init_ciph_up_scale_plain(&_pgen_tmp_80, &_preg_268435493, &_pgen_tmp_81);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_80._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_80._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435493._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_81._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_80._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435493._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_81._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_81._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_82, &_pgen_tmp_80);
  Rescale(&_pgen_tmp_82._c0_poly, &_pgen_tmp_80._c0_poly);
  Rescale(&_pgen_tmp_82._c1_poly, &_pgen_tmp_80._c1_poly);
  Free_poly_data(&_pgen_tmp_80._c1_poly);
  Free_poly_data(&_pgen_tmp_80._c0_poly);
  _preg_268435494 = App_relu(_preg_268435493, _pgen_tmp_82);
  Free_poly_data(&_pgen_tmp_82._c1_poly);
  Free_poly_data(&_pgen_tmp_82._c0_poly);
  Free_poly_data(&_preg_268435493._c1_poly);
  Free_poly_data(&_preg_268435493._c0_poly);
  Copy_ciph(&_preg_268435495, &_preg_268435494);
  Free_poly_data(&_preg_268435494._c1_poly);
  Free_poly_data(&_preg_268435494._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n10, 0, sizeof(result_n10));
  Copy_ciph(&input_dup_n10, &_preg_268435495);
  for (index_khw1_n10 = 0; index_khw1_n10 < 9; index_khw1_n10 = index_khw1_n10 + 1) {
    _preg_268435683 = Rotate(input_dup_n10, _cst_167[index_khw1_n10]);
    Copy_ciph(&input_roll_n10[index_khw1_n10], &_preg_268435683);
    Free_poly_data(&_preg_268435683._c1_poly);
    Free_poly_data(&_preg_268435683._c0_poly);
  }
  Free_poly_data(&input_dup_n10._c1_poly);
  Free_poly_data(&input_dup_n10._c0_poly);
  for (index_cin_n10 = 0; index_cin_n10 < 16; index_cin_n10 = index_cin_n10 + 1) {
    memset(&result_cin_n10, 0, sizeof(result_cin_n10));
    for (index_khw2_n10 = 0; index_khw2_n10 < 9; index_khw2_n10 = index_khw2_n10 + 1) {
      _rescale_tmp_1061 = input_roll_n10[index_khw2_n10];
      Pt_from_msg(&_pgen_tmp_83, index_khw2_n10 + (index_cin_n10 * 9) + 1206 /* cst_165_0 */, 16384, Sc_degree(&_rescale_tmp_1061), Level(&_rescale_tmp_1061));
      Init_ciph_up_scale_plain(&_pgen_tmp_84, &_rescale_tmp_1061, &_pgen_tmp_83);
      Init_ciph_same_scale(&result_cin_n10, &result_cin_n10, &_pgen_tmp_84);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n10._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_84._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_1061._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_83._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_84._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_1061._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_83._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n10._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n10._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_84._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n10._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n10._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_84._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435684 = Rotate(result_cin_n10, -16384);
    Init_ciph_same_scale(&_pgen_tmp_85, &result_cin_n10, &_preg_268435684);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_85._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_85._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n10._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435684._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_85._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n10._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435684._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435685 = Rotate(_pgen_tmp_85, index_cin_n10 * 1024);
    Init_ciph_same_scale(&result_n10, &result_n10, &_preg_268435685);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n10._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n10._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n10._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435685._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n10._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n10._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435685._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435684._c1_poly);
    Free_poly_data(&_preg_268435684._c0_poly);
    Free_poly_data(&_preg_268435685._c1_poly);
    Free_poly_data(&_preg_268435685._c0_poly);
  }
  Free_ciph_poly(input_roll_n10, 9);
  Free_poly_data(&_pgen_tmp_83._poly);
  Free_poly_data(&_pgen_tmp_84._c1_poly);
  Free_poly_data(&_pgen_tmp_84._c0_poly);
  Free_poly_data(&result_cin_n10._c1_poly);
  Free_poly_data(&result_cin_n10._c0_poly);
  Free_poly_data(&_pgen_tmp_85._c1_poly);
  Free_poly_data(&_pgen_tmp_85._c0_poly);
  Pt_from_msg(&_pgen_tmp_86, 1350 /* cst_166 */, 16384, Sc_degree(&result_n10), Level(&result_n10));
  Init_ciph_same_scale_plain(&result_n10, &result_n10, &_pgen_tmp_86);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n10._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n10._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n10._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_86._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n10._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n10._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_86._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_5631, &result_n10);
  Rescale(&_rescale_tmp_5631._c0_poly, &result_n10._c0_poly);
  Rescale(&_rescale_tmp_5631._c1_poly, &result_n10._c1_poly);
  Pt_from_msg(&_pgen_tmp_87, 1351 /* cst_168 */, 16384, Sc_degree(&_rescale_tmp_5631), Level(&_rescale_tmp_5631));
  Init_ciph_up_scale_plain(&result_n10, &_rescale_tmp_5631, &_pgen_tmp_87);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n10._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n10._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5631._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_87._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n10._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5631._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_87._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_87._poly);
    Free_poly_data(&_rescale_tmp_5631._c1_poly);
    Free_poly_data(&_rescale_tmp_5631._c0_poly);
  }
  Copy_ciph(&_preg_268435496, &result_n10);
  Free_poly_data(&result_n10._c1_poly);
  Free_poly_data(&result_n10._c0_poly);
  Tm_taken("Tensor::conv");
  Init_ciph_down_scale(&_pgen_tmp_88, &_preg_268435496);
  Rescale(&_pgen_tmp_88._c0_poly, &_preg_268435496._c0_poly);
  Rescale(&_pgen_tmp_88._c1_poly, &_preg_268435496._c1_poly);
  Free_poly_data(&_preg_268435496._c1_poly);
  Free_poly_data(&_preg_268435496._c0_poly);
  Bootstrap(&_preg_268435497, &_pgen_tmp_88, 15);
  Free_poly_data(&_pgen_tmp_88._c1_poly);
  Free_poly_data(&_pgen_tmp_88._c0_poly);
  Encode_float(&_pgen_tmp_90, &_cst_454, 1, 1, Level(&_preg_268435497));
  Init_ciph_up_scale_plain(&_pgen_tmp_89, &_preg_268435497, &_pgen_tmp_90);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_89._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_89._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435497._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_90._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_89._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435497._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_90._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_90._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_91, &_pgen_tmp_89);
  Rescale(&_pgen_tmp_91._c0_poly, &_pgen_tmp_89._c0_poly);
  Rescale(&_pgen_tmp_91._c1_poly, &_pgen_tmp_89._c1_poly);
  Free_poly_data(&_pgen_tmp_89._c1_poly);
  Free_poly_data(&_pgen_tmp_89._c0_poly);
  _preg_268435498 = App_relu(_preg_268435497, _pgen_tmp_91);
  Free_poly_data(&_pgen_tmp_91._c1_poly);
  Free_poly_data(&_pgen_tmp_91._c0_poly);
  Free_poly_data(&_preg_268435497._c1_poly);
  Free_poly_data(&_preg_268435497._c0_poly);
  Copy_ciph(&_preg_268435499, &_preg_268435498);
  Free_poly_data(&_preg_268435498._c1_poly);
  Free_poly_data(&_preg_268435498._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n11, 0, sizeof(result_n11));
  Copy_ciph(&input_dup_n11, &_preg_268435499);
  Free_poly_data(&_preg_268435499._c1_poly);
  Free_poly_data(&_preg_268435499._c0_poly);
  for (index_khw1_n11 = 0; index_khw1_n11 < 9; index_khw1_n11 = index_khw1_n11 + 1) {
    _preg_268435686 = Rotate(input_dup_n11, _cst_173[index_khw1_n11]);
    Copy_ciph(&input_roll_n11[index_khw1_n11], &_preg_268435686);
    Free_poly_data(&_preg_268435686._c1_poly);
    Free_poly_data(&_preg_268435686._c0_poly);
  }
  Free_poly_data(&input_dup_n11._c1_poly);
  Free_poly_data(&input_dup_n11._c0_poly);
  for (index_cin_n11 = 0; index_cin_n11 < 16; index_cin_n11 = index_cin_n11 + 1) {
    memset(&result_cin_n11, 0, sizeof(result_cin_n11));
    for (index_khw2_n11 = 0; index_khw2_n11 < 9; index_khw2_n11 = index_khw2_n11 + 1) {
      _rescale_tmp_1169 = input_roll_n11[index_khw2_n11];
      Pt_from_msg(&_pgen_tmp_92, index_khw2_n11 + (index_cin_n11 * 9) + 1352 /* cst_171_0 */, 16384, Sc_degree(&_rescale_tmp_1169), Level(&_rescale_tmp_1169));
      Init_ciph_up_scale_plain(&_pgen_tmp_93, &_rescale_tmp_1169, &_pgen_tmp_92);
      Init_ciph_same_scale(&result_cin_n11, &result_cin_n11, &_pgen_tmp_93);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n11._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_93._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_1169._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_92._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_93._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_1169._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_92._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n11._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n11._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_93._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n11._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n11._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_93._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435687 = Rotate(result_cin_n11, -16384);
    Init_ciph_same_scale(&_pgen_tmp_94, &result_cin_n11, &_preg_268435687);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_94._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_94._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n11._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435687._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_94._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n11._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435687._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435688 = Rotate(_pgen_tmp_94, index_cin_n11 * 1024);
    Init_ciph_same_scale(&result_n11, &result_n11, &_preg_268435688);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n11._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n11._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n11._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435688._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n11._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n11._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435688._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435687._c1_poly);
    Free_poly_data(&_preg_268435687._c0_poly);
    Free_poly_data(&_preg_268435688._c1_poly);
    Free_poly_data(&_preg_268435688._c0_poly);
  }
  Free_ciph_poly(input_roll_n11, 9);
  Free_poly_data(&_pgen_tmp_92._poly);
  Free_poly_data(&_pgen_tmp_93._c1_poly);
  Free_poly_data(&_pgen_tmp_93._c0_poly);
  Free_poly_data(&result_cin_n11._c1_poly);
  Free_poly_data(&result_cin_n11._c0_poly);
  Free_poly_data(&_pgen_tmp_94._c1_poly);
  Free_poly_data(&_pgen_tmp_94._c0_poly);
  Pt_from_msg(&_pgen_tmp_95, 1496 /* cst_172 */, 16384, Sc_degree(&result_n11), Level(&result_n11));
  Init_ciph_same_scale_plain(&result_n11, &result_n11, &_pgen_tmp_95);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n11._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n11._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n11._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_95._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n11._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n11._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_95._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_5644, &result_n11);
  Rescale(&_rescale_tmp_5644._c0_poly, &result_n11._c0_poly);
  Rescale(&_rescale_tmp_5644._c1_poly, &result_n11._c1_poly);
  Pt_from_msg(&_pgen_tmp_96, 1497 /* cst_174 */, 16384, Sc_degree(&_rescale_tmp_5644), Level(&_rescale_tmp_5644));
  Init_ciph_up_scale_plain(&result_n11, &_rescale_tmp_5644, &_pgen_tmp_96);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n11._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n11._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5644._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_96._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n11._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5644._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_96._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_96._poly);
    Free_poly_data(&_rescale_tmp_5644._c1_poly);
    Free_poly_data(&_rescale_tmp_5644._c0_poly);
  }
  Copy_ciph(&_preg_268435500, &result_n11);
  Free_poly_data(&result_n11._c1_poly);
  Free_poly_data(&result_n11._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::add");
  Init_ciph_down_scale(&_pgen_tmp_97, &_preg_268435500);
  Rescale(&_pgen_tmp_97._c0_poly, &_preg_268435500._c0_poly);
  Rescale(&_pgen_tmp_97._c1_poly, &_preg_268435500._c1_poly);
  Free_poly_data(&_preg_268435500._c1_poly);
  Free_poly_data(&_preg_268435500._c0_poly);
  Init_ciph_same_scale(&_preg_268435501, &_pgen_tmp_97, &_preg_268435495);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_preg_268435501._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_preg_268435501._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_97._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435495._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_preg_268435501._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_97._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435495._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435495._c1_poly);
    Free_poly_data(&_preg_268435495._c0_poly);
    Free_poly_data(&_pgen_tmp_97._c1_poly);
    Free_poly_data(&_pgen_tmp_97._c0_poly);
  }
  Tm_taken("Tensor::add");
  Bootstrap(&_preg_268435502, &_preg_268435501, 15);
  Free_poly_data(&_preg_268435501._c1_poly);
  Free_poly_data(&_preg_268435501._c0_poly);
  Encode_float(&_pgen_tmp_99, &_cst_455, 1, 1, Level(&_preg_268435502));
  Init_ciph_up_scale_plain(&_pgen_tmp_98, &_preg_268435502, &_pgen_tmp_99);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_98._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_98._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435502._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_99._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_98._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435502._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_99._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_99._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_100, &_pgen_tmp_98);
  Rescale(&_pgen_tmp_100._c0_poly, &_pgen_tmp_98._c0_poly);
  Rescale(&_pgen_tmp_100._c1_poly, &_pgen_tmp_98._c1_poly);
  Free_poly_data(&_pgen_tmp_98._c1_poly);
  Free_poly_data(&_pgen_tmp_98._c0_poly);
  _preg_268435503 = App_relu(_preg_268435502, _pgen_tmp_100);
  Free_poly_data(&_pgen_tmp_100._c1_poly);
  Free_poly_data(&_pgen_tmp_100._c0_poly);
  Free_poly_data(&_preg_268435502._c1_poly);
  Free_poly_data(&_preg_268435502._c0_poly);
  Copy_ciph(&_preg_268435504, &_preg_268435503);
  Free_poly_data(&_preg_268435503._c1_poly);
  Free_poly_data(&_preg_268435503._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n12, 0, sizeof(result_n12));
  Copy_ciph(&input_dup_n12, &_preg_268435504);
  for (index_khw1_n12 = 0; index_khw1_n12 < 9; index_khw1_n12 = index_khw1_n12 + 1) {
    _preg_268435689 = Rotate(input_dup_n12, _cst_180[index_khw1_n12]);
    Copy_ciph(&input_roll_n12[index_khw1_n12], &_preg_268435689);
    Free_poly_data(&_preg_268435689._c1_poly);
    Free_poly_data(&_preg_268435689._c0_poly);
  }
  Free_poly_data(&input_dup_n12._c1_poly);
  Free_poly_data(&input_dup_n12._c0_poly);
  for (index_cin_n12 = 0; index_cin_n12 < 16; index_cin_n12 = index_cin_n12 + 1) {
    memset(&result_cin_n12, 0, sizeof(result_cin_n12));
    for (index_khw2_n12 = 0; index_khw2_n12 < 9; index_khw2_n12 = index_khw2_n12 + 1) {
      _rescale_tmp_1283 = input_roll_n12[index_khw2_n12];
      Pt_from_msg(&_pgen_tmp_101, index_khw2_n12 + (index_cin_n12 * 9) + 1498 /* cst_178_0 */, 16384, Sc_degree(&_rescale_tmp_1283), Level(&_rescale_tmp_1283));
      Init_ciph_up_scale_plain(&_pgen_tmp_102, &_rescale_tmp_1283, &_pgen_tmp_101);
      Init_ciph_same_scale(&result_cin_n12, &result_cin_n12, &_pgen_tmp_102);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n12._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_102._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_1283._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_101._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_102._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_1283._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_101._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n12._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n12._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_102._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n12._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n12._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_102._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435690 = Rotate(result_cin_n12, -16384);
    Init_ciph_same_scale(&_pgen_tmp_103, &result_cin_n12, &_preg_268435690);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_103._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_103._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n12._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435690._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_103._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n12._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435690._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435691 = Rotate(_pgen_tmp_103, index_cin_n12 * 1024);
    Init_ciph_same_scale(&result_n12, &result_n12, &_preg_268435691);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n12._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n12._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n12._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435691._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n12._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n12._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435691._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435690._c1_poly);
    Free_poly_data(&_preg_268435690._c0_poly);
    Free_poly_data(&_preg_268435691._c1_poly);
    Free_poly_data(&_preg_268435691._c0_poly);
  }
  Free_ciph_poly(input_roll_n12, 9);
  Free_poly_data(&_pgen_tmp_101._poly);
  Free_poly_data(&_pgen_tmp_102._c1_poly);
  Free_poly_data(&_pgen_tmp_102._c0_poly);
  Free_poly_data(&result_cin_n12._c1_poly);
  Free_poly_data(&result_cin_n12._c0_poly);
  Free_poly_data(&_pgen_tmp_103._c1_poly);
  Free_poly_data(&_pgen_tmp_103._c0_poly);
  Pt_from_msg(&_pgen_tmp_104, 1642 /* cst_179 */, 16384, Sc_degree(&result_n12), Level(&result_n12));
  Init_ciph_same_scale_plain(&result_n12, &result_n12, &_pgen_tmp_104);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n12._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n12._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n12._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_104._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n12._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n12._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_104._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_5657, &result_n12);
  Rescale(&_rescale_tmp_5657._c0_poly, &result_n12._c0_poly);
  Rescale(&_rescale_tmp_5657._c1_poly, &result_n12._c1_poly);
  Pt_from_msg(&_pgen_tmp_105, 1643 /* cst_181 */, 16384, Sc_degree(&_rescale_tmp_5657), Level(&_rescale_tmp_5657));
  Init_ciph_up_scale_plain(&result_n12, &_rescale_tmp_5657, &_pgen_tmp_105);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n12._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n12._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5657._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_105._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n12._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5657._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_105._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_105._poly);
    Free_poly_data(&_rescale_tmp_5657._c1_poly);
    Free_poly_data(&_rescale_tmp_5657._c0_poly);
  }
  Copy_ciph(&_preg_268435505, &result_n12);
  Free_poly_data(&result_n12._c1_poly);
  Free_poly_data(&result_n12._c0_poly);
  Tm_taken("Tensor::conv");
  Init_ciph_down_scale(&_pgen_tmp_106, &_preg_268435505);
  Rescale(&_pgen_tmp_106._c0_poly, &_preg_268435505._c0_poly);
  Rescale(&_pgen_tmp_106._c1_poly, &_preg_268435505._c1_poly);
  Free_poly_data(&_preg_268435505._c1_poly);
  Free_poly_data(&_preg_268435505._c0_poly);
  Bootstrap(&_preg_268435506, &_pgen_tmp_106, 15);
  Free_poly_data(&_pgen_tmp_106._c1_poly);
  Free_poly_data(&_pgen_tmp_106._c0_poly);
  Encode_float(&_pgen_tmp_108, &_cst_456, 1, 1, Level(&_preg_268435506));
  Init_ciph_up_scale_plain(&_pgen_tmp_107, &_preg_268435506, &_pgen_tmp_108);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_107._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_107._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435506._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_108._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_107._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435506._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_108._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_108._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_109, &_pgen_tmp_107);
  Rescale(&_pgen_tmp_109._c0_poly, &_pgen_tmp_107._c0_poly);
  Rescale(&_pgen_tmp_109._c1_poly, &_pgen_tmp_107._c1_poly);
  Free_poly_data(&_pgen_tmp_107._c1_poly);
  Free_poly_data(&_pgen_tmp_107._c0_poly);
  _preg_268435507 = App_relu(_preg_268435506, _pgen_tmp_109);
  Free_poly_data(&_pgen_tmp_109._c1_poly);
  Free_poly_data(&_pgen_tmp_109._c0_poly);
  Free_poly_data(&_preg_268435506._c1_poly);
  Free_poly_data(&_preg_268435506._c0_poly);
  Copy_ciph(&_preg_268435508, &_preg_268435507);
  Free_poly_data(&_preg_268435507._c1_poly);
  Free_poly_data(&_preg_268435507._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n13, 0, sizeof(result_n13));
  Copy_ciph(&input_dup_n13, &_preg_268435508);
  Free_poly_data(&_preg_268435508._c1_poly);
  Free_poly_data(&_preg_268435508._c0_poly);
  for (index_khw1_n13 = 0; index_khw1_n13 < 9; index_khw1_n13 = index_khw1_n13 + 1) {
    _preg_268435692 = Rotate(input_dup_n13, _cst_186[index_khw1_n13]);
    Copy_ciph(&input_roll_n13[index_khw1_n13], &_preg_268435692);
    Free_poly_data(&_preg_268435692._c1_poly);
    Free_poly_data(&_preg_268435692._c0_poly);
  }
  Free_poly_data(&input_dup_n13._c1_poly);
  Free_poly_data(&input_dup_n13._c0_poly);
  for (index_cin_n13 = 0; index_cin_n13 < 16; index_cin_n13 = index_cin_n13 + 1) {
    memset(&result_cin_n13, 0, sizeof(result_cin_n13));
    for (index_khw2_n13 = 0; index_khw2_n13 < 9; index_khw2_n13 = index_khw2_n13 + 1) {
      _rescale_tmp_1391 = input_roll_n13[index_khw2_n13];
      Pt_from_msg(&_pgen_tmp_110, index_khw2_n13 + (index_cin_n13 * 9) + 1644 /* cst_184_0 */, 16384, Sc_degree(&_rescale_tmp_1391), Level(&_rescale_tmp_1391));
      Init_ciph_up_scale_plain(&_pgen_tmp_111, &_rescale_tmp_1391, &_pgen_tmp_110);
      Init_ciph_same_scale(&result_cin_n13, &result_cin_n13, &_pgen_tmp_111);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n13._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_111._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_1391._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_110._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_111._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_1391._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_110._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n13._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n13._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_111._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n13._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n13._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_111._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435693 = Rotate(result_cin_n13, -16384);
    Init_ciph_same_scale(&_pgen_tmp_112, &result_cin_n13, &_preg_268435693);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_112._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_112._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n13._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435693._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_112._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n13._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435693._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435694 = Rotate(_pgen_tmp_112, index_cin_n13 * 1024);
    Init_ciph_same_scale(&result_n13, &result_n13, &_preg_268435694);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n13._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n13._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n13._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435694._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n13._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n13._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435694._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435693._c1_poly);
    Free_poly_data(&_preg_268435693._c0_poly);
    Free_poly_data(&_preg_268435694._c1_poly);
    Free_poly_data(&_preg_268435694._c0_poly);
  }
  Free_ciph_poly(input_roll_n13, 9);
  Free_poly_data(&_pgen_tmp_110._poly);
  Free_poly_data(&_pgen_tmp_111._c1_poly);
  Free_poly_data(&_pgen_tmp_111._c0_poly);
  Free_poly_data(&result_cin_n13._c1_poly);
  Free_poly_data(&result_cin_n13._c0_poly);
  Free_poly_data(&_pgen_tmp_112._c1_poly);
  Free_poly_data(&_pgen_tmp_112._c0_poly);
  Pt_from_msg(&_pgen_tmp_113, 1788 /* cst_185 */, 16384, Sc_degree(&result_n13), Level(&result_n13));
  Init_ciph_same_scale_plain(&result_n13, &result_n13, &_pgen_tmp_113);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n13._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n13._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n13._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_113._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n13._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n13._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_113._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_5670, &result_n13);
  Rescale(&_rescale_tmp_5670._c0_poly, &result_n13._c0_poly);
  Rescale(&_rescale_tmp_5670._c1_poly, &result_n13._c1_poly);
  Pt_from_msg(&_pgen_tmp_114, 1789 /* cst_187 */, 16384, Sc_degree(&_rescale_tmp_5670), Level(&_rescale_tmp_5670));
  Init_ciph_up_scale_plain(&result_n13, &_rescale_tmp_5670, &_pgen_tmp_114);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n13._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n13._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5670._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_114._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n13._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5670._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_114._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_114._poly);
    Free_poly_data(&_rescale_tmp_5670._c1_poly);
    Free_poly_data(&_rescale_tmp_5670._c0_poly);
  }
  Copy_ciph(&_preg_268435509, &result_n13);
  Free_poly_data(&result_n13._c1_poly);
  Free_poly_data(&result_n13._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::add");
  Init_ciph_down_scale(&_pgen_tmp_115, &_preg_268435509);
  Rescale(&_pgen_tmp_115._c0_poly, &_preg_268435509._c0_poly);
  Rescale(&_pgen_tmp_115._c1_poly, &_preg_268435509._c1_poly);
  Free_poly_data(&_preg_268435509._c1_poly);
  Free_poly_data(&_preg_268435509._c0_poly);
  Init_ciph_same_scale(&_preg_268435510, &_pgen_tmp_115, &_preg_268435504);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_preg_268435510._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_preg_268435510._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_115._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435504._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_preg_268435510._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_115._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435504._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435504._c1_poly);
    Free_poly_data(&_preg_268435504._c0_poly);
    Free_poly_data(&_pgen_tmp_115._c1_poly);
    Free_poly_data(&_pgen_tmp_115._c0_poly);
  }
  Tm_taken("Tensor::add");
  Bootstrap(&_preg_268435511, &_preg_268435510, 15);
  Free_poly_data(&_preg_268435510._c1_poly);
  Free_poly_data(&_preg_268435510._c0_poly);
  Encode_float(&_pgen_tmp_117, &_cst_457, 1, 1, Level(&_preg_268435511));
  Init_ciph_up_scale_plain(&_pgen_tmp_116, &_preg_268435511, &_pgen_tmp_117);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_116._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_116._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435511._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_117._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_116._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435511._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_117._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_117._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_118, &_pgen_tmp_116);
  Rescale(&_pgen_tmp_118._c0_poly, &_pgen_tmp_116._c0_poly);
  Rescale(&_pgen_tmp_118._c1_poly, &_pgen_tmp_116._c1_poly);
  Free_poly_data(&_pgen_tmp_116._c1_poly);
  Free_poly_data(&_pgen_tmp_116._c0_poly);
  _preg_268435512 = App_relu(_preg_268435511, _pgen_tmp_118);
  Free_poly_data(&_pgen_tmp_118._c1_poly);
  Free_poly_data(&_pgen_tmp_118._c0_poly);
  Free_poly_data(&_preg_268435511._c1_poly);
  Free_poly_data(&_preg_268435511._c0_poly);
  Copy_ciph(&_preg_268435513, &_preg_268435512);
  Free_poly_data(&_preg_268435512._c1_poly);
  Free_poly_data(&_preg_268435512._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n14, 0, sizeof(result_n14));
  Copy_ciph(&input_dup_n14, &_preg_268435513);
  for (index_khw1_n14 = 0; index_khw1_n14 < 9; index_khw1_n14 = index_khw1_n14 + 1) {
    _preg_268435695 = Rotate(input_dup_n14, _cst_193[index_khw1_n14]);
    Copy_ciph(&input_roll_n14[index_khw1_n14], &_preg_268435695);
    Free_poly_data(&_preg_268435695._c1_poly);
    Free_poly_data(&_preg_268435695._c0_poly);
  }
  Free_poly_data(&input_dup_n14._c1_poly);
  Free_poly_data(&input_dup_n14._c0_poly);
  for (index_cin_n14 = 0; index_cin_n14 < 16; index_cin_n14 = index_cin_n14 + 1) {
    memset(&result_cin_n14, 0, sizeof(result_cin_n14));
    for (index_khw2_n14 = 0; index_khw2_n14 < 9; index_khw2_n14 = index_khw2_n14 + 1) {
      _rescale_tmp_1505 = input_roll_n14[index_khw2_n14];
      Pt_from_msg(&_pgen_tmp_119, index_khw2_n14 + (index_cin_n14 * 9) + 1790 /* cst_191_0 */, 16384, Sc_degree(&_rescale_tmp_1505), Level(&_rescale_tmp_1505));
      Init_ciph_up_scale_plain(&_pgen_tmp_120, &_rescale_tmp_1505, &_pgen_tmp_119);
      Init_ciph_same_scale(&result_cin_n14, &result_cin_n14, &_pgen_tmp_120);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n14._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_120._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_1505._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_119._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_120._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_1505._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_119._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n14._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n14._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_120._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n14._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n14._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_120._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435696 = Rotate(result_cin_n14, -16384);
    Init_ciph_same_scale(&_pgen_tmp_121, &result_cin_n14, &_preg_268435696);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_121._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_121._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n14._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435696._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_121._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n14._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435696._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435697 = Rotate(_pgen_tmp_121, index_cin_n14 * 1024);
    Init_ciph_same_scale(&result_n14, &result_n14, &_preg_268435697);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n14._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n14._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n14._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435697._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n14._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n14._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435697._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435696._c1_poly);
    Free_poly_data(&_preg_268435696._c0_poly);
    Free_poly_data(&_preg_268435697._c1_poly);
    Free_poly_data(&_preg_268435697._c0_poly);
  }
  Free_ciph_poly(input_roll_n14, 9);
  Free_poly_data(&_pgen_tmp_119._poly);
  Free_poly_data(&_pgen_tmp_120._c1_poly);
  Free_poly_data(&_pgen_tmp_120._c0_poly);
  Free_poly_data(&result_cin_n14._c1_poly);
  Free_poly_data(&result_cin_n14._c0_poly);
  Free_poly_data(&_pgen_tmp_121._c1_poly);
  Free_poly_data(&_pgen_tmp_121._c0_poly);
  Pt_from_msg(&_pgen_tmp_122, 1934 /* cst_192 */, 16384, Sc_degree(&result_n14), Level(&result_n14));
  Init_ciph_same_scale_plain(&result_n14, &result_n14, &_pgen_tmp_122);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n14._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n14._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n14._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_122._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n14._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n14._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_122._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_5683, &result_n14);
  Rescale(&_rescale_tmp_5683._c0_poly, &result_n14._c0_poly);
  Rescale(&_rescale_tmp_5683._c1_poly, &result_n14._c1_poly);
  Pt_from_msg(&_pgen_tmp_123, 1935 /* cst_194 */, 16384, Sc_degree(&_rescale_tmp_5683), Level(&_rescale_tmp_5683));
  Init_ciph_up_scale_plain(&result_n14, &_rescale_tmp_5683, &_pgen_tmp_123);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n14._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n14._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5683._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_123._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n14._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5683._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_123._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_123._poly);
    Free_poly_data(&_rescale_tmp_5683._c1_poly);
    Free_poly_data(&_rescale_tmp_5683._c0_poly);
  }
  Copy_ciph(&_preg_268435514, &result_n14);
  Free_poly_data(&result_n14._c1_poly);
  Free_poly_data(&result_n14._c0_poly);
  Tm_taken("Tensor::conv");
  Init_ciph_down_scale(&_pgen_tmp_124, &_preg_268435514);
  Rescale(&_pgen_tmp_124._c0_poly, &_preg_268435514._c0_poly);
  Rescale(&_pgen_tmp_124._c1_poly, &_preg_268435514._c1_poly);
  Free_poly_data(&_preg_268435514._c1_poly);
  Free_poly_data(&_preg_268435514._c0_poly);
  Bootstrap(&_preg_268435515, &_pgen_tmp_124, 15);
  Free_poly_data(&_pgen_tmp_124._c1_poly);
  Free_poly_data(&_pgen_tmp_124._c0_poly);
  Encode_float(&_pgen_tmp_126, &_cst_458, 1, 1, Level(&_preg_268435515));
  Init_ciph_up_scale_plain(&_pgen_tmp_125, &_preg_268435515, &_pgen_tmp_126);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_125._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_125._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435515._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_126._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_125._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435515._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_126._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_126._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_127, &_pgen_tmp_125);
  Rescale(&_pgen_tmp_127._c0_poly, &_pgen_tmp_125._c0_poly);
  Rescale(&_pgen_tmp_127._c1_poly, &_pgen_tmp_125._c1_poly);
  Free_poly_data(&_pgen_tmp_125._c1_poly);
  Free_poly_data(&_pgen_tmp_125._c0_poly);
  _preg_268435516 = App_relu(_preg_268435515, _pgen_tmp_127);
  Free_poly_data(&_pgen_tmp_127._c1_poly);
  Free_poly_data(&_pgen_tmp_127._c0_poly);
  Free_poly_data(&_preg_268435515._c1_poly);
  Free_poly_data(&_preg_268435515._c0_poly);
  Copy_ciph(&_preg_268435517, &_preg_268435516);
  Free_poly_data(&_preg_268435516._c1_poly);
  Free_poly_data(&_preg_268435516._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n15, 0, sizeof(result_n15));
  Copy_ciph(&input_dup_n15, &_preg_268435517);
  Free_poly_data(&_preg_268435517._c1_poly);
  Free_poly_data(&_preg_268435517._c0_poly);
  for (index_khw1_n15 = 0; index_khw1_n15 < 9; index_khw1_n15 = index_khw1_n15 + 1) {
    _preg_268435698 = Rotate(input_dup_n15, _cst_199[index_khw1_n15]);
    Copy_ciph(&input_roll_n15[index_khw1_n15], &_preg_268435698);
    Free_poly_data(&_preg_268435698._c1_poly);
    Free_poly_data(&_preg_268435698._c0_poly);
  }
  Free_poly_data(&input_dup_n15._c1_poly);
  Free_poly_data(&input_dup_n15._c0_poly);
  for (index_cin_n15 = 0; index_cin_n15 < 16; index_cin_n15 = index_cin_n15 + 1) {
    memset(&result_cin_n15, 0, sizeof(result_cin_n15));
    for (index_khw2_n15 = 0; index_khw2_n15 < 9; index_khw2_n15 = index_khw2_n15 + 1) {
      _rescale_tmp_1613 = input_roll_n15[index_khw2_n15];
      Pt_from_msg(&_pgen_tmp_128, index_khw2_n15 + (index_cin_n15 * 9) + 1936 /* cst_197_0 */, 16384, Sc_degree(&_rescale_tmp_1613), Level(&_rescale_tmp_1613));
      Init_ciph_up_scale_plain(&_pgen_tmp_129, &_rescale_tmp_1613, &_pgen_tmp_128);
      Init_ciph_same_scale(&result_cin_n15, &result_cin_n15, &_pgen_tmp_129);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n15._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_129._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_1613._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_128._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_129._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_1613._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_128._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n15._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n15._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_129._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n15._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n15._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_129._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435699 = Rotate(result_cin_n15, -16384);
    Init_ciph_same_scale(&_pgen_tmp_130, &result_cin_n15, &_preg_268435699);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_130._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_130._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n15._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435699._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_130._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n15._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435699._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435700 = Rotate(_pgen_tmp_130, index_cin_n15 * 1024);
    Init_ciph_same_scale(&result_n15, &result_n15, &_preg_268435700);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n15._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n15._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n15._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435700._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n15._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n15._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435700._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435699._c1_poly);
    Free_poly_data(&_preg_268435699._c0_poly);
    Free_poly_data(&_preg_268435700._c1_poly);
    Free_poly_data(&_preg_268435700._c0_poly);
  }
  Free_ciph_poly(input_roll_n15, 9);
  Free_poly_data(&_pgen_tmp_128._poly);
  Free_poly_data(&_pgen_tmp_129._c1_poly);
  Free_poly_data(&_pgen_tmp_129._c0_poly);
  Free_poly_data(&result_cin_n15._c1_poly);
  Free_poly_data(&result_cin_n15._c0_poly);
  Free_poly_data(&_pgen_tmp_130._c1_poly);
  Free_poly_data(&_pgen_tmp_130._c0_poly);
  Pt_from_msg(&_pgen_tmp_131, 2080 /* cst_198 */, 16384, Sc_degree(&result_n15), Level(&result_n15));
  Init_ciph_same_scale_plain(&result_n15, &result_n15, &_pgen_tmp_131);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n15._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n15._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n15._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_131._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n15._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n15._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_131._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_5696, &result_n15);
  Rescale(&_rescale_tmp_5696._c0_poly, &result_n15._c0_poly);
  Rescale(&_rescale_tmp_5696._c1_poly, &result_n15._c1_poly);
  Pt_from_msg(&_pgen_tmp_132, 2081 /* cst_200 */, 16384, Sc_degree(&_rescale_tmp_5696), Level(&_rescale_tmp_5696));
  Init_ciph_up_scale_plain(&result_n15, &_rescale_tmp_5696, &_pgen_tmp_132);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n15._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n15._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5696._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_132._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n15._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5696._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_132._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_132._poly);
    Free_poly_data(&_rescale_tmp_5696._c1_poly);
    Free_poly_data(&_rescale_tmp_5696._c0_poly);
  }
  Copy_ciph(&_preg_268435518, &result_n15);
  Free_poly_data(&result_n15._c1_poly);
  Free_poly_data(&result_n15._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::add");
  Init_ciph_down_scale(&_pgen_tmp_133, &_preg_268435518);
  Rescale(&_pgen_tmp_133._c0_poly, &_preg_268435518._c0_poly);
  Rescale(&_pgen_tmp_133._c1_poly, &_preg_268435518._c1_poly);
  Free_poly_data(&_preg_268435518._c1_poly);
  Free_poly_data(&_preg_268435518._c0_poly);
  Init_ciph_same_scale(&_preg_268435519, &_pgen_tmp_133, &_preg_268435513);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_preg_268435519._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_preg_268435519._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_133._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435513._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_preg_268435519._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_133._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435513._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435513._c1_poly);
    Free_poly_data(&_preg_268435513._c0_poly);
    Free_poly_data(&_pgen_tmp_133._c1_poly);
    Free_poly_data(&_pgen_tmp_133._c0_poly);
  }
  Tm_taken("Tensor::add");
  Bootstrap(&_preg_268435520, &_preg_268435519, 18);
  Free_poly_data(&_preg_268435519._c1_poly);
  Free_poly_data(&_preg_268435519._c0_poly);
  Encode_float(&_pgen_tmp_135, &_cst_459, 1, 1, Level(&_preg_268435520));
  Init_ciph_up_scale_plain(&_pgen_tmp_134, &_preg_268435520, &_pgen_tmp_135);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_134._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_134._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435520._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_135._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_134._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435520._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_135._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_135._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_136, &_pgen_tmp_134);
  Rescale(&_pgen_tmp_136._c0_poly, &_pgen_tmp_134._c0_poly);
  Rescale(&_pgen_tmp_136._c1_poly, &_pgen_tmp_134._c1_poly);
  Free_poly_data(&_pgen_tmp_134._c1_poly);
  Free_poly_data(&_pgen_tmp_134._c0_poly);
  _preg_268435521 = App_relu(_preg_268435520, _pgen_tmp_136);
  Free_poly_data(&_pgen_tmp_136._c1_poly);
  Free_poly_data(&_pgen_tmp_136._c0_poly);
  Free_poly_data(&_preg_268435520._c1_poly);
  Free_poly_data(&_preg_268435520._c0_poly);
  Copy_ciph(&_preg_268435522, &_preg_268435521);
  Free_poly_data(&_preg_268435521._c1_poly);
  Free_poly_data(&_preg_268435521._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n16, 0, sizeof(result_n16));
  _preg_268435701 = Rotate(_preg_268435522, -16384);
  Init_ciph_same_scale(&input_dup_n16, &_preg_268435522, &_preg_268435701);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&input_dup_n16._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&input_dup_n16._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435522._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435701._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&input_dup_n16._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435522._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435701._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435701._c1_poly);
    Free_poly_data(&_preg_268435701._c0_poly);
  }
  for (index_khw1_n16 = 0; index_khw1_n16 < 9; index_khw1_n16 = index_khw1_n16 + 1) {
    _preg_268435702 = Rotate(input_dup_n16, _cst_206[index_khw1_n16]);
    Copy_ciph(&input_roll_n16[index_khw1_n16], &_preg_268435702);
    Free_poly_data(&_preg_268435702._c1_poly);
    Free_poly_data(&_preg_268435702._c0_poly);
  }
  Free_poly_data(&input_dup_n16._c1_poly);
  Free_poly_data(&input_dup_n16._c0_poly);
  for (index_cin_n16 = 0; index_cin_n16 < 16; index_cin_n16 = index_cin_n16 + 1) {
    memset(&result_cin_n16, 0, sizeof(result_cin_n16));
    for (index_khw2_n16 = 0; index_khw2_n16 < 9; index_khw2_n16 = index_khw2_n16 + 1) {
      _rescale_tmp_1731 = input_roll_n16[index_khw2_n16];
      Pt_from_msg(&_pgen_tmp_137, index_khw2_n16 + (index_cin_n16 * 9) + 2082 /* cst_204_0 */, 32768, Sc_degree(&_rescale_tmp_1731), Level(&_rescale_tmp_1731));
      Init_ciph_up_scale_plain(&_pgen_tmp_138, &_rescale_tmp_1731, &_pgen_tmp_137);
      Init_ciph_same_scale(&result_cin_n16, &result_cin_n16, &_pgen_tmp_138);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n16._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_138._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_1731._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_137._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_138._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_1731._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_137._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n16._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n16._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_138._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n16._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n16._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_138._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435703 = Rotate(result_cin_n16, index_cin_n16 * 1024);
    Init_ciph_same_scale(&result_n16, &result_n16, &_preg_268435703);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n16._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n16._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n16._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435703._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n16._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n16._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435703._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435703._c1_poly);
    Free_poly_data(&_preg_268435703._c0_poly);
  }
  Free_ciph_poly(input_roll_n16, 9);
  Free_poly_data(&_pgen_tmp_137._poly);
  Free_poly_data(&_pgen_tmp_138._c1_poly);
  Free_poly_data(&_pgen_tmp_138._c0_poly);
  Free_poly_data(&result_cin_n16._c1_poly);
  Free_poly_data(&result_cin_n16._c0_poly);
  Pt_from_msg(&_pgen_tmp_139, 2226 /* cst_205 */, 32768, Sc_degree(&result_n16), Level(&result_n16));
  Init_ciph_same_scale_plain(&result_n16, &result_n16, &_pgen_tmp_139);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n16._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n16._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n16._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_139._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n16._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n16._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_139._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_5709, &result_n16);
  Rescale(&_rescale_tmp_5709._c0_poly, &result_n16._c0_poly);
  Rescale(&_rescale_tmp_5709._c1_poly, &result_n16._c1_poly);
  Pt_from_msg(&_pgen_tmp_140, 2227 /* cst_207 */, 32768, Sc_degree(&_rescale_tmp_5709), Level(&_rescale_tmp_5709));
  Init_ciph_up_scale_plain(&result_n16, &_rescale_tmp_5709, &_pgen_tmp_140);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n16._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n16._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5709._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_140._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n16._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5709._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_140._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_140._poly);
    Free_poly_data(&_rescale_tmp_5709._c1_poly);
    Free_poly_data(&_rescale_tmp_5709._c0_poly);
  }
  Copy_ciph(&orig_result16, &result_n16);
  Free_poly_data(&result_n16._c1_poly);
  Free_poly_data(&result_n16._c0_poly);
  memset(&comb_row_result17, 0, sizeof(comb_row_result17));
  for (combine_row_index_n17 = 0; combine_row_index_n17 < 16; combine_row_index_n17 = combine_row_index_n17 + 1) {
    _preg_268435704 = Rotate(orig_result16, combine_row_index_n17 * 1);
    Copy_ciph(&_ckks_gen_tmp_1793, &_preg_268435704);
    Init_ciph_down_scale(&_rescale_tmp_5715, &_ckks_gen_tmp_1793);
    Rescale(&_rescale_tmp_5715._c0_poly, &_ckks_gen_tmp_1793._c0_poly);
    Rescale(&_rescale_tmp_5715._c1_poly, &_ckks_gen_tmp_1793._c1_poly);
    Pt_from_msg(&_pgen_tmp_141, combine_row_index_n17 + 2228 /* cst_208_0 */, 32768, Sc_degree(&_rescale_tmp_5715), Level(&_rescale_tmp_5715));
    Init_ciph_up_scale_plain(&_pgen_tmp_142, &_rescale_tmp_5715, &_pgen_tmp_141);
    Init_ciph_same_scale(&comb_row_result17, &comb_row_result17, &_pgen_tmp_142);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&comb_row_result17._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_142._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5715._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_141._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_142._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5715._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_141._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_row_result17._c0_poly, _pgen_rns_idx, degree), Coeffs(&comb_row_result17._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_142._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_row_result17._c1_poly, _pgen_rns_idx, degree), Coeffs(&comb_row_result17._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_142._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435704._c1_poly);
    Free_poly_data(&_preg_268435704._c0_poly);
  }
  Free_poly_data(&orig_result16._c1_poly);
  Free_poly_data(&orig_result16._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_1793._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_1793._c0_poly);
  Free_poly_data(&_pgen_tmp_141._poly);
  Free_poly_data(&_rescale_tmp_5715._c1_poly);
  Free_poly_data(&_rescale_tmp_5715._c0_poly);
  Free_poly_data(&_pgen_tmp_142._c1_poly);
  Free_poly_data(&_pgen_tmp_142._c0_poly);
  memset(&comb_rc_result17, 0, sizeof(comb_rc_result17));
  for (combine_rc_index_n17 = 0; combine_rc_index_n17 < 16; combine_rc_index_n17 = combine_rc_index_n17 + 1) {
    _preg_268435705 = Rotate(comb_row_result17, combine_rc_index_n17 * 48);
    Copy_ciph(&_ckks_gen_tmp_1824, &_preg_268435705);
    Init_ciph_down_scale(&_rescale_tmp_5721, &_ckks_gen_tmp_1824);
    Rescale(&_rescale_tmp_5721._c0_poly, &_ckks_gen_tmp_1824._c0_poly);
    Rescale(&_rescale_tmp_5721._c1_poly, &_ckks_gen_tmp_1824._c1_poly);
    Pt_from_msg(&_pgen_tmp_143, combine_rc_index_n17 + 2244 /* cst_209_0 */, 32768, Sc_degree(&_rescale_tmp_5721), Level(&_rescale_tmp_5721));
    Init_ciph_up_scale_plain(&_pgen_tmp_144, &_rescale_tmp_5721, &_pgen_tmp_143);
    Init_ciph_same_scale(&comb_rc_result17, &comb_rc_result17, &_pgen_tmp_144);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&comb_rc_result17._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_144._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5721._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_143._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_144._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5721._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_143._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_rc_result17._c0_poly, _pgen_rns_idx, degree), Coeffs(&comb_rc_result17._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_144._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_rc_result17._c1_poly, _pgen_rns_idx, degree), Coeffs(&comb_rc_result17._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_144._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435705._c1_poly);
    Free_poly_data(&_preg_268435705._c0_poly);
  }
  Free_poly_data(&comb_row_result17._c1_poly);
  Free_poly_data(&comb_row_result17._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_1824._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_1824._c0_poly);
  Free_poly_data(&_pgen_tmp_143._poly);
  Free_poly_data(&_rescale_tmp_5721._c1_poly);
  Free_poly_data(&_rescale_tmp_5721._c0_poly);
  Free_poly_data(&_pgen_tmp_144._c1_poly);
  Free_poly_data(&_pgen_tmp_144._c0_poly);
  memset(&final_result17, 0, sizeof(final_result17));
  for (combine_cc_index_n17 = 0; combine_cc_index_n17 < 32; combine_cc_index_n17 = combine_cc_index_n17 + 1) {
    _preg_268435706 = Rotate(comb_rc_result17, combine_cc_index_n17 * 768);
    Copy_ciph(&_ckks_gen_tmp_1855, &_preg_268435706);
    Init_ciph_down_scale(&_rescale_tmp_5727, &_ckks_gen_tmp_1855);
    Rescale(&_rescale_tmp_5727._c0_poly, &_ckks_gen_tmp_1855._c0_poly);
    Rescale(&_rescale_tmp_5727._c1_poly, &_ckks_gen_tmp_1855._c1_poly);
    Pt_from_msg(&_pgen_tmp_145, combine_cc_index_n17 + 2260 /* cst_210_0 */, 32768, Sc_degree(&_rescale_tmp_5727), Level(&_rescale_tmp_5727));
    Init_ciph_up_scale_plain(&_pgen_tmp_146, &_rescale_tmp_5727, &_pgen_tmp_145);
    Init_ciph_same_scale(&final_result17, &final_result17, &_pgen_tmp_146);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&final_result17._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_146._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5727._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_145._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_146._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5727._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_145._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&final_result17._c0_poly, _pgen_rns_idx, degree), Coeffs(&final_result17._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_146._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&final_result17._c1_poly, _pgen_rns_idx, degree), Coeffs(&final_result17._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_146._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435706._c1_poly);
    Free_poly_data(&_preg_268435706._c0_poly);
  }
  Free_poly_data(&comb_rc_result17._c1_poly);
  Free_poly_data(&comb_rc_result17._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_1855._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_1855._c0_poly);
  Free_poly_data(&_pgen_tmp_145._poly);
  Free_poly_data(&_rescale_tmp_5727._c1_poly);
  Free_poly_data(&_rescale_tmp_5727._c0_poly);
  Free_poly_data(&_pgen_tmp_146._c1_poly);
  Free_poly_data(&_pgen_tmp_146._c0_poly);
  Copy_ciph(&_preg_268435523, &final_result17);
  Free_poly_data(&final_result17._c1_poly);
  Free_poly_data(&final_result17._c0_poly);
  Tm_taken("Tensor::conv");
  Init_ciph_down_scale(&_pgen_tmp_147, &_preg_268435523);
  Rescale(&_pgen_tmp_147._c0_poly, &_preg_268435523._c0_poly);
  Rescale(&_pgen_tmp_147._c1_poly, &_preg_268435523._c1_poly);
  Free_poly_data(&_preg_268435523._c1_poly);
  Free_poly_data(&_preg_268435523._c0_poly);
  Bootstrap(&_preg_268435524, &_pgen_tmp_147, 15);
  Free_poly_data(&_pgen_tmp_147._c1_poly);
  Free_poly_data(&_pgen_tmp_147._c0_poly);
  Encode_float(&_pgen_tmp_149, &_cst_460, 1, 1, Level(&_preg_268435524));
  Init_ciph_up_scale_plain(&_pgen_tmp_148, &_preg_268435524, &_pgen_tmp_149);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_148._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_148._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435524._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_149._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_148._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435524._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_149._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_149._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_150, &_pgen_tmp_148);
  Rescale(&_pgen_tmp_150._c0_poly, &_pgen_tmp_148._c0_poly);
  Rescale(&_pgen_tmp_150._c1_poly, &_pgen_tmp_148._c1_poly);
  Free_poly_data(&_pgen_tmp_148._c1_poly);
  Free_poly_data(&_pgen_tmp_148._c0_poly);
  _preg_268435525 = App_relu(_preg_268435524, _pgen_tmp_150);
  Free_poly_data(&_pgen_tmp_150._c1_poly);
  Free_poly_data(&_pgen_tmp_150._c0_poly);
  Free_poly_data(&_preg_268435524._c1_poly);
  Free_poly_data(&_preg_268435524._c0_poly);
  Copy_ciph(&_preg_268435526, &_preg_268435525);
  Free_poly_data(&_preg_268435525._c1_poly);
  Free_poly_data(&_preg_268435525._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n18, 0, sizeof(result_n18));
  Copy_ciph(&input_dup_n18, &_preg_268435526);
  Free_poly_data(&_preg_268435526._c1_poly);
  Free_poly_data(&_preg_268435526._c0_poly);
  for (index_khw1_n18 = 0; index_khw1_n18 < 9; index_khw1_n18 = index_khw1_n18 + 1) {
    _preg_268435707 = Rotate(input_dup_n18, _cst_215[index_khw1_n18]);
    Copy_ciph(&input_roll_n18[index_khw1_n18], &_preg_268435707);
    Free_poly_data(&_preg_268435707._c1_poly);
    Free_poly_data(&_preg_268435707._c0_poly);
  }
  Free_poly_data(&input_dup_n18._c1_poly);
  Free_poly_data(&input_dup_n18._c0_poly);
  for (index_cin_n18 = 0; index_cin_n18 < 32; index_cin_n18 = index_cin_n18 + 1) {
    memset(&result_cin_n18, 0, sizeof(result_cin_n18));
    for (index_khw2_n18 = 0; index_khw2_n18 < 9; index_khw2_n18 = index_khw2_n18 + 1) {
      _rescale_tmp_1930 = input_roll_n18[index_khw2_n18];
      Pt_from_msg(&_pgen_tmp_151, index_khw2_n18 + (index_cin_n18 * 9) + 2292 /* cst_213_0 */, 8192, Sc_degree(&_rescale_tmp_1930), Level(&_rescale_tmp_1930));
      Init_ciph_up_scale_plain(&_pgen_tmp_152, &_rescale_tmp_1930, &_pgen_tmp_151);
      Init_ciph_same_scale(&result_cin_n18, &result_cin_n18, &_pgen_tmp_152);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n18._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_152._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_1930._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_151._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_152._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_1930._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_151._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n18._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n18._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_152._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n18._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n18._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_152._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435708 = Rotate(result_cin_n18, -8192);
    Init_ciph_same_scale(&_pgen_tmp_153, &result_cin_n18, &_preg_268435708);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_153._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_153._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n18._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435708._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_153._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n18._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435708._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435709 = Rotate(_pgen_tmp_153, index_cin_n18 * 256);
    Init_ciph_same_scale(&result_n18, &result_n18, &_preg_268435709);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n18._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n18._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n18._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435709._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n18._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n18._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435709._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435708._c1_poly);
    Free_poly_data(&_preg_268435708._c0_poly);
    Free_poly_data(&_preg_268435709._c1_poly);
    Free_poly_data(&_preg_268435709._c0_poly);
  }
  Free_ciph_poly(input_roll_n18, 9);
  Free_poly_data(&_pgen_tmp_151._poly);
  Free_poly_data(&_pgen_tmp_152._c1_poly);
  Free_poly_data(&_pgen_tmp_152._c0_poly);
  Free_poly_data(&result_cin_n18._c1_poly);
  Free_poly_data(&result_cin_n18._c0_poly);
  Free_poly_data(&_pgen_tmp_153._c1_poly);
  Free_poly_data(&_pgen_tmp_153._c0_poly);
  Pt_from_msg(&_pgen_tmp_154, 2580 /* cst_214 */, 8192, Sc_degree(&result_n18), Level(&result_n18));
  Init_ciph_same_scale_plain(&result_n18, &result_n18, &_pgen_tmp_154);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n18._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n18._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n18._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_154._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n18._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n18._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_154._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_5740, &result_n18);
  Rescale(&_rescale_tmp_5740._c0_poly, &result_n18._c0_poly);
  Rescale(&_rescale_tmp_5740._c1_poly, &result_n18._c1_poly);
  Pt_from_msg(&_pgen_tmp_155, 2581 /* cst_216 */, 8192, Sc_degree(&_rescale_tmp_5740), Level(&_rescale_tmp_5740));
  Init_ciph_up_scale_plain(&result_n18, &_rescale_tmp_5740, &_pgen_tmp_155);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n18._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n18._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5740._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_155._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n18._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5740._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_155._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_155._poly);
    Free_poly_data(&_rescale_tmp_5740._c1_poly);
    Free_poly_data(&_rescale_tmp_5740._c0_poly);
  }
  Copy_ciph(&_preg_268435527, &result_n18);
  Free_poly_data(&result_n18._c1_poly);
  Free_poly_data(&result_n18._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::conv");
  memset(&result_n19, 0, sizeof(result_n19));
  _preg_268435710 = Rotate(_preg_268435522, -16384);
  Init_ciph_same_scale(&input_dup_n19, &_preg_268435522, &_preg_268435710);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&input_dup_n19._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&input_dup_n19._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435522._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435710._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&input_dup_n19._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435522._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435710._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435710._c1_poly);
    Free_poly_data(&_preg_268435710._c0_poly);
    Free_poly_data(&_preg_268435522._c1_poly);
    Free_poly_data(&_preg_268435522._c0_poly);
  }
  for (index_khw1_n19 = 0; index_khw1_n19 < 1; index_khw1_n19 = index_khw1_n19 + 1) {
    _preg_268435711 = Rotate(input_dup_n19, _cst_221[index_khw1_n19]);
    Copy_ciph(&input_roll_n19[index_khw1_n19], &_preg_268435711);
    Free_poly_data(&_preg_268435711._c1_poly);
    Free_poly_data(&_preg_268435711._c0_poly);
  }
  Free_poly_data(&input_dup_n19._c1_poly);
  Free_poly_data(&input_dup_n19._c0_poly);
  for (index_cin_n19 = 0; index_cin_n19 < 16; index_cin_n19 = index_cin_n19 + 1) {
    memset(&result_cin_n19, 0, sizeof(result_cin_n19));
    for (index_khw2_n19 = 0; index_khw2_n19 < 1; index_khw2_n19 = index_khw2_n19 + 1) {
      _rescale_tmp_2032 = input_roll_n19[index_khw2_n19];
      Pt_from_msg(&_pgen_tmp_156, index_khw2_n19 + (index_cin_n19 * 1) + 2582 /* cst_219_0 */, 32768, Sc_degree(&_rescale_tmp_2032), Level(&_rescale_tmp_2032));
      Init_ciph_up_scale_plain(&_pgen_tmp_157, &_rescale_tmp_2032, &_pgen_tmp_156);
      Init_ciph_same_scale(&result_cin_n19, &result_cin_n19, &_pgen_tmp_157);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n19._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_157._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2032._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_156._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_157._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2032._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_156._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n19._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n19._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_157._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n19._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n19._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_157._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435712 = Rotate(result_cin_n19, index_cin_n19 * 1024);
    Init_ciph_same_scale(&result_n19, &result_n19, &_preg_268435712);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n19._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n19._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n19._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435712._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n19._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n19._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435712._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435712._c1_poly);
    Free_poly_data(&_preg_268435712._c0_poly);
  }
  Free_ciph_poly(input_roll_n19, 1);
  Free_poly_data(&_pgen_tmp_156._poly);
  Free_poly_data(&_pgen_tmp_157._c1_poly);
  Free_poly_data(&_pgen_tmp_157._c0_poly);
  Free_poly_data(&result_cin_n19._c1_poly);
  Free_poly_data(&result_cin_n19._c0_poly);
  Pt_from_msg(&_pgen_tmp_158, 2598 /* cst_220 */, 32768, Sc_degree(&result_n19), Level(&result_n19));
  Init_ciph_same_scale_plain(&result_n19, &result_n19, &_pgen_tmp_158);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n19._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n19._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n19._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_158._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n19._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n19._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_158._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_5751, &result_n19);
  Rescale(&_rescale_tmp_5751._c0_poly, &result_n19._c0_poly);
  Rescale(&_rescale_tmp_5751._c1_poly, &result_n19._c1_poly);
  Pt_from_msg(&_pgen_tmp_159, 2599 /* cst_222 */, 32768, Sc_degree(&_rescale_tmp_5751), Level(&_rescale_tmp_5751));
  Init_ciph_up_scale_plain(&result_n19, &_rescale_tmp_5751, &_pgen_tmp_159);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n19._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n19._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5751._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_159._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n19._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5751._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_159._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_159._poly);
    Free_poly_data(&_rescale_tmp_5751._c1_poly);
    Free_poly_data(&_rescale_tmp_5751._c0_poly);
  }
  Copy_ciph(&orig_result19, &result_n19);
  Free_poly_data(&result_n19._c1_poly);
  Free_poly_data(&result_n19._c0_poly);
  memset(&comb_row_result20, 0, sizeof(comb_row_result20));
  for (combine_row_index_n20 = 0; combine_row_index_n20 < 16; combine_row_index_n20 = combine_row_index_n20 + 1) {
    _preg_268435713 = Rotate(orig_result19, combine_row_index_n20 * 1);
    Copy_ciph(&_ckks_gen_tmp_2094, &_preg_268435713);
    Init_ciph_down_scale(&_rescale_tmp_5757, &_ckks_gen_tmp_2094);
    Rescale(&_rescale_tmp_5757._c0_poly, &_ckks_gen_tmp_2094._c0_poly);
    Rescale(&_rescale_tmp_5757._c1_poly, &_ckks_gen_tmp_2094._c1_poly);
    Pt_from_msg(&_pgen_tmp_160, combine_row_index_n20 + 2600 /* cst_223_0 */, 32768, Sc_degree(&_rescale_tmp_5757), Level(&_rescale_tmp_5757));
    Init_ciph_up_scale_plain(&_pgen_tmp_161, &_rescale_tmp_5757, &_pgen_tmp_160);
    Init_ciph_same_scale(&comb_row_result20, &comb_row_result20, &_pgen_tmp_161);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&comb_row_result20._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_161._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5757._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_160._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_161._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5757._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_160._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_row_result20._c0_poly, _pgen_rns_idx, degree), Coeffs(&comb_row_result20._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_161._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_row_result20._c1_poly, _pgen_rns_idx, degree), Coeffs(&comb_row_result20._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_161._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435713._c1_poly);
    Free_poly_data(&_preg_268435713._c0_poly);
  }
  Free_poly_data(&orig_result19._c1_poly);
  Free_poly_data(&orig_result19._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_2094._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_2094._c0_poly);
  Free_poly_data(&_pgen_tmp_160._poly);
  Free_poly_data(&_rescale_tmp_5757._c1_poly);
  Free_poly_data(&_rescale_tmp_5757._c0_poly);
  Free_poly_data(&_pgen_tmp_161._c1_poly);
  Free_poly_data(&_pgen_tmp_161._c0_poly);
  memset(&comb_rc_result20, 0, sizeof(comb_rc_result20));
  for (combine_rc_index_n20 = 0; combine_rc_index_n20 < 16; combine_rc_index_n20 = combine_rc_index_n20 + 1) {
    _preg_268435714 = Rotate(comb_row_result20, combine_rc_index_n20 * 48);
    Copy_ciph(&_ckks_gen_tmp_2125, &_preg_268435714);
    Init_ciph_down_scale(&_rescale_tmp_5763, &_ckks_gen_tmp_2125);
    Rescale(&_rescale_tmp_5763._c0_poly, &_ckks_gen_tmp_2125._c0_poly);
    Rescale(&_rescale_tmp_5763._c1_poly, &_ckks_gen_tmp_2125._c1_poly);
    Pt_from_msg(&_pgen_tmp_162, combine_rc_index_n20 + 2616 /* cst_224_0 */, 32768, Sc_degree(&_rescale_tmp_5763), Level(&_rescale_tmp_5763));
    Init_ciph_up_scale_plain(&_pgen_tmp_163, &_rescale_tmp_5763, &_pgen_tmp_162);
    Init_ciph_same_scale(&comb_rc_result20, &comb_rc_result20, &_pgen_tmp_163);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&comb_rc_result20._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_163._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5763._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_162._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_163._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5763._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_162._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_rc_result20._c0_poly, _pgen_rns_idx, degree), Coeffs(&comb_rc_result20._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_163._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_rc_result20._c1_poly, _pgen_rns_idx, degree), Coeffs(&comb_rc_result20._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_163._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435714._c1_poly);
    Free_poly_data(&_preg_268435714._c0_poly);
  }
  Free_poly_data(&comb_row_result20._c1_poly);
  Free_poly_data(&comb_row_result20._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_2125._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_2125._c0_poly);
  Free_poly_data(&_pgen_tmp_162._poly);
  Free_poly_data(&_rescale_tmp_5763._c1_poly);
  Free_poly_data(&_rescale_tmp_5763._c0_poly);
  Free_poly_data(&_pgen_tmp_163._c1_poly);
  Free_poly_data(&_pgen_tmp_163._c0_poly);
  memset(&final_result20, 0, sizeof(final_result20));
  for (combine_cc_index_n20 = 0; combine_cc_index_n20 < 32; combine_cc_index_n20 = combine_cc_index_n20 + 1) {
    _preg_268435715 = Rotate(comb_rc_result20, combine_cc_index_n20 * 768);
    Copy_ciph(&_ckks_gen_tmp_2156, &_preg_268435715);
    Init_ciph_down_scale(&_rescale_tmp_5769, &_ckks_gen_tmp_2156);
    Rescale(&_rescale_tmp_5769._c0_poly, &_ckks_gen_tmp_2156._c0_poly);
    Rescale(&_rescale_tmp_5769._c1_poly, &_ckks_gen_tmp_2156._c1_poly);
    Pt_from_msg(&_pgen_tmp_164, combine_cc_index_n20 + 2632 /* cst_225_0 */, 32768, Sc_degree(&_rescale_tmp_5769), Level(&_rescale_tmp_5769));
    Init_ciph_up_scale_plain(&_pgen_tmp_165, &_rescale_tmp_5769, &_pgen_tmp_164);
    Init_ciph_same_scale(&final_result20, &final_result20, &_pgen_tmp_165);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&final_result20._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_165._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5769._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_164._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_165._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5769._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_164._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&final_result20._c0_poly, _pgen_rns_idx, degree), Coeffs(&final_result20._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_165._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&final_result20._c1_poly, _pgen_rns_idx, degree), Coeffs(&final_result20._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_165._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435715._c1_poly);
    Free_poly_data(&_preg_268435715._c0_poly);
  }
  Free_poly_data(&comb_rc_result20._c1_poly);
  Free_poly_data(&comb_rc_result20._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_2156._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_2156._c0_poly);
  Free_poly_data(&_pgen_tmp_164._poly);
  Free_poly_data(&_rescale_tmp_5769._c1_poly);
  Free_poly_data(&_rescale_tmp_5769._c0_poly);
  Free_poly_data(&_pgen_tmp_165._c1_poly);
  Free_poly_data(&_pgen_tmp_165._c0_poly);
  Copy_ciph(&_preg_268435528, &final_result20);
  Free_poly_data(&final_result20._c1_poly);
  Free_poly_data(&final_result20._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::add");
  Init_ciph_same_scale(&_preg_268435529, &_preg_268435527, &_preg_268435528);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_preg_268435529._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_preg_268435529._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435527._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435528._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_preg_268435529._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435527._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435528._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435528._c1_poly);
    Free_poly_data(&_preg_268435528._c0_poly);
    Free_poly_data(&_preg_268435527._c1_poly);
    Free_poly_data(&_preg_268435527._c0_poly);
  }
  Tm_taken("Tensor::add");
  Init_ciph_down_scale(&_pgen_tmp_166, &_preg_268435529);
  Rescale(&_pgen_tmp_166._c0_poly, &_preg_268435529._c0_poly);
  Rescale(&_pgen_tmp_166._c1_poly, &_preg_268435529._c1_poly);
  Free_poly_data(&_preg_268435529._c1_poly);
  Free_poly_data(&_preg_268435529._c0_poly);
  Bootstrap(&_preg_268435530, &_pgen_tmp_166, 15);
  Free_poly_data(&_pgen_tmp_166._c1_poly);
  Free_poly_data(&_pgen_tmp_166._c0_poly);
  Encode_float(&_pgen_tmp_168, &_cst_461, 1, 1, Level(&_preg_268435530));
  Init_ciph_up_scale_plain(&_pgen_tmp_167, &_preg_268435530, &_pgen_tmp_168);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_167._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_167._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435530._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_168._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_167._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435530._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_168._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_168._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_169, &_pgen_tmp_167);
  Rescale(&_pgen_tmp_169._c0_poly, &_pgen_tmp_167._c0_poly);
  Rescale(&_pgen_tmp_169._c1_poly, &_pgen_tmp_167._c1_poly);
  Free_poly_data(&_pgen_tmp_167._c1_poly);
  Free_poly_data(&_pgen_tmp_167._c0_poly);
  _preg_268435531 = App_relu(_preg_268435530, _pgen_tmp_169);
  Free_poly_data(&_pgen_tmp_169._c1_poly);
  Free_poly_data(&_pgen_tmp_169._c0_poly);
  Free_poly_data(&_preg_268435530._c1_poly);
  Free_poly_data(&_preg_268435530._c0_poly);
  Copy_ciph(&_preg_268435532, &_preg_268435531);
  Free_poly_data(&_preg_268435531._c1_poly);
  Free_poly_data(&_preg_268435531._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n21, 0, sizeof(result_n21));
  Copy_ciph(&input_dup_n21, &_preg_268435532);
  for (index_khw1_n21 = 0; index_khw1_n21 < 9; index_khw1_n21 = index_khw1_n21 + 1) {
    _preg_268435716 = Rotate(input_dup_n21, _cst_231[index_khw1_n21]);
    Copy_ciph(&input_roll_n21[index_khw1_n21], &_preg_268435716);
    Free_poly_data(&_preg_268435716._c1_poly);
    Free_poly_data(&_preg_268435716._c0_poly);
  }
  Free_poly_data(&input_dup_n21._c1_poly);
  Free_poly_data(&input_dup_n21._c0_poly);
  for (index_cin_n21 = 0; index_cin_n21 < 32; index_cin_n21 = index_cin_n21 + 1) {
    memset(&result_cin_n21, 0, sizeof(result_cin_n21));
    for (index_khw2_n21 = 0; index_khw2_n21 < 9; index_khw2_n21 = index_khw2_n21 + 1) {
      _rescale_tmp_2237 = input_roll_n21[index_khw2_n21];
      Pt_from_msg(&_pgen_tmp_170, index_khw2_n21 + (index_cin_n21 * 9) + 2664 /* cst_229_0 */, 8192, Sc_degree(&_rescale_tmp_2237), Level(&_rescale_tmp_2237));
      Init_ciph_up_scale_plain(&_pgen_tmp_171, &_rescale_tmp_2237, &_pgen_tmp_170);
      Init_ciph_same_scale(&result_cin_n21, &result_cin_n21, &_pgen_tmp_171);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n21._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_171._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2237._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_170._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_171._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2237._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_170._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n21._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n21._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_171._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n21._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n21._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_171._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435717 = Rotate(result_cin_n21, -8192);
    Init_ciph_same_scale(&_pgen_tmp_172, &result_cin_n21, &_preg_268435717);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_172._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_172._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n21._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435717._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_172._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n21._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435717._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435718 = Rotate(_pgen_tmp_172, index_cin_n21 * 256);
    Init_ciph_same_scale(&result_n21, &result_n21, &_preg_268435718);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n21._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n21._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n21._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435718._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n21._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n21._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435718._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435717._c1_poly);
    Free_poly_data(&_preg_268435717._c0_poly);
    Free_poly_data(&_preg_268435718._c1_poly);
    Free_poly_data(&_preg_268435718._c0_poly);
  }
  Free_ciph_poly(input_roll_n21, 9);
  Free_poly_data(&_pgen_tmp_170._poly);
  Free_poly_data(&_pgen_tmp_171._c1_poly);
  Free_poly_data(&_pgen_tmp_171._c0_poly);
  Free_poly_data(&result_cin_n21._c1_poly);
  Free_poly_data(&result_cin_n21._c0_poly);
  Free_poly_data(&_pgen_tmp_172._c1_poly);
  Free_poly_data(&_pgen_tmp_172._c0_poly);
  Pt_from_msg(&_pgen_tmp_173, 2952 /* cst_230 */, 8192, Sc_degree(&result_n21), Level(&result_n21));
  Init_ciph_same_scale_plain(&result_n21, &result_n21, &_pgen_tmp_173);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n21._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n21._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n21._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_173._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n21._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n21._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_173._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_5782, &result_n21);
  Rescale(&_rescale_tmp_5782._c0_poly, &result_n21._c0_poly);
  Rescale(&_rescale_tmp_5782._c1_poly, &result_n21._c1_poly);
  Pt_from_msg(&_pgen_tmp_174, 2953 /* cst_232 */, 8192, Sc_degree(&_rescale_tmp_5782), Level(&_rescale_tmp_5782));
  Init_ciph_up_scale_plain(&result_n21, &_rescale_tmp_5782, &_pgen_tmp_174);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n21._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n21._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5782._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_174._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n21._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5782._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_174._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_174._poly);
    Free_poly_data(&_rescale_tmp_5782._c1_poly);
    Free_poly_data(&_rescale_tmp_5782._c0_poly);
  }
  Copy_ciph(&_preg_268435533, &result_n21);
  Free_poly_data(&result_n21._c1_poly);
  Free_poly_data(&result_n21._c0_poly);
  Tm_taken("Tensor::conv");
  Init_ciph_down_scale(&_pgen_tmp_175, &_preg_268435533);
  Rescale(&_pgen_tmp_175._c0_poly, &_preg_268435533._c0_poly);
  Rescale(&_pgen_tmp_175._c1_poly, &_preg_268435533._c1_poly);
  Free_poly_data(&_preg_268435533._c1_poly);
  Free_poly_data(&_preg_268435533._c0_poly);
  Bootstrap(&_preg_268435534, &_pgen_tmp_175, 15);
  Free_poly_data(&_pgen_tmp_175._c1_poly);
  Free_poly_data(&_pgen_tmp_175._c0_poly);
  Encode_float(&_pgen_tmp_177, &_cst_462, 1, 1, Level(&_preg_268435534));
  Init_ciph_up_scale_plain(&_pgen_tmp_176, &_preg_268435534, &_pgen_tmp_177);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_176._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_176._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435534._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_177._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_176._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435534._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_177._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_177._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_178, &_pgen_tmp_176);
  Rescale(&_pgen_tmp_178._c0_poly, &_pgen_tmp_176._c0_poly);
  Rescale(&_pgen_tmp_178._c1_poly, &_pgen_tmp_176._c1_poly);
  Free_poly_data(&_pgen_tmp_176._c1_poly);
  Free_poly_data(&_pgen_tmp_176._c0_poly);
  _preg_268435535 = App_relu(_preg_268435534, _pgen_tmp_178);
  Free_poly_data(&_pgen_tmp_178._c1_poly);
  Free_poly_data(&_pgen_tmp_178._c0_poly);
  Free_poly_data(&_preg_268435534._c1_poly);
  Free_poly_data(&_preg_268435534._c0_poly);
  Copy_ciph(&_preg_268435536, &_preg_268435535);
  Free_poly_data(&_preg_268435535._c1_poly);
  Free_poly_data(&_preg_268435535._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n22, 0, sizeof(result_n22));
  Copy_ciph(&input_dup_n22, &_preg_268435536);
  Free_poly_data(&_preg_268435536._c1_poly);
  Free_poly_data(&_preg_268435536._c0_poly);
  for (index_khw1_n22 = 0; index_khw1_n22 < 9; index_khw1_n22 = index_khw1_n22 + 1) {
    _preg_268435719 = Rotate(input_dup_n22, _cst_237[index_khw1_n22]);
    Copy_ciph(&input_roll_n22[index_khw1_n22], &_preg_268435719);
    Free_poly_data(&_preg_268435719._c1_poly);
    Free_poly_data(&_preg_268435719._c0_poly);
  }
  Free_poly_data(&input_dup_n22._c1_poly);
  Free_poly_data(&input_dup_n22._c0_poly);
  for (index_cin_n22 = 0; index_cin_n22 < 32; index_cin_n22 = index_cin_n22 + 1) {
    memset(&result_cin_n22, 0, sizeof(result_cin_n22));
    for (index_khw2_n22 = 0; index_khw2_n22 < 9; index_khw2_n22 = index_khw2_n22 + 1) {
      _rescale_tmp_2345 = input_roll_n22[index_khw2_n22];
      Pt_from_msg(&_pgen_tmp_179, index_khw2_n22 + (index_cin_n22 * 9) + 2954 /* cst_235_0 */, 8192, Sc_degree(&_rescale_tmp_2345), Level(&_rescale_tmp_2345));
      Init_ciph_up_scale_plain(&_pgen_tmp_180, &_rescale_tmp_2345, &_pgen_tmp_179);
      Init_ciph_same_scale(&result_cin_n22, &result_cin_n22, &_pgen_tmp_180);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n22._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_180._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2345._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_179._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_180._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2345._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_179._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n22._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n22._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_180._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n22._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n22._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_180._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435720 = Rotate(result_cin_n22, -8192);
    Init_ciph_same_scale(&_pgen_tmp_181, &result_cin_n22, &_preg_268435720);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_181._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_181._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n22._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435720._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_181._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n22._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435720._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435721 = Rotate(_pgen_tmp_181, index_cin_n22 * 256);
    Init_ciph_same_scale(&result_n22, &result_n22, &_preg_268435721);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n22._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n22._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n22._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435721._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n22._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n22._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435721._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435720._c1_poly);
    Free_poly_data(&_preg_268435720._c0_poly);
    Free_poly_data(&_preg_268435721._c1_poly);
    Free_poly_data(&_preg_268435721._c0_poly);
  }
  Free_ciph_poly(input_roll_n22, 9);
  Free_poly_data(&_pgen_tmp_179._poly);
  Free_poly_data(&_pgen_tmp_180._c1_poly);
  Free_poly_data(&_pgen_tmp_180._c0_poly);
  Free_poly_data(&result_cin_n22._c1_poly);
  Free_poly_data(&result_cin_n22._c0_poly);
  Free_poly_data(&_pgen_tmp_181._c1_poly);
  Free_poly_data(&_pgen_tmp_181._c0_poly);
  Pt_from_msg(&_pgen_tmp_182, 3242 /* cst_236 */, 8192, Sc_degree(&result_n22), Level(&result_n22));
  Init_ciph_same_scale_plain(&result_n22, &result_n22, &_pgen_tmp_182);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n22._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n22._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n22._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_182._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n22._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n22._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_182._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_5795, &result_n22);
  Rescale(&_rescale_tmp_5795._c0_poly, &result_n22._c0_poly);
  Rescale(&_rescale_tmp_5795._c1_poly, &result_n22._c1_poly);
  Pt_from_msg(&_pgen_tmp_183, 3243 /* cst_238 */, 8192, Sc_degree(&_rescale_tmp_5795), Level(&_rescale_tmp_5795));
  Init_ciph_up_scale_plain(&result_n22, &_rescale_tmp_5795, &_pgen_tmp_183);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n22._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n22._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5795._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_183._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n22._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5795._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_183._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_183._poly);
    Free_poly_data(&_rescale_tmp_5795._c1_poly);
    Free_poly_data(&_rescale_tmp_5795._c0_poly);
  }
  Copy_ciph(&_preg_268435537, &result_n22);
  Free_poly_data(&result_n22._c1_poly);
  Free_poly_data(&result_n22._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::add");
  Init_ciph_down_scale(&_pgen_tmp_184, &_preg_268435537);
  Rescale(&_pgen_tmp_184._c0_poly, &_preg_268435537._c0_poly);
  Rescale(&_pgen_tmp_184._c1_poly, &_preg_268435537._c1_poly);
  Free_poly_data(&_preg_268435537._c1_poly);
  Free_poly_data(&_preg_268435537._c0_poly);
  Init_ciph_same_scale(&_preg_268435538, &_pgen_tmp_184, &_preg_268435532);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_preg_268435538._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_preg_268435538._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_184._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435532._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_preg_268435538._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_184._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435532._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435532._c1_poly);
    Free_poly_data(&_preg_268435532._c0_poly);
    Free_poly_data(&_pgen_tmp_184._c1_poly);
    Free_poly_data(&_pgen_tmp_184._c0_poly);
  }
  Tm_taken("Tensor::add");
  Bootstrap(&_preg_268435539, &_preg_268435538, 15);
  Free_poly_data(&_preg_268435538._c1_poly);
  Free_poly_data(&_preg_268435538._c0_poly);
  Encode_float(&_pgen_tmp_186, &_cst_463, 1, 1, Level(&_preg_268435539));
  Init_ciph_up_scale_plain(&_pgen_tmp_185, &_preg_268435539, &_pgen_tmp_186);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_185._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_185._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435539._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_186._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_185._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435539._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_186._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_186._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_187, &_pgen_tmp_185);
  Rescale(&_pgen_tmp_187._c0_poly, &_pgen_tmp_185._c0_poly);
  Rescale(&_pgen_tmp_187._c1_poly, &_pgen_tmp_185._c1_poly);
  Free_poly_data(&_pgen_tmp_185._c1_poly);
  Free_poly_data(&_pgen_tmp_185._c0_poly);
  _preg_268435540 = App_relu(_preg_268435539, _pgen_tmp_187);
  Free_poly_data(&_pgen_tmp_187._c1_poly);
  Free_poly_data(&_pgen_tmp_187._c0_poly);
  Free_poly_data(&_preg_268435539._c1_poly);
  Free_poly_data(&_preg_268435539._c0_poly);
  Copy_ciph(&_preg_268435541, &_preg_268435540);
  Free_poly_data(&_preg_268435540._c1_poly);
  Free_poly_data(&_preg_268435540._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n23, 0, sizeof(result_n23));
  Copy_ciph(&input_dup_n23, &_preg_268435541);
  for (index_khw1_n23 = 0; index_khw1_n23 < 9; index_khw1_n23 = index_khw1_n23 + 1) {
    _preg_268435722 = Rotate(input_dup_n23, _cst_244[index_khw1_n23]);
    Copy_ciph(&input_roll_n23[index_khw1_n23], &_preg_268435722);
    Free_poly_data(&_preg_268435722._c1_poly);
    Free_poly_data(&_preg_268435722._c0_poly);
  }
  Free_poly_data(&input_dup_n23._c1_poly);
  Free_poly_data(&input_dup_n23._c0_poly);
  for (index_cin_n23 = 0; index_cin_n23 < 32; index_cin_n23 = index_cin_n23 + 1) {
    memset(&result_cin_n23, 0, sizeof(result_cin_n23));
    for (index_khw2_n23 = 0; index_khw2_n23 < 9; index_khw2_n23 = index_khw2_n23 + 1) {
      _rescale_tmp_2459 = input_roll_n23[index_khw2_n23];
      Pt_from_msg(&_pgen_tmp_188, index_khw2_n23 + (index_cin_n23 * 9) + 3244 /* cst_242_0 */, 8192, Sc_degree(&_rescale_tmp_2459), Level(&_rescale_tmp_2459));
      Init_ciph_up_scale_plain(&_pgen_tmp_189, &_rescale_tmp_2459, &_pgen_tmp_188);
      Init_ciph_same_scale(&result_cin_n23, &result_cin_n23, &_pgen_tmp_189);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n23._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_189._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2459._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_188._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_189._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2459._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_188._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n23._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n23._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_189._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n23._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n23._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_189._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435723 = Rotate(result_cin_n23, -8192);
    Init_ciph_same_scale(&_pgen_tmp_190, &result_cin_n23, &_preg_268435723);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_190._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_190._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n23._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435723._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_190._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n23._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435723._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435724 = Rotate(_pgen_tmp_190, index_cin_n23 * 256);
    Init_ciph_same_scale(&result_n23, &result_n23, &_preg_268435724);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n23._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n23._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n23._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435724._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n23._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n23._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435724._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435723._c1_poly);
    Free_poly_data(&_preg_268435723._c0_poly);
    Free_poly_data(&_preg_268435724._c1_poly);
    Free_poly_data(&_preg_268435724._c0_poly);
  }
  Free_ciph_poly(input_roll_n23, 9);
  Free_poly_data(&_pgen_tmp_188._poly);
  Free_poly_data(&_pgen_tmp_189._c1_poly);
  Free_poly_data(&_pgen_tmp_189._c0_poly);
  Free_poly_data(&result_cin_n23._c1_poly);
  Free_poly_data(&result_cin_n23._c0_poly);
  Free_poly_data(&_pgen_tmp_190._c1_poly);
  Free_poly_data(&_pgen_tmp_190._c0_poly);
  Pt_from_msg(&_pgen_tmp_191, 3532 /* cst_243 */, 8192, Sc_degree(&result_n23), Level(&result_n23));
  Init_ciph_same_scale_plain(&result_n23, &result_n23, &_pgen_tmp_191);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n23._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n23._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n23._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_191._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n23._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n23._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_191._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_5808, &result_n23);
  Rescale(&_rescale_tmp_5808._c0_poly, &result_n23._c0_poly);
  Rescale(&_rescale_tmp_5808._c1_poly, &result_n23._c1_poly);
  Pt_from_msg(&_pgen_tmp_192, 3533 /* cst_245 */, 8192, Sc_degree(&_rescale_tmp_5808), Level(&_rescale_tmp_5808));
  Init_ciph_up_scale_plain(&result_n23, &_rescale_tmp_5808, &_pgen_tmp_192);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n23._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n23._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5808._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_192._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n23._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5808._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_192._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_192._poly);
    Free_poly_data(&_rescale_tmp_5808._c1_poly);
    Free_poly_data(&_rescale_tmp_5808._c0_poly);
  }
  Copy_ciph(&_preg_268435542, &result_n23);
  Free_poly_data(&result_n23._c1_poly);
  Free_poly_data(&result_n23._c0_poly);
  Tm_taken("Tensor::conv");
  Init_ciph_down_scale(&_pgen_tmp_193, &_preg_268435542);
  Rescale(&_pgen_tmp_193._c0_poly, &_preg_268435542._c0_poly);
  Rescale(&_pgen_tmp_193._c1_poly, &_preg_268435542._c1_poly);
  Free_poly_data(&_preg_268435542._c1_poly);
  Free_poly_data(&_preg_268435542._c0_poly);
  Bootstrap(&_preg_268435543, &_pgen_tmp_193, 15);
  Free_poly_data(&_pgen_tmp_193._c1_poly);
  Free_poly_data(&_pgen_tmp_193._c0_poly);
  Encode_float(&_pgen_tmp_195, &_cst_464, 1, 1, Level(&_preg_268435543));
  Init_ciph_up_scale_plain(&_pgen_tmp_194, &_preg_268435543, &_pgen_tmp_195);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_194._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_194._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435543._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_195._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_194._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435543._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_195._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_195._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_196, &_pgen_tmp_194);
  Rescale(&_pgen_tmp_196._c0_poly, &_pgen_tmp_194._c0_poly);
  Rescale(&_pgen_tmp_196._c1_poly, &_pgen_tmp_194._c1_poly);
  Free_poly_data(&_pgen_tmp_194._c1_poly);
  Free_poly_data(&_pgen_tmp_194._c0_poly);
  _preg_268435544 = App_relu(_preg_268435543, _pgen_tmp_196);
  Free_poly_data(&_pgen_tmp_196._c1_poly);
  Free_poly_data(&_pgen_tmp_196._c0_poly);
  Free_poly_data(&_preg_268435543._c1_poly);
  Free_poly_data(&_preg_268435543._c0_poly);
  Copy_ciph(&_preg_268435545, &_preg_268435544);
  Free_poly_data(&_preg_268435544._c1_poly);
  Free_poly_data(&_preg_268435544._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n24, 0, sizeof(result_n24));
  Copy_ciph(&input_dup_n24, &_preg_268435545);
  Free_poly_data(&_preg_268435545._c1_poly);
  Free_poly_data(&_preg_268435545._c0_poly);
  for (index_khw1_n24 = 0; index_khw1_n24 < 9; index_khw1_n24 = index_khw1_n24 + 1) {
    _preg_268435725 = Rotate(input_dup_n24, _cst_250[index_khw1_n24]);
    Copy_ciph(&input_roll_n24[index_khw1_n24], &_preg_268435725);
    Free_poly_data(&_preg_268435725._c1_poly);
    Free_poly_data(&_preg_268435725._c0_poly);
  }
  Free_poly_data(&input_dup_n24._c1_poly);
  Free_poly_data(&input_dup_n24._c0_poly);
  for (index_cin_n24 = 0; index_cin_n24 < 32; index_cin_n24 = index_cin_n24 + 1) {
    memset(&result_cin_n24, 0, sizeof(result_cin_n24));
    for (index_khw2_n24 = 0; index_khw2_n24 < 9; index_khw2_n24 = index_khw2_n24 + 1) {
      _rescale_tmp_2567 = input_roll_n24[index_khw2_n24];
      Pt_from_msg(&_pgen_tmp_197, index_khw2_n24 + (index_cin_n24 * 9) + 3534 /* cst_248_0 */, 8192, Sc_degree(&_rescale_tmp_2567), Level(&_rescale_tmp_2567));
      Init_ciph_up_scale_plain(&_pgen_tmp_198, &_rescale_tmp_2567, &_pgen_tmp_197);
      Init_ciph_same_scale(&result_cin_n24, &result_cin_n24, &_pgen_tmp_198);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n24._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_198._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2567._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_197._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_198._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2567._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_197._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n24._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n24._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_198._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n24._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n24._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_198._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435726 = Rotate(result_cin_n24, -8192);
    Init_ciph_same_scale(&_pgen_tmp_199, &result_cin_n24, &_preg_268435726);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_199._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_199._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n24._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435726._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_199._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n24._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435726._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435727 = Rotate(_pgen_tmp_199, index_cin_n24 * 256);
    Init_ciph_same_scale(&result_n24, &result_n24, &_preg_268435727);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n24._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n24._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n24._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435727._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n24._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n24._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435727._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435726._c1_poly);
    Free_poly_data(&_preg_268435726._c0_poly);
    Free_poly_data(&_preg_268435727._c1_poly);
    Free_poly_data(&_preg_268435727._c0_poly);
  }
  Free_ciph_poly(input_roll_n24, 9);
  Free_poly_data(&_pgen_tmp_197._poly);
  Free_poly_data(&_pgen_tmp_198._c1_poly);
  Free_poly_data(&_pgen_tmp_198._c0_poly);
  Free_poly_data(&result_cin_n24._c1_poly);
  Free_poly_data(&result_cin_n24._c0_poly);
  Free_poly_data(&_pgen_tmp_199._c1_poly);
  Free_poly_data(&_pgen_tmp_199._c0_poly);
  Pt_from_msg(&_pgen_tmp_200, 3822 /* cst_249 */, 8192, Sc_degree(&result_n24), Level(&result_n24));
  Init_ciph_same_scale_plain(&result_n24, &result_n24, &_pgen_tmp_200);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n24._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n24._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n24._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_200._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n24._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n24._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_200._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_5821, &result_n24);
  Rescale(&_rescale_tmp_5821._c0_poly, &result_n24._c0_poly);
  Rescale(&_rescale_tmp_5821._c1_poly, &result_n24._c1_poly);
  Pt_from_msg(&_pgen_tmp_201, 3823 /* cst_251 */, 8192, Sc_degree(&_rescale_tmp_5821), Level(&_rescale_tmp_5821));
  Init_ciph_up_scale_plain(&result_n24, &_rescale_tmp_5821, &_pgen_tmp_201);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n24._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n24._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5821._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_201._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n24._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5821._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_201._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_201._poly);
    Free_poly_data(&_rescale_tmp_5821._c1_poly);
    Free_poly_data(&_rescale_tmp_5821._c0_poly);
  }
  Copy_ciph(&_preg_268435546, &result_n24);
  Free_poly_data(&result_n24._c1_poly);
  Free_poly_data(&result_n24._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::add");
  Init_ciph_down_scale(&_pgen_tmp_202, &_preg_268435546);
  Rescale(&_pgen_tmp_202._c0_poly, &_preg_268435546._c0_poly);
  Rescale(&_pgen_tmp_202._c1_poly, &_preg_268435546._c1_poly);
  Free_poly_data(&_preg_268435546._c1_poly);
  Free_poly_data(&_preg_268435546._c0_poly);
  Init_ciph_same_scale(&_preg_268435547, &_pgen_tmp_202, &_preg_268435541);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_preg_268435547._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_preg_268435547._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_202._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435541._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_preg_268435547._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_202._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435541._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435541._c1_poly);
    Free_poly_data(&_preg_268435541._c0_poly);
    Free_poly_data(&_pgen_tmp_202._c1_poly);
    Free_poly_data(&_pgen_tmp_202._c0_poly);
  }
  Tm_taken("Tensor::add");
  Bootstrap(&_preg_268435548, &_preg_268435547, 15);
  Free_poly_data(&_preg_268435547._c1_poly);
  Free_poly_data(&_preg_268435547._c0_poly);
  Encode_float(&_pgen_tmp_204, &_cst_465, 1, 1, Level(&_preg_268435548));
  Init_ciph_up_scale_plain(&_pgen_tmp_203, &_preg_268435548, &_pgen_tmp_204);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_203._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_203._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435548._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_204._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_203._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435548._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_204._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_204._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_205, &_pgen_tmp_203);
  Rescale(&_pgen_tmp_205._c0_poly, &_pgen_tmp_203._c0_poly);
  Rescale(&_pgen_tmp_205._c1_poly, &_pgen_tmp_203._c1_poly);
  Free_poly_data(&_pgen_tmp_203._c1_poly);
  Free_poly_data(&_pgen_tmp_203._c0_poly);
  _preg_268435549 = App_relu(_preg_268435548, _pgen_tmp_205);
  Free_poly_data(&_pgen_tmp_205._c1_poly);
  Free_poly_data(&_pgen_tmp_205._c0_poly);
  Free_poly_data(&_preg_268435548._c1_poly);
  Free_poly_data(&_preg_268435548._c0_poly);
  Copy_ciph(&_preg_268435550, &_preg_268435549);
  Free_poly_data(&_preg_268435549._c1_poly);
  Free_poly_data(&_preg_268435549._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n25, 0, sizeof(result_n25));
  Copy_ciph(&input_dup_n25, &_preg_268435550);
  for (index_khw1_n25 = 0; index_khw1_n25 < 9; index_khw1_n25 = index_khw1_n25 + 1) {
    _preg_268435728 = Rotate(input_dup_n25, _cst_257[index_khw1_n25]);
    Copy_ciph(&input_roll_n25[index_khw1_n25], &_preg_268435728);
    Free_poly_data(&_preg_268435728._c1_poly);
    Free_poly_data(&_preg_268435728._c0_poly);
  }
  Free_poly_data(&input_dup_n25._c1_poly);
  Free_poly_data(&input_dup_n25._c0_poly);
  for (index_cin_n25 = 0; index_cin_n25 < 32; index_cin_n25 = index_cin_n25 + 1) {
    memset(&result_cin_n25, 0, sizeof(result_cin_n25));
    for (index_khw2_n25 = 0; index_khw2_n25 < 9; index_khw2_n25 = index_khw2_n25 + 1) {
      _rescale_tmp_2681 = input_roll_n25[index_khw2_n25];
      Pt_from_msg(&_pgen_tmp_206, index_khw2_n25 + (index_cin_n25 * 9) + 3824 /* cst_255_0 */, 8192, Sc_degree(&_rescale_tmp_2681), Level(&_rescale_tmp_2681));
      Init_ciph_up_scale_plain(&_pgen_tmp_207, &_rescale_tmp_2681, &_pgen_tmp_206);
      Init_ciph_same_scale(&result_cin_n25, &result_cin_n25, &_pgen_tmp_207);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n25._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_207._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2681._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_206._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_207._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2681._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_206._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n25._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n25._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_207._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n25._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n25._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_207._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435729 = Rotate(result_cin_n25, -8192);
    Init_ciph_same_scale(&_pgen_tmp_208, &result_cin_n25, &_preg_268435729);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_208._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_208._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n25._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435729._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_208._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n25._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435729._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435730 = Rotate(_pgen_tmp_208, index_cin_n25 * 256);
    Init_ciph_same_scale(&result_n25, &result_n25, &_preg_268435730);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n25._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n25._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n25._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435730._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n25._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n25._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435730._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435729._c1_poly);
    Free_poly_data(&_preg_268435729._c0_poly);
    Free_poly_data(&_preg_268435730._c1_poly);
    Free_poly_data(&_preg_268435730._c0_poly);
  }
  Free_ciph_poly(input_roll_n25, 9);
  Free_poly_data(&_pgen_tmp_206._poly);
  Free_poly_data(&_pgen_tmp_207._c1_poly);
  Free_poly_data(&_pgen_tmp_207._c0_poly);
  Free_poly_data(&result_cin_n25._c1_poly);
  Free_poly_data(&result_cin_n25._c0_poly);
  Free_poly_data(&_pgen_tmp_208._c1_poly);
  Free_poly_data(&_pgen_tmp_208._c0_poly);
  Pt_from_msg(&_pgen_tmp_209, 4112 /* cst_256 */, 8192, Sc_degree(&result_n25), Level(&result_n25));
  Init_ciph_same_scale_plain(&result_n25, &result_n25, &_pgen_tmp_209);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n25._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n25._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n25._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_209._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n25._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n25._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_209._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_5834, &result_n25);
  Rescale(&_rescale_tmp_5834._c0_poly, &result_n25._c0_poly);
  Rescale(&_rescale_tmp_5834._c1_poly, &result_n25._c1_poly);
  Pt_from_msg(&_pgen_tmp_210, 4113 /* cst_258 */, 8192, Sc_degree(&_rescale_tmp_5834), Level(&_rescale_tmp_5834));
  Init_ciph_up_scale_plain(&result_n25, &_rescale_tmp_5834, &_pgen_tmp_210);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n25._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n25._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5834._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_210._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n25._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5834._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_210._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_210._poly);
    Free_poly_data(&_rescale_tmp_5834._c1_poly);
    Free_poly_data(&_rescale_tmp_5834._c0_poly);
  }
  Copy_ciph(&_preg_268435551, &result_n25);
  Free_poly_data(&result_n25._c1_poly);
  Free_poly_data(&result_n25._c0_poly);
  Tm_taken("Tensor::conv");
  Init_ciph_down_scale(&_pgen_tmp_211, &_preg_268435551);
  Rescale(&_pgen_tmp_211._c0_poly, &_preg_268435551._c0_poly);
  Rescale(&_pgen_tmp_211._c1_poly, &_preg_268435551._c1_poly);
  Free_poly_data(&_preg_268435551._c1_poly);
  Free_poly_data(&_preg_268435551._c0_poly);
  Bootstrap(&_preg_268435552, &_pgen_tmp_211, 15);
  Free_poly_data(&_pgen_tmp_211._c1_poly);
  Free_poly_data(&_pgen_tmp_211._c0_poly);
  Encode_float(&_pgen_tmp_213, &_cst_466, 1, 1, Level(&_preg_268435552));
  Init_ciph_up_scale_plain(&_pgen_tmp_212, &_preg_268435552, &_pgen_tmp_213);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_212._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_212._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435552._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_213._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_212._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435552._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_213._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_213._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_214, &_pgen_tmp_212);
  Rescale(&_pgen_tmp_214._c0_poly, &_pgen_tmp_212._c0_poly);
  Rescale(&_pgen_tmp_214._c1_poly, &_pgen_tmp_212._c1_poly);
  Free_poly_data(&_pgen_tmp_212._c1_poly);
  Free_poly_data(&_pgen_tmp_212._c0_poly);
  _preg_268435553 = App_relu(_preg_268435552, _pgen_tmp_214);
  Free_poly_data(&_pgen_tmp_214._c1_poly);
  Free_poly_data(&_pgen_tmp_214._c0_poly);
  Free_poly_data(&_preg_268435552._c1_poly);
  Free_poly_data(&_preg_268435552._c0_poly);
  Copy_ciph(&_preg_268435554, &_preg_268435553);
  Free_poly_data(&_preg_268435553._c1_poly);
  Free_poly_data(&_preg_268435553._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n26, 0, sizeof(result_n26));
  Copy_ciph(&input_dup_n26, &_preg_268435554);
  Free_poly_data(&_preg_268435554._c1_poly);
  Free_poly_data(&_preg_268435554._c0_poly);
  for (index_khw1_n26 = 0; index_khw1_n26 < 9; index_khw1_n26 = index_khw1_n26 + 1) {
    _preg_268435731 = Rotate(input_dup_n26, _cst_263[index_khw1_n26]);
    Copy_ciph(&input_roll_n26[index_khw1_n26], &_preg_268435731);
    Free_poly_data(&_preg_268435731._c1_poly);
    Free_poly_data(&_preg_268435731._c0_poly);
  }
  Free_poly_data(&input_dup_n26._c1_poly);
  Free_poly_data(&input_dup_n26._c0_poly);
  for (index_cin_n26 = 0; index_cin_n26 < 32; index_cin_n26 = index_cin_n26 + 1) {
    memset(&result_cin_n26, 0, sizeof(result_cin_n26));
    for (index_khw2_n26 = 0; index_khw2_n26 < 9; index_khw2_n26 = index_khw2_n26 + 1) {
      _rescale_tmp_2789 = input_roll_n26[index_khw2_n26];
      Pt_from_msg(&_pgen_tmp_215, index_khw2_n26 + (index_cin_n26 * 9) + 4114 /* cst_261_0 */, 8192, Sc_degree(&_rescale_tmp_2789), Level(&_rescale_tmp_2789));
      Init_ciph_up_scale_plain(&_pgen_tmp_216, &_rescale_tmp_2789, &_pgen_tmp_215);
      Init_ciph_same_scale(&result_cin_n26, &result_cin_n26, &_pgen_tmp_216);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n26._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_216._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2789._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_215._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_216._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2789._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_215._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n26._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n26._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_216._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n26._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n26._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_216._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435732 = Rotate(result_cin_n26, -8192);
    Init_ciph_same_scale(&_pgen_tmp_217, &result_cin_n26, &_preg_268435732);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_217._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_217._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n26._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435732._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_217._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n26._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435732._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435733 = Rotate(_pgen_tmp_217, index_cin_n26 * 256);
    Init_ciph_same_scale(&result_n26, &result_n26, &_preg_268435733);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n26._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n26._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n26._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435733._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n26._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n26._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435733._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435732._c1_poly);
    Free_poly_data(&_preg_268435732._c0_poly);
    Free_poly_data(&_preg_268435733._c1_poly);
    Free_poly_data(&_preg_268435733._c0_poly);
  }
  Free_ciph_poly(input_roll_n26, 9);
  Free_poly_data(&_pgen_tmp_215._poly);
  Free_poly_data(&_pgen_tmp_216._c1_poly);
  Free_poly_data(&_pgen_tmp_216._c0_poly);
  Free_poly_data(&result_cin_n26._c1_poly);
  Free_poly_data(&result_cin_n26._c0_poly);
  Free_poly_data(&_pgen_tmp_217._c1_poly);
  Free_poly_data(&_pgen_tmp_217._c0_poly);
  Pt_from_msg(&_pgen_tmp_218, 4402 /* cst_262 */, 8192, Sc_degree(&result_n26), Level(&result_n26));
  Init_ciph_same_scale_plain(&result_n26, &result_n26, &_pgen_tmp_218);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n26._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n26._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n26._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_218._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n26._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n26._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_218._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_5847, &result_n26);
  Rescale(&_rescale_tmp_5847._c0_poly, &result_n26._c0_poly);
  Rescale(&_rescale_tmp_5847._c1_poly, &result_n26._c1_poly);
  Pt_from_msg(&_pgen_tmp_219, 4403 /* cst_264 */, 8192, Sc_degree(&_rescale_tmp_5847), Level(&_rescale_tmp_5847));
  Init_ciph_up_scale_plain(&result_n26, &_rescale_tmp_5847, &_pgen_tmp_219);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n26._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n26._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5847._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_219._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n26._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5847._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_219._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_219._poly);
    Free_poly_data(&_rescale_tmp_5847._c1_poly);
    Free_poly_data(&_rescale_tmp_5847._c0_poly);
  }
  Copy_ciph(&_preg_268435555, &result_n26);
  Free_poly_data(&result_n26._c1_poly);
  Free_poly_data(&result_n26._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::add");
  Init_ciph_down_scale(&_pgen_tmp_220, &_preg_268435555);
  Rescale(&_pgen_tmp_220._c0_poly, &_preg_268435555._c0_poly);
  Rescale(&_pgen_tmp_220._c1_poly, &_preg_268435555._c1_poly);
  Free_poly_data(&_preg_268435555._c1_poly);
  Free_poly_data(&_preg_268435555._c0_poly);
  Init_ciph_same_scale(&_preg_268435556, &_pgen_tmp_220, &_preg_268435550);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_preg_268435556._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_preg_268435556._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_220._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435550._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_preg_268435556._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_220._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435550._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435550._c1_poly);
    Free_poly_data(&_preg_268435550._c0_poly);
    Free_poly_data(&_pgen_tmp_220._c1_poly);
    Free_poly_data(&_pgen_tmp_220._c0_poly);
  }
  Tm_taken("Tensor::add");
  Bootstrap(&_preg_268435557, &_preg_268435556, 15);
  Free_poly_data(&_preg_268435556._c1_poly);
  Free_poly_data(&_preg_268435556._c0_poly);
  Encode_float(&_pgen_tmp_222, &_cst_467, 1, 1, Level(&_preg_268435557));
  Init_ciph_up_scale_plain(&_pgen_tmp_221, &_preg_268435557, &_pgen_tmp_222);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_221._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_221._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435557._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_222._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_221._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435557._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_222._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_222._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_223, &_pgen_tmp_221);
  Rescale(&_pgen_tmp_223._c0_poly, &_pgen_tmp_221._c0_poly);
  Rescale(&_pgen_tmp_223._c1_poly, &_pgen_tmp_221._c1_poly);
  Free_poly_data(&_pgen_tmp_221._c1_poly);
  Free_poly_data(&_pgen_tmp_221._c0_poly);
  _preg_268435558 = App_relu(_preg_268435557, _pgen_tmp_223);
  Free_poly_data(&_pgen_tmp_223._c1_poly);
  Free_poly_data(&_pgen_tmp_223._c0_poly);
  Free_poly_data(&_preg_268435557._c1_poly);
  Free_poly_data(&_preg_268435557._c0_poly);
  Copy_ciph(&_preg_268435559, &_preg_268435558);
  Free_poly_data(&_preg_268435558._c1_poly);
  Free_poly_data(&_preg_268435558._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n27, 0, sizeof(result_n27));
  Copy_ciph(&input_dup_n27, &_preg_268435559);
  for (index_khw1_n27 = 0; index_khw1_n27 < 9; index_khw1_n27 = index_khw1_n27 + 1) {
    _preg_268435734 = Rotate(input_dup_n27, _cst_270[index_khw1_n27]);
    Copy_ciph(&input_roll_n27[index_khw1_n27], &_preg_268435734);
    Free_poly_data(&_preg_268435734._c1_poly);
    Free_poly_data(&_preg_268435734._c0_poly);
  }
  Free_poly_data(&input_dup_n27._c1_poly);
  Free_poly_data(&input_dup_n27._c0_poly);
  for (index_cin_n27 = 0; index_cin_n27 < 32; index_cin_n27 = index_cin_n27 + 1) {
    memset(&result_cin_n27, 0, sizeof(result_cin_n27));
    for (index_khw2_n27 = 0; index_khw2_n27 < 9; index_khw2_n27 = index_khw2_n27 + 1) {
      _rescale_tmp_2903 = input_roll_n27[index_khw2_n27];
      Pt_from_msg(&_pgen_tmp_224, index_khw2_n27 + (index_cin_n27 * 9) + 4404 /* cst_268_0 */, 8192, Sc_degree(&_rescale_tmp_2903), Level(&_rescale_tmp_2903));
      Init_ciph_up_scale_plain(&_pgen_tmp_225, &_rescale_tmp_2903, &_pgen_tmp_224);
      Init_ciph_same_scale(&result_cin_n27, &result_cin_n27, &_pgen_tmp_225);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n27._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_225._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2903._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_224._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_225._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_2903._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_224._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n27._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n27._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_225._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n27._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n27._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_225._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435735 = Rotate(result_cin_n27, -8192);
    Init_ciph_same_scale(&_pgen_tmp_226, &result_cin_n27, &_preg_268435735);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_226._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_226._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n27._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435735._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_226._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n27._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435735._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435736 = Rotate(_pgen_tmp_226, index_cin_n27 * 256);
    Init_ciph_same_scale(&result_n27, &result_n27, &_preg_268435736);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n27._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n27._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n27._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435736._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n27._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n27._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435736._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435735._c1_poly);
    Free_poly_data(&_preg_268435735._c0_poly);
    Free_poly_data(&_preg_268435736._c1_poly);
    Free_poly_data(&_preg_268435736._c0_poly);
  }
  Free_ciph_poly(input_roll_n27, 9);
  Free_poly_data(&_pgen_tmp_224._poly);
  Free_poly_data(&_pgen_tmp_225._c1_poly);
  Free_poly_data(&_pgen_tmp_225._c0_poly);
  Free_poly_data(&result_cin_n27._c1_poly);
  Free_poly_data(&result_cin_n27._c0_poly);
  Free_poly_data(&_pgen_tmp_226._c1_poly);
  Free_poly_data(&_pgen_tmp_226._c0_poly);
  Pt_from_msg(&_pgen_tmp_227, 4692 /* cst_269 */, 8192, Sc_degree(&result_n27), Level(&result_n27));
  Init_ciph_same_scale_plain(&result_n27, &result_n27, &_pgen_tmp_227);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n27._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n27._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n27._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_227._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n27._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n27._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_227._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_5860, &result_n27);
  Rescale(&_rescale_tmp_5860._c0_poly, &result_n27._c0_poly);
  Rescale(&_rescale_tmp_5860._c1_poly, &result_n27._c1_poly);
  Pt_from_msg(&_pgen_tmp_228, 4693 /* cst_271 */, 8192, Sc_degree(&_rescale_tmp_5860), Level(&_rescale_tmp_5860));
  Init_ciph_up_scale_plain(&result_n27, &_rescale_tmp_5860, &_pgen_tmp_228);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n27._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n27._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5860._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_228._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n27._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5860._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_228._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_228._poly);
    Free_poly_data(&_rescale_tmp_5860._c1_poly);
    Free_poly_data(&_rescale_tmp_5860._c0_poly);
  }
  Copy_ciph(&_preg_268435560, &result_n27);
  Free_poly_data(&result_n27._c1_poly);
  Free_poly_data(&result_n27._c0_poly);
  Tm_taken("Tensor::conv");
  Init_ciph_down_scale(&_pgen_tmp_229, &_preg_268435560);
  Rescale(&_pgen_tmp_229._c0_poly, &_preg_268435560._c0_poly);
  Rescale(&_pgen_tmp_229._c1_poly, &_preg_268435560._c1_poly);
  Free_poly_data(&_preg_268435560._c1_poly);
  Free_poly_data(&_preg_268435560._c0_poly);
  Bootstrap(&_preg_268435561, &_pgen_tmp_229, 15);
  Free_poly_data(&_pgen_tmp_229._c1_poly);
  Free_poly_data(&_pgen_tmp_229._c0_poly);
  Encode_float(&_pgen_tmp_231, &_cst_468, 1, 1, Level(&_preg_268435561));
  Init_ciph_up_scale_plain(&_pgen_tmp_230, &_preg_268435561, &_pgen_tmp_231);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_230._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_230._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435561._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_231._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_230._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435561._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_231._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_231._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_232, &_pgen_tmp_230);
  Rescale(&_pgen_tmp_232._c0_poly, &_pgen_tmp_230._c0_poly);
  Rescale(&_pgen_tmp_232._c1_poly, &_pgen_tmp_230._c1_poly);
  Free_poly_data(&_pgen_tmp_230._c1_poly);
  Free_poly_data(&_pgen_tmp_230._c0_poly);
  _preg_268435562 = App_relu(_preg_268435561, _pgen_tmp_232);
  Free_poly_data(&_pgen_tmp_232._c1_poly);
  Free_poly_data(&_pgen_tmp_232._c0_poly);
  Free_poly_data(&_preg_268435561._c1_poly);
  Free_poly_data(&_preg_268435561._c0_poly);
  Copy_ciph(&_preg_268435563, &_preg_268435562);
  Free_poly_data(&_preg_268435562._c1_poly);
  Free_poly_data(&_preg_268435562._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n28, 0, sizeof(result_n28));
  Copy_ciph(&input_dup_n28, &_preg_268435563);
  Free_poly_data(&_preg_268435563._c1_poly);
  Free_poly_data(&_preg_268435563._c0_poly);
  for (index_khw1_n28 = 0; index_khw1_n28 < 9; index_khw1_n28 = index_khw1_n28 + 1) {
    _preg_268435737 = Rotate(input_dup_n28, _cst_276[index_khw1_n28]);
    Copy_ciph(&input_roll_n28[index_khw1_n28], &_preg_268435737);
    Free_poly_data(&_preg_268435737._c1_poly);
    Free_poly_data(&_preg_268435737._c0_poly);
  }
  Free_poly_data(&input_dup_n28._c1_poly);
  Free_poly_data(&input_dup_n28._c0_poly);
  for (index_cin_n28 = 0; index_cin_n28 < 32; index_cin_n28 = index_cin_n28 + 1) {
    memset(&result_cin_n28, 0, sizeof(result_cin_n28));
    for (index_khw2_n28 = 0; index_khw2_n28 < 9; index_khw2_n28 = index_khw2_n28 + 1) {
      _rescale_tmp_3011 = input_roll_n28[index_khw2_n28];
      Pt_from_msg(&_pgen_tmp_233, index_khw2_n28 + (index_cin_n28 * 9) + 4694 /* cst_274_0 */, 8192, Sc_degree(&_rescale_tmp_3011), Level(&_rescale_tmp_3011));
      Init_ciph_up_scale_plain(&_pgen_tmp_234, &_rescale_tmp_3011, &_pgen_tmp_233);
      Init_ciph_same_scale(&result_cin_n28, &result_cin_n28, &_pgen_tmp_234);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n28._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_234._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3011._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_233._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_234._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3011._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_233._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n28._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n28._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_234._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n28._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n28._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_234._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435738 = Rotate(result_cin_n28, -8192);
    Init_ciph_same_scale(&_pgen_tmp_235, &result_cin_n28, &_preg_268435738);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_235._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_235._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n28._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435738._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_235._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n28._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435738._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435739 = Rotate(_pgen_tmp_235, index_cin_n28 * 256);
    Init_ciph_same_scale(&result_n28, &result_n28, &_preg_268435739);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n28._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n28._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n28._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435739._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n28._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n28._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435739._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435738._c1_poly);
    Free_poly_data(&_preg_268435738._c0_poly);
    Free_poly_data(&_preg_268435739._c1_poly);
    Free_poly_data(&_preg_268435739._c0_poly);
  }
  Free_ciph_poly(input_roll_n28, 9);
  Free_poly_data(&_pgen_tmp_233._poly);
  Free_poly_data(&_pgen_tmp_234._c1_poly);
  Free_poly_data(&_pgen_tmp_234._c0_poly);
  Free_poly_data(&result_cin_n28._c1_poly);
  Free_poly_data(&result_cin_n28._c0_poly);
  Free_poly_data(&_pgen_tmp_235._c1_poly);
  Free_poly_data(&_pgen_tmp_235._c0_poly);
  Pt_from_msg(&_pgen_tmp_236, 4982 /* cst_275 */, 8192, Sc_degree(&result_n28), Level(&result_n28));
  Init_ciph_same_scale_plain(&result_n28, &result_n28, &_pgen_tmp_236);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n28._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n28._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n28._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_236._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n28._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n28._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_236._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_5873, &result_n28);
  Rescale(&_rescale_tmp_5873._c0_poly, &result_n28._c0_poly);
  Rescale(&_rescale_tmp_5873._c1_poly, &result_n28._c1_poly);
  Pt_from_msg(&_pgen_tmp_237, 4983 /* cst_277 */, 8192, Sc_degree(&_rescale_tmp_5873), Level(&_rescale_tmp_5873));
  Init_ciph_up_scale_plain(&result_n28, &_rescale_tmp_5873, &_pgen_tmp_237);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n28._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n28._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5873._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_237._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n28._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5873._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_237._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_237._poly);
    Free_poly_data(&_rescale_tmp_5873._c1_poly);
    Free_poly_data(&_rescale_tmp_5873._c0_poly);
  }
  Copy_ciph(&_preg_268435564, &result_n28);
  Free_poly_data(&result_n28._c1_poly);
  Free_poly_data(&result_n28._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::add");
  Init_ciph_down_scale(&_pgen_tmp_238, &_preg_268435564);
  Rescale(&_pgen_tmp_238._c0_poly, &_preg_268435564._c0_poly);
  Rescale(&_pgen_tmp_238._c1_poly, &_preg_268435564._c1_poly);
  Free_poly_data(&_preg_268435564._c1_poly);
  Free_poly_data(&_preg_268435564._c0_poly);
  Init_ciph_same_scale(&_preg_268435565, &_pgen_tmp_238, &_preg_268435559);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_preg_268435565._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_preg_268435565._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_238._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435559._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_preg_268435565._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_238._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435559._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435559._c1_poly);
    Free_poly_data(&_preg_268435559._c0_poly);
    Free_poly_data(&_pgen_tmp_238._c1_poly);
    Free_poly_data(&_pgen_tmp_238._c0_poly);
  }
  Tm_taken("Tensor::add");
  Bootstrap(&_preg_268435566, &_preg_268435565, 15);
  Free_poly_data(&_preg_268435565._c1_poly);
  Free_poly_data(&_preg_268435565._c0_poly);
  Encode_float(&_pgen_tmp_240, &_cst_469, 1, 1, Level(&_preg_268435566));
  Init_ciph_up_scale_plain(&_pgen_tmp_239, &_preg_268435566, &_pgen_tmp_240);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_239._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_239._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435566._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_240._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_239._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435566._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_240._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_240._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_241, &_pgen_tmp_239);
  Rescale(&_pgen_tmp_241._c0_poly, &_pgen_tmp_239._c0_poly);
  Rescale(&_pgen_tmp_241._c1_poly, &_pgen_tmp_239._c1_poly);
  Free_poly_data(&_pgen_tmp_239._c1_poly);
  Free_poly_data(&_pgen_tmp_239._c0_poly);
  _preg_268435567 = App_relu(_preg_268435566, _pgen_tmp_241);
  Free_poly_data(&_pgen_tmp_241._c1_poly);
  Free_poly_data(&_pgen_tmp_241._c0_poly);
  Free_poly_data(&_preg_268435566._c1_poly);
  Free_poly_data(&_preg_268435566._c0_poly);
  Copy_ciph(&_preg_268435568, &_preg_268435567);
  Free_poly_data(&_preg_268435567._c1_poly);
  Free_poly_data(&_preg_268435567._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n29, 0, sizeof(result_n29));
  Copy_ciph(&input_dup_n29, &_preg_268435568);
  for (index_khw1_n29 = 0; index_khw1_n29 < 9; index_khw1_n29 = index_khw1_n29 + 1) {
    _preg_268435740 = Rotate(input_dup_n29, _cst_283[index_khw1_n29]);
    Copy_ciph(&input_roll_n29[index_khw1_n29], &_preg_268435740);
    Free_poly_data(&_preg_268435740._c1_poly);
    Free_poly_data(&_preg_268435740._c0_poly);
  }
  Free_poly_data(&input_dup_n29._c1_poly);
  Free_poly_data(&input_dup_n29._c0_poly);
  for (index_cin_n29 = 0; index_cin_n29 < 32; index_cin_n29 = index_cin_n29 + 1) {
    memset(&result_cin_n29, 0, sizeof(result_cin_n29));
    for (index_khw2_n29 = 0; index_khw2_n29 < 9; index_khw2_n29 = index_khw2_n29 + 1) {
      _rescale_tmp_3125 = input_roll_n29[index_khw2_n29];
      Pt_from_msg(&_pgen_tmp_242, index_khw2_n29 + (index_cin_n29 * 9) + 4984 /* cst_281_0 */, 8192, Sc_degree(&_rescale_tmp_3125), Level(&_rescale_tmp_3125));
      Init_ciph_up_scale_plain(&_pgen_tmp_243, &_rescale_tmp_3125, &_pgen_tmp_242);
      Init_ciph_same_scale(&result_cin_n29, &result_cin_n29, &_pgen_tmp_243);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n29._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_243._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3125._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_242._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_243._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3125._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_242._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n29._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n29._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_243._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n29._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n29._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_243._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435741 = Rotate(result_cin_n29, -8192);
    Init_ciph_same_scale(&_pgen_tmp_244, &result_cin_n29, &_preg_268435741);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_244._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_244._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n29._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435741._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_244._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n29._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435741._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435742 = Rotate(_pgen_tmp_244, index_cin_n29 * 256);
    Init_ciph_same_scale(&result_n29, &result_n29, &_preg_268435742);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n29._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n29._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n29._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435742._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n29._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n29._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435742._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435741._c1_poly);
    Free_poly_data(&_preg_268435741._c0_poly);
    Free_poly_data(&_preg_268435742._c1_poly);
    Free_poly_data(&_preg_268435742._c0_poly);
  }
  Free_ciph_poly(input_roll_n29, 9);
  Free_poly_data(&_pgen_tmp_242._poly);
  Free_poly_data(&_pgen_tmp_243._c1_poly);
  Free_poly_data(&_pgen_tmp_243._c0_poly);
  Free_poly_data(&result_cin_n29._c1_poly);
  Free_poly_data(&result_cin_n29._c0_poly);
  Free_poly_data(&_pgen_tmp_244._c1_poly);
  Free_poly_data(&_pgen_tmp_244._c0_poly);
  Pt_from_msg(&_pgen_tmp_245, 5272 /* cst_282 */, 8192, Sc_degree(&result_n29), Level(&result_n29));
  Init_ciph_same_scale_plain(&result_n29, &result_n29, &_pgen_tmp_245);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n29._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n29._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n29._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_245._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n29._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n29._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_245._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_5886, &result_n29);
  Rescale(&_rescale_tmp_5886._c0_poly, &result_n29._c0_poly);
  Rescale(&_rescale_tmp_5886._c1_poly, &result_n29._c1_poly);
  Pt_from_msg(&_pgen_tmp_246, 5273 /* cst_284 */, 8192, Sc_degree(&_rescale_tmp_5886), Level(&_rescale_tmp_5886));
  Init_ciph_up_scale_plain(&result_n29, &_rescale_tmp_5886, &_pgen_tmp_246);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n29._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n29._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5886._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_246._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n29._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5886._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_246._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_246._poly);
    Free_poly_data(&_rescale_tmp_5886._c1_poly);
    Free_poly_data(&_rescale_tmp_5886._c0_poly);
  }
  Copy_ciph(&_preg_268435569, &result_n29);
  Free_poly_data(&result_n29._c1_poly);
  Free_poly_data(&result_n29._c0_poly);
  Tm_taken("Tensor::conv");
  Init_ciph_down_scale(&_pgen_tmp_247, &_preg_268435569);
  Rescale(&_pgen_tmp_247._c0_poly, &_preg_268435569._c0_poly);
  Rescale(&_pgen_tmp_247._c1_poly, &_preg_268435569._c1_poly);
  Free_poly_data(&_preg_268435569._c1_poly);
  Free_poly_data(&_preg_268435569._c0_poly);
  Bootstrap(&_preg_268435570, &_pgen_tmp_247, 15);
  Free_poly_data(&_pgen_tmp_247._c1_poly);
  Free_poly_data(&_pgen_tmp_247._c0_poly);
  Encode_float(&_pgen_tmp_249, &_cst_470, 1, 1, Level(&_preg_268435570));
  Init_ciph_up_scale_plain(&_pgen_tmp_248, &_preg_268435570, &_pgen_tmp_249);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_248._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_248._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435570._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_249._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_248._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435570._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_249._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_249._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_250, &_pgen_tmp_248);
  Rescale(&_pgen_tmp_250._c0_poly, &_pgen_tmp_248._c0_poly);
  Rescale(&_pgen_tmp_250._c1_poly, &_pgen_tmp_248._c1_poly);
  Free_poly_data(&_pgen_tmp_248._c1_poly);
  Free_poly_data(&_pgen_tmp_248._c0_poly);
  _preg_268435571 = App_relu(_preg_268435570, _pgen_tmp_250);
  Free_poly_data(&_pgen_tmp_250._c1_poly);
  Free_poly_data(&_pgen_tmp_250._c0_poly);
  Free_poly_data(&_preg_268435570._c1_poly);
  Free_poly_data(&_preg_268435570._c0_poly);
  Copy_ciph(&_preg_268435572, &_preg_268435571);
  Free_poly_data(&_preg_268435571._c1_poly);
  Free_poly_data(&_preg_268435571._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n30, 0, sizeof(result_n30));
  Copy_ciph(&input_dup_n30, &_preg_268435572);
  Free_poly_data(&_preg_268435572._c1_poly);
  Free_poly_data(&_preg_268435572._c0_poly);
  for (index_khw1_n30 = 0; index_khw1_n30 < 9; index_khw1_n30 = index_khw1_n30 + 1) {
    _preg_268435743 = Rotate(input_dup_n30, _cst_289[index_khw1_n30]);
    Copy_ciph(&input_roll_n30[index_khw1_n30], &_preg_268435743);
    Free_poly_data(&_preg_268435743._c1_poly);
    Free_poly_data(&_preg_268435743._c0_poly);
  }
  Free_poly_data(&input_dup_n30._c1_poly);
  Free_poly_data(&input_dup_n30._c0_poly);
  for (index_cin_n30 = 0; index_cin_n30 < 32; index_cin_n30 = index_cin_n30 + 1) {
    memset(&result_cin_n30, 0, sizeof(result_cin_n30));
    for (index_khw2_n30 = 0; index_khw2_n30 < 9; index_khw2_n30 = index_khw2_n30 + 1) {
      _rescale_tmp_3233 = input_roll_n30[index_khw2_n30];
      Pt_from_msg(&_pgen_tmp_251, index_khw2_n30 + (index_cin_n30 * 9) + 5274 /* cst_287_0 */, 8192, Sc_degree(&_rescale_tmp_3233), Level(&_rescale_tmp_3233));
      Init_ciph_up_scale_plain(&_pgen_tmp_252, &_rescale_tmp_3233, &_pgen_tmp_251);
      Init_ciph_same_scale(&result_cin_n30, &result_cin_n30, &_pgen_tmp_252);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n30._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_252._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3233._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_251._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_252._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3233._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_251._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n30._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n30._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_252._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n30._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n30._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_252._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435744 = Rotate(result_cin_n30, -8192);
    Init_ciph_same_scale(&_pgen_tmp_253, &result_cin_n30, &_preg_268435744);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_253._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_253._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n30._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435744._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_253._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n30._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435744._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435745 = Rotate(_pgen_tmp_253, index_cin_n30 * 256);
    Init_ciph_same_scale(&result_n30, &result_n30, &_preg_268435745);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n30._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n30._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n30._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435745._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n30._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n30._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435745._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435744._c1_poly);
    Free_poly_data(&_preg_268435744._c0_poly);
    Free_poly_data(&_preg_268435745._c1_poly);
    Free_poly_data(&_preg_268435745._c0_poly);
  }
  Free_ciph_poly(input_roll_n30, 9);
  Free_poly_data(&_pgen_tmp_251._poly);
  Free_poly_data(&_pgen_tmp_252._c1_poly);
  Free_poly_data(&_pgen_tmp_252._c0_poly);
  Free_poly_data(&result_cin_n30._c1_poly);
  Free_poly_data(&result_cin_n30._c0_poly);
  Free_poly_data(&_pgen_tmp_253._c1_poly);
  Free_poly_data(&_pgen_tmp_253._c0_poly);
  Pt_from_msg(&_pgen_tmp_254, 5562 /* cst_288 */, 8192, Sc_degree(&result_n30), Level(&result_n30));
  Init_ciph_same_scale_plain(&result_n30, &result_n30, &_pgen_tmp_254);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n30._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n30._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n30._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_254._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n30._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n30._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_254._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_5899, &result_n30);
  Rescale(&_rescale_tmp_5899._c0_poly, &result_n30._c0_poly);
  Rescale(&_rescale_tmp_5899._c1_poly, &result_n30._c1_poly);
  Pt_from_msg(&_pgen_tmp_255, 5563 /* cst_290 */, 8192, Sc_degree(&_rescale_tmp_5899), Level(&_rescale_tmp_5899));
  Init_ciph_up_scale_plain(&result_n30, &_rescale_tmp_5899, &_pgen_tmp_255);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n30._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n30._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5899._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_255._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n30._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5899._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_255._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_255._poly);
    Free_poly_data(&_rescale_tmp_5899._c1_poly);
    Free_poly_data(&_rescale_tmp_5899._c0_poly);
  }
  Copy_ciph(&_preg_268435573, &result_n30);
  Free_poly_data(&result_n30._c1_poly);
  Free_poly_data(&result_n30._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::add");
  Init_ciph_down_scale(&_pgen_tmp_256, &_preg_268435573);
  Rescale(&_pgen_tmp_256._c0_poly, &_preg_268435573._c0_poly);
  Rescale(&_pgen_tmp_256._c1_poly, &_preg_268435573._c1_poly);
  Free_poly_data(&_preg_268435573._c1_poly);
  Free_poly_data(&_preg_268435573._c0_poly);
  Init_ciph_same_scale(&_preg_268435574, &_pgen_tmp_256, &_preg_268435568);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_preg_268435574._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_preg_268435574._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_256._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435568._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_preg_268435574._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_256._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435568._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435568._c1_poly);
    Free_poly_data(&_preg_268435568._c0_poly);
    Free_poly_data(&_pgen_tmp_256._c1_poly);
    Free_poly_data(&_pgen_tmp_256._c0_poly);
  }
  Tm_taken("Tensor::add");
  Bootstrap(&_preg_268435575, &_preg_268435574, 15);
  Free_poly_data(&_preg_268435574._c1_poly);
  Free_poly_data(&_preg_268435574._c0_poly);
  Encode_float(&_pgen_tmp_258, &_cst_471, 1, 1, Level(&_preg_268435575));
  Init_ciph_up_scale_plain(&_pgen_tmp_257, &_preg_268435575, &_pgen_tmp_258);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_257._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_257._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435575._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_258._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_257._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435575._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_258._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_258._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_259, &_pgen_tmp_257);
  Rescale(&_pgen_tmp_259._c0_poly, &_pgen_tmp_257._c0_poly);
  Rescale(&_pgen_tmp_259._c1_poly, &_pgen_tmp_257._c1_poly);
  Free_poly_data(&_pgen_tmp_257._c1_poly);
  Free_poly_data(&_pgen_tmp_257._c0_poly);
  _preg_268435576 = App_relu(_preg_268435575, _pgen_tmp_259);
  Free_poly_data(&_pgen_tmp_259._c1_poly);
  Free_poly_data(&_pgen_tmp_259._c0_poly);
  Free_poly_data(&_preg_268435575._c1_poly);
  Free_poly_data(&_preg_268435575._c0_poly);
  Copy_ciph(&_preg_268435577, &_preg_268435576);
  Free_poly_data(&_preg_268435576._c1_poly);
  Free_poly_data(&_preg_268435576._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n31, 0, sizeof(result_n31));
  Copy_ciph(&input_dup_n31, &_preg_268435577);
  for (index_khw1_n31 = 0; index_khw1_n31 < 9; index_khw1_n31 = index_khw1_n31 + 1) {
    _preg_268435746 = Rotate(input_dup_n31, _cst_296[index_khw1_n31]);
    Copy_ciph(&input_roll_n31[index_khw1_n31], &_preg_268435746);
    Free_poly_data(&_preg_268435746._c1_poly);
    Free_poly_data(&_preg_268435746._c0_poly);
  }
  Free_poly_data(&input_dup_n31._c1_poly);
  Free_poly_data(&input_dup_n31._c0_poly);
  for (index_cin_n31 = 0; index_cin_n31 < 32; index_cin_n31 = index_cin_n31 + 1) {
    memset(&result_cin_n31, 0, sizeof(result_cin_n31));
    for (index_khw2_n31 = 0; index_khw2_n31 < 9; index_khw2_n31 = index_khw2_n31 + 1) {
      _rescale_tmp_3347 = input_roll_n31[index_khw2_n31];
      Pt_from_msg(&_pgen_tmp_260, index_khw2_n31 + (index_cin_n31 * 9) + 5564 /* cst_294_0 */, 8192, Sc_degree(&_rescale_tmp_3347), Level(&_rescale_tmp_3347));
      Init_ciph_up_scale_plain(&_pgen_tmp_261, &_rescale_tmp_3347, &_pgen_tmp_260);
      Init_ciph_same_scale(&result_cin_n31, &result_cin_n31, &_pgen_tmp_261);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n31._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_261._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3347._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_260._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_261._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3347._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_260._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n31._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n31._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_261._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n31._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n31._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_261._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435747 = Rotate(result_cin_n31, -8192);
    Init_ciph_same_scale(&_pgen_tmp_262, &result_cin_n31, &_preg_268435747);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_262._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_262._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n31._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435747._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_262._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n31._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435747._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435748 = Rotate(_pgen_tmp_262, index_cin_n31 * 256);
    Init_ciph_same_scale(&result_n31, &result_n31, &_preg_268435748);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n31._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n31._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n31._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435748._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n31._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n31._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435748._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435747._c1_poly);
    Free_poly_data(&_preg_268435747._c0_poly);
    Free_poly_data(&_preg_268435748._c1_poly);
    Free_poly_data(&_preg_268435748._c0_poly);
  }
  Free_ciph_poly(input_roll_n31, 9);
  Free_poly_data(&_pgen_tmp_260._poly);
  Free_poly_data(&_pgen_tmp_261._c1_poly);
  Free_poly_data(&_pgen_tmp_261._c0_poly);
  Free_poly_data(&result_cin_n31._c1_poly);
  Free_poly_data(&result_cin_n31._c0_poly);
  Free_poly_data(&_pgen_tmp_262._c1_poly);
  Free_poly_data(&_pgen_tmp_262._c0_poly);
  Pt_from_msg(&_pgen_tmp_263, 5852 /* cst_295 */, 8192, Sc_degree(&result_n31), Level(&result_n31));
  Init_ciph_same_scale_plain(&result_n31, &result_n31, &_pgen_tmp_263);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n31._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n31._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n31._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_263._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n31._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n31._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_263._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_5912, &result_n31);
  Rescale(&_rescale_tmp_5912._c0_poly, &result_n31._c0_poly);
  Rescale(&_rescale_tmp_5912._c1_poly, &result_n31._c1_poly);
  Pt_from_msg(&_pgen_tmp_264, 5853 /* cst_297 */, 8192, Sc_degree(&_rescale_tmp_5912), Level(&_rescale_tmp_5912));
  Init_ciph_up_scale_plain(&result_n31, &_rescale_tmp_5912, &_pgen_tmp_264);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n31._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n31._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5912._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_264._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n31._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5912._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_264._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_264._poly);
    Free_poly_data(&_rescale_tmp_5912._c1_poly);
    Free_poly_data(&_rescale_tmp_5912._c0_poly);
  }
  Copy_ciph(&_preg_268435578, &result_n31);
  Free_poly_data(&result_n31._c1_poly);
  Free_poly_data(&result_n31._c0_poly);
  Tm_taken("Tensor::conv");
  Init_ciph_down_scale(&_pgen_tmp_265, &_preg_268435578);
  Rescale(&_pgen_tmp_265._c0_poly, &_preg_268435578._c0_poly);
  Rescale(&_pgen_tmp_265._c1_poly, &_preg_268435578._c1_poly);
  Free_poly_data(&_preg_268435578._c1_poly);
  Free_poly_data(&_preg_268435578._c0_poly);
  Bootstrap(&_preg_268435579, &_pgen_tmp_265, 15);
  Free_poly_data(&_pgen_tmp_265._c1_poly);
  Free_poly_data(&_pgen_tmp_265._c0_poly);
  Encode_float(&_pgen_tmp_267, &_cst_472, 1, 1, Level(&_preg_268435579));
  Init_ciph_up_scale_plain(&_pgen_tmp_266, &_preg_268435579, &_pgen_tmp_267);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_266._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_266._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435579._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_267._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_266._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435579._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_267._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_267._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_268, &_pgen_tmp_266);
  Rescale(&_pgen_tmp_268._c0_poly, &_pgen_tmp_266._c0_poly);
  Rescale(&_pgen_tmp_268._c1_poly, &_pgen_tmp_266._c1_poly);
  Free_poly_data(&_pgen_tmp_266._c1_poly);
  Free_poly_data(&_pgen_tmp_266._c0_poly);
  _preg_268435580 = App_relu(_preg_268435579, _pgen_tmp_268);
  Free_poly_data(&_pgen_tmp_268._c1_poly);
  Free_poly_data(&_pgen_tmp_268._c0_poly);
  Free_poly_data(&_preg_268435579._c1_poly);
  Free_poly_data(&_preg_268435579._c0_poly);
  Copy_ciph(&_preg_268435581, &_preg_268435580);
  Free_poly_data(&_preg_268435580._c1_poly);
  Free_poly_data(&_preg_268435580._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n32, 0, sizeof(result_n32));
  Copy_ciph(&input_dup_n32, &_preg_268435581);
  Free_poly_data(&_preg_268435581._c1_poly);
  Free_poly_data(&_preg_268435581._c0_poly);
  for (index_khw1_n32 = 0; index_khw1_n32 < 9; index_khw1_n32 = index_khw1_n32 + 1) {
    _preg_268435749 = Rotate(input_dup_n32, _cst_302[index_khw1_n32]);
    Copy_ciph(&input_roll_n32[index_khw1_n32], &_preg_268435749);
    Free_poly_data(&_preg_268435749._c1_poly);
    Free_poly_data(&_preg_268435749._c0_poly);
  }
  Free_poly_data(&input_dup_n32._c1_poly);
  Free_poly_data(&input_dup_n32._c0_poly);
  for (index_cin_n32 = 0; index_cin_n32 < 32; index_cin_n32 = index_cin_n32 + 1) {
    memset(&result_cin_n32, 0, sizeof(result_cin_n32));
    for (index_khw2_n32 = 0; index_khw2_n32 < 9; index_khw2_n32 = index_khw2_n32 + 1) {
      _rescale_tmp_3455 = input_roll_n32[index_khw2_n32];
      Pt_from_msg(&_pgen_tmp_269, index_khw2_n32 + (index_cin_n32 * 9) + 5854 /* cst_300_0 */, 8192, Sc_degree(&_rescale_tmp_3455), Level(&_rescale_tmp_3455));
      Init_ciph_up_scale_plain(&_pgen_tmp_270, &_rescale_tmp_3455, &_pgen_tmp_269);
      Init_ciph_same_scale(&result_cin_n32, &result_cin_n32, &_pgen_tmp_270);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n32._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_270._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3455._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_269._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_270._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3455._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_269._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n32._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n32._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_270._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n32._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n32._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_270._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435750 = Rotate(result_cin_n32, -8192);
    Init_ciph_same_scale(&_pgen_tmp_271, &result_cin_n32, &_preg_268435750);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_271._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_271._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n32._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435750._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_271._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n32._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435750._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435751 = Rotate(_pgen_tmp_271, index_cin_n32 * 256);
    Init_ciph_same_scale(&result_n32, &result_n32, &_preg_268435751);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n32._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n32._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n32._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435751._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n32._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n32._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435751._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435750._c1_poly);
    Free_poly_data(&_preg_268435750._c0_poly);
    Free_poly_data(&_preg_268435751._c1_poly);
    Free_poly_data(&_preg_268435751._c0_poly);
  }
  Free_ciph_poly(input_roll_n32, 9);
  Free_poly_data(&_pgen_tmp_269._poly);
  Free_poly_data(&_pgen_tmp_270._c1_poly);
  Free_poly_data(&_pgen_tmp_270._c0_poly);
  Free_poly_data(&result_cin_n32._c1_poly);
  Free_poly_data(&result_cin_n32._c0_poly);
  Free_poly_data(&_pgen_tmp_271._c1_poly);
  Free_poly_data(&_pgen_tmp_271._c0_poly);
  Pt_from_msg(&_pgen_tmp_272, 6142 /* cst_301 */, 8192, Sc_degree(&result_n32), Level(&result_n32));
  Init_ciph_same_scale_plain(&result_n32, &result_n32, &_pgen_tmp_272);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n32._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n32._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n32._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_272._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n32._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n32._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_272._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_5925, &result_n32);
  Rescale(&_rescale_tmp_5925._c0_poly, &result_n32._c0_poly);
  Rescale(&_rescale_tmp_5925._c1_poly, &result_n32._c1_poly);
  Pt_from_msg(&_pgen_tmp_273, 6143 /* cst_303 */, 8192, Sc_degree(&_rescale_tmp_5925), Level(&_rescale_tmp_5925));
  Init_ciph_up_scale_plain(&result_n32, &_rescale_tmp_5925, &_pgen_tmp_273);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n32._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n32._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5925._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_273._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n32._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5925._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_273._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_273._poly);
    Free_poly_data(&_rescale_tmp_5925._c1_poly);
    Free_poly_data(&_rescale_tmp_5925._c0_poly);
  }
  Copy_ciph(&_preg_268435582, &result_n32);
  Free_poly_data(&result_n32._c1_poly);
  Free_poly_data(&result_n32._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::add");
  Init_ciph_down_scale(&_pgen_tmp_274, &_preg_268435582);
  Rescale(&_pgen_tmp_274._c0_poly, &_preg_268435582._c0_poly);
  Rescale(&_pgen_tmp_274._c1_poly, &_preg_268435582._c1_poly);
  Free_poly_data(&_preg_268435582._c1_poly);
  Free_poly_data(&_preg_268435582._c0_poly);
  Init_ciph_same_scale(&_preg_268435583, &_pgen_tmp_274, &_preg_268435577);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_preg_268435583._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_preg_268435583._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_274._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435577._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_preg_268435583._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_274._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435577._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435577._c1_poly);
    Free_poly_data(&_preg_268435577._c0_poly);
    Free_poly_data(&_pgen_tmp_274._c1_poly);
    Free_poly_data(&_pgen_tmp_274._c0_poly);
  }
  Tm_taken("Tensor::add");
  Bootstrap(&_preg_268435584, &_preg_268435583, 18);
  Free_poly_data(&_preg_268435583._c1_poly);
  Free_poly_data(&_preg_268435583._c0_poly);
  Encode_float(&_pgen_tmp_276, &_cst_473, 1, 1, Level(&_preg_268435584));
  Init_ciph_up_scale_plain(&_pgen_tmp_275, &_preg_268435584, &_pgen_tmp_276);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_275._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_275._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435584._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_276._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_275._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435584._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_276._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_276._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_277, &_pgen_tmp_275);
  Rescale(&_pgen_tmp_277._c0_poly, &_pgen_tmp_275._c0_poly);
  Rescale(&_pgen_tmp_277._c1_poly, &_pgen_tmp_275._c1_poly);
  Free_poly_data(&_pgen_tmp_275._c1_poly);
  Free_poly_data(&_pgen_tmp_275._c0_poly);
  _preg_268435585 = App_relu(_preg_268435584, _pgen_tmp_277);
  Free_poly_data(&_pgen_tmp_277._c1_poly);
  Free_poly_data(&_pgen_tmp_277._c0_poly);
  Free_poly_data(&_preg_268435584._c1_poly);
  Free_poly_data(&_preg_268435584._c0_poly);
  Copy_ciph(&_preg_268435586, &_preg_268435585);
  Free_poly_data(&_preg_268435585._c1_poly);
  Free_poly_data(&_preg_268435585._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n33, 0, sizeof(result_n33));
  _preg_268435752 = Rotate(_preg_268435586, -8192);
  Init_ciph_same_scale(&input_dup_n33, &_preg_268435586, &_preg_268435752);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&input_dup_n33._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&input_dup_n33._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435586._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435752._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&input_dup_n33._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435586._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435752._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435752._c1_poly);
    Free_poly_data(&_preg_268435752._c0_poly);
  }
  for (index_khw1_n33 = 0; index_khw1_n33 < 9; index_khw1_n33 = index_khw1_n33 + 1) {
    _preg_268435753 = Rotate(input_dup_n33, _cst_309[index_khw1_n33]);
    Copy_ciph(&input_roll_n33[index_khw1_n33], &_preg_268435753);
    Free_poly_data(&_preg_268435753._c1_poly);
    Free_poly_data(&_preg_268435753._c0_poly);
  }
  Free_poly_data(&input_dup_n33._c1_poly);
  Free_poly_data(&input_dup_n33._c0_poly);
  for (index_cin_n33 = 0; index_cin_n33 < 32; index_cin_n33 = index_cin_n33 + 1) {
    memset(&result_cin_n33, 0, sizeof(result_cin_n33));
    for (index_khw2_n33 = 0; index_khw2_n33 < 9; index_khw2_n33 = index_khw2_n33 + 1) {
      _rescale_tmp_3573 = input_roll_n33[index_khw2_n33];
      Pt_from_msg(&_pgen_tmp_278, index_khw2_n33 + (index_cin_n33 * 9) + 6144 /* cst_307_0 */, 16384, Sc_degree(&_rescale_tmp_3573), Level(&_rescale_tmp_3573));
      Init_ciph_up_scale_plain(&_pgen_tmp_279, &_rescale_tmp_3573, &_pgen_tmp_278);
      Init_ciph_same_scale(&result_cin_n33, &result_cin_n33, &_pgen_tmp_279);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n33._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_279._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3573._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_278._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_279._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3573._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_278._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n33._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n33._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_279._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n33._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n33._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_279._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435754 = Rotate(result_cin_n33, -16384);
    Init_ciph_same_scale(&_pgen_tmp_280, &result_cin_n33, &_preg_268435754);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_280._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_280._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n33._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435754._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_280._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n33._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435754._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435755 = Rotate(_pgen_tmp_280, index_cin_n33 * 256);
    Init_ciph_same_scale(&result_n33, &result_n33, &_preg_268435755);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n33._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n33._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n33._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435755._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n33._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n33._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435755._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435754._c1_poly);
    Free_poly_data(&_preg_268435754._c0_poly);
    Free_poly_data(&_preg_268435755._c1_poly);
    Free_poly_data(&_preg_268435755._c0_poly);
  }
  Free_ciph_poly(input_roll_n33, 9);
  Free_poly_data(&_pgen_tmp_278._poly);
  Free_poly_data(&_pgen_tmp_279._c1_poly);
  Free_poly_data(&_pgen_tmp_279._c0_poly);
  Free_poly_data(&result_cin_n33._c1_poly);
  Free_poly_data(&result_cin_n33._c0_poly);
  Free_poly_data(&_pgen_tmp_280._c1_poly);
  Free_poly_data(&_pgen_tmp_280._c0_poly);
  Pt_from_msg(&_pgen_tmp_281, 6432 /* cst_308 */, 16384, Sc_degree(&result_n33), Level(&result_n33));
  Init_ciph_same_scale_plain(&result_n33, &result_n33, &_pgen_tmp_281);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n33._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n33._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n33._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_281._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n33._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n33._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_281._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_5938, &result_n33);
  Rescale(&_rescale_tmp_5938._c0_poly, &result_n33._c0_poly);
  Rescale(&_rescale_tmp_5938._c1_poly, &result_n33._c1_poly);
  Pt_from_msg(&_pgen_tmp_282, 6433 /* cst_310 */, 16384, Sc_degree(&_rescale_tmp_5938), Level(&_rescale_tmp_5938));
  Init_ciph_up_scale_plain(&result_n33, &_rescale_tmp_5938, &_pgen_tmp_282);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n33._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n33._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5938._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_282._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n33._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5938._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_282._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_282._poly);
    Free_poly_data(&_rescale_tmp_5938._c1_poly);
    Free_poly_data(&_rescale_tmp_5938._c0_poly);
  }
  Copy_ciph(&orig_result33, &result_n33);
  Free_poly_data(&result_n33._c1_poly);
  Free_poly_data(&result_n33._c0_poly);
  memset(&comb_row_result34, 0, sizeof(comb_row_result34));
  for (combine_row_index_n34 = 0; combine_row_index_n34 < 8; combine_row_index_n34 = combine_row_index_n34 + 1) {
    _preg_268435756 = Rotate(orig_result33, combine_row_index_n34 * 1);
    Copy_ciph(&_ckks_gen_tmp_3639, &_preg_268435756);
    Init_ciph_down_scale(&_rescale_tmp_5944, &_ckks_gen_tmp_3639);
    Rescale(&_rescale_tmp_5944._c0_poly, &_ckks_gen_tmp_3639._c0_poly);
    Rescale(&_rescale_tmp_5944._c1_poly, &_ckks_gen_tmp_3639._c1_poly);
    Pt_from_msg(&_pgen_tmp_283, combine_row_index_n34 + 6434 /* cst_311_0 */, 16384, Sc_degree(&_rescale_tmp_5944), Level(&_rescale_tmp_5944));
    Init_ciph_up_scale_plain(&_pgen_tmp_284, &_rescale_tmp_5944, &_pgen_tmp_283);
    Init_ciph_same_scale(&comb_row_result34, &comb_row_result34, &_pgen_tmp_284);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&comb_row_result34._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_284._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5944._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_283._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_284._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5944._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_283._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_row_result34._c0_poly, _pgen_rns_idx, degree), Coeffs(&comb_row_result34._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_284._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_row_result34._c1_poly, _pgen_rns_idx, degree), Coeffs(&comb_row_result34._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_284._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435756._c1_poly);
    Free_poly_data(&_preg_268435756._c0_poly);
  }
  Free_poly_data(&orig_result33._c1_poly);
  Free_poly_data(&orig_result33._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_3639._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_3639._c0_poly);
  Free_poly_data(&_pgen_tmp_283._poly);
  Free_poly_data(&_rescale_tmp_5944._c1_poly);
  Free_poly_data(&_rescale_tmp_5944._c0_poly);
  Free_poly_data(&_pgen_tmp_284._c1_poly);
  Free_poly_data(&_pgen_tmp_284._c0_poly);
  memset(&comb_rc_result34, 0, sizeof(comb_rc_result34));
  for (combine_rc_index_n34 = 0; combine_rc_index_n34 < 8; combine_rc_index_n34 = combine_rc_index_n34 + 1) {
    _preg_268435757 = Rotate(comb_row_result34, combine_rc_index_n34 * 24);
    Copy_ciph(&_ckks_gen_tmp_3670, &_preg_268435757);
    Init_ciph_down_scale(&_rescale_tmp_5950, &_ckks_gen_tmp_3670);
    Rescale(&_rescale_tmp_5950._c0_poly, &_ckks_gen_tmp_3670._c0_poly);
    Rescale(&_rescale_tmp_5950._c1_poly, &_ckks_gen_tmp_3670._c1_poly);
    Pt_from_msg(&_pgen_tmp_285, combine_rc_index_n34 + 6442 /* cst_312_0 */, 16384, Sc_degree(&_rescale_tmp_5950), Level(&_rescale_tmp_5950));
    Init_ciph_up_scale_plain(&_pgen_tmp_286, &_rescale_tmp_5950, &_pgen_tmp_285);
    Init_ciph_same_scale(&comb_rc_result34, &comb_rc_result34, &_pgen_tmp_286);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&comb_rc_result34._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_286._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5950._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_285._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_286._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5950._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_285._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_rc_result34._c0_poly, _pgen_rns_idx, degree), Coeffs(&comb_rc_result34._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_286._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_rc_result34._c1_poly, _pgen_rns_idx, degree), Coeffs(&comb_rc_result34._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_286._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435757._c1_poly);
    Free_poly_data(&_preg_268435757._c0_poly);
  }
  Free_poly_data(&comb_row_result34._c1_poly);
  Free_poly_data(&comb_row_result34._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_3670._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_3670._c0_poly);
  Free_poly_data(&_pgen_tmp_285._poly);
  Free_poly_data(&_rescale_tmp_5950._c1_poly);
  Free_poly_data(&_rescale_tmp_5950._c0_poly);
  Free_poly_data(&_pgen_tmp_286._c1_poly);
  Free_poly_data(&_pgen_tmp_286._c0_poly);
  memset(&final_result34, 0, sizeof(final_result34));
  for (combine_cc_index_n34 = 0; combine_cc_index_n34 < 64; combine_cc_index_n34 = combine_cc_index_n34 + 1) {
    _preg_268435758 = Rotate(comb_rc_result34, combine_cc_index_n34 * 192);
    Copy_ciph(&_ckks_gen_tmp_3701, &_preg_268435758);
    Init_ciph_down_scale(&_rescale_tmp_5956, &_ckks_gen_tmp_3701);
    Rescale(&_rescale_tmp_5956._c0_poly, &_ckks_gen_tmp_3701._c0_poly);
    Rescale(&_rescale_tmp_5956._c1_poly, &_ckks_gen_tmp_3701._c1_poly);
    Pt_from_msg(&_pgen_tmp_287, combine_cc_index_n34 + 6450 /* cst_313_0 */, 16384, Sc_degree(&_rescale_tmp_5956), Level(&_rescale_tmp_5956));
    Init_ciph_up_scale_plain(&_pgen_tmp_288, &_rescale_tmp_5956, &_pgen_tmp_287);
    Init_ciph_same_scale(&final_result34, &final_result34, &_pgen_tmp_288);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&final_result34._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_288._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5956._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_287._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_288._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5956._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_287._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&final_result34._c0_poly, _pgen_rns_idx, degree), Coeffs(&final_result34._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_288._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&final_result34._c1_poly, _pgen_rns_idx, degree), Coeffs(&final_result34._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_288._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435758._c1_poly);
    Free_poly_data(&_preg_268435758._c0_poly);
  }
  Free_poly_data(&comb_rc_result34._c1_poly);
  Free_poly_data(&comb_rc_result34._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_3701._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_3701._c0_poly);
  Free_poly_data(&_pgen_tmp_287._poly);
  Free_poly_data(&_rescale_tmp_5956._c1_poly);
  Free_poly_data(&_rescale_tmp_5956._c0_poly);
  Free_poly_data(&_pgen_tmp_288._c1_poly);
  Free_poly_data(&_pgen_tmp_288._c0_poly);
  Copy_ciph(&_preg_268435587, &final_result34);
  Free_poly_data(&final_result34._c1_poly);
  Free_poly_data(&final_result34._c0_poly);
  Tm_taken("Tensor::conv");
  Init_ciph_down_scale(&_pgen_tmp_289, &_preg_268435587);
  Rescale(&_pgen_tmp_289._c0_poly, &_preg_268435587._c0_poly);
  Rescale(&_pgen_tmp_289._c1_poly, &_preg_268435587._c1_poly);
  Free_poly_data(&_preg_268435587._c1_poly);
  Free_poly_data(&_preg_268435587._c0_poly);
  Bootstrap(&_preg_268435588, &_pgen_tmp_289, 15);
  Free_poly_data(&_pgen_tmp_289._c1_poly);
  Free_poly_data(&_pgen_tmp_289._c0_poly);
  Encode_float(&_pgen_tmp_291, &_cst_474, 1, 1, Level(&_preg_268435588));
  Init_ciph_up_scale_plain(&_pgen_tmp_290, &_preg_268435588, &_pgen_tmp_291);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_290._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_290._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435588._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_291._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_290._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435588._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_291._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_291._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_292, &_pgen_tmp_290);
  Rescale(&_pgen_tmp_292._c0_poly, &_pgen_tmp_290._c0_poly);
  Rescale(&_pgen_tmp_292._c1_poly, &_pgen_tmp_290._c1_poly);
  Free_poly_data(&_pgen_tmp_290._c1_poly);
  Free_poly_data(&_pgen_tmp_290._c0_poly);
  _preg_268435589 = App_relu(_preg_268435588, _pgen_tmp_292);
  Free_poly_data(&_pgen_tmp_292._c1_poly);
  Free_poly_data(&_pgen_tmp_292._c0_poly);
  Free_poly_data(&_preg_268435588._c1_poly);
  Free_poly_data(&_preg_268435588._c0_poly);
  Copy_ciph(&_preg_268435590, &_preg_268435589);
  Free_poly_data(&_preg_268435589._c1_poly);
  Free_poly_data(&_preg_268435589._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n35, 0, sizeof(result_n35));
  Copy_ciph(&input_dup_n35, &_preg_268435590);
  Free_poly_data(&_preg_268435590._c1_poly);
  Free_poly_data(&_preg_268435590._c0_poly);
  for (index_khw1_n35 = 0; index_khw1_n35 < 9; index_khw1_n35 = index_khw1_n35 + 1) {
    _preg_268435759 = Rotate(input_dup_n35, _cst_318[index_khw1_n35]);
    Copy_ciph(&input_roll_n35[index_khw1_n35], &_preg_268435759);
    Free_poly_data(&_preg_268435759._c1_poly);
    Free_poly_data(&_preg_268435759._c0_poly);
  }
  Free_poly_data(&input_dup_n35._c1_poly);
  Free_poly_data(&input_dup_n35._c0_poly);
  for (index_cin_n35 = 0; index_cin_n35 < 64; index_cin_n35 = index_cin_n35 + 1) {
    memset(&result_cin_n35, 0, sizeof(result_cin_n35));
    for (index_khw2_n35 = 0; index_khw2_n35 < 9; index_khw2_n35 = index_khw2_n35 + 1) {
      _rescale_tmp_3776 = input_roll_n35[index_khw2_n35];
      Pt_from_msg(&_pgen_tmp_293, index_khw2_n35 + (index_cin_n35 * 9) + 6514 /* cst_316_0 */, 4096, Sc_degree(&_rescale_tmp_3776), Level(&_rescale_tmp_3776));
      Init_ciph_up_scale_plain(&_pgen_tmp_294, &_rescale_tmp_3776, &_pgen_tmp_293);
      Init_ciph_same_scale(&result_cin_n35, &result_cin_n35, &_pgen_tmp_294);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n35._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_294._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3776._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_293._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_294._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3776._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_293._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n35._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n35._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_294._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n35._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n35._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_294._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435760 = Rotate(result_cin_n35, -4096);
    Init_ciph_same_scale(&_pgen_tmp_295, &result_cin_n35, &_preg_268435760);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_295._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_295._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n35._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435760._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_295._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n35._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435760._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435761 = Rotate(_pgen_tmp_295, index_cin_n35 * 64);
    Init_ciph_same_scale(&result_n35, &result_n35, &_preg_268435761);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n35._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n35._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n35._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435761._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n35._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n35._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435761._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435760._c1_poly);
    Free_poly_data(&_preg_268435760._c0_poly);
    Free_poly_data(&_preg_268435761._c1_poly);
    Free_poly_data(&_preg_268435761._c0_poly);
  }
  Free_ciph_poly(input_roll_n35, 9);
  Free_poly_data(&_pgen_tmp_293._poly);
  Free_poly_data(&_pgen_tmp_294._c1_poly);
  Free_poly_data(&_pgen_tmp_294._c0_poly);
  Free_poly_data(&result_cin_n35._c1_poly);
  Free_poly_data(&result_cin_n35._c0_poly);
  Free_poly_data(&_pgen_tmp_295._c1_poly);
  Free_poly_data(&_pgen_tmp_295._c0_poly);
  Pt_from_msg(&_pgen_tmp_296, 7090 /* cst_317 */, 4096, Sc_degree(&result_n35), Level(&result_n35));
  Init_ciph_same_scale_plain(&result_n35, &result_n35, &_pgen_tmp_296);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n35._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n35._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n35._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_296._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n35._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n35._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_296._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_5969, &result_n35);
  Rescale(&_rescale_tmp_5969._c0_poly, &result_n35._c0_poly);
  Rescale(&_rescale_tmp_5969._c1_poly, &result_n35._c1_poly);
  Pt_from_msg(&_pgen_tmp_297, 7091 /* cst_319 */, 4096, Sc_degree(&_rescale_tmp_5969), Level(&_rescale_tmp_5969));
  Init_ciph_up_scale_plain(&result_n35, &_rescale_tmp_5969, &_pgen_tmp_297);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n35._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n35._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5969._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_297._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n35._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5969._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_297._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_297._poly);
    Free_poly_data(&_rescale_tmp_5969._c1_poly);
    Free_poly_data(&_rescale_tmp_5969._c0_poly);
  }
  Copy_ciph(&_preg_268435591, &result_n35);
  Free_poly_data(&result_n35._c1_poly);
  Free_poly_data(&result_n35._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::conv");
  memset(&result_n36, 0, sizeof(result_n36));
  _preg_268435762 = Rotate(_preg_268435586, -8192);
  Init_ciph_same_scale(&input_dup_n36, &_preg_268435586, &_preg_268435762);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&input_dup_n36._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&input_dup_n36._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435586._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435762._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&input_dup_n36._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435586._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435762._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435762._c1_poly);
    Free_poly_data(&_preg_268435762._c0_poly);
    Free_poly_data(&_preg_268435586._c1_poly);
    Free_poly_data(&_preg_268435586._c0_poly);
  }
  for (index_khw1_n36 = 0; index_khw1_n36 < 1; index_khw1_n36 = index_khw1_n36 + 1) {
    _preg_268435763 = Rotate(input_dup_n36, _cst_324[index_khw1_n36]);
    Copy_ciph(&input_roll_n36[index_khw1_n36], &_preg_268435763);
    Free_poly_data(&_preg_268435763._c1_poly);
    Free_poly_data(&_preg_268435763._c0_poly);
  }
  Free_poly_data(&input_dup_n36._c1_poly);
  Free_poly_data(&input_dup_n36._c0_poly);
  for (index_cin_n36 = 0; index_cin_n36 < 32; index_cin_n36 = index_cin_n36 + 1) {
    memset(&result_cin_n36, 0, sizeof(result_cin_n36));
    for (index_khw2_n36 = 0; index_khw2_n36 < 1; index_khw2_n36 = index_khw2_n36 + 1) {
      _rescale_tmp_3878 = input_roll_n36[index_khw2_n36];
      Pt_from_msg(&_pgen_tmp_298, index_khw2_n36 + (index_cin_n36 * 1) + 7092 /* cst_322_0 */, 16384, Sc_degree(&_rescale_tmp_3878), Level(&_rescale_tmp_3878));
      Init_ciph_up_scale_plain(&_pgen_tmp_299, &_rescale_tmp_3878, &_pgen_tmp_298);
      Init_ciph_same_scale(&result_cin_n36, &result_cin_n36, &_pgen_tmp_299);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n36._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_299._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3878._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_298._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_299._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_3878._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_298._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n36._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n36._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_299._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n36._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n36._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_299._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435764 = Rotate(result_cin_n36, -16384);
    Init_ciph_same_scale(&_pgen_tmp_300, &result_cin_n36, &_preg_268435764);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_300._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_300._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n36._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435764._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_300._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n36._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435764._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435765 = Rotate(_pgen_tmp_300, index_cin_n36 * 256);
    Init_ciph_same_scale(&result_n36, &result_n36, &_preg_268435765);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n36._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n36._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n36._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435765._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n36._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n36._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435765._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435764._c1_poly);
    Free_poly_data(&_preg_268435764._c0_poly);
    Free_poly_data(&_preg_268435765._c1_poly);
    Free_poly_data(&_preg_268435765._c0_poly);
  }
  Free_ciph_poly(input_roll_n36, 1);
  Free_poly_data(&_pgen_tmp_298._poly);
  Free_poly_data(&_pgen_tmp_299._c1_poly);
  Free_poly_data(&_pgen_tmp_299._c0_poly);
  Free_poly_data(&result_cin_n36._c1_poly);
  Free_poly_data(&result_cin_n36._c0_poly);
  Free_poly_data(&_pgen_tmp_300._c1_poly);
  Free_poly_data(&_pgen_tmp_300._c0_poly);
  Pt_from_msg(&_pgen_tmp_301, 7124 /* cst_323 */, 16384, Sc_degree(&result_n36), Level(&result_n36));
  Init_ciph_same_scale_plain(&result_n36, &result_n36, &_pgen_tmp_301);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n36._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n36._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n36._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_301._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n36._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n36._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_301._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_5980, &result_n36);
  Rescale(&_rescale_tmp_5980._c0_poly, &result_n36._c0_poly);
  Rescale(&_rescale_tmp_5980._c1_poly, &result_n36._c1_poly);
  Pt_from_msg(&_pgen_tmp_302, 7125 /* cst_325 */, 16384, Sc_degree(&_rescale_tmp_5980), Level(&_rescale_tmp_5980));
  Init_ciph_up_scale_plain(&result_n36, &_rescale_tmp_5980, &_pgen_tmp_302);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n36._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n36._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5980._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_302._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n36._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5980._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_302._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_302._poly);
    Free_poly_data(&_rescale_tmp_5980._c1_poly);
    Free_poly_data(&_rescale_tmp_5980._c0_poly);
  }
  Copy_ciph(&orig_result36, &result_n36);
  Free_poly_data(&result_n36._c1_poly);
  Free_poly_data(&result_n36._c0_poly);
  memset(&comb_row_result37, 0, sizeof(comb_row_result37));
  for (combine_row_index_n37 = 0; combine_row_index_n37 < 8; combine_row_index_n37 = combine_row_index_n37 + 1) {
    _preg_268435766 = Rotate(orig_result36, combine_row_index_n37 * 1);
    Copy_ciph(&_ckks_gen_tmp_3944, &_preg_268435766);
    Init_ciph_down_scale(&_rescale_tmp_5986, &_ckks_gen_tmp_3944);
    Rescale(&_rescale_tmp_5986._c0_poly, &_ckks_gen_tmp_3944._c0_poly);
    Rescale(&_rescale_tmp_5986._c1_poly, &_ckks_gen_tmp_3944._c1_poly);
    Pt_from_msg(&_pgen_tmp_303, combine_row_index_n37 + 7126 /* cst_326_0 */, 16384, Sc_degree(&_rescale_tmp_5986), Level(&_rescale_tmp_5986));
    Init_ciph_up_scale_plain(&_pgen_tmp_304, &_rescale_tmp_5986, &_pgen_tmp_303);
    Init_ciph_same_scale(&comb_row_result37, &comb_row_result37, &_pgen_tmp_304);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&comb_row_result37._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_304._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5986._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_303._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_304._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5986._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_303._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_row_result37._c0_poly, _pgen_rns_idx, degree), Coeffs(&comb_row_result37._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_304._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_row_result37._c1_poly, _pgen_rns_idx, degree), Coeffs(&comb_row_result37._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_304._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435766._c1_poly);
    Free_poly_data(&_preg_268435766._c0_poly);
  }
  Free_poly_data(&orig_result36._c1_poly);
  Free_poly_data(&orig_result36._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_3944._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_3944._c0_poly);
  Free_poly_data(&_pgen_tmp_303._poly);
  Free_poly_data(&_rescale_tmp_5986._c1_poly);
  Free_poly_data(&_rescale_tmp_5986._c0_poly);
  Free_poly_data(&_pgen_tmp_304._c1_poly);
  Free_poly_data(&_pgen_tmp_304._c0_poly);
  memset(&comb_rc_result37, 0, sizeof(comb_rc_result37));
  for (combine_rc_index_n37 = 0; combine_rc_index_n37 < 8; combine_rc_index_n37 = combine_rc_index_n37 + 1) {
    _preg_268435767 = Rotate(comb_row_result37, combine_rc_index_n37 * 24);
    Copy_ciph(&_ckks_gen_tmp_3975, &_preg_268435767);
    Init_ciph_down_scale(&_rescale_tmp_5992, &_ckks_gen_tmp_3975);
    Rescale(&_rescale_tmp_5992._c0_poly, &_ckks_gen_tmp_3975._c0_poly);
    Rescale(&_rescale_tmp_5992._c1_poly, &_ckks_gen_tmp_3975._c1_poly);
    Pt_from_msg(&_pgen_tmp_305, combine_rc_index_n37 + 7134 /* cst_327_0 */, 16384, Sc_degree(&_rescale_tmp_5992), Level(&_rescale_tmp_5992));
    Init_ciph_up_scale_plain(&_pgen_tmp_306, &_rescale_tmp_5992, &_pgen_tmp_305);
    Init_ciph_same_scale(&comb_rc_result37, &comb_rc_result37, &_pgen_tmp_306);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&comb_rc_result37._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_306._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5992._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_305._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_306._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5992._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_305._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_rc_result37._c0_poly, _pgen_rns_idx, degree), Coeffs(&comb_rc_result37._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_306._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&comb_rc_result37._c1_poly, _pgen_rns_idx, degree), Coeffs(&comb_rc_result37._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_306._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435767._c1_poly);
    Free_poly_data(&_preg_268435767._c0_poly);
  }
  Free_poly_data(&comb_row_result37._c1_poly);
  Free_poly_data(&comb_row_result37._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_3975._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_3975._c0_poly);
  Free_poly_data(&_pgen_tmp_305._poly);
  Free_poly_data(&_rescale_tmp_5992._c1_poly);
  Free_poly_data(&_rescale_tmp_5992._c0_poly);
  Free_poly_data(&_pgen_tmp_306._c1_poly);
  Free_poly_data(&_pgen_tmp_306._c0_poly);
  memset(&final_result37, 0, sizeof(final_result37));
  for (combine_cc_index_n37 = 0; combine_cc_index_n37 < 64; combine_cc_index_n37 = combine_cc_index_n37 + 1) {
    _preg_268435768 = Rotate(comb_rc_result37, combine_cc_index_n37 * 192);
    Copy_ciph(&_ckks_gen_tmp_4006, &_preg_268435768);
    Init_ciph_down_scale(&_rescale_tmp_5998, &_ckks_gen_tmp_4006);
    Rescale(&_rescale_tmp_5998._c0_poly, &_ckks_gen_tmp_4006._c0_poly);
    Rescale(&_rescale_tmp_5998._c1_poly, &_ckks_gen_tmp_4006._c1_poly);
    Pt_from_msg(&_pgen_tmp_307, combine_cc_index_n37 + 7142 /* cst_328_0 */, 16384, Sc_degree(&_rescale_tmp_5998), Level(&_rescale_tmp_5998));
    Init_ciph_up_scale_plain(&_pgen_tmp_308, &_rescale_tmp_5998, &_pgen_tmp_307);
    Init_ciph_same_scale(&final_result37, &final_result37, &_pgen_tmp_308);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&final_result37._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_308._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5998._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_307._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_308._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5998._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_307._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&final_result37._c0_poly, _pgen_rns_idx, degree), Coeffs(&final_result37._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_308._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&final_result37._c1_poly, _pgen_rns_idx, degree), Coeffs(&final_result37._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_308._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435768._c1_poly);
    Free_poly_data(&_preg_268435768._c0_poly);
  }
  Free_poly_data(&comb_rc_result37._c1_poly);
  Free_poly_data(&comb_rc_result37._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_4006._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_4006._c0_poly);
  Free_poly_data(&_pgen_tmp_307._poly);
  Free_poly_data(&_rescale_tmp_5998._c1_poly);
  Free_poly_data(&_rescale_tmp_5998._c0_poly);
  Free_poly_data(&_pgen_tmp_308._c1_poly);
  Free_poly_data(&_pgen_tmp_308._c0_poly);
  Copy_ciph(&_preg_268435592, &final_result37);
  Free_poly_data(&final_result37._c1_poly);
  Free_poly_data(&final_result37._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::add");
  Init_ciph_same_scale(&_preg_268435593, &_preg_268435591, &_preg_268435592);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_preg_268435593._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_preg_268435593._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435591._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435592._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_preg_268435593._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435591._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435592._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435592._c1_poly);
    Free_poly_data(&_preg_268435592._c0_poly);
    Free_poly_data(&_preg_268435591._c1_poly);
    Free_poly_data(&_preg_268435591._c0_poly);
  }
  Tm_taken("Tensor::add");
  Init_ciph_down_scale(&_pgen_tmp_309, &_preg_268435593);
  Rescale(&_pgen_tmp_309._c0_poly, &_preg_268435593._c0_poly);
  Rescale(&_pgen_tmp_309._c1_poly, &_preg_268435593._c1_poly);
  Free_poly_data(&_preg_268435593._c1_poly);
  Free_poly_data(&_preg_268435593._c0_poly);
  Bootstrap(&_preg_268435594, &_pgen_tmp_309, 15);
  Free_poly_data(&_pgen_tmp_309._c1_poly);
  Free_poly_data(&_pgen_tmp_309._c0_poly);
  Encode_float(&_pgen_tmp_311, &_cst_475, 1, 1, Level(&_preg_268435594));
  Init_ciph_up_scale_plain(&_pgen_tmp_310, &_preg_268435594, &_pgen_tmp_311);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_310._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_310._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435594._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_311._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_310._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435594._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_311._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_311._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_312, &_pgen_tmp_310);
  Rescale(&_pgen_tmp_312._c0_poly, &_pgen_tmp_310._c0_poly);
  Rescale(&_pgen_tmp_312._c1_poly, &_pgen_tmp_310._c1_poly);
  Free_poly_data(&_pgen_tmp_310._c1_poly);
  Free_poly_data(&_pgen_tmp_310._c0_poly);
  _preg_268435595 = App_relu(_preg_268435594, _pgen_tmp_312);
  Free_poly_data(&_pgen_tmp_312._c1_poly);
  Free_poly_data(&_pgen_tmp_312._c0_poly);
  Free_poly_data(&_preg_268435594._c1_poly);
  Free_poly_data(&_preg_268435594._c0_poly);
  Copy_ciph(&_preg_268435596, &_preg_268435595);
  Free_poly_data(&_preg_268435595._c1_poly);
  Free_poly_data(&_preg_268435595._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n38, 0, sizeof(result_n38));
  Copy_ciph(&input_dup_n38, &_preg_268435596);
  for (index_khw1_n38 = 0; index_khw1_n38 < 9; index_khw1_n38 = index_khw1_n38 + 1) {
    _preg_268435769 = Rotate(input_dup_n38, _cst_334[index_khw1_n38]);
    Copy_ciph(&input_roll_n38[index_khw1_n38], &_preg_268435769);
    Free_poly_data(&_preg_268435769._c1_poly);
    Free_poly_data(&_preg_268435769._c0_poly);
  }
  Free_poly_data(&input_dup_n38._c1_poly);
  Free_poly_data(&input_dup_n38._c0_poly);
  for (index_cin_n38 = 0; index_cin_n38 < 64; index_cin_n38 = index_cin_n38 + 1) {
    memset(&result_cin_n38, 0, sizeof(result_cin_n38));
    for (index_khw2_n38 = 0; index_khw2_n38 < 9; index_khw2_n38 = index_khw2_n38 + 1) {
      _rescale_tmp_4087 = input_roll_n38[index_khw2_n38];
      Pt_from_msg(&_pgen_tmp_313, index_khw2_n38 + (index_cin_n38 * 9) + 7206 /* cst_332_0 */, 4096, Sc_degree(&_rescale_tmp_4087), Level(&_rescale_tmp_4087));
      Init_ciph_up_scale_plain(&_pgen_tmp_314, &_rescale_tmp_4087, &_pgen_tmp_313);
      Init_ciph_same_scale(&result_cin_n38, &result_cin_n38, &_pgen_tmp_314);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n38._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_314._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_4087._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_313._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_314._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_4087._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_313._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n38._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n38._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_314._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n38._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n38._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_314._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435770 = Rotate(result_cin_n38, -4096);
    Init_ciph_same_scale(&_pgen_tmp_315, &result_cin_n38, &_preg_268435770);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_315._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_315._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n38._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435770._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_315._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n38._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435770._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435771 = Rotate(_pgen_tmp_315, index_cin_n38 * 64);
    Init_ciph_same_scale(&result_n38, &result_n38, &_preg_268435771);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n38._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n38._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n38._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435771._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n38._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n38._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435771._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435770._c1_poly);
    Free_poly_data(&_preg_268435770._c0_poly);
    Free_poly_data(&_preg_268435771._c1_poly);
    Free_poly_data(&_preg_268435771._c0_poly);
  }
  Free_ciph_poly(input_roll_n38, 9);
  Free_poly_data(&_pgen_tmp_313._poly);
  Free_poly_data(&_pgen_tmp_314._c1_poly);
  Free_poly_data(&_pgen_tmp_314._c0_poly);
  Free_poly_data(&result_cin_n38._c1_poly);
  Free_poly_data(&result_cin_n38._c0_poly);
  Free_poly_data(&_pgen_tmp_315._c1_poly);
  Free_poly_data(&_pgen_tmp_315._c0_poly);
  Pt_from_msg(&_pgen_tmp_316, 7782 /* cst_333 */, 4096, Sc_degree(&result_n38), Level(&result_n38));
  Init_ciph_same_scale_plain(&result_n38, &result_n38, &_pgen_tmp_316);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n38._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n38._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n38._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_316._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n38._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n38._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_316._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_6011, &result_n38);
  Rescale(&_rescale_tmp_6011._c0_poly, &result_n38._c0_poly);
  Rescale(&_rescale_tmp_6011._c1_poly, &result_n38._c1_poly);
  Pt_from_msg(&_pgen_tmp_317, 7783 /* cst_335 */, 4096, Sc_degree(&_rescale_tmp_6011), Level(&_rescale_tmp_6011));
  Init_ciph_up_scale_plain(&result_n38, &_rescale_tmp_6011, &_pgen_tmp_317);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n38._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n38._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_6011._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_317._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n38._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_6011._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_317._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_317._poly);
    Free_poly_data(&_rescale_tmp_6011._c1_poly);
    Free_poly_data(&_rescale_tmp_6011._c0_poly);
  }
  Copy_ciph(&_preg_268435597, &result_n38);
  Free_poly_data(&result_n38._c1_poly);
  Free_poly_data(&result_n38._c0_poly);
  Tm_taken("Tensor::conv");
  Init_ciph_down_scale(&_pgen_tmp_318, &_preg_268435597);
  Rescale(&_pgen_tmp_318._c0_poly, &_preg_268435597._c0_poly);
  Rescale(&_pgen_tmp_318._c1_poly, &_preg_268435597._c1_poly);
  Free_poly_data(&_preg_268435597._c1_poly);
  Free_poly_data(&_preg_268435597._c0_poly);
  Bootstrap(&_preg_268435598, &_pgen_tmp_318, 15);
  Free_poly_data(&_pgen_tmp_318._c1_poly);
  Free_poly_data(&_pgen_tmp_318._c0_poly);
  Encode_float(&_pgen_tmp_320, &_cst_476, 1, 1, Level(&_preg_268435598));
  Init_ciph_up_scale_plain(&_pgen_tmp_319, &_preg_268435598, &_pgen_tmp_320);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_319._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_319._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435598._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_320._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_319._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435598._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_320._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_320._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_321, &_pgen_tmp_319);
  Rescale(&_pgen_tmp_321._c0_poly, &_pgen_tmp_319._c0_poly);
  Rescale(&_pgen_tmp_321._c1_poly, &_pgen_tmp_319._c1_poly);
  Free_poly_data(&_pgen_tmp_319._c1_poly);
  Free_poly_data(&_pgen_tmp_319._c0_poly);
  _preg_268435599 = App_relu(_preg_268435598, _pgen_tmp_321);
  Free_poly_data(&_pgen_tmp_321._c1_poly);
  Free_poly_data(&_pgen_tmp_321._c0_poly);
  Free_poly_data(&_preg_268435598._c1_poly);
  Free_poly_data(&_preg_268435598._c0_poly);
  Copy_ciph(&_preg_268435600, &_preg_268435599);
  Free_poly_data(&_preg_268435599._c1_poly);
  Free_poly_data(&_preg_268435599._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n39, 0, sizeof(result_n39));
  Copy_ciph(&input_dup_n39, &_preg_268435600);
  Free_poly_data(&_preg_268435600._c1_poly);
  Free_poly_data(&_preg_268435600._c0_poly);
  for (index_khw1_n39 = 0; index_khw1_n39 < 9; index_khw1_n39 = index_khw1_n39 + 1) {
    _preg_268435772 = Rotate(input_dup_n39, _cst_340[index_khw1_n39]);
    Copy_ciph(&input_roll_n39[index_khw1_n39], &_preg_268435772);
    Free_poly_data(&_preg_268435772._c1_poly);
    Free_poly_data(&_preg_268435772._c0_poly);
  }
  Free_poly_data(&input_dup_n39._c1_poly);
  Free_poly_data(&input_dup_n39._c0_poly);
  for (index_cin_n39 = 0; index_cin_n39 < 64; index_cin_n39 = index_cin_n39 + 1) {
    memset(&result_cin_n39, 0, sizeof(result_cin_n39));
    for (index_khw2_n39 = 0; index_khw2_n39 < 9; index_khw2_n39 = index_khw2_n39 + 1) {
      _rescale_tmp_4195 = input_roll_n39[index_khw2_n39];
      Pt_from_msg(&_pgen_tmp_322, index_khw2_n39 + (index_cin_n39 * 9) + 7784 /* cst_338_0 */, 4096, Sc_degree(&_rescale_tmp_4195), Level(&_rescale_tmp_4195));
      Init_ciph_up_scale_plain(&_pgen_tmp_323, &_rescale_tmp_4195, &_pgen_tmp_322);
      Init_ciph_same_scale(&result_cin_n39, &result_cin_n39, &_pgen_tmp_323);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n39._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_323._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_4195._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_322._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_323._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_4195._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_322._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n39._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n39._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_323._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n39._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n39._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_323._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435773 = Rotate(result_cin_n39, -4096);
    Init_ciph_same_scale(&_pgen_tmp_324, &result_cin_n39, &_preg_268435773);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_324._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_324._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n39._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435773._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_324._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n39._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435773._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435774 = Rotate(_pgen_tmp_324, index_cin_n39 * 64);
    Init_ciph_same_scale(&result_n39, &result_n39, &_preg_268435774);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n39._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n39._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n39._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435774._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n39._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n39._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435774._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435773._c1_poly);
    Free_poly_data(&_preg_268435773._c0_poly);
    Free_poly_data(&_preg_268435774._c1_poly);
    Free_poly_data(&_preg_268435774._c0_poly);
  }
  Free_ciph_poly(input_roll_n39, 9);
  Free_poly_data(&_pgen_tmp_322._poly);
  Free_poly_data(&_pgen_tmp_323._c1_poly);
  Free_poly_data(&_pgen_tmp_323._c0_poly);
  Free_poly_data(&result_cin_n39._c1_poly);
  Free_poly_data(&result_cin_n39._c0_poly);
  Free_poly_data(&_pgen_tmp_324._c1_poly);
  Free_poly_data(&_pgen_tmp_324._c0_poly);
  Pt_from_msg(&_pgen_tmp_325, 8360 /* cst_339 */, 4096, Sc_degree(&result_n39), Level(&result_n39));
  Init_ciph_same_scale_plain(&result_n39, &result_n39, &_pgen_tmp_325);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n39._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n39._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n39._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_325._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n39._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n39._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_325._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_6024, &result_n39);
  Rescale(&_rescale_tmp_6024._c0_poly, &result_n39._c0_poly);
  Rescale(&_rescale_tmp_6024._c1_poly, &result_n39._c1_poly);
  Pt_from_msg(&_pgen_tmp_326, 8361 /* cst_341 */, 4096, Sc_degree(&_rescale_tmp_6024), Level(&_rescale_tmp_6024));
  Init_ciph_up_scale_plain(&result_n39, &_rescale_tmp_6024, &_pgen_tmp_326);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n39._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n39._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_6024._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_326._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n39._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_6024._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_326._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_326._poly);
    Free_poly_data(&_rescale_tmp_6024._c1_poly);
    Free_poly_data(&_rescale_tmp_6024._c0_poly);
  }
  Copy_ciph(&_preg_268435601, &result_n39);
  Free_poly_data(&result_n39._c1_poly);
  Free_poly_data(&result_n39._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::add");
  Init_ciph_down_scale(&_pgen_tmp_327, &_preg_268435601);
  Rescale(&_pgen_tmp_327._c0_poly, &_preg_268435601._c0_poly);
  Rescale(&_pgen_tmp_327._c1_poly, &_preg_268435601._c1_poly);
  Free_poly_data(&_preg_268435601._c1_poly);
  Free_poly_data(&_preg_268435601._c0_poly);
  Init_ciph_same_scale(&_preg_268435602, &_pgen_tmp_327, &_preg_268435596);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_preg_268435602._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_preg_268435602._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_327._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435596._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_preg_268435602._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_327._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435596._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435596._c1_poly);
    Free_poly_data(&_preg_268435596._c0_poly);
    Free_poly_data(&_pgen_tmp_327._c1_poly);
    Free_poly_data(&_pgen_tmp_327._c0_poly);
  }
  Tm_taken("Tensor::add");
  Bootstrap(&_preg_268435603, &_preg_268435602, 15);
  Free_poly_data(&_preg_268435602._c1_poly);
  Free_poly_data(&_preg_268435602._c0_poly);
  Encode_float(&_pgen_tmp_329, &_cst_477, 1, 1, Level(&_preg_268435603));
  Init_ciph_up_scale_plain(&_pgen_tmp_328, &_preg_268435603, &_pgen_tmp_329);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_328._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_328._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435603._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_329._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_328._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435603._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_329._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_329._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_330, &_pgen_tmp_328);
  Rescale(&_pgen_tmp_330._c0_poly, &_pgen_tmp_328._c0_poly);
  Rescale(&_pgen_tmp_330._c1_poly, &_pgen_tmp_328._c1_poly);
  Free_poly_data(&_pgen_tmp_328._c1_poly);
  Free_poly_data(&_pgen_tmp_328._c0_poly);
  _preg_268435604 = App_relu(_preg_268435603, _pgen_tmp_330);
  Free_poly_data(&_pgen_tmp_330._c1_poly);
  Free_poly_data(&_pgen_tmp_330._c0_poly);
  Free_poly_data(&_preg_268435603._c1_poly);
  Free_poly_data(&_preg_268435603._c0_poly);
  Copy_ciph(&_preg_268435605, &_preg_268435604);
  Free_poly_data(&_preg_268435604._c1_poly);
  Free_poly_data(&_preg_268435604._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n40, 0, sizeof(result_n40));
  Copy_ciph(&input_dup_n40, &_preg_268435605);
  for (index_khw1_n40 = 0; index_khw1_n40 < 9; index_khw1_n40 = index_khw1_n40 + 1) {
    _preg_268435775 = Rotate(input_dup_n40, _cst_347[index_khw1_n40]);
    Copy_ciph(&input_roll_n40[index_khw1_n40], &_preg_268435775);
    Free_poly_data(&_preg_268435775._c1_poly);
    Free_poly_data(&_preg_268435775._c0_poly);
  }
  Free_poly_data(&input_dup_n40._c1_poly);
  Free_poly_data(&input_dup_n40._c0_poly);
  for (index_cin_n40 = 0; index_cin_n40 < 64; index_cin_n40 = index_cin_n40 + 1) {
    memset(&result_cin_n40, 0, sizeof(result_cin_n40));
    for (index_khw2_n40 = 0; index_khw2_n40 < 9; index_khw2_n40 = index_khw2_n40 + 1) {
      _rescale_tmp_4309 = input_roll_n40[index_khw2_n40];
      Pt_from_msg(&_pgen_tmp_331, index_khw2_n40 + (index_cin_n40 * 9) + 8362 /* cst_345_0 */, 4096, Sc_degree(&_rescale_tmp_4309), Level(&_rescale_tmp_4309));
      Init_ciph_up_scale_plain(&_pgen_tmp_332, &_rescale_tmp_4309, &_pgen_tmp_331);
      Init_ciph_same_scale(&result_cin_n40, &result_cin_n40, &_pgen_tmp_332);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n40._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_332._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_4309._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_331._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_332._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_4309._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_331._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n40._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n40._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_332._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n40._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n40._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_332._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435776 = Rotate(result_cin_n40, -4096);
    Init_ciph_same_scale(&_pgen_tmp_333, &result_cin_n40, &_preg_268435776);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_333._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_333._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n40._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435776._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_333._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n40._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435776._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435777 = Rotate(_pgen_tmp_333, index_cin_n40 * 64);
    Init_ciph_same_scale(&result_n40, &result_n40, &_preg_268435777);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n40._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n40._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n40._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435777._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n40._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n40._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435777._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435776._c1_poly);
    Free_poly_data(&_preg_268435776._c0_poly);
    Free_poly_data(&_preg_268435777._c1_poly);
    Free_poly_data(&_preg_268435777._c0_poly);
  }
  Free_ciph_poly(input_roll_n40, 9);
  Free_poly_data(&_pgen_tmp_331._poly);
  Free_poly_data(&_pgen_tmp_332._c1_poly);
  Free_poly_data(&_pgen_tmp_332._c0_poly);
  Free_poly_data(&result_cin_n40._c1_poly);
  Free_poly_data(&result_cin_n40._c0_poly);
  Free_poly_data(&_pgen_tmp_333._c1_poly);
  Free_poly_data(&_pgen_tmp_333._c0_poly);
  Pt_from_msg(&_pgen_tmp_334, 8938 /* cst_346 */, 4096, Sc_degree(&result_n40), Level(&result_n40));
  Init_ciph_same_scale_plain(&result_n40, &result_n40, &_pgen_tmp_334);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n40._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n40._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n40._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_334._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n40._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n40._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_334._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_6037, &result_n40);
  Rescale(&_rescale_tmp_6037._c0_poly, &result_n40._c0_poly);
  Rescale(&_rescale_tmp_6037._c1_poly, &result_n40._c1_poly);
  Pt_from_msg(&_pgen_tmp_335, 8939 /* cst_348 */, 4096, Sc_degree(&_rescale_tmp_6037), Level(&_rescale_tmp_6037));
  Init_ciph_up_scale_plain(&result_n40, &_rescale_tmp_6037, &_pgen_tmp_335);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n40._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n40._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_6037._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_335._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n40._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_6037._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_335._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_335._poly);
    Free_poly_data(&_rescale_tmp_6037._c1_poly);
    Free_poly_data(&_rescale_tmp_6037._c0_poly);
  }
  Copy_ciph(&_preg_268435606, &result_n40);
  Free_poly_data(&result_n40._c1_poly);
  Free_poly_data(&result_n40._c0_poly);
  Tm_taken("Tensor::conv");
  Init_ciph_down_scale(&_pgen_tmp_336, &_preg_268435606);
  Rescale(&_pgen_tmp_336._c0_poly, &_preg_268435606._c0_poly);
  Rescale(&_pgen_tmp_336._c1_poly, &_preg_268435606._c1_poly);
  Free_poly_data(&_preg_268435606._c1_poly);
  Free_poly_data(&_preg_268435606._c0_poly);
  Bootstrap(&_preg_268435607, &_pgen_tmp_336, 15);
  Free_poly_data(&_pgen_tmp_336._c1_poly);
  Free_poly_data(&_pgen_tmp_336._c0_poly);
  Encode_float(&_pgen_tmp_338, &_cst_478, 1, 1, Level(&_preg_268435607));
  Init_ciph_up_scale_plain(&_pgen_tmp_337, &_preg_268435607, &_pgen_tmp_338);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_337._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_337._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435607._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_338._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_337._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435607._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_338._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_338._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_339, &_pgen_tmp_337);
  Rescale(&_pgen_tmp_339._c0_poly, &_pgen_tmp_337._c0_poly);
  Rescale(&_pgen_tmp_339._c1_poly, &_pgen_tmp_337._c1_poly);
  Free_poly_data(&_pgen_tmp_337._c1_poly);
  Free_poly_data(&_pgen_tmp_337._c0_poly);
  _preg_268435608 = App_relu(_preg_268435607, _pgen_tmp_339);
  Free_poly_data(&_pgen_tmp_339._c1_poly);
  Free_poly_data(&_pgen_tmp_339._c0_poly);
  Free_poly_data(&_preg_268435607._c1_poly);
  Free_poly_data(&_preg_268435607._c0_poly);
  Copy_ciph(&_preg_268435609, &_preg_268435608);
  Free_poly_data(&_preg_268435608._c1_poly);
  Free_poly_data(&_preg_268435608._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n41, 0, sizeof(result_n41));
  Copy_ciph(&input_dup_n41, &_preg_268435609);
  Free_poly_data(&_preg_268435609._c1_poly);
  Free_poly_data(&_preg_268435609._c0_poly);
  for (index_khw1_n41 = 0; index_khw1_n41 < 9; index_khw1_n41 = index_khw1_n41 + 1) {
    _preg_268435778 = Rotate(input_dup_n41, _cst_353[index_khw1_n41]);
    Copy_ciph(&input_roll_n41[index_khw1_n41], &_preg_268435778);
    Free_poly_data(&_preg_268435778._c1_poly);
    Free_poly_data(&_preg_268435778._c0_poly);
  }
  Free_poly_data(&input_dup_n41._c1_poly);
  Free_poly_data(&input_dup_n41._c0_poly);
  for (index_cin_n41 = 0; index_cin_n41 < 64; index_cin_n41 = index_cin_n41 + 1) {
    memset(&result_cin_n41, 0, sizeof(result_cin_n41));
    for (index_khw2_n41 = 0; index_khw2_n41 < 9; index_khw2_n41 = index_khw2_n41 + 1) {
      _rescale_tmp_4417 = input_roll_n41[index_khw2_n41];
      Pt_from_msg(&_pgen_tmp_340, index_khw2_n41 + (index_cin_n41 * 9) + 8940 /* cst_351_0 */, 4096, Sc_degree(&_rescale_tmp_4417), Level(&_rescale_tmp_4417));
      Init_ciph_up_scale_plain(&_pgen_tmp_341, &_rescale_tmp_4417, &_pgen_tmp_340);
      Init_ciph_same_scale(&result_cin_n41, &result_cin_n41, &_pgen_tmp_341);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n41._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_341._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_4417._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_340._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_341._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_4417._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_340._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n41._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n41._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_341._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n41._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n41._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_341._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435779 = Rotate(result_cin_n41, -4096);
    Init_ciph_same_scale(&_pgen_tmp_342, &result_cin_n41, &_preg_268435779);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_342._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_342._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n41._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435779._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_342._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n41._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435779._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435780 = Rotate(_pgen_tmp_342, index_cin_n41 * 64);
    Init_ciph_same_scale(&result_n41, &result_n41, &_preg_268435780);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n41._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n41._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n41._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435780._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n41._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n41._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435780._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435779._c1_poly);
    Free_poly_data(&_preg_268435779._c0_poly);
    Free_poly_data(&_preg_268435780._c1_poly);
    Free_poly_data(&_preg_268435780._c0_poly);
  }
  Free_ciph_poly(input_roll_n41, 9);
  Free_poly_data(&_pgen_tmp_340._poly);
  Free_poly_data(&_pgen_tmp_341._c1_poly);
  Free_poly_data(&_pgen_tmp_341._c0_poly);
  Free_poly_data(&result_cin_n41._c1_poly);
  Free_poly_data(&result_cin_n41._c0_poly);
  Free_poly_data(&_pgen_tmp_342._c1_poly);
  Free_poly_data(&_pgen_tmp_342._c0_poly);
  Pt_from_msg(&_pgen_tmp_343, 9516 /* cst_352 */, 4096, Sc_degree(&result_n41), Level(&result_n41));
  Init_ciph_same_scale_plain(&result_n41, &result_n41, &_pgen_tmp_343);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n41._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n41._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n41._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_343._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n41._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n41._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_343._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_6050, &result_n41);
  Rescale(&_rescale_tmp_6050._c0_poly, &result_n41._c0_poly);
  Rescale(&_rescale_tmp_6050._c1_poly, &result_n41._c1_poly);
  Pt_from_msg(&_pgen_tmp_344, 9517 /* cst_354 */, 4096, Sc_degree(&_rescale_tmp_6050), Level(&_rescale_tmp_6050));
  Init_ciph_up_scale_plain(&result_n41, &_rescale_tmp_6050, &_pgen_tmp_344);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n41._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n41._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_6050._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_344._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n41._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_6050._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_344._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_344._poly);
    Free_poly_data(&_rescale_tmp_6050._c1_poly);
    Free_poly_data(&_rescale_tmp_6050._c0_poly);
  }
  Copy_ciph(&_preg_268435610, &result_n41);
  Free_poly_data(&result_n41._c1_poly);
  Free_poly_data(&result_n41._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::add");
  Init_ciph_down_scale(&_pgen_tmp_345, &_preg_268435610);
  Rescale(&_pgen_tmp_345._c0_poly, &_preg_268435610._c0_poly);
  Rescale(&_pgen_tmp_345._c1_poly, &_preg_268435610._c1_poly);
  Free_poly_data(&_preg_268435610._c1_poly);
  Free_poly_data(&_preg_268435610._c0_poly);
  Init_ciph_same_scale(&_preg_268435611, &_pgen_tmp_345, &_preg_268435605);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_preg_268435611._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_preg_268435611._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_345._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435605._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_preg_268435611._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_345._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435605._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435605._c1_poly);
    Free_poly_data(&_preg_268435605._c0_poly);
    Free_poly_data(&_pgen_tmp_345._c1_poly);
    Free_poly_data(&_pgen_tmp_345._c0_poly);
  }
  Tm_taken("Tensor::add");
  Bootstrap(&_preg_268435612, &_preg_268435611, 15);
  Free_poly_data(&_preg_268435611._c1_poly);
  Free_poly_data(&_preg_268435611._c0_poly);
  Encode_float(&_pgen_tmp_347, &_cst_479, 1, 1, Level(&_preg_268435612));
  Init_ciph_up_scale_plain(&_pgen_tmp_346, &_preg_268435612, &_pgen_tmp_347);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_346._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_346._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435612._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_347._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_346._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435612._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_347._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_347._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_348, &_pgen_tmp_346);
  Rescale(&_pgen_tmp_348._c0_poly, &_pgen_tmp_346._c0_poly);
  Rescale(&_pgen_tmp_348._c1_poly, &_pgen_tmp_346._c1_poly);
  Free_poly_data(&_pgen_tmp_346._c1_poly);
  Free_poly_data(&_pgen_tmp_346._c0_poly);
  _preg_268435613 = App_relu(_preg_268435612, _pgen_tmp_348);
  Free_poly_data(&_pgen_tmp_348._c1_poly);
  Free_poly_data(&_pgen_tmp_348._c0_poly);
  Free_poly_data(&_preg_268435612._c1_poly);
  Free_poly_data(&_preg_268435612._c0_poly);
  Copy_ciph(&_preg_268435614, &_preg_268435613);
  Free_poly_data(&_preg_268435613._c1_poly);
  Free_poly_data(&_preg_268435613._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n42, 0, sizeof(result_n42));
  Copy_ciph(&input_dup_n42, &_preg_268435614);
  for (index_khw1_n42 = 0; index_khw1_n42 < 9; index_khw1_n42 = index_khw1_n42 + 1) {
    _preg_268435781 = Rotate(input_dup_n42, _cst_360[index_khw1_n42]);
    Copy_ciph(&input_roll_n42[index_khw1_n42], &_preg_268435781);
    Free_poly_data(&_preg_268435781._c1_poly);
    Free_poly_data(&_preg_268435781._c0_poly);
  }
  Free_poly_data(&input_dup_n42._c1_poly);
  Free_poly_data(&input_dup_n42._c0_poly);
  for (index_cin_n42 = 0; index_cin_n42 < 64; index_cin_n42 = index_cin_n42 + 1) {
    memset(&result_cin_n42, 0, sizeof(result_cin_n42));
    for (index_khw2_n42 = 0; index_khw2_n42 < 9; index_khw2_n42 = index_khw2_n42 + 1) {
      _rescale_tmp_4531 = input_roll_n42[index_khw2_n42];
      Pt_from_msg(&_pgen_tmp_349, index_khw2_n42 + (index_cin_n42 * 9) + 9518 /* cst_358_0 */, 4096, Sc_degree(&_rescale_tmp_4531), Level(&_rescale_tmp_4531));
      Init_ciph_up_scale_plain(&_pgen_tmp_350, &_rescale_tmp_4531, &_pgen_tmp_349);
      Init_ciph_same_scale(&result_cin_n42, &result_cin_n42, &_pgen_tmp_350);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n42._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_350._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_4531._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_349._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_350._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_4531._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_349._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n42._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n42._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_350._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n42._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n42._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_350._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435782 = Rotate(result_cin_n42, -4096);
    Init_ciph_same_scale(&_pgen_tmp_351, &result_cin_n42, &_preg_268435782);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_351._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_351._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n42._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435782._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_351._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n42._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435782._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435783 = Rotate(_pgen_tmp_351, index_cin_n42 * 64);
    Init_ciph_same_scale(&result_n42, &result_n42, &_preg_268435783);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n42._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n42._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n42._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435783._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n42._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n42._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435783._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435782._c1_poly);
    Free_poly_data(&_preg_268435782._c0_poly);
    Free_poly_data(&_preg_268435783._c1_poly);
    Free_poly_data(&_preg_268435783._c0_poly);
  }
  Free_ciph_poly(input_roll_n42, 9);
  Free_poly_data(&_pgen_tmp_349._poly);
  Free_poly_data(&_pgen_tmp_350._c1_poly);
  Free_poly_data(&_pgen_tmp_350._c0_poly);
  Free_poly_data(&result_cin_n42._c1_poly);
  Free_poly_data(&result_cin_n42._c0_poly);
  Free_poly_data(&_pgen_tmp_351._c1_poly);
  Free_poly_data(&_pgen_tmp_351._c0_poly);
  Pt_from_msg(&_pgen_tmp_352, 10094 /* cst_359 */, 4096, Sc_degree(&result_n42), Level(&result_n42));
  Init_ciph_same_scale_plain(&result_n42, &result_n42, &_pgen_tmp_352);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n42._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n42._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n42._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_352._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n42._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n42._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_352._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_6063, &result_n42);
  Rescale(&_rescale_tmp_6063._c0_poly, &result_n42._c0_poly);
  Rescale(&_rescale_tmp_6063._c1_poly, &result_n42._c1_poly);
  Pt_from_msg(&_pgen_tmp_353, 10095 /* cst_361 */, 4096, Sc_degree(&_rescale_tmp_6063), Level(&_rescale_tmp_6063));
  Init_ciph_up_scale_plain(&result_n42, &_rescale_tmp_6063, &_pgen_tmp_353);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n42._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n42._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_6063._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_353._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n42._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_6063._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_353._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_353._poly);
    Free_poly_data(&_rescale_tmp_6063._c1_poly);
    Free_poly_data(&_rescale_tmp_6063._c0_poly);
  }
  Copy_ciph(&_preg_268435615, &result_n42);
  Free_poly_data(&result_n42._c1_poly);
  Free_poly_data(&result_n42._c0_poly);
  Tm_taken("Tensor::conv");
  Init_ciph_down_scale(&_pgen_tmp_354, &_preg_268435615);
  Rescale(&_pgen_tmp_354._c0_poly, &_preg_268435615._c0_poly);
  Rescale(&_pgen_tmp_354._c1_poly, &_preg_268435615._c1_poly);
  Free_poly_data(&_preg_268435615._c1_poly);
  Free_poly_data(&_preg_268435615._c0_poly);
  Bootstrap(&_preg_268435616, &_pgen_tmp_354, 15);
  Free_poly_data(&_pgen_tmp_354._c1_poly);
  Free_poly_data(&_pgen_tmp_354._c0_poly);
  Encode_float(&_pgen_tmp_356, &_cst_480, 1, 1, Level(&_preg_268435616));
  Init_ciph_up_scale_plain(&_pgen_tmp_355, &_preg_268435616, &_pgen_tmp_356);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_355._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_355._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435616._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_356._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_355._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435616._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_356._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_356._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_357, &_pgen_tmp_355);
  Rescale(&_pgen_tmp_357._c0_poly, &_pgen_tmp_355._c0_poly);
  Rescale(&_pgen_tmp_357._c1_poly, &_pgen_tmp_355._c1_poly);
  Free_poly_data(&_pgen_tmp_355._c1_poly);
  Free_poly_data(&_pgen_tmp_355._c0_poly);
  _preg_268435617 = App_relu(_preg_268435616, _pgen_tmp_357);
  Free_poly_data(&_pgen_tmp_357._c1_poly);
  Free_poly_data(&_pgen_tmp_357._c0_poly);
  Free_poly_data(&_preg_268435616._c1_poly);
  Free_poly_data(&_preg_268435616._c0_poly);
  Copy_ciph(&_preg_268435618, &_preg_268435617);
  Free_poly_data(&_preg_268435617._c1_poly);
  Free_poly_data(&_preg_268435617._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n43, 0, sizeof(result_n43));
  Copy_ciph(&input_dup_n43, &_preg_268435618);
  Free_poly_data(&_preg_268435618._c1_poly);
  Free_poly_data(&_preg_268435618._c0_poly);
  for (index_khw1_n43 = 0; index_khw1_n43 < 9; index_khw1_n43 = index_khw1_n43 + 1) {
    _preg_268435784 = Rotate(input_dup_n43, _cst_366[index_khw1_n43]);
    Copy_ciph(&input_roll_n43[index_khw1_n43], &_preg_268435784);
    Free_poly_data(&_preg_268435784._c1_poly);
    Free_poly_data(&_preg_268435784._c0_poly);
  }
  Free_poly_data(&input_dup_n43._c1_poly);
  Free_poly_data(&input_dup_n43._c0_poly);
  for (index_cin_n43 = 0; index_cin_n43 < 64; index_cin_n43 = index_cin_n43 + 1) {
    memset(&result_cin_n43, 0, sizeof(result_cin_n43));
    for (index_khw2_n43 = 0; index_khw2_n43 < 9; index_khw2_n43 = index_khw2_n43 + 1) {
      _rescale_tmp_4639 = input_roll_n43[index_khw2_n43];
      Pt_from_msg(&_pgen_tmp_358, index_khw2_n43 + (index_cin_n43 * 9) + 10096 /* cst_364_0 */, 4096, Sc_degree(&_rescale_tmp_4639), Level(&_rescale_tmp_4639));
      Init_ciph_up_scale_plain(&_pgen_tmp_359, &_rescale_tmp_4639, &_pgen_tmp_358);
      Init_ciph_same_scale(&result_cin_n43, &result_cin_n43, &_pgen_tmp_359);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n43._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_359._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_4639._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_358._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_359._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_4639._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_358._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n43._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n43._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_359._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n43._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n43._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_359._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435785 = Rotate(result_cin_n43, -4096);
    Init_ciph_same_scale(&_pgen_tmp_360, &result_cin_n43, &_preg_268435785);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_360._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_360._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n43._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435785._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_360._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n43._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435785._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435786 = Rotate(_pgen_tmp_360, index_cin_n43 * 64);
    Init_ciph_same_scale(&result_n43, &result_n43, &_preg_268435786);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n43._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n43._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n43._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435786._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n43._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n43._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435786._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435785._c1_poly);
    Free_poly_data(&_preg_268435785._c0_poly);
    Free_poly_data(&_preg_268435786._c1_poly);
    Free_poly_data(&_preg_268435786._c0_poly);
  }
  Free_ciph_poly(input_roll_n43, 9);
  Free_poly_data(&_pgen_tmp_358._poly);
  Free_poly_data(&_pgen_tmp_359._c1_poly);
  Free_poly_data(&_pgen_tmp_359._c0_poly);
  Free_poly_data(&result_cin_n43._c1_poly);
  Free_poly_data(&result_cin_n43._c0_poly);
  Free_poly_data(&_pgen_tmp_360._c1_poly);
  Free_poly_data(&_pgen_tmp_360._c0_poly);
  Pt_from_msg(&_pgen_tmp_361, 10672 /* cst_365 */, 4096, Sc_degree(&result_n43), Level(&result_n43));
  Init_ciph_same_scale_plain(&result_n43, &result_n43, &_pgen_tmp_361);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n43._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n43._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n43._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_361._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n43._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n43._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_361._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_6076, &result_n43);
  Rescale(&_rescale_tmp_6076._c0_poly, &result_n43._c0_poly);
  Rescale(&_rescale_tmp_6076._c1_poly, &result_n43._c1_poly);
  Pt_from_msg(&_pgen_tmp_362, 10673 /* cst_367 */, 4096, Sc_degree(&_rescale_tmp_6076), Level(&_rescale_tmp_6076));
  Init_ciph_up_scale_plain(&result_n43, &_rescale_tmp_6076, &_pgen_tmp_362);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n43._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n43._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_6076._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_362._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n43._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_6076._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_362._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_362._poly);
    Free_poly_data(&_rescale_tmp_6076._c1_poly);
    Free_poly_data(&_rescale_tmp_6076._c0_poly);
  }
  Copy_ciph(&_preg_268435619, &result_n43);
  Free_poly_data(&result_n43._c1_poly);
  Free_poly_data(&result_n43._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::add");
  Init_ciph_down_scale(&_pgen_tmp_363, &_preg_268435619);
  Rescale(&_pgen_tmp_363._c0_poly, &_preg_268435619._c0_poly);
  Rescale(&_pgen_tmp_363._c1_poly, &_preg_268435619._c1_poly);
  Free_poly_data(&_preg_268435619._c1_poly);
  Free_poly_data(&_preg_268435619._c0_poly);
  Init_ciph_same_scale(&_preg_268435620, &_pgen_tmp_363, &_preg_268435614);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_preg_268435620._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_preg_268435620._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_363._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435614._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_preg_268435620._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_363._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435614._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435614._c1_poly);
    Free_poly_data(&_preg_268435614._c0_poly);
    Free_poly_data(&_pgen_tmp_363._c1_poly);
    Free_poly_data(&_pgen_tmp_363._c0_poly);
  }
  Tm_taken("Tensor::add");
  Bootstrap(&_preg_268435621, &_preg_268435620, 15);
  Free_poly_data(&_preg_268435620._c1_poly);
  Free_poly_data(&_preg_268435620._c0_poly);
  Encode_float(&_pgen_tmp_365, &_cst_481, 1, 1, Level(&_preg_268435621));
  Init_ciph_up_scale_plain(&_pgen_tmp_364, &_preg_268435621, &_pgen_tmp_365);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_364._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_364._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435621._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_365._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_364._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435621._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_365._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_365._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_366, &_pgen_tmp_364);
  Rescale(&_pgen_tmp_366._c0_poly, &_pgen_tmp_364._c0_poly);
  Rescale(&_pgen_tmp_366._c1_poly, &_pgen_tmp_364._c1_poly);
  Free_poly_data(&_pgen_tmp_364._c1_poly);
  Free_poly_data(&_pgen_tmp_364._c0_poly);
  _preg_268435622 = App_relu(_preg_268435621, _pgen_tmp_366);
  Free_poly_data(&_pgen_tmp_366._c1_poly);
  Free_poly_data(&_pgen_tmp_366._c0_poly);
  Free_poly_data(&_preg_268435621._c1_poly);
  Free_poly_data(&_preg_268435621._c0_poly);
  Copy_ciph(&_preg_268435623, &_preg_268435622);
  Free_poly_data(&_preg_268435622._c1_poly);
  Free_poly_data(&_preg_268435622._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n44, 0, sizeof(result_n44));
  Copy_ciph(&input_dup_n44, &_preg_268435623);
  for (index_khw1_n44 = 0; index_khw1_n44 < 9; index_khw1_n44 = index_khw1_n44 + 1) {
    _preg_268435787 = Rotate(input_dup_n44, _cst_373[index_khw1_n44]);
    Copy_ciph(&input_roll_n44[index_khw1_n44], &_preg_268435787);
    Free_poly_data(&_preg_268435787._c1_poly);
    Free_poly_data(&_preg_268435787._c0_poly);
  }
  Free_poly_data(&input_dup_n44._c1_poly);
  Free_poly_data(&input_dup_n44._c0_poly);
  for (index_cin_n44 = 0; index_cin_n44 < 64; index_cin_n44 = index_cin_n44 + 1) {
    memset(&result_cin_n44, 0, sizeof(result_cin_n44));
    for (index_khw2_n44 = 0; index_khw2_n44 < 9; index_khw2_n44 = index_khw2_n44 + 1) {
      _rescale_tmp_4753 = input_roll_n44[index_khw2_n44];
      Pt_from_msg(&_pgen_tmp_367, index_khw2_n44 + (index_cin_n44 * 9) + 10674 /* cst_371_0 */, 4096, Sc_degree(&_rescale_tmp_4753), Level(&_rescale_tmp_4753));
      Init_ciph_up_scale_plain(&_pgen_tmp_368, &_rescale_tmp_4753, &_pgen_tmp_367);
      Init_ciph_same_scale(&result_cin_n44, &result_cin_n44, &_pgen_tmp_368);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n44._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_368._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_4753._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_367._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_368._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_4753._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_367._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n44._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n44._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_368._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n44._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n44._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_368._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435788 = Rotate(result_cin_n44, -4096);
    Init_ciph_same_scale(&_pgen_tmp_369, &result_cin_n44, &_preg_268435788);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_369._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_369._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n44._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435788._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_369._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n44._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435788._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435789 = Rotate(_pgen_tmp_369, index_cin_n44 * 64);
    Init_ciph_same_scale(&result_n44, &result_n44, &_preg_268435789);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n44._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n44._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n44._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435789._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n44._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n44._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435789._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435788._c1_poly);
    Free_poly_data(&_preg_268435788._c0_poly);
    Free_poly_data(&_preg_268435789._c1_poly);
    Free_poly_data(&_preg_268435789._c0_poly);
  }
  Free_ciph_poly(input_roll_n44, 9);
  Free_poly_data(&_pgen_tmp_367._poly);
  Free_poly_data(&_pgen_tmp_368._c1_poly);
  Free_poly_data(&_pgen_tmp_368._c0_poly);
  Free_poly_data(&result_cin_n44._c1_poly);
  Free_poly_data(&result_cin_n44._c0_poly);
  Free_poly_data(&_pgen_tmp_369._c1_poly);
  Free_poly_data(&_pgen_tmp_369._c0_poly);
  Pt_from_msg(&_pgen_tmp_370, 11250 /* cst_372 */, 4096, Sc_degree(&result_n44), Level(&result_n44));
  Init_ciph_same_scale_plain(&result_n44, &result_n44, &_pgen_tmp_370);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n44._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n44._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n44._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_370._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n44._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n44._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_370._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_6089, &result_n44);
  Rescale(&_rescale_tmp_6089._c0_poly, &result_n44._c0_poly);
  Rescale(&_rescale_tmp_6089._c1_poly, &result_n44._c1_poly);
  Pt_from_msg(&_pgen_tmp_371, 11251 /* cst_374 */, 4096, Sc_degree(&_rescale_tmp_6089), Level(&_rescale_tmp_6089));
  Init_ciph_up_scale_plain(&result_n44, &_rescale_tmp_6089, &_pgen_tmp_371);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n44._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n44._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_6089._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_371._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n44._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_6089._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_371._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_371._poly);
    Free_poly_data(&_rescale_tmp_6089._c1_poly);
    Free_poly_data(&_rescale_tmp_6089._c0_poly);
  }
  Copy_ciph(&_preg_268435624, &result_n44);
  Free_poly_data(&result_n44._c1_poly);
  Free_poly_data(&result_n44._c0_poly);
  Tm_taken("Tensor::conv");
  Init_ciph_down_scale(&_pgen_tmp_372, &_preg_268435624);
  Rescale(&_pgen_tmp_372._c0_poly, &_preg_268435624._c0_poly);
  Rescale(&_pgen_tmp_372._c1_poly, &_preg_268435624._c1_poly);
  Free_poly_data(&_preg_268435624._c1_poly);
  Free_poly_data(&_preg_268435624._c0_poly);
  Bootstrap(&_preg_268435625, &_pgen_tmp_372, 15);
  Free_poly_data(&_pgen_tmp_372._c1_poly);
  Free_poly_data(&_pgen_tmp_372._c0_poly);
  Encode_float(&_pgen_tmp_374, &_cst_482, 1, 1, Level(&_preg_268435625));
  Init_ciph_up_scale_plain(&_pgen_tmp_373, &_preg_268435625, &_pgen_tmp_374);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_373._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_373._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435625._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_374._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_373._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435625._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_374._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_374._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_375, &_pgen_tmp_373);
  Rescale(&_pgen_tmp_375._c0_poly, &_pgen_tmp_373._c0_poly);
  Rescale(&_pgen_tmp_375._c1_poly, &_pgen_tmp_373._c1_poly);
  Free_poly_data(&_pgen_tmp_373._c1_poly);
  Free_poly_data(&_pgen_tmp_373._c0_poly);
  _preg_268435626 = App_relu(_preg_268435625, _pgen_tmp_375);
  Free_poly_data(&_pgen_tmp_375._c1_poly);
  Free_poly_data(&_pgen_tmp_375._c0_poly);
  Free_poly_data(&_preg_268435625._c1_poly);
  Free_poly_data(&_preg_268435625._c0_poly);
  Copy_ciph(&_preg_268435627, &_preg_268435626);
  Free_poly_data(&_preg_268435626._c1_poly);
  Free_poly_data(&_preg_268435626._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n45, 0, sizeof(result_n45));
  Copy_ciph(&input_dup_n45, &_preg_268435627);
  Free_poly_data(&_preg_268435627._c1_poly);
  Free_poly_data(&_preg_268435627._c0_poly);
  for (index_khw1_n45 = 0; index_khw1_n45 < 9; index_khw1_n45 = index_khw1_n45 + 1) {
    _preg_268435790 = Rotate(input_dup_n45, _cst_379[index_khw1_n45]);
    Copy_ciph(&input_roll_n45[index_khw1_n45], &_preg_268435790);
    Free_poly_data(&_preg_268435790._c1_poly);
    Free_poly_data(&_preg_268435790._c0_poly);
  }
  Free_poly_data(&input_dup_n45._c1_poly);
  Free_poly_data(&input_dup_n45._c0_poly);
  for (index_cin_n45 = 0; index_cin_n45 < 64; index_cin_n45 = index_cin_n45 + 1) {
    memset(&result_cin_n45, 0, sizeof(result_cin_n45));
    for (index_khw2_n45 = 0; index_khw2_n45 < 9; index_khw2_n45 = index_khw2_n45 + 1) {
      _rescale_tmp_4861 = input_roll_n45[index_khw2_n45];
      Pt_from_msg(&_pgen_tmp_376, index_khw2_n45 + (index_cin_n45 * 9) + 11252 /* cst_377_0 */, 4096, Sc_degree(&_rescale_tmp_4861), Level(&_rescale_tmp_4861));
      Init_ciph_up_scale_plain(&_pgen_tmp_377, &_rescale_tmp_4861, &_pgen_tmp_376);
      Init_ciph_same_scale(&result_cin_n45, &result_cin_n45, &_pgen_tmp_377);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n45._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_377._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_4861._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_376._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_377._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_4861._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_376._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n45._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n45._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_377._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n45._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n45._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_377._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435791 = Rotate(result_cin_n45, -4096);
    Init_ciph_same_scale(&_pgen_tmp_378, &result_cin_n45, &_preg_268435791);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_378._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_378._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n45._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435791._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_378._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n45._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435791._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435792 = Rotate(_pgen_tmp_378, index_cin_n45 * 64);
    Init_ciph_same_scale(&result_n45, &result_n45, &_preg_268435792);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n45._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n45._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n45._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435792._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n45._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n45._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435792._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435791._c1_poly);
    Free_poly_data(&_preg_268435791._c0_poly);
    Free_poly_data(&_preg_268435792._c1_poly);
    Free_poly_data(&_preg_268435792._c0_poly);
  }
  Free_ciph_poly(input_roll_n45, 9);
  Free_poly_data(&_pgen_tmp_376._poly);
  Free_poly_data(&_pgen_tmp_377._c1_poly);
  Free_poly_data(&_pgen_tmp_377._c0_poly);
  Free_poly_data(&result_cin_n45._c1_poly);
  Free_poly_data(&result_cin_n45._c0_poly);
  Free_poly_data(&_pgen_tmp_378._c1_poly);
  Free_poly_data(&_pgen_tmp_378._c0_poly);
  Pt_from_msg(&_pgen_tmp_379, 11828 /* cst_378 */, 4096, Sc_degree(&result_n45), Level(&result_n45));
  Init_ciph_same_scale_plain(&result_n45, &result_n45, &_pgen_tmp_379);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n45._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n45._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n45._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_379._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n45._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n45._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_379._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_6102, &result_n45);
  Rescale(&_rescale_tmp_6102._c0_poly, &result_n45._c0_poly);
  Rescale(&_rescale_tmp_6102._c1_poly, &result_n45._c1_poly);
  Pt_from_msg(&_pgen_tmp_380, 11829 /* cst_380 */, 4096, Sc_degree(&_rescale_tmp_6102), Level(&_rescale_tmp_6102));
  Init_ciph_up_scale_plain(&result_n45, &_rescale_tmp_6102, &_pgen_tmp_380);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n45._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n45._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_6102._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_380._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n45._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_6102._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_380._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_380._poly);
    Free_poly_data(&_rescale_tmp_6102._c1_poly);
    Free_poly_data(&_rescale_tmp_6102._c0_poly);
  }
  Copy_ciph(&_preg_268435628, &result_n45);
  Free_poly_data(&result_n45._c1_poly);
  Free_poly_data(&result_n45._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::add");
  Init_ciph_down_scale(&_pgen_tmp_381, &_preg_268435628);
  Rescale(&_pgen_tmp_381._c0_poly, &_preg_268435628._c0_poly);
  Rescale(&_pgen_tmp_381._c1_poly, &_preg_268435628._c1_poly);
  Free_poly_data(&_preg_268435628._c1_poly);
  Free_poly_data(&_preg_268435628._c0_poly);
  Init_ciph_same_scale(&_preg_268435629, &_pgen_tmp_381, &_preg_268435623);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_preg_268435629._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_preg_268435629._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_381._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435623._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_preg_268435629._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_381._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435623._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435623._c1_poly);
    Free_poly_data(&_preg_268435623._c0_poly);
    Free_poly_data(&_pgen_tmp_381._c1_poly);
    Free_poly_data(&_pgen_tmp_381._c0_poly);
  }
  Tm_taken("Tensor::add");
  Bootstrap(&_preg_268435630, &_preg_268435629, 15);
  Free_poly_data(&_preg_268435629._c1_poly);
  Free_poly_data(&_preg_268435629._c0_poly);
  Encode_float(&_pgen_tmp_383, &_cst_483, 1, 1, Level(&_preg_268435630));
  Init_ciph_up_scale_plain(&_pgen_tmp_382, &_preg_268435630, &_pgen_tmp_383);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_382._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_382._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435630._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_383._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_382._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435630._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_383._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_383._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_384, &_pgen_tmp_382);
  Rescale(&_pgen_tmp_384._c0_poly, &_pgen_tmp_382._c0_poly);
  Rescale(&_pgen_tmp_384._c1_poly, &_pgen_tmp_382._c1_poly);
  Free_poly_data(&_pgen_tmp_382._c1_poly);
  Free_poly_data(&_pgen_tmp_382._c0_poly);
  _preg_268435631 = App_relu(_preg_268435630, _pgen_tmp_384);
  Free_poly_data(&_pgen_tmp_384._c1_poly);
  Free_poly_data(&_pgen_tmp_384._c0_poly);
  Free_poly_data(&_preg_268435630._c1_poly);
  Free_poly_data(&_preg_268435630._c0_poly);
  Copy_ciph(&_preg_268435632, &_preg_268435631);
  Free_poly_data(&_preg_268435631._c1_poly);
  Free_poly_data(&_preg_268435631._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n46, 0, sizeof(result_n46));
  Copy_ciph(&input_dup_n46, &_preg_268435632);
  for (index_khw1_n46 = 0; index_khw1_n46 < 9; index_khw1_n46 = index_khw1_n46 + 1) {
    _preg_268435793 = Rotate(input_dup_n46, _cst_386[index_khw1_n46]);
    Copy_ciph(&input_roll_n46[index_khw1_n46], &_preg_268435793);
    Free_poly_data(&_preg_268435793._c1_poly);
    Free_poly_data(&_preg_268435793._c0_poly);
  }
  Free_poly_data(&input_dup_n46._c1_poly);
  Free_poly_data(&input_dup_n46._c0_poly);
  for (index_cin_n46 = 0; index_cin_n46 < 64; index_cin_n46 = index_cin_n46 + 1) {
    memset(&result_cin_n46, 0, sizeof(result_cin_n46));
    for (index_khw2_n46 = 0; index_khw2_n46 < 9; index_khw2_n46 = index_khw2_n46 + 1) {
      _rescale_tmp_4975 = input_roll_n46[index_khw2_n46];
      Pt_from_msg(&_pgen_tmp_385, index_khw2_n46 + (index_cin_n46 * 9) + 11830 /* cst_384_0 */, 4096, Sc_degree(&_rescale_tmp_4975), Level(&_rescale_tmp_4975));
      Init_ciph_up_scale_plain(&_pgen_tmp_386, &_rescale_tmp_4975, &_pgen_tmp_385);
      Init_ciph_same_scale(&result_cin_n46, &result_cin_n46, &_pgen_tmp_386);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n46._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_386._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_4975._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_385._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_386._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_4975._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_385._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n46._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n46._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_386._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n46._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n46._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_386._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435794 = Rotate(result_cin_n46, -4096);
    Init_ciph_same_scale(&_pgen_tmp_387, &result_cin_n46, &_preg_268435794);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_387._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_387._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n46._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435794._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_387._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n46._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435794._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435795 = Rotate(_pgen_tmp_387, index_cin_n46 * 64);
    Init_ciph_same_scale(&result_n46, &result_n46, &_preg_268435795);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n46._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n46._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n46._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435795._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n46._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n46._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435795._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435794._c1_poly);
    Free_poly_data(&_preg_268435794._c0_poly);
    Free_poly_data(&_preg_268435795._c1_poly);
    Free_poly_data(&_preg_268435795._c0_poly);
  }
  Free_ciph_poly(input_roll_n46, 9);
  Free_poly_data(&_pgen_tmp_385._poly);
  Free_poly_data(&_pgen_tmp_386._c1_poly);
  Free_poly_data(&_pgen_tmp_386._c0_poly);
  Free_poly_data(&result_cin_n46._c1_poly);
  Free_poly_data(&result_cin_n46._c0_poly);
  Free_poly_data(&_pgen_tmp_387._c1_poly);
  Free_poly_data(&_pgen_tmp_387._c0_poly);
  Pt_from_msg(&_pgen_tmp_388, 12406 /* cst_385 */, 4096, Sc_degree(&result_n46), Level(&result_n46));
  Init_ciph_same_scale_plain(&result_n46, &result_n46, &_pgen_tmp_388);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n46._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n46._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n46._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_388._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n46._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n46._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_388._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_6115, &result_n46);
  Rescale(&_rescale_tmp_6115._c0_poly, &result_n46._c0_poly);
  Rescale(&_rescale_tmp_6115._c1_poly, &result_n46._c1_poly);
  Pt_from_msg(&_pgen_tmp_389, 12407 /* cst_387 */, 4096, Sc_degree(&_rescale_tmp_6115), Level(&_rescale_tmp_6115));
  Init_ciph_up_scale_plain(&result_n46, &_rescale_tmp_6115, &_pgen_tmp_389);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n46._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n46._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_6115._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_389._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n46._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_6115._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_389._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_389._poly);
    Free_poly_data(&_rescale_tmp_6115._c1_poly);
    Free_poly_data(&_rescale_tmp_6115._c0_poly);
  }
  Copy_ciph(&_preg_268435633, &result_n46);
  Free_poly_data(&result_n46._c1_poly);
  Free_poly_data(&result_n46._c0_poly);
  Tm_taken("Tensor::conv");
  Init_ciph_down_scale(&_pgen_tmp_390, &_preg_268435633);
  Rescale(&_pgen_tmp_390._c0_poly, &_preg_268435633._c0_poly);
  Rescale(&_pgen_tmp_390._c1_poly, &_preg_268435633._c1_poly);
  Free_poly_data(&_preg_268435633._c1_poly);
  Free_poly_data(&_preg_268435633._c0_poly);
  Bootstrap(&_preg_268435634, &_pgen_tmp_390, 15);
  Free_poly_data(&_pgen_tmp_390._c1_poly);
  Free_poly_data(&_pgen_tmp_390._c0_poly);
  Encode_float(&_pgen_tmp_392, &_cst_484, 1, 1, Level(&_preg_268435634));
  Init_ciph_up_scale_plain(&_pgen_tmp_391, &_preg_268435634, &_pgen_tmp_392);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_391._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_391._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435634._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_392._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_391._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435634._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_392._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_392._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_393, &_pgen_tmp_391);
  Rescale(&_pgen_tmp_393._c0_poly, &_pgen_tmp_391._c0_poly);
  Rescale(&_pgen_tmp_393._c1_poly, &_pgen_tmp_391._c1_poly);
  Free_poly_data(&_pgen_tmp_391._c1_poly);
  Free_poly_data(&_pgen_tmp_391._c0_poly);
  _preg_268435635 = App_relu(_preg_268435634, _pgen_tmp_393);
  Free_poly_data(&_pgen_tmp_393._c1_poly);
  Free_poly_data(&_pgen_tmp_393._c0_poly);
  Free_poly_data(&_preg_268435634._c1_poly);
  Free_poly_data(&_preg_268435634._c0_poly);
  Copy_ciph(&_preg_268435636, &_preg_268435635);
  Free_poly_data(&_preg_268435635._c1_poly);
  Free_poly_data(&_preg_268435635._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n47, 0, sizeof(result_n47));
  Copy_ciph(&input_dup_n47, &_preg_268435636);
  Free_poly_data(&_preg_268435636._c1_poly);
  Free_poly_data(&_preg_268435636._c0_poly);
  for (index_khw1_n47 = 0; index_khw1_n47 < 9; index_khw1_n47 = index_khw1_n47 + 1) {
    _preg_268435796 = Rotate(input_dup_n47, _cst_392[index_khw1_n47]);
    Copy_ciph(&input_roll_n47[index_khw1_n47], &_preg_268435796);
    Free_poly_data(&_preg_268435796._c1_poly);
    Free_poly_data(&_preg_268435796._c0_poly);
  }
  Free_poly_data(&input_dup_n47._c1_poly);
  Free_poly_data(&input_dup_n47._c0_poly);
  for (index_cin_n47 = 0; index_cin_n47 < 64; index_cin_n47 = index_cin_n47 + 1) {
    memset(&result_cin_n47, 0, sizeof(result_cin_n47));
    for (index_khw2_n47 = 0; index_khw2_n47 < 9; index_khw2_n47 = index_khw2_n47 + 1) {
      _rescale_tmp_5083 = input_roll_n47[index_khw2_n47];
      Pt_from_msg(&_pgen_tmp_394, index_khw2_n47 + (index_cin_n47 * 9) + 12408 /* cst_390_0 */, 4096, Sc_degree(&_rescale_tmp_5083), Level(&_rescale_tmp_5083));
      Init_ciph_up_scale_plain(&_pgen_tmp_395, &_rescale_tmp_5083, &_pgen_tmp_394);
      Init_ciph_same_scale(&result_cin_n47, &result_cin_n47, &_pgen_tmp_395);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n47._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_395._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5083._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_394._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_395._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5083._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_394._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n47._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n47._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_395._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n47._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n47._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_395._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435797 = Rotate(result_cin_n47, -4096);
    Init_ciph_same_scale(&_pgen_tmp_396, &result_cin_n47, &_preg_268435797);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_396._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_396._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n47._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435797._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_396._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n47._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435797._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435798 = Rotate(_pgen_tmp_396, index_cin_n47 * 64);
    Init_ciph_same_scale(&result_n47, &result_n47, &_preg_268435798);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n47._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n47._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n47._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435798._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n47._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n47._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435798._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435797._c1_poly);
    Free_poly_data(&_preg_268435797._c0_poly);
    Free_poly_data(&_preg_268435798._c1_poly);
    Free_poly_data(&_preg_268435798._c0_poly);
  }
  Free_ciph_poly(input_roll_n47, 9);
  Free_poly_data(&_pgen_tmp_394._poly);
  Free_poly_data(&_pgen_tmp_395._c1_poly);
  Free_poly_data(&_pgen_tmp_395._c0_poly);
  Free_poly_data(&result_cin_n47._c1_poly);
  Free_poly_data(&result_cin_n47._c0_poly);
  Free_poly_data(&_pgen_tmp_396._c1_poly);
  Free_poly_data(&_pgen_tmp_396._c0_poly);
  Pt_from_msg(&_pgen_tmp_397, 12984 /* cst_391 */, 4096, Sc_degree(&result_n47), Level(&result_n47));
  Init_ciph_same_scale_plain(&result_n47, &result_n47, &_pgen_tmp_397);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n47._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n47._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n47._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_397._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n47._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n47._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_397._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_6128, &result_n47);
  Rescale(&_rescale_tmp_6128._c0_poly, &result_n47._c0_poly);
  Rescale(&_rescale_tmp_6128._c1_poly, &result_n47._c1_poly);
  Pt_from_msg(&_pgen_tmp_398, 12985 /* cst_393 */, 4096, Sc_degree(&_rescale_tmp_6128), Level(&_rescale_tmp_6128));
  Init_ciph_up_scale_plain(&result_n47, &_rescale_tmp_6128, &_pgen_tmp_398);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n47._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n47._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_6128._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_398._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n47._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_6128._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_398._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_398._poly);
    Free_poly_data(&_rescale_tmp_6128._c1_poly);
    Free_poly_data(&_rescale_tmp_6128._c0_poly);
  }
  Copy_ciph(&_preg_268435637, &result_n47);
  Free_poly_data(&result_n47._c1_poly);
  Free_poly_data(&result_n47._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::add");
  Init_ciph_down_scale(&_pgen_tmp_399, &_preg_268435637);
  Rescale(&_pgen_tmp_399._c0_poly, &_preg_268435637._c0_poly);
  Rescale(&_pgen_tmp_399._c1_poly, &_preg_268435637._c1_poly);
  Free_poly_data(&_preg_268435637._c1_poly);
  Free_poly_data(&_preg_268435637._c0_poly);
  Init_ciph_same_scale(&_preg_268435638, &_pgen_tmp_399, &_preg_268435632);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_preg_268435638._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_preg_268435638._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_399._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435632._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_preg_268435638._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_399._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435632._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435632._c1_poly);
    Free_poly_data(&_preg_268435632._c0_poly);
    Free_poly_data(&_pgen_tmp_399._c1_poly);
    Free_poly_data(&_pgen_tmp_399._c0_poly);
  }
  Tm_taken("Tensor::add");
  Bootstrap(&_preg_268435639, &_preg_268435638, 15);
  Free_poly_data(&_preg_268435638._c1_poly);
  Free_poly_data(&_preg_268435638._c0_poly);
  Encode_float(&_pgen_tmp_401, &_cst_485, 1, 1, Level(&_preg_268435639));
  Init_ciph_up_scale_plain(&_pgen_tmp_400, &_preg_268435639, &_pgen_tmp_401);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_400._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_400._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435639._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_401._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_400._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435639._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_401._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_401._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_402, &_pgen_tmp_400);
  Rescale(&_pgen_tmp_402._c0_poly, &_pgen_tmp_400._c0_poly);
  Rescale(&_pgen_tmp_402._c1_poly, &_pgen_tmp_400._c1_poly);
  Free_poly_data(&_pgen_tmp_400._c1_poly);
  Free_poly_data(&_pgen_tmp_400._c0_poly);
  _preg_268435640 = App_relu(_preg_268435639, _pgen_tmp_402);
  Free_poly_data(&_pgen_tmp_402._c1_poly);
  Free_poly_data(&_pgen_tmp_402._c0_poly);
  Free_poly_data(&_preg_268435639._c1_poly);
  Free_poly_data(&_preg_268435639._c0_poly);
  Copy_ciph(&_preg_268435641, &_preg_268435640);
  Free_poly_data(&_preg_268435640._c1_poly);
  Free_poly_data(&_preg_268435640._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n48, 0, sizeof(result_n48));
  Copy_ciph(&input_dup_n48, &_preg_268435641);
  for (index_khw1_n48 = 0; index_khw1_n48 < 9; index_khw1_n48 = index_khw1_n48 + 1) {
    _preg_268435799 = Rotate(input_dup_n48, _cst_399[index_khw1_n48]);
    Copy_ciph(&input_roll_n48[index_khw1_n48], &_preg_268435799);
    Free_poly_data(&_preg_268435799._c1_poly);
    Free_poly_data(&_preg_268435799._c0_poly);
  }
  Free_poly_data(&input_dup_n48._c1_poly);
  Free_poly_data(&input_dup_n48._c0_poly);
  for (index_cin_n48 = 0; index_cin_n48 < 64; index_cin_n48 = index_cin_n48 + 1) {
    memset(&result_cin_n48, 0, sizeof(result_cin_n48));
    for (index_khw2_n48 = 0; index_khw2_n48 < 9; index_khw2_n48 = index_khw2_n48 + 1) {
      _rescale_tmp_5197 = input_roll_n48[index_khw2_n48];
      Pt_from_msg(&_pgen_tmp_403, index_khw2_n48 + (index_cin_n48 * 9) + 12986 /* cst_397_0 */, 4096, Sc_degree(&_rescale_tmp_5197), Level(&_rescale_tmp_5197));
      Init_ciph_up_scale_plain(&_pgen_tmp_404, &_rescale_tmp_5197, &_pgen_tmp_403);
      Init_ciph_same_scale(&result_cin_n48, &result_cin_n48, &_pgen_tmp_404);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n48._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_404._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5197._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_403._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_404._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5197._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_403._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n48._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n48._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_404._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n48._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n48._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_404._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435800 = Rotate(result_cin_n48, -4096);
    Init_ciph_same_scale(&_pgen_tmp_405, &result_cin_n48, &_preg_268435800);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_405._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_405._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n48._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435800._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_405._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n48._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435800._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435801 = Rotate(_pgen_tmp_405, index_cin_n48 * 64);
    Init_ciph_same_scale(&result_n48, &result_n48, &_preg_268435801);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n48._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n48._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n48._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435801._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n48._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n48._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435801._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435800._c1_poly);
    Free_poly_data(&_preg_268435800._c0_poly);
    Free_poly_data(&_preg_268435801._c1_poly);
    Free_poly_data(&_preg_268435801._c0_poly);
  }
  Free_ciph_poly(input_roll_n48, 9);
  Free_poly_data(&_pgen_tmp_403._poly);
  Free_poly_data(&_pgen_tmp_404._c1_poly);
  Free_poly_data(&_pgen_tmp_404._c0_poly);
  Free_poly_data(&result_cin_n48._c1_poly);
  Free_poly_data(&result_cin_n48._c0_poly);
  Free_poly_data(&_pgen_tmp_405._c1_poly);
  Free_poly_data(&_pgen_tmp_405._c0_poly);
  Pt_from_msg(&_pgen_tmp_406, 13562 /* cst_398 */, 4096, Sc_degree(&result_n48), Level(&result_n48));
  Init_ciph_same_scale_plain(&result_n48, &result_n48, &_pgen_tmp_406);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n48._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n48._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n48._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_406._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n48._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n48._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_406._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_6141, &result_n48);
  Rescale(&_rescale_tmp_6141._c0_poly, &result_n48._c0_poly);
  Rescale(&_rescale_tmp_6141._c1_poly, &result_n48._c1_poly);
  Pt_from_msg(&_pgen_tmp_407, 13563 /* cst_400 */, 4096, Sc_degree(&_rescale_tmp_6141), Level(&_rescale_tmp_6141));
  Init_ciph_up_scale_plain(&result_n48, &_rescale_tmp_6141, &_pgen_tmp_407);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n48._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n48._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_6141._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_407._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n48._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_6141._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_407._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_407._poly);
    Free_poly_data(&_rescale_tmp_6141._c1_poly);
    Free_poly_data(&_rescale_tmp_6141._c0_poly);
  }
  Copy_ciph(&_preg_268435642, &result_n48);
  Free_poly_data(&result_n48._c1_poly);
  Free_poly_data(&result_n48._c0_poly);
  Tm_taken("Tensor::conv");
  Init_ciph_down_scale(&_pgen_tmp_408, &_preg_268435642);
  Rescale(&_pgen_tmp_408._c0_poly, &_preg_268435642._c0_poly);
  Rescale(&_pgen_tmp_408._c1_poly, &_preg_268435642._c1_poly);
  Free_poly_data(&_preg_268435642._c1_poly);
  Free_poly_data(&_preg_268435642._c0_poly);
  Bootstrap(&_preg_268435643, &_pgen_tmp_408, 15);
  Free_poly_data(&_pgen_tmp_408._c1_poly);
  Free_poly_data(&_pgen_tmp_408._c0_poly);
  Encode_float(&_pgen_tmp_410, &_cst_486, 1, 1, Level(&_preg_268435643));
  Init_ciph_up_scale_plain(&_pgen_tmp_409, &_preg_268435643, &_pgen_tmp_410);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_409._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_409._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435643._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_410._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_409._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435643._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_410._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_410._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_411, &_pgen_tmp_409);
  Rescale(&_pgen_tmp_411._c0_poly, &_pgen_tmp_409._c0_poly);
  Rescale(&_pgen_tmp_411._c1_poly, &_pgen_tmp_409._c1_poly);
  Free_poly_data(&_pgen_tmp_409._c1_poly);
  Free_poly_data(&_pgen_tmp_409._c0_poly);
  _preg_268435644 = App_relu(_preg_268435643, _pgen_tmp_411);
  Free_poly_data(&_pgen_tmp_411._c1_poly);
  Free_poly_data(&_pgen_tmp_411._c0_poly);
  Free_poly_data(&_preg_268435643._c1_poly);
  Free_poly_data(&_preg_268435643._c0_poly);
  Copy_ciph(&_preg_268435645, &_preg_268435644);
  Free_poly_data(&_preg_268435644._c1_poly);
  Free_poly_data(&_preg_268435644._c0_poly);
  Tm_start("Tensor::conv");
  memset(&result_n49, 0, sizeof(result_n49));
  Copy_ciph(&input_dup_n49, &_preg_268435645);
  Free_poly_data(&_preg_268435645._c1_poly);
  Free_poly_data(&_preg_268435645._c0_poly);
  for (index_khw1_n49 = 0; index_khw1_n49 < 9; index_khw1_n49 = index_khw1_n49 + 1) {
    _preg_268435802 = Rotate(input_dup_n49, _cst_405[index_khw1_n49]);
    Copy_ciph(&input_roll_n49[index_khw1_n49], &_preg_268435802);
    Free_poly_data(&_preg_268435802._c1_poly);
    Free_poly_data(&_preg_268435802._c0_poly);
  }
  Free_poly_data(&input_dup_n49._c1_poly);
  Free_poly_data(&input_dup_n49._c0_poly);
  for (index_cin_n49 = 0; index_cin_n49 < 64; index_cin_n49 = index_cin_n49 + 1) {
    memset(&result_cin_n49, 0, sizeof(result_cin_n49));
    for (index_khw2_n49 = 0; index_khw2_n49 < 9; index_khw2_n49 = index_khw2_n49 + 1) {
      _rescale_tmp_5305 = input_roll_n49[index_khw2_n49];
      Pt_from_msg(&_pgen_tmp_412, index_khw2_n49 + (index_cin_n49 * 9) + 13564 /* cst_403_0 */, 4096, Sc_degree(&_rescale_tmp_5305), Level(&_rescale_tmp_5305));
      Init_ciph_up_scale_plain(&_pgen_tmp_413, &_rescale_tmp_5305, &_pgen_tmp_412);
      Init_ciph_same_scale(&result_cin_n49, &result_cin_n49, &_pgen_tmp_413);
      {
        _pgen_modulus = Q_modulus();
        _pgen_num_q = Poly_level(&result_cin_n49._c0_poly);
        for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
          Hw_modmul(Coeffs(&_pgen_tmp_413._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5305._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_412._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modmul(Coeffs(&_pgen_tmp_413._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5305._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_412._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n49._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n49._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_413._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          Hw_modadd(Coeffs(&result_cin_n49._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n49._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_413._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
          _pgen_modulus = _pgen_modulus + 1;
        }
      }
    }
    _preg_268435803 = Rotate(result_cin_n49, -4096);
    Init_ciph_same_scale(&_pgen_tmp_414, &result_cin_n49, &_preg_268435803);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_pgen_tmp_414._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_414._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n49._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435803._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_414._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_cin_n49._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435803._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    _preg_268435804 = Rotate(_pgen_tmp_414, index_cin_n49 * 64);
    Init_ciph_same_scale(&result_n49, &result_n49, &_preg_268435804);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&result_n49._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&result_n49._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n49._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435804._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&result_n49._c1_poly, _pgen_rns_idx, degree), Coeffs(&result_n49._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435804._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435803._c1_poly);
    Free_poly_data(&_preg_268435803._c0_poly);
    Free_poly_data(&_preg_268435804._c1_poly);
    Free_poly_data(&_preg_268435804._c0_poly);
  }
  Free_ciph_poly(input_roll_n49, 9);
  Free_poly_data(&_pgen_tmp_412._poly);
  Free_poly_data(&_pgen_tmp_413._c1_poly);
  Free_poly_data(&_pgen_tmp_413._c0_poly);
  Free_poly_data(&result_cin_n49._c1_poly);
  Free_poly_data(&result_cin_n49._c0_poly);
  Free_poly_data(&_pgen_tmp_414._c1_poly);
  Free_poly_data(&_pgen_tmp_414._c0_poly);
  Pt_from_msg(&_pgen_tmp_415, 14140 /* cst_404 */, 4096, Sc_degree(&result_n49), Level(&result_n49));
  Init_ciph_same_scale_plain(&result_n49, &result_n49, &_pgen_tmp_415);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n49._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&result_n49._c0_poly, _pgen_rns_idx, degree), Coeffs(&result_n49._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_415._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&result_n49._c1_poly, _pgen_rns_idx, degree, Coeffs(&result_n49._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_415._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_6154, &result_n49);
  Rescale(&_rescale_tmp_6154._c0_poly, &result_n49._c0_poly);
  Rescale(&_rescale_tmp_6154._c1_poly, &result_n49._c1_poly);
  Pt_from_msg(&_pgen_tmp_416, 14141 /* cst_406 */, 4096, Sc_degree(&_rescale_tmp_6154), Level(&_rescale_tmp_6154));
  Init_ciph_up_scale_plain(&result_n49, &_rescale_tmp_6154, &_pgen_tmp_416);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&result_n49._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&result_n49._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_6154._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_416._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&result_n49._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_6154._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_416._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_416._poly);
    Free_poly_data(&_rescale_tmp_6154._c1_poly);
    Free_poly_data(&_rescale_tmp_6154._c0_poly);
  }
  Copy_ciph(&_preg_268435646, &result_n49);
  Free_poly_data(&result_n49._c1_poly);
  Free_poly_data(&result_n49._c0_poly);
  Tm_taken("Tensor::conv");
  Tm_start("Tensor::add");
  Init_ciph_down_scale(&_pgen_tmp_417, &_preg_268435646);
  Rescale(&_pgen_tmp_417._c0_poly, &_preg_268435646._c0_poly);
  Rescale(&_pgen_tmp_417._c1_poly, &_preg_268435646._c1_poly);
  Free_poly_data(&_preg_268435646._c1_poly);
  Free_poly_data(&_preg_268435646._c0_poly);
  Init_ciph_same_scale(&_preg_268435647, &_pgen_tmp_417, &_preg_268435641);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_preg_268435647._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&_preg_268435647._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_417._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435641._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_preg_268435647._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_417._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435641._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435641._c1_poly);
    Free_poly_data(&_preg_268435641._c0_poly);
    Free_poly_data(&_pgen_tmp_417._c1_poly);
    Free_poly_data(&_pgen_tmp_417._c0_poly);
  }
  Tm_taken("Tensor::add");
  Bootstrap(&_preg_268435648, &_preg_268435647, 17);
  Free_poly_data(&_preg_268435647._c1_poly);
  Free_poly_data(&_preg_268435647._c0_poly);
  Encode_float(&_pgen_tmp_419, &_cst_487, 1, 1, Level(&_preg_268435648));
  Init_ciph_up_scale_plain(&_pgen_tmp_418, &_preg_268435648, &_pgen_tmp_419);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_418._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_418._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435648._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_419._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_418._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435648._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_419._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_419._poly);
  }
  Init_ciph_down_scale(&_pgen_tmp_420, &_pgen_tmp_418);
  Rescale(&_pgen_tmp_420._c0_poly, &_pgen_tmp_418._c0_poly);
  Rescale(&_pgen_tmp_420._c1_poly, &_pgen_tmp_418._c1_poly);
  Free_poly_data(&_pgen_tmp_418._c1_poly);
  Free_poly_data(&_pgen_tmp_418._c0_poly);
  _preg_268435649 = App_relu(_preg_268435648, _pgen_tmp_420);
  Free_poly_data(&_pgen_tmp_420._c1_poly);
  Free_poly_data(&_pgen_tmp_420._c0_poly);
  Free_poly_data(&_preg_268435648._c1_poly);
  Free_poly_data(&_preg_268435648._c0_poly);
  Copy_ciph(&_preg_268435650, &_preg_268435649);
  Free_poly_data(&_preg_268435649._c1_poly);
  Free_poly_data(&_preg_268435649._c0_poly);
  Tm_start("Tensor::global_avg_pool");
  for (sum_index_n49 = 0; sum_index_n49 < 6; sum_index_n49 = sum_index_n49 + 1) {
    _preg_268435805 = Rotate(_preg_268435650, 1 << sum_index_n49);
    Init_ciph_same_scale(&_preg_268435650, &_preg_268435650, &_preg_268435805);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&_preg_268435650._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_preg_268435650._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435650._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435805._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_preg_268435650._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435650._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435805._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435805._c1_poly);
    Free_poly_data(&_preg_268435805._c0_poly);
  }
  Copy_ciph(&_rescale_tmp_5390, &_preg_268435650);
  Free_poly_data(&_preg_268435650._c1_poly);
  Free_poly_data(&_preg_268435650._c0_poly);
  Pt_from_msg(&_pgen_tmp_421, 14142 /* cst_409 */, 4096, Sc_degree(&_rescale_tmp_5390), Level(&_rescale_tmp_5390));
  Init_ciph_up_scale_plain(&gap_result49, &_rescale_tmp_5390, &_pgen_tmp_421);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&gap_result49._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&gap_result49._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5390._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_421._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&gap_result49._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_5390._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_421._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_421._poly);
    Free_poly_data(&_rescale_tmp_5390._c1_poly);
    Free_poly_data(&_rescale_tmp_5390._c0_poly);
  }
  for (combine_cc_index_n49 = 0; combine_cc_index_n49 < 6; combine_cc_index_n49 = combine_cc_index_n49 + 1) {
    _preg_268435806 = Rotate(gap_result49, (1 << combine_cc_index_n49) * 63);
    Init_ciph_same_scale(&gap_result49, &gap_result49, &_preg_268435806);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&gap_result49._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&gap_result49._c0_poly, _pgen_rns_idx, degree), Coeffs(&gap_result49._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435806._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&gap_result49._c1_poly, _pgen_rns_idx, degree), Coeffs(&gap_result49._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435806._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435806._c1_poly);
    Free_poly_data(&_preg_268435806._c0_poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_6167, &gap_result49);
  Rescale(&_rescale_tmp_6167._c0_poly, &gap_result49._c0_poly);
  Rescale(&_rescale_tmp_6167._c1_poly, &gap_result49._c1_poly);
  Pt_from_msg(&_pgen_tmp_422, 14143 /* cst_410 */, 64, Sc_degree(&_rescale_tmp_6167), Level(&_rescale_tmp_6167));
  Init_ciph_up_scale_plain(&gap_result49, &_rescale_tmp_6167, &_pgen_tmp_422);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&gap_result49._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&gap_result49._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_6167._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_422._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&gap_result49._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_6167._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_422._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_422._poly);
    Free_poly_data(&_rescale_tmp_6167._c1_poly);
    Free_poly_data(&_rescale_tmp_6167._c0_poly);
  }
  Copy_ciph(&_preg_268435651, &gap_result49);
  Free_poly_data(&gap_result49._c1_poly);
  Free_poly_data(&gap_result49._c0_poly);
  Tm_taken("Tensor::global_avg_pool");
  Tm_start("Tensor::flatten");
  Copy_ciph(&_preg_268435652, &_preg_268435651);
  Free_poly_data(&_preg_268435651._c1_poly);
  Free_poly_data(&_preg_268435651._c0_poly);
  Tm_taken("Tensor::flatten");
  Tm_start("Tensor::gemm");
  memset(&tmp_result_n50, 0, sizeof(tmp_result_n50));
  _preg_268435807 = Rotate(_preg_268435652, -70);
  Init_ciph_same_scale(&input_dup_n50, &_preg_268435652, &_preg_268435807);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&input_dup_n50._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&input_dup_n50._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435652._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435807._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&input_dup_n50._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435652._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435807._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_preg_268435807._c1_poly);
    Free_poly_data(&_preg_268435807._c0_poly);
    Free_poly_data(&_preg_268435652._c1_poly);
    Free_poly_data(&_preg_268435652._c0_poly);
  }
  for (index_gemm_n50 = 0; index_gemm_n50 < 10; index_gemm_n50 = index_gemm_n50 + 1) {
    _preg_268435808 = Rotate(input_dup_n50, index_gemm_n50);
    Copy_ciph(&_ckks_gen_tmp_5455, &_preg_268435808);
    Init_ciph_down_scale(&_rescale_tmp_6173, &_ckks_gen_tmp_5455);
    Rescale(&_rescale_tmp_6173._c0_poly, &_ckks_gen_tmp_5455._c0_poly);
    Rescale(&_rescale_tmp_6173._c1_poly, &_ckks_gen_tmp_5455._c1_poly);
    Pt_from_msg(&_pgen_tmp_423, index_gemm_n50 + 14144 /* cst_413_0 */, 70, Sc_degree(&_rescale_tmp_6173), Level(&_rescale_tmp_6173));
    Init_ciph_up_scale_plain(&_pgen_tmp_424, &_rescale_tmp_6173, &_pgen_tmp_423);
    Init_ciph_same_scale(&tmp_result_n50, &tmp_result_n50, &_pgen_tmp_424);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&tmp_result_n50._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modmul(Coeffs(&_pgen_tmp_424._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_6173._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_423._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modmul(Coeffs(&_pgen_tmp_424._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_6173._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_423._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&tmp_result_n50._c0_poly, _pgen_rns_idx, degree), Coeffs(&tmp_result_n50._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_424._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&tmp_result_n50._c1_poly, _pgen_rns_idx, degree), Coeffs(&tmp_result_n50._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_424._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435808._c1_poly);
    Free_poly_data(&_preg_268435808._c0_poly);
  }
  Free_poly_data(&input_dup_n50._c1_poly);
  Free_poly_data(&input_dup_n50._c0_poly);
  Free_poly_data(&_ckks_gen_tmp_5455._c1_poly);
  Free_poly_data(&_ckks_gen_tmp_5455._c0_poly);
  Free_poly_data(&_pgen_tmp_423._poly);
  Free_poly_data(&_rescale_tmp_6173._c1_poly);
  Free_poly_data(&_rescale_tmp_6173._c0_poly);
  Free_poly_data(&_pgen_tmp_424._c1_poly);
  Free_poly_data(&_pgen_tmp_424._c0_poly);
  for (index_add_n50 = 0; index_add_n50 < 3; index_add_n50 = index_add_n50 + 1) {
    _preg_268435809 = Rotate(tmp_result_n50, (1 << index_add_n50) * 10);
    Init_ciph_same_scale(&tmp_result_n50, &tmp_result_n50, &_preg_268435809);
    {
      _pgen_modulus = Q_modulus();
      _pgen_num_q = Poly_level(&tmp_result_n50._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&tmp_result_n50._c0_poly, _pgen_rns_idx, degree), Coeffs(&tmp_result_n50._c0_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435809._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&tmp_result_n50._c1_poly, _pgen_rns_idx, degree), Coeffs(&tmp_result_n50._c1_poly, _pgen_rns_idx, degree), Coeffs(&_preg_268435809._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly_data(&_preg_268435809._c1_poly);
    Free_poly_data(&_preg_268435809._c0_poly);
  }
  Pt_from_msg(&_pgen_tmp_425, 14154 /* cst_1 */, 10, Sc_degree(&tmp_result_n50), Level(&tmp_result_n50));
  Init_ciph_same_scale_plain(&tmp_result_n50, &tmp_result_n50, &_pgen_tmp_425);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&tmp_result_n50._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modadd(Coeffs(&tmp_result_n50._c0_poly, _pgen_rns_idx, degree), Coeffs(&tmp_result_n50._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_425._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Set_coeffs(&tmp_result_n50._c1_poly, _pgen_rns_idx, degree, Coeffs(&tmp_result_n50._c1_poly, _pgen_rns_idx, degree));
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_425._poly);
  }
  Init_ciph_down_scale(&_rescale_tmp_6179, &tmp_result_n50);
  Rescale(&_rescale_tmp_6179._c0_poly, &tmp_result_n50._c0_poly);
  Rescale(&_rescale_tmp_6179._c1_poly, &tmp_result_n50._c1_poly);
  Pt_from_msg(&_pgen_tmp_426, 14155 /* cst_414 */, 10, Sc_degree(&_rescale_tmp_6179), Level(&_rescale_tmp_6179));
  Init_ciph_up_scale_plain(&tmp_result_n50, &_rescale_tmp_6179, &_pgen_tmp_426);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&tmp_result_n50._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&tmp_result_n50._c0_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_6179._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_426._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&tmp_result_n50._c1_poly, _pgen_rns_idx, degree), Coeffs(&_rescale_tmp_6179._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_426._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
    Free_poly_data(&_pgen_tmp_426._poly);
    Free_poly_data(&_rescale_tmp_6179._c1_poly);
    Free_poly_data(&_rescale_tmp_6179._c0_poly);
  }
  Copy_ciph(&output, &tmp_result_n50);
  Free_poly_data(&tmp_result_n50._c1_poly);
  Free_poly_data(&tmp_result_n50._c0_poly);
  Tm_taken("Tensor::gemm");
  Set_output_data("output", 0, &output);
  return true;
}

int Get_input_count() {
  return 1;
}

DATA_SCHEME* Get_encode_scheme(int idx) {
  static MAP_DESC desc_0[] = {{NORMAL, 0, 0, 0, 0}};
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
  static MAP_DESC desc[] = {{NORMAL, 0, 0, 0, 0}};
  static DATA_SCHEME scheme = {
    "output", {0, 0, 0, 0}, 1, desc
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
  Encode_float(&_pgen_tmp_4, &_cst_415, 1, Sc_degree(&_pgen_tmp_3), Level(&_pgen_tmp_3));
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
  Encode_float(&_pgen_tmp_11, &_cst_416, 1, Sc_degree(&_pgen_tmp_10), Level(&_pgen_tmp_10));
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
  Encode_double(&_pgen_tmp_18, &_cst_417, 1, 1, Level(&input_ct1));
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
  Encode_double(&_pgen_tmp_22, &_cst_418, 1, 1, Level(&input_ct1));
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
  Encode_double(&_pgen_tmp_30, &_cst_419, 1, 1, Level(&input_ct1));
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
  Encode_double(&_pgen_tmp_34, &_cst_420, 1, 1, Level(&input_ct1));
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
  Encode_float(&_pgen_tmp_43, &_cst_421, 1, Sc_degree(&_pgen_tmp_42), Level(&_pgen_tmp_42));
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
  Encode_float(&_pgen_tmp_51, &_cst_422, 1, 1, Level(&_pgen_tmp_50));
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
  Encode_float(&_pgen_tmp_59, &_cst_423, 1, Sc_degree(&_pgen_tmp_58), Level(&_pgen_tmp_58));
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
  Encode_float(&_pgen_tmp_66, &_cst_424, 1, Sc_degree(&_pgen_tmp_65), Level(&_pgen_tmp_65));
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
  Encode_double(&_pgen_tmp_77, &_cst_425, 1, 1, Level(&_pgen_tmp_76));
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
  Encode_double(&_pgen_tmp_82, &_cst_426, 1, 1, Level(&_pgen_tmp_81));
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
  Encode_double(&_pgen_tmp_88, &_cst_428, 1, 1, Level(&_pgen_tmp_87));
  Init_ciph_up_scale_plain(&_pgen_tmp_89, &_pgen_tmp_87, &_pgen_tmp_88);
  Init_ciph_down_scale(&_pgen_tmp_90, &_relu_tmp45);
  Rescale(&_pgen_tmp_90._c0_poly, &_relu_tmp45._c0_poly);
  Rescale(&_pgen_tmp_90._c1_poly, &_relu_tmp45._c1_poly);
  Encode_double(&_pgen_tmp_91, &_cst_427, 1, 1, Level(&_pgen_tmp_90));
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
  Encode_double(&_pgen_tmp_101, &_cst_430, 1, 1, Level(&_pgen_tmp_100));
  Init_ciph_up_scale_plain(&_pgen_tmp_102, &_pgen_tmp_100, &_pgen_tmp_101);
  Init_ciph_down_scale(&_pgen_tmp_103, &_relu_tmp45);
  Rescale(&_pgen_tmp_103._c0_poly, &_relu_tmp45._c0_poly);
  Rescale(&_pgen_tmp_103._c1_poly, &_relu_tmp45._c1_poly);
  Encode_double(&_pgen_tmp_104, &_cst_429, 1, 1, Level(&_pgen_tmp_103));
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
  Encode_double(&_pgen_tmp_110, &_cst_432, 1, 1, Level(&_pgen_tmp_109));
  Init_ciph_up_scale_plain(&_pgen_tmp_111, &_pgen_tmp_109, &_pgen_tmp_110);
  Init_ciph_down_scale(&_pgen_tmp_112, &_relu_tmp45);
  Rescale(&_pgen_tmp_112._c0_poly, &_relu_tmp45._c0_poly);
  Rescale(&_pgen_tmp_112._c1_poly, &_relu_tmp45._c1_poly);
  Free_poly_data(&_relu_tmp45._c1_poly);
  Free_poly_data(&_relu_tmp45._c0_poly);
  Encode_double(&_pgen_tmp_113, &_cst_431, 1, 1, Level(&_pgen_tmp_112));
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
  Encode_float(&_pgen_tmp_123, &_cst_433, 1, Sc_degree(&_pgen_tmp_122), Level(&_pgen_tmp_122));
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
  Encode_float(&_pgen_tmp_131, &_cst_434, 1, 1, Level(&_pgen_tmp_130));
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
  Encode_float(&_pgen_tmp_139, &_cst_435, 1, Sc_degree(&_pgen_tmp_138), Level(&_pgen_tmp_138));
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
  Encode_float(&_pgen_tmp_146, &_cst_436, 1, Sc_degree(&_pgen_tmp_145), Level(&_pgen_tmp_145));
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
  Encode_double(&_pgen_tmp_155, &_cst_437, 1, 1, Level(&input_ct0));
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
  Encode_double(&_pgen_tmp_165, &_cst_439, 1, 1, Level(&input_ct0));
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
  Encode_double(&_pgen_tmp_172, &_cst_438, 1, 1, Level(&input_ct0));
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
  Encode_double(&_pgen_tmp_186, &_cst_441, 1, 1, Level(&input_ct0));
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
  Encode_double(&_pgen_tmp_193, &_cst_440, 1, 1, Level(&input_ct0));
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
  Encode_double(&_pgen_tmp_203, &_cst_443, 1, 1, Level(&input_ct0));
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
  Encode_double(&_pgen_tmp_210, &_cst_442, 1, 1, Level(&input_ct0));
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
  Encode_float(&_pgen_tmp_216, &_cst_444, 1, 1, Level(&input_ct0));
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
    LIB_ANT, 65536, 0, 33, 0, 51, 46, 3, 192, 197,
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
    "/app/release/resnet44_cifar10_pre.onnx.omp.msg",
    "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX",
    DE_MSG_F32
  };
  return &info;
}

int32_t _cst_109[9] = {
  -33, -32, -31, -1, 0, 1, 31, 32,
  33
};
int32_t _cst_115[9] = {
  -33, -32, -31, -1, 0, 1, 31, 32,
  33
};
int32_t _cst_121[9] = {
  -33, -32, -31, -1, 0, 1, 31, 32,
  33
};
int32_t _cst_128[9] = {
  -33, -32, -31, -1, 0, 1, 31, 32,
  33
};
int32_t _cst_134[9] = {
  -33, -32, -31, -1, 0, 1, 31, 32,
  33
};
int32_t _cst_141[9] = {
  -33, -32, -31, -1, 0, 1, 31, 32,
  33
};
int32_t _cst_147[9] = {
  -33, -32, -31, -1, 0, 1, 31, 32,
  33
};
int32_t _cst_154[9] = {
  -33, -32, -31, -1, 0, 1, 31, 32,
  33
};
int32_t _cst_160[9] = {
  -33, -32, -31, -1, 0, 1, 31, 32,
  33
};
int32_t _cst_167[9] = {
  -33, -32, -31, -1, 0, 1, 31, 32,
  33
};
int32_t _cst_173[9] = {
  -33, -32, -31, -1, 0, 1, 31, 32,
  33
};
int32_t _cst_180[9] = {
  -33, -32, -31, -1, 0, 1, 31, 32,
  33
};
int32_t _cst_186[9] = {
  -33, -32, -31, -1, 0, 1, 31, 32,
  33
};
int32_t _cst_193[9] = {
  -33, -32, -31, -1, 0, 1, 31, 32,
  33
};
int32_t _cst_199[9] = {
  -33, -32, -31, -1, 0, 1, 31, 32,
  33
};
int32_t _cst_206[9] = {
  -33, -32, -31, -1, 0, 1, 31, 32,
  33
};
int32_t _cst_215[9] = {
  -17, -16, -15, -1, 0, 1, 15, 16,
  17
};
int32_t _cst_221[1] = {
  0
};
int32_t _cst_231[9] = {
  -17, -16, -15, -1, 0, 1, 15, 16,
  17
};
int32_t _cst_237[9] = {
  -17, -16, -15, -1, 0, 1, 15, 16,
  17
};
int32_t _cst_244[9] = {
  -17, -16, -15, -1, 0, 1, 15, 16,
  17
};
int32_t _cst_250[9] = {
  -17, -16, -15, -1, 0, 1, 15, 16,
  17
};
int32_t _cst_257[9] = {
  -17, -16, -15, -1, 0, 1, 15, 16,
  17
};
int32_t _cst_263[9] = {
  -17, -16, -15, -1, 0, 1, 15, 16,
  17
};
int32_t _cst_270[9] = {
  -17, -16, -15, -1, 0, 1, 15, 16,
  17
};
int32_t _cst_276[9] = {
  -17, -16, -15, -1, 0, 1, 15, 16,
  17
};
int32_t _cst_283[9] = {
  -17, -16, -15, -1, 0, 1, 15, 16,
  17
};
int32_t _cst_289[9] = {
  -17, -16, -15, -1, 0, 1, 15, 16,
  17
};
int32_t _cst_296[9] = {
  -17, -16, -15, -1, 0, 1, 15, 16,
  17
};
int32_t _cst_302[9] = {
  -17, -16, -15, -1, 0, 1, 15, 16,
  17
};
int32_t _cst_309[9] = {
  -17, -16, -15, -1, 0, 1, 15, 16,
  17
};
int32_t _cst_318[9] = {
  -9, -8, -7, -1, 0, 1, 7, 8,
  9
};
int32_t _cst_324[1] = {
  0
};
int32_t _cst_334[9] = {
  -9, -8, -7, -1, 0, 1, 7, 8,
  9
};
int32_t _cst_340[9] = {
  -9, -8, -7, -1, 0, 1, 7, 8,
  9
};
int32_t _cst_347[9] = {
  -9, -8, -7, -1, 0, 1, 7, 8,
  9
};
int32_t _cst_353[9] = {
  -9, -8, -7, -1, 0, 1, 7, 8,
  9
};
int32_t _cst_360[9] = {
  -9, -8, -7, -1, 0, 1, 7, 8,
  9
};
int32_t _cst_366[9] = {
  -9, -8, -7, -1, 0, 1, 7, 8,
  9
};
int32_t _cst_373[9] = {
  -9, -8, -7, -1, 0, 1, 7, 8,
  9
};
int32_t _cst_379[9] = {
  -9, -8, -7, -1, 0, 1, 7, 8,
  9
};
int32_t _cst_386[9] = {
  -9, -8, -7, -1, 0, 1, 7, 8,
  9
};
int32_t _cst_392[9] = {
  -9, -8, -7, -1, 0, 1, 7, 8,
  9
};
int32_t _cst_399[9] = {
  -9, -8, -7, -1, 0, 1, 7, 8,
  9
};
int32_t _cst_405[9] = {
  -9, -8, -7, -1, 0, 1, 7, 8,
  9
};
