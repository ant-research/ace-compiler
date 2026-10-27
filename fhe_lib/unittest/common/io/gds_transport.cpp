//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief test_gds_transport.cpp
//! Unit tests for GdsTransport and GDS driver availability.

#include "fhe/io/gds_transport.h"

#include "fhe/io/cufile_api.h"
#include "fhe/io/gds_io.h"

#include <gtest/gtest.h>

using namespace rtio;

//=============================================================================
// GdsTransport — host-only check
//=============================================================================

TEST(GdsTransportTest, GdsDriverAvailabilityCheck)
{
    bool available = gds_driver_available();
    (void)available;
}
