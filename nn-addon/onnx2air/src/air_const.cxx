//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn/onnx2air/air_const.h"

#include "nn/onnx2air/air_gen.h"
#include "nn/onnx2air/air_stmt.h"
#include "nn/onnx2air/air_utils.h"

namespace nn {
namespace onnx2air {

int AIRCONSTGEN::Data_elem_size(int32_t data_type) {
  std::unordered_map<int32_t, int> onnx_type_map = {
      {onnx::TensorProto_DataType_FLOAT,  sizeof(float)   },
      {onnx::TensorProto_DataType_DOUBLE, sizeof(double)  },
      {onnx::TensorProto_DataType_INT8,   sizeof(int8_t)  },
      {onnx::TensorProto_DataType_UINT8,  sizeof(uint8_t) },
      {onnx::TensorProto_DataType_INT16,  sizeof(int16_t) },
      {onnx::TensorProto_DataType_UINT16, sizeof(uint16_t)},
      {onnx::TensorProto_DataType_INT32,  sizeof(int32_t) },
      {onnx::TensorProto_DataType_UINT32, sizeof(uint32_t)},
      {onnx::TensorProto_DataType_INT64,  sizeof(int64_t) },
      {onnx::TensorProto_DataType_UINT64, sizeof(uint64_t)},
      {onnx::TensorProto_DataType_BOOL,   sizeof(bool)    }
  };

  auto got = onnx_type_map.find(data_type);

  if (got == onnx_type_map.end())
    AIR_ASSERT_MSG(false, "unhandled tensor data type in tensor");

  return got->second;
}

size_t AIRCONSTGEN::Parse_offset_or_length(const std::string& value) {
  char*  end              = nullptr;
  size_t offset_or_length = strtoull(value.c_str(), &end, 0);
  return offset_or_length;
}

void AIRCONSTGEN::Read_external_data(const onnx::TensorProto& tensor) {
  std::string location;
  uint64_t    offset = 0;
  int64_t     length = -1;
  for (const onnx::StringStringEntryProto& entry : tensor.external_data()) {
    if (entry.key() == "location") {
      location = entry.value();
    } else if (entry.key() == "offset") {
      offset = Parse_offset_or_length(entry.value());
    } else if (entry.key() == "length") {
      length = Parse_offset_or_length(entry.value());
    }
  }

  std::string separator = "/";
  std::string current_data_file_name =
      _airgen->Directory_path() + separator + location;
  // external data is specified by current_data_file_name, offset and length.
  // TODO: provide this for const data.
}
CONSTANT_PTR
AIRCONSTGEN::Convert_const(const onnx::TensorProto& tensor, TYPE_PTR var_type) {
  int64_t data_num_elements = 1;
  for (int dim : tensor.dims()) {
    data_num_elements *= dim;
  }
  int32_t datatype = tensor.data_type();
  if (onnx::TensorProto_DataType_IsValid(datatype) == false)
    AIR_ASSERT_MSG(false, "Non-valid data type");
  onnx::TensorProto_DataType data_type =
      static_cast<onnx::TensorProto_DataType>(datatype);

  ARRAY_TYPE_PTR& aty    = var_type->Cast_to_arr();
  TYPE_PTR        ele_ty = aty->Elem_type();

  size_t byte_len = data_num_elements * Data_elem_size(tensor.data_type());
  std::unique_ptr<char[]> data_buffer(new char[byte_len]);
  if (tensor.has_raw_data()) {
    std::string raw_data = tensor.raw_data();
    memcpy(data_buffer.get(), raw_data.c_str(), raw_data.size());
  } else if (tensor.has_data_location() &&
             tensor.data_location() == onnx::TensorProto::EXTERNAL) {
    // The data_location of tensor shows that the data is external.
    Read_external_data(tensor);
  } else {
    AIR_ASSERT_MSG(false,
                   "Catch the case where the data is not in raw format.");
    switch (datatype) {
      case onnx::TensorProto_DataType_INT8:
        for (int i = 0; i < data_num_elements; i++)
          ((int8_t*)data_buffer.get())[i] = tensor.int32_data(i);
        break;
      case onnx::TensorProto_DataType_UINT8:
        for (int i = 0; i < data_num_elements; i++)
          ((uint8_t*)data_buffer.get())[i] = tensor.int32_data(i);
        break;
      case onnx::TensorProto_DataType_INT16:
        for (int i = 0; i < data_num_elements; i++)
          ((int16_t*)data_buffer.get())[i] = tensor.int32_data(i);
        break;
      case onnx::TensorProto_DataType_UINT16:
        for (int i = 0; i < data_num_elements; i++)
          ((uint16_t*)data_buffer.get())[i] = tensor.int32_data(i);
        break;
      case onnx::TensorProto_DataType_INT32:
        for (int i = 0; i < data_num_elements; i++)
          ((int32_t*)data_buffer.get())[i] = tensor.int32_data(i);
        break;
      case onnx::TensorProto_DataType_UINT32:
        for (int i = 0; i < data_num_elements; i++)
          ((uint32_t*)data_buffer.get())[i] = tensor.int32_data(i);
        break;
      case onnx::TensorProto_DataType_INT64:
        for (int i = 0; i < data_num_elements; i++)
          ((int64_t*)data_buffer.get())[i] = tensor.int64_data(i);
        break;
      case onnx::TensorProto_DataType_UINT64:
        for (int i = 0; i < data_num_elements; i++)
          ((uint64_t*)data_buffer.get())[i] = tensor.uint64_data(i);
        break;
      case onnx::TensorProto_DataType_FLOAT:
        for (int i = 0; i < data_num_elements; i++)
          ((float*)data_buffer.get())[i] = tensor.float_data(i);
        break;
      default:
        AIR_ASSERT_MSG(false, "unhandled tensor data type in tensor ");
        break;
    };
  }
  CONSTANT_PTR cst = _airgen->Get_glob()->New_const(
      CONSTANT_KIND::ARRAY, var_type, data_buffer.get(), byte_len);
  return cst;
}
}  // namespace onnx2air
}  // namespace nn
