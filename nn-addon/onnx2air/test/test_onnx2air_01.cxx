//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================
#include "air/base/container.h"
#include "nn/core/opcode.h"
#include "nn/onnx2air/air_gen.h"
#include "onnx.pb.h"

using namespace air::base;
using namespace air::core;
using namespace air::util;
using namespace onnx;

#define ONNX_OPSET_VERSION 11

int Test(onnx::ModelProto& onnx_model) {
  GLOB_SCOPE*          glob = GLOB_SCOPE::Get();
  nn::onnx2air::AIRGEN air_gen(glob);
  air_gen.Process_graph(onnx_model);
  std::cout << "Tensor IR:" << std::endl;
  glob->Print();
  return 0;
}

int Test_translation(const char* fnname) {
  ModelProto model_proto;
  model_proto.set_ir_version(7);
  auto* opset_version = model_proto.add_opset_import();
  opset_version->set_version(ONNX_OPSET_VERSION);

  auto* graph = model_proto.mutable_graph();

  auto elt_type = TensorProto_DataType::TensorProto_DataType_FLOAT;

  auto* x = graph->add_input();
  x->set_name("x");
  auto* x_type = x->mutable_type()->mutable_tensor_type();
  x_type->set_elem_type(elt_type);
  auto* x_shape = x_type->mutable_shape();
  x_shape->add_dim()->set_dim_value(10);

  auto* y = graph->add_output();
  y->set_name("y");
  auto* y_type = y->mutable_type()->mutable_tensor_type();
  y_type->set_elem_type(elt_type);
  auto* y_shape = y_type->mutable_shape();
  y_shape->add_dim()->set_dim_value(10);

  auto* node = graph->add_node();
  node->add_input("x");
  node->add_output("y");
  node->set_op_type(fnname);
  node->set_name(fnname);
  return Test(model_proto);
}
int main() {
  bool ret = air::core::Register_core();
  AIR_ASSERT_MSG(ret, "Register core domain failed");
  ret = nn::core::Register_nn();
  AIR_ASSERT_MSG(ret, "Register nn domain failed");

  Test_translation("Flatten");
}
