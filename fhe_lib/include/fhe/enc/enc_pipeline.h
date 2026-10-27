//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief enc_pipeline.h
//! Templated offline weight encoding pipeline for FHE runtime backends.
//!
//! EncPipeline<VendorTraits> converts DE_MSG_F32 weight files to
//! DE_PLAINTEXT pre-encoded files using a vendor's CKKS encoder.

#ifndef FHERT_ENC_PIPELINE_H
#define FHERT_ENC_PIPELINE_H

#include <cstddef>
#include <string>

namespace fhert
{

//=============================================================================
// EncPipeline<VendorTraits>
//=============================================================================

template <typename Traits> class EncPipeline
{
public:
    using ContextT = typename Traits::Context;
    using Plaintext = typename Traits::Plaintext;

    /// Encode a weight file: DE_MSG_F32 input → DE_PLAINTEXT output.
    /// Uses the encode-only context (no key generation).
    ///
    /// @param input_file   Path to DE_MSG_F32 weight file
    /// @param output_file  Path to write DE_PLAINTEXT weight file
    /// @return true on success
    static bool encode_file(const std::string& input_file, const std::string& output_file);
};

}  // namespace fhert

#endif  // FHERT_ENC_PIPELINE_H
