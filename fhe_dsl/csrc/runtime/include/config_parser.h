//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef ACE_RUNTIME_CONFIG_PARSER_H
#define ACE_RUNTIME_CONFIG_PARSER_H

#include <pybind11/pybind11.h>

namespace ace {
namespace runtime {

/**
 * @brief Parse configuration from Python dict and set runtime configuration.
 *
 * This function parses the Python dictionary containing FHE configuration
 * and stores it in the global CONFIG_MANAGER. It also registers the
 * configuration callbacks with the runtime library.
 *
 * @param config_dict Python dictionary containing configuration data
 * @throws std::runtime_error if parsing fails or required fields are missing
 */
void Parse_and_set_config(const pybind11::dict& config_dict);

} // namespace runtime
} // namespace ace

#endif // ACE_RUNTIME_CONFIG_PARSER_H