//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#include "nn/onnx2air/air_stmt.h"

#include <cmath>

#include "air/base/meta_info.h"
#include "nn/core/opcode.h"
#include "nn/onnx2air/air_func.h"
#include "nn/onnx2air/air_gen.h"
#include "nn/onnx2air/air_utils.h"

namespace nn {
namespace onnx2air {

bool AIRSTMTGEN::Convert_stmts(FUNC_SCOPE*       func_scope,
                               onnx::GraphProto& onnx_graph) {
  uint32_t num_unresolved_prev_round;
  uint32_t num_unresolved = onnx_graph.node_size();

  do {
    num_unresolved_prev_round = num_unresolved;
    num_unresolved            = 0;

    for (onnx::NodeProto n : onnx_graph.node()) {
      bool res = Try_resolve_node(&n, func_scope);
      if (res == false) num_unresolved++;
    }
    if (num_unresolved == 0) break;

  } while (num_unresolved < num_unresolved_prev_round);

  CONTAINER*                   cntr = &func_scope->Container();
  STMT_LIST                    sl   = cntr->Stmt_list();
  std::vector<ADDR_DATUM_PTR>& outs = Get_airgen()->Sg().Output_vars();
  AIR_ASSERT_MSG(outs.size() <= 1,
                 "Number of output tensor is assumed to be one");
  // If there is no output variable, the compiler will not generate
  // any return statement at the end of the function
  if (outs.size() == 0) {
    CMPLR_USR_MSG(U_CODE::No_Output_Var,
                  func_scope->Owning_func()->Name()->Char_str());
    return false;
  }

  ADDR_DATUM_PTR out_st_ptr = outs[0];
  SPOS           spos       = Get_airgen()->Get_glob()->Unknown_simple_spos();
  NODE_PTR       ldid       = cntr->New_ld(out_st_ptr, spos);
  STMT_PTR       ret_stmt   = cntr->New_retv(ldid, spos);
  sl.Append(ret_stmt);
  return true;
}

bool AIRSTMTGEN::Get_node_input_tensors(onnx::NodeProto*       node,
                                        std::vector<NODE_PTR>& inputs) {
  FUNC_SCOPE* func_scope = Get_airgen()->Get_func_scope();
  CONTAINER*  cntr       = &func_scope->Container();
  SPOS        spos       = Get_airgen()->Get_glob()->Unknown_simple_spos();
  for (auto tensor_name : node->input()) {
    bool     input_resolved = false;
    NAME_MAP st_ptr         = Get_airgen()->Sg().Get_result(tensor_name);
    if (st_ptr.Is_cst()) {
      input_resolved = true;
      inputs.push_back(cntr->New_ldc(st_ptr.Cst(), spos));
    } else if (st_ptr.Is_sym()) {
      input_resolved = true;
      inputs.push_back(cntr->New_ld(st_ptr.Sym(), spos));
    } else if (st_ptr.Is_preg()) {
      input_resolved = true;
      inputs.push_back(cntr->New_ldp(st_ptr.Preg(), spos));
    }
    if (input_resolved == false) {
      return false;
    }
  }

  return true;
}

CONSTANT_PTR AIRSTMTGEN::Parse_attribute_tensor(onnx::NodeProto* node,
                                                const onnx::AttributeProto& a) {
  if (a.type() != onnx::AttributeProto_AttributeType_TENSOR)
    AIR_ASSERT_MSG(false, "Attribute type is not tensor");
  if (a.has_t() == false) AIR_ASSERT_MSG(false, "No tensor in attribute");
  FUNC_SCOPE* func_scope = Get_airgen()->Get_func_scope();
  CONTAINER*  cntr       = &func_scope->Container();
  SPOS        spos       = Get_airgen()->Get_glob()->Unknown_simple_spos();

  CONSTANT_PTR cst = Get_airgen()->Sg().Convert_init_sym(a.t());

  return cst;
}

float AIRSTMTGEN::Parse_attribute_float(const onnx::AttributeProto& a) {
  if (a.has_f() == false) AIR_ASSERT_MSG(false, ("Bad attribute "));
  return a.f();
}

int AIRSTMTGEN::Parse_attribute_int(const onnx::AttributeProto& a) {
  if (a.has_i() == false) AIR_ASSERT_MSG(false, ("Bad attribute "));
  return a.i();
}

std::vector<int> AIRSTMTGEN::Parse_attribute_ints(
    const onnx::AttributeProto& a) {
  if (a.ints_size() == 0) AIR_ASSERT_MSG(false, "Not a ints attribute");

  std::vector<int> rv;

  for (auto i : a.ints()) rv.push_back(i);

  return rv;
}

std::vector<float> AIRSTMTGEN::Parse_attribute_floats(
    const onnx::AttributeProto& a) {
  if (a.floats_size() == 0) AIR_ASSERT_MSG(false, "Not a floats attribute");

  std::vector<float> rv;

  for (float f : a.floats()) rv.push_back(f);

  return rv;
}

void AIRSTMTGEN::Parse_attributes(onnx::NodeProto* node) {
  auto it = _onx_func_map.find(node->op_type());
  if (it != _onx_func_map.end())
    return (this->*((it->second)._parse_attrs))(node);
  else
    return Unimplemented();
}

void AIRSTMTGEN::Unimplemented() {
  AIR_ASSERT_MSG(false, ("Unsupported operator\n"));
}

void AIRSTMTGEN::Set_attribute_float(std::string attr_name, NODE_PTR node) {
  if (_attributes_for_float.find(attr_name) != _attributes_for_float.end()) {
    float attr_val = _attributes_for_float[attr_name];
    node->Set_attr(attr_name.c_str(), &attr_val, 1);
  }
}
void AIRSTMTGEN::Set_attribute_floats(std::string attr_name, NODE_PTR node) {
  if (_attributes_for_floats.find(attr_name) != _attributes_for_floats.end()) {
    std::vector<float> attr_vals = _attributes_for_floats[attr_name];
    node->Set_attr(attr_name.c_str(), attr_vals.data(), attr_vals.size());
  }
}

int AIRSTMTGEN::Get_attribute_int(std::string attr_name) {
  if (_attributes_for_int.find(attr_name) != _attributes_for_int.end()) {
    return _attributes_for_int[attr_name];
  }
  return 0;
}

std::vector<int> AIRSTMTGEN::Get_attribute_ints(std::string attr_name) {
  if (_attributes_for_ints.find(attr_name) != _attributes_for_ints.end()) {
    return _attributes_for_ints[attr_name];
  }
  std::vector<int> res;
  return res;
}
float AIRSTMTGEN::Get_attribute_float(std::string attr_name) {
  if (_attributes_for_float.find(attr_name) != _attributes_for_float.end()) {
    return _attributes_for_float[attr_name];
  }
  return 0.0;
}

std::vector<float> AIRSTMTGEN::Get_attribute_floats(std::string attr_name) {
  if (_attributes_for_floats.find(attr_name) != _attributes_for_floats.end()) {
    return _attributes_for_floats[attr_name];
  }
  std::vector<float> res;
  return res;
}

void AIRSTMTGEN::Set_attribute_int(std::string attr_name, NODE_PTR node) {
  if (_attributes_for_int.find(attr_name) != _attributes_for_int.end()) {
    int attr_val = _attributes_for_int[attr_name];
    node->Set_attr(attr_name.c_str(), &attr_val, 1);
  }
}

void AIRSTMTGEN::Set_attribute_ints(std::string attr_name, NODE_PTR node) {
  if (_attributes_for_ints.find(attr_name) != _attributes_for_ints.end()) {
    std::vector<int> attr_vals = _attributes_for_ints[attr_name];
    node->Set_attr(attr_name.c_str(), attr_vals.data(), attr_vals.size());
  }
}

void AIRSTMTGEN::Update_attributes(onnx::NodeProto* onnx_node, NODE_PTR node) {
  auto it = _onx_func_map.find(onnx_node->op_type());
  if (it != _onx_func_map.end())
    return (this->*((it->second)._update_attrs))(node);
  else
    return Unimplemented();
}

bool AIRSTMTGEN::Try_resolve_node(onnx::NodeProto* node,
                                  FUNC_SCOPE*      func_scope) {
  CONTAINER*            cntr = &func_scope->Container();
  std::vector<NODE_PTR> inputs;
  if (_nodes.count(node->name())) return true;

  if (Get_node_input_tensors(node, inputs) == false) return false;

  SPOS        spos     = Get_airgen()->Get_glob()->Unknown_simple_spos();
  std::string new_node = node->op_type();

  _attributes_for_tensor.clear();
  _attributes_for_int.clear();
  _attributes_for_float.clear();
  Parse_attributes(node);

  NODE_PTR node_op = Create_node(new_node, inputs, func_scope);
  if (META_INFO::Has_prop<OPR_PROP::ATTR>(node_op->Opcode()) &&
      node->has_name()) {
    node_op->Set_attr("name", node->name().c_str());
  }

  std::vector<bool> output_used;

  for (int nn = 0; nn < node->output_size(); nn++) {
    if (node->output(nn) == "") {
      output_used.push_back(false);
    } else {
      output_used.push_back(true);
    }
  }

  TYPE_PTR         base_ty;
  std::vector<int> result_dim;
  Resolve(node, inputs, base_ty, result_dim);
  std::string onnx_name;
  for (uint32_t o = 0; o < output_used.size(); o++) {
    if (output_used[o])
      onnx_name = node->output(o);
    else
      onnx_name = "";
  }
  NAME_MAP out_st_ptr = Get_airgen()->Sg().Get_tensor_sym_or_preg(
      onnx_name, base_ty, result_dim, func_scope);
  AIR_ASSERT_MSG(out_st_ptr.Is_preg() || out_st_ptr.Is_sym(),
                 ("Expect output to be preg or symbol.\n"));
  if (node_op->Opcode() == air::core::OPC_LDC && onnx_name != "" &&
      out_st_ptr.Is_preg()) {
    Get_airgen()->Sg().Put_operator_cst(onnx_name, node_op->Const());
  } else {
    STMT_PTR stmt;
    if (out_st_ptr.Is_preg())
      stmt = cntr->New_stp(node_op, out_st_ptr.Preg(), spos);
    else
      stmt = cntr->New_st(node_op, out_st_ptr.Sym(), spos);
    cntr->Stmt_list().Append(stmt);
  }

  Update_attributes(node, node_op);
  _nodes.insert(node->op_type());
  return true;
}

void AIRSTMTGEN::Get_dimension_size(TYPE_PTR input_ty, std::vector<int>& dims) {
  ARRAY_TYPE_PTR& input_aty = input_ty->Cast_to_arr();
  DIM_ITER        dim_iter  = input_aty->Begin_dim();
  DIM_ITER        end_iter  = input_aty->End_dim();
  for (; dim_iter != end_iter; ++dim_iter) {
    int dim_len = ((*dim_iter)->Ub_val() - (*dim_iter)->Lb_val()) /
                  (*dim_iter)->Stride_val();
    dims.push_back(dim_len);
  }
}

void AIRSTMTGEN::Resolve(onnx::NodeProto* node, std::vector<NODE_PTR>& inputs,
                         TYPE_PTR& base_ty, std::vector<int>& res) {
  auto it = _onx_func_map.find(node->op_type());
  if (it != _onx_func_map.end())
    return (this->*((it->second)._resolve))(node, inputs, base_ty, res);
  else
    return Unimplemented();
}

std::vector<int> AIRSTMTGEN::Resolve_util(TYPE_PTR elem_ty, TYPE_PTR& base_ty) {
  ARRAY_TYPE_PTR& aty = elem_ty->Cast_to_arr();
  base_ty             = aty->Elem_type();
  std::vector<int> result_dim;
  DIM_ITER         dim_iter = aty->Begin_dim();
  DIM_ITER         end_iter = aty->End_dim();
  for (; dim_iter != end_iter; ++dim_iter) {
    result_dim.push_back((((*dim_iter)->Ub_val() - (*dim_iter)->Lb_val()) /
                          (*dim_iter)->Stride_val()));
  }
  AIR_ASSERT_MSG(result_dim.size() != 0, ("Expect tensor type is resloved.\n"));

  return result_dim;
}

NODE_PTR AIRSTMTGEN::Create_node(std::string            OP_name,
                                 std::vector<NODE_PTR>& input,
                                 FUNC_SCOPE*            func_scope) {
  CONTAINER* cntr = &func_scope->Container();
  SPOS       spos = Get_airgen()->Get_glob()->Unknown_simple_spos();
  NODE_PTR   op_node;
  auto       it = _onx_func_map.find(OP_name);
  if (it != _onx_func_map.end())
    (this->*((it->second)._create_node))(cntr, input, spos, op_node);
  else
    Unimplemented();
  return op_node;
}

void AIRSTMTGEN::Create_node_for_add(CONTAINER*             cntr,
                                     std::vector<NODE_PTR>& input,
                                     const SPOS& spos, NODE_PTR& op_node) {
  op_node = cntr->New_bin_arith(
      air::base::OPCODE(nn::core::NN, nn::core::OPCODE::ADD), input[0],
      input[1], spos);
}

void AIRSTMTGEN::Parse_attributes_for_add(onnx::NodeProto* node) {}

void AIRSTMTGEN::Resolve_for_add(onnx::NodeProto*       node,
                                 std::vector<NODE_PTR>& inputs,
                                 TYPE_PTR& base_ty, std::vector<int>& res) {
  TYPE_PTR elem_ty = inputs[0]->Rtype();
  res              = Resolve_util(elem_ty, base_ty);
}

void AIRSTMTGEN::Update_attributes_for_add(NODE_PTR node) {}

void AIRSTMTGEN::Create_node_for_mul(CONTAINER*             cntr,
                                     std::vector<NODE_PTR>& input,
                                     const SPOS& spos, NODE_PTR& op_node) {
  op_node = cntr->New_bin_arith(
      air::base::OPCODE(nn::core::NN, nn::core::OPCODE::MUL), input[0],
      input[1], spos);
}

void AIRSTMTGEN::Parse_attributes_for_mul(onnx::NodeProto* node) {}

void AIRSTMTGEN::Resolve_for_mul(onnx::NodeProto*       node,
                                 std::vector<NODE_PTR>& inputs,
                                 TYPE_PTR& base_ty, std::vector<int>& res) {
  TYPE_PTR elem_ty = inputs[0]->Rtype();
  res              = Resolve_util(elem_ty, base_ty);
}

void AIRSTMTGEN::Update_attributes_for_mul(NODE_PTR node) {}

void AIRSTMTGEN::Create_node_for_constant(CONTAINER*             cntr,
                                          std::vector<NODE_PTR>& input,
                                          const SPOS& spos, NODE_PTR& op_node) {
  op_node = cntr->New_ldc(_attributes_for_tensor["value"], spos);
}

void AIRSTMTGEN::Parse_attributes_for_constant(onnx::NodeProto* node) {
  for (const auto& a : node->attribute()) {
    if (a.name() == "value") {
      _attributes_for_tensor[a.name()] = Parse_attribute_tensor(node, a);
      break;
    }
  }
}

void AIRSTMTGEN::Resolve_for_constant(onnx::NodeProto*       node,
                                      std::vector<NODE_PTR>& inputs,
                                      TYPE_PTR&              base_ty,
                                      std::vector<int>&      res) {
  TYPE_PTR elem_ty = _attributes_for_tensor["value"]->Type();
  res              = Resolve_util(elem_ty, base_ty);
}

void AIRSTMTGEN::Update_attributes_for_constant(NODE_PTR node) {}

void AIRSTMTGEN::Create_node_for_gemm(CONTAINER*             cntr,
                                      std::vector<NODE_PTR>& input,
                                      const SPOS& spos, NODE_PTR& op_node) {
  op_node = cntr->New_tern_arith(
      air::base::OPCODE(nn::core::NN, nn::core::OPCODE::GEMM), input[0],
      input[1], input[2], spos);
}

void AIRSTMTGEN::Parse_attributes_for_gemm(onnx::NodeProto* node) {
  for (const auto& a : node->attribute()) {
    if (a.name() == "alpha")
      _attributes_for_float[a.name()] = Parse_attribute_float(a);
    else if (a.name() == "beta")
      _attributes_for_float[a.name()] = Parse_attribute_float(a);
    else if (a.name() == "transA")
      _attributes_for_int[a.name()] = Parse_attribute_int(a);
    else if (a.name() == "transB")
      _attributes_for_int[a.name()] = Parse_attribute_int(a);
    else
      AIR_ASSERT_MSG(false, ("Unknown attribute.\n"));
  }
}

void AIRSTMTGEN::Resolve_for_gemm(onnx::NodeProto*       node,
                                  std::vector<NODE_PTR>& inputs,
                                  TYPE_PTR&              base_ty,
                                  std::vector<int>&      result_dim) {
  base_ty     = inputs[0]->Rtype()->Cast_to_arr()->Elem_type();
  int trans_a = _attributes_for_int["transA"];
  int trans_b = _attributes_for_int["transB"];

  std::vector<int> dims;

  Get_dimension_size(inputs[0]->Rtype(), dims);
  AIR_ASSERT_MSG(dims.size() == 2, ("Unsupported dimension size in Gemm\n"));

  if (trans_a)
    result_dim.push_back(dims[1]);
  else
    result_dim.push_back(dims[0]);

  dims.clear();
  Get_dimension_size(inputs[1]->Rtype(), dims);
  AIR_ASSERT_MSG(dims.size() == 2, ("Unsupported dimension size in Gemm\n"));
  if (trans_b)
    result_dim.push_back(dims[0]);
  else
    result_dim.push_back(dims[1]);
}

void AIRSTMTGEN::Update_attributes_for_gemm(NODE_PTR node) {
  Set_attribute_float("alpha", node);
  Set_attribute_float("beta", node);
  Set_attribute_int("transA", node);
  Set_attribute_int("transB", node);
}

void AIRSTMTGEN::Create_node_for_flatten(CONTAINER*             cntr,
                                         std::vector<NODE_PTR>& input,
                                         const SPOS& spos, NODE_PTR& op_node) {
  op_node = cntr->New_una_arith(
      air::base::OPCODE(nn::core::NN, nn::core::OPCODE::FLATTEN), input[0],
      spos);
}

void AIRSTMTGEN::Parse_attributes_for_flatten(onnx::NodeProto* node) {
  for (const auto& a : node->attribute()) {
    if (a.name() == "axis")
      _attributes_for_int[a.name()] = Parse_attribute_int(a);
    else
      AIR_ASSERT_MSG(false, ("Unknown attribute.\n"));
  }
}

void AIRSTMTGEN::Resolve_for_flatten(onnx::NodeProto*       node,
                                     std::vector<NODE_PTR>& inputs,
                                     TYPE_PTR&              base_ty,
                                     std::vector<int>&      result_dim) {
  base_ty = inputs[0]->Rtype()->Cast_to_arr()->Elem_type();
  std::vector<int> dims;
  Get_dimension_size(inputs[0]->Rtype(), dims);
  int dim_size = dims.size();

  int axis       = _attributes_for_int["axis"];
  int count_axis = axis;
  if (axis < 0) count_axis = dim_size + axis;

  int dim = 1;
  int i;
  for (i = 0; i < count_axis; i++) {
    dim *= dims[i];
  }
  result_dim.push_back(dim);
  dim = 1;
  for (; i < dim_size; i++) {
    dim *= dims[i];
  }
  result_dim.push_back(dim);
}

void AIRSTMTGEN::Update_attributes_for_flatten(NODE_PTR node) {
  Set_attribute_int("axis", node);
}

void AIRSTMTGEN::Create_node_for_conv(CONTAINER*             cntr,
                                      std::vector<NODE_PTR>& input,
                                      const SPOS& spos, NODE_PTR& op_node) {
  op_node = cntr->New_tern_arith(
      air::base::OPCODE(nn::core::NN, nn::core::OPCODE::CONV), input[0],
      input[1], input[2], spos);
}

void AIRSTMTGEN::Parse_attributes_for_conv(onnx::NodeProto* node) {
  for (const auto& a : node->attribute()) {
    if (a.name() == "dilations")
      _attributes_for_ints[a.name()] = Parse_attribute_ints(a);
    else if (a.name() == "group")
      _attributes_for_int[a.name()] = Parse_attribute_int(a);
    else if (a.name() == "kernel_shape")
      _attributes_for_ints[a.name()] = Parse_attribute_ints(a);
    else if (a.name() == "pads")
      _attributes_for_ints[a.name()] = Parse_attribute_ints(a);
    else if (a.name() == "strides")
      _attributes_for_ints[a.name()] = Parse_attribute_ints(a);
    else
      AIR_ASSERT_MSG(false, ("Unknown attribute.\n"));
  }
}

// TODO. Need to figure out why -2 is necessary.
uint32_t AIRSTMTGEN::Get_num_dim_from_tensor_ty(ARRAY_TYPE_PTR& aty) {
  return aty->Shape().size() - 2;
}

std::vector<int> AIRSTMTGEN::Resolve_strides(ARRAY_TYPE_PTR& aty) {
  uint32_t         num_data_dim = Get_num_dim_from_tensor_ty(aty);
  std::vector<int> strides      = Get_attribute_ints("strides");
  if (strides.empty())
    for (uint32_t i = 0; i < num_data_dim; i++) strides.push_back(1);
  return strides;
}

std::vector<int> AIRSTMTGEN::Resolve_dilations(ARRAY_TYPE_PTR& aty) {
  uint32_t         num_data_dim = Get_num_dim_from_tensor_ty(aty);
  std::vector<int> dilations    = Get_attribute_ints("dilations");
  if (dilations.size() == 0)
    for (uint32_t i = 0; i < num_data_dim; i++) dilations.push_back(1);
  return dilations;
}

std::vector<int> AIRSTMTGEN::Resolve_pads(ARRAY_TYPE_PTR& aty) {
  uint32_t         num_data_dim = Get_num_dim_from_tensor_ty(aty);
  std::vector<int> pads         = Get_attribute_ints("pads");
  if (pads.size() == 0) {
    pads.resize(num_data_dim * 2);
    for (uint32_t i = 0; i < num_data_dim; i++) {
      pads[i]                = 0;
      pads[i + num_data_dim] = 0;
    }
  }
  return pads;
}

std::vector<int> AIRSTMTGEN::Resolve_kernel_shape(ARRAY_TYPE_PTR& aty) {
  uint32_t num_data_dim = aty->Shape().size();
  // if kernel shape is not given, infer from w
  std::vector<int> kernel_shape = Get_attribute_ints("kernel_shape");
  if (kernel_shape.size() == 0) {
    // skip M and C/group dimensions
    std::vector<int64_t> data_dim = aty->Shape();
    for (uint32_t i = 2; i < num_data_dim; i++)
      kernel_shape.push_back(data_dim[i]);
  }
  return kernel_shape;
}

std::vector<int> AIRSTMTGEN::Resolve_conv_output_size(
    ARRAY_TYPE_PTR& xty, ARRAY_TYPE_PTR& wty, std::vector<int>& strides,
    std::vector<int>& dilations, std::vector<int>& pads,
    std::vector<int>& kernel_shape) {
  std::vector<int>     rv;
  uint32_t             num_data_dim = Get_num_dim_from_tensor_ty(xty);
  std::vector<int64_t> x_data_dim   = xty->Shape();
  std::vector<int64_t> w_data_dim   = wty->Shape();
  rv.push_back(x_data_dim[0]);  // batch size
  rv.push_back(w_data_dim[0]);  //"number of feature maps"

  // TODO. auto_pad is set to NOTSET since the option is not passed in.
  std::string auto_pad("NOTSET");
  for (uint32_t xdim = 2; xdim < x_data_dim.size(); xdim++) {
    int      outdim;
    uint32_t dim = xdim - 2;
    // Not sure if the naming is correct. Here
    // kernel: the (number of) weights of the filter
    // filter: the spatial placement of the kernel weights
    // NB: 'dilation==1' is what is used for "no spacing in the filter"
    int filter_size = kernel_shape[dim];
    filter_size += (kernel_shape[dim] - 1) * (dilations[dim] - 1);

    // From ONNX Operators.md:
    // SAME_UPPER or SAME_LOWER mean pad the input so that the output spatial
    // size match the input. "match" here means "is equal".
    if (auto_pad == "SAME_UPPER" || auto_pad == "SAME_LOWER")
      outdim = x_data_dim[xdim];
    else if (auto_pad == "NOTSET" || auto_pad == "VALID") {
      // padded input
      uint32_t input_size =
          x_data_dim[xdim] + pads[dim] + pads[dim + num_data_dim];
      // [ 0 1 2 3 4 5 6 7 8 9  ]
      //                |kern=3|
      // last output=7
      uint32_t last_out = input_size - filter_size;
      outdim            = last_out / strides[dim] + 1;
    }

    rv.push_back(outdim);
  }

  return rv;
}
std::vector<int> AIRSTMTGEN::Resolve_pooling_output_size(
    ARRAY_TYPE_PTR& xty, std::vector<int>& strides, std::vector<int>& dilations,
    std::vector<int>& pads, std::vector<int>& kernel_shape) {
  std::vector<int>     rv;
  std::vector<int64_t> x_data_dim = xty->Shape();
  rv.push_back(x_data_dim[0]);  // batch
  rv.push_back(x_data_dim[1]);  // channel

  uint32_t         data_dims = Get_num_dim_from_tensor_ty(xty);
  std::vector<int> pad_shapes;
  for (uint32_t i = 0; i < data_dims; i++) {
    pad_shapes.push_back(pads[i] + pads[data_dims + i]);
  }
  // Calculate output shape. Pads are now calculated
  // for those auto_pad modes that need them.
  int32_t ceil_mode = Get_attribute_int("ceil_mode");
  for (uint32_t i = 2; i < x_data_dim.size(); i++) {
    int32_t d;
    int32_t in_dim   = x_data_dim[i];
    int32_t kernel   = kernel_shape[i - 2];
    int32_t dilation = dilations.size() == 0 ? 1 : dilations[i - 2];
    int32_t stride   = strides[i - 2];
    // auto_pad is specified by the compiler option. At present we don't
    // implement this. I assume auto_pad is "NOTSET" by default as work around.
    // if ( auto_pad == "NOTSET" ) {
    int32_t pad_sh = pad_shapes[i - 2];
    if (ceil_mode)
      d = ceil((float)(in_dim + pad_sh - ((kernel - 1) * dilation + 1)) /
                   stride +
               1);
    else
      d = floor((float)(in_dim + pad_sh - ((kernel - 1) * dilation + 1)) /
                    stride +
                1);
      //}
#if 0
                        else if( auto_pad == "VALID" )
                                d = ceil((float)( in_dim - ((kernel - 1) *dilation + 1) +1) /  stride );

                        else // auto_pad == "SAME_UPPER" || auto_pad == "SAME_LOWER"
                                d = ceil( (float)in_dim / stride );
#endif
    rv.push_back(d);
  }
  return rv;
}

void AIRSTMTGEN::Resolve_for_conv(onnx::NodeProto*       node,
                                  std::vector<NODE_PTR>& inputs,
                                  TYPE_PTR&              base_ty,
                                  std::vector<int>&      result_dim) {
  NODE_PTR x = inputs[0];
  NODE_PTR w = inputs[1];

  AIR_ASSERT_MSG(x->Rtype()->Is_array(), "Expect tensor type");
  std::vector<int> strides   = Resolve_strides(x->Rtype()->Cast_to_arr());
  std::vector<int> dilations = Resolve_dilations(x->Rtype()->Cast_to_arr());
  std::vector<int> pads      = Resolve_pads(x->Rtype()->Cast_to_arr());
  AIR_ASSERT_MSG(w->Rtype()->Is_array(), "Expect tensor type");
  std::vector<int> kernel_shape =
      Resolve_kernel_shape(w->Rtype()->Cast_to_arr());
  result_dim = Resolve_conv_output_size(x->Rtype()->Cast_to_arr(),
                                        w->Rtype()->Cast_to_arr(), strides,
                                        dilations, pads, kernel_shape);
  base_ty    = x->Rtype()->Cast_to_arr()->Elem_type();
}

void AIRSTMTGEN::Update_attributes_for_conv(NODE_PTR node) {
  Set_attribute_ints("dilations", node);
  Set_attribute_int("group", node);
  Set_attribute_ints("kernel_shape", node);
  Set_attribute_ints("pads", node);
  Set_attribute_ints("strides", node);
}

void AIRSTMTGEN::Create_node_for_relu(CONTAINER*             cntr,
                                      std::vector<NODE_PTR>& input,
                                      const SPOS& spos, NODE_PTR& op_node) {
  op_node = cntr->New_una_arith(
      air::base::OPCODE(nn::core::NN, nn::core::OPCODE::RELU), input[0], spos);
}
void AIRSTMTGEN::Parse_attributes_for_relu(onnx::NodeProto* node) {}
void AIRSTMTGEN::Resolve_for_relu(onnx::NodeProto*       node,
                                  std::vector<NODE_PTR>& inputs,
                                  TYPE_PTR&              base_ty,
                                  std::vector<int>&      result_dim) {
  TYPE_PTR elem_ty = inputs[0]->Rtype();
  result_dim       = Resolve_util(elem_ty, base_ty);
}

void AIRSTMTGEN::Update_attributes_for_relu(NODE_PTR node) {}

void AIRSTMTGEN::Create_node_for_max_pool(CONTAINER*             cntr,
                                          std::vector<NODE_PTR>& input,
                                          const SPOS& spos, NODE_PTR& op_node) {
  op_node = cntr->New_una_arith(
      air::base::OPCODE(nn::core::NN, nn::core::OPCODE::MAX_POOL), input[0],
      spos);
}
void AIRSTMTGEN::Parse_attributes_for_max_pool(onnx::NodeProto* node) {
  for (const auto& a : node->attribute()) {
    if (a.name() == "ceil_mode" || a.name() == "count_include_pad")
      _attributes_for_int[a.name()] = Parse_attribute_int(a);
    else if (a.name() == "kernel_shape")
      _attributes_for_ints[a.name()] = Parse_attribute_ints(a);
    else if (a.name() == "pads")
      _attributes_for_ints[a.name()] = Parse_attribute_ints(a);
    else if (a.name() == "strides")
      _attributes_for_ints[a.name()] = Parse_attribute_ints(a);
    else
      AIR_ASSERT_MSG(false, ("Unknown attribute.\n"));
  }
}
void AIRSTMTGEN::Resolve_for_max_pool(onnx::NodeProto*       node,
                                      std::vector<NODE_PTR>& inputs,
                                      TYPE_PTR&              base_ty,
                                      std::vector<int>&      result_dim) {
  NODE_PTR x = inputs[0];

  AIR_ASSERT_MSG(x->Rtype()->Is_array(), "Expect tensor type");
  std::vector<int> strides   = Resolve_strides(x->Rtype()->Cast_to_arr());
  std::vector<int> dilations = Resolve_dilations(x->Rtype()->Cast_to_arr());
  std::vector<int> pads      = Resolve_pads(x->Rtype()->Cast_to_arr());
  std::vector<int> kernel_shape =
      Resolve_kernel_shape(x->Rtype()->Cast_to_arr());
  result_dim = Resolve_pooling_output_size(x->Rtype()->Cast_to_arr(), strides,
                                           dilations, pads, kernel_shape);
  base_ty    = x->Rtype()->Cast_to_arr()->Elem_type();
  // TODO
  // The auto_pad mess:
  // pads are needed to calculate output shape, but output shape is needed to
  // calculate pads Run this after resolve_output_size() to patch up
  // Update_pads();
}

void AIRSTMTGEN::Update_attributes_for_max_pool(NODE_PTR node) {
  Set_attribute_int("ceil_mode", node);
  Set_attribute_ints("kernel_shape", node);
  Set_attribute_ints("pads", node);
  Set_attribute_ints("strides", node);
}

void AIRSTMTGEN::Create_node_for_average_pool(CONTAINER*             cntr,
                                              std::vector<NODE_PTR>& input,
                                              const SPOS&            spos,
                                              NODE_PTR&              op_node) {
  op_node = cntr->New_una_arith(
      air::base::OPCODE(nn::core::NN, nn::core::OPCODE::AVERAGE_POOL), input[0],
      spos);
}

void AIRSTMTGEN::Parse_attributes_for_average_pool(onnx::NodeProto* node) {
  Parse_attributes_for_max_pool(node);
}

void AIRSTMTGEN::Resolve_for_average_pool(onnx::NodeProto*       node,
                                          std::vector<NODE_PTR>& inputs,
                                          TYPE_PTR&              base_ty,
                                          std::vector<int>&      result_dim) {
  Resolve_for_max_pool(node, inputs, base_ty, result_dim);
}

void AIRSTMTGEN::Update_attributes_for_average_pool(NODE_PTR node) {
  Update_attributes_for_max_pool(node);
}

void AIRSTMTGEN::Create_node_for_global_average_pool(
    CONTAINER* cntr, std::vector<NODE_PTR>& input, const SPOS& spos,
    NODE_PTR& op_node) {
  op_node = cntr->New_una_arith(
      air::base::OPCODE(nn::core::NN, nn::core::OPCODE::GLOBAL_AVERAGE_POOL),
      input[0], spos);
}
void AIRSTMTGEN::Parse_attributes_for_global_average_pool(
    onnx::NodeProto* node) {}
void AIRSTMTGEN::Resolve_for_global_average_pool(onnx::NodeProto*       node,
                                                 std::vector<NODE_PTR>& inputs,
                                                 TYPE_PTR&              base_ty,
                                                 std::vector<int>& result_dim) {
  NODE_PTR             x          = inputs[0];
  ARRAY_TYPE_PTR       xty        = x->Rtype()->Cast_to_arr();
  std::vector<int64_t> x_data_dim = xty->Shape();
  AIR_ASSERT_MSG(x_data_dim.size() > 1,
                 "Expect tensor with more than 1 dimension.");
  result_dim.push_back(x_data_dim[0]);
  result_dim.push_back(x_data_dim[1]);
  for (uint32_t i = 2; i < x_data_dim.size(); i++) {
    result_dim.push_back(1);
  }
  base_ty = xty->Elem_type();
}
void AIRSTMTGEN::Update_attributes_for_global_average_pool(NODE_PTR node) {}

void AIRSTMTGEN::Create_node_for_reshape(CONTAINER*             cntr,
                                         std::vector<NODE_PTR>& input,
                                         const SPOS& spos, NODE_PTR& op_node) {
  op_node = cntr->New_bin_arith(
      air::base::OPCODE(nn::core::NN, nn::core::OPCODE::RESHAPE), input[0],
      input[1], spos);
}
void AIRSTMTGEN::Parse_attributes_for_reshape(onnx::NodeProto* node) {
  // TODO for attribute allowzero
}
void AIRSTMTGEN::Resolve_for_reshape(onnx::NodeProto*       node,
                                     std::vector<NODE_PTR>& inputs,
                                     TYPE_PTR&              base_ty,
                                     std::vector<int>&      result_dim) {
  NODE_PTR data = inputs[0];
  AIR_ASSERT_MSG(data->Rtype()->Is_array(), "Expect tensor type");
  ARRAY_TYPE_PTR       xty             = data->Rtype()->Cast_to_arr();
  std::vector<int64_t> x_data_dim      = xty->Shape();
  int64_t              x_data_dim_size = 1;
  for (auto e : x_data_dim) {
    x_data_dim_size *= e;
  }
  NODE_PTR shape = inputs[1];
  AIR_ASSERT_MSG(shape->Opcode() == air::core::OPC_LDC, "Expect ldc");
  CONSTANT_PTR const_ptr     = shape->Const();
  size_t       data_num_elem = const_ptr->Array_byte_len() / sizeof(int64_t);
  bool         negative_shape_found = false;
  int          negative_shape_at    = -1;
  uint64_t     output_size          = 1;
  for (uint32_t i = 0; i < data_num_elem; i++) {
    int64_t s = const_ptr->Array_elem<int64_t>(i);
    if (s < 0) {
      negative_shape_found = true;
      negative_shape_at    = i;
    } else if (s == 0) {
      AIR_ASSERT_MSG(i < x_data_dim.size(),
                     "Bad input: reshape request duplication of input "
                     "dimension that doesn't exit");
      s = x_data_dim[i];
    }
    result_dim.push_back(s);
    if (s > 0) {
      output_size *= s;
    }
    if (negative_shape_found) {
      int64_t missing_dim           = x_data_dim_size / output_size;
      result_dim[negative_shape_at] = missing_dim;
    }
  }
  base_ty = xty->Elem_type();
}
void AIRSTMTGEN::Update_attributes_for_reshape(NODE_PTR node) {}

}  // namespace onnx2air
}  // namespace nn
