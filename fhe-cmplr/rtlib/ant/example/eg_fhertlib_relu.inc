// external header files
#include "rt_ant/rt_ant.h"

typedef float float64_t;
typedef float float32_t;

// global constants definition
float64_t _cst_0 = -60.9472;
float64_t _cst_1 = 111.108;
float64_t _cst_2 = -60.6401;
float64_t _cst_3 = 10.6451;
float64_t _cst_4 = -0.327737;
float64_t _cst_5 = 4.25234;
float64_t _cst_6 = 60.3433;
float64_t _cst_7 = -22.2785;
float64_t _cst_8 = 72.3931;
float64_t _cst_9 = -89.8639;
float64_t _cst_10 = 5.90652;
float64_t _cst_11 = -29.2968;
float64_t _cst_12 = 0.113779;
float64_t _cst_13 = 2.53647;
float64_t _cst_14 = -0.821135;
float64_t _cst_15 = 4.57054;
float64_t _cst_16 = -4.36575;
float64_t _cst_17 = 1.48283;
float64_t _cst_18 = -3.01674;
float64_t _cst_19 = 0.5;

CIPHERTEXT App_relu(CIPHERTEXT input_ct);

bool Main_graph() {
  CIPHERTEXT input;
  CIPHERTEXT output;
  CIPHERTEXT bt_ciph;
  CIPHERTEXT _preg_268435458;
  uint32_t  degree = Degree();
  input = Get_input_data("input", 0);
  memset(&output, 0, sizeof(output));
  memset(&bt_ciph, 0, sizeof(bt_ciph));
  Bootstrap(&bt_ciph, &input, 0);
  _preg_268435458 = App_relu(bt_ciph);
  output = _preg_268435458;
  Set_output_data("output", 0, &output);
  return true;
}

CIPHERTEXT App_relu(CIPHERTEXT input_ct) {
  CIPHERTEXT _relu_tmp4;
  CIPHERTEXT _relu_tmp8;
  CIPHERTEXT _relu_tmp30;
  CIPHERTEXT _relu_tmp34;
  CIPHERTEXT _relu_tmp38;
  CIPHERTEXT _relu_tmp42;
  CIPHERTEXT _relu_tmp46;
  CIPHERTEXT _relu_tmp86;
  CIPHERTEXT _relu_tmp90;
  CIPHERTEXT _relu_tmp94;
  CIPHERTEXT _relu_tmp98;
  CIPHERTEXT _relu_tmp102;
  CIPHERTEXT _relu_tmp106;
  CIPHERTEXT _relu_tmp146;
  CIPHERTEXT _relu_tmp152;
  CIPHERTEXT3 _pgen_tmp_0;
  MODULUS* _pgen_modulus;
  uint32_t _pgen_num_q;
  uint32_t _pgen_rns_idx;
  POLY _pgen_tmp_1;
  POLY _pgen_tmp_2;
  POLY _pgen_swk_c0;
  POLY _pgen_swk_c1;
  POLY _pgen_ext;
  POLY _pgen_tmp_poly;
  POLY _pgen_mod_down_c0;
  POLY _pgen_mod_down_c1;
  POLY _pgen_decomp;
  SWITCH_KEY* _pgen_swk;
  uint32_t _pgen_part_idx;
  POLY _pgen_key0;
  POLY _pgen_key1;
  uint32_t _pgen_num_p;
  uint32_t _pgen_p_ofst;
  uint32_t _pgen_p_idx;
  uint32_t _pgen_key_p_ofst;
  uint32_t _pgen_key_p_idx;
  CIPHERTEXT3 _pgen_tmp_3;
  CIPHERTEXT _pgen_tmp_4;
  CIPHERTEXT _pgen_tmp_5;
  POLY _pgen_tmp_6;
  POLY _pgen_tmp_7;
  CIPHERTEXT _pgen_tmp_8;
  CIPHERTEXT3 _pgen_tmp_9;
  CIPHERTEXT _pgen_tmp_10;
  CIPHERTEXT _pgen_tmp_11;
  CIPHERTEXT _pgen_tmp_12;
  CIPHERTEXT3 _pgen_tmp_13;
  CIPHERTEXT _pgen_tmp_14;
  CIPHERTEXT _pgen_tmp_15;
  PLAINTEXT _pgen_tmp_16;
  CIPHERTEXT _pgen_tmp_17;
  POLY _pgen_tmp_18;
  POLY _pgen_tmp_19;
  PLAINTEXT _pgen_tmp_20;
  CIPHERTEXT _pgen_tmp_21;
  CIPHERTEXT _pgen_tmp_22;
  POLY _pgen_tmp_23;
  POLY _pgen_tmp_24;
  CIPHERTEXT _pgen_tmp_25;
  CIPHERTEXT3 _pgen_tmp_26;
  CIPHERTEXT _pgen_tmp_27;
  CIPHERTEXT _pgen_tmp_28;
  PLAINTEXT _pgen_tmp_29;
  CIPHERTEXT _pgen_tmp_30;
  POLY _pgen_tmp_31;
  POLY _pgen_tmp_32;
  PLAINTEXT _pgen_tmp_33;
  CIPHERTEXT _pgen_tmp_34;
  CIPHERTEXT _pgen_tmp_35;
  CIPHERTEXT3 _pgen_tmp_36;
  CIPHERTEXT _pgen_tmp_37;
  CIPHERTEXT _pgen_tmp_38;
  POLY _pgen_tmp_39;
  POLY _pgen_tmp_40;
  CIPHERTEXT3 _pgen_tmp_41;
  CIPHERTEXT _pgen_tmp_42;
  CIPHERTEXT _pgen_tmp_43;
  POLY _pgen_tmp_44;
  POLY _pgen_tmp_45;
  CIPHERTEXT3 _pgen_tmp_46;
  CIPHERTEXT _pgen_tmp_47;
  CIPHERTEXT _pgen_tmp_48;
  POLY _pgen_tmp_49;
  POLY _pgen_tmp_50;
  CIPHERTEXT3 _pgen_tmp_51;
  CIPHERTEXT _pgen_tmp_52;
  CIPHERTEXT _pgen_tmp_53;
  POLY _pgen_tmp_54;
  POLY _pgen_tmp_55;
  CIPHERTEXT _pgen_tmp_56;
  CIPHERTEXT3 _pgen_tmp_57;
  CIPHERTEXT _pgen_tmp_58;
  CIPHERTEXT _pgen_tmp_59;
  CIPHERTEXT _pgen_tmp_60;
  CIPHERTEXT3 _pgen_tmp_61;
  CIPHERTEXT _pgen_tmp_62;
  CIPHERTEXT _pgen_tmp_63;
  CIPHERTEXT _pgen_tmp_64;
  CIPHERTEXT3 _pgen_tmp_65;
  CIPHERTEXT _pgen_tmp_66;
  CIPHERTEXT _pgen_tmp_67;
  CIPHERTEXT _pgen_tmp_68;
  PLAINTEXT _pgen_tmp_69;
  CIPHERTEXT _pgen_tmp_70;
  POLY _pgen_tmp_71;
  POLY _pgen_tmp_72;
  CIPHERTEXT _pgen_tmp_73;
  PLAINTEXT _pgen_tmp_74;
  CIPHERTEXT _pgen_tmp_75;
  CIPHERTEXT _pgen_tmp_76;
  POLY _pgen_tmp_77;
  POLY _pgen_tmp_78;
  CIPHERTEXT _pgen_tmp_79;
  PLAINTEXT _pgen_tmp_80;
  CIPHERTEXT _pgen_tmp_81;
  CIPHERTEXT _pgen_tmp_82;
  PLAINTEXT _pgen_tmp_83;
  CIPHERTEXT _pgen_tmp_84;
  CIPHERTEXT _pgen_tmp_85;
  CIPHERTEXT _pgen_tmp_86;
  POLY _pgen_tmp_87;
  POLY _pgen_tmp_88;
  CIPHERTEXT _pgen_tmp_89;
  CIPHERTEXT3 _pgen_tmp_90;
  CIPHERTEXT _pgen_tmp_91;
  CIPHERTEXT _pgen_tmp_92;
  CIPHERTEXT _pgen_tmp_93;
  PLAINTEXT _pgen_tmp_94;
  CIPHERTEXT _pgen_tmp_95;
  CIPHERTEXT _pgen_tmp_96;
  PLAINTEXT _pgen_tmp_97;
  CIPHERTEXT _pgen_tmp_98;
  CIPHERTEXT _pgen_tmp_99;
  POLY _pgen_tmp_100;
  POLY _pgen_tmp_101;
  CIPHERTEXT _pgen_tmp_102;
  PLAINTEXT _pgen_tmp_103;
  CIPHERTEXT _pgen_tmp_104;
  CIPHERTEXT _pgen_tmp_105;
  PLAINTEXT _pgen_tmp_106;
  CIPHERTEXT _pgen_tmp_107;
  CIPHERTEXT _pgen_tmp_108;
  CIPHERTEXT _pgen_tmp_109;
  CIPHERTEXT3 _pgen_tmp_110;
  CIPHERTEXT _pgen_tmp_111;
  POLY _pgen_tmp_112;
  POLY _pgen_tmp_113;
  CIPHERTEXT3 _pgen_tmp_114;
  CIPHERTEXT _pgen_tmp_115;
  CIPHERTEXT _pgen_tmp_116;
  POLY _pgen_tmp_117;
  POLY _pgen_tmp_118;
  CIPHERTEXT3 _pgen_tmp_119;
  CIPHERTEXT _pgen_tmp_120;
  CIPHERTEXT _pgen_tmp_121;
  POLY _pgen_tmp_122;
  POLY _pgen_tmp_123;
  CIPHERTEXT3 _pgen_tmp_124;
  CIPHERTEXT _pgen_tmp_125;
  CIPHERTEXT _pgen_tmp_126;
  POLY _pgen_tmp_127;
  POLY _pgen_tmp_128;
  CIPHERTEXT3 _pgen_tmp_129;
  CIPHERTEXT _pgen_tmp_130;
  CIPHERTEXT _pgen_tmp_131;
  POLY _pgen_tmp_132;
  POLY _pgen_tmp_133;
  CIPHERTEXT _pgen_tmp_134;
  CIPHERTEXT3 _pgen_tmp_135;
  CIPHERTEXT _pgen_tmp_136;
  CIPHERTEXT _pgen_tmp_137;
  CIPHERTEXT _pgen_tmp_138;
  CIPHERTEXT3 _pgen_tmp_139;
  CIPHERTEXT _pgen_tmp_140;
  CIPHERTEXT _pgen_tmp_141;
  CIPHERTEXT _pgen_tmp_142;
  PLAINTEXT _pgen_tmp_143;
  CIPHERTEXT _pgen_tmp_144;
  POLY _pgen_tmp_145;
  POLY _pgen_tmp_146;
  CIPHERTEXT _pgen_tmp_147;
  CIPHERTEXT3 _pgen_tmp_148;
  CIPHERTEXT _pgen_tmp_149;
  CIPHERTEXT _pgen_tmp_150;
  CIPHERTEXT _pgen_tmp_151;
  PLAINTEXT _pgen_tmp_152;
  CIPHERTEXT _pgen_tmp_153;
  POLY _pgen_tmp_154;
  POLY _pgen_tmp_155;
  CIPHERTEXT _pgen_tmp_156;
  PLAINTEXT _pgen_tmp_157;
  CIPHERTEXT _pgen_tmp_158;
  CIPHERTEXT _pgen_tmp_159;
  CIPHERTEXT _pgen_tmp_160;
  POLY _pgen_tmp_161;
  POLY _pgen_tmp_162;
  CIPHERTEXT _pgen_tmp_163;
  CIPHERTEXT3 _pgen_tmp_164;
  CIPHERTEXT _pgen_tmp_165;
  CIPHERTEXT _pgen_tmp_166;
  CIPHERTEXT _pgen_tmp_167;
  CIPHERTEXT3 _pgen_tmp_168;
  CIPHERTEXT _pgen_tmp_169;
  CIPHERTEXT _pgen_tmp_170;
  CIPHERTEXT _pgen_tmp_171;
  PLAINTEXT _pgen_tmp_172;
  CIPHERTEXT _pgen_tmp_173;
  POLY _pgen_tmp_174;
  POLY _pgen_tmp_175;
  CIPHERTEXT _pgen_tmp_176;
  PLAINTEXT _pgen_tmp_177;
  CIPHERTEXT _pgen_tmp_178;
  CIPHERTEXT _pgen_tmp_179;
  POLY _pgen_tmp_180;
  POLY _pgen_tmp_181;
  CIPHERTEXT _pgen_tmp_182;
  CIPHERTEXT3 _pgen_tmp_183;
  CIPHERTEXT _pgen_tmp_184;
  CIPHERTEXT _pgen_tmp_185;
  CIPHERTEXT _pgen_tmp_186;
  PLAINTEXT _pgen_tmp_187;
  CIPHERTEXT _pgen_tmp_188;
  POLY _pgen_tmp_189;
  POLY _pgen_tmp_190;
  CIPHERTEXT _pgen_tmp_191;
  PLAINTEXT _pgen_tmp_192;
  CIPHERTEXT _pgen_tmp_193;
  CIPHERTEXT _pgen_tmp_194;
  CIPHERTEXT _pgen_tmp_195;
  PLAINTEXT _pgen_tmp_196;
  CIPHERTEXT _pgen_tmp_197;
  uint32_t  degree = Degree();
  memset(&_relu_tmp4, 0, sizeof(_relu_tmp4));
  memset(&_relu_tmp8, 0, sizeof(_relu_tmp8));
  memset(&_relu_tmp30, 0, sizeof(_relu_tmp30));
  memset(&_relu_tmp34, 0, sizeof(_relu_tmp34));
  memset(&_relu_tmp38, 0, sizeof(_relu_tmp38));
  memset(&_relu_tmp42, 0, sizeof(_relu_tmp42));
  memset(&_relu_tmp46, 0, sizeof(_relu_tmp46));
  memset(&_relu_tmp86, 0, sizeof(_relu_tmp86));
  memset(&_relu_tmp90, 0, sizeof(_relu_tmp90));
  memset(&_relu_tmp94, 0, sizeof(_relu_tmp94));
  memset(&_relu_tmp98, 0, sizeof(_relu_tmp98));
  memset(&_relu_tmp102, 0, sizeof(_relu_tmp102));
  memset(&_relu_tmp106, 0, sizeof(_relu_tmp106));
  memset(&_relu_tmp146, 0, sizeof(_relu_tmp146));
  memset(&_relu_tmp152, 0, sizeof(_relu_tmp152));
  memset(&_pgen_tmp_0, 0, sizeof(_pgen_tmp_0));
  memset(&_pgen_tmp_3, 0, sizeof(_pgen_tmp_3));
  memset(&_pgen_tmp_4, 0, sizeof(_pgen_tmp_4));
  memset(&_pgen_tmp_5, 0, sizeof(_pgen_tmp_5));
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
  memset(&_pgen_tmp_20, 0, sizeof(_pgen_tmp_20));
  memset(&_pgen_tmp_21, 0, sizeof(_pgen_tmp_21));
  memset(&_pgen_tmp_22, 0, sizeof(_pgen_tmp_22));
  memset(&_pgen_tmp_25, 0, sizeof(_pgen_tmp_25));
  memset(&_pgen_tmp_26, 0, sizeof(_pgen_tmp_26));
  memset(&_pgen_tmp_27, 0, sizeof(_pgen_tmp_27));
  memset(&_pgen_tmp_28, 0, sizeof(_pgen_tmp_28));
  memset(&_pgen_tmp_29, 0, sizeof(_pgen_tmp_29));
  memset(&_pgen_tmp_30, 0, sizeof(_pgen_tmp_30));
  memset(&_pgen_tmp_33, 0, sizeof(_pgen_tmp_33));
  memset(&_pgen_tmp_34, 0, sizeof(_pgen_tmp_34));
  memset(&_pgen_tmp_35, 0, sizeof(_pgen_tmp_35));
  memset(&_pgen_tmp_36, 0, sizeof(_pgen_tmp_36));
  memset(&_pgen_tmp_37, 0, sizeof(_pgen_tmp_37));
  memset(&_pgen_tmp_38, 0, sizeof(_pgen_tmp_38));
  memset(&_pgen_tmp_41, 0, sizeof(_pgen_tmp_41));
  memset(&_pgen_tmp_42, 0, sizeof(_pgen_tmp_42));
  memset(&_pgen_tmp_43, 0, sizeof(_pgen_tmp_43));
  memset(&_pgen_tmp_46, 0, sizeof(_pgen_tmp_46));
  memset(&_pgen_tmp_47, 0, sizeof(_pgen_tmp_47));
  memset(&_pgen_tmp_48, 0, sizeof(_pgen_tmp_48));
  memset(&_pgen_tmp_51, 0, sizeof(_pgen_tmp_51));
  memset(&_pgen_tmp_52, 0, sizeof(_pgen_tmp_52));
  memset(&_pgen_tmp_53, 0, sizeof(_pgen_tmp_53));
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
  memset(&_pgen_tmp_73, 0, sizeof(_pgen_tmp_73));
  memset(&_pgen_tmp_74, 0, sizeof(_pgen_tmp_74));
  memset(&_pgen_tmp_75, 0, sizeof(_pgen_tmp_75));
  memset(&_pgen_tmp_76, 0, sizeof(_pgen_tmp_76));
  memset(&_pgen_tmp_79, 0, sizeof(_pgen_tmp_79));
  memset(&_pgen_tmp_80, 0, sizeof(_pgen_tmp_80));
  memset(&_pgen_tmp_81, 0, sizeof(_pgen_tmp_81));
  memset(&_pgen_tmp_82, 0, sizeof(_pgen_tmp_82));
  memset(&_pgen_tmp_83, 0, sizeof(_pgen_tmp_83));
  memset(&_pgen_tmp_84, 0, sizeof(_pgen_tmp_84));
  memset(&_pgen_tmp_85, 0, sizeof(_pgen_tmp_85));
  memset(&_pgen_tmp_86, 0, sizeof(_pgen_tmp_86));
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
  memset(&_pgen_tmp_114, 0, sizeof(_pgen_tmp_114));
  memset(&_pgen_tmp_115, 0, sizeof(_pgen_tmp_115));
  memset(&_pgen_tmp_116, 0, sizeof(_pgen_tmp_116));
  memset(&_pgen_tmp_119, 0, sizeof(_pgen_tmp_119));
  memset(&_pgen_tmp_120, 0, sizeof(_pgen_tmp_120));
  memset(&_pgen_tmp_121, 0, sizeof(_pgen_tmp_121));
  memset(&_pgen_tmp_124, 0, sizeof(_pgen_tmp_124));
  memset(&_pgen_tmp_125, 0, sizeof(_pgen_tmp_125));
  memset(&_pgen_tmp_126, 0, sizeof(_pgen_tmp_126));
  memset(&_pgen_tmp_129, 0, sizeof(_pgen_tmp_129));
  memset(&_pgen_tmp_130, 0, sizeof(_pgen_tmp_130));
  memset(&_pgen_tmp_131, 0, sizeof(_pgen_tmp_131));
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
  memset(&_pgen_tmp_147, 0, sizeof(_pgen_tmp_147));
  memset(&_pgen_tmp_148, 0, sizeof(_pgen_tmp_148));
  memset(&_pgen_tmp_149, 0, sizeof(_pgen_tmp_149));
  memset(&_pgen_tmp_150, 0, sizeof(_pgen_tmp_150));
  memset(&_pgen_tmp_151, 0, sizeof(_pgen_tmp_151));
  memset(&_pgen_tmp_152, 0, sizeof(_pgen_tmp_152));
  memset(&_pgen_tmp_153, 0, sizeof(_pgen_tmp_153));
  memset(&_pgen_tmp_156, 0, sizeof(_pgen_tmp_156));
  memset(&_pgen_tmp_157, 0, sizeof(_pgen_tmp_157));
  memset(&_pgen_tmp_158, 0, sizeof(_pgen_tmp_158));
  memset(&_pgen_tmp_159, 0, sizeof(_pgen_tmp_159));
  memset(&_pgen_tmp_160, 0, sizeof(_pgen_tmp_160));
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
  memset(&_pgen_tmp_176, 0, sizeof(_pgen_tmp_176));
  memset(&_pgen_tmp_177, 0, sizeof(_pgen_tmp_177));
  memset(&_pgen_tmp_178, 0, sizeof(_pgen_tmp_178));
  memset(&_pgen_tmp_179, 0, sizeof(_pgen_tmp_179));
  memset(&_pgen_tmp_182, 0, sizeof(_pgen_tmp_182));
  memset(&_pgen_tmp_183, 0, sizeof(_pgen_tmp_183));
  memset(&_pgen_tmp_184, 0, sizeof(_pgen_tmp_184));
  memset(&_pgen_tmp_185, 0, sizeof(_pgen_tmp_185));
  memset(&_pgen_tmp_186, 0, sizeof(_pgen_tmp_186));
  memset(&_pgen_tmp_187, 0, sizeof(_pgen_tmp_187));
  memset(&_pgen_tmp_188, 0, sizeof(_pgen_tmp_188));
  memset(&_pgen_tmp_191, 0, sizeof(_pgen_tmp_191));
  memset(&_pgen_tmp_192, 0, sizeof(_pgen_tmp_192));
  memset(&_pgen_tmp_193, 0, sizeof(_pgen_tmp_193));
  memset(&_pgen_tmp_194, 0, sizeof(_pgen_tmp_194));
  memset(&_pgen_tmp_195, 0, sizeof(_pgen_tmp_195));
  memset(&_pgen_tmp_196, 0, sizeof(_pgen_tmp_196));
  memset(&_pgen_tmp_197, 0, sizeof(_pgen_tmp_197));
  Init_ciph3_up_scale(&_pgen_tmp_0, &input_ct, &input_ct);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_0._c0_poly);
    _pgen_tmp_1 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_2 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_1, 0, degree), Coeffs(&input_ct._c0_poly, _pgen_rns_idx, degree), Coeffs(&input_ct._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_2, 0, degree), Coeffs(&input_ct._c1_poly, _pgen_rns_idx, degree), Coeffs(&input_ct._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_0._c0_poly, _pgen_rns_idx, degree), Coeffs(&input_ct._c0_poly, _pgen_rns_idx, degree), Coeffs(&input_ct._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_0._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_1, 0, degree), Coeffs(_pgen_tmp_2, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_0._c2_poly, _pgen_rns_idx, degree), Coeffs(&input_ct._c1_poly, _pgen_rns_idx, degree), Coeffs(&input_ct._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_same_scale_ciph3(&_relu_tmp4, &_pgen_tmp_0);
  {
    _pgen_swk_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_0._c2_poly), 1);
    _pgen_swk_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_0._c2_poly), 1);
    _pgen_ext = Alloc_poly(degree, Poly_level(&_pgen_tmp_0._c2_poly), 1);
    _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_0._c0_poly), 0);
    _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_0._c2_poly), 0);
    _pgen_decomp = Alloc_poly(degree, Poly_level(&_pgen_tmp_0._c0_poly), 0);
    _pgen_swk = Swk(0, 0);
    for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&_pgen_tmp_0._c2_poly); _pgen_part_idx = _pgen_part_idx + 1) {
      Decomp(_pgen_decomp, &_pgen_tmp_0._c2_poly, _pgen_part_idx);
      Mod_up(_pgen_ext, _pgen_decomp, _pgen_part_idx);
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
      _pgen_num_q = Poly_level(&_pgen_tmp_0._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_relu_tmp4._c0_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_0._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_relu_tmp4._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_0._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly(_pgen_swk_c0);
    Free_poly(_pgen_swk_c1);
    Free_poly(_pgen_ext);
    Free_poly(_pgen_tmp_poly);
    Free_poly(_pgen_decomp);
  }
  Init_ciph_down_scale(&_pgen_tmp_4, &_relu_tmp4);
  Rescale(&_pgen_tmp_4._c0_poly, &_relu_tmp4._c0_poly);
  Rescale(&_pgen_tmp_4._c1_poly, &_relu_tmp4._c1_poly);
  Init_ciph_down_scale(&_pgen_tmp_5, &_relu_tmp4);
  Rescale(&_pgen_tmp_5._c0_poly, &_relu_tmp4._c0_poly);
  Rescale(&_pgen_tmp_5._c1_poly, &_relu_tmp4._c1_poly);
  Init_ciph3_up_scale(&_pgen_tmp_3, &_pgen_tmp_4, &_pgen_tmp_5);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_3._c0_poly);
    _pgen_tmp_6 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_7 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_6, 0, degree), Coeffs(&_pgen_tmp_4._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_5._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_7, 0, degree), Coeffs(&_pgen_tmp_4._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_5._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_3._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_4._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_5._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_3._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_6, 0, degree), Coeffs(_pgen_tmp_7, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_3._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_4._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_5._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_same_scale_ciph3(&_relu_tmp8, &_pgen_tmp_3);
  {
    _pgen_swk_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_3._c2_poly), 1);
    _pgen_swk_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_3._c2_poly), 1);
    _pgen_ext = Alloc_poly(degree, Poly_level(&_pgen_tmp_3._c2_poly), 1);
    _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_3._c0_poly), 0);
    _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_3._c2_poly), 0);
    _pgen_decomp = Alloc_poly(degree, Poly_level(&_pgen_tmp_3._c0_poly), 0);
    _pgen_swk = Swk(0, 0);
    for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&_pgen_tmp_3._c2_poly); _pgen_part_idx = _pgen_part_idx + 1) {
      Decomp(_pgen_decomp, &_pgen_tmp_3._c2_poly, _pgen_part_idx);
      Mod_up(_pgen_ext, _pgen_decomp, _pgen_part_idx);
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
      _pgen_num_q = Poly_level(&_pgen_tmp_3._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_relu_tmp8._c0_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_3._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_relu_tmp8._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_3._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly(_pgen_swk_c0);
    Free_poly(_pgen_swk_c1);
    Free_poly(_pgen_ext);
    Free_poly(_pgen_tmp_poly);
    Free_poly(_pgen_decomp);
  }
  Init_ciph_down_scale(&_pgen_tmp_10, &_relu_tmp8);
  Rescale(&_pgen_tmp_10._c0_poly, &_relu_tmp8._c0_poly);
  Rescale(&_pgen_tmp_10._c1_poly, &_relu_tmp8._c1_poly);
  Init_ciph_down_scale(&_pgen_tmp_14, &_relu_tmp4);
  Rescale(&_pgen_tmp_14._c0_poly, &_relu_tmp4._c0_poly);
  Rescale(&_pgen_tmp_14._c1_poly, &_relu_tmp4._c1_poly);
  Encode_float(&_pgen_tmp_16, &_cst_0, 1, Sc_degree(&input_ct), Level(&input_ct));
  Init_ciph_up_scale_plain(&_pgen_tmp_15, &input_ct, &_pgen_tmp_16);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_15._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_15._c0_poly, _pgen_rns_idx, degree), Coeffs(&input_ct._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_16._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_15._c1_poly, _pgen_rns_idx, degree), Coeffs(&input_ct._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_16._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_down_scale(&_pgen_tmp_17, &_pgen_tmp_15);
  Rescale(&_pgen_tmp_17._c0_poly, &_pgen_tmp_15._c0_poly);
  Rescale(&_pgen_tmp_17._c1_poly, &_pgen_tmp_15._c1_poly);
  Init_ciph3_up_scale(&_pgen_tmp_13, &_pgen_tmp_14, &_pgen_tmp_17);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_13._c0_poly);
    _pgen_tmp_18 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_19 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_18, 0, degree), Coeffs(&_pgen_tmp_14._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_17._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_19, 0, degree), Coeffs(&_pgen_tmp_14._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_17._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_13._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_14._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_17._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_13._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_18, 0, degree), Coeffs(_pgen_tmp_19, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_13._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_14._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_17._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_same_scale_ciph3(&_pgen_tmp_12, &_pgen_tmp_13);
  {
    _pgen_swk_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_13._c2_poly), 1);
    _pgen_swk_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_13._c2_poly), 1);
    _pgen_ext = Alloc_poly(degree, Poly_level(&_pgen_tmp_13._c2_poly), 1);
    _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_13._c0_poly), 0);
    _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_13._c2_poly), 0);
    _pgen_decomp = Alloc_poly(degree, Poly_level(&_pgen_tmp_13._c0_poly), 0);
    _pgen_swk = Swk(0, 0);
    for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&_pgen_tmp_13._c2_poly); _pgen_part_idx = _pgen_part_idx + 1) {
      Decomp(_pgen_decomp, &_pgen_tmp_13._c2_poly, _pgen_part_idx);
      Mod_up(_pgen_ext, _pgen_decomp, _pgen_part_idx);
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
      _pgen_num_q = Poly_level(&_pgen_tmp_13._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_12._c0_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_13._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_12._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_13._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly(_pgen_swk_c0);
    Free_poly(_pgen_swk_c1);
    Free_poly(_pgen_ext);
    Free_poly(_pgen_tmp_poly);
    Free_poly(_pgen_decomp);
  }
  Encode_float(&_pgen_tmp_20, &_cst_1, 1, Sc_degree(&input_ct), Level(&input_ct));
  Init_ciph_up_scale_plain(&_pgen_tmp_21, &input_ct, &_pgen_tmp_20);
  Init_ciph_same_scale(&_pgen_tmp_11, &_pgen_tmp_12, &_pgen_tmp_21);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_11._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_21._c0_poly, _pgen_rns_idx, degree), Coeffs(&input_ct._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_20._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_21._c1_poly, _pgen_rns_idx, degree), Coeffs(&input_ct._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_20._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_11._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_12._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_21._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_11._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_12._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_21._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_down_scale(&_pgen_tmp_22, &_pgen_tmp_11);
  Rescale(&_pgen_tmp_22._c0_poly, &_pgen_tmp_11._c0_poly);
  Rescale(&_pgen_tmp_22._c1_poly, &_pgen_tmp_11._c1_poly);
  Init_ciph3_up_scale(&_pgen_tmp_9, &_pgen_tmp_10, &_pgen_tmp_22);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_9._c0_poly);
    _pgen_tmp_23 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_24 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_23, 0, degree), Coeffs(&_pgen_tmp_10._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_22._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_24, 0, degree), Coeffs(&_pgen_tmp_10._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_22._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_9._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_10._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_22._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_9._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_23, 0, degree), Coeffs(_pgen_tmp_24, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_9._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_10._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_22._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_same_scale_ciph3(&_pgen_tmp_8, &_pgen_tmp_9);
  {
    _pgen_swk_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_9._c2_poly), 1);
    _pgen_swk_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_9._c2_poly), 1);
    _pgen_ext = Alloc_poly(degree, Poly_level(&_pgen_tmp_9._c2_poly), 1);
    _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_9._c0_poly), 0);
    _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_9._c2_poly), 0);
    _pgen_decomp = Alloc_poly(degree, Poly_level(&_pgen_tmp_9._c0_poly), 0);
    _pgen_swk = Swk(0, 0);
    for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&_pgen_tmp_9._c2_poly); _pgen_part_idx = _pgen_part_idx + 1) {
      Decomp(_pgen_decomp, &_pgen_tmp_9._c2_poly, _pgen_part_idx);
      Mod_up(_pgen_ext, _pgen_decomp, _pgen_part_idx);
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
      _pgen_num_q = Poly_level(&_pgen_tmp_9._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_8._c0_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_9._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_8._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_9._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly(_pgen_swk_c0);
    Free_poly(_pgen_swk_c1);
    Free_poly(_pgen_ext);
    Free_poly(_pgen_tmp_poly);
    Free_poly(_pgen_decomp);
  }
  Init_ciph_down_scale(&_pgen_tmp_27, &_relu_tmp4);
  Rescale(&_pgen_tmp_27._c0_poly, &_relu_tmp4._c0_poly);
  Rescale(&_pgen_tmp_27._c1_poly, &_relu_tmp4._c1_poly);
  Encode_float(&_pgen_tmp_29, &_cst_2, 1, Sc_degree(&input_ct), Level(&input_ct));
  Init_ciph_up_scale_plain(&_pgen_tmp_28, &input_ct, &_pgen_tmp_29);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_28._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_28._c0_poly, _pgen_rns_idx, degree), Coeffs(&input_ct._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_29._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_28._c1_poly, _pgen_rns_idx, degree), Coeffs(&input_ct._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_29._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_down_scale(&_pgen_tmp_30, &_pgen_tmp_28);
  Rescale(&_pgen_tmp_30._c0_poly, &_pgen_tmp_28._c0_poly);
  Rescale(&_pgen_tmp_30._c1_poly, &_pgen_tmp_28._c1_poly);
  Init_ciph3_up_scale(&_pgen_tmp_26, &_pgen_tmp_27, &_pgen_tmp_30);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_26._c0_poly);
    _pgen_tmp_31 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_32 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_31, 0, degree), Coeffs(&_pgen_tmp_27._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_30._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_32, 0, degree), Coeffs(&_pgen_tmp_27._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_30._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_26._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_27._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_30._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_26._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_31, 0, degree), Coeffs(_pgen_tmp_32, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_26._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_27._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_30._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_same_scale_ciph3(&_pgen_tmp_25, &_pgen_tmp_26);
  {
    _pgen_swk_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_26._c2_poly), 1);
    _pgen_swk_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_26._c2_poly), 1);
    _pgen_ext = Alloc_poly(degree, Poly_level(&_pgen_tmp_26._c2_poly), 1);
    _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_26._c0_poly), 0);
    _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_26._c2_poly), 0);
    _pgen_decomp = Alloc_poly(degree, Poly_level(&_pgen_tmp_26._c0_poly), 0);
    _pgen_swk = Swk(0, 0);
    for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&_pgen_tmp_26._c2_poly); _pgen_part_idx = _pgen_part_idx + 1) {
      Decomp(_pgen_decomp, &_pgen_tmp_26._c2_poly, _pgen_part_idx);
      Mod_up(_pgen_ext, _pgen_decomp, _pgen_part_idx);
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
      _pgen_num_q = Poly_level(&_pgen_tmp_26._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_25._c0_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_26._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_25._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_26._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly(_pgen_swk_c0);
    Free_poly(_pgen_swk_c1);
    Free_poly(_pgen_ext);
    Free_poly(_pgen_tmp_poly);
    Free_poly(_pgen_decomp);
  }
  Encode_float(&_pgen_tmp_33, &_cst_3, 1, Sc_degree(&input_ct), Level(&input_ct));
  Init_ciph_up_scale_plain(&_pgen_tmp_34, &input_ct, &_pgen_tmp_33);
  Init_ciph_same_scale(&_pgen_tmp_35, &_pgen_tmp_25, &_pgen_tmp_34);
  Init_ciph_same_scale(&_relu_tmp30, &_pgen_tmp_8, &_pgen_tmp_35);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_relu_tmp30._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_34._c0_poly, _pgen_rns_idx, degree), Coeffs(&input_ct._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_33._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_34._c1_poly, _pgen_rns_idx, degree), Coeffs(&input_ct._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_33._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_35._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_25._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_34._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_35._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_25._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_34._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_relu_tmp30._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_8._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_35._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_relu_tmp30._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_8._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_35._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_down_scale(&_pgen_tmp_37, &_relu_tmp30);
  Rescale(&_pgen_tmp_37._c0_poly, &_relu_tmp30._c0_poly);
  Rescale(&_pgen_tmp_37._c1_poly, &_relu_tmp30._c1_poly);
  Init_ciph_down_scale(&_pgen_tmp_38, &_relu_tmp30);
  Rescale(&_pgen_tmp_38._c0_poly, &_relu_tmp30._c0_poly);
  Rescale(&_pgen_tmp_38._c1_poly, &_relu_tmp30._c1_poly);
  Init_ciph3_up_scale(&_pgen_tmp_36, &_pgen_tmp_37, &_pgen_tmp_38);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_36._c0_poly);
    _pgen_tmp_39 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_40 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_39, 0, degree), Coeffs(&_pgen_tmp_37._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_38._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_40, 0, degree), Coeffs(&_pgen_tmp_37._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_38._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_36._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_37._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_38._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_36._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_39, 0, degree), Coeffs(_pgen_tmp_40, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_36._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_37._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_38._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_same_scale_ciph3(&_relu_tmp34, &_pgen_tmp_36);
  {
    _pgen_swk_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_36._c2_poly), 1);
    _pgen_swk_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_36._c2_poly), 1);
    _pgen_ext = Alloc_poly(degree, Poly_level(&_pgen_tmp_36._c2_poly), 1);
    _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_36._c0_poly), 0);
    _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_36._c2_poly), 0);
    _pgen_decomp = Alloc_poly(degree, Poly_level(&_pgen_tmp_36._c0_poly), 0);
    _pgen_swk = Swk(0, 0);
    for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&_pgen_tmp_36._c2_poly); _pgen_part_idx = _pgen_part_idx + 1) {
      Decomp(_pgen_decomp, &_pgen_tmp_36._c2_poly, _pgen_part_idx);
      Mod_up(_pgen_ext, _pgen_decomp, _pgen_part_idx);
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
      _pgen_num_q = Poly_level(&_pgen_tmp_36._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_relu_tmp34._c0_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_36._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_relu_tmp34._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_36._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly(_pgen_swk_c0);
    Free_poly(_pgen_swk_c1);
    Free_poly(_pgen_ext);
    Free_poly(_pgen_tmp_poly);
    Free_poly(_pgen_decomp);
  }
  Init_ciph_down_scale(&_pgen_tmp_42, &_relu_tmp30);
  Rescale(&_pgen_tmp_42._c0_poly, &_relu_tmp30._c0_poly);
  Rescale(&_pgen_tmp_42._c1_poly, &_relu_tmp30._c1_poly);
  Init_ciph_down_scale(&_pgen_tmp_43, &_relu_tmp34);
  Rescale(&_pgen_tmp_43._c0_poly, &_relu_tmp34._c0_poly);
  Rescale(&_pgen_tmp_43._c1_poly, &_relu_tmp34._c1_poly);
  Init_ciph3_up_scale(&_pgen_tmp_41, &_pgen_tmp_42, &_pgen_tmp_43);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_41._c0_poly);
    _pgen_tmp_44 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_45 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_44, 0, degree), Coeffs(&_pgen_tmp_42._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_43._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_45, 0, degree), Coeffs(&_pgen_tmp_42._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_43._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_41._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_42._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_43._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_41._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_44, 0, degree), Coeffs(_pgen_tmp_45, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_41._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_42._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_43._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_same_scale_ciph3(&_relu_tmp38, &_pgen_tmp_41);
  {
    _pgen_swk_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_41._c2_poly), 1);
    _pgen_swk_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_41._c2_poly), 1);
    _pgen_ext = Alloc_poly(degree, Poly_level(&_pgen_tmp_41._c2_poly), 1);
    _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_41._c0_poly), 0);
    _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_41._c2_poly), 0);
    _pgen_decomp = Alloc_poly(degree, Poly_level(&_pgen_tmp_41._c0_poly), 0);
    _pgen_swk = Swk(0, 0);
    for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&_pgen_tmp_41._c2_poly); _pgen_part_idx = _pgen_part_idx + 1) {
      Decomp(_pgen_decomp, &_pgen_tmp_41._c2_poly, _pgen_part_idx);
      Mod_up(_pgen_ext, _pgen_decomp, _pgen_part_idx);
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
      _pgen_num_q = Poly_level(&_pgen_tmp_41._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_relu_tmp38._c0_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_41._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_relu_tmp38._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_41._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly(_pgen_swk_c0);
    Free_poly(_pgen_swk_c1);
    Free_poly(_pgen_ext);
    Free_poly(_pgen_tmp_poly);
    Free_poly(_pgen_decomp);
  }
  Init_ciph_down_scale(&_pgen_tmp_47, &_relu_tmp34);
  Rescale(&_pgen_tmp_47._c0_poly, &_relu_tmp34._c0_poly);
  Rescale(&_pgen_tmp_47._c1_poly, &_relu_tmp34._c1_poly);
  Init_ciph_down_scale(&_pgen_tmp_48, &_relu_tmp34);
  Rescale(&_pgen_tmp_48._c0_poly, &_relu_tmp34._c0_poly);
  Rescale(&_pgen_tmp_48._c1_poly, &_relu_tmp34._c1_poly);
  Init_ciph3_up_scale(&_pgen_tmp_46, &_pgen_tmp_47, &_pgen_tmp_48);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_46._c0_poly);
    _pgen_tmp_49 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_50 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_49, 0, degree), Coeffs(&_pgen_tmp_47._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_48._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_50, 0, degree), Coeffs(&_pgen_tmp_47._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_48._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_46._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_47._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_48._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_46._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_49, 0, degree), Coeffs(_pgen_tmp_50, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_46._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_47._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_48._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_same_scale_ciph3(&_relu_tmp42, &_pgen_tmp_46);
  {
    _pgen_swk_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_46._c2_poly), 1);
    _pgen_swk_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_46._c2_poly), 1);
    _pgen_ext = Alloc_poly(degree, Poly_level(&_pgen_tmp_46._c2_poly), 1);
    _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_46._c0_poly), 0);
    _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_46._c2_poly), 0);
    _pgen_decomp = Alloc_poly(degree, Poly_level(&_pgen_tmp_46._c0_poly), 0);
    _pgen_swk = Swk(0, 0);
    for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&_pgen_tmp_46._c2_poly); _pgen_part_idx = _pgen_part_idx + 1) {
      Decomp(_pgen_decomp, &_pgen_tmp_46._c2_poly, _pgen_part_idx);
      Mod_up(_pgen_ext, _pgen_decomp, _pgen_part_idx);
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
      _pgen_num_q = Poly_level(&_pgen_tmp_46._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_relu_tmp42._c0_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_46._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_relu_tmp42._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_46._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly(_pgen_swk_c0);
    Free_poly(_pgen_swk_c1);
    Free_poly(_pgen_ext);
    Free_poly(_pgen_tmp_poly);
    Free_poly(_pgen_decomp);
  }
  Init_ciph_down_scale(&_pgen_tmp_52, &_relu_tmp42);
  Rescale(&_pgen_tmp_52._c0_poly, &_relu_tmp42._c0_poly);
  Rescale(&_pgen_tmp_52._c1_poly, &_relu_tmp42._c1_poly);
  Init_ciph_down_scale(&_pgen_tmp_53, &_relu_tmp42);
  Rescale(&_pgen_tmp_53._c0_poly, &_relu_tmp42._c0_poly);
  Rescale(&_pgen_tmp_53._c1_poly, &_relu_tmp42._c1_poly);
  Init_ciph3_up_scale(&_pgen_tmp_51, &_pgen_tmp_52, &_pgen_tmp_53);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_51._c0_poly);
    _pgen_tmp_54 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_55 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_54, 0, degree), Coeffs(&_pgen_tmp_52._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_53._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_55, 0, degree), Coeffs(&_pgen_tmp_52._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_53._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_51._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_52._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_53._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_51._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_54, 0, degree), Coeffs(_pgen_tmp_55, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_51._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_52._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_53._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_same_scale_ciph3(&_relu_tmp46, &_pgen_tmp_51);
  {
    _pgen_swk_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_51._c2_poly), 1);
    _pgen_swk_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_51._c2_poly), 1);
    _pgen_ext = Alloc_poly(degree, Poly_level(&_pgen_tmp_51._c2_poly), 1);
    _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_51._c0_poly), 0);
    _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_51._c2_poly), 0);
    _pgen_decomp = Alloc_poly(degree, Poly_level(&_pgen_tmp_51._c0_poly), 0);
    _pgen_swk = Swk(0, 0);
    for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&_pgen_tmp_51._c2_poly); _pgen_part_idx = _pgen_part_idx + 1) {
      Decomp(_pgen_decomp, &_pgen_tmp_51._c2_poly, _pgen_part_idx);
      Mod_up(_pgen_ext, _pgen_decomp, _pgen_part_idx);
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
      _pgen_num_q = Poly_level(&_pgen_tmp_51._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_relu_tmp46._c0_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_51._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_relu_tmp46._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_51._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly(_pgen_swk_c0);
    Free_poly(_pgen_swk_c1);
    Free_poly(_pgen_ext);
    Free_poly(_pgen_tmp_poly);
    Free_poly(_pgen_decomp);
  }
  Init_ciph_down_scale(&_pgen_tmp_58, &_relu_tmp46);
  Rescale(&_pgen_tmp_58._c0_poly, &_relu_tmp46._c0_poly);
  Rescale(&_pgen_tmp_58._c1_poly, &_relu_tmp46._c1_poly);
  Init_ciph_down_scale(&_pgen_tmp_62, &_relu_tmp42);
  Rescale(&_pgen_tmp_62._c0_poly, &_relu_tmp42._c0_poly);
  Rescale(&_pgen_tmp_62._c1_poly, &_relu_tmp42._c1_poly);
  Init_ciph_down_scale(&_pgen_tmp_66, &_relu_tmp34);
  Rescale(&_pgen_tmp_66._c0_poly, &_relu_tmp34._c0_poly);
  Rescale(&_pgen_tmp_66._c1_poly, &_relu_tmp34._c1_poly);
  Init_ciph_down_scale(&_pgen_tmp_68, &_relu_tmp30);
  Rescale(&_pgen_tmp_68._c0_poly, &_relu_tmp30._c0_poly);
  Rescale(&_pgen_tmp_68._c1_poly, &_relu_tmp30._c1_poly);
  Encode_float(&_pgen_tmp_69, &_cst_4, 1, Sc_degree(&_pgen_tmp_68), Level(&_pgen_tmp_68));
  Init_ciph_up_scale_plain(&_pgen_tmp_67, &_pgen_tmp_68, &_pgen_tmp_69);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_67._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_67._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_68._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_69._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_67._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_68._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_69._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_down_scale(&_pgen_tmp_70, &_pgen_tmp_67);
  Rescale(&_pgen_tmp_70._c0_poly, &_pgen_tmp_67._c0_poly);
  Rescale(&_pgen_tmp_70._c1_poly, &_pgen_tmp_67._c1_poly);
  Init_ciph3_up_scale(&_pgen_tmp_65, &_pgen_tmp_66, &_pgen_tmp_70);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_65._c0_poly);
    _pgen_tmp_71 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_72 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_71, 0, degree), Coeffs(&_pgen_tmp_66._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_70._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_72, 0, degree), Coeffs(&_pgen_tmp_66._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_70._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_65._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_66._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_70._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_65._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_71, 0, degree), Coeffs(_pgen_tmp_72, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_65._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_66._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_70._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_same_scale_ciph3(&_pgen_tmp_64, &_pgen_tmp_65);
  {
    _pgen_swk_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_65._c2_poly), 1);
    _pgen_swk_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_65._c2_poly), 1);
    _pgen_ext = Alloc_poly(degree, Poly_level(&_pgen_tmp_65._c2_poly), 1);
    _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_65._c0_poly), 0);
    _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_65._c2_poly), 0);
    _pgen_decomp = Alloc_poly(degree, Poly_level(&_pgen_tmp_65._c0_poly), 0);
    _pgen_swk = Swk(0, 0);
    for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&_pgen_tmp_65._c2_poly); _pgen_part_idx = _pgen_part_idx + 1) {
      Decomp(_pgen_decomp, &_pgen_tmp_65._c2_poly, _pgen_part_idx);
      Mod_up(_pgen_ext, _pgen_decomp, _pgen_part_idx);
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
      _pgen_num_q = Poly_level(&_pgen_tmp_65._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_64._c0_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_65._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_64._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_65._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly(_pgen_swk_c0);
    Free_poly(_pgen_swk_c1);
    Free_poly(_pgen_ext);
    Free_poly(_pgen_tmp_poly);
    Free_poly(_pgen_decomp);
  }
  Init_ciph_down_scale(&_pgen_tmp_73, &_relu_tmp30);
  Rescale(&_pgen_tmp_73._c0_poly, &_relu_tmp30._c0_poly);
  Rescale(&_pgen_tmp_73._c1_poly, &_relu_tmp30._c1_poly);
  Encode_float(&_pgen_tmp_74, &_cst_5, 1, Sc_degree(&_pgen_tmp_73), Level(&_pgen_tmp_73));
  Init_ciph_up_scale_plain(&_pgen_tmp_75, &_pgen_tmp_73, &_pgen_tmp_74);
  Init_ciph_same_scale(&_pgen_tmp_63, &_pgen_tmp_64, &_pgen_tmp_75);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_63._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_75._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_73._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_74._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_75._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_73._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_74._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_63._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_64._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_75._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_63._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_64._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_75._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_down_scale(&_pgen_tmp_76, &_pgen_tmp_63);
  Rescale(&_pgen_tmp_76._c0_poly, &_pgen_tmp_63._c0_poly);
  Rescale(&_pgen_tmp_76._c1_poly, &_pgen_tmp_63._c1_poly);
  Init_ciph3_up_scale(&_pgen_tmp_61, &_pgen_tmp_62, &_pgen_tmp_76);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_61._c0_poly);
    _pgen_tmp_77 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_78 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_77, 0, degree), Coeffs(&_pgen_tmp_62._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_76._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_78, 0, degree), Coeffs(&_pgen_tmp_62._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_76._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_61._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_62._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_76._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_61._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_77, 0, degree), Coeffs(_pgen_tmp_78, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_61._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_62._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_76._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_same_scale_ciph3(&_pgen_tmp_60, &_pgen_tmp_61);
  {
    _pgen_swk_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_61._c2_poly), 1);
    _pgen_swk_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_61._c2_poly), 1);
    _pgen_ext = Alloc_poly(degree, Poly_level(&_pgen_tmp_61._c2_poly), 1);
    _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_61._c0_poly), 0);
    _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_61._c2_poly), 0);
    _pgen_decomp = Alloc_poly(degree, Poly_level(&_pgen_tmp_61._c0_poly), 0);
    _pgen_swk = Swk(0, 0);
    for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&_pgen_tmp_61._c2_poly); _pgen_part_idx = _pgen_part_idx + 1) {
      Decomp(_pgen_decomp, &_pgen_tmp_61._c2_poly, _pgen_part_idx);
      Mod_up(_pgen_ext, _pgen_decomp, _pgen_part_idx);
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
      _pgen_num_q = Poly_level(&_pgen_tmp_61._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_60._c0_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_61._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_60._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_61._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly(_pgen_swk_c0);
    Free_poly(_pgen_swk_c1);
    Free_poly(_pgen_ext);
    Free_poly(_pgen_tmp_poly);
    Free_poly(_pgen_decomp);
  }
  Init_ciph_down_scale(&_pgen_tmp_79, &_relu_tmp38);
  Rescale(&_pgen_tmp_79._c0_poly, &_relu_tmp38._c0_poly);
  Rescale(&_pgen_tmp_79._c1_poly, &_relu_tmp38._c1_poly);
  Encode_float(&_pgen_tmp_80, &_cst_7, 1, Sc_degree(&_pgen_tmp_79), Level(&_pgen_tmp_79));
  Init_ciph_up_scale_plain(&_pgen_tmp_81, &_pgen_tmp_79, &_pgen_tmp_80);
  Init_ciph_down_scale(&_pgen_tmp_82, &_relu_tmp30);
  Rescale(&_pgen_tmp_82._c0_poly, &_relu_tmp30._c0_poly);
  Rescale(&_pgen_tmp_82._c1_poly, &_relu_tmp30._c1_poly);
  Encode_float(&_pgen_tmp_83, &_cst_6, 1, Sc_degree(&_pgen_tmp_82), Level(&_pgen_tmp_82));
  Init_ciph_up_scale_plain(&_pgen_tmp_84, &_pgen_tmp_82, &_pgen_tmp_83);
  Init_ciph_same_scale(&_pgen_tmp_85, &_pgen_tmp_81, &_pgen_tmp_84);
  Init_ciph_same_scale(&_pgen_tmp_59, &_pgen_tmp_60, &_pgen_tmp_85);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_59._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_81._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_79._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_80._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_81._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_79._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_80._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_84._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_82._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_83._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_84._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_82._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_83._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_85._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_81._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_84._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_85._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_81._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_84._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_59._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_60._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_85._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_59._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_60._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_85._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_down_scale(&_pgen_tmp_86, &_pgen_tmp_59);
  Rescale(&_pgen_tmp_86._c0_poly, &_pgen_tmp_59._c0_poly);
  Rescale(&_pgen_tmp_86._c1_poly, &_pgen_tmp_59._c1_poly);
  Init_ciph3_up_scale(&_pgen_tmp_57, &_pgen_tmp_58, &_pgen_tmp_86);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_57._c0_poly);
    _pgen_tmp_87 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_88 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_87, 0, degree), Coeffs(&_pgen_tmp_58._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_86._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_88, 0, degree), Coeffs(&_pgen_tmp_58._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_86._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_57._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_58._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_86._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_57._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_87, 0, degree), Coeffs(_pgen_tmp_88, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_57._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_58._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_86._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_same_scale_ciph3(&_pgen_tmp_56, &_pgen_tmp_57);
  {
    _pgen_swk_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_57._c2_poly), 1);
    _pgen_swk_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_57._c2_poly), 1);
    _pgen_ext = Alloc_poly(degree, Poly_level(&_pgen_tmp_57._c2_poly), 1);
    _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_57._c0_poly), 0);
    _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_57._c2_poly), 0);
    _pgen_decomp = Alloc_poly(degree, Poly_level(&_pgen_tmp_57._c0_poly), 0);
    _pgen_swk = Swk(0, 0);
    for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&_pgen_tmp_57._c2_poly); _pgen_part_idx = _pgen_part_idx + 1) {
      Decomp(_pgen_decomp, &_pgen_tmp_57._c2_poly, _pgen_part_idx);
      Mod_up(_pgen_ext, _pgen_decomp, _pgen_part_idx);
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
      _pgen_num_q = Poly_level(&_pgen_tmp_57._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_56._c0_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_57._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_56._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_57._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly(_pgen_swk_c0);
    Free_poly(_pgen_swk_c1);
    Free_poly(_pgen_ext);
    Free_poly(_pgen_tmp_poly);
    Free_poly(_pgen_decomp);
  }
  Init_ciph_down_scale(&_pgen_tmp_91, &_relu_tmp42);
  Rescale(&_pgen_tmp_91._c0_poly, &_relu_tmp42._c0_poly);
  Rescale(&_pgen_tmp_91._c1_poly, &_relu_tmp42._c1_poly);
  Init_ciph_down_scale(&_pgen_tmp_93, &_relu_tmp38);
  Rescale(&_pgen_tmp_93._c0_poly, &_relu_tmp38._c0_poly);
  Rescale(&_pgen_tmp_93._c1_poly, &_relu_tmp38._c1_poly);
  Encode_float(&_pgen_tmp_94, &_cst_9, 1, Sc_degree(&_pgen_tmp_93), Level(&_pgen_tmp_93));
  Init_ciph_up_scale_plain(&_pgen_tmp_95, &_pgen_tmp_93, &_pgen_tmp_94);
  Init_ciph_down_scale(&_pgen_tmp_96, &_relu_tmp30);
  Rescale(&_pgen_tmp_96._c0_poly, &_relu_tmp30._c0_poly);
  Rescale(&_pgen_tmp_96._c1_poly, &_relu_tmp30._c1_poly);
  Encode_float(&_pgen_tmp_97, &_cst_8, 1, Sc_degree(&_pgen_tmp_96), Level(&_pgen_tmp_96));
  Init_ciph_up_scale_plain(&_pgen_tmp_98, &_pgen_tmp_96, &_pgen_tmp_97);
  Init_ciph_same_scale(&_pgen_tmp_92, &_pgen_tmp_95, &_pgen_tmp_98);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_92._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_95._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_93._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_94._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_95._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_93._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_94._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_98._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_96._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_97._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_98._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_96._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_97._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_92._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_95._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_98._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_92._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_95._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_98._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_down_scale(&_pgen_tmp_99, &_pgen_tmp_92);
  Rescale(&_pgen_tmp_99._c0_poly, &_pgen_tmp_92._c0_poly);
  Rescale(&_pgen_tmp_99._c1_poly, &_pgen_tmp_92._c1_poly);
  Init_ciph3_up_scale(&_pgen_tmp_90, &_pgen_tmp_91, &_pgen_tmp_99);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_90._c0_poly);
    _pgen_tmp_100 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_101 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_100, 0, degree), Coeffs(&_pgen_tmp_91._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_99._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_101, 0, degree), Coeffs(&_pgen_tmp_91._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_99._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_90._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_91._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_99._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_90._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_100, 0, degree), Coeffs(_pgen_tmp_101, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_90._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_91._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_99._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_same_scale_ciph3(&_pgen_tmp_89, &_pgen_tmp_90);
  {
    _pgen_swk_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_90._c2_poly), 1);
    _pgen_swk_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_90._c2_poly), 1);
    _pgen_ext = Alloc_poly(degree, Poly_level(&_pgen_tmp_90._c2_poly), 1);
    _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_90._c0_poly), 0);
    _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_90._c2_poly), 0);
    _pgen_decomp = Alloc_poly(degree, Poly_level(&_pgen_tmp_90._c0_poly), 0);
    _pgen_swk = Swk(0, 0);
    for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&_pgen_tmp_90._c2_poly); _pgen_part_idx = _pgen_part_idx + 1) {
      Decomp(_pgen_decomp, &_pgen_tmp_90._c2_poly, _pgen_part_idx);
      Mod_up(_pgen_ext, _pgen_decomp, _pgen_part_idx);
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
      _pgen_num_q = Poly_level(&_pgen_tmp_90._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_89._c0_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_90._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_89._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_90._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly(_pgen_swk_c0);
    Free_poly(_pgen_swk_c1);
    Free_poly(_pgen_ext);
    Free_poly(_pgen_tmp_poly);
    Free_poly(_pgen_decomp);
  }
  Init_ciph_down_scale(&_pgen_tmp_102, &_relu_tmp38);
  Rescale(&_pgen_tmp_102._c0_poly, &_relu_tmp38._c0_poly);
  Rescale(&_pgen_tmp_102._c1_poly, &_relu_tmp38._c1_poly);
  Encode_float(&_pgen_tmp_103, &_cst_11, 1, Sc_degree(&_pgen_tmp_102), Level(&_pgen_tmp_102));
  Init_ciph_up_scale_plain(&_pgen_tmp_104, &_pgen_tmp_102, &_pgen_tmp_103);
  Init_ciph_down_scale(&_pgen_tmp_105, &_relu_tmp30);
  Rescale(&_pgen_tmp_105._c0_poly, &_relu_tmp30._c0_poly);
  Rescale(&_pgen_tmp_105._c1_poly, &_relu_tmp30._c1_poly);
  Encode_float(&_pgen_tmp_106, &_cst_10, 1, Sc_degree(&_pgen_tmp_105), Level(&_pgen_tmp_105));
  Init_ciph_up_scale_plain(&_pgen_tmp_107, &_pgen_tmp_105, &_pgen_tmp_106);
  Init_ciph_same_scale(&_pgen_tmp_108, &_pgen_tmp_104, &_pgen_tmp_107);
  Init_ciph_same_scale(&_pgen_tmp_109, &_pgen_tmp_89, &_pgen_tmp_108);
  Init_ciph_same_scale(&_relu_tmp86, &_pgen_tmp_56, &_pgen_tmp_109);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_relu_tmp86._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_104._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_102._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_103._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_104._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_102._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_103._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_107._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_105._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_106._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_107._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_105._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_106._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_108._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_104._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_107._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_108._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_104._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_107._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_109._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_89._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_108._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_109._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_89._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_108._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_relu_tmp86._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_56._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_109._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_relu_tmp86._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_56._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_109._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_down_scale(&_pgen_tmp_111, &_relu_tmp86);
  Rescale(&_pgen_tmp_111._c0_poly, &_relu_tmp86._c0_poly);
  Rescale(&_pgen_tmp_111._c1_poly, &_relu_tmp86._c1_poly);
  Init_ciph3_up_scale(&_pgen_tmp_110, &input_ct, &_pgen_tmp_111);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_110._c0_poly);
    _pgen_tmp_112 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_113 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_112, 0, degree), Coeffs(&input_ct._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_111._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_113, 0, degree), Coeffs(&input_ct._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_111._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_110._c0_poly, _pgen_rns_idx, degree), Coeffs(&input_ct._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_111._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_110._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_112, 0, degree), Coeffs(_pgen_tmp_113, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_110._c2_poly, _pgen_rns_idx, degree), Coeffs(&input_ct._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_111._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_same_scale_ciph3(&_relu_tmp90, &_pgen_tmp_110);
  {
    _pgen_swk_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_110._c2_poly), 1);
    _pgen_swk_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_110._c2_poly), 1);
    _pgen_ext = Alloc_poly(degree, Poly_level(&_pgen_tmp_110._c2_poly), 1);
    _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_110._c0_poly), 0);
    _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_110._c2_poly), 0);
    _pgen_decomp = Alloc_poly(degree, Poly_level(&_pgen_tmp_110._c0_poly), 0);
    _pgen_swk = Swk(0, 0);
    for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&_pgen_tmp_110._c2_poly); _pgen_part_idx = _pgen_part_idx + 1) {
      Decomp(_pgen_decomp, &_pgen_tmp_110._c2_poly, _pgen_part_idx);
      Mod_up(_pgen_ext, _pgen_decomp, _pgen_part_idx);
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
      _pgen_num_q = Poly_level(&_pgen_tmp_110._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_relu_tmp90._c0_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_110._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_relu_tmp90._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_110._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly(_pgen_swk_c0);
    Free_poly(_pgen_swk_c1);
    Free_poly(_pgen_ext);
    Free_poly(_pgen_tmp_poly);
    Free_poly(_pgen_decomp);
  }
  Init_ciph_down_scale(&_pgen_tmp_115, &_relu_tmp86);
  Rescale(&_pgen_tmp_115._c0_poly, &_relu_tmp86._c0_poly);
  Rescale(&_pgen_tmp_115._c1_poly, &_relu_tmp86._c1_poly);
  Init_ciph_down_scale(&_pgen_tmp_116, &_relu_tmp86);
  Rescale(&_pgen_tmp_116._c0_poly, &_relu_tmp86._c0_poly);
  Rescale(&_pgen_tmp_116._c1_poly, &_relu_tmp86._c1_poly);
  Init_ciph3_up_scale(&_pgen_tmp_114, &_pgen_tmp_115, &_pgen_tmp_116);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_114._c0_poly);
    _pgen_tmp_117 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_118 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_117, 0, degree), Coeffs(&_pgen_tmp_115._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_116._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_118, 0, degree), Coeffs(&_pgen_tmp_115._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_116._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_114._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_115._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_116._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_114._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_117, 0, degree), Coeffs(_pgen_tmp_118, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_114._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_115._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_116._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_same_scale_ciph3(&_relu_tmp94, &_pgen_tmp_114);
  {
    _pgen_swk_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_114._c2_poly), 1);
    _pgen_swk_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_114._c2_poly), 1);
    _pgen_ext = Alloc_poly(degree, Poly_level(&_pgen_tmp_114._c2_poly), 1);
    _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_114._c0_poly), 0);
    _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_114._c2_poly), 0);
    _pgen_decomp = Alloc_poly(degree, Poly_level(&_pgen_tmp_114._c0_poly), 0);
    _pgen_swk = Swk(0, 0);
    for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&_pgen_tmp_114._c2_poly); _pgen_part_idx = _pgen_part_idx + 1) {
      Decomp(_pgen_decomp, &_pgen_tmp_114._c2_poly, _pgen_part_idx);
      Mod_up(_pgen_ext, _pgen_decomp, _pgen_part_idx);
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
      _pgen_num_q = Poly_level(&_pgen_tmp_114._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_relu_tmp94._c0_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_114._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_relu_tmp94._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_114._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly(_pgen_swk_c0);
    Free_poly(_pgen_swk_c1);
    Free_poly(_pgen_ext);
    Free_poly(_pgen_tmp_poly);
    Free_poly(_pgen_decomp);
  }
  Init_ciph_down_scale(&_pgen_tmp_120, &_relu_tmp86);
  Rescale(&_pgen_tmp_120._c0_poly, &_relu_tmp86._c0_poly);
  Rescale(&_pgen_tmp_120._c1_poly, &_relu_tmp86._c1_poly);
  Init_ciph_down_scale(&_pgen_tmp_121, &_relu_tmp94);
  Rescale(&_pgen_tmp_121._c0_poly, &_relu_tmp94._c0_poly);
  Rescale(&_pgen_tmp_121._c1_poly, &_relu_tmp94._c1_poly);
  Init_ciph3_up_scale(&_pgen_tmp_119, &_pgen_tmp_120, &_pgen_tmp_121);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_119._c0_poly);
    _pgen_tmp_122 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_123 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_122, 0, degree), Coeffs(&_pgen_tmp_120._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_121._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_123, 0, degree), Coeffs(&_pgen_tmp_120._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_121._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_119._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_120._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_121._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_119._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_122, 0, degree), Coeffs(_pgen_tmp_123, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_119._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_120._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_121._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_same_scale_ciph3(&_relu_tmp98, &_pgen_tmp_119);
  {
    _pgen_swk_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_119._c2_poly), 1);
    _pgen_swk_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_119._c2_poly), 1);
    _pgen_ext = Alloc_poly(degree, Poly_level(&_pgen_tmp_119._c2_poly), 1);
    _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_119._c0_poly), 0);
    _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_119._c2_poly), 0);
    _pgen_decomp = Alloc_poly(degree, Poly_level(&_pgen_tmp_119._c0_poly), 0);
    _pgen_swk = Swk(0, 0);
    for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&_pgen_tmp_119._c2_poly); _pgen_part_idx = _pgen_part_idx + 1) {
      Decomp(_pgen_decomp, &_pgen_tmp_119._c2_poly, _pgen_part_idx);
      Mod_up(_pgen_ext, _pgen_decomp, _pgen_part_idx);
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
      _pgen_num_q = Poly_level(&_pgen_tmp_119._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_relu_tmp98._c0_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_119._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_relu_tmp98._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_119._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly(_pgen_swk_c0);
    Free_poly(_pgen_swk_c1);
    Free_poly(_pgen_ext);
    Free_poly(_pgen_tmp_poly);
    Free_poly(_pgen_decomp);
  }
  Init_ciph_down_scale(&_pgen_tmp_125, &_relu_tmp94);
  Rescale(&_pgen_tmp_125._c0_poly, &_relu_tmp94._c0_poly);
  Rescale(&_pgen_tmp_125._c1_poly, &_relu_tmp94._c1_poly);
  Init_ciph_down_scale(&_pgen_tmp_126, &_relu_tmp94);
  Rescale(&_pgen_tmp_126._c0_poly, &_relu_tmp94._c0_poly);
  Rescale(&_pgen_tmp_126._c1_poly, &_relu_tmp94._c1_poly);
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
  }
  Init_ciph_same_scale_ciph3(&_relu_tmp102, &_pgen_tmp_124);
  {
    _pgen_swk_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_124._c2_poly), 1);
    _pgen_swk_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_124._c2_poly), 1);
    _pgen_ext = Alloc_poly(degree, Poly_level(&_pgen_tmp_124._c2_poly), 1);
    _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_124._c0_poly), 0);
    _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_124._c2_poly), 0);
    _pgen_decomp = Alloc_poly(degree, Poly_level(&_pgen_tmp_124._c0_poly), 0);
    _pgen_swk = Swk(0, 0);
    for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&_pgen_tmp_124._c2_poly); _pgen_part_idx = _pgen_part_idx + 1) {
      Decomp(_pgen_decomp, &_pgen_tmp_124._c2_poly, _pgen_part_idx);
      Mod_up(_pgen_ext, _pgen_decomp, _pgen_part_idx);
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
      _pgen_num_q = Poly_level(&_pgen_tmp_124._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_relu_tmp102._c0_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_124._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_relu_tmp102._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_124._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly(_pgen_swk_c0);
    Free_poly(_pgen_swk_c1);
    Free_poly(_pgen_ext);
    Free_poly(_pgen_tmp_poly);
    Free_poly(_pgen_decomp);
  }
  Init_ciph_down_scale(&_pgen_tmp_130, &_relu_tmp102);
  Rescale(&_pgen_tmp_130._c0_poly, &_relu_tmp102._c0_poly);
  Rescale(&_pgen_tmp_130._c1_poly, &_relu_tmp102._c1_poly);
  Init_ciph_down_scale(&_pgen_tmp_131, &_relu_tmp102);
  Rescale(&_pgen_tmp_131._c0_poly, &_relu_tmp102._c0_poly);
  Rescale(&_pgen_tmp_131._c1_poly, &_relu_tmp102._c1_poly);
  Init_ciph3_up_scale(&_pgen_tmp_129, &_pgen_tmp_130, &_pgen_tmp_131);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_129._c0_poly);
    _pgen_tmp_132 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_133 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_132, 0, degree), Coeffs(&_pgen_tmp_130._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_131._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_133, 0, degree), Coeffs(&_pgen_tmp_130._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_131._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_129._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_130._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_131._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_129._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_132, 0, degree), Coeffs(_pgen_tmp_133, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_129._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_130._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_131._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_same_scale_ciph3(&_relu_tmp106, &_pgen_tmp_129);
  {
    _pgen_swk_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_129._c2_poly), 1);
    _pgen_swk_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_129._c2_poly), 1);
    _pgen_ext = Alloc_poly(degree, Poly_level(&_pgen_tmp_129._c2_poly), 1);
    _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_129._c0_poly), 0);
    _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_129._c2_poly), 0);
    _pgen_decomp = Alloc_poly(degree, Poly_level(&_pgen_tmp_129._c0_poly), 0);
    _pgen_swk = Swk(0, 0);
    for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&_pgen_tmp_129._c2_poly); _pgen_part_idx = _pgen_part_idx + 1) {
      Decomp(_pgen_decomp, &_pgen_tmp_129._c2_poly, _pgen_part_idx);
      Mod_up(_pgen_ext, _pgen_decomp, _pgen_part_idx);
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
      _pgen_num_q = Poly_level(&_pgen_tmp_129._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_relu_tmp106._c0_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_129._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_relu_tmp106._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_129._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly(_pgen_swk_c0);
    Free_poly(_pgen_swk_c1);
    Free_poly(_pgen_ext);
    Free_poly(_pgen_tmp_poly);
    Free_poly(_pgen_decomp);
  }
  Init_ciph_down_scale(&_pgen_tmp_136, &_relu_tmp106);
  Rescale(&_pgen_tmp_136._c0_poly, &_relu_tmp106._c0_poly);
  Rescale(&_pgen_tmp_136._c1_poly, &_relu_tmp106._c1_poly);
  Init_ciph_down_scale(&_pgen_tmp_140, &_relu_tmp102);
  Rescale(&_pgen_tmp_140._c0_poly, &_relu_tmp102._c0_poly);
  Rescale(&_pgen_tmp_140._c1_poly, &_relu_tmp102._c1_poly);
  Init_ciph_down_scale(&_pgen_tmp_142, &_relu_tmp90);
  Rescale(&_pgen_tmp_142._c0_poly, &_relu_tmp90._c0_poly);
  Rescale(&_pgen_tmp_142._c1_poly, &_relu_tmp90._c1_poly);
  Encode_float(&_pgen_tmp_143, &_cst_12, 1, Sc_degree(&_pgen_tmp_142), Level(&_pgen_tmp_142));
  Init_ciph_up_scale_plain(&_pgen_tmp_141, &_pgen_tmp_142, &_pgen_tmp_143);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_141._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_141._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_142._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_143._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_141._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_142._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_143._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_down_scale(&_pgen_tmp_144, &_pgen_tmp_141);
  Rescale(&_pgen_tmp_144._c0_poly, &_pgen_tmp_141._c0_poly);
  Rescale(&_pgen_tmp_144._c1_poly, &_pgen_tmp_141._c1_poly);
  Init_ciph3_up_scale(&_pgen_tmp_139, &_pgen_tmp_140, &_pgen_tmp_144);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_139._c0_poly);
    _pgen_tmp_145 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_146 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_145, 0, degree), Coeffs(&_pgen_tmp_140._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_144._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_146, 0, degree), Coeffs(&_pgen_tmp_140._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_144._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_139._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_140._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_144._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_139._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_145, 0, degree), Coeffs(_pgen_tmp_146, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_139._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_140._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_144._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_same_scale_ciph3(&_pgen_tmp_138, &_pgen_tmp_139);
  {
    _pgen_swk_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_139._c2_poly), 1);
    _pgen_swk_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_139._c2_poly), 1);
    _pgen_ext = Alloc_poly(degree, Poly_level(&_pgen_tmp_139._c2_poly), 1);
    _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_139._c0_poly), 0);
    _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_139._c2_poly), 0);
    _pgen_decomp = Alloc_poly(degree, Poly_level(&_pgen_tmp_139._c0_poly), 0);
    _pgen_swk = Swk(0, 0);
    for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&_pgen_tmp_139._c2_poly); _pgen_part_idx = _pgen_part_idx + 1) {
      Decomp(_pgen_decomp, &_pgen_tmp_139._c2_poly, _pgen_part_idx);
      Mod_up(_pgen_ext, _pgen_decomp, _pgen_part_idx);
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
      _pgen_num_q = Poly_level(&_pgen_tmp_139._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_138._c0_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_139._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_138._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_139._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly(_pgen_swk_c0);
    Free_poly(_pgen_swk_c1);
    Free_poly(_pgen_ext);
    Free_poly(_pgen_tmp_poly);
    Free_poly(_pgen_decomp);
  }
  Init_ciph_down_scale(&_pgen_tmp_149, &_relu_tmp94);
  Rescale(&_pgen_tmp_149._c0_poly, &_relu_tmp94._c0_poly);
  Rescale(&_pgen_tmp_149._c1_poly, &_relu_tmp94._c1_poly);
  Init_ciph_down_scale(&_pgen_tmp_151, &_relu_tmp90);
  Rescale(&_pgen_tmp_151._c0_poly, &_relu_tmp90._c0_poly);
  Rescale(&_pgen_tmp_151._c1_poly, &_relu_tmp90._c1_poly);
  Encode_float(&_pgen_tmp_152, &_cst_14, 1, Sc_degree(&_pgen_tmp_151), Level(&_pgen_tmp_151));
  Init_ciph_up_scale_plain(&_pgen_tmp_150, &_pgen_tmp_151, &_pgen_tmp_152);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_150._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_150._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_151._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_152._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_150._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_151._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_152._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_down_scale(&_pgen_tmp_153, &_pgen_tmp_150);
  Rescale(&_pgen_tmp_153._c0_poly, &_pgen_tmp_150._c0_poly);
  Rescale(&_pgen_tmp_153._c1_poly, &_pgen_tmp_150._c1_poly);
  Init_ciph3_up_scale(&_pgen_tmp_148, &_pgen_tmp_149, &_pgen_tmp_153);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_148._c0_poly);
    _pgen_tmp_154 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_155 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_154, 0, degree), Coeffs(&_pgen_tmp_149._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_153._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_155, 0, degree), Coeffs(&_pgen_tmp_149._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_153._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_148._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_149._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_153._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_148._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_154, 0, degree), Coeffs(_pgen_tmp_155, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_148._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_149._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_153._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_same_scale_ciph3(&_pgen_tmp_147, &_pgen_tmp_148);
  {
    _pgen_swk_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_148._c2_poly), 1);
    _pgen_swk_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_148._c2_poly), 1);
    _pgen_ext = Alloc_poly(degree, Poly_level(&_pgen_tmp_148._c2_poly), 1);
    _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_148._c0_poly), 0);
    _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_148._c2_poly), 0);
    _pgen_decomp = Alloc_poly(degree, Poly_level(&_pgen_tmp_148._c0_poly), 0);
    _pgen_swk = Swk(0, 0);
    for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&_pgen_tmp_148._c2_poly); _pgen_part_idx = _pgen_part_idx + 1) {
      Decomp(_pgen_decomp, &_pgen_tmp_148._c2_poly, _pgen_part_idx);
      Mod_up(_pgen_ext, _pgen_decomp, _pgen_part_idx);
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
      _pgen_num_q = Poly_level(&_pgen_tmp_148._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_147._c0_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_148._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_147._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_148._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly(_pgen_swk_c0);
    Free_poly(_pgen_swk_c1);
    Free_poly(_pgen_ext);
    Free_poly(_pgen_tmp_poly);
    Free_poly(_pgen_decomp);
  }
  Init_ciph_down_scale(&_pgen_tmp_156, &_relu_tmp90);
  Rescale(&_pgen_tmp_156._c0_poly, &_relu_tmp90._c0_poly);
  Rescale(&_pgen_tmp_156._c1_poly, &_relu_tmp90._c1_poly);
  Encode_float(&_pgen_tmp_157, &_cst_13, 1, Sc_degree(&_pgen_tmp_156), Level(&_pgen_tmp_156));
  Init_ciph_up_scale_plain(&_pgen_tmp_158, &_pgen_tmp_156, &_pgen_tmp_157);
  Init_ciph_same_scale(&_pgen_tmp_159, &_pgen_tmp_147, &_pgen_tmp_158);
  Init_ciph_same_scale(&_pgen_tmp_137, &_pgen_tmp_138, &_pgen_tmp_159);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_137._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_158._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_156._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_157._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_158._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_156._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_157._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_159._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_147._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_158._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_159._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_147._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_158._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_137._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_138._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_159._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_137._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_138._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_159._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_down_scale(&_pgen_tmp_160, &_pgen_tmp_137);
  Rescale(&_pgen_tmp_160._c0_poly, &_pgen_tmp_137._c0_poly);
  Rescale(&_pgen_tmp_160._c1_poly, &_pgen_tmp_137._c1_poly);
  Init_ciph3_up_scale(&_pgen_tmp_135, &_pgen_tmp_136, &_pgen_tmp_160);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_135._c0_poly);
    _pgen_tmp_161 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_162 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_161, 0, degree), Coeffs(&_pgen_tmp_136._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_160._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_162, 0, degree), Coeffs(&_pgen_tmp_136._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_160._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_135._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_136._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_160._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_135._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_161, 0, degree), Coeffs(_pgen_tmp_162, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_135._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_136._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_160._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_same_scale_ciph3(&_pgen_tmp_134, &_pgen_tmp_135);
  {
    _pgen_swk_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_135._c2_poly), 1);
    _pgen_swk_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_135._c2_poly), 1);
    _pgen_ext = Alloc_poly(degree, Poly_level(&_pgen_tmp_135._c2_poly), 1);
    _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_135._c0_poly), 0);
    _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_135._c2_poly), 0);
    _pgen_decomp = Alloc_poly(degree, Poly_level(&_pgen_tmp_135._c0_poly), 0);
    _pgen_swk = Swk(0, 0);
    for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&_pgen_tmp_135._c2_poly); _pgen_part_idx = _pgen_part_idx + 1) {
      Decomp(_pgen_decomp, &_pgen_tmp_135._c2_poly, _pgen_part_idx);
      Mod_up(_pgen_ext, _pgen_decomp, _pgen_part_idx);
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
      _pgen_num_q = Poly_level(&_pgen_tmp_135._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_134._c0_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_135._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_134._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_135._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly(_pgen_swk_c0);
    Free_poly(_pgen_swk_c1);
    Free_poly(_pgen_ext);
    Free_poly(_pgen_tmp_poly);
    Free_poly(_pgen_decomp);
  }
  Init_ciph_down_scale(&_pgen_tmp_165, &_relu_tmp102);
  Rescale(&_pgen_tmp_165._c0_poly, &_relu_tmp102._c0_poly);
  Rescale(&_pgen_tmp_165._c1_poly, &_relu_tmp102._c1_poly);
  Init_ciph_down_scale(&_pgen_tmp_169, &_relu_tmp94);
  Rescale(&_pgen_tmp_169._c0_poly, &_relu_tmp94._c0_poly);
  Rescale(&_pgen_tmp_169._c1_poly, &_relu_tmp94._c1_poly);
  Init_ciph_down_scale(&_pgen_tmp_171, &_relu_tmp90);
  Rescale(&_pgen_tmp_171._c0_poly, &_relu_tmp90._c0_poly);
  Rescale(&_pgen_tmp_171._c1_poly, &_relu_tmp90._c1_poly);
  Encode_float(&_pgen_tmp_172, &_cst_16, 1, Sc_degree(&_pgen_tmp_171), Level(&_pgen_tmp_171));
  Init_ciph_up_scale_plain(&_pgen_tmp_170, &_pgen_tmp_171, &_pgen_tmp_172);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_170._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_170._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_171._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_172._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_170._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_171._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_172._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_down_scale(&_pgen_tmp_173, &_pgen_tmp_170);
  Rescale(&_pgen_tmp_173._c0_poly, &_pgen_tmp_170._c0_poly);
  Rescale(&_pgen_tmp_173._c1_poly, &_pgen_tmp_170._c1_poly);
  Init_ciph3_up_scale(&_pgen_tmp_168, &_pgen_tmp_169, &_pgen_tmp_173);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_168._c0_poly);
    _pgen_tmp_174 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_175 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_174, 0, degree), Coeffs(&_pgen_tmp_169._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_173._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_175, 0, degree), Coeffs(&_pgen_tmp_169._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_173._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_168._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_169._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_173._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_168._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_174, 0, degree), Coeffs(_pgen_tmp_175, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_168._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_169._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_173._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_same_scale_ciph3(&_pgen_tmp_167, &_pgen_tmp_168);
  {
    _pgen_swk_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_168._c2_poly), 1);
    _pgen_swk_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_168._c2_poly), 1);
    _pgen_ext = Alloc_poly(degree, Poly_level(&_pgen_tmp_168._c2_poly), 1);
    _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_168._c0_poly), 0);
    _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_168._c2_poly), 0);
    _pgen_decomp = Alloc_poly(degree, Poly_level(&_pgen_tmp_168._c0_poly), 0);
    _pgen_swk = Swk(0, 0);
    for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&_pgen_tmp_168._c2_poly); _pgen_part_idx = _pgen_part_idx + 1) {
      Decomp(_pgen_decomp, &_pgen_tmp_168._c2_poly, _pgen_part_idx);
      Mod_up(_pgen_ext, _pgen_decomp, _pgen_part_idx);
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
      _pgen_num_q = Poly_level(&_pgen_tmp_168._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_167._c0_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_168._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_167._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_168._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly(_pgen_swk_c0);
    Free_poly(_pgen_swk_c1);
    Free_poly(_pgen_ext);
    Free_poly(_pgen_tmp_poly);
    Free_poly(_pgen_decomp);
  }
  Init_ciph_down_scale(&_pgen_tmp_176, &_relu_tmp90);
  Rescale(&_pgen_tmp_176._c0_poly, &_relu_tmp90._c0_poly);
  Rescale(&_pgen_tmp_176._c1_poly, &_relu_tmp90._c1_poly);
  Encode_float(&_pgen_tmp_177, &_cst_15, 1, Sc_degree(&_pgen_tmp_176), Level(&_pgen_tmp_176));
  Init_ciph_up_scale_plain(&_pgen_tmp_178, &_pgen_tmp_176, &_pgen_tmp_177);
  Init_ciph_same_scale(&_pgen_tmp_166, &_pgen_tmp_167, &_pgen_tmp_178);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_166._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_178._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_176._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_177._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_178._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_176._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_177._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_166._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_167._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_178._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_166._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_167._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_178._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_down_scale(&_pgen_tmp_179, &_pgen_tmp_166);
  Rescale(&_pgen_tmp_179._c0_poly, &_pgen_tmp_166._c0_poly);
  Rescale(&_pgen_tmp_179._c1_poly, &_pgen_tmp_166._c1_poly);
  Init_ciph3_up_scale(&_pgen_tmp_164, &_pgen_tmp_165, &_pgen_tmp_179);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_164._c0_poly);
    _pgen_tmp_180 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_181 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_180, 0, degree), Coeffs(&_pgen_tmp_165._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_179._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_181, 0, degree), Coeffs(&_pgen_tmp_165._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_179._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_164._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_165._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_179._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_164._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_180, 0, degree), Coeffs(_pgen_tmp_181, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_164._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_165._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_179._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_same_scale_ciph3(&_pgen_tmp_163, &_pgen_tmp_164);
  {
    _pgen_swk_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_164._c2_poly), 1);
    _pgen_swk_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_164._c2_poly), 1);
    _pgen_ext = Alloc_poly(degree, Poly_level(&_pgen_tmp_164._c2_poly), 1);
    _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_164._c0_poly), 0);
    _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_164._c2_poly), 0);
    _pgen_decomp = Alloc_poly(degree, Poly_level(&_pgen_tmp_164._c0_poly), 0);
    _pgen_swk = Swk(0, 0);
    for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&_pgen_tmp_164._c2_poly); _pgen_part_idx = _pgen_part_idx + 1) {
      Decomp(_pgen_decomp, &_pgen_tmp_164._c2_poly, _pgen_part_idx);
      Mod_up(_pgen_ext, _pgen_decomp, _pgen_part_idx);
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
      _pgen_num_q = Poly_level(&_pgen_tmp_164._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_163._c0_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_164._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_163._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_164._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly(_pgen_swk_c0);
    Free_poly(_pgen_swk_c1);
    Free_poly(_pgen_ext);
    Free_poly(_pgen_tmp_poly);
    Free_poly(_pgen_decomp);
  }
  Init_ciph_down_scale(&_pgen_tmp_184, &_relu_tmp94);
  Rescale(&_pgen_tmp_184._c0_poly, &_relu_tmp94._c0_poly);
  Rescale(&_pgen_tmp_184._c1_poly, &_relu_tmp94._c1_poly);
  Init_ciph_down_scale(&_pgen_tmp_186, &_relu_tmp90);
  Rescale(&_pgen_tmp_186._c0_poly, &_relu_tmp90._c0_poly);
  Rescale(&_pgen_tmp_186._c1_poly, &_relu_tmp90._c1_poly);
  Encode_float(&_pgen_tmp_187, &_cst_18, 1, Sc_degree(&_pgen_tmp_186), Level(&_pgen_tmp_186));
  Init_ciph_up_scale_plain(&_pgen_tmp_185, &_pgen_tmp_186, &_pgen_tmp_187);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_185._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_185._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_186._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_187._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_185._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_186._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_187._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_down_scale(&_pgen_tmp_188, &_pgen_tmp_185);
  Rescale(&_pgen_tmp_188._c0_poly, &_pgen_tmp_185._c0_poly);
  Rescale(&_pgen_tmp_188._c1_poly, &_pgen_tmp_185._c1_poly);
  Init_ciph3_up_scale(&_pgen_tmp_183, &_pgen_tmp_184, &_pgen_tmp_188);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_pgen_tmp_183._c0_poly);
    _pgen_tmp_189 = Alloc_poly(degree, 1, 0);
    _pgen_tmp_190 = Alloc_poly(degree, 1, 0);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(_pgen_tmp_189, 0, degree), Coeffs(&_pgen_tmp_184._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_188._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(_pgen_tmp_190, 0, degree), Coeffs(&_pgen_tmp_184._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_188._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_183._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_184._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_188._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_183._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_tmp_189, 0, degree), Coeffs(_pgen_tmp_190, 0, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_183._c2_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_184._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_188._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Init_ciph_same_scale_ciph3(&_pgen_tmp_182, &_pgen_tmp_183);
  {
    _pgen_swk_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_183._c2_poly), 1);
    _pgen_swk_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_183._c2_poly), 1);
    _pgen_ext = Alloc_poly(degree, Poly_level(&_pgen_tmp_183._c2_poly), 1);
    _pgen_tmp_poly = Alloc_poly(degree, 1, 0);
    _pgen_mod_down_c0 = Alloc_poly(degree, Poly_level(&_pgen_tmp_183._c0_poly), 0);
    _pgen_mod_down_c1 = Alloc_poly(degree, Poly_level(&_pgen_tmp_183._c2_poly), 0);
    _pgen_decomp = Alloc_poly(degree, Poly_level(&_pgen_tmp_183._c0_poly), 0);
    _pgen_swk = Swk(0, 0);
    for (_pgen_part_idx = 0; _pgen_part_idx < Num_decomp(&_pgen_tmp_183._c2_poly); _pgen_part_idx = _pgen_part_idx + 1) {
      Decomp(_pgen_decomp, &_pgen_tmp_183._c2_poly, _pgen_part_idx);
      Mod_up(_pgen_ext, _pgen_decomp, _pgen_part_idx);
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
      _pgen_num_q = Poly_level(&_pgen_tmp_183._c0_poly);
      for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
        Hw_modadd(Coeffs(&_pgen_tmp_182._c0_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c0, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_183._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        Hw_modadd(Coeffs(&_pgen_tmp_182._c1_poly, _pgen_rns_idx, degree), Coeffs(_pgen_mod_down_c1, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_183._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
        _pgen_modulus = _pgen_modulus + 1;
      }
    }
    Free_poly(_pgen_swk_c0);
    Free_poly(_pgen_swk_c1);
    Free_poly(_pgen_ext);
    Free_poly(_pgen_tmp_poly);
    Free_poly(_pgen_decomp);
  }
  Init_ciph_down_scale(&_pgen_tmp_191, &_relu_tmp90);
  Rescale(&_pgen_tmp_191._c0_poly, &_relu_tmp90._c0_poly);
  Rescale(&_pgen_tmp_191._c1_poly, &_relu_tmp90._c1_poly);
  Encode_float(&_pgen_tmp_192, &_cst_17, 1, Sc_degree(&_pgen_tmp_191), Level(&_pgen_tmp_191));
  Init_ciph_up_scale_plain(&_pgen_tmp_193, &_pgen_tmp_191, &_pgen_tmp_192);
  Init_ciph_same_scale(&_pgen_tmp_194, &_pgen_tmp_182, &_pgen_tmp_193);
  Init_ciph_same_scale(&_pgen_tmp_195, &_pgen_tmp_163, &_pgen_tmp_194);
  Init_ciph_same_scale(&_relu_tmp146, &_pgen_tmp_134, &_pgen_tmp_195);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_relu_tmp146._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_193._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_191._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_192._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_193._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_191._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_192._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_194._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_182._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_193._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_194._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_182._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_193._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_195._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_163._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_194._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_pgen_tmp_195._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_163._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_194._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_relu_tmp146._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_134._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_195._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_relu_tmp146._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_134._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_195._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  Encode_float(&_pgen_tmp_196, &_cst_19, 1, Sc_degree(&input_ct), Level(&input_ct));
  Init_ciph_up_scale_plain(&_pgen_tmp_197, &input_ct, &_pgen_tmp_196);
  Init_ciph_same_scale(&_relu_tmp152, &_relu_tmp146, &_pgen_tmp_197);
  {
    _pgen_modulus = Q_modulus();
    _pgen_num_q = Poly_level(&_relu_tmp152._c0_poly);
    for (_pgen_rns_idx = 0; _pgen_rns_idx < _pgen_num_q; _pgen_rns_idx = _pgen_rns_idx + 1) {
      Hw_modmul(Coeffs(&_pgen_tmp_197._c0_poly, _pgen_rns_idx, degree), Coeffs(&input_ct._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_196._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modmul(Coeffs(&_pgen_tmp_197._c1_poly, _pgen_rns_idx, degree), Coeffs(&input_ct._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_196._poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_relu_tmp152._c0_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp146._c0_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_197._c0_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      Hw_modadd(Coeffs(&_relu_tmp152._c1_poly, _pgen_rns_idx, degree), Coeffs(&_relu_tmp146._c1_poly, _pgen_rns_idx, degree), Coeffs(&_pgen_tmp_197._c1_poly, _pgen_rns_idx, degree), _pgen_modulus, degree);
      _pgen_modulus = _pgen_modulus + 1;
    }
  }
  return _relu_tmp152;
}

CKKS_PARAMS* Get_context_params() {
  static CKKS_PARAMS parm = {
    LIB_ANT, 128, 0, 31, 0, 33, 30, 0, 0, 0,
    {  }
  };
  return &parm;
}

DATA_SCHEME* Get_encode_scheme(int idx) {
  static MAP_DESC desc_0[] = {{NORMAL, 0, 0, 0, 0}};
  static DATA_SCHEME scheme_0 = {
    "input", {0, 0, 0, 0}, 1, desc_0
  };
  static DATA_SCHEME* scheme[] = { &scheme_0 };
  return scheme[idx];
}

DATA_SCHEME* Get_decode_scheme(int idx) {
  static MAP_DESC desc[] = {{NORMAL, 0, 0, 0, 0}};
  static DATA_SCHEME scheme = {
    "output", {0, 0, 0, 0}, 1, desc
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
