//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef FHE_CORE_LIB_PROVIDER_H
#define FHE_CORE_LIB_PROVIDER_H

namespace fhe {

namespace core {

//! @brief Define all supported library in following format:
//! DEFINE_PROVIDER(enum_id, cmd_line_opts, name_in_C_code)
#define ALL_PROVIDERS()                     \
  DEFINE_PROVIDER(ANT, "ant", "LIB_ANT")    \
  DEFINE_PROVIDER(SEAL, "seal", "LIB_SEAL") \
  DEFINE_PROVIDER(OPENFHE, "openfhe", "LIB_OPENFHE")

//! @brief Define enum for all supported library
enum class PROVIDER : uint32_t {
#define DEFINE_PROVIDER(id, opt, name) id,
  ALL_PROVIDERS()
#undef DEFINE_PROVIDER
      INVALID
};

//! @brief Get enum from command line options
static inline PROVIDER Provider_id(const char* opt) {
  static const char* names[(int)PROVIDER::INVALID] = {
#define DEFINE_PROVIDER(id, opt, name) opt,
      ALL_PROVIDERS()
#undef DEFINE_PROVIDER
  };
  if (opt == nullptr || *opt == '\0') {
    // use ANT by default
    return PROVIDER::ANT;
  }
  for (int i = 0; i < sizeof(names) / sizeof(names[0]); ++i) {
    if (strcmp(opt, names[i]) == 0) {
      return (PROVIDER)i;
    }
  }
  AIR_ASSERT_MSG(false, "invalid lib provider");
  return PROVIDER::INVALID;
}

//! @brief Get library name to be written to C code
static inline const char* Provider_name(PROVIDER id) {
  static const char* names[(int)PROVIDER::INVALID] = {
#define DEFINE_PROVIDER(id, opt, name) name,
      ALL_PROVIDERS()
#undef DEFINE_PROVIDER
  };
  if (id < PROVIDER::INVALID) {
    return names[(int)id];
  }
  AIR_ASSERT_MSG(false, "invalid lib provider");
  return nullptr;
}

//! @brief Get header file name to be written to C code
static inline const char* Provider_header(PROVIDER id) {
  static const char* names[(int)PROVIDER::INVALID] = {
#define DEFINE_PROVIDER(id, opt, name) "rt_" opt "/rt_" opt ".h",
      ALL_PROVIDERS()
#undef DEFINE_PROVIDER
  };
  if (id < PROVIDER::INVALID) {
    return names[(int)id];
  }
  AIR_ASSERT_MSG(false, "invalid lib provider");
  return nullptr;
}

}  // namespace core

}  // namespace fhe

#endif  // FHE_CORE_LIB_PROVIDER_H
