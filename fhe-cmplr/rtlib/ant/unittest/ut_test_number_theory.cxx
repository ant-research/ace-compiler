//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "gtest/gtest.h"
#include "util/fhe_utils.h"
#include "util/number_theory.h"

TEST(number_theory, mod_exp) {
  EXPECT_EQ(Mod_exp(12312, 53, 9393333), 2678490);
  EXPECT_EQ(Mod_exp(3880, 391, 9000), 1000);
  EXPECT_EQ(Mod_exp(-1, 432413, 88), 87);
  EXPECT_EQ(Mod_exp(1230911482, 268435451, 8796092858369), 3778006589735);
}

TEST(number_theory, fast_mod_exp) {
  MODULUS mod1, mod2, mod3, mod4;
  Init_modulus(&mod1, 9393333);
  Init_modulus(&mod2, 9000);
  Init_modulus(&mod3, 9999);
  Init_modulus(&mod4, 8796092858369);
  EXPECT_EQ(Fast_mod_exp(12312, 53, &mod1), 2678490);
  EXPECT_EQ(Fast_mod_exp(3880, 391, &mod2), 1000);
  EXPECT_EQ(Fast_mod_exp(2, 432413, &mod3), 1526);
  EXPECT_EQ(Fast_mod_exp(1230911482, 268435451, &mod4), 3778006589735);
}

TEST(number_theory, mul_int64_mod_barret) {
  MODULUS mod1;
  Init_modulus(&mod1, 1001);
  EXPECT_EQ(Mul_int64_mod_barret(12312, 53, &mod1),
            Mul_int64_with_mod(12312, 53, 1001));
}

TEST(number_theory, mod_inv_prime) {
  MODULUS mod1, mod2, mod3;
  Init_modulus(&mod1, 19);
  Init_modulus(&mod2, 103);
  Init_modulus(&mod3, 97);
  EXPECT_EQ(Mod_inv_prime(7, &mod1), 11);
  EXPECT_EQ(Mod_inv_prime(43, &mod2), 12);
  EXPECT_EQ(Mod_inv_prime(94, &mod3), 32);
}

TEST(number_theory, mod_inv) {
  MODULUS mod1, mod2, mod3, mod4;
  Init_modulus(&mod1, 19);
  Init_modulus(&mod2, 103);
  Init_modulus(&mod3, 97);
  Init_modulus(&mod4, 32);
  EXPECT_EQ(Mod_inv(7, &mod1), 11);
  EXPECT_EQ(Mod_inv(43, &mod2), 12);
  EXPECT_EQ(Mod_inv(94, &mod3), 32);
  EXPECT_EQ(Mod_inv(25, &mod4), 9);
}

TEST(number_theory, find_generator) {
  MODULUS mod1, mod2, mod3;
  Init_modulus(&mod1, 5);
  Init_modulus(&mod2, 7);
  Init_modulus(&mod3, 11);
  EXPECT_EQ(Find_generator(&mod1), 2);
  EXPECT_EQ(Find_generator(&mod2), 3);
  EXPECT_EQ(Find_generator(&mod3), 2);
}

TEST(number_theory, root_of_unity) {
  MODULUS mod1, mod2, mod3;
  Init_modulus(&mod1, 5);
  Init_modulus(&mod2, 7);
  Init_modulus(&mod3, 11);
  EXPECT_EQ(Root_of_unity(2, &mod1), 4);
  EXPECT_EQ(Root_of_unity(3, &mod2), 2);
  EXPECT_EQ(Root_of_unity(5, &mod3), 4);
}

TEST(number_theory, is_prime) {
  EXPECT_TRUE(Is_prime(2));
  EXPECT_TRUE(Is_prime(3));
  EXPECT_TRUE(Is_prime(5));
  EXPECT_TRUE(Is_prime(7));
  EXPECT_TRUE(Is_prime(11));
  EXPECT_FALSE(Is_prime(12));
  EXPECT_FALSE(Is_prime(14));
  EXPECT_FALSE(Is_prime(15));
  EXPECT_FALSE(Is_prime(21));
  EXPECT_FALSE(Is_prime(25));

  EXPECT_TRUE(Is_prime(7919));
  EXPECT_FALSE(Is_prime(7921));

  EXPECT_TRUE(Is_prime(0x7fffffd8001));
  EXPECT_TRUE(Is_prime(0x7fffffc8001));
  EXPECT_TRUE(Is_prime(0xfffffffc001));
  EXPECT_TRUE(Is_prime(0xffffff6c001));
}

TEST(number_theory, precompute_const_128) {
  UINT128_T prec = Precompute_const_128(576460752298835969ULL);
  EXPECT_EQ(prec >> 64, 32);
  EXPECT_EQ((uint64_t)prec, 4697619456);
}