//-*-c++-*-
//=============================================================================
//
// Copyright (c) Ant Group Co., Ltd
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//=============================================================================

#ifndef AIR_SYM_H
#define AIR_SYM_H

#include "nn/onnx2air/name_map.h"
#include "nn/onnx2air/onnx2air_decl.h"
#include "onnx.pb.h"

namespace nn {
namespace onnx2air {

typedef struct ONE_NAME_TYPE_PAIR {
  std::string _name;
  TYPE_PTR    _ty_ptr;
} NAME_TYPE_PAIR;

class AIRGEN;
/**
 * @brief Interface of generating the symbol for tensor.
 *
 */
class AIRSYMGEN {
public:
  AIRSYMGEN(AIRGEN* airgen) : _airgen(airgen) {}

  ~AIRSYMGEN() {}

  void         Init() {}
  FUNC_SCOPE*  Convert_func_sym(onnx::GraphProto& onnx_graph);
  FUNC_SCOPE*  Create_function(const char*                  func_name,
                               std::vector<NAME_TYPE_PAIR>& param_node,
                               std::vector<NAME_TYPE_PAIR>& ret_node);
  CONSTANT_PTR Convert_init_sym(const onnx::TensorProto& tensor);

  NAME_MAP Get_result(const std::string name);
  void     Put_result(const std::string name, const NAME_MAP& res);

  PREG_PTR Get_preg(const std::string name);
  void     Put_preg(const std::string name, PREG_PTR preg);

  /**
   * @brief Function Get_st returns the correponding symbol based on given name.
   *
   */
  ADDR_DATUM_PTR Get_st(const std::string name);
  /**
   * @brief Function Put_st builds a map table between the name and the symbol.
   *
   */
  void Put_st(const std::string name, ADDR_DATUM_PTR sym_ptr);

  CONSTANT_PTR Get_cst(const std::string name);
  void         Put_cst(const std::string name, CONSTANT_PTR cst);

  CONSTANT_PTR Get_operator_cst(const std::string name);
  void         Put_operator_cst(const std::string name, CONSTANT_PTR cst);

  ADDR_DATUM_PTR Generate_sym(std::string sym_name, TYPE_PTR ty_ptr,
                              FUNC_SCOPE* func_scope);
  NAME_MAP       Get_sym_or_preg(std::string sym_name, TYPE_PTR ty_ptr,
                                 FUNC_SCOPE* func_scope);
  NAME_MAP       Get_tensor_sym_or_preg(std::string sym_name, TYPE_PTR base_ty,
                                        std::vector<int>& dim,
                                        FUNC_SCOPE*       func_scope);
  std::vector<ADDR_DATUM_PTR>& Input_vars() { return _input_sts; }
  std::vector<ADDR_DATUM_PTR>& Output_vars() { return _output_sts; }
  AIRGEN*                      Get_airgen() { return _airgen; }

private:
  AIRSYMGEN(void);              // REQUIRED UNDEFINED UNWANTED methods
  AIRSYMGEN(const AIRSYMGEN&);  // REQUIRED UNDEFINED UNWANTED methods
  AIRSYMGEN& operator=(
      const AIRSYMGEN&);  // REQUIRED UNDEFINED UNWANTED methods

  AIRGEN*                                         _airgen;
  std::unordered_map<std::string, ADDR_DATUM_PTR> _st_map;
  std::unordered_map<std::string, PREG_PTR>       _preg_map;
  std::unordered_map<std::string, CONSTANT_PTR>   _cst_map;

  // the map between the output tensor name and the onnx constant operator
  std::unordered_map<std::string, CONSTANT_PTR> _onx_const_map;
  std::vector<ADDR_DATUM_PTR>                   _input_sts;
  std::vector<ADDR_DATUM_PTR>                   _output_sts;
};
}  // namespace onnx2air
}  // namespace nn
#endif
