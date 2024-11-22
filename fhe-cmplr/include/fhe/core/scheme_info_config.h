//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CORE_SCHEME_INFO_CONFIG_H
#define FHE_CORE_SCHEME_INFO_CONFIG_H

#include "air/driver/common_config.h"
#include "air/driver/driver_ctx.h"

namespace fhe {
namespace core {

enum SECURITY_LEVEL : uint16_t {
  HE_STD_NOT_SET     = 0,
  HE_STD_128_CLASSIC = 128,
  HE_STD_192_CLASSIC = 192,
  HE_STD_256_CLASSIC = 256,
  HE_STD_END         = 0xffff,
};

enum SCHEME_INFO_ANA_TRACE_DETAIL {
  TRACE_ANA_OPTION = 0,
  TRACE_ANA_RES    = 1,
};

//! Configuration of SCHEME_INFO_ANA_PASS
class SCHEME_INFO_CONFIG : public air::util::COMMON_CONFIG {
public:
  SCHEME_INFO_CONFIG(void) {}

  void Register_options(air::driver::DRIVER_CTX* ctx);
  void Update_options();

  void           Print(std::ostream& os) const;
  SECURITY_LEVEL Security_level() const { return _security_level; }
  uint64_t       Hamming_weight() const { return _secret_key_hamming_weight; }
  uint64_t       Bit_num_q0() const { return _q0; }
  uint64_t       Bit_num_sf() const { return _sf; }
  // leave this member public so that OPTION_DESC can access it
  SECURITY_LEVEL _security_level            = HE_STD_NOT_SET;
  uint64_t       _secret_key_hamming_weight = 0;
  uint64_t       _q0                        = 0;
  uint64_t       _sf                        = 0;
};

//! @brief Macro to define API to access scheme info config
#define DECLARE_SCHEME_INFO_CONFIG_ACCESS_API(cfg)                       \
  uint64_t       Hamming_weight() const { return cfg.Hamming_weight(); } \
  uint64_t       Bit_num_q0() const { return cfg.Bit_num_q0(); }         \
  uint64_t       Bit_num_sf() const { return cfg.Bit_num_sf(); }         \
  SECURITY_LEVEL Security_level() const { return cfg.Security_level(); } \
  DECLARE_COMMON_CONFIG_ACCESS_API(cfg)

}  // namespace core
}  // namespace fhe

#endif  // FHE_CORE_SCHEME_INFO_CONFIG_H
