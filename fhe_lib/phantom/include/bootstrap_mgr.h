//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef RTLIB_PHANTOM_BOOTSTRAP_MGR_H
#define RTLIB_PHANTOM_BOOTSTRAP_MGR_H

// Standard library
#include <cstdint>
#include <memory>
#include <unordered_map>

// External libraries
#include "boot/Bootstrapper.cuh"

namespace phantom {

enum class BTS_SLOTS : uint32_t {
    POW_15 = 32768,  // 2^15
    POW_14 = 16384,  // 2^14
    POW_13 = 8192,   // 2^13
    POW_12 = 4096    // 2^12
};

class BOOTSTRAP_MGR {
public:
    Bootstrapper* Get(BTS_SLOTS bts_config) {
        auto it = _bts.find(static_cast<uint32_t>(bts_config));
        return (it != _bts.end()) ? it->second.get() : nullptr;
    }

    void Set(BTS_SLOTS bts_config, std::unique_ptr<Bootstrapper> bs) {
        _bts[static_cast<uint32_t>(bts_config)] = std::move(bs);
    }

private:
    std::unordered_map<uint32_t, std::unique_ptr<Bootstrapper>> _bts;
};

}  // namespace phantom

#endif  // RTLIB_PHANTOM_BOOTSTRAP_MGR_H