//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_PHANTOM_BTS_MGR_H
#define RTLIB_PHANTOM_BTS_MGR_H

//! @file bts_mgr.h
//! @brief Phantom backend owned-state for bootstrap slots.
//!
//! Framework placement note: this is a backend IMPLEMENTATION detail (the
//! `phantom::BtsMgr` cache over `Bootstrapper*`), NOT the vendor-injection
//! point into the framework. It lives under fhe/vendor/phantom/ because
//! `vendor_traits.h` references `phantom::BtsMgr` as one of the trait's
//! type aliases (`using BtsMgr = phantom::BtsMgr;`); moving it out would
//! force the injection header to include from a src/ path. Treat as an
//! implementation header consumed only by vendor_traits.h and context.cu.

#include "boot/Bootstrapper.cuh"

#include <cstdint>
#include <memory>
#include <unordered_map>

namespace phantom
{

enum class BTS_SLOTS : uint32_t
{
    POW_15 = 32768,  // 2^15
    POW_14 = 16384,  // 2^14
    POW_13 = 8192,   // 2^13
    POW_12 = 4096    // 2^12
};

class BtsMgr
{
public:
    Bootstrapper* Get(BTS_SLOTS bts_config)
    {
        auto it = _bts.find(static_cast<uint32_t>(bts_config));
        return (it != _bts.end()) ? it->second.get() : nullptr;
    }

    void Set(BTS_SLOTS bts_config, std::unique_ptr<Bootstrapper> bs)
    {
        _bts[static_cast<uint32_t>(bts_config)] = std::move(bs);
    }

private:
    std::unordered_map<uint32_t, std::unique_ptr<Bootstrapper>> _bts;
};

}  // namespace phantom

#endif  // RTLIB_PHANTOM_BTS_MGR_H
