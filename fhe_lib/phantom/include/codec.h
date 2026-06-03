//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_PHANTOM_CODEC_H
#define RTLIB_PHANTOM_CODEC_H

// Standard library
#include <memory>

// External libraries
#include "rt_phantom/phantom_api.h"
#include "rt_phantom/rt_phantom.h"

namespace phantom {

struct CODEC {
    std::unique_ptr<PhantomCKKSEncoder> _encoder;
    std::unique_ptr<CKKSEvaluator> _evaluator;
};

}  // namespace phantom

#endif  // RTLIB_PHANTOM_CODEC_H