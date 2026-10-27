//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

//! @brief enc_mgr_api.cu
//! C-linkage wrappers for EncMgr (ACE backend).
//! All dispatch logic is in the common template — these shims just forward.

#include "common/pt_mgr.h"
#include "fhe/context.h"
#include "fhe/vendor/ace/vendor_traits.h"

#define ENC_MGR_CTX_TYPE fhert::FheContext<ace::VendorTraits>
#define ENC_MGR_PT_TYPE ace::VendorTraits::Plaintext

#include "fhe/enc/enc_mgr_api_impl.h"
