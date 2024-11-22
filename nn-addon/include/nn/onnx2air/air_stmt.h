//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_STMT_H
#define AIR_STMT_H

#include <unordered_set>

#include "nn/onnx2air/air_gen.h"

namespace nn {
namespace onnx2air {

// Declare a list for all implemented onnx operators. Format:
//   DEF_OPERATOR(Name, name)
//   where Name is used in ONNX model and name is used in AIRSTMTGEN
// To support new ONNX operators, just need to add a line below
// and implement the corresponding AIRSTMTGEN methods in cxx file

#define DECL_ONNC_OPERATOR()                           \
  DEF_OPERATOR(Add, add)                               \
  DEF_OPERATOR(AveragePool, average_pool)              \
  DEF_OPERATOR(Constant, constant)                     \
  DEF_OPERATOR(Conv, conv)                             \
  DEF_OPERATOR(Flatten, flatten)                       \
  DEF_OPERATOR(Gemm, gemm)                             \
  DEF_OPERATOR(GlobalAveragePool, global_average_pool) \
  DEF_OPERATOR(MaxPool, max_pool)                      \
  DEF_OPERATOR(Mul, mul)                               \
  DEF_OPERATOR(Relu, relu)                             \
  DEF_OPERATOR(Reshape, reshape)

class AIRGEN;
/**
 * @brief Interface of generating the statement for tensor operator.
 *
 */
class AIRSTMTGEN {
public:
  AIRSTMTGEN(AIRGEN* airgen) : _airgen(airgen) { Init(); }

  ~AIRSTMTGEN() {}
  bool     Convert_stmts(FUNC_SCOPE* func_scope, onnx::GraphProto& onnx_graph);
  bool     Try_resolve_node(onnx::NodeProto* node, FUNC_SCOPE* func_scope);
  bool     Get_node_input_tensors(onnx::NodeProto*       node,
                                  std::vector<NODE_PTR>& inputs);
  NODE_PTR Create_node(std::string OP_name, std::vector<NODE_PTR>& input,
                       FUNC_SCOPE* func_scope);
  void     Resolve(onnx::NodeProto* node, std::vector<NODE_PTR>& inputs,
                   TYPE_PTR& base_ty, std::vector<int>& res);
  std::vector<int> Resolve_util(TYPE_PTR elem_ty, TYPE_PTR& base_ty);

  CONSTANT_PTR       Parse_attribute_tensor(onnx::NodeProto*            node,
                                            const onnx::AttributeProto& a);
  int                Parse_attribute_int(const onnx::AttributeProto& a);
  std::vector<int>   Parse_attribute_ints(const onnx::AttributeProto& a);
  std::vector<float> Parse_attribute_floats(const onnx::AttributeProto& a);
  float              Parse_attribute_float(const onnx::AttributeProto& a);
  void               Parse_attributes(onnx::NodeProto* node);
  void    Get_dimension_size(TYPE_PTR input_ty, std::vector<int>& dims);
  AIRGEN* Get_airgen() { return _airgen; }
  void    Set_attribute_float(std::string attr_name, NODE_PTR node);
  void    Set_attribute_floats(std::string attr_name, NODE_PTR node);
  void    Set_attribute_int(std::string attr_name, NODE_PTR node);
  void    Set_attribute_ints(std::string attr_name, NODE_PTR node);
  int     Get_attribute_int(std::string attr_name);
  std::vector<int>   Get_attribute_ints(std::string attr_name);
  float              Get_attribute_float(std::string attr_name);
  std::vector<float> Get_attribute_floats(std::string attr_name);
  std::vector<int>   Resolve_strides(ARRAY_TYPE_PTR& aty);
  std::vector<int>   Resolve_dilations(ARRAY_TYPE_PTR& aty);
  std::vector<int>   Resolve_pads(ARRAY_TYPE_PTR& aty);
  std::vector<int>   Resolve_kernel_shape(ARRAY_TYPE_PTR& aty);
  std::vector<int>   Resolve_conv_output_size(ARRAY_TYPE_PTR&   xty,
                                              ARRAY_TYPE_PTR&   aty,
                                              std::vector<int>& strides,
                                              std::vector<int>& dilations,
                                              std::vector<int>& pads,
                                              std::vector<int>& kernel_shape);
  std::vector<int>   Resolve_pooling_output_size(ARRAY_TYPE_PTR&   xty,
                                                 std::vector<int>& strides,
                                                 std::vector<int>& dilations,
                                                 std::vector<int>& pads,
                                                 std::vector<int>& kernel_shape);

  void Update_attributes(onnx::NodeProto* onnx_node, NODE_PTR node);

private:
  AIRSTMTGEN(void);               // REQUIRED UNDEFINED UNWANTED methods
  AIRSTMTGEN(const AIRSTMTGEN&);  // REQUIRED UNDEFINED UNWANTED methods
  AIRSTMTGEN& operator=(
      const AIRSTMTGEN&);  // REQUIRED UNDEFINED UNWANTED methods

  AIRGEN*                                             _airgen;
  std::unordered_set<std::string>                     _nodes;
  std::unordered_map<std::string, CONSTANT_PTR>       _attributes_for_tensor;
  std::unordered_map<std::string, int>                _attributes_for_int;
  std::unordered_map<std::string, float>              _attributes_for_float;
  std::unordered_map<std::string, std::vector<int>>   _attributes_for_ints;
  std::unordered_map<std::string, std::vector<float>> _attributes_for_floats;
  std::unordered_map<std::string, std::string>        _attributes_for_string;
  uint32_t Get_num_dim_from_tensor_ty(ARRAY_TYPE_PTR& aty);

  // initialize the handler map
  void Init() {
    // expand the ONNX_OPERATOR macro to init the handler map
#define DEF_OPERATOR(NAME, name)                                           \
  _onx_func_map[#NAME]._create_node = &AIRSTMTGEN::Create_node_for_##name; \
  _onx_func_map[#NAME]._parse_attrs =                                      \
      &AIRSTMTGEN::Parse_attributes_for_##name;                            \
  _onx_func_map[#NAME]._resolve = &AIRSTMTGEN::Resolve_for_##name;         \
  _onx_func_map[#NAME]._update_attrs =                                     \
      &AIRSTMTGEN::Update_attributes_for_##name;
    DECL_ONNC_OPERATOR()
#undef DEF_OPERATOR
  }

  // expand the ONNX_OPERATOR macro to generate declarations
#define DEF_OPERATOR(NAME, name)                                             \
  void Create_node_for_##name(CONTAINER* cntr, std::vector<NODE_PTR>& input, \
                              const SPOS& spos, NODE_PTR& op_node);          \
  void Parse_attributes_for_##name(onnx::NodeProto* node);                   \
  void Resolve_for_##name(onnx::NodeProto*       node,                       \
                          std::vector<NODE_PTR>& inputs, TYPE_PTR& base_ty,  \
                          std::vector<int>& res);                            \
  void Update_attributes_for_##name(NODE_PTR node);
  DECL_ONNC_OPERATOR()
#undef DEF_OPERATOR

  // for unimplemented ONNX operators
  void Unimplemented();

  // handler map
  typedef void (AIRSTMTGEN::*ONX_OPERATION_PARSE)(onnx::NodeProto* node);
  typedef void (AIRSTMTGEN::*ONX_OPERATION_RESOLVE)(
      onnx::NodeProto* node, std::vector<NODE_PTR>& inputs, TYPE_PTR& base_ty,
      std::vector<int>& res);
  typedef void (AIRSTMTGEN::*ONX_OPERATION_UPDATE_ATTRS)(NODE_PTR node);
  typedef void (AIRSTMTGEN::*ONX_OPERATION_CREATE_NODE)(
      CONTAINER* cntr, std::vector<NODE_PTR>& input, const SPOS& spos,
      NODE_PTR& op_node);
  struct ONNX_FUNC_POINTERS {
    ONX_OPERATION_CREATE_NODE  _create_node;
    ONX_OPERATION_PARSE        _parse_attrs;
    ONX_OPERATION_RESOLVE      _resolve;
    ONX_OPERATION_UPDATE_ATTRS _update_attrs;
  };
  std::unordered_map<std::string, ONNX_FUNC_POINTERS> _onx_func_map;
};

}  // namespace onnx2air
}  // namespace nn
#endif
