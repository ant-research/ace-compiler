//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "fhe/sihe/config.h"

#include "gtest/gtest.h"

using namespace fhe::sihe;

// ---------------------------------------------------------------------------
// Parse_relu_vr tests
// ---------------------------------------------------------------------------

TEST(SIHE_CONFIG, ParseReluVr_SingleEntry) {
  SIHE_CONFIG cfg;
  cfg._relu_value_range = "conv1=3.5";
  cfg._relu_value_range_default = 1.0;

  double val = 0.0;
  EXPECT_TRUE(cfg.Parse_relu_vr("conv1", val));
  EXPECT_DOUBLE_EQ(val, 3.5);
}

TEST(SIHE_CONFIG, ParseReluVr_MultipleEntries) {
  SIHE_CONFIG cfg;
  cfg._relu_value_range = "conv1=2.0;conv2=4.0;fc=8.0";
  cfg._relu_value_range_default = 1.0;

  double val = 0.0;
  EXPECT_TRUE(cfg.Parse_relu_vr("conv1", val));
  EXPECT_DOUBLE_EQ(val, 2.0);

  EXPECT_TRUE(cfg.Parse_relu_vr("conv2", val));
  EXPECT_DOUBLE_EQ(val, 4.0);

  EXPECT_TRUE(cfg.Parse_relu_vr("fc", val));
  EXPECT_DOUBLE_EQ(val, 8.0);
}

TEST(SIHE_CONFIG, ParseReluVr_NotFound) {
  SIHE_CONFIG cfg;
  cfg._relu_value_range = "conv1=2.0;conv2=4.0";
  cfg._relu_value_range_default = 1.0;

  double val = 0.0;
  EXPECT_FALSE(cfg.Parse_relu_vr("conv3", val));
}

TEST(SIHE_CONFIG, ParseReluVr_EmptyString) {
  SIHE_CONFIG cfg;
  cfg._relu_value_range = "";
  cfg._relu_value_range_default = 1.0;

  double val = 0.0;
  EXPECT_FALSE(cfg.Parse_relu_vr("conv1", val));
}

TEST(SIHE_CONFIG, ParseReluVr_NullName) {
  SIHE_CONFIG cfg;
  cfg._relu_value_range = "conv1=2.0";
  cfg._relu_value_range_default = 1.0;

  double val = 0.0;
  EXPECT_FALSE(cfg.Parse_relu_vr(nullptr, val));
}

TEST(SIHE_CONFIG, ParseReluVr_NamePrefix) {
  // "conv1" should not match "conv10"
  SIHE_CONFIG cfg;
  cfg._relu_value_range = "conv1=2.0;conv10=5.0";
  cfg._relu_value_range_default = 1.0;

  double val = 0.0;
  EXPECT_TRUE(cfg.Parse_relu_vr("conv1", val));
  EXPECT_DOUBLE_EQ(val, 2.0);

  EXPECT_TRUE(cfg.Parse_relu_vr("conv10", val));
  EXPECT_DOUBLE_EQ(val, 5.0);
}

TEST(SIHE_CONFIG, ParseReluVr_NegativeValue) {
  SIHE_CONFIG cfg;
  cfg._relu_value_range = "relu1=-3.0";
  cfg._relu_value_range_default = 1.0;

  double val = 0.0;
  // std::isnormal(-3.0) is true, negative values are valid
  EXPECT_TRUE(cfg.Parse_relu_vr("relu1", val));
  EXPECT_DOUBLE_EQ(val, -3.0);
}

TEST(SIHE_CONFIG, ParseReluVr_ZeroValue) {
  SIHE_CONFIG cfg;
  cfg._relu_value_range = "relu1=0.0";
  cfg._relu_value_range_default = 1.0;

  double val = 0.0;
  // std::isnormal(0.0) is false, so zero is rejected
  EXPECT_FALSE(cfg.Parse_relu_vr("relu1", val));
}

TEST(SIHE_CONFIG, ParseReluVr_TrailingSemicolon) {
  SIHE_CONFIG cfg;
  cfg._relu_value_range = "conv1=2.0;";
  cfg._relu_value_range_default = 1.0;

  double val = 0.0;
  EXPECT_TRUE(cfg.Parse_relu_vr("conv1", val));
  EXPECT_DOUBLE_EQ(val, 2.0);
}

// ---------------------------------------------------------------------------
// Relu_vr tests (uses Parse_relu_vr + default fallback)
// ---------------------------------------------------------------------------

TEST(SIHE_CONFIG, ReluVr_FoundEntry) {
  SIHE_CONFIG cfg;
  cfg._relu_value_range = "conv1=3.0;conv2=5.0";
  cfg._relu_value_range_default = 1.0;

  EXPECT_DOUBLE_EQ(cfg.Relu_vr("conv1"), 3.0);
  EXPECT_DOUBLE_EQ(cfg.Relu_vr("conv2"), 5.0);
}

TEST(SIHE_CONFIG, ReluVr_FallbackToDefault) {
  SIHE_CONFIG cfg;
  cfg._relu_value_range = "conv1=3.0";
  cfg._relu_value_range_default = 1.0;

  // "conv2" not in per-name list, should return default
  EXPECT_DOUBLE_EQ(cfg.Relu_vr("conv2"), 1.0);
}

TEST(SIHE_CONFIG, ReluVr_NullName) {
  SIHE_CONFIG cfg;
  cfg._relu_value_range = "conv1=3.0";
  cfg._relu_value_range_default = 1.0;

  EXPECT_DOUBLE_EQ(cfg.Relu_vr(nullptr), 1.0);
}

TEST(SIHE_CONFIG, ReluVr_EmptyRange) {
  SIHE_CONFIG cfg;
  cfg._relu_value_range = "";
  cfg._relu_value_range_default = 1.0;

  EXPECT_DOUBLE_EQ(cfg.Relu_vr("conv1"), 1.0);
}

TEST(SIHE_CONFIG, ReluVr_DefaultZeroClampedToOne) {
  SIHE_CONFIG cfg;
  cfg._relu_value_range = "";
  cfg._relu_value_range_default = 0.0;

  // Update_options clamps zero/near-zero default to 1.0
  cfg.Update_options();
  EXPECT_DOUBLE_EQ(cfg._relu_value_range_default, 1.0);
  EXPECT_DOUBLE_EQ(cfg.Relu_vr("conv1"), 1.0);
}

// ---------------------------------------------------------------------------
// Has_relu_vr tests
// ---------------------------------------------------------------------------

TEST(SIHE_CONFIG, HasReluVr_EntryExists) {
  SIHE_CONFIG cfg;
  cfg._relu_value_range = "conv1=3.0;conv2=5.0";

  EXPECT_TRUE(cfg.Has_relu_vr("conv1"));
  EXPECT_TRUE(cfg.Has_relu_vr("conv2"));
}

TEST(SIHE_CONFIG, HasReluVr_EntryNotFound) {
  SIHE_CONFIG cfg;
  cfg._relu_value_range = "conv1=3.0";

  EXPECT_FALSE(cfg.Has_relu_vr("conv3"));
}

TEST(SIHE_CONFIG, HasReluVr_EmptyRange) {
  SIHE_CONFIG cfg;
  cfg._relu_value_range = "";

  EXPECT_FALSE(cfg.Has_relu_vr("conv1"));
}

TEST(SIHE_CONFIG, HasReluVr_NullName) {
  SIHE_CONFIG cfg;
  cfg._relu_value_range = "conv1=3.0";

  EXPECT_FALSE(cfg.Has_relu_vr(nullptr));
}

// ---------------------------------------------------------------------------
// Priority: CLI per-name > AIR node attr > CLI default
// (This tests the logic in tensor2sihe_impl.h Handle_relu;
//  here we test the SIHE_CONFIG building blocks that enable it.)
// ---------------------------------------------------------------------------

TEST(SIHE_CONFIG, Priority_CliPerNameUsedWhenPresent) {
  SIHE_CONFIG cfg;
  cfg._relu_value_range = "relu1=4.0";  // CLI per-name override
  cfg._relu_value_range_default = 1.0;    // CLI default

  // Has_relu_vr("relu1") returns true -> CLI per-name takes priority
  EXPECT_TRUE(cfg.Has_relu_vr("relu1"));
  EXPECT_DOUBLE_EQ(cfg.Relu_vr("relu1"), 4.0);
}

TEST(SIHE_CONFIG, Priority_CliDefaultUsedWhenNoPerName) {
  SIHE_CONFIG cfg;
  cfg._relu_value_range = "relu1=4.0";  // only relu1 has per-name override
  cfg._relu_value_range_default = 1.0;

  // Has_relu_vr("relu2") returns false -> caller should fall back to AIR attr,
  // and finally to CLI default
  EXPECT_FALSE(cfg.Has_relu_vr("relu2"));
  EXPECT_DOUBLE_EQ(cfg.Relu_vr("relu2"), 1.0);  // CLI default
}

// ---------------------------------------------------------------------------
// Edge cases
// ---------------------------------------------------------------------------

TEST(SIHE_CONFIG, ParseReluVr_SameNameDifferentSuffix) {
  SIHE_CONFIG cfg;
  cfg._relu_value_range = "relu=2.0;relu1=3.0;relu10=6.0";
  cfg._relu_value_range_default = 1.0;

  EXPECT_DOUBLE_EQ(cfg.Relu_vr("relu"), 2.0);
  EXPECT_DOUBLE_EQ(cfg.Relu_vr("relu1"), 3.0);
  EXPECT_DOUBLE_EQ(cfg.Relu_vr("relu10"), 6.0);
}

TEST(SIHE_CONFIG, ParseReluVr_IntegerValue) {
  SIHE_CONFIG cfg;
  cfg._relu_value_range = "conv1=5";

  double val = 0.0;
  // Integer values are valid (isnormal(5.0) is true)
  EXPECT_TRUE(cfg.Parse_relu_vr("conv1", val));
  EXPECT_DOUBLE_EQ(val, 5.0);
}

TEST(SIHE_CONFIG, ParseReluVr_FractionalValue) {
  SIHE_CONFIG cfg;
  cfg._relu_value_range = "conv1=0.125";

  double val = 0.0;
  EXPECT_TRUE(cfg.Parse_relu_vr("conv1", val));
  EXPECT_DOUBLE_EQ(val, 0.125);
}
