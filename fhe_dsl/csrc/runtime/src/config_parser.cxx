//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include <pybind11/stl.h>

#include "ace/runtime/config_parser.h"
#include "ace/runtime/config_manager.h"
#include "ace/runtime/rtlib_interface.h"

namespace ace {
namespace runtime {

namespace {

//! @brief Helper: Create CKKS_PARAMS with flexible array
CKKS_PARAMS* create_ckks_params_with_rot_idxs(size_t num_rot_idxs) {
  // Allocate memory: struct size + flexible array size
  size_t total_size = sizeof(CKKS_PARAMS) + num_rot_idxs * sizeof(int32_t);
  CKKS_PARAMS* params = static_cast<CKKS_PARAMS*>(std::malloc(total_size));

  if (!params) {
      throw std::runtime_error("Failed to allocate CKKS_PARAMS");
  }

  // Initialize to zero
  std::memset(params, 0, total_size);
  params->_num_rot_idx = num_rot_idxs;

  return params;
}

//! @brief Helper: Create CKKS_PARAMS from Python dict (handle flexible array)
CKKS_PARAMS* create_ckks_params_from_dict(const pybind11::dict& params_dict) {
  size_t num_rot_idxs = 0;

  // First get _num_rot_idx or infer from _rot_idxs list
  if (params_dict.contains("_num_rot_idx")) {
      num_rot_idxs = params_dict["_num_rot_idx"].cast<size_t>();
  } else if (params_dict.contains("_rot_idxs")) {
      pybind11::list rot_list = params_dict["_rot_idxs"].cast<pybind11::list>();
      num_rot_idxs = rot_list.size();
  }

  // Allocate memory with flexible array
  CKKS_PARAMS* params = create_ckks_params_with_rot_idxs(num_rot_idxs);

  try {
      // Fill basic fields
      if (params_dict.contains("_provider")) {
          params->_provider = static_cast<LIB_PROV>(params_dict["_provider"].cast<int>());
      }
      if (params_dict.contains("_poly_degree")) {
          params->_poly_degree = params_dict["_poly_degree"].cast<uint32_t>();
      }
      if (params_dict.contains("_sec_level")) {
          params->_sec_level = params_dict["_sec_level"].cast<size_t>();
      }
      if (params_dict.contains("_mul_depth")) {
          params->_mul_depth = params_dict["_mul_depth"].cast<size_t>();
      }
      if (params_dict.contains("_input_level")) {
          params->_input_level = params_dict["_input_level"].cast<size_t>();
      }
      if (params_dict.contains("_first_mod_size")) {
          params->_first_mod_size = params_dict["_first_mod_size"].cast<size_t>();
      }
      if (params_dict.contains("_scaling_mod_size")) {
          params->_scaling_mod_size = params_dict["_scaling_mod_size"].cast<size_t>();
      }
      if (params_dict.contains("_num_q_parts")) {
          params->_num_q_parts = params_dict["_num_q_parts"].cast<size_t>();
      }
      if (params_dict.contains("_hamming_weight")) {
          params->_hamming_weight = params_dict["_hamming_weight"].cast<size_t>();
      }

      // Fill flexible array _rot_idxs
      if (params_dict.contains("_rot_idxs")) {
          pybind11::list rot_list = params_dict["_rot_idxs"].cast<pybind11::list>();
          if (rot_list.size() != params->_num_rot_idx) {
              free(params);
              throw std::runtime_error("_rot_idxs size mismatch with _num_rot_idx");
          }

          for (size_t i = 0; i < rot_list.size(); ++i) {
              params->_rot_idxs[i] = rot_list[i].cast<int32_t>();
          }
      }

  } catch (...) {
      free(params);
      throw;
  }

  return params;
}

//! @brief Helper: Create SHAPE from Python list
SHAPE create_shape_from_list(const pybind11::list& shape_list) {
  if (shape_list.size() != 4) {
      throw std::runtime_error("SHAPE list must have exactly 4 elements [N, C, H, W]");
  }

  SHAPE shape;
  shape._n = shape_list[0].cast<size_t>();
  shape._c = shape_list[1].cast<size_t>();
  shape._h = shape_list[2].cast<size_t>();
  shape._w = shape_list[3].cast<size_t>();

  return shape;
}

//! @brief Helper: Create MAP_DESC from Python dict
MAP_DESC create_map_desc_from_dict(const pybind11::dict& desc_dict) {
  MAP_DESC desc;

  // Check required fields
  std::vector<std::string> required_fields = {"_kind", "_count", "_start", "_pad", "_stride"};
  for (const auto& field : required_fields) {
      if (!desc_dict.contains(field.c_str())) {
          throw std::runtime_error("MAP_DESC missing field: " + field);
      }
  }

  desc._kind = static_cast<MAP_KIND>(desc_dict["_kind"].cast<int>());
  desc._count = desc_dict["_count"].cast<int>();
  desc._start = desc_dict["_start"].cast<int>();
  desc._pad = desc_dict["_pad"].cast<int>();
  desc._stride = desc_dict["_stride"].cast<int>();

  return desc;
}

//! @brief Helper: Create DATA_SCHEME from Python dict
DATA_SCHEME create_data_scheme_from_dict(const pybind11::dict& scheme_dict) {
  DATA_SCHEME scheme = {0};

  // Check required fields
  std::vector<std::string> required_fields = {"_name", "_shape", "_count", "_desc"};
  for (const auto& field : required_fields) {
      if (!scheme_dict.contains(field.c_str())) {
          throw std::runtime_error("DATA_SCHEME missing field: " + field);
      }
  }

  // Handle _name (copy string)
  std::string name_str = scheme_dict["_name"].cast<std::string>();
  scheme._name = strdup(name_str.c_str());

  // Handle _shape (now a list [N, C, H, W])
  if (pybind11::isinstance<pybind11::list>(scheme_dict["_shape"])) {
      pybind11::list shape_list = scheme_dict["_shape"].cast<pybind11::list>();
      scheme._shape = create_shape_from_list(shape_list);
  } else {
      free((void*)scheme._name);
      throw std::runtime_error("_shape must be a list [N, C, H, W]");
  }

  // Handle _count
  scheme._count = scheme_dict["_count"].cast<int>();

  // Handle _desc (array)
  pybind11::list desc_list = scheme_dict["_desc"].cast<pybind11::list>();
  if (desc_list.size() != static_cast<size_t>(scheme._count)) {
      free((void*)scheme._name);
      throw std::runtime_error("_desc list size != _count");
  }

  // Allocate and fill _desc array
  scheme._desc = new MAP_DESC[scheme._count];
  for (size_t i = 0; i < desc_list.size(); ++i) {
      pybind11::dict desc_dict = desc_list[i].cast<pybind11::dict>();
      scheme._desc[i] = create_map_desc_from_dict(desc_dict);
  }

  return scheme;
}

RT_DATA_INFO create_rt_data_info_from_dict(
    const pybind11::dict& weight_dict,
    std::string& out_file_name,
    std::string& out_file_uuid) {
  RT_DATA_INFO info;

  if (weight_dict.contains("_file_name")) {
      out_file_name = weight_dict["_file_name"].cast<std::string>();
      info._file_name = out_file_name.c_str();
  } else {
      info._file_name = nullptr;
  }

  if (weight_dict.contains("_file_uuid")) {
      out_file_uuid = weight_dict["_file_uuid"].cast<std::string>();
      info._file_uuid = out_file_uuid.c_str();
  } else {
      info._file_uuid = nullptr;
  }

  if (weight_dict.contains("_entry_type")) {
      int entry_type_int = weight_dict["_entry_type"].cast<int>();

      // Validate range
      if (entry_type_int < 0 || entry_type_int > 2) {
          throw std::runtime_error("Invalid entry_type: must be 0, 1, or 2");
      }

      info._entry_type = static_cast<DATA_ENTRY_TYPE>(entry_type_int);
  } else {
      info._entry_type = DE_MSG_F32;
  }

  return info;
}

} // anonymous namespace

//! @brief Main function: Set runtime configuration (handle uintptr_t)
void Parse_and_set_config(const pybind11::dict& config_dict) {
  // Allow reconfiguration: clean up previous config's heap memory
  if (CONFIG_MANAGER::Instance().Is_configured()) {
      CONFIG_MANAGER::Instance().Reset_config();
  }

  RUNTIME_CONFIG config;

  std::vector<std::string> required_fields = {"input_count", "output_count", "context_params", "decode_schemes", "encode_schemes", "rt_data_info"};
  for (const auto& field : required_fields) {
      if (!config_dict.contains(field.c_str())) {
          throw std::runtime_error("RUNTIME_CONFIG missing field: " + field);
      }
  }

  // Setting Input/Output
  config._input_cnt = config_dict["input_count"].cast<int>();
  config._output_cnt = config_dict["output_count"].cast<int>();

  // Setting CKKS Params
  auto params_dict = config_dict["context_params"].cast<pybind11::dict>();
  CKKS_PARAMS* params = create_ckks_params_from_dict(params_dict);
  // Convert to uintptr_t for storage
  config._ctx_param = reinterpret_cast<uintptr_t>(params);

  // Setting Encoding
  auto encode_list = config_dict["encode_schemes"].cast<pybind11::list>();
  for (auto item : encode_list) {
      config._encode_sch.push_back(create_data_scheme_from_dict(item.cast<pybind11::dict>()));
  }

  // Setting Decoding
  auto decode_list = config_dict["decode_schemes"].cast<pybind11::list>();
  for (auto item : decode_list) {
      config._decode_sch.push_back(create_data_scheme_from_dict(item.cast<pybind11::dict>()));
  }

  // Setting Weight info
  auto weight_dict = config_dict["rt_data_info"].cast<pybind11::dict>();
  config._weight_info = create_rt_data_info_from_dict(
      weight_dict, config._weight_file_name, config._weight_file_uuid);

  CONFIG_MANAGER::Instance().Set_config(config);
  CONFIG_MANAGER::Instance().Freeze_config();

  Register_config_func();
}

bool is_fhe_configured() {
  return CONFIG_MANAGER::Instance().Is_configured();
}

} // namespace runtime
} // namespace ace