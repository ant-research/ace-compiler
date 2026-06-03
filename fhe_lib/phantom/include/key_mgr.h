//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_PHANTOM_KEY_MGR_H
#define RTLIB_PHANTOM_KEY_MGR_H

// Standard library
#include <memory>

// External libraries
#include "rt_phantom/phantom_api.h"
#include "rt_phantom/rt_phantom.h"

namespace phantom {

class KEY_MANAGER {
public:
    void Init(bool with_bootstrap);

    // Reference accessors
    const PhantomSecretKey& Secret_key() const { return *_sk; }
    const PhantomPublicKey& Public_key() const { return *_pk; }
    const PhantomRelinKey& Relin_key() const { return *_rlk; }
    const PhantomGaloisKey& Rotate_key() const { return *_rtk; }

    PhantomRelinKey& Relin_key() { return *_rlk; }
    PhantomGaloisKey& Rotate_key() { return *_rtk; }

// Low-level context access (for direct kernel calls)
    PhantomContext* ctx() { return _ctx.get(); }
    const PhantomContext* ctx() const { return _ctx.get(); }

private:
    PhantomSecretKey* sk() { return _sk.get(); }
    PhantomPublicKey* pk() { return _pk.get(); }
    PhantomRelinKey* rlk() { return _rlk.get(); }
    PhantomGaloisKey* rtk() { return _rtk.get(); }

    std::unique_ptr<PhantomContext> _ctx;
    std::unique_ptr<PhantomSecretKey> _sk;
    std::unique_ptr<PhantomPublicKey> _pk;
    std::unique_ptr<PhantomRelinKey> _rlk;
    std::unique_ptr<PhantomGaloisKey> _rtk;

    friend class PHANTOM_CONTEXT;
};

}  // namespace phantom

#endif  // RTLIB_PHANTOM_KEY_MGR_H